#pragma once

#include "user.h"
#include "LogToFile.h"
#include "..\common\winutil.h"

struct ResetNPC
{
	short	Class;
	short	MapNumber;
	BYTE	X;
	BYTE	Y;
};

struct ResetCleaning
{
	bool	Stats;
	bool	Points;
	bool	Skills;
	bool	Equipment;
	bool	Inventory;
	bool	Shop;
	bool	MasterLevel;
	bool	MasterPoints;
	bool	MasterSkills;
	bool	MarlonQuest;
};

struct ResetChecks
{
	bool	Equipment;
	bool	Inventory;
	bool	Shop;
};

struct ResetReward
{
	BYTE	Class;
	int		Point;
	int		WCoinC;
	int		WCoinP;
	int		WCoinG;
	int		GensP;
	int		Credits;
};

struct ResetItem
{
	short	Type;
	short	Index;
	BYTE	MinLevel;
	BYTE	MaxLevel;
	BYTE	MinDurability;
	BYTE	MaxDurability;
	BYTE	MinOption;
	BYTE	MaxOption;
	short	Luck;
	short	Skill;
	short	Excellent;
	short	Ancient;
	short	Socket;
	short	Element;
	short	Count;
};

struct ResetGroup
{
	short	MinLevel;
	short	MaxLevel;
	int		MinReset;
	int		MaxReset;
	int		Money;
	int		WCoinC;
	int		WCoinP;
	int		WCoinG;
	int		GensP;
	int		Credits;
	int		Delay;
	BYTE	PointMode;
	ResetReward RewardList[7];
	std::vector<ResetItem> ItemList;
	ResetCleaning Cleaning;
	ResetChecks Checks;
};

struct ResetData
{
	bool			m_Enabled;
	ResetNPC		m_NPC;
	
	std::vector<ResetGroup> m_GroupList;
};

namespace ResetType
{
	enum T
	{
		Normal	= 0,
		Grand	= 1,
	};
};

struct PacketResetDialog
{
	PBMSG_HEAD2 Head;
	BYTE Type;
	WORD NextReset;
	DWORD RewardPoint;
	WORD ReqLevel;
	DWORD ReqMoney;
	DWORD Delay;
	DWORD Credits;
	bool AllowCredits;
	BYTE WaitProc;
	DWORD WaitMin;
};

#pragma pack(push, 1)
struct PacketResetMessageBox
{
	PBMSG_HEAD2 Head;
	char Title[60];
	char Text[140];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PacketResetStart
{
	PBMSG_HEAD2 Head;
	BYTE Type;
	BYTE PayType;
	BYTE ProcWait;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PacketAnswerStart
{
	PBMSG_HEAD2 Head;
	BYTE Type;
	BYTE PayType;
	BYTE WaitProc;
	DWORD WaitMin;
	BYTE Result;
};
#pragma pack(pop)

class ResetSystem  
{
public:
			ResetSystem();
	virtual ~ResetSystem();

	void	Init();
	void	Load();
	void	Read(LPSTR File);

	bool	ProcDialog(LPOBJ lpUser, LPOBJ lpNpc);
	bool	ProcReset(LPOBJ lpUser, ResetType::T Type, BYTE PayType, BYTE ProcWait);

	void	GC_OpenDialog(LPOBJ lpUser, ResetType::T Type);
	void	GC_MessageBox(LPOBJ lpUser, LPSTR Title, LPSTR Text, ...);
	void	CG_RequestStart(int UserIndex, PacketResetStart* lpRequest);

private:
	bool	IsValidItemNode(ResetItem NewItem);
	bool	CheckRequirements(LPOBJ lpUser, ResetGroup* Group, ResetType::T Type, bool NotifyMessage, BYTE PayType);
	bool	CheckRequirements(LPOBJ lpUser, ResetType::T Type);
	BYTE	GetItemInventoryPos(LPOBJ lpUser, ResetItem Item);
	int		GetResetMoney(LPOBJ lpUser, ResetGroup* Group, ResetType::T Type, bool ForNextReset);
	ResetGroup*	GetResetGroup(LPOBJ lpUser, ResetType::T Type);

private:
	ResetData	m_NormalInfo;
	ResetData	m_GrandInfo;

private:
	CLogToFile* Log;
	
}; extern ResetSystem g_ResetSystem;