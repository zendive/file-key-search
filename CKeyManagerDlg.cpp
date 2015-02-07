// CKeyManagerDlg.cpp : implementation file
//

#include "StdAfx.h"
#include ".\CApp.h"
#include ".\CKeyManagerDlg.h"
#include ".\CNewNameDlg.h"


// CKeyManagerDlg dialog

IMPLEMENT_DYNAMIC(CKeyManagerDlg, CDialog)


CKeyManagerDlg::CKeyManagerDlg(CWnd* pParent)
  : CDialog(CKeyManagerDlg::IDD, pParent)
  , m_sForKeyName(_T(""))
{
  m_pKeyList = NULL;
  m_curKey = NULL;
  m_bHasChanges = false;
}

CKeyManagerDlg::~CKeyManagerDlg()
{
}

void CKeyManagerDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_KEYNAMES, m_lbNames);
  DDX_Control(pDX, IDC_EDIT_COMMENT, m_txtComment);
  DDX_Control(pDX, IDC_EDIT_EXTENSIONS, m_txtExt);
  DDX_Control(pDX, IDC_EDIT_CSO, m_txtCSO);
  DDX_Control(pDX, IDC_RADIO_CSO_TYPE_CONTENT, m_radCSOType_Content);
  DDX_Control(pDX, IDC_RADIO_CSO_TYPE_TOTAL, m_radCSOType_Total);
  DDX_Control(pDX, IDC_EDIT_SKIP_SCOPE, m_txtSkipScope);
  DDX_Control(pDX, IDC_EDIT_SIGNATURE, m_txtSignature);
  DDX_Control(pDX, IDC_SPIN_CSO, m_spinCSO);
  DDX_Control(pDX, IDC_SPIN_SKIP_SCOPE, m_spinSkipScope);
  DDX_Control(pDX, IDC_BTN_APPLY, m_btnApply);
  DDX_Control(pDX, IDC_TXT_KEYLIST_TITLE, m_txtKeyListTitle);
}

BEGIN_MESSAGE_MAP(CKeyManagerDlg, CDialog)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_BN_CLICKED(IDC_BTN_NEW_KEY, OnBnClickedBtnNewKey)
  ON_BN_CLICKED(IDC_BTN_DELETE_KEY, OnBnClickedBtnDeleteKey)
  ON_BN_CLICKED(IDC_BTN_APPLY, OnBnClickedBtnApply)
  ON_LBN_DBLCLK(IDC_LIST_KEYNAMES, OnLbnDblclkListKeynames)
  ON_LBN_SELCHANGE(IDC_LIST_KEYNAMES, OnLbnSelchangeListKeynames)
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CSO, &CKeyManagerDlg::SpinCSO_OnDeltaPos)
  ON_BN_CLICKED(IDOK, &CKeyManagerDlg::OnBnClickedOK)
END_MESSAGE_MAP()

BOOL CKeyManagerDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  UpdateKeyList();
  
  if (m_sForKeyName.IsEmpty())
  {
    SetWindowText(sPRG_KEYMANAGERDLG_GLOBALCAPTION);
    m_lbNames.SetCurSel(0);
  }
  else
  {
    SetWindowText(sPRG_KEYMANAGERDLG_LOCALCAPTION);
    m_lbNames.SelectString((int) -1, m_sForKeyName);
  }

  Display();

  m_lbNames.SetFocus();
  return FALSE;
}

void CKeyManagerDlg::OnCancel()
{
  this->OnBnClickedCancel();
}

void CKeyManagerDlg::OnBnClickedCancel()
{
  m_bHasChanges = false;
  CDialog::OnCancel();
}

void CKeyManagerDlg::UpdateKeyList()
{
  if (m_pKeyList == NULL)
  {
    theApp.Report(
      _T("ERROR: void CKeyManagerDlg::UpdateKeyList() { (m_pKeyList == NULL) }"));
    return;
  }

  m_lbNames.ResetContent();

  CKey* key = NULL;
  for (key = m_pKeyList->GetRoot(); NULL != key; key = key->Next())
  {
    m_lbNames.AddString(key->m_sName);
  }

  CString str;
  str.Format(_T("%s: %d"), sPRG_KEYMANAGERDLG_KEYLISTTITLE, m_pKeyList->Count());
  m_txtKeyListTitle.SetWindowTextW(str);
}

CString CKeyManagerDlg::GetCurKeyName()
{
  int iIndex = m_lbNames.GetCurSel();
  int iCount = m_lbNames.GetCount();
  CString sName(_T(""));
  int iBuffLen;

  if ((iIndex != LB_ERR) && (iCount > 0))
  {
    iBuffLen = m_lbNames.GetTextLen(iIndex);
    m_lbNames.GetText(iIndex, sName.GetBuffer(iBuffLen));
    sName.ReleaseBuffer();
  }
  else
  {
    return _T("");
  }

  return sName;
}

CKey* CKeyManagerDlg::GetCurKey()
{
  if (m_pKeyList == NULL)
  {
    theApp.Report(
      _T("ERROR: CKey* CKeyManagerDlg::GetCurKey() ")
      _T("{ (m_pKeyList == NULL) }"));
    return NULL;
  }

  CString sName = GetCurKeyName();
  return m_pKeyList->GetKey(sName);
}

void CKeyManagerDlg::DisplayCSOType(const CKey* _key)
{
  if (_key == NULL)
  {
    theApp.Report(
      _T("ERROR: void CKeyManagerDlg::DisplayCSOType(CKey* _key) ")
      _T("{ (_key == NULL) }"));
    return;
  }

  if (_key->m_dwCSO == 0)
  {
    m_radCSOType_Content.SetCheck(BST_UNCHECKED);
    m_radCSOType_Content.EnableWindow(FALSE);
    m_radCSOType_Total.SetCheck(BST_UNCHECKED);
    m_radCSOType_Total.EnableWindow(FALSE);
  }
  else
  {
    m_radCSOType_Content.EnableWindow(TRUE);
    m_radCSOType_Total.EnableWindow(TRUE);

    switch (_key->m_dwCSOType)
    {
    case CKey::CSOTYPE_CONTENT_SIZE:
      m_radCSOType_Content.SetCheck(BST_CHECKED);
      m_radCSOType_Total.SetCheck(BST_UNCHECKED);
      break;

    case CKey::CSOTYPE_TOTAL_SIZE:
      m_radCSOType_Content.SetCheck(BST_UNCHECKED);
      m_radCSOType_Total.SetCheck(BST_CHECKED);
      break;
    }
  }
}

DWORD CKeyManagerDlg::ParseCSOType()
{
  if (m_radCSOType_Content.GetCheck() == BST_CHECKED)
  {
    return (DWORD) CKey::CSOTYPE_CONTENT_SIZE;
  }
  else if (m_radCSOType_Total.GetCheck() == BST_CHECKED)
  {
    return (DWORD) CKey::CSOTYPE_TOTAL_SIZE;
  }
  else
  {
    return (DWORD) CKey::CSOTYPE_UNASSIGNED;
  }
}

void CKeyManagerDlg::Display()
{
  if (m_pKeyList == NULL)
  {
    return;
  }

  m_curKey = GetCurKey();
  if (m_curKey == NULL)
  {// no selected key
    return;
  }

  CString sTemp;

  m_txtComment.SetLimitText(nPRG_MAX_KEYCOMMENT_SIZE - 1);
  m_txtComment.SetWindowText(m_curKey->m_sComment);

  m_txtExt.SetLimitText(nPRG_MAX_KEYEXTENSION_SIZE - 1);
  m_txtExt.SetWindowText(m_curKey->m_sExtensions);

  m_txtCSO.SetLimitText(5);
  sTemp.Format(_T("%d"), m_curKey->m_dwCSO);
  m_txtCSO.SetWindowText(sTemp);

  m_spinCSO.SetBuddy((CWnd *) &m_txtCSO);
  m_spinCSO.SetRange32(0, 0x7FFF);

  DisplayCSOType(m_curKey);

  m_txtSkipScope.SetLimitText(7);
  sTemp.Format(_T("%d"), m_curKey->m_dwSkipScope);
  m_txtSkipScope.SetWindowText(sTemp);

  m_spinSkipScope.SetBuddy((CWnd *) &m_txtSkipScope);
  m_spinSkipScope.SetRange32(0, 0x7FFF);

  m_txtSignature.SetLimitText((nPRG_MAX_KEYSIGNATURE_SIZE * 3) - 1);
  m_txtSignature.SetWindowText(m_curKey->GetSignAsString());
}

bool CKeyManagerDlg::Apply()
{
  if (m_curKey == NULL || m_pKeyList->Count() < 1)
  {
    theApp.Report(_T("At least one key should be defined!"));
    return false;
  }

  // temp key copy
  CKey key(*m_curKey);

  // signature
  try
  {
    CString sSign;
    m_txtSignature.GetWindowText(sSign);
    key.SetSignFromString(sSign);
  }
  catch (int _iErrPos)
  {
    CString sReport;
    sReport.Format(_T("Signature is invalid at character: {%d}"), _iErrPos);
    theApp.Report(sReport);

    m_txtSignature.SetFocus();
    m_txtSignature.SetSel(_iErrPos, _iErrPos + 1);
    return false;
  }
  catch (LPCTSTR _sErr)
  {
    theApp.Report(_sErr);
    return false;
  }

  //  comment
  m_txtComment.GetWindowText(key.m_sComment);

  // extensions
  m_txtExt.GetWindowText(key.m_sExtensions);
  if (key.m_sExtensions.IsEmpty())
  {
    theApp.Report(_T("Need Extension"));
    return false;
  }

  int iErrPos = key.m_sExtensions.FindOneOf(sPRG_KEY_EXTENSION_BADCHARS);
  if (iErrPos != -1)
  {
    CString sReport;
    sReport.Format(_T("Extensions is invalid at character: {%d}"), iErrPos);
    theApp.Report(sReport);

    m_txtExt.SetFocus();
    m_txtExt.SetSel(iErrPos, iErrPos + 1);
    return false;
  }

  // CSO
  key.m_dwCSO = m_spinCSO.GetPos32();
  if (0 != key.m_dwCSO
    && key.m_dwSignSize > key.m_dwCSO)
  {
    CString sReport;
    sReport.Format(_T("Offset in bounds of signature length (%d)!"), key.m_dwSignSize);
    theApp.Report(sReport);
    return false;
  }

  // CSO type
  key.m_dwCSOType = ParseCSOType();
  
  // skip scope
  key.m_dwSkipScope = m_spinSkipScope.GetPos32();

  // apply
  m_curKey->Copy(&key);
  return true;
}

bool CKeyManagerDlg::IsCurKeyChanged()
{
  while (true)
  {
    BREAKIF(m_curKey == NULL);
    BREAKIF(m_txtComment.GetModify() || m_txtExt.GetModify() || m_txtSignature.GetModify());
    BREAKIF((DWORD) m_spinCSO.GetPos32() != m_curKey->m_dwCSO);
    BREAKIF(ParseCSOType() != m_curKey->m_dwCSOType)
    BREAKIF((DWORD) m_spinSkipScope.GetPos32() != m_curKey->m_dwSkipScope);
    return false;
  }

  m_bHasChanges = true;
  return true;
}

void CKeyManagerDlg::OnBnClickedBtnNewKey()
{
  if ((m_pKeyList->Count() != 0 && IsCurKeyChanged()) && PromptApply() && !Apply())
  {
    return;
  }

  // Generate new unique name
  CKey* key = NULL;
  CString sNewName;

  for (DWORD i = 0; i <= nPRG_MAX_KEY_COUNT; i++)
  {
    sNewName.Format(_T("%s%d"), sPRG_KEYTEMPLATE_NAME, i);
    key = m_pKeyList->GetKey(sNewName);
    if (NULL == key)
    {
      break;
    }
  }

  CNewNameDlg dlg(this);
  dlg.m_sNewName = sNewName;

  INT_PTR ret;
  ret = dlg.DoModal();
  if (IDOK != ret)
  { // cancel new key creation
    return;
  }

  CString sName = dlg.m_sNewName.Trim();

  // Check new name uniqueness
  if (NULL != m_pKeyList->GetKey(sName))
  {
    ::MessageBox(m_hWnd
      , _T("Key already exists. Please rename it.")
      , sPRG_NAMECONFLICT_CAPTION
      , MB_ICONINFORMATION | MB_OK);

    OnBnClickedBtnNewKey();   // enter recursion
    return;
  }

  CKey* newKey = new CKey(sName);
  if (newKey == NULL)
  {
    theApp.Report(_T("Insufficient memory resources."));
    return;
  }

  m_pKeyList->AddKey(newKey);

  UpdateKeyList();
  m_lbNames.SelectString((int) -1, sName);
  Display();

  m_bHasChanges = true;
}

void CKeyManagerDlg::OnBnClickedBtnDeleteKey()
{
  CString sName = GetCurKeyName();
  if (sName.IsEmpty())
  { // no selected item
    return;
  }

  CString sMessage;
  sMessage.Format(_T("You are about to delete {%s}."), sName);

  if (MessageBox(sMessage, _T("Delete Key...")
    , MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
  { // cancel key delete
    return;
  }

  m_pKeyList->Delete(sName);

  UpdateKeyList();

  if (m_pKeyList->Count() != 0)
  {
    m_lbNames.SetCurSel(0);
    Display();
  }
  else
  {
    // clear key-data fields
    m_txtComment.SetWindowText(_T(""));
    m_txtExt.SetWindowText(_T(""));
    m_txtCSO.SetWindowText(_T(""));
    m_radCSOType_Content.SetCheck(BST_UNCHECKED);
    m_radCSOType_Content.EnableWindow(FALSE);
    m_radCSOType_Total.SetCheck(BST_UNCHECKED);
    m_radCSOType_Total.EnableWindow(FALSE);
    m_txtSkipScope.SetWindowText(_T(""));
    m_txtSignature.SetWindowText(_T(""));

    m_curKey = NULL;
  }

  m_bHasChanges = true;
}

void CKeyManagerDlg::OnLbnDblclkListKeynames()
{
  CNewNameDlg dlg(this);

  dlg.m_sNewName = m_curKey->m_sName;

  INT_PTR ret = dlg.DoModal();
  if (IDOK != ret)
  { // rename canceled
    return;
  }

  CString sName = dlg.m_sNewName.Trim();

  if (0 == sName.CompareNoCase(m_curKey->m_sName))
  {// name dosn't changed
    return;
  }

  // Check new name uniqueness
  if (NULL != m_pKeyList->GetKey(sName))
  {
    ::MessageBox(m_hWnd
      , _T("Key with that name is already exist.")
      _T("\nPlease, name it different.")
      , sPRG_NAMECONFLICT_CAPTION
      , MB_ICONHAND | MB_OK);

    OnLbnDblclkListKeynames();  // enter recursion for another try
    return;
  }

  m_curKey->m_sName = sName;

  UpdateKeyList();
  m_lbNames.SelectString((int) -1, sName);
  Display();

  m_bHasChanges = true;
}

void CKeyManagerDlg::OnLbnSelchangeListKeynames()
{
  if (IsCurKeyChanged() && PromptApply() && !Apply())
  { // abort new selection cause apply canceled or changed data is invalid
    return;
  }

  Display();
}

void CKeyManagerDlg::SpinCSO_OnDeltaPos(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

  // get new CSO
  int iCSO = pNMUpDown->iPos + pNMUpDown->iDelta;

  if ((iCSO == 0) || (iCSO > UD_MAXVAL))
  {// disable CSO Type
    m_radCSOType_Content.EnableWindow(FALSE);
    m_radCSOType_Total.EnableWindow(FALSE);

    m_radCSOType_Content.SetCheck(BST_UNCHECKED);
    m_radCSOType_Total.SetCheck(BST_UNCHECKED);
  }
  else
  {// enable CSO Type
    m_radCSOType_Content.EnableWindow(TRUE);
    m_radCSOType_Total.EnableWindow(TRUE);

    if (pNMUpDown->iPos == 0)
    {// previous value was 0? - set default radio box
      m_radCSOType_Content.SetCheck(BST_CHECKED);
    }
  }

  *pResult = 0;
}

void CKeyManagerDlg::OnBnClickedBtnApply()
{
  if (IsCurKeyChanged())
  {
    Apply();
  }
}

bool CKeyManagerDlg::PromptApply()
{
  int iRet;
  iRet = MessageBox(_T("Apply changes?"), _T("Key data was changed ...")
    , MB_OKCANCEL | MB_ICONQUESTION);
  return (iRet == IDOK);
}

void CKeyManagerDlg::OnBnClickedOK()
{
  if (IsCurKeyChanged() && !Apply())
  { // abort "save and close" cause invalid data
    return;
  }

  OnOK();
}
