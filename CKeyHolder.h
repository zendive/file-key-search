#pragma once
#include "CKeyList.h"

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// validate registry function return value
#define REGOK(_ret)                   \
  if (_ret != 0) {                    \
    throw CKeyHolder::ERR_BAD_REG;   \
  }

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// validate pointer
#define PTROK(_ptr)                   \
  if (_ptr == NULL) {                 \
    throw CKeyHolder::ERR_LOW_MEM;   \
  }

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// ret: true or false if call to ShellExecute() is passed or not respectively
// (refer to MSDN about ret codes)
#define SHELLEXECUTE_OK(_ret)         \
  ((int)_ret > 32)

//•••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
// Manage private key-list along with providing interface for registry storage
// manipulations
class CKeyHolder
{
private:
  // error codes
  enum ERR
  {
    // registry function returned with error
    ERR_BAD_REG = 1,
    // low memory error
    ERR_LOW_MEM
  };
  // local path to application
  CString m_sAppPath;
  // list of keys
  CKeyList m_keyList;

  // Return new CKey* readen from the registry by key-name
  CKey* ReadRegistryForKeyName(const CString _sKeyName) const;
  // Execute REG File with the `regedit`. With afterward sleepping time.
  void Execute_REG_File(CString _sRegFile);
  // Escape characters to store in .REG
  CString StringEscape(CString _str);

public:
  CKeyHolder(void);
  ~CKeyHolder(void);
  const CKeyList* const GetKeyListPtr() const { return &m_keyList; }
  // Init KeyHolder.
  void Init(CString _sAppPath);
  // Load keys from Registry to local key-list (discarding current key-list-data)
  void LoadRegKeys();
  // Delete Key from the registry.
  ///void RegistryRemoveKey(const CString _sKeyName) const;
  // Delete ALL Keys from the registry
  void RegistryRemoveKeyList() const;
  // Store key in the Registry
  // return: true if added
  bool RegistrySaveKey(const IN CKey* _pKey) const;
  // Store key in registry and add to local key-list
  void RegistryAddKey(const IN CKey* _pKey);
  // Export key-list into the file. Ret: true if succeded.
  bool ExportToFile(CString _sFileName);
  // Import Keys from file with Import Type 
  // - Replace if Key has the same name.
  CString Import_Replace(CString _sImportFile, OUT bool &_bResult);
  // Import Keys from file with Import Type 
  // - Don't replace if Key has the same name.
  CString Import_Skip(CString _sImportFile, OUT bool &_bResult);
  // Import Keys from file with Import Type 
  // - Clear all current Keys & import from file.
  CString Import_Clear(CString _sImportFile, OUT bool &_bResult);
  // Get count of keys
  DWORD Count() const;
  // Absorb external list inside
  void Absorb(CKeyList* _pKeyList);
};
