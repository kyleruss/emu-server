#include "stdafx.h"
#include "ItemPrice.h"
#include "GameMain.h"
#include "..\pugixml\pugixml.hpp"
#include "Shop.h"
// -------------------------------------------------------------------------------

using namespace pugi;
ItemPrice g_ItemPrice;
// -------------------------------------------------------------------------------

ItemPrice::ItemPrice()
{

}
// -------------------------------------------------------------------------------

ItemPrice::~ItemPrice()
{

}
// -------------------------------------------------------------------------------

void ItemPrice::Init()
{
	this->m_PriceInfo.clear();
	if( this->m_PriceInfo.capacity() > 0 )
	{
		std::vector<ItemPriceInfo>().swap(this->m_PriceInfo);
	}
}
// -------------------------------------------------------------------------------

void ItemPrice::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CUSTOM_ITEMPRICE));
}
// -------------------------------------------------------------------------------

void ItemPrice::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[ItemPrice] File %s not found!", File);
		return;
	}
	// ----
	xml_node ItemPrice = Document.child("itemprice");
	// ----
	for( xml_node Item = ItemPrice.child("item"); Item; Item = Item.next_sibling() )
	{
		ItemPriceInfo lpItem;
		lpItem.m_ItemCode = ITEMGET(Item.attribute("type").as_int(), Item.attribute("index").as_int());
		lpItem.m_ItemLevel = (BYTE)Item.attribute("level").as_int(-1);
		lpItem.m_ItemPrice = Item.attribute("price").as_uint();
		lpItem.m_Divisor = Item.attribute("divisor").as_int();
		lpItem.m_ShopNumber = Item.attribute("shop").as_int();
		if( lpItem.m_Divisor <= 0 )
		{
			lpItem.m_Divisor = 1;
		}
		this->m_PriceInfo.push_back(lpItem);
	}
}
// -------------------------------------------------------------------------------

bool ItemPrice::GetPrice(short ShopNumber, WORD ItemCode, BYTE ItemLevel, DWORD& Price, BYTE& Divisor)
{
	for( int i = 0; i < this->m_PriceInfo.size(); i++ )
	{
		if( this->m_PriceInfo[i].m_ShopNumber != (short)-1 )
		{
			if( this->m_PriceInfo[i].m_ShopNumber != ShopNumber )
			{
				continue;
			}
		}
		// ----
		if( this->m_PriceInfo[i].m_ItemCode != ItemCode )
		{
			continue;
		}
		// ----
		if( this->m_PriceInfo[i].m_ItemLevel != (BYTE)-1 )
		{
			if( this->m_PriceInfo[i].m_ItemLevel != ItemLevel )
			{
				continue;
			}
		}
		// ----
		Price = this->m_PriceInfo[i].m_ItemPrice;
		Divisor = this->m_PriceInfo[i].m_Divisor;
		return true;
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

bool ItemPrice::GetSendPrice(short ShopNumber, WORD ItemCode, BYTE ItemLevel, DWORD& Price, BYTE& Divisor)
{
	for( int i = 0; i < this->m_PriceInfo.size(); i++ )
	{
		if( this->m_PriceInfo[i].m_ShopNumber != (short)-1 )
		{
			if( this->m_PriceInfo[i].m_ShopNumber != ShopNumber )
			{
				continue;
			}
		}
		// ----
		if( this->m_PriceInfo[i].m_ItemCode != ItemCode )
		{
			continue;
		}
		// ----
		if( this->m_PriceInfo[i].m_ItemLevel != (BYTE)-1 )
		{
			if( this->m_PriceInfo[i].m_ItemLevel != ItemLevel )
			{
				continue;
			}
		}
		// ----
		Price = this->m_PriceInfo[i].m_ItemPrice;
		Divisor = this->m_PriceInfo[i].m_Divisor;
		return true;
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

void ItemPrice::SendPrice(int UserIndex, int ShopNumber)
{
	if( !gObjIsConnected(UserIndex) || ShopNumber < 0 )
	{
		return;
	}
	// ----
	const int Size = sizeof(ITEMPRICE_ANS_COUNT) 
		+ (sizeof(ITEMPRICE_ANS_INFO) * MAX_ITEM_IN_SHOP) 
		+ (sizeof(ITEMPRICE_ANS_INFO) * INVENTORY_SIZE);
	BYTE Buffer[Size] = { 0 };
	ITEMPRICE_ANS_COUNT lpAnswerCount = { 0 };
	lpAnswerCount.h.c			= 0xC2;
	lpAnswerCount.h.headcode	= 0xFD;
	lpAnswerCount.h.subcode		= 0x00; 
	int Offset = sizeof(ITEMPRICE_ANS_COUNT);
	// ----
	CShop* lpShop = g_ShopManager.Get(ShopNumber);
	if( !lpShop )
	{
		return;
	}
	// ----
	for( int i = 0; i < MAX_ITEM_IN_SHOP; i++ )
	{
		if( !lpShop->m_item[i].IsItem() )
		{
			continue;
		}
		// ----
		ITEMPRICE_ANS_INFO lpAnswerInfo = { 0 };
		lpAnswerInfo.ItemCode = lpShop->m_item[i].m_Type;
		lpAnswerInfo.ItemLevel = lpShop->m_item[i].m_Level;
		// ----
		if( !this->GetSendPrice(ShopNumber, lpAnswerInfo.ItemCode, lpAnswerInfo.ItemLevel, 
			lpAnswerInfo.ItemPrice, lpAnswerInfo.Divisor) )
		{
			continue;
		}
		// ----
		bool AlreadyInUse = false;
		// ----
		for( int ItemCount = 0; ItemCount < lpAnswerCount.Count; ItemCount++ )
		{
			int PosOffset = sizeof(ITEMPRICE_ANS_COUNT) + ItemCount * sizeof(ITEMPRICE_ANS_INFO);
			ITEMPRICE_ANS_INFO* lpItem = (ITEMPRICE_ANS_INFO*)&Buffer[PosOffset];
			// ----
			if(		lpItem->ItemCode == lpAnswerInfo.ItemCode 
				&&	lpItem->ItemLevel == lpAnswerInfo.ItemLevel )
			{
				AlreadyInUse = true;
				break;
			}
		}
		// ----
		if( AlreadyInUse )
		{
			continue;
		}
		// ----
		memcpy(&Buffer[Offset], (LPBYTE)&lpAnswerInfo, sizeof(ITEMPRICE_ANS_INFO));
		Offset += sizeof(ITEMPRICE_ANS_INFO);
		lpAnswerCount.Count++;
	}
	// ----
	for( int n = 0; n < INVENTORY_SIZE; n++ )
	{
		if( !gObj[UserIndex].pInventory[n].IsItem() )
		{
			continue;
		}
		// ----
		ITEMPRICE_ANS_INFO lpAnswerInfo = { 0 };
		lpAnswerInfo.ItemCode = gObj[UserIndex].pInventory[n].m_Type;
		lpAnswerInfo.ItemLevel = gObj[UserIndex].pInventory[n].m_Level;
		// ----
		if( !this->GetSendPrice(ShopNumber, 
			lpAnswerInfo.ItemCode, lpAnswerInfo.ItemLevel, 
			lpAnswerInfo.ItemPrice, lpAnswerInfo.Divisor) )
		{
			continue;
		}
		// ----
		bool AlreadyInUse = false;
		// ----
		for( int ItemCount = 0; ItemCount < lpAnswerCount.Count; ItemCount++ )
		{
			int PosOffset = sizeof(ITEMPRICE_ANS_COUNT) + ItemCount * sizeof(ITEMPRICE_ANS_INFO);
			ITEMPRICE_ANS_INFO* lpItem = (ITEMPRICE_ANS_INFO*)&Buffer[PosOffset];
			// ----
			if(		lpItem->ItemCode == lpAnswerInfo.ItemCode 
				&&	lpItem->ItemLevel == lpAnswerInfo.ItemLevel )
			{
				AlreadyInUse = true;
				break;
			}
		}
		// ----
		if( AlreadyInUse )
		{
			continue;
		}
		// ----
		memcpy(&Buffer[Offset], (LPBYTE)&lpAnswerInfo, sizeof(ITEMPRICE_ANS_INFO));
		Offset += sizeof(ITEMPRICE_ANS_INFO);
		lpAnswerCount.Count++;
	}
	// ----
	lpAnswerCount.h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount.h.sizeL = SET_NUMBERL(Offset);
	memcpy(Buffer, (LPBYTE)&lpAnswerCount, sizeof(ITEMPRICE_ANS_COUNT));
	DataSend(UserIndex, Buffer, Offset);
}
// -------------------------------------------------------------------------------