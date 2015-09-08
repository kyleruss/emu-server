// SummonSpot.cpp: implementation of the SummonSpot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SummonSpot.h"
#include "logproc.h"
#include "GameMain.h"
#include "gObjMonster.h"
#include "Minimap.h"
#include "..\\common\\winutil.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SummonSpot g_SummonSpot;

SummonSpot::SummonSpot()
{
	this->LastSpotID = 1;
}

SummonSpot::~SummonSpot()
{

}

bool SummonSpot::DropScroll(LPOBJ lpUser, WORD ItemType)
{
#if( __NOVUS__ == 0 )
	return 0;
#else
	BYTE MapAttr = MapC[lpUser->MapNumber].GetAttr(lpUser->X, lpUser->Y);
	// ----
	if( MapAttr & 1 != 0 )
	{
		LogAddTD("[SummonSpot] [%s] [%s] Drop fail, used in safe zone",
			lpUser->AccountID, lpUser->Name);
		return false;
	}
	// ----
	int MonsterType = -1;
	// ----
	if( ItemType == ITEMGET(14, 305) )
	{
		MonsterType = 20;	//Elite Yeti
	}
	else if( ItemType == ITEMGET(14, 306) )
	{
		MonsterType = 62;	//Mutant
	}
	else if( ItemType == ITEMGET(14, 307) )
	{
		MonsterType = 291;	//Fire Golem
	}
	// ----
	for( int i = 0; i < 7; i++ )
	{
		int MonsterIndex = gObjAddMonster(lpUser->MapNumber);
		// ----
		if( MonsterIndex < 0 )
		{
			LogAddTD("[SummonSpot] [%s] [%s] Drop fail, monster index error",
				lpUser->AccountID, lpUser->Name);
			return false;
		}
		// ----
		gObj[MonsterIndex].m_SummonTick		= GetTickCount();
		gObj[MonsterIndex].m_SummonIndex	= lpUser->m_Index;
		gObj[MonsterIndex].m_SummonID		= this->LastSpotID;
		// ----
		gObj[MonsterIndex].m_PosNum		= -1;
		gObj[MonsterIndex].X			= lpUser->X + rand() % 2;
		gObj[MonsterIndex].Y			= lpUser->Y + rand() % 2;
		gObj[MonsterIndex].MapNumber	= lpUser->MapNumber;
		gObj[MonsterIndex].TX			= gObj[MonsterIndex].X;
		gObj[MonsterIndex].TY			= gObj[MonsterIndex].Y;
		gObj[MonsterIndex].m_OldX		= gObj[MonsterIndex].X;
		gObj[MonsterIndex].m_OldY		= gObj[MonsterIndex].Y;
		gObj[MonsterIndex].StartX		= gObj[MonsterIndex].X;
		gObj[MonsterIndex].StartY		= gObj[MonsterIndex].Y;
		gObj[MonsterIndex].m_ActState.Emotion		= 1;
		gObj[MonsterIndex].m_ActState.EmotionCount	= 15;
		// ----
		PMSG_SERVERCMD ServerCmd;
		PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
		ServerCmd.CmdType = 0;
		ServerCmd.X = gObj[MonsterIndex].X;
		ServerCmd.Y = gObj[MonsterIndex].Y;
		MsgSendV2(lpUser, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
		DataSend(lpUser->m_Index, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
		// ----
		LPMONSTER_ATTRIBUTE lpMonAttr = gMAttr.GetAttr(MonsterType);
		// ----
		if( lpMonAttr == NULL )
		{
			gObjDel(MonsterIndex);
			LogAddTD("[SummonSpot] [%s] [%s] Drop fail, monster attr error",
				lpUser->AccountID, lpUser->Name);
			return false;
		}
		// ----
		gObjSetMonster(MonsterIndex, MonsterType);
		gObj[MonsterIndex].Dir = rand() % 8;
	}
	// ----
	this->LastSpotID++;
	// ----
	if( this->LastSpotID > 100 )
	{
		this->LastSpotID = 1;
	}
	// ----
	this->SendSpotInfo(lpUser);
	// ----
	return true;
#endif
}

bool SummonSpot::CheckSpotState(LPOBJ lpMonster)
{
#if( __NOVUS__ == 0 )
	return 0;
#else
	if( lpMonster->m_SummonTick > 0 )
	{
		DWORD WorkTime = (GetTickCount() - lpMonster->m_SummonTick) / 60000;
		// ----
		if( WorkTime >= SUMMONSPOT_USE_TIME )
		{
			gObjViewportListProtocolDestroy(lpMonster);
			gObjViewportClose(lpMonster);
			gObjDel(lpMonster->m_Index);
			lpMonster->m_SummonIndex = -1;
			lpMonster->m_SummonTick = 0;
			lpMonster->m_SummonID = 0;
			lpMonster->Live = FALSE;
			lpMonster->DieRegen = TRUE;
			// ----
			LogAddTD("[SummonSpot] [%d] [%d] Monster has been deleted", WorkTime, lpMonster->m_Index);
			return true;
		}
	}
	// ----
	return false;
#endif
}

void SummonSpot::SendSpotInfo(LPOBJ lpUser)
{
#if( __NOVUS__ == 0 )
	return;
#else
	BYTE TempSpotID = 0;
	// ----
	for( int i = 0; i < OBJMAX; i++ )
	{
		if(		gObj[i].m_SummonTick
			&&	gObj[i].m_SummonID
			&&	gObj[i].m_SummonIndex == lpUser->m_Index )
		{
			if( TempSpotID == gObj[i].m_SummonID )
			{
				continue;
			}
			// ----
			DWORD WorkTime = (GetTickCount() - gObj[i].m_SummonTick) / 60000;
			// ----
			PMSG_MINIMAP_SEND_NPC pMsg;
			PHeadSubSetB((LPBYTE)&pMsg, 0xE7, 0x03, sizeof(PMSG_MINIMAP_SEND_NPC));
			pMsg.btNum		= 150 + gObj[i].m_SummonID;
			pMsg.btGroup	= 1;
			pMsg.btAction	= 0;
			pMsg.btType		= 3;
			pMsg.btX		= gObj[i].StartX;
			pMsg.btY		= gObj[i].StartY;
			sprintf(pMsg.szName, "Own Spot [%s] %d", gObj[i].Name, WorkTime);
			DataSend(lpUser->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
			LogAddTD("[SummonSpot] [%s] [%s] (%s) Sended mapinfo", lpUser->AccountID, lpUser->Name, pMsg.szName);
		}
	}
#endif
}