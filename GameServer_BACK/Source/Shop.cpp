#include "stdafx.h"
#include "GameServer.h"
#include "Shop.h"
#include "LogProc.h"
#include "..\include\readscript.h"
#include "GameMain.h"
#include "..\common\WzMemScript.h"
#include "ElementalSystem.h"
#include "..\pugixml\pugixml.hpp"
//#include "License.h"

using namespace pugi;

//CShop ShopC[MAX_SHOP];

CShop::CShop()
{
	return;
}


CShop::~CShop()
{
	return;
}

void CShop::Init()
{
	this->SendItemDataLen = 0;
	this->ItemCount = 0;
	memset(ShopInventoryMap, 0, sizeof(ShopInventoryMap));
	memset(m_CreditPriceTable, 0, sizeof(m_CreditPriceTable));
}

BOOL CShop::InsertItem(int type, int index, int level, int dur, int op1,  int op2, int op3, int exc, int anc, int socket, int element, int creditprice)
{
	int itemp;
	int width;
	int height;
	int x;
	int y;
	int blank;

	blank = -1;
	itemp = type * MAX_SUBTYPE_ITEMS + index;
	
	if ( itemp < 0 )
	{
		MsgBox(lMsg.Get(MSGGET(2, 53)), __FILE__, __LINE__, type, index);
		return false;
	}
	
	ItemGetSize(itemp, width, height);

	if ( width < 0 || height < 0 )
	{
		// Error in getting item size in shop %s %d
		MsgBox(lMsg.Get(MSGGET(2, 54)), __FILE__, __LINE__);
		return FALSE;
	}

	for ( y=0;y<15;y++)
	{
		for ( x=0;x<8;x++)
		{
			if ( this->ShopInventoryMap[x + y*8] == 0 )
			{
				blank = this->InentoryMapCheck(x, y, width, height);

				if ( blank >= 0 )
				{
					goto skiploop;
				}
			}
		}
	}

	if ( blank < 0 )
	{
		MsgBox("error-L2 : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

skiploop:
	
	m_CreditPriceTable[blank] = creditprice;
	this->m_item[blank].m_Level = level;

	if ( dur == 0 )
	{
		dur = ItemGetDurability(ITEMGET(type, index), level, exc, anc);
	}

	this->m_item[blank].m_Durability = (float)dur;
	this->m_item[blank].Convert(itemp, op1, op2, op3, exc, anc, 0,0,-1, CURRENT_DB_VERSION);
	this->m_item[blank].Value();

	if( g_SocketItem.IsSocketItem(itemp) == TRUE && socket > 0 )
	{
		this->m_item[blank].InitSocketItem(socket);
	}

	if( g_ElementalSystem.IsElementalItem(itemp) && element != -1 )
	{
		if( g_ElementalSystem.IsPentagram(itemp) )
		{
			for(int i = 0; i < 5; i++)
			{
				this->m_item[blank].m_SocketOption[i] = (BYTE)-2;
			}
		}
		this->m_item[blank].m_SocketBonus = element;
	}

	this->SendItemData[this->SendItemDataLen] = blank;
	this->SendItemDataLen++;
	ItemByteConvert((LPBYTE)&this->SendItemData[this->SendItemDataLen], this->m_item[blank]);
	this->SendItemDataLen += MAX_ITEM_INFO;
	this->ItemCount++;

	return TRUE;
}

int CShop::InentoryMapCheck(int sx, int sy, int width, int height)
{
	int x;
	int y;
	int blank = 0;

	if ( (sx+width) > 8 )
		return -1;

	if ( (sy+height) > 15 )
		return -1;

	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			if ( this->ShopInventoryMap[( sy + y) * 8 + (sx + x)] )
			{
				blank++;
				break;
			}
		}
	}

	if ( blank == 0 )
	{
		for(y=0;y<height;y++)
		{
			for(x=0;x<width;x++)
			{
				this->ShopInventoryMap[( sy + y) * 8 + (sx + x)] = 1;
			}
		}

		return (sx + sy*8);
	}

	return -1;
}

BOOL CShop::LoadShopItem(char* filename )
{
	this->Init();
	xml_document doc;
	xml_parse_result result = doc.load_file(filename);

	if( result.status != status_ok )
	{
		MsgBox("Shop data load error %s\nError description: %s\n", filename,result.description());
		return FALSE;
	}

	xml_node shop = doc.child("shop");

	m_CreditPrice = shop.attribute("credits").as_bool(false);
	
	for (xml_node item = shop.child("item"); item; item = item.next_sibling())	{
		int type;
		int index;
		int dur;
		int level;
		int op1;
		int op2;
		int op3;
		int exc;
		int anc;
		int socket;
		int element;

		type = item.attribute("id").as_int();
		index = item.attribute("num").as_int();
		dur = item.attribute("dur").as_int();
		level = item.attribute("level").as_int();
		op1 = item.attribute("skill").as_int();
		op2 = item.attribute("luck").as_int();
		op3 = item.attribute("option").as_int();
		exc = item.attribute("exc").as_int();
		anc = item.attribute("anc").as_int();
		socket = item.attribute("socket").as_int();
		element = item.attribute("element").as_int(-1);
		int creditprice = item.attribute("price").as_int(0);

		if (this->InsertItem(type, index, level, dur, op1, op2, op3, exc, anc, socket, element, creditprice) == FALSE )
		{
			MsgBox("error-L3 : %s %s %d", filename, __FILE__, __LINE__);
		}
    }

	return TRUE;
}


/*BOOL ShopDataLoad()
{
	char szShop[128];
	int i = 0;

	for( ; i < (MAX_SHOP-1); i++)	
	{
		sprintf(szShop,"Shop\\Shop%d.xml",i);
		char* szShopPath = gDirPath.GetNewPath(szShop);

		if( _access(szShopPath,4) != -1 )
			ShopC[i].LoadShopItem(szShopPath);
	}

#ifdef __NOVUS__
	if( _access(gDirPath.GetNewPath("Shop\\ExPoint\\PvP.xml"), 4) != -1 )
	{
			ShopC[48].LoadShopItem(gDirPath.GetNewPath("Shop\\ExPoint\\PvP.xml"));
	}
	// ----
	if( _access(gDirPath.GetNewPath("Shop\\ExPoint\\Event.xml"), 4) != -1 )
	{
			ShopC[49].LoadShopItem(gDirPath.GetNewPath("Shop\\ExPoint\\Event.xml"));
	}
	// ----
	if( _access(gDirPath.GetNewPath("Shop\\ExPoint\\Gens.xml"), 4) != -1 )
	{
			ShopC[50].LoadShopItem(gDirPath.GetNewPath("Shop\\ExPoint\\Gens.xml"));
	}
#endif

	LogAdd(lMsg.Get(MSGGET(1, 209)));
	return TRUE;
}*/

ShopManager g_ShopManager;

ShopManager::ShopManager()
{

}

ShopManager::~ShopManager()
{

}

void ShopManager::Init()
{
	this->m_ShopList.clear();
	if( this->m_ShopList.capacity() > 0 )
	{
		std::vector<CShop>().swap(this->m_ShopList);
	}
}

void ShopManager::Read(int ShopNumber)
{
	if( ShopNumber < 0 )
	{
		MsgBox("[ShopManager] File %d not found!", ShopNumber);
		return;
	}

	for( int i = 0; i < this->m_ShopList.size(); i++ )
	{
		if( this->m_ShopList[i].m_ShopNumber == ShopNumber )
		{
			return;
		}
	}

	CShop lpShop;
	lpShop.Init();
	lpShop.m_ShopNumber = ShopNumber;

	char FilePath[MAX_PATH] = { 0 };
	sprintf(FilePath, "Shop\\Shop%d.xml", ShopNumber);
	if( _access(gDirPath.GetNewPath(FilePath), 4) != -1 )
	{
		if( lpShop.LoadShopItem(gDirPath.GetNewPath(FilePath)) )
		{
			lpShop.m_Loaded = true;
		}
	}

	this->m_ShopList.push_back(lpShop);
}

void ShopManager::Reload()
{
	for( int i = 0; i < this->m_ShopList.size(); i++ )
	{
		this->m_ShopList[i].Init();
		this->m_ShopList[i].m_ShopNumber = i;
		char FilePath[MAX_PATH] = { 0 };
		sprintf(FilePath, "Shop\\Shop%d.xml", i);
		if( _access(gDirPath.GetNewPath(FilePath), 4) != -1 )
		{
			if( this->m_ShopList[i].LoadShopItem(gDirPath.GetNewPath(FilePath)) )
			{
				this->m_ShopList[i].m_Loaded = true;
			}
		}
	}
}

CShop* ShopManager::Get(int ShopNumber)
{
	for( int i = 0; i < this->m_ShopList.size(); i++ )
	{
		if( this->m_ShopList[i].m_ShopNumber == ShopNumber
			&& this->m_ShopList[i].m_Loaded == true )
		{
			return &this->m_ShopList[i];
		}
	}
	return NULL;
}