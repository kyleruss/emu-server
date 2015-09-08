#if !defined(__ILLUSION_TEMPLE_PROCESS_H__)
#define __ILLUSION_TEMPLE_PROCESS_H__

#include "user.h"

#define IT_STATE_CLOSED	0
#define IT_STATE_PREPARE 1
#define IT_STATE_PLAYING 2
#define IT_STATE_PLAYEND 3

#define IT_MAXPLAYER	10
#define IT_TEAMMAXPLAYER	6
#define IT_MAXSTATUE	2
#define IT_MAXMONSTER	40

#define IT_ALLIED_TEAM	0
#define IT_ILLUSION_TEAM 1

#define IT_PLAYER_RANGE(x) ( ((x)<0)?FALSE:((x)>IT_MAXPLAYER-1)?FALSE:TRUE  )

struct PMSG_ILLUSIONTEMPLE_STATESEND
{
	PBMSG_HEAD2 h;
	BYTE Temple;
	BYTE State;
};

struct IT_PLAYER_STRUCT
{
	// Constructor 
	IT_PLAYER_STRUCT(){ m_Index = -1; };
	// Members

	int		m_Index;
	BYTE	m_Team;
	BYTE	m_SkillPts;
	DWORD	m_Skill1Time;
	DWORD	m_Skill2Time;

	BYTE	m_PlayerKillCount;
	BYTE	m_MonsterKillCount;
	BYTE	m_RelicSuccess;
	BYTE	m_UseSkillCnt;

	int unk;
#pragma pack(1)
	__int64 m_RewardExp;
#pragma pack()

	int m_RankPoint;
	int m_bReward;

	BYTE m_TicketPos;
	int unk2;
};

struct IT_NPC_STRUCT
{
	int m_Index;
	short m_Num;
};

class CIllusionTempleProcess
{
public:
// Member Functions
	void Reset(int iTemple);
	void UserInit(int iUser);
	void CheckSync();
	void Run(int iTICK_COUNT);

	void ProcState_CLOSE(int iTICK_COUNT);
	void ProcState_PREPARE(int iTICK_COUNT);
	void ProcState_PLAY(int iTICK_COUNT);
	void ProcState_PLAYEND(int iTICK_COUNT);

	void SetState(int State);

	void SetState_CLOSED();
	void SetState_PREPARE();
	void SetState_PLAY();
	void SetState_PLAYEND();

	void UserResetPoints();

	BYTE GetRemainTime();
	BYTE GetRemainTime2();
	BOOL GetCheckSync();
	void SetCheckSyncTrue();
	void PickUpRelic(int aIndex,BYTE pos);
	void PickUpRelicSend(LPOBJ lpObj);
	BYTE GetState();
	int GetUserCount();
	BOOL OpenState();
	void CheckUsers();
	BOOL EnterUser(int aIndex, BYTE Temple, BYTE Pos);
	void SendState(BYTE State,int aIndex);
	void ClickToStatus(LPOBJ lpNpc,LPOBJ lpObj);
	void ClickAlliedNpc(LPOBJ lpNpc,LPOBJ lpObj);
	void ClickIllusionNpc(LPOBJ lpNpc,LPOBJ lpObj);
	int GetEnterItem(int aIndex);
	void DeleteRelic(LPOBJ lpObj);
	void DropRelic(int aIndex);
	void AddNpc(int Class,int Number);
	void AddMonster(int Class,int Number);
	void ResetObject();
	void AddPlayerKillCount(int iIndex);
	void AddMonsterKillCount(int iIndex);
	void AddUseSkillCnt(int aIndex);
	BYTE AddKillPts(int iIndex,BYTE Pts);
	BYTE GetKillPts(int iIndex);
	int SetKillPts(int iIndex, BYTE Pts);
	void UseSkill(int iIndex,WORD Skill,int iTargetIndex,BYTE State);
	void SkillProc(LPOBJ lpObj);
	void UserDie(LPOBJ lpObj);
	BOOL SetProtect(LPOBJ lpObj);
	BOOL SetImobilize(LPOBJ lpObj,LPOBJ lpTargetObj);
	BOOL SetTeleport(LPOBJ lpObj);
	BOOL SetShield(LPOBJ lpObj,LPOBJ lpTargetObj);
	void ItemDrop(LPOBJ lpObj);
	void DropReward(int iIndex);
	BYTE GetLeftTime();
	int Is1stSkillActive(int iEventIndex);
	int Is2ndSkillActive(int iEventIndex);
	BYTE GetTeam(int aIndex);
	int GetRelicPlayer();
	BOOL LeaveUser(int aIndex);
	void MonsterRegen();

	void SendAllUserAnyMsg(LPBYTE lpMsg,int size);
	void OperateMiniMap(BYTE TEAM);
	void RegetStatue();
	void CalcWinRewardExp();
	void SendRewardExp();
	int LevelUp(int iUserIndex,int iAddExp);

	void SpawnStatue();
	void SpawnTeamNpc();
	void SpawnMonster();
	void SetStatueTime();

	void GCSendUseSkill(LPOBJ lpObj,int skill);
	void GCSendUseSkill(int iIndex,int iTargetIndex,WORD skill,int bSuccess);
	void RemoveEventInfo();
	void AddSuccessRelic(int aIndex);
	int GetRelicPos(int aIndex);
	int ChangeTempleUserInformation(int iUserIndex,int iIllusionTempleIndex,int iCurIndex);
	BYTE GetSkillPoint(int nIndex);

	CIllusionTempleProcess()
	{
		Temple = -1;
		State = IT_STATE_CLOSED;
		dwTICK_COUNT3 = 0;
		m_STATUE_NPC_COUNT = 0;
		m_NPC_COUNT = 0;
		InitializeCriticalSection(&m_CritSec);
		dwTICK_COUNT = -1;

		ResetObject();
	}

	virtual ~CIllusionTempleProcess()
	{
		DeleteCriticalSection(&m_CritSec);
	}

protected:
// Member variables
	int unk1;
	CRITICAL_SECTION m_CritSec;
	IT_PLAYER_STRUCT m_PlayerData[IT_MAXPLAYER];
	int UserMap[IT_MAXPLAYER];
	int UserCount;

	BYTE Temple;
	BYTE State;

	IT_NPC_STRUCT m_STATUE_NPC[IT_MAXSTATUE];
	IT_NPC_STRUCT m_ALLIED_NPC;
	IT_NPC_STRUCT m_ILLUSION_NPC;
	IT_NPC_STRUCT m_MONSTER_NPC[IT_MAXMONSTER];

	int m_OpenState;	// 390
	int iNPC_TICK_COUNT;	// 394
	int m_iRemainMinute;	// 398
	int dwTICK_COUNT;		// 39C
	int dwTICK_COUNT2;	// 3A0
	int dwTICK_COUNT3;	// 3A4
	int iUNK_SECONDS;	// 3A8
	int m_IT_END_MOVE;	// 3AC
	int m_IT_MSG_BEFORE_ENTER;	// 3B0
	int m_IT_MSG_BEFORE_START;	// 3B4

	int m_RELIC_PLAYER;		// 3B8
	BYTE m_RELIC_TEAM;		// 3BC
	BYTE m_RELIC_POS;		// 3BD

	int m_STATUE_NPC_COUNT;	// 3C0
	int m_NPC_COUNT;		// 3C4
	int m_ALLIED_TEAM_COUNT;// 3C8
	int m_ILLUSION_TEAM_COUNT;	// 3CC

	BYTE m_ALLIED_TEAM_PTS;	// 3D0
	BYTE m_ILLUSION_TEAM_PTS;	// 3D1
	BYTE m_WinTeam;	// 3D2

	BYTE m_ALLIED_TEAM[MAX_TYPE_PLAYER];	// 3D3
	BYTE m_ILLUSION_TEAM[MAX_TYPE_PLAYER];	// 3D9

	int m_bIT_CHECKSYNC;	// 3E0
	int m_IT_REWARD_SENT;	// 3E4
};

#endif