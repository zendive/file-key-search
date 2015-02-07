#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CMediaDlg dialog

class CMediaDlg: public CDialog
{
	DECLARE_DYNAMIC(CMediaDlg)
// Dialog Data
	enum { IDD = IDD_MEDIA_DLG };

private:
  const ULARGE_INTEGER m_qwFileSize;
  const CKey* const m_pKey;

public:
  // Media name (file-name)
  CEdit m_txtName;
  // Media size is - from offset till the end of file
  CButton m_chkEnd;
  // Media offset
  CEdit m_txtOffset;
  // Media size
  CEdit m_txtSize;
  // Offset value of `m_txtOffset` controll
  ULONGLONG m_qwOffset;
  // Size value of `m_txtSize` controll
  ULONGLONG m_qwSize;
  // Name value of `m_txtName` controll
  CString m_sName;

	CMediaDlg(CWnd* pParentconst
    , ULARGE_INTEGER _qwFileSize
    , const CKey* const _pKey);
	virtual ~CMediaDlg();
  // Validate internall state  according to the name-offset-size rules
  // return true if valid
  bool Validate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnEnChangeTxtName();
  afx_msg void OnBnClickedCheckEnd();
  afx_msg void OnEnChangeTxtOffset();
};
