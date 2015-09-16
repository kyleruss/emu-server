// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// GS-n 0.99.60T 0x0041D4D0
// GS-N	1.00.18	JNP	0x00424550	-	Completed


#include "stdafx.h"
#include "DSProtocol.h"
#include "logproc.h"
#include "DBSockMng.h"
#include "GameMain.h"
#include "GameServer.h"
#include "MapServerManager.h"
#include "CastleSiegeSync.h"
#include "ChaosBox.h"
#include "QuestInfo.h"
#include "TNotice.h"
#include "SProtocol.h"
#include "EDSProtocol.h"
#include "protocol.h"
#include "user.h"
#include "giocp.h"
#include "ObjUseSkill.h"
#include "CashItemPeriodSystem.h"
#include "BloodCastle.h"
#include "..\common\winutil.h"
#include "ObjCalCharacter.h"
#include "CrywolfSync.h"
#include "CastleSiege.h"
#include "Crywolf.h"
#include "IllusionTempleEvent.h"
#include "MasterSkillSystem.h"
#include "GameEvent.h"
#include "CashShop.h"
#include "ElementalSystem.h"
#include "LogToFile.h"
#include "TemporaryUserManager.h"
#include "PartyClass.h"
#include "Minimap.h"
#include "GuildMatch.h"
#include "Notice.h"
#include "MultiWareHouseSystem.h"

#if (ENABLETEST_ARCA == 1)
#include "ArcaBattle.h"
#endif

#if (ENABLETEST_ACHERON == 1)
#include "AcheronEvent.h"
#endif

#if (ENABLE_CUSTOM_HARDCORE == 1)
#include "HardcoreMode.h"
#endif

#if (ENABLETEST_RUMMY == 1)
#include "MuRummy.h"
#endif

#if (ENABLETEST_MUUN == 1)
#include "MuunSystem.h"
#endif

#if (DEBUG_DROP==1)
CLogToFile DROP_LOG_DEBUG(LOG_NAME_DROP, LOG_PATH_DROP, TRUE);
#endif

#if( __4GAMERS__ == 1 )
CLogToFile g_ResetDebug("Reset", ".\\Log\\Debug", TRUE);
#endif

void DataServerProtocolCore(BYTE protoNum, unsigned char* aRecv, int aLen)
{

#if( DEBUG_PROTOCOL == 1 )
		LogAddHeadHexFunc(1, (LPSTR)aRecv, aLen);
#endif

	switch ( protoNum )
	{
		case 0x00:
			DataServerLoginResult((SDHP_RESULT *)aRecv);
			break;

		case 0x01:
			JGPGetCharList(aRecv);
			break;

		case 0x04:
			JGCharacterCreateRequest((SDHP_CREATECHARRESULT *)aRecv);
			break;

		case 0x05:
			JGCharDelRequest((SDHP_CHARDELETERESULT *)aRecv);
			break;

		case 0x06:
			JGGetCharacterInfo((SDHP_DBCHAR_INFORESULT *)aRecv);
			break;

		case 0x08:
			DGGetWarehouseList((SDHP_GETWAREHOUSEDB_SAVE *)aRecv);
			break;

		case 0x10:
			GDGetWarehouseNoItem((SDHP_GETWAREHOUSEDB_RESULT *)aRecv);
			break;
		case 0x14:
			g_MonsterKillInfo.GDAnsMonsterKillInfo(aRecv);
			break;
#if (__QUEST_SYSTEM__== 1)
		case 0x16:
			gQuestInfoEx.DGAnsQuestInfo(aRecv);
			break;
#endif
#if (__GENS_SYSTEM__ == 1 )
		case 0x40:
			{
				PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;
				switch( lpDef->subcode )
				{
				case 0x00:
					gGensSystem.DGAnswerServerInfo(aRecv);
					break;
				case 0x01:
					gGensSystem.DGAnsGensInfo(aRecv);
					break;
				case 0x02:
					gGensSystem.DGAnsJoinRequest(aRecv);
					break;
				case 0x04:
					gGensSystem.DGAnsLeaveRequest(aRecv);
					break;
				}
			}
			break;
#endif
		case 0x30:
			g_MasterExp.GDAnsMasterInfo(aRecv);
			break;
		case 0x52:
			ItemSerialCreateRecv((SDHP_ITEMCREATERECV *)aRecv);
			break;

		case 0x56:
			DGRecvPetItemInfo(aRecv);
			break;

		case 0x60:
			DGOptionDataRecv((SDHP_SKILLKEYDATA_SEND *)aRecv);
			break;

		case 0x61:
			DGMuBotOptionRecv((LPMUBOT_SETTINGS_SEND)aRecv);
			break;

		case 0x80:
			{
				PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

				switch ( lpDef1->subcode )
				{
					case 0x00:
						GS_DGAnsCastleTotalInfo(aRecv);
						break;

					case 0x01:
						GS_DGAnsOwnerGuildMaster(aRecv);
						break;

					case 0x03:
						GS_DGAnsCastleNpcBuy(aRecv);
						break;

					case 0x04:
						GS_DGAnsCastleNpcRepair(aRecv);
						break;

					case 0x05:
						GS_DGAnsCastleNpcUpgrade(aRecv);
						break;

					case 0x06:
						GS_DGAnsTaxInfo(aRecv);
						break;

					case 0x07:
						GS_DGAnsTaxRateChange(aRecv);
						break;

					case 0x08:
						GS_DGAnsCastleMoneyChange(aRecv);
						break;

					case 0x09:
						GS_DGAnsSiegeDateChange(aRecv);
						break;

					case 0x0A:
						GS_DGAnsGuildMarkRegInfo(aRecv);
						break;

					case 0x0B:
						GS_DGAnsSiegeEndedChange(aRecv);
						break;

					case 0x0C:
						GS_DGAnsCastleOwnerChange(aRecv);
						break;

					case 0x0D:
						GS_DGAnsRegAttackGuild(aRecv);
						break;

					case 0x0E:
						GS_DGAnsRestartCastleState(aRecv);
						break;

					case 0x0F:
						GS_DGAnsMapSvrMsgMultiCast(aRecv);
						break;

					case 0x10:
						GS_DGAnsRegGuildMark(aRecv);
						break;

					case 0x11:
						GS_DGAnsGuildMarkReset(aRecv);
						break;

					case 0x12:
						GS_DGAnsGuildSetGiveUp(aRecv);
						break;

					case 0x16:
						GS_DGAnsNpcRemove(aRecv);
						break;

					case 0x17:
						GS_DGAnsCastleStateSync(aRecv);
						break;

					case 0x18:
						GS_DGAnsCastleTributeMoney(aRecv);
						break;

					case 0x19:
						GS_DGAnsResetCastleTaxInfo(aRecv);
						break;

					case 0x1A:
						GS_DGAnsResetSiegeGuildInfo(aRecv);
						break;

					case 0x1B:
						GS_DGAnsResetRegSiegeInfo(aRecv);
						break;
				}
			}
			break;

		case 0x81:
			GS_DGAnsCastleInitData(aRecv);
			break;

		case 0x82:
			GS_DGAnsCastleNpcInfo(aRecv);
			break;

		case 0x83:
			GS_DGAnsAllGuildMarkRegInfo(aRecv);
			break;

		case 0x84:
			GS_DGAnsFirstCreateNPC(aRecv);
			break;

		case 0x85:
			GS_DGAnsCalcREgGuildList(aRecv);
			break;

		case 0x86:
			GS_DGAnsCsGulidUnionInfo(aRecv);
			break;

		case 0x87:
			GS_DGAnsCsSaveTotalGuildInfo(aRecv);
			break;

		case 0x88:
			GS_DGAnsCsLoadTotalGuildInfo(aRecv);
			break;

		case 0x89:
			GS_DGAnsCastleNpcUpdate(aRecv);
			break;

		case 0xB0:
			DGAnsCrywolfSync(aRecv);
			break;

		case 0xB1:
			DGAnsCrywolfInfoLoad(aRecv);
			break;

		case 0xB2:
			DGAnsCrywolfInfoSave(aRecv);
			break;

		case 0xCF:
			DGMoveOtherServer((SDHP_CHARACTER_TRANSFER_RESULT *)aRecv);
			break;

		case 0xD0:
			{
				int subcode = -1;
				if( aRecv[0] == 0xC1 )
				{
					PMSG_DEFAULT2 * lpMsg = (PMSG_DEFAULT2 *)aRecv;
					subcode = lpMsg->subcode;
				}
				else if( aRecv[0] == 0xC2 )
				{
					PWMSG_HEAD2 * lpMsg = (PWMSG_HEAD2 *)aRecv;
					subcode = lpMsg->subcode;
				}
				else
				{
					break;
				}


				switch ( subcode )
				{
					case 0x02:
						g_CashItemPeriodSystem.DGAnsPeriodItemInquiry((PMSG_ANS_PERIODITEM_INQUIRY *)aRecv);
						break;

					case 0x04:
						g_CashItemPeriodSystem.DGAnsPeriodItemInsert((PMSG_ANS_PERIODITEM_INSERT *)aRecv);
						break;

					case 0x06:
						g_CashItemPeriodSystem.DGAnsPeriodItemList((PMSG_ANS_PERIODITEM_LIST *)aRecv);
						break;

					case 0x08:
						g_CashItemPeriodSystem.DGAnsPeriodItemUpdate((PMSG_ANS_PERIODITEM_UPDATE *)aRecv);
						break;
					case 0x0A:
						g_CashItemPeriodSystem.DGAnsPeriodItemDelete((PMSG_ANS_PERIODITEM_DELETE *)aRecv);
						break;
				}
			}
			break;
		case 0x72:
			JGPSummonerInfo((PMSG_ANS_SUMMONER_CREATE*)aRecv);
			break;
		case 0xA4:
			g_ElementalSystem.DGAnsErtelList((PMSG_ANS_ERTELLIST*)aRecv);
			break;
		case 0xEF:
			{
				int subcode = -1;
				if( aRecv[0] == 0xC1 )
				{
					PMSG_DEFAULT2 * lpMsg = (PMSG_DEFAULT2 *)aRecv;
					subcode = lpMsg->subcode;
				}
				else if( aRecv[0] == 0xC2 )
				{
					PWMSG_HEAD2 * lpMsg = (PWMSG_HEAD2 *)aRecv;
					subcode = lpMsg->subcode;
				}

				switch(subcode)
				{
				case 0x00:
					DGAnsPartyMatchReg((PMSG_PARTYMATCHING_REG_ANS*)aRecv);
					break;
				case 0x01:
					DGAnsPartyMatchInfo(aRecv);
					break;
				case 0x02:
					DGAnsPartyMatchJoin((PMSG_PARTYMATCHING_JOIN_ANS*)aRecv);
					break;
				case 0x03:
					DGAnsPartyMatchWaitlist((PMSG_PARTYMATCHING_WAIT_ANS*)aRecv);
					break;
				case 0x04:
					DGAnsPartyMatchAcceptList(aRecv);
					break;
				case 0x05:
					DGAnsPartyMatchRequestAnswer((PARTYMATCH_DGANS_REQUESTANSWER*)aRecv);
					break;
				case 0x06:
					DGAnsPartyMatchCancel((PMSG_PARTYMATCHING_CANCEL_ANS*)aRecv);
					break;
				}
			}
			break;
		case 0xED:
			{
				int subcode = -1;

				if( aRecv[0] == 0xC1 )
				{
					PMSG_DEFAULT2 * lpMsg = (PMSG_DEFAULT2 *)aRecv;
					subcode = lpMsg->subcode;
				}
				else if( aRecv[0] == 0xC2 )
				{
					PWMSG_HEAD2 * lpMsg = (PWMSG_HEAD2 *)aRecv;
					subcode = lpMsg->subcode;
				}
				
				switch(subcode)
				{
				case 0x00:
					g_GuildMatch.DGAnsOfferList(aRecv);
					break;
				case 0x02:
					g_GuildMatch.DGAnsOfferCreate((GUILDMATCH_DGANS_OFFERCREATE*)aRecv);
					break;
				case 0x03:
					g_GuildMatch.DGAnsOfferDelete((GUILDMATCH_DGANS_OFFERDELETE*)aRecv);
					break;
				case 0x04:
					g_GuildMatch.DGAnsJoinReq((GUILDMATCH_DGANS_JOINREQ*)aRecv);
					break;
				case 0x05:
					g_GuildMatch.DGAnsJoinReqDelete((GUILDMATCH_DGANS_OFFERDELETE*)aRecv);
					break;
				case 0x06:
					g_GuildMatch.DGAnsJoinReqAnswer((GUILDMATCH_DGANS_REQUESTANSWER*)aRecv);
					break;
				case 0x07:
					g_GuildMatch.DGAnsJoinReqList(aRecv);
					break;
				case 0x08:
					g_GuildMatch.DGAnsJoinReqStatus((GUILDMATCH_DGANS_JOINREQSTATUS*)aRecv);
					break;
				}
			}
			break;
#if (ENABLETEST_RUMMY == 1)
		case 0xE8:
			{
				if (aRecv[3] == 0)
				{
					g_CMuRummyMng.GDAnsCardInfo((_tagPMSG_ANS_MURUMMY_SELECT_DS*)aRecv);
				}
			}
			break;
#endif
		case 0xCD:
			{
				DGBroadCastGlobalMessage(aRecv);
			}
			break;
		case 0xFD:
			{
				int subcode = -1;
				
				if( aRecv[0] == 0xC1 )
				{
					PMSG_DEFAULT2 * lpMsg = (PMSG_DEFAULT2 *)aRecv;
					subcode = lpMsg->subcode;
				}
				else if( aRecv[0] == 0xC2 )
				{
					PWMSG_HEAD2 * lpMsg = (PWMSG_HEAD2 *)aRecv;
					subcode = lpMsg->subcode;
				}
				
				switch(subcode)
				{
					case 0x04:
						gWareHouseSYSTEM.DBSendVaultResult((PMSG_VAULT_RESULT*)aRecv);
						break;
				}
			}
			break;
		case 0xFE: 
			{
				int subcode = -1;
				if( aRecv[0] == 0xC1 )
				{
					PMSG_DEFAULT2 * lpMsg = (PMSG_DEFAULT2 *)aRecv;
					subcode = lpMsg->subcode;
				}
				else if( aRecv[0] == 0xC2 )
				{
					PWMSG_HEAD2 * lpMsg = (PWMSG_HEAD2 *)aRecv;
					subcode = lpMsg->subcode;
				}

				switch(subcode) {
		case 2:
			answerDbInfo((packetDbAnswerInfo*)aRecv);
			break;
				}
			 } break;
		case 0xFF:	// Test Packet
			{
				PMSG_TEST * pMsg = (PMSG_TEST *)aRecv;

				LogAddC(2, "error-L2DataServer RECV : %d (%d)", aLen, pMsg->count);
			}
			break;
	}
}

struct PMSG_SUMMONER_UNLOCK 
{
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE unlock;
};

void JGPSummonerInfo(PMSG_ANS_SUMMONER_CREATE* lpMsg)
{
	char szId[11];
	szId[10] = 0;
	memcpy(szId,lpMsg->szAccount,10);
	int aIndex = lpMsg->Number;

	if ( ::gObjIsAccontConnect(aIndex, szId) == FALSE )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 165)), gObj[aIndex].AccountID, szId);
		CloseClient(aIndex);

		return;
	}

#if (__CUSTOM__ == 1)
	lpMsg->Result = 1;
#endif

	if( lpMsg->Result == TRUE )
	{
		gObj[aIndex].m_iSummonerCreateFlag = TRUE;
	}
}



void TestDSSend()
{
	PMSG_TEST pMsg;
	int size;

	size = sizeof(PMSG_TEST);
	pMsg.h.c=0xC2;
	pMsg.h.headcode =0xFF;
	pMsg.h.sizeH = SET_NUMBERH(size);
	pMsg.h.sizeL = SET_NUMBERL(size);

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf );
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	wsprintf(pMsg.testbuf, "%d", iCount);
	LogAdd("%s", pMsg.testbuf);
	cDBSMng.Send((char *)&pMsg, size);
	iCount++;

	LogAdd("Test Data Send");
}





void DataServerLogin(int server)
{
	SDHP_SERVERINFO pInfo;

	pInfo.h.c =0xC1;
	pInfo.h.size = sizeof(SDHP_SERVERINFO);	// Size
	pInfo.h.headcode =0x00;
	pInfo.Port =(WORD)GameServerPort;
	pInfo.Type =1;
	pInfo.ServerCode =gGameServerCode;
	strcpy( (char*)&pInfo.ServerName[0], (const char*)&szServerName[0] );

	cDBSMng.Send(server, (char*)&pInfo , pInfo.h.size  );	
}






void DataServerLoginResult(SDHP_RESULT * lpMsg)
{
	if (lpMsg->Result == false )
	{
		MsgBox(lMsg.Get(MSGGET(1, 210)));	// Authentication error from authentication server
		return;
	}
	
	DataServerConnected = TRUE;
	gServerReady++;
	SendMessage(ghWnd, WM_START_SERVER, 0, 0); // Review the Message WM_USER +5
	
#if( ENABLE_CUSTOM_HARDCORE == 1 )
	g_HardcoreMode.Activate();
#endif
}








/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Char List Count Packet
 *	Direction : GameServer -> Client
 *  Code     : 0xC1
 *	HeadCode : 0xF3
 *	SubCode  : 0x00
 *	BASE PACKET - Complete with PMSG_CHARLIST
 */

#if(__ENG__ == 1)
#pragma pack(1)
struct PMSG_CHARLISTCOUNT
{
	PBMSG_HEAD h;	// C1:F3:00
	BYTE subcode;	// 3
	BYTE MaxClass;	// 4
	BYTE MoveCnt;	// 5
	BYTE Count;	// 6
#if (ENABLETEST_SEASON10DOT2 == 1)
	BYTE CharacterSlotCount;
#endif
	BYTE MultiWarehouse;
};
#pragma pack()
#else
#pragma pack(1)
struct PMSG_CHARLISTCOUNT
{
	PBMSG_HEAD h;	// C1:F3:00
	BYTE subcode;	// 3
	BYTE MaxClass;	// 4
	BYTE MoveCnt;	// 5
	BYTE Count;	// 6
	BYTE CharacterSlotCount;
	BYTE MultiWarehouse;
};
#pragma pack()
#endif


#if (__ENG__ == 1)
struct PMSG_CHARLIST
{
	BYTE Index;	// 0
	char Name[11];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
#if (ENABLETEST_SEASON10DOT2 == 1)
	BYTE PK_Level;
#endif
};
#else
struct PMSG_CHARLIST
{
	BYTE Index;	// 0
	char Name[10];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
};
#endif

struct PMSG_UNLOCKCHAR
{
	PBMSG_HEAD h;
	BYTE btDefault;
	BYTE btUnlock;
};

typedef struct
{
	PBMSG_HEAD h;
	BYTE btResult;
	BYTE IsSummonerEnable;
}PMSG_UPD_SUMMONER_CREATE, *LPPMSG_UPD_SUMMONER_CREATE;

void JGPGetCharList(unsigned char * lpRecv)
{
	SDHP_CHARLISTCOUNT * lpCount = (SDHP_CHARLISTCOUNT *)lpRecv;
	SDHP_CHARLIST * lpCL;
	char szId[MAX_ACCOUNT_LEN+1];
	BYTE sendbuf[256];
	int lOfs = 0;
	int lsOfs = sizeof(SDHP_CHARLISTCOUNT);
	int aIndex = lpCount->Number;
	BYTE index;
	PMSG_CHARLISTCOUNT pCLCount;	// Packet Char List Count
	PMSG_CHARLIST pCList;
	WORD TempInventory[12];

	pCLCount.h.c = 0xC1;
	pCLCount.h.headcode = 0xF3;
	pCLCount.subcode = 0x00;
	pCLCount.Count = lpCount->Count;

	szId[MAX_ACCOUNT_LEN] = 0;
	memcpy(szId, lpCount->AccountId, MAX_ACCOUNT_LEN);

	if ( ::gObjIsAccontConnect(aIndex, szId) == FALSE )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 165)), gObj[aIndex].AccountID, szId);
		CloseClient(aIndex);

		return;
	}

	gObj[aIndex].Magumsa		= 2;//lpCount->Magumsa;
	pCLCount.MaxClass			= lpCount->Magumsa + 2;
	pCLCount.MaxClass			+= 1;
	pCLCount.MoveCnt			= lpCount->MoveCnt;

#if (ENABLETEST_SEASON10DOT2 == 1)
	pCLCount.CharacterSlotCount = 5; // test
#endif

	gObj[aIndex].pWarehouseExtend = lpCount->btExtendVault;

	pCLCount.MaxClass = 0;
	pCLCount.MoveCnt = lpCount->MoveCnt;
	pCLCount.MultiWarehouse = lpCount->btExtendVault;

	memset(sendbuf, 0, sizeof(sendbuf));
	lOfs += sizeof(PMSG_CHARLISTCOUNT);

	BYTE GenerableClass = 0;

	pCLCount.MaxClass |= 3;
	gObj[aIndex].m_AccountMaxLevel = 1;

	if ( pCLCount.Count > 0 )
	{
		for ( int n=0;n<pCLCount.Count ;n++)
		{
			
			memset(&pCList, 0, sizeof(pCList));
			lpCL = (SDHP_CHARLIST *)&lpRecv[lsOfs];

			pCList.Index = lpCL->Index;
			pCList.Level = lpCL->Level;
			pCList.CtlCode = lpCL->CtlCode;
			pCList.btGuildStatus = lpCL->btGuildStatus;

			if( lpCL->Level > gObj[aIndex].m_AccountMaxLevel )
			{
				gObj[aIndex].m_AccountMaxLevel = lpCL->Level;
			}

			/*if( lpCL->Level >= 250 && pCLCount.MaxClass < 3 )
				pCLCount.MaxClass |= 3;
			else if( lpCL->Level >= 220 && pCLCount.MaxClass < 2 )
				pCLCount.MaxClass |= 2;
			else if( lpCL->Level >= 150 && pCLCount.MaxClass < 1 )
				pCLCount.MaxClass |= 1;*/
	
			if ( gObj[aIndex].m_cAccountItemBlock != 0 )
				pCList.CtlCode |= 0x10;	// Set Block Item

			memcpy(pCList.Name, lpCL->Name, MAX_ACCOUNT_LEN);
			
#if (ENABLETEST_SEASON10DOT2 == 1)
			pCList.CharSet[CS_CLASS] = lpCL->Class & 0xF0;
			int changeup = lpCL->Class & 1;
			int changeup2 = (lpCL->Class >> 1) & 1;
			if (changeup2) {
				changeup = 1;
			}
			pCList.CharSet[CS_CLASS] |= (changeup2 << 2) & 0x4;
			pCList.CharSet[CS_CLASS] |= (changeup << 3) & 0x8;
#else
			pCList.CharSet[CS_CLASS] = CS_GET_CLASS(lpCL->Class);
			int changeup = CS_GET_CHANGEUP(lpCL->Class);
			pCList.CharSet[CS_CLASS] |= CS_SET_CHANGEUP(changeup);
			pCList.CharSet[CS_CLASS] |= ((changeup >> 1) << 3) & 8;
#endif

			pCList.CharSet[9] = 0;
			if ( lpCL->DbVersion == 0 )
			{
				if ( lpCL->dbInventory[0] != (BYTE)-1 )
				{
					WORD wItemCode = lpCL->dbInventory[0] / MAX_TYPE_ITEMS * MAX_SUBTYPE_ITEMS + lpCL->dbInventory[0] % MAX_TYPE_ITEMS;
					pCList.CharSet[1] = (BYTE)wItemCode;
					pCList.CharSet[12] |= DBI_GET_TYPE(wItemCode);
				}
				else
				{
					pCList.CharSet[1] = -1;
					pCList.CharSet[12] |= 0xF0;
				}

				if ( lpCL->dbInventory[2] != (BYTE)-1 )
				{
					WORD wItemCode = lpCL->dbInventory[2] / MAX_TYPE_ITEMS * MAX_SUBTYPE_ITEMS + lpCL->dbInventory[2] % MAX_TYPE_ITEMS;
					pCList.CharSet[2] = (BYTE)wItemCode;
					pCList.CharSet[13] |= DBI_GET_TYPE(wItemCode);
				}
				else
				{
					pCList.CharSet[2] = -1;
					pCList.CharSet[13] |= 0xF0;
				}

				if ( lpCL->dbInventory[4] == (BYTE)-1 )
				{
					index = 0xF0;
				}
				else
				{
					index = (lpCL->dbInventory[4] & 0x0F) * MAX_TYPE_ITEMS;
				}

				if ( lpCL->dbInventory[6] == (BYTE)-1 )
				{
					index |= 0x0F;
				}
				else
				{
					index |= (lpCL->dbInventory[6] & 0x0F);
				}

				pCList.CharSet[3] = index;

				if ( lpCL->dbInventory[8] == (BYTE)-1 )
				{
					index = 0xF0;
				}
				else
				{
					index = (lpCL->dbInventory[8] & 0x0F) * MAX_TYPE_ITEMS;
				}

				if ( lpCL->dbInventory[10] == (BYTE)-1 )
				{
					index |= 0x0F;
				}
				else
				{
					index |= (lpCL->dbInventory[10] & 0x0F);
				}

				pCList.CharSet[4] = index;

				if ( lpCL->dbInventory[12] == (BYTE)-1 )
				{
					index = 0xF0;
				}
				else
				{
					index = (lpCL->dbInventory[12] & 0x0F) * MAX_TYPE_ITEMS;
				}

				if ( lpCL->dbInventory[14] == (BYTE)-1 )
				{
					index |= 0x0C;
				}
				else
				{
					index |= ((lpCL->dbInventory[14] & 0x03) * 4 );
				}

				if ( lpCL->dbInventory[16] == (BYTE)-1 )
				{
					index |= 0x03;
				}
				else
				{
					index |= (lpCL->dbInventory[16] & 0x03);
				}

				pCList.CharSet[5] = index;

				if ( lpCL->dbInventory[4] == (BYTE)-1 )
					pCList.CharSet[9] = 0x80;

				if ( lpCL->dbInventory[6] == (BYTE)-1 )
					pCList.CharSet[9] |= 0x40;

				if ( lpCL->dbInventory[8] == (BYTE)-1 )
					pCList.CharSet[9] |= 0x20;

				if ( lpCL->dbInventory[10] == (BYTE)-1 )
					pCList.CharSet[9] |= 0x10;

				if ( lpCL->dbInventory[12] == (BYTE)-1 )
					pCList.CharSet[9] |= 0x08;

				int levelindex = LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[1]));
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[3])) << 3;
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[5])) << 6;
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[7])) << 9;
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[9])) << 12;
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[11])) << 15;
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[13])) << 18;

				pCList.CharSet[6] = levelindex >> 16 & 0xFF;
				pCList.CharSet[7] = (levelindex >> 8) & 0xFF;
				pCList.CharSet[8] = (levelindex ) & 0xFF;
				pCList.CharSet[10] = 0;
				pCList.CharSet[11] = 0;

				if ( lpCL->dbInventory[4] == (BYTE)-1 )
					pCList.CharSet[13] |= 0x0F;

				if ( lpCL->dbInventory[8] == (BYTE)-1 )
					pCList.CharSet[14] |= 0xF0;

				if ( lpCL->dbInventory[10] == (BYTE)-1 )
					pCList.CharSet[14] |= 0x0F;

				if ( lpCL->dbInventory[12] == (BYTE)-1 )
					pCList.CharSet[15] |= 0xF0;

				if ( lpCL->dbInventory[14] == (BYTE)-1 )
					pCList.CharSet[15] |= 0x0F;

				pCList.CharSet[16] = 0;
				pCList.CharSet[17] = 0;
			}
			else if ( lpCL->DbVersion <= 2 )
			{
				if ( lpCL->dbInventory[0] != (BYTE)-1 )
				{
					TempInventory[0] = (lpCL->dbInventory[0] / 32) * MAX_SUBTYPE_ITEMS + lpCL->dbInventory[0] % 32;
				}
				else
				{
					TempInventory[0] = -1;
				}

				if ( lpCL->dbInventory[3] != (BYTE)-1 )
				{
					TempInventory[1] = (lpCL->dbInventory[3] / 32) * MAX_SUBTYPE_ITEMS + lpCL->dbInventory[3] % 32;
				}
				else
				{
					TempInventory[1] = -1;
				}

				if ( lpCL->dbInventory[6] == (BYTE)-1 && (lpCL->dbInventory[8] & 0x80) == 0x80)
				{
					TempInventory[2] = -1;
				}
				else
				{
					TempInventory[2] = (lpCL->dbInventory[6] + ((lpCL->dbInventory[8] >> 7) <<  8) ) % 32;
				}

				if ( lpCL->dbInventory[9] == (BYTE)-1 && (lpCL->dbInventory[11] & 0x80) == 0x80)
				{
					TempInventory[3] = -1;
				}
				else
				{
					TempInventory[3] = (lpCL->dbInventory[9] + ((lpCL->dbInventory[11] >> 7) <<  8) ) % 32;
				}

				if ( lpCL->dbInventory[12] == (BYTE)-1 && (lpCL->dbInventory[14] & 0x80) == 0x80)
				{
					TempInventory[4] = 0x1F;
				}
				else
				{
					TempInventory[4] = (lpCL->dbInventory[12] + ((lpCL->dbInventory[14] >> 7) <<  8) ) % 32;
				}

				if ( lpCL->dbInventory[15] == (BYTE)-1 && (lpCL->dbInventory[17] & 0x80) == 0x80)
				{
					TempInventory[5] = 0x1F;
				}
				else
				{
					TempInventory[5] = (lpCL->dbInventory[15] + ((lpCL->dbInventory[17] >> 7) <<  8) ) % 32;
				}

				if ( lpCL->dbInventory[18] == (BYTE)-1 && (lpCL->dbInventory[20] & 0x80) == 0x80)
				{
					TempInventory[6] = 0x1F;
				}
				else
				{
					TempInventory[6] = (lpCL->dbInventory[18] + ((lpCL->dbInventory[20] >> 7) <<  8) ) % 32;
				}

				if ( lpCL->dbInventory[21] == (BYTE)-1 && (lpCL->dbInventory[23] & 0x80) == 0x80)
				{
					TempInventory[7] = 0x1F;
				}
				else
				{
					TempInventory[7] = (lpCL->dbInventory[21] + ((lpCL->dbInventory[23] >> 7) <<  8) ) % 32;
				}

				if ( lpCL->dbInventory[24] == (BYTE)-1 && (lpCL->dbInventory[26] & 0x80) == 0x80)
				{
					TempInventory[8] = 0x1F;
				}
				else
				{
					TempInventory[8] = (lpCL->dbInventory[24] + ((lpCL->dbInventory[26] >> 7) <<  8) ) % 32;
				}

				if ( TempInventory[0] != (WORD)-1 )
				{
					pCList.CharSet[1] = (BYTE)TempInventory[0];
					pCList.CharSet[12] |= DBI_GET_TYPE(TempInventory[0]);
				}
				else
				{
					pCList.CharSet[1] = -1;
					pCList.CharSet[12] |= 0xF0;
				}

				if ( TempInventory[1] != (WORD)-1 )
				{
					pCList.CharSet[2] = (BYTE)TempInventory[1];
					pCList.CharSet[13] |= DBI_GET_TYPE(TempInventory[1]);
				}
				else
				{
					pCList.CharSet[2] = -1;
					pCList.CharSet[13] |= 0xF0;
				}

				pCList.CharSet[3] = TempInventory[2] % MAX_TYPE_ITEMS * MAX_TYPE_ITEMS + TempInventory[3] % MAX_TYPE_ITEMS;
				pCList.CharSet[4] = TempInventory[4] % MAX_TYPE_ITEMS * MAX_TYPE_ITEMS + TempInventory[5] % MAX_TYPE_ITEMS;
				index = TempInventory[6] % MAX_TYPE_ITEMS * MAX_TYPE_ITEMS;
				
				if (TempInventory[7]== 0x1F )
				{
					index |= 0x0C;
				}
				else
				{
					index |= (TempInventory[7] & 0x03) * 4;
				}

				if ( (TempInventory[8] & 0x04) == 0 )
				{
					if ( TempInventory[8] == 0x1F )
					{
						index |= 0x03;
					}
					else
					{
						index |= TempInventory[8] & 0x03;
					}
				}
				else
				{
					index |= 0x03;
				}

				pCList.CharSet[5] = index;
				int levelindex = 0;

				if ( TempInventory[0] != (WORD)-1 )
				{
					levelindex = LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[1]));
				}

				if ( TempInventory[1] != (WORD)-1 )
				{
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[4]))<<3;
				}

				if ( TempInventory[2] < 0x1F )
				{	
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[7]))<<6;
				}

				if ( TempInventory[3] < 0x1F )
				{	
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[10]))<<9;
				}

				if ( TempInventory[4] < 0x1F )
				{	
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[13]))<<12;
				}

				if ( TempInventory[5] < 0x1F )
				{	
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[16]))<<15;
				}

				if ( TempInventory[6] < 0x1F )
				{	
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[19]))<<18;
				}

				pCList.CharSet[6] = ((int)levelindex>>16 ) & 0xFF;
				pCList.CharSet[7] = ((int)levelindex>>8 ) & 0xFF;
				pCList.CharSet[8] = levelindex & 0xFF;

				if ( TempInventory[2] > 0x0F )
					pCList.CharSet[9] = 0x80;

				if ( TempInventory[3] > 0x0F )
					pCList.CharSet[9] |= 0x40;

				if ( TempInventory[4] > 0x0F )
					pCList.CharSet[9] |= 0x20;

				if ( TempInventory[5] > 0x0F )
					pCList.CharSet[9] |= 0x10;

				if ( TempInventory[6] > 0x0F )
					pCList.CharSet[9] |= 0x8;

				if ( (TempInventory[7] >= 3 && TempInventory[7] <= 6) || TempInventory[7] == 0x1E)
				{
					pCList.CharSet[5] |= 0x0C;
					
					if ( TempInventory[7] == 0x1E )
					{
						pCList.CharSet[9] |= 0x05;
					}
					else
					{
						pCList.CharSet[9] |= (TempInventory[7]-2) & 0x07;
					}
				}

				if ( (TempInventory[7] >= 0x24 && TempInventory[7] <= 0x28) || TempInventory[7] == 0x2B )
				{
					pCList.CharSet[5] |= 0x0C;
					pCList.CharSet[16] |= ((TempInventory[7] - 0x23) & 7) << 2;

					if( TempInventory[7] == 0x2B )
					{
						pCList.CharSet[16] |= 0x18;
					}
				}

				pCList.CharSet[10] = 0;

				if ( (TempInventory[8] & 0x03) != 0 && TempInventory[8] != 0x1F )
					pCList.CharSet[10] |= 1;

				pCList.CharSet[11] = 0;

				if ( (TempInventory[8] & 0x04) != 0 && TempInventory[8] != 0x1F )
					pCList.CharSet[12] |= 1;

				if ( TempInventory[2] >= 0x1F ) pCList.CharSet[13] |= 0x0F;
				if ( TempInventory[3] >= 0x1F ) pCList.CharSet[14] |= 0xF0;
				if ( TempInventory[4] >= 0x1F ) pCList.CharSet[14] |= 0x0F;
				if ( TempInventory[5] >= 0x1F ) pCList.CharSet[15] |= 0xF0;
				if ( TempInventory[6] >= 0x1F ) pCList.CharSet[15] |= 0x0F;

				pCList.CharSet[16] = 0;
				pCList.CharSet[17] = 0;
			}
			else	// Case DB 3
			{
				if ( lpCL->dbInventory[0] == (BYTE)-1 && (lpCL->dbInventory[2]& 0x80) == 0x80 && (lpCL->dbInventory[3]& 0xF0) == 0xF0  )
				{
					TempInventory[0] = -1;
				}
				else
				{
					TempInventory[0] = (lpCL->dbInventory[0] + (lpCL->dbInventory[2]&0x80)*2)+ (lpCL->dbInventory[3]&0xF0)*32;
				}

				if ( lpCL->dbInventory[4] == (BYTE)-1 && (lpCL->dbInventory[6]& 0x80) == 0x80 && (lpCL->dbInventory[7]& 0xF0) == 0xF0  )
				{
					TempInventory[1] = -1;
				}
				else
				{
					TempInventory[1] = (lpCL->dbInventory[4] + (lpCL->dbInventory[6]&0x80)*2)+ (lpCL->dbInventory[7]&0xF0)*32;
				}

				if ( lpCL->dbInventory[8] == (BYTE)-1 && (lpCL->dbInventory[10]& 0x80) == 0x80 && (lpCL->dbInventory[11]& 0xF0) == 0xF0  )
				{
					TempInventory[2] = 0x1FF;
				}
				else
				{
					TempInventory[2] = ((lpCL->dbInventory[8] + (lpCL->dbInventory[10]&0x80)*2)+ (lpCL->dbInventory[11]&0xF0)*32)%MAX_SUBTYPE_ITEMS;
				}

				if ( lpCL->dbInventory[12] == (BYTE)-1 && (lpCL->dbInventory[14]& 0x80) == 0x80 && (lpCL->dbInventory[15]& 0xF0) == 0xF0  )
				{
					TempInventory[3] = 0x1FF;
				}
				else
				{
					TempInventory[3] = ((lpCL->dbInventory[12] + (lpCL->dbInventory[14]&0x80)*2)+ (lpCL->dbInventory[15]&0xF0)*32)%MAX_SUBTYPE_ITEMS;
				}

				if ( lpCL->dbInventory[16] == (BYTE)-1 && (lpCL->dbInventory[18]& 0x80) == 0x80 && (lpCL->dbInventory[19]& 0xF0) == 0xF0  )
				{
					TempInventory[4] = 0x1FF;
				}
				else
				{
					TempInventory[4] = ((lpCL->dbInventory[16] + (lpCL->dbInventory[18]&0x80)*2)+ (lpCL->dbInventory[19]&0xF0)*32)%MAX_SUBTYPE_ITEMS;
				}

				if ( lpCL->dbInventory[20] == (BYTE)-1 && (lpCL->dbInventory[22]& 0x80) == 0x80 && (lpCL->dbInventory[23]& 0xF0) == 0xF0  )
				{
					TempInventory[5] = 0x1FF;
				}
				else
				{
					TempInventory[5] = ((lpCL->dbInventory[20] + (lpCL->dbInventory[22]&0x80)*2)+ (lpCL->dbInventory[23]&0xF0)*32)%MAX_SUBTYPE_ITEMS;
				}

				if ( lpCL->dbInventory[24] == (BYTE)-1 && (lpCL->dbInventory[26]& 0x80) == 0x80 && (lpCL->dbInventory[27]& 0xF0) == 0xF0  )
				{
					TempInventory[6] = 0x1FF;
				}
				else
				{
					TempInventory[6] = ((lpCL->dbInventory[24] + (lpCL->dbInventory[26]&0x80)*2)+ (lpCL->dbInventory[27]&0xF0)*32)%MAX_SUBTYPE_ITEMS;
				}

				if ( lpCL->dbInventory[28] == (BYTE)-1 && (lpCL->dbInventory[30]& 0x80) == 0x80 && (lpCL->dbInventory[31]& 0xF0) == 0xF0  )
				{
					TempInventory[7] = 0x1FF;
				}
				else
				{
					TempInventory[7] = ((lpCL->dbInventory[28] + (lpCL->dbInventory[30]&0x80)*2)+ (lpCL->dbInventory[31]&0xF0)*32)%MAX_SUBTYPE_ITEMS;
				}

				if ( lpCL->dbInventory[32] == (BYTE)-1 && (lpCL->dbInventory[34]& 0x80) == 0x80 && (lpCL->dbInventory[35]& 0xF0) == 0xF0  )
				{
					TempInventory[8] = 0x1FF;
				}
				else
				{
					TempInventory[8] = ((lpCL->dbInventory[32] + (lpCL->dbInventory[34]&0x80)*2)+ (lpCL->dbInventory[35]&0xF0)*32)%MAX_SUBTYPE_ITEMS;
				}

				pCList.CharSet[12] |= DBI_GET_TYPE(TempInventory[0]);
				pCList.CharSet[1] = TempInventory[0] % 256;

				pCList.CharSet[13] |= DBI_GET_TYPE(TempInventory[1]);
				pCList.CharSet[2] = TempInventory[1] % 256;

				pCList.CharSet[13] |= (int)(TempInventory[2] & 0x1E0) >> 5;
				pCList.CharSet[9] |= (int)(TempInventory[2] & 0x10) << 3;
				pCList.CharSet[3] |= (int)(TempInventory[2] & 0x0F) << 4;

				pCList.CharSet[14] |= (int)(TempInventory[3] & 0x1E0) >> 1;
				pCList.CharSet[9] |= (int)(TempInventory[3] & 0x10) << 2;
				pCList.CharSet[3] |= (int)(TempInventory[3] & 0x0F);

				pCList.CharSet[14] |= (int)(TempInventory[4] & 0x1E0) >> 5;
				pCList.CharSet[9] |= (int)(TempInventory[4] & 0x10) << 1;
				pCList.CharSet[4] |= (int)(TempInventory[4] & 0x0F) << 4;

				pCList.CharSet[15] |= (int)(TempInventory[5] & 0x1E0) >> 1;
				pCList.CharSet[9] |= (int)(TempInventory[5] & 0x10);
				pCList.CharSet[4] |= (int)(TempInventory[5] & 0x0F);

				pCList.CharSet[15] |= (int)(TempInventory[6] & 0x1E0) >> 5;
				pCList.CharSet[9] |= (int)(TempInventory[6] & 0x10) >> 1;
				pCList.CharSet[5] |= (int)(TempInventory[6] & 0x0F) << 4;

				index = 0;

				if ( TempInventory[7] == 0x1FF )
				{
					index = 0;
				}
				else
				{
					if( TempInventory[7] >= 0 && TempInventory[7] <= 2 )
					{
						index |= 4;
						pCList.CharSet[9] |= TempInventory[7]+1;
					}
					else if( TempInventory[7] == 41 )
					{
						index |= 4;
						pCList.CharSet[9] |= 4;
					}
					else if( TempInventory[7] == 266 )
					{
						index |= 4;
						pCList.CharSet[9] |= 5;
					}
					else if( TempInventory[7] == 267 )
					{
						index |= 4;
						pCList.CharSet[9] |= 6;
					}
				}

				if ( (TempInventory[8] & 0x04) == 0 )
				{
					if ( TempInventory[8] == 0x1FF )
					{
						index |= 0x03;
					}
					else if( (TempInventory[8] & 64) == 0 )
					{
						index |= (TempInventory[8] ) & 0x03;
					}
				}
				else
				{
					index |= 0x03;
				}

				pCList.CharSet[5] |= index;

				int levelindex = 0;

				if ( TempInventory[0] != (WORD)-1 )
				{
					levelindex = LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[1]));
				}

				if ( TempInventory[1] != (WORD)-1 )
				{
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[5])) << 3;
				}

				if ( TempInventory[2] < 0x1FF )
				{
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[9])) << 6;
				}

				if ( TempInventory[3] < 0x1FF )
				{
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[13])) << 9;
				}

				if ( TempInventory[4] < 0x1FF )
				{
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[17])) << 12;
				}

				if ( TempInventory[5] < 0x1FF )
				{
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[21])) << 15;
				}

				if ( TempInventory[6] < 0x1FF )
				{
					levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[25])) << 18;
				}

				pCList.CharSet[6] = ((int)levelindex >> 0x10) & 0xFF;
				pCList.CharSet[7] = ((int)levelindex >> 0x08) & 0xFF;
				pCList.CharSet[8] = ((int)levelindex) & 0xFF;

				if ( (TempInventory[7] >= 3 && TempInventory[7] <= 6) || TempInventory[7] == 0x1E || 
					  TempInventory[7] == 0x2A || TempInventory[7] == 0x31 )
				{
					pCList.CharSet[5] |= 0x08;

					if ( TempInventory[7] == 0x1E )
					{
						pCList.CharSet[9] |= 0x05;
					}
					else if( TempInventory[7] == 0x2A )
					{
						pCList.CharSet[9] |= 0x06;
					}
					else if( TempInventory[7] == 0x31 )
					{
						pCList.CharSet[9] |= 0x07;
					}
					else
					{
						pCList.CharSet[9] |= (TempInventory[7] - 2) & 0x07;
					}
				}

				pCList.CharSet[10] =0;

				if ( (TempInventory[8]& 0x03) != 0 && TempInventory[8] != 0x1FF )
				{
					pCList.CharSet[10] |= 0x01;
				}

				pCList.CharSet[11] = 0;

				if ( (TempInventory[8]& 0x04) != 0 && TempInventory[8] != 0x1FF )
				{
					pCList.CharSet[12] |= 0x01;
				}

				pCList.CharSet[16] = 0;
				pCList.CharSet[17] = 0;

				if ( TempInventory[7] >= 0x106 && TempInventory[7] <= 0x109 )
				{
					pCList.CharSet[5] |= 0x08;
					pCList.CharSet[16] = ((TempInventory[7]-0x105)&7)<<2;
				}

				if ( TempInventory[8] == 0x25 && TempInventory[8] != 0x1FF )
				{
					pCList.CharSet[10] &= 0xFE;
					pCList.CharSet[12] &= 0xFE;
					pCList.CharSet[12] |= 0x04;
					BYTE btExcellentOption = lpCL->dbInventory[34] & 0x3F;

					if ( (btExcellentOption&1) == 1 )
					{
						pCList.CharSet[16] |= 0x01;
					}

					if ( (btExcellentOption&2) == 2 )
					{
						pCList.CharSet[16] |= 0x02;
					}

					if ( (btExcellentOption&4) == 4 )
					{
						pCList.CharSet[17] |= 0x01;
					}
				}

				if ( (TempInventory[7] >= 0x24 && TempInventory[7] <= 0x28) 
					|| TempInventory[7] == 0x2B || TempInventory[7] == 0x32 )
				{
					pCList.CharSet[5] |= 0x0C;
					pCList.CharSet[9] |= (TempInventory[7] - 0x23) & 7;

					if( TempInventory[7] == 0x2B )
					{
						pCList.CharSet[9] |= 0x06;
					}
					else if( TempInventory[7] == 0x32 )
					{
						pCList.CharSet[9] |= 0x07;
					}
				}

				if( TempInventory[7] >= 130 && TempInventory[7] <= 135 )
				{
					pCList.CharSet[5] |= CS_SET_WING(3);
					pCList.CharSet[17] |= ((TempInventory[7]-1)&7)*32;
				}

				if ( (TempInventory[8] & 4) == FALSE && TempInventory[8] != 0x1FF )
				{
					BYTE Helper = 0;

					switch( TempInventory[8] )
					{
					case 0x40:
						Helper = 0x20;
						break;
					case 0x41:
						Helper = 0x40;
						break;
					case 0x43:
						Helper = 0x80;
						break;
					case 0x50:
						Helper = 0xE0;
						break;
					case 0x6A:
						Helper = 0xA0;
						break;
					case 0x7B:
						Helper = 0x60;
						break;
					}

					pCList.CharSet[16] |= Helper;
				}

#if (CUSTOM_WINGS == 1)
				if ( TempInventory[7] >= 440 && TempInventory[7] <= 445 )
				{
					pCList.CharSet[17] |= 0x10 + TempInventory[7]%440;
				}
#endif
			}

			memcpy(&sendbuf[lOfs], &pCList, sizeof(pCList));
			lsOfs += sizeof(SDHP_CHARLIST);
			lOfs += sizeof(pCList);
		}
	}

	pCLCount.h.size = lOfs;
	memcpy(sendbuf, &pCLCount, sizeof(PMSG_CHARLISTCOUNT));
	DataSend(aIndex, sendbuf, lOfs);

		if( gObj[aIndex].m_AccountMaxLevel >= g_CreateCharLevelSUM )
		{
			GenerableClass += 1;
		}
		// ----
		if( gObj[aIndex].m_AccountMaxLevel >= g_CreateCharLevelRF )
		{
			GenerableClass += 8;
		}
		// ----
		if( gObj[aIndex].m_AccountMaxLevel >= g_CreateCharLevelMG )
		{
			GenerableClass += 4;
		}
		// ----
		if( gObj[aIndex].m_AccountMaxLevel >= g_CreateCharLevelDL )
		{
			GenerableClass += 2;
		}
		// ----
		PMSG_UPD_SUMMONER_CREATE pMsg = { 0 };
		pMsg.h.set((LPBYTE)&pMsg, 0xDE, sizeof(pMsg));
		pMsg.btResult = 0;
		pMsg.IsSummonerEnable = GenerableClass;
		// ----
		DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));

	
}





/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Get Char List Request Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x01
 */
struct SDHP_GETCHARLIST
{
	PBMSG_HEAD h;	// C1:01
	char Id[10];	// 3
	short Number;	// E
};




void DataServerGetCharListRequest(short aIndex)
{
	SDHP_GETCHARLIST pMsg;
	char TempId[11];
	char* AccountId;

	memset(TempId, 0, sizeof(TempId));
	AccountId = gObjGetAccountId(aIndex);

	if ( AccountId == 0 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 166)), __FILE__, __LINE__);
		CloseClient(aIndex);
		return;
	}

	// When Len is Less than 1
	if ( strlen(AccountId) < 1 )
	{
		// Error Message
		//
		LogAdd(lMsg.Get(MSGGET(1, 167)), __FILE__, __LINE__);
		CloseClient(aIndex);
		return;
	}

	strcpy((char*)TempId, AccountId);

	pMsg.h.c  = 0xC1;
	pMsg.h.size  = sizeof(SDHP_GETCHARLIST);
	pMsg.h.headcode = 0x01;
	pMsg.Number = aIndex;
	memcpy(pMsg.Id , TempId, sizeof(TempId)-1);

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
	
}


void JGCharacterCreateRequest(SDHP_CREATECHARRESULT* lpMsg)
{
	PMSG_CHARCREATERESULT pResult;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	int aIndex = lpMsg->Number;

	szAccountID[MAX_ACCOUNT_LEN] = 0;
	memcpy(szAccountID, lpMsg->AccountId, sizeof(lpMsg->AccountId));

	if ( gObjIsAccontConnect(aIndex, szAccountID) == FALSE )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 168)), szAccountID);
		CloseClient(aIndex);
		return;
	}

	pResult.h.c = 0xC1;
	pResult.h.size = sizeof(PMSG_CHARCREATERESULT);
	pResult.h.headcode = 0xF3;
	pResult.subcode = 0x01;
	pResult.Result = lpMsg->Result;
	pResult.pos = lpMsg->Pos;
	pResult.Class = (lpMsg->ClassSkin >> 4 << 5) & 0xE0;
	int changeup = lpMsg->ClassSkin & 0x07; 
	pResult.Class |= (changeup << 4) & 0x10;
	pResult.Level = lpMsg->Level;
	memcpy(pResult.Name, lpMsg->Name, sizeof(pResult.Name));

	if ( pResult.Result == 1 )
	{
		memcpy(pResult.Equipment, lpMsg->Equipment, sizeof(pResult.Equipment));
	}

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
}



void JGCharacterCreateFailSend(int aIndex, char* id)
{
	PMSG_CHARCREATERESULT  pResult;

	pResult.h.c = 0xC1;
	pResult.h.size = sizeof (PMSG_CHARCREATERESULT);
	pResult.h.headcode = 0xF3;
	pResult.subcode = 0x01;
	pResult.Result = false;
	pResult.pos = 0;
	memcpy(pResult.Name, id, sizeof(pResult.Name));

	DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size);
}








void JGCharDelRequest(SDHP_CHARDELETERESULT * lpMsg)
{
	PMSG_RESULT pResult;
	char szAccountId[MAX_ACCOUNT_LEN+1];
	int aIndex = lpMsg->Number;
	szAccountId[MAX_ACCOUNT_LEN] = 0;
	memcpy(szAccountId, lpMsg->AccountID, sizeof(lpMsg->AccountID));

	if ( ::gObjIsAccontConnect(aIndex, szAccountId) == FALSE )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 169)), szAccountId);
		CloseClient(aIndex);

		return;
	}

	pResult.h.c = 0xC1;
	pResult.h.size = sizeof(PMSG_RESULT);
	pResult.h.headcode = 0xF3;
	pResult.subcode = 0x02;
	pResult.result = lpMsg->Result;

	DataSend(lpMsg->Number, (UCHAR*)&pResult, pResult.h.size);
}





/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Get Character Info Packet
 *	Direction : GameServer -> Client
 *  Code     : 0xC3
 *	HeadCode : 0xF3
 *	SubCode  : 0x03
 */
struct PMSG_CHARMAPJOINRESULT
{
	PBMSG_HEAD h;	// C3:F3:03
	BYTE subcode;	// 3
	BYTE MapX;	// 4
	BYTE MapY;	// 5
	BYTE MapNumber;	// 6
	BYTE Dir;	// 7

	//__int64 Exp;	// 8
	//__int64 NextExp;	// C

	BYTE Exp[8];
	BYTE NextExp[8];

	WORD LevelUpPoint;	// 10
	WORD Str;	// 12
	WORD Dex;	// 14
	WORD Vit;	// 16
	WORD Energy;	// 18
	WORD Life;	// 1A
	WORD MaxLife;	// 1C
	WORD Mana;	// 1E
	WORD MaxMana;	// 20
	WORD wShield;	// 22
	WORD wMaxShield;	// 24
	WORD BP;
	WORD MaxBP;	// 26
	WORD Unk;	// 28
	int Money;	// 2C
	BYTE PkLevel;	// 30
	BYTE CtlCode;	// 31
	short AddPoint;	// 32
	short MaxAddPoint;	// 34
	WORD Leadership;	// 36
	WORD wMinusPoint;	// 38
	WORD wMaxMinusPoint;	// 3A
	BYTE btInventoryExpansion;

#if (FIX_HP==1)
	int HP;
	int MAXHP;
	int MP;
	int MAXMP;
	int SD;
	int MAXSD;
#endif
};

void JGGetCharacterInfo( SDHP_DBCHAR_INFORESULT * lpMsg)
{
	PMSG_CHARMAPJOINRESULT pjMsg;

	char szAccountId[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	int aIndex = lpMsg->Number;

	szAccountId[MAX_ACCOUNT_LEN] = 0;
	memcpy(szAccountId, lpMsg->AccountID, sizeof(lpMsg->AccountID));

	if ( gObjIsAccontConnect(aIndex, szAccountId) == FALSE )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 170)), szAccountId);
		CloseClient(aIndex);

		return;
	}

	szName[MAX_ACCOUNT_LEN] = 0;
	memcpy(szName, lpMsg->Name, MAX_ACCOUNT_LEN);

	if ( lpMsg->result == false )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 171)), szName, gObj[aIndex].AccountID);
		CloseClient(aIndex);

		return;
	}

	if ( (lpMsg->CtlCode&1) != 0 )
	{
		LogAddC(2, "error-L1 : Load Block Character infomation", szName, gObj[aIndex].AccountID);	// Deathway Fix HERE
		CloseClient(aIndex);

		return;
	}

	for (int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
	{
		if ( gObj[i].Connected == PLAYER_PLAYING )
		{
			if ( gObj[i].Type == OBJ_USER )
			{
				if ( !strncmp(szName, gObj[i].Name, MAX_ACCOUNT_LEN) || !strncmp(szAccountId, gObj[i].AccountID, MAX_ACCOUNT_LEN))
				{
					LogAddTD("[Anti-HACK][JGGetCharacterInfo] Attempted Character-Copy by double logging [%s][%s]",
						szName, gObj[aIndex].AccountID);
					CloseClient(aIndex);
					return;
				}
			}
		}
	}



	if ( gObjSetCharacter( (UCHAR*)lpMsg, aIndex) == FALSE )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 172)), szName);
		CloseClient(aIndex);
		return;
	}

	gObj[aIndex].iShield = gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield;

	LPOBJ lpObj = &gObj[aIndex];

	if ( gObj[aIndex].m_Index != aIndex )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 173)), __FILE__, __LINE__);
		CloseClient(aIndex);

		return;
	}

	if ( lpObj->Money < 0 )
	{
		LogAddC(2, "[ ZEN MODIFY ] [%s][%s] %d -> 0", szName, gObj[aIndex].AccountID, lpObj->Money);
		lpObj->Money = 0;
	}

	if ( lpObj->m_bMapSvrMoveReq == false )
	{
		if ( MapNumberCheck(lpObj->MapNumber) == FALSE )
		{
			LogAdd("error-L3 : %s %d", __FILE__, __LINE__);
			CloseClient(aIndex);

			return;
		}

		short wGameServerCode = g_MapServerManager.CheckMoveMapSvr(lpObj->m_Index, lpObj->MapNumber, lpObj->m_sPrevMapSvrCode);

		LogAdd("mapserver code: %d", wGameServerCode);
		if ( wGameServerCode != gGameServerCode )
		{
			if ( wGameServerCode == -1 )
			{
				LogAddC(2,"[MapServerMng] Map Server Move Fail : CheckMoveMapSvr() == -1 [%s][%s] (%d)",
					lpObj->AccountID, lpObj->Name, lpObj->m_Index);
				CloseClient(aIndex);

				return;
			}

			GJReqMapSvrMove(lpObj->m_Index, wGameServerCode, lpObj->MapNumber, lpObj->X, lpObj->Y);
			LogAddTD("[MapServerMng] Request to Move Map Server : (%d) - [%s][%s] (%d)",
				wGameServerCode, lpObj->AccountID, lpObj->Name, lpObj->m_Index);

			return;
		}
	}

	lpObj->m_bMapSvrMoveReq = false;


	pjMsg.h.c = 0xC3;
	pjMsg.h.headcode = 0xF3;
	pjMsg.h.size = sizeof(PMSG_CHARMAPJOINRESULT);
	pjMsg.subcode = 0x03;
	pjMsg.MapX = lpObj->X;
	pjMsg.MapY = lpObj->Y;
	pjMsg.MapNumber = lpObj->MapNumber;
	pjMsg.Dir = lpObj->Dir;

	__int64 Exp = lpObj->Experience;

	if( g_MasterExp.IsEnabled(lpObj) )
	{
		Exp = lpObj->MasterExperience;
	}
	
	pjMsg.Exp[0] = DWORD((DWORD(Exp >> 32) >> 16) & 0xFFFF) >> 8;
	pjMsg.Exp[1] = DWORD((DWORD(Exp >> 32) >> 16) & 0xFFFF) & 0xFF;
	pjMsg.Exp[2] = DWORD((DWORD(Exp >> 32) & 0xFFFF) & 0xFFFF) >> 8;
	pjMsg.Exp[3] = DWORD((DWORD(Exp >> 32) & 0xFFFF) & 0xFFFF) & 0xFF;

	pjMsg.Exp[4] = DWORD((DWORD(Exp & 0xFFFFFFFF) >> 16) & 0xFFFF) >> 8;
	pjMsg.Exp[5] = DWORD((DWORD(Exp & 0xFFFFFFFF) >> 16) & 0xFFFF) & 0xFF;
	pjMsg.Exp[6] = DWORD((DWORD(Exp & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) >> 8;
	pjMsg.Exp[7] = DWORD((DWORD(Exp & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) & 0xFF;

	__int64 NextExp = lpObj->NextExp;

	if( g_MasterExp.IsEnabled(lpObj) )
	{
		NextExp = lpObj->MasterNextExp;
	}

	pjMsg.NextExp[0] = DWORD((DWORD(NextExp >> 32) >> 16) & 0xFFFF) >> 8;
	pjMsg.NextExp[1] = DWORD((DWORD(NextExp >> 32) >> 16) & 0xFFFF) & 0xFF;
	pjMsg.NextExp[2] = DWORD((DWORD(NextExp >> 32) & 0xFFFF) & 0xFFFF) >> 8;
	pjMsg.NextExp[3] = DWORD((DWORD(NextExp >> 32) & 0xFFFF) & 0xFFFF) & 0xFF;

	pjMsg.NextExp[4] = DWORD((DWORD(NextExp & 0xFFFFFFFF) >> 16) & 0xFFFF) >> 8;
	pjMsg.NextExp[5] = DWORD((DWORD(NextExp & 0xFFFFFFFF) >> 16) & 0xFFFF) & 0xFF;
	pjMsg.NextExp[6] = DWORD((DWORD(NextExp & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) >> 8;
	pjMsg.NextExp[7] = DWORD((DWORD(NextExp & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) & 0xFF;

	pjMsg.LevelUpPoint = lpObj->LevelUpPoint;
	pjMsg.Str = lpObj->Strength;
	pjMsg.Dex = lpObj->Dexterity;
	pjMsg.Vit = lpObj->Vitality;
	pjMsg.Energy = lpObj->Energy;
	pjMsg.Money = lpObj->Money;
	pjMsg.PkLevel = lpObj->m_PK_Level;
	pjMsg.Life = (WORD)lpObj->Life;
	pjMsg.MaxLife = (WORD)(lpObj->AddLife + lpObj->MaxLife);
	pjMsg.Mana = (WORD)lpObj->Mana;
	pjMsg.MaxMana = (WORD)(lpObj->AddMana + lpObj->MaxMana);
	pjMsg.wShield = lpObj->iShield;
	pjMsg.wMaxShield = lpObj->iMaxShield + lpObj->iAddShield;
	pjMsg.CtlCode = lpMsg->CtlCode;
	pjMsg.BP = lpObj->BP;
	pjMsg.MaxBP = lpObj->MaxBP + lpObj->AddBP;
	pjMsg.Leadership = lpObj->Leadership;

#if (FIX_HP==1)
	pjMsg.HP = (int)lpObj->Life;
	pjMsg.MAXHP = (int)(lpObj->MaxLife+lpObj->AddLife);

	pjMsg.MP = (int)lpObj->Mana;
	pjMsg.MAXMP = (int)(lpObj->MaxMana+lpObj->AddMana);
	
	pjMsg.SD = lpObj->iShield;
	pjMsg.MAXSD = lpObj->iMaxShield+lpObj->iAddShield;
#endif

	short AddPoint = 0;
	short MaxAddPoint = 0;
	short MinusPoint = 0;
	short MaxMinusPoint = 0;
	
	gObjGetStatPointState(lpObj->m_Index, AddPoint, MaxAddPoint, MinusPoint, MaxMinusPoint);
	pjMsg.AddPoint = AddPoint;
	pjMsg.MaxAddPoint = MaxAddPoint;
	pjMsg.wMinusPoint = MinusPoint;
	pjMsg.wMaxMinusPoint = MaxMinusPoint;
	pjMsg.btInventoryExpansion = lpObj->pInventoryExtend;

	LogAddTD("[FRUIT System] [%s][%s] (MinusPoint:%d/PlusPoint:%d) (MaxMinus:%d/MaxPlus:%d)",
		lpObj->AccountID, lpObj->Name, MinusPoint, AddPoint, MaxMinusPoint, MaxAddPoint);

	if ( AddPoint < 0 || AddPoint > MaxAddPoint || MinusPoint < 0 || MinusPoint > MaxMinusPoint )
	{
		LogAddTD("[FRUIT System] Character Stat Error [%s][%s] (MinusPoint:%d/PlusPoint:%d) (MaxMinus:%d/MaxPlus:%d)",
			lpObj->AccountID, lpObj->Name, MinusPoint, AddPoint, MaxMinusPoint, MaxAddPoint);
	}

#if (ENABLETEST_ARCA == 1)
	if (g_ArcaBattle.IsArcaBattleServer()
#if (ENABLETEST_ACHERON == 1)
		&& g_AcheronGuardianEvent.IsPlayStart()
#endif
		)
	{
		g_ArcaBattle.SendArcaBattleCurrentState(*lpObj);
	}

	if (g_ArcaBattle.IsArcaBattleServer()
#if (ENABLETEST_ACHERON == 1)
		&& !g_AcheronGuardianEvent.IsPlayStart()
#endif
		)
	{
		g_ArcaBattle.SendArcaBattlePlayInfo(*lpObj, 1);
		g_ArcaBattle.SendArcaBattleCurrentState(*lpObj);
		g_ArcaBattle.EnterArcaBattleEvent(lpObj->m_Index);
		//LogAddEvent("AcheronGuardianEvent:EnterUser", lpObj);
	}
#endif

	g_QuestInfo.QuestInfoSave(lpObj);
	
	DataSend(aIndex, (UCHAR*)&pjMsg, pjMsg.h.size);

	GCItemListSend(aIndex);
	g_CashShop.GCSendAllItemInfo(lpObj);

#if (ENABLETEST_MUUN == 1)
	g_CMuunSystem.GDReqLoadMuunInvenItem(*lpObj);
#endif

#if (ENABLETEST_RUMMY == 1)
	g_CMuRummyMng.GDReqCardInfo(lpObj);
#endif

	GCMagicListMultiSend(lpObj, 1);
	g_MasterExp.DGReqMasterInfo(lpObj);
	g_ElementalSystem.GDReqErtelList(aIndex);
	CGRequestQuestInfo(aIndex);
	g_MonsterKillInfo.DGReqMonsterKillInfo(lpObj);
	DGGuildMemberInfoRequest(aIndex);
	FriendListRequest(aIndex);

#if (__QUEST_SYSTEM__== 1)
	gQuestInfoEx.GDReqQuestInfo(lpObj);
#else

#endif

#if (__GENS_SYSTEM__ == 1 )
	gGensSystem.GDReqGensInfo(aIndex);
#endif

	gObjUseSkill.SkillChangeUse(aIndex);

	//g_MiniMap.SendMinimap(aIndex);
	g_Minimap.SendNpcCoordinate(aIndex, gObj[aIndex].MapNumber, 0);
    g_Minimap.SendPortalCoordinate(aIndex, gObj[aIndex].MapNumber);

	int iTempUserDataSlotIndex = cpDSPTempUserManager->CheckMatchCharacter(lpObj->Name);

	if( iTempUserDataSlotIndex != -1 )
	{
		cpDSPTempUserManager->SendEventMapInfo(iTempUserDataSlotIndex,aIndex);
		cpDSPTempUserManager->AddToPriviousParty(aIndex, iTempUserDataSlotIndex);
	}

#ifndef SILVER
	bool Login = true;
	// ----
	for( int Node = 1; Node < 4; Node++ )
	{
		if( gPremiumServer[Node] > 0 )
		{
			Login = false;
			// ----
			if( m_ObjBill[aIndex].GetPayCode() == Node )
			{
				Login = true;
				break;
			}
		}
	}
	// ----
	if( !Login )
	{
		GCServerMsgStringSend("For this server you need premium status", lpObj->m_Index, 0);
		gObjCloseSet(aIndex, 2);
		return;
	}
#endif
	// ----
	g_Notice.SendOnConnect(lpObj->m_Index);
//	cCustomManager.GMNotice(lpObj);
	// ----
#if (__CUSTOM_QUEST__ == 1)
	g_MyQuest.ReqUserInfo(aIndex);
#endif

#if (CUSTOM_PETS == 1)
	gCustomPets.RequestSummonInfo(lpObj->m_Index,lpObj->Name);
#endif

}


void GCItemListSend(int aIndex)
{
	PMSG_INVENTORYLISTCOUNT pMsgILC;
	PMSG_INVENTORYLIST pMsgIL;
	int sOfs = sizeof(PMSG_INVENTORYLISTCOUNT);
	int pMsgILSize = sizeof(PMSG_INVENTORYLIST);
	BYTE sendBuf[4096];
	LPOBJ lpObj = &gObj[aIndex];
	int itemcount = 0;
	
	for ( int n=0;n<INVENTORY_SIZE;n++)
	{
		if ( lpObj->pInventory[n].IsItem() == TRUE )
		{
			if ( lpObj->pInventory[n].m_bItemExist == false )
			{
				continue;
			}

			pMsgIL.Pos = n;
			ItemByteConvert(pMsgIL.ItemInfo,lpObj->pInventory[n]);
			memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
			itemcount++;
			sOfs += pMsgILSize;
		}
	}

	pMsgILC.h.c = 0xC4;
	pMsgILC.h.headcode = 0xF3;
	pMsgILC.subcode = 0x10;
	pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
	pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
	pMsgILC.Count = itemcount;
	memcpy(sendBuf, &pMsgILC, sizeof(PMSG_INVENTORYLISTCOUNT));

	DataSend(aIndex, sendBuf, sOfs);
}


struct SDHP_DBCHAR_INFOSAVE
{
	PWMSG_HEAD h;	// C2:07
	char Name[11];	// 4
	short Level;	// 10
	BYTE Class;	// 12
	int LevelUpPoint;	// 14
	DWORD Exp;	// 18
	DWORD NextExp;	// 1C
	int Money;	// 20
#if( FIX_MAXSTAT == 1 )
	WORD Str;	// 24
	WORD Dex;	// 26
	WORD Vit;	// 28
	WORD Energy;	// 2A
#else
	short Str;	// 24
	short Dex;	// 26
	short Vit;	// 28
	short Energy;	// 2A
#endif
	WORD Life;	// 2C
	WORD MaxLife;	// 2E
	WORD Mana;	// 30
	WORD MaxMana;	// 32
	BYTE dbInventory[3792];	// 34
	BYTE dbMagicList[MAGIC_SIZE*3];	// 6F4
	BYTE MapNumber;	// 7A8
	BYTE MapX;	// 7A9
	BYTE MapY;	// 7AA
	BYTE Dir;	// 7AB
	int PkCount;	// 7AC
	int PkLevel;	// 7B0
	int PkTime;	// 7B4
	BYTE dbQuest[50];	// 7B8
	BYTE CharInfoSave;	// 7EA
	WORD Leadership;	// 7EC
	WORD ChatLitmitTime;	// 7EE
	int iFruitPoint;	// 7F0
#if (__CUSTOM__ == 1)
	int iReset;
#endif
#if( ENABLE_CUSTOM_HARDCORE == 1 )
	short	HardcoreLife;
#endif
//#ifdef STREAM
	DWORD m_Credits;
//#endif
};


#if (__CUSTOM__ == 1)
void GJSetCharacterInfo(LPOBJ lpObj, int aIndex, BOOL bMapServerMove,BOOL bSaveItem)
#else
void GJSetCharacterInfo(LPOBJ lpObj, int aIndex, BOOL bMapServerMove)
#endif
{
	if ( lpObj->m_bMapSvrMoveQuit == true )
	{
		LogAddC(2,"[MapServerMng] GJSetCharacterInfo() - Inventory Already Saved [%s][%s] (%d)",
			lpObj->AccountID, lpObj->Name, lpObj->m_Index);

		return;
	}

	if ( bMapServerMove == TRUE )
	{
		lpObj->m_bMapSvrMoveQuit = TRUE;

		LogAddC(5, "[MapServerMng][%s][%s] GJSetCharacterInfo() - MapServerMove Character Save Start (%d)",
			lpObj->AccountID, lpObj->Name, lpObj->m_Index);
	}


	if ( lpObj->m_IfState.use > 0 && lpObj->m_IfState.type == 6 && lpObj->m_IfState.state == 1)
	{
		GDSetWarehouseList(aIndex);
	}

	preSaveCheck(lpObj);

	SDHP_DBCHAR_INFOSAVE pCSave;	// Packet Character Save
 
	pCSave.h.c = 0xC2;
	pCSave.h.headcode = 0x07;
	pCSave.h.sizeH = sizeof(SDHP_DBCHAR_INFOSAVE) >> 8;
	pCSave.h.sizeL = sizeof(SDHP_DBCHAR_INFOSAVE) & 0xFF;
	pCSave.Name[MAX_ACCOUNT_LEN] = 0;
	memcpy(pCSave.Name, lpObj->Name, sizeof(pCSave.Name)-1);
	pCSave.Class = lpObj->DbClass;
	pCSave.Level = lpObj->Level;
	pCSave.LevelUpPoint = lpObj->LevelUpPoint;
	pCSave.Exp = lpObj->Experience;
	pCSave.NextExp = lpObj->NextExp;
	pCSave.Money = lpObj->Money;
	pCSave.Str = lpObj->Strength;
	pCSave.Dex = lpObj->Dexterity;
	pCSave.Vit = lpObj->Vitality;
	pCSave.Energy = lpObj->Energy;
	pCSave.Life = (WORD)(lpObj->Life*10.0f);
	pCSave.MaxLife = (WORD)(lpObj->MaxLife*10.0f);
	pCSave.Mana = (WORD)(lpObj->Mana*10.0f);
	pCSave.MaxMana = (WORD)(lpObj->MaxMana*10.0f);
	ItemByteConvert16((LPBYTE)pCSave.dbInventory , lpObj->pInventory, INVENTORY_SIZE);
	MagicByteConvert(pCSave.dbMagicList, lpObj->Magic, MAGIC_SIZE);
	pCSave.MapNumber = lpObj->MapNumber;
	pCSave.MapX = lpObj->X;
	pCSave.MapY = lpObj->Y;
	pCSave.Dir = lpObj->Dir;
	pCSave.PkCount = lpObj->m_PK_Count;
	pCSave.PkLevel = lpObj->m_PK_Level;
	pCSave.PkTime = lpObj->m_PK_Time;
	pCSave.Leadership = lpObj->Leadership;
	pCSave.ChatLitmitTime = lpObj->ChatLimitTime;
	pCSave.iFruitPoint = lpObj->iFruitPoint;
//#ifdef STREAM
	pCSave.m_Credits = lpObj->m_Credits;
//#endif

#if( ENABLE_CUSTOM_HARDCORE == 1 )
	pCSave.HardcoreLife = lpObj->m_HardcoreLife;
#endif

	pCSave.CharInfoSave = 0;
#if (__CUSTOM__ == 1)
	pCSave.iReset = lpObj->iResetCount;
#if( __4GAMERS__ == 1 )
	g_ResetDebug.Output("[%s] [%s] Save character, reset: %d",
		lpObj->AccountID, lpObj->Name, lpObj->iResetCount);
#endif
#endif
	memcpy(pCSave.dbQuest, lpObj->m_Quest, sizeof(pCSave.dbQuest));

	if ( cDBSMng.Send((char*)&pCSave, sizeof(SDHP_DBCHAR_INFOSAVE)) == FALSE )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 174)), lpObj->Name);
		return;
	}

#if (__CUSTOM__==1)
	if( bSaveItem == TRUE )	{
		gObjItemTextSave(lpObj);
	}
#else
	gObjItemTextSave(lpObj);
#endif
	gObjStatTextSave(lpObj);
	gObjSavePetItemInfo(aIndex, 0);
	g_MonsterKillInfo.SaveMonsterKillInfo(lpObj);
	g_MasterExp.SaveMasterInfo(lpObj);
	g_ElementalSystem.GDSaveErtelList(aIndex);
#if (ENABLETEST_MUUN == 1)
	g_CMuunSystem.GDReqSaveMuunInvenItem(*lpObj);
#endif
}




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Get Warehouse Info Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x08
 */
struct SDHP_GETWAREHOUSEDB
{
	PBMSG_HEAD h;	// C1:08
	char AccountID[10];	// 3
	short aIndex;	// E
};


void GDGetWarehouseList(int aIndex, char* AccountID)
{
	if ( strlen(AccountID) < 1 )
	{
		return;
	}

	SDHP_GETWAREHOUSEDB	pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x08;
	pMsg.h.size = sizeof(SDHP_GETWAREHOUSEDB);
	pMsg.aIndex = aIndex;
	memcpy(pMsg.AccountID, AccountID, sizeof(pMsg.AccountID));

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}






void DGGetWarehouseList(SDHP_GETWAREHOUSEDB_SAVE * lpMsg)
{
	char szId[11];
	int aIndex = 0;
	PMSG_TALKRESULT pResult;

	szId[MAX_ACCOUNT_LEN]=0;
	memcpy(szId, lpMsg->AccountID, sizeof(lpMsg->AccountID));
	aIndex = lpMsg->aIndex;

	if ( !gObjIsAccontConnect(aIndex, szId))
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 175)), szId, aIndex);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	int itype;
	int _type;
	BYTE OptionData;
	CItem item;

	memset(gObj[aIndex].pWarehouseMap, (BYTE)-1, WAREHOUSE_SIZE);
	gObj[aIndex].WarehouseMoney = lpMsg->Money;
	gObj[aIndex].WarehousePW = lpMsg->pw;

	int dbsize = 7;

	if( lpMsg->DbVersion >= 3 )
		dbsize = 16;
	else if( lpMsg->DbVersion >= 1)
		dbsize = 10;

	for ( int n=0;n<WAREHOUSE_SIZE;n++)
	{
		itype = lpMsg->dbItems[n*dbsize];

		if ( dbsize == 16 )
		{
			itype |= (lpMsg->dbItems[n*dbsize+DBI_OPTION380_DATA] & 0xF0) << 5;
			itype |= (lpMsg->dbItems[n*dbsize+DBI_NOPTION_DATA] & 0x80) << 1;
			_type = itype;
		}
		else if ( dbsize == 10 )
		{
			if ( ((lpMsg->dbItems[n*dbsize+DBI_NOPTION_DATA] >> 7) & 0x01) )
			{
				itype += 256;
			}

			_type = (itype / 32 * MAX_SUBTYPE_ITEMS) + itype % 32;
		}
		else
		{
			_type = (itype / MAX_TYPE_ITEMS * MAX_SUBTYPE_ITEMS) + itype % 16;
		}

		gObj[aIndex].pWarehouse[n].Clear();

		if ( dbsize >= 16 )
		{
			if ( lpMsg->dbItems[n*dbsize] == (BYTE)-1 && (lpMsg->dbItems[n*dbsize+DBI_OPTION380_DATA] & 0xF0) == 0xF0 && (lpMsg->dbItems[n*dbsize+7] & 0x80) == 0x80)
			{
				itype = -1;
			}
		}
		else if ( dbsize >= 10 )
		{
			if ( lpMsg->dbItems[n*dbsize] == (BYTE)-1 && (lpMsg->dbItems[n*dbsize+DBI_NOPTION_DATA] & 0x80) == 0x80)
			{
				itype = -1;
			}
		}
		else
		{
			if ( lpMsg->dbItems[n*dbsize] == (BYTE)-1 )
			{
				itype = -1;
			}
		}

		if ( IsItem(_type) == FALSE )
		{
			itype = -1;
		}

		if ( _type == ITEMGET(14,17) || _type == ITEMGET(14,18) )
		{
			if ( lpMsg->dbItems[n*dbsize+DBI_DUR]== 0 )	// Level 0
			{
				_type = -1;	// #error maybe this is itype
			}
		}

		if ( _type == ITEMGET(14,19) )
		{
			if ( lpMsg->dbItems[n*dbsize+DBI_DUR] == (BYTE)-1 )
			{
				itype = -1;
			}
		}

		if ( itype != -1 )
		{
			item.m_Level = DBI_GET_LEVEL(lpMsg->dbItems[n*dbsize+DBI_OPTION_DATA]);

			if ( lpMsg->DbVersion	< 2 )
			{
				lpMsg->dbItems[n*dbsize+DBI_SOPTION_DATA] = -1;
				lpMsg->dbItems[n*dbsize+DBI_OPTION380_DATA] = -1;
			}

			if ( _type != ITEMGET(14,11) )	// Box Of Luck
			{
				if ( item.m_Level > MAX_ITEM_LEVEL )
				{
					item.m_Level = 0;
				}
			}

			OptionData = lpMsg->dbItems[n*dbsize+DBI_OPTION_DATA];
			item.m_Option1 = DBI_GET_SKILL(OptionData);
			item.m_Option2 = DBI_GET_LUCK(OptionData);
			item.m_Option3 = DBI_GET_OPTION(OptionData);

			item.m_Option3 |= DBI_GET_OPTION16(lpMsg->dbItems[n*dbsize+DBI_NOPTION_DATA]);
			item.m_Durability = lpMsg->dbItems[n*dbsize+DBI_DUR];
			item.m_JewelOfHarmonyOption = lpMsg->dbItems[n*dbsize+DBI_JOH_DATA];

			if ( dbsize >= 10 )
			{
				item.m_ItemOptionEx = DBI_GET_380OPTION(lpMsg->dbItems[n*dbsize+DBI_OPTION380_DATA]);
			}

			if ( dbsize >= 16 )
			{
				if ( item.m_ItemOptionEx != 0 )
				{
					item.m_Type = itype;
					if ( g_kItemSystemFor380.Is380Item(&item)== FALSE )
					{
						item.m_ItemOptionEx = 0;
							LogAddTD("[380Item][%s][%s] Invalid 380 Item Option in Warehouse pos[%d]",
							lpObj->AccountID, lpObj->Name, n);
					}
				}
			}

			BYTE SocketOptions[MAX_SOCKET_COUNT];
			BYTE SocketBonus = 0xFF;
			int i = 0;

			memset(SocketOptions,0xFF,MAX_SOCKET_COUNT);

			if( g_SocketItem.IsSocketItem(itype) == TRUE ||
				g_ElementalSystem.IsElementalItem(itype) == TRUE )
			{
				SocketBonus = lpMsg->dbItems[n*dbsize+10];

				for( i = 0; i < MAX_SOCKET_COUNT; i++)
				{
					SocketOptions[i] = lpMsg->dbItems[n*dbsize+11+i];
				}
			}
			else
			{
				SocketBonus = 0;

				for( i = 0; i < MAX_SOCKET_COUNT; i++)
				{
					SocketOptions[i] = 0xFF;
				}
			}

			item.m_bLOCKED = ( (lpMsg->dbItems[n*dbsize+DBI_SOPTION_DATA]&0xC0) == 0xC0)  ? TRUE : FALSE;

			if ( lpMsg->DbVersion == 0 )
			{
				item.Convert(itype, item.m_Option1, item.m_Option2, item.m_Option3, 0, 0, 0,0,0xFF,3);
			}
			else if ( lpMsg->DbVersion <= 2 )
			{
				item.Convert(itype, item.m_Option1, item.m_Option2, item.m_Option3, DBI_GET_NOPTION(lpMsg->dbItems[n*dbsize+DBI_NOPTION_DATA]), lpMsg->dbItems[n*dbsize+DBI_SOPTION_DATA], item.m_ItemOptionEx, 0,0xFF,1);
			}
			else
			{
				item.Convert(itype, item.m_Option1, item.m_Option2, item.m_Option3, DBI_GET_NOPTION(lpMsg->dbItems[n*dbsize+DBI_NOPTION_DATA]), lpMsg->dbItems[n*dbsize+DBI_SOPTION_DATA], item.m_ItemOptionEx, &SocketOptions[0],SocketBonus,3);
			}

			if ( _type == ITEMGET(14,7) ) // Siege Potion
			{

			}
			else if ( (_type >= ITEMGET(14,0) && _type <= ITEMGET(14,8)) || ( _type >= ITEMGET(14,35) && _type <= ITEMGET(14,40) ) )
			{
				if ( item.m_Durability == 0.0f )
				{
					item.m_Durability = 1.0f;
				}

				if ( item.m_Durability > 255.0f )
				{
					item.m_Durability = 255.0f;
				}
			}
			else if ( _type != ITEMGET(13,10) && _type != ITEMGET(14,29) && _type != ITEMGET(14,21) && item.m_Level == 3 && _type != ITEMGET(14,100) )
			{
				if ( item.m_Durability > item.m_BaseDurability )
				{
					item.m_Durability = item.m_BaseDurability;
				}
			}

			lpObj->pWarehouse[n].m_Option1 = item.m_Option1;
			lpObj->pWarehouse[n].m_Option2 = item.m_Option2;
			lpObj->pWarehouse[n].m_Option3 = item.m_Option3;
			lpObj->pWarehouse[n].m_JewelOfHarmonyOption = item.m_JewelOfHarmonyOption;
			lpObj->pWarehouse[n].m_ItemOptionEx = item.m_ItemOptionEx;

			WORD hiword = MAKE_NUMBERW(lpMsg->dbItems[n*dbsize+DBI_SERIAL1], lpMsg->dbItems[n*dbsize+DBI_SERIAL2]);
			WORD loword = MAKE_NUMBERW(lpMsg->dbItems[n*dbsize+DBI_SERIAL3], lpMsg->dbItems[n*dbsize+DBI_SERIAL4]);
			item.m_Number = MAKE_NUMBERDW(hiword, loword);
			
			gObjWarehouseInsertItemPos(lpObj->m_Index, item, n, -1);
			gObjWarehouseItemSet(lpObj->m_Index, n, 1);
		}
	}

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(PMSG_TALKRESULT);
	pResult.result = 2;

	DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
	GCUserWarehouseSend(&gObj[aIndex]);

	if ( bCanWarehouseLock == TRUE )
	{
		if ( gObj[aIndex].WarehousePW > 0 )
		{
			if ( gObj[aIndex].WarehouseLock == 0 )
			{
				GCWarehouseStateSend(aIndex, 12);
			}
			else
			{
				GCWarehouseStateSend(aIndex, 1);
			}
		}
		else
		{
			GCWarehouseStateSend(aIndex, 0);
		}
	}

	lpObj->LoadWareHouseInfo = true;
	lpObj->m_ReqWarehouseOpen = false;
	gObjRequestPetItemInfo(lpObj->m_Index, 1);

}

void GDGetWarehouseNoItem( SDHP_GETWAREHOUSEDB_RESULT * lpMsg)
{
	char szId[11];
	int aIndex = lpMsg->aIndex;
	PMSG_TALKRESULT pResult;

	szId[MAX_ACCOUNT_LEN] = 0;
	memcpy(szId, lpMsg->AccountID, sizeof(lpMsg->AccountID));

	if ( !gObjIsAccontConnect(aIndex, szId) )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 175)), szId, aIndex);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	
	if ( lpObj->m_IfState.type == 6 )
	{
		if ( lpObj->m_IfState.state == 0 )
		{
			lpObj->m_IfState.state = 1;
			lpObj->WarehouseSave = 1;
			pResult.h.c = 0xC3;
			pResult.h.headcode = 0x30;
			pResult.h.size = sizeof(pResult);
			pResult.result = 2;

			DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
			LogAddC(2, lMsg.Get(MSGGET(1, 176)), lpObj->Name);
		}
	}

	lpObj->LoadWareHouseInfo = true;
	lpObj->m_ReqWarehouseOpen = false;
}

void GDSetWarehouseList(int aIndex)
{
	SDHP_GETWAREHOUSEDB_SAVE pMsg;
	
	if ( gObj[aIndex].LoadWareHouseInfo == false )
	{
		LogAddTD("[%s][%s] WareHouse Save Fail : Not Open",
			gObj[aIndex].AccountID, gObj[aIndex].Name);

		return;
	}

	memcpy(pMsg.AccountID, gObj[aIndex].AccountID, sizeof(pMsg.AccountID));
	pMsg.aIndex = aIndex;
	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0x09;
	pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
	pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
	pMsg.Money = gObj[aIndex].WarehouseMoney;
	pMsg.pw = gObj[aIndex].WarehousePW;
	ItemByteConvert16(pMsg.dbItems, gObj[aIndex].pWarehouse, WAREHOUSE_SIZE);

	cDBSMng.Send((PCHAR)&pMsg, sizeof(pMsg));
	gObjSavePetItemInfo(aIndex, 1);
	gObjWarehouseTextSave(&gObj[aIndex]);
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu DB Char Item Save Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC2
 *	HeadCode  : 0x11
 */
struct SDHP_DBCHAR_ITEMSAVE
{
	PWMSG_HEAD h;	// C2:11
	char Name[10];	// 4
	BYTE dbInventory[3792];	// E
};


void GDUserItemSave(LPOBJ lpObj)
{
	SDHP_DBCHAR_ITEMSAVE pMsg;
	CLogToFile logger = CLogToFile(LOG_NAME_DEBUG, LOG_PATH_DEBUG, true);
	logger.Output("USER ITEM SAVE");

	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0x11;
	pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
	pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
	memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
	ItemByteConvert16(pMsg.dbInventory, lpObj->pInventory, INVENTORY_SIZE);

	cDBSMng.Send((PCHAR)&pMsg, sizeof(pMsg));
}




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Warehouse Money Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x12
 */
struct SDHP_WAREHOUSEMONEY_SAVE
{
	PBMSG_HEAD h;	// C1:12
	char AccountID[10];	// 3
	short aIndex;	// E
	int Money;	// 10
};


void GDSetWarehouseMoney(int aIndex)
{
	SDHP_WAREHOUSEMONEY_SAVE pMsg;

	memcpy(pMsg.AccountID, gObj[aIndex].AccountID, sizeof(pMsg.AccountID));
	pMsg.aIndex = aIndex;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x12;
	pMsg.h.size = sizeof(pMsg);
	pMsg.Money = gObj[aIndex].WarehouseMoney;

	cDBSMng.Send((PCHAR)&pMsg, sizeof(pMsg));
}

void GDGameServerInfoSave()
{
	return;		// GHere goes a MACRO
}




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Item Serial Create Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x52, 0x55 (in Case of PET )
 */
struct SDHP_ITEMCREATE
{
	PBMSG_HEAD h;	// C1:XX
	BYTE x;	// 3
	BYTE y;	// 4
	BYTE MapNumber;	// 5 [235:Mu_2nd_Aniv], [236:CashShop]
	short Type;	// 6
	BYTE Level;	// 8
	BYTE Dur;	// 9
	BYTE Op1;	// A
	BYTE Op2;	// B
	BYTE Op3;	// C
	BYTE NewOption;	// D
	int aIndex;	// 10
	short lootindex;	// 14
	BYTE SetOption;	// 16
	BYTE SocketBonus;
	BYTE SocketOptions[5];
};





// MapNumber [235:Mu_2nd_Aniv], [236:CashShop]
void ItemSerialCreateSend(int aIndex, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption,BYTE SocketBonus,LPBYTE SocketOptions)
{
	SDHP_ITEMCREATE  pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x52;
	pMsg.h.size = sizeof(pMsg);
	pMsg.MapNumber = MapNumber;
	pMsg.x = x;
	pMsg.y = y;
	pMsg.Type = type;
	pMsg.Level = level;

	if (dur == 0 )
	{
		dur = ItemGetDurability(type, level, NewOption, SetOption);
	}

	pMsg.Dur = dur;
	pMsg.Op1 = Op1;
	pMsg.Op2 = Op2;
	pMsg.Op3 = Op3;
	pMsg.NewOption = NewOption;
	pMsg.aIndex = aIndex;
	pMsg.lootindex = LootIndex;
	pMsg.SetOption = SetOption;

	if( SocketOptions != NULL )
	{
		pMsg.SocketBonus = SocketBonus;

		for(int i = 0; i < 5; i++)
		{
			pMsg.SocketOptions[i] = SocketOptions[i];
		}
	}
	else if( g_SocketItem.IsSocketItem(type) )
	{
		int SocketCount = g_SocketItem.GetSocketCount(type);
		pMsg.SocketBonus = (BYTE)-1;

		LogAddTD("[SocketItem] Generate Socket Item ( Socket Slot : %d ) - %s, [%d,%d,%d,%d], Serial : %u, ItemCode : %d, MapNumber : %d(%d/%d)",
			SocketCount,ItemAttribute[type].Name,level,Op1,Op2,Op3,type,MapNumber,x,y);

		for(int i = 0;i < MAX_SOCKET_COUNT;i++)
		{
			if( i < SocketCount )
			{
				pMsg.SocketOptions[i] = (BYTE)-2;
			}
			else
			{
				pMsg.SocketOptions[i] = (BYTE)-1;
			}
		}
	}
	else
	{
		pMsg.SocketBonus = (BYTE)-1;
		for(int i = 0; i < 5; i++)	{
			pMsg.SocketOptions[i] = (BYTE)-1;
		}
	}

	if ( OBJMAX_RANGE(aIndex) != FALSE  && gObj[aIndex].Type != OBJ_USER )
	{
		BYTE NewOption[MAX_EXOPTION_SIZE];

		for ( int i=0;i<MAX_EXOPTION_SIZE;i++)
		{
			NewOption[i] = 0;
		}

		if ( (pMsg.NewOption & 0x20) != 0 )
		{
			NewOption[0] = TRUE;
		}
		if ( (pMsg.NewOption & 0x10) != 0 )
		{
			NewOption[1] = TRUE;
		}

		if ( (pMsg.NewOption & 0x8) != 0 )
		{
			NewOption[2] = TRUE;
		}
		if ( (pMsg.NewOption & 0x4) != 0 )
		{
			NewOption[3] = TRUE;
		}

		if ( (pMsg.NewOption & 0x2) != 0 )
		{
			NewOption[4] = TRUE;
		}
		if ( (pMsg.NewOption & 0x1) != 0 )
		{
			NewOption[5] = TRUE;
		}

#if( DEBUG_DROP == 1)
		DROP_LOG_DEBUG.Output("Monster Item Drop Request [%s] [%d][%d][%d] : [%s][%d][%d][%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d] | LootUser: [%s]",
			gObj[aIndex].Name, MapNumber, pMsg.x, pMsg.y, ItemAttribute[pMsg.Type].Name,
			pMsg.Level, pMsg.Op1,pMsg.Op2, pMsg.Op3, NewOption[0], NewOption[1], NewOption[2],
			NewOption[3], NewOption[4],	NewOption[5], NewOption[6], pMsg.SetOption, (OBJMAX_RANGE(LootIndex)) ? gObj[LootIndex].Name : "none");
#else
		LogAddTD("Monster Item Drop Request [%s] [%d][%d][%d] : [%s][%d][%d][%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d]",
			gObj[aIndex].Name, MapNumber, pMsg.x, pMsg.y, ItemAttribute[pMsg.Type].Name,
			pMsg.Level, pMsg.Op1,pMsg.Op2, pMsg.Op3, NewOption[0], NewOption[1], NewOption[2],
			NewOption[3], NewOption[4],	NewOption[5], NewOption[6], pMsg.SetOption);
#endif
	}

	cDBSMng.Send( (char*)&pMsg, pMsg.h.size);
}





void PetItemSerialCreateSend(int aIndex, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption)
{
	SDHP_ITEMCREATE  pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x55;
	pMsg.h.size = sizeof(pMsg);
	pMsg.MapNumber = MapNumber;
	pMsg.x = x;
	pMsg.y = y;
	pMsg.Type = type;
	pMsg.Level = level;

	if (dur == 0 )
	{
		dur = ItemGetDurability(type, level, NewOption, SetOption);
	}

	pMsg.Dur = dur;
	pMsg.Op1 = Op1;
	pMsg.Op2 = Op2;
	pMsg.Op3 = Op3;
	pMsg.NewOption = NewOption;
	pMsg.aIndex = aIndex;
	pMsg.lootindex = LootIndex;
	pMsg.SetOption = SetOption;

	pMsg.SocketBonus = (BYTE)-1;

	for(int i = 0; i < 5; i++)	{
		pMsg.SocketOptions[i] = (BYTE)-1;
	}

	cDBSMng.Send( (char*)&pMsg, pMsg.h.size);
}





void ItemSerialCreateRecv(SDHP_ITEMCREATERECV * lpMsg)
{

#if( DEBUG_DROP == 1)
	DROP_LOG_DEBUG.Output("PACKET_RECIEVED [%d][%d][%d] : serial:%d [%s][%d][%d][%d][%d] Ex:[%d] Set:[%d]",
		lpMsg->MapNumber, lpMsg->x, lpMsg->y, lpMsg->m_Number, ItemAttribute[lpMsg->Type].Name, lpMsg->Level,
		lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption,lpMsg->SetOption);
#endif

	LPOBJ lpObj;
	int mapnumber = lpMsg->MapNumber;
	int aIndex = lpMsg->aIndex;
	int lootindex = lpMsg->lootindex;
	BYTE SocketBonus = lpMsg->SocketBonus;
	BYTE SocketOption[5];

	if ( mapnumber != (BYTE)-1)
	{
		if ( mapnumber < 0 || (mapnumber > MAX_NUMBER_MAP-1 && mapnumber < 235) )
		{
			return;
		}
	}

	SocketBonus = lpMsg->SocketBonus;

	for(int i = 0; i < MAX_SOCKET_COUNT; i++)
	{
		SocketOption[i] = lpMsg->SocketOptions[i];
	}



	if ( lpMsg->MapNumber == (BYTE)-1 || lpMsg->MapNumber == (BYTE)-2 )
	{
		if ( gObjIsConnectedGP(aIndex) == FALSE )
		{
			LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
			return;
		}
		

		lpObj = &gObj[aIndex];

		if ( lpMsg->MapNumber == (BYTE)-2 )
		{
			if ( lpObj->m_IfState.type != 13 )
			{
				LogAdd("error-L2: DarkTrainerBox not used.");
				return;
			}
		}
		else if ( lpObj->m_IfState.type != 7 )
		{
			LogAdd("error-L2: ChaosBox not used.");
			return;
		}
		
		PMSG_CHAOSMIXRESULT pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x86;
		pMsg.h.size = sizeof(PMSG_CHAOSMIXRESULT);

		if ( lpMsg->MapNumber == (BYTE)-2 )
		{
			pMsg.Result = 100;
		}
		else
		{
			pMsg.Result = 1;
		}

		CItem NewItem;

		NewItem.m_Level = lpMsg->Level;
		NewItem.m_Durability = (float)ItemGetDurability(lpMsg->Type, lpMsg->Level, lpMsg->NewOption, lpMsg->SetOption);

		if ( lpMsg->Type == ITEMGET(14,7) ) // Siege Potion
		{
			NewItem.m_Durability = lpMsg->Dur;
		}

		if ( lpMsg->Type == ITEMGET(13,37) )
		{
			NewItem.m_Durability = lpMsg->Dur;
		}

		NewItem.Convert(lpMsg->Type,lpMsg->Op1, lpMsg->Op2, lpMsg->Op3,lpMsg->NewOption, lpMsg->SetOption,NewItem.m_ItemOptionEx,SocketOption,SocketBonus, CURRENT_DB_VERSION);
		
		ItemByteConvert(pMsg.ItemInfo, NewItem);
		NewItem.m_Number = lpMsg->m_Number;
		g_ChaosBox.ChaosBoxInit(lpObj);
		gObjChaosBoxInsertItemPos(aIndex, NewItem, 0, -1);
		gObjChaosItemSet(aIndex, 0, 1);
		BYTE ExOption[MAX_EXOPTION_SIZE];
		ItemIsBufExOption(ExOption, &NewItem);

#if( DEBUG_DROP == 1)
		DROP_LOG_DEBUG.Output("[%s][%s] CBMix Item Create Item:%s [%d][%d] [%d][%d][%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d]",
			lpObj->AccountID, lpObj->Name, NewItem.GetName(), lpMsg->m_Number, lpMsg->Dur, NewItem.m_Level, 
			NewItem.m_Option1, NewItem.m_Option2, NewItem.m_Option3, ExOption[0], ExOption[1],
			ExOption[2], ExOption[3], ExOption[4], ExOption[5], ExOption[6], lpMsg->SetOption);
#else
		LogAddTD("[%s][%s] CBMix Item Create Item:%s [%d][%d] [%d][%d][%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d]",
			lpObj->AccountID, lpObj->Name, NewItem.GetName(), lpMsg->m_Number, lpMsg->Dur, NewItem.m_Level, 
			NewItem.m_Option1, NewItem.m_Option2, NewItem.m_Option3, ExOption[0], ExOption[1],
			ExOption[2], ExOption[3], ExOption[4], ExOption[5], ExOption[6], lpMsg->SetOption);
#endif

		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		gObj[aIndex].ChaosLock = FALSE;

		return;
	}

	if ( lpMsg->MapNumber == 235 )
	{
		if ( gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED )
		{
			int iType  = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);

			CItem NewItem;
			NewItem.m_Level = lpMsg->Level;
			NewItem.m_Durability = lpMsg->Dur;
			NewItem.m_Number = lpMsg->m_Number; //Serial fix
			NewItem.Convert(lpMsg->Type,lpMsg->Op1, lpMsg->Op2, lpMsg->Op3,lpMsg->NewOption, lpMsg->SetOption,0,SocketOption,SocketBonus, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(lpMsg->aIndex, NewItem);

			if( iItemPos == (BYTE)-1 )
			{
#if( DEBUG_DROP == 1)
				DROP_LOG_DEBUG.Output("[ItemCreate] Error : Failed To Insert Item Type:%d, TypeIndex:%d to [%s][%s]",
					iType, iTypeIndex, gObj[lpMsg->aIndex].AccountID, gObj[lpMsg->aIndex].Name);
#else
				LogAddTD("[ItemCreate] Error : Failed To Insert Item Type:%d, TypeIndex:%d to [%s][%s]",
					iType, iTypeIndex, gObj[lpMsg->aIndex].AccountID, gObj[lpMsg->aIndex].Name);
#endif
			}
			else
			{
				::GCInventoryItemOneSend(lpMsg->aIndex, iItemPos);
#if (ENABLE_CUSTOM_OFFLINETRADE == 1)

				if( gObj[lpMsg->aIndex].bOffTrade && PShop_CheckInventoryEmpty(lpMsg->aIndex) )
				{
					gObj[lpMsg->aIndex].OffTradeWaitItem--;
					if( gObj[lpMsg->aIndex].OffTradeWaitItem <= 0 )
					{
						gObjDel(lpMsg->aIndex);
					}
				}
#endif
			}

		}
	}
	else if ( lpMsg->MapNumber == 236 )
	{
		if ( gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED )
		{
			CItem pCreateItem;
			int iItemType = ITEM_GET_TYPE(lpMsg->Type);
			int iItemIndex	= ITEM_GET_INDEX(lpMsg->Type);

			pCreateItem.m_Level = lpMsg->Level;
			pCreateItem.m_Durability = lpMsg->Dur;

			pCreateItem.Convert(ITEMGET(iItemType, iItemIndex), lpMsg->Op1, lpMsg->Op2,
				lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0,0,-1, 3);

			BYTE btItemPos = gObjInventoryInsertItem(lpMsg->aIndex, pCreateItem);

			if ( btItemPos == (BYTE)-1 )
			{
#if( DEBUG_DROP == 1)
				DROP_LOG_DEBUG.Output("[CashShop] Error : Failed To Insert Item Type:%d, TypeIndex:%d to [%s][%s]",
					iItemType, iItemIndex, gObj[lpMsg->aIndex].AccountID, gObj[lpMsg->aIndex].Name);
#else
				LogAddTD("[CashShop] Error : Failed To Insert Item Type:%d, TypeIndex:%d to [%s][%s]",
					iItemType, iItemIndex, gObj[lpMsg->aIndex].AccountID, gObj[lpMsg->aIndex].Name);
#endif
			}
			else
			{
				GCInventoryItemOneSend(lpMsg->aIndex, btItemPos);
			}
		}
	}
#if( __QUEST_SYSTEM__ == 1)
	else if ( lpMsg->MapNumber == 237 )
	{
		if( gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED )
		{
			CItem pCreateItem;
			int iItemType = ITEM_GET_TYPE(lpMsg->Type);
			int iItemIndex	= ITEM_GET_INDEX(lpMsg->Type);

			pCreateItem.m_Level = lpMsg->Level;
			pCreateItem.m_Durability = lpMsg->Dur;

			pCreateItem.Convert(ITEMGET(iItemType, iItemIndex), lpMsg->Op1, lpMsg->Op2,
				lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0,0,-1, 3);

			BYTE btItemPos = gObjInventoryInsertItem(lpMsg->aIndex, pCreateItem);

			if ( btItemPos == (BYTE)-1 )
			{
#if( DEBUG_DROP == 1)
				DROP_LOG_DEBUG.Output("[QuestSystem] Error : Failed To Insert Item Type:%d, TypeIndex:%d to [%s][%s]",
					iItemType, iItemIndex, gObj[lpMsg->aIndex].AccountID, gObj[lpMsg->aIndex].Name);
#else
				LogAddTD("[QuestSystem] Error : Failed To Insert Item Type:%d, TypeIndex:%d to [%s][%s]",
					iItemType, iItemIndex, gObj[lpMsg->aIndex].AccountID, gObj[lpMsg->aIndex].Name);
#endif
			}
			else
			{
				GCInventoryItemOneSend(lpMsg->aIndex, btItemPos);
			}
		}
	}
#endif
	else
	{
		int iRetMapNumber = lpMsg->MapNumber;

		if ( lpMsg->MapNumber >= 238 && lpMsg->MapNumber <= 245 )
		{
			mapnumber = g_BloodCastle.GetItemMap(iRetMapNumber);
		}

		if ( lpMsg->MapNumber >= 246 && lpMsg->MapNumber <= 253 )
		{
			mapnumber = g_BloodCastle.GetRewardMap(iRetMapNumber);
		}

		int iItemCount = MapC[mapnumber].MonsterItemDrop(lpMsg->Type, lpMsg->Level, lpMsg->Dur, lpMsg->x,
			lpMsg->y, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption,
			lootindex, lpMsg->m_Number, 0,SocketOption,SocketBonus);

		if ( iItemCount != -1 )
		{
			if ( iRetMapNumber >= 238 && iRetMapNumber <= 245)
			{
				MapC[mapnumber].m_cItem[iItemCount].m_Time = GetTickCount() + 300000;
				MapC[mapnumber].m_cItem[iItemCount].m_LootTime = GetTickCount() + 20000;
			}

			if ( iRetMapNumber >= 246 && iRetMapNumber <= 253)
			{
				MapC[mapnumber].m_cItem[iItemCount].m_Time = GetTickCount() + 900000;
				MapC[mapnumber].m_cItem[iItemCount].m_LootTime = GetTickCount() + 10000;

				int iBridgeIndex = g_BloodCastle.GetBridgeLevel(mapnumber);
				g_BloodCastle.m_BridgeData[iBridgeIndex].m_nBC_QUESTITEM_SERIAL = lpMsg->m_Number;
			}
		}

		if ( IT_MAP_RANGE( mapnumber ) )
		{
			if( lpMsg->Type == ITEMGET(14,64) )
			{
				MapC[mapnumber].m_cItem[iItemCount].m_Time = GetTickCount() + 15000;
				MapC[mapnumber].m_cItem[iItemCount].m_LootTime = GetTickCount() + 5000;
			}

			if( lpMsg->Type == ITEMGET(12,15) )
			{
				MapC[mapnumber].m_cItem[iItemCount].m_Time = GetTickCount() + 300000;
				MapC[mapnumber].m_cItem[iItemCount].m_LootTime = GetTickCount() + 10000;
			}
		}

		//int iItemCount;
		BYTE NewOption[MAX_EXOPTION_SIZE];

		for ( int i=0;i<MAX_EXOPTION_SIZE;i++)
		{
			NewOption[i] = FALSE;
		}

		if ( (lpMsg->NewOption & 0x20) != 0 )
		{
			NewOption[0] = TRUE;
		}
		if ( (lpMsg->NewOption & 0x10) != 0 )
		{
			NewOption[1] = TRUE;
		}

		if ( (lpMsg->NewOption & 0x8) != 0 )
		{
			NewOption[2] = TRUE;
		}
		if ( (lpMsg->NewOption & 0x4) != 0 )
		{
			NewOption[3] = TRUE;
		}

		if ( (lpMsg->NewOption & 0x2) != 0 )
		{
			NewOption[4] = TRUE;
		}
		if ( (lpMsg->NewOption & 0x1) != 0 )
		{
			NewOption[5] = TRUE;
		}

#if (DEBUG_DROP==1)
		DROP_LOG_DEBUG.Output("Monster Item Drop [%d][%d][%d] : serial:%d [%s][%d][%d][%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d]",
			mapnumber, lpMsg->x, lpMsg->y, lpMsg->m_Number, ItemAttribute[lpMsg->Type].Name, lpMsg->Level,
			lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, 
			NewOption[0],NewOption[1],NewOption[2],NewOption[3],NewOption[4],NewOption[5],NewOption[6],
			lpMsg->SetOption);
#else
		LogAddTD("Monster Item Drop [%d][%d][%d] : serial:%d [%s][%d][%d][%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d]",
			mapnumber, lpMsg->x, lpMsg->y, lpMsg->m_Number, ItemAttribute[lpMsg->Type].Name, lpMsg->Level,
			lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, 
			NewOption[0],NewOption[1],NewOption[2],NewOption[3],NewOption[4],NewOption[5],NewOption[6],
			lpMsg->SetOption);
#endif
		
	}
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Item Move Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x53
 */
struct SDHP_ITEMMOVESAVE
{
	PBMSG_HEAD h;	// C1:53
	DWORD Serial;	// 4
	char ServerName[20];	// 8
	char Account[10];	// 1C
	char Name[10];	// 26
	char ItemName[21];	// 30
	BYTE X;	// 45
	BYTE Y;	// 46
	BYTE ItemLevel;	// 47
	BYTE ItemOp1;	// 48
	BYTE ItemOp2;	// 49
	BYTE ItemOp3;	// 4A
};



void ItemMovePathSave(char* ActID, char* Name, BYTE level, BYTE mapnumber, BYTE x, BYTE y, char* Item, BYTE op1, BYTE op2, BYTE op3, DWORD serial)
{
	SDHP_ITEMMOVESAVE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x53;
	pMsg.h.size = sizeof(pMsg);
	pMsg.ItemLevel = level;
	pMsg.Serial = serial;
	pMsg.ItemOp1 = op1;
	pMsg.ItemOp2 = op2;
	pMsg.ItemOp3 = op3;
	pMsg.X = x;
	pMsg.Y = y;
	memcpy(pMsg.Account, ActID, sizeof(pMsg.Account));
	strcpy(pMsg.ItemName, Item);
	memcpy(pMsg.Name, Name, sizeof(pMsg.Name));
	strcpy(pMsg.ServerName, szServerName);

	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}





/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Skill Key Data Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x60
 */


struct SDHP_SKILLKEYDATA
{
	PBMSG_HEAD h;	// C1:60
	char Name[10];	// 3
	BYTE SkillKeyBuffer[20];	// D
	BYTE GameOption;	// 17
	BYTE QkeyDefine;	// 18
	BYTE WkeyDefine;	// 19
	BYTE EkeyDefine;	// 1A
	BYTE ChatWindow;	// 1B
	BYTE RkeyDefine;
	DWORD QWERLevel;
	char EnableTransfromMode; //1.01.06
};



void DGOptionDataSend(int aIndex, char* szName, BYTE * KeyBuffer, BYTE GO, BYTE Qk, BYTE Wk, BYTE Ek,BYTE ChatWnd, BYTE Rk,DWORD QWERLevel, char EnableTransfromMode)
{
	SDHP_SKILLKEYDATA pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x60;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.Name, szName, sizeof(pMsg.Name));
	memcpy(pMsg.SkillKeyBuffer, KeyBuffer, sizeof(pMsg.SkillKeyBuffer));
	pMsg.GameOption = GO;
	pMsg.QkeyDefine = Qk;
	pMsg.WkeyDefine = Wk;
	pMsg.EkeyDefine = Ek;
	pMsg.ChatWindow = ChatWnd;
	pMsg.RkeyDefine = Rk;
	pMsg.QWERLevel = QWERLevel;
	pMsg.EnableTransfromMode = EnableTransfromMode; //1.01.06

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}




void DGOptionDataRecv(SDHP_SKILLKEYDATA_SEND * lpMsg)
{
	int aIndex = lpMsg->aIndex;
	char szName[MAX_ACCOUNT_LEN+1];

	if ( aIndex < 0 || aIndex >= OBJMAX )
	{
		return;
	}

	memset(szName, 0, sizeof(szName));
	memcpy(szName, lpMsg->Name, sizeof(lpMsg->Name));

	if ( strcmp(gObj[aIndex].Name, szName) != 0 )
	{
		return;
	}

	::GCSkillKeySend(aIndex, lpMsg->SkillKeyBuffer, lpMsg->GameOption, lpMsg->QkeyDefine, lpMsg->WkeyDefine, lpMsg->EkeyDefine, lpMsg->ChatWindow, lpMsg->RkeyDefine, lpMsg->QWERLevel);
	gObj[aIndex].m_EnableUseChangeSkin = lpMsg->EnableTransfromMode;
    gObjViewportListProtocolCreate(&gObj[aIndex]);
}






void DGMoveOtherServer(SDHP_CHARACTER_TRANSFER_RESULT * lpMsg)
{
	if ( !gObjIsConnectedGP(lpMsg->Number))
	{
		LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->Number];

	if ( lpMsg->Result != 0 )
	{
		PMSG_DEFRESULT pResult;

		PHeadSetB((LPBYTE)&pResult, 0x99, sizeof(pResult));
		pResult.result = 1;

		DataSend(lpMsg->Number, (LPBYTE)&pResult, pResult.h.size);

		LogAddTD("[CharTrasfer] Fail [%s][%s] (%d)",
			lpObj->AccountID, lpObj->Name, lpMsg->Result);
		
		lpObj->m_MoveOtherServer = 0;
		
		GCServerMsgStringSend("문제 발생시 change@webzen.co.kr로 문의해 주시기바랍니다",lpObj->m_Index, 1);
		// Deathway translation here
		return;
	}

	LogAddTD("[CharTrasfer] Success [%s][%s] (%d)",
		lpObj->AccountID, lpObj->Name, lpMsg->Result);

	GCServerMsgStringSend("접속이 종료됩니다.", lpObj->m_Index, 1);// Deathway translation here
	GCServerMsgStringSend("분할 서버로 접속해주시기 바랍니다.", lpObj->m_Index, 1);// Deathway translation here
	GJSetCharacterInfo(lpObj, lpObj->m_Index, 0);
	lpObj->LoadWareHouseInfo = false;
	gObjCloseSet(lpObj->m_Index, 2);
	lpObj->m_MoveOtherServer = 0;
}




struct Request_PetItem_Info
{
	BYTE nPos;	// 0
	int nSerial;	// 4
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Request Pet Info Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC2
 *	HeadCode  : 0x56
 *	BASE PACKET - Complete with Request_PetItem_Info
 */
struct SDHP_REQUEST_PETITEM_INFO
{
	PWMSG_HEAD h;
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
};


void gObjRequestPetItemInfo(int aIndex, int inventype)
{
	LPOBJ lpObj = &gObj[aIndex];
	char pbuffer[1024];
	int lofs = sizeof(SDHP_REQUEST_PETITEM_INFO);
	int founditemcount = 0;
	Request_PetItem_Info petIteminfo;

	if ( inventype == 0 )
	{
		for ( int n=0;n<INVENTORY_SIZE;n++)
		{
			if ( lpObj->pInventory[n].IsItem() )
			{
				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,4) || lpObj->pInventory[n].m_Type == ITEMGET(13,5) )
				{
					founditemcount++;
					petIteminfo.nPos = n;
					petIteminfo.nSerial = lpObj->pInventory[n].m_Number;
					memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
					lofs +=sizeof(petIteminfo);
				}
			}
		}
	}
	else if ( inventype == 1 )
	{
		for ( int n=0;n<WAREHOUSE_SIZE;n++)
		{
			if ( lpObj->pWarehouse[n].IsItem() )
			{
				if ( lpObj->pWarehouse[n].m_Type == ITEMGET(13,4) || lpObj->pWarehouse[n].m_Type == ITEMGET(13,5) )
				{
					founditemcount++;
					petIteminfo.nPos = n;
					petIteminfo.nSerial = lpObj->pWarehouse[n].m_Number;
					memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
					lofs +=sizeof(petIteminfo);
				}
			}
		}
	}
	
	if ( founditemcount )
	{
		SDHP_REQUEST_PETITEM_INFO pMsg;

		pMsg.h.set((LPBYTE)&pMsg, 0x56, lofs + sizeof(pMsg));
		memcpy(pMsg.AccountID, lpObj->AccountID, MAX_ACCOUNT_LEN);
		pMsg.Number = aIndex;
		pMsg.InvenType = inventype;
		pMsg.nCount = founditemcount;
		memcpy(pbuffer, &pMsg, sizeof(pMsg));

		cDBSMng.Send((PCHAR)pbuffer, lofs+sizeof(pMsg));
	}
}



struct Recv_PetItem_Info
{
	BYTE nPos;	// 0
	int nSerial;	// 4
	BYTE Level;	// 8
	int Exp;	// C
};


struct SDHP_RECV_PETITEM_INFO
{
	PWMSG_HEAD h;
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
};




void DGRecvPetItemInfo(LPBYTE lpData)
{
	SDHP_RECV_PETITEM_INFO* lpMsg = (SDHP_RECV_PETITEM_INFO *)lpData;
	Recv_PetItem_Info* pPetItemInfo = (Recv_PetItem_Info *)((DWORD)lpData + sizeof(SDHP_RECV_PETITEM_INFO));
	int aIndex = lpMsg->Number;
	char szAccountId[MAX_ACCOUNT_LEN+1];
	szAccountId[MAX_ACCOUNT_LEN]=0;

	memcpy(szAccountId, lpMsg->AccountID, MAX_ACCOUNT_LEN);
	
	if ( !gObjIsAccontConnect(aIndex, szAccountId))
	{
		LogAddC(2, "Request to receive petitem infomation doesn't match the user %s", szAccountId);
		CloseClient(aIndex);
		
		return;
	}

	if ( lpMsg->InvenType == 0 )
	{
		BOOL ReCalc = 0;

		for ( int n=0;n<lpMsg->nCount;n++)
		{
			if ( pPetItemInfo[n].nPos < INVETORY_WEAR_SIZE )
			{
				ReCalc = 1;
			}

			if ( gObj[aIndex].pInventory[pPetItemInfo[n].nPos].IsItem() )
			{
				if ( gObj[aIndex].pInventory[pPetItemInfo[n].nPos].m_Number == pPetItemInfo[n].nSerial )
				{
					gObj[aIndex].pInventory[pPetItemInfo[n].nPos].SetPetItemInfo(pPetItemInfo[n].Level, pPetItemInfo[n].Exp);
				}
			}
		}

		if ( ReCalc != FALSE )
		{
			gObjCalCharacter(aIndex);
		}
	}
	else if ( lpMsg->InvenType == 1 )
	{
		for ( int n=0;n<lpMsg->nCount;n++)
		{

			if ( gObj[aIndex].pWarehouse[pPetItemInfo[n].nPos].IsItem() )
			{
				if ( gObj[aIndex].pWarehouse[pPetItemInfo[n].nPos].m_Number == pPetItemInfo[n].nSerial )
				{
					gObj[aIndex].pWarehouse[pPetItemInfo[n].nPos].SetPetItemInfo(pPetItemInfo[n].Level, pPetItemInfo[n].Exp);
				}
			}
		}

	}
}



struct Save_PetItem_Info
{
	int nSerial;	// 0
	BYTE Level;	// 3
	int Exp;	// 8
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu PetInfo Save Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC2
 *	HeadCode  : 0x55
 *	BASE PACKET - Complete with Save_PetItem_Info
 */
struct SDHP_SAVE_PETITEM_INFO
{
	PWMSG_HEAD h;
	BYTE nCount;	// 4
};


void gObjSavePetItemInfo(int aIndex, int inventype)
{
	LPOBJ lpObj = &gObj[aIndex];
	char pbuffer[1024];
	int lofs = sizeof(SDHP_SAVE_PETITEM_INFO);
	int founditemcount = 0;
	Save_PetItem_Info petIteminfo;

	if ( inventype == 0 )
	{
		for ( int n=0;n<INVENTORY_SIZE;n++)
		{
			if ( lpObj->pInventory[n].IsItem() )
			{
				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,4) || lpObj->pInventory[n].m_Type == ITEMGET(13,5) )
				{
					if ( lpObj->pInventory[n].m_IsLoadPetItemInfo != FALSE )
					{
						founditemcount++;
						petIteminfo.nSerial = lpObj->pInventory[n].m_Number;
						petIteminfo.Level = lpObj->pInventory[n].m_PetItem_Level;
						petIteminfo.Exp = lpObj->pInventory[n].m_PetItem_Exp;

						LogAddTD("[%s][%s][PI][%d,%s]serial:[%d] Level:%d Exp:%d",
							lpObj->AccountID, lpObj->Name, n, lpObj->pInventory[n].GetName(),petIteminfo.nSerial,
							petIteminfo.Level, petIteminfo.Exp);

						memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
						lofs += sizeof(petIteminfo);
					}
				}
			}
		}
	}
	else if ( inventype == 1 )
	{
		for ( int n=0;n<WAREHOUSE_SIZE;n++)
		{
			if ( lpObj->pWarehouse[n].IsItem() )
			{
				if ( lpObj->pWarehouse[n].m_Type == ITEMGET(13,4) || lpObj->pWarehouse[n].m_Type == ITEMGET(13,5) )
				{
					if ( lpObj->pWarehouse[n].m_IsLoadPetItemInfo != FALSE )
					{
						founditemcount++;
						petIteminfo.nSerial = lpObj->pWarehouse[n].m_Number;
						petIteminfo.Level = lpObj->pWarehouse[n].m_PetItem_Level;
						petIteminfo.Exp = lpObj->pWarehouse[n].m_PetItem_Exp;

						LogAddTD("[%s][%s][PW][%d,%s]serial:[%d] Level:%d Exp:%d",
							lpObj->AccountID, lpObj->Name, n, lpObj->pWarehouse[n].GetName(),petIteminfo.nSerial,
							petIteminfo.Level, petIteminfo.Exp);

						memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
						lofs += sizeof(petIteminfo);
					}
				}
			}
		}
	}

	if ( founditemcount )
	{
		SDHP_SAVE_PETITEM_INFO pMsg;

		pMsg.h.set((LPBYTE)&pMsg, 0x57, lofs+sizeof(pMsg));
		pMsg.nCount = founditemcount;
		memcpy(pbuffer, &pMsg, sizeof(pMsg));

		cDBSMng.Send(pbuffer, lofs+sizeof(pMsg));
	}
}




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_CASTLEDATA
{
	PBMSG_HEAD2 h;	// C1:80:00
	WORD wMapSvrNum;	// 4
	int iCastleEventCycle;	// 8
};



void GS_GDReqCastleTotalInfo(int iMapSvrGroup, int iCastleEventCycle)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_CASTLEDATA pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x00, sizeof(CSP_REQ_CASTLEDATA));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleEventCycle = iCastleEventCycle;

	::cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_OWNERGUILDMASTER
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};


void GS_GDReqOwnerGuildMaster(int iMapSvrGroup, int aIndex)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_OWNERGUILDMASTER pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x01, sizeof(CSP_REQ_OWNERGUILDMASTER));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}





/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_NPCBUY
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcDfLevel;	// 14
	int iNpcRgLevel;	// 18
	int iNpcMaxHp;	// 1C
	int iNpcHp;	// 20
	BYTE btNpcX;	// 24
	BYTE btNpcY;	// 25
	BYTE btNpcDIR;	// 26
	int iBuyCost;	// 28
};


void GS_GDReqCastleNpcBuy(int iMapSvrGroup, int aIndex, int iNpcNumber, int iNpcIndex, int iNpcDfLevel, int iNpcRgLevel, int iNpcMaxHP, int iNpcHP, BYTE btNpcX, BYTE btNpcY, BYTE btNpcDIR, int iBuyCost)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_NPCBUY pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x03, sizeof(CSP_REQ_NPCBUY));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	pMsg.iNpcDfLevel = iNpcDfLevel;
	pMsg.iNpcRgLevel = iNpcRgLevel;
	pMsg.iNpcMaxHp = iNpcMaxHP;
	pMsg.iNpcHp = iNpcHP;
	pMsg.btNpcX = btNpcX;
	pMsg.btNpcY = btNpcY;
	pMsg.btNpcDIR = btNpcDIR;
	pMsg.iBuyCost = iBuyCost;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}





/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_NPCREPAIR
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iRepairCost;	// 14
};




void GS_GDReqCastleNpcRepair(int iMapSvrGroup, int aIndex, int iNpcNumber, int iNpcIndex, int iRepairCost)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_NPCREPAIR pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x04, sizeof(CSP_REQ_NPCREPAIR));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	pMsg.iRepairCost = iRepairCost;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_NPCUPGRADE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcUpType;	// 14
	int iNpcUpValue;	// 18
	int iNpcUpIndex;	// 1C
};


void GS_GDReqCastleNpcUpgrade(int iMapSvrGroup, int aIndex, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue, int iNpcUpIndex)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_NPCUPGRADE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x05, sizeof(CSP_REQ_NPCUPGRADE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	pMsg.iNpcUpType = iNpcUpType;
	pMsg.iNpcUpValue = iNpcUpValue;
	pMsg.iNpcUpIndex = iNpcUpIndex;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_TAXINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 3
	int iIndex;	// 8
};


void GS_GDReqTaxInfo(int iMapSvrGroup, int aIndex)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_TAXINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x06, sizeof(CSP_REQ_TAXINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_TAXRATECHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iTaxKind;	// C
	int iTaxRate;	// 10
};



void GS_GDReqTaxRateChange( int iMapSvrGroup, int aIndex, int iTaxType, int iTaxRate)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_TAXRATECHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x07, sizeof(CSP_REQ_TAXRATECHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iTaxRate = iTaxRate; 
	pMsg.iTaxKind = iTaxType;
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}





/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_MONEYCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iMoneyChanged;	// C
};


void GS_GDReqCastleMoneyChange(int iMapSvrGroup, int aIndex,int iMoneyChange)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_MONEYCHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x08, sizeof(CSP_REQ_MONEYCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iMoneyChanged = iMoneyChange;
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_SDEDCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	WORD wStartYear;	// C
	BYTE btStartMonth;	// E
	BYTE btStartDay;	// F
	WORD wEndYear;	// 10
	BYTE btEndMonth;	// 12
	BYTE btEndDay;	// 13
};



void GS_GDReqCastleDateChange(int iMapSvrGroup, int aIndex,WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_SDEDCHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x09, sizeof(CSP_REQ_SDEDCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.wStartYear = wStartYear;
	pMsg.btStartMonth = btStartMonth;
	pMsg.btStartDay = btStartDay;
	pMsg.wEndYear = wEndYear;
	pMsg.btEndMonth = btEndMonth;
	pMsg.btEndDay = btEndDay;
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_GUILDREGINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
};


void GS_GDReqGuildMarkReqInfo(int iMapSvrGroup, int aIndex)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_GUILDREGINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0A, sizeof(CSP_REQ_GUILDREGINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	memcpy(pMsg.szGuildName, gObj[aIndex].GuildName, 8);
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_SIEGEENDCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsSiegeEnded;	// 8
};


void GS_GDReqSiegeEndedChange(int iMapSvrGroup, BOOL bIsSiegeEnded)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}


	CSP_REQ_SIEGEENDCHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0B, sizeof(CSP_REQ_SIEGEENDCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.bIsSiegeEnded = bIsSiegeEnded;
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_CASTLEOWNERCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsCastleOccupied;	// 8
	char szOwnerGuildName[8];	// C
};


void GS_GDReqCastleOwnerChange( int iMapSvrGroup, BOOL bIsCastleOccupied, char* lpszGuildName)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( lpszGuildName == NULL )
	{
		return;
	}


	CSP_REQ_CASTLEOWNERCHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0C, sizeof(CSP_REQ_CASTLEOWNERCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.bIsCastleOccupied = bIsCastleOccupied;
	memcpy(pMsg.szOwnerGuildName, lpszGuildName, 8);
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_REGATTACKGUILD
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szEnemyGuildName[8];	// C
};


void GS_GDReqRegAttackGuild( int iMapSvrGroup, int aIndex)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_REGATTACKGUILD pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0D, sizeof(CSP_REQ_REGATTACKGUILD));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	memcpy(pMsg.szEnemyGuildName, gObj[aIndex].GuildName, 8);
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_CASTLESIEGEEND
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};


void GS_GDReqRestartCastleState( int iMapSvrGroup)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_CASTLESIEGEEND pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0E, sizeof(CSP_REQ_CASTLESIEGEEND));
	pMsg.wMapSvrNum = iMapSvrGroup;
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_MAPSVRMULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
};


void GS_GDReqMapSvrMsgMultiCast( int iMapSvrGroup,char * lpszMsgText)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( lpszMsgText == NULL )
	{
		return;
	}


	CSP_REQ_MAPSVRMULTICAST pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0F, sizeof(CSP_REQ_MAPSVRMULTICAST));
	pMsg.wMapSvrNum = iMapSvrGroup;
	strcpy(pMsg.szMsgText, lpszMsgText);
	pMsg.szMsgText[127] = 0;
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_GUILDREGMARK
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	int iItemPos;	// 14
};


void GS_GDReqGuildMark( int iMapSvrGroup, int aIndex, int iItemPos)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}


	CSP_REQ_GUILDREGMARK pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x10, sizeof(CSP_REQ_GUILDREGMARK));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iItemPos = iItemPos;
	memcpy(pMsg.szGuildName, gObj[aIndex].GuildName, sizeof(pMsg.szGuildName));
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_GUILDRESETMARK
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
};


void GS_GDReqGuildMarkReset( int iMapSvrGroup, int aIndex, char* lpszGuildName)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}

	if ( lpszGuildName == NULL )
	{
		return;
	}

	CSP_REQ_GUILDRESETMARK pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x11, sizeof(CSP_REQ_GUILDRESETMARK));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	memcpy(pMsg.szGuildName, lpszGuildName, 8);
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_GUILDSETGIVEUP
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	BOOL bIsGiveUp;	// 14
};


void GS_GDReqGuildSetGiveUp( int iMapSvrGroup, int aIndex, BOOL bIsGiveUp)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}

	CSP_REQ_GUILDSETGIVEUP pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x12, sizeof(CSP_REQ_GUILDSETGIVEUP));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.bIsGiveUp = bIsGiveUp;
	memcpy(pMsg.szGuildName, gObj[aIndex].GuildName, 8);
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_NPCREMOVE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iNpcNumber;	// 8
	int iNpcIndex;	// C
};


void GS_GDReqNpcRemove(int iMapSvrGroup, int iNpcNumber, int iNpcIndex)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_NPCREMOVE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x16, sizeof(CSP_REQ_NPCREMOVE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_CASTLESTATESYNC
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
};


void GS_GDReqCastleStateSync( int iMapSvrGroup, int iCastleState, int iTaxRateChaos, int iTaxRateStore, int iTaxHuntZone, char* lpszOwnerGuild)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_CASTLESTATESYNC pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x17, sizeof(CSP_REQ_CASTLESTATESYNC));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleState = iCastleState;
	pMsg.iTaxRateChaos = iTaxRateChaos;
	pMsg.iTaxRateStore = iTaxRateStore;
	pMsg.iTaxHuntZone = iTaxHuntZone;
	memcpy(pMsg.szOwnerGuildName, lpszOwnerGuild, 8);
	
	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_CASTLETRIBUTEMONEY
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleTributeMoney;	// 8
};



void GS_GDReqCastleTributeMoney(int iMapSvrGroup, int iCastleTributeMoney)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	if ( iCastleTributeMoney < 0 )
	{
		return;
	}

	CSP_REQ_CASTLETRIBUTEMONEY pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x18, sizeof(CSP_REQ_CASTLETRIBUTEMONEY));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleTributeMoney = iCastleTributeMoney;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_RESETCASTLETAXINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};


void GS_GDReqResetCastleTaxInfo( int iMapSvrGroup)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_RESETCASTLETAXINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x19, sizeof(CSP_REQ_RESETCASTLETAXINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_RESETSIEGEGUILDINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};


void GS_GDReqResetSiegeGuildInfo( int iMapSvrGroup)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_RESETSIEGEGUILDINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x1A, sizeof(CSP_REQ_RESETSIEGEGUILDINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode   :
 */
struct CSP_REQ_RESETREGSIEGEINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 2
};



void GS_GDReqResetReqSiegeInfo( int iMapSvrGroup)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_RESETREGSIEGEINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x1B, sizeof(CSP_REQ_RESETREGSIEGEINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_CSINITDATA
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCastleEventCycle;	// 8
};


void GS_GDReqCastleInitData( int iMapSvrGroup, int iCastleEventCycle)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_CSINITDATA pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x81, sizeof(CSP_REQ_CSINITDATA));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleEventCycle = iCastleEventCycle;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}




/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_NPCDATA
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};



void GS_GDReqCaslteNpcInfo( int iMapSvrGroup, int aIndex)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_NPCDATA pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x82, sizeof(CSP_REQ_NPCDATA));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_ALLGUILDREGINFO
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};


void GS_GDReqAllGuildMarkRegInfo( int iMapSvrGroup, int aIndex)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_ALLGUILDREGINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x83, sizeof(CSP_REQ_ALLGUILDREGINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_CALCREGGUILDLIST
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};


void GS_GDReqCalcRegGuildList( int iMapSvrGroup)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_CALCREGGUILDLIST pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x85, sizeof(CSP_REQ_CALCREGGUILDLIST));
	pMsg.wMapSvrNum = iMapSvrGroup;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 
 */
struct CSP_REQ_CSLOADTOTALGUILDINFO
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};


void GS_GDReqCsLoadTotalGuildInfo( int iMapSvrGroup)
{
	if ( iMapSvrGroup < 0 )
	{
		return;
	}

	CSP_REQ_CSLOADTOTALGUILDINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x88, sizeof(CSP_REQ_CSLOADTOTALGUILDINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	cDBSMng.Send((char*)&pMsg, pMsg.h.size);
}

void GS_GDReqGuildMarkRegInfo(int iMapSvrGroup, int aIndex)
{
#if (GS_CASTLE == 1)
	if(iMapSvrGroup < 0)
		return;

	if(OBJMAX_RANGE(aIndex) == FALSE)
		return;

	CSP_REQ_GUILDREGINFO pMsg;
	pMsg.h.set((PBYTE)&pMsg,0x80,0x0A,sizeof(pMsg));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;

	memcpy(pMsg.szGuildName,gObj[aIndex].GuildName,8);
	cDBSMng.Send((char*)&pMsg,pMsg.h.size);
#endif
}

void GS_GDReqSiegeDateChange(int iMapSvrGroup, int aIndex, WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay)
{
#if (GS_CASTLE == 1)
	if(iMapSvrGroup < 0)
		return;

	CSP_REQ_SDEDCHANGE pMsg;

	pMsg.h.set((PBYTE)&pMsg,0x80,9,sizeof(pMsg));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.wStartYear = wStartYear;
	pMsg.btStartMonth = btStartMonth;
	pMsg.btStartDay = btStartDay;
	pMsg.wEndYear = wEndYear;
	pMsg.btEndMonth = btEndMonth;
	pMsg.btEndDay = btEndDay;

	cDBSMng.Send((char*)&pMsg,pMsg.h.size);
#endif
}

void GS_GDReqRegGuildMark(int iMapSvrGroup, int aIndex, int iItemPos)
{
#if (GS_CASTLE == 1)
	if(iMapSvrGroup < 0)
		return;

	if(OBJMAX_RANGE(aIndex) == FALSE)
		return;

	CSP_REQ_GUILDREGMARK pMsg;

	pMsg.h.set((PBYTE)&pMsg,0x80,0x10,sizeof(pMsg));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iItemPos = iItemPos;
	memcpy(pMsg.szGuildName,gObj[aIndex].GuildName,8);

	cDBSMng.Send((char*)&pMsg,sizeof(pMsg));
#endif
}

void GS_GDReqResetRegSiegeInfo(int iMapSvrGroup)
{
#if (GS_CASTLE == 1)
	if(iMapSvrGroup < 0)
		return;

	CSP_REQ_RESETREGSIEGEINFO pMsg;
	pMsg.h.set((PBYTE)&pMsg,0x80,0x1B,sizeof(pMsg));
	pMsg.wMapSvrNum = iMapSvrGroup;

	cDBSMng.Send((char*)&pMsg,pMsg.h.size);
#endif
}

struct CSP_ANS_CASTLEDATA
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	unsigned short wStartYear;
	unsigned char btStartMonth;
	unsigned char btStartDay;
	unsigned short wEndYear;
	unsigned char btEndMonth;
	unsigned char btEndDay;
	unsigned char btIsSiegeGuildList;
	unsigned char btIsSiegeEnded;
	unsigned char btIsCastleOccupied;
	char szCastleOwnGuild[0x8];
	__int64 i64CastleMoney;
	int iTaxRateChaos;
	int iTaxRateStore;
	int iTaxHuntZone;
	int iFirstCreate;
};

void GS_DGAnsCastleTotalInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_CASTLEDATA * lpMsg = (CSP_ANS_CASTLEDATA *)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x00] GS_DGAnsCastleTotalInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
#endif
}

struct CSP_ANS_OWNERGUILDMASTER
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szCastleOwnGuild[0x8];
	char szCastleOwnGuildMaster[0xA];
};

void GS_DGAnsOwnerGuildMaster(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_OWNERGUILDMASTER * lpMsg = (CSP_ANS_OWNERGUILDMASTER *)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x00] GS_DGAnsCastleTotalInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GCAnsCastleSiegeState(lpMsg->iIndex,lpMsg->iResult,lpMsg->szCastleOwnGuild,lpMsg->szCastleOwnGuildMaster);
#endif
}


struct CSP_ANS_NPCBUY
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iNpcNumber;
	int iNpcIndex;
	int iBuyCost;
};

void GS_DGAnsCastleNpcBuy(unsigned char * lpRecv )
{
#if (GS_CASTLE == 1)
	CSP_ANS_NPCBUY * lpMsg = (CSP_ANS_NPCBUY *)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x03] GS_DGAnsCastleNpcBuy() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		int bRETVAL = g_CastleSiege.AddDbNPC(lpMsg->iNpcNumber,lpMsg->iNpcIndex);

		if(bRETVAL == TRUE)
		{
			if(gObjIsConnected(lpMsg->iIndex))
			{
				gObj[lpMsg->iIndex].Money -= lpMsg->iBuyCost;

				if(gObj[lpMsg->iIndex].Money < 0)
				{
					gObj[lpMsg->iIndex].Money = 0;
				}

				GCMoneySend(lpMsg->iIndex,gObj[lpMsg->iIndex].Money);
			}

			LogAddTD("[CastleSiege] GS_DGAnsCastleNpcBuy() - CCastleSiege::AddDbNPC() OK - Npc:(CLS:%d, IDX:%d)",
				lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		}
		else
		{
			LogAddTD("[CastleSiege] GS_DGAnsCastleNpcBuy() - CCastleSiege::AddDbNPC() FAILED - Npc:(CLS:%d, IDX:%d)",
				lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		}
	}
	else
	{
		LogAddTD("[CastleSiege] GS_DGAnsCastleNpcBuy() FAILED - Result:(%d), Npc:(CLS:%d, IDX:%d)",
			lpMsg->iResult,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
	}

	GCAnsNpcBuy(lpMsg->iIndex,lpMsg->iResult,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
#endif
}

struct CSP_ANS_NPCREPAIR
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcMaxHp;
	int iNpcHp;
	int iRepairCost;
};

void GS_DGAnsCastleNpcRepair(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_NPCREPAIR * lpMsg = (CSP_ANS_NPCREPAIR*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x04] GS_DGAnsCastleNpcRepair() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		int bRETVAL = g_CastleSiege.RepairDbNPC(lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcHp,lpMsg->iNpcMaxHp);

		if(bRETVAL == TRUE)
		{
			if(gObjIsConnected(lpMsg->iIndex))
			{
				gObj[lpMsg->iIndex].Money -= lpMsg->iRepairCost;

				if(gObj[lpMsg->iIndex].Money < 0)
					gObj[lpMsg->iIndex].Money = 0;

				GCMoneySend(lpMsg->iIndex,gObj[lpMsg->iIndex].Money);
			}

			LogAddTD("[CastleSiege] GS_DGAnsCastleNpcRepair() - CCastleSiege::RepairDbNPC() OK - Npc:(CLS:%d, IDX:%d)",
				lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		}
		else
		{
			LogAddTD("[CastleSiege] GS_DGAnsCastleNpcRepair() - CCastleSiege::RepairDbNPC() FAILED - Npc:(CLS:%d, IDX:%d)",
				lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		}
	}
	else
	{
		LogAddTD("[CastleSiege] GS_DGAnsCastleNpcRepair() FAILED - Result:(%d), Npc:(CLS:%d, IDX:%d)",
			lpMsg->iResult,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		GCAnsNpcRepair(lpMsg->iIndex,lpMsg->iResult,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcHp,lpMsg->iNpcMaxHp);
	}
#endif
}

struct CSP_ANS_NPCUPGRADE
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcUpType;
	int iNpcUpValue;
	int iNpcUpIndex;
};

void GS_DGAnsCastleNpcUpgrade(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_NPCUPGRADE * lpMsg = (CSP_ANS_NPCUPGRADE*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x05] GS_DGAnsCastleNpcUpgrade() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == FALSE)
	{
		LogAddC(2,"[CastleSiege] ERROR - Castle NPC Upgrade Fail() (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)",
			lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
	}
	else
	{
		g_CastleSiege.UpgradeDbNPC(lpMsg->iIndex,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue,lpMsg->iNpcUpIndex);
		LogAddTD("[CastleSiege] [0x80][0x06] GS_DGAnsTaxInfo() - Npc Upgrade OK (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)",
			lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
	}

	GCAnsNpcUpgrade(lpMsg->iIndex,lpMsg->iResult,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
#endif
}

struct CSP_ANS_TAXINFO
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	__int64 i64CastleMoney;
	int iTaxRateChaos;
	int iTaxRateStore;
	int iTaxHuntZone;
};

void GS_DGAnsTaxInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_TAXINFO * lpMsg = (CSP_ANS_TAXINFO*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x06] GS_DGAnsTaxInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		GCAnsTaxMoneyInfo(lpMsg->iIndex,lpMsg->iResult,lpMsg->iTaxRateChaos,lpMsg->iTaxRateStore,lpMsg->i64CastleMoney);
		g_CastleSiege.SetCastleMoney(lpMsg->i64CastleMoney);
	}
#endif
}

struct CSP_ANS_TAXRATECHANGE
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iTaxKind;
	int iTaxRate;
};

void GS_DGAnsTaxRateChange(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_TAXRATECHANGE * lpMsg = (CSP_ANS_TAXRATECHANGE*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x07] GS_DGAnsTaxRateChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		g_CastleSiege.SetTaxRate(lpMsg->iTaxKind,lpMsg->iTaxRate);
		GCAnsTaxRateChange(lpMsg->iIndex,lpMsg->iResult,lpMsg->iTaxKind,lpMsg->iTaxRate);
	}
#endif
}

struct CSP_ANS_MONEYCHANGE
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iMoneyChanged;
	__int64 i64CastleMoney;
};

void GS_DGAnsCastleMoneyChange(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_MONEYCHANGE * lpMsg = (CSP_ANS_MONEYCHANGE*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x08] GS_DGAnsCastleMoneyChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		if(gObjIsConnected(lpMsg->iIndex) && lpMsg->iMoneyChanged < 0)
		{
			gObj[lpMsg->iIndex].Money -= lpMsg->iMoneyChanged;

			if(gObj[lpMsg->iIndex].Money > 2000000000)
			{
				gObj[lpMsg->iIndex].Money = 2000000000;
			}

			GCMoneySend(lpMsg->iIndex,gObj[lpMsg->iIndex].Money);
			LogAddTD("[CastleSiege] [0x80][0x08] GS_DGAnsCastleMoneyChange() - Withdraw Request OK [%s][%s] (ReqMoney:%d, TotMoney:%I64d)",
			gObj[lpMsg->iIndex].AccountID,gObj[lpMsg->iIndex].Name,lpMsg->iMoneyChanged,lpMsg->i64CastleMoney);
		}

		g_CastleSiege.SetCastleMoney(lpMsg->i64CastleMoney);
	}

	GCAnsMoneyDrawOut(lpMsg->iIndex,lpMsg->iResult,lpMsg->i64CastleMoney);
#endif
}

struct CSP_ANS_SDEDCHANGE
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	unsigned short wStartYear;
	unsigned char btStartMonth;
	unsigned char btStartDay;
	unsigned short wEndYear;
	unsigned char btEndMonth;
	unsigned char btEndDay;
};

void GS_DGAnsSiegeDateChange(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_SDEDCHANGE * lpMsg = (CSP_ANS_SDEDCHANGE*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x09] GS_DGAnsSiegeDateChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
#endif
}

void GS_DGAnsGuildMarkRegInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)

	CSP_ANS_GUILDREGINFO * lpMsg = (CSP_ANS_GUILDREGINFO*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x0A] GS_DGAnsGuildMarkRegInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GCAnsGuildRegInfo(lpMsg->iIndex,lpMsg->iResult,lpMsg);
#endif
}

struct CSP_ANS_SIEGEENDCHANGE
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int bIsSiegeEnded;
};

void GS_DGAnsSiegeEndedChange(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_SIEGEENDCHANGE * lpMsg = (CSP_ANS_SIEGEENDCHANGE*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x0B] GS_DGAnsSiegeEndedChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
#endif
}

struct CSP_ANS_CASTLEOWNERCHANGE
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int bIsCastleOccupied;
	char szOwnerGuildName[0x8];
};

void GS_DGAnsCastleOwnerChange(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_CASTLEOWNERCHANGE * lpMsg = (CSP_ANS_CASTLEOWNERCHANGE*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x0C] GS_DGAnsCastleOwnerChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
#endif
}

struct CSP_ANS_REGATTACKGUILD
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szEnemyGuildName[0x8];
};

void GS_DGAnsRegAttackGuild(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_REGATTACKGUILD * lpMsg = (CSP_ANS_REGATTACKGUILD*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x0D] GS_DGAnsRegAttackGuild() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GCAnsRegCastleSiege(lpMsg->iIndex,lpMsg->iResult,lpMsg->szEnemyGuildName);
#endif
}

struct CSP_ANS_CASTLESIEGEEND
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
};

void GS_DGAnsRestartCastleState(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_REGATTACKGUILD * lpMsg = (CSP_ANS_REGATTACKGUILD*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x0E] GS_DGAnsRestartCastleState() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	g_CastleSiege.ResetCastleCycle();
#endif
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : DataServer -> GameServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode	  :
 */
struct CSP_ANS_MAPSVRMULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
};


void GS_DGAnsMapSvrMsgMultiCast(LPBYTE lpRecv)
{
	CSP_ANS_MAPSVRMULTICAST * lpMsg = (CSP_ANS_MAPSVRMULTICAST *)lpRecv;

	if ( lpMsg == NULL )
	{
		return;
	}

	if ( lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup() )
	{
		LogAddC(2, "[CastleSiege] PACKET-ERROR [0x80][0x0F] GS_DGAnsMapSvrMsgMultiCast() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	char szNotice[144] = {0};
	memcpy(szNotice, lpMsg->szMsgText, 128);

	if ( strlen(szNotice) > 1 )
	{
		PMSG_NOTICE pNotice;

		TNotice::MakeNoticeMsg((TNotice *)&pNotice, 0, szNotice);
		TNotice::SetNoticeProperty((TNotice *)&pNotice, 0, _ARGB(255, 255, 200, 80), 1, 0, 20);
		TNotice::SendNoticeToAllUser((TNotice *)&pNotice);
	}

}

void GS_DGAnsRegGuildMark(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_GUILDREGMARK * lpMsg = (CSP_ANS_GUILDREGMARK*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x10] GS_DGAnsRegGuildMark() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GCAnsRegGuildMark(lpMsg->iIndex,lpMsg->iResult,lpMsg);
#endif
}

struct CSP_ANS_GUILDRESETMARK
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szGuildName[0x8];
	int iRegMarkCount;
};

void GS_DGAnsGuildMarkReset(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_GUILDRESETMARK * lpMsg = (CSP_ANS_GUILDRESETMARK*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x10] GS_DGAnsRegGuildMark() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
#endif
}

struct CSP_ANS_GUILDSETGIVEUP
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szGuildName[0x8];
	int bIsGiveUp;
	int iRegMarkCount;
};

#pragma warning ( disable : 4800 )
void GS_DGAnsGuildSetGiveUp(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_GUILDSETGIVEUP * lpMsg = (CSP_ANS_GUILDSETGIVEUP*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x12] GS_DGAnsGuildSetGiveUp() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GCAnsGiveUpCastleSiege(lpMsg->iIndex,lpMsg->iResult,lpMsg->bIsGiveUp,lpMsg->iRegMarkCount,lpMsg->szGuildName);
#endif
}
#pragma warning ( default : 4800 )

struct CSP_ANS_NPCREMOVE
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iNpcNumber;
	int iNpcIndex;
};

void GS_DGAnsNpcRemove(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_NPCREMOVE * lpMsg = (CSP_ANS_NPCREMOVE*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x10] GS_DGAnsRegGuildMark() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
#endif
}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : DataServer -> GameServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode	  :
 */
struct CSP_ANS_CASTLESTATESYNC
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
};


void GS_DGAnsCastleStateSync(LPBYTE lpRecv)
{
	CSP_ANS_CASTLESTATESYNC * lpMsg = (CSP_ANS_CASTLESTATESYNC *)lpRecv;

	if ( lpMsg == NULL )
	{
		return;
	}

	if ( lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup() )
	{
		return;
	}

	g_CastleSiegeSync.SetCastleState(lpMsg->iCastleState); 
	g_CastleSiegeSync.SetTaxRateChaos(lpMsg->iTaxRateChaos);
	g_CastleSiegeSync.SetTaxRateStore(lpMsg->iTaxRateStore);
	g_CastleSiegeSync.SetTaxHuntZone(lpMsg->iTaxHuntZone);
	g_CastleSiegeSync.SetCastleOwnerGuild(lpMsg->szOwnerGuildName);
}


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Castle Siege          Packet
 *	Direction : DataServer -> GameServer
 *  Code      : 0xC1
 *	HeadCode  : 
 *	SubCode	  :
 */
struct CSP_ANS_CASTLETRIBUTEMONEY
{
	PBMSG_HEAD2 h;
	int iResult;	// 4
	WORD wMapSvrNum;	// 8
};


void GS_DGAnsCastleTributeMoney(LPBYTE lpRecv)
{
	CSP_ANS_CASTLETRIBUTEMONEY * lpMsg = (CSP_ANS_CASTLETRIBUTEMONEY *)lpRecv;

	if ( lpMsg == NULL )
	{
		return;
	}

	if ( lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup() )
	{
		LogAddC(2, "[CastleSiege] PACKET-ERROR [0x80][0x18] GS_DGAnsCastleTributeMoney() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
	
	LogAddTD("[CastleSiege] [0x80][0x18] GS_DGAnsCastleTributeMoney() - Money Tribute OK (%d)",
		g_CastleSiegeSync.GetTributeMoney());
	g_CastleSiegeSync.ResetTributeMoney();
	
}

struct CSP_ANS_RESETCASTLETAXINFO
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
};

void GS_DGAnsResetCastleTaxInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_RESETCASTLETAXINFO * lpMsg = (CSP_ANS_RESETCASTLETAXINFO*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x19] GS_DGAnsResetCastleTaxInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult = 1)
	{
		g_CastleSiege.ResetCastleTaxInfo();
	}
#endif
}

struct CSP_ANS_RESETSIEGEGUILDINFO
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
};

void GS_DGAnsResetSiegeGuildInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_RESETSIEGEGUILDINFO * lpMsg = (CSP_ANS_RESETSIEGEGUILDINFO*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x1A] GS_DGAnsResetSiegeGuildInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
#endif
}

struct CSP_ANS_RESETREGSIEGEINFO
{
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
};

void GS_DGAnsResetRegSiegeInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_RESETREGSIEGEINFO * lpMsg = (CSP_ANS_RESETREGSIEGEINFO*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x80][0x1B] GS_DGAnsResetRegSiegeInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
#endif
}

void GS_DGAnsCastleInitData(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_CSINITDATA * lpMsg = (CSP_ANS_CSINITDATA*)lpRecv;
	CSP_CSINITDATA * lpMsgBody = (CSP_CSINITDATA*)&lpRecv[sizeof(CSP_ANS_CSINITDATA)];

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x81] GS_DGAnsCastleInitData() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(g_CastleSiege.GetDataLoadState() != 2)
	{
		LogAddC(2,"[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_2 (%d)",
			g_CastleSiege.GetDataLoadState());
		return;
	}

	g_CastleSiege.SetDataLoadState(3);

	if(lpMsg->iResult == 0)
	{
		LogAddC(2,"[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - lpMsg->iResult == 0");
		MsgBox("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - lpMsg->iResult == 0");
		return;
	}

	int bRET_VAL = FALSE;

	bRET_VAL = g_CastleSiege.SetCastleInitData(lpMsg);

	if(bRET_VAL == FALSE)
	{
		LogAddC(2,"[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - g_CastleSiege.SetCastleInitData() == FALSE");
		return;
	}

	bRET_VAL = g_CastleSiege.SetCastleNpcData(lpMsgBody,lpMsg->iCount);

	if(bRET_VAL == FALSE)
	{
		LogAddC(2,"[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - g_CastleSiege.SetCastleNpcData() == FALSE");
		return;
	}

	g_CastleSiege.SetDbDataLoadOK(TRUE);
	g_CastleSiege.SetDataLoadState(4);

	if(lpMsg->iFirstCreate == TRUE)
	{
		g_CastleSiege.FirstCreateDbNPC();
	}

	g_CastleSiege.Init();
#endif
}

struct CSP_ANS_NPCDATA
{
	PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iCount;
};

struct CSP_NPCDATA
{
	int iNpcNumber;
	int iNpcIndex;
	int iNpcDfLevel;
	int iNpcRgLevel;
	int iNpcMaxHp;
	int iNpcHp;
	unsigned char btNpcX;
	unsigned char btNpcY;
	unsigned char btNpcDIR;
};

void GS_DGAnsCastleNpcInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_NPCDATA * lpMsg = (CSP_ANS_NPCDATA*)lpRecv;
	CSP_NPCDATA * lpMsgBody = (CSP_NPCDATA*)&lpRecv[sizeof(CSP_ANS_NPCDATA)];

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x82] GS_DGAnsCastleNpcInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
#endif
}

struct CSP_ANS_ALLGUILDREGINFO
{
	PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iCount;
};

struct CSP_GUILDREGINFO
{
	char szGuildName[0x8];
	int iRegMarkCount;
	unsigned char bIsGiveUp;
	unsigned char btRegRank;
};

struct PMSG_ANS_CSREGGUILDLIST
{
	PWMSG_HEAD h;
	unsigned char btResult;
	int iCount;
};

struct PMSG_CSREGGUILDLIST
{
	char szGuildName[0x8];
	unsigned char btRegMarks1;
	unsigned char btRegMarks2;
	unsigned char btRegMarks3;
	unsigned char btRegMarks4;
	unsigned char btIsGiveUp;
	unsigned char btSeqNum;
};

void GS_DGAnsAllGuildMarkRegInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_ALLGUILDREGINFO * lpMsg = (CSP_ANS_ALLGUILDREGINFO*)lpRecv;
	CSP_GUILDREGINFO * lpMsgBody = (CSP_GUILDREGINFO*)&lpRecv[sizeof(CSP_ANS_ALLGUILDREGINFO)];

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x83] GS_DGAnsAllGuildMarkRegInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(gObjIsConnected(lpMsg->iIndex) == FALSE)
		return;

	char cBUFFER[0x684];

	PMSG_ANS_CSREGGUILDLIST * lpMsgSend = (PMSG_ANS_CSREGGUILDLIST*)cBUFFER;
	PMSG_CSREGGUILDLIST * lpMsgSendBody = (PMSG_CSREGGUILDLIST*)&cBUFFER[sizeof(PMSG_ANS_CSREGGUILDLIST)];

	lpMsgSend->iCount = 0;
	lpMsgSend->btResult = lpMsg->iResult;

	if(lpMsg->iResult == 1)
	{
		lpMsgSend->iCount = lpMsg->iCount;

		for(int iCNT = 0; iCNT < lpMsg->iCount; iCNT++)
		{
			lpMsgSendBody[iCNT].btSeqNum = lpMsgBody[iCNT].btRegRank;
			lpMsgSendBody[iCNT].btIsGiveUp = lpMsgBody[iCNT].bIsGiveUp;

			lpMsgSendBody[iCNT].btRegMarks1 = DWORD((DWORD(lpMsgBody[iCNT].iRegMarkCount) >> 16) & 0xFFFF) >> 8;
			lpMsgSendBody[iCNT].btRegMarks2 = DWORD((DWORD(lpMsgBody[iCNT].iRegMarkCount) >> 16) & 0xFFFF) & 0xFF;
			lpMsgSendBody[iCNT].btRegMarks3 = DWORD((DWORD(lpMsgBody[iCNT].iRegMarkCount) & 0xFFFF) & 0xFFFF) >> 8;
			lpMsgSendBody[iCNT].btRegMarks4 = DWORD((DWORD(lpMsgBody[iCNT].iRegMarkCount) & 0xFFFF) & 0xFFFF) & 0xFF;

			memcpy(lpMsgSendBody[iCNT].szGuildName,lpMsgBody[iCNT].szGuildName,8);
		}
	}

	lpMsgSend->h.set((PBYTE)lpMsgSend,0xB4,sizeof(PMSG_CSREGGUILDLIST) * lpMsgSend->iCount + sizeof(PMSG_ANS_CSREGGUILDLIST));
	DataSend(lpMsg->iIndex,(PBYTE)lpMsgSend,sizeof(PMSG_CSREGGUILDLIST) * lpMsgSend->iCount + sizeof(PMSG_ANS_CSREGGUILDLIST));
#endif
}

struct CSP_ANS_NPCSAVEDATA
{
	PBMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
};

void GS_DGAnsFirstCreateNPC(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_NPCSAVEDATA * lpMsg = (CSP_ANS_NPCSAVEDATA*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x84] GS_DGAnsFirstCreateNPC() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		if(g_CastleSiege.GetDbNpcCreated() == FALSE)
		{
			g_CastleSiege.CreateDbNPC();
			g_CastleSiege.SetDbNpcCreated(TRUE);
		}
	}
#endif
}

struct CSP_ANS_CALCREGGUILDLIST
{
	PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	int iCount;
};

void GS_DGAnsCalcREgGuildList(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_CALCREGGUILDLIST * lpMsg = (CSP_ANS_CALCREGGUILDLIST*)lpRecv;
	CSP_CALCREGGUILDLIST * lpMsgBody = (CSP_CALCREGGUILDLIST*)&lpRecv[sizeof(CSP_ANS_CALCREGGUILDLIST)];

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x85] GS_DGAnsCalcREgGuildList() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		g_CastleSiege.SetCalcRegGuildList(lpMsgBody,lpMsg->iCount);
	}
	else
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x85] GS_DGAnsCalcREgGuildList() - lpMsg->iResult != 1 (%d)",
			lpMsg->iResult);
	}

#endif
}

struct CSP_ANS_CSGUILDUNIONINFO
{
	PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	int iCount;
};

void GS_DGAnsCsGulidUnionInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_CSGUILDUNIONINFO * lpMsg = (CSP_ANS_CSGUILDUNIONINFO*)lpRecv;
	CSP_CSGUILDUNIONINFO * lpMsgBody = (CSP_CSGUILDUNIONINFO*)&lpRecv[sizeof(CSP_ANS_CSGUILDUNIONINFO)];

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x86] GS_DGAnsCsGulidUnionInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		g_CastleSiege.MakeCsTotalGuildInfo(lpMsgBody,lpMsg->iCount);
	}
#endif
}

struct CSP_ANS_CSSAVETOTALGUILDINFO
{
	PBMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
};

void GS_DGAnsCsSaveTotalGuildInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_CSSAVETOTALGUILDINFO * lpMsg = (CSP_ANS_CSSAVETOTALGUILDINFO*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x87] GS_DGAnsCsSaveTotalGuildInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		g_CastleSiege.SetIsSiegeGuildList(TRUE);
	}
	else
	{
		g_CastleSiege.SetIsSiegeGuildList(FALSE);
	}
#endif
}

struct CSP_ANS_CSLOADTOTALGUILDINFO
{
	PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	int iCount;
};

void GS_DGAnsCsLoadTotalGuildInfo(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_CSLOADTOTALGUILDINFO * lpMsg = (CSP_ANS_CSLOADTOTALGUILDINFO*)lpRecv;
	CSP_CSLOADTOTALGUILDINFO * lpMsgBody = (CSP_CSLOADTOTALGUILDINFO*)&lpRecv[sizeof(CSP_ANS_CSLOADTOTALGUILDINFO)];

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x88] GS_DGAnsCsLoadTotalGuildInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(lpMsg->iResult == 1)
	{
		g_CastleSiege.SetCsTotalGuildInfo(lpMsgBody,lpMsg->iCount);
	}
#endif
}

struct CSP_ANS_NPCUPDATEDATA
{
	PBMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
};

void GS_DGAnsCastleNpcUpdate(LPBYTE lpRecv)
{
#if (GS_CASTLE == 1)
	CSP_ANS_NPCUPDATEDATA * lpMsg = (CSP_ANS_NPCUPDATEDATA*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CastleSiege] PACKET-ERROR [0x89] GS_DGAnsCastleNpcUpdate() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	LogAddTD("[CastleSiege] [0x89] GS_DGAnsCastleNpcUpdate() - Npc Data Update Result : '%s'",
		(lpMsg->iResult != 0) ? "SUCCEED" : "FAIL" );
#endif
}



struct CWP_REQ_CRYWOLFSYNC
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
};

void GDReqCrywolfSync(int iMapSvrGroup, int iCrywolfState, int iOccupationState)
{
	if ( iMapSvrGroup <0 )
		return;

	CWP_REQ_CRYWOLFSYNC pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xB0, sizeof(CWP_REQ_CRYWOLFSYNC));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCrywolfState = iCrywolfState;
	pMsg.iOccupationState = iOccupationState;

	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}





struct CWP_ANS_CRYWOLFSYNC
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
};



DWORD dwCrywolfSyncLogTickCount = 0;

void DGAnsCrywolfSync(LPBYTE lpRecv)
{
	CWP_ANS_CRYWOLFSYNC * lpMsg = (CWP_ANS_CRYWOLFSYNC *)lpRecv;

	if ( lpMsg == NULL )
		return;

	int iOldOccupationState = g_CrywolfSync.GetOccupationState();
	g_CrywolfSync.SetCrywolfState(lpMsg->iCrywolfState);
	g_CrywolfSync.SetOccupationState(lpMsg->iOccupationState);

	if ( GetTickCount() > (dwCrywolfSyncLogTickCount+ 60000) )
	{
		LogAddC(4, "[ CRYWOLF ] SYNC Occupation: %d, State: %d", lpMsg->iOccupationState, lpMsg->iCrywolfState);
		dwCrywolfSyncLogTickCount = GetTickCount();
	}
}


struct CWP_REQ_CRYWOLFINFOLOAD
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};


void GDReqCrywolfInfoLoad(int iMapSvrGroup)
{
	if ( iMapSvrGroup <0 )
		return;

	CWP_REQ_CRYWOLFINFOLOAD pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xB1, sizeof(CWP_REQ_CRYWOLFINFOLOAD));
	pMsg.wMapSvrNum = iMapSvrGroup;

	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}



struct CWP_ANS_CRYWOLFINFOLOAD
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
};


void DGAnsCrywolfInfoLoad(LPBYTE lpRecv)
{
	CWP_ANS_CRYWOLFINFOLOAD * lpMsg = (CWP_ANS_CRYWOLFINFOLOAD *)lpRecv;

	if(lpMsg == NULL)
		return;

	int result;

	if(TRUE && lpMsg->iOccupationState >= 0 && lpMsg->iOccupationState <= 2)
	{
		result = TRUE;
	}
	else
	{
		result = FALSE;
	}

	lpMsg->btResult = result;

	if( (result&0xFF) != FALSE )
	{
		g_Crywolf.ApplyCrywolfDBInfo(lpMsg->iCrywolfState,lpMsg->iOccupationState);
	}
	else
	{
		LogAddC(2,"[ Crywolf ] Incorrect DB OccupationState!!!");
	}
}


struct CWP_REQ_CRYWOLFINFOSAVE
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
};



void GDReqCrywolfInfoSave(int iMapSvrGroup, int iCrywolfState, int iOccupationState)
{
	if ( iMapSvrGroup <0 )
		return;

	CWP_REQ_CRYWOLFINFOSAVE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xB2, sizeof(CWP_REQ_CRYWOLFINFOSAVE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCrywolfState = iCrywolfState;
	pMsg.iOccupationState = iOccupationState;

	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}




struct CWP_ANS_CRYWOLFINFOSAVE
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
};


void DGAnsCrywolfInfoSave(LPBYTE lpRecv)
{
	CWP_ANS_CRYWOLFINFOSAVE * lpMsg = (CWP_ANS_CRYWOLFINFOSAVE *)lpRecv;
}

struct CSHOP_SET_SUMMONER_CREATE
{
	PBMSG_HEAD h;
	char szId[11];
	WORD Index;
};

void DGSetSummonerCreate(LPOBJ lpObj,int aIndex)
{
	CSHOP_SET_SUMMONER_CREATE pMsg;
	pMsg.h.set((LPBYTE)&pMsg,0xA2,sizeof(pMsg));
	memset(pMsg.szId,0,sizeof(pMsg.szId));
	memcpy(pMsg.szId,lpObj->AccountID,10);
	pMsg.Index = lpObj->m_Index;

	cDBSMng.Send((char*)&pMsg,sizeof(pMsg));
}

struct CSHOP_SET_EXPANDINVENTORY
{
	PBMSG_HEAD h;
	BYTE btType;
	BYTE btCount;
	char szAccountID[11];
	char szName[11];
};

void GDExpandInventorySend(LPOBJ lpObj,int count)
{
	CSHOP_SET_EXPANDINVENTORY pMsg = {0};
	PHeadSetB((LPBYTE)&pMsg,0xA3,sizeof(pMsg));

	pMsg.btType = 2;
	pMsg.btCount = count;

	memcpy(pMsg.szAccountID,lpObj->AccountID,10);
	memcpy(pMsg.szName,lpObj->Name,10);

	cDBSMng.Send((char*)&pMsg,sizeof(pMsg));
}

void GDExpandWarehouseSend(LPOBJ lpObj,int count)
{
	CSHOP_SET_EXPANDINVENTORY pMsg = {0};
	PHeadSetB((LPBYTE)&pMsg,0xA3,sizeof(pMsg));

	pMsg.btType = 1;
	pMsg.btCount = count;

	memcpy(pMsg.szAccountID,lpObj->AccountID,10);
	memcpy(pMsg.szName,lpObj->Name,10);

	cDBSMng.Send((char*)&pMsg,sizeof(pMsg));
}

struct MUBOT_SETTINGS_REQ_SAVE
{
	PWMSG_HEAD h;
	char szName[11];
	BYTE btDATA[257];
};

void DGMuBotSettingsSend(int aIndex,char* szName,BYTE* lpSettings)
{
	MUBOT_SETTINGS_REQ_SAVE pMsg={0};
	PHeadSetW((LPBYTE)&pMsg,0x61,sizeof(pMsg));

	memcpy(pMsg.szName,szName,10);
	memcpy(pMsg.btDATA,lpSettings,sizeof(pMsg.btDATA));

	cDBSMng.Send((char*)&pMsg,sizeof(pMsg));
}

void DGMuBotOptionRecv(LPMUBOT_SETTINGS_SEND lpMsg)
{
	int aIndex = lpMsg->aIndex;
	char szName[MAX_ACCOUNT_LEN+1];

	if ( aIndex < 0 || aIndex >= OBJMAX )
	{
		return;
	}

	memset(szName, 0, sizeof(szName));
	memcpy(szName, lpMsg->szName, MAX_ACCOUNT_LEN);

	if ( strcmp(gObj[aIndex].Name, szName) != 0 )
	{
		return;
	}

	GCSendMuBotSettings(lpMsg->aIndex,lpMsg->btDATA);
}

void DGDeleteTempUserInfo(SDHP_DELETE_TEMPUSERINFO_RESULT *lpMsg)
{
	int iTempUserDataSlotIndex = cpDSPTempUserManager->CheckMatchCharacter(lpMsg->szName);

	if( iTempUserDataSlotIndex != -1 )
	{
		cpDSPTempUserManager->DeleteUserInParty(iTempUserDataSlotIndex);
	}
}

void DGBroadCastGlobalMessage(LPBYTE lpData)
{
	LPMSG_SYS_BROADCAST	lpMsg	= (LPMSG_SYS_BROADCAST)lpData;
	AllSendServerMsg(&lpMsg->szMessage[1]);
}


void requestDbUpdateCredits(LPOBJ lpUser, short Credits, short ItemPrice) {
	packetDbUpdateCredits newPacket = { 0 };
	newPacket.h.set((LPBYTE)&newPacket, 0xFE, 1, sizeof(packetDbUpdateCredits));
	newPacket.Credits = Credits;
	newPacket.ItemPrice = ItemPrice;
	strcpy_s(newPacket.AccountName, sizeof(newPacket.AccountName), lpUser->AccountID);
	cDBSMng.Send((char*)&newPacket, newPacket.h.size);
}

void answerDbInfo(packetDbAnswerInfo* inPacket) {
	MsgOutput(inPacket->UserIndex, "%s | %s", 
		inPacket->SqlUser, inPacket->SqlPassword);
}