// RegistInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "RegistInfoDlg.h"
#include "afxdialogex.h"


// CRegistInfoDlg dialog

IMPLEMENT_DYNAMIC(CRegistInfoDlg, CDialogEx)

CRegistInfoDlg::CRegistInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRegistInfoDlg::IDD, pParent)
{

}

CRegistInfoDlg::~CRegistInfoDlg()
{
}

void CRegistInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegistInfoDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_REGEST, &CRegistInfoDlg::OnBnClickedButtonRegest)
END_MESSAGE_MAP()


// CRegistInfoDlg message handlers


BOOL CRegistInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_fdate.CreateFont(19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("΢���ź�"));

	GetDlgItem(IDC_STATIC_RNO)->SetFont(&m_fdate);
	GetDlgItem(IDC_EDIT_REGISTNO)->SetFont(&m_fdate);
	GetDlgItem(IDC_EDIT_REGISTNO)->SetWindowText(m_strRegistNo);

	GetDlgItem(IDC_EDIT_WARNING)->SetWindowText(_T("��ע������ԣ�"));
	MoveWindow(500, 300, 500, 300);

	SetBackgroundColor(RGB(15, 115, 188));
	m_hbrushBlue = CreateSolidBrush(RGB(15, 115, 188));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CRegistInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	switch (pWnd->GetDlgCtrlID())
	{

	case IDC_STATIC_RNO:

		pDC->SetBkMode(TRANSPARENT);
		//���ñ���Ϊ͸�� 
		pDC->SetTextColor(RGB(255, 255, 255));   //����������ɫ 
		break;

	case IDC_EDIT_REGISTNO:
		pDC->SetBkMode(TRANSPARENT);
		//���ñ���Ϊ͸�� 
		pDC->SetTextColor(RGB(129, 129, 129));   //����������ɫ 
		break;
	case IDC_EDIT_WARNING:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(15, 115, 188));
		//���ñ���Ϊ͸�� 
		pDC->SetTextColor(RGB(255, 0, 0));   //����������ɫ 
		hbr = m_hbrushBlue;
		break;
	default:
		;
	}
	return hbr;
}

void CRegistInfoDlg::SetRegistNo(CString strRegistNo)
{

	m_strRegistNo = strRegistNo;

}

void CRegistInfoDlg::OnBnClickedButtonRegest()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
