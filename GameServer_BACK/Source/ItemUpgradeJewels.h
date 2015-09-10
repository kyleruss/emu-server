#pragma once

#include "user.h"

struct ItemUpgradeJewelsRateChangeInfo
{
	BYTE	Type;
	BYTE	Level;
	BYTE	Option;
	DWORD	RateIncrease;
	DWORD	RateDecrease;

	ItemUpgradeJewelsRateChangeInfo()
	{
		this->Type			= (BYTE)-1;
		this->Level			= (BYTE)-1;
		this->Option		= (BYTE)-1;
		this->RateIncrease	= 0;
		this->RateDecrease	= 0;
	}
};

struct ItemUpgradeJewelsInfo
{
	short	ItemType;
	short	ItemIndex;
	DWORD	DefaultRate;
	BYTE	TargetLevelMin;
	BYTE	TargetLevelMax;
	BYTE	TargetRankNormal;
	BYTE	TargetRankExcellent;
	BYTE	TargetRankAncient;
	BYTE	TargetRankSocket;
	std::vector<ItemUpgradeJewelsRateChangeInfo> m_RateChangeRules;

	~ItemUpgradeJewelsInfo()
	{
		this->m_RateChangeRules.clear();
		if( this->m_RateChangeRules.capacity() > 0 )
		{
			std::vector<ItemUpgradeJewelsRateChangeInfo>().swap(this->m_RateChangeRules);
		}
	}
};

namespace ItemUpgradeJewelsRateType
{
	enum T
	{
		Luck		= 0,
		Excellent	= 1,
		Ancient		= 2,
		Socket		= 3,
		Level		= 4,
		Option		= 5
	};
};

class ItemUpgradeJewels  
{
public:
			ItemUpgradeJewels();
	virtual ~ItemUpgradeJewels();

	void	Init();
	void	Load();
	void	Read(LPSTR File);
	
	void	ProcInsert(LPOBJ lpUser, int JewelPos, int TargetPos);
	bool	ProcUpgrade(LPOBJ lpUser, int JewelPos, int TargetPos, ItemUpgradeJewelsInfo* lpJewel);
	
	bool	IsJewel(int ItemCode);

	ItemUpgradeJewelsRateChangeInfo* GetRateChangeRule(ItemUpgradeJewelsInfo* lpJewel, BYTE Type);
private:
	std::vector<ItemUpgradeJewelsInfo> m_UpradeInfo;

}; extern ItemUpgradeJewels g_ItemUpgradeJewels;
