#include "stdafx.h"
#include "main.h"
#include "Camera.h"
#include "ExPoint.h"
#include "TDebugLog.h"
#include "TMemory.h"
#include "FixHP.h"

pFunctionVOID mainDrawInterface = (pFunctionVOID)0x004DCB9B;
pFunctionVOID mainGlEnable = (pFunctionVOID)0x006275FB;
pFunctionVOID mainGLDisable = (pFunctionVOID)0x00627581;
pFunctionVOID CallScaleForm = (pFunctionVOID)0x0097E579;
pFunctionDWORD CDC_GetMessageThis = (pFunctionDWORD)0x0098CE32;
pCDC_GetMessageBox CDC_GetMessageBox = (pCDC_GetMessageBox)0x004A021A;
pCDC_MessageBox CDC_MessageBox = (pCDC_MessageBox)0x009D8342;

//pDrawInterface DrawInterface = (pDrawInterface)0x00974196;
//pCDCTabbedTextOut CDCTabbedTextOut = (pCDCTabbedTextOut)0x004725A7;
//pCDC_This CDC_InterfaceThis = (pCDC_This)0x00472310;


void __cdecl MyDrawInterface()
{
	g_Camera.Rotate();
	mainDrawInterface();
	mainGlEnable(1);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
#ifdef __NOVUS__
	g_ExPoint.DrawBox();
#endif
#if (__CUSTOM_OLYMP__==1)
	gw_OlympNPC.Draw();
#endif

	if( (*(BYTE*)(*(DWORD*)PLAYER_CONST1 + 11) & 7) == 6 ) //Monk
	{
		SetByte((PVOID)(0x00554B6A + 6), 15);
	}
	//if( (*(BYTE*)(*(DWORD*)PLAYER_CONST1 + 11) & 7) == 2 )	//Elf
	//{
	//	SetByte((PVOID)(0x00554B6A + 6), 0);
	//}
	else
	{
		SetByte((PVOID)(0x00554B6A + 6), 2); //Other
	}
	
	mainGLDisable();
}