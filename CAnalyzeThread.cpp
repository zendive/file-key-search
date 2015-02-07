#include "stdafx.h"
#include "CAnalyzeThread.h"
#include "CApp.h"

IMPLEMENT_DYNCREATE(CAnalyzeThread, CWinThread);

BEGIN_MESSAGE_MAP(CAnalyzeThread, CWinThread)
END_MESSAGE_MAP()

CAnalyzeThread::CAnalyzeThread()
{
  m_bDone = false;
  m_bAbort = false;
  m_bPaused = false;
  
  m_pKey = NULL;
  m_dwProgress = 0;
  m_dwFound = 0;
  m_ErrState = CAnalyzeThread::ERR_ALRIGHT;
  m_qwFileSize.QuadPart = 0;
  m_qwBufOffset.QuadPart = 0;
  m_qwBTRLeft.QuadPart = 0;
  m_qwTotalBR.QuadPart = 0;
  m_peTotal.Reset();

  m_dwBufferAllocationSize = nPRG_ANALYZE_MEMORY_LOW;
  ::InitializeCriticalSection(&m_csBufferAllocationSize);
}

CAnalyzeThread::~CAnalyzeThread()
{
  ::DeleteCriticalSection(&m_csBufferAllocationSize);
}

int CAnalyzeThread::ExitInstance()
{
  return CWinThread::ExitInstance();
}

BOOL CAnalyzeThread::InitInstance()
{
  if (!m_bAbort)
  {
    this->PostThreadMessage(nPRG_WM_ANALYZETHREAD_START, 0L, 0L);
  }

  return TRUE;
}

BOOL CAnalyzeThread::PreTranslateMessage(MSG* pMsg)
{
  if (nPRG_WM_ANALYZETHREAD_START == pMsg->message)
  {
    this->Start();    // workload
    ::ExitThread(0);  // all done
    return TRUE;      // processed
  }

  return CWinThread::PreTranslateMessage(pMsg);
}

DWORD CAnalyzeThread::GetBPMS()
{
  DWORD dwElapsed = m_peTotal.GetCurTime();
  return (DWORD)(0 == dwElapsed? 0 : m_qwTotalBR.QuadPart / dwElapsed);
}

HANDLE CAnalyzeThread::PrepeareFile()
{
  HANDLE hFile = CreateFile(m_sFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING
    , FILE_ATTRIBUTE_READONLY | FILE_FLAG_RANDOM_ACCESS, NULL);
  if ((INVALID_HANDLE_VALUE == hFile) || (0 == hFile))
  {
    return 0;
  }

  // read file size
  DWORD dwHighPart;
  m_qwFileSize.LowPart = ::GetFileSize(hFile, &dwHighPart);
  m_qwFileSize.HighPart = dwHighPart;

  return hFile;
}

void CAnalyzeThread::Start()
{
  RETURNIF(m_bAbort);
  m_pKey->DeleteMediaSet();

  HANDLE hFile = NULL;          // file handle
  DWORD dwBTR = 0;              // bytes to read
  DWORD dwBRN = 0;              // bytes readen
  char *pBuf = NULL;            // file-read buffer
  DWORD dwBufSize = 0;          // buffer size
  ULARGE_INTEGER qwMinBufSize;   // minimal buffer size
  signed __int64 i64Pos;        // signed position to move/last_item_detected
  ULARGE_INTEGER qwSeek;         // seek position
  BOOL bReadFile;
  DWORD dwSISS;                 // search inner self-state
  double dPM = 1;               // %5th parts multiplier

  m_peTotal.Start();

  if (m_pKey->m_dwCSO)
  { // CS offset to size (+1) (note that content size offset is after the signature)
    // + size of CSO placeholder
    // + one byte of virtual data
    qwMinBufSize.QuadPart = m_pKey->m_dwCSO+1 + 4 + 1;
  }
  else
  { // signature size
    // + one byte of virtual data
    qwMinBufSize.QuadPart = m_pKey->m_dwSignSize + 1;
  }

  // open shared source file for reading
  hFile = PrepeareFile();
  if (0 == hFile)
  {
    m_ErrState = CAnalyzeThread::ERR_DEADFILE;
    m_bDone = true;
    m_dwProgress = 100;
    m_peTotal.Stop();

    PostQuitMessage(0);
    return;
  }

  m_qwBTRLeft.QuadPart = m_qwFileSize.QuadPart;

  // progress update point is every 2.5% from total file size
  double chkpnt = ((double)m_qwFileSize.QuadPart * 0.025);

  ::EnterCriticalSection(&m_csBufferAllocationSize);
  dwBufSize = m_dwBufferAllocationSize;
  pBuf = (char*) VirtualAlloc(NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
  ::LeaveCriticalSection(&m_csBufferAllocationSize);

  // SCAN THROUTH THE FILE PORTIONS //
  // scan till the bytes_to_read_left is bigger than key_sign_size
  while (!m_bAbort && (m_qwBTRLeft.QuadPart > qwMinBufSize.QuadPart))
  {
    if (m_bPaused)
    { // On Pause
      m_peTotal.PauseON();

      while (m_bPaused && !m_bAbort) // enter sleepping loop
      {
        ::Sleep(nPRG_THREAD_WHILE_ONPAUSE_SLEEP_MS);
      }

      m_peTotal.PauseOFF();
    }

    BREAKIF(m_bAbort);

    /// READ PORTION FROM FILE ///

    ::EnterCriticalSection(&m_csBufferAllocationSize);
    if (dwBufSize != m_dwBufferAllocationSize && NULL != pBuf)
    {
      dwBufSize = m_dwBufferAllocationSize;
      VirtualFree(pBuf, 0, MEM_RELEASE);
      pBuf = (char*) VirtualAlloc(NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
    }
    ::LeaveCriticalSection(&m_csBufferAllocationSize);

    if (NULL == pBuf)
    { // number of tries for successfull buffer allocation before rising an error
      DWORD dwTry = nPRG_ANALTHREAD_RETRYONMALLOCFAIL_TRIES_NUMBER;
      do
      { // if failed - sleep a while... and then try with minimal buf size...
        ::Sleep(nPRG_ANALTHREAD_RETRYONMALLOCFAIL_SLEEP_MS);

        dwBufSize = nPRG_ANALYZE_MEMORY_SOS;
        pBuf = (char*) malloc(dwBufSize * sizeof(char));
        if (NULL != pBuf)
        { // ok now
          break;
        }
      } while (--dwTry != 0);

      if (0 == dwTry && NULL == pBuf)
      { // if tries count elapsed - break the loop
        m_ErrState = CAnalyzeThread::ERR_MEMOVERLOAD;
        break;
      }
    }

    m_qwTotalBR.QuadPart += dwBufSize;  // correct total bytes readen

    // correct buffer offset
    m_qwBufOffset.QuadPart = m_qwFileSize.QuadPart - m_qwBTRLeft.QuadPart;

    // fill buffer
    dwBTR = dwBufSize;                  // bytes to read
    bReadFile = ReadFile(hFile, pBuf, dwBTR, &(DWORD)dwBRN, NULL);
    if (FALSE == bReadFile)
    {
      CString sReport;
      sReport.Format(_T("Analyze was tried to read from target file and failed.\n")
        _T("Requested bytes to read: %d\nBytes readen: %d")
        , dwBTR, dwBRN);
      theApp.Report(sReport);
      m_ErrState = CAnalyzeThread::ERR_DEADFILE;
      break;
    }
    
    m_qwBTRLeft.QuadPart -= dwBRN;      // correct bytes to left

    /// FIND KEY IN BUFFER with buffer_size == bytes_readen_from_file ///
    i64Pos = this->Find(pBuf, dwBRN, OUT dwSISS);

    /// RESPOND ON RETURNED SEARCH_INNER_SELF_STATE or ERROR ///
    if (CAnalyzeThread::SISS_DO_JUMP == dwSISS)
    { // while offset-depended search need jump to
      // `i64Pos` :== global jump location
      qwSeek.QuadPart = i64Pos;
      qwSeek.LowPart = SetFilePointer(hFile, qwSeek.LowPart,
        (LONG*) &qwSeek.HighPart, FILE_CURRENT);

      if (INVALID_SET_FILE_POINTER == qwSeek.LowPart)
      {
        theApp.Report(_T("Analyze tried to seek through file forward and failed."));
        m_ErrState = CAnalyzeThread::ERR_DEADFILE;
        break;
      }

      m_qwBTRLeft.QuadPart -= i64Pos;
    }
    else if (CAnalyzeThread::SISS_SHORTAGE == dwSISS)
    { // Find in tracking state, request for
      // `i64Pos` :== bytes to return back
      // positive jump value convert to negative
      qwSeek.QuadPart = -(i64Pos);

      qwSeek.LowPart = ::SetFilePointer(hFile, qwSeek.LowPart,
        (LONG*) &qwSeek.HighPart, FILE_CURRENT);

      if (INVALID_SET_FILE_POINTER == qwSeek.LowPart)
      {
        theApp.Report(_T("Analyze tried to seek through file backward and failed."));
        m_ErrState = CAnalyzeThread::ERR_DEADFILE;
        break;
      }

      m_qwBTRLeft.QuadPart += i64Pos;
    }
    else if (CAnalyzeThread::ERR_ITEMS_LIMIT == dwSISS)
    { //* Items number reached the limit
      //  - set ret status and break the search loop
      m_ErrState = (CAnalyzeThread::ERR)dwSISS;
      m_bAbort = true;    // call break at next loop head
    }
    else if (CAnalyzeThread::ERR_MEMOVERLOAD == dwSISS)
    {
      m_ErrState = (CAnalyzeThread::ERR)dwSISS;
      m_bAbort = true;    // call break at next loop head
    }

    // update progress state
    if ((m_qwBufOffset.QuadPart) > (chkpnt * dPM))
    {
      ++dPM;

      m_dwProgress = (DWORD) (
        ( (double) (m_qwBufOffset.QuadPart + 1) /
          (double) m_qwFileSize.QuadPart ) * 100.0 );
    }
  }// END of the search loop

  VirtualFree(pBuf, 0, MEM_RELEASE);
  CloseHandle(hFile);
  
  // enclose search result if something is found
  if (NULL != m_pKey && !m_pKey->m_MediaSet.IsEmpty())
  {
    if (m_ErrState != CAnalyzeThread::ERR_ITEMS_LIMIT
      && m_ErrState != CAnalyzeThread::ERR_MEMOVERLOAD)
    {
      m_ErrState = CAnalyzeThread::ERR_ALRIGHT;
    }

    CMedia* pMedia = (CMedia *) m_pKey->m_MediaSet.GetTail();

    // enclosing last item (called only in continious scan)
    if (0 == m_pKey->m_dwCSO
      && 0 == pMedia->m_qwSize.QuadPart)
    {
      pMedia->m_qwSize.QuadPart =
        (m_qwFileSize.QuadPart - pMedia->m_qwOffset.QuadPart);
    }
  }

  m_bDone = true;
  m_dwProgress = 100;
  m_peTotal.Stop();
}

DWORD CAnalyzeThread::GetContentSize32(char *_buf)
{
  THROWIF(NULL == _buf
    , _T("DWORD CAnalyzeThread::GetContentSize32(char *_buf) --> NULL == _buf }"));

  DWORD dwCS = (_buf[0] & 0x000000FF)
    | ((_buf[1] << 8)   & 0x0000FF00)
    | ((_buf[2] << 16)  & 0x00FF0000)
    | ((_buf[3] << 24)  & 0xFF000000);

  return dwCS;
}

signed __int64 CAnalyzeThread::Find(char* _buf, DWORD _dwBufSize, OUT DWORD &_SISS)
{
  signed __int64 i64Pos = 0;  // position of found key relative to buffer
  QWORD qwBufIndx;   // cross buffer index
  QWORD qwKeyIndx;   // cross key compare indx
  QWORD qwGlobalOffset;       // global offset of the current item

  // bytes left in buffer
  QWORD qwLeftInBuffer = _dwBufSize;
  QWORD qwThisItemSize;       // calculated item size with header
  QWORD qwContentSize;        // calculated size on Content-Size-Offset

  // etalon limit for skipping multiple headers in this bytes scope
  DWORD dwSkipScopeEtalon = m_pKey->m_dwSkipScope;
  // scope skip counter
  QWORD qwInScopeCounter = dwSkipScopeEtalon + 1;
  // true if key compare is valid [assigned later]
  bool bKeyIsValid;

  // default value to jump back,
  // if `key` or `qwThisItemSize` is at buffer limit
  DWORD LackBreakPoint = (m_pKey->m_dwSignSize + m_pKey->m_dwCSO + 4); /*note: 4 not sizeof(DWORD)*/
  char* pBufCmpByte = NULL;   // ptr on file buffer byte to compare with
  char* pKeyCmpByte = NULL;   // ptr on key buffer byte to compare with
  CMedia* pMedia = NULL;      // ptr on media to deal with

  _SISS = CAnalyzeThread::SISS_GENERAL;

  qwBufIndx = 0;
  while ((qwBufIndx < (QWORD)_dwBufSize) && !m_bAbort)
  {
    qwKeyIndx = 0;

    // do ptr's fitting now (don't waste time in loop)
    pBufCmpByte = _buf + qwBufIndx;
    pKeyCmpByte = m_pKey->m_pSign;

    do
    {
      if (*pBufCmpByte == *pKeyCmpByte)
      { // bytes are equal
        bKeyIsValid = true;
        
        // if not enouth bytes in buffer to complete successful compare
        if ((qwBufIndx + qwKeyIndx) == (_dwBufSize - 1))
        {
          _SISS = CAnalyzeThread::SISS_SHORTAGE;

          // return number of valid bytes to start search from the clean page
          return (qwKeyIndx + 1);
        }
      }
      else
      { // not valid
        bKeyIsValid = false;
        break;
      }

      ++qwKeyIndx;
      __asm
      {
        inc pBufCmpByte
        inc pKeyCmpByte
      }
    } while ((qwKeyIndx < m_pKey->m_dwSignSize)
      && ((qwLeftInBuffer - qwKeyIndx) > 0)
      && !m_bAbort);
    
    if (m_bAbort) { return 0; }

    // KEY COMPARATION VALIDATION //
    if (bKeyIsValid)
    {
      if (m_pKey->m_dwCSO)
      { // OFFSET-DEPENDED SEARCH //
        // if not enouth buffer to calculate offsetted header size
        if ((qwBufIndx + LackBreakPoint) > _dwBufSize)
        { // jump backward by size of ..
          i64Pos = LackBreakPoint;
          _SISS = CAnalyzeThread::SISS_SHORTAGE;
          break;
        }

        qwContentSize = GetContentSize32(_buf + qwBufIndx + m_pKey->m_dwCSO);
        qwThisItemSize = m_pKey->GetItemSize(qwContentSize);
        qwGlobalOffset = m_qwBufOffset.QuadPart + qwBufIndx;

        // item size validation
        if ( (qwThisItemSize > (m_qwFileSize.QuadPart - qwGlobalOffset))
          || (!qwContentSize))
        {
          ++qwBufIndx;
          --qwLeftInBuffer;
          continue;
        }

        pMedia = new CMedia(qwGlobalOffset, qwThisItemSize);
        if (pMedia != NULL)
        {
          ++m_dwFound;
          pMedia->m_sName.Format(_T("%s_%04d%s")
            , m_pKey->m_sName, m_dwFound, m_pKey->GetExtension());

          m_pKey->m_MediaSet.AddTail(pMedia);
        }
        else
        {
          _SISS = CAnalyzeThread::ERR_MEMOVERLOAD;
          break;
        }

        qwBufIndx += qwThisItemSize - 1;
        qwLeftInBuffer -= qwThisItemSize - 1;

        // offset-relative:
        // if item ends in outer space of the current buffer
        if (  (qwGlobalOffset + qwThisItemSize)
            > (QWORD) (m_qwBufOffset.QuadPart + _dwBufSize - 1))
        { // tell to Analyze to read next buffer from that
          // cur-pos-relative position.
          // next is math-simplification of:
          // i64Pos = (m_qwBufOffset+qwBufIndx)-(m_qwBufOffset+_dwBufSize-1);
          i64Pos = qwBufIndx -_dwBufSize + 1;
          _SISS = CAnalyzeThread::SISS_DO_JUMP;
          break;
        }
      }
      else
      { // CONTINIOUS SEARCH //
        // first item found will be added to item list, second
        // item will be added only if `qwInScopeCntr` passed
        // `dwSkipScopeEtalon`.
        // (Skip-scope function valid only in continious search)
        if (qwInScopeCounter >= dwSkipScopeEtalon)
        {
          qwInScopeCounter = 0;

          // calculate global item position (file offset depended)
          qwGlobalOffset = m_qwBufOffset.QuadPart + qwBufIndx;

          // closing deal with previous item (if was)
          if (!m_pKey->m_MediaSet.IsEmpty())
          {
            pMedia = (CMedia *) m_pKey->m_MediaSet.GetTail();
          }

          if (pMedia != NULL)
          {
            pMedia->m_qwSize.QuadPart = qwGlobalOffset - pMedia->m_qwOffset.QuadPart;
          }

          // add new found item, item size (arg:2) will be defined:
          // - or in next item-found-case
          // - or at the end of analyze loop
          pMedia = new CMedia(qwGlobalOffset, 0);
          if (pMedia != NULL)
          {
            ++m_dwFound;
            pMedia->m_sName.Format(_T("%s~%04d%s")
              , m_pKey->m_sName, m_dwFound, m_pKey->GetExtension());

            m_pKey->m_MediaSet.AddTail(pMedia);

            i64Pos = qwGlobalOffset;
            qwBufIndx += qwKeyIndx;
            qwLeftInBuffer -= qwKeyIndx;
          }
          else
          {
            _SISS = ERR_MEMOVERLOAD;
            break;
          }
        }
      }

      // check for items limit reached
      if (pMedia)
      { // at last one item was found per loop
        if (m_pKey->m_MediaSet.GetCount() == nPRG_MAX_ITEMS_LIMIT)
        {  // leave comparation loop
          _SISS = CAnalyzeThread::ERR_ITEMS_LIMIT;
          break;
        }
      }

    }// if compare is broken - continue from here

    ++qwBufIndx;
    --qwLeftInBuffer;
    ++qwInScopeCounter;
  }

  // translate to global SISS
  if (_SISS == CAnalyzeThread::SISS_GENERAL)
  {
    _SISS = CAnalyzeThread::SISS_NEED_NEXT;
  }

  return i64Pos;
}
