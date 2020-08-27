#include <stdio.h>
#include "stdafx.h" 
#include "Monitor.h"
#include "MonitorDlg.h"
#include <afxinet.h>  
#import <msxml3.dll> 
#include <iostream>
#include <string>
#include <cstring>
#include"RegistInfoDlg.h"
#include"SetKbaseInfoDlg.h"

#include "tlhelp32.h" 
/*#include "sha3.h"*/
#include "Aes.h"
#include "Base64.h"
#define  BUFFER_SIZE  5*1024
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


SYSTEMTIME st;
CString logInfo; 



// CMonitorApp

BEGIN_MESSAGE_MAP(CMonitorApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMonitorApp construction

CMonitorApp::CMonitorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_bRequestClose = FALSE;
	m_nGetInfoVersion = 0;
	m_nConnectNo = 0;
	DelayLoadReaderExDLL();
}


// The one and only CMonitorApp object

CMonitorApp theApp;


// CMonitorApp initialization

BOOL WriteLog(CString strWrite)
{

	CString strFileName = _T("d:\\myfile.txt");

	BOOL bNeedSetUnicode = FALSE;

	CFile mFile;
	if (mFile.Open(strFileName, CFile::modeRead | CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite))
	{
		mFile.SeekToEnd();

		mFile.Write("\xff\xfe", 2);

		mFile.Write(strWrite, strWrite.GetLength()*sizeof(wchar_t));

		mFile.Close();
		return TRUE;
	}
	return FALSE;
}


void WriteLogTHMain(CString strWrite, int number)
{

	GetLocalTime(&st);
	logInfo.Format(_T("%s : at %.2d:%.2d:%.2d:%.3d. Line %d."), 
					strWrite, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, number);
	CString strLogPath = theApp.m_strCanChangePath + _T("\\TempUpdate\\MonitorlogTHMain.txt");
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



BOOL getInfoFromUpdateInfo()
{
    if (!tableExist(_T("UPDATEINFO"))) {
		AfxMessageBox(_T("Database is not complete! Please contact with Database Administrator.\n"));
        return FALSE;
	}

	CString strSqlSelect = _T("select ID, DOWNLOADURL, CONTENT, REQUESTKIND, REQUESTSUBKIND, ORDERNO,")
						   _T(" RequestTimeGap, PRIORITY, TABLEINFO, LASTUPDATESTANDER FROM UPDATEINFO ORDER BY ORDERNO;");
	CSqlStatement *stmt = NULL;
	
R
	while (!theApp.LockRead(INFINITE, 10));

	stmt = theApp.m_sqlite.Statement(strSqlSelect);
	if (stmt != NULL && !stmt->IsNullStmt())
	{
		UPDATEINFOITEM updateItem;
		UPDATETHREADINFO updatethreadinfo;

		BOOL XWDTFlag = 0;	// 第二次进入XWDT模块标志
		while (stmt->NextRow())
		{
			updateItem.strKindId = stmt->ValueString(0);
			updateItem.strDownloadUrl = stmt->ValueString(1);
			updateItem.strContent = stmt->ValueString(2);
			updateItem.nRequestKind = stmt->ValueInt(3);
			updateItem.nRequestSubKind = stmt->ValueInt(4);
			updateItem.nOrderNo = stmt->ValueInt(5);
			updateItem.nPriority = stmt->ValueInt(7);
            updateItem.tableInfo = stmt->ValueString(8);
			//  存储线程信息 
            // XWDT 开一个线程， 
            // 表中JPLY到DHLGG开一个线程
            // QKYL和TSYD开一个线程
			if (updateItem.strKindId == _T("XWDT")) {
				if (!XWDTFlag) {
					updatethreadinfo.nStartNo = stmt->ValueInt(5);
					XWDTFlag = TRUE;
				}
				else {
					updatethreadinfo.nEndNo = stmt->ValueInt(5);
					updatethreadinfo.nRequestTimeGap = stmt->ValueInt(6);
					// 新闻动态模块的信息
					theApp.m_updateThreadArray.Add(updatethreadinfo);
				}
			}

			if (updateItem.strKindId == _T("JPLY")) {
				updatethreadinfo.nStartNo = stmt->ValueInt(5);
			}
			if (updateItem.strKindId == _T("DHLGX")) {
				updatethreadinfo.nEndNo = stmt->ValueInt(5);
				updatethreadinfo.nRequestTimeGap = stmt->ValueInt(6);
				// 另一个线程的其他所有模块的信息
				theApp.m_updateThreadArray.Add(updatethreadinfo);
			}

            if(updateItem.strKindId == _T("QKYL")) {
				updatethreadinfo.nStartNo = stmt->ValueInt(5);
            }       
			if (updateItem.strKindId == _T("TSYD")) {
				updatethreadinfo.nEndNo = stmt->ValueInt(5);
				updatethreadinfo.nRequestTimeGap = stmt->ValueInt(6);
				// 另一个线程的其他所有模块的信息
				theApp.m_updateThreadArray.Add(updatethreadinfo);
			}

			theApp.m_updateinfoArray.Add(updateItem);
		}
		delete stmt;
		stmt = NULL;
	}
	else
	{
		theApp.UnLockRead();
		ER
		Sleep(10);
		delete stmt;
		stmt = NULL;
		AfxMessageBox(_T("Select Sqlite  Error\n") + strSqlSelect);
		return FALSE;
	}

	// 从updateinfo表中获取期刊和图书的最后更新时间标志
	CString strSqlSelcetQK_TS;
	CString QKTableName, TSTableName;
	int tableInfoSeperator = theApp.m_updateinfoArray[12].tableInfo.Find('-');
	QKTableName = theApp.m_updateinfoArray[12].tableInfo.Left(tableInfoSeperator);
	CString QKDirName = theApp.m_updateinfoArray[12].tableInfo.Mid(tableInfoSeperator + 1);
	strSqlSelcetQK_TS.Format(_T("SELECT UpdateTime FROM %s ORDER BY UpdateTime DESC LIMIT 1;"), QKTableName); 
	stmt = theApp.m_sqlite.Statement(strSqlSelcetQK_TS);

	if (stmt != NULL && !stmt->IsNullStmt()) {
		stmt->Execute();
		theApp.m_updateinfoArray[12].strLastUpdateStander = stmt->ValueString(0);
		
		delete stmt;
		stmt = NULL;
	}
	//// 时间加上一天作为更新开始标志
	//addTime(theApp.m_updateinfoArray[12].strLastUpdateStander, 0, 0, 1);

	TSTableName = theApp.m_updateinfoArray[13].tableInfo;
	strSqlSelcetQK_TS.Format(_T("SELECT UpdateTime FROM %s ORDER BY UpdateTime DESC LIMIT 1;"), TSTableName);
	stmt = theApp.m_sqlite.Statement(strSqlSelcetQK_TS);
	if (stmt != NULL && !stmt->IsNullStmt()) {
		stmt->Execute();
		theApp.m_updateinfoArray[13].strLastUpdateStander = stmt->ValueString(0);

		delete stmt;
		stmt = NULL;
	}

	theApp.UnLockRead();
ER
	Sleep(10);
	return TRUE;
}
BOOL getTemplateFromTable()
{
	COMMONITEM commonItem;
	int tableInfoSeperator = theApp.m_updateinfoArray[12].tableInfo.Find('-');
	CString QK_TSTableName = theApp.m_updateinfoArray[12].tableInfo.Left(tableInfoSeperator);
	CSqlStatement *stmt = NULL;
	CStringArray strSqlSelect;
	CString strSqlSelectZJXZ, strSqlSelectBGJJ, strSqlSelectBGFW, strSqlSelectZLGG, strSqlSelectBGKX, strSqlSelectQKTS;
	strSqlSelectZJXZ.Format(_T("SELECT * FROM %s WHERE ID = 'ZJXZTYPE';"), theApp.m_updateinfoArray[8].tableInfo);	//0
	strSqlSelectBGJJ.Format(_T("SELECT * FROM %s WHERE ID = 'BGJJTYPE';"), theApp.m_updateinfoArray[7].tableInfo);	//1
	strSqlSelectBGFW.Format(_T("SELECT * FROM %s WHERE ID = 'BGFWTYPE';"), theApp.m_updateinfoArray[9].tableInfo);	//2
	strSqlSelectZLGG.Format(_T("SELECT * FROM %s WHERE ID = 'ZLGGTYPE';"), theApp.m_updateinfoArray[2].tableInfo);	//3
	strSqlSelectBGKX.Format(_T("SELECT * FROM %s WHERE ID = 'BGKXTYPE';"), theApp.m_updateinfoArray[3].tableInfo);	//4
	strSqlSelectQKTS.Format(_T("SELECT * FROM %s WHERE ID = 'QKYLTYPE';"), QK_TSTableName);	//5
	strSqlSelect.Add(strSqlSelectZJXZ);
	strSqlSelect.Add(strSqlSelectBGJJ);
	strSqlSelect.Add(strSqlSelectBGFW);
	strSqlSelect.Add(strSqlSelectZLGG);
	strSqlSelect.Add(strSqlSelectBGKX);
	strSqlSelect.Add(strSqlSelectQKTS);

	Sleep(5);
	while (!theApp.LockRead(INFINITE, 10));
R
	for (int i = 0; i < strSqlSelect.GetCount(); i++) {
		stmt = theApp.m_sqlite.Statement(strSqlSelect.GetAt(i));
		while (stmt->NextRow())
		{
			// ZLGG  和 BGKX表结构不相同
			if (i <= 2) {
				commonItem.FmKind = stmt->ValueString(7);
				commonItem.FmBkPath = stmt->ValueString(8);
				commonItem.FmPath = stmt->ValueString(9);
				commonItem.ShowInLevel = stmt->ValueString(10);
				commonItem.OpenItemKind = stmt->ValueString(11);
				commonItem.OpenItemShowKind = stmt->ValueString(12);
				commonItem.DirGetKind = stmt->ValueString(13);
				commonItem.DirInTable = stmt->ValueString(14);
				commonItem.DirShowKind = stmt->ValueString(15);
				commonItem.MoreGetKind = stmt->ValueString(16);
				commonItem.MoreShowKind = stmt->ValueString(17);
				commonItem.DataGetKind = stmt->ValueString(18);
				commonItem.DataKind = stmt->ValueString(19);
				commonItem.DataPath = stmt->ValueString(20);
				commonItem.SubItemExist = stmt->ValueString(21);
				commonItem.SubItemInfoTable = stmt->ValueString(22);
				commonItem.SubItemIsShowInList = stmt->ValueString(23);
				commonItem.nSubItemShowKind = stmt->ValueInt(24);
				commonItem.SubItemListLevel = stmt->ValueString(25);
				commonItem.SubItemListPageChangeKind = stmt->ValueString(31);
				commonItem.ShowEwmKind = stmt->ValueString(32);
				commonItem.OrderNo = stmt->ValueInt(33);
				commonItem.IsCanFind = stmt->ValueString(34);
				commonItem.IsCanLarge = stmt->ValueInt(35);
				commonItem.InfoKind = stmt->ValueString(37);
				commonItem.appendkind = stmt->ValueString(39);
				commonItem.ZanExist = stmt->ValueString(40);
				commonItem.BeTemplate = 0;
				commonItem.NextWinowShowInRect = stmt->ValueString(52);
				commonItem.BeShowInHomePage = stmt->ValueInt(53);
			} else if(i <= 4) {
				commonItem.FmKind = stmt->ValueString(8);
				commonItem.FmBkPath = stmt->ValueString(9);
				commonItem.FmPath = stmt->ValueString(10);
				commonItem.ShowInLevel = stmt->ValueString(11);
				commonItem.OpenItemKind = stmt->ValueString(12);
				commonItem.OpenItemShowKind = stmt->ValueString(13);
				commonItem.DirGetKind = stmt->ValueString(14);
				commonItem.DirInTable = stmt->ValueString(15);
				commonItem.DirShowKind = stmt->ValueString(16);
				commonItem.MoreGetKind = stmt->ValueString(17);
				commonItem.MoreShowKind = stmt->ValueString(18);
				commonItem.DataGetKind = stmt->ValueString(19);
				commonItem.DataKind = stmt->ValueString(20);
				commonItem.DataPath = stmt->ValueString(21);
				commonItem.SubItemExist = stmt->ValueString(22);
				commonItem.SubItemInfoTable = stmt->ValueString(23);
				commonItem.SubItemIsShowInList = stmt->ValueString(24);
				commonItem.nSubItemShowKind = stmt->ValueInt(25);
				commonItem.SubItemListLevel = stmt->ValueString(26);
				commonItem.SubItemListPageChangeKind = stmt->ValueString(32);
				commonItem.ShowEwmKind = stmt->ValueString(33);
				commonItem.OrderNo = stmt->ValueInt(34);
				commonItem.IsCanLarge = stmt->ValueInt(35);
				commonItem.IsCanFind = stmt->ValueString(36);
				commonItem.InfoKind = stmt->ValueString(38);
				commonItem.appendkind = stmt->ValueString(40);
				commonItem.ZanExist = stmt->ValueString(41);
				commonItem.BeTemplate = 0;
				commonItem.BeShowInHomePage = stmt->ValueInt(50);
				commonItem.NextWinowShowInRect = stmt->ValueString(54);
			} else {
				commonItem.GGParentID = stmt->ValueString(7);
				commonItem.GGGParentID = stmt->ValueString(8);
				commonItem.FmKind = stmt->ValueString(9);
				commonItem.FmPath = stmt->ValueString(11);
				commonItem.ShowInLevel = stmt->ValueString(12);
				commonItem.OpenItemKind = stmt->ValueString(13);
				commonItem.OpenItemShowKind = stmt->ValueString(14);
				commonItem.DirGetKind = stmt->ValueString(15);
				commonItem.DirInTable = stmt->ValueString(16);
				commonItem.DirShowKind = stmt->ValueString(17);
				commonItem.MoreGetKind = stmt->ValueString(18);
				commonItem.MoreShowKind = stmt->ValueString(19);
				commonItem.DataGetKind = stmt->ValueString(20);
				commonItem.DataKind = stmt->ValueString(21);
				commonItem.DataPath = stmt->ValueString(22);
				commonItem.SubItemExist = stmt->ValueString(23);
				commonItem.SubItemIsShowInList = stmt->ValueString(25);
				commonItem.nSubItemShowKind = stmt->ValueInt(26);
				commonItem.SubItemListLevel = stmt->ValueString(27);
				commonItem.SubItemListPageChangeKind = stmt->ValueString(33);
				commonItem.ShowEwmKind = stmt->ValueString(34);
				commonItem.OrderNo = stmt->ValueInt(35);
				commonItem.IsCanLarge = stmt->ValueInt(36);
				commonItem.IsCanFind = stmt->ValueString(37);
				commonItem.InfoKind = stmt->ValueString(39);
				commonItem.appendkind = stmt->ValueString(41);
				commonItem.ZanExist = stmt->ValueString(42);
				//commonItem.BeTemplate = stmt->ValueString(45);
				commonItem.BeValid = stmt->ValueInt(46);
				commonItem.append1kind = stmt->ValueString(47);
				commonItem.BeShowInHomePage = stmt->ValueInt(51);
			}
			theApp.totalItem.Add(commonItem);
		}
		delete stmt;
		stmt = NULL;
	}
	theApp.UnLockRead();
ER
	Sleep(10);
return TRUE;
}

BOOL CMonitorApp::InitInstance()
{
// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	CTime m_time;
	CString m_strDateTime;

	//theApp.LockWrite(-1, -1);
	//AfxMessageBox(_T("1"));
	////while (1)
	////{

	////}
	//theApp.UnLockWrite();



//	while (!theApp.LockRead(INFINITE, 10));
//while(1)
//{
//
//}
	//theApp.UnLockRead();

 //   while(!theApp.LockWrite(-1, -1));

	//AfxMessageBox(_T("1"));


	if (FindPrevInstance())
	{
		AfxMessageBox(_T("程序已启动！"));
		return FALSE;
	}

	CWinAppEx::InitInstance();

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;

	while (!InitCommonControlsEx(&InitCtrls))
	{
		Sleep(2000);
		AfxMessageBox(_T("重试"));
	}


	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	m_iUpdateFinishPercent = 0;

	m_bQuestClose = FALSE;

	m_strGUIDOLDUnInstal = _T("{606C42C5-7D1A-40E6-BBA2-36A992DE1942}");

	m_strProgramVertion = _T("11101");//版本号临时弄一个编号：前两位：11 公图 、12法律 、13党政、14旅游、15军事、16交通、17农业   第三位：1横版 、2竖版   第四五位：01 表示标准版……（其他各部门自己定义）

	CString str = GetFileMd5(_T("G:\\d123.txt"));

	CString strSave = _T("D:\\TEST\\SAVEPIC\\SAVE.jpg");

	TCHAR *pAppName = m_strExePath.GetBuffer(MAX_PATH);
	GetModuleFileName(NULL, pAppName, MAX_PATH);
	// 获得上一层目录
	PathRemoveFileSpec(pAppName);
	//PathRemoveFileSpec(pAppName);
	m_strExePath.ReleaseBuffer();

	TCHAR szDictPath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szDictPath);
	PathAppend(szDictPath, _T("\\TTKN"));
	m_strCanChangePath = szDictPath;

	if (!PathFileExists(m_strCanChangePath))
	{
		CreateDirectory(m_strCanChangePath, NULL);
	}
	m_strCanChangePath = m_strCanChangePath + _T("\\CAJTouchViewer");

	if (!PathFileExists(m_strCanChangePath))
	{
		CreateDirectory(m_strCanChangePath, NULL);
	}


	if (!GetInitParamFromXML(m_strCanChangePath + _T("\\xml\\profile.xml")))
	{
		AfxMessageBox(_T("从配置文件读取数据失败！") + m_strCanChangePath + _T("\\xml\\profile.xml"));
		return FALSE;
	}

	ReadTotalSavePath();

	GetMacArray();

	if (!PathFileExists(m_strSqliteDataPath))
	{
		AfxMessageBox(_T("SQLITE数据库不存在！") + m_strSqliteDataPath);
		return FALSE;
	}

	BOOL bResult = m_sqlite.Open(m_strSqliteDataPath);
	if (!bResult)
	{
		AfxMessageBox(_T("打不开数据库！"));
		return FALSE;
	}
	m_sqlite.SetBusyTimeOut(60 * 1000);

	CString strGetRegistNoUrl;

    getInfoFromUpdateInfo();
	getTemplateFromTable();
	ReadSerialNo();

	if (IsRegest() == FALSE)
	{
		while (!GetSerialNoFromServer(strGetRegistNoUrl))
		{
			CRegistInfoDlg   dlgReg;
			dlgReg.SetRegistNo(m_strRegestNo);
			/*m_pMainWnd = &dlgReg;*/
			dlgReg.DoModal();
		}
		CString strIniPath = _T("\\xml\\serialno.ini");

		strIniPath = m_strCanChangePath + strIniPath;

		::WritePrivateProfileStringW(_T("SerialNo"), _T("No"), m_strSerialNo, strIniPath);
		::WritePrivateProfileStringW(_T("RegistNo"), _T("No"), m_strRegestNo, strIniPath);
	}

	GetBuyYear();

	CTime t = CTime::GetCurrentTime(); //获取系统日期
	UINT nlNowYear = t.GetYear();
	if (_ttoi(m_strBuyEndYear)<nlNowYear)
	{
		CString strTempSerailNo;
		if (GetSerialNoFromServer(strGetRegistNoUrl, strTempSerailNo) && strTempSerailNo != m_strSerialNo)
		{
			m_strSerialNo = strTempSerailNo;
			CString strIniPath = _T("\\xml\\serialno.ini");

			strIniPath = m_strCanChangePath + strIniPath;

			::WritePrivateProfileStringW(_T("SerialNo"), _T("No"), m_strSerialNo, strIniPath);
			GetBuyYear();
		}
	}
	//CString strKbaseIdTemp = m_strKbaseId, strKbasePortTemp = m_strKbasePort, strKbaseUserNameTemp = m_strKbaseUserName, strKbasePassWordTemp = m_strKbasePassWord;

	//while (m_strKbaseId.IsEmpty() || m_strKbasePort.IsEmpty() || m_strKbaseUserName.IsEmpty() || m_strKbasePassWord.IsEmpty())
	//{

	//	CSetKbaseInfoDlg setkDlg;
	//	setkDlg.m_strKbaseId = strKbaseIdTemp;
	//	setkDlg.m_strKbaseUserName = strKbaseUserNameTemp;
	//	setkDlg.m_strKbasePort = strKbasePortTemp;
	//	setkDlg.m_strKbasePassWord = strKbasePassWordTemp;
	//	int iEndKind=setkDlg.DoModal();
	//	if (iEndKind==2)
	//	{
	//		return FALSE;
	//	}
	//	else if (iEndKind==1)
	//	{
	//		strKbaseIdTemp = setkDlg.m_strKbaseId;
	//		strKbaseUserNameTemp = setkDlg.m_strKbaseUserName;
	//		strKbasePortTemp = setkDlg.m_strKbasePort;
	//		strKbasePassWordTemp = setkDlg.m_strKbasePassWord;

	//		BSTR bstrID = strKbaseIdTemp.AllocSysString();
	//		BSTR bstrUserName = strKbaseUserNameTemp.AllocSysString();
	//		BSTR bstrUserPassword = strKbasePassWordTemp.AllocSysString();
	//		UINT nPort = _ttoi(strKbasePortTemp);

	//		if (strKbaseIdTemp.IsEmpty() || strKbasePortTemp.IsEmpty() || strKbaseUserNameTemp.IsEmpty() || strKbasePassWordTemp.IsEmpty() || !OpenEx(m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
	//		{
	//			SysFreeString(bstrID);
	//			SysFreeString(bstrUserName);
	//			SysFreeString(bstrUserPassword);
	//			AfxMessageBox(_T("请确认自己输入的信息正确！"));
	//			//return FALSE;
	//		}
	//		else
	//		{
	//			SysFreeString(bstrID);
	//			SysFreeString(bstrUserName);
	//			SysFreeString(bstrUserPassword);
	//			m_strKbaseId = strKbaseIdTemp; 
	//			m_strKbasePort=strKbasePortTemp;
	//			m_strKbaseUserName = strKbaseUserNameTemp;
	//			m_strKbasePassWord=strKbasePassWordTemp;
	//			UpdateXmlAboutKbase();
	//			break;
	//		}

	//	}
	//}

	//while (DetectProgectExist(_T("KBaseServer.exe")) == 0 )
	//{
	//	Sleep(1000);
	//}

	//Sleep();


	CMonitorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


BOOL CMonitorApp::GetInitParamFromXML(CString strXmlPath)
{

	if (!PathFileExists(strXmlPath))
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

	hr = spDoc->load(strXmlPath.AllocSysString());//load xml文件  

	MSXML2::IXMLDOMNodeListPtr spNodeList;
	MSXML2::IXMLDOMNodePtr pItem;

	spNodeList = spDoc->getElementsByTagName(_bstr_t("a"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strKbaseId = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();


	spNodeList = spDoc->getElementsByTagName(_bstr_t("b"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strKbasePort = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("c"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strKbaseUserName = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("d"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strKbasePassWord = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	if (!m_strKbasePassWord.IsEmpty())
	{
		m_strKbasePassWord = m_DecTemp.base64decode(m_strKbasePassWord);

	}
	pItem.Release();
	spNodeList.Release();

	//spNodeList = spDoc->getElementsByTagName(_bstr_t("d"));   //获取根结点  
	//spNodeList->get_item(0, &pItem);

	//pItem->get_nodeTypedValue(&varNodeValue);
	//m_strInterFacePicPath = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	//pItem.Release();


	//spNodeList = spDoc->getElementsByTagName(_bstr_t("e"));   //获取根结点  
	//spNodeList->get_item(0, &pItem);
	//pItem->get_nodeTypedValue(&varNodeValue);
	//m_nAutoFlipTime = _ttoi((CString)(LPCTSTR)(_bstr_t)varNodeValue);
	//pItem.Release();

	MSXML2::IXMLDOMNamedNodeMapPtr pAttrs = NULL;
	spNodeList = spDoc->getElementsByTagName(_bstr_t("f"));   //获取根结点  
	spNodeList->get_item(0, &pItem);




	pItem->get_nodeTypedValue(&varNodeValue);
	m_strCloseTime = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem->get_attributes(&pAttrs);
	pItem.Release();

	long nCount;
	CString strAttrName;
	MSXML2::IXMLDOMNodePtr pAttrItem;
	BOOL bAllowClose = TRUE;
	pAttrs->get_length(&nCount); //获取节点属性个数
	for (int i = 0; i<nCount; i++)
	{
		pAttrs->get_item(i, &pAttrItem);

		strAttrName = (char*)(_bstr_t)pAttrItem->nodeName;
		if (strAttrName == _T("ballowclose"))
		{
			CString strAttrValue = (LPCTSTR)(_bstr_t)pAttrItem->nodeTypedValue;

			if (strAttrValue == _T("0"))
			{
				bAllowClose = FALSE;
			}
			pAttrItem.Release();
			break;
		}
		pAttrItem.Release();
	}


	if (bAllowClose == FALSE)
	{
		m_strCloseTime.Empty();
	}

	m_bAllowClose = bAllowClose;
	pAttrs.Release();
	spNodeList.Release();

	// 	spNodeList=spDoc->getElementsByTagName(_bstr_t("g"));   //获取根结点  
	// 	spNodeList->get_item(0,&pItem);
	// 	pItem->get_nodeTypedValue(&varNodeValue);
	// 	m_strDataPath=(CString)(LPCTSTR)(_bstr_t)varNodeValue;
	// 	pItem.Release();

	//spNodeList = spDoc->getElementsByTagName(_bstr_t("h"));   //获取根结点  
	//spNodeList->get_item(0, &pItem);
	//pItem->get_nodeTypedValue(&varNodeValue);
	//m_strLoadDataAdrr = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	//pItem.Release();

	//spNodeList = spDoc->getElementsByTagName(_bstr_t("i"));   //获取根结点  
	//spNodeList->get_item(0, &pItem);
	//pItem->get_nodeTypedValue(&varNodeValue);
	//m_strXmlTotalPath = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	//pItem.Release();

	//spNodeList=spDoc->getElementsByTagName(_bstr_t("j"));   //获取根结点  
	//spNodeList->get_item(0,&pItem);
	//pItem->get_nodeTypedValue(&varNodeValue);
	//m_strSerialNo=(CString)(LPCTSTR)(_bstr_t)varNodeValue;
	//pItem.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("k"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_nPaperSaveDayLength = _ttoi((CString)(LPCTSTR)(_bstr_t)varNodeValue);
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("l"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strStartYearMonth = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("m"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_nGetInfoVersion = _ttoi((CString)(LPCTSTR)(_bstr_t)varNodeValue);
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("n"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strTitleName = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();


	spNodeList = spDoc->getElementsByTagName(_bstr_t("o"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strUserWebAdrr = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("p"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strCnkiServerId = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("aa"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strCnkiServerPath = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("s"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strSqliteDataPath = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("t"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->get_nodeTypedValue(&varNodeValue);
	m_strKindEname = (CString)(LPCTSTR)(_bstr_t)varNodeValue;
	pItem.Release();
	spNodeList.Release();

	spDoc.Release();

	::CoUninitialize();

	g_mutex.Unlock();

	return TRUE;
}






int GetMac(char*lpszMac)
{
	PIP_ADAPTER_INFO pAdapterInfo; ULONG ulOutbufLen = sizeof(IP_ADAPTER_INFO);//注意是ULONG类型,而非PULONG类型
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));//为其动态分配内存,若大小不够,后面会有调整

	if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(pAdapterInfo, &ulOutbufLen))
	{//MSDN上介绍,因为获取的消息,可能超过预先设置的缓冲区长度,造成返回,而一旦返回获取的资料将为空,此处为防止这种情况发生,
		free(pAdapterInfo);//可以测试一下结果,去掉这部分代码后,返回值为空.此处重构,直到成功
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutbufLen); //第1次大小不够,第2次返回所需长度
	}
	if (ERROR_SUCCESS == GetAdaptersInfo(pAdapterInfo, &ulOutbufLen))//MSDN上是那样写,也可写成NO_ERROR,都正确
	{
		sprintf_s(lpszMac, 128, "%02x-%02x-%02x-%02x-%02x-%02x\n",
			pAdapterInfo->Address[0], //注意此处是数组格式,网上有的没有[0][1]等,str.Format格式就是sprintf的MFC版
			pAdapterInfo->Address[1],
			pAdapterInfo->Address[2],
			pAdapterInfo->Address[3],
			pAdapterInfo->Address[4],
			pAdapterInfo->Address[5]);//,pAdapterInfo->Address[6],如果你想知道第7个是个什么值?可以一试,但是值为00(即空值)

		return 1;
	}



	return 0;
}


BOOL CMonitorApp::GetMacArray()
{
	char lpszMac[128];
	memset(lpszMac, 0x00, sizeof(lpszMac));
	PIP_ADAPTER_INFO pAdapterInfo; ULONG ulOutbufLen = sizeof(IP_ADAPTER_INFO);//注意是ULONG类型,而非PULONG类型
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));//为其动态分配内存,若大小不够,后面会有调整

	m_strMacArray.RemoveAll();
	if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(pAdapterInfo, &ulOutbufLen))
	{//MSDN上介绍,因为获取的消息,可能超过预先设置的缓冲区长度,造成返回,而一旦返回获取的资料将为空,此处为防止这种情况发生,
		free(pAdapterInfo);//可以测试一下结果,去掉这部分代码后,返回值为空.此处重构,直到成功
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutbufLen); //第1次大小不够,第2次返回所需长度
	}
	if (ERROR_SUCCESS == GetAdaptersInfo(pAdapterInfo, &ulOutbufLen))//MSDN上是那样写,也可写成NO_ERROR,都正确
	{
		while (pAdapterInfo)
		{
			sprintf_s(lpszMac, 128, "%02x-%02x-%02x-%02x-%02x-%02x\n",
				pAdapterInfo->Address[0], //注意此处是数组格式,网上有的没有[0][1]等,str.Format格式就是sprintf的MFC版
				pAdapterInfo->Address[1],
				pAdapterInfo->Address[2],
				pAdapterInfo->Address[3],
				pAdapterInfo->Address[4],
				pAdapterInfo->Address[5]);//,pAdapterInfo->Address[6],如果你想知道第7个是个什么值?可以一试,但是值为00(即空值)

			CString strTemp = CString(pAdapterInfo->Description);
			if (strTemp.Find(_T("PCI")) >= 0)
			{
				m_strMacArray.InsertAt(0, CString(lpszMac));
			}
			else
			{
				m_strMacArray.Add(CString(lpszMac));
			}

			pAdapterInfo = pAdapterInfo->Next;
		}

		return TRUE;
		//sprintf_s(lpszMac,128,"%02x-%02x-%02x-%02x-%02x-%02x\n",
		//	pAdapterInfo->Address[0], //注意此处是数组格式,网上有的没有[0][1]等,str.Format格式就是sprintf的MFC版
		//	pAdapterInfo->Address[1],
		//	pAdapterInfo->Address[2],
		//	pAdapterInfo->Address[3],
		//	pAdapterInfo->Address[4],
		//	pAdapterInfo->Address[5]);//,pAdapterInfo->Address[6],如果你想知道第7个是个什么值?可以一试,但是值为00(即空值)

		//strMac=CString(lpszMac);

		//return TRUE;
	}

	return FALSE;
}


void getIp(CString& strIp)
{
	char szHostName[128];

	//获得计算机名称，保存在szHostName变量  

	if (gethostname(szHostName, 128) == 0)
	{
		struct hostent *pHost;

		//	int i;  

		//IP为不唯一的情况，形成IP列表  

		pHost = gethostbyname(szHostName);

		//for(i=0;pHost!=NULL&&pHost->h_addr_list[i]!=NULL;i++)  
		{
			LPSTR psz = inet_ntoa(*(struct in_addr *)pHost->h_addr_list[0]);
			CString strTemp(psz);
			strIp = strTemp;
			//	break;

		}

	}
}

// 解密AES算法
const char g_key[17] = "cnkiby201906";
const char g_iv[17] = "gfdertfghjkuyrtg";

string EncryptionAES(const string& strSrc) //AES加密
{
	size_t length = strSrc.length();
	int block_num = length / BLOCK_SIZE + 1;
	//明文
	char* szDataIn = new char[block_num * BLOCK_SIZE + 1];
	memset(szDataIn, 0x00, block_num * BLOCK_SIZE + 1);
	strcpy(szDataIn, strSrc.c_str());

	//进行PKCS7Padding填充。
	int k = length % BLOCK_SIZE;
	int j = length / BLOCK_SIZE;
	int padding = BLOCK_SIZE - k;
	for (int i = 0; i < padding; i++)
	{
		szDataIn[j * BLOCK_SIZE + k + i] = padding;
	}
	szDataIn[block_num * BLOCK_SIZE] = '\0';

	//加密后的密文
	char *szDataOut = new char[block_num * BLOCK_SIZE + 1];
	memset(szDataOut, 0, block_num * BLOCK_SIZE + 1);

	//进行进行AES的CBC模式加密
	AES aes;
	aes.MakeKey(g_key, g_iv, 16, 16);
	aes.Encrypt(szDataIn, szDataOut, block_num * BLOCK_SIZE, AES::ECB);
	string str = base64_encode((unsigned char*)szDataOut,
		block_num * BLOCK_SIZE);
	delete[] szDataIn;
	delete[] szDataOut;
	return str;
}
string DecryptionAES(const string& strSrc) //AES解密
{
	string strData = base64_decode(strSrc);
	size_t length = strData.length();
	//密文
	char *szDataIn = new char[length + 1];
	memcpy(szDataIn, strData.c_str(), length + 1);
	//明文
	char *szDataOut = new char[length + 1];
	memcpy(szDataOut, strData.c_str(), length + 1);

	//进行AES的CBC模式解密
	AES aes;
	aes.MakeKey(g_key, g_iv, 16, 16);
	aes.Decrypt(szDataIn, szDataOut, length, AES::ECB);

	//去PKCS7Padding填充
	if (0x00 < szDataOut[length - 1] <= 0x16)
	{
		int tmp = szDataOut[length - 1];
		for (int i = length - 1; i >= length - tmp; i--)
		{
			if (szDataOut[i] != tmp)
			{
				memset(szDataOut, 0, length);
				cout << "去填充失败！解密出错！！" << endl;
				break;
			}
			else
				szDataOut[i] = 0;
		}
	}
	string strDest(szDataOut);
	delete[] szDataIn;
	delete[] szDataOut;
	return strDest;
}

BOOL CMonitorApp::GetSerialNoFromServer(CString strBaseUrl)
{
	for (int i = 0; i < m_strMacArray.GetSize(); i++)
	{
		CString strRegistNoTemp;
		m_DecTemp.GetRegistNo(m_strMacArray.GetAt(i), strRegistNoTemp);

		UPDATEINFOITEM updateIfItem = m_updateinfoArray.GetAt(0);
		CString strResult;
		CString strRequestUrl = theApp.m_strCnkiServerId + updateIfItem.strDownloadUrl;
		char content[100];
		CString contentTemp;
		contentTemp.Format(_T("{\"serialNo\":\"%s\"}"), strRegistNoTemp);
		CString2Char(contentTemp, content);

		SendStrFromWeb(strRequestUrl, strResult, content, "");

		Json::Reader reader;
		Json::Value itemArr;
		char buf[10000] = { 0 };
		CString2Char(strResult, buf);
		// get "token":"....."
		if (reader.parse(buf, itemArr, FALSE)) {
			if (string2CString(itemArr["success"].asString()).CompareNoCase(_T("true")) == 0) {
				token = "token:" + itemArr["data"]["token"].asString();
				m_strRegestNo = strRegistNoTemp;

				// get serialNo 					
				updateIfItem = m_updateinfoArray.GetAt(1);
				strRequestUrl = theApp.m_strCnkiServerId + updateIfItem.strDownloadUrl;
				SendStrFromWeb(strRequestUrl, strResult, "", token.c_str());
				CString2Char(strResult, buf);
				reader.parse(buf, itemArr, FALSE);
				m_strSerialNo = string2CString(itemArr["data"]["code"].asString());
				char tmpSerialNo[200];
				CString2Char(m_strSerialNo, tmpSerialNo);
				m_strSerialNo = string2CString(DecryptionAES(tmpSerialNo));

				CString strGetRegistNo;
				m_DecTemp.DecryptionRegist(m_strSerialNo, strGetRegistNo);

				// 如果根据序列号解出来的注册码和mac地址生成的注册码一致，则成功
				if (strGetRegistNo.CompareNoCase(strRegistNoTemp) == 0)
					return TRUE;
			}
		}
	}
	return FALSE;
}

//BOOL CMonitorApp::GetSerialNoFromServer(CString strBaseUrl)
//{
//	if (m_strRegestNo.IsEmpty())
//	{
//		for (int i = 0; i<m_strMacArray.GetSize(); i++)
//		{
//			CString strRegistNoTemp;
//			m_DecTemp.GetRegistNo(m_strMacArray.GetAt(i), strRegistNoTemp);
//
//			CString strUrl;
//			strUrl.Format(_T("%s?ComputerNo =%s&RequestKind =3&Kind =Serialno"),
//				strBaseUrl, m_strRegestNo);
//
//			CString strTemp;
//			GetStrFromWeb(strUrl, strTemp);
//
//			if (!strTemp.IsEmpty() && strTemp.Left(4) != _T("Err:"))
//			{
//				m_strSerialNo = strTemp;
//				m_strRegestNo = strRegistNoTemp;
//				return TRUE;
//			}
//
//		}
//		if (m_strRegestNo.IsEmpty() && m_strMacArray.GetSize()>0)
//		{
//			m_DecTemp.GetRegistNo(m_strMacArray.GetAt(0), m_strRegestNo);
//
//		}
//	}
//	else
//	{
//		CString strUrl;
//		strUrl.Format(_T("%s?ComputerNo =%s&RequestKind =3&Kind =Serialno"),
//			strBaseUrl, m_strRegestNo);
//		CString strTemp;
//		GetStrFromWeb(strUrl, strTemp);
//
//		if (!strTemp.IsEmpty() && strTemp.Left(4) != _T("Err:"))
//		{
//			m_strSerialNo = strTemp;
//			return TRUE;
//		}
//
//	}
//
//
//	return FALSE;
//}


BOOL CMonitorApp::GetSerialNoFromServer(CString strBaseUrl, CString& strGetSerialNo)
{
	if (!m_strRegestNo.IsEmpty())
	{
		UPDATEINFOITEM updateIfItem;
		CString strRequestUrlHead = theApp.m_strCnkiServerId;
		CString strResult;
		char buf[10000] = { 0 };
		updateIfItem = m_updateinfoArray.GetAt(1);
		CString strRequestUrl = strRequestUrlHead + updateIfItem.strDownloadUrl;
		if (token.empty()) {
			getNewToken(buf);
		}
		SendStrFromWeb(strRequestUrl, strResult, "", token.c_str());

		Json::Reader reader;
		Json::Value itemArr;
		CString2Char(strResult, buf);
		if (reader.parse(buf, itemArr, FALSE)) {
			m_strSerialNo = string2CString(itemArr["data"]["code"].asString());
			//string decryptedSerialNo;
			// Decrypt serialNo
			//decryptedSerialNo = DecryptionAES(sourceSerialNo);
			//m_strSerialNo = string2CString(decryptedSerialNo);
			//m_strSerialNo = string2CString(sourceSerialNo);
		}
		strGetSerialNo = m_strSerialNo;
		return TRUE;

	}


	return FALSE;
}

BOOL CMonitorApp::IsRegest()
{
	if (m_nGetInfoVersion == 0)
	{
		//UINT nAllowCount = 0;
		//if (m_strSerialNo.IsEmpty())
		//{
		//	return FALSE;
		//}
		//else
		//{
		//	CString strTemp;
		//	m_DecTemp.DecryptionMaxLink(m_strSerialNo, m_strRegestNo, strTemp);
		//	nAllowCount = _ttoi(strTemp);
		//}
		//CString strIp;
		//getIp(strIp);
		//int nErrCode = 0;
		//CString strSql;
		//strSql.Format(_T("select SQL_NO_CACHE * from CAJTOUCHIP where ID=%s"), strIp);
		//if (!OpenRecordSet(strSql.AllocSysString()))
		//{
		//	return FALSE;
		//}

		//int nRecordCount = GetRecordCount();
		//CloseRecordSet();
		//if (nRecordCount <= 0)
		//{
		//	strSql = _T("select SQL_NO_CACHE * from CAJTOUCHIP ");
		//	if (!OpenRecordSet(strSql.AllocSysString()))
		//	{
		//		return FALSE;
		//	}

		//	int lCount = GetRecordCount();
		//	CloseRecordSet();
		//	if (lCount < 0 || lCount >= (int)nAllowCount)
		//	{

		//		return FALSE;
		//	}
		//	else
		//	{
		//		strSql.Format(_T("INSERT INTO CAJTOUCHIP (ID) VALUES(%s)"), strIp);
		//		InsertValue(strSql);
		//	}
		//}
		if (m_strSerialNo.IsEmpty())
		{
			return FALSE;
		}

		CString strGetRegistNo;
		m_DecTemp.DecryptionRegist(m_strSerialNo, strGetRegistNo);

		for (int i = 0; i < m_strMacArray.GetSize(); i++)
		{
			CString strRegistNoTemp;
			m_DecTemp.GetRegistNo(m_strMacArray.GetAt(i), strRegistNoTemp);
			if (strRegistNoTemp == strGetRegistNo)
			{
				m_strRegestNo = strRegistNoTemp;

				return TRUE;
			}
		}
		return FALSE;
	}
	else
	{
		if (m_strSerialNo.IsEmpty())
		{
			return FALSE;
		}

		CString strGetRegistNo;

		m_DecTemp.DecryptionRegist(m_strSerialNo, strGetRegistNo);
		for (int i = 0; i < m_strMacArray.GetSize(); i++)
		{
			CString strRegistNoTemp;
			m_DecTemp.GetRegistNo(m_strMacArray.GetAt(i), strRegistNoTemp);
			if (strRegistNoTemp == strGetRegistNo)
			{
				m_strRegestNo = strRegistNoTemp;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CMonitorApp::ReadxmlUpdateDate()
{

	CString strIniFileName = _T("\\xml\\UpdateDate_Info.xml");

	//strIniFileName = m_strCanChangePath + strIniFileName;

	//if (!PathFileExists(strIniFileName))
	//{
	//	AfxMessageBox(_T("更新标志文件不存在！"));
	//	return FALSE;
	//}

	////从xml中读取配置信息
	//::CoInitialize(NULL);
	//BSTR strNodeValue;
	//MSXML2::IXMLDOMDocumentPtr spDoc;
	//HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));    //创建文档对象 

	//BSTR bstrIniFileName = strIniFileName.AllocSysString();
	//SysFreeString(bstrIniFileName);
	//hr = spDoc->load(bstrIniFileName);//load xml文件  

	//MSXML2::IXMLDOMNodeListPtr spNodeList;
	//MSXML2::IXMLDOMNodePtr pItem;

	//spNodeList = spDoc->getElementsByTagName(_bstr_t("ZZHC"));   //获取根结点  
	//long lListNo = 0;
	//CString strGetTemp;
	//spNodeList->get_length(&lListNo);
	//if (lListNo>0)
	//{
	//	long lSubListNo = 0;
	//	MSXML2::IXMLDOMNodeListPtr spChildNodeList;
	//	MSXML2::IXMLDOMNodePtr pSubItem;
	//	spNodeList->get_item(0, &pItem);
	//	pItem->get_childNodes(&spChildNodeList);
	//	spChildNodeList->get_length(&lSubListNo);
	//	if (lSubListNo>0)
	//	{
	//		spChildNodeList->get_item(0, &pSubItem);
	//		pSubItem->get_text(&strNodeValue);
	//		strGetTemp = (CString)strNodeValue;

	//		if (strGetTemp.Trim().IsEmpty())
	//		{
	//			m_nlZzhcDate = 0;
	//		}
	//		else
	//		{
	//			m_nlZzhcDate = _ttol(strGetTemp);
	//		}
	//		pSubItem.Release();
	//	}
	//	if (lSubListNo>1)
	//	{
	//		spChildNodeList->get_item(1, &pSubItem);
	//		pSubItem->get_text(&strNodeValue);
	//		strGetTemp = (CString)strNodeValue;

	//		if (strGetTemp.Trim().IsEmpty())
	//		{
	//			m_nLLoadPart = 0;
	//		}
	//		else
	//		{
	//			if (strGetTemp.GetLength()>9)
	//			{
	//				LONGLONG llTemp = (LONGLONG)powl(10, 9);
	//				m_nLLoadPart = _ttol(strGetTemp.Left(strGetTemp.GetLength() - 9))*llTemp;
	//				m_nLLoadPart += _ttol(strGetTemp.Right(9));
	//			}
	//			else
	//			{
	//				m_nLLoadPart = (unsigned long)_ttol(strGetTemp);
	//			}
	//		}
	//		pSubItem.Release();
	//	}
	//	spChildNodeList.Release();

	//	pItem.Release();
	//}


	//spNodeList.Release();

	//spDoc.Release();

	//::CoUninitialize();
	return TRUE;
	// 	::CoInitialize(NULL);
	// 	CComPtr<MSXML::IXMLDOMDocument> spDoc;  
	// 	HRESULT hr = spDoc.CoCreateInstance(__uuidof(MSXML::DOMDocument));    //创建文档对象 
	// 	VARIANT_BOOL bFlag; 
	// 	CString strPath=_T("\\xml\\UpdateDate_Info.xml");
	// 
	// 	strPath =m_strCanChangePath+strPath;
	// 	if (!PathFileExists(strPath))
	// 	{
	// 		AfxMessageBox(_T("配置文件不存在。"));
	// 		return FALSE;
	// 
	// 	}
	// 
	// 	CComVariant varNodeValue; 
	// 	hr = spDoc->load(CComVariant(strPath), &bFlag);               //load xml文件  
	// 	CComPtr<MSXML::IXMLDOMNodeList> spNodeList; 
	// 	//CComPtr<MSXML::IXMLDOMElement> spElement;
	// 	MSXML::IXMLDOMNodePtr pAttrItem;
	// 	CComPtr<MSXML::IXMLDOMNode> spNode;
	// 	CString strGetTemp;
	// 
	// 
	// 	hr = spDoc->getElementsByTagName(_bstr_t("ZZHC"),&spNodeList);   //获取根结点   
	// 	spNodeList->get_item(0,&pAttrItem);
	// 
	// 	pAttrItem->get_nodeTypedValue(&varNodeValue);
	// 	strGetTemp=(CString)(LPCTSTR)(_bstr_t)varNodeValue;
	// 	if (strGetTemp.Trim().IsEmpty())
	// 	{
	// 		m_nlZzhcDate=0;
	// 	}
	// 	else
	// 	{
	// 	   m_nlZzhcDate=_ttol(strGetTemp);
	// 	}
	// 
	// 	//spElement.Release();
	// 
	// 	pAttrItem.Release();
	// 	spNodeList->get_item(1,&pAttrItem);
	// 	pAttrItem->get_nodeTypedValue(&varNodeValue);
	// 	strGetTemp=(CString)(LPCTSTR)(_bstr_t)varNodeValue;
	// 	if (strGetTemp.Trim().IsEmpty())
	// 	{
	// 		m_nLLoadPart=0;
	// 	}
	// 	else
	// 	{
	// 		m_nLLoadPart=_ttol(strGetTemp);
	// 	}
	// 	pAttrItem.Release();
	// 	spNodeList.Release(); 
	// 	spDoc.Release();  
	// 	::CoUninitialize();  
	// 	return TRUE;


}

BOOL CMonitorApp::UpdateXmlAboutCloseData(BOOL bAllowClose, CString strSetCloseTime)
{
	CString strIniFileName = _T("\\xml\\profile.xml");

	strIniFileName = m_strCanChangePath + strIniFileName;

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

	hr = spDoc->load(strIniFileName.AllocSysString());//load xml文件  

	MSXML2::IXMLDOMNodeListPtr spNodeList;
	MSXML2::IXMLDOMNodePtr pItem;

	MSXML2::IXMLDOMNamedNodeMapPtr pAttrs = NULL;

	spNodeList = spDoc->getElementsByTagName(_bstr_t("f"));   //获取根结点  
	spNodeList->get_item(0, &pItem);

	pItem->get_nodeTypedValue(&varNodeValue);
	if (m_strCloseTime != strSetCloseTime)
	{
		BSTR BStrSetCloseTime = m_strCloseTime.AllocSysString();
		pItem->put_text(BStrSetCloseTime);
		SysFreeString(BStrSetCloseTime);
		m_strCloseTime = strSetCloseTime;
	}

	pItem->get_attributes(&pAttrs);
	pItem.Release();

	long nCount;
	CString strAttrName;
	MSXML2::IXMLDOMNodePtr pAttrItem;

	pAttrs->get_length(&nCount); //获取节点属性个数
	for (int i = 0; i<nCount; i++)
	{
		pAttrs->get_item(i, &pAttrItem);

		strAttrName = (char*)(_bstr_t)pAttrItem->nodeName;
		if (strAttrName == _T("ballowclose"))
		{

			if (m_bAllowClose != bAllowClose)
			{
				BSTR BstrBeAlloswClose;
				if (m_bAllowClose)
				{
					BstrBeAlloswClose = _T("1");
				}
				else
				{
					BstrBeAlloswClose = _T("0");
				}

				pAttrItem->put_text(BstrBeAlloswClose);
			}
			pAttrItem.Release();
			break;

		}
		pAttrItem.Release();
	}



	m_bAllowClose = bAllowClose;
	pAttrs.Release();
	spNodeList.Release();


	spDoc->save((_variant_t)strIniFileName);
	spDoc.Release();
	::CoUninitialize();
	g_mutex.Unlock();
	return TRUE;
}

BOOL CMonitorApp::UpdateXmlAboutKbase()
{
	CString strIniFileName = _T("\\xml\\profile.xml");

	strIniFileName = m_strCanChangePath + strIniFileName;

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

	hr = spDoc->load(strIniFileName.AllocSysString());//load xml文件  

	MSXML2::IXMLDOMNodeListPtr spNodeList;
	MSXML2::IXMLDOMNodePtr pItem;

	MSXML2::IXMLDOMNamedNodeMapPtr pAttrs = NULL;

	spNodeList = spDoc->getElementsByTagName(_bstr_t("a"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->put_text(m_strKbaseId.AllocSysString());
	pItem.Release();
	spNodeList.Release();


	spNodeList = spDoc->getElementsByTagName(_bstr_t("b"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->put_text(m_strKbasePort.AllocSysString());
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("c"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->put_text(m_strKbaseUserName.AllocSysString());
	pItem.Release();
	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("d"));   //获取根结点  
	spNodeList->get_item(0, &pItem);
	pItem->put_text(m_DecTemp.base64encode(m_strKbasePassWord, m_strKbasePassWord.GetLength()).AllocSysString());

	pItem.Release();
	spNodeList.Release();


	spDoc->save((_variant_t)strIniFileName);
	spDoc.Release();
	::CoUninitialize();
	g_mutex.Unlock();
	return TRUE;
}

BOOL CMonitorApp::UpdatexmlUpdateDate()
{
	// 	::CoInitialize(NULL);
	// 	CComPtr<MSXML::IXMLDOMDocument> spDoc;  
	// 	HRESULT hr = spDoc.CoCreateInstance(__uuidof(MSXML::DOMDocument));    //创建文档对象 
	// 	VARIANT_BOOL bFlag; 
	// 	CString strPath=_T("\\xml\\UpdateDate_Info.xml");
	// 
	// 	strPath =m_strCanChangePath+strPath;
	// 	if (!PathFileExists(strPath))
	// 	{
	// 		AfxMessageBox(_T("配置文件不存在。"));
	// 		return FALSE;
	// 
	// 	}
	// 
	// 	CComVariant varNodeValue; 
	// 	hr = spDoc->load(CComVariant(strPath), &bFlag);               //load xml文件  
	// 	CComPtr<MSXML::IXMLDOMNodeList> spNodeList; 
	// 	//CComPtr<MSXML::IXMLDOMElement> spElement;
	// 	MSXML::IXMLDOMNodePtr pAttrItem;
	// 	CComPtr<MSXML::IXMLDOMNode> spNode;
	// 	CString strSetTemp;
	// 
	// 
	// 	hr = spDoc->getElementsByTagName(_bstr_t("ZZHC"),&spNodeList);   //获取根结点   
	// 	spNodeList->get_item(0,&pAttrItem);
	// 
	// 	strSetTemp.Format(_T("%u"),m_nlZzhcDate);
	// 	pAttrItem->put_nodeTypedValue((_variant_t)(LPCTSTR)strSetTemp);
	// 	pAttrItem.Release();
	// 
	// 	spNodeList->get_item(1,&pAttrItem);
	// 	strSetTemp.Format(_T("%u"),m_nLLoadPart);
	// 	pAttrItem->put_nodeTypedValue((_variant_t)(LPCTSTR)strSetTemp);
	// 	pAttrItem.Release();
	// 	spNodeList.Release(); 
	// 	spDoc.Release();  
	// 	::CoUninitialize();  
	// 	return TRUE;

	//CString strIniFileName = _T("\\xml\\UpdateDate_Info.xml");

	//strIniFileName = m_strCanChangePath + strIniFileName;

	//if (!PathFileExists(strIniFileName))
	//{
	//	AfxMessageBox(_T("配置文件不存在！"));
	//	return FALSE;
	//}

	////从xml中读取配置信息
	//::CoInitialize(NULL);
	//MSXML2::IXMLDOMDocumentPtr spDoc;
	//HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));    //创建文档对象 

	//BSTR bstrIniFileName = strIniFileName.AllocSysString();
	//SysFreeString(bstrIniFileName);
	//hr = spDoc->load(bstrIniFileName);//load xml文件  

	//MSXML2::IXMLDOMNodeListPtr spNodeList;
	//MSXML2::IXMLDOMNodePtr pItem;
	//CString strSetTemp;


	//spNodeList = spDoc->getElementsByTagName(_bstr_t("ZZHC"));   //获取根结点  

	//long lListNo = 0;
	//CString strGetTemp;
	//spNodeList->get_length(&lListNo);
	//if (lListNo>0)
	//{
	//	long lSubListNo = 0;
	//	MSXML2::IXMLDOMNodeListPtr spChildNodeList;
	//	MSXML2::IXMLDOMNodePtr pSubItem;
	//	spNodeList->get_item(0, &pItem);
	//	pItem->get_childNodes(&spChildNodeList);
	//	spChildNodeList->get_length(&lSubListNo);
	//	if (lSubListNo>0)
	//	{
	//		strSetTemp.Format(_T("%u"), m_nlZzhcDate);
	//		spChildNodeList->get_item(0, &pSubItem);
	//		BSTR bstrSetTemp = strSetTemp.AllocSysString();
	//		SysFreeString(bstrSetTemp);
	//		pSubItem->put_text(bstrSetTemp);
	//		pSubItem.Release();
	//	}
	//	if (lSubListNo>1)
	//	{
	//		LONGLONG llTemp = (LONGLONG)powl(10, 9);

	//		if (ULONG(m_nLLoadPart / llTemp)>0)
	//		{
	//			strSetTemp.Format(_T("%u%09u"), ULONG(m_nLLoadPart / llTemp), ULONG(m_nLLoadPart%llTemp));
	//		}
	//		else
	//		{
	//			strSetTemp.Format(_T("%u"), m_nLLoadPart);
	//		}
	//		spChildNodeList->get_item(1, &pSubItem);
	//		BSTR bstrSetTemp = strSetTemp.AllocSysString();
	//		SysFreeString(bstrSetTemp);
	//		pSubItem->put_text(bstrSetTemp);
	//		pSubItem.Release();
	//	}
	//	spChildNodeList.Release();

	//	pItem.Release();
	//}


	//spNodeList.Release();
	//spDoc->save((_variant_t)strIniFileName);

	//spDoc.Release();

	//::CoUninitialize();
	return TRUE;

}


BOOL CMonitorApp::ReadCloseTime()
{

	if (m_nGetInfoVersion == 0)
	{
		//BSTR bstrID = m_strKbaseId.AllocSysString();
		//BSTR bstrUserName = m_strKbaseUserName.AllocSysString();
		//BSTR bstrUserPassword = m_strKbasePassWord.AllocSysString();
		//UINT nPort = _ttoi(m_strKbasePort);

		//if (!IsOpenEx(m_nConnectNo) && !OpenEx(m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
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
		//strSql = _T("select SQL_NO_CACHE * from CAJTOUCHSYSTEMSET where ID=SS003");
		//BSTR bstrSql = strSql.AllocSysString();
		//SysFreeString(bstrSql);
		//UINT nRecord = 0;
		//if (!OpenRecordSetEx(m_nConnectNo, nRecord, bstrSql))
		//{
		//	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
		//	return FALSE;
		//}
		//UINT nNoCount = GetRecordCountEx(m_nConnectNo, nRecord);
		//if (nNoCount <= 0)
		//{
		//	CloseRecordSetEx(m_nConnectNo, nRecord);
		//	return FALSE;
		//}


		//m_strCloseTime = CString(GetFieldValueEx(m_nConnectNo, nRecord, 2));
		//CloseRecordSetEx(m_nConnectNo, nRecord);


		//if (m_strCloseTime.Find(_T("+")) >= 0)
		//{
		//	if (m_strCloseTime.Left(1) == _T("0"))
		//	{
		//		m_strCloseTime.Empty();
		//	}
		//	else
		//	{
		//		m_strCloseTime = m_strCloseTime.Right(m_strCloseTime.GetLength() - 2);
		//	}
		//}

	}
	else
	{
		//从xml中读取
		CString strIniFileName = _T("\\xml\\profile.xml");

		strIniFileName = m_strCanChangePath + strIniFileName;

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

		hr = spDoc->load(strIniFileName.AllocSysString());//load xml文件  

		MSXML2::IXMLDOMNodeListPtr spNodeList;
		MSXML2::IXMLDOMNodePtr pItem;
		MSXML2::IXMLDOMNamedNodeMapPtr pAttrs = NULL;

		//关闭时间
		spNodeList = spDoc->getElementsByTagName(_bstr_t("f"));   //获取根结点  
		long nListNo = 0;
		spNodeList->get_length(&nListNo);
		if (nListNo <= 0)
		{
			m_strCloseTime.Empty();
			spNodeList.Release();
			spDoc.Release();
			::CoUninitialize();
			g_mutex.Unlock();
			return FALSE;
		}

		spNodeList->get_item(0, &pItem);
		pItem->get_text(&strNodeValue);
		m_strCloseTime = (CString)strNodeValue;

		pItem->get_attributes(&pAttrs);
		pItem.Release();


		long nCount;
		CString strAttrName;
		MSXML2::IXMLDOMNodePtr pAttrItem;
		BOOL bAllowClose = TRUE;
		pAttrs->get_length(&nCount); //获取节点属性个数
		for (int i = 0; i<nCount; i++)
		{
			pAttrs->get_item(i, &pAttrItem);

			strAttrName = (char*)(_bstr_t)pAttrItem->nodeName;
			if (strAttrName == _T("ballowclose"))
			{
				CString strAttrValue = (LPCTSTR)(_bstr_t)pAttrItem->nodeTypedValue;

				if (strAttrValue == _T("0"))
				{
					bAllowClose = FALSE;
				}
				pAttrItem.Release();
				break;
			}
			pAttrItem.Release();
		}


		if (bAllowClose == FALSE)
		{
			m_strCloseTime.Empty();
		}

		pAttrs.Release();

		spNodeList.Release();

		spDoc.Release();
		::CoUninitialize();

		g_mutex.Unlock();
	}

	return TRUE;
	 	//CString strPath=_T("\\xml\\profile3.html");
		//
	 	//strPath =m_strCanChangePath+strPath;
	 	//if (!PathFileExists(strPath))
	 	//{
	 	//	return FALSE;
	 	//}
	 
	 	//CMarkup xml;
	 	//if (!xml.Load(strPath))
	 	//{
	 	//	return FALSE;
	 	//}
	 
	 	//if (!xml.FindChildElem())
	 	//{
	 	//	return FALSE;
	 	//}
	 	//xml.IntoElem();
	 
	 	//do{      
	 	//	int iJust=_ttoi(xml.GetAttrib(xml.GetAttribName(0)));
	 	//	CString strValue=xml.GetData();
	 
	 	//	if (iJust==11)
	 	//	{
	 	//		m_strCloseTime=strValue;
	 	//		break;
	 	//	}
	  //                 
	 	//}   while (xml.FindElem());
	 
	 	//xml.OutOfElem();
	 	//return TRUE;
}

BOOL CMonitorApp::ReadDataPathAndStaleDate()
{
	//CString strIniFileName = _T("\\xml\\profile.xml");

	//strIniFileName = m_strCanChangePath + strIniFileName;

	//if (!PathFileExists(strIniFileName))
	//{
	//	AfxMessageBox(_T("配置文件不存在！"));
	//	return FALSE;
	//}

	//CMutex g_mutex(false, _T("mutex_Update_profile.xml"));
	//g_mutex.Lock();
	////从xml中读取配置信息
	//::CoInitialize(NULL);
	//BSTR strNodeValue;
	//MSXML2::IXMLDOMDocumentPtr spDoc;
	//HRESULT hr = spDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));    //创建文档对象 

	//BSTR bstrIniFileName = strIniFileName.AllocSysString();
	//SysFreeString(bstrIniFileName);

	//hr = spDoc->load(bstrIniFileName);//load xml文件  

	//MSXML2::IXMLDOMNodeListPtr spNodeList;
	//MSXML2::IXMLDOMNodePtr pItem;

	//spNodeList = spDoc->getElementsByTagName(_bstr_t("k"));   //获取根结点  
	//long nListNo = 0;
	//spNodeList->get_length(&nListNo);
	//if (nListNo <= 0)
	//{
	//	m_nStaleDate = 0;
	//	spNodeList.Release();
	//	spDoc.Release();
	//	::CoUninitialize();
	//	g_mutex.Unlock();
	//	return FALSE;
	//}

	//spNodeList->get_item(0, &pItem);
	//pItem->get_text(&strNodeValue);
	//m_nStaleDate = _ttoi((CString)strNodeValue);

	//pItem.Release();

	//spNodeList = spDoc->getElementsByTagName(_bstr_t("g"));   //获取根结点  
	//spNodeList->get_length(&nListNo);
	//if (nListNo <= 0)
	//{
	//	m_strDataPath.Empty();
	//	spNodeList.Release();
	//	spDoc.Release();
	//	::CoUninitialize();
	//	g_mutex.Unlock();
	//	return FALSE;
	//}

	//spNodeList->get_item(0, &pItem);
	//pItem->get_text(&strNodeValue);
	//m_strDataPath = (CString)strNodeValue;
	//pItem.Release();

	//spNodeList.Release();

	//spDoc.Release();
	//::CoUninitialize();
	//g_mutex.Unlock();
	return TRUE;
}

BOOL CMonitorApp::ReadServerId()
{
	CString strIniFileName = _T("\\xml\\profile.xml");

	strIniFileName = m_strCanChangePath + strIniFileName;

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

	spNodeList = spDoc->getElementsByTagName(_bstr_t("a"));   //获取根结点  
	long nListNo = 0;
	spNodeList->get_length(&nListNo);
	if (nListNo <= 0)
	{
		m_strServerId.Empty();
		spNodeList.Release();
		spDoc.Release();
		::CoUninitialize();
		g_mutex.Unlock();
		return FALSE;
	}

	spNodeList->get_item(0, &pItem);
	pItem->get_text(&strNodeValue);
	m_strServerId = (CString)strNodeValue;
	//m_sKbasId = m_strServerId.AllocSysString();

	pItem.Release();

	spNodeList.Release();

	spDoc.Release();
	::CoUninitialize();
	g_mutex.Unlock();
	return TRUE;
}

BOOL CMonitorApp::ReadVersion()
{
	CString strIniFileName = _T("\\xml\\profile.xml");

	strIniFileName = m_strCanChangePath + strIniFileName;

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

	spNodeList = spDoc->getElementsByTagName(_bstr_t("m"));   //获取根结点  
	long nListNo = 0;
	spNodeList->get_length(&nListNo);
	if (nListNo <= 0)
	{
		spNodeList.Release();
		spDoc.Release();
		::CoUninitialize();
		g_mutex.Unlock();
		return FALSE;
	}

	spNodeList->get_item(0, &pItem);
	pItem->get_text(&strNodeValue);
	m_nGetInfoVersion = _ttoi((CString)strNodeValue);

	pItem.Release();

	spNodeList.Release();

	spDoc.Release();
	::CoUninitialize();
	g_mutex.Unlock();
	return TRUE;
}


BOOL CMonitorApp::ReadCnkiServerId()
{
	CString strIniFileName = _T("\\xml\\profile.xml");

	strIniFileName = m_strCanChangePath + strIniFileName;

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

	spNodeList = spDoc->getElementsByTagName(_bstr_t("p"));   //获取根结点  
	long nListNo = 0;
	spNodeList->get_length(&nListNo);
	if (nListNo <= 0)
	{
		m_strCnkiServerId.Empty();
		spNodeList.Release();
		spDoc.Release();
		::CoUninitialize();
		g_mutex.Unlock();
		return FALSE;
	}

	spNodeList->get_item(0, &pItem);
	pItem->get_text(&strNodeValue);
	m_strCnkiServerId = (CString)strNodeValue;

	pItem.Release();

	spNodeList.Release();

	spDoc.Release();
	::CoUninitialize();
	g_mutex.Unlock();
	return TRUE;
}

BOOL CMonitorApp::ReadStartDate()
{
	CString strIniFileName = _T("\\xml\\profile.xml");

	strIniFileName = m_strCanChangePath + strIniFileName;

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

	spNodeList = spDoc->getElementsByTagName(_bstr_t("l"));   //获取根结点  
	long nListNo = 0;
	spNodeList->get_length(&nListNo);
	if (nListNo <= 0)
	{
		spNodeList.Release();
		spDoc.Release();
		::CoUninitialize();
		g_mutex.Unlock();
		return FALSE;
	}

	spNodeList->get_item(0, &pItem);
	pItem->get_text(&strNodeValue);
	m_strStartYearMonthDay = (CString)strNodeValue;

	pItem.Release();

	spNodeList.Release();

	spDoc.Release();
	::CoUninitialize();
	g_mutex.Unlock();
	return TRUE;
}


BOOL CMonitorApp::ReadTotalSavePath()
{
	CString strIniFileName = _T("\\xml\\profile.xml");

	strIniFileName = m_strCanChangePath + strIniFileName;

	if (!PathFileExists(strIniFileName))
	{
		AfxMessageBox(_T("配置文件不存在！"));
		return FALSE;
	}


	CMutex g_mutex(false, _T("begin ReadTotalSavePath():\nmutex_Update_profile.xml"));
	CString m_strDateTime;
	CTime m_time;
	g_mutex.Lock();
	m_time = CTime::GetCurrentTime();
	m_strDateTime = m_time.Format(_T("begin readTotalSave:%Y-%m-%d %H:%M:%S\n"));
	WriteLog(m_strDateTime);
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

	spNodeList = spDoc->getElementsByTagName(_bstr_t("w"));   //获取根结点  
	long nListNo = 0;
	spNodeList->get_length(&nListNo);
	if (nListNo <= 0)
	{
		spNodeList.Release();
		spDoc.Release();
		::CoUninitialize();
		g_mutex.Unlock();
		m_time = CTime::GetCurrentTime();
		m_strDateTime = m_time.Format(_T("end readTotalSave:%Y-%m-%d %H:%M:%S\r\n"));
		WriteLog(m_strDateTime);
		return FALSE;
	}

	spNodeList->get_item(0, &pItem);
	pItem->get_text(&strNodeValue);
	m_strTotalDataPath = (CString)strNodeValue;

	pItem.Release();

	spNodeList.Release();

	spNodeList = spDoc->getElementsByTagName(_bstr_t("x"));   //获取根结点  
	nListNo = 0;
	spNodeList->get_length(&nListNo);
	if (nListNo <= 0)
	{
		spNodeList.Release();
		spDoc.Release();
		::CoUninitialize();
		g_mutex.Unlock();
		return FALSE;
	}

	spNodeList->get_item(0, &pItem);
	pItem->get_text(&strNodeValue);
	m_strTotalFmPath = (CString)strNodeValue;

	pItem.Release();

	spNodeList.Release();

	spDoc.Release();
	::CoUninitialize();
	g_mutex.Unlock();
	m_time = CTime::GetCurrentTime();
	m_strDateTime = m_time.Format(_T("end ReadTotalSavePath():\n%Y-%m-%d %H:%M:%S\r\n"));
	WriteLog(m_strDateTime);
	return TRUE;
}

BOOL CMonitorApp::ReadSerialNo()
{

	// 从xml中读取
	if (m_nGetInfoVersion == 0)
	{
		//BSTR bstrID = m_strKbaseId.AllocSysString();
		//BSTR bstrUserName = m_strKbaseUserName.AllocSysString();
		//BSTR bstrUserPassword = m_strKbasePassWord.AllocSysString();
		//UINT nPort = _ttoi(m_strKbasePort);

		//if (!IsOpenEx(m_nConnectNo) && !OpenEx(m_nConnectNo, bstrUserName, bstrUserPassword, bstrID, _T(""), nPort))
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
		//strSql = _T("select SQL_NO_CACHE * from CAJTOUCHSYSTEMSET where ID=SS005");
		//BSTR bstrSql = strSql.AllocSysString();
		//SysFreeString(bstrSql);
		//UINT nRecord = 0;
		//if (!OpenRecordSetEx(m_nConnectNo, nRecord, bstrSql))
		//{
		//	AfxMessageBox(_T("OpenRecordSet Error\n") + strSql);
		//	return FALSE;
		//}
		//UINT nNoCount = GetRecordCountEx(m_nConnectNo, nRecord);
		//if (nNoCount <= 0)
		//{
		//	CloseRecordSetEx(m_nConnectNo, nRecord);
		//	return FALSE;
		//}

		//m_strSerialNo = CString(GetFieldValueEx(m_nConnectNo, nRecord, 2));
		//CloseRecordSetEx(m_nConnectNo, nRecord);
	}
	else
	{
		CString strIniPath = _T("\\xml\\serialno.ini");

		strIniPath = m_strCanChangePath + strIniPath;

		if (!PathFileExists(strIniPath))
		{
			CFile m_file;
			m_file.Open(strIniPath, CFile::modeCreate | CFile::modeWrite);
			m_file.Close();
		}
		else
		{
			CString strSerialNo;
 			::GetPrivateProfileStringW(_T("SerialNo"), _T("No"), _T("NULL"), strSerialNo.GetBuffer(MAX_PATH), MAX_PATH, strIniPath);
			strSerialNo.ReleaseBuffer();
			if (strSerialNo != _T("NULL"))
			{
				m_strSerialNo = strSerialNo;
			}
		}
	}

	return TRUE;
}

void CMonitorApp::GetBuyYear()
{
	if (m_strSerialNo.IsEmpty())
	{
		m_strBuyEndYear.Empty();
		m_strBuyStartYear.Empty();
		return;
	}

	CString strDate;
	CString strGetRegistNo;
	
	
	//m_strSerialNo = _T("S8VB8jJJYZWaZZZjJPsQNjZSJSSPEbX7XNEOXZqJ77OsLJiqJLNk5bnM");
	//m_strRegestNo = _T("0AZL - AR33 - YDEU - DDDR - 31OG - HRD03");

	m_DecTemp.DecryptionDate(m_strSerialNo, m_strRegestNo, strDate);
	m_strBuyStartYear = strDate.Left(4);
	m_strBuyEndYear = strDate.Right(2);
	if (_ttoi(m_strBuyEndYear) >= 90)
	{
		m_strBuyEndYear.Format(__T("%d"), _ttoi(m_strBuyStartYear) + _ttoi(m_strBuyEndYear.Right(1)));
	}
	else
	{
		m_strBuyEndYear.Format(__T("%d"), _ttoi(m_strBuyStartYear) + _ttoi(m_strBuyEndYear));
	}
}


void CMonitorApp::CorrectBuyYearEx(ULONG& nLStartYMD, ULONG& nLEndYMD)
{
	CString strStartYearMonthDay, strEndYearMonthDay;
	CTime t = CTime::GetCurrentTime(); //获取系统日期
	ULONG nlNowDate = t.GetYear();
	if (_ttoi(m_strBuyEndYear) >= (int)nlNowDate)
	{

		strEndYearMonthDay.Format(_T("%ld"), nlNowDate * 100 * 100 + t.GetMonth() * 100 + t.GetDay());
	}
	else
	{
		strEndYearMonthDay = m_strBuyEndYear + _T("1231");
	}


	strStartYearMonthDay = m_strBuyStartYear + _T("0101");

	nLStartYMD = _ttoi(strStartYearMonthDay);
	nLEndYMD = _ttoi(strEndYearMonthDay);
}

void CMonitorApp::CorrectBuyYear(ULONG& nLStartYM, ULONG& nLEndYM)
{
	CString strStartYearMonth, strEndYearMonth;
	CTime t = CTime::GetCurrentTime(); //获取系统日期
	ULONG nlNowDate = t.GetYear();
	if (_ttoi(m_strBuyEndYear) >= (int)nlNowDate)
	{
		strEndYearMonth.Format(_T("%ld"), nlNowDate * 100 + t.GetMonth());
	}
	else
	{
		strEndYearMonth = m_strBuyEndYear + _T("12");
	}

	if (_ttol(m_strBuyStartYear) <= _ttol(m_strStartYearMonth.Left(4)))
	{
		strStartYearMonth = m_strStartYearMonth;
	}
	else
	{
		strStartYearMonth = m_strBuyStartYear + _T("01");
	}

	nLStartYM = _ttoi(strStartYearMonth);
	nLEndYM = _ttoi(strEndYearMonth);
}



BOOL CMonitorApp::SendStrFromWeb(CString strUrl, CString& strReturn,  const char* content, const char *token)
{
	DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS;
	CInternetSession *sess = new CInternetSession(NULL, dwAccessType);
	sess->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 800 * 1000);
	sess->SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, 800 * 1000);
	sess->SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, 800 * 1000);
	//const TCHAR szHeaders[] = _T("Content-Type:application/json\r\nAccept-Encoding:gzip, deflate, br\n\rAccept:*/* \r\nConnection: Keep-Alive\r\nUser-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\napplication/x-www-form-urlencoded\r\nAccept-Encoding:gzip,deflate,br\r\n");
	wstring strHtml = _T("");
	DWORD dwHttpReauwstFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE;
	const TCHAR szHeaders[] = _T("Content-Type:application/json\r\nAccept-Encoding:gzip,deflate,br\n\rapplication/x-www-form-urlencoded\r\nAccept:*/* \r\nConnection: Keep-Alive\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:80.0) Gecko/20100101 Firefox/80.0\r\napplication/x-www-form-urlencoded\r\nAccept-Encoding:gzip,deflate,br\r\n");
	CHttpConnection *pServer = NULL;
	CHttpFile* pFile = NULL;

	/*   try*/
	CString strServerName;
	CString strObject;
	INTERNET_PORT nPort;
	DWORD dwServiceType;

	CSemaphore semaphore(3, 3, NULL, NULL);
	semaphore.Lock();
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
		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject, NULL, (DWORD)1, NULL, NULL, dwHttpReauwstFlags);

		if (!pFile)
		{
			//发起post请求失败   
			delete pServer; pServer = NULL;
			sess->Close(); delete sess;
			return FALSE;
		}

		CString strHeaderTemp;
		if (*token != '\0') {
			int wcslen = ::MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)token, -1, NULL, 0);
			TCHAR* wszString = new TCHAR[wcslen + 1];
			::MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)token, -1, wszString, wcslen);
			wszString[wcslen] = '\0';
			strHeaderTemp.Format(_T("%s\r\n%s"), wszString, szHeaders);
		}
		else
			strHeaderTemp = szHeaders;

		int iFind = strHeaderTemp.Find(_T("\r\n"));
		while (iFind>0)
		{
			CString strHeaderPart = strHeaderTemp.Left(iFind);
			pFile->AddRequestHeaders(strHeaderPart);	// add: add one or more http request headers
			strHeaderTemp = strHeaderTemp.Right(strHeaderTemp.GetLength() - iFind - CString(_T("\r\n")).GetLength());
			iFind = strHeaderTemp.Find(_T("\r\n"));
		}
		BOOL bSendSuc = pFile->SendRequest(NULL, 0, (void*)content, strlen(content));
		if (!bSendSuc)
		{
			delete pFile; pFile = NULL;
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

//BOOL CMonitorApp::SendStrFromWeb(CString strUrl, CString& strReturn, const char* content)
//{
//
//
//	DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS;
//	CInternetSession *sess = new CInternetSession(NULL, dwAccessType);
//	sess->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 800 * 1000);
//	sess->SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, 800 * 1000);
//	sess->SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, 800 * 1000);
//
//	wstring strHtml = _T("");
//	DWORD dwHttpReauwstFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE;
//	//const TCHAR szHeaders[] = _T("Accept: text/*\r\nUser-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\nConnection: Keep-Alive\r\n\r\n");
//
//	CHttpConnection *pServer = NULL;
//	CHttpFile* pFile = NULL;
//
//	/*   try*/
//	CString strServerName;
//	CString strObject;
//	INTERNET_PORT nPort;
//	DWORD dwServiceType;
//
//	CSemaphore semaphore(3, 3, NULL, NULL);
//	semaphore.Lock();
//	try
//	{
//
//		if (!AfxParseURL(strUrl, dwServiceType, strServerName, strObject, nPort) ||
//			dwServiceType != INTERNET_SERVICE_HTTP)
//		{
//			return FALSE;
//		}
//
//		pServer = sess->GetHttpConnection(strServerName, nPort);
//		if (pServer == NULL)
//		{
//			//建立网络连接失败  
//			sess->Close(); delete sess;
//			return FALSE;
//		}
//		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject, NULL, (DWORD)1, NULL, NULL, dwHttpReauwstFlags);
//
//		if (!pFile)
//		{
//			//发起post请求失败   
//			delete pServer; pServer = NULL;
//			sess->Close(); delete sess;
//			return FALSE;
//		}
//		//pFile->AddRequestHeaders(szHeaders);
//
//
//
//
//		BOOL bSendSuc = pFile->SendRequest(NULL, 0, (void*)content, strlen(content));
//		if (!bSendSuc)
//		{
//			delete pFile; pFile = NULL;
//			delete pServer; pServer = NULL;
//			sess->Close(); delete sess;
//			return FALSE;
//		}
//
//
//
//		TCHAR sz[BUFFER_SIZE + 1];
//		pFile->SetReadBufferSize(BUFFER_SIZE);
//		CString strTemp = _T("");
//		int pageCode = -1;
//		CString strTestKind;
//		while (pFile->ReadString(sz, BUFFER_SIZE))
//		{
//			strTemp += sz;
//			int wcslen = ::MultiByteToWideChar(CP_UTF8, NULL, (LPCSTR)sz, -1, NULL, 0);
//			TCHAR* wszString = new TCHAR[wcslen + 1];
//			::MultiByteToWideChar(CP_UTF8, NULL, (LPCSTR)sz, -1, wszString, wcslen);
//			wszString[wcslen] = '\0';
//			strHtml.append(wszString);
//			strTestKind += wszString;
//			delete[] wszString;
//			strTestKind.MakeLower();
//			if (strTestKind.Find(_T("charset=utf-8")) != -1)
//			{
//				pageCode = CP_UTF8;
//				break;
//			}
//			else if (strTestKind.Find(_T("charset=gb2312")) != -1)
//			{
//				pageCode = CP_ACP;
//				break;
//			}
//			else if (strTestKind.Find(_T("charset=gbk")) != -1)
//			{
//				pageCode = CP_ACP;
//				break;
//			}
//
//		}
//
//		if (pageCode != -1)
//		{
//			// 			if (pageCode==CP_UTF8)
//			// 			{
//			// 				strHtml.erase(strHtml.begin(),strHtml.end());
//			// 				TCHAR *szTemp=strTemp.GetBuffer(strTemp.GetLength());
//			// 
//			// 				int wcslen=::MultiByteToWideChar(pageCode,NULL,(LPCSTR)szTemp,-1,NULL,0);
//			// 				TCHAR* wszString=new TCHAR[wcslen+1];
//			// 				::MultiByteToWideChar(pageCode,NULL,(LPCSTR)szTemp,-1,wszString,wcslen);
//			// 				strTemp.ReleaseBuffer();
//			// 				wszString[wcslen]='\0';
//			// 				strHtml.append(wszString);
//			// 				delete [] wszString;
//			// 
//			// 			}
//
//			//阅读余下的部分
//			while (pFile->ReadString((LPTSTR)sz, BUFFER_SIZE))
//			{
//				int wcslen = ::MultiByteToWideChar(pageCode, NULL, (LPCSTR)sz, -1, NULL, 0);
//				TCHAR* wszString = new TCHAR[wcslen + 1];
//				::MultiByteToWideChar(pageCode, NULL, (LPCSTR)sz, -1, wszString, wcslen);
//				wszString[wcslen] = '\0';
//				strHtml.append(wszString);
//				delete[] wszString;
//
//
//			}
//
//		}
//		/*	delete [] sz;*/
//		pFile->Close();
//		delete pFile; pFile = NULL;
//		pServer->Close();
//		delete pServer; pServer = NULL;
//		sess->Close(); delete sess;
//		strReturn = CString(strHtml.c_str());
//	}
//	catch (...)
//	{
//		pFile->Close();
//		delete pFile; pFile = NULL;
//		pServer->Close();
//		delete pServer; pServer = NULL;
//		sess->Close(); delete sess;
//		return FALSE;
//	}
//	return TRUE;
//}


BOOL CMonitorApp::GetStrFromWeb(CString strUrl, CString& strReturn, CString strHeader)
{
	DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS;
	CInternetSession *sess = new CInternetSession(NULL, dwAccessType);
	sess->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 800 * 1000);
	sess->SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, 800 * 1000);
	sess->SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, 800 * 1000);

	wstring strHtml = _T("");
	DWORD dwHttpReauwstFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE;
	CString szHeaders = strHeader;

	if (strHeader.IsEmpty())
	{
		// add: szHeaders=_T("Accept: text/*\r\nContent-Type:application/json\r\nUser-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\nConnection: Keep-Alive\r\n");
		//szHeaders = _T("Accept: text/*\r\nUser-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\nConnection: Keep-Alive\r\n")
		szHeaders = _T("Content - Type: application / json\r\nAccept:*/* \r\nConnection: Keep-Alive\r\nUser-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n\r\n");
	}

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
			(dwServiceType != AFX_INET_SERVICE_HTTPS  && dwServiceType != AFX_INET_SERVICE_HTTP) )
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
			// add: 打开http连接失败
			//发起GET请求失败   
			delete pServer; pServer = NULL;
			sess->Close(); delete sess;
			return FALSE;
		}
		CString strHeaderTemp = szHeaders;
		int iFind = strHeaderTemp.Find(_T("\r\n"));
		while (iFind>0)
		{
			CString strHeaderPart = strHeaderTemp.Left(iFind);
			pFile->AddRequestHeaders(strHeaderPart);	// add: add one or more http request headers
			strHeaderTemp = strHeaderTemp.Right(strHeaderTemp.GetLength() - iFind - CString(_T("\r\n")).GetLength());
			iFind = strHeaderTemp.Find(_T("\r\n"));

		}





		BOOL bSendSuc = pFile->SendRequest();		// add: 向http服务器发送请求,此处参数默认设置为get方式
		if (!bSendSuc)
		{
			delete pFile; pFile = NULL;
			delete pServer; pServer = NULL;
			sess->Close(); delete sess;
			return FALSE;
		}

		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);	// add: Retrieves the status code associated with an HTTP request
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

		strHeaderTemp = szHeaders;
		iFind = strHeaderTemp.Find(_T("\r\n"));
		while (iFind>0)
		{
			CString strHeaderPart = strHeaderTemp.Left(iFind);
			pFile->AddRequestHeaders(strHeaderPart);
			strHeaderTemp = strHeaderTemp.Right(strHeaderTemp.GetLength() - iFind - CString(_T("\r\n")).GetLength());
			iFind = strHeaderTemp.Find(_T("\r\n"));

		}
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

CMutex g_mutexsqlitoperator_read(false, _T("mutex_sqlit_operator_read"));
CMutex g_mutexsqlitoperator_write(false, _T("mutex_sqlit_operator_write"));
CSemaphore g_Semapsqlitoperator_ReadNo(0,200,_T("semaphore_sqlit_operator_read"));


bool CMonitorApp::LockRead(long lTimeOut, long lTimeOut1)
{
	if (lTimeOut>0 || lTimeOut == INFINITE)
	{
		if (!g_mutexsqlitoperator_read.Lock(lTimeOut)) return false;

		if (!g_Semapsqlitoperator_ReadNo.Lock(lTimeOut1))//前面无人读取
		{
			if (!g_mutexsqlitoperator_write.Lock(lTimeOut1))
			{
				g_mutexsqlitoperator_read.Unlock();
				return false;
			}
		}
		else
		{
			g_Semapsqlitoperator_ReadNo.Unlock();
		}

		if (!g_Semapsqlitoperator_ReadNo.Unlock())
		{
			g_mutexsqlitoperator_read.Unlock();
			return false;
		}

		g_mutexsqlitoperator_read.Unlock();
		return true;
	}
	return false;

}
bool CMonitorApp::UnLockRead()
{
	g_mutexsqlitoperator_read.Lock();
	g_Semapsqlitoperator_ReadNo.Lock();

	if (!g_Semapsqlitoperator_ReadNo.Lock(10))//前面无人读取
	{
		g_mutexsqlitoperator_write.Unlock();
	}
	else
	{
		g_Semapsqlitoperator_ReadNo.Unlock();
	}

	g_mutexsqlitoperator_read.Unlock();
	return true;


}

bool CMonitorApp::LockWrite(long lTimeOut, long lTimeOut1)
{

    bool bHaveReadPerson=true;
	if (!g_mutexsqlitoperator_read.Lock(1)) return false;

	if (!g_Semapsqlitoperator_ReadNo.Lock(1))//前面无人读取
	{
		bHaveReadPerson=false;
	}
	else
	{
       g_Semapsqlitoperator_ReadNo.Unlock();
	}
	g_mutexsqlitoperator_read.Unlock();
     
	if (!bHaveReadPerson)
	{
	 return  g_mutexsqlitoperator_write.Lock(lTimeOut);
	}
    Sleep(10);
	return false;
}
bool CMonitorApp::UnLockWrite()
{
	g_mutexsqlitoperator_write.Unlock();
	return true;
}



BOOL CMonitorApp::FindPrevInstance()
{
	CreateMutex(NULL, TRUE, _T("Monitor"));

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CMonitorApp::CloseThread(DWORD id)
//通过id关闭进程
{
	if (id != 0)
	{
		HANDLE Hwnd;
		Hwnd = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, 0, id);
		if (Hwnd)
		{
			if (!TerminateProcess(Hwnd, 0))
			{
				AfxMessageBox(_T("关闭程序失败！"));
				return FALSE;
			}
		}
	}

	return TRUE;
}



BOOL CMonitorApp::ClosePross(CString strProssName)
{
	//关闭开启的cajatouchview
	DWORD id = 0;
	HANDLE   SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot == NULL)
	{
		AfxMessageBox(_T("检测当前进程失败! "));
		return FALSE;
	}
	SHFILEINFO   shSmall;
	CString strProName;
	PROCESSENTRY32   ProcessInfo;//声明进程信息变量 
	ProcessInfo.dwSize = sizeof(ProcessInfo);//设置ProcessInfo的大小 
	//返回系统中第一个进程的信息 
	BOOL   Status = Process32First(SnapShot, &ProcessInfo);
	int   m_nProcess = 0, num = 0;
	while (Status)
	{
		m_nProcess++;
		num++;
		//获取进程文件信息 
		SHGetFileInfo(ProcessInfo.szExeFile, 0, &shSmall,
			sizeof(shSmall), SHGFI_ICON | SHGFI_SMALLICON);
		//在列表控件中添加映像名称 
		strProName = ProcessInfo.szExeFile;
		if (strProName == strProssName)
		{
			id = ProcessInfo.th32ProcessID;

			CloseHandle(SnapShot);
			break;
		}
		//获取下一个进程的信息 
		Status = Process32Next(SnapShot, &ProcessInfo);
	}
	CloseHandle(SnapShot);


	//通过id关闭进程
	if (id != 0)
	{
		HANDLE Hwnd;
		Hwnd = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, 0, id);
		if (Hwnd)
		{
			if (!TerminateProcess(Hwnd, 0))
			{
				AfxMessageBox(_T("关闭程序失败！"));
				return FALSE;
			}
		}
	}

	return TRUE;
}

__int64 CMonitorApp::myFileSeek(HANDLE hf, __int64 distance, DWORD MoveMethod)

{

	LARGE_INTEGER li;

	li.QuadPart = distance;

	li.LowPart = SetFilePointer(hf,

		li.LowPart,

		&li.HighPart,

		MoveMethod);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError()

		!= NO_ERROR)

	{

		li.QuadPart = -1;

	}

	return li.QuadPart;

}




BOOL CMonitorApp::DownLoadDataNOIDC(CString strDownLoadUrl, CString& strSavePath, UINT nKind)//nKind 1 下载内容带后缀   2下载内容不带后缀
{
	// 查询是否有完整性校验码  IDC
	CString strIDC;
	//int iStart = strDownLoadUrl.Find(_T("IDC="));
	//int	iEnd = 0;

	//if (iStart)
	//{
	//	iEnd = strDownLoadUrl.Find(_T("&"), iStart);
	//	if (iEnd>0)
	//	{
	//		strIDC = strDownLoadUrl.Mid(iStart + 4, iEnd - iStart - 4);

	//	}
	//	else
	//	{
	//		strIDC = strDownLoadUrl.Right(strDownLoadUrl.GetLength() - iStart - 4);
	//	}

	//	strDownLoadUrl = strDownLoadUrl.Left(iStart - 1);//-1 去掉 ？
	//}



	//查询保存路径 是否存在 不存在则创建

	if (strSavePath.Find(_T("\\\\")) >= 0)
	{
		strSavePath.Replace(_T("\\\\"), _T("\\"));
	}

	CString strTemp = strSavePath;
	int iFindGapNo = strTemp.Find(_T("\\"));
	while (iFindGapNo>0)
	{
		strTemp = strSavePath.Left(iFindGapNo);
		if (strTemp.Find(_T("\\"))>0 && !PathFileExists(strTemp))
		{
			CreateDirectory(strTemp, NULL);
		}
		iFindGapNo = strSavePath.Find(_T("\\"), iFindGapNo + 1);

	}
	if (nKind == 1)
	{
		strSavePath += strDownLoadUrl.Right(strDownLoadUrl.GetLength() - strDownLoadUrl.ReverseFind('.'));

	}


	//if (PathFileExists(strSavePath))
	//{
	//	DeleteFile(strSavePath);
	//}

	if (strDownLoadUrl.GetLength() < 7 || strDownLoadUrl.Left(7) != _T("http://"))
	{
		strDownLoadUrl = _T("http://") + strDownLoadUrl;
	}


	ULONGLONG lTemp = 0;
	UINT nResult = theApp.RequestHttp(strDownLoadUrl, strSavePath, lTemp);
	if (nResult != RES_REQ_DONE_OK)
	{
		DeleteFile(strSavePath);

		if (nResult == RES_REQ_ERROR_NOSPACE)
		{
			AfxMessageBox(_T(" 空间不足！  文件  ") + strSavePath + _T("  存储失败！"), MB_OK | MB_SYSTEMMODAL);
		}
		return false;
	}

	//if (!strIDC.IsEmpty() && GetFileMd5(strSavePath) != strIDC)
	//{
	//	DeleteFile(strSavePath);
	//	return false;
	//}

	return true;
}












BOOL CMonitorApp::DownLoadData(CString strDownLoadUrl, CString& strSavePath, UINT nKind)//nKind 1 下载内容带后缀   2下载内容不带后缀
{
	// 查询是否有完整性校验码  IDC
	CString strIDC;
	int iStart = strDownLoadUrl.Find(_T("IDC="));
	int	iEnd = 0;

	if (iStart)
	{
		iEnd = strDownLoadUrl.Find(_T("&"), iStart);
		if (iEnd>0)
		{
			strIDC = strDownLoadUrl.Mid(iStart + 4, iEnd - iStart - 4);

		}
		else
		{
			strIDC = strDownLoadUrl.Right(strDownLoadUrl.GetLength() - iStart - 4);
		}

		strDownLoadUrl = strDownLoadUrl.Left(iStart - 1);//-1 去掉 ？
	}



	//查询保存路径 是否存在 不存在则创建

	if (strSavePath.Find(_T("\\\\")) >= 0)
	{
		strSavePath.Replace(_T("\\\\"), _T("\\"));
	}

	CString strTemp = strSavePath;
	int iFindGapNo = strTemp.Find(_T("\\"));
	while (iFindGapNo>0)
	{
		strTemp = strSavePath.Left(iFindGapNo);
		if (strTemp.Find(_T("\\"))>0 && !PathFileExists(strTemp))
		{
			CreateDirectory(m_strCanChangePath, NULL);
		}
		iFindGapNo = strSavePath.Find(_T("\\"), iFindGapNo + 1);

	}
	if (nKind == 1)
	{
		strSavePath += strDownLoadUrl.Right(strDownLoadUrl.GetLength() - strDownLoadUrl.ReverseFind('.'));

	}


	if (PathFileExists(strSavePath))
	{
		DeleteFile(strSavePath);
	}

	if (strDownLoadUrl.GetLength() < 7 || strDownLoadUrl.Left(7) != _T("http://"))
	{
		strDownLoadUrl = _T("http://") + strDownLoadUrl;
	}


	ULONGLONG lTemp = 0;
	UINT nResult = theApp.RequestHttp(strDownLoadUrl, strSavePath, lTemp);
	if (nResult != RES_REQ_DONE_OK)
	{
		DeleteFile(strSavePath);

		if (nResult == RES_REQ_ERROR_NOSPACE)
		{
			AfxMessageBox(_T(" 空间不足！  文件  ") + strSavePath + _T("  存储失败！"), MB_OK | MB_SYSTEMMODAL);
		}
		return false;
	}

	if (!strIDC.IsEmpty() && GetFileMd5(strSavePath) != strIDC)
	{
		DeleteFile(strSavePath);
		return false;
	}

	return true;
}


int CMonitorApp::RequestHttp(LPCTSTR url, LPCTSTR lpszSaveName, ULONGLONG& nLoadedPart)
{
	//	CInternetSession *sess = new CInternetSession;
	CInternetSession session(_T(""), 1, PRE_CONFIG_INTERNET_ACCESS, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10 * 60 * 1000);
	session.SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, 10 * 60 * 1000);
	session.SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, 10 * 60 * 1000);
	session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 10 * 60 * 1000);
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 10); //重试次数 

REDIRECT1_:

	CString strHtml = _T("");
	DWORD dwVersion = GetVersion();
	CString strTemp;
	strTemp.Format(_T("Content-Type:application/json\r\nAccept: */*\r\nUser-Agent:  Mozilla/4.0 (compatible; ")
		_T("MSIE 9.00; System %u.%u.%u; Fyter Web Requester)\r\n"),
		LOBYTE(LOWORD(dwVersion)), HIBYTE(LOWORD(dwVersion)), HIWORD(dwVersion));

	CHttpConnection *pServer = NULL;

	CHttpFile* pFile = NULL;
	/*   try*/
	CString strServerName;
	CString strObject;
	INTERNET_PORT nPort;
	DWORD dwServiceType;

	if (!AfxParseURL(url, dwServiceType, strServerName, strObject, nPort) ||
		(dwServiceType != AFX_INET_SERVICE_HTTPS  && dwServiceType != AFX_INET_SERVICE_HTTP))
	{
		return FALSE;
	}

	ULONGLONG uReadLen = 0;
	// 	CString szRequestHeader;// HTTP 头
	HTTPREQUESTRESULT result = RES_REQ_ERROR_HTTP;
	// 
	try
	{
		pServer = session.GetHttpConnection(strServerName, INTERNET_FLAG_RELOAD, nPort/*,_T("administrator"),_T("8g6f2skum")*/);

		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, NULL, (DWORD)1, NULL, NULL, INTERNET_FLAG_RELOAD);
		if (pFile)
		{
			pFile->AddRequestHeaders(strTemp);
			pFile->SendRequest();
			DWORD status = 0;
			pFile->QueryInfoStatusCode(status);
			if (status >= 200 && status <= 299)
			{
				DWORD dwWritten;
				UINT  nRead = 0;
				HANDLE  hFile = CreateFile(lpszSaveName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				char sz[10 * 1024];
				ULONGLONG nlFileSize = pFile->GetLength();

				DWORD nLen = 0;
				pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, nLen);

				if (nlFileSize<nLen)
				{
					nlFileSize = nLen;
				}

				if (nLoadedPart>0)
				{
					myFileSeek(hFile, nLoadedPart, FILE_BEGIN);
					pFile->Seek(nLoadedPart, CFile::begin);
					nlFileSize = nlFileSize>nLoadedPart ? nlFileSize - nLoadedPart : 0;
				}

				ULARGE_INTEGER   FreeAv, TotalBytes, FreeBytes;
				FreeBytes.QuadPart = 0;
				GetDiskFreeSpaceEx(_T("c:\\"), &FreeAv, &TotalBytes, &FreeBytes);

				//WriteLog(_T("BEGAIN"));
				if (nlFileSize<FreeBytes.QuadPart)
				{
					memset(sz, 0, sizeof(sz));
					nRead = pFile->Read(sz, 10 * 1024);
					while ((nRead>0 || nlFileSize>0) && !theApp.m_bQuestClose)
					{
						if (theApp.m_bRequestClose)
						{
							break;
						}
						else if (nRead>0)
						{
							WriteFile(hFile, sz, nRead, &dwWritten, NULL);
							nLoadedPart += nRead;
							uReadLen += nRead;
							//	CString strLog;
							// 						LONGLONG llTemp=(LONGLONG)powl(10,9);
							// 
							// 						if (ULONG(nlFileSize/llTemp)>0)
							// 						{
							// 							strLog.Format(_T("%u%09u"),ULONG(nlFileSize/llTemp),ULONG(nlFileSize%llTemp)/*断点续传的开始值*/);
							// 						}
							// 						else
							// 						{
							//		strLog.Format(_T("%d"),nRead/*断点续传的开始值*/);
							/*						}*/


						}
						else if (nRead <= 0)
						{
							if (pFile != NULL)
							{
								pFile->Close();
								delete pFile;
								pFile = NULL;
							}
							if (pServer != NULL)
							{
								pServer->Close();
								delete pServer;
								pServer = NULL;
							}
							CloseHandle(hFile);
							goto REDIRECT1_;
						}
						nlFileSize = nlFileSize>nRead ? nlFileSize - nRead : 0;
						memset(sz, 0, sizeof(sz));
						nRead = pFile->Read(sz, 10 * 1024);

					}

					if (nlFileSize <= 0)
					{
						result = RES_REQ_DONE_OK;
						nLoadedPart = 0;
					}
				}

				else
				{
					m_strSuspendUrl = url;
					result = RES_REQ_ERROR_NOSPACE;
				}

				if (pFile != NULL)
				{
					pFile->Close();
					delete pFile;
					pFile = NULL;
				}
				if (pServer != NULL)
				{
					pServer->Close();
					delete pServer;
					pServer = NULL;
				}
				CloseHandle(hFile);
			}

		}

		if (pFile != NULL)
		{
			pFile->Close();
			delete pFile;
			pFile = NULL;
		}
		if (pServer != NULL)
		{
			pServer->Close();
			delete pServer;
			pServer = NULL;
		}
		session.Close();
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet

		DWORD Eerror = pEx->m_dwError;
		pEx->Delete();

		if (pFile != NULL)
		{
			pFile->Close();
			delete pFile;
			pFile = NULL;
		}
		if (pServer != NULL)
		{
			pServer->Close();
			delete pServer;
			pServer = NULL;
		}

		if (ERROR_INTERNET_CONNECTION_RESET == Eerror)
		{

			goto REDIRECT1_;
		}

		if (uReadLen>0)
		{

			m_strSuspendUrl = url;
			result = RES_REQ_ERROR_GET;
		}


		session.Close();
	}
	catch (...)
	{
	}
	return result;

}


//CString CMonitorApp::GetFileMd5(CString strFilePath)
//{
//	CFile file;
//	if (file.Open(strFilePath, CFile::modeRead) == 0)
//		return(_T(""));
//
//	MD5_CTX contex;
//	CmyMD5 md5;
//	md5.MD5Init(&contex);
//	int nLength = 0;       //number of bytes read from the file  
//	const int nBufferSize = 1024; //checksum the file in blocks of 1024 bytes  
//	BYTE Buffer[nBufferSize];   //buffer for data read from the file  
//
//	//checksum the file in blocks of 1024 bytes  
//	while ((nLength = file.Read(Buffer, nBufferSize)) > 0)
//	{
//		md5.MD5UpdateByte(&contex, Buffer, nLength);
//	}
//	file.Close();
//	md5.MD5FinalByte(&contex);
//	return CString(md5.MD5TransByte(contex)).MakeLower();
//
//}

CString CMonitorApp::GetFileMd5(CString strFilePath)
{
	FILE * fp;
	if (!PathFileExists(strFilePath))
	{
		return false;
	}
	_bstr_t bstrMac = strFilePath;
	char* pFilePath = bstrMac;

	if ((fp = fopen(pFilePath, "r")) == NULL){
		return false;
	}

	MD5_CTX contex;
	CmyMD5 md5;
	md5.MD5Init(&contex);
	int nLength = 0;       //number of bytes read from the file  
	const int nBufferSize = 1024; //checksum the file in blocks of 1024 bytes  
	BYTE Buffer[nBufferSize];   //buffer for data read from the file  

	//checksum the file in blocks of 1024 bytes  
	while ((nLength = fread(Buffer, 1, nBufferSize, fp)) >0)
	{
		md5.MD5UpdateByte(&contex, Buffer, nLength);

		memset(Buffer, 0, sizeof(Buffer));
	}
	fclose(fp);

	md5.MD5FinalByte(&contex);

	return CString(md5.MD5TransByte(contex)).MakeLower();
}


BOOL CMonitorApp::ProssExist(CString strProssName)
{
	DWORD id = 0;
	HANDLE   SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot == NULL)
	{
		AfxMessageBox(_T("检测当前进程失败! "));
		return FALSE;
	}
	SHFILEINFO   shSmall;
	CString strProName;
	PROCESSENTRY32   ProcessInfo;//声明进程信息变量 
	ProcessInfo.dwSize = sizeof(ProcessInfo);//设置ProcessInfo的大小 
	//返回系统中第一个进程的信息 
	BOOL   Status = Process32First(SnapShot, &ProcessInfo);
	int   m_nProcess = 0, num = 0;
	while (Status)
	{
		m_nProcess++;
		num++;
		//获取进程文件信息 
		SHGetFileInfo(ProcessInfo.szExeFile, 0, &shSmall,
			sizeof(shSmall), SHGFI_ICON | SHGFI_SMALLICON);
		//在列表控件中添加映像名称 
		strProName = ProcessInfo.szExeFile;
		if (strProName == strProssName)
		{
			id = ProcessInfo.th32ProcessID;
			CloseHandle(SnapShot);
			return TRUE;
		}
		//获取下一个进程的信息 
		Status = Process32Next(SnapShot, &ProcessInfo);
	}
	CloseHandle(SnapShot);
	return FALSE;
}


DWORD CMonitorApp::DetectProgectExist(CString strCheckProName)
{
	DWORD id = 0;
	HANDLE   SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot == NULL)
	{
		AfxMessageBox(_T("检测当前进程失败! "));
		return 0;
	}
	SHFILEINFO   shSmall;
	CString strProName;
	PROCESSENTRY32   ProcessInfo;//声明进程信息变量 
	ProcessInfo.dwSize = sizeof(ProcessInfo);//设置ProcessInfo的大小 
	//返回系统中第一个进程的信息 
	BOOL   Status = Process32First(SnapShot, &ProcessInfo);
	int   m_nProcess = 0, num = 0;

	int iCount = 0;
	while (Status)
	{
		m_nProcess++;
		num++;
		//获取进程文件信息 
		SHGetFileInfo(ProcessInfo.szExeFile, 0, &shSmall,
			sizeof(shSmall), SHGFI_DISPLAYNAME/*SHGFI_ICON|SHGFI_SMALLICON*/);
		//在列表控件中添加映像名称 
		strProName = ProcessInfo.szExeFile;
		id = ProcessInfo.th32ProcessID;
		if (strProName == strCheckProName)
		{
			//  			CString str=_T("GrabPaper_C");
			//  			CWnd *pWnd=CWnd::FindWindow(NULL,str);
			// 			if (pWnd!=NULL)
			// 			{
			// 
			// 				DWORD_PTR dwResult = 0;  
			// 				LRESULT lr = ::SendMessageTimeout(pWnd->m_hWnd, WM_LBUTTONDOWN, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK,2000, &dwResult);  
			// 				if (lr)  
			// 				{  
			// 					if (iCount==0)
			// 					{
			// 						iCount++;
			// 						Status=Process32Next(SnapShot,&ProcessInfo); 
			// 						continue;
			// 					}
			// 					// 还可以响应 
			// 					id=ProcessInfo.th32ProcessID;
			// 					CloseHandle(SnapShot); 
			// 					return id;
			// 				}  
			// 				else  
			// 				{  
			// 					// 已经停止响应了（俗话说的窗口挂死了）  
			// 
			// 					//通过id关闭进程
			// 					CloseThread(id);
			// 					CloseHandle(SnapShot); 
			// 					return 0;
			// 				}  
			// 
			// 			}


			// 			if (IsHungAppWindow(pWnd->m_hWnd))
			// 			{
			// 				DWORD id=ProcessInfo.th32ProcessID;
			// 				//通过id关闭进程
			// 				CloseThread(id);
			// 				CloseHandle(SnapShot); 
			// 				return 0;
			// 			}
			CloseHandle(SnapShot);
			return id;
		}
		//获取下一个进程的信息 
		Status = Process32Next(SnapShot, &ProcessInfo);
	}
	CloseHandle(SnapShot);
	return 0;
}



typedef void (CALLBACK* LPFNSETDLLDIRECTORY)(LPCTSTR);
void CMonitorApp::DelayLoadReaderExDLL()
{
	TCHAR path[MAX_PATH];
	TCHAR dllPathName[MAX_PATH];
	//CString strPath;
	//CRegistry reg;
	//reg.SetRootKey(HKEY_LOCAL_MACHINE);
	//if (reg.SetKey(_T("Software\\TTOD\\CAJViewer"),FALSE))
	//{
	//	 strPath = reg.ReadString(_T("TargetDir"), _T(""));
	//	 PathRemoveBackslash(strPath.GetBuffer(MAX_PATH));
	//	 strPath.ReleaseBuffer();
	//}
	//if (strPath.CompareNoCase(m_strAppDir) == 0)
	//{

	if (SHGetSpecialFolderPath(NULL, path, CSIDL_PROGRAM_FILES_COMMON, FALSE))
	{
		PathAppend(path, _T("TTKN\\Bin"));
	}
	else
		throw;
	//strDLLPath = path;
	//}
	//else
	//{
	//	lstrcpy(path, m_strAppDir);
	//}
	lstrcpy(dllPathName, path);

	//寻找SetDllDirectory入口
	LPFNSETDLLDIRECTORY MySetDllDirectory = NULL;
	HMODULE hmod = GetModuleHandle(_T("kernel32.dll"));
	BOOL bSetDllDirectory = FALSE;
	if (hmod != NULL)
	{
#ifndef UNICODE
		MySetDllDirectory = (LPFNSETDLLDIRECTORY)GetProcAddress(hmod, _T("SetDllDirectoryA"));
#else
		MySetDllDirectory = (LPFNSETDLLDIRECTORY)GetProcAddress(hmod, "SetDllDirectoryW");
#endif

		if (MySetDllDirectory != NULL)
		{
			MySetDllDirectory(dllPathName);
			bSetDllDirectory = TRUE;
		}

	}
	if (!bSetDllDirectory)
		::SetCurrentDirectory(dllPathName);
#ifdef _DEBUG
	PathAppend(dllPathName, _T("ReaderEx_d.dll"));
#else
	PathAppend(dllPathName, _T("ReaderEx.dll"));
#endif
	LoadLibrary(dllPathName);
	/*DWORD dw = GetLastError();
	CString strErrorCode;
	strErrorCode.Format(_T("%d"),dw);
	AfxMessageBox(strErrorCode);*/
	lstrcpy(dllPathName, path);
	PathAppend(dllPathName, _T("sysinfo.dll"));
	LoadLibrary(dllPathName);
}