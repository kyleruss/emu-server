#include "stdafx.h"
#include "main.h"

#if (__FIX_HP__ == 1)

int gHP = 0;
int gMAXHP = 0;
int gMP = 0;
int gMAXMP = 0;
int gSD = 0;
int gMAXSD = 0;
int gAttackRate = 0;
int gDamageRate = 0;

void __fastcall CalculateAttackRate(int This, LPVOID EDX)
{
	double AttackRate	= 0.0;
	int AttackSumm		= *(WORD*)(This + 52) + *(WORD*)(This + 26);
	// ----
	switch(*(BYTE*)(This + 11) & 7)
	{
	case 1:
		AttackRate = (double)*(WORD*)(This + 14) * 3.0 + (double)AttackSumm * 4.5;
		break;
	case 4:
		AttackRate = (double)*(WORD*)(This + 14) * 3.0 + (double)AttackSumm * 4.0;
		break;
	case 2:
		AttackRate = (double)*(WORD*)(This + 14) * 3.0 + (double)AttackSumm * 0.6000000238418579;
		break;
	case 3:
	case 5:
		AttackRate = (double)*(WORD*)(This + 14) * 3.0 + (double)AttackSumm * 3.5;
		break;
	case 0:
		AttackRate = (double)*(WORD*)(This + 14) * 3.0 + (double)AttackSumm * 4.0;
		break;
	case 6:
		AttackRate = (double)*(WORD*)(This + 14) * 2.599999904632568 + (double)AttackSumm * 3.599999904632568;
		break;
	}
	// ----
	gAttackRate = (int)AttackRate;
	*(WORD*)(This + 46) = (WORD)AttackRate;
}

void __declspec(naked) AttackRateFix1()
{
	_asm
	{
		mov esi, gAttackRate
		mov eax, 0x009C4F48
		jmp eax
	}
}

void __declspec(naked) AttackRateFix2()
{
	_asm
	{
		mov esi, gAttackRate
		mov eax, 0x009C4E9B
		jmp eax
	}
}

#define CharStatThis			((LPVOID(*)()) 0x0059518D)
#define RefreshDamageRate		((void(__thiscall*)(LPVOID This, int Damage, int Code)) 0x004F102D)

//00592D1D
void __fastcall CalculateDamageRate(int This, LPVOID EDX)
{
	WORD Strength	= *(WORD*)(This + 24) + *(WORD*)(This + 50);
	WORD Dexterity	= *(WORD*)(This + 26) + *(WORD*)(This + 52);
	WORD Leadership	= *(WORD*)(This + 32) + *(WORD*)(This + 62);
	// ----
	if( (*(BYTE*)(This + 11) & 7) == 4 )	//Dark Lord
	{
		gDamageRate = (Leadership / 10) + (Strength / 6) + (5 * Dexterity / 2) + (5 * *(WORD*)(This + 14));
		*(WORD*)(This + 86) = (Leadership / 10) + (Strength / 6) + (5 * Dexterity / 2) + (5 * *(WORD*)(This + 14));
	}
	else if( (*(BYTE*)(This + 11) & 7) == 6 )	//Monk
	{
		gDamageRate = (Strength / 6) + (5 * Dexterity / 4) + (3 * *(WORD*)(This + 14));
		*(WORD*)(This + 86) = (Strength / 6) + (5 * Dexterity / 4) + (3 * *(WORD*)(This + 14));
	}
	else	//Other
	{
		gDamageRate = ((signed int)(unsigned __int16)Strength >> 2) + (3 * Dexterity / 2) + (5 * *(WORD*)(This + 14));
		*(WORD*)(This + 86) = ((signed int)(unsigned __int16)Strength >> 2) + (3 * Dexterity / 2) + (5 * *(WORD*)(This + 14));
	}
	// ----
	RefreshDamageRate(CharStatThis(), *(WORD*)(This + 86), 177);
	gDamageRate += *(DWORD*)0x9C35654;
	*(WORD*)(This + 86) += *(DWORD*)0x9C35654;
}

void __declspec(naked) DamageRateFix1()
{
	_asm
	{
		mov esi, gDamageRate
		mov eax, 0x009C4F1D
		jmp eax
	}
}

void __declspec(naked) DamageRateFix2()
{
	_asm
	{
		mov esi, gDamageRate
		mov eax, 0x009C4E70
		jmp eax
	}
}

void gObjUserSetHP(int hp)
{
	gHP = hp;
}

void gObjUserSetMAXHP(int maxhp)
{
	gMAXHP = maxhp;
}

void gObjUserSetMP(int mp)
{
	gMP = mp;
}

void gObjUserSetMAXMP(int maxmp)
{
	gMAXMP = maxmp;
}

void gObjUserSetSD(int sd)	
{
	gSD = sd;
}

void gObjUserSetMAXSD(int maxsd)
{
	gMAXSD = maxsd;
}

void SetHPInfo(int& MainHP,int& MainMAXHP)
{
	MainMAXHP = gMAXHP;
	MainHP = (gHP < 0) ? 0 : ( (gHP > gMAXHP) ? gMAXHP : gHP );
}

void SetMPInfo(int& MainMP,int& MainMAXMP)
{
	MainMAXMP = gMAXMP;
	MainMP = (gMP < 0) ? 0 : ( (gMP > gMAXMP) ? gMAXMP : gMP );
}

void SetSDInfo(int& MainSD,int& MainMAXSD)
{
	MainMAXSD = (gMAXSD < 1) ? 1 : gMAXSD;
	MainSD = (gSD > gMAXSD) ? gMAXSD : gSD;
}

void InitHPFix()
{
	VMBEGIN
	// HP Fix
	BYTE tmpHPFix[] = {0x8B,0x45,0xF8,0x90 };
	HookBUFFER((LPVOID)0x009E5C92,tmpHPFix,sizeof(tmpHPFix));

	BYTE tmpHPFix2[] = {0x8B,0x45,0xEC,0x90 };
	HookBUFFER((LPVOID)0x009E5C97,tmpHPFix2,sizeof(tmpHPFix2));

	BYTE tmpHPFix3[] = {0xA1,0x00,0x00,0x00,0x40,0x90,0x90};
	HookBUFFER((LPVOID)0x0078BEFF,tmpHPFix3,sizeof(tmpHPFix3));
	HookDWORD((LPVOID)(0x0078BEFF+1),(DWORD)&gMAXHP);
	HookBUFFER((LPVOID)0x009C8218,tmpHPFix3,sizeof(tmpHPFix3));
	HookDWORD((LPVOID)(0x009C8218+1),(DWORD)&gMAXHP);

	BYTE tmpHPFix4[] = {0xA1,0x00,0x00,0x00,0x40,0x90,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x0078BF07,tmpHPFix4,sizeof(tmpHPFix4));
	HookDWORD((LPVOID)(0x0078BF07+1),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x0078BF32,tmpHPFix4,sizeof(tmpHPFix4));
	HookDWORD((LPVOID)(0x0078BF32+1),(DWORD)&gMAXHP);
	HookBUFFER((LPVOID)0x0078BF3C,tmpHPFix4,sizeof(tmpHPFix4));
	HookDWORD((LPVOID)(0x0078BF3C+1),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x009C8220,tmpHPFix4,sizeof(tmpHPFix4));
	HookDWORD((LPVOID)(0x009C8220+1),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x009C824B,tmpHPFix4,sizeof(tmpHPFix4));
	HookDWORD((LPVOID)(0x009C824B+1),(DWORD)&gMAXHP);
	HookBUFFER((LPVOID)0x009C8255,tmpHPFix4,sizeof(tmpHPFix4));
	HookDWORD((LPVOID)(0x009C8255+1),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x0094B908,tmpHPFix4,sizeof(tmpHPFix4));
	HookDWORD((LPVOID)(0x0094B908+1),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x0094D0B9,tmpHPFix4,sizeof(tmpHPFix4));
	HookDWORD((LPVOID)(0x0094D0B9+1),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x0094D183,tmpHPFix4,sizeof(tmpHPFix4));
	HookDWORD((LPVOID)(0x0094D183+1),(DWORD)&gHP);

	BYTE tmpHPFix5[] = {0x8B,0x45,0xEC,0x90};
	HookBUFFER((LPVOID)0x00807B5C,tmpHPFix5,sizeof(tmpHPFix5));
	HookBUFFER((LPVOID)0x00807B82,tmpHPFix5,sizeof(tmpHPFix5));
	HookBUFFER((LPVOID)0x00807BC7,tmpHPFix5,sizeof(tmpHPFix5));
	HookBUFFER((LPVOID)0x00807BCF,tmpHPFix5,sizeof(tmpHPFix5));
	HookBUFFER((LPVOID)0x00807BE8,tmpHPFix5,sizeof(tmpHPFix5));
	HookBUFFER((LPVOID)0x00807E77,tmpHPFix5,sizeof(tmpHPFix5));

	BYTE tmpHPFix6[] = {0x8B,0x85,0xD4,0xFE,0xFF,0xFF,0x90};
	HookBUFFER((LPVOID)0x00807B64,tmpHPFix6,sizeof(tmpHPFix6));
	HookBUFFER((LPVOID)0x00807B6F,tmpHPFix6,sizeof(tmpHPFix6));
	HookBUFFER((LPVOID)0x00807E0E,tmpHPFix6,sizeof(tmpHPFix6));
	HookBUFFER((LPVOID)0x00807E7C,tmpHPFix6,sizeof(tmpHPFix6));
	
	BYTE tmpHPFix7[] = {0x8B,0x8D,0xD4,0xFE,0xFF,0xFF,0x90};
	HookBUFFER((LPVOID)0x00807BD3,tmpHPFix7,sizeof(tmpHPFix7));

	BYTE tmpHPFix8[] = {0xA1,0x00,0x00,0x00,0x04,0xEB,0x18,0x90};
	HookBUFFER((LPVOID)0x0094B8B3,tmpHPFix8,sizeof(tmpHPFix8));
	HookDWORD((LPVOID)(0x0094B8B3+1),(DWORD)&gMAXHP);
	HookBUFFER((LPVOID)0x0094D064,tmpHPFix8,sizeof(tmpHPFix8));
	HookDWORD((LPVOID)(0x0094D064+1),(DWORD)&gMAXHP);
	HookBUFFER((LPVOID)0x0094D12A,tmpHPFix8,sizeof(tmpHPFix8));
	HookDWORD((LPVOID)(0x0094D12A+1),(DWORD)&gMAXHP);
	HookBUFFER((LPVOID)0x009759C1,tmpHPFix8,sizeof(tmpHPFix8));
	HookDWORD((LPVOID)(0x009759C1+1),(DWORD)&gMAXHP);

	BYTE tmpHPFix9[] = {0x8B,0x0D,0x00,0x00,0x00,0x40,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x0094B8D5,tmpHPFix9,sizeof(tmpHPFix9));
	HookDWORD((LPVOID)(0x0094B8D5+2),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x0094D086,tmpHPFix9,sizeof(tmpHPFix9));
	HookDWORD((LPVOID)(0x0094D086+2),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x0094D14C,tmpHPFix9,sizeof(tmpHPFix9));
	HookDWORD((LPVOID)(0x0094D14C+2),(DWORD)&gHP);

	BYTE tmpHPFixA[] = {0xA3,0x00,0x00,0x00,0x04,0x90,0x90,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x00634ECD,tmpHPFixA,sizeof(tmpHPFixA));
	HookDWORD((LPVOID)(0x00634ECD+1),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x00634F53,tmpHPFixA,sizeof(tmpHPFixA));
	HookDWORD((LPVOID)(0x00634F53+1),(DWORD)&gHP);

	BYTE tmpHPFixB[] = {0x89,0x0D,0x00,0x00,0x00,0x04,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x00634EE9,tmpHPFixB,sizeof(tmpHPFixB));
	HookDWORD((LPVOID)(0x00634EE9+2),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x00634F6F,tmpHPFixB,sizeof(tmpHPFixB));
	HookDWORD((LPVOID)(0x00634F6F+2),(DWORD)&gHP);

	BYTE tmpHPFixC[] = {0x8B,0x0D,0x00,0x00,0x00,0x40,0x90,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x00634EBD,tmpHPFixC,sizeof(tmpHPFixC));
	HookDWORD((LPVOID)(0x00634EBD+2),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x00634EDD,tmpHPFixC,sizeof(tmpHPFixC));
	HookDWORD((LPVOID)(0x00634EDD+2),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x00634F43,tmpHPFixC,sizeof(tmpHPFixC));
	HookDWORD((LPVOID)(0x00634F43+2),(DWORD)&gHP);
	HookBUFFER((LPVOID)0x00634F63,tmpHPFixC,sizeof(tmpHPFixC));
	HookDWORD((LPVOID)(0x00634F63+2),(DWORD)&gHP);

	VMEND
}

void InitMPFix()
{
	VMBEGIN

	BYTE tmpMPFix[] = {0xA1,0x00,0x00,0x00,0x40,0x90,0x90};
	HookBUFFER((LPVOID)0x0078C2D3,tmpMPFix,sizeof(tmpMPFix));
	HookDWORD((LPVOID)(0x0078C2D3+1),(DWORD)&gMAXMP);
	HookBUFFER((LPVOID)0x009C85D6,tmpMPFix,sizeof(tmpMPFix));
	HookDWORD((LPVOID)(0x009C85D6+1),(DWORD)&gMAXMP);

	BYTE tmpMPFix2[] = {0xA1,0x00,0x00,0x00,0x40,0x90,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x0078C2DB,tmpMPFix2,sizeof(tmpMPFix2));
	HookDWORD((LPVOID)(0x0078C2DB+1),(DWORD)&gMP);
	HookBUFFER((LPVOID)0x0078C306,tmpMPFix2,sizeof(tmpMPFix2));
	HookDWORD((LPVOID)(0x0078C306+1),(DWORD)&gMAXMP);
	HookBUFFER((LPVOID)0x0078C310,tmpMPFix2,sizeof(tmpMPFix2));
	HookDWORD((LPVOID)(0x0078C310+1),(DWORD)&gMP);
	HookBUFFER((LPVOID)0x009C85DE,tmpMPFix2,sizeof(tmpMPFix2));
	HookDWORD((LPVOID)(0x009C85DE+1),(DWORD)&gMP);
	HookBUFFER((LPVOID)0x009C8609,tmpMPFix2,sizeof(tmpMPFix2));
	HookDWORD((LPVOID)(0x009C8609+1),(DWORD)&gMAXMP);
	HookBUFFER((LPVOID)0x009C8613,tmpMPFix2,sizeof(tmpMPFix2));
	HookDWORD((LPVOID)(0x009C8613+1),(DWORD)&gMP);
	HookBUFFER((LPVOID)0x00952728,tmpMPFix2,sizeof(tmpMPFix2));
	HookDWORD((LPVOID)(0x00952728+1),(DWORD)&gMP);

	BYTE tmpMPFix3[] = {0x8B,0x45,0xE8,0x90};
	HookBUFFER((LPVOID)0x00807BFE,tmpMPFix3,sizeof(tmpMPFix3));
	HookBUFFER((LPVOID)0x00807C06,tmpMPFix3,sizeof(tmpMPFix3));
	HookBUFFER((LPVOID)0x00807C1F,tmpMPFix3,sizeof(tmpMPFix3));
	HookBUFFER((LPVOID)0x0080800A,tmpMPFix3,sizeof(tmpMPFix3));


	BYTE tmpMPFix4[] = {0x8B,0x8D,0xD8,0xFE,0xFF,0xFF,0x90};
	HookBUFFER((LPVOID)0x00807C0A,tmpMPFix4,sizeof(tmpMPFix4));

	BYTE tmpMPFix5[] = {0x8B,0x85,0xD8,0xFE,0xFF,0xFF,0x90};
	HookBUFFER((LPVOID)0x00807FA1,tmpMPFix5,sizeof(tmpMPFix5));
	HookBUFFER((LPVOID)0x0080800F,tmpMPFix5,sizeof(tmpMPFix5));

	BYTE tmpMPFix6[] = {0x8B,0x45,0xF4,0x90};
	HookBUFFER((LPVOID)0x009E5CE9,tmpMPFix6,sizeof(tmpMPFix6));

	BYTE tmpMPFix7[] = {0x8B,0x45,0xF0,0x90};
	HookBUFFER((LPVOID)0x009E5CEE,tmpMPFix7,sizeof(tmpMPFix7));

	BYTE tmpMPFix8[] = {0xA1,0x00,0x00,0x00,0x40,0xEB,0x18,0x90};
	HookBUFFER((LPVOID)0x009759C1,tmpMPFix8,sizeof(tmpMPFix8));
	HookDWORD((LPVOID)(0x009759C1+1),(DWORD)&gMAXMP);

	VMEND
}

void InitSDFix()
{
	VMBEGIN

	BYTE tmpSDFix[] = {0x8B,0x45,0xE8,0x90};
	HookBUFFER((LPVOID)0x00808418,tmpSDFix,sizeof(tmpSDFix));
	HookBUFFER((LPVOID)0x00808420,tmpSDFix,sizeof(tmpSDFix));
	HookBUFFER((LPVOID)0x00808436,tmpSDFix,sizeof(tmpSDFix));
	HookBUFFER((LPVOID)0x00808596,tmpSDFix,sizeof(tmpSDFix));

	BYTE tmpSDFix2[] = {0x8B,0x4D,0xFC,0x90};
	HookBUFFER((LPVOID)0x00808424,tmpSDFix2,sizeof(tmpSDFix2));

	BYTE tmpSDFix3[] = {0x8B,0x45,0xFC,0x90};
	HookBUFFER((LPVOID)0x0080851E,tmpSDFix3,sizeof(tmpSDFix3));
	HookBUFFER((LPVOID)0x0080859B,tmpSDFix3,sizeof(tmpSDFix3));
	HookBUFFER((LPVOID)0x009E5DE1,tmpSDFix3,sizeof(tmpSDFix3));
	HookBUFFER((LPVOID)0x009E5DFF,tmpSDFix3,sizeof(tmpSDFix3));
	HookBUFFER((LPVOID)0x009E5E1E,tmpSDFix3,sizeof(tmpSDFix3));

	BYTE tmpSDFix4[] = {0x8B,0x45,0xF8,0x90};
	HookBUFFER((LPVOID)0x009E5DF0,tmpSDFix4,sizeof(tmpSDFix4));
	HookBUFFER((LPVOID)0x009E5E0C,tmpSDFix4,sizeof(tmpSDFix4));
	HookBUFFER((LPVOID)0x009E5E19,tmpSDFix4,sizeof(tmpSDFix4));

	BYTE tmpSDFix5[] = {0x8B,0x0D,0x00,0x00,0x00,0x40,0x90,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x00634A49,tmpSDFix5,sizeof(tmpSDFix5));
	HookDWORD((LPVOID)(0x00634A49+2),(DWORD)&gSD);
	HookBUFFER((LPVOID)0x00634A69,tmpSDFix5,sizeof(tmpSDFix5));
	HookDWORD((LPVOID)(0x00634A69+2),(DWORD)&gSD);
	HookBUFFER((LPVOID)0x00634AF8,tmpSDFix5,sizeof(tmpSDFix5));
	HookDWORD((LPVOID)(0x00634AF8+2),(DWORD)&gSD);
	HookBUFFER((LPVOID)0x00634B18,tmpSDFix5,sizeof(tmpSDFix5));
	HookDWORD((LPVOID)(0x00634B18+2),(DWORD)&gSD);


	BYTE tmpSDFix6[] = {0xA3,0x00,0x00,0x00,0x40,0x90,0x90,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x00634A59,tmpSDFix6,sizeof(tmpSDFix6));
	HookDWORD((LPVOID)(0x00634A59+1),(DWORD)&gSD);
	HookBUFFER((LPVOID)0x00634B08,tmpSDFix6,sizeof(tmpSDFix6));
	HookDWORD((LPVOID)(0x00634B08+1),(DWORD)&gSD);

	BYTE tmpSDFix7[] = {0x89,0x0D,0x00,0x00,0x00,0x40,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x00634A75,tmpSDFix7,sizeof(tmpSDFix7));
	HookDWORD((LPVOID)(0x00634A75+2),(DWORD)&gSD);
	HookBUFFER((LPVOID)0x00634B24,tmpSDFix7,sizeof(tmpSDFix7));
	HookDWORD((LPVOID)(0x00634B24+2),(DWORD)&gSD);
	
	VMEND
}

void InitDMGFix()
{
	BYTE tmpDMGFix[] = {0x8B,0x40,0x0C,0x89,0x45,0xE4,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x00634D65,tmpDMGFix,sizeof(tmpDMGFix));

	BYTE tmpDMGFix2[] = {0x8B,0x45,0xE4,0x90};
	HookBUFFER((LPVOID)0x00634DF4,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00634E2A,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00634EB9,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00634ED9,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00634F3F,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00634F5F,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00635106,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x0063511E,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x006352C7,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x006352F3,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00635398,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00635404,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x0063544C,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x006354B0,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00635527,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x0063559E,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00635635,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x00635682,tmpDMGFix2,sizeof(tmpDMGFix2));
	HookBUFFER((LPVOID)0x006356AD,tmpDMGFix2,sizeof(tmpDMGFix2));

	BYTE tmpDMGFix3[] = {0x8B,0x40,0x10,0x89,0x45,0xF0,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90};
	HookBUFFER((LPVOID)0x00634DD6,tmpDMGFix3,sizeof(tmpDMGFix3));

	BYTE tmpDMGFix4[] = {0x8B,0x45,0xF0,0x90};
	HookBUFFER((LPVOID)0x00634E40,tmpDMGFix4,sizeof(tmpDMGFix4));
	HookBUFFER((LPVOID)0x00634E9D,tmpDMGFix4,sizeof(tmpDMGFix4));
	HookBUFFER((LPVOID)0x00634EF2,tmpDMGFix4,sizeof(tmpDMGFix4));
	HookBUFFER((LPVOID)0x00634F12,tmpDMGFix4,sizeof(tmpDMGFix4));
	HookBUFFER((LPVOID)0x00634F78,tmpDMGFix4,sizeof(tmpDMGFix4));
	HookBUFFER((LPVOID)0x00634F98,tmpDMGFix4,sizeof(tmpDMGFix4));
	HookBUFFER((LPVOID)0x006356C3,tmpDMGFix4,sizeof(tmpDMGFix4));
	HookBUFFER((LPVOID)0x00635729,tmpDMGFix4,sizeof(tmpDMGFix4));


	BYTE tmpDMGFix5[] = {0x8B,0x44,0x08,0x04,0x89,0x45,0xE4,0x90,0x90};
	HookBUFFER((LPVOID)0x00516D28,tmpDMGFix5,sizeof(tmpDMGFix5));

	BYTE tmpDMGFix6[] = {0x8B,0x45,0xE4,0x90};
	HookBUFFER((LPVOID)0x00516E60,tmpDMGFix6,sizeof(tmpDMGFix6));
	HookBUFFER((LPVOID)0x00516F07,tmpDMGFix6,sizeof(tmpDMGFix6));	
	HookBUFFER((LPVOID)0x00516F69,tmpDMGFix6,sizeof(tmpDMGFix6));
	HookBUFFER((LPVOID)0x00516FCB,tmpDMGFix6,sizeof(tmpDMGFix6));
	HookBUFFER((LPVOID)0x00517042,tmpDMGFix6,sizeof(tmpDMGFix6));
	HookBUFFER((LPVOID)0x00517080,tmpDMGFix6,sizeof(tmpDMGFix6));
	HookBUFFER((LPVOID)0x005170A8,tmpDMGFix6,sizeof(tmpDMGFix6));
}

DWORD __FixHP1_MOV = 0x009E5BDC;

void __declspec(naked) __FixHP1()
{
	__asm
	{
		LEA EAX,DWORD PTR SS:[EBP-0x08]
		PUSH EAX
		LEA EAX,DWORD PTR SS:[EBP-0x14]
		PUSH EAX
		CALL SetHPInfo
		ADD ESP,8
		LEA EAX,DWORD PTR SS:[EBP-0x0C]
		PUSH EAX
		LEA EAX,DWORD PTR SS:[EBP-0x10]
		PUSH EAX
		CALL SetMPInfo
		ADD ESP,8
		JMP DWORD PTR DS:[__FixHP1_MOV]
	}
}

DWORD __FixHP2_MOV = 0x00807B5C;

void __declspec(naked) __FixHP2()
{
	__asm
	{
		LEA EAX,DWORD PTR SS:[EBP-0x14]
		PUSH EAX
		LEA EAX,DWORD PTR SS:[EBP-0x12C]
		PUSH EAX
		CALL SetHPInfo
		ADD ESP,8
		LEA EAX,DWORD PTR SS:[EBP-0x18]
		PUSH EAX
		LEA EAX,DWORD PTR SS:[EBP-0x128]
		PUSH EAX
		CALL SetMPInfo
		ADD ESP,8
		JMP DWORD PTR DS:[__FixHP2_MOV]
	}
}

DWORD __FixSD1_MOV = 0x00808418;

void __declspec(naked) __FixSD1()
{
	__asm
	{
		LEA EAX,DWORD PTR SS:[EBP-0x18]
		PUSH EAX
		LEA EAX,DWORD PTR SS:[EBP-0x04]
		PUSH EAX
		CALL SetSDInfo
		ADD ESP,8
		JMP DWORD PTR DS:[__FixSD1_MOV]
	}
}

DWORD __FixSD2_MOV = 0x009E5DE1;

void __declspec(naked) __FixSD2()
{
	__asm
	{
		LEA EAX,DWORD PTR SS:[EBP-0x08]
		PUSH EAX
		LEA EAX,DWORD PTR SS:[EBP-0x04]
		PUSH EAX
		CALL SetSDInfo
		ADD ESP,8
		JMP DWORD PTR DS:[__FixSD2_MOV]
	}
}

#endif