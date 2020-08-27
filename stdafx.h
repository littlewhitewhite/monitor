
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include "../include/readerexapi.h"


#ifndef _DEBUG
#pragma comment(lib,"..\\lib\\readerex.lib")
#pragma comment(lib,"..\\lib\\sqlite3.lib") 
#else
#pragma comment(lib,"..\\lib\\readerex_d.lib")
#pragma comment(lib,"..\\lib\\sqlite3_d.lib") 
#endif




#include "../include/json/json.h"
#ifdef _DEBUG
#pragma comment(lib, "..\\lib\\lib_jsond_mtd.lib")
#else
#pragma comment(lib, "..\\lib\\lib_json.lib")
#endif

#pragma comment(lib,"../Release/CTVDatabase.lib") 
extern "C"   bool  IsOpen();
extern "C"   bool  Open(BSTR  bstrUsrName, BSTR bstrPassWord, BSTR bstrUsrIP, int nPort);//外部调用
extern "C"   bool  Close();
extern "C"   bool  OpenRecordSet(BSTR selectText);//查询数据
extern "C"   int   GetRecordCount();
extern "C"   bool  CloseRecordSet();
extern "C"   BSTR  GetFieldValue(int nNo);
extern "C"   bool  SetFieldValue(BSTR szSelectText, int nNo);
extern "C"   bool  MoveNext();
extern "C"   bool  MovePrev();
extern "C"   bool  MoveFirst();
extern "C"   bool  MoveLast();
extern "C"   bool  Edit();
extern "C"   bool   Update();

extern "C"   bool  DeleteSel(BSTR selectText);
extern "C"   bool  InsertValue(CString strInsertSql);

extern "C"   bool OpenEx(UINT& nConnectNo, BSTR  bstrUsrName/*=::SysAllocString(L"DBOWN")*/, BSTR bstrPassWord/*=::SysAllocString(L"")*/, BSTR bstrUsrIP/*=::SysAllocString(L"192.168.20.129")*/, BSTR bstrDbName, int nPort/*=4567*/);//外部调用
extern "C"   bool  CloseEx(UINT nConnectNo);
extern "C"   bool  IsOpenEx(UINT nConnectNo);
extern "C"   bool  OpenRecordSetEx(UINT nConnectNo, UINT& nRecordNo, BSTR selectText);//查询数据
extern "C"   bool CloseRecordSetEx(UINT nConnectNo, UINT nRecordNo);
extern "C"   BSTR  GetFieldValueEx(UINT nConnectNo, UINT nRecordNo, int nNo);
extern "C"   bool  EditEx(UINT nConnectNo, UINT nRecordNo);

extern "C"   bool  SetFieldValueEx(UINT nConnectNo, UINT nRecordNo, BSTR szSelectText, int nNo);
extern "C"   bool  MoveNextEx(UINT nConnectNo, UINT nRecordNo);
extern "C"   bool  MovePrevEx(UINT nConnectNo, UINT nRecordNo);
extern "C"   bool  MoveFirstEx(UINT nConnectNo, UINT nRecordNo);
extern "C"   bool  MoveLastEx(UINT nConnectNo, UINT nRecordNo);
extern "C"   bool MoveEx(UINT nConnectNo, UINT nRecordNo, long lNo);

extern "C"   int   GetDataLenEx(UINT nConnectNo, UINT nRecordNo, int nNo);
extern "C"   int   GetBLobEx(UINT nConnectNo, UINT nRecordNo, int nNo, int offset, char *  buf, int bufLen);
extern "C"   int   GetRecordCountEx(UINT nConnectNo, UINT nRecordNo);
//extern "C"   bool   Deletes(UINT nDbKind,LPVOID pResult,int nDelCount,BSTR szDels);
extern "C"   bool   DeleteEx(UINT nConnectNo, UINT nRecordNo);
extern "C"   bool   DeleteSelEx(UINT nConnectNo, BSTR selectText);
extern "C"   bool   DeleteFilesEx(UINT nConnectNo, BSTR szFileName);
extern "C"   bool   DeleteTableEx(UINT nConnectNo, BSTR szTableName);
extern "C"   bool   UpdateEx(UINT nConnectNo, UINT nRecordNo);
// extern "C"   bool   AddNew();
extern "C"   bool  AppentRecTextEx(UINT nConnectNo, BSTR strTableName, BSTR strPackFile);
extern "C"   bool  TableNameExistsEx(UINT nConnectNo, BSTR strTableName);

extern "C"   bool  GetTablePathEx(UINT nConnectNo, UINT nRecordNo, BSTR * pszTablePath);

extern "C"   bool  ExecSQL(UINT nConnectNo, CString strSql);

enum HTTPREQUESTRESULT
{
	RES_REQ_DONE_OK,
	RES_REQ_ERROR_HTTP,
	RES_REQ_ERROR_GET,
	RES_REQ_ERROR_NOSPACE,
	//  		RES_REQ_ERROR_NET
};



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


