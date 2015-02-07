#include "stdafx.h"
#include "CApp.h"
#include "CKey.h"

IMPLEMENT_SERIAL(CKey, CObject, 1);

CKey::CKey()
{
  m_sName = _T("");
  m_sComment = _T("");
  m_dwSignSize = 0;
  m_pSign = NULL;
  m_dwCSO = 0;
  m_dwCSOType = CKey::CSOTYPE_UNASSIGNED;
  m_dwSkipScope = 0;
  m_sExtensions = _T("");

  next = NULL;
}

CKey::CKey(const CKey& _key)
{
  m_sName = (LPCTSTR)_key.m_sName;
  m_sComment = (LPCTSTR)_key.m_sComment;
  m_dwSignSize = _key.m_dwSignSize;

  m_pSign = (char*) calloc(m_dwSignSize, sizeof (char));
  memcpy(m_pSign, _key.m_pSign, m_dwSignSize);

  m_dwCSO = _key.m_dwCSO;
  m_dwCSOType = _key.m_dwCSOType;
  m_dwSkipScope = _key.m_dwSkipScope;
  m_sExtensions = (LPCTSTR)_key.m_sExtensions;

  next = NULL;
}

CKey& CKey::operator=(const CKey& /*_key*/)
{
  throw _T("PLEASE, USE CKey::Copy() INSTEAD");
  /*
  if (this == &_key)
  { // self-assignment
    return *this;
  }
  */
  return *this;
}

CKey::CKey(const CString _sNewName)
{
  m_sName = (LPCTSTR)_sNewName;
  m_sComment = sPRG_KEYTEMPLATE_COMMENT;

  m_dwSignSize = nPRG_KEYTEMPLATE_SIGNSIZE;
  char pSign[nPRG_KEYTEMPLATE_SIGNSIZE] = sPRG_KEYTEMPLATE_SIGNATURE;
  m_pSign = (char *) calloc(m_dwSignSize, sizeof (char));
  memcpy(m_pSign, pSign, m_dwSignSize);

  m_dwCSO = nPRG_KEYTEMPLATE_CSO;
  m_dwCSOType = nPRG_KEYTEMPLATE_CSOTYPE;
  m_dwSkipScope = nPRG_KEYTEMPLATE_SKIPSCOPE;
  m_sExtensions = sPRG_KEYTEMPLATE_EXTENSIONS;

  next = NULL;
}

CKey::~CKey()
{
  DeleteMediaSet();

  m_sName.Empty();
  m_sComment.Empty();

  if (m_pSign)
  {
    SecureZeroMemory(m_pSign, m_dwSignSize);
    free(m_pSign);
    m_pSign = NULL;
    m_dwSignSize = 0;
  }
  
  m_dwCSO = 0;
  m_dwCSOType = CKey::CSOTYPE_UNASSIGNED;
  m_dwSkipScope = 0;
  m_sExtensions.Empty();

  next = NULL;
}

void CKey::DeleteMediaSet()
{
  CMedia *pMedia = NULL;
  while (!m_MediaSet.IsEmpty())
  {
    pMedia = m_MediaSet.GetHead();
    delete pMedia;
    m_MediaSet.RemoveHead();
  }
}

CKey* CKey::Next() const
{
  return this->next;
}

void CKey::Copy(const CKey* _pKey)
{
  if (!_pKey->Validate())
  {
    return; 
  };

  m_sName = (LPCTSTR)_pKey->m_sName;
  m_sComment = (LPCTSTR)_pKey->m_sComment;

  if (m_pSign)
  {
    SecureZeroMemory(m_pSign, m_dwSignSize);
    free(m_pSign);
  }

  m_pSign = (char*) calloc(_pKey->m_dwSignSize, sizeof (char));
  memcpy(m_pSign, _pKey->m_pSign, _pKey->m_dwSignSize);
  m_dwSignSize = _pKey->m_dwSignSize;

  m_dwCSO = _pKey->m_dwCSO;
  m_dwCSOType = _pKey->m_dwCSOType;
  m_dwSkipScope = _pKey->m_dwSkipScope;
  m_sExtensions = (LPCTSTR)_pKey->m_sExtensions;
 
  next = next; // remain pointer to next
}

bool CKey::Compare(const CKey* _key) const
{
  if (m_sName.Compare(_key->m_sName) != 0
    || m_sComment.Compare(_key->m_sComment) != 0
    || m_dwSignSize != _key->m_dwSignSize
    || memcmp(m_pSign, _key->m_pSign, m_dwSignSize) != 0
    || m_dwCSO != _key->m_dwCSO
    || m_dwCSOType != _key->m_dwCSOType
    || m_dwSkipScope != _key->m_dwSkipScope
    || m_sExtensions.Compare(_key->m_sExtensions) != 0)
  {
    return false;
  }

  return true;
}

QWORD CKey::GetItemSize(const QWORD _i64ContentSize) const
{
  ULARGE_INTEGER qwItemSize;

  switch (m_dwCSOType)
  {
    // DWORD on CSOffset must be reinterpretiered as Size that include inself
    // only Content. That's without Header size and DWORD's 4-bytes. So calculate
    // total size of the item.
  case CKey::CSOTYPE_CONTENT_SIZE:
    {
      qwItemSize.QuadPart =
        m_dwCSO                   // offset where to get CSO (actualy it's Header+ size)
        + 4                       // 4 bytes of CS value "container"
        + _i64ContentSize;        // Content size
      break;
    }

    // This type of CSOffset DWORD means that it's already include content size,
    // header and the DWORD's 4-bytes. So no need in any calculation.
  case CKey::CSOTYPE_TOTAL_SIZE:
    {
      qwItemSize.QuadPart = _i64ContentSize;
      break;
    }

    // CSO type can't be undefined in callback context of this method.
  case CKey::CSOTYPE_UNASSIGNED:
  default:
    {
      throw _T("Design error: Unassigned key type!");
      break;
    }
  }

  return qwItemSize.QuadPart;
}

CString CKey::GetExtension() const
{
  return GetExtensionAt(0);
}

CString CKey::GetExtensionAt(DWORD _N) const
{
  CString sExt      = _T(".");
  CString sDefault  = _T(".HeX");
  CString sToken    = _T(";");

  if ((m_sExtensions.GetLength() == 0)
    || (_N >= (nPRG_MAX_KEYEXTENSION_SIZE/2)) )
  {
    return sDefault;
  }

  sExt += m_sExtensions.Tokenize(sToken, (int &) _N);
  if (sExt.GetLength() == 1)
  {
    return sDefault;
  }

  return sExt;
}

CString CKey::GetSignAsString(CString _sByteSeparator/*=_T(" ")*/) const
{
  return GetSignAsStringTrancated(m_dwSignSize, _sByteSeparator);
}

CString CKey::GetSignAsStringTrancated(DWORD _dwLen, CString _sByteSeparator/*=_T(" ")*/) const
{
  if (NULL == m_pSign || 0 == m_dwSignSize) {
    return _T("(empty)");
  }

  CString sSign;

  for (DWORD i = 0; i < m_dwSignSize && i < _dwLen; i++) {
    sSign.AppendFormat(_T("%s%.02X"), (0 == i? _T("") : _sByteSeparator), (UCHAR)m_pSign[i]);
  }

  if (_dwLen < m_dwSignSize) {
    sSign += _T("...");
  }

  return sSign;
}

ULARGE_INTEGER CKey::GetTotalMediaSize() const
{
  ULARGE_INTEGER qwTotal;
  qwTotal.QuadPart = 0;
  
  POSITION pos = m_MediaSet.GetHeadPosition();
  DWORD dwMediaCount = (DWORD) m_MediaSet.GetCount();

  for (DWORD i = 0; i < dwMediaCount; i++)
  {
    qwTotal.QuadPart += m_MediaSet.GetNext(pos)->m_qwSize.QuadPart;
  }

  return qwTotal;
}

CMedia* CKey::GetMedia(const CString _sName) const
{
  CMedia* pMedia = NULL;
  POSITION pos = m_MediaSet.GetHeadPosition();
  while (pos != NULL)
  {
    pMedia = m_MediaSet.GetNext(pos);
    if (pMedia != NULL && 0 == pMedia->m_sName.CompareNoCase(_sName))
    {
      return pMedia;
    }
  }

  return NULL;
}

void CKey::SetSignFromString(CString _sSign)
{
  _sSign.Trim();
  _sSign.MakeUpper();
  
  if (_sSign.GetLength() < 2)
  { // expect at least 1 byte signature ("FF")
    throw 0;
  }

  // approximate buffer size (larger or equal to the real data)
  DWORD dwSignSize = _sSign.GetLength() / 2;
  char* pSign = NULL;         // new signature buffer
  int iSignIndex = 0;
  bool bLow4bits = false;     // flag of half of byte
  wchar_t c;                  // curent char in parsing string
  CString sDelimiters(sPRG_KEY_SIGNATURE_DELIMETERS);

  // Allocate memory for new Signature
  pSign = (char *) calloc(dwSignSize, dwSignSize * sizeof (char));
  if (pSign == NULL)
  {
    throw _T("Insufficient memory resources for parsing the signature.");
  }

  // Start the parsing of the string
  int iStrIndex;
  for (iStrIndex = 0; (iStrIndex < _sSign.GetLength()); iStrIndex++)
  {
    c = _sSign[iStrIndex];

    if (sDelimiters.Find(c, 0) != -1)
    { // delimiter
      continue;
    }
    else
    { // not delimiter
      if (c >= 48 && c <= 57)
      { // numbers 0 <= 9
        c -= 48;
      }
      else if (c >= 65 && c <= 70)
      { // uppercase A <= F
        c -= 55;
      }
      else
      { // not a HEX character
        throw iStrIndex;
      }

      if (bLow4bits == false)
      {// set high-order 4 bits (they occures first)
        pSign[iSignIndex] = (char) (c << 4);
      }
      else
      {// set low-order 4 bits
        pSign[iSignIndex] |= (char) c;
        ++iSignIndex;
      }

      bLow4bits = !bLow4bits;
    }
  }

  if (bLow4bits == true)
  { // incomplate parsing
    throw (iStrIndex - 1);
  }

  // parsing complate successfuly

  // change signature buffers
  free(this->m_pSign); this->m_pSign = NULL;
  
  m_dwSignSize = iSignIndex;  // correct the buffer size (less or equal to dwSignSize)
  this->m_pSign = (char *) realloc(pSign, m_dwSignSize);
  if (this->m_pSign == NULL)
  {
    throw _T("Insufficient memory resources for parsing the signature.");
  }
}

CString CKey::GetCSOTypeAsString() const
{
  switch (m_dwCSOType)
  {
  case CSOTYPE_UNASSIGNED: return _T("(unassigned)");
  case CSOTYPE_CONTENT_SIZE: return _T("content");
  case CSOTYPE_TOTAL_SIZE: return _T("total");
  default: __assume(0);
  }
}

void CKey::Serialize(CArchive& ar)
{
  CString sSign;

  if (ar.IsStoring())
  {	// storing code
    sSign = GetSignAsString();

    ar << m_sName;
    ar << m_sComment;
    ar << sSign;
    ar << m_dwSignSize;
    ar << m_dwCSO;
    ar << m_dwCSOType;
    ar << m_dwSkipScope;
    ar << m_sExtensions;
  }
  else
  {	// loading code
    ar >> m_sName;
    ar >> m_sComment;
    ar >> sSign;
    ar >> m_dwSignSize;
    ar >> m_dwCSO;
    ar >> m_dwCSOType;
    ar >> m_dwSkipScope;
    ar >> m_sExtensions;

    try
    {
      SetSignFromString(sSign);
    }
    catch (int iErrPos)
    {
      CString sReport;
      sReport.Format(_T("The Signature string for key {%s}\n{%s}\ncontains invalid")
        _T(" character at {%d} position!"), m_sName, sSign, iErrPos);
      theApp.Report(sReport);
    }
    catch (LPCTSTR _sErr)
    {
      theApp.Report(_sErr);
    }
  }

  m_MediaSet.Serialize(ar);
}

bool CKey::Validate() const
{
  while (true)
  {

    BREAKIF(m_sName.IsEmpty() || m_sExtensions.IsEmpty());
    BREAKIF(-1 != m_sExtensions.FindOneOf(sPRG_KEY_EXTENSION_BADCHARS));
    // if CSO is defined its type can't be unassigned
    BREAKIF(0 != m_dwCSO && (DWORD)CKey::CSOTYPE_UNASSIGNED == m_dwCSOType);
    // if CSO is defined it can't point to signature
    BREAKIF(0 != m_dwCSO && m_dwSignSize > m_dwCSO);
    // signature should be defined
    BREAKIF(0 == m_dwSignSize || NULL == m_pSign);
    return true;
  }

  CString sReport;
  sReport.Format(_T("Internall validation failed for key: {%s}"), m_sName);
  theApp.Report(sReport);
  return false;
}