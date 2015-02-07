// CApp.cpp : Defines the class behaviors for the application.
//
#include "StdAfx.h"
#include ".\CMainFrame.h"
#include ".\CChildFrame.h"
#include ".\CDoc.h"
#include ".\CKView.h"
#include ".\CReportDlg.h"
#include ".\CKeyExportDlg.h"
#include ".\CKeyManagerDlg.h"
#include ".\CApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
  ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
  ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
  ON_COMMAND(ID_MANAGEKEYS, OnManageKeys)
  ON_COMMAND(ID_KEYS_IMPORT, OnKeysImport)
  ON_COMMAND(ID_KEYS_EXPORT, OnKeysExport)
END_MESSAGE_MAP()


CApp::CApp()
{
  m_pDlgImport = NULL;
  m_pStubBuffer = NULL;
}

CApp::~CApp()
{
  if (NULL != m_pDlgImport)
  {
    delete m_pDlgImport;
    m_pDlgImport = NULL;
  }

  if (NULL != m_pStubBuffer)
  {
    free(m_pStubBuffer); m_pStubBuffer = NULL;
  }
}

CString CApp::s_sSystemInfo = CApp::GetSystemInfoString();

// The one and only CApp object
CApp theApp;

CString CApp::GetAppPath()
{
  TCHAR* pModuleFilename = (TCHAR*)calloc(nPRG_MAX_PATH, sizeof (TCHAR));

  THROWIF(NULL == m_pStubBuffer || NULL == pModuleFilename
    , _T("Insufficient Memory resources"));

  // get module name and name_size
  if (!GetModuleFileName(NULL, pModuleFilename, nPRG_MAX_PATH))
  {
    return _T("");
  }

  CPath sAppPath;
  sAppPath = pModuleFilename;
  free(pModuleFilename);

  sAppPath.RemoveFileSpec();
  sAppPath.AddBackslash();

  return (CString) sAppPath;
}

void CApp::SubInitialization()
{
  m_pStubBuffer = (char*)malloc(nPRG_APP_REPORT_MEMORY_STUB_AMOUNT);

  m_sAppPath = GetAppPath();

  /// Init Key Holder
  m_keyHolder.Init(m_sAppPath);
  m_keyHolder.LoadRegKeys();
}

BOOL CApp::InitInstance()
{
  // InitCommonControls() is required on Windows XP if an application
  // manifest specifies use of ComCtl32.dll version 6 or later to enable
  // visual styles.  Otherwise, any window creation will fail.
  InitCommonControls();
  CWinApp::InitInstance();

  // Standard initialization
  SetRegistryKey(sPRG_COMPANY_NAME);
  LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

  try
  {
    this->SubInitialization();
  }
  catch (CString _sMsg)
  {
    this->Report(_sMsg);
  }
  catch (LPCTSTR _sMsg)
  {
    this->Report(_sMsg);
  }

  // Register the application's document templates.  Document templates
  //  serve as the connection between documents, frame windows and views
  CMultiDocTemplate* pDocTemplate;
  pDocTemplate = new CMultiDocTemplate(IDR_APP_TYPE,
    RUNTIME_CLASS(CDoc),
    RUNTIME_CLASS(CChildFrame), // custom MDI child frame
    RUNTIME_CLASS(CKView));

  if (!pDocTemplate)
  {
    return FALSE;
  }

  AddDocTemplate(pDocTemplate);

  // create main MDI Frame window
  CMainFrame* pMainFrame = new CMainFrame;
  if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
  {
    return FALSE;
  }
  m_pMainWnd = pMainFrame;

  // call DragAcceptFiles only if there's a suffix
  //  In an MDI app, this should occur immediately after setting m_pMainWnd
  DragAcceptFiles(m_pMainWnd->m_hWnd, true);

  // Parse command line for standard shell commands, DDE, file open
  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);
  
  if (CCommandLineInfo::FileNew == cmdInfo.m_nShellCommand)
  { // do not open new file on unargumented startup (need before ProcessShellCommand)
    cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
  }

  // Dispatch commands specified on the command line.  Will return FALSE if
  // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
  if (!ProcessShellCommand(cmdInfo))
  {
    return FALSE;
  }

  // The main window has been initialized, so show and update it
  pMainFrame->ShowWindow(m_nCmdShow);
  pMainFrame->UpdateWindow();

  // try load last opened documents
  this->RestoreLastOpenedDocuments(pDocTemplate);

  return TRUE;
}

void CApp::Report(CString _sMessage)
{
  if (NULL != m_pStubBuffer)
  {
    free(m_pStubBuffer); m_pStubBuffer = NULL;
  }

  // format system last error
  wchar_t wsLastError[nPRG_MAX_ERRORMESSAGE_SIZE];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL
    , GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
    , wsLastError, (nPRG_MAX_ERRORMESSAGE_SIZE - 1), NULL);

  // show modal report
  CReportDlg dlg;
  dlg.m_txtLastError = wsLastError;
  dlg.m_txtMessage = _sMessage;
  dlg.DoModal();

  m_pStubBuffer = (char*) malloc(nPRG_APP_REPORT_MEMORY_STUB_AMOUNT);

  // terminate if canceled
  if (dlg.m_bTerminate)
  {
    this->HideApplication();
    this->ExitInstance();
    exit(-1);
  }
}

bool CApp::IsFileExists(CString _sFileName)
{
  HANDLE hFile = CreateFile(_sFileName, 0/*query without accessing*/
    , FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

  if (INVALID_HANDLE_VALUE == hFile || NULL == hFile)
  {
    return false;
  }

  CloseHandle(hFile);
  return true;
}

void CApp::StringToCliboard(CString _str/*str2cpy*/)
{
  try
  {
	  THROWIF(_str.IsEmpty(), _T("String is empty"));
    THROWIF(!::OpenClipboard(m_pMainWnd->m_hWnd), _T("Can not open clipboard"));
    THROWIF(!::EmptyClipboard(), _T("Can not empty clipboard"));
  	
    // Allocate a buffer for the text
    DWORD dwStrBufferSize = (_str.GetLength() +1)*sizeof(TCHAR);
    HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, dwStrBufferSize);
	  if (NULL == hGlobal)
	  {
		  THROWIF(!::CloseClipboard(), _T("Can not close clipboard after global memory allocation failure"));
      THROWIF(true, _T("Global memory allocation failed"));
	  }

	  // Copy the text from `_str`
	  TCHAR* pStr = (TCHAR*)::GlobalLock(hGlobal);
    memcpy(pStr, _str.GetBuffer(), dwStrBufferSize);
	  ::GlobalUnlock(hGlobal);

	  // Place the handle on the clipboard
	  THROWIF(!::SetClipboardData(CF_UNICODETEXT, hGlobal), _T("Can not assign clipboard data"));
	  THROWIF(!::CloseClipboard(), _T("Can not close clipboard after data assignment"));
  }
  catch (LPTSTR _sErr)
  {
    CString sReport;
    sReport.Format(_T("Could not copy \"%s\" to clippboard.\r\nError: %s"), _str, _sErr);
    this->Report(sReport);
  }
}

CString CApp::GetSystemInfoString()
{
  CString str;
  OSVERSIONINFOEX osvi;
  SecureZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

  try
  {
    ::GetVersionEx((OSVERSIONINFO*) &osvi);
    str.Format(_T("v%d.%d, Platform: %d, Build: %d")
      , osvi.dwMajorVersion, osvi.dwMinorVersion
      , osvi.dwPlatformId, osvi.dwBuildNumber);
  }
  catch (...)
  {
    str = _T("undefined");
  }
  
  return str;
}

CString CApp::GetOpenFileFilename()
{
  TCHAR szFileName[nPRG_MAX_PATH] = _T("\0");
  TCHAR szTitle[] = _T("Select file for Analyze...");
  
  OPENFILENAME ofn;
  SecureZeroMemory(&ofn, sizeof(ofn));
  
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = m_pMainWnd->m_hWnd;
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = nPRG_MAX_PATH * sizeof(TCHAR);
  ofn.lpstrFilter = _T("Any file\0*\0\0");
  ofn.nFilterIndex = 1;
  ofn.lpstrTitle = szTitle;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING
    | OFN_EXPLORER | OFN_READONLY | OFN_LONGNAMES | OFN_NONETWORKBUTTON;
  
  if (FALSE == ::GetOpenFileName(&ofn))
  { // if no file selected
    return CString(_T(""));
  }
  else
  {
    return CString(szFileName);
  }
}

CString CApp::GetOpenFileFolder(CPath _sInitFolder)
{
  CString sFolder(_T(""));
  CComPtr<IShellFolder> spFolder;

  HRESULT hRes = SHGetDesktopFolder(&spFolder);
  if (hRes < 0)
  { // handle error
    return sFolder;
  }

  ULONG chEaten;
  ULONG uAttr = 0;
  LPITEMIDLIST pidlInitFolder;

  hRes = spFolder->ParseDisplayName(NULL, NULL, _sInitFolder.m_strPath.GetBuffer(),
    &chEaten, &pidlInitFolder, &uAttr);

  if (hRes < 0)
  { // handle error
    return sFolder;
  }

	LPMALLOC pIMalloc = NULL;
	if (SUCCEEDED(::SHGetMalloc(&pIMalloc)))
	{
		BROWSEINFO bi;
		SecureZeroMemory(&bi, sizeof(bi));

    bi.lpszTitle = _T("Choose a Folder for Extraction, where next folder will be created:")
      _T("\r\n{$FileName}_") sPRG_APP_NAME _T("/{$KeyName}/{$MediaName} ...");
		bi.ulFlags = BIF_USENEWUI | BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;
    bi.hwndOwner = m_pMainWnd->m_hWnd;
    //disables freedom of choice//bi.pidlRoot = pidlInitFolder;

    TCHAR szDisplayName[MAX_PATH];
    SecureZeroMemory(&szDisplayName, sizeof(MAX_PATH));
		bi.pszDisplayName = szDisplayName;

		LPITEMIDLIST pidlSelectedFolder = ::SHBrowseForFolder(&bi);

		if (NULL != pidlSelectedFolder)
		{
			TCHAR sDir[MAX_PATH];
      SecureZeroMemory(&sDir, sizeof(MAX_PATH));

			if (::SHGetPathFromIDList(pidlSelectedFolder, sDir))
      {
				sFolder = (LPTSTR) sDir;
      }

			pIMalloc->Free(pidlSelectedFolder);
      pIMalloc->Free(pidlInitFolder);
		}

		pIMalloc->Release();
	}

  return sFolder;
}

void CApp::OnManageKeys()
{
  // launch key-manager with a copy of the of generic keys
  CKeyManagerDlg dlg(this->m_pMainWnd);
  dlg.m_pKeyList = new CKeyList(*m_keyHolder.GetKeyListPtr());
  INT_PTR iRet = dlg.DoModal();

  switch (iRet)
  {
  case IDOK:      // save changes if there are
    if (dlg.m_bHasChanges)
    {
      this->m_keyHolder.Absorb(dlg.m_pKeyList);
    }
    break;
  case IDCANCEL:  // discard changes
    break;
  }
  
  if (NULL != dlg.m_pKeyList)
  {
    delete dlg.m_pKeyList;
  }
}

void CApp::OnKeysImport()
{
  RETURNIF(m_pDlgImport && (m_pDlgImport->IsWindowVisible() == TRUE));

  if (m_pDlgImport != NULL)
  {
    delete m_pDlgImport;
  }

  m_pDlgImport = new CKeyImportDlg(this->m_pMainWnd);

  m_pDlgImport->m_sAppPath = m_sAppPath;
  m_pDlgImport->m_pKeyHolder = &m_keyHolder;

  m_pDlgImport->Create(CKeyImportDlg::IDD, this->m_pMainWnd);
  m_pDlgImport->ShowWindow(SW_SHOW);
}

void CApp::OnKeysExport()
{
  CKeyExportDlg keyExportDlg(this->m_pMainWnd);

  keyExportDlg.m_sAppPath = m_sAppPath;
  keyExportDlg.m_pKeyHolder = &m_keyHolder;
  keyExportDlg.DoModal();
}

ULARGE_INTEGER CApp::GetFileSize(const CString _sFile)
{
  ULARGE_INTEGER qwFileSize;
  qwFileSize.QuadPart = 0;

  if (_sFile.IsEmpty())
  {
    return qwFileSize;  // 0
  }

  // try 2 open file
  HANDLE hFile = CreateFile(_sFile, GENERIC_READ, FILE_SHARE_READ,
    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (INVALID_HANDLE_VALUE == hFile || NULL == hFile)
  {
    return qwFileSize;  // 0
  }

  // try to detect file size
  DWORD dwHighPart;
  qwFileSize.LowPart = (DWORD) ::GetFileSize(hFile, &dwHighPart);
  qwFileSize.HighPart = dwHighPart;

  CloseHandle(hFile);

  return qwFileSize;
}

void CApp::GetDocumentList(/*IN/OUT*/ CObList* pDocList)
{
  ASSERT(pDocList->IsEmpty());

  POSITION pos = GetFirstDocTemplatePosition();

  while (pos)
  {
    CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate(pos);
    POSITION pos2 = pTemplate->GetFirstDocPosition();
    while (pos2)
    {
      CDocument * pDocument;
      if ((pDocument = pTemplate->GetNextDoc(pos2)) != NULL)
      {
        pDocList->AddHead(pDocument);
      }
    }
  }
}

void CApp::StoreCurrentOpenedDocuments()
{
  CObList olDocs;
  theApp.GetDocumentList(/*IN/OUT*/ &olDocs);
  CRegKey reg;

  while (true)
  {
    if (0 == reg.Open(HKEY_CURRENT_USER, sPRG_REGPATH_APP, KEY_READ | KEY_WRITE))
    {
      reg.RecurseDeleteKey(sPRG_REGNAME_LASTOPENED);
    }

    INT_PTR iCount = olDocs.GetCount();
    if (0 != iCount)
    { // store currently opened documents
      BREAKIF(0 != reg.Create(HKEY_CURRENT_USER, sPRG_REGPATH_LASTOPENED, REG_NONE
        , REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE));

      POSITION pos = olDocs.GetHeadPosition();
      CDoc* pDoc;
      CString sRegName;
     
      for (int c = 0; c < iCount && c < nPRG_MAX_LASTOPENED_REMEMBERED; c++)
      {
        pDoc = (CDoc*) olDocs.GetNext(pos);
        BREAKIF(NULL == pDoc);
        
        // (iCount-c) means store first item in the list as a last record in reg.
        // at startup, document appearence will be swapped by the document open order sequence
        sRegName.Format(_T("File%d"), iCount-c);
        if (theApp.IsFileExists(pDoc->m_sMediaSetFile))
        { // if document was saved - preffer to store that file refference
          BREAKIF(0 != reg.SetStringValue(sRegName, pDoc->m_sMediaSetFile));
        }
        else
        { // otherwise - store document target
          BREAKIF(0 != reg.SetStringValue(sRegName, pDoc->m_DocData.m_sFileName));
        }
      }
    }

    break;
  }

  reg.Close();
}

void CApp::RestoreLastOpenedDocuments(CMultiDocTemplate *_pDocTemplate)
{
  RETURNIF(NULL == _pDocTemplate);

  CRegKey reg;
  RETURNIF(0 != reg.Open(HKEY_CURRENT_USER, sPRG_REGPATH_LASTOPENED, KEY_READ));
  CDoc* pDoc = NULL;
  CString sFilePath;
  CString sValueName;
  DWORD dwValueSize;
  TCHAR* sValue = NULL;

  for (int c = 0; c < nPRG_MAX_LASTOPENED_REMEMBERED; c++)
  {
    sValueName.Format(_T("File%d"), c+1);

    dwValueSize = nPRG_MAX_PATH;
    sValue = (TCHAR*) calloc(dwValueSize, sizeof(TCHAR));
    BREAKIF(NULL == sValue);
    
    BREAKIF(0 != reg.QueryStringValue(sValueName, sValue, &dwValueSize));
    sFilePath = CString(sValue);
    free(sValue); sValue = NULL;

    pDoc = (CDoc*) _pDocTemplate->OpenDocumentFile(sFilePath);
  }

  if (NULL != sValue)
  {
    free(sValue); sValue = NULL;
  }

  reg.Close();
}

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

  // Dialog Data
  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  // Implementation
protected:
  DECLARE_MESSAGE_MAP()
public:
  // Descripton about the programm
  CString m_editAboutPrg;
};


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
  , m_editAboutPrg(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_ABOUT_PRG, m_editAboutPrg);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CApp::OnAppAbout()
{
  CAboutDlg dlg;
  
  dlg.m_editAboutPrg.Format(
    _T("Version: %s (%s)\r\nBirthday: %s, %s\r\nOS: %s")
    _T("\r\n\r\nMax Key count: %d")
    _T("\r\nMax Media count: %d (items per Key)")
    _T("\r\nMax Key Signature size: %d (bytes)")
    , sPRG_VERSION
# ifdef _DEBUG
	  , _T("debug")
# else
	  , _T("release")
# endif
    , _T(__TIME__), _T(__DATE__), s_sSystemInfo
    , nPRG_MAX_KEY_COUNT
    , nPRG_MAX_ITEMS_LIMIT
    , nPRG_MAX_KEYSIGNATURE_SIZE);

  dlg.DoModal();
}
