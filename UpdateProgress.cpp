// UpdateProgress.cpp : 实现文件
//

#include "stdafx.h"
#include "Monitor.h"
#include "UpdateProgress.h"
#include "afxdialogex.h"


// CUpdateProgress 对话框

IMPLEMENT_DYNAMIC(CUpdateProgress, CDialogEx)

CUpdateProgress::CUpdateProgress(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUpdateProgress::IDD, pParent)
{
	m_hThreadUpdataData=NULL;
}

CUpdateProgress::~CUpdateProgress()
{
}

void CUpdateProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_UPDATE, m_ProCtrlUpdate);
}


BEGIN_MESSAGE_MAP(CUpdateProgress, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CUpdateProgress 消息处理程序


BOOL CUpdateProgress::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_ProCtrlUpdate.SetRange(0,100);
	GetDlgItem(IDC_STATIC_FINISHPERCENT)->SetWindowTextW(_T("0%"));

	m_hThreadUpdataData=CreateThread(NULL,0,CUpdateProgress::ThreadUpdateProgress,this,0,NULL);
	SetProcessAffinityMask(m_hThreadUpdataData,0x0000000E);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

DWORD WINAPI CUpdateProgress::ThreadUpdateProgress(LPVOID lpParame)
{
	CUpdateProgress* pdlg=(CUpdateProgress*)lpParame;
	CString strPercent;
	while (theApp.m_iUpdateFinishPercent>=0&&theApp.m_iUpdateFinishPercent<100)
	{
		pdlg->m_ProCtrlUpdate.SetPos(theApp.m_iUpdateFinishPercent);
		
		strPercent.Format(_T("%d"),theApp.m_iUpdateFinishPercent);
		pdlg->GetDlgItem(IDC_STATIC_FINISHPERCENT)->SetWindowTextW(strPercent+_T("%"));
		Sleep(1000);
	}
	
	if (theApp.m_iUpdateFinishPercent==100)
	{
		pdlg->m_ProCtrlUpdate.SetPos(100);
		pdlg->GetDlgItem(IDC_STATIC_FINISHPERCENT)->SetWindowTextW(_T("100%"));
	}
	pdlg->QuestClose();
	return 1;
}

void CUpdateProgress::QuestClose()
{
	SetTimer(TIMER_CLOSE,1000,NULL);
}

void CUpdateProgress::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent==TIMER_CLOSE)
	{
		KillTimer(TIMER_CLOSE);
		WaitForSingleObject(m_hThreadUpdataData,INFINITE);
		OnOK();
	}
	CDialogEx::OnTimer(nIDEvent);
}
