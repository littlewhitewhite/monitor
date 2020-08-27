// SetKbaseInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "SetKbaseInfoDlg.h"
#include "afxdialogex.h"


// CSetKbaseInfoDlg dialog

IMPLEMENT_DYNAMIC(CSetKbaseInfoDlg, CDialogEx)

CSetKbaseInfoDlg::CSetKbaseInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetKbaseInfoDlg::IDD, pParent)
{

}

CSetKbaseInfoDlg::~CSetKbaseInfoDlg()
{
}

void CSetKbaseInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetKbaseInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetKbaseInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetKbaseInfoDlg message handlers


BOOL CSetKbaseInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	SetBackgroundColor(RGB(15, 115, 188));
	GetDlgItem(IDC_IPADDRESS_SETKBASE)->SetWindowText(m_strKbaseId);

	GetDlgItem(IDC_EDIT_KBASESET_PORT)->SetWindowText(m_strKbasePort);

	GetDlgItem(IDC_EDIT_KBASESET_MANIGERID)->SetWindowText(m_strKbaseUserName);


	GetDlgItem(IDC_EDIT_KBASESET_PASSSWORD)->SetWindowText(m_strKbasePassWord);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSetKbaseInfoDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_IPADDRESS_SETKBASE)->GetWindowText(m_strKbaseId);

	GetDlgItem(IDC_EDIT_KBASESET_PORT)->GetWindowText(m_strKbasePort);

	GetDlgItem(IDC_EDIT_KBASESET_MANIGERID)->GetWindowText(m_strKbaseUserName);


	GetDlgItem(IDC_EDIT_KBASESET_PASSSWORD)->GetWindowText(m_strKbasePassWord);
	CDialogEx::OnOK();
}
