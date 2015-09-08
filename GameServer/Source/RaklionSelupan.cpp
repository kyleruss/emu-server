// RaklionSelupanMng.cpp: implementation of the CRaklionSelupanMng class.
// GS	1.00.90	JPN	0xXXXXXXXX	-	Completed
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaklionSelupan.h"
#include "User.h"
#include "TmonsterSkillManager.h"
#include "LogProc.h"
#include "gObjMonster.h"
#include "GameMain.h"
#include "MagicInf.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaklionSelupan::CRaklionSelupan()
{
	this->ClearData();
}

CRaklionSelupan::~CRaklionSelupan()
{

}

void CRaklionSelupan::ClearData()
{
	this->m_iSelupanObjIndex	= -1;
	this->m_iBerserkIndex		= 0;
	this->m_iBossAttackMin		= 0;
	this->m_iBossAttackMax		= 0;
}

void CRaklionSelupan::SelupanAct_FirstSkill()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = this->GetSelupanTargetIndex();
	
	LPOBJ lpObj = &gObj[aIndex];

	if ( tIndex == -1 )
	{
		tIndex = aIndex;
	}

	//if ( tIndex != -1 )
	{
		CMagicInf lpMagic;
		memset(&lpMagic, 0, sizeof(lpMagic));
		lpMagic.m_Skill = 253;
		TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 31, 0,&lpMagic);
	}

	LogAddTD("[ RAKLION ][ FirstSkill ] Skill Using : Index(%d) %s", aIndex,lpObj->Name);
}

void CRaklionSelupan::SelupanAct_PoisonAttack()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = this->GetSelupanTargetIndex();

	LPOBJ lpObj = &gObj[aIndex];


	if ( tIndex == -1 )
	{
		tIndex = aIndex;
		TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 7, NULL);
	}
	else
	{
		CMagicInf lpMagic;
		memset(&lpMagic, 0, sizeof(lpMagic));
		lpMagic.m_Skill = 250;
		TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 1,&lpMagic);
	}

	LogAddTD("[ RAKLION ][ PoisonAttack ] Skill Using : Index(%d) %s", aIndex,lpObj->Name);

}

void CRaklionSelupan::SelupanAct_IceStorm()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = this->GetSelupanTargetIndex();

	LPOBJ lpObj = &gObj[aIndex];

	if ( tIndex == -1 )
	{
		tIndex = aIndex;
		TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 7, NULL);
	}
	else
	{
		CMagicInf lpMagic;

		memset(&lpMagic, 0, sizeof(lpMagic));

		lpMagic.m_Skill = 251;
		TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 2,&lpMagic);
	}
	LogAddTD("[ RAKLION ][ IceStorm ] Skill Using : Index(%d) %s", aIndex,lpObj->Name);
}

void CRaklionSelupan::SelupanAct_IceStrike()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = this->GetSelupanTargetIndex();

	LPOBJ lpObj = &gObj[aIndex];

	if ( tIndex == -1 )
	{
		tIndex = aIndex;
		TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 7, NULL);
	}
	else
	{
		CMagicInf lpMagic;
		memset(&lpMagic, 0, sizeof(lpMagic));
		lpMagic.m_Skill = 252;
		TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 3,&lpMagic);
	}

	LogAddTD("[ RAKLION ][ IceStrike ] Skill Using : Index(%d) %s", aIndex,lpObj->Name);
}

void CRaklionSelupan::SelupanAct_SummonMonster()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = aIndex;

	LPOBJ lpObj = &gObj[aIndex];
	
	TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 30, 4, NULL);
	
	this->CreateSummonMonster();
	
	LogAddTD("[ RAKLION ][ SummonMonster ] Skill Using : Index(%d) %s", aIndex,lpObj->Name);
}

void CRaklionSelupan::SelupanAct_Heal()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = aIndex;

	LPOBJ lpObj = &gObj[aIndex];

	TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 20, 5, NULL);

	LogAddTD("[ RAKLION ][ Heal ] Skill Using : Index(%d) %s", aIndex,lpObj->Name);
}

void CRaklionSelupan::SelupanAct_Freeze()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = this->GetSelupanTargetIndex();

	LPOBJ lpObj = &gObj[aIndex];

	if ( tIndex == -1 )
	{
		tIndex = aIndex;
		TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 7, NULL);
	}
	else
	{
		TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 6, NULL);
	}
	LogAddTD("[ RAKLION ][ Freeze ] Skill Using : Index(%d) %s", aIndex,lpObj->Name);
}

void CRaklionSelupan::SelupanAct_Teleport()
{
	return;

	int aIndex = this->GetSelupanObjIndex();
	int tIndex = this->GetSelupanTargetIndex();

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[tIndex];
	
	if ( tIndex < 0 )
	{
		tIndex = aIndex;
	}

	TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 7, NULL);
	LogAddTD("[ RAKLION ][ Teleport ] Skill Using : Index(%d) %s", aIndex,lpObj->Name);
}

void CRaklionSelupan::SelupanAct_Invincibility()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = aIndex;

	LPOBJ lpObj = &gObj[aIndex];
	
	TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 10, 8, NULL);
	
	LogAddTD("[ RAKLION ][ Invincibility ] Skill Using : Index(%d) %s", aIndex,lpObj->Name);

}

void CRaklionSelupan::SelupanAct_BerserkCansel()
{
	int aIndex = this->GetSelupanObjIndex();
	
	LPOBJ lpObj = &gObj[aIndex];
	
	lpObj->m_AttackDamageMin = this->m_iBossAttackMin;
	lpObj->m_AttackDamageMax = this->m_iBossAttackMax;
	
	LogAddTD("[ RAKLION ][ Berserk Cancel ] MinDamage(%d) Damage(%d~%d)",lpObj->m_AttackDamageMin,lpObj->m_AttackDamageMin,lpObj->m_AttackDamageMax);

}

void CRaklionSelupan::SelupanAct_Berserk1()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = aIndex;

	LPOBJ lpObj = &gObj[aIndex];

	TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 9, NULL);
	
	this->m_iBerserkIndex = 1;

	int loc5 = 0;
	loc5 = this->m_iBerserkIndex * lpObj->m_MonsterSkillElementInfo.m_iSkillElementAttack;

	lpObj->m_AttackDamageMin = this->m_iBossAttackMin + loc5;
	lpObj->m_AttackDamageMax = this->m_iBossAttackMax + loc5;


	LogAddTD("[ RAKLION ][ Berserk1 ] Skill Using : Index(%d) Damage(%d~%d)",aIndex,lpObj->m_AttackDamageMin,lpObj->m_AttackDamageMax);
}

void CRaklionSelupan::SelupanAct_Berserk2()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = aIndex;

	LPOBJ lpObj = &gObj[aIndex];

	TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 9, NULL);
	
	this->m_iBerserkIndex = 2;

	int loc5 = 0;
	loc5 = this->m_iBerserkIndex * lpObj->m_MonsterSkillElementInfo.m_iSkillElementAttack;

	lpObj->m_AttackDamageMin = this->m_iBossAttackMin + loc5;
	lpObj->m_AttackDamageMax = this->m_iBossAttackMax + loc5;


	LogAddTD("[ RAKLION ][ Berserk2 ] Skill Using : Index(%d) Damage(%d~%d)",aIndex,lpObj->m_AttackDamageMin,lpObj->m_AttackDamageMax);
}

void CRaklionSelupan::SelupanAct_Berserk3()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = aIndex;

	LPOBJ lpObj = &gObj[aIndex];

	TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 9, NULL);
	
	this->m_iBerserkIndex = 3;

	int loc5 = 0;
	loc5 = this->m_iBerserkIndex * lpObj->m_MonsterSkillElementInfo.m_iSkillElementAttack;

	lpObj->m_AttackDamageMin = this->m_iBossAttackMin + loc5;
	lpObj->m_AttackDamageMax = this->m_iBossAttackMax + loc5;


	LogAddTD("[ RAKLION ][ Berserk3 ] Skill Using : Index(%d) Damage(%d~%d)",aIndex,lpObj->m_AttackDamageMin,lpObj->m_AttackDamageMax);
}

void CRaklionSelupan::SelupanAct_Berserk4()
{
	int aIndex = this->GetSelupanObjIndex();
	int tIndex = aIndex;

	LPOBJ lpObj = &gObj[aIndex];

	TMonsterSkillManager::UseMonsterSkill(aIndex, tIndex, 1, 9, NULL);
	
	this->m_iBerserkIndex = 4;

	int loc5 = 0;
	loc5 = this->m_iBerserkIndex * lpObj->m_MonsterSkillElementInfo.m_iSkillElementAttack;

	lpObj->m_AttackDamageMin = this->m_iBossAttackMin + loc5;
	lpObj->m_AttackDamageMax = this->m_iBossAttackMax + loc5;


	LogAddTD("[ RAKLION ][ Berserk4 ] Skill Using : Index(%d) Damage(%d~%d)",aIndex,lpObj->m_AttackDamageMin,lpObj->m_AttackDamageMax);
}

DWORD CRaklionSelupan::GetSelupanLife()
{
	int aIndex = this->GetSelupanObjIndex();
	
	LPOBJ lpObj = &gObj[aIndex];

	return lpObj->Life;
}

void CRaklionSelupan::SetSelupanLife(int Percent)
{
	int aIndex = this->GetSelupanObjIndex();

	LPOBJ lpObj = &gObj[aIndex];
	
	float loc4 = lpObj->AddLife + lpObj->MaxLife;

	lpObj->Life = loc4 * Percent / 100;
}

void CRaklionSelupan::InitSelupanObjIndex()
{
	for(int i = 0;i < OBJ_MAXMONSTER;i++)
	{
		if( gObj[i].Connected == PLAYER_PLAYING )
		{
			if( gObj[i].Class == 459)
			{
				this->m_iSelupanObjIndex = i;
			}
		}	
	}

	if( this->m_iSelupanObjIndex == -1 )
	{
		LogAddC(2,"[ RAKLION ] [ InitSelupanIndex ] error : SelupanObjIndex(%d)",this->m_iSelupanObjIndex);
	}
}

BOOL CRaklionSelupan::CreateSelupan()
{
	int m_Type = -1;

	for ( int n=0;n<gMSetBase.m_Count;n++)
	{
		if( gMSetBase.m_Mp[n].m_Type == 459 )
		{
			m_Type = n;
		}
	}

	LPOBJ lpObj = NULL;
	int result = 0;


	result = gObjAddMonster(MAP_INDEX_RAKLIONBOSS);

	if ( result < 0 )
	{
		LogAddTD("[RAKLION] CreateSelupan error");
		return FALSE;
	}
	
	lpObj = &gObj[result];
	
	gObjSetPosMonster(result, m_Type);
	
	gObjSetMonster(result, 459); 
	
	LogAddTD("[RAKLION] Create Selupan : X (%d) / Y(%d)",lpObj->X,lpObj->Y);

	LPOBJ lpTargetObj = NULL;

	for (int n=0;n<OBJMAX;n++)
	{
		lpTargetObj = &gObj[n];

		if(lpTargetObj->MapNumber == MAP_INDEX_RAKLIONBOSS)
		{
			gObjStateSetCreate(n);
		}
	}

	for (int n=0;n<OBJMAX;n++)
	{
		lpTargetObj = &gObj[n];

		if(lpTargetObj->MapNumber == MAP_INDEX_RAKLIONBOSS)
		{
			gObjViewportListDestroy(n);
		}
	}
	
	for (int n=0;n<OBJMAX;n++)
	{
		lpTargetObj = &gObj[n];

		if(lpTargetObj->MapNumber == MAP_INDEX_RAKLIONBOSS)
		{
			gObjViewportListCreate(n);
		}
	}
	
	for (int n=0;n<OBJMAX;n++)
	{
		lpTargetObj = &gObj[n];

		if(lpTargetObj->MapNumber == MAP_INDEX_RAKLIONBOSS)
		{
			gObjViewportListProtocol(n);
		}
	}

	gObjSetState();

	this->m_iBossAttackMin = lpObj->m_AttackDamageMin;
	this->m_iBossAttackMax = lpObj->m_AttackDamageMax;

	return TRUE;
}

void CRaklionSelupan::DeleteSelupan()
{
	int aIndex = this->GetSelupanObjIndex();
	gObjViewportListProtocolDestroy(&gObj[aIndex]);
	gObjViewportClose(&gObj[aIndex]);
	gObjDel(aIndex);
}

int CRaklionSelupan::GetSelupanTargetIndex()
{
	int iSelupanIndex = this->GetSelupanObjIndex();

	LPOBJ lpObj = &gObj[iSelupanIndex];

	
	if ( lpObj->TargetNumber >= 0 && lpObj->TargetNumber < OBJMAX ) 
	{
		return lpObj->TargetNumber;
	}

	return gObjMonsterSearchEnemy(lpObj,lpObj->Type);
}

void CRaklionSelupan::SetSelupanSkillDelay(int SkillDelay)
{
	if ( SkillDelay < 0 || SkillDelay > 50000 )
	{
		LogAddC(2,"[ RAKLION ][ SetSelupanSkillDelay ] SkillDelay error : Delay(%d)",this->m_iSelupanSkillDelay);
		return;
	}
	
	this->m_iSelupanSkillDelay = SkillDelay;
}

int CRaklionSelupan::GetSelupanSkillDelay()
{
	if ( this->m_iSelupanSkillDelay < 0 || this->m_iSelupanSkillDelay > 50000 )
	{
		LogAddC(2,"[ RAKLION ][ GetSelupanSkillDelay ] SkillDelay error : Delay(%d)",this->m_iSelupanSkillDelay);
		return -1;
	}

	return this->m_iSelupanSkillDelay;
}

int CRaklionSelupan::GetBerserkIndex()
{
	return this->m_iBerserkIndex;
}

void CRaklionSelupan::SetBossAttackDamage(int DamageMin,int DamageMax)
{
	this->m_iBossAttackMin = DamageMin;
	this->m_iBossAttackMax = DamageMax;
}

void CRaklionSelupan::SetSelupanObjIndex(int arg1)
{
	if ( arg1 < 0 || arg1 > OBJ_MAXMONSTER ) 
	{
		LogAddC(2,"[ RAKLION ][ SetSelupanObjIndex ] SelupanObjIndex error : Index(%d)", arg1);
		return;
	}

	this->m_iSelupanObjIndex = arg1;
}

int CRaklionSelupan::GetSelupanObjIndex()
{
	this->InitSelupanObjIndex();

	return this->m_iSelupanObjIndex;
}

void CRaklionSelupan::CreateSummonMonster()
{
	LPOBJ lpObj = NULL;//loc2 
	int loc3 = -1;
	int loc4 = -1;

	for (int i = 0; i < OBJ_MAXMONSTER; ++i )
	{
		lpObj = &gObj[i];

		if ( lpObj->MapNumber != MAP_INDEX_RAKLIONBOSS )
		{
			continue;
		}

		if(lpObj->Class == 457 || lpObj->Class == 458)
		{
			if(lpObj->Connected == 3 && lpObj->DieRegen == 0 && lpObj->Live == 1) 
			{
				continue;
			}

			loc4 = lpObj->Class;
			loc3 = lpObj->m_PosNum;
			
			gObjCharZeroSet(i);
			
			gObjDel(lpObj->m_Index);

			lpObj->Class = 0;

			int iMonsterIndex = gObjAddMonster(MAP_INDEX_RAKLIONBOSS);

			lpObj = &gObj[iMonsterIndex];

			if (iMonsterIndex >= 0)
			{
				gObjSetPosMonster(iMonsterIndex, loc3);
				gObjSetMonster(iMonsterIndex, loc4); 
				lpObj->Live = 1;
				lpObj->DieRegen = 0;
				LogAddTD("[RAKLION] Summoned Monster : %s => X-(%d) Y-(%d)",lpObj->Name,lpObj->X,lpObj->Y);
				return;
			}

			LogAddTD("[RAKLION] CreateSelupan error");	
			return;
		}
	}
}

void CRaklionSelupan::DeleteSummonMonster()
{
	LPOBJ lpObj = NULL;

	for(int i=0;i< OBJ_MAXMONSTER;i++)
	{
		lpObj = &gObj[i];

		if(lpObj->MapNumber != MAP_INDEX_RAKLIONBOSS)
		{
			continue;
		}
			
		if(lpObj->Class == 457 || lpObj->Class == 458)
		{
			gObjViewportListProtocolDestroy(lpObj);
			gObjViewportClose(lpObj);
			gObjDel(lpObj->m_Index);
			lpObj->Live = FALSE; 
			lpObj->DieRegen = TRUE;
		}
	}

	LogAddC(5,"[ RAKLION ] [ DeleteSummonMonster ] Delete All SummonMonster");
}