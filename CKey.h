#pragma once
#include <afx.h>
#include "CMedia.h"

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// Key signature
class CKey : public CObject
{
  DECLARE_SERIAL(CKey);
  friend class CKeyList;  // only CKeyList may access private members

public:
  // Content-size offset type can be
  enum CSOTYPE
  {
    // no type is assigned.
    CSOTYPE_UNASSIGNED = 0,
    // local size, when size of the item represent only
    // - size of the content WITHOUT header and CSO.
    CSOTYPE_CONTENT_SIZE = 1,
    // total size, when size of the item include inself
    // - size of the header and CSO value.
    CSOTYPE_TOTAL_SIZE = 2
  };

  // LEGEND: [R] - registry storred; [S] - serializable; [C] - copied to another key instance

  // [RSC] Display Name / Registry Name
  CString m_sName;
  // [RSC] Comment
  CString m_sComment;
  // [RSC] Ptr. on signature
  char* m_pSign;
  // [RSC] Signature size
  DWORD m_dwSignSize;
  // [RSC] Content-Size Offset
  DWORD m_dwCSO;
  // [RSC] Item size calculation type
  DWORD m_dwCSOType;
  // [RSC] Skip scope for skiping multiple found signatures (900 for JFIF)
  DWORD m_dwSkipScope;
  // [RSC] Extensions set ("jpg;jpeg;jfif;hex", "bin;dat;hex", etc.)
  CString m_sExtensions;
  // [S] found multimedia
  CMediaSet m_MediaSet;

private:
  // [] ptr on next key in linked list
  CKey* next;

public:
  CKey();
  // Key copy constructor
  CKey(const CKey& _key);
  // assignment
  CKey& operator=(const CKey &_key);
  // Key template with a given name
  CKey(const CString _sNewName);
  virtual ~CKey();
  CKey* Next() const;
  // Copy given key definition
  void Copy(const CKey* _key);
  // Return true if identical
  bool Compare(const CKey* _key) const;
  bool operator == (CKey* _key) const { return Compare(_key); };
  bool operator != (CKey* _key) const { return !Compare(_key); };
  // Delete all found media and free memory
  void DeleteMediaSet();
  // Calculate Item size relative to CSO Type
  QWORD GetItemSize(const QWORD _i64ContentSize) const;
  // Get First extension
  CString GetExtension() const;
  // Get extension at index `_N`
  CString GetExtensionAt(DWORD _N) const;
  // Get signature as string
  CString GetSignAsString(CString _sByteSeparator=_T(" ")) const;
  // Return first `_dwLen` bytes of signature as string
  CString GetSignAsStringTrancated(DWORD _dwLen, CString _sByteSeparator=_T(" ")) const;
  // Get total media-set size
  ULARGE_INTEGER GetTotalMediaSize() const;
  // Get Media by name (case insensitive compare)
  // return NULL when not found
  CMedia* GetMedia(const CString _sName) const;
  // Set signature from string; May throw int with invalid position or LPCTSTR;
  void SetSignFromString(CString _sSign);
  // Get CSO Type as string
  CString GetCSOTypeAsString() const;
  // Serialize internall data members (those marked [S])
  virtual void Serialize(CArchive& ar);
  // Validate internal state; return true if valid
  bool Validate() const;
};
