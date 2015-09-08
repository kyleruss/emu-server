#pragma once
// -------------------------------------------------------------------------------

#include "user.h"
// -------------------------------------------------------------------------------

#define ITEMDROPRATE_MAX_RATE	10000
// -------------------------------------------------------------------------------

struct ItemDropRateInfo
{
	WORD	m_ItemType;
	WORD	m_ItemIndex;
	BYTE	m_ItemMinLevel;
	BYTE	m_ItemMaxLevel;
	BYTE	m_Skill;
	BYTE	m_Luck;
	BYTE	m_Option;
	BYTE	m_Excellent;
	BYTE	m_Ancient;
	short	m_MonsterMinLevel;
	short	m_MonsterMaxLevel;
	short	m_MapNumber;
	int		m_Rate;
};
// -------------------------------------------------------------------------------

class ItemDropRate  
{
public:
			ItemDropRate();
	virtual	~ItemDropRate();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	bool	Drop(LPOBJ lpMonster);
	BYTE	GetRanExc(int ExcNumber);
	// ----
private:
	std::vector<ItemDropRateInfo> m_DropList;
	// ----
}; extern ItemDropRate g_ItemDropRate;
// -------------------------------------------------------------------------------
void	MonsterItemDrop(LPOBJ lpMonster);
// -------------------------------------------------------------------------------
