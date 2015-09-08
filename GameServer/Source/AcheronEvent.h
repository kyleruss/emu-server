#pragma once

#include "user.h"

#define AE_MAX_BATTLEPOS		5
#define AE_MAX_MONGROUP			50
#define AE_REWARD_MAX_ITEMCOUNT	5

#define AE_STATE_NONE			1
#define AE_STATE_CLOSED			2
#define AE_STATE_READY			3
#define AE_STATE_PLAYING		4
#define AE_STATE_CLOSE			5
#define AE_STATE_END			6

struct _tagAcheronEventTime
{
    _tagAcheronEventTime()
	{
		this->m_iHour = 0;
		this->m_iMinute = 0;
		this->m_btEvent = 0;
	};

    int m_iHour;
    int m_iMinute;
    BYTE m_btEvent;
};

struct _tagAE_OBELISK_INFO
{
    _tagAE_OBELISK_INFO()
	{
		this->m_iGroupNumber = 0;
		this->m_iAttrKind = 0;
		this->m_iMapIndex = 0;
		this->m_iPosX = 0;
		this->m_iPosY = 0;
		this->m_iMonsterNum = 0;
		this->m_iMonIndex = -1;
	}

    void Clear()
	{
		this->m_iMonIndex = -1;
	};

    int m_iGroupNumber;
    int m_iAttrKind;
    int m_iMapIndex;
    int m_iPosX;
    int m_iPosY;
    int m_iMonsterNum;
    int m_iMonIndex;
};

struct _stAEMonRegenArea
{
    _stAEMonRegenArea()
	{
		this->m_nBeginX = -1;
		this->m_nBeginY = -1;
		this->m_nEndX = -1;
		this->m_nEndY = -1;
	};

    int m_nBeginX;
    int m_nBeginY;
    int m_nEndX;
    int m_nEndY;
};

struct _stAEMonIndexInf
{
    _stAEMonIndexInf()
	{
		this->m_nMonsterIndex = -1;
	};

    int m_nMonsterIndex;
    int m_nGenSqe;
    int m_nMonAttr;
};

struct _stAEMonPosition
{
    _stAEMonPosition()
	{
		this->m_nGroupNumber = -1;
		this->m_nRegenAreaCnt = 0;
	};

    int m_nGroupNumber;
    int m_nRegenAreaCnt;
    int m_nMonsterKindCnt;
    _stAEMonRegenArea m_stRegenArea[AE_MAX_MONGROUP]; 
    _stAEMonIndexInf m_stMonsterIndexInfo[20];
};

struct _stAEMonAccountNumInfo
{
    _stAEMonAccountNumInfo()
	{
		this->m_nIndex = -1;
	};

    int m_nIndex;
};

struct _stAEMonGroupInfo
{
    _stAEMonGroupInfo()
	{
		this->m_nGroupNumber = -1;
		this->m_nMapNumber = -1;
		this->m_nCnt = 0;
		this->m_nRegenTimeMin = 0;
		this->m_dwNextRegenTime = 0;
		this->m_nGenTotCnt = 0;
	};

    int m_nGroupNumber;
    int m_nMapNumber;
    int m_nCnt;
    int m_nGenTotCnt;
    int m_nRegenTimeMin;
    DWORD m_dwNextRegenTime;
    _stAEMonAccountNumInfo m_stMonsterAccountNumInfo[AE_MAX_MONGROUP];
};

struct _ITEM_DROP_CNT_RATE
{
    _ITEM_DROP_CNT_RATE()
	{
		this->iCnt = 0;
		this->iRate = 0;
	};

    int iCnt;
    int iRate;
};

struct _tagPMSG_REQ_ACHERON_EVENT_ENTER
{
    PBMSG_HEAD2 h;
};

struct _tagPMSG_ANS_AE_PLAY_DS
{
    _tagPMSG_ANS_AE_PLAY_DS()
	{

	};

    PBMSG_HEAD2 h;
    BYTE btPlay;
};

struct _tagPMSG_SEND_WINDOW_CLOSE
{
    PBMSG_HEAD2 h;
};

struct _PMSG_ANS_NOTI_ENTER_EVENTMAP_ERROR
{
    PBMSG_HEAD2 h;
    int nResult;
};

struct _tagPMSG_ANS_ACHERON_EVENT_ENTER
{
    _tagPMSG_ANS_ACHERON_EVENT_ENTER()
	{
		this->btResult = 0;
	};

    PBMSG_HEAD2 h;
    BYTE btResult;
};

struct _tagPMSG_REQ_AE_PLAY_DS
{
    _tagPMSG_REQ_AE_PLAY_DS()
	{
		this->btPlay = 0;
	};

    PBMSG_HEAD2 h;
    WORD wMapSvrNum;
    BYTE btPlay;
};

class CAcheronGuardianEvent
{
public:
    CAcheronGuardianEvent();
    ~CAcheronGuardianEvent();

    int LoadScriptAcheronEvent(LPSTR lpszFileName);
    int IsInitAEProcMultiCast() { return this->m_bAEInitState; };
    void SetAcheronEventProcMultiCast(int bABInitState);
    int GetDropItemCnt();
    void Clear();
    int CheckSync();
    void Run();
    void ProcStateEnd();
    void ProcStateClosed();
    void ProcStateReady();
    void ProcStatePlaying();
    void ProcStatePlayEnd();
    void ProcStateChannelClose();
    void SetProcState(int iState) { this->m_iState = iState; };
    int GetProcState() { return this->m_iState; };
    int IsPlayStart() { return this->m_bAE_PLAY_START; };
    void SetPlayStart(BYTE btAE_PLAY_START) { this->m_bAE_PLAY_START = btAE_PLAY_START; };
    void CGReqAcheronEventEnter(_tagPMSG_REQ_ACHERON_EVENT_ENTER* lpMsg, int iIndex);
    void GDReqAcheronEventProcMultiCast(BYTE btPlay);
    void DGAnsAcheronEventProcMultiCast(_tagPMSG_ANS_AE_PLAY_DS* lpMsg);
    void SetRandomObeliskAttr();
    void GenObelisk(int iGroupNum);
    void DelObelisk();
    void DestoryObelisk(LPOBJ lpObj, BYTE btMapNumber, BYTE cX, BYTE cY, int nMaxHitUser);
    void RegenMonsterRun();
    void RegenMonster(int nGroupNumber, int iGenSeq, int iGenMonCnt);
    int SetPosMonster(int aIndex, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY, BYTE Element);
    int GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, BYTE& mx, BYTE& my/*short& mx, short& my*/);
    void ResetGenTime();
    void DeleteMonster(int nGroupNumber);
    void DeleteArcaBattleAllMonster();
    void ProbabilityLoadData();
    void SendMapServerGroupMsg(LPSTR lpszMsg);

private:
    int m_bAEInitState;
    int m_iState;
    int m_iGroupNum;
    int m_bAE_PLAY_START;
    int m_iAB_PlayTimeMin;
    int m_iAB_ChCloseMin;
    int m_iDayOfWeekCycle[7];
    std::vector<_tagAcheronEventTime> m_vtAcheronEventTime;
    int m_iREMAIN_MSEC;
    int m_iTICK_COUNT;
    _tagAE_OBELISK_INFO m_stObeliskInfo[AE_MAX_BATTLEPOS];
    _stAEMonPosition m_stMonsterPosition[AE_MAX_BATTLEPOS];
    _stAEMonGroupInfo m_stMonsterGroupInfo[AE_MAX_BATTLEPOS];
    //CProbabilityItemBag* m_pAcheronEventRewardItem;
    _ITEM_DROP_CNT_RATE m_stItemDropCntRate[AE_MAX_BATTLEPOS];
    bool m_bWindowClose;
};

extern CAcheronGuardianEvent g_AcheronGuardianEvent;