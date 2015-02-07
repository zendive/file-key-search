// CKView.cpp : implementation of the CKView class
//

#include "stdafx.h"
#include ".\CApp.h"
#include ".\CDoc.h"
#include ".\CKView.h"
#include ".\CAView.h"
#include ".\CKeyManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKView

IMPLEMENT_DYNCREATE(CKView, CTreeView)

// CKView diagnostics

#ifdef _DEBUG
void CKView::AssertValid() const
{
  CTreeView::AssertValid();
}

void CKView::Dump(CDumpContext& dc) const
{
  CTreeView::Dump(dc);
}

CDoc* CKView::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
  return (CDoc*)m_pDocument;
}
#endif //_DEBUG

BEGIN_MESSAGE_MAP(CKView, CTreeView)
  ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
  ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
  ON_NOTIFY_REFLECT(NM_RETURN, OnDoubleClick)
END_MESSAGE_MAP()

// CKView construction/destruction

CKView::CKView()
: m_tree(GetTreeCtrl())
{
  m_pDoc = NULL;

  m_ImageList.Create(16,16, ILC_COLOR16, 0, 4);
  CWinApp* app = ::AfxGetApp();
  m_ImageList.Add(app->LoadIcon(IDI_ICON_LEAF_RED));
  m_ImageList.Add(app->LoadIcon(IDI_ICON_LEAF_GREY));
  m_ImageList.Add(app->LoadIcon(IDI_ICON_KEY_GOLD));
  m_ImageList.Add(app->LoadIcon(IDI_ICON_KEY_GREY));
  m_ImageList.Add(app->LoadIcon(IDI_ICON_KEY_RED));

  m_hFound = NULL;
  m_hNotFound = NULL;
}

CKView::~CKView()
{
  if (m_pDoc)
  {
    m_pDoc->m_pKView = NULL;
  }
}

BOOL CKView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
  cs.style |= TVS_SHOWSELALWAYS;
  
  return CTreeView::PreCreateWindow(cs);
}

void CKView::OnInitialUpdate()
{
  m_pDoc = GetDocument();
  m_pDoc->m_pKView = this;

  LONG lTreeStyle = GetWindowLong(m_tree.m_hWnd, GWL_STYLE);
  lTreeStyle |= TVS_SHOWSELALWAYS | TVS_LINESATROOT | TVS_HASBUTTONS;

  SetWindowLong(m_tree.m_hWnd, GWL_STYLE, lTreeStyle);

  m_tree.SetImageList(&m_ImageList, TVSIL_NORMAL);

  CTreeView::OnInitialUpdate();
}

void CKView::ClearAll()
{
  m_tree.SetBkColor(::GetSysColor(COLOR_3DFACE));

  ResetItemData(m_hFound);
  ResetItemData(m_hNotFound);

  m_hFound = NULL;
  m_hNotFound = NULL;
}

void CKView::ResetItemData(HTREEITEM _hItem)
{
  while (_hItem != NULL)
  {
    m_tree.SetItemData(_hItem, NULL);

    if (m_tree.ItemHasChildren(_hItem))
    { // recursion
      this->ResetItemData(m_tree.GetChildItem(_hItem));
    }

    m_tree.DeleteItem(_hItem);
    _hItem = m_tree.GetNextSiblingItem(_hItem);
  }
}

HTREEITEM CKView::FindTreeItemByKey(CKey* _pKey)
{
  HTREEITEM hItem = m_tree.GetChildItem(m_hFound);

  while (hItem != NULL)
  {
    TVITEM item;
    SecureZeroMemory(&item, sizeof (TVITEM));

    item.mask = TVIF_HANDLE | TVIF_PARAM;
    item.hItem = hItem;   // that I have
    item.lParam = NULL;   // this I need

    BOOL bOK = m_tree.GetItem(/*out*/ &item);
    if ((TRUE == bOK) && (_pKey == (CKey *) item.lParam))
    {
      return hItem;
    }

    hItem = m_tree.GetNextSiblingItem(hItem);
  }

  return NULL;
}

void CKView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
  // called from CDocument::UpdateAllViews
  this->ReflectKeyList();
}

void CKView::ReflectKeyList()
{
  RETURNIF(NULL == m_pDoc || m_pDoc->m_WorkState.IsActive());
  CWaitCursor toggleWaitCursor;
  this->ClearAll();
  
  m_hFound = m_tree.InsertItem(sPRG_KEYTREE_FOUND, 0,0);
  m_hNotFound = m_tree.InsertItem(sPRG_KEYTREE_NOTFOUND, 1,1);
  
  HTREEITEM hItem;
  DWORD dwFound = 0;
  m_dwTotalFound = 0;

  ::EnterCriticalSection(&m_pDoc->m_DocData.m_klUsed_lock);
  CKeyList* pKeyList = &m_pDoc->m_DocData.m_klUsed;
  for (CKey* pKey = pKeyList->GetRoot(); NULL != pKey; pKey = pKey->Next())
  {
    dwFound = (DWORD) pKey->m_MediaSet.GetCount();

    if (dwFound == 0)
    {
      hItem = m_tree.InsertItem(pKey->m_sName, 3,3, m_hNotFound);
    }
    else
    {
      m_dwTotalFound += dwFound;
      CString str;
      str.Format(_T("%s: %d"), pKey->m_sName, dwFound);
      hItem = m_tree.InsertItem(str, 2,4, m_hFound);
    }

    m_tree.SetItemData(hItem, (INT_PTR) pKey);
  }
  ::LeaveCriticalSection(&m_pDoc->m_DocData.m_klUsed_lock);

  CString sFound;
  sFound.Format(_T("%s: %d"), sPRG_KEYTREE_FOUND, m_dwTotalFound);
  m_tree.SetItemText(m_hFound, sFound);

  m_tree.SetBkColor(RGB(255,255,255));

  // Set first selection
  
  m_tree.Expand(m_hNotFound, TVE_EXPAND);
  m_tree.Expand(m_hFound, TVE_EXPAND);
  m_tree.SetFocus();
  m_tree.SelectItem(m_hFound);
}

void CKView::RemoveFromKey(CKey* _pKey, DWORD _dwRemoved)
{
  RETURNIF(NULL == _pKey || 0 == _dwRemoved);

  HTREEITEM hItem = m_tree.GetSelectedItem();

  // update `m_dwTotalFound` counter and Total Found tree leaf
  m_dwTotalFound -= _dwRemoved;
  CString sFound;
  sFound.Format(_T("%s: %d"), sPRG_KEYTREE_FOUND, m_dwTotalFound);
  m_tree.SetItemText(m_hFound, sFound);

  if (hItem != m_hFound)
  { // removed some media from MEDIA Table
    DWORD dwMediaCount = (DWORD) _pKey->m_MediaSet.GetCount();
    if (dwMediaCount != 0)
    { // key still has some media
      CString str;
      str.Format(_T("%s: %d"), _pKey->m_sName, dwMediaCount);
      m_tree.SetItemText(hItem, str);
      return;
    }
  }
  else
  { // key removed from SUMMARY Table
    hItem = FindTreeItemByKey(_pKey);
  }

  // key is empty now - move it to "Not Found"
  m_tree.SetItemData(hItem, NULL);
  m_tree.DeleteItem(hItem);
  hItem = m_tree.InsertItem(_pKey->m_sName, 3,3, m_hNotFound, TVI_SORT);
  m_tree.SetItemData(hItem, (INT_PTR)_pKey);
  m_tree.Expand(m_hNotFound, TVE_EXPAND);
}

void CKView::OnTvnSelchanged(NMHDR* /*pNMHDR*/, LRESULT* /*pResult*/)
{
  RETURNIF(NULL == m_pDoc || NULL == m_pDoc->m_pAView || m_pDoc->m_WorkState.IsActive());

  CWaitCursor toggleWaitCursor;

  if (::TryEnterCriticalSection(&m_pDoc->m_DocData.m_klUsed_lock))
  {
    HTREEITEM hItem = m_tree.GetSelectedItem();
    CKey* pKey = (CKey *)m_tree.GetItemData(hItem);

    if (hItem == m_hFound || NULL != pKey && pKey->m_MediaSet.GetCount() != 0)
    {
      if (NULL == pKey)
      {
        m_pDoc->m_pAView->ShowSummaryForKeys(&m_pDoc->m_DocData.m_klUsed);
      }
      else
      {
        m_pDoc->m_pAView->ShowMediaForKey(pKey);
      }
    }
    else
    {
      m_pDoc->m_pAView->ClearAll();
    }

    ::LeaveCriticalSection(&m_pDoc->m_DocData.m_klUsed_lock);
  }else{
    AFXDUMP("!!!!!!!!!!!!!!!! ::TryEnterCriticalSection(&m_pDoc->m_DocData.m_klUsed_lock) !!!!!!!!!!!!!!!!\n");
  }
}

void CKView::ShowKey(CKey* _pKey)
{
  if (NULL == _pKey || m_pDoc->m_WorkState.IsActive()) { return; }

  HTREEITEM hItem = FindTreeItemByKey(_pKey);
  if (hItem != NULL)
  {
    m_tree.SetFocus();
    m_tree.Expand(m_hFound, TVE_EXPAND);
    m_tree.SelectItem(hItem);
  }
}

void CKView::OnDoubleClick(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
  if (NULL == m_pDoc || m_pDoc->m_WorkState.IsActive())
  {
    return;
  }
  m_pDoc->m_WorkState.m_bMakingKeys = true;
  
  HTREEITEM hItem = m_tree.GetSelectedItem();
  CKey* pKey = (CKey*)m_tree.GetItemData(hItem);

  if (NULL != pKey)
  {
    CKeyManagerDlg dlg(theApp.m_pMainWnd);
    dlg.m_sForKeyName = pKey->m_sName;

    ::EnterCriticalSection(&this->m_pDoc->m_DocData.m_klUsed_lock);
    dlg.m_pKeyList = new CKeyList(this->m_pDoc->m_DocData.m_klUsed);
    ::LeaveCriticalSection(&this->m_pDoc->m_DocData.m_klUsed_lock);

    INT_PTR iRet = dlg.DoModal();

    if (IDOK == iRet && dlg.m_bHasChanges)
    {
      ::EnterCriticalSection(&this->m_pDoc->m_DocData.m_klUsed_lock);
      this->ClearAll();
      this->m_pDoc->m_DocData.m_klUsed.Absorb(dlg.m_pKeyList);
      ::LeaveCriticalSection(&this->m_pDoc->m_DocData.m_klUsed_lock);

      this->m_pDoc->SetModifiedFlag(TRUE);
      m_pDoc->m_WorkState.m_bMakingKeys = false;
      this->ReflectKeyList();
    }

    delete dlg.m_pKeyList;
    *pResult = TRUE; // message processed
  }
  else
  {
    *pResult = FALSE;
  }
  
  m_pDoc->m_WorkState.m_bMakingKeys = false;
}
