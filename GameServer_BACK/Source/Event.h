#ifndef ___EVENT_H
#define ___EVENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h"


#define MAX_EVENTCHIP_TYPE	5

#define EVENCHIP_TYPE_RANGE(x)  ( ((x)<0)?FALSE:((x)>MAX_EVENTCHIP_TYPE-1)?FALSE:TRUE )





struct PMSG_REGEVENTCHIP_RESULT
{
	PBMSG_HEAD h;	// C1:95
	BYTE Type;	// 3
	short ChipCount;	// 4
};


struct PMSG_GETMUTONUMBER_RESULT
{
	PBMSG_HEAD h;	// C1:96
	short MutoNum[3];	// 4
};


struct PMSG_EVENTCHIPINFO
{
	PBMSG_HEAD h;	// C1:94
	BYTE Type;	// 3
	int ChipCount;	// 4
	short MutoNum[3];	// 6
};



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct PMSG_ANS_VIEW_EC_MN
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
	short nEVENT_CHIPS;	// 14
	int iMUTO_NUM;	// 18
};


struct PMSG_ANS_REGISTER_EVENTCHIP
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	BYTE Pos;	// 8
	char szUID[11];	// 9
	char bSUCCESS;	// 14
	short nEVENT_CHIPS;	// 16
};



struct PMSG_ANS_REGISTER_MUTONUM
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
	int iMUTO_NUM;	// 14
};



struct PMSG_ANS_RESET_EVENTCHIP
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
};

struct PMSG_ANS_VIEW_STONES
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
	int iStoneCount;	// 14
};

struct PMSG_ANS_REGISTER_STONES
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	BYTE iPosition;	// 8
	char szUID[11];	// 9
	char bSUCCESS;	// 14
	int iStoneCount;	// 18
};

struct PMSG_ANS_DELETE_STONES
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
	int iStoneCount;	// 14
};

struct PMSG_ANS_2ANIV_SERIAL
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	BYTE btIsRegistered;	// 13
	int iGiftNumber;	// 14
};


struct PMSG_ANS_REG_RINGGIFT
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	BYTE btIsRegistered;	// 13
	BYTE btGiftSection;	// 14
	BYTE btGiftKind;	// 15
};

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

struct PMSG_ANS_BLOODCASTLE_ENTERCOUNT
{
	PBMSG_HEAD h;
	char AccountID[10];	// 3
	char GameID[10];	// D
	int ServerCode;	// 18
	int iObjIndex;	// 1C
	int iLeftCount;	// 20
};

struct PMSG_ANS_REG_CC_OFFLINE_GIFT
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
	int iResultCode;	// 24
	char szGIFT_NAME[50];	/// 28
};


struct PMSG_ANS_REG_DL_OFFLINE_GIFT
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
	int iResultCode;	// 24
	char szGIFT_NAME[50];	/// 28
};


struct PMSG_ANS_REG_HT_OFFLINE_GIFT
{
	PBMSG_HEAD h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
	int iResultCode;	// 24
	char szGIFT_NAME[50];	/// 28

};

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

struct PMSG_REQ_BLOODCASTLE_ENTERCOUNT
{
	PBMSG_HEAD h;	// C1:0B
	char AccountID[10];	// 3
	char GameID[10];	// D
	int ServerCode;	// 18
	int iObjIndex;	// 1C
};

void EventChipEventProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen);
void DataSendEventChip(PCHAR pMsg, int size);

void EGRecvEventChipInfo(PMSG_ANS_VIEW_EC_MN * aRecv);
void EGResultRegEventChip(PMSG_ANS_REGISTER_EVENTCHIP * aRecv);
void EGRecvRegMutoNum( PMSG_ANS_REGISTER_MUTONUM* aRecv);
void EGRecvChangeRena( PMSG_ANS_RESET_EVENTCHIP* aRecv);
void EGRecvStoneInfo( PMSG_ANS_VIEW_STONES* aRecv);
void EGRecvRegStone( PMSG_ANS_REGISTER_STONES* aRecv);
void EGRecvDeleteStone( PMSG_ANS_DELETE_STONES* aRecv);
void EGRecvChangeStones( PMSG_ANS_RESET_EVENTCHIP* aRecv);
void EGRecv2AnvRegSerial( PMSG_ANS_2ANIV_SERIAL* aRecv);
void EGRecvRegRingGift( PMSG_ANS_REG_RINGGIFT* aRecv);
void EGReqDevilSquareEnter(PMSG_REQ_MOVEDEVILSQUARE* lpMsg,int aIndex);
void EGAnsDevilSquareEnter(PMSG_ANS_BLOODCASTLE_ENTER* lpMsg);
void EGDevilSquareEnter(int aIndex);
void EGReqBloodCastleEnter(PMSG_REQ_MOVEBLOODCASTLE* lpMsg,int aIndex);
void EGAnsBloodCastleEnter(PMSG_ANS_BLOODCASTLE_ENTER* lpMsg);
void EGBloodCastleEnter(int aIndex);
void EGReqBloodCastleEnterCount(int iIndex);
void EGAnsBloodCastleEnterCount( PMSG_ANS_BLOODCASTLE_ENTERCOUNT* lpMsg);
void EGReqRegCCOfflineGift(int iIndex);
void EGAnsRegCCOfflineGift( PMSG_ANS_REG_CC_OFFLINE_GIFT* lpMsg);
void EGReqRegDLOfflineGift(int iIndex);
void EGAnsRegDLOfflineGift( PMSG_ANS_REG_DL_OFFLINE_GIFT* lpMsg);
void EGReqRegHTOfflineGift(int iIndex);
void EGAnsRegHTOfflineGift( PMSG_ANS_REG_HT_OFFLINE_GIFT* lpMsg);
void Japan1StAnivBoxOpen(LPOBJ lpObj, int iBoxLevel);

#if( GS_CASTLE == 1)
void CastleHuntZoneBossRewardOpen(LPOBJ lpObj,BYTE btMapNumber,BYTE cX,BYTE cY);
#endif

#if( GS_CASTLE == 0 || GS_SPEC == 1)
void KanturuMayaHandItemBagOpen(LPOBJ lpObj, BYTE btMapNumber, BYTE cX, BYTE cY);
void KanturuNightmareItemBagOpen(LPOBJ lpObj, BYTE btMapNumber, BYTE cX, BYTE cY);
#endif

extern LPOBJ pEventObj;

#endif