#pragma once
#include <afx.h>
#include "CKey.h"

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// Linked-List manager of CKey
class CKeyList: public CObject
{
  DECLARE_SERIAL(CKeyList);

private:
  // the head of the list
  CKey* root;
  // number of keyst in the list
  DWORD m_count;

public:
  CKeyList();
  CKeyList(const CKeyList& _keyList);
  CKeyList& operator=(const CKeyList& /*_keyList*/);
  ~CKeyList();

  // Add new key keeping the sort order by names
  // Ret: true on success, false otherwize
  bool AddKey(CKey* _newKey);
  // Get number of keys in the list
  DWORD Count() const;
  // Get key by it's name (case insensitive compare)
  // return NULL when not found
  CKey* GetKey(const CString _sKeyName) const;
  // Return head of the List
  CKey* GetRoot() const;
  // Clear key list content
  void DeleteAll();
  // Delete key by name
  void Delete(const CString _sKeyName);
  // Absorb external list inside
  void Absorb(const CKeyList* _pKeyList);
  // Get total media-set size
  ULARGE_INTEGER GetTotalMediaSize() const;
  virtual void Serialize(CArchive& ar);
};
