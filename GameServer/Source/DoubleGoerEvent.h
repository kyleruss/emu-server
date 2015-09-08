#if !defined(__DOUBLE_GOER_EVENT__)
#define __DOUBLE_GOER_EVENT__

#include "..\common\winutil.h"
#include "EventItemBag.h"

#define DOUBLEGOER_STATE_CLOSED		0
#define DOUBLEGOER_STATE_WAITING	1
#define DOUBLEGOER_STATE_PLAYING	2
#define DOUBLEGOER_STATE_PLAYEND	3

#define DOUBLEGOER_MAXPLAYER	5
#define DOUBLEGOER_MAXMAP		4
#define DOUBLEGOER_MAXMONSTER	100
#define DOUBLEGOER_MAXBOX		6
#define DOUBLEGOER_MAXLEVELINFO	61

#define DG_MAP_RANGE(value) ( (( (value)   )<MAP_INDEX_DOUBLEGOER1)?FALSE:(( (value)  )>MAP_INDEX_DOUBLEGOER4)?FALSE:TRUE  )

struct PMSG_DOUBLEGOER_REQ_ENTER
{
	PBMSG_HEAD2 h;
	BYTE btPos;
};

struct PMSG_DOUBLEGOER_ANS_ENTER
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_DOUBLEGOER_SET_START
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_DOUBLEGOER_MONSTERSET
{
	PBMSG_HEAD2 h;
	BYTE btMaxMon;
	BYTE btCurMon;
};

struct PMSG_DOUBLEGOER_WAVEINFO
{
	PBMSG_HEAD2 h;
	BYTE btDist;
};

struct PMSG_DOUBLEGOER_BOSSINFO
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btPos;
};

struct PMSG_DOUBLEGOER_EVENTEND
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_DOUBLEGOER_PLAYERINFO
{
	WORD aIndex;
	BYTE btUnk;
	BYTE Pos;
};

struct PMSG_DOUBLEGOER_PLAYINFO
{
	PBMSG_HEAD2 h;
	WORD EventTime;
	BYTE btCount;

	PMSG_DOUBLEGOER_PLAYERINFO m_Info[DOUBLEGOER_MAXPLAYER];
};

struct DOUBLEGOER_PLAYERDATA
{
	int aIndex;
	int Dist;
};

struct DOUBLEGOER_MONSTERINFO
{
	float LEVEL_mul;
	float HP_mul;
	float DMG_mul;
	float DEF_mul;
};

struct PMSG_DOUBLEGOER_SENDEFFECT
{
	PBMSG_HEAD h;
	BYTE btType;
	WORD wIndex;
	BYTE btEffect;
};

class CDoubleGoerEvent
{

	struct _st_reward
	{
		_st_reward()
		{
			m_ResetMin = 0;
			m_ResetMax = 0;
			m_LevelMin = 0;
			m_LevelMax = 0;
			m_SilverBox = NULL;
			m_GoldenBox = NULL;
		}
		
		int m_ResetMin;
		int m_ResetMax;
		int m_LevelMin;
		int m_LevelMax;
		EventItemBag* m_SilverBox;;
		EventItemBag* m_GoldenBox;
	};

	typedef std::vector<_st_reward> vtReward;

public:
	CDoubleGoerEvent();
	virtual ~CDoubleGoerEvent();

	void Init();
	void LoadData(char* szFileName);
	void EnterRequest(PMSG_DOUBLEGOER_REQ_ENTER* lpMsg,int aIndex);

	void Run();
	void ProcState_WAITING();
	void ProcState_PLAYING();
	void ProcState_PLAYEND();

	void SetState(int iState);
	void SetState_CLOSED();
	void SetState_WAITING();
	void SetState_PLAYING();
	void SetState_PLAYEND();

	void CompensationBox(LPOBJ lpNpc,LPOBJ lpObj);
	void GoldCompensationBox(LPOBJ lpNpc,LPOBJ lpObj);

	void SendStart();
	void SendMonsterCount();
	void SendWaveInfo();
	void SendPlayInfo();
	void SendBossInfo(int iBossIndex);
	void SendBlockInfo(int iIndex,int MapIndex,int bType);

	void SendData(LPBYTE pMsg,int size);

	void SummonMonster();
	int SummonMonster(int Id,int map,int x, int y,int MaxLevel, int MaxReset);
	BOOL SetMonster(int aIndex, int Class, int nMaxLevel, int nMaxReset);

	EventItemBag* GetRewardBox(int type, LPOBJ lpUser);

	void RemoveMonster();
	void MonsterDieHandle(LPOBJ lpObj);

	BOOL AddUser(int aIndex);
	BOOL DelUser(int aIndex);
	void CheckUser();

	BOOL AddMonster(int aIndex);
	BOOL DelMonster(int aIndex);
	BOOL AddBox(int aIndex);
	BOOL DelBox(int aIndex);

	void gObjMonsterMove(LPOBJ lpObj);

	BYTE GetOpenTime();
	BYTE GetEnterState();
	void GetPosition();

	void CalcDistance();
	int CalcDistance(int x1,int y1, int x2, int y2);
public:

	int bEnabled;
	int bLoaded;

	int m_iMirrorDropRate;
	int m_iEVENT_WAIT;
	int m_iEVENT_TIME;
	int m_iEVENT_PLAYEND;

	int m_iMONSTER_COUNT;
	int m_iMONSTER_PER_MINUTE;

	int m_iState;
	int m_iREMAIN_TIME;
	DWORD m_dwTICK_COUNT;
	int m_iCurentMap;
	int m_iPlayerCount;
	int m_iMonsterPassed;
	int m_iWaveDist;

	int m_iMONSTER_MINUTE_SPAWN;
	int m_bMSG_BEFORE_START;
	int m_bMSG_BEFORE_PLAY;
	int m_bMSG_BEFORE_TIMEOUT;
	int m_bMSG_BEFORE_END;
	int m_bSEND_PLAY_START;
	int m_bSUMMON_1ST_BOSS;
	int m_bSUMMON_2ND_BOSS;
	int m_bSUMMON_3RD_BOSS;
	int m_iBossIndex;

	CRITICAL_SECTION m_sCrit;
	int m_iLeaderIndex;
	int m_iPartyCount;
	DOUBLEGOER_PLAYERDATA m_PlayerInfo[DOUBLEGOER_MAXPLAYER];

	int m_MonsterInfo[DOUBLEGOER_MAXMONSTER];
	int m_BoxInfo[DOUBLEGOER_MAXBOX];

	int m_EventReset;
	int m_EventLevel;
	int m_iLoadedInfo;

	vtReward m_vtReward;
};

extern CDoubleGoerEvent gDoubleGoer;

#endif