#include "stdafx.h"
#include "CConvert.h"

CConvert::CConvert()
{
}

CConvert::~CConvert()
{
}

CString CConvert::NumberToString(DWORD _dwNumber)
{
  ULARGE_INTEGER qwNumber;
  qwNumber.QuadPart = _dwNumber;

  return CConvert::NumberToString(qwNumber);
}

CString CConvert::NumberToString(ULARGE_INTEGER _qwNumber)
{
  CString sNumber;
  sNumber.Format(_T("%I64u"), _qwNumber.QuadPart);
  return sNumber;
}

CString CConvert::NumberToHumanString(DWORD _dwNumber)
{
  ULARGE_INTEGER qwNumber;
  qwNumber.QuadPart = _dwNumber;

  return CConvert::NumberToHumanString(qwNumber);
}

CString CConvert::NumberToHumanString(ULARGE_INTEGER _qwNumber)
{
  CString sNumber = CConvert::NumberToString(_qwNumber);
  DWORD dwNumberLen = sNumber.GetLength();  // incoming number length
  DWORD dwSplitCount = dwNumberLen / 3;     // number of split times
  DWORD dwInsIndex = 0;                     // delimeter insertion position
  CString sDelimiter = _T("'");

  if (dwNumberLen > 3)
  { // split aquaired
    while (0 != dwSplitCount)
    { // while need to split
      dwInsIndex = dwNumberLen - (dwSplitCount * 3);

      if (0 != dwInsIndex)
      { // exclude '000'000 situation
        sNumber.Insert(dwInsIndex, sDelimiter);

        ++dwNumberLen;
      }

      --dwSplitCount;
    }
  }

  return sNumber;
}

CString CConvert::NumberToKibiSizeString(DWORD _dwSizeInBytes)
{
  ULARGE_INTEGER qwNumber;
  qwNumber.QuadPart = _dwSizeInBytes;

  return CConvert::NumberToKibiSizeString(qwNumber);
}

CString CConvert::NumberToKibiSizeString(ULARGE_INTEGER _qwSizeInBytes)
{
  const int MAX_POSTFIXES = 9;
  wchar_t saPostfix[MAX_POSTFIXES][4*sizeof(TCHAR)] = {
    _T("B"), _T("KiB"), _T("MiB")
    , _T("GiB"), _T("TiB"), _T("PiB")
    , _T("EiB"), _T("ZiB"), _T("YiB")
    
  };
  int indx = 0;
  ULARGE_INTEGER qwDevider;
  qwDevider.QuadPart = 1;
  CString sPostfix(_T("?"));

  do  // determine devider and postfix
  {
    qwDevider.QuadPart *= 1024;
    sPostfix = saPostfix[indx++];
  }
  while ((_qwSizeInBytes.QuadPart > qwDevider.QuadPart) && (indx < MAX_POSTFIXES));
  qwDevider.QuadPart /= 1024;

  // construct string
  _qwSizeInBytes.QuadPart /= qwDevider.QuadPart;
  CString sSize = CConvert::NumberToHumanString(_qwSizeInBytes);

  CString sSizePostfixed;
  sSizePostfixed.Format(_T("%s %s"), sSize, sPostfix);

  return sSizePostfixed;
}

CString CConvert::NumberToSizeString(DWORD _dwSizeInBytes)
{
  ULARGE_INTEGER qwNumber;
  qwNumber.QuadPart = _dwSizeInBytes;

  return CConvert::NumberToSizeString(qwNumber);
}

CString CConvert::NumberToSizeString(ULARGE_INTEGER _qwSizeInBytes)
{
  const int MAX_POSTFIXES = 9;
  wchar_t saPostfix[MAX_POSTFIXES][3*sizeof(TCHAR)] = {
    _T("B"), _T("KB"), _T("MB")
    , _T("GB"), _T("TB"), _T("PB")
    , _T("EB"), _T("ZB"), _T("YB")
    
  };
  int indx = 0;
  ULARGE_INTEGER qwDevider;
  qwDevider.QuadPart = 1;
  CString sPostfix(_T("?"));

  do  // determine devider and postfix
  {
    qwDevider.QuadPart *= 1000;
    sPostfix = saPostfix[indx++];
  }
  while ((_qwSizeInBytes.QuadPart > qwDevider.QuadPart) && (indx < MAX_POSTFIXES));
  qwDevider.QuadPart /= 1000;

  // construct string
  _qwSizeInBytes.QuadPart /= qwDevider.QuadPart;
  CString sSize = CConvert::NumberToHumanString(_qwSizeInBytes);

  CString sSizePostfixed;
  sSizePostfixed.Format(_T("%s %s"), sSize, sPostfix);

  return sSizePostfixed;
}