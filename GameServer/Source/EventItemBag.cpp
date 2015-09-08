#include "stdafx.h"
#include "EventItemBag.h"
#include "..\pugixml\pugixml.hpp"
#include "GameMain.h"
#include "LargeRand.h"
#include "..\common\SetItemOption.h"
#include "DSProtocol.h"
#include "..\\common\\winutil.h"
#include "gObjMonster.h"
#include "TNotice.h"
//#include "License.h"
#include "SocketItemOption.h"
// -------------------------------------------------------------------------------

using namespace pugi;
EventItemBagManager g_EventItemBagManager;
// -------------------------------------------------------------------------------

EventItemBag::EventItemBag()
{
	
}
// -------------------------------------------------------------------------------

EventItemBag::~EventItemBag()
{

}
// -------------------------------------------------------------------------------

void EventItemBag::Init()
{
	this->m_IsLoaded = false;
	ZeroMemory(this->m_Name, sizeof(m_Name));
	ZeroMemory(this->m_File, sizeof(m_File));
	this->m_MinMoney = 0;
	this->m_MaxMoney = 0;
	this->m_Mode = -1;
	this->m_ItemType = -1;
	this->m_ItemIndex = -1;
	this->m_ItemLevel = 0;
	this->m_MinUserLevel = 0;
	this->m_Effect = EVENTBAG_EFFECT_NONE;
	this->m_MonsterID = -1;
	this->m_Repeat = 1;
	this->m_SetItemRate = 0;
	this->m_Notify = 0;
	this->m_SpecialType = -1;
	this->m_ExcellentPool.InitPool();
	this->m_GroupList.clear();
	if( this->m_GroupList.capacity() > 0 )
	{
		std::vector<EventBag>().swap(this->m_GroupList);
	}
}
// -------------------------------------------------------------------------------

void EventItemBag::Read(LPSTR File)
{
//	VMBEGIN
/*	if( g_CheckedLicense.getData() != 1 )
	{
		return;
	}*/
//	VMEND
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[EventItemBag] File %s not found!", File);
		return;
	}
	// ----
	xml_node ItemBag	= Document.child("itembag");
	xml_node Settings	= ItemBag.child("settings");
	// ----
	strncpy(this->m_Name, Settings.child("name").text().as_string(), sizeof(this->m_Name));
	this->m_MinMoney = Settings.child("zen").attribute("min").as_int();
	this->m_MaxMoney = Settings.child("zen").attribute("max").as_int();
	// ----
	if( this->m_MinMoney > this->m_MaxMoney )
	{
		MsgBox("[EventItemBag] %s\n[Error] MinMoney > MaxMoney", File);
		return;
	}
	// ----
	int Op1Rate		= Settings.child("excoption").attribute("op1").as_int();
	int Op2Rate		= Settings.child("excoption").attribute("op2").as_int();
	int Op4Rate		= Settings.child("excoption").attribute("op4").as_int();
	int Op8Rate		= Settings.child("excoption").attribute("op8").as_int();
	int Op16Rate	= Settings.child("excoption").attribute("op16").as_int();
	int Op32Rate	= Settings.child("excoption").attribute("op32").as_int();
	// ----
	this->m_ExcellentPool.InitPool();
	this->m_ExcellentPool.AddValue(1, Op1Rate);
	this->m_ExcellentPool.AddValue(2, Op2Rate);
	this->m_ExcellentPool.AddValue(4, Op4Rate);
	this->m_ExcellentPool.AddValue(8, Op8Rate);
	this->m_ExcellentPool.AddValue(16, Op16Rate);
	this->m_ExcellentPool.AddValue(32, Op32Rate);
	// ----	
	for( xml_node ItemList = ItemBag.child("itemlist"); ItemList; ItemList = ItemList.next_sibling() )
	{
		EventBag lpBag;

		lpBag.m_Rate = ItemList.attribute("rate").as_int();
		// ----
		if( this->m_GroupList.size() <= 0 )
		{
			lpBag.m_MaxRate = lpBag.m_Rate;
		}
		else
		{
			lpBag.m_MaxRate = this->m_GroupList.back().m_MaxRate + lpBag.m_Rate;
		}
		// ----	
		for( xml_node Item = ItemList.first_child(); Item; Item = Item.next_sibling() )
		{
			EventItem lpItem;
			// ----
			lpItem.m_Type		= Item.attribute("id").as_int();
			lpItem.m_Index		= Item.attribute("num").as_int();
			lpItem.m_MinLevel	= Item.attribute("lvlmin").as_int();
			lpItem.m_MaxLevel	= Item.attribute("lvlmax").as_int();
			// ----
			if( lpItem.m_MinLevel > lpItem.m_MaxLevel )
			{
				MsgBox("[EventItemBag] %s\n[Error] MinLevel > MaxLevel", File);
				return;
			}
			// ----
			lpItem.m_IsSkill		= Item.attribute("skill").as_int();
			lpItem.m_IsLuck			= Item.attribute("luck").as_int();
			lpItem.m_IsOption		= Item.attribute("option").as_int();
			lpItem.m_MinExcCount	= Item.attribute("excmin").as_int();
			lpItem.m_MaxExcCount	= Item.attribute("excmax").as_int();
			// ----
			if( lpItem.m_MinExcCount > lpItem.m_MaxExcCount )
			{
				MsgBox("[EventItemBag] %s\n[Error] MinExcCount > MaxExcCount", File);
				return;
			}
			// ----
			lpItem.m_IsSet			= Item.attribute("is_anc").as_int();
			lpItem.m_MinSocketCount = Item.attribute("sockmin").as_int();
			lpItem.m_MaxSocketCount = Item.attribute("sockmax").as_int();
			// ----
			if( lpItem.m_MinSocketCount > lpItem.m_MaxSocketCount )
			{
				MsgBox("[EventItemBag] %s\n[Error] MinSocketCount > MaxSocketCount", File);
				return;
			}
			// ----
			lpBag.m_ItemList.push_back(lpItem);
		}
		// ----
		this->m_GroupList.push_back(lpBag);
	}
	// ----
	LogAdd(lMsg.Get(MSGGET(1, 198)), File);
	this->m_IsLoaded = true;
}
// -------------------------------------------------------------------------------

EventItem* EventItemBag::GetItem(int Rate)
{
	int Group = this->m_GroupList.size();
	// ----
	while(Group--)
	{
		int MaxItem = this->m_GroupList[Group].m_ItemList.size();
		// ----
		if( MaxItem > 0 )
		{
			if( Group == 0 )
			{
				return &this->m_GroupList[Group].m_ItemList[rand() % MaxItem];
			}
			// ----
			if(		Rate >= EVENTBAG_MAX_RATE - this->m_GroupList[Group].m_MaxRate
				&&	Rate < EVENTBAG_MAX_RATE - this->m_GroupList[Group - 1].m_MaxRate )
			{
				return &this->m_GroupList[Group].m_ItemList[rand() % MaxItem];
			}
		}
	}
	// ----
	return NULL;
}
// -------------------------------------------------------------------------------

bool EventItemBag::Open(int UserIndex, int MapNumber, BYTE X, BYTE Y)
{
	if( !this->m_IsLoaded )
	{
		return false;
	}
	// ----
	BYTE Dur			= 0;
	int ItemNumber		= 0;
	int Level			= 0;
	int cX				= 0;
	int cY				= 0;
	int Option1			= 0;
	int Option2			= 0;
	int Option3			= 0;
	int ExcOption		= 0;
	int SetOption		= 0;
	int SocketCount		= 0;
	BYTE SocketBonus	= (BYTE)-1;
	BYTE SocketOption[5];
	LPOBJ lpObj			= &gObj[UserIndex];
	// ----
	if( this->m_GroupList.size() <= 0 )
	{
		return false;
	}
	// ----
	int DropRate = g_MersenneRand.IRandom(EVENTBAG_MAX_RATE);
	// ----
	if(	DropRate < EVENTBAG_MAX_RATE - this->m_GroupList.back().m_MaxRate )
	{
		int Money = this->m_MinMoney;
		// ----
		if( this->m_MinMoney != this->m_MaxMoney )
		{
			Money += g_MersenneRand.IRandom(1 + (this->m_MaxMoney - this->m_MinMoney));
		}
		// ----
		MapC[lpObj->MapNumber].MoneyItemDrop(Money, X, Y);
		LogAddTD("[%s][%s] [EventItemBag] (%s) Money drop %d",
			lpObj->AccountID, lpObj->Name, this->GetBagName(), Money);
		return true;
	}
	// ----
	int GroupCount		= this->m_GroupList.size();
	EventItem* lpItem	= this->GetItem(DropRate);
	// ----
	if( lpItem == NULL )
	{
		return false;
	}
	// ----
	if( MapNumber != 255 )
	{
		if( X == FALSE && Y == FALSE )
		{
			cX = lpObj->X;
			cY = lpObj->Y;
		}
		else
		{
			cX = X;
			cY = Y;
		}
	}
	// ----
	if( lpItem->m_MinLevel == lpItem->m_MaxLevel )
	{
		Level = lpItem->m_MinLevel;
	}
	else
	{
		int Sub = (lpItem->m_MaxLevel - lpItem->m_MinLevel) + 1;
		Level = lpItem->m_MinLevel + (rand() % Sub);
	}
	// ----
	ItemNumber = ItemGetNumberMake(lpItem->m_Type, lpItem->m_Index);
	// ----
	if( ItemNumber == -1 )
	{
		return false;
	}
	// ----
	if( lpItem->m_IsSkill )
	{
		Option1 = 1;
	}
	// ----
	if( lpItem->m_IsLuck )
	{
		Option2 = 0;
		// ----
		if( rand() % 2 == 0 )
		{
			Option2 = 1;
		}
	}
	// ----
	if( lpItem->m_IsOption )
	{
		if( rand() % 5 < 1 )
		{
			Option3 = 3;
		}
		else
		{
			Option3 = rand() % 3;
		}
	}
	// ----
	if( lpItem->m_MinExcCount )
	{
		int ExcOptionCount = lpItem->m_MinExcCount;
		// ----
		if( ExcOptionCount != lpItem->m_MaxExcCount )
		{
			ExcOptionCount += rand() % (1 + (lpItem->m_MaxExcCount - ExcOptionCount));
		}
		// ----
		while(ExcOptionCount != 0)
		{
			BYTE ExcValue = this->m_ExcellentPool.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
			// ----
			if( (ExcOption & ExcValue) == 0 )
			{
				ExcOption |= ExcValue;
				ExcOptionCount--;
			}
		}
	}
	// ----
	if( lpItem->m_IsSet )
	{
		SetOption = gSetItemOption.GenSetOption(ItemNumber);
	}
	// ----
	if( g_SocketItem.IsSocketItem(ItemNumber) )
	{
		if( lpItem->m_MinSocketCount < 1 )
		{
			lpItem->m_MinSocketCount = 1;
		}
		// ----
		if( lpItem->m_MaxSocketCount < lpItem->m_MinSocketCount )
		{
			lpItem->m_MaxSocketCount = lpItem->m_MinSocketCount;
		}
		// ----
		SocketCount = lpItem->m_MinSocketCount;
		// ----
		if( lpItem->m_MinSocketCount != lpItem->m_MaxSocketCount )
		{
			SocketCount += rand() % (1 + (lpItem->m_MaxSocketCount - lpItem->m_MinSocketCount));
		}
		// ----
		for( int i = 0; i < 5; i++ )
		{
			if( i < SocketCount )
			{
				SocketOption[i] = (BYTE)-2;
			}
			else
			{
				SocketOption[i] = (BYTE)-1;
			}
		}
	}
	// ----
	ItemSerialCreateSend(lpObj->m_Index, MapNumber, cX, cY, ItemNumber, Level, Dur, Option1, Option2, Option3, 
		lpObj->m_Index, ExcOption, SetOption, SocketBonus, SocketOption);
	LogAddTD("[%s][%s] [EventItemBag] (%s) Item drop (%d)(%d/%d) Item:(%s)%d Level:%d op1:%d op2:%d op3:%d ExOp:%d Anc:%d SocketCount:%d",
		lpObj->AccountID, lpObj->Name, this->GetBagName(), MapNumber, cX, cY, ItemAttribute[ItemNumber].Name, ItemNumber, Level, 
		Option1, Option2, Option3, ExcOption, SetOption, SocketCount);
	// ----
	return true;
}
// -------------------------------------------------------------------------------

EventItemBagManager::EventItemBagManager()
{
	this->Init();
}
// -------------------------------------------------------------------------------

EventItemBagManager::~EventItemBagManager()
{

}
// -------------------------------------------------------------------------------

void EventItemBagManager::Init()
{
	this->m_ItemBagList.clear();
	if( this->m_ItemBagList.capacity() > 0 )
	{
		std::vector<EventItemBag>().swap(this->m_ItemBagList);
	}
}
// -------------------------------------------------------------------------------

void EventItemBagManager::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_EVENTITEMBAG));
	// ----
	for( int i = 0; i < this->m_ItemBagList.size(); i++ )
	{
		this->m_ItemBagList[i].Read(this->m_ItemBagList[i].m_File);
	}
}
// -------------------------------------------------------------------------------

void EventItemBagManager::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[EventItemBag] File %s not found!", File);
		return;
	}
	// ----
	xml_node Main			= Document.child("eventitembag");
	xml_node ItemList		= Main.child("itemdrop");
	xml_node MonsterList	= Main.child("monsterdie");
	xml_node SpecialList	= Main.child("special");
	xml_node Node;
	EventItemBag ItemBag;
	// ----
	for( Node = ItemList.child("bag"); Node; Node = Node.next_sibling() )
	{
		ItemBag.Init();
		ItemBag.m_Mode = EVENTBAG_TYPE_ITEM;
		ItemBag.m_ItemType = Node.attribute("itemtype").as_int();
		ItemBag.m_ItemIndex = Node.attribute("itemindex").as_int();
		if( Node.attribute("itemlevel").as_int() == -1 )
		{
			ItemBag.m_ItemLevel = 255;
		}
		else
		{
			ItemBag.m_ItemLevel = Node.attribute("itemlevel").as_int();
		}
		ItemBag.m_MinUserLevel = Node.attribute("minlevel").as_int();
		ItemBag.m_Repeat = Node.attribute("repeat").as_int();
		ItemBag.m_Effect = Node.attribute("effect").as_int();
		sprintf(ItemBag.m_File, "%s%s", gDirPath.GetNewPath(PATH_EVENTITEMBAG), Node.text().as_string());
		this->m_ItemBagList.push_back(ItemBag);
	}
	// ----
	for( Node = MonsterList.child("bag"); Node; Node = Node.next_sibling() )
	{
		ItemBag.Init();
		ItemBag.m_Mode = EVENTBAG_TYPE_MONSTER;
		ItemBag.m_MonsterID = Node.attribute("monster").as_int();
		ItemBag.m_Repeat = Node.attribute("repeat").as_int();
		ItemBag.m_SetItemRate = Node.attribute("setrate").as_int();
		ItemBag.m_Notify = Node.attribute("notify").as_int();
		sprintf(ItemBag.m_File, "%s%s", gDirPath.GetNewPath(PATH_EVENTITEMBAG), Node.text().as_string());
		this->m_ItemBagList.push_back(ItemBag);
	}
	// ----
	for( Node = SpecialList.child("bag"); Node; Node = Node.next_sibling() )
	{
		ItemBag.Init();
		ItemBag.m_Mode = EVENTBAG_TYPE_SPECIAL;
		ItemBag.m_SpecialType = Node.attribute("type").as_int();
		sprintf(ItemBag.m_File, "%s%s", gDirPath.GetNewPath(PATH_EVENTITEMBAG), Node.text().as_string());
		this->m_ItemBagList.push_back(ItemBag);
	}
}
// -------------------------------------------------------------------------------

BYTE EventItemBagManager::OpenBox(int UserIndex, int ItemNumber, BYTE ItemLevel)
{
	for( int i = 0; i < this->m_ItemBagList.size(); i++ )
	{
		if( this->m_ItemBagList[i].m_Mode == EVENTBAG_TYPE_ITEM )
		{
			if( ITEMGET(this->m_ItemBagList[i].m_ItemType, this->m_ItemBagList[i].m_ItemIndex) == ItemNumber )
			{
				if(		this->m_ItemBagList[i].m_ItemLevel == 255
					||	this->m_ItemBagList[i].m_ItemLevel == ItemLevel )
				{
					if( gObj[UserIndex].Level < this->m_ItemBagList[i].m_MinUserLevel )
					{
						return 0;
					}
					// ----
					int OpenedCount = 0;
					for( int n = 0; n < this->m_ItemBagList[i].m_Repeat; n++ )
					{
						BYTE X = gObj[UserIndex].X;
						BYTE Y = gObj[UserIndex].Y;
						// ----
						if( !gObjGetRandomItemDropLocation(gObj[UserIndex].MapNumber, X, Y, 3, 3, 10) )
						{
							X = gObj[UserIndex].X;
							Y = gObj[UserIndex].Y;
						}
						// ----
						if( this->m_ItemBagList[i].Open(UserIndex, gObj[UserIndex].MapNumber, X, Y) )
						{
							OpenedCount++;
							if( this->m_ItemBagList[i].m_Effect == EVENTBAG_EFFECT_FIREWORK )
							{
								PMSG_SERVERCMD ServerCmd = { 0 };
								PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
								ServerCmd.CmdType = 0;
								ServerCmd.X = gObj[UserIndex].X;
								ServerCmd.Y = gObj[UserIndex].Y;
								MsgSendV2(&gObj[UserIndex], (LPBYTE)&ServerCmd, sizeof(ServerCmd));
								DataSend(UserIndex, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
							}
							else if( this->m_ItemBagList[i].m_Effect == EVENTBAG_EFFECT_CHERRY )
							{
								PMSG_SERVERCMD ServerCmd = { 0 };
								PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
								ServerCmd.CmdType = 58;
								ServerCmd.X = gObj[UserIndex].X;
								ServerCmd.Y = gObj[UserIndex].Y;
								MsgSendV2(&gObj[UserIndex], (LPBYTE)&ServerCmd, sizeof(ServerCmd));
								DataSend(UserIndex, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
							}
						}
					}

					if( OpenedCount == this->m_ItemBagList[i].m_Repeat )
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}
			}
		}
	}
	// ----
	return 2;
}
// -------------------------------------------------------------------------------

BYTE EventItemBagManager::OpenMonster(int MonsterIndex)
{
	bool Success = false;
	int UserIndex = gObjMonsterTopHitDamageUser(&gObj[MonsterIndex]);
	// ----
	if( UserIndex == -1 )
	{
		return Success;
	}
	// ----
	for( int i = 0; i < this->m_ItemBagList.size(); i++ )
	{
		if( this->m_ItemBagList[i].m_Mode != EVENTBAG_TYPE_MONSTER )
		{
			continue;
		}
		// ----
		if( this->m_ItemBagList[i].m_MonsterID == gObj[MonsterIndex].Class )
		{
			for( int n = 0; n < this->m_ItemBagList[i].m_Repeat; n++ )
			{
				BYTE X = gObj[MonsterIndex].X;
				BYTE Y = gObj[MonsterIndex].Y;
				// ----
				if( !gObjGetRandomItemDropLocation(gObj[UserIndex].MapNumber, X, Y, 3, 3, 10) )
				{
					X = gObj[MonsterIndex].X;
					Y = gObj[MonsterIndex].Y;
				}
				// ----
				if( rand() % EVENTBAG_MAX_RATE < this->m_ItemBagList[i].m_SetItemRate )
				{
					MakeRewardSetItem(UserIndex, X, Y, 1, gObj[MonsterIndex].MapNumber);
					LogAddTD("[%s][%s] [EventItemBag] (%s) Set item drop",
						gObj[UserIndex].AccountID, gObj[UserIndex].Name, this->m_ItemBagList[i].GetBagName());
					continue;
				}
				// ----
				this->m_ItemBagList[i].Open(UserIndex, gObj[UserIndex].MapNumber, X, Y);
			}
			// ----
			Success = true;
			if( this->m_ItemBagList[i].m_Notify > 0 )
			{
				PMSG_NOTICE pNotice;
				char Text[256];
				ZeroMemory(Text, sizeof(Text));
				sprintf(Text, "%s has been killed by %s", gObj[MonsterIndex].Name, gObj[UserIndex].Name);
				TNotice::MakeNoticeMsg((TNotice*)&pNotice, 0x00, Text);
				// ----
				for( int Object = OBJ_STARTUSERINDEX ; Object < OBJMAX ; Object++)
				{
					if( gObj[Object].Connected == PLAYER_PLAYING )
					{
						if( gObj[Object].Type == OBJ_USER )
						{
							if( this->m_ItemBagList[i].m_Notify == 1 )
							{
								if( gObj[Object].MapNumber == gObj[MonsterIndex].MapNumber )
								{
									DataSend(Object, (LPBYTE)&pNotice, pNotice.h.size);
								}
							}
							else if( this->m_ItemBagList[i].m_Notify == 2 )
							{
								DataSend(Object, (LPBYTE)&pNotice, pNotice.h.size);
							}
						}
					}
				}
			}
			break;
		}
	}
	// ----
	return Success;
}
// -------------------------------------------------------------------------------

BYTE EventItemBagManager::OpenSpecial(BYTE SpecialType, int UserIndex, int MapNumber, BYTE X, BYTE Y)
{
	for( int i = 0; i < this->m_ItemBagList.size(); i++ )
	{
		if( this->m_ItemBagList[i].m_Mode != EVENTBAG_TYPE_SPECIAL )
		{
			continue;
		}
		// ----
		if( this->m_ItemBagList[i].m_SpecialType == SpecialType )
		{
			this->m_ItemBagList[i].Open(UserIndex, MapNumber, X, Y);
			return 1;
		}
	}
	// ----
	return 0;
}
// -------------------------------------------------------------------------------