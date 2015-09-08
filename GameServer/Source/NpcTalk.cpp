#include "stdafx.h"
#include "user.h"
#include "NpcTalk.h"
#include "gObjMonster.h"
#include "..\common\winutil.h"
#include "protocol.h"
#include "GameMain.h"
#include "BloodCastle.h"
#include "gObjMonster.h"
#include "DevilSquare.h"
#include "QuestInfo.h"
#include "CastleSiegeSync.h"
#include "KanturuEntranceNPC.h"
#include "JewelOfHarmonySystem.h"
#include "Gamemain.h"
#include "Event.h"
#include "CastleSiege.h"
#include "IllusionTempleEvent.h"
#include "DSProtocol.h"
#include "BuffEffectSlot.h"
#include "ItemAddOption.h"
#include "DoubleGoerEvent.h"
#include "GameEvent.h"
#include "ShopBoard.h"
#include "ResetSystem.h"
#if (ENABLETEST_NEWPVP == 1)
#include "NewPVP.h"
#endif

BOOL NpcTalk(LPOBJ lpNpc, LPOBJ lpObj)
{
	int npcnum = lpNpc->Class;

	if ( npcnum < 0)
	{
		return FALSE;
	}

	if (g_ResetSystem.ProcDialog(lpObj, lpNpc))
	{
		return TRUE;
	}

	if ( (lpObj->m_IfState.use) > 0 )
	{
		return TRUE;
	}

	if ( npcnum == 229 ) // Marlon
	{
		gQeustNpcTeleport.TalkRefAdd();
		lpObj->TargetShopNumber = npcnum;
	}

#if(__CUSTOM_QUEST__ == 1)
	if ( npcnum == 371 )
	{
		g_MyQuest.NpcTalk(lpObj,lpNpc);
		return TRUE;
	}
#endif

	if ( NpcQuestCheck(lpNpc, lpObj) != FALSE )
	{
		return TRUE;
	}

	

	switch ( npcnum )
	{
		case 254:
			return NpcFasi ( lpNpc, lpObj );

		case 241:
			return NpcGuildMasterTalk( lpNpc, lpObj );

		case 239:
			return NpcBattleAnnouncer( lpNpc, lpObj );

		case 249:
			return NpcRolensiaGuard( lpNpc, lpObj );

		case 238:
			return NpcChaosGoblelin( lpNpc, lpObj );

		case 237:
			return NpcRusipher( lpNpc, lpObj );

		case 253:
			return NpcPosionGirl( lpNpc, lpObj );

		case 245:
			return NpcDeviasWizard( lpNpc, lpObj );

		case 247:
			return NpcDeviasGuard( lpNpc, lpObj );

		case 240:
			return NpcWarehouse( lpNpc, lpObj );

		case 226:
			return NpcDarkSpiritTrainer( lpNpc, lpObj );

		case 242:
			return NpcNoriaRara( lpNpc, lpObj );

		case 244:
			return NpcDeviasMadam( lpNpc, lpObj );

		case 236:
			return NpcEventChipNPC( lpNpc, lpObj );

		case 234:
			return NpcServerDivision( lpNpc, lpObj );

		case 232:
			return NpcAngelKing( lpNpc, lpObj );

		case 233:
			return NpcAngelMessanger( lpNpc, lpObj );

		case 255:
			return NpcRolensiaMadam( lpNpc, lpObj );

		case 250:
			return NpcRoadMerchant( lpNpc, lpObj );

		case 246:
			return NpcDeviasWeapon( lpNpc, lpObj );

		case 251:
			return NpcRorensiaSmith( lpNpc, lpObj );

		case 243:
			return NpcNoriJangIn( lpNpc, lpObj );

		case 256:
			return NpcJewelMixDealer( lpNpc, lpObj );

#if (GS_CASTLE == 0 || GS_SPEC == 1)
		case 367:
			return NpcMainatenceMachine ( lpNpc, lpObj );
#endif

		case 221:
			return NpcSiegeMachine_Attack( lpNpc, lpObj );

		case 222:
			return NpcSiegeMachine_Defense( lpNpc, lpObj );

		case 223:
			return NpcElderCircle( lpNpc, lpObj );

		case 224:
			return NpcCastleGuard( lpNpc, lpObj );

		case 219:
			return NpcCastleGateLever( lpNpc, lpObj );

		case 216:
			return NpcCastleCrown( lpNpc, lpObj );

		case 217:
		case 218:
			return NpcCastleSwitch( lpNpc, lpObj );

#if (GS_CASTLE == 1)
		case 220:
			return NpcCastleHuntZoneGuard( lpNpc, lpObj );
#endif
		case 257:
			return NpcShadowPhantom( lpNpc, lpObj );

#if (GS_CASTLE == 1)

		case 205:
			return NpcCrywolfAltar(lpNpc,lpObj);

		case 206:
			return NpcCrywolfAltar(lpNpc,lpObj);

		case 207:
			return NpcCrywolfAltar(lpNpc,lpObj);

		case 208:
			return NpcCrywolfAltar(lpNpc,lpObj);

		case 209:
			return NpcCrywolfAltar(lpNpc,lpObj);
#endif
#if (CUSTOM_OLYMP==1)
		case 258:
			return g_CustomOlymp.NpcTalk(lpNpc,lpObj);
#endif
		case 259:
			return NpcReira ( lpNpc, lpObj );

		case 368:
		case 369:
		case 370:
			return g_kJewelOfHarmonySystem.NpcJewelOfHarmony( lpNpc, lpObj );

		case 375:
			return NpcChaosCardMaster( lpNpc, lpObj );

		case 376:
			return NpcDrinkSellerHillary( lpNpc, lpObj );

		case 377:
			return NpcDrinkSellerLindsay( lpNpc, lpObj );

		case 379:
			return NpcFireWorkGirl( lpNpc, lpObj );

		case 385:
			return NpcIllusionMaster( lpNpc, lpObj );

		case 380:
			return NpcIllusionHolyStatue( lpNpc, lpObj );

		case 383:
			return NpcAlliedTeamForge( lpNpc, lpObj );

		case 384:
			return NpcIllusionTeamForge( lpNpc, lpObj );

		case 415:
			return NpcSilvia( lpNpc, lpObj );

		case 416:
			return NpcLeah( lpNpc, lpObj );

		case 417:
			return NpcMarseille( lpNpc, lpObj );

		case 452:
			return NpcSeedMaster( lpNpc, lpObj );

		case 453:
			return NpcSeedResearch( lpNpc, lpObj );

		case 450:
			return NpcCherryBlossom( lpNpc, lpObj );

		case 465:
			return NpcSanta( lpNpc,lpObj);

		case 467:
			return NpcSnowman( lpNpc,lpObj);

		case 468:
			return NpcLitleSanta( lpNpc,lpObj,0x5E);

		case 469:
			return NpcLitleSanta( lpNpc,lpObj,0x5F);

		case 470:
			return NpcLitleSanta( lpNpc,lpObj,0);

		case 471:
			return NpcLitleSanta( lpNpc,lpObj,1);

		case 472:
			return NpcLitleSanta( lpNpc,lpObj,0x5C);

		case 473:
			return NpcLitleSanta( lpNpc,lpObj,0x5D);

		case 474:
			return NpcLitleSanta( lpNpc,lpObj,0x60);

		case 475:
			return NpcLitleSanta( lpNpc,lpObj,0x61);

		case 478:
			return NpcDelgado( lpNpc, lpObj );
			break;

		case 479:
			return NpcTitus( lpNpc, lpObj );

		case 522:
			return NpcImperialFort( lpNpc, lpObj );

		case 540:
			return NpcLugard( lpNpc, lpObj );

		case 541:
			return NpcCompensationBox( lpNpc, lpObj );

		case 542:
			return NpcGoldCompensationBox( lpNpc, lpObj );

		case 543:
			return NpcDuprianSteward( lpNpc, lpObj );

		case 544:
			return NpcVanertSteward( lpNpc, lpObj );

		case 547:
			return NpcJulia( lpNpc, lpObj );

		case 579:
			return NpcDavid( lpNpc, lpObj );

		case 580:
			return NpcCaptainSlaugh( lpNpc, lpObj );

		case 582:
			return NpcAdniel( lpNpc, lpObj );

		case 584:
			return NpcSirLesnar( lpNpc, lpObj );

		case 651:
			return NpcPrivateStoreBoard(lpNpc, lpObj);

	}

	return FALSE;
}


BOOL NpcMainatenceMachine(LPOBJ lpNpc, LPOBJ lpObj)
{
	g_KanturuEntranceNPC.NotifyEntranceInfo(lpObj->m_Index);
	return TRUE;
}


BOOL NpcReira(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}

BOOL NpcBattleAnnouncer(LPOBJ lpNpc, LPOBJ lpObj)
{
	return 0;
}


BOOL NpcFasi(LPOBJ lpNpc, LPOBJ lpObj)
{
	int bPk = FALSE;

#if( ENABLE_FIX_NPCTALKPK == 1 )
	if( lpObj->m_PK_Level >= 4 )
	{
		bPk = TRUE;
	}
#else
	if ( lpObj->PartyNumber >= 0 )
	{
		if( gParty.GetPartyPkLevel(lpObj->PartyNumber) >= 4 )
		{
			bPk = TRUE;
		}
	}
	else if( lpObj->m_PK_Level >= 4 )
	{
		bPk = TRUE;
	}

#endif

	if ( bPk == TRUE )
	{
		int hour = 0;
		int min = 0;

		if ( lpNpc->TargetNumber != lpObj->m_Index )
		{
			if ( gObjGetPkTime(lpObj, hour, min) == TRUE )
			{
				char szTemp[100];

				wsprintf(szTemp, lMsg.Get(MSGGET(4, 79)), hour, min);

				ChatTargetSend(lpNpc, szTemp, lpObj->m_Index);

				lpNpc->TargetNumber = lpObj->m_Index;

				if ( gPkLimitFree == FALSE )
				{
					return TRUE;
				}
			}
		}
		else if ( gPkLimitFree == FALSE )
		{
			int numbertext = rand() % 3;

			switch ( numbertext )
			{
				case 0:
					ChatTargetSend(lpNpc, lMsg.Get(MSGGET(4, 98)), lpObj->m_Index);
					break;

				case 1:
					ChatTargetSend(lpNpc, lMsg.Get(MSGGET(4, 99)), lpObj->m_Index);
					break;

				case 2:
					ChatTargetSend(lpNpc, lMsg.Get(MSGGET(4, 100)), lpObj->m_Index);
					break;
			}

			lpNpc->TargetNumber = 0;	// Apply Deathway Fix, change 0 to -1;
			return TRUE;
		}
	}

	return FALSE;
}




BOOL NpcGuildMasterTalk(LPOBJ lpNpc, LPOBJ lpObj)
{
#if (__GENS_SYSTEM__ == 1)

	if( lpObj->m_GensInfo.btFamily == GENS_NONE )
	{
		MsgOutput(lpObj->m_Index,"You are not a member of any gens clan!");
		return TRUE;
	}

#endif

	if ( gGuildCreate == FALSE )
	{
		ChatTargetSend(lpNpc, "Guild creation is disabled!", lpObj->m_Index);
		return TRUE;
	}

	if ( lpObj->GuildNumber > 0 )
	{
		ChatTargetSend(lpNpc, lMsg.Get(MSGGET(4, 116)), lpObj->m_Index);
		return TRUE;
	}

	int capacity = gObjGuildMasterCapacityTest(lpObj);

	if ( capacity == 0 )
	{
		ChatTargetSend(lpNpc, lMsg.Get(MSGGET(4, 117)), lpObj->m_Index);
	}
	else if ( capacity == 2 )
	{
		ChatTargetSend(lpNpc, lMsg.Get(MSGGET(4, 118)), lpObj->m_Index);
	}
	else
	{
		GCGuildMasterQuestionSend(lpObj->m_Index);
	}

	return TRUE;
}




BOOL NpcRolensiaGuard(LPOBJ lpNpc, LPOBJ lpObj)
{
	if( gEnableEventNPCTalk != FALSE )
	{
		GCServerCmd(lpObj->m_Index, 4, 0, 0);
	}

	return TRUE;
}





BOOL NpcChaosGoblelin(LPOBJ lpNpc, LPOBJ lpObj)
{
	gUserFindDevilSquareKeyEyes(lpObj->m_Index);

	return FALSE;
}



BOOL g_bUseLotterySystem = TRUE;


BOOL NpcChaosCardMaster(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( g_bUseLotterySystem == FALSE )
		return FALSE;

	if ( bCanChaosBox == TRUE )
	{
		if ( lpObj->m_IfState.use > 0 )
			return FALSE;

		if ( lpObj->m_bPShopOpen == true )
		{
			LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
				lpObj->AccountID, lpObj->Name);

			GCServerMsgStringSend(lMsg.Get(MSGGET(4,194)), lpObj->m_Index, 1);
			return TRUE;
		}

		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		pMsg.result = 0x15;

		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

		lpObj->m_IfState.use = 1;
		lpObj->m_IfState.type = 7;

		gObjInventoryTrans(lpObj->m_Index);

		LogAddTD("[JewelMix] [%s][%s] Jewel Mix Dealer Start",
			lpObj->AccountID, lpObj->Name);

		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}

	return TRUE;
}




BOOL NpcRusipher(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( gDevilSquareEvent == FALSE )
	{
		return TRUE;
	}


	int aIndex = lpObj->m_Index;

#ifdef BEREZNUK
	if ( lpObj->m_PK_Level >= 4 && !g_AllowPKEventDS )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(13,72)),lpObj->m_Index,1);
		return TRUE;
	}
#endif

	if ( (lpObj->m_IfState.use ) > 0 )
	{
		return TRUE;
	}

	if ( gUserFindDevilSquareInvitation(aIndex) == FALSE )
	{
		GCServerCmd(lpObj->m_Index, 1, 2 ,0);
	}
	else if (g_DevilSquare.GetState() == 1 )
	{
		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		pMsg.result = 0x04;

		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

		lpObj->m_IfState.use = 1;
		lpObj->m_IfState.type = 12;
	}
	else
	{
		GCServerCmd(lpObj->m_Index, 1, 3, 0);
	}

	return TRUE;
}




BOOL NpcPosionGirl(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}




BOOL NpcDeviasWizard(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}



BOOL NpcDeviasGuard(LPOBJ lpNpc, LPOBJ lpObj)
{
	return TRUE;
}



BOOL NpcDeviasWareHousemen(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}


BOOL NpcWarehouse(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}



BOOL NpcNoriaRara(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( gEnableEventNPCTalk != FALSE )
	{
		GCServerCmd(lpObj->m_Index, 4, 2, 0);
	}

	return FALSE;
}


BOOL NpcDeviasMadam(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( gEnableEventNPCTalk != FALSE )
	{
		GCServerCmd(lpObj->m_Index, 4, 1, 0);
	}

	return FALSE;
}


struct PMSG_REQ_VIEW_EC_MN
{
	PBMSG_HEAD h;	// C1:01
	int iINDEX;	// 4
	char szUID[11];	// 8
};

BOOL NpcEventChipNPC(LPOBJ lpNpc, LPOBJ lpObj)
{
#if (__CUSTOM__ == 1)
	if ( (lpObj->m_IfState.use) > 0 )
	{
		return TRUE;
	}
	
	if ( lpNpc->MapNumber == 0 )
	{
		PMSG_EVENTCHIPINFO Result;

		PHeadSetB((LPBYTE)&Result, 0x94, sizeof(Result));
		Result.ChipCount = g_RenaCount;
		Result.Type = 0;
		
		DataSend(lpObj->m_Index, (LPBYTE)&Result, Result.h.size);

		lpObj->m_IfState.type = 9;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;

		return TRUE;
	}

	if ( lpNpc->MapNumber == 2 )
	{
		PMSG_EVENTCHIPINFO Result;

		PHeadSetB((LPBYTE)&Result, 0x94, sizeof(Result));
		Result.ChipCount = g_StoneCount;
		Result.Type = 1;
		
		DataSend(lpObj->m_Index, (LPBYTE)&Result, Result.h.size);

		lpObj->m_IfState.type = 9;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;

		return TRUE;
	}

	PMSG_EVENTCHIPINFO Result;

	PHeadSetB((LPBYTE)&Result, 0x94, sizeof(Result));
	Result.ChipCount = g_StoneCount;
	Result.Type = 2;
		
	DataSend(lpObj->m_Index, (LPBYTE)&Result, Result.h.size);

	lpObj->m_IfState.type = 9;
	lpObj->m_IfState.state = 0;
	lpObj->m_IfState.use = 1;
	return TRUE;
#else

	if ( (lpObj->m_IfState.use) > 0 )
	{
		return TRUE;
	}
	
	if ( lpNpc->MapNumber == 0 )
	{
		PMSG_REQ_VIEW_EC_MN pMsgTha;

		PHeadSetB((LPBYTE)&pMsgTha, 0x01, sizeof(pMsgTha));
		pMsgTha.iINDEX = lpObj->m_Index;
		strcpy(pMsgTha.szUID, lpObj->AccountID);
		pEventObj = lpNpc;

		DataSendEventChip((char*)&pMsgTha, sizeof(pMsgTha));
		
		lpObj->m_IfState.type = 9;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;

		return TRUE;
	}

	if ( lpNpc->MapNumber == 2 )
	{
		PMSG_EVENTCHIPINFO Result;

		PHeadSetB((LPBYTE)&Result, 0x94, sizeof(Result));
		Result.Type = 2;
		
		DataSend(lpObj->m_Index, (LPBYTE)&Result, Result.h.size);

		lpObj->m_IfState.type = 9;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;

		return TRUE;
	}

#endif
	return FALSE;
}



BOOL NpcRorensiaSmith(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}




BOOL NpcNoriJangIn(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}





BOOL NpcQuestCheck(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( g_QuestInfo.NpcTalk(lpNpc, lpObj) != FALSE )
	{
		return TRUE;
	}

	return FALSE;
}




BOOL NpcServerDivision(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( gEnableServerDivision == FALSE )
	{
		return TRUE;
	}

	if ( lpObj->lpGuild != NULL )
	{
		GCServerCmd(lpObj->m_Index, 6, 0, 0);
		return TRUE;
	}

	lpObj->m_IfState.type = 11;
	lpObj->m_IfState.state = 0;
	lpObj->m_IfState.use = 1;

	return FALSE;
}



BOOL NpcRoadMerchant(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}





BOOL NpcAngelKing(LPOBJ lpNpc, LPOBJ lpObj)
{
	int iITEM_LEVEL = 0;

	if ( (  (lpObj->m_cBloodCastleIndex < 0)?FALSE:(lpObj->m_cBloodCastleIndex > MAX_BLOOD_CASTLE_LEVEL-1)?FALSE:TRUE ) == FALSE )
	{
		GCServerCmd(lpObj->m_Index, 1, 0x18, 0);
		return FALSE;
	}

	LogAddTD("[Blood Castle] (%d) [%s][%s] Talk to Angel King",
		lpObj->m_cBloodCastleIndex+1, lpObj->AccountID, lpObj->Name);

	if ( g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_bBC_REWARDED != false )
	{
		GCServerCmd(lpObj->m_Index, 1, 0x2E, 0);
		return FALSE;
	}

	if ( g_BloodCastle.GetCurrentState(lpObj->m_cBloodCastleIndex) != 2 || g_BloodCastle.CheckPlayStart(lpObj->m_cBloodCastleIndex) == false)
	{
		GCServerCmd(lpObj->m_Index, 1, 0x18, 0);
		return FALSE;
	}

	if ( lpObj->m_bBloodCastleComplete == true)
	{
		GCServerCmd(lpObj->m_Index, 1, 0x2E, 0);
		return FALSE;
	}

	if ( iITEM_LEVEL = iITEM_LEVEL = CHECK_LIMIT( g_BloodCastle.CheckQuestItem(lpObj->m_Index), 3)  )
	{
		if ( lpNpc->m_cBloodCastleIndex != lpObj->m_cBloodCastleIndex )
		{
			GCServerCmd(lpObj->m_Index, 1, 0x18, 0);
			return FALSE;
		}

		if ( g_BloodCastle.CheckUserBridgeMember(lpObj->m_cBloodCastleIndex, lpObj->m_Index) == false )
		{
			GCServerCmd(lpObj->m_Index, 1, 0x18, 0);
			return FALSE;
		}

		g_BloodCastle.SetUserState(lpObj->m_Index, 2);
		g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_iExtraEXP_Win_Quest_Party = lpObj->PartyNumber;
		g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_iExtraEXP_Win_Quest_Index = lpObj->m_Index;
		memcpy(g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szWin_Quest_CharName, lpObj->Name, 10);
		memcpy(g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szWin_Quest_AccountID, lpObj->AccountID, 10);
		g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szWin_Quest_CharName[10] = 0;	// Zero String terminated
		g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szWin_Quest_AccountID[10] = 0;	// Zero String Terminated
		
		LogAddTD("[Blood Castle] (%d) Blood Castle Quest Succeed Result -> Destroy Castle Door [%s][%s]",
			lpObj->m_cBloodCastleIndex+1, g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szKill_Door_AccountID,
			g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szKill_Door_CharName);
		LogAddTD("[Blood Castle] (%d) Blood Castle Quest Succeed Result -> Destroy Saint Status [%s][%s]",
			lpObj->m_cBloodCastleIndex+1, g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szKill_Door_AccountID,	// #error Correct this
			g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szKill_Status_CharName);
		LogAddTD("[Blood Castle] (%d) Blood Castle Quest Succeed Result -> Win Quest [%s][%s]",
			lpObj->m_cBloodCastleIndex+1, g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szWin_Quest_AccountID,
			g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_szWin_Quest_CharName);

		if ( g_BloodCastle.SetBridgeWinner(lpObj->m_cBloodCastleIndex, lpObj->m_Index) == TRUE )
		{
			lpObj->m_bBloodCastleComplete = true;

			if ( g_BloodCastle.CheckPartyExist(lpObj->m_Index) == FALSE )
			{
				GCServerCmd(lpObj->m_Index, 1, 0x17, 0);

				LogAddTD("[Blood Castle] (%d) (Account:%s, Name:%s) Succeed Blood Castle Quest",
					lpNpc->MapNumber-10, lpObj->AccountID, lpObj->Name);
			}
			else
			{
				GCServerCmd(lpObj->m_Index, 1, 0x17, 0);
				LogAddTD("[Blood Castle] (%d) (Account:%s, Name:%s) Succeed Blood Castle Quest - More Party to register",
					lpNpc->MapNumber-10, lpObj->AccountID, lpObj->Name);
			}
		}
		else
		{
			LogAddTD("[Blood Castle] (%d) (Account:%s, Name:%s) Winner Already Exist !!",
				lpNpc->MapNumber-10, lpObj->AccountID, lpObj->Name);

			return FALSE;
		}

		g_BloodCastle.SearchUserDeleteQuestItem(lpObj->m_Index);
	}
	else
	{
		if ( g_BloodCastle.CheckUserWinnerParty(lpObj->m_cBloodCastleIndex, lpObj->m_Index) == TRUE )
		{
			lpObj->m_bBloodCastleComplete = true;

			if ( g_BloodCastle.CheckPartyExist(lpObj->m_Index) == FALSE )
			{
				GCServerCmd(lpObj->m_Index, 1, 0x17, 0);

				LogAddTD("[Blood Castle] (%d) (Account:%s, Name:%s) Succeed Blood Castle Quest",
					lpNpc->MapNumber-10, lpObj->AccountID, lpObj->Name);
			}
			else
			{
				GCServerCmd(lpObj->m_Index, 1, 0x17, 0);
				LogAddTD("[Blood Castle] (%d) (Account:%s, Name:%s) Succeed Blood Castle Quest - More Party to register",
					lpNpc->MapNumber-10, lpObj->AccountID, lpObj->Name);
			}

			return FALSE;
		}

		GCServerCmd(lpObj->m_Index, 1, 0x18, 0);
	}
	
	return FALSE;
}



BOOL NpcAngelMessanger(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( (lpObj->m_IfState.use ) > 0 )
	{
		return TRUE;
	}

	int iIndex = lpObj->m_Index;

#ifdef BEREZNUK
	if ( gObj[iIndex].m_PK_Level >= 4 && !g_AllowPKEventBC )
	{
		GCServerCmd(iIndex, 0x39, 0, 0);
		return TRUE;
	}
#else

#if(ENABLE_FIX_NPCTALKPK == 1)
	if( gObj[iIndex].m_PK_Level >= 4 )
	{
		GCServerCmd(iIndex, 0x39, 0, 0);
		return TRUE;
	}
#endif

#endif

	int iITEM_LEVEL = 0;

	iITEM_LEVEL = g_BloodCastle.CheckEnterItem(lpObj->m_Index);

	if ( iITEM_LEVEL != 0 )
	{
		if ( iITEM_LEVEL != 10 && g_BloodCastle.CheckEnterLevel(lpObj->m_Index, iITEM_LEVEL-1) )
		{
			if ( g_BloodCastle.CheckEnterFreeTicket(lpObj->m_Index) == true )
			{
				iITEM_LEVEL = 10;
			}
		}

		if ( iITEM_LEVEL == 10 )
		{
			for (int i=0;i<MAX_CLOACK_LEVEL;i++)
			{
				if ( g_BloodCastle.CheckEnterLevel(lpObj->m_Index, i) == FALSE )
				{
					iITEM_LEVEL = i;
				}
			}
		}

		if ( g_BloodCastle.GetCurrentState(iITEM_LEVEL-1) == BC_STATE_CLOSED && g_BloodCastle.CheckCanEnter(iITEM_LEVEL-1) != false )
		{
			PMSG_TALKRESULT pMsg;

			pMsg.h.c = 0xC3;
			pMsg.h.headcode = 0x30;
			pMsg.h.size = sizeof(pMsg);
			pMsg.result = 0x06;

			DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

			lpObj->m_IfState.use = 1;
			lpObj->m_IfState.type = 12;
		}
		else
		{
			GCServerCmd(lpObj->m_Index, 1, 0x14, 0);
		}
	}
	else
	{
		GCServerCmd(lpObj->m_Index, 1, 0x15, 0);
	}

	return FALSE;
}




BOOL NpcRolensiaMadam(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}





BOOL NpcDeviasWeapon(LPOBJ lpNpc, LPOBJ lpObj)
{
	return FALSE;
}




BOOL NpcDarkSpiritTrainer(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( bCanChaosBox == TRUE )
	{
		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		
		if ( lpObj->m_bPShopOpen == true )
		{
			LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
				lpObj->AccountID, lpObj->Name);
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);

			return TRUE;
		}

		lpObj->m_IfState.type = 13;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;
		
		pMsg.result = 0x07;
		lpObj->bIsChaosMixCompleted = false;
		pMsg.level1 = gDQChaosSuccessRateLevel1;
		pMsg.level2 = gDQChaosSuccessRateLevel2;
		pMsg.level3 = gDQChaosSuccessRateLevel3;
		pMsg.level4 = gDQChaosSuccessRateLevel4;
		pMsg.level5 = gDQChaosSuccessRateLevel5;
		pMsg.level6 = gDQChaosSuccessRateLevel6;

		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

		GCAnsCsMapSvrTaxInfo(lpObj->m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index));
		gObjInventoryTrans(lpObj->m_Index);

		LogAddTD("[%s][%s] Open Chaos Box", lpObj->AccountID, lpObj->Name);
		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}

	return TRUE;
}





BOOL NpcJewelMixDealer(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( bCanChaosBox == TRUE )
	{
		if ( (lpObj->m_IfState.use) > 0 )
		{
			return TRUE;
		}

		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		pMsg.result = 0x09;

		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

		lpObj->m_IfState.use = 1;
		lpObj->m_IfState.type = 12;

		LogAddTD("[JewelMix] [%s][%s] Jewel Mix Dealer Start", lpObj->AccountID, lpObj->Name);

		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);

	}

	return TRUE;
}



struct PMSG_ANS_USE_WEAPON_INTERFACE
{
	PBMSG_HEAD2 h;	// C1:B7:00
	BYTE btResult;	// 4
	BYTE btWeaponType;	// 5
	BYTE btObjIndexH;	// 6
	BYTE btObjIndexL;	// 7
};


BOOL NpcSiegeMachine_Attack(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( (lpObj->m_IfState.use) > 0 )
	{
		return TRUE;
	}

	PMSG_ANS_USE_WEAPON_INTERFACE pResult = {0};

	PHeadSubSetB((LPBYTE)&pResult, 0xB7, 0x00, sizeof(pResult));

	pResult.btResult = 0;
	pResult.btWeaponType = 1;
	pResult.btObjIndexH = SET_NUMBERH(lpNpc->m_Index);
	pResult.btObjIndexL = SET_NUMBERL(lpNpc->m_Index);

	if ( gObjIsConnectedGP(lpNpc->m_iWeaponUser) == FALSE )
	{
		lpNpc->m_iWeaponUser = 0;
		lpNpc->m_btWeaponState = 1;
	}

	lpNpc->m_btWeaponState = 1;


	if ( lpNpc->m_btWeaponState == 1 )
	{
		pResult.btResult = 1;
		lpNpc->m_btWeaponState = 2;
		lpNpc->m_iWeaponUser = lpObj->m_Index;

		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		lpObj->m_IfState.use = 1;
		lpObj->m_IfState.type = 15;
	}
	else
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
	}

	return TRUE;
}



BOOL NpcSiegeMachine_Defense(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( (lpObj->m_IfState.use ) > 0 )
	{
		return TRUE;
	}

	PMSG_ANS_USE_WEAPON_INTERFACE pResult = {0};

	PHeadSubSetB((LPBYTE)&pResult, 0xB7, 0x00, sizeof(pResult));

	pResult.btResult = 0;
	pResult.btWeaponType = 2;
	pResult.btObjIndexH = SET_NUMBERH(lpNpc->m_Index);
	pResult.btObjIndexL = SET_NUMBERL(lpNpc->m_Index);

	if ( gObjIsConnectedGP(lpNpc->m_iWeaponUser) == FALSE )
	{
		lpNpc->m_iWeaponUser = 0;
		lpNpc->m_btWeaponState = 1;
	}

	lpNpc->m_btWeaponState = 1;


	if ( lpNpc->m_btWeaponState == 1 )
	{
		pResult.btResult = 1;
		lpNpc->m_btWeaponState = 2;
		lpNpc->m_iWeaponUser = lpObj->m_Index;

		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		lpObj->m_IfState.use = 1;
		lpObj->m_IfState.type = 16;
	}
	else
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
	}

	return TRUE;
}




BOOL NpcElderCircle(LPOBJ lpNpc, LPOBJ lpObj)
{
#if (GS_CASTLE == 1)
	if ( (lpObj->m_IfState.use ) > 0 )
	{
		return TRUE;
	}

	if( g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		ChatTargetSend(lpNpc,lMsg.Get(MSGGET(6,85)),lpObj->m_Index);
		return TRUE;
	}

	if( strcmp(lpObj->GuildName,"") == NULL)
	{
		ChatTargetSend(lpNpc,lMsg.Get(MSGGET(6,86)),lpObj->m_Index);
		return TRUE;
	}

	if( g_CastleSiege.CheckGuildOwnCastle(lpObj->GuildName) == FALSE
		|| (lpObj->GuildStatus != G_MASTER && lpObj->GuildStatus != G_SUB_MASTER))
	{
		ChatTargetSend(lpNpc,lMsg.Get(MSGGET(6,86)),lpObj->m_Index);
		return TRUE;
	}

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x0C;

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 12;

	pMsg.level1 = 0;
	pMsg.level2 = 0;
	pMsg.level3 = 0;
	pMsg.level4 = 0;
	pMsg.level5 = 0;
	pMsg.level6 = 0;

	if( bCanChaosBox == TRUE )
	{
		lpObj->m_IfState.type = 7;
		lpObj->bIsChaosMixCompleted = FALSE;
		pMsg.level1 = 1;

		GCAnsCsMapSvrTaxInfo(lpObj->m_Index,1,g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index));
		gObjInventoryTrans(lpObj->m_Index);
		LogAddTD("[%s][%s] Open Chaos Box",
			lpObj->AccountID,lpObj->Name);

		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}

	DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
#endif



	return TRUE;
}





BOOL NpcCastleGuard(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( (lpObj->m_IfState.use ) > 0 )
	{
		return TRUE;
	}

#if (GS_CASTLE == 1)

	if( g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		ChatTargetSend(lpNpc,lMsg.Get(MSGGET(6,87)),lpObj->m_Index);
		return TRUE;
	}

#endif

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x0D;

	DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 12;


	return TRUE;
}




BOOL NpcCastleGateLever(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( (lpObj->m_IfState.use) > 0 )
	{
		return TRUE;
	}

#if (GS_CASTLE == 1)

	int bControlEnable = FALSE;
	BYTE btResult = NULL;
	int iGateIndex = -1;

	if( g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE )
	{
		if(lpObj->m_btCsJoinSide != 1)
		{
			btResult = 4;
			bControlEnable = FALSE;
		}
		else
		{
			bControlEnable = TRUE;
		}
	}
	else
	{
		if(g_CastleSiege.CheckCastleOwnerMember(lpObj->m_Index) == FALSE
			&& g_CastleSiege.CheckCastleOwnerUnionMember(lpObj->m_Index) == FALSE)
		{
			btResult = 4;
			bControlEnable = FALSE;
		}
		else
		{
			bControlEnable = TRUE;
		}
	}

	if(bControlEnable != FALSE)
	{
		if(g_CastleSiege.CheckLeverAlive(lpNpc->m_Index) == TRUE)
		{
			if(g_CastleSiege.CheckCsGateAlive(lpNpc->m_iCsGateLeverLinkIndex) == TRUE)
			{
				btResult = 1;
				iGateIndex = lpNpc->m_iCsGateLeverLinkIndex;
			}
			else
			{
				btResult = 2;
			}
		}
		else
		{
			btResult = 3;
		}
	}

	GCAnsCsGateState(lpObj->m_Index,btResult,iGateIndex);

	if(btResult != 1)
	{
		return TRUE;
	}
	else
	{
		lpObj->m_IfState.use = 1;
		lpObj->m_IfState.type = 12;
	}
#endif

	return TRUE;
}





BOOL NpcCastleCrown(LPOBJ lpNpc, LPOBJ lpObj)
{
#if (GS_CASTLE == 1)

	if( gObjIsConnected(lpObj->m_Index) == FALSE )
		return TRUE;

	if( g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE )
		return TRUE;

	if( lpObj->m_btCsJoinSide < 2 || lpObj->m_bCsGuildInvolved == FALSE )
		return TRUE;

	if( g_CastleSiege.CheckUnionGuildMaster(lpObj->m_Index) == FALSE )
		return TRUE;

	if( abs( lpObj->Y - lpNpc->Y) > 3 || abs(lpObj->X - lpNpc->X) > 3)
		return TRUE;
	
	if( g_CastleSiege.GetRegCrownAvailable() == FALSE )
		return TRUE;

	int iCrownUser = g_CastleSiege.GetCrownUserIndex();

	if( gObjIsConnected( iCrownUser ) == FALSE )
	{
		int iSwitchUser1 = g_CastleSiege.GetCrownSwitchUserIndex(217);
		int iSwitchUser2 = g_CastleSiege.GetCrownSwitchUserIndex(218);

		if( gObjIsConnected(iSwitchUser1) == FALSE || gObjIsConnected(iSwitchUser2) == FALSE )
		{
			GCAnsCsAccessCrownState(lpObj->m_Index,4);
			LogAddTD("[CastleSiege] [%s][%s] Failed to Register Castle Crown (GUILD:%s)",
				lpObj->AccountID,lpObj->Name,lpObj->GuildName);
			return TRUE;
		}

		if( lpObj->m_btCsJoinSide != gObj[iSwitchUser1].m_btCsJoinSide
			|| lpObj->m_btCsJoinSide != gObj[iSwitchUser2].m_btCsJoinSide )
		{
			GCAnsCsAccessCrownState(lpObj->m_Index,4);
			LogAddTD("[CastleSiege] [%s][%s] Failed to Register Castle Crown (GUILD:%s) (S1:%s/%s)(S2:%s/%s)",
				lpObj->AccountID,lpObj->Name,lpObj->GuildName,gObj[iSwitchUser1].AccountID,gObj[iSwitchUser1].Name,gObj[iSwitchUser2].AccountID,gObj[iSwitchUser2].Name);
			return TRUE;
		}

		GCAnsCsAccessCrownState(lpObj->m_Index,0);
		g_CastleSiege.SetCrownUserIndex(lpObj->m_Index);
		g_CastleSiege.SetCrownAccessUserX(lpObj->X);
		g_CastleSiege.SetCrownAccessUserY(lpObj->Y);
		g_CastleSiege.SetCrownAccessTickCount();

		g_CastleSiege.NotifyAllUserCsProgState(0,lpObj->GuildName);
		LogAddTD("[CastleSiege] [%s][%s] Start to Register Castle Crown (GUILD:%s)",
			lpObj->AccountID,lpObj->Name,lpObj->GuildName);
	}
	else if(lpObj->m_Index != iCrownUser)
	{
		GCAnsCsAccessCrownState(lpObj->m_Index,3);
	}

#endif
	return TRUE;
}




BOOL NpcCastleSwitch(LPOBJ lpNpc, LPOBJ lpObj)
{
#if (GS_CASTLE == 1)

	if(gObjIsConnected(lpObj->m_Index) == FALSE)
		return TRUE;

	if(g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
		return TRUE;

	if(lpObj->m_btCsJoinSide < 2)
		return TRUE;

	if( abs( lpObj->Y - lpNpc->Y) > 3 || abs(lpObj->X - lpNpc->X) > 3)
		return TRUE;

	if(g_CastleSiege.CheckGuardianStatueExist() == TRUE)
	{
		MsgOutput(lpObj->m_Index,lMsg.Get(MSGGET(6,200)));
		return TRUE;
	}

	int SwitchIndex = g_CastleSiege.GetCrownSwitchUserIndex(lpNpc->Class);

	if( gObjIsConnected(SwitchIndex) == FALSE)
	{
		GCAnsCsAccessSwitchState(lpObj->m_Index,lpNpc->m_Index,-1,1);
		g_CastleSiege.SetCrownSwitchUserIndex(lpNpc->Class,lpObj->m_Index);
		LogAddTD("[CastleSiege] [%s][%s] Start to Push Castle Crown Switch (GUILD:%s) - CS X:%d/Y:%d",
			lpObj->AccountID,lpObj->Name,lpObj->GuildName,lpNpc->X,lpNpc->Y);
	}
	else if(lpObj->m_Index != SwitchIndex)
	{
		GCAnsCsAccessSwitchState(lpObj->m_Index,lpNpc->m_Index,SwitchIndex,2);
	}



#endif
	return TRUE;
}

struct PMSG_TALKRESULT_EX
{
	PBMSG_HEAD	h;
	BYTE btResult;
	WORD wNpcId;
	int Info;
};

BOOL NpcShadowPhantom(LPOBJ lpNpc, LPOBJ lpObj)
{
	if ( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}

#if (__QUEST_SYSTEM__ == 1)

	PMSG_TALKRESULT_EX pMsg;
	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0xF9;
	pMsg.h.size = sizeof(pMsg);

	pMsg.btResult = TRUE;
	pMsg.wNpcId = 257;

	lpObj->TargetShopNumber = 257;
	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

#else

	if ( lpObj->Level > g_NPCBuffLevel || lpObj->ChangeUP2 != FALSE )
	{
		GCServerCmd(lpObj->m_Index, 0x0D, 0, 0);
		return TRUE;
	}

#if (__CUSTOM__ == 1)
	int Defense = lpObj->Level / g_ElfBuffDefenseDiv + g_ElfBuffDefenseStart;
	int Attack = lpObj->Level / g_ElfBuffAttackDiv + g_ElfBuffAttackStart;

	gObjAddBuffEffect(lpObj,BUFF_NPC_PHANTOM,ADD_OPTION_ATTACK_DAMAGE,Attack,ADD_OPTION_DEFENSE,Defense,0,0,g_ElfBuffTime);
#else
	int Defense = lpObj->Level / 5 + 50;
	int Attack = lpObj->Level / 3 + 45;

	gObjAddBuffEffect(lpObj,BUFF_NPC_PHANTOM,ADD_OPTION_ATTACK_DAMAGE,Attack,ADD_OPTION_DEFENSE,Defense,0,0,1800000);
	
#endif

#endif

	return TRUE;

}

#if (GS_CASTLE == 1)

struct PMSG_ANS_GUARD_IN_CASTLE_HUNTZONE
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	unsigned char btUsable;
	int iCurrentPrice;
	int iMaxPrice;
	int iUnitOfPrice;
};

BOOL NpcCastleHuntZoneGuard(LPOBJ lpNpc, LPOBJ lpObj)
{
	if(gObjIsConnected(lpObj->m_Index) == FALSE)
		return TRUE;

	if((lpObj->m_IfState.use) > 0)
		return TRUE;

	PMSG_ANS_GUARD_IN_CASTLE_HUNTZONE pResult = {0};

	PHeadSubSetB((PBYTE)&pResult,0xB9,0x03,sizeof(pResult));
	pResult.btResult = 1;
	pResult.iMaxPrice = 300000;
	pResult.iUnitOfPrice = 10000;
	pResult.btUsable = FALSE;
	pResult.iCurrentPrice = 0;

	pResult.iCurrentPrice = g_CastleSiegeSync.GetTaxHuntZone(lpObj->m_Index,FALSE);

	if(g_CastleSiege.GetHuntZoneEnter() != FALSE)
	{
		pResult.btUsable = TRUE;
	}

	if(lpObj->lpGuild != FALSE)
	{
		if(g_CastleSiege.CheckCastleOwnerUnionMember(lpObj->m_Index) != FALSE)
		{
			pResult.btResult = 2;
		}

		if(g_CastleSiege.CheckCastleOwnerMember(lpObj->m_Index) != FALSE && lpObj->GuildStatus == G_MASTER)
		{
			pResult.btResult = 3;
		}
	}
	else if(g_CastleSiege.GetHuntZoneEnter())
	{
		pResult.btResult = 1;
	}

	DataSend(lpObj->m_Index,(PBYTE)&pResult,pResult.h.size);
return TRUE;
}

BOOL NpcCrywolfAltar(LPOBJ lpNpc, LPOBJ lpObj)
{
	if(gObjIsConnected(lpObj->m_Index))
	{
		return TRUE;
	}
return TRUE;
}

#endif

BOOL NpcDrinkSellerHillary(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}
return FALSE;
}

BOOL NpcDrinkSellerLindsay(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}
return FALSE;
}

BOOL NpcFireWorkGirl(LPOBJ lpNpc,LPOBJ lpObj )
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}
return FALSE;
}

BOOL NpcIllusionMaster(LPOBJ lpNpc, LPOBJ lpObj)
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}

#ifdef BEREZNUK
	if ( lpObj->m_PK_Level >= 4 && !g_AllowPKEventIT )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(13,72)),lpObj->m_Index,1);
		return TRUE;
	}
#endif

	g_IllusionTemple.ClickEnterNpc(lpNpc,lpObj);
	return TRUE;
}

BOOL NpcIllusionHolyStatue(LPOBJ lpNpc, LPOBJ lpObj)
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}

	g_IllusionTemple.ClickToStatus(lpObj->MapNumber,lpNpc,lpObj);

return TRUE;
}


BOOL NpcAlliedTeamForge(LPOBJ lpNpc,LPOBJ lpObj )
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}

	g_IllusionTemple.ClickAlliedNpc(lpObj->MapNumber,lpNpc,lpObj);

return TRUE;
}

BOOL NpcIllusionTeamForge(LPOBJ lpNpc,LPOBJ lpObj )
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}

	g_IllusionTemple.ClickIllusionNpc(lpObj->MapNumber,lpNpc,lpObj);

return TRUE;
}

BOOL NpcSilvia(LPOBJ lpNpc,LPOBJ lpObj )
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}
return FALSE;
}

BOOL NpcLeah(LPOBJ lpNpc,LPOBJ lpObj )
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}
return FALSE;
}

BOOL NpcMarseille(LPOBJ lpNpc,LPOBJ lpObj )
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}
return FALSE;
}

BOOL NpcCherryBlossom(LPOBJ lpNpc,LPOBJ lpObj )
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}
return FALSE;
}

BOOL NpcSeedMaster(LPOBJ lpNpc,LPOBJ lpObj)
{
	if ( bCanChaosBox == TRUE )
	{
		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		
		if ( lpObj->m_bPShopOpen == true )
		{
			LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
				lpObj->AccountID, lpObj->Name);
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);
			return TRUE;
		}

		lpObj->m_IfState.type = 7;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;
		
		pMsg.result = 23;
		lpObj->bIsChaosMixCompleted = false;
		pMsg.level1 = gDQChaosSuccessRateLevel1;
		pMsg.level2 = gDQChaosSuccessRateLevel2;
		pMsg.level3 = gDQChaosSuccessRateLevel3;
		pMsg.level4 = gDQChaosSuccessRateLevel4;
		pMsg.level5 = gDQChaosSuccessRateLevel5;
		pMsg.level6 = gDQChaosSuccessRateLevel6;

		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

		GCAnsCsMapSvrTaxInfo(lpObj->m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index));
		gObjInventoryTrans(lpObj->m_Index);

		LogAddTD("[%s][%s] Open Chaos Box", lpObj->AccountID, lpObj->Name);
		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}

	return TRUE;
}

BOOL NpcSeedResearch(LPOBJ lpNpc,LPOBJ lpObj )
{
	if ( bCanChaosBox == TRUE )
	{
		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		
		if ( lpObj->m_bPShopOpen == true )
		{
			LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
				lpObj->AccountID, lpObj->Name);
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);
			return TRUE;
		}

		lpObj->m_IfState.type = 7;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;
		
		pMsg.result = 24;
		lpObj->bIsChaosMixCompleted = false;
		pMsg.level1 = gDQChaosSuccessRateLevel1;
		pMsg.level2 = gDQChaosSuccessRateLevel2;
		pMsg.level3 = gDQChaosSuccessRateLevel3;
		pMsg.level4 = gDQChaosSuccessRateLevel4;
		pMsg.level5 = gDQChaosSuccessRateLevel5;
		pMsg.level6 = gDQChaosSuccessRateLevel6;

		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

		GCAnsCsMapSvrTaxInfo(lpObj->m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index));
		gObjInventoryTrans(lpObj->m_Index);

		LogAddTD("[%s][%s] Open Chaos Box", lpObj->AccountID, lpObj->Name);
		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}

	return TRUE;
}

BOOL NpcSanta(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( !gObjIsConnected(lpObj->m_Index) )
	{
		return true;
	}
return true;
}

BOOL NpcSnowman(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( !gObjIsConnected(lpObj->m_Index) )
	{
		return true;
	}

	GCServerCmd(lpObj->m_Index,0x11,1,0);
	return true;
}

BOOL NpcLitleSanta(LPOBJ lpNpc,LPOBJ lpObj,int Buff)
{
	if( !gObjIsConnected(lpObj->m_Index) )
	{
		return true;
	}

	switch( Buff )
	{
	case 0:
		lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
		GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
		break;
	case 1:
		lpObj->Mana = lpObj->MaxMana + lpObj->MaxMana;
		GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
		break;
	case 0x5E:
		gObjAddBuffEffect(lpObj,BUFF_SANTA_STRENGHTHENER,ADD_OPTION_SANTABUFF,30,0,0,1800);
		break;
	case 0x5F:
		gObjAddBuffEffect(lpObj,BUFF_SANTA_DEFENSE,ADD_OPTION_SANTADEFENSE,100,0,0,1800);
		break;
	case 0x5C:
		gObjAddBuffEffect(lpObj,BUFF_SANTA_HEALING,ADD_OPTION_LIFE,500,0,0,1800);
		break;
	case 0x5D:
		gObjAddBuffEffect(lpObj,BUFF_SANTA_PROTECTION,ADD_OPTION_MANA,500,0,0,1800);
		break;
	case 0x60:
		gObjAddBuffEffect(lpObj,BUFF_SANTA_QUICKNESS,ADD_OPTION_ATTACK_SPEED,15,0,0,1800);
		break;
	case 0x61:
		gObjAddBuffEffect(lpObj,BUFF_SANTA_FORTUNE,ADD_OPTION_SANTABPRECOVERY,10,0,0,1800);
		break;
	}

return true;
}

BOOL  NpcDelgado(LPOBJ lpNpc,LPOBJ lpObj )
{
	if( !gObjIsConnected(lpObj->m_Index) )
		return true;

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 32;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	return true;
}

BOOL NpcTitus(LPOBJ lpNpc,LPOBJ lpObj)
{
#if (ENABLETEST_NEWPVP == 1)
	if (lpObj->m_IfState.use > 0) {
		// ... wait return
	} else {
		g_NewPVP.SendChannelList(*lpNpc, *lpObj);
		PMSG_TALKRESULT pResult;
		pResult.h.c = 0xC3;
		pResult.h.headcode = 0x30;
		pResult.h.size = sizeof(pResult);
		pResult.result = 33;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		lpObj->m_IfState.type	= 20;
		lpObj->m_IfState.use	= 1;
	}
	return TRUE;
#else
	if( //g_DuelManager.SendDuelStatus(lpObj) == TRUE)
	{
		return TRUE;
	}
	return FALSE;
#endif
}


BOOL NpcImperialFort(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( !gObjIsConnected(lpObj->m_Index) )
		return true;

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 36;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	return true;
}

BOOL NpcLugard(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( !gObjIsConnected(lpObj->m_Index) )
		return true;

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 35;
	pMsg.level1 = gDoubleGoer.GetOpenTime()/60;
	pMsg.level2 = gDoubleGoer.GetOpenTime()%60;

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 21;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	return true;
}

BOOL NpcCompensationBox(LPOBJ lpNpc,LPOBJ lpObj)
{

	if( !gObjIsConnected(lpObj->m_Index) )
		return true;

	gDoubleGoer.CompensationBox(lpNpc,lpObj);
return true;
}

BOOL NpcGoldCompensationBox(LPOBJ lpNpc,LPOBJ lpObj)
{

	if( !gObjIsConnected(lpObj->m_Index) )
		return true;

	gDoubleGoer.GoldCompensationBox(lpNpc,lpObj);
return true;
}

BOOL NpcDuprianSteward(LPOBJ lpNpc,LPOBJ lpObj)
{
#if (__GENS_SYSTEM__ == 1)
	if ( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}

	PMSG_TALKRESULT_EX pMsg = {0};
	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0xF9;
	pMsg.h.size = sizeof(pMsg);

	pMsg.btResult = TRUE;
	pMsg.wNpcId = lpNpc->Class;

	pMsg.Info = lpObj->m_GensInfo.iContribution;

	lpObj->TargetShopNumber = 543;
	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
#endif
	return TRUE;
}

BOOL NpcVanertSteward(LPOBJ lpNpc,LPOBJ lpObj)
{
#if (__GENS_SYSTEM__ == 1)
	if ( gObjIsConnected(lpObj->m_Index) == FALSE )
	{
		return TRUE;
	}

	PMSG_TALKRESULT_EX pMsg = {0};
	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0xF9;
	pMsg.h.size = sizeof(pMsg);

	pMsg.btResult = TRUE;
	pMsg.wNpcId = lpNpc->Class;

	pMsg.Info = lpObj->m_GensInfo.iContribution;


	lpObj->TargetShopNumber = 544;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
#endif
	return TRUE;
}

BOOL NpcJulia(LPOBJ lpNpc, LPOBJ lpObj)
{
	if( !gObjIsConnected(lpObj->m_Index) )
		return true;

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 37;

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 21;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	return true;
}

BOOL NpcDavid(LPOBJ lpNpc,LPOBJ lpObj)
{
	if ( bCanChaosBox == TRUE )
	{
		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		
		if ( lpObj->m_bPShopOpen == true )
		{
			LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
				lpObj->AccountID, lpObj->Name);
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);
			return TRUE;
		}

		lpObj->m_IfState.type = 7;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;
		
		pMsg.result = 38;
		lpObj->bIsChaosMixCompleted = false;
		pMsg.level1 = gDQChaosSuccessRateLevel1;
		pMsg.level2 = gDQChaosSuccessRateLevel2;
		pMsg.level3 = gDQChaosSuccessRateLevel3;
		pMsg.level4 = gDQChaosSuccessRateLevel4;
		pMsg.level5 = gDQChaosSuccessRateLevel5;
		pMsg.level6 = gDQChaosSuccessRateLevel6;

		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

		GCAnsCsMapSvrTaxInfo(lpObj->m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index));
		gObjInventoryTrans(lpObj->m_Index);

		LogAddTD("[%s][%s] Open Chaos Box", lpObj->AccountID, lpObj->Name);
		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}

	return TRUE;
}

BOOL NpcCaptainSlaugh(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( !gObjIsConnected(lpObj->m_Index) )
		return true;

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 39;

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 21;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	return true;
}

BOOL NpcAdniel(LPOBJ lpNpc,LPOBJ lpObj )
{
	if ( bCanChaosBox == TRUE )
	{
		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		
		if ( lpObj->m_bPShopOpen == true )
		{
			LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
				lpObj->AccountID, lpObj->Name);
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);
			return TRUE;
		}

		lpObj->m_IfState.type = 7;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;
		

		// 39 - entering acheron
		// 40 - arca war npc
		// 41 - mix npc

		pMsg.result = 41;
		lpObj->bIsChaosMixCompleted = false;
		pMsg.level1 = gDQChaosSuccessRateLevel1;
		pMsg.level2 = gDQChaosSuccessRateLevel2;
		pMsg.level3 = gDQChaosSuccessRateLevel3;
		pMsg.level4 = gDQChaosSuccessRateLevel4;
		pMsg.level5 = gDQChaosSuccessRateLevel5;
		pMsg.level6 = gDQChaosSuccessRateLevel6;

		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

		GCAnsCsMapSvrTaxInfo(lpObj->m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index));
		gObjInventoryTrans(lpObj->m_Index);

		LogAddTD("[%s][%s] Open Chaos Box", lpObj->AccountID, lpObj->Name);
		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}
	return true;
}

BOOL NpcSirLesnar(LPOBJ lpNpc,LPOBJ lpObj)
{
	if ( bCanChaosBox == TRUE )
	{
		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		
		if ( lpObj->m_bPShopOpen == true )
		{
			LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
				lpObj->AccountID, lpObj->Name);
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);
			return TRUE;
		}

		lpObj->m_IfState.type = 21;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;
		
		// 39 - entering acheron
		// 40 - arca war npc
		// 41 - mix npc

		pMsg.result = 40;
		lpObj->bIsChaosMixCompleted = false;
		pMsg.level1 = gDQChaosSuccessRateLevel1;
		pMsg.level2 = gDQChaosSuccessRateLevel2;
		pMsg.level3 = gDQChaosSuccessRateLevel3;
		pMsg.level4 = gDQChaosSuccessRateLevel4;
		pMsg.level5 = gDQChaosSuccessRateLevel5;
		pMsg.level6 = gDQChaosSuccessRateLevel6;

		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
	}
	return true;
}

BOOL NpcPrivateStoreBoard(LPOBJ lpNpc,LPOBJ lpObj)
{
	SHOPBOARD_CGREQ_ITEM lpInfo;
	lpInfo.ItemID = (WORD)-1;
	g_ShopBoard.CGReqItemSearch(lpObj->m_Index, &lpInfo);
	return true;
}

#if (CUSTOM_OLYMP==1)

BOOL CCustomOlymp::NpcTalk(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( !this->m_bOlympEnable )
		return true;

	if( !gObjIsConnected(lpObj->m_Index) )
		return true;

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 21;

	PSMG_OLP_NPCTALK pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xFE,0x00,sizeof(PSMG_OLP_NPCTALK));

	pMsg.btOpen = m_bOPEN;

	if( m_bFIGHT_START || m_dwFIGHT_START > 0 )
		pMsg.btOpen = FALSE;

	pMsg.btRegistered = IsRegistered(lpObj->m_Index);

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	return true;
}

#endif