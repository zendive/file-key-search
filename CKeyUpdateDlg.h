#pragma once
#include "afxwin.h"
#include ".\CKeyList.h"


// CKeyUpdateDlg dialog

class CKeyUpdateDlg : public CDialog
{
	DECLARE_DYNAMIC(CKeyUpdateDlg)

public:
  CKeyList* m_pklNewKeys;

private:
  void LoadKeys();

public:
	CKeyUpdateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKeyUpdateDlg();

// Dialog Data
	enum { IDD = IDD_KEYUPDATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  // list box of keys to update
  CListCtrl m_lcKeys;
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedBtnKeymanager();
};
