// CAView.cpp : implementation of the CAView class
//

#include "stdafx.h"
#include ".\CApp.h"

#include ".\CDoc.h"
#include ".\CAView.h"
#include ".\CKview.h"
#include ".\CMediaDlg.h"
#include ".\CNewExtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAView

IMPLEMENT_DYNCREATE(CAView, CListView)

BEGIN_MESSAGE_MAP(CAView, CListView)
  ON_WM_STYLECHANGED()
  ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRclick)
  ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
  ON_NOTIFY_REFLECT(NM_RETURN, OnNMDblclk)
  ON_COMMAND(ID_MEDIA_VIEW,           ShowSelectedKeyMedia)
  ON_COMMAND(ID_MEDIA_REMOVE,         OnMediaRemove)
  ON_COMMAND(ID_MEDIA_SET_EXTENSION,  OnMediaExtension)
  ON_COMMAND(ID_MEDIA_PROPERTIES,     OnMediaProperties)
  ON_COMMAND(ID_MEDIA_COPY_NAME,      OnMediaCopyName)
  ON_COMMAND(ID_MEDIA_COPY_OFFSET,    OnMediaCopyOffset)
  ON_COMMAND(ID_MEDIA_COPY_SIZE,      OnMediaCopySize)
  ON_COMMAND(ID_MEDIA_EXTRACT,        OnMediaExtract)
END_MESSAGE_MAP()

// CAView construction/destruction

CAView::CAView()
: m_list(GetListCtrl())
{
  m_pDoc = NULL;
  m_Table = CAView::TABLE_NONE;
  m_pCurKey = NULL;
}

CAView::~CAView()
{
  if (m_pDoc)
  {
    m_pDoc->m_pAView = NULL;
  }
}

BOOL CAView::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.style |= LVS_SHOWSELALWAYS | LVS_REPORT;

  return CListView::PreCreateWindow(cs);
}

void CAView::OnInitialUpdate()
{
  CListView::OnInitialUpdate();

  m_pDoc = GetDocument();
  m_pDoc->m_pAView = this;

  m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT);
  m_list.SetBkColor(::GetSysColor(COLOR_3DFACE));

  ShowSummaryForKeys(&m_pDoc->m_DocData.m_klUsed);
}

void CAView::ClearAll()
{
  m_list.SetBkColor(::GetSysColor(COLOR_3DFACE));
  ResetItemData();
  m_list.DeleteAllItems();

  if (m_Table == CAView::TABLE_MEDIA)
  {
    m_list.DeleteColumn(CAView::MEDIA_COL_SIZE);
    m_list.DeleteColumn(CAView::MEDIA_COL_OFFSET);
    m_list.DeleteColumn(CAView::MEDIA_COL_NAME);
  }
  else if (m_Table == CAView::TABLE_SUMMARY)
  {
    m_list.DeleteColumn(CAView::SUMMARY_COL_FILLFACTOR);
    m_list.DeleteColumn(CAView::SUMMARY_COL_TOTALSIZE);
    m_list.DeleteColumn(CAView::SUMMARY_COL_COUNT);
    m_list.DeleteColumn(CAView::SUMMARY_COL_KEYNAME);
  }

  m_Table = CAView::TABLE_NONE;
  m_pCurKey = NULL;
}

// CAView diagnostics

#ifdef _DEBUG
void CAView::AssertValid() const
{
  CListView::AssertValid();
}

void CAView::Dump(CDumpContext& dc) const
{
  CListView::Dump(dc);
}

CDoc* CAView::GetDocument() const // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
  return (CDoc*)m_pDocument;
}
#endif //_DEBUG


// CAView message handlers
void CAView::OnStyleChanged(int /*nStyleType*/, LPSTYLESTRUCT /*lpStyleStruct*/)
{
  //TODO: add code to react to the user changing the view style of your window
  Default();
}

void CAView::ResetItemData()
{
  int item = -1;
  item = m_list.GetNextItem(item, LVNI_ALL);
  while (item != -1)
  {
    m_list.SetItemData(item, NULL);
    item = m_list.GetNextItem(item, LVNI_ALL);
  }
}

void CAView::InitMediaTable()
{
  if (CAView::TABLE_MEDIA == m_Table)
  { // already initialazed
    return;
  }

  ClearAll();
  m_Table = CAView::TABLE_MEDIA;

  m_list.InsertColumn(CAView::MEDIA_COL_NAME, _T("Media"), LVCFMT_LEFT, 100);
  m_list.InsertColumn(CAView::MEDIA_COL_OFFSET, _T("Offset"), LVCFMT_RIGHT, 80);
  m_list.InsertColumn(CAView::MEDIA_COL_SIZE, _T("Size"), LVCFMT_RIGHT, 80);

  m_list.SetBkColor(RGB(255,255,255));
}

void CAView::InitSummaryTable()
{
  if (CAView::TABLE_SUMMARY == m_Table)
  { // already initialazed
    return;
  }

  ClearAll();
  m_Table = CAView::TABLE_SUMMARY;

  m_list.InsertColumn(CAView::SUMMARY_COL_KEYNAME, _T("Key name"), LVCFMT_LEFT, 100);
  m_list.InsertColumn(CAView::SUMMARY_COL_COUNT, _T("Found"), LVCFMT_RIGHT, 80);
  m_list.InsertColumn(CAView::SUMMARY_COL_TOTALSIZE, _T("Total Size"), LVCFMT_RIGHT, 80);
  m_list.InsertColumn(CAView::SUMMARY_COL_FILLFACTOR, _T("Fill factor"), LVCFMT_RIGHT, 80);

  m_list.SetBkColor(RGB(255,255,255));
  m_list.SetTextBkColor(RGB(255,255,255));
  m_list.SetTextColor(RGB(0,0,0));
}

void CAView::ShowMediaForKey(CKey* _pKey)
{
  if (m_pDoc->m_WorkState.IsActive()
    || NULL == _pKey || _pKey->m_MediaSet.IsEmpty())
  {
    return;
  }

  InitMediaTable();
  m_list.DeleteAllItems();
  m_pCurKey = _pKey;
  DWORD dwMediaCount = (DWORD) _pKey->m_MediaSet.GetCount();

  m_list.SetItemCount(/*(int)*/ dwMediaCount);

  CMedia* pMedia = NULL;
  POSITION pos;
  int ret = 0;

  pos = _pKey->m_MediaSet.GetHeadPosition();
  for (DWORD i = 0; i < dwMediaCount; i++)
  {
    pMedia = _pKey->m_MediaSet.GetNext(pos);

    ret = m_list.InsertItem(i, pMedia->m_sName);

    m_list.SetItemText(i, CAView::MEDIA_COL_OFFSET
      , CConvert::NumberToHumanString(pMedia->m_qwOffset));

    m_list.SetItemText(i, CAView::MEDIA_COL_SIZE
      , CConvert::NumberToHumanString(pMedia->m_qwSize));

    // store `pMedia` data ptr
    m_list.SetItemData(i, (DWORD_PTR) pMedia);
  }
  
  m_list.SetColumnWidth(CAView::MEDIA_COL_NAME, LVSCW_AUTOSIZE_USEHEADER);
  m_list.SetColumnWidth(CAView::MEDIA_COL_OFFSET, LVSCW_AUTOSIZE_USEHEADER);
  m_list.SetColumnWidth(CAView::MEDIA_COL_SIZE, LVSCW_AUTOSIZE);
}

void CAView::ShowSummaryForKeys(CKeyList* _pKeyList)
{
  RETURNIF(m_pDoc->m_WorkState.IsActive() || NULL == _pKeyList);

  InitSummaryTable();
  m_list.DeleteAllItems();
  m_pCurKey = NULL;

  DWORD i;
  CKey* _pKey;
  ULARGE_INTEGER qwTotalMediaSize;
  double dFillPerc = 0.0;
  CString sItem;

  for (_pKey = _pKeyList->GetRoot(), i = 0; _pKey != NULL; _pKey = _pKey->Next(), i++)
  {
    if (_pKey->m_MediaSet.IsEmpty())
    {
      --i;
      continue;
    }

    // Key Name
    m_list.InsertItem(i, _pKey->m_sName);
    
    // Pointer to Key
    m_list.SetItemData(i, (DWORD_PTR) _pKey);

    // Media Count
    m_list.SetItemText(i, CAView::SUMMARY_COL_COUNT
      , CConvert::NumberToHumanString((DWORD) _pKey->m_MediaSet.GetCount()) );

    // Media Total Size
    qwTotalMediaSize = _pKey->GetTotalMediaSize();
    m_list.SetItemText(i, CAView::SUMMARY_COL_TOTALSIZE
      , CConvert::NumberToHumanString(qwTotalMediaSize));

    // Fill Factor
    if (m_pDoc->m_DocData.m_qwFileSize.QuadPart != 0)
    {
      dFillPerc = (((double) qwTotalMediaSize.QuadPart
        / (double) m_pDoc->m_DocData.m_qwFileSize.QuadPart) * 100.0);

      sItem.Format(_T("%02.1f%%"), dFillPerc);
    }
    else
    {
      sItem = _T("\"unknown\"");
    }
    m_list.SetItemText(i, CAView::SUMMARY_COL_FILLFACTOR, sItem);
  }

  m_list.SetColumnWidth(CAView::SUMMARY_COL_KEYNAME, LVSCW_AUTOSIZE_USEHEADER);
  m_list.SetColumnWidth(CAView::SUMMARY_COL_COUNT, LVSCW_AUTOSIZE_USEHEADER);
  m_list.SetColumnWidth(CAView::SUMMARY_COL_TOTALSIZE, LVSCW_AUTOSIZE_USEHEADER);
  m_list.SetColumnWidth(CAView::SUMMARY_COL_FILLFACTOR, LVSCW_AUTOSIZE_USEHEADER);
}

void CAView::OnNMRclick(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
  *pResult = 0;
  RETURNIF(CAView::TABLE_NONE == m_Table);
  // no selection
  RETURNIF(-1 == m_list.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED));

  // check if it's multiselection
  bool bMultiSelect = false;
  UINT uiSelected = m_list.GetSelectedCount();
  if (uiSelected > 1)
  {
    bMultiSelect = true;
  }

  POINT cPoint;           // cursor position point
  RETURNIF(!GetCursorPos(&cPoint));

  // create menu object
  CMenu menu;
  RETURNIF(!menu.LoadMenu(IDR_MENU_MEDIA));

  if (CAView::TABLE_SUMMARY == m_Table)
  {
    if (bMultiSelect)
    {
      menu.GetSubMenu(m_Table)->EnableMenuItem(
        ID_MEDIA_VIEW, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }

    if (bMultiSelect || m_pDoc->m_WorkState.IsActive())
    {
      menu.GetSubMenu(m_Table)->EnableMenuItem(
        ID_MEDIA_SET_EXTENSION, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }

    if (m_pDoc->m_WorkState.IsActive())
    {
      menu.GetSubMenu(m_Table)->EnableMenuItem(
        ID_MEDIA_EXTRACT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }

    menu.GetSubMenu(m_Table)->SetDefaultItem(ID_MEDIA_VIEW);
  }
  else if (CAView::TABLE_MEDIA == m_Table)
  {
    if (bMultiSelect)
    {
      menu.GetSubMenu(m_Table)->EnableMenuItem(
        ID_MEDIA_COPY_NAME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
      menu.GetSubMenu(m_Table)->EnableMenuItem(
        ID_MEDIA_COPY_OFFSET, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
      menu.GetSubMenu(m_Table)->EnableMenuItem(
        ID_MEDIA_COPY_SIZE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }

    if (bMultiSelect || m_pDoc->m_WorkState.IsActive())
    {
      menu.GetSubMenu(m_Table)->EnableMenuItem(
        ID_MEDIA_PROPERTIES, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }

    if (m_pDoc->m_WorkState.IsActive())
    {
      menu.GetSubMenu(m_Table)->EnableMenuItem(
        ID_MEDIA_EXTRACT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }

    menu.GetSubMenu(m_Table)->SetDefaultItem(ID_MEDIA_PROPERTIES);
  }

  CBitmap bmpProperties; bmpProperties.LoadBitmap(IDB_MEDIA_PROPERTIES);
  CBitmap bmpExtract; bmpExtract.LoadBitmap(IDB_MEDIA_EXTRACT);
  CBitmap bmpRemove; bmpRemove.LoadBitmap(IDB_MEDIA_REMOVE);

  menu.GetSubMenu(m_Table)->SetMenuItemBitmaps(
    ID_MEDIA_PROPERTIES, MF_BYCOMMAND, &bmpProperties, NULL);
  menu.GetSubMenu(m_Table)->SetMenuItemBitmaps(
    ID_MEDIA_EXTRACT, MF_BYCOMMAND, &bmpExtract, NULL);
  menu.GetSubMenu(m_Table)->SetMenuItemBitmaps(
    ID_MEDIA_REMOVE, MF_BYCOMMAND, &bmpRemove, NULL);

  menu.GetSubMenu(m_Table)->TrackPopupMenu(TPM_RIGHTBUTTON,
    cPoint.x - 2, cPoint.y - 2, this);

  *pResult = 1;
}

void CAView::OnMediaRemove()
{
  RETURNIF(m_pDoc->m_WorkState.IsActive() || CAView::TABLE_NONE == m_Table
    || (CAView::TABLE_MEDIA == m_Table && NULL == m_pCurKey));
  // no selection
  RETURNIF((-1) == m_list.GetNextItem(-1, LVNI_SELECTED));

  int iItem = (-1);
  if (CAView::TABLE_MEDIA == m_Table)
  { // Selected one or more from the Media items
    CMedia* pMedia = NULL;

    // find media position and remove it from the Set
    while ((iItem = m_list.GetNextItem(iItem, LVNI_SELECTED)) != (-1))
    {
      pMedia = (CMedia*) m_list.GetItemData(iItem);
      RETURNIF(NULL == pMedia);

      POSITION posMedia = m_pCurKey->m_MediaSet.Find(pMedia);
      m_pCurKey->m_MediaSet.RemoveAt(posMedia);
      delete pMedia;
      pMedia = NULL;
    }

    // update tree information for this key
    UINT uiSelected = m_list.GetSelectedCount();
    m_pDoc->m_pKView->RemoveFromKey(m_pCurKey, uiSelected);
  }
  else if (CAView::TABLE_SUMMARY == m_Table)
  { // Selected one or more from the Summary table keys
    CKey* pKey = NULL;

    // find media position and remove it from the Set
    while ((iItem = m_list.GetNextItem(iItem, LVNI_SELECTED)) != (-1))
    {
      pKey = (CKey*) m_list.GetItemData(iItem);
      RETURNIF(!pKey);
      
      DWORD dwMediaCount = (DWORD) pKey->m_MediaSet.GetCount();
      pKey->DeleteMediaSet();

      // update tree information for this key
      m_pDoc->m_pKView->RemoveFromKey(pKey, dwMediaCount);
    }
  }

  // clean the list from the selected items
  while ((iItem = m_list.GetNextItem((-1), LVNI_SELECTED)) != (-1))
  {
    m_list.SetItemData(iItem, NULL);
    m_list.DeleteItem(iItem);
  }

  m_pDoc->SetModifiedFlag(TRUE);
}

void CAView::OnMediaExtract()
{
  RETURNIF(m_pDoc->m_WorkState.IsActive() || CAView::TABLE_NONE == m_Table
    || (CAView::TABLE_MEDIA == m_Table && NULL == m_pCurKey));
  // no selection
  RETURNIF((-1) == m_list.GetNextItem(-1, LVNI_SELECTED));

  int iItem = (-1);
  // key-list to be populated with media for extraction
  CKeyList* pklForExtract = new CKeyList();
  CKey* pKey = NULL;
  CKey* pKeyNew = NULL;
  POSITION pos;
  DWORD dwMediaCount = 0;
  CMedia* pMedia = NULL;
  CMedia* pMediaNew = NULL;

  if (CAView::TABLE_MEDIA == m_Table)
  { // Selected one or more from the Media items
    pKeyNew = new CKey(*m_pCurKey);

    // find media position and add it to the extraction key
    while ((iItem = m_list.GetNextItem(iItem, LVNI_SELECTED)) != (-1))
    {
      pMedia = (CMedia *) m_list.GetItemData(iItem);
      RETURNIF(!pMedia);

      pMediaNew = new CMedia(pMedia->m_qwOffset.QuadPart
        , pMedia->m_qwSize.QuadPart, pMedia->m_sName);
      pKeyNew->m_MediaSet.AddTail(pMediaNew);
    }
    
    pklForExtract->AddKey(pKeyNew);
  }
  else if (CAView::TABLE_SUMMARY == m_Table)
  { // Selected one or more from the Summary table keys
    // find media position and add it to the extraction key
    while ((iItem = m_list.GetNextItem(iItem, LVNI_SELECTED)) != (-1))
    {
      pKey = (CKey*) m_list.GetItemData(iItem);
      RETURNIF(NULL == pKey);
      pKeyNew = new CKey(*pKey);

      dwMediaCount = (DWORD) pKey->m_MediaSet.GetCount();
      pos = pKey->m_MediaSet.GetHeadPosition();

      for (DWORD i = 0; i < dwMediaCount; i++)
      {
        pMedia = pKey->m_MediaSet.GetNext(pos);
        pMediaNew = new CMedia(pMedia->m_qwOffset.QuadPart
          , pMedia->m_qwSize.QuadPart, pMedia->m_sName);
        pKeyNew->m_MediaSet.AddTail(pMediaNew);
      }

      pklForExtract->AddKey(pKeyNew);
    }
  }

  m_pDoc->MediaExtractDlgStart(pklForExtract);
}

void CAView::OnNMDblclk(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
  RETURNIF(CAView::TABLE_NONE == m_Table || m_pDoc->m_WorkState.IsActive());

  if (CAView::TABLE_SUMMARY == m_Table)
  {
    ShowSelectedKeyMedia();
    *pResult = 1;
  }
  else if (CAView::TABLE_MEDIA == m_Table)
  {
    OnMediaProperties();
    *pResult = 1;
  }
}

void CAView::OnMediaProperties()
{
  RETURNIF(CAView::TABLE_MEDIA != m_Table || m_pDoc->m_WorkState.IsActive());
  // there must be single item selected
  RETURNIF(NULL == m_pCurKey || m_list.GetSelectedCount() != 1);

  // get selected item media data
  int iItem = m_list.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED);
  CMedia* pMedia = (CMedia *) m_list.GetItemData(iItem);
  RETURNIF(NULL == pMedia);
  
  // init media dialog
  CMediaDlg dlg(theApp.m_pMainWnd, m_pDoc->m_DocData.m_qwFileSize, m_pCurKey);
  dlg.m_sName = pMedia->m_sName;
  dlg.m_qwOffset = pMedia->m_qwOffset.QuadPart;
  dlg.m_qwSize = pMedia->m_qwSize.QuadPart;

  INT_PTR iRet = dlg.DoModal();
  RETURNIF(IDOK != iRet);
  RETURNIF(0 == pMedia->m_sName.Compare(dlg.m_sName)
    && pMedia->m_qwOffset.QuadPart == dlg.m_qwOffset
    && pMedia->m_qwSize.QuadPart == dlg.m_qwSize);

  // apply changes
  pMedia->m_sName = dlg.m_sName;
  pMedia->m_qwOffset.QuadPart = dlg.m_qwOffset;
  pMedia->m_qwSize.QuadPart = dlg.m_qwSize;

  // update item on the list
  m_list.SetItemText(iItem, CAView::MEDIA_COL_NAME, pMedia->m_sName);
  m_list.SetItemText(iItem, CAView::MEDIA_COL_OFFSET
    , CConvert::NumberToHumanString(pMedia->m_qwOffset));
  m_list.SetItemText(iItem, CAView::MEDIA_COL_SIZE
    , CConvert::NumberToHumanString(pMedia->m_qwSize));
  m_list.SetColumnWidth(CAView::MEDIA_COL_NAME, LVSCW_AUTOSIZE_USEHEADER);
  m_list.SetColumnWidth(CAView::MEDIA_COL_OFFSET, LVSCW_AUTOSIZE_USEHEADER);
  m_list.SetColumnWidth(CAView::MEDIA_COL_SIZE, LVSCW_AUTOSIZE);

  // flag modified
  m_pDoc->SetModifiedFlag(TRUE);
}

void CAView::ShowSelectedKeyMedia()
{
  RETURNIF(CAView::TABLE_SUMMARY != m_Table || m_pDoc->m_WorkState.IsActive());

  // there must be single item selected
  int iItem = m_list.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED);
  RETURNIF(-1 == iItem || m_list.GetSelectedCount() != 1);

  CKey* pKey = (CKey *) m_list.GetItemData(iItem);
  RETURNIF(NULL == pKey);

  m_pCurKey = pKey;
  m_pDoc->m_pKView->ShowKey(m_pCurKey);
}

void CAView::OnMediaExtension()
{
  RETURNIF(CAView::TABLE_NONE == m_Table || m_pDoc->m_WorkState.IsActive());
  RETURNIF(CAView::TABLE_MEDIA == m_Table && NULL == m_pCurKey);

  int iItem = m_list.GetNextItem(-1, LVNI_SELECTED);
  RETURNIF(-1 == iItem);  // some should be selected
  
  // for summary table only one item must be selected
  RETURNIF((CAView::TABLE_SUMMARY == m_Table) && (m_list.GetSelectedCount() > 1));

  CNewExtDlg dlg(theApp.m_pMainWnd);
  CKey* pKey = NULL;

  // set extension-set depending on current table
  if (CAView::TABLE_MEDIA == m_Table)
  { // Table MEDIA
    dlg.m_sExtensionSet = m_pCurKey->m_sExtensions;
  }
  else
  { // Table SUMMARY
    pKey = (CKey*) m_list.GetItemData(iItem);
    RETURNIF(NULL == pKey);

    dlg.m_sExtensionSet = pKey->m_sExtensions;
  }

  // get new extension
  INT_PTR res = dlg.DoModal();
  RETURNIF(IDOK != res);

  CString sName;
  CString sNewName;
  CMedia* pMedia = NULL;
  int iPos = 0;

  // set new extension regarding to the source table
  if (CAView::TABLE_MEDIA == m_Table)
  { // Table MEDIA
    iItem = -1;
    while ((iItem = m_list.GetNextItem(iItem, LVNI_SELECTED)) != (-1))
    {
      pMedia = (CMedia *) m_list.GetItemData(iItem);
      RETURNIF(NULL == pMedia);

      iPos = pMedia->m_sName.ReverseFind(_T('.'));
      sNewName.Format(_T("%s%s"), pMedia->m_sName.Left(iPos + 1)
        , dlg.m_sSelected);

      if (NULL == m_pCurKey->GetMedia(sNewName))
      { // is unique
        pMedia->m_sName = sNewName;
        m_list.SetItemText(iItem, CAView::MEDIA_COL_NAME, sNewName);
      }
    }
  }
  else
  { // Table SUMMARY
    POSITION mediaPos = pKey->m_MediaSet.GetHeadPosition();
    while (mediaPos != NULL)
    {
      pMedia = (CMedia *) pKey->m_MediaSet.GetNext(mediaPos);
      if (pMedia != NULL)
      {
        iPos = pMedia->m_sName.ReverseFind(_T('.'));
        sNewName.Format(_T("%s%s"), pMedia->m_sName.Left(iPos + 1)
          , dlg.m_sSelected);

        if (NULL == pKey->GetMedia(sNewName))
        { // is unique
          pMedia->m_sName = sNewName;
        }
      }
    }
  }

  m_pDoc->SetModifiedFlag(TRUE);
}

void CAView::OnMediaCopyName()
{
  int iItem = m_list.GetNextItem(-1, LVNI_SELECTED);
  RETURNIF(-1 == iItem);  // some should be selected

  CMedia* pMedia = (CMedia *) m_list.GetItemData(iItem);
  RETURNIF(NULL == pMedia);
  theApp.StringToCliboard(pMedia->m_sName);
}

void CAView::OnMediaCopyOffset()
{
  int iItem = m_list.GetNextItem(-1, LVNI_SELECTED);
  RETURNIF(-1 == iItem);  // some should be selected

  CMedia* pMedia = (CMedia *) m_list.GetItemData(iItem);
  RETURNIF(NULL == pMedia);
  theApp.StringToCliboard(CConvert::NumberToString(pMedia->m_qwOffset));
}

void CAView::OnMediaCopySize()
{
  int iItem = m_list.GetNextItem(-1, LVNI_SELECTED);
  RETURNIF(-1 == iItem);  // some should be selected

  CMedia* pMedia = (CMedia *) m_list.GetItemData(iItem);
  RETURNIF(NULL == pMedia);
  theApp.StringToCliboard(CConvert::NumberToString(pMedia->m_qwSize));
}
