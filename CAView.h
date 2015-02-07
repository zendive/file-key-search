#pragma once
#include ".\CKey.h"

class CAView: public CListView
{
  DECLARE_DYNCREATE(CAView)

private:
  // Media Table columns
  enum MEDIA
  {
    // media name indexed with counter number of occurrence
    MEDIA_COL_NAME = 0,
    // media offset in source file
    MEDIA_COL_OFFSET,
    // media size
    MEDIA_COL_SIZE
  };
  // Summary Table columns
  enum SUMMARY
  {
    // key-name the media-key
    SUMMARY_COL_KEYNAME = 0,
    // number of found media for this key
    SUMMARY_COL_COUNT,
    // sumary of media sizes for all medies found
    SUMMARY_COL_TOTALSIZE,
    // relation of total medies size to source file-size
    SUMMARY_COL_FILLFACTOR
  };
  // View's table types, should match to context submenu `IDR_MENU_MEDIA`
  enum TABLE
  {
    TABLE_MEDIA = 0, TABLE_SUMMARY = 1, TABLE_NONE
  };

  // View list table
  // item data: TABLE_SUMMARY -> CKey* || TABLE_MEDIA -> CMedia*
  CListCtrl& m_list;
  // pointer to view's document
  CDoc* m_pDoc;
  // type of the current view's table
  DWORD m_Table;
  // ptr on Owner key of the current media set
  CKey* m_pCurKey;

  void ShowSelectedKeyMedia();
  void ResetItemData();

public:
  // Clear this view
  void ClearAll();
  // Init media table
  void InitMediaTable();
  // Display media table
  void ShowMediaForKey(CKey* _key);
  // Init summary table
  void InitSummaryTable();
  // Display sumary table
  void ShowSummaryForKeys(CKeyList* _kl);

  CAView();

protected:
  virtual ~CAView();
  virtual void OnInitialUpdate();
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
  CDoc* GetDocument() const;

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions

DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
  afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnMediaRemove();
  afx_msg void OnMediaExtract();
  afx_msg void OnMediaProperties();
  afx_msg void OnMediaExtension();
  afx_msg void OnMediaCopyName();
  afx_msg void OnMediaCopyOffset();
  afx_msg void OnMediaCopySize();
};

#ifndef _DEBUG  // debug version in CAView.cpp
inline CDoc* CAView::GetDocument() const
{ return reinterpret_cast<CDoc*>(m_pDocument); }
#endif

