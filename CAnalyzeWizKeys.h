#pragma once
#include "afxwin.h"
#include ".\CAnalyzeData.h"

// CAnalyzeWizKeys dialog

class CAnalyzeWizKeys : public CPropertyPage
{
	DECLARE_DYNAMIC(CAnalyzeWizKeys)

private:
  CAnalyzeData* m_pDocData;

public:
	CAnalyzeWizKeys();
  CAnalyzeWizKeys(CAnalyzeData* _pDocData);
	virtual ~CAnalyzeWizKeys();

// Dialog Data
	enum { IDD = IDD_ANAL_WIZPAGE_KEYS };

private:
  void ControlAdvance();
  void LoadKeyNames();
  CString GetSelectedKeyName(CListBox* _plb);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnSetActive();
  afx_msg void OnBnClickedBtnAdd();
  afx_msg void OnBnClickedBtnDel();
  afx_msg void OnBnClickedBtnAddall();
  afx_msg void OnBnClickedBtnDelall();
  // available keys
  CListBox m_lbAvailable;
  // selected keys
  CListBox m_lbSelected;
  afx_msg void OnLbnDblclkListAvailable();
  afx_msg void OnLbnDblclkListSelected();
  CButton m_btnAdd;
  CButton m_btnDel;
  CButton m_btnAddAll;
  CButton m_btnDelAll;
};
