#include "StdAfx.h"
#include "CApp.h"
#include "CKeyList.h"

IMPLEMENT_SERIAL(CKeyList, CObject, 1);

CKeyList::CKeyList()
{
  root = NULL;
  m_count = 0;
}

CKeyList::CKeyList(const CKeyList&  _keyList)
{
  root = NULL;
  m_count = 0;

  for (CKey* pKey = _keyList.GetRoot(); pKey != NULL; pKey = pKey->Next())
  {
    this->AddKey(new CKey(*pKey));
  }
}

CKeyList& CKeyList::operator=(const CKeyList& /*_keyList*/)
{
  throw _T("DO NOT USE `CKeyList::operator=`");
  /*
  if (this == &_keyList)
  { // self-assignment
    return *this;
  }
  */
  return *this;
}

CKeyList::~CKeyList(void)
{
  DeleteAll();
}

void CKeyList::DeleteAll()
{
  CKey* delme = NULL;

  while (root != NULL)
  {
    delme = root;
    root = root->next;
    delete delme;
  }

  root = NULL;
  m_count = 0;
}

bool CKeyList::AddKey(CKey* _pKey)
{
  if (!_pKey->Validate())
  {
    return false;
  }
  
  if (NULL == _pKey)
  { // integrity check
    theApp.Report(_T("ERROR: void CKeyList::AddKey(CKey* _pKey) { ")
      _T("if (_pKey == NULL) } -> Can't add NULL pKey."));
    return false;
  }

  if (m_count == nPRG_MAX_KEY_COUNT)
  {
    theApp.Report(sPRG_MAX_KEY_COUNT_MESSAGE);
    return false;
  }

  if (NULL == root)
  { // set as root if list was empty till now
    root = _pKey;
    root->next = NULL;
    m_count = 1;
    return true;
  }

  // find pKey with the name less valuable in descending comparation
  CKey* pKey = root;
  CKey* pPrevKey = NULL;
  bool bAdded = false;
  int iCmpRes;

  while (pKey != NULL)
  {
    iCmpRes = _pKey->m_sName.CompareNoCase(pKey->m_sName);

    if (0 == iCmpRes)
    {
      theApp.Report(
      _T("DESIGN ERROR: void CKeyList::AddKey(CKey* _pKey) { ")
      _T("Key name uniqueness constraint fired."));

      return false;
    }
    else if (iCmpRes < 0)
    { // found
      if (pPrevKey == NULL)
      { // append as root
        _pKey->next = root;
        root = _pKey;
        bAdded = true;
        break;
      }
      else
      { // append before found pKey
        _pKey->next = pPrevKey->next;
        pPrevKey->next = _pKey;
        bAdded = true;
        break;
      }
    }
    else
    { // seek next
      pPrevKey = pKey;
      pKey = pKey->next;
    }
  }

  if (bAdded == false)
  { // append to the end of the list
    pPrevKey->next = _pKey;
    _pKey->next = NULL;  // already done in constructor, here for integrity.
  }

  ++m_count;
  return true;
}

DWORD CKeyList::Count() const
{
  return m_count;
}

CKey* CKeyList::GetKey(const CString _sKeyName) const
{
  CKey* pKey = root;

  while (pKey != NULL)
  {
    if (0 == pKey->m_sName.CompareNoCase(_sKeyName))
    {
      return pKey;
    }
    else
    {
      pKey = pKey->next;
    }
  }

  // if pKey with such a name isn't found...
  return NULL;
}

CKey* CKeyList::GetRoot() const
{
  return this->root;
}

void CKeyList::Delete(const CString _sKeyName)
{
  if (NULL == root)
  {
    return;
  }

  CKey* pKey = root;
  CKey* pPrevKey = NULL;

  while (pKey != NULL)
  {
    if (0 == pKey->m_sName.CompareNoCase(_sKeyName))
    {// found
      if (pPrevKey == NULL)
      {// it's a root
        root = pKey->next;
        delete pKey;

        --m_count;
        return;
      }
      else
      {// it's not root
        pPrevKey->next = pKey->next;
        delete pKey;

        --m_count;
        return;
      }
    }
    else
    { // not found - move to next
      pPrevKey = pKey;
      pKey = pKey->next;
    }
  }

  // if pKey with such a name isn't found...
  return;
}

void CKeyList::Absorb(const CKeyList* _pKeyList)
{
  if (_pKeyList == NULL)
  {
    theApp.Report(_T("ERROR: void CKeyList::Absorb(CKeyList* _pKeyList)")
      _T(" { (_pKeyList == NULL) } -> Can't absorb NULL!"));
    return;
  }

  this->DeleteAll();

  for (CKey* pKey = _pKeyList->GetRoot(); NULL != pKey; pKey = pKey->Next())
  {
    this->AddKey(new CKey(*pKey));
  }
}

ULARGE_INTEGER CKeyList::GetTotalMediaSize() const
{
  ULARGE_INTEGER qwTotal;
  qwTotal.QuadPart = 0;

  for (CKey* pKey = this->GetRoot(); NULL != pKey; pKey = pKey->Next())
  {
    qwTotal.QuadPart += pKey->GetTotalMediaSize().QuadPart;
  }

  return qwTotal;
}

void CKeyList::Serialize(CArchive& ar)
{
  CKey* pKey = NULL;

  if (ar.IsStoring())
  {	// storing code
    ar.WriteCount((DWORD_PTR) m_count);

    pKey = root;
    while (pKey != NULL)
    {
      pKey->Serialize(ar);
      pKey = pKey->next;
    }
  }
  else
  {	// loading code
    DWORD_PTR dwCount = (DWORD_PTR) ar.ReadCount();
    if (dwCount >= nPRG_MAX_KEY_COUNT)
    {
      ::MessageBox(theApp.m_pMainWnd->m_hWnd
        , sPRG_MAX_KEY_COUNT_MESSAGE2
        , sPRG_WARNING_CAPTION, MB_OK | MB_ICONHAND);
      return;
    }

    while (0 < dwCount--)
    {
      pKey = new CKey();
      pKey->Serialize(ar);
      this->AddKey(pKey);
    }
  }
}
