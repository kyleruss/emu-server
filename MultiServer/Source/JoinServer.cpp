#include "stdafx.h"
#include "JoinServer.h"
#include "WzUdp.h"
#include "JoinServerDB.h"
#include "winutil.h"
#include <map>
#include "Window.h"


CJoinServerData g_JSData;
DWORD gObjCount = NULL;
JSInfo g_JSInfo;
CWZServerState g_WZServerState;
ObjectStruct*		gObj;
//TObjectMap		gObjectMap;


DWORD MakeAccountKey(LPTSTR lpszAccountID)
{
	int len = strlen(lpszAccountID);

	if ( len <= 0 )
		return 0;

	DWORD dwKey = 0;

	for(int i=0;i<len;i++)
		dwKey += lpszAccountID[i] + 17;
	
	dwKey = (dwKey + (10 - len) * 17) % 256;
	return dwKey % 256;
}

DWORD	WINAPI	ServerStateWorker(void* p)
{
	CWZServerState* WZServerState = (CWZServerState*)p;
	return WZServerState->Run();
}

CWZServerState::CWZServerState()
{
	m_bLive = TRUE;	
}

CWZServerState::~CWZServerState()
{
	m_bLive = FALSE;
	WaitForSingleObject(m_hThread, INFINITE);
}

void CWZServerState::Init(DWORD ST_TYPE)
{
	DWORD	ThreadID;

	S_TYPE = ST_TYPE;

	if ((m_hThread = CreateThread(NULL, 0, ServerStateWorker, this,
			0, &ThreadID)) == NULL)
	{
		g_Window.ServerLogAdd(S_TYPE,"CreateThread() [Bill Check] failed with error %d", GetLastError());	
	}
}

DWORD CWZServerState::Run()
{
	PMSG_JOINSERVER_STAT msg;

	msg.h.size		= sizeof( msg );
	msg.h.c			= PMHC_BYTE;
	msg.h.headcode	= 0x02;	

	while(m_bLive)
	{	// Sending joinserver status to ConnectServer
		msg.iQueueCount = WzRecvQ.GetCount();
		g_WzUdp.SendData((LPBYTE)&msg, msg.h.size);
		Sleep(1000);
	}
	return TRUE;
}

void UdpProtocolCore(BYTE protoNum, BYTE *aRecv, int aLen, char *ip)
{
}

void JoinServerInit()
{
	g_bIsServer[ST_JOINSERVER] = TRUE;
	g_bIsServer[ST_JOINSERVER] = TRUE;

	g_JSInfo.TCPPort = GetPrivateProfileInt(JS_INISECTION,"TCPPort",55970,INI_FILE);
	g_JSInfo.UDPPort = GetPrivateProfileInt(JS_INISECTION,"UDPPort",55557,INI_FILE);
	g_JSInfo.bMD5 = GetPrivateProfileInt(JS_INISECTION,"UseMD5Encrypt",0,INI_FILE);
	g_JSInfo.bPremiumBlock = GetPrivateProfileInt(JS_INISECTION,"PremiumBlock",0,INI_FILE);
	g_JSInfo.bAutoreg = GetPrivateProfileInt(JS_INISECTION,"AutoRegistration",0,INI_FILE);
	g_JSInfo.AutoRegMinLength = GetPrivateProfileInt(JS_INISECTION,"AutoRegistrationMinLength",0,INI_FILE);

	GetPrivateProfileString(JS_INISECTION,"UDPIp","127.0.0.1",g_JSInfo.UDPIp,sizeof(g_JSInfo.UDPIp),INI_FILE);
	GetPrivateProfileString(JS_INISECTION,"DSN","MuOnlineJoinDB",g_JSInfo.dbDSN,sizeof(g_JSInfo.dbDSN),INI_FILE);
	GetPrivateProfileString(JS_INISECTION,"DSN_CONNECT","MuOnline",g_JSInfo.dbDSN2,sizeof(g_JSInfo.dbDSN2),INI_FILE);

	g_JoinServerDB.S_TYPE = ST_JOINSERVER;
	g_JoinServerConnectDB.S_TYPE = ST_JOINSERVER;

	gObjUserInit();

	if(g_JoinServerDB.Connect(3,g_JSInfo.dbDSN,SQLUser,SQLPass) == TRUE)
	{
		g_Window.LogAdd(Log::Info, "[ %s ] connection successful !",g_JSInfo.dbDSN);
	}

	if( g_JoinServerConnectDB.Connect(3,g_JSInfo.dbDSN2,SQLUser,SQLPass) == TRUE )
	{
		g_Window.LogAdd(Log::Info, "[ %s ] connection successful !",g_JSInfo.dbDSN2);
	}

	if(CreateGIocp(g_JSInfo.TCPPort,ST_JOINSERVER) != TRUE)
	{
		g_Window.LogAdd(Log::Error, "Failed to start [ ST_JOINSERVER ] on port [ %d ]",g_JSInfo.TCPPort);
		return;
	}

	g_WzUdp.CreateSocket();
	g_WzUdp.SendSet(g_JSInfo.UDPIp,g_JSInfo.UDPPort,ST_JOINSERVER);
	g_WZServerState.Init(ST_JOINSERVER);

	g_Window.LogAdd(Log::Info, "[ JoinServer ] created on port [ %d ]",g_JSInfo.TCPPort);
}

void BillCheckProc()
{
	for(int i = 0; i < MAX_USEROBJECT; i++)
	{
		if(gObj[i].lpBill == NULL)
			continue;

		BillStruct * lpBill = gObj[i].lpBill;

		if( lpBill->PayCode > 0 && lpBill->EndTime > 0 )
		{
			lpBill->EndTime--;

			if(lpBill->EndTime <= 0)
			{
				RemoveBill(gObj[i].Id);
				delete gObj[i].lpBill;

				gObj[i].lpBill = NULL;
			}
		}
	}
}

void JSProtocolCore(int aIndex, DWORD headcode, LPBYTE aRecv, int Len)
{
	switch(headcode)
	{
	case 0x00:
		JoinServerLogin((LPSDHP_SERVERINFO)aRecv,aIndex);
		break;
	case 0x01:
		GJJoinIdPassRequest((LPSDHP_IDPASS)aRecv, aIndex);
		break;
	case 0x02 :
		GJPUserJoinFail((LPSDHP_JOINFAIL)aRecv, aIndex);
		break;
	case 0x04:
		GJPUserBlock((LPSDHP_COMMAND_BLOCK)aRecv, aIndex);
		break;
	case 0x05:
		GJPUserCloseIDMsg((LPSDHP_USERCLOSE_ID)aRecv, aIndex);
		break;
	case 0x06:
		GJPUserBillCheck((LPSDHP_SDHP_BILLSEARCH)aRecv, aIndex);
		break;
	case 0x30:
		LoveHeartEventRecv((LPSDHP_LOVEHEARTEVENT)aRecv, aIndex);
		break;
	case 0x31:
		GDHeartCreate((LPSDHP_LOVEHEARTCREATE)aRecv, aIndex);
		break;
	case 0x7A:
		GJReqMapSvrMove((LPMSG_REQ_MAPSVRMOVE)aRecv,aIndex);
		break;
	case 0x7B:
		GJReqMapSvrAuth((LPMSG_REQ_MAPSVRAUTH)aRecv,aIndex);
		break;
	case 0x7C:
		GJNotifyMaxUserCount((PMSG_NOTIFY_MAXUSER *) aRecv,aIndex);
		break;
	case 0x80:
		GJNotifyUserIp((LPMSG_SENDJSCLIENTIP)aRecv);
		break;
	case 0xCD:
		BroadCastMessage((BroadCastMessageInfo*)aRecv, aIndex);
		break;
	}
}

void JoinServerLogin(LPSDHP_SERVERINFO lpMsg,int aIndex)
{
	SDHP_RESULT pResult;

	pResult.h.c			= PMHC_BYTE;
	pResult.h.headcode	= 0x00;
	pResult.h.size		= sizeof(pResult);
	pResult.Result		= 0x01;

	g_JSData.Set(aIndex,lpMsg->ServerCode);

	if( gSObjSetInfo(aIndex, lpMsg->Port, lpMsg->Type, lpMsg->ServerName) == FALSE )
	{
		pResult.Result = 0x00;
		g_Window.ServerLogAdd(ST_JOINSERVER,"Client Authentication Fail");
	}

	gSObj[aIndex].ServerCode =  lpMsg->ServerCode;
	gSObj[aIndex].Flag = 1;
	DataSend(aIndex,(LPBYTE)&pResult,pResult.h.size);
}

void GJJoinIdPassRequest(LPSDHP_IDPASS lpMsgIdPass,int aIndex )
{
	char szId[MAX_IDSTRING+1];
#if (S6EP2 == 1 || EX700==1)
	char szPass[20+1];
#else
	char szPass[MAX_IDSTRING+1];
#endif
	char szIpAddress[17];
	char szJoominNumber[21]="";
	BYTE result=0x01;
	int  sbOfs=0;
	int  vNumber=0;
	int  UserNumber=-1;
	int  DBNumber;	
	SDHP_IDPASSRESULT	spResult;

	memset(szId,   0, sizeof(szId));
	memset(szPass, 0, sizeof(szPass));
	memset(szIpAddress, 0, sizeof(szIpAddress));

	memcpy(szId,lpMsgIdPass->Id,MAX_IDSTRING);
	memcpy(szPass,lpMsgIdPass->Pass,20);


	BuxConvert(szId, MAX_IDSTRING);
	BuxConvert(szPass, 20);

	char Block='0';

	if( SQLSyntexCheck(szId) == FALSE || SQLSyntexCheck(szPass) == FALSE || SpaceSyntexCheck(szId) == FALSE)
	{
		result = 0x02;
	}
	else
	{

		EnterCriticalSection(&g_JSData.m_critJoinUser);

		int nRet = IsUser(szId,szPass,szJoominNumber,Block,DBNumber);

		if( nRet == 0 )
		{
			result = 0;
		}

		if(gObjSearchUser(szId) == -1)
		{
			if( nRet == 1 && Block > '0' )
			{
				if( Block == 'A' )
				{
					result=0x0E;
				}
				else if( Block == 'B' )
				{
					result=0x0F;
				}
				else result=0x05;
			}
			else if(nRet == 1)
			{
				UserNumber = gObjAdd(szId,szPass,DBNumber,aIndex,lpMsgIdPass->IpAddress,lpMsgIdPass->Number);

				if(UserNumber < 0)
				{
					result = 0x04;
				}
				else
				{
					gObj[UserNumber].bloc_code = Block;
					memcpy(gObj[UserNumber].JoomingNumber,szJoominNumber,20);
					g_Window.ServerLogAdd(ST_JOINSERVER,"[ %s ] User login",szId);
				}
			}
			else if(nRet == 0)
			{
				result = 0x00;
			}
			else
			{
				result = 0x02;
			}
		}
		else
		{
			if( result == 0 )
			{
				result = 0x03;
			}
			else
			{
				result = 0x06;
			}

			g_Window.ServerLogAdd(ST_JOINSERVER,"[ %s ] [ERROR] User Connected login",szId);
		}

		LeaveCriticalSection(&g_JSData.m_critJoinUser);
	}

	spResult.h.size     = sizeof( spResult );
	spResult.h.c		= PMHC_BYTE;
	spResult.h.headcode = 0x01;
	spResult.result     = result;
	spResult.Number     = lpMsgIdPass->Number;
	spResult.UserNumber = UserNumber;
	spResult.DBNumber   = DBNumber;

	memcpy(spResult.JoominNumber, szJoominNumber, 20);
	memcpy(spResult.Id, szId, MAX_IDSTRING);

	DataSend(aIndex,(LPBYTE)&spResult,spResult.h.size);
}

void GJPUserJoinFail(LPSDHP_JOINFAIL lpMsg, int aIndex)
{
	char szAccount[MAX_IDSTRING];

	memcpy(szAccount,lpMsg->Id,MAX_IDSTRING);
	szAccount[MAX_IDSTRING] = 0;

	int n = gObjSearchUser(szAccount);
	if( n == -1 )
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"error : account not found. (%s) (%s %d)", szAccount, __FILE__,__LINE__);
		return;
	}
	gObjDel(n, gObj[n].DBNumber);
}

void GJPUserBlock(LPSDHP_COMMAND_BLOCK lpMsg, int aIndex)
{
	char szAccount[MAX_IDSTRING+1];
	int index = lpMsg->UserNumber;

	if( index < 0 || index > MAX_USEROBJECT ) 
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"(%s)(%d) = incorect index. (%d)", __FILE__, __LINE__, index);
		return;
	}

	if( gObj[index].DBNumber != lpMsg->DBNumber) 
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"error : incorect dbnumber Name:[%s],Ip[%s] %d/%d", gObj[index].Id, gObj[index].IpAddress, gObj[index].DBNumber, lpMsg->DBNumber);
		return;
	}

	memcpy(szAccount,lpMsg->Id,MAX_IDSTRING);
	szAccount[MAX_IDSTRING] = 0;

	AccountBlock(szAccount,lpMsg->BlockCode);
}

void GJPUserCloseIDMsg(LPSDHP_USERCLOSE_ID lpMsg, int aIndex)
{
	char szAccountId[MAX_IDSTRING+1];
	memcpy(szAccountId,lpMsg->szId,MAX_IDSTRING);
	szAccountId[MAX_IDSTRING] = 0;

	int n = gObjSearchUser(szAccountId);
	if( n == -1 )
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"error : account not found. (%s) (%s %d)", szAccountId, __FILE__,__LINE__);
		return;
	}

	gObjDel(n,gObj[n].DBNumber);
}

void GJPUserBillCheck(LPSDHP_SDHP_BILLSEARCH lpMsg, int aIndex)
{
	char szId[MAX_IDSTRING+1] = {0};
	memcpy(szId,lpMsg->Id,MAX_IDSTRING);

	SDHP_BILLSEARCH_RESULT pMsg;

	pMsg.h.c		= PMHC_BYTE;
	pMsg.h.size		= sizeof( pMsg );
	pMsg.h.headcode = 0x06;

	pMsg.Number = lpMsg->Number;
	strcpy(pMsg.Id,szId);

	pMsg.PayCode = GetBill(lpMsg->Number,szId,&pMsg);

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof( pMsg ));
}

void LoveHeartEventRecv(LPSDHP_LOVEHEARTEVENT lpMsg, int aIndex)
{
	char szAccount[MAX_IDSTRING+1];
	char szName[MAX_IDSTRING+1];

	SDHP_LOVEHEARTEVENT_RESULT	pMsg;
	
	szAccount[MAX_IDSTRING] = '\0';
	szName[MAX_IDSTRING]  = '\0';

	memcpy(szAccount, lpMsg->Account, MAX_IDSTRING);
	memcpy(szName,    lpMsg->Name, MAX_IDSTRING);
	
	int number = CreateHeart();
	
	pMsg.Result = 0x00;

	if( number < 0 ) return;

	int m_server;
	int usernumber;
	usernumber = gObjSearchUser(szAccount);

	if(usernumber>=0)
	{
		m_server = gObj[usernumber].GameServerIndex;

		HeartEventAll(number, szAccount, gSObj[m_server].ServerName, szName);

		if( LoveHeartEventCheck(number) == TRUE )
		{
			HeartEventLuck(number, szAccount, gSObj[m_server].ServerName, szName);
			pMsg.Result = 0x01;
			g_Window.ServerLogAdd(ST_JOINSERVER,"LoveHeartEvent Number : [%d] ID:[%s] SERVER:[%s]", number, szAccount, gSObj[m_server].ServerName);
		}
	}

	pMsg.h.c		= PMHC_BYTE;
	pMsg.h.size		= sizeof( pMsg );
	pMsg.h.headcode = 0x30;
	pMsg.Number		= number;
	memcpy(pMsg.Name, szName, MAX_IDSTRING);

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void GDHeartCreate(LPSDHP_LOVEHEARTCREATE lpMsg, int aIndex)
{
	CreateHeartAll();
}

void GJReqMapSvrMove(LPMSG_REQ_MAPSVRMOVE aRecv,int aIndex)
{
	char szId[MAX_IDSTRING+1];
	szId[MAX_IDSTRING] = 0;
	memcpy(szId,aRecv->szAccountID,MAX_IDSTRING);

	int result = 0;

	if(strlen(szId) < 1)	result = 1;

	PMSG_ANS_MAPSVRMOVE pResult;

	int UserIndex = -1;

	pResult.h.c		= PMHC_BYTE;
	pResult.h.size		= sizeof( pResult );
	pResult.h.headcode = 0x7A;

	pResult.iIndex = aRecv->iIndex;
	memcpy(pResult.szAccountID, aRecv->szAccountID, 10);
	memcpy(pResult.szCharName, aRecv->szCharName, 10);
	pResult.btX = aRecv->btX;
	pResult.btY = aRecv->btY;
	pResult.wMapNumber = aRecv->wMapNumber;
	pResult.wCurMapSvrCode = aRecv->wCurMapSvrCode;
	pResult.wDstMapSvrCode = aRecv->wDstMapSvrCode;

	pResult.iJoinAuthCode1 = GetTickCount();
	pResult.iJoinAuthCode2 = GetTickCount() + rand() % 10000;
	pResult.iJoinAuthCode3 = GetTickCount() + rand() % 777;
	pResult.iJoinAuthCode4 = GetTickCount() + rand() % 8911;

	int iOldServer = g_JSData.SearchServer(aRecv->wCurMapSvrCode);
	int iNewServer = g_JSData.SearchServer(aRecv->wDstMapSvrCode);

	if(result == 1)
	{

	}
	else if(iOldServer == -1 || iNewServer == -1)
	{
		result = 2;
		g_Window.ServerLogAdd(ST_JOINSERVER,"GJReqMapSvrMove() -> ServerIndex = -1 ( cur:%d | dest: %d) Account:(%s)",
			aRecv->wCurMapSvrCode,aRecv->wDstMapSvrCode,aRecv->szAccountID);
	}
	else
	{
		if(g_JSData.CheckFreeSpace(iNewServer) == FALSE)
		{
			result = 3;
			g_Window.ServerLogAdd(ST_JOINSERVER,"GJReqMapSvrMove -> Server(%d) UserLimit reached",aRecv->wDstMapSvrCode);
		}
		else
		{
			UserIndex = gObjSearchUser(szId);

			if(UserIndex == -1)
			{
				result = 1;
				g_Window.ServerLogAdd(ST_JOINSERVER,"GJReqMapSvrMove -> iUserIndex == -1 (%s)(%s)",aRecv->szAccountID,aRecv->szCharName);
			}
			else
			{
				if(gObj[UserIndex].m_bMoveReq == TRUE)
				{
					result = 4;
					g_Window.ServerLogAdd(ST_JOINSERVER,"GJReqMapSvrMove -> m_bRequestMapSvrMove == TRUE (%s)(%s)",aRecv->szAccountID,aRecv->szCharName);
				}
				else
				{
					gObj[UserIndex].m_bMoveReq = TRUE;
					gObj[UserIndex].pMoveInfo = new PMSG_ANS_MAPSVRMOVE;
					memcpy(gObj[UserIndex].pMoveInfo,&pResult,sizeof(PMSG_ANS_MAPSVRMOVE));
				}
			}
		}
	}

	pResult.iResult = result;

	DataSend(aIndex,(LPBYTE)&pResult,pResult.h.size);
}

void GJReqMapSvrAuth(LPMSG_REQ_MAPSVRAUTH aRecv,int aIndex)
{
	char szAccountID[MAX_IDSTRING+1];
	szAccountID[MAX_IDSTRING] = 0;
	memcpy(szAccountID, aRecv->szAccountID, MAX_IDSTRING);

	int result = 0;

	PMSG_ANS_MAPSVRAUTH pResult;

	pResult.h.c		= PMHC_BYTE;
	pResult.h.size		= sizeof( pResult );
	pResult.h.headcode = 0x7B;

	memcpy(pResult.szAccountID, aRecv->szAccountID, MAX_IDSTRING);
	memcpy(pResult.szCharName, aRecv->szCharName, MAX_IDSTRING);
	pResult.iIndex = aRecv->iIndex;

	int UserIndex = gObjSearchUser(szAccountID);

	if(UserIndex == -1)
	{
		result = 0x01;
	}
	else if(gObj[UserIndex].m_bMoveReq == TRUE && gObj[UserIndex].pMoveInfo != NULL)
	{
		LPMSG_ANS_MAPSVRMOVE pMoveInfo = gObj[UserIndex].pMoveInfo;

		if ( CHECK_LIMIT(pMoveInfo->wCurMapSvrCode, MAX_SERVEROBJECT) &&
			 CHECK_LIMIT(pMoveInfo->wDstMapSvrCode, MAX_SERVEROBJECT) )
		{
			if ( aRecv->iJoinAuthCode1 == pMoveInfo->iJoinAuthCode1 &&
				 aRecv->iJoinAuthCode2 == pMoveInfo->iJoinAuthCode2 &&
				 aRecv->iJoinAuthCode3 == pMoveInfo->iJoinAuthCode3 &&
				 aRecv->iJoinAuthCode4 == pMoveInfo->iJoinAuthCode4 )
			{
				gObj[UserIndex].m_bMoveReq = FALSE;
				pResult.btX = pMoveInfo->btX;
				pResult.btY = pMoveInfo->btY;
				pResult.wPrevMapSvrCode = pMoveInfo->wCurMapSvrCode;
				pResult.wMapNumber = pMoveInfo->wMapNumber;

				pResult.iUserNumber = UserIndex;
				pResult.iDBNumber = gObj[UserIndex].DBNumber;
				memcpy(pResult.cJoominNumber,gObj[UserIndex].JoomingNumber,20);
				pResult.btBlockCode = gObj[UserIndex].bloc_code;
			}
			else
			{
				result = 0x02;
			}
		}
		else
		{
			result = 0x03;
		}
	}
	else
	{
		result = 0x04;
	}

	pResult.iResult = result;

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

	g_Window.ServerLogAdd(ST_JOINSERVER,"GJReqMapSvrAuth() -> User(%s) Character(%s) result(%d)",
		pResult.szAccountID,pResult.szAccountID,pResult.iResult);
}

void GJNotifyMaxUserCount(LPMSG_NOTIFY_MAXUSER aRecv,int aIndex)
{
	int index = g_JSData.SearchServer(aRecv->iSvrCode);

	if(index != -1)
	{
		g_JSData.SetInfo(aRecv->iSvrCode,aRecv->iCurUserCount,aRecv->iMaxUserCount);
	}
}

void gObjDel(int index, int aDBNumber)
{
	if( index < 0 || index > MAX_USEROBJECT ) 
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"(%s)(%d) = incorect index. (%d)", __FILE__, __LINE__, index);
		return;
	}

	if( gObj[index].DBNumber != aDBNumber) 
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"error : incorect dbnumber Name:[%s],Ip[%s] %d/%d", gObj[index].Id, gObj[index].IpAddress, gObj[index].DBNumber, aDBNumber);
		return;
	}

	gObj[index].Connected = 0;

	if(gObj[index].pMoveInfo != NULL)
		delete gObj[index].pMoveInfo;

	gObj[index].pMoveInfo = NULL;

	if(gObj[index].lpBill != NULL)
		delete gObj[index].lpBill;

	gObj[index].lpBill = NULL;

	g_Window.ServerLogAdd(ST_JOINSERVER,"[ %s ][ %s ] Close User", gObj[index].IpAddress, gObj[index].Id);

	MembStat_Disconnect(gObj[index].Id);

	/*TObjectMap::iterator Itor = gObjectMap.find(gObj[index].Id);
	if( Itor != gObjectMap.end() )
	{
		gObjectMap.erase(Itor);
	}*/
}

void gObjUserInit()
{
	gObj = new ObjectStruct[MAX_USEROBJECT];

	for(int i = 0; i < MAX_USEROBJECT; i++)
	{
		memset(&gObj[i],0x00,sizeof(gObj[i]));
	}

	//gObjectMap.clear();
}

void gObjClear()
{
	delete[] gObj;
	//gObjectMap.clear();
}

int gObjSearchUser(char* szAccountID)
{
	/*TObjectMap::iterator Itor = gObjectMap.begin();

	for( ; Itor != gObjectMap.end(); Itor++)
	{
		const char* szAccount = &(*Itor).first[0];

		if( strcmp(szAccountID,szAccount) == NULL )
		{
			LPObjectStruct pObject = (*Itor).second;
			return pObject->Index;
		}
	}*/

	for(int i = 0; i < MAX_USEROBJECT; i++)
	{
		if( gObj[i].Connected )
		{
			//if( gObj[i].Id[0] == szAccountID[0] &&
			//	gObj[i].Id[1] == szAccountID[1] )
			//{
				if( !strcmpi(szAccountID,gObj[i].Id ) )
					return gObj[i].Index;
			//}
		}
	}

	return -1;
}

int gObjAdd(char *szId, char *szPass, int aDBNumber, short aGameServerIndex, char *IpAddress, int gIndex)
{
	int count=0, totalcount=0;
	
	count = gObjCount;

	while( true )
	{		
		if( gObj[count].Connected == 0 )
		{
			memset(&gObj[count], 0, sizeof(ObjectStruct));
			strcpy(gObj[count].Id, szId);
			strcpy(gObj[count].Pass, szPass);
			
			gObj[count].DBNumber			= aDBNumber;
			gObj[count].Connected			= 1;
			gObj[count].GameServerIndex		= aGameServerIndex;
			gObj[count].gaIndex             = gIndex;			

			gObj[count].Index             = count;
			
			strcpy(gObj[count].IpAddress, IpAddress);	
			//gObjectMap[szId] = &gObj[count];

			//gObjectMap.insert( std::pair<std::string,ObjectStruct*> ( szId, &gObj[count] ) );

			gObjCount++;
			if( gObjCount >= MAX_USEROBJECT ) gObjCount = 0;

			MembStat_Connect(gObj[count].Id, gSObj[aGameServerIndex].ServerName, IpAddress);
			return count;
		}
		count++;
		if( count > (MAX_USEROBJECT-1) ) count = 0;
		totalcount++;
		if( totalcount > (MAX_USEROBJECT-1 )) 
		{
			g_Window.ServerLogAdd(ST_JOINSERVER,"Error : No more User append [%s] %d",__FILE__, __LINE__ );
			return -1;
		}
	}

	return -1;
}

void gObjAllDelete(int index)
{
	for(int n = 0; n < MAX_USEROBJECT; n++)
	{
		if( gObj[n].Connected && (gObj[n].GameServerIndex == index) )
		{			
			gObjDel(n, gObj[n].DBNumber);
		}
	}
}

void GJNotifyUserIp(LPMSG_SENDJSCLIENTIP aRecv)
{
	char szAccount[MAX_IDSTRING+1];
	char szIp[16];

	memcpy(szAccount,aRecv->szAccount,MAX_IDSTRING);
	memcpy(szIp,aRecv->Ip,16);
	szAccount[MAX_IDSTRING] = '\0';
	szIp[16] = '\0';

	g_Window.ServerLogAdd(ST_JOINSERVER,"[%s] Ip [%s]",szAccount,szIp);
}

void BroadCastMessage(BroadCastMessageInfo* lpData, int aIndex)
{
	BroadCastMessageInfo lpResult = { 0 };
	lpResult.h.set((LPBYTE)&lpResult, 0xCD, sizeof(lpResult));
	lpResult.Type = lpData->Type;
	strcpy(lpResult.Text, lpData->Text);
	strcpy(lpResult.Sender, lpData->Sender);

	WORD SenderCode = gSObj[aIndex].ServerCode;
	WORD ChannelTemp = SenderCode / 20;
	WORD ChannelStart = ChannelTemp * 20;
	WORD ChannelEnd	= ChannelStart + 20;

	for( int i = 0; i < MAX_SERVEROBJECT; i++ )
	{
		if( gSObj[i].Connected == 2 && gSObj[i].Type == 1 && gSObj[i].Flag == 1 )
		{
			if(		gSObj[i].ServerCode >= ChannelStart 
				&&	gSObj[i].ServerCode < ChannelEnd )
			{
				DataSend(i, (LPBYTE)&lpResult, sizeof(BroadCastMessageInfo));
			}
		}
	}
}