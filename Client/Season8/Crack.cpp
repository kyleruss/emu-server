#include "stdafx.h"
#include "Crack.h"
#include "main.h"
#include "Camera.h"
#include "Interface.h"
#include "User.h"
#include "Other.h"
#include "Protocol.h"
#include "ItemGlow.h"
#include "ItemExcellentOption.h"
// -------------------------------------------------------------------------------

Crack g_Crack;
// -------------------------------------------------------------------------------

LPBYTE g_CrackAttachHookAddr	= (LPBYTE)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetStartupInfoA");
LPBYTE g_CrackReturnAddr		= (LPBYTE)0x00CD1960;
// -------------------------------------------------------------------------------

void Crack::Load()
{
	VMBEGIN
	memcpy(this->m_AttachRestoreBuff, (LPVOID)g_CrackAttachHookAddr, 5);
	SetOp((LPVOID)g_CrackAttachHookAddr, (LPVOID)Crack::ProcAttach, JMP);
	VMEND
}
// -------------------------------------------------------------------------------

__declspec(naked) void Crack::ProcAttach()
{
	__asm
	{
		pushad
		pushfd
		call dword ptr ds:[Crack::ProcLoading]
		popfd
		popad
		jmp dword ptr ds:[g_CrackAttachHookAddr]
	}
}
// -------------------------------------------------------------------------------


void Crack::ProcLoading()
{
	VMBEGIN
	g_Crack.ProcCrack();
	g_Protocol.Load();
	g_Other.Load();
	g_User.Load();
	g_Camera.Load();
	g_Interface.Load();
	g_ItemGlow.Load();
	g_ItemExcellentOption.Load();

	HexInit();
	TMainClass::Init();

	//SetOp((LPVOID)0x00A076EA, (LPVOID)MUClientModelDecrypt, CALL);
	//Some for pets:
	//*(WORD*)(0x00919282 + 6) = ITEMGET(12, 15);
	//*(WORD*)(0x0092D934 + 6) = ITEMGET(14, 14);
	//*(WORD*)(0x0092ED1E + 6) = ITEMGET(14, 15);
	//*(WORD*)(0x00932B88 + 6) = ITEMGET(14, 16);

	MouseSetHook(true);
	srand((DWORD)time(NULL));
	KeyboardSetHook(true);

	SetOp((LPVOID)0x00594593, (LPVOID)CalculateAttackRate, CALL);
	SetRange((LPVOID)0x009C4F3F, 9, NOP);
	SetOp((LPVOID)0x009C4F3F, (LPVOID)AttackRateFix1, JMP);
	SetRange((LPVOID)0x009C4E92, 9, NOP);
	SetOp((LPVOID)0x009C4E92, (LPVOID)AttackRateFix2, JMP);
	SetOp((LPVOID)0x00594563, (LPVOID)CalculateDamageRate, CALL);
	SetRange((LPVOID)0x009C4F14, 9, NOP);
	SetOp((LPVOID)0x009C4F14, (LPVOID)DamageRateFix1, JMP);
	SetRange((LPVOID)0x009C4E67, 9, NOP);
	SetOp((LPVOID)0x009C4E67, (LPVOID)DamageRateFix2, JMP);

	SetBuffer((LPVOID)g_CrackAttachHookAddr, g_Crack.m_AttachRestoreBuff, 5);
	SetOp((LPVOID)0x004D8D5A, (LPVOID)&Crack::ProcReturn, NONE);
	VMEND
}
// -------------------------------------------------------------------------------

void Crack::ProcCrack()
{
	VMBEGIN
	//SetItemType.bmd crc
	SetByte(0x004EF3F4, SJMP);
	//Disable redirect
	setNumeric<BYTE>(0x004D9085, SJMP);
	setNumeric<BYTE>(0x004D90C1, SJMP);
	setNumeric<BYTE>(0x004D9498, SJMP);

	//SetByte(0x004D9085, SJMP);
	//SetByte(0x004D90C1, SJMP);
	//SetByte(0x004D9498, SJMP);
	//Remove GameGuard
	BYTE GG_HOOK1[] = { 0xE9, 0x8B, 0x00, 0x00, 0x00, 0x90 };
	SetBuffer((LPVOID)0x004D94DB, GG_HOOK1, sizeof(GG_HOOK1));
	BYTE GG_HOOK2[] = { 0xE9, 0x35, 0xC5, 0x8B, 0xF6, 0x90 };
	SetBuffer((LPVOID)0x0A18E336, GG_HOOK2, sizeof(GG_HOOK2));
	BYTE GG_HOOK3[] = { 0xE9, 0x24, 0x1F, 0xB7, 0xF6, 0x90 };
	SetBuffer((LPVOID)0x09ED8C2F, GG_HOOK3, sizeof(GG_HOOK3));
	BYTE GG_HOOK4[] = { 0xE9, 0x83, 0x55, 0x77, 0xF6, 0x90 };
	SetBuffer((LPVOID)0x0A2D542D, GG_HOOK4, sizeof(GG_HOOK4));
	BYTE GG_HOOK5[] = { 0xE9, 0x39, 0x66, 0xA2, 0xF6, 0x90 };
	SetBuffer((LPVOID)0x0A024484, GG_HOOK5, sizeof(GG_HOOK5));
	//Remove additional encryption
	SetWord((LPVOID)0x006C2FCD, 0x9090);
	//Remove encryption change
	SetRange((LPVOID)0x09E22167, 3, NOP);
	SetRange((LPVOID)0x0A2AD2B0, 3, NOP);
	
	//Macro ignore

	/*SetOp((PVOID)0x004E1006, (PVOID)0x0A2C9427, NONE);
	SetOp((PVOID)0x004E0FF2, (PVOID)0x0A935FC4, NONE);
	SetOp((PVOID)0x0065BA7B, (PVOID)0x09DEF915, NONE);
	SetOp((PVOID)0x0065BBE1, (PVOID)0x0A2D4D41, NONE);
	SetOp((PVOID)0x0065ADB5, (PVOID)0x0A2F3F2D, NONE);
	SetOp((PVOID)0x00659F20, (PVOID)0x0A3B2D25, NONE);
	SetOp((PVOID)0x0065AF27, (PVOID)0x0A0281E0, NONE);
	SetOp((PVOID)0x004DDAD8, (PVOID)0x0A554767, NONE);*/

	// client closing
	SetOp((LPVOID)0x0065AD9F, (LPVOID)0x0AC13809, NONE);

	SetOp((LPVOID)0x004DE34E, (LPVOID)0x0A554767, NONE);
	SetOp((LPVOID)0x00659F20, (LPVOID)0x0A3B2D25, NONE);
	SetOp((LPVOID)0x0065BA7B, (LPVOID)0x09DEF915, NONE);
	SetOp((LPVOID)0x0065BBE1, (LPVOID)0x0A2D4D41, NONE);
	SetOp((LPVOID)0x004E0FF2, (LPVOID)0x0A935FC4, NONE);
	//SetOp((LPVOID)0x0065AD9F, (LPVOID)0x0AC13809, NONE);
	SetOp((LPVOID)0x004E1006, (LPVOID)0x0A2C9427, NONE);
	SetOp((LPVOID)0x0065ADB5, (LPVOID)0x0A2F3F2D, NONE);
	SetOp((LPVOID)0x004E0369, (LPVOID)0x09DEBB01, NONE);
	SetOp((LPVOID)0x004E07FF, (LPVOID)0x0A360739, NONE);
	SetOp((LPVOID)0x004E0165, (LPVOID)0x0A2FC68D, NONE);
	SetOp((LPVOID)0x0065BD9E, (LPVOID)0x0A01E64D, NONE);
	SetOp((LPVOID)0x004E0364, (LPVOID)0x0A554767, NONE);
	SetOp((LPVOID)0x0065ADAA, (LPVOID)0x09FD0928, NONE);
	SetOp((LPVOID)0x0065A3DF, (LPVOID)0x0AA3AB0F, NONE);
	SetOp((LPVOID)0x00659C84, (LPVOID)0x0A078E8F, NONE);
	SetOp((LPVOID)0x00659F0F, (LPVOID)0x0AB751ED, NONE);
	SetOp((LPVOID)0x004DDAD8, (LPVOID)0x0A554767, NONE);
	SetOp((LPVOID)0x0065B56B, (LPVOID)0x09DE92A7, NONE);
	SetOp((LPVOID)0x00952B7D, (LPVOID)0x0A16A6C7, NONE);
	SetOp((LPVOID)0x00952BF0, (LPVOID)0x0AD5B554, NONE);
	SetOp((LPVOID)0x00952BA5, (LPVOID)0x0A0D9857, NONE);
	SetOp((LPVOID)0x00952BBE, (LPVOID)0x0A0694A1, NONE);
	SetOp((LPVOID)0x00952BD7, (LPVOID)0x0A198DFA, NONE);
	SetOp((LPVOID)0x004DE630, (LPVOID)0x0A360739, NONE);
	SetOp((LPVOID)0x004DDF52, (LPVOID)0x0A360739, NONE);
	SetOp((LPVOID)0x0065BA65, (LPVOID)0x0A9DD2D6, NONE);
	SetOp((LPVOID)0x0065BD5C, (LPVOID)0x0AD5778E, NONE);
	SetOp((LPVOID)0x0065BD67, (LPVOID)0x09D640D2, NONE);
	SetOp((LPVOID)0x0065BD93, (LPVOID)0x0AB4BAC7, NONE);
	SetOp((LPVOID)0x0065BD88, (LPVOID)0x0A02098A, NONE);
	SetOp((LPVOID)0x0065BD72, (LPVOID)0x0A54D809, NONE);
	SetOp((LPVOID)0x0065BD7D, (LPVOID)0x0AB762AF, NONE);
	SetOp((LPVOID)0x0065BF9C, (LPVOID)0x09D0BFFF, NONE);
	SetOp((LPVOID)0x0065BFC8, (LPVOID)0x0AB5340B, NONE);
	SetOp((LPVOID)0x0065BFBD, (LPVOID)0x09EC1E58, NONE);
	SetOp((LPVOID)0x0065BFA7, (LPVOID)0x0A5A5195, NONE);
	SetOp((LPVOID)0x0065AF27, (LPVOID)0x0A0281E0, NONE);
	SetOp((LPVOID)0x0065B994, (LPVOID)0x09DF4A68, NONE);
	SetOp((LPVOID)0x0065BAD1, (LPVOID)0x0A557EA8, NONE);
	SetOp((LPVOID)0x0065BA20, (LPVOID)0x09E06CE0, NONE);
	SetOp((LPVOID)0x0065BA2E, (LPVOID)0x09DEB99A, NONE);
	SetOp((LPVOID)0x0065BAB5, (LPVOID)0x09CFE433, NONE);
	SetOp((LPVOID)0x0065B9CC, (LPVOID)0x09C6EB21, NONE);
	SetOp((LPVOID)0x0065BAC3, (LPVOID)0x09DEDE79, NONE);
	SetOp((LPVOID)0x0065B986, (LPVOID)0x09EC2C12, NONE);
	SetOp((LPVOID)0x0065B9A2, (LPVOID)0x0A1960C2, NONE);
	SetOp((LPVOID)0x0065BADF, (LPVOID)0x0A3AEB9D, NONE);
	SetOp((LPVOID)0x0065BA9C, (LPVOID)0x09D023E9, NONE);
	SetOp((LPVOID)0x00659F3A, (LPVOID)0x09DEAD1D, NONE);
	SetOp((LPVOID)0x0065BA86, (LPVOID)0x0A48802A, NONE);
	SetOp((LPVOID)0x0065BA5A, (LPVOID)0x0A292563, NONE);
	SetOp((LPVOID)0x0065BA4F, (LPVOID)0x0A2736C5, NONE);
	SetOp((LPVOID)0x0065BA70, (LPVOID)0x0AB72944, NONE);
	SetOp((LPVOID)0x00659F01, (LPVOID)0x09D636B1, NONE);
	SetOp((LPVOID)0x0065AF0B, (LPVOID)0x09E0CA4E, NONE);
	SetOp((LPVOID)0x0065AF19, (LPVOID)0x0ACAA2D1, NONE);
	SetOp((LPVOID)0x0065C1CA, (LPVOID)0x0A184DA5, NONE);
	SetOp((LPVOID)0x0065A0FF, (LPVOID)0x0A9A254B, NONE);
	SetOp((LPVOID)0x00599D8C, (LPVOID)0x0A297372, NONE);
	SetOp((LPVOID)0x00599DBA, (LPVOID)0x0A297372, NONE);
	SetOp((LPVOID)0x0065C160, (LPVOID)0x0AA8F5FE, NONE);
	SetOp((LPVOID)0x0065C128, (LPVOID)0x0A2702E2, NONE);
	SetOp((LPVOID)0x0065BA12, (LPVOID)0x0A557D32, NONE); //Castle Guard option	
	SetOp((LPVOID)0x0065BB70, (LPVOID)0x09F68925, NONE); //Castle Slingshot Attack
	SetOp((LPVOID)0x0065AB13, (LPVOID)0x0A8DD8E1, NONE); //Castle Slingshot Defense
	SetOp((LPVOID)0x0065B9B0, (LPVOID)0x0A069133, NONE); //Castle Senior gate buy
	//SetOp((LPVOID)0x0091EC4F, (LPVOID)0x0CB03AC0, NONE);
	VMEND
}
// -------------------------------------------------------------------------------

void Crack::ProcUnprotect()
{
	DWORD OldProtect;
	VirtualProtect((LPVOID)0x0401000, 0x00C2cffe, PAGE_EXECUTE_READ, &OldProtect);
}
// -------------------------------------------------------------------------------

__declspec(naked) void Crack::ProcReturn()
{
	__asm
	{
		pushad
		pushfd
		call dword ptr ds:[Crack::ProcUnprotect]
		popfd
		popad
		jmp dword ptr ds:[g_CrackReturnAddr]
	}
}
// -------------------------------------------------------------------------------