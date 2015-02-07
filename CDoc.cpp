#include "stdafx.h"
#include ".\CApp.h"
#include ".\CDoc.h"
#include ".\CAnalyzeWizFile.h"
#include ".\CAnalyzeWizKeys.h"
#include ".\CAnalyzeWizFinish.h"
#include ".\CKeyUpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDoc

IMPLEMENT_DYNCREATE(CDoc, CDocument)

BEGIN_MESSAGE_MAP(CDoc, CDocument)
  ON_COMMAND(ID_ANALYZE_START, &CDoc::OnKeysAnalyze)
  ON_COMMAND(ID_EXTRACT_START, &CDoc::OnMediaExtract)
  ON_COMMAND(ID_MEDIA_OPENEXTRACTFOLDER, &CDoc::OnOpenExtractFolder)
  ON_UPDATE_COMMAND_UI(ID_ANALYZE_START, &CDoc::OnUpdateKeysAnalyze)
  ON_UPDATE_COMMAND_UI(ID_EXTRACT_START, &CDoc::OnUpdateMediaExtract)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CDoc::OnUpdateFileSave)
  ON_UPDATE_COMMAND_UI(ID_MEDIA_OPENEXTRACTFOLDER, &CDoc::OnUpdateOpenExtractFolder)
END_MESSAGE_MAP()

// CDoc construction/destruction

CDoc::CDoc()
: m_DocData(theApp.m_keyHolder.GetKeyListPtr())
{
  m_pKView = NULL;
  m_pAView = NULL;
  m_pDlgAnalyze = NULL;
  m_pDlgExtract = NULL;
}

CDoc::~CDoc()
{
  if (NULL != m_pDlgAnalyze)
  {
    delete m_pDlgAnalyze;
    m_pDlgAnalyze = NULL;
  }

  if (NULL != m_pDlgExtract)
  {
    if (NULL != m_pDlgExtract->m_pKeyList
      && m_pDlgExtract->m_pKeyList != &m_DocData.m_klUsed)
    { // delete key-list used for extraction if it not managed by `m_DocData`
      delete m_pDlgExtract->m_pKeyList;
      m_pDlgExtract->m_pKeyList = NULL;
    }

    delete m_pDlgExtract;
    m_pDlgExtract = NULL;
  }
}

BOOL CDoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
  {
    return FALSE;
  }

  switch (StartAnalyzeWizard(NULL))
  {
    case CDoc::ANALYZE_WIZARD_CANCEL: return FALSE;
    case CDoc::ANALYZE_WIZARD_START: OnKeysAnalyze(); break;
    case CDoc::ANALYZE_WIZARD_CONTINUE: break;
  }

  this->m_sMediaSetFile.Format(_T("%s%s")
    , this->m_DocData.m_sFileName, sPRG_EXTENSION);

  this->SetTitle(NULL);
  return TRUE;
}

// CDoc commands

BOOL CDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
  CPath sPathName = lpszPathName;

  if (!sPathName.FileExists() || sPathName.IsDirectory())
  {
    return FALSE;
  }

  // serialyze "document's" files, otherwise afford analyze wizard
  if (0 == sPathName.GetExtension().MakeLower().CompareNoCase(sPRG_EXTENSION))
  {
    this->m_sMediaSetFile.Format(_T("%s"), lpszPathName);

    if (!CDocument::OnOpenDocument(lpszPathName))
    {
      return FALSE;
    }

    ValidateDocSource();
    ValidateDocKeys();
  }
  else
  {
    this->m_DocData.m_sFileName.Format(_T("%s"), lpszPathName);
    this->m_sMediaSetFile.Format(_T("%s%s"), lpszPathName, sPRG_EXTENSION);
    
    switch (StartAnalyzeWizard(lpszPathName))
    {
      case CDoc::ANALYZE_WIZARD_CANCEL: return FALSE;
      case CDoc::ANALYZE_WIZARD_START: OnKeysAnalyze(); break;
      case CDoc::ANALYZE_WIZARD_CONTINUE: break;
    }
  }

  this->SetTitle(NULL);
  return TRUE;
}

void CDoc::OnCloseDocument()
{
  RETURNIF(m_WorkState.IsActive());
  CDocument::OnCloseDocument();
}

void CDoc::SetModifiedFlag(BOOL _bModified)
{
  CDocument::SetModifiedFlag(_bModified);
  this->SetTitle(NULL);
}

// CDoc serialization

void CDoc::Serialize(CArchive& ar)
{
  m_DocData.Serialize(ar);
}

// CDoc diagnostics

#ifdef _DEBUG
void CDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

DWORD CDoc::StartAnalyzeWizard(CString _sFileName)
{
  if (0 == (m_DocData.m_pklAvailable->Count()))
  {
    int ret;
    ret = ::MessageBox(theApp.m_pMainWnd->m_hWnd
      , _T("You need to define at least one search key to be available ")
        _T("to make file Analyze.\nDo you want to Manage Keys now?")
      , sPRG_WARNING_CAPTION
      , MB_ICONHAND | MB_OKCANCEL);

    if (IDOK == ret)
    {
      theApp.OnManageKeys();
    }

    return CDoc::ANALYZE_WIZARD_CANCEL;
  }

  if (!_sFileName.IsEmpty())
  { // start analyze wizard for specific target file
    m_DocData.m_sFileName = _sFileName;
    m_DocData.m_qwFileSize = theApp.GetFileSize(m_DocData.m_sFileName);
  }

  CAnalyzeWizFile awPageFile(&m_DocData);
  CAnalyzeWizKeys awPageKeys(&m_DocData);
  CAnalyzeWizFinish awPageFinish(&m_DocData);

  CPropertySheet psAnalyzeWizard;
  psAnalyzeWizard.AddPage(&awPageFile);
  psAnalyzeWizard.AddPage(&awPageKeys);
  psAnalyzeWizard.AddPage(&awPageFinish);
  psAnalyzeWizard.SetWizardMode();

  INT_PTR ret = psAnalyzeWizard.DoModal();

  if (ret != ID_WIZFINISH)
  {
    return CDoc::ANALYZE_WIZARD_CANCEL;
  }

  //@`m_DocData` is full of data

  if (TRUE == awPageFinish.m_bStartNow)
  {
    return CDoc::ANALYZE_WIZARD_START;
  }

  return CDoc::ANALYZE_WIZARD_CONTINUE;
}

void CDoc::ValidateDocSource()
{
  // Locals
  // File - Full qualified filename
  // Path - Only path to the file
  // Filename - Only filename without path

  // Validate file existance
  CPath sDocFile = m_DocData.m_sFileName;
  if (sDocFile.FileExists() == FALSE)
  { // try to find it in the current path of the document
    // get MediaSet Path
    CPath sMediaSetPath = this->m_sMediaSetFile;
    sMediaSetPath.RemoveFileSpec();

    // get Doc filename
    CPath sDocFilename = sDocFile;
    sDocFilename.StripPath();

    sDocFile = sMediaSetPath;
    sDocFile.AddBackslash();
    sDocFile.Append(sDocFilename);
    
    // test for existance at new location
    if (sDocFile.FileExists() == FALSE)
    { // report about extract limitations
      CString sReport = _T("Source file for analyze (given in opened file) is not found");

      if (m_DocData.m_sFileName.Compare(sDocFile) == 0)
      {
        sReport.AppendFormat(_T(" at:\n- {%s}"), m_DocData.m_sFileName);
      }
      else
      {
        sReport.AppendFormat(_T(" at:\n- {%s}\nneither at\n- {%s}")
          , m_DocData.m_sFileName, sDocFile);
      }

      sReport.Append(_T("\nloaction.\n\nThe Analyze and Extraction will be NOT possible."));
      ::MessageBox(theApp.m_pMainWnd->m_hWnd, sReport, sPRG_WARNING_CAPTION
        , MB_ICONHAND | MB_OK);

      m_WorkState.m_bDenyAnalyze = true;
      m_WorkState.m_bDenyExtract = true;

      return;
    }
    else
    { // update source file location
      m_DocData.m_sFileName = CString( sDocFile );
    }
  }

  ValidateDocFileSize();
}

void CDoc::ValidateDocFileSize()
{
  ULARGE_INTEGER qwFileSize = theApp.GetFileSize(m_DocData.m_sFileName);

  if (qwFileSize.QuadPart != m_DocData.m_qwFileSize.QuadPart)
  {
    CString sReport;
    sReport.Format(_T("File size of the current and originaly analyzed file are different.")
      _T("\nCurrent:\t{%s} bytes")
      _T("\nOriginal:\t{%s} bytes")
      _T("\n\nThe Media Extraction will be NOT possible till reanalyzing.")
      , CConvert::NumberToHumanString(qwFileSize)
      , CConvert::NumberToHumanString(m_DocData.m_qwFileSize));

    ::MessageBox(theApp.m_pMainWnd->m_hWnd, sReport
      , sPRG_WARNING_CAPTION, MB_ICONHAND | MB_OK);

    m_WorkState.m_bDenyAnalyze = false;
    m_WorkState.m_bDenyExtract = true;

    m_DocData.m_qwFileSize = qwFileSize;
  }
}

void CDoc::ValidateDocKeys()
{
  CKey* pKeyUsed = NULL;
  CKey* pKeyFound = NULL;
  CKeyList* pklNew = new CKeyList();

  // loop throuthg deocument used keys to find missing in app-available
  for (pKeyUsed = m_DocData.m_klUsed.GetRoot(); NULL != pKeyUsed; pKeyUsed = pKeyUsed->Next())
  {
    pKeyFound = m_DocData.m_pklAvailable->GetKey(pKeyUsed->m_sName);
    
    if (NULL == pKeyFound)
    { // document has a key with a new name
      pklNew->AddKey(new CKey(*pKeyUsed));
    }
    else if (!pKeyFound->Compare(pKeyUsed))
    { // document has a key with the same name but with different data
      // now, need to choose other name for importing in format
      // "{$key-name}_new{$iteration}".
      // the drawback - in most cases user will rename keys manually afterwards,
      // so that will draw more and more "new key found alerts". But whole
      // process is for the good reason - detect "manually" changed keys

      CString sNewName;
      for (int i = 1; i < nPRG_MAX_KEY_COUNT; i++)
      {
        sNewName.Format(_T("%s_new%d"), pKeyUsed->m_sName, i);
        if (m_DocData.m_pklAvailable->GetKey(sNewName) == NULL
          && m_DocData.m_klUsed.GetKey(sNewName) == NULL)
        { // name is unique for the available and for used lists
          // change the name to be eable to distinguish between new imported
          pKeyUsed->m_sName = (LPCTSTR)sNewName;
          break;
        }
      }

      pklNew->AddKey(new CKey(*pKeyUsed));
    }
    // else - `m_DocData.m_pklAvailable` has exact same key
  }

  if (pklNew->Count() != 0)
  { // start keys-update dialog
    CKeyUpdateDlg dlg;
    dlg.m_pklNewKeys = pklNew;
    dlg.DoModal();
  }

  delete pklNew;
}

void CDoc::SetTitle(LPCTSTR lpszTitle)
{
  CString sTitle = this->GetDocumentTitle();
  if (sTitle.IsEmpty())
  {
    sTitle = lpszTitle;
  }
  CDocument::SetTitle(sTitle);
}

BOOL CDoc::DoFileSave()
{
  // do not save while any activity is going on
  if (m_WorkState.IsActive())
  {
    return FALSE;
  }

  // ensure that saved file suggestion is always for media file, thus having media extension
  m_strPathName = this->m_sMediaSetFile;
  return CDocument::DoFileSave();
}

CString CDoc::GetDocumentTitle()
{
  if (m_DocData.m_sFileName.IsEmpty())
  {
    return _T("");
  }

  if (0 == m_DocData.m_qwFileSize.QuadPart)
  {
    m_DocData.m_qwFileSize = theApp.GetFileSize(m_DocData.m_sFileName);
  }
  
  CString sFileSizeLong = CConvert::NumberToHumanString(m_DocData.m_qwFileSize);
  CString sFileSizeShort = CConvert::NumberToKibiSizeString(m_DocData.m_qwFileSize);

  CPath sFilename = m_DocData.m_sFileName;
  sFilename.StripPath();

  CString sTitle;
  sTitle.Format(_T("%s%s: %s B (%s) - %s")
    , (this->IsModified()? _T("* ") : _T(""))
    , sFilename, sFileSizeLong, sFileSizeShort, m_DocData.m_sFileName);

  return sTitle;
}

void CDoc::ClearAllViews()
{
  if (NULL != this->m_pKView)
  {
    this->m_pKView->ClearAll();
  }

  if (NULL != this->m_pAView)
  {
    this->m_pAView->ClearAll();
  }
}

void CDoc::OnKeysAnalyze()
{
  RETURNIF(m_WorkState.m_bDenyAnalyze || m_WorkState.IsActive());

  m_WorkState.m_bDenyAnalyze = true;
  m_WorkState.m_bDenyExtract = true;
  m_WorkState.m_bMakingAnalyze = true;

  this->ClearAllViews();

  // revalidate file-size (may change between the calls)
  ValidateDocFileSize();

  // remove previous copy and recreate non-modal again
  if (m_pDlgAnalyze != NULL)
  {
    delete m_pDlgAnalyze; m_pDlgAnalyze = NULL;
  }

  m_pDlgAnalyze = new CAnalyzeDlg(theApp.m_pMainWnd, this);
  m_pDlgAnalyze->Create(CAnalyzeDlg::IDD, theApp.m_pMainWnd);
  m_pDlgAnalyze->ShowWindow(SW_SHOW | SW_SHOWNOACTIVATE);
}

void CDoc::OnKeyAnalyzeDlgEnd()
{ // called from CAnalyzeDlg::OnCancel()
  RETURNIF(NULL == m_pDlgAnalyze);
  
  m_WorkState.m_bMakingAnalyze = false;
  m_WorkState.m_bDenyAnalyze = false;
  m_WorkState.m_bDenyExtract = (m_pDlgAnalyze->m_dwTotalFound == 0);
  
  // after non modal dialog end the `m_pDlgAnalyze` will be deleted
  // on second analyze call or in ~CDoc
  
  SetModifiedFlag(TRUE);
  UpdateAllViews(NULL);
}

void CDoc::OnMediaExtract()
{
  RETURNIF(m_WorkState.m_bDenyExtract || m_WorkState.IsActive());
  MediaExtractDlgStart(&m_DocData.m_klUsed);
}

void CDoc::MediaExtractDlgStart(CKeyList* _pKeyList)
{
  RETURNIF(m_WorkState.m_bDenyExtract || m_WorkState.IsActive());
  RETURNIF(NULL == _pKeyList);

  if (!m_DocData.m_sExtractDir.IsEmpty())
  { // if extraction is known - afford to choose another option
    CString sMessage;
    sMessage.Format(_T("Keep current extract location?\r\n\"%s\""), m_DocData.m_sExtractDir);

    int iRet = ::MessageBox(theApp.m_pMainWnd->m_hWnd, sMessage
      , _T(" Question"), MB_ICONQUESTION | MB_YESNOCANCEL);

    if (IDNO == iRet)
    {
      m_DocData.m_sExtractDir.Empty();
    }
    else if (IDCANCEL == iRet)
    {
      return;
    }
  }

  if (m_DocData.m_sExtractDir.IsEmpty())
  { // choose extracion point if not already defined
    CPath sInitFolder = m_sMediaSetFile;
    sInitFolder.RemoveFileSpec();

    CString sExtractDir;
    sExtractDir = theApp.GetOpenFileFolder(sInitFolder);
    if (!sExtractDir.IsEmpty())
    {
      CPath sExtractDirName(m_DocData.m_sFileName);
      sExtractDirName.StripPath();

      sExtractDir.AppendFormat(_T("\\%s_%s"), sExtractDirName, sPRG_APP_NAME);
      m_DocData.m_sExtractDir = sExtractDir;
    }
  }

  RETURNIF(m_DocData.m_sExtractDir.IsEmpty());

  ULARGE_INTEGER uliFreeBytesAvailable;
  GetDiskFreeSpaceEx(m_DocData.m_sExtractDir, &uliFreeBytesAvailable, NULL, NULL);
  ULARGE_INTEGER uliTotalMediaSize = _pKeyList->GetTotalMediaSize();

  if (uliTotalMediaSize.QuadPart > uliFreeBytesAvailable.QuadPart)
  {
    CString sReport;
    sReport.Format(
      _T("There is no enough space on selected storage device to extract selected items.")
      _T("\r\nLocation: \"%s\"")
      _T("\r\nRequired space:\t\t%s Bytes")
      _T("\r\nAvailable free space:\t%s Bytes")
      , m_DocData.m_sExtractDir
      , CConvert::NumberToHumanString(uliTotalMediaSize)
      , CConvert::NumberToHumanString(uliFreeBytesAvailable));
    theApp.Report(sReport);
    m_DocData.m_sExtractDir.Empty();
    
    return;
  }

  m_WorkState.m_bDenyAnalyze = true;
  m_WorkState.m_bDenyExtract = true;
  m_WorkState.m_bMakingExtract = true;

  if (NULL != m_pDlgExtract)
  {
    delete m_pDlgExtract; m_pDlgExtract = NULL;
  }

  m_pDlgExtract = new CMediaExtractDlg(theApp.m_pMainWnd);
  m_pDlgExtract->m_pDoc = this;
  m_pDlgExtract->m_pKeyList = _pKeyList;
  m_pDlgExtract->m_uliTotalMediaSize = uliTotalMediaSize;

  m_pDlgExtract->Create(CMediaExtractDlg::IDD, theApp.m_pMainWnd);
  m_pDlgExtract->ShowWindow(SW_SHOW | SW_SHOWNOACTIVATE);
}

void CDoc::OnMediaExtractDlgEnd()
{
  RETURNIF(NULL == m_pDlgExtract);

  m_WorkState.m_bDenyAnalyze = false;
  m_WorkState.m_bDenyExtract = false;
  m_WorkState.m_bMakingExtract = false;

  // after nonmodal extract dialog end the `m_pDlgExtract` will be deleted
  // on second extract call or in ~CDoc

  if (m_pDlgExtract->m_pKeyList != &m_DocData.m_klUsed)
  { // delete key-list used for extraction if it not managed by `m_DocData`
    delete m_pDlgExtract->m_pKeyList;
    m_pDlgExtract->m_pKeyList = NULL;
  }
}

void CDoc::OnOpenExtractFolder()
{
  RETURNIF(m_DocData.m_sExtractDir.IsEmpty());

  ::ShellExecute(theApp.m_pMainWnd->m_hWnd, _T("explore")
    , _T("\"")+ m_DocData.m_sExtractDir +_T("\\\"")
    , NULL, NULL, SW_SHOWNORMAL);
}

void CDoc::OnUpdateFileSave(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(this->IsModified());
}

void CDoc::OnUpdateMediaExtract(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(!m_WorkState.m_bDenyExtract);
}

void CDoc::OnUpdateKeysAnalyze(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(!m_WorkState.m_bDenyAnalyze);
}

void CDoc::OnUpdateOpenExtractFolder(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(!m_DocData.m_sExtractDir.IsEmpty());
}