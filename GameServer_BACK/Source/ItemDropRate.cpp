#include "stdafx.h"
#include "ItemDropRate.h"
#include "..\pugixml\pugixml.hpp"
#include "gObjMonster.h"
#include "LargeRand.h"
#include "..\common\SetItemOption.h"
#include "ElementalSystem.h"
#include "DSProtocol.h"
#include "GameMain.h"
// -------------------------------------------------------------------------------

using namespace pugi;
ItemDropRate g_ItemDropRate;
// -------------------------------------------------------------------------------

ItemDropRate::ItemDropRate()
{
	
}
// -------------------------------------------------------------------------------

ItemDropRate::~ItemDropRate()
{

}
// -------------------------------------------------------------------------------

void ItemDropRate::Init()
{
	this->m_DropList.clear();
	if( this->m_DropList.capacity() > 0 )
	{
		std::vector<ItemDropRateInfo>().swap(this->m_DropList);
	}
}
// -------------------------------------------------------------------------------

void ItemDropRate::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CUSTOM_ITEMDROPRATE));
}
// -------------------------------------------------------------------------------

void ItemDropRate::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[ItemDropRate] File %s not found!", File);
		return;
	}
	// ----
	xml_node ItemDropRate = Document.child("itemdroprate");
	// ----
	for( xml_node Item = ItemDropRate.child("item"); Item; Item = Item.next_sibling() )
	{
		ItemDropRateInfo lpItem;
		lpItem.m_ItemType = Item.attribute("type").as_int();
		lpItem.m_ItemIndex = Item.attribute("index").as_int();
		lpItem.m_ItemMinLevel = Item.attribute("minlvl").as_int();
		lpItem.m_ItemMaxLevel = Item.attribute("maxlvl").as_int();
		lpItem.m_Skill = Item.attribute("skill").as_int();
		lpItem.m_Luck = Item.attribute("luck").as_int();
		lpItem.m_Option = Item.attribute("opt").as_int();
		lpItem.m_Excellent = Item.attribute("exc").as_int();
		lpItem.m_Ancient = Item.attribute("anc").as_int();
		lpItem.m_MonsterMinLevel = Item.attribute("monminlvl").as_int();
		lpItem.m_MonsterMaxLevel = Item.attribute("monmaxlvl").as_int();
		lpItem.m_MapNumber = Item.attribute("map").as_int();
		lpItem.m_Rate = Item.attribute("rate").as_int();
		this->m_DropList.push_back(lpItem);
	}
}
// -------------------------------------------------------------------------------

bool ItemDropRate::Drop(LPOBJ lpMonster)
{
	if( this->m_DropList.empty() )
	{
		return false;
	}
	// ----
	int MaxHitUser = gObjMonsterTopHitDamageUser(lpMonster);
	// ----
	for( int i = 0; i < this->m_DropList.size(); i++ )
	{
		if (lpMonster->Level >= this->m_DropList[i].m_MonsterMinLevel && lpMonster->Level <= this->m_DropList[i].m_MonsterMaxLevel)
		{
			if (lpMonster->MapNumber == this->m_DropList[i].m_MapNumber || this->m_DropList[i].m_MapNumber == -1)
			{
				if ((rand () % ITEMDROPRATE_MAX_RATE) < this->m_DropList[i].m_Rate)
				{
					int ItemNumber	= ITEMGET(this->m_DropList[i].m_ItemType, this->m_DropList[i].m_ItemIndex);
					int Option1		= 0;
					int Option2		= 0;
					int Option3		= 0;
					BYTE ExcOption	= 0;
					BYTE SetOption	= 0;
					int Durability	= 0;
					int Level		= 0;

					int Sub = (this->m_DropList[i].m_ItemMaxLevel - this->m_DropList[i].m_ItemMinLevel) + 1;
					Level = this->m_DropList[i].m_ItemMinLevel + (rand() % Sub);

					// ----
					if( this->m_DropList[i].m_Skill != 0 )
					{
						if( rand() % 2 )
						{
							Option1 = 1;
						}
					}
					// ----
					if( this->m_DropList[i].m_Luck != 0 )
					{
						if( rand() % 2 )
						{
							Option2 = 1;
						}
					}
					// ----
					if( this->m_DropList[i].m_Option != 0 )
					{
						if( rand() % 5 == 0 )
						{
							Option3 = 3;
						}
						else
						{
							Option3 = rand() % 3;
						}
					}
					// ----
					if( this->m_DropList[i].m_Excellent != 0 )
					{
						ExcOption = NewOptionRand(0);
					}
					// ----
					if( this->m_DropList[i].m_Ancient != 0 )
					{
						SetOption = gSetItemOption.GenSetOption(ItemNumber);
					}
					// ----
					BYTE SocketBonus = (BYTE)-1;
					BYTE SocketOption[5];
					SocketOption[0] = (BYTE)-1;
					SocketOption[1] = (BYTE)-1;
					SocketOption[2] = (BYTE)-1;
					SocketOption[3] = (BYTE)-1;
					SocketOption[4] = (BYTE)-1;
					// ----
					if( g_ElementalSystem.IsElementalItem(ItemNumber) )
					{
						SocketBonus = (lpMonster->m_btAttribute == ELEMENT_NONE) ? (1 + rand() % 5) : lpMonster->m_btAttribute;
						// ----
						if( g_ElementalSystem.IsPentagram(ItemNumber) )
						{
							int SlotCount = g_ElementalSystem.m_SlotCountRate.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
							int Count = 0;
							// ----
							while(SlotCount != Count)
							{
								int Slot = rand() % 5;
								// ----
								if( SocketOption[Slot] == (BYTE)-1)
								{
									SocketOption[Slot] = 0xFE;
									Count++;
								}
							}
						}
					}
					// ----
					ItemSerialCreateSend(lpMonster->m_Index, lpMonster->MapNumber, lpMonster->X, lpMonster->Y,
						ItemNumber, Level, Durability, Option1, Option2, Option3, MaxHitUser, ExcOption, SetOption,
						SocketBonus, SocketOption);
					return true;
					
				}
			}
		}
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------
BYTE ItemDropRate::GetRanExc(int ExcNumber)
{
	BYTE ExtCode[6] = {0x08, 0x01, 0x20, 0x02, 0x10, 0x04};
	BYTE SerialNumber;
	// ----
	srand((unsigned)time(NULL));
	// ----
	for (int i = 0; i < 6; i++)
	{
		// ----
		SerialNumber = rand() % 6;
		// ----
		BYTE bTemp	= ExtCode[SerialNumber];
		// ----
		ExtCode[SerialNumber] = ExtCode[i];
		// ----
		ExtCode[i]	= bTemp;
	}
	
	BYTE Exc = 0;
	// ----
	if (ExcNumber == 0)
	{
		return Exc;
	}
	// ----
	for (int n = 0; n < ExcNumber; n++)
	{
		Exc += ExtCode[n];
	}
	// ----
	return Exc;
}
// -------------------------------------------------------------------------------
void MonsterItemDrop(LPOBJ lpMonster)
{
	if (g_ItemDropRate.Drop(lpMonster))
		return;
}