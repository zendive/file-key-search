// CNewExtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CApp.h"
#include "CNewExtDlg.h"
#include ".\cnewextdlg.h"


// CNewExtDlg dialog

IMPLEMENT_DYNAMIC(CNewExtDlg, CDialog)
CNewExtDlg::CNewExtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewExtDlg::IDD, pParent)
{
}

CNewExtDlg::~CNewExtDlg()
{
}

void CNewExtDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CMB_EXT, m_cmbExt);
}


BEGIN_MESSAGE_MAP(CNewExtDlg, CDialog)
END_MESSAGE_MAP()


// CNewExtDlg message handlers

BOOL CNewExtDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  SetWindowText(sPRG_NEWEXTDLG_CAPTION);
  LoadExtensions();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewExtDlg::OnOK()
{
  int iCur = m_cmbExt.GetCurSel();
  int iLen = m_cmbExt.GetLBTextLen(iCur);

  m_cmbExt.GetLBText(iCur, m_sSelected.GetBuffer(iLen));
  m_sSelected.ReleaseBuffer();

  CDialog::OnOK();
}

void CNewExtDlg::LoadExtensions()
{
  int iPos = 0;
  CString sExt;

  sExt = m_sExtensionSet.Tokenize(_T(";"), iPos);
  while (iPos != (-1))
  {
    m_cmbExt.AddString(sExt);
    sExt = m_sExtensionSet.Tokenize(_T(";"), iPos);
  }

  m_cmbExt.SetCurSel(0);
}

