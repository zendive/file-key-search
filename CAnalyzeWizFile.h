#pragma once
#include "afxwin.h"
#include ".\CAnalyzeData.h"

// CAnalyzeWizFile dialog

class CAnalyzeWizFile : public CPropertyPage
{
	DECLARE_DYNAMIC(CAnalyzeWizFile)
private:
  CAnalyzeData* m_pDocData;

public:
	CAnalyzeWizFile();
  CAnalyzeWizFile(CAnalyzeData* _pDocData);
	virtual ~CAnalyzeWizFile();

// Dialog Data
	enum { IDD = IDD_ANAL_WIZPAGE_FILE };

private:
  void ControlAdvance();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnSetActive();
  afx_msg void OnBnClickedBtnOpen();
  CEdit m_txtFileName;
};
