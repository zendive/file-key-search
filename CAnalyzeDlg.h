#pragma once
#include "stdafx.h"
#include "resource.h"
#include ".\CAnalyzeThread.h"

class CDoc;

class CAnalyzeDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnalyzeDlg)

private:
  enum TIMER
  {
    TIMER_ID_PROGRESS = 5,
    TIMER_ID_PROGRESS_ELAPSE = 1000
  };

  // mimic selection index of `m_cmbMemory`
  enum MEMORY
  {
    MEMORY_HIGH = 0, MEMORY_NORMAL, MEMORY_LOW
  };

  // progressbar foregraund color
  enum PB_COLOR
  {
    // green while running
    PB_COLOR_IS_OK     = RGB(0x00, 0xFF, 0x00),
    // yellow while suspended
    PB_COLOR_IS_PAUSED = RGB(0xFF, 0xFC, 0x06),
    // red on trouble
    PB_COLOR_IS_ERROR  = RGB(0xFF, 0x00, 0x00)
  };

  // ptr on document
  CDoc* m_pDoc;
  // analyze progress reflection in %
  DWORD m_dwTotalProgress;
  // update dlg timer
  UINT_PTR m_hTimerPrg;
  // array of threads
  CAnalyzeThread** m_paThreads;
  // array size
  DWORD m_dwThreads;
  // pause/resume flag
  bool m_bPaused;
  // indication that analyze is canceled [default: true]
  bool m_bNeedAbort;
  // previous progress-bar foreground color
  COLORREF m_clrPrevPBColor;
  // Display total progress of analyze
  CProgressCtrl m_prgrsTotal;
  // Button for Pause/Resume
  CButton m_btnPauseResume;
  // Main cancel button
  CButton m_btnCancel;
  // Combo box for memory usage amount
  CComboBox m_cmbMemory;
  // statistics information field
  CStatic m_txtStatistics;

public:
  // at the end of analyze has information
  // about type of the rised error in group of threads
  CString m_sErrorSummary;
  // number of total found medies
  DWORD m_dwTotalFound;

private:
  // private default constructor - must be never used (see public constructor)
  // standard constructor
  CAnalyzeDlg(CWnd* pParent = NULL);
  void SetDialogCaption();
  void InitTimer();
  void InitProgressBar();
  void InitMemoryCombobox();
  // normal code-flow - start analyze
  void Start();
  // collect error messages from threads, if any
  void CollectErrorInformation();
  void UpdateStatistics(DWORD _dwBusyTreads, ULARGE_INTEGER _qwBPMS, ULARGE_INTEGER _qwBTRLeft);
  // normal code-flow - stop analyze (self-complete stop)
  void Stop();
  void AbortAllThreads();
  void DestroyThreads();

public:
  CAnalyzeDlg(CWnd* pParent, CDoc* _pDoc);
	virtual ~CAnalyzeDlg();

	enum { IDD = IDD_ANALYZE_DLG };

protected:
  // DDX/DDV support
	virtual void DoDataExchange(CDataExchange* pDX);
  // abnormal && after self-complete code-flow
  virtual void OnCancel();
  virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnBnClickedBtnPause();
  afx_msg void OnCbnSelchangeCmbMemory();
};
