////////////////////////////////////////////////////////////////////////////
//   Copyright : Tsinghua Tongfang Optical Disc Co., LTD.
//   2002-03-14
//   Created By : CoFan
//
//   Email :CoFan@263.net
//
//   Description : READER.H 
//   History :
//
////////////////////////////////////////////////////////////////////////////

// Update:2007-12-3 16:44
//ʹ��1/100��ȡ��ԭ�������λ��ԭ�е�λ����̫�ͣ�
// Update:2007-11-28 11:32
// Update:2007-9-26 10:51
#ifndef READER_H_1
#define READER_H_1

#ifndef _WIN32
//#include <vector>
#ifdef __cplusplus
extern "C" {
#endif
#endif

#define  LATIN_CHARSET 3
#ifdef _WIN32
#include <Windows.h>
#pragma pack(push,2)
typedef unsigned __int64 __uint64;
typedef long CAJBOOL;
#else
#pragma pack(push,2)
typedef void *HANDLE;
typedef HANDLE HLOCAL;

typedef int CAJBOOL;


typedef unsigned char BYTE;

typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned short USHORT;
#ifndef __APPLE__
typedef unsigned int size_t;
typedef long long __int64;
typedef unsigned long long __uint64;
typedef unsigned long DWORD;
typedef DWORD COLORREF;
typedef int LONG;
#else
#ifdef __x86_64__
    typedef long __int64;
    typedef unsigned long __uint64;
    typedef int LONG;
    typedef unsigned int DWORD;
    typedef DWORD COLORREF;
#else
    typedef long long __int64;
    typedef unsigned long long __uint64;
    typedef long LONG;
    typedef unsigned long DWORD;
    typedef DWORD COLORREF;
#endif

#endif
#define WINAPI
#define CALLBACK
#define TRUE 1
#define FALSE 0
#define _MAX_EXT 256
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define HIWORD(l) ((WORD)((DWORD)(l) >> 16))
#define LOWORD(l) ((WORD)((DWORD)(l) & 0xffff))
#define MAKELONG(a, b) ((LONG)(((WORD)((DWORD)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD)(b) & 0xffff))) << 16))
#define LOBYTE(w) ((BYTE)((DWORD)(w) & 0xff))
#define HIBYTE(w) ((BYTE)((DWORD)(w) >> 8))
#define MAKEWORD(a, b) ((WORD)(((BYTE)((DWORD)(a) & 0xff)) | ((WORD)((BYTE)((DWORD)(b) & 0xff))) << 8))
#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb)>>16))
#define SRCCOPY             (DWORD)0x00CC0020 /* dest = source                   */
#define SRCAND              (DWORD)0x008800C6 /* dest = source AND dest          */
//#define ULONG_MAX     0xffffffffUL  /* maximum unsigned long value */
//#define UINT_MAX      0xffffffff    /* maximum unsigned int value */
#define ANSI_CHARSET            0
#define DEFAULT_CHARSET         1
#define SYMBOL_CHARSET          2
#define SHIFTJIS_CHARSET        128
#define HANGEUL_CHARSET         129
#define HANGUL_CHARSET          129
#define GB2312_CHARSET          134
#define CHINESEBIG5_CHARSET     136
#define OEM_CHARSET             255
#define JOHAB_CHARSET           130
#define HEBREW_CHARSET          177
#define ARABIC_CHARSET          178
#define GREEK_CHARSET           161
#define TURKISH_CHARSET         162
#define VIETNAMESE_CHARSET      163
#define THAI_CHARSET            222
#define EASTEUROPE_CHARSET      238
#define RUSSIAN_CHARSET         204

typedef struct tagRECT
{
	LONG    left;
	LONG    top;
	LONG    right;
	LONG    bottom;
} RECT, *PRECT, *LPRECT;
typedef const RECT *LPCRECT;
typedef struct tagPOINT
{
	LONG  x;
	LONG  y;
} POINT, *PPOINT, *LPPOINT;

typedef struct tagSIZE
{
	LONG        cx;
	LONG        cy;
} SIZE, *PSIZE, *LPSIZE;
/* structures for defining DIBs */
typedef struct tagBITMAPCOREHEADER {
	DWORD   bcSize;                 /* used to get to color table */
	WORD    bcWidth;
	WORD    bcHeight;
	WORD    bcPlanes;
	WORD    bcBitCount;
} BITMAPCOREHEADER, *LPBITMAPCOREHEADER, *PBITMAPCOREHEADER;

typedef struct tagBITMAPFILEHEADER {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;
typedef RGBQUAD * LPRGBQUAD;

typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD             bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;
#ifndef __APPLE__
typedef unsigned short WORD;
#endif
typedef struct _SYSTEMTIME {
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

#define MAX_PATH 260
void WINAPI CAJFILE_SetConfigFile(const char* cfgFileName);
//void WINAPI CAJFILE_AddFontDir(const char* fontDir);
//#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
//#pragma pack(push,2)

#ifdef READEREX_EXPORTS
#include "gtypes.h"
#else
typedef unsigned char Guchar;
typedef unsigned short Gushort;
typedef unsigned int Guint;
typedef unsigned short widechar;
#ifdef __APPLE__
# if defined(__x86_64) || defined(__x86_64__)
typedef unsigned int guint32;
typedef int gint32;
typedef unsigned int Gulong;
#else
typedef unsigned long guint32;
typedef long gint32;
typedef unsigned long Gulong;
#endif
#else
typedef unsigned long guint32;
typedef long gint32;
typedef unsigned long Gulong;
#endif
#endif
#define INTERNET_MAX_PATH_LENGTH        2048
#define INTERNET_MAX_SCHEME_LENGTH      32          // longest protocol name length
#define INTERNET_MAX_URL_LENGTH         (INTERNET_MAX_SCHEME_LENGTH \
	+ sizeof("://") \
	+ INTERNET_MAX_PATH_LENGTH)

//#ifndef widechar

//#endif
enum SELECT_TYPE{
	General=0,
	Col=1
};

#define TEB_VERSION		0x1
#define TEB_VERSION_PDF 0x2
#define TEB_VERSION_V3	0x3
#define TEB_VERSION_V4	0x4
#define TEB_VERSION_CAJ	0x5

typedef enum 
{
	DOC_UNKNOWN = 0,
	DOC_CAJ,
	DOC_NH,
	DOC_PDF,
	DOC_KDH,
	DOC_CAS,
	DOC_MLF,
	DOC_MLF1,
	DOC_TEB,
	DOC_TEB_V2,
	DOC_TEB_V3,
	DOC_CAA,
	DOC_DJVU,
	DOC_ODT,
	DOC_TEB_V4,			//Composite
	DOC_PS,
	DOC_BMP,
	DOC_JPEG,
	DOC_TIFF,
	DOC_PNG,
	DOC_MPS,
	DOC_S2,
	DOC_S72,
	DOC_S92,
	DOC_PS2,
	DOC_NPS,
	DOC_S10,
	DOC_CAJ3,
}CAJ_DOCUMENT_TYPE;

typedef struct{
	Gulong dwFlag;				//must be "CAJ\0"
	Gulong dwVersion;			//Document Version, MAKELONG(minor,major)
	Gulong dwCRC;				//CRC code, allow zero
	Gulong dwType;				//Document Type
	Gulong dwPageNum;			//page count
	Gulong dwPagesOffset;
	Gulong IsLinearized;			//linearized flag
	Gulong dwCatalog;	//
	Gulong dwXrefOffset;		//
	Gulong dwXrefNum;
	Gulong dwAppInfoLen;
	Gulong dwAppInfoOff;
	Gulong dwCommObjOffset;
	Gulong dwCommObjLength;
	Gulong dwReserved[24-2];
}CAJ_FILE_HEADER;

typedef struct {
	Gulong dwOffset;
	Gulong dwLength;
	Gulong dwID;
}PAGEOFFSET,*LPPAGEOFFSET;

#define NH_FILE_FLAG	((unsigned short) ('N' << 8) | 'H')
typedef struct
{
	Gulong dwFlag; // must be 'NH' or CAJ3
	Gulong dwVersion; // must 100
	Gulong dwDataOffset;
	Gulong dwSize;
	char  szProducer[60];
	short encrypt;
	char  szReserved[58];
}NH_FILE_HEADINFO,*LPNH_FILE_HEADINFO;
//sizeof(NH_FILE_HEADINFO) must be 128

typedef struct
{
	Gulong dwVersion; //0
	Gulong encrypt;
	Gulong dwPageNum;
	Gulong dwDocType;
}CAJ_FILE_HEADERINFO,*LPCAJ_FILE_HEADERINFO;

typedef struct
{
	Gulong dwPageOffset; //from file begin
	Gulong dwTextSize;  // not contain picture
	Gushort  wPicNum;
	Gushort  wSrcPage; // page printed on original copy
				 // if  srcPage == 0 then use physical page( caj file page seguence no )
}CAJ_FILE_PAGEINFO,*LPCAJ_FILE_PAGEINFO;
typedef struct
{
	Gulong dwPzCount;
	Gulong dwInfoOffset;
}CAJ_FILE_ZHAOZI_INFO,*LPCAJ_FILE_ZHAOZI_INFO;

typedef struct
{
	Gulong dwPageOffset; //from file begin
	Gulong dwTextSize;  // not contain picture
	Gushort  wPicNum;
	Gushort  wSrcPage; // page printed on original copy
				 // if  srcPage == 0 then use physical page( caj file page seguence no )
	CAJ_FILE_ZHAOZI_INFO zhaoziInfo;
}CAJ_FILE_PAGEINFO1,*LPCAJ_FILE_PAGEINFO1;

typedef struct {
	Gulong dwFlag;
	Gulong dwType;
}PAGE_TYPE;

typedef struct
{
	Gulong dwPageOffset; //from file begin
	Gulong dwTextSize;  // not contain picture
	Gulong dwDataSize;  //
	union{
		Gushort  wPicNum;
		Gushort  wPageOffsetLow;
	};
	union {
		Gushort  wSrcPage;    // page printed on original copy
		Gushort  wPageOffsetHigh;
	};
	// if  srcPage == 0 then use physical page( caj file page seguence no )
	union {
		CAJ_FILE_ZHAOZI_INFO zhaoziInfo;
		PAGE_TYPE type;
	};
	RECT  pageRect;
}CAJ_FILE_PAGEINFO2,*LPCAJ_FILE_PAGEINFO2;

typedef struct
{
	Gulong dwPicType; // 0: tif/jbig ,1 :tif/jpg, 2:new jpg, 3
	Gulong dwPicOffset;
	Gulong dwSize;
}CAJ_FILE_PICINFO,*LPCAJ_FILE_PICINFO;

typedef struct
{
	Gulong dwCode;
	Gulong dwSize;
	Gulong dwOffset;
	Gushort  wWidth;
	Gushort  wHeight;
}CAJ_FILE_ZHAOZI_ITEM,*LPCAJ_FILE_ZHAOZI_ITEM;

typedef struct
{
	BYTE	bVersion[12];
	Gushort	xOrgOfPage;
	Gushort	yOrgOfPage;
	Gushort	WidthOfPage;
	Gushort	HeightOfPage;
	Gulong	subVersion; // 0xe6020100
	BYTE	dummy[40];
}S2_FILE_HEAD,*LPS2_FILE_HEAD;

#define MUDLEN 32

typedef struct{
	union
	{;
		BYTE szSign[8];          //��־  'CAJ SE'
		__uint64 magic;
	};
	Gulong version;           //�汾��
	Gushort Linearized;         //���Ի���־
	Gushort  encrypt;           //���ܼ���
	SYSTEMTIME createTime;   //����ʱ��
	Gulong dwCreateSystemVersion; //����
	Gulong fileLength;        //�ļ�����
	char szMUD[MUDLEN];   //MUD
	Gulong headerLen;      // for linearized
	Gulong pagesLenOffset; // for linearized
	BYTE szCopyright[120-MUDLEN];   //��Ȩ��Ϣ
} CAJSE_FILEHEADER;
/*
 *	TEB file required
 */
#define FILE_ID_LEN			20
#define SYSTEM_KEY_LEN		32

#define ENCRYPT_NONE		0x0
#define ENCRYPT_DATA		0x1
#define ENCRYPT_CATALOG		0x2
#define ENCRYPT_APPINFO		0x4
#define ENCRYPT_INACTIVE	0x8
#define ENCRYPT_DUMMY		0x10
#define ENCRYPT_PAGEINFO	0x20
#define ENCRYPT_DRM			0x40

#define TEB_FILE_FLAG		(0x00424554)			//TEB\0
//#define TEBV4_FILE_FLAG		(0x34424554)			//TEB4
#define DJVU_FILE_FLAG		(0x54265441)			//AT&T
#define CAJ2_FILE_FLAG		(0x004A4143)			//CAJ\0
#define CAJ3_FILE_FLAG		(0x334A4143)			//CAJ3
#define KDH_FILE_FLAG		(0x2048444B)			//KDH\x20
#define PDF_FILE_FLAG		(0x46445025)			//%PDF
#define MLF_FILE_FLAG		(0x00464C4D)			//MLF\0
#define MLF1_FILE_FLAG		(0x20464C4D)			//MLF\x20
#define MLF2_FILE_FLAG		(0x08464C4D)			//MLF\b
#define CAJSE_FILE_FLAG		(0x00004553204A4143ULL)	//CAJ SE\0\0
#define CAA_FILE_FLAG		(0x5D5445475241545BULL)	//[TARGET]
#define PS_FILE_FLAG		(0x6F64412D53502125ULL)	//%!PS-Ado
#define PNG_FILE_FLAG		(0x0a1a0a0d474e5089ULL)	//\x89PNG\x0d\x0a\x1a\x0a
//#define GIF89_FILE_FLAG		(0x613938464947)	//GIF89a
//#define GIF87_FILE_FLAG		(0x613738464947)	//GIF87a
#define GIF_FILE_FLAG		(0x38464947)	//GIF8
typedef struct{
	union{
		char sign[4];          //��־  'TEB\0'
		guint32 magic;
	};
	gint32 version;            //�汾��
	char fileId[FILE_ID_LEN];
	short encrypt;           //���ܼ���
	short linearized;
	char copyright[128];   //��Ȩ��Ϣ
} TEB_FILEHEADER,*LPTEB_FILEHEADER;

typedef struct {
	gint32 pageCount;			//��ҳ��
	gint32 pageOffset;		//ҳ��Ϣƫ��λ��
	gint32 dirOffset;			//Ŀ¼��ƫ��λ��
	gint32 dirLength;			//Ŀ¼��Ϣ���ܳ���
	gint32 appOffset;			//������Ϣ��ƫ��λ��
	gint32 appLength;			//������Ϣ���ܳ���
	gint32 docType;			//s2 �ĵ�����
}TEB_FILEINFOHEADER,*LPTEB_FILEINFOHEADER;

typedef struct {
	gint32 commObjOffset;
	gint32 commObjLength;
	gint32 trailerOffset;
	gint32 trailerLength;
	gint32 encryptID;
	gint32 refNum;	//�������ñ���ܸ���
	gint32 refOffset;	//�������ñ��ƫ��λ��
	char cbPDFFileVer[20];
}TEB_PDFFILEINFOHEADER,*LPTEB_PDFFILEINFOHEADER;

typedef struct{
	union
	{
		BYTE szCopyright[40];
		struct {
			guint32 magic;
			char ver[36];
		}Version;
	};
	guint32 dwVersion;
	guint32 dwDirLen;
	guint32 dwDirOffset;
	guint32 dwAppendLen;
	guint32 dwAppendInfo;
	guint32 dwTrailerOffset;
	char szMUD[MUDLEN];
	short encrypt;
	BYTE bReserve[254-52-8-4-2-MUDLEN];
} KDH_HEADER;

typedef struct {
	Gushort dirRootID;        //��Ŀ¼�Ķ����
	Gushort dirCount;         //�ܵ�Ŀ¼�����
	Gushort catalogID;        //��ҳ�Ķ����
	Gushort pagesNum;         //��ҳ��
	Gushort watermarkID;      //����ˮӡ�Ķ����
	Gushort appendHeaderID;   //������Ϣ�����
	Gushort  refNum;          //������ñ�����ܸ���
	guint32 refOffset;       //�������ñ��ƫ��λ��
} CAJSE_INFOHEADER;


//##ModelId=3D943077031A
typedef struct
{
	char szName[128];
	gint32  iCount;
} NH_CONTENT_HEADER,*LPNH_CONTENT_HEADER;

//##ModelId=3D9430770340
#define NH_CONTENT_ITEM_NAME_LEN 256
typedef struct
{
	char szName[NH_CONTENT_ITEM_NAME_LEN];
	char szCode[24];  // ÿ2λΪһ��,max:10
	char szPhysicalPage[12];
	char szPeriodPage[12];
	gint32  iGrade;
} NH_CONTENT_ITEM,*LPNH_CONTENT_ITEM;


typedef struct
{
	widechar szName[NH_CONTENT_ITEM_NAME_LEN];
	char szCode[24];  // ÿ2λΪһ��,max:10
	char szPhysicalPage[12];
	char szPeriodPage[12];
	gint32 iGrade;
} NH_CONTENT_ITEMW,*LPNH_CONTENT_ITEMW;

typedef struct
{
	widechar szName[NH_CONTENT_ITEM_NAME_LEN];
	char szCode[24];  // ÿ2λΪһ��,max:10
	char szPhysicalPage[12];
	char szPeriodPage[12];
	gint32 iGrade;
	gint32 x,y,z;
} NH_CONTENT_ITEMEXW,*LPNH_CONTENT_ITEMEXW;

typedef struct tagOUTLINE_ITEM
{
	//gint32 cbSize;
	gint32 x;
	gint32 y;
	gint32 z;
	gint32 nPage;
	gint32 nLevel;
#ifdef __APPLE__
    void* pParent;
	void* pChild;
	void* pSibling;
#else
	tagOUTLINE_ITEM* pParent;
	tagOUTLINE_ITEM* pChild;
	tagOUTLINE_ITEM* pSibling;
#endif
	gint32 nLength;
	widechar szTitle[1];
}OUTLINE_ITEM, *LPOUTLINE_ITEM;

typedef enum
{
	CDT_UNKNOWN = -1,
	CDT_PAGE = 0,
	CDT_FILE_PAGE,
	CDT_URL,
}CATALOG_DEST_TYPE;
typedef struct 
{
	CATALOG_DEST_TYPE Type;
	union {
		struct {
			gint32 nPage;
			gint32 x;
			gint32 y;
		}Page;
		struct {
			gint32 nPage;
			gint32 x;
			gint32 y;
			widechar *pFileName;
		}FilePage;
		widechar *pUrl;
	};
}CATALOG_DEST;
typedef struct tagCATALOG_ITEMEX
{
	CATALOG_DEST dest;
#if ((defined __APPLE__) || (defined __ANDROID__))
    void* pParent;
	void* pChild;
	void* pSibling;
#else
	tagCATALOG_ITEMEX* pParent;
	tagCATALOG_ITEMEX* pChild;
	tagCATALOG_ITEMEX* pSibling;
#endif
	gint32 nLength;
	widechar szTitle[1];
}CATALOG_ITEMEX, *LPCATALOG_ITEMEX;

//##ModelId=3D943077035E
#define FIND_TEXT_LEN	127
typedef struct
{
	char	szFindText[FIND_TEXT_LEN+1]; //��Ҫ���ҵ��ı�
	CAJBOOL	bFindMultiSymboAndNum;		 //�Ƿ����ȫ�Ƿ��ź�����
	CAJBOOL	bMatchCase;					 //�Ƿ����ִ�Сд
	gint32	iDirection;					 //���ҷ���0-ֻ�ڵ�ǰҳ���ң�1-���²��ң�2-��ǰ
										 //���ң�3-ȫ��ҳ����
	Gulong	nPageIndex;					 //ָ������ʱ�ĵ�ǰҳ
}FIND_TEXT,*LPFIND_TEXT;

typedef struct
{
	widechar szFindText[FIND_TEXT_LEN+1]; //��Ҫ���ҵ��ı�
	CAJBOOL	bFindMultiSymboAndNum;		 //�Ƿ����ȫ�Ƿ��ź�����
	CAJBOOL	bMatchCase;					 //�Ƿ����ִ�Сд
	gint32	iDirection;					 //���ҷ���0-ֻ�ڵ�ǰҳ���ң�1-���²��ң�2-��ǰ
	//���ң�3-ȫ��ҳ����
	Gulong	nPageIndex;					 //ָ������ʱ�ĵ�ǰҳ
}FIND_TEXTW,*LPFIND_TEXTW;

#define MULTI_SYMBONUM		0x00000001		//����ȫ���ַ�
#define MATCH_CASE			0x00000002		//���ִ�Сд
#define REGULAR_EXP			0x00000004		//������ʽ����
#define MATCH_FORMAT		0x00000008		//ƥ���ʽ�����壩
#define ONLY_FIRST_RESULT	0x00000010		//ֻ�����ҵ���ÿһҳ��һ�����
#define INCLUDE_NEAR_TEXT	0x00000020		//��������ٽ��ı�
#define EXCLUDE_TEXT		0x00000040		//�������ı�
#define EXP_IS_OUTER		0x00000080		//��Ҫ���ҵı��ʽ��һ���ⲿָ�룬szFindText��ǰ4���ֽڱ�ʾ���ȣ�
											//����4(��8)���ֽ�Ϊһ��ָ���ַ
#define ONLY_FIRST_RESULT_ALL	0x00000110  //ֻ�����ҵ��ĵ�һ�����
#define SKIP_WHITESPACE		0x00000200
//����4���ֽ�Ϊһ��ָ���ַ

#define CUR_PAGE		0x0000				//ֻ�ڵ�ǰҳ����
#define DOWN_PAGE		0x0001				//�ӵ�ǰҳ��ʼ��������
#define UP_PAGE			0x0002				//�ӵ�ǰҳ��ʼ��������
#define ALL_PAGE		0x0003				//������ҳ������

typedef struct
{
	Gushort cbSize;							//�˽�ṹ�Ĵ�С
	widechar	szFindText[FIND_TEXT_LEN+1];	//��Ҫ���ҵ��ı�����ʽ
	Gulong  uFlags;							//���ұ�־λ��MULTI_SYMBONUM,MATCH_CASE�ȣ������ǵ����
	Gulong  uDirect;						//HIWORDΪ���ҷ��򣬿�ΪCUR_PAGE,DOWN_PAGE,UP_PAGE,ALL_PAGE�е�һ����LOWORDΪ��ǰҳ
	Gulong  uFontSize;						//�����С�����û������MATCH_FORMATλ������������ԣ� low:lo, high:hi
	Gulong  uNearTextLen;                   //���ҽ��ǰ���ı��ĳ���
}FIND_TEXTEXW,*LPFIND_TEXTEXW;

/////////////////////////////////////////////////////////////////////////////
// SEL_TEXT: metrics unit is POINT (1/72 inch)

//##ModelId=3D9430770386
typedef struct
{
    gint32 iPage;
    gint32 nFound;         // 0 if failed
    char* pszFoundText;   // ASCIIZ
	union{
    RECT    prcFound[1];     // in point
	RECT	rcFound[1];
	};
}LSEL_TEXT, *LPLSEL_TEXT;

typedef struct
{
	gint32 iPage;
	gint32 nFound;         // 0 if failed
	widechar* pszFoundText;   // ASCIIZ
#ifndef _WIN32
	gint32 nChar;
#endif
	union{
		RECT    prcFound[1];     // in point
		RECT	rcFound[1];
	};
}LSEL_TEXTW, *LPLSEL_TEXTW;
// Returns NULL if failed
/////////////////////////////////////////////////////////////////////////////

typedef struct
{
	Gulong	nPageIndex; // ��ǰ������ڵ�ҳ
	char    *nearText;  // ���ҵ��Ľ���������������������ı�����������<H>sample</H>����
	gint32     nFound;    // �����Χ�������ľ��θ���
	RECT	prcFound[1];// �����Χ�ľ���
}FOUND_RESULT,*LPFOUND_RESULT;

typedef struct
{
	Gulong	nFound;		//�������
	LPFOUND_RESULT *ppResult; //�������н����ָ������
}FOUND_RESULT_ARRAY,*LPFOUND_RESULT_ARRAY;

typedef struct
{
	Gulong	nPageIndex;
	widechar   *nearText;
	gint32    nFound;
	RECT	prcFound[1];
}FOUND_RESULTW,*LPFOUND_RESULTW;

typedef struct
{
	Gulong	nFound;
	LPFOUND_RESULTW *ppResult;
}FOUND_RESULT_ARRAYW,*LPFOUND_RESULT_ARRAYW;

typedef struct
{
	Gulong	nPageIndex;
	widechar   *nearText;
	gint32    nFound;
	RECT	*prcFound;
	double FontSize;
}FOUND_RESULT1W,*LPFOUND_RESULT1W;

typedef struct
{
	Gulong	nFound;
	LPFOUND_RESULT1W *ppResult;
}FOUND_RESULT_ARRAY1W,*LPFOUND_RESULT_ARRAY1W;

typedef struct {
	union {
		double x1;
		double left;
	};
	union {
		double y1;
		double top;
	};
	union {
		double x2;
		double right;
	};
	union {
		double y2;
		double bottom;
	};
}RECTF;

typedef struct {
	double x1;
	double y1;
	double x2;
	double y2;
	double x3;
	double y3;
	double x4;
	double y4;
}QUADF;

typedef struct {
	gint32 cbSize;
	widechar* pszTitle;
	char* pszURL;
	int nRect;
	RECTF Rects[1];
}ANNOT_ITEM, *LPANNOT_ITEM;

#define MAX_URI_TITLE	2048


typedef enum  {
	AT_TEXT,
	AT_LINK,
}PDF_ANNOT_TYPE;

typedef enum  {
	ALAT_URI,
	ALAT_GOTO,
}ANNOT_LINK_ACTION_TYPE;

typedef struct {
	ANNOT_LINK_ACTION_TYPE eType;
}ANNOT_LINK_ACTION;

typedef struct {
	ANNOT_LINK_ACTION_TYPE eType;
	char szURL[INTERNET_MAX_URL_LENGTH+1];
}ANNOT_LINK_ACTION_URI;

typedef struct {
	ANNOT_LINK_ACTION_TYPE eType;
	int nPage;
	int nFitT;          //XYZ=0,Fit=1, 
	double left;
	double top;
	double right;
	double bottom;
	double zoom;
	int changeLeft;
	int changeTop;
	int changeZoom;
}ANNOT_LINK_ACTION_GOTO;

typedef struct {
	PDF_ANNOT_TYPE eType;
}ANNOT;

typedef struct {
	PDF_ANNOT_TYPE eType;
	//gint32 cbSize;
	widechar szTitle[MAX_URI_TITLE+1];
	ANNOT_LINK_ACTION *pAction;
	//char szURL[INTERNET_MAX_URL_LENGTH+1];
	int nQuad;
	double W;
	int S;
	RGBQUAD color;
	QUADF quads[1];
}LINKANNOT_ITEM, *LPLINKANNOT_ITEM;

typedef struct {
	//gint32 cbSize;
	guint32 nFlags;
	int nCount;
	ANNOT* pAnnot[1];
}ANNOTS;
/////////////////////////////////////////////////////////////////////////////
typedef void* CAJFILE;
typedef void* CAJERROR;

enum CAJFILE_ERROR_CODE{
	CAJFILE_NO_ERROR=0,
	CAJFILE_OPEN_ERROR,			// Could not open the target file.
	CAJFILE_TYPE_ERROR,			// Target file was not recognised.
	CAJFILE_CIPHER_ERROR,		// Cipher check error, must be update.
	CAJFILE_PAGE_INDEX_ERROR,
	CAJFILE_ALLOC_BUFFER_ERROR,
	CAJFILE_UNCOMPRESS_DATA_ERROR,
	CAJFILE_CANNOT_READ_XREF,
	CAJFILE_CANNOT_READ_PAGE,
	CAJFILE_FILE_VERSION_ERROR,
	CAJFILE_XREF_ERROR,
	CAJFILE_IO_ERROR,
	CAJFILE_UNKNOWN_ERROR,				//δ֪����
	CAJFILE_REQUIRED_USER_PASSWD,		//��Ҫ������
	CAJFILE_INACTIVE,					//��Ҫ����
	CAJFILE_ERROR_ENCRYPT,				//����ļ��ܲ���
	CAJFILE_REQUIRED_CERTIFICATE,		//��Ҫ֤����֤
	CAJFILE_IO_OVERLAPPED,		    	//IO����δ���
	CAJFILE_PASSWD_EXPIRED,				//�����Ѿ�����
	CAJFILE_AUTH_UNKNOWN_ERROR,			//��֤����
	CAJFILE_AUTH_PASSWD_EXPIRED,		//�����ѹ���
	CAJFILE_AUTH_OPEN_TIMES_EXCEED,		//�ﵽ�򿪴�������
	CAJFILE_AUTH_CANNOT_CONNECT_SERVER,	//������֤����������
	CAJFILE_AUTH_NO_RIGHTS_FILE,		//δ�ҵ�Ȩ���ļ�
	CAJFILE_AUTH_RIGHTS_FILE_ERROR,		//Ȩ���ļ�����
	CAJFILE_AUTH_SERVER_ERROR,			//����������
	CAJFILE_AUTH_CLIENT_REQUEST_ERROR,	//����������
	CAJFILE_NOT_FOUND_CERTIFICATE,		//δ�ҵ����ʵ�֤��
	CAJFILE_NOT_FOUND_CAJVD,			//δ��װCAJVD
	CAJFILE_OPEN_CERTIFICATE_ERROR,		//�޷���֤���ļ���֤���ļ�����
	CAJFILE_OPEN_CAJVD_ERROR,			//�޷���CAJVD
	CAJFILE_MACHINE_NOT_REGISTERED,		//�����δע��
	CAJFILE_READ_CAJVD_ERROR,			//�޷���ȡCAJVD
	CAJFILE_ONLY_IN_USB_DISK,			//ֻ����ָ����USB���ϴ�
	CAJFILE_NOT_FOUND_USB_DISK,			//û���ҵ��ʺϵ�USB��
	CAJFILE_AUTH_RIGHTS_FILE_VER_ERROR,		//Ȩ���ļ��汾����
	CAJFILE_AUTH_IP_ERROR,					//IP��Χ����
	CAJFILE_AUTH_ERROR_UERNAME,				//�û�������
	CAJFILE_AUTH_PT_ERROR,					//��Ʒ������֤ʧ��

};
/*
typedef struct
{
    widechar faceName[LF_FACE_SIZE+1];
}FONT_INFO;

typedef struct
{
    int count;
    FONT_INFO info[1];
}FONT_COLLECTIONS
  */  
//FONT_COLLECTIONS* WINAPI CAJFILE_GetFontCollection(int charset, unsigned short langid);
/*
 *	ͨ����������ȡ�ô������
 */
int WINAPI CAJFILE_GetLastErrorCode(
	CAJERROR hError);

const char* WINAPI CAJFILE_GetLastErrorMsg(CAJERROR hError);

CAJBOOL WINAPI CAJFILE_GetTempPath(int cchMax, char* pszPath);
/*
 *	ȡ���ĵ�����
 */
guint32 WINAPI CAJFILE_GetDocType(
	const char* lpszSource);
guint32 WINAPI CAJFILE_GetDocTypeW(
	const widechar* lpszSource);

guint32 WINAPI CAJFILE_GetDocTypeEx(
	const char* lpszSource,
	CAJBOOL *pbIsCAJ2);
guint32 WINAPI CAJFILE_GetDocTypeExW(
	const widechar* lpszSource,
	CAJBOOL *pbIsCAJ2);
/*
 *	
 */
const char* WINAPI CAJFILE_GetLinearized(
	const char* lpszSource);

typedef struct {
	CAJFILE hFile;
	int nPage;
} PAGE_LOAD_S;
/*
 *	����������
 */
CAJERROR WINAPI CAJFILE_CreateErrorObject();
/*
*   ReaderEx��ʼ��
*/
#ifndef _WIN32
void WINAPI CAJFILE_Init(const char* execDir, const char** fontDirs, int fontDirCount);
#else
void WINAPI CAJFILE_Init();
#endif
/************************************************************************/
/*  ��һ���ļ�                                                */
/************************************************************************/
CAJFILE WINAPI CAJFILE_Open(
	const char* lpszFileName);
CAJFILE WINAPI CAJFILE_OpenW(
	const widechar* lpszFileName);
/************************************************************************/
/*  ReaderEx�ͷ�                                                 */
/************************************************************************/
void WINAPI CAJFILE_Shutdown();

typedef enum 
{	
	RHT_REDRAW,
	RHT_PAGE_CACHE,
	RHT_FONT_COLLECTION,
	RHT_FONT_CACHE,
	RHT_FILE_CACHE,
}REFRESH_TYPE;
/************************************************************************/
/* ˢ��ҳ������建��                                           */
/************************************************************************/
void WINAPI CAJFILE_Refresh(CAJFILE hFile, REFRESH_TYPE _Type);
CAJBOOL WINAPI CAJFILE_IsFileComplete(CAJFILE hFile);
/*
*/
typedef enum 
{
	HARD_FILE=0,
	MEM_FILE,
	NET_FILE,
	HARD_FILEW,
	STREAM_FILE
}_FILE_TYPE;

typedef void* (WINAPI *FSTREAM_OPEN)(const char* file, const char* mode);
typedef long (WINAPI *FSTREAM_READ)(void *stream, void *lpBuffer, long Size);
typedef long (WINAPI *FSTREAM_WRITE)(void *stream, void *lpBuffer, long Size);
typedef long (WINAPI *FSTREAM_TELL)(void *stream);
typedef long (WINAPI *FSTREAM_SEEK)(void *stream, long Off, long Way);
typedef long (WINAPI *FSTREAM_EOF)(void *stream);
typedef long (WINAPI *FSTREAM_CLOSE)(void *stream);

typedef struct {
	Gulong cbSize;		// Size of the OPEN_PARAMS structure.
	CAJERROR hError;	// Error object handle.
	CAJBOOL bPreparsePage;
	char szPassword[128];
	Gulong nDocType;
	_FILE_TYPE nFileType;
	char szFileCachePath[MAX_PATH];
}OPEN_PARAMS,*LPOPEN_PARAMS;

typedef struct {
	Gulong cbSize;		// Size of the OPEN_PARAMS structure.
	CAJERROR hError;	// Error object handle.
	CAJBOOL bPreparsePage;
	char szPassword[128];
	Gulong nDocType;
	_FILE_TYPE nFileType;
	widechar szFileCachePath[MAX_PATH];
}OPEN_PARAMSW,*LPOPEN_PARAMSW;

#define OPEN_FILE_FLAG_UNICODE			0x00000001
#define OPEN_FILE_FLAG_PREPARSE			0x00000002
//#define OPEN_FILE_FLAG_UNICODE			0x00000004
#define OPEN_FILE_FLAG_NO_PAGE_MARGIN	0x00000008
#define OPEN_FILE_FLAG_IGNORE_FILEID	0x00000010
#define OPEN_FILE_FLAG_RIGHTS_FILE		0x00000020
#define OPEN_FILE_FLAG_RIGHTS_MEMFILE	0x00000040
#define OPEN_FILE_FLAG_RIGHTS_NETFILE	0x00000080
#define OPEN_FILE_FLAG_CACHE_PAGE_IMAGE	0x00000100
#define OPEN_FILE_FLAG_SET_COOKIE		0x00000200

#define OPEN_FILE_FLAG_LOCAL_ENCRYPT	0x00000400
typedef struct {
	Gulong Length;
	char *pv;
}OPV;

typedef struct {
	char szPassword[128];
	int cam;
	int blockSize;
}LOCAL_ENCRYPT;

typedef struct {
	Gulong cbSize;		// Size of the OPEN_PARAMS structure.
	Gulong uFlags;
	CAJERROR hError;	// Error object handle.
	char szPassword[128];
	Gulong nDocType;
	_FILE_TYPE nFileType;
	FSTREAM_OPEN pfnOpen;			//�ļ������ص�����
	FSTREAM_READ pfnRead;
	FSTREAM_SEEK pfnSeek;
	FSTREAM_TELL pfnTell;
	FSTREAM_EOF	 pfnEof;
	FSTREAM_CLOSE pfnClose;
	union {
		char szFileCachePath[MAX_PATH];
		widechar wszFileCachePath[MAX_PATH];
	};
	char *pv;
}OPEN_PARAMSEX,*LPOPEN_PARAMSEX;
/*
 *	 ��չ�Ĵ��ļ�����
 */
CAJFILE WINAPI CAJFILE_OpenEx(
	const char* lpszFileName,
	LPOPEN_PARAMS lpop
	);

CAJFILE WINAPI CAJFILE_OpenExW(
	const widechar* lpszFileName,
	LPOPEN_PARAMSW lpop
	);

CAJFILE WINAPI CAJFILE_OpenEx1(
	const void* lpszFileName,
	LPOPEN_PARAMSEX lpop
	);
Gulong WINAPI CAJFILE_GetDocTypeEx1(
	const void* lpszSource,
	LPOPEN_PARAMSEX lpop,
	CAJBOOL *pbIsCAJ2);
//�رմ򿪵��ļ�
void WINAPI CAJFILE_Close(
	CAJFILE hFile);
typedef enum  {
	FCA_NONE,
	FCA_CLEAR_CACHE
}FILE_CLOSE_ACTION;
void WINAPI CAJFILE_CloseEx(
	CAJFILE hFile, FILE_CLOSE_ACTION fca);
/*
 *	
 *  
 */
void WINAPI CAJFILE_ResetErrorHandle(
	CAJERROR hError);
void WINAPI CAJFILE_ReleaseErrorHandle(
	CAJERROR hError);

int WINAPI CAJFILE_GetPysicalPage(
	CAJFILE hFile,
	Gulong nPageIndex);
int WINAPI CAJFILE_GetPysicalPageEx(
	const char* pszFileName, 
	Gulong nPageIndex);
//ȡ����ҳ��
int WINAPI CAJFILE_GetPageCount(
	CAJFILE hFile);

typedef enum  {
	PLS_NA,
	PLS_NLOAD,
	PLS_LOADING,
	PLS_LOAD
}PAGE_LOAD_STATUS;
/************************************************************************/
/* �������ʱ����׼��ҳ������                                   */
/************************************************************************/
PAGE_LOAD_STATUS WINAPI CAJFILE_PreparePage(CAJFILE hFile, int nPage, CAJBOOL bLoadIt);
void WINAPI CAJFILE_SetFilePassword(
	CAJFILE hFile,
	const char* pszPassword);
/************************************************************************/
/* ȡ���ļ�ID,(�Ѿ��ϳ�,��CAJFILE_GetDocInfoEx(DIT_FILE_ID)����)     */
/************************************************************************/
CAJBOOL WINAPI CAJFILE_GetFileID(const char* pszFileName,char* id);

/************************************************************************/
/* �������ʱ����ȡ�÷�����������Ϣ                             */
/* (�Ѿ��ϳ�,��CAJFILE_GetDocInfoEx(DIT_DOC_INFO)����)          */
/************************************************************************/
Gulong WINAPI CAJFILE_GetDocInfo(
	CAJFILE hFile,
	char *pBuf,
	Gulong uBufLen);

typedef enum 
{
	DIT_SERVER_RESPONSE,
	DIT_APP_INFO,
	DIT_DOC_XML,
	DIT_BACKGROUNDMUSIC,
	DIT_FILE_ID,
	DIT_FILE_DOI,
	DIT_LECTURE_FILE,
	DIT_ATTACHMENT_FILE_COUNT,
	DIT_ATTACHMENT_FILE_INFO,
	DIT_CATALOG_XML,
	DIT_NET_FILE_SIZE,//�����ļ���С
	DIT_NET_FILE_CUR_SIZE,//�����ļ���ǰ���ش�С
	DIT_DOC_INFO,
    DIT_DOC_PATH,
	DIT_NET_FILE_CAN_PREDOWNLOAD,
    DIT_DOC_PATHW,
}DOC_INFO_TYPE;

Gulong WINAPI CAJFILE_GetDocInfoEx(
	CAJFILE hFile,
	DOC_INFO_TYPE _Type,
	char *pBuf,
	Gulong uBufLen);

#define  ATTACH_FILE_NAME_LEN	128
typedef struct {
	int nIndex;
	int Size;
	Gulong CreationDate;
	Gulong ModDate;
	Guchar CheckSum[16];
	widechar szName[ATTACH_FILE_NAME_LEN];
}ATTACH_FILE, *LPATTACH_FILE;

#define AF_FLAG_HIDDEN			0x00000001
typedef struct {
	int Size;
	Gulong Flags;
	Gulong CreationDate;
	Gulong ModDate;
	widechar szName[ATTACH_FILE_NAME_LEN];
	widechar szFileName[MAX_PATH];
}ATTACH_FILE1, *LPATTACH_FILE1;

int WINAPI CAJFILE_GetAttachFileInfo(
	CAJFILE hFile,
	LPATTACH_FILE pAttach,
	int nSize);
CAJBOOL WINAPI CAJFILE_SaveAttachFile(
	CAJFILE hFile,
	int nIndex,
	const char* pszFileName);
CAJBOOL WINAPI CAJFILE_SaveAttachFile2(
	CAJFILE hFile,
	int nIndex,
	void *stream,
	FSTREAM_WRITE pfnWrite);
int WINAPI CAJFILE_GetAnnots(CAJFILE hFile, int nPage, ANNOTS *pAnnots);
void WINAPI CAJFILE_ReleaseAnnots(ANNOTS *pAnnots);

#define ATTACH_FILE_1_UNICODE	0x00000001		//unicode �ļ���
#define ATTACH_FILE_1_TO_FILE	0x00000002		//���浽�ļ�
typedef struct {
	guint32 uFlags;
	union {
		const char *pszSrcName;
		const widechar *pwszSrcName;
	};
	union {
		union {
			const char *pszDestName;
			const widechar *pwszDestName;
		};
		struct {
			gint32 Len;
			char *pBuffer;
		}Buffer;
	}SaveTo;

}SAVE_ATTACH_FILE_1, *LPSAVE_ATTACH_FILE_1;

/*
�����ļ�����
*/
int WINAPI CAJFILE_SaveAttachFile1(CAJFILE hFile, LPSAVE_ATTACH_FILE_1 pSave);
/*
 *	��ָ���豸����ʾָ��ҳ
 */
typedef enum 
{
	RGB24,
	GRAY,
	WHITE_BLACK
}COLOR_TYPE;
typedef struct tagLOGPAGE {
	CAJBOOL print;
	COLOR_TYPE colorType;
	gint32 rotate;			// ��ת�Ƕ�(0,90,180,270)
	gint32 nScale;			// ���űȣ��԰ٷֱ�Ϊ��λ
	union {
		gint32 sliceX;			// ��Ҫ��ʾҳ�����ʼX
		gint32 left;
	};
	union {
		gint32 sliceY;			// ��Ҫ��ʾҳ�����ʼY
		gint32 top;
	};
	gint32 sliceW;			// ��Ҫ��ʾҳ��Ŀ��
	gint32 sliceH;			// ��Ҫ��ʾҳ��ĸ߶�
	union {
		gint32 X2;
		gint32 right;
	};
	union {
		gint32 Y2;
		gint32 bottom;
	};
	gint32 offsetX;		// ����ʾDC��ƫ��X
	gint32 offsetY;		// ����ʾDC��ƫ��Y
	CAJBOOL bCachePage;
#ifndef _WIN32
	int dpi;
#endif
}LOGPAGE,*LPLOGPAGE;

void CAJFILE_SetReDraw(
	CAJFILE hFile,
	CAJBOOL bReDraw);
#if (defined _WIN32) && (!defined __WPHONE__)
typedef void (CALLBACK* NEEDREPAINTCALLBACK)(CAJFILE hFile, int nPage, RECT rcRepaint);
void CAJFILE_NeedRepaintCallback(NEEDREPAINTCALLBACK lpfn);

void WINAPI CAJFILE_DrawPageSlice(
	CAJFILE hFile,		// �ļ����
	int nPage,			// ��Ҫ��ʾ��ҳ1 <= nPage <= nPageCount
	HDC dc,				// ��ʾDC
	int rotate,			// ��ת�Ƕ�(0,90,180,270)
	int nScale,			// ���űȣ��԰ٷֱ�Ϊ��λ
	int sliceX,			// ��Ҫ��ʾҳ�����ʼX
	int sliceY,			// ��Ҫ��ʾҳ�����ʼY
	int sliceW,			// ��Ҫ��ʾҳ��Ŀ��
	int sliceH,			// ��Ҫ��ʾҳ��ĸ߶�
	int offsetX,		// ����ʾDC��ƫ��X
	int offsetY,		// ����ʾDC��ƫ��Y
	CAJBOOL bCachePage);

void WINAPI CAJFILE_PrintPageSlice(
	CAJFILE hFile,		// �ļ����
	int nPage,			// ��Ҫ��ʾ��ҳ1 <= nPage <= nPageCount
	HDC dc,				// ��ʾDC
	int rotate,			// ��ת�Ƕ�(0,90,180,270)
	int nScale,			// ���űȣ��԰ٷֱ�Ϊ��λ
	int sliceX,			// ��Ҫ��ʾҳ�����ʼX
	int sliceY,			// ��Ҫ��ʾҳ�����ʼY
	int sliceW,			// ��Ҫ��ʾҳ��Ŀ��
	int sliceH,			// ��Ҫ��ʾҳ��ĸ߶�
	int offsetX,		// ����ʾDC��ƫ��X
	int offsetY);		// ����ʾDC��ƫ��Y
#else

#endif
typedef struct tagDRAW_PARAM {
	int cbSize;				// size of struct, caller must fill it.
	COLOR_TYPE colorType;
	int rotate;			// ��ת�Ƕ�(0,90,180,270)
	int nScale;			// ���űȣ��԰ٷֱ�Ϊ��λ
	int sliceX;			// ��Ҫ��ʾҳ�����ʼX
	int sliceY;			// ��Ҫ��ʾҳ�����ʼY
	int sliceW;			// ��Ҫ��ʾҳ��Ŀ��
	int sliceH;			// ��Ҫ��ʾҳ��ĸ߶�
	int offsetX;		// ����ʾDC��ƫ��X
	int offsetY;		// ����ʾDC��ƫ��Y
	int dpi;
} DRAW_PARAM,*LPDRAW_PARAM;
#if (defined _WIN32) && (!defined __WPHONE__)
void WINAPI CAJFILE_PrintPageSliceEx(
	CAJFILE hFile,
	HDC dc,
	int nPage,
	LPDRAW_PARAM lpDraw
	);
#else
typedef void (CALLBACK* DRAWPAGESLICE_CALLBACK)(void *param,int XDest,int YDest,int dwWidth,int dwHeight,int XSrc,int YSrc,char *lpvBits,int depth,int bpl);
void CAJFILE_DrawPageSlice(CAJFILE hFile, int nPage, LPDRAW_PARAM lpDraw, DRAWPAGESLICE_CALLBACK fnDraw, void *param);
#endif

char* WINAPI CAJFILE_DrawPageSlice1(
	CAJFILE hFile,
	int nPage,
	LPDRAW_PARAM lpDraw, 
	int* XDest,
	int* YDest,
	int* dwWidth,
	int* dwHeight,
	int* XSrc,
	int* YSrc,
	int* depth,
	int* bpl);

//
CAJBOOL WINAPI CAJFILE_IsImagePage(
	CAJFILE hFile,
	int nPage);
//ȡ�õ�ǰҳ���С
void WINAPI CAJFILE_GetPageSize(
	CAJFILE hFile,
	int nPage,
	LPSIZE lpPageSize,
#ifndef __APPLE__
	int nMod=0);
#else
    int nMod);
#endif
void WINAPI CAJFILE_GetCropBox(
	CAJFILE hFile,
	int nPage,
	LPRECT lpCropBox);
//ȡ�õ�ǰĿ¼ͷ
CAJBOOL WINAPI CAJFILE_GetCatalogHeader(
	CAJFILE hFile,
	LPNH_CONTENT_HEADER pCatalogHeader);
CAJBOOL WINAPI CAJFILE_GetCatalogItem(
	CAJFILE hFile,
	int iItemCount,
	LPNH_CONTENT_ITEM pCatalogItem,
#ifndef __APPLE__
	int iIndex=0);
#else
    int iIndex);
#endif
/*
����iItemCount=0,��ֻ����pCatalogItem
*/
LPNH_CONTENT_ITEM WINAPI CAJFILE_GetCatalog(
	CAJFILE hFile,
	LPNH_CONTENT_HEADER pHeader,
	int* iItemCount,
#ifndef __APPLE__
                                            int iIndex=0);
#else
    int iIndex);
#endif

LPCATALOG_ITEMEX WINAPI CAJFILE_GetCatalogEx(CAJFILE hFile);

LPNH_CONTENT_ITEMW WINAPI CAJFILE_GetCatalogW(
	CAJFILE hFile,
	LPNH_CONTENT_HEADER pHeader,
	int* iItemCount,
#ifndef __APPLE__
                                              int iIndex=0);
#else
    int iIndex);
#endif

void WINAPI CAJFILE_ReleaseCatalog(
	LPNH_CONTENT_ITEM pItem);
void WINAPI CAJFILE_ReleaseCatalogW(
	LPNH_CONTENT_ITEMW pItem);
void WINAPI CAJFILE_ReleaseCatalogEx(LPCATALOG_ITEMEX pOutline);

//��������
#ifndef __APPLE__
void WINAPI CAJFILE_SetBig5(
	CAJBOOL bShowAsBig5=TRUE);
#endif
typedef enum  {
	RECT_SEL,
	ROW_SEL,
	COL_SEL,
	ALL_SEL,
	RECT_SEL_P,
	ROW_SEL_P,
	COL_SEL_P,
	ALL_SEL_P,
}SEL_MODE;
//ѡ������
LPLSEL_TEXT WINAPI CAJFILE_SelectTextEx(
	CAJFILE hFile,
	LPPOINT lptStart,
	LPPOINT lptEnd,
	SEL_MODE mode,
	int iPage);

LPLSEL_TEXTW WINAPI CAJFILE_SelectTextExW(
	CAJFILE hFile,
	LPPOINT lptStart,
	LPPOINT lptEnd,
	SEL_MODE mode,
	int iPage);

void WINAPI CAJFILE_ReleaseSelTextResult(
	LPLSEL_TEXT lpSelResult);
void WINAPI CAJFILE_ReleaseSelTextResultW(
	LPLSEL_TEXTW lpSelResult);
//ȡ�õ�ǰҳ������
int  WINAPI CAJFILE_GetPageText(
	CAJFILE hFile,
	int iPage,
	char* lpTextBuffer,
	int iTextBufferLen);
int  WINAPI CAJFILE_GetPageTextW(
	CAJFILE hFile,
	int iPage,
	void* lpTextBuffer,
	int iTextBufferLen);

int	 WINAPI CAJFILE_FindTextEx(
	CAJFILE hFile,
	int iPage,
	const char *pszFindText,
	LPRECT lprcFind);
/*
 *	���һص�����
 *  nPageCount		:��Ҫ���ҵ���ҳ��
 *  nCurPage		:��ǰҪ���ҵ�ҳ
 *  bStop			:�����ֹͣ���ң��򽫴�����ΪTRUE
 *  ppRsult			:���˲��ҵ��Ľ��
 *  nResultCount	:���˲��ҵ��Ľ������
 */
typedef void (CALLBACK* FINDCALLBACK)(int nPageCount,int nCurPage,CAJBOOL *bStop,const LPFOUND_RESULT *ppResult,gint32 nResultCount);
typedef void (CALLBACK* FINDCALLBACKW)(int nPageCount,int nCurPage,CAJBOOL *bStop,const LPFOUND_RESULTW *ppResult,gint32 nResultCount);

LPFOUND_RESULT_ARRAY WINAPI CAJFILE_FindAllText(
	CAJFILE hFile,
	LPFIND_TEXT lpFind,
#ifndef __APPLE__
	FINDCALLBACK pfnFindCallback=0);
#else
    FINDCALLBACK pfnFindCallback);
#endif

void WINAPI CAJFILE_ReleaseFindResult(LPFOUND_RESULT_ARRAY pArray);

LPFOUND_RESULT_ARRAYW WINAPI CAJFILE_FindAllTextW(
	CAJFILE hFile,
	LPFIND_TEXTW lpFind,
#ifndef __APPLE__
    FINDCALLBACKW pfnFindCallback=0);
#else
    FINDCALLBACKW pfnFindCallback);
#endif

void WINAPI CAJFILE_ReleaseFindResultW(LPFOUND_RESULT_ARRAYW pArray);
LPFOUND_RESULT_ARRAYW WINAPI CAJFILE_FindAllTextExW(
    CAJFILE hFile,
	LPFIND_TEXTEXW lpFind,
#ifndef __APPLE__
    FINDCALLBACKW pfnFindCallback=0);
#else
    FINDCALLBACKW pfnFindCallback);
#endif

//��ʾ����
enum CAJFILE_DISPLAY_QUALITY {
	DISPLAY_QUALITY_LOW=1,
	DISPLAY_QUALITY_NORMAL,
	DISPLAY_QUALITY_HIGH
};

void WINAPI CAJFILE_SetDisplayQuality(
	Gulong nQuality);

void WINAPI CAJFILE_SetBaseFont(
	const char* lpszFaceName);
void WINAPI CAJFILE_SetBaseFontEx(
	const char* lpszFaceName,
	int charset);

#define LF_FACE_SIZE 32
#define LF_FONTNAME_SIZE (LF_FACE_SIZE+14)

typedef struct {
	widechar faceName[LF_FACE_SIZE+1];
	int charset;
    unsigned short langid;
}FONT_LIST_ITEM;

typedef struct {
	widechar faceName[LF_FONTNAME_SIZE+1];
	int charset;
	unsigned short langid;
}FONT_LIST_ITEM1;

typedef struct {
	Gulong cbSize;
	Gulong nItem;
	FONT_LIST_ITEM items[1];
}FONT_LIST;

typedef struct {
	Gulong cbSize;
	Gulong nItem;
	FONT_LIST_ITEM1 items[1];
}FONT_LIST1;

FONT_LIST* WINAPI CAJFILE_GetFontList();
void WINAPI CAJFILE_ReleaseFontList(FONT_LIST* fontList);
FONT_LIST1* WINAPI CAJFILE_GetFontList1();
void WINAPI CAJFILE_SetBkColor(
	COLORREF crBk);
COLORREF WINAPI CAJFILE_GetBkColor();

void WINAPI CAJFILE_SetImagePrint(CAJBOOL bImagePrint,gint32 dpi,long height);
/*
*/
typedef Gulong PermissionType;
#define ptDenyAll 0x00000000
#define ptOkAll   0xFFFFFFFF
#define ptOkPrint 0x00000001
#define ptOkEdit  0x00000002
#define ptOkCopy  0x00000004
#define ptOkNotes 0x00000008
#define ptError   0x80000000

PermissionType WINAPI CAJFILE_GetPermissions(CAJFILE hFile);
PermissionType WINAPI CAJFILE_GetPermissionsEx(CAJFILE hFile);
typedef struct  
{
	Gulong cbSize;
	short okPrint;
	short okCopyText;
	short okCopyImage;
	short okSaveAs;

	short PrintDPI;
	short PrintPageCount;
	short CopyCharCount;
	short CopyImageDPI;
	
}PERMISSIONS_INFO;
CAJBOOL WINAPI CAJFILE_GetPermissionsEx2(CAJFILE hFile, PERMISSIONS_INFO *pInfo);

typedef enum {
	camRC4,
	camRC4128,
	camAES,
	camIDEA,
	camDES3,
	camRC5
}CRYPTALGORITHM;

typedef enum  {
	VM_NONE=-1,
	VM_PASSWORD,
	VM_CERTIFICATE,
	VM_CA
}VERIFICATION_METHOD;
typedef struct {
	Gulong cbSize;
	VERIFICATION_METHOD	vm;
	CRYPTALGORITHM cryptAlgorithm;
	PermissionType permFlags;
}ENCRYPT_INFO, *LPENCRYPT_INFO;
CAJBOOL WINAPI CAJFILE_GetEncryptInfo(CAJFILE hFile, LPENCRYPT_INFO pEncryptInfo);

CAJBOOL WINAPI CAJFILE_PackAppInfo(const char* lpszSource,const char* pAppInfo,int nAppLength);
CAJBOOL WINAPI CAJFILE_PackAppInfoW(const widechar* lpszSource,const char* pAppInfo,int nAppLength);

CAJBOOL WINAPI CAJFILE_GetAppInfo(const char* lpszSource, char* pAppInfo, guint32 *nAppLength);
CAJBOOL WINAPI CAJFILE_GetAppInfoW(const widechar* lpszSource, char* pAppInfo, guint32* nAppLength);
/*
typedef struct tagNoteItem {
	long cbSize;
	long nType;
	long nPage;
	COLORREF clrColor;
	char *pDescText;
	char *pData;  //
	void *pDisp;
	long nShowType;
}NOTE_ITEM, *LPNOTE_ITEM;

typedef struct tagNote {
	long cbSize;
	CAJBOOL bReadOnly;
	long nPageStart;
	long nUinit; // 1,percent
	char *pszAuthor;
	long nCount;
	LPNOTE_ITEM *ppItems;
}NOTE, *LPNOTE;
*/
/*
 *	Dependency with desktop color depth.
 */
CAJBOOL WINAPI CAJFILE_Miniature(const char* lpszFileName,const char* lpszSaveName,int iPageIndex,float fScale);
CAJBOOL WINAPI CAJFILE_MiniatureEx(CAJFILE hFile,const char* lpszSaveName,int iPageIndex,float fScale);
#ifdef _WIN32
HBITMAP WINAPI CAJFILE_MiniatureEx1(CAJFILE hFile,int iPageIndex,float fScale);
HANDLE WINAPI CAJFILE_MiniatureEx2(CAJFILE hFile,int iPageIndex,float fScale);
HANDLE WINAPI CAJFILE_MiniatureEx3(CAJFILE hFile,int iPageIndex,float fScale,int quality, int *w, int *h);
char* WINAPI CAJFILE_MiniatureEx4(CAJFILE hFile,int iPageIndex,float fScale,int quality, int *len);
#endif
/*
 *	Unit is point.
 */
/************************************************************************/
/* ȡ���ļ�������Ϣ                                             */
/* (�Ѿ��ϳ�,��CAJFILE_GetDocInfoEx(DIT_APP_INFO)����)                */
/************************************************************************/
int WINAPI CAJFILE_GetAppendantInfoLength(CAJFILE hFile);
CAJBOOL WINAPI CAJFILE_GetAppendantInfo(CAJFILE hFile, char* pBuf,int iBufLen);

typedef enum  {
	CATALOG,
	APPINFO
}CRD_CLASS;

typedef enum  {
	SYNC,
	ASYNC
}CRD_METHOD;
int WINAPI CAJFILE_PreloadData(CAJFILE hFile, CRD_CLASS _Type);
/*
 *	This method set current cipher that is used to decrypt TEB files.
 *  The key length is always 32 bytes.
 */
void WINAPI CAJFILE_SetKey(const char* key,int len);
#ifdef _WIN32
HANDLE WINAPI BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal,int bits=-1);
CAJBOOL WINAPI SaveBitmap(HBITMAP hBitmap, const char* lpFileName,int nBPC=-1);
CAJBOOL WINAPI SaveDIBEx(HANDLE hDib, const char* lpFileName);
HANDLE WINAPI ZoomDIB(HANDLE hSrc, int scaledWidth, int scaledHeight);
HANDLE WINAPI CreateDIB(guint32 dwWidth, guint32 dwHeight, unsigned short wBitCount);
HANDLE WINAPI DecodeImage(const char *pszImage);
HANDLE WINAPI DecodeImageFromBuffer(char *pImageData, int nImageDatalen);
#endif
char* WINAPI FindDIBBits(char* lpbi);
guint32 WINAPI DIBHeight(char* lpDIB);
guint32 WINAPI DIBWidth(char* lpDIB);
int WINAPI strhalf(char* pszBuf, CAJBOOL bTrimSpace);
CAJBOOL WINAPI KDH2PDFEx(const char* lpszSrcFile, const char* lpszDestFile,CAJBOOL bAddTrademark,int logi);

#define UNICODE_FILENAME	1
#define APPEND_TRADEMARK	2
//#define 
typedef struct {
	Gulong cbSize;
	Gulong ErrorCode;
	_FILE_TYPE FileType;
	Gulong uFlags;
	union {
		const widechar *pwszCAJFile;
		const char *pszCAJFile;
	};
	union {
		const widechar *pwszPDFFile;
		const char *pszPDFFile;
	};
	
}CAJ2PDF_PARAMS,*LPCAJ2PDF_PARAMS;

CAJBOOL WINAPI CAJ2PDF(LPCAJ2PDF_PARAMS lpop);
CAJBOOL WINAPI CAJ2PDF2(const char* lpszSrcFile, const char* lpszDestFile,CAJBOOL bAddTrademark,int logi);

typedef struct tagFixParam
{
	CAJBOOL bOnlyCheck;
	CAJBOOL bSaveFontBitmap;
	CAJBOOL bAddCharMap;
	char Path[MAX_PATH];
	char NewFile[MAX_PATH];
}FixParam;
/*
	0:�ļ���������
	1:������ȱ��CharCode2Unicodeӳ�����Ӽ�������
	2:����ȱ��CharCode2Unicodeӳ�����Ӽ�������
*/
int WINAPI FixPDFEmbeddedFont(const char* lpszFileName,FixParam *param);

typedef enum  {
	eic_add=1,
	eic_del,
	eic_mov,
	eic_rep
}EDIT_IMAGE_CMD;
/*
*/
typedef struct _TAG_IMAGE_PARAM {
	gint32 cbSize;			// �ṹ�Ĵ�С
	EDIT_IMAGE_CMD uCmd;
	gint32 nPageIndex;		// ���ͼƬ��ҳ�룬��0��ʼ
	gint32 nIndex;		// ͼ���˳�򣬳�����Χ��������һ��
	gint32 nCompType;			// ѹ����������ɫ֧��JBIG��JBIG2�����ɫ/�Ҷ�֧��JPEG��JPEG2000
	// Ĭ�Ϸֱ�ΪJBIG��JPEG��
	gint32 nQuality;			// ѹ������
	RECT rcBBox;			// ͼ����ҳ���λ�ã��԰�Ϊ��λ
	char szName[MAX_PATH];	// ��ӵ�ͼƬ�ļ�����֧��tiff,jpeg,bmp��ʽ
}IMAGE_PARAM,*LPIMAGE_PARAM;

int WINAPI CAJFILE_GetImageCount(CAJFILE hFile,int nPage);
typedef struct _TAG_IMAGE_SAVE_PARAM {
	gint32 nPage;
	gint32 nIndex;
	CAJBOOL bSaveAsFile;
	CAJBOOL bRot;
	union {
		const char* pszFileName;
#ifdef _WIN32
		HANDLE hDIB;
#endif
	};
}IMAGE_SAVE_PARAM, *LPIMAGE_SAVE_PARAM;
CAJBOOL WINAPI CAJFILE_SaveImage(CAJFILE hFile,LPIMAGE_SAVE_PARAM pSave);

typedef struct tagImage_Info {
	RECT rcPos;			//ͼ��λ��
	gint32 nBPC;
	gint32 nWidth;
	gint32 nHeight;
	CAJBOOL bRot;
}IMAGE_INFO,*LPIMAGE_INFO;
#ifdef _WIN32
typedef HGLOBAL (WINAPI* TIFF_DECODE)(const char *file);
#endif
int WINAPI CAJFILE_GetModified(CAJFILE hFile);
CAJBOOL WINAPI CAJFILE_GetImageInfo(CAJFILE hFile,int nPage,int nIndex, LPIMAGE_INFO pInfo);
CAJBOOL WINAPI CAJFILE_EditImage(CAJFILE hFile,LPIMAGE_PARAM lpImage);
CAJBOOL WINAPI CAJFILE_Save(CAJFILE hFile);

typedef struct _TAG_TEXT_LINE {
	gint32 nCharCount;		//�ַ�����
	gint32 ySize;				//
	gint32 yMax;
	gint32 orientation;		//�������,0-ˮƽ,1-��ֱ
	gint32 *xLeft;
	gint32 xSize;
	gint32 nSubLineCount;		//���и���
	gint32 nFirstLineIndex;
//add 12/25/2006
	union{
		char *pszText;
		widechar *pwszText;
	};
}TEXT_LINE,*LPTEXT_LINE;

typedef struct _TAG_TEXT_INFO {
	gint32 cbSize;
	gint32 nLineCount;
	LPTEXT_LINE *pLine;
	CAJBOOL bIsUnicode;
}TEXT_INFO,*LPTEXT_INFO;

CAJBOOL WINAPI CAJFILE_GetTexInfo(CAJFILE hFile,int nPageIndex,LPTEXT_INFO pTextInfo);
typedef enum  {
	ecc_add=1,
	ecc_del,
	ecc_mov
}EDIT_CHAR_CMD;
typedef struct _TAG_CHAR_EDIT {
	gint32 cbSize;
	Gushort c;
	EDIT_CHAR_CMD uCmd;
	gint32 nFirstLine;
	gint32 nCharIndex;
	gint32 nLineCount;
	gint32 nCharLastCount;
	//gint32 *LineIndex;
} CHAR_EDIT, *LPCHAR_EDIT;


CAJBOOL WINAPI CAJFILE_EditChar(CAJFILE hFile, int nPageIndex, LPCHAR_EDIT pChar);
void WINAPI CAJFILE_ReleaseTextInfo(LPTEXT_INFO pTextInfo);

/*
CAJBOOL WINAPI CAJFILE_AddTrademark(const char* lpszSrcFile, const char* lpszDestFile,
								 const char* lpszTrademark);
*/
CAJBOOL WINAPI ClipImage(const char* lpszSrcFile, const char* lpszDestFile,
					  LPRECT rcBBox);
#ifdef _WIN32
void WINAPI DIBPaintEx(HDC hDC,	LPRECT lpDCRect, HANDLE hdib);

CAJBOOL WINAPI CAJFILE_PackCatalogToFile(
	LPCTSTR lpszFileName,
	NH_CONTENT_HEADER *pContentHeader,
	NH_CONTENT_ITEM *pContentItem);
#endif
BYTE *WINAPI CAJFILE_GetCharGlyph(const char* lpszFaceName,int c, int size,int *m1);
void WINAPI CAJFILE_ReleaseGlyph(BYTE* pGlyph);

#ifdef _WIN32
#ifdef READEREX_EXPORTS
extern Gulong __declspec( dllexport )UWM_READER_MESSAGE;
#else
extern Gulong __declspec( dllimport )UWM_READER_MESSAGE;
#endif
#endif

#define NM_NOT_FOUND_COLLECTION		1
#define NM_REQURE_USER_PASSWORD		2
#define NM_NOT_FOUND_FONT			3
#define NM_NOT_FOUND_BUILDIN_FONT	4
#define NM_PAGE_LOAD				5
#define NM_CATALOG_DONE				6
#define NM_APPINFO_DONE				7
#define NM_DOWNLOAD_COMPLETE		8
#define NM_INTERNAL_FILE_LOAD		9
#ifdef _WIN32
void WINAPI CAJFILE_SetNotifyMessageHwnd(HWND hwnd);
void WINAPI CAJFILE_SetNotifyMessageThreadId(DWORD dwID);
#else
    typedef int (WINAPI *NOTIFYMESSAGE_CALLBACK)(int msg, void *param);
    void WINAPI CAJFILE_SetNotifyMessageCallback(NOTIFYMESSAGE_CALLBACK pfnCallback);
#endif

#define FONT_RENDER_MONO		0x01
#define FONT_RENDER_LCD			0x02
#define FONT_RENDER_LCD_V		0x04
#define FONT_RENDER_NO_HINTING	0x08
#define FONT_RENDER_NO_AUTOHINT	0x10
    
#ifdef __APPLE__
#define __POINTER__ unsigned long
#else
#define __POINTER__ unsigned int
#endif
void WINAPI CAJFILE_SetSysMetrics(const char* pFieldName, __POINTER__ Value);
int WINAPI CAJFILE_GetSysMetrics(const char* pFieldName, __POINTER__ *Value);

typedef struct _WITS_21_S72_PARA_FONT1
{
	short horzSize;
	short vertSize;
	short fontCode;
	short wtfontCode;
	short fontType;
	short Orientation; // 0 :normal, 1: vert; only apply to literal  data
	short Escapement;  // slant angle -15~15 degree
	short fontWeight; 
	short xBase;		// in pixel
	short yBase;
	short mathCode;
	short underLine;
	short rotate;
}WITS_21_S72_PARA_FONT1;

typedef struct tagCHAR_PARAM {
	char cchar[8];
	RECT rect;
	WITS_21_S72_PARA_FONT1 font;
}CHAR_PARAM,*LPCHAR_PARAM;

int WINAPI CAJFILE_GetFirstChar(CAJFILE hFile,int nPage);
void WINAPI CAJFILE_GetNextChar(CAJFILE hFile,int nPage,int *pos, LPCHAR_PARAM char_param);
CAJBOOL WINAPI CAJFILE_DistillPage(
const char* lpszSource,
const char* lpszPageRange,
const char* lpszSaveFile);
void* WINAPI CAJFILE_DistillPageEx(const char* lpszSource,const char* lpszPageRange,Gulong *pcchLen);

typedef struct {
	Gulong cbSize;

}WATERMARK_INFO, *LPWATERMARK_INFO;

typedef enum  {
	CT_ISSUER,
	CT_CER_FILE,
	CT_CER_MEMFILE,
	CT_PEM_FILE,
	CT_PFX_FILE
}CERT_TYPE;

typedef struct
{
	CERT_TYPE CT;
	union
	{
		char certFile[MAX_PATH];
		struct  
		{
			char *pCert;
			int CertLen;
		}Cert_Mem;
		struct
		{
			Guchar* Name;
			Gulong NameLen;
			Guchar* SerialNumber;
			Gulong SerialNumberLen;
		}Issuer;
	};
}CERT, *PCERT;

typedef struct
{
	Gulong cbSize;
	PermissionType permit;
	CRYPTALGORITHM crypt;
	union
	{
		struct
		{
			char ownerPasswd[33];
			char userPasswd[33];
		}pass;
		CERT Cert;
		struct 
		{
			char *pdata;
			int data_len;
			char* seed;
		}ca_info;
	};
	VERIFICATION_METHOD vm;
}DOC_SECURITY, *LPDOC_SECURITY;

typedef struct {
	const widechar *pText;
	const char *pAddText;
	const char *pAddTextWithSign;
	COLORREF crText;
	float FontSize;
}PUB_ADD_TEXT;
#define TP_FLAG_AUTO_TEXT_ADD	0x00000001	//�Զ�
#define TP_FLAG_FIND_ANY_WORD	0x00000002  //����һ���ؼ����ҵ�������TP_FLAG_AUTO_TEXT_ADD
typedef struct 
{
	Gulong uFlags;
	const char *pTrademark;
	Gulong nTrademarkLen;
	COLORREF crTrademark;
	float TrademarkCA;
	int nTextCount;
	PUB_ADD_TEXT *pTexts;
}TRADEMARK_AND_PUB;

typedef struct 
{
	Gulong cbSize;
	Gulong uFlags;
	int percent;
	int stream_min_size;
	int added;             //out,������ɺ󷵻�����˶��ٸ�
	const char* copyright; //����
	int bmplen;               //BMPͼƬ�ֽڳ���
	char* bmp;          //BMPͼƬ
}WATERMARKEX1;

#define DP_FLAG_TRADEMARK		0x00000001 //�Ƿ����ˮӡ
#define DP_FLAG_SAVETOFILE		0x00000002 //���浽�ļ���
#define DP_FLAG_TOPDF			0x00000004 //�Ƿ�ת��ΪPDF
#define DP_FLAG_TONH			0x00000008 //��CAJ��NHת��ʱ����NH
#define DP_FLAG_CLIENT_BUF		0x00000010 //ʹ�õ������ṩ�Ļ�����
#define DP_FLAG_CATALOG			0x00000020 //�Ƿ���Ŀ¼
#define DP_FLAG_USE_JPEG2000	0x00000040 //ʹ��JPEG2000����ѹ����ɫ,�Ҷ�ͼ
#define DP_FLAG_USE_JBIG2		0x00000080 //ʹ��JBIG2����ѹ����ɫͼ
#define DP_FLAG_COMPRESS_XREF		0x00000100 //ѹ��PDF�ļ��Ľ������ñ�
#define DP_FLAG_COMPRESS_COMMOBJ	0x00000200 //ѹ��PDF�ļ��ķ�ѹ������
//#define DP_FLAG_PARAM_CALLBACK	0x00000400 //pParam����ΪDISTILL_CALLBACK����
#define DP_FLAG_PARAM_TRADEMARK_AND_PUB	0x00000800 //pParam����Ϊˮӡ�ӳ���ʱ����ַ
#define DP_FLAG_MAKE_SAME_SIZE		0x00001000 //����CAJ/NH�ļ�ͼƬ��С��һ�µ����(�ױ�)
#define DP_FLAG_MAKE_SAME_SIZE_F	0x00002000 //ֻ�����һҳ
#define DP_FLAG_PARAM_UNAVAILABLE_PAGE	0x00004000 //pParam����Ϊ������ҳ��
#define DP_FLAG_UNAVAILABLE_PAGE_BLANK	0x00008000 //������ҳ�滻Ϊ�հ�
#define DP_FLAG_PARAM_TRADEMARK_EX		0x00010000 //pParam����Ϊˮӡ(XML)
#define DP_FLAG_PARAM_LOCAL_ENCRYPT		0x00020000 //pParam����Ϊ���ؼ��ܲ���
#define DP_FLAG_PARAM_EXTEND			0x00040000 //pParam����Ϊ��չ������ͨ��
#define DP_FLAG_TONEWCAJ				0x00080000 //��CAJ��NH�ļ�ת��Ϊ�µģ�֧��DRM��CAJ�ļ�
#define DP_FLAG_PARAM_WATERMARK_EX1		0x00100000 //pParam����Ϊˮӡ�ṹWATERMARKEX1

#define DP_FLAG_REMOVE_CNKIDOI			0x10000000 //ɾ��PDF�ļ��е�CNKI DOI


typedef struct 
{
	Gulong cbSize;
	int count;
	int *pagein;
	char* copyright; //����
	int bmplen;               //BMPͼƬ�ֽڳ���
	char* bmp;          //BMPͼƬ
}PICKUP_WATERMARKEX;

CAJBOOL WINAPI CAJFILE_PickupWatermark(const char* lpszSource, PICKUP_WATERMARKEX* pickup);
void WINAPI CAJFILE_ReleasePickupWatermark(PICKUP_WATERMARKEX* pickup);
//typedef void* (WINAPI *FSTREAM_OPEN)(const char* file, const char* mode);
//typedef gint32 (WINAPI *FSTREAM_READ)(void *stream, void *lpBuffer, gint32 Size);
typedef int (WINAPI *DISTILL_CALLBACK)(int nPageCount, int nCurPage); //��������0��ʾֹͣ��ǰת��
typedef struct {
	Gulong cbSize;					// Size of the DISTILL_PARAM2 structure.
	const char* lpszPageRange;		//ҳ�뷶Χ��Ϊ�ձ�ʾȫ��ҳ
	Gulong uFlags;
	const char* lpszSource;			//Դ�ļ���ʶ��
	const char* lpszExt;			//Դ�ļ���չ��������
	FSTREAM_OPEN pfnOpen;			//�ļ������ص�����,Ϊ�յ���Ĭ���ļ���д����
	FSTREAM_READ pfnRead;
	FSTREAM_SEEK pfnSeek;
	FSTREAM_TELL pfnTell;
	FSTREAM_EOF	 pfnEof;
	FSTREAM_CLOSE pfnClose;
	union {
		const char* lpszSaveFile;	//������ļ�
		struct {					//���浽�ڴ棬���ڴ治��Ҫ�����߷��䣬Ҳ����Ҫ�ͷ�
			void *lpBuffer;
			Gulong cchLen;
		} Buffer;
		struct {					//
			void *lpBuffer;
			Gulong cchBufSize;
			Gulong cchLen;
			void *pParam;
#ifdef __APPLE__
            void *(WINAPI *_Palloc)(void*, int);	// the pointer to allocator function
			void (WINAPI *_Pfree)(void*, void *);	// the pointer to free function
#else
			void *(WINAPI *_Palloc)(void*, size_t);	// the pointer to allocator function
			void (WINAPI *_Pfree)(void*, void *);	// the pointer to free function
#endif
		} ClientBuffer;
	};
}DISTILL_PARAM, *LPDISTILL_PARAM;
CAJBOOL WINAPI CAJFILE_DistillPageEx1(LPDISTILL_PARAM lpp);
typedef struct {
	Gulong cbSize;					// Size of the DISTILL_PARAM structure.
	const char* lpszPageRange;		//ҳ�뷶Χ��Ϊ�ձ�ʾȫ��ҳ
	Gulong uFlags;				
	const char* lpszSource;			//Դ�ļ���ʶ��
	const char* lpszExt;			//Դ�ļ���չ��������
	FSTREAM_OPEN pfnOpen;			//�ļ������ص�����,Ϊ�յ���Ĭ���ļ���д����
	FSTREAM_READ pfnRead;
	FSTREAM_SEEK pfnSeek;
	FSTREAM_TELL pfnTell;
	FSTREAM_EOF	 pfnEof;
	FSTREAM_CLOSE pfnClose;
	union {
		const char* lpszSaveFile;	//������ļ�
		struct {					//���浽�ڴ棬���ڴ治��Ҫ�����߷��䣬Ҳ����Ҫ�ͷ�
			void *lpBuffer;
			Gulong cchLen;
		} Buffer;
		struct {					//
			void *lpBuffer;
			Gulong cchBufSize;
			Gulong cchLen;
			void *pParam;
#ifdef __APPLE__
            void *(WINAPI *_Palloc)(void*, int);	// the pointer to allocator function
			void (WINAPI *_Pfree)(void*, void *);	// the pointer to free function
#else
			void *(WINAPI *_Palloc)(void*, size_t);	// the pointer to allocator function
			void (WINAPI *_Pfree)(void*, void *);	// the pointer to free function
#endif
		} ClientBuffer;
	};
	DISTILL_CALLBACK pfnCallBack;
	LPDOC_SECURITY pDocSec;
	//Gulong nParams;
	void *pParam;
}DISTILL_PARAM2, *LPDISTILL_PARAM2;

#define DP_PARAM_ANNOTS				1
#define DP_PARAM_TRADEMARK_EX		2
#define DP_PARAM_LOCAL_ENCRYPT		3

CAJBOOL WINAPI CAJFILE_DistillPageEx2(LPDISTILL_PARAM2 lpp);
void* WINAPI CAJFILE_DistillNewParam();
void WINAPI CAJFILE_DistillSetParam(void* param_stu, int type, void* param, int cb);
void WINAPI CAJFILE_DistillDeleteParam(void* param_stu);

typedef struct {
	Gulong cbSize;					// Size of the COMBINE_PARAM structure.
	Gulong uFlags;
	Gulong uDocType;
	FSTREAM_OPEN pfnOpen;			//�ļ������ص�����,Ϊ�յ���Ĭ���ļ���д����
	FSTREAM_READ pfnRead;
	FSTREAM_SEEK pfnSeek;
	FSTREAM_TELL pfnTell;
	FSTREAM_EOF	 pfnEof;
	FSTREAM_CLOSE pfnClose;
	union {
		const char* lpszSaveFile;	//������ļ�
		struct {					//���浽�ڴ棬���ڴ治��Ҫ�����߷��䣬Ҳ����Ҫ�ͷ�
			void *lpBuffer;
			Gulong cchLen;
		} Buffer;
		struct {					//
			void *lpBuffer;
			Gulong cchBufSize;
			Gulong cchLen;
			void *pParam;
#ifdef __APPLE__
            void *(WINAPI *_Palloc)(void*, int);	// the pointer to allocator function
			void (WINAPI *_Pfree)(void*, void *);	// the pointer to free function
#else
			void *(WINAPI *_Palloc)(void*, size_t);	// the pointer to allocator function
			void (WINAPI *_Pfree)(void*, void *);	// the pointer to free function
#endif
		} ClientBuffer;
	};
	DISTILL_CALLBACK pfnCallBack;
	LPDOC_SECURITY pDocSec;
	void *pParam;
}COMBINE_PARAM, *LPCOMBINE_PARAM;

#ifdef _WIN32
LPVOID WINAPI CAJFILE_NewCombine(LPCOMBINE_PARAM lpcp);

/*void WINAPI CAJFILE_SetCombine(
							   LPVOID pCombineData,
							   LPCTSTR lpszProperty,
							   LPVOID pPropertyData);
*/
int WINAPI CAJFILE_CombineAddFile(LPVOID pCombineData,  const char* lpszSource, const char* lpszExt, const char* lpszPageRange, int *nPage);
CAJBOOL WINAPI CAJFILE_Combine(LPVOID pCombineData);
void WINAPI CAJFILE_CombineRelease(LPVOID pCombineData);

CAJBOOL WINAPI CAJFILE_EncryptFile(const char *pstrSour,const char *pstrDest,const char *pszPassword,PermissionType pt);
CAJBOOL WINAPI CAJFILE_LinearizedFile(const char* lpszSource,const char* lpszSaveFile);
#endif
typedef struct {
	Gulong cbSize;
	gint32 bUnicode;
	union {
		const char* pszSrc;
		const widechar* pszSrcW;
	};
	union {
		const char* pszDest;
		const widechar* pszDestW;
	};
	const char *pszPassword;
	gint32 Encrypt;    //1 = encrypt, 0 = decrypt
	int passwordLen;
	gint32 BlockSize;   /*256, 512, 1024, 2048, 4096, etc*/
	CRYPTALGORITHM cryptAlgorithm; //only camRC4128, camIDEA, camDES3, camRC5
	
}CRYPTFILE_INFO, *LPCRYPTFILE_INFO;
CAJBOOL WINAPI CAJFILE_CryptFileEx(LPCRYPTFILE_INFO pEncrypt);

typedef enum  {
	ICT_DEFAULT,
	ICT_JBIG,
	ICT_JBIG2,
	ICT_JPEG,
	ICT_JPEG2
}IMAGE_C_TYPE;

void* WINAPI CAJFILE_NewPicPack(Gulong nDocType);
void WINAPI CAJFILE_SetCompressType(void* pPackData,int bit,IMAGE_C_TYPE nType);
void WINAPI CAJFILE_SetPack(
	void* pPackData,
	const char* lpszProperty,
	void* pPropertyData);

int WINAPI CAJFILE_PackAddPic(
	void* pPackData,
	const char* lpszSource);

CAJBOOL WINAPI CAJFILE_PackPic(
	void* pPackData,
	const char* lpszDestFile,
	int picQuality);

void WINAPI CAJFILE_PackRelease(void* pPackData);
CAJBOOL WINAPI CAJFILE_CheckFile(const char* pszFileName, CAJBOOL bQuick);
CAJBOOL WINAPI CAJFILE_CheckFile2(const char* pszFileName);
char* WINAPI MD5EncodeBuf1(unsigned char digest[16],const char* buf,int buflen);
char* WINAPI MD5EncodeFile1(unsigned char digest[16],const char* filename);
int WINAPI GB2BIG5( unsigned char * Str,int nLen );

int WINAPI Base64Encode2(const char* lpSrc,int srclen,char* lpszDst,int* iDetLen);
int WINAPI Base64Decode2(const char* lpSrc,char* lpszDst,int* iDetLen);
int WINAPI Base64Decode3(const char* lpSrc,char* lpszDst,int* iDetLen);
int WINAPI Base64Encode3(const char* lpSrc, int srclen, char* lpszDst,int* iDetLen);
int WINAPI IDEADecrypt1(char *data,char *key,int ibuflen);
int WINAPI IDEAEncrypt1(char *data,char *key,int ibuflen);
int WINAPI R5Decrypt1(char *data,char *key,int ibuflen);
int WINAPI R5Encrypt1(char *data,char *key,int ibuflen);
int WINAPI URLDecode1(const char* lpSrc,char* lpszDst,int iDetLen);
int WINAPI URLEncode1(const char* lpSrc,char* lpszDst,int iDetLen);


int WINAPI Compress1(const char *source, unsigned long sourceLen, char *dest,unsigned long *destLen,int nLevel);
int WINAPI UnCompress1(char *dest, unsigned long *destLen, const char *source,unsigned long sourceLen);
#ifndef _ZLIBAPI_H_
typedef void* HZIPFILE;
#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)

/* compression levels */
#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
typedef void* (WINAPI *ZIPSTREAM_OPEN)(const char* file, const char* mode);
typedef long (WINAPI *ZIPSTREAM_READ)(void *stream, void *lpBuffer, long Size);
typedef long (WINAPI *ZIPSTREAM_WRITE)(void *stream, const void *lpBuffer, long Size);
typedef long (WINAPI *ZIPSTREAM_TELL)(void *stream);
typedef long (WINAPI *ZIPSTREAM_SEEK)(void *stream, long Off, long Way);
typedef long (WINAPI *ZIPSTREAM_EOF)(void *stream);
typedef long (WINAPI *ZIPSTREAM_CLOSE)(void *stream);
#define ZIP_FLAG_EXT	0x00000001
typedef struct 
{
	const char* lpszSource;			//Դ�ļ���ʶ��
	const char* mode;
	guint32 flags;
	ZIPSTREAM_OPEN pfnOpen;			//�ļ������ص�����
	ZIPSTREAM_READ pfnRead;
	ZIPSTREAM_WRITE pfnWrite;
	ZIPSTREAM_SEEK pfnSeek;
	ZIPSTREAM_TELL pfnTell;
	ZIPSTREAM_EOF  pfnEof;
	ZIPSTREAM_CLOSE pfnClose;
	//guint32 local_magic;
}ZIP_OPEN_PARAMS;
typedef struct 
{
	guint32 magic;
	unsigned short version;
	unsigned short version_needed;
	unsigned short flag;
	unsigned short method;
	guint32 dosDate;
	guint32 crc;
	guint32 comprsize;
	guint32 uncomprsize;
	unsigned short filenamesize; 
	unsigned short internal_fa;
	guint32 external_fa;
	guint32 pos_local_header;
	char filename[MAX_PATH];
}zip_file_info;

#define TCALG_3DES		4
#define TCALG_RC5		5
#define TCALG_IDEA		3
#define TCALG_AES		2
#define TCALG_RC4		1
typedef guint32 TCALG_ID;
typedef void* HCRYPT;
HCRYPT WINAPI TCryptCreate(TCALG_ID id, const char* key, int keyLen);
int WINAPI TCryptDecrypt(HCRYPT cryptProv, unsigned char *data_in, int data_len, unsigned char* data_out);
int WINAPI TCryptEncrypt(HCRYPT cryptProv, unsigned char *data_in, int data_len, unsigned char* data_out);
void WINAPI TCryptDelete(HCRYPT cryptProv);
void WINAPI TCryptSetKey(HCRYPT cryptProv, const char* key, int keyLen);
unsigned long  WINAPI CalculateCRC32(unsigned long crc, const unsigned char *buf, unsigned int len);
#endif
HZIPFILE WINAPI ZipOpenEx1(ZIP_OPEN_PARAMS *zip);
HZIPFILE WINAPI ZipOpen1(const char* lpszFileName,int bForAdd);
void WINAPI ZipClose1(HZIPFILE zip);

int WINAPI ZipGetFileLength1(HZIPFILE zip, const char* file);
int WINAPI ZipReadFile1(HZIPFILE zip, const char* file, char* buf, int len);
int WINAPI ZipSaveFile1(HZIPFILE zip, const char* file, const char* saved);
int WINAPI ZipGoToFirstFile1(HZIPFILE zip);
int WINAPI ZipGotoNextFile1(HZIPFILE zip);
int WINAPI ZipGetCurrentFileName1(HZIPFILE zip, char* szFileName, int fileNameBufferSize);
int WINAPI ZipWriteCurrentFile1(HZIPFILE zip, char* buf, int len);
HZIPFILE WINAPI ZipOpenFile1(HZIPFILE zip, const char* file, int level, int repl);
void WINAPI ZipCloseCurrentFile1(HZIPFILE zip);
int WINAPI ZipGetCurrentFileInfo1(HZIPFILE zip, zip_file_info *fileinfo);
int WINAPI ZipGetFileInfo1(HZIPFILE zip, HZIPFILE file, zip_file_info *fileinfo);
int WINAPI ZipReadFileFromHandle1(HZIPFILE zip, HZIPFILE file, char* buf, int len);
int WINAPI ZipCloseFile1(HZIPFILE zip, HZIPFILE file);
int WINAPI ZipDeleteFile1(HZIPFILE zip, const char* file);
int WINAPI ZipRebuild(const char* zipfile);

CAJBOOL WINAPI CAJFILE_InternalFileIsReady(CAJFILE hFile, const char* file, CAJBOOL bQueued, unsigned int param1, unsigned int param2);
HZIPFILE WINAPI CAJFILE_InternalFileOpen(CAJFILE hFile, const char* file, guint32 uFlags);
int WINAPI CAJFILE_InternalFileLength(CAJFILE hFile, HZIPFILE hInternal);
gint32 WINAPI CAJFILE_InternalFileSeek(CAJFILE hFile, HZIPFILE hInternal, gint32 Off, gint32 Way);
gint32 WINAPI CAJFILE_InternalFileRead(CAJFILE hFile, HZIPFILE hInternal, void *Buffer, gint32 nread);
void WINAPI CAJFILE_InternalFileClose(CAJFILE hFile, HZIPFILE hInternal);

void *WINAPI FT_Init();
#ifdef _WIN32
void WINAPI FT_DrawText(void* ftEngine, HDC dc, int x, int y, const char* FontName, double size, const widechar *pszText, Gulong loadFlags);
#endif
void WINAPI FT_Done(void* ftEngine);

typedef struct
{
	Gulong dwMajorVersion;
	Gulong dwMinorVersion;
	Gulong dwRecVersion;
	Gulong dwBuildNumber;
}FILEVERSION,*LPFILEVERSION;

CAJBOOL WINAPI GetFileVersion1(const char* lpszFilename,LPFILEVERSION pVer);
int WINAPI EncodeImage( const void *lpBits,const BITMAPINFO *lpBitsInfo,const char *lpFileName,int quality );

CAJBOOL WINAPI CAJFILE_PackFile(const char *pstrSour,const char *pstrDest,int type);
CAJBOOL WINAPI CAJFILE_PDF2MLF(const char* lpszPDFFile,const char* lpszMLFFile);
CAJBOOL WINAPI CAJFILE_PackCatalog(const char* lpszFileName, LPOUTLINE_ITEM pItems);

//#define NP_FLAG_USE_CCITT4		0x00000020 
#define NP_FLAG_USE_JPEG2000	0x00000040 //ʹ��JPEG2000ѹ����ɫ�ĻҶ�ͼ
#define NP_FLAG_USE_JBIG2		0x00000080 //ʹ��JBIG2ѹ����ɫͼ
#define NP_FLAG_COMPRESS_XREF		0x00000100 //ѹ��PDF�ļ��Ľ������ñ�
#define NP_FLAG_COMPRESS_COMMOBJ	0x00000200 //ѹ��PDF�ļ��ķ�ѹ������
typedef struct {
	Gulong cbSize;		// Size of the NEW_PARAMS structure.
	CAJERROR hError;	// Error object handle.
	CAJ_DOCUMENT_TYPE DocType;  // DOC_CAJ or DOC_PDF
	_FILE_TYPE FileType;    // MEM_FILE or HARD_FILE
	union {
		char *FileBuffer;
		const char* FileName;
		const widechar* FileNameW;
		void* stream;
	};
	Gulong BufferLen;
	LPDOC_SECURITY lpSec;
	Gulong flags;
}NEW_PARAMS,*LPNEW_PARAMS;

CAJFILE WINAPI CAJE_NewDoc(LPNEW_PARAMS lpop);
CAJFILE WINAPI CAJE_OpenDoc(const char* pszFileName);
CAJFILE WINAPI CAJE_OpenDocW(const widechar* pszFileName);
CAJBOOL WINAPI CAJE_Close(CAJFILE hFile);

typedef struct {
	Gulong cbSize;
	const widechar* pszTitle;
	const widechar* pszAuthor;
	const widechar* pszSubject;
	const widechar* pszKeywords;
	const widechar* pszCreator;
	const widechar* pszProducer;
}DOC_INFO, *LPDOC_INFO;

typedef void* CAJPAGE;
CAJBOOL WINAPI CAJE_SetDocInfo(CAJFILE hFile, LPDOC_INFO lpInfo);

CAJBOOL WINAPI CAJE_SetSecurityClass(CAJFILE hFile, LPDOC_SECURITY lpSec);

CAJPAGE WINAPI CAJE_NewPage(CAJFILE hFile);
CAJPAGE WINAPI CAJE_GetPage(CAJFILE hFile, gint32 PageIndex);
CAJBOOL WINAPI CAJE_FinalizePage(CAJFILE hFile, CAJPAGE _Page);
gint32 WINAPI CAJE_GetPageCount(CAJFILE hFile);
//CAJBOOL CAJE_SetCurPage(CAJFILE hFile, gint32 Page, CAJBOOL bIsPageIndex);
typedef enum {
	NONE_SUBSET,
	ALWAYS_SUBSET,
	AUTO_SUBSET
}FONT_SUBSET;

typedef struct {
	Gulong cbSize;
	const char* pszFontName;
	double FontSize;
	Gulong charset;
	CAJBOOL Italic;
	CAJBOOL Bold;
	CAJBOOL Glyph;
	CAJBOOL UseDefault;
	FONT_SUBSET Embed;
	const char* pszReplaceFontName;
}DOC_FONT, *LPDOC_FONT;


typedef struct {
	Gulong cbSize;
	const char* pszFontName;
	double FontSize;
	widechar* ToUnicode;
	guint32 ToUnicodeStartChar;
	guint32 ToUnicodeEndChar;
	FONT_SUBSET Embed;  //ALWAYS_SUBSET
	char *FileBuffer;
	Gulong BufferLen;
}DOC_T1FONT, *LPDOC_T1FONT;
typedef struct {
	double x1;
	double y1;
	double x2;
	double y2;
}DOC_BOX, *LPDOC_BOX;

#define MEDIABOX	0
#define CROPBOX		1
#define BLEEDBOX	2
#define TRIMBOX		3
#define ARTBOX		4

typedef struct {
	Gulong cbSize;
	double x;
	double y;
	CAJBOOL IsUnicode;
	union {
		const char* pszText;
		const widechar* pszwText;
	};
	int cbString;
	const double* lpDx;
	Gulong Angle;
}TEXT_OUT, *LPTEXT_OUT;
typedef struct {
	Gulong cbSize;
	double x;
	double y;
	CAJBOOL IsUnicode;
	union {
		const char* pszText;
		const widechar* pszwText;
	};
	int cbString;
	const double* lpDx;
	Gulong Angle;
	struct {
		double a;
		double b;
		double c;
		double d;
	} Tm;
}TEXT_OUTEX, *LPTEXT_OUTEX;
void WINAPI CAJE_SetPageSize(CAJFILE hFile, CAJPAGE _Page, LPDOC_BOX _Box, short _Type);
CAJBOOL WINAPI CAJE_AddOutlines(CAJFILE hFile, LPOUTLINE_ITEM pItem);

CAJBOOL WINAPI CAJE_AddAnnot(CAJFILE hFile, CAJPAGE _Page, LPANNOT_ITEM* ppItem, int nCount);
//CAJBOOL CAJE_SetFont(CAJFILE hFile, CAJPAGE _Page, LPDOC_FONT _Font);
//CAJBOOL CAJE_TextOut(CAJFILE hFile, CAJPAGE _Page, LPTEXT_OUT _Text);

typedef enum  {
	TSD_TC,//Set character spacing
	TSD_TW,//Set word spacing
	TSD_TL,//Set text leading
	TSD_TZ,//Set horizontal scaling
	TSD_TR,//Set the text rendering mode
	TSD_TS,//Set text rise
	TSD_TF,//Set font and size
	TSD_TD,//Td
	TSD_TD1,//TD
	TSD_TM,
	TSD_TJ,
	TSD_TF1, //Set Font(Type1)
	TSD_TD2, // T*
	TSD_MTJ, //
	TSD_MTD, //Td 
	TSD_MTD1, //TD
	TSD_MTD2, //TD
}TEXT_STATE_ID;

CAJBOOL WINAPI CAJE_SetTextState(CAJFILE hFile, CAJPAGE _Page, TEXT_STATE_ID _TSD,...);

typedef enum  {
	ICM_NONE,
	ICM_JPEG,
	ICM_JPEG2000,
	ICM_CCITT3,
	ICM_CCITT4,
	ICM_LZW,
	ICM_JBIG,
	ICM_JBIG2,
	ICM_FLATE,
	ICM_JPEG1,
	ICM_NONE_NONE,
}ICM_TYPE;

typedef enum  {
	ICM_SRCCOPY,
	ICM_SRCAND,
	ICM_SRCINVERT
}ICM_ROP;
typedef enum  {
	/*PAT_BDIAGONAL,
	PAT_CROSS,
	PAT_DIAGCROSS,
	PAT_FDIAGONAL,
	PAT_HORIZONTAL,
	PAT_VERTICAL*/
	PAT_TILING,
	PAT_SHADING,
}PATTERN_TYPE;

#define DI_FLAG_ISFILE			0x00000001
#define DI_FLAG_FREEBUFFER		0x00000002
#define DI_FLAG_APPLY_DECODE	0x00000004
#define DI_FLAG_USER_CTM		0x00000010
typedef struct {
	gint32 cbSize;
	gint32 Quality;
	gint32 Width;
	gint32 Height;
	gint32 BitCount;
	gint32 SizeImage;
	gint32 XPelsPerMeter;
	gint32 YPelsPerMeter;
	ICM_TYPE Compression;
	gint32 SizePerLine;
	guint32 uFlags;
	union {
		const void*	Data;
		const char* FileName;
	};
	const void*	Pal;
	gint32 xSrc;
	gint32 ySrc;
	gint32 wSrc;
	gint32 hSrc;
	double xDest;
	double yDest;
	double wDest;
	double hDest;
	double Angle;
	double Decode[8];
} DOC_IMAGE, *LPDOC_IMAGE;
//CAJBOOL CAJE_SetImage(CAJFILE hFile, CAJPAGE _Page,LPDOC_IMAGE _Image);

typedef struct {
	gint32 PaintType;					//1 Colored tiling pattern. 2 Uncolored tiling pattern.
	gint32 TilingType;				//
	double XStep;
	double YStep;
	DOC_BOX BBox;
	double Matrix[6];
}TILING_PATTERN;


typedef struct {
	gint32 PaintType;					//1 Colored tiling pattern. 2 Uncolored tiling pattern.
	gint32 TilingType;				//
	double XStep;
	double YStep;
	DOC_BOX BBox;
	double Matrix[6];
}SHADING_PATTERN;

typedef struct {
	PATTERN_TYPE _Type;
	char Name[128];
	unsigned char r;
	unsigned char g;
	unsigned char b;
	union {
		TILING_PATTERN tpat;
		SHADING_PATTERN spat;
	};
}SF_COLORN;
typedef enum 
{
	CS_NONE,
	//DEVICE
	DeviceGray,
	DeviceRGB,
	DeviceCMYK,
	//CIE-BASED
	CalGray,
	CalRGB,
	Lab,
	ICCBased,
	//SPECIAL
	Indexed,
	_Pattern,
	Separation,
	DeviceN
}RE_COLOR_SPACE;

typedef struct {
	RE_COLOR_SPACE _Type;
	union {
		struct{
			double WhitePoint[3];
			double BlackPoint[3];
			double Gamma;
		}CalGray_;

		struct {
			double WhitePoint[3];
			double BlackPoint[3];
			double Gamma[3];
			double Matrix[9];
		}CalRGB_;

		struct {
			double WhitePoint[3];
			double BlackPoint[3];
			double Range[4];
		}Lab_;

		struct {
			gint32 N;
			RE_COLOR_SPACE Alternate[4];
			double Range[8];
			char *Metadata;
			gint32 MetadataLen;
		}ICCBased_;

		struct {
			RE_COLOR_SPACE Base;
			gint32 Count;
			char *Pal;
			gint32 PalLen;
		}Indexed_;

		
	};
}R_COLOR_SPACE;

/* Line cap values */
typedef enum {
	CAP_BUTT = 0,
	CAP_ROUND = 1,
	CAP_SQUARE = 2,
	CAP_UNKNOWN = 4
} GS_LINE_CAP;

#define gs_line_cap_max 3

/* Line join values */
typedef enum {
	JOIN_MITER = 0,
	JOIN_ROUND = 1,
	JOIN_BEVEL = 2,
	JOIN_UNKNOWN = 5
} GS_LINE_JOIN;

typedef struct {
	gint32 Start;
	gint32 Length;
	gint32 DashList[1];
}LINE_DASH;

typedef enum  {
	GSD_SAVE,
	GSD_RESTORE,
	GSD_CM,
	GSD_FLAT,
	GSD_LINECAP,
	GSD_LINEDASH,
	GSD_LINEJOIN,
	GSD_LINEWIDTH,
	GSD_MITERLIMIT,
	GSD_GS,
	GSD_FILLGRAY,
	GSD_STROKEGRAY,
	GSD_FILLCMYK,
	GSD_STROKECMYK,
	GSD_FILLRGB,
	GSD_STROKERGB,
	GSD_FILLCOLORSPACE,
	GSD_STROKECOLORSPACE,
	//Path
	GSD_MOVETO,
	GSD_LINETO,
	GSD_CURVETO, //c
	GSD_CURVETO1, //v
	GSD_CURVETO2,//y
	GSD_RECTANGLE,
	GSD_CLOSEPATH,
	GSD_NEWPATH,
	GSD_STROKE,
	GSD_CLOSESTROKE,
	GSD_FILL,
	GSD_EOFFILL,
	GSD_FILLSTROKE,
	GSD_CLOSEFILLSTROKE,
	GSD_EOFFILLSTROKE,
	GSD_CLOSEEOFFILLSTROKE,
	//Clip
	GSD_CLIP,
	GSD_EOFCLIP,
	//XObject
	GSD_IM,
	GSD_FILLCOLORN,
	GSD_STROKECOLORN,
	GSD_TRADMARK,
	GSD_FILLCOLORN1,
	GSD_STROKECOLORN1,
	GSD_END_PATTERN,
	GSD_CM1,
}GRAPHICS_STATE_ID;

CAJBOOL WINAPI CAJE_SetGraphicsState(CAJFILE hFile, CAJPAGE _Page, GRAPHICS_STATE_ID _GSD,...);

CAJBOOL WINAPI CAJFILE_CreateClientCert();
CAJBOOL WINAPI CAJFILE_GetClientCert(unsigned char *pbCertEncoded, gint32 *pcbCertEncoded);
CAJBOOL WINAPI CAJFILE_GetClientCertEx(unsigned char *pbCertEncoded, gint32 *pcbCertEncoded,
									unsigned char *pSerialNumber, gint32 cbSerialNumber,
									const char *pIssuer, const void *pvPara);

CAJBOOL WINAPI CAJFILE_CertPublicKeyCrypto(const BYTE *pbCertEncoded, DWORD cbCertEncoded, BYTE *pbData,
								   DWORD *pdwDataLen, DWORD dwBufLen, CAJBOOL bEncrypt);
CAJBOOL WINAPI CAJFILE_PackNHFile(const char *sour,const char*dest,int picQuality);

//CAJBOOL WINAPI EncryptTebSimple(unsigned char** cert, gint32* cert_len, int cert_num, const char *teb);

typedef struct{
	char DiskLetter;
	char DiskId[256];
}USB_DISK_ID;
int WINAPI GetUsbDiskInfo(USB_DISK_ID *diskinfo, int n);
#define TEB_FILE_ENCRYPTED				0x00000001		//ʹ��֤�����
#define TEB_FILE_ENCRYPT_USE_CERT		0x00000003		//ʹ��֤�����
#define TEB_FILE_ENCRYPT_USE_SERVER		0x00000005		//ʹ�÷���������, ��ʱ��Ҫ��ϵ������
#define TEB_FILE_ENCRYPT_USE_PKCS7		0x00000009		//ʹ��֤�����ʱ, ��Ϣ��װΪPKCS7��ʽ, Ĭ��ΪX509
//#define TEB_FILE_ENCRYPT_TEST			0x00000005		//
#define TEB_FILE_ENCRYPT_USE_USBID		0x00000011		//ʹ��USB_DISK_ID
#define TEB_FILE_ENCRYPT_IGNORE_PERMIT	0x00000020		//�����û������Ȩ��
#define TEB_FILE_ENCRYPT_2				0x00000040		//ʹ��TEB_FILE_ENCRYPT2�ṹ
#define TEB_FILE_ENCRYPT_3				0x00000080		//ʹ��TEB_FILE_ENCRYPT3�ṹ
#define TEB_FILE_ENCRYPT_USE_IPHONE_ID	0x00000101		//ʹ��IPAD/IPHONE APP ID
#define TEB_FILE_ENCRYPT_USE_ANDROID_ID	0x00000201		//ʹ��IPAD/IPHONE APP ID
typedef struct
{
	guint32 Flags;
	const char* fileid;					// �ļ�ID, �����, ���ڱ�ʶÿһ�����ص��ļ�
	union {
		struct
		{
			unsigned char** cert;		// ֤���б�, һ��ʹ��PEM��ʽ��X509֤��, ����ͬʱʹ�ö��
			gint32* cert_len;
			gint32 cert_num;
			
		} Cert;
		struct
		{
			unsigned char* cert;		// �û�֤��, ����Ϊ��, һ��ʹ��PEM��ʽ��X509֤��
			gint32 cert_len;
			const char *url;			// ��֤��������ַ(https://mytest.net/auth.aspx, ����ͨ��POST����, fileid=..&doi=...&ucert=...)
			const char *password;		// ʹ�÷���������ʱ������(>=16, ֻʹ��ǰ16���ֽ�)
		} Server;
		struct
		{
			unsigned char* usbid;		// usb disk id
			gint32 usbid_len;
			gint32 access_limit;		// �Ƿ���������
		} Usb;
	} Auth;
	const char *update_cert_url;	// ����֤�����õ�����
	const char *reg_machine_url;    // ע���������õ�����
	const char *userinfo;	// �û���Ϣ
	const char *organizationName;
	struct 
	{
	/************************************************************************/
	/*�û����Ȩ��                                                          */
	/************************************************************************/
		struct
		{
			CAJBOOL copyAllow;
			CAJBOOL copyText;
			CAJBOOL LimitCopyText;
			gint32 CopyTextCharCount;

			CAJBOOL copyImage;
			CAJBOOL LimitCopyImageDPI;
			gint32 CopyImageDPI;
		} Copy;
		struct
		{
			CAJBOOL printAllow;
			CAJBOOL LimitPrintDPI;
			gint32 PrintDPI;
			CAJBOOL LimitPrintPage;
			gint32 PrintPageCount;
		} Print;
		struct
		{
			CAJBOOL CanAddNote;
			CAJBOOL CanEditOrgNote;
		} Note;
		struct
		{
			const char *validBeginDate;
			const char *validEndDate;
			gint32 openTimes;
		} Valid;
		//gint32 reserved[1024];
	} Permit;
}TEB_FILE_ENCRYPT;

typedef struct 
{
	/************************************************************************/
	/*�û����Ȩ��                                                          */
	/************************************************************************/
	struct
	{
		CAJBOOL copyAllow;
		CAJBOOL copyText;
		CAJBOOL LimitCopyText;
		gint32 CopyTextCharCount;

		CAJBOOL copyImage;
		CAJBOOL LimitCopyImageDPI;
		gint32 CopyImageDPI;
	} Copy;
	struct
	{
		CAJBOOL printAllow;
		CAJBOOL LimitPrintDPI;
		gint32 PrintDPI;
		CAJBOOL LimitPrintPage;
		gint32 PrintPageCount;
	} Print;
	struct
	{
		CAJBOOL CanAddNote;
		CAJBOOL CanEditOrgNote;
	} Note;
	struct
	{
		const char *validBeginDate;
		const char *validEndDate;
		gint32 openTimes;
	} Valid;
	//gint32 reserved[1024];
} PERMIT_PARAM;
typedef struct
{
	guint32 Flags;
	const char* fileid;					// �ļ�ID, �����, ���ڱ�ʶÿһ�����ص��ļ�
	union {
		struct
		{
			unsigned char** cert;		// ֤���б�, һ��ʹ��PEM��ʽ��X509֤��, ����ͬʱʹ�ö��
			unsigned char** cert_id;	// ֤��id
			gint32* cert_len;
			gint32 cert_num;
		} Cert;
		struct
		{
			unsigned char* cert;		// �û�֤��, ����Ϊ��, һ��ʹ��PEM��ʽ��X509֤��
			gint32 cert_len;
			const char *url;			// ��֤��������ַ(https://mytest.net/auth.aspx, ����ͨ��POST����, fileid=..&doi=...&ucert=...)
			const char *password;		// ʹ�÷���������ʱ������(>=16, ֻʹ��ǰ16���ֽ�)
		} Server;
		struct
		{
			unsigned char* usbid;		// usb disk id
			gint32 usbid_len;
			gint32 access_limit;		// �Ƿ���������
		} Usb;
	} Auth;
	const char *update_cert_url;	// ����֤�����õ�����
	const char *reg_machine_url;    // ע���������õ�����
	const char *userinfo;	// �û���Ϣ
	const char *organizationName;
	PERMIT_PARAM Permit;
}TEB_FILE_ENCRYPT2;
typedef struct 
{
	guint32 AuthFlags;
	union {
		struct
		{
			unsigned char* cert;		// ֤���б�, һ��ʹ��PEM��ʽ��X509֤��, ����ͬʱʹ�ö��
			unsigned char* cert_id;	// ֤��id
			gint32 cert_len;
			int no_binding;
			unsigned char* enrypt_pfx;
			int enrypt_pfx_len;
		} Cert;
		struct
		{
			unsigned char* cert;		// �û�֤��, ����Ϊ��, һ��ʹ��PEM��ʽ��X509֤��
			gint32 cert_len;
			char *url;			// ��֤��������ַ(https://mytest.net/auth.aspx, ����ͨ��POST����, fileid=..&doi=...&ucert=...)
			char *password;		// ʹ�÷���������ʱ������(>=16, ֻʹ��ǰ16���ֽ�)
		} Server;
		struct
		{
			unsigned char* usbid;		// usb disk id
			gint32 usbid_len;
			gint32 access_limit;		// �Ƿ���������
		} Usb;
		struct
		{
			unsigned char* id;		// id
			gint32 id_len;
		} Iphone, Android;
	} Auth;
}AUTH_PARAM;

typedef struct
{
	guint32 Flags;
	const char* fileUniqueId;			// �ļ�Ψһ��ʶ
	const char* fileid;					// �ļ�ID, �����, ���ڱ�ʶÿһ�����ص��ļ�
	guint32 auth_num;
	AUTH_PARAM *auth;
	const char *update_cert_url[2];	// ����֤�����õ�����
	const char *reg_machine_url;    // ע���������õ�����
	const char *userinfo;	// �û���Ϣ
	const char *organizationName;
	PERMIT_PARAM Permit;
}TEB_FILE_ENCRYPT3;
#ifndef __APPLE__
//CAJBOOL WINAPI EncryptTebSimple(unsigned char** cert, gint32* cert_len, int cert_num, const char *teb, const char *fileid);
#endif
#ifdef _WIN32
int WINAPI EncryptRights(TEB_FILE_ENCRYPT& tebconf, const char* rights_file, int rights_file_len, char *new_rights_file, int new_rights_file_len);
int WINAPI EncryptRights1(TEB_FILE_ENCRYPT& tebconf, const char* rights_file, int rights_file_len, char *new_rights_file, int new_rights_file_len, char *passowrd, int *passwordLen);
#endif
int WINAPI EncryptRights2(TEB_FILE_ENCRYPT2* tebconf, const char* rights_file, int rights_file_len, char *new_rights_file, int new_rights_file_len, char *passowrd, int *passwordLen);
int WINAPI EncryptRights3(TEB_FILE_ENCRYPT3* tebconf, const char* rights_file, int rights_file_len, char *new_rights_file, int new_rights_file_len, char *passowrd, int *passwordLen);
CAJBOOL WINAPI EncryptTeb(TEB_FILE_ENCRYPT tebconf, const char *teb);
int WINAPI DecryptRights(const char* en, int len, HANDLE *phRights);
int WINAPI GetFileRights(const char* srcfile, char *rights, int rights_len);
typedef enum
{
	RP_PASSWORD,
	RP_USB_MATCH,
}RIGHTS_PARAM;
int WINAPI RightsGetParam(HANDLE hRights, RIGHTS_PARAM rp, void *buf, int buflen);
void WINAPI ReleaseRights(HANDLE hRights);

typedef enum
{
	CP_VERSION,
	CP_SERIAL_NUMBER,
	CP_ISSUER_NAME,
	CP_SUBJECT,
}CERT_PARAM;

void* WINAPI ParseCert(unsigned char* cert, int cert_len,  CAJBOOL x);
int WINAPI GetCertParam(void* cert, CERT_PARAM n, char *buf, int* buflen);
void WINAPI ReleaseCert(void* cert);
/*
������������֤��, �ɹ��򷵻�DER��ʽ��PKCS12��������֤��
��������Ҫʹ��LocalFree�ͷ�
*/
HLOCAL WINAPI CreatePKCS12(
	const char* friendlyName,				// [in]�üǵ�����, ��Ϊ��
	const char* countryName,				// [in]C, һ��ΪCN(�й�)
	const char* commonName,					// [in]CN
	const char* localityName,				// [in]L, ��Ϊ��
	const char* organizationName,			// [in]O
	const char* organizationalUnitName,		// [in]OU, ��Ϊ��
	const char* emailAddress,				// [in]emailAddress, ��Ϊ��
	const unsigned char* serial,			// [in]֤�����к�, ���Ϊ0���������
	int serialLen,							
	int days,								// [in]��Ч��, �Ӵ���ʱ��ʼ���������
	char *ca_cert,							// [in]CA֤��, PFX��ʽ, Ϊ���򴴽���ǩ��֤��
	int ca_cert_len,						// [in]CA֤�鳤��
	const char *ca_cert_pass,				// [in]CA֤��ı�������
	const char *pkcs12_pass,				// [in]�´�������֤��ı�������
	HLOCAL *hX509							// [out]�´�������֤���X509֤��(PEM��ʽ), ��������Ҫʹ��LocalFree�ͷ�
);
CAJBOOL WINAPI CertIsValid( unsigned char *cert, gint32 cert_len, gint32 type);
CAJBOOL WINAPI EncryptUseCert( unsigned char *cert, gint32 cert_len, unsigned char* from, unsigned char* to, gint32 *data_len, CAJBOOL bEncrypt);
CAJBOOL WINAPI EncryptUseCert1( unsigned char *cert, gint32 cert_len, unsigned char* from, unsigned char* to, gint32 *data_len, CAJBOOL bEncrypt);
CAJBOOL WINAPI EncryptUsePfx( char *pfx, gint32 pfx_len, const char* pass, unsigned char* from, unsigned char* to, gint32 *data_len, CAJBOOL bEncrypt);
CAJBOOL WINAPI DecryptUsePfx( char *pfx, gint32 pfx_len, const char* pass, unsigned char* from, unsigned char* to, gint32 *data_len);

CAJBOOL WINAPI EarseTebPage( const char *src, const char* dest, const char* pages, CAJBOOL bDelete, CAJBOOL bCopy);

#pragma pack(pop)
#ifndef _WIN32
#ifdef __cplusplus
};
#endif
#endif
#endif //#ifndef READER_H_
