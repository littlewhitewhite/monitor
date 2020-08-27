/*
���ߣ�  crazyd <crazydiamondzgy@hotmail.com>
���ڣ�  2002-12-29
˵��:   ��δ�����Ƕ��ʽ���д��,md5��Ϊ���ĵĲ����û��д��
		���кܸߵ������ٶ�,��C����콫������,
		ʹ�÷����ǰ��մ������MD5Init,MD5Update,MD5Final����������
		������ı�׼���룺

��׼���룺
	#include "md5.h"
	void main()
	{
		MD5_CTX context;
		
		MD5Init (&context);
		MD5Update (&context, "crazyd", strlen("crazyd"));
		MD5Final (&context);
	}

����˵����
	context.state[0]��32λ���޷������������������Ԫ��Ҳ��������
	�������ڴ�Ĵ�ź���ʾ�ķ������෴�ģ�����Ҫ�ر�ע�⡣
*/
#include <windows.h>
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <comutil.h>
#if !defined(_MD5_H_)
#define _MD5_H_

typedef struct _MD5_CTX{
	DWORD state[4];			//encypted message
	DWORD count[2];			//bits of plaintext
	unsigned char buffer[64];
}MD5_CTX;


#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define a esi
#define b edi
#define c edx
#define d ebx
#define tmp1 eax
#define tmp2 ecx

#define x(i) [x+4*i]

static void MD5Transform (DWORD [4], unsigned char [64]);
static void MD5TransformByte (DWORD [4], BYTE[64]);

static unsigned char PADDING[64] = 
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define FF(a, b, c, d, x, s, ac) \
	__asm mov tmp1,b \
	__asm and tmp1,c \
	__asm mov tmp2,b \
	__asm not tmp2 \
	__asm and tmp2,d \
	__asm or tmp2,tmp1 \
	__asm lea a,[tmp2+a+ac] \
	__asm add a,x \
	__asm rol a,s \
	__asm add a,b \

#define GG(a, b, c, d, x, s, ac) \
	__asm mov tmp1,b \
	__asm and tmp1,d \
	__asm mov tmp2,d \
	__asm not tmp2 \
	__asm and tmp2,c \
	__asm or tmp2,tmp1 \
	__asm lea a,[tmp2+a+ac] \
	__asm add a,x \
	__asm rol a,s \
	__asm add a,b 

#define HH(a,b,c, d, x, s, ac) \
	__asm mov tmp2,b \
	__asm xor tmp2,c \
	__asm xor tmp2,d \
	__asm lea a,[tmp2+a+ac] \
	__asm add a,x \
	__asm rol a,s \
	__asm add a,b

#define II(a, b, c, d, x, s, ac) \
	__asm mov tmp2,d \
	__asm not tmp2 \
	__asm or tmp2,b \
	__asm xor tmp2,c \
	__asm lea a,[tmp2+a+ac] \
	__asm add a,x \
	__asm rol a,s \
	__asm add a,b

class CmyMD5
{
public:
//  	CString MD5Trans(MD5_CTX contex)
//  	{
//  		unsigned char* p=(unsigned char*)&contex;
//  		CString str;
//  		for (int i=0;i<16;i++)
//  		{
//  			str+=trans(*p);
//  			p++;
//  		}
//  		return str;
//  	}

 	BSTR MD5Trans(MD5_CTX contex)
 	{
 		unsigned char* p=(unsigned char*)&contex;
 		char cMd5[120];
 		int ij=0;
		CString strTest;
 		for (int i=0;i<16;i++)
 		{
			strTest+=*p;
 			BSTR strTemp=trans(*p);
 			_bstr_t bstrTemp=strTemp;
 			char* cTemp=bstrTemp;
 			int j=0;
 			while (cTemp[j]!=0)
 			{
 				cMd5[ij++]=cTemp[j++];
 			}
 			p++;
 		}
 		cMd5[ij]='\0';
 		return _com_util::ConvertStringToBSTR(cMd5);
 	}

	void MD5Init (MD5_CTX *context)
	{
		context->count[0] = context->count[1] = 0;
		context->state[0] = 0x67452301;
		context->state[1] = 0xefcdab89;
		context->state[2] = 0x98badcfe;
		context->state[3] = 0x10325476;
	}
	void MD5Update (MD5_CTX *context, unsigned char *input, unsigned int inputLen)
	{
	    unsigned int i, index, partLen;
    	index = (unsigned int)((context->count[0] >> 3) & 0x3F);
	    if ((context->count[0] += ((DWORD)inputLen << 3)) < ((DWORD)inputLen << 3))
		    context->count[1]++;
	    context->count[1] += ((DWORD)inputLen >> 29);	
	    partLen = 64 - index;

	    if (inputLen >= partLen)
		{
		    CopyMemory(&context->buffer[index],input, partLen);
		    MD5Transform (context->state, context->buffer);
		
		    for (i = partLen; i + 63 < inputLen; i += 64)
			{
				  //MD5Transform (context->state, input);
			}
		    index = 0;
		}
    	else
		    i = 0;
	
	    /* Buffer remaining input */
	    CopyMemory(&context->buffer[index],&input[i],inputLen-i);
	}

	void MD5UpdateByte(MD5_CTX *context, BYTE*input, unsigned int inputLen)
	{ 
		//Compute number of bytes mod 64
		unsigned int i, index, partLen;
		index = (unsigned int)((context->count[0] >> 3) & 0x3F);
		if ((context->count[0] += ((DWORD)inputLen << 3)) < ((DWORD)inputLen << 3))
			context->count[1]++;
		context->count[1] += ((DWORD)inputLen >> 29);	
		partLen = 64 - index;

		if (inputLen >= partLen)
		{
			CopyMemory(&context->buffer[index],input, partLen);
			MD5TransformByte(context->state, context->buffer);

			for (i = partLen; i + 63 < inputLen; i += 64)
			{
				MD5TransformByte (context->state, &input[i]);
			}
			index = 0;
		}
		else
			i = 0;

		/* Buffer remaining input */
		CopyMemory(&context->buffer[index],&input[i],inputLen-i);
	}



    void MD5Final (MD5_CTX *context)
	{
    	unsigned char bits[8];
    	unsigned int index, padLen;
	
    	CopyMemory(bits,context->count,8);
	
    	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
    	padLen = (index < 56) ? (56 - index) : (120 - index);
    	MD5Update (context, PADDING, padLen);
    	MD5Update (context, bits, 8);

	}


	void MD5FinalByte (MD5_CTX *context)
	{
		BYTE bits[8];
		unsigned int index, padLen;

		CopyMemory(bits,context->count,8);

		index = (unsigned int)((context->count[0] >> 3) & 0x3f);
		padLen = (index < 56) ? (56 - index) : (120 - index);
		MD5UpdateByte(context, PADDING, padLen);
		MD5UpdateByte (context, bits, 8);

	}

	BSTR MD5TransByte(MD5_CTX contex)
	{
		BYTE* p=(BYTE*)&contex;
		char cMd5[120];
		int ij=0;
		CString strTest;
		for (int i=0;i<16;i++)
		{
			strTest+=*p;
			BSTR strTemp=trans(*p);
			_bstr_t bstrTemp=strTemp;
			char* cTemp=bstrTemp;
			int j=0;
			while (cTemp[j]!=0)
			{
				cMd5[ij++]=cTemp[j++];
			}
			p++;
		}
		cMd5[ij]='\0';
		return _com_util::ConvertStringToBSTR(cMd5);
	}

private:
//  	CString trans(int x)
//  	{
//  		char a[3]; 
//  		CString str;
//  		int i = 0, rem = 0;
//  		if (x <= 16) str = '0';
//  		do{
//  			rem = x%16;
//  			x = x/16;
//  			if(rem < 10) a[i] = 48 + rem;
//  			else a[i] = 55 + rem;
//  			i++;
//  		}while(x!=0);
//  		while(i>0) str += a[--i];
//  		return str;
//  	}

 	BSTR trans(int x)
 	{
 		char a[4]; 
		char b[4];
 
 		int i = 0, rem = 0,j=0;;
 		if (x <= 16) b[j++] = '0';
 		do{
 			rem = x%16;
 			x = x/16;
 			if(rem < 10) a[i] = 48 + rem;
 			else a[i] = 55 + rem;
 			i++;
 		}while(x!=0);

 		while(i>0) b[j++]= a[--i];
		b[j]='\0';
 		return _com_util::ConvertStringToBSTR(b);
 	}

    static void MD5Transform (DWORD state[4], unsigned char block[64])
	{
	    DWORD x[16];
    	__asm
		{
		    //initial
		    mov a,0x67452301
		    mov b,0xefcdab89
		    mov c,0x98badcfe
	    	mov d,0x10325476
	    	//copy string from block to state
	    	//���ǵ���API��Ӱ��Ĵ����������Լ�ʵ����һ���ڴ濽��
	    	push esi
	    	push edi
			
		    xor ecx,ecx
		    mov esi,dword ptr [block]
	    	lea edi,[x]
        ROLL:
		    mov eax,dword ptr [esi+ecx]
		    mov dword ptr [edi+ecx],eax
		    add ecx,4
		    cmp ecx,64
		    jb  ROLL
			
		    pop edi
	        pop esi
		}
	
	    /* Round 1 */
    	FF(a, b, c, d, x( 0), S11, 0xd76aa478); /* 1 */
    	FF(d, a, b, c, x( 1), S12, 0xe8c7b756); /* 2 */
    	FF(c, d, a, b, x( 2), S13, 0x242070db); /* 3 */
    	FF(b, c, d, a, x( 3), S14, 0xc1bdceee); /* 4 */
    	FF(a, b, c, d, x( 4), S11, 0xf57c0faf); /* 5 */
    	FF(d, a, b, c, x( 5), S12, 0x4787c62a); /* 6 */
    	FF(c, d, a, b, x( 6), S13, 0xa8304613); /* 7 */
    	FF(b, c, d, a, x( 7), S14, 0xfd469501); /* 8 */
    	FF(a, b, c, d, x( 8), S11, 0x698098d8); /* 9 */
    	FF(d, a, b, c, x( 9), S12, 0x8b44f7af); /* 10 */
    	FF(c, d, a, b, x(10), S13, 0xffff5bb1); /* 11 */
	    FF(b, c, d, a, x(11), S14, 0x895cd7be); /* 12 */
    	FF(a, b, c, d, x(12), S11, 0x6b901122); /* 13 */
    	FF(d, a, b, c, x(13), S12, 0xfd987193); /* 14 */
	    FF(c, d, a, b, x(14), S13, 0xa679438e); /* 15 */
    	FF(b, c, d, a, x(15), S14, 0x49b40821); /* 16 */
	
	    /* Round 2 */
	    GG (a, b, c, d, x( 1), S21, 0xf61e2562); /* 17 */
	    GG (d, a, b, c, x( 6), S22, 0xc040b340); /* 18 */
	    GG (c, d, a, b, x(11), S23, 0x265e5a51); /* 19 */
	    GG (b, c, d, a, x( 0), S24, 0xe9b6c7aa); /* 20 */
	    GG (a, b, c, d, x( 5), S21, 0xd62f105d); /* 21 */
	    GG (d, a, b, c, x(10), S22,  0x2441453); /* 22 */
	    GG (c, d, a, b, x(15), S23, 0xd8a1e681); /* 23 */
	    GG (b, c, d, a, x( 4), S24, 0xe7d3fbc8); /* 24 */
    	GG (a, b, c, d, x( 9), S21, 0x21e1cde6); /* 25 */
	    GG (d, a, b, c, x(14), S22, 0xc33707d6); /* 26 */
	    GG (c, d, a, b, x( 3), S23, 0xf4d50d87); /* 27 */
	    GG (b, c, d, a, x( 8), S24, 0x455a14ed); /* 28 */
     	GG (a, b, c, d, x(13), S21, 0xa9e3e905); /* 29 */
	    GG (d, a, b, c, x( 2), S22, 0xfcefa3f8); /* 30 */
	    GG (c, d, a, b, x( 7), S23, 0x676f02d9); /* 31 */
	    GG (b, c, d, a, x(12), S24, 0x8d2a4c8a); /* 32 */
	
	    /* Round 3 */
    	HH (a, b, c, d, x( 5), S31, 0xfffa3942); /* 33 */
     	HH (d, a, b, c, x( 8), S32, 0x8771f681); /* 34 */
    	HH (c, d, a, b, x(11), S33, 0x6d9d6122); /* 35 */
	    HH (b, c, d, a, x(14), S34, 0xfde5380c); /* 36 */
	    HH (a, b, c, d, x( 1), S31, 0xa4beea44); /* 37 */
	    HH (d, a, b, c, x( 4), S32, 0x4bdecfa9); /* 38 */
    	HH (c, d, a, b, x( 7), S33, 0xf6bb4b60); /* 39 */
    	HH (b, c, d, a, x(10), S34, 0xbebfbc70); /* 40 */
    	HH (a, b, c, d, x(13), S31, 0x289b7ec6); /* 41 */
    	HH (d, a, b, c, x( 0), S32, 0xeaa127fa); /* 42 */
    	HH (c, d, a, b, x( 3), S33, 0xd4ef3085); /* 43 */
    	HH (b, c, d, a, x( 6), S34,  0x4881d05); /* 44 */
    	HH (a, b, c, d, x( 9), S31, 0xd9d4d039); /* 45 */
    	HH (d, a, b, c, x(12), S32, 0xe6db99e5); /* 46 */
      	HH (c, d, a, b, x(15), S33, 0x1fa27cf8); /* 47 */
    	HH (b, c, d, a, x( 2), S34, 0xc4ac5665); /* 48 */
	
	    /* Round 4 */
	    II (a, b, c, d, x( 0), S41, 0xf4292244); /* 49 */
    	II (d, a, b, c, x( 7), S42, 0x432aff97); /* 50 */
     	II (c, d, a, b, x(14), S43, 0xab9423a7); /* 51 */
    	II (b, c, d, a, x( 5), S44, 0xfc93a039); /* 52 */
    	II (a, b, c, d, x(12), S41, 0x655b59c3); /* 53 */
    	II (d, a, b, c, x( 3), S42, 0x8f0ccc92); /* 54 */
    	II (c, d, a, b, x(10), S43, 0xffeff47d); /* 55 */
    	II (b, c, d, a, x( 1), S44, 0x85845dd1); /* 56 */
    	II (a, b, c, d, x( 8), S41, 0x6fa87e4f); /* 57 */
    	II (d, a, b, c, x(15), S42, 0xfe2ce6e0); /* 58 */
     	II (c, d, a, b, x( 6), S43, 0xa3014314); /* 59 */
    	II (b, c, d, a, x(13), S44, 0x4e0811a1); /* 60 */
    	II (a, b, c, d, x( 4), S41, 0xf7537e82); /* 61 */
    	II (d, a, b, c, x(11), S42, 0xbd3af235); /* 62 */
    	II (c, d, a, b, x( 2), S43, 0x2ad7d2bb); /* 63 */
    	II (b, c, d, a, x( 9), S44, 0xeb86d391); /* 64 */
	
    	__asm 
		{
		    mov tmp1,DWORD PTR [state]
		    add DWORD PTR [tmp1],a
		    add DWORD PTR [tmp1+4],b
		    add DWORD PTR [tmp1+8],c
		    add DWORD PTR [tmp1+12],d
		}
	}

	static void MD5TransformByte (DWORD state[4], BYTE block[64])
	{
		DWORD x[16];
		__asm
		{
			//initial
			   mov tmp1,DWORD PTR [state]
			    mov a,DWORD PTR [tmp1]
				mov b,DWORD PTR [tmp1+4]
				mov c,DWORD PTR [tmp1+8]
				mov d,DWORD PTR [tmp1+12]
	
				//copy string from block to state
				//���ǵ���API��Ӱ��Ĵ����������Լ�ʵ����һ���ڴ濽��
				push esi
				push edi

				xor ecx,ecx
				mov esi,dword ptr [block]
			lea edi,[x]
ROLL:
			mov eax,dword ptr [esi+ecx]
			mov dword ptr [edi+ecx],eax
				add ecx,4
				cmp ecx,64
				jb  ROLL

				pop edi
				pop esi
		}

		/* Round 1 */
		FF(a, b, c, d, x( 0), S11, 0xd76aa478); /* 1 */
		FF(d, a, b, c, x( 1), S12, 0xe8c7b756); /* 2 */
		FF(c, d, a, b, x( 2), S13, 0x242070db); /* 3 */
		FF(b, c, d, a, x( 3), S14, 0xc1bdceee); /* 4 */
		FF(a, b, c, d, x( 4), S11, 0xf57c0faf); /* 5 */
		FF(d, a, b, c, x( 5), S12, 0x4787c62a); /* 6 */
		FF(c, d, a, b, x( 6), S13, 0xa8304613); /* 7 */
		FF(b, c, d, a, x( 7), S14, 0xfd469501); /* 8 */
		FF(a, b, c, d, x( 8), S11, 0x698098d8); /* 9 */
		FF(d, a, b, c, x( 9), S12, 0x8b44f7af); /* 10 */
		FF(c, d, a, b, x(10), S13, 0xffff5bb1); /* 11 */
		FF(b, c, d, a, x(11), S14, 0x895cd7be); /* 12 */
		FF(a, b, c, d, x(12), S11, 0x6b901122); /* 13 */
		FF(d, a, b, c, x(13), S12, 0xfd987193); /* 14 */
		FF(c, d, a, b, x(14), S13, 0xa679438e); /* 15 */
		FF(b, c, d, a, x(15), S14, 0x49b40821); /* 16 */

		/* Round 2 */
		GG (a, b, c, d, x( 1), S21, 0xf61e2562); /* 17 */
		GG (d, a, b, c, x( 6), S22, 0xc040b340); /* 18 */
		GG (c, d, a, b, x(11), S23, 0x265e5a51); /* 19 */
		GG (b, c, d, a, x( 0), S24, 0xe9b6c7aa); /* 20 */
		GG (a, b, c, d, x( 5), S21, 0xd62f105d); /* 21 */
		GG (d, a, b, c, x(10), S22,  0x2441453); /* 22 */
		GG (c, d, a, b, x(15), S23, 0xd8a1e681); /* 23 */
		GG (b, c, d, a, x( 4), S24, 0xe7d3fbc8); /* 24 */
		GG (a, b, c, d, x( 9), S21, 0x21e1cde6); /* 25 */
		GG (d, a, b, c, x(14), S22, 0xc33707d6); /* 26 */
		GG (c, d, a, b, x( 3), S23, 0xf4d50d87); /* 27 */
		GG (b, c, d, a, x( 8), S24, 0x455a14ed); /* 28 */
		GG (a, b, c, d, x(13), S21, 0xa9e3e905); /* 29 */
		GG (d, a, b, c, x( 2), S22, 0xfcefa3f8); /* 30 */
		GG (c, d, a, b, x( 7), S23, 0x676f02d9); /* 31 */
		GG (b, c, d, a, x(12), S24, 0x8d2a4c8a); /* 32 */

		/* Round 3 */
		HH (a, b, c, d, x( 5), S31, 0xfffa3942); /* 33 */
		HH (d, a, b, c, x( 8), S32, 0x8771f681); /* 34 */
		HH (c, d, a, b, x(11), S33, 0x6d9d6122); /* 35 */
		HH (b, c, d, a, x(14), S34, 0xfde5380c); /* 36 */
		HH (a, b, c, d, x( 1), S31, 0xa4beea44); /* 37 */
		HH (d, a, b, c, x( 4), S32, 0x4bdecfa9); /* 38 */
		HH (c, d, a, b, x( 7), S33, 0xf6bb4b60); /* 39 */
		HH (b, c, d, a, x(10), S34, 0xbebfbc70); /* 40 */
		HH (a, b, c, d, x(13), S31, 0x289b7ec6); /* 41 */
		HH (d, a, b, c, x( 0), S32, 0xeaa127fa); /* 42 */
		HH (c, d, a, b, x( 3), S33, 0xd4ef3085); /* 43 */
		HH (b, c, d, a, x( 6), S34,  0x4881d05); /* 44 */
		HH (a, b, c, d, x( 9), S31, 0xd9d4d039); /* 45 */
		HH (d, a, b, c, x(12), S32, 0xe6db99e5); /* 46 */
		HH (c, d, a, b, x(15), S33, 0x1fa27cf8); /* 47 */
		HH (b, c, d, a, x( 2), S34, 0xc4ac5665); /* 48 */

		/* Round 4 */
		II (a, b, c, d, x( 0), S41, 0xf4292244); /* 49 */
		II (d, a, b, c, x( 7), S42, 0x432aff97); /* 50 */
		II (c, d, a, b, x(14), S43, 0xab9423a7); /* 51 */
		II (b, c, d, a, x( 5), S44, 0xfc93a039); /* 52 */
		II (a, b, c, d, x(12), S41, 0x655b59c3); /* 53 */
		II (d, a, b, c, x( 3), S42, 0x8f0ccc92); /* 54 */
		II (c, d, a, b, x(10), S43, 0xffeff47d); /* 55 */
		II (b, c, d, a, x( 1), S44, 0x85845dd1); /* 56 */
		II (a, b, c, d, x( 8), S41, 0x6fa87e4f); /* 57 */
		II (d, a, b, c, x(15), S42, 0xfe2ce6e0); /* 58 */
		II (c, d, a, b, x( 6), S43, 0xa3014314); /* 59 */
		II (b, c, d, a, x(13), S44, 0x4e0811a1); /* 60 */
		II (a, b, c, d, x( 4), S41, 0xf7537e82); /* 61 */
		II (d, a, b, c, x(11), S42, 0xbd3af235); /* 62 */
		II (c, d, a, b, x( 2), S43, 0x2ad7d2bb); /* 63 */
		II (b, c, d, a, x( 9), S44, 0xeb86d391); /* 64 */

		__asm 
		{
				mov tmp1,DWORD PTR [state]
				add DWORD PTR [tmp1],a
				add DWORD PTR [tmp1+4],b
				add DWORD PTR [tmp1+8],c
				add DWORD PTR [tmp1+12],d
		}

	}

protected:
private:
};

# endif