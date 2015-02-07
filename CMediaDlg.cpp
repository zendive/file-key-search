// CMediaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CApp.h"
#include "CMediaDlg.h"

// CMediaDlg dialog

IMPLEMENT_DYNAMIC(CMediaDlg, CDialog)

CMediaDlg::CMediaDlg(
  CWnd* pParent
  , const ULARGE_INTEGER _qwFileSize
  , const CKey* const _pKey)
: CDialog(CMediaDlg::IDD, pParent)
  , m_qwFileSize(_qwFileSize)
  , m_pKey(_pKey)
{
  m_sName = _T("");
  m_qwOffset = 0;
  m_qwSize = 0;
}

CMediaDlg::~CMediaDlg()
{
}

void CMediaDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TXT_NAME, m_txtName);
  DDX_Control(pDX, IDC_CHECK_END, m_chkEnd);
  DDX_Control(pDX, IDC_TXT_OFFSET, m_txtOffset);
  DDX_Control(pDX, IDC_TXT_SIZE, m_txtSize);
  DDX_Text(pDX, IDC_TXT_OFFSET, m_qwOffset);
  DDX_Text(pDX, IDC_TXT_SIZE, m_qwSize);
  DDX_Text(pDX, IDC_TXT_NAME, m_sName);
}

BEGIN_MESSAGE_MAP(CMediaDlg, CDialog)
  ON_BN_CLICKED(IDOK, &CMediaDlg::OnBnClickedOk)
  ON_EN_CHANGE(IDC_TXT_NAME, &CMediaDlg::OnEnChangeTxtName)
  ON_BN_CLICKED(IDC_CHECK_END, &CMediaDlg::OnBnClickedCheckEnd)
  ON_EN_CHANGE(IDC_TXT_OFFSET, &CMediaDlg::OnEnChangeTxtOffset)
END_MESSAGE_MAP()

BOOL CMediaDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  SetWindowText(sPRG_MEDIADLG_CAPTION);

  m_txtName.SetLimitText(nPRG_MAX_MEDIANAME_SIZE);
  m_txtName.SetSel(m_sName.GetLength(), m_sName.GetLength());
  m_txtName.SetFocus();

  m_txtOffset.SetLimitText(19);
  m_txtSize.SetLimitText(19);
  // [19] 9223372036854775807 == 0x7fffffffffffffff

  // if media reaches the EOF - auto set that checkbox
  if ((m_qwOffset + m_qwSize) == m_qwFileSize.QuadPart)
  {
    m_chkEnd.SetCheck(BST_CHECKED);
    m_txtSize.SetReadOnly(TRUE);
  }

  return FALSE;
}

// CMediaDlg message handlers

void CMediaDlg::OnBnClickedOk()
{
  RETURNIF(!Validate());
  
  OnOK();
}

bool CMediaDlg::Validate()
{
  this->UpdateData(TRUE);

  while (true)
  {
    if (m_qwOffset > m_qwFileSize.QuadPart
      || m_qwSize  > m_qwFileSize.QuadPart
      || (m_qwOffset + m_qwSize) > m_qwFileSize.QuadPart)
    {
      theApp.Report(_T("Offset + Size should not exceed overall file size.")
        _T("\r\nHint: To adjust the Size so that it will include file content")
        _T(" from Offset till the End Of File - use [expand to EOF] checkbox."));
      break;
    }

    if (0 == m_qwSize)
    {
      theApp.Report(_T("Size can not be zero.\r\nHint: to remove media use context menu."));
      break;
    }

    return true;
  }

  return false;
}

void CMediaDlg::OnEnChangeTxtName()
{
  CString sName;
  
  m_txtName.GetWindowText(sName);
  int iPos = sName.FindOneOf(sPRG_MEDIA_FILENAME_BADCHARS);
  if (0 <= iPos)
  {
    sName.Remove(sName.GetAt(iPos));
    m_txtName.SetWindowText(sName);
    m_txtName.SetSel(iPos, iPos);
  }
}

void CMediaDlg::OnBnClickedCheckEnd()
{
  this->UpdateData(TRUE);
  bool bEnd = (BST_CHECKED == m_chkEnd.GetCheck());
  if (bEnd)
  {
    m_qwSize = m_qwFileSize.QuadPart - m_qwOffset;
    this->UpdateData(FALSE);
  }

  m_txtSize.SetReadOnly((BOOL) bEnd);
}

void CMediaDlg::OnEnChangeTxtOffset()
{
  OnBnClickedCheckEnd();
}
