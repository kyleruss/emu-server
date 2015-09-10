#include "StdAfx.h"
#include "ClassCalc.h"
#include "logproc.h"
#include "GameMain.h"
#if (ENABLETEST_ZTLUA == 1)
#include "ZTLuaManager.h"
#endif
// -------------------------------------------------------------------------------

ClassCalc g_ClassCalc;
// -------------------------------------------------------------------------------

ClassCalc::ClassCalc()
{
	
}
// -------------------------------------------------------------------------------

ClassCalc::~ClassCalc()
{

}
// -------------------------------------------------------------------------------

void ClassCalc::Load()
{
	this->ReadData(gDirPath.GetNewPath(FILE_CUSTOM_CLASSCALC));
}
// -------------------------------------------------------------------------------

void ClassCalc::ReadData(LPSTR File)
{ 
	LPSTR ClassType[] = { "Wizard", "Knight", "Elf", "Gladiator", "Lord", "Summoner", "Fighter" };
	char FloatValue[256];
	// ----
	for( int Class = CLASS_WIZARD; Class < MAX_TYPE_PLAYER; Class++ )
	{
		this->m_Data[Class].LevelPoints					= GetPrivateProfileInt(ClassType[Class], "LevelPoints", 0, File);
		this->m_Data[Class].MasterLevelPoints			= GetPrivateProfileInt(ClassType[Class], "MasterLevelPoints", 0, File);
		this->m_Data[Class].MarlonBonusPoints			= GetPrivateProfileInt(ClassType[Class], "MarlonBonusPoints", 0, File);
		this->m_Data[Class].AttackDamageMinRightDiv1	= GetPrivateProfileInt(ClassType[Class], "AttackDamageMinRightDiv1", 0, File);
		this->m_Data[Class].AttackDamageMaxRightDiv1	= GetPrivateProfileInt(ClassType[Class], "AttackDamageMaxRightDiv1", 0, File);
		this->m_Data[Class].AttackDamageMinLeftDiv1		= GetPrivateProfileInt(ClassType[Class], "AttackDamageMinLeftDiv1", 0, File);
		this->m_Data[Class].AttackDamageMaxLeftDiv1		= GetPrivateProfileInt(ClassType[Class], "AttackDamageMaxLeftDiv1", 0, File);
		if( Class == CLASS_ELF || Class == CLASS_MAGUMSA || Class == CLASS_DARKLORD || Class == CLASS_FIGHTER )
		{
			this->m_Data[Class].AttackDamageMinRightDiv2	= GetPrivateProfileInt(ClassType[Class], "AttackDamageMinRightDiv2", 0, File);
			this->m_Data[Class].AttackDamageMaxRightDiv2	= GetPrivateProfileInt(ClassType[Class], "AttackDamageMaxRightDiv2", 0, File);
			this->m_Data[Class].AttackDamageMinLeftDiv2		= GetPrivateProfileInt(ClassType[Class], "AttackDamageMinLeftDiv2", 0, File);
			this->m_Data[Class].AttackDamageMaxLeftDiv2		= GetPrivateProfileInt(ClassType[Class], "AttackDamageMaxLeftDiv2", 0, File);
		}
		if( Class == CLASS_ELF )
		{
			this->m_Data[Class].AttackDamageMinRightDiv3	= GetPrivateProfileInt(ClassType[Class], "AttackDamageMinRightDiv3", 0, File);
			this->m_Data[Class].AttackDamageMaxRightDiv3	= GetPrivateProfileInt(ClassType[Class], "AttackDamageMaxRightDiv3", 0, File);
			this->m_Data[Class].AttackDamageMinLeftDiv3		= GetPrivateProfileInt(ClassType[Class], "AttackDamageMinLeftDiv3", 0, File);
			this->m_Data[Class].AttackDamageMaxLeftDiv3		= GetPrivateProfileInt(ClassType[Class], "AttackDamageMaxLeftDiv3", 0, File);
		}
		if( Class == CLASS_WIZARD || Class == CLASS_MAGUMSA || Class == CLASS_SUMMONER )
		{
			this->m_Data[Class].MagicDamageMinDiv	= GetPrivateProfileInt(ClassType[Class], "MagicDamageMinDiv", 0, File);
			this->m_Data[Class].MagicDamageMaxDiv	= GetPrivateProfileInt(ClassType[Class], "MagicDamageMaxDiv", 0, File);
			if( Class == CLASS_SUMMONER )
			{
				this->m_Data[Class].CurseDamageMinDiv	= GetPrivateProfileInt(ClassType[Class], "CurseDamageMinDiv", 0, File);
				this->m_Data[Class].CurseDamageMaxDiv	= GetPrivateProfileInt(ClassType[Class], "CurseDamageMaxDiv", 0, File);
			}
		}
		this->m_Data[Class].AttackSpeedDiv			= GetPrivateProfileInt(ClassType[Class], "AttackSpeedDiv", 0, File);
		this->m_Data[Class].MagicSpeedDiv			= GetPrivateProfileInt(ClassType[Class], "MagicSpeedDiv", 0, File);
		this->m_Data[Class].DefenseDiv				= GetPrivateProfileInt(ClassType[Class], "DefenseDiv", 0, File);
		if( Class == CLASS_FIGHTER )
		{
			GetPrivateProfileString(ClassType[Class], "PvPAttackRateFact1", "0", FloatValue, 5, File);
			sscanf(FloatValue, "%f", &this->m_Data[Class].PvPAttackRateFact1RF);
		}
		else
		{
			this->m_Data[Class].PvPAttackRateFact1	= GetPrivateProfileInt(ClassType[Class], "PvPAttackRateFact1", 0, File);
		}
		this->m_Data[Class].PvPAttackRateFact2		= GetPrivateProfileInt(ClassType[Class], "PvPAttackRateFact2", 0, File);
		if( Class != CLASS_DARKLORD && Class != CLASS_WIZARD )
		{
			this->m_Data[Class].PvPAttackRateDiv	= GetPrivateProfileInt(ClassType[Class], "PvPAttackRateDiv", 0, File);
		}
		if( Class == CLASS_FIGHTER )
		{
			GetPrivateProfileString(ClassType[Class], "PvPDefenseRateFact", "0", FloatValue, 5, File);
			sscanf(FloatValue, "%f", &this->m_Data[Class].PvPDefenseRateFactRF);
		}
		else
		{
			this->m_Data[Class].PvPDefenseRateFact		= GetPrivateProfileInt(ClassType[Class], "PvPDefenseRateFact", 0, File);
		}
		this->m_Data[Class].PvPDefenseRateDiv		= GetPrivateProfileInt(ClassType[Class], "PvPDefenseRateDiv", 0, File);
		this->m_Data[Class].SuccessBlockDiv			= GetPrivateProfileInt(ClassType[Class], "SuccessBlockDiv", 0, File);
		if( Class == CLASS_DARKLORD )
		{
			this->m_Data[Class].PetAttackDamageMinFact		= GetPrivateProfileInt(ClassType[Class], "PetAttackDamageMinFact", 0, File);
			this->m_Data[Class].PetAttackDamageMinDiv		= GetPrivateProfileInt(ClassType[Class], "PetAttackDamageMinDiv", 0, File);
			this->m_Data[Class].PetAttackDamageMinStart		= GetPrivateProfileInt(ClassType[Class], "PetAttackDamageMinStart", 0, File);
			this->m_Data[Class].PetAttackDamageMaxFact		= GetPrivateProfileInt(ClassType[Class], "PetAttackDamageMaxFact", 0, File);
			this->m_Data[Class].PetAttackDamageMaxDiv		= GetPrivateProfileInt(ClassType[Class], "PetAttackDamageMaxDiv", 0, File);
			this->m_Data[Class].PetAttackDamageMaxStart		= GetPrivateProfileInt(ClassType[Class], "PetAttackDamageMaxStart", 0, File);
			this->m_Data[Class].PetCriticalAttackRate		= GetPrivateProfileInt(ClassType[Class], "PetCriticalAttackRate", 0, File);
			this->m_Data[Class].PetAttackSpeedFact			= GetPrivateProfileInt(ClassType[Class], "PetAttackSpeedFact", 0, File);
			this->m_Data[Class].PetAttackSpeedDiv1			= GetPrivateProfileInt(ClassType[Class], "PetAttackSpeedDiv1", 0, File);
			this->m_Data[Class].PetAttackSpeedDiv2			= GetPrivateProfileInt(ClassType[Class], "PetAttackSpeedDiv2", 0, File);
			this->m_Data[Class].PetAttackSpeedStart			= GetPrivateProfileInt(ClassType[Class], "PetAttackSpeedStart", 0, File);
			this->m_Data[Class].PetSuccessAttackRateDiv		= GetPrivateProfileInt(ClassType[Class], "PetSuccessAttackRateDiv", 0, File);
			this->m_Data[Class].PetSuccessAttackRateStart	= GetPrivateProfileInt(ClassType[Class], "PetSuccessAttackRateStart", 0, File);
		}
	}
}
// -------------------------------------------------------------------------------

WORD ClassCalc::GetLevelPoint(LPOBJ lpUser, BYTE OnlyMaster, BYTE OnlyMarlon)
{
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("getLevelPoints", lpUser->m_Index, OnlyMaster, OnlyMarlon);
	WORD Result = LuaMng->getRef()->cast<WORD>();
	ZTLuaManager::Delete(LuaMng);
	return Result;
#else
	WORD Class = lpUser->Class;
	// ----
	if( Class < CLASS_WIZARD || Class > CLASS_FIGHTER )
	{
		LogAddC(2, "[ClassCalc] [GetLevelPoint] [%s][%s] Wrong class: %d",
			lpUser->AccountID, lpUser->Name, Class);
		return 5;
	}
	// ----
	if( OnlyMarlon )
	{
		return this->m_Data[Class].MarlonBonusPoints;
	}
	// ----
	if( OnlyMaster )
	{
		return this->m_Data[Class].MasterLevelPoints;
	}
	// ----
	return this->m_Data[Class].LevelPoints;
#endif
}
// -------------------------------------------------------------------------------

void ClassCalc::GetAttackDamage(LPOBJ lpUser)
{
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("setAttackDamage", lpUser->m_Index);
	ZTLuaManager::Delete(LuaMng);
#else
	CItem* Right	= &lpUser->pInventory[0];
	CItem* Left		= &lpUser->pInventory[1];
	int Strength	= lpUser->Strength + lpUser->AddStrength;
	int Dexterity	= lpUser->Dexterity + lpUser->AddDexterity;
	int Vitality	= lpUser->Vitality + lpUser->AddVitality;
	int Energy		= lpUser->Energy + lpUser->AddEnergy;
	WORD Class		= lpUser->Class;
	// ----
	if( Class == CLASS_WIZARD || Class == CLASS_KNIGHT )
	{
		lpUser->m_AttackDamageMinRight	= Strength / this->m_Data[Class].AttackDamageMinRightDiv1;
		lpUser->m_AttackDamageMaxRight	= Strength / this->m_Data[Class].AttackDamageMaxRightDiv1;
		lpUser->m_AttackDamageMinLeft	= Strength / this->m_Data[Class].AttackDamageMinLeftDiv1;
		lpUser->m_AttackDamageMaxLeft	= Strength / this->m_Data[Class].AttackDamageMaxLeftDiv1;
	}
	else if( Class == CLASS_ELF )
	{
		if(		(Right->m_Type >= ITEMGET(4,8) && Right->m_Type < ITEMGET(4,15) )
			||	(Left->m_Type >= ITEMGET(4,0) && Left->m_Type < ITEMGET(4,7))
			||	Right->m_Type == ITEMGET(4,16)
			||	Left->m_Type == ITEMGET(4,17)
			||	Right->m_Type == ITEMGET(4,18)
			||	Right->m_Type == ITEMGET(4,19)
			||	Left->m_Type == ITEMGET(4,20)
			||	Left->m_Type == ITEMGET(4,21)
			||	Left->m_Type == ITEMGET(4,22)
			||	Left->m_Type == ITEMGET(4,23)
			||	Left->m_Type == ITEMGET(4,24)
			||	Left->m_Type == ITEMGET(4,25)
			||	Right->m_Type == ITEMGET(4,26)
			||	Left->m_Type == ITEMGET(4,27))
		{
			if(		(Right->IsItem() && !Right->m_IsValidItem) 
				||	(Left->IsItem() && !Left->m_IsValidItem) )
			{
				lpUser->m_AttackDamageMinRight	= (Dexterity + Strength) / this->m_Data[Class].AttackDamageMinRightDiv3;
				lpUser->m_AttackDamageMaxRight	= (Dexterity + Strength) / this->m_Data[Class].AttackDamageMaxRightDiv3;
				lpUser->m_AttackDamageMinLeft	= (Dexterity + Strength) / this->m_Data[Class].AttackDamageMinLeftDiv3;
				lpUser->m_AttackDamageMaxLeft	= (Dexterity + Strength) / this->m_Data[Class].AttackDamageMaxLeftDiv3;
			}
			else
			{
				lpUser->m_AttackDamageMinRight	= (Dexterity / this->m_Data[Class].AttackDamageMinRightDiv1) + (Strength / this->m_Data[Class].AttackDamageMinRightDiv2);
				lpUser->m_AttackDamageMaxRight	= (Dexterity / this->m_Data[Class].AttackDamageMaxRightDiv1) + (Strength / this->m_Data[Class].AttackDamageMaxRightDiv2);
				lpUser->m_AttackDamageMinLeft	= (Dexterity / this->m_Data[Class].AttackDamageMinLeftDiv1) + (Strength / this->m_Data[Class].AttackDamageMinLeftDiv2);
				lpUser->m_AttackDamageMaxLeft	= (Dexterity / this->m_Data[Class].AttackDamageMaxLeftDiv1) + (Strength / this->m_Data[Class].AttackDamageMaxLeftDiv2);
			}
		}
		else
		{
			lpUser->m_AttackDamageMinRight	= (Dexterity + Strength) / this->m_Data[Class].AttackDamageMinRightDiv3;
			lpUser->m_AttackDamageMaxRight	= (Dexterity + Strength) / this->m_Data[Class].AttackDamageMaxRightDiv3;
			lpUser->m_AttackDamageMinLeft	= (Dexterity + Strength) / this->m_Data[Class].AttackDamageMinLeftDiv3;
			lpUser->m_AttackDamageMaxLeft	= (Dexterity + Strength) / this->m_Data[Class].AttackDamageMaxLeftDiv3;
		}
	}
	else if( Class == CLASS_MAGUMSA || Class == CLASS_DARKLORD )
	{
		lpUser->m_AttackDamageMinRight	= (Strength / this->m_Data[Class].AttackDamageMinRightDiv1) + (Energy / this->m_Data[Class].AttackDamageMinRightDiv2);
		lpUser->m_AttackDamageMaxRight	= (Strength / this->m_Data[Class].AttackDamageMaxRightDiv1) + (Energy / this->m_Data[Class].AttackDamageMaxRightDiv2);
		lpUser->m_AttackDamageMinLeft	= (Strength / this->m_Data[Class].AttackDamageMinLeftDiv1) + (Energy / this->m_Data[Class].AttackDamageMinLeftDiv2);
		lpUser->m_AttackDamageMaxLeft	= (Strength / this->m_Data[Class].AttackDamageMaxLeftDiv1) + (Energy / this->m_Data[Class].AttackDamageMaxLeftDiv2);
	}
	else if( Class == CLASS_SUMMONER )
	{
		lpUser->m_AttackDamageMinRight	= ((Strength + Dexterity) / this->m_Data[Class].AttackDamageMinRightDiv1);
		lpUser->m_AttackDamageMaxRight	= ((Strength + Dexterity) / this->m_Data[Class].AttackDamageMaxRightDiv1);
		lpUser->m_AttackDamageMinLeft	= ((Strength + Dexterity) / this->m_Data[Class].AttackDamageMinLeftDiv1);
		lpUser->m_AttackDamageMaxLeft	= ((Strength + Dexterity) / this->m_Data[Class].AttackDamageMaxLeftDiv1);
	}
	else if( Class == CLASS_FIGHTER )
	{
		lpUser->m_AttackDamageMinRight	= Vitality / this->m_Data[Class].AttackDamageMinRightDiv1 + Strength / this->m_Data[Class].AttackDamageMinRightDiv2;
		lpUser->m_AttackDamageMaxRight	= Vitality / this->m_Data[Class].AttackDamageMaxRightDiv1 + Strength / this->m_Data[Class].AttackDamageMaxRightDiv2;
		lpUser->m_AttackDamageMinLeft	= Vitality / this->m_Data[Class].AttackDamageMinLeftDiv1 + Strength / this->m_Data[Class].AttackDamageMinLeftDiv2;
		lpUser->m_AttackDamageMaxLeft	= Vitality / this->m_Data[Class].AttackDamageMaxLeftDiv1 + Strength / this->m_Data[Class].AttackDamageMaxLeftDiv2;
	}
	else
	{
		lpUser->m_AttackDamageMinRight	= Strength / 8;
		lpUser->m_AttackDamageMaxRight	= Strength / 4;
		lpUser->m_AttackDamageMinLeft	= Strength / 8;
		lpUser->m_AttackDamageMaxLeft	= Strength / 4;
	}
#endif
}
// -------------------------------------------------------------------------------

void ClassCalc::GetMagicDamage(LPOBJ lpUser)
{
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("setMagicDamage", lpUser->m_Index);
	ZTLuaManager::Delete(LuaMng);
#else
	int Energy = lpUser->Energy + lpUser->AddEnergy;
	// ----
	if( lpUser->Class == CLASS_WIZARD || lpUser->Class == CLASS_MAGUMSA )
	{
		lpUser->m_MagicDamageMin = Energy / this->m_Data[lpUser->Class].MagicDamageMinDiv;
		lpUser->m_MagicDamageMax = Energy / this->m_Data[lpUser->Class].MagicDamageMaxDiv;
	}
	else if( lpUser->Class == CLASS_SUMMONER )
	{
		lpUser->m_MagicDamageMin = Energy / this->m_Data[lpUser->Class].MagicDamageMinDiv;
		lpUser->m_MagicDamageMax = (int)(double(0.015) + Energy / this->m_Data[lpUser->Class].MagicDamageMaxDiv);
		lpUser->m_CurseDamageMin = Energy / m_Data[lpUser->Class].CurseDamageMinDiv;
		lpUser->m_CurseDamageMax = (int)(double(0.015) + Energy / this->m_Data[lpUser->Class].CurseDamageMaxDiv);
		lpUser->m_MagicDamageMax += lpUser->m_MPSkillOpt.MpsMaxMagicDamage;
		lpUser->m_MagicDamageMin += lpUser->m_MPSkillOpt.MpsMinMagicDamage;
	}
	else
	{
		lpUser->m_MagicDamageMin = Energy / 9;
		lpUser->m_MagicDamageMax = Energy / 4;
	}
#endif
}
// -------------------------------------------------------------------------------

void ClassCalc::GetAttackSpeed(LPOBJ lpUser)
{
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("setAttackSpeed", lpUser->m_Index);
	ZTLuaManager::Delete(LuaMng);
#else
	if( lpUser->Class >= MAX_TYPE_PLAYER || lpUser->Class < 0 )
	{
		LogAddC(2, "[ClassCalc] [%s] [%s] (GetAttackSpeed) wrong class (%d)",
			lpUser->AccountID, lpUser->Name, lpUser->Class);
		return;
	}

	int Dexterity = lpUser->Dexterity + lpUser->AddDexterity;

	//if( Dexterity > g_MaxStatValue || Dexterity < 0 )
//	{
	//	LogAddC(2, "[ClassCalc] [%s] [%s] (GetAttackSpeed) wrong Dexterity (%d)",
	//		lpUser->AccountID, lpUser->Name, Dexterity);
	//	return;
	//}
	
	if( this->m_Data[lpUser->Class].AttackSpeedDiv < 0 )
	{
		LogAddC(2, "[ClassCalc] [%s] [%s] (GetAttackSpeed) wrong AttackSpeedDiv (%d) for class (%d)",
			lpUser->AccountID, lpUser->Name, this->m_Data[lpUser->Class].AttackSpeedDiv, lpUser->Class);
		return;
	}

	if( this->m_Data[lpUser->Class].MagicSpeedDiv < 0 )
	{
		LogAddC(2, "[ClassCalc] [%s] [%s] (GetAttackSpeed) wrong MagicSpeedDiv (%d) for class (%d)",
			lpUser->AccountID, lpUser->Name, this->m_Data[lpUser->Class].MagicSpeedDiv, lpUser->Class);
		return;
	}

	lpUser->m_AttackSpeed = Dexterity / this->m_Data[lpUser->Class].AttackSpeedDiv;
	lpUser->m_MagicSpeed = Dexterity / this->m_Data[lpUser->Class].MagicSpeedDiv;
#endif
}
// -------------------------------------------------------------------------------

void ClassCalc::GetSuccessfulBlocking(LPOBJ lpUser)
{
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("setSuccessfulBlocking", lpUser->m_Index);
	ZTLuaManager::Delete(LuaMng);
#else
	if( lpUser->Class >= MAX_TYPE_PLAYER || lpUser->Class < 0 )
	{
		LogAddC(2, "[ClassCalc] [%s] [%s] (GetSuccessfulBlocking) wrong class (%d)",
			lpUser->AccountID, lpUser->Name, lpUser->Class);
		return;
	}

	int Dexterity = lpUser->Dexterity + lpUser->AddDexterity;

//	if( Dexterity > g_MaxStatValue || Dexterity < 0 )
//	{
//		LogAddC(2, "[ClassCalc] [%s] [%s] (GetSuccessfulBlocking) wrong Dexterity (%d)",
//			lpUser->AccountID, lpUser->Name, Dexterity);
//		return;
//	}

	if( this->m_Data[lpUser->Class].SuccessBlockDiv < 0 )
	{
		LogAddC(2, "[ClassCalc] [%s] [%s] (GetSuccessfulBlocking) wrong SuccessBlockDiv (%d) for class (%d)",
			lpUser->AccountID, lpUser->Name, this->m_Data[lpUser->Class].SuccessBlockDiv, lpUser->Class);
		return;
	}

	lpUser->m_SuccessfulBlocking = Dexterity / this->m_Data[lpUser->Class].SuccessBlockDiv;
#endif
}
// -------------------------------------------------------------------------------

void ClassCalc::GetDefense(LPOBJ lpUser)
{
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("setDefense", lpUser->m_Index);
	ZTLuaManager::Delete(LuaMng);
#else
	if( lpUser->Class >= MAX_TYPE_PLAYER || lpUser->Class < 0 )
	{
		LogAddC(2, "[ClassCalc] [%s] [%s] (GetDefense) wrong class (%d)",
			lpUser->AccountID, lpUser->Name, lpUser->Class);
		return;
	}

	int Dexterity = lpUser->Dexterity + lpUser->AddDexterity;

//	if( Dexterity > g_MaxStatValue || Dexterity < 0 )
//	{
//		LogAddC(2, "[ClassCalc] [%s] [%s] (GetDefense) wrong Dexterity (%d)",
//			lpUser->AccountID, lpUser->Name, Dexterity);
//		return;
//	}

	if( this->m_Data[lpUser->Class].DefenseDiv < 0 )
	{
		LogAddC(2, "[ClassCalc] [%s] [%s] (GetDefense) wrong DefenseDiv (%d) for class (%d)",
			lpUser->AccountID, lpUser->Name, this->m_Data[lpUser->Class].DefenseDiv, lpUser->Class);
		return;
	}

	lpUser->m_Defense = Dexterity / g_ClassCalc.m_Data[lpUser->Class].DefenseDiv;
#endif
}
// -------------------------------------------------------------------------------

float ClassCalc::GetAttackRate(LPOBJ lpUser)
{
	float AttackRate	= 0.0f;

#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("getAttackRate", lpUser->m_Index);
	AttackRate = LuaMng->getRef()->cast<float>();
	ZTLuaManager::Delete(LuaMng);
	return AttackRate;
#else
	if( lpUser->Class >= MAX_TYPE_PLAYER || lpUser->Class < 0 )
	{
		LogAddC(2, "[ClassCalc] [%s] [%s] (GetAttackRate) wrong class (%d)",
			lpUser->AccountID, lpUser->Name, lpUser->Class);
		return 0.0f;
	}	
	int Level			= lpUser->Level + lpUser->MasterLevel;
	WORD Class			= lpUser->Class;
	// ----
	if( Class == CLASS_WIZARD || Class == CLASS_DARKLORD )
	{
		AttackRate = (float)(Level * this->m_Data[Class].PvPAttackRateFact1 + lpUser->Dexterity * this->m_Data[Class].PvPAttackRateFact2);
	}
	else if( Class == CLASS_KNIGHT || Class == CLASS_ELF || Class == CLASS_MAGUMSA || Class == CLASS_SUMMONER )
	{
		AttackRate = (float)(Level * this->m_Data[Class].PvPAttackRateFact1  + (lpUser->Dexterity * this->m_Data[Class].PvPAttackRateFact2) / this->m_Data[Class].PvPAttackRateDiv);
	}
	else if( Class == CLASS_FIGHTER )
	{
		AttackRate = (float)(Level * this->m_Data[Class].PvPAttackRateFact1RF + (lpUser->Dexterity * this->m_Data[Class].PvPAttackRateFact2) / this->m_Data[Class].PvPAttackRateDiv);
	}
	// ----
	if( AttackRate <= 0.0f )
	{
		LogAddC(2, "[ClassCalc] [GetAttackRate] [%s][%s] Returned wrong value: %f",
			lpUser->AccountID, lpUser->Name, AttackRate);
	}
	// ----
	return AttackRate;
#endif
}
// -------------------------------------------------------------------------------

float ClassCalc::GetDefenseRate(LPOBJ lpUser)
{
	float DefenseRate	= 0.0f;
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("getDefenseRate", lpUser->m_Index);
	DefenseRate = LuaMng->getRef()->cast<float>();
	ZTLuaManager::Delete(LuaMng);
	return DefenseRate;
#else
	int Level			= lpUser->Level + lpUser->MasterLevel;
	WORD Class			= lpUser->Class;
	// ----
	if( Class < CLASS_WIZARD || Class > CLASS_FIGHTER )
	{
		LogAddC(2, "[ClassCalc] [GetDefenseRate] [%s][%s] Wrong class: %d",
			lpUser->AccountID, lpUser->Name, Class);
		return 0.0f;
	}
	// ----
	if( Class != CLASS_FIGHTER )
	{
		DefenseRate = (float)(Level * this->m_Data[Class].PvPDefenseRateFact + lpUser->Dexterity / this->m_Data[Class].PvPDefenseRateDiv);
	}
	else
	{
		DefenseRate = (float)(Level * this->m_Data[Class].PvPDefenseRateFactRF + lpUser->Dexterity / this->m_Data[Class].PvPDefenseRateDiv);
	}
	// ----
	if( DefenseRate <= 0.0f )
	{
		LogAddC(2, "[ClassCalc] [GetDefenseRate] [%s][%s] Returned wrong value: %f",
			lpUser->AccountID, lpUser->Name, DefenseRate);
	}
	// ----
	return DefenseRate;
#endif
}
// -------------------------------------------------------------------------------