// SkillAdditionInfo.cpp: implementation of the CSkillAdditionInfo class.
//	GS-N	1.00.18	JPN	0x00485060	-	Completed
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkillAdditionInfo.h"
#include "LogProc.h"
#include "Gamemain.h"

CSkillAdditionInfo g_SkillAdditionInfo;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkillAdditionInfo::CSkillAdditionInfo()
{
	this->Init();
}

CSkillAdditionInfo::~CSkillAdditionInfo()
{
	return;
}



void CSkillAdditionInfo::Init()
{
	this->m_iInfinityArrowSkillTime = 0;
	this->m_iInfinityArrowUseLevel = 0;
	this->m_iInfinityArrowMPConsumptionPlus0 = 0;
	this->m_iInfinityArrowMPConsumptionPlus1 = 0;
	this->m_iInfinityArrowMPConsumptionPlus2 = 0;
}



BOOL CSkillAdditionInfo::Load(LPSTR lpszFileName)
{
	if ( lpszFileName == NULL || strcmp(lpszFileName, "") == 0 )
	{
		LogAddTD("[Skill Addition Info] - File load error : File Name Error");
		return FALSE;
	}

	this->m_iInfinityArrowSkillTime = GetPrivateProfileInt("SkillInfo", "InfinityArrowSkillTime", 600, gDirPath.GetNewPath(lpszFileName)); 
	this->m_iInfinityArrowUseLevel = GetPrivateProfileInt("SkillInfo", "InfinityArrowUseLevel", 220, gDirPath.GetNewPath(lpszFileName)); 
	this->m_iInfinityArrowMPConsumptionPlus0 = GetPrivateProfileInt("SkillInfo", "InfinityArraowMPConsumptionPlus0", 0, gDirPath.GetNewPath(lpszFileName));
	this->m_iInfinityArrowMPConsumptionPlus1 = GetPrivateProfileInt("SkillInfo", "InfinityArraowMPConsumptionPlus1", 0, gDirPath.GetNewPath(lpszFileName));
	this->m_iInfinityArrowMPConsumptionPlus2 = GetPrivateProfileInt("SkillInfo", "InfinityArraowMPConsumptionPlus2", 0, gDirPath.GetNewPath(lpszFileName));


	LogAddTD("[Skill Addition Info] [Infinity Arrow] Skill Time[%d], UseLevel[%d], Arrow MP Consumption +0[%d] +1[%d] +2[%d]",
		this->m_iInfinityArrowSkillTime, this->m_iInfinityArrowUseLevel,
		this->m_iInfinityArrowMPConsumptionPlus0, this->m_iInfinityArrowMPConsumptionPlus1,
		this->m_iInfinityArrowMPConsumptionPlus2);

	this->m_bFireScreamSkill = GetPrivateProfileInt("SkillInfo", "FireScreamSkill", 1, gDirPath.GetNewPath(lpszFileName));
	this->m_iFireScreamExplosionAttackDistance = GetPrivateProfileInt("SkillInfo", "FireScreamExplosionAttackDistance", 1, gDirPath.GetNewPath(lpszFileName));
	this->m_iFireScreamExplosionRate = GetPrivateProfileInt("SkillInfo", "FireScreamExplosionRate", 300, gDirPath.GetNewPath(lpszFileName));
	this->m_iFireScreamMaxAttackCountSameSerial = GetPrivateProfileInt("SkillInfo", "FireScreamMaxAttackCountSameSerial", 3, gDirPath.GetNewPath(lpszFileName));
	this->m_iFireScreamSerialInitCount = GetPrivateProfileInt("SkillInfo", "FireScreamSerialInitCount", 100, gDirPath.GetNewPath(lpszFileName));
	this->m_bCheckFireScreamSerial = GetPrivateProfileInt("SkillInfo", "CheckFireScreamSerial", 1, gDirPath.GetNewPath(lpszFileName));


	LogAddTD("[Skill Addition Info] [Fire Scream] Use(%d) : Explosion Distance(%d) - Rate(%d), MaxAttack_SameSerial(%d), SerialInitCount(%d), Check Serial(%d)",
		this->m_bFireScreamSkill, this->m_iFireScreamExplosionAttackDistance,
		this->m_iFireScreamExplosionRate, this->m_iFireScreamMaxAttackCountSameSerial,
		this->m_iFireScreamSerialInitCount, this->m_bCheckFireScreamSerial);

	BYTE defVal = 0;

	for(int i = 1; i < 21; i++)
	{
		char szTmp[128];
		wsprintf(szTmp,"SoulBarrierManaRate_Level%d",i);

		defVal = 20+i;

		this->m_SB_ManaRate[i] = GetPrivateProfileInt("SkillInfo", szTmp, defVal, gDirPath.GetNewPath(lpszFileName));
	}


	LogAddTD("[Skill Addition Info] [SoulBarrier Strengrhen] BaseSkill(Level0):%d, Level1:%d, 2:%d, 3:%d, 4:%d, 5:%d",
		this->m_SB_ManaRate[0],this->m_SB_ManaRate[1],this->m_SB_ManaRate[2],this->m_SB_ManaRate[3],this->m_SB_ManaRate[4],this->m_SB_ManaRate[5]);

	this->m_iBloodStormExplosionAttackDistance = GetPrivateProfileInt("SkillInfo", "BloodStormExplosionAttackDistance", 3, gDirPath.GetNewPath(lpszFileName));
	this->m_iBloodStormExplosionRate = GetPrivateProfileInt("SkillInfo", "BloodStormExplosionRate", 1000, gDirPath.GetNewPath(lpszFileName));
	this->m_iBloodStormExplosionDamage = GetPrivateProfileInt("SkillInfo", "BloodStormExplosionDamageProc", 70, gDirPath.GetNewPath(lpszFileName));

	LogAddTD("[Skill Addition Info] [Blood Storm] Explosion Distance(%d) - Rate(%d),DamageProc(%d)",
		this->m_iBloodStormExplosionAttackDistance,this->m_iBloodStormExplosionRate,this->m_iBloodStormExplosionDamage);

	return TRUE;
}

