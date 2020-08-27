
// Monitor.h : main header file for the PROJECT_NAME application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "EnDecryption.h"       

#include <windows.h>
#include <string>
#import <msxml3.dll> 
#include"DbSqlite.h"

#define nNumsEachTime 50
using namespace std;

/* log infomation*/
//CTime tmNow;
//CString m_strDateTime;


// CMonitorApp:
// See Monitor.cpp for the implementation of this class
//

//定义函数指针类型,和要调用的DLL中导出函数一致


#define _DEBUG_

extern SYSTEMTIME st;
extern CString logInfo; 
void WriteLogTHMain(CString strWrite, int number);


#ifdef _DEBUG_

#define W			WriteLogTHMain(_T("W "), __LINE__);
#define EW			WriteLogTHMain(_T("EW"), __LINE__);

#define R			WriteLogTHMain(_T("R "), __LINE__);
#define ER			WriteLogTHMain(_T("ER"), __LINE__);


//Thread 0
#define W0			WriteLogTH0(_T("W0 "), __LINE__);
#define EW0			WriteLogTH0(_T("EW0"), __LINE__);

#define R0			WriteLogTH0(_T("R0 "), __LINE__);
#define ER0			WriteLogTH0(_T("ER0"), __LINE__);


//Thread 1
#define W1			WriteLogTH1(_T("W1 "), __LINE__);
#define EW1			WriteLogTH1(_T("EW1"), __LINE__);

#define R1			WriteLogTH1(_T("R1 "), __LINE__);
#define ER1			WriteLogTH1(_T("ER1"), __LINE__);

//Thread 2
#define W2			WriteLogTH2(_T("W2 "), __LINE__);
#define EW2			WriteLogTH2(_T("EW2"), __LINE__);

#define R2			WriteLogTH2(_T("R2 "), __LINE__);
#define ER2			WriteLogTH2(_T("ER2"), __LINE__);

#else

#define W 
#define EW
#define R
#define ER


#define W0 
#define EW0
#define R0
#define ER0

#define W1 
#define EW1
#define R1
#define ER1

#define W2
#define EW2
#define R2
#define ER2
#endif

struct UPDATETHREADINFO
{
	UINT nStartNo;
	UINT nEndNo;
	UINT nRequestTimeGap;//请求间隙
	UINT nThreadNo;	// 线程ID
};

struct UPDATEINFOITEM
{
	CString strKindId;
	CString strDownloadUrl;
	CString strContent;				//post 参数
	CString strLastUpdateStander;	// 最近一次的更新信息
	UINT nUpdateKind;				// 指明UpdateStander的参数类型，1.最后更新日期，2.THName(期刊更新)
	UINT nRequestKind;				// 请求模块种类
	UINT nRequestSubKind;			// 请求模块子种类
	UINT nPriority;					// 模块的优先级
	UINT nOrderNo;					// 模块的序号，用于更新content参数时使用
	BOOL isNeedReadFromTable;
	CString tableInfo;				// 用于确定模块所操作的表位置
};

struct COMMONITEM
{
	CString GGParentID;
	CString GGGParentID;
	CString FmKind;
	CString FmBkPath;
	CString FmPath;
	CString ShowInLevel;
	CString OpenItemKind;
	CString OpenItemShowKind;
	CString DirGetKind;
	CString DirInTable;
	CString DirShowKind;
	CString MoreGetKind;
	CString MoreShowKind;
	CString DataGetKind;
	CString DataKind;
	CString DataPath;
	CString SubItemExist;
	CString SubItemInfoTable;
	CString SubItemIsShowInList;
	UINT nSubItemShowKind;
	CString SubItemListLevel;
	CString SubItemListPageChangeKind;
	CString ShowEwmKind;
	UINT OrderNo;
	CString IsCanFind;
	UINT IsCanLarge;
	CString InfoKind;
	CString appendkind;
	CString Add_InfoKind;
	CString ZanExist;
	UINT BeTemplate;
	CString NextWinowShowInRect;
	UINT BeShowInHomePage;
	UINT BeValid;
	CString append1kind;
};


class CMonitorApp : public CWinAppEx
{
public:
	CMonitorApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	BOOL FindPrevInstance();
	BOOL CloseThread(DWORD id);
	BOOL ClosePross(CString strProssName);
	BOOL ProssExist(CString strProssName);
	int m_iUpdateFinishPercent;
// Implementation

	DECLARE_MESSAGE_MAP()

public:
	CString m_strCanChangePath;
	CString m_strExePath;
	CString m_strKbaseId;
	CString m_strKbaseUserName;
	CString m_strKbasePort;
	CString m_strKbasePassWord;

	CString m_strCloseTime;
	CString m_strStartYearMonth;
	CString m_strUserWebAdrr;
	CString m_strCnkiServerId;
	CString m_strCnkiServerPath;
	CString m_strSqliteDataPath;
	CString m_strKindEname;
	UINT m_nGetInfoVersion;//0表示从 kbase得到相关信息  1表示其他
	BOOL m_bAllowClose;
	UINT m_nPaperSaveDayLength;
	CString m_strTitleName;

	//CString m_strDataPath;

	CString m_strTotalDataPath;
	CString m_strTotalFmPath;

	CDbSQLite m_sqlite;

	CString m_strAppDir;
	CString m_strServerId;


	//CString m_strDictPath;
	CString m_strDvdPath;

	BOOL m_bRequestClose;


	UINT m_nStaleDate;

	CString m_strProgramVertion;//版本号临时弄一个编号：前两位：11 公图 、12法律 、13党政、14旅游、15军事、16交通、17农业   第三位：1横版 、2竖版   第四五位：01 表示标准版……（其他各部门自己定义）
	CStringArray m_strMacArray;
	CString m_strSerialNo;
	CString m_strRegestNo;
	CString m_strBuyStartYear;
	CString m_strBuyEndYear;

	CString m_strStartYearMonthDay;

	ULONG m_nlZzhcDate;

	//HANDLE m_hMutexOperatorKbas;

//	CString m_strDvdPath;
	CString m_strGUIDOLDUnInstal;//安装包的唯一标示

	UINT m_nConnectNo;

	BOOL m_bQuestClose;

	EnDecryption m_DecTemp;


	CArray<UPDATETHREADINFO, UPDATETHREADINFO> m_updateThreadArray;

	CArray<UPDATEINFOITEM, UPDATEINFOITEM> m_updateinfoArray;

	CArray<COMMONITEM, COMMONITEM> totalItem;	// 存储与表结构相关的信息

	BOOL GetStrFromWeb(CString strUrl,CString& strReturn,CString strHeader=_T(""));
	BOOL SendStrFromWeb(CString strUrl, CString& strReturn, const char* content, const char *token = "");
	BOOL GetInitParamFromXML(CString strXmlPath);


	__int64 myFileSeek(HANDLE hf, __int64 distance, DWORD MoveMethod);
	CString m_strSuspendUrl;
	int RequestHttp(LPCTSTR url, LPCTSTR lpszSaveName, ULONGLONG& nLoadedPart);

	//nKind 1 下载内容带后缀   2下载内容不带后缀
	BOOL DownLoadData(CString strDownLoadUrl, CString& strSavePath, UINT nKind);
	BOOL DownLoadDataNOIDC(CString strDownLoadUrl, CString& strSavePath, UINT nKind);	
	BOOL ReadxmlUpdateDate();
	BOOL UpdatexmlUpdateDate();
	BOOL UpdateXmlAboutKbase();
	BOOL UpdateXmlAboutCloseData(BOOL bAllowClose,CString strSetCloseTime);
	BOOL ReadCloseTime();
	BOOL ReadServerId();
	BOOL ReadVersion();
	BOOL ReadStartDate();
	BOOL ReadCnkiServerId();
	BOOL ReadDataPathAndStaleDate();
	BOOL ReadSerialNo();
	BOOL ReadTotalSavePath();

	BOOL GetMacArray();
	BOOL IsRegest();
	BOOL GetSerialNoFromServer(CString strBaseUrl);

	BOOL GetSerialNoFromServer(CString strBaseUrl,CString& strGetSerialNo);

	void GetBuyYear();

	void CorrectBuyYear(ULONG& nLStartYM, ULONG& nLEndYM);

	
	void CorrectBuyYearEx(ULONG& nLStartYMD, ULONG& nLEndYMD);

	CString GetFileMd5(CString strFilePath);

	DWORD DetectProgectExist(CString strCheckProName);//存在则返回进程id不存在返回0；


	void DelayLoadReaderExDLL();


    bool LockRead(long lTimeOut, long lTimeOut1);
	bool UnLockRead();
	bool LockWrite(long lTimeOut, long lTimeOut1);
	bool UnLockWrite();
};

//extern CMonitorApp theApp;
extern string token;
extern CString logInfo;
extern CMonitorApp theApp;

BOOL getTemplateFromTable();
BOOL getInfoFromUpdateInfo();