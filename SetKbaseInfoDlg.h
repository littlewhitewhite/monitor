#pragma once


// CSetKbaseInfoDlg dialog

class CSetKbaseInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetKbaseInfoDlg)

public:
	CSetKbaseInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetKbaseInfoDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_KBASESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CString m_strKbaseId;
	CString m_strKbaseUserName;
	CString m_strKbasePort;
	CString m_strKbasePassWord;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
