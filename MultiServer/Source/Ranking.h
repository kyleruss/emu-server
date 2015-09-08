#ifndef __RANKING_H__
#define __RANKING_H__

#include "CQuery.h"

extern CQuery g_RankingDB;

struct RSInfo
{
	WORD TCPPort;
	char dbDSN[64];
};

#define RS_INISECTION "Ranking"
extern RSInfo g_RSInfo;

typedef struct 
{
  PBMSG_HEAD h;
  char AccountID[10];	// 3
  char GameID[10];	// d
  int ServerCode;	// 18
  int Score;	// 1c
  int Class;	// 20
  int SquareNum; // 24
}PMSG_ANS_EVENTUSERSCORE, *LPMSG_ANS_EVENTUSERSCORE;

typedef struct
{
  PBMSG_HEAD h;
  char AccountID[10];
  char GameID[10];
  int ServerCode;
  int iObjIndex;
  int iBloodCastle;
  int iBloodCastleItemPos;
}PMSG_REQ_BLOODCASTLE_ENTER, *LPMSG_REQ_BLOODCASTLE_ENTER;

typedef struct
{
  PBMSG_HEAD h;
  char AccountID[10];
  char GameID[10];
  int ServerCode;
  int iObjIndex;
  int iResult;
  int iBloodCastle;
  int iBloodCastleItemPos;
}PMSG_ANS_BLOODCASTLE_ENTER, *LPMSG_ANS_BLOODCASTLE_ENTER;

typedef struct
{
  PBMSG_HEAD h;
  char AccountID[10];	// 3
  char GameID[10];	// d
  int ServerCode;	// 18
  int iObjIndex;	// 1c
}PMSG_REQ_BLOODCASTLE_ENTERCOUNT, *LPMSG_REQ_BLOODCASTLE_ENTERCOUNT;

typedef struct
{
  PBMSG_HEAD h;
  char AccountID[10];
  char GameID[10];
  int ServerCode;
  int iObjIndex;
  int iLeftCount;
}PMSG_ANS_BLOODCASTLE_ENTERCOUNT, *LPMSG_ANS_BLOODCASTLE_ENTERCOUNT;

typedef struct 
{
  PBMSG_HEAD h;
  char AccountID[10];	// 3
  char GameID[10];	// d
  int ServerCode;	// 18
  int Score;	// 1c
  int Class;	// 20
  int BridgeNum;	// 24
  int iLeftTime;	// 28
  int iAlivePartyCount;	// 2c
}PMSG_ANS_BLOODCASTLESCORE_5TH, *LPMSG_ANS_BLOODCASTLESCORE_5TH;


void RankingServerInit();
void RSProtocolCore(int aIndex, DWORD headcode, LPBYTE aRecv, int Len);
void DevilSqureScore(LPMSG_ANS_EVENTUSERSCORE lpMsg);

void EGDevilSquareEnter(LPMSG_REQ_BLOODCASTLE_ENTERCOUNT lpMsg);
void EGReqDevilSquareEnter(LPMSG_REQ_BLOODCASTLE_ENTER lpMsg,int aIndex);

void EGReqBloodCastleEnter(LPMSG_REQ_BLOODCASTLE_ENTER lpMsg,int aIndex);
void EGBloodCastleEnter(LPMSG_REQ_BLOODCASTLE_ENTERCOUNT lpMsg);
void EGReqBloodCastleEnterCount(LPMSG_REQ_BLOODCASTLE_ENTERCOUNT lpMsg,int aIndex);

void BloodCastleScore_5TH(LPMSG_ANS_BLOODCASTLESCORE_5TH lpMsg);

#endif