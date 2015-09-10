#pragma once

#include "LogToFile.h"
#include "user.h"

struct AchievMonsterInfo
{
	short	m_MonsterID;
	short	m_MapNumber;
};

struct AchievGDUpdateMonster
{
	PBMSG_HEAD2 Head;
	char	Name[11];
	short	MonsterID;
};

struct AchievGDUpdateEvent
{
	PBMSG_HEAD2 Head;
	char	Name[11];
	BYTE	EventType;
	int		BC_Exp;
	int		BC_ExtExp;
	int		BC_Score;
	int		DS_Exp;
	int		DS_Point;
	int		CC_UKill;
	int		CC_MKill;
	int		CC_TExp;
	int		CC_RExp;
};

struct AchievGDUpdateMix
{
	PBMSG_HEAD2 Head;
	char	Name[11];
	bool	Success;
};

namespace AchievEventType
{
	enum T
	{
		BloodCastle	= 0,
		DevilSquare	= 1,
		ChaosCastle	= 2
	};
};

class Achievements
{
public:
			Achievements();
	virtual ~Achievements();

	void	Init();
	void	Load();
	void	ReadMonsterData(LPSTR File);

	void	MonsterProc(LPOBJ lpUser, LPOBJ lpMonster);
	AchievMonsterInfo* MonsterGet(LPOBJ lpMonster);
	void	GD_UpdateMonsterData(LPOBJ lpUser, LPOBJ lpMonster);

	void	GD_UpdateEventDataBC(LPOBJ lpUser, int BC_Exp, int BC_ExtExp, int BC_Score);
	void	GD_UpdateEventDataDS(LPOBJ lpUser, int DS_Exp, int DS_Point);
	void	GD_UpdateEventDataCC(LPOBJ lpUser, int CC_UKill, int CC_MKill, int CC_TExp, int CC_RExp);

	void	GD_UpdateMixData(LPOBJ lpUser, bool Success);

private:
	std::vector<AchievMonsterInfo> m_MonsterList;

}; extern Achievements g_Achievements;

#if( __4GAMERS__ == 1 )
extern CLogToFile g_AchievLog;
#endif