// CMainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include ".\CApp.h"
#include ".\CMainFrame.h"
#include ".\CDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
  ON_WM_CREATE()
  ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
  ID_SEPARATOR,           // status line indicator
  ID_INDICATOR_CAPS,
  ID_INDICATOR_NUM,
  ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
  // TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  if (!m_wndToolBar.CreateEx(this
    , TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
    | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
    || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
  {
    TRACE0("Failed to create standard toolbar\n");
    return (-1);      // fail to create
  }

  if (!m_wndTBApp.CreateEx(this
    , TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_LEFT
    | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
    || !m_wndTBApp.LoadToolBar(IDR_TB_APP))
  {
    TRACE0("Failed to create IDR_TB_APP toolbar\n");
    return (-1);
  }

  //if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
  //  CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
  //{
  //  TRACE0("Failed to create dialogbar\n");
  //  return -1;		// fail to create
  //}

  //if (!m_wndReBar.Create(this) ||
  //  !m_wndReBar.AddBar(&m_wndToolBar) ||
  //  !m_wndReBar.AddBar(&m_wndDlgBar))
  //{
  //  TRACE0("Failed to create rebar\n");
  //  return -1;      // fail to create
  //}

  if (!m_wndStatusBar.Create(this) ||
    !m_wndStatusBar.SetIndicators(indicators,
    sizeof(indicators)/sizeof(UINT)))
  {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  if( !CMDIFrameWnd::PreCreateWindow(cs) )
    return FALSE;
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  // set main window width and heigh
  cs.cx = 600;
  cs.cy = 600;

  return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
  CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
  CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

// CMainFrame message handlers

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
  /*remove:
  if (nPRG_WM_ANALDLG_END == pMsg->message) {
    CDoc* pDocMsgOwner = (CDoc*) pMsg->lParam;
    if (NULL != pDocMsgOwner) {
      try {
        pDocMsgOwner->AnalyzeDialogEnd();
      } catch (...) { ; }
    }
    return TRUE;
  }
  */
  return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnClose()
{
  theApp.StoreCurrentOpenedDocuments();

  CMDIFrameWnd::OnClose();
}
