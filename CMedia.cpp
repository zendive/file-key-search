#include "StdAfx.h"
#include ".\CMedia.h"

IMPLEMENT_SERIAL(CMedia, CObject, 1);

CMedia::CMedia()
{
  m_sName = _T("");
  m_qwOffset.QuadPart = 0L;
  m_qwSize.QuadPart = 0L;
}

CMedia::CMedia(QWORD _i64Offset, QWORD _i64Size, CString _sName /*= _T("")*/)
{
  m_sName = _sName;
  m_qwOffset.QuadPart = _i64Offset;
  m_qwSize.QuadPart = _i64Size;
}

CMedia::~CMedia()
{
}

void CMedia::Serialize(CArchive& ar)
{
  CObject::Serialize(ar);

  if (ar.IsStoring())
  {	// storing code
    ar << m_sName << m_qwOffset.QuadPart << m_qwSize.QuadPart;
  }
  else
  {	// loading code
    ar >> m_sName >> m_qwOffset.QuadPart >> m_qwSize.QuadPart;
  }
}
