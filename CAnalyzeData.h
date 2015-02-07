#pragma once
#include ".\CKeyList.h"

class CAnalyzeData: public CObject
{
  DECLARE_SERIAL(CAnalyzeData);

public:
  // pointer to general application handled keys
  const CKeyList* const m_pklAvailable;
  // analyze target file-name
  CString m_sFileName;
  // analyze target file-size
  ULARGE_INTEGER m_qwFileSize;
  // extraction folder
  CString m_sExtractDir;
  // keys used for the analyze target file
  CKeyList m_klUsed;
  // access CS-lock used in operations on m_klUsed
  CRITICAL_SECTION m_klUsed_lock;

private:
  CAnalyzeData();
public:
  CAnalyzeData(const CKeyList* const _pAvailableKeyList);
  ~CAnalyzeData();

private:
  void Init();
public:
  virtual void Serialize(CArchive& ar);
};
