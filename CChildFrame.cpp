// CChildFrame.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "CApp.h"

#include "CChildFrame.h"
#include "CKView.h"
#include "CAView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
  ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
  ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
END_MESSAGE_MAP()


// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
  // TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
  // create splitter window
  if (!m_wndSplitter.CreateStatic(this, 1, 2))
  {
    return FALSE;
  }

  if (
    !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CKView), CSize(200, 100), pContext)
    ||
    !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CAView), CSize(100, 100), pContext)
    )
  {
    m_wndSplitter.DestroyWindow();
    return FALSE;
  }
  return TRUE;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
  if ( !CMDIChildWnd::PreCreateWindow(cs) )
  {
    return FALSE;
  }

  cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
    | FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

  return TRUE;
}


// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
  CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
  CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// CChildFrame message handlers
CAView* CChildFrame::GetRightPane()
{
  CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
  CAView* pView = DYNAMIC_DOWNCAST(CAView, pWnd);
  return pView;
}

void CChildFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
  if (!pCmdUI)
  {
    return;
  }

  // TODO: customize or extend this code to handle choices on the View menu.
  CAView* pView = GetRightPane(); 

  // if the right-hand pane hasn't been created or isn't a view, disable commands in our range
  if (pView == NULL)
  {
    pCmdUI->Enable(FALSE);
  }
  else
  {
    DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;
    // if the command is ID_VIEW_LINEUP, only enable command
    // when we're in LVS_ICON or LVS_SMALLICON mode
    if (pCmdUI->m_nID == ID_VIEW_LINEUP)
    {
      if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
      {
        pCmdUI->Enable();
      }
      else
      {
        pCmdUI->Enable(FALSE);
      }
    }
    else
    {
      // otherwise, use dots to reflect the style of the view
      pCmdUI->Enable();
      BOOL bChecked = FALSE;

      switch (pCmdUI->m_nID)
      {
      case ID_VIEW_DETAILS:
        bChecked = (dwStyle == LVS_REPORT);
        break;

      case ID_VIEW_SMALLICON:
        bChecked = (dwStyle == LVS_SMALLICON);
        break;

      case ID_VIEW_LARGEICON:
        bChecked = (dwStyle == LVS_ICON);
        break;

      case ID_VIEW_LIST:
        bChecked = (dwStyle == LVS_LIST);
        break;

      default:
        bChecked = FALSE;
        break;
      }

      pCmdUI->SetRadio(bChecked ? 1 : 0);
    }
  }
}

void CChildFrame::OnViewStyle(UINT nCommandID)
{
  // TODO: customize or extend this code to handle choices on the View menu.
  CAView* pView = GetRightPane();

  // if the right-hand pane has been created and is a CAView, process the menu commands...
  if (pView != NULL)
  {
    int nStyle = -1;

    switch (nCommandID)
    {
    case ID_VIEW_LINEUP:
      {
        // ask the list control to snap to grid
        CListCtrl& refListCtrl = pView->GetListCtrl();
        refListCtrl.Arrange(LVA_SNAPTOGRID);
      }
      break;

      // other commands change the style on the list control
    case ID_VIEW_DETAILS:
      nStyle = LVS_REPORT;
      break;

    case ID_VIEW_SMALLICON:
      nStyle = LVS_SMALLICON;
      break;

    case ID_VIEW_LARGEICON:
      nStyle = LVS_ICON;
      break;

    case ID_VIEW_LIST:
      nStyle = LVS_LIST;
      break;
    }

    // change the style; window will repaint automatically
    if (nStyle != -1)
    {
      pView->ModifyStyle(LVS_TYPEMASK, nStyle);
    }
  }
}
