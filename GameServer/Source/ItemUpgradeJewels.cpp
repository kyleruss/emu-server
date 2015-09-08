#include "stdafx.h"
#include "ItemUpgradeJewels.h"
#include "..\pugixml\pugixml.hpp"
#include "logproc.h"
#include "GameMain.h"
#include "..\common\SetItemOption.h"

using namespace pugi;
ItemUpgradeJewels g_ItemUpgradeJewels;

ItemUpgradeJewels::ItemUpgradeJewels()
{

}

ItemUpgradeJewels::~ItemUpgradeJewels()
{

}

void ItemUpgradeJewels::Init()
{
	this->m_UpradeInfo.clear();
	if( this->m_UpradeInfo.capacity() > 0 )
	{
		std::vector<ItemUpgradeJewelsInfo>().swap(this->m_UpradeInfo);
	}
}

void ItemUpgradeJewels::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_ITEM_UPGRADE_JEWELS));
}

void ItemUpgradeJewels::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[ItemUpgradeJewels] File %s not found!", File);
		return;
	}
	// ----
	xml_node ChildCommon = Document.child("itemupgradejewels");
	xml_node Node;
	// ----
	for( Node = ChildCommon.child("jewel"); Node; Node = Node.next_sibling() )
	{
		ItemUpgradeJewelsInfo lpItem;

		lpItem.ItemType = Node.attribute("type").as_int((int)-1);
		lpItem.ItemIndex = Node.attribute("index").as_int((int)-1);
		lpItem.DefaultRate = Node.attribute("rate").as_uint();
		lpItem.TargetLevelMin = Node.child("rules").child("level").attribute("min").as_int((BYTE)-1);
		lpItem.TargetLevelMax = Node.child("rules").child("level").attribute("max").as_int((BYTE)-1);
		lpItem.TargetRankNormal = Node.child("rules").child("rank").attribute("normal").as_int((BYTE)-1);
		lpItem.TargetRankExcellent = Node.child("rules").child("rank").attribute("excellent").as_int((BYTE)-1);
		lpItem.TargetRankAncient = Node.child("rules").child("rank").attribute("ancient").as_int((BYTE)-1);
		lpItem.TargetRankSocket = Node.child("rules").child("rank").attribute("socket").as_int((BYTE)-1);
		
		lpItem.m_RateChangeRules.clear();
		if( lpItem.m_RateChangeRules.capacity() > 0 )
		{
			std::vector<ItemUpgradeJewelsRateChangeInfo>().swap(lpItem.m_RateChangeRules);
		}
		
		xml_node RateNode;

		for( RateNode = Node.child("ratechange"); RateNode; RateNode = RateNode.next_sibling() )
		{
			ItemUpgradeJewelsRateChangeInfo lpRate;
			lpRate.Type = RateNode.attribute("type").as_int((BYTE)-1);
			lpRate.Level = RateNode.attribute("level").as_int((BYTE)-1);
			lpRate.Option = RateNode.attribute("option").as_int((BYTE)-1);
			lpRate.RateIncrease = RateNode.attribute("increase").as_uint(0);
			lpRate.RateDecrease = RateNode.attribute("decrease").as_uint(0);
			lpItem.m_RateChangeRules.push_back(lpRate);
		}

		this->m_UpradeInfo.push_back(lpItem);
	}
}

void ItemUpgradeJewels::ProcInsert(LPOBJ lpUser, int JewelPos, int TargetPos)
{
	ItemUpgradeJewelsInfo* lpJewel = NULL;

	for( int i = 0; i < this->m_UpradeInfo.size(); i++ )
	{
		if( ITEMGET(this->m_UpradeInfo[i].ItemType, 
			this->m_UpradeInfo[i].ItemIndex) == lpUser->pInventory[JewelPos].m_Type)
		{
			lpJewel = &this->m_UpradeInfo[i];
			break;
		}
	}
	
	if( lpJewel == NULL )
	{
		GCReFillSend(lpUser->m_Index, (WORD)lpUser->Life, 0xFD, 1, lpUser->iShield);
		return;
	}

	if( this->ProcUpgrade(lpUser, JewelPos, TargetPos, lpJewel) )
	{
		gObjInventoryItemSet(lpUser->m_Index, JewelPos, -1);
		lpUser->pInventory[JewelPos].Clear();
		GCInventoryItemOneSend(lpUser->m_Index, TargetPos);
		GCInventoryItemDeleteSend(lpUser->m_Index, JewelPos, 1);
	}
	else
	{
		GCReFillSend(lpUser->m_Index, (WORD)lpUser->Life, 0xFD, 1, lpUser->iShield);
	}
}

bool ItemUpgradeJewels::ProcUpgrade(LPOBJ lpUser, int JewelPos, int TargetPos, ItemUpgradeJewelsInfo* lpJewel)
{
	if( JewelPos < 0 || JewelPos > MAIN_INVENTORY_SIZE-1 )
	{
		return false;
	}

	if( TargetPos < 0 || TargetPos > MAIN_INVENTORY_SIZE-1 )
	{
		return false;
	}

	if(		!lpUser->pInventory[JewelPos].IsItem()
		||	!lpUser->pInventory[TargetPos].IsItem() )
	{
		return false;
	}
	
	int JewelCode = lpUser->pInventory[JewelPos].m_Type;
	int TargetCode = lpUser->pInventory[TargetPos].m_Type;

	if(		JewelCode == ITEMGET(14, 13) 
		&&	TargetCode == ITEMGET(0, 41) )
	{
		if( gObjItemLevelUpPickAxe(lpUser, JewelPos, TargetPos) == TRUE )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if(		JewelCode == ITEMGET(14, 13) 
		&&	TargetCode == ITEMGET(13, 37) )
	{
		CItem* ItemFenrir = &lpUser->pInventory[TargetPos];

		if( ItemFenrir->m_Durability >= 255 )
		{
			return false;
		}

		if( rand()% 10000 < g_iFenrirRepairRate )
		{
			int iAddDur = rand()%150 + 50;

			if( (ItemFenrir->m_Durability + iAddDur) > 255 )
			{
				ItemFenrir->m_Durability = 255.0f;
			}
			else
			{
				ItemFenrir->m_Durability += iAddDur;
			}

			MsgOutput(lpUser->m_Index,lMsg.Get(3342),int(ItemFenrir->m_Durability));
			LogAddTD("[FENRIR REPAIR][SUCCESS] [%s][%s] - %d/255 (+%d)",lpUser->AccountID,
				lpUser->Name,ItemFenrir->m_Durability,iAddDur);
		}
		else
		{
			MsgOutput(lpUser->m_Index,lMsg.Get(3343));
			LogAddTD("[FENRIR REPAIR][FAILED] [%s][%s] - %d/255 (+%d)",lpUser->AccountID,lpUser->Name,
				ItemFenrir->m_Durability);
		}

		return true;
	}

	CItem* ItemTarget = &lpUser->pInventory[TargetPos];

	if( ItemTarget->m_bLuckySet == TRUE )
	{
		return false;
	}
	
	if( lpJewel->TargetLevelMax != (BYTE)-1
		&& ItemTarget->m_Level >= lpJewel->TargetLevelMax )
	{
		return false;
	}

	if( lpJewel->TargetLevelMin != (BYTE)-1
		&& ItemTarget->m_Level < lpJewel->TargetLevelMin )
	{
		return false;
	}
	
	if( ItemTarget->m_Type >= ITEMGET(12,36) && 
		ItemTarget->m_Type <= ITEMGET(12,40) )
	{

	}
#if (CUSTOM_WINGS == 1)
	else if( ItemTarget->m_Type >= ITEMGET(12,440) && ItemTarget->m_Type <= ITEMGET(12,445) )
	{

	}
#endif
	else if( ItemTarget->m_Type >= ITEMGET(12,41) &&
			 ItemTarget->m_Type <= ITEMGET(12,43) )
	{

	}
	else if( ItemTarget->m_Type == ITEMGET(12,49) ||
			 ItemTarget->m_Type == ITEMGET(12,50) )
	{

	}
	else if( ItemTarget->m_Type >= ITEMGET(12,200) && ItemTarget->m_Type <= ITEMGET(12,214) )
	{

	}
	else if( ItemTarget->m_Type >= ITEMGET(12,262) &&
			 ItemTarget->m_Type <= ITEMGET(12,265) )
	{

	}
	else if(!(ItemTarget->m_Type < ITEMGET(12,7)
		|| ItemTarget->m_Type == ITEMGET(13,30) )
		|| ItemTarget->m_Type == ITEMGET(4,7)
		|| ItemTarget->m_Type == ITEMGET(4,15))
	{
		return false;
	}

	if( ItemTarget->m_NewOption && lpJewel->TargetRankExcellent != 1 )
	{
		return false;
	}

	if( gSetItemOption.IsSetItem(ItemTarget->m_Type) && ItemTarget->m_SetOption > 0 
		&& lpJewel->TargetRankAncient != 1 )
	{
		return false;
	}

	if( g_SocketItem.IsSocketItem(ItemTarget->m_Type) && lpJewel->TargetRankSocket != 1 )
	{
		return false;
	}

	DWORD Rate = lpJewel->DefaultRate;

	for( int i = 0; i < lpJewel->m_RateChangeRules.size(); i++ )
	{
		bool Bonus = false;

		if(		lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Luck
			&&	ItemTarget->m_Option2 )
		{
			Bonus = true;
		}
		else if(	lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Excellent
				&&	ItemTarget->m_NewOption )
		{
			Bonus = true;
		}
		else if(	lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Ancient
				&&	gSetItemOption.IsSetItem(ItemTarget->m_Type) && ItemTarget->m_SetOption > 0 )
		{
			Bonus = true;
		}
		else if(	lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Socket
				&&	g_SocketItem.IsSocketItem(ItemTarget->m_Type) )
		{
			Bonus = true;
		}
		else if(	lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Level
				&&	lpJewel->m_RateChangeRules[i].Level != (BYTE)-1 && lpJewel->m_RateChangeRules[i].Level == ItemTarget->m_Level )
		{
			Bonus = true;
		}
		else if(	lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Option
				&&	lpJewel->m_RateChangeRules[i].Option != (BYTE)-1 && lpJewel->m_RateChangeRules[i].Option == ItemTarget->m_Option3 )
		{
			Bonus = true;
		}

		if( Bonus )
		{
			Rate -= lpJewel->m_RateChangeRules[i].RateDecrease;
			Rate += lpJewel->m_RateChangeRules[i].RateIncrease;
		}
	}

	if( JewelCode == ITEMGET(14, 13) )
	{
		if( rand() % 10000 < Rate )
		{
			ItemTarget->m_Level++;
		
			if( ItemTarget->m_Level > lpJewel->TargetLevelMax )
			{
				ItemTarget->m_Level = lpJewel->TargetLevelMax;
			}
		}		
	}
	else if( JewelCode == ITEMGET(14, 14) )
	{
		if( rand() % 10000 < Rate )
		{
			ItemTarget->m_Level++;
		
			if( ItemTarget->m_Level > lpJewel->TargetLevelMax )
			{
				ItemTarget->m_Level = lpJewel->TargetLevelMax;
			}
		}
		else
		{
			if( ItemTarget->m_Level >= 7 )
			{
				ItemTarget->m_Level = 0;
			}
			else
			{
				ItemTarget->m_Level--;

				if( ItemTarget->m_Level < 0 )
				{
					ItemTarget->m_Level = 0;
				}
			}
		}
	}
	else if( JewelCode == ITEMGET(14, 16) )
	{
		if( ItemTarget->m_Option3 == 0 )
		{
			if(ItemTarget->m_Type >= ITEMGET(12,3) && ItemTarget->m_Type <= ITEMGET(12,6) ||
				ItemTarget->m_Type == ITEMGET(12,49) ||
				ItemTarget->m_Type == ITEMGET(12,42) )
			{
				ItemTarget->m_NewOption &= 0xDF;

				if(rand()%2)
				{
					ItemTarget->m_NewOption |= 0x20;
				}
			}
			
			if( ItemTarget->m_Type >= ITEMGET(12,36) &&
				ItemTarget->m_Type <= ITEMGET(12,40) ||
				ItemTarget->m_Type == ITEMGET(12,43) ||
				ItemTarget->m_Type == ITEMGET(12,50)
	#if (CUSTOM_WINGS == 1)
				|| 
				ItemTarget->m_Type >= ITEMGET(12,440) &&
				ItemTarget->m_Type <= ITEMGET(12,445)
	#endif
				)
			{
				ItemTarget->m_NewOption &= 0xDF;

				if(rand()%3)
				{
					ItemTarget->m_NewOption |= 0x20;
				}
				else if(rand()%3)
				{
					ItemTarget->m_NewOption |= 0x10;
				}
			}

			if( ItemTarget->m_Type >= ITEMGET(12,262) &&
				ItemTarget->m_Type <= ITEMGET(12,265) )
			{
				ItemTarget->m_NewOption &= 0xDF;

				if( rand()%2 )
				{
					ItemTarget->m_NewOption |= 0x10;
				}
			}
		}

		if( ItemTarget->m_Option3 >= 7 )
		{
			return false;
		}

		if( rand() % 10000 < Rate )
		{
			ItemTarget->m_Option3++;
		}
		else
		{
			ItemTarget->m_Option3 = 0;
		}
	}
	else if( JewelCode == ITEMGET(14, 42) )
	{
		if( g_kJewelOfHarmonySystem.m_bSystemSmeltingItem == false )
		{
			return false;
		}

		if( g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(ItemTarget) )
		{
			LogAddTD("[JewelOfHarmony][Strengten Item] Already Strengtened [%s][%s]",
				lpUser->AccountID, lpUser->Name);
			return false;
		}
		
		int iItemType = g_kJewelOfHarmonySystem._GetItemType(ItemTarget);

		if( iItemType == JEWELOFHARMONY_ITEM_TYPE_NULL )
		{
			LogAddTD("[JewelOfHarmony][Strengten Item] Strenghten Fail [%s][%s] Name[%s] Type[%d] Serial[%d] Invalid ItemType[%d]",
				lpUser->AccountID, lpUser->Name, ItemTarget->GetName(), ItemTarget->m_Type,
				ItemTarget->m_Number, iItemType);
			return false;
		}

		int iItemOption = g_kJewelOfHarmonySystem._GetSelectRandomOption(ItemTarget, iItemType);

		if( iItemOption == AT_JEWELOFHARMONY_NOT_STRENGTHEN_ITEM )
		{
			LogAddTD("[JewelOfHarmony][Strengten Item] Strenghten Fail - NOT OPTION [%s][%s] Name[%s] Type[%d] Serial[%d] ItemType[%d]",
				lpUser->AccountID, lpUser->Name, ItemTarget->GetName(), ItemTarget->m_Type,
				ItemTarget->m_Number, iItemType);
			return false;
		}
		
		int iItemOptionLevel = g_kJewelOfHarmonySystem.m_itemOption[iItemType][iItemOption].iRequireLevel;

		if( rand() % 10000 < Rate )
		{
			g_kJewelOfHarmonySystem._MakeOption(ItemTarget, iItemOption, iItemOptionLevel);
			LogAddTD("[JewelOfHarmony][Strengten Item] Strenghten Success [%s][%s] Name[%s] Type[%d] Serial[%d] Rate (%d/%d) Option %d OptionLevel %d",
				lpUser->AccountID, lpUser->Name, ItemTarget->GetName(), ItemTarget->m_Type,
				ItemTarget->m_Number, Rate, Rate,
				iItemOption, iItemOptionLevel);

			GCServerMsgStringSend(lMsg.Get(MSGGET(13, 46)), lpUser->m_Index, 1);
		}
		else
		{
			LogAddTD("[JewelOfHarmony][Strengten Item] Strenghten Fail [%s][%s] Name[%s] Type[%d] Serial[%d]  Rate (%d/%d)",
				lpUser->AccountID, lpUser->Name, ItemTarget->GetName(), ItemTarget->m_Type,
				ItemTarget->m_Number, Rate, Rate);
			GCServerMsgStringSend(lMsg.Get(MSGGET(13, 45)), lpUser->m_Index, 1);
		}
	}

	gObjMakePreviewCharSet(lpUser->m_Index);

	float levelitemdur = (float)ItemGetDurability(lpUser->pInventory[TargetPos].m_Type,
		lpUser->pInventory[TargetPos].m_Level,
		lpUser->pInventory[TargetPos].IsExtItem(),
		lpUser->pInventory[TargetPos].IsSetItem());

	lpUser->pInventory[TargetPos].m_Durability = levelitemdur * lpUser->pInventory[TargetPos].m_Durability / lpUser->pInventory[TargetPos].m_BaseDurability;

	lpUser->pInventory[TargetPos].Convert(
			lpUser->pInventory[TargetPos].m_Type,
			lpUser->pInventory[TargetPos].m_Option1,
			lpUser->pInventory[TargetPos].m_Option2,
			lpUser->pInventory[TargetPos].m_Option3,
			lpUser->pInventory[TargetPos].m_NewOption,
			lpUser->pInventory[TargetPos].m_SetOption,
			lpUser->pInventory[TargetPos].m_ItemOptionEx,0,-1,
			CURRENT_DB_VERSION);

	LogAddTD(lMsg.Get(557),lpUser->AccountID,lpUser->Name,lpUser->pInventory[JewelPos].m_Number,
			lpUser->pInventory[TargetPos].GetName(),lpUser->pInventory[TargetPos].m_Number,
			lpUser->pInventory[TargetPos].m_Level);
	return true;
}

bool ItemUpgradeJewels::IsJewel(int ItemCode)
{
	for( int i = 0; i < m_UpradeInfo.size(); i++ )
	{
		if( ITEMGET(m_UpradeInfo[i].ItemType, 
			m_UpradeInfo[i].ItemIndex) == ItemCode)
		{
			return true;
		}
	}
	return false;
}


ItemUpgradeJewelsRateChangeInfo* ItemUpgradeJewels::GetRateChangeRule(ItemUpgradeJewelsInfo* lpJewel, BYTE Type)
{
	for( int i = 0; i < lpJewel->m_RateChangeRules.size(); i++ )
	{
		if( lpJewel->m_RateChangeRules[i].Type == Type )
		{
			return &lpJewel->m_RateChangeRules[i];
		}
	}
	return NULL;
}