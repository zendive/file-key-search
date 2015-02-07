#pragma once

#include "resource.h"       // main symbols
#include "afxwin.h"

// CReportDlg dialog

class CReportDlg : public CDialog
{
	DECLARE_DYNAMIC(CReportDlg)

public:
  bool m_bTerminate;

public:
	CReportDlg(CWnd* pParent = NULL);   // standard constructor

  // create dialog with the message
  CReportDlg(CString _sMessage);
	virtual ~CReportDlg();

// Dialog Data
	enum { IDD = IDD_REPORTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  
  // The message container.
  CString m_txtMessage;

  // System Last Error Message container.
  CString m_txtLastError;
  // Message container controll
  CEdit m_editMsgCtrl;

  afx_msg void OnBnClickedBtnTerminate();
};
