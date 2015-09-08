#include "stdafx.h"
#include "DarkSpirit.h"
#include "MapClass.h"
#include "GameMain.h"
#include "protocol.h"
#include "BattleSoccerManager.h"
#include "gObjMonster.h"
#include "ObjAttack.h"
#include "ChaosCastle.h"
#include "..\common\winutil.h"
#include "CastleSiege.h"
#include "Crywolf.h"
#include "BuffEffectSlot.h"
#include "IllusionTempleEvent.h"
#include "ImperialGuardian.h"
#include "ClassCalc.h"

#if (ENABLETEST_ARCA == 1)
#include "ArcaBattle.h"
#endif

#if (ENABLETEST_ACHERON == 1)
#include "AcheronEvent.h"
#endif

CDarkSpirit gDarkSpirit[OBJMAX];





CDarkSpirit::CDarkSpirit() : m_ActionMode(PetItem_Mode_Normal)
{
	this->Init();
}





CDarkSpirit::~CDarkSpirit()
{
	return;
}





void CDarkSpirit::Init()
{
	this->m_AttackDamageMin = 0;
	this->m_AttackDamageMax = 0;
	this->m_AttackSpeed = 0;
	this->m_SuccessAttackRate = 0;
	this->m_dwLastAttackTime = 0;
	this->m_iMasterIndex = -1;
	this->m_iTargetIndex = -1;
	this->m_pPetItem = NULL;
}





void CDarkSpirit::Run()
{
	if ( this->m_iMasterIndex == -1 )
	{
		return;
	}

	LPOBJ lpObj = &gObj[this->m_iMasterIndex];

	if ( lpObj->Class != 4 ) // dñl
	{
		return;
	}

	if ( this->m_pPetItem == NULL )
	{
		return;
	}

	if ( this->m_pPetItem->m_Durability <= 0.0f )
	{
		return;
	}

	BYTE attr = MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y);

	if ( (attr&1) != 0 )
	{
		return;
	}

	if ( this->m_dwLastAttackTime > GetTickCount() )
	{
		return;
	}

	this->m_dwLastAttackTime = ( GetTickCount() + 1500 ) - ( this->m_AttackSpeed * 10 );

	switch ( this->m_ActionMode )
	{
		case PetItem_Mode_Normal:
			this->ModeNormal();
			break;

		case PetItem_Mode_Attack_Random:
			this->ModeAttackRandom();
			break;

		case PetItem_Mode_Attack_WithMaster:
			this->ModeAttackWithMaster();
			break;

		case PetItem_Mode_Attack_Target:
			this->ModeAttakTarget();
			break;
	}
}





void CDarkSpirit::ModeNormal()
{
	LPOBJ lpObj = &gObj[this->m_iMasterIndex];
}




void CDarkSpirit::ModeAttackRandom()
{
	LPOBJ lpObj = &gObj[this->m_iMasterIndex];
	int tObjNum;
	int count = 0;
	int FindObj[MAX_VIEWPORT];
	int FindObjCount = 0;
	int dis;
	int DuelIndex = lpObj->m_iDuelUser;
	BOOL EnableAttack;
	int criticaldamage = 0;
	int iActionType = 0;

	if ( (rand()%100) < (this->m_iCriticalSuccessRate+lpObj->m_MPSkillOpt.MpsDarkSpiritStr2) )
	{
		criticaldamage = 1;
		iActionType = 1;
	}

	if( (rand()%100) < (this->m_iExcelentSuccessRate+lpObj->m_MPSkillOpt.MpsDarkSpiritStr3) )
	{
		criticaldamage = 2;
		iActionType = 1;
	}

	while ( true )
	{
		if ( lpObj->VpPlayer2[count].state != 0 )
		{
			if ( lpObj->VpPlayer2[count].type == OBJ_MONSTER )
			{
				tObjNum = lpObj->VpPlayer2[count].number;

				if ( tObjNum >= 0 )
				{
					EnableAttack = FALSE;

					if ( gObj[tObjNum].Life > 0.0f && (gObj[tObjNum].Class < 100 || gObj[tObjNum].Class  > 110 ) )
					{
						if ( lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
						{
							EnableAttack = TRUE;
						}
						else if ( DuelIndex == tObjNum )
						{
							EnableAttack = TRUE;
						}
						else
						{
							int lc85 = gObj[tObjNum].m_Index;

							if ( gObj[tObjNum].Type == OBJ_MONSTER )
							{
								if ( gObj[tObjNum].m_RecallMon >= 0 )
								{
									lc85 = gObj[gObj[tObjNum].m_RecallMon].m_Index; 
								}
							}

							if ( gObjTargetGuildWarCheck(lpObj, &gObj[lc85]) == TRUE )
							{
								EnableAttack = TRUE;
							}

							if ( IF_MAP_RANGE(gObj[tObjNum].MapNumber) )
							{
								if( gObj[tObjNum].Live && g_ImperialGuardian.IsAttackAbleMonster(gObj[tObjNum].m_Index) )
								{
									EnableAttack = TRUE;
								}
								else
								{
									EnableAttack = FALSE;
								}
							}
						}

						if ( EnableAttack != FALSE )
						{
							if ( lpObj->MapNumber == gObj[tObjNum].MapNumber )
							{
								dis = gObjCalDistance(lpObj, &gObj[tObjNum]);

								if ( dis < RAVEN_ATTACK_DISTANCE-2 )
								{
									FindObj[FindObjCount] = tObjNum;
									FindObjCount++;
								}
							}
						}
					}
				}
			}
		}

		count++;

		if ( count > MAX_VIEWPORT-1 )
		{
			break;
		}
	}

	if ( FindObjCount != 0 )
	{
		if ( criticaldamage != 0 )
		{
			int target = FindObj[rand()%FindObjCount];
			this->SendAttackMsg( lpObj->m_Index, target, criticaldamage, iActionType);
		}
		else
		{
			int target = FindObj[rand()%FindObjCount];
			this->RangeAttack( lpObj->m_Index, target);
		}
	}
}






void CDarkSpirit::ModeAttackWithMaster()
{
	LPOBJ lpObj = &gObj[this->m_iMasterIndex];
	int count = 0;
	int dis;

	if ( this->m_iTargetIndex == -1 )
	{
		return;
	}

	if ( lpObj->MapNumber == gObj[this->m_iTargetIndex].MapNumber )
	{
		if ( gObj[this->m_iTargetIndex].Life > 0.0f )
		{
			dis = gObjCalDistance(lpObj, &gObj[this->m_iTargetIndex]);

			if ( dis < RAVEN_ATTACK_DISTANCE )
			{
				int criticaldamage = 0;
				int iActionType = 0;

				if ( (rand()%100) < (this->m_iCriticalSuccessRate+lpObj->m_MPSkillOpt.MpsDarkSpiritStr2) )
				{
					criticaldamage = 1;
					iActionType = 1;
				}

				if( (rand()%100) < (this->m_iExcelentSuccessRate+lpObj->m_MPSkillOpt.MpsDarkSpiritStr3) )
				{
					criticaldamage = 2;
					iActionType = 1;
				}

				if ( criticaldamage != FALSE )
				{
					this->SendAttackMsg(lpObj->m_Index, this->m_iTargetIndex, criticaldamage, iActionType);
				}
				else
				{
					this->RangeAttack(lpObj->m_Index, this->m_iTargetIndex);
				}
			}
		}
		else
		{
			this->ReSetTarget(this->m_iTargetIndex);
		}
	}
}






void CDarkSpirit::ModeAttakTarget()
{
	LPOBJ lpObj = &gObj[this->m_iMasterIndex];
	int count = 0;
	int dis;

	if ( this->m_iTargetIndex == -1 )
	{
		return;
	}

	if ( lpObj->MapNumber == gObj[this->m_iTargetIndex].MapNumber )
	{
		if ( gObj[this->m_iTargetIndex].Life > 0.0f )
		{
			dis = gObjCalDistance(lpObj, &gObj[this->m_iTargetIndex]);

			if ( dis < RAVEN_ATTACK_DISTANCE )
			{
				int criticaldamage = TRUE;
				int msg = 1;	// Attack with picada

				if ( criticaldamage != FALSE )
				{
					this->SendAttackMsg(lpObj->m_Index, this->m_iTargetIndex, criticaldamage, msg);
				}
				else
				{
					this->RangeAttack(lpObj->m_Index, this->m_iTargetIndex);
				}
			}
		}
		else
		{
			this->ReSetTarget(this->m_iTargetIndex);
		}
	}
}






void CDarkSpirit::RangeAttack(int aIndex, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int StartDis = 1;
	int tObjNum;
	int count = 0;
	int loopcount = 0;
	int attackcheck;
	int EnableAttack;
	int HitCount = 0;

	this->SendAttackMsg(lpObj->m_Index, aTargetIndex, 0, 0);

	while ( true )
	{
		if ( lpObj->VpPlayer2[count].state != 0 )
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if ( tObjNum >= 0 && aTargetIndex != tObjNum )
			{
				EnableAttack = FALSE;
				
				if ( lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}
				else if ( tObjNum == aTargetIndex )
				{
					EnableAttack = TRUE;
				}
				else
				{
					int lc10 = gObj[tObjNum].m_Index;

					if ( gObj[tObjNum].Type == OBJ_MONSTER )
					{
						if ( gObj[tObjNum].m_RecallMon >= 0 )
						{
							lc10 = gObj[gObj[tObjNum].m_RecallMon].m_Index;
						}
					}

					if ( gObjTargetGuildWarCheck(lpObj, &gObj[lc10]) == TRUE )
					{
						EnableAttack = TRUE;
					}
				}

				if ( EnableAttack != FALSE )
				{
					attackcheck = FALSE;

					if ( loopcount == 0 )
					{
						attackcheck = TRUE;
					}
					else if ( (rand()%3) == 0 )
					{
						attackcheck = TRUE;
					}

					if ( attackcheck != FALSE )
					{
						if ( gObjCalDistance(&gObj[aTargetIndex], &gObj[tObjNum]) < RAVEN_ATTACK_DISTANCE-3 )
						{
							this->SendAttackMsg(lpObj->m_Index, tObjNum, 2, 0);
							HitCount++;

							if ( HitCount > 3 )
							{
								break;
							}
						}
					}
				}
			}				
		}

		count++;

		if ( count > MAX_VIEWPORT-1 )
		{
			break;
		}
	}
}



struct PMSG_PET_ITEM_ATTACK_COMMAND
{
	PBMSG_HEAD h;	// C1:A8
	BYTE PetType;	//	3
	BYTE SkillType;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
	BYTE TargetNumberH;	// 7
	BYTE TargetNumberL;	// 8
};


void CDarkSpirit::SendAttackMsg(int aIndex, int aTargetIndex, int criticaldamage, int iActionType)
{
	PMSG_PET_ITEM_ATTACK_COMMAND pMsg;

	if ( criticaldamage != 3 )
	{
		pMsg.PetType = 0;
		pMsg.SkillType = (criticaldamage)? 0 : 1;
		pMsg.NumberH = SET_NUMBERH(aIndex);
		pMsg.NumberL = SET_NUMBERL(aIndex);
		pMsg.TargetNumberH = SET_NUMBERH(aTargetIndex);
		pMsg.TargetNumberL = SET_NUMBERL(aTargetIndex);
		pMsg.h.set((LPBYTE)&pMsg, 0xA8, sizeof(pMsg));
		
		DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
		MsgSendV2(&gObj[aIndex], (UCHAR*)&pMsg, pMsg.h.size);

		if ( iActionType == 1 )
		{
			criticaldamage = 0;
			LPOBJ lpObj = &gObj[aIndex];

			if ( (rand()%100) < (this->m_iCriticalSuccessRate+lpObj->m_MPSkillOpt.MpsDarkSpiritStr2) )
			{
				criticaldamage = 1;
			}

			if( (rand()%100) < (this->m_iExcelentSuccessRate+lpObj->m_MPSkillOpt.MpsDarkSpiritStr3) )
			{
				criticaldamage = 2;
			}
		}
	}
	else
	{
		criticaldamage = 0;
	}

	gObjAddAttackProcMsgSendDelay(&gObj[aIndex], 51, aTargetIndex, 600, criticaldamage, iActionType);
}





void CDarkSpirit::SetTarget(int aTargetIndex)
{
	if ( this->m_iMasterIndex == -1 )
	{
		return;
	}

	LPOBJ lpObj = &gObj[this->m_iMasterIndex];

	if ( lpObj->Class != 4 )	// DL
	{
		return;
	}

	this->m_iTargetIndex = aTargetIndex;
}





void CDarkSpirit::ReSetTarget(int aTargetIndex)
{
	if ( this->m_iMasterIndex == -1 )
	{
		return;
	}

	LPOBJ lpObj = &gObj[this->m_iMasterIndex];

	if ( lpObj->Class != 4 )	// DL
	{
		return;
	}

	if ( aTargetIndex == this->m_iTargetIndex )
	{
		this->m_iTargetIndex = -1;

		if ( this->m_ActionMode == PetItem_Mode_Attack_Target )
		{
			this->SetMode(PetItem_Mode_Normal, -1);
		}
	}
}




struct PMSG_SEND_PET_ITEM_COMMAND
{
	PBMSG_HEAD h;	// C1:A7
	BYTE PetType;	// 3
	BYTE Command;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
};

void CDarkSpirit::SetMode(ePetItem_Mode mode, int iTargetindex)
{
	if ( this->m_iMasterIndex == -1 )
	{
		return;
	}

	LPOBJ lpObj = &gObj[this->m_iMasterIndex];

	if ( lpObj->Class != CLASS_DARKLORD )	// DL
	{
		return;
	}

	if ( mode == PetItem_Mode_Attack_Target || this->m_ActionMode != mode)
	{
		if ( mode == PetItem_Mode_Attack_Target )
		{
			if ( OBJMAX_RANGE(iTargetindex) != FALSE )
			{
				this->m_iTargetIndex = iTargetindex;
			}
			else
			{
				return;
			}
		}

		this->m_ActionMode = mode;
		PMSG_SEND_PET_ITEM_COMMAND pMsg;
		pMsg.h.set((LPBYTE)&pMsg, 0xA7, sizeof(pMsg));
		pMsg.PetType = 0;
		pMsg.Command = mode;
		pMsg.NumberH = SET_NUMBERH(this->m_iTargetIndex);
		pMsg.NumberL = SET_NUMBERL(this->m_iTargetIndex);
		DataSend(this->m_iMasterIndex, (UCHAR*)&pMsg, sizeof(pMsg));
	}
}


void CDarkSpirit::Set(int aIndex, CItem * pPetItem)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->Class != 4 )	// DL
	{
		return;
	}

	BOOL bChange = FALSE;

	if ( this->m_iMasterIndex == -1 )
	{
		bChange = TRUE;
	}

	this->Init();

	if ( pPetItem->m_Type != ITEMGET(13,5) ) // Talon of Dark Raven
	{
		return;
	}

	if ( pPetItem->m_Durability <= 0.0f )
	{
		return;
	}

	int petitemlevel = pPetItem->m_PetItem_Level;
	int leadership = lpObj->Leadership + lpObj->AddLeadership;

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	this->m_AttackDamageMin			= petitemlevel * g_ClassCalc.GetPetAttackDamageMinFact() + leadership / g_ClassCalc.GetPetAttackDamageMinDiv() + g_ClassCalc.GetPetAttackDamageMinStart();
	this->m_AttackDamageMax			= petitemlevel * g_ClassCalc.GetPetAttackDamageMaxFact() + leadership / g_ClassCalc.GetPetAttackDamageMaxDiv() + g_ClassCalc.GetPetAttackDamageMaxStart();
	this->m_iCriticalSuccessRate	= (float)g_ClassCalc.GetPetCriticalAttackRate();
	this->m_AttackSpeed				= petitemlevel * g_ClassCalc.GetPetAttackSpeedFact() / g_ClassCalc.GetPetAttackSpeedDiv1() + leadership / g_ClassCalc.GetPetAttackSpeedDiv2() + g_ClassCalc.GetPetAttackSpeedStart();
	this->m_SuccessAttackRate		= petitemlevel + petitemlevel / g_ClassCalc.GetPetSuccessAttackRateDiv() + g_ClassCalc.GetPetSuccessAttackRateStart();
#else
	this->m_AttackDamageMin			= petitemlevel*15 + leadership/8 + 180;
	this->m_AttackDamageMax			= petitemlevel*15 + leadership/4 + 200;
	this->m_iCriticalSuccessRate	= 30.0f;
	this->m_AttackSpeed				= petitemlevel * 4 / 5 + leadership / 50 + 20;
	this->m_SuccessAttackRate		= petitemlevel + petitemlevel / 15 + 1000;
#endif

	this->m_AttackSpeed += lpObj->m_MPSkillOpt.MpsDarkSpiritStr4;
	this->m_iExcelentSuccessRate = 0;
	this->m_iMasterIndex = aIndex;
	this->m_pPetItem = pPetItem;

	if ( bChange != FALSE )
	{
		this->SetMode(PetItem_Mode_Normal, -1);
	}
}





BOOL CDarkSpirit::Attack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf * lpMagic, int criticaldamage, int iActionType)
{
	int skillSuccess = 0;
	LPOBJ lpCallObj;
	LPOBJ lpCallTargetObj;
	int MsgDamage = 0;
	int ManaChange = 0;
	int iTempShieldDamage = 0;
	int iTotalShieldDamage = 0;

	if ( (lpTargetObj->Authority&2) == 2 )
		return FALSE;

	if ( lpObj->MapNumber != lpTargetObj->MapNumber )
		return FALSE;

#if (__GENS_SYSTEM__ == 1)
	if (gGensSystem.GetFriendlyFire() && gGensSystem.GensMap(lpObj->MapNumber)) {
		if (lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER
			&& lpObj->m_GensInfo.btFamily == lpTargetObj->m_GensInfo.btFamily) {
			return false;
		}
	}
#endif

	if ( IF_MAP_RANGE(lpObj->MapNumber) && !g_ImperialGuardian.IsAttackAbleMonster(lpTargetObj->m_Index) )
		return FALSE;

	if ( lpObj->MapNumber == MAP_INDEX_PKFIELD && lpObj->m_iDuelUser == -1)
		return FALSE;
	
#if (GS_CASTLE == 1)
	if(g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_READY || g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_END)
	{
		if( ((lpTargetObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE) ? TRUE : FALSE) && lpTargetObj->Type == OBJ_MONSTER)
		{
			return FALSE;
		}
	}
#endif

	int skill = 0;
	
	if ( lpMagic )
		skill = lpMagic->m_Skill;

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
		}
	}

	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
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

			return TRUE;
		}
	}

	if ( lpObj->Type == OBJ_USER )
	{
		if ( !gObjIsConnected(lpObj))
		{
			return FALSE;
		}
	}

	if ( lpTargetObj->Type == OBJ_USER )
	{
		if ( !gObjIsConnected(lpTargetObj))
		{
			return FALSE;
		}
	}

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
		return FALSE;

#if (ENABLETEST_ARCA == 1)
	if ( g_ArcaBattle.IsArcaBattleServer() && !g_AcheronGuardianEvent.IsPlayStart()
      && !g_ArcaBattle.IsEnableAttackObelisk(*lpObj, lpTargetObj->Class))
      return 0;
#endif

#if (GS_CASTLE == 1)

	if ( g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE 
		&& lpObj->m_btCsJoinSide > 0
		&& lpObj->m_btCsJoinSide == lpTargetObj->m_btCsJoinSide)
	{
		return FALSE;
	}
#endif

	if ( this->CheckAttackArea(lpObj, lpTargetObj) == FALSE )
		return FALSE;

	lpCallObj = lpObj;
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

	int MSBFlag = 0;
	
	if ( criticaldamage == 1 )
	{
		MsgDamage = 3;
	}
	else if( criticaldamage == 2 )
	{
		MsgDamage = 2;
	}
	else
	{
		MsgDamage = 0;
	}

	BOOL bAllMiss = FALSE;
	int AttackDamage = 0;

	if ( g_ShieldSystemOn == TRUE )
	{
		if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
		{
			if ( !this->MissCheckPvP(lpObj, lpTargetObj, skill, skillSuccess, bAllMiss) )
			{
				return FALSE;
			}
		}
		else if ( !this->MissCheck(lpObj, lpTargetObj, skill, skillSuccess, bAllMiss) )
		{
			return FALSE;
		}

	}
	else if ( !this->MissCheck(lpObj, lpTargetObj, skill, skillSuccess, bAllMiss) )
	{
		return FALSE;
	}

	int targetdefense = this->GetTargetDefense(lpObj, lpTargetObj, MsgDamage);

	if( targetdefense > 0 )
	{
		float fpercent = rand()%100;
		if( fpercent < lpObj->m_MPSkillOpt.MpsDarkSpiritStr6 )
		{
			targetdefense = 0;
			MsgDamage = 1;
		}
	}

	AttackDamage = this->GetAttackDamage(lpObj, targetdefense, bIsOnDuel, criticaldamage,(criticaldamage==2) ? TRUE : FALSE);

	if ( criticaldamage == 0 && iActionType == 0 )
	{
		AttackDamage = (int)(AttackDamage/1.5f);
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

	int tlevel = lpObj->Level / 10;

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

	lpObj->pInventory[1].m_DurabilitySmall += AttackDamage * 2 / 100;

	int SmallDur = 800;

	if ( lpObj->pInventory[1].m_DurabilitySmall >= (SmallDur+lpObj->m_MPSkillOpt.MpsPetDurDownSpeed) )
	{
		lpObj->pInventory[1].m_DurabilitySmall = 0;
		lpObj->pInventory[1].m_Durability -= 1.0f;

		if ( lpObj->pInventory[1].m_Durability < 1.0f )
		{
			lpObj->pInventory[1].m_Durability = 0;
			int iPetExp = lpObj->pInventory[1].m_PetItem_Exp;

			if ( lpObj->pInventory[1].DecPetItemExp(10) )
			{
				LogAddTD("[%s][%s][PetItemExpDown] [%s] Level:[%d]Exp:[%d]DecExp[%d]",
					lpObj->AccountID, lpObj->Name, lpObj->pInventory[1].GetName(),
					lpObj->pInventory[1].m_PetItem_Level,
					lpObj->pInventory[1].m_PetItem_Exp, iPetExp - lpObj->pInventory[1].m_PetItem_Exp);

				this->Set(lpObj->m_Index, &lpObj->pInventory[1]);
				this->SendLevelmsg(lpObj->m_Index, 1, 0, 0xFF);
			}

			LogAddTD("[%s][%s] DarkSpirit Item is Broken because durability is exhausted [%d]",
				lpObj->AccountID, lpObj->Name, lpObj->pInventory[1].m_Number);
		}
		else
		{
			this->SendLevelmsg(lpObj->m_Index,1,0,0);
		}

		GCItemDurSend(lpObj->m_Index, 1, (BYTE)lpObj->pInventory[1].m_Durability, 0);
	}

	if ( gObjAngelSprite(lpTargetObj) == TRUE )
	{
		if ( AttackDamage > 1 )
		{
			float  damage = (AttackDamage * 8) / 10.0f;
			AttackDamage = (int)damage;
		}
	}

	if ( gObjWingSprite(lpTargetObj) == TRUE )
	{
		CItem * Wing = &lpTargetObj->pInventory[7];

		if ( AttackDamage > 1 )
		{
			if( Wing->m_Type == ITEMGET(12,36) ||
				Wing->m_Type == ITEMGET(12,37) ||
				Wing->m_Type == ITEMGET(12,38) ||
				Wing->m_Type == ITEMGET(12,39) )
			{
				float damage = AttackDamage * (61 - Wing->m_Level*2) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,262) )
			{
				float damage = AttackDamage * (87 - Wing->m_Level*2) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,263) )
			{
				float damage = AttackDamage * (70 - Wing->m_Level*2) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,264) || Wing->m_Type == ITEMGET(12,265) )
			{
				float damage = AttackDamage * (71 - Wing->m_Level*2) / 100.0f;
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
				else{
					damage = AttackDamage * (61 - Wing->m_Level*3) / 100.0f;
				}
				AttackDamage = (int)damage;
			}
#endif
			else if( Wing->m_Type == ITEMGET(12,40) )
			{
				float damage = AttackDamage * (76 - Wing->m_Level*2) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,41) )
			{
				float damage = AttackDamage * (88 - Wing->m_Level*2) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,42) )
			{
				float damage = AttackDamage * (75 - Wing->m_Level*2) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,43) )
			{
				float damage = AttackDamage * (61 - Wing->m_Level*2) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,49) )
			{
				float damage = AttackDamage * (90 - Wing->m_Level*2) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,50) )
			{
				float damage = AttackDamage * (76 - Wing->m_Level*2) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(13,30) )
			{
				float damage = AttackDamage * (90 - Wing->m_Level) / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,266) )
			{
				float damage = AttackDamage * 29 / 100.0f;
				AttackDamage = (int)damage;
			}
			else if( Wing->m_Type == ITEMGET(12,267) )
			{
				float damage = AttackDamage * (40 - Wing->m_Level) / 100.0f;
				AttackDamage = (int)damage;
			}
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

		GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life, 0xFF, 0, lpObj->iShield);
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
		BYTE btManashield = 0;

		if( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_MANASHIELD) == TRUE )
			btManashield = 1;
		else if( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_MANASHIELD2) )
			btManashield = 2;

		if ( btManashield && AttackDamage > 0 )
		{
			int EffectVal1 = 0;
			int EffectVal2 = 0;

			if( btManashield == 1 )
				gObjGetValueOfBuffIndex(lpTargetObj,BUFF_MANASHIELD,&EffectVal1,&EffectVal2);
			else if( btManashield == 2 )
				gObjGetValueOfBuffIndex(lpTargetObj,BUFF_MANASHIELD2,&EffectVal1,&EffectVal2);

			int replacemana = 0;

			if( EffectVal2 > 0 )
			{
				replacemana = ((WORD)lpTargetObj->Mana * EffectVal2) / 1000;
			}
			else
			{
				replacemana = ((WORD)lpTargetObj->Mana * 2) / 100;
			}

			if ( replacemana < lpTargetObj->Mana )
			{
				lpTargetObj->Mana -= replacemana;
				int decattackdamage = 0;
				
				if( EffectVal1 > 0 )
				{
					decattackdamage = AttackDamage * EffectVal1 / 100;
				}
				AttackDamage -= decattackdamage;
				ManaChange = TRUE;
			}
		}

		if ( g_ShieldSystemOn == FALSE )
		{
			if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
			{
				if ( CC_MAP_RANGE(lpObj->MapNumber ) && CC_MAP_RANGE(lpTargetObj->MapNumber) )
				{
					AttackDamage = AttackDamage * 50 / 100;
				}
			}
		}

#if (GS_CASTLE == 1)

		if ( g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE 
			&& lpObj->Type == OBJ_USER
			&& lpTargetObj->Type == OBJ_USER
			&& lpObj->MapNumber == MAP_INDEX_CASTLESIEGE
			&& lpTargetObj->MapNumber == MAP_INDEX_CASTLESIEGE)
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

		if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_MONSTER )
		{
			if ( lpTargetObj->Class == 283 )
			{
				if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_BLESS) ||
					 gObjCheckUsedBuffEffect(lpObj,BUFF_POT_SOUL) )
				{
					AttackDamage += (AttackDamage * 20) / 100;
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

					AttackDamage = AttackDamage * 20 / 100;
				}
			}

			if ( lpTargetObj->Class == 277 )
			{
				if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_BLESS) ||
					 gObjCheckUsedBuffEffect(lpObj,BUFF_POT_SOUL) )
				{
					AttackDamage += (AttackDamage * 20) / 100;
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

					AttackDamage = AttackDamage * 20 / 100;
				}
			}
		}

		if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
		{
			iTempShieldDamage = this->GetShieldDamage(lpObj, lpTargetObj, AttackDamage);
			lpTargetObj->iShield -= iTempShieldDamage;
			lpTargetObj->Life -= AttackDamage - iTempShieldDamage;
			iTotalShieldDamage += iTempShieldDamage;

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
			lpTargetObj->m_Agro.IncAgro(lpObj->m_Index, AttackDamage / 100);
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
			else if ( CC_MAP_RANGE(lpObj->MapNumber) || CC_MAP_RANGE(lpTargetObj->MapNumber) )
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
			if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				if( lpObj->m_btCsJoinSide > 0 )
				{
					selfdefense = FALSE;
				}
			}
#endif

			else if( IT_MAP_RANGE(lpObj->MapNumber) ||
				IT_MAP_RANGE(lpTargetObj->MapNumber) )
			{
				selfdefense = FALSE;
			}
			else if( lpObj->MapNumber == MAP_INDEX_VULCANUS || lpTargetObj->MapNumber == MAP_INDEX_VULCANUS )
			{
				selfdefense = FALSE;
			}
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
		}

		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_ICEARROW) == TRUE )
		{
			gObjRemoveBuffEffect(lpTargetObj,BUFF_ICEARROW);
			GCMagicCancelSend(lpTargetObj,51);
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

	if ( selfdefense == TRUE )
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

	if ( AttackDamage >= 5 )	// To make strong hit
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

	if ( lpObj->Type == OBJ_USER )
	{
		if ( lpObj->m_Change == 9 )
		{
			GCMagicAttackNumberSend(lpObj, 3, lpTargetObj->m_Index, 1);
		}
	}

	lpObj->m_Rest = 0;

	if ( AttackDamage > 0 )
	{
		int atd_reflect = (int)((float)AttackDamage * lpTargetObj->DamageReflect / 100.0f);

		if ( atd_reflect )
		{
			gObjAddMsgSendDelay(lpTargetObj, 10, lpObj->m_Index, 10, atd_reflect);
		}

		if ( (rand()%100) < lpObj->SetOpReflectionDamage )
		{
			gObjAddMsgSendDelay(lpTargetObj, 10, lpObj->m_Index, 10, AttackDamage);
		}

		AttackDamage -= iTotalShieldDamage;

		float fpercent = rand()%100;

		if ( fpercent < lpObj->m_MPSkillOpt.MpsDarkSpiritStr5 )
		{
			if ( lpTargetObj->Type == OBJ_USER )
			{
				iTempShieldDamage = this->GetShieldDamage(lpObj, lpTargetObj, AttackDamage);
				lpTargetObj->iShield -= iTempShieldDamage;
				lpTargetObj->Life -= AttackDamage - iTempShieldDamage;
				iTotalShieldDamage += iTempShieldDamage;
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
		
		gObjLifeCheck(lpTargetObj, lpObj, AttackDamage, 0, MSBFlag, MsgDamage, skill, iTotalShieldDamage);
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

	return TRUE;
}





int  CDarkSpirit::GetAttackDamage(LPOBJ lpObj, int targetDefense, BOOL bIsOnDuel, int crititcaldamage,int excellentdamage)
{
	if ( g_ShieldSystemOn == TRUE )
	{
		if ( bIsOnDuel == TRUE )
		{
			bIsOnDuel = FALSE;
		}
	}

	int ad;
	int sub = this->m_AttackDamageMax - this->m_AttackDamageMin;
	int AttackDamage;

	if ( crititcaldamage )
	{
		AttackDamage = this->m_AttackDamageMin + sub;
	}
	else
	{
		AttackDamage = this->m_AttackDamageMin + (rand()%(sub+1));
	}

	if( excellentdamage )
	{
		AttackDamage += AttackDamage*20/100;
	}

	if ( ( lpObj->pInventory[0].m_Type >= ITEMGET(2,8) && lpObj->pInventory[0].m_Type <= ITEMGET(2,12) ) ||
		   lpObj->pInventory[0].m_Type == ITEMGET(2,13) ||
		   lpObj->pInventory[0].m_Type == ITEMGET(2,14) ||
		   lpObj->pInventory[0].m_Type == ITEMGET(2,15) ||
		   lpObj->pInventory[0].m_Type == ITEMGET(2,17) ||
		   lpObj->pInventory[0].m_Type == ITEMGET(2,18) ||
		   lpObj->pInventory[0].m_Type == ITEMGET(2,19) )
	{
		int damage = lpObj->pInventory[0].m_Magic / 2;
		damage -= (int)(damage * lpObj->pInventory[0].m_CurrentDurabilityState);
		damage = AttackDamage * damage / 100;
		AttackDamage += damage;
	}

	if ( bIsOnDuel )
	{
		ad = AttackDamage * 60 / 100 - targetDefense;
	}
	else
	{
		ad = AttackDamage - targetDefense;
	}

	if( lpObj->m_MPSkillOpt.MpsDarkSpiritStr1 > 0.0f )
	{
		ad+= lpObj->m_MPSkillOpt.MpsDarkSpiritStr1;
	}

	if( lpObj->m_MPSkillOpt.MpsUseScepterPetStr > 0.0f )
	{
		ad+= lpObj->m_MPSkillOpt.MpsUseScepterPetStr;
	}

	return ad;
}



#include "SoulSystem.h"

BOOL CDarkSpirit::MissCheck(LPOBJ lpObj, LPOBJ lpTargetObj, int skill,  int skillSuccess, BOOL& bAllMiss)
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
		int SuccessAttackRate = 0;
		int TargetSuccessfulBlocking = lpTargetObj->m_SuccessfulBlocking;
		int MsgDamage = 0;

		SuccessAttackRate += lpObj->m_MPSkillOpt.MpsAttackSuccessRate;

		if ( this->m_SuccessAttackRate < TargetSuccessfulBlocking )
		{
			bAllMiss = TRUE;
		}

		if ( IT_MAP_RANGE(lpTargetObj->MapNumber) && g_IllusionTemple.GetState(lpTargetObj->MapNumber) == 2 )
		{
			if( lpTargetObj->Type == OBJ_USER )
			{
				if( g_IllusionTemple.Is1stSkillActive(lpTargetObj->iTempleIndex,lpTargetObj->MapNumber) )
				{
					GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, 0, 0);
					return FALSE;
				}
			}

			if( lpObj->PartyNumber == lpTargetObj->PartyNumber )
			{
				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, 0, 0);
				return FALSE;
			}
		}

		if ( bAllMiss != FALSE )
		{
			if ( (rand()%100) >= 5 )
			{
				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, MsgDamage, 0);
				return FALSE;
			}
		}
		else if ( this->m_SuccessAttackRate )
		{
			if ( (rand()%this->m_SuccessAttackRate) < TargetSuccessfulBlocking)
			{
				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, MsgDamage, 0);
				return FALSE;
			}
		}
	}
	__except (this->ModeNormal(),-1)
	{
		return FALSE;
	}
	return TRUE;
}



BOOL CDarkSpirit::MissCheckPvP(LPOBJ lpObj, LPOBJ lpTargetObj, int skill,  int skillSuccess, BOOL& bAllMiss)
{
	float iAttackRate = 0;
	float iDefenseRate = 0;
	int iAttackSuccessRate = 0;

	if ( IT_MAP_RANGE(lpTargetObj->MapNumber) && g_IllusionTemple.GetState(lpTargetObj->MapNumber) == 2 )
	{
		if( lpTargetObj->Type == OBJ_USER )
		{
			if( g_IllusionTemple.Is1stSkillActive(lpTargetObj->iTempleIndex,lpTargetObj->MapNumber) )
			{
				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, 0, 0);
				return FALSE;
			}
		}

		if( lpObj->PartyNumber == lpTargetObj->PartyNumber )
		{
			GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, 0, 0);
			return FALSE;
		}
	}

	int Level = lpObj->Level + lpObj->MasterLevel;

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	iAttackRate = g_ClassCalc.GetAttackRate(lpObj);
#else
	if ( lpObj->Class == CLASS_KNIGHT )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 45) / 10);	// #formula
	}
	else if ( lpObj->Class == CLASS_DARKLORD )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 4 ));	// #formula
	}
	else if ( lpObj->Class == CLASS_ELF )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 6 ) / 10);	// #formula
	}
	else if ( lpObj->Class == CLASS_MAGUMSA )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 35 ) / 10);	// #formula
	}
	else if ( lpObj->Class == CLASS_WIZARD )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 4 ));	// #formula
	}
	else if( lpObj->Class == CLASS_SUMMONER )
	{
		iAttackRate = (float)(Level * 3 + ( lpObj->Dexterity * 35 ) / 10);	// #formula
	}
	else if( lpObj->Class == CLASS_FIGHTER )
	{
		iAttackRate = (float)(Level * 2.6 + ( lpObj->Dexterity * 36 ) / 10);	// #formula
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

	if ( iAttackRate <= 0.0f || iDefenseRate <= 0.0f || lpObj->Level <= 0 || lpTargetObj->Level <= 0 )
		return FALSE;

	iAttackRate += (float)(lpObj->m_ItemOptionExFor380.OpAddAttackSuccessRatePVP);
	iDefenseRate += (float)(lpTargetObj->m_ItemOptionExFor380.OpAddDefenseSuccessRatePvP);

	iAttackRate += (float)(lpObj->m_JewelOfHarmonyEffect.HJOpAddAttackSuccessRatePVP);
	iDefenseRate += (float)(lpTargetObj->m_JewelOfHarmonyEffect.HJOpAddDefenseSuccessRatePvP);

	iAttackRate += lpObj->m_MPSkillOpt.MpsPVPAttackDmgRate;
	iDefenseRate += lpTargetObj->m_MPSkillOpt.MpsPVPBlockingRate;

	float iExpressionA = ( iAttackRate * 10000.0f ) / ( iAttackRate + iDefenseRate );	// #formula
	float iExpressionB = ( Level * 10000.0f ) / ( Level + TargetLevel );	// #formula

	iExpressionA /= 10000.0f;
	iExpressionB /= 10000.0f;

	iAttackSuccessRate = (int)(100.0f * iExpressionA * g_fSuccessAttackRateOption * iExpressionB);

	if ( (TargetLevel - Level) >= 100 )
	{
		iAttackSuccessRate -= 5;
	}
	else if ( (TargetLevel - Level) >= 200 )
	{
		iAttackSuccessRate -= 10;
	}
	else if ( (TargetLevel - Level) >= 300 )
	{
		iAttackSuccessRate -= 15;
	}

	int iRate = rand() % 100;

	if ( iRate > iAttackSuccessRate )
	{
		GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, 0, 0);
		return FALSE;
	}

	return TRUE;
}

	


void CDarkSpirit::ChangeCommand(int command, int targetindex)
{
	CDarkSpirit::ePetItem_Mode setmode = PetItem_Mode_Normal;

	if ( command == PetItem_Mode_Normal )
	{
		setmode = PetItem_Mode_Normal;
	}
	else if ( command == PetItem_Mode_Attack_Random )
	{
		setmode = PetItem_Mode_Attack_Random;
	}
	else if ( command == PetItem_Mode_Attack_WithMaster )
	{
		setmode = PetItem_Mode_Attack_WithMaster;
	}
	else if ( command == PetItem_Mode_Attack_Target )
	{
		setmode = PetItem_Mode_Attack_Target;
	}

	this->SetMode(setmode, targetindex);
}


void __cdecl CDarkSpirit::SendLevelmsg(int aIndex, int nPos, int PetType, int InvenType)
{
	PMSG_SEND_PET_ITEMINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xA9, sizeof(pMsg));
	pMsg.PetType = PetType;
	pMsg.InvenType = InvenType;
	pMsg.nPos = nPos;
	pMsg.Level = gObj[aIndex].pInventory[nPos].m_PetItem_Level;
	pMsg.Exp = gObj[aIndex].pInventory[nPos].m_PetItem_Exp;
	pMsg.Life = (BYTE)gObj[aIndex].pInventory[nPos].m_Durability;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}



int CDarkSpirit::GetShieldDamage(LPOBJ lpObj, LPOBJ lpTargetObj, int iAttackDamage)
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
