
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

//���庯��ָ������,��Ҫ���õ�DLL�е�������һ��


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
	UINT nRequestTimeGap;//�����϶
	UINT nThreadNo;	// �߳�ID
};

struct UPDATEINFOITEM
{
	CString strKindId;
	CString strDownloadUrl;
	CString strContent;				//post ����
	CString strLastUpdateStander;	// ���һ�εĸ�����Ϣ
	UINT nUpdateKind;				// ָ��UpdateStander�Ĳ������ͣ�1.���������ڣ�2.THName(�ڿ�����)
	UINT nRequestKind;				// ����ģ������
	UINT nRequestSubKind;			// ����ģ��������
	UINT nPriority;					// ģ������ȼ�
	UINT nOrderNo;					// ģ�����ţ����ڸ���content����ʱʹ��
	BOOL isNeedReadFromTable;
	CString tableInfo;				// ����ȷ��ģ���������ı�λ��
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
	UINT m_nGetInfoVersion;//0��ʾ�� kbase�õ������Ϣ  1��ʾ����
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

	CString m_strProgramVertion;//�汾����ʱŪһ����ţ�ǰ��λ��11 ��ͼ ��12���� ��13������14���Ρ�15���¡�16��ͨ��17ũҵ   ����λ��1��� ��2����   ������λ��01 ��ʾ��׼�桭���������������Լ����壩
	CStringArray m_strMacArray;
	CString m_strSerialNo;
	CString m_strRegestNo;
	CString m_strBuyStartYear;
	CString m_strBuyEndYear;

	CString m_strStartYearMonthDay;

	ULONG m_nlZzhcDate;

	//HANDLE m_hMutexOperatorKbas;

//	CString m_strDvdPath;
	CString m_strGUIDOLDUnInstal;//��װ����Ψһ��ʾ

	UINT m_nConnectNo;

	BOOL m_bQuestClose;

	EnDecryption m_DecTemp;


	CArray<UPDATETHREADINFO, UPDATETHREADINFO> m_updateThreadArray;

	CArray<UPDATEINFOITEM, UPDATEINFOITEM> m_updateinfoArray;

	CArray<COMMONITEM, COMMONITEM> totalItem;	// �洢���ṹ��ص���Ϣ

	BOOL GetStrFromWeb(CString strUrl,CString& strReturn,CString strHeader=_T(""));
	BOOL SendStrFromWeb(CString strUrl, CString& strReturn, const char* content, const char *token = "");
	BOOL GetInitParamFromXML(CString strXmlPath);


	__int64 myFileSeek(HANDLE hf, __int64 distance, DWORD MoveMethod);
	CString m_strSuspendUrl;
	int RequestHttp(LPCTSTR url, LPCTSTR lpszSaveName, ULONGLONG& nLoadedPart);

	//nKind 1 �������ݴ���׺   2�������ݲ�����׺
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

	DWORD DetectProgectExist(CString strCheckProName);//�����򷵻ؽ���id�����ڷ���0��


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