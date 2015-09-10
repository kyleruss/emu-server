#pragma once

#include "..\crypto\cryptlib.h"
#include "..\crypto\modes.h"
#include "..\crypto\des.h"

using namespace CryptoPP;

class CSimpleModulus
{

public:

	CSimpleModulus();
	virtual ~CSimpleModulus();

	void Init();
public:

	//int Encrypt(void * lpDest, void * lpSource, int iSize);
	int Encrypt(void * lpDest, void * lpSource, int iSize);
	int Decrypt(void * lpDest, void * lpSource, int iSize);
protected:

	ECB_Mode<DES_XEX3>::Encryption m_Enc;
	ECB_Mode<DES_XEX3>::Decryption m_Dec;
};