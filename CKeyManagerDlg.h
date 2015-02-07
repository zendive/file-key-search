#pragma once
#include <afxwin.h>
#include "resource.h"
#include ".\CKeyHolder.h"

// CKeyManagerDlg dialog

class CKeyManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CKeyManagerDlg)

public:
  // key-list to manage
  CKeyList* m_pKeyList;
  // On init - first select this key
  CString m_sForKeyName;
  // changes was made to `m_pKeyList` data (tested outside this dialog)
  bool m_bHasChanges;         

private:
  CKey* m_curKey;             // current selected key
  CListBox m_lbNames;         // Key Names
  CEdit m_txtComment;         // Key Comment
  CEdit m_txtExt;             // Key Extensions
  CEdit m_txtCSO;             // Content Size Offset
  CButton m_radCSOType_Content;// CSO Type Content
  CButton m_radCSOType_Total; // CSO Type Total
  CEdit m_txtSkipScope;       // Skip Scope
  CEdit m_txtSignature;       // Key Signature
  CStatic m_txtKeyListTitle;
  CSpinButtonCtrl m_spinCSO;
  CSpinButtonCtrl m_spinSkipScope;
  CButton m_btnApply;

  void UpdateKeyList();
  CString GetCurKeyName();
  CKey* GetCurKey();
  void Display();
  bool Apply();
  void DisplayCSOType(const CKey* _key);
  DWORD ParseCSOType();
  // DEL: int ParseIntFrom(const CEdit *_editInt);
  bool IsCurKeyChanged();
  bool PromptApply();

public:
  // Dialog Data
	enum
  {
    IDD = IDD_MANAGER_DLG
  };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnCancel();

public:
	CKeyManagerDlg(CWnd* pParent);
	virtual ~CKeyManagerDlg();
  virtual BOOL OnInitDialog();

  DECLARE_MESSAGE_MAP()

  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedBtnNewKey();
  afx_msg void OnBnClickedBtnDeleteKey();
  afx_msg void OnBnClickedBtnApply();
  afx_msg void OnLbnDblclkListKeynames();
  afx_msg void OnLbnSelchangeListKeynames();
  afx_msg void SpinCSO_OnDeltaPos(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedOK();
};
