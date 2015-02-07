// CKeyImportDlg.cpp : implementation file
//

#include "StdAfx.h"
#include ".\CKeyImportDlg.h"

// CKeyImportDlg dialog

IMPLEMENT_DYNAMIC(CKeyImportDlg, CDialog)

CKeyImportDlg::CKeyImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyImportDlg::IDD, pParent)
  , m_eventLog(_T(""))
{
  m_sAppPath = _T("");
  m_pKeyHolder = NULL;
}

CKeyImportDlg::~CKeyImportDlg()
{
}

void CKeyImportDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EVENTLOG, m_eventLog);
  DDX_Control(pDX, IDC_EVENTLOG, m_event);
  DDX_Control(pDX, IDC_BTN_IMPORT, m_btnImport);
  DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
  DDX_Control(pDX, IDC_CMB_IMPORT_TYPE, m_cmbImportType);
}

BEGIN_MESSAGE_MAP(CKeyImportDlg, CDialog)
  ON_BN_CLICKED(IDC_BTN_OPENFILE, OnBnClickedBtnOpenfile)
  ON_BN_CLICKED(IDC_BTN_IMPORT, OnBnClickedBtnImport)
  ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
  ON_CBN_SELCHANGE(IDC_CMB_IMPORT_TYPE, OnImportType_SelChange)
END_MESSAGE_MAP()


// CKeyImportDlg message handlers

BOOL CKeyImportDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  SetWindowText(sPRG_IMPORTDLG_CAPTION);  // set dialog caption

  InitImportTypeCombo();

  CString sReport;
  sReport.Format(sPRG_APP_NAME _T(" currently has {%d} keys.")
    , m_pKeyHolder->Count());
  Log(sReport);

  LogImportTypeChange();

  Log(_T("\r\n> Please, select the Source File."));

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CKeyImportDlg::OnBnClickedBtnOpenfile()
{
  CString sReport;

  Log(_T("\r\n---------------------------\r\n- Retrieving file name..."));
  m_sImportFile = GetOpenFileFilename();

  if (m_sImportFile != _T(""))
  {
    // received valid filename
    m_btnImport.EnableWindow();
    m_btnImport.SetFocus();

    sReport.Format(_T("\r\n  {%s}"), m_sImportFile);
    sReport += _T("\r\n> Ready to Import.");
  }
  else
  {
    // received invalid filename or canceled
    m_btnImport.EnableWindow(FALSE);

    sReport = _T("\r\n  File selection: Canceled.");
  }

  Log(sReport);
}

void CKeyImportDlg::OnBnClickedBtnImport()
{
  bool bResult = false;
  CKeyImportDlg::IMPORT_TYPE importType;
  CString sImportHistory;     // Import history from CKeyHolder

  Log(_T("\r\n- Importing..."));

  importType = (CKeyImportDlg::IMPORT_TYPE) m_cmbImportType.GetCurSel();
  switch (importType)
  {
  case CKeyImportDlg::IMPORT_TYPE_REPLACE_THE_SAME:
    {
      sImportHistory = m_pKeyHolder->Import_Replace(m_sImportFile, bResult);
    }
    break;

  case CKeyImportDlg::IMPORT_TYPE_SKIP_THE_SAME:
    {
      sImportHistory = m_pKeyHolder->Import_Skip(m_sImportFile, bResult);
    }
    break;

  case CKeyImportDlg::IMPORT_TYPE_IMPORT_ON_CLEAR:
    {
      sImportHistory = m_pKeyHolder->Import_Clear(m_sImportFile, bResult);
    }
    break;

  default:
    __assume(0);
  }

  Log(sImportHistory);

  if (bResult == true)
  {
    // ok, saved
    Log(_T("\r\n\tSUCCESFUL"));
    m_btnClose.SetFocus();
  }
  else
  {
    // bad
    Log(_T("\r\n\tFAILED!"));
  }
}

void CKeyImportDlg::OnBnClickedBtnClose()
{
  this->UpdateData(FALSE);

  CDialog::OnCancel();
  this->EndDialog(0);
}

void CKeyImportDlg::Log(CString _str)
{
  m_eventLog += _str;
  this->UpdateData(FALSE);
  m_event.LineScroll(m_event.GetLineCount());
}

CString CKeyImportDlg::GetOpenFileFilename()
{
  wchar_t szFileName[nPRG_MAX_PATH] = _T("\0");
  wchar_t szTitle[] = _T("Please, select REG file you want to import...");

  // Open "Open file..." dialog to get file name to export to
  // ...prepeare...
  OPENFILENAME ofn;
  SecureZeroMemory(&ofn, sizeof (ofn));
  ofn.lStructSize = sizeof (ofn);
  ofn.hwndOwner = m_hWnd;
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = nPRG_MAX_PATH * sizeof (wchar_t);
  ofn.lpstrFilter = _T("Registry (*.REG)\0*.REG\0\0");
  ofn.nFilterIndex = 1;
  ofn.lpstrInitialDir = m_sAppPath;
  ofn.lpstrTitle = szTitle;
  ofn.Flags =
    OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING
    | OFN_EXPLORER | OFN_READONLY | OFN_LONGNAMES | OFN_NONETWORKBUTTON;

  // ...open...
  if (GetOpenFileName(&ofn) == FALSE)
  {// if no file selected
    return CString(_T(""));
  }
  else
  {
    return CString(szFileName);
  }
}

void CKeyImportDlg::InitImportTypeCombo()
{
  m_cmbImportType.ResetContent();

  for (DWORD i = 0; i < 3; i++)
  {
    m_cmbImportType.AddString(GetImportTypeString(i));
  }

  m_cmbImportType.SetCurSel(1);
}

CString CKeyImportDlg::GetImportTypeString(DWORD _dwIndex)
{
  switch (_dwIndex)
  {
  case CKeyImportDlg::IMPORT_TYPE_IMPORT_ON_CLEAR:
    return sPRG_IMPORTTYPE_IMPORT_ON_CLEAR;

  case CKeyImportDlg::IMPORT_TYPE_REPLACE_THE_SAME:
    return sPRG_IMPORTTYPE_REPLACE_THE_SAME;

  case CKeyImportDlg::IMPORT_TYPE_SKIP_THE_SAME:
    return sPRG_IMPORTTYPE_SKIP_THE_SAME;

  default: __assume(0);
  }
}

void CKeyImportDlg::LogImportTypeChange()
{
  DWORD iSelected;
  CString sLog;

  iSelected = m_cmbImportType.GetCurSel();
  sLog.Format(_T("\r\n- Import Type:\r\n  {%s}"), GetImportTypeString(iSelected));
  Log( sLog );
}

void CKeyImportDlg::OnImportType_SelChange()
{
  LogImportTypeChange();
}
