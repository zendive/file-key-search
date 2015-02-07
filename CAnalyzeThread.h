#pragma once
#include "stdafx.h"
#include ".\CKey.h"
#include ".\CPExTA.h"

class CAnalyzeThread : public CWinThread
{
  DECLARE_DYNCREATE(CAnalyzeThread);

private:
  // search inner self state...
  enum SISS
  {
    // common state
    SISS_GENERAL = 100,
    // need next buffer
    SISS_NEED_NEXT,
    // find proc in tracking state, need `ret` bytes to complete
    SISS_SHORTAGE,
    // while offset depended search need concretized position
    // (in `ret`) in processed file
    SISS_DO_JUMP
  };

  // proc execution through total analyze
  CPExTA m_peTotal;
  ULARGE_INTEGER m_qwFileSize;
  ULARGE_INTEGER m_qwBufOffset;
  // total bytes readen through analyze
  ULARGE_INTEGER m_qwTotalBR;

public:
  // error code
  enum ERR
  {
    // analyze done without error
    ERR_ALRIGHT = 0,
    // maximum media items is found
    ERR_ITEMS_LIMIT,
    // file operation problems
    ERR_DEADFILE,
    // insufficient memory resources
    ERR_MEMOVERLOAD
  };

  // file to analyze
  CString m_sFile;
  // media information to find
  CKey* m_pKey;
  // complation status flag
  bool m_bDone;
  // abort analyze flag
  bool m_bAbort;
  // pause analyze flag
  bool m_bPaused;
  // progres status in percents
  DWORD m_dwProgress;
  // number of found media items
  DWORD m_dwFound;
  // error state according to `CAnalyzeThread:ERR`
  ERR m_ErrState;
  // bytes to read left till the EOF
  ULARGE_INTEGER m_qwBTRLeft;
  DWORD m_dwBufferAllocationSize;
  CRITICAL_SECTION m_csBufferAllocationSize;

private:
  void Start();
  // calculate Content Size by flipping bytes
  // also known as Big-Endian to Little-Endian conversion
  DWORD GetContentSize32(char* _buf);
  // ret: 0 on error
  HANDLE PrepeareFile();
  signed __int64 Find(char* _buf, DWORD _dwBufSize, OUT DWORD &_SISS);

public:
  // Calculate current Bytes Per Millisecond
  DWORD GetBPMS();

  CAnalyzeThread(void);
  virtual ~CAnalyzeThread(void);
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()
};
