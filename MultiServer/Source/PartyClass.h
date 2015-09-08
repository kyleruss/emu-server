#if !defined(__PARTYCLASS_H__)
#define __PARTYCLASS_H__

#include "CQuery.h"
#include "Server.h"
#include "zMultiServer.h"

extern CQuery g_PartyMatchDB;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	int ServerCode;
	char szName[11];
	char szDesc[41];
	char szPass[5];
	short MinLevel;
	short MaxLevel;
	short MoveReqId;
	short ClassInfo;
	BYTE btAutoAccept;
	BYTE Gens;
}PMSG_PARTYMATCHING_REG_REQ,*LPMSG_PARTYMATCHING_REG_REQ;

typedef struct
{
	PBMSG_HEAD2 h;
	int iResult;
	int aIndex;
	char szName[11];
}PMSG_PARTYMATCHING_REG_ANS,*LPMSG_PARTYMATCHING_REG_ANS;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
	int RequestPage;
	char szSearch[11];
	BYTE ReqType;
	short Level;
	short Class;
	short Gens;
}PMSG_PARTYMATCHING_INFO_REQ,*LPMSG_PARTYMATCHING_INFO_REQ;

struct PARTYMATCH_INFO
{
	char szLeader[11];
	char szDescription[41];
	short LeaderLevel;
	BYTE LeaderClass;
	BYTE LeaderChannel;
	BYTE btPartyCount;
	BYTE btIsPassword;
	BYTE btClass;
	short MinLevel;
	short MaxLevel;
	short MoveReqId;
	short Gens;
};

typedef struct
{
	PWMSG_HEAD2 h;
	int aIndex;
	char szName[11];
	int iResult;
	int count;
	int page;
	int totalpage;
}PMSG_PARTYMATCHING_INFO_ANS,*LPMSG_PARTYMATCHING_INFO_ANS;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
	char szPartyLeader[11];
	char szPassword[5];
	short ServerCode;
	short RequestType;
	short Level;
	short Class;
	short Gens;
}PMSG_PARTYMATCHING_JOIN_REQ,*LPMSG_PARTYMATCHING_JOIN_REQ;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
	int iResult;
}PMSG_PARTYMATCHING_JOIN_ANS,*LPMSG_PARTYMATCHING_JOIN_ANS;

typedef struct
{
	PBMSG_HEAD2 h;
	char szPartyLeader[11];
}PMSG_PARTYMATCHING_JOIN_NOTIFY,*LPMSG_PARTYMATCHING_JOIN_NOTIFY;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
}PMSG_PARTYMATCHING_WAIT_REQ,*LPMSG_PARTYMATCHING_WAIT_REQ;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
	int iResult;
	char szPartyLeader[11];
	int ServerCode;
}PMSG_PARTYMATCHING_WAIT_ANS,*LPMSG_PARTYMATCHING_WAIT_ANS;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
	BYTE btType;
}PMSG_PARTYMATCHING_CANCEL_REQ,*LPMSG_PARTYMATCHING_CANCEL_REQ;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
	int iResult;
	BYTE btType;
}PMSG_PARTYMATCHING_CANCEL_ANS,*LPMSG_PARTYMATCHING_CANCEL_ANS;

#pragma pack(push, 1)
struct PARTYMATCH_GDREQ_REQUESTLIST
{
	PBMSG_HEAD2 h;
	int UserIndex;
	char PartyLeader[11];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PARTYMATCH_DGANS_REQUESTLIST_COUNT
{
	PWMSG_HEAD2 h;
	int UserIndex;
	int Count;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PARTYMATCH_DGANS_REQUESTLIST_INFO
{
	char Character[11];
	BYTE Class;
	int Level;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PARTYMATCH_GDREQ_REQUESTANSWER
{
	PBMSG_HEAD2 h;
	int UserIndex;
	char Character[11];
	int Type;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PARTYMATCH_DGANS_REQUESTANSWER
{
	PBMSG_HEAD2 h;
	int UserIndex;
	char Character[11];
	int Type;
	int Result;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PARTYMATCH_GDREQ_UPDATESTATUS
{
	PBMSG_HEAD2 h;
	char Character[11];
	BYTE Status;
};
#pragma pack(pop)

void DGPartyMatchRegister(LPMSG_PARTYMATCHING_REG_REQ lpMsg,int aIndex);
void DGPartyMatchInfo(LPMSG_PARTYMATCHING_INFO_REQ lpMsg,int aIndex);
void DGPartyMatchJoin(LPMSG_PARTYMATCHING_JOIN_REQ lpMsg,int aIndex);
void DGPartyMatchWaitInfo(LPMSG_PARTYMATCHING_WAIT_REQ lpMsg,int aIndex);
void DGPartyMatchCancel(LPMSG_PARTYMATCHING_CANCEL_REQ lpMsg,int aIndex);
void DGPartyMatchReqList(PARTYMATCH_GDREQ_REQUESTLIST* lpData, int aIndex);
void DGPartyMatchRequestAnswer(PARTYMATCH_GDREQ_REQUESTANSWER* lpData, int aIndex);
void GDPartyMatchUpdateStatus(PARTYMATCH_GDREQ_UPDATESTATUS* lpData);

int PartyMatchRegister(LPMSG_PARTYMATCHING_REG_REQ lpMsg);
int PartyMatchInfo(LPMSG_PARTYMATCHING_INFO_REQ lpMsg,PARTYMATCH_INFO* pBodyMsg,int& count,int& pages);
int PartyMatchJoin(LPMSG_PARTYMATCHING_JOIN_REQ lpMsg);
int PartyMatchCancel(char* szName,int Type);

void PartyMatchUpdateServerInfo(int ServerCode, LPSTR szName);
void PartyMatchUpdateStatus(LPSTR szName, BYTE Status);

#endif