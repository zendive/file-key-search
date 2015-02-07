#pragma once
#include <afx.h>

// CConvert class provides only static methods.
class CConvert
{
private:
  CConvert();

public:
  ~CConvert();
 
  // Convert Number to string
  static CString NumberToString(DWORD _dwNumber);
  static CString NumberToString(ULARGE_INTEGER _qwNumber);

  // Convert Number to string with Hundreds parts splitting
  // in the way like: 1'000'000
  static CString NumberToHumanString(DWORD _dwNumber);
  static CString NumberToHumanString(ULARGE_INTEGER _qwNumber);

  // Convert Number to size string with Hundreds parts splitting
  // in the way like: 20 MiB
  static CString NumberToKibiSizeString(DWORD _dwSizeInBytes);
  static CString NumberToKibiSizeString(ULARGE_INTEGER _qwSizeInBytes);
  
  // Convert Number to size string with Hundreds parts splitting
  // in the way like: 20 MB
  static CString NumberToSizeString(DWORD _dwSizeInBytes);
  static CString NumberToSizeString(ULARGE_INTEGER _qwSizeInBytes);
};
