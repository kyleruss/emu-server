#if !defined(AFX_EXPOINT_H__9CC4CAFC_1100_4D40_9455_80D99F92B0BB__INCLUDED_)
#define AFX_EXPOINT_H__9CC4CAFC_1100_4D40_9455_80D99F92B0BB__INCLUDED_
// -------------------------------------------------------------------------------

#if _MSC_VER > 1000
#pragma once
#endif
// -------------------------------------------------------------------------------

#include "..\common\winutil.h"
// -------------------------------------------------------------------------------

struct EXPOINT_UPDATE_ANS
{
	PBMSG_HEAD2		Head;
	WORD			ExPointPvP;
	WORD			ExPointEvent;
	WORD			ExPointGens;
};
// -------------------------------------------------------------------------------

class ExPoint  
{
public:
	ExPoint();
	virtual ~ExPoint();
	// ----
	void	Update(int UserIndex);
	// ----
}; extern ExPoint g_ExPoint;
// -------------------------------------------------------------------------------

#endif
// -------------------------------------------------------------------------------
