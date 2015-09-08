#include "stdafx.h"
#include "main.h"


pFunctionBYTE Check3rdClass = (pFunctionBYTE)0x0058ABC0;

FUNC_REPLACE g_FuncSet[] = {
	0,0,
	0x006548B0,(DWORD)&ProtocolCore,
	// Special Effect
	0x00565EC7,(DWORD)&MySetSpecialEffect,
	0x006052D4,(DWORD)&MySetSpecialEffect,

	0x00602C77,(DWORD)MySetSpecialEffect2,
	0x00602DC6,(DWORD)MySetSpecialEffect2,
	0x00602E7F,(DWORD)MySetSpecialEffect2,
	0x00602F46,(DWORD)MySetSpecialEffect2,
	0x0060301F,(DWORD)MySetSpecialEffect2,
	0x00603104,(DWORD)MySetSpecialEffect2,
	0x006031D1,(DWORD)MySetSpecialEffect2,
	0x00603279,(DWORD)MySetSpecialEffect2,
	0x006032EE,(DWORD)MySetSpecialEffect2,
	0x00603363,(DWORD)MySetSpecialEffect2,
	0x006033FB,(DWORD)MySetSpecialEffect2,
	0x0060349C,(DWORD)MySetSpecialEffect2,
	0x00603541,(DWORD)MySetSpecialEffect2,
	0x0060361E,(DWORD)MySetSpecialEffect2,
	0x006036FB,(DWORD)MySetSpecialEffect2,
	0x00603805,(DWORD)MySetSpecialEffect2,
	0x0060390F,(DWORD)MySetSpecialEffect2,
	0x00603A19,(DWORD)MySetSpecialEffect2,
	0x00603B23,(DWORD)MySetSpecialEffect2,
	0x00603BF2,(DWORD)MySetSpecialEffect2,
	0x00603C38,(DWORD)MySetSpecialEffect2,

	0x007DEB4E,(DWORD)&ItemConvertEx,

	0x005A472F,(DWORD)&IsItemSpecial,
	0x0082A954,(DWORD)&IsItemSpecial,
	0x00A0E803,(DWORD)&IsItemSpecial,
	0x00A0FAFE,(DWORD)&IsItemSpecial,

	0x0057D33B,(DWORD)&myGlow,
	0x005FBA3E,(DWORD)&myGlow,
	0x005FC9CA,(DWORD)&myGlow2,

	0x00625274,(DWORD)&myLoadTextures,

	0x0050EC83, (DWORD)GetItemValue,
	0x005B951F, (DWORD)GetItemValue,
	0x005B95F9, (DWORD)GetItemValue,
	0x005BA8F2, (DWORD)GetItemValue,
	0x007E0861, (DWORD)GetItemValue,
	0x007E07B2, (DWORD)GetItemValue,
	0x007E07FA, (DWORD)GetItemValue,
	0x00836ECB, (DWORD)GetItemValue,
	0x00836F35, (DWORD)GetItemValue,
	0x00975884, (DWORD)GetItemValue,
	0x009AC0F9, (DWORD)GetItemValue,
	0x009AC188, (DWORD)GetItemValue,
	0x00A0E5EC, (DWORD)GetItemValue,
	0x00A0F8E7, (DWORD)GetItemValue,

#if (__CUSTOM_WINGS__==1)
	0x004B59C7,(DWORD)&MyObjectPreview,
	0x0062DC66,(DWORD)&MyObjectPreview,
	0x00631D4C,(DWORD)&MyObjectPreview,
	0x00632BBD,(DWORD)&MyObjectPreview,
	0x006332F4,(DWORD)&MyObjectPreview,
	0x00653695,(DWORD)&MyObjectPreview,
#endif
};

void Warning(int Id)
{
	char szTmp[128];
	sprintf_s(szTmp,"Protection Macro #%d",Id);
	MessageBox(NULL,szTmp,"Warning!",NULL);
}

DWORD __ProtectionWarn1_MOV = 0x006C6D3C;

void __declspec(naked) __ProtectionWarn1()
{
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 1
		CALL Warning
		ADD ESP,4
		POPFD
		POPAD
		JMP DWORD PTR DS:[__ProtectionWarn1_MOV]
	}
}

DWORD __ProtectionWarn2_MOV = 0x006C6C51;

void __declspec(naked) __ProtectionWarn2()
{
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 2
		CALL Warning
		ADD ESP,4
		POPFD
		POPAD
		JMP DWORD PTR DS:[__ProtectionWarn2_MOV]
	}
}

DWORD __ProtectionWarn3_MOV = 0x006C74E9;

void __declspec(naked) __ProtectionWarn3()
{
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 3
		CALL Warning
		ADD ESP,4
		POPFD
		POPAD
		JMP DWORD PTR DS:[__ProtectionWarn3_MOV]
	}
}

DWORD __ProtectionWarn4_MOV = 0x006C69EF;

void __declspec(naked) __ProtectionWarn4()
{
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 4
		CALL Warning
		ADD ESP,4
		POPFD
		POPAD
		JMP DWORD PTR DS:[__ProtectionWarn4_MOV]
	}
}

DWORD __ProtectionWarn5_MOV = 0x006C8276;

void __declspec(naked) __ProtectionWarn5()
{
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 5
		CALL Warning
		ADD ESP, 4
		POPFD
		POPAD
		JMP DWORD PTR DS:[__ProtectionWarn5_MOV]
	}
}

DWORD __ProtectionWarn6_MOV = 0x006C834F;

void __declspec(naked) __ProtectionWarn6()
{
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 6
		CALL Warning
		ADD ESP, 4
		POPFD
		POPAD
		JMP DWORD PTR DS:[__ProtectionWarn6_MOV]
	}
}

DWORD __ProtectionWarn7_MOV = 0x006C8198;

void __declspec(naked) __ProtectionWarn7()
{
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 7
		CALL Warning
		ADD ESP, 4
		POPFD
		POPAD
		JMP DWORD PTR DS:[__ProtectionWarn7_MOV]
	}
}

DWORD __ProtectionWarn8_MOV = 0x006C75F8;

void __declspec(naked) __ProtectionWarn8()
{
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 8
		CALL Warning
		ADD ESP, 4
		POPFD
		POPAD
		JMP DWORD PTR DS:[__ProtectionWarn8_MOV]
	}
}

DWORD __ProtectionWarn9_MOV = 0x006C6B20;

void __declspec(naked) __ProtectionWarn9()
{
	__asm
	{
		PUSHAD
		PUSHFD
		PUSH 9
		CALL Warning
		ADD ESP, 4
		POPFD
		POPAD
		JMP DWORD PTR DS:[__ProtectionWarn9_MOV]
	}
}


FUNC_REPLACE JmpTable[] = {
	0,0,
	0x0043E1A5,(DWORD)&_MyLoginTabFix,
	0x004DA525,(DWORD)&SpeedHackCheck,
	0x009C474D,(DWORD)&__FixElfBuff,
	0x0057D116,(DWORD)&__FixElfBuffEffect,
	0x0058EBF5,(DWORD)&__FixExchangeSymbolPrice,
	0x0096D574,(DWORD)&__FixExcellentSocket,

#if (__CUSTOM_WINGS__==1)
	0x007E326B,(DWORD)&__Fix4thWingsDisplay,
	0x0096D818,(DWORD)&__Fix4thWingsHPOption,
#endif

#if (__NEWOPTION__)
	0x007E19A1,(DWORD)&__GetLvl15Option,
	0x007E2ACB,(DWORD)&__FixWizRise,
	0x009C962A,(DWORD)&__FixWizRise2,
#endif

#if (__FIX_HP__==1)
	0x009E5A2B,(DWORD)&__FixHP1,
	0x00807933,(DWORD)&__FixHP2,
	0x00808337,(DWORD)&__FixSD1,
	0x009E5D29,(DWORD)&__FixSD2,
#endif
	
	0x09DEE91E,(DWORD)&__ProtectionWarn1,
	0x09EC170E,(DWORD)&__ProtectionWarn2,
	0x09FBA372,(DWORD)&__ProtectionWarn3,
	0x0A074E85,(DWORD)&__ProtectionWarn4,
	0x0A273FBA,(DWORD)&__ProtectionWarn5,
	0x0A2C3900,(DWORD)&__ProtectionWarn6,
	// pt for s10 00A6582D
	0x0A5608DB,(DWORD)&__ProtectionWarn7,
	0x0A5B0F2E,(DWORD)&__ProtectionWarn8,
	0x0AA8E8EC,(DWORD)&__ProtectionWarn9,
};

DWORD _MyLoginTabFix_MOV = 0x0043E1BE;

void __declspec( naked ) _MyLoginTabFix()
{
	__asm
	{
		PUSH 0
		MOV EAX,DWORD PTR SS:[EBP-0x38]
		MOV ECX,DWORD PTR DS:[EAX+0x350]
		MOV EDX,DWORD PTR SS:[EBP-0x38]
		MOV EAX,DWORD PTR DS:[EDX+0x350]
		MOV EDX,DWORD PTR DS:[EAX]
		CALL DWORD PTR DS:[EDX+0x30]
		MOV EAX,DWORD PTR SS:[EBP-0x38]
		MOV ECX,DWORD PTR DS:[EAX+0x354]
		PUSH ECX
		MOV EDX,DWORD PTR SS:[EBP-0x38]
		MOV ECX,DWORD PTR DS:[EDX+0x350]
		MOV EAX,DWORD PTR SS:[EBP-0x38]
		MOV EDX,DWORD PTR DS:[EAX+0x350]
		MOV EAX,DWORD PTR DS:[EDX]
		CALL DWORD PTR DS:[EAX+0x58]
		MOV ECX,DWORD PTR SS:[EBP-0x38]
		MOV EDX,DWORD PTR DS:[ECX+0x350]
		PUSH EDX
		MOV EAX,DWORD PTR SS:[EBP-0x38]
		MOV ECX,DWORD PTR DS:[EAX+0x354]
		MOV EDX,DWORD PTR SS:[EBP-0x38]
		MOV EAX,DWORD PTR DS:[EDX+0x354]
		MOV EDX,DWORD PTR DS:[EAX]
		CALL DWORD PTR DS:[EDX+0x58]
		JMP DWORD PTR DS:[_MyLoginTabFix_MOV]
	}
}

DWORD g_dwMainThread = GetTickCount();
int StatusProtection = 0;

void MainThread()
{
	while( TRUE )
	{
		VMBEGIN

		if( (GetTickCount()-g_dwMainThread) >= 10000 )
		{
			g_dwMainThread = GetTickCount();
		
			if( SpeedHackCheck() == TRUE )
			{
				if( pPlayerState == 4 || pPlayerState == 5 )
				{
					//GameClose(0);
				}
				//MessageBox(NULL, "Были найдены запрешеные программы!\nЗакройте все ненужные программы и попробуйте снова!", "[Ошибка]", NULL);
				//ExitProcess(0);
			}
		}

		Sleep(1000);

		VMEND
	}
}

double __g_fixcoin_h = 0.025;
double __g_fixcoin_w = 0.0;

void HexInit()
{
	VMBEGIN
	
	for(int i = 0; i < sizeof(JmpTable)/sizeof(FUNC_REPLACE); i++)	{
		if( JmpTable[i].OrAddr == NULL )
			continue;

		HookBYTE((LPVOID)JmpTable[i].OrAddr,0xE9);
		SetOp((LPVOID)JmpTable[i].OrAddr, (LPVOID)JmpTable[i].NewAddr, NONE);
	}

	
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)MainThread,NULL,NULL,&dwThreadID);

	//if( hThread )
	//	CloseHandle(hThread);

	// CALL REPLACE
	for(int i = 0; i < sizeof(g_FuncSet)/sizeof(FUNC_REPLACE);i++)	{
		if( g_FuncSet[i].OrAddr != NULL )	{
			SetOp((LPVOID)g_FuncSet[i].OrAddr, (LPVOID)g_FuncSet[i].NewAddr, NONE);
		}
	}

#if (__NEWOPTION__==1)
	for(int i = 0; i < sizeof(pBonusOffsets)/sizeof(__ENCHANT_BONUS); i++)	{
		HookBYTE((LPVOID)(pBonusOffsets[i].Offset10+2),5);
		HookBYTE((LPVOID)(pBonusOffsets[i].Offset11+2),8);
		HookBYTE((LPVOID)(pBonusOffsets[i].Offset12+2),12);
		HookBYTE((LPVOID)(pBonusOffsets[i].Offset13+2),17);
		HookBYTE((LPVOID)(pBonusOffsets[i].Offset14+2),23);
		HookBYTE((LPVOID)(pBonusOffsets[i].Offset15+2),30);
	}

	if( !gNewOptions.LoadData() )
	{
		MessageBox(NULL,"Failed to load ExOptions.bmd","ERROR!",NULL);
		ExitProcess(GetCurrentProcessId());
	}
#endif

#if (__FIX_HP__ ==1)
	InitHPFix();
	InitMPFix();
	InitSDFix();
	InitDMGFix();
#endif

/*	
	// mining in hunting zones
	//	CPU Disasm
	//Address   Hex dump              Command                                                  Comments
	//005A2DB8    90                  NOP

	HookDWORD((LPVOID)(0x005BEBBA+2),(DWORD)&__g_fixcoin_w);
	HookDWORD((LPVOID)(0x005BEBC6+2),(DWORD)&__g_fixcoin_h);
	*/
	VMEND
}