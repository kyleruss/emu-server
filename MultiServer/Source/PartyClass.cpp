#include "stdafx.h"
#include "PartyClass.h"
#include "winutil.h"

CQuery g_PartyMatchDB;

int PartyMatchRegister(LPMSG_PARTYMATCHING_REG_REQ lpMsg)
{
	char szQuery[256];
	int retCode = -1;

#ifdef ZTEAMDB
	sprintf(szQuery,"EXEC PartyMatching_Register %d, '%s', '%s', '%s', %d, %d, %d, %d, %d, %d",
		lpMsg->ServerCode,lpMsg->szName,lpMsg->szDesc,lpMsg->szPass,
		lpMsg->MinLevel,lpMsg->MaxLevel,lpMsg->MoveReqId,lpMsg->ClassInfo,lpMsg->btAutoAccept, lpMsg->Gens);
#else
	sprintf(szQuery,"EXEC WZ_PM_Register %d, '%s', '%s', '%s', %d, %d, %d, %d, %d",
		lpMsg->ServerCode,lpMsg->szName,lpMsg->szDesc,lpMsg->szPass,
		lpMsg->MinLevel,lpMsg->MaxLevel,lpMsg->MoveReqId,lpMsg->ClassInfo,lpMsg->btAutoAccept);
#endif

	if( !g_PartyMatchDB.Exec(szQuery) || g_PartyMatchDB.Fetch() == SQL_NO_DATA )
	{
		g_PartyMatchDB.Clear();
		return retCode;
	}

	retCode = g_PartyMatchDB.GetInt("RegResult");
	g_PartyMatchDB.Clear();
	return retCode;
}

void DGPartyMatchRegister(LPMSG_PARTYMATCHING_REG_REQ lpMsg,int aIndex)
{
	PMSG_PARTYMATCHING_REG_ANS pMsg;
	pMsg.h.c = PMHC_BYTE;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xEF;
	pMsg.h.subcode = 0x00;

	memcpy(pMsg.szName,lpMsg->szName,MAX_IDSTRING);
	pMsg.szName[MAX_IDSTRING] = '\0';

	pMsg.aIndex = lpMsg->aIndex;
	pMsg.iResult = PartyMatchRegister(lpMsg);

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

int PartyMatchInfo(LPMSG_PARTYMATCHING_INFO_REQ lpMsg,PARTYMATCH_INFO* pBodyMsg,int& count,int& pages)
{
	char szQuery[256];
	long dwLen = 0;
	int TotalPages = 0;

#ifdef ZTEAMDB
	sprintf(szQuery,"EXEC PartyMatching_GetList ?,%d,%d,'%s',%d,%d,%d",
		lpMsg->RequestPage,lpMsg->ReqType,lpMsg->szSearch,
		lpMsg->Level,lpMsg->Class,lpMsg->Gens);
#else
	sprintf(szQuery,"EXEC WZ_PM_Listinfo ?,%d,%d,'%s',%d,%d,%d",
		lpMsg->RequestPage,lpMsg->ReqType,lpMsg->szSearch,
		lpMsg->Level,lpMsg->Class,lpMsg->Gens);
#endif

	g_PartyMatchDB.BindParameterIntOutput(1,&TotalPages,&dwLen);

	if( !g_PartyMatchDB.Exec(szQuery) )
	{
		g_PartyMatchDB.Clear();
		return -1;
	}

	count = 0;
	int fResult;

	while( (fResult = g_PartyMatchDB.Fetch()) != SQL_NO_DATA && fResult != SQL_ERROR )
	{
		pBodyMsg[count].btPartyCount = g_PartyMatchDB.GetInt("PartyCount");
		pBodyMsg[count].LeaderLevel = g_PartyMatchDB.GetInt("LeaderLevel");
		pBodyMsg[count].LeaderClass = g_PartyMatchDB.GetInt("LeaderClass");
		pBodyMsg[count].LeaderChannel = g_PartyMatchDB.GetInt("LeaderChannel");
		pBodyMsg[count].btClass = g_PartyMatchDB.GetInt("Class");
		pBodyMsg[count].MinLevel = g_PartyMatchDB.GetInt("MinLevel");
		pBodyMsg[count].MaxLevel = g_PartyMatchDB.GetInt("MaxLevel");
		pBodyMsg[count].MoveReqId = g_PartyMatchDB.GetInt("MoveReqId");
		pBodyMsg[count].Gens = g_PartyMatchDB.GetInt("Gens");

		char szTmpPassword[8] = {0};
		g_PartyMatchDB.GetStr("Password",szTmpPassword);
		pBodyMsg[count].btIsPassword = (strlen(szTmpPassword) > 1) ? TRUE : FALSE;

		g_PartyMatchDB.GetStr("PartyLeader",pBodyMsg[count].szLeader);
		g_PartyMatchDB.GetStr("Description",pBodyMsg[count].szDescription);
		count++;
	}

	while( g_PartyMatchDB.MoreResults() != SQL_NO_DATA )
	{

	}

	pages = TotalPages;
	g_PartyMatchDB.Clear();
	return 0;
}


void DGPartyMatchInfo(LPMSG_PARTYMATCHING_INFO_REQ lpMsg,int aIndex)
{
	BYTE cBUFFER[1024];

	int lOfs = sizeof(PMSG_PARTYMATCHING_INFO_ANS);

	LPMSG_PARTYMATCHING_INFO_ANS lpMsgCount = (LPMSG_PARTYMATCHING_INFO_ANS)&cBUFFER[0];
	PARTYMATCH_INFO* lpMsgBody = (PARTYMATCH_INFO*)&cBUFFER[lOfs];

	lpMsgCount->h.c = PMHC_WORD;
	lpMsgCount->h.headcode = 0xEF;
	lpMsgCount->h.subcode = 0x01;
	lpMsgCount->page = lpMsg->RequestPage;
	lpMsgCount->aIndex = lpMsg->aIndex;

	memcpy(lpMsgCount->szName,lpMsg->szName,sizeof(lpMsgCount->szName)-1);
	lpMsgCount->szName[sizeof(lpMsgCount->szName)-1] = '\0';

	lpMsgCount->iResult = PartyMatchInfo(lpMsg,lpMsgBody,lpMsgCount->count,lpMsgCount->totalpage);

	lOfs += lpMsgCount->count*sizeof(PARTYMATCH_INFO);

	lpMsgCount->h.sizeH = SET_NUMBERH(lOfs);
	lpMsgCount->h.sizeL = SET_NUMBERL(lOfs);

	DataSend(aIndex,(LPBYTE)&cBUFFER[0],lOfs);
}

int PartyMatchJoin(LPMSG_PARTYMATCHING_JOIN_REQ lpMsg)
{
	int iResult = 0;
	char szQuery[256];

#ifdef ZTEAMDB
	sprintf(szQuery,"EXEC PartyMatching_JoinRequest %d,%d,'%s','%s','%s',%d,%d,%d",
		lpMsg->RequestType,lpMsg->ServerCode,lpMsg->szPartyLeader,lpMsg->szPassword,
		lpMsg->szName,lpMsg->Level,lpMsg->Class,lpMsg->Gens);
#else
	sprintf(szQuery,"EXEC WZ_PM_JoinRequest %d,%d,'%s','%s','%s',%d,%d,%d",
		lpMsg->RequestType,lpMsg->ServerCode,lpMsg->szPartyLeader,lpMsg->szPassword,
		lpMsg->szName,lpMsg->Level,lpMsg->Class,lpMsg->Gens);
#endif

	if( !g_PartyMatchDB.Exec(szQuery) || g_PartyMatchDB.Fetch() == SQL_NO_DATA )
	{
		g_PartyMatchDB.Clear();
		return -8;
	}

	iResult = g_PartyMatchDB.GetInt("RequestResult");

	g_PartyMatchDB.Clear();
	return iResult;
}

void DGPartyMatchJoin(LPMSG_PARTYMATCHING_JOIN_REQ lpMsg,int aIndex)
{
	PMSG_PARTYMATCHING_JOIN_ANS pMsg;

	pMsg.h.c = PMHC_BYTE;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xEF;
	pMsg.h.subcode = 0x02;

	pMsg.aIndex = lpMsg->aIndex;
	memcpy(pMsg.szName,lpMsg->szPartyLeader,sizeof(pMsg.szName)-1);
	pMsg.szName[sizeof(pMsg.szName)-1] = '\0';

	pMsg.iResult = PartyMatchJoin(lpMsg);
	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

int PartyMatchWaitInfo(char* szName,int& ServerCode,char* szPartyLeader)
{
	char szQuery[256];

#ifdef ZTEAMDB
	sprintf(szQuery,"EXEC PartyMatching_Waiting '%s'",szName);
#else
	sprintf(szQuery,"EXEC WZ_PM_Waiting '%s'",szName);
#endif

	if( !g_PartyMatchDB.Exec(szQuery) || g_PartyMatchDB.Fetch() == SQL_NO_DATA )
	{
		g_PartyMatchDB.Clear();
		return -1;
	}
	
	ServerCode = g_PartyMatchDB.GetInt("PartyChannel");
	g_PartyMatchDB.GetStr("PartyLeader",szPartyLeader);
	g_PartyMatchDB.Clear();
	return 0;
}

void DGPartyMatchWaitInfo(LPMSG_PARTYMATCHING_WAIT_REQ lpMsg,int aIndex)
{
	PMSG_PARTYMATCHING_WAIT_ANS pMsg;
	pMsg.h.c = PMHC_BYTE;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xEF;
	pMsg.h.subcode = 0x03;
	pMsg.iResult = 0;
	pMsg.aIndex = lpMsg->aIndex;
	memcpy(pMsg.szName,lpMsg->szName,sizeof(pMsg.szName)-1);
	pMsg.szName[sizeof(pMsg.szName)-1] = '\0';

	PartyMatchWaitInfo(lpMsg->szName,pMsg.ServerCode,pMsg.szPartyLeader);
	
	if( pMsg.ServerCode == -1 )
	{
		pMsg.iResult = -1;
	}

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

int PartyMatchCancel(char* szName,int Type)
{
	int iResult = 0;
	char szQuery[256];

#ifdef ZTEAMDB
	sprintf(szQuery,"EXEC PartyMatching_Cancel '%s',%d",szName,Type);
#else
	sprintf(szQuery,"EXEC WZ_PM_Cancel '%s',%d",szName,Type);
#endif

	if( !g_PartyMatchDB.Exec(szQuery) || g_PartyMatchDB.Fetch() == SQL_NO_DATA )
	{
		g_PartyMatchDB.Clear();
		return 2;
	}

	iResult = g_PartyMatchDB.GetInt("CancelResult");
	g_PartyMatchDB.Clear();
	return iResult;
}

void DGPartyMatchCancel(LPMSG_PARTYMATCHING_CANCEL_REQ lpMsg,int aIndex)
{
	PMSG_PARTYMATCHING_CANCEL_ANS pMsg;
	pMsg.h.c = PMHC_BYTE;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xEF;
	pMsg.h.subcode = 0x06;

	pMsg.aIndex = lpMsg->aIndex;
	pMsg.btType = lpMsg->btType;

	memcpy(pMsg.szName,lpMsg->szName,sizeof(pMsg.szName)-1);
	pMsg.szName[sizeof(pMsg.szName)-1] = '\0';

	pMsg.iResult = PartyMatchCancel(lpMsg->szName,lpMsg->btType);

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void DGPartyMatchReqList(PARTYMATCH_GDREQ_REQUESTLIST* lpData, int aIndex)
{
	const int MaxSize = sizeof(PARTYMATCH_DGANS_REQUESTLIST_COUNT) + sizeof(PARTYMATCH_DGANS_REQUESTLIST_INFO) * 80;
	BYTE Buffer[MaxSize + 1] = { 0 };
	int Offset = sizeof(PARTYMATCH_DGANS_REQUESTLIST_COUNT);
	// ----
	PARTYMATCH_DGANS_REQUESTLIST_COUNT* lpAnswerCount = (PARTYMATCH_DGANS_REQUESTLIST_COUNT*)Buffer;
	PARTYMATCH_DGANS_REQUESTLIST_INFO* lpAnswerInfo = (PARTYMATCH_DGANS_REQUESTLIST_INFO*)&Buffer[Offset];
	// ----
	lpAnswerCount->h.c = PMHC_WORD;
	lpAnswerCount->h.headcode = 0xEF;
	lpAnswerCount->h.subcode = 0x04;
	lpAnswerCount->h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount->h.sizeL = SET_NUMBERL(Offset);
	lpAnswerCount->UserIndex = lpData->UserIndex;
	// ----
	char Query[256];
	long Len = 0;
	// ----
	sprintf(Query, "EXEC PartyMatching_GetRequestList '%s'", lpData->PartyLeader);
	// ----
	if( !g_PartyMatchDB.Exec(Query) )
	{
		g_PartyMatchDB.Clear();
		DataSend(aIndex, (LPBYTE)lpAnswerCount, Offset);
		return;
	}
	// ----
	int Result, Count = 0;
	// ----
	while( (Result = g_PartyMatchDB.Fetch()) != SQL_NO_DATA && Result != SQL_ERROR )
	{
		g_PartyMatchDB.GetStr("Name", lpAnswerInfo[Count].Character);
		lpAnswerInfo[Count].Class = g_PartyMatchDB.GetInt("SenderClass");
		lpAnswerInfo[Count].Level = g_PartyMatchDB.GetInt("SenderLevel");
		Offset += sizeof(PARTYMATCH_DGANS_REQUESTLIST_INFO);
		Count++;
	}
	// ----
	while( g_PartyMatchDB.MoreResults() != SQL_NO_DATA )
	{
		
	}
	// ----
	g_PartyMatchDB.Clear();
	// ----
	lpAnswerCount->h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount->h.sizeL = SET_NUMBERL(Offset);
	lpAnswerCount->Count = Count;
	// ----
	DataSend(aIndex, (LPBYTE)Buffer, Offset);
}

void DGPartyMatchRequestAnswer(PARTYMATCH_GDREQ_REQUESTANSWER* lpData, int aIndex)
{
	char Query[256];
	// ----
	PARTYMATCH_DGANS_REQUESTANSWER lpAnswer = { 0 };
	// ----
	lpAnswer.h.c = PMHC_BYTE;
	lpAnswer.h.headcode = 0xEF;
	lpAnswer.h.subcode = 0x05;
	lpAnswer.h.size = sizeof(PARTYMATCH_DGANS_REQUESTANSWER);
	lpAnswer.UserIndex = lpData->UserIndex;
	memcpy(lpAnswer.Character, lpData->Character, 10);
	lpAnswer.Type = lpData->Type;
	lpAnswer.Result = -1;
	// ----
	if( lpData->Type == 0 )
	{
		sprintf(Query, "DELETE FROM PartyMatching_List WHERE Name = '%s'", lpData->Character);
		// ----
		if( !g_PartyMatchDB.Exec(Query) )
		{
			g_PartyMatchDB.Clear();
			DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(PARTYMATCH_DGANS_REQUESTANSWER));
			return;
		}
		// ----
		g_PartyMatchDB.Clear();
		lpAnswer.Result = 1;
		DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(PARTYMATCH_DGANS_REQUESTANSWER));
	}
	// ----
	DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(PARTYMATCH_DGANS_REQUESTANSWER));
}

void PartyMatchUpdateServerInfo(int ServerCode, LPSTR szName)
{
	char szQuery[256];
	sprintf(szQuery,"SELECT * FROM PartyMatching_InfoData WHERE PartyLeader = '%s'", szName);
	g_PartyMatchDB.Exec(szQuery);

	if( g_PartyMatchDB.Fetch() != SQL_NO_DATA )
	{
		g_PartyMatchDB.Clear();
		ZeroMemory(szQuery, sizeof(szQuery));
		sprintf(szQuery,"UPDATE PartyMatching_InfoData SET ServerCode = %d WHERE PartyLeader = '%s'", ServerCode, szName);
		g_PartyMatchDB.Exec(szQuery);
	}
	g_PartyMatchDB.Clear();
	PartyMatchUpdateStatus(szName, 1);
}

void PartyMatchUpdateStatus(LPSTR szName, BYTE Status)
{
	char szQuery[256];
	sprintf(szQuery,"SELECT * FROM PartyMatching_InfoData WHERE PartyLeader = '%s'", szName);
	g_PartyMatchDB.Exec(szQuery);

	if( g_PartyMatchDB.Fetch() != SQL_NO_DATA )
	{
		g_PartyMatchDB.Clear();
		ZeroMemory(szQuery, sizeof(szQuery));
		sprintf(szQuery,"UPDATE PartyMatching_InfoData SET Status = %d WHERE PartyLeader = '%s'", Status, szName);
		g_PartyMatchDB.Exec(szQuery);
	}
	g_PartyMatchDB.Clear();
}

void GDPartyMatchUpdateStatus(PARTYMATCH_GDREQ_UPDATESTATUS* lpData)
{
	PartyMatchUpdateStatus(lpData->Character, lpData->Status);
}
