#if !defined(__ILLUSION_TEMPLE_EVENT_H__)
#define __ILLUSION_TEMPLE_EVENT_H__

#include "user.h"
#include "IllusionTempleProcess.h"

#define IT_STARTMAP	45
#define IT_MAXTEMPLE 6
#define IT_TEAMCOUNT 2

#define IT_TEMPLE_RANGE(x) ( ((x)<0)?FALSE:((x)>IT_MAXTEMPLE-1)?FALSE:TRUE  )
#define IT_TICKET_LEVEL_RANGE(x)  (  ((x)<0)?FALSE:((x)>IT_MAXTEMPLE)?FALSE:TRUE )
#define IT_MAP_RANGE(value) ( (( (value)   )<MAP_INDEX_ILLUSIONTEMPLE1)?FALSE:(( (value)  )>MAP_INDEX_ILLUSIONTEMPLE6)?FALSE:TRUE  )


typedef struct
{
	int iHour;
	int iMinute;
}ILLUSION_TIME;

class CIllusionTempleEvent
{
public:
	BOOL Init();
	void LoadData();
	BOOL LoadFile(char* szFileName);
	void Run();
	void PickupRelic(BYTE MapNumber,int aIndex,BYTE Pos);
	void PickupRelicSend(LPOBJ lpObj);
	BOOL Enter(int aIndex,BYTE Temple,BYTE Pos);
	BOOL RankingCheck(int aIndex,BYTE Temple,BYTE Pos,int Level);
	BOOL LeaveUser(int aIndex,BYTE MapNumber);
	int GetEnterLevel(int aIndex);
	void ClickToStatus(BYTE MapNumber,LPOBJ lpNpc,LPOBJ lpObj);
	void ClickAlliedNpc(BYTE MapNumber,LPOBJ lpNpc,LPOBJ lpObj);
	void ClickIllusionNpc(BYTE MapNumber,LPOBJ lpNpc,LPOBJ lpObj);
	void ClickEnterNpc(LPOBJ lpNpc,LPOBJ lpObj);
	void DeleteRelic(BYTE MapNumber,LPOBJ lpObj);
	void DropRelic(BYTE MapNumber, int aIndex);
	BYTE GetState(BYTE MapNumber);
	void AddNpc(BYTE MapNumber,int Class,int Number);
	void AddMonster(BYTE MapNumber,int Class,int Number);
	void ResetObject();
	BYTE GetTeam(BYTE MapNumber,int aIndex);
	void ChaosMix(LPOBJ lpObj);
	int ChaosMixItemGet(int iIndex);
	int CharmOfLuckCount(int iIndex);
	BYTE TicketMix(int iIndex, int iLevel,int iCharmCount);
	BYTE PointAdd(int iIndex,BYTE MapNumber,BYTE Type);
	void UseSkill(int iIndex,WORD Skill,int iTargetIndex,BYTE State);
	void SkillProc(LPOBJ lpObj);
	void UserDie(LPOBJ lpObj);
	void RegenUser(LPOBJ lpObj);
	void SetMummyNpc(int iIndex);
	void SendInfo();
	int TeleportLock(int iIndex);
	void ItemDrop(LPOBJ lpObj);
	void DropReward(int iIndex);
	BYTE GetLeftTime();
	int Is1stSkillActive(int iEventIndex,BYTE MapNumber);
	int Is2ndSkillActive(int iEventIndex,BYTE MapNumber);
	int CheckCanEnter(int aIndex,int Level);
	void SetStatueTime(BYTE MapNumber);
	int ChangeTempleUserInformation(int iIndex,int iCurUserIndex,int iIllusionTempleIndex,unsigned char bMapNumber);
	BYTE GetSkillPoint(int nIndex,unsigned char bMapNumber);

	CIllusionTempleEvent();
	virtual ~CIllusionTempleEvent();
protected:

	int unk;
	CIllusionTempleProcess m_TempleData[IT_MAXTEMPLE];

	int iEnterNpc;
};

extern CIllusionTempleEvent	g_IllusionTemple;

extern std::list<ILLUSION_TIME> m_IllusionStartTime;

extern int m_iIT_TIME_MIN_OPEN;
extern int m_iIT_TIME_MIN_PREPARE;
extern int m_iIT_TIME_MIN_PLAY;
extern int m_iIT_TIME_MIN_CLOSE;

static int IT_TicketRate[IT_MAXTEMPLE] = {
	70,
	70,
	70,
	70,
	70,
	70,
};

static int IT_TicketZen[IT_MAXTEMPLE] = {
	3000000,
	5000000,
	7000000,
	9000000,
	11000000,
	13000000,
};

static int IT_REWARD_A[IT_MAXTEMPLE] = {
	300000,
	340000,
	380000,
	420000,
	460000,
	500000,
};

static int IT_REWARD_B[IT_MAXTEMPLE] = {
	50000,
	60000,
	70000,
	80000,
	90000,
	100000,
};

static int IT_REWARD_C[IT_MAXTEMPLE] = {
	600,
	1200,
	1800,
	2400,
	3000,
	3600,
};

static int IT_REWARD_D[IT_MAXTEMPLE] = {
	1200,
	2400,
	3600,
	4800,
	6000,
	7200,
};

static int IT_REWARD_E[IT_MAXTEMPLE] = {
	10,
	20,
	30,
	40,
	50,
	60
};

#endif