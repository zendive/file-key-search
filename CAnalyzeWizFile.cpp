// CAnalyzeWizFile.cpp : implementation file
//

#include "stdafx.h"
#include ".\CApp.h"
#include ".\CAnalyzeWizFile.h"


// CAnalyzeWizFile dialog

IMPLEMENT_DYNAMIC(CAnalyzeWizFile, CPropertyPage)

CAnalyzeWizFile::CAnalyzeWizFile()
	: CPropertyPage(CAnalyzeWizFile::IDD)
{
}

CAnalyzeWizFile::CAnalyzeWizFile(CAnalyzeData* _pDocData)
	: CPropertyPage(CAnalyzeWizFile::IDD)
{
  m_pDocData = _pDocData;
}

CAnalyzeWizFile::~CAnalyzeWizFile()
{
}

void CAnalyzeWizFile::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT_FILENAME, m_txtFileName);
}


BEGIN_MESSAGE_MAP(CAnalyzeWizFile, CPropertyPage)
  ON_BN_CLICKED(IDC_BTN_OPEN, OnBnClickedBtnOpen)
END_MESSAGE_MAP()


// CAnalyzeWizFile message handlers

BOOL CAnalyzeWizFile::OnSetActive()
{
  m_txtFileName.SetWindowText(m_pDocData->m_sFileName);

  ControlAdvance();

  return CPropertyPage::OnSetActive();
}

void CAnalyzeWizFile::OnBnClickedBtnOpen()
{
  // "Open file..." dialog to get file name to analyze
  m_pDocData->m_sFileName = theApp.GetOpenFileFilename();
  m_pDocData->m_qwFileSize = theApp.GetFileSize(m_pDocData->m_sFileName);

  m_txtFileName.SetWindowText(m_pDocData->m_sFileName);

  ControlAdvance();
}

void CAnalyzeWizFile::ControlAdvance()
{
  CPropertySheet* psheet = (CPropertySheet*) GetParent();

  if (m_pDocData->m_sFileName.GetLength() != 0)
  {
    psheet->SetWizardButtons(PSWIZB_NEXT);
  }
  else
  {
    psheet->SetWizardButtons(0);
  }
}
