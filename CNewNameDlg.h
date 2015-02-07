#pragma once
#include "resource.h"
#include "afxwin.h"

// CNewNameDlg dialog

class CNewNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewNameDlg)
private:
  bool Validate();

public:
  // New Name
  CString m_sNewName;
  CEdit m_txtNewName;

	CNewNameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewNameDlg();

// Dialog Data
	enum { IDD = IDD_NEWKEYNAME_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnEnChangeTxtNewName();
};
