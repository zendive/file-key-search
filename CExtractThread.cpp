#include "stdafx.h"
#include "CExtractThread.h"
#include "CApp.h"

IMPLEMENT_DYNCREATE(CExtractThread, CWinThread)

BEGIN_MESSAGE_MAP(CExtractThread, CWinThread)
END_MESSAGE_MAP()

CExtractThread::CExtractThread()
{
  m_pKeyList = NULL;
  m_bDone = false;
  m_bAbort = false;
  m_bPaused = false;
  m_ErrState = CExtractThread::ERR_ALRIGHT;
  m_peTotal.Reset();
  m_sTargetFileName = _T("");
  m_sExtractDir = _T("");
  m_uliTotalBytesReaden.QuadPart = 0;
}

CExtractThread::~CExtractThread()
{
}

BOOL CExtractThread::InitInstance()
{
  if (!m_bAbort)
  {
    this->PostThreadMessage(nPRG_WM_EXTRACTTHREAD_START, 0L, 0L);
  }

	return TRUE;
}

int CExtractThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BOOL CExtractThread::PreTranslateMessage(MSG* pMsg)
{
  if (nPRG_WM_EXTRACTTHREAD_START == pMsg->message)
  {
    this->Start();    // workload
    ::ExitThread(0);  // all done
    return TRUE;      // processed
  }

  return CWinThread::PreTranslateMessage(pMsg);
}

DWORD CExtractThread::GetBPMS()
{
  DWORD dwElapsed = m_peTotal.GetCurTime();
  return (DWORD)(0 == dwElapsed? 0 : m_uliTotalBytesReaden.QuadPart / dwElapsed);
}

void CExtractThread::Start()
{
  ULARGE_INTEGER uliSeek;
  bool bRetry = false;    // retry media extraction
  char* pBuffer = NULL;   // extraction buffer
  const DWORD dwBufferSize = nPRG_EXTRACT_BUFFER_SIZE;
  DWORD dwBytesToRead, dwBytesReaden, dwBytesToWrite, dwBytesWritten;
  BOOL bReadFile;
  ULARGE_INTEGER uliTotalBytesReaden;
  HANDLE hMediaFile;
  INT_PTR ipMediaCount;
  POSITION pos;
  CMedia* pMedia = NULL;
  CString sMediaDir;

  HANDLE hTargetFile = CreateFile(m_sTargetFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING
    , FILE_ATTRIBUTE_READONLY | FILE_FLAG_RANDOM_ACCESS, NULL);

  if (INVALID_HANDLE_VALUE == hTargetFile || NULL == hTargetFile)
  {
    m_ErrState = CExtractThread::ERR_DEADFILE;
    m_bDone = true;
    return;
  }

  pBuffer = (char*) VirtualAlloc(NULL, dwBufferSize, MEM_COMMIT, PAGE_READWRITE);

  if (NULL == pBuffer)
  {
    m_ErrState = CExtractThread::ERR_MEMOVERLOAD;
    m_bDone = true;
    return;
  }

  ::CreateDirectory(m_sExtractDir, NULL);

  m_peTotal.Start();

  for (CKey* pKey = m_pKeyList->GetRoot()
    ; pKey != NULL && NULL != hTargetFile
    ; pKey = pKey->Next())
  {
    CONTINUEIF(NULL == pKey);

    ipMediaCount = pKey->m_MediaSet.GetCount();
    CONTINUEIF(0 == ipMediaCount);

    pos = pKey->m_MediaSet.GetHeadPosition();
    pMedia = NULL;

    sMediaDir.Format(_T("%s\\%s"), m_sExtractDir, pKey->m_sName);
    ::CreateDirectory(sMediaDir, NULL);

    for (INT_PTR c = 0; c < ipMediaCount && !m_bAbort; c++)
    {
      if (bRetry)
      { // keep `pMedia` the same
        bRetry = false;
      }
      else
      {
        pMedia = pKey->m_MediaSet.GetNext(pos);
        CONTINUEIF(NULL == pMedia);
        m_sMediaFileName.Format(_T("%s\\%s"), sMediaDir, pMedia->m_sName);
      }

      hMediaFile = ::CreateFile(m_sMediaFileName, GENERIC_WRITE
        , NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

      if (INVALID_HANDLE_VALUE == hMediaFile || NULL == hMediaFile)
      { // afford Abort/Retry/Ignore media extract
        hMediaFile = NULL;
        m_ErrState = CExtractThread::ERR_DEADFILE;

        CString sMessage;
        sMessage.Format(_T("Could not handle extract for media:\r\n%s"), m_sMediaFileName);

        int iRet = ::MessageBox(theApp.m_pMainWnd->m_hWnd, sMessage
          , _T(" Extraction failure"), MB_ICONQUESTION | MB_ABORTRETRYIGNORE);

        switch (iRet)
        {
          case IDABORT: m_bAbort = true; break;
          case IDRETRY: bRetry = true; --c; break;
          case IDIGNORE: continue; break;
          default: __assume(0);
        }

        BREAKIF(m_bAbort);
        CONTINUEIF(bRetry);
      }

      // set target position
      uliSeek = pMedia->m_qwOffset;
      uliSeek.LowPart = ::SetFilePointer(hTargetFile, uliSeek.LowPart,
        (LONG*) &uliSeek.HighPart, FILE_BEGIN);

      uliTotalBytesReaden.QuadPart = 0;

      do
      {
        if (m_bPaused)
        { // on pause enter sleeping loop
          m_peTotal.PauseON();
          while (m_bPaused && !m_bAbort)
          {
            ::Sleep(nPRG_THREAD_WHILE_ONPAUSE_SLEEP_MS);
          }
          m_peTotal.PauseOFF();
        }

        if ((pMedia->m_qwSize.QuadPart - uliTotalBytesReaden.QuadPart) < dwBufferSize)
        { // if last amount of bytes fits into `dwBufferSize` than copy it all
          dwBytesToRead = (DWORD)(pMedia->m_qwSize.QuadPart - uliTotalBytesReaden.QuadPart);
        }
        else
        { // if last ammount of bytes ecceeds `dwBufferSize` than copy by portions
          dwBytesToRead = dwBufferSize;
        }

        bReadFile = ::ReadFile(hTargetFile, pBuffer, dwBytesToRead, &dwBytesReaden, NULL);
        if (FALSE == bReadFile)
        { // can not continue if can not read from target file
          CString sReport;
          sReport.Format(_T("Extract was tried to read from target file and failed.")
            _T("\nRequested bytes to read: %s\nBytes readen: %s")
            , CConvert::NumberToHumanString(dwBytesToRead)
            , CConvert::NumberToHumanString(dwBytesReaden));
          theApp.Report(sReport);
          m_ErrState = CExtractThread::ERR_DEADFILE;
          m_bAbort = true;
          break;
        }

        dwBytesToWrite = dwBytesReaden;
        uliTotalBytesReaden.QuadPart += dwBytesReaden;
        m_uliTotalBytesReaden.QuadPart += dwBytesReaden;

        ::WriteFile(hMediaFile, pBuffer, dwBytesToWrite, &dwBytesWritten, NULL);
      }
      while (uliTotalBytesReaden.QuadPart < pMedia->m_qwSize.QuadPart
        && !m_bAbort);

      CloseHandle(hMediaFile);
    }//@pMedia

    BREAKIF(m_bAbort);
  }//@pKey

  CloseHandle(hTargetFile);
  VirtualFree(pBuffer, 0, MEM_RELEASE);

  m_bDone = true;
  m_peTotal.Stop();
}
