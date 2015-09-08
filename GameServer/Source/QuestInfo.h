#ifndef QUESTINFO_H
#define QUESTINFO_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\classdef.h"
#include "user.h"

#define MAX_QUEST_INFO 200
#define MAX_SUBINFO_QUEST 10
#define MAX_CONDITION_QUEST 10
#define ITEM_QUEST_DROP_PROBABILITY 1000
#define QUEST_COMPENSATION_CHANGEUP 201
#define QUEST_COMPENSATION_STATUP 200
#define QUEST_COMPENSATION_PLUSSSTAT 202
#define QUEST_COMPENSATION_COMBOSKILL 203
#define QUEST_COMPENSATION_CHANGEUP2 204
#define QUEST_MINLEVEL_PLUSSTAT 220
#define QUEST_STATE_SIZE 50


typedef struct QUEST_SUB_INFO
{
	int QuestType;	// 0
	int NeedType;	// 4
	int NeedSubType;	// 8
	int ItemLevel;	// C
	int NeedNumber;	// 10
	int NeedTargetMinLevel;	// 14
	int NeedTargetMaxLevel;	// 18
	int NeedDropRate;	// 1C
	int RewardType;	// 20
	int RewardSubType;	// 24
	int RewardCount;	// 28
	int LinkConditionIndex;	// 2C
	int ContextBeforeReg;	// 30
	int ContextAfterReg;	// 34
	int ContextCompleteQuest;	// 38
	int ContextQuestClear;	// 3C
	BYTE RequireClass[MAX_TYPE_PLAYER];	// 40

} QUEST_SUB_INFO, * LPQUEST_SUB_INFO;


typedef struct QUEST_CONDITION
{
	int Index;	// 0
	int NeedQuestIndex;	// 4
	int MinLevel;	// 8
	int MaxLevel;	// C
	int ReqStr;	// 10
	int NeedZen;	// 14
	int StartContext;	// 18

} QUEST_CONDITION, * LPQUEST_CONDITION;


typedef struct QUEST_INFO	
{
	char Name[50];	// 0
	int QuestIndex;	// 34
	int QuestStartType;	// 38
	int QuestStartSubType;	// 3C
	int QuestSubInfoCount;	// 40
	QUEST_SUB_INFO QuestSubInfo[MAX_SUBINFO_QUEST];	// 44
	int QuestConditionCount;	// 314
	QUEST_CONDITION QuestCondition[MAX_CONDITION_QUEST];	// 318

} QUEST_INFO, * LPQUEST_INFO;



class CQuestInfo
{
public:

	CQuestInfo();
	virtual ~CQuestInfo();

	void Init();
	int IsQuest(int QuestIndex);
	int GetQeustCount(){return m_QuestCount;}
	int LoadQuestInfo(char* Buffer, int iSize);
	int LoadQuestInfo(char* filename);
	void InitQuestItem();
	LPQUEST_INFO GetQuestInfo(int QuestIndex);
	LPQUEST_SUB_INFO GetSubquestInfo(LPOBJ lpObj, LPQUEST_INFO lpQuestInfo, int subquestindex);
	LPQUEST_CONDITION GetQuestCondition(LPQUEST_INFO lpQuestInfo, int conditionindex);
	int GetQuestState(LPOBJ lpObj, int QuestIndex);
	BYTE GetQuestStateBYTE(LPOBJ lpObj, int QuestIndex);
	unsigned char SetQuestState(LPOBJ lpObj, int QuestIndex, int State);
	unsigned char ReSetQuestState(LPOBJ lpObj, int QuestIndex);
	unsigned char QuestClearConditionCheck(LPOBJ lpObj, int QuestIndex);
	unsigned char QuestRunConditionCheck(LPOBJ lpObj, int QuestIndex);
	int CompareCondition(LPOBJ lpObj, LPQUEST_CONDITION lpCondition);
	int NpcTalk(LPOBJ lpNpc, LPOBJ lpObj);
	int MonsterKill(LPOBJ Npc,LPOBJ lpObj);
	int MonsterItemDrop(LPOBJ lpObj);
	int MonsterItemDropParty(LPOBJ MonObj,LPOBJ lpObj);
	int QuestAccept(LPOBJ lpObj, int QuestIndex);
	int QuestClear(LPOBJ lpObj, int QuestIndex);
	void QuestInfoSave(LPOBJ lpObj);
	int SendMonsterKillInfo(int QuestIndex,int iIndex);
	void NotifyResultEnterBarracks(int aIndex);
	void NotifyResultEnterRefuge(int aIndex);
	int CheckItemCount(int aIndex,short Type,short Count);


private:
	int GetQuestCount()	{ return m_QuestCount; };
	int m_QuestCount;	// 4
	QUEST_INFO QuestInfo[MAX_QUEST_INFO];	// 8

};


extern CQuestInfo g_QuestInfo;

class CQuestMonsterKillInfo
{
public:
	CQuestMonsterKillInfo() {};
	virtual ~CQuestMonsterKillInfo(){};

	void DGReqMonsterKillInfo(LPOBJ lpObj);
	void GDAnsMonsterKillInfo(LPBYTE aRecv);
	void SaveMonsterKillInfo(LPOBJ lpObj);
};

extern CQuestMonsterKillInfo g_MonsterKillInfo;

#if (__QUEST_SYSTEM__ == 1)

#define MAX_QUESTEX_ACTION		3
#define MAX_QUESTEX_PROGRESS	200
#define MAX_QUESTEX_CONDITION	5
#define MAX_QUESTEX_REWARD		5
#define MAX_QUESTEX_INFO		140

#define QUESTEX_NONE	0
#define QUESTEX_ACCEPT	1
#define QUESTEX_START	2

#define ACTION_START	0
#define ACTION_MSG		1

struct QUESTEX_STATE_INFO
{
	WORD aQuest;
	WORD aGroup;
};

struct PMSG_QUESTEX_ANS_STATE
{
	PBMSG_HEAD2	h;
	BYTE btCount;
};

#pragma pack(push)
#pragma pack(1)

struct QUESTEX_CONDITION_INFO
{
	int iType;
	WORD ReqId;
	int Count;
	int CurCount;
	BYTE ItemInfo[12];
};

struct QUESTEX_REWARD_INFO
{
	int iType;
	WORD RewardId;
	int Count;
	BYTE ItemInfo[12];
};

struct PMSG_QUESTEX_CONDITION
{
	PBMSG_HEAD2	h;
	QUESTEX_STATE_INFO	Quest;
	BYTE btConCount;
	BYTE btRewCount;
	BYTE btTmp;

	QUESTEX_CONDITION_INFO	pCondition[5];
	QUESTEX_REWARD_INFO		pReward[5];
};

#pragma pack(pop)

typedef struct
{
	short QuestId;
	short MinLevel;
	short MaxLevel;
	BYTE btAction[MAX_QUESTEX_ACTION];
	BYTE btValue[MAX_QUESTEX_ACTION];

	BYTE btClass[MAX_TYPE_PLAYER];
}QUESTEX_PROGRESS_ACTION, *LP_QUESTEX_ACTION;

struct QUESTEX_PROGRESS_GROUP
{
	int m_iQuestInfo;
	int NpcNumber1;
	int NpcNumber2;
	QUESTEX_PROGRESS_ACTION QuestAction[MAX_QUESTEX_PROGRESS];
};

struct QUESTEX_INFO_REQUIREMENT
{
	int Type;
	WORD reqType;
	WORD reqSubType;
	WORD reqLevel;
	int reqCount;
};

struct QUESTEX_INFO_REWARD
{
	int Type;
	int rewType;
	int rewSubType;
	int rewLevel;
	int rewCount;
};

struct QUESTEX_INFO
{
	int							QuestIndex;
	QUESTEX_INFO_REQUIREMENT	QuestCondition[MAX_QUESTEX_CONDITION];
	QUESTEX_INFO_REWARD			QuestReward[MAX_QUESTEX_REWARD];
	BYTE						btNextState[MAX_TYPE_PLAYER];
};

struct QUESTEX_GROUP_INFO
{
	int Count;
	QUESTEX_INFO	QuestInfo[MAX_QUESTEX_INFO];
};

class CQuestInfoEx
{
public:
	CQuestInfoEx();
	virtual ~CQuestInfoEx();

	void LoadQuestProgress(char* lpszFileName);
	void LoadQuestInfo(char* lpszFileName);

	void GCQuestInit(int aIndex);
	void CGRequestQuestState(int aIndex);
	void CGRequestInitQuest(LPBYTE aRecv,int aIndex);
	void CGQuestComplete(LPBYTE aRecv,int aIndex);
	void CGQuitQuest(LPBYTE aRecv,int aIndex);
	void CGRequestQuestStep(LPBYTE aRecv,int aIndex);
	void CGClientAction(LPBYTE aRecv,int aIndex);
	void CGReqQuestInfo(LPBYTE aRecv,int aIndex);

	void GDSaveQuestInfo(LPOBJ lpObj);
	void GDReqQuestInfo(LPOBJ lpObj);
	void DGAnsQuestInfo(LPBYTE aRecv);

	void GCSendAvaibleQuest(int aIndex);
	void GCSendQuestAction(int aIndex,int QuestGroup,int QuestIndex);
	void GCSendQuestStart(int aIndex,int QuestGroup,int QuestIndex);
	void GCSendBuff(int aIndex);
	// Quest Functions
	QUESTEX_PROGRESS_ACTION* GetQuestAction(int QuestGroup,int QuestId,LPOBJ lpObj);
	QUESTEX_PROGRESS_ACTION* GetQuestAction(QUESTEX_PROGRESS_GROUP* lpQuestGroup,LPOBJ lpObj);


	QUESTEX_INFO* GetQuestInfo(int QuestGroup,int QuestId);
	BOOL CheckConditions(QUESTEX_INFO* lpQuest,QUESTEX_USER* lpQuestUser,int aIndex);
	BOOL GiveReward(QUESTEX_INFO* lpQuest,int aIndex);
	void CreateInfoMsg(PMSG_QUESTEX_CONDITION* lpMsg,QUESTEX_INFO* lpQuest,LPOBJ lpObj);
	int GetItemCount(LPOBJ lpObj,int ItemIndex,int ItemLevel);
	int MonsterKill(LPOBJ lpNpc,LPOBJ lpObj);
public:

	//QUESTEX_PROGRESS_GROUP	m_QuestProgress[MAX_QUESTEX_GROUP];
	std::map<BYTE,QUESTEX_PROGRESS_GROUP>	m_mapQuestProgress;
	std::map<BYTE,QUESTEX_GROUP_INFO>		m_mapQuestInfo;
};

extern CQuestInfoEx gQuestInfoEx;

#endif

#endif