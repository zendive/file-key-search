// CAnalyzeWizKeys.cpp : implementation file
//

#include "stdafx.h"
#include ".\CApp.h"
#include ".\CAnalyzeWizKeys.h"


// CAnalyzeWizKeys dialog

IMPLEMENT_DYNAMIC(CAnalyzeWizKeys, CPropertyPage)

CAnalyzeWizKeys::CAnalyzeWizKeys()
	: CPropertyPage(CAnalyzeWizKeys::IDD)
{
}

CAnalyzeWizKeys::CAnalyzeWizKeys(CAnalyzeData* _pDocData)
	: CPropertyPage(CAnalyzeWizKeys::IDD)
{
  m_pDocData = _pDocData;
}

CAnalyzeWizKeys::~CAnalyzeWizKeys()
{
}

void CAnalyzeWizKeys::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_AVAILABLE, m_lbAvailable);
  DDX_Control(pDX, IDC_LIST_SELECTED, m_lbSelected);
  DDX_Control(pDX, IDC_BTN_ADD, m_btnAdd);
  DDX_Control(pDX, IDC_BTN_DEL, m_btnDel);
  DDX_Control(pDX, IDC_BTN_ADDALL, m_btnAddAll);
  DDX_Control(pDX, IDC_BTN_DELALL, m_btnDelAll);
}

BEGIN_MESSAGE_MAP(CAnalyzeWizKeys, CPropertyPage)
  ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
  ON_BN_CLICKED(IDC_BTN_DEL, OnBnClickedBtnDel)
  ON_BN_CLICKED(IDC_BTN_ADDALL, OnBnClickedBtnAddall)
  ON_BN_CLICKED(IDC_BTN_DELALL, OnBnClickedBtnDelall)
  ON_LBN_DBLCLK(IDC_LIST_AVAILABLE, OnLbnDblclkListAvailable)
  ON_LBN_DBLCLK(IDC_LIST_SELECTED, OnLbnDblclkListSelected)
END_MESSAGE_MAP()


// CAnalyzeWizKeys message handlers

BOOL CAnalyzeWizKeys::OnSetActive()
{
  if ((NULL == m_pDocData) || (0 == m_pDocData->m_pklAvailable->Count()))
  {
    theApp.Report(
      _T("BOOL CAnalyzeWizKeys::OnSetActive() { ")
      _T("((NULL == m_pDocData) || (0 == m_pDocData->m_pklAvailable->Count())) }"));

    exit(-1);
  }

  LoadKeyNames();
  ControlAdvance();

  return CPropertyPage::OnSetActive();
}

void CAnalyzeWizKeys::ControlAdvance()
{
  CPropertySheet* psheet = (CPropertySheet*) GetParent();

  if ((0 != m_pDocData->m_sFileName.GetLength())
    && (0 < m_pDocData->m_klUsed.Count()))
  {
    psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
  }
  else
  {
    psheet->SetWizardButtons(PSWIZB_BACK);
  }

  if (m_lbAvailable.GetCount() <= 0)
  {// no more to add
    m_btnAdd.EnableWindow(FALSE);
    m_btnAddAll.EnableWindow(FALSE);
  }
  else
  {
    m_btnAdd.EnableWindow(TRUE);
    m_btnAddAll.EnableWindow(TRUE);
  }

  if (m_lbSelected.GetCount() <= 0)
  {// no more to delete
    m_btnDel.EnableWindow(FALSE);
    m_btnDelAll.EnableWindow(FALSE);
  }
  else
  {
    m_btnDel.EnableWindow(TRUE);
    m_btnDelAll.EnableWindow(TRUE);
  }
}

void CAnalyzeWizKeys::OnBnClickedBtnAdd()
{
  CString sSlctdName = GetSelectedKeyName(&m_lbAvailable);
  if (0 == sSlctdName.GetLength())
  {
    return;
  }

  CKey* pKey = m_pDocData->m_pklAvailable->GetKey(sSlctdName);
  CKey* pNewKey = new CKey(*pKey);

  ::EnterCriticalSection(&m_pDocData->m_klUsed_lock);
  m_pDocData->m_klUsed.AddKey(pNewKey);
  ::LeaveCriticalSection(&m_pDocData->m_klUsed_lock);

  m_lbAvailable.DeleteString(m_lbAvailable.GetCurSel());
  m_lbSelected.AddString(sSlctdName);

  m_lbAvailable.SetCurSel(0);
  m_lbSelected.SetCurSel(0);

  ControlAdvance();
}

void CAnalyzeWizKeys::OnBnClickedBtnAddall()
{
  CKey* pNewKey = NULL;
  CKey* pKey = NULL;

  ::EnterCriticalSection(&m_pDocData->m_klUsed_lock);
  m_pDocData->m_klUsed.DeleteAll();
  ::LeaveCriticalSection(&m_pDocData->m_klUsed_lock);

  m_lbSelected.ResetContent();

  ::EnterCriticalSection(&m_pDocData->m_klUsed_lock);
  for (pKey = m_pDocData->m_pklAvailable->GetRoot(); pKey != NULL; pKey = pKey->Next())
  {
    pNewKey = new CKey(*pKey);
    m_pDocData->m_klUsed.AddKey(pNewKey);

    m_lbSelected.AddString(pNewKey->m_sName);
  }
  ::LeaveCriticalSection(&m_pDocData->m_klUsed_lock);

  m_lbAvailable.ResetContent();

  m_lbSelected.SetCurSel(0);
  ControlAdvance();
}

void CAnalyzeWizKeys::OnBnClickedBtnDel()
{
  CString sSlctdName = GetSelectedKeyName(&m_lbSelected);
  if (0 == sSlctdName.GetLength())
  {
    return;
  }

  ::EnterCriticalSection(&m_pDocData->m_klUsed_lock);
  m_pDocData->m_klUsed.Delete(sSlctdName);
  ::LeaveCriticalSection(&m_pDocData->m_klUsed_lock);

  m_lbSelected.DeleteString(m_lbSelected.GetCurSel());

  m_lbAvailable.AddString(sSlctdName);

  m_lbAvailable.SetCurSel(0);
  m_lbSelected.SetCurSel(0);

  ControlAdvance();
}

void CAnalyzeWizKeys::OnBnClickedBtnDelall()
{
  CKey* key = NULL;

  ::EnterCriticalSection(&m_pDocData->m_klUsed_lock);
  for (key = m_pDocData->m_klUsed.GetRoot(); NULL != key; key = key->Next())
  {
    m_lbAvailable.AddString(key->m_sName);
  }
  m_pDocData->m_klUsed.DeleteAll();
  ::LeaveCriticalSection(&m_pDocData->m_klUsed_lock);

  m_lbSelected.ResetContent();
  m_lbAvailable.SetCurSel(0);

  ControlAdvance();
}

void CAnalyzeWizKeys::OnLbnDblclkListAvailable()
{
  OnBnClickedBtnAdd();
}

void CAnalyzeWizKeys::OnLbnDblclkListSelected()
{
  OnBnClickedBtnDel();
}

void CAnalyzeWizKeys::LoadKeyNames()
{
  m_lbAvailable.ResetContent();
  m_lbSelected.ResetContent();

  CKey* key = NULL;

  ::EnterCriticalSection(&m_pDocData->m_klUsed_lock);
  for (key = m_pDocData->m_pklAvailable->GetRoot(); NULL != key; key = key->Next())
  {
    if (NULL == m_pDocData->m_klUsed.GetKey(key->m_sName))
    {
      m_lbAvailable.AddString(key->m_sName);
    }
  }

  for (key = m_pDocData->m_klUsed.GetRoot(); NULL != key; key = key->Next())
  {
    m_lbSelected.AddString(key->m_sName);
  }
  ::LeaveCriticalSection(&m_pDocData->m_klUsed_lock);

  m_lbAvailable.SetCurSel(0);

  ControlAdvance();
}

CString CAnalyzeWizKeys::GetSelectedKeyName(CListBox* _plb)
{
  int iIndex = _plb->GetCurSel();
  int iCount = _plb->GetCount();

  CString sName(_T(""));
  int iBuffLen;

  if ((iIndex != LB_ERR) && (iCount > 0))
  {
    iBuffLen = _plb->GetTextLen(iIndex);
    _plb->GetText(iIndex, sName.GetBuffer(iBuffLen));
    sName.ReleaseBuffer();
  }

  return sName;
}
