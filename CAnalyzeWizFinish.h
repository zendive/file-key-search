#pragma once
#include ".\CAnalyzeData.h"

// CAnalyzeWizFinish dialog

class CAnalyzeWizFinish : public CPropertyPage
{
	DECLARE_DYNAMIC(CAnalyzeWizFinish)

private:
  CAnalyzeData* m_pDocData;

public:
	CAnalyzeWizFinish();
  CAnalyzeWizFinish(CAnalyzeData* _pDocData);
	virtual ~CAnalyzeWizFinish();

// Dialog Data
	enum { IDD = IDD_ANAL_WIZPAGE_ANAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnSetActive();
  BOOL m_bStartNow;
};
