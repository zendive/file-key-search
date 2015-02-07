// CKeyUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CApp.h"
#include ".\CKeyUpdateDlg.h"
#include ".\CKeyManagerDlg.h"


// CKeyUpdateDlg dialog

IMPLEMENT_DYNAMIC(CKeyUpdateDlg, CDialog)
CKeyUpdateDlg::CKeyUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyUpdateDlg::IDD, pParent)
{
  m_pklNewKeys = NULL;
}

CKeyUpdateDlg::~CKeyUpdateDlg()
{
}

void CKeyUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_KEYS, m_lcKeys);
}


BEGIN_MESSAGE_MAP(CKeyUpdateDlg, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDC_BTN_KEYMANAGER, OnBnClickedBtnKeymanager)
END_MESSAGE_MAP()


// CKeyUpdateDlg message handlers

void CKeyUpdateDlg::OnBnClickedOk()
{
  for (CKey* key = m_pklNewKeys->GetRoot(); NULL != key; key = key->Next())
  {
    theApp.m_keyHolder.RegistryAddKey(key);
  }
  
  OnOK();
}

BOOL CKeyUpdateDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  SetWindowText(sPRG_KEYUPDATE_CAPTION);

  if (m_pklNewKeys == NULL)
  {
    theApp.Report(_T("BOOL CKeyUpdateDlg::OnInitDialog() ")
    _T("{ (m_pklNewKeys == NULL) }"));
    return FALSE;
  }

  LoadKeys();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CKeyUpdateDlg::LoadKeys()
{
  LVCOLUMN lvc;
  lvc.mask = LVCF_FMT;
  lvc.fmt = LVCFMT_JUSTIFYMASK & LVCFMT_LEFT;

  m_lcKeys.InsertColumn(0, _T("Name"), lvc.fmt);
  m_lcKeys.InsertColumn(1, sPRG_KEY_COMMENT, lvc.fmt);
  m_lcKeys.InsertColumn(2, sPRG_KEY_EXTENSIONS, lvc.fmt);
  m_lcKeys.InsertColumn(3, sPRG_KEY_CSO, lvc.fmt);
  m_lcKeys.InsertColumn(4, sPRG_KEY_CSOTYPE, lvc.fmt);
  m_lcKeys.InsertColumn(5, sPRG_KEY_SKIPSCOPE, lvc.fmt);
  m_lcKeys.InsertColumn(6, sPRG_KEY_SIGNATURE
    _T(" (shown only first 55 bytes)"), lvc.fmt);

  m_lcKeys.SetExtendedStyle(
     LVS_EX_FULLROWSELECT    // row is selecteble
     | LVS_EX_GRIDLINES      // show grid lines
     | LVS_EX_HEADERDRAGDROP // movable headers
     );

  m_lcKeys.SetItemCount(m_pklNewKeys->Count());

  DWORD indx = 0;
  CKey* key = NULL;
  for (key = m_pklNewKeys->GetRoot();
    (NULL != key); key = key->Next(), indx++)
  {
    m_lcKeys.InsertItem(LVIF_TEXT, indx, key->m_sName, 0, 0, 0, 0L);
    m_lcKeys.SetItemText(indx, 1, key->m_sComment); 
    m_lcKeys.SetItemText(indx, 2, key->m_sExtensions);
    m_lcKeys.SetItemText(indx, 3, CConvert::NumberToString(key->m_dwCSO));
    m_lcKeys.SetItemText(indx, 4, key->GetCSOTypeAsString());
    m_lcKeys.SetItemText(indx, 5, CConvert::NumberToString(key->m_dwSkipScope));
    m_lcKeys.SetItemText(indx, 6, key->GetSignAsStringTrancated(55));
  }

  m_lcKeys.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
  m_lcKeys.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
  m_lcKeys.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
  m_lcKeys.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
  m_lcKeys.SetColumnWidth(4, LVSCW_AUTOSIZE_USEHEADER);
  m_lcKeys.SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
  m_lcKeys.SetColumnWidth(6, LVSCW_AUTOSIZE_USEHEADER);

}

void CKeyUpdateDlg::OnBnClickedBtnKeymanager()
{
  theApp.OnManageKeys();
}
