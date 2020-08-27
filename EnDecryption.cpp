#include "stdafx.h"
#include "EnDecryption.h"



char cShowValues[62]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S',
'T','U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
'w','x','y','z'};

 
void CString2Char(CString str, char ch[])
{
	int i;
	char *tmpch;

#ifndef UNICODE
	tmpch=str.GetBuffer(str.GetLength());
#else
	int wLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);//�õ�Char�ĳ���
	tmpch = new char[wLen + 1];                                             //��������ĵ�ַ��С
	WideCharToMultiByte(CP_ACP, 0, str, -1, tmpch, wLen, NULL, NULL);       //��CStringת����char*
#endif

	for(i = 0; tmpch[i] != '\0'; i++) ch[i] = tmpch[i];
	ch[i] = '\0';
	delete tmpch;
}


void GetRegistNo(CString strMac,CString& strRegistNo)
{
	int i=0;
	char lpszMac[40];
	CString2Char(strMac,lpszMac);

	while (lpszMac[i]!=0)i++;
	lpszMac[i++]='i';
	lpszMac[i++]='k';
	lpszMac[i++]='n';
	lpszMac[i++]='c';
	lpszMac[i++]='\0';

	char cShowValues[32]={'A','B','D','E','F','G','H','J','L','M','O','P','Q','R','S',
		'T','U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9'};



	MD5_CTX contex;
	unsigned char* p=(unsigned char*)&contex;

	CmyMD5 md5;
	md5.MD5Init(&contex);
	md5.MD5Update(&contex,(unsigned char*)lpszMac,strlen(lpszMac));
	md5.MD5Final(&contex);	
	CString strMd5=md5.MD5Trans(contex);

	int*piValue=new int[strMd5.GetLength()];

	for (int i=0;i<strMd5.GetLength();i++)
	{

		CString strTemp=strMd5.Mid(i,1);
		if   (strTemp==_T("A") || strTemp==_T("a"))  piValue[i]= 10;   
		else if   (strTemp==_T("B")   ||   strTemp ==_T("b"))   piValue[i]   =11;   
		else if   (strTemp==_T("C")  ||   strTemp==_T("c"))   piValue[i]   =12;   
		else if   (strTemp==_T("D")   ||   strTemp==_T("d"))   piValue[i]   =13;   
		else if   (strTemp==_T("E")   ||   strTemp==_T("e"))   piValue[i]   =14;   
		else if   (strTemp==_T("F")  ||   strTemp==_T("f"))   piValue[i]   =15;   
		else   /*sscanf_s(&cMd5[i], "%d",&piValue[i],1);   */  
			piValue[i]=_ttoi(strTemp);

	}
	int iShowValue[26];
	for (int j=0;j<24;j=j+4)
	{
		int i=j/4+j;
		iShowValue[j]=piValue[i]*2+piValue[i+1]/8;
		iShowValue[j+1]=piValue[i+1]*4%32+piValue[i+2]/4;
		iShowValue[j+2]=piValue[i+2]*8%32+piValue[i+3]/2;
		iShowValue[j+3]=piValue[i+3]*16%32+piValue[i+4];
	}
	iShowValue[25]=piValue[strMd5.GetLength()-1];

	delete piValue;

	char cRegistNo[31];
	for (int i=0;i<29;i++)
	{
		if ((i+1)%5==0)
		{
			cRegistNo[i]='-';
		}
		else
		{
			cRegistNo[i]=cShowValues[iShowValue[i-i/5]];
		}

	}
	cRegistNo[29]=cShowValues[25];
	cRegistNo[30]='\0';

	strRegistNo=CString(cRegistNo);
}

void GeneratePassNo(CString& strPassNo)
{
	srand( (unsigned)time( NULL ) ); 

	/* Display 10 numbers. */ 
	char cPass[24];
	for( int i = 0; i < 23;i++ ) 
	{
		if ((i+1)%6==0)
		{
			cPass[i]='-';
		}
		else
		{
			cPass[i]=cShowValues[rand()%36];
		}
	}
	cPass[23]='\0';
	strPassNo=CString(cPass);
}

int FindSameItemNo(char cFind)
{

	for(int pos=0;pos<62;pos++)
	{
		if(cShowValues[pos]==cFind)
		{
			return pos;

		}

	}
	return -1;
}




void GetCpu1ID(CString strID)
{
	long s1, s2;
	__asm
	{
		Mov eax,0x1
			xor edx,edx
			cpuid
			mov s1,edx
			mov s2,eax
	}
	strID.Format(_T("%08x%08x"), s1, s2);
}
void Encryption(CString strResRegistNo,CString strResPass,CString strResDate,CString strResMaxLink,CString& strDir)
{
	int key1=59;
	int key2=34;

	int key3=43;
	int key4=23;

	int key11=41;
	int key33=13;
	char cResRegistNo[40];

	UINT nItemCount=0;
	CString2Char(strResRegistNo,cResRegistNo);
	for(int i=0;i<strResRegistNo.GetLength();i++)
	{
		if (cResRegistNo[i]!='-')
		{
			int iPos=FindSameItemNo(cResRegistNo[i]);
			if (iPos>=0)
			{
				iPos=iPos*key1+key2;
				iPos=iPos%62;
				strDir+=CString(cShowValues[iPos]);
				nItemCount++;
			}

		}

	}

	char cResMaxLink[4];

	CString2Char(strResMaxLink,cResMaxLink);
	for(int i=0;i<strResMaxLink.GetLength();i++)
	{

		int iPos=FindSameItemNo(cResMaxLink[i]);
		if (iPos>=0)
		{
			iPos=iPos*key1+key2;
			iPos=iPos%62;
			strDir+=CString(cShowValues[iPos]);
			nItemCount++;
		}

	}

	// 	//Cpuid ���� ע����
	// 	CString strCpuId;
	// 	GetCpu1ID(strCpuId);
	// 	char cCpuId[40];
	// 	CString2Char(strCpuId,cCpuId);
	// 	for (int i=0;i<strResRegistNo.GetLength()&& i<strCpuId.GetLength();i++)
	// 	{
	// 		if (cResRegistNo[i]!='-')
	// 		{
	//           cResRegistNo[i]=cResRegistNo[i]^cCpuId[i];
	// 		}
	// 	}


	CString strTempAddPass,strTempAddPass1;
	if (strResPass.GetLength()<=0 || strResPass.GetLength()>=100)
	{
		return;
	}
	else
	{
		if (strResPass.GetLength()<10)
		{
			strTempAddPass=_T("0");
		}
		strTempAddPass.AppendFormat(_T("%d"),strResPass.GetLength());
	}

	UINT nCountSpecail=0;
	for(int i=0;i<strResPass.GetLength();i++)
	{
		if (strResPass.Mid(i,1)==_T("-"))
		{
			nCountSpecail++;
			if (i<10)
			{
				strTempAddPass1+=_T("0");
			}

			strTempAddPass1.AppendFormat(_T("%d"),i);
		}
	}
	if (nCountSpecail<10)
	{
		strTempAddPass+=_T("0");
	}
	strTempAddPass.AppendFormat(_T("%d"),nCountSpecail);

	char cResPass[40];
	strResPass=strTempAddPass+strTempAddPass1+strResPass;
	CString2Char(strResPass,cResPass);
	int j=0;
	for(int i=0;i<strResPass.GetLength();i++)
	{
		if (cResPass[i]!='-')
		{
			// 			while(j<strResRegistNo.GetLength())
			// 			{
			// 				if (cResRegistNo[j]!='-')
			// 				{
			// 					//cResPass[i]=cResPass[i]^cResRegistNo[j];
			// 					break;
			// 				}
			// 				j++;
			// 			}
			while(cResRegistNo[j]=='-')
			{
				j++;
				if (j>=strResRegistNo.GetLength())
				{
					j=0;
				}
			}

			int iPosNo=FindSameItemNo(cResRegistNo[j]);

			int iPosPass=FindSameItemNo(cResPass[i]);
			if (iPosNo>=0&&iPosPass>=0)
			{
				if (int(iPosPass^iPosNo)<=61)
				{
					iPosPass=iPosPass^iPosNo;
				}
			}


			iPosPass=iPosPass*key3+key4;
			iPosPass=iPosPass%62;
			strDir+=CString(cShowValues[iPosPass]);
			nItemCount++;

		}
	}



	char cResDate[40];
	CString2Char(strResDate,cResDate);
	//������Ч��
	for (int i=0;i<strResDate.GetLength();i++)
	{
		if (cResPass[i%strResPass.GetLength()]!='-')
		{
			int iPosNo=FindSameItemNo(cResDate[i]);

			int iPosPass=FindSameItemNo(cResPass[i%strResPass.GetLength()]);
			if (iPosNo>=0&&iPosPass>=0)
			{
				if (int(iPosPass^iPosNo)<=61)
				{
					iPosNo=iPosPass^iPosNo;
				}
			}
			cResDate[i]=cShowValues[iPosNo];
		}

	}

	for(int i=0;i<strResDate.GetLength();i++)
	{
		int iPos=FindSameItemNo(cResDate[i]);
		if (iPos>=0)
		{
			iPos=iPos*key1+key2;
			iPos=iPos%62;
			strDir+=CString(cShowValues[iPos]);
			nItemCount++;
		}

	}
}

void Encryption(CString strResRegistNo,CString strResPass,CString strResDate,CString& strDir)
{
	int key1=59;
	int key2=34;

	int key3=43;
	int key4=23;

	int key11=41;
	int key33=13;
	char cResRegistNo[40];

	UINT nItemCount=0;
	CString2Char(strResRegistNo,cResRegistNo);
	for(int i=0;i<strResRegistNo.GetLength();i++)
	{
		if (cResRegistNo[i]!='-')
		{
			int iPos=FindSameItemNo(cResRegistNo[i]);
			if (iPos>=0)
			{
				iPos=iPos*key1+key2;
				iPos=iPos%62;
				strDir+=CString(cShowValues[iPos]);
				nItemCount++;
			}

		}

	}

	if (nItemCount==0)
	{
		AfxMessageBox(_T("ע��Ŵ���"));
		return;
	}

	// 	//Cpuid ���� ע����
	// 	CString strCpuId;
	// 	GetCpu1ID(strCpuId);
	// 	char cCpuId[40];
	// 	CString2Char(strCpuId,cCpuId);
	// 	for (int i=0;i<strResRegistNo.GetLength()&& i<strCpuId.GetLength();i++)
	// 	{
	// 		if (cResRegistNo[i]!='-')
	// 		{
	//           cResRegistNo[i]=cResRegistNo[i]^cCpuId[i];
	// 		}
	// 	}

	char cResPass[40];

	CString2Char(strResPass,cResPass);
	int j=0;
	for(int i=0;i<strResPass.GetLength();i++)
	{
		if (cResPass[i]!='-')
		{
			// 			while(j<strResRegistNo.GetLength())
			// 			{
			// 				if (cResRegistNo[j]!='-')
			// 				{
			// 					//cResPass[i]=cResPass[i]^cResRegistNo[j];
			// 					break;
			// 				}
			// 				j++;
			// 			}
			while(cResRegistNo[j]=='-')
			{
				j++;
				if (j>=strResRegistNo.GetLength())
				{
					j=0;
				}
			}

			int iPosNo=-1;
			if (j<strResPass.GetLength())
			{
				iPosNo=FindSameItemNo(cResRegistNo[j]);
			}

			int iPosPass=FindSameItemNo(cResPass[i]);
			if (iPosNo>=0&&iPosPass>=0)
			{
				if (int(iPosPass^iPosNo)<=61)
				{
					iPosPass=iPosPass^iPosNo;
				}
			}


			iPosPass=iPosPass*key3+key4;
			iPosPass=iPosPass%62;
			strDir+=CString(cShowValues[iPosPass]);
			nItemCount++;

		}
	}



	char cResDate[40];
	CString2Char(strResDate,cResDate);
	//������Ч��
	for (int i=0;i<strResDate.GetLength()&& i<strResPass.GetLength();i++)
	{
		if (cResPass[i]!='-')
		{
			int iPosNo=FindSameItemNo(cResDate[i]);

			int iPosPass=FindSameItemNo(cResPass[i]);
			if (iPosNo>=0&&iPosPass>=0)
			{
				if (int(iPosPass^iPosNo)<=61)
				{
					iPosNo=iPosPass^iPosNo;
				}
			}
			cResDate[i]=cShowValues[iPosNo];
		}

	}

	for(int i=0;i<strResDate.GetLength();i++)
	{
		int iPos=FindSameItemNo(cResDate[i]);
		if (iPos>=0)
		{
			iPos=iPos*key1+key2;
			iPos=iPos%62;
			strDir+=CString(cShowValues[iPos]);
			nItemCount++;
		}

	}
}

void DecryptionRegist(CString strResSrialNo,CString &strDirRegistNo)
{
	int key2=34;
	int key11=41;
	CString strTemp;
	strTemp=strResSrialNo.Left(25);

	char cResRegistNo[40];
	char cResRegistNoShow[40];//��������
	CString2Char(strTemp,cResRegistNo);
	int j=0;
	for(int i=0;i<strTemp.GetLength();i++)
	{
		int iPosPass=FindSameItemNo(cResRegistNo[i]);
		iPosPass=key11*(iPosPass-key2);
		iPosPass=iPosPass%62;
		while(iPosPass<0)iPosPass+=62;
		cResRegistNoShow[j++]=cShowValues[iPosPass];
		if ((j+1)%5==0&&i<strTemp.GetLength()-2)
		{
			cResRegistNoShow[j++]='-';
		}

	}
	cResRegistNoShow[j]='\0';

	strDirRegistNo=CString(cResRegistNoShow);

}

void DecryptionMaxLink(CString strResSrialNo,CString strDirRegistNo,CString& strMaxLink)
{
	int key2=34;
	int key11=41;

	int iGapNoCount=0;
	char cResRegistNo[40];
	CString2Char(strDirRegistNo,cResRegistNo);
	for(int i=0;i<strDirRegistNo.GetLength();i++)
	{
		if (cResRegistNo[i]=='-')
		{
			iGapNoCount++;
		}
	}

	CString strTemp;
	strTemp=strResSrialNo.Right(strResSrialNo.GetLength()+iGapNoCount-strDirRegistNo.GetLength());
	strTemp=strTemp.Left(3);


	char cResMaxLink[8];
	CString2Char(strTemp,cResMaxLink);
	int i=0;
	for(;i<strTemp.GetLength();i++)
	{
		int iPosPass=FindSameItemNo(cResMaxLink[i]);
		iPosPass=key11*(iPosPass-key2);
		iPosPass=iPosPass%62;
		while(iPosPass<0)iPosPass+=62;
		cResMaxLink[i]=cShowValues[iPosPass];

	}
	cResMaxLink[i]='\0';

	strMaxLink=CString(cResMaxLink);

}



void  DecryptionChangePass(CString strResSrialNo,CString strResRegistNo,CString &strDirPass)
{
	int key4=23;

	int key33=13;

	int iGapNoCount=0;
	char cResRegistNo[40];
	CString2Char(strResRegistNo,cResRegistNo);
	for(int i=0;i<strResRegistNo.GetLength();i++)
	{
		if (cResRegistNo[i]=='-')
		{
			iGapNoCount++;
		}
	}

	CString strTemp;
	strTemp=strResSrialNo.Right(strResSrialNo.GetLength()+iGapNoCount-strResRegistNo.GetLength());
	strTemp=strTemp.Right(strTemp.GetLength()-3);
	strTemp=strTemp.Left(strTemp.GetLength()-6);

	char cResPass[40];
	CString2Char(strTemp,cResPass);

	for(int i=0;i<strTemp.GetLength();i++)
	{
		int iPosPass=FindSameItemNo(cResPass[i]);
		iPosPass=key33*(iPosPass-key4);
		iPosPass=iPosPass%62;
		while(iPosPass<0)iPosPass+=62;
		cResPass[i]=cShowValues[iPosPass];

	}

	// 	//Cpuid ���� ע����
	// 	CString strCpuId;
	// 	GetCpu1ID(strCpuId);
	// 	char cCpuId[40];
	// 	CString2Char(strCpuId,cCpuId);
	// 	for (int i=0;i<strResRegistNo.GetLength()&& i<strCpuId.GetLength();i++)
	// 	{
	// 		if (cResRegistNo[i]!='-')
	// 		{
	// 			cResRegistNo[i]=cResRegistNo[i]^cCpuId[i];
	// 		}
	// 	}

	int j=0;
	for(int i=0;i<strTemp.GetLength();i++)
	{

		// 		while(j<strResRegistNo.GetLength())
		// 		{
		// 			if (cResRegistNo[j]!='-')
		// 			{
		// 				break;
		// 			}
		// 			j++;
		// 		}
		while(cResRegistNo[j]=='-')
		{
			j++;
			if (j>=strResRegistNo.GetLength())
			{
				j=0;
			}
		}

		int iPosNo=FindSameItemNo(cResRegistNo[j]);

		int iPosPass=FindSameItemNo(cResPass[i]);
		if (iPosNo>=0&&iPosPass>=0)
		{
			if (int(iPosPass^iPosNo)<=61)
			{
				iPosPass=iPosPass^iPosNo;
				cResPass[i]=cShowValues[iPosPass];

			}
		}
	}

	strDirPass=CString(cResPass);
}

void  DecryptionPass(CString strResSrialNo,CString strResRegistNo,CString &strDirPass)
{
	CString strTempPass,strPass;
	DecryptionChangePass(strResSrialNo,strResRegistNo,strTempPass);
	strPass=strTempPass.Right(_ttoi(strTempPass.Left(2))-_ttoi(strTempPass.Mid(2,2)));
	for(int i=0;i<_ttoi(strTempPass.Mid(2,2));i++)
	{
		UINT nPos=_ttoi(strTempPass.Mid(2+(i+1)*2,2));
		strPass=strPass.Left(nPos)+_T("-")+strPass.Right(strPass.GetLength()-nPos);
	}
	strDirPass=strPass;
}

void  Decryption(CString strResSrialNo,CString strResRegistNo,CString &strDirPass)
{

	int key4=23;

	int key33=13;

	int iGapNoCount=0;
	char cResRegistNo[40];
	CString2Char(strResRegistNo,cResRegistNo);
	for(int i=0;i<strResRegistNo.GetLength();i++)
	{
		if (cResRegistNo[i]=='-')
		{
			iGapNoCount++;
		}
	}

	CString strTemp;
	strTemp=strResSrialNo.Right(strResSrialNo.GetLength()+iGapNoCount-strResRegistNo.GetLength());
	strTemp=strTemp.Right(strTemp.GetLength()-3);
	strTemp=strTemp.Left(strTemp.GetLength()-6);

	char cResPass[40];
	CString2Char(strTemp,cResPass);

	for(int i=0;i<strTemp.GetLength();i++)
	{
		if (cResPass[i]!='-')
		{
			int iPosPass=FindSameItemNo(cResPass[i]);
			iPosPass=key33*(iPosPass-key4);
			iPosPass=iPosPass%62;
			while(iPosPass<0)iPosPass+=62;
			cResPass[i]=cShowValues[iPosPass];
		}
	}

	// 	//Cpuid ���� ע����
	// 	CString strCpuId;
	// 	GetCpu1ID(strCpuId);
	// 	char cCpuId[40];
	// 	CString2Char(strCpuId,cCpuId);
	// 	for (int i=0;i<strResRegistNo.GetLength()&& i<strCpuId.GetLength();i++)
	// 	{
	// 		if (cResRegistNo[i]!='-')
	// 		{
	// 			cResRegistNo[i]=cResRegistNo[i]^cCpuId[i];
	// 		}
	// 	}

	int j=0;
	for(int i=0;i<strTemp.GetLength();i++)
	{
		if (cResPass[i]!='-')
		{
			// 			while(j<strResRegistNo.GetLength())
			// 			{
			// 				if (cResRegistNo[j]!='-')
			// 				{
			// 					//cResPass[i]=cResPass[i]^cResRegistNo[j];
			// 					break;
			// 				}
			// 				j++;
			// 			}

			int iPosNo=-1;
			if (j<strResRegistNo.GetLength())
			{
				iPosNo=FindSameItemNo(cResRegistNo[j]);
			}

			int iPosPass=FindSameItemNo(cResPass[i]);
			if (iPosNo>=0&&iPosPass>=0)
			{
				if (int(iPosPass^iPosNo)<=61)
				{
					iPosPass=iPosPass^iPosNo;
					cResPass[i]=cShowValues[iPosPass];

				}
			}

		}
	}

	// 	CString strTempPass,strPass;
	// 	strTempPass=CString(cResPass);
	// 	strPass=strTempPass.Right(_ttoi(strTempPass.Left(2))-_ttoi(strTempPass.Mid(2,2)));
	// 	for(int i=0;i<_ttoi(strTempPass.Mid(2,2));i++)
	// 	{
	// 		UINT nPos=_ttoi(strTempPass.Mid(2+(i+1)*2,2));
	// 		strPass=strPass.Left(nPos)+_T("-")+strPass.Right(strPass.GetLength()-nPos);
	// 	}
	strDirPass=CString(cResPass);

	// 	char cResPassNoHGap[40];
	// 	int jNo=0;
	// 	for (int i=0;cResPass[i]!='\0';i++)
	// 	{
	// 		if ((jNo+1)%6==0)
	// 		{
	// 			cResPassNoHGap[jNo++]='-';
	// 
	// 		}
	// 
	// 		cResPassNoHGap[jNo++]=cResPass[i];
	// 
	// 	}
	// 	cResPassNoHGap[jNo]='\0';
	// 
	// 	strDirPass=CString(cResPassNoHGap);

}





void  DecryptionDate(CString strResSrialNo,CString strResRegistNo,CString& strDirDate)
{
	int key2=34;
	int key11=41;

	CString strTemp=strResSrialNo.Right(6);
	char cDirDate[40];
	CString2Char(strTemp,cDirDate);


	// 	//Pass ���� 
	CString strPass;
	DecryptionChangePass(strResSrialNo,strResRegistNo,strPass);


	for(int i=0;i<strTemp.GetLength();i++)
	{

		int iPosDate=FindSameItemNo(cDirDate[i]);
		iPosDate=key11*(iPosDate-key2);
		iPosDate=iPosDate%62;
		while(iPosDate<0)iPosDate+=62;

		cDirDate[i]=cShowValues[iPosDate];


	}
	char cPass[40];
	CString2Char(strPass,cPass);
	for (int i=0;i<strTemp.GetLength();i++)
	{
		if (cPass[i%strPass.GetLength()]!='-')
		{
			int iPosNo=FindSameItemNo(cDirDate[i]);

			int iPosPass=FindSameItemNo(cPass[i%strPass.GetLength()]);
			if (iPosNo>=0&&iPosPass>=0)
			{
				if (int(iPosPass^iPosNo)<=61)
				{
					iPosNo=iPosPass^iPosNo;
					cDirDate[i]=cShowValues[iPosNo];

				}
			}
		}
	}
	cDirDate[strTemp.GetLength()]='\0';

	strDirDate=CString(cDirDate);
}

//unsigned char * base64=(unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";  
unsigned char * base64=(unsigned char *)"LZp8yQ0+E57T1ciArhYSkGjK94Psbn/udeRxo6tf3UCXHzNlvMIBOqaVgJw2mWFD";

CString base64encode(CString src,int srclen)  
{  
	int n,buflen,i,j;  
	static unsigned char *dst;  
	CString buf=src;  
	buflen=n=srclen;  
	dst=(unsigned char*)malloc(buflen/3*4+3);  
	memset(dst,0,buflen/3*4+3);  
	for(i=0,j=0;i<=buflen-3;i+=3,j+=4) {  
		dst[j]=(buf[i]&0xFC)>>2;  
		dst[j+1]=((buf[i]&0x03)<<4) + ((buf[i+1]&0xF0)>>4);  
		dst[j+2]=((buf[i+1]&0x0F)<<2) + ((buf[i+2]&0xC0)>>6);  
		dst[j+3]=buf[i+2]&0x3F;  
	}  
	if(n%3==1) {  
		dst[j]=(buf[i]&0xFC)>>2;  
		dst[j+1]=((buf[i]&0x03)<<4);  
		dst[j+2]=64;  
		dst[j+3]=64;  
		j+=4;  
	}  
	else if(n%3==2) {  
		dst[j]=(buf[i]&0xFC)>>2;  
		dst[j+1]=((buf[i]&0x03)<<4)+((buf[i+1]&0xF0)>>4);  
		dst[j+2]=((buf[i+1]&0x0F)<<2);  
		dst[j+3]=64;  
		j+=4;  
	}  
	for(i=0;i<j;i++) /* map 6 bit value to base64 ASCII character */  
		dst[i]=base64[(int)dst[i]];  
	dst[j]=0;  
	return CString(dst);  
}  

CString base64decode(CString inpt)  
{  
	int n,i,j;  
	unsigned char *p;  
	static unsigned char *dst;  
	unsigned char * src;   
	n=inpt.GetLength();  
	src=new unsigned char [n];  
	for(i=0;i<n;i++)  
		src[i]=(unsigned char)inpt[i];  

	while(n>0&&src[n-1]=='=') {  
		src[n-1]=0;  
		n--;  
	}  
	for(i=0;i<n;i++)   { /* map base64 ASCII character to 6 bit value */  
		p=(unsigned char *)strchr((const char *)base64,(int)src[i]);  
		if(!p)  
			break;  
		src[i]=p-(unsigned char *)base64;  
	}  

	dst=(unsigned char *)malloc(n*3/4+1);  
	memset(dst,0,n*3/4+1);  
	for(i=0,j=0;i<n;i+=4,j+=3) {  
		dst[j]=(src[i]<<2) + ((src[i+1]&0x30)>>4);  
		dst[j+1]=((src[i+1]&0x0F)<<4) + ((src[i+2]&0x3C)>>2);  
		dst[j+2]=((src[i+2]&0x03)<<6) + src[i+3];   
	}  
	return CString(dst);  
}

void EncryptionID(CString strResId,CString& strIdData)
{
	strIdData=base64encode(strResId,strResId.GetLength());
}

void DecryptionID(CString strResId,CString& strIdData)
{
	strIdData=base64decode(strResId);

}

void EncryptionUrlDate(CString strResUrlData,CString& strUrlData)
{

	strUrlData=base64encode(strResUrlData,strResUrlData.GetLength());

// 	int key1=101;
// 	int key2=59;
// 
// 	char cUrlData[200];
// 
// 	CString2Char(strResUrlData,cUrlData);
// 	for(int i=0;i<strResUrlData.GetLength();i++)
// 	{
// 		CString strTemp;
// 
// 		strTemp.Format(_T("%d"),cUrlData[i]);
// 		int iPos=_ttoi(strTemp);
// 		if (iPos>=0)
// 		{
// 			iPos=iPos*key1+key2;
// 			iPos=iPos%128;
// 			strTemp.Format(_T("%c"),iPos);
// 			strUrlData+=strTemp;
// 		}
// 
// 	}
}


void DecryptionRegistUrlDate(CString strResUrlData,CString& strUrlData)
{
	int key2=59;
	int key11=109;

	char cUrlData[200];
	CString2Char(strResUrlData,cUrlData);
	int j=0;
	for(int i=0;i<strResUrlData.GetLength();i++)
	{
		CString strTemp;
		strTemp.Format(_T("%d"),cUrlData[i]);
		int iPos=_ttoi(strTemp);
		iPos=key11*(iPos-key2);
		iPos=iPos%128;
		while(iPos<0)iPos+=128;
		strUrlData.AppendFormat(_T("%c"),iPos);
	}
}