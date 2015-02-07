#pragma once
#include "stdafx.h"
#include "resource.h"
#include "CExtractThread.h"

class CDoc;

class CMediaExtractDlg : public CDialog
{
	DECLARE_DYNAMIC(CMediaExtractDlg)

private:
  enum TIMER
  {
    TIMER_ID_PROGRESS = 6,
    TIMER_ID_PROGRESS_ELAPSE = 1000
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

  CExtractThread* m_pExtractThread;
  // pause/resume flag
  bool m_bPaused;
  bool m_bNeedAbort;
  // previous progress-bar foreground color
  COLORREF m_clrPrevPBColor;
  // update dlg timer
  UINT_PTR m_hTimerPrg;
  // Total extraction progress bar
  CProgressCtrl m_pbAdvance;
  // Extraction advance text status
  CStatic m_txtStatus;
  // pause Extract button
  CButton m_btnPause;
  // cancel Extract button
  CButton m_btnCancel;
  // at the end of extract has information
  // about type of the rised error in main threads
  CString m_sErrorSummary;

public:
  CKeyList* m_pKeyList;
  CDoc* m_pDoc;
  ULARGE_INTEGER m_uliTotalMediaSize;

private:
  void Start();
  void Stop();
  void DestroyThread();
  void AbortThread();

public:
	CMediaExtractDlg(CWnd* pParent = NULL);
	virtual ~CMediaExtractDlg();

	enum { IDD = IDD_MEDIA_EXTRACT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
  virtual void OnCancel();
  virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedBtnPause();
  afx_msg void OnTimer(UINT nIDEvent);
};
