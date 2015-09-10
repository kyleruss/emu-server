#include "stdafx.h"
#include "MasterLevelSkillTreeSystem.h"
#include "logproc.h"
#include "GameMain.h"
#include "LuaFun.h"
#include "MasterSkillSystem.h"
#include "ObjCalCharacter.h"
#include "SkillAdditionInfo.h"
#include "BuffEffectSlot.h"
#include "ChaosCastle.h"
#include "CastleSiege.h"
#include "ObjUseSkill.h"
#include "SkillHitBox.h"
#include "zzzmathlib.h"
#include "IllusionTempleEvent.h"
#include "ItemAddOption.h"
#include "..\\common\\classdef.h"
#include "..\\include\\readscript.h"
#include "..\\common\\winutil.h"
#include "BuffSkillEffect.h"

struct _stCharacterTable
{
	int ClassCode;
	int Class;
} CHARACTER_TABLE[] = {
	2,	CLASS_WIZARD,
	1,	CLASS_KNIGHT,
	4,	CLASS_ELF,
	16, CLASS_MAGUMSA,
	32, CLASS_DARKLORD,
	8,	CLASS_SUMMONER,
	64, CLASS_FIGHTER,
};

CMasterLevelSkillTreeSystem g_MasterSkillSystem;

CMasterLevelSkillTreeSystem::CMasterLevelSkillTreeSystem()
{
	this->m_LuaLoaded = false;
}


CMasterLevelSkillTreeSystem::~CMasterLevelSkillTreeSystem()
{

}

void CMasterLevelSkillTreeSystem::Init()
{
	if( this->m_LuaLoaded )
	{
		return;
	}

	memset(this->m_SkillTreeData,0,sizeof(this->m_SkillTreeData));
	memset(this->m_fValue,0,sizeof(this->m_fValue));

	lua_State* pLuaState = this->m_Lua.GetLua();

	if( pLuaState == NULL )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] - Error - [%s] [%d]",__FILE__,__LINE__);
		return;
	}

	this->m_Lua.DoFile(gDirPath.GetNewPath(FILE_SKILL_MASTERTREE_EFFECT));
	lua_State* L = this->m_Lua.GetLua();

	double fValue;

	for(int i = 1; i <= MAX_SKILLTREE_LEVEL; i++)
	{
		g_Generic_Call(L,"FN_LuaGetPercent","i>d",i,&fValue);
		this->m_fValue[1][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetPercent2","i>d",i,&fValue);
		this->m_fValue[2][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetSkillDamageMin","i>d",i,&fValue);
		this->m_fValue[3][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetSkillDamageMiddle","i>d",i,&fValue);
		this->m_fValue[4][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetSkillDamageMax","i>d",i,&fValue);
		this->m_fValue[5][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetMonsterKill","i>d",i,&fValue);
		this->m_fValue[6][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetPercent3","i>d",i,&fValue);
		this->m_fValue[7][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetMaxAG","i>d",i,&fValue);
		this->m_fValue[8][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetMaxLifeMana","i>d",i,&fValue);
		this->m_fValue[9][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetAttack","i>d",i,&fValue);
		this->m_fValue[10][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetSDMonsterKill","i>d",i,&fValue);
		this->m_fValue[11][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetBlockingRate","i>d",i,&fValue);
		this->m_fValue[12][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetAttackSuccessRate","i>d",i,&fValue);
		this->m_fValue[13][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetManAttackRate","i>d",i,&fValue);
		this->m_fValue[14][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetPercent4","i>d",i,&fValue);
		this->m_fValue[15][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetIncreaseBlocking","i>d",i,&fValue);
		this->m_fValue[16][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetIncreaseStat","i>d",i,&fValue);
		this->m_fValue[17][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetDecreaseMana","i>d",i,&fValue);
		this->m_fValue[18][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetChangeBlocking","i>d",i,&fValue);
		this->m_fValue[19][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetControlAttack","i>d",i,&fValue);
		this->m_fValue[20][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetManaIncRate","i>d",i,&fValue);
		this->m_fValue[21][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetSkillDamageMin2","i>d",i,&fValue);
		this->m_fValue[22][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetAttackSpeed","i>d",i,&fValue);
		this->m_fValue[23][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaBloodStorm","i>d",i,&fValue);
		this->m_fValue[25][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaEarthPrison","i>d",i,&fValue);
		this->m_fValue[26][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaPoisonArrow","i>d",i,&fValue);
		this->m_fValue[27][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaIronDefense","i>d",i,&fValue);
		this->m_fValue[28][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetBlockingRateFighter","i>d",i,&fValue);
		this->m_fValue[29][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetAttackSuccessRateFighter","i>d",i,&fValue);
		this->m_fValue[30][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetManAttackRateFighter","i>d",i,&fValue);
		this->m_fValue[31][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetMaxSDFighter","i>d",i,&fValue);
		this->m_fValue[33][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetMaxLifeManaFighter","i>d",i,&fValue);
		this->m_fValue[34][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetIncreaseBlockingFighter","i>d",i,&fValue);
		this->m_fValue[35][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetIncreaseStatFighter","i>d",i,&fValue);
		this->m_fValue[36][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetMaxAGFighter","i>d",i,&fValue);
		this->m_fValue[37][i] = this->RoundValue(fValue,2);

		g_Generic_Call(L,"FN_LuaGetPercent5","i>d",i,&fValue);
		this->m_fValue[38][i] = this->RoundValue(fValue,2);
	}

	this->m_Lua.Release();
	this->m_LuaLoaded = true;
}

void CMasterLevelSkillTreeSystem::Load(char const * lpszFileName)
{
	if( lpszFileName == NULL || !strcmp(lpszFileName,"") )
	{
		MsgBox("[MasterLevelSkillTreeSystem] - File load error : File Name Error");
		return;
	}

	try
	{
		SMDFile = fopen(lpszFileName,"r");

		if( SMDFile == NULL )
		{
			MsgBox("[MasterLevelSkillTreeSystem] - Can't Open %s ",lpszFileName);
			return;
		}

		this->Init();
		SMDToken Token;
		int iIndex = -1;

		while( TRUE )
		{
			if( !strcmp("end",TokenString) )
				break;

			while( TRUE )
			{
				Token = GetToken();

				if( !strcmp("end",TokenString) )
					break;

				Token = GetToken();

				iIndex = (int)TokenNumber;
				int iTempIndex = iIndex%36-1;
				int iRank = iTempIndex/4;
				int iPos = iTempIndex%4;

				Token = GetToken();
				int iClassType = (WORD)TokenNumber;

				Token = GetToken();
				int iTreeKind = TokenNumber;

				Token = GetToken();
				int iPoint = TokenNumber;

				Token = GetToken();
				int iMaxPoint = TokenNumber;

				Token = GetToken();

				Token = GetToken();
				int iParentSkill1 = TokenNumber;

				Token = GetToken();
				int iParentSkill2 = TokenNumber;

				Token = GetToken();
				int iSkillID = TokenNumber;

				Token = GetToken();

				for(int i = 0; i < MAX_SKILLTREE_CLASS; i++)
				{
					if( CHARACTER_TABLE[i].ClassCode&iClassType )
					{
						this->m_SkillTreeData[CHARACTER_TABLE[i].Class].m_SkillInfo[iTreeKind][iRank][iPos].m_iIndex = iIndex;
						this->m_SkillTreeData[CHARACTER_TABLE[i].Class].m_SkillInfo[iTreeKind][iRank][iPos].m_btPoint = iPoint;
						this->m_SkillTreeData[CHARACTER_TABLE[i].Class].m_SkillInfo[iTreeKind][iRank][iPos].m_btMaxPoint = iMaxPoint;
						this->m_SkillTreeData[CHARACTER_TABLE[i].Class].m_SkillInfo[iTreeKind][iRank][iPos].m_iParentSkill1 = iParentSkill1;
						this->m_SkillTreeData[CHARACTER_TABLE[i].Class].m_SkillInfo[iTreeKind][iRank][iPos].m_iParentSkill2 = iParentSkill2;
						this->m_SkillTreeData[CHARACTER_TABLE[i].Class].m_SkillInfo[iTreeKind][iRank][iPos].m_iSkillID = iSkillID;
					}
				}
			}
		}

		fclose(SMDFile);
		LogAddTD("[MasterLevelSkillTreeSystem] - %s file is Loaded",lpszFileName);
	}
	catch(int)
	{
		DWORD dwError = GetLastError();
		MsgBox("[MasterLevelSkillTreeSystem] - Loading Exception Error (%s) File. ",lpszFileName);
	}
}

void CMasterLevelSkillTreeSystem::InitPassiveSkillData(LPOBJ lpObj)
{
	memset(&lpObj->m_MPSkillOpt,0,sizeof(lpObj->m_MPSkillOpt));
}

void CMasterLevelSkillTreeSystem::CalcPassiveSkillData(LPOBJ lpObj,int iMLSkill,int iSkillLevel)
{
	int iReqStr = MagicDamageC.GetRequireStrength(iMLSkill);

	if( lpObj->Strength < iReqStr )
		return;

	int iReqDex = MagicDamageC.GetRequireDexterity(iMLSkill);

	if( lpObj->Dexterity < iReqDex )
		return;

	float iSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(iMLSkill), iSkillLevel);

	int iBaseMLS = this->GetBaseMasterLevelSkill(iMLSkill);

	switch( iBaseMLS )
	{
	case 325:
	case 599:
		lpObj->m_MPSkillOpt.MpsAttackSuccessRate = iSkillValue;
		break;
	case 347:
	case 603:
		lpObj->m_MPSkillOpt.MpsPVPAttackDmgRate = iSkillValue;
		break;
	case 301:
	case 579:
		lpObj->m_MPSkillOpt.MpsPVPBlockingRate = iSkillValue;
		break;
	case 300:
	case 578:
		lpObj->m_MPSkillOpt.MpsDownDur1 = iSkillValue;
		lpObj->m_MPSkillOpt.btMpsDownDur1Level = iSkillLevel;
		break;
	case 305:
	case 583:
		lpObj->m_MPSkillOpt.MpsDownDur2 = iSkillValue;
		break;
	case 312:
	case 590:
		lpObj->m_MPSkillOpt.MpsDownDur3 = iSkillValue;
		break;
	case 304:
	case 582:
		lpObj->m_MPSkillOpt.MpsResistancePoison = iSkillValue;
		break;
	case 308:
	case 586:
		lpObj->m_MPSkillOpt.MpsResistanceThunder = iSkillValue;
		break;
	case 311:
	case 589:
		lpObj->m_MPSkillOpt.MpsResistanceIce = iSkillValue;
		break;
	case 307:
	case 585:
		lpObj->m_MPSkillOpt.MpsAutoRecoverLife = iSkillValue;
		break;
	case 313:
	case 591:
		lpObj->m_MPSkillOpt.MpsBlockingRate = iSkillValue;
		break;
	case 309:
	case 587:
		lpObj->m_MPSkillOpt.MpsDefence = iSkillValue;
		break;
	case 334:
	case 600:
		lpObj->m_MPSkillOpt.MpsMaxHP = iSkillValue;
		break;
	case 338:
	case 601:
		lpObj->m_MPSkillOpt.MpsMaxMP = iSkillValue;
		break;
	case 341:
		lpObj->m_MPSkillOpt.MpsMaxBP = iSkillValue;
		break;
	case 362:
	case 608:
		lpObj->m_MPSkillOpt.MpsMonsterDieGetMana = iSkillValue;
		break;
	case 359:
	case 606:
		lpObj->m_MPSkillOpt.MpsMonsterDieGetLife = iSkillValue;
		break;
	case 358:
	case 605:
		lpObj->m_MPSkillOpt.MpsMonsterDieGetSD = iSkillValue;
		break;
	case 303:
	case 581:
		lpObj->m_MPSkillOpt.MpsAutoRecoverMana = iSkillValue;
		break;
	case 310:
	case 588:
		lpObj->m_MPSkillOpt.MpsAutoRecoverAG = iSkillValue;
		break;
	case 302:
	case 580:
		lpObj->m_MPSkillOpt.MpsMaxSD = iSkillValue;
		break;
	case 306:
	case 584:
		lpObj->m_MPSkillOpt.MpsSDSpeed = iSkillValue;
		break;
	/*case 364:
		lpObj->m_MPSkillOpt.MpsMaxAttackDamage = iSkillValue;
		break;
	case 361:
		lpObj->m_MPSkillOpt.MpsMinAttackDamage = iSkillValue;
		break;*/
	case 357:
	case 604:
		lpObj->m_MPSkillOpt.MpsDecreaseMana = iSkillValue;
		break;
	case 407:
		lpObj->m_MPSkillOpt.MpsMaxMagicDamage = iSkillValue;
		break;
	case 405:
		lpObj->m_MPSkillOpt.MpsMinMagicDamage = iSkillValue;
		break;
	case 535:
		lpObj->m_MPSkillOpt.MpsPetDurDownSpeed = iSkillValue;
		break;
	case 335:
	case 485:
	case 513:
	case 557:
		lpObj->m_MPSkillOpt.MpsWeaponMastery = iSkillValue;
		break;
	case 348:
		lpObj->m_MPSkillOpt.MpsTwoHandSwordStrength = iSkillValue;
		break;
	case 349:
		lpObj->m_MPSkillOpt.MpsOneHandSwordStrength = iSkillValue;
		break;
	case 350:
		lpObj->m_MPSkillOpt.MpsMaceStrength = iSkillValue;
		break;
	case 351:
		lpObj->m_MPSkillOpt.MpsSpearStrength = iSkillValue;
		break;
	case 352:
		lpObj->m_MPSkillOpt.MpsTwoHandSwordMastery = iSkillValue;
		break;
	case 353:
		lpObj->m_MPSkillOpt.MpsOneHandSwordMastery = iSkillValue;
		break;
	case 354:
		lpObj->m_MPSkillOpt.MpsMaceMastery = iSkillValue;
		break;
	case 355:
		lpObj->m_MPSkillOpt.MpsSpearMastery = iSkillValue;
		break;
	case 386:
	case 488:
		lpObj->m_MPSkillOpt.MpsMagicMastery = iSkillValue;
		break;
	case 397:
		lpObj->m_MPSkillOpt.MpsOneHandStaffStrength = iSkillValue;
		break;
	case 398:
		lpObj->m_MPSkillOpt.MpsTwoHandStaffStrength = iSkillValue;
		break;
	case 399:
		lpObj->m_MPSkillOpt.MpsShieldStrength = iSkillValue;
		break;
	case 400:
		lpObj->m_MPSkillOpt.MpsOneHandStaffMastery = iSkillValue;
		break;
	case 401:
		lpObj->m_MPSkillOpt.MpsTwoHandStaffMastery = iSkillValue;
		break;
	case 402:
		lpObj->m_MPSkillOpt.MpsShieldMastery = iSkillValue;
		break;
	case 435:
		lpObj->m_MPSkillOpt.MpsBowStrength = iSkillValue;
		break;
	case 438:
		lpObj->m_MPSkillOpt.MpsBowMastery = iSkillValue;
		break;
	case 436:
		lpObj->m_MPSkillOpt.MpsCrossbowStrength = iSkillValue;
		break;
	case 439:
		lpObj->m_MPSkillOpt.MpsCrossbowMastery = iSkillValue;
		break;
	case 421:
		lpObj->m_MPSkillOpt.MpsWeaponMastery2 = iSkillValue;
		break;
	case 415:
		lpObj->m_MPSkillOpt.MpsSummonMonster1 = iSkillValue;
		break;
	case 419:
		lpObj->m_MPSkillOpt.MpsSummonMonster2 = iSkillValue;
		break;
	case 428:
		lpObj->m_MPSkillOpt.MpsSummonMonster3 = iSkillValue;
		break;
	case 361:
	case 442:
	case 607:
		lpObj->m_MPSkillOpt.MpsMinAttackInc = iSkillValue;
		break;
	case 364:
	case 443:
		lpObj->m_MPSkillOpt.MpsMaxAttackInc = iSkillValue;
		break;
	case 448:
		lpObj->m_MPSkillOpt.MpsFireTomeStrength = iSkillValue;
		break;
	case 449:
		lpObj->m_MPSkillOpt.MpsWindTomeStrength = iSkillValue;
		break;
	case 450:
		lpObj->m_MPSkillOpt.MpsLightTomeStrength = iSkillValue;
		break;
	case 451:
		lpObj->m_MPSkillOpt.MpsFireTomeMastery = iSkillValue;
		break;
	case 452:
		lpObj->m_MPSkillOpt.MpsWindTomeMastery = iSkillValue;
		break;
	case 453:
		lpObj->m_MPSkillOpt.MpsLightTomeMastery = iSkillValue;
		break;
	case 457:
		lpObj->m_MPSkillOpt.MpsMagicMastery2 = iSkillValue;
		break;
	case 465:
		lpObj->m_MPSkillOpt.MpsStickStrength = iSkillValue;
		break;
	case 466:
		lpObj->m_MPSkillOpt.MpsTomeStrength = iSkillValue;
		break;
	case 467:
		lpObj->m_MPSkillOpt.MpsStickMastery = iSkillValue;
		break;
	case 468:
		lpObj->m_MPSkillOpt.MpsTomeMastery = iSkillValue;
		break;
	case 471:
		lpObj->m_MPSkillOpt.MpsMinWizCurseInc = iSkillValue;
		break;
	case 526:
		lpObj->m_MPSkillOpt.MpsDarkSpiritStr1 = iSkillValue;
		break;
	case 530:
		lpObj->m_MPSkillOpt.MpsDarkSpiritStr2 = iSkillValue;
		break;
	case 534:
		lpObj->m_MPSkillOpt.MpsDarkSpiritStr3 = iSkillValue;
		break;
	case 527:
		lpObj->m_MPSkillOpt.MpsScepterStrength = iSkillValue;
		break;
	case 531:
		lpObj->m_MPSkillOpt.MpsScepterMastery = iSkillValue;
		break;
	case 528:
		lpObj->m_MPSkillOpt.MpsShieldStrength3 = iSkillValue;
		break;
	case 532:
		lpObj->m_MPSkillOpt.MpsShieldMastery3 = iSkillValue;
		break;
	case 529:
		lpObj->m_MPSkillOpt.MpsUseScepterPetStr = iSkillValue;
		break;
	case 533:
		lpObj->m_MPSkillOpt.MpsCommandAttackInc = iSkillValue;
		break;
	case 568:
		lpObj->m_MPSkillOpt.MpsFistStrength = iSkillValue;
		break;
	case 571:
		lpObj->m_MPSkillOpt.MpsFistMastery = iSkillValue;
		break;

	case 315:
		lpObj->m_MPSkillOpt.MpsArmorSetBonusIncrease = iSkillValue;
		break;
	case 316:
		lpObj->m_MPSkillOpt.MpsReflectDamage = iSkillValue;
		break;
	case 317:
		lpObj->m_MPSkillOpt.MpsEnergyIncrease = iSkillValue;
		break;
	case 318:
		lpObj->m_MPSkillOpt.MpsVitalityIncrease = iSkillValue;
		break;
	case 319:
		lpObj->m_MPSkillOpt.MpsDexterityIncrease = iSkillValue;
		break;
	case 320:
		lpObj->m_MPSkillOpt.MpsStrengthIncrease = iSkillValue;
		break;
	case 506:
		lpObj->m_MPSkillOpt.MpsLeadershipIncrease = iSkillValue;
		break;
	case 322:
	case 375:
	case 410:
	case 446:
	case 476:
	case 505:
		lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = iSkillValue;
		break;
	case 324:
	case 377:
	case 412:
	case 447:
	case 478:
	case 507:
		lpObj->m_MPSkillOpt.MpsWingAttackIncrease = iSkillValue;
		break;
	case 366:
		lpObj->m_MPSkillOpt.MpsCriticalRateIncrease = iSkillValue;
		break;
	case 367:
		lpObj->m_MPSkillOpt.MpsRestoreFullMP = iSkillValue;
		break;
	case 368:
		lpObj->m_MPSkillOpt.MpsRestoreFullHP = iSkillValue;
		break;
	case 369:
		lpObj->m_MPSkillOpt.MpsExcelentRateIncrease = iSkillValue;
		break;
	case 372:
		lpObj->m_MPSkillOpt.MpsRestoreFullSD = iSkillValue;
		break;
	case 370:
		lpObj->m_MPSkillOpt.MpsDoubleDamageRate = iSkillValue;
		break;
	case 371:
		lpObj->m_MPSkillOpt.MpsIgnoreDef = iSkillValue;
		break;
	case 536:
		lpObj->m_MPSkillOpt.MpsDarkSpiritStr4 = iSkillValue;
		break;
	case 538:
		lpObj->m_MPSkillOpt.MpsDarkSpiritStr5 = iSkillValue;
		break;
	case 539:
		lpObj->m_MPSkillOpt.MpsDarkSpiritStr5 = iSkillValue;
		break;
	default:
		break;
	}
}

int CMasterLevelSkillTreeSystem::CheckSkillCondition(LPOBJ lpObj,int iMLSkill,int iSkillLevel)
{
	if( !gObjIsConnected(lpObj) )
	{
		GCResultSend(lpObj->m_Index,0x51,3);
		return 2;
	}

	if ( iMLSkill <0 || iMLSkill > MAX_SKILL -1 )
	{
		MsgBox(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return 3;
	}

	// g_MasterLevelSystem.IsMasterLevelUser

#if (__CUSTOM__ == 1)
	if( lpObj->bMasterInfoLoaded == FALSE || lpObj->ChangeUP2 != 1 )
	{
		return 2;
	}
#else
	if( !g_MasterExp.IsEnabled(lpObj) )
		return 2;
#endif

	if( !this->CheckMasterLevelSkill(iMLSkill) )
	{
		return 4;
	}

	if( this->GetMaxPointOfMasterSkill(lpObj->Class,iMLSkill) < iSkillLevel )
	{
		return 4;
	}

	int iReqMLPoint = this->GetRequireMLPoint(lpObj->Class,iMLSkill);

	if( iReqMLPoint < 0 )
	{
		return 4;
	}

	if( lpObj->MasterPoint < iReqMLPoint )
	{
		return 4;
	}

	if( iSkillLevel == 1 )
	{
		if( !this->CheckCompleteBrandOfSkill(lpObj,iMLSkill) )
			return 4;

		if( !this->CheckPreviousRankSkill(lpObj,iMLSkill,iSkillLevel) )
			return 4;
	}

return 1;
}

int CMasterLevelSkillTreeSystem::CheckCompleteBrandOfSkill(LPOBJ lpObj,int iMLSkill)
{
	int iSkillUseType = MagicDamageC.GetSkillUseType(iMLSkill);

	if( iSkillUseType == 4 )
	{
		WORD iBrandOfSkill = MagicDamageC.GetBrandOfSkill(iMLSkill);
		int iCompleteSkill = 0;

		if( iBrandOfSkill > 0 )
		{
			for(int iCount = 0; iCount < MAX_MAGIC; iCount++)
			{
				if( lpObj->Magic[iCount].IsMagic() == TRUE )
				{
					if( lpObj->Magic[iCount].m_Skill == iBrandOfSkill )
					{
						iCompleteSkill = TRUE;
						break;
					}
				}
			}
		}

		if( iCompleteSkill == 0 )
			return 0;
	}

	int iBrandOfSkill1 = this->GetParentSkill1(lpObj->Class,iMLSkill);
	int iBrandOfSkill2 = this->GetParentSkill2(lpObj->Class,iMLSkill);

	if( iBrandOfSkill1 <= 0 && iBrandOfSkill2 <= 0 )
		return true;

	int iCompleteSkill1 = 0;

	if( iBrandOfSkill1 > 0 )
	{
		for(int iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE )
			{
				if( lpObj->Magic[iCount].m_Skill == iBrandOfSkill1 &&
					lpObj->Magic[iCount].m_Level >= 10 )
				{
					iCompleteSkill1 = TRUE;
					break;
				}
			}
		}
	}

	int iCompleteSkill2 = 0;

	if( iBrandOfSkill2 > 0 )
	{
		for(int iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE )
			{
				if( lpObj->Magic[iCount].m_Skill == iBrandOfSkill2 &&
					lpObj->Magic[iCount].m_Level >= 10 )
				{
					iCompleteSkill2 = TRUE;
					break;
				}
			}
		}
	}

	int iSum = iBrandOfSkill1+iBrandOfSkill2;

	if( iSum == iBrandOfSkill1 )
	{
		if( iCompleteSkill1 != 0 )
			return true;
	}
	else if( iSum == iBrandOfSkill2 )
	{
		if( iCompleteSkill2 != 0 )
			return true;
	}
	else if( iCompleteSkill1 != 0 && iCompleteSkill2 != 0 )
	{
		return true;
	}

	return 0;
}

int CMasterLevelSkillTreeSystem::CheckMasterLevelSkill(int iMLSkill)
{
	int iSkillUseType = MagicDamageC.GetSkillUseType(iMLSkill);

	if( iSkillUseType == 3 || iSkillUseType == 4 )
		return true;
	return false;
}

int CMasterLevelSkillTreeSystem::IsExistMLSBasedSkill(LPOBJ lpObj,int iSkill)
{
	int iBaseMLS = 0;

	for(int iCount = 0; iCount < MAX_MAGIC; iCount++)
	{
		if( lpObj->Magic[iCount].IsMagic() == TRUE &&
			this->CheckMasterLevelSkill(lpObj->Magic[iCount].m_Skill) &&
			iSkill == MagicDamageC.GetBrandOfSkill(lpObj->Magic[iCount].m_Skill))
		{
			return true;
		}
	}
	return false;
}

int CMasterLevelSkillTreeSystem::GetBaseMasterLevelSkill(int iMLSkill)
{
	if( !this->CheckMasterLevelSkill(iMLSkill) )
		return -1;
	return iMLSkill;
}

int CMasterLevelSkillTreeSystem::gObjMagicAdd_MLS(LPOBJ lpObj,int iMLSkill,int iSkillLevel)
{
	int skill = -1;
	int n;

	if( g_EnergyCheckOff == 1 )
	{
		int reqeng = MagicDamageC.SkillGetRequireEnergy(iMLSkill);

		if( reqeng < 0 )
		{
			LogAdd(lMsg.Get(MSGGET(2,37)));
			return -1;
		}

		if( lpObj->Class == CLASS_KNIGHT )
		{
			reqeng -= 10;
		}
		
		if( lpObj->Class == CLASS_ELF )
		{
			reqeng -= 5;
		}
		
		if( lpObj->Class == CLASS_DARKLORD )
		{
			reqeng -= 5;
		}

		if( (lpObj->Energy+lpObj->AddEnergy) < reqeng )
		{
			LogAdd(lMsg.Get(MSGGET(2,37)));
			return -1;
		}
	}

	if( MagicDamageC.SkillGetRequireClass(lpObj->Class,lpObj->ChangeUP,lpObj->ChangeUP2,iMLSkill) < 1 )
	{
		LogAdd(lMsg.Get(MSGGET(2,38)));
		return -1;
	}

	for( n = 0; n < MAX_MAGIC; n++)
	{
		if( lpObj->Magic[n].IsMagic() == TRUE &&
			lpObj->Magic[n].m_Skill == iMLSkill &&
			lpObj->Magic[n].m_Level == iSkillLevel )
		{
			LogAdd(lMsg.Get(MSGGET(2,43)),lpObj->Magic[n].m_Skill,iMLSkill);
			return -1;
		}
	}

	if( MagicDamageC.GetSkillUseType(iMLSkill) == 3 )
	{
		if( iSkillLevel > 1 )
		{
			for( n = 0; n < MAX_MAGIC; n++)
			{
				if( lpObj->Magic[n].IsMagic() == TRUE &&
					lpObj->Magic[n].m_Skill == iMLSkill &&
					lpObj->Magic[n].m_Level == iSkillLevel-1 )
				{
					skill = lpObj->Magic[n].UpdateMasterSkill(iMLSkill,iSkillLevel);

					if( skill < 0 )
						return -1;

					return n;
				}
			}
		}
	}
	else if( MagicDamageC.GetSkillUseType(iMLSkill) == 4 )
	{
		WORD iBrand = MagicDamageC.GetBrandOfSkill(iMLSkill);

		for( n = 0; n < MAX_MAGIC; n++)
		{
			if( lpObj->Magic[n].IsMagic() == TRUE )
			{
				if( lpObj->Magic[n].m_Skill == iBrand )
				{
					lpObj->Magic[n].m_bBrand = TRUE;
				}

				if( lpObj->Magic[n].m_Skill == iMLSkill )
				{
					skill = lpObj->Magic[n].UpdateMasterSkill(iMLSkill,iSkillLevel);

					if( skill < 0 )
						return -1;

					return n;
				}
			}
		}
	}
	else
	{
		return -1;
	}

	for( n = 0; n < MAX_MAGIC; n++)
	{
		if( lpObj->Magic[n].IsMagic() == FALSE )
		{
			skill = lpObj->Magic[n].Set(iMLSkill,iSkillLevel);

			if( skill < 0 )
			{
				LogAdd(lMsg.Get(MSGGET(2,41)),__FILE__,__LINE__);
				return -1;
			}

			if( MagicDamageC.GetBrandOfSkill(iMLSkill) == 0 &&
				MagicDamageC.GetSkillUseType(iMLSkill) == 4 )
			{
				GCMagicListOneSend(lpObj->m_Index,n,iMLSkill,iSkillLevel,0,0);
			}

			lpObj->MagicCount++;
			return n;
		}
	}

	LogAdd(lMsg.Get(MSGGET(2,42)),__FILE__,__LINE__);
return -1;
}

#pragma pack(push)
#pragma pack(4)

struct PMSG_MASTERSKILL_COUNT
{
	PWMSG_HEAD2	h;
	int count;				// 8
};

struct MASTERSKILL_STRUCT
{
	BYTE btMasterSkillId;
	BYTE btLevel;
	float value;
	float nextvalue;
};

struct PMSG_ANS_MASTERLEVEL_SKILL
{
	PBMSG_HEAD2	h;
	BYTE btResult;			// 4
	WORD wLevelPoint;		// 6
	BYTE btMasterSkillId;	// 8
	DWORD dwSkillId;		// C
	int iLevel;			// 10
	float value;			// 14
	float nextvalue;		// 18
};

#pragma pack(pop)

void CMasterLevelSkillTreeSystem::CGReqGetMasterLevelSkillTree(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
	{
		CloseClient(aIndex);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_MASTERSKILL_COUNT pCountMsg;
	PHeadSubSetW((LPBYTE)&pCountMsg,0xF3,0x53,sizeof(pCountMsg));
	int nCount = 0;

	BYTE sendbuf[1024];
	memset(&sendbuf[0],0,sizeof(sendbuf));

	int lOfs = sizeof(pCountMsg);

	MASTERSKILL_STRUCT pElementMsg;

	for(int i = 0; i < MAX_MAGIC; i++)
	{
		if( lpObj->Magic[i].IsMagic() == TRUE &&
			this->CheckMasterLevelSkill(lpObj->Magic[i].m_Skill) == TRUE )
		{
			pElementMsg.btMasterSkillId = this->GetMasterSkillUIIndex(lpObj->Class,lpObj->Magic[i].m_Skill);
			pElementMsg.btLevel = lpObj->Magic[i].m_Level;

			if( this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[i].m_Skill),lpObj->Magic[i].m_Level) > 0.0f )
			{
				pElementMsg.value = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[i].m_Skill),lpObj->Magic[i].m_Level);
			}
			else
			{
				pElementMsg.value = -1.0f;
			}

			if( lpObj->Magic[i].m_Level < MAX_SKILLTREE_LEVEL && this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[i].m_Skill),lpObj->Magic[i].m_Level+1) != 0.0f )
			{
				pElementMsg.nextvalue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[i].m_Skill),lpObj->Magic[i].m_Level+1);
			}
			else
			{
				pElementMsg.nextvalue = -1.0f;
			}

			nCount++;
			memcpy(&sendbuf[lOfs],&pElementMsg,sizeof(pElementMsg));
			lOfs += sizeof(pElementMsg);
		}
	}

	pCountMsg.count = nCount;
	pCountMsg.h.sizeH = SET_NUMBERH(lOfs);
	pCountMsg.h.sizeL = SET_NUMBERL(lOfs);

	memcpy(&sendbuf[0],&pCountMsg,sizeof(pCountMsg));

	DataSend(aIndex,(LPBYTE)sendbuf,lOfs);
}

void CMasterLevelSkillTreeSystem::CGReqGetMasterLevelSkill(PMSG_REQ_MASTERLEVEL_SKILL * lpMsg,int aIndex)
{
	if( !gObjIsConnected(aIndex) )
	{
		CloseClient(aIndex);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	int iResult;

	PMSG_ANS_MASTERLEVEL_SKILL pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xF3,0x52,sizeof(pMsg));
	pMsg.btResult = 0;
	pMsg.wLevelPoint = lpObj->MasterPoint;
	pMsg.dwSkillId = -1;
	pMsg.iLevel = 0;

	if( lpObj->m_IfState.use == TRUE &&
		lpObj->m_IfState.type == 1 )
	{
		iResult = 6;
		LogAddTD("[MasterSkill] [%s][%s] Fail(Use Trade Interface) - Add Magic List",lpObj->AccountID,lpObj->Name);
		pMsg.btResult = iResult;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	int iSkillLevel = this->CheckSkillLevel(lpObj,lpMsg->AddSkill) + this->GetRequireMLPoint(lpObj->Class,lpMsg->AddSkill);

	iResult = this->CheckSkillCondition(lpObj,lpMsg->AddSkill,iSkillLevel);

	if( iResult == 1 )
	{
		if( this->gObjMagicAdd_MLS(lpObj,lpMsg->AddSkill,iSkillLevel) < 0 )
		{
			iResult = 5;
			LogAddTD("[MasterSkill] [%s][%s] Fail - Add Magic List, Skill:%d Level:%d",
				lpObj->AccountID,lpObj->Name,lpMsg->AddSkill,iSkillLevel);
		}
		else
		{
			int iReqMLPoint = this->GetRequireMLPoint(lpObj->Class,lpMsg->AddSkill);

			if( iReqMLPoint < 0 )
				return;

			lpObj->MasterPoint -= iReqMLPoint;
			lpObj->MasterPointSpent += iReqMLPoint;

			pMsg.btResult = iResult;
			pMsg.wLevelPoint = lpObj->MasterPoint;
			pMsg.btMasterSkillId = this->GetMasterSkillUIIndex(lpObj->Class,lpMsg->AddSkill);
			pMsg.dwSkillId = lpMsg->AddSkill;
			pMsg.iLevel = iSkillLevel;

			if( this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMsg->AddSkill),iSkillLevel) > 0.0f )
			{
				pMsg.value = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMsg->AddSkill),iSkillLevel);
			}
			else
			{
				pMsg.value = -1.0f;
			}

			if( iSkillLevel < MAX_SKILLTREE_LEVEL && this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMsg->AddSkill),iSkillLevel+1) > 0.0f )
			{
				pMsg.nextvalue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMsg->AddSkill),iSkillLevel+1);
			}
			else
			{
				pMsg.nextvalue = -1.0f;
			}

			this->CalcPassiveSkillData(lpObj,lpMsg->AddSkill,iSkillLevel);
			gObjCalCharacter(lpObj->m_Index);

			LogAddTD("[MasterSkill] [%s][%s] Success - Add Magic List, Skill:%d Level:%d MLPoint:%d",
				lpObj->AccountID,lpObj->Name,lpMsg->AddSkill,iSkillLevel,lpObj->MasterPoint);
		}
	}
	else
	{
		pMsg.btResult = iResult;
	}

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
}

int CMasterLevelSkillTreeSystem::RunningSkill_MLS(LPOBJ lpObj,int aTargetIndex,CMagicInf* lpMagic,int bCombo,BYTE x,BYTE y,BYTE dir,BYTE aTargetPos)
{
	if( lpObj == NULL || lpMagic == NULL )
		return false;

	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	int iBaseMLS = g_MasterSkillSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill);

	switch( iBaseMLS )
	{
	case 403:
		this->MLS_WizardMagicDefense(lpObj,aTargetIndex,lpMagic);
		break;
	case 413:
		this->MLS_Healing(lpObj,aTargetIndex,lpMagic);
		break;
	case 330:
	case 332:
	case 481:
		this->MLS_Wheel(lpObj,lpMagic,aTargetIndex,bCombo);
		break;
	case 360:
		this->MLS_SkillFulKnightSkillAddLife(lpObj,lpMagic);
		break;
	case 356:
		this->MLS_KnightSkillAddLife(lpObj,lpMagic);
		break;
	case AT_MSKILL_DK_SWELLLIFE3:
		this->MLS_SkillMasteryKnightSkillAddLife(lpObj,lpMagic);
		break;
	case 512:
	case 516:
		this->MLS_DarkHorseAttack(lpObj,aTargetIndex,lpMagic);
		break;
	case 508:
	case 514:
		this->MLS_FireBurst(lpObj,aTargetIndex,lpMagic);
		break;
	case AT_MSKILL_DW_ICESTORM1:
	case AT_MSKILL_DW_ICESTORM2:
		this->MLS_SuddenIce(lpObj->m_Index,lpMagic,x,y,aTargetIndex);
		break;
	case AT_MSKILL_DK_DEATHSTAB1:
	case AT_MSKILL_DK_DEATHSTAB2:
	case AT_MSKILL_DK_DEATHSTAB3:
		this->MLS_KnightBlow(lpObj->m_Index,aTargetIndex,lpMagic,bCombo);
		break;
	case 331:
	case 333:
		this->MLS_SkillBlowOfFury(lpObj->m_Index,lpMagic,x,y,aTargetIndex,bCombo);
		break;
	case 417:
		this->MLS_SkillDefense(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case 420:
		this->MLS_SkillAttack(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case 482:
		this->MLS_PowerSlash(lpObj->m_Index,lpMagic,x,y,aTargetPos,aTargetIndex);
		break;
	case 454:
		this->MLS_SkillSleep(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case 455:
		this->MLS_SkillChainLightning(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case 456:
		this->MLS_SkillLightningShock(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case AT_MSKILL_SUM_DRAINLIFE1:
	case AT_MSKILL_SUM_DRAINLIFE2:
		this->MLS_SkillDrainLife(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case 326:
		this->MLS_SkillSword4(lpObj,lpMagic,aTargetIndex,0);
		break;
	case 479:
		this->MLS_SkillSword4_Magumsa(lpObj,lpMagic,aTargetIndex,0);
		break;
	case 327:
		this->MLS_SkillSword5(lpObj,lpMagic,aTargetIndex,0);
		break;
	case 328:
		this->MLS_SkillSword1(lpObj,lpMagic,aTargetIndex,0);
		break;
	case 329:
		this->MLS_SkillSword2(lpObj,lpMagic,aTargetIndex,0);
		break;
	case AT_MSKILL_DK_DESTRUCTION1:
	case AT_MSKILL_DK_DESTRUCTION2:
	case AT_MSKILL_DK_DESTRUCTION3:
		this->MLS_SkillStrengthenDesruction(lpObj,lpMagic,x,y,aTargetIndex,0);
		break;
	case 380:
		this->MLS_SkillStrengthenMagicPower(lpObj,lpMagic,aTargetIndex);
		break;
	case 383:
		this->MLS_SkillSkillfulMagicPower(lpObj,lpMagic,aTargetIndex);
		break;
	case 379:
	case 480:
		this->MLS_SkillStrengthenThunder(lpObj,lpMagic,aTargetIndex);
		break;
	case 384:
		this->MLS_SkillStrengthenPoison(lpObj,lpMagic,aTargetIndex);
		break;
	case 387:
		this->MLS_SkillStrengthenExPoison(lpObj->m_Index,lpMagic,x,y,aTargetIndex);
		break;
	case 389:
	case 489:
	case AT_MSKILL_MG_ICE2:
		this->MLS_SkillStrengthenIce(lpObj,lpMagic,aTargetIndex);
		break;
	case AT_MSKILL_DW_MANASHIELD2:
		this->MLS_SkillWizardMagicDefense(lpObj,lpMagic,aTargetIndex);
		break;
	case 422:
		this->MLS_SkillStrengthenAttack(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case 423:
		this->MLS_SkillStrengthenDefense(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case 441:
		this->MLS_SkillInfinityArrow(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case 469:
		this->MLS_SkillStrengthenBerserker(lpObj->m_Index,lpMagic);
		break;
	case 470:
		this->MLS_SkillSkillfulBerserker(lpObj->m_Index,lpMagic);
		break;
	case 472:
		this->MLS_SkillMasteryBerserker(lpObj->m_Index,lpMagic);
		break;
	case 511:
		this->MLS_SkillStrendthenAddCriticalDamage(lpObj->m_Index,lpMagic->m_Level,lpMagic);
		break;
	case 515:
		this->MLS_SkillSkillFulAddCriticalDamage(lpObj->m_Index,lpMagic->m_Level,lpMagic);
		break;
	case 517:
		this->MLS_SkillMasteryAddCriticalDamage(lpObj->m_Index,lpMagic->m_Level,lpMagic);
		break;
	case AT_MSKILL_DL_CRITICALDAMAGE4:
		this->MLS_SkillFulMasteryAddCriticalDamage(lpObj->m_Index,lpMagic->m_Level,lpMagic);
		break;
	case 509:
		this->MLS_SkillStrengthenSpear(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case 424:
		gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,0,0,0,0,0);
		break;
	case AT_MSKILL_RF_KILLBLOW1:
	case AT_MSKILL_RF_UPERCUT1:
	case AT_MSKILL_RF_CHAINDRIVE1:
	case AT_MSKILL_RF_CHAINDRIVE2:
		this->MLS_SkillMonkBarrageJustOneTarget(lpObj->m_Index,lpMagic,aTargetIndex);
		break;
	case AT_MSKILL_RF_KILLBLOW2:
	case AT_MSKILL_RF_UPERCUT2:
		this->MLS_SkillMonkBarrageJustOneTargetMastery(lpObj->m_Index,lpMagic,aTargetIndex);
		break;
	case AT_MSKILL_RF_DARKSIDE1:
	case AT_MSKILL_RF_DARKSIDE2:
		{
			PMSG_MONK_DARKSIDE_RECV pMsg = {0};
			pMsg.NumberH = SET_NUMBERH(aTargetIndex);
			pMsg.NumberL = SET_NUMBERL(aTargetIndex);

			pMsg.MagicNumberH = SET_NUMBERH(lpMagic->m_Skill);
			pMsg.MagicNumberL = SET_NUMBERL(lpMagic->m_Skill);

			GCMonkDarkSideTargetSelect(&pMsg,lpObj->m_Index);

			if( lpObj->m_btDarkSideTargetCount )
			{
				for(int i = 0; i < 5; i++)
				{
					if( lpObj->m_wDarkSideTarget[i] != 10000 )
						MLS_SkillMonkBarrageJustOneTarget(lpObj->m_Index,lpMagic,lpObj->m_wDarkSideTarget[i]);
				}
			}
		}
		break;
	case AT_MSKILL_RF_DRAGONRORE1:
	case AT_MSKILL_RF_DRAGONRORE2:
		{
			if( !OBJMAX_RANGE(aTargetIndex) )
			{
				LogAddTD("[InvalidTargetIndex][CMasterLevelSkillTreeSystem::RunningSkill_MLS][AT_MSKILL_RF_DRAGONRORE1] Index :%d , AccountID : %s ",
					lpObj->m_Index,lpObj->AccountID);
				break;
			}

			this->MLS_SkillAreaMonsterAttack(lpObj->m_Index,lpMagic,gObj[aTargetIndex].X,gObj[aTargetIndex].Y,aTargetIndex,3,1,0,0);
		}
		break;
	case AT_MSKILL_RF_DEFRATE1:
	case AT_MSKILL_RF_FITNESS1:
		MLS_SkillMonkBuffApplyParty(lpObj->m_Index,lpMagic);
		break;
	case AT_MSKILL_RF_DEFRATE2:
		MLS_SkillMonkBuffApplyPartyMastery(lpObj->m_Index,lpMagic);
		break;
	case AT_MSKILL_DK_BLOODSTORM:
	case AT_MSKILL_DK_BLOODSTORM1:
		gObjUseSkill.SkillBloodStorm(lpObj->m_Index,aTargetIndex,lpMagic,bCombo);
		break;
	case AT_MSKILL_DW_METEORIT1:
	case AT_MSKILL_DW_METEORIT2:
		this->MLS_SkillMeteorit(lpObj,aTargetIndex,lpMagic);
		break;
	case AT_MSKILL_DW_NOVA1:
		this->MLS_SkillHellFire2(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case AT_MSKILL_DW_EARTHPRISON:
	case AT_MSKILL_DW_EARTHPRISON1:
		this->MLS_SkillEarthPrison(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case AT_MSKILL_DW_MANASHIELD3:
		this->MLS_SkillWizardMagicDefenseMastery(lpObj,lpMagic,aTargetIndex);
		break;
	case AT_MSKILL_FE_CURE:
		this->MLS_SkillCure(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case AT_MSKILL_FE_MULTISHOT1:
	case AT_MSKILL_FE_MULTISHOT2:
		this->MLS_SkillMultiShot(lpObj->m_Index,lpMagic,aTargetPos,aTargetIndex);
		break;
	case AT_MSKILL_FE_SUMMONSATYROS:
		gObjUseSkill.SkillMonsterCall(lpObj->m_Index,597,lpObj->X-1,lpObj->Y+1);
		break;
	case AT_MSKILL_FE_POISONARROW:
	case AT_MSKILL_FE_POISONARROW1:
		gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,0,0,0,0,0);
		break;
	case AT_MSKILL_FE_BLESS:
	case AT_MSKILL_FE_BLESS1:
		this->MLS_SkillBless(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case AT_MSKILL_MG_GIGANTICSTORM1:
		this->MLS_SkillGiganticStorm(lpObj->m_Index,lpMagic,x,y,aTargetIndex);
		break;
	case AT_MSKILL_MG_FLAMESTRIKE1:
	case AT_MSKILL_MG_FLAMESTRIKE2:
		this->MLS_SkillFlameStrike(lpObj->m_Index,lpMagic,aTargetPos,aTargetIndex);
		break;
	case AT_MSKILL_SUM_INNOVATION1:
		this->MLS_SkillInnovation(lpObj->m_Index,lpMagic,x,y,aTargetIndex);
		break;
	case AT_MSKILL_SUM_WEAKNESS1:
		this->MLS_SkillWeakness(lpObj->m_Index,lpMagic,x,y,aTargetIndex);
		break;
	case AT_MSKILL_SUM_BLIND:
	case AT_MSKILL_SUM_BLIND1:
		this->MLS_SkillBlind(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case AT_MSKILL_DL_ELECTRICSPARK1:
		this->MLS_SkillElectricSpark(lpObj->m_Index,lpMagic,x,y,dir,aTargetPos,aTargetIndex);
		break;
	case AT_MSKILL_DL_CHAOTICDESEAR1:
		this->MLS_SkillChaoticDesier(lpObj->m_Index,lpMagic,aTargetPos,aTargetIndex);
		break;
	case AT_MSKILL_DL_IRONDEFENSE:
	case AT_MSKILL_DL_IRONDEFENSE1:
		this->MLS_SkillIronDefense(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	case AT_MSKILL_FE_PARTYHEAL:
	case AT_MSKILL_FE_PARTYHEAL1:
		MLS_SkillPartyHeal(lpObj->m_Index,aTargetIndex,lpMagic);
		break;
	}

return true;
}

void CMasterLevelSkillTreeSystem::MLS_WizardMagicDefense(LPOBJ lpObj,int aTargetIndex,CMagicInf * lpMagic)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int skillSuccess = TRUE;

	if( lpTargetObj->Type != OBJ_USER &&
		lpTargetObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_WIZARD &&
		lpObj->Class != CLASS_MAGUMSA )
		return;

	if( lpObj->PartyNumber != lpTargetObj->PartyNumber )
		return;

	float iDefenseValue = 0;
	int iSkillValidTime = 0;

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	iDefenseValue = lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2 + (lpObj->Dexterity + lpObj->AddDexterity) / lpInfo->m_Arg3;

	if( iDefenseValue > lpInfo->m_Arg4 )
		iDefenseValue = lpInfo->m_Arg4;

	float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
	iDefenseValue += fSkillValue;

	int iManaRate = g_SkillAdditionInfo.GetManaShieldRate(lpMagic->m_Level);

	iSkillValidTime = lpInfo->m_Arg5 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg6;

	if( iSkillValidTime > lpInfo->m_Arg7)
		iSkillValidTime = lpInfo->m_Arg7;

	gObjAddBuffEffect(lpTargetObj,BUFF_MANASHIELD,ADD_OPTION_MSDECREASE,(int)iDefenseValue,ADD_OPTION_MSTIME,iManaRate,iSkillValidTime);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,skillSuccess);
}

void CMasterLevelSkillTreeSystem::MLS_Healing(LPOBJ lpObj,int aTargetIndex,CMagicInf * lpMagic)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int skillSuccess = TRUE;

	if( lpTargetObj->Type != OBJ_USER &&
		lpTargetObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_ELF )
		return;

	if( CC_MAP_RANGE(lpObj->MapNumber) )
	{
		if( lpObj->m_Index != lpTargetObj->m_Index )
			return;
	}
	
	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	float fAddLife = lpInfo->m_Arg1+(lpObj->Energy+lpObj->AddEnergy)/lpInfo->m_Arg2;
	float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	if( fSkillValue != 0.0f )
	{
		fAddLife += fAddLife*fSkillValue/100.0f;
	}

	lpTargetObj->Life += fAddLife;

	if( (lpTargetObj->MaxLife+lpTargetObj->AddLife) < lpTargetObj->Life )
		lpTargetObj->Life = lpTargetObj->MaxLife+lpTargetObj->AddLife;

	if(lpTargetObj->Type == OBJ_USER)
	{
		GCReFillSend(lpTargetObj->m_Index,(WORD)lpTargetObj->Life,0xFF,0,lpTargetObj->iShield);

		for(int n = 0; n < MAX_VIEWPORT; n++)
		{
			if(lpObj->VpPlayer2[n].type == OBJ_MONSTER && lpObj->VpPlayer2[n].state != 0)
			{
				LPOBJ lpMonster = &gObj[lpObj->VpPlayer2[n].number];
				if(lpMonster->m_iCurrentAI != 0)
				{
					lpMonster->m_Agro.IncAgro(lpObj->m_Index,(lpObj->Energy / 5)/40);
				}
			}
		}
	}
	else if(lpTargetObj->m_RecallMon >= 0)
	{
		GCRecallMonLife(lpTargetObj->m_RecallMon,(int)lpTargetObj->MaxLife,(int)lpTargetObj->Life);
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}

void CMasterLevelSkillTreeSystem::MLS_Wheel(LPOBJ lpObj,CMagicInf *lpMagic,int aTargetIndex,int isCombo)
{
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					int iSkillDis = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

					if(gObjCalDistance(lpObj,&gObj[tObjNum]) <= iSkillDis )
					{
						bAttack = 0;

						if(HitCount > 15)
						{
							bAttack = 0;
						}

						if(HitCount >= 10)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{

							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,0,1,0,isCombo,0,0);

							if(isCombo != 0)
							{
								GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,gObj[tObjNum].m_Index,1);
							}
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword4(LPOBJ lpObj,CMagicInf * lpMagic,int aTargetIndex,int isCombo)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( !gObjIsConnected(lpObj->m_Index) )
		return;

	if( !gObjIsConnected(lpTargetObj->m_Index) )
		return;

	if( lpObj->Type != OBJ_USER )
		return;

	if( lpObj->Class != CLASS_KNIGHT )
		return;

	gObjAttack(lpObj,lpTargetObj,lpMagic,0,1,0,isCombo,0,0);

	if(isCombo != 0)
	{
		GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,lpObj->m_Index,1);
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword4_Magumsa(LPOBJ lpObj,CMagicInf * lpMagic,int aTargetIndex,int isCombo)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( !gObjIsConnected(lpObj->m_Index) )
		return;

	if( !gObjIsConnected(lpTargetObj->m_Index) )
		return;

	if( lpObj->Type != OBJ_USER )
		return;

	if( lpObj->Class != CLASS_MAGUMSA )
		return;

	gObjAttack(lpObj,lpTargetObj,lpMagic,0,1,0,isCombo,0,0);

	if(isCombo != 0)
	{
		GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,lpObj->m_Index,1);
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword5(LPOBJ lpObj,CMagicInf * lpMagic,int aTargetIndex,int isCombo)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( !gObjIsConnected(lpObj->m_Index) )
		return;

	if( !gObjIsConnected(lpTargetObj->m_Index) )
		return;

	if( lpObj->Type != OBJ_USER )
		return;

	if( lpObj->Class != CLASS_KNIGHT )
		return;

	gObjAttack(lpObj,lpTargetObj,lpMagic,0,1,0,isCombo,0,0);

	if(isCombo != 0)
	{
		GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,lpObj->m_Index,1);
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword1(LPOBJ lpObj,CMagicInf * lpMagic,int aTargetIndex,int isCombo)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( !gObjIsConnected(lpObj->m_Index) )
		return;

	if( !gObjIsConnected(lpTargetObj->m_Index) )
		return;

	if( lpObj->Type != OBJ_USER )
		return;

	if( lpObj->Class != CLASS_KNIGHT )
		return;

	gObjAttack(lpObj,lpTargetObj,lpMagic,0,1,0,isCombo,0,0);

	if(isCombo != 0)
	{
		GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,lpObj->m_Index,1);
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword2(LPOBJ lpObj,CMagicInf * lpMagic,int aTargetIndex,int isCombo)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( !gObjIsConnected(lpObj->m_Index) )
		return;

	if( !gObjIsConnected(lpTargetObj->m_Index) )
		return;

	if( lpObj->Type != OBJ_USER )
		return;

	if( lpObj->Class != CLASS_KNIGHT )
		return;

	gObjAttack(lpObj,lpTargetObj,lpMagic,0,1,0,isCombo,0,0);

	if(isCombo != 0)
	{
		GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,lpObj->m_Index,1);
	}
}

int CMasterLevelSkillTreeSystem::MLS_SkillStrengthenDesruction(LPOBJ lpObj,CMagicInf *lpMagic,BYTE x,BYTE y,int aTargetIndex,int isCombo)
{
	int bRet = FALSE;
	int count = 0;
	int tObjNum;
	int DuelIndex = lpObj->m_iDuelUser;
	int nHitCount = 0;
	int bAttack = 0;
	int nMinCount = 4;
	int nMaxCount = 8;
	int aDistance = 3;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);

	int iSkillDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);


	if( this->MLS_CalDistance(lpObj->X,lpObj->Y,x,y) <= iSkillDistance )
	{
		while ( true )
		{
			if(lpObj->VpPlayer2[count].state != 0)
			{
				tObjNum = lpObj->VpPlayer2[count].number;	
				if(tObjNum >= 0)
				{
					int EnableAttack = 0;

					if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}
#if (GS_CASTLE == 1)
					else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
					{
						if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
							EnableAttack = 1;
					}
#endif
					else
					{
						int CallMonIndex = gObj[tObjNum].m_Index;
	
						if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
						{
							CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
						}
	
						if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
						{
							EnableAttack = 1;
						}
					}

					if( EnableAttack != 0 )
					{
						if( gObj[tObjNum].X >= (x -aDistance) &&
						gObj[tObjNum].X <= (x +aDistance) &&
						gObj[tObjNum].Y >= (y -aDistance) &&
						gObj[tObjNum].Y <= (y +aDistance) )
						{
							gObj[tObjNum].lpAttackObj = lpObj;
							nHitCount++;

							if( nHitCount > nMinCount )
							{
								if( rand()%2 )
									bAttack = TRUE;
							}
							else
							{
								bAttack = TRUE;
							}

							if( nHitCount > nMaxCount )
								bAttack = FALSE;

							if( bAttack == TRUE )
							{
								gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,500,lpMagic->m_Skill,0);

								if( isCombo != 0 )
								{
									GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,lpObj->m_Index,1);
								}
							}

							bRet = TRUE;
						}
					}
				}
			}

			count++;
			if(count > MAX_VIEWPORT -1)
			{
				break;
			}
		}
	}

	return bRet;
}

void CMasterLevelSkillTreeSystem::MLS_KnightSkillAddLife(LPOBJ lpObj,CMagicInf *lpMagic)
{
	int skillSuccess = TRUE;

	if( lpObj->Type != OBJ_USER &&
		lpObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_KNIGHT &&
		lpObj->Class != CLASS_MAGUMSA )
		return;

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[5];
	int viewplayer = 0;
	int viewpercent = 0;
	LPOBJ lpPartyObj;

	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];

				if( lpObj->MapNumber == lpPartyObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,&gObj[number]);

					if( dis < 10 )
					{
						ApplyPartyIndex[n] = number;
						viewplayer++;
					}
				}
			}
		}
	}

	if( viewplayer > 1 )
	{
		if( viewplayer == 2 )
		{
			viewpercent = 1;
		}
		else if( viewplayer == 3 )
		{
			viewpercent = 2;
		}
		else if( viewplayer == 4 )
		{
			viewpercent = 3;
		}
		else
		{
			viewpercent = 4;
		}
	}

	float iaddLifepower;
	float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}
	
	iaddLifepower = lpInfo->m_Arg1 + (lpObj->Vitality + lpObj->AddVitality) / lpInfo->m_Arg2 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg3 + viewpercent;

	if( iaddLifepower > lpInfo->m_Arg4 )
		iaddLifepower = lpInfo->m_Arg4;

	iaddLifepower += fSkillValue;

	int iLifeTime = lpInfo->m_Arg5 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg6;

	if( iLifeTime > lpInfo->m_Arg7 )
		iLifeTime = lpInfo->m_Arg7;

	int iaddlife;

	if( partynum == -1 )
	{
		iaddlife = lpObj->MaxLife*iaddLifepower/100;
		gObjAddBuffEffect(lpObj,BUFF_SWELL_LIFE,ADD_OPTION_LIFE,iaddlife,0,0,iLifeTime);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,skillSuccess);
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY;n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];
				iaddlife = lpPartyObj->MaxLife*iaddLifepower/100;
				gObjAddBuffEffect(lpPartyObj,BUFF_SWELL_LIFE,ADD_OPTION_LIFE,iaddlife,0,0,iLifeTime);
				GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,skillSuccess);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillFulKnightSkillAddLife(LPOBJ lpObj,CMagicInf *lpMagic)
{
	int skillSuccess = TRUE;

	if( lpObj->Type != OBJ_USER &&
		lpObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_KNIGHT &&
		lpObj->Class != CLASS_MAGUMSA )
		return;

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[5];
	int viewplayer = 0;
	int viewpercent = 0;
	LPOBJ lpPartyObj;

	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];

				if( lpObj->MapNumber == lpPartyObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,&gObj[number]);

					if( dis < 10 )
					{
						ApplyPartyIndex[n] = number;
						viewplayer++;
					}
				}
			}
		}
	}

	if( viewplayer > 1 )
	{
		if( viewplayer == 2 )
		{
			viewpercent = 1;
		}
		else if( viewplayer == 3 )
		{
			viewpercent = 2;
		}
		else if( viewplayer == 4 )
		{
			viewpercent = 3;
		}
		else
		{
			viewpercent = 4;
		}
	}

	float iaddLifepower;

	int iPreSkill = this->GetParentSkill1(lpObj->Class,lpMagic->m_Skill);

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	if( iPreSkill > 0 )
	{
		int iPreLevel = this->GetCurSkillLevel(lpObj,iPreSkill);

		if( iPreLevel > 0 )
		{
			float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill),iPreLevel);

			iaddLifepower = lpInfo->m_Arg1 + (lpObj->Vitality + lpObj->AddVitality) / lpInfo->m_Arg2 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg3 + viewpercent;

			if( lpInfo->m_Arg4 > lpInfo->m_Arg4 )
			{
				iaddLifepower = lpInfo->m_Arg4;
			}

			iaddLifepower += fSkillValue;
		}
	}
	else
	{
		iaddLifepower = lpInfo->m_Arg1 + (lpObj->Vitality + lpObj->AddVitality) / lpInfo->m_Arg2 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg3 + viewpercent;

		if( lpInfo->m_Arg4 > lpInfo->m_Arg4 )
		{
			iaddLifepower = lpInfo->m_Arg4;
		}
	}



	float iaddManapower;
	
	if( lpMagic->m_Skill == 360 )
	{
		float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);
		iaddManapower = fSkillValue+viewpercent;
	}

	int iLifeTime = lpInfo->m_Arg5 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg6;

	if( iLifeTime > lpInfo->m_Arg7 )
		iLifeTime = lpInfo->m_Arg7;

	int iaddlife;
	int iaddMana;

	if( partynum == -1 )
	{
		iaddlife = lpObj->MaxLife*iaddLifepower/100;
		iaddMana = lpObj->MaxMana*iaddManapower/100;

		gObjAddBuffEffect(lpObj,BUFF_SWELL_LIFE2,ADD_OPTION_LIFE,iaddlife,ADD_OPTION_MANA,iaddMana,iLifeTime);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,skillSuccess);
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY;n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];
				iaddlife = lpPartyObj->MaxLife*iaddLifepower/100;
				iaddMana = lpPartyObj->MaxMana*iaddManapower/100;

				gObjAddBuffEffect(lpPartyObj,BUFF_SWELL_LIFE2,ADD_OPTION_LIFE,iaddlife,ADD_OPTION_MANA,iaddMana,iLifeTime);
				GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,skillSuccess);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryKnightSkillAddLife(LPOBJ lpObj,CMagicInf *lpMagic)
{
	int skillSuccess = TRUE;

	if( lpObj->Type != OBJ_USER &&
		lpObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_KNIGHT &&
		lpObj->Class != CLASS_MAGUMSA )
		return;

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[5];
	int viewplayer = 0;
	int viewpercent = 0;
	LPOBJ lpPartyObj;

	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];

				if( lpObj->MapNumber == lpPartyObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,&gObj[number]);

					if( dis < 10 )
					{
						ApplyPartyIndex[n] = number;
						viewplayer++;
					}
				}
			}
		}
	}

	if( viewplayer > 1 )
	{
		if( viewplayer == 2 )
		{
			viewpercent = 1;
		}
		else if( viewplayer == 3 )
		{
			viewpercent = 2;
		}
		else if( viewplayer == 4 )
		{
			viewpercent = 3;
		}
		else
		{
			viewpercent = 4;
		}
	}
	
	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	float iaddLifepower = lpInfo->m_Arg1 + (lpObj->Vitality + lpObj->AddVitality) / lpInfo->m_Arg2 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg3 + viewpercent;

	if( iaddLifepower > lpInfo->m_Arg4 )
	{
		iaddLifepower = lpInfo->m_Arg4;
	}

	float fSkillValue2 = this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,2);

	if( fSkillValue2 > 0.0f )
	{
		iaddLifepower += fSkillValue2;
	}

	float iaddManapower;
	float fSkillValue1 = this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,1);

	if( fSkillValue1 > 0.0f )
	{
		iaddManapower = fSkillValue1+viewpercent;
	}

	float iaddBPpower;
	float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	if( fSkillValue > 0.0f )
	{
		iaddBPpower = fSkillValue+viewpercent;
	}

	int iLifeTime = lpInfo->m_Arg5 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg6;

	if( iLifeTime > lpInfo->m_Arg7 )
		iLifeTime = lpInfo->m_Arg7;

	int iaddlife;
	int iaddMana;
	int iaddBP;

	if( partynum == -1 )
	{
		iaddlife = lpObj->MaxLife*iaddLifepower/100;
		iaddMana = lpObj->MaxMana*iaddManapower/100;
		iaddBP = lpObj->MaxBP*iaddBPpower/100;

		lpObj->m_MPSkillOpt.MpsSwellLifeAddBP = iaddBP;

		gObjAddBuffEffect(lpObj,BUFF_SWELL_LIFE3,ADD_OPTION_LIFE,iaddlife,ADD_OPTION_MANA,iaddMana,iLifeTime);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,skillSuccess);
		gObjCalCharacter(lpObj->m_Index);
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY;n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];
				iaddlife = lpPartyObj->MaxLife*iaddLifepower/100;
				iaddMana = lpPartyObj->MaxMana*iaddManapower/100;
				iaddBP = lpPartyObj->MaxBP*iaddBPpower/100;

				lpPartyObj->m_MPSkillOpt.MpsSwellLifeAddBP = iaddBP;

				gObjAddBuffEffect(lpPartyObj,BUFF_SWELL_LIFE3,ADD_OPTION_LIFE,iaddlife,ADD_OPTION_MANA,iaddMana,iLifeTime);
				GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,skillSuccess);
				gObjCalCharacter(lpPartyObj->m_Index);
			}
		}
	}
}

int CMasterLevelSkillTreeSystem::MLS_SkillStrengthenMagicPower(LPOBJ lpObj,CMagicInf *lpMagic,int aTargetIndex)
{
	if( lpObj->Type != OBJ_USER )
		return false;

	if( lpObj->Class != CLASS_WIZARD )
		return false;

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_EXP_WIZARDRY2) == TRUE )
		return false;

	float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);


	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}

	int nEffectValue = (lpObj->Energy/lpInfo->m_Arg1)*lpInfo->m_Arg2;

	if( nEffectValue > lpInfo->m_Arg3 )
	{
		nEffectValue = lpInfo->m_Arg3;
	}

	float fMagicDamage = (lpObj->m_MagicDamageMax/100.0f)*fSkillValue;

	gObjAddBuffEffect(lpObj,BUFF_EXP_WIZARDRY2,ADD_OPTION_MAGICDAMAGEMIN,nEffectValue,ADD_OPTION_MAGICDAMAGEMAX,fMagicDamage,lpInfo->m_Arg4);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
	return true;
}

int CMasterLevelSkillTreeSystem::MLS_SkillSkillfulMagicPower(LPOBJ lpObj,CMagicInf *lpMagic,int aTargetIndex)
{
	if( lpObj->Type != OBJ_USER )
		return false;

	if( lpObj->Class != CLASS_WIZARD )
		return false;

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_EXP_WIZARDRY3) == TRUE )
		return false;

	float fSkillValue = this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,1);

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}

	int nEffectValue = (lpObj->Energy/lpInfo->m_Arg1)*lpInfo->m_Arg2;

	if( nEffectValue > lpInfo->m_Arg3 )
	{
		nEffectValue = lpInfo->m_Arg3;
	}

	float fMagicDamage = (lpObj->m_MagicDamageMax/100.0f)*fSkillValue;
	fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);
	lpObj->m_MPSkillOpt.MpsExpWizardryCrit = fSkillValue;

	gObjAddBuffEffect(lpObj,BUFF_EXP_WIZARDRY3,ADD_OPTION_MAGICDAMAGEMIN,nEffectValue,ADD_OPTION_MAGICDAMAGEMAX,fMagicDamage,lpInfo->m_Arg4);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
	return true;
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenFlame(LPOBJ lpObj,CMagicInf *lpMagic,int aTargetIndex)
{
	if( lpObj->Type != OBJ_USER )
		return;

	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
	gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,0,0,0,0,0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenThunder(LPOBJ lpObj,CMagicInf *lpMagic,int aTargetIndex)
{
	if( lpObj->Type != OBJ_USER )
		return;

	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
	gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,0,0,0,0,0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenPoison(LPOBJ lpObj,CMagicInf *lpMagic,int aTargetIndex)
{
	if( lpObj->Type != OBJ_USER )
		return;

	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpTargetObj->m_Index, TRUE);
	gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,0,0,0,0,0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillMeteorit(LPOBJ lpObj,int aTargetIndex,CMagicInf* lpMagic)
{
	if( lpObj->Type != OBJ_USER )
		return;

	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,0,0,0,0,0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenExPoison(int aIndex,CMagicInf *lpMagic,BYTE x,BYTE y,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER )
		return;

	int tObjNum;
	int count = 0;

	int FirstHit = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(this->MLS_CalDistance(x,y,gObj[tObjNum].X,gObj[tObjNum].Y) < 4)
					{
						bAttack = 0;

						if(HitCount > 10)
						{
							bAttack = 0;
						}
						
						if(HitCount >= 5)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{
							int delay = rand()%500;
							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,delay,lpMagic->m_Skill,0);
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return;
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenIce(LPOBJ lpObj,CMagicInf *lpMagic,int aTargetIndex)
{
	if( lpObj->Type != OBJ_USER )
		return;

	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
	gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,0,0,0,0,0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillWizardMagicDefense(LPOBJ lpObj,CMagicInf *lpMagic,int aTargetIndex)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int skillSuccess = TRUE;

	if( lpTargetObj->Type != OBJ_USER &&
		lpTargetObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_WIZARD &&
		lpObj->Class != CLASS_MAGUMSA )
		return;

	if( lpObj->PartyNumber != lpTargetObj->PartyNumber )
		return;

	float iDefenseValue = 0;
	int iSkillValidTime = 0;

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	int iPreSkill = this->GetParentSkill1(lpObj->Class,lpMagic->m_Skill);

	if( iPreSkill > 0 )
	{
		int iPreLevel = this->GetCurSkillLevel(lpObj,iPreSkill);

		if( iPreLevel > 0 )
		{
			iDefenseValue = lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2 + (lpObj->Dexterity + lpObj->AddDexterity) / lpInfo->m_Arg3;

			if( iDefenseValue > lpInfo->m_Arg4 )
				iDefenseValue = lpInfo->m_Arg4;

			float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill),iPreLevel);
			iDefenseValue += fSkillValue;
		}
	}
	else
	{
		iDefenseValue = lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2 + (lpObj->Dexterity + lpObj->AddDexterity) / lpInfo->m_Arg3;

			if( iDefenseValue > lpInfo->m_Arg4 )
				iDefenseValue = lpInfo->m_Arg4;
	}

	int iManaRate = g_SkillAdditionInfo.GetManaShieldRate(lpMagic->m_Level);

	iSkillValidTime = lpInfo->m_Arg5 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg6;

	if( iSkillValidTime > lpInfo->m_Arg7)
		iSkillValidTime = lpInfo->m_Arg7;

	iSkillValidTime += this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	gObjAddBuffEffect(lpTargetObj,BUFF_MANASHIELD,ADD_OPTION_MSDECREASE,(int)iDefenseValue,ADD_OPTION_MSTIME,iManaRate,iSkillValidTime);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,skillSuccess);
}

void CMasterLevelSkillTreeSystem::MLS_SkillWizardMagicDefenseMastery(LPOBJ lpObj,CMagicInf *lpMagic,int aTargetIndex)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int skillSuccess = TRUE;

	if( lpTargetObj->Type != OBJ_USER &&
		lpTargetObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_WIZARD &&
		lpObj->Class != CLASS_MAGUMSA )
		return;

	if( lpObj->PartyNumber != lpTargetObj->PartyNumber )
		return;
	
	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	float iDefenseValue = lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2 + (lpObj->Dexterity+lpObj->AddDexterity)/lpInfo->m_Arg3;

	if( iDefenseValue > lpInfo->m_Arg4 )
		iDefenseValue = lpInfo->m_Arg4;

	int iSkillValidTime = 0;

	float fSkillValue2 = this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,2);

	if( fSkillValue2 > 0.0f )
	{
		iDefenseValue += fSkillValue2;
	}

	int iManaRate = g_SkillAdditionInfo.GetManaShieldRate(lpMagic->m_Level);

	iSkillValidTime = lpInfo->m_Arg5 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg6;

	if( iSkillValidTime > lpInfo->m_Arg7)
		iSkillValidTime = lpInfo->m_Arg7;

	float fSkillValue1 = this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,1);

	if( fSkillValue1 > 0.0f )
	{
		iSkillValidTime += fSkillValue1;
	}

	float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	if( fSkillValue > 0.0f )
	{
		lpTargetObj->m_MPSkillOpt.MpsManaShieldAddMana = lpTargetObj->MaxMana*fSkillValue/100.0f;
	}

	gObjAddBuffEffect(lpTargetObj,BUFF_MANASHIELD2,ADD_OPTION_MSDECREASE,(int)iDefenseValue,ADD_OPTION_MSTIME,iManaRate,iSkillValidTime);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,skillSuccess);
	gObjCalCharacter(lpTargetObj->m_Index);
}

int CMasterLevelSkillTreeSystem::MLS_SkillInfinityArrow(int aIndex,int aTargetIndex,CMagicInf *lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER )
		return false;

	if( lpObj->Class != CLASS_ELF )
		return false;

	if( lpObj->ChangeUP == FALSE )
		return false;

	if( lpObj->m_Index != aTargetIndex )
		return false;

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW2) == TRUE )
		return false;

	lpObj->m_InfinityArrowDmgMul = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);
	gObjAddBuffEffect(lpObj,BUFF_INFINITY_ARROW2,0,0,0,0,g_SkillAdditionInfo.GetInfinityArrowSkillTime());
	GCMagicAttackNumberSend(lpObj,441,lpObj->m_Index,TRUE);

	LogAddTD("[%s][%s] Use Infinity Arrow Skill (Time:%d)(Character Level : %d)(ChangeUp: %d)",
		lpObj->AccountID,lpObj->Name,g_SkillAdditionInfo.GetInfinityArrowSkillTime(),lpObj->Level,lpObj->ChangeUP);

	return true;
}

int CMasterLevelSkillTreeSystem::MLS_SkillStrengthenBerserker(int aIndex,CMagicInf *lpMagic)
{
	if( !OBJMAX_RANGE(aIndex) )
		return false;

	LPOBJ lpObj = &gObj[aIndex];

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}

	int iDuration = lpInfo->m_Arg3 + lpObj->Energy/lpInfo->m_Arg4;

	if( iDuration > lpInfo->m_Arg5 )
	{
		iDuration = lpInfo->m_Arg5;
	}

	int EffectUPValue = lpObj->Energy/lpInfo->m_Arg1;
	if( EffectUPValue > lpInfo->m_Arg2 )
	{
		EffectUPValue = lpInfo->m_Arg2;
	}

	int EffectDownValue = EffectUPValue / 2;

	lpObj->m_MPSkillOpt.MpsBerserkInc1 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	gObjAddBuffEffect(lpObj,BUFF_BERSERKER2,ADD_OPTION_BERSERKMANA_PERC,EffectUPValue,ADD_OPTION_BERSERKLIFE_PERC,EffectDownValue,iDuration);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);

	return true;
}

int CMasterLevelSkillTreeSystem::MLS_SkillSkillfulBerserker(int aIndex,CMagicInf *lpMagic)
{
	if( !OBJMAX_RANGE(aIndex) )
		return false;

	LPOBJ lpObj = &gObj[aIndex];

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}

	int iDuration = lpInfo->m_Arg3 + lpObj->Energy/lpInfo->m_Arg4;
	if( iDuration > lpInfo->m_Arg5 )
	{
		iDuration = lpInfo->m_Arg5;
	}
	int EffectUPValue = lpObj->Energy/lpInfo->m_Arg1;
	if( EffectUPValue > lpInfo->m_Arg2 )
	{
		EffectUPValue = lpInfo->m_Arg2;
	}
	int EffectDownValue = EffectUPValue / 2;

	lpObj->m_MPSkillOpt.MpsBerserkInc2 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	gObjAddBuffEffect(lpObj,BUFF_BERSERKER3,ADD_OPTION_BERSERKMANA_PERC,EffectUPValue,ADD_OPTION_BERSERKLIFE_PERC,EffectDownValue,iDuration);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);

	return true;
}

int CMasterLevelSkillTreeSystem::MLS_SkillMasteryBerserker(int aIndex,CMagicInf *lpMagic)
{
	if( !OBJMAX_RANGE(aIndex) )
		return false;

	LPOBJ lpObj = &gObj[aIndex];

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}

	int iDuration = lpInfo->m_Arg3 + lpObj->Energy/lpInfo->m_Arg4;
	if( iDuration > lpInfo->m_Arg5 )
	{
		iDuration = lpInfo->m_Arg5;
	}
	int EffectUPValue = lpObj->Energy/lpInfo->m_Arg1;
	if( EffectUPValue > lpInfo->m_Arg2 )
	{
		EffectUPValue = lpInfo->m_Arg2;
	}
	int EffectDownValue = EffectUPValue / 2;

	lpObj->m_MPSkillOpt.MpsBerserkInc3 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	gObjAddBuffEffect(lpObj,BUFF_BERSERKER4,ADD_OPTION_BERSERKMANA_PERC,EffectUPValue,ADD_OPTION_BERSERKLIFE_PERC,EffectDownValue,iDuration);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);

	return true;
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrendthenAddCriticalDamage(int aIndex,int aTargetIndex,CMagicInf *lpMagic)
{
	int skillSuccess = TRUE;
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER &&
		lpObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_DARKLORD )
		return;

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	LPOBJ lpPartyObj;

	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];

				if( lpObj->MapNumber == lpPartyObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,lpPartyObj);

					if( dis < 10 )
					{
						ApplyPartyIndex[n] = lpPartyObj->m_Index;
					}
				}
			}
		}
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	int addcriticaldamagevalue = (lpObj->Energy+lpObj->AddEnergy)/lpInfo->m_Arg2 + (lpObj->Leadership+lpObj->AddLeadership)/lpInfo->m_Arg1;
	
	if( addcriticaldamagevalue > lpInfo->m_Arg3 )
	{
		addcriticaldamagevalue = lpInfo->m_Arg3;
	}
	
	addcriticaldamagevalue += this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);
	int SkillTime = lpInfo->m_Arg4 + (lpObj->Energy+lpObj->AddEnergy)/lpInfo->m_Arg5;
	
	if( SkillTime > lpInfo->m_Arg6 )
	{
		SkillTime = lpInfo->m_Arg6;
	}

	if( partynum == -1 )
	{
		gObjAddBuffEffect(lpObj,AT_INCREASE_CRITICAL_DMG,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,0,0,SkillTime);
		MsgOutput(aIndex,lMsg.Get(MSGGET(4,216)),SkillTime);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];

				gObjAddBuffEffect(lpPartyObj,AT_INCREASE_CRITICAL_DMG,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,0,0,SkillTime);
				MsgOutput(lpPartyObj->m_Index,lMsg.Get(MSGGET(4,216)),SkillTime);
				GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillSkillFulAddCriticalDamage(int aIndex,int aTargetIndex,CMagicInf *lpMagic)
{
	int skillSuccess = TRUE;
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER &&
		lpObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_DARKLORD )
		return;

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	LPOBJ lpPartyObj;

	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];

				if( lpObj->MapNumber == lpPartyObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,lpPartyObj);

					if( dis < 10 )
					{
						ApplyPartyIndex[n] = lpPartyObj->m_Index;
					}
				}
			}
		}
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	int addcriticaldamagevalue = (lpObj->Energy+lpObj->AddEnergy)/lpInfo->m_Arg2 + (lpObj->Leadership+lpObj->AddLeadership)/lpInfo->m_Arg1;

	if( addcriticaldamagevalue > lpInfo->m_Arg3 )
	{
		addcriticaldamagevalue = lpInfo->m_Arg3;
	}

	addcriticaldamagevalue += this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,1);
	int SkillTime = lpInfo->m_Arg4 + (lpObj->Energy+lpObj->AddEnergy)/lpInfo->m_Arg5 + this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);
	
	if( SkillTime > lpInfo->m_Arg6 )
	{
		SkillTime = lpInfo->m_Arg6;
	}

	if( partynum == -1 )
	{
		gObjAddBuffEffect(lpObj,AT_INCREASE_CRITICAL_DMG,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,0,0,SkillTime);
		MsgOutput(aIndex,lMsg.Get(MSGGET(4,216)),SkillTime);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];

				gObjAddBuffEffect(lpPartyObj,AT_INCREASE_CRITICAL_DMG,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,0,0,SkillTime);
				MsgOutput(lpPartyObj->m_Index,lMsg.Get(MSGGET(4,216)),SkillTime);
				GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryAddCriticalDamage(int aIndex,int aTargetIndex,CMagicInf *lpMagic)
{
		int skillSuccess = TRUE;
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER &&
		lpObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_DARKLORD )
		return;

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	LPOBJ lpPartyObj;

	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];

				if( lpObj->MapNumber == lpPartyObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,lpPartyObj);

					if( dis < 10 )
					{
						ApplyPartyIndex[n] = lpPartyObj->m_Index;
					}
				}
			}
		}
	}
	
	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	int addcriticaldamagevalue = (lpObj->Energy+lpObj->AddEnergy)/lpInfo->m_Arg2 + (lpObj->Leadership+lpObj->AddLeadership)/lpInfo->m_Arg1;
	addcriticaldamagevalue += this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,2);

	if( addcriticaldamagevalue > lpInfo->m_Arg3 )
	{
		addcriticaldamagevalue = lpInfo->m_Arg3;
	}

	float fSkillValue = this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,1);
	int SkillTime = lpInfo->m_Arg4 + (lpObj->Energy+lpObj->AddEnergy)/lpInfo->m_Arg5 + fSkillValue;

	if( SkillTime > lpInfo->m_Arg6 )
	{
		SkillTime = lpInfo->m_Arg6;
	}

	float fRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	if( partynum == -1 )
	{
		gObjAddBuffEffect(lpObj,BUFF_ADD_CRITDMG2,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,ADD_OPTION_INCCRITDMG,fRate,SkillTime);
		MsgOutput(aIndex,lMsg.Get(MSGGET(4,216)),SkillTime);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];

				gObjAddBuffEffect(lpPartyObj,BUFF_ADD_CRITDMG2,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,ADD_OPTION_INCCRITDMG,fRate,SkillTime);
				MsgOutput(lpPartyObj->m_Index,lMsg.Get(MSGGET(4,216)),SkillTime);
				GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillFulMasteryAddCriticalDamage(int aIndex,int aTargetIndex,CMagicInf *lpMagic)
{
	int skillSuccess = TRUE;
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER &&
		lpObj->m_RecallMon == -1 )
		return;

	if( lpObj->Class != CLASS_DARKLORD )
		return;

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	LPOBJ lpPartyObj;

	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];

				if( lpObj->MapNumber == lpPartyObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,lpPartyObj);

					if( dis < 10 )
					{
						ApplyPartyIndex[n] = lpPartyObj->m_Index;
					}
				}
			}
		}
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	int addcriticaldamagevalue = (lpObj->Energy+lpObj->AddEnergy)/lpInfo->m_Arg2 + (lpObj->Leadership+lpObj->AddLeadership)/lpInfo->m_Arg1;
	addcriticaldamagevalue += this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,3);

	if( addcriticaldamagevalue > lpInfo->m_Arg3 )
	{
		addcriticaldamagevalue = lpInfo->m_Arg3;
	}

	float fSkillValue = this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,2);
	int SkillTime = lpInfo->m_Arg4 + (lpObj->Energy+lpObj->AddEnergy)/lpInfo->m_Arg5 + fSkillValue;

	if( SkillTime > lpInfo->m_Arg6 )
	{
		SkillTime = lpInfo->m_Arg6;
	}

	float fRate = this->GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,1);
	float fExcRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	if( partynum == -1 )
	{
		gObjAddBuffEffect(lpObj,BUFF_ADD_CRITDMG3,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,ADD_OPTION_INCCRITDMG,fRate,SkillTime);
		MsgOutput(aIndex,lMsg.Get(MSGGET(4,216)),SkillTime);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
		lpObj->m_MPSkillOpt.MpsCritBuffExcDamage = fExcRate;
		gObjCalCharacter(lpObj->m_Index);
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];

				gObjAddBuffEffect(lpPartyObj,BUFF_ADD_CRITDMG3,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,ADD_OPTION_INCCRITDMG,fRate,SkillTime);
				MsgOutput(lpPartyObj->m_Index,lMsg.Get(MSGGET(4,216)),SkillTime);
				GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,TRUE);
				lpPartyObj->m_MPSkillOpt.MpsCritBuffExcDamage = fExcRate;
				gObjCalCharacter(lpPartyObj->m_Index);
			}
		}
	}
}

int CMasterLevelSkillTreeSystem::MLS_DarkHorseAttack(LPOBJ lpObj,int aTargetIndex,CMagicInf * lpMagic)
{
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,1);

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;
				if( (gObj[tObjNum].Class < 100 || gObj[tObjNum].Class > 110) && gObj[tObjNum].Class != 523 )
				{
	
					if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}
#if (GS_CASTLE == 1)
					else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
					{
						if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
							EnableAttack = 1;
					}
#endif
					else
					{
						int CallMonIndex = gObj[tObjNum].m_Index;
	
						if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
						{
							CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
						}
	
						if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
						{
							EnableAttack = 1;
						}
					}
	
					if(EnableAttack != 0)
					{
						if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 5)
						{
							bAttack = 0;
	
							if(HitCount > 15)
							{
								bAttack = 0;
							}
	
							if(HitCount >= 10)
							{
								if(rand()%2)
								{
									bAttack = 1;
								}
							}
							else
							{
								bAttack = 1;
							}
	
							if(bAttack != 0)
							{
								gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,500,lpMagic->m_Skill,0);
							}
						}
					}
				}
			}
		}
	
		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
return true;
}

int CMasterLevelSkillTreeSystem::MLS_FireBurst(LPOBJ lpObj,int aTargetIndex,CMagicInf * lpMagic)
{
	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int StartDis = 1;
	int tObjNum;
	int count = 0;
	int loopcount = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,1);
	gObjAttack(lpObj,lpTargetObj,lpMagic,0,1,0,0,0,0);

	int attackcheck;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;
	int delaytime;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0 && aTargetIndex != tObjNum)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == lpTargetObj->m_Index || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					attackcheck = 0;

					if(loopcount == 0)
					{
						attackcheck = 1;
					}
					else if(rand()%3==0)
					{
						attackcheck = 1;
					}

					if(attackcheck != 0)
					{
						if(gObjCalDistance(&gObj[aTargetIndex],&gObj[tObjNum]) < 3)
						{
							delaytime = (rand()*17)%300 + 500;

							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,delaytime,lpMagic->m_Skill,0);
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return true;
}

int CMasterLevelSkillTreeSystem::MLS_SkillStrengthenSpear(int aIndex,int aTargetIndex,CMagicInf *lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int StartDis = 1;
	int tObjNum;

	int iangle = this->MLS_GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	int count = 0;
	int loopcount = 0;
	int attackcheck;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	if(lpObj->SkillLongSpearChange == 0)
	{
		gObjAttack(lpObj,&gObj[aTargetIndex],lpMagic,0,1,0,0,0,0);
		return true;
	}

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					attackcheck = 0;

					if(loopcount == 0)
					{
						attackcheck = 1;
					}
					else if(rand()%3==0)
					{
						attackcheck = 1;
					}

					if(attackcheck != 0)
					{
						if(SkillSpearHitBox.HitCheck(iangle,lpObj->X,lpObj->Y,gObj[tObjNum].X,gObj[tObjNum].Y))
						{
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,0,1,0,0,0,0);
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return true;
}

int CMasterLevelSkillTreeSystem::MLS_SkillAreaMonsterAttack(int aIndex,CMagicInf *lpMagic,BYTE x,BYTE y,int aTargetIndex,int aDistance,int bPVP,int nSuccessRate,int nDelay)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bRet = FALSE;
	int count = 0;
	int tObjNum;
	int attackcheck;
	int DuelIndex = lpObj->m_iDuelUser;
	int nHitCount = 0;
	int bAttack = FALSE;

	int nMinCount = 4;
	int nMaxCount = 8;

	if( lpMagic->m_Skill == AT_MSKILL_MG_GIGANTICSTORM1 )
	{
		nMinCount = 8;
		nMaxCount = 12;
	}

	if( x == FALSE &&
		y == FALSE )
	{
		x = lpObj->X;
		y = lpObj->Y;
	}

	int iSkillDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	if( this->MLS_CalDistance(lpObj->X,lpObj->Y,x,y) <= iSkillDistance )
	{
		while( TRUE )
		{
			if(lpObj->VpPlayer2[count].state != 0)
			{
				tObjNum = lpObj->VpPlayer2[count].number;

				if( tObjNum >= 0 )
				{
					int EnableAttack = 0;

					if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if( (tObjNum == aTargetIndex && bPVP == 1) || DuelIndex ==tObjNum )
					{
						EnableAttack = 1;
					}
#if (GS_CASTLE == 1)
					else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
					{
						if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
							EnableAttack = 1;
					}	
#endif
					else
					{
						int CallMonIndex = gObj[tObjNum].m_Index;
	
						if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
						{
							CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
						}
	
						if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
						{
							EnableAttack = 1;
						}
					}

					if( EnableAttack != 0 )
					{
						attackcheck = 0;

						if( nSuccessRate == 0 )
						{
							attackcheck = 1;
						}
						else
						{
							attackcheck = rand()%nSuccessRate;
						}
						
						if( attackcheck != 0 )
						{
							if( gObj[tObjNum].X >= (x -aDistance) &&
								gObj[tObjNum].X <= (x +aDistance) &&
								gObj[tObjNum].Y >= (y -aDistance) &&
								gObj[tObjNum].Y <= (y +aDistance) )
							{
								gObj[tObjNum].lpAttackObj = lpObj;
								nHitCount++;
	
								if( nHitCount > nMinCount )
								{
									if(rand()%2)
									{
										bAttack = TRUE;
									}
								}
								else
								{
									bAttack = TRUE;
								}
	
								if( nHitCount > nMaxCount )
								{
									bAttack = FALSE;
								}

								if( bAttack == TRUE )
								{
									if( nDelay == 0 )
									{
										if( lpMagic->m_Skill == AT_MSKILL_RF_DRAGONRORE1 
											|| lpMagic->m_Skill == AT_MSKILL_RF_DRAGONRORE2 )
										{
											for(int i = 1; i <= 4; i++)
											{
												if( i == 1 )
												{
													gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,TRUE,0,0,i,0);
												}
												else
												{
													gObjAttack(lpObj,&gObj[tObjNum],lpMagic,FALSE,TRUE,0,0,i,0);
												}
											}
										}
										else
										{
											gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,FALSE,0,0,0,0);
										}
									}
									else
									{
										gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,nDelay,lpMagic->m_Skill,0);
									}
								}

								bRet = TRUE;
							}
						}
					}
				}
			}

			count++;

			if( count > (MAX_VIEWPORT-1) )
			{
				break;
			}
		}
	}
	return bRet;
}

int CMasterLevelSkillTreeSystem::MLS_SuddenIce(int aIndex,CMagicInf * lpMagic,BYTE x,BYTE y,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;
	int count = 0;
	int FirstHit = 0;
	int HitCount = 0;
	int bAttack;
	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if( this->MLS_CalDistance(x,y,gObj[tObjNum].X,gObj[tObjNum].Y) < 4)
					{
						bAttack = 0;
						
						if(HitCount >= 5)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(HitCount > 10)
						{
							bAttack = 0;
						}

						if(bAttack != 0)
						{
							int delay = rand()%500;
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,1,1,0,0,0,0);
//							gObjAddBuffEffect(&gObj[tObjNum],BUFF_STUN,0,0,0,0,3); //fix
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return true;
}

int CMasterLevelSkillTreeSystem::MLS_CalDistance(int x,int y,int x1,int y1)
{
	if( x == x1 &&
		y == y1 )
		return 0;

	float tx = x - x1;
	float ty = y - y1;

	return sqrt((tx*tx)+(ty*ty));
}

#ifndef M_PI
#define M_PI		3.14159265358979323846f	// matches value in gcc v2 math.h
#endif

int CMasterLevelSkillTreeSystem::MLS_GetAngle(int x,int y,int tx,int ty)
{
	double diffX = x - tx;
	double diffY = y - ty;

	double rad = atan2(diffY,diffX);

	int angle  = rad*180/M_PI + 90.0f;

	if( angle < 0 )
		angle += 360;

return angle;
}

void CMasterLevelSkillTreeSystem::MLS_SkillFrustrum(int aIndex,float fangle,float tx,float ty)
{
	LPOBJ lpObj = &gObj[aIndex];

	float p[MAX_ARRAY_FRUSTRUM][3];

	p[0][0] = -tx;
	p[0][1] = ty;
	p[0][2] = 0;
	p[1][0] = tx;
	p[1][1] = ty;
	p[1][2] = 0;
	p[2][0] = 1.0f;
	p[2][1] = 0;
	p[2][2] = 0;
	p[3][0] = -1.0f;
	p[3][1] = 0;
	p[3][2] = 0;

	float Angle[3];

	Angle[0] = 0;
	Angle[1] = 0;
	Angle[2] = fangle;

	float Matrix[3][MAX_ARRAY_FRUSTRUM];
	float vFrustrum[MAX_ARRAY_FRUSTRUM][3];

	AngleMatrix(Angle,Matrix);

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM; i++)
	{
		VectorRotate((const float *)&p[i],(const float *)&Matrix[0],(float *)&vFrustrum[i]);

		lpObj->fSkillFrustrumX[i] = (int)vFrustrum[i][0] + lpObj->X;
		lpObj->fSkillFrustrumY[i] = (int)vFrustrum[i][1] + lpObj->Y;
	}
}

int CMasterLevelSkillTreeSystem::MLS_KnightBlow(int aIndex,int aTargetIndex,CMagicInf *lpMagic,int isCombo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int StartDis = 1;
	int tObjNum;

	float fangle = this->MLS_GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	float fSkillDist = MagicDamageC.GetSkillDistance(lpMagic->m_Skill)+1.0f;

	this->MLS_SkillFrustrum(aIndex,fangle,1.5f,fSkillDist);

	int count = 0;
	int loopcount = 0;
	int attackcheck;
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					attackcheck = 0;

					if(loopcount == 0)
					{
						attackcheck = 1;
					}
					else if(rand()%3==0)
					{
						attackcheck = 1;
					}

					if(attackcheck != 0)
					{
						if(::SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex))
						{
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,0,1,0,isCombo,0,0);
							if(isCombo != 0)
							{
								GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,gObj[tObjNum].m_Index,1);
							}
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			loopcount++;
			count = 0;
			if(loopcount >= 3)
			{
				break;
			}
		}
	}
	return true;
}

void CMasterLevelSkillTreeSystem::MLS_SkillBlowOfFury(int aIndex,CMagicInf *lpMagic,BYTE x,BYTE y,int aTargetIndex,int isCombo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;
	int count = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int HitCount = 0;
	int bAttack = 0;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 4)
					{
						bAttack = 0;

						if(HitCount > 15)
						{
							bAttack = 0;
						}

						if(HitCount >= 10)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{
							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,500,lpMagic->m_Skill,isCombo);

							if(isCombo != 0)
							{
								GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,aIndex,1);
							}
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillDefense(int aIndex,int aTargetIndex,CMagicInf * lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpTargetObj->Type != OBJ_USER && lpTargetObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_ELF)
	{
		return;
	}

	if(CC_MAP_RANGE(lpObj->MapNumber) && lpObj->m_Index != lpTargetObj->m_Index)
	{
		return;
	}

	float skill_defense;
	float aMLSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}
	
	skill_defense = (float)(lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2);
	skill_defense += skill_defense*aMLSkillValue/100.0f;

	if( skill_defense > lpInfo->m_Arg3 )
	{
		skill_defense = lpInfo->m_Arg3;
	}

	gObjAddBuffEffect(lpTargetObj,BUFF_ADD_DEFENSE,ADD_OPTION_DEFENSE,(int)skill_defense,0,0,lpInfo->m_Arg4);

	for(int n = 0; n < MAX_VIEWPORT; n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJ_MONSTER && lpObj->VpPlayer2[n].state != 0)
		{
			LPOBJ lpMonster = &gObj[lpObj->VpPlayer2[n].number];
			if(lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro.IncAgro(lpObj->m_Index,int(skill_defense)/10);
			}
		}
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}

void CMasterLevelSkillTreeSystem::MLS_SkillAttack(int aIndex,int aTargetIndex,CMagicInf * lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpTargetObj->Type != OBJ_USER && lpTargetObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_ELF)
	{
		return;
	}

	if(CC_MAP_RANGE(lpObj->MapNumber) && lpObj->m_Index != lpTargetObj->m_Index)
	{
		return;
	}

	float skill_attack;
	float aMLSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);
	
	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	skill_attack = (float)lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2;
	skill_attack += skill_attack*aMLSkillValue/100.0f;

	if( skill_attack > lpInfo->m_Arg3 )
	{
		skill_attack = lpInfo->m_Arg3;
	}


	gObjAddBuffEffect(lpTargetObj,BUFF_ADD_ATTACK,ADD_OPTION_ATTACK_DAMAGE,(int)skill_attack,0,0,lpInfo->m_Arg4);


	for(int n = 0; n < MAX_VIEWPORT; n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJ_MONSTER && lpObj->VpPlayer2[n].state != 0)
		{
			LPOBJ lpMonster = &gObj[lpObj->VpPlayer2[n].number];
			if(lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro.IncAgro(lpObj->m_Index,int(skill_attack)/10);
			}
		}
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenAttack(int aIndex,int aTargetIndex,CMagicInf *lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpTargetObj->Type != OBJ_USER && lpTargetObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_ELF)
	{
		return;
	}

	if(CC_MAP_RANGE(lpObj->MapNumber) && lpObj->m_Index != lpTargetObj->m_Index)
	{
		return;
	}

	float skill_attack;
	float fValue;
	float aMLSkillValue = 0;
	
	int iPreSkill = this->GetParentSkill1(lpObj->Class,lpMagic->m_Skill);

	if( iPreSkill > 0 )
	{
		int iPreLevel = this->GetCurSkillLevel(lpObj,iPreSkill);

		if( iPreLevel > 0 )
		{
			aMLSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill),iPreLevel);
		}
	}

	fValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	skill_attack = (float)lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2;
	skill_attack += skill_attack*(aMLSkillValue+fValue)/100.0f;

	if( skill_attack > lpInfo->m_Arg3 )
	{
		skill_attack = lpInfo->m_Arg3;
	}

	int nTime = lpInfo->m_Arg4 + fValue;

	gObjAddBuffEffect(lpTargetObj,BUFF_ADD_ATTACK,ADD_OPTION_ATTACK_DAMAGE,skill_attack,0,0,nTime);

	for(int n = 0; n < MAX_VIEWPORT; n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJ_MONSTER && lpObj->VpPlayer2[n].state != 0)
		{
			LPOBJ lpMonster = &gObj[lpObj->VpPlayer2[n].number];
			if(lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro.IncAgro(lpObj->m_Index,int(skill_attack)/10);
			}
		}
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenDefense(int aIndex,int aTargetIndex,CMagicInf *lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpTargetObj->Type != OBJ_USER && lpTargetObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_ELF)
	{
		return;
	}

	if(CC_MAP_RANGE(lpObj->MapNumber) && lpObj->m_Index != lpTargetObj->m_Index)
	{
		return;
	}

	float skill_defense;
	float fValue;
	float aMLSkillValue;
	
	int iPreSkill = this->GetParentSkill1(lpObj->Class,lpMagic->m_Skill);

	if( iPreSkill > 0 )
	{
		int iPreLevel = this->GetCurSkillLevel(lpObj,iPreSkill);

		if( iPreLevel > 0 )
		{
			aMLSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill),iPreLevel);
		}
	}
	
	fValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	skill_defense = (float)( lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2);
	skill_defense += skill_defense*(fValue+aMLSkillValue)/100.0f;

	if( skill_defense > lpInfo->m_Arg3 )
	{
		skill_defense = lpInfo->m_Arg3;
	}

	int nTime = lpInfo->m_Arg4 + fValue;

	gObjAddBuffEffect(lpTargetObj,BUFF_ADD_DEFENSE,ADD_OPTION_DEFENSE,(int)skill_defense,0,0,nTime);

	for(int n = 0; n < MAX_VIEWPORT; n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJ_MONSTER && lpObj->VpPlayer2[n].state != 0)
		{
			LPOBJ lpMonster = &gObj[lpObj->VpPlayer2[n].number];
			if(lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro.IncAgro(lpObj->m_Index,int(skill_defense)/10);
			}
		}
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}

void CMasterLevelSkillTreeSystem::MLS_MaGumSkillDefenseDown(int aIndex,int aTargetIndex)
{
	int skillSuccess = 1;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpObj->Type == OBJ_USER && lpObj->Class != CLASS_MAGUMSA)
	{
		return;
	}

	if( retCalcSkillResistance(lpTargetObj,R_POISON) )
	{
		gObjAddBuffEffect(lpTargetObj,BUFF_DEFENSEDOWN,ADD_OPTION_DEFENSEDOWN,10,0,0,10);
		GCMagicAttackNumberSend(lpObj,AT_MSKILL_MG_FIRESLASH1,lpTargetObj->m_Index,skillSuccess);
	}
}

void CMasterLevelSkillTreeSystem::MLS_MaGumSkillDefenseDownMastery(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	int skillSuccess = 1;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpObj->Type == OBJ_USER && lpObj->Class != CLASS_MAGUMSA)
	{
		return;
	}

	if( retCalcSkillResistance(lpTargetObj,R_POISON) )
	{
		int nEffectValue = 10;
		float fSkillValue = GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

		if( fSkillValue > 0.0f )
		{
			nEffectValue += fSkillValue;
		}

		gObjAddBuffEffect(lpTargetObj,BUFF_DEFENSEDOWN,ADD_OPTION_DEFENSEDOWN,nEffectValue,0,0,10);
		GCMagicAttackNumberSend(lpObj,AT_MSKILL_MG_FIRESLASH2,lpTargetObj->m_Index,skillSuccess);
	}
}

void CMasterLevelSkillTreeSystem::MLS_PowerSlash(int aIndex,CMagicInf *lpMagic,BYTE x,BYTE y,BYTE Targetangle,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int StartDis = 1;

	int iSkillDis = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);
	float fAngle = Targetangle*360/255;
	this->MLS_SkillFrustrum(aIndex,fAngle,iSkillDis+1.0f,iSkillDis+1.0f);

	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex))
					{
						bAttack = 0;

						if(HitCount > 10)
						{
							bAttack = 0;
						}
						
						if(HitCount >= 5)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,1,1,0,0,0,0);
						}

						HitCount++;
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

int CMasterLevelSkillTreeSystem::MLS_FireScream(int aIndex,int aTargetIndex,CMagicInf * lpMagic)
{
	if(g_SkillAdditionInfo.GetFireScreamSkill() == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(gObjIsConnected(lpObj->m_Index) == false && gObjIsConnected(lpTargetObj->m_Index) == false)
	{
		return false;
	}

	if(lpObj->Type != OBJ_USER)
	{
		return false;
	}

	if(lpObj->Class != CLASS_DARKLORD)
	{
		return false;
	}

	gObjAttack(lpObj,lpTargetObj,lpMagic,0,0,0,0,0,0);
	return true;
}

int CMasterLevelSkillTreeSystem::MLS_WeaponSkillDel(LPOBJ lpObj,BYTE aSkill,BYTE Level)
{
	int iBaseMLS = 0;

	for( int iCount = 0; iCount < MAX_MAGIC; iCount++)
	{
		if( lpObj->Magic[iCount].IsMagic() == TRUE && 
			this->CheckMasterLevelSkill(lpObj->Magic[iCount].m_Skill) )
		{
			iBaseMLS = this->GetBaseMasterLevelSkill(lpObj->Magic[iCount].m_Skill);

			if( iBaseMLS > 0 )
			{
				WORD BrandSkill = MagicDamageC.GetNormalBrandOfSkill(iBaseMLS);

				if( aSkill ==  BrandSkill )
				{
					BYTE byIsItemSkill = MagicDamageC.IsItemAttachedSkill(BrandSkill);

					if( byIsItemSkill )
					{
						GCMagicListOneDelSend(lpObj->m_Index,iCount,lpObj->Magic[iCount].m_Skill,lpObj->Magic[iCount].m_Level,0,0);
						return true;
					}
				}
			}
		}
	}
	return 0;
}

int CMasterLevelSkillTreeSystem::MLS_WeaponSkillAdd(LPOBJ lpObj,BYTE aSkill,BYTE Level)
{
	int iBaseMLS = 0;

	for(int iCount = 0; iCount < MAX_MAGIC; iCount++)
	{
		if( lpObj->Magic[iCount].IsMagic() == TRUE && 
			this->CheckMasterLevelSkill(lpObj->Magic[iCount].m_Skill) )
		{
			iBaseMLS = this->GetBaseMasterLevelSkill(lpObj->Magic[iCount].m_Skill);

			if( iBaseMLS > 0 && 
				aSkill == MagicDamageC.GetBrandOfSkill(iBaseMLS) )
			{
				//GCMagicListOneSend(lpObj->m_Index,iCount,lpObj->Magic[iCount].m_Skill,lpObj->Magic[iCount].m_Level,0,0);
				return true;
			}
		}
	}
	return 0;
}

int CMasterLevelSkillTreeSystem::CheckUsableWeaponSkill(LPOBJ lpObj,int nCount)
{
	int iBaseMLS = 0;

	if( this->CheckMasterLevelSkill(lpObj->Magic[nCount].m_Skill) )
	{
		iBaseMLS = this->GetBaseMasterLevelSkill(lpObj->Magic[nCount].m_Skill);

		if( iBaseMLS > 0 && 
			MagicDamageC.GetSkillType(iBaseMLS) == 4 )
		{
			WORD BrandSkill = MagicDamageC.GetNormalBrandOfSkill(iBaseMLS);
			BYTE byIsItemSkill = MagicDamageC.IsItemAttachedSkill(lpObj->Magic[nCount].m_Skill);

			if( byIsItemSkill )
			{
				int bUseSkill = 0;

				if( lpObj->pInventory[1].IsItem() == TRUE &&
					lpObj->pInventory[1].m_Special[0] == BrandSkill )
					bUseSkill = TRUE;

				if( lpObj->pInventory[0].IsItem() == TRUE &&
					lpObj->pInventory[0].m_Special[0] == BrandSkill )
					bUseSkill = TRUE;

				if( bUseSkill == 0 )
					return false;
			}
		}
	}

return true;
}

int CMasterLevelSkillTreeSystem::MLS_SkillSleep(int aIndex,int aTargetIndex,CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bRet = FALSE;
	int tObjNum;

	int Angle = this->MLS_GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	int count = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE(lpObj->MapNumber) ||
						 IT_MAP_RANGE(lpObj->MapNumber) ||
						 lpObj->MapNumber == MAP_INDEX_VULCANUS )
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					tObjNum == aTargetIndex && 
					gCheckSkillDistance(aIndex,aTargetIndex,lpMagic->m_Skill) != FALSE )
				{
					gObjAttack(lpObj,&gObj[aTargetIndex],lpMagic,1,0,0,0,0,0);
					bRet = TRUE;
					break;
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bRet;
}

struct PMSG_SKILL_TARGET_COUNT
{
	PBMSG_HEAD2 h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD Index;
	BYTE Count;
};

struct PMSG_SKILL_TARGET
{
	WORD Target;
};

int CMasterLevelSkillTreeSystem::MLS_SkillChainLightning(int aIndex,int aTargetIndex,CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	int bSuccess = TRUE;
	int tObjNum;
	int count = 0;

	int Target[3];

	Target[0] = -1;
	Target[1] = -1;
	Target[2] = -1;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	if( gCheckSkillDistance(aIndex,aTargetIndex,lpMagic->m_Skill) == FALSE )
	{
		return FALSE;
	}

	Target[0] = aTargetIndex;

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 )
				{
					if( gObj[tObjNum].X >= (lpTargetObj->X -1) &&
						gObj[tObjNum].X <= (lpTargetObj->X +1) &&
						gObj[tObjNum].Y >= (lpTargetObj->Y -1) &&
						gObj[tObjNum].Y <= (lpTargetObj->Y +1) &&
						gObj[tObjNum].Type == OBJ_MONSTER )
					{
						Target[1] = tObjNum;

						if(Target[2] != -1)
							break;
					}
					else if( gObj[tObjNum].X >= (lpTargetObj->X -2) &&
						gObj[tObjNum].X <= (lpTargetObj->X +2) &&
						gObj[tObjNum].Y >= (lpTargetObj->Y -2) &&
						gObj[tObjNum].Y <= (lpTargetObj->Y +2) &&
						gObj[tObjNum].Type == OBJ_MONSTER )
					{
						Target[2] = tObjNum;
	
						if(Target[1] != -1)
							break;
					}
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

	if( Target[2] == -1 )
	{
		Target[2] = Target[1];
	}

	if( Target[1] == -1 )
	{
		Target[1] = Target[0];
		Target[2] = Target[1];
	}

	int lOfs = 0;
	BYTE cBUFF[256];
	PMSG_SKILL_TARGET_COUNT pMsg;

	pMsg.MagicNumberH = SET_NUMBERH(lpMagic->m_Skill);
	pMsg.MagicNumberL = SET_NUMBERL(lpMagic->m_Skill);
	pMsg.Index = aIndex;
	pMsg.Count = 3;

	lOfs += sizeof(pMsg);

	int i;
	for(i = 0; i < 3; i++)
	{
		WORD wIndex = Target[i];
		memcpy(&cBUFF[lOfs],&wIndex,2);
		lOfs+= 2;
	}

	PHeadSubSetB((LPBYTE)&pMsg,0xBF,10,lOfs);
	memcpy(&cBUFF[0],&pMsg,sizeof(pMsg));

	if( lpObj->Type == OBJ_USER )
	{
		DataSend(lpObj->m_Index,(LPBYTE)&cBUFF[0],pMsg.h.size);
	}

	MsgSendV2(lpObj,(LPBYTE)&cBUFF[0],pMsg.h.size);

	for(i = 0; i < 3; i++)
	{
		gObjAddAttackProcMsgSendDelay(lpObj,50,Target[i],200 + 200*i,lpMagic->m_Skill,i+1);
	}
return bSuccess;
}

int CMasterLevelSkillTreeSystem::MLS_SkillLightningShock(int aIndex,int aTargetIndex,CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bRet = 0;
	int bAttack = 0;
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int iDistance = 7;
	int nHitCount = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int EnableAttack;
	int bPVP = TRUE;
	int DuelIndex = lpObj->m_iDuelUser;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0 && gObj[tObjNum].Live )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE( lpObj->MapNumber ) || IT_MAP_RANGE( lpObj->MapNumber ) 
//					|| lpObj->MapNumber == MAP_INDEX_VULCANUS
					)
				{
					EnableAttack = 1;
				}
				else if( (tObjNum == aTargetIndex && bPVP == TRUE ) || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 7)
					{
						nHitCount++;

						if( nHitCount > 5)
						{
							if( rand()%2 )
							{
								bAttack = TRUE;
							}
						}
						else
						{
							bAttack = TRUE;
						}

						if( nHitCount > 12)
						{
							bAttack = FALSE;
						}

						if(bAttack == TRUE)
						{
							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,250,lpMagic->m_Skill,0);
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
return TRUE;
}

int CMasterLevelSkillTreeSystem::MLS_SkillDrainLife(int aIndex,int aTargetIndex,CMagicInf * lpMagic)
{
	if( !OBJMAX_RANGE(aTargetIndex) )
		return false;

	LPOBJ lpObj = &gObj[aIndex];
	int bRet = FALSE;
	int tObjNum;

	int Angle = this->MLS_GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	int count = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					tObjNum == aTargetIndex && 
					gCheckSkillDistance(aIndex,aTargetIndex,lpMagic->m_Skill) != FALSE )
				{
					gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,700,lpMagic->m_Skill,0);
					bRet = TRUE;
					break;
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bRet;
}

int CMasterLevelSkillTreeSystem::GetRequireMLPoint(int iClass,int iSkill)
{

	if( iClass < CLASS_WIZARD || iClass > CLASS_FIGHTER )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] GetRequireMLPoint - Invalid Class Code  : %d",iClass);
		return -1;
	}

	if( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] GetRequireMLPoint - Invalid Skill Code : %d",iSkill);
		return -1;
	}

	int nReturn = -1;

	for(int iTreeKind = 0; iTreeKind < MAX_SKILLTREE_TYPE; iTreeKind++)
	{
		for(int iRank = 0; iRank < MAX_SKILLTREE_RANK; iRank++)
		{
			for(int iPos = 0; iPos < MAX_SKILLTREE_POS; iPos++)
			{
				if( this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iSkillID == iSkill )
				{
					nReturn = this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_btPoint;
					break;
				}
			}
		}
	}
return nReturn;
}

int CMasterLevelSkillTreeSystem::GetMaxPointOfMasterSkill(int iClass,int iSkill)
{
	if( iClass < CLASS_WIZARD || iClass > CLASS_FIGHTER )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] GetRequireMLPoint - Invalid Class Code  : %d",iClass);
		return -1;
	}

	if( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] GetRequireMLPoint - Invalid Skill Code : %d",iSkill);
		return -1;
	}

	int nReturn = -1;

	for(int iTreeKind = 0; iTreeKind < MAX_SKILLTREE_TYPE; iTreeKind++)
	{
		for(int iRank = 0; iRank < MAX_SKILLTREE_RANK; iRank++)
		{
			for(int iPos = 0; iPos < MAX_SKILLTREE_POS; iPos++)
			{
				if( this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iSkillID == iSkill )
				{
					nReturn = this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_btMaxPoint;
					break;
				}
			}
		}
	}
return nReturn;
}

float CMasterLevelSkillTreeSystem::GetMasterSkillValue(int iType,int iPoint)
{
	if( iPoint < 1 || iPoint > MAX_SKILLTREE_LEVEL )
	{
//		LogAddTD("[MasterLevelSkillTreeSystem] GetMasterSkillValue - Invalid Point : %d",iPoint);
		return 0.0f;
	}

	if( iType < 0 || iType > 38 )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] GetMasterSkillValue - Invalid Type : %d",iType);
		return 0.0f;
	}

	return this->m_fValue[iType][iPoint];
}

float CMasterLevelSkillTreeSystem::GetMasterSkillManaIncRate(int iPoint)
{
	if( iPoint < 1 || iPoint > MAX_SKILLTREE_LEVEL )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] GetMasterSkillManaIncRate - Invalid Point : %d",iPoint);
		return -1.0f;
	}

	return this->m_fValue[21][iPoint];
}

float CMasterLevelSkillTreeSystem::GetBrandOfMasterSkillValue(LPOBJ lpObj,int iSkill,int iStep)
{
	int nStep = iStep;
	int nSkill = iSkill;
	int nPrevSkill = nSkill;

	while( nStep != 0 )
	{
		if( nPrevSkill == 0 )
			break;

		nPrevSkill = MagicDamageC.GetBrandOfSkill(nSkill);
		nSkill = nPrevSkill;
		nStep--;
	}

	WORD iBrandOfSkill = nSkill;
	int iLevel;

	if( this->CheckMasterLevelSkill(iBrandOfSkill) )
	{
		for(int iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE && 
				lpObj->Magic[iCount].m_Skill == iBrandOfSkill )
			{
				iLevel = lpObj->Magic[iCount].m_Level;
				return this->GetMasterSkillValue(MagicDamageC.SkillGet(iBrandOfSkill),iLevel);
			}
		}
	}

	return 0.0f;
}

int CMasterLevelSkillTreeSystem::CheckPreviousRankSkill(LPOBJ lpObj,int iSkill,int iLevel)
{
	if( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] GetRequireMLPoint - Invalid Skill Code : %d",iSkill);
		return -1;
	}

	int nTreeKind = -1;
	int nRank = MagicDamageC.GetSkillRank(iSkill)-1;

	if( nRank < 1 )
		return 1;

	int iClass = lpObj->Class;
	int nReturn = -1;

	for(int iTreeKind = 0; iTreeKind < MAX_SKILLTREE_TYPE; iTreeKind++)
	{
		for(int iRank = 0; iRank < MAX_SKILLTREE_RANK; iRank++)
		{
			for(int iPos = 0; iPos < MAX_SKILLTREE_POS; iPos++)
			{
				if( this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iSkillID == iSkill )
				{
					nTreeKind = iTreeKind;
					break;
				}
			}
		}
	}

	int nPoint = 0;
	nRank -= 1;

	for(int iPos2 = 0; iPos2 < MAX_SKILLTREE_POS; iPos2++ )
	{
		for(int i = 0; i < MAX_MAGIC; i++)
		{
			if( lpObj->Magic[i].IsMagic() == TRUE && this->CheckMasterLevelSkill(lpObj->Magic[i].m_Skill) == TRUE  )
			{
				if( lpObj->Magic[i].m_Skill == this->m_SkillTreeData[iClass].m_SkillInfo[nTreeKind][nRank][iPos2].m_iSkillID )
				{
					nPoint |= lpObj->Magic[i].m_Level;
				}
			}
		}
	}

	if( nPoint >= 10 )
		nReturn = TRUE;
	else
		nReturn = FALSE;

	return nReturn;
}

int CMasterLevelSkillTreeSystem::CheckSkillLevel(LPOBJ lpObj,int iSkill)
{
	for(int i = 0; i < MAX_MAGIC; i++)
	{
		if( lpObj->Magic[i].IsMagic() == TRUE &&
			lpObj->Magic[i].m_Skill == iSkill )
		{
			return lpObj->Magic[i].m_Level;
		}
	}


return 0;
}

int CMasterLevelSkillTreeSystem::GetMasterSkillUIIndex(int iClass,int iSkill)
{
	if( iClass < CLASS_WIZARD || iClass > CLASS_FIGHTER )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] GetRequireMLPoint - Invalid Class Code  : %d",iClass);
		return -1;
	}

	if( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAddTD("[MasterLevelSkillTreeSystem] GetRequireMLPoint - Invalid Skill Code : %d",iSkill);
		return -1;
	}

	int nReturn = -1;

	for(int iTreeKind = 0; iTreeKind < MAX_SKILLTREE_TYPE; iTreeKind++)
	{
		for(int iRank = 0; iRank < MAX_SKILLTREE_RANK; iRank++)
		{
			for(int iPos = 0; iPos < MAX_SKILLTREE_POS; iPos++)
			{
				if( this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iSkillID == iSkill )
				{
					nReturn = this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iIndex;
					return nReturn;
				}
			}
		}
	}

return nReturn;
}

int CMasterLevelSkillTreeSystem::GetParentSkill1(int iClass,int iSkill)
{
	int nReturn = 0;

	for(int iTreeKind = 0; iTreeKind < MAX_SKILLTREE_TYPE; iTreeKind++)
	{
		for(int iRank = 0; iRank < MAX_SKILLTREE_RANK; iRank++)
		{
			for(int iPos = 0; iPos < MAX_SKILLTREE_POS; iPos++)
			{
				if( this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iSkillID == iSkill )
				{
					nReturn = this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iParentSkill1;
					return nReturn;
				}
			}
		}
	}

	return nReturn;
}

int CMasterLevelSkillTreeSystem::GetParentSkill2(int iClass,int iSkill)
{
	int nReturn = 0;

	for(int iTreeKind = 0; iTreeKind < MAX_SKILLTREE_TYPE; iTreeKind++)
	{
		for(int iRank = 0; iRank < MAX_SKILLTREE_RANK; iRank++)
		{
			for(int iPos = 0; iPos < MAX_SKILLTREE_POS; iPos++)
			{
				if( this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iSkillID == iSkill )
				{
					nReturn = this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iParentSkill2;
					return nReturn;
				}
			}
		}
	}

	return nReturn;
}

void CMasterLevelSkillTreeSystem::SetItemMLPassiveSkill(LPOBJ lpObj,int iType)
{
	int iCount = 0;

	switch(iType)
	{
	case 0:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 348 )
			{
				lpObj->m_MPSkillOpt.MpsTwoHandSwordStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 352 )
			{
				lpObj->m_MPSkillOpt.MpsTwoHandSwordMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 1:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 349 )
			{
				lpObj->m_MPSkillOpt.MpsOneHandSwordStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 353 )
			{
				lpObj->m_MPSkillOpt.MpsOneHandSwordMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 2:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 350 )
			{
				lpObj->m_MPSkillOpt.MpsMaceStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 354 )
			{
				lpObj->m_MPSkillOpt.MpsMaceMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 3:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 351 )
			{
				lpObj->m_MPSkillOpt.MpsSpearStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 355 )
			{
				lpObj->m_MPSkillOpt.MpsSpearMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 4:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 397 )
			{
				lpObj->m_MPSkillOpt.MpsOneHandStaffStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 400 )
			{
				lpObj->m_MPSkillOpt.MpsOneHandStaffMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 5:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 398 )
			{
				lpObj->m_MPSkillOpt.MpsTwoHandStaffStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 401 )
			{
				lpObj->m_MPSkillOpt.MpsTwoHandStaffMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 6:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE )
			{
				if( lpObj->Magic[iCount].m_Skill == 399 )
				{
					lpObj->m_MPSkillOpt.MpsShieldStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
					break;
				}

				if( lpObj->Magic[iCount].m_Skill == 437 )
				{
					lpObj->m_MPSkillOpt.MpsShieldStrength2 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
					break;
				}

				if( lpObj->Magic[iCount].m_Skill == 528 )
				{
					lpObj->m_MPSkillOpt.MpsShieldStrength3 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
					break;
				}
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE )
			{
				if( lpObj->Magic[iCount].m_Skill == 402 )
				{
					lpObj->m_MPSkillOpt.MpsShieldMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
					break;
				}

				if( lpObj->Magic[iCount].m_Skill == 440 )
				{
					lpObj->m_MPSkillOpt.MpsShieldMastery2 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
					break;
				}

				if( lpObj->Magic[iCount].m_Skill == 532 )
				{
					lpObj->m_MPSkillOpt.MpsShieldMastery3 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
					break;
				}
			}
		}
		break;
	case 7:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 435 )
			{
				lpObj->m_MPSkillOpt.MpsBowStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 438 )
			{
				lpObj->m_MPSkillOpt.MpsBowMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 8:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 436 )
			{
				lpObj->m_MPSkillOpt.MpsCrossbowStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 439 )
			{
				lpObj->m_MPSkillOpt.MpsCrossbowMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 9:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 465 )
			{
				lpObj->m_MPSkillOpt.MpsStickStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 467 )
			{
				lpObj->m_MPSkillOpt.MpsStickMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 10:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 466 )
			{
				lpObj->m_MPSkillOpt.MpsTomeStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}

		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 468 )
			{
				lpObj->m_MPSkillOpt.MpsTomeMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 11:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 527 )
			{
				lpObj->m_MPSkillOpt.MpsScepterStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 529 )
			{
				lpObj->m_MPSkillOpt.MpsUseScepterPetStr = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 531 )
			{
				lpObj->m_MPSkillOpt.MpsScepterMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 533 )
			{
				lpObj->m_MPSkillOpt.MpsCommandAttackInc = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 12:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 568 )
			{
				lpObj->m_MPSkillOpt.MpsFistStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 571 )
			{
				lpObj->m_MPSkillOpt.MpsFistMastery = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 20:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 322 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 324 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 21:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 375 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 377 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 22:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 410 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 412 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 23:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 476 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 478 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 24:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 505 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 507 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	case 25:
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 446 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		for( iCount = 0; iCount < MAX_MAGIC; iCount++)
		{
			if( lpObj->Magic[iCount].IsMagic() == TRUE &&
				lpObj->Magic[iCount].m_Skill == 447 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
				break;
			}
		}
		break;
	}
}

void CMasterLevelSkillTreeSystem::SetPetItemMLPassiveSkill(LPOBJ lpObj,int iType)
{
	if( iType != ITEMGET(13,4) )
		return;

	for(int iCount = 0; iCount < MAX_MAGIC; iCount++)
	{
		if( lpObj->Magic[iCount].IsMagic() == TRUE &&
			lpObj->Magic[iCount].m_Skill == 510 )
		{
			lpObj->m_MPSkillOpt.MpsDarkHorseStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpObj->Magic[iCount].m_Skill),lpObj->Magic[iCount].m_Level);
			break;
		}
	}
}

int CMasterLevelSkillTreeSystem::GetCurSkillLevel(LPOBJ lpObj,int iSkill)
{
	for(int iCount = 0; iCount < MAX_MAGIC; iCount++)
	{
		if( lpObj->Magic[iCount].IsMagic() &&
			lpObj->Magic[iCount].m_Skill == iSkill )
			return lpObj->Magic[iCount].m_Level;
	}
	return -1;
}

float CMasterLevelSkillTreeSystem::RoundValue(double dbValue,int iPos)
{
	char szTemp[256];
	char szPos[16];

	sprintf(szPos,"%%.%d",iPos);
	strcat(szPos,"f");
	sprintf(szTemp,szPos,dbValue);
	return (float)atof(szTemp);
}

float CMasterLevelSkillTreeSystem::GetSkillAttackDamage(LPOBJ lpObj,int iSkill)
{
	if( lpObj == NULL )
		return 0.0f;

	if( !this->CheckMasterLevelSkill(iSkill) )
		return 0.0f;

	int nSkillGroup = MagicDamageC.GetSkillGroup(iSkill);
	WORD nBrandOfSkill = iSkill;
	WORD iBrandOfSkill;
	int iValuType;
	int nCnt = 0;

	while( g_MasterSkillSystem.CheckMasterLevelSkill(nBrandOfSkill) )
	{
		iBrandOfSkill = nBrandOfSkill;

		if( MagicDamageC.GetSkillGroup(iBrandOfSkill) != nSkillGroup )
		{
			iBrandOfSkill = 0;
			break;
		}

		iValuType = MagicDamageC.SkillGet(iBrandOfSkill);

		if( iValuType == 3 ||
			iValuType == 4 ||
			iValuType == 5 ||
			iValuType == 22 )
		{
			break;
		}
		
		nBrandOfSkill = MagicDamageC.GetBrandOfSkill(iBrandOfSkill);
		nCnt++;

		if( nCnt > 2 )
		{
			LogAddTD("[GetSkillAttackDamage] fail!!! - %d",iSkill);
			iBrandOfSkill = 0;
			break;
		}
	}

	if( iBrandOfSkill > 0 )
	{
		for(int n = 0; n < MAX_MAGIC; n++)
		{
			if( lpObj->Magic[n].IsMagic() == TRUE &&
				lpObj->Magic[n].m_Skill == iBrandOfSkill )
			{
				return this->GetMasterSkillValue(iValuType,lpObj->Magic[n].m_Level);	
			}
		}
	}
	return 0.0f;
}

void CMasterLevelSkillTreeSystem::MLS_SkillMonkBarrageJustOneTarget(int aIndex,CMagicInf * lpMagic,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int nTargetObjNum = 0;
	int nViewPortObjectCount = 0;
	int DuelIndex = lpObj->m_iDuelUser;

	if( !OBJMAX_RANGE(aTargetIndex) )
	{
		LogAddTD("[InvalidTargetIndex][MLS_SkillMonkBarrageJustOneTarget] Index :%d , AccountID : %s ",
			aIndex,gObj[aIndex].AccountID);
		return;
	}

	int EnableAttack;
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	while( TRUE )
	{
		if( lpObj->VpPlayer2[nViewPortObjectCount].state != 0 )
		{
			nTargetObjNum = lpObj->VpPlayer2[nViewPortObjectCount].number;

			if( nTargetObjNum >= 0 && 
				nTargetObjNum == aTargetIndex )
			{
				EnableAttack = FALSE;

				if(lpObj->VpPlayer2[nViewPortObjectCount].type == OBJ_USER )
				{
					EnableAttack = TRUE;
				}
				else if( lpTargetObj->Type == OBJ_MONSTER && gObj[nTargetObjNum].m_RecallMon < 0 )
				{
					EnableAttack = TRUE;
				}
				else if( nTargetObjNum == DuelIndex )
				{
					EnableAttack = TRUE;
				}
				else
				{
					int CallMonIndex = gObj[nTargetObjNum].m_Index;

					if(gObj[nTargetObjNum].Type == OBJ_MONSTER && gObj[nTargetObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[nTargetObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = TRUE;
					}
				}

				if( EnableAttack != FALSE && 
					MLS_CalDistance(gObj[aTargetIndex].X,gObj[aTargetIndex].Y,gObj[nTargetObjNum].X,gObj[nTargetObjNum].Y) < 4 )
				{
					int nMaxBarrageCount = 0;

					if( lpMagic->m_Skill == AT_MSKILL_RF_KILLBLOW1 )
					{
						nMaxBarrageCount = 5;
					}
					else if( lpMagic->m_Skill == AT_MSKILL_RF_UPERCUT1 )
					{
						nMaxBarrageCount = 3;
					}
					else if( lpMagic->m_Skill == AT_MSKILL_RF_CHAINDRIVE1 || lpMagic->m_Skill == AT_MSKILL_RF_CHAINDRIVE2 )
					{
						nMaxBarrageCount = 5;
					}
					else if( lpMagic->m_Skill == AT_MSKILL_RF_DARKSIDE1 
						|| lpMagic->m_Skill == AT_MSKILL_RF_DARKSIDE2 )
					{
						nMaxBarrageCount = 3;
					}
					else
					{
						nMaxBarrageCount = 0;
					}

					if( lpMagic->m_Skill == AT_MSKILL_RF_KILLBLOW1 && 
						rand()%100 < 10 )
					{
						gObjAddBuffEffect(lpTargetObj,BUFF_WEAKNESS,ADD_OPTION_WEAKNESS,5,0,0,10);
					}
					else if( lpMagic->m_Skill == AT_MSKILL_RF_UPERCUT1 &&
						rand()%100 < 10 )
					{
						gObjAddBuffEffect(lpTargetObj,BUFF_DEFENSEDOWN,ADD_OPTION_DEFENSEDOWN,10,0,0,10);
					}
					else if( (lpMagic->m_Skill == AT_MSKILL_RF_CHAINDRIVE1 || lpMagic->m_Skill == AT_MSKILL_RF_CHAINDRIVE2 ) &&
						rand()%100 < 40 )
					{
						lpTargetObj->lpAttackObj = lpObj;
						lpTargetObj->DelayActionTime = 1000;
						lpTargetObj->DelayLevel = 1;

						gObjAddBuffEffect(lpTargetObj,BUFF_COLD,ADD_OPTION_REDUCEMOVEMENT,0,0,0,10);
					}

					for(int i = 1; i < nMaxBarrageCount; i++)
					{
						if( (i%(nMaxBarrageCount-1)) == TRUE  )
						{
							gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,TRUE,0,0,i,0);
						}
						else
						{
							gObjAttack(lpObj,lpTargetObj,lpMagic,FALSE,TRUE,0,0,i,0);
						}
					}
				}
				break;
			}
		}

		nViewPortObjectCount++;

		if(nViewPortObjectCount > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillMonkBarrageJustOneTargetMastery(int aIndex,CMagicInf * lpMagic,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int nTargetObjNum = 0;
	int nViewPortObjectCount = 0;
	int DuelIndex = lpObj->m_iDuelUser;

	if( !OBJMAX_RANGE(aTargetIndex) )
	{
		LogAddTD("[InvalidTargetIndex][MLS_SkillMonkBarrageJustOneTarget] Index :%d , AccountID : %s ",
			aIndex,gObj[aIndex].AccountID);
		return;
	}

	int EnableAttack;
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	float fSkillValue = GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	while( TRUE )
	{
		if( lpObj->VpPlayer2[nViewPortObjectCount].state != 0 )
		{
			nTargetObjNum = lpObj->VpPlayer2[nViewPortObjectCount].number;

			if( nTargetObjNum >= 0 && 
				nTargetObjNum == aTargetIndex )
			{
				EnableAttack = FALSE;

				if(lpObj->VpPlayer2[nViewPortObjectCount].type == OBJ_USER )
				{
					EnableAttack = TRUE;
				}
				else if( lpTargetObj->Type == OBJ_MONSTER && gObj[nTargetObjNum].m_RecallMon < 0 )
				{
					EnableAttack = TRUE;
				}
				else if( nTargetObjNum == DuelIndex )
				{
					EnableAttack = TRUE;
				}
				else
				{
					int CallMonIndex = gObj[nTargetObjNum].m_Index;

					if(gObj[nTargetObjNum].Type == OBJ_MONSTER && gObj[nTargetObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[nTargetObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = TRUE;
					}
				}

				if( EnableAttack != FALSE && 
					MLS_CalDistance(gObj[aTargetIndex].X,gObj[aTargetIndex].Y,gObj[nTargetObjNum].X,gObj[nTargetObjNum].Y) < 4 )
				{
					int nMaxBarrageCount = 0;

					if( lpMagic->m_Skill == AT_MSKILL_RF_KILLBLOW2 )
					{
						nMaxBarrageCount = 5;
					}
					else if( lpMagic->m_Skill == AT_MSKILL_RF_UPERCUT2 )
					{
						nMaxBarrageCount = 3;
					}
					else
					{
						nMaxBarrageCount = 0;
					}

					if( lpMagic->m_Skill == AT_MSKILL_RF_KILLBLOW2 && 
						rand()%100 < 10 )
					{
						gObjAddBuffEffect(lpTargetObj,BUFF_WEAKNESS,ADD_OPTION_WEAKNESS,5+fSkillValue,0,0,10);
					}
					else if( lpMagic->m_Skill == AT_MSKILL_RF_UPERCUT2 &&
						rand()%100 < 10 )
					{
						gObjAddBuffEffect(lpTargetObj,BUFF_DEFENSEDOWN,ADD_OPTION_DEFENSEDOWN,10+fSkillValue,0,0,10);
					}

					for(int i = 1; i < nMaxBarrageCount; i++)
					{
						if( (i%(nMaxBarrageCount-1)) == TRUE  )
						{
							gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,TRUE,0,0,i,0);
						}
						else
						{
							gObjAttack(lpObj,lpTargetObj,lpMagic,FALSE,TRUE,0,0,i,0);
						}
					}
				}
				break;
			}
		}

		nViewPortObjectCount++;

		if(nViewPortObjectCount > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

int CMasterLevelSkillTreeSystem::MLS_SkillMonkBuffApplyParty(int aIndex,CMagicInf * lpMagic)
{
	int skillSuccess = TRUE;
	LPOBJ lpObj = &gObj[aIndex];
	int number;

	if( lpObj->Type != OBJ_USER && lpObj->m_RecallMon == -1 )
		return FALSE;

	if( lpObj->Class != CLASS_FIGHTER )
		return FALSE;

	int partynum = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	int nEffectValue;
	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));
	LPOBJ lpPartyObj;

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];


				if( lpPartyObj->MapNumber == lpObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,&gObj[number]);
					if( dis < 4 )
					{
						ApplyPartyIndex[n] = lpPartyObj->m_Index;
					}
				}
			}
		}
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}
	
	int Duration;

	if( lpMagic->m_Skill == AT_MSKILL_RF_FITNESS1 )
	{
		nEffectValue = lpInfo->m_Arg1 + (lpObj->Energy+lpObj->AddEnergy-lpInfo->m_Arg2)/10.0;

		if( nEffectValue > lpInfo->m_Arg3 )
			nEffectValue = lpInfo->m_Arg3;

		Duration = lpInfo->m_Arg4+lpObj->Energy/lpInfo->m_Arg5;

		if( Duration > lpInfo->m_Arg6 )
		{
			Duration = lpInfo->m_Arg6;
		}
	}
	else if( lpMagic->m_Skill == AT_MSKILL_RF_DEFRATE1 )
	{
		nEffectValue = lpInfo->m_Arg1 + (lpObj->Energy+lpObj->AddEnergy-lpInfo->m_Arg2)/10.0;

		if( nEffectValue > lpInfo->m_Arg3 )
			nEffectValue = lpInfo->m_Arg3;

		Duration = lpInfo->m_Arg4+lpObj->Energy/lpInfo->m_Arg5;

		if( Duration > lpInfo->m_Arg6 )
		{
			Duration = lpInfo->m_Arg6;
		}
	}
	else
	{
		return FALSE;
	}

	float fSkillValue = GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

	if( partynum == -1 )
	{
		if( lpMagic->m_Skill == AT_MSKILL_RF_FITNESS1 )
		{
			gObjAddBuffEffect(lpObj,BUFF_INCREASE_VIT2,ADD_OPTION_INCRVITALITY,nEffectValue+fSkillValue,0,0,Duration);
			GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
			gObjCalCharacter(lpObj->m_Index);
		}
		else if( lpMagic->m_Skill == AT_MSKILL_RF_DEFRATE1 )
		{
			gObjAddBuffEffect(lpObj,BUFF_INCREASE_DEFRATE2,ADD_OPTION_DEFRATE,nEffectValue+lpObj->m_SuccessfulBlocking*fSkillValue/100.0f,0,0,Duration);
			GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
		}
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];

				if( lpMagic->m_Skill == AT_MSKILL_RF_FITNESS1 )
				{
					gObjAddBuffEffect(lpPartyObj,BUFF_INCREASE_VIT2,ADD_OPTION_INCRVITALITY,nEffectValue+fSkillValue,0,0,Duration);
					GCMagicAttackNumberSend(lpPartyObj,lpMagic->m_Skill,lpPartyObj->m_Index,TRUE);
					gObjCalCharacter(lpPartyObj->m_Index);
				}
				else if( lpMagic->m_Skill == AT_MSKILL_RF_DEFRATE1 )
				{
					gObjAddBuffEffect(lpPartyObj,BUFF_INCREASE_DEFRATE2,ADD_OPTION_DEFRATE,nEffectValue+lpPartyObj->m_SuccessfulBlocking*fSkillValue/100.0f,0,0,Duration);
					GCMagicAttackNumberSend(lpPartyObj,lpMagic->m_Skill,lpPartyObj->m_Index,TRUE);
				}
			}
		}
	}
	return TRUE;
}

int CMasterLevelSkillTreeSystem::MLS_SkillMonkBuffApplyPartyMastery(int aIndex,CMagicInf * lpMagic)
{
	int skillSuccess = TRUE;
	LPOBJ lpObj = &gObj[aIndex];
	int number;

	if( lpObj->Type != OBJ_USER && lpObj->m_RecallMon == -1 )
		return FALSE;

	if( lpObj->Class != CLASS_FIGHTER )
		return FALSE;

	int partynum = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	int nEffectValue;
	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));
	LPOBJ lpPartyObj;

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];


				if( lpPartyObj->MapNumber == lpObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,&gObj[number]);
					if( dis < 4 )
					{
						ApplyPartyIndex[n] = lpPartyObj->m_Index;
					}
				}
			}
		}
	}

	if( lpMagic->m_Skill == AT_MSKILL_RF_DEFRATE2 )
	{
		nEffectValue = 10.0f + (lpObj->Energy+lpObj->AddEnergy-80)/10.0f;

		if( nEffectValue > 100 )
			nEffectValue = 100;
	}
	else
	{
		return FALSE;
	}

	float fSkillValue = GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,1);
	float fSkillValue2 = GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);
	

	if( partynum == -1 )
	{
		if( lpMagic->m_Skill == AT_MSKILL_RF_DEFRATE2 )
		{
			gObjAddBuffEffect(lpObj,BUFF_INCREASE_DEFRATE3,ADD_OPTION_DEFRATE,nEffectValue+lpObj->m_SuccessfulBlocking*fSkillValue/100.0f,ADD_OPTION_DEFENSE,fSkillValue2,60+lpObj->Energy/5);
			GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);
		}
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];

				if( lpMagic->m_Skill == AT_MSKILL_RF_DEFRATE2 )
				{
					gObjAddBuffEffect(lpPartyObj,BUFF_INCREASE_DEFRATE3,ADD_OPTION_DEFRATE,nEffectValue+lpPartyObj->m_SuccessfulBlocking*fSkillValue/100.0f,ADD_OPTION_DEFENSE,fSkillValue2,60+lpObj->Energy/5);
					GCMagicAttackNumberSend(lpPartyObj,lpMagic->m_Skill,lpPartyObj->m_Index,TRUE);
				}
			}
		}
	}
	return TRUE;
}

BOOL CMasterLevelSkillTreeSystem::MLS_SkillHellFire2(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;

	if(lpObj->SkillHellFire2State == 0)
	{
		return MLS_SkillHellFire2Start(aIndex,lpMagic);
	}

	lpObj->SkillHellFire2State = 0;
	int count = 0;
	lpObj->SkillHellFire2Time = 0;

	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(aTargetIndex == tObjNum || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) <= 6)
					{
						gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,600,lpMagic->m_Skill,0);
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,1);
	return true;
}




BOOL CMasterLevelSkillTreeSystem::MLS_SkillHellFire2Start(int aIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->SkillHellFire2State != 0)
	{
		LogAddTD("[%s][%s] SkillHellFire2 error on start",lpObj->AccountID,lpObj->Name);
		return false;
	}

	lpObj->SkillHellFire2Time = GetTickCount();
	lpObj->SkillHellFire2State = 1;
	lpObj->SkillHellFire2Count = 0;

	GCMagicAttackNumberSend(lpObj,395,aIndex,1);
	return true;
}

void CMasterLevelSkillTreeSystem::MLS_SkillEarthPrison(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tmpvar = 0;
	int bAttack = 0;
	int tObjNum;
	int count = 0;
	int HitCount = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int EnableAttack;
	int bForceAttack = TRUE;
	int DuelIndex = lpObj->m_iDuelUser;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE( lpObj->MapNumber ) || IT_MAP_RANGE( lpObj->MapNumber ) )
				{
					EnableAttack = 1;
				}
				else if( (tObjNum == aTargetIndex && bForceAttack == TRUE ) || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) <= 3)
					{
						HitCount++;

						if(HitCount > 5)
						{
							if( rand()%2 )
							{
								bAttack = TRUE;
							}
						}
						else
						{
							bAttack = TRUE;
						}

						if(HitCount > 12)
						{
							bAttack = FALSE;
						}

						if(bAttack == TRUE)
						{
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,0,0,0,0,0);
							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,800,lpMagic->m_Skill,0);
//							gObjAddBuffEffect(&gObj[tObjNum],BUFF_STUN,0,0,0,0,5); //fix
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

int CMasterLevelSkillTreeSystem::MLS_SkillCure(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	int skillSuccess = true;

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( lpObj->Type != OBJ_USER )
	{
		return true;
	}
	
	gObjClearBuffEffect(lpObj,CLEAR_TYPE_MONSTERREGEN);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
	return true;
}

void CMasterLevelSkillTreeSystem::MLS_SkillMultiShot(int aIndex,CMagicInf* lpMagic,BYTE targetpos,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	this->MLS_SkillFrustrum3(aIndex,targetpos,6.0f,7.0f,1.0f,0);

	float p[5][3];
	float Matrix[3][MAX_ARRAY_FRUSTRUM];
	float vFrustrum[5][3];

	p[4][0] = 0;
	p[4][1] = 6.0f;
	p[4][2] = 0;

	p[0][0] = 0;
	p[0][1] = 0;
	p[0][2] = 40.0f;

	p[1][0] = 0;
	p[1][1] = 0;
	p[1][2] = 20.0f;

	p[2][0] = 0;
	p[2][1] = 0;
	p[2][2] = 340.0f;

	p[3][0] = 0;
	p[3][1] = 0;
	p[3][2] = 320.0f;

	AngleMatrix((const float *)&p[0],Matrix);
	VectorRotate((const float *)&p[4],(const float *)&Matrix[0],(float *)&vFrustrum[0]);

	AngleMatrix((const float *)&p[1],Matrix);
	VectorRotate((const float *)&p[4],(const float *)&Matrix[0],(float *)&vFrustrum[1]);

	vFrustrum[2][0] = 0;
	vFrustrum[2][1] = 6.0f;
	vFrustrum[2][2] = 0;

	AngleMatrix((const float *)&p[2],Matrix);
	VectorRotate((const float *)&p[4],(const float *)&Matrix[0],(float *)&vFrustrum[3]);

	AngleMatrix((const float *)&p[3],Matrix);
	VectorRotate((const float *)&p[4],(const float *)&Matrix[0],(float *)&vFrustrum[4]);

	p[0][0] = 0;
	p[0][1] = 0;
	p[0][2] = targetpos*360/255;

	AngleMatrix((const float *)&p[0],Matrix);

	int fSkillFrustrumX[5];
	int fSkillFrustrumY[5];

	float vFrustrum2[5][3];

	for(int i = 0; i < 5; i++)
	{
		VectorRotate(vFrustrum[i],(const float *)&Matrix[0],vFrustrum2[i]);

		fSkillFrustrumX[i] = int(vFrustrum2[i][0]) + lpObj->X;
		fSkillFrustrumY[i] = int(vFrustrum2[i][1]) + lpObj->Y;
	}

	int tObjNum;
	int count = 0;
	int tmpvar = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0 && gObj[tObjNum].Live)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != FALSE )
				{
					if( SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex) )
					{

						if( lpMagic->m_Skill == AT_MSKILL_FE_MULTISHOT2 )
						{
							float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

							if( fSkillValue > 0.0f )
							{
								float fpercent = rand()%100;

								if( fpercent < fSkillValue )
								{
									gObjAddBuffEffect(&gObj[tObjNum],BUFF_STUN,0,0,0,0,2);
									gObjSetPosition(gObj[tObjNum].m_Index,gObj[tObjNum].X,gObj[tObjNum].Y);
								}
							}
						}

						for(int i = 0; i < 5; i++)
						{
							float Dis = (fSkillFrustrumY[i] - lpObj->Y)*(gObj[tObjNum].Y - lpObj->Y) - (fSkillFrustrumX[i] - lpObj->X)*(gObj[tObjNum].X - lpObj->X);
						
							if( (Dis >= -5 && Dis <= 5) || tObjNum == aTargetIndex )
							{
								gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,0,0,0,0,0);
							}
						}
					}
				}
			}
		}

		count++;

		if( count > (MAX_VIEWPORT-1) )
		{
			break;
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillFrustrum3(int aIndex,BYTE aTargetPos,float f1,float f2,float f3,float f4)
{
	LPOBJ lpObj = &gObj[aIndex];

	float p[MAX_ARRAY_FRUSTRUM][3];

	p[0][0] = -f1;
	p[0][1] = f2;
	p[0][2] = 0;
	p[1][0] = f1;
	p[1][1] = f2;
	p[1][2] = 0;
	p[2][0] = f3;
	p[2][1] = f4;
	p[2][2] = 0;
	p[3][0] = -f3;
	p[3][1] = f4;
	p[3][2] = 0;

	float Angle[3];

	Angle[0] = 0;
	Angle[1] = 0;
	Angle[2] = (float)(aTargetPos * 360 / 255);

	float Matrix[3][MAX_ARRAY_FRUSTRUM];
	float vFrustrum[MAX_ARRAY_FRUSTRUM][3];

	AngleMatrix(Angle,Matrix);

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM; i++)
	{
		VectorRotate((const float *)&p[i],(const float *)&Matrix[0],(float *)&vFrustrum[i]);

		lpObj->fSkillFrustrumX[i] = (int)vFrustrum[i][0] + lpObj->X;
		lpObj->fSkillFrustrumY[i] = (int)vFrustrum[i][1] + lpObj->Y;
	}
}

int CMasterLevelSkillTreeSystem::MLS_SkillBless(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	int skillSuccess = true;

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( lpObj->Type != OBJ_USER )
	{
		return true;
	}

	int iAddStats = lpObj->Energy/100;

	if( gObjCheckPowerfulEffect(lpTargetObj,BUFF_BLESS,iAddStats,0) )
	{
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,0);
		return false;
	}

	if( lpMagic->m_Skill == AT_MSKILL_FE_BLESS1 )
	{
		float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

		if( fSkillValue > 0.0f )
		{
			iAddStats += fSkillValue;
		}

		gObjAddBuffEffect(lpTargetObj,BUFF_BLESS2,ADD_OPTION_ALLSTATS,iAddStats,0,0,300);
	}
	else
	{
		gObjAddBuffEffect(lpTargetObj,BUFF_BLESS,ADD_OPTION_ALLSTATS,iAddStats,0,0,300);
	}

	
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
	gObjCalCharacter(lpTargetObj->m_Index);
	return true;
}

void CMasterLevelSkillTreeSystem::MLS_SkillGiganticStorm(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex)
{
	MLS_SkillAreaMonsterAttack(aIndex,lpMagic,x,y,aTargetIndex,6,1,0,400);
}

void CMasterLevelSkillTreeSystem::MLS_SkillFlameStrike(int aIndex, CMagicInf * lpMagic,BYTE targetangle, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = TRUE;
	int tObjNum;

	MLS_SkillFrustrum3(aIndex,targetangle,2.0f,4.0f,5.0f,0);

	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if( tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if( SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex) )
					{
						bAttack = FALSE;

						if(HitCount > 8)
						{
							if( rand()%2 )
							{
								bAttack = TRUE;
							}
						}
						else
						{
							bAttack = TRUE;
						}

						if(bAttack != FALSE)
						{
							if( lpMagic->m_Skill == AT_MSKILL_MG_FLAMESTRIKE2 )
							{
								float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

								if( fSkillValue > 0.0f )
								{
									float fpercent = rand()%100;

									if( fpercent < fSkillValue )
									{
										gObjBackSpring2(&gObj[tObjNum],lpObj,2);
									}
								}
							}
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,0,0,0,0,0);
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,0,0,0,0,0);
						}

						HitCount++;
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

}



int CMasterLevelSkillTreeSystem::MLS_SkillWeakness(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE(lpObj->MapNumber) ||
						 IT_MAP_RANGE(lpObj->MapNumber) ||
						 lpObj->MapNumber == MAP_INDEX_VULCANUS)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
					{
						if( gObj[tObjNum].Class != 283 &&
							gObj[tObjNum].Class != 277 )
							EnableAttack = 1;
					}
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					gObjCalDistance(lpObj,&gObj[tObjNum]) < 5 )
				{
					gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,700,lpMagic->m_Skill,0);
					HitCount++;

					if(HitCount >= 5)
					{
						break;
					}
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bSuccess;
}

int CMasterLevelSkillTreeSystem::MLS_SkillInnovation(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE(lpObj->MapNumber) ||
						 IT_MAP_RANGE(lpObj->MapNumber) ||
						 lpObj->MapNumber == MAP_INDEX_VULCANUS)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
					{
						if( gObj[tObjNum].Class != 283 &&
							gObj[tObjNum].Class != 277 )
							EnableAttack = 1;
					}
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					gObjCalDistance(lpObj,&gObj[tObjNum]) < 5 )
				{
					gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,700,lpMagic->m_Skill,0);
					HitCount++;

					if(HitCount >= 5)
					{
						break;
					}
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bSuccess;
}

int CMasterLevelSkillTreeSystem::MLS_SkillBlind(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;
	int tObjNum;

	int Angle = MLS_GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	int count = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int Rate = 0;

	if( gObj[aTargetIndex].Type == OBJ_MONSTER )
	{
		Rate = lpObj->m_AttackRating * (20 + lpObj->Energy / 50) / 100;
	}
	else
	{
		int Tmp = lpObj->Level * 3 + lpObj->Dexterity * 35 / 10;
		Rate = Tmp * (20 + lpObj->Energy / 50) / 100;
	}

	if( Rate < rand()%100 )
	{
		GCDamageSend(lpObj->m_Index,gObj[aTargetIndex].m_Index,0,0,0,0);
		return false;
	}

	int EnableAttack;

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex && DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE(lpObj->MapNumber) ||
						 IT_MAP_RANGE(lpObj->MapNumber) || 
						 lpObj->MapNumber == MAP_INDEX_VULCANUS )
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					tObjNum == aTargetIndex && 
					gCheckSkillDistance(aIndex,aTargetIndex,lpMagic->m_Skill) != FALSE )
				{
					int BuffNumber = (lpMagic->m_Skill==AT_MSKILL_SUM_BLIND1) ? BUFF_BLIND2 : BUFF_BLIND;
					int Value = 20 + lpObj->Energy / 50;

					if( Value > 100 )
						Value = 100;

					int Time = 0;

					if( gObj[aTargetIndex].Type == OBJ_MONSTER )
					{
						Time = (9 + lpObj->Energy/46) - (gObj[aTargetIndex].Level / 15);
					}
					else if( gObj[aTargetIndex].Type == OBJ_USER )
					{
						Time = 3 + (lpObj->Energy/210) + (gObj[aTargetIndex].Level-gObj[aTargetIndex].Level) / 100;
					}

					if( BuffNumber == BUFF_BLIND2 )
					{
						float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

						if( fSkillValue > 0.0f )
						{
							float fpercent = rand()%100;

							if( fpercent < fSkillValue )
							{
								gObjAddBuffEffect(&gObj[aTargetIndex],BUFF_STUN,0,0,0,0,2);
								gObjSetPosition(aTargetIndex,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
							}
						}
					}

					if( gObjCheckPowerfulEffect(&gObj[aTargetIndex],BuffNumber,Value,0) == TRUE )
					{
						GCMagicAttackNumberSend(lpObj ,lpMagic->m_Skill, aTargetIndex, 0);
						return FALSE;
					}

					gObjAddBuffEffect(&gObj[aTargetIndex],BuffNumber,ADD_OPTION_DECATTACKRATE,Value,0,0,Time);
					bSuccess = TRUE;
					break;
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bSuccess;
}

void CMasterLevelSkillTreeSystem::MLS_SkillElectricSpark(int aIndex, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int TargePosx = lpObj->X - (8 - (TargetPos & 15));
	int TargePosy = lpObj->Y - (8 - ((TargetPos & 240) >> 4));

	int tObjNum;
	int count = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int iangle = MLS_GetAngle(lpObj->X,lpObj->Y,TargePosx,TargePosy);
	int delaytime;
	int Hit = 0;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(SkillElectricSparkHitBox.HitCheck(iangle,gObj[aIndex].X,gObj[aIndex].Y,gObj[tObjNum].X,gObj[tObjNum].Y))
					{
						delaytime = 500;
						Hit = 1;

						gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,delaytime,lpMagic->m_Skill,0);
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

	if(Hit != 0)
	{
		DecPartyMemberHPandMP(lpObj);
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillChaoticDesier(int aIndex, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = TRUE;

	MLS_SkillFrustrum3(aIndex,TargetPos,1.5f,6.0f,1.5f,0);

	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if( tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != FALSE && SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex) )
				{
					bAttack = FALSE;

					if( HitCount > 8 )
					{
						if(rand()%2)
						{
							bAttack = TRUE;
						}
					}
					else
					{
						bAttack = TRUE;
					}

					if( bAttack != FALSE )
					{
						gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,250,lpMagic->m_Skill,0);
					}

					if( OBJMAX_RANGE(aTargetIndex) && aTargetIndex == tObjNum)
					{
						gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,300,lpMagic->m_Skill,0);
					}

					HitCount++;
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillIronDefense(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	int skillSuccess = true;

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( lpObj->Type != OBJ_USER || lpObj->Class != CLASS_DARKLORD)
	{
		return;
	}


	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	int AddDefense = lpInfo->m_Arg1;
	int AddLife = lpInfo->m_Arg2;

	int BuffNumber = BUFF_IRONDEFENSE;

	if( lpMagic->m_Skill == AT_MSKILL_DL_IRONDEFENSE1 )
	{
		BuffNumber = BUFF_IRONDEFENSE2;

		float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

		AddDefense += fSkillValue;
		AddLife += fSkillValue;
	}

	AddDefense = (AddDefense*10)/20;

	gObjAddBuffEffect(lpTargetObj,BuffNumber,ADD_OPTION_DEFENSE,AddDefense,ADD_OPTION_LIFE,AddLife,lpInfo->m_Arg3);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}

void CMasterLevelSkillTreeSystem::MLS_SkillPartyHeal(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	int skillSuccess = TRUE;
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Class != CLASS_ELF)
	{
		return;
	}

	if( !OBJMAX_RANGE(aTargetIndex) )
	{
		aTargetIndex = aIndex;
	}
	
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( lpObj->PartyNumber == -1 || lpObj->PartyNumber != lpTargetObj->PartyNumber )
	{
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aIndex,FALSE);
		return;
	}	

	int partynumber = lpObj->PartyNumber;
	int addlife = (5 + (lpObj->Energy + lpObj->AddEnergy) / 5);

	if( lpMagic->m_Skill == AT_MSKILL_FE_PARTYHEAL1 )
	{
		float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

		if( fSkillValue > 0.0f )
		{
			addlife += addlife*fSkillValue/100.0f;
		}
	}

	int iApplyHealIndex[4];
	int iApplyHealDistance[4];
	memset(iApplyHealIndex,-1,sizeof(iApplyHealIndex));
	memset(iApplyHealDistance,-1,sizeof(iApplyHealDistance));

	iApplyHealIndex[0] = aTargetIndex;
	iApplyHealDistance[0] = 0;

	int i;

	for( i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		int number = gParty.m_PartyS[partynumber].Number[i];

		if( number >= 0 )
		{
			if( lpTargetObj->MapNumber != gObj[number].MapNumber )
				continue;

			int dis = gObjCalDistance(lpTargetObj,&gObj[number]);

			if( dis >= 10 )
				continue;

			if( iApplyHealIndex[1] == -1 || iApplyHealDistance[1] > dis )
			{
				iApplyHealIndex[1] = number;
				iApplyHealDistance[1] = dis;
			}
			else if( iApplyHealIndex[2] == -1 || iApplyHealDistance[2] > dis )
			{
				iApplyHealIndex[2] = number;
				iApplyHealDistance[2] = dis;
			}
			else if( iApplyHealIndex[3] == -1 || iApplyHealDistance[3] > dis )
			{
				iApplyHealIndex[3] = number;
				iApplyHealDistance[3] = dis;
			}
		}
	}

	for( i = 0; i < 4; i++)
	{
		if( iApplyHealIndex[i] != -1 )
		{
			LPOBJ lpHealObj = &gObj[iApplyHealIndex[i]];
			lpHealObj->Life += addlife;

			if( lpHealObj->Life > lpHealObj->MaxLife+lpHealObj->AddLife )
				lpHealObj->Life = lpHealObj->MaxLife+lpHealObj->AddLife;

			GCReFillSend(lpTargetObj->m_Index,(WORD)lpTargetObj->Life,0xFF,0,lpTargetObj->iShield);
			GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpHealObj->m_Index,skillSuccess);
		}
	}
}

char CMasterLevelSkillTreeSystem::CheckMasterSkillPoint(int aIndex, int nTreeType) {
	LPOBJ lpUser = &gObj[aIndex];
	int nSkillCnt = 0;
	int iClass = -1;

	if (lpUser == NULL) {
		return -1;
	}

	if (nTreeType < 0 || nTreeType > MAX_SKILLTREE_TYPE) {
		return -1;
	}

	iClass = lpUser->Class;

	if (nTreeType > 0) {
		for (int n = 0; n < MAGIC_SIZE; n++) {
			if (lpUser->Magic[n].IsMagic()
				&& g_MasterSkillSystem.CheckMasterLevelSkill(lpUser->Magic[n].m_Skill)) {
				if (this->CheckSkillInTree(iClass, lpUser->Magic[n].m_Skill, nTreeType - 1)) {
					nSkillCnt++;
				}
			}
		}

		if (nSkillCnt == 0) {
			return 0;
		}
	}

	return 1;
}

char CMasterLevelSkillTreeSystem::ResetMasterSkill(int aIndex, int nTreeType) {
	LPOBJ lpUser = &gObj[aIndex];
	int iClass = 0;

	if (lpUser == NULL) {
		return -1;
	}

	if (nTreeType < 0 || nTreeType > MAX_SKILLTREE_TYPE) {
		return -1;
	}

	iClass = lpUser->Class;

	for (int n = 0; n < MAGIC_SIZE; n++) {
		if (lpUser->Magic[n].IsMagic()
			&& g_MasterSkillSystem.CheckMasterLevelSkill(lpUser->Magic[n].m_Skill)
			&& (nTreeType == 0 || this->CheckSkillInTree(iClass, lpUser->Magic[n].m_Skill, nTreeType - 1))) {
				if (MagicDamageC.GetSkillUseType(lpUser->Magic[n].m_Skill) == 3) {
					lpUser->MasterPoint += lpUser->Magic[n].m_Level;
					lpUser->Magic[n].Clear();
				}
				else {
					int iBrandOfSkill = MagicDamageC.GetBrandOfSkill(lpUser->Magic[n].m_Skill);

					if (iBrandOfSkill == 0) {
						lpUser->MasterPoint += lpUser->Magic[n].m_Level;
						lpUser->Magic[n].Clear();
					}
					else if (g_MasterSkillSystem.CheckMasterLevelSkill(iBrandOfSkill)) {
						lpUser->MasterPoint += lpUser->Magic[n].m_Level;
						lpUser->Magic[n].Clear();
					}
					else {
						CItem *lpLeftItem = &lpUser->pInventory[1];
						CItem *lpRightItem = &lpUser->pInventory[0];

						if (lpLeftItem->m_Special[0] == iBrandOfSkill) {
							lpUser->MasterPoint += lpUser->Magic[n].m_Level;
							lpUser->Magic[n].UpdateMasterSkill(iBrandOfSkill, (BYTE)lpLeftItem->m_Level);
						}
						else if (lpRightItem->m_Special[0] == iBrandOfSkill) {
							lpUser->MasterPoint += lpUser->Magic[n].m_Level;
							lpUser->Magic[n].UpdateMasterSkill(iBrandOfSkill, (BYTE)lpRightItem->m_Level);
						}
						else {
							lpUser->MasterPoint += lpUser->Magic[n].m_Level;
							lpUser->Magic[n].UpdateMasterSkill(iBrandOfSkill, 0);
						}
					}
				}
		}
	}
	
	return false;
}

bool CMasterLevelSkillTreeSystem::CheckSkillInTree(int iClass, int iSkill, int iTreeKind) {
	int iPos;
	int iRank;

	for (int iRank = 0; iRank < MAX_SKILLTREE_RANK; iRank++) {
		for (int iPos = 0; iPos < MAX_SKILLTREE_POS; iPos++) {
			if (this->m_SkillTreeData[iClass].m_SkillInfo[iTreeKind][iRank][iPos].m_iSkillID == iSkill) {
				return true;
			}
		}
	}
	return false;
}