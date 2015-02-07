// CReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include ".\CReportDlg.h"


// CReportDlg dialog

IMPLEMENT_DYNAMIC(CReportDlg, CDialog)

CReportDlg::CReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReportDlg::IDD, pParent)
  , m_txtMessage(_T(""))
  , m_txtLastError(_T(""))
{
  m_bTerminate = false;
}

CReportDlg::~CReportDlg()
{
}

void CReportDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_MESSAGE, m_txtMessage);
  DDX_Text(pDX, IDC_EDIT_LASTERROR, m_txtLastError);
  DDX_Control(pDX, IDC_EDIT_MESSAGE, m_editMsgCtrl);
}

BEGIN_MESSAGE_MAP(CReportDlg, CDialog)
  ON_BN_CLICKED(IDC_BTN_TERMINATE, &CReportDlg::OnBnClickedBtnTerminate)
END_MESSAGE_MAP()

// CReportDlg message handlers
BOOL CReportDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  SetWindowText(sPRG_REPORTDLG_CAPTION);  // set dialog caption
  m_editMsgCtrl.LineScroll(m_editMsgCtrl.GetLineCount());

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CReportDlg::OnBnClickedBtnTerminate()
{
  m_bTerminate = true;
  OnCancel();
}
