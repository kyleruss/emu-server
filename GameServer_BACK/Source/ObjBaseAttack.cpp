#include "stdafx.h"
#include "ObjBaseAttack.h"
#include "GameMain.h"
#include "LogProc.h"
#include "DevilSquare.h"
#include "BloodCastle.h"
#include "ChaosCastle.h"
#include "CrywolfAltar.h"
#include "IllusionTempleEvent.h"
#include "CastleSiegeSync.h"
#include "MasterSkillSystem.h"
#include "MasterLevelSkillTreeSystem.h"
#include "BuffEffectSlot.h"
#include "ItemAddOption.h"
#include "DoubleGoerEvent.h"
#include "ImperialGuardian.h"
#include "ClassCalc.h"
#include "BuffSkillEffect.h"

#if (ENABLETEST_MUUN == 1)
#include "MuunSystem.h"
#endif

#if (ENABLE_CUSTOM_PVPBALANCE == 1)
#include "PvPBalance.h"
#endif

CObjBaseAttack::CObjBaseAttack()
{
	return;
}

CObjBaseAttack::~CObjBaseAttack()
{
	return;
}

BOOL CObjBaseAttack::DecreaseArrow(LPOBJ lpObj)
{
	if ( lpObj->Class == CLASS_ELF && lpObj->Type == OBJ_USER )
	{
		CItem * Right = &lpObj->pInventory[0];
		CItem * Left  = &lpObj->pInventory[1];

		if ( (Right->m_Type >= ITEMGET(4,8) && Right->m_Type <= ITEMGET(4,14)) ||
			  Right->m_Type == ITEMGET(4,16) || Right->m_Type == ITEMGET(4,18) ||
			  Right->m_Type == ITEMGET(4,19) )
		{
			if ( Left->m_Type == ITEMGET(4,7) )
			{
				if ( Left->m_Durability < 1.0f )
				{
					return FALSE;
				}

				Left->m_Durability -= 1.0f;
				GCItemDurSend(lpObj->m_Index, 1, (BYTE)Left->m_Durability, 0);

				if ( Left->m_Durability < 1.0f )
				{
					lpObj->pInventory[1].Clear();
					GCInventoryItemDeleteSend(lpObj->m_Index, 1, 0);
				}
			}
			else
			{
				return FALSE;
			}
		}
		else if ( (Left->m_Type >= ITEMGET(4,0) && Left->m_Type <= ITEMGET(4,6) ) ||
				   Left->m_Type == ITEMGET(4,17) ||
				   Left->m_Type == ITEMGET(4,20) ||
				   Left->m_Type == ITEMGET(4,21) ||
				   Left->m_Type == ITEMGET(4,22) ||
				   Left->m_Type == ITEMGET(4,23) ||
				   Left->m_Type == ITEMGET(4,24) )
		{
			if ( Right->m_Type == ITEMGET(4,15) )
			{
				if ( Right->m_Durability < 1.0f )
				{
					return FALSE;
				}

				Right->m_Durability -= 1.0f;
				GCItemDurSend(lpObj->m_Index, 0, (BYTE)Right->m_Durability, 0);

				if ( Right->m_Durability < 1.0f )
				{
					lpObj->pInventory[0].Clear();
					GCInventoryItemDeleteSend(lpObj->m_Index, 0, 0);
				}
			}
			else
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}





BOOL CObjBaseAttack::CheckAttackArea(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	if ( lpTargetObj->Type == OBJ_USER || lpTargetObj->m_RecallMon >= 0 )
	{
		BYTE attr = MapC[lpTargetObj->MapNumber].GetAttr(lpTargetObj->X, lpTargetObj->Y);

		if ( (attr&1) == 1 )
		{
			return FALSE;
		}
	}

	if ( lpObj->Type == OBJ_USER )
	{
		int iRet = gObjCheckAttackArea(lpObj->m_Index, lpTargetObj->m_Index);

		if ( iRet != 0 )
		{
			LogAddTD("[%s][%s] Try Attack In Not Attack Area (%s,%d,%d) errortype = %d",
				lpObj->AccountID, lpObj->Name, lMsg.Get(MSGGET(7, 208) + lpObj->MapNumber),
				lpObj->X, lpObj->Y, iRet);

			if ( bIsIgnorePacketSpeedHackDetect != FALSE )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}





BOOL CObjBaseAttack::PkCheck(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	if ( gNonPK != 0 )
	{
		if( lpObj->Type == OBJ_USER )
		{
			int bTmp = 0;

#if (ENABLE_FIX_PARTYPK == 1)

			if( lpObj->m_PK_Level >= 6 )
			{
				bTmp = TRUE;
			}

#else
			if( lpObj->PartyNumber >= 0 )
			{
				if( gParty.GetPartyPkLevel(lpObj->PartyNumber) >= 6 )
				{
					bTmp = TRUE;
				}
			}
			else if( lpObj->m_PK_Level >= 6 )
			{
				bTmp = TRUE;
			}
#endif
			if( bTmp == TRUE )
			{
				if( lpObj->m_PK_Count >= 3 )
				{
					if( gPkLimitFree == FALSE )
					{
						return FALSE;
					}
				}
			}
		}
	}

	if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER)
	{
		if ( lpTargetObj->Level <= 5 || lpObj->Level <= 5 )
		{
			return FALSE;
		}

		if ( gObjGetRelationShip(lpObj, lpTargetObj) == 2 ) // Rivals
		{
			if ( gNonPK == FALSE )
			{
				return TRUE;
			}
		}

		if ( lpObj->lpGuild != NULL && lpTargetObj->lpGuild != NULL )
		{
			if ( lpObj->lpGuild->WarState == 1 && lpTargetObj->lpGuild->WarState == 1 )
			{
				if ( lpObj->lpGuild->Number == lpTargetObj->lpGuild->Number )
				{
					return FALSE;
				}
			}
		}

		if ( gObjTargetGuildWarCheck(lpObj, lpTargetObj) == FALSE && lpTargetObj->lpGuild != NULL && lpTargetObj->lpGuild->WarState != 0)
		{
			if ( lpTargetObj->lpGuild->WarType == 1 )
			{
				if ( lpTargetObj->MapNumber != 6 )
				{
					if ( !gNonPK )
					{
						return TRUE;
					}
				}
			}

			if ( CC_MAP_RANGE(lpTargetObj->MapNumber) == FALSE )
			{
				return FALSE;
			}
		}

		if ( IT_MAP_RANGE(lpObj->MapNumber) )
		{

		}
		else if ( CC_MAP_RANGE(lpObj->MapNumber) != FALSE )
		{
			if ( g_ChaosCastle.GetCurrentState(g_ChaosCastle.GetFloor(lpObj->MapNumber)) != 2 )
			{
				return FALSE;
			}
		}
		else if ( gNonPK != FALSE )
		{
			return FALSE;
		}

		if ( DS_MAP_RANGE(lpObj->MapNumber) ) // DEvil
		{
			return FALSE;
		}

		if ( BC_MAP_RANGE(lpObj->MapNumber) )
		{
			return FALSE;
		}

		if ( DG_MAP_RANGE(lpObj->MapNumber) )
		{
			return FALSE;
		}

		if ( IF_MAP_RANGE(lpObj->MapNumber) )
		{
			return FALSE;
		}

		if ( gObjDuelCheck(lpObj, lpTargetObj) == FALSE )
		{
			if ( gObjDuelCheck(lpTargetObj) )
			{
				return FALSE;
			}
		}

		int bTmp = 0;

#if (ENABLE_FIX_PARTYPK == 1)
		if (lpObj->m_PK_Level >= 6 )
		{
			bTmp = TRUE;
		}
#else
		if ( lpObj->PartyNumber >= 0 )
		{
			if( gParty.GetPartyPkLevel(lpObj->PartyNumber) >= 6 )
			{
				bTmp = TRUE;
			}
		}
		else if (lpObj->m_PK_Level >= 6 )
		{
			bTmp = TRUE;
		}
#endif

		if( bTmp == TRUE )
		{
			if( lpObj->m_PK_Count >= 3 && gPkLimitFree == FALSE )
			{
				return FALSE;
			}
		}

		bTmp = 0;

#if (ENABLE_FIX_PARTYPK == 1)
		if (lpTargetObj->m_PK_Level >= 6 )
		{
			bTmp = TRUE;
		}
#else
		if ( lpTargetObj->PartyNumber >= 0 )
		{
			if( gParty.GetPartyPkLevel(lpTargetObj->PartyNumber) >= 6 )
			{
				bTmp = TRUE;
			}
		}
		else if (lpTargetObj->m_PK_Level >= 6 )
		{
			bTmp = TRUE;
		}
#endif

		if( bTmp == TRUE )
		{
#if (__CUSTOM__ == 1)
#else
			if( lpTargetObj->PartyNumber >= 0 )
			{
				if( gPkLimitFree == FALSE && g_CastleSiegeSync.GetCastleState() != 7)
				{
					return FALSE;
				}
			}
			else if( lpTargetObj->m_PK_Count >= 3 && gPkLimitFree == FALSE && g_CastleSiegeSync.GetCastleState() != 7)
			{
				return FALSE;
			}
#endif
		}
	}
	return TRUE;
}





BOOL CObjBaseAttack::ResistanceCheck(LPOBJ lpObj, LPOBJ lpTargetObj, int skill)
{
#if (GS_CASTLE == 1)
	if( lpTargetObj->Type != OBJ_USER )
	{
		if(lpTargetObj->Class == 277
			|| lpTargetObj->Class == 283
			|| lpTargetObj->Class == 288
			|| lpTargetObj->Class == 278
			|| lpTargetObj->Class == 215
			|| lpTargetObj->Class == 216
			|| lpTargetObj->Class == 217
			|| lpTargetObj->Class == 218
			|| lpTargetObj->Class == 219)
		{
			return FALSE;
		}
	}

	if( lpTargetObj->Type != OBJ_USER )
	{
		if(CRYWOLF_ALTAR_CLASS_RANGE(lpTargetObj->Class) || ((lpTargetObj->Class == 204) ? TRUE : FALSE))
		{
			return FALSE;
		}
	}

#endif

	if ( skill == AT_SKILL_DARKHORSE_ATTACK
		|| skill == AT_MSKILL_DL_EARTHSHAKE1
		|| skill == AT_MSKILL_DL_EARTHSHAKE2 )
	{
		gObjBackSpring2(lpTargetObj, lpObj, 3);
	}

	float fValue = 0;

	if( skill == AT_MSKILL_DL_EARTHSHAKE1 )
	{
		for(int i = 0; i < MAX_MAGIC; i++)
		{
			if( lpObj->Magic[i].IsMagic() )
			{
				if( lpObj->Magic[i].m_Skill == skill )
				{
					fValue = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(skill),lpObj->Magic[i].m_Level);
					break;
				}
			}
		}

		float fRand = rand()%100;

		if( fRand < fValue )
		{
			gObjBackSpring(lpTargetObj,lpObj);
		}
	}

	if ( skill == 19 || skill == 20 || skill == 21 || skill == 22 || skill == 23 ||
		 skill == AT_MSKILL_DK_CYCLONE1 || skill == AT_MSKILL_DK_SLASH1 || skill == AT_MSKILL_DK_FALLINGSLASH1 ||
		 skill == AT_MSKILL_DK_LUNGE1 || skill == AT_MSKILL_MG_CYCLONE1 ||
		 skill == AT_SKILL_RAGEFIGHTER_WEAPON1 || skill == AT_SKILL_RAGEFIGHTER_WEAPON2 ||
		 skill == AT_SKILL_RAGEFIGHTER_CHAINDRIVE || skill == AT_SKILL_PHOENIXSHOT )
	{
		gObjAddMsgSendDelay(lpTargetObj, 2, lpObj->m_Index, 150, 0);
	}
	else if ( skill == AT_SKILL_THUNDER || skill == AT_MSKILL_DW_LIGHTNING1 || skill == AT_MSKILL_MG_LIGHTNING1 )	// Lightning
	{
		if ( retResistance(lpTargetObj, R_LIGHTNING) == 0 )
		{
			gObjAddMsgSendDelay(lpTargetObj, 2, lpObj->m_Index, 150, 0);
			return TRUE;
		}

		return FALSE;
	}
	else if ( skill == AT_SKILL_POISON || skill == AT_MSKILL_DW_POISON1 )
	{
		if ( gObjCheckUsedBuffEffect(lpTargetObj, BUFF_POISON) == FALSE)
		{
			if( retResistance(lpTargetObj,R_POISON) == FALSE )
			{
				lpTargetObj->lpAttackObj = lpObj;
				gObjAddBuffEffect(lpTargetObj,BUFF_POISON,ADD_OPTION_REDUCE_HPPERC,3,0,0,20);
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else if ( skill == AT_SKILL_EXPPOISON || skill == AT_MSKILL_DW_DECAY1 ) // Death Poison
	{
		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_POISON) == FALSE)
		{
			if( retResistance(lpTargetObj,R_POISON) == FALSE )
			{
				lpTargetObj->lpAttackObj = lpObj;
				gObjAddBuffEffect(lpTargetObj,BUFF_POISON,ADD_OPTION_REDUCE_HPPERC,3,0,0,10);
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else if ( skill == AT_SKILL_SLOW || skill == AT_MSKILL_DW_ICE1 ||
			  skill == AT_SKILL_EXPICE || skill == AT_MSKILL_MG_ICE1 ||
			  skill == AT_MSKILL_MG_ICE2 ||
			  skill == AT_MSKILL_DW_ICESTORM1 || skill == AT_MSKILL_DW_ICESTORM2 )
	{
		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_ICE) == FALSE)
		{
			if( retResistance(lpTargetObj,R_ICE) == FALSE )
			{
				lpTargetObj->lpAttackObj = lpObj;
				lpTargetObj->DelayActionTime = 800;
				lpTargetObj->DelayLevel = 1;
				gObjAddBuffEffect(lpTargetObj,BUFF_ICE,ADD_OPTION_REDUCEMOVEMENT,0,0,0,10);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else if ( skill == AT_SKILL_ELFHARDEN || skill == AT_MSKILL_FE_ICEARROW1 )
	{
		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_ICEARROW) == FALSE)
		{
			if( retResistance(lpTargetObj,R_ICE) == FALSE )
			{
				BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(skill);

				if( !lpInfo )
				{
					return 0;
				}

				gObjAddBuffEffect(lpTargetObj,BUFF_ICEARROW,0,0,0,0,lpInfo->m_Arg1);
				lpTargetObj->PathCount = 0;
				lpTargetObj->PathStartEnd = 0;

				gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else if( skill == AT_MSKILL_FE_POISONARROW || skill == AT_MSKILL_FE_POISONARROW1 )
	{
		if( gObjCheckUsedBuffEffect(lpTargetObj,BUFF__POISON) == FALSE )
		{
			lpTargetObj->lpAttackObj = lpObj;
			gObjAddBuffEffect(lpTargetObj,BUFF__POISON,ADD_OPTION_REDUCE_HPPERC,3,0,0,10);
			return TRUE;
		}
	}
	return TRUE;
}



#include "SoulSystem.h"

BOOL CObjBaseAttack::MissCheck(LPOBJ lpObj, LPOBJ lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE byBarrageCount)
{
#if(__NOVUS__==1)
	if( lpObj->Type == OBJ_MONSTER && lpTargetObj->Type == OBJ_USER )
	{
		if( g_SoulSystem.IsMap(lpTargetObj->MapNumber) && !g_SoulSystem.WithSoul(lpTargetObj) )
		{
			LogAddC(2, "[DEBUG] [SoulSystem] [%s] Miss ignored", lpTargetObj->Name);
			return 1;
		}
	}
#endif
	__try
	{
		int iAttackRate = 0;
		int iDefenseRate = lpTargetObj->m_SuccessfulBlocking;

		int iMSBDamage = 0;

		if ( IT_MAP_RANGE(lpTargetObj->MapNumber) &&
			 g_IllusionTemple.GetState(lpTargetObj->MapNumber) == IT_STATE_PLAYING)
		{
			if( lpTargetObj->Type == OBJ_USER )
			{
				if( g_IllusionTemple.Is1stSkillActive(lpTargetObj->iTempleIndex,lpTargetObj->MapNumber) )
				{
					GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
					return FALSE;
				}
			}
			
			if( lpObj->PartyNumber == lpTargetObj->PartyNumber )
			{
				GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
				return FALSE;
			}
		}	

		if ( lpObj->Type == OBJ_USER )	// Miss for Uses
		{
			int Level = lpObj->Level + lpObj->MasterLevel;

			if ( lpObj->Class == CLASS_DARKLORD ) // DL
			{
				iAttackRate = Level * 5 + ((lpObj->Dexterity + lpObj->AddDexterity) * 6) / 2 + ( lpObj->Strength + lpObj->AddStrength) / 4  + ((lpObj->Leadership + lpObj->AddLeadership)/10);
				iAttackRate += iAttackRate * lpObj->SetOpImproveSuccessAttackRate / 100;
			}
			else if( lpObj->Class == CLASS_FIGHTER )
			{
				iAttackRate = Level * 3 + ((lpObj->Dexterity + lpObj->AddDexterity) * 5) / 4 + ( lpObj->Strength + lpObj->AddStrength) / 6 ;
				iAttackRate += iAttackRate * lpObj->SetOpImproveSuccessAttackRate / 100;
			}
			else
			{
				iAttackRate = Level * 5 + ((lpObj->Dexterity + lpObj->AddDexterity) * 3) / 2 + ( lpObj->Strength + lpObj->AddStrength) / 4 ;
				iAttackRate += iAttackRate * lpObj->SetOpImproveSuccessAttackRate / 100;
			}
		}
		else	// Miss for Monsters
		{
			iAttackRate = lpObj->m_AttackRating;
		}

		iAttackRate += lpObj->m_MPSkillOpt.MpsAttackSuccessRate;

		if ( iAttackRate < iDefenseRate )
		{
			bAllMiss = TRUE;
		}

		if ( bAllMiss != FALSE )	// When All Miss
		{
			if ( (rand()%100) >= 5 )
			{
				if( lpObj->Class == CLASS_FIGHTER && byBarrageCount != 0 )
				{
					if( skill == AT_SKILL_RAGEFIGHTER_WEAPON2 ||
						skill == AT_SKILL_RAGEFIGHTER_DARKSIDE ||
						skill == AT_MSKILL_RF_UPERCUT1 ||
						skill == AT_MSKILL_RF_UPERCUT2 ||
						skill == AT_MSKILL_RF_DARKSIDE1 ||
						skill == AT_MSKILL_RF_DARKSIDE2 )
					{
						if( (byBarrageCount%2) == 0 )
						{
							iMSBDamage |= 32;
						}
						else
						{
							iMSBDamage |= 16;
						}
					}
					else
					{
						if( (byBarrageCount%4) == 0 )
						{
							iMSBDamage |= 32;
						}
						else
						{
							iMSBDamage |= 16;
						}
					}
				}

				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, iMSBDamage, 0);

				if (magicsend != 0 )
				{
					GCMagicAttackNumberSend(lpObj, skill, lpTargetObj->m_Index, skillSuccess);
				}

				return FALSE;
			}
		}
		else	// if the is a chance  ot hit the target
		{
			if ( (rand()%iAttackRate) < iDefenseRate)
			{
				if( lpObj->Class == CLASS_FIGHTER && byBarrageCount != 0 )
				{
					if( skill == AT_SKILL_RAGEFIGHTER_WEAPON2 ||
						skill == AT_SKILL_RAGEFIGHTER_DARKSIDE ||
						skill == AT_MSKILL_RF_UPERCUT1 ||
						skill == AT_MSKILL_RF_UPERCUT2 ||
						skill == AT_MSKILL_RF_DARKSIDE1 ||
						skill == AT_MSKILL_RF_DARKSIDE2 )
					{
						if( (byBarrageCount%2) == 0 )
						{
							iMSBDamage |= 32;
						}
						else
						{
							iMSBDamage |= 16;
						}
					}
					else
					{
						if( (byBarrageCount%4) == 0 )
						{
							iMSBDamage |= 32;
						}
						else
						{
							iMSBDamage |= 16;
						}
					}
				}

				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, iMSBDamage, 0);

				if ( magicsend != 0 )
				{
					GCMagicAttackNumberSend(lpObj, skill, lpTargetObj->m_Index,  skillSuccess);
				}

				return FALSE;
			}
		}
	}
	__except (lpObj->Level=1,-1)
	{
		LogAdd(lMsg.Get(MSGGET(2, 12)), lpObj->Name);
		return FALSE;
	}

	return TRUE;
}


BOOL CObjBaseAttack::MissCheckPvP(LPOBJ lpObj , LPOBJ lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL & bAllMiss, BYTE byBarrageCount)
{
	float iAttackRate = 0;
	float iDefenseRate = 0;
	int iAttackSuccessRate = 0;

	if ( IT_MAP_RANGE(lpTargetObj->MapNumber) &&
			 g_IllusionTemple.GetState(lpTargetObj->MapNumber) == IT_STATE_PLAYING)
	{
		if( lpTargetObj->Type == OBJ_USER )
		{
			if( g_IllusionTemple.Is1stSkillActive(lpTargetObj->iTempleIndex,lpTargetObj->MapNumber) )
			{
				GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
				return FALSE;
			}
		}
		
		if( lpObj->PartyNumber == lpTargetObj->PartyNumber )
		{
			GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
			return FALSE;
		}
	}	
	
	int Level = lpObj->Level + lpObj->MasterLevel;

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	iAttackRate = g_ClassCalc.GetAttackRate(lpObj);
#else
	if( lpObj->Class == CLASS_KNIGHT )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 45) / 10);
	}
	else if ( lpObj->Class == CLASS_DARKLORD )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 4 ));
	}
	else if ( lpObj->Class == CLASS_ELF )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 6 ) / 10);
	}
	else if ( lpObj->Class == CLASS_MAGUMSA )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 35 ) / 10);
	}
	else if ( lpObj->Class == CLASS_WIZARD )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 4 ));
	}
	else if( lpObj->Class == CLASS_SUMMONER )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 35 ) / 10);
	}
	else if( lpObj->Class == CLASS_FIGHTER )
	{
		iAttackRate = (float)(Level * 2.6f + ( lpObj->Dexterity * 36 ) / 10);
	}
#endif

	int TargetLevel = lpTargetObj->Level + lpTargetObj->MasterLevel;

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	iDefenseRate = g_ClassCalc.GetDefenseRate(lpTargetObj);
#else
	if ( lpTargetObj->Class == CLASS_KNIGHT )
	{
		iDefenseRate = (float)(TargetLevel * 2 + lpTargetObj->Dexterity / 2);	// #formula
	}
	else if ( lpTargetObj->Class == CLASS_DARKLORD )
	{
		iDefenseRate = (float)(TargetLevel * 2 + lpTargetObj->Dexterity / 2);	// #formula
	}
	else if ( lpTargetObj->Class == CLASS_ELF )
	{
		iDefenseRate = (float)(TargetLevel * 2 + lpTargetObj->Dexterity / 10);	// #formula
	}
	else if ( lpTargetObj->Class == CLASS_MAGUMSA )
	{
		iDefenseRate = (float)(TargetLevel * 2 + lpTargetObj->Dexterity / 4);	// #formula
	}
	else if ( lpTargetObj->Class == CLASS_WIZARD )
	{
		iDefenseRate = (float)(TargetLevel * 2 + lpTargetObj->Dexterity / 4);	// #formula
	}
	else if ( lpTargetObj->Class == CLASS_SUMMONER )
	{
		iDefenseRate = (float)(TargetLevel * 2 + lpTargetObj->Dexterity / 2);	// #formula
	}
	else if ( lpTargetObj->Class == CLASS_FIGHTER )
	{
		iDefenseRate = (float)(TargetLevel*1.5f + lpTargetObj->Dexterity / 5);
	}
#endif

#if (ENABLE_CUSTOM_PVPBALANCE == 1)
	g_PvPBalance.modifyAttackRate(lpObj->m_Index, lpTargetObj->m_Index, iAttackRate);
	g_PvPBalance.modifyDefenseRate(lpObj->m_Index, lpTargetObj->m_Index, iDefenseRate);
#endif

	if ( iAttackRate <= 0.0f || iDefenseRate <= 0.0f || lpObj->Level <= 0 || lpTargetObj->Level <= 0 )
		return FALSE;

	int defdecrate = gObjGetTotalValueOfEffect(lpTargetObj,ADD_OPTION_DECDEFRATE);

	if( defdecrate )	{
		iDefenseRate -= iDefenseRate*defdecrate/100;
	}

	int attdecrate = gObjGetTotalValueOfEffect(lpTargetObj,ADD_OPTION_DECATTACKRATE);

	if( attdecrate )	{
		if( attdecrate > 100 )
			attdecrate = 100;
		iAttackRate -= iAttackRate*attdecrate/100;
	}

	iAttackRate += lpObj->m_ItemOptionExFor380.OpAddAttackSuccessRatePVP;
	iDefenseRate += lpTargetObj->m_ItemOptionExFor380.OpAddDefenseSuccessRatePvP;

	iAttackRate += lpObj->m_JewelOfHarmonyEffect.HJOpAddAttackSuccessRatePVP;
	iDefenseRate += lpTargetObj->m_JewelOfHarmonyEffect.HJOpAddDefenseSuccessRatePvP;

	iAttackRate += lpObj->m_MPSkillOpt.MpsPVPAttackDmgRate;
	iDefenseRate += lpTargetObj->m_MPSkillOpt.MpsPVPBlockingRate;

	float iExpressionA = ( iAttackRate * 10000.0f ) / ( iAttackRate + iDefenseRate );	// #formula
	float iExpressionB = ( Level * 10000.0f ) / ( Level + TargetLevel );	// #formula

	iExpressionA /= 10000.0f;
	iExpressionB /= 10000.0f;

	iAttackSuccessRate = int(100.0f * iExpressionA * g_fSuccessAttackRateOption * iExpressionB);

	if ( (TargetLevel - Level) >= 100 )
	{
		iAttackSuccessRate -= 5;
	}
	else if ( (TargetLevel - Level ) >= 200 )
	{
		iAttackSuccessRate -= 10;
	}
	else if ( (TargetLevel - Level) >= 300 )
	{
		iAttackSuccessRate -= 15;
	}

	DWORD dwRate = rand() % 100;

	if ( dwRate > (DWORD)iAttackSuccessRate )
	{
		int MsgDamage = 0;

		if( lpObj->Class == CLASS_FIGHTER && byBarrageCount != 0 )
		{
			if( skill == AT_SKILL_RAGEFIGHTER_WEAPON2 ||
				skill == AT_SKILL_RAGEFIGHTER_DARKSIDE ||
				skill == AT_MSKILL_RF_UPERCUT1 ||
				skill == AT_MSKILL_RF_UPERCUT2 ||
				skill == AT_MSKILL_RF_DARKSIDE1 ||
				skill == AT_MSKILL_RF_DARKSIDE2 )
			{
				if( (byBarrageCount%2) == 0 )
				{
					MsgDamage |= 32;
				}
				else
				{
					MsgDamage |= 16;
				}
			}
			else
			{
				if( (byBarrageCount%4) == 0 )
				{
					MsgDamage |= 32;
				}
				else
				{
					MsgDamage |= 16;
				}
			}
		}

		GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, MsgDamage, 0);

		if ( g_bShieldComboMissOptionOn == TRUE )
		{
			if ( lpObj->comboSkill.ProgressIndex >= 0 )
			{
				LogAddTD("[Shield] ComboSkill Cancel! [%s][%s]",
					lpObj->AccountID, lpObj->Name);

				lpObj->comboSkill.Init();
			}
		}

		return FALSE;
	}

	return TRUE;
}





int  CObjBaseAttack::GetTargetDefense(LPOBJ lpObj, LPOBJ lpTargetObj, int& MsgDamage)
{
	int targetdefense = lpTargetObj->m_Defense;

	if( lpTargetObj->m_MPSkillOpt.MpsWingDefenseIncrease > 0.0f )
	{
		targetdefense += lpTargetObj->m_MPSkillOpt.MpsWingDefenseIncrease/2.0f;
	}

	if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
	{
		targetdefense += lpTargetObj->m_ItemOptionExFor380.OpAddDefense / 2;
	}

#if (ENABLETEST_MUUN == 1)
	int nMuunItemEffectValue = 0;
	if (g_CMuunSystem.GetMuunItemValueOfOptType(lpTargetObj, 2, &nMuunItemEffectValue, 0)) {
		targetdefense += 10 * nMuunItemEffectValue / 20;
	}
#endif

	int ReduceDefense = 0;

	ReduceDefense = gObjGetTotalValueOfEffect(lpTargetObj,ADD_OPTION_DEFENSEDOWN);
	targetdefense -= (targetdefense *  ReduceDefense)/100;
	
	if ( lpTargetObj->m_MonsterSkillElementInfo.m_iSkillElementDefenseTime > 0 )
	{
		targetdefense += lpTargetObj->m_MonsterSkillElementInfo.m_iSkillElementDefense;

		if ( targetdefense <0 )
			targetdefense = 0;

	}

	int percentdamage = 0;

	if ( lpObj->pInventory[7].IsItem() != FALSE )
	{
		percentdamage = lpObj->pInventory[7].IsWingOpGetOnePercentDamage();
		percentdamage += lpObj->pInventory[7].Is25WingOpGetOnePercentDamage();
		percentdamage += lpObj->pInventory[7].Is3rdWingOpGetOnePercentDamage();

		if( lpObj->pInventory[7].m_Type == ITEMGET(12,266) &&
			lpObj->pInventory[7].m_Durability > 0.0f &&
			(lpObj->pInventory[7].m_NewOption&1) == 1 )
		{
			percentdamage += 5;
		}

		if( lpObj->pInventory[7].m_Type == ITEMGET(12,267) &&
			lpObj->pInventory[7].m_Durability > 0.0f &&
			(lpObj->pInventory[7].m_NewOption&2) == 2 )
		{
			percentdamage += 4;
		}

#if (CUSTOM_WINGS == 1)
		if( lpObj->pInventory[7].m_Durability > 0 )
		{
			if( lpObj->pInventory[7].m_Type >= ITEMGET(12,440) &&
				lpObj->pInventory[7].m_Type <= ITEMGET(12,445) )
			{
				if( (lpObj->pInventory[7].m_NewOption & 1) == 1 )
				{
					percentdamage = 8;
				}
			}
		}
#endif
	}

	if ( gObjCheckUsedBuffEffect(lpObj,BUFF_IGNORE_DEFENSE) )
	{
		int nEffectValue = 0;
		gObjGetValueOfBuffIndex(lpObj,BUFF_IGNORE_DEFENSE,&nEffectValue,0);
		percentdamage += nEffectValue;
	}

	percentdamage += lpObj->SetOpIgnoreDefense;

	if ( lpObj->m_MPSkillOpt.MpsIgnoreDef > 0.0f )
	{
		percentdamage += lpObj->m_MPSkillOpt.MpsIgnoreDef;
	}

	if ( percentdamage != 0)
	{
		if ( (rand()%100) <= percentdamage)
		{
			targetdefense = 0;
			MsgDamage = 1;
		}
	}



	return targetdefense;
}







int CObjBaseAttack::GetPartyMemberCount(LPOBJ lpObj)
{
	LPOBJ lpPartyObj;
	int partynum = lpObj->PartyNumber;

	if ( OBJMAX_RANGE(partynum) == FALSE )
	{
		LogAdd("error : DecPartyMemberHPandMP %s %d", __FILE__, __LINE__);
		return 0;
	}

	int partycount = gParty.m_PartyS[partynum].Count;
	int retcount = 0;

	for ( int n=0;n<MAX_USER_IN_PARTY;n++)
	{
		int memberindex = gParty.m_PartyS[partynum].Number[n];

		if ( memberindex >= 0 )
		{
			lpPartyObj = &gObj[memberindex];

			if ( lpObj->MapNumber == lpPartyObj->MapNumber )
			{
				int dis = gObjCalDistance(lpObj, &gObj[memberindex]);
				
				if ( dis < MAX_PARTY_DISTANCE_EFFECT )
				{
					retcount++;
				}
			}
		}
	}

	return retcount;
}
