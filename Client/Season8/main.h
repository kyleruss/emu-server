#pragma once

#include "resource.h"
#include "Security.h"
#include "MainClass.h"
#include "Protocol.h"
#include "ItemClass.h"
#include "Olymp.h"
#include "Hooks.h"
#include "NewOptions.h"
#include "FixHP.h"

// PROTOTYPE DEFINE
typedef void(*pFunctionVOID) ( ... );
typedef BYTE(*pFunctionBYTE) ( ... );
typedef WORD(*pFunctionWORD) ( ... );
typedef DWORD(*pFunctionDWORD) ( ... );

#define PLAYER_CONST0	0x84CF3D8
#define PLAYER_CONST1	0x84CF3DC
#define ITEM_CONST1		0x11E8DA4
#define ITEM_CONST3		0x7F36D38

struct FUNC_REPLACE
{
	DWORD OrAddr;
	DWORD NewAddr;
};

extern HINSTANCE hDLLInst;

void HexInit();
void XorData(char *FileData,int size);
BYTE CheckMaster(BYTE Master);

void _MyLoginTabFix();
