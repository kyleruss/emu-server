#pragma once

#include "ImperialGuardianRewardExp.h"
#include "user.h"

#define IMPERIAL_GUARDIAN_MAXZONE	4
#define IMPERIAL_STATE_READY			0
#define IMPERIAL_STATE_BEGINTIMEATTACK	1
#define IMPERIAL_STATE_BEGINWAITPLAYER	2
#define IMPERIAL_STATE_BEGINLOOTTIME	3
#define IMPERIAL_STATE_PROCWARPNEXTZONE	4
#define IMPERIAL_STATE_MISSIONFAIL		5
#define IMPERIAL_STATE_KICKALL			6
#define IMPERIAL_STATE_MISSIONCLEAR		7

#define IMPERIAL_ZONE_CHECK(value) ( (value >= 0 ) ? (value <= (IMPERIAL_GUARDIAN_MAXZONE-1)) :  FALSE )
#define IF_MAP_RANGE(value) ( ( value < MAP_INDEX_IMPERIALFORT1 ) ? FALSE : (value <= MAP_INDEX_IMPERIALFORT4) )
#define IF_MONSTER_RANGE(value) ( ( value < 504 ) ? FALSE : ( value <= 521 ) )
#define IMPERIAL_COORD_CHECK(value) ( (value >= 0 ) ? (value <= 255) :  FALSE )

struct _stMonsterIndexInfo
{
	_stMonsterIndexInfo()
	{
		m_bLiveMonster = 0;
		m_iIndex = -1;
		m_bAttack = 1;
	}


	int m_iIndex;
	BYTE m_bLiveMonster;
	bool m_bAttack;
};

struct _stZoneInfo
{
	_stZoneInfo()
	{

	}

	int m_iState;	// 0
	int m_iPartyNumber;	// 4
	BYTE btUnk0;	// 8
	BYTE m_bInitTimeAttack;	// 9
	BYTE m_bInitLootTime;	// A
	BYTE m_bInitWaitPlayer;	// B
	BYTE m_bWait;	// C
	BYTE m_bMonsterSpawn;	// D
	BYTE btUnk6;	// E

	WORD m_wMonsterRegenTable;		// 10
	int m_iMaxLevel;	// 14
	int m_iNotifyMinute;	// 18

	std::vector<_stMonsterIndexInfo> m_vtMonsterIndexInfo;	// 1C
	std::vector<int> m_vtUserIndex;	// 2C
	std::vector<int> m_vtPartyUserIndex;	// 3C

	DWORD m_dwREMAIN_MSEC;	// 4C
	DWORD m_dwREMAIN_MSEC2;	// 50
	DWORD m_dwREMAIN_MSEC3;	// 54
	int m_dwTICK_COUNT1;	// 58
	int m_dwTICK_COUNT2;	// 5C
	int m_dwTICK_COUNT3;	// 60

	std::vector<int> m_vtDestroyedGate;	// 64
};

class CImperialGuardian
{
public:
	CImperialGuardian();
	virtual ~CImperialGuardian();

	void Init();
	void InitEventDungeon();
	void LoadScript(char const* szFileName);

	bool IsEnableEvent() { return this->m_bEnabled; }

	void Run();
	void ProcReady(int nZoneIndex);
	void ProcBeginTimeAttack(int nZoneIndex);
	void ProcBeginLootTime(int nZoneIndex);
	void ProcBeginWaitPlayer(int nZoneIndex);
	void ProcAllWarpNextZone(int nZoneIndex);
	void ProcAllKick(int nZoneIndex);
	void ProcMissionFail(int nZoneIndex);
	void ProcMissionClear(int nZoneIndex);
	bool IsEmptyZone(int nZoneIndex);
	void KickInvalidUser();
	bool CheckValidUser(int nUserNumber);
	int GetZoneState(int nZoneIndex);
	bool SetZoneState(int nZoneIndex,int nState);
	void CGEnterPortal(int nUserIndex,int nDestZoneIndex);
	void RegenMonster(int nZoneIndex,int nMonsterRegenTableIndex,int nMaxUserLevel,bool bOnlyCreateGate);
	void DeleteMonster(int nZoneIndex,int nMonsterRegenTableIndex);
	void InitZone(int nZoneIndex);
	bool SetMonster(int nIndex,int MonsterClass,int nMaxLevel);
	int GetGateNumber(int nZoneIndex,int nDayOfWeek);
	int GetMapNumber(int nZoneIndex);
	void ProcCheckDungeon(int nZoneIndex);
	int GetLiveMonsterCount(int nZoneIndex);
	void CheckLiveMonster(int nZoneIndex);
	int GetCurrentZoneIndex(int nUserNumber);
	bool AddUserInZone(int nZoneIndex,int nUserIndex);
	void RemoveUserInZone(int nZoneIndex,int nUserIndex);
	void RemoveUserInAllZone(int nUserNumber);
	int GetPlayerCount(int nZoneIndex);
	int GetTotalPlayerCount();
	bool IsLastZone(int nZoneIndex);
	bool GCNotifyZoneClear(int nZoneIndex);
	bool GCNotifyAllZoneClear(int nZoneIndex);
	void GCMissionFail(int nZoneIndex);
	void GCMissionFailUseDie(int nUserIndex);
	void GCSendDataToUser(int aIndex,LPBYTE lpMsg,int nSize);
	bool GCNotifyRemainTickCount(int nZoneIndex,BYTE btMsgType,DWORD dwRemainTick);
	void SetGateBlockState(int nMapNumber,int nZoneIndex,int nX,int nY,int iGateState,int nDir);
	void GCSendCastleGateInfo(int nGateIndex,int nZoneIndex,int nUserIndex);
	void DropItem(int nIndex,int nMonsterIndex);
	int CheckOverlapMysteriousPaper(int nIndex,int nItemLevel);
	void RollBackUserPos(int nUserNumber);
	int CheckGaionOrderPaper(int nIndex);
	int CheckFullSecromicon(int nIndex);
	bool IsAttackAbleMonster(int nMonsterIndex);
	void SetAtackAbleState(int nZoneIndex,int nMonsterClass,bool bState);
	void SetDayOfWeekGM(int nDayOfWeeks);
	void SetCheatModeGM(short nCheatMode);
	void WarpZoneGM(int nUserIndex,int nZoneIndex);
	void SetTargetMoveAllMonster(int nZoneIndex,int nTargetNumber);
	void DestroyGate(int nZoneIndex,int nIndex,int nTargetIndex);
	_stZoneInfo GetZoneInfo(int nZoneIndex);
	void GCSendServerMsgAll(int nZoneIndex,char* szMsg);
	int ImperialGuardianLevelUp(int iIndex,int iAddExp);
	void RegAllPartyUser(int nZoneIndex,int nFirstEnterUserIndex);
	bool IsRegPartyUser(int nZoneIndex,int nUserIndex);
	void UserMonsterCountCheck();
	void MonsterBaseAct(LPOBJ lpObj);
	int ChangeUserIndex(int nUserIndex,int nCurrentUserIndex,int nZoneIndex);
	int SendCurStateToUser(int nUserIndex,int nZoneIndex);
	int SendCurGateInfoToUser(int nUserIndex,int nZoneIndex);
	int GetPlayUserCountRightNow(int nZoneIndex);

private:
	short m_wCheatMode;	// 4
	int m_iDayOfWeek;	// 8
	int iUnk1;	// C

	BYTE m_bScriptLoaded;		// 10
	bool m_bEnabled;	// 11
	BYTE m_bPromotion;	// 12

	_stZoneInfo m_ZoneInfo[IMPERIAL_GUARDIAN_MAXZONE];	// 14
	WORD m_wDayOfWeek;				// 1E4
	int m_iLootTime;		// 1E8
	int m_iWaitPlayerTime;	// 1EC
	int m_iTimeAttackEvent;	// 1F0
	int m_iMixNeedZen;		// 1F4
	CImperialGuardianRewardExp m_RewardExp;	// 1F8
	CRITICAL_SECTION m_CritSection;			// 210

};

extern CImperialGuardian g_ImperialGuardian;
extern int gGetPartyMaxLevel(int nPartyNumber);
extern int g_nMysteriousPaperDropRate;