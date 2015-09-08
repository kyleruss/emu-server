#pragma once

#include "..\include\prodef.h"

#define MURUMMY_MAX_CARD		24
#define MURUMMY_MAX_CARD_COLOR	3
#define MURUMMY_MAX_SLOT		8
#define MURUMMY_REWARD_MONEY	500000

struct _tagMuRummyCardInfo
{
	char btColor;
	char btNumber;
	char btSlotNum;
};

struct _tagMuRummyCardInfoDS
{
    _tagMuRummyCardInfoDS()
	{
		this->btColor = -1;
		this->btNumber = -1;
		this->btSlotNum = -1;
		this->btSeq = -1;
		this->btStatus = -1;
	};

    BYTE btColor;
    BYTE btNumber;
    BYTE btSlotNum;
    BYTE btSeq;
    BYTE btStatus;
};

struct _tagMuRummyCardUpdateDS
{
    _tagMuRummyCardUpdateDS()
	{
		this->btSlotNum = -1;
		this->btSeq = -1;
		this->btStatus = -1;
	};

    BYTE btSlotNum;
    BYTE btSeq;
    BYTE btStatus;
};

struct _tagPMSG_REQ_MURUMMY_EVENT_OPEN
{
    PBMSG_HEAD2 h;
};

struct _tagPMSG_ANS_MURUMMY_EVENT_OPEN
{
    PBMSG_HEAD2 h;
    BYTE btResult;
    BYTE btEventTime1;
    BYTE btEventTime2;
    BYTE btEventTime3;
    BYTE btEventTime4;
};

struct _tagPMSG_REQ_MURUMMY_INFO
{
    PBMSG_HEAD2 h;
};

struct _tagPMSG_ANS_MURUMMY_INFO
{
    _tagPMSG_ANS_MURUMMY_INFO()
	{
		this->btTotScoreH = 0;
		this->btTotScoreL = 0;
		this->btCardDeckCnt = 0;
	};

    PBMSG_HEAD2 h;
    BYTE btTotScoreH;
    BYTE btTotScoreL;
    BYTE btCardDeckCnt;
    _tagMuRummyCardInfo stMuRummyCardInfo[5];
};

struct _tagPMSG_REQ_REVEAL_CARD
{
    PBMSG_HEAD2 h;
};

struct _tagPMSG_ANS_REVEAL_CARD
{
    _tagPMSG_ANS_REVEAL_CARD()
	{
		this->btCardDeckCnt = 0;
	};

    PBMSG_HEAD2 h;
    _tagMuRummyCardInfo stMuRummyCardInfo[5];
    BYTE btCardDeckCnt;
};

struct _tagPMSG_REQ_CARD_MOVE
{
    _tagPMSG_REQ_CARD_MOVE()
	{

	};

    PBMSG_HEAD2 h;
    BYTE sSlot;
    BYTE tSlot;
};

struct _tagPMSG_ANS_CARD_MOVE
{
	_tagPMSG_ANS_CARD_MOVE()
	{
		this->sSlot = -1;
		this->tSlot = -1;
		this->btColor = -1;
		this->btNumber = -1;
	};

	PBMSG_HEAD2 h;
	BYTE sSlot;
    BYTE tSlot;
	BYTE btColor;
    BYTE btNumber;
};

struct _tagPMSG_REQ_CARD_REMOVE
{
    _tagPMSG_REQ_CARD_REMOVE()
	{

	};

    PBMSG_HEAD2 h;
    BYTE btSlot;
};

struct _tagPMSG_ANS_CARD_REMOVE
{
    _tagPMSG_ANS_CARD_REMOVE()
	{

	};

    PBMSG_HEAD2 h;
    BYTE btResult;
};

struct _tagPMSG_REQ_CARD_MATCH
{
    _tagPMSG_REQ_CARD_MATCH()
	{

	};

    PBMSG_HEAD2 h;
};

struct _tagPMSG_ANS_CARD_MATCH
{
    _tagPMSG_ANS_CARD_MATCH()
	{

	};

    PBMSG_HEAD2 h;
    BYTE btScoreH;
    BYTE btScoreL;
    BYTE btTotScoreH;
    BYTE btTotScoreL;
    BYTE btResult;
};

struct _tagPMSG_REQ_MURUMMY_END
{
    _tagPMSG_REQ_MURUMMY_END()
	{

	};

    PBMSG_HEAD2 h;
};

struct _tagPMSG_ANS_MURUMMY_END
{
    _tagPMSG_ANS_MURUMMY_END()
	{

	};

    PBMSG_HEAD2 h;
    BYTE btResult;
};


struct _tagPMSG_SEND_SLOTCARD_LIST
{
    _tagPMSG_SEND_SLOTCARD_LIST()
	{

	};

    PBMSG_HEAD2 h;
    _tagMuRummyCardInfo stMuRummyCardInfo[5];
};

struct _tagPMSG_REQ_MURUMMY_SELECT_DS
{
    PBMSG_HEAD2 h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
    WORD aIndex;
};

struct _tagPMSG_ANS_MURUMMY_SELECT_DS
{
    _tagPMSG_ANS_MURUMMY_SELECT_DS()
	{

	};

    PBMSG_HEAD2 h;
    WORD aIndex;
    WORD wScore;
    BYTE btResult;
    _tagMuRummyCardInfoDS stMuRummyCardInfoDS[MURUMMY_MAX_CARD];
};

struct _tagPMSG_SEND_MURUMMY_MSG
{
    _tagPMSG_SEND_MURUMMY_MSG()
	{
		this->btNotiIndex = 0;
		this->btValueH = 0;
		this->btValueL = 0;
	};

    PBMSG_HEAD2 h;
    BYTE btNotiIndex;
    BYTE btValueH;
    BYTE btValueL;
};

struct _tagPMSG_SEND_MURUMMYEVENT_ON
{
    _tagPMSG_SEND_MURUMMYEVENT_ON()
	{

	};

    PBMSG_HEAD2 h;
};

struct _tagPMSG_REQ_MURUMMY_INSERT_DS
{
    _tagPMSG_REQ_MURUMMY_INSERT_DS()
	{

	};

    PBMSG_HEAD2 h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
    WORD aIndex;
    _tagMuRummyCardInfoDS stMuRummyCardInfoDS[MURUMMY_MAX_CARD];
};

struct _tagPMSG_REQ_MURUMMY_UPDATE_DS 
{
    PBMSG_HEAD2 h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
    BYTE btSlotNum;
    BYTE btStatus;
    BYTE btSequence;
};

struct _tagPMSG_REQ_MURUMMY_SCORE_UPDATE_DS
{
    _tagPMSG_REQ_MURUMMY_SCORE_UPDATE_DS()
	{

	};

    PBMSG_HEAD2 h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
    WORD wScore;
    _tagMuRummyCardUpdateDS stCardUpdateDS[3];
};

struct _tagPMSG_REQ_MURUMMY_DELETE_DS
{
    PBMSG_HEAD2 h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
};

struct _tagPMSG_REQ_MURUMMY_SLOTUPDATE_DS
{
    _tagPMSG_REQ_MURUMMY_SLOTUPDATE_DS()
	{

	};

    PBMSG_HEAD2 h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
    _tagMuRummyCardUpdateDS stCardUpdateDS;
};

struct _tagPMSG_REQ_MURUMMY_INFO_UPDATE_DS
{
    _tagPMSG_REQ_MURUMMY_INFO_UPDATE_DS()
	{

	};

    PBMSG_HEAD2 h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
    WORD wScore;
    _tagMuRummyCardUpdateDS stMuRummyCardUpdateDS[MURUMMY_MAX_CARD];
};

struct _tagPMSG_REQ_MURUMMY_LOG_INSERT_DS
{
    PBMSG_HEAD2 h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
    WORD wScore;
};