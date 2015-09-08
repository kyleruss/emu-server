#include "stdafx.h"
#include "SimpleModulus.h"

CSimpleModulus::CSimpleModulus()	// Completed
{ 
	this->Init(); 
}

void CSimpleModulus::Init()
{
	BYTE DES_XEX3[] = {
		0x0C,0xB0,0x66,0xCC,0xEF,0x92,0x8C,0x5C,
		0x65,0xF4,0xAC,0x3F,0x71,0xF2,0x7B,0xCE,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};
	this->m_Enc.SetKey(DES_XEX3,sizeof(DES_XEX3));
	this->m_Dec.SetKey(DES_XEX3,sizeof(DES_XEX3));
}


CSimpleModulus::~CSimpleModulus()	// Completed
{
	return;
}

int CSimpleModulus::Decrypt(void* lpDest,void* lpSource,int iSize)
{
	/*if ( lpDest == NULL)
	{
		return iSize;
	}

	LPBYTE lpTempDest = (LPBYTE)lpDest;
	LPBYTE lpTempSrc = (LPBYTE)lpSource;

	int iResult = 0;
	int iDecLen = 0;

	BYTE btSizeDiff = lpTempSrc[iSize-1];

	if ( iSize > 0 )
	{
		int lOfs = 0;
		do
		{
			this->DecryptBlock((LPDWORD)&lpTempSrc[lOfs],(LPDWORD)&lpTempDest[lOfs]);
			lOfs += 16;
		}
		while( lOfs < iSize );
	}*/

	if( lpDest == NULL )
		return iSize;

	if( lpSource == NULL )
		return 0;

	int nSize = iSize-1;

	if( (nSize%8) )
	{
		nSize = (1+nSize/8)*8;
	}

	LPBYTE lpTempDest = (LPBYTE)lpDest;
	LPBYTE lpTempSrc = (LPBYTE)lpSource;

	BYTE btSizeDiff = lpTempSrc[iSize-1];
	this->m_Dec.ProcessData(lpTempDest,lpTempSrc,nSize);
	return (iSize-btSizeDiff);
}

int CSimpleModulus::Encrypt(void* lpDest,void* lpSource,int iSize)
{
	/*if ( lpDest == NULL)
	{
		return iSize;
	}

	LPBYTE lpTempDest = (LPBYTE)lpDest;
	LPBYTE lpTempSrc = (LPBYTE)lpSource;

	int iResult = 0;
	int iDecLen = 0;
	int lOfs = 0;


	if ( iSize > 0 )
	{
		do
		{
			this->EncryptBlock((LPDWORD)&lpTempSrc[lOfs],(LPDWORD)&lpTempDest[lOfs]);
			lOfs += 16;
		}
		while( lOfs < iSize );
		lpTempDest[lOfs] = lOfs-iSize;
		lOfs++;
	}*/

	LPBYTE lpTempDest = (LPBYTE)lpDest;
	LPBYTE lpTempSrc = (LPBYTE)lpSource;

	int nSize = iSize;

	if( (iSize%8) )
	{
		nSize = (1+(iSize/8))*8;
	}

	m_Enc.ProcessData(lpTempDest,lpTempSrc,nSize);
	lpTempDest[nSize] = nSize-iSize;
	nSize++;
return nSize;
}
