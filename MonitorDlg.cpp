
// MonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "MonitorDlg.h"

#include <time.h>
#include <Windows.h>

#include <afxinet.h>  
#include <math.h>
/*#include "Markup.h"*/
//#include "unzip.h"
#include <stdio.h>    
#include <afxinet.h>  
#include "UpdateProgress.h"
#include "unzip.h"
#include <mmsystem.h>
#include <shellapi.h>
#include <fstream>
#pragma   comment(lib,"winmm.lib")
#pragma comment(lib,"iphlpapi.lib")
#include "ZLibWrap.h"


#include"HMAC_SHA1.h"
#include <fstream>
#define  OUTSIZE_N 1000
#define  BUFFER_SIZE  5*1024

#pragma comment(lib,"../lib/ZLibWrap.lib") 



#include "ZipImplement.h"
#define IS_NUMBER(c)        ((c) && (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') ||((c) >= 'A' && (c) <= 'F')))
#define ROWS 10			// 默认一个page10条信息

#pragma comment(linker, "/STACK:10000000 ")



struct  PSENDTOTHREED
{
	HWND hWndDlg;

	UINT nThreadNo;

};

CCriticalSection SectionChangeDownLoadNo;


WINMMAPI MCIERROR  WINAPI mciSendStringA(__in LPCSTR lpstrCommand, __out_ecount_opt(uReturnLength) LPSTR lpstrReturnString,
	__in UINT uReturnLength, __in_opt HWND hwndCallback);


void ClearDirectory(CString szPath)    //参数示范  "C:\\bb"  C盘下的bb目录
{
	if (!PathFileExists(szPath))
	{
		return;
	}
	CFileFind m_ff;
	CString path = szPath;

	if (path.Right(1) != _T("\\"))
		path += _T("\\");

	path += _T("*.*");
	BOOL res = m_ff.FindFile(path);
	CString stTempFileName;

	while (res)
	{
		res = m_ff.FindNextFile();
		//是文件直接删除
		if (!m_ff.IsDirectory() && !m_ff.IsDots())
		{
			stTempFileName = m_ff.GetFilePath();

			DeleteFile(stTempFileName);

		}
		else if (m_ff.IsDots())
			continue;
		else if (m_ff.IsDirectory())
		{
			path = m_ff.GetFilePath();
			//是目录时继续递归，删除该目录下的文件
			ClearDirectory(path);
			//目录为空后删除目录
			RemoveDirectory(path);
		}
	}
	//最终目录被情空，删除该目录
	RemoveDirectory(szPath);
	m_ff.Close();
}


void CString2Char(CString str, char ch[])
{
	long i = 0;
	char *tmpch;

#ifndef UNICODE
	tmpch = str.GetBuffer(str.GetLength());

	for (i = 0; tmpch[i] != '\0'; i++) ch[i] = tmpch[i];
	ch[i] = '\0';

	str.ReleaseBuffer();
#else
	for (long lCount = 0; lCount < ceil(str.GetLength()*1.0 / 10000); lCount++)
	{
		CString stTemp = str.Mid(lCount * 10000, 10000);
		int iLen = WideCharToMultiByte(CP_ACP, 0, stTemp, -1, NULL, 0, NULL, NULL);//得到Char的长度
		tmpch = new char[iLen + 1];                                             //分配变量的地址大小
		WideCharToMultiByte(CP_ACP, 0, stTemp, -1, tmpch, iLen, NULL, NULL);       //将CString转换成char*

		for (long j = 0; tmpch[j] != '\0'; j++, i++) ch[i] = tmpch[j];

		delete[] tmpch;
		tmpch = NULL;

	}
	ch[i] = '\0';
#endif
}


void WriteLog(CString strWrite)
{
	CString strLogPath = theApp.m_strCanChangePath + _T("\\TempUpdate\\Monitorlog.txt");
	CFile file;

	BOOL bCheck = file.Open(strLogPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	if (bCheck)
	{
		CString strTemp;
		char pTemp[300];
		file.SeekToEnd();

		strTemp.Format(_T("%s:\r\n"), strWrite);
		CString2Char(strTemp, pTemp);
		file.Write(pTemp, strlen(pTemp));

		file.Close();

	}
}

void WriteLog_shfw(CString strWrite)
{
	CString strLogPath = theApp.m_strCanChangePath + _T("\\TempUpdate\\Monitorlogshfw.txt");
	CFile file;

	BOOL bCheck = file.Open(strLogPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	if (bCheck)
	{
		CString strTemp;
		char* pTemp = new char[strWrite.GetLength() * 2];
		file.SeekToEnd();

		strTemp.Format(_T("%s:\r\n"), strWrite);
		CString2Char(strTemp, pTemp);
		file.Write(pTemp, strlen(pTemp));

		file.Close();
		delete pTemp;
		pTemp = NULL;

	}
}



void WriteLogTH0(CString strWrite, int number)
{
	GetLocalTime(&st);
	logInfo.Format(_T("%s : at %.2d:%.2d:%.2d:%.3d. Line %d."), 
					strWrite, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, number);
	CString strLogPath = theApp.m_strCanChangePath + _T("\\TempUpdate\\MonitorlogTH0.txt");
	CFile file;

	BOOL bCheck = file.Open(strLogPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	if (bCheck)
	{
		CString strTemp;
		char pTemp[300];
		file.SeekToEnd();

		strTemp.Format(_T("%s:\r\n"), logInfo);
		CString2Char(strTemp, pTemp);
		file.Write(pTemp, strlen(pTemp));

		file.Close();

	}
}

void WriteLogTH1(CString strWrite, int number)
{

	GetLocalTime(&st);
	logInfo.Format(_T("%s : at %.2d:%.2d:%.2d:%.3d. Line %d."), 
					strWrite, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, number);
	CString strLogPath = theApp.m_strCanChangePath + _T("\\TempUpdate\\MonitorlogTH1.txt");
	CFile file;

	BOOL bCheck = file.Open(strLogPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	if (bCheck)
	{
		CString strTemp;
		char pTemp[300];
		file.SeekToEnd();

		strTemp.Format(_T("%s:\r\n"), logInfo);
		CString2Char(strTemp, pTemp);
		file.Write(pTemp, strlen(pTemp));

		file.Close();
	}
}

void WriteLogTH2(CString strWrite, int number)
{

	GetLocalTime(&st);
	logInfo.Format(_T("%s : at %.2d:%.2d:%.2d:%.3d. Line %d."), 
					strWrite, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, number);
	CString strLogPath = theApp.m_strCanChangePath + _T("\\TempUpdate\\QK.txt");
	CFile file;

	BOOL bCheck = file.Open(strLogPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	if (bCheck)
	{
		CString strTemp;
		char pTemp[300];
		file.SeekToEnd();

		strTemp.Format(_T("%s:\r\n"), logInfo);
		CString2Char(strTemp, pTemp);
		file.Write(pTemp, strlen(pTemp));

		file.Close();

	}
}

/*#import "msxml.dll" raw_interfaces_only*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMonitorDlg dialog




CMonitorDlg::CMonitorDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nLLoadPart = 0;

	m_bGetUpdateFromServer = FALSE;

	m_hThreadGetUpdataInfoData = NULL;
	m_hThreadUpdateData = NULL;
	m_hGetAppUpdateInfoData = NULL;
	m_hClearInvalidateData = NULL;
	m_hTheradGetNewData = NULL;
	m_hThreadGetUpdataInfoData_PersonSet = NULL;
	m_hThreadGetUpdataInfoData_ShfwSet = NULL;

}

void CMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMonitorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMonitorDlg message handlers

TCHAR* CString2TCHAR(CString &str)
{
	int iLen = str.GetLength();
	TCHAR* szRs = new TCHAR[iLen];
	lstrcpy(szRs, str.GetBuffer(iLen));
	str.ReleaseBuffer();
	return szRs;
}

char* CString2char(CString &str)
{
	int len = str.GetLength();
	char* chRtn = (char*)malloc((len * 2 + 1)*sizeof(char));//CString的长度中汉字算一个长度 
	memset(chRtn, 0, 2 * len + 1);
	USES_CONVERSION;
	strcpy((LPSTR)chRtn, OLE2A(str.LockBuffer()));
	return chRtn;
}

//BOOL SetSaveWeather(CString strNowWeather)
//{
//	CString strIniFileName = _T("\\xml\\weather.xml");
//
//	strIniFileName = theApp.m_strCanChangePath + strIniFileName;
//
//	if (!PathFileExists(strIniFileName))
//	{
//		AfxMessageBox(_T("配置文件不存在！"));
//		return FALSE;
//	}
//
//	//从xml中读取配置信息
//	::CoInitialize(NULL);
//	BSTR strNodeValue;
//	MSXML2::IXMLDOMDocumentPtr spDoc;
//	HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));    //创建文档对象 
//
//	BSTR bstrIniFileName = strIniFileName.AllocSysString();
//	SysFreeString(bstrIniFileName);
//	hr = spDoc->load(bstrIniFileName);//load xml文件  
//
//	MSXML2::IXMLDOMNodeListPtr spNodeList;
//	MSXML2::IXMLDOMNodePtr pItem;
//
//	spNodeList = spDoc->getElementsByTagName(_bstr_t("a"));   //获取根结点  
//	long nListNo = 0;
//	spNodeList->get_length(&nListNo);
//	if (nListNo <= 0)
//	{
//		spNodeList.Release();
//		spDoc.Release();
//		::CoUninitialize();
//		return FALSE;
//	}
//
//	spNodeList->get_item(0, &pItem);
//	strNodeValue = strNowWeather.AllocSysString();
//	SysFreeString(strNodeValue);
//	pItem->put_text(strNodeValue);
//
//	pItem.Release();
//
//	spNodeList.Release();
//	spDoc->save((_variant_t)strIniFileName);
//	spDoc.Release();
//	::CoUninitialize();
//
//	return TRUE;
//}


BOOL CMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//TestDb();

	MoveWindow(CRect(0, 0, 0, 0));
	ShowWindow(SW_HIDE);

	//从任务栏中去掉.
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	wp.flags = WPF_RESTORETOMAXIMIZED;
	wp.showCmd = SW_HIDE;
	SetWindowPlacement(&wp);

	TCHAR *pAppName = theApp.m_strAppDir.GetBuffer(MAX_PATH);
	GetModuleFileName(NULL, pAppName, MAX_PATH);
	// 获得上一层目录
	PathRemoveFileSpec(pAppName);
	theApp.m_strAppDir.ReleaseBuffer();


	//如果是双核cpu 那就用第二个
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	DWORD dwMask = 0x0000;
	DWORD dwtmp = 0x0001;
	if (SystemInfo.dwNumberOfProcessors > 1)
	{
		dwtmp *= 2;
	}
	//HideTaskBar(TRUE);


	//更新程序
	//CString strUpdateAppPath;
	//if (IsNeedUpdateApplication(strUpdateAppPath))
	//{
	//	//AfxMessageBox(_T("更新程序！"));
	//	UpdateApp(strUpdateAppPath);
	//	return TRUE;
	//}

	//清空无用数据（先数据后表）

	ClearInvalidedData();

	SetWindowPos(&wndTop, 0, 0, 0, 0, NULL);
	OpenCajTouchViewPro();
	SetTimer(TIME_MONITER, 5000, NULL);
	SetTimer(CLOS__WERFAULT_TIMER, 5 * 1000, NULL);
	SetTimer(CLOS_WINDOW_TIMER, 1000, NULL);

	//按照次序启动更新线程
	// 1.用户控制类

	// 2.期刊类

	// 3.个性化定制类


	//启动下载报纸程序（前面要判断是否需要启动）


	//检查最新的用户设置并下载

	/***********************************************检测最新程序并下载 ***********************************************************/
	m_hGetAppUpdateInfoData = CreateThread(NULL, 0, CMonitorDlg::ThreadApplicationUpdatePackage, this, 0, NULL);
	//SetProcessAffinityMask(m_hGetAppUpdateInfoData, dwtmp);
	/*************************************************************************************************************************/

	////从数据库里面查询更新方式


	//CString strSql = _T("select UPDATEKINDID,DOWNLOADURL,REQUESTKIND,UPDATEKIND,LASTUPDATESTANDER,THREADNO FROM UPDATEINFO ORDER BY THREADNO,ORDERNO");

	//while (!theApp.LockRead(INFINITE, 10));

	//CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
	//if (stmt != NULL && !stmt->IsNullStmt())
	//{
	//	int iPreThreadNo = -1;
	//	while (stmt->NextRow())
	//	{
	//		if (updateThreadArray.GetSize() == 0 || iPreThreadNo != _ttoi(CString(stmt->ValueString(3))))
	//		{
	//			UPDATETHREADINFO updatethreadinfo;
	//			updatethreadinfo.nStartNo = updateinfoArray.GetSize();
	//			updatethreadinfo.nEndNo = updatethreadinfo.nStartNo + 1;
	//			updatethreadinfo.nRequestTimeGap = 5;//默认间隔五分钟
	//			updateThreadArray.Add(updatethreadinfo);

	//			iPreThreadNo = _ttoi(CString(stmt->ValueString(5)));
	//		}
	//		else
	//		{
	//			updateThreadArray.GetAt(updateThreadArray.GetSize() - 1).nEndNo += 1;
	//		}

	//		UPDATEINFOITEM updateItem;
	//		updateItem.strKindId = stmt->ValueString(0);
	//		updateItem.strDownloadUrl = stmt->ValueString(1);
	//		updateItem.nRequestKind = _ttoi(CString(stmt->ValueString(2)));
	//		updateItem.nUpdateKind = _ttoi(CString(stmt->ValueString(3)));
	//		updateItem.strLastUpdateStander = stmt->ValueString(4);
	//	
	//		updateinfoArray.Add(updateItem);

	//	}

	//	delete stmt;
	//	stmt = NULL;
	//}
	//else
	//{
	//	theApp.UnLockRead();
	//	AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
	//	return FALSE;
	//}
	//theApp.UnLockRead();

	m_hThreadLoad[0] = CreateThread(NULL, 0, ThreadGetUpdateInfoAndData_XWDT, LPVOID(&theApp.m_updateThreadArray.GetAt(0)), 0, NULL);
	m_hThreadLoad[1] = CreateThread(NULL, 0, ThreadGetUpdateInfoAndData_Others, LPVOID(&theApp.m_updateThreadArray.GetAt(1)), 0, NULL);
	m_hThreadLoad[2] = CreateThread(NULL, 0, ThreadGetUpdateInfoAndData_QK_TS, LPVOID(&theApp.m_updateThreadArray.GetAt(2)), 0, NULL);

	//开启下载报纸程序  
	//OpenLoadPaperPro();


	////更新数据
	//if (IsNeedUpdateData_OneDayDowload())
	//{
	//	if (!DoUpdateData())
	//	{
	//		AfxMessageBox(_T("更新失败！"));
	//		//SendMessage(WM_CLOSE);
	//		//return FALSE;
	//	}
	//}

	////检测最新数据并下载
	//m_hThreadGetUpdataInfoData = CreateThread(NULL, 0, CMonitorDlg::ThreadGetUpdateInfoAndDataEx, this, 0, NULL);
	//SetProcessAffinityMask(m_hThreadGetUpdataInfoData, dwtmp);


	////检测最新数据并下载--ZJZY
	//m_hThreadGetUpdataInfoData_ZJZY = CreateThread(NULL, 0, CMonitorDlg::ThreadGetUpdateInfoAndDataEx_ZJZY, this, 0, NULL);
	//SetProcessAffinityMask(m_hThreadGetUpdataInfoData_ZJZY, dwtmp);



	//m_hThreadGetUpdataInfoData_PersonSet = CreateThread(NULL, 0, CMonitorDlg::ThreadGetUpdateInfoAndDataEx_Personset, this, 0, NULL);
	//SetProcessAffinityMask(m_hThreadGetUpdataInfoData_PersonSet, dwtmp);

	//m_hThreadGetUpdataInfoData_ShfwSet = CreateThread(NULL, 0, CMonitorDlg::ThreadGetUpdateInfoAndDataEx_ShfwSet, this, 0, NULL);
	//SetProcessAffinityMask(m_hThreadGetUpdataInfoData_ShfwSet, dwtmp);



	//清楚无用数据
	//	GetPaperCnameArray();
	//	m_hClearInvalidateData = CreateThread(NULL, 0, CMonitorDlg::ThreadClearInvalidateData, this, 0, NULL);
	//	SetProcessAffinityMask(m_hClearInvalidateData, dwtmp);

	//得到最新动态更新的新闻
	//m_hTheradGetNewData = CreateThread(NULL, 0, CMonitorDlg::TheradGetNewData, this, 0, NULL);
	//SetProcessAffinityMask(m_hTheradGetNewData, dwtmp);


	//	//m_strGUIDOLDUnInstal=_T("{43761EBC-B641-4231-9E20-4FFE3A4E401D}");//无二维码 和原来的有二维码
	//	//m_strGUIDOLDUnInstal=_T("{9F586C52-34FA-45D5-B656-A35870F2A827}");//有二维码
	//	//m_strGUIDOLDUnInstal = _T("{04039A6E-6A40-46B7-A504-A50515F8D6E1}");
	//	m_strGUIDOLDUnInstal  =   _T("{DD00955E-920A-4223-8F16-5B06CC6F890F}"); //日更新
	//	
	//	//UpdateFirstLevlTable();
	//	//资源路径
	//	TCHAR szDictPath[MAX_PATH];
	//	SHGetFolderPath(NULL,CSIDL_COMMON_APPDATA,NULL, 0,szDictPath);
	//	PathAppend(szDictPath, _T("\\TTKN\\CAJTouchViewerre"));
	//	m_strDictPath = szDictPath;
	//
	//	//从任务栏中去掉.
	//	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);
	//	WINDOWPLACEMENT wp;
	//	wp.length=sizeof(WINDOWPLACEMENT);
	//	wp.flags=WPF_RESTORETOMAXIMIZED;
	//	wp.showCmd=SW_HIDE;
	//	SetWindowPlacement(&wp);
	//	//SetTimer(CLOS_WINDOW_TIMER,1000,NULL);
	//
	//	TCHAR *pAppName = theApp.m_strAppDir.GetBuffer(MAX_PATH);
	//	GetModuleFileName(NULL, pAppName, MAX_PATH);
	//	// 获得上一层目录
	//	PathRemoveFileSpec(pAppName);
	//	theApp.m_strAppDir.ReleaseBuffer();
	//
	//	if (!PathFileExists(theApp.m_strCanChangePath+_T("\\TempUpdate")))
	//	{
	//		CreateDirectory(theApp.m_strCanChangePath+_T("\\TempUpdate"),NULL);
	//	}
	//
	//	CString strMonitorlog=theApp.m_strCanChangePath+_T("\\TempUpdate\\Monitorlog.txt");
	//
	//	if (PathFileExists(strMonitorlog))
	//	{
	//		DeleteFile(strMonitorlog);
	//	}
	//
	//
	//
	//	SYSTEM_INFO SystemInfo;
	//	GetSystemInfo(&SystemInfo);
	//	DWORD dwMask = 0x0000;
	//	DWORD dwtmp = 0x0001;
	//	if (SystemInfo.dwNumberOfProcessors > 1)
	//	{
	//		dwtmp *= 2;
	//	}
	//
	//
	//
	//	ReadVersion();
	//	ReadServerId();
	//	while (!IsOpen()&&!Open(_T("DBOWN"),_T(""),m_sKbasId, 4567))
	//	{
	//		Sleep(1000);
	//	}
	//
	//	if (!OpenEx(theApp.m_nConnectNo,_T("DBOWN"),_T(""),_T("127.0.0.1"),_T(""), 4567))
	//	{
	//		AfxMessageBox(_T("连接数据库失败"));
	//	}
	//
	//	//m_hMutexOperatorKbas=CreateMutex(NULL,FALSE,_T("OperatorKbas"));
	//	HideTaskBar(TRUE);
	//	ReadSerialNo();
	//	ReadCnkiServerId();
	//	GetMacArray();
	//	if (IsRegest() == FALSE)
	//	{
	//
	//		while (!GetSerialNoFromServer())
	//		{
	//			CRegistInfoDlg   dlgReg;
	//			dlgReg.SetRegistNo(m_strRegestNo);
	//			/*m_pMainWnd = &dlgReg;*/
	//			dlgReg.DoModal();
	//		}
	//		CString strIniPath = _T("\\xml\\serialno.ini");
	//
	//		strIniPath = m_strDictPath + strIniPath;
	//
	//		::WritePrivateProfileStringW(_T("SerialNo"), _T("No"), m_strSerialNo, strIniPath);
	//
	//	}
	//	GetBuyYear();
	//
	//	GetPaperCnameArray();
	//	ReadDataPathAndStaleDate();
	//	ReadStartDate();
	//	GetFmPath(_T("ZZHC"));
	//
	//
	//	//// 获得最新的天气信息
	//	//m_hGetWeather = CreateThread(NULL, 0, CMonitorDlg::ThreadGetWeather, this, 0, NULL);
	//	//SetProcessAffinityMask(m_hGetWeather, dwtmp);
	//	///*	Sleep(1000*20);*/
	//
	//
	//	//更新程序
	//	CString strUpdateAppPath;
	//	if (IsNeedUpdateApplication(strUpdateAppPath))
	//	{
	//		UpdateApp(strUpdateAppPath);
	//		return TRUE;
	//	}
	//	//更新数据
	//	//if (IsNeedUpdateData())
	//	//{
	//	//	m_hThreadUpdateData=CreateThread(NULL,0,CMonitorDlg::ThreadUpdateZZHC,this,0,NULL);
	//	//	SetProcessAffinityMask(m_hThreadUpdateData, dwtmp);
	//	//	CUpdateProgress updatepro;
	//	//	updatepro.DoModal();
	//	//}
	//
	//
	//	if (IsNeedUpdateData_OneDayDowload())
	//	{
	//		if (!DoUpdateData())
	//		{
	//			AfxMessageBox(_T("更新失败！"));
	//			SendMessage(WM_CLOSE);
	//			return FALSE;
	//		}
	//	}
	//
	//	//检测最新数据并下载
	//	m_hThreadGetUpdataInfoData = CreateThread(NULL, 0, CMonitorDlg::ThreadGetUpdateInfoAndDataEx, this, 0, NULL);
	//	SetProcessAffinityMask(m_hThreadGetUpdataInfoData, dwtmp);
	//
	//	//检测最新数据并下载--ZJZY
	//	m_hThreadGetUpdataInfoData_ZJZY = CreateThread(NULL, 0, CMonitorDlg::ThreadGetUpdateInfoAndDataEx_ZJZY, this, 0, NULL);
	//	SetProcessAffinityMask(m_hThreadGetUpdataInfoData_ZJZY, dwtmp);
	//
	//	//检测最新程序并下载
	//	m_hGetAppUpdateInfoData=CreateThread(NULL,0,CMonitorDlg::ThreadApplicationUpdatePackage,this,0,NULL);
	//	SetProcessAffinityMask(m_hGetAppUpdateInfoData, dwtmp);
	//
	//	//清楚无用数据
	//	m_hClearInvalidateData=CreateThread(NULL,0,CMonitorDlg::ThreadClearInvalidateData,this,0,NULL);
	//	SetProcessAffinityMask(m_hClearInvalidateData, dwtmp);
	//
	//	//得到最新动态更新的新闻
	//	//m_hTheradGetNewData = CreateThread(NULL, 0, CMonitorDlg::TheradGetNewData, this, 0, NULL);
	//	//SetProcessAffinityMask(m_hTheradGetNewData, dwtmp);
	//
	//
	//	//开启下载报纸程序  
	//	OpenLoadPaperPro();

	//	if (theApp.m_nGetInfoVersion==0)
	//	{
	//		SetTimer(DOCLEAR_INVALIDE_TABLEDATA,30*60*1000,NULL);
	//	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CMonitorDlg::TestDb()
{
	//UINT nConnect=1;
	//if(!IsOpenEx(nConnect))
	//{
	//	AfxMessageBox(_T(""));
	//}
	//if (!OpenEx(nConnect,_T("DBOWN"),_T(""),_T("127.0.0.1"),_T(""), 4567))
	//{
	//	AfxMessageBox(_T(""));
	//}


	//CString strSql;

	//strSql=_T("INSERT INTO UPDATEDATE (PYNAME,C_NAME,最新更新日期) VALUES(anja,安家,201403) ");
	//BSTR  bstrSql=strSql.AllocSysString();
	//SysFreeString(bstrSql);
	//ExecSQL(theApp.m_nConnectNo,bstrSql);

	//strSql=_T("SELECT * FROM UPDATEDATE ");

	//bstrSql=strSql.AllocSysString();
	//SysFreeString(bstrSql);
	//LPVOID pResult=NULL;
	//UINT nResult=0;
	//if( !OpenRecordSetEx(nConnect,nResult,bstrSql) )
	//{
	//	AfxMessageBox(_T( "OpenRecordSet Error\n" ));
	//	return ;     
	//}
	//UINT nNoCount=GetRecordCountEx(nConnect,nResult);
	//if (nNoCount<0)
	//{
	//	CloseRecordSetEx(nConnect,nResult);
	//	return ; 
	//}
	//theApp.m_strSerialNo = CString(GetFieldValueEx(nConnect, nResult, 2));
	//CloseRecordSetEx(nConnect,nResult);


	//OnCancel();
}

BOOL CMonitorDlg::ClearInvalidedData()
{
	//CString strSql = _T("select nkind, sdetail FROM NEEDDELETE  order by nkind desc");//先执行删除表信息操作
	struct StructDeleteItemInfo
	{
		UINT nKnd;//1位路径  2位表
		CString strDetailInfo;//详细信息
	};
	CArray<StructDeleteItemInfo, StructDeleteItemInfo> structDelInfoArray;

	//  如果存在CHANGEDINFO表，就启动程序删除数据
	CSqlStatement *stmt;
	CString strSqlDelete;
	if (tableExist(_T("CHANGEDINFO")))
	{
		Sleep(1000);
		while(!theApp.LockWrite(-1, -1));
W
		theApp.m_sqlite.Begin();

		strSqlDelete = _T("DELETE FROM CHANGEDINFO;");
		stmt = theApp.m_sqlite.Statement(strSqlDelete);
		stmt->Execute();
		delete stmt;
		stmt = NULL;

		theApp.m_sqlite.Commit();
		theApp.UnLockWrite();
EW
		Sleep(1000);
	}
	// 删除那些BeValid为0的项，即虚假删除的项 
	CArray<CString, CString> tableArray;
	tableArray.Add(theApp.m_updateinfoArray[8].tableInfo);	// SECONDLEVEL
	tableArray.Add(theApp.m_updateinfoArray[2].tableInfo);	// THIRDLEVEL
	BOOL itemExists = FALSE;

	Sleep(1000);
	while(!theApp.LockWrite(-1, -1));
W
	theApp.m_sqlite.Begin();

	for (int i = 0; i < tableArray.GetSize(); i++) {
		strSqlDelete.Format(_T("DELETE FROM %s WHERE BeValid = 0;"), tableArray.GetAt(i));

		stmt = theApp.m_sqlite.Statement(strSqlDelete);
		stmt->Execute();
		delete stmt;
		stmt = NULL;
	}

	// 删除USERLEAVEMSG中valid为0（即已经上传的数据）
	strSqlDelete.Format(_T("DELETE FROM %s WHERE Valid = 0;"), theApp.m_updateinfoArray[6].tableInfo);	// USERLEAVEMSG
	stmt = theApp.m_sqlite.Statement(strSqlDelete);
	stmt->Execute();
	delete stmt;
	stmt = NULL;

	theApp.m_sqlite.Commit();
	theApp.UnLockWrite();
EW
	Sleep(1000);

	if (structDelInfoArray.GetSize()>0)
	{
		while(!theApp.LockWrite(-1, -1));
W
		theApp.m_sqlite.Begin();
		int iNo = 0;
		CString strTemp;
		CString strSql;
		while (structDelInfoArray.GetAt(iNo).nKnd == 2)
		{
			strTemp = structDelInfoArray.GetAt(iNo).strDetailInfo;
			if (strTemp.Find(_T("+"))>0)
			{
				strSql.Format(_T("delete from \'%s\' where ID=\'%s\'"), strTemp.Left(strTemp.Find(_T("+"))), strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("+")) - 1));
				theApp.m_sqlite.DirectStatement(strSql);
			}
			iNo++;
		}

		strSql = _T("delete FROM NEEDDELETE ");//清空表
		theApp.m_sqlite.DirectStatement(strSql);

		theApp.m_sqlite.Commit();
		theApp.UnLockWrite();
EW
		Sleep(1000);

		for (size_t i = iNo; i < structDelInfoArray.GetSize(); i++)
		{
			strTemp = structDelInfoArray.GetAt(iNo).strDetailInfo;
			if (PathFileExists(strTemp))
			{
				DeleteFile(strTemp);
			}
		}
	}
	Sleep(5);

	return TRUE;
}

void CMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CMonitorDlg::IsExist(CString strId)
{
	return TRUE;
}


BOOL GetStrFromWeb(CString strUrl, CString& strReturn)
{
	DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS;
	CInternetSession *sess = new CInternetSession(NULL, dwAccessType);
	sess->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 800 * 1000);
	sess->SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, 800 * 1000);
	sess->SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, 800 * 1000);

	wstring strHtml = _T("");
	DWORD dwHttpReauwstFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE;
	const TCHAR szHeaders[] = _T("Accept: text/*\r\ncontent-type: application/json;charset=UTF-8\r\nUser-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\nConnection: Keep-Alive\r\n\r\n");

	CHttpConnection *pServer = NULL;
	CHttpFile* pFile = NULL;

	/*   try*/
	CString strServerName;
	CString strObject;
	INTERNET_PORT nPort;
	DWORD dwServiceType;


	try
	{

		if (!AfxParseURL(strUrl, dwServiceType, strServerName, strObject, nPort) ||
			(dwServiceType != AFX_INET_SERVICE_HTTPS  && dwServiceType != AFX_INET_SERVICE_HTTP))
		{
			return FALSE;
		}

		pServer = sess->GetHttpConnection(strServerName, nPort);
		if (pServer == NULL)
		{
			//建立网络连接失败  
			sess->Close(); delete sess;
			return FALSE;
		}
		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, NULL, (DWORD)1, NULL, NULL, dwHttpReauwstFlags);

		if (!pFile)
		{
			//发起GET请求失败   
			delete pServer; pServer = NULL;
			sess->Close(); delete sess;
			return FALSE;
		}
		pFile->AddRequestHeaders(szHeaders);




		BOOL bSendSuc = pFile->SendRequest();
		if (!bSendSuc)
		{
			delete pFile; pFile = NULL;
			delete pServer; pServer = NULL;
			sess->Close(); delete sess;
			return FALSE;
		}

		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);
		if (dwRet == HTTP_STATUS_DENIED)
		{
			AfxMessageBox(L"Access to the secured http site is denied!");
		}

		CString strNewLocation;
		pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS, strNewLocation);

		if (dwRet == HTTP_STATUS_MOVED || dwRet == HTTP_STATUS_REDIRECT || dwRet == HTTP_STATUS_REDIRECT_METHOD)
		{
			CString strNewLocation;

			pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);
			int nPlace = strNewLocation.Find(_T("Location: "));
			if (nPlace == -1)
			{
				delete pFile; pFile = NULL;
				delete pServer; pServer = NULL;
				sess->Close(); delete sess;
				return FALSE;
			}

			//AfxMessageBox(strNewLocation);
			strNewLocation = strNewLocation.Mid(nPlace + 10);
			nPlace = strNewLocation.Find('\n');
			if (nPlace>0)
			{
				strNewLocation = strNewLocation.Left(nPlace);
			}
			pFile->Close();
			delete pFile;
			pServer->Close();
			delete pServer;
		}


		if ((dwServiceType != AFX_INET_SERVICE_HTTPS  && dwServiceType != AFX_INET_SERVICE_HTTP))
		{
			sess->Close();
			return FALSE;
		}

		pServer = sess->GetHttpConnection(strServerName, nPort);

		pFile->Close();
		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, NULL, (DWORD)1, NULL, NULL, dwHttpReauwstFlags);
		pFile->AddRequestHeaders(szHeaders);
		pFile->SendRequest();

		pFile->QueryInfoStatusCode(dwRet);
		if (dwRet != HTTP_STATUS_OK)
		{
			pFile->Close();
			delete pFile; pFile = NULL;
			pServer->Close();
			delete pServer; pServer = NULL;
			sess->Close(); delete sess;
			return FALSE;
		}


		TCHAR sz[BUFFER_SIZE + 1];
		pFile->SetReadBufferSize(BUFFER_SIZE);
		CString strTemp = _T("");
		int pageCode = -1;
		CString strTestKind;
		while (pFile->ReadString(sz, BUFFER_SIZE))
		{
			strTemp += sz;
			int wcslen = ::MultiByteToWideChar(CP_UTF8, NULL, (LPCSTR)sz, -1, NULL, 0);
			TCHAR* wszString = new TCHAR[wcslen + 1];
			::MultiByteToWideChar(CP_UTF8, NULL, (LPCSTR)sz, -1, wszString, wcslen);
			wszString[wcslen] = '\0';
			strHtml.append(wszString);
			strTestKind += wszString;
			delete[] wszString;
			strTestKind.MakeLower();
			if (strTestKind.Find(_T("charset=utf-8")) != -1)
			{
				pageCode = CP_UTF8;
				break;
			}
			else if (strTestKind.Find(_T("charset=gb2312")) != -1)
			{
				pageCode = CP_ACP;
				break;
			}
			else if (strTestKind.Find(_T("charset=gbk")) != -1)
			{
				pageCode = CP_ACP;
				break;
			}

		}

		if (pageCode != -1)
		{
			// 			if (pageCode==CP_UTF8)
			// 			{
			// 				strHtml.erase(strHtml.begin(),strHtml.end());
			// 				TCHAR *szTemp=strTemp.GetBuffer(strTemp.GetLength());
			// 
			// 				int wcslen=::MultiByteToWideChar(pageCode,NULL,(LPCSTR)szTemp,-1,NULL,0);
			// 				TCHAR* wszString=new TCHAR[wcslen+1];
			// 				::MultiByteToWideChar(pageCode,NULL,(LPCSTR)szTemp,-1,wszString,wcslen);
			// 				strTemp.ReleaseBuffer();
			// 				wszString[wcslen]='\0';
			// 				strHtml.append(wszString);
			// 				delete [] wszString;
			// 
			// 			}

			//阅读余下的部分
			while (pFile->ReadString((LPTSTR)sz, BUFFER_SIZE))
			{
				int wcslen = ::MultiByteToWideChar(pageCode, NULL, (LPCSTR)sz, -1, NULL, 0);
				TCHAR* wszString = new TCHAR[wcslen + 1];
				::MultiByteToWideChar(pageCode, NULL, (LPCSTR)sz, -1, wszString, wcslen);
				wszString[wcslen] = '\0';
				strHtml.append(wszString);
				delete[] wszString;


			}

		}
		/*	delete [] sz;*/
		pFile->Close();
		delete pFile; pFile = NULL;
		pServer->Close();
		delete pServer; pServer = NULL;
		sess->Close(); delete sess;
		strReturn = CString(strHtml.c_str());
	}
	catch (...)
	{
		pFile->Close();
		delete pFile; pFile = NULL;
		pServer->Close();
		delete pServer; pServer = NULL;
		sess->Close(); delete sess;
		return FALSE;
	}
	return TRUE;

}

int CompareVerSion(CString strVerSionOld, CString strVerSionNow)
{
	CString strTempO, strTempN;
	int i = 0;
	while (i<3)
	{
		i++;
		int iLength = strVerSionOld.Find(_T("."))>0 ? strVerSionOld.Find(_T(".")) : strVerSionOld.GetLength();
		strTempO = strVerSionOld.Left(iLength);
		strVerSionOld = strVerSionOld.Right(strVerSionOld.GetLength() - iLength - 1);
		iLength = strVerSionNow.Find(_T("."))>0 ? strVerSionNow.Find(_T(".")) : strVerSionNow.GetLength();
		strTempN = strVerSionNow.Left(iLength);
		strVerSionNow = strVerSionNow.Right(strVerSionNow.GetLength() - iLength - 1);
		if (!strTempO.IsEmpty() && !strTempN.IsEmpty())
		{
			if (_ttoi(strTempO)<_ttoi(strTempN))
			{
				return 1;
			}
			else if (_ttoi(strTempO)>_ttoi(strTempN))
			{
				return -1;
			}
		}
		else
		{
			if (!strTempO.IsEmpty() && strTempN.IsEmpty())
			{
				return -1;
			}
			else if (strTempO.IsEmpty() && !strTempN.IsEmpty())
			{
				return 1;
			}
		}


	}

	return 0;
}



CString GetFileMd5(CString strFilePath)
{
	CFile file;
	if (file.Open(strFilePath, CFile::modeRead) == 0)
		return(_T(""));

	MD5_CTX contex;
	CmyMD5 md5;
	md5.MD5Init(&contex);
	int nLength = 0;       //number of bytes read from the file  
	const int nBufferSize = 1024; //checksum the file in blocks of 1024 bytes  
	BYTE Buffer[nBufferSize];   //buffer for data read from the file  

	//checksum the file in blocks of 1024 bytes  
	while ((nLength = file.Read(Buffer, nBufferSize)) > 0)
	{
		md5.MD5UpdateByte(&contex, Buffer, nLength);
	}
	file.Close();
	md5.MD5FinalByte(&contex);
	return CString(md5.MD5TransByte(contex)).MakeLower();

}


BOOL CMonitorDlg::IsNeedUpdateApplication(CString& strUpdateAppPath)
{
	//得到现在版本
	CString strVersionNo;
	CString strKey;
	CRegKey regkey;
	wchar_t pInstallLocation[MAX_PATH];
	::ZeroMemory(pInstallLocation, MAX_PATH);
	DWORD size = MAX_PATH;
	strKey.Format(_T("SOFTWARE\\Microsoft\\WINDOWS\\CURRENTVERSION\\UNINSTALL\\%s"), theApp.m_strGUIDOLDUnInstal);
	LONG lResult = -1;
	BSTR bKey = strKey.AllocSysString();
	lResult = regkey.Open(HKEY_LOCAL_MACHINE, bKey);
	SysFreeString(bKey);

	if (lResult == ERROR_SUCCESS)
	{
		lResult = regkey.QueryValue(pInstallLocation, _T("DisplayVersion"), &size);
		if (lResult == ERROR_SUCCESS)
		{
			strVersionNo = pInstallLocation;
		}

	}
	else
	{
		bKey = strKey.AllocSysString();
		lResult = regkey.Open(HKEY_CURRENT_USER, bKey);
		SysFreeString(bKey);
		if (lResult == ERROR_SUCCESS)
		{
			::ZeroMemory(pInstallLocation, MAX_PATH);
			lResult = regkey.QueryValue(pInstallLocation, _T("DisplayVersion"), &size);
			if (lResult == ERROR_SUCCESS)
			{
				strVersionNo = pInstallLocation;
			}

		}

	}


	//得到更新包 名称

	CString strCanUpdateVersion;
	CString strFilePath = theApp.m_strCanChangePath + _T("\\") + theApp.m_strGUIDOLDUnInstal + _T("\\ini\\AppUpdate.ini");
	if (PathFileExists(strFilePath))
	{
		::GetPrivateProfileString(_T("VERSION"), _T("VERSION"), _T("Error"), strCanUpdateVersion.GetBuffer(MAX_PATH), MAX_PATH, strFilePath);
		strCanUpdateVersion.ReleaseBuffer();
		strCanUpdateVersion = strCanUpdateVersion.Trim();
		if (!strCanUpdateVersion.IsEmpty() && strCanUpdateVersion != _T("Error"))
		{
			CString strFileName = theApp.m_strCanChangePath + _T("\\") + theApp.m_strGUIDOLDUnInstal + _T("\\AppUpdate\\") + strCanUpdateVersion + _T(".exe");
			if (PathFileExists(strFileName) && CompareVerSion(strVersionNo, strCanUpdateVersion)>0)
			{
				strUpdateAppPath = strFileName;
				return TRUE;
			}
		}

	}

	return FALSE;
}

void CMonitorDlg::UpdateApp(CString strUpdateAppPath)
{

	SHELLEXECUTEINFO sei;
	memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = _T("open");
	sei.lpFile = strUpdateAppPath;
	sei.nShow = SW_SHOWDEFAULT;
	ShellExecuteEx(&sei);

	//WaitForSingleObject(sei.hProcess, INFINITE);
	//CloseHandle(sei.hProcess);

}

DWORD WINAPI CMonitorDlg::ThreadApplicationUpdatePackage(LPVOID lpParame)
{
	CMonitorDlg* pDlg = (CMonitorDlg*)lpParame;
	static int  iPreHour = -1;
	int iNowHour = -1;

	CString strVersionNo = _T("");

	//根据 ini文件确定是否需要下载
	//CString strVersionNo;

	CString strFilePath = theApp.m_strCanChangePath + _T("\\") + theApp.m_strGUIDOLDUnInstal;

	if (!PathFileExists(strFilePath))
	{
		if (!CreateDirectory(strFilePath, NULL))
		{
			AfxMessageBox(_T("保存路径创建失败！"));
			return 0;
		}

	}

	strFilePath += _T("\\ini");

	if (!PathFileExists(strFilePath))
	{
		if (!CreateDirectory(strFilePath, NULL))
		{
			AfxMessageBox(_T("保存路径创建失败！"));
			return 0;
		}

	}
	strFilePath += _T("\\AppUpdate.ini");
	if (PathFileExists(strFilePath))
	{
		::GetPrivateProfileString(_T("VERSION"), _T("VERSION"), _T("Error"), strVersionNo.GetBuffer(MAX_PATH), MAX_PATH, strFilePath);
		strVersionNo.ReleaseBuffer();
		strVersionNo = strVersionNo.Trim();
	}
	else
	{
		//根据注册表得到现在版本
		CString strKey;
		CRegKey regkey;
		strKey.Format(_T("SOFTWARE\\Microsoft\\WINDOWS\\CURRENTVERSION\\UNINSTALL\\%s"), theApp.m_strGUIDOLDUnInstal);
		LONG lResult = -1;
		lResult = regkey.Open(HKEY_LOCAL_MACHINE, strKey, KEY_ALL_ACCESS);

		if (lResult == ERROR_SUCCESS)
		{
			wchar_t pInstallLocation[MAX_PATH];
			::ZeroMemory(pInstallLocation, MAX_PATH);
			DWORD size = MAX_PATH;
			lResult = regkey.QueryValue(pInstallLocation, _T("DisplayVersion"), &size);
			if (lResult == ERROR_SUCCESS)
			{
				strVersionNo = pInstallLocation;
			}
			else
			{
				regkey.Close();
				lResult = regkey.Open(HKEY_CURRENT_USER, strKey, KEY_ALL_ACCESS);
				if (lResult == ERROR_SUCCESS)
				{
					::ZeroMemory(pInstallLocation, MAX_PATH);
					lResult = regkey.QueryValue(pInstallLocation, _T("DisplayVersion"), &size);
					if (lResult == ERROR_SUCCESS)
					{
						strVersionNo = pInstallLocation;
					}
				}
			}
		}

		regkey.Close();

		::WritePrivateProfileStringW(_T("VERSION"), _T("VERSION"), strVersionNo, strFilePath);
	}

	if (strVersionNo == _T("Error"))
	{
		AfxMessageBox(_T("程序更新数据包配置文件问题."));
		return FALSE;
	}
	else
	{
		while (!theApp.m_bQuestClose  && !strVersionNo.IsEmpty())
		{
			Sleep(5);
			//得到最新版本信息
			CTime time = CTime::GetCurrentTime();
			iNowHour = time.GetHour();
			if (iPreHour == iNowHour)
			{
				continue;
			}
			else
			{
				iPreHour = iNowHour;
			}

			CString userKind = _T("&userKind=LYWB");
			CString strUrl = theApp.m_strCnkiServerId + _T("app/down/get/version.html?lastversion=") + strVersionNo + userKind;

			//strUrl.AppendFormat(_T("id=%s"),theApp.m_strGUIDOLDUnInstal);
			CString strGet;
			CString strDownUrl;
			CString strSerVerFileMd5;
			CString strTempNowVerSinoNo;
			long lSize = 0;
			//if (theApp.GetStrFromWeb(strUrl,strGet,_T("")))
			//{
			//	CString strFind=_T("\"ErrorNo\": \"");
			//	int iStart=strGet.Find(strFind)+strFind.GetLength();
			//	int iEnd=strGet.Find(_T("\""),iStart);
			//	CString strErrorNo=strGet.Mid(iStart,iEnd-iStart);
			//	if (strErrorNo==_T("0"))
			//	{
			//		strFind=_T("\"DownUrl\": \"");
			//		iStart=strGet.Find(strFind)+strFind.GetLength(); 
			//		iEnd=strGet.Find(_T("\""),iStart);
			//		strDownUrl=strGet.Mid(iStart,iEnd-iStart);
			//		strFind=_T("\"VersonNo\": \"");
			//		iStart=strGet.Find(strFind)+strFind.GetLength();
			//		iEnd=strGet.Find(_T("\""),iStart);
			//		strTempNowVerSinoNo=strGet.Mid(iStart,iEnd-iStart);

			//		strFind=_T("\"Size\":");
			//		iStart=strGet.Find(strFind)+strFind.GetLength();
			//		iEnd=strGet.Find(_T(","),iStart);
			//		lSize=_ttol(strGet.Mid(iStart,iEnd-iStart));

			//		strFind=_T("\"FileMd5\": \"");
			//		iStart=strGet.Find(strFind)+strFind.GetLength();
			//		iEnd=strGet.Find(_T("\""),iStart);
			//		strSerVerFileMd5=strGet.Mid(iStart,iEnd-iStart);
			//	}
			//}

			if (theApp.GetStrFromWeb(strUrl, strGet, _T(""))) {
				CString urlHeader = theApp.m_strCnkiServerId + theApp.m_strCnkiServerPath; //资源对应的url头
				Json::Reader reader;
				Json::Value itemArr;

				char buf[100000] = { 0 };
				CString2Char(strGet, buf);

				if (reader.parse(buf, itemArr, FALSE)) {
					if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
						if (itemArr["data"].empty()) {
							continue;
						}
						strDownUrl = urlHeader + string2CString(itemArr["data"]["downloadurl"].asString());
						strTempNowVerSinoNo = string2CString(itemArr["data"]["version"].asString()).Trim();
					}
				}
			}

			if (strTempNowVerSinoNo.IsEmpty())
			{
				continue;
			}

			if (strVersionNo == strTempNowVerSinoNo || CompareVerSion(strVersionNo, strTempNowVerSinoNo) <= 0)
			{
				continue;
			}


			//下载 更新包
			long lDownPart = 0;
			BOOL bFinishDownload = FALSE;
			CString strFileName = theApp.m_strCanChangePath + _T("\\") + theApp.m_strGUIDOLDUnInstal;

			strFileName += _T("\\AppUpdate\\");
			if (!PathFileExists(strFileName))
			{
				if (!CreateDirectory(strFileName, NULL))
				{
					AfxMessageBox(_T("保存路径创建失败！"));
					break;
				}

			}

			strFileName += strTempNowVerSinoNo + _T(".exe");	// 新的更新包名字
			if (PathFileExists(strFileName))
			{
				CFileStatus fileStatus;
				if (CFile::GetStatus(strFileName, fileStatus))
				{
					lDownPart = fileStatus.m_size;
				}
				if (lDownPart>lSize)
				{
					DeleteFile(strFileName);
				}
				else if (lDownPart == lSize)
				{
					bFinishDownload = TRUE;
				}
			}

			while (!strDownUrl.IsEmpty() && !bFinishDownload && !theApp.m_bQuestClose)
			{
				ULONGLONG llDownPart = lDownPart;
				int nResult = theApp.RequestHttp(strDownUrl, strFileName, llDownPart);
				lDownPart = llDownPart;
				//m_bGetUpdateFromServer=FALSE;

				if (nResult != RES_REQ_DONE_OK)
				{
					if (nResult == RES_REQ_ERROR_NOSPACE)
					{
						AfxMessageBox(_T("磁盘空间不足！"));
						break;
					}
					else if (nResult == RES_REQ_ERROR_GET && PathFileExists(strFileName))
					{
						continue;
					}
					else
					{
						break;
					}

				}
				else
				{
					bFinishDownload = TRUE;
					break;
				}
			}

			//更新ini配置文件 并删除无用的更新包
			if (bFinishDownload)
			{
				CString strFileName = theApp.m_strCanChangePath + _T("\\") + theApp.m_strGUIDOLDUnInstal + _T("\\AppUpdate\\") + strTempNowVerSinoNo + _T(".exe");
				//if (GetFileMd5(strFileName)!=strSerVerFileMd5)//判断下载的包是否正确 MD5 验证
				//{
				//	DeleteFile(strFileName);
				//	continue;
				//}
				CString strFilePath = theApp.m_strCanChangePath + _T("\\") + theApp.m_strGUIDOLDUnInstal + _T("\\ini\\AppUpdate.ini");
				::WritePrivateProfileString(_T("VERSION"), _T("VERSION"), strTempNowVerSinoNo, strFilePath);
				//删除无用更新包
				CString strOldFileName = theApp.m_strCanChangePath + _T("\\") + theApp.m_strGUIDOLDUnInstal + _T("\\AppUpdate\\") + strVersionNo + _T(".exe");
				if (PathFileExists(strOldFileName))
				{
					DeleteFile(strOldFileName);
				}
				strVersionNo = strTempNowVerSinoNo;
			}

			// 执行新的安装包
			SHELLEXECUTEINFO  ShExecInfo;
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = NULL;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = _T("open");
			ShExecInfo.lpFile = strFileName; // 执行的程序名 
			ShExecInfo.lpParameters = NULL;
			ShExecInfo.lpDirectory = NULL;
			ShExecInfo.nShow = SHOW_FULLSCREEN;              // 全屏显示这个程序 
			ShExecInfo.hInstApp = NULL;

			ShellExecuteEx(&ShExecInfo);
		}

	}

	return 1;
}



bool CMonitorDlg::Download(const CString& strUrl, const CString & strSavePath)
{
	if (strUrl.IsEmpty())
		return FALSE;
	if (strSavePath.IsEmpty())
		return FALSE;

	unsigned short nPort;       //用于保存目标HTTP服务端口  
	CString strServer, strObject;   //strServer用于保存服务器地址，strObject用于保存文件对象名称  
	DWORD dwServiceType, dwRet;      //dwServiceType用于保存服务类型，dwRet用于保存提交GET请求返回的状态号  

	CHAR *szBuffer = NULL;
	//解析URL，获取信息  
	if (!AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort))
	{
		//解析失败，该Url不正确  
		//AfxThrowInternetException(1);
		return FALSE;
	}

	//创建网络连接对象，HTTP连接对象指针和用于该连接的HttpFile文件对象指针，注意delete  
	CInternetSession intsess;
	CHttpFile *pHtFile = NULL;
	CHttpConnection *pHtCon = NULL;

	BOOL bStripMode = FALSE;
	DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS;

	CInternetSession session(_T("MNGRApp"), 1, dwAccessType);
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 800 * 1000);
	session.SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, 800 * 1000);
	session.SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, 800 * 1000);
	//建立网络连接  
	pHtCon = intsess.GetHttpConnection(strServer, nPort, _T("paperserver"), _T("wyf"));
	if (pHtCon == NULL)
	{
		//建立网络连接失败  
		intsess.Close();
		return FALSE;
	}
	//发起GET请求  
	pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject);
	if (pHtFile == NULL)
	{
		//发起GET请求失败   
		delete pHtCon; pHtCon = NULL;
		intsess.Close();
		return FALSE;
	}

	//提交请求  
	pHtFile->SendRequest();
	//获取服务器返回的状态号  
	pHtFile->QueryInfoStatusCode(dwRet);
	if (dwRet != HTTP_STATUS_OK)
	{
		//服务器不接受请求   

		delete pHtFile; pHtFile = NULL;
		delete pHtCon; pHtCon = NULL;
		intsess.Close();
		return FALSE;
	}

	TRY
	{
		//创建文件  
		CFile PicFile(strSavePath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
		CHAR sz[1024];
		DWORD dwWritten;
		//获取文件大小  
		UINT nFileSize = (UINT)pHtFile->GetLength();
		UINT nRead = 0;

		while (nFileSize>0)
		{
			nRead = pHtFile->Read(sz, 1024);
			if (nRead>0)
			{
				WriteFile(PicFile, sz, nRead, &dwWritten, NULL);
			}
			nFileSize = (UINT)pHtFile->GetLength();

		}
		// 		szBuffer = new CHAR[nFileLen] ;
		// 		CHAR* cTemp= szBuffer;
		// 		while (true)
		// 		{
		// 			// 每次下载8Kb
		// 			int   n = pHtFile->Read (cTemp, (nFileLen < 8192) ? nFileLen : 8192) ;
		// 			if (n <= 0)
		// 				break ;
		// 			PicFile.Write(szBuffer,n);
		// 
		// 			cTemp += n ; nFileLen -= n ;
		// 		}

		//关闭文件  
		PicFile.Close();
		//释放内存  
		delete[]szBuffer;
		delete pHtFile;
		delete pHtCon;
		//关闭网络连接  
		intsess.Close();

	}
	CATCH(CFileException, e)
	{
		//释放内存 
		if (szBuffer != NULL)
		{
			delete[]szBuffer;
			szBuffer = NULL;
		}

		delete pHtFile;
		delete pHtCon;
		//关闭网络连接  
		intsess.Close();
		return FALSE;          //读写文件异常  
	}
	END_CATCH

		return TRUE;
}




//void CMonitorDlg::DowloadData(CString strKindId,CString strKindCname,CString strShowKind,CStringArray& strArrayRes,CStringArray& strArrayAdd)
//{
//
//	CString  strBuyKindEname, strBuyKindCname;
//	if (strShowKind.Find(_T("+"))>0)
//	{
//		strBuyKindEname = strShowKind.Left(strShowKind.Find(_T("+")));
//		strShowKind = strShowKind.Right(strShowKind.GetLength() - strShowKind.Find(_T("+"))-1);
//	}
//	if (strShowKind.Find(_T("+"))>0)
//	{
//		strBuyKindCname = strShowKind.Left(strShowKind.Find(_T("+")));
//		strShowKind = strShowKind.Right(strShowKind.GetLength() - strShowKind.Find(_T("+")) - 1);
//	}
//
//	CString strSql;
//	for (int i=0;i<strArrayRes.GetSize();i++)
//	{
//		CString strItem=strArrayRes.GetAt(i);
//		strItem.Remove('/');
//		strSql=_T("SELECT SQL_NO_CACHE * FROM ZZHCITEM  where thname=")+strItem;
//		BSTR bstrSql=strSql.AllocSysString();
//		SysFreeString(bstrSql);
//		UINT nRecord=0;
//		if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
//		{
//			AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
//			return ;     
//		}
//		UINT nNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
//		CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
//		if (nNoCount==0)
//		{
//			strArrayAdd.Add(strItem);
//		}
//		
//	}
//
//	for (int i=0;i<strArrayAdd.GetSize();i++)
//	{
//		UINT nItemState=0;
//		//得到下载状态
//		strSql=_T("SELECT SQL_NO_CACHE STATE FROM ITEMINFOANDDOWNLOADSTATE  where thname=")+strArrayAdd.GetAt(i);
//		BSTR bstrSql=strSql.AllocSysString();
//		SysFreeString(bstrSql);
//		UINT nRecord=0;
//		if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
//		{
//			AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
//			return ;     
//		}
//		int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
//		if (iNoCount>0)
//		{
//			nItemState=_ttoi(CString(GetFieldValueEx(theApp.m_nConnectNo,nRecord,0)));
//		}
//		
//		CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
//
//		if (iNoCount==0)
//		{
//			CString strSql;
//			strSql.Format(_T("INSERT INTO ITEMINFOANDDOWNLOADSTATE (thname,kindname,c_name,showkind,年,期,state,BUYKINDENAME,BUYKINDCNAME) VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")"),
//				           strArrayAdd.GetAt(i),strKindId,strKindCname,strShowKind,strArrayAdd.GetAt(i).Mid(strKindId.GetLength(),4),
//						   strArrayAdd.GetAt(i).Mid(strKindId.GetLength() + 4), _T("0"), strKindId, strKindCname);
//			if (!ExecSQL(theApp.m_nConnectNo,strSql))
//			{
//				AfxMessageBox(_T("插入ITEMINFOANDDOWNLOADSTATE表失败！")+strSql);
//				continue;
//			}
//
//		}
//		else if(nItemState>=4)
//		{
//			continue;
//		}
//
//		//得到封面和加密数据下载地址
//		CString strGetFromUrl,strUrl,strDowloadFmUrl,strDowloadDataUrl;
//
//		strUrl = theApp.m_strCnkiServerId + _T("/questts/qr.ashx?id=");
//		strUrl += theApp.m_strRegestNo + _T("&m=1&n=") + strArrayAdd.GetAt(i) + _T("&userid=") + theApp.m_strRegestNo;
//
//		if (nItemState<=1 && theApp.GetStrFromWeb(strUrl,strGetFromUrl))
//		{
//			if (!strGetFromUrl.IsEmpty() && strGetFromUrl.Left(4)!=_T("Err:")&& strGetFromUrl.Find(_T("\"result\": false,"))<0)
//			{
//			    int iStart,iEnd;
//				if (nItemState==0)
//				{
//					iStart=strGetFromUrl.Find(_T("\"coverurl\": \""));
//					if (iStart>=0)
//					{
//						iStart+=CString(_T("\"coverurl\": \"")).GetLength();
//						iEnd=strGetFromUrl.Find(_T("\""),iStart);
//						if (iEnd>iStart)
//						{
//							strDowloadFmUrl=strGetFromUrl.Mid(iStart,iEnd-iStart);
//						}
//					}
//
//				}
//
//				iStart=strGetFromUrl.Find(_T("\"downloadurl\": \""));
//				if (iStart>=0)
//				{
//					iStart+=CString(_T("\"downloadurl\": \"")).GetLength();
//					iEnd=strGetFromUrl.Find(_T("\""),iStart);
//					if (iEnd>iStart)
//					{
//						strDowloadDataUrl=strGetFromUrl.Mid(iStart,iEnd-iStart);
//					}
//				}
//			}
//			else
//			{
//              continue;
//			}
//		}
//
//		if (!strDowloadFmUrl.IsEmpty())
//		{
//			//封面
//			CString strSavePath=m_strFmPath+strKindId+_T("\\")+strArrayAdd.GetAt(i)+_T(".jpg");
//			if (!PathFileExists(m_strFmPath + strKindId))
//			{
//				CreateDirectory(m_strFmPath + strKindId, NULL);
//			}
//			if (PathFileExists(strSavePath))
//			{
//				DeleteFile(strSavePath);
//			}
//
//			if (!Download(strDowloadFmUrl,strSavePath))
//			{
//				DeleteFile(strSavePath);
//				continue;
//			}
//
//			CString strSql;
//			strSql.Format(_T("update ITEMINFOANDDOWNLOADSTATE set state=1 where thname=%s"),strArrayAdd.GetAt(i));
//			if (!ExecSQL(theApp.m_nConnectNo,strSql))
//			{
//				AfxMessageBox(_T("更新DOWNLOADSTATE表到状态1失败！"));
//				continue;
//			}
//
//		}
//		else if(nItemState==0)
//		{
//			continue;
//		}
//
//		if (!strDowloadDataUrl.IsEmpty())
//		{
//			//加密数据
//			CString strSavePath=m_strDataPath+_T("\\WYQK\\")+strArrayAdd.GetAt(i);
//
//			if (PathFileExists(strSavePath))
//			{
//				strSql=_T("SELECT SQL_NO_CACHE * FROM cjbdlast  where thname=")+strArrayAdd.GetAt(i);
//				BSTR bstrSql=strSql.AllocSysString();
//				SysFreeString(bstrSql);
//				UINT nRecord=0;
//				if( !OpenRecordSetEx(theApp.m_nConnectNo,nRecord,bstrSql) )
//				{
//					AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
//					return ;     
//				}
//				UINT nNoCount=GetRecordCountEx(theApp.m_nConnectNo,nRecord);
//				if (nNoCount>0)
//				{
//					DeleteFile(strSavePath);
//				}
//				CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
//				
//			}
//
//			if (!PathFileExists(strSavePath))
//			{
//				ULONGLONG nPart=0;
//				int nResult=RequestHttp(strDowloadDataUrl,strSavePath,nPart);
//
//				if (nResult!=RES_REQ_DONE_OK)
//				{
//					if (nResult==RES_REQ_ERROR_NOSPACE)
//					{
//						AfxMessageBox(_T("空间不足！"));
//					}
//
//					DeleteFile(strSavePath);
//					continue;
//				}
//			}
//
//			CString strSql;
//			strSql.Format(_T("update ITEMINFOANDDOWNLOADSTATE set state=2 where thname=%s"),strArrayAdd.GetAt(i));
//			if (!ExecSQL(theApp.m_nConnectNo,strSql))
//			{
//				AfxMessageBox(_T("更新DOWNLOADSTATE表到状态2失败！"));
//				continue;
//			}
//
//			
//		}
//		else if(nItemState<=1)
//		{
//			continue;
//		}
//
//
//		//目录和otherpages
//		if (nItemState<3)
//		{
//			strUrl = theApp.m_strCnkiServerId + _T("/tsdl/tsinfo.ashx?kind=directory_allpages&itemid=") + strArrayAdd.GetAt(i);
//			CString strGetDir;
//			CString strDelSel = _T("select SQL_NO_CACHE * from newcjbdlast where thname=") + strArrayAdd.GetAt(i);
//			BSTR bstrDel = strDelSel.AllocSysString();
//			SysFreeString(bstrDel);
//			BOOL bNeedClear = FALSE;
//			nRecord = 0;
//			if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrDel))
//			{
//				AfxMessageBox(_T("OpenRecordSet Error\n") + strDelSel);
//				return;
//			}
//			UINT nNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
//			if (nNoCount>0)
//			{
//				bNeedClear = TRUE;
//			}
//			CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
//
//			if (bNeedClear)
//			{
//				if (!DeleteSelEx(theApp.m_nConnectNo, bstrDel))
//				{
//					continue;
//				}
//			}
//
//
//			//得到目录 写入数据库newcjbdlast，
//			CString strItemUpdateDate;
//			BOOL bWriteSucced = FALSE;
//			if (theApp.GetStrFromWeb(strUrl, strGetDir))
//			{
//				if (!strGetDir.IsEmpty() && strGetDir.Left(4) != _T("Err:"))
//				{
//					CString strSql;
//					strSql = _T("INSERT INTO NEWCJBDLAST (篇名,作者,第一责任人,机构,文献作者,");
//					strSql += _T("中文关键词,中文摘要,英文篇名,英文作者,英文摘要,");
//					strSql += _T("英文关键词,引文,基金,光盘号,文献号,");
//					strSql += _T("拼音刊名,中文刊名,英文刊名,出版单位,来源代码,");
//					strSql += _T("年,期,年期,语种,影响因子,");
//					strSql += _T("文件名,表名,CN,ISSN,DOI,");
//					strSql += _T("注册DOI,卷期号,EISSN,页,页数,");
//					strSql += _T("文件大小,分类号,专辑代码,专题代码,专题子栏目代码,");
//					strSql += _T("子栏目代码,全文,正文快照,SCI收录刊,EI收录刊,");
//					strSql += _T("核心期刊,中英文篇名,中英文作者,中英文刊名,中英文摘要,");
//					strSql += _T("复合关键词,主题,出版日期,更新日期,机标关键词,");
//					strSql += _T("SYS_VSM,是否高下载,是否高被引,是否高他引,下载频次,");
//					strSql += _T("被引频次,他引频次,作者代码,机构代码,机构作者代码,");
//					strSql += _T("基金代码,是否基金文献,文献标识码,文献类型标识,期刊标识码,");
//					strSql += _T("来源标识码,主题词,来源数据库,TABLENAME,作者简介,");
//					strSql += _T("文章属性,允许全文上网,允许检索,印刷页码,原文格式,");
//					strSql += _T("专题整刊代码,复合专题代码,网络总库专辑代码,期刊栏目层次,FX内容代码,");
//					strSql += _T("FX文献类型,地域导航,发布机关名称,发布机关代码,时效性,");
//					strSql += _T("发文日期,实施日期,发文字号,推荐文献,封面文献,");
//					strSql += _T("栏目,CLASS1,CLASS2,CLASS3,CLASS4,CLASS5,THNAME)");
//					strSql += _T("VALUES(\"%s\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",");
//					strSql += _T("\"\",\"\",\"%s\",\"%s\",\"\",\"\",\"\",\"%s\",\"%s\",\"\",\"\",0,");
//					strSql += _T("\"%s\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"%s\",\"\",\"\",\"\",\"\",\"\",");
//					strSql += _T("\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"%s\",\"\",");
//					strSql += _T("\"\",\"\",\"\",\"\",0,0,0,\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",");
//					strSql += _T("\"文化期刊\",\"CJBDLAST\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",");
//					strSql += _T("\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"%s\",\"\",\"\",\"\",");
//					strSql += _T("\"\",\"\",\"%s\")");
//					CString strDir, strColun, strColun_Sub, strUpdateDate, strFileName;
//					BOOL bBreak = FALSE;
//					CString strItem = strArrayAdd.GetAt(i);
//					while (!bBreak)
//					{
//						CString strFindSub = _T("/r/t");
//						CString strFind = _T("/r/n");
//						CString strPage = strGetDir.Left(strGetDir.Find(strFindSub));
//						strGetDir = strGetDir.Mid(strGetDir.Find(strFindSub) + strFindSub.GetLength());
//
//						strColun = strGetDir.Left(strGetDir.Find(strFindSub));
//						strGetDir = strGetDir.Mid(strGetDir.Find(strFindSub) + strFindSub.GetLength());
//
//						strFileName = strGetDir.Left(strGetDir.Find(strFindSub));
//						strGetDir = strGetDir.Mid(strGetDir.Find(strFindSub) + strFindSub.GetLength());
//
//						strUpdateDate = strGetDir.Left(strGetDir.Find(strFindSub));
//						strGetDir = strGetDir.Mid(strGetDir.Find(strFindSub) + strFindSub.GetLength());
//						if (strItemUpdateDate.IsEmpty() || strItemUpdateDate<strUpdateDate)
//						{
//							strItemUpdateDate = strUpdateDate;
//						}
//
//						if (strItemUpdateDate.IsEmpty() || strUpdateDate>strItemUpdateDate)
//						{
//							strItemUpdateDate = strUpdateDate;
//						}
//
//						strColun_Sub = strGetDir.Left(strGetDir.Find(strFind));
//						if (strGetDir.Find(strFind) + strFind.GetLength() == strGetDir.GetLength())
//						{
//							strGetDir.Empty();
//							bBreak = TRUE;
//						}
//						else
//						{
//							strGetDir = strGetDir.Mid(strGetDir.Find(strFind) + strFind.GetLength());
//
//						}
//						CString strSqlTemp;
//						strColun_Sub.Replace(_T("\""), _T("\\\""));
//						strSqlTemp.Format(strSql, strColun_Sub, strKindId, strKindCname, strItem.Mid(strKindId.GetLength(), 4),
//							strItem.Mid(strKindId.GetLength() + 4), strFileName, strPage, strUpdateDate, strColun, strItem);
//
//						if (!ExecSQL(theApp.m_nConnectNo, strSqlTemp))
//						{
//							AfxMessageBox(_T("插入到表NEWCJBDLAST失败！") + strSqlTemp);
//							break;
//						}
//						if (bBreak)
//						{
//							bWriteSucced = TRUE;
//						}
//					}
//
//				}
//
//
//			}
//
//			if (!bWriteSucced)
//			{
//				continue;
//			}
//			else
//			{
//				CString strSql;
//				strSql.Format(_T("update ITEMINFOANDDOWNLOADSTATE set state=3,UPDATEDATE='%s' where thname=%s"), strItemUpdateDate, strArrayAdd.GetAt(i));
//				if (!ExecSQL(theApp.m_nConnectNo, strSql))
//				{
//					AfxMessageBox(_T("更新DOWNLOADSTATE表到状态3失败！"));
//					continue;
//				}
//			}
//
//		}
//
//
//		//得到 总页数 和可看页码
//		strUrl = theApp.m_strCnkiServerId + _T("/tsdl/tsinfo.ashx?kind=itempageinfo&itemid=") + strArrayAdd.GetAt(i);
//		CString strGetPageInfo;
//		CString strSel = _T("select SQL_NO_CACHE * from ITEMINFOANDDOWNLOADSTATE where thname=") + strArrayAdd.GetAt(i);
//		BSTR bstr = strSel.AllocSysString();
//		SysFreeString(bstr);
//		nRecord = 0;
//		if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstr))
//		{
//			AfxMessageBox(_T("OpenRecordSet Error\n") + strSel);
//			return;
//		}
//		UINT nNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
//		if (nNoCount<=0)
//		{
//			CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
//			return;
//		}
//		CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
//
//		//更新数据库ITEMINFOANDDOWNLOADSTATE
//		BOOL bSucced = FALSE;
//		if (theApp.GetStrFromWeb(strUrl, strGetPageInfo))
//		{
//			if (!strGetPageInfo.IsEmpty() && strGetPageInfo.Left(4) != _T("Err:"))
//			{
//				CString strPageCount, strCanOpenPages;
//				if (strGetPageInfo.Find(_T("/"))>=0)
//				{
//					strPageCount = strGetPageInfo.Left(strGetPageInfo.Find(_T("/")));
//					strCanOpenPages = strGetPageInfo.Right(strGetPageInfo.GetLength() - strGetPageInfo.Find(_T("/")) - 1);
//				}
//				
//				if (!strPageCount.IsEmpty() && !strCanOpenPages.IsEmpty())
//				{
//					CString strSql;
//					strSql.Format(_T("update ITEMINFOANDDOWNLOADSTATE set pages=%s,otherpages=\"%s\" where thname=%s"), strPageCount, strCanOpenPages,strArrayAdd.GetAt(i));
//					if (!ExecSQL(theApp.m_nConnectNo, strSql))
//					{
//						AfxMessageBox(_T("更新ITEMINFOANDDOWNLOADSTATE的页码信息失败！"));
//						//continue;
//					}
//					else
//					{
//						bSucced = TRUE;
//					}
//				}
//
//
//			}
//		}
//
//		if (!bSucced)
//		{
//			continue;
//		}
//		else
//		{
//			CString strSql;
//			strSql.Format(_T("update ITEMINFOANDDOWNLOADSTATE set state=4 where thname=%s"),  strArrayAdd.GetAt(i));
//			if (!ExecSQL(theApp.m_nConnectNo, strSql))
//			{
//				AfxMessageBox(_T("更新DOWNLOADSTATE表到状态4失败！"));
//				continue;
//			}
//		}
//
//
//// 		//更新cjbdlast
//// 
//// 		strSql=_T("select * from newcjbdlast");
//// 		UINT nRecordNo=0;
//// 		BOOL bNeedUpdate=FALSE;
//// 		BSTR bstrSql=strSql.AllocSysString();
//// 		SysFreeString(bstrSql);
//// 		if (OpenRecordSetEx(theApp.m_nConnectNo,nRecordNo,bstrSql) && GetRecordCountEx(theApp.m_nConnectNo,nRecordNo)>0)
//// 		{
////            bNeedUpdate=TRUE;
//// 		}
//// 		CloseRecordSetEx(theApp.m_nConnectNo,nRecordNo);
//// 
//// 		if (bNeedUpdate)
//// 		{
//// 			strSql=_T("BULKLOAD TABLE cjbdlast from table newcjbdlast");
////             if(!ExecSQL(theApp.m_nConnectNo,strSql))
//// 			{
//// 				continue;
//// 			}
//// 		}
//// 		else
//// 		{
//// 			continue;
//// 		}
//	}
//
//
//	
//}

DWORD WINAPI CMonitorDlg::ThreadGetUpdateInfoAndDataEx(LPVOID lpParame)
{

	//CMonitorDlg* pDlg = (CMonitorDlg*)lpParame;

	//int iHour = -1;
	//CTime tmNow = CTime::GetCurrentTime();


	////得到最新的购买种类
	//CString strURL, strGetFromWeb;
	//strURL.Format(_T("%s/tsdl/tsinfo.ashx?kind=userbuysubkind&findparentkind=cjfd&cid=%s"), theApp.m_strCnkiServerId,
	//	theApp.m_strRegestNo);

	//while (!theApp.m_bQuestClose)
	//{

	//	if (theApp.GetStrFromWeb(strURL, strGetFromWeb))
	//	{
	//		CStringArray strArrayNew;
	//		if (!strGetFromWeb.IsEmpty() && strGetFromWeb.Left(4) != _T("Err:"))
	//		{
	//			if (strGetFromWeb.Left(12) == _T("userbuyzzhc\%") && strGetFromWeb.Right(12) == _T("\%userbuyzzhc"))
	//			{
	//				strGetFromWeb = strGetFromWeb.Mid(12, strGetFromWeb.GetLength() - 24);
	//				int iStart = 0;
	//				int iEnd = 0;
	//				iEnd = strGetFromWeb.Find(_T("-"), iStart);
	//				if (iEnd<0)
	//				{
	//					iEnd = strGetFromWeb.GetLength() - 1;
	//				}
	//				while (iEnd > iStart)
	//				{
	//					strArrayNew.Add(strGetFromWeb.Mid(iStart, iEnd - iStart));
	//					strGetFromWeb = strGetFromWeb.Right(strGetFromWeb.GetLength() - iEnd - 1);

	//					iStart = 0;
	//					iEnd = strGetFromWeb.Find(_T("-"), iStart);
	//					if (iEnd<0)
	//					{
	//						iEnd = strGetFromWeb.GetLength() - 1;
	//					}
	//				}
	//				if (strGetFromWeb.GetLength() > 0)
	//				{
	//					strArrayNew.Add(strGetFromWeb);
	//				}
	//			}

	//		}
	//		if (strArrayNew.GetSize()>0)
	//		{

	//			BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	//			BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//			BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//			UINT nPort = _ttoi(theApp.m_strKbasePort);

	//			if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	//			{
	//				SysFreeString(bstrID);
	//				SysFreeString(bstrUserName);
	//				SysFreeString(bstrUserPassword);
	//				AfxMessageBox(_T("数据库连接失败！"));
	//				return FALSE;
	//			}
	//			SysFreeString(bstrID);
	//			SysFreeString(bstrUserName);
	//			SysFreeString(bstrUserPassword);


	//			CString strSql;
	//			CString strNotIn;
	//			for (size_t i = 0; i < strArrayNew.GetSize(); i++)
	//			{
	//				strNotIn += _T(" not id=") + strArrayNew.GetAt(i);
	//			}

	//			strSql.Format(_T("update USERBUYKIND set KINDTYPE=0  where  ID=*%s"), strNotIn);

	//			if (!ExecSQL(theApp.m_nConnectNo, strSql))
	//			{
	//				AfxMessageBox(_T("update Error\n"));
	//				return FALSE;
	//			}

	//			for (size_t i = 0; i < strArrayNew.GetSize(); i++)
	//			{

	//				strSql.Format(_T("SELECT KINDTYPE from USERBUYKIND where ID=\'%s\' and parentid='cjfd' "), strArrayNew.GetAt(i));
	//				UINT nRecordNo = 0;
	//				if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecordNo, strSql.AllocSysString()))
	//				{
	//					AfxMessageBox(_T("OpenRecordSetEx Error\n"));
	//					return FALSE;
	//				}
	//				UINT nNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecordNo);
	//				if (nNoCount <= 0)
	//				{
	//					CloseRecordSetEx(theApp.m_nConnectNo, nRecordNo);


	//					strSql = _T("INSERT INTO USERBUYKIND (ID,parentid,KINDTYPE) ");
	//					strSql.AppendFormat(_T("VALUES(\'%s\',\'cjfd\',\'1\')"), strArrayNew.GetAt(i));
	//					ExecSQL(theApp.m_nConnectNo, strSql);
	//					continue;
	//				}
	//				else if (_ttoi(CString(GetFieldValueEx(theApp.m_nConnectNo, nRecordNo, 0))) == 0)
	//				{
	//					SetFieldValueEx(theApp.m_nConnectNo, nRecordNo, _T("1"), 0);
	//				}

	//				CloseRecordSetEx(theApp.m_nConnectNo, nRecordNo);

	//			}
	//			break;
	//		}
	//	}


	//}

	////下载更新包
	//tmNow = CTime::GetCurrentTime();
	//CTime  ctmPre = tmNow;
	//CTimeSpan cts(1, 0, 0, 0);  //设置天数差
	//while (!theApp.m_bQuestClose)
	//{
	//	Sleep(5);
	//	if (tmNow.GetHour()>8 && iHour != tmNow.GetHour())
	//	{
	//		ULONG nLStartYMD, nLEndYMD;

	//		theApp.CorrectBuyYearEx(nLStartYMD, nLEndYMD);


	//		CString strTempUrl, strTempDir;

	//		if (!PathFileExists(theApp.m_strCanChangePath + _T("\\TempGet")))
	//		{
	//			CreateDirectory(theApp.m_strCanChangePath + _T("\\TempGet"), NULL);
	//		}

	//		if (!PathFileExists(theApp.m_strCanChangePath + _T("\\TempGet\\cjfd")))
	//		{
	//			CreateDirectory(theApp.m_strCanChangePath + _T("\\TempGet\\cjfd"), NULL);
	//		}

	//		if (nLStartYMD<_ttol(theApp.m_strStartYearMonthDay))
	//		{
	//			nLStartYMD = _ttol(theApp.m_strStartYearMonthDay);
	//		}

	//		CArray<ULONG, ULONG> nlArrayItem;

	//		CArray<ULONGLONG, ULONGLONG> nllArrayItemGeted;

	//		ULONG lNowDown = _ttol(ctmPre.Format(_T("%Y%m%d")));

	//		while (lNowDown <= nLEndYMD && lNowDown >= nLStartYMD)
	//		{

	//			BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	//			BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//			BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//			UINT nPort = _ttoi(theApp.m_strKbasePort);

	//			if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	//			{
	//				SysFreeString(bstrID);
	//				SysFreeString(bstrUserName);
	//				SysFreeString(bstrUserPassword);
	//				AfxMessageBox(_T("数据库连接失败！"));
	//				return FALSE;
	//			}
	//			SysFreeString(bstrID);
	//			SysFreeString(bstrUserName);
	//			SysFreeString(bstrUserPassword);

	//			CString strSql;
	//			strSql.Format(_T("SELECT STATE,DOWNLPART FROM UPDATEINFO  where id=%ld "), lNowDown);
	//			BSTR bstrSql = strSql.AllocSysString();
	//			SysFreeString(bstrSql);
	//			UINT nRecord = 0;

	//			if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//			{
	//				AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//				return 0;
	//			}
	//			int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
	//			UINT nState = 0;
	//			ULONGLONG nLtemp = 0;
	//			if (iNoCount>0)
	//			{
	//				CString strKindId, strBookId;
	//				nState = _ttoi(CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 0)));
	//				if (nState == 1)
	//				{
	//					nLtemp = _ttol(CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 1)));
	//				}

	//			}

	//			CloseRecordSetEx(theApp.m_nConnectNo, nRecord);

	//			if (nState < 2)
	//			{
	//				if (nLtemp>0)
	//				{
	//					nlArrayItem.InsertAt(0, lNowDown);
	//					nllArrayItemGeted.InsertAt(0, nLtemp);
	//				}
	//				else
	//				{
	//					nlArrayItem.Add(lNowDown);
	//					nllArrayItemGeted.Add(0);
	//				}

	//			}
	//			ctmPre = ctmPre - cts;  //1天前
	//			lNowDown = _ttol(ctmPre.Format(_T("%Y%m%d")));

	//		}

	//		for (ULONG i = 0; i < nlArrayItem.GetSize() && !theApp.m_bQuestClose; i++)
	//		{

	//			strTempUrl.Format(_T("%s/oneday_update_package/CJFD/%ld.zip"), theApp.m_strCnkiServerId, nlArrayItem.GetAt(i));
	//			strTempDir.Format(_T("%s\\TempGet\\cjfd\\%ld.zip"), theApp.m_strCanChangePath, nlArrayItem.GetAt(i));


	//			ULONGLONG nLtemp = nllArrayItemGeted.GetAt(i);
	//			if (nLtemp == 0 && PathFileExists(strTempDir))
	//			{
	//				DeleteFile(strTempDir);

	//			}
	//			if (nLtemp>0 && !PathFileExists(strTempDir))
	//			{
	//				nLtemp = 0;
	//			}

	//			UINT nResult = theApp.RequestHttp(strTempUrl, strTempDir, nLtemp);
	//			if (nResult != RES_REQ_DONE_OK)
	//			{
	//				if (nResult == RES_REQ_ERROR_NOSPACE)
	//				{
	//					AfxMessageBox(theApp.m_strCanChangePath + _T(" 空间不足！"), MB_OK | MB_SYSTEMMODAL);
	//					break;
	//				}
	//				else if (nResult == RES_REQ_ERROR_GET && PathFileExists(strTempDir))
	//				{
	//					//DeleteFile(strTempDir);
	//					pDlg->UpdateUpdateTable(nlArrayItem.GetAt(i), 1, nLtemp);

	//					break;
	//				}

	//			}
	//			else
	//			{
	//				CZipImplement ZipTemp;
	//				CString strDirUnZip, strDirUnZipTemp;
	//				strDirUnZip.Format(_T("%s\\TempGet\\cjfd"), theApp.m_strCanChangePath);
	//				strDirUnZipTemp.Format(_T("%s\\TempGet\\cjfd\\%ld"), theApp.m_strCanChangePath, nlArrayItem.GetAt(i));
	//				if (PathFileExists(strDirUnZipTemp))
	//				{
	//					ClearDirectory(strDirUnZipTemp);
	//				}
	//				if (!ZipTemp.Zip_UnPackFiles(strTempDir, strDirUnZip))
	//				{
	//					ClearDirectory(strDirUnZipTemp);

	//				}
	//				else
	//				{
	//					DeleteFile(strTempDir);
	//					pDlg->UpdateUpdateTable(nlArrayItem.GetAt(i), 2, 0);
	//				}



	//			}
	//		}
	//		iHour = tmNow.GetHour();
	//	}
	//	tmNow = CTime::GetCurrentTime();
	//}
	return 1;
}

CString string2CString(string stringres)
{
	DWORD dwMinSize;
	CString strReturn;
	dwMinSize = MultiByteToWideChar(0, 0, stringres.c_str(), -1, NULL, 0);
	wchar_t *desWtemp = new wchar_t[dwMinSize];
	MultiByteToWideChar(0, 0, stringres.c_str(), -1, desWtemp, dwMinSize);
	strReturn = CString(desWtemp);
	delete[]desWtemp;
	return strReturn;
}

char* UnicodeToUTF8(const wchar_t *str, long& lCountNo)
{
	char * result;
	long textlen;
	// wide char to multi char
	textlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char *)malloc((textlen + 1)*sizeof(char));
	memset(result, 0, sizeof(char)* (textlen + 1));
	WideCharToMultiByte(CP_UTF8, 0, str, -1, result, textlen, NULL, NULL);
	lCountNo = textlen;
	return result;
}

void GetSysBit(bool& bIs32Bit)
{
	bIs32Bit = true;
	SYSTEM_INFO si;
	// Copy the hardware information to the SYSTEM_INFO structure.  
	GetNativeSystemInfo(&si);
	// Display the contents of the SYSTEM_INFO structure.  

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		bIs32Bit = false;
	}
	else{
		bIs32Bit = true;
	}
}

BOOL InserterHtml(CString strHtmlModePath, CString strHtmlPath, CString strAdd)
{
	if (!PathFileExists(strHtmlModePath))
	{
		return false;
	}

	if (PathFileExists(strHtmlPath))
	{
		DeleteFile(strHtmlPath);
		//return TRUE;
	}

	CString   strReadHtml;
	CFile   fileTemplate;

	if (fileTemplate.Open(strHtmlModePath, CFile::modeRead, NULL))
	{

		char* ptchBuffer = NULL;

		int nCount = fileTemplate.GetLength();

		ptchBuffer = new char[nCount + 1];

		ptchBuffer[nCount] = '\0';

		fileTemplate.Read(ptchBuffer, fileTemplate.GetLength());

		fileTemplate.Close();
		strReadHtml = CA2W(ptchBuffer, CP_UTF8);

		if (NULL != ptchBuffer)
		{
			delete[] ptchBuffer;

			ptchBuffer = NULL;
		}

		if (strAdd.Left(1) == _T("\""))
		{
			strAdd = strAdd.Right(strAdd.GetLength() - 1);
		}

		if (strAdd.Right(1) == _T("\""))
		{
			strAdd = strAdd.Left(strAdd.GetLength() - 1);
		}
		strAdd.Replace(_T("\\\""), _T("\""));

		strReadHtml.Replace(_T("&emptyforchange&"), strAdd);


		CStdioFile   fileNew;
		if (!fileNew.Open(strHtmlPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			AfxMessageBox(_T("打开文件失败！"));

			return FALSE;

		}
		//   WORD unicodeFlag = 0xFEFF;  // 文件采用unicode格式  

		char szBOM[3] = { (char)0xEF, (char)0xBB, (char)0xBF };
		CString strHeader;
		strHeader.Format(_T("%s"), szBOM);
		fileNew.Write(szBOM, 3);
		long lWrite = 0;
		char* pTempWrite = UnicodeToUTF8(strReadHtml, lWrite);

		fileNew.Write(pTempWrite, lWrite);
		delete pTempWrite;

		fileNew.Close();


		return TRUE;
	}


	return FALSE;
}

BOOL CMonitorDlg::SecrectData(CString strRescPath, CString strDirPath)
{
	CString stTempFileName;
	UINT nBlockSize = 512;

	//加密
	if (PathFileExists(strDirPath))
	{
		//AfxMessageBox(strWrite+_T("已经存在！"));
		DeleteFile(strDirPath);
	}

	void *cryptProv = NULL;
	void *cryptProv1 = NULL;
	TCALG_ID id = camIDEA;
	cryptProv = TCryptCreate(id, "0A54223F-E4D5-4E", 16);
	cryptProv1 = TCryptCreate(TCALG_RC4, "0A54223F-E4D5-4E", 16);
	fstream fileOut;
	fileOut.open(strRescPath, ios_base::in | ios_base::out | ios_base::app | ios_base::binary);
	//fileOut.seekg(0, ios::end);
	//ULONGLONG lDataLength = fileOut.tellg();
	//fileOut.seekg(0, ios::);
	fstream file;
	file.open(strDirPath, ios_base::in | ios_base::out | ios_base::app | ios_base::binary);
	char cPart[512 + 1];
	char cOut[512 * OUTSIZE_N + 1];

	bool bFinish = FALSE;
	ULONGLONG nGetSizeCount = 0;
	ULONGLONG nTemp = 0;
	UINT i = 0;
	BOOL bLaskUBlock = false;
	while (!fileOut.eof())
	{
		fileOut.read(cPart, nBlockSize);
		if (i%OUTSIZE_N == 0)
		{
			memset(cOut, 0, sizeof(cOut));
			nGetSizeCount = 0;
		}
		UINT nGetSize = fileOut.gcount();
		if (nGetSize == nBlockSize)
		{
			TCryptEncrypt(cryptProv, (unsigned char *)cPart, nGetSize, (unsigned char *)cOut + (i%OUTSIZE_N) * 512);
			nGetSizeCount += nBlockSize;
		}
		else
		{
			TCryptEncrypt(cryptProv1, (unsigned char *)cPart, nGetSize, (unsigned char *)cOut + (i%OUTSIZE_N) * 512);
			bLaskUBlock = TRUE;
			nGetSizeCount += nGetSize;
		}

		memset(cPart, 0, sizeof(cPart));


		if (i%OUTSIZE_N == OUTSIZE_N - 1 || bLaskUBlock || fileOut.eof())
		{
			file.write(cOut, nGetSizeCount);
			nTemp += nGetSizeCount;
		}

		i++;
	}

	TCryptDelete(cryptProv);
	TCryptDelete(cryptProv1);
	file.close();

	return TRUE;
}



BOOL CMonitorDlg::ItemIsInTable(CString strTableName, CString strFindConditions)
{
	CString strSql;
	BOOL bExist = FALSE;

	strSql.Format(_T("SELECT * from \'%s\' where %s"), strTableName, strFindConditions);
	CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
R
	while (!theApp.LockRead(INFINITE, 10));
	if (stmt != NULL && !stmt->IsNullStmt())
	{
		if (stmt->NextRow())
		{
			bExist = TRUE;
		}

		delete stmt;
		stmt = NULL;
	}
	theApp.UnLockRead();
ER
	Sleep(10);
	return bExist;
}

CString GUIDGen()
{
	CString sguid;
	GUID guid;
	HRESULT hResult = CoCreateGuid(&guid);
	if (S_OK == hResult)
	{
		CComBSTR bstrGuid(guid);
		sguid = bstrGuid;
	}
	return sguid;
}



BOOL BeExistInTable(CString strItem, CString strTable, BOOL& bExist)
{
	bExist = false;
	CString strSql;
	strSql.Format(_T("select COUNT(*) from '%s' where ID = '%s' and BeValid='1'"), strTable, strItem);

R
	while (!theApp.LockRead(INFINITE, 10));

	CSqlStatement *  stmt = theApp.m_sqlite.Statement(strSql);
	if (stmt != NULL && !stmt->IsNullStmt())
	{
		if (stmt->NextRow() && _ttoi(CString(stmt->ValueString(0))) > 0)
		{
			bExist = true;
		}

		delete stmt;
		stmt = NULL;
		theApp.UnLockRead();
ER
		Sleep(10);
	}
	else
	{
		theApp.UnLockRead();
ER
		Sleep(10);
		AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
		return FALSE;
	}
	return TRUE;
}


BOOL GetUpdateInfoAndData_Kind1(Json::Value itemArr, CString strQuestKind, CString strQuestSubKind, CString & strLastUpdateStander)//类似期刊类更新
{
	if (strLastUpdateStander.Find(_T("_")) <= 0)
	{
		return false;
	}
	CString strAddUpdateStanderTemp = strLastUpdateStander.Left(strLastUpdateStander.Find(_T("_")));
	CString strUpdateAndDelUpdateStanderTemp = strLastUpdateStander.Right(strLastUpdateStander.GetLength() - strLastUpdateStander.Find(_T("_")) - 1);

	//CString strAddUpdateStanderTemp = strAddUpdateStander;
	//CString strUpdateAndDelUpdateStanderTemp = strUpdateAndDelUpdateStander;


	if (string2CString(itemArr["BSuccess"].asString()).CompareNoCase(_T("true")) == 0
		&& itemArr["nReturnSubCount"].asInt() > 0)
	{
		CString strTableName = string2CString(itemArr["StrInTableName"].asString());

		//得到表结构  
		struct STRUCTTABLEATRRIBUTE
		{
			CString strName;
			CString strValue;
		};
		CArray<STRUCTTABLEATRRIBUTE, STRUCTTABLEATRRIBUTE> structTableAtrriArray;
		CArray<STRUCTTABLEATRRIBUTE, STRUCTTABLEATRRIBUTE> structGetInfoAtrriArray;

		int iTableLevel = -1;

		while (!theApp.LockRead(INFINITE, 10));
		//查询给定表是否存在
		BOOL bTabelExist = false;
		CString strSql;
		strSql.Format(_T("select COUNT(*) from sqlite_master where type = 'table' and name = '%s'"), strTableName);

		CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
		if (stmt != NULL && !stmt->IsNullStmt())
		{

			if (stmt->NextRow() && _ttoi(CString(stmt->ValueString(0))) > 0)
			{
				bTabelExist = TRUE;
			}

			delete stmt;
			stmt = NULL;

		}
		else
		{
			theApp.UnLockRead();
			AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
			return FALSE;
		}
		if (!bTabelExist)
		{
			theApp.UnLockRead();
			AfxMessageBox(_T("指定的表不存在！表名为：") + strTableName);
			return FALSE;
		}
		//查询所有属性
		strSql.Format(_T("PRAGMA table_info([%s])"), strTableName);
		stmt = theApp.m_sqlite.Statement(strSql);
		if (stmt != NULL)
		{
			while (stmt->NextRow())
			{
				STRUCTTABLEATRRIBUTE item;
				item.strName = stmt->ValueString(1);
				structTableAtrriArray.Add(item);
			}

			delete stmt;
			stmt = NULL;

		}
		else
		{
			theApp.UnLockRead();
			AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
			return FALSE;
		}

		//查看表的等级
		CString strFindColumnName = _T("GGGPARENTID");
		for (int i = 4; i >= 1; i--)
		{
			strSql.Format(_T("select COUNT(*) from sqlite_master where name = '%s' and sql like '%%%s%%'"), strTableName, strFindColumnName);
			stmt = theApp.m_sqlite.Statement(strSql);
			if (stmt != NULL && !stmt->IsNullStmt())
			{
				if (stmt->NextRow() && _ttoi(CString(stmt->ValueString(0))) > 0)
				{
					iTableLevel = i;
				}

				delete stmt;
				stmt = NULL;
				if (iTableLevel > 0)
					break;
			}
			else
			{
				theApp.UnLockRead();
				AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
				return FALSE;
			}
			strFindColumnName = strFindColumnName.Right(strFindColumnName.GetLength() - 1);
		}


		theApp.UnLockRead();

		if (iTableLevel<0)
		{
			return false;
		}

		struct SructMustNeed
		{
			CString StrID; //更新数据的ID
			UINT nKind;//操作类型 1.增加 2.修改（注：修改本和单篇）3.删除(注：删单本)
			CString strSubKind;//0 无 1 修改 strCname 2 修改 strFmDurl 3 修改 StrDataDUrl 4.修改StrDir（目前无用 为了减少副服务器压力 通过6 来解决） 5修改  StrOtherPage 6修改nSubSubId 指定的子数据
			// 1 2 3 5 可以叠加
			CString StrCname;  //中文名

			CString StrFmDUrl;  //封面下载地址

			CString StrDataDUrl;  //数据下载地址

			CString StrDir;   //目录相关（json格式：SFileid:,STitle,SPageNo）

			CString StrOtherPage;  // 其他可用页

			UINT nSubSubId;  //对应nSubKind=6

			UINT nSubSubIdKind;  //对nSubKind=6 的实现：2为修改3为删除

			CString StrSubSubInfo;//对应nSubKind=6 (json格式：,Sfileid,STitle,SPageNo）

			CString StrDate;//
			CString  StrTypeId;
		};


		CStringArray strOperSqlArray;

		int iContent_size = itemArr["List"].size();


		for (int j = 0; j < iContent_size; j++)
		{

			strOperSqlArray.RemoveAll();
			structGetInfoAtrriArray.RemoveAll();
			SructMustNeed MustNeedItem;
			Json::Value::Members getmember = itemArr["List"][j].getMemberNames();

			for (size_t in = 0; in < getmember.size(); in++)//记录所有得到的 属性及值
			{
				STRUCTTABLEATRRIBUTE item;

				item.strName = string2CString(getmember[in]);

				if (item.strName.CompareNoCase(_T("StrDir")) == 0 || item.strName.CompareNoCase(_T("StrSubSubInfo")) == 0)
				{
					structGetInfoAtrriArray.Add(item);
				}
				else
				{
					char* pCharTemp = CString2char(item.strName);

					item.strValue = string2CString(itemArr["List"][j][pCharTemp].asString());

					structGetInfoAtrriArray.Add(item);

					delete pCharTemp;
					pCharTemp = NULL;

					//填充必选项
					if (item.strName.CompareNoCase(_T("StrID")) == 0)
					{
						MustNeedItem.StrID = item.strValue;
					}
					else if (item.strName.CompareNoCase(_T("nKind")) == 0)
					{
						MustNeedItem.nKind = _ttoi(item.strValue);
					}
					else if (item.strName.CompareNoCase(_T("SSubKind")) == 0)
					{
						MustNeedItem.strSubKind = item.strValue;
					}
					else if (item.strName.CompareNoCase(_T("StrCname")) == 0)
					{
						MustNeedItem.StrCname = item.strValue;
					}
					else if (item.strName.CompareNoCase(_T("StrFmDUrl")) == 0)
					{
						MustNeedItem.StrFmDUrl = item.strValue;
					}
					else if (item.strName.CompareNoCase(_T("StrDataDUrl")) == 0)
					{
						MustNeedItem.StrDataDUrl = item.strValue;
					}
					else if (item.strName.CompareNoCase(_T("StrOtherPage")) == 0)
					{
						MustNeedItem.StrOtherPage = item.strValue;
					}
					else if (item.strName.CompareNoCase(_T("nSubSubId")) == 0)
					{
						MustNeedItem.nSubSubId = _ttoi(item.strValue);
					}
					else if (item.strName.CompareNoCase(_T("nSubSubIdKind")) == 0)
					{
						MustNeedItem.nSubSubIdKind = _ttoi(item.strValue);
					}
					else if (item.strName.CompareNoCase(_T("StrTypeId")) == 0)
					{
						MustNeedItem.StrTypeId = item.strValue;
					}
					else if (item.strName.CompareNoCase(_T("StrDate")) == 0)
					{
						MustNeedItem.StrDate = item.strValue;
					}

				}

			}



			//查看指定id是否在表内存在  +  已有数据存放路径
			bool beExistOperatorItem = false;
			CString strGetFmPathFromTable, strGetDataPathFromTable;
			while (!theApp.LockRead(INFINITE, 10));
			strSql.Format(_T("select count(*),FmPath,DataPath  from '%s' where id = '%s'  and  BeValid='1'"), strTableName, MustNeedItem.StrID);
			stmt = theApp.m_sqlite.Statement(strSql);
			if (stmt != NULL && !stmt->IsNullStmt())
			{
				if (stmt->NextRow() && _ttoi(CString(stmt->ValueString(0))) > 0)
				{
					beExistOperatorItem = TRUE;

					strGetFmPathFromTable = stmt->ValueString(1);
					strGetDataPathFromTable = stmt->ValueString(2);
				}
				delete stmt;
				stmt = NULL;

			}
			else
			{
				theApp.UnLockRead();
				AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
				return FALSE;
			}


			CString strAppendSavePath;//按照层级存储的层级

			//得到所有父类
			CString strGGGParent, strGGParent, strGParent, strParent;

			CString strFindId;
			if (MustNeedItem.nKind == 1)
			{
				strFindId = MustNeedItem.StrTypeId;

			}
			else
			{
				strFindId = MustNeedItem.StrID;
			}

			if (iTableLevel == 4)
			{
				strSql.Format(_T("select GGGParentID,GGParentID,GParentID,ParentID from '%s' where id = '%s'  and  BeValid='1'"), strTableName, strFindId);
			}
			else if (iTableLevel == 3)
			{
				strSql.Format(_T("select GGParentID,GParentID,ParentID from '%s' where id = '%s'  and  BeValid='1'"), strTableName, strFindId);
			}
			else if (iTableLevel == 2)
			{
				strSql.Format(_T("select GParentID,ParentID from '%s' where id = '%s'  and  BeValid='1'"), strTableName, strFindId);
			}
			else if (iTableLevel == 1)
			{
				strSql.Format(_T("select ParentID from '%s' where id = '%s'  and  BeValid='1'"), strTableName, MustNeedItem.StrID);
			}
			else
				return false;
			stmt = theApp.m_sqlite.Statement(strSql);
			if (stmt != NULL && !stmt->IsNullStmt())
			{
				if (stmt->NextRow())
				{

					if (iTableLevel == 4)
					{
						strGGGParent = stmt->ValueString(0);

						strGGParent = stmt->ValueString(1);

						strGParent = stmt->ValueString(2);

						strParent = stmt->ValueString(3);
						strAppendSavePath = strGGGParent + _T("\\") + strGGParent + _T("\\") + strGParent + _T("\\") + strParent;
					}
					else if (iTableLevel == 3)
					{
						strGGParent = stmt->ValueString(0);

						strGParent = stmt->ValueString(1);

						strParent = stmt->ValueString(2);

						strAppendSavePath = strGGParent + _T("\\") + strGParent + _T("\\") + strParent;
					}
					else if (iTableLevel == 2)
					{
						strGParent = stmt->ValueString(0);

						strParent = stmt->ValueString(1);

						strAppendSavePath = strGParent + _T("\\") + strParent;
					}
					else if (iTableLevel == 1)
					{
						strParent = stmt->ValueString(0);

						strAppendSavePath = strParent;
					}

				}
				delete stmt;
				stmt = NULL;

			}
			else
			{
				theApp.UnLockRead();
				AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
				return FALSE;
			}



			theApp.UnLockRead();


			//下载数据
			switch (MustNeedItem.nKind)
			{
			case 1://操作类型是 增加
				if (!beExistOperatorItem)
				{
					//下载 fm
					CString strSaveFmPath = theApp.m_strTotalFmPath + _T("\\") + strAppendSavePath + _T("\\") + MustNeedItem.StrID;

					if (!theApp.DownLoadData(MustNeedItem.StrFmDUrl, strSaveFmPath, 1)) return false;

					for (size_t ij = 0; ij < structGetInfoAtrriArray.GetSize(); ij++)
					{
						if (structGetInfoAtrriArray.GetAt(ij).strName.CompareNoCase(_T("StrFmDUrl")) == 0)
						{
							structGetInfoAtrriArray.GetAt(ij).strValue = strSaveFmPath;
							break;
						}

					}


					// 下载 data
					CString strSaveDataPath = theApp.m_strTotalDataPath + _T("\\") + strAppendSavePath + _T("\\") + MustNeedItem.StrID;


					if (!theApp.DownLoadData(MustNeedItem.StrDataDUrl, strSaveDataPath, 2)) return false;

					for (size_t ij = 0; ij < structGetInfoAtrriArray.GetSize(); ij++)
					{
						if (structGetInfoAtrriArray.GetAt(ij).strName.CompareNoCase(_T("StrDataDUrl")) == 0)
						{
							structGetInfoAtrriArray.GetAt(ij).strValue = strSaveDataPath;
							break;
						}
					}

					//得到type

					while (!theApp.LockRead(INFINITE, 10));
					//通过给定表 查询Type;
					strSql.Format(_T("select  *  from '%s' where ID = '%s' and  BeValid='1'"), strTableName, MustNeedItem.StrTypeId);

					CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
					if (stmt != NULL && !stmt->IsNullStmt())
					{
						if (stmt->NextRow())
						{
							CString strTemp;
							for (size_t ij = 0; ij < structTableAtrriArray.GetSize(); ij++)
							{
								strTemp = stmt->ValueString(ij);
								if (ij == 0)
								{
									structTableAtrriArray.GetAt(ij).strValue = MustNeedItem.StrID;
								}
								else if (strTemp.GetLength()>5 && (strTemp.Left(2) == _T("%d") || strTemp.Left(2) == _T("%s") || strTemp.Left(3) == _T("%ld")))
								{
									CString strFindId = strTemp.Mid(strTemp.Find(_T("_")) + 1, strTemp.ReverseFind('_') - strTemp.Find(_T("_")) - 1);
									for (size_t in = 0; in < structGetInfoAtrriArray.GetSize(); in++)
									{
										if (structGetInfoAtrriArray.GetAt(in).strName.CompareNoCase(strFindId) == 0)
										{
											structTableAtrriArray.GetAt(ij).strValue = structGetInfoAtrriArray.GetAt(in).strValue;
											break;
										}
									}
								}
								else
								{
									structTableAtrriArray.GetAt(ij).strValue = strTemp;
								}
								if (structTableAtrriArray.GetAt(ij).strName.CompareNoCase(_T("BeTemplate")) == 0)
								{
									structTableAtrriArray.GetAt(ij).strValue = _T("0");
								}


							}
						}

						delete stmt;
						stmt = NULL;

					}
					else
					{
						theApp.UnLockRead();
						AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
						return FALSE;
					}


					//修正目录表

					int iDirTableNo = 0;
					bool bNeedCreateTableForDir = false;
					while (true)
					{
						strSql.Format(_T("select count(*)  from 'DIR_%d' "), iDirTableNo);
						stmt = theApp.m_sqlite.Statement(strSql);
						if (stmt != NULL && !stmt->IsNullStmt())
						{
							if (stmt->NextRow() && _ttol(CString(stmt->ValueString(0))) > 1000000)
							{
								iDirTableNo++;
							}
							else
							{
								break;
							}
							delete stmt;
							stmt = NULL;

						}
						else
						{
							bNeedCreateTableForDir = true;
							break;
						}
					}

					if (bNeedCreateTableForDir)
					{
						//加入创建新表语句
						CString strSqlAdd;
						strSqlAdd.Format(_T("CREATE TABLE \"DIR_%d\" (\"ID\"  TEXT NOT NULL,\"C_NAME\"  TEXT,\"pages\"  TEXT,\"thname\"  TEXT,PRIMARY KEY(\"ID\")"), iDirTableNo);
						strOperSqlArray.Add(strSqlAdd);
					}


					theApp.UnLockRead();

					//根据type 得到真正的插入语句

					CString strSqlAdd;

					BOOL bExist = FALSE;
					if (!BeExistInTable(MustNeedItem.StrID, strTableName, bExist)) return FALSE;
					if (!bExist)
					{
						CString strTemp;
						strSqlAdd = _T("replace into ") + strTableName + _T(" values(");
						for (size_t ij = 0; ij < structTableAtrriArray.GetSize(); ij++)
						{
							strTemp = structTableAtrriArray.GetAt(ij).strValue;
							if (strTemp.Find(_T("'"))>0)
							{
								strTemp.Replace(_T("'"), _T("''"));
							}
							strSqlAdd += _T(" '") + strTemp + _T("' ");
							if (ij + 1 != structTableAtrriArray.GetSize())
							{
								strSqlAdd += _T(",");
							}
							else
							{
								strSqlAdd += _T(")");
							}
						}

						strOperSqlArray.Add(strSqlAdd);
					}

					//加入 目录
					int iSubContent_size = itemArr["List"][j]["StrDir"].size();
					for (int in = 0; in < iSubContent_size; in++)
					{
						bExist = FALSE;

						if (!BeExistInTable(string2CString(itemArr["List"][j]["StrDir"][in]["SFileid"].asString()), strTableName, bExist)) return FALSE;
						if (!bExist)
						{
							CString strTitle = string2CString(itemArr["List"][j]["StrDir"][in]["STitle"].asString());
							if (strTitle.Find(_T("'"))>0)
							{
								strTitle.Replace(_T("'"), _T("''"));
							}
							strSqlAdd.Format(_T("replace into 'DIR_%d' values('%s','%s','%s','%s')"), iDirTableNo, string2CString(itemArr["List"][j]["StrDir"][in]["SFileid"].asString()),
								strTitle, string2CString(itemArr["List"][j]["StrDir"][in]["SPageNo"].asString()), MustNeedItem.StrID);
							strOperSqlArray.Add(strSqlAdd);
						}
					}


					//加入 修正更新时间语句

					if (strAddUpdateStanderTemp != MustNeedItem.StrID)
					{
						strAddUpdateStanderTemp = MustNeedItem.StrID;
						strLastUpdateStander = strAddUpdateStanderTemp + _T("_") + strUpdateAndDelUpdateStanderTemp;
						strSqlAdd.Format(_T("update UPDATEINFO set LASTUPDATESTANDER='%s' where  ID=\'%s\'"), strLastUpdateStander, strQuestKind);
						strOperSqlArray.Add(strSqlAdd);
					}


				}
				break;

			case 2://操作类型是修改
				if (beExistOperatorItem)
				{
					vector<int> vIntSubKind;
					CString strTemp = MustNeedItem.strSubKind;
					while (strTemp.Find(_T("+"))>0)
					{
						vIntSubKind.push_back(_ttoi(strTemp.Left(strTemp.Find(_T("+")))));
						strTemp = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("+")) - 1);
					}
					if (!strTemp.IsEmpty())
					{
						vIntSubKind.push_back(_ttoi(strTemp));
					}
					CString strSqlAdd = _T("update ") + strTableName + _T(" set ");
					bool bNeedAddSql = false;
					for (size_t in = 0; in < vIntSubKind.size(); in++)
					{
						CString strFmSavePath, strDataSavePath;

						//根据配置文件 找到 根目录 按照层级保存

						switch (vIntSubKind[in])//1 修改 strCname 2 修改 strFmDurl 3 修改 StrDataDUrl 5修改 otherpage
						{
						case 1:
							if (bNeedAddSql)
							{
								strSqlAdd += _T(",");
							}
							strTemp = MustNeedItem.StrCname;
							if (strTemp.Find(_T("'"))>0)
							{
								strTemp.Replace(_T("'"), _T("''"));
							}

							strSqlAdd += _T("c_name='") + strTemp + _T("' ");
							bNeedAddSql = true;
							break;
						case 2:
							strFmSavePath = theApp.m_strTotalFmPath + _T("\\") + strAppendSavePath + _T("\\") + MustNeedItem.StrID;
							if (strGetFmPathFromTable.ReverseFind('.')>0)
							{
								strTemp = strGetFmPathFromTable.Left(strGetFmPathFromTable.ReverseFind('.'));
								if (strFmSavePath == strTemp)
								{
									strFmSavePath += _T("_1");
								}
							}

							if (!theApp.DownLoadData(MustNeedItem.StrFmDUrl, strFmSavePath, 1)) return false;

							if (bNeedAddSql)
							{
								strSqlAdd += _T(",");
							}

							strSqlAdd += _T("FmPath='") + strFmSavePath + _T("' ");
							bNeedAddSql = true;

							if (!strGetFmPathFromTable.IsEmpty() && PathFileExists(strGetFmPathFromTable))
							{
								strSqlAdd = _T("insert into 'NEEDDELETE' values('1','") + strGetFmPathFromTable + _T("')");
								strOperSqlArray.Add(strSqlAdd);
							}

							break;
						case 3:
							strDataSavePath = theApp.m_strTotalDataPath + _T("\\") + strAppendSavePath + _T("\\") + MustNeedItem.StrID;

							if (strFmSavePath == strGetDataPathFromTable)
							{
								strFmSavePath += _T("_1");
							}
							if (!theApp.DownLoadData(MustNeedItem.StrDataDUrl, strDataSavePath, 2)) return false;
							if (bNeedAddSql)
							{
								strSqlAdd += _T(",");
							}
							strSqlAdd += _T("DataPath='") + strDataSavePath + _T("' ");
							bNeedAddSql = true;

							if (!strGetDataPathFromTable.IsEmpty() && PathFileExists(strGetDataPathFromTable))
							{
								strSqlAdd = _T("insert into 'NEEDDELETE' values('1','") + strGetDataPathFromTable + _T("')");
								strOperSqlArray.Add(strSqlAdd);
							}

							break;
							//case 4:
							//	break;
						case 5:

							if (bNeedAddSql)
							{
								strSqlAdd += _T(",");
							}
							strSqlAdd += _T("append1info='") + MustNeedItem.StrOtherPage + _T("' ");
							bNeedAddSql = true;
							break;
							//case 6:
							//	break;
						default:
							break;

						}
					}
					if (bNeedAddSql)
					{
						strSqlAdd += _T(" where id = '") + MustNeedItem.StrID + _T("'");
						strOperSqlArray.Add(strSqlAdd);
					}
					else if (vIntSubKind.size() == 1 && vIntSubKind[0] == 6)//修改dir
					{
						CString strDirTabelName;//需要获取
						while (!theApp.LockRead(INFINITE, 10));
						for (size_t i = 0; i < 20; i++)
						{
							strSql.Format(_T("select count(*)  from 'DIR_%d' where id=\'%s\'"), i, string2CString(itemArr["List"][j]["StrSubSubId"].asString()));
							stmt = theApp.m_sqlite.Statement(strSql);
							if (stmt != NULL && !stmt->IsNullStmt())
							{
								if (stmt->NextRow() && _ttol(CString(stmt->ValueString(0)))>0)
								{
									delete stmt;
									stmt = NULL;
									strDirTabelName.Format(_T("DIR_%d"), i);
									break;
								}
								delete stmt;
								stmt = NULL;



							}

						}

						theApp.UnLockRead();

						if (strDirTabelName.IsEmpty())
						{
							return FALSE;

						}
						if (MustNeedItem.nSubSubIdKind == 2)//修改
						{
							CString strTitle = string2CString(itemArr["List"][j]["StrSubSubInfo"]["STitle"].asString());
							if (strTitle.Find(_T("'"))>0)
							{
								strTitle.Replace(_T("'"), _T("''"));
							}
							strSqlAdd = _T("update ") + strDirTabelName + _T(" set C_NAME='") + strTitle;
							strSqlAdd += _T("',pages='") + string2CString(itemArr["List"][j]["StrSubSubInfo"]["SPageNo"].asString()) + _T("' WHERE ID='") + string2CString(itemArr["List"][j]["StrSubSubId"].asString()) + _T("'");
							strOperSqlArray.Add(strSqlAdd);
						}
						else if (MustNeedItem.nSubSubIdKind == 3)//删除
						{
							strSqlAdd = _T("DELETE FROM ") + strDirTabelName + _T(" WHERE ID='") + string2CString(itemArr["List"][j]["StrSubSubId"].asString()) + _T("'");
							strOperSqlArray.Add(strSqlAdd);

						}

					}

					//加入 修正更新时间语句

					if (!MustNeedItem.StrDate.IsEmpty() && MustNeedItem.StrDate>strUpdateAndDelUpdateStanderTemp)
					{
						strUpdateAndDelUpdateStanderTemp = MustNeedItem.StrDate;
						strLastUpdateStander = strAddUpdateStanderTemp + _T("_") + strUpdateAndDelUpdateStanderTemp;

						strSqlAdd.Format(_T("update UPDATEINFO set LASTUPDATESTANDER='%s' where  ID=\'%s\'"), strLastUpdateStander, strQuestKind);
						strOperSqlArray.Add(strSqlAdd);
					}


				}

				break;
			case 3://操作类型是删除
				if (beExistOperatorItem)
				{
					//得到删除语句
					CString strSqlAdd = _T("update ") + strTableName + _T(" set Bevalid='0'  where id='") + MustNeedItem.StrID + _T("'");
					strOperSqlArray.Add(strSqlAdd);
					//删除目录
					CString strDirTabelName;
					while (!theApp.LockRead(INFINITE, 10));
					for (size_t i = 0; i < 20; i++)
					{
						strSql.Format(_T("select count(*)  from 'DIR_%d' where thname=\'%s\'"), i, MustNeedItem.StrID);
						stmt = theApp.m_sqlite.Statement(strSql);
						if (stmt != NULL && !stmt->IsNullStmt())
						{
							if (stmt->NextRow() && _ttol(CString(stmt->ValueString(0)))>0)
							{
								delete stmt;
								stmt = NULL;
								strDirTabelName.Format(_T("DIR_%d"), i);
								break;
							}
							delete stmt;
							stmt = NULL;



						}
					}

					theApp.UnLockRead();

					if (!strDirTabelName.IsEmpty())
					{
						strSqlAdd.Format(_T("delete FROM  \'%s\' where thname='%s'"), strDirTabelName, MustNeedItem.StrID);
						strOperSqlArray.Add(strSqlAdd);
					}



					//记录要删除的地址 写入NEEDDELETE
					strSqlAdd = _T("insert into 'NEEDDELETE' values('1','") + strGetFmPathFromTable + _T("')");
					strOperSqlArray.Add(strSqlAdd);
					strSqlAdd = _T("insert into 'NEEDDELETE' values('1','") + strGetDataPathFromTable + _T("')");
					strOperSqlArray.Add(strSqlAdd);

					//记录要删除的表属性
					strSqlAdd = _T("insert into 'NEEDDELETE' values('2','") + strTableName + _T("+") + MustNeedItem.StrID + _T("')");
					strOperSqlArray.Add(strSqlAdd);


					//加入 修正更新时间语句

					if (!MustNeedItem.StrDate.IsEmpty() && MustNeedItem.StrDate>strUpdateAndDelUpdateStanderTemp)
					{
						strUpdateAndDelUpdateStanderTemp = MustNeedItem.StrDate;
						strLastUpdateStander = strAddUpdateStanderTemp + _T("_") + strUpdateAndDelUpdateStanderTemp;

						strSqlAdd.Format(_T("update UPDATEINFO set LASTUPDATESTANDER='%s' where  ID=\'%s\'"), strLastUpdateStander, strQuestKind);
						strOperSqlArray.Add(strSqlAdd);
					}
				}

				break;
			default:
				break;
			}

			//执行表操作
			while(!theApp.LockWrite(-1, -1));

			theApp.m_sqlite.Begin();

			for (size_t i = 0; i < strOperSqlArray.GetSize(); i++)// 执行sql命令
			{
				if (!theApp.m_sqlite.DirectStatement(strOperSqlArray.GetAt(i)))
				{
					AfxMessageBox(_T("sqlite error!") + strOperSqlArray.GetAt(i));
				}
			}


			theApp.m_sqlite.Commit();
			theApp.UnLockWrite();


		}
	}

	return  true;
}

BOOL GetUpdateInfoAndData_Kind2_SubList(Json::Value itemArr, BOOL bFirstLevel, CString strQuestKind, CString strQuestSubKind, CString & strLastUpdateStander)
{

	struct SructMustNeed
	{
		CString StrInTableName;// 数据操作表名
		CString StrTypeId;// 
		CString StrID;//  标识,
		CString StrCname;// 标题,
		CString StrFmDUrl;//  //封面下载地址
		UINT nContentKind;// 1.url地址 2.html内容 3.pdf下载地址，4：子类的StrType值
		CString StrContent;   //内容（url地址/html内容/pdf下载地址/StrType模板）
		UINT nOpenContentKind;
		CString StrDate;//更新日期
		UINT nUpdateKind;// 1.增加 2.修改3.删除
		CString StrUpdateSubKind;//0 无 1修改StrCname 2 修改StrFmDUrl  3修改 nContentKind+StrContent+nOpenContentKind 4.修改在list中体现
	};

	struct STRUCTTABLEATRRIBUTE
	{
		CString strName;
		CString strValue;
	};
	CArray<STRUCTTABLEATRRIBUTE, STRUCTTABLEATRRIBUTE> structTableAtrriArray;
	CArray<STRUCTTABLEATRRIBUTE, STRUCTTABLEATRRIBUTE> structGetInfoAtrriArray;

	SructMustNeed MustNeedItem;

	Json::Value::Members getmember = itemArr.getMemberNames();
	CString  strTypeId;


	CStringArray strOperSqlArray;


	int iStartNo = 0;

	if (bFirstLevel)
	{
		iStartNo = 2;
	}

	for (size_t in = iStartNo; in < getmember.size(); in++)//记录所有得到的 属性及值
	{
		STRUCTTABLEATRRIBUTE item;
		item.strName = string2CString(getmember[in]);
		if (item.strName.CompareNoCase(_T("List")) == 0)
		{
			structGetInfoAtrriArray.Add(item);
			continue;
		}

		item.strValue = string2CString(itemArr[in].asString());
		structGetInfoAtrriArray.Add(item);

		//填充必选项
		if (item.strName.CompareNoCase(_T("StrInTableName")) == 0)
		{
			MustNeedItem.StrInTableName = item.strValue;
		}
		else if (item.strName.CompareNoCase(_T("StrTypeId")) == 0)
		{
			MustNeedItem.StrTypeId = item.strValue;
		}
		else if (item.strName.CompareNoCase(_T("StrID")) == 0)
		{
			MustNeedItem.StrID = item.strValue;
		}
		else if (item.strName.CompareNoCase(_T("StrCname")) == 0)
		{
			MustNeedItem.StrCname = item.strValue;
		}
		else if (item.strName.CompareNoCase(_T("StrFmDUrl")) == 0)
		{
			MustNeedItem.StrFmDUrl = item.strValue;
		}
		else if (item.strName.CompareNoCase(_T("nContentKind")) == 0)
		{
			MustNeedItem.nContentKind = _ttoi(item.strValue);
		}
		else if (item.strName.CompareNoCase(_T("StrContent")) == 0)
		{
			MustNeedItem.StrDate = item.strValue;
		}
		else if (item.strName.CompareNoCase(_T("StrDate")) == 0)
		{
			MustNeedItem.StrDate = item.strValue;
		}
		else if (item.strName.CompareNoCase(_T("nUpdateKind")) == 0)
		{
			MustNeedItem.nUpdateKind = _ttoi(item.strValue);
		}
		else if (item.strName.CompareNoCase(_T("StrUpdateSubKind")) == 0)
		{
			MustNeedItem.StrUpdateSubKind = item.strValue;
		}
		else if (item.strName.CompareNoCase(_T("nOpenContentKind")) == 0)
		{
			MustNeedItem.nOpenContentKind = _ttoi(item.strValue);
		}


	}

	int iTableLevel = -1;

	while (!theApp.LockRead(INFINITE, 10));
	//查询给定表是否存在
	BOOL bTabelExist = false;
	CString strSql;
	strSql.Format(_T("select count(*)  from sqlite_master where type = 'table' and name = '%s'"), MustNeedItem.StrInTableName);

	CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
	if (stmt != NULL && !stmt->IsNullStmt())
	{
		if (stmt->NextRow() && _ttoi(CString(stmt->ValueString(0))) > 0)
		{
			bTabelExist = TRUE;
		}

		delete stmt;
		stmt = NULL;

	}
	else
	{
		theApp.UnLockRead();
		AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
		return FALSE;
	}
	if (!bTabelExist)
	{
		theApp.UnLockRead();
		AfxMessageBox(_T("指定的表不存在！表名为：") + MustNeedItem.StrInTableName);
		return FALSE;
	}
	//查询所有属性
	strSql.Format(_T("select PRAGMA table_info([%s])"), MustNeedItem.StrInTableName);
	stmt = theApp.m_sqlite.Statement(strSql);
	if (stmt != NULL)
	{
		while (stmt->NextRow())
		{
			STRUCTTABLEATRRIBUTE item;
			item.strName = stmt->ValueString(1);
			structTableAtrriArray.Add(item);
		}

		delete stmt;
		stmt = NULL;

	}
	else
	{
		theApp.UnLockRead();
		AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
		return FALSE;
	}

	//查看表的等级
	CString strFindColumnName = _T("GGGPARENTID");
	for (int i = 4; i >= 1; i--)
	{
		strSql.Format(_T("select COUNT(*) from sqlite_master where name = '%s' and sql like '%%s%'"), MustNeedItem.StrInTableName, strFindColumnName);
		stmt = theApp.m_sqlite.Statement(strSql);
		if (stmt != NULL && !stmt->IsNullStmt())
		{
			if (stmt->NextRow() && _ttoi(CString(stmt->ValueString(0))) > 0)
			{
				iTableLevel = i;
			}

			delete stmt;
			stmt = NULL;
			if (iTableLevel > 0)
				break;
		}
		else
		{
			theApp.UnLockRead();
			AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
			return FALSE;
		}
		strFindColumnName = strFindColumnName.Right(strFindColumnName.GetLength() - 1);
	}



	if (iTableLevel < 0)
	{
		theApp.UnLockRead();
		return false;
	}

	//查看指定id是否在表内存在  +  已有数据存放路径
	bool beExistOperatorItem = false;
	CString strGetFmPathFromTable, strGetDataPathFromTable;

	strSql.Format(_T("select count(*),FmPath,DataPath  from '%s' where id = '%s'  and  BeValid='1'"), MustNeedItem.StrInTableName, MustNeedItem.StrID);
	stmt = theApp.m_sqlite.Statement(strSql);
	if (stmt != NULL && !stmt->IsNullStmt())
	{
		if (stmt->NextRow() && _ttoi(CString(stmt->ValueString(0))) > 0)
		{
			beExistOperatorItem = TRUE;

			strGetFmPathFromTable = stmt->ValueString(1);
			strGetDataPathFromTable = stmt->ValueString(2);
		}
		delete stmt;
		stmt = NULL;

	}
	else
	{
		theApp.UnLockRead();
		AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
		return FALSE;
	}


	CString strAppendSavePath;//按照层级存储的层级

	//得到所有父类
	CString strGGGParent, strGGParent, strGParent, strParent;

	CString strFindId;
	if (MustNeedItem.nUpdateKind == 1)
	{
		strFindId = MustNeedItem.StrTypeId;

	}
	else
	{
		strFindId = MustNeedItem.StrID;
	}

	if (iTableLevel == 4)
	{
		strSql.Format(_T("select GGGParentID,GGParentID,GParentID,ParentID from '%s' where id = '%s'  and  BeValid='1'"), MustNeedItem.StrInTableName, strFindId);
	}
	else if (iTableLevel == 3)
	{
		strSql.Format(_T("select GGParentID,GParentID,ParentID from '%s' where id = '%s'  and  BeValid='1'"), MustNeedItem.StrInTableName, strFindId);
	}
	else if (iTableLevel == 2)
	{
		strSql.Format(_T("select GParentID,ParentID from '%s' where id = '%s'  and  BeValid='1'"), MustNeedItem.StrInTableName, strFindId);
	}
	else if (iTableLevel == 1)
	{
		strSql.Format(_T("select ParentID from '%s' where id = '%s'  and  BeValid='1'"), MustNeedItem.StrInTableName, MustNeedItem.StrID);
	}
	else
	{
		theApp.UnLockRead();
		return false;
	}

	stmt = theApp.m_sqlite.Statement(strSql);
	if (stmt != NULL && !stmt->IsNullStmt())
	{
		if (stmt->IsNullStmt() && _ttoi(CString(stmt->ValueString(0))) > 0)
		{
			if (iTableLevel == 4)
			{
				strGGGParent = stmt->ValueString(0);
				strGGParent = stmt->ValueString(1);
				strGParent = stmt->ValueString(2);
				strParent = stmt->ValueString(3);
				strAppendSavePath = strGGGParent + _T("\\") + strGGParent + _T("\\") + strGParent + _T("\\") + strParent;
			}
			else if (iTableLevel == 3)
			{
				strGGParent = stmt->ValueString(0);
				strGParent = stmt->ValueString(1);
				strParent = stmt->ValueString(2);
				strAppendSavePath = strGGParent + _T("\\") + strGParent + _T("\\") + strParent;
			}
			else if (iTableLevel == 2)
			{
				strGParent = stmt->ValueString(0);
				strParent = stmt->ValueString(1);
				strAppendSavePath = strGParent + _T("\\") + strParent;
			}
			else if (iTableLevel == 1)
			{
				strParent = stmt->ValueString(0);
				strAppendSavePath = strParent;
			}

		}
		delete stmt;
		stmt = NULL;

	}
	else
	{
		theApp.UnLockRead();
		AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
		return FALSE;
	}



	theApp.UnLockRead();


	//下载数据
	switch (MustNeedItem.nUpdateKind)
	{
	case 1://操作类型是 增加
		if (!beExistOperatorItem)
		{
			//下载 fm
			CString strSaveFmPath = theApp.m_strTotalFmPath + _T("\\") + strAppendSavePath + _T("\\") + MustNeedItem.StrID;

			if (!MustNeedItem.StrFmDUrl.IsEmpty())//！！！！！存在封面下载链接为空 封面不能下载的情况
			{
				if (!theApp.DownLoadData(MustNeedItem.StrFmDUrl, strSaveFmPath, 1)) return false;

				for (size_t ij = 0; ij < structGetInfoAtrriArray.GetSize(); ij++)
				{
					if (structGetInfoAtrriArray.GetAt(ij).strName.CompareNoCase(_T("StrFmDUrl")) == 0)
					{
						structGetInfoAtrriArray.GetAt(ij).strValue = strSaveFmPath;
						break;
					}

				}

			}


			// 下载 data
			CString strSaveDataPath = theApp.m_strTotalDataPath + _T("\\") + strAppendSavePath + _T("\\") + MustNeedItem.StrID;

			switch (MustNeedItem.nContentKind)
			{
			case 2://html内容

				strSaveDataPath += _T(".html");
				if (!InserterHtml(theApp.m_strCanChangePath + _T("\\html\\emptyforchange.html"), strSaveDataPath, MustNeedItem.StrContent))
				{
					return false;
				}
				break;
			case 3://pdf下载地址
				if (!theApp.DownLoadData(MustNeedItem.StrContent, strSaveDataPath, 1)) return false;

				for (size_t ij = 0; ij < structGetInfoAtrriArray.GetSize(); ij++)
				{
					if (structGetInfoAtrriArray.GetAt(ij).strName.CompareNoCase(_T("StrContent")) == 0)
					{
						structGetInfoAtrriArray.GetAt(ij).strValue = strSaveDataPath;
						break;
					}
				}
				break;
			case 4://子类的StrType值
				if (MustNeedItem.StrContent.Find(_T("+"))>0)
				{
					CString strTemp = MustNeedItem.StrContent;
					CString strTableName = strTemp.Left(strTemp.Find(_T("+")));
					CString strInsertValue = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("+")) - 1);
					CString strSqlAdd;

					BOOL bExist = FALSE;
					if (!BeExistInTable(strInsertValue.Mid(1, strInsertValue.Find(_T("',")) - 1), strTableName, bExist)) return FALSE;
					if (!bExist)
					{
						while(!theApp.LockWrite(-1, -1));
						theApp.m_sqlite.Begin();
						strSqlAdd.Format(_T("insert into '%s' values(%s)"), strTableName, strInsertValue);
						theApp.m_sqlite.DirectStatement(strSqlAdd);
						theApp.m_sqlite.Commit();
						theApp.UnLockWrite();
					}


				}
				break;
			default:
				break;
			}

			if (_ttoi(MustNeedItem.StrUpdateSubKind) == 4)
			{
				for (size_t i = 0; i < itemArr["List"].size(); i++)
				{
					//	GetUpdateInfoAndData_Kind2_SubList(itemArr["List"][i], false, strQuestKind,strLastUpdateStander);
				}
			}


			//得到type

			while (!theApp.LockRead(INFINITE, 10));
			//通过给定表 查询Type;
			strSql.Format(_T("select  *  from '%s' where ID = '%s' and  BeValid='1'"), MustNeedItem.StrInTableName, MustNeedItem.StrTypeId);

			CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
			if (stmt != NULL && !stmt->IsNullStmt())
			{
				if (stmt->NextRow())
				{
					CString strTemp;
					for (size_t ij = 0; ij < structTableAtrriArray.GetSize(); ij++)
					{
						strTemp = stmt->ValueString(ij);
						if (ij == 0)
						{
							structTableAtrriArray.GetAt(ij).strValue = MustNeedItem.StrID;
						}
						else if (strTemp.GetLength()>5 && (strTemp.Left(2) == _T("%d") || strTemp.Left(2) == _T("%s") || strTemp.Left(3) == _T("%ld")))
						{
							CString strFindId = strTemp.Mid(strTemp.Find(_T("_")) + 1, strTemp.ReverseFind('_') - strTemp.Find(_T("_")));
							for (size_t in = 0; in < structGetInfoAtrriArray.GetSize(); in++)
							{
								if (structGetInfoAtrriArray.GetAt(in).strName.CompareNoCase(strFindId) == 0)
								{
									structTableAtrriArray.GetAt(ij).strValue = structGetInfoAtrriArray.GetAt(in).strValue;
									break;
								}
							}
						}
						else
						{
							structTableAtrriArray.GetAt(ij).strValue = strTemp;
						}



					}
				}

				delete stmt;
				stmt = NULL;

			}
			else
			{
				theApp.UnLockRead();
				AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
				return FALSE;
			}

			theApp.UnLockRead();

			//根据type 得到真正的插入语句

			BOOL bExist = FALSE;
			if (!BeExistInTable(MustNeedItem.StrID, MustNeedItem.StrInTableName, bExist)) return FALSE;
			if (!bExist)
			{
				CString strSqlAdd = _T("replace into ") + MustNeedItem.StrInTableName + _T(" values(");
				CString strTemp;
				for (size_t ij = 0; ij < structTableAtrriArray.GetSize(); ij++)
				{
					strTemp = structTableAtrriArray.GetAt(ij).strValue;
					if (strTemp.Find(_T("'"))>0)
					{
						strTemp.Replace(_T("'"), _T("''"));
					}
					strSqlAdd += _T(" '") + strTemp + _T("' ");

					if (ij + 1 != structTableAtrriArray.GetSize())
					{
						strSqlAdd += _T(",");
					}
					else
					{
						strSqlAdd += _T(")");
					}
				}

				strOperSqlArray.Add(strSqlAdd);
			}


		}
		break;

	case 2://操作类型是修改
		if (beExistOperatorItem)
		{
			vector<int> vIntSubKind;
			CString strTemp = MustNeedItem.StrUpdateSubKind;
			while (strTemp.Find(_T("+"))>0)
			{
				vIntSubKind.push_back(_ttoi(strTemp.Left(strTemp.Find(_T("+")))));
				strTemp = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("+")) - 1);
			}
			if (!strTemp.IsEmpty())
			{
				vIntSubKind.push_back(_ttoi(strTemp));
			}
			CString strSqlAdd = _T("update ") + MustNeedItem.StrInTableName + _T(" set ");
			bool bNeedAddSql = false;
			for (size_t in = 0; in < vIntSubKind.size(); in++)
			{
				CString strFmSavePath, strDataSavePath;

				//根据配置文件 找到 根目录 按照层级保存

				switch (vIntSubKind[in])//0 无 1修改StrCname 2 修改StrFmDUrl  3修改 nContentKind+StrContent+nOpenContentKind 4.修改在list中体现
				{
				case 1:
					if (bNeedAddSql)
					{
						strSqlAdd += _T(",");
					}
					strTemp = MustNeedItem.StrCname;
					if (strTemp.Find(_T("'"))>0)
					{
						strTemp.Replace(_T("'"), _T("''"));
					}
					strSqlAdd += _T("c_name='") + strTemp + _T("' ");
					bNeedAddSql = true;
					break;
				case 2:
					strFmSavePath = theApp.m_strTotalFmPath + _T("\\") + strAppendSavePath + _T("\\") + MustNeedItem.StrID;
					if (strGetFmPathFromTable.ReverseFind('.') > 0)
					{
						strTemp = strGetFmPathFromTable.Left(strGetFmPathFromTable.ReverseFind('.'));
						if (strFmSavePath == strTemp)
						{
							strFmSavePath += _T("_1");
						}
					}

					if (!theApp.DownLoadData(MustNeedItem.StrFmDUrl, strFmSavePath, 1)) return false;
					if (bNeedAddSql)
					{
						strSqlAdd += _T(",");
					}
					strSqlAdd += _T("FmPath='") + strFmSavePath + _T("' ");
					bNeedAddSql = true;

					if (!strGetFmPathFromTable.IsEmpty() && PathFileExists(strGetFmPathFromTable))
					{
						strSqlAdd = _T("insert into 'NEEDDELETE' values('1','") + strGetFmPathFromTable + _T("')");
						strOperSqlArray.Add(strSqlAdd);
					}

					break;
				case 3:
					strDataSavePath = theApp.m_strTotalDataPath + _T("\\") + strAppendSavePath + _T("\\") + MustNeedItem.StrID;
					if (strGetDataPathFromTable.ReverseFind('.') > 0)
					{
						CString strTemp = strGetDataPathFromTable.Left(strGetDataPathFromTable.ReverseFind('.'));
						if (strDataSavePath == strTemp)
						{
							strFmSavePath += _T("_1");
						}
					}

					switch (MustNeedItem.nContentKind)
					{
					case 1:
						if (bNeedAddSql)
						{
							strSqlAdd += _T(",");
						}
						strSqlAdd += _T("DataPath='") + MustNeedItem.StrContent + _T("' ");
						strSqlAdd.AppendFormat(_T("OpenItemKind='%d' "), MustNeedItem.nOpenContentKind);
						bNeedAddSql = true;
						break;
					case 2://html内容
					case 3://pdf下载地址
						if (MustNeedItem.nContentKind == 2)
						{
							strDataSavePath += _T(".html");
							if (!InserterHtml(theApp.m_strCanChangePath + _T("\\html\\emptyforchange.html"), strDataSavePath, MustNeedItem.StrContent))
							{
								continue;
							}
						}

						else
						{
							if (!theApp.DownLoadData(MustNeedItem.StrContent, strDataSavePath, 1)) return false;
						}

						if (bNeedAddSql)
						{
							strSqlAdd += _T(",");
						}
						strSqlAdd += _T("DataPath='") + strDataSavePath + _T("' ");
						strSqlAdd.AppendFormat(_T("OpenItemKind='%d' "), MustNeedItem.nOpenContentKind);
						bNeedAddSql = true;

						if (!strGetDataPathFromTable.IsEmpty() && PathFileExists(strGetDataPathFromTable))
						{
							strSqlAdd = _T("insert into 'NEEDDELETE' values('1','") + strGetDataPathFromTable + _T("')");
							strOperSqlArray.Add(strSqlAdd);
						}
						break;
					case 4://在下一级表中插入StrType值
						//if (MustNeedItem.StrContent.Find(_T("+"))>0)
						//{
						//	CString strTemp = MustNeedItem.StrContent;
						//	CString strTableName = strTemp.Left(strTemp.Find(_T("+")));
						//	CString strInsertValue = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("+"))-1);

						//	strSqlAdd.Format(_T("insert into '%s' values(%s)"), strTableName, strInsertValue);
						//	strOperSqlArray.Add(strSqlAdd);

						//}


						break;
					default:
						break;
					}


					break;

				case 4:
					for (size_t i = 0; i < itemArr["List"].size(); i++)
					{
						//	GetUpdateInfoAndData_Kind2_SubList(itemArr["List"][i], false, strQuestKind, strLastUpdateStander);
					}

					break;
				default:
					break;

				}
			}
			if (bNeedAddSql)
			{
				strSqlAdd += _T(" where id = '") + MustNeedItem.StrID + _T("'");
				strOperSqlArray.Add(strSqlAdd);
			}


		}

		break;
	case 3://操作类型是删除
		if (beExistOperatorItem)
		{
			//得到删除语句
			CString strSqlAdd = _T("update ") + MustNeedItem.StrInTableName + _T(" set Bevalid='0'  where id='") + MustNeedItem.StrID + _T("'");
			strOperSqlArray.Add(strSqlAdd);
			//记录要删除的地址 写入NEEDDELETE
			strSqlAdd = _T("insert into 'NEEDDELETE' values('1','") + strGetFmPathFromTable + _T("')");
			strOperSqlArray.Add(strSqlAdd);
			strSqlAdd = _T("insert into 'NEEDDELETE' values('1','") + strGetDataPathFromTable + _T("')");
			strOperSqlArray.Add(strSqlAdd);

			//记录要删除的表属性
			strSqlAdd = _T("insert into 'NEEDDELETE' values('2','") + MustNeedItem.StrInTableName + _T("+") + MustNeedItem.StrID + _T("')");
			strOperSqlArray.Add(strSqlAdd);
		}

		break;
	default:
		break;
	}



	//加入 修正更新时间语句

	if (!MustNeedItem.StrDate.IsEmpty() && MustNeedItem.StrDate>strLastUpdateStander)
	{
		CString strSqlAdd;
		strLastUpdateStander = MustNeedItem.StrDate;
		strSqlAdd.Format(_T("update UPDATEINFO set LASTUPDATESTANDER='%s' where  ID=\'%s\'"), strLastUpdateStander, strQuestKind);
		strOperSqlArray.Add(strSqlAdd);
	}


	//执行表操作
	while(!theApp.LockWrite(-1, -1));

	theApp.m_sqlite.Begin();

	for (size_t i = 0; i < strOperSqlArray.GetSize(); i++)// 执行sql命令
	{
		theApp.m_sqlite.DirectStatement(strOperSqlArray.GetAt(i));
	}


	theApp.m_sqlite.Commit();
	theApp.UnLockWrite();

	if (!MustNeedItem.StrDate.IsEmpty() && MustNeedItem.StrDate>strLastUpdateStander)
	{
		strLastUpdateStander = MustNeedItem.StrDate;
	}

	return  true;

}

BOOL GetUpdateInfoAndData_Kind2(Json::Value itemArr, CString strQuestKind, CString strQuestSubKind, CString & strLastUpdateStander)//非期刊类下载更新(非单一请求值)
{
	if (string2CString(itemArr["BSuccess"].asString()).CompareNoCase(_T("true")) == 0)
	{
		//int iListLevel = _ttoi(string2CString(itemArr["nListLevel"].asString()));
		//		GetUpdateInfoAndData_Kind2_SubList(itemArr, TRUE, strQuestKind, strLastUpdateStander);
	}
	return  true;
}

BOOL GetUpdateInfoAndData_Kind3(Json::Value itemArr, CString strQuestKind, CString strQuestSubKind, CString & strLastUpdateStander)// ????需要增加一个更新时间？？？？
{
	if (string2CString(itemArr["BSuccess"].asString()).CompareNoCase(_T("true")) == 0)
	{
		//int iListLevel = _ttoi(string2CString(itemArr["nListLevel"].asString()));
		CString StrReturnInfo;//返回的信息（请求序列号和关机设置<1-06:00    0表示不关机，1：表示关机-时间>
		StrReturnInfo = string2CString(itemArr["StrReturnInfo"].asString());
		if (!StrReturnInfo.IsEmpty())
		{
			if (strQuestKind == _T("RegistNo"))
			{
				if (theApp.m_strRegestNo != StrReturnInfo)
				{
					CString strIniPath = _T("\\xml\\serialno.ini");

					strIniPath = theApp.m_strCanChangePath + strIniPath;
					theApp.m_strRegestNo = StrReturnInfo;
					::WritePrivateProfileStringW(_T("SerialNo"), _T("No"), StrReturnInfo, strIniPath);
				}
			}
			else if (strQuestKind == _T("ShutdownSet"))
			{
				bool bNeedColse = StrReturnInfo.Left(1) == 1 ? true : false;
				CString strSetTime = StrReturnInfo.Right(StrReturnInfo.GetLength() - StrReturnInfo.Find(_T("-")) - 1);

				if (theApp.m_bAllowClose != bNeedColse || strSetTime != theApp.m_strCloseTime)
				{
					//更新关机时间 xml
					theApp.UpdateXmlAboutCloseData(bNeedColse, strSetTime);
				}

			}
		}

	}
	return  true;
}


BOOL tableExist(CString strTableName)
{
	CSqlStatement *stmt;
	CString strSqlAlter, strSqlQueryItem;
	BOOL bExist = FALSE;

	while (!theApp.LockRead(INFINITE, 10));
R
	strSqlQueryItem.Format(_T("select COUNT(*) from sqlite_master where type = 'table' and name = '%s';"), strTableName);
	stmt = theApp.m_sqlite.Statement(strSqlQueryItem);

	if (stmt != NULL && !stmt->IsNullStmt()) {
		if (stmt->NextRow() && _ttoi(CString(stmt->ValueString(0))) > 0) {
			bExist = TRUE;
		}
		delete stmt;
		stmt = NULL;
	}
	else {
		theApp.UnLockRead();
ER
		Sleep(10);
		AfxMessageBox(_T("Select Sqlite  Error\n") + strSqlQueryItem);
		return FALSE;
	}
	theApp.UnLockRead();
ER
	Sleep(10);
	return bExist;
}

BOOL columnIsInTable(CString strColumn, CString strTable, BOOL& bExist)
{
	CSqlStatement *stmt;
	CString strSqlAlter, strSqlQueryItem;

	while (!theApp.LockRead(INFINITE, 10));
R
	strSqlQueryItem.Format(_T("select COUNT(*) from sqlite_master where type = 'table' and name = '%s' and sql like '%%%s%%';"), strTable, strColumn);
	stmt = theApp.m_sqlite.Statement(strSqlQueryItem);

	if (stmt != NULL && !stmt->IsNullStmt()) {
		if (stmt->NextRow() && _ttoi(CString(stmt->ValueString(0))) > 0) {
			bExist = TRUE;
		}
		delete stmt;
		stmt = NULL;
	}
	else {
		theApp.UnLockRead();
ER
		Sleep(10);
		AfxMessageBox(_T("Select Sqlite  Error\n") + strSqlQueryItem);
		return FALSE;
	}
	theApp.UnLockRead();
ER
	Sleep(10);
	return bExist;
}

// get token at the first ,Monitor.cpp and MonitorDlg.cpp use
string token;

BOOL getNewToken(char *buf)
{
	CString strRequestUrlHead = theApp.m_strCnkiServerId;
	CString strRequestUrl, strResult, strContent;
	UPDATEINFOITEM updateIfItem = theApp.m_updateinfoArray.GetAt(0);
	strRequestUrl = strRequestUrlHead + updateIfItem.strDownloadUrl;

	strContent.Format(_T("{\"serialNo\":\"%s\"}"), theApp.m_strRegestNo);
	int len = WideCharToMultiByte(CP_ACP, 0, strContent, strContent.GetLength(), NULL, 0, NULL, NULL);
	char * content = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, strContent, strContent.GetLength(), content, len, NULL, NULL);
	content[len] = '\0';

	Json::Reader reader;
	Json::Value itemArr;

	CString2Char(strResult, buf);

	// "token":"....."
	if (reader.parse(buf, itemArr, FALSE)) {
		token = "token:" + itemArr["data"]["token"].asString();
	}
	return TRUE;
}

BOOL dealwithXWDT(Json::Value &itemArr, UPDATEINFOITEM &updateIfItem)
{
	CString strUpdateTableName = _T("CHANGEDINFO");
	CString strAddTableName = theApp.m_updateinfoArray[2].tableInfo;
	CString RandomID, ParentID;
	CString levelStruct;
	CString m_strDateTime;
	CTime m_time;
	CString strRequestUrlHead = theApp.m_strCnkiServerId;
	CString resourceHeader = strRequestUrlHead + theApp.m_strCnkiServerPath;
	UINT nRequestSubKind = 1;
	UINT index;
	UINT prio = updateIfItem.nPriority;
	UINT orderNo = updateIfItem.nOrderNo;
	ULONGLONG llDownPart = 0;

	CSqlStatement *stmt = NULL;
	CString strAddItem, strUpdateItem, strDelItem, strUpdateCHANGEDItem, strSqlUpdateUPDATEINFO;
	CStringArray strSqlAdd, strSqlUpdate, strSqlDel, strSqlUpdateCHANGEDINFO;	
	if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
		int times = itemArr["data"]["total"].asInt();

		//展览公告
		if (updateIfItem.nRequestSubKind == 1) {
			ParentID = _T("ZLGG");
			nRequestSubKind = 1;
			index = 3;
		}
		else if (updateIfItem.nRequestSubKind == 2) {
			ParentID = _T("BGKX");
			nRequestSubKind = 2;
			index = 4;
		}
		else
			return TRUE;

		CString fileName, filePath, filePathHead = theApp.totalItem[index].FmPath;
		CString urlBody, url;
		CString moreInfo;
		int slashSeparator = 0;	
		for (int i = 0; i < ROWS && i < times; i++)
		{				CString tmpp;tmpp.Format(_T("%s"), itemArr["data"]["items"][i]["contentHtml"]);
			//展览公告	
			RandomID.Format(_T("%s%.3d"), ParentID, itemArr["data"]["items"][i]["id"]);
			//ADD
			if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("add")) == 0) {
				urlBody = string2CString(itemArr["data"]["items"][i]["item"]["cover"].asString());
				if (!urlBody.IsEmpty()) {
					slashSeparator = urlBody.ReverseFind('/') + 1;
					fileName = urlBody.Mid(slashSeparator);
					filePath = filePathHead + fileName;
					url = resourceHeader + urlBody;
					
					theApp.DownLoadDataNOIDC(url, filePath, 2);					

					CString exTime = string2CString(itemArr["data"]["items"][i]["item"]["exTime"].asString());
					CString exLocation = string2CString(itemArr["data"]["items"][i]["item"]["exLocation"].asString());
					CString exType = string2CString(itemArr["data"]["items"][i]["item"]["exType"].asString());
					moreInfo = _T("&展览时间：") + exTime + _T("&展览地点：") + exLocation + _T("&展览类型：") + exType + _T("&");
				}

				strAddItem.Format(_T("INSERT OR REPLACE INTO %s(ID, C_NAME, Orgernization, ShowDate, ParentID, GParentID, GGParentID,")
								  _T(" FmKind, FmBkPath, FmPath, ShowInLevel, OpenItemKind, OpenItemShowKind, DirGetKind, DirInTable,")
								  _T(" DirShowKind, MoreGetKind, MoreShowKind, DataGetKind, DataKind, DataPath, SubItemExist, SubItemInfoTable, ")
								  _T("SubItemIsShowInList, nSubItemShowKind, SubItemListLevel, SubItemListPageChangeKind, ShowEwmKind, OrderNo,")
								  _T(" IsCanFind, IsCanLarge, InfoKind, appendkind, ZanExist, BeTemplate, BeShowInHomePage, ItemIntroduce,")
								  _T(" ItemMoreInfo, NextWinowShowInRect, itemID, BeValid) VALUES")
								  _T("('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', ")
								  _T("'%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s', '%s', %d, '%s', %d, '%s', '%s', '%s', %d, %d, ")
								  _T("'%s', '%s', '%s', '%d', %d);"),
									 strAddTableName,
									 RandomID,
									 string2CString(itemArr["data"]["items"][i]["item"]["title"].asString()),
									 string2CString(itemArr["data"]["items"][i]["item"]["organCode"].asString()),
									 string2CString(itemArr["data"]["items"][i]["item"]["time"].asString()).Left(10),
									 ParentID,
									 _T("XWDT"),
									 _T("BYGYBG"),
									 theApp.totalItem[index].FmKind,
									 theApp.totalItem[index].FmBkPath,
									 filePath,
									 theApp.totalItem[index].ShowInLevel,
									 theApp.totalItem[index].OpenItemKind,
									 theApp.totalItem[index].OpenItemShowKind,
									 theApp.totalItem[index].DirGetKind,
									 theApp.totalItem[index].DirInTable,
									 theApp.totalItem[index].DirShowKind,
									 theApp.totalItem[index].MoreGetKind,
									 theApp.totalItem[index].MoreShowKind,
									 theApp.totalItem[index].DataGetKind,
									 theApp.totalItem[index].DataKind,
									 theApp.totalItem[index].DataPath,
									 theApp.totalItem[index].SubItemExist,
									 theApp.totalItem[index].SubItemInfoTable,
									 theApp.totalItem[index].SubItemIsShowInList,
									 theApp.totalItem[index].nSubItemShowKind,
									 theApp.totalItem[index].SubItemListLevel,
									 theApp.totalItem[index].SubItemListPageChangeKind,
									 theApp.totalItem[index].ShowEwmKind,
									 itemArr["data"]["items"][i]["item"]["sort"].asInt(),
									 theApp.totalItem[index].IsCanFind,
									 theApp.totalItem[index].IsCanLarge,
									 theApp.totalItem[index].InfoKind,
									 theApp.totalItem[index].appendkind,
									 theApp.totalItem[index].ZanExist,
									 0,
									 theApp.totalItem[index].BeShowInHomePage,
									 string2CString(itemArr["data"]["items"][i]["item"]["content"].asString()),
									 string2CString(itemArr["data"]["items"][i]["item"]["synopsis"].asString()) + moreInfo,
									 theApp.totalItem[index].NextWinowShowInRect,
									 itemArr["data"]["items"][i]["id"].asInt(),
									 1
									 );
				strSqlAdd.Add(strAddItem);
			}
			//UPDATE
			else if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("update")) == 0) {
				urlBody = string2CString(itemArr["data"]["items"][i]["item"]["cover"].asString());
				if (!urlBody.IsEmpty()) {
					slashSeparator = urlBody.ReverseFind('/') + 1;
					fileName = urlBody.Mid(slashSeparator);
					filePath = filePathHead + fileName;
					url = resourceHeader + urlBody;
					theApp.DownLoadDataNOIDC(url, filePath, 2);	

					CString exTime = string2CString(itemArr["data"]["items"][i]["item"]["exTime"].asString());
					CString exLocation = string2CString(itemArr["data"]["items"][i]["item"]["exLocation"].asString());
					CString exType = string2CString(itemArr["data"]["items"][i]["item"]["exType"].asString());
					moreInfo = _T("&展览时间：") + exTime + _T("&展览地点：") + exLocation + _T("&展览类型：") + exType + _T("&");
				}

				strUpdateItem.Format(_T("UPDATE %s SET ShowDate = '%s', C_NAME = '%s', FmPath = '%s',ItemIntroduce = '%s',")
									 _T(" ItemMoreInfo = '%s', OrderNo = %d WHERE itemID = %d and ParentID = '%s';"),
									strAddTableName,
									string2CString(itemArr["data"]["items"][i]["item"]["time"].asString()).Left(10),
									string2CString(itemArr["data"]["items"][i]["item"]["title"].asString()),
									filePath,
									string2CString(itemArr["data"]["items"][i]["item"]["content"].asString()),
									string2CString(itemArr["data"]["items"][i]["item"]["synopsis"].asString()) + moreInfo,
									itemArr["data"]["items"][i]["item"]["sort"].asInt(),
									itemArr["data"]["items"][i]["id"].asInt(),
									ParentID
									);
				strSqlUpdate.Add(strUpdateItem);
			}
			// DEL
			else if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("del")) == 0) {
				strDelItem.Format(_T("UPDATE %s SET BeValid = 0 WHERE itemID = %d and ParentID = '%s';"),
					strAddTableName,
					itemArr["data"]["items"][i]["id"].asInt(),
					ParentID);
				strSqlDel.Add(strDelItem);
			}

			//并且把add, update, del的记录存放在CHANGEDINFO更新表中
			m_time = CTime::GetCurrentTime();
			m_strDateTime = m_time.Format(_T("%Y-%m-%d %H:%M:%S"));
			levelStruct.Format(_T("BYGYBG/XWDT/%s/%s"), ParentID, RandomID);
			strUpdateCHANGEDItem.Format(_T("INSERT OR REPLACE INTO %s(levelStruct, priority, ")
										_T("updateDBTime, typeID, operate) VALUES('%s', %d, '%s', %d, '%s');"),
										strUpdateTableName,
										levelStruct,
										prio,
										m_strDateTime,
										itemArr["data"]["items"][i]["id"].asInt(),
										string2CString(itemArr["data"]["items"][i]["operate_type"].asString())
										);
			strSqlUpdateCHANGEDINFO.Add(strUpdateCHANGEDItem);
		}

		// 把最后一条记录的date和id更新到UPDATEINFO表中
		if (times > 0) {
			int lastInfoIndex = (times < ROWS) ? times - 1 : ROWS - 1;
			CString date = string2CString(itemArr["data"]["items"][lastInfoIndex]["time"].asString());
			int id = itemArr["data"]["items"][lastInfoIndex]["id"].asInt();
			CString strContent;
			strContent.Format(_T("{\"date\":\"%s\", \"page\":1,\"rows\":10, \"type\":\"%s\", \"id\":%d}"), date, ParentID, id);
			
			theApp.m_updateinfoArray[orderNo].strContent = strContent;	// 把最后一条消息记录标志更新到结构体中

			strSqlUpdateUPDATEINFO.Format(_T("UPDATE UPDATEINFO SET CONTENT = '%s' WHERE ID = '%s' AND REQUESTSUBKIND = %d;"),
											strContent,
											_T("XWDT"),
											nRequestSubKind
											);
			//updateIfItem.isNeedReadFromTable = TRUE;

			//执行add，update，del操作，并且更新UPDATEINFO，和CHANGEDINFO		
			int addCount = strSqlAdd.GetCount();
			int updateCount = strSqlUpdate.GetCount();
			int delCount = strSqlDel.GetCount();
			int updateCHANGEDINFOCount = strSqlUpdateCHANGEDINFO.GetCount();

			while(!theApp.LockWrite(-1, -1));
W0
			theApp.m_sqlite.Begin();
			for (int i = 0; i < addCount; i++) {
				stmt = theApp.m_sqlite.Statement(strSqlAdd.GetAt(i));
				if (stmt != NULL && !stmt->IsNullStmt()) {
					stmt->Execute();
					delete stmt;
				}
			}
			for (int i = 0; i < updateCount; i++) {
				stmt = theApp.m_sqlite.Statement(strSqlUpdate.GetAt(i));
				if (stmt != NULL && !stmt->IsNullStmt()) {
					stmt->Execute();
					delete stmt;
				}
			}
			for (int i = 0; i < delCount; i++) {
				stmt = theApp.m_sqlite.Statement(strSqlDel.GetAt(i));
				if (stmt != NULL && !stmt->IsNullStmt()) {
					stmt->Execute();
					delete stmt;
				}
			}
			for (int i = 0; i < updateCHANGEDINFOCount; i++) {
				stmt = theApp.m_sqlite.Statement(strSqlUpdateCHANGEDINFO.GetAt(i));
				if (stmt != NULL && !stmt->IsNullStmt()) {
					stmt->Execute();
					delete stmt;
				}
			}

			// 更新完一页后记录本次标志
			stmt = theApp.m_sqlite.Statement(strSqlUpdateUPDATEINFO);
			if (stmt != NULL && !stmt->IsNullStmt()) {
				stmt->Execute();
				delete stmt;
			}
			theApp.m_sqlite.Commit();

			theApp.UnLockWrite();
EW0
			Sleep(10);
		}
	}
	return TRUE;
}

BOOL dealwithOthers(Json::Value &itemArr, UPDATEINFOITEM &updateIfItem)
{
	CString strSqlUpdateUPDATEINFO;
	CString strAddItem, strUpdateItem, strDelItem, strSqlUpdateCHANGEDINFOItem;

	CSqlStatement *stmt = NULL;
	CString strUpdateTableName = _T("CHANGEDINFO");

	CString levelStruct;
	CString m_strDateTime;
	CTime m_time;
	UINT orderNo = updateIfItem.nOrderNo;
	UINT prio = updateIfItem.nPriority;
	UINT times;
	CString strRequestUrlHead = theApp.m_strCnkiServerId;
	CString resourceHeader = strRequestUrlHead + theApp.m_strCnkiServerPath;
	CString urlBody, url;
	BOOL bExist;

	switch (orderNo) {
	{
		case 4:	// 留言管理下的精品留言	（get的数据中有操作类型：增加,删除）
			CString strTableName = theApp.m_updateinfoArray[4].tableInfo;
			CStringArray strSqlAdd, strSqlDel, strSqlUpdateCHANGEDINFO;

			if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
				times = itemArr["data"]["total"].asInt();
				CString RandomID;

				for (int i = 0; i < ROWS && i < times; i++) {
					// ADD
					if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("add")) == 0) {
						// 暂时设置ID和itemID相同
						RandomID.Format(_T("JPLY%d"), itemArr["data"]["items"][i]["id"]);
						strAddItem.Format(_T("INSERT OR REPLACE INTO %s(ID, username, updatetime, content, ParentID, Valid, itemID)")
										_T(" VALUES('%s', '%s', '%s', '%s', '%s', %d, %d);"),
										strTableName,
										RandomID,
										string2CString(itemArr["data"]["items"][i]["item"]["name"].asString()),
										string2CString(itemArr["data"]["items"][i]["time"].asString()),
										string2CString(itemArr["data"]["items"][i]["item"]["message"].asString()),
										_T("BYWYLY"),
										1,
										itemArr["data"]["items"][i]["id"].asInt()
										);
						strSqlAdd.Add(strAddItem);
					}
					else if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("del")) == 0) {
						strDelItem.Format(_T("UPDATE %s SET Valid = 0 WHERE itemID = %d;"),
										strTableName, 
										itemArr["data"]["items"][i]["id"].asInt());
						strSqlDel.Add(strDelItem);
					}

					m_time = CTime::GetCurrentTime();
					m_strDateTime = m_time.Format(_T("%Y-%m-%d %H:%M:%S"));
					levelStruct.Format(_T("BYWYLY/JPLY"));
					strSqlUpdateCHANGEDINFOItem.Format(_T("INSERT OR IGNORE INTO %s(levelStruct, priority, updateDBTime, typeID, operate)")
														_T(" VALUES('%s', %d, '%s', %d, '%s');"), 
														strUpdateTableName,
														levelStruct,
														prio,
														m_strDateTime,
														itemArr["data"]["items"][i]["id"].asInt(),
														string2CString(itemArr["data"]["items"][i]["operate_type"].asString())
														);
					strSqlUpdateCHANGEDINFO.Add(strSqlUpdateCHANGEDINFOItem);
				}

				// 更新UPDATEINFO表
				if (times > 0) {
					//把最后一条信息的time写入到UPDATEINFO表中的content的时间参数中
					int lastInfoIndex = (times < ROWS) ? times - 1 : ROWS - 1;
					CString date = string2CString(itemArr["data"]["items"][lastInfoIndex]["time"].asString());
					int id = itemArr["data"]["items"][lastInfoIndex]["id"].asInt();
					CString strContent;
					strContent.Format(_T("{\"date\":\"%s\", \"page\":1,\"rows\":10, \"id\":%d}"), date, id);
					theApp.m_updateinfoArray[orderNo].strContent = strContent;

					// 每次把更新的最后一条记录在表中
					strSqlUpdateUPDATEINFO.Format(_T("UPDATE UPDATEINFO SET CONTENT = '%s' WHERE ID = 'JPLY'"), strContent);
					updateIfItem.isNeedReadFromTable = TRUE;

					// 执行add，del操作，并且更新UPDATEINFO，和CHANGEDINFO					
					int addCount = strSqlAdd.GetCount();
					int delCount = strSqlDel.GetCount();
					int updateCount = strSqlUpdateCHANGEDINFO.GetCount();

					while(!theApp.LockWrite(-1, -1));
W1
					theApp.m_sqlite.Begin();
					
					for (int i = 0; i < addCount; i++) {
						stmt = theApp.m_sqlite.Statement(strSqlAdd.GetAt(i));
						if (stmt != NULL && !stmt->IsNullStmt()) {
							stmt->Execute();
							delete stmt;
						}
					}
					for (int i = 0; i < delCount; i++) {
						stmt = theApp.m_sqlite.Statement(strSqlDel.GetAt(i));
						if (stmt != NULL && !stmt->IsNullStmt()) {
							stmt->Execute();
							delete stmt;
						}
					}
					for (int i = 0; i < updateCount; i++) {
						stmt = theApp.m_sqlite.Statement(strSqlUpdateCHANGEDINFO.GetAt(i));
						if (stmt != NULL && !stmt->IsNullStmt()) {
							stmt->Execute();
							delete stmt;
						}
					}

					stmt = theApp.m_sqlite.Statement(strSqlUpdateUPDATEINFO);
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}

					theApp.m_sqlite.Commit();
					theApp.UnLockWrite();
EW1
					Sleep(1000);
				}
			}
		break;
	}
	case 5:		// 留言管理下的扫码留言
		break;
	case 6:		// 留言管理下的上传留言
	{
		// 把本次上传完成的留言在表中删除，设置Valid = 0
		CString strSqlSelect, strSqlAlter;
		if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
			CString lastDeleteID = updateIfItem.strLastUpdateStander;

			while(!theApp.LockWrite(-1, -1));
W1
			theApp.m_sqlite.Begin();
			strSqlAlter.Format(_T("Update %s SET Valid = 0 WHERE ID = '%s';"), theApp.m_updateinfoArray[6].tableInfo, lastDeleteID);
			stmt = theApp.m_sqlite.Statement(strSqlAlter);
			if (stmt != NULL && !stmt->IsNullStmt()) {
				stmt->Execute();
				delete stmt;
			}
			theApp.m_sqlite.Commit();
			theApp.UnLockWrite();
EW1
			Sleep(1000);
		}
		break;
	}
	case 7:		//关于本馆下的本馆简介
	{
		CString strAddTableName = theApp.m_updateinfoArray[7].tableInfo;
		CString strSqlAdd, strSqlUpdate;
		CString strSqlUpdateCHANGEDINFO, strSqlUpdateUPDATEINFO;
		CString RandomID;
		CString strContent;
		CString strSqlSelect;
		bExist = false;
		if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
			// UPDATE
			if (string2CString(itemArr["data"]["operate_type"].asString()).CompareNoCase(_T("update")) == 0) {
				strSqlUpdate.Format(_T("UPDATE %s SET C_NAME = '%s', ItemIntroduce = '%s' WHERE ID = 'BGJJ001';"), 
									strAddTableName,
									string2CString(itemArr["data"]["item"]["title"].asString()),
									string2CString(itemArr["data"]["item"]["content"].asString()) 
									);
				while(!theApp.LockWrite(-1, -1));
W1
				theApp.m_sqlite.Begin();
				stmt = theApp.m_sqlite.Statement(strSqlUpdate);
				if (stmt != NULL && !stmt->IsNullStmt()) {
					stmt->Execute();
					delete stmt;
					stmt = NULL;
					theApp.m_sqlite.Commit();
					theApp.UnLockWrite();
EW1
					Sleep(1000);
				} else {
					//delete stmt;
					stmt = NULL;
					theApp.m_sqlite.Commit();
					theApp.UnLockWrite();
EW1
					Sleep(1000);
				}
			}
			else {
				// 本馆信息无需更新
				break;
			}

			// 把更新的东西写到CHANGEDINFO中
			m_time = CTime::GetCurrentTime();
			m_strDateTime = m_time.Format(_T("%Y-%m-%d %H:%M:%S"));
			levelStruct.Format(_T("BYGYBG/BGJJ"));
			strSqlUpdate = _T("");
			strSqlUpdateCHANGEDINFO.Format(_T("INSERT OR IGNORE INTO %s(levelStruct, priority, updateDBTime, typeID, operate)")
											_T(" VALUES('%s', %d, '%s', %d, '%s');"),
											strUpdateTableName,
											levelStruct,
											prio,
											m_strDateTime,
											itemArr["data"]["id"].asInt(),
											string2CString(itemArr["data"]["operate_type"].asString())
											);
			// 把本条记录的时间存放到结构中以便下一次post时作为date参数.
			strContent.Format(_T("{\"date\":\"%s\"}"), string2CString(itemArr["data"]["time"].asString()));
			theApp.m_updateinfoArray[orderNo].strContent = strContent;
			// 同时存放到表中
			strSqlUpdateUPDATEINFO.Format(_T("UPDATE UPDATEINFO SET CONTENT = '%s' WHERE ID = 'BGJJ';"), strContent);

			Sleep(1000);
			while(!theApp.LockWrite(-1, -1));
W1
			theApp.m_sqlite.Begin();

			stmt = theApp.m_sqlite.Statement(strSqlUpdateCHANGEDINFO);
			if (stmt != NULL && !stmt->IsNullStmt()) {
				stmt->Execute();
				delete stmt;
			}

			stmt = theApp.m_sqlite.Statement(strSqlUpdateUPDATEINFO);
			if (stmt != NULL && !stmt->IsNullStmt()) {
				stmt->Execute();
				delete stmt;
			}
			theApp.m_sqlite.Commit();
			theApp.UnLockWrite();
EW1
			Sleep(1000);
		}
		break;
	}
	case 8:		//关于本馆下的专家学者
	{
		CString strAddTableName = theApp.m_updateinfoArray[8].tableInfo;
		CString RandomID, ParentID = _T("ZJXZ"), GParentID = _T("BYGYBG");
		CStringArray strSqlAdd, strSqlUpdate, strSqlDel, strSqlUpdateCHANGEDINFO;
		int slashSeparator = 0, index = 0;
		CString fileName, filePath, filePathHead = theApp.totalItem[index].FmPath;
		CString url, urlBody;
		ULONGLONG llDownPart = 0;

		if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
			times = itemArr["data"]["total"].asInt();

			for (int i = 0; i < ROWS && i < times; i++)
			{
				// ADD
				if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("add")) == 0) {
					RandomID.Format(_T("ZJXZ%.3d"), itemArr["data"]["items"][i]["id"]);
					urlBody = string2CString(itemArr["data"]["items"][i]["item"]["image"].asString());
					if (!urlBody.IsEmpty()) {
						slashSeparator = urlBody.ReverseFind('/') + 1;
						fileName = urlBody.Mid(slashSeparator);
						filePath = filePathHead + fileName;
						url = resourceHeader + urlBody;
						theApp.DownLoadDataNOIDC(url, filePath, 2);	
						//theApp.RequestHttp(url, filePath, llDownPart);						
					}
					strAddItem.Format(_T("INSERT OR REPLACE INTO %s(ID, C_NAME, Orgernization, ShowDate, ParentID, GParentID, ")
										_T("FmKind, FmBkPath, FmPath, ShowInLevel, OpenItemKind, OpenItemShowKind, DirGetKind,")
										_T(" DirInTable, DirShowKind, MoreGetKind, MoreShowKind, DataGetKind, DataKind, DataPath,")
										_T(" SubItemExist, SubItemInfoTable, SubItemIsShowInList,nSubItemShowKind, SubItemListLevel,")
										_T(" SubItemListPageChangeKind,ShowEwmKind, OrderNo, IsCanFind, IsCanLarge, InfoKind, appendkind,")
										_T(" ZanExist, BeTemplate, NextWinowShowInRect, BeShowInHomePage, itemID, BeValid, ItemIntroduce, ItemMoreInfo) ")
										_T("VALUES('%s', '%s', '%s', '%s', '%s', '%s','%s', '%s', '%s', '%s', '%s', '%s','%s', '%s', '%s',")
										_T(" '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d,'%s', '%s', '%s', %d, '%s', %d,'%s', '%s',")
										_T(" '%s', %d, '%s', '%s', %d, %d, '%s', '%s');"),
										strAddTableName,
										RandomID,
										string2CString(itemArr["data"]["items"][i]["item"]["name"].asString()),
										string2CString(itemArr["data"]["items"][i]["item"]["organCode"].asString()),
										string2CString(itemArr["data"]["items"][i]["time"].asString()).Left(10),
										ParentID,
										GParentID,
										theApp.totalItem[index].FmKind,
										theApp.totalItem[index].FmBkPath,
										theApp.totalItem[index].FmPath + fileName,
										theApp.totalItem[index].ShowInLevel,
										theApp.totalItem[index].OpenItemKind,
										theApp.totalItem[index].OpenItemShowKind,
										theApp.totalItem[index].DirGetKind,
										theApp.totalItem[index].DirInTable,
										theApp.totalItem[index].DirShowKind,
										theApp.totalItem[index].MoreGetKind,
										theApp.totalItem[index].MoreShowKind,
										theApp.totalItem[index].DataGetKind,
										theApp.totalItem[index].DataKind,
										theApp.totalItem[index].DataPath,
										theApp.totalItem[index].SubItemExist,
										theApp.totalItem[index].SubItemInfoTable,
										theApp.totalItem[index].SubItemIsShowInList,
										theApp.totalItem[index].nSubItemShowKind,
										theApp.totalItem[index].SubItemListLevel,
										theApp.totalItem[index].SubItemListPageChangeKind,
										theApp.totalItem[index].ShowEwmKind,
										itemArr["data"]["items"][i]["item"]["sort"].asInt(),
										theApp.totalItem[index].IsCanFind,
										theApp.totalItem[index].IsCanLarge,
										theApp.totalItem[index].InfoKind,
										theApp.totalItem[index].appendkind,
										theApp.totalItem[index].ZanExist,
										0,
										theApp.totalItem[index].NextWinowShowInRect,
										theApp.totalItem[index].BeShowInHomePage,
										itemArr["data"]["items"][i]["id"].asInt(),
										1,
										string2CString(itemArr["data"]["items"][i]["item"]["content"].asString()),
										string2CString(itemArr["data"]["items"][i]["item"]["synopsis"].asString())
										);
					strSqlAdd.Add(strAddItem);
				}
				//UPDATE
				else if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("update")) == 0) {
					urlBody = string2CString(itemArr["data"]["items"][i]["item"]["image"].asString());
					if (!urlBody.IsEmpty()) {
						slashSeparator = urlBody.ReverseFind('/') + 1;
						fileName = urlBody.Mid(slashSeparator);
						filePath = filePathHead + fileName;
						url = resourceHeader + urlBody;
						theApp.DownLoadDataNOIDC(url, filePath, 2);	
						//theApp.RequestHttp(url, filePath, llDownPart);	
					}
					theApp.DownLoadDataNOIDC(url, filePath, 2);						
					//theApp.RequestHttp(url, filePath, llDownPart);	
					// URLDownloadToFile(NULL, url, filePath, NULL, NULL);

					strUpdateItem.Format(_T("UPDATE %s SET ShowDate = '%s', C_NAME = '%s', FmPath = '%s', ItemIntroduce = '%s',")
										_T(" ItemMoreInfo = '%s', OrderNo = %d  WHERE itemID = %d AND ParentID = 'ZJXZ';"),
										strAddTableName,
										string2CString(itemArr["data"]["items"][i]["time"].asString()).Left(10),
										string2CString(itemArr["data"]["items"][i]["item"]["name"].asString()),
										theApp.totalItem[index].FmPath + fileName,
										string2CString(itemArr["data"]["items"][i]["item"]["content"].asString()),
										string2CString(itemArr["data"]["items"][i]["item"]["synopsis"].asString()),
										itemArr["data"]["items"][i]["id"].asInt(),
										itemArr["data"]["items"][i]["id"]
										);
					strSqlUpdate.Add(strUpdateItem);
				}
				//DEL
				else if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("del")) == 0) {
					strDelItem.Format(_T("UPDATE %s SET BeValid = 0 WHERE itemID = %d AND ParentID = 'ZJXZ';"),
										strAddTableName,
										itemArr["data"]["items"][i]["id"].asInt());
					strSqlDel.Add(strDelItem);
				}

				m_time = CTime::GetCurrentTime();
				m_strDateTime = m_time.Format(_T("%Y-%m-%d %H:%M:%S"));
				levelStruct.Format(_T("BYGYBG/ZJXZ"));
				strSqlUpdateCHANGEDINFOItem.Format(_T("INSERT INTO %s(levelStruct, priority, updateDBTime, typeID, operate)")
													_T(" VALUES('%s', %d, '%s', %d, '%s');"),
													 strUpdateTableName,
													levelStruct,
													prio,
													m_strDateTime,
													itemArr["data"]["items"][i]["id"].asInt(),
													string2CString(itemArr["data"]["items"][i]["operate_type"].asString())
													);
				strSqlUpdateCHANGEDINFO.Add(strSqlUpdateCHANGEDINFOItem);
			}

			if (times > 0) {
				int lastInfoIndex = (times < ROWS) ? times - 1 : ROWS - 1;
				CString date = string2CString(itemArr["data"]["items"][lastInfoIndex]["time"].asString());
				int id = itemArr["data"]["items"][lastInfoIndex]["id"].asInt();
				CString strContent;
				strContent.Format(_T("{\"date\":\"%s\", \"page\":1,\"rows\":10, \"id\":%d}"), date, id);
				theApp.m_updateinfoArray[orderNo].strContent = strContent;

				// 把本次最后一条的记录在表中
				strSqlUpdateUPDATEINFO.Format(_T("UPDATE UPDATEINFO SET CONTENT = '%s' WHERE ID = 'ZJXZ';"), strContent);
				updateIfItem.isNeedReadFromTable = TRUE;

				int addCount = strSqlAdd.GetCount();
				int updateCount = strSqlUpdate.GetCount();
				int delCount = strSqlDel.GetCount();
				int updateCHANGEDINFOCount = strSqlUpdateCHANGEDINFO.GetCount();

				while(!theApp.LockWrite(-1, -1));
W1
				theApp.m_sqlite.Begin();
				for (int i = 0; i < addCount; i++) {
					stmt = theApp.m_sqlite.Statement(strSqlAdd.GetAt(i));
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}
				}
				for (int i = 0; i < updateCount; i++) {
					stmt = theApp.m_sqlite.Statement(strSqlUpdate.GetAt(i));
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}
				}
				for (int i = 0; i < delCount; i++) {
					stmt = theApp.m_sqlite.Statement(strSqlDel.GetAt(i));
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}
				}
				for (int i = 0; i < updateCHANGEDINFOCount; i++) {
					stmt = theApp.m_sqlite.Statement(strSqlUpdateCHANGEDINFO.GetAt(i));
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}
				}

				stmt = theApp.m_sqlite.Statement(strSqlUpdateUPDATEINFO);
				if (stmt != NULL && !stmt->IsNullStmt()) {
					stmt->Execute();
					delete stmt;
				}

				theApp.m_sqlite.Commit();
				theApp.UnLockWrite();
EW1
				Sleep(1000);
			}
		}
		break;
	}
	case 9:		// 关于本馆下的本馆发文
	{
		CString strAddTableName = theApp.m_updateinfoArray[9].tableInfo;
		CString RandomID, ParentID = _T("BGFW"), GParentID = _T("BYGYBG");
		CStringArray strSqlAdd, strSqlDel, strSqlUpdateCHANGEDINFO;
		CString url, urlBody;
		int index = 2;
		CString fileName, filePath, filePathHead = theApp.totalItem[index].FmPath;

		int slashSeparator = 0;	
		ULONGLONG llDownPart = 0;

		if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
			int times = itemArr["data"]["total"].asInt();
			for (int i = 0; i < ROWS && i < times; i++) {
				// ADD
				if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("add")) == 0) {
					RandomID.Format(_T("BGFW%.4d"), itemArr["data"]["items"][i]["id"]);
					CString urlBody = string2CString(itemArr["data"]["items"][i]["item"]["fileUrl"].asString());
					if (!urlBody.IsEmpty()) {
						slashSeparator = urlBody.ReverseFind('/') + 1;
						fileName = urlBody.Mid(slashSeparator);
						filePath = filePathHead + fileName;
						url = resourceHeader + urlBody;
						theApp.DownLoadDataNOIDC(url, filePath, 2);
						//theApp.RequestHttp(url, filePath, llDownPart);
					}
						theApp.DownLoadDataNOIDC(url, filePath, 2);

					int timeStamp = string2Ctime((itemArr["data"]["items"][i]["item"]["releaseTime"].asString()).c_str());

					strAddItem.Format(_T("INSERT OR REPLACE INTO %s(ID, C_NAME, Orgernization, ShowDate, ParentID, GParentID, FmKind, ")
									  _T("FmBkPath, FmPath, ShowInLevel, OpenItemKind, OpenItemShowKind, DirGetKind, DirInTable, DirShowKind,")
									  _T(" MoreGetKind, MoreShowKind, DataGetKind, DataKind, DataPath, SubItemExist, SubItemInfoTable,")
									  _T("SubItemIsShowInList,nSubItemShowKind, SubItemListLevel, SubItemListPageChangeKind,ShowEwmKind, ")
									  _T("OrderNo, IsCanFind, IsCanLarge, InfoKind, appendkind, ZanExist, BeTemplate, NextWinowShowInRect, ")
									  _T(" BeShowInHomePage, itemID, BeValid)")
									  _T(" VALUES('%s', '%s', '%s', '%s', '%s', '%s','%s', '%s', '%s', '%s', '%s', '%s','%s', '%s', '%s', '%s',")
									  _T(" '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d,'%s', '%s', '%s', %d, '%s', %d, '%s', '%s', '%s', ")
									  _T("%d, '%s', '%s', %d, %d);"),
									  strAddTableName,
									  RandomID,
									  string2CString(itemArr["data"]["items"][i]["item"]["title"].asString()),
									  string2CString(itemArr["data"]["items"][i]["item"]["organCode"].asString()),
									  string2CString(itemArr["data"]["items"][i]["time"].asString()).Left(10), // 只需要精确到天
									  ParentID,
									  GParentID,
									  theApp.totalItem[index].FmKind,
									  theApp.totalItem[index].FmBkPath,
									  theApp.totalItem[index].FmPath,
									  theApp.totalItem[index].ShowInLevel,
									  theApp.totalItem[index].OpenItemKind,
									  theApp.totalItem[index].OpenItemShowKind,
									  theApp.totalItem[index].DirGetKind,
									  theApp.totalItem[index].DirInTable,
									  theApp.totalItem[index].DirShowKind,
									  theApp.totalItem[index].MoreGetKind,
									  theApp.totalItem[index].MoreShowKind,
									  theApp.totalItem[index].DataGetKind,
									  theApp.totalItem[index].DataKind,
									  theApp.totalItem[index].DataPath + fileName,
									  theApp.totalItem[index].SubItemExist,
									  theApp.totalItem[index].SubItemInfoTable,
									  theApp.totalItem[index].SubItemIsShowInList,
									  theApp.totalItem[index].nSubItemShowKind,
									  theApp.totalItem[index].SubItemListLevel,
									  theApp.totalItem[index].SubItemListPageChangeKind,
									  theApp.totalItem[index].ShowEwmKind,
									  timeStamp,
									  theApp.totalItem[index].IsCanFind,
									  theApp.totalItem[index].IsCanLarge,
									  theApp.totalItem[index].InfoKind,
									  theApp.totalItem[index].appendkind,
									  theApp.totalItem[index].ZanExist,
									  0,
									  theApp.totalItem[index].NextWinowShowInRect,
									  theApp.totalItem[index].BeShowInHomePage,
									  itemArr["data"]["items"][i]["id"].asInt(),
									  1
									  );
					strSqlAdd.Add(strAddItem);
				}
				// DEL
				else if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("del")) == 0) {
					strDelItem.Format(_T("UPDATE %s SET BeValid = 0 WHERE itemID = %d AND ParentID = '%s';"),
						strAddTableName,
						itemArr["data"]["items"][i]["id"].asInt(),
						ParentID);
					strSqlDel.Add(strDelItem);
				}

				// 并且把add, del的记录存放在CHANGEDINFO更新表中
				m_time = CTime::GetCurrentTime();
				m_strDateTime = m_time.Format(_T("%Y-%m-%d %H:%M:%S"));
				levelStruct.Format(_T("BYGYBG/BGFW"));
				strSqlUpdateCHANGEDINFOItem.Format(_T("INSERT OR IGNORE INTO %s(levelStruct, priority, updateDBTime, typeID, operate) VALUES('%s', %d, '%s', %d, '%s');"),
					strUpdateTableName,
					levelStruct,
					prio,
					m_strDateTime,
					itemArr["data"]["items"][i]["id"].asInt(),
					string2CString(itemArr["data"]["items"][i]["operate_type"].asString())
					);
				strSqlUpdateCHANGEDINFO.Add(strSqlUpdateCHANGEDINFOItem);
			}
			if (times > 0) {
				// 把最后一条记录的date和id更新到UPDATEINFO表中
				int lastInfoIndex = (times < ROWS) ? times - 1 : ROWS - 1;
				CString date = string2CString(itemArr["data"]["items"][lastInfoIndex]["time"].asString());
				int id = itemArr["data"]["items"][lastInfoIndex]["id"].asInt();
				CString strContent;
				strContent.Format(_T("{\"date\":\"%s\", \"page\":1,\"rows\":10, \"id\":%d}"), date, id);
				theApp.m_updateinfoArray[orderNo].strContent = strContent;

				// 更新最后一条的记录到表中
				strSqlUpdateUPDATEINFO.Format(_T("UPDATE UPDATEINFO SET CONTENT = '%s' WHERE ID = 'BGFW';"), strContent);
				updateIfItem.isNeedReadFromTable = TRUE;

				while(!theApp.LockWrite(-1, -1));
W1
				theApp.m_sqlite.Begin();

				int addCount = strSqlAdd.GetCount();
				int delCount = strSqlDel.GetCount();
				int updateCHANGEDINFOCount = strSqlUpdateCHANGEDINFO.GetCount();
				for (int i = 0; i < addCount; i++) {
					stmt = theApp.m_sqlite.Statement(strSqlAdd.GetAt(i));
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}
				}
				for (int i = 0; i < delCount; i++) {
					stmt = theApp.m_sqlite.Statement(strSqlDel.GetAt(i));
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}
				}
				for (int i = 0; i < updateCHANGEDINFOCount; i++) {
					stmt = theApp.m_sqlite.Statement(strSqlUpdateCHANGEDINFO.GetAt(i));
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}
				}

					stmt = theApp.m_sqlite.Statement(strSqlUpdateUPDATEINFO);
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}


				theApp.m_sqlite.Commit();
				theApp.UnLockWrite();
EW1
				Sleep(1000);
			}
		}
		break;
	}
	case 10:		// 操作日志
	{
		// 删除本次已经上传的数据
		CString strSqlSelect, strSqlDelete;
		if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
			// 从结构中获取上次更新时间
			CString lastDeleteTime = updateIfItem.strLastUpdateStander;
			
			while(!theApp.LockWrite(-1, -1));
W1
			theApp.m_sqlite.Begin();

			strSqlDelete.Format(_T("DELETE FROM %s WHERE CLICKTIME <= '%s';"), theApp.m_updateinfoArray[10].tableInfo, lastDeleteTime);
			stmt = theApp.m_sqlite.Statement(strSqlDelete);
			if (stmt != NULL && !stmt->IsNullStmt()) {
				stmt->Execute();
				delete stmt;
			}

			theApp.m_sqlite.Commit();
			theApp.UnLockWrite();
EW1
			Sleep(1000);
		}
		break;
	}
	case 11: //导航栏更新
	{
		CString strUpdateItem, strSqlUpdateUPDATEINFO;
		CStringArray strSqlUpdate, strSqlUpdateCHANGEDINFO;

		if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
			int times = itemArr["data"]["total"].asInt();
			for (int i = 0; i < ROWS && i < times; i++)
			{
				// update
				if (string2CString(itemArr["data"]["items"][i]["operate_type"].asString()).CompareNoCase(_T("update")) == 0) {
					strUpdateItem.Format(_T("UPDATE %s SET C_NAME = '%s' WHERE ID = '%s';"),
											theApp.m_updateinfoArray[11].tableInfo,
											string2CString(itemArr["data"]["items"][i]["item"]["text"].asString()),
											string2CString(itemArr["data"]["items"][i]["item"]["code"].asString()));
				}
				strSqlUpdate.Add(strUpdateItem);

				// 并且把add,update,del的记录存放在CHANGEDINFO更新表中
				m_time = CTime::GetCurrentTime();
				m_strDateTime = m_time.Format(_T("%Y-%m-%d %H:%M:%S"));
				levelStruct.Format(_T("BYGYBG/DHLGX"));
				strSqlUpdateCHANGEDINFOItem.Format(_T("INSERT OR IGNORE INTO %s(levelStruct, priority, updateDBTime, typeID, operate)")
													_T(" VALUES('%s', %d, '%s', %d, '%s');"), 
													strUpdateTableName,
													levelStruct,
													prio,
													m_strDateTime,
													itemArr["data"]["items"][i]["id"].asInt(),
													string2CString(itemArr["data"]["items"][i]["operate_type"].asString())
													);
				strSqlUpdateCHANGEDINFO.Add(strSqlUpdateCHANGEDINFOItem);
			}

			if (times > 0) {
				// 把最后一条记录的date和id更新到UPDATEINFO表中
				int lastInfoIndex = (times < ROWS) ? times - 1 : ROWS - 1;
				CString date = string2CString(itemArr["data"]["items"][lastInfoIndex]["time"].asString());
				int id = itemArr["data"]["items"][lastInfoIndex]["id"].asInt();
				CString strContent;
				strContent.Format(_T("{\"date\":\"%s\", \"page\":1,\"rows\":10, \"id\":%d}"), date, id);
				theApp.m_updateinfoArray[orderNo].strContent = strContent;

				strSqlUpdateUPDATEINFO.Format(_T("UPDATE UPDATEINFO SET CONTENT = '%s' WHERE ID = 'DHLGX';"), strContent);
				updateIfItem.isNeedReadFromTable = TRUE;
				
				while(!theApp.LockWrite(-1, -1));
W1
				theApp.m_sqlite.Begin();

				int updateCount = strSqlUpdate.GetCount();
				int updateCHANGEDINFOCount = strSqlUpdateCHANGEDINFO.GetCount();
				for (int i = 0; i < updateCount; i++) {
					stmt = theApp.m_sqlite.Statement(strSqlUpdate.GetAt(i));
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}
				}
				for (int i = 0; i < updateCHANGEDINFOCount; i++) {
					stmt = theApp.m_sqlite.Statement(strSqlUpdateCHANGEDINFO.GetAt(i));
					if (stmt != NULL && !stmt->IsNullStmt()) {
						stmt->Execute();
						delete stmt;
					}
				}

				stmt = theApp.m_sqlite.Statement(strSqlUpdateUPDATEINFO);
				if (stmt != NULL && !stmt->IsNullStmt()) {
					stmt->Execute();
					delete stmt;
				}

				theApp.m_sqlite.Commit();
				theApp.UnLockWrite();
EW1
				Sleep(1000);
			}
		}
	}
	}

	return TRUE;
}

// change string like "2019-10-08 16:47:45" to time_t calander time
unsigned long string2Ctime(const char *str)
{
	struct tm time = {};
	sscanf_s(str, "%4d - %2d - %2d %2d :%2d :%2d",
		&time.tm_year,
		&time.tm_mon,
		&time.tm_mday,
		&time.tm_hour,
		&time.tm_min,
		&time.tm_sec
		);
	time.tm_isdst = -1;
	time.tm_year -= 1900;
	time.tm_mon--;

	time_t CTime = mktime(&time);

	return CTime;
}

DWORD WINAPI CMonitorDlg::ThreadGetUpdateInfoAndData_XWDT(LPVOID lpParameter)
{
	UINT nStartNo = ((UPDATETHREADINFO*)lpParameter)->nStartNo;  
	UINT nEndNo = ((UPDATETHREADINFO*)lpParameter)->nEndNo;
	UINT nRequestTimeGap = ((UPDATETHREADINFO*)lpParameter)->nRequestTimeGap;

	UPDATEINFOITEM updateIfItem;
	CString strRequestUrlHead = theApp.m_strCnkiServerId;
	CString resourceHeader = strRequestUrlHead + theApp.m_strCnkiServerPath;
	CString strRequestUrl, strResult, strContent;

	CTime tmNow = CTime::GetCurrentTime();
	CTimeSpan cts(nRequestTimeGap / 60 / 24, nRequestTimeGap / 60 % 24, nRequestTimeGap % 60, 0);  //设置时间差
	CTime  tmPre = tmNow - cts;

	while (!theApp.m_bQuestClose) {
		Sleep(5);
		if (tmPre + cts <= tmNow) {
			tmPre = tmNow;
			for (int i = nStartNo; i <= nEndNo; ++i) {
				updateIfItem = theApp.m_updateinfoArray.GetAt(i);
				strRequestUrl = strRequestUrlHead + updateIfItem.strDownloadUrl;
				strContent = updateIfItem.strContent;

				int len = WideCharToMultiByte(CP_ACP, 0, strContent, strContent.GetLength(), NULL, 0, NULL, NULL);
				char * content = new char[len + 1];
				WideCharToMultiByte(CP_ACP, 0, strContent, strContent.GetLength(), content, len, NULL, NULL);
				content[len] = '\0';
				theApp.SendStrFromWeb(strRequestUrl, strResult, content, token.c_str());
				Json::Reader reader;
				Json::Value itemArr;

				char buf[1000000] = { 0 };
				CString2Char(strResult, buf);

				if (reader.parse(buf, itemArr, FALSE)) {
					if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("false")) == 0) {
						getNewToken(buf);
						theApp.SendStrFromWeb(strRequestUrl, strResult, content, token.c_str());
						CString2Char(strResult, buf);
						reader.parse(buf, itemArr, FALSE);
					}	
					dealwithXWDT(itemArr, updateIfItem);
				}
			}
		}
		tmNow = CTime::GetCurrentTime();
	}

	return 1;
}

CString getCZRZContent(UPDATEINFOITEM &updateIfItem)
{
	CString strSqlSelectCZRZ;
	strSqlSelectCZRZ.Format(_T("SELECT CLICKID, CLICKTIME FROM %s ORDER BY CLICKTIME ASC LIMIT %d;"), theApp.m_updateinfoArray[10].tableInfo, nNumsEachTime);
	CString CZRZContent, CZRZContentTmp, CZRZCount, totalContent;
	CString strOperateTime, strOperateContent;
	UINT nCount = 0;
	CSqlStatement *stmt = NULL;

	while (!theApp.LockRead(INFINITE, 10));
R1
	stmt = theApp.m_sqlite.Statement(strSqlSelectCZRZ);
	if (stmt != NULL && !stmt->IsNullStmt()) {
		while (stmt->NextRow() && nCount < nNumsEachTime) {
			nCount++;
			strOperateContent = stmt->ValueString(0);
			strOperateTime = stmt->ValueString(1);
			CZRZContentTmp.Format(_T("{ \"strOperatorTime\": \"%s\", \"strOperatorContent\": \"%s\"},"), strOperateTime, strOperateContent);
			CZRZContent += CZRZContentTmp;
		}
	}
	delete stmt;
	theApp.UnLockRead();
	ER1
	Sleep(1000);

	if (nCount == 0)
		return CString();

	// 更新最后一条操作日志的时间到updateIfItem中， 不更新到表中，节省表操作
	updateIfItem.strLastUpdateStander = strOperateTime;

	CZRZContent = CZRZContent.Left(CZRZContent.GetLength() - 1);
	CZRZCount.Format(_T("\"nReturnSubCount\":%d"), nCount);
	totalContent.Format(_T("{\"strComuputerNo\":\"%s\", %s, \"list\":[%s]}"), theApp.m_strRegestNo, CZRZCount, CZRZContent);

	return totalContent;
}


CString getUploadMSGContent(UPDATEINFOITEM &updateIfItem)
{
	CSqlStatement *stmt = NULL;
	CString ID, msg, name, totalContent, strSqlSelectUploadMSG = _T("SELECT ID, content, username FROM USERLEAVEMSG WHERE Valid = 2 LIMIT 1;");

	while (!theApp.LockRead(INFINITE, 10));
R1
	stmt = theApp.m_sqlite.Statement(strSqlSelectUploadMSG);
	stmt->Execute();
	if (CString(stmt->ValueString(0)).CompareNoCase(_T("(null)")) != 0) {
		ID = stmt->ValueString(0);
		msg = stmt->ValueString(1);
		name = stmt->ValueString(2);

		theApp.UnLockRead();
ER1
		Sleep(1000);
		totalContent.Format(_T("{\"machineSerialNo\":\"%s\",\"message\":\"%s\", \"phoneNo\":\"%s\", \"name\":\"%s\"}"),
			theApp.m_strRegestNo, msg, _T(""), name);
		updateIfItem.strLastUpdateStander = ID;		// 本次待删除的数据
		return totalContent;
	} else {
		theApp.UnLockRead();
ER1
		Sleep(1000);
		return CString();
	}
}

// 处理一个线程中的多个模块
DWORD WINAPI CMonitorDlg::ThreadGetUpdateInfoAndData_Others(LPVOID lpParameter)
{
	UINT nStartNo = ((UPDATETHREADINFO*)lpParameter)->nStartNo;
	UINT nEndNo = ((UPDATETHREADINFO*)lpParameter)->nEndNo;
	UINT nRequestTimeGap = ((UPDATETHREADINFO*)lpParameter)->nRequestTimeGap;

	UPDATEINFOITEM updateIfItem;
	CString strRequestUrlHead = theApp.m_strCnkiServerId;
	CString resourceHeader = strRequestUrlHead + theApp.m_strCnkiServerPath;
	CString strRequestUrl, strResult, strContent;

	CTime tmNow = CTime::GetCurrentTime();
	CTimeSpan cts(nRequestTimeGap / 60 / 24, nRequestTimeGap / 60 % 24, nRequestTimeGap % 60, 0);  //设置时间差
	CTime  tmPre = tmNow - cts;

	while (!theApp.m_bQuestClose) {
		Sleep(5);
		if (tmPre + cts <= tmNow) {
			tmPre = tmNow;
			for (int i = nStartNo; i <= nEndNo; ++i) {
				updateIfItem = theApp.m_updateinfoArray.GetAt(i);
				strRequestUrl = strRequestUrlHead + updateIfItem.strDownloadUrl;
				//下载数据
				strContent = updateIfItem.strContent;

				// 扫码留言中URL需要加上registNo, content为registNo
				if (i == 5) {
					//strRequestUrl += theApp.m_strRegestNo;
					//strContent.Format(_T("{\"serialNo\":\"%s\"}"), theApp.m_strRegestNo);
					continue;
				}

				// 上传留言需要用到registNo
				if (i == 6) {
					strContent = getUploadMSGContent(updateIfItem);
					if (strContent.IsEmpty()) {
						continue;
					}
				}
				// 上传操作日志时需要加上strComuputerNo，这个从ini文件中读取 上传日志在表中的OrderNo为10
				if (i == 10) {
					strContent = getCZRZContent(updateIfItem);
					if (strContent.IsEmpty())
						continue;
				}

				int len = WideCharToMultiByte(CP_UTF8, 0, strContent, strContent.GetLength(), NULL, 0, NULL, NULL);
				char * content = new char[len + 1];
				WideCharToMultiByte(CP_UTF8, 0, strContent, strContent.GetLength(), content, len, NULL, NULL);
				content[len] = '\0';

				theApp.SendStrFromWeb(strRequestUrl, strResult, content, token.c_str());

				Json::Reader reader;
				Json::Value itemArr;

				char buf[1000000] = { 0 };
				CString2Char(strResult, buf);

				if (reader.parse(buf, itemArr, FALSE)) {
					if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("false")) == 0) {
						getNewToken(buf);

						theApp.SendStrFromWeb(strRequestUrl, strResult, content, token.c_str());
						CString2Char(strResult, buf);
						reader.parse(buf, itemArr, FALSE);
					}
					dealwithOthers(itemArr, updateIfItem);
				}
			}
		}
		tmNow = CTime::GetCurrentTime();
	}
	return 1;
}

CString &addTime(CString &date, int year, int month, int day)
{
	int year_now = _ttoi(date.Left(4));
	int month_now = _ttoi(date.Mid(5, 2));
	int day_now = _ttoi(date.Mid(8, 2));
	struct tm date_now;
	date_now.tm_year = year_now + year - 1900;
	date_now.tm_mon = month_now + month - 1;
	date_now.tm_mday = day_now + day;
	date_now.tm_sec = 0;
	date_now.tm_min = 0;
	date_now.tm_hour = 0;
	date_now.tm_isdst = -1;
	mktime(&date_now);	// 恢复正确日期格式
	date.Format(_T("%.4d-%.2d-%.2d"), date_now.tm_year + 1900, date_now.tm_mon + 1, date_now.tm_mday);
	return date;
}

DWORD WINAPI CMonitorDlg::ThreadGetUpdateInfoAndData_QK_TS(LPVOID lpParameter)
{
	UINT nRequestTimeGap = ((UPDATETHREADINFO*)lpParameter)->nRequestTimeGap;
    UINT nStartNo = ((UPDATETHREADINFO*)lpParameter)->nStartNo;
    UINT nEndNo = ((UPDATETHREADINFO*)lpParameter)->nEndNo;

	UPDATEINFOITEM updateIfItem;
	CString strRequestUrlHead = theApp.m_strCnkiServerId;

	CString resourceHeader = strRequestUrlHead + theApp.m_strCnkiServerPath;
	CString strRequestUrl, strResult, strContent;

	CTime tmNow = CTime::GetCurrentTime();
	CTimeSpan cts(nRequestTimeGap / 60 / 24, nRequestTimeGap / 60 % 24, nRequestTimeGap % 60, 0);  //设置时间差
	CTime  tmPre = tmNow - cts;

	while (!theApp.m_bQuestClose) {
		Sleep(5);
		if (tmPre + cts <= tmNow) {
			tmPre = tmNow;
			for(int i = nStartNo; i < nEndNo; i++) {
				// 时间加上一天作为更新开始标志
				addTime(theApp.m_updateinfoArray.GetAt(i).strLastUpdateStander, 0, 0, 1);

				updateIfItem = theApp.m_updateinfoArray.GetAt(i);
				CString requestSubKind, requestItemsData, currentTimeUpdateStander = updateIfItem.strLastUpdateStander;
				updateIfItem.nUpdateKind = 1;				// nUpdateKind为1代表按照时间更新，为2代表按照单本更新
				if(updateIfItem.nRequestSubKind == 1) {
					requestSubKind = _T("qk");
				}
				else {
					requestSubKind = _T("ts");
				}

				
				int countPrePage = 0;
				int nPage = 1;
				BOOL firstPageFlag = TRUE;	// 设置第一页标志为TRUE，作用为完成第一页最后一条的那个时间的所有期刊都要更新
					// 把库里最后更新日期加一天作为新时间提取
				// 增加一层循环  分别处理每一个pages页
				do{
					// 现在改成线程执行一次只更新一页，然后把这一页的最后一条作为标记记录下来
					requestItemsData.Format(_T("?computerNo=%s&requestKind=%d&requestSubKind=%s&updateKind=%d&updateStander=%s&page=%d&rows=%d"),
											theApp.m_strRegestNo,
											updateIfItem.nRequestKind,
											requestSubKind,
											1,
											currentTimeUpdateStander,
											nPage,
											ROWS
											);
					strRequestUrl = strRequestUrlHead + updateIfItem.strDownloadUrl + requestItemsData;
					if (theApp.GetStrFromWeb(strRequestUrl, strResult, _T(""))) {
						Json::Reader reader;
						Json::Value itemArr;
						char buf[1000000] = { 0 };

						CString2Char(strResult, buf);
						if (reader.parse(buf, itemArr, FALSE)) {
							countPrePage = itemArr["count"].asInt();
							if(countPrePage > 0)
								countPrePage = dealwithQK_TS(itemArr, theApp.m_updateinfoArray.GetAt(i), firstPageFlag);
						}
					}
					nPage++;
				} while (countPrePage >= ROWS);
			}
		}
		tmNow = CTime::GetCurrentTime();
	}
	return TRUE;
}

/***************************************************************************************************************************************************
 * 处理期刊更新的当前nPage页，把当前页的所有本期刊全部更新到表中. FOURTHLEVEL表 DKYDDIR_0表 CHANGEDINFO表.
 * 由于需要避免定时关机时更新标识没有正确记录最后时间，需要在本次关机时把最后那一本期刊的时间之前的所有期刊都更新完
 * 
 *
 * itemArr      @从WebServer请求的当前nPage页的所有期刊信息，Value 类型的值 
 * updateIfItem @当前线程更新用到的从表中查询得到的数据信息
 *
 * 返回值：		@本次dealwith更新的期刊的本数
 **************************************************************************************************************************************************/
int dealwithQK_TS(Json::Value &itemArr, UPDATEINFOITEM &updateIfItem, BOOL &firstPageFlag)
{
	int index = 5;				// 期刊图书的模板信息在数组的index为5里面
	CString strSqlInsertQK_TS, strSqlInsertDir, strSqlUpdateCHANGEDINFO, filePathHead = theApp.totalItem[index].FmPath;
	CString fmUrl, fmFileName, fmDirLevel1, fmDirLevel2, fmFilePath, dataUrl, dataFileName, dataDirLevel1, dataDirLevel2, dataFilePath;
	int tableInfoSeperator = theApp.m_updateinfoArray[12].tableInfo.Find('-');
	CString QK_TSTableName = theApp.m_updateinfoArray[12].tableInfo.Left(tableInfoSeperator);
	CString QK_TSDirName = theApp.m_updateinfoArray[12].tableInfo.Mid(tableInfoSeperator + 1);
	int numOfDirItemsPre;					// 每本期刊的目录分隔的数量
	int i = 0;	// 代表本次更新的期刊的本数
	CString modifiedC_NAME;					// C_NAME中可能含有单引号的字符，所以在sqlite中需要把单引号转换为两个单引号再插入
	int priority = updateIfItem.nPriority;
	CString operate;						// 每本期刊的操作 1：更新.  3：删除.
	CTime m_time;
	CString m_strDateTime;

	if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
		int times = itemArr["count"].asInt();
		int lastTimeIndex = 0;
		CString ID, ParentID, GParentID, strSqlSelectGParentID;
		CSqlStatement *stmtQK_TS = NULL, *stmtQK_TS_Dir = NULL;
		CString orderNo;
		orderNo.Format(_T("%d"), theApp.totalItem[index].OrderNo);
		for (i = 0; i < times; i++) {
			ID = string2CString(itemArr["list"][i]["id"].asString());
			ParentID = ID.Left(4);
			int numOfDirItemsPre = itemArr["list"][i]["dir"].size();
			operate = string2CString(itemArr["list"][i]["nKind"].asString());
			
			if (string2CString(itemArr["list"][i]["updatedate"].asString()).CompareNoCase(updateIfItem.strLastUpdateStander) != 0 
				&& firstPageFlag == FALSE)
				break;

			if (operate.CompareNoCase(_T("1")) == 0) {
				strSqlSelectGParentID.Format(_T("SELECT GParentID FROM %s WHERE ParentID = '%s';"), QK_TSTableName, ParentID);
				
				Sleep(1000);
				while (!theApp.LockRead(INFINITE, 10));
R2
				stmtQK_TS = theApp.m_sqlite.Statement(strSqlSelectGParentID);
				stmtQK_TS->Execute();
				GParentID = stmtQK_TS->ValueString(0);
				delete stmtQK_TS;
				theApp.UnLockRead();	
ER2
				Sleep(10);
ER2
				fmUrl = string2CString(itemArr["list"][i]["fmDUrl"].asString());
				fmFileName = fmUrl.Mid(fmUrl.ReverseFind('/') + 1);
				fmDirLevel1 = filePathHead + GParentID + _T("\\");
				fmDirLevel2 = fmDirLevel1 + + ParentID + _T("\\");
				fmFilePath = fmDirLevel2 + fmFileName;
				if (!PathFileExists(fmDirLevel2)) {
					if (!PathFileExists(fmDirLevel1))
						if (!CreateDirectory(fmDirLevel1, NULL)) {
							AfxMessageBox(_T("保存路径创建失败！"));
							return 0;
						}

					if (!CreateDirectory(fmDirLevel2, NULL)) {
						AfxMessageBox(_T("保存路径创建失败！"));
						return 0;
					}
				}
ER2
				// 下载封面
				ULONGLONG llDownPart = 0;
				theApp.DownLoadDataNOIDC(fmUrl, fmFilePath, 2);	
				//theApp.RequestHttp(fmUrl, fmFilePath, llDownPart);
ER2
				dataUrl = string2CString(itemArr["list"][i]["dataDUrl"].asString());
				dataFileName = dataUrl.Mid(dataUrl.ReverseFind('/') + 1);
				dataDirLevel1 = filePathHead + GParentID + _T("\\");
				dataDirLevel2 = fmDirLevel1 + + ParentID + _T("\\");
				dataFilePath = dataDirLevel2 + dataFileName;
				if (!PathFileExists(dataDirLevel2)) {
					if (!PathFileExists(dataDirLevel1))
						if (!CreateDirectory(dataDirLevel1, NULL)) {
							AfxMessageBox(_T("保存路径创建失败！"));
							return 0;
						}
					if (!CreateDirectory(dataDirLevel2, NULL)) {
						AfxMessageBox(_T("保存路径创建失败！"));
						return 0;
					}
				}
				// 下载数据文件
				//theApp.RequestHttp(dataUrl, dataFilePath, llDownPart);
				theApp.DownLoadDataNOIDC(dataUrl, dataFilePath, 2);
ER2
				// 插入单本期刊到FOURTHLEVEL中
				strSqlInsertQK_TS.Format(_T("INSERT OR REPLACE INTO %s(ID, ParentID, GParentID, GGParentID, GGGParentID,"
										_T(" FmKind, FmPath, ShowInLevel, OpenItemKind, OpenItemShowKind, DirGetKind, DirInTable,")
										_T("DirShowKind, MoreGetKind, MoreShowKind, DataGetKind, DataKind, DataPath, SubItemExist,")
										_T(" SubItemIsShowInList, nSubItemShowKind, SubItemListLevel, SubItemListPageChangeKind,")
										_T(" ShowEwmKind, OrderNo, IsCanLarge, IsCanFind, UpdateTime, InfoKind, appendkind, ZanExist,")
										_T(" BeTemplate, BeValid, append1kind, append1info, BeShowInHomePage) VALUES('%s', '%s', '%s', '%s',")
										_T(" '%s', '%s', '%s', '%s', '%s', '%s',  '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s',")
										_T(" %d, '%s', '%s', '%s', '%s', %d, '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s', %d);")),
										QK_TSTableName,
										ID,
										ParentID,
										GParentID,
										theApp.totalItem[index].GGParentID,
										theApp.totalItem[index].GGGParentID,
										theApp.totalItem[index].FmKind,
										fmFilePath,
										theApp.totalItem[index].ShowInLevel,
										theApp.totalItem[index].OpenItemKind,
										theApp.totalItem[index].OpenItemShowKind,
										theApp.totalItem[index].DirGetKind,
										theApp.totalItem[index].DirInTable,
										theApp.totalItem[index].DirShowKind,
										theApp.totalItem[index].MoreGetKind,
										theApp.totalItem[index].MoreShowKind,
										theApp.totalItem[index].DataGetKind,
										theApp.totalItem[index].DataKind,
										dataFilePath,
										theApp.totalItem[index].SubItemExist,
										theApp.totalItem[index].SubItemIsShowInList,
										theApp.totalItem[index].nSubItemShowKind,
										theApp.totalItem[index].SubItemListLevel,
										theApp.totalItem[index].SubItemListPageChangeKind,
										theApp.totalItem[index].ShowEwmKind,
										orderNo,
										theApp.totalItem[index].IsCanLarge,
										theApp.totalItem[index].IsCanFind,
										string2CString(itemArr["list"][i]["updatedate"].asString()),
										theApp.totalItem[index].InfoKind,
										theApp.totalItem[index].appendkind,
										theApp.totalItem[index].ZanExist,
										0,
										theApp.totalItem[index].BeValid,
										theApp.totalItem[index].append1kind,
										string2CString(itemArr["list"][i]["pages"].asString()) + _T("/") +
										string2CString(itemArr["list"][i]["otherPage"].asString()),
										theApp.totalItem[index].BeShowInHomePage
										);
				// 插入单本期刊的所有目录到DKYDDIR_0表
				strSqlInsertDir.Format(_T("INSERT OR REPLACE INTO %s(ID, C_NAME, pages, thname) VALUES (?, ?, ?, ?);"), QK_TSDirName);

W2				
				while(!theApp.LockWrite(-1, -1));
W2
				theApp.m_sqlite.Begin();
				
				stmtQK_TS = theApp.m_sqlite.Statement(strSqlInsertQK_TS);
				stmtQK_TS_Dir = theApp.m_sqlite.Statement(strSqlInsertDir);

				stmtQK_TS->Execute();
				delete stmtQK_TS;

				// 插入单本期刊的所有目录到DKYDDIR_0表 
				for (int j = 0; j < numOfDirItemsPre; j++) {
					(modifiedC_NAME = string2CString(itemArr["list"][i]["dir"][j]["title"].asString())).Replace(_T("'"), _T("''"));
					stmtQK_TS_Dir->Bind(0, string2CString(itemArr["list"][i]["dir"][j]["fileid"].asString()));
					stmtQK_TS_Dir->Bind(1, modifiedC_NAME);
					stmtQK_TS_Dir->Bind(2, string2CString(itemArr["list"][i]["dir"][j]["pages"].asString()));
					stmtQK_TS_Dir->Bind(3, string2CString(itemArr["list"][i]["dir"][j]["fileid"].asString()).Left(10));

					stmtQK_TS_Dir->Reset();
				}
				delete stmtQK_TS_Dir;

				lastTimeIndex = i;		// 把本页最后一本期刊更新的时间记录 作为下次更新的标识

				theApp.m_sqlite.Commit();
				theApp.UnLockWrite();
EW2
				Sleep(30 * 1000);
			}	else if (operate.CompareNoCase(_T("3")) == 0) {
				CString strSqlDeleteJournal, strSqlDelteDir;
				strSqlDeleteJournal.Format(_T("DELETE FROM %s WHERE ID = '%s';"), QK_TSTableName, ID);
				strSqlDelteDir.Format(_T("DELETE FROM %s WHERE thname = '%s';"), QK_TSDirName, ID);

				while(!theApp.LockWrite(-1, -1));
W2
				theApp.m_sqlite.Begin();

				stmtQK_TS = theApp.m_sqlite.Statement(strSqlDeleteJournal);
				stmtQK_TS_Dir = theApp.m_sqlite.Statement(strSqlDelteDir);

				stmtQK_TS->Execute();
				stmtQK_TS_Dir->Execute();
				delete stmtQK_TS;
				delete stmtQK_TS_Dir;

				theApp.m_sqlite.Commit();
				theApp.UnLockWrite();
EW2
				Sleep(100);
			} else {
				continue;
			}

			// update CHANGEDINFO TABLE
			m_time = CTime::GetCurrentTime();
			m_strDateTime = m_time.Format(_T("%Y-%m-%d %H:%M:%S"));
			CString levelStruct = theApp.totalItem[index].GGGParentID + _T("/") + theApp.totalItem[index].GGParentID + _T("/")+ GParentID + _T("/") + _T("/") + ID;
			strSqlUpdateCHANGEDINFO.Format(_T("INSERT INTO CHANGEDINFO(levelStruct, priority, operate, updateDBTime) VALUES ")
										   _T("('%s', %d, '%s', '%s');"),
											levelStruct,
											priority,
											operate.CompareNoCase(_T("1")) ? _T("del") : _T("add"),
											m_strDateTime
											);
			
			while(!theApp.LockWrite(-1, -1));
W2
			theApp.m_sqlite.Begin();
			stmtQK_TS = theApp.m_sqlite.Statement(strSqlUpdateCHANGEDINFO);
			stmtQK_TS->Execute();
			delete stmtQK_TS;
			theApp.m_sqlite.Commit();		// fourthlevel表和DKYDDIR_0表和CHANGEDINFO表需要保持原子性，用commit.
			theApp.UnLockWrite();
EW2
			Sleep(1000);
		}
		if(firstPageFlag)	// 只需要记录第一次（也即第一页）更新后的最后一条期刊的日期
			updateIfItem.strLastUpdateStander = string2CString(itemArr["list"][lastTimeIndex]["updatedate"].asString());
		firstPageFlag = FALSE;
	}
	return i;
}

//DWORD WINAPI CMonitorDlg::ThreadGetUpdateInfoAndData_TreadNo(LPVOID lpParameter)
//{
//	HWND hwnd = ((PSENDTOTHREED*)lpParameter)->hWndDlg;
//
//	UINT nPartNo = ((PSENDTOTHREED*)lpParameter)->nThreadNo - 1;
//
//	CMonitorDlg* pDlg = (CMonitorDlg*)FromHandle(hwnd);
//
//	CTime tmNow = CTime::GetCurrentTime();
//	CTimeSpan cts(theApp.m_updateThreadArray.GetAt(nPartNo).nRequestTimeGap / 60 / 60, theApp.m_updateThreadArray.GetAt(nPartNo).nRequestTimeGap / 60 % 60, theApp.m_updateThreadArray.GetAt(nPartNo).nRequestTimeGap % 60, 0);  //����ʱ���
//	CTime  ctmPre = tmNow - cts;
//
//	while (!theApp.m_bQuestClose)
//	{
//		Sleep(5);
//
//		if (theApp.m_updateThreadArray.GetAt(nPartNo).nStartNo<theApp.m_updateThreadArray.GetAt(nPartNo).nEndNo)
//		{
//			if (ctmPre + cts <= tmNow)
//			{
//				ctmPre = tmNow;
//				for (size_t i = theApp.m_updateThreadArray.GetAt(nPartNo).nStartNo; i < theApp.m_updateThreadArray.GetAt(nPartNo).nEndNo; i++)
//				{
//					//��������
//					UPDATEINFOITEM updateIfItem = theApp.m_updateinfoArray.GetAt(i);
//
//					CString strRequestUrl, strResult;
//					strRequestUrl.Format(_T("%s?ComputerNo=%s&VerSion=%s&RequestKind=%d&RequestSubKind=%d&Kind=%s&UpdateKind=%d&UpdateStander=%s"),
//						updateIfItem.strDownloadUrl, theApp.m_strRegestNo, theApp.m_strProgramVertion, updateIfItem.nRequestKind, updateIfItem.nRequestSubKind, updateIfItem.strKindId, updateIfItem.nUpdateKind, updateIfItem.strLastUpdateStander);
//
//
//					if (!theApp.GetStrFromWeb(strRequestUrl, strResult) || strResult.IsEmpty())
//					{
//
//						continue;
//					}
//
//					Json::Reader reader;
//					Json::Value itemArr;
//
//					char buf[1000000] = { 0 };
//					CString2Char(strResult, buf);
//
//
//					if (reader.parse(buf, itemArr, FALSE) && !theApp.m_bQuestClose)
//					{
//						switch (updateIfItem.nRequestKind)//�������� 1Ϊ�����ڿ���  2 Ϊ ���ض�����   3Ϊ���ص�һ����
//						{
//						case 1:
//							GetUpdateInfoAndData_Kind1(itemArr, updateIfItem.strKindId, updateIfItem.strLastUpdateStander);
//							break;
//						case 2:
//							GetUpdateInfoAndData_Kind2(itemArr, updateIfItem.strKindId, updateIfItem.strLastUpdateStander);
//							break;
//						case 3://Ŀǰ ֻ�� �õ����к� ��  �õ��ػ�ʱ��
//							GetUpdateInfoAndData_Kind3(itemArr, updateIfItem.strKindId, updateIfItem.strLastUpdateStander);
//							break;
//						case 4:
//							break;
//						case 5:
//							break;
//
//						default:
//							break;
//						}
//
//
//					}
//
//
//
//
//				}
//			}
//
//			tmNow = CTime::GetCurrentTime();
//		}
//		else
//		{
//			return 1;
//		}
//
//	}
//
//	return 1;
//}

DWORD WINAPI CMonitorDlg::ThreadGetUpdateInfoAndDataEx_Personset(LPVOID lpParame)
{
	//得到用户设置更新
	CMonitorDlg* pDlg = (CMonitorDlg*)lpParame;
	CTime tmNow = CTime::GetCurrentTime();
	CTimeSpan cts(0, 0, 5, 0);  //设置时间差
	CTime  ctmPre = tmNow - cts;
	while (!theApp.m_bQuestClose)
	{
		Sleep(5);
		if (ctmPre + cts <= tmNow)
		{
			ctmPre = tmNow;

			CStringArray strArrayPersonsetId;
			CStringArray strArrayUpdateTime;
			CStringArray strArrayPersonsetIdCname;
			CString strSql;


			strSql = _T("SELECT PERSONSETINFO.KINDID,PERSONSETINFO.UPDATETIME,PERSONSETINFO.KINDCNAME FROM PERSONSETINFO group by  PERSONSETINFO.KINDID");
			CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
			while (!theApp.LockRead(INFINITE, 10));
			if (stmt != NULL && !stmt->IsNullStmt())
			{
				while (stmt->NextRow())
				{
					strArrayPersonsetId.Add(stmt->ValueString(0));
					strArrayPersonsetIdCname.Add(stmt->ValueString(2));
					strArrayUpdateTime.Add(stmt->ValueString(1));
				}

				delete stmt;
				stmt = NULL;
			}
			else
			{

				AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
				theApp.UnLockRead();
				return FALSE;
			}




			if (strArrayPersonsetId.GetSize()>0)
			{
				struct ItemSruct
				{
					CString _pid;  //唯一标示
					CString _ptitle;  //标题
					CString _pimageurl; //图片链接
					CString _psinfoflg;  //内容种类
					CString _pcontent; //内容    html  或者pdf文件路径
					CString _pdate; //更新日期  
					CString _pKindiD;
					CString _pUpdateFlage;//更新类型
				};

				struct InsertTableInfo
				{
					CString  strid;  //唯一标示
					CString  strpicname; //图片链接
					CString  strtitle;  //标题

					CString strcontentkind;  //内容种类
					CString strdir; //指向（url或内容 id）
					CString strUpdateTime; //更新日期  
					CString  strKindId;
					CString  strKindCname;
					UINT nItemOperatorKind;// 1(删除) 2(修改)  3(新增)

				};


				CArray<InsertTableInfo, InsertTableInfo>itemArray;
				for (size_t i = 0; i < strArrayPersonsetId.GetSize(); i++)
				{
					CString strResult, strTempUrl;

					strTempUrl.Format(_T("%s/api/Touch/GetPersonalData?MacNumber=%s&kindname=%s&datetime=%s&typename=%s"),
						theApp.m_strCnkiServerId, theApp.m_strRegestNo, strArrayPersonsetId.GetAt(i), strArrayUpdateTime.GetAt(i), theApp.m_strKindEname);
					if (!theApp.GetStrFromWeb(strTempUrl, strResult) || strResult.IsEmpty())
					{

						continue;
					}

					Json::Reader reader;
					Json::Value itemArr;

					char buf[1000000] = { 0 };
					CString2Char(strResult, buf);


					if (reader.parse(buf, itemArr, FALSE) && !theApp.m_bQuestClose)
					{

						if (string2CString(itemArr["isTrue"].asString()).CompareNoCase(_T("true")) == 0
							&& string2CString(itemArr["isupdate"].asString()).CompareNoCase(_T("true")) == 0
							&& string2CString(itemArr["PkindName"].asString()).CompareNoCase(strArrayPersonsetId.GetAt(i)) == 0)
						{
							int iContent_size = itemArr["ContentList"].size();

							for (int j = 0; j < iContent_size; j++)
							{
								ItemSruct item;
								item._pid = string2CString(itemArr["ContentList"][j]["PID"].asString());
								item._ptitle = string2CString(itemArr["ContentList"][j]["PTitle"].asString());
								item._pimageurl = string2CString(itemArr["ContentList"][j]["PImageUrl"].asString());
								item._psinfoflg = string2CString(itemArr["ContentList"][j]["PSInfoFlag"].asString());
								item._pcontent = string2CString(itemArr["ContentList"][j]["PContent"].asString());
								item._pdate = string2CString(itemArr["ContentList"][j]["PDate"].asString());
								item._pUpdateFlage = string2CString(itemArr["ContentList"][j]["PUpdateFlag"].asString());


								item._pKindiD = strArrayPersonsetId.GetAt(i);

								InsertTableInfo insertinfo;
								insertinfo.strid = item._pid;
								insertinfo.strtitle = item._ptitle;
								insertinfo.strcontentkind = item._psinfoflg;
								insertinfo.strdir = item._pcontent;
								insertinfo.strKindId = item._pKindiD;
								insertinfo.strUpdateTime = item._pdate;
								insertinfo.strKindCname = strArrayPersonsetIdCname.GetAt(i);

								if (item._pUpdateFlage == _T("delete"))
								{
									insertinfo.nItemOperatorKind = 1;
									CString strFind = _T("KINDID=\'") + insertinfo.strKindId + _T("\'") _T(" and ID=\'") + insertinfo.strid + _T("\'");
									if (!pDlg->ItemIsInTable(_T("PERSONSETINFO"), strFind))
									{
										insertinfo.nItemOperatorKind = 0;
									}
								}
								else if (item._pUpdateFlage == _T("update"))
								{
									insertinfo.nItemOperatorKind = 2;
									CString strFind = _T("KINDID=\'") + insertinfo.strKindId + _T("\'") _T(" and ID=\'") + insertinfo.strid + _T("\'");

									if (!pDlg->ItemIsInTable(_T("PERSONSETINFO"), strFind))
									{
										insertinfo.nItemOperatorKind = 3;
									}
								}
								else if (item._pUpdateFlage == _T("add"))
								{
									insertinfo.nItemOperatorKind = 3;
								}
								else
								{
									insertinfo.nItemOperatorKind = 0;
								}


								CString strFmDir, strDataDir;
								if (strArrayPersonsetId.GetAt(i).CompareNoCase(_T("logo")) == 0
									|| strArrayPersonsetId.GetAt(i).CompareNoCase(_T("bannername")) == 0
									|| strArrayPersonsetId.GetAt(i).CompareNoCase(_T("urlimage")) == 0)
								{
									strFmDir = theApp.m_strExePath + _T("\\image_") + theApp.m_strKindEname + _T("\\");
									strDataDir = _T("");

								}
								else if (strArrayPersonsetId.GetAt(i).CompareNoCase(_T("mimage")) == 0)
								{
									strFmDir = theApp.m_strCanChangePath + _T("\\showpic\\image\\");
									strDataDir = theApp.m_strCanChangePath + _T("\\showpic\\data\\");
									if (!PathFileExists(strDataDir))
									{
										CreateDirectory(strDataDir, NULL);
									}
								}
								else if (strArrayPersonsetId.GetAt(i).CompareNoCase(_T("news")) == 0 ||
									strArrayPersonsetId.GetAt(i).CompareNoCase(_T("notf")) == 0)
								{
									strFmDir = theApp.m_strCanChangePath + _T("\\") + strArrayPersonsetId.GetAt(i) + _T("\\image\\");
									strDataDir = theApp.m_strCanChangePath + _T("\\") + strArrayPersonsetId.GetAt(i) + _T("\\data\\");
									if (!PathFileExists(strDataDir))
									{
										CreateDirectory(strDataDir, NULL);
									}
								}
								else
								{
									strFmDir = theApp.m_strExePath + _T("\\image_") + theApp.m_strKindEname + _T("\\");
									strDataDir = _T("");
								}


								if (!item._pimageurl.IsEmpty())
								{
									CString strimageName;
									CString strUrl = item._pimageurl;
									CString strDirPath = strFmDir;
									if (strUrl.ReverseFind('/')>0)
									{
										strimageName = strUrl.Right(strUrl.GetLength() - strUrl.ReverseFind('/') - 1);
									}
									insertinfo.strpicname = strimageName;
									strDirPath += strimageName;
									CString strFind = _T("KINDID=\'") + insertinfo.strKindId + _T("\'") _T(" and ID=\'") + insertinfo.strid + _T("\'") + _T("\'  and  PICNAME=\'") + strimageName + _T("\'");

									if ((insertinfo.nItemOperatorKind == 2 && !pDlg->ItemIsInTable(_T("PERSONSETINFO"), strFind))
										|| insertinfo.nItemOperatorKind == 3)
									{
										if (PathFileExists(strDirPath))
										{
											DeleteFile(strDirPath);
										}

										if (strUrl.GetLength()<7 || strUrl.Left(7) != _T("http://"))
										{
											strUrl = _T("http://") + strUrl;
										}


										ULONGLONG lTemp = 0;
										UINT nResult = theApp.RequestHttp(strUrl, strDirPath, lTemp);

										if (nResult != RES_REQ_DONE_OK)
										{
											DeleteFile(strDirPath);

											if (nResult == RES_REQ_ERROR_NOSPACE)
											{
												AfxMessageBox(_T(" 空间不足！  文件  ") + strDirPath + _T("  存储失败！"), MB_OK | MB_SYSTEMMODAL);
											}
											continue;
										}
									}


								}

								if (insertinfo.nItemOperatorKind == 2 || insertinfo.nItemOperatorKind == 3)
								{
									if (item._psinfoflg == _T("内容") && !strDataDir.IsEmpty() && !item._pcontent.IsEmpty())
									{
										insertinfo.strdir = GUIDGen() + _T(".html");
										CString strDirPath = strDataDir + insertinfo.strdir;
										if (!InserterHtml(theApp.m_strCanChangePath + _T("\\html\\emptyforchange.html"), strDirPath, item._pcontent))
										{
											continue;
										}

									}
									else if (item._psinfoflg == _T("内容"))
									{
										continue;
									}
								}



								itemArray.Add(insertinfo);
							}



						}


					}

				}


				//修正personset表
				if (itemArray.GetSize()>0)
				{


					for (size_t j = 0; j < itemArray.GetSize(); j++)
					{

						if (itemArray.GetAt(j).nItemOperatorKind == 2 || itemArray.GetAt(j).nItemOperatorKind == 3)
						{
							strSql = _T("INSERT INTO [PERSONSETINFO] ([ID],[PICNAME],[TEXT],[ULTIMATEAIM],[KINDID],[KINDCNAME],[UPDATETIME],[CONTENTKIND],[OPERATORKIND])  ");
							strSql.AppendFormat(_T("VALUES(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',1)"),
								itemArray.GetAt(j).strid, itemArray.GetAt(j).strpicname, itemArray.GetAt(j).strtitle, itemArray.GetAt(j).strdir, itemArray.GetAt(j).strKindId,
								itemArray.GetAt(j).strKindCname, itemArray.GetAt(j).strUpdateTime, itemArray.GetAt(j).strcontentkind);
							if (!theApp.m_sqlite.DirectStatement(strSql))
							{

								AfxMessageBox(_T("插入数据失败！") + strSql);
								return FALSE;
							}
						}


					}
					//	CString strPreKind;
					for (size_t j = 0; j < itemArray.GetSize(); j++)
					{
						//if (strPreKind == itemArray.GetAt(j).strKindId)
						//{
						//	continue;
						//}
						//else
						//{
						//	strPreKind = itemArray.GetAt(j).strKindId;
						//}
						if (itemArray.GetAt(j).nItemOperatorKind == 1)
						{
							strSql.Format(_T("update  PERSONSETINFO set  OPERATORKIND=0 where PERSONSETINFO.ID=\'%s\' and PERSONSETINFO.KINDID = \'%s\' "),
								itemArray.GetAt(j).strid, itemArray.GetAt(j).strKindId);

							if (!theApp.m_sqlite.DirectStatement(strSql))
							{

								AfxMessageBox(_T("修正数据失败！") + strSql);
								return FALSE;
							}
						}
						else if (itemArray.GetAt(j).nItemOperatorKind == 2)
						{
							strSql.Format(_T("update  PERSONSETINFO set  OPERATORKIND=0 where PERSONSETINFO.ID=\'%s\' and PERSONSETINFO.KINDID = \'%s\'  and PERSONSETINFO.UPDATETIME <> \'%s\'"),
								itemArray.GetAt(j).strid, itemArray.GetAt(j).strKindId, itemArray.GetAt(j).strUpdateTime);

							if (!theApp.m_sqlite.DirectStatement(strSql))
							{

								AfxMessageBox(_T("修正数据失败！") + strSql);
								return FALSE;
							}

						}
						else if (itemArray.GetAt(j).nItemOperatorKind == 3)
						{
							if (itemArray.GetAt(j).strKindId.CompareNoCase(_T("logo")) == 0
								|| itemArray.GetAt(j).strKindId.CompareNoCase(_T("bannername")) == 0)
							{
								strSql.Format(_T("update  PERSONSETINFO set  OPERATORKIND=0 where   PERSONSETINFO.KINDID = \'%s\'  and PERSONSETINFO.UPDATETIME <> \'%s\'"),
									itemArray.GetAt(j).strKindId, itemArray.GetAt(j).strUpdateTime);

								if (!theApp.m_sqlite.DirectStatement(strSql))
								{

									AfxMessageBox(_T("修正数据失败！") + strSql);
									return FALSE;
								}
							}

						}


					}



				}


			}
		}

		tmNow = CTime::GetCurrentTime();
	}
	theApp.UnLockRead();
}

DWORD WINAPI CMonitorDlg::ThreadGetUpdateInfoAndDataEx_ShfwSet(LPVOID lpParame)
{
	//CMonitorDlg* pDlg = (CMonitorDlg*)lpParame;
	//CTime tmNow = CTime::GetCurrentTime();
	//CTimeSpan cts(0, 0, 2, 0);  //设置时间差
	//CTime  ctmPre = tmNow - cts;
	//while (!theApp.m_bQuestClose)
	//{
	//	Sleep(5);
	//	if (ctmPre + cts <= tmNow)
	//	{
	//		ctmPre = tmNow;

	//		CString strUpdateTime = _T("");
	//		BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	//		BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//		BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//		UINT nPort = _ttoi(theApp.m_strKbasePort);

	//		CMutex g_mutexBasekindinfoTable(false, _T("mutex_kbase_basekindinfo_operator"));
	//		g_mutexBasekindinfoTable.Lock();

	//		if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	//		{
	//			SysFreeString(bstrID);
	//			SysFreeString(bstrUserName);
	//			SysFreeString(bstrUserPassword);
	//			AfxMessageBox(_T("数据库连接失败！"));
	//			bstrID = theApp.m_strKbaseId.AllocSysString();
	//			bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//			bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//			g_mutexBasekindinfoTable.Unlock();
	//			return FALSE;
	//		}
	//		SysFreeString(bstrID);
	//		SysFreeString(bstrUserName);
	//		SysFreeString(bstrUserPassword);

	//		struct KindStruct
	//		{
	//			CString strKindID;  //唯一标示，主键
	//			CString strKindCname;
	//			CString strKindFmPath;
	//			CString strKindDataPath;
	//			CString strPreUpdateTime;

	//		};
	//		CArray<KindStruct, KindStruct> KindStructArray;

	//		//得到需要下载的类型
	//		CString strSql = _T("SELECT pykm,c_name,SHOWLEVEL,PICTOTALPATHFL,PICTOTALPATHTL,DATAPATH  FROM BASEKINDINFO where updatekind=5");



	//		BSTR bstrSql = strSql.AllocSysString();
	//		UINT nRecord = 0;
	//		if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//		{
	//			SysFreeString(bstrSql);
	//			g_mutexBasekindinfoTable.Unlock();
	//			AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//			return 0;
	//		}
	//		SysFreeString(bstrSql);

	//		int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);

	//		if (iNoCount > 0)
	//		{
	//			CString strKindId, strBookId;
	//			UINT nState;
	//			do
	//			{
	//				KindStruct kindItem;
	//				kindItem.strKindID = CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 0));
	//				kindItem.strKindCname = CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 1));

	//				if (_ttoi(CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 2))) == 1)
	//				{
	//					kindItem.strKindFmPath = CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 4));
	//				}
	//				else
	//				{
	//					kindItem.strKindFmPath = CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 3));
	//				}

	//				kindItem.strKindDataPath = CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 5));
	//				kindItem.strPreUpdateTime = strUpdateTime;
	//				KindStructArray.Add(kindItem);
	//			} while (MoveNextEx(theApp.m_nConnectNo, nRecord));

	//		}

	//		CloseRecordSetEx(theApp.m_nConnectNo, nRecord);

	//		g_mutexBasekindinfoTable.Unlock();

	//		if (KindStructArray.GetSize() <= 0)
	//		{
	//			return 0;
	//		}

	//		////得到最后的更新日期
	//	
	//		CString strTempUrl;
	//		for (int i = KindStructArray.GetSize() - 1; i >= 0; i--)
	//		{
	//			//--kbase
	//			//if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, _T("DBOWN"), _T(""), pDlg->m_sKbasId, _T(""), 4567))
	//			//{
	//			//	AfxMessageBox(_T("数据库连接失败！"));
	//			//	return FALSE;
	//			//}

	//			//strSql.Format(_T("SELECT * FROM UPDATEINFO WHERE ID=%ld and kindid=%s and state>=2"), LnowWeek, KindStructArray.GetAt(i).strKindID);
	//			//bstrSql = strSql.AllocSysString();
	//			//UINT nRecord = 0;
	//			//SysFreeString(bstrSql);
	//			//if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//			//{
	//			//	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//			//	return 0;
	//			//}
	//			//int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
	//			//CloseRecordSetEx(theApp.m_nConnectNo, nRecord);

	//			//if (iNoCount > 0)
	//			//{
	//			//	KindStructArray.RemoveAt(i);
	//			//}
	//			//--sqlite
	//			strSql.Format(_T("SELECT ID FROM UPDATEINFO WHERE  kindid=\'%s\'"), KindStructArray.GetAt(i).strKindID);
	//			CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
	//			if (stmt != NULL && !stmt->IsNullStmt())
	//			{ 

	//				if (stmt->NextRow())
	//				{
	//					KindStructArray.GetAt(i).strPreUpdateTime = CString(stmt->ValueString(0));

	//				}
	//				delete stmt;
	//				stmt = NULL;

	//			}
	//			else
	//			{
	//			
	//				AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
	//				return FALSE;
	//			}


	//		}

	//		if (KindStructArray.GetSize() <= 0)
	//		{
	//			return 0;
	//		}


	//		//进行下载
	//		//	CString strTime = tmNow.Format(_T("%Y-%m-%d"));
	//		for (int i = 0; i < KindStructArray.GetSize(); i++)
	//		{
	//			CTime tmNow;
	//			tmNow = CTime::GetCurrentTime();
	//			CString strNowTime = tmNow.Format(_T("%Y-%m-%dT%H:%M:%S"));

	//			CString strResult;
	//			CString strTempUrl;

	//			//ttp://192.168.22.49:8042/api/Resource/GetSHFWList?MacNumber=5a9q-46jh-mmxb-4vmz-zjv3-hzqp3&kindname=C-2&datetime=2017-10-18
	//			strTempUrl.Format(_T("%s/api/Resource/GetSHFWList?datetime=%s&MacNumber=%s&kindname=%s"),
	//				theApp.m_strCnkiServerId, KindStructArray.GetAt(i).strPreUpdateTime, theApp.m_strRegestNo, KindStructArray.GetAt(i).strKindID);

	//			if (!theApp.GetStrFromWeb(strTempUrl, strResult) || strResult.IsEmpty())
	//			{
	//				continue;
	//			}

	//			Json::Reader reader;
	//			Json::Value getvalue;


	//			char* pbuf = new char[strResult.GetLength() * 2 + 1];
	//			CString2Char(strResult, pbuf);



	//			if (reader.parse(pbuf, getvalue, FALSE) && !theApp.m_bQuestClose)
	//			{
	//				delete[] pbuf;


	//				CString strError = string2CString(getvalue["EroorMessage"].asString());
	//				if (!strError.IsEmpty())
	//				{

	//					continue;

	//				}

	//				CString strGetID = string2CString(getvalue["SHFW_ModelCode"].asString());

	//				if (KindStructArray.GetAt(i).strKindID.CompareNoCase(strGetID) != 0)
	//				{

	//					continue;
	//				}

	//				CString strCname = string2CString(getvalue["SHFW_ModelName"].asString());

	//				BOOL bCNameChange = KindStructArray.GetAt(i).strKindCname.CompareNoCase(strCname) != 0;

	//				BOOL bKindChange = BOOL(_ttoi(string2CString(getvalue["ISChange"].asString())));



	//				if (bKindChange)
	//				{

	//					strSql.Format(_T("update \'%sBASEINFO\' set OPERATORKIND=0 "), KindStructArray.GetAt(i).strKindID);
	//					if (!theApp.m_sqlite.DirectStatement(strSql))
	//					{
	//						
	//						AfxMessageBox(_T("修正数据失败！") + strSql);
	//						return FALSE;
	//					}
	//					
	//				}



	//				BOOL bDataChange = BOOL(_ttoi(string2CString(getvalue["HaveData"].asString())));
	//				if (bDataChange == 0 && !bCNameChange)
	//				{

	//					continue;
	//				}



	//				Json::Value itemArr;

	//				itemArr = getvalue["SHFW_List"];
	//				if (bDataChange && itemArr.size() > 0)
	//				{

	//					struct ItemSruct
	//					{
	//						CString _psinfoid;  //唯一标示，主键
	//						CString _psinfotitle;  //文章标题
	//						CString _psinfohottitle;  //文章副标题
	//						CString _psinfosource;  //文章来源
	//						CString _psinfocontent; //文章内容    html  或者pdf文件路径
	//						CString _psinfotitleimage; //文章标题图片  用于列表左测显示
	//						CString  _psinfodate; //文章日期  
	//						CString _psinfotypeid; //文章分类编码  有分类编码对应表
	//						CString _psinfotypename; //文章分类名称
	//						CString _psinfoadduser; //文章添加人的账号
	//						CString _psinfoareaid; //添加人所属地区编号
	//						CString _psinfoopengrade; //文章开放级别   全国？本省？本市？
	//						CString _psinfostate; //文章状态   待审核？未通过？已通过？
	//						CString  _psbrowsetimes; //文章阅读次数
	//						CString  _psliketimes; //文章点赞次数
	//						CString _psinfocwords; //文章关键词
	//						CString _psinfoflag;  //标示
	//						CString _belongarg;  //所属系统（暂时不用管）
	//						CString _kindcode;  //所属种类
	//						CString _pupdateflag;//更新类型
	//					};

	//					int itemCnt = itemArr.size();
	//					BOOL bStart = FALSE;
	//					CArray<ItemSruct, ItemSruct> ItemStructArray;
	//					for (int j = 0; j < itemCnt; ++j)
	//					{
	//						ItemSruct IStructGet;

	//						IStructGet._psinfoid = string2CString(itemArr[j]["PSInfoID"].asString());

	//						IStructGet._psinfotitle = string2CString(itemArr[j]["PSInfoTitle"].asString());
	//						IStructGet._psinfotitle.Replace(_T("\""), _T("\\\""));

	//						IStructGet._psinfohottitle = string2CString(itemArr[j]["PSInfoHotTitle"].asString());
	//						IStructGet._psinfohottitle.Replace(_T("\""), _T("\\\""));

	//						IStructGet._psinfosource = string2CString(itemArr[j]["PSInfoSource"].asString());

	//						IStructGet._psinfocontent = string2CString(itemArr[j]["PSInfoContent"].asString());

	//						IStructGet._psinfotitleimage = string2CString(itemArr[j]["PSInfoTitleImage"].asString());

	//						IStructGet._psinfodate = string2CString(itemArr[j]["PSInfoDate"].asString());

	//						IStructGet._psinfotypeid = string2CString(itemArr[j]["PSInfoTypeID"].asString());

	//						IStructGet._psinfotypename = string2CString(itemArr[j]["PSInfoTypeName"].asString());

	//						IStructGet._psinfoadduser = string2CString(itemArr[j]["PSInfoAddUser"].asString());

	//						IStructGet._psinfoareaid = string2CString(itemArr[j]["PSInfoAreaID"].asString());

	//						IStructGet._psinfoopengrade = string2CString(itemArr[j]["PSInfoOpenGrade"].asString());

	//						IStructGet._psinfostate = string2CString(itemArr[j]["PSInfoState"].asString());

	//						IStructGet._psbrowsetimes = string2CString(itemArr[j]["PSBrowseTimes"].asString());

	//						IStructGet._psliketimes = string2CString(itemArr[j]["PSLikeTimes"].asString());

	//						IStructGet._psinfocwords = string2CString(itemArr[j]["PSInfoCWords"].asString());

	//						IStructGet._psinfoflag = string2CString(itemArr[j]["PSInfoFlag"].asString());

	//						IStructGet._belongarg = string2CString(itemArr[j]["BelongArg"].asString());

	//						IStructGet._kindcode = string2CString(itemArr[j]["KindCode"].asString());

	//						IStructGet._pupdateflag = string2CString(itemArr[j]["PUpdateFlag"].asString());


	//						ItemStructArray.Add(IStructGet);
	//					}

	//					if (ItemStructArray.GetSize() > 0 && !theApp.m_bQuestClose)
	//					{
	//						BOOL bKindDowloadOwer = FALSE;
	//						for (size_t j = 0; j < ItemStructArray.GetSize() && !theApp.m_bQuestClose; j++)
	//						{
	//							//kbase
	//							////判断是否需要下载（根据表内有无记录）
	//							//if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, _T("DBOWN"), _T(""), pDlg->m_sKbasId, _T(""), 4567))
	//							//{
	//							//	AfxMessageBox(_T("数据库连接失败！"));
	//							//	return FALSE;
	//							//}

	//							//strSql.Format(_T("SELECT * FROM UPDATEPARTINFO WHERE ID=%s and UPDATEDATEID=%ld and kindid=%s"), ItemStructArray.GetAt(j)._psinfoid, LnowWeek, KindStructArray.GetAt(i).strKindID);

	//							//bstrSql = strSql.AllocSysString();
	//							//UINT nRecord = 0;
	//							//SysFreeString(bstrSql);
	//							//if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//							//{
	//							//	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//							//	return 0;
	//							//}
	//							//int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
	//							//CloseRecordSetEx(theApp.m_nConnectNo, nRecord);

	//							//if (iNoCount > 0)
	//							//{
	//							//	continue;
	//							//}

	//							UINT nItemOperatorKind = 0;
	//							CString strFind = _T("PSInfoID=\'") + ItemStructArray.GetAt(j)._psinfoid + _T("\'");
	//							BOOL bExistInTable = pDlg->ItemIsInTable(KindStructArray.GetAt(i).strKindID + _T("BASEINFO"), strFind);


	//							if (ItemStructArray.GetAt(j)._pupdateflag == _T("delete"))
	//							{
	//								nItemOperatorKind = 1;

	//								if (!bExistInTable)
	//								{
	//									nItemOperatorKind = 0;
	//								}
	//							}
	//							else if (ItemStructArray.GetAt(j)._pupdateflag == _T("update"))
	//							{
	//								nItemOperatorKind = 2;

	//								if (!bExistInTable)
	//								{
	//									nItemOperatorKind = 3;
	//								}
	//							}
	//							else if (ItemStructArray.GetAt(j)._pupdateflag == _T("add"))
	//							{
	//								nItemOperatorKind = 3;
	//								if (bExistInTable)
	//								{
	//									nItemOperatorKind = 0;
	//								}
	//							}
	//							else
	//							{
	//								nItemOperatorKind = 0;
	//							}

	//							if (nItemOperatorKind == 0 || nItemOperatorKind == 2)
	//							{

	//								continue;
	//							}

	//							if (nItemOperatorKind == 3 && ItemStructArray.GetAt(j)._psinfoflag != _T("URL"))
	//							{
	//								//下载封面 并下载或者生成数据
	//								CString strUrl = ItemStructArray.GetAt(j)._psinfotitleimage;
	//								CString strFmPostFix = _T(".jpg");
	//								if (strUrl.ReverseFind('.') > 0)
	//								{
	//									strFmPostFix = strUrl.Right(strUrl.GetLength() - strUrl.ReverseFind('.'));
	//								}
	//								CString strDirPath = KindStructArray.GetAt(i).strKindFmPath + ItemStructArray.GetAt(j)._psinfoid + strFmPostFix;


	//								if (PathFileExists(strDirPath))
	//								{
	//									DeleteFile(strDirPath);
	//								}

	//								if (strUrl.GetLength() < 7 || strUrl.Left(7) != _T("http://"))
	//								{
	//									strUrl = _T("http://") + strUrl;
	//								}


	//								ULONGLONG lTemp = 0;
	//								UINT nResult = theApp.RequestHttp(strUrl, strDirPath, lTemp);
	//								if (nResult != RES_REQ_DONE_OK)
	//								{
	//									DeleteFile(strDirPath);

	//									if (nResult == RES_REQ_ERROR_NOSPACE)
	//									{
	//										AfxMessageBox(_T(" 空间不足！  文件  ") + strDirPath + _T("  存储失败！"), MB_OK | MB_SYSTEMMODAL);
	//									}
	//									continue;
	//								}

	//								strDirPath = KindStructArray.GetAt(i).strKindDataPath + ItemStructArray.GetAt(j)._psinfoid;

	//								if (!PathFileExists(KindStructArray.GetAt(i).strKindDataPath))
	//								{
	//									CreateDirectory(KindStructArray.GetAt(i).strKindDataPath, NULL);
	//								}

	//								if (ItemStructArray.GetAt(j)._psinfoflag == _T("文件"))
	//								{
	//									CString strSecrectPath = strDirPath;
	//									strUrl = ItemStructArray.GetAt(j)._psinfocontent;

	//									if (strUrl.GetLength() < 7 || strUrl.Left(7) != _T("http://"))
	//									{
	//										strUrl = _T("http://") + strUrl;
	//									}


	//									strDirPath += strUrl.Right(strUrl.GetLength() - strUrl.ReverseFind('.'));
	//									if (PathFileExists(strDirPath))
	//									{
	//										DeleteFile(strDirPath);
	//									}
	//									ULONGLONG lTemp = 0;
	//									UINT nResult = theApp.RequestHttp(strUrl, strDirPath, lTemp);
	//									if (nResult != RES_REQ_DONE_OK)
	//									{
	//										DeleteFile(strDirPath);

	//										if (nResult == RES_REQ_ERROR_NOSPACE)
	//										{
	//											AfxMessageBox(_T(" 空间不足！  文件  ") + strDirPath + _T("  存储失败！"), MB_OK | MB_SYSTEMMODAL);
	//										}
	//										continue;
	//									}
	//									else
	//									{
	//										if (PathFileExists(strSecrectPath))
	//										{
	//											DeleteFile(strSecrectPath);
	//										}
	//										pDlg->SecrectData(strDirPath, strSecrectPath);

	//										if (PathFileExists(strDirPath))
	//										{
	//											DeleteFile(strDirPath);
	//										}

	//									}
	//								}
	//								else
	//								{
	//									if (!PathFileExists(KindStructArray.GetAt(i).strKindDataPath + _T("js")))
	//									{
	//										CreateDirectory(KindStructArray.GetAt(i).strKindDataPath + _T("js"), NULL);
	//									}

	//									if (!PathFileExists(KindStructArray.GetAt(i).strKindDataPath + _T("js\\noresize.js")))
	//									{
	//										CopyFile(theApp.m_strCanChangePath + _T("\\html\\js\\noresize.js"), KindStructArray.GetAt(i).strKindDataPath + _T("js\\noresize.js"), false);
	//									}
	//									if (!PathFileExists(KindStructArray.GetAt(i).strKindDataPath + _T("js\\jquery-1.10.2.min.js")))
	//									{
	//										CopyFile(theApp.m_strCanChangePath + _T("\\html\\js\\jquery-1.10.2.min.js"), KindStructArray.GetAt(i).strKindDataPath + _T("js\\jquery-1.10.2.min.js"), false);
	//									}

	//									strDirPath += _T(".html");
	//									if (PathFileExists(strDirPath))
	//									{
	//										DeleteFile(strDirPath);
	//									}
	//									CString strUrlTemp = ItemStructArray.GetAt(j)._psinfocontent;
	//									if (strUrlTemp.GetLength() < 7 || strUrlTemp.Left(7) != _T("http://"))
	//									{
	//										strUrlTemp = _T("http://") + strUrlTemp;
	//									}
	//									CString strContent;
	//									if (!theApp.GetStrFromWeb(strUrlTemp, strContent) || !InserterHtml(theApp.m_strCanChangePath + _T("\\html\\emptyforchange.html"), strDirPath, strContent))
	//									{
	//										continue;
	//									}
	//								}

	//							}



	//							//sqlite
	//							//修改信息表

	//							if (nItemOperatorKind == 3)
	//							{
	//								CString strImgeName = ItemStructArray.GetAt(j)._psinfotitleimage;
	//								strImgeName = strImgeName.Right(strImgeName.GetLength() - strImgeName.ReverseFind('/') - 1);

	//								strSql.Format(_T("INSERT INTO [%sBASEINFO] "), KindStructArray.GetAt(i).strKindID);
	//								strSql += _T("([PSInfoID],[PSInfoTitle],[PSInfoHotTitle],[PSInfoSource],[PSInfoContent],[PSInfoTitleImage],[PSInfoDate],[PSInfoTypeID],[PSInfoTypeName],[PSInfoAddUser],[PSInfoAreaID],[PSInfoOpenGrade],[PSInfoState],[PSBrowseTimes],[PSLikeTimes],[PSInfoCWords],[PSInfoFlag],[BelongArg],[KindCode],[OPERATORKIND]) ");
	//								strSql.AppendFormat(_T("VALUES(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%d)"),
	//									ItemStructArray.GetAt(j)._psinfoid, ItemStructArray.GetAt(j)._psinfotitle, ItemStructArray.GetAt(j)._psinfohottitle, ItemStructArray.GetAt(j)._psinfosource, _T(""),
	//									strImgeName, ItemStructArray.GetAt(j)._psinfodate, ItemStructArray.GetAt(j)._psinfotypeid, ItemStructArray.GetAt(j)._psinfotypename,
	//									ItemStructArray.GetAt(j)._psinfoadduser, ItemStructArray.GetAt(j)._psinfoareaid, ItemStructArray.GetAt(j)._psinfoopengrade, ItemStructArray.GetAt(j)._psinfostate,
	//									ItemStructArray.GetAt(j)._psbrowsetimes, ItemStructArray.GetAt(j)._psliketimes, ItemStructArray.GetAt(j)._psinfocwords, ItemStructArray.GetAt(j)._psinfoflag,
	//									ItemStructArray.GetAt(j)._belongarg, KindStructArray.GetAt(i).strKindID, 1);

	//								if (!theApp.m_sqlite.DirectStatement(strSql))
	//								{
	//								
	//									WriteLog_shfw(strSql);
	//									AfxMessageBox(_T("插入数据失败！") + strSql);
	//									return FALSE;
	//								}
	//							}
	//							else if (nItemOperatorKind == 1)
	//							{
	//								strSql.Format(_T("update \'%sBASEINFO\' set OPERATORKIND=0 WHERE PSInfoID=\'%s\' "), KindStructArray.GetAt(i).strKindID, ItemStructArray.GetAt(j)._psinfoid);

	//								if (!theApp.m_sqlite.DirectStatement(strSql))
	//								{
	//								
	//									AfxMessageBox(_T("修正数据失败！") + strSql);
	//									return FALSE;
	//								}
	//							}

	//						

	//						}
	//					}
	//				}

	//				if (bCNameChange)//修改名字(是否需要lock)
	//				{

	//					CMutex g_mutexBasekindinfoTable(false, _T("mutex_kbase_basekindinfo_operator"));
	//					g_mutexBasekindinfoTable.Lock();

	//					BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	//					BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//					BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//					UINT nPort = _ttoi(theApp.m_strKbasePort);

	//					if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	//					{
	//						SysFreeString(bstrID);
	//						SysFreeString(bstrUserName);
	//						SysFreeString(bstrUserPassword);
	//						g_mutexBasekindinfoTable.Unlock();
	//						AfxMessageBox(_T("数据库连接失败！"));
	//						bstrID = theApp.m_strKbaseId.AllocSysString();
	//						bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//						bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//				
	//						return FALSE;
	//					}
	//					SysFreeString(bstrID);
	//					SysFreeString(bstrUserName);
	//					SysFreeString(bstrUserPassword);


	//					strSql.Format(_T("SELECT * FROM BASEKINDINFO WHERE PYKM=\'%s\' "), KindStructArray.GetAt(i).strKindID);
	//					bstrSql = strSql.AllocSysString();
	//					UINT nRecord = 0;
	//					SysFreeString(bstrSql);

	//					if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//					{
	//						g_mutexBasekindinfoTable.Unlock();
	//						AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//						return 0;
	//					}
	//					int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);

	//					if (iNoCount > 0)
	//					{
	//						BSTR bCname = strCname.AllocSysString();
	//						SetFieldValueEx(theApp.m_nConnectNo, nRecord, bCname, 1);
	//						SysFreeString(bCname);
	//					}
	//					CloseRecordSetEx(theApp.m_nConnectNo, nRecord);

	//					g_mutexBasekindinfoTable.Unlock();
	//				}


	//				strSql.Format(_T("SELECT * FROM UPDATEINFO WHERE  kindid=\'%s\' "), KindStructArray.GetAt(i).strKindID);

	//				CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
	//				if (stmt != NULL&& !stmt->IsNullStmt())
	//				{
	//					bool bExist = false;
	//					if (stmt->NextRow())
	//					{
	//						bExist = true;
	//					}
	//					delete stmt;
	//					stmt = NULL;

	//					UINT nState = 2;


	//					if (bExist)
	//					{
	//						strSql.Format(_T("update UPDATEINFO set ID=\'%s\',state=\'%d\' WHERE  kindid=\'%s\' "), strNowTime, nState, KindStructArray.GetAt(i).strKindID);
	//						if (!theApp.m_sqlite.DirectStatement(strSql))
	//						{
	//							
	//							AfxMessageBox(_T("修正数据失败！") + strSql);
	//							return FALSE;
	//						}
	//					}
	//					else
	//					{
	//						strSql = _T("INSERT INTO [UPDATEINFO] ([ID],[STATE],[DOWNLPART],[KINDID])   ");
	//						strSql.AppendFormat(_T("VALUES(\'%s\',\'%d\','0',\'%s\')"),
	//							strNowTime, nState, KindStructArray.GetAt(i).strKindID);
	//						if (!theApp.m_sqlite.DirectStatement(strSql))
	//						{
	//							
	//							AfxMessageBox(_T("插入数据失败！") + strSql);
	//							return FALSE;
	//						}
	//					}

	//				}
	//				else
	//				{
	//					
	//					AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
	//					return FALSE;
	//				}
	//			


	//			}




	//		}
	//	}

	//	tmNow = CTime::GetCurrentTime();
	//}

	return 1;
}

DWORD WINAPI CMonitorDlg::ThreadGetUpdateInfoAndDataEx_ZJZY(LPVOID lpParame)
{
	//	CMonitorDlg* pDlg = (CMonitorDlg*)lpParame;
	//
	//	CTime tmNow;
	//	tmNow = CTime::GetCurrentTime();
	//	CTime  ctmPre = tmNow;
	//	ULONG LnowWeek = _ttol(ctmPre.Format(_T("%Y%W")));
	//
	//	BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	//	BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//	BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//	UINT nPort = _ttoi(theApp.m_strKbasePort);
	//
	//	CMutex g_mutexBasekindinfoTable(false, _T("mutex_kbase_basekindinfo_operator"));
	//	g_mutexBasekindinfoTable.Lock();
	//
	//	if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	//	{
	//		SysFreeString(bstrID);
	//		SysFreeString(bstrUserName);
	//		SysFreeString(bstrUserPassword);
	//		AfxMessageBox(_T("数据库连接失败！"));
	//		bstrID = theApp.m_strKbaseId.AllocSysString();
	//		bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//		bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//		g_mutexBasekindinfoTable.Unlock();
	//		return FALSE;
	//	}
	//	SysFreeString(bstrID);
	//	SysFreeString(bstrUserName);
	//	SysFreeString(bstrUserPassword);
	//
	//	struct KindStruct
	//	{
	//		CString strKindID;  //唯一标示，主键
	//		CString strKindCname;  
	//		CString strKindFmPath;  
	//		CString strKindDataPath;  
	//
	//	};
	//	CArray<KindStruct, KindStruct> KindStructArray;
	//
	//	//得到需要下载的类型
	//	CString strSql = _T("SELECT pykm,c_name,SHOWLEVEL,PICTOTALPATHFL,PICTOTALPATHTL,DATAPATH  FROM BASEKINDINFO where updatekind=2");
	//
	//
	//	
	//	BSTR bstrSql = strSql.AllocSysString();
	//	UINT nRecord = 0;
	//	if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//	{
	//		SysFreeString(bstrSql);
	//		g_mutexBasekindinfoTable.Unlock();
	//		AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//		return 0;
	//	}
	//	SysFreeString(bstrSql);
	//
	//	int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
	//
	//	if (iNoCount>0)
	//	{
	//		CString strKindId, strBookId;
	//		UINT nState;
	//		do
	//		{
	//			KindStruct kindItem;
	//			kindItem.strKindID=CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 0));
	//			kindItem.strKindCname=CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 1));
	//
	//			if (_ttoi(CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 2))) == 1)
	//			{
	//				kindItem.strKindFmPath=CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 4));
	//			}
	//			else
	//			{
	//				kindItem.strKindFmPath=CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 3));
	//			}
	//
	//			kindItem.strKindDataPath = CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 5));
	//
	//			KindStructArray.Add(kindItem);
	//		} while (MoveNextEx(theApp.m_nConnectNo, nRecord));
	//
	//	}
	//
	//	CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
	//	g_mutexBasekindinfoTable.Unlock();
	//
	//	if (KindStructArray.GetSize() <= 0)
	//	{
	//		return 0;
	//	}
	//
	//	//得到当前日期是否已经下载完成
	//
	//	CString strTempUrl;
	//	for (int i = KindStructArray.GetSize()-1; i >= 0; i--)
	//	{
	////--kbase
	//		//if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, _T("DBOWN"), _T(""), pDlg->m_sKbasId, _T(""), 4567))
	//		//{
	//		//	AfxMessageBox(_T("数据库连接失败！"));
	//		//	return FALSE;
	//		//}
	//
	//		//strSql.Format(_T("SELECT * FROM UPDATEINFO WHERE ID=%ld and kindid=%s and state>=2"), LnowWeek, KindStructArray.GetAt(i).strKindID);
	//		//bstrSql = strSql.AllocSysString();
	//		//UINT nRecord = 0;
	//		//SysFreeString(bstrSql);
	//		//if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//		//{
	//		//	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//		//	return 0;
	//		//}
	//		//int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
	//		//CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
	//
	//		//if (iNoCount > 0)
	//		//{
	//		//	KindStructArray.RemoveAt(i);
	//		//}
	////--sqlite
	//		strSql.Format(_T("SELECT * FROM UPDATEINFO WHERE ID=\'%ld\' and kindid=\'%s\' and state>=2"), LnowWeek, KindStructArray.GetAt(i).strKindID);
	//		CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
	//		if (stmt != NULL && !stmt->IsNullStmt())
	//		{
	//			bool bExist = false;
	//			if (stmt->NextRow())
	//			{
	//				bExist = true;
	//			}
	//			delete stmt;
	//			stmt = NULL;
	//
	//			if (bExist)
	//			{
	//				KindStructArray.RemoveAt(i);
	//			}
	//
	//		}
	//		else
	//		{
	//	
	//			AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
	//			return FALSE;
	//		}
	//
	//
	//	}
	//
	//	if (KindStructArray.GetSize() <= 0)
	//	{
	//		return 0;
	//	}
	//
	//
	//
	//	//进行下载
	//	CString strTime = tmNow.Format(_T("%Y-%m-%d"));
	//	for (int i = 0; i < KindStructArray.GetSize(); i++)
	//	{
	//		CString strResult;
	//		strTempUrl.Format(_T("%s/api/Resource/GetResourceListByWeek?datetime=%s&MacNumber=%s&bdzttypename=%s&typeflag=%s&typename=%s"), 
	//			theApp.m_strCnkiServerId, strTime, theApp.m_strRegestNo, theApp.m_DecTemp.UrlEncode(KindStructArray.GetAt(i).strKindCname), KindStructArray.GetAt(i).strKindID,theApp.m_strKindEname);
	//
	//
	//		if (!theApp.GetStrFromWeb(strTempUrl, strResult) || strResult.IsEmpty())
	//		{
	//			continue;
	//		}
	//
	//		Json::Reader reader;
	//		Json::Value itemArr;
	//
	//	
	//		char* pbuf = new char[strResult.GetLength()*2+1];
	//		CString2Char(strResult, pbuf);
	//
	//
	//		if (reader.parse(pbuf, itemArr, FALSE) && !theApp.m_bQuestClose)
	//		{
	//			delete[] pbuf;
	//
	//			if (itemArr.size()>0)
	//			{
	//
	//				struct ItemSruct
	//				{
	//					CString _psinfoid;  //唯一标示，主键
	//					CString _psinfotitle;  //文章标题
	//					CString _psinfohottitle;  //文章副标题
	//					CString _psinfosource;  //文章来源
	//					CString _psinfocontent; //文章内容    html  或者pdf文件路径
	//					CString _psinfotitleimage; //文章标题图片  用于列表左测显示
	//					CString  _psinfodate; //文章日期  
	//					CString _psinfotypeid; //文章分类编码  有分类编码对应表
	//					CString _psinfotypename; //文章分类名称
	//					CString _psinfoadduser; //文章添加人的账号
	//					CString _psinfoareaid; //添加人所属地区编号
	//					CString _psinfoopengrade; //文章开放级别   全国？本省？本市？
	//					CString _psinfostate; //文章状态   待审核？未通过？已通过？
	//					CString  _psbrowsetimes; //文章阅读次数
	//					CString  _psliketimes; //文章点赞次数
	//					CString _psinfocwords; //文章关键词
	//					CString _psinfoflag;  //标示
	//					CString _belongarg;  //所属系统（暂时不用管）
	//					CString _kindcode;  //所属种类
	//				};
	//
	//				int itemCnt = itemArr.size();
	//				BOOL bStart = FALSE;
	//				CArray<ItemSruct, ItemSruct> ItemStructArray;
	//				for (int j = 0; j < itemCnt; ++j)
	//				{
	//					ItemSruct IStructGet;
	//
	//					IStructGet._psinfoid = string2CString(itemArr[j]["PSInfoID"].asString());
	//
	//					IStructGet._psinfotitle = string2CString(itemArr[j]["PSInfoTitle"].asString());
	//					IStructGet._psinfotitle.Replace(_T("\""), _T("\\\""));
	//
	//					IStructGet._psinfohottitle = string2CString(itemArr[j]["PSInfoHotTitle"].asString());
	//					IStructGet._psinfohottitle.Replace(_T("\""), _T("\\\""));
	//
	//					IStructGet._psinfosource = string2CString(itemArr[j]["PSInfoSource"].asString());
	//
	//					IStructGet._psinfocontent = string2CString(itemArr[j]["PSInfoContent"].asString());
	//
	//					IStructGet._psinfotitleimage = string2CString(itemArr[j]["PSInfoTitleImage"].asString());
	//
	//					IStructGet._psinfodate = string2CString(itemArr[j]["PSInfoDate"].asString());
	//
	//					IStructGet._psinfotypeid = string2CString(itemArr[j]["PSInfoTypeID"].asString());
	//
	//					IStructGet._psinfotypename = string2CString(itemArr[j]["PSInfoTypeName"].asString());
	//
	//					IStructGet._psinfoadduser = string2CString(itemArr[j]["PSInfoAddUser"].asString());
	//
	//					IStructGet._psinfoareaid = string2CString(itemArr[j]["PSInfoAreaID"].asString());
	//
	//					IStructGet._psinfoopengrade = string2CString(itemArr[j]["PSInfoOpenGrade"].asString());
	//
	//					IStructGet._psinfostate = string2CString(itemArr[j]["PSInfoState"].asString());
	//
	//					IStructGet._psbrowsetimes = string2CString(itemArr[j]["PSBrowseTimes"].asString());
	//
	//					IStructGet._psliketimes = string2CString(itemArr[j]["PSLikeTimes"].asString());
	//
	//					IStructGet._psinfocwords = string2CString(itemArr[j]["PSInfoCWords"].asString());
	//
	//					IStructGet._psinfoflag = string2CString(itemArr[j]["PSInfoFlag"].asString());
	//
	//					IStructGet._belongarg = string2CString(itemArr[j]["BelongArg"].asString());
	//
	//					IStructGet._kindcode = string2CString(itemArr[j]["KindCode"].asString());
	//
	//					ItemStructArray.Add(IStructGet);
	//				}
	//
	//				if (ItemStructArray.GetSize()>0 && !theApp.m_bQuestClose)
	//				{
	//					BOOL bKindDowloadOwer = FALSE;
	//
	//
	//					for (size_t j = 0; j < ItemStructArray.GetSize() && !theApp.m_bQuestClose; j++)
	//					{
	////kbase
	//						////判断是否需要下载（根据表内有无记录）
	//						//if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, _T("DBOWN"), _T(""), pDlg->m_sKbasId, _T(""), 4567))
	//						//{
	//						//	AfxMessageBox(_T("数据库连接失败！"));
	//						//	return FALSE;
	//						//}
	//
	//						//strSql.Format(_T("SELECT * FROM UPDATEPARTINFO WHERE ID=%s and UPDATEDATEID=%ld and kindid=%s"), ItemStructArray.GetAt(j)._psinfoid, LnowWeek, KindStructArray.GetAt(i).strKindID);
	//
	//						//bstrSql = strSql.AllocSysString();
	//						//UINT nRecord = 0;
	//						//SysFreeString(bstrSql);
	//						//if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//						//{
	//						//	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//						//	return 0;
	//						//}
	//						//int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
	//						//CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
	//
	//						//if (iNoCount > 0)
	//						//{
	//						//	continue;
	//						//}
	//					
	////sqlite                 
	//						strSql.Format(_T("SELECT * FROM UPDATEPARTINFO WHERE ID=\'%s\' and UPDATEDATEID=\'%ld\' and kindid=\'%s\'"), ItemStructArray.GetAt(j)._psinfoid, LnowWeek, KindStructArray.GetAt(i).strKindID);
	//
	//						CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
	//						if (stmt != NULL&& !stmt->IsNullStmt())
	//						{
	//							bool bExist = false;
	//							if (stmt->NextRow())
	//							{
	//								bExist = true;
	//							}
	//							delete stmt;
	//							stmt = NULL;
	//
	//							if (bExist)
	//							{
	//							
	//								continue;
	//							}
	//
	//						}
	//						else
	//						{
	//						
	//							AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
	//							return FALSE;
	//						}
	//			
	//				
	//
	//						if (ItemStructArray.GetAt(j)._psinfoflag != _T("URL"))
	//						{
	//							//下载封面 并下载或者生成数据
	//							CString strUrl = ItemStructArray.GetAt(j)._psinfotitleimage;
	//							CString strFmPostFix = _T(".jpg");
	//							if (strUrl.ReverseFind('.')>0)
	//							{
	//								strFmPostFix = strUrl.Right(strUrl.GetLength() - strUrl.ReverseFind('.'));
	//							}
	//							CString strDirPath = KindStructArray.GetAt(i).strKindFmPath + ItemStructArray.GetAt(j)._psinfoid + strFmPostFix;
	//					
	//
	//							if (PathFileExists(strDirPath))
	//							{
	//								DeleteFile(strDirPath);
	//							}
	//
	//							if (strUrl.GetLength()<7 || strUrl.Left(7) != _T("http://"))
	//							{
	//								strUrl = _T("http://") + strUrl;
	//							}
	//
	//
	//							ULONGLONG lTemp = 0;
	//							UINT nResult = theApp.RequestHttp(strUrl, strDirPath, lTemp);
	//							if (nResult != RES_REQ_DONE_OK)
	//							{
	//								DeleteFile(strDirPath);
	//
	//								if (nResult == RES_REQ_ERROR_NOSPACE)
	//								{
	//									AfxMessageBox(_T(" 空间不足！  文件  ") + strDirPath + _T("  存储失败！"), MB_OK | MB_SYSTEMMODAL);
	//								}
	//								continue;
	//							}
	//
	//							strDirPath = KindStructArray.GetAt(i).strKindDataPath + ItemStructArray.GetAt(j)._psinfoid;
	//
	//							if (!PathFileExists(KindStructArray.GetAt(i).strKindDataPath))
	//							{
	//								CreateDirectory(KindStructArray.GetAt(i).strKindDataPath, NULL);
	//							}
	//
	//							if (ItemStructArray.GetAt(j)._psinfoflag == _T("文件"))
	//							{
	//								CString strSecrectPath = strDirPath;
	//								strUrl = ItemStructArray.GetAt(j)._psinfocontent;
	//
	//								if (strUrl.GetLength()<7 || strUrl.Left(7) != _T("http://"))
	//								{
	//									strUrl = _T("http://") + strUrl;
	//								}
	//
	//
	//								strDirPath += strUrl.Right(strUrl.GetLength() - strUrl.ReverseFind('.'));
	//								if (PathFileExists(strDirPath))
	//								{
	//									DeleteFile(strDirPath);
	//								}
	//								ULONGLONG lTemp = 0;
	//								UINT nResult = theApp.RequestHttp(strUrl, strDirPath, lTemp);
	//								if (nResult != RES_REQ_DONE_OK)
	//								{
	//									DeleteFile(strDirPath);
	//
	//									if (nResult == RES_REQ_ERROR_NOSPACE)
	//									{
	//										AfxMessageBox(_T(" 空间不足！  文件  ") + strDirPath + _T("  存储失败！"), MB_OK | MB_SYSTEMMODAL);
	//									}
	//									continue;
	//								}
	//								else
	//								{
	//									if (PathFileExists(strSecrectPath))
	//									{
	//										DeleteFile(strSecrectPath);
	//									}
	//									pDlg->SecrectData(strDirPath, strSecrectPath);
	//
	//									if (PathFileExists(strDirPath))
	//									{
	//										DeleteFile(strDirPath);
	//									}
	//
	//								}
	//							}
	//							else
	//							{
	//								if (!PathFileExists(KindStructArray.GetAt(i).strKindDataPath+_T("js")))
	//								{
	//									CreateDirectory(KindStructArray.GetAt(i).strKindDataPath + _T("js"), NULL);
	//								}
	//
	//								if (!PathFileExists(KindStructArray.GetAt(i).strKindDataPath + _T("js\\noresize.js")))
	//								{
	//									CopyFile(theApp.m_strCanChangePath + _T("\\html\\js\\noresize.js"), KindStructArray.GetAt(i).strKindDataPath + _T("js\\noresize.js"), false);
	//								}
	//								if (!PathFileExists(KindStructArray.GetAt(i).strKindDataPath + _T("js\\jquery-1.10.2.min.js")))
	//								{
	//									CopyFile(theApp.m_strCanChangePath + _T("\\html\\js\\jquery-1.10.2.min.js"), KindStructArray.GetAt(i).strKindDataPath + _T("js\\jquery-1.10.2.min.js"), false);
	//								}
	//
	//								strDirPath += _T(".html");
	//								if (PathFileExists(strDirPath))
	//								{
	//									DeleteFile(strDirPath);
	//								}
	//								CString strUrlTemp = ItemStructArray.GetAt(j)._psinfocontent;
	//								if (strUrlTemp.GetLength()<7 || strUrlTemp.Left(7) != _T("http://"))
	//								{
	//									strUrlTemp = _T("http://") + strUrlTemp;
	//								}
	//								CString strContent;
	//								if (!theApp.GetStrFromWeb(strUrlTemp, strContent) || !InserterHtml(theApp.m_strCanChangePath + _T("\\html\\emptyforchange.html"), strDirPath, strContent))
	//								{
	//									continue;
	//								}
	//							}
	//
	//						}
	//					
	//
	////kbase
	//						////插入信息表
	//						//CMutex g_mutex(false, _T("mutex_updatetableZJZY"));
	//						//g_mutex.Lock();
	//						//if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, _T("DBOWN"), _T(""), pDlg->m_sKbasId, _T(""), 4567))
	//						//{
	//						//	AfxMessageBox(_T("数据库连接失败！"));
	//						//	return FALSE;
	//						//}
	//
	//						//strSql.Format(_T("SELECT * FROM bdzt WHERE ID=%s "), ItemStructArray.GetAt(j)._psinfoid);
	//						//bstrSql = strSql.AllocSysString();
	//						//nRecord = 0;
	//						//SysFreeString(bstrSql);
	//						//if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//						//{
	//						//	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//						//	return 0;
	//						//}
	//						//iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
	//						//CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
	//
	//						//if (iNoCount == 0)
	//						//{
	//						//	strSql = _T("INSERT INTO bdzt (ID,UPDATEDATEID,STATE,DOWNLPART,KINDID) ");
	//						//	strSql.AppendFormat(_T("VALUES(\'%s\',\'%ld\',2,0,\'%s\')"), ItemStructArray.GetAt(j)._psinfoid, LnowWeek, KindStructArray.GetAt(i).strKindID);
	//						//	ExecSQL(theApp.m_nConnectNo, strSql);
	//						//}
	//
	//						//g_mutex.Unlock();
	//
	////sqlite
	//						//插入信息表
	//
	//						strSql.Format(_T("SELECT * FROM \'%sBASEINFO\' WHERE PSInfoID=\'%s\' "), KindStructArray.GetAt(i).strKindID,ItemStructArray.GetAt(j)._psinfoid);
	//
	//						stmt = theApp.m_sqlite.Statement(strSql);
	//						if (stmt != NULL&& !stmt->IsNullStmt())
	//						{
	//							bool bExist = false;
	//							if (stmt->NextRow())
	//							{
	//								bExist = true;
	//							}
	//							delete stmt;
	//							stmt = NULL;
	//
	//							if (!bExist)
	//							{
	//								CString strImgeName = ItemStructArray.GetAt(j)._psinfotitleimage;
	//								strImgeName = strImgeName.Right(strImgeName.GetLength() - strImgeName.ReverseFind('/') - 1);
	//								strSql.Format(_T("INSERT INTO [%sBASEINFO] "), KindStructArray.GetAt(i).strKindID);
	//								strSql += _T("([PSInfoID],[PSInfoTitle],[PSInfoHotTitle],[PSInfoSource],[PSInfoContent],[PSInfoTitleImage],[PSInfoDate],[PSInfoTypeID],[PSInfoTypeName],[PSInfoAddUser],[PSInfoAreaID],[PSInfoOpenGrade],[PSInfoState],[PSBrowseTimes],[PSLikeTimes],[PSInfoCWords],[PSInfoFlag],[BelongArg],[KindCode]) ");
	//								strSql.AppendFormat(_T("VALUES(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\')"),
	//									ItemStructArray.GetAt(j)._psinfoid, ItemStructArray.GetAt(j)._psinfotitle, ItemStructArray.GetAt(j)._psinfohottitle, ItemStructArray.GetAt(j)._psinfosource, _T(""),
	//									strImgeName, ItemStructArray.GetAt(j)._psinfodate, ItemStructArray.GetAt(j)._psinfotypeid, ItemStructArray.GetAt(j)._psinfotypename,
	//									ItemStructArray.GetAt(j)._psinfoadduser, ItemStructArray.GetAt(j)._psinfoareaid, ItemStructArray.GetAt(j)._psinfoopengrade, ItemStructArray.GetAt(j)._psinfostate,
	//									ItemStructArray.GetAt(j)._psbrowsetimes, ItemStructArray.GetAt(j)._psliketimes, ItemStructArray.GetAt(j)._psinfocwords, ItemStructArray.GetAt(j)._psinfoflag,
	//									ItemStructArray.GetAt(j)._belongarg, ItemStructArray.GetAt(j)._kindcode);
	//
	//								if (!theApp.m_sqlite.DirectStatement(strSql))
	//								{
	//									
	//									AfxMessageBox(_T("插入数据失败！")+strSql);
	//									return FALSE;
	//								}
	//							}
	//
	//						}
	//						else
	//						{
	//						
	//							AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
	//							return FALSE;
	//						}
	//					
	//
	//
	//						//修正下载状态（插入记录）
	//						strSql = _T("INSERT INTO [UPDATEPARTINFO] ([ID],[UPDATEDATEID],[DOWNLPART],[KINDID])  ");
	//						strSql.AppendFormat(_T("VALUES(\'%s\',\'%ld\','0',\'%s\')"),
	//							ItemStructArray.GetAt(j)._psinfoid, LnowWeek, KindStructArray.GetAt(i).strKindID);
	//						if (!theApp.m_sqlite.DirectStatement(strSql))
	//						{
	//		
	//							AfxMessageBox(_T("插入数据失败！") + strSql);
	//							return FALSE;
	//						}
	//				
	//					}
	//
	//					//修正 周来下载状态
	//
	////--kbase
	//					//if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, _T("DBOWN"), _T(""), pDlg->m_sKbasId, _T(""), 4567))
	//					//{
	//					//	AfxMessageBox(_T("数据库连接失败！"));
	//					//	return FALSE;
	//					//}
	//
	//
	//					//strSql.Format(_T("SELECT * FROM UPDATEINFO WHERE ID=%ld and kindid=%s "), LnowWeek, KindStructArray.GetAt(i).strKindID);
	//					//bstrSql = strSql.AllocSysString();
	//					//UINT nRecord = 0;
	//					//SysFreeString(bstrSql);
	//					//if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	//					//{
	//					//	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	//					//	return 0;
	//					//}
	//					//int iNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
	//					////CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
	//
	//					//if (iNoCount>0)
	//					//{
	//					//	if (bKindDowloadOwer)
	//					//	{
	//					//		//修正本周的下载状态 2
	//					//		SetFieldValueEx(theApp.m_nConnectNo, nRecord, _T("2"), 1);
	//
	//					//	}
	//					//	else
	//					//	{
	//					//		//修正本周的下载状态1
	//					//		SetFieldValueEx(theApp.m_nConnectNo, nRecord, _T("1"), 1);
	//					//	}
	//
	//					//	CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
	//					//}
	//					//else
	//					//{
	//					//	CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
	//					//	if (bKindDowloadOwer)
	//					//	{
	//					//		//修正本周的下载状态 2
	//					//		strSql = _T("INSERT INTO UPDATEINFO (ID,STATE,DOWNLPART,KINDID) ");
	//					//		strSql.AppendFormat(_T("VALUES(\'%ld\',2,0,\'%s\')"), LnowWeek, KindStructArray.GetAt(i).strKindID);
	//					//		ExecSQL(theApp.m_nConnectNo, strSql);
	//
	//					//	}
	//					//	else
	//					//	{
	//					//		//修正本周的下载状态1
	//					//		strSql = _T("INSERT INTO UPDATEINFO (ID,STATE,DOWNLPART,KINDID) ");
	//					//		strSql.AppendFormat(_T("VALUES(\'%ld\',1,0,\'%s\')"), LnowWeek, KindStructArray.GetAt(i).strKindID);
	//					//		ExecSQL(theApp.m_nConnectNo, strSql);
	//					//	}
	//					//}
	//
	//
	////--sqlite
	//				
	//					strSql.Format(_T("SELECT * FROM UPDATEINFO WHERE ID=\'%ld\' and kindid=\'%s\' "), LnowWeek, KindStructArray.GetAt(i).strKindID);
	//
	//					CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
	//					if (stmt != NULL&& !stmt->IsNullStmt())
	//					{
	//						bool bExist = false;
	//						if (stmt->NextRow())
	//						{
	//							bExist = true;
	//						}
	//						delete stmt;
	//						stmt = NULL;
	//
	//						UINT nState = 1;
	//						if (bKindDowloadOwer) nState = 2;
	//						
	//						if (bExist)
	//						{
	//							strSql.Format(_T("update UPDATEINFO set state=\'%d\' WHERE ID=\'%ld\' and kindid=\'%s\' "), nState, LnowWeek, KindStructArray.GetAt(i).strKindID);
	//							if (!theApp.m_sqlite.DirectStatement(strSql))
	//							{
	//								
	//								AfxMessageBox(_T("修正数据失败！") + strSql);
	//								return FALSE;
	//							}
	//						}
	//						else
	//						{
	//							strSql = _T("INSERT INTO [UPDATEINFO] ([ID],[STATE],[DOWNLPART],[KINDID])   ");
	//							strSql.AppendFormat(_T("VALUES(\'%ld\',\'%d\','0',\'%s\')"),
	//								LnowWeek, nState, KindStructArray.GetAt(i).strKindID);
	//							if (!theApp.m_sqlite.DirectStatement(strSql))
	//							{
	//
	//							
	//								AfxMessageBox(_T("插入数据失败！") + strSql);
	//								return FALSE;
	//							}
	//						}
	//
	//					}
	//					else
	//					{
	//						
	//						AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
	//						return FALSE;
	//					}
	//				
	//				}
	//			}
	//		}
	//
	//	}
	//
	//	
	return 1;
}

//DWORD WINAPI CMonitorDlg::ThreadGetUpdateInfoAndData(LPVOID lpParame)
//{
//	CMonitorDlg* pDlg=(CMonitorDlg*)lpParame;
//// 	while (true)
//// 	{
//// 		CString strSql=_T("select * from newcjbdlast");
//// 		UINT nRecordNo=0;
//// 		BOOL bNeedUpdate=FALSE;
//// 		BSTR bstrSql=strSql.AllocSysString();
//// 		SysFreeString(bstrSql);
//// 		if (OpenRecordSetEx(theApp.m_nConnectNo,nRecordNo,bstrSql) && GetRecordCountEx(theApp.m_nConnectNo,nRecordNo)>0)
//// 		{
//// 			bNeedUpdate=TRUE;
//// 		}
//// 		CloseRecordSetEx(theApp.m_nConnectNo,nRecordNo);
//// 
//// 		if (bNeedUpdate)
//// 		{
//// 			strSql=_T("BULKLOAD TABLE cjbdlast from table newcjbdlast");
//// 			if(!ExecSQL(theApp.m_nConnectNo,strSql))
//// 			{
//// 				AfxMessageBox(_T("执行失败")+strSql);
//// 			}
//// 		}
//	// 	}
//	int iHour = -1;
//	CTime tmNow = CTime::GetCurrentTime();
//
//	static int iYearNowLoad = tmNow.GetYear();
//
//	iYearNowLoad = min(iYearNowLoad, _ttoi(theApp.m_strBuyEndYear));
//
//	while (!theApp.m_bQuestClose)
//	{
//		Sleep(5);
//		if (iHour != tmNow.GetHour())
//		{
//
//			//得到种类信息
//			CString strUrl, strGetFromWeb;
//			strUrl.Format(_T("%s/tsdl/tsinfo.ashx?kind=userbuykindinfo&cid=%s"), theApp.m_strCnkiServerId, theApp.m_strRegestNo);
//			while (!theApp.GetStrFromWeb(strUrl, strGetFromWeb) && !theApp.m_bQuestClose);
//			CString strBuyKindGetFromServer;
//			if (!strGetFromWeb.IsEmpty() && strGetFromWeb.Left(4) != _T("Err:"))
//			{
//				strBuyKindGetFromServer = strGetFromWeb;
//			}
//			else
//			{
//				continue;
//			}
//
//
//			CString strIniPath = theApp.m_strCanChangePath+_T("\\xml\\buykind.ini");
//
//			if (!strGetFromWeb.IsEmpty() && strGetFromWeb.Left(4) != _T("Err:"))
//
//			if (!PathFileExists(strIniPath))
//			{
//				CFile m_file;
//				m_file.Open(strIniPath, CFile::modeCreate | CFile::modeWrite);
//				m_file.Close();
//			}
//			else
//			{
//				::WritePrivateProfileStringW(_T("newbuykindno"), _T("No"), strBuyKindGetFromServer, strIniPath);
//
//				CString strBuykind;
//				::GetPrivateProfileStringW(_T("buykindno"), _T("No"), _T("NULL"), strBuykind.GetBuffer(MAX_PATH), MAX_PATH, strIniPath);
//				strBuykind.ReleaseBuffer();
//				if (strBuykind == _T("NULL"))
//				{
//					::WritePrivateProfileStringW(_T("buykindno"), _T("No"), strBuyKindGetFromServer, strIniPath);
//				}
//				else if (strBuykind != strBuyKindGetFromServer) //需要改变类型 
//				{
//				
//					UINT nConnetctNo = 0;
//					CStringArray strArrayItem, strEnameArray, strCnameArray;
//					CArray<int, int>iArrayItemState;
//					BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
//					BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
//					BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
//					UINT nPort = _ttoi(theApp.m_strKbasePort);
//
//					if (!OpenEx(nConnetctNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
//					{
//						SysFreeString(bstrID);
//						SysFreeString(bstrUserName);
//						SysFreeString(bstrUserPassword);
//						AfxMessageBox(_T("连接数据库失败"));
//						return FALSE;
//					}
//					SysFreeString(bstrID);
//					SysFreeString(bstrUserName);
//					SysFreeString(bstrUserPassword);
//					//按照dowloadstate表 对数据进行整理
//					//CString strSql = _T("SELECT thname,kindname,state FROM ITEMINFOANDDOWNLOADSTATE  where state<2");
//					//BSTR bstrSql = strSql.AllocSysString();
//					//SysFreeString(bstrSql);
//					//UINT nRecord = 0;
//					//if (!OpenRecordSetEx(nConnetctNo, nRecord, bstrSql))
//					//{
//					//	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
//					//	return 0;
//					//}
//					//int iNoCount = GetRecordCountEx(nConnetctNo, nRecord);
//
//					//if (iNoCount>0)
//					//{
//					//	CString strKindId, strBookId;
//					//	UINT nState;
//					//	do
//					//	{
//					//		nState = _ttoi(CString(GetFieldValueEx(nConnetctNo, nRecord, 2)));
//					//		strKindId = CString(GetFieldValueEx(nConnetctNo, nRecord, 1));
//					//		strBookId = CString(GetFieldValueEx(nConnetctNo, nRecord, 0));
//
//					//		CString strSavePath = pDlg->m_strDataPath + _T("\\WYQK\\") + strBookId;
//					//		if (PathFileExists(strSavePath))
//					//		{
//					//			DeleteFile(strSavePath);
//					//		}
//					//		if (nState<1)
//					//		{
//					//			strSavePath = pDlg->m_strFmPath + strKindId + _T("\\") + strBookId + _T(".jpg");
//					//			if (PathFileExists(strSavePath))
//					//			{
//					//				DeleteFile(strSavePath);
//					//			}
//
//					//		}
//
//
//					//	} while (MoveNextEx(nConnetctNo, nRecord));
//
//					//}
//
//					//CloseRecordSetEx(nConnetctNo, nRecord);
//
//
//
//					//清空 newcjbdlast 和 dowloadstate表
//
//					CString strSql;
//					strSql.Format(_T("delete form newcjbdlast"));
//					if (!ExecSQL(nConnetctNo, strSql))
//					{
//						AfxMessageBox(_T("清空newcjbdlast表失败!"));
//						return 0;
//					}
//
//					strSql.Format(_T("delete form ITEMINFOANDDOWNLOADSTATE"));
//					if (!ExecSQL(nConnetctNo, strSql))
//					{
//						AfxMessageBox(_T("清空ITEMINFOANDDOWNLOADSTATE表失败!"));
//						return 0;
//					}
//				}
//
//
//
//			}
//
//
//			CString strPreSerailNo = theApp.m_strSerialNo;
//			if (theApp.GetSerialNoFromServer() && theApp.IsRegest())
//			{
//			
//				CString strIniPath = theApp.m_strCanChangePath;
//				strIniPath += _T("xml\\serialno.ini");
//
//				::WritePrivateProfileStringW(_T("SerialNo"), _T("No"), theApp.m_strSerialNo, strIniPath);
//				if (strPreSerailNo != theApp.m_strSerialNo)
//				{
//					theApp.GetBuyYear();
//				}
//
//			}
//			else
//			{
//				theApp.m_strSerialNo = strPreSerailNo;
//			}
//
//			//CString strUrl, strGetFromWeb;
//			strUrl.Format(_T("%s/tsdl/tsinfo.ashx?kind=all_dayupdate&cid=%s&Year=%d-%d"), theApp.m_strCnkiServerId,
//				theApp.m_strRegestNo, iYearNowLoad, iYearNowLoad);
//
//			while (!theApp.GetStrFromWeb(strUrl, strGetFromWeb) && !theApp.m_bQuestClose);
//	
//			CStringArray strArrayNew;
//			if (!strGetFromWeb.IsEmpty() && strGetFromWeb.Left(4) != _T("Err:"))
//			{
//				int iStart = 0;
//				int iEnd = 0;
//				iEnd = strGetFromWeb.Find(_T("-"), iStart);
//				while (iEnd > iStart)
//				{
//					strArrayNew.Add(strGetFromWeb.Mid(iStart, iEnd - iStart));
//					strGetFromWeb = strGetFromWeb.Right(strGetFromWeb.GetLength() - iEnd - 1);
//
//					iStart = 0;
//					iEnd = strGetFromWeb.Find(_T("-"), iStart);
//				}
//				if (strGetFromWeb.GetLength() > 0)
//				{
//					strArrayNew.Add(strGetFromWeb);
//				}
//			}
//			if (strArrayNew.GetSize() > 0)
//			{
//				CString strShowKind, strEname, strSzm, strCname;
//				for (int i = 0; i < strArrayNew.GetSize() && !theApp.m_bQuestClose; i++)
//				{
//					// 				m_strArrayLoadTemp_Requ.RemoveAll();
//					// 				m_iArrayLoadState.RemoveAll();
//					// 				m_nNowLoadNo=0;
//					CStringArray strArray;
//					CString strTemp = strArrayNew.GetAt(i);
//					strShowKind = strTemp.Left(strTemp.Find(_T("/")));
//					strTemp = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("/")) - 1);
//					strEname = strTemp.Left(strTemp.Find(_T("/")));
//					strTemp = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("/")) - 1);
//					strSzm = strTemp.Left(strTemp.Find(_T("/")));
//					strTemp = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("/")) - 1);
//					strCname = strTemp.Left(strTemp.Find(_T("/")));
//					strTemp = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("/")) - 1);
//					while (!strTemp.IsEmpty())
//					{
//						int iNo = strTemp.Find(_T("/"));
//						if (iNo > 0)
//						{
//							strArray.Add(strEname + _T("/") + strTemp.Left(iNo));
//							strTemp = strTemp.Right(strTemp.GetLength() - iNo - 1);
//
//						}
//						else
//						{
//							strArray.Add(strEname + _T("/") + strTemp);
//							strTemp.Empty();
//						}
//						//m_iArrayLoadState.Add(0);				
//					}
//					for (size_t j = 0; j < strArray.GetSize(); j++)
//					{
//
//					}
//
//					if (strArray.GetSize() > 0)
//					{
//						//DowloadFm_ALL(strPyname);	
//						CStringArray strNeedAdd;
//						pDlg->DowloadData(strEname, strCname, strShowKind, strArray, strNeedAdd);
//						Sleep(5);
//
//					}
//
//
//
//				}
//			}
//			iYearNowLoad--;
//
//			if (iYearNowLoad<_ttoi(theApp.m_strBuyStartYear))
//			{
//				iYearNowLoad = min(iYearNowLoad, _ttoi(theApp.m_strBuyEndYear));
//			}
//
//			iHour = tmNow.GetHour();
//		}
//		tmNow = CTime::GetCurrentTime();
//	}
//	return 1;
//}

DWORD WINAPI CMonitorDlg::ThreadClearInvalidateData(LPVOID lpParame)
{
	CMonitorDlg* pDlg = (CMonitorDlg*)lpParame;
	if (theApp.m_nGetInfoVersion == 0)
	{
		pDlg->CheckAndDelInvalidateWebPage();
	}

	if (theApp.m_nPaperSaveDayLength>0)
	{
		int i = 0;
		CTime tim_now = CTime::GetCurrentTime();
		CString strDataPath;
		pDlg->GetDataPath(_T("BZZX"), strDataPath);
		while (i<pDlg->m_strArrayCname.GetSize() && !theApp.m_bQuestClose && (tim_now.GetHour()>5 && tim_now.GetHour()<23))
		{
			if (PathFileExists(strDataPath + pDlg->m_strArrayCname.GetAt(i)))
			{
				pDlg->CheckAndDelPaper(strDataPath + pDlg->m_strArrayCname.GetAt(i));
			}
			i++;
			tim_now = CTime::GetCurrentTime();
		}

	}
	return 0;
}








CString ReadSaveWeather()
{

	CString strWeather;
	CString strIniFileName = _T("\\xml\\weather.xml");

	strIniFileName = theApp.m_strCanChangePath + strIniFileName;

	if (!PathFileExists(strIniFileName))
	{
		AfxMessageBox(_T("配置文件不存在！"));
		return FALSE;
	}

	//从xml中读取配置信息
	::CoInitialize(NULL);
	BSTR strNodeValue;
	MSXML2::IXMLDOMDocumentPtr spDoc;
	HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));    //创建文档对象 

	BSTR bstrIniFileName = strIniFileName.AllocSysString();
	SysFreeString(bstrIniFileName);
	hr = spDoc->load(bstrIniFileName);//load xml文件  

	MSXML2::IXMLDOMNodeListPtr spNodeList;
	MSXML2::IXMLDOMNodePtr pItem;

	spNodeList = spDoc->getElementsByTagName(_bstr_t("a"));   //获取根结点  
	long nListNo = 0;
	spNodeList->get_length(&nListNo);
	if (nListNo <= 0)
	{
		spNodeList.Release();
		spDoc.Release();
		::CoUninitialize();
		return strWeather;
	}

	spNodeList->get_item(0, &pItem);
	pItem->get_text(&strNodeValue);
	strWeather = (CString)strNodeValue;

	pItem.Release();

	spNodeList.Release();

	spDoc.Release();
	::CoUninitialize();

	return strWeather;
}

BOOL bSamePeriodTime(CTime tPre, CTime tNow, BOOL& bSameDay)
{
	CTimeSpan cspan = tNow - tPre;
	UINT nPreHour = tPre.GetHour();
	UINT nNowHour = tNow.GetHour();
	bSameDay = TRUE;
	if (cspan.GetDays() >0)
	{
		bSameDay = FALSE;
		return FALSE;
	}
	else
	{
		if ((nPreHour<8 && nNowHour >= 8) || (nPreHour<11 && nNowHour >= 11) || (nPreHour<18 && nNowHour >= 18))
		{
			return FALSE;
		}
	}
	return TRUE;
}



DWORD CMonitorDlg::GetHash(CString strRes, DWORD dwDataLen, ALG_ID algId, CString& strHash)
{

	DWORD dwReturn = 0;
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		return (dwReturn = GetLastError());

	HCRYPTHASH hHash;
	//Alg Id:CALG_MD5,CALG_SHA
	if (!CryptCreateHash(hProv, algId, 0, 0, &hHash))
	{
		dwReturn = GetLastError();
		CryptReleaseContext(hProv, 0);
		return dwReturn;
	}

	if (!CryptHashData(hHash, (BYTE*)CString2char(strRes), strlen(CString2char(strRes)), 0))
	{
		dwReturn = GetLastError();
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return dwReturn;
	}

	DWORD dwSize;
	DWORD dwLen = sizeof(dwSize);
	CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)(&dwSize), &dwLen, 0);

	BYTE* pHash = new BYTE[dwSize];
	dwLen = dwSize;
	CryptGetHashParam(hHash, HP_HASHVAL, pHash, &dwLen, 0);

	TCHAR szTemp[3];
	for (DWORD i = 0; i < dwLen; ++i)
	{
		//wsprintf(szTemp, _T("%X%X"), pHash[i] >> 4, pHash[i] & 0xf);
		wsprintf(szTemp, _T("%02X"), pHash[i]);
		strHash += szTemp;
	}
	delete[] pHash;

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	return dwReturn;

}

//DWORD WINAPI CMonitorDlg::ThreadGetWeather(LPVOID lpParame)
//{
//	CTime tCurent = CTime::GetCurrentTime();
//	CString strWeather = ReadSaveWeather();
//	CTime tPreTime(1999,1,1,1,1,1);
//	static BOOL bFirstGet = TRUE;
//	if (!strWeather.IsEmpty())
//	{
//		CTime t(_ttoi(strWeather.Left(4)), _ttoi(strWeather.Mid(4, 2)), _ttoi(strWeather.Mid(6, 2)), 0,0, 0);
//		tPreTime = t;
//	}
//
//	CMutex g_mutex(false, _T("mutex_cqweatherxml"));
//
//	while (!theApp.m_bQuestClose)
//	{
//		if (!bFirstGet)
//		{
//			Sleep(5);
//		}
//		else
//		{
//			bFirstGet = FALSE;
//		}
//
//		tCurent = CTime::GetCurrentTime();
//		BOOL bSameDay;
//		if (!bSamePeriodTime(tPreTime, tCurent, bSameDay))
//		{
//			tPreTime = tCurent;
//			CString strDayIcon, strNightIcon, strDayTemperature, strNightTemperature, strDayAndNightBoundary;
//
//
//			CString  strTemp = _T("http://open.weather.com.cn/data/?areaid=101040100&type=forecast_f&date=201512301344&appid=99f468691bcca5e9");
//			CString  strKey = _T("10eccb_SmartWeatherAPI_c255bbd");
//			CString strGet;
//			BYTE * BTemp = new BYTE[20];
//			CHMAC_SHA1 hmac_sha1;
//			hmac_sha1.HMAC_SHA1((BYTE*)CString2char(strTemp), strlen(CString2char(strTemp)), (BYTE*)CString2char(strKey), strlen(CString2char(strKey)), BTemp);
//
//			strGet = theApp.m_DecTemp.base64_encode_stander(BTemp, 20);
//			strGet = theApp.m_DecTemp.UrlEncode(strGet);
//			CString strUrl = _T("http://open.weather.com.cn/data/?areaid=101040100&type=forecast_f&date=201512301344&appid=99f468&key=") + strGet;
//			GetStrFromWeb(strUrl, strGet);
//
//			delete BTemp;
//			BTemp = NULL;
//
//			CString strFind;
//			int iStart, iEnd;
//
//			strFind = _T("\"fa\":\"");
//			iStart = strGet.Find(strFind);
//			if (iStart >= 0)
//			{
//				iStart += strFind.GetLength();
//				iEnd = strGet.Find(_T("\""), iStart);
//				strDayIcon = strGet.Mid(iStart, iEnd - iStart);
//			}
//
//
//			strFind = _T("\"fb\":\"");
//			iStart = strGet.Find(strFind);
//			if (iStart >= 0)
//			{
//				iStart += strFind.GetLength();
//				iEnd = strGet.Find(_T("\""), iStart);
//				strNightIcon = strGet.Mid(iStart, iEnd - iStart);
//			}
//
//
//			strFind = _T("\"fc\":\"");
//			iStart = strGet.Find(strFind);
//			if (iStart >= 0)
//			{
//				iStart += strFind.GetLength();
//				iEnd = strGet.Find(_T("\""), iStart);
//				strDayTemperature = strGet.Mid(iStart, iEnd - iStart);
//			}
//
//			strFind = _T("\"fd\":\"");
//			iStart = strGet.Find(strFind);
//			if (iStart >= 0)
//			{
//				iStart += strFind.GetLength();
//				iEnd = strGet.Find(_T("\""), iStart);
//				strNightTemperature = strGet.Mid(iStart, iEnd - iStart);
//			}
//
//			if (strDayTemperature.IsEmpty())
//			{
//				if (bSameDay && !strWeather.IsEmpty())
//				{
//					iStart = strWeather.Find(_T(";"));
//					iStart += 1;
//					iStart = strWeather.Find(_T(";"));
//					iStart += 1;
//					iStart = strWeather.Find(_T(";"));
//					iStart += 1;
//					strDayTemperature = strWeather.Mid(iStart, strWeather.Find(_T(";"), iStart) - iStart);
//				}
//				else
//				{
//					strDayTemperature = strNightTemperature;
//				}
//			}
//
//			strFind = _T("\"fi\":\"");
//			iStart = strGet.Find(strFind);
//			if (iStart >= 0)
//			{
//				iStart += strFind.GetLength();
//				iEnd = strGet.Find(_T("\""), iStart);
//				strDayAndNightBoundary = strGet.Mid(iStart, iEnd - iStart);
//			}
//
//			if (!strDayIcon.IsEmpty() && !strNightIcon.IsEmpty() && !strDayTemperature.IsEmpty() && !strNightTemperature.IsEmpty() && !strDayAndNightBoundary.IsEmpty())
//			{
//				CString strWeather = tCurent.Format(_T("%Y%m%d%H"));
//				strWeather.AppendFormat(_T(";%s;%s;%s;%s;%s;"), strDayIcon, strNightIcon, strDayTemperature, strNightTemperature, strDayAndNightBoundary);
//
//				//AfxMessageBox(strWeather)
//				
//				g_mutex.Lock();
//				SetSaveWeather(strWeather);
//				g_mutex.Unlock();
//			    
//			}
//
//
//		}
//	}
//	
//	
//	return 0;
//}


CString FindSubStrFromStr(CString strRes, CString strStart, CString strEnd, UINT nStart, int& iEnd)
{
	CString strReturn;
	int iStart = strRes.Find(strStart, nStart);
	iEnd = -1;
	if (iStart >= 0)
	{
		iStart += strStart.GetLength();
		iEnd = strRes.Find(strEnd, iStart);
	}

	if (iStart >= 0 && iEnd > iStart)
	{
		strReturn = strRes.Mid(iStart, iEnd - iStart);
		iEnd += strEnd.GetLength();
	}
	return strReturn;
}

BOOL GetUserNewInfoXML(CString strXmlPath, CStringArray& strArrayPicUrl, CStringArray& strArrayPicHLink, CStringArray& strArrayText, CStringArray& strArrayTextHLink)
{

	if (!PathFileExists(strXmlPath))
	{
		AfxMessageBox(_T("usernew配置文件不存在！"));
		return FALSE;
	}

	//从xml中读取配置信息
	::CoInitialize(NULL);
	CComVariant varNodeValue;
	MSXML2::IXMLDOMDocumentPtr spDoc;
	HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));    //创建文档对象 

	hr = spDoc->load(strXmlPath.AllocSysString());//load xml文件  

	MSXML2::IXMLDOMNodeListPtr spNodeList;
	MSXML2::IXMLDOMNodePtr pItem;

	spNodeList = spDoc->getElementsByTagName(_bstr_t("pic"));   //获取根结点  

	long lCount;
	spNodeList->get_length(&lCount);

	MSXML2::IXMLDOMNodeListPtr spNodeListSub;
	MSXML2::IXMLDOMNodePtr pItemSub;

	for (size_t i = 0; i < lCount; i++)
	{
		spNodeList->get_item(i, &pItem);

		spNodeListSub = pItem->GetchildNodes();
		long lSubCount;
		spNodeListSub->get_length(&lSubCount);
		if (lSubCount == 2)
		{
			spNodeListSub->get_item(0, &pItemSub);
			pItemSub->get_nodeTypedValue(&varNodeValue);
			strArrayPicUrl.Add((CString)(LPCTSTR)(_bstr_t)varNodeValue);
			pItemSub.Release();
			spNodeListSub->get_item(1, &pItemSub);
			pItemSub->get_nodeTypedValue(&varNodeValue);
			strArrayPicHLink.Add((CString)(LPCTSTR)(_bstr_t)varNodeValue);
			pItemSub.Release();
		}

		spNodeListSub.Release();
		pItem.Release();
	}

	spNodeList.Release();


	spNodeList = spDoc->getElementsByTagName(_bstr_t("text"));   //获取根结点  
	spNodeList->get_length(&lCount);

	for (size_t i = 0; i < lCount; i++)
	{
		spNodeList->get_item(i, &pItem);

		spNodeListSub = pItem->GetchildNodes();
		long lSubCount;
		spNodeListSub->get_length(&lSubCount);
		if (lSubCount == 2)
		{
			spNodeListSub->get_item(0, &pItemSub);
			pItemSub->get_nodeTypedValue(&varNodeValue);
			strArrayText.Add((CString)(LPCTSTR)(_bstr_t)varNodeValue);
			pItemSub.Release();
			spNodeListSub->get_item(1, &pItemSub);
			pItemSub->get_nodeTypedValue(&varNodeValue);
			strArrayTextHLink.Add((CString)(LPCTSTR)(_bstr_t)varNodeValue);
			pItemSub.Release();
		}

		spNodeListSub.Release();
		pItem.Release();
	}

	spNodeList.Release();



	spDoc.Release();
	::CoUninitialize();

	return TRUE;
}



BOOL GetUserNewInfoFromUrl(CString strUrl, CStringArray& strArrayPicUrl, CStringArray& strArrayPicHLink, CStringArray& strArrayText, CStringArray& strArrayTextHLink, CString strMoreUrl)
{
	CString strGet;

	theApp.GetStrFromWeb(strUrl, strGet);
	CString strMainUrl = strUrl.Left(strUrl.GetLength() - 1);
	if (!strGet.IsEmpty())
	{

		//得到图片及图片链接
		CString strStart, strEnd;
		strStart = _T("<div class=\"lxfscroll\">");
		strEnd = _T("</div>");
		int iEnd = -1;
		int iSubEnd = 0;
		CString strTemp = FindSubStrFromStr(strGet, strStart, strEnd, 0, iEnd);

		while (!strTemp.IsEmpty())
		{
			strStart = _T("href=\"");
			strEnd = _T("\"");
			CString strPicHLink, strPicUrl;

			strPicHLink = FindSubStrFromStr(strTemp, strStart, strEnd, 0, iSubEnd);
			if (!strPicHLink.IsEmpty())
			{
				strStart = _T("src=\"");
				strEnd = _T("\"");
				strPicUrl = FindSubStrFromStr(strTemp, strStart, strEnd, iSubEnd, iSubEnd);
				if (!strPicUrl.IsEmpty())
				{
					strArrayPicHLink.Add(strMainUrl + strPicHLink);
					strArrayPicUrl.Add(strMainUrl + strPicUrl);
				}
			}
			if (iSubEnd<0 || iSubEnd >= strTemp.GetLength())
			{
				strTemp.Empty();
			}
			else
			{
				strTemp = strTemp.Right(strTemp.GetLength() - iSubEnd);
			}

		}



		//得到新闻的文字和链接
		strStart = _T("<div class=\"picboxr\">");
		strEnd = _T("</div>");
		iEnd = -1;
		iSubEnd = 0;
		strTemp = FindSubStrFromStr(strGet, strStart, strEnd, 0, iEnd);

		if (!strTemp.IsEmpty())//先得到更多
		{
			strStart = _T("<div class=\"gengduo\"><a href=\"");
			strEnd = _T("\"");
			strMoreUrl = strMainUrl + FindSubStrFromStr(strTemp, strStart, strEnd, 0, iEnd);
		}


		while (!strTemp.IsEmpty() && strArrayTextHLink.GetSize()<10)
		{
			strStart = _T("href=\"");
			strEnd = _T("\"");
			CString strNewHLink, strNewText;

			strNewHLink = FindSubStrFromStr(strTemp, strStart, strEnd, 0, iSubEnd);
			if (!strNewHLink.IsEmpty())
			{
				strStart = _T(">");
				strEnd = _T("</a></li>");
				strNewText = FindSubStrFromStr(strTemp, strStart, strEnd, iSubEnd, iSubEnd);
				if (!strNewText.IsEmpty())
				{
					strArrayTextHLink.Add(strMainUrl + strNewHLink);
					strArrayText.Add(strNewText);
				}
			}
			if (iSubEnd<0 || iSubEnd >= strTemp.GetLength())
			{
				strTemp.Empty();
			}
			else
			{
				strTemp = strTemp.Right(strTemp.GetLength() - iSubEnd);
			}

		}
		return TRUE;
	}

	return FALSE;
}


CString ReadNewsAddr()
{

	CString strReturn;
	TCHAR szDictPath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szDictPath);
	PathAppend(szDictPath, _T("\\TTKN\\CAJTouchViewerre"));
	CString strDictPath = szDictPath;
	CString strIniFileName = _T("\\xml\\profile.xml");

	strIniFileName = strDictPath + strIniFileName;

	if (!PathFileExists(strIniFileName))
	{
		AfxMessageBox(_T("配置文件不存在！"));
		return FALSE;
	}

	CMutex g_mutex(false, _T("mutex_Update_profile.xml"));
	g_mutex.Lock();

	//从xml中读取配置信息
	::CoInitialize(NULL);
	CComVariant varNodeValue;
	MSXML2::IXMLDOMDocumentPtr spDoc;
	HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));    //创建文档对象 

	BSTR bstrInitFile = strIniFileName.AllocSysString();
	SysFreeString(bstrInitFile);
	hr = spDoc->load(bstrInitFile);//load xml文件  

	MSXML2::IXMLDOMNodeListPtr spNodeList;
	MSXML2::IXMLDOMNodePtr pItem;


	spNodeList = spDoc->getElementsByTagName(_bstr_t("q"));   //获取根结点  
	spNodeList->get_item(0, &pItem);

	MSXML2::IXMLDOMNodeListPtr spNodeListSub;
	MSXML2::IXMLDOMNodePtr pItemSub;
	spNodeListSub = pItem->GetchildNodes();

	spNodeListSub->get_item(0, &pItemSub);
	pItemSub->get_nodeTypedValue(&varNodeValue);
	strReturn = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItemSub.Release();

	spNodeListSub.Release();
	pItem.Release();


	spNodeList.Release();

	spDoc.Release();
	::CoUninitialize();
	g_mutex.Unlock();
	return strReturn;
}

CString ReadHtmlPath()
{

	CString strReturn;
	TCHAR szDictPath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szDictPath);
	PathAppend(szDictPath, _T("\\TTKN\\CAJTouchViewerre"));
	CString strDictPath = szDictPath;
	CString strIniFileName = _T("\\xml\\profile.xml");

	strIniFileName = strDictPath + strIniFileName;

	if (!PathFileExists(strIniFileName))
	{
		AfxMessageBox(_T("配置文件不存在！"));
		return FALSE;
	}
	CMutex g_mutex(false, _T("mutex_Update_profile.xml"));
	g_mutex.Lock();

	//从xml中读取配置信息
	::CoInitialize(NULL);
	BSTR strNodeValue;
	MSXML2::IXMLDOMDocumentPtr spDoc;
	HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));    //创建文档对象 

	BSTR bstrIniFileName = strIniFileName.AllocSysString();
	SysFreeString(bstrIniFileName);

	hr = spDoc->load(bstrIniFileName);//load xml文件  

	MSXML2::IXMLDOMNodeListPtr spNodeList;
	MSXML2::IXMLDOMNodePtr pItem;

	spNodeList = spDoc->getElementsByTagName(_bstr_t("i"));   //获取根结点  
	long nListNo = 0;
	spNodeList->get_length(&nListNo);
	if (nListNo <= 0)
	{
		strReturn.Empty();
		spNodeList.Release();
		spDoc.Release();
		::CoUninitialize();
		g_mutex.Unlock();
		return FALSE;
	}

	spNodeList->get_item(0, &pItem);
	pItem->get_text(&strNodeValue);
	strReturn = (CString)strNodeValue;

	pItem.Release();

	spNodeList.Release();

	spDoc.Release();
	::CoUninitialize();
	g_mutex.Unlock();
	return strReturn;
}


DWORD WINAPI CMonitorDlg::ThreadLoadUserHtmlDate(LPVOID lpParameter)
{

	static CTime tMoni = CTime::GetCurrentTime();
	CTimeSpan ts = CTime::GetCurrentTime() - tMoni;
	BOOL bFirst = TRUE;
	while (!theApp.m_bQuestClose)
	{
		if (ts.GetTotalMinutes()<20 && !bFirst)
		{
			ts = CTime::GetCurrentTime() - tMoni;
			Sleep(5);
			continue;
		}
		else
		{
			tMoni = CTime::GetCurrentTime();
			ts = CTime::GetCurrentTime() - tMoni;
			bFirst = FALSE;
		}

		CString strAddr = ReadNewsAddr();
		CString strHtmlpath = ReadHtmlPath();
		if (strAddr.IsEmpty() || strHtmlpath.IsEmpty())
		{
			continue;
		}

		CStringArray strArrayPicUrl, strArrayPicHLink, strArrayText, strArrayTextHLink;
		CString strMoreUrl;
		if (GetUserNewInfoFromUrl(strAddr, strArrayPicUrl, strArrayPicHLink, strArrayText, strArrayTextHLink, strMoreUrl))
		{
			if (strArrayPicUrl.GetSize() == 0 || strArrayText.GetSize() == 0)
			{
				continue;
			}
			CStringArray strArrayPicUrl_old, strArrayPicHLink_old, strArrayText_old, strArrayTextHLink_old;
			CString strXmlPath = strHtmlpath + _T("\\news\\index.html");
			if (GetUserNewInfoXML(strXmlPath, strArrayPicUrl_old, strArrayPicHLink_old, strArrayText_old, strArrayTextHLink_old))
			{


				int iNewLength = strArrayPicUrl.GetSize();
				int iOldLength = strArrayPicUrl_old.GetSize();
				int iMin = min(iNewLength, iOldLength);

				//修正xml
				::CoInitialize(NULL);
				CComVariant varNodeValue;
				MSXML2::IXMLDOMDocumentPtr spDoc;
				HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));    //创建文档对象 
				BSTR bxmlPath = strXmlPath.AllocSysString();
				hr = spDoc->load(bxmlPath);//load xml文件  
				SysFreeString(bxmlPath);
				MSXML2::IXMLDOMNodeListPtr spNodeList;
				MSXML2::IXMLDOMNodePtr pItem;

				MSXML2::IXMLDOMNamedNodeMapPtr pAttrs = NULL;
				spNodeList = spDoc->getElementsByTagName(_bstr_t("pic"));   //获取根结点  

				MSXML2::IXMLDOMNodeListPtr spNodeListSub;
				MSXML2::IXMLDOMNodePtr pItemSub;

				for (size_t i = 0; i < iMin; i++)//纠正
				{
					spNodeList->get_item(i, &pItem);
					spNodeListSub = pItem->GetchildNodes();
					if (strArrayPicUrl.GetAt(i) != strArrayPicUrl_old.GetAt(i))
					{
						spNodeListSub->get_item(0, &pItemSub);
						BSTR bSet = strArrayPicUrl.GetAt(i).AllocSysString();
						pItemSub->put_text(bSet);
						SysFreeString(bSet);
						pItemSub.Release();
					}

					if (strArrayPicHLink.GetAt(i) != strArrayPicHLink_old.GetAt(i))
					{
						spNodeListSub->get_item(1, &pItemSub);
						BSTR bSet = strArrayPicHLink.GetAt(i).AllocSysString();
						pItemSub->put_text(bSet);
						SysFreeString(bSet);
						pItemSub.Release();
					}

					spNodeListSub.Release();
					pItem.Release();


				}

				if (iNewLength<iOldLength) //删除
				{
					for (size_t i = iNewLength; i < iOldLength; i++)
					{
						pItem = spDoc->GetdocumentElement();
						for (size_t i = iNewLength; i < iOldLength; i++)
						{
							spNodeList->get_item(i, &pItemSub);
							pItem->removeChild(pItemSub);
							pItemSub.Release();
						}
						pItem.Release();
					}
				}
				else if (iNewLength>iOldLength)//添加
				{
					pItem = spDoc->GetdocumentElement();
					MSXML2::IXMLDOMNodePtr pNode = NULL;
					MSXML2::IXMLDOMElementPtr pElement = NULL;
					for (size_t i = iOldLength; i < iNewLength; i++)
					{

						pNode = spDoc->createNode((_variant_t)(long)NODE_ELEMENT, (_bstr_t)(char*)"pic", (_bstr_t)(char*)"");

						pElement = spDoc->createElement((_bstr_t)(char*)"Url");
						//pElement->setAttribute((_bstr_t)(char*)"Units", (_variant_t)(char*)"Thousand Square kilometers"); // 统计单位   
						BSTR bSet = strArrayPicUrl.GetAt(i).AllocSysString();
						pElement->put_text(bSet);
						SysFreeString(bSet);
						pNode->appendChild(pElement); // 节点   

						pElement = spDoc->createElement((_bstr_t)(char*)"HLink");
						bSet = strArrayPicHLink.GetAt(i).AllocSysString();
						pElement->put_text(bSet);
						SysFreeString(bSet);
						pNode->appendChild(pElement); // 节点   

						pItem->appendChild(pNode);

					}

				}




				spNodeList.Release();

				spNodeList = spDoc->getElementsByTagName(_bstr_t("text"));   //获取根结点  
				iNewLength = strArrayText.GetSize();
				iOldLength = strArrayText_old.GetSize();
				iMin = min(iNewLength, iOldLength);

				for (size_t i = 0; i < iMin; i++)//纠正
				{

					spNodeList->get_item(i, &pItem);
					spNodeListSub = pItem->GetchildNodes();
					if (strArrayText.GetAt(i) != strArrayText_old.GetAt(i))
					{
						spNodeListSub->get_item(0, &pItemSub);
						BSTR bSet = strArrayText.GetAt(i).AllocSysString();
						pItemSub->put_text(bSet);
						SysFreeString(bSet);
						pItemSub.Release();
					}

					if (strArrayTextHLink.GetAt(i) != strArrayTextHLink_old.GetAt(i))
					{
						spNodeListSub->get_item(1, &pItemSub);
						BSTR bSet = strArrayTextHLink.GetAt(i).AllocSysString();
						pItemSub->put_text(bSet);
						SysFreeString(bSet);
						pItemSub.Release();
					}

					spNodeListSub.Release();
					pItem.Release();
				}

				if (iNewLength<iOldLength) //删除
				{
					pItem = spDoc->GetdocumentElement();
					for (size_t i = iNewLength; i < iOldLength; i++)
					{
						spNodeList->get_item(i, &pItemSub);
						pItem->removeChild(pItemSub);
						pItemSub.Release();
					}
					pItem.Release();
				}
				else if (iNewLength>iOldLength)//添加
				{
					pItem = spDoc->GetdocumentElement();
					MSXML2::IXMLDOMNodePtr pNode = NULL;
					MSXML2::IXMLDOMElementPtr pElement = NULL;
					for (size_t i = iOldLength; i < iNewLength; i++)
					{

						pNode = spDoc->createNode((_variant_t)(long)NODE_ELEMENT, (_bstr_t)(char*)"text", (_bstr_t)(char*)"");

						pElement = spDoc->createElement((_bstr_t)(char*)"Text");
						//pElement->setAttribute((_bstr_t)(char*)"Units", (_variant_t)(char*)"Thousand Square kilometers"); // 统计单位   
						BSTR bSet = strArrayText.GetAt(i).AllocSysString();
						pElement->put_text(bSet);
						SysFreeString(bSet);
						pNode->appendChild(pElement); // 节点   

						pElement = spDoc->createElement((_bstr_t)(char*)"HLink");
						bSet = strArrayTextHLink.GetAt(i).AllocSysString();
						pElement->put_text(bSet);
						SysFreeString(bSet);
						pNode->appendChild(pElement); // 节点   

						pItem->appendChild(pNode);

					}
				}



				spNodeList.Release();
				spDoc->save((_variant_t)strXmlPath);
				spDoc.Release();
				::CoUninitialize();

				return TRUE;


			}
		}


	}
	return 1;
}



DWORD WINAPI CMonitorDlg::ThreadPostUserUsedInfo(LPVOID lpParameter)
{
	struct UPDATEITEM
	{
		CString strClickTime;
		CString strClickInfo;
	};

	CArray<UPDATEITEM, UPDATEITEM> updateItemArray;
	CMonitorDlg* pDlg = (CMonitorDlg*)lpParameter;

	CTime tMoni = CTime::GetCurrentTime();

	int iNowHour = tMoni.GetHour();


	CString strRequestUrl;

	while (!theApp.m_bQuestClose && tMoni.GetHour() != iNowHour)
	{

		iNowHour = tMoni.GetHour();

		updateItemArray.RemoveAll();

		while (!theApp.LockRead(INFINITE, 10));
		CString strSql;
		strSql = _T("SELECT CLICKID,CLICKTIME  FROM CLICKINFO order by CLICKTIME LIMIT 50");
		CSqlStatement *stmt = theApp.m_sqlite.Statement(strSql);
		if (stmt != NULL && !stmt->IsNullStmt())
		{
			while (stmt->NextRow())
			{
				UPDATEITEM updateItem;
				updateItem.strClickInfo = stmt->ValueString(0);
				updateItem.strClickTime = stmt->ValueString(1);
				updateItemArray.Add(updateItem);
			}

			delete stmt;
			stmt = NULL;
		}
		else
		{
			theApp.UnLockRead();
			AfxMessageBox(_T("Select Sqlite  Error\n") + strSql);
			return FALSE;
		}
		theApp.UnLockRead();



		if (updateItemArray.GetSize() > 0)
		{
			Json::Value newjson;   // 构建对象
			int iNoCount = 5;
			char cStart[25], cEnd[25];
			CString2Char(updateItemArray.GetAt(0).strClickTime, cStart);
			CString2Char(updateItemArray.GetAt(updateItemArray.GetSize() - 1).strClickTime, cEnd);
			newjson["count"] = iNoCount;
			newjson["starttime"] = cStart;
			newjson["endtime"] = cEnd;

			for (int i = 0; i < updateItemArray.GetSize(); ++i)
			{
				char cTime[25], cClickId[100];
				CString2Char(updateItemArray.GetAt(i).strClickTime, cTime);
				CString2Char(updateItemArray.GetAt(i).strClickInfo, cClickId);

				Json::Value jvalueTemp;
				jvalueTemp["ClickIdInfo"] = cClickId;
				jvalueTemp["ClickTime"] = cTime;
				newjson["detailinfo"].append(jvalueTemp);
			}


			string str = newjson.toStyledString();

			const char *p = str.c_str();
			CString strReturn;
			theApp.SendStrFromWeb(strRequestUrl, strReturn, p);


			//需要知道返回值具体是什么 strReturn
			if (strReturn == _T(""))
			{
				//删除数据库内数据
				while(!theApp.LockWrite(-1, -1));
				theApp.m_sqlite.Begin();
				strSql.Format(_T("delete form CLICKINFO where CLICKTIME<'%s'"), updateItemArray.GetAt(updateItemArray.GetSize() - 1).strClickTime);

				if (!theApp.m_sqlite.DirectStatement(strSql))
				{
					theApp.UnLockWrite();
					AfxMessageBox(_T("更新数据库失败！ ") + strSql);
					return FALSE;
				}
				theApp.m_sqlite.Commit();
				theApp.UnLockWrite();
			}
		}



	}


	return 1;
}

wstring TranslateString(LPCWSTR wszOrig)

{

	wstring ret;

	LPCWSTR curChar = wszOrig;



	while (*curChar)
	{
		if (curChar[0] == L'\\' && (curChar[1] == L'u' || curChar[1] == L'U') && IS_NUMBER(curChar[2]) && IS_NUMBER(curChar[3]) && IS_NUMBER(curChar[4]) && IS_NUMBER(curChar[5]))
		{
			wchar_t hex[8] = { '0', 'x' };
			hex[2] = curChar[2];
			hex[3] = curChar[3];
			hex[4] = curChar[4];
			hex[5] = curChar[5];

			int i;
			StrToIntExW(hex, STIF_SUPPORT_HEX, &i);
			ret += (wchar_t)i;

			curChar += 6;
		}
		else
		{
			ret += (wchar_t)*curChar;
			curChar++;
		}

	}



	return ret;
}



void CMonitorDlg::Get1PartOneKindData(OPERATOR1PARTITEMSTRUCT& structitem)
{
	CString strUrl = structitem.strUrl;
	CString strHeader = structitem.strHeader;
	CString strGet;

	if (strHeader.Find(_T("\r\n"))<0)
	{
		strHeader += _T("\r\n");
	}

	long lSize = 0;
	if (theApp.GetStrFromWeb(strUrl, strGet, strHeader))
	{
		//EnDecryption DecTemp;
		//strGet=DecTemp.base64decode(strGet);
		CString str(TranslateString(strGet).c_str());
		str.Replace(_T("\\"), _T(""));
		strGet = str;
		CString strStart = structitem.strStartIdentification;
		CString strEnd = structitem.strEndIdentification;
		int istart = -1, iEnd = -1;
		if (strStart.IsEmpty())
		{
			istart = 0;
		}
		else
		{
			istart = strGet.Find(strStart);
		}

		if (istart >= 0)
		{
			if (strEnd.IsEmpty())
			{
				iEnd = strGet.GetLength();
			}
			else
			{
				iEnd = strGet.Find(strEnd, istart + strStart.GetLength());
			}

		}

		if (iEnd>0)
		{
			strGet = strGet.Mid(istart, iEnd - istart - strStart.GetLength());


			while (structitem.strArraySubId.GetSize()<50)
			{
				strStart = structitem.strStartRecordIdentification;
				strEnd = structitem.strEndRecordIdentification;
				istart = -1;
				iEnd = -1;
				istart = strGet.Find(strStart);
				if (istart >= 0)
				{
					iEnd = strGet.Find(strEnd, istart + strStart.GetLength());
				}
				if (iEnd>0)
				{
					CString strRecord = strGet.Mid(istart, iEnd - istart - strStart.GetLength());
					strGet = strGet.Right(strGet.GetLength() - iEnd - strEnd.GetLength());
					CString strText, strHttp;
					UINT nTandHDirection = structitem.nTextAndHttpDirection;
					if (nTandHDirection == 0)//文字在前
					{
						strStart = structitem.strTextStartIdentification;
						strEnd = structitem.strTextEndIdentification;
						iEnd = -1;
						istart = strRecord.Find(strStart);
						if (istart >= 0)
						{
							iEnd = strRecord.Find(strEnd, istart + strStart.GetLength());
						}
						if (iEnd>0)
						{
							strText = strRecord.Mid(istart + strStart.GetLength(), iEnd - istart - strStart.GetLength());

							strRecord = strRecord.Right(strRecord.GetLength() - iEnd - strEnd.GetLength());
						}
						else
						{
							continue;
						}
						strStart = structitem.strHttpStartIdentification;
						strEnd = structitem.strHttpEndIdentification;
						iEnd = -1;
						istart = strRecord.Find(strStart);
						if (istart >= 0)
						{
							iEnd = strRecord.Find(strEnd, istart + strStart.GetLength());
						}
						if (iEnd>0)
						{
							strHttp = strRecord.Mid(istart + strStart.GetLength(), iEnd - istart - strStart.GetLength());
						}
						else
						{
							continue;
						}
					}
					else
					{

						strStart = structitem.strHttpStartIdentification;
						strEnd = structitem.strHttpEndIdentification;
						iEnd = -1;
						istart = strRecord.Find(strStart);
						if (istart >= 0)
						{
							iEnd = strRecord.Find(strEnd, istart + strStart.GetLength());
						}
						if (iEnd>0)
						{
							strHttp = strRecord.Mid(istart + strStart.GetLength(), iEnd - istart - strStart.GetLength());
							strRecord = strRecord.Right(strRecord.GetLength() - iEnd - strEnd.GetLength());
						}
						else
						{
							continue;
						}

						strStart = structitem.strTextStartIdentification;
						strEnd = structitem.strTextEndIdentification;
						iEnd = -1;
						istart = strRecord.Find(strStart);
						if (istart >= 0)
						{
							iEnd = strRecord.Find(strEnd, istart + strStart.GetLength());
						}
						if (iEnd>0)
						{
							strText = strRecord.Mid(istart + strStart.GetLength(), iEnd - istart - strStart.GetLength());

						}
						else
						{
							continue;
						}

					}

					if (!strHttp.IsEmpty() && !strText.IsEmpty())
					{
						SYSTEMTIME st;
						CString strID;
						GetLocalTime(&st);
						strID.Format(_T("1PARTITEM%4d%02d%02d%02d%02d%02d%03d%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, structitem.strArraySubId.GetSize());
						structitem.strArraySubId.Add(strID);
						structitem.strArraySubCname.Add(strText);
						structitem.strArraySubHttp.Add(strHttp);
						//Sleep(1000);
					}



				}
				else
				{
					break;
				}
			}


		}

	}


}



//DWORD WINAPI CMonitorDlg::TheradGetNewData(LPVOID lpParameter)
//{
//
//	int iHour = -1;
//
//	while (!theApp.m_bQuestClose)
//	{
//		CTime tmNow = CTime::GetCurrentTime();
//		Sleep(5);
//		if (iHour != tmNow.GetHour())
//		{
//
//			UINT nConnetctNo = 0;
//			CArray<OPERATOR1PARTITEMSTRUCT, OPERATOR1PARTITEMSTRUCT> structArray1PartInfo;
//
//
//			BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
//			BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
//			BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
//			UINT nPort = _ttoi(theApp.m_strKbasePort);
//
//			if (!OpenEx(nConnetctNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
//			{
//				SysFreeString(bstrID);
//				SysFreeString(bstrUserName);
//				SysFreeString(bstrUserPassword);
//				AfxMessageBox(_T("连接数据库失败"));
//				return FALSE;
//			}
//
//			CMutex g_mutex(false, _T("mutex_Update1PartTabel"));
//
//			g_mutex.Lock();
//			CString strSql = _T("SELECT ID, C_NAME, KINDTYPE, STARTPI, ENDPI, RECORDSTARTPI, RECORDENDPI, TANDHDIR,UPDATETIME,LINKURL,QUESTHEADER,TEXTSTARTPI,TEXTENDPI,HTTPSTARTPI,HTTPENDPI from 1PARTKIND order by UPDATETIME");
//
//			if (!IsOpenEx(nConnetctNo) && !OpenEx(nConnetctNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
//			{
//				AfxMessageBox(_T("数据库连接失败！"));
//				g_mutex.Unlock();
//				return FALSE;
//			}
//			UINT nRecordNo = 0;
//			if (!OpenRecordSetEx(nConnetctNo, nRecordNo, strSql.AllocSysString()))
//			{
//				AfxMessageBox(_T("OpenRecordSetEx Error\n"));
//				g_mutex.Unlock();
//				return FALSE;
//			}
//			UINT nNoCount = GetRecordCountEx(nConnetctNo, nRecordNo);
//			if (nNoCount <= 0)
//			{
//				CloseRecordSetEx(nConnetctNo, nRecordNo);
//				g_mutex.Unlock();
//				return FALSE;
//			}
//			do
//			{
//				if (_ttoi(CString(GetFieldValueEx(nConnetctNo, nRecordNo, 2))) == 1) //接口
//				{
//					OPERATOR1PARTITEMSTRUCT item;
//					item.strID = GetFieldValueEx(nConnetctNo, nRecordNo, 0);
//					item.strCName = GetFieldValueEx(nConnetctNo, nRecordNo, 1);
//					item.nKind = _ttoi(CString(GetFieldValueEx(nConnetctNo, nRecordNo, 2)));
//					item.strUpdateTime = GetFieldValueEx(nConnetctNo, nRecordNo, 8);
//					item.strStartIdentification = GetFieldValueEx(nConnetctNo, nRecordNo, 3);
//					item.strEndIdentification = GetFieldValueEx(nConnetctNo, nRecordNo, 4);
//					item.strStartRecordIdentification = GetFieldValueEx(nConnetctNo, nRecordNo, 5);
//					item.strEndRecordIdentification = GetFieldValueEx(nConnetctNo, nRecordNo, 6);
//					item.nTextAndHttpDirection = _ttoi(CString(GetFieldValueEx(nConnetctNo, nRecordNo, 7)));
//					item.strTextStartIdentification = GetFieldValueEx(nConnetctNo, nRecordNo, 11);
//					item.strTextEndIdentification = GetFieldValueEx(nConnetctNo, nRecordNo, 12);
//					item.strHttpStartIdentification = GetFieldValueEx(nConnetctNo, nRecordNo, 13);
//					item.strHttpEndIdentification = GetFieldValueEx(nConnetctNo, nRecordNo, 14);
//
//					item.strUrl = GetFieldValueEx(nConnetctNo, nRecordNo, 9);
//					item.strHeader = GetFieldValueEx(nConnetctNo, nRecordNo, 10);
//
//					structArray1PartInfo.Add(item);
//				}
//
//
//
//
//			} while (MoveNextEx(nConnetctNo, nRecordNo) && !theApp.m_bQuestClose);
//
//			CloseRecordSetEx(nConnetctNo, nRecordNo);
//
//			g_mutex.Unlock();
//
//
//			for (size_t i = 0; i < structArray1PartInfo.GetSize() && !theApp.m_bQuestClose; i++)
//			{
//
//				((CMonitorDlg *)lpParameter)->Get1PartOneKindData(structArray1PartInfo.GetAt(i));
//
//				CString strSql;
//				if (structArray1PartInfo.GetAt(i).strArraySubId.GetSize() > 0)
//				{
//					g_mutex.Lock();
//
//					if (!IsOpenEx(nConnetctNo) && !OpenEx(nConnetctNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
//					{
//						AfxMessageBox(_T("数据库连接失败！"));
//						return FALSE;
//					}
//
//					strSql.Format(_T("SELECT KINDTYPE from 1PARTKIND where ID=\'%s\'"), structArray1PartInfo.GetAt(i).strID);
//					UINT nRecordNo = 0;
//					if (!OpenRecordSetEx(nConnetctNo, nRecordNo, strSql.AllocSysString()))
//					{
//						AfxMessageBox(_T("OpenRecordSetEx Error\n"));
//						g_mutex.Unlock();
//						return FALSE;
//					}
//					UINT nNoCount = GetRecordCountEx(nConnetctNo, nRecordNo);
//					if (nNoCount <= 0 || _ttoi(CString(GetFieldValueEx(nConnetctNo, nRecordNo, 0))) == 0)
//					{
//						CloseRecordSetEx(nConnetctNo, nRecordNo);
//						g_mutex.Unlock();
//						continue;
//					}
//					else
//					{
//						CloseRecordSetEx(nConnetctNo, nRecordNo);
//						strSql.Format(_T("SELECT * FROM 1PARTSHOWDETAIL WHERE KINDID=%s"), structArray1PartInfo.GetAt(i).strID);
//
//						if (!OpenRecordSetEx(nConnetctNo, nRecordNo, strSql.AllocSysString()))
//						{
//							AfxMessageBox(_T("OpenRecordSetEx Error\n"));
//							g_mutex.Unlock();
//							return FALSE;
//						}
//						nNoCount = GetRecordCountEx(nConnetctNo, nRecordNo);
//
//						CloseRecordSetEx(nConnetctNo, nRecordNo);
//						if (nNoCount>0)
//						{
//							if (!DeleteSelEx(nConnetctNo, strSql.AllocSysString()))
//							{
//								AfxMessageBox(_T("删除数据失败！") + strSql);
//
//							}
//						}
//
//
//						for (size_t j = 0; j < structArray1PartInfo.GetAt(i).strArraySubId.GetSize() && !theApp.m_bQuestClose; j++)
//						{
//							strSql = _T("INSERT INTO 1PARTSHOWDETAIL (ID,C_NAME,HTTP,KINDID) ");
//							strSql.AppendFormat(_T("VALUES(\'%s\',\'%s\',\'%s\',\'%s\')"), structArray1PartInfo.GetAt(i).strArraySubId.GetAt(j),
//								structArray1PartInfo.GetAt(i).strArraySubCname.GetAt(j), structArray1PartInfo.GetAt(i).strArraySubHttp.GetAt(j), structArray1PartInfo.GetAt(i).strID);
//							ExecSQL(nConnetctNo, strSql);
//						}
//					}
//					g_mutex.Unlock();
//					Sleep(50);
//
//				}
//			}
//
//
//			//tmNow = CTime::GetCurrentTime();
//			iHour = tmNow.GetHour();
//
//			SysFreeString(bstrID);
//			SysFreeString(bstrUserName);
//			SysFreeString(bstrUserPassword);
//		}
//	}
//}


bool WriteTxtFile(CString sFilePath, CString sTxt)
{
	char *tmpch;
#ifndef UNICODE
	tmpch = sTxt.GetBuffer(str.GetLength());
#else
	int wLen = WideCharToMultiByte(CP_ACP, 0, sTxt, -1, NULL, 0, NULL, NULL);//得到Char的长度
	tmpch = new char[wLen + 1];                                             //分配变量的地址大小
	WideCharToMultiByte(CP_ACP, 0, sTxt, -1, tmpch, wLen, NULL, NULL);       //将CString转换成char*
#endif

	CFile  f;
	f.Open(sFilePath, CFile::modeReadWrite | CFile::modeCreate);
	f.Write(tmpch, wLen);
	f.Close();
	delete tmpch;
	return true;
}

//DWORD WINAPI CMonitorDlg::ThreadUpdateZZHC(LPVOID lpParame)
//{
//	UINT nConnetctNo=0;
//	CStringArray strArrayItem,strEnameArray,strCnameArray;
//	CArray<int,int>iArrayItemState;
//	BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
//	BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
//	BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
//	UINT nPort = _ttoi(theApp.m_strKbasePort);
//
//	if (!OpenEx(nConnetctNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
//	{
//		SysFreeString(bstrID);
//		SysFreeString(bstrUserName);
//		SysFreeString(bstrUserPassword);
//		AfxMessageBox(_T("连接数据库失败"));
//		return FALSE;
//	}
//
//	CString strSql=_T("SELECT SQL_NO_CACHE thname,kindname,state FROM ITEMINFOANDDOWNLOADSTATE  where state>=4");
//	BSTR bstrSql=strSql.AllocSysString();
//	SysFreeString(bstrSql);
//	UINT nRecord=0;
//	if( !OpenRecordSetEx(nConnetctNo,nRecord,bstrSql) )
//	{
//		AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
//		return 0;     
//	}
//	int iNoCount=GetRecordCountEx(nConnetctNo,nRecord);
//
//	if (iNoCount>0)
//	{
//		CString strEname;
//		do 
//		{
//			strArrayItem.Add(CString(GetFieldValueEx(nConnetctNo,nRecord,0)));
//			iArrayItemState.Add(_ttoi(CString(GetFieldValueEx(nConnetctNo,nRecord,2))));
//			CString strTemp=CString(GetFieldValueEx(nConnetctNo,nRecord,1));
//			if (strEname!=strTemp && !strTemp.IsEmpty())
//			{
//				strEnameArray.Add(strTemp);
//				strEname=strTemp;
//			}
//
//		} while (MoveNextEx(nConnetctNo,nRecord));
//
//	}
//
//	CloseRecordSetEx(nConnetctNo,nRecord);
//
//
//	strSql.Format(_T("index newcjbdlast on thname,年,文件名,更新日期"));
//	if(!ExecSQL(nConnetctNo,strSql))
//	{			
//		AfxMessageBox(_T("索引任务表失败!"));
//		return 0;
//	}	
//
//// 	if (!PathFileExists(theApp.m_strCanChangePath+_T("\\TempUpdate")))
//// 	{
//// 		CreateDirectory(theApp.m_strCanChangePath+_T("\\TempUpdate"),NULL);
//// 	}
//// 
//// 	CString strFileNameRec=theApp.m_strCanChangePath+_T("\\TempUpdate\\FileNameRec.txt");
//// 
//// 	if (PathFileExists(strFileNameRec))
//// 	{
//// 		DeleteFile(strFileNameRec);
//// 	}
//// 	//	sScriptPath=CGlobal::GetExcutePath()+"Script.txt";
//// 	CString strScriptPcPath=theApp.m_strCanChangePath+_T("\\TempUpdate\\ScriptPc.txt");
//// 	if (PathFileExists(strScriptPcPath))
//// 	{
//// 		DeleteFile(strScriptPcPath);
//// 	}
//// 	CString strScriptPc;
//// 	strSql.Format(_T("select 文件名 from CJBDLAST into %s"),strFileNameRec);
//// 
//// 	if(!ExecSQL(nConnetctNo,strSql))
//// 	{			
//// 		AfxMessageBox(_T("统计原有数据失败!"));
//// 		return 0;
//// 	}	
//// 
//// 	strScriptPc=_T("CREATE OPTIMIZEKEY 文件名 ON TABLE newcjbdlast\r\n");
//// 	strScriptPc+=_T("DELETE FROM TABLE newcjbdlast WITH  KEY=文件名,RelevantCol=NULL,RelevantIndex=NULL\r\n");
//// 	strScriptPc+=_T("INFILE \"")+strFileNameRec+_T("\"\r\n");
//// 	strScriptPc+=_T("DROP OPTIMIZEKEY 文件名 ON TABLE newcjbdlast\r\n");
//// 	WriteTxtFile(strScriptPcPath,strScriptPc);
//// 
//// 	strSql.Format(_T("DBUM UPDATE TABLE BY \"%s\""),strScriptPcPath);
//// 
//// 	if(!ExecSQL(nConnetctNo,strSql))
//// 	{			
//// 		AfxMessageBox(_T("与用户数据表排重失败!"));
//// 		return 0;
//// 	}	
//
//	
//
//	for (int i=0;i<strArrayItem.GetSize()&&iArrayItemState.GetAt(i)==4 && !theApp.m_bQuestClose;i++)
//	{
//		//更新cjbdlast
//
//		strSql=_T("select SQL_NO_CACHE * from newcjbdlast where thname=")+strArrayItem.GetAt(i);
//		UINT nRecordNo=0;
//		BOOL bNeedUpdate=FALSE;
//		BSTR bstrSql=strSql.AllocSysString();
//		SysFreeString(bstrSql);
//		if (OpenRecordSetEx(nConnetctNo,nRecordNo,bstrSql) && GetRecordCountEx(nConnetctNo,nRecordNo)>0)
//		{
//			bNeedUpdate=TRUE;
//		}
//		CloseRecordSetEx(nConnetctNo,nRecordNo);
//
//		if (bNeedUpdate)
//		{
//			
//			CString strResREC=theApp.m_strCanChangePath+_T("\\TempUpdate\\ResREC.txt");
//			if (PathFileExists(strResREC))
//			{
//				DeleteFile(strResREC);
//			}
//
//			strSql=_T("select SQL_NO_CACHE 文件名,* from newcjbdlast where thname=");
//			strSql+=strArrayItem.GetAt(i)+_T(" INTO ")+strResREC;
//
//			if(!ExecSQL(nConnetctNo,strSql))
//			{
//				AfxMessageBox(_T("数据导出失败！"));
//				continue;
//			}
//
//			CString strScript;
//			CString strScriptPath=theApp.m_strCanChangePath+_T("\\TempUpdate\\Script.txt");
//			if (PathFileExists(strScriptPath))
//			{
//				DeleteFile(strScriptPath);
//			}
//			strScript=_T("CREATE OPTIMIZEKEY 文件名 ON TABLE  CJBDLAST \r\n");
//			strScript+=_T("INSERT INTO TABLE CJBDLAST WITH  KEY=文件名\r\n");
//			strScript+=_T("INFILE \"")+strResREC+_T("\"\r\n");
//			strScript+=_T("DROP OPTIMIZEKEY 文件名 ON TABLE CJBDLAST \r\n");
//
//			WriteTxtFile(strScriptPath,strScript);
//
//			strSql.Format(_T("DBUM UPDATE TABLE BY \"%s\" "),strScriptPath);
//			if(!ExecSQL(nConnetctNo,strSql))
//			{			
//				AfxMessageBox(_T("数据写入失败!"));
//				continue;
//			}	
//			//strSql=_T("DBUM refresh SORTFILE OF TABLE CJBDLAST");
//			//	
//			//if(!ExecSQL(nConnetctNo,strSql))
//			//{
//			//	AfxMessageBox(_T("更新用户KBase表失败!"));
//			//	continue;
//			//}
//
//			//更新信息表
//			strSql.Format(_T("update ITEMINFOANDDOWNLOADSTATE set state=5 where thname=%s"),strArrayItem.GetAt(i));
//			if (!ExecSQL(nConnetctNo,strSql))
//			{
//				AfxMessageBox(_T("更新信息表-5失败！"));
//				continue;
//			}
//
//			strSql = _T("delete  from newcjbdlast where thname=");
//			strSql += strArrayItem.GetAt(i);
//
//			if (!ExecSQL(nConnetctNo, strSql))
//			{
//				AfxMessageBox(_T("数据清除失败！"));
//				continue;
//			}
//						
//		}
//		else
//		{
//			continue;
//		}
//		theApp.m_iUpdateFinishPercent=min(i*1.0/strArrayItem.GetSize()*50,50);
//		Sleep(100);
//	}
//
//	theApp.m_iUpdateFinishPercent=50;
//	CStringArray strArrayShowKind ,strArrayBuyKindEname,strArrayBuyKindCname;
//
//	strSql=_T("SELECT SQL_NO_CACHE  kindname,c_name,showkind, BUYKINDENAME,BUYKINDCNAME FROM ITEMINFOANDDOWNLOADSTATE  where state=5 order by kindname ");
//	bstrSql=strSql.AllocSysString();
//	SysFreeString(bstrSql);
//	if( !OpenRecordSetEx(nConnetctNo,nRecord,bstrSql) )
//	{
//		AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
//		return 0;     
//	}
//	iNoCount=GetRecordCountEx(nConnetctNo,nRecord);
//	strEnameArray.RemoveAll();
//	strCnameArray.RemoveAll();
//	if (iNoCount>0)
//	{
//		CString strPreEname;
//		do 
//		{
//			CString strNowEname=CString(GetFieldValueEx(nConnetctNo,nRecord,0));
//			if (strEnameArray.GetSize()==0 || strPreEname!=strNowEname)
//			{
//				strEnameArray.Add(strNowEname);
//				strCnameArray.Add(CString(GetFieldValueEx(nConnetctNo,nRecord,1)));
//				if (CString(GetFieldValueEx(nConnetctNo,nRecord,2))==_T("0"))
//				{
//					strArrayShowKind.Add(_T("否"));
//				}
//				else
//				{
//					strArrayShowKind.Add(_T("是"));
//				}
//				strPreEname=strNowEname;
//
//				strArrayBuyKindEname.Add(CString(GetFieldValueEx(nConnetctNo, nRecord, 3)));
//				strArrayBuyKindCname.Add(CString(GetFieldValueEx(nConnetctNo, nRecord, 4)));
//
//			}
//
//		
//			
//
//		} while (MoveNextEx(nConnetctNo,nRecord));
//
//	}
//
//	CloseRecordSetEx(nConnetctNo,nRecord);
//	theApp.m_iUpdateFinishPercent=75;
//	Sleep(100);
//	for (int i=0;i<strEnameArray.GetSize()&& !theApp.m_bQuestClose;i++ )
//	{
//		//更新期刊类型表 FIRSTLEVELALL ZZHCSECONDLEVEL 
//			CString strKindId=strEnameArray.GetAt(i);
//		UINT nRecordNo;
//		strSql=_T("select SQL_NO_CACHE * from ZZHCSECONDLEVEL where ID=")+strKindId;
//		BSTR bstrSql=strSql.AllocSysString();
//		SysFreeString(bstrSql);
//		BOOL bNeedUpdate=FALSE;
//		if (OpenRecordSetEx(nConnetctNo,nRecordNo,bstrSql) && GetRecordCountEx(nConnetctNo,nRecordNo)==0)
//		{
//			bNeedUpdate=TRUE;
//		}
//		CloseRecordSetEx(nConnetctNo,nRecordNo);
//
//		if (bNeedUpdate)
//		{
//			strSql=_T("INSERT INTO ZZHCSECONDLEVEL (ID,C_NAME,是否精彩推荐,阅读次数,显示次序,");
//			strSql+=_T("KINDNAME,PARENTKIND,SHOWMODE,HAVECHILD,ITEMSHOWMODE,");
//			strSql+=_T("OPENOCXNO,ITEMBKMODE,TABLENAME,MOREKIND,DIRKIND,");
//			strSql+=_T("GROUPID,DETAILINFOTABLENAME,POSTFIX,KINDCNAME,GROUPCNAME,STATE) ");
//			strSql+=_T("VALUES(\"%s\",\"%s\",\"%s\",0,100,\"%s\",ZZHC,\"1,0,2\",否,0,1,0,BASEINFOBOOK,1,1,");
//			strSql+=_T("\"\",CJBDLAST,\"\",\"\",\"\",1)");
//			
//			CString strSqlTemp;
//			strSqlTemp.Format(strSql,strKindId,strCnameArray.GetAt(i),strArrayShowKind.GetAt(i),strArrayBuyKindEname.GetAt(i));
//			if (!ExecSQL(nConnetctNo,strSqlTemp))
//			{
//				AfxMessageBox(_T("插入SECONDLEVEL表失败！"));
//				return 0;
//			}
//		}
//
//
//		strSql = _T("select SQL_NO_CACHE * from FIRSTLEVELALL where PARENTKIND=ZZHC AND ID=") + strArrayBuyKindEname.GetAt(i);
//		bstrSql=strSql.AllocSysString();
//		SysFreeString(bstrSql);
//		bNeedUpdate=FALSE;
//		if (OpenRecordSetEx(nConnetctNo,nRecordNo,bstrSql) && GetRecordCountEx(nConnetctNo,nRecordNo)==0)
//		{
//			bNeedUpdate=TRUE;
//			//AfxMessageBox(strSql);
//		}
//		CloseRecordSetEx(nConnetctNo,nRecordNo);
//
//		if (bNeedUpdate)
//		{
//			strSql=_T("INSERT INTO FIRSTLEVELALL (ID,C_NAME,是否精彩推荐,阅读次数,显示次序,");
//			strSql+=_T("PARENTKIND,SHOWMODE,HAVECHILD,ITEMSHOWMODE,OPENOCXNO,");
//			strSql+=_T("ITEMBKMODE,TABLENAME,MOREKIND,DIRKIND,DETAILINFOTABLENAME,POSTFIX,ISPERSONUPLOAD) ");
//			strSql+=_T("VALUES(\"%s\",\"%s\",\"%s\",0,100,ZZHC,\"1,0,2\",否,0,1,0,\"\",1,1,CJBDLAST,NH,0)");
//			CString strSqlTemp;
//			strSqlTemp.Format(strSql, strArrayBuyKindEname.GetAt(i), strArrayBuyKindCname.GetAt(i),strArrayShowKind.GetAt(i));
//			if (!ExecSQL(nConnetctNo,strSqlTemp))
//			{
//				AfxMessageBox(_T("插入FIRSTLEVEL表失败！"));
//				return 0;
//			}
//		}	
//
//		strSql.Format(_T("update ITEMINFOANDDOWNLOADSTATE set state=6 where kindname=%s and state=5"),strKindId);
//		if (!ExecSQL(nConnetctNo,strSql))
//		{
//			AfxMessageBox(_T("更新信息表-6失败！"));
//			continue;
//
//
//		}
//
//		theApp.m_iUpdateFinishPercent=min(75+i*1.0*20/strArrayItem.GetSize(),90);
//	}
//
//	strSql=_T("SELECT SQL_NO_CACHE * FROM ITEMINFOANDDOWNLOADSTATE  where state=6");
//	bstrSql=strSql.AllocSysString();
//	SysFreeString(bstrSql);
//	nRecord=0;
//	if( !OpenRecordSetEx(nConnetctNo,nRecord,bstrSql) )
//	{
//		AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
//		return 0;     
//	}
//	iNoCount=GetRecordCountEx(nConnetctNo,nRecord);
//	CloseRecordSetEx(nConnetctNo,nRecord);
//
//
//
//	CString strEname;
//	if (iNoCount>0)
//	{
//		CString strResItemREC=theApp.m_strCanChangePath+_T("\\TempUpdate\\ResItemREC.txt");
//		if (PathFileExists(strResItemREC))
//		{
//			DeleteFile(strResItemREC);
//		}
//
//		strSql=_T("select SQL_NO_CACHE thname,kindname,c_name,年,期,UPDATEDATE,pages,otherpages from ITEMINFOANDDOWNLOADSTATE  where state=6 INTO ")+strResItemREC;
//
//		if(!ExecSQL(nConnetctNo,strSql))
//		{
//			AfxMessageBox(_T("数据导出失败！"));
//			return 0;
//		}
//
//		CString strScript;
//		CString strScriptPath=theApp.m_strCanChangePath+_T("\\TempUpdate\\Script.txt");
//		if (PathFileExists(strScriptPath))
//		{
//			DeleteFile(strScriptPath);
//		}
//		strScript=_T("CREATE OPTIMIZEKEY THNAME ON TABLE  ZZHCITEM \r\n");
//		strScript+=_T("INSERT INTO TABLE ZZHCITEM WITH  KEY=THNAME\r\n");
//		strScript+=_T("INFILE \"")+strResItemREC+_T("\"\r\n");
//		strScript+=_T("DROP OPTIMIZEKEY THNAME ON TABLE ZZHCITEM \r\n");
//
//		WriteTxtFile(strScriptPath,strScript);
//
//		strSql.Format(_T("DBUM UPDATE TABLE BY \"%s\" "),strScriptPath);
//		if(!ExecSQL(nConnetctNo,strSql))
//		{			
//			AfxMessageBox(_T("数据写入失败!"));
//			return 0;
//		}	
//		strSql=_T("DBUM refresh SORTFILE OF TABLE ZZHCITEM");
//
//		if(!ExecSQL(nConnetctNo,strSql))
//		{
//			AfxMessageBox(_T("更新用户KBase表失败!"));
//			return 0;
//		}
//
//	}
//
//	strSql=_T("delete from ITEMINFOANDDOWNLOADSTATE where state=6");
//	if (!ExecSQL(nConnetctNo,strSql))
//	{
//		AfxMessageBox(_T("删除更新完毕信息失败！"));
//		return 0;
//	}
//
//	theApp.m_iUpdateFinishPercent=100;
//
//	SysFreeString(bstrID);
//	SysFreeString(bstrUserName);
//	SysFreeString(bstrUserPassword);
//
//	return 1;
//}





int FindNowHour()
{
	CTime t = CTime::GetCurrentTime(); //获取系统日期

	return t.GetHour();
}




void CMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case TIME_MONITER:
	{
						 DWORD id1 = theApp.DetectProgectExist(_T("CAJTouchview_lywb.exe"));
						 //DWORD id3=DetectProgectExist(_T("GrabPaper_C.exe"));


						 //CTime t = CTime::GetCurrentTime(); //获取系统日期
						 //if (id3==0)
						 //{
						 //	if (t.GetHour()>=7 && t.GetHour()<20)
						 //	{
						 //		OpenLoadPaperPro();
						 //	}
						 //}

						 if (id1 == 0)
						 {
							 OpenCajTouchViewPro();
						 }
	}
		break;
	case DOWNLUPDATE_DATA_TIMER:

		break;

	case DOUPDATE_DATA_TIMER:

		break;

	case DOCLEAR_STALE_DATA:
		break;

	case DOCLEAR_INVALIDE_TABLEDATA:
		// 		{
		// 			WriteLog(_T("clearstart"));
		// 			CTime tim_now=CTime::GetCurrentTime();
		// 			if (tim_now.GetHour()>=23)
		// 			{
		// 				KillTimer(DOCLEAR_INVALIDE_TABLEDATA);
		// 				//CheckAndDelInvalidatePicVideo(0);
		// 				//CheckAndDelInvalidatePicVideo(1);
		// 				CheckAndDelInvalidateWebPage();
		// 				SetTimer(DOCLEAR_INVALIDE_TABLEDATA,30*60*1000,NULL);
		// 			}
		// 			WriteLog(_T("clearend"));
		// 		}
		break;
	case CLOS__WERFAULT_TIMER:
	{
								 DWORD id1 = theApp.DetectProgectExist(_T("WerFault.exe"));

								 if (id1 != 0)
								 {
									 theApp.ClosePross(_T("WerFault.exe"));
								 }

	}

		break;

		//DWORD id = 0;
		//	HANDLE   SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
		//	if(SnapShot==NULL) 
		//	{ 
		//		AfxMessageBox( _T("检测当前进程失败! ")); 
		//		return; 
		//	} 
		//	SHFILEINFO   shSmall; 
		//	CString strProName;
		//	PROCESSENTRY32   ProcessInfo;//声明进程信息变量 
		//	ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小 
		//	//返回系统中第一个进程的信息 
		//	BOOL   Status=Process32First(SnapShot,&ProcessInfo); 
		//	int   m_nProcess=0,num=0; 

		//	int iCount=0;
		//	while(Status) 
		//	{ 
		//		m_nProcess++; 
		//		num++; 
		//		//获取进程文件信息 
		//		SHGetFileInfo(ProcessInfo.szExeFile,0,&shSmall, 
		//			sizeof(shSmall),SHGFI_DISPLAYNAME/*SHGFI_ICON|SHGFI_SMALLICON*/); 
		//		//在列表控件中添加映像名称 
		//		strProName=ProcessInfo.szExeFile; 
		//		id=ProcessInfo.th32ProcessID;
		//		if (strProName==_T("WerFault.exe"))
		//		{
		//			CloseThread(id);

		//		}
		//		//获取下一个进程的信息 
		//		Status=Process32Next(SnapShot,&ProcessInfo); 
		//	} 
		//	CloseHandle(SnapShot); 
		//}



	case CLOS_WINDOW_TIMER:
	{

							  // 			WaitForSingleObject(m_hMutexOperatorKbas,INFINITE);
							  // 			//从数据库中读取
							  // 			if (!IsOpen()&&!Open(_T("DBOWN"),_T(""),m_sKbasId, 4567))
							  // 			{
							  // 				AfxMessageBox(_T("数据库连接失败！"));
							  // 				ReleaseMutex(m_hMutexOperatorKbas);
							  // 				break;;
							  // 			}
							  // 			CString strSql;
							  // 			strSql=_T("select SQL_NO_CACHE * from cjbdlast where thname=123");
							  // 
							  // 			BSTR bstrSql=strSql.AllocSysString();
							  // 			SysFreeString(bstrSql);
							  // 
							  // 			if( !OpenRecordSet(bstrSql) )
							  // 			{
							  // 				AfxMessageBox(_T( "OpenRecordSet Error\n" ));
							  // 				ReleaseMutex(m_hMutexOperatorKbas);
							  // 				break;     
							  // 			}
							  // 			UINT nNoCount=GetRecordCount();
							  // 			if (nNoCount<0)
							  // 			{
							  // 				CloseRecordSet();
							  // 				ReleaseMutex(m_hMutexOperatorKbas);
							  // 				break;
							  // 			}
							  // 			CString str=CString(GetFieldValue(2));
							  // 			AfxMessageBox(str);
							  // 			CloseRecordSet();
							  // 			ReleaseMutex(m_hMutexOperatorKbas);
							  // 
							  CTime tim_now = CTime::GetCurrentTime();
							  CString str_nowtime = tim_now.Format(_T("%H:%M"));

							  if (!theApp.ReadCloseTime())
							  {
								  AfxMessageBox(_T("读取关机时间失败！"));
								  return;
							  }
							  //readCloseTime(m_strCloseTime);
							  if (!theApp.m_strCloseTime.IsEmpty() && str_nowtime == theApp.m_strCloseTime)/* ||m_bRequestClose )*/
							  {
								  // 				if (m_bGetUpdateFromServer)
								  // 				{
								  // 					m_bRequestClose=TRUE;
								  // 					break;
								  // 				}
								  HideTaskBar(FALSE);

								  OSVERSIONINFO   OsVersionInfo;       //包含操作系统版本信息的数据结构 
								  OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
								  GetVersionEx(&OsVersionInfo);         //获取操作系统版本信息 
								  if (OsVersionInfo.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
								  {
									  HANDLE   hToken;
									  TOKEN_PRIVILEGES   tkp;   //   得到进程权标

									  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))   //得到关机的特权的LUID 
									  {
										  AfxMessageBox(_T("OpenProcessToken "));
										  return;
									  }

									  CloseThread();	// 等待线程关闭
									  LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

									  tkp.PrivilegeCount = 1;     //   设置一个特权          
									  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

									  //   //得到进程关机的特权    

									  AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

									  //   Cannot   test   the   return   value   of   AdjustTokenPrivileges.   

									  if (GetLastError() != ERROR_SUCCESS)
									  {
										  AfxMessageBox(_T("AdjustTokenPrivileges "));
										  return;
									  }

									  //   Shut   down   the   system   and   force   all   applications   to   close.   

									  if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF, 0))
										  //EWX_REBOOT,EWX_LOGOFF             (uFlags) 
										  //EWX_FORCE,EWX_FORCEIFHUNG     (Value) 
									  {
										  AfxMessageBox(_T("ExitWindowsEx "));
										  return;
									  }


								  }
								  else
								  {
									  //Windows98,调用ExitWindowsEx()函数重新启动计算机 
									  ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF, 0);     //可以改变第一个参数，实现注销用户、   
									  //关机、关闭电源等操作 


								  }
							  }
	}
		break;
	default:
		;
	}

	CDialog::OnTimer(nIDEvent);
}

// BOOL CMonitorDlg::FindAndUpdateData()
// {
// 	WriteLog(_T("updatestart"));
// 	CTime tim_now=CTime::GetCurrentTime();
// 	CStringArray strIdArray;
// 	if (GetNeedUpdateYearMonthArray(strIdArray))
// 	{
// 		CString strDir=_T("c:\\TempGet");
// 
// 		for (int i=0;i<strIdArray.GetSize();i++)
// 		{
// 			CString strPath;
// 			strPath.Format(_T("%s\\%s"),strDir,strIdArray.GetAt(i));
// 			if (PathFileExists(strPath))
// 			{
// 				CString strParam;
// 				strParam.Format(_T("[0][D:\\TempDir][%s]"),strPath);
// 				CString strID=strIdArray.GetAt(i);
// 				if (UpdateData(strParam))
// 				{
// 					UpdateFirstLevlTable();
// 					ClearDirectory(strPath);
// 					UpdateUpdateTable(_ttol(strID),3,0);
// 				}
// 
// 			}
// 			Sleep(2000);
// 		}
// 	}
// 	WriteLog(_T("updateend"));
// 
// 	return TRUE;
// }


// 
// void CMonitorDlg::OnTimer(UINT_PTR nIDEvent)
// {
// 	// TODO: Add your message handler code here and/or call default
// 	switch (nIDEvent)
// 	{
// 	case TIME_MONITER:
// 		{
// 			 DWORD id3=DetectProgectExist(_T("GrabPaper_C.exe"));
// 
// 			 CTime t = CTime::GetCurrentTime(); //获取系统日期
// 			 if (id3==0)
// 			 {
// 				 if (t.GetHour()>=6 && t.GetHour()<22)
// 				 {
// 					 OpenLoadPaperPro();
// 				 }
// 			 }
// 			 else
// 			 {
// 				 if (t.GetHour()>=22 || t.GetHour()<6)
// 				 {
// 					 CloseThread(id3);
// 				 }
// 				 // OpenLoadPaperPro();
// 			 }
// 
// 		 }
// 		break;
// 	case DOWNLUPDATE_DATA_TIMER:
// 
// 		if (!ReadxmlUpdateDate())
// 		{
// 			AfxMessageBox(_T("读取更新时间失败！"));
// 			return ;
// 		}
// 
// 		//下载更新包
// 		if (m_nlZzhcDate>=0 )
// 		{
// 			CTime t = CTime::GetCurrentTime(); //获取系统日期
// 			ULONG nlNowDate=t.GetYear()*100+t.GetMonth();
// 			ULONG nlTemp=m_nlZzhcDate;
// 			CString strUrl=m_strServerId;
// 			CString strDir=_T("c:\\TempGet");
// 			if (nlTemp==0)
// 			{
//                nlTemp=nlNowDate;
// 			   m_nLLoadPart=0;
// 			}
// 			else if (m_nLLoadPart==0)
// 			{
//                nlTemp+=1;
// 			}
// 			CTime tim_now=CTime::GetCurrentTime();
// 			while (nlTemp<=nlNowDate && (tim_now.GetHour()<18 || tim_now.GetHour()>20))
// 			{	
// 				tim_now=CTime::GetCurrentTime();
// 				CString strTempUrl;
// 				CString strTempDir;
// 
// 				strTempUrl.Format(_T("%s/ZZHC/%u.zip"),strUrl,nlTemp);
// 				strTempDir.Format(_T("%s\\%u.zip"),strDir,nlTemp);
// 
// 				if (!PathFileExists(strDir))
// 				{
// 					if (!CreateDirectory(strDir,NULL))
// 					{
// 						AfxMessageBox(_T("保存路径创建失败！"));
// 						return;
// 					}
// 
// 				}
// 
// 
// 				m_bGetUpdateFromServer=TRUE;
// 				UINT nResult=RequestHttp(strTempUrl,strTempDir);
// 				m_bGetUpdateFromServer=FALSE;
// 
// 				if (nResult!=RES_REQ_DONE_OK)
// 				{
// 					if (nResult==RES_REQ_ERROR_NOSPACE)
// 					{
// 						AfxMessageBox(_T("C盘空间不足！"));
//                         break;
// 					}
// 					else if (nResult==RES_REQ_ERROR_GET && PathFileExists(strTempDir))
// 					{
// 						//DeleteFile(strTempDir);
// 						  m_nlZzhcDate=nlTemp;
// 						UpdatexmlUpdateDate();
// 						m_nlZzhcDate=0;
// 						break;
// 					}
// 
// 				}
// 				else
// 				{
// 		/*			Sleep(1000);*/
//  					CZipImplement ZipTemp;
//  					if (!ZipTemp.Zip_UnPackFiles(strTempDir,strDir))
//  					{
//  						ClearDirectory(strDir);
//  						break;
//  					}
// 
// //g
// 					m_nlZzhcDate=nlTemp;
// 					DeleteFile(strTempDir);
// 					UpdatexmlUpdateDate();
// 
// 				 }
// 
// 				nlTemp+=1;
// 
// 				//ClearDirectory(strDir);
// 			}
// 		}
// 		
// 		break;
// 
// 	case DOUPDATE_DATA_TIMER:
// 		{
// 
// 			if (!m_strDvdPath.IsEmpty() && PathFileExists(m_strDvdPath))
// 			{
// 				if (UpdateData(_T("[1][D:\\TempDir][]")))
// 				{
// 					UpdatexmlUpdateDate();
// 					UpdateFirstLevlTable();
// 				}
// 				mciSendStringA("set cdaudio door open", 0, 0, 0);
// 			}
// 
// 			CTime tim_now=CTime::GetCurrentTime();
// 			if (m_nlZzhcDate<=0)
// 			{
// 				if (!ReadxmlUpdateDate())
// 				{
// 					AfxMessageBox(_T("读取更新时间失败！"));
// 					return ;
// 				}
// 			}
// 
// 			if (tim_now.GetHour()==12&& m_nlZzhcDate>0)
// 			{
// 
// 				ULONG nlTemp=m_nlZzhcDate;
// 				CString strDir=_T("c:\\TempGet");
// 
// 				if (m_nLLoadPart>0)
// 				{
// 					nlTemp-=1;
// 				}
// 				while (m_nlZzhcDate-nlTemp<10 && tim_now.GetHour()==12)
// 				{
// 					CString strPath;
// 					strPath.Format(_T("%s\\%u"),strDir,nlTemp);
// 					if (PathFileExists(strPath))
// 					{
// 						CString strParam;
// 						strParam.Format(_T("[0][D:\\TempDir][%s]"),strPath);
// 
// 						if (UpdateData(strParam))
// 						{
// 							UpdateFirstLevlTable();
// 							ClearDirectory(strPath);
// 						}
// 		
// 					}
// 					nlTemp--;
// 					tim_now=CTime::GetCurrentTime();
// 				
// 				}
// 			}
// 
// 		}
// 		break;
// 
// 	case DOCLEAR_STALE_DATA:
// 		if (theApp.m_nPaperSaveDayLength>0)
// 		{
// 			int i=0;
// 			CTime tim_now=CTime::GetCurrentTime();
// 			 while (i<m_strArrayCname.GetSize() && tim_now.GetHour()!=12)
// 			 {
// 				 if (PathFileExists(m_strDataPath+_T("\\BZZX\\")+m_strArrayCname.GetAt(i)))
// 				 {
// 					 CheckAndDelPaper(m_strDataPath+_T("\\BZZX\\")+m_strArrayCname.GetAt(i));
// 				 }
// 				 i++;
// 				 tim_now=CTime::GetCurrentTime();
// 			 }
// 		}
// 		break;
// 
// 	case CLOS__WERFAULT_TIMER:
// 		{
// 			DWORD id=0;
// 			HANDLE   SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
// 			if(SnapShot==NULL) 
// 			{ 
// 				AfxMessageBox( _T("检测当前进程失败! ")); 
// 				return; 
// 			} 
// 			SHFILEINFO   shSmall; 
// 			CString strProName;
// 			PROCESSENTRY32   ProcessInfo;//声明进程信息变量 
// 			ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小 
// 			//返回系统中第一个进程的信息 
// 			BOOL   Status=Process32First(SnapShot,&ProcessInfo); 
// 			int   m_nProcess=0,num=0; 
// 
// 			int iCount=0;
// 			while(Status) 
// 			{ 
// 				m_nProcess++; 
// 				num++; 
// 				//获取进程文件信息 
// 				SHGetFileInfo(ProcessInfo.szExeFile,0,&shSmall, 
// 					sizeof(shSmall),SHGFI_DISPLAYNAME/*SHGFI_ICON|SHGFI_SMALLICON*/); 
// 				//在列表控件中添加映像名称 
// 				strProName=ProcessInfo.szExeFile; 
// 				id=ProcessInfo.th32ProcessID;
// 				if (strProName==_T("WerFault.exe"))
// 				{
//                      CloseThread(id);
//                     
// 				}
// 				//获取下一个进程的信息 
// 				Status=Process32Next(SnapShot,&ProcessInfo); 
// 			} 
// 			CloseHandle(SnapShot); 
// 			return;
// 		}
// 
// 		break;
// 
// 	case CLOS_WINDOW_TIMER:
// 		{
//  
// 			CTime tim_now=CTime::GetCurrentTime();
// 			CString str_nowtime=tim_now.Format(_T("%H:%M:%S"));
// 
// 			if (!ReadCloseTime())
// 			{
// 				AfxMessageBox(_T("读取关机时间失败！"));
// 				return ;
// 			}
// 			//readCloseTime(m_strCloseTime);
// 			if (str_nowtime==m_strCloseTime ||m_bRequestClose )
// 			{
// 				m_bRequestClose=TRUE;
// 				if (m_bGetUpdateFromServer)
// 				{
// 					break;
// 				}
// 				HideTaskBar(TRUE);
// 
// 				OSVERSIONINFO   OsVersionInfo;       //包含操作系统版本信息的数据结构 
// 				OsVersionInfo.dwOSVersionInfoSize   =   sizeof(OSVERSIONINFO); 
// 				GetVersionEx(&OsVersionInfo);         //获取操作系统版本信息 
// 				if(OsVersionInfo.dwPlatformId   !=   VER_PLATFORM_WIN32_WINDOWS) 
// 				{ 
// 					HANDLE   hToken;   
// 					TOKEN_PRIVILEGES   tkp;   //   得到进程权标
// 
// 					if   (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))   //得到关机的特权的LUID 
// 					{
// 						AfxMessageBox( _T("OpenProcessToken ")); 
// 						return;
// 					}
// 
// 
// 					LookupPrivilegeValue(NULL,   SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);   
// 
// 					tkp.PrivilegeCount   =   1;     //   设置一个特权          
// 					tkp.Privileges[0].Attributes   =   SE_PRIVILEGE_ENABLED;   
// 
// 					//   //得到进程关机的特权    
// 
// 					AdjustTokenPrivileges(hToken, FALSE, &tkp,   0,(PTOKEN_PRIVILEGES)NULL,   0);   
// 
// 					//   Cannot   test   the   return   value   of   AdjustTokenPrivileges.   
// 
// 					if   (GetLastError()!=ERROR_SUCCESS) 
// 					{
// 						AfxMessageBox( _T("AdjustTokenPrivileges "));   
// 						return;
// 					}
// 
// 					//   Shut   down   the   system   and   force   all   applications   to   close.   
// 
// 					if   (!ExitWindowsEx(EWX_SHUTDOWN|EWX_POWEROFF,   0))   
// 						//EWX_REBOOT,EWX_LOGOFF             (uFlags) 
// 						//EWX_FORCE,EWX_FORCEIFHUNG     (Value) 
// 					{
// 						AfxMessageBox( _T("ExitWindowsEx "));   
// 						return;
// 					}
// 
// 
// 				} 
// 				else 
// 				{ 
// 					//Windows98,调用ExitWindowsEx()函数重新启动计算机 
// 					ExitWindowsEx(EWX_SHUTDOWN   |EWX_POWEROFF,0);     //可以改变第一个参数，实现注销用户、   
// 					//关机、关闭电源等操作 
// 
// 
// 				}
// 			}
// 		}
// 		break;
// 	default:
// 		;
// 	}
// 
// 	CDialog::OnTimer(nIDEvent);
// }




//BOOL CMonitorDlg::GetRegestNo()
//{
//	CString strRegistNo;
//	char lpszMac[128];
//	memset(lpszMac, 0x00, sizeof(lpszMac));
//	GetMac(lpszMac);
//	EnDecryption DecTemp;
//	DecTemp.GetRegistNo(CString(lpszMac),m_strRegestNo);
//	return TRUE;
//}

BOOL CMonitorDlg::IsNeedUpdateData_OneDayDowload()
{
	/*BOOL bNeedUpdate = FALSE;
	CString strSql = _T("SELECT SQL_NO_CACHE ID FROM UPDATEINFO  where state=2 order by ID DESC");
	BSTR bstrSql = strSql.AllocSysString();
	UINT nRecordNo = 0;
	if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecordNo, bstrSql))
	{
	SysFreeString(bstrSql);
	return bNeedUpdate;
	}
	SysFreeString(bstrSql);

	int nRecordCount = GetRecordCountEx(theApp.m_nConnectNo, nRecordNo);
	if (nRecordCount>0)
	{
	m_strArrayNeedUpdateName.RemoveAll();

	do
	{
	m_strArrayNeedUpdateName.Add(CString(GetFieldValueEx(theApp.m_nConnectNo, nRecordNo, 0)));
	} while (MoveNextEx(theApp.m_nConnectNo, nRecordNo));
	bNeedUpdate = TRUE;
	}

	CloseRecordSetEx(theApp.m_nConnectNo, nRecordNo);*/
	//return bNeedUpdate;
	return 0;
}


BOOL CMonitorDlg::DoUpdateData()
{
	CZipImplement ZipTemp;


	CString strDir = theApp.m_strCanChangePath + _T("\\TempGet\\cjfd\\");




	//KillTimer(UPDATE_DATA_TIMER);
	CString strParam;
	if (m_strArrayNeedUpdateName.GetSize()>0)
	{
		for (size_t i = 0; i < min(2, m_strArrayNeedUpdateName.GetSize()); i++)
		{
			strParam.Format(_T("[0][D:\\TempDir][%s%s\\CJFD]"), strDir, m_strArrayNeedUpdateName.GetAt(i));

			if (UpdateData(strParam))
			{
				UpdateUpdateTable(_ttol(m_strArrayNeedUpdateName.GetAt(i)), 3, 0);

			}
			else
			{
				return FALSE;
			}
		}




	}

	OpenCajTouchViewPro();

	return TRUE;

}


BOOL CMonitorDlg::IsNeedUpdateData()
{
	//BOOL bNeedUpdate=FALSE;

	////版本是否变化

	//CString strIniPath = theApp.m_strCanChangePath + _T("\\xml\\buykind.ini");

	//if (!PathFileExists(strIniPath))
	//{
	//	return bNeedUpdate;
	//}
	//else
	//{

	//	CString strBuykind,strNewBuyKind;
	//	::GetPrivateProfileStringW(_T("buykindno"), _T("No"), _T("NULL"), strBuykind.GetBuffer(MAX_PATH), MAX_PATH, strIniPath);
	//	strBuykind.ReleaseBuffer();
	//	::GetPrivateProfileStringW(_T("newbuykindno"), _T("No"), _T("NULL"), strNewBuyKind.GetBuffer(MAX_PATH), MAX_PATH, strIniPath);
	//	strNewBuyKind.ReleaseBuffer();
	//	if (strBuykind != _T("NULL") && strNewBuyKind != _T("NULL") && !strBuykind.IsEmpty() && !strNewBuyKind.IsEmpty() && strBuykind != strNewBuyKind)
	//	{
	//		CString strSql;
	//		strSql.Format(_T("delete form FIRSTLEVELALL where parentkind=zzhc"));
	//		if (!ExecSQL(theApp.m_nConnectNo, strSql))
	//		{
	//			AfxMessageBox(_T("清空newcjbdlast表失败!"));
	//			return 0;
	//		}

	//		strSql.Format(_T("delete form ZZHCITEM"));
	//		if (!ExecSQL(theApp.m_nConnectNo, strSql))
	//		{
	//			AfxMessageBox(_T("清空ZZHCITEM表失败!"));
	//			return 0;
	//		}

	//		strSql.Format(_T("delete form ZZHCSECONDLEVEL"));
	//		if (!ExecSQL(theApp.m_nConnectNo, strSql))
	//		{
	//			AfxMessageBox(_T("清空ZZHCSECONDLEVEL表失败!"));
	//			return 0;
	//		}

	//		strSql.Format(_T("delete form cjbdlast"));
	//		if (!ExecSQL(theApp.m_nConnectNo, strSql))
	//		{
	//			AfxMessageBox(_T("清空cjbdlast表失败!"));
	//			return 0;
	//		}
	//	}

	//	::WritePrivateProfileStringW(_T("buykindno"), _T("No"), strNewBuyKind, strIniPath);
	//}

	//CString strSql=_T("SELECT SQL_NO_CACHE * FROM ITEMINFOANDDOWNLOADSTATE  where  state>=4");
	//BSTR bstrSql=strSql.AllocSysString();
	//SysFreeString(bstrSql);
	//UINT nRecordNo=0;
	//if (!OpenRecordSetEx(theApp.m_nConnectNo,nRecordNo,bstrSql))
	//{
	//	return bNeedUpdate;
	//}

	//int nRecordCount =GetRecordCountEx(theApp.m_nConnectNo,nRecordNo);
	//if (nRecordCount>0)
	//{
	//      bNeedUpdate=TRUE;
	//}

	//CloseRecordSetEx(theApp.m_nConnectNo,nRecordNo);

	//return bNeedUpdate;
	return 0;
}

BOOL CMonitorDlg::GetFmPath(CString strKindId, CString& strfmPath)
{
	/*CString strSql ;
	strSql.Format(_T("select SHOWLEVEL,PICTOTALPATHFL,PICTOTALPATHTL from BASEKINDINFO where PYKM=\'%s\'"),strKindId);
	BSTR bstrSql=strSql.AllocSysString();
	SysFreeString(bstrSql);
	UINT nRecordNo=0;
	CMutex g_mutexBasekindinfoTable(false, _T("mutex_kbase_basekindinfo_operator"));
	g_mutexBasekindinfoTable.Lock();
	if (!OpenRecordSetEx(theApp.m_nConnectNo,nRecordNo,bstrSql))
	{
	g_mutexBasekindinfoTable.Unlock();
	return FALSE;
	}

	int nRecordCount = GetRecordCountEx(theApp.m_nConnectNo, nRecordNo);
	if (nRecordCount>0)
	{
	if (_ttoi(CString(GetFieldValueEx(theApp.m_nConnectNo, nRecordNo, 0))) == 1)
	{
	strfmPath = CString(GetFieldValueEx(theApp.m_nConnectNo, nRecordNo, 2));
	}
	else
	{
	strfmPath = CString(GetFieldValueEx(theApp.m_nConnectNo, nRecordNo, 1));
	}
	}


	CloseRecordSetEx(theApp.m_nConnectNo, nRecordNo);
	g_mutexBasekindinfoTable.Unlock();*/
	return TRUE;
}


BOOL CMonitorDlg::GetDataPath(CString strKindId, CString& strDataPath)
{
	/*BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	UINT nPort = _ttoi(theApp.m_strKbasePort);

	CMutex g_mutexBasekindinfoTable(false, _T("mutex_kbase_basekindinfo_operator"));
	g_mutexBasekindinfoTable.Lock();

	if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	{
	SysFreeString(bstrID);
	SysFreeString(bstrUserName);
	SysFreeString(bstrUserPassword);
	g_mutexBasekindinfoTable.Unlock();
	AfxMessageBox(_T("数据库连接失败！"));

	return FALSE;
	}
	SysFreeString(bstrID);
	SysFreeString(bstrUserName);
	SysFreeString(bstrUserPassword);

	CString strSql;
	strSql.Format(_T("SELECT DATAPATH FROM BASEKINDINFO where PYKM=\'%s\'"), strKindId);
	BSTR bstrSql = strSql.AllocSysString();
	SysFreeString(bstrSql);
	UINT nRecord = 0;
	if (!OpenRecordSetEx(theApp.m_nConnectNo, nRecord, bstrSql))
	{
	g_mutexBasekindinfoTable.Unlock();
	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
	return FALSE;
	}
	UINT nNoCount = GetRecordCountEx(theApp.m_nConnectNo, nRecord);
	if (nNoCount <= 0)
	{
	CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
	g_mutexBasekindinfoTable.Unlock();
	return FALSE;
	}

	strDataPath = CString(GetFieldValueEx(theApp.m_nConnectNo, nRecord, 0));
	CloseRecordSetEx(theApp.m_nConnectNo, nRecord);
	g_mutexBasekindinfoTable.Unlock();*/
	return TRUE;
}

BOOL CMonitorDlg::GetUpdateResultFromIni()
{
	CString str, strDate;
	CString strFilePath = theApp.m_strAppDir + _T("\\update\\CDSUpdate.ini");
	::GetPrivateProfileString(_T("IsDone"), _T("IsDone"), _T("Error"), str.GetBuffer(20), 20, strFilePath);
	str.ReleaseBuffer();
	str.TrimRight(_T("\\"));
	str.TrimRight();


	if (str == _T("FALSE"))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CMonitorDlg::GetUpdateResultFromIni(CString& strUpdateDate)
{
	CString str, strDate;
	CString strFilePath = theApp.m_strAppDir + _T("\\update\\CDSUpdate.ini");
	::GetPrivateProfileString(_T("IsDone"), _T("IsDone"), _T("Error"), str.GetBuffer(20), 20, strFilePath);
	str.ReleaseBuffer();
	str.TrimRight(_T("\\"));
	str.TrimRight();


	if (str == _T("FALSE"))
	{
		return FALSE;
	}
	else
	{
		::GetPrivateProfileString(_T("Disc"), _T("Disc"), _T("Error"), strDate.GetBuffer(20), 20, strFilePath);
		strDate.ReleaseBuffer();
		strDate.TrimRight();
		if (strDate.Left(4) == _T("CJBD"))
		{
			strUpdateDate = strDate.Right(strDate.GetLength() - 4);
		}
		return TRUE;
	}
}





void CMonitorDlg::GetDataAndFmPath(CString strKind, CString& strDataPath, CString& strFmPath)
{

	/*BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	UINT nPort = _ttoi(theApp.m_strKbasePort);

	if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	{
	SysFreeString(bstrID);
	SysFreeString(bstrUserName);
	SysFreeString(bstrUserPassword);
	AfxMessageBox(_T("数据库连接失败！"));
	return;
	}
	SysFreeString(bstrID);
	SysFreeString(bstrUserName);
	SysFreeString(bstrUserPassword);

	CString strSql;
	strSql.Format(_T("SELECT PICTOTALPATHTL,DATAPATH FROM BASEKINDINFO where PYKM=\'%s\'"),strKind);
	BSTR bstrSql=strSql.AllocSysString();
	SysFreeString(bstrSql);
	UINT nRecord=0;
	if( !OpenRecordSetEx(theApp.m_nConnectNo,nRecord,bstrSql) )
	{
	AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
	return;
	}
	UINT nNoCount=GetRecordCountEx(theApp.m_nConnectNo,nRecord);
	if (nNoCount<=0)
	{
	CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
	return;
	}

	strDataPath=CString(GetFieldValueEx(theApp.m_nConnectNo,nRecord,1));
	strFmPath=CString(GetFieldValueEx(theApp.m_nConnectNo,nRecord,0));
	CloseRecordSetEx(theApp.m_nConnectNo,nRecord);*/

}


void CMonitorDlg::CheckAndDelInvalidatePicVideo(UINT nKind)
{
	//BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	//BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//UINT nPort = _ttoi(theApp.m_strKbasePort);

	//if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	//{
	//	SysFreeString(bstrID);
	//	SysFreeString(bstrUserName);
	//	SysFreeString(bstrUserPassword);
	//	AfxMessageBox(_T("数据库连接失败！"));
	//	return;
	//}
	//SysFreeString(bstrID);
	//SysFreeString(bstrUserName);
	//SysFreeString(bstrUserPassword);

	////删除二级列表的索引信息
	//CString strSql,strSql2;
	//CStringArray strArrayInvalidateData;
	//if (nKind==0)
	//{
	//	strSql=_T("SELECT SQL_NO_CACHE ID FROM PERSONTPGSSECONDLEVEL where state=0");
	//	strSql2=_T("SELECT SQL_NO_CACHE ID FROM PERSONPICTUREINFO where ID=");
	//}
	//else if(nKind==1)
	//{
	//	strSql=_T("SELECT SQL_NO_CACHE ID FROM PERSONSPXSSECONDLEVEL where state=0");	
	//	strSql2=_T("SELECT SQL_NO_CACHE ID FROM PERSONVIDEOINFO where ID=");
	//}
	//else
	//{
	//	return;
	//}
	//BSTR bstrSql=strSql.AllocSysString();
	//SysFreeString(bstrSql);
	//UINT nRecord=0;
	//if( !OpenRecordSetEx(theApp.m_nConnectNo,nRecord,bstrSql) )
	//{
	//	AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
	//	return;     
	//}
	//UINT nNoCount=GetRecordCountEx(theApp.m_nConnectNo,nRecord);
	//if (nNoCount<=0)
	//{
	//	CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
	//	return; 
	//}

	//do 
	//{
	//	strArrayInvalidateData.Add(CString(GetFieldValueEx(theApp.m_nConnectNo,nRecord,0)));
	//} while (MoveNextEx(theApp.m_nConnectNo,nRecord));

	//CloseRecordSetEx(theApp.m_nConnectNo,nRecord);

	//if (strArrayInvalidateData.GetSize()>0)
	//{
	//	BSTR bstrSql=strSql.AllocSysString();
	//	SysFreeString(bstrSql);
	//	DeleteSelEx(theApp.m_nConnectNo,bstrSql);
	//}

	////删除信息表数据
	//CString strSqlInfo;
	//for (int i=0;i<strArrayInvalidateData.GetSize();i++)
	//{
	//	strSqlInfo=strSql2;
	//	strSqlInfo.AppendFormat(_T("%s"),strArrayInvalidateData.GetAt(i));

	//	BSTR bstrSqlInfo=strSqlInfo.AllocSysString();
	//	SysFreeString(bstrSqlInfo);
	//	nRecord=0;
	//	if(!OpenRecordSetEx(theApp.m_nConnectNo,nRecord,bstrSqlInfo))
	//	{
	//		AfxMessageBox(_T("OpenRecordSet Error\n" ));
	//		return;         
	//	}
	//	nNoCount=GetRecordCountEx(theApp.m_nConnectNo,nRecord);
	//	if (nNoCount<=0)
	//	{
	//		CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
	//		continue; 
	//	}

	//	CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
	//	DeleteSelEx(theApp.m_nConnectNo,bstrSqlInfo);

	//}

	////数据删除
	//CString strDataPathPre,strFmPathhPre;
	//if (nKind==0)
	//{		
	//	GetDataAndFmPath(_T("TPGS"),strDataPathPre,strFmPathhPre);

	//}
	//else if(nKind==1)
	//{
	//	GetDataAndFmPath(_T("SPXS"),strDataPathPre,strFmPathhPre);
	//}

	//for (int i=0;i<strArrayInvalidateData.GetSize();i++)
	//{
	//	CString strDataPath,strFmPath;

	//	if (nKind==1)
	//	{
	//		strFmPath=strFmPathhPre+_T("\\SPXS\\")+strArrayInvalidateData.GetAt(i)+_T(".jpg");
	//		if (PathFileExists(strFmPath))
	//		{
	//			DeleteFile(strFmPath);
	//		}

	//		strDataPath=strDataPathPre+_T("\\ZCSP\\")+strArrayInvalidateData.GetAt(i)+_T(".avi");
	//	}
	//	else if(nKind==0)
	//	{
	//		strDataPath=strDataPathPre+_T("\\ZCTP\\")+strArrayInvalidateData.GetAt(i)+_T(".jpg");;
	//	}

	//	if (PathFileExists(strDataPath))
	//	{
	//		DeleteFile(strDataPath);
	//	}

	//}

}

void CMonitorDlg::CheckAndDelInvalidateWebPage()
{
	/*BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	UINT nPort = _ttoi(theApp.m_strKbasePort);

	if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	{
	SysFreeString(bstrID);
	SysFreeString(bstrUserName);
	SysFreeString(bstrUserPassword);
	AfxMessageBox(_T("数据库连接失败！"));
	return;
	}
	SysFreeString(bstrID);
	SysFreeString(bstrUserName);
	SysFreeString(bstrUserPassword);

	CString strIntroduceFolder,strActivityFolder;
	CString strSql;
	strSql=_T("select * from CAJTOUCHSYSTEMSET");
	BSTR bstrSql=strSql.AllocSysString();
	SysFreeString(bstrSql);
	UINT nRecord=0;
	if( !OpenRecordSetEx(theApp.m_nConnectNo,nRecord,bstrSql) )
	{
	AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
	return ;
	}
	UINT nNoCount=GetRecordCountEx(theApp.m_nConnectNo,nRecord);
	if (nNoCount<=0)
	{
	CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
	return;
	}

	do
	{
	CString strId=CString(GetFieldValueEx(theApp.m_nConnectNo,nRecord,0));
	if (strId==_T("SS001"))
	{
	strIntroduceFolder=CString(GetFieldValueEx(theApp.m_nConnectNo,nRecord,2));
	}
	else if(strId==_T("SS002"))
	{
	strActivityFolder=CString(GetFieldValueEx(theApp.m_nConnectNo,nRecord,2));
	}
	} while (MoveNextEx(theApp.m_nConnectNo,nRecord));
	CloseRecordSetEx(theApp.m_nConnectNo,nRecord);

	TCHAR szDictPath[MAX_PATH];
	SHGetFolderPath(NULL,CSIDL_COMMON_APPDATA,NULL, 0,szDictPath);
	PathAppend(szDictPath, _T("\\TTKN\\CAJTouchViewerre\\html"));
	CString strWebPagePath = szDictPath;

	CString strWebPageIntroducePath=strWebPagePath+_T("\\User");
	if (!strIntroduceFolder.IsEmpty() && PathFileExists(strWebPageIntroducePath))
	{
	CFileFind m_ff;
	CString path=strWebPageIntroducePath;

	if (path.Right(1)!=_T("\\"))
	path+=_T("\\");

	path+=_T("*.*");
	BOOL res=m_ff.FindFile(path);
	CString stTempFileName;

	while(res)
	{
	res=m_ff.FindNextFile();
	if(m_ff.IsDots())
	continue;
	else if (m_ff.IsDirectory())
	{
	path=m_ff.GetFilePath();
	stTempFileName=m_ff.GetFileName();
	if (stTempFileName.Left(CString(_T("简介")).GetLength())==_T("简介"))
	{
	if (stTempFileName!=_T("简介")+strIntroduceFolder)
	{
	ClearDirectory(path);
	RemoveDirectory(path);
	}
	}

	}
	}
	m_ff.Close();
	}


	CString strWebPageActivityPath=strWebPagePath+_T("\\User\\近期活动");
	if (!strActivityFolder.IsEmpty() && PathFileExists(strWebPageActivityPath))
	{
	CFileFind m_ff;
	CString path=strWebPageActivityPath;

	if (path.Right(1)!=_T("\\"))
	path+=_T("\\");

	path+=_T("*.*");
	BOOL res=m_ff.FindFile(path);
	CString stTempFileName;

	while(res)
	{
	res=m_ff.FindNextFile();
	if(m_ff.IsDots())
	continue;
	else if (m_ff.IsDirectory())
	{
	path=m_ff.GetFilePath();
	stTempFileName=m_ff.GetFileName();
	if (stTempFileName.Left(CString(_T("近期活动")).GetLength())==_T("近期活动"))
	{
	if (stTempFileName!=_T("近期活动")+strActivityFolder)
	{
	ClearDirectory(path);
	RemoveDirectory(path);
	}
	}

	}
	}
	m_ff.Close();
	}
	*/
}

void CMonitorDlg::CheckAndDelPaper(CString strDelPaperPath)
{

	if (theApp.m_nPaperSaveDayLength <= 0 || theApp.m_nPaperSaveDayLength>365)
	{
		return;
	}

	CTime t = CTime::GetCurrentTime(); //获取系统日期

	CTimeSpan TimeSpan(theApp.m_nPaperSaveDayLength, 0, 0, 0);
	t = t - TimeSpan;

	CString strYear, strMonth, strDay;

	t.GetDay() / 10 ? strDay.Format(_T("%d"), t.GetDay()) : strDay.Format(_T("0%d"), t.GetDay());
	t.GetMonth() / 10 ? strMonth.Format(_T("%d"), t.GetMonth()) : strMonth.Format(_T("0%d"), t.GetMonth());
	t.GetYear() / 10 ? strYear.Format(_T("%d"), t.GetYear()) : strYear.Format(_T("0%d"), t.GetYear());

	long lCheck = _ttol(strYear + strMonth + strDay);


	CFileFind m_ff;
	CString path = strDelPaperPath;

	if (path.Right(1) != _T("\\"))
		path += _T("\\");

	path += _T("*.*");
	BOOL res = m_ff.FindFile(path);
	CString strTempFileName;
	CString strTempFilePath;

	while (res)
	{
		res = m_ff.FindNextFile();
		if (!m_ff.IsDirectory() && !m_ff.IsDots())
		{
			strTempFileName = m_ff.GetFileName();
			if (_ttol(strTempFileName.Left(8))<(long)lCheck && strTempFileName.Left(5) != _T("temp."))
			{
				strTempFilePath = m_ff.GetFilePath();
				DeleteFile(strTempFilePath);
			}
		}
		else if (m_ff.IsDots())
			continue;
		else if (m_ff.IsDirectory())
		{
			path = m_ff.GetFilePath();
			strTempFileName = m_ff.GetFileName();
			if (_ttol(strTempFileName.Left(8))<(long)lCheck)
			{
				WriteLog(_T("---") + path);
				ClearDirectory(path);
				//目录为空后删除目录
				RemoveDirectory(path);
			}

		}
	}

	m_ff.Close();

}




HWND CMonitorDlg::GetWindowHandleByPID(DWORD dwProcessID)
{
	HWND h = GetTopWindow()->m_hWnd;
	while (h)
	{
		DWORD pid = 0;
		DWORD dwTheardId = GetWindowThreadProcessId(h, &pid);

		if (dwTheardId != 0)
		{
			if (pid == dwProcessID/*your process id*/)
			{
				// here h is the handle to the window
				return h;
			}
		}


		h = GetNextWindow(GW_HWNDNEXT)->m_hWnd;
	}

	return NULL;
}


void CMonitorDlg::HideTaskBar(BOOL bHide)

{

	int nCmdShow;

	HWND hWnd;

	LPARAM lParam;



	hWnd = FindWindow(_T("Shell_TrayWnd"), NULL)->m_hWnd;

	if (bHide == TRUE)

	{

		nCmdShow = SW_HIDE;

		lParam = ABS_AUTOHIDE | ABS_ALWAYSONTOP;

	}

	else

	{

		nCmdShow = SW_SHOW;

		lParam = ABS_ALWAYSONTOP;

	}



	::ShowWindow(hWnd, nCmdShow);//隐藏任务栏 





#ifndef ABM_SETSTATE 

#define ABM_SETSTATE 0x0000000a 

#endif 



	APPBARDATA apBar;

	memset(&apBar, 0, sizeof(apBar));

	apBar.cbSize = sizeof(apBar);

	apBar.hWnd = hWnd;

	if (apBar.hWnd != NULL)

	{

		apBar.lParam = lParam;

		SHAppBarMessage(ABM_SETSTATE, &apBar); //设置任务栏自动隐藏 

	}

}


void CMonitorDlg::GetDvdPath()
{
	TCHAR buf[100];
	DWORD len = GetLogicalDriveStrings(sizeof(buf) / sizeof(TCHAR), buf);
	// 显示每个驱动器的信息

	for (TCHAR* s = buf; *s; s += _tcslen(s) + 1)
	{
		LPCTSTR sDrivePath = s;
		UINT uDriveType = GetDriveType(sDrivePath);
		if (DRIVE_CDROM == uDriveType)
		{
			theApp.m_strDvdPath = CString(sDrivePath);
			break;
		}

	}

	return;
}

void CMonitorDlg::GetPaperCnameArray()
{

	/*BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	UINT nPort = _ttoi(theApp.m_strKbasePort);

	if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	{
	SysFreeString(bstrID);
	SysFreeString(bstrUserName);
	SysFreeString(bstrUserPassword);
	AfxMessageBox(_T("数据库连接失败！"));
	return;
	}
	SysFreeString(bstrID);
	SysFreeString(bstrUserName);
	SysFreeString(bstrUserPassword);


	CString strSql;
	strSql=_T("SELECT C_NAME FROM DOWNLAODPAPERIF");
	BSTR bstrSql=strSql.AllocSysString();
	SysFreeString(bstrSql);
	UINT nRecord=0;
	if( !OpenRecordSetEx(theApp.m_nConnectNo,nRecord,bstrSql) )
	{
	AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
	return ;
	}
	UINT nNoCount=GetRecordCountEx(theApp.m_nConnectNo,nRecord);
	if (nNoCount<=0)
	{
	CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
	return ;
	}

	m_strArrayCname.RemoveAll();
	do
	{
	m_strArrayCname.Add(CString(GetFieldValueEx(theApp.m_nConnectNo,nRecord,0)));
	} while (MoveNextEx(theApp.m_nConnectNo,nRecord));

	CloseRecordSetEx(theApp.m_nConnectNo,nRecord);*/
}


void CMonitorDlg::OpenLoadPaperPro()
{
	CString strTemp = theApp.m_strAppDir + _T("\\GrabPaper_C.exe");
	if (theApp.DetectProgectExist(_T("GrabPaper_C.exe")) == 0)
	{
		SHELLEXECUTEINFO  ShExecInfo;
		//memset(&ShExecInfo, 0, sizeof(ShExecInfo));
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = NULL;
		ShExecInfo.hwnd = NULL;
		//ShExecInfo.lpVerb    = _T("runas");  
		ShExecInfo.lpVerb = _T("open");

		ShExecInfo.lpFile = strTemp; // 执行的程序名 
		ShExecInfo.lpParameters = NULL;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOWNORMAL;              // 全屏显示这个程序 
		ShExecInfo.hInstApp = NULL;

		ShellExecuteEx(&ShExecInfo);

		//WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	}

}


void CMonitorDlg::OpenUpdateDataPro(CString strParam)
{
	CString strTemp = theApp.m_strAppDir + _T("\\update\\TSCDSUpdate.exe");
	if (theApp.DetectProgectExist(_T("TSCDSUpdate.exe")) == 0)
	{
		SHELLEXECUTEINFO  ShExecInfo;
		//memset(&ShExecInfo, 0, sizeof(ShExecInfo));
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = NULL;
		ShExecInfo.hwnd = NULL;
		//ShExecInfo.lpVerb    = _T("runas");  
		ShExecInfo.lpVerb = _T("open");

		ShExecInfo.lpFile = strTemp; // 执行的程序名 
		ShExecInfo.lpParameters = strParam;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOWNORMAL;              // 全屏显示这个程序 
		ShExecInfo.hInstApp = NULL;

		ShellExecuteEx(&ShExecInfo);

		//WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	}
}


void CMonitorDlg::OpenCajTouchViewPro()
{
	//DWORD id=DetectProgectExist(_T("Init.exe"));

	DWORD id1 = theApp.DetectProgectExist(_T("CAJTouchview_lywb.exe"));

	//DWORD id2=DetectProgectExist(_T("freespace.exe"));

	//DWORD id3 = theApp.DetectProgectExist(_T("TSCDSUpdate.exe"));


	HideTaskBar(TRUE);
	if (/*id==0&&*/id1 == 0/*&&id2==0&&id3==0*/)
	{

		//重启程序CAJTouchViewer
		CString strTemp = theApp.m_strAppDir + _T("\\CAJTouchview_lywb.exe");
		SHELLEXECUTEINFO  ShExecInfo;
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = NULL;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = _T("open");
		ShExecInfo.lpFile = strTemp; // 执行的程序名 
		ShExecInfo.lpParameters = NULL;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SHOW_FULLSCREEN;              // 全屏显示这个程序 
		ShExecInfo.hInstApp = NULL;

		ShellExecuteEx(&ShExecInfo);

		//		 WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

		//                  idm=DetectProgectExist(_T("main.exe"));
		// 
		//  				// CWnd::FromHandle(GetWindowHandleByPID(idm))->ShowWindow(SW_HIDE);
		// 
		// 				 //把窗体设为前台
		// 
		// 				 CWnd::FromHandle(GetWindowHandleByPID(idm))->SetForegroundWindow();
		// 
		// 				 //隐藏后把窗体设置为全屏
		// 
		// 				 CRect rcScreen;
		// 				 rcScreen.SetRect( 0, 0, GetSystemMetrics( SM_CXSCREEN ), 
		// 
		// 					 GetSystemMetrics( SM_CYSCREEN ));
		// 				 CWnd::FromHandle(GetWindowHandleByPID(idm))->MoveWindow(&rcScreen, TRUE);
		// 
		// 				 AfxMessageBox(_T("12"));
		// 
		// 				 //CWnd::FromHandle(GetWindowHandleByPID(idm))->ShowWindow(SW_SHOW);
	}
}


BOOL CMonitorDlg::UpdateData(CString strParam, CString& strUpdateDate)
{
	KillTimer(TIME_MONITER);
	// 	KillTimer(UPDATE_DATA_TIMER);
	theApp.ClosePross(_T("CAJTouchview_lywb.exe"));
	//ClosePross(_T("Init.exe"));
	OpenUpdateDataPro(strParam);
	while (theApp.ProssExist(_T("TSCDSUpdate.exe")))
	{
		Sleep(1000);
	}
	SetTimer(TIME_MONITER, 1000, NULL);


	return GetUpdateResultFromIni(strUpdateDate);

	//mciSendStringA("set cdaudio door open", 0, 0, 0);
	// 	SetTimer(TIME_MONITER,1000,NULL);
	// 	SetTimer(UPDATE_DATA_TIMER,1000,NULL);
}

BOOL CMonitorDlg::UpdateData(CString strParam)
{
	KillTimer(TIME_MONITER);
	// 	KillTimer(UPDATE_DATA_TIMER);
	theApp.ClosePross(_T("CAJTouchview_lywb.exe"));
	//ClosePross(_T("Init.exe"));
	OpenUpdateDataPro(strParam);
	while (theApp.ProssExist(_T("TSCDSUpdate.exe")))
	{
		Sleep(1000);
	}
	SetTimer(TIME_MONITER, 1000, NULL);


	return GetUpdateResultFromIni();

}

BOOL CMonitorDlg::UpdateUpdateTable(LONG LID, UINT nState, LONGLONG lPart)
{

	//BSTR bstrID = theApp.m_strKbaseId.AllocSysString();
	//BSTR bstrUserName = theApp.m_strKbaseUserName.AllocSysString();
	//BSTR bstrUserPassword = theApp.m_strKbasePassWord.AllocSysString();
	//UINT nPort = _ttoi(theApp.m_strKbasePort);

	//if (!IsOpenEx(theApp.m_nConnectNo) && !OpenEx(theApp.m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	//{
	//	SysFreeString(bstrID);
	//	SysFreeString(bstrUserName);
	//	SysFreeString(bstrUserPassword);
	//	AfxMessageBox(_T("数据库连接失败！"));
	//	return FALSE;
	//}
	//SysFreeString(bstrID);
	//SysFreeString(bstrUserName);
	//SysFreeString(bstrUserPassword);

	//CString strSql;
	//strSql.Format(_T("SELECT * FROM UPDATEINFO WHERE ID=%ld"),LID);
	//BSTR bstrSql=strSql.AllocSysString();
	//SysFreeString(bstrSql);
	//UINT nRecord=0;
	//if( !OpenRecordSetEx(theApp.m_nConnectNo,nRecord,bstrSql) )
	//{
	//	AfxMessageBox(_T( "OpenRecordSet Error\n" )+strSql);
	//	return FALSE;     
	//}
	//UINT nNoCount=GetRecordCountEx(theApp.m_nConnectNo,nRecord);
	//CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
	//if (nNoCount<0)
	//{	
	//	return FALSE; 
	//}


	//CString strSetTemp;
	//if (nState==1)
	//{
	//	LONGLONG llTemp=(LONGLONG)powl(10,9);
	//	if (ULONG(lPart/llTemp)>0)
	//	{
	//		strSetTemp.Format(_T("%u%09u"),ULONG(lPart/llTemp),ULONG(lPart%llTemp));
	//	}
	//	else
	//	{
	//		strSetTemp.Format(_T("%u"),lPart);
	//	}
	//}

	//if (nNoCount==0)
	//{
	//	strSql=_T("INSERT INTO UPDATEINFO (ID,STATE,DOWNLPART,KINDID)");
	//	strSql.AppendFormat(_T("VALUES(\"%ld\",\"%d\",\"%s\",ZZHC)"),LID,nState,strSetTemp);
	//	ExecSQL(theApp.m_nConnectNo, strSql);
	//}
	//else
	//{
	//	strSql.Format(_T("SELECT * FROM UPDATEINFO WHERE ID=%ld"),LID);

	//	BSTR bstrSql=strSql.AllocSysString();
	//	SysFreeString(bstrSql);
	//	if( !OpenRecordSetEx(theApp.m_nConnectNo,nRecord,bstrSql) )
	//	{
	//		AfxMessageBox(_T( "OpenRecordSet Error\n" ));
	//		return  FALSE;         
	//	}
	//	CString strState;
	//	strState.Format(_T("%d"),nState);
	//	//Edit();
	//	BSTR bstrState=strState.AllocSysString();
	//	SysFreeString(bstrState);
	//	SetFieldValueEx(theApp.m_nConnectNo,nRecord,bstrState,1);
	//	BSTR bstrSetTemp=strSetTemp.AllocSysString();
	//	SysFreeString(bstrSetTemp);
	//	SetFieldValueEx(theApp.m_nConnectNo,nRecord,bstrSetTemp,2);
	//	//Update();
	//	CloseRecordSetEx(theApp.m_nConnectNo,nRecord);
	//}
	return TRUE;
}




// 请求



// BOOL CMonitorDlg::IsHaveUpdateData(ULONG nlYearMonth)
// {
// 	BOOL bHaveData=FALSE;
// 	WaitForSingleObject(m_hMutexOperatorKbas,INFINITE);
// 	if (!IsOpen()&&!Open(_T("DBOWN"),_T(""),m_sKbasId, 4567))
// 	{
// 		AfxMessageBox(_T("数据库连接失败！"));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return FALSE;
// 	}
// 	CString strSql;
// 	strSql.Format(_T("SELECT SQL_NO_CACHE * from UPDATEPACKAGEINFO where ID=\"%ld\""),nlYearMonth);
// 	BSTR bstrSql=strSql.AllocSysString();
// 	SysFreeString(bstrSql);
// 	if(!OpenRecordSet(bstrSql))
// 	{
// 		AfxMessageBox(_T("OpenRecordSet Error\n" ));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return FALSE;         
// 	}
// 	int nRecordCount =GetRecordCount();
// 	if (nRecordCount>0)
// 	{
// 		bHaveData=TRUE;
// 	}
// 	CloseRecordSet();
// 	ReleaseMutex(m_hMutexOperatorKbas);
// 	return bHaveData;
// }


// BOOL CMonitorDlg::UpdateFirstLevlTable()
// {
// 	WaitForSingleObject(m_hMutexOperatorKbas,INFINITE);
// 	if (!IsOpen()&&!Open(_T("DBOWN"),_T(""),m_sKbasId, 4567))
// 	{
// 		AfxMessageBox(_T("数据库连接失败！"));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return FALSE;
// 	}
// 	CString strSql;
// 	CStringArray strArrayAddEs,strArrayAddCs;
// 	strSql=_T("SELECT SQL_NO_CACHE 拼音刊名,中文刊名 FROM CJBDLAST group by 拼音刊名");
// 	BSTR bstrSql=strSql.AllocSysString();
// 	SysFreeString(bstrSql);
// 	if(!OpenRecordSet(bstrSql))
// 	{
// 		AfxMessageBox(_T("OpenRecordSet Error\n" ));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return FALSE;         
// 	}
// 	int nRecordCount =GetRecordCount();
// 	if (nRecordCount>0)
// 	{
// 		do 
// 		{
// 			CString strTemp=GetFieldValue(0);
// 			CString strTemp1=GetFieldValue(1);
// 			//		   strSql.Format(_T("SELECT * FROM FIRSTLEVELALL WHERE ID=%s and PARENTKIND=ZZHC"),strTemp);
// 			strSql.Format(_T("SELECT * FROM SPLENDID_RANK_INFO WHERE PYKM=%s"),strTemp);
// 			BSTR bstrSql=strSql.AllocSysString();
// 			SysFreeString(bstrSql);
// 			if(!OpenRecordSet(bstrSql))
// 			{
// 				AfxMessageBox(_T("OpenRecordSet Error\n" ));
// 				ReleaseMutex(m_hMutexOperatorKbas);
// 				return FALSE;         
// 			}
// 			nRecordCount =GetRecordCount();
// 			if (nRecordCount<=0)
// 			{
// 				strArrayAddEs.Add(strTemp);
// 				strArrayAddCs.Add(strTemp1);
// 			}
// 			CloseRecordSet();
// 
// 		} while (MoveNext());
// 	}
// 	CloseRecordSet();
// 
// 
// 	for (int i=0;i<strArrayAddEs.GetSize();i++)
// 	{
// 		if (!IsOpen()&&!Open(_T("DBOWN"),_T(""),m_sKbasId, 4567))
// 		{
// 			AfxMessageBox(_T("数据库连接失败！"));
// 			ReleaseMutex(m_hMutexOperatorKbas);
// 			return FALSE;
// 		}
// 
// 		strSql=_T("INSERT INTO SPLENDID_RANK_INFO (PYKM,C_NAME,是否精彩推荐,阅读次数,是否首页显示)");
// 		strSql.AppendFormat(_T("VALUES(%s,\"%s\",否,0,否)"),strArrayAddEs.GetAt(i),strArrayAddCs.GetAt(i));
// 		// 		strSql=_T("INSERT INTO FIRSTLEVELALL (ID,C_NAME,是否精彩推荐,阅读次数,显示次序,");
// 		// 		strSql+=_T("PARENTKIND,SHOWMODE,HAVECHILD,ITEMSHOWMODE,OPENOCXNO,");
// 		// 		strSql+=_T("ITEMBKMODE,TABLENAME,MOREKIND,DIRKIND,DETAILINFOTABLENAME,POSTFIX)");
// 		// 		strSql.AppendFormat(_T("VALUES(%s,\"%s\",否,0,100,ZZHC,\"1,0,2\",否,0,1,0,\"\",1,1,CJBDLAST,NH)"),strArrayAddEs.GetAt(i),strArrayAddCs.GetAt(i));
// 		InsertValue(strSql);
// 
// 	}
// 	ReleaseMutex(m_hMutexOperatorKbas);
// 	return TRUE;
// }



// UINT CMonitorDlg::GetUpdateState(ULONG nlYearMonth,CString& strDownedPart)
// {
// 	UINT nState=0;
// 	WaitForSingleObject(m_hMutexOperatorKbas,INFINITE);
// 	if (!IsOpen()&&!Open(_T("DBOWN"),_T(""),m_sKbasId, 4567))
// 	{
// 		AfxMessageBox(_T("数据库连接失败！"));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return 0;
// 	}
// 	CString strSql;
// 	strSql.Format(_T("SELECT STATE,DOWNLPART FROM UPDATEINFO WHERE ID=%ld"),nlYearMonth);
// 
// 	BSTR bstrSql=strSql.AllocSysString();
// 	SysFreeString(bstrSql);
// 	if(!OpenRecordSet(bstrSql))
// 	{
// 		AfxMessageBox(_T("OpenRecordSet Error\n" ));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return 0;         
// 	}
// 	int nRecordCount =GetRecordCount();
// 	if (nRecordCount==0)
// 	{
// 		nState=0;
// 	}
// 	else if(nRecordCount>0)
// 	{
// 		nState=_ttoi(CString(GetFieldValue(0)));
// 		if (nState==1)
// 		{
// 			strDownedPart=CString(GetFieldValue(1));
// 		}
// 	}
// 	CloseRecordSet();
// 	ReleaseMutex(m_hMutexOperatorKbas);
// 	return nState;
// }



// BOOL CMonitorDlg::ClearUnusableData()
// {
// 	CStringArray strArray;
// 	WaitForSingleObject(m_hMutexOperatorKbas,INFINITE);
// 	if (!IsOpen()&&!Open(_T("DBOWN"),_T(""),m_sKbasId, 4567))
// 	{
// 		AfxMessageBox(_T("数据库连接失败！"));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return 0;
// 	}
// 	CString strSql;
// 	CStringArray strArrayAddEs,strArrayAddCs;
// 	strSql=_T("SELECT SQL_NO_CACHE ID FROM UPDATEINFO WHERE STATE=3");
// 
// 	BSTR bstrSql=strSql.AllocSysString();
// 	SysFreeString(bstrSql);
// 	if(!OpenRecordSet(bstrSql))
// 	{
// 		AfxMessageBox(_T("OpenRecordSet Error\n" ));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return 0;         
// 	}
// 	int nRecordCount =GetRecordCount();
// 
// 	if(nRecordCount>0)
// 	{
// 		strArray.Add(CString(GetFieldValue(0)));
// 
// 	}
// 	CloseRecordSet();
// 	ReleaseMutex(m_hMutexOperatorKbas);
// 
// 	for (int i=0;i<strArray.GetSize();i++)
// 	{
// 			CString strPath;
// 			strPath.Format(_T("c:\\TempGet\\%s"),strArray.GetAt(i));
// 			if (PathFileExists(strPath))
// 			{
// 				ClearDirectory(strPath);
// 			}
// 	}
// 	return TRUE;
// }


// BOOL CMonitorDlg::GetNeedUpdateYearMonthArray(CStringArray& strArray)
// {
// 	WaitForSingleObject(m_hMutexOperatorKbas,INFINITE);
// 	if (!IsOpen()&&!Open(_T("DBOWN"),_T(""),m_sKbasId, 4567))
// 	{
// 		AfxMessageBox(_T("数据库连接失败！"));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return 0;
// 	}
// 	CString strSql;
// 	CStringArray strArrayAddEs,strArrayAddCs;
// 	strSql=_T("SELECT SQL_NO_CACHE ID FROM UPDATEINFO WHERE STATE=2");
// 
// 	BSTR bstrSql=strSql.AllocSysString();
// 	SysFreeString(bstrSql);
// 	if(!OpenRecordSet(bstrSql))
// 	{
// 		AfxMessageBox(_T("OpenRecordSet Error\n" ));
// 		ReleaseMutex(m_hMutexOperatorKbas);
// 		return 0;         
// 	}
// 	int nRecordCount =GetRecordCount();
// 
// 	if(nRecordCount>0)
// 	{
// 		strArray.Add(CString(GetFieldValue(0)));
// 
// 	}
// 	CloseRecordSet();
// 	ReleaseMutex(m_hMutexOperatorKbas);
// 	return TRUE;
// }




void CMonitorDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (theApp.m_bQuestClose)
	{
		return;
	}
	else
	{
		theApp.m_bQuestClose = TRUE;
	}


	//if (m_hThreadGetUpdataInfoData_ShfwSet != NULL)
	//{
	//	WaitForSingleObject(m_hThreadGetUpdataInfoData_ShfwSet, INFINITE);

	//}


	//if (m_hThreadGetUpdataInfoData_PersonSet != NULL)
	//{
	//	WaitForSingleObject(m_hThreadGetUpdataInfoData_PersonSet, INFINITE);

	//}
	//if (m_hThreadGetUpdataInfoData != NULL)
	//{
	//	WaitForSingleObject(m_hThreadGetUpdataInfoData, INFINITE);

	//}

	//if (m_hThreadUpdateData != NULL)
	//{
	//	WaitForSingleObject(m_hThreadUpdateData, INFINITE);
	//}
	if (m_hGetAppUpdateInfoData)
	{
		WaitForSingleObject(m_hGetAppUpdateInfoData, INFINITE);
	}


	if (m_hThreadLoad[0])
	{
		WaitForSingleObject(m_hThreadLoad[0], INFINITE);
	}

	if (m_hThreadLoad[1])
	{
		WaitForSingleObject(m_hThreadLoad[1], INFINITE);
	}
	if (m_hThreadLoad[2])
	{
		WaitForSingleObject(m_hThreadLoad[2], INFINITE);
	}
	//if (m_hClearInvalidateData)
	//{
	//	WaitForSingleObject(m_hClearInvalidateData, INFINITE);

	//}
	//if (m_hTheradGetNewData)
	//{
	//	WaitForSingleObject(m_hClearInvalidateData, INFINITE);

	//}
	HideTaskBar(FALSE);
	CDialog::OnClose();
}

void CMonitorDlg::CloseThread()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (theApp.m_bQuestClose)
	{
		return;
	}
	else
	{
		theApp.m_bQuestClose = TRUE;
	}

	if (m_hGetAppUpdateInfoData)
	{
		WaitForSingleObject(m_hGetAppUpdateInfoData, INFINITE);
	}

	// 等待线程结束
	if (m_hThreadLoad[0])
	{
		WaitForSingleObject(m_hThreadLoad[0], INFINITE);
	}

	if (m_hThreadLoad[1])
	{
		WaitForSingleObject(m_hThreadLoad[1], INFINITE);
	}
	if (m_hThreadLoad[2])
	{
		WaitForSingleObject(m_hThreadLoad[2], INFINITE);
	}
}