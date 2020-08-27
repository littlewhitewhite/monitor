/*
	100% free public domain implementation of the HMAC-SHA1 algorithm
	by Chien-Chung, Chung (Jim Chung) <jimchung1221@gmail.com>
*/


#ifndef __HMAC_SHA1_H__
#define __HMAC_SHA1_H__

#include "SHA1.h"

typedef unsigned char BYTE ;

class CHMAC_SHA1 : public CSHA1
{
    private:
		BYTE m_ipad[64];
        BYTE m_opad[64];

		char * szReport ;
		char * SHA1_Key ;
		char * AppendBuf1 ;
		char * AppendBuf2 ;


	public:
		
		enum {
			SHA1_DIGEST_LENGTH	= 20,
			SHA1_BLOCK_SIZE		= 64,
			HMAC_BUF_LEN		= 4096
		} ;

		CHMAC_SHA1()
			:szReport(new char[HMAC_BUF_LEN]),
             AppendBuf1(new char[HMAC_BUF_LEN]),
             AppendBuf2(new char[HMAC_BUF_LEN]),
             SHA1_Key(new char[HMAC_BUF_LEN])
		{}

        ~CHMAC_SHA1()
        {
			if (szReport!=NULL)
			{
				delete[] szReport;
			}
			if (AppendBuf1 != NULL)
			{
				delete[] AppendBuf1;
			}
			if (AppendBuf2 != NULL)
			{
				delete[] AppendBuf2;
			}
			if (SHA1_Key != NULL)
			{
				delete[] SHA1_Key;
			}

        }

        void HMAC_SHA1(BYTE *text, int text_len, BYTE *key, int key_len, BYTE *digest);

		void Release();
};


#endif /* __HMAC_SHA1_H__ */
