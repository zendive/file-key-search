// CKeyExportDlg.cpp : implementation file
//

#include "StdAfx.h"
#include ".\CKeyExportDlg.h"

// CKeyExportDlg dialog

IMPLEMENT_DYNAMIC(CKeyExportDlg, CDialog)

CKeyExportDlg::CKeyExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyExportDlg::IDD, pParent)
  , m_eventLog(_T(""))
{
  m_sAppPath = _T("");
  m_pKeyHolder = NULL;
}

CKeyExportDlg::~CKeyExportDlg()
{
}

void CKeyExportDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_EVENTLOG, m_eventLog);
  DDX_Control(pDX, IDC_BTN_EXPORT, m_btnExport);
  DDX_Control(pDX, IDC_EDIT_EVENTLOG, m_event);
  DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
}

BEGIN_MESSAGE_MAP(CKeyExportDlg, CDialog)
  ON_BN_CLICKED(IDC_BTN_SAVETO, OnBnClickedBtnSaveto)
  ON_BN_CLICKED(IDC_BTN_EXPORT, OnBnClickedBtnExport)
  ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
END_MESSAGE_MAP()


// CKeyExportDlg message handlers


BOOL CKeyExportDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  SetWindowText(sPRG_EXPORTDLG_CAPTION);

  CString sReport;
  sReport.Format(sPRG_APP_NAME _T(" currently has {%d} keys.")
    _T("\r\n> Please, select the destination File.")
    , m_pKeyHolder->Count());
  Log(sReport);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CKeyExportDlg::OnBnClickedBtnSaveto()
{
  CString sReport;

  Log(_T("\r\n---------------------------\r\n- Retrieving file name..."));
  m_sExportFile = GetSaveToFilename();

  if (m_sExportFile != _T(""))
  {
    // received valid filename
    m_btnExport.EnableWindow();
    m_btnExport.SetFocus();

    sReport.Format(_T("\r\n  {%s}"), m_sExportFile);
    sReport += _T("\r\n> Ready to Export.");
  }
  else
  {
    // received invalid filename or canceled
    m_btnExport.EnableWindow(FALSE);

    sReport = _T("\r\n  File selection: Canceled.");
  }

  Log(sReport);
}

void CKeyExportDlg::OnBnClickedBtnExport()
{
  bool bResult;

  Log(_T("\r\n- Exporting..."));
  bResult = m_pKeyHolder->ExportToFile(m_sExportFile);
  if (bResult == true)
  {
    // ok, saved
    Log(_T("\tSUCCESFUL."));
    m_btnClose.SetFocus();
  }
  else
  {
    // bad
    Log(_T("\tFAILED!"));
  }
}

void CKeyExportDlg::OnBnClickedBtnClose()
{
  this->UpdateData(FALSE);

  CDialog::OnCancel();
  this->EndDialog(0);
}

CString CKeyExportDlg::GetSaveToFilename()
{
  wchar_t szFileName[nPRG_MAX_PATH] = sPRG_KEY_EXPORT_DEFAULT_NAME;
  wchar_t szTitle[] = _T("Please, name the REG file to Export the Keys into...");

  // Open "save as" dialog to get file name to export to
  // ...prepeare...
  OPENFILENAME ofn;
  SecureZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = m_hWnd;
  ofn.lpstrFilter = _T("Registry (*.REG)\0*.REG\0\0");
  ofn.nFilterIndex = 1;
  ofn.lpstrDefExt = _T("REG");
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = nPRG_MAX_PATH * sizeof (wchar_t);
  ofn.lpstrInitialDir = m_sAppPath;
  ofn.lpstrTitle = szTitle;
  ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES
    | OFN_OVERWRITEPROMPT;

  // ...open...
  if (GetSaveFileName(&ofn) == FALSE)
  {// if no file selected
    return CString(_T(""));
  }
  else
  {
    return CString( szFileName );
  }
}

void CKeyExportDlg::Log(CString _str)
{
  m_eventLog += _str;
  this->UpdateData(FALSE);
  m_event.LineScroll(m_event.GetLineCount());
}
