#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include ".\CKeyHolder.h"
#include ".\CKeyImportDlg.h"

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// CApp:
// See CApp.cpp for the implementation of this class
//
class CApp: public CWinApp
{
private:
  // stub buffer used to be freed in low memory conditions to display message box
  char* m_pStubBuffer;
  CKeyImportDlg* m_pDlgImport;

  // Sub initialization for application
  void SubInitialization();

  // Get Application path
  CString GetAppPath();

public:
  CKeyHolder m_keyHolder;
  CString m_sAppPath;

  CApp();
  ~CApp();

  // system info string holder
  static CString s_sSystemInfo;
  // Build file system info string
  static CString GetSystemInfoString();

  // Browse shell file
  CString GetOpenFileFilename();
  // Browse shell folder
  CString GetOpenFileFolder(CPath _sInitFolder);
  // Get requested file size
  ULARGE_INTEGER GetFileSize(const CString _sFile);
  // Copy string to clipboard
  void StringToCliboard(CString _str);
  // Report message in Modal Dialog
  void Report(CString _sMessage);
  // Populates given list of currently opened documents
  void GetDocumentList(/*IN/OUT*/ CObList* pDocList);
  // Store currently opened document's media filepath to registry
  void StoreCurrentOpenedDocuments();
  // Retsore last opened media files
  void RestoreLastOpenedDocuments(CMultiDocTemplate* _pDocTemplate);
  // Test file existance. Ret true if so.
  bool IsFileExists(CString _sFileName);

// Overrides
  virtual BOOL InitInstance();

  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnManageKeys();
  afx_msg void OnAppAbout();
  // Import Keys from registry
  afx_msg void OnKeysImport();
  // Export Keys to file
  afx_msg void OnKeysExport();
};

extern CApp theApp;
