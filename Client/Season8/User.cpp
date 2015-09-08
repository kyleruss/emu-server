#include "stdafx.h"
#include "User.h"
// -------------------------------------------------------------------------------

User g_User;
// -------------------------------------------------------------------------------

void User::Load()
{
	this->m_BattleInfoLoaded = false;
	// ----
	SetOp((LPVOID)oIsBattleServer_Call1, (LPVOID)IsBattleServer, CALL);
	SetOp((LPVOID)oIsBattleServer_Call2, (LPVOID)IsBattleServer, CALL);
	SetOp((LPVOID)oIsBattleServer_Call3, (LPVOID)IsBattleServer, CALL);
	SetOp((LPVOID)0x004A6E07, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x00596F9A, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x005989A6, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x0059902F, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x0059DF41, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x0059E311, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x005A300D, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x0064F67F, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x0072C103, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x0078F43C, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x007D15F7, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x007D1F62, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x009CB24E, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x009DAF38, (LPVOID)IsBattleZone, CALL);
	SetOp((LPVOID)0x009E7F06, (LPVOID)IsBattleZone, CALL);
	SetByte((PVOID)oVulcanusMove, 0);
	SetOp((LPVOID)0x0063BB68, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x0063BC28, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x0062E677, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x0062E800, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x009C40D4, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x0063E136, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x0080832B, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x008085FC, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x00808674, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x009E5F86, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x009E601B, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x0085C4E7, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x00860025, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x00860055, (LPVOID)IsMasterClass, CALL);
	SetOp((LPVOID)0x009FFE23, (LPVOID)IsMasterClass, CALL);
}
// -------------------------------------------------------------------------------

void User::Refresh()
{
	
}
// -------------------------------------------------------------------------------

void User::GetBattleInfo(USER_GET_BATTLEINFO* lpData)
{
	for( int i = 0; i < MAX_BATTLE_LIST; i++ )
	{
		this->m_BattleZoneList[i] = lpData->BattleZoneList[i];
	}
	// ----
	for( int i = 0; i < MAX_BATTLE_LIST-1; i++ )
	{
		SetByte((PVOID)((oBattleMapStart + 7 * i) + 3), lpData->BattleMapList[i]);
	}
	// ----
	this->m_BattleInfoLoaded = true;
}
// -------------------------------------------------------------------------------

bool User::IsBattleZone(int MapNumber)
{
	if( !g_User.m_BattleInfoLoaded )
	{
		switch(MapNumber)
		{
		case 7:
		case 33:
		case 37:
		case 56:
		case 63:
			return true;
			break;
		}
		return false;
	}
	// ----
	for( int i = 0; i < MAX_BATTLE_LIST; i++ )
	{
		if( g_User.m_BattleZoneList[i] == MapNumber )
		{
			return true;
		}
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

bool User::IsMasterClass(BYTE Class)
{
	g_User.Refresh();
	if( pIsMasterClass(Class) && User::GetChar()->m_Level >= 400 )
	{
		return true;
	}
	return false;
}
// -------------------------------------------------------------------------------