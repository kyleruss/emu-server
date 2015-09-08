#include "StdAfx.h"
#include "MonsterStatCalc.h"
#include "GameMain.h"
#include "logproc.h"
#include "..\include\ReadScript.h"

CMonsterStatCalc g_MonsterStatCalc;

CMonsterStatCalc::CMonsterStatCalc()
{

}

CMonsterStatCalc::~CMonsterStatCalc()
{

}

void CMonsterStatCalc::LoadScript(char* szFileName)
{
	int Token;
	this->m_bLoaded = false;

	SMDFile = fopen(szFileName,"r");

	if( SMDFile == NULL )
	{
		LogAdd(lMsg.Get(MSGGET(1,197)),szFileName);
		return;
	}

	// vector should be first cleared... 
	this->m_vtMonsterStatRef.clear();

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		if( Token == NUMBER )
		{
			int nType = TokenNumber;

			if( nType == 0 )
			{
				while( TRUE )
				{
					Token = GetToken();

					if( Token == NAME )
					{
						if( !strcmp("end",TokenString) )
							break;
					}

					_stMonsterStatRef monsterStatRef;

					monsterStatRef.m_MinReset = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_MaxReset = TokenNumber;
					
					Token = GetToken();
					monsterStatRef.m_MinLevel = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_MaxLevel = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_LevelMul = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_LifeMul = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_ManaMul = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_AttackMinMul = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_AttackMaxMul = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_DefenseMul = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_MagicDefenseMul = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_AttackRatingMul = TokenNumber;

					Token = GetToken();
					monsterStatRef.m_SuccessfulBlockingMul = TokenNumber;

					this->m_vtMonsterStatRef.push_back(monsterStatRef);
				}
			}
		}
	}

	fclose(SMDFile);
	LogAdd(lMsg.Get(MSGGET(1,198)),szFileName);
	this->m_bLoaded = true;
}


int CMonsterStatCalc::ConvertMonsterStat(LPOBJ lpObj, int nUserMaxLevel, int nUserMaxReset)
{
	_stMonsterStatRef monsterStatRef;
	LPMONSTER_ATTRIBUTE lpm;

	if( this->m_bLoaded == false )
		return false;

	if( lpObj == NULL )
		return false;

	lpm = gMAttr.GetAttr(lpObj->Class);

	if( lpm == NULL )
	{
		LogAdd(lMsg.Get(MSGGET(1,253)),lpObj->Class,__FILE__,__LINE__);
		return false;
	}

	if( lpm->m_Level == 0 )
	{
		LogAdd(lMsg.Get(MSGGET(1,254)),lpObj->Class,__FILE__,__LINE__);
		return false;
	}

	int nSize = this->m_vtMonsterStatRef.size();

	for(int i = 0; i < nSize; i++)
	{
		monsterStatRef = this->m_vtMonsterStatRef.at(i);

		if( nUserMaxLevel >= monsterStatRef.m_MinLevel &&
			nUserMaxLevel <= monsterStatRef.m_MaxLevel )
		{
			if( nUserMaxReset >= monsterStatRef.m_MinReset &&
				nUserMaxReset <= monsterStatRef.m_MaxReset )
			{ 
			strncpy(lpObj->Name,lpm->m_Name,10);
			lpObj->Level = monsterStatRef.m_LevelMul*lpm->m_Level;
			lpObj->Life = monsterStatRef.m_LifeMul*lpm->m_Hp;
			lpObj->MaxLife = monsterStatRef.m_LifeMul*lpm->m_Hp;
			lpObj->Mana = monsterStatRef.m_ManaMul*lpm->m_Mp;
			lpObj->MaxMana = monsterStatRef.m_ManaMul*lpm->m_Mp;
			lpObj->m_AttackDamageMin = monsterStatRef.m_AttackMinMul*lpm->m_DamageMin;
			lpObj->m_AttackDamageMax = monsterStatRef.m_AttackMaxMul*lpm->m_DamageMax;
			lpObj->m_Defense = monsterStatRef.m_DefenseMul*lpm->m_Defense;
			lpObj->m_MagicDefense = monsterStatRef.m_MagicDefenseMul*lpm->m_MagicDefense;
			lpObj->m_AttackRating = monsterStatRef.m_AttackRatingMul*lpm->m_AttackRating;
			lpObj->m_SuccessfulBlocking = monsterStatRef.m_SuccessfulBlockingMul*lpm->m_Successfulblocking;
			lpObj->m_MoveRange = lpm->m_MoveRange;
			lpObj->m_AttackSpeed = lpm->m_AttackSpeed;
			lpObj->m_MoveSpeed = lpm->m_MoveSpeed;
			lpObj->MaxRegenTime = lpm->m_RegenTime*1000;
			lpObj->m_AttackRange = lpm->m_AttackRange;
			lpObj->m_ViewRange = lpm->m_ViewRange;
			lpObj->m_Attribute = lpm->m_Attribute;
			lpObj->m_AttackType = lpm->m_AttackType;
			lpObj->m_ItemRate = lpm->m_ItemRate;
			lpObj->m_MoneyRate = lpm->m_MoneyRate;
			// ? strange setting ... 
			lpObj->Dexterity = 0;
			lpObj->m_Resistance[0] = lpm->m_Resistance[0];
			lpObj->m_Resistance[1] = lpm->m_Resistance[1];
			lpObj->m_Resistance[2] = lpm->m_Resistance[2];
			lpObj->m_Resistance[3] = lpm->m_Resistance[3];
			lpObj->m_iScriptMaxLife = lpm->m_iScriptHP;
			lpObj->m_iBasicAI = lpm->m_iAINumber;
			lpObj->m_iCurrentAI = lpm->m_iAINumber;
			lpObj->m_iCurrentAIState = 0;
			lpObj->m_iLastAIRunTime = 0;
			lpObj->m_iGroupNumber = 0;
			lpObj->m_iSubGroupNumber = 0;
			lpObj->m_iGroupMemberGuid = -1;
			lpObj->m_iRegenType = 0;
			lpObj->m_Agro.ResetAll();
			lpObj->m_iLastAutomataRuntime = 0;
			lpObj->m_iLastAutomataDelay = 0;

			memcpy(lpObj->m_Resistance,lpm->m_Resistance,7);

			gObjGiveItemSearch(lpObj,lpm->m_MaxItemLevel);
			gObjGiveItemWarehouseSearch(lpObj,lpm->m_MaxItemLevel);
			break;
			}
		}
	}
	return false;
}