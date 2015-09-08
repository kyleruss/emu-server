#include "stdafx.h"
#include "zMultiServer.h"
#include "DataServer.h"
#include "Window.h"
#include "winutil.h"
#include "Ranking.h"

CQuery g_RankingDB;
RSInfo g_RSInfo;

void RankingServerInit()
{
g_bIsServer[ST_RANKINGSERVER] = TRUE;

	g_RSInfo.TCPPort = GetPrivateProfileInt(RS_INISECTION,"TCPPort",44455,INI_FILE);
	GetPrivateProfileString(RS_INISECTION,"DSN","Ranking",g_RSInfo.dbDSN,sizeof(g_RSInfo.dbDSN),INI_FILE);

	g_RankingDB.S_TYPE = ST_RANKINGSERVER;

	if( g_RankingDB.Connect(3,g_RSInfo.dbDSN,SQLUser,SQLPass) == TRUE )
	{
		g_Window.LogAdd(Log::Info, "[ %s ] connection successful !",g_RSInfo.dbDSN);
	}

	if(CreateGIocp(g_RSInfo.TCPPort,ST_RANKINGSERVER) != TRUE)
	{
		g_Window.LogAdd(Log::Error, "Failed to start [ ST_RANKINGSERVER ] on port [ %d ]",g_RSInfo.TCPPort);
	}
	else
	{
		g_Window.LogAdd(Log::Info, "[ RankingServer ] created on port [ %d ]",g_RSInfo.TCPPort);
	}
}

void RSProtocolCore(int aIndex, DWORD headcode, LPBYTE aRecv, int Len)
{
//	g_ServerInfoDisplayer.SetState(TRUE,gSObj[aIndex].SType);

	switch( headcode )
	{
	case 0x01:
		DevilSqureScore((LPMSG_ANS_EVENTUSERSCORE)aRecv);
	case 0x06:
		EGReqDevilSquareEnter((LPMSG_REQ_BLOODCASTLE_ENTER)aRecv,aIndex);
		break;
	case 0x07:
		EGDevilSquareEnter((LPMSG_REQ_BLOODCASTLE_ENTERCOUNT)aRecv);
		break;
	case 0x08:
		EGReqBloodCastleEnter((LPMSG_REQ_BLOODCASTLE_ENTER)aRecv,aIndex);
		break;
	case 0x09:
		EGBloodCastleEnter((LPMSG_REQ_BLOODCASTLE_ENTERCOUNT)aRecv);
		break;
	case 0x0B:
		EGReqBloodCastleEnterCount((LPMSG_REQ_BLOODCASTLE_ENTERCOUNT)aRecv,aIndex);
		break;
	case 0x0D:
		BloodCastleScore_5TH((LPMSG_ANS_BLOODCASTLESCORE_5TH)aRecv);
		break;
	}

//	g_ServerInfoDisplayer.SetState(FALSE,gSObj[aIndex].SType);
}

void DevilSqureScore(LPMSG_ANS_EVENTUSERSCORE lpMsg)
{
	char szQuery[256];
	char szAccountID[MAX_IDSTRING+1] = {0};
	char szName[MAX_IDSTRING+1] = {0};

	memcpy(szAccountID,lpMsg->AccountID,MAX_IDSTRING);
	memcpy(szName,lpMsg->GameID,MAX_IDSTRING);

	g_Window.ServerLogAdd(ST_RANKINGSERVER,"[DEVIL] SERVER:%d-%d, SQUARE:%d, ACCNT:%s, CHAR:%s, CLASS:%d, SCORE:%d",
		lpMsg->ServerCode/20+1,lpMsg->ServerCode % 20 + 1,lpMsg->SquareNum,szAccountID,szName,lpMsg->Class,lpMsg->Score);

	sprintf(szQuery,"EXEC SP_POINT_ACCUMULATION '%d', '%d', '%s', '%s', '%d', '%d'",
		lpMsg->ServerCode/20+1,lpMsg->SquareNum,szAccountID,szName,lpMsg->Class,lpMsg->Score);

	if(g_RankingDB.Exec(szQuery) == FALSE)
	{
		g_Window.ServerLogAdd(ST_RANKINGSERVER,"error-L1 : %s", szQuery);
	}
	g_RankingDB.Clear();
}

void EGReqDevilSquareEnter(LPMSG_REQ_BLOODCASTLE_ENTER lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccountID[MAX_IDSTRING+1] = {0};
	char szName[MAX_IDSTRING+1] = {0};

	PMSG_ANS_BLOODCASTLE_ENTER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x06;
	pMsg.h.size = sizeof(pMsg);

	memcpy(szAccountID,lpMsg->AccountID,MAX_IDSTRING);
	memcpy(szName,lpMsg->GameID,MAX_IDSTRING);
	memcpy(pMsg.AccountID,szAccountID,MAX_IDSTRING);
	memcpy(pMsg.GameID,szName,MAX_IDSTRING);

	pMsg.ServerCode = lpMsg->ServerCode;
	pMsg.iObjIndex = lpMsg->iObjIndex;
	pMsg.iBloodCastle = lpMsg->iBloodCastle;
	pMsg.iBloodCastleItemPos = lpMsg->iBloodCastleItemPos;

	pMsg.iResult = 0;

	sprintf(szQuery,"EXEC SP_CHECK_DS '%s', '%s', '%d'",szAccountID,szName,lpMsg->ServerCode);

	if(g_RankingDB.Exec(szQuery) == TRUE && g_RankingDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = g_RankingDB.GetInt("EnterResult");
	}

	g_RankingDB.Clear();
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void EGDevilSquareEnter(LPMSG_REQ_BLOODCASTLE_ENTERCOUNT lpMsg)
{
	char szQuery[256];
	char szAccountID[MAX_IDSTRING+1] = {0};
	char szName[MAX_IDSTRING+1] = {0};

	memcpy(szAccountID,lpMsg->AccountID,MAX_IDSTRING);
	memcpy(szName,lpMsg->GameID,MAX_IDSTRING);

	sprintf(szQuery,"EXEC SP_ENTER_DS '%s', '%s', '%d'",szAccountID,szName,lpMsg->ServerCode);
	g_RankingDB.Exec(szQuery);
}

void EGReqBloodCastleEnter(LPMSG_REQ_BLOODCASTLE_ENTER lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccountID[MAX_IDSTRING+1] = {0};
	char szName[MAX_IDSTRING+1] = {0};

	PMSG_ANS_BLOODCASTLE_ENTER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x08;
	pMsg.h.size = sizeof(pMsg);

	memcpy(szAccountID,lpMsg->AccountID,MAX_IDSTRING);
	memcpy(szName,lpMsg->GameID,MAX_IDSTRING);
	memcpy(pMsg.AccountID,szAccountID,MAX_IDSTRING);
	memcpy(pMsg.GameID,szName,MAX_IDSTRING);

	pMsg.ServerCode = lpMsg->ServerCode;
	pMsg.iObjIndex = lpMsg->iObjIndex;
	pMsg.iBloodCastle = lpMsg->iBloodCastle;
	pMsg.iBloodCastleItemPos = lpMsg->iBloodCastleItemPos;

	pMsg.iResult = 0;

	sprintf(szQuery,"EXEC SP_CHECK_BC '%s', '%s', '%d'",szAccountID,szName,lpMsg->ServerCode);

	if(g_RankingDB.Exec(szQuery) == TRUE && g_RankingDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = g_RankingDB.GetInt("EnterResult");
	}

	g_RankingDB.Clear();
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void EGBloodCastleEnter(LPMSG_REQ_BLOODCASTLE_ENTERCOUNT lpMsg)
{
	char szQuery[256];
	char szAccountID[MAX_IDSTRING+1] = {0};
	char szName[MAX_IDSTRING+1] = {0};

	memcpy(szAccountID,lpMsg->AccountID,MAX_IDSTRING);
	memcpy(szName,lpMsg->GameID,MAX_IDSTRING);

	sprintf(szQuery,"EXEC SP_ENTER_BC '%s', '%s', '%d'",szAccountID,szName,lpMsg->ServerCode);
	g_RankingDB.Exec(szQuery);
}

void EGReqBloodCastleEnterCount(LPMSG_REQ_BLOODCASTLE_ENTERCOUNT lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccountID[MAX_IDSTRING+1] = {0};
	char szName[MAX_IDSTRING+1] = {0};

	PMSG_ANS_BLOODCASTLE_ENTERCOUNT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x0B;
	pMsg.h.size = sizeof(pMsg);

	memcpy(szAccountID,lpMsg->AccountID,MAX_IDSTRING);
	memcpy(szName,lpMsg->GameID,MAX_IDSTRING);
	memcpy(pMsg.AccountID,szAccountID,MAX_IDSTRING);
	memcpy(pMsg.GameID,szName,MAX_IDSTRING);

	pMsg.ServerCode = lpMsg->ServerCode;
	pMsg.iObjIndex = lpMsg->iObjIndex;

	pMsg.iLeftCount = -1;

	sprintf(szQuery,"EXEC SP_LEFT_ENTERCOUNT_BC '%s', '%s', '%d'",szAccountID,szName,lpMsg->ServerCode);

	if(g_RankingDB.Exec(szQuery) == TRUE && g_RankingDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iLeftCount = g_RankingDB.GetInt("LeftEnterCount");
	}

	g_RankingDB.Clear();
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}


void BloodCastleScore_5TH(LPMSG_ANS_BLOODCASTLESCORE_5TH lpMsg)
{
	char szQuery[256];
	char szAccount[11] = {0};
	char szName[11] = {0};

	memcpy(szAccount,lpMsg->AccountID,10);
	memcpy(szName,lpMsg->GameID,10);

	g_Window.ServerLogAdd(ST_RANKINGSERVER,"[BLOOD] SERVER:%d-%d, BRIDGE:%d, ACCNT:%s, CHAR:%s, CLASS:%d, SCORE:%d, LEFTTIME:%d, ALIVEPARTY:%d",
		lpMsg->ServerCode / 20 + 1,lpMsg->ServerCode % 20 + 1,lpMsg->BridgeNum,szAccount,szName,lpMsg->Class,lpMsg->Score,lpMsg->iLeftTime,lpMsg->iAlivePartyCount);

	sprintf(szQuery,"EXEC SP_POINT_ACCM_BC_5TH '%d', '%d', '%s', '%s', '%d', '%d', '%d', '%d'",
		lpMsg->ServerCode / 20 + 1,lpMsg->BridgeNum,szAccount,szName,lpMsg->Class,lpMsg->Score,lpMsg->iLeftTime,lpMsg->iAlivePartyCount);

	if(g_RankingDB.Exec(szQuery) == FALSE)
	{
		g_Window.ServerLogAdd(ST_RANKINGSERVER,"error-L1 : %s", szQuery);
	}
	g_RankingDB.Clear();
}