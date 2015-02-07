// CAnalyzeWizFinish.cpp : implementation file
//

#include "stdafx.h"
#include ".\CApp.h"
#include ".\CAnalyzeWizFinish.h"


// CAnalyzeWizFinish dialog

IMPLEMENT_DYNAMIC(CAnalyzeWizFinish, CPropertyPage)

CAnalyzeWizFinish::CAnalyzeWizFinish()
	: CPropertyPage(CAnalyzeWizFinish::IDD)
  , m_bStartNow(FALSE)
{
}

CAnalyzeWizFinish::CAnalyzeWizFinish(CAnalyzeData* _pDocData)
	: CPropertyPage(CAnalyzeWizFinish::IDD)
{
  m_pDocData = _pDocData;

  m_bStartNow = TRUE;
}

CAnalyzeWizFinish::~CAnalyzeWizFinish()
{
}

void CAnalyzeWizFinish::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_CHECK_START_NOW, m_bStartNow);
}


BEGIN_MESSAGE_MAP(CAnalyzeWizFinish, CPropertyPage)
END_MESSAGE_MAP()


// CAnalyzeWizFinish message handlers

BOOL CAnalyzeWizFinish::OnSetActive()
{
  CPropertySheet* psheet = (CPropertySheet*) GetParent();
  psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

  return CPropertyPage::OnSetActive();
}
