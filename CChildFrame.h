// CChildFrame.h : interface of the CChildFrame class
//


#pragma once

class CAView;

class CChildFrame : public CMDIChildWnd
{
  DECLARE_DYNCREATE(CChildFrame)
public:
  CChildFrame();

  // Attributes
protected:
  CSplitterWnd m_wndSplitter;
public:

  // Operations
public:

  // Overrides
public:
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

  // Implementation
public:
  virtual ~CChildFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  CAView* GetRightPane();
  // Generated message map functions
protected:
  afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
  afx_msg void OnViewStyle(UINT nCommandID);
  DECLARE_MESSAGE_MAP()
};
