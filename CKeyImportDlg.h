#pragma once
#include "afxwin.h"
#include "resource.h"
#include ".\CKeyHolder.h"

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// CKeyImportDlg dialog
//
class CKeyImportDlg : public CDialog
{
	DECLARE_DYNAMIC(CKeyImportDlg)


private:
  CString m_sImportFile;    // filename of the import file

private:
  enum IMPORT_TYPE          // import type
  {
    IMPORT_TYPE_REPLACE_THE_SAME = 0,
    IMPORT_TYPE_SKIP_THE_SAME,
    IMPORT_TYPE_IMPORT_ON_CLEAR
  };

public:
  CString m_sAppPath;       // Application path

  CKeyHolder* m_pKeyHolder; // Ptr on app-keyHolder

  CString m_eventLog;       // Event-Log Value

  CEdit m_event;            // Event-Log Control

  CButton m_btnImport;      // Button for Improt command

  CButton m_btnClose;       // Button for Close command

  CComboBox m_cmbImportType;// Combo Box of Import type (Drop List)


public:
	CKeyImportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKeyImportDlg();

// Dialog Data
	enum { IDD = IDD_KEYIMPORT_DLG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


private:
  // Display Message in log edit control window.
  void Log(CString _str);

  // Load file dialog
  CString GetOpenFileFilename();

  // Load strings for `Import Type`
  void InitImportTypeCombo();

  // Get Import Type string by index
  CString GetImportTypeString(DWORD _dwIndex);

  // Log Selected Import Type String
  void LogImportTypeChange();


	DECLARE_MESSAGE_MAP()

public:
  virtual BOOL OnInitDialog();

  // On BTN [Open File] - Select Filename for the import.
  afx_msg void OnBnClickedBtnOpenfile();

  // On BTN [Import] - Import the keys to the file.
  afx_msg void OnBnClickedBtnImport();

  // On BTN [Close] - Close the dialog.
  afx_msg void OnBnClickedBtnClose();

  // On Import Type selection change.
  afx_msg void OnImportType_SelChange();
};


//? EOF
