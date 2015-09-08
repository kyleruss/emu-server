#pragma once
// -------------------------------------------------------------------------------

#include "ArcaBattleProtocol.h"
//#include "ProbabilityItemBag.h"
#include "user.h"
// -------------------------------------------------------------------------------

#define AB_DEBUG_TRACE		0
// -------------------------------------------------------------------------------

#define AB_MAX_PLAYER		300
#define AB_MAX_INFOLOG		10
#define AB_MAX_GATE			10
#define AB_MAX_BOOTYMIX		10
#define AB_MAX_BATTLEPOS	5
#define AB_MAX_AURAPOS		3
#define AB_MAX_AURASTATE	2
#define AB_MAX_MONGROUP		50
#define AB_MAX_MONCOUNT		255
// -------------------------------------------------------------------------------

#define AB_REW_BUFF_TIME	604800
#define AB_REW_GENS_BOSS	30
#define AB_REW_GENS_USER1	5	//Target level == User level
#define AB_REW_GENS_USER2	7	//Target level - User level >= 51
#define AB_REW_GENS_USER3	6	//Target level - User level >= 31 && <= 50
#define AB_REW_GENS_USER4	6	//Target level - User level >= 11 && <= 30
#define AB_REW_GENS_USER5	5	//Target level - User level >= 0 && <= 10
#define AB_REW_GENS_USER6	1	//User level - Target level >= 51
#define AB_REW_GENS_USER7	2	//User level - Target level >= 31 && <= 50
#define AB_REW_GENS_USER8	3	//User level - Target level >= 11 && <= 30
#define AB_REW_GENS_USER9	5	//User level - Target level >= 0 && <= 10
#define AB_REW_KILL_OBELISK	50
#define AB_REW_KILL_MONSTER	5
#define AB_REW_KILL_USER	10
#define AB_REW_KILL_TROPHY	ITEMGET(13, 147)
#define AB_REW_KILL_EXP1	300000	//Kill points >= 0 && Kill points <= 10
#define AB_REW_KILL_EXP2	500000	//Kill points >= 11 && Kill points <= 50
#define AB_REW_KILL_EXP3	700000	//Kill points >= 51 && Kill points <= 100
#define AB_REW_KILL_EXP4	1000000	//Kill points >= 101 && Kill points <= 300
#define AB_REW_KILL_EXP5	2000000	//Kill points >= 301 && Kill points <= 500
#define AB_REW_KILL_EXP6	3000000	//Kill points >= 501
#define AB_REW_ITEMBAG		"EventItembag79.txt"
// -------------------------------------------------------------------------------

#define AB_OBELISK_FIRE		1
#define AB_OBELISK_WATER	2
#define AB_OBELISK_WIND		3
#define AB_OBELISK_EARTH	4
#define AB_OBELISK_DARKNESS	5
// -------------------------------------------------------------------------------

#define AB_OBSTATE_OCCUPY	1
// -------------------------------------------------------------------------------

#define AB_STATE_NONE		1
#define AB_STATE_CLOSED		2
#define AB_STATE_START		3
#define AB_STATE_REG		4
#define AB_STATE_READY		5
#define AB_STATE_PATY		6
#define AB_STATE_PLAY		7
#define AB_STATE_RESULT		8
#define AB_STATE_CLOSE		9
#define AB_STATE_END		10
// -------------------------------------------------------------------------------

#define AB_TIME_PLAY_NOT	3
#define AB_TIME_REMAIN		30000
// -------------------------------------------------------------------------------

#define AB_CHECKSLOT(X)		(((X) < 0 )?FALSE:((X) > AB_MAX_PLAYER-1)?FALSE:TRUE)
#define AB_CHECKMONGROUP(X)	(((X) < 0 )?FALSE:((X) > AB_MAX_MONGROUP-1)?FALSE:TRUE)
// -------------------------------------------------------------------------------

struct _stABAcquiredPoints
{
    DWORD	dwBootyCnt;
    DWORD	dwContributePoints;
    DWORD	dwKillPoints;
	// ----
	_stABAcquiredPoints::_stABAcquiredPoints() //Line: 100
	{
		this->Clear();
	};
	// ----
    void _stABAcquiredPoints::Clear() //Line: 106
	{
		this->dwBootyCnt			= 0;
		this->dwContributePoints	= 0;
		this->dwKillPoints			= 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagArcaBattleUserInfo
{
    char	szCharName[11];
    bool	bBootyDrop;
    _stABAcquiredPoints m_stAcquiredPoints;
	// ----
	_tagArcaBattleUserInfo::_tagArcaBattleUserInfo() //Line: 121
	{
		memset(this->szCharName, 0, 11);
		this->bBootyDrop = 0;
	};
	// ----
	void _tagArcaBattleUserInfo::Clear() //Line: 128
	{
		memset(this->szCharName, 0, 11);
		this->bBootyDrop = 0;
		this->m_stAcquiredPoints.Clear();
	};
};
// -------------------------------------------------------------------------------

struct _stABWinGuildInfo
{
	char	szGuildName[9];
	DWORD	dwGuild;
	WORD	wOccupyObelisk;
	WORD	wObeliskGroup;
	// ----
	_stABWinGuildInfo::_stABWinGuildInfo() //Line: 145
	{
		memset(this->szGuildName, 0, 9);
		this->dwGuild			= 0;
		this->wOccupyObelisk	= 0;
		this->wObeliskGroup		= 0;
	};
	// ----
	void _stABWinGuildInfo::Clear() //Line: 155
	{
		memset(this->szGuildName, 0, 9);
		this->dwGuild			= 0;
		this->wOccupyObelisk	= 0;
		this->wObeliskGroup		= 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagARCA_BATTLE_BOOTY_REWARD
{
    int		iItemLink;
    int		iItemType;
    int		iItemIndex;
    int		iItemLevel;
    int		iDurab;
    int		iRate;
	// ----
	_tagARCA_BATTLE_BOOTY_REWARD::_tagARCA_BATTLE_BOOTY_REWARD() //Line: 194
	{
		this->iItemLink		= 0;
		this->iItemType		= 0;
		this->iItemIndex	= 0;
		this->iItemLevel	= 0;
		this->iDurab		= 0;
		this->iRate			= 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagARCA_BATTLE_BOOTY_MIX
{
    int		iItemLink;
    int		iMinCnt;
    int		iMaxCnt;
    int		iBaseRate;
    int		iAddRate;
    int		iBootyRewardCnt;
    _tagARCA_BATTLE_BOOTY_REWARD stBootyReward[20];
	// ----
	_tagARCA_BATTLE_BOOTY_MIX::_tagARCA_BATTLE_BOOTY_MIX() //Line: 216
	{
		this->iItemLink			= 0;
		this->iMinCnt			= 0;
		this->iMaxCnt			= 0;
		this->iBaseRate			= 0;
		this->iAddRate			= 0;
		this->iBootyRewardCnt	= 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagARCA_BATTLE_START_TIME
{
    int		m_iHour;
    int		m_iMinute;
    BYTE	m_btEvent;
	// ----
	_tagARCA_BATTLE_START_TIME::_tagARCA_BATTLE_START_TIME() //Line: 237
	{
		this->m_iHour	= 0;
		this->m_iMinute = 0;
		this->m_btEvent = 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagAURA_INFO
{
    int		m_iGroupNumber;
    int		m_iMapIndex;
    int		m_iAuraPosX;
    int		m_iAuraPosY;
	// ----
	_tagAURA_INFO::_tagAURA_INFO() //Line: 253
	{
		this->m_iMapIndex		= -1;
		this->m_iGroupNumber	= -1;
		this->m_iAuraPosX		= -1;
		this->m_iAuraPosY		= -1;
	};
};
// -------------------------------------------------------------------------------

struct _tagOBELISK_INFO
{
    int		m_iGroupNumber;
    int		m_iAttrKind;
    int		m_iHP;
    int		m_iOccupyHP;
    int		m_iMapIndex;
    int		m_iPosX;
    int		m_iPosY;
    int		m_iMonsterNum;
    DWORD	m_dwTick;
    DWORD	m_dwRegenTime;
    _tagAURA_INFO m_stAuraInfo[3];
	// ----
	_tagOBELISK_INFO::_tagOBELISK_INFO() //Line: 278
	{
		this->m_iGroupNumber	= 0;
		this->m_iAttrKind		= 0;
		this->m_iMapIndex		= -1;
		this->m_iHP				= 0;
		this->m_iOccupyHP		= 0;
		this->m_iPosX			= -1;
		this->m_iPosY			= -1;
		this->m_iMonsterNum		= 0;
		this->m_dwTick			= 0;
		this->m_dwRegenTime		= 0;
		memset(this->m_stAuraInfo, -1, 48);	//sizeof(m_stAuraInfo)?
	};
};
// -------------------------------------------------------------------------------

struct _tagAURA_STATE
{
    int		m_iGroupNumber;
    BYTE	m_btAuraState;
    int		m_iHoverUserIndex;
    int		m_iIndex;
	// ----
	_tagAURA_STATE::_tagAURA_STATE() //Line: 302
	{
		this->m_iGroupNumber	= -1;
		this->m_btAuraState		= 0;
		this->m_iHoverUserIndex = 0;
		this->m_iIndex			= -1;
	};
};
// -------------------------------------------------------------------------------

struct _tagOBELISK_STATE
{
    int		m_iGroupNumber;
    int		m_iMonIndex;
    int		m_iAuraState;
    int		m_iAuraReleaseGuildNum;
    int		m_iObeliskState;
    int		m_iOccupyGuildNum;
    char	m_szOccupyGuildName[9];
    _tagAURA_STATE m_stAuraState[3];
	// ----
	_tagOBELISK_STATE::_tagOBELISK_STATE() //Line: 324
	{
		this->m_iGroupNumber			= -1;
		this->m_iMonIndex				= -1;
		this->m_iAuraState				= 0;
		this->m_iAuraReleaseGuildNum	= -1;
		this->m_iOccupyGuildNum			= -1;
		this->m_iObeliskState			= 0;
		memset(this->m_szOccupyGuildName, 0, 9);
		memset(this->m_stAuraState, -1, 48);
	};
	// ----
	void _tagOBELISK_STATE::Clear() //Line: 338
	{
		this->m_iGroupNumber			= -1;
		this->m_iMonIndex				= -1;
		this->m_iAuraState				= 0;
		this->m_iAuraReleaseGuildNum	= -1;
		this->m_iOccupyGuildNum			= -1;
		this->m_iObeliskState			= 0;
		memset(this->m_szOccupyGuildName, 0, 9);
		memset(this->m_stAuraState, -1, 48);
	};
};
// -------------------------------------------------------------------------------

struct _tagARCA_BATTLE_RESULT_INFO
{
    int		iGuildNum;
    int		iObeliskAttrKind;
	// ----
	_tagARCA_BATTLE_RESULT_INFO::_tagARCA_BATTLE_RESULT_INFO() //Line: 358
	{
		this->iGuildNum			= 0;
		this->iObeliskAttrKind	= 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagOccupyGuildInfo
{
    int		iObeliskGroupNum;
    int		iGuildNum;
    int		iObeliskAttrKind;
    long	nDuration;
	// ----
	_tagOccupyGuildInfo::_tagOccupyGuildInfo() //Line: 373
	{
		//Unfilled
	};
};
// -------------------------------------------------------------------------------

struct _stABMonAccountNumInfo
{
    int		m_nIndex;
    bool	bItemDrop;
	// ----
	_stABMonAccountNumInfo::_stABMonAccountNumInfo() //Line: 398
	{
		this->bItemDrop = 0;
		this->m_nIndex	= -1;
	};
};
// -------------------------------------------------------------------------------

struct _stABMonGroupInfo
{
    int		m_nGroupNumber;
    int		m_nMapNumber;
    int		m_nCnt;
    int		m_nRegenTimeMin;
    DWORD	m_dwNextRegenTime;
    _stABMonAccountNumInfo m_stMonsterAccountNumInfo[255];
	// ----
	_stABMonGroupInfo::_stABMonGroupInfo() //Line: 417
	{
		this->m_nGroupNumber	= -1;
		this->m_nMapNumber		= -1;
		this->m_nCnt			= 0;
		this->m_nRegenTimeMin	= 0;
		this->m_dwNextRegenTime = 0;
	};
};
// -------------------------------------------------------------------------------

struct _stABMonIndexInf
{
    int		m_nMonsterIndex;
	// ----
	_stABMonIndexInf::_stABMonIndexInf() //Line: 431
	{
		this->m_nMonsterIndex = -1;
	};
};
// -------------------------------------------------------------------------------

struct _stABMonRegenArea
{
    int		m_nBeginX;
    int		m_nBeginY;
    int		m_nEndX;
    int		m_nEndY;
	// ----
	_stABMonRegenArea::_stABMonRegenArea() //Line: 443
	{
		this->m_nBeginX = -1;
		this->m_nBeginY = -1;
		this->m_nEndX	= -1;
		this->m_nEndY	= -1;
	};
};
// -------------------------------------------------------------------------------

struct _stABMonPosition
{
    int		m_nGroupNumber;
    int		m_nRegenAreaCnt;
    int		m_nMonsterKindCnt;
    _stABMonRegenArea m_stRegenArea[AB_MAX_MONGROUP];
    _stABMonIndexInf m_stMonsterIndexInfo[20];
	// ----
	_stABMonPosition::_stABMonPosition() //Line: 461
	{
		this->m_nGroupNumber	= -1;
		this->m_nRegenAreaCnt	= 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagAB_USER_AREA	//Other coder
{
    int		map_number;
    int		beginX;
    int		beginY;
    int		endX;
    int		endY;
	// ----
	_tagAB_USER_AREA::_tagAB_USER_AREA() //Line: 477
	{
		this->map_number	= -1;
		this->beginX		= -1;
		this->beginY		= -1;
		this->endX			= -1;
		this->endY			= -1;
	};
};
// -------------------------------------------------------------------------------

struct _stAB_USER_POSITION //Other coder
{
    int		group_number;
    int		area_cnt;
    _tagAB_USER_AREA stUserPos[10];
	// ----
	_stAB_USER_POSITION::_stAB_USER_POSITION() //Line: 495
	{
		this->group_number	= -1;
		this->area_cnt		= 0;
	};
};
// -------------------------------------------------------------------------------

struct _stAB_GUILD_MASTER_GATE_INFO
{
    int		nGuildNum;
    int		nGateNum;
	// ----
	_stAB_GUILD_MASTER_GATE_INFO::_stAB_GUILD_MASTER_GATE_INFO() //Line: 507
	{
		this->nGuildNum = -1;
		this->nGateNum	= -1;
	};
	// ---
    void _stAB_GUILD_MASTER_GATE_INFO::Clear() //Line: 514
	{
		this->nGuildNum = -1;
		this->nGateNum	= -1;
	};
};
// -------------------------------------------------------------------------------

struct _tagAB_GUILD_MEMBER_INFO_LOG
{
    int		nGuildNum;
    bool	bLogPrint;
	// ----
	_tagAB_GUILD_MEMBER_INFO_LOG::_tagAB_GUILD_MEMBER_INFO_LOG() //Line: 529
	{
		this->nGuildNum = -1;
		this->bLogPrint = 0;
	};
	// ----
    void _tagAB_GUILD_MEMBER_INFO_LOG::Clear() //Line: 536
	{
		this->nGuildNum = -1;
		this->bLogPrint = 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_SEND_OBELISK_LIFE
{
    PBMSG_HEAD2 h;
    _stObeliskLife m_stObeliskLife[AB_MAX_BATTLEPOS];
	// ----
	_tagPMSG_SEND_OBELISK_LIFE::_tagPMSG_SEND_OBELISK_LIFE()
	{
		//_vector_constructor_iterator_
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_SEND_OBELISK_STATE
{
    PBMSG_HEAD2 h;
    BYTE btObeliskAttribute;
    BYTE btObeliskState;
    BYTE btAuraState;
};
// -------------------------------------------------------------------------------

struct _tagPMSG_AB_RESULT
{
    PBMSG_HEAD2 h;
    BYTE	btABResult;
    ULONG	dwContributePoint;
    ULONG	dwKillPoint;
    ULONG	dwRewardExp;
    WORD	wBootyCnt;
    _stABObeliskOccupy m_stABObeliskOccupy[AB_MAX_BATTLEPOS];
	// ----
	_tagPMSG_AB_RESULT::_tagPMSG_AB_RESULT()
	{
		//_vector_constructor_iterator_
	}
};

class CArcaBattle
{
public:
    		CArcaBattle();
    		~CArcaBattle();
	// ----
    void	PrintGuildMemberLog(_GUILD_INFO_STRUCT * lpGuild);
    void	EnableEvent(BOOL bDoEvent);
    void	Clear();
    void	GDReqWinGuildInfo();
    void	GDReqProcState();
    void	AddArcaBattleWinGuildInfo(_stABWinGuildInfoDS * pABWinGuildInfoDS, int iGuildCnt);
    BOOL	IsArcaBattleWinGuild(OBJECTSTRUCT & obj);
    BOOL	IsArcaBattleOccupyZone(OBJECTSTRUCT & obj, int gt);
    void	SendArcaBattleOccupyZone(OBJECTSTRUCT & obj, int gt);
    BOOL	LoadScriptBootyMix(LPSTR lpszFileName);
    void	BootyExchange(LPOBJ lpObj);
    int		GetBootyRewardItem(int iBootyCnt, _tagARCA_BATTLE_BOOTY_REWARD * pBootyRewardItem);
    BOOL	LoadScriptArcaBattle(LPSTR lpszFileName);
    BOOL	EnterArcaBattleEvent(int aIndex);
    int		IsDataLoadWinGuild();
	void	SetDataLoadWinGuild(BOOL bReqABWinGuildInfo) { this->m_bReqABWinGuildInfo = bReqABWinGuildInfo; }; //Line: 586
    int		IsDataLoadProcState();
	void	SetDataLoadProcState(BOOL bReqABProcState) { this->m_bReqABProcState = bReqABProcState; };
    void	ReqGuildMemberDelBuffDel(LPOBJ lpObj, LPSTR szGuildName);
    void	ReqRemoveRewardGuildBuff(LPSTR szGuildName);
	void	DGAnsRemoveRewardBuff(_tagPMSG_ANS_REMOVE_GUILD_BUFF_DS * lpMsg);
    void	RemoveRewardBuff(LPSTR szGuildName);
    void	GDReqArcaBattleJoinMemberUnderReq();
	void	DGAnsArcaBattleJoinMemberUnderReq(_tagPMSG_ANS_AB_JOIN_CANCEL_DS * lpMsg);
    void	SendArcaBattlePlayInfo(OBJECTSTRUCT & obj, WORD wGuildGroupNum);
    void	GDReqJoinMemberUnder();
    int		GetJoinMemberCnt();
    int		GetDataLoadState();
    void	SetDataLoadState(int iDataLoadState);
    void	SetState(int iAB_State);
    int		GetState();
    BOOL	DBDataRequest();
    void	Run();
    BOOL	CheckSync();
    void	ProcStateClosed();
    void	ProcStateMasterReg();
    void	ProcStateReg();
    void	ProcStateReady();
    void	ProcStatePaty();
    void	ProcStatePlaying();
    void	ProcStatePlayEnd();
    void	ProcStateResult();
    void	ProcStateChannelClose();
    void	SetStateReady();
    void	SetStateClosed();
    void	SetStatePlaying();
    void	SetStatePlayEnd();
    void	GDReqArcaBattleProcMultiCast(int iState);
    void	SendAllUserAnyMsg(LPVOID, int); //Unfilled
    void	SendMapServerGroupMsg(LPSTR lpszMsg);
    void	SetRandomObeliskAttr();
    BOOL	SetObelisk();
    BOOL	RegenObelisk();
    void	SendObeliskLife();
    void	SendObeliskState();
    void	ChkAuraUserHover();
    BOOL	IsEnableAttackObelisk(OBJECTSTRUCT & obj, int iMonNumber);
    BOOL	IsOccupyObelisk(OBJECTSTRUCT & obj, int iMonIndex);
    int		GetMaxObeliskCnt();
    BOOL	GetGuildMasterPos(OBJECTSTRUCT & obj, int nGateNum, BYTE & x, BYTE & y);
    void	SetGuildMasterGateNum(int nGateNum, int nGuildNum);
    int		GetGuildMasterGateNum(int nGuildNum);
    BOOL	GetUserPos(int nGateNum, BYTE & nOutX, BYTE & nOutY);
	void	SetUserMapInfo(LPOBJ lpObj, int nGateNum);
	int		GetArcaBattleMapIndex() {  return this->m_iArcaBattleMapIndex; };
    BOOL	IsArcaBattleServer();
    BOOL	IsArcaBattleMap(int iMapNumber);
    int		IsArcaBattleEnter();
    int		IsArcaBattlePlay();
    BOOL	IsPkEnable(OBJECTSTRUCT & obj, OBJECTSTRUCT & Tarobj);
    void	ArcaBattlePenaltyDebuff(OBJECTSTRUCT &); //Unfilled
    void	SendArcaBattleStateAll(int iState);
    void	SendArcaBattleCurrentState(OBJECTSTRUCT & obj);
    void	NotifyArcaBattle(int iNotifyNum);
    void	SendPlayResult();
    void	GDReqWinGuildInfoInsert();
    void	GDReqDeleteArcaBattleInfo();
    void	GiveRewardBuffWinningGuild();
    void	ReqRemoveRewardBuffWinningGuild();
    void	DGAnsRemoveAllRewardBuff();
    void	RemoveGuildBuff(LPSTR szGuildName, WORD wBuffIndex);
    void	DeleteArcaBattleAllMonster();
	void	DGAnsRegisteredMemberCnt(_tagPMSG_ANS_AB_REG_MEMBER_CNT_DS * lpMsg);
    WORD	GetObeliskAttrToRewardBuffIndex(int iObeliskAttr);
    void	WinGuildMemberAddBuff(LPOBJ lpObj, WORD wObeliskAttr);
    void	RegenMonster(int nGroupNumber);
    BOOL	SetPosMonster(int aIndex, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY);
    BOOL	GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, BYTE & mx, BYTE & my);
    void	DeleteMonster(int nGroupNumber);
    void	RegenMonsterRun();
    void	AddContributePoint(OBJECTSTRUCT & obj, OBJECTSTRUCT & Tarobj);
    void	AddKillPoint(OBJECTSTRUCT & obj, OBJECTSTRUCT & Tarobj);
    void	BootyItemGetCnt(OBJECTSTRUCT & obj);
    void	CalcRewardExp(OBJECTSTRUCT & obj, __int64 & iRewardExp);
    _tagArcaBattleUserInfo * GetUserInfo(LPSTR szName);
    BOOL	DropItem(LPOBJ lpObj, BYTE btMapNumber, BYTE cX, BYTE cY, int nMaxHitUser);
    void	ProbabilityLoadData();
	// ----
private:
	BOOL	m_bDoEvent;
    _tagARCA_BATTLE_BOOTY_MIX		m_stBootyMix[AB_MAX_BOOTYMIX];
    int		m_iBootyMixCnt;
    _tagArcaBattleUserInfo			m_UserData[AB_MAX_PLAYER];
    _tagOBELISK_INFO				m_stObeliskInfo[AB_MAX_BATTLEPOS];
    _tagOBELISK_STATE				m_stObeliskState[AB_MAX_BATTLEPOS];
    _tagARCA_BATTLE_RESULT_INFO		m_stArcaBattleResultInfo[AB_MAX_BATTLEPOS];
    _tagOccupyGuildInfo				m_stOccupyGuildInfo[AB_MAX_BATTLEPOS];
//    CProbabilityItemBag*			m_pAcheronProbability;
    _stABWinGuildInfo				m_stABWinGuildInfo[AB_MAX_BATTLEPOS];
    int		m_iWinGuildCnt;
    DWORD	m_dwOBELISK_REGEN_TICK_COUNT;
    BOOL	m_bObeliskDie;
    int		m_iArcaBattleLoadState;
    BOOL	m_bReqABWinGuildInfo;
    BOOL	m_bReqABProcState;
    int		m_iAB_State;
    DWORD	m_dw_TIME_MIN_OPEN;
    int		m_iAB_REMAIN_MSEC;
    int		m_iAB_TICK_COUNT;
    int		m_iAB_NOTIFY_COUNT;
    bool	m_bAB_CAN_ENTER;
    bool	m_bAB_MSG_BEFORE_PLAY;
    bool	m_bAB_MSG_BEFORE_END;
    bool	m_bAB_PLAY_START;
    bool	m_bAns_PLAY_START;
    DWORD	m_dwDbDataLoadTickCount;
    int		m_iAB_DayOfWeekCycle[7];
	SYSTEMTIME m_tmStartDate;
    int		m_iJoinGuildMemberCnt;
    DWORD	m_dwAB_TICK_COUNT;
    DWORD	m_dwAB_TICK_COUNT_SEC;
    int		m_iAB_MasterRegMin;
    int		m_iAB_RegTimeMin;
    int		m_iAB_WaitTimeMin;
    int		m_iAB_WaitPartyTimeMin;
    int		m_iAB_PlayNotifyTimeMin;
    int		m_iAB_PlayTimeMin;
    int		m_iAB_ChannelTimeMin;
    std::vector<_tagARCA_BATTLE_START_TIME> m_vtArcaBattleOpenTime;
    int		m_iCurUserCount;
    int		m_iObeliskCnt;
    int		m_iArcaBattleMapIndex;
    _stABMonPosition				m_stMonsterPosition[AB_MAX_MONGROUP];
    _stABMonGroupInfo				m_stMonsterGroupInfo[AB_MAX_MONGROUP];
    _stAB_GUILD_MASTER_GATE_INFO	m_stGMasterGateNum[AB_MAX_GATE];
    _stAB_USER_POSITION				m_stAB_UserPos[AB_MAX_BATTLEPOS];
    int		m_iAuraChk;
    _tagAB_GUILD_MEMBER_INFO_LOG	m_stGuildMemberInfoLog[AB_MAX_INFOLOG];
	// ----
}; extern CArcaBattle g_ArcaBattle;
// -------------------------------------------------------------------------------