#ifndef __JOINSERVER_H__
#define __JOINSERVER_H__
#define JS_INISECTION "JoinServer"
#include "CQuery.h"
#include "Server.h"
#include "zMultiServer.h"

#include <map>
#include <iostream>
#include <string>
using namespace std;

#define MAX_USEROBJECT 25000

class CJoinServerData
{
	struct JS_DATA
	{
		int m_Index;
		WORD m_wCurrentUser;
		WORD m_wMaxUser;
	};

public:
	CJoinServerData() { Init();};
	virtual ~CJoinServerData() { m_SvrCodeMap.clear();DeleteCriticalSection(&m_critJoinUser); };

	void Init() {
		m_SvrCodeMap.clear();
		InitializeCriticalSection(&m_critJoinUser);
	};

	int SearchServer(WORD wCode)	{
		JS_DATA * pData = GetData(wCode);

		if(pData==NULL)
			return -1;

	return pData->m_Index;
	};

	void Del(int index)
	{
		for(TSvrCodeMap::iterator itor = m_SvrCodeMap.begin(); itor != m_SvrCodeMap.end(); itor++)
		{
			if(itor->second.m_Index == index)
			{
				m_SvrCodeMap.erase(itor);
				break;
			}
		}
	};

	void Set(int index,WORD wCode)
	{
		JS_DATA * pData = GetData(wCode);

		if(pData != NULL)
		{
			pData->m_Index = index;
			return;
		}

		JS_DATA Data = {0};
		Data.m_Index = index;

		m_SvrCodeMap[wCode] = Data;
	}

	void SetInfo(WORD wCode,WORD wCur,WORD wMax)
	{
		JS_DATA * pData = GetData(wCode);

		if(pData != NULL)
		{
			pData->m_wCurrentUser = wCur;
			pData->m_wMaxUser = wMax;
		}
	}

	BOOL CheckFreeSpace(WORD wCode)	{
		return TRUE;
	};

	CRITICAL_SECTION	m_critJoinUser;

private:

	JS_DATA * GetData(WORD wCode)
	{
		TSvrCodeMap::iterator itor = m_SvrCodeMap.find(wCode);
		if( itor == m_SvrCodeMap.end() )
		{
			return NULL;
		}

	return &itor->second;
	}

	typedef map<WORD,JS_DATA> TSvrCodeMap;
	TSvrCodeMap	m_SvrCodeMap;
};

extern CJoinServerData g_JSData;

struct JSInfo
{
	WORD TCPPort;
	WORD UDPPort;
	char UDPIp[17];
	char dbDSN[64];
	char dbDSN2[64];
	int bMD5;
	int bAutoreg;
	BYTE AutoRegMinLength;
	int bPremiumBlock;
};

class CWZServerState  
{
	BOOL		m_bLive;
	HANDLE		m_hThread;

	DWORD		S_TYPE;
public:
	CWZServerState();
	virtual ~CWZServerState();

	void	Init(DWORD S_TYPE);

	DWORD	Run();
};

extern CWZServerState g_WZServerState;

void gObjAllDelete(int index);
void gObjDel(int index, int dbnumber);
void gObjUserInit();
void gObjClear();
int gObjSearchUser(char * szId);
extern int gObjAdd(char *szId, char *szPass, int aDBNumber, short aGameServerIndex, char *IpAddress, int gIndex);


DWORD MakeAccountKey(LPTSTR lpszAccountID);
void JoinServerInit();
void UdpProtocolCore(BYTE protoNum, BYTE *aRecv, int aLen, char *ip);
void JSProtocolCore(int index, DWORD Head, LPBYTE aRecv, int Len);

// Protocol Definitions

typedef struct
{
	PBMSG_HEAD	h;		
	int			iQueueCount;
} PMSG_JOINSERVER_STAT, *LPPMSG_JOINSERVER_STAT;

// Headcode [0x01]
typedef struct
{
	PBMSG_HEAD	h;

	char		Id[MAX_IDSTRING];
	char		Pass[20];
	short		Number;
	char		IpAddress[17];
} SDHP_IDPASS, *LPSDHP_IDPASS;

typedef struct
{
	PBMSG_HEAD	h;	
	BYTE		result;
	
	short		Number;
	char		Id[MAX_IDSTRING];
	int			UserNumber;
	int			DBNumber;
	char		JoominNumber[20];
} SDHP_IDPASSRESULT, *LPSDHP_IDPASSRESULT;

// Headcode [0x02]
typedef struct
{
	PBMSG_HEAD	h;

	short		Number;
	char		Id[MAX_IDSTRING+1];
	int			UserNumber;
	int			DBNumber;
} SDHP_JOINFAIL, * LPSDHP_JOINFAIL;
// Headcode [0x04]
typedef struct
{
	PBMSG_HEAD	h;

	short		Number;
	char		Id[MAX_IDSTRING+1];
	int			UserNumber;
	int			DBNumber;
	BYTE		BlockCode;
} SDHP_COMMAND_BLOCK, * LPSDHP_COMMAND_BLOCK;
// Headcode [0x05]
typedef struct 
{
	PBMSG_HEAD	h;
	char szId[MAX_IDSTRING];
	WORD Level;
	BYTE DbClass;
} SDHP_USERCLOSE_ID, *LPSDHP_USERCLOSE_ID;



// Headcode [0x06]
typedef struct
{
	PBMSG_HEAD	h;
	
	char		Id[MAX_IDSTRING];
	short		Number;
} SDHP_BILLSEARCH, * LPSDHP_SDHP_BILLSEARCH;

typedef struct
{
	PBMSG_HEAD	h;
	
	char		Id[MAX_IDSTRING];
	short		Number;
	int			PayCode;
	int			EndTime;

	char EndDays[13];
} SDHP_BILLSEARCH_RESULT, * LPSDHP_SDHP_BILLSEARCH_RESULT;

// Headcode [0x30]
typedef struct
{
	PBMSG_HEAD	h;
	char		Account[MAX_IDSTRING];
	char		Name[MAX_IDSTRING];
} SDHP_LOVEHEARTEVENT, * LPSDHP_LOVEHEARTEVENT;

typedef struct
{
	PBMSG_HEAD	h;

	BYTE		Result;

	char		Name[MAX_IDSTRING];
	int			Number;
	
} SDHP_LOVEHEARTEVENT_RESULT, * LPSDHP_LOVEHEARTEVENT_RESULT;

// Headcode [0x31]
typedef struct 
{
	PBMSG_HEAD	h;
	BYTE		x;
	BYTE		y;
	BYTE		MapNumber;
} SDHP_LOVEHEARTCREATE, *LPSDHP_LOVEHEARTCREATE;

typedef struct 
{
	PBMSG_HEAD	h;
} SDHP_LOVEHEARTCREATERECV, *LPSDHP_LOVEHEARTCREATERECV;

// Headcode [0x7A]
typedef struct PMSG_REQ_MAPSVRMOVE
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_IDSTRING+1];
	char szCharName[MAX_IDSTRING+1];
	WORD wCurMapSvrCode;
	WORD wDstMapSvrCode;
	WORD wMapNumber;
	BYTE btX;
	BYTE btY;
} PMSG_REQ_MAPSVRMOVE, *LPMSG_REQ_MAPSVRMOVE;

typedef struct
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_IDSTRING+1];
	char szCharName[MAX_IDSTRING+1];
	WORD wCurMapSvrCode;
	WORD wDstMapSvrCode;
	WORD wMapNumber;
	BYTE btX;
	BYTE btY;
	int iResult;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
}PMSG_ANS_MAPSVRMOVE, *LPMSG_ANS_MAPSVRMOVE;

// Headcode [0x7B]
typedef struct
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_IDSTRING+1];
	char szCharName[MAX_IDSTRING+1];
	unsigned short wDstMapSvrCode;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
}PMSG_REQ_MAPSVRAUTH, *LPMSG_REQ_MAPSVRAUTH;

typedef struct
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_IDSTRING+1];
	char szCharName[MAX_IDSTRING+1];
	WORD wPrevMapSvrCode;
	WORD wMapNumber;
	BYTE btX;
	BYTE btY;
	int iResult;
	int iUserNumber;
	int iDBNumber;
	char cJoominNumber[20];
	unsigned char btBlockCode;
}PMSG_ANS_MAPSVRAUTH, *LPMSG_ANS_MAPSVRAUTH;

// Headcode [0x7C]

typedef struct
{
	PBMSG_HEAD h;
	int iSvrCode;	// 4
	int iMaxUserCount;	// 8
	int iCurUserCount;	//	C
}PMSG_NOTIFY_MAXUSER, *LPMSG_NOTIFY_MAXUSER;

struct SDHP_BILLKILLUSER
{
	PBMSG_HEAD h;
	char Id[10];
	short Number;
};

typedef struct 
{
	PBMSG_HEAD h;
	char szAccount[10];
	char Ip[16];
}PMSG_SENDJSCLIENTIP, *LPMSG_SENDJSCLIENTIP;


struct BroadCastMessageInfo
{
	PWMSG_HEAD	h;
	BYTE		Type;
	BYTE		SenderChannel;
	char		Sender[10];
	char		Text[90];
};

// Protocol Functions
void JoinServerLogin(LPSDHP_SERVERINFO lpMsg,int aIndex);
void GJJoinIdPassRequest(LPSDHP_IDPASS lpMsg,int aIndex);
void GJPUserJoinFail(LPSDHP_JOINFAIL lpMsg, int aIndex);
void GJPUserBlock(LPSDHP_COMMAND_BLOCK lpMsg, int aIndex);
void GJPUserCloseIDMsg(LPSDHP_USERCLOSE_ID lpMsg, int aIndex);
void GJPUserBillCheck(LPSDHP_SDHP_BILLSEARCH lpMsg, int aIndex);
void LoveHeartEventRecv(LPSDHP_LOVEHEARTEVENT lpMsg, int aIndex);
void GDHeartCreate(LPSDHP_LOVEHEARTCREATE lpMsg, int aIndex);
void GJReqMapSvrMove(LPMSG_REQ_MAPSVRMOVE aRecv,int aIndex);
void GJReqMapSvrAuth(LPMSG_REQ_MAPSVRAUTH aRecv,int aIndex);
void GJNotifyMaxUserCount(LPMSG_NOTIFY_MAXUSER aRecv,int aIndex);
void GJNotifyUserIp(LPMSG_SENDJSCLIENTIP aRecv);

void BroadCastMessage(BroadCastMessageInfo* lpData, int aIndex);

extern JSInfo g_JSInfo;

typedef struct
{
	int number;
	int PayCode;
	int EndTime;
} BillStruct, * LPBillStruct;

typedef struct
{
	BOOL Connected;

	int	  Index;

	char  Id[MAX_IDSTRING+1];
	char  Pass[MAX_IDSTRING+1];
	char  IpAddress[17];
	int   DBNumber;
	short GameServerIndex;
	int   gaIndex;

	char JoomingNumber[20];
	char bloc_code;

	int m_bMoveReq;
	LPMSG_ANS_MAPSVRMOVE pMoveInfo;

	LPBillStruct lpBill;
} ObjectStruct, * LPObjectStruct;

extern void BillCheckProc();

typedef map<string,ObjectStruct*> TObjectMap;

//extern TObjectMap		gObjectMap;
extern ObjectStruct*		gObj;

#endif