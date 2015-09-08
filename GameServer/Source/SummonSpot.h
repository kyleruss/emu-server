// SummonSpot.h: interface for the SummonSpot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUMMONSPOT_H__F135BF89_24E8_47F4_897E_3E9D49E906BE__INCLUDED_)
#define AFX_SUMMONSPOT_H__F135BF89_24E8_47F4_897E_3E9D49E906BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h"

#define SUMMONSPOT_USE_TIME		5

class SummonSpot  
{
public:
			SummonSpot();
	virtual ~SummonSpot();
	// ----	
	bool	DropScroll(LPOBJ lpUser, WORD ItemType);
	bool	CheckSpotState(LPOBJ lpMonster);
	void	SendSpotInfo(LPOBJ lpUser);
	// ----
	bool	IsScroll(WORD ItemType) { return ItemType >= ITEMGET(14, 305) && ItemType <= ITEMGET(14, 307); };
	// ----
	BYTE	LastSpotID;
	// ----
}; extern SummonSpot g_SummonSpot;

#endif // !defined(AFX_SUMMONSPOT_H__F135BF89_24E8_47F4_897E_3E9D49E906BE__INCLUDED_)
