// CKView.h : interface of the CKView class
// The tree of the selected for analyze keys
//

#pragma once
#include ".\CKey.h"

class CDoc;

class CKView : public CTreeView
{
DECLARE_DYNCREATE(CKView)

private:
  CTreeCtrl& m_tree;
  // PTR on owner document
  CDoc* m_pDoc;
  // List of icons for the tree
  CImageList m_ImageList;
  // Default tree item for all found keys
  HTREEITEM m_hFound;
  // Default tree item for not found keys
  HTREEITEM m_hNotFound;
  // Total found media
  DWORD m_dwTotalFound;

protected:
  // Create from serialization only
  CKView();

  // SELECT hItem FROM tree WHERE lParam == _pKey
  HTREEITEM FindTreeItemByKey(CKey* _pKey);
  void ResetItemData(HTREEITEM _hItem);

public:
  virtual ~CKView();
  CDoc* GetDocument();
  // Clear this view
  void ClearAll();
  // Reload key list in tree
  void ReflectKeyList();
  // Select that key in tree & show it content in CAView
  void ShowKey(CKey* _pKey);
  // Remove that key from tree & update Total Found
  void RemoveFromKey(CKey* _pKey, DWORD _dwRemoved);

protected:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
  virtual void OnInitialUpdate(); // called first time after construct

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  DECLARE_MESSAGE_MAP()
  afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDoubleClick(NMHDR *pNMHDR, LRESULT *pResult);
};

#ifndef _DEBUG  // debug version in CKView.cpp
inline CDoc* CKView::GetDocument()
{ return reinterpret_cast<CDoc*>(m_pDocument); }
#endif
