
// MonitorDlg.h : header file
//
#pragma once



#define TIME_MONITER 100
#define CLOS_WINDOW_TIMER 110
#define CLOS__WERFAULT_TIMER 111
#define DOWNLUPDATE_DATA_TIMER 120
/*#define UPDATE_DATA_TIMER 120*/

#define DOUPDATE_DATA_TIMER 121

#define DOCLEAR_STALE_DATA 122

#define DOCLEAR_INVALIDE_TABLEDATA 123
//#define _WIN32_WINNT 0x0400
#include <tchar.h>
#include <wincrypt.h>

// CMonitorDlg dialog
class CMonitorDlg : public CDialog
{
// Construction
public:
	CMonitorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	ULONGLONG m_nLLoadPart;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:



	struct  OPERATOR1PARTITEMSTRUCT
	{

		OPERATOR1PARTITEMSTRUCT()
		{
			strArraySubId.RemoveAll();
			strArraySubCname.RemoveAll();
			strArraySubHttp.RemoveAll();
			//	nArraySubItemOperatorKind.RemoveAll();
		}
		~OPERATOR1PARTITEMSTRUCT()  //�������� �����ڴ�
		{
			strArraySubId.RemoveAll();
			strArraySubCname.RemoveAll();
			strArraySubHttp.RemoveAll();
			//	nArraySubItemOperatorKind.RemoveAll();
		}

		OPERATOR1PARTITEMSTRUCT(const OPERATOR1PARTITEMSTRUCT & other)
		{
			strID = other.strID;
			strCName = other.strCName;
			nKind = other.nKind;
			strUpdateTime = other.strUpdateTime;
			strUrl = other.strUrl;
			strHeader = other.strHeader;
			strStartIdentification = other.strStartIdentification;
			strEndIdentification = other.strEndIdentification;
			strStartRecordIdentification = other.strStartRecordIdentification;
			strEndRecordIdentification = other.strEndRecordIdentification;
			nTextAndHttpDirection = other.nTextAndHttpDirection;
			strTextStartIdentification = other.strTextStartIdentification;
			strTextEndIdentification = other.strTextEndIdentification;
			strHttpStartIdentification = other.strHttpStartIdentification;
			strHttpEndIdentification = other.strHttpEndIdentification;

			strArraySubId.Copy(other.strArraySubId);
			strArraySubCname.Copy(other.strArraySubCname);
			strArraySubHttp.Copy(other.strArraySubHttp);

			//nArraySubItemOperatorKind.Copy(other.nArraySubItemOperatorKind);

		}
		OPERATOR1PARTITEMSTRUCT & operator = (OPERATOR1PARTITEMSTRUCT &other)
		{

			strID = other.strID;
			strCName = other.strCName;
			nKind = other.nKind;
			strUpdateTime = other.strUpdateTime;
			strUrl = other.strUrl;
			strHeader = other.strHeader;

			strStartIdentification = other.strStartIdentification;
			strEndIdentification = other.strEndIdentification;
			strStartRecordIdentification = other.strStartRecordIdentification;
			strEndRecordIdentification = other.strEndRecordIdentification;
			nTextAndHttpDirection = other.nTextAndHttpDirection;
			strTextStartIdentification = other.strTextStartIdentification;
			strTextEndIdentification = other.strTextEndIdentification;
			strHttpStartIdentification = other.strHttpStartIdentification;
			strHttpEndIdentification = other.strHttpEndIdentification;

			strArraySubCname.Copy(other.strArraySubCname);
			strArraySubId.Copy(other.strArraySubId);
			strArraySubHttp.Copy(other.strArraySubHttp);

			//nArraySubItemOperatorKind.Copy(other.nArraySubItemOperatorKind);
			return *this;
		}
		CString strID;
		CString strCName;
		UINT nKind;//0 �Դ� 1�ӿ�
		CString strUpdateTime;
		CString strUrl;
		CString strHeader;

		CStringArray strArraySubId;
		CStringArray strArraySubCname;
		CStringArray strArraySubHttp;
		//	CStringArray strArraySubUpdateDate;
		//CArray<UINT> nArraySubItemOperatorKind; //0 ���� 1 �޸� 2 ɾ�� 3����

		CString strStartIdentification;//��ʼ��ʶ
		CString strEndIdentification;//������ʶ
		CString strStartRecordIdentification;//��¼��ʼ��ʶ
		CString strEndRecordIdentification;//��¼��ֹ��ʶ
		UINT nTextAndHttpDirection;//�����볬���ӵ�λ�ù�ϵ  0 text��ǰ   1  http��ǰ
		CString strTextStartIdentification;//��ʼ��ʶ
		CString strTextEndIdentification;//������ʶ
		CString strHttpStartIdentification;//��ʼ��ʶ
		CString strHttpEndIdentification;//������ʶ

	};

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//BOOL GetFileFromServer(CString strUrl,CString strDirPath);



	BOOL GetFmPath(CString strKindId,CString& strfmPath);
	BOOL GetDataPath(CString strKindId, CString& strDataPath);

	BOOL IsNeedUpdateData();


	BOOL IsNeedUpdateData_OneDayDowload();
	BOOL DoUpdateData();


	BOOL GetUpdateResultFromIni(CString& strUpdateDate);
	BOOL GetUpdateResultFromIni();
	void CheckAndDelPaper(CString strDelPaperPath);
	void CheckAndDelInvalidatePicVideo(UINT nKind);
	void CheckAndDelInvalidateWebPage();
	void GetDataAndFmPath(CString strKind,CString& strDataPath,CString& strFmPath);

    HWND GetWindowHandleByPID(DWORD dwProcessID);
	void HideTaskBar(BOOL bHide); 
	void GetDvdPath();
	void GetPaperCnameArray();
	void OpenLoadPaperPro();

	void OpenUpdateDataPro(CString strParam);

	void OpenCajTouchViewPro();

	//BOOL FindAndUpdateData();

	BOOL UpdateData(CString strParam,CString& strUpdateDate);
	BOOL UpdateData(CString strParam);

	BOOL UpdateUpdateTable(LONG LID,UINT nState,LONGLONG lPart);


	//BOOL UpdateFirstLevlTable();

	//UINT GetUpdateState(ULONG nlYearMonth,CString& strDownedPart);//0û��ʼ����  1 ������һ���� 2 ������ϣ�δ���£� 3 �������

	//BOOL GetNeedUpdateYearMonthArray(CStringArray& strArray);

	//BOOL ClearUnusableData();

	//BOOL IsHaveUpdateData(ULONG nlYearMonth);//�Ƿ���������Ѿ��ϴ���������



	BOOL IsNeedUpdateApplication(CString& strUpdateAppPath);
	void UpdateApp(CString strUpdateAppPath);

	BOOL ClearInvalidedData();

	static DWORD WINAPI ThreadApplicationUpdatePackage(LPVOID lpParame);

	
	static DWORD WINAPI ThreadGetUpdateInfoAndData_XWDT(LPVOID lpParameter);
	static DWORD WINAPI ThreadGetUpdateInfoAndData_Others(LPVOID lpParameter);
	static DWORD WINAPI ThreadGetUpdateInfoAndData_QK_TS(LPVOID lpParameter);
DWORD WINAPI CMonitorDlg::testLock(LPVOID lpParameter);


	//static DWORD WINAPI ThreadGetUpdateInfoAndData(LPVOID lpParame);
	static DWORD WINAPI ThreadGetUpdateInfoAndDataEx(LPVOID lpParame);

	static DWORD WINAPI ThreadGetUpdateInfoAndDataEx_ZJZY(LPVOID lpParame);

	static DWORD WINAPI ThreadGetUpdateInfoAndDataEx_Personset(LPVOID lpParame);

	static DWORD WINAPI ThreadGetUpdateInfoAndDataEx_ShfwSet(LPVOID lpParame);

	static DWORD WINAPI ThreadClearInvalidateData(LPVOID lpParame);

	static DWORD WINAPI ThreadGetWeather(LPVOID lpParame);

	static DWORD WINAPI TheradGetNewData(LPVOID lpParameter);

	static DWORD WINAPI ThreadLoadUserHtmlDate(LPVOID lpParameter);

	static DWORD WINAPI ThreadPostUserUsedInfo(LPVOID lpParameter);
	static DWORD WINAPI ThreadGetUpdateInfoAndData_TreadNo(LPVOID lpParameter);

	void Get1PartOneKindData(OPERATOR1PARTITEMSTRUCT& structitem);

	BOOL IsExist(CString strId);

	BOOL ItemIsInTable(CString strTableName, CString strFindConditions);

	//void DowloadData(CString strKindId,CString strKindCname,CString strShowKind,CStringArray& strArrayRes,CStringArray& strArrayAdd);
	bool Download(const CString& strUrl, const CString & strSavePath);

	static DWORD WINAPI ThreadUpdateZZHC(LPVOID lpParame);


	HANDLE m_hThreadGetUpdataInfoData;
	HANDLE m_hThreadGetUpdataInfoData_ZJZY;
	HANDLE m_hThreadGetUpdataInfoData_PersonSet;
	HANDLE m_hThreadGetUpdataInfoData_ShfwSet;
	HANDLE m_hThreadUpdateData;
	HANDLE m_hGetAppUpdateInfoData;
	HANDLE m_hClearInvalidateData;
	HANDLE m_hTheradGetNewData;
	HANDLE m_hGetWeather;
	void TestDb();
	DWORD GetHash(CString strRes, DWORD dwDataLen, ALG_ID algId, CString& strHash);
	BOOL SecrectData(CString strRescPath, CString strDirPath);

	afx_msg void OnClose();
	afx_msg void CloseThread();



	//CString m_strFmPath;
	//CString m_strDataPath;
	BOOL m_bGetUpdateFromServer;
	CStringArray m_strArrayCname;
	CStringArray m_strArrayNeedUpdateName;


	HANDLE m_hThreadLoad[10];
};

// function prototype
BOOL tableExist(CString strTableName);
BOOL columnIsInTable(CString strColumn, CString strTable, BOOL& bExist);

BOOL getNewToken(char *buf);
CString getUploadMSGContent(UPDATEINFOITEM &updateIfItem);
CString getCZRZContent(UPDATEINFOITEM &updateIfItem);

void CString2Char(CString str, char ch[]);
CString string2CString(string stringres);

BOOL dealwithXWDT(Json::Value &itemArr, UPDATEINFOITEM &updateIfItem);
BOOL dealwithOthers(Json::Value &itemArr, UPDATEINFOITEM &updateIfItem);
int dealwithQK_TS(Json::Value &itemArr, UPDATEINFOITEM &updateIfItem, BOOL &firstPageFlag);

// add some year, month or days into CString date
CString &addTime(CString &date, int year, int month, int day);
unsigned long string2Ctime(const char *str);

