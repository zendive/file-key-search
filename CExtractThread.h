#pragma once
#include "CKeyList.h"
#include "CPExTA.h"

class CExtractThread : public CWinThread
{
	DECLARE_DYNCREATE(CExtractThread)

private:
  // proc execution through total analyze
  CPExTA m_peTotal;

public:
  enum ERR
  {
    // analyze done without error
    ERR_ALRIGHT = 0,
    // file operation problems
    ERR_DEADFILE,
    // insufficient memory resources
    ERR_MEMOVERLOAD
  };
  ERR m_ErrState;
  CKeyList* m_pKeyList;
  CString m_sTargetFileName;
  CString m_sExtractDir;
  CString m_sMediaFileName;
  ULARGE_INTEGER m_uliTotalBytesReaden;
  // complation status flag
  bool m_bDone;
  // abort extract flag
  bool m_bAbort;
  // pause extract flag
  bool m_bPaused;

private:
  void Start();

public:
  // Calculate current Bytes Per Millisecond
  DWORD GetBPMS();

	CExtractThread();
	virtual ~CExtractThread();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
  virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()
};
