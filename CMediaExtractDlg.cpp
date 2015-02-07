#include "stdafx.h"
#include "CApp.h"
#include "CMediaExtractDlg.h"
#include "CDoc.h"

// CMediaExtractDlg dialog

IMPLEMENT_DYNAMIC(CMediaExtractDlg, CDialog)

void CMediaExtractDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_PROGRESS_BAR, m_pbAdvance);
  DDX_Control(pDX, IDC_TXT_PROGRESS, m_txtStatus);
  DDX_Control(pDX, IDC_BTN_PAUSE, m_btnPause);
  DDX_Control(pDX, IDCANCEL, m_btnCancel);
}

BEGIN_MESSAGE_MAP(CMediaExtractDlg, CDialog)
  ON_WM_TIMER()
  ON_BN_CLICKED(IDCANCEL, &CMediaExtractDlg::OnBnClickedCancel)
  ON_BN_CLICKED(IDC_BTN_PAUSE, &CMediaExtractDlg::OnBnClickedBtnPause)
END_MESSAGE_MAP()

CMediaExtractDlg::CMediaExtractDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMediaExtractDlg::IDD, pParent)
{
  m_pExtractThread = NULL;
  m_pKeyList = NULL;
  m_pDoc = NULL;
  m_bPaused = false;
  m_bNeedAbort = true;
  m_sErrorSummary = _T("");
  m_clrPrevPBColor = CMediaExtractDlg::PB_COLOR_IS_OK;
}

CMediaExtractDlg::~CMediaExtractDlg()
{
  DestroyThread();
  CDialog::~CDialog();
}

void CMediaExtractDlg::DestroyThread()
{
  RETURNIF(NULL == m_pExtractThread);
  
  ::CloseHandle(m_pExtractThread->m_hThread);
  m_pExtractThread->m_hThread = NULL;

  delete m_pExtractThread;
  m_pExtractThread = NULL;
}

BOOL CMediaExtractDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_btnPause.EnableWindow(FALSE);
  m_btnCancel.EnableWindow(FALSE);

  m_hTimerPrg = SetTimer(CMediaExtractDlg::TIMER_ID_PROGRESS
    , CMediaExtractDlg::TIMER_ID_PROGRESS_ELAPSE, NULL);
  if (NULL == m_hTimerPrg)
  {
    theApp.Report(_T("Warning! Insufficient Timer resources.")
      _T("\r\n- Statistics will not refresh.")
      _T("\r\n- Howether analyze may continue IF nothing else bad will happen.")
      _T("\r\n- Will recommend to wait till CPU idlness resumes."));
  }

  m_pbAdvance.SetRange(0, 100);
  m_pbAdvance.SetStep(1);

  // background color
  m_pbAdvance.SetBkColor(RGB(0, 0, 0));
  // foreground color
  ::SendMessage(m_pbAdvance.m_hWnd
    , PBM_SETBARCOLOR
    , (WPARAM) 0 // not used must be zero
    , (LPARAM) m_clrPrevPBColor);

  Start();
  OnTimer(0);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMediaExtractDlg::Start()
{
  m_btnPause.EnableWindow(TRUE);
  m_btnCancel.EnableWindow(TRUE);

  m_pExtractThread = new CExtractThread();
  m_pExtractThread->m_bAutoDelete = FALSE;
  m_pExtractThread->m_pKeyList = m_pKeyList;
  m_pExtractThread->m_sTargetFileName = m_pDoc->m_DocData.m_sFileName;
  m_pExtractThread->m_sExtractDir = m_pDoc->m_DocData.m_sExtractDir;

  m_pExtractThread->CreateThread(CREATE_SUSPENDED);
  m_pExtractThread->SetThreadPriority(THREAD_PRIORITY_IDLE);
  m_pExtractThread->ResumeThread();
}

void CMediaExtractDlg::OnBnClickedCancel()
{
  OnCancel();
}

void CMediaExtractDlg::Stop()
{
  m_bNeedAbort = false;
  this->OnCancel();
}

void CMediaExtractDlg::OnCancel()
{
  SetWindowText(_T("Finishing..."));
  m_btnCancel.EnableWindow(FALSE);
  m_btnPause.EnableWindow(FALSE);

  if (NULL != m_hTimerPrg)
  {
    KillTimer(m_hTimerPrg);
    m_hTimerPrg = NULL;
  }

  if (m_bNeedAbort)
  {
    AbortThread();
  }

  DestroyThread();

  if (!m_sErrorSummary.IsEmpty())
  { // have something to tell
    theApp.Report(m_sErrorSummary);
  }

  //@non modal dialog should call DestroyWindow
  this->DestroyWindow();
  m_pDoc->OnMediaExtractDlgEnd();
}

void CMediaExtractDlg::OnBnClickedBtnPause()
{
  m_btnPause.EnableWindow(FALSE);

  LPARAM rgbColor;
  
  if (m_bPaused)
  { // if already paused
    m_btnPause.SetWindowText(_T("&Suspend"));
    rgbColor = m_clrPrevPBColor;
  }
  else
  {
    m_btnPause.SetWindowText(_T("&Resume"));
    rgbColor = CMediaExtractDlg::PB_COLOR_IS_PAUSED;
  }

  // foreground color
  ::SendMessage(m_pbAdvance.m_hWnd
    , PBM_SETBARCOLOR
    , (WPARAM) 0 // not used must be zero
    , (LPARAM) rgbColor);

  m_bPaused = !m_bPaused;

  if (!m_pExtractThread->m_bDone)
  {
    m_pExtractThread->m_bPaused = m_bPaused;
  }

  m_btnPause.EnableWindow(TRUE);
  m_btnPause.SetFocus();
}

void CMediaExtractDlg::AbortThread()
{
  RETURNIF(NULL == m_pExtractThread);

  m_pExtractThread->m_bAbort = true;

  DWORD dwTry2Sleep = 50;
  while (!m_pExtractThread->m_bDone && dwTry2Sleep--)
  {
    ::Sleep(100);
  }

  if (!dwTry2Sleep)
  { // sleep's timeout, so its frozen?
    // warning C6258: Using TerminateThread does not allow proper thread clean up
    ::TerminateThread(*m_pExtractThread, 13);
  }
}

void CMediaExtractDlg::OnTimer(UINT /*nIDEvent*/)
{
  RETURNIF(NULL == m_pExtractThread || m_bPaused);
  
  m_txtStatus.SetWindowText(m_pExtractThread->m_sMediaFileName);
  
  int iProgress = (int)(100 * m_pExtractThread->m_uliTotalBytesReaden.QuadPart / m_uliTotalMediaSize.QuadPart);
  m_pbAdvance.SetPos(iProgress);

  CString sStatus;
  sStatus.Format(_T("Extracted %s of %s at %s KB/sec")
    , CConvert::NumberToSizeString(m_pExtractThread->m_uliTotalBytesReaden)
    , CConvert::NumberToSizeString(m_uliTotalMediaSize)
    , CConvert::NumberToHumanString(m_pExtractThread->GetBPMS()));
  SetWindowText(sStatus);

  if (m_pExtractThread->m_bDone)
  {
    this->Stop();
  }

  //CDialog::OnTimer(nIDEvent);
}