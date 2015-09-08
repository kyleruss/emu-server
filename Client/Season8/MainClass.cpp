#include "stdafx.h"
#include "main.h"
#include "ExPoint.h"

TMainClass g_MainClass;
int TMainClass::g_iUSER_ADD_STATS = 0;

pMainInitUser MainInitUser = (pMainInitUser)0x00501D0D;
pMainGetEffectStatus MainGetEffectStatus = (pMainGetEffectStatus)0x004CFE6A;
pFunctionVOID mainLoadTextures = (pFunctionVOID)0x0060A49A;

void TMainClass::Init()
{
	VMBEGIN

	DWORD tmpClassOffset = -1;

	__asm
	{
		mov tmpClassOffset, offset TMainClass::MySend
	}
	HookDWORD((LPVOID)WSOCK_SEND,tmpClassOffset);

#if (__NEWOPTION__== 1)
	__asm
	{
		mov tmpClassOffset, offset CNewOptions::GetExcellentOption
	}

	HookDWORD((LPVOID)(0x0096D7BA+1),tmpClassOffset-(0x0096D7BA+5));
#endif

	VMEND
}

pFunctionVOID LoadGUIText23 = (pFunctionVOID)0x0077D055;

void myLoadTextures()
{
	mainLoadTextures();

	LoadGUIText23("Custom\\Button.tga",0xFF10,0x2601,2900,1,0);

#if (__CUSTOM_OLYMP__ == 1)
	gw_OlympNPC.LoadTextures();
#endif
}

DWORD __FixElfBuff_MOV = 0x009C4752;

void __declspec(naked) __FixElfBuff()
{
	__asm
	{
		MOV ECX,DWORD PTR DS:[PLAYER_CONST0]
		CALL DWORD PTR DS:[MainInitUser]
		MOV EAX,DWORD PTR DS:[PLAYER_CONST1]
		JMP DWORD PTR DS:[__FixElfBuff_MOV]
	}
};

DWORD __FixElfBuffEffect_MOV = 0x0057D175;
DWORD __FixElfBuffEffect_MOVOK = 0x0057D11D;

void __declspec( naked ) __FixElfBuffEffect()
{
	__asm
	{
		MOVZX EAX,AL
		TEST EAX,EAX
		JNZ fix
		PUSH 0xA1
		MOV ECX,DWORD PTR SS:[EBP+8]
		ADD ECX,0x518
		CALL DWORD PTR DS:[MainGetEffectStatus]
		MOVZX EAX, AL
		TEST EAX,EAX
		JE move
fix:
		JMP DWORD PTR DS:[__FixElfBuffEffect_MOVOK]
move:
		JMP DWORD PTR DS:[__FixElfBuffEffect_MOV]
	}
}

