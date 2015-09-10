// SoulSystem.cpp: implementation of the SoulSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SoulSystem.h"
#include "GameMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SoulSystem g_SoulSystem;

SoulSystem::SoulSystem()
{

}

SoulSystem::~SoulSystem()
{

}

DWORD SoulSystem::IsMap(int MapNumber)
{
	if( MapNumber == 37 ) //Kanturu
	{
		return ITEMGET(14, 300);
	}
	else if( MapNumber == 38 ) //Kanturu Remain
	{
		return ITEMGET(14, 301);
	}
	else if( MapNumber == 56 ) //Swamp of Calmness
	{
		return ITEMGET(14, 302);
	}
	else if( MapNumber == 57 ) //Raklion
	{
		return ITEMGET(14, 303);
	}
	else if( MapNumber == 63 ) //Vulcanus
	{
		return ITEMGET(14, 304);
	}
	// ----
	return 0;
}

bool SoulSystem::WithSoul(LPOBJ lpUser)
{
	for( int n = INVENTORY_BAG_START; n < MAIN_INVENTORY_SIZE; n++ )	
	{
		if( !lpUser->pInventory[n].IsItem() )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[n].m_Type != this->IsMap(lpUser->MapNumber) )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[n].m_Durability < 1.0f )
		{
			continue;
		}
		// ----
		return true;
	}
	// ----
	return false;
}