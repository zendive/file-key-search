#include "StdAfx.h"
#include ".\CApp.h"
#include ".\CKeyHolder.h"

CKeyHolder::CKeyHolder(void)
{
}

CKeyHolder::~CKeyHolder(void)
{
}

void CKeyHolder::Init(CString _sAppPath)
{
  m_sAppPath = _sAppPath;
}

void CKeyHolder::LoadRegKeys()
{
  CRegKey reg;
  CKey* pKey = NULL;

  try
  {
    REGOK(reg.Create(HKEY_CURRENT_USER, sPRG_REGPATH_KEYS, REG_NONE
      , REG_OPTION_NON_VOLATILE, KEY_READ) );

    m_keyList.DeleteAll();

    LONG lRetCode = 0;
    DWORD dwEnum = 0;
    wchar_t wchKeyName[nPRG_MAX_KEYNAME_SIZE];
    DWORD dwValueSize;

    while (true)
    {
      dwValueSize = nPRG_MAX_KEYNAME_SIZE;
      lRetCode = reg.EnumKey(dwEnum, wchKeyName, &dwValueSize);
      switch (lRetCode)
      {
      case ERROR_NO_MORE_ITEMS:
        reg.Close();
        return;

      case ERROR_MORE_DATA: // key name is to big
        {
        CString sReport;
        sReport.Format(_T("Loading Key #%d was skipped ")
          _T("because his key-name is too long (%d).")
          _T(" Valid key-name length must be less than {%d}.")
          , (dwEnum + 1)
          , dwValueSize
          , nPRG_MAX_KEYNAME_SIZE);
        theApp.Report(sReport);
        break;
        }

      default:
        {
          pKey = this->ReadRegistryForKeyName(wchKeyName);
          this->m_keyList.AddKey(pKey);
        }
      }

      ++dwEnum;
    }
  }
  catch (CKeyHolder::ERR _err)
  {
    reg.Close();

    switch (_err)
    {
    case CKeyHolder::ERR_BAD_REG:
      theApp.Report(_T("The Key's storage in registry has found corrupted.")
        _T(" Loading Keys has been interrupted.")
        _T(" You may want to Import previously saved keys to solve this problem."));
      break;

    case CKeyHolder::ERR_LOW_MEM:
      exit(-1);
      break;
    }
  }
}

CKey* CKeyHolder::ReadRegistryForKeyName(const CString _sKeyName) const
{
  if (_sKeyName == _T(""))
  {
    throw CKeyHolder::ERR_BAD_REG;
  }

  CRegKey reg;              // key in the registry
  CString sRegPath;         // path to the key in the registry
  CKey* pKey = NULL;         // key to build
  wchar_t* sValue = NULL;   // value buffer
  DWORD dwValueSize = 0;    // value buffer size

  try
  {
    pKey = new CKey();
    pKey->m_sName = _sKeyName;

    // Jump into key name and load information about that key
    sRegPath.Format(_T("%s\\%s"), sPRG_REGPATH_KEYS, pKey->m_sName);
    REGOK(reg.Open(HKEY_CURRENT_USER, sRegPath, KEY_READ));

    // Retrieve Content-size offset
    REGOK(reg.QueryDWORDValue(sPRG_KEY_CSO, pKey->m_dwCSO));

    // Retrieve CSO type
    REGOK(reg.QueryDWORDValue(sPRG_KEY_CSOTYPE, pKey->m_dwCSOType));

    // Retrieve skip scope
    REGOK(reg.QueryDWORDValue(sPRG_KEY_SKIPSCOPE, pKey->m_dwSkipScope));

    // Retrieve signature size
    REGOK(reg.QueryDWORDValue(sPRG_KEY_SIGNSIZE, pKey->m_dwSignSize));

    // Retrieve signature array itself
    // ...prepeare (allocate with its size)...
    pKey->m_pSign = (char *) calloc(pKey->m_dwSignSize, sizeof (char));
    PTROK(pKey->m_pSign);
    // ...retrieve...
    REGOK(reg.QueryBinaryValue(sPRG_KEY_SIGNATURE, pKey->m_pSign, &(pKey->m_dwSignSize)));

    // Retrieve key comment
    // ...prepeare (allocate with default size)...
    dwValueSize = nPRG_MAX_KEYCOMMENT_SIZE;
    sValue = (wchar_t *) calloc(dwValueSize, sizeof (wchar_t));
    PTROK(sValue);
    // ...retrieve...
    REGOK(reg.QueryStringValue(sPRG_KEY_COMMENT, sValue, &dwValueSize));
    pKey->m_sComment = CString(sValue);
    free(sValue);
    sValue = NULL;

    // Retrieve extension set
    // ...prepeare...
    dwValueSize = nPRG_MAX_KEYEXTENSION_SIZE;
    sValue = (wchar_t *) calloc(dwValueSize, sizeof (wchar_t));
    PTROK(sValue);
    // ...retrieve...
    REGOK(reg.QueryStringValue(sPRG_KEY_EXTENSIONS, sValue, &dwValueSize));
    pKey->m_sExtensions = CString(sValue);
    free(sValue);
    sValue = NULL;
  }
  catch (CKeyHolder::ERR _err)
  {
    if (pKey != NULL)
    {
      delete pKey;
      pKey = NULL;
    }

    if (_err == CKeyHolder::ERR_BAD_REG)
    {
      CString sErrReport;
      sErrReport.Format(_T("The Key definition for key-name={%s}")
        _T(" has found corrupted while Loading Keys from the Registry.")
        _T(" Loading of this key has been skipped"), _sKeyName);
      theApp.Report(sErrReport);
    }

    reg.Close();
    throw _err;
  }

  reg.Close();
  return pKey;
}
/*
void CKeyHolder::RegistryRemoveKey(CString _sKeyName) const
{
  CRegKey reg;

  REGOK(reg.Create(HKEY_CURRENT_USER, sPRG_REGPATH_KEYS));
  REGOK(reg.RecurseDeleteKey(_sKeyName));
  reg.Close();
}
*/
void CKeyHolder::RegistryRemoveKeyList() const
{
  CRegKey reg;

  REGOK(reg.Create(HKEY_CURRENT_USER, sPRG_REGPATH_VER, REG_NONE
    , REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE));
  REGOK(reg.RecurseDeleteKey(sPRG_REGNAME_KEYS));
  reg.Close();
}

bool CKeyHolder::RegistrySaveKey(const IN CKey* _key) const
{
  if (NULL == _key) { return false; }
  CRegKey reg;
  CString sRegPath;
  bool bOk = false;

  try
  {
    sRegPath.Format(_T("%s\\%s"), sPRG_REGPATH_KEYS, _key->m_sName);
    
    REGOK(reg.Create(HKEY_CURRENT_USER, sRegPath, REG_NONE
      , REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE));
    REGOK(reg.SetDWORDValue(sPRG_KEY_SIGNSIZE, _key->m_dwSignSize));
    REGOK(reg.SetDWORDValue(sPRG_KEY_CSO, _key->m_dwCSO));
    REGOK(reg.SetDWORDValue(sPRG_KEY_CSOTYPE, _key->m_dwCSOType));
    REGOK(reg.SetDWORDValue(sPRG_KEY_SKIPSCOPE, _key->m_dwSkipScope));
    REGOK(reg.SetBinaryValue(sPRG_KEY_SIGNATURE, _key->m_pSign, _key->m_dwSignSize));
    REGOK(reg.SetStringValue(sPRG_KEY_COMMENT, _key->m_sComment));
    REGOK(reg.SetStringValue(sPRG_KEY_EXTENSIONS, _key->m_sExtensions));

    bOk = true;
  }
  catch (CKeyHolder::ERR)
  {
    CString sReport;
    sReport.Format(_T("ERROR: Could not save key {%s} in registry."), _key->m_sName);
    theApp.Report(sReport);
    bOk = false;
  }

  reg.Close();
  return bOk;
}

void CKeyHolder::RegistryAddKey(const IN CKey* _pKey)
{
  if (NULL == _pKey)
  {
    return;
  }

  if (this->m_keyList.AddKey(new CKey(*_pKey)))
  {
    this->RegistrySaveKey(_pKey);
  }
}

CString CKeyHolder::StringEscape(CString _str)
{
  _str.Replace(_T("\\"), _T("\\\\"));
  _str.Replace(_T("\""), _T("\\\""));
  return _str;
}

bool CKeyHolder::ExportToFile(CString _sFileName)
{
  if (_sFileName.IsEmpty())
  {
    theApp.Report(
      _T("INNER ERROR: void CKeyHolder::SaveToFile(CString _sFileName)")
      _T(" { (_sFileName == _T(\"\")) } -> Undefined file-name to save into."));
    return false;
  }

  SYSTEMTIME st;
  GetLocalTime(&st);

  // Set first 2 bytes to Unicode mask (in the file it will be like "FF FE")
  CString str(_T("\xFEFF"));
  str.Append(_T("REGEDIT4\r\n"));
  str.Append(_T("\r\n;-------------------------------------------------------------------------------"));
  str.Append(_T("\r\n; ") sPRG_APP_NAME _T(" keys definition file"));
  str.Append(_T("\r\n; This file was automaticaly generated by Export command"));
  str.Append(_T("\r\n; Please, do not edit"));
  str.Append(_T("\r\n; ---"));
  str.Append(_T("\r\n; To Import this key definitions use Import command from application menu:"));
  str.Append(_T("\r\n; [Keys] -> [Import from File...]"));
  str.Append(_T("\r\n;-------------------------------------------------------------------------------"));
  str.AppendFormat(_T("\r\n; Time stamp: {%d/%.02d/%.02d[%d] @ %.02d:%.02d:%.02d.%.03d}")
    , st.wYear, st.wMonth, st.wDay, (st.wDayOfWeek + 1)
    , st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  str.AppendFormat(_T("\r\n; Total Keys: {%d}"), m_keyList.Count());
  str.AppendFormat(_T("\r\n; Version:    {%s}"), sPRG_VERSION);
  str.AppendFormat(_T("\r\n; OS:         {%s}"), CApp::s_sSystemInfo);
  str.Append(_T("\r\n;-------------------------------------------------------------------------------"));

  for (CKey* key = m_keyList.GetRoot(); NULL != key; key = key->Next())
  {
    str.Append(_T("\r\n"));

    str.AppendFormat(_T("\r\n[HKEY_CURRENT_USER\\%s\\%s]"), sPRG_REGPATH_KEYS, key->m_sName);
    str.AppendFormat(_T("\r\n\"%s\"=\"%s\""), sPRG_KEY_COMMENT, this->StringEscape(key->m_sComment));
    str.AppendFormat(_T("\r\n\"%s\"=hex:%s"), sPRG_KEY_SIGNATURE, key->GetSignAsString(_T(",")));
    str.AppendFormat(_T("\r\n\"%s\"=dword:%.08X"), sPRG_KEY_SIGNSIZE, key->m_dwSignSize);
    str.AppendFormat(_T("\r\n\"%s\"=dword:%.08X"), sPRG_KEY_CSO, key->m_dwCSO);
    str.AppendFormat(_T("\r\n\"%s\"=dword:%.08X"), sPRG_KEY_CSOTYPE, key->m_dwCSOType);
    str.AppendFormat(_T("\r\n\"%s\"=dword:%.08X"), sPRG_KEY_SKIPSCOPE, key->m_dwSkipScope);
    str.AppendFormat(_T("\r\n\"%s\"=\"%s\""), sPRG_KEY_EXTENSIONS, this->StringEscape(key->m_sExtensions));
  }
  str.Append(_T("\r\n\r\n"));
  
  HANDLE hFile = CreateFile(_sFileName, GENERIC_WRITE, FILE_SHARE_WRITE,
    NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE || hFile == 0)
  {
    CString sReport;
    sReport.Format(_T("Can't Export to file={%s}. Failed while file creation.")
      , _sFileName);
    theApp.Report(sReport);
    return false;
  }

  DWORD dwToWrite = str.GetLength() * sizeof(TCHAR);
  DWORD dwWriten = 0;
  BOOL bRetCode = WriteFile(hFile, str, dwToWrite, &dwWriten, NULL);
  CloseHandle(hFile);

  if ((bRetCode == 0) || (dwToWrite != dwWriten))
  {
    CString sReport;
    sReport.Format(_T("Can't Export to file={%s}. Failed after writing.")
      , _sFileName);
    theApp.Report(sReport);
    return false;
  }

  return true;
}

// Import Keys from file with Import Type 
// - Replace if Key has the same name.
//
CString CKeyHolder::Import_Replace(CString _sImportFile, OUT bool &_bResult)
{
  CString sHistory(_T("\r\n- Registry update:"));
  CString sTemp;

  // Load keys from file to the registry
  try
  {
    Execute_REG_File(_sImportFile);
    sHistory += _T("\tDONE");
    _bResult = true;
  }
  catch (CString _sErr)
  {
    theApp.Report(_sErr);
    sHistory.AppendFormat(_T("\tFAILED!\r\n!- %s"), _sErr);
    _bResult = false;

    return sHistory;
  }

  // Reread keys from registry
  try
  {
    sHistory += _T("\r\n- Reloading Keys:");
    LoadRegKeys();
    sHistory.AppendFormat(_T("\tDONE\r\n- Total {%d} Keys reloaded."), m_keyList.Count());
    _bResult = true;    // the last point of success definition
  }
  catch (CString _sErr)
  {
    theApp.Report(_sErr);
    sHistory.AppendFormat(_T("\tFAILED!\r\n!- %s"), _sErr);
    _bResult = false;
  }

  return sHistory;
}

// Import Keys from file with Import Type 
// - Don't replace if Key has the same name.
CString CKeyHolder::Import_Skip(CString _sImportFile, OUT bool &_bResult)
{
  CString sHistory;
  CString sTemp;

  // Reserve Old Keys
  CKeyList oldKeyList(m_keyList);
  sHistory.AppendFormat(_T("\r\n- {%d} Keys reserved"), oldKeyList.Count());
  
  // Load keys to regitry with replacing equal keys
  // NOTE: that updates `m_keyList`
  sHistory += Import_Replace(_sImportFile, OUT _bResult);

  if (_bResult == false)
  {
    return sHistory;
  }

  // Make the equal-named keys to rollback to previous data
  // with separated logging of Added and Skipped keys

  CKey* oldKey = NULL;

  // walk through new keys
  for (CKey* key = m_keyList.GetRoot(); NULL != key; key = key->Next())
  { // search for coincidence in old list
    oldKey = oldKeyList.GetKey( key->m_sName );
    if (oldKey != NULL)
    { // restore key from oldKeyList
      key->Copy(oldKey);
      
      // update the registry with `oldKey`
      if (this->RegistrySaveKey(oldKey) == false)
      {
        sTemp.Format(_T("FAILED to store old Key: {%s} in the registry.")
          _T("\r\nThe old Key's data are still in memory.")
          _T("\r\nProceeding with normal operation.")
          , key->m_sName);

        theApp.Report(sTemp);
        sHistory.AppendFormat(_T("\r\n\t%s"), sTemp);
      }
      else
      { // registry updated
        sHistory.AppendFormat(_T("\r\n\tSkipped Key:\t{%s}"), key->m_sName);
      }
    }
    else
    { // not found in old keys - report about new key
      sHistory.AppendFormat(_T("\r\n\tAdded New Key:\t{%s}"), key->m_sName);
    }
  }

  _bResult = true;

  return sHistory;
}

// Import Keys from file with Import Type
// - Clear all current Keys & import from file.
CString CKeyHolder::Import_Clear(CString _sImportFile, OUT bool &_bResult)
{
  CString sHistory(_T("\r\n- Removing Keys:"));
  CString sTemp;

  // Removing all registry keys

  try
  {
    this->RegistryRemoveKeyList();
    sHistory += _T("\tDONE");
    _bResult = true;
  }
  catch(CString _sErr)
  {
    theApp.Report(_sErr);
    sHistory += _T("\tFAILED!");
    _bResult = false;
    return sHistory;
  }

  // Load keys from file
  try
  {
    sHistory += _T("\r\n- Registry update:");

    Execute_REG_File(_sImportFile);

    sHistory += _T("\tDONE");
    _bResult = true;
  }
  catch (CString _sErr)
  {
    theApp.Report(_sErr);
    sHistory += _T("\tFAILED!");
    _bResult = false;

    return sHistory;
  }

  // Reread keys from registry

  try
  {
    sHistory += _T("\r\n- Reloading Keys:");

    LoadRegKeys();

    sTemp.Format(_T("\tDONE\r\n- Total {%d} Keys reloaded."), m_keyList.Count());
    sHistory += sTemp;
    _bResult = true;    // the last point of success definition
  }
  catch (CString _sErr)
  {
    theApp.Report(_sErr);
    sHistory += _T("\tFAILED!");
    _bResult = false;

    return sHistory;
  }

  return sHistory;
}

// Execute REG File with the `regedit`. With afterward sleepping time.
void CKeyHolder::Execute_REG_File(CString _sRegFile)
{
  RETURNIF(_sRegFile.IsEmpty());
  CString sLog;

  HANDLE hFile = CreateFile(_sRegFile, GENERIC_READ, FILE_SHARE_READ
    , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE || hFile == 0)
  {
    sLog.Format(_T("REG file: {%s} -> Not found."), _sRegFile);
    throw sLog;
  }
  else
  { // file exist, continue...
    CloseHandle(hFile);
  }

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  SecureZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.wShowWindow = SW_HIDE;
  SecureZeroMemory(&pi, sizeof(pi));

  CString sRegedit;
  THROWIF(0 == sRegedit.GetEnvironmentVariable(_T("SystemRoot"))
    , _T("Can not locate %SystemRoot% system variable."));
  sRegedit.Append(_T("\\regedit.exe"));

  CString sCommand;
  sCommand.AppendFormat(_T("\"%s\" /s \"%s\""), sRegedit, _sRegFile);

  if (FALSE == ::CreateProcess(NULL, sCommand.GetBuffer()
    , NULL, NULL, FALSE, 0, NULL, m_sAppPath, &si, &pi))
  {
    sLog.Format(_T("Failed to execute import command: {%s}"), sCommand);
    throw sLog;
  }

  // Wait until child processes exit
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

// Get count of keys.
DWORD CKeyHolder::Count() const
{
  return m_keyList.Count();
}

// Absorb external list inside
void CKeyHolder::Absorb(CKeyList* _pKeyList)
{
  if (NULL == _pKeyList)
  {
    theApp.Report(_T("ERROR: void CKeyHolder::Absorb(CKeyList* _pKeyList) ")
      _T("{ (_pKeyList == NULL) } -> Can't do anyfing with NULL."));
    return;
  }

  RegistryRemoveKeyList();

  // remove keys in list and load from `_pKeyList`
  m_keyList.Absorb(_pKeyList);

  CKey* key = NULL;

  // store keys in registry
  for (key = m_keyList.GetRoot(); (NULL != key); key = key->Next())
  {
    RegistrySaveKey(key);
  }
}
