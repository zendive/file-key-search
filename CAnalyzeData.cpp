#include "StdAfx.h"
#include ".\CAnalyzeData.h"

IMPLEMENT_SERIAL(CAnalyzeData, CObject, 1);

CAnalyzeData::CAnalyzeData()
  :m_pklAvailable(NULL)
{
  throw _T("private default constructor CAnalyzeData::CAnalyzeData() -> should never be called");
}

CAnalyzeData::CAnalyzeData(const CKeyList* const _pAvailableKeyList)
  :m_pklAvailable(_pAvailableKeyList)
{
  Init();
}

void CAnalyzeData::Init()
{
  m_sFileName = _T("");
  m_qwFileSize.QuadPart = 0;
  ::InitializeCriticalSection(&m_klUsed_lock);
  m_sExtractDir = _T("");
}

CAnalyzeData::~CAnalyzeData(void)
{
  ::DeleteCriticalSection(&m_klUsed_lock);
}

void CAnalyzeData::Serialize(CArchive& ar)
{
  CString sHeader = sPRG_APP_RESULT_HEADER;
  WORD wVersion = nPRG_MAJOR_VERSION;

  if (ar.IsStoring())
  {	// storing code
    ar << sHeader;
    ar << wVersion;
    ar << m_sFileName;
    ar << m_qwFileSize.QuadPart;
  }
  else
  {	// loading code
    ar >> sHeader;
    ar >> wVersion;
    ar >> m_sFileName;
    ar >> m_qwFileSize.QuadPart;
  }

  // used keys for this file
  m_klUsed.Serialize(ar);
}
