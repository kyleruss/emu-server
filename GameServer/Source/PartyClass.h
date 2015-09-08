#ifndef PARTYCLASS_H
#define PARTYCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h"

#define MAX_USER_IN_PARTY 5
#define MAX_PARTY_DISTANCE_EFFECT	10

struct PARTY_STRUCT
{
	int Count;	// 0
	int Number[MAX_USER_IN_PARTY];	// 4
	int DbNumber[MAX_USER_IN_PARTY];	// 18
	short m_MaxLevel;	// 2C
	short m_MinLevel;	// 2E
	char m_PkLevel[MAX_USER_IN_PARTY];
	char m_PartyPkLevel;
	int m_PkCount;
};

struct PartyBonusByClass
{
	BYTE m_ClassTable[MAX_TYPE_PLAYER];
	int m_AddExp;
};

struct PartyBonusByNearby
{
	BYTE m_NearbyCount;
	int m_AddExp;
};

struct PartyBonusInfo
{
	std::vector<PartyBonusByNearby> m_NearbyBonus;
	std::vector<PartyBonusByClass> m_ClassBonus;
};

class PartyClass
{

public:

	PartyClass();
	virtual ~PartyClass();

	BOOL IsParty(int party_number);
	int Create(int usernumber, int dbnumber, int level);
	BOOL Destroy(int party_number);
	int Add(int party_number, int usernumber, int dbnumber,  int level);
	void Delete(int party_number, int index);
	int Delete(int party_number, int usernumber, int dbnumber);
	int GetCount(int party_number);
	int GetIndexUser(int party_number, int index, int& usernumber,  int& dbnumber);
	int GetPartyCount(int party_number);
	int GetIndex(int party_number, int usernumber, int dbnumber);
	BOOL SetLevel(int party_number, int level);
	BOOL GetLevel(int party_number, int& maxlevel, int& minlevel);
	BOOL Isleader(int party_number, int usernumber, int dbnumber);
	void Paint(int party_number);
	void PartyMemberLifeSend(int party_number);
	void PartyLog(int iPartyNumber,int iUserNumber,int iFlag);
	void UpdatePKPartyPanalty(int party_number);
	int GetReallyConnectPartyMemberCount(int party_number);
	void SwitchLeader(int party_number);
	void ChangeLeader(int party_number);

	char GetPartyPkLevel(int party_number);
	void SetPkLevel(int party_number, int usernumber, int dbnumber, BYTE PkLevel);
	void SetPkCount(int party_number);
	void ResetLeader(int party_number);

	void ReadBonusInfo(LPSTR File);
	int GetExpBonus(LPOBJ lpUser, LPOBJ lpMonster, int PartyNumber);
	bool SetLeader(int OldLeader, int NewLeader);

public:
	int m_PartyCount;	// 4
	PARTY_STRUCT m_PartyS[OBJMAX];	// 8
	char m_szTempPaint[256];	// 56B88
	PartyBonusInfo m_BonusInfo;

};

struct PMSG_PARTYMATCH_REG_REQ
{
	PBMSG_HEAD2 h;	// C1:EF:00
	char szDesc[41];
	char szPass[5];
	short MinLevel;
	short MaxLevel;
	short MoveReqId;
	short ClassInfo;
	BYTE btUnk[7];
	BYTE btAutoAccept;
};

struct PMSG_PARTYMATCH_REG_ANS
{
	PBMSG_HEAD2 h;
	int iResult;
};

struct PMSG_PARTYMATCH_INFO_REQ
{
	PBMSG_HEAD2 h;
	int RequestPage;
	char szSearch[11];
	BYTE ReqType;
};

struct PMSG_PARTYMATCH_COUNT
{
	PWMSG_HEAD2 h;
	int count;
	int curpage;
	int totalpage;
	int result;
};

struct PMSG_PARTYMATCH_INFO
{
	char szName[11];
	char szDescription[41];
	short MinLevel;
	short MaxLevel;
	int MoveReqId;
	int LeaderLevel;
	BYTE LeaderClass;
	BYTE Class;
	BYTE PartyCount;
	BYTE IsProtected;
	int iUnk1;
	int iUnk2;
	int Channel;
	BYTE Gens;
};

struct PMSG_PARTYMATCH_JOIN_REQ
{
	PBMSG_HEAD2 h;
	char szLeader[11];
	char szPassword[5];
	BYTE btIsPassword;
	BYTE btType;
};

struct PMSG_PARTYMATCH_JOIN_ANS
{
	PBMSG_HEAD2 h;
	int iResult;
};

struct PMSG_PARTYMATCH_WAIT_ANS
{
	PBMSG_HEAD2 h;
	int iResult;
	int iServerCode;
	char szPartyLeader[11];
};

struct PMSG_PARTYMATCH_CANCEL_REQ
{
	PBMSG_HEAD2 h;
	BYTE btType;
};

struct PMSG_PARTYMATCH_CANCEL_ANS
{
	PBMSG_HEAD2 h;
	int Type;
	int Result;
};

// DB Side

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
struct PARTYMATCH_GCSET_JOINNOTIFICATION //-> 8 [0xC1 -> 0xEF -> 0x08]
{
	PBMSG_HEAD2 h;			//+0
	int Result;				//+4
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PARTYMATCH_GDREQ_REQUESTLIST
{
	PBMSG_HEAD2 h;
	int UserIndex;
	char PartyLeader[11];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PARTYMATCH_CGREQ_REQUESTANSWER //-> 19 [0xC1 -> 0xEF -> 0x05]
{
	PBMSG_HEAD2 h;			//+0
	char Character[11];		//+8
	int Result;				//+4 (0: aborted, 1: accepted)
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

void CGReqPartyMatchWindow(PMSG_PARTYMATCH_INFO_REQ* lpMsg,int aIndex);
void CGReqPartyMatchReg(PMSG_PARTYMATCH_REG_REQ* lpMsg,int aIndex);
void CGReqPartyMatchJoin(PMSG_PARTYMATCH_JOIN_REQ* lpMsg,int aIndex);
void CGReqPartyMatchWaitingList(int aIndex);
void CGReqPartyMatchCancel(PMSG_PARTYMATCH_CANCEL_REQ* lpMsg,int aIndex);
void CGReqPartyMatchAcceptList(int aIndex);
void CGReqPartyMatchRequestAnswer(PARTYMATCH_CGREQ_REQUESTANSWER* lpData, int UserIndex);

void DGAnsPartyMatchReg(PMSG_PARTYMATCHING_REG_ANS* lpMsg);
void DGAnsPartyMatchInfo(LPBYTE lpMsg);
void DGAnsPartyMatchJoin(PMSG_PARTYMATCHING_JOIN_ANS* lpMsg);
void DGAnsPartyMatchWaitlist(PMSG_PARTYMATCHING_WAIT_ANS* lpMsg);
void DGAnsPartyMatchCancel(PMSG_PARTYMATCHING_CANCEL_ANS* lpMsg);
void DGAnsPartyMatchAcceptList(LPBYTE lpData);
void DGAnsPartyMatchRequestAnswer(PARTYMATCH_DGANS_REQUESTANSWER* lpData);

void GDPartyMatchUpdateStatus(LPSTR Character, BYTE Status);

struct _PARTY_MATCH
{
	int iLeaderIndex;
	int iPartyIndex;
};

enum _PARTY_MATCH_STATUS
{
	ST_NONE = 0,
	ST_WAIT = 1,
	ST_ACTIVE = 2,
};

#endif