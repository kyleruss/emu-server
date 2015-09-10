#include "stdafx.h"
#include "ObjAttack.h"
#include "BattleSoccerManager.h"
#include "DarkSpirit.h"
#include "ObjUseSkill.h"
#include "gObjMonster.h"
#include "BloodCastle.h"
#include "ChaosCastle.h"
#include "CastleSiege.h"
#include "Crywolf.h"
#include "IllusionTempleEvent.h"
#include "BuffEffectSlot.h"
#include "MasterSkillSystem.h"
#include "MasterLevelSkillTreeSystem.h"
#include "Gamemain.h"
#include "ItemAddOption.h"
#include "ImperialGuardian.h"
#include "ElementalSystem.h"
#include "SoulSystem.h"
#include "BuffSkillEffect.h"

#if (ENABLETEST_ACHERON == 1)
#include "AcheronEvent.h"
#endif

#if (ENABLETEST_ARCA == 1)
#include "ArcaBattle.h"
#endif

#if (ENABLETEST_ZTLUA == 1)
#include "ZTLuaManager.h"
#endif

#if (ENABLETEST_MUUN == 1)
#include "MuunSystem.h"
#endif

#if (ENABLETEST_NEWPVP == 1)
#include "NewPVP.h"
#endif

#if (ENABLE_CUSTOM_PVPBALANCE == 1)
#include "PvPBalance.h"
#endif

CObjAttack gclassObjAttack;

CObjAttack::CObjAttack()
{
	return;
}




CObjAttack::~CObjAttack()
{
	return;
}


BOOL CObjAttack::Attack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf* lpMagic,  int magicsend, unsigned char MSBFlag, int AttackDamage, BOOL bCombo,BYTE byBarrageCount, BYTE byReflect)
{
	int skillSuccess = 0;
	LPOBJ lpCallObj;
	LPOBJ lpCallTargetObj;
	int MsgDamage = 0;
	int ManaChange = 0;
	int iTempShieldDamage = 0;
	int iTotalShieldDamage = 0;
	int bDragonKickSDAttackSuccess = 0;

	if ( (lpTargetObj->Authority&2) == 2 )
		return FALSE;

	if ( (lpObj->Authority&32) == 32 ||
		 (lpTargetObj->Authority&32) == 32 )
	{
		if( gObjCheckUsedBuffEffect(lpObj,BUFF_INVISIBILITY) == TRUE)
		{
			return false;
		}
	}

	if ( lpObj->MapNumber != lpTargetObj->MapNumber )
	{
		return false;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (lpObj->Type == OBJ_USER && g_NewPVP.IsDuel(*lpObj) && g_NewPVP.IsSafeState(*lpObj)) {
		return false;
	}
	
	if (lpTargetObj->Type == OBJ_USER && g_NewPVP.IsDuel(*lpTargetObj) && g_NewPVP.IsSafeState(*lpTargetObj)) {
		return false;
	}
#endif

#if (__GENS_SYSTEM__ == 1)
	if (!gGensSystem.GetFriendlyFire() && gGensSystem.GensMap(lpObj->MapNumber)) {
		if (lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER
			&& lpObj->m_GensInfo.btFamily == lpTargetObj->m_GensInfo.btFamily) {
			return false;
		}
	}
#endif

#if (GS_CASTLE == 1)

	if(g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_READY || g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_END)
	{
		if( ((lpTargetObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE) ? TRUE : FALSE) && lpTargetObj->Type == OBJ_MONSTER)
		{
			return FALSE;
		}
	}

#endif

#if (ENABLETEST_ARCA)
	if (g_ArcaBattle.IsArcaBattleServer() && !g_AcheronGuardianEvent.IsPlayStart() )
	{
		if (lpObj->Type == 1 && lpTargetObj->Type == 1 && !g_ArcaBattle.IsPkEnable(*lpObj, *lpTargetObj))
		  return 0;

		if (lpObj->Type == 1 && lpTargetObj->Type == 2 && !g_ArcaBattle.IsEnableAttackObelisk(*lpObj, lpTargetObj->Class))
		  return 0;

		if (lpObj->Type == 2 && lpTargetObj->Type == 2 && lpObj->m_Attribute == 100 
			&& !g_ArcaBattle.IsEnableAttackObelisk(*lpObj, lpTargetObj->Class))
		{
			//lpObj->m_ActState._bf0 &= 0xFFFF00FFu;
			//lpObj->m_ActState._bf0 &= 0xFFFFFF0Fu;
			return 0;
		}
	}
#endif

	if ( g_iUseCharacterAutoRecuperationSystem && lpObj->Level <= g_iCharacterRecuperationMaxLevel )
	{
		if ( lpObj->Type == OBJ_USER )
		{
			if ( lpObj->Level <= g_iCharacterRecuperationMaxLevel ) // #warning useless code
			{
				lpObj->m_iAutoRecuperationTime = GetTickCount();
			}
		}

		if ( lpTargetObj->Type == OBJ_USER )
		{
			if ( lpTargetObj->Level <= g_iCharacterRecuperationMaxLevel )
			{
				lpTargetObj->m_iAutoRecuperationTime = GetTickCount();
			}
		}
	}

	if ( lpObj->Type == OBJ_USER )
		lpObj->dwShieldAutoRefillTimer = GetTickCount();

	if ( lpTargetObj->Type == OBJ_USER )
		lpTargetObj->dwShieldAutoRefillTimer = GetTickCount();

	int skill = 0;
	
	if ( lpMagic )
		skill = lpMagic->m_Skill;

	if ( lpObj->Class == 77 )
	{
		if ( lpObj->m_SkyBossMonSheild && skill == 17 )
		{
			skill = 3;
		}
	}

	skillSuccess = TRUE;

	if ( lpObj->GuildNumber > 0 )
	{
		if ( lpObj->lpGuild )
		{
			if ( lpObj->lpGuild->WarState )
			{
				if ( lpObj->lpGuild->WarType == 1 )
				{
					if ( !GetBattleSoccerGoalMove(0) )
					{
						return TRUE;
					}
				}
			}

			if ( lpObj->lpGuild->WarState )
			{
				if ( lpObj->lpGuild->WarType == 0 )
				{
					if ( lpTargetObj->Type == OBJ_MONSTER )
					{
						return TRUE;
					}
				}
			}
		}
	}

	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
		if ( lpTargetObj->m_iMonsterBattleDelay > 0 )
			return TRUE;

		if ( lpTargetObj->Class == 200 )
		{
			if ( skill )
			{
				gObjMonsterStateProc(lpTargetObj, 7, lpObj->m_Index, 0);
			}
			else
			{
				gObjMonsterStateProc(lpTargetObj, 6, lpObj->m_Index, 0);
			}

			if ( magicsend )
			{
				GCMagicAttackNumberSend(lpObj, skill, lpTargetObj->m_Index, skillSuccess);
			}

			return TRUE;
		}

		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_MONSTERMAGIC_RESIST) == TRUE )
		{
			BOOL bCheckAttackIsMagicType = gObjCheckAttackTypeMagic(lpObj->Class, skill);

			if ( bCheckAttackIsMagicType == 1 )
			{
				GCMagicAttackNumberSend(lpObj, skill, lpTargetObj->m_Index, 0);
				return TRUE;
			}
		}

		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_MONCHEMISTRY_RESIST) == TRUE  )
		{
			BOOL bCheckAttackIsMagicType = gObjCheckAttackTypeMagic(lpObj->Class, skill);

			if ( bCheckAttackIsMagicType == 0 )
			{
				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, 0, 0);
				return TRUE;
			}
		}


		if ( lpTargetObj->m_MonsterSkillElementInfo.m_iSkillElementImmuneTime > 0 )
		{
			if ( lpTargetObj->m_MonsterSkillElementInfo.m_iSkillElementImmuneNumber == skill )
			{
				return TRUE;
			}
		}

		if ( lpTargetObj->m_MonsterSkillElementInfo.m_iSkillElementSpecialImmuneTime > 0 )
		{
			int MSBDamage = 32;
			GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, MSBDamage, 0);
			return TRUE;
		}

		if ( lpTargetObj->Class == 523 )
			return true;

		if ( lpTargetObj->Class >= 524 && lpTargetObj->Class <= 528 && lpTargetObj->Class != 526 )
		{
			if( !g_ImperialGuardian.IsAttackAbleMonster(lpTargetObj->m_Index) )
				return true;
		}
	}

	if ( lpObj->Type == OBJ_USER )
	{
		if ( !gObjIsConnected(lpObj) )
		{
			return FALSE;
		}

		gDarkSpirit[lpObj->m_Index].SetTarget(lpTargetObj->m_Index);
	}

	if ( lpTargetObj->Type == OBJ_USER )
	{
		if ( !gObjIsConnected(lpTargetObj))
		{
			return FALSE;
		}
	}

	if(lpObj->m_RecallMon == lpTargetObj->m_Index)

	if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_MONSTER )	// PLAYER VS MONSTER
	{
		if ( lpObj->m_RecallMon >= 0 )
		{
			if ( lpObj->m_RecallMon == lpTargetObj->m_Index )
			{
				return FALSE;
			}
		}
	}
	
	if ( !gObjAttackQ(lpTargetObj))
	{
		return FALSE;
	}

	if ( lpObj->m_RecallMon >= 0 )
		gObjCallMonsterSetEnemy(lpObj, lpTargetObj->m_Index);

	if ( AttackDamage == 0 )
	{
		if ( skill != 76 )
		{
			if ( gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW) == FALSE &&
				 gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW2) == FALSE )
			{
				if ( !this->DecreaseArrow(lpObj) )
				{
					return FALSE;
				}
			}
		}
	}

	if ( this->CheckAttackArea(lpObj, lpTargetObj) == FALSE )
	{
		return FALSE;
	}

	lpCallObj = lpObj;

	if ( lpObj->Type == OBJ_MONSTER )
	{
		if ( lpObj->m_RecallMon >= 0 )
		{
			lpCallObj = &gObj[lpObj->m_RecallMon];
		}
	}

	lpCallTargetObj = lpTargetObj;

	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
		if ( lpTargetObj->m_RecallMon >= 0 )
		{
			lpCallTargetObj = &gObj[lpTargetObj->m_RecallMon];
		}
	}

	if ( this->PkCheck(lpCallObj, lpTargetObj) == FALSE )
		return FALSE;

	int Strength = lpObj->Strength + lpObj->AddStrength;
	int Dexterity = lpObj->Dexterity + lpObj->AddDexterity;
	int Vitality = lpObj->Vitality + lpObj->AddVitality;
	int Energy = lpObj->Energy + lpObj->AddEnergy;
	BOOL bIsOnDuel = gObjDuelCheck(lpObj, lpTargetObj);

#if (CUSTOM_OLYMP!=1)
	if ( bIsOnDuel )
	{
		lpObj->m_iDuelTickCount = GetTickCount();
		lpTargetObj->m_iDuelTickCount = GetTickCount();
	}
#endif

	if ( lpObj->pInventory[0].m_Type == ITEMGET(2,5) && lpObj->pInventory[0].m_IsValidItem != false )	// Crystal Sword
	{
		if ( (rand()%20) == 0 )
		{
			skill = 7;
			lpMagic = &DefMagicInf[7];
			magicsend = 1;
		}
	}
	else if ( lpObj->pInventory[1].m_Type == ITEMGET(2,5) && lpObj->pInventory[1].m_IsValidItem != false )	// Crystal Sword
	{
		if ( (rand()%20) == 0 )
		{
			skill = 7;
			lpMagic = &DefMagicInf[7];
			magicsend = 1;
		}
	}

	MSBFlag = 0;
	MsgDamage = 0;
	skillSuccess = this->ResistanceCheck(lpObj, lpTargetObj, skill);
	BOOL skillIceArrowSuccess = skillSuccess;

	if ( skill == AT_SKILL_ELFHARDEN || skill == AT_MSKILL_FE_ICEARROW1 )
	{
		skillSuccess = 0;
	}
	BOOL bAllMiss = FALSE;
	
	if ( gObjCheckUsedBuffEffect(lpObj,BUFF_INVISIBILITY) == TRUE && (lpObj->Authority & 32) != 32)
	{
		gObjUseSkill.RemoveCloakingEffect(lpObj->m_Index);
	}

	BOOL bDamageReflect = FALSE;
	
	if ( AttackDamage == 0 )
	{
		if ( g_ShieldSystemOn == TRUE )
		{
			if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
			{
				if ( !this->MissCheckPvP(lpObj, lpTargetObj, skill, skillSuccess, magicsend, bAllMiss, byBarrageCount) )
				{
					return FALSE;
				}
			}
			else if ( !this->MissCheck(lpObj, lpTargetObj, skill, skillSuccess, magicsend, bAllMiss, byBarrageCount) )
			{
				return FALSE;
			}

		}
		else if ( !this->MissCheck(lpObj, lpTargetObj, skill, skillSuccess, magicsend, bAllMiss, byBarrageCount) )
		{
			return FALSE;
		}

		if ( (skill == AT_SKILL_ELFHARDEN || skill == AT_MSKILL_FE_ICEARROW1) && skillIceArrowSuccess == TRUE )
		{
			BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(skill);

			if( !lpInfo )
			{
				return 0;
			}

			gObjAddBuffEffect(lpTargetObj,BUFF_ICEARROW,0,0,0,0,lpInfo->m_Arg1);
			lpTargetObj->lpAttackObj = lpObj;
			lpTargetObj->PathCount = 0;
			lpTargetObj->PathStartEnd = 0;
			skillSuccess = TRUE;
			gObjSetPosition(lpTargetObj->m_Index, lpTargetObj->X, lpTargetObj->Y);
		}
		
		int targetdefense = this->GetTargetDefense(lpObj, lpTargetObj, MsgDamage);
		
		if( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_BERSERKER) == TRUE ||
			gObjCheckUsedBuffEffect(lpTargetObj,BUFF_BERSERKER2) == TRUE ||
			gObjCheckUsedBuffEffect(lpTargetObj,BUFF_BERSERKER3) == TRUE ||
			gObjCheckUsedBuffEffect(lpTargetObj,BUFF_BERSERKER4) == TRUE )
		{
			float fDefense = (lpTargetObj->Dexterity+lpTargetObj->AddDexterity)/3;
			float fValue = (float)gObjGetTotalValueOfEffect(lpTargetObj,ADD_OPTION_BERSERKLIFE_PERC);
			float fPercent = (40- (fValue+lpObj->m_MPSkillOpt.MpsBerserkInc1))/100;
			fPercent = (0.1f > fPercent) ? 0.1f : fPercent;
		}

		int iCurseValue = 0;
		if( !gObjGetValueOfBuffIndex(lpTargetObj,BUFF_INNOVATION2,&iCurseValue,0) )
			gObjGetValueOfBuffIndex(lpTargetObj,BUFF_INNOVATION,&iCurseValue,0);

		targetdefense -= targetdefense * iCurseValue / 100;
		
		switch( skill )
		{
		case AT_SKILL_SWORD1:
		case AT_SKILL_SWORD2:
		case AT_SKILL_SWORD3:
		case AT_SKILL_SWORD4:
		case AT_SKILL_SWORD5:
		case AT_SKILL_SWORD6:
		case AT_SKILL_WHEEL:
		case AT_SKILL_KNIGHTSPEAR:
		case AT_SKILL_BLOWOFFURY:
		case AT_SKILL_KNIGHTDINORANT:
		case AT_SKILL_STRIKE:
		case AT_SKILL_DEFENSEDOWN:
		case AT_SKILL_RUSH:
		case AT_SKILL_ONE_FLASH:
		case AT_SKILL_SPACE_SPLIT:
		case AT_SKILL_MULTISHOT:
		case AT_SKILL_DESTRUCT:
		case AT_SKILL_FLAMESTRIKE:
		case AT_SKILL_CHAOTICDISEIER:
		case AT_MSKILL_DK_CYCLONE1:
		case AT_MSKILL_DK_SLASH1:
		case AT_MSKILL_DK_FALLINGSLASH1:
		case AT_MSKILL_DK_LUNGE1:
		case AT_MSKILL_DK_TWISTINGSLASH1:
		case AT_MSKILL_DK_RAGEFULBLOW1:
		case AT_MSKILL_DK_TWISTINGSLASH2:
		case AT_MSKILL_DK_RAGEFULBLOW2:
		case AT_MSKILL_DK_DEATHSTAB1:
		case AT_MSKILL_DK_DEATHSTAB2:
		case AT_MSKILL_DK_DEATHSTAB3:
		case AT_MSKILL_DK_DESTRUCTION1:
		case AT_MSKILL_DK_DESTRUCTION2:
		case AT_MSKILL_DK_DESTRUCTION3:
		case AT_MSKILL_DK_BLOODSTORM:
		case AT_MSKILL_DK_BLOODSTORM1:
		case AT_MSKILL_FE_MULTISHOT1:
		case AT_MSKILL_FE_MULTISHOT2:
		case AT_MSKILL_MG_CYCLONE1:
		case AT_MSKILL_MG_TWISTINGSLASH1:
		case AT_MSKILL_MG_FIRESLASH1:
		case AT_MSKILL_MG_FIRESLASH2:
		case AT_MSKILL_MG_POWERSLASH1:
		case AT_MSKILL_MG_FLAMESTRIKE1:
		case AT_MSKILL_MG_FLAMESTRIKE2:
			{
				AttackDamage = this->GetAttackDamage(lpObj, targetdefense, MsgDamage, bIsOnDuel, lpMagic);

				if ( AttackDamage > 0 )
				{
					gObjWeaponDurDown(lpObj, lpTargetObj, 0);
				}
			}
			break;
		case 76:
			{
				int iFenrirAttackDmg = 0;

				if ( lpObj->Class == CLASS_KNIGHT || lpObj->Class == CLASS_MAGUMSA )
				{
					iFenrirAttackDmg = lpObj->Strength / 3 + lpObj->Dexterity / 5 + lpObj->Vitality / 5 + lpObj->Energy / 7;	// #formula
				}
				else if ( lpObj->Class == CLASS_WIZARD )
				{
					iFenrirAttackDmg = lpObj->Strength / 5 + lpObj->Dexterity / 5 + lpObj->Vitality / 7 + lpObj->Energy / 3;	// #formula
				}
				else if ( lpObj->Class == CLASS_ELF )
				{
					iFenrirAttackDmg = lpObj->Strength / 5 + lpObj->Dexterity / 3 + lpObj->Vitality / 7 + lpObj->Energy / 5;	// #formula
				}
				else	// Dark Lord
				{
					iFenrirAttackDmg = lpObj->Strength / 5 + lpObj->Dexterity / 5 + lpObj->Vitality / 7 + lpObj->Energy / 3 + lpObj->Leadership / 3;	// #formula
				}

				if ( iFenrirAttackDmg < 0 )
					iFenrirAttackDmg = 0;

				if ( lpObj->m_CriticalDamage > 0 )
				{
					if ( (rand()%100) < lpObj->m_CriticalDamage )
					{
						MsgDamage = 3;
					}
				}

				if ( lpObj->m_ExcelentDamage > 0 )
				{
					if ( (rand()%100) < lpObj->m_ExcelentDamage )
					{
						MsgDamage = 2;
					}
				}

				if ( MsgDamage == 3 )	// Critical Damage
				{
					AttackDamage =  iFenrirAttackDmg + lpMagic->m_DamageMax;
					AttackDamage += lpObj->SetOpAddCriticalDamage;
					int adddamage = 0;
					gObjGetValueOfBuffIndex(lpObj,5,&adddamage,0);
					AttackDamage += adddamage;
					gObjGetValueOfBuffIndex(lpObj,148,&adddamage,0);
					AttackDamage += adddamage;
					AttackDamage += lpObj->m_JewelOfHarmonyEffect.HJOpAddCriticalDamage;
					AttackDamage -= targetdefense;
				}
				else if ( MsgDamage == 2 )	// Excellent
				{
					
					AttackDamage = iFenrirAttackDmg + lpMagic->m_DamageMax;
					AttackDamage += AttackDamage * 20 / 100;
					AttackDamage += lpObj->SetOpAddExDamage;
					AttackDamage -= targetdefense;
				}
				else
				{
					AttackDamage = (iFenrirAttackDmg + lpMagic->m_DamageMin) + (rand()%(lpMagic->m_DamageMax - lpMagic->m_DamageMin + 1));
					AttackDamage -= targetdefense;
				}
			}
			break;
		default:
			{
				if ( ( lpObj->Class == CLASS_WIZARD || lpObj->Class == CLASS_MAGUMSA ) && skill )
				{
					AttackDamage = GetAttackDamageWizard(lpObj, targetdefense, lpMagic, MsgDamage, bIsOnDuel);
	
					if ( AttackDamage > 0 )
					{
						gObjWeaponDurDown(lpObj, lpTargetObj, 1);
					}
				}
				else if( lpObj->Class == CLASS_SUMMONER && skill )
				{
					AttackDamage = GetAttackDamageSummoner(lpObj, targetdefense, lpMagic, MsgDamage, bIsOnDuel);
	
					if ( AttackDamage > 0 )
					{
						gObjWeaponDurDown(lpObj, lpTargetObj, 1);
					}
				}
				else if( lpObj->Class == CLASS_FIGHTER )
				{
					AttackDamage = GetAttackDamage(lpObj, targetdefense, MsgDamage, bIsOnDuel, lpMagic);
	
					if ( AttackDamage > 0 )
					{
						gObjWeaponDurDown(lpObj, lpTargetObj, 0);
					}
				}
				else
				{
					AttackDamage = GetAttackDamage(lpObj, targetdefense, MsgDamage, bIsOnDuel, lpMagic);
					if ( AttackDamage > 0 )
					{
						gObjWeaponDurDown(lpObj, lpTargetObj, 0);
					}
				}
			}
			break;
		}
		
		int nBuffIndex = -1;
		int nValue = 0;

		int attackdamagebefore = AttackDamage;

		switch( MsgDamage )
		{
		case 3:
			nBuffIndex = AT_CSHOP_SCROLL_OF_BATTLE;

			if( nBuffIndex > -1 )
			{
				gObjGetValueOfBuffIndex(lpObj,nBuffIndex,&nValue,0);
				if( nValue > 0 )
				{
					AttackDamage += AttackDamage * nValue / 100;
				}
			}

			nValue = 0;
			nBuffIndex = 0x7A;

			if( nBuffIndex > -1 )
			{
				gObjGetValueOfBuffIndex(lpObj,nBuffIndex,&nValue,0);
				if( nValue > 0 )
				{
					AttackDamage += AttackDamage * nValue / 100;
				}
			}
			break;
		case 2:
			nBuffIndex = AT_CSHOP_SCROLL_OF_STRENGTHENER;
			if( nBuffIndex > -1 )
			{
				gObjGetValueOfBuffIndex(lpObj,nBuffIndex,&nValue,0);
				if( nValue > 0 )
				{
					AttackDamage += AttackDamage * nValue / 100;
				}
			}

			nValue = 0;
			nBuffIndex = 0x7B;

			if( nBuffIndex > -1 )
			{
				gObjGetValueOfBuffIndex(lpObj,nBuffIndex,&nValue,0);
				if( nValue > 0 )
				{
					AttackDamage += AttackDamage * nValue / 100;
				}
			}
			break;
		default:
			nBuffIndex = -1;
			break;
		}

#if (ENABLETEST_MUUN == 1)
		if (ManaChange == 2) {
			int EffectValue1 = 0;
			if (g_CMuunSystem.GetMuunItemValueOfOptType(lpObj, 3, &EffectValue1, 0)) {
				AttackDamage += EffectValue1;
			}
		} else if (ManaChange == 3) {
			int nMuunItemEffectValue = 0;
			if (g_CMuunSystem.GetMuunItemValueOfOptType(lpObj, 4, &nMuunItemEffectValue, 0)) {
				AttackDamage += nMuunItemEffectValue;
			}
		}
#endif

		if( g_bAbilityDebug == TRUE )
		{
			if( MsgDamage == 3 || MsgDamage == 2)
			{
				char szTmpMsg[256];
				sprintf(szTmpMsg,"%s AttackDamage: %d %d",(MsgDamage == 3)?"CRITICAL":"EXCELLENT",
					attackdamagebefore,AttackDamage);
				GCServerMsgStringSend(szTmpMsg,lpObj->m_Index,1);
			}
		}

		if ( bAllMiss )
		{
			AttackDamage = ( AttackDamage * 30 ) / 100;
		}
		
		if ( lpTargetObj->DamageMinus )
		{
			int beforeDamage = AttackDamage;
			AttackDamage -= ( ( AttackDamage * (int)lpTargetObj->DamageMinus) / 100 );
		}

		int tlevel = (lpObj->Level+lpObj->MasterLevel) / 10;

		if ( AttackDamage < tlevel )
		{
			if ( tlevel < 1 )
			{
				tlevel = 1;
			}

			AttackDamage = tlevel;
		}

		if ( lpTargetObj->m_SkillNumber == 18 )
		{
			if ( AttackDamage > 1 )
			{
				AttackDamage >>= 1;
			}
		}

#if(__NOVUS__==1)
		if( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_MONSTER )
		{
			if( g_SoulSystem.IsMap(lpObj->MapNumber) && !g_SoulSystem.WithSoul(lpObj) )
			{
				int OldAttack = AttackDamage;
				AttackDamage = AttackDamage * 5 / 100;
				LogAddC(2, "[DEBUG] [SoulSystem] [%s] [%d / %d] Damage has been decreased", lpObj->Name, OldAttack, AttackDamage);
			}
		}

		if( lpObj->Type == OBJ_MONSTER && lpTargetObj->Type == OBJ_USER )
		{
			if( g_SoulSystem.IsMap(lpTargetObj->MapNumber) && !g_SoulSystem.WithSoul(lpTargetObj) )
			{
				AttackDamage = lpTargetObj->MaxLife + lpTargetObj->AddLife + 1000;
				LogAddC(2, "[DEBUG] [SoulSystem] [%s] Defense has been ignored", lpTargetObj->Name);
			}
		}
#endif
		gObjSpriteDamage(lpTargetObj, AttackDamage);

#if (ENABLETEST_MUUN == 1)
		g_CMuunSystem.MuunItemDamage(lpTargetObj, AttackDamage);
#endif
		if ( gObjSatanSprite(lpObj) == TRUE )
		{
			if( lpObj->Class == CLASS_FIGHTER )
			{
				if( skill == AT_SKILL_RAGEFIGHTER_DARKSIDE ||
					skill == AT_SKILL_RAGEFIGHTER_SIEGE ||
					skill == AT_SKILL_RAGEFIGHTER_CHAINDRIVE ||
					skill == AT_MSKILL_RF_CHAINDRIVE1 ||
					skill == AT_MSKILL_RF_CHAINDRIVE2 ||
					skill == AT_MSKILL_RF_DARKSIDE1 ||
					skill == AT_MSKILL_RF_DARKSIDE2 )
				{
					lpObj->Life -= 4.0f;
				}
				else if( skill == AT_SKILL_RAGEFIGHTER_DRAGONSLAYER )
				{
					lpObj->Life -= 100.0f;
				}
				else if( skill == AT_SKILL_RAGEFIGHTER_WEAPON1 ||
					skill == AT_SKILL_RAGEFIGHTER_WEAPON2 ||
					skill == AT_SKILL_RAGEFIGHTER_DRAGONLORE ||
					skill == AT_MSKILL_RF_KILLBLOW1 || 
					skill == AT_MSKILL_RF_KILLBLOW2 || 
					skill == AT_MSKILL_RF_UPERCUT1 ||
					skill == AT_MSKILL_RF_UPERCUT2 )
				{
					lpObj->Life -= 2.0f;
				}
				else if( skill == AT_SKILL_PHOENIXSHOT )
				{
					lpObj->Life -= 2.0f;
				}
				else
				{
					lpObj->Life -= 3.0f;
				}
			}
			else
			{
				lpObj->Life -= 3.0f;
			}

			if ( lpObj->Life < 0.0f )
			{
				lpObj->Life = 0.0f;
			}
			else
			{
				AttackDamage = AttackDamage * 13 / 10;
			}

			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life, 0xFF, 0, lpObj->iShield);
		}

		if ( gObjAngelSprite(lpTargetObj) == TRUE )
		{
			if ( AttackDamage > 1 )
			{
				float  damage = (AttackDamage * 8) / 10.0f;
				AttackDamage = (int)damage;
			}
		}
		
		
		if ( gObjGuardianSprice(lpTargetObj) == TRUE )
		{
			if ( AttackDamage > 1 )
			{
				float damage = AttackDamage * 7 / 10.0f;
				AttackDamage = (int)damage;
			}
		}

		if ( (lpObj->pInventory[10].IsItem() &&
			 (lpObj->pInventory[10].m_Type == ITEMGET(13, 76) || lpObj->pInventory[10].m_Type == ITEMGET(13, 77) || lpObj->pInventory[10].m_Type == ITEMGET(13, 78)) &&
			 lpObj->pInventory[10].m_Durability > 0.0f) || 

			 (lpObj->pInventory[11].IsItem() &&
			 (lpObj->pInventory[11].m_Type == ITEMGET(13, 76) || lpObj->pInventory[11].m_Type == ITEMGET(13, 77) || lpObj->pInventory[11].m_Type == ITEMGET(13, 78)) &&
			 lpObj->pInventory[11].m_Durability > 0.0f) )
		{
			AttackDamage += 30;
		}

		if ( gObjWingSprite(lpObj) == TRUE )
		{
			CItem * Wing = &lpObj->pInventory[7];

			if ( lpObj->Class == CLASS_WIZARD || lpObj->Class == CLASS_ELF )
			{
				lpObj->Life -= 1.0f;
			}
			else if ( lpObj->Class == CLASS_SUMMONER )
			{
				lpObj->Life -= 1.0f;
			}
			else if( lpObj->Class == CLASS_FIGHTER )
			{
				if( skill == AT_SKILL_RAGEFIGHTER_DARKSIDE ||
					skill == AT_SKILL_RAGEFIGHTER_SIEGE ||
					skill == AT_SKILL_RAGEFIGHTER_CHAINDRIVE ||
					skill == AT_MSKILL_RF_CHAINDRIVE1 ||
					skill == AT_MSKILL_RF_CHAINDRIVE2 ||
					skill == AT_MSKILL_RF_DARKSIDE1 ||
					skill == AT_MSKILL_RF_DARKSIDE2 )
				{
					lpObj->Life -= 4.0f;
				}
				else if( skill == AT_SKILL_RAGEFIGHTER_DRAGONSLAYER )
				{
					lpObj->Life -= 100.0f;
				}
				else if( skill == AT_SKILL_RAGEFIGHTER_WEAPON1 ||
					skill == AT_SKILL_RAGEFIGHTER_WEAPON2 ||
					skill == AT_SKILL_RAGEFIGHTER_DRAGONLORE ||
					skill == AT_MSKILL_RF_KILLBLOW1 || 
					skill == AT_MSKILL_RF_KILLBLOW2 || 
					skill == AT_MSKILL_RF_UPERCUT1 ||
					skill == AT_MSKILL_RF_UPERCUT2 )
				{
					lpObj->Life -= 2.0f;
				}
				else if( skill == AT_SKILL_PHOENIXSHOT )
				{
					lpObj->Life -= 2.0f;
				}
				else
				{
					lpObj->Life -= 3.0f;
				}
			}
			else
			{
				lpObj->Life -= 3.0f;
			}

			if ( lpObj->Life < 0.0f )
			{
				lpObj->Life = 0.0f;
			}
			else
			{
				if( Wing->m_Type >= ITEMGET(12,36) && Wing->m_Type <= ITEMGET(12,40) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level*2 + 139) / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,262) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level + 127) / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,263) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level + 133) / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,264) || Wing->m_Type == ITEMGET(12,265) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level + 135) / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,266) )
				{
					AttackDamage = AttackDamage * 171 / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,267) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level + 160) / 100;
				}
#if (CUSTOM_WINGS == 1)
				else if( Wing->m_Type >= ITEMGET(12,440) && Wing->m_Type <= ITEMGET(12,445))
				{
					if( Wing->m_Type == ITEMGET(12,444) )
					{
						AttackDamage = AttackDamage * (Wing->m_Level*3 + 130) / 100;
					}
					else
					{
						AttackDamage = AttackDamage * (Wing->m_Level*3 + 139) / 100;
					}
				}
#endif
				else if( Wing->m_Type == ITEMGET(13,30) || Wing->m_Type == ITEMGET(12,130) || Wing->m_Type == ITEMGET(12,135) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level*2 + 120) / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,41) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level*2 + 112) / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,42) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level + 132) / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,43) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level*2 + 139) / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,49) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level*2 + 120) / 100;
				}
				else if( Wing->m_Type == ITEMGET(12,50) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level*2 + 139) / 100;
				}
				else if( Wing->m_Type > ITEMGET(12,2) )
				{
					AttackDamage = AttackDamage * (Wing->m_Level + 132) / 100;
				}
				else
				{
					AttackDamage = AttackDamage * (Wing->m_Level*2 + 112) / 100;
				}
			}

			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life, 0xFF, 0, lpObj->iShield);
		}

		int RestoreHP = 0;
		int RestoreMP = 0;
		int RestoreSD = 0;

#if(__NOVUS__==1)
		if( lpObj->Type == OBJ_MONSTER && lpTargetObj->Type == OBJ_USER )
		{
			if( g_SoulSystem.IsMap(lpTargetObj->MapNumber) && !g_SoulSystem.WithSoul(lpTargetObj) )
			{

			}
		}
		else
		{
#endif
		if ( gObjWingSprite(lpTargetObj) == TRUE )
		{
			CItem * Wing = &lpTargetObj->pInventory[7];

			if ( AttackDamage > 1 )
			{
				if( (Wing->m_Type >= ITEMGET(12,36) && Wing->m_Type <= ITEMGET(12,40)) ||
					 Wing->m_Type == ITEMGET(12,43) || Wing->m_Type == ITEMGET(12,50) )
				{
					if( Wing->m_Type == ITEMGET(12,40) )
					{
						float damage = AttackDamage * (76 - Wing->m_Level*2) / 100.0f;
						AttackDamage = (int)damage;
					}
					else if( Wing->m_Type == ITEMGET(12,50) )
					{
						float damage = AttackDamage * (76 - Wing->m_Level*2) / 100.0f;
						AttackDamage = (int)damage;
					}
					else
					{
						float damage = AttackDamage * (61 - Wing->m_Level*2) / 100.0f;
						AttackDamage = (int)damage;
					}

					BYTE ExcOp = lpTargetObj->pInventory[7].m_NewOption;

					if( (ExcOp&4) == 4 )
						RestoreHP += 5;
					
					if( (ExcOp&8) == 8 )
						RestoreMP += 5;
				}
				else if( Wing->m_Type >= ITEMGET(12,262) && Wing->m_Type <= ITEMGET(12,265) )
				{
					float damage = AttackDamage * (70 - Wing->m_Level*2) / 100.0f;
					AttackDamage = (int)damage;

					BYTE ExcOp = lpTargetObj->pInventory[7].m_NewOption;

					if( (ExcOp&2) == 2 )
						RestoreHP += 5;
				}
				else if( Wing->m_Type == ITEMGET(12,266) )
				{
					float damage = AttackDamage * 29 / 100.0f;
					AttackDamage = (int)damage;

					BYTE ExcOp = lpTargetObj->pInventory[7].m_NewOption;

					if( (ExcOp&8) == 8 )
						RestoreMP += 5;

					if( (ExcOp&4) == 4 )
						RestoreHP += 5;
				}
				else if( Wing->m_Type == ITEMGET(12,267) )
				{
					float damage = AttackDamage * (40 - Wing->m_Level) / 100.0f;
					AttackDamage = (int)damage;

					BYTE ExcOp = lpTargetObj->pInventory[7].m_NewOption;

					if( (ExcOp&4) == 4 )
						RestoreHP += 5;
				}
				else if( Wing->m_Type == ITEMGET(12,49) )
				{
					float damage = AttackDamage * (90 - Wing->m_Level*2) / 100.0f;
					AttackDamage = (int)damage;
				}
				else if( Wing->m_Type == ITEMGET(12,130) || Wing->m_Type == ITEMGET(12,135) )
				{
					float damage = AttackDamage * (80 - Wing->m_Level*2) / 100.0f;
					AttackDamage = (int)damage;
				}
				else if( Wing->m_Type == ITEMGET(13,30) )
				{
					float damage = AttackDamage * (90 - Wing->m_Level) / 100.0f;
					AttackDamage = (int)damage;
				}
#if (CUSTOM_WINGS == 1)
				else if( Wing->m_Type >= ITEMGET(12,440) && Wing->m_Type <= ITEMGET(12,445) )
				{
					float damage;

					if( Wing->m_Type == ITEMGET(12,444) )
					{
						damage = AttackDamage * (70 - Wing->m_Level*3) / 100.0f;
					}
					else
					{
						damage = AttackDamage * (61 - Wing->m_Level*3) / 100.0f;
					}
					
					AttackDamage = (int)damage;

					BYTE ExcOp = lpTargetObj->pInventory[7].m_NewOption;

					if( (ExcOp&4) == 4 )
					{
						RestoreHP += 10;
					}
				}
#endif
				else if( Wing->m_Type > ITEMGET(12,2) )
				{
					float damage = AttackDamage * (75 - Wing->m_Level*2) / 100.0f;
					AttackDamage = (int)damage;
				}
				else
				{
					float damage = AttackDamage * (88 - Wing->m_Level*2) / 100.0f;
					AttackDamage = (int)damage;
				}
			}
		}
#if(__NOVUS__==1)
		}
#endif

		if( lpTargetObj->Type == OBJ_USER )
		{
			RestoreHP += (int)lpTargetObj->m_MPSkillOpt.MpsRestoreFullHP;
			RestoreMP += (int)lpTargetObj->m_MPSkillOpt.MpsRestoreFullMP;
			RestoreSD += (int)lpTargetObj->m_MPSkillOpt.MpsRestoreFullSD;

			if( RestoreHP && rand()%100 < RestoreHP )
			{
				gObjAddMsgSendDelay(lpTargetObj,13,lpObj->m_Index,100,0);
			}
			else if( RestoreMP && rand()%100 < RestoreMP )
			{
				gObjAddMsgSendDelay(lpTargetObj,14,lpObj->m_Index,100,0);
			}
			else if( RestoreSD && rand()%100 < RestoreSD )
			{
				gObjAddMsgSendDelay(lpTargetObj,15,lpObj->m_Index,100,0);
			}
		}

		if ( gObjDenorantSprite(lpObj ) )
		{
			lpObj->Life -= 1.0f;

			if ( lpObj->Life < 0.0f )
			{
				lpObj->Life = 0.0f;
			}
			else
			{
				AttackDamage = AttackDamage * 115 / 100;
			}

			GCReFillSend(lpObj->m_Index, (WORD)lpObj->Life, 0xFF, 0, lpObj->iShield);
		}

		if ( gObjDenorantSprite(lpTargetObj ) )
		{
			CItem * Dinorant = &lpTargetObj->pInventory[8];
			int dinorantdecdamage = 90 - Dinorant->IsDinorantReduceAttackDamaege();
			lpObj->Life -= 1.0f;

			if ( lpObj->Life < 0.0f )
			{
				lpObj->Life = 0.0f;
			}
			else
			{
				AttackDamage = AttackDamage * dinorantdecdamage / 100;
			}

			GCReFillSend(lpObj->m_Index, (WORD)lpObj->Life, 0xFF, 0, lpObj->iShield);
		}

		if ( gObjDarkHorse(lpTargetObj ) )
		{
			CItem * Darkhorse = &lpTargetObj->pInventory[8];
			int decdamage = 100 - ((Darkhorse->m_PetItem_Level + 30) / 2 );

			lpTargetObj->Life -= 1.0f;

			if ( lpTargetObj->Life < 0.0f )
			{
				lpTargetObj->Life = 0.0f;
			}
			else
			{
				AttackDamage = AttackDamage * decdamage / 100;
			}

			GCReFillSend(lpTargetObj->m_Index, (WORD)lpTargetObj->Life, 0xFF, 0, lpTargetObj->iShield);
		}

		if ( lpTargetObj->Live )
		{
			switch ( skill )
			{
				case AT_SKILL_SWORD1:
				case AT_SKILL_SWORD2:
				case AT_SKILL_SWORD3:
				case AT_SKILL_SWORD4:
				case AT_SKILL_SWORD5:
				case AT_SKILL_WHEEL:
				case AT_SKILL_BLOWOFFURY:
				case AT_SKILL_STRIKE:
				case AT_SKILL_RUSH:
				case AT_SKILL_KNIGHTDINORANT:
				case AT_SKILL_DEFENSEDOWN:
				case AT_SKILL_ONE_FLASH:
					if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA )
					{
						AttackDamage *= 2;
					}
					else if( lpObj->Class == CLASS_FIGHTER )
					{
						AttackDamage *= 2;
					}
					else
					{
						AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100;
					}
					break;
				case AT_SKILL_KNIGHTSPEAR:
					if ( lpObj->pInventory[8].m_Type == ITEMGET(13,3) ||
						 lpObj->pInventory[8].m_Type == ITEMGET(13,2) ||
						 lpObj->pInventory[8].m_Type == ITEMGET(13,37) )
					{
						if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA )
						{
							AttackDamage *= 2;
						}
						else
						{
							AttackDamage = ( AttackDamage * ( Energy / 10 + 200 )  ) / 100;
						}
					}
					break;
				case AT_SKILL_SWORD6:
					AttackDamage *= 2;
					break;
				case AT_SKILL_DEEPIMPACT:
				case AT_SKILL_ELFHARDEN:
				case AT_SKILL_PENETRATION:
					AttackDamage *= 2;
					break;
				case AT_SKILL_SPEAR:
				case AT_SKILL_FIREBURST:
				case AT_SKILL_DARKHORSE_ATTACK:
				case AT_SKILL_ELECTRICSPARK:
				case AT_SKILL_SPACE_SPLIT:
				case AT_SKILL_FIRESCREAM:
					AttackDamage = ( AttackDamage * ( ( ( lpObj->Energy + lpObj->AddEnergy ) / 20 + 200 ) ) ) / 100;
					break;

				case AT_SKILL_FENRIR_ATTACK:
					{
						int iDamageInc = lpObj->Level - 300;
						iDamageInc += lpObj->MasterLevel;

						if ( iDamageInc < 0 )
							iDamageInc = 0;

						iDamageInc /= 5;
						AttackDamage = ( AttackDamage * ( iDamageInc + 200 ) ) / 100;
					}
					break;
				case AT_SKILL_LIGHTEXPLODE:
					// rename to SplashDamage
					gObjUseSkill.SkillLightExplode(lpObj,lpTargetObj,216,AttackDamage,1,50);
					break;
				case AT_SKILL_DRAINLIFE:
					{
						int RestoreHP =0;

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							RestoreHP = (int)(float(lpObj->Energy / 15) + float(lpTargetObj->Level / 2.5f));
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							RestoreHP = AttackDamage * 10 / 100 + lpObj->Energy / 23;
						}
	
						int totalhp = (int)(lpObj->Life + RestoreHP);
	
						if( totalhp > lpObj->MaxLife + lpObj->AddLife )
						{
							lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
						}
						else	
						{
							lpObj->Life = (float)totalhp;
						}
	
						GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
					}	
					break;
				case AT_SKILL_CHAINLIGHTING:
					switch( bCombo )
					{
					case 1:
						break;
					case 2:
						AttackDamage = AttackDamage * 70 / 100;
						break;
					case 3:
						AttackDamage = AttackDamage * 50 / 100;
						break;
					default:
						return FALSE;
					}

					bCombo = FALSE;
					break;
				case AT_SKILL_SAHAMUT:
					{
						int Damage;
						if( lpObj->m_MPSkillOpt.MpsFireTomeMastery > 0 )
						{
							Damage = AttackDamage * (60.0f+lpObj->m_MPSkillOpt.MpsFireTomeMastery) / 100.0f;
						}
						else
						{
							Damage = AttackDamage * 60.0f / 100.0f;
						}

						gObjAddBuffEffect(lpTargetObj,BUFF_EXPLOSION,ADD_OPTION_REDUCE_HP,Damage,0,0,5);
					}
					break;
				case AT_SKILL_NEIL:
					{
						int Damage = AttackDamage * 60 / 100;
						gObjAddBuffEffect(lpTargetObj,BUFF_REQUIEM,ADD_OPTION_REDUCE_HP,Damage,0,0,5);
					}
					break;
				case AT_SKILL_SLEEP:
					{
						int Rate = 0;

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							Rate = 20 + lpObj->Energy / 30 + lpObj->m_CurseDamageBook / 6;
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							Rate = 15 + lpObj->Energy / 37 + lpObj->m_CurseDamageBook / 6;
						}

						if( Rate < rand()%100)
						{
							GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
							return FALSE;
						}

						int SleepTime = 0;

						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							SleepTime = (lpInfo->m_Arg4 + lpObj->Energy /lpInfo->m_Arg5) - lpTargetObj->Level / 20;

							if( SleepTime > lpInfo->m_Arg6 )
							{
								SleepTime = lpInfo->m_Arg6;
							}
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							SleepTime = lpInfo->m_Arg1 + ((lpObj->Energy /lpInfo->m_Arg2) + (lpObj->Level - lpTargetObj->Level) / 100);

							if( SleepTime > lpInfo->m_Arg3 )
							{
								SleepTime = lpInfo->m_Arg3;
							}
						}

						if( SleepTime < 1 )
						{
							GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
							return FALSE;
						}

						if( gObjCheckPowerfulEffect(lpTargetObj,0x48,SleepTime,0) == TRUE )
						{
							GCMagicAttackNumberSend(lpObj,219,lpTargetObj->m_Index,0);
							return FALSE;
						}

						gObjAddBuffEffect(lpTargetObj,BUFF_SLEEP,ADD_OPTION_SLEEPTIME,SleepTime,0,0,SleepTime);
						return TRUE;
					}
					break;
				case AT_SKILL_WEAKNESS:
					{
						int Rate = 0;

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							Rate = 32 + lpObj->Energy / 50 + lpObj->m_CurseDamageBook / 6;
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							Rate = 17 + lpObj->Energy / 50 + lpObj->m_CurseDamageBook / 6;
						}

						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						if( Rate >= rand() % 100 )
						{
							int DecValue = 0;
							int DecTime = 0;

							if( lpTargetObj->Type == OBJ_MONSTER )
							{
								int MaxEnergy = lpObj->Energy;

								if( lpObj->Energy > 4000 )
								{
									MaxEnergy = 4000;
								}

								DecValue = lpInfo->m_Arg7 + MaxEnergy / lpInfo->m_Arg8;
	
								if( DecValue > lpInfo->m_Arg9 )
								{
									DecValue = lpInfo->m_Arg9;
								}

								DecTime = lpInfo->m_Arg10 + MaxEnergy / lpInfo->m_Arg11 - lpTargetObj->Level / 20;

								if( DecTime > lpInfo->m_Arg12 )
								{
									DecTime = lpInfo->m_Arg12;
								}
							}
							else if( lpTargetObj->Type == OBJ_USER )
							{
								int MaxEnergy = lpObj->Energy;

								DecValue = lpInfo->m_Arg1 + MaxEnergy / lpInfo->m_Arg2;

								if( DecValue > lpInfo->m_Arg3 )
								{
									DecValue = lpInfo->m_Arg3;
								}

								DecTime = lpInfo->m_Arg4 + MaxEnergy / lpInfo->m_Arg5 + (lpObj->Level - lpTargetObj->Level) / 150;

								if( DecTime > lpInfo->m_Arg6 )
								{
									DecTime = lpInfo->m_Arg6;
								}
							}

							if( DecTime < 1 )
							{
								GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
								return FALSE;
							}

							if( gObjCheckPowerfulEffect(lpObj,AT_WEAKNESS,DecValue,0) == TRUE )
							{
								GCMagicAttackNumberSend(lpObj,skill,lpTargetObj->m_Index,0);
								return FALSE;
							}

							gObjAddBuffEffect(lpTargetObj,AT_WEAKNESS,ADD_OPTION_WEAKNESS,DecValue,0,0,DecTime);
							return TRUE;
						}
						else
						{
							GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
							return FALSE;
						}
					}
					break;
				case AT_SKILL_INNOVATION:
					{
						int Rate = 0;

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							Rate = 32 + lpObj->Energy / 50 + lpObj->m_CurseDamageBook / 6;
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							Rate = 17 + lpObj->Energy / 50 + lpObj->m_CurseDamageBook / 6;
						}

						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						if( Rate >= rand()%100 )
						{
							int DecValue = 0;
							int DecTime = 0;

							if( lpTargetObj->Type == OBJ_MONSTER )
							{
								int MaxEnergy = lpObj->Energy;

								DecValue = lpInfo->m_Arg7 + MaxEnergy / lpInfo->m_Arg8;

								if( DecValue > lpInfo->m_Arg9 )
								{
									DecValue = lpInfo->m_Arg9;
								}

								DecTime = lpInfo->m_Arg10 + MaxEnergy / lpInfo->m_Arg11 - lpTargetObj->Level / 20;

								if( DecTime > lpInfo->m_Arg12 )
								{
									DecTime = lpInfo->m_Arg12;
								}
							}
							else if( lpTargetObj->Type == OBJ_USER )
							{
								int MaxEnergy = lpObj->Energy;

								DecValue = lpInfo->m_Arg1 + MaxEnergy / lpInfo->m_Arg2;
	
								if( DecValue > lpInfo->m_Arg3 )
								{
									DecValue = lpInfo->m_Arg3;
								}

								DecTime = lpInfo->m_Arg4 + MaxEnergy / lpInfo->m_Arg5 + (lpObj->Level - lpTargetObj->Level) / 150;

								if( DecTime > lpInfo->m_Arg6 )
								{
									DecTime = lpInfo->m_Arg6;
								}
							}

							if( DecTime < 1 )
							{
								GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
								return FALSE;
							}

							if( gObjCheckPowerfulEffect(lpObj,AT_INNOVATION,DecValue,0) == TRUE )
							{
								GCMagicAttackNumberSend(lpObj,skill,lpTargetObj->m_Index,0);
								return FALSE;
							}

							gObjAddBuffEffect(lpTargetObj,AT_INNOVATION,ADD_OPTION_INNOVATION,DecValue,0,0,DecTime);
							return TRUE;
						}
						else
						{
							GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
							return FALSE;
						}
					}
					break;
				case AT_SKILL_LAGLE:

					if( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_ICE) == FALSE )
					{
						lpTargetObj->lpAttackObj = lpObj;
						lpTargetObj->DelayActionTime = 1000;
						lpTargetObj->DelayLevel = 1;

						gObjAddBuffEffect(lpTargetObj,BUFF_ICE,ADD_OPTION_REDUCEMOVEMENT,0,0,0,2);
					}
					break;
				case AT_SKILL_LIGHTSHOCK:
					gObjAddBuffEffect(lpTargetObj,BUFF_LIGHTNINGSHOCK,0,0,0,0,1);
					break;
				case AT_SKILL_DESTRUCT:
					lpTargetObj->lpAttackObj = lpObj;
					lpTargetObj->DelayActionTime = 1000;
					lpTargetObj->DelayLevel = 1;
						
					gObjAddBuffEffect(lpTargetObj,BUFF_COLD,ADD_OPTION_REDUCEMOVEMENT,0,0,0,10);

					AttackDamage = AttackDamage*(200 + Energy/10)/100;
					break;
				case AT_SKILL_FLAMESTRIKE:
					AttackDamage *= 2;
					gObjAddBuffEffect(lpTargetObj,BUFF_FLAMESTRIKE,0,0,0,0,1);
					break;
				case AT_SKILL_GIGANTICSTORM:
					gObjAddBuffEffect(lpTargetObj,BUFF_GIGANTSTORM,0,0,0,0,1);
					break;
				case AT_SKILL_CHAOTICDISEIER:
					AttackDamage = AttackDamage*(200 + Energy/25)/100;

					if( lpMagic->m_Skill == AT_SKILL_CHAOTICDISEIER &&
						lpObj->Class == 561 )
					{
						if( gObjCheckUsedBuffEffect(lpTargetObj,AT_ICE) )
						{
							lpObj->DelayActionTime = 800;
							lpObj->DelayLevel = 1;
							lpObj->lpAttackObj = lpObj;

							if( rand()%100 < 50 )
								gObjAddBuffEffect(lpTargetObj,AT_STUN,0,0,0,0,5);

							AttackDamage = 1000;
						}
					}
					break;
				case 250:
					AttackDamage *= 2;
					break;
				case 251:
					AttackDamage *= 2.2f;
					break;
				case 252:
					AttackDamage *= 2.3f;
					break;
				case 253:
					AttackDamage *= 2.5;
					break;
				case 239:
					gObjBackSpring(lpTargetObj,lpObj);
					break;
				case AT_SKILL_RAGEFIGHTER_WEAPON1:
				case AT_SKILL_RAGEFIGHTER_WEAPON2:
				case AT_SKILL_RAGEFIGHTER_CHAINDRIVE:
				case AT_SKILL_RAGEFIGHTER_SIEGE:
				case AT_SKILL_PHOENIXSHOT:
					{
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}
						
						AttackDamage = AttackDamage * (lpInfo->m_Arg1 + (Vitality / lpInfo->m_Arg2)) / 100.0f;
					}
					break;
				case AT_SKILL_RAGEFIGHTER_DARKSIDE:
					{
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						AttackDamage = (AttackDamage)*(lpInfo->m_Arg1 + (Dexterity/lpInfo->m_Arg2) + (Energy/lpInfo->m_Arg3))/100.0f;
					}
					break;
				case AT_SKILL_RAGEFIGHTER_DRAGONLORE:
					{
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}
						
						AttackDamage = (AttackDamage) * (lpInfo->m_Arg1 + (Energy/lpInfo->m_Arg2)) / 100.0f;
					}
					break;
				case AT_SKILL_RAGEFIGHTER_DRAGONSLAYER:
					{
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						if( lpTargetObj->Type == OBJ_USER )
						{
							AttackDamage = AttackDamage * (lpInfo->m_Arg1 + (Energy/lpInfo->m_Arg2)) / 100.0f;
						}
						else
						{
							AttackDamage = ((AttackDamage * (lpInfo->m_Arg3 + (Energy/lpInfo->m_Arg4)) / 100.0f) + 100.0f)*lpInfo->m_Arg5;
						}
					}
					break;
			}

			if( g_MasterSkillSystem.CheckMasterLevelSkill(skill) )
			{
				float iSkillValue = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);
				float iSkillBrandValue1 = g_MasterSkillSystem.GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,1);
				float iSkillBrandValue2 = g_MasterSkillSystem.GetBrandOfMasterSkillValue(lpObj,lpMagic->m_Skill,2);

				float fPrevValue = 0;

				int iMLSBase = g_MasterSkillSystem.GetBaseMasterLevelSkill(skill);

				switch( iMLSBase )
				{
				case AT_MSKILL_DW_EARTHPRISON:
				case AT_MSKILL_DW_EARTHPRISON1:
					if (rand() % 100 < 10)
					{
					  gObjAddBuffEffect(lpTargetObj, BUFF_EARTHBINDS, 0, 0, 0, 0, 5);
					  lpTargetObj->PathCount = 0;
					  lpTargetObj->PathStartEnd = 0;
					  gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
					}
					break;
				case AT_MSKILL_DK_TWISTINGSLASH1:
				case AT_MSKILL_DK_TWISTINGSLASH2:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100;
					break;
				case AT_MSKILL_MG_TWISTINGSLASH1:
					AttackDamage *= 2;
					break;
				case AT_MSKILL_DK_CYCLONE1:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100.0f;
					break;
				case AT_MSKILL_MG_CYCLONE1:
					AttackDamage *= 2;
					break;
				case AT_MSKILL_DK_SLASH1:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100.0f;
					break;
				case AT_MSKILL_DK_FALLINGSLASH1:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100.0f;
					break;
				case AT_MSKILL_DK_LUNGE1:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100.0f;
					break;
				case AT_MSKILL_DK_DESTRUCTION1:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100.0f;

					lpTargetObj->lpAttackObj = lpObj;
					lpTargetObj->DelayActionTime = 1000;
					lpTargetObj->DelayLevel = 1;
						
					gObjAddBuffEffect(lpTargetObj,BUFF_COLD,ADD_OPTION_REDUCEMOVEMENT,0,0,0,10);
					break;
				case AT_MSKILL_DK_DESTRUCTION2:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100.0f;

					lpTargetObj->lpAttackObj = lpObj;
					lpTargetObj->DelayActionTime = 1000;
					lpTargetObj->DelayLevel = 1;
						
					gObjAddBuffEffect(lpTargetObj,BUFF_COLD,ADD_OPTION_REDUCEMOVEMENT,0,0,0,10);

					if( iSkillValue > 0 )
					{
						float fPercent = rand()%100;

						if( fPercent < iSkillValue )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_STUN,0,0,0,0,3);
							gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
						}
					}
					break;
				case AT_MSKILL_DK_DESTRUCTION3:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100.0f;

					lpTargetObj->lpAttackObj = lpObj;
					lpTargetObj->DelayActionTime = 1000;
					lpTargetObj->DelayLevel = 1;
						
					gObjAddBuffEffect(lpTargetObj,BUFF_COLD,ADD_OPTION_REDUCEMOVEMENT,0,0,0,10);

					if( iSkillValue > 0.0f )
					{
						float fPercent = rand()%100;

						if( fPercent < iSkillValue )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_DECREASE_ATTACKSPEED,0,0,0,0,5);
						}
					}

					if( iSkillBrandValue1 > 0.0f )
					{
						float fPercent = rand()%100;

						if( fPercent < iSkillBrandValue1 )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_STUN,0,0,0,0,3);
							gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
						}
					}
					break;
				case AT_MSKILL_FE_PENETRATION1:
					AttackDamage *= 2;
					break;
				case AT_MSKILL_FE_ICEARROW1:
					AttackDamage *= 2;
					break;
				case AT_MSKILL_DL_EARTHSHAKE2:
					AttackDamage = ( AttackDamage * ( ( ( lpObj->Energy + lpObj->AddEnergy ) / 20 + 200 ) ) ) / 100;

					if( iSkillValue > 0 )
					{
						float fPercent = rand()%100;

						if( fPercent < iSkillValue )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_STUN,0,0,0,0,2);
							gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
						}
					}
					break;
				case AT_MSKILL_DL_FIREBURST2:
					AttackDamage = ( AttackDamage * ( ( ( lpObj->Energy + lpObj->AddEnergy ) / 20 + 200 ) ) ) / 100;

					if( iSkillValue > 0 )
					{
						float fPercent = rand()%100;

						if( fPercent < iSkillValue )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_STUN,0,0,0,0,2);
							gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
						}
					}
					break;
				case AT_MSKILL_DL_FORCEWAVE1:
					AttackDamage = ( AttackDamage * ( ( ( lpObj->Energy + lpObj->AddEnergy ) / 20 + 200 ) ) ) / 100;
					break;
				case AT_MSKILL_DL_EARTHSHAKE1:
					AttackDamage = ( AttackDamage * ( ( ( lpObj->Energy + lpObj->AddEnergy ) / 20 + 200 ) ) ) / 100;
					break;
				case AT_MSKILL_DL_FIREBURST1:
					AttackDamage = ( AttackDamage * ( ( ( lpObj->Energy + lpObj->AddEnergy ) / 20 + 200 ) ) ) / 100;
					break;
				case AT_MSKILL_DK_DEATHSTAB1:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100;
					break;
				case AT_MSKILL_DK_DEATHSTAB2:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100;
					if( iSkillValue > 0 )
					{
						float fPercent = rand()%100;

						if( fPercent < iSkillValue )
						{
							int Damage = Strength/10;
							lpTargetObj->lpAttackObj = lpObj;
							gObjAddBuffEffect(lpTargetObj,BUFF_BLEEDING,ADD_OPTION_REDUCE_HP,Damage,0,0,10);
						}
					}
					break;
				case AT_MSKILL_DK_DEATHSTAB3:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100;
					if( iSkillValue > 0.0f )
					{
						float fPercent = rand()%100;

						if( fPercent < iSkillValue )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_STUN,0,0,0,0,2);
							gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
						}
					}

					if( iSkillBrandValue1 > 0.0f )
					{
						float fPercent = rand()%100;

						if( fPercent < iSkillBrandValue1 )
						{
							int Damage = Strength/10;
							lpTargetObj->lpAttackObj = lpObj;
							gObjAddBuffEffect(lpTargetObj,BUFF_BLEEDING,ADD_OPTION_REDUCE_HP,Damage,0,0,10);
						}
					}
					break;
				case AT_MSKILL_DK_RAGEFULBLOW1:
				case AT_MSKILL_DK_RAGEFULBLOW2:
					AttackDamage = ( AttackDamage * ( 200 + ( Energy / 10 ) ) ) / 100;
					break;
				case AT_MSKILL_MG_FIRESLASH1:
				case AT_MSKILL_MG_FIRESLASH2:
				case AT_MSKILL_MG_FLAMESTRIKE1:
				case AT_MSKILL_MG_FLAMESTRIKE2:
					AttackDamage *= 2;
					break;
				case AT_MSKILL_MG_POWERSLASH1:
					AttackDamage *= 2;
					break;
				case AT_MSKILL_DL_FIRESCREAM1:
				case AT_MSKILL_DL_FIRESCREAM2:
					AttackDamage = ( AttackDamage * ( ( ( lpObj->Energy + lpObj->AddEnergy ) / 20 + 200 ) ) ) / 100;
					break;
				case AT_MSKILL_SUM_SLEEP1:
					{
						int skillSuccessRate = 0;

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							skillSuccessRate = 20 + lpObj->Energy/30 + lpObj->m_CurseDamageBook/6;
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							skillSuccessRate = 15 + lpObj->Energy/37 + lpObj->m_CurseDamageBook/6;
						}

						skillSuccessRate += iSkillValue;

						if( skillSuccessRate < rand()%100 )
						{
							GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
							return 0;
						}

						int nEffectTime = 0;
						
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							nEffectTime = (lpInfo->m_Arg4 + lpObj->Energy /lpInfo->m_Arg5) - lpTargetObj->Level / 20;

							if( nEffectTime > lpInfo->m_Arg6 )
							{
								nEffectTime = lpInfo->m_Arg6;
							}
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							nEffectTime = lpInfo->m_Arg1 + ((lpObj->Energy /lpInfo->m_Arg2) + (lpObj->Level - lpTargetObj->Level) / 100);

							if( nEffectTime > lpInfo->m_Arg3 )
							{
								nEffectTime = lpInfo->m_Arg3;
							}
						}

						if( nEffectTime < 1 )
						{
							GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
							return FALSE;
						}

						if( gObjCheckPowerfulEffect(lpTargetObj,BUFF_SLEEP,nEffectTime,0) == TRUE )
						{
							GCMagicAttackNumberSend(lpObj,AT_SKILL_SLEEP,lpTargetObj->m_Index,0);
							return FALSE;
						}

						gObjAddBuffEffect(lpTargetObj,BUFF_SLEEP,ADD_OPTION_SLEEPTIME,nEffectTime,0,0,nEffectTime);
						return TRUE;
					}
					break;
				case AT_MSKILL_SUM_CHAINLIGHTNING1:
					switch( bCombo )
					{
					case 1:
						break;
					case 2:
						AttackDamage = AttackDamage * 70 / 100;
						break;
					case 3:
						AttackDamage = AttackDamage * 50 / 100;
						break;
					default:
						return FALSE;
					}

					bCombo = FALSE;
					break;
				case AT_MSKILL_SUM_LIGHTNSHOCK1:
					gObjAddBuffEffect(lpTargetObj,BUFF_LIGHTNINGSHOCK,0,0,0,0,1);
					break;
				case AT_MSKILL_SUM_DRAINLIFE1:
					{
						int nAddHP = 0;

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							nAddHP = (int)(float(lpObj->Energy / 15) + float(lpTargetObj->Level / 2.5f));
							nAddHP += iSkillValue;
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							nAddHP = AttackDamage * 10 / 100 + lpObj->Energy / 23;
							nAddHP += iSkillValue;
						}
	
						int tmpLife = (int)(lpObj->Life + nAddHP);
	
						if( tmpLife > lpObj->MaxLife + lpObj->AddLife )
						{
							lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
						}
						else	
						{
							lpObj->Life = (float)tmpLife;
						}
	
						GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
					}
					break;
				case AT_MSKILL_SUM_DRAINLIFE2:
					{
						int nAddHP = 0;

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							nAddHP = (int)(float(lpObj->Energy / 15) + float(lpTargetObj->Level / 2.5f));
							nAddHP += iSkillBrandValue1;
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							nAddHP = AttackDamage * 10 / 100 + lpObj->Energy / 23;
							nAddHP += iSkillBrandValue1;
						}
	
						int tmpLife = (int)(lpObj->Life + nAddHP);
	
						if( tmpLife > lpObj->MaxLife + lpObj->AddLife )
						{
							lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
						}
						else	
						{
							lpObj->Life = (float)tmpLife;
						}
	
						GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);

						if( iSkillValue > 0.0f )
						{
							float fpercent = rand()%100;

							if( fpercent < 30 )
							{
								lpTargetObj->lpAttackObj = lpObj;
								gObjAddBuffEffect(lpTargetObj,BUFF_VAMPIRIC,BUFF_ABSORB_EFFECT,iSkillValue,0,0,5);
							}
						}
					}
					break;
				case AT_MSKILL_RF_KILLBLOW1:
				case AT_MSKILL_RF_KILLBLOW2:
				case AT_MSKILL_RF_UPERCUT1:
				case AT_MSKILL_RF_UPERCUT2:
				case AT_MSKILL_RF_CHAINDRIVE1:
				case AT_MSKILL_RF_CHAINDRIVE2:
					{
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}
						
						AttackDamage = AttackDamage * (lpInfo->m_Arg1 + (Vitality / lpInfo->m_Arg2)) / 100.0f;
					}
					break;
				case AT_MSKILL_RF_DARKSIDE1:
				case AT_MSKILL_RF_DARKSIDE2:
					{
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}
						
						AttackDamage = (AttackDamage)*(lpInfo->m_Arg1 + (Dexterity/lpInfo->m_Arg2) + (Energy/lpInfo->m_Arg3))/100.0f;
					}
					break;
				case AT_MSKILL_RF_DRAGONRORE1:
				case AT_MSKILL_RF_DRAGONRORE2:
					{
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						AttackDamage = (AttackDamage)*(lpInfo->m_Arg1 + (Energy/lpInfo->m_Arg2))/100.0f;
					}
					break;
				case AT_MSKILL_RF_DRAGONSLASH1:
				case AT_MSKILL_RF_DRAGONSLASH2:
					{
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						if( lpTargetObj->Type == OBJ_USER )
						{
							AttackDamage = AttackDamage * (lpInfo->m_Arg1 + (Energy/lpInfo->m_Arg2)) / 100.0f;
						}
						else
						{
							AttackDamage = ((AttackDamage * (lpInfo->m_Arg3 + (Energy/lpInfo->m_Arg4)) / 100.0f) + 100.0f)*lpInfo->m_Arg5;
						}
					}
					break;
				case AT_MSKILL_DK_BLOODSTORM:
				case AT_MSKILL_DK_BLOODSTORM1:
					{
						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						AttackDamage = ( AttackDamage * ( lpInfo->m_Arg1 + ( Energy / lpInfo->m_Arg2 ) ) ) / 100.0f;
					}
					break;
				case AT_MSKILL_DW_METEORIT2:
					if( iSkillValue > 0.0f )
					{
						float fPercent = rand()%100;

						if( fPercent < iSkillValue )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_STUN,0,0,0,0,2);
							gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
						}
					}
					break;
				case AT_MSKILL_DW_ICESTORM2:
					if( iSkillValue > 0.0f )
					{
						float fPercent = rand()%100;
						
						if( fPercent < iSkillValue )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_FREEZE,0,0,0,0,3);
							gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
						}
					}
					break;
				case AT_MSKILL_FE_POISONARROW:
				case AT_MSKILL_FE_POISONARROW1:
					AttackDamage *= 2;
					break;
				case AT_MSKILL_MG_ICE2:
					{
						float fPercent = rand()%100;
	
						if( fPercent < iSkillValue )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_STUN,0,0,0,0,3);
							gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
						}
					}
					break;
					case AT_MSKILL_SUM_INNOVATION1:
					{
						int Rate = 0;

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							Rate = 32 + lpObj->Energy / 50 + lpObj->m_CurseDamageBook / 6;
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							Rate = 17 + lpObj->Energy / 50 + lpObj->m_CurseDamageBook / 6;
						}

						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						if( Rate >= rand()%100 )
						{
							int DecValue = 0;
							int DecTime = 0;

							if( lpTargetObj->Type == OBJ_MONSTER )
							{
								int MaxEnergy = lpObj->Energy;

								DecValue = lpInfo->m_Arg7 + MaxEnergy / lpInfo->m_Arg8 + iSkillValue;

								if( DecValue > lpInfo->m_Arg9 )
								{
									DecValue = lpInfo->m_Arg9;
								}

								DecTime = lpInfo->m_Arg10 + MaxEnergy / lpInfo->m_Arg11 - lpTargetObj->Level / 20 + iSkillValue;

								if( DecTime > lpInfo->m_Arg12 )
								{
									DecTime = lpInfo->m_Arg12;
								}
							}
							else if( lpTargetObj->Type == OBJ_USER )
							{
								int MaxEnergy = lpObj->Energy;

								DecValue = lpInfo->m_Arg1 + MaxEnergy / lpInfo->m_Arg2 + iSkillValue;

								if( DecValue > lpInfo->m_Arg3 )
								{
									DecValue = lpInfo->m_Arg3;
								}

								DecTime = lpInfo->m_Arg4 + MaxEnergy / lpInfo->m_Arg5 + (lpObj->Level - lpTargetObj->Level) / 150 + iSkillValue;

								if( DecTime > lpInfo->m_Arg6 )
								{
									DecTime = lpInfo->m_Arg6;
								}
							}

							if( DecTime < 1 )
							{
								GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
								return FALSE;
							}

							if( gObjCheckPowerfulEffect(lpObj,AT_INNOVATION,DecValue,0) == TRUE )
							{
								GCMagicAttackNumberSend(lpObj,skill,lpTargetObj->m_Index,0);
								return FALSE;
							}

							gObjAddBuffEffect(lpTargetObj,AT_INNOVATION,ADD_OPTION_INNOVATION,DecValue,0,0,DecTime);
							return TRUE;
						}
						else
						{
							GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
							return FALSE;
						}
					}
					break;
				case AT_MSKILL_SUM_WEAKNESS1:
					{
						int Rate = 0;

						if( lpTargetObj->Type == OBJ_MONSTER )
						{
							Rate = 32 + lpObj->Energy / 50 + lpObj->m_CurseDamageBook / 6;
						}
						else if( lpTargetObj->Type == OBJ_USER )
						{
							Rate = 17 + lpObj->Energy / 50 + lpObj->m_CurseDamageBook / 6;
						}

						BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

						if( !lpInfo )
						{
							return 0;
						}

						if( Rate >= rand() % 100 )
						{
							int DecValue = 0;
							int DecTime = 0;

							if( lpTargetObj->Type == OBJ_MONSTER )
							{
								int MaxEnergy = lpObj->Energy;

								DecValue = lpInfo->m_Arg7 + MaxEnergy / lpInfo->m_Arg8 + iSkillValue;

								if( DecValue > lpInfo->m_Arg9 )
								{
									DecValue = lpInfo->m_Arg9;
								}

								DecTime = lpInfo->m_Arg10 + MaxEnergy / lpInfo->m_Arg11 - lpTargetObj->Level / 20 + iSkillValue;

								if( DecTime > lpInfo->m_Arg12 )
								{
									DecTime = lpInfo->m_Arg12;
								}
							}
							else if( lpTargetObj->Type == OBJ_USER )
							{
								int MaxEnergy = lpObj->Energy;

								DecValue = lpInfo->m_Arg1 + MaxEnergy / lpInfo->m_Arg2 + iSkillValue;

								if( DecValue > lpInfo->m_Arg3 )
								{
									DecValue = lpInfo->m_Arg3;
								}

								DecTime = lpInfo->m_Arg4 + MaxEnergy / lpInfo->m_Arg5 + (lpObj->Level - lpTargetObj->Level) / 150 + iSkillValue;

								if( DecTime > lpInfo->m_Arg6 )
								{
									DecTime = lpInfo->m_Arg6;
								}
							}

							if( DecTime < 1 )
							{
								GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
								return FALSE;
							}

							if( gObjCheckPowerfulEffect(lpObj,AT_WEAKNESS,DecValue,0) == TRUE )
							{
								GCMagicAttackNumberSend(lpObj,skill,lpTargetObj->m_Index,0);
								return FALSE;
							}

							gObjAddBuffEffect(lpTargetObj,AT_WEAKNESS,ADD_OPTION_WEAKNESS,DecValue,0,0,DecTime);
							return TRUE;
						}
						else
						{
							GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
							return FALSE;
						}
					}
					break;
				case AT_MSKILL_DL_CHAOTICDESEAR1:
					AttackDamage = AttackDamage*(200 + Energy/25)/100;
					break;
				}
			}

			if ( lpObj->pInventory[1].GetDetailItemType() == 10 &&
				 lpObj->pInventory[1].m_Special[0] == skill )
			{
				if( MagicDamageC.GetSkillAttr(skill) == R_FIRE )
				{
					if( lpObj->m_MPSkillOpt.MpsFireTomeStrength > 0.0f )
					{
						AttackDamage += lpObj->m_MPSkillOpt.MpsFireTomeStrength;
					}
				}
				else if( MagicDamageC.GetSkillAttr(skill) == R_WIND )
				{
					if( lpObj->m_MPSkillOpt.MpsWindTomeStrength > 0.0f )
					{
						AttackDamage += lpObj->m_MPSkillOpt.MpsWindTomeStrength;
					}

					if( lpObj->m_MPSkillOpt.MpsWindTomeMastery > 0.0f )
					{
						float fPercent = rand()%100;

						if( fPercent < lpObj->m_MPSkillOpt.MpsWindTomeMastery )
						{
							gObjAddBuffEffect(lpTargetObj,BUFF_STUN,0,0,0,0,3);
							gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
						}
					}
				}
				else if( MagicDamageC.GetSkillAttr(skill) == R_LIGHTNING )
				{
					if( lpObj->m_MPSkillOpt.MpsLightTomeStrength > 0.0f )
					{
						AttackDamage += lpObj->m_MPSkillOpt.MpsLightTomeStrength;
					}

					if( lpObj->m_MPSkillOpt.MpsLightTomeMastery > 0.0f )
					{
						float fPercent = rand()%100;

						if( fPercent < lpObj->m_MPSkillOpt.MpsLightTomeMastery )
						{
							gObjBackSpring2(lpTargetObj,lpObj,2);
						}
					}
				}
			}

			if ( skill == 0 )
			{
				if ( lpObj->pInventory[8].m_Type == ITEMGET(13, 3) )
				{
					AttackDamage = AttackDamage * 130 / 100;
				}
			}

			int iManaShieldType = 0;

			if( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_MANASHIELD) == TRUE )
				iManaShieldType = 1;
			else if( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_MANASHIELD2) == TRUE )
				iManaShieldType = 2;
			
			if ( iManaShieldType != 0 && AttackDamage > 0 )
			{
				int iWizardSkillDefense = 0;
				int iManaRate = 0;

				if( iManaShieldType == 1 )
					gObjGetValueOfBuffIndex(lpTargetObj,BUFF_MANASHIELD,&iWizardSkillDefense,&iManaRate);
				else
					gObjGetValueOfBuffIndex(lpTargetObj,BUFF_MANASHIELD2,&iWizardSkillDefense,&iManaRate);

				int iReplacementMana = 0;

				if( iManaRate > 0 )
				{
					iReplacementMana = (lpTargetObj->Mana * iManaRate) / 1000;
				}
				else
				{
					iReplacementMana = (lpTargetObj->Mana * 0.02f);
				}

				if ( iReplacementMana < lpTargetObj->Mana )
				{
					lpTargetObj->Mana -= iReplacementMana;
					float fWizardSkillDefense = iWizardSkillDefense/100.0f;
					int decattackdamage = 0;
					
					if( iWizardSkillDefense > 0 )
					{
						decattackdamage = AttackDamage * fWizardSkillDefense;
					}
					AttackDamage -= decattackdamage;
					ManaChange = TRUE;
				}
			}

			AttackDamage += lpObj->SetOpAddDamage;

			if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
			{
				AttackDamage += lpObj->m_ItemOptionExFor380.OpAddDamage;

				if ( CC_MAP_RANGE(lpObj->MapNumber ) && CC_MAP_RANGE(lpTargetObj->MapNumber) )
				{
					AttackDamage = AttackDamage * 50 / 100;
				}
#if (ENABLE_CUSTOM_PVPBALANCE == 1)
				g_PvPBalance.modifyDamage(lpObj->m_Index, lpTargetObj->m_Index, AttackDamage);
#endif

#if (ENABLETEST_ZTLUA == 1)
			ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
			ZTLuaUnit::Bind(LuaMng->getLua());
			ZTLuaItem::Bind(LuaMng->getLua());
			LuaMng->Call("correctDamagePvP", lpObj->m_Index, lpTargetObj->m_Index, AttackDamage);
			AttackDamage = LuaMng->getRef()->cast<int>();
			ZTLuaManager::Delete(LuaMng);
			LogAddDebug("After: %d", AttackDamage);
			LogAddDebug("Diff: %d", AttackDamage - Before);
#endif
			}

#if (ENABLE_SWITCH_CS_DAMAGEDEC == 1)
			if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE
				&& lpObj->Type == OBJ_USER
				&& lpTargetObj->Type == OBJ_USER
				&& lpObj->MapNumber == MAP_INDEX_CASTLESIEGE
				&& lpTargetObj->MapNumber == MAP_INDEX_CASTLESIEGE )
			{
				if(lpObj->m_btCsJoinSide == lpTargetObj->m_btCsJoinSide)
				{
					AttackDamage = AttackDamage * 20 / 100;
				}
				else
				{
					AttackDamage = AttackDamage * 40 / 100;
				}
			}
#endif
			if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_MONSTER )
			{
				if ( lpTargetObj->Class == 283 )
				{
					if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_BLESS) == TRUE )
					{
						//AttackDamage = AttackDamage;
					}
					else if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_SOUL) == TRUE )
					{
						//AttackDamage = AttackDamage;
					}
					else 
					{
						if ( lpObj->m_iAccumulatedDamage > 100 )
						{
							gObjWeaponDurDownInCastle(lpObj, lpTargetObj, 1);
							lpObj->m_iAccumulatedDamage = 0;
						}
						else
						{
							lpObj->m_iAccumulatedDamage += AttackDamage;
						}

						AttackDamage = AttackDamage * 5 / 100;
					}
				}

				if ( lpTargetObj->Class == 277 )
				{
					if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_BLESS) == TRUE )
					{
						//AttackDamage = AttackDamage;
					}
					else if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_SOUL) == TRUE )
					{
						//AttackDamage = AttackDamage;
					}
					else 
					{
						if ( lpObj->m_iAccumulatedDamage > 100 )
						{
							gObjWeaponDurDownInCastle(lpObj, lpTargetObj, 1);
							lpObj->m_iAccumulatedDamage = 0;
						}
						else
						{
							lpObj->m_iAccumulatedDamage += AttackDamage;
						}

						AttackDamage = AttackDamage * 5 / 100;
					}
				}
			}

			if( gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW2) == TRUE )
				AttackDamage += AttackDamage/100.0f * lpObj->m_InfinityArrowDmgMul;

			if ( gObjFenrir( lpObj ) )
			{
				int iIncPercent = lpObj->pInventory[8].IsFenrirIncLastAttackDamage();

				if ( iIncPercent > 0 )
				{
					AttackDamage += AttackDamage * iIncPercent / 100;
				}
			}

			if ( gObjFenrir( lpTargetObj ) )
			{
				int iDecPercent = lpTargetObj->pInventory[8].IsFenrirDecLastAttackDamage();

				if ( iDecPercent > 0 )
				{
					AttackDamage -= AttackDamage * iDecPercent / 100;
				}
			}
#if (ENABLETEST_MUUN == 1)
			int MuunDecrease = 0;
			if (g_CMuunSystem.GetMuunItemValueOfOptType(lpTargetObj, 6, &MuunDecrease, 0)) {
				AttackDamage -= MuunDecrease;
			}
#endif
			if ( AttackDamage < 0 )
				AttackDamage = 0;

			if ( skill == 76 )
			{
				if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
				{
					if ( AttackDamage > 0 )
					{
#if (GS_CASTLE == 1)
						if(lpObj->m_btCsJoinSide == 0 || lpObj->m_btCsJoinSide != lpTargetObj->m_btCsJoinSide)
						{
#endif
						int iEquipmentPos = rand()%5 + 2;	// Select and Armor
						CItem * lpEquipment = &lpTargetObj->pInventory[iEquipmentPos];

						if ( lpEquipment && lpEquipment->IsItem() && !lpEquipment->m_bLuckySet)
						{
							int ItemPerc = 50;

							if( lpObj->m_MPSkillOpt.MpsDownDur1 != 0.0f )
							{
								ItemPerc += lpObj->m_MPSkillOpt.btMpsDownDur1Level*7;
							}

							if( ItemPerc < 0 )
								ItemPerc = 1;

							int iDurEquipment = (int)(lpEquipment->m_Durability * ItemPerc / 100.0f);

							for(int iBuffIndex = 0; iBuffIndex < lpTargetObj->btEffectCount; iBuffIndex++)
							{
								if( lpObj->pEffectInfo[iBuffIndex].btEffectNum == BUFF_TALISMAN_ITEMPROTECT )
								{
									iDurEquipment = lpEquipment->m_Durability;
									break;
								}
							}

							// need to rewrite lucky system
							if( lpEquipment->m_bLuckySet )
							{
								iDurEquipment = lpEquipment->m_Durability;
							}

							lpEquipment->m_Durability = (float)iDurEquipment;

							if ( lpEquipment->m_Durability < 0.0f )
							{
								lpEquipment->m_Durability = 0.0f;
							}

							GCItemDurSend(lpTargetObj->m_Index, iEquipmentPos, (BYTE)lpEquipment->m_Durability, 0);
						}
#if (GS_CASTLE == 1)
						}
#endif
					}
				}
			}

			if ( !byReflect )
			{
				if( lpObj->m_MPSkillOpt.MpsTwoHandSwordMastery > 0.0f &&
					lpObj->Type == OBJ_USER &&
					lpTargetObj->Type == OBJ_USER )
				{
					AttackDamage += lpObj->m_MPSkillOpt.MpsTwoHandSwordMastery;
				}

				if( lpObj->m_MPSkillOpt.MpsTwoHandStaffMastery > 0.0f &&
					lpObj->Type == OBJ_USER &&
					lpTargetObj->Type == OBJ_USER )
				{
					AttackDamage += lpObj->m_MPSkillOpt.MpsTwoHandStaffMastery;
				}

				if( lpObj->m_MPSkillOpt.MpsCrossbowMastery > 0.0f &&
					lpObj->Type == OBJ_USER &&
					lpTargetObj->Type == OBJ_USER )
				{
					AttackDamage += lpObj->m_MPSkillOpt.MpsCrossbowMastery;
				}

				if( lpObj->m_MPSkillOpt.MpsStickMastery > 0.0f &&
					lpObj->Type == OBJ_USER &&
					lpTargetObj->Type == OBJ_USER )
				{
					AttackDamage += lpObj->m_MPSkillOpt.MpsStickMastery;
				}

				if( lpObj->m_MPSkillOpt.MpsScepterMastery > 0.0f &&
					lpObj->Type == OBJ_USER &&
					lpTargetObj->Type == OBJ_USER )
				{
					AttackDamage += lpObj->m_MPSkillOpt.MpsScepterMastery;
				}
			}

			if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
			{
				if( skill == AT_SKILL_RAGEFIGHTER_DRAGONSLAYER && 
					rand()%100 < (10+Energy/100) )
				{
					bDragonKickSDAttackSuccess = TRUE;
					int nDecreaseSDRate = 10 + Energy/30;

					if( nDecreaseSDRate > 100 )
						nDecreaseSDRate = 100;

					int nDecreaseSD = lpTargetObj->iShield*nDecreaseSDRate/100;
					lpTargetObj->iShield -= nDecreaseSD;
					lpTargetObj->Life -= AttackDamage;
					iTotalShieldDamage += nDecreaseSD;
				}
				else
				{
					iTempShieldDamage = this->GetShieldDamage(lpObj, lpTargetObj, AttackDamage);
					lpTargetObj->iShield -= iTempShieldDamage;
					lpTargetObj->Life -= AttackDamage - iTempShieldDamage;
					iTotalShieldDamage += iTempShieldDamage;
				}

				if ( lpTargetObj->Life < 0.0f )
				{
					lpTargetObj->Life = 0.0f;
				}
			}
			else
			{
				lpTargetObj->Life -= AttackDamage;

				if ( lpTargetObj->Life < 0.0f )
				{
					lpTargetObj->Life = 0.0f;
				}
			}
		}
	}	
	else
	{
		if( skill == AT_SKILL_LIGHTEXPLODE )
		{
			if( g_ShieldSystemOn ==  TRUE )
			{
				if( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
				{
					if( MissCheckPvP(lpObj,lpTargetObj,skill,skillSuccess,magicsend,bAllMiss, byBarrageCount ) == FALSE )
					{
						return FALSE;
					}
				}
				else
				{
					if( MissCheck(lpObj,lpTargetObj,skill,skillSuccess,magicsend,bAllMiss, byBarrageCount) == FALSE )
					{
						return FALSE;
					}
				}
			}
			else
			{
				if( MissCheck(lpObj,lpTargetObj,skill,skillSuccess,magicsend,bAllMiss, byBarrageCount) == FALSE )
				{
					return FALSE;
				}
			}
		}


#if(GS_CASTLE == 1)
		if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE
			&& lpObj->Type == OBJ_USER
			&& lpTargetObj->Type == OBJ_USER
			&& lpObj->MapNumber == MAP_INDEX_CASTLESIEGE
			&& lpTargetObj->MapNumber == MAP_INDEX_CASTLESIEGE )
		{
			if(lpObj->m_btCsJoinSide == lpTargetObj->m_btCsJoinSide)
			{
				AttackDamage = AttackDamage * 20 / 100;
			}
			else if(g_ShieldSystemOn == FALSE)
			{
				AttackDamage = AttackDamage * 40 / 100;
			}
		}
#endif

		if ( skill != AT_SKILL_EXPLOSION && skill != AT_SKILL_LIGHTEXPLODE )
		{
			bDamageReflect = TRUE;
			MsgDamage = 4;
		}
#if(__NOVUS__==1)
		if( lpObj->Type == OBJ_MONSTER && lpTargetObj->Type == OBJ_USER )
		{
			if( g_SoulSystem.IsMap(lpTargetObj->MapNumber) && !g_SoulSystem.WithSoul(lpTargetObj) )
			{
				AttackDamage = lpTargetObj->MaxLife + lpTargetObj->AddLife + 1000;
				LogAddC(2, "[DEBUG] [SoulSystem] [%s] Defense has been ignored", lpTargetObj->Name);
			}
		}
#endif
		if( gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW2) == TRUE )
			AttackDamage += AttackDamage/100.0f * lpObj->m_InfinityArrowDmgMul;

		if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
		{
			if( skill == AT_SKILL_RAGEFIGHTER_DRAGONSLAYER && 
					rand()%100 < (10+Energy/100) )
			{
				bDragonKickSDAttackSuccess = TRUE;
				int nDecreaseSDRate = 10 + Energy/30;

				if( nDecreaseSDRate > 100 )
					nDecreaseSDRate = 100;

				int nDecreaseSD = lpTargetObj->iShield*nDecreaseSDRate/100;
				lpTargetObj->iShield -= nDecreaseSD;
				lpTargetObj->Life -= AttackDamage;
				iTotalShieldDamage += nDecreaseSD;
			}
			else
			{
				iTempShieldDamage = this->GetShieldDamage(lpObj, lpTargetObj, AttackDamage);
				lpTargetObj->iShield -= iTempShieldDamage;
				lpTargetObj->Life -= AttackDamage - iTempShieldDamage;
				iTotalShieldDamage += iTempShieldDamage;
			}

			if ( lpTargetObj->Life < 0.0f )
			{
				lpTargetObj->Life = 0.0f;
			}
		}
		else
		{
			lpTargetObj->Life -= AttackDamage;

			if ( lpTargetObj->Life < 0.0f )
			{
				lpTargetObj->Life = 0.0f;
			}
		}
	}

	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
		gObjAddMsgSendDelay(lpTargetObj, 0, lpObj->m_Index, 100, 0);
		lpTargetObj->LastAttackerID = lpObj->m_Index;

		if ( lpTargetObj->m_iCurrentAI )
		{
			lpTargetObj->m_Agro.IncAgro(lpObj->m_Index, AttackDamage / 50);
		}
	}

	BOOL selfdefense = 0;
	lpCallObj = lpTargetObj;
	
	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
		if ( lpTargetObj->m_RecallMon >= 0 )
		{
			lpCallObj = &gObj[lpTargetObj->m_RecallMon];
		}
	}



	if ( AttackDamage >= 1 )
	{
		if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
		{
			if ( gObjDuelCheck(lpObj, lpTargetObj) )
			{
				selfdefense = 0;
			}
#if (ENABLETEST_NEWPVP == 1)
			else if (g_NewPVP.IsVulcanusMap(lpObj->MapNumber) || g_NewPVP.IsVulcanusMap(lpTargetObj->MapNumber)) {
				selfdefense = 0;
			}
#endif
			else if ( CC_MAP_RANGE(lpObj->MapNumber) || CC_MAP_RANGE(lpTargetObj->MapNumber) )
			{
				selfdefense = 0;
			}
			else if ( IT_MAP_RANGE(lpObj->MapNumber) || IT_MAP_RANGE(lpTargetObj->MapNumber) )
			{
				selfdefense = 0;
			}
			else if ( lpObj->MapNumber == MAP_INDEX_VULCANUS || lpTargetObj->MapNumber == MAP_INDEX_VULCANUS )
			{
				selfdefense = 0;
			}
#if (__GENS_SYSTEM__ == 1)
			else if ( gGensSystem.GensMap(lpObj->MapNumber) )
			{
				selfdefense = 0;
			}
#endif
			else
			{
				selfdefense = 1;
			}

			if ( gObjGetRelationShip(lpObj, lpTargetObj) == 2 )
			{
				selfdefense = FALSE;
			}
#if (GS_CASTLE == 1)			
			if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE && lpObj->m_btCsJoinSide > 0)
			{
				selfdefense = FALSE;
			}
#endif
		}
		else if ( lpTargetObj->Type == OBJ_MONSTER && lpObj->Type == OBJ_USER )
		{
			if ( lpTargetObj->m_RecallMon >= 0 )
			{
				selfdefense = TRUE;
			}
		}

		if ( lpTargetObj->Type == OBJ_USER )
		{
			gObjArmorRandomDurDown(lpTargetObj, lpObj);

			if( skill == AT_MSKILL_DK_RAGEFULBLOW2 )
			{
				float fDurDownRate = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

				if( rand()%100 < fDurDownRate )
				{
#if (GS_CASTLE == 1)
					if(lpObj->m_btCsJoinSide == 0 || lpObj->m_btCsJoinSide != lpTargetObj->m_btCsJoinSide)
					{
#endif
						int item_num[5];
						item_num[0] = 2;
						item_num[1] = 3;
						item_num[2] = 4;
						item_num[3] = 5;
						item_num[4] = 6;
						int iEquipmentPos = rand()%5;	// Select and Armor
						CItem * lpEquipment = &lpTargetObj->pInventory[item_num[iEquipmentPos]];

						if ( lpEquipment && lpEquipment->IsItem() )
						{
							float iDurDecValue = 0;

							if( lpObj->m_MPSkillOpt.MpsDownDur1 != 0.0f )
							{
								iDurDecValue += lpObj->m_MPSkillOpt.btMpsDownDur1Level/5.0f;
							}

							if( iDurDecValue == 0.0f )
								iDurDecValue = 1.0f;

							int damagemin = lpEquipment->m_BaseDurability/100.0f*(10-iDurDecValue);

							int iDurEquipment = lpEquipment->m_Durability - damagemin;

							for(int iBuffIndex = 0; iBuffIndex < lpObj->btEffectCount; iBuffIndex++)
							{
								if( lpObj->pEffectInfo[iBuffIndex].btEffectNum == BUFF_TALISMAN_ITEMPROTECT )
								{
									iDurEquipment = lpEquipment->m_Durability;
									break;
								}
							}

							if( lpEquipment->m_bLuckySet )
							{
								iDurEquipment = lpEquipment->m_Durability;
							}

							lpEquipment->m_Durability = (float)iDurEquipment;

							if ( lpEquipment->m_Durability < 0.0f )
							{
								lpEquipment->m_Durability = 0.0f;
							}

							GCItemDurSend(lpTargetObj->m_Index, iEquipmentPos, (BYTE)lpEquipment->m_Durability, 0);
						}
#if (GS_CASTLE == 1)
					}
#endif
				}
			}
		}

		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_ICEARROW) == TRUE )
		{
			gObjAddBuffEffect(lpTargetObj,BUFF_ICEARROW,-1);

			if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_ICEARROW) == FALSE )
			{
				GCMagicCancelSend(lpTargetObj,51);
			}
		}

		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_SLEEP) == TRUE )
		{
			gObjRemoveBuffEffect(lpTargetObj,BUFF_SLEEP);
		}
	}

	if ( lpObj->PartyNumber >= 0 &&
		 lpCallObj->PartyNumber >= 0)
	{
		if( lpObj->PartyNumber == lpCallObj->PartyNumber )
		{
			selfdefense = 0;
		}
	}

	if ( selfdefense == TRUE && bDamageReflect == FALSE )
	{
		if ( !gObjTargetGuildWarCheck(lpObj, lpCallObj) )
		{
			if( lpCallObj->PartyNumber >= 0 )
			{
				int bTmp = 0;
				int PartyNumber = lpCallObj->PartyNumber;

				if( gParty.GetPartyPkLevel(PartyNumber) < 5 )
				{
					gObjCheckSelfDefense(lpObj, lpCallObj->m_Index);
				}
			}
			else
			{
				gObjCheckSelfDefense(lpObj, lpCallObj->m_Index);
			}
		}
	}

	if ( lpTargetObj->Class == 275 )	// KUNDUN
	{
		if ( lpTargetObj->m_iMonsterBattleDelay <= 0 )
		{
			if ( (rand()%15) < 1 )
			{
				gObjAddMsgSendDelay(lpTargetObj, 4, lpObj->m_Index, 100, 0);
				lpTargetObj->m_iMonsterBattleDelay = 10;
				GCActionSend(lpTargetObj, 126, lpTargetObj->m_Index, lpObj->m_Index);
			}
		}
	}

	if ( lpTargetObj->Class == 131 ||BC_STATUE_RANGE(lpTargetObj->Class-132) )
	{
		gObjAddMsgSendDelay(lpTargetObj, 4, lpObj->m_Index, 100, 0);
		gObjSetPosition(lpTargetObj->m_Index, lpTargetObj->X, lpTargetObj->Y);
	}
	else if ( AttackDamage >= 5 )	// To make strong hit
	{
		if ( lpTargetObj->Type == OBJ_MONSTER )
		{
			if ( (rand()%26) == 0 )
			{
				gObjAddMsgSendDelay(lpTargetObj,4, lpObj->m_Index, 100, 0);
			}
		}
		else if ( (rand()%4) == 0 )
		{
			if ( !gObjUniriaSprite(lpTargetObj) )
			{
				MSBFlag = 1;
			}
		}
	}

	if ( ManaChange )
	{
		GCManaSend(lpTargetObj->m_Index, (WORD)lpTargetObj->Mana, 0xFF, 0, lpTargetObj->BP);
	}

	if ( magicsend )
	{
		GCMagicAttackNumberSend(lpObj, skill, lpTargetObj->m_Index, skillSuccess);
	}

	if ( lpObj->Type == OBJ_USER )
	{
		if ( lpObj->m_Change == 9 )
		{
			GCMagicAttackNumberSend(lpObj, 3, lpTargetObj->m_Index, 1);
		}
	}

	if ( lpObj->Class == CLASS_ELF && lpObj->Level == 1 && AttackDamage > 10 )
	{
		LogAdd("error-Level1 : [%s][%s] Str:%d %d %d %d %d %d %d",
			lpObj->AccountID, lpObj->Name, lpObj->Strength,
			lpObj->m_AttackDamageMinRight, lpObj->m_AttackDamageMaxRight,
			lpObj->m_AttackDamageMinLeft, lpObj->m_AttackDamageMaxLeft, 
			lpObj->m_AttackDamageMax, lpObj->m_AttackDamageMin);
	}

	lpObj->m_Rest = 0;

	if ( lpObj->Class >= 504 && lpObj->Class <= 511 )
	{
		if( gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER) )
			AttackDamage *= 2;
	}

	if ( lpTargetObj->Class >= 504 && lpTargetObj->Class <= 511 )
	{
		if( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_BERSERKER) )
			AttackDamage /= 2;
	}

	if ( AttackDamage > 0 )
	{
		int atd_reflect = (int)((float)AttackDamage * lpTargetObj->DamageReflect / 100.0f);
#if(__NOVUS__==1)
		if( lpObj->Type == OBJ_MONSTER 
			&& lpTargetObj->Type == OBJ_USER 
			&& g_SoulSystem.IsMap(lpTargetObj->MapNumber) 
			&& !g_SoulSystem.WithSoul(lpTargetObj) )
		{

		}
		else
		{
#endif
			if ( atd_reflect )
			{
				gObjAddMsgSendDelay(lpTargetObj, 10, lpObj->m_Index, 10, atd_reflect);
			}

			if ( (rand()%100) < lpObj->SetOpReflectionDamage )
			{
				gObjAddMsgSendDelay(lpTargetObj, 10, lpObj->m_Index, 10, AttackDamage);
			}
#if(__NOVUS__==1)
		}
#endif

		int FullReflect = 0;

		if( gObjWingSprite(lpTargetObj) == TRUE )
		{
			CItem * Wing = &lpTargetObj->pInventory[7];

			if( (Wing->m_Type >= ITEMGET(12,36) && Wing->m_Type <= ITEMGET(12,40))
				 || Wing->m_Type == ITEMGET(12,43) 
				 || Wing->m_Type == ITEMGET(12,50) )
			{
				BYTE ExcOpt = lpTargetObj->pInventory[7].m_NewOption;

				if( (ExcOpt & 2) == 2 )	{
					FullReflect+=5;
				}
			}
			else if( Wing->m_Type == ITEMGET(12,266) )
			{
				BYTE ExcOpt = lpTargetObj->pInventory[7].m_NewOption;

				if( (ExcOpt & 2) == 2 )	{
					FullReflect+=5;
				}
			}
#if (CUSTOM_WINGS == 1)
			else if( Wing->m_Type >= ITEMGET(12,440) && Wing->m_Type <= ITEMGET(12,445) )
			{
				BYTE ExcOpt = lpTargetObj->pInventory[7].m_NewOption;

				if( (ExcOpt & 2) == 2 )
				{
					FullReflect+=10;
				}
			}
#endif
		}

		if( lpTargetObj->m_MPSkillOpt.MpsReflectDamage > 0.0f )
		{
			FullReflect += lpTargetObj->m_MPSkillOpt.MpsReflectDamage;
		}
#if(__NOVUS__==1)
		if( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_MONSTER )
		{
			if( g_SoulSystem.IsMap(lpObj->MapNumber) && !g_SoulSystem.WithSoul(lpObj) )
			{
				FullReflect = 0;
			}
		}
#endif
		if( FullReflect && (rand()%100 < FullReflect) )	
		{
			if( lpObj->Type == OBJ_MONSTER )
			{
				gObjAddMsgSendDelay(lpTargetObj, 12, lpObj->m_Index, 10, lpObj->m_AttackDamageMax);
			}
			else
			{
				gObjAddMsgSendDelay(lpTargetObj, 12, lpObj->m_Index, 10, AttackDamage);
			}
		}

		if ( bCombo )
		{
			int iComboDamage = ( Strength + Dexterity + Energy ) / 2;	// #formula
			

			if( lpObj->m_MPSkillOpt.MpsComboAttackIncrease > 0.0f )
			{
				iComboDamage += iComboDamage*lpObj->m_MPSkillOpt.MpsComboAttackIncrease/100.0f;
			}

			AttackDamage += iComboDamage;

			if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
			{
				iTempShieldDamage = this->GetShieldDamage(lpObj, lpTargetObj, iComboDamage);
				lpTargetObj->iShield -= iTempShieldDamage;
				lpTargetObj->Life -= iComboDamage - iTempShieldDamage;
				iTotalShieldDamage += iTempShieldDamage;

				if ( lpTargetObj->Life < 0.0f )
				{
					lpTargetObj->Life = 0.0f;
				}
			}
			else
			{
				lpTargetObj->Life -= iComboDamage;

				if ( lpTargetObj->Life < 0.0f )
				{
					lpTargetObj->Life = 0.0f;
				}
			}

			MsgDamage |= 0x80;
			skill = 59;
		}

		float fRate = lpObj->SetOpDoubleDamage+lpObj->m_MPSkillOpt.MpsSpearMastery;
		fRate += lpObj->m_MPSkillOpt.MpsFistMastery;
		fRate += lpObj->m_MPSkillOpt.MpsDoubleDamageRate;

		if( gObjWingSprite(lpObj) )
		{
			if( lpObj->pInventory[7].m_Type == ITEMGET(12,267) &&
				(lpObj->pInventory[7].m_NewOption&1) == 1)
			{
				fRate += 4;
			}
		}

		if ( (rand()%100) < fRate )
		{
			if ( skill == AT_SKILL_FIRESCREAM || skill == AT_MSKILL_DL_FIRESCREAM1 )
			{
				gObjUseSkill.FireScreamExplosionAttack(lpObj, lpTargetObj, AttackDamage);
			}
			else if( skill == AT_MSKILL_DL_FIRESCREAM2 )
			{
				float fSkillValue = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

				gObjUseSkill.FireScreamExplosionAttack(lpObj,lpTargetObj,AttackDamage+fSkillValue);
			}

			if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
			{
				if( skill == AT_SKILL_RAGEFIGHTER_DRAGONSLAYER && 
					rand()%100 < (10+Energy/100) )
				{	
					bDragonKickSDAttackSuccess = TRUE;
					
					int nDecreaseSDRate = 10 + Energy/30;

					if( nDecreaseSDRate > 100 )
					nDecreaseSDRate = 100;

					int nDecreaseSD = lpTargetObj->iShield*nDecreaseSDRate/100;
					lpTargetObj->iShield -= nDecreaseSD;
					lpTargetObj->Life -= AttackDamage;
					iTotalShieldDamage += nDecreaseSD;
				}
				else
				{
					iTempShieldDamage = this->GetShieldDamage(lpObj, lpTargetObj, AttackDamage);
					lpTargetObj->iShield -= iTempShieldDamage;
					lpTargetObj->Life -= AttackDamage - iTempShieldDamage;
					iTotalShieldDamage += iTempShieldDamage;
				}

				AttackDamage += AttackDamage;

				if ( lpTargetObj->Life < 0.0f )
				{
					lpTargetObj->Life = 0.0f;
				}
			}
			else
			{
				lpTargetObj->Life -= AttackDamage;
				AttackDamage += AttackDamage;

				if ( lpTargetObj->Life < 0.0f )
				{
					lpTargetObj->Life = 0.0f;
				}
			}

			MsgDamage |= 0x40;
		}

		if ( g_ShieldSystemOn == TRUE && bDragonKickSDAttackSuccess == 0 )
		{
			AttackDamage -= iTotalShieldDamage;
		}

		if ( g_ShieldSystemOn == FALSE )
		{
			if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
			{
				if ( CC_MAP_RANGE(lpObj->MapNumber) && CC_MAP_RANGE(lpTargetObj->MapNumber) )
				{
					AttackDamage = AttackDamage * 50 / 100;
				}
			}
		}

		if( lpObj->Class == CLASS_FIGHTER && byBarrageCount )
		{
			if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_WEAPON2 ||
				lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DARKSIDE ||
				lpMagic->m_Skill == AT_MSKILL_RF_UPERCUT1 ||
				lpMagic->m_Skill == AT_MSKILL_RF_UPERCUT2 ||
				lpMagic->m_Skill == AT_MSKILL_RF_DARKSIDE1 ||
				lpMagic->m_Skill == AT_MSKILL_RF_DARKSIDE2 )
			{
				if( (byBarrageCount%2) == 0 )
				{
					MsgDamage |= 0x20;
				}
				else
				{
					MsgDamage |= 0x10;
				}
			}
			else
			{
				if( (byBarrageCount%4) == 0 )
				{
					MsgDamage |= 0x20;
				}
				else
				{
					MsgDamage |= 0x10;
				}
			}
		}

		gObjLifeCheck(lpTargetObj, lpObj, AttackDamage, 0, MSBFlag, MsgDamage, skill, iTotalShieldDamage);

		if ( iTotalShieldDamage > 0 )
		{
			LogAddTD("[PvP System] Victim:[%s][%s], Attacker:[%s][%s] - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				lpTargetObj->AccountID, lpTargetObj->Name, lpObj->AccountID, lpObj->Name,
				lpTargetObj->iShield + iTotalShieldDamage, lpTargetObj->Life + AttackDamage, 
				lpTargetObj->iShield, lpTargetObj->Life);
		}
	}
	else
	{
		GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, MsgDamage, 0);
	}

	if ( lpObj->Life <= 0.0f && lpObj->Type == OBJ_USER )
	{
		if ( lpObj->m_CheckLifeTime <= 0 )
		{
			lpObj->lpAttackObj = lpTargetObj;

			if ( lpTargetObj->Type == OBJ_USER )
			{
				lpObj->m_bAttackerKilled = true;
			}
			else
			{
				lpObj->m_bAttackerKilled = false;
			}

			lpObj->m_CheckLifeTime = 3;
		}
	}

	CItem* Right = &lpObj->pInventory[0];
	CItem* Left = &lpObj->pInventory[1];

	if( Right->GetDetailItemType() == 2 ||
		Left->GetDetailItemType() == 2 )
	{
		if( lpObj->m_MPSkillOpt.MpsMaceMastery > 0.0f )
		{
			float fPercent = rand()%100;

			if( fPercent < lpObj->m_MPSkillOpt.MpsMaceMastery )
			{
				gObjAddBuffEffect(lpTargetObj,BUFF_STUN,0,0,0,0,2);
				gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
			}
		}
	}

	if ( lpMagic && lpMagic->m_Skill == AT_MSKILL_DK_TWISTINGSLASH2 )
	{
		float fSpringValue = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);

		if( fSpringValue > 0.0f )
		{
			float fPercent = rand()%100;

			if( fPercent < fSpringValue )
			{
				gObjBackSpring2(lpTargetObj,lpObj,2);
			}
		}
	}

	if ( lpMagic )
	{
		gObjUseSkill.SpecificSkillAdditionTreat(lpObj, lpTargetObj, lpMagic, AttackDamage);
	}
	return TRUE;
}





int CObjAttack::GetAttackDamage(LPOBJ lpObj, int targetDefense, int& effect, BOOL bIsOnDuel, CMagicInf* lpMagic)
{
	if ( g_ShieldSystemOn == TRUE )
	{
		if ( bIsOnDuel == TRUE )
		{
			bIsOnDuel = FALSE;
		}
	}

	int ad;
	int sub;
	int SkillRightMaxDamage =0;
	int SkillRightMinDamage =0;
	int SkillLeftMaxDamage =0;
	int SkillLeftMinDamage =0;
	CItem * Right = &lpObj->pInventory[0];
	CItem * Left = &lpObj->pInventory[1];
	BOOL bTwoHandWeapon = FALSE;

	if ( lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC )
	{
		int AttackMin = 0;
		sub = lpObj->m_AttackDamageMax - lpObj->m_AttackDamageMin;
		AttackMin = lpObj->m_AttackDamageMin + (rand()%(sub+1));
		lpObj->m_AttackDamageRight = AttackMin;
		lpObj->m_AttackDamageLeft = AttackMin;
	}
	else
	{
		int cDamage=0;

		if ( lpObj->Class == CLASS_DARKLORD ||
			 lpObj->Class == CLASS_KNIGHT   ||
			 lpObj->Class == CLASS_MAGUMSA ||
			 lpObj->Class == CLASS_FIGHTER )
		{
			if ( Right->m_Type >= ITEMGET(0,0) && Right->m_Type < ITEMGET(4,0) &&
				 Left->m_Type >= ITEMGET(0,0) && Left->m_Type < ITEMGET(4,0) )
			{
				if ( Right->m_IsValidItem && Left->m_IsValidItem )
				{
					bTwoHandWeapon = TRUE;
				}
			}
		}

		if ( lpMagic )
		{
			if ( (lpMagic ->m_Skill == AT_SKILL_SPEAR ||
				  lpMagic ->m_Skill == AT_MSKILL_DL_FORCEWAVE1)
				 && lpObj->SkillLongSpearChange )
			{
				SkillRightMaxDamage = DefMagicInf[66].m_DamageMax;
				SkillRightMinDamage = DefMagicInf[66].m_DamageMin;
			}
			else
			{
				SkillRightMaxDamage = lpMagic->m_DamageMax;
				SkillRightMinDamage = lpMagic->m_DamageMin;
			}

			if ( g_MasterSkillSystem.CheckMasterLevelSkill(lpMagic->m_Skill) )
			{
				float fDamage = g_MasterSkillSystem.GetSkillAttackDamage(lpObj,lpMagic->m_Skill);
				SkillRightMaxDamage += fDamage;
				SkillRightMinDamage += fDamage;
			}

			if ( bTwoHandWeapon == FALSE )
			{
				SkillLeftMaxDamage = SkillRightMaxDamage;
				SkillLeftMinDamage = SkillRightMinDamage;
			}

			SkillRightMaxDamage += lpObj->SetOpAddSkillAttack;
			SkillRightMinDamage += lpObj->SetOpAddSkillAttack;
			SkillLeftMaxDamage += lpObj->SetOpAddSkillAttack;
			SkillLeftMinDamage += lpObj->SetOpAddSkillAttack;

#if (ENABLETEST_MUUN == 1) 
			int nMuunItemEffectValue = 0;
			if (g_CMuunSystem.GetMuunItemValueOfOptType(lpObj, 5, &nMuunItemEffectValue, 0)) {
				SkillLeftMaxDamage += nMuunItemEffectValue;
				SkillLeftMinDamage += nMuunItemEffectValue;
				SkillRightMaxDamage += nMuunItemEffectValue;
				SkillRightMinDamage += nMuunItemEffectValue;
			}
#endif

			int SkillAttr = MagicDamageC.GetSkillAttr(lpMagic->m_Skill);

			if ( SkillAttr != -1 )
			{
				if( (lpObj->Authority & 32) == 32 && 
					(lpObj->pInventory[10].m_Type == ITEMGET(13,42) ||
						lpObj->pInventory[11].m_Type == ITEMGET(13,42) ))
				{
					SkillRightMaxDamage += 0xFF;
					SkillRightMinDamage += 0xFF;
					SkillLeftMaxDamage += 0xFF;
					SkillLeftMinDamage += 0xFF;
				}
				else
				{
					SkillRightMaxDamage += (BYTE)lpObj->m_AddResistance[SkillAttr];
					SkillRightMinDamage += (BYTE)lpObj->m_AddResistance[SkillAttr];
					SkillLeftMaxDamage  += (BYTE)lpObj->m_AddResistance[SkillAttr];
					SkillLeftMinDamage  += (BYTE)lpObj->m_AddResistance[SkillAttr];
				}
			}

			SkillRightMaxDamage += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;
			SkillRightMinDamage += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;
			SkillLeftMaxDamage += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;
			SkillLeftMinDamage += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;

			if ( lpMagic->m_Skill == 65 )
			{
				int iPartyCount = this->GetPartyMemberCount(lpObj);
				int addskilldamage = ( lpObj->Leadership + lpObj->AddLeadership ) / 10 + ( iPartyCount * 50 );	// #formula

				SkillRightMaxDamage += addskilldamage;
				SkillRightMinDamage += addskilldamage;
				SkillLeftMaxDamage += addskilldamage;
				SkillLeftMinDamage += addskilldamage;
			}
			else if( lpMagic->m_Skill == 62
					|| lpMagic->m_Skill == AT_MSKILL_DL_EARTHSHAKE1
					|| lpMagic->m_Skill == AT_MSKILL_DL_EARTHSHAKE2 )
			{
				if (lpObj->pInventory[8].m_Type == ITEMGET(13,4) )	// Dark Horse
				{
					int addskilldamage = ( lpObj->Strength + lpObj->AddStrength ) / 10 + ( lpObj->Leadership + lpObj->AddLeadership ) / 5 + lpObj->pInventory[8].m_PetItem_Level * 10;	// #formula

					SkillRightMaxDamage += addskilldamage;
					SkillRightMinDamage += addskilldamage;
					SkillLeftMaxDamage += addskilldamage;
					SkillLeftMinDamage += addskilldamage;
				}
			}
			else if ( lpObj->Class == CLASS_DARKLORD && 
				(lpMagic->m_Skill == AT_SKILL_CHAOTICDISEIER || lpMagic->m_Skill == AT_MSKILL_DL_CHAOTICDESEAR1))
			{
				int addskilldamage = ( lpObj->Strength + lpObj->AddStrength ) / 30 + ( lpObj->Energy + lpObj->AddEnergy ) / 55;


				SkillRightMaxDamage += addskilldamage;
				SkillRightMinDamage += addskilldamage;
				SkillLeftMaxDamage += addskilldamage;
				SkillLeftMinDamage += addskilldamage;
			}
			else if ( lpObj->Class == CLASS_DARKLORD )
			{
				int iadddamage = ( lpObj->Strength + lpObj->AddStrength ) / 25 + ( lpObj->Energy + lpObj->AddEnergy ) / 50;	// #formula

				SkillRightMaxDamage += iadddamage;
				SkillRightMinDamage += iadddamage;
				SkillLeftMaxDamage += iadddamage;
				SkillLeftMinDamage += iadddamage;
			}
			
			
			if ( lpObj->Class == CLASS_ELF && 
				(lpMagic->m_Skill == AT_SKILL_MULTISHOT ||
				 lpMagic->m_Skill == AT_MSKILL_FE_MULTISHOT1 ||
				 lpMagic->m_Skill == AT_MSKILL_FE_MULTISHOT2) )
			{
				SkillLeftMaxDamage = SkillLeftMaxDamage * 80 / 100;
				SkillLeftMinDamage = SkillLeftMinDamage * 80 / 100;
			}
		}

		if ( lpObj->m_CriticalDamage > 0 )
		{
			if ( (rand()%100) < lpObj->m_CriticalDamage )
			{
				cDamage = TRUE;
				effect = 3;
			}
		}

		if ( lpObj->m_ExcelentDamage > 0 )
		{
			if ( (rand()%100) < lpObj->m_ExcelentDamage )
			{
				cDamage = TRUE;
				effect = 2;
			}
		}

		int AddDmgMin = 0;
		int AddDmgMax = 0;

		if( gObjCheckUsedBuffEffect( lpObj,BUFF_BERSERKER ) == TRUE )
		{
			this->GetBuffTypePhysicalIncrease( lpObj, AddDmgMin,AddDmgMax, BUFF_BERSERKER );
		}

		if( gObjCheckUsedBuffEffect( lpObj,BUFF_BERSERKER2 ) == TRUE )
		{
			this->GetBuffTypePhysicalIncrease( lpObj, AddDmgMin,AddDmgMax, BUFF_BERSERKER2 );
		}

		if( gObjCheckUsedBuffEffect( lpObj,BUFF_BERSERKER3 ) == TRUE )
		{
			this->GetBuffTypePhysicalIncrease( lpObj, AddDmgMin,AddDmgMax, BUFF_BERSERKER3 );
		}

		if( gObjCheckUsedBuffEffect( lpObj,BUFF_BERSERKER4 ) == TRUE )
		{
			this->GetBuffTypePhysicalIncrease( lpObj, AddDmgMin,AddDmgMax, BUFF_BERSERKER4 );
		}

		__try
		{
			sub = ( lpObj->m_AttackDamageMaxRight + SkillRightMaxDamage + AddDmgMax) - ( lpObj->m_AttackDamageMinRight + SkillRightMinDamage + AddDmgMin );	// #formula
			lpObj->m_AttackDamageRight = ( lpObj->m_AttackDamageMinRight + SkillRightMinDamage + AddDmgMin ) + (rand()%(sub+1));
		}
		__except ( sub=1, 1 )
		{

		}

		__try
		{
			sub = ( lpObj->m_AttackDamageMaxLeft + SkillLeftMaxDamage + AddDmgMax ) - ( lpObj->m_AttackDamageMinLeft + SkillLeftMinDamage + AddDmgMin );	// #formula
			lpObj->m_AttackDamageLeft = ( lpObj->m_AttackDamageMinLeft + SkillLeftMinDamage + AddDmgMin ) + (rand()%(sub+1));
		}
		__except ( sub=1, 1 )
		{

		}

		if ( cDamage )
		{
			lpObj->m_AttackDamageRight = lpObj->m_AttackDamageMaxRight + SkillRightMaxDamage+ AddDmgMax;
			lpObj->m_AttackDamageLeft = lpObj->m_AttackDamageMaxLeft + SkillLeftMaxDamage+AddDmgMin;

			lpObj->m_AttackDamageRight += lpObj->SetOpAddCriticalDamage;
			lpObj->m_AttackDamageLeft += lpObj->SetOpAddCriticalDamage;

			lpObj->m_AttackDamageRight += lpObj->m_JewelOfHarmonyEffect.HJOpAddCriticalDamage;
			lpObj->m_AttackDamageLeft += lpObj->m_JewelOfHarmonyEffect.HJOpAddCriticalDamage;

			int AddCriticalDmg = 0;

			gObjGetValueOfBuffIndex(lpObj,BUFF_ADD_CRITDMG,&AddCriticalDmg,0);

			lpObj->m_AttackDamageRight += AddCriticalDmg;
			lpObj->m_AttackDamageLeft += AddCriticalDmg;

			gObjGetValueOfBuffIndex(lpObj,BUFF_ADD_CRITDMG2,&AddCriticalDmg,0);

			lpObj->m_AttackDamageRight += AddCriticalDmg;
			lpObj->m_AttackDamageLeft += AddCriticalDmg;

			gObjGetValueOfBuffIndex(lpObj,BUFF_ADD_CRITDMG3,&AddCriticalDmg,0);

			lpObj->m_AttackDamageRight += AddCriticalDmg;
			lpObj->m_AttackDamageLeft += AddCriticalDmg;

			

			if ( effect == 2 )
			{
				lpObj->m_AttackDamageRight += ( lpObj->m_AttackDamageMaxRight + SkillRightMaxDamage ) * 20 / 100;
				lpObj->m_AttackDamageLeft += ( lpObj->m_AttackDamageMaxLeft + SkillLeftMaxDamage ) * 20 / 100;

				lpObj->m_AttackDamageRight += lpObj->SetOpAddExDamage;
				lpObj->m_AttackDamageLeft += lpObj->SetOpAddExDamage;
			}

#if (CUSTOM_EXOPT==1)

			if( effect == 3 )
			{
				if( lpObj->m_CriticalDamageMul > 0 )
				{
					lpObj->m_AttackDamageRight += ( lpObj->m_AttackDamageMaxRight + SkillRightMaxDamage ) * lpObj->m_CriticalDamageMul / 100;
					lpObj->m_AttackDamageLeft += ( lpObj->m_AttackDamageMaxLeft + SkillLeftMaxDamage ) * lpObj->m_CriticalDamageMul / 100;
				}
			}
			else if( effect == 2 )
			{
				if( lpObj->m_ExcelentDamageMul > 0 )
				{
					lpObj->m_AttackDamageRight += ( lpObj->m_AttackDamageMaxRight + SkillRightMaxDamage ) * lpObj->m_ExcelentDamageMul / 100;
					lpObj->m_AttackDamageLeft += ( lpObj->m_AttackDamageMaxLeft + SkillLeftMaxDamage ) * lpObj->m_ExcelentDamageMul / 100;
				}
			}

#endif
		}
	}

	if ( bTwoHandWeapon )
	{
		if ( bIsOnDuel )
		{
			ad = ( lpObj->m_AttackDamageRight + lpObj->m_AttackDamageLeft ) * 60 / 100 - targetDefense;
		}
		else
		{
			ad = ( lpObj->m_AttackDamageRight + lpObj->m_AttackDamageLeft ) - targetDefense;
		}

		ad -= lpObj->SetOpAddCriticalDamage;
	}
	else if ( (Left->m_Type >= ITEMGET(4, 0) && Left->m_Type < ITEMGET(4, 7)) ||
			 Left->m_Type == ITEMGET(4,20) ||
			 Left->m_Type == ITEMGET(4,21) ||
			 Left->m_Type == ITEMGET(4,22) ||
			 Left->m_Type == ITEMGET(4,23) )
	{
		if ( bIsOnDuel )
		{
			ad = ( lpObj->m_AttackDamageLeft ) * 60 / 100 - targetDefense;
		}
		else
		{
			ad = lpObj->m_AttackDamageLeft - targetDefense;
		}

		if( lpObj->m_MPSkillOpt.MpsBowStrength > 0.0f )
		{
			ad += lpObj->m_MPSkillOpt.MpsBowStrength;
		}
	}
	else if ( (Right->m_Type >= ITEMGET(4, 8) && Right->m_Type < ITEMGET(4, 15)) ||
			  ( Right->m_Type >= ITEMGET(4,16) && Right->m_Type < ITEMGET(5,0) )  )
	{
		if ( bIsOnDuel )
		{
			ad = ( lpObj->m_AttackDamageRight ) * 60 / 100 - targetDefense;
		}
		else
		{
			ad = lpObj->m_AttackDamageRight - targetDefense;
		}

		if( lpObj->m_MPSkillOpt.MpsCrossbowStrength > 0.0f )
		{
			ad += lpObj->m_MPSkillOpt.MpsCrossbowStrength;
		}
	}
	else if ( Right->GetDetailItemType() == 0 || Left->GetDetailItemType() == 0 )
	{
		if ( bIsOnDuel )
		{
			ad = ( lpObj->m_AttackDamageRight ) * 60 / 100 - targetDefense;
		}
		else
		{
			ad = lpObj->m_AttackDamageRight - targetDefense;
		}

		if( lpObj->m_MPSkillOpt.MpsTwoHandSwordStrength > 0.0f )
		{
			ad += lpObj->m_MPSkillOpt.MpsTwoHandSwordStrength;
		}
	}
	else if ( Right->m_Type >= ITEMGET(0,0) && Right->m_Type < ITEMGET(4,0) )
	{
		if ( bIsOnDuel )
		{
			ad = ( lpObj->m_AttackDamageRight ) * 60 / 100 - targetDefense;
		}
		else
		{
			ad = lpObj->m_AttackDamageRight - targetDefense;
		}
	}
	else if ( Right->m_Type >= ITEMGET(5,0) && Right->m_Type < ITEMGET(6,0) )
	{
		if ( bIsOnDuel )
		{
			ad = ( lpObj->m_AttackDamageRight ) * 60 / 100 - targetDefense;
		}
		else
		{
			ad = lpObj->m_AttackDamageRight - targetDefense;
		}
	}
	else if ( bIsOnDuel )
	{
		ad = ( lpObj->m_AttackDamageLeft ) * 60 / 100 - targetDefense;
	}
	else
	{
		ad = lpObj->m_AttackDamageLeft - targetDefense;
	}

	if ( lpObj->SetOpTwoHandSwordImproveDamage )
	{
		ad += ad * lpObj->SetOpTwoHandSwordImproveDamage  / 100;
	}

	int AddDamage = 0;
	AddDamage = gObjGetTotalValueOfEffect(lpObj,ADD_OPTION_ATTACK_DAMAGE);
	AddDamage += gObjGetTotalValueOfEffect(lpObj,14);
	ad += AddDamage;
	
	if( lpObj->m_SkillAttack2 )
	{
#ifdef __NOVUS__
		ad += 15 + lpObj->Level / 2;
#else
		ad += 15;
#endif
	}

	if ( lpObj->m_MonsterSkillElementInfo.m_iSkillElementAttackTime > 0 )
	{
		ad += lpObj->m_MonsterSkillElementInfo.m_iSkillElementAttack;

		if ( ad < 0 )
		{
			ad = 0;
		}
	}

	int ReduceDamage = 0;
	gObjGetValueOfBuffIndex(lpObj,BUFF_WEAKNESS,&ReduceDamage,0);
	ad -= ad * ReduceDamage / 100;

	float nAddAttackDamageLeft = 0;
	float nAddAttackDamageRight = 0;

	switch( Left->GetDetailItemType() )
	{
	case 1:
		if( lpObj->m_MPSkillOpt.MpsOneHandSwordStrength > 0.0f )
		{
			nAddAttackDamageLeft += lpObj->m_MPSkillOpt.MpsOneHandSwordStrength;
		}
		break;
	case 2:
		if( lpObj->m_MPSkillOpt.MpsMaceStrength > 0.0f )
		{
			nAddAttackDamageLeft += lpObj->m_MPSkillOpt.MpsMaceStrength;
		}
		break;
	case 3:
		if( lpObj->m_MPSkillOpt.MpsSpearStrength > 0.0f )
		{
			nAddAttackDamageLeft += lpObj->m_MPSkillOpt.MpsSpearStrength;
		}
		break;
	case 11:
		if( lpObj->m_MPSkillOpt.MpsScepterStrength > 0.0f )
		{
			nAddAttackDamageLeft += lpObj->m_MPSkillOpt.MpsScepterStrength;
		}
		break;
	case 12:
		if( lpObj->m_MPSkillOpt.MpsFistStrength > 0.0f )
		{
			nAddAttackDamageLeft += lpObj->m_MPSkillOpt.MpsFistStrength;
		}
		break;
	default:break;
	}

	switch( Right->GetDetailItemType() )
	{
	case 1:
		if( lpObj->m_MPSkillOpt.MpsOneHandSwordStrength > 0.0f )
		{
			nAddAttackDamageRight += lpObj->m_MPSkillOpt.MpsOneHandSwordStrength;
		}
		break;
	case 2:
		if( lpObj->m_MPSkillOpt.MpsMaceStrength > 0.0f )
		{
			nAddAttackDamageRight += lpObj->m_MPSkillOpt.MpsMaceStrength;
		}
		break;
	case 3:
		if( lpObj->m_MPSkillOpt.MpsSpearStrength > 0.0f )
		{
			nAddAttackDamageRight += lpObj->m_MPSkillOpt.MpsSpearStrength;
		}
		break;
	case 11:
		if( lpObj->m_MPSkillOpt.MpsScepterStrength > 0.0f )
		{
			nAddAttackDamageRight += lpObj->m_MPSkillOpt.MpsScepterStrength;
		}
		break;
	case 12:
		if( lpObj->m_MPSkillOpt.MpsFistStrength > 0.0f )
		{
			nAddAttackDamageLeft += lpObj->m_MPSkillOpt.MpsFistStrength;
		}
		break;
	default:break;
	}

	if( bTwoHandWeapon != 0 )
	{
		ad += nAddAttackDamageRight*0.5+nAddAttackDamageLeft*0.5;
	}
	else
	{
		ad += nAddAttackDamageRight;
		ad += nAddAttackDamageLeft;
	}

	if( lpObj->Class == CLASS_KNIGHT ||
		lpObj->Class == CLASS_MAGUMSA ||
		lpObj->Class == CLASS_DARKLORD || 
		lpObj->Class == CLASS_FIGHTER )
	{
		if( lpObj->m_MPSkillOpt.MpsWeaponMastery > 0.0f )
		{
			ad += lpObj->m_MPSkillOpt.MpsWeaponMastery;
		}
	}

	if( lpObj->Class == CLASS_ELF )
	{
		if( lpObj->m_MPSkillOpt.MpsWeaponMastery2 > 0.0f )
		{
			ad += lpObj->m_MPSkillOpt.MpsWeaponMastery2;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsWingAttackIncrease > 0.0f )
	{
		ad += lpObj->m_MPSkillOpt.MpsWingAttackIncrease;
	}

	if( lpObj->m_MPSkillOpt.MpsCommandAttackInc > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 11 ||
			lpObj->pInventory[1].GetDetailItemType() == 11 )
		{
			int nAddAttack = (lpObj->Leadership+lpObj->AddLeadership)/lpObj->m_MPSkillOpt.MpsCommandAttackInc;
			ad += nAddAttack;
		}
	}

	return ad;
}





int  CObjAttack::GetAttackDamageWizard(LPOBJ lpObj, int targetDefense, CMagicInf* lpMagic, int& effect, BOOL bIsOnDuel)
{
	if ( g_ShieldSystemOn == TRUE )
	{
		if ( bIsOnDuel == TRUE )
		{
			bIsOnDuel = FALSE;
		}
	}

	int damagemin;
	int damagemax;
	int ad;

	if ( lpMagic->m_Skill == AT_SKILL_EXPHELL || lpMagic->m_Skill == AT_MSKILL_DW_NOVA1 )
	{
		if ( lpObj->SkillHellFire2Count >= 0 )
		{
			int SkillHellFire2CountDamageTable[13] =
			{
				0,	20,	50,	99,	160,
				225,	325,	425,	550,	700,
				880,	1090,	1320
			};
			int CountDamage;

			if ( lpObj->SkillHellFire2Count > 12 )
			{
				CountDamage = 0;
			}
			else
			{
				CountDamage = SkillHellFire2CountDamageTable[lpObj->SkillHellFire2Count];
			}

			ad = ( lpObj->Strength + lpObj->AddStrength ) / 2 + CountDamage;
			damagemin = ad + lpObj->m_MagicDamageMin;
			damagemax = ad + lpObj->m_MagicDamageMax;

			if( g_MasterSkillSystem.CheckMasterLevelSkill(lpMagic->m_Skill) )
			{
				float fDamage = g_MasterSkillSystem.GetSkillAttackDamage(lpObj,lpMagic->m_Skill);

				damagemin += fDamage;
				damagemax += fDamage;
			}

#if (ENABLETEST_MUUN == 1) 
			int nMuunItemEffectValue = 0;
			if (g_CMuunSystem.GetMuunItemValueOfOptType(lpObj, 5, &nMuunItemEffectValue, 0)) {
				damagemin += nMuunItemEffectValue;
				damagemax += nMuunItemEffectValue;
			}
#endif

			damagemin += lpObj->SetOpAddSkillAttack;
			damagemax += lpObj->SetOpAddSkillAttack;

			int SkillAttr = MagicDamageC.GetSkillAttr(lpMagic->m_Skill);

			if ( SkillAttr != -1 )
			{
				if( (lpObj->Authority & 32) == 32 &&
					(lpObj->pInventory[10].m_Type == ITEMGET(13,42) ||
					lpObj->pInventory[11].m_Type == ITEMGET(13,42) ) )
				{
					damagemin += 0xFF;
					damagemax += 0xFF;
				}
			}

			damagemin += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;
			damagemax += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;
		}
	}
	else
	{
		ad = lpMagic->GetDamage();


		damagemin = lpMagic->m_DamageMin + lpObj->m_MagicDamageMin;
		damagemax = lpMagic->m_DamageMax + lpObj->m_MagicDamageMax;

		if( g_MasterSkillSystem.CheckMasterLevelSkill(lpMagic->m_Skill) )
		{
			float fDamage = g_MasterSkillSystem.GetSkillAttackDamage(lpObj,lpMagic->m_Skill);

			damagemin += fDamage;
			damagemax += fDamage;
		}

#if (ENABLETEST_MUUN == 1) 
		int nMuunItemEffectValue = 0;
		if (g_CMuunSystem.GetMuunItemValueOfOptType(lpObj, 5, &nMuunItemEffectValue, 0)) {
			damagemin += nMuunItemEffectValue;
			damagemax += nMuunItemEffectValue;
		}
#endif

		damagemin += lpObj->SetOpAddSkillAttack;
		damagemax += lpObj->SetOpAddSkillAttack;

		int SkillAttr = MagicDamageC.GetSkillAttr(lpMagic->m_Skill);

		if ( SkillAttr != -1 )
		{
			if( (lpObj->Authority & 32) == 32 &&
				(lpObj->pInventory[10].m_Type == ITEMGET(13,42) ||
				lpObj->pInventory[11].m_Type == ITEMGET(13,42) ) )
			{
				damagemin += 0xFF;
				damagemax += 0xFF;
			}
		}

		damagemin += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;
		damagemax += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;
	}

	if( lpObj->m_MPSkillOpt.MpsWingAttackIncrease > 0.0f )
	{
		damagemin += lpObj->m_MPSkillOpt.MpsWingAttackIncrease;
		damagemax += lpObj->m_MPSkillOpt.MpsWingAttackIncrease;
	}

	CItem * Staff = &lpObj->pInventory[0];

	if ( Staff->IsItem() )
	{
		if ( (Staff->m_Type >= ITEMGET(5,0) && Staff->m_Type < ITEMGET(6,0) ) ||
			 Staff->m_Type == ITEMGET(0,30) ||
			 Staff->m_Type == ITEMGET(0,31) ||
			 Staff->m_Type == ITEMGET(0,21) ||
			 Staff->m_Type == ITEMGET(0,23) ||
			 Staff->m_Type == ITEMGET(0,25) ||
			 Staff->m_Type == ITEMGET(0,28) )
		{
			if ( Staff->m_IsValidItem  )
			{
				int damage = Staff->m_Magic / 2 + Staff->m_Level * 2;	// #formula
				damage -= (WORD)(Staff->m_CurrentDurabilityState * damage);	// #formula

				damagemin += damagemin * damage / 100;	// #formula
				damagemax += damagemax * damage / 100;	// #formula
			}
		}
	}

	int subd = damagemax - damagemin;

	__try
	{
		if ( bIsOnDuel )
		{
			ad = ( damagemin + (rand()%(subd+1)) ) * 60 / 100 - targetDefense;	// #formula
		}
		else
		{
			ad = ( damagemin + (rand()%(subd+1)) ) - targetDefense;
		}

		int nCritical = lpObj->m_CriticalDamage;

		if ( gObjCheckUsedBuffEffect(lpObj,BUFF_EXP_WIZARDRY3) == TRUE )
		{
			nCritical += lpObj->m_MPSkillOpt.MpsExpWizardryCrit;
		}

		if ( nCritical )
		{
			if ( (rand()%100) < nCritical )
			{
				if ( bIsOnDuel )
				{
					ad = damagemax * 60 / 100 - targetDefense;
				}
				else
				{
					ad = damagemax - targetDefense;
				}

				ad += lpObj->SetOpAddCriticalDamage;
				ad += lpObj->m_JewelOfHarmonyEffect.HJOpAddCriticalDamage;

				int AddCriticalDmg = 0;
				gObjGetValueOfBuffIndex(lpObj,BUFF_ADD_CRITDMG,&AddCriticalDmg,0);
				ad += AddCriticalDmg;
				gObjGetValueOfBuffIndex(lpObj,BUFF_ADD_CRITDMG2,&AddCriticalDmg,0);
				ad += AddCriticalDmg;
				gObjGetValueOfBuffIndex(lpObj,BUFF_ADD_CRITDMG3,&AddCriticalDmg,0);
				ad += AddCriticalDmg;

				effect = 3;

#if (CUSTOM_EXOPT==1)
				if( lpObj->m_CriticalDamageMul > 0 )
				{
					ad += ad * lpObj->m_CriticalDamageMul / 100;
				}
#endif
			}
		}

		if ( lpObj->m_ExcelentDamage > 0 )
		{
			if ( (rand()%100) < lpObj->m_ExcelentDamage )
			{
				if ( bIsOnDuel )
				{
					ad = damagemax * 60 / 100 - targetDefense;
				}
				else
				{
					ad = damagemax - targetDefense;
				}

				ad += damagemax * 20 / 100;
				ad += lpObj->SetOpAddExDamage;

				effect = 2;

#if (CUSTOM_EXOPT==1)
				if( lpObj->m_ExcelentDamageMul > 0 )
				{
					ad += ad * lpObj->m_ExcelentDamageMul / 100;
				}
#endif
			}
		}
	}
	__except ( subd=1, 1 )
	{

	}

	int AddDamage = 0;

	AddDamage = gObjGetTotalValueOfEffect(lpObj,ADD_OPTION_ATTACK_DAMAGE);
	AddDamage += gObjGetTotalValueOfEffect(lpObj,15);
	ad+= AddDamage;

	if ( lpObj->m_SkillAttack2 )
	{
#ifdef __NOVUS__
		ad += 15 + lpObj->Level / 2;
#else
		ad += 10;
#endif
	}

	return ad;
}

int CObjAttack::GetAttackDamageSummoner(LPOBJ lpObj, int targetDefense, CMagicInf* lpMagic, int& effect, BOOL bIsOnDuel)
{
	if ( g_ShieldSystemOn == TRUE )
	{
		if ( bIsOnDuel == TRUE )
		{
			bIsOnDuel = FALSE;
		}
	}

	float fCurseDamageMin = 0;
	float fCurseDamageMax = 0;

	float fMagicDamageMin = 0;
	float fMagicDamageMax = 0;

	int damagemin;
	int damagemax;
	int skilldamage;

	int ad = 0;

	skilldamage = lpMagic->GetDamage();

	switch( lpMagic->m_Skill )
	{
	case 219:
	case 220:
	case 221:
	case AT_SKILL_INNOVATION:
	case AT_MSKILL_SUM_INNOVATION1:
	case 223:
	case 224:
	case 225:
		{
			if( gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER) == TRUE )
			{
				float adddmgperc = (float)gObjGetTotalValueOfEffect(lpObj,31);

				fCurseDamageMax = ((lpObj->Energy + lpObj->AddEnergy)/4) * adddmgperc / 100.0f;
				fCurseDamageMin = ((lpObj->Energy + lpObj->AddEnergy)/9) * adddmgperc / 100.0f;
			}

			if( gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER2) == TRUE ||
				gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER3) == TRUE ||
				gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER4) == TRUE )
			{
				float adddmgperc = (float)gObjGetTotalValueOfEffect(lpObj,31);

				if( lpObj->m_MPSkillOpt.MpsBerserkInc1 > 0.0f )
				{
					adddmgperc += lpObj->m_MPSkillOpt.MpsBerserkInc1;
				}

				fCurseDamageMax = ((lpObj->Energy + lpObj->AddEnergy)/4) * adddmgperc / 100.0f;
				fCurseDamageMin = ((lpObj->Energy + lpObj->AddEnergy)/9) * adddmgperc / 100.0f;
			}

			damagemin = int(lpMagic->m_DamageMin + lpObj->m_CurseDamageMin + fCurseDamageMin);
			damagemax = int(lpMagic->m_DamageMax + lpObj->m_CurseDamageMax + fCurseDamageMax);

			if( g_MasterSkillSystem.CheckMasterLevelSkill(lpMagic->m_Skill) )
			{
				float fDamage = g_MasterSkillSystem.GetSkillAttackDamage(lpObj,lpMagic->m_Skill);

				damagemin += fDamage;
				damagemax += fDamage;
			}

			if( lpObj->m_MPSkillOpt.MpsMagicMastery2 > 0.0f )
			{
				damagemin += lpObj->m_MPSkillOpt.MpsMagicMastery2;
				damagemax += lpObj->m_MPSkillOpt.MpsMagicMastery2;
			}

			if( lpObj->m_MPSkillOpt.MpsMinWizCurseInc > 0.0f )
			{
				damagemin += lpObj->m_MPSkillOpt.MpsMinWizCurseInc;
			}

			if( lpObj->m_MPSkillOpt.MpsWingAttackIncrease > 0.0f )
			{
				damagemin += lpObj->m_MPSkillOpt.MpsWingAttackIncrease;
				damagemax += lpObj->m_MPSkillOpt.MpsWingAttackIncrease;
			}

			CItem* Right = &lpObj->pInventory[1];
	
			int CurseDamage;
	
			if( Right->IsItem() )
			{
				if( ( Right->m_Type >= ITEMGET(5,0) && Right->m_Type < ITEMGET(6,0) )
					&& Right->m_IsValidItem )
				{
					CurseDamage = Right->m_CurseMagic / 2 + Right->m_Level * 2;
					CurseDamage -= (WORD)(Right->m_CurrentDurabilityState * CurseDamage);	// #formula
	
					damagemin += damagemin * CurseDamage / 100;
					damagemax += damagemax * CurseDamage / 100;
				}
			}

			ad += gObjGetTotalValueOfEffect(lpObj,0x1B);
		}
		break;
	default:
		{
			if( gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER) == TRUE )
			{
				float adddmgperc = (float)gObjGetTotalValueOfEffect(lpObj,31);

				fMagicDamageMin = ((lpObj->Energy + lpObj->AddEnergy)/9) * adddmgperc / 100 ;
				fMagicDamageMax = ((lpObj->Energy + lpObj->AddEnergy)/4) * adddmgperc / 100 ;
			}

			if( gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER2) == TRUE ||
				gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER3) == TRUE ||
				gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER4) == TRUE )
			{
				float adddmgperc = (float)gObjGetTotalValueOfEffect(lpObj,31);

				if( lpObj->m_MPSkillOpt.MpsBerserkInc2 > 0.0f )
				{
					adddmgperc += lpObj->m_MPSkillOpt.MpsBerserkInc2;
				}

				fMagicDamageMin = ((lpObj->Energy + lpObj->AddEnergy)/9) * adddmgperc / 100;
				fMagicDamageMax = ((lpObj->Energy + lpObj->AddEnergy)/4) * adddmgperc / 100;
			}

			if( lpObj->m_MPSkillOpt.MpsMagicMastery2 > 0.0f )
			{
				fMagicDamageMin += lpObj->m_MPSkillOpt.MpsMagicMastery2;
				fMagicDamageMax += lpObj->m_MPSkillOpt.MpsMagicMastery2;
			}

			if( lpObj->m_MPSkillOpt.MpsMinWizCurseInc > 0.0f )
			{
				fMagicDamageMin += lpObj->m_MPSkillOpt.MpsMinWizCurseInc;
			}

			damagemin = (int)(lpMagic->m_DamageMin + lpObj->m_MagicDamageMin + fMagicDamageMin);
			damagemax = (int)(lpMagic->m_DamageMax + lpObj->m_MagicDamageMax + fMagicDamageMax);

			if( g_MasterSkillSystem.CheckMasterLevelSkill(lpMagic->m_Skill) )
			{
				float fDamage = g_MasterSkillSystem.GetSkillAttackDamage(lpObj,lpMagic->m_Skill);

				damagemin += fDamage;
				damagemax += fDamage;
			}

			if( lpObj->m_MPSkillOpt.MpsWingAttackIncrease > 0.0f )
			{
				damagemin += lpObj->m_MPSkillOpt.MpsWingAttackIncrease;
				damagemax += lpObj->m_MPSkillOpt.MpsWingAttackIncrease;
			}
	
			CItem* Left = &lpObj->pInventory[0];

			int Damage;
	
			if( Left->IsItem() )
			{
				if( ( Left->m_Type >= ITEMGET(5,0) && Left->m_Type < ITEMGET(6,0) )
					&& Left->m_IsValidItem )
				{
					Damage = Left->m_Magic / 2 + Left->m_Level * 2;
					Damage -= (WORD)(Left->m_CurrentDurabilityState * Damage);	// #formula
	
					damagemin += damagemin * Damage / 100;
					damagemax += damagemax * Damage / 100;
				}
			}

			ad += gObjGetTotalValueOfEffect(lpObj,15);
		}
		break;
	}

	damagemin += lpObj->SetOpAddSkillAttack;
	damagemax += lpObj->SetOpAddSkillAttack;

	int SkillAttr = MagicDamageC.GetSkillAttr(lpMagic->m_Skill);

	if ( SkillAttr != -1 )
	{
		if( (lpObj->Authority & 32) == 32 &&
			(lpObj->pInventory[10].m_Type == ITEMGET(13,42) ||
			lpObj->pInventory[11].m_Type == ITEMGET(13,42) ) )
		{
			damagemin += 0xFF;
			damagemax += 0xFF;
		}
		else
		{
			damagemin += (BYTE)lpObj->m_AddResistance[SkillAttr];
			damagemax += (BYTE)lpObj->m_AddResistance[SkillAttr];
		}
	}


	damagemin += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;
	damagemax += lpObj->m_JewelOfHarmonyEffect.HJOpAddSkillAttack;

#if (ENABLETEST_MUUN == 1) 
		int nMuunItemEffectValue = 0;
		if (g_CMuunSystem.GetMuunItemValueOfOptType(lpObj, 5, &nMuunItemEffectValue, 0)) {
			damagemin += nMuunItemEffectValue;
			damagemax += nMuunItemEffectValue;
		}
#endif

	int subd = damagemax - damagemin;

	__try
	{
		if ( bIsOnDuel )
		{
			skilldamage = ( damagemin + (rand()%(subd+1)) ) * 60 / 100 - targetDefense;	// #formula
		}
		else
		{
			skilldamage = ( damagemin + (rand()%(subd+1)) ) - targetDefense;
		}

		if ( lpObj->m_CriticalDamage > 0 )
		{
			if ( (rand()%100) < lpObj->m_CriticalDamage )
			{
				if ( bIsOnDuel )
				{
					skilldamage = damagemax * 60 / 100 - targetDefense;
				}
				else
				{
					skilldamage = damagemax - targetDefense;
				}

				skilldamage += lpObj->SetOpAddCriticalDamage;
				skilldamage += lpObj->m_JewelOfHarmonyEffect.HJOpAddCriticalDamage;

				int AddCriticalDmg = 0;
				gObjGetValueOfBuffIndex(lpObj,BUFF_ADD_CRITDMG,&AddCriticalDmg,0);
				skilldamage += AddCriticalDmg;
				gObjGetValueOfBuffIndex(lpObj,BUFF_ADD_CRITDMG2,&AddCriticalDmg,0);
				skilldamage += AddCriticalDmg;
				gObjGetValueOfBuffIndex(lpObj,BUFF_ADD_CRITDMG3,&AddCriticalDmg,0);
				skilldamage += AddCriticalDmg;
				effect = 3;

#if (CUSTOM_EXOPT==1)
				if( lpObj->m_CriticalDamageMul > 0 )
				{
					ad += ad * lpObj->m_CriticalDamageMul / 100;
				}
#endif
			}
		}

		if ( lpObj->m_ExcelentDamage > 0 )
		{
			if ( (rand()%100) < lpObj->m_ExcelentDamage )
			{
				if ( bIsOnDuel )
				{
					skilldamage= damagemax * 60 / 100 - targetDefense;
				}
				else
				{
					skilldamage = damagemax - targetDefense;
				}

				skilldamage += damagemax * 20 / 100;
				skilldamage += lpObj->SetOpAddExDamage;

				effect = 2;

#if (CUSTOM_EXOPT==1)
				if( lpObj->m_ExcelentDamageMul > 0 )
				{
					ad += ad * lpObj->m_ExcelentDamageMul / 100;
				}
#endif
			}
		}
	}
	__except( subd=1, 1 )
	{

	}

	ad += gObjGetTotalValueOfEffect(lpObj,ADD_OPTION_ATTACK_DAMAGE);
	skilldamage += ad;

	if ( lpObj->m_SkillAttack2 )
	{
#ifdef __NOVUS__
		skilldamage += 15 + lpObj->Level / 2;
#else
		skilldamage += 10;
#endif
	}

	if( lpObj->m_MPSkillOpt.MpsBerserkInc2 > 0.0f )
	{
		skilldamage += skilldamage*lpObj->m_MPSkillOpt.MpsBerserkInc2/100.0f;
	}
	
	return skilldamage;
}


BOOL gObjDenorantSprite(LPOBJ lpObj)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem * Denorant = & lpObj->pInventory[8];

	if ( Denorant->m_Type == ITEMGET(13,3) )
	{
		if ( Denorant->m_Durability > 0.0f )
		{
			return TRUE;
		}
	}

	return FALSE;
}




BOOL gObjDarkHorse(LPOBJ lpObj)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem * Denorant = & lpObj->pInventory[8];

	if ( Denorant->m_Type == ITEMGET(13,4) )
	{
		if ( Denorant->m_Durability > 0.0f )
		{
			return TRUE;
		}
	}

	return FALSE;
}



BOOL gObjFenrir(LPOBJ lpObj)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem * lpFenrir = & lpObj->pInventory[8];

	if ( lpFenrir->m_Type == ITEMGET(13,37) )	// Fenrir
	{
		if ( lpFenrir->m_Durability > 0.0f )
		{
			return TRUE;
		}
	}

	return FALSE;
}


int CObjAttack::GetShieldDamage(LPOBJ lpObj, LPOBJ lpTargetObj, int iAttackDamage)
{
	int iShieldDamage = 0;

	if ( g_ShieldSystemOn == FALSE )
		return 0;

	if ( iAttackDamage <= 0 )
		return 0;

	int iReduceLife = 0;
	int iReduceShield = 0;
	int iReduceLifeForEffect = 0; 
	bool bReduceShieldGage = 0;
	int iDamageDevideToSDRate = g_iDamageDevideToSDRate;
	iDamageDevideToSDRate -= lpObj->m_JewelOfHarmonyEffect.HJOpDecreaseSDRate;
	iDamageDevideToSDRate += lpTargetObj->m_JewelOfHarmonyEffect.HJOpAddSDRate;

	if ( iDamageDevideToSDRate < 0 )
		iDamageDevideToSDRate = 0;

	if ( iDamageDevideToSDRate > 100 )
		iDamageDevideToSDRate = 100;

	if ( lpObj->m_JewelOfHarmonyEffect.HJOpAddIgnoreSDRate > 0 )
	{
		int iRand = rand()%100;

		if ( iRand < lpObj->m_JewelOfHarmonyEffect.HJOpAddIgnoreSDRate )
		{
			iDamageDevideToSDRate = 0;
		}
	}

	if ( lpObj->m_JewelOfHarmonyEffect.HJOpDecreaseSDRate ||
		 lpTargetObj->m_JewelOfHarmonyEffect.HJOpAddSDRate ||
		 lpObj->m_JewelOfHarmonyEffect.HJOpAddIgnoreSDRate )
	{
		LogAddTD("[JewelOfHarmony][PvP System] Attacker:[%s][%s]-SD Decrease[%d] SD Ignore[%d] Defender:[%s][%s] SD Increase Option[%d] - SD Rate[%d]",
			lpObj->AccountID, lpObj->Name,
			lpObj->m_JewelOfHarmonyEffect.HJOpDecreaseSDRate,
			lpObj->m_JewelOfHarmonyEffect.HJOpAddIgnoreSDRate,
			lpTargetObj->AccountID, lpTargetObj->Name,
			lpTargetObj->m_JewelOfHarmonyEffect.HJOpAddSDRate,
			iDamageDevideToSDRate);
	}

	iReduceShield = iAttackDamage * iDamageDevideToSDRate / 100;
	iReduceLife = iAttackDamage - iReduceShield;

	if ( (lpTargetObj->iShield-iReduceShield) <0 )
	{
		iReduceLife += iReduceShield  - lpTargetObj->iShield;
		iReduceShield = lpTargetObj->iShield;

		if ( lpTargetObj->iShield > 0 )
		{
			bReduceShieldGage = true;
		}
	}

	iReduceLifeForEffect = (int)(( lpTargetObj->MaxLife + lpTargetObj->AddLife ) * 20.0f / 100.0f);

	if ( bReduceShieldGage == true && iReduceLife > iReduceLifeForEffect )
	{
		if ( !CC_MAP_RANGE(lpTargetObj->MapNumber) )
		{
			GCSendEffectInfo(lpTargetObj->m_Index, 17);
		}
	}

	return iReduceShield;
}


bool CObjAttack::GetBuffTypePhysicalIncrease(LPOBJ lpObj,int& iAttackBerserkerMin,int& iAttackBerserkerMax,int nBuffType)
{
	iAttackBerserkerMin = ( lpObj->Strength+lpObj->AddStrength + lpObj->Dexterity + lpObj->AddDexterity ) / 7;
	iAttackBerserkerMax = ( lpObj->Strength+lpObj->AddStrength + lpObj->Dexterity + lpObj->AddDexterity ) / 4;

	int iValue = 0;
	gObjGetValueOfBuffIndex(lpObj,nBuffType,&iValue,0);

	iAttackBerserkerMin = iAttackBerserkerMin*iValue / 100;
	iAttackBerserkerMax = iAttackBerserkerMax*iValue / 100;
return true;
}