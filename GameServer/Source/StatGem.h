// StatGem.h: interface for the StatGem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATGEM_H__5A94B613_8499_4413_BF10_4D14E5AA258E__INCLUDED_)
#define AFX_STATGEM_H__5A94B613_8499_4413_BF10_4D14E5AA258E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h"

struct STATGEM_DATA
{
	WORD	ItemID;
	BYTE	EffectType;
	WORD	EffectValue;
};

namespace StatGemType
{
	enum T
	{
		DoubleDamageRate	= 0,
		ExcellentDamageRate	= 1,
		DamageDecrease		= 2,
		DefenseIncrease		= 3,
		ZenHuntIncrease		= 4,
		MaxLifeIncrease		= 5,
	};
};

#define MAX_STATGEM_COUNT	6

class StatGem  
{
public:
	StatGem();
	virtual ~StatGem();
	// ----
	bool	IsGem(WORD ItemID) { return ItemID >= ITEMGET(14, 308) && ItemID <= ITEMGET(14, 313); };
	int		GetGemCount(LPOBJ lpUser, BYTE Type);
	bool	WithGem(LPOBJ lpUser, BYTE Type);
	void	SimpleEffectAdd(LPOBJ lpUser);
private:
	STATGEM_DATA m_GemInfo[MAX_STATGEM_COUNT];
}; extern StatGem g_StatGem;

#endif // !defined(AFX_STATGEM_H__5A94B613_8499_4413_BF10_4D14E5AA258E__INCLUDED_)
