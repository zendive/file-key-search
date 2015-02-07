#pragma once
#include "CKeyList.h"
#include "CAnalyzeData.h"
#include "CWorkState.h"
#include "CAnalyzeDlg.h"
#include "CMediaExtractDlg.h"
#include "CKView.h"
#include "CAView.h"

class CKView;
class CAView;

class CDoc : public CDocument
{
  DECLARE_DYNCREATE(CDoc)
  
private:
  enum ANALYZE_WIZARD
  {
    ANALYZE_WIZARD_CANCEL,
    ANALYZE_WIZARD_START,
    ANALYZE_WIZARD_CONTINUE
  };

public:
  // all data about document target
  CAnalyzeData m_DocData;
  // "document's" target meta file
  CString m_sMediaSetFile;
  CWorkState m_WorkState;
  CAnalyzeDlg *m_pDlgAnalyze;
  CMediaExtractDlg* m_pDlgExtract;
  CKView* m_pKView;
  CAView* m_pAView;

protected:
  // create from serialization only
  CDoc();

private:
  // Start analyze wizard
  DWORD StartAnalyzeWizard(CString _sFilename);
  // Ensure document target file-size is not changed
  void ValidateDocSource();
  // Unsure target media file-size is the same as defined in media-file and on Disk
  void ValidateDocFileSize();
  // Analyze `m_DocData.m_klUsed` on new and/or different keys
  // and so resolve any conflicts in the future.
  // NOTE: if user chooses to import new kes, - both 
  // m_DocData.m_pklAvailable and Registry will be updated!
  void ValidateDocKeys();
  // Build document title
  CString GetDocumentTitle();

public:
  // Handle end of analyze dialog
  void OnKeyAnalyzeDlgEnd();
  // Handle end of extraction dialog
  void OnMediaExtractDlgEnd();
  // Handle cleanings of all child views
  void ClearAllViews();
  // note: `_pKeyList` memory should be released at finish
  void MediaExtractDlgStart(CKeyList* _pKeyList);

public:
  // Destroy document data
  virtual ~CDoc();
  // Launch analyze wizard
  virtual BOOL OnNewDocument();
  // Serialize document data
  virtual void Serialize(CArchive& ar);
  // Manage formats opened by the app
  virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
  // Make custom document title
  virtual void SetTitle(LPCTSTR lpszTitle);
  // Deny saving when doing any activity
  // ensure that saved file suggestion is always for media file, thus having media extension
  virtual BOOL DoFileSave();
  // Deny saving when doing any activity
  virtual void OnCloseDocument();
  // Update title when document modification flag is changed
  virtual void SetModifiedFlag(BOOL _bModified);

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnKeysAnalyze();
  afx_msg void OnMediaExtract();
  afx_msg void OnOpenExtractFolder();
  afx_msg void OnUpdateKeysAnalyze(CCmdUI *pCmdUI);
  afx_msg void OnUpdateMediaExtract(CCmdUI *pCmdUI);
  afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
  afx_msg void OnUpdateOpenExtractFolder(CCmdUI *pCmdUI);
};
