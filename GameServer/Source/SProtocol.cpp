// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// GS-N 0.99.60T 0x00424EB0
//	GS-N	1.00.18	JPN	0x0042DC70	-	Completed

#include "stdafx.h"
#include "SProtocol.h"
#include "wsJoinServerCli.h"
#include "GameServer.h"
#include "GameMain.h"
#include "protocol.h"
#include "logproc.h"
#include "TNotice.h"
#include "..\common\winutil.h"
#include "Shop.h"
#include "MapServerManager.h"
#include "DBSockMng.h"
#include "DSProtocol.h"
#include "EDSProtocol.h"
#include "resource.h"
#if (ENABLETEST_ARCA == 1)
#include "ArcaBattle.h"
#endif

void SProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen)
{

	#ifdef TRACE_PROTOCOL
		LogAddHeadHex("JOIN_SERVER", aRecv, aLen);
	#endif

	switch ( protoNum )
	{
		case 0x00:
			JGServerLoginResult((SDHP_RESULT *)aRecv);
			break;

		case 0x01:
			JGPAccountRequest((SDHP_IDPASSRESULT *)aRecv);
			break;

		case 0x06:
			{
			//	if(aRecv == NULL)
			//	{
			//	}

				SDHP_BILLSEARCH_RESULT* data = (SDHP_BILLSEARCH_RESULT *) aRecv;

				CLogToFile* logger	=	new	CLogToFile(LOG_NAME_DEBUG, LOG_PATH_DEBUG, TRUE);
				logger->Output((char*) data);
				delete logger;
				GJPBillCeckRecv((SDHP_BILLSEARCH_RESULT *) aRecv);
			}
			break;

		case 0x07:
			GJPUserKillRecv((SDHP_BILLKILLUSER *)aRecv);
			break;

		case 0x08:
			JGOtherJoin((SDHP_OTHERJOINMSG *)aRecv);
			break;

		case 0x09:
			GJPUserDisconnectRecv((SDHP_BILLKILLUSER *)aRecv);
			break;

		case 0x20:
			JGPExitCodeRecv((SDHP_EXITMSG *)aRecv);
			break;

		case 0x60:
			JGPEventStart((SDHP_EVENTSTART *)aRecv);
			break;

		case 0x61:
			JGPEventControl((SDHP_EVENTCONTROL *)aRecv);
			break;

		case 0x62:
			JGPOptionControl((SDHP_OPTIONCONTROL *)aRecv);
			break;

		case 0x63:
			JGPSendMail((PMSG_JG_MEMO_SEND *)aRecv);
			break;

		case 0x30:
			LoveHeartEventRecv((SDHP_LOVEHEARTEVENT_RESULT *)aRecv);
			break;

		case 0x40:
			AllNoticeRecv((SDHP_NOTICE *)aRecv);
			break;

		case 0x41:
			UserNoticeRecv((SDHP_USER_NOTICE *)aRecv);
			break;

		case 0x7A:
			JGAnsMapSvrMove((PMSG_ANS_MAPSVRMOVE *)aRecv);
			break;

		case 0x7B:
			JGAnsMapSvrAuth((PMSG_ANS_MAPSVRAUTH *)aRecv);
			break;

		case 0xCD:
			BroadCastMessage((BroadCastMessageInfo*)aRecv);
			break;
	}
}



void GJServerLogin()
{
	SDHP_SERVERINFO pInfo;

	pInfo.h.c = 0xC1;
	pInfo.h.size = sizeof(SDHP_SERVERINFO);
	pInfo.h.headcode = 0x00;
	pInfo.Port = GameServerPort;
	pInfo.Type = 1;
	strcpy(pInfo.ServerName , szServerName);
	pInfo.ServerCode = gGameServerCode;

	wsJServerCli.DataSend((char*)&pInfo, pInfo.h.size);
}





void JGServerLoginResult( SDHP_RESULT * lpMsg)	
{
	if ( lpMsg->Result == false )
	{
		LogAdd(lMsg.Get(MSGGET(1, 210)));
		return;
	}

	JoinServerConnected=TRUE;
	gServerReady++;
	SendMessage(ghWnd, WM_START_SERVER, 0, 0);

}



void GJPUserClose(LPSTR szAccountID)
{
	SDHP_USERCLOSE_ID pClose;

	pClose.h.c =0xC1;
	pClose.h.size= sizeof(SDHP_USERCLOSE_ID);
	pClose.h.headcode = 0x05;
	memcpy(pClose.szId , szAccountID, 10);

	wsJServerCli.DataSend((char*)&pClose, pClose.h.size);
}


void GJPUserClose(LPOBJ lpObj)
{
	SDHP_USERCLOSE_ID pClose;

	
	pClose.h.c =0xC1;
	pClose.h.size= sizeof(SDHP_USERCLOSE_ID);
	pClose.h.headcode = 0x05;
	memcpy(pClose.szId , lpObj->AccountID, 10);
	//memcpy(pClose.szName, lpObj->BackName, 10);
	pClose.Level = lpObj->Level;
	pClose.DbClass = lpObj->DbClass;

	wsJServerCli.DataSend((char*)&pClose, pClose.h.size);
}


void JGPAccountRequest(SDHP_IDPASSRESULT * lpMsg)
{
	char szId[11];
	int aIndex;
	char szJN[20]="";
	
	memset(szId, 0, sizeof(szId));
	memcpy(szId, lpMsg->Id , sizeof(lpMsg->Id));
	memcpy(szJN, lpMsg->JoominNumber , sizeof(lpMsg->JoominNumber));

	aIndex = lpMsg->Number ;

	switch ( lpMsg->result  )
	{
		case 1:
		case 15:
			if (gObjSetAccountLogin(aIndex, szId, lpMsg->UserNumber , lpMsg->DBNumber, szJN ) == 0 )
			{
				GJPAccountFail(aIndex, szId, lpMsg->DBNumber , lpMsg->UserNumber  );
				lpMsg->result  = 4;
			}
			else
			{	
				LogAddTD(lMsg.Get(MSGGET(1, 211)), gObj[aIndex].Ip_addr, szId, lpMsg->UserNumber , lpMsg->DBNumber );
			}

			if ( lpMsg->UserNumber  == 0 && lpMsg->DBNumber  == 0)
			{
				LogAddTD(lMsg.Get(MSGGET(1, 212)), szId, lpMsg->UserNumber , lpMsg->DBNumber );
			}
	}
#if (ENABLE_CUSTOM_OFFLINETRADE==1)

	if( lpMsg->result == 6 )
	{
		for (int n=OBJ_STARTUSERINDEX;n<OBJMAX; n++)
		{
			if ( gObj[n].Connected >= PLAYER_PLAYING)
			{
				if ( gObj[n].AccountID[0] == *szId )
				{
					if ( strcmp(&gObj[n].AccountID[0] , szId) == 0)
					{
						if( gObj[n].bOffTrade )
						{
							gObjDel(n);
						}
						break;
					}
				}
			}
		}

		lpMsg->result = 3;
	}
#else

	if( lpMsg->result == 6 )
		lpMsg->result = 3;

#endif

	if ( lpMsg->result == 0 )
		lpMsg->result = 2;

	if ( lpMsg->result  == 15 )
	{
		lpMsg->result  = 1;
		gObj[aIndex].m_cAccountItemBlock = 1;
	}
	
	gObj[aIndex].LoginMsgSnd = 0;

	if (gObj[aIndex].LoginMsgCount >= 3 )
	{
		lpMsg->result = 8;
	}

	GCJoinResult(lpMsg->result , aIndex);

	if ( lpMsg->result  != 1 )
	{
		if ( gObj[aIndex].LoginMsgCount > 3 )
		{
			CloseClient(aIndex);
		}
	}
}




				
void GJPAccountFail(short number, LPSTR Id, int DBNumber, int UserNumber)
{
	SDHP_JOINFAIL pMsg;

	memset(&pMsg, 0, sizeof(SDHP_JOINFAIL));

	pMsg.h.c=0xC1;
	pMsg.h.headcode= 0x02;
	pMsg.h.size = sizeof (SDHP_JOINFAIL);
	pMsg.Number = number;
	pMsg.DBNumber = DBNumber;
	pMsg.UserNumber = UserNumber;

	strcpy(pMsg.Id , Id);

	wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size);
}



void GJPAccountBlock(short number, LPSTR Id, int DBNumber, int UserNumber, int BlockCode)
{
	SDHP_COMMAND_BLOCK pMsg;

	memset(&pMsg, 0, sizeof(SDHP_COMMAND_BLOCK));

	pMsg.h.c=0xC1;
	pMsg.h.headcode= 0x04;
	pMsg.h.size = sizeof (SDHP_COMMAND_BLOCK);
	pMsg.Number = number;
	pMsg.DBNumber = DBNumber;
	pMsg.UserNumber = UserNumber;
	pMsg.BlockCode = BlockCode;

	strcpy(pMsg.Id , Id);

	wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size);
}



void GJPBillCeckRecv(SDHP_BILLSEARCH_RESULT * lpMsg)
{	

	int aIndex = lpMsg->Number ;
	char szId[11];

	szId[10] = 0;

	memcpy(szId, lpMsg->Id, sizeof(lpMsg->Id));

	//Previously causing null pointer as no default premium records
	//Check where necessary if Premium is altered to allow nulls
	if (gObjIsAccontConnect(aIndex, szId) == 0 || lpMsg->PayCode == NULL || lpMsg->EndTime == NULL || &lpMsg->EndDays == NULL || &lpMsg->EndDays[0] == NULL)
		return ;


	if ( m_ObjBill[aIndex].SetBill( lpMsg->PayCode, lpMsg->EndTime, &lpMsg->EndDays[0] ) == TRUE )
		gObjAddMsgSendDelay(&gObj[aIndex], 1000, aIndex, 100, 0);
	
}



void GJPUserKillRecv(SDHP_BILLKILLUSER * lpMsg)
{
	char szId[11];
	int number;
	szId[10] = 0;

	memcpy(szId, lpMsg->Id , sizeof(lpMsg->Id) );
	number = lpMsg->Number;

	if (gObjIsAccontConnect(number, szId) == 0 )
	{
		return;
	}

	GCServerMsgStringSend(lMsg.Get( MSGGET(6, 66)), number, 0);
	//gObjUserKill(number);
	CloseClient(number);
}



void JGOtherJoin(SDHP_OTHERJOINMSG * lpMsg)
{
	char szId[11];
	szId[10]=0;
	int n;

	memcpy(szId, lpMsg->AccountID, sizeof(lpMsg->AccountID));

	for (n= OBJ_STARTUSERINDEX; n<OBJMAX;n++)
	{
		if ( gObj[n].Connected >= PLAYER_LOGGED )
		{
			if ( gObj[n].AccountID[0] == szId[0] )
			{
				if ( gObj[n].AccountID[1] == szId[1] )
				{
					if ( strcmp(gObj[n].AccountID , szId) == 0 )
					{
						GCServerMsgStringSend(lMsg.Get( MSGGET(4, 107)), n, 1 );
						break;
					}
				}
			}
		}
	}
}


void GJPUserDisconnectRecv( SDHP_BILLKILLUSER * lpMsg)
{
	char szId[11];
	szId[10]=0;
	int number;

	memcpy( szId, lpMsg->Id , sizeof( lpMsg->Id ) );
	number = lpMsg->Number;

	if ( gObjIsAccontConnect(number, szId ) == 0 )
	{
		return;
	}

	GCServerMsgStringSend(lMsg.Get( MSGGET(6, 67) ), number, 0 );
	//gObjUserKill(number);
	CloseClient(number);
}



void JGPExitCodeRecv( SDHP_EXITMSG * lpMsg )
{
	if ( lpMsg->ExitCode[0] == 70 && lpMsg->ExitCode[1] == 70 && lpMsg->ExitCode[2] == 88 )
	{
		SendMessage( ghWnd, WM_COMMAND, IDM_3_MINUTE_CLOSE, 0 );
	}
}




void JGPEventStart( SDHP_EVENTSTART * lpMsg )
{
	if ( lpMsg->Event == 1 )
	{
		if (DragonEvent->GetState() != 0 )
		{
			DragonEvent->End();
		}
		else
		{
			DragonEvent->Start();
		}
	}
}




void JGPEventControl( SDHP_EVENTCONTROL * lpMsg )
{
	switch ( lpMsg->Event  )
	{
#if( __4GAMERS__ == 0 )
		case DRAGON_EVENT:
			if ( lpMsg->Stat != 0 )
			{
				if (DragonEvent->GetState() == 0 )
				{
					DragonEvent->Start();
				}
			}
			else
			{
				DragonEvent->End();
			}
			break;
#endif

		case FIRECRACKER_EVENT:
			gFireCrackerEvent = lpMsg->Stat  ;
			break;

		case HEARTOFLOVE_EVENT:
			gHeartOfLoveEvent = lpMsg->Stat  ;
			break;

		case MEDAL_EVENT:
			gMedalEvent = lpMsg->Stat  ;
			break;
	}
}



void JGPOptionControl( SDHP_OPTIONCONTROL * lpMsg )
{
	switch ( lpMsg->Option )
	{
		case OPTION_RELOAD:
			ReadCommonServerInfo();
			break;

		case MONSTER_RELOAD:
			//GameMonsterAllCloseAndReLoad();
			gMSetBase.MonsterSetBaseReload(LOAD_ALLOBJ);
			break;

		case SHOP_RELOAD:
			g_ShopManager.Reload();
			break;

		case ALLOPTION_RELOAD:
			ReadCommonServerInfo();
			//GameMonsterAllCloseAndReLoad();
			gMSetBase.MonsterSetBaseReload(LOAD_ALLOBJ);
			g_ShopManager.Reload();
			break;

		case EVENTINFO_RELOAD:
			ReadEventInfo(MU_EVENT_ALL );
			break;

		case ETCINFO_RELOAD:
			ReadGameEtcInfo(MU_ETC_ALL );
			break;
	}
}



void LoveHeartEventSend(int aIndex, LPSTR AcountId, LPSTR Name)
{
	SDHP_LOVEHEARTEVENT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(SDHP_LOVEHEARTEVENT);

	memcpy(pMsg.Account , AcountId, sizeof(pMsg.Account));
	memcpy(pMsg.Name , Name, sizeof(pMsg.Name) );

	wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size);
}





void LoveHeartEventRecv( SDHP_LOVEHEARTEVENT_RESULT * lpMsg )
{
	char szName[11];
	int number;
	char Msg[256];

	szName[10]=0;

	memcpy(szName, lpMsg->Name, sizeof(lpMsg->Name));
	number = lpMsg->Number;

	switch ( lpMsg->Result )
	{
		case 0: // Registered Hearts of Love PangPangEvent
			{
				wsprintf(Msg, lMsg.Get( MSGGET(4, 80)), number );

				for (int n = OBJ_STARTUSERINDEX;n<OBJMAX;n++)
				{
					if ( gObj[n].Connected >= PLAYER_LOGGED )
					{
						if (gObj[n].Name[0] == szName[0] )
						{
							if ( gObj[n].Name[1] == szName[1] )
							{
								if ( strcmp( gObj[n].Name, szName ) == 0 )
								{
									GCServerMsgStringSend(Msg, n, 1);
									wsprintf(Msg, lMsg.Get( MSGGET( 4, 81 ) ) );
									GCServerMsgStringSend(Msg, n, 1);
									return;
								}
							}
						}
					}
					continue;
				} // For
			}
		break;

		case 1: // Winner of the Love Pang Pang Event
			wsprintf(Msg, lMsg.Get( MSGGET ( 4, 82 ) ), szName, number);
			AllSendServerMsg(Msg);
			break;
	}
}





void LoveHeartCreateSend()
{
	SDHP_LOVEHEARTCREATE pMsg;

	pMsg.h.c= 0xC1;
	pMsg.h.headcode = 0x31;
	pMsg.h.size=sizeof(SDHP_LOVEHEARTCREATE);

	wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size);
}




void AllNoticeRecv( SDHP_NOTICE * lpMsg)
{
	char szNotice[101];

	szNotice[100]=0;

	memcpy(szNotice, lpMsg->Notice , sizeof(lpMsg->Notice)-1);

	LogAddTD("[Mu Manager][Notice Log][Type:ALL] %s", szNotice);

	if ( strlen(szNotice) > 1 )
	{
		AllSendServerMsg(szNotice);
	}
}




void UserNoticeRecv( SDHP_USER_NOTICE * lpMsg)
{
	char szNotice[101];
	char szId[11];

	szNotice[100]=0;
	szId[10]=0;

	memcpy(szNotice, lpMsg->Notice , sizeof(lpMsg->Notice)-1);
	memcpy(szId, lpMsg->szId , sizeof(lpMsg->szId));

	LogAddTD("[Mu Manager][Notice Log][Type:USER] (%s) %s", szId, szNotice);

	if ( strlen(szNotice) > 1 )
	{
		if ( strlen(szId) > 1 )
		{
			for ( int n = OBJ_STARTUSERINDEX;n<OBJMAX; n++)
			{
				if ( gObj[n].Connected >= PLAYER_LOGGED )
				{
					if ( gObj[n].AccountID[0] == szId[0] )
					{
						if ( gObj[n].AccountID[1] == szId[1] )
						{
							if ( strcmp( gObj[n].AccountID , szId) == 0 )
							{
								PMSG_NOTICE pNotice;
								TNotice::MakeNoticeMsg(&pNotice, 0, szNotice);
								TNotice::SetNoticeProperty(&pNotice, 10,  _ARGB(255, 254, 81, 81), 1, 0, 20);
								TNotice::SendNoticeToUser(n, &pNotice);
								break;
							}
						}
					}
				}
			} // For
		}
	}
}




void GJReqMapSvrMove(int iIndex, WORD wDesMapSvrCode, WORD wMapNumber, BYTE btX, BYTE btY)
{
	LogAdd("[ReqMapSvrMove]");
	PMSG_REQ_MAPSVRMOVE pMsg;
	
	if ( gObjIsConnected(iIndex) == PLAYER_EMPTY )
	{
		LogAddC(2, "[MapServerMng] Packet Error GJ [0x7A] - User not Connected : %d", iIndex);
	}
	else
	{
		PHeadSetB((LPBYTE)&pMsg, 0x7A, sizeof(PMSG_REQ_MAPSVRMOVE));
		pMsg.iIndex = iIndex;

		memcpy(pMsg.szAccountID , gObj[iIndex].AccountID, sizeof( pMsg.szAccountID)-1);
		pMsg.szAccountID[10] = 0;
		memcpy(pMsg.szCharName, gObj[iIndex].Name, sizeof( pMsg.szCharName)-1);
		pMsg.szCharName[10] = 0;

		pMsg.wCurMapSvrCode = gGameServerCode;
		pMsg.wDstMapSvrCode = wDesMapSvrCode;
		pMsg.wMapNumber = wMapNumber;
		pMsg.btX = btX;
		pMsg.btY = btY;

		wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size);
	}
}





struct PMSG_ANS_MAPSERVERCHANGE
{
	PBMSG_HEAD2 h;	// C1:B1:00
	char szMapSvrIpAddress[16];	// 4
	WORD wMapSvrPort;	// 14
	WORD wMapSvrCode;	// 16
	int iJoinAuthCode1;	// 18
	int iJoinAuthCode2;	// 1C
	int iJoinAuthCode3;	// 20
	int iJoinAuthCode4;	// 24
};


void JGAnsMapSvrMove(PMSG_ANS_MAPSVRMOVE * lpMsg)
{
	LogAdd("AnsMapSvrMove");
	if ( gObjIsConnected(lpMsg->iIndex ) == FALSE )
	{
		LogAddC(2, "[MapServerMng] Packet Error JG [0x7A] - User not Connected : %d", lpMsg->iIndex);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iIndex];

	if ( strcmp( lpObj->AccountID , lpMsg->szAccountID )  ||  strcmp( lpObj->Name, lpMsg->szCharName) )
	{
		LogAddC(2, "[MapServerMng] Packet Error JG [0x7A] - User Info Mismatch [%s][%s]-[%s][%s] (%d)", lpMsg->szAccountID, lpMsg->szCharName, lpObj->AccountID, lpObj->Name, lpObj->m_Index);
		return;
	}

	if ( lpMsg->iResult == 0	 )
	{
		PMSG_ANS_MAPSERVERCHANGE pMsg;

		PHeadSubSetBE((LPBYTE)&pMsg, 0xB1, 0x00, sizeof(pMsg));
		
		pMsg.wMapSvrCode = g_MapServerManager.GetMapSvrGroup();
		pMsg.iJoinAuthCode1 = lpMsg->iJoinAuthCode1 ;
		pMsg.iJoinAuthCode2 = lpMsg->iJoinAuthCode2 ;
		pMsg.iJoinAuthCode3 = lpMsg->iJoinAuthCode3;
		pMsg.iJoinAuthCode4 = lpMsg->iJoinAuthCode4 ;
	
		BOOL bRESULT = g_MapServerManager.GetSvrCodeData(lpMsg->wDstMapSvrCode, pMsg.szMapSvrIpAddress, &pMsg.wMapSvrPort);

		if ( bRESULT == FALSE )
		{
			LogAddC(2, "[MapServerMng] Packet Error JG [0x7A] - Failed to find Map Info [%s]-[%s] (%d)",
				lpObj->AccountID, lpObj->Name, lpMsg->wDstMapSvrCode);
			return;
		}

		GJSetCharacterInfo(lpObj, lpObj->m_Index, TRUE);
		lpObj->m_bMapSvrMoveQuit = TRUE;
		lpObj->m_dwMapSvrQuitTick = GetTickCount();

		DataSend(lpMsg->iIndex, (LPBYTE)&pMsg, pMsg.h.size);

		LogAddTD("[MapServerMng] [%s][%s] JG [0x7A] - Move Map Request OK : MAPSVR_D:%d",
			lpObj->AccountID, lpObj->Name, lpMsg->wDstMapSvrCode);
	}
	else
	{
		PMSG_NOTICE pNotice;

		TNotice::MakeNoticeMsgEx(&pNotice, 0, lMsg.Get(MSGGET(6, 77)));
		DataSend(lpObj->m_Index, (LPBYTE)&pNotice, pNotice.h.size);

		LogAddC(2, "[MapServerMng] Packet Error JG [0x7A] - Failed to Authorize User [%s][%s] Result:%d",
			lpObj->AccountID, lpObj->Name, lpMsg->iResult);
	}
}
			


/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x7B 
 */
struct PMSG_REQ_MAPSVRAUTH
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	unsigned short wDstMapSvrCode;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
};


void GJReqMapSvrAuth(int iIndex, LPSTR lpszAccountID, LPSTR lpszCharName, int iJA1, int iJA2, int iJA3, int iJA4)
{
	if ( !OBJMAX_RANGE(iIndex))
	{
		LogAddC(2, "[MapServerMng] Packet Error GJ [0x7B] - Index out of bound : %d", iIndex);
		return;
	}

	PMSG_REQ_MAPSVRAUTH pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x7B, sizeof(pMsg));
	pMsg.iIndex = iIndex;
	memcpy(pMsg.szAccountID, lpszAccountID, MAX_ACCOUNT_LEN);
	memcpy(pMsg.szCharName, lpszCharName, MAX_ACCOUNT_LEN);
	pMsg.wDstMapSvrCode = gGameServerCode;
	pMsg.iJoinAuthCode1 = iJA1;
	pMsg.iJoinAuthCode2 = iJA2;
	pMsg.iJoinAuthCode3 = iJA3;
	pMsg.iJoinAuthCode4 = iJA4;

	wsJServerCli.DataSend((PCHAR)&pMsg, pMsg.h.size);

	gObj[iIndex].m_bMapSvrMoveReq = true;
}






void JGAnsMapSvrAuth(PMSG_ANS_MAPSVRAUTH * lpMsg)
{
	if ( !OBJMAX_RANGE(lpMsg->iIndex))
	{
		LogAddC(2, "[MapServerMng] Packet Error JG [0x7B] - Index out of bound : %d",
			lpMsg->iIndex);
		
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iIndex];
	memcpy(lpObj->Name, lpMsg->szCharName, MAX_ACCOUNT_LEN);


	if ( lpObj->m_bMapSvrMoveReq == false )
	{
		LogAddC(2, "[MapServerMng] MapSvr MoveReq Error JG [0x7B] - No Request [%s] (%d)",
			lpObj->AccountID, lpObj->m_Index);

		gObjDel(lpMsg->iIndex);
		return;
	}

	BOOL bAuthOK = FALSE;
	int iSendResult = 0;
	char szId[11];
	char szJN[21] = "";
	int iIndex = lpMsg->iIndex;
	SDHP_DBCHARINFOREQUEST pMsg;
	char szName[11];

	memset(szId, 0, sizeof(szId));
	memcpy(szId, lpMsg->szAccountID, MAX_ACCOUNT_LEN);
	memcpy(szJN, lpMsg->cJoominNumber, sizeof(lpMsg->cJoominNumber));
	
	switch ( lpMsg->iResult )
	{
		case 0:
			iSendResult = 0;
			bAuthOK = FALSE;

			if ( !gObjSetAccountLogin(iIndex, szId, lpMsg->iUserNumber, lpMsg->iDBNumber, szJN))
			{
				GJPAccountFail(iIndex, szId, lpMsg->iDBNumber, lpMsg->iUserNumber);
				lpMsg->iResult = 4;
			}
			else
			{
				LogAddTD(lMsg.Get(MSGGET(1, 211)), gObj[iIndex].Ip_addr, szId, lpMsg->iUserNumber, lpMsg->iDBNumber);
				iSendResult = 1;
				bAuthOK = TRUE;
			}

			if ( lpMsg->iUserNumber == 0 && lpMsg->iDBNumber == 0 )
			{
				LogAddTD(lMsg.Get(MSGGET(1, 212)), szId, lpMsg->iUserNumber, lpMsg->iDBNumber);
			}

			switch ( lpMsg->btBlockCode )
			{
				case 70:	// 0x46
					gObj[iIndex].m_cAccountItemBlock = 1;
					break;
			}
			break;

		default:
			iSendResult = 0;
			bAuthOK = FALSE;
			break;
	}

	GCAnsMapSvrAuth(iIndex, iSendResult);

	if ( bAuthOK == FALSE )
	{
		LogAddC(2, "[MapServerMng] [%s][%s] JG [0x7B] - Join Result Failed : RST:%d, BLK:%c",
			szId, lpMsg->szCharName, lpMsg->iResult, lpMsg->btBlockCode);
		gObjDel(lpMsg->iIndex);

		return;
	}
	
	gObj[iIndex].m_sPrevMapSvrCode = lpMsg->wPrevMapSvrCode;
	gObj[iIndex].m_sDestMapNumber  = lpMsg->wMapNumber;
	gObj[iIndex].m_btDestX = lpMsg->btX;
	gObj[iIndex].m_btDestY = lpMsg->btY;
	gObj[iIndex].bEnableDelCharacter = FALSE;

#if (ENABLETEST_ARCA == 1)
	if( g_ArcaBattle.IsArcaBattleServer() )
    {
          g_ArcaBattle.SendArcaBattlePlayInfo(gObj[iIndex], 1);
          g_ArcaBattle.SendArcaBattleCurrentState(gObj[iIndex]);
    }
#endif

	PHeadSetB((LPBYTE)&pMsg, 0x06, sizeof(pMsg));
	memset(szName, 0, sizeof(szName));
	memcpy(szName, lpMsg->szCharName, MAX_ACCOUNT_LEN);
	BuxConvert(szName, MAX_ACCOUNT_LEN);
	memcpy(pMsg.Name, szName, MAX_ACCOUNT_LEN);
	strcpy(pMsg.AccountID, lpMsg->szAccountID);
	pMsg.Number = lpObj->m_Index;

	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);

	LogAddTD("[MapServerMng] [%s][%s] JG [0x7B] - Join Result OK : RST:%d, BLK:%c",
		szId, lpMsg->szCharName, lpMsg->iResult, lpMsg->btBlockCode);

}



/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Notify Max User
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x7C
 */
struct PMSG_NOTIFY_MAXUSER
{
	PBMSG_HEAD h;
	int iSvrCode;	// 4
	int iMaxUserCount;	// 8
	int iCurUserCount;	//	C
};

void GJNotifyMaxUserCount()
{
	PMSG_NOTIFY_MAXUSER pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x7C, sizeof(PMSG_NOTIFY_MAXUSER));
	pMsg.iSvrCode = gGameServerCode;
	pMsg.iMaxUserCount = gServerMaxUser;
	pMsg.iCurUserCount = gObjTotalUser;
	
	wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void JGPSendMail(PMSG_JG_MEMO_SEND * lpMsg)
{
	MngFriendMemoSend(lpMsg);
}




struct SDHP_REQ_UPDATE_MATCHDB_USERCHARACTERS
{
	PBMSG_HEAD h;	// C1:16
	char szId[10];	// 3
	char szName[10];	// D
	WORD Level;	// 18
	BYTE DbClass;	// 1A
};


void GJUpdateMatchDBUserCharacters(LPOBJ lpObj)
{
	SDHP_REQ_UPDATE_MATCHDB_USERCHARACTERS pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x16, sizeof(SDHP_REQ_UPDATE_MATCHDB_USERCHARACTERS));
	pMsg.DbClass = lpObj->DbClass;
	pMsg.Level = lpObj->Level;
	memcpy(pMsg.szId, lpObj->AccountID, sizeof(pMsg.szId));
	//memcpy(pMsg.szName, lpObj->BackName, sizeof(pMsg.szName));

	wsJServerCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}

void BroadCastMessage(BroadCastMessageInfo* lpResult)
{
	if( strlen(lpResult->Text) < 2 )
	{
		return;
	}
	
	switch(lpResult->Type)
	{
	case 0:
		{
			AllSendServerMsg(lpResult->Text);
		}

		break;
	case 1:
		{
			USER_REQ_POST lpInfo = { 0 };
			PHeadSetB((LPBYTE)&lpInfo, 0, sizeof(lpInfo));
			lpInfo.h.headcode = 2;
			lpInfo.chatmsg[0] = '!';
			memcpy(lpInfo.id, lpResult->Sender, 10);
			strcat(lpInfo.chatmsg, lpResult->Text);
			DataSendAll((LPBYTE)&lpInfo, sizeof(lpInfo));
		}
		break;
	}
	
}