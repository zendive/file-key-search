#include "stdafx.h"
#include "CApp.h"
#include "CAnalyzeDlg.h"
#include "CDoc.h"

// CAnalyzeDlg dialog

IMPLEMENT_DYNAMIC(CAnalyzeDlg, CDialog)

// private default constructor - must be never used
CAnalyzeDlg::CAnalyzeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnalyzeDlg::IDD, pParent)
{
}

CAnalyzeDlg::CAnalyzeDlg(CWnd* pParent, CDoc* _pDoc)
	: CDialog(CAnalyzeDlg::IDD, pParent)
{
  m_pDoc = _pDoc;
  m_dwTotalProgress = 0;
  m_hTimerPrg = NULL;
  m_paThreads = NULL;
  m_dwThreads = 0;
  m_bPaused = false;
  m_bNeedAbort = true;
  m_clrPrevPBColor = CAnalyzeDlg::PB_COLOR_IS_OK;
  m_sErrorSummary = _T("");
  m_dwTotalFound = 0;
}

CAnalyzeDlg::~CAnalyzeDlg()
{
  DestroyThreads();

  CDialog::~CDialog();
}

void CAnalyzeDlg::DestroyThreads()
{
  RETURNIF(NULL == m_paThreads);

  while (m_dwThreads--)
  {
    CONTINUEIF(NULL == m_paThreads[m_dwThreads]);

    CloseHandle(m_paThreads[m_dwThreads]->m_hThread);
    m_paThreads[m_dwThreads]->m_hThread = NULL;
    delete m_paThreads[m_dwThreads];
    m_paThreads[m_dwThreads] = NULL;
  }

  delete [] m_paThreads;
  m_paThreads = NULL;
  m_dwThreads = 0;
}

void CAnalyzeDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_STATIC_STATISTICS, m_txtStatistics);
  DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_prgrsTotal);
  DDX_Control(pDX, IDC_BTN_PAUSE, m_btnPauseResume);
  DDX_Control(pDX, IDCANCEL, m_btnCancel);
  DDX_Control(pDX, IDC_CMB_MEMORY, m_cmbMemory);
}

BEGIN_MESSAGE_MAP(CAnalyzeDlg, CDialog)
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_BTN_PAUSE, OnBnClickedBtnPause)
  ON_CBN_SELCHANGE(IDC_CMB_MEMORY, OnCbnSelchangeCmbMemory)
END_MESSAGE_MAP()

// CAnalyzeDlg message handlers

BOOL CAnalyzeDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_btnCancel.EnableWindow(FALSE);
  m_btnPauseResume.EnableWindow(FALSE);

  InitTimer();
  InitProgressBar();
  InitMemoryCombobox();
  SetDialogCaption();
  Start();
  OnTimer(NULL);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnalyzeDlg::InitTimer()
{
  m_hTimerPrg = SetTimer(CAnalyzeDlg::TIMER_ID_PROGRESS
    , CAnalyzeDlg::TIMER_ID_PROGRESS_ELAPSE, NULL);

  if (NULL == m_hTimerPrg)
  {
    theApp.Report(_T("Warning! Insufficient Timer resources.")
      _T("\r\n- Statistics will not refresh.")
      _T("\r\n- Howether analyze may continue IF nothing else bad will happen.")
      _T("\r\n- Will recommend to wait till CPU idlness resumes."));
  }
}

void CAnalyzeDlg::InitProgressBar()
{
  m_prgrsTotal.SetRange(0, 100);
  m_prgrsTotal.SetStep(1);

  // background color
  m_prgrsTotal.SetBkColor(RGB(0, 0, 0));
  // foreground color
  ::SendMessage(m_prgrsTotal.m_hWnd
    , PBM_SETBARCOLOR
    , (WPARAM) 0 // not used must be zero
    , (LPARAM) m_clrPrevPBColor);
}

void CAnalyzeDlg::InitMemoryCombobox()
{
  CString sEntry;

  sEntry.Format(_T("%s, High"), CConvert::NumberToSizeString(nPRG_ANALYZE_MEMORY_HIGH));
  m_cmbMemory.AddString(sEntry);
  sEntry.Format(_T("%s, Normal"), CConvert::NumberToSizeString(nPRG_ANALYZE_MEMORY_NORMAL));
  m_cmbMemory.AddString(sEntry);
  sEntry.Format(_T("%s, Low"), CConvert::NumberToSizeString(nPRG_ANALYZE_MEMORY_LOW));
  m_cmbMemory.AddString(sEntry);

  m_cmbMemory.SetCurSel(CAnalyzeDlg::MEMORY_NORMAL);
}

void CAnalyzeDlg::CollectErrorInformation()
{
  if (NULL == m_paThreads)
  {
    m_sErrorSummary = _T("Insufficient memory resources;");
    return;
  }
  
  CString sErr;
  m_sErrorSummary = _T("");
  bool bFirst = true;

  for (DWORD i = 0; i < m_dwThreads; i++)
  {
    if (NULL == m_paThreads[i]) { continue; }
    if (m_paThreads[i]->m_ErrState != CAnalyzeThread::ERR_ALRIGHT)
    {
      sErr.Format(_T("%sAnalyze for key {%s} was stopped because of:\r\n- ")
        , ((bFirst)? _T("") : _T("\r\n")), m_paThreads[i]->m_pKey->m_sName);
      bFirst = false;

      switch (m_paThreads[i]->m_ErrState)
      {
        case CAnalyzeThread::ERR_DEADFILE:
          sErr += _T("General file IO error;");
          break;

        case CAnalyzeThread::ERR_ITEMS_LIMIT:
          sErr += _T("Found more items than it is possible to display in one view;");
          break;

        case CAnalyzeThread::ERR_MEMOVERLOAD:
          sErr += _T("Insufficient memory resources to continue;");
          break;

        default: __assume(0);
      }

      m_sErrorSummary += sErr;
    }
  }
}

void CAnalyzeDlg::AbortAllThreads()
{
  RETURNIF(NULL == m_paThreads || !m_dwThreads);

  DWORD c = m_dwThreads;
  while (c--)
  { // abort all running threads
    CONTINUEIF(NULL == m_paThreads[c] || m_paThreads[c]->m_bDone);
    m_paThreads[c]->m_bAbort = true;
    ::Sleep(100);
  }

  DWORD dwTry2Sleep;
  c = m_dwThreads;
  
  while (c--)
  { // wait till it done or sleep timeout
    CONTINUEIF(NULL == m_paThreads[c] || m_paThreads[c]->m_bDone);

    dwTry2Sleep = 50;
    while (!m_paThreads[c]->m_bDone && dwTry2Sleep--)
    {
      ::Sleep(100);
    }

    if (!dwTry2Sleep)
    { // sleep's timeout, so its frozen?
      // warning C6258: Using TerminateThread does not allow proper thread clean up
      ::TerminateThread(*m_paThreads[c], 13);
    }
  }

  c = m_dwThreads;
  m_dwTotalFound = 0;
  while (c--)
  { // collect their results
    CONTINUEIF(NULL == m_paThreads[c]);
    m_dwTotalFound += m_paThreads[c]->m_dwFound;
  }
}

void CAnalyzeDlg::SetDialogCaption()
{
  CPath sFileName = m_pDoc->m_DocData.m_sFileName;
  sFileName.StripPath();

  CString sCaption;
  sCaption.Format(_T(" Searching: %s"), sFileName);
  SetWindowText(sCaption);
}

void CAnalyzeDlg::UpdateStatistics(DWORD _dwBusyTreads
  , ULARGE_INTEGER _qwBPMS, ULARGE_INTEGER _qwBTRLeft)
{
  CString sTime;
  const __int64 i64_MSecPerSec = 1000;
  const __int64 i64_SecPerMin = 60;
  const __int64 i64_SecPerHour = i64_SecPerMin * 60;  // 3600
  const __int64 i64_SecPerDay = i64_SecPerHour * 24;  // 86400
  const __int64 i64_HourPerDay = 24;
  ULARGE_INTEGER qwTimeSec;

  if ((_qwBPMS.QuadPart > 0) && (_qwBTRLeft.QuadPart > 1000))
  {
    // to seconds
    qwTimeSec.QuadPart =  (_qwBTRLeft.QuadPart / _qwBPMS.QuadPart) / i64_MSecPerSec;

    if ((qwTimeSec.QuadPart/i64_SecPerHour) > i64_HourPerDay)
    { // DDd HH:MM:SS
      sTime.Format(_T("%I64ud %.02I64u:%.02I64u:%.02I64u")
        , qwTimeSec.QuadPart/i64_SecPerDay
        , (qwTimeSec.QuadPart/i64_SecPerHour)%i64_HourPerDay
        , (qwTimeSec.QuadPart/i64_SecPerMin)%i64_SecPerMin
        , qwTimeSec.QuadPart % i64_SecPerMin);
    }
    else
    { // HH:MM:SS
      sTime.Format(_T("%.02I64u:%.02I64u:%.02I64u")
        , qwTimeSec.QuadPart/i64_SecPerHour
        , (qwTimeSec.QuadPart/i64_SecPerMin)%i64_SecPerMin
        , qwTimeSec.QuadPart % i64_SecPerMin);
    }
  }
  else
  {
    sTime = _T("undefined");
  }
  
  CString sSpeed;
  sSpeed.Format(_T("%s KB/sec"), CConvert::NumberToHumanString(_qwBPMS));
  
  CString sStatistics;
  sStatistics.Format(_T("Left %d key%s of %d, %s, Found %d, %s")
    , _dwBusyTreads
    , (_dwBusyTreads != 1)? _T("s") : _T("")
    , m_dwThreads
    , sTime
    , m_dwTotalFound
    , sSpeed);
  m_txtStatistics.SetWindowText(sStatistics);
  
  m_prgrsTotal.SetPos(m_dwTotalProgress);
}

void CAnalyzeDlg::OnTimer(UINT /*nIDEvent*/)
{
  RETURNIF(NULL == m_paThreads || NULL == m_hTimerPrg || m_bPaused);
  m_btnCancel.EnableWindow(TRUE);
  m_btnPauseResume.EnableWindow(TRUE);

  // collect statistics from the threads
  ULARGE_INTEGER qwBTRLeft;   // average bytes left to read
  ULARGE_INTEGER qwBPMS;      // average BPMS
  bool bError = false;        // thread encountered an error
  DWORD dwBusyTreads = 0;     // number of busy threads

  m_dwTotalFound = 0;
  m_dwTotalProgress = 0;
  qwBTRLeft.QuadPart = 0;
  qwBPMS.QuadPart = 0;

  for (DWORD i = 0; i < m_dwThreads; i++)
  {
    m_dwTotalFound += m_paThreads[i]->m_dwFound;
    m_dwTotalProgress += m_paThreads[i]->m_dwProgress;

    if (!m_paThreads[i]->m_bDone)
    {
      ++dwBusyTreads;

      qwBTRLeft.QuadPart += m_paThreads[i]->m_qwBTRLeft.QuadPart;
      qwBPMS.QuadPart += m_paThreads[i]->GetBPMS();
    }

    if (m_paThreads[i]->m_ErrState != CAnalyzeThread::ERR_ALRIGHT)
    {
      bError = true;
    }
  }

  // calculate average progress
  if (m_dwThreads)
  {
    m_dwTotalProgress /= m_dwThreads;
  }

  // --//-- average statistics
  if (dwBusyTreads)
  {
    qwBTRLeft.QuadPart /= dwBusyTreads;
    qwBPMS.QuadPart /= dwBusyTreads;
  }

  UpdateStatistics(dwBusyTreads, qwBPMS, qwBTRLeft);

  // when all done
  if (!dwBusyTreads)
  {
    this->Stop();
  }

  if (bError && m_clrPrevPBColor != CAnalyzeDlg::PB_COLOR_IS_ERROR)
  { // reflect error existance by changing progress-bar foreground color
    m_clrPrevPBColor = CAnalyzeDlg::PB_COLOR_IS_ERROR;
    ::SendMessage(m_prgrsTotal.m_hWnd
      , PBM_SETBARCOLOR // foreground color
      , (WPARAM) 0      // not used must be zero
      , (LPARAM) m_clrPrevPBColor);
  }

  ///CDialog::OnTimer(nIDEvent);
}

void CAnalyzeDlg::Start()
{
  CKeyList* klUsed = &m_pDoc->m_DocData.m_klUsed;
  CKey* key = NULL;

  DWORD dwKlSize = m_pDoc->m_DocData.m_klUsed.Count();
  m_dwThreads = 0;

  m_paThreads = new CAnalyzeThread* [dwKlSize];
  if (NULL == m_paThreads)
  {
    this->Stop();
    return;
  }

  // Thread allocation
  DWORD i = 0;
  for (key = klUsed->GetRoot(); (key != NULL); i++, key = key->Next())
  {
    m_paThreads[i] = new CAnalyzeThread();

    if (NULL == m_paThreads[i])
    { // begin rollback on memory lack
      while (m_dwThreads--)
      {
        CloseHandle(m_paThreads[m_dwThreads]->m_hThread);
        m_paThreads[m_dwThreads]->m_hThread = NULL;
        delete m_paThreads[m_dwThreads];
      }

      delete[] m_paThreads;
      m_paThreads = NULL;
      m_dwThreads = 0;
      
      m_sErrorSummary = _T("Insufficient memory resources;");
      this->Stop();
      return;
    }

    ++m_dwThreads;
  }

  m_txtStatistics.SetWindowText(_T("Starting..."));

  // initialization
  for (i = 0, key = klUsed->GetRoot(); (key != NULL); i++, key = key->Next())
  {
    m_paThreads[i]->m_bAutoDelete = FALSE;
    m_paThreads[i]->CreateThread(CREATE_SUSPENDED);
    m_paThreads[i]->SetThreadPriority(THREAD_PRIORITY_IDLE);
    m_paThreads[i]->m_dwBufferAllocationSize = nPRG_ANALYZE_MEMORY_NORMAL;
    m_paThreads[i]->m_pKey = key;
    m_paThreads[i]->m_sFile = m_pDoc->m_DocData.m_sFileName;
  }
  
  // launching
  i = m_dwThreads;
  while (i--)
  {
    m_paThreads[i]->ResumeThread();
    AFXDUMP("Thread id:" << m_paThreads[i]->m_nThreadID
      << "; k:" << m_paThreads[i]->m_pKey->m_sName << "\n");
  }
}

void CAnalyzeDlg::Stop()
{
  m_bNeedAbort = false;
  this->OnCancel();
}

void CAnalyzeDlg::OnCancel()
{
  m_txtStatistics.SetWindowText(_T("Finishing..."));
  m_btnCancel.EnableWindow(FALSE);
  m_btnPauseResume.EnableWindow(FALSE);

  if (NULL != m_hTimerPrg)
  {
    KillTimer(m_hTimerPrg);
    m_hTimerPrg = NULL;
  }

  CollectErrorInformation();
  
  if (m_bNeedAbort)
  {
    AbortAllThreads();
  }
  
  DestroyThreads();
  
  if (!m_sErrorSummary.IsEmpty())
  { // have something to tell
    theApp.Report(m_sErrorSummary);
  }

  //@non modal dialog should call DestroyWindow
  this->DestroyWindow();
  m_pDoc->OnKeyAnalyzeDlgEnd();
}

void CAnalyzeDlg::OnBnClickedBtnPause()
{
  RETURNIF(NULL == m_paThreads);
  m_btnPauseResume.EnableWindow(FALSE);

  LPARAM rgbColor;

  if (m_bPaused)
  { // if already paused
    m_btnPauseResume.SetWindowText(_T("&Suspend"));
    rgbColor = m_clrPrevPBColor;
  }
  else
  {
    m_btnPauseResume.SetWindowText(_T("&Resume"));
    rgbColor = CAnalyzeDlg::PB_COLOR_IS_PAUSED;
  }

  // foreground color
  ::SendMessage(m_prgrsTotal.m_hWnd
    , PBM_SETBARCOLOR
    , (WPARAM) 0 // not used must be zero
    , (LPARAM) rgbColor);

  m_bPaused = !m_bPaused;

  DWORD c = m_dwThreads;
  while (c--)
  {
    CONTINUEIF(m_paThreads[c]->m_bDone);
    m_paThreads[c]->m_bPaused = m_bPaused;
  }

  m_btnPauseResume.EnableWindow(TRUE);
  m_btnPauseResume.SetFocus();
}

void CAnalyzeDlg::OnCbnSelchangeCmbMemory()
{
  RETURNIF(NULL == m_paThreads);
  DWORD dwBufferSize = nPRG_ANALYZE_MEMORY_NORMAL;
  
  int iCurSel = m_cmbMemory.GetCurSel();
  switch (iCurSel)
  {
    case CAnalyzeDlg::MEMORY_HIGH: dwBufferSize = nPRG_ANALYZE_MEMORY_HIGH; break;
    case CAnalyzeDlg::MEMORY_NORMAL: dwBufferSize = nPRG_ANALYZE_MEMORY_NORMAL; break;
    case CAnalyzeDlg::MEMORY_LOW: dwBufferSize = nPRG_ANALYZE_MEMORY_LOW; break;
    default: dwBufferSize = nPRG_ANALYZE_MEMORY_LOW; break;
  }
  
  DWORD c = m_dwThreads;
  while (c--)
  {
    if (!m_paThreads[c]->m_bDone)
    {
      ::EnterCriticalSection(&m_paThreads[c]->m_csBufferAllocationSize);
      m_paThreads[c]->m_dwBufferAllocationSize = dwBufferSize;
      ::LeaveCriticalSection(&m_paThreads[c]->m_csBufferAllocationSize);
    }
  }
}
