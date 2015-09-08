#include "stdafx.h"
#include "UserDieItemDrop.h"
#include "GameMain.h"
#include "..\pugixml\pugixml.hpp"
#include "LargeRand.h"
// -------------------------------------------------------------------------------

using namespace pugi;
UserDieItemDrop g_UserDieItemDrop;
// -------------------------------------------------------------------------------

UserDieItemDrop::UserDieItemDrop()
{

}
// -------------------------------------------------------------------------------

UserDieItemDrop::~UserDieItemDrop()
{

}
// -------------------------------------------------------------------------------

void UserDieItemDrop::Init()
{
	this->m_PvPEnabled = true;
	this->m_PvPMinPK = 4;
	this->m_PvPRate = 5000;
	this->m_PvMEnabled = false;
	this->m_PvMMinPK = 4;
	this->m_PvMRate = 1500;
	this->m_RollMaxCount = 1;
	this->m_RollInventory = 0;
	this->m_RollWeapon = 1;
	this->m_RollEquip = 1;
	this->m_RollWings = 0;
	this->m_RollPet = 0;
	this->m_RollAccessory = 1;
	this->m_MapListEnabled = false;
	this->m_MapList.clear();
	if( this->m_MapList.capacity() > 0 )
	{
		std::vector<BYTE>().swap(this->m_MapList);
	}
}
// -------------------------------------------------------------------------------

void UserDieItemDrop::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CUSTOM_USERDIEITEMDROP));
}
// -------------------------------------------------------------------------------

void UserDieItemDrop::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[UserDieItemDrop] File %s not found!", File);
		return;
	}
	// ----
#pragma warning (disable : 4800)
	xml_node UserDieItemDrop = Document.child("userdieitemdrop");
	this->m_PvPEnabled = UserDieItemDrop.child("pvp").attribute("enabled").as_int();
	this->m_PvPMinPK = UserDieItemDrop.child("pvp").attribute("minpklevel").as_int();
	this->m_PvPRate = UserDieItemDrop.child("pvp").attribute("rate").as_int();
	this->m_PvMEnabled = UserDieItemDrop.child("pvm").attribute("enabled").as_int();
	this->m_PvMMinPK = UserDieItemDrop.child("pvm").attribute("minpklevel").as_int();
	this->m_PvMRate = UserDieItemDrop.child("pvm").attribute("rate").as_int();
	xml_node Common = UserDieItemDrop.child("common");
	this->m_MapListEnabled = Common.child("maplist").text().as_int();
	this->m_RollMaxCount = Common.child("maxitem").text().as_int();
	xml_node DropSlots = UserDieItemDrop.child("dropslots");
	this->m_RollInventory = DropSlots.child("inventory").text().as_int();
	this->m_RollWeapon = DropSlots.child("weapon").text().as_int();
	this->m_RollEquip = DropSlots.child("equipment").text().as_int();
	this->m_RollWings = DropSlots.child("wings").text().as_int();
	this->m_RollPet = DropSlots.child("pet").text().as_int();
	this->m_RollAccessory = DropSlots.child("accessory").text().as_int();
	// ----
	for( xml_node Node = UserDieItemDrop.child("maplist").child("map"); Node; Node = Node.next_sibling() )
	{
		this->m_MapList.push_back(Node.attribute("id").as_int());
	}
}
// -------------------------------------------------------------------------------

void UserDieItemDrop::Drop(LPOBJ lpUser, LPOBJ lpKiller)
{
	if( lpUser->Type != OBJ_USER )
	{
		return;
	}
	// ----
	if( !this->m_PvPEnabled && !this->m_PvMEnabled )
	{
		return;
	}
	// ----
	DWORD Rate = 0;
	// ----
	if( lpKiller->Type == OBJ_USER )
	{
		if( !this->m_PvPEnabled )
		{
			return;
		}
		// ----
		if( lpUser->m_PK_Level < this->m_PvPMinPK )
		{
			return;
		}
		// ----
		Rate = this->m_PvPRate;
	}
	// ----
	if( lpKiller->Type == OBJ_MONSTER )
	{
		if( !this->m_PvMEnabled )
		{
			return;
		}
		// ----
		if( lpUser->m_PK_Level < this->m_PvMMinPK )
		{
			return;
		}
		// ----
		Rate = this->m_PvMRate;
	}
	// ----
	if( this->m_MapListEnabled )
	{
		bool IsCorrectMap = false;
		// ----
		for( int i = 0; i < this->m_MapList.size(); i++ )
		{
			if( this->m_MapList[i] == lpUser->MapNumber )
			{
				IsCorrectMap = true;
			}
		}
		// ----
		if( !IsCorrectMap )
		{
			return;
		}
	}
	// ----
	if( rand() % 10000 >= Rate )
	{
		return;
	}
	// ----
	for( int i = 0; i < this->m_RollMaxCount; i++ )
	{
		for( int n = 0; n < INVENTORY_MAP_SIZE; n++ )
		{
			short Slot = -1;
			// ----
			if( this->m_RollInventory )
			{
				Slot = rand() % INVENTORY_SIZE;
			}
			else
			{
				Slot = rand() % INVETORY_WEAR_SIZE;
			}
			// ----
			if( Slot == 0 || Slot == 1 )
			{
				if( !this->m_RollWeapon )
				{
					continue;
				}
			}
			// ----
			if( Slot >= 2 && Slot <= 6 )
			{
				if( !this->m_RollEquip )
				{
					continue;
				}
			}
			// ----
			if( Slot == 7 )
			{
				if( !this->m_RollWings )
				{
					continue;
				}
			}
			// ----
			if( Slot == 8 )
			{
				if( !this->m_RollPet )
				{
					continue;
				}
			}
			// ----
			if( Slot >= 9 && Slot <= 11)
			{
				if( !this->m_RollAccessory )
				{
					continue;
				}
			}
			// ----
			if(		lpUser->pInventory[Slot].IsItem() 
				&&	!g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(&lpUser->pInventory[Slot]) )
			{
				PMSG_ITEMTHROW lpMsg;
				lpMsg.Ipos = Slot;
				lpMsg.px = lpUser->X;
				lpMsg.py = lpUser->Y;
				// ----
				if( CGItemDropRequest(&lpMsg, lpUser->m_Index, 1) )
				{
					LogAddTD("[UserDieItemDrop] [%s][%s] Item has been droped (Slot: %d)",
						lpUser->AccountID, lpUser->Name);
					break;
				}
			}
		}
	}
	
}
// -------------------------------------------------------------------------------