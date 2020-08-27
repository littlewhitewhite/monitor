#pragma once


// CRegistInfoDlg dialog

class CRegistInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegistInfoDlg)

public:
	CRegistInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRegistInfoDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_REGESTINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CFont m_fdate;
	HBRUSH m_hbrushBlue;
	CString m_strRegistNo;


public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void SetRegistNo(CString strRegistNo);
	afx_msg void OnBnClickedButtonRegest();
};
