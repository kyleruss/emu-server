#pragma once
// -------------------------------------------------------------------------------

#include "TRandomPoolMgr.h"
// -------------------------------------------------------------------------------

#define EVENTBAG_MAX_RATE			10000
#define EVENTBAG_TYPE_ITEM			0
#define EVENTBAG_TYPE_MONSTER		1
#define EVENTBAG_TYPE_SPECIAL		2
#define EVENTBAG_EFFECT_NONE		0
#define EVENTBAG_EFFECT_FIREWORK	1
#define EVENTBAG_EFFECT_CHERRY		2
// -------------------------------------------------------------------------------

namespace EventBagSpecial
{
	enum Type
	{
		CastleMix		= 0,
		BloodCastle1	= 1,
		BloodCastle2	= 2,
		BloodCastle3	= 3,
		BloodCastle4	= 4,
		BloodCastle5	= 5,
		BloodCastle6	= 6,
		BloodCastle7	= 7,
		BloodCastle8	= 8,
		IllusionTemple1	= 9,
		IllusionTemple2	= 10,
		IllusionTemple3	= 11,
		IllusionTemple4	= 12,
		IllusionTemple5	= 13,
		IllusionTemple6	= 14,
		CherryMixWhite	= 15,
		CherryMixRed	= 16,
		CherryMixGold	= 17,
		GoldenArcher	= 18,
		ChaosCastle1	= 19,
		ChaosCastle2	= 20,
		ChaosCastle3	= 21,
		ChaosCastle4	= 22,
		ChaosCastle5	= 23,
		ChaosCastle6	= 24,
		ChaosCastle7	= 25,
		DevilSquare1	= 26,
		DevilSquare2	= 27,
		DevilSquare3	= 28,
		Acheron			= 29,
		DuelArena		= 30,
	};
};
// -------------------------------------------------------------------------------

struct EventItem
{
	EventItem()
	{
		this->m_Type			= 0;
		this->m_Index			= 0;
		this->m_MinLevel		= 0;
		this->m_MaxLevel		= 0;
		this->m_IsSkill			= false;
		this->m_IsLuck			= false;
		this->m_IsOption		= false;
		this->m_MinExcCount		= 0;
		this->m_MaxExcCount		= 0;
		this->m_IsSet			= false;
		this->m_MinSocketCount	= 0;
		this->m_MaxSocketCount	= 0;
	}
	// ----
	BYTE m_Type;
	BYTE m_Index;
	BYTE m_MinLevel;
	BYTE m_MaxLevel;
	BYTE m_IsSkill;
	BYTE m_IsLuck;
	BYTE m_IsOption;
	BYTE m_MinExcCount;
	BYTE m_MaxExcCount;
	BYTE m_IsSet;
	BYTE m_MinSocketCount;
	BYTE m_MaxSocketCount;
};
// -------------------------------------------------------------------------------

struct EventBag
{
	int m_Rate;
	int m_MaxRate;
	std::vector<EventItem> m_ItemList;
};
// -------------------------------------------------------------------------------

class EventItemBag
{
public:
			EventItemBag();
	virtual ~EventItemBag();
	// ----
	void	Init();
	void	Read(LPSTR File);
	// ----
	LPSTR	GetBagName() { return &this->m_Name[0]; };
	EventItem* GetItem(int Rate);
	bool	Open(int UserIndex, int MapNumber, BYTE X, BYTE Y);
	// ----
//private:
	bool	m_IsLoaded;
	BYTE	m_Mode;		//-> 0: item drop event, 1: monster die event, 2: special event (mix and etc)
	char	m_Name[255];
	char	m_File[MAX_PATH];
	int		m_MinMoney;
	int		m_MaxMoney;
	WORD	m_ItemType;
	WORD	m_ItemIndex;
	BYTE	m_ItemLevel;
	WORD	m_MinUserLevel;
	BYTE	m_Effect;
	WORD	m_MonsterID;
	BYTE	m_Repeat;
	int		m_SetItemRate;
	BYTE	m_Notify;
	BYTE	m_SpecialType;
	TRandomPoolMgr m_ExcellentPool;
	std::vector<EventBag> m_GroupList;
};
// -------------------------------------------------------------------------------

class EventItemBagManager
{
public:
			EventItemBagManager();
	virtual	~EventItemBagManager();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	// ----
	BYTE	OpenBox(int UserIndex, int ItemNumber, BYTE ItemLevel);
	BYTE	OpenMonster(int MonsterIndex);
	BYTE	OpenSpecial(BYTE SpecialType, int UserIndex, int MapNumber, BYTE X, BYTE Y);
	// ----
private:
	std::vector<EventItemBag> m_ItemBagList;
	// ----
}; extern EventItemBagManager g_EventItemBagManager;
// -------------------------------------------------------------------------------
