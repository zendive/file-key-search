#pragma once
#include "stdafx.h"

class CMedia: public CObject
{
public:
  CString m_sName;          // media name (based on the key name and counter)
  ULARGE_INTEGER m_qwOffset; // media offset in the analized file
  ULARGE_INTEGER m_qwSize;   // media size calculated through key type


public:
  CMedia();
  CMedia(QWORD _i64Offset, QWORD _i64Size, CString _sName = _T(""));
  virtual ~CMedia();
  virtual void Serialize(CArchive& ar);

protected:
DECLARE_SERIAL(CMedia);
};

// Provides a type-safe "wrapper" for objects of class CObList.
// And also serialization to file advantages.
typedef CTypedPtrList<CObList, CMedia*> CMediaSet;
