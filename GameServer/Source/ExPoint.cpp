#include "stdafx.h"
#include "ExPoint.h"
#include "GameMain.h"
#include "user.h"
#include "logproc.h"
// -------------------------------------------------------------------------------

ExPoint g_ExPoint;
// -------------------------------------------------------------------------------

ExPoint::ExPoint()
{

}
// -------------------------------------------------------------------------------

ExPoint::~ExPoint()
{

}
// -------------------------------------------------------------------------------

void ExPoint::Update(int UserIndex)
{
#if(__NOVUS__ == 1)
	if( gObj[UserIndex].Connected != 3 )
	{
		return;
	}
	// ----
	EXPOINT_UPDATE_ANS pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0xFE, 0x03, sizeof(pResult));
	pResult.ExPointPvP = gObj[UserIndex].m_ExPointPvP;
	pResult.ExPointEvent = gObj[UserIndex].m_ExPointEvent;
	pResult.ExPointGens = gObj[UserIndex].m_ExPointGens;
	DataSend(UserIndex, (LPBYTE)&pResult, pResult.Head.size);
#endif
}
// -------------------------------------------------------------------------------