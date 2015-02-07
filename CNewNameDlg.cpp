// CNewNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include ".\CApp.h"
#include ".\CNewNameDlg.h"

// CNewNameDlg dialog

IMPLEMENT_DYNAMIC(CNewNameDlg, CDialog)
CNewNameDlg::CNewNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewNameDlg::IDD, pParent)
  , m_sNewName(_T(""))
{
}

CNewNameDlg::~CNewNameDlg()
{
}

void CNewNameDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_NEWNAME, m_sNewName);
  DDX_Control(pDX, IDC_EDIT_NEWNAME, m_txtNewName);
}

BEGIN_MESSAGE_MAP(CNewNameDlg, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_EN_CHANGE(IDC_EDIT_NEWNAME, &CNewNameDlg::OnEnChangeTxtNewName)
END_MESSAGE_MAP()

BOOL CNewNameDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  SetWindowText(sPRG_NEWNAMEDLG_CAPTION);

  m_txtNewName.SetLimitText(nPRG_MAX_KEYNAME_SIZE);
  m_txtNewName.SetFocus();
  m_txtNewName.SetSel(m_sNewName.GetLength(), m_sNewName.GetLength());

  return FALSE;
}

void CNewNameDlg::OnBnClickedOk()
{
  RETURNIF(!Validate());

  OnOK();
}

bool CNewNameDlg::Validate()
{
  this->UpdateData(TRUE);

  while (true)
  {
    BREAKIF(-1 != m_sNewName.FindOneOf(sPRG_MEDIA_FILENAME_BADCHARS));
    return true;
  }

  return false;
}

void CNewNameDlg::OnEnChangeTxtNewName()
{
  CString sName;
  
  m_txtNewName.GetWindowText(sName);
  int iPos = sName.FindOneOf(sPRG_MEDIA_FILENAME_BADCHARS);
  if (0 <= iPos)
  {
    sName.Remove(sName.GetAt(iPos));
    m_txtNewName.SetWindowText(sName);
    m_txtNewName.SetSel(iPos, iPos);
  }
}
