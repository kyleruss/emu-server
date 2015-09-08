#include "stdafx.h"
#include "zMultiServer.h"
#include "DataServer.h"
#include "Window.h"
#include "winutil.h"
#include "EventServer.h"

ES_INFO g_ESInfo;
CQuery g_EventDB;
CQuery g_MyEventDB;

void EventServerInit()
{
g_bIsServer[ST_EVENTSERVER] = TRUE;

	g_ESInfo.TCPPort = GetPrivateProfileInt(ES_INISECTION,"TCPPort",44456,INI_FILE);
	GetPrivateProfileString(ES_INISECTION,"DSN","MU2003_EVENT_DB",g_ESInfo.dbDSN,sizeof(g_ESInfo.dbDSN),INI_FILE);
	GetPrivateProfileString(ES_INISECTION,"MyDSN","MuOnline",g_ESInfo.mydbDSN,sizeof(g_ESInfo.mydbDSN),INI_FILE);

	g_EventDB.S_TYPE = ST_EVENTSERVER;
	g_MyEventDB.S_TYPE = ST_EVENTSERVER;

	if( g_EventDB.Connect(3,g_ESInfo.dbDSN,SQLUser,SQLPass) == TRUE )
	{
		g_Window.LogAdd(Log::Info, "[ %s ] connection successful !",g_ESInfo.dbDSN);
	}

	if( g_MyEventDB.Connect(3,g_ESInfo.mydbDSN,SQLUser,SQLPass) == TRUE )
	{
		g_Window.LogAdd(Log::Info, "[ %s ] connection successful !",g_ESInfo.mydbDSN);
	}

	if(CreateGIocp(g_ESInfo.TCPPort,ST_EVENTSERVER) != TRUE)
	{
		g_Window.LogAdd(Log::Error, "Failed to start [ ST_EVENTSERVER ] on port [ %d ]",g_ESInfo.TCPPort);
	}
	else
	{
		g_Window.LogAdd(Log::Info, "[ EventServer ] created on port [ %d ]",g_ESInfo.TCPPort);
	}
}

void ESProtocolCore(int aIndex, DWORD headcode, LPBYTE aRecv, int Len)
{
//	g_ServerInfoDisplayer.SetState(TRUE,gSObj[aIndex].SType);

	switch( headcode )
	{
	case 0x01:
		EGAnsEventChipInfo((LPMSG_REQ_VIEW_EC_MN)aRecv,aIndex);
		break;
	case 0x02:
		EGAnsRegEventChipInfo((LPMSG_REQ_REGISTER_EVENTCHIP)aRecv,aIndex);
		break;
	case 0x04:
		EGAnsResetEventChip((LPMSG_REQ_RESET_EVENTCHIP)aRecv,aIndex);
		break;
	case 0x05:
		EGAnsEventStoneInfo((LPMSG_REQ_VIEW_EC_MN)aRecv,aIndex);
		break;
	case 0x06:
		EGAnsRegEventStoneInfo((LPMSG_REQ_REGISTER_STONES)aRecv,aIndex);
		break;
	case 0x07:
		EGAnsDeleteStones((LPMSG_REQ_DELETE_STONES)aRecv,aIndex);
		break;
	case 0x08:
		EGAns2AnivRegSerial((LPMSG_REQ_2ANIV_SERIAL)aRecv,aIndex);
		break;
	case 0x09:
		EGAnsResetStoneInfo((LPMSG_REQ_RESET_EVENTCHIP)aRecv,aIndex);
		break;

	case 0x29:
		EGAnsPCBangUserConnect(aRecv,aIndex);
		break;
	case 0x24:
		EGAnsPCBangUpdateUser(aRecv,aIndex);
		break;

	case 0x40:
		EGAnsEnterDoubleGoer(aRecv,aIndex);
		break;
	case 0x41:
		EGUpdateDoubleGoer(aRecv,aIndex);
		break;


	case 0x64:
		{
		PBMSG_HEAD2 * lpRecv = (PBMSG_HEAD2*)aRecv;

			switch(lpRecv->subcode)
			{
			case 0:
				//ReqExchangeSymbols((LPMSG_EXSYMBOL)aRecv,aIndex);
				break;
			}
		}
		break;
	case 0x65:
		{
		PBMSG_HEAD2 * lpRecv = (PBMSG_HEAD2*)aRecv;

			switch(lpRecv->subcode)
			{
			case 0:
				//CustomQuestReqInfo((LPMSG_CUSTOMQUEST_REQUSER)aRecv,aIndex);
				break;
			case 2:
				//CustomQuestSaveInfo((LPMSG_CUSTOMQUEST_SAVEUSER)aRecv,aIndex);
				break;
			case 10:
				AddCashPoints((LPMSG_ADD_CP)aRecv,aIndex);
				break;
			case 16:
				//CustomQuest2ReqUser((PMSG_QUEST2_REQ_USER*)aRecv,aIndex);
				break;
			case 18:
				//CustomQuest2SaveUser((PMSG_QUEST2_SAVE_USER*)aRecv);
				break;
			}
		}
		break;
	case 0x66:
		{
		PBMSG_HEAD2 * lpRecv = (PBMSG_HEAD2*)aRecv;

			switch(lpRecv->subcode)
			{
			case 0:
				//ReqSummonPetInfo((LPBMSG_REQSUMMONINFO)aRecv,aIndex);
				break;
			case 1:
				//ReqSaveSummonInfo((LPBMSG_REQSAVESUMMON)aRecv);
				break;
			case 2:
				//ReqMarry((LPMSG_REQ_MARRYINFO)aRecv,aIndex);
				break;
			case 3:
				//DoMarry((LPMSG_REQ_MARRY2)aRecv,aIndex);
				break;
			case 4:
				//Divorce((LPMSG_REQ_DIVORCE)aRecv,aIndex);
				break;

			case 16:
				//LoadNoblessInfo((LPMSG_REQ_NOBLESSINFO)aRecv,aIndex);
				break;
			case 17:
				//SaveNoblessInfo((LPMSG_REQ_SAVENOBLESSINFO)aRecv);
				break;
			}
		}
		break;

	case 0x67:
		UpdateConnectState((PMSG_UPDATE_CONNECT_STAT*)aRecv);
		break;

	case 0x68:
		{
			PBMSG_HEAD2 * lpRecv = (PBMSG_HEAD2*)aRecv;

			switch(lpRecv->subcode)
			{
			case 0:
				//LoadOlympInfo((LOLP_REQ_OLYMPINITDATA)aRecv,aIndex);
				break;
			case 1:
				//ReqOlympChangeDate((LPOLP_REQ_SDEDCHANGE)aRecv,aIndex);
				break;
			case 2:
				//ReqRegOlymp((LPOLP_REQ_REGFIGHT)aRecv,aIndex);
				break;
			case 3:
				//OlympEndFight((LPOLP_ENFIGHT)aRecv,aIndex);
				break;
			case 4:
				//OlympResetPeriod((LPOLP_REQ_RESET)aRecv,aIndex);
				break;
			case 5:
				//OlympRewardRequest((LPOLP_REQ_REWARD)aRecv,aIndex);
				break;
			case 6:
				//OlympStatsRequest((LPOLP_REQ_STATS)aRecv,aIndex);
				break;
			}
		}
		break;

	case 0x70:
		//ReqSellFullItem((LPMSG_REQ_SELLFULLOPT)aRecv,aIndex);
		break;
	}

//	g_ServerInfoDisplayer.SetState(FALSE,gSObj[aIndex].SType);
}

void EGAnsEventChipInfo(LPMSG_REQ_VIEW_EC_MN lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccountID[MAX_IDSTRING+1] = {0};

	memcpy(szAccountID,lpMsg->szUID, MAX_IDSTRING);

	PMSG_ANS_VIEW_EC_MN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x01;
	pMsg.h.size = sizeof( pMsg );

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,szAccountID);
	pMsg.szUID[10] = 0;

	sprintf(szQuery,"SELECT EventChips, Check_Code, MuttoNumber FROM T_MU2003_EVENT WHERE AccountID='%s'",szAccountID);

	if(g_EventDB.Exec(szQuery) != TRUE)
	{
		pMsg.bSUCCESS = FALSE;
	}
	else
	{
		if(g_EventDB.Fetch() != SQL_NO_DATA)
		{
			if(g_EventDB.GetInt("Check_Code") != 0)
			{
				pMsg.nEVENT_CHIPS = 0;
			}
			else
			{
				pMsg.nEVENT_CHIPS = g_EventDB.GetInt("EventChips");
			}

			pMsg.iMUTO_NUM = g_EventDB.GetInt("MuttonNumber");
			pMsg.bSUCCESS = TRUE;
		}
		else
		{
			g_EventDB.Clear();
			sprintf(szQuery,"INSERT INTO T_MU2003_EVENT (AccountID) VALUES ('%s')",szAccountID);
			if(g_EventDB.Exec(szQuery) == FALSE)
			{
				pMsg.bSUCCESS = FALSE;
			}
			else
			{
				pMsg.nEVENT_CHIPS = 0;
				pMsg.iMUTO_NUM = 0;
				pMsg.bSUCCESS = TRUE;
			}
		}
	}
	g_EventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void EGAnsRegEventChipInfo(LPMSG_REQ_REGISTER_EVENTCHIP lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccount[MAX_IDSTRING+1] = {0};

	memcpy(szAccount,lpMsg->szUID,MAX_IDSTRING);

	PMSG_ANS_REGISTER_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x02;
	pMsg.h.size = sizeof( pMsg );

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.Pos = lpMsg->Pos;

	strcpy(pMsg.szUID,szAccount);

	pMsg.szUID[10] = 0;
	pMsg.bSUCCESS = FALSE;

	sprintf(szQuery,"UPDATE T_MU2003_EVENT SET EventChips = EventChips + 1 WHERE AccountID = '%s'",szAccount);

	if(g_EventDB.Exec(szQuery) == TRUE)
	{
		g_EventDB.Clear();

		sprintf(szQuery,"SELECT EventChips FROM T_MU2003_EVENT WHERE AccountID='%s'",szAccount);

		if(g_EventDB.Exec(szQuery) == TRUE && g_EventDB.Fetch() != SQL_NO_DATA)
		{
			pMsg.bSUCCESS = TRUE;
			pMsg.nEVENT_CHIPS = g_EventDB.GetInt("EventChips");
		}
	}

	g_EventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void EGAnsResetEventChip(LPMSG_REQ_RESET_EVENTCHIP lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccount[MAX_IDSTRING+1] = {0};

	memcpy(szAccount,lpMsg->szUID,MAX_IDSTRING);

	PMSG_ANS_RESET_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x04;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,szAccount);
	pMsg.szUID[10] = 0;

	sprintf(szQuery,"UPDATE T_MU2003_EVENT SET Check_Code = 2 WHERE AccountID = '%s'",szAccount);

	pMsg.bSUCCESS = FALSE;

	if(g_EventDB.Exec(szQuery) == TRUE)
	{
		pMsg.bSUCCESS = TRUE;
	}

	g_EventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void EGAnsEventStoneInfo(LPMSG_REQ_VIEW_EC_MN lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccount[MAX_IDSTRING+1] = {0};

	memcpy(szAccount,lpMsg->szUID,MAX_IDSTRING);
	PMSG_ANS_VIEW_STONES pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x05;
	pMsg.h.size = sizeof(pMsg);

	strcpy(pMsg.szUID,szAccount);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.szUID[10] = 0;

	sprintf(szQuery,"SELECT StoneCount, Check_Code FROM T_BLOOD_CASTLE WHERE AccountID='%s'",szAccount);

	pMsg.bSUCCESS = FALSE;

	if(g_EventDB.Exec(szQuery) == TRUE)
	{
		if(g_EventDB.Fetch() != SQL_NO_DATA)
		{
			if(g_EventDB.GetInt("Check_Code") != 0)
			{
				pMsg.iStoneCount = 0;
			}
			else
			{
				pMsg.iStoneCount = g_EventDB.GetInt("StoneCount");
			}
			pMsg.bSUCCESS = TRUE;
		}
		else
		{
			g_EventDB.Clear();

			sprintf(szQuery,"INSERT INTO T_BLOOD_CASTLE (AccountID) VALUES ('%s')",szAccount);

			if(g_EventDB.Exec(szQuery) == TRUE)
			{
				pMsg.bSUCCESS = TRUE;
				pMsg.iStoneCount = 0;
			}
		}
	}

	g_EventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void EGAnsRegEventStoneInfo(LPMSG_REQ_REGISTER_STONES lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccount[MAX_IDSTRING+1] = {0};

	memcpy(szAccount,lpMsg->szUID,MAX_IDSTRING);
	PMSG_ANS_REGISTER_STONES pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x06;
	pMsg.h.size = sizeof(pMsg);

	strcpy(pMsg.szUID,szAccount);
	pMsg.szUID[10] = 0;

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.iPosition = lpMsg->iPosition;

	pMsg.bSUCCESS = FALSE;

	sprintf(szQuery,"UPDATE T_BLOOD_CASTLE SET StoneCount = StoneCount + 1 WHERE AccountID = '%s'",szAccount);

	if(g_EventDB.Exec(szQuery) == TRUE)
	{
		g_EventDB.Clear();

		sprintf(szQuery,"SELECT StoneCount FROM T_BLOOD_CASTLE WHERE AccountID='%s'",szAccount);

		if(g_EventDB.Exec(szQuery) == TRUE && g_EventDB.Fetch() != SQL_NO_DATA)
		{
			pMsg.iStoneCount = g_EventDB.GetInt("StoneCount");
			pMsg.bSUCCESS = TRUE;
		}
	}
	g_EventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void EGAnsDeleteStones(LPMSG_REQ_DELETE_STONES lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccount[MAX_IDSTRING+1] = {0};

	memcpy(szAccount,lpMsg->szUID,MAX_IDSTRING);
	PMSG_ANS_DELETE_STONES pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x07;
	pMsg.h.size = sizeof( pMsg );

	strcpy(pMsg.szUID,szAccount);
	pMsg.szUID[10] = 0;

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.bSUCCESS = FALSE;

	sprintf(szQuery,"SELECT StoneCount FROM T_BLOOD_CASTLE WHERE AccountID='%s'",szAccount);

	if( g_EventDB.Exec(szQuery) == TRUE && g_EventDB.Fetch() != SQL_NO_DATA )
	{
		int Stones = g_EventDB.GetInt("StoneCount");

		g_EventDB.Clear();

		if(Stones >= lpMsg->iStoneCount)
		{
			sprintf(szQuery,"UPDATE T_BLOOD_CASTLE SET StoneCount = StoneCount - %d WHERE AccountID='%s'",Stones,szAccount);

			if(g_EventDB.Exec(szQuery) == TRUE)
			{
				pMsg.bSUCCESS = TRUE;
				pMsg.iStoneCount = Stones;
			}
		}
	}

	g_EventDB.Clear();
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void EGAns2AnivRegSerial(LPMSG_REQ_2ANIV_SERIAL lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccount[MAX_IDSTRING+1] = {0};

	memcpy(szAccount,lpMsg->szUID,MAX_IDSTRING);
	PMSG_ANS_2ANIV_SERIAL pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x08;
	pMsg.h.size = sizeof( pMsg );

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,szAccount);

	sprintf(szQuery,"EXEC SP_REG_SERIAL '%s', '%d', '%s', '%s', '%s'",
		szAccount,lpMsg->iMEMB_GUID,lpMsg->SERIAL1,lpMsg->SERIAL2,lpMsg->SERIAL3);

	pMsg.btIsRegistered = 4;

	if(g_EventDB.Exec(szQuery) && g_EventDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.btIsRegistered = g_EventDB.GetInt("RegResult");
		pMsg.iGiftNumber = g_EventDB.GetInt("F_Register_Section");

		if(pMsg.iGiftNumber == -1)
		{
			pMsg.btIsRegistered = 4;
		}
	}
	g_EventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void EGAnsResetStoneInfo(LPMSG_REQ_RESET_EVENTCHIP lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccount[MAX_IDSTRING+1] = {0};

	memcpy(szAccount,lpMsg->szUID,MAX_IDSTRING);
	PMSG_ANS_RESET_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x09;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,szAccount);
	pMsg.szUID[10] = 0;

	pMsg.bSUCCESS = FALSE;

	sprintf(szQuery,"UPDATE T_BLOOD_CASTLE SET Check_Code = 2 WHERE AccountID = '%s'",szAccount);

	if(g_EventDB.Exec(szQuery) == TRUE)
	{
		pMsg.bSUCCESS = TRUE;
	}

	g_EventDB.Clear();
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void ReqExchangeSymbols(LPMSG_EXSYMBOL lpMsg,int aIndex)
{
	char szQuery[256];
	char szAccount[MAX_IDSTRING+1];
	szAccount[MAX_IDSTRING] = 0;
	memcpy(szAccount,lpMsg->szAccount,MAX_IDSTRING);

	//sprintf(szQuery,"SELECT count(*) FROM MEMB_CREDITS WHERE memb___id = '%s'",szAccount);

	/*if(g_MyEventDB.Exec(szQuery) == TRUE)
	{
		if(g_MyEventDB.Fetch() != SQL_NO_DATA)
		{
			g_MyEventDB.Clear();

			sprintf(szQuery,"UPDATE MEMB_CREDITS SET credits = credits + %d WHERE memb___id = '%s'",lpMsg->points,szAccount);

			if( g_MyEventDB.Exec(szQuery) == TRUE )
			{
				LogAddTD(ST_EVENTSERVER,"[Exhange Symbol] Account (%s) add [%d] points",szAccount,lpMsg->points);
			}
		}
		else
		{
			g_MyEventDB.Clear();
			sprintf(szQuery,"INSERT INTO MEMB_CREDITS (memb___id,credits) VALUES ('%s',%d)",szAccount,lpMsg->points);

			if( g_MyEventDB.Exec(szQuery) == TRUE )
			{
				LogAddTD(ST_EVENTSERVER,"[Exhange Symbol] Account Insert (%s) add [%d] points",szAccount,lpMsg->points);
			}
		}
	}*/

	sprintf(szQuery,"UPDATE MEMB_INFO SET web_points = web_points + %d WHERE memb___id = '%s'",lpMsg->points,szAccount);

	g_MyEventDB.Exec(szQuery);
	g_MyEventDB.Clear();
}

void AddCashPoints(LPMSG_ADD_CP lpMsg,int aIndex)
{
	char szQuery[256];
	char szName[MAX_IDSTRING+1] = {0};

	memcpy(szName,lpMsg->Name,MAX_IDSTRING);

#ifdef ZTEAMDB
	sprintf(szQuery,"UPDATE GameShop_Data SET WCoinC = WCoinC + %d WHERE MemberGuid = %d",lpMsg->iPoints,lpMsg->DBNumber);
#else
	sprintf(szQuery,"UPDATE T_CashShop_DATA SET wcoin_c = wcoin_c + %d WHERE memb_guid = %d",lpMsg->iPoints,lpMsg->DBNumber);
#endif

	if(g_MyEventDB.Exec(szQuery) == FALSE)
	{
		g_Window.ServerLogAdd(ST_EVENTSERVER,"[CashShop] Points add fail (%d) - [%d] (%s)",lpMsg->iPoints,lpMsg->DBNumber,szName);
	}
	else
	{
		g_Window.ServerLogAdd(ST_EVENTSERVER,"[CashShop] Points add (%d) - [%d] (%s)",lpMsg->iPoints,lpMsg->DBNumber,szName);
	}
}

void ReqSummonPetInfo(LPBMSG_REQSUMMONINFO lpMsg,int aIndex)
{
char szQuery[256];
char szName[MAX_IDSTRING+1] = {0};

	memcpy(szName,lpMsg->szName,MAX_IDSTRING);

	g_Window.ServerLogAdd(ST_EVENTSERVER,"[SUMMON PET] Data request (%s)",szName);

	PBMSG_ANSSUMMONINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof( pMsg );
	pMsg.h.headcode = 0x66;
	pMsg.h.subcode = 0;

	pMsg.aIndex = lpMsg->aIndex;
	strcpy(pMsg.szName,szName);

	pMsg.result = FALSE;

	sprintf(szQuery,"SELECT summon_level, summon_exp FROM T_SummonPet_Info WHERE memb__char = '%s'",szName);

	if( g_MyEventDB.Exec(szQuery) == TRUE )
	{
		pMsg.result = TRUE;

		pMsg.Level = 0;
		pMsg.Exp = 0;

		if( g_MyEventDB.Fetch() != SQL_NO_DATA )
		{
			pMsg.Level = g_MyEventDB.GetInt("summon_level");
			pMsg.Exp = g_MyEventDB.GetInt64("summon_exp");
		}
		else
		{
			g_MyEventDB.Clear();

			sprintf(szQuery,"INSERT INTO T_SummonPet_Info (memb__char,summon_level,summon_exp) VALUES ('%s',1,0)",szName);
			g_MyEventDB.Exec(szQuery);
		}
	}
	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof( pMsg ));
}

void ReqSaveSummonInfo(LPBMSG_REQSAVESUMMON lpMsg)
{
char szQuery[256];
char szName[MAX_IDSTRING + 1] = {0};

	memcpy(szName,lpMsg->szName, MAX_IDSTRING);

	sprintf(szQuery,"UPDATE T_SummonPet_Info SET summon_level = %d, summon_exp = %I64d WHERE memb__char = '%s'",
		lpMsg->level,lpMsg->exp,szName);

	g_MyEventDB.Exec(szQuery);
	g_MyEventDB.Clear();
}



void ReqMarry(LPMSG_REQ_MARRYINFO lpMsg,int aIndex)
{
	char szQuery[256];
	char szName[MAX_IDSTRING+1] = {0};
	char szTargetName[MAX_IDSTRING+1] = {0};

	memcpy(szName,lpMsg->ReqName,MAX_IDSTRING);

	PMSG_ANS_MARRY pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof( pMsg );
	pMsg.h.headcode = 0x66;
	pMsg.h.subcode = 0x02;

	pMsg.Result = FALSE;

	pMsg.ReqIndex = lpMsg->ReqIndex;
	memcpy(pMsg.ReqName,szName,MAX_IDSTRING);

	sprintf(szQuery,"SELECT * FROM wedding WHERE husband = '%s' OR wife = '%s'",
		szName,szName);

	if( g_MyEventDB.Exec(szQuery) == TRUE )
	{
		if( g_MyEventDB.Fetch() != SQL_NO_DATA )
		{
			pMsg.Result = 2;
			char szWife[11] = {0};
			char szHusband[11] = {0};

			g_MyEventDB.GetStr("husband",szHusband);
			g_MyEventDB.GetStr("wife",szWife);

			if( strcmp(szWife,szName) == NULL)
			{
				memcpy(pMsg.MarriedName,szHusband,MAX_IDSTRING);
			}
			else
			{
				memcpy(pMsg.MarriedName,szWife,MAX_IDSTRING);
			}
		}
		else
		{
			pMsg.Result = 1;
		}
	}

	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof( pMsg ));
}

void DoMarry(LPMSG_REQ_MARRY2 lpMsg,int aIndex)
{
	char szQuery[256];
	char szName[MAX_IDSTRING+1] = {0};
	char szTName[MAX_IDSTRING+1] = {0};

	memcpy(szName,lpMsg->aName,MAX_IDSTRING);
	memcpy(szTName,lpMsg->tName,MAX_IDSTRING);

	sprintf(szQuery,"INSERT INTO wedding (husband,wife,date) VALUES ('%s','%s',GETDATE())",
		szName,szTName);

	g_MyEventDB.Exec(szQuery);
	g_MyEventDB.Clear();
}

void Divorce(LPMSG_REQ_DIVORCE lpMsg,int aIndex)
{
	char szQuery[256];
	char szName[MAX_IDSTRING+1] = {0};

	memcpy(szName,lpMsg->szName,MAX_IDSTRING);

	sprintf(szQuery,"SELECT * FROM wedding WHERE husband = '%s' OR wife = '%s'",
		szName,szName);

	PMSG_ANS_DIVORCE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x66;
	pMsg.h.subcode = 0x04;

	pMsg.btResult = FALSE;

	if( g_MyEventDB.Exec(szQuery) == TRUE )
	{
		if( g_MyEventDB.Fetch() == SQL_NO_DATA )
		{
			pMsg.btResult =2;
		}
		else
		{
			g_MyEventDB.GetStr("wife",pMsg.Wife);
			g_MyEventDB.GetStr("husband",pMsg.Husband);
			g_MyEventDB.Clear();
			sprintf(szQuery,"DELETE FROM wedding WHERE husband = '%s' OR wife = '%s'",szName,szName);
			if( g_MyEventDB.Exec(szQuery) == TRUE )
			{
				pMsg.btResult = TRUE;
			}
		}
	}

	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof( pMsg ));
}

void CustomQuestReqInfo(LPMSG_CUSTOMQUEST_REQUSER lpMsg,int aIndex)
{
	char szName[MAX_IDSTRING+1];
	szName[MAX_IDSTRING] = '\0';

	memcpy(szName,lpMsg->szUserName,MAX_IDSTRING);

	PMSG_CUSTOMQUEST_ANSUSER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x65;
	pMsg.h.subcode = 0x01;

	pMsg.result = FALSE;
	pMsg.aIndex = lpMsg->aIndex;
	memcpy(pMsg.szUserName,lpMsg->szUserName,MAX_IDSTRING);

	pMsg.QuestGroup = -1;
	pMsg.QuestIndex = -1;
	pMsg.QuestItem[0] = 0;
	pMsg.QuestItem[1] = 0;
	pMsg.QuestItem[2] = 0;

	char szQuery[256];

	sprintf(szQuery,"SELECT QuestGroup,QuestIndex,QuestState,QuestItem_1,QuestItem_2,QuestItem_3 FROM T_CustomQuest WHERE CHAR_NAME = '%s'",szName);

	if( g_MyEventDB.Exec(szQuery) == TRUE )
	{
		if( g_MyEventDB.Fetch() == SQL_NO_DATA )
		{
			g_MyEventDB.Clear();

			sprintf(szQuery,"INSERT INTO T_CustomQuest (CHAR_NAME,QuestGroup,QuestIndex,QuestState,QuestItem_1,QuestItem_2,QuestItem_3) VALUES ('%s',-1,-1,0,0,0,0)",szName);

			if( g_MyEventDB.Exec(szQuery) == TRUE )
			{
				pMsg.result = TRUE;
			}
		}
		else
		{
			pMsg.result = TRUE;

			pMsg.QuestGroup = g_MyEventDB.GetInt("QuestGroup");
			pMsg.QuestIndex = g_MyEventDB.GetInt("QuestIndex");
			pMsg.QuestState = g_MyEventDB.GetInt("QuestState");
			pMsg.QuestItem[0] = g_MyEventDB.GetInt("QuestItem_1");
			pMsg.QuestItem[1] = g_MyEventDB.GetInt("QuestItem_2");
			pMsg.QuestItem[2] = g_MyEventDB.GetInt("QuestItem_3");
		}
	}

	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void CustomQuestSaveInfo(LPMSG_CUSTOMQUEST_SAVEUSER lpMsg,int aIndex)
{
	char szName[MAX_IDSTRING+1];
	szName[MAX_IDSTRING] = '\0';

	memcpy(szName,lpMsg->szUserName,MAX_IDSTRING);

	char szQuery[256];

	sprintf(szQuery,"UPDATE T_CustomQuest SET QuestGroup = %d,QuestIndex = %d,QuestState = %d,QuestItem_1 = %d,QuestItem_2 = %d,QuestItem_3 = %d WHERE CHAR_NAME = '%s'",
		lpMsg->QuestGroup,lpMsg->QuestIndex,lpMsg->QuestState,lpMsg->QuestItem[0],lpMsg->QuestItem[1],lpMsg->QuestItem[2],szName);

	g_MyEventDB.Exec(szQuery);
	g_MyEventDB.Clear();
}

void CustomQuest2ReqUser(PMSG_QUEST2_REQ_USER* lpMsg,int aIndex)
{
	char szName[MAX_IDSTRING+1];
	szName[MAX_IDSTRING] = '\0';

	memcpy(szName,lpMsg->szName,MAX_IDSTRING);

	PMSG_QUEST2_ANS_USER pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg,0x65,0x11,sizeof(pMsg));

	pMsg.aIndex = lpMsg->aIndex;
	strcpy(pMsg.szName,szName);

	char szQuery[256];
	sprintf(szQuery,"SELECT QUEST_DATA FROM T_CustomQuest2 WHERE CHAR_NAME = '%s'",szName);

	switch( g_MyEventDB.ReadBlob(szQuery,&pMsg.m_UserData) )
	{
	case 0:
		sprintf(szQuery,"INSERT INTO T_CustomQuest2 (CHAR_NAME,QUEST_DATA) VALUES ('%s',?)",szName);
		g_MyEventDB.WriteBlob(szQuery,&pMsg.m_UserData,sizeof(pMsg.m_UserData));
		break;
	case sizeof(pMsg.m_UserData):
		break;
	default:
		return;
	}

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void CustomQuest2SaveUser(PMSG_QUEST2_SAVE_USER* lpMsg)
{
	char szName[MAX_IDSTRING+1];
	szName[MAX_IDSTRING] = '\0';

	memcpy(szName,lpMsg->szName,MAX_IDSTRING);

	char szQuery[256];
	sprintf(szQuery,"UPDATE T_CustomQuest2 SET QUEST_DATA = ? WHERE CHAR_NAME = '%s'",szName);

	g_MyEventDB.WriteBlob(szQuery,lpMsg->m_UserData,sizeof(lpMsg->m_UserData));
}

struct PMSG_PCBANGUSER_CONNECT
{
	PBMSG_HEAD h;
	WORD aIndex;
	BYTE btUnk1;
	char szAccountID[11];
};

struct PMSG_PCBANG_USERDATA_GETINFO
{
	PBMSG_HEAD h;
	short aIndex;
	char szAccountId[11];
	short Points;
};

void EGAnsPCBangUserConnect(LPBYTE aRecv,int aIndex)
{
	PMSG_PCBANGUSER_CONNECT* lpMsg = (PMSG_PCBANGUSER_CONNECT*)aRecv;

	char szAccountId[MAX_IDSTRING+1];
	szAccountId[MAX_IDSTRING] = '\0';
	memcpy(szAccountId,lpMsg->szAccountID,MAX_IDSTRING);

	char szQuery[256];
	sprintf(szQuery,"SELECT pcbang_id,pcbang_points FROM PCBANG_USER WHERE pcbang_id = '%s'",szAccountId);

	PMSG_PCBANG_USERDATA_GETINFO pMsg ={0};
	PHeadSetB((LPBYTE)&pMsg,0x23,sizeof(pMsg));
	pMsg.aIndex = lpMsg->aIndex;
	pMsg.Points = 0;

	if( g_EventDB.Exec(szQuery) == TRUE )
	{
		if( g_EventDB.Fetch() != SQL_NO_DATA )
		{
			pMsg.Points = g_EventDB.GetInt("pcbang_points");
		}
		else
		{
			g_EventDB.Clear();
			sprintf(szQuery,"INSERT INTO PCBANG_USER (pcbang_id,pcbang_points,reset_date) VALUES ('%s',0,getdate()+7)",szAccountId);
			g_EventDB.Exec(szQuery);
		}
	}

	g_EventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

struct PMSG_PCBANGUSER_SENDINFO
{
	PBMSG_HEAD h;
	short aIndex;
	char szAccountID[11];
	BYTE btResult;
	short Point;
	/*short wYear;
	short wMonth;
	short wDay;
	short wHour;*/
};

void EGAnsPCBangUpdateUser(LPBYTE aRecv,int aIndex)
{
	PMSG_PCBANGUSER_SENDINFO* lpMsg = (PMSG_PCBANGUSER_SENDINFO*)aRecv;
	PMSG_PCBANGUSER_SENDINFO pMsg;

	char szAccountId[MAX_IDSTRING+1];

	memcpy(szAccountId,lpMsg->szAccountID,MAX_IDSTRING);
	szAccountId[MAX_IDSTRING] = '\0';

	PHeadSetB((LPBYTE)&pMsg,0x24,sizeof(pMsg));
	pMsg.aIndex = lpMsg->aIndex;
	pMsg.Point = lpMsg->Point;
	pMsg.btResult = 0;

	char szQuery[256];
	sprintf(szQuery,"UPDATE PCBANG_USER SET pcbang_points = %d WHERE pcbang_id = '%s'",
		lpMsg->Point,szAccountId);

	if( g_EventDB.Exec(szQuery) == TRUE )
	{
		pMsg.btResult = TRUE;
	}
	g_EventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));

}

void EGAnsEnterDoubleGoer(LPBYTE aRecv,int aIndex)
{
	LPMSG_DOUBLEGOER_REQ_ENTER_EVENT lpMsg = (LPMSG_DOUBLEGOER_REQ_ENTER_EVENT)aRecv;


	PMSG_DOUBLEGOER_ANS_ENTER_EVENT pMsg;

	PHeadSetB((LPBYTE)&pMsg,0x40,sizeof(pMsg));
	pMsg.btPos = lpMsg->btPos;
	pMsg.aIndex = lpMsg->aIndex;
	pMsg.iResult = 0;

	char szQuery[512];

	wsprintf(szQuery,"SELECT DATEDIFF(mi,getdate(),NEXT_EVENT) as LeftHour FROM DG_ROOM WHERE CHAR_NAME = '%s' AND NEXT_EVENT > getdate()",lpMsg->szName);

	if( g_EventDB.Exec(szQuery) != FALSE )
	{
		if( g_EventDB.Fetch() != SQL_NO_DATA )
		{
			pMsg.iResult = g_EventDB.GetInt("LeftHour");
		}
	}
	else
	{
		pMsg.iResult = -1;
	}

	g_EventDB.Clear();


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGUpdateDoubleGoer(LPBYTE aRecv,int aIndex)
{
	LPMSG_DOUBLEGOER_UPDATE_EVENT lpMsg = (LPMSG_DOUBLEGOER_UPDATE_EVENT)aRecv;

	char szName[MAX_IDSTRING+1];
	memcpy(szName,lpMsg->szName,MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	char szQuery[512];

	wsprintf(szQuery,"SELECT * FROM DG_ROOM WHERE CHAR_NAME = '%s'",szName);

	if( g_EventDB.Exec(szQuery) != FALSE )
	{
		if( g_EventDB.Fetch() == SQL_NO_DATA )
		{
			g_EventDB.Clear();
			wsprintf(szQuery,"INSERT INTO DG_ROOM (CHAR_NAME,NEXT_EVENT) VALUES ('%s',DATEADD(DAY,1,GETDATE()))",szName);
			g_EventDB.Exec(szQuery);
		}
		else
		{
			g_EventDB.Clear();
			wsprintf(szQuery,"UPDATE DG_ROOM SET NEXT_EVENT = DATEADD(DAY,1,GETDATE()) WHERE CHAR_NAME = '%s'",szName);
			g_EventDB.Exec(szQuery);
		}
	}

	g_EventDB.Clear();
}

void UpdateConnectState(PMSG_UPDATE_CONNECT_STAT* lpMsg)
{
	char szName[MAX_IDSTRING+1];
	memcpy(szName,lpMsg->szAccount,MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	char szQuery[512];

	wsprintf(szQuery,"UPDATE MEMB_STAT SET ConnectStat = 2 WHERE memb___id = '%s'",szName);
	g_MyEventDB.Exec(szQuery);
	g_MyEventDB.Clear();
}

void LoadNoblessInfo(LPMSG_REQ_NOBLESSINFO lpMsg,int aIndex)
{
	char szName[MAX_IDSTRING+1];
	memcpy(szName,lpMsg->szName,MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	PMSG_ANS_NOBLESSINFO pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x66;
	pMsg.h.subcode = 0x10;

	pMsg.aIndex = lpMsg->aIndex;
	memcpy(pMsg.szName,szName,MAX_IDSTRING);

	pMsg.btResult = TRUE;

	char szQuery[512];
	sprintf(szQuery,"SELECT NoblessRank,NoblessPts FROM NoblessInfo WHERE CHAR_NAME = '%s'",
		szName);

	if( !g_MyEventDB.Exec(szQuery) )
	{
		pMsg.btResult = FALSE;
	}
	else
	{
		if( g_MyEventDB.Fetch() == SQL_NO_DATA )
		{
			g_MyEventDB.Clear();

			sprintf(szQuery,"INSERT INTO NoblessInfo (CHAR_NAME,NoblessRank,NoblessPts) VALUES ('%s',0,0)",
				szName);

			g_MyEventDB.Exec(szQuery);

			pMsg.NoblessRank = 0;
			pMsg.NoblessPts = 0;
		}
		else
		{
			pMsg.NoblessRank = g_MyEventDB.GetInt("NoblessRank");
			pMsg.NoblessPts = g_MyEventDB.GetInt("NoblessPts");
		}
	}

	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void SaveNoblessInfo(LPMSG_REQ_SAVENOBLESSINFO lpMsg)
{
	char szName[MAX_IDSTRING+1];
	memcpy(szName,lpMsg->szName,MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	char szQuery[512];
	sprintf(szQuery,"UPDATE NoblessInfo SET NoblessRank = %d, NoblessPts = %d WHERE CHAR_NAME = '%s'",
		lpMsg->NoblessRank,lpMsg->NoblessPts,lpMsg->szName);

	g_MyEventDB.Exec(szQuery);
	g_MyEventDB.Clear();
}

void ReqSellFullItem(LPMSG_REQ_SELLFULLOPT lpMsg,int aIndex)
{
	char szName[MAX_IDSTRING+1];
	memcpy(szName,lpMsg->szAccount,MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	char szQuery[512];
	sprintf(szQuery,"exec WZ_SellFullItem '%s', '%08X'",
		szName,lpMsg->dwSerial);

	g_MyEventDB.Exec(szQuery);
	g_MyEventDB.Clear();
}

void LoadOlympInfo(LOLP_REQ_OLYMPINITDATA lpMsg,int aIndex)
{
	OLP_ANS_OLYMPINITDATA pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x68;
	pMsg.h.subcode = 0x00;

	pMsg.wMapSvrNum = lpMsg->wMapSvrNum;
	pMsg.iResult = FALSE;


	char szQuery[512];
	sprintf(szQuery,"EXEC WZ_OG_GetOlympTotalInfo %d, %d",
		lpMsg->wMapSvrNum,lpMsg->iOlympEventCycle);

	if( g_MyEventDB.Exec(szQuery) == TRUE && g_MyEventDB.Fetch() != SQL_NO_DATA )
	{
		pMsg.wStartYear = (WORD)g_MyEventDB.GetInt("SYEAR");
		pMsg.btStartMonth = (BYTE)g_MyEventDB.GetInt("SMONTH");
		pMsg.btStartDay = (BYTE)g_MyEventDB.GetInt("SDAY");
		pMsg.wEndYear = (WORD)g_MyEventDB.GetInt("EYEAR");
		pMsg.btEndMonth = (BYTE)g_MyEventDB.GetInt("EMONTH");
		pMsg.btEndDay = (BYTE)g_MyEventDB.GetInt("EDAY");
		pMsg.btFirstCreate = g_MyEventDB.GetInt("FIRST_CREATE");
		pMsg.iResult = TRUE;
	}

	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void ReqOlympChangeDate(LPOLP_REQ_SDEDCHANGE lpMsg,int aIndex)
{
	OLP_ANS_SDEDCHANGE pMsg;

	pMsg.h.c = PMHC_BYTE;
	pMsg.h.size = sizeof( pMsg );
	pMsg.h.headcode = 0x68;
	pMsg.h.subcode = 0x01;

	pMsg.iIndex = lpMsg->iIndex;
	pMsg.wMapSvrNum = lpMsg->wMapSvrNum;

	pMsg.wStartYear = lpMsg->wStartYear;
	pMsg.btStartMonth = lpMsg->btStartMonth;
	pMsg.btStartDay = lpMsg->btStartDay;
	pMsg.wEndYear = lpMsg->wEndYear;
	pMsg.btEndMonth = lpMsg->btEndMonth;
	pMsg.btEndDay = lpMsg->btEndDay;

	pMsg.iResult = FALSE;

	char szQuery[512];
	
	sprintf(szQuery,"EXEC WZ_OG_ModifyOlympSchedule %d, '%d-%d-%d 00:00:00', '%d-%d-%d 00:00:00'",
		lpMsg->wMapSvrNum,lpMsg->wStartYear,lpMsg->btStartMonth,lpMsg->btStartDay,
		lpMsg->wEndYear,lpMsg->btEndMonth,lpMsg->btEndDay);

	if( g_MyEventDB.Exec(szQuery) == TRUE && g_MyEventDB.Fetch() != SQL_NO_DATA )
	{
		pMsg.iResult = g_MyEventDB.GetInt("QueryResult");
	}
	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void ReqRegOlymp(LPOLP_REQ_REGFIGHT lpMsg,int aIndex)
{
	char szName[MAX_IDSTRING+1];
	memcpy(szName,lpMsg->szName,MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	OLP_ANS_REGFIGHT pMsg;
	pMsg.h.c = PMHC_BYTE;
	pMsg.h.size = sizeof( pMsg );
	pMsg.h.headcode = 0x68;
	pMsg.h.subcode = 0x02;
	pMsg.iIndex = lpMsg->iIndex;
	memcpy(pMsg.szName,szName,MAX_IDSTRING);

	pMsg.iBattleCount = 0;
	pMsg.iBattlePts = 0;
	pMsg.btResult = 0;

	char szQuery[512];
	sprintf(szQuery,"EXEC WZ_OG_UserInfo %d, '%s',%d",lpMsg->wMapSvrNum,szName,lpMsg->iClass);

	if( g_MyEventDB.Exec(szQuery) && g_MyEventDB.Fetch() != SQL_NO_DATA )
	{
		pMsg.btResult = TRUE;
		pMsg.iBattleCount = g_MyEventDB.GetInt("BATTLE_COUNT");
		pMsg.iBattlePts = g_MyEventDB.GetInt("BATTLE_POINTS");
		pMsg.iLastRegTime = g_MyEventDB.GetInt("LAST_REG");
	}
	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void OlympEndFight(LPOLP_ENFIGHT lpMsg,int aIndex)
{
	char szName1[MAX_IDSTRING+1];
	memcpy(szName1,lpMsg->szWinner,MAX_IDSTRING);
	szName1[MAX_IDSTRING] = '\0';

	char szName2[MAX_IDSTRING+1];
	memcpy(szName2,lpMsg->szLooser,MAX_IDSTRING);
	szName2[MAX_IDSTRING] = '\0';

	char szQuery[512];
	sprintf(szQuery,"EXEC WZ_OG_EndFight %d,'%s','%s',%d",
		lpMsg->wMapSvrNum,szName1,szName2,lpMsg->iEndResult);

	g_MyEventDB.Exec(szQuery);
	g_MyEventDB.Clear();
}

void OlympResetPeriod(LPOLP_REQ_RESET lpMsg,int aIndex)
{
	char szQuery[512];
	sprintf(szQuery,"EXEC WZ_OG_ResetPeriod	%d",lpMsg->wMapSvrNum);

	g_MyEventDB.Exec(szQuery);
	g_MyEventDB.Clear();
}

void OlympRewardRequest(LPOLP_REQ_REWARD lpMsg,int aIndex)
{
	char szName[MAX_IDSTRING+1];
	memcpy(szName,lpMsg->szName,MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	OLP_ANS_REWARD pMsg;
	pMsg.h.c = PMHC_BYTE;
	pMsg.h.headcode = 0x68;
	pMsg.h.size = sizeof( pMsg );
	pMsg.h.subcode = 0x05;

	pMsg.iIndex = lpMsg->iIndex;
	memcpy(pMsg.szName,szName,10);

	pMsg.btResult = 0;
	pMsg.btRank = 0;
	pMsg.btReward = 0;

	char szQuery[512];
	sprintf(szQuery,"EXEC WZ_OG_UserReward %d, '%s'",
		lpMsg->wMapSvrNum,szName);

	if( g_MyEventDB.Exec(szQuery) == TRUE && g_MyEventDB.Fetch() != SQL_NO_DATA )
	{
		pMsg.btResult = g_MyEventDB.GetInt("qResult");
		pMsg.btRank = g_MyEventDB.GetInt("USER_RANK");
		pMsg.btReward = g_MyEventDB.GetInt("IS_REWARDED");
	}

	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void OlympStatsRequest(LPOLP_REQ_STATS lpMsg,int aIndex)
{
	char szName[MAX_IDSTRING+1];
	memcpy(szName,lpMsg->szName,MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	OLP_ANS_STATS pMsg;
	pMsg.h.c = PMHC_BYTE;
	pMsg.h.headcode = 0x68;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.subcode = 0x06;

	pMsg.iIndex = lpMsg->iIndex;
	memcpy(pMsg.szName,szName,MAX_IDSTRING);

	pMsg.btResult = FALSE;
	pMsg.iBattles = 0;
	pMsg.iPts = 0;
	

	char szQuery[512];
	sprintf(szQuery,"SELECT BATTLE_COUNT,BATTLE_POINTS FROM OLYMP_USERS WHERE CHAR_NAME = '%s' AND MAP_SVR_GROUP = %d",
		szName,lpMsg->wMapSvrNum);

	if( g_MyEventDB.Exec(szQuery) && g_MyEventDB.Fetch() != SQL_NO_DATA )
	{
		pMsg.btResult = TRUE;
		pMsg.iBattles = g_MyEventDB.GetInt("BATTLE_COUNT");
		pMsg.iPts = g_MyEventDB.GetInt("BATTLE_POINTS");
	}
	g_MyEventDB.Clear();

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}