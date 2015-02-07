#pragma once
#include "afxwin.h"


// CNewExtDlg dialog

class CNewExtDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewExtDlg)

public:
  CString m_sExtensionSet;
  CString m_sSelected;

private:
  void LoadExtensions();

public:
	CNewExtDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewExtDlg();

// Dialog Data
	enum { IDD = IDD_NEWEXT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  // Set of extensions
  CComboBox m_cmbExt;
protected:
  virtual void OnOK();
public:
  virtual BOOL OnInitDialog();
};
