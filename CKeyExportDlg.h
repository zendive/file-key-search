#pragma once
#include "afxwin.h"
#include "resource.h"
#include ".\CKeyHolder.h"

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// CKeyExportDlg dialog
//
class CKeyExportDlg : public CDialog
{
	DECLARE_DYNAMIC(CKeyExportDlg)


private:
  CString m_sExportFile;    // filename of the export file


public:
  CString m_sAppPath;       // Application path

  CKeyHolder* m_pKeyHolder; // Ptr on app-keyHolder

  CButton m_btnExport;      // Button for Export command

  CButton m_btnClose;       // Close button

  CString m_eventLog;       // Event log Value

  CEdit m_event;            // Event log controll


public:
	CKeyExportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKeyExportDlg();

// Dialog Data
	enum { IDD = IDD_KEYEXPORT_DLG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


private:
  // Open Save to... Dialog. Ret: file-name to save to, or "" otherwise.
  CString GetSaveToFilename();

  // Display Message in log edit control window.
  void Log(CString _str);


	DECLARE_MESSAGE_MAP()

public:
  virtual BOOL OnInitDialog();

  // On BTN [Save to] - Select Filename for the export.
  afx_msg void OnBnClickedBtnSaveto();

  // On BTN [Export] - Export the keys to the file.
  afx_msg void OnBnClickedBtnExport();

  // On BTN [Close] - Close the dialog.
  afx_msg void OnBnClickedBtnClose();
};


//? EOF
