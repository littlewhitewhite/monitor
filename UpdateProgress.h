#pragma once
#include "afxcmn.h"

#define  TIMER_CLOSE 300
// CUpdateProgress �Ի���

class CUpdateProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CUpdateProgress)

public:
	CUpdateProgress(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUpdateProgress();

// �Ի�������
	enum { IDD = IDD_DIALOG_UPDATEPROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ProCtrlUpdate;
	HANDLE m_hThreadUpdataData;
	virtual BOOL OnInitDialog();
	static DWORD WINAPI ThreadUpdateProgress(LPVOID lpParame);
	void QuestClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
