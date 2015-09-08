#if !defined(__EVENTSERVER_H__)
#define __EVENTSERVER_H__

#include "CQuery.h"

#define ES_INISECTION "EventServer"

void EventServerInit();
void ESProtocolCore(int aIndex, DWORD headcode, LPBYTE aRecv, int Len);

struct ES_INFO
{
	WORD TCPPort;
	char dbDSN[64];
	char mydbDSN[64];
};

extern ES_INFO g_ESInfo;
extern CQuery g_EventDB;
extern CQuery g_MyEventDB;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
}PMSG_REQ_VIEW_EC_MN, *LPMSG_REQ_VIEW_EC_MN;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
	short nEVENT_CHIPS;
	int iMUTO_NUM;
}PMSG_ANS_VIEW_EC_MN, *LPMSG_ANS_VIEW_EC_MN;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	unsigned char Pos;
	char szUID[11];
}PMSG_REQ_REGISTER_EVENTCHIP, *LPMSG_REQ_REGISTER_EVENTCHIP ;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	unsigned char Pos;
	char szUID[11];
	char bSUCCESS;
	short nEVENT_CHIPS;
}PMSG_ANS_REGISTER_EVENTCHIP, *LPMSG_ANS_REGISTER_EVENTCHIP;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
}PMSG_REQ_RESET_EVENTCHIP, *LPMSG_REQ_RESET_EVENTCHIP;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
}PMSG_ANS_RESET_EVENTCHIP, *LPMSG_ANS_RESET_EVENTCHIP;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
	int iStoneCount;
}PMSG_ANS_VIEW_STONES, *LPMSG_ANS_VIEW_STONES;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	unsigned char iPosition;
	char szUID[11];
}PMSG_REQ_REGISTER_STONES, *LPMSG_REQ_REGISTER_STONES;

typedef struct  {
	PBMSG_HEAD h;
	int iINDEX;
	unsigned char iPosition;
	char szUID[11];
	char bSUCCESS;
	int iStoneCount;
}PMSG_ANS_REGISTER_STONES, *LPMSG_ANS_REGISTER_STONES;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
	int iStoneCount;
}PMSG_REQ_DELETE_STONES, *LPMSG_REQ_DELETE_STONES;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
	int iStoneCount;
}PMSG_ANS_DELETE_STONES, *LPMSG_ANS_DELETE_STONES;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char SERIAL1[5];
	char SERIAL2[5];
	char SERIAL3[5];
	int iMEMB_GUID;
}PMSG_REQ_2ANIV_SERIAL, *LPMSG_REQ_2ANIV_SERIAL;

typedef struct
{
	PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned char btIsRegistered;
	int iGiftNumber;
}PMSG_ANS_2ANIV_SERIAL, *LPMSG_ANS_2ANIV_SERIAL;

typedef struct 
{
	PBMSG_HEAD2 h;
	char szAccount[11];
	int points;
}PMSG_EXSYMBOL, *LPMSG_EXSYMBOL;

typedef struct 
{
	PBMSG_HEAD2	h;
	char szUserName[10];
	short QuestGroup;
	short QuestIndex;
	short QuestState;
	short QuestItem[3];
}PMSG_CUSTOMQUEST_SAVEUSER, *LPMSG_CUSTOMQUEST_SAVEUSER;

typedef struct
{
	PBMSG_HEAD2 h;
	char szUserName[10];
	int aIndex;
}PMSG_CUSTOMQUEST_REQUSER, *LPMSG_CUSTOMQUEST_REQUSER;

typedef struct 
{
	PBMSG_HEAD2	h;
	BYTE result;
	char szUserName[10];
	int aIndex;
	short QuestGroup;
	short QuestIndex;
	short QuestState;
	short QuestItem[3];
}PMSG_CUSTOMQUEST_ANSUSER, *LPMSG_CUSTOMQUEST_ANSUSER;

typedef struct 
{
	PBMSG_HEAD2 h;
	int DBNumber;
	int iPoints;
	char Name[11];
}PMSG_ADD_CP, *LPMSG_ADD_CP;

typedef struct 
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[10];
}PBMSG_REQSUMMONINFO, *LPBMSG_REQSUMMONINFO;

typedef struct
{
	PBMSG_HEAD2 h;
	BYTE result;

	int aIndex;
	char szName[MAX_IDSTRING+1];

	int Level;
	__int64 Exp;
}PBMSG_ANSSUMMONINFO, *LPBMSG_ANSSUMMONINFO;


typedef struct 
{
	PBMSG_HEAD2 h;

	char szName[10];
	int level;
	__int64 exp;
}PBMSG_REQSAVESUMMON, *LPBMSG_REQSAVESUMMON;

typedef struct
{
	PBMSG_HEAD2 h;
	int ReqIndex;
	char ReqName[10];
}PMSG_REQ_MARRYINFO, *LPMSG_REQ_MARRYINFO;

typedef struct
{
	PBMSG_HEAD2 h;
	int ReqIndex;
	char ReqName[10];
	char MarriedName[10];
	BYTE Result;
}PMSG_ANS_MARRY, *LPMSG_ANS_MARRY;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char aName[10];
	int tIndex;
	char tName[10];
}PMSG_REQ_MARRY2, *LPMSG_REQ_MARRY2;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[10];
}PMSG_REQ_DIVORCE, *LPMSG_REQ_DIVORCE;

typedef struct
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	char Wife[11];
	char Husband[11];
}PMSG_ANS_DIVORCE, *LPMSG_ANS_DIVORCE;

typedef struct
{
	PBMSG_HEAD h;
	BYTE btPos;
	int aIndex;
	char szName[11];
}PMSG_DOUBLEGOER_REQ_ENTER_EVENT, *LPMSG_DOUBLEGOER_REQ_ENTER_EVENT;

typedef struct
{
	PBMSG_HEAD h;
	int iResult;
	BYTE btPos;
	int aIndex;
}PMSG_DOUBLEGOER_ANS_ENTER_EVENT, *LPMSG_DOUBLEGOER_ANS_ENTER_EVENT;

typedef struct
{
	PBMSG_HEAD h;
	char szName[10];
}PMSG_DOUBLEGOER_UPDATE_EVENT, *LPMSG_DOUBLEGOER_UPDATE_EVENT;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[10];
}PMSG_REQ_NOBLESSINFO, *LPMSG_REQ_NOBLESSINFO;

typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[10];
	BYTE btResult;
	int NoblessRank;
	int NoblessPts;
}PMSG_ANS_NOBLESSINFO, *LPMSG_ANS_NOBLESSINFO;

typedef struct
{
	PBMSG_HEAD2 h;
	char szName[10];
	int NoblessRank;
	int NoblessPts;
}PMSG_REQ_SAVENOBLESSINFO, *LPMSG_REQ_SAVENOBLESSINFO;

void EGAnsEventChipInfo(LPMSG_REQ_VIEW_EC_MN lpMsg,int aIndex);
void EGAnsRegEventChipInfo(LPMSG_REQ_REGISTER_EVENTCHIP lpMsg,int aIndex);
void EGAnsResetEventChip(LPMSG_REQ_RESET_EVENTCHIP lpMsg,int aIndex);
void EGAnsEventStoneInfo(LPMSG_REQ_VIEW_EC_MN lpMsg,int aIndex);
void EGAnsRegEventStoneInfo(LPMSG_REQ_REGISTER_STONES lpMsg,int aIndex);
void EGAnsDeleteStones(LPMSG_REQ_DELETE_STONES lpMsg,int aIndex);
void EGAns2AnivRegSerial(LPMSG_REQ_2ANIV_SERIAL lpMsg,int aIndex);
void EGAnsResetStoneInfo(LPMSG_REQ_RESET_EVENTCHIP lpMsg,int aIndex);

void EGAnsPCBangUserConnect(LPBYTE aRecv,int aIndex);
void EGAnsPCBangUpdateUser(LPBYTE aRecv,int aIndex);

void EGAnsEnterDoubleGoer(LPBYTE aRecv,int aIndex);
void EGUpdateDoubleGoer(LPBYTE aRecv,int aIndex);

void ReqExchangeSymbols(LPMSG_EXSYMBOL lpMsg,int aIndex);

void CustomQuestReqInfo(LPMSG_CUSTOMQUEST_REQUSER lpMsg,int aIndex);
void CustomQuestSaveInfo(LPMSG_CUSTOMQUEST_SAVEUSER lpMsg,int aIndex);
void AddCashPoints(LPMSG_ADD_CP lpMsg,int aIndex);

void ReqSummonPetInfo(LPBMSG_REQSUMMONINFO lpMsg,int aIndex);
void ReqSaveSummonInfo(LPBMSG_REQSAVESUMMON lpMsg);
void ReqMarry(LPMSG_REQ_MARRYINFO lpMsg,int aIndex);
void DoMarry(LPMSG_REQ_MARRY2 lpMsg,int aIndex);
void Divorce(LPMSG_REQ_DIVORCE lpMsg,int aIndex);

void LoadNoblessInfo(LPMSG_REQ_NOBLESSINFO lpMsg,int aIndex);
void SaveNoblessInfo(LPMSG_REQ_SAVENOBLESSINFO lpMsg);


#define MAX_QUEST2_GROUP 3

struct MyQuest2_USERQUEST
{
	WORD QuestIndex;
	WORD QuestState;

	DWORD TimeStamp;
};

struct PMSG_QUEST2_REQ_USER
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
};

struct PMSG_QUEST2_ANS_USER
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];

	MyQuest2_USERQUEST m_UserData[MAX_QUEST2_GROUP];
};

struct PMSG_QUEST2_SAVE_USER
{
	PBMSG_HEAD2 h;
	char szName[11];
	MyQuest2_USERQUEST m_UserData[MAX_QUEST2_GROUP];
};

void CustomQuest2ReqUser(PMSG_QUEST2_REQ_USER* lpMsg,int aIndex);
void CustomQuest2SaveUser(PMSG_QUEST2_SAVE_USER* lpMsg);


struct PMSG_UPDATE_CONNECT_STAT
{
	PBMSG_HEAD2 h;
	char szAccount[11];
};

void UpdateConnectState(PMSG_UPDATE_CONNECT_STAT* lpMsg);

typedef struct
{
	PBMSG_HEAD h;
	int aIndex;
	char szAccount[10];
	DWORD dwSerial;
}PMSG_REQ_SELLFULLOPT, *LPMSG_REQ_SELLFULLOPT;

void ReqSellFullItem(LPMSG_REQ_SELLFULLOPT lpMsg,int aIndex);


typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iOlympEventCycle;
}OLP_REQ_OLYMPINITDATA, *LOLP_REQ_OLYMPINITDATA;

typedef struct
{
	PBMSG_HEAD2 h;
	int iResult;
	WORD wMapSvrNum;
	WORD wStartYear;
	BYTE btStartMonth;
	BYTE btStartDay;
	WORD wEndYear;
	BYTE btEndMonth;
	BYTE btEndDay;
	BYTE btFirstCreate;
}OLP_ANS_OLYMPINITDATA, *LPOLP_ANS_OLYMPINITDATA;

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	WORD wStartYear;
	BYTE btStartMonth;
	BYTE btStartDay;
	WORD wEndYear;
	BYTE btEndMonth;
	BYTE btEndDay;
}OLP_REQ_SDEDCHANGE, *LPOLP_REQ_SDEDCHANGE;

typedef struct
{
	PBMSG_HEAD2 h;
	int iResult;
	WORD wMapSvrNum;
	int iIndex;
	WORD wStartYear;
	BYTE btStartMonth;
	BYTE btStartDay;
	WORD wEndYear;
	BYTE btEndMonth;
	BYTE btEndDay;
}OLP_ANS_SDEDCHANGE, *LPOLP_ANS_SDEDCHANGE;

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	int iClass;
	char szName[10];
}OLP_REQ_REGFIGHT,*LPOLP_REQ_REGFIGHT;

typedef struct
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	int iIndex;
	char szName[10];
	int iBattleCount;
	int iBattlePts;
	int iLastRegTime;
}OLP_ANS_REGFIGHT,*LPOLP_ANS_REGFIGHT;

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iEndResult;
	char szWinner[10];
	char szLooser[10];
}OLP_ENFIGHT, *LPOLP_ENFIGHT;

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
}OLP_REQ_RESET, *LPOLP_REQ_RESET;

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	char szName[10];
}OLP_REQ_REWARD, *LPOLP_REQ_REWARD;

typedef struct
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btRank;
	BYTE btReward;
	int iIndex;
	char szName[10];
}OLP_ANS_REWARD, *LPOLP_ANS_REWARD;

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	char szName[10];
}OLP_REQ_STATS, *LPOLP_REQ_STATS;

typedef struct
{
	PBMSG_HEAD2 h;
	int iIndex;
	char szName[10];
	BYTE btResult;
	int iPts;
	int iBattles;
}OLP_ANS_STATS, *LPOLP_ANS_STATS;

void LoadOlympInfo(LOLP_REQ_OLYMPINITDATA lpMsg,int aIndex);
void ReqOlympChangeDate(LPOLP_REQ_SDEDCHANGE lpMsg,int aIndex);
void ReqRegOlymp(LPOLP_REQ_REGFIGHT lpMsg,int aIndex);
void OlympEndFight(LPOLP_ENFIGHT lpMsg,int aIndex);
void OlympResetPeriod(LPOLP_REQ_RESET lpMsg,int aIndex);
void OlympRewardRequest(LPOLP_REQ_REWARD lpMsg,int aIndex);
void OlympStatsRequest(LPOLP_REQ_STATS lpMsg,int aIndex);

#endif