#include "stdafx.h"
#include "zMultiServer.h"
#include "DataServer.h"
#include "winutil.h"
#include "CashShopServer.h"
#include "Window.h"
#include "ReadScript.h"

CS_INFO g_CSInfo;
CQuery g_CashShopDB;

void CashShopServerInit()
{
	g_bIsServer[ST_CASHSHOPSERVER] = TRUE;

	g_CSInfo.TCPPort = GetPrivateProfileInt(CS_INISECTION,"TCPPort",55821,INI_FILE);
	GetPrivateProfileString(CS_INISECTION,"DSN","MU2003_EVENT_DB",g_CSInfo.dbDSN,sizeof(g_CSInfo.dbDSN),INI_FILE);

	g_CashShopDB.S_TYPE = ST_CASHSHOPSERVER;

	//LoadCashItemList();
	LoadCashShopPackage();
	LoadCashShopProduct();

	if( g_CashShopDB.Connect(3,g_CSInfo.dbDSN,SQLUser,SQLPass) == TRUE )
	{
		g_Window.LogAdd(Log::Info, "[ %s ] connection successful !",g_CSInfo.dbDSN);
	}

	if(CreateGIocp(g_CSInfo.TCPPort,ST_CASHSHOPSERVER) != TRUE)
	{
		g_Window.LogAdd(Log::Error, "Failed to start [ ST_CASHSHOPSERVER ] on port [ %d ]",g_CSInfo.TCPPort);
	}
	else
	{
		g_Window.LogAdd(Log::Info, "[ CashShopServer ] created on port [ %d ]",g_CSInfo.TCPPort);
	}
}


typedef std::map<int,sellItemPrice *> tagPricePointer;
typedef std::map<int,sellItem *> tagSellItemPointer;

sellItem g_SellItemList[MAX_SELL_ITEMS];
tagPricePointer g_SellPriceMap;
tagSellItemPointer g_SellItemMap;

void CSProtocolCore(int aIndex, DWORD HeadCode, LPBYTE aRecv, int iSize)
{	
//	g_ServerInfoDisplayer.SetState(TRUE,gSObj[aIndex].SType);

	protocol::HDRMessageID* hdrid = (protocol::HDRMessageID*)&HeadCode;

	if( hdrid->messageID == 0 )
	{
		switch ( hdrid->category )
		{
		case 0x100:
			SGAnsCashPoint(aIndex,(protocol::MSG_GTOS_USER_CASH_REQ *)aRecv);
			break;
		case 0x300:
			SGAnsCashItemList(aIndex);
			break;
		case 0x900:
			SGAnsBuyCashItem(aIndex,(protocol::MSG_GTOS_BUY_ITEM_REQ *)aRecv);
			break;
		}
	}
	else if( hdrid->messageID == 1 )
	{
		switch ( hdrid->category )
		{
		case 0x100:
			SGAnsCashPointEx(aIndex,(protocol::MSG_GTOS_USER_CASH_REQ_EX *)aRecv);
			break;
		case 0x300:
			SGAndCashItemListEx(aIndex,(protocol::MSG_GTOS_ITEM_LIST_REQ_EX *)aRecv);
			break;
		case 0x500:
			SGAnsCashItemUseEx(aIndex,(protocol::MSG_GTOS_ITEM_USE_REQ_EX *)aRecv);
			break;
		case 0x700:
			SGAndCashGiftListEx(aIndex,(protocol::MSG_GTOS_GIFT_LIST_REQ_EX *)aRecv);
			break;
		case 0x900:
			SGAnsBuyCashItemEx(aIndex,(protocol::MSG_GTOS_BUY_ITEM_REQ_EX *)aRecv);
			break;
		case 0xB00:
			SGAnsUseCashItemNotifyEx(aIndex,(protocol::MSG_GTOS_USE_ITEM_NTF_EX *)aRecv);
			break;
		case 0xD00:
			SGAnsGiftCashItemEx(aIndex,(protocol::MSG_GTOS_GIFT_ITEM_REQ_EX *)aRecv);
			break;
		case 0xF00:
			SGUpdatePointInfo(aIndex,(protocol::MSG_GTOS_UPDATE_POINT_EX *)aRecv);
			break;
		}
	}
//	g_ServerInfoDisplayer.SetState(FALSE,gSObj[aIndex].SType);
}

BOOL LoadCashItemList()
{
	DWORD TickStart = GetTickCount();
	memset(g_SellItemList,0x00,sizeof(g_SellItemList));

	g_SellPriceMap.clear();
	g_SellItemMap.clear();

	SMDFile = fopen("CashItemList.txt","r");

	if(SMDFile == FALSE)
	{
		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop] CashItemList.txt not found");
		return FALSE;
	}

	int Token,ItemGuid = 0,PriceGuid = 1;
	sellItem item;

	while( true )
	{
		Token = GetToken();

		if(Token == END)
		{
			break;
		}

		if(ItemGuid >= MAX_SELL_ITEMS)
		{

				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop] LoadItemList() -> ItemGuid > MAX_SELL_ITEMS");
			fclose(SMDFile);
			return FALSE;
		}

		item.dwItemGuid = ItemGuid+1;

		int iGroup = (BYTE)TokenNumber;
		Token = GetToken();
		int iIndex = (WORD)TokenNumber;
		item.dwItemCODE = iGroup * 512 + iIndex;

		Token = GetToken();
		item.dwCategoryID = (DWORD)TokenNumber;

		Token = GetToken();
		item.dwUseType = (DWORD)TokenNumber;

		Token = GetToken();
		//strncpy(item.szItemName,TokenString,sizeof(item.szItemName));

		Token = GetToken();
		//strncpy(item.szItemDesc,TokenString,sizeof(item.szItemDesc));

		int n = 0;

		while( true )
		{
			Token = GetToken();

			if(!strcmp(TokenString,"end"))
			{
				break;
			}

			if(n >= MAX_ITEM_PRICE)
			{

				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop] LoadItemList() -> PriceGuid > MAX_ITEM_PRICE");
				fclose(SMDFile);
				return FALSE;
			}

			item.itemPrice[n].dwItemGuid = ItemGuid+1;
			item.itemPrice[n].dwPriceGuid = PriceGuid;
			item.itemPrice[n].dwUseTime = (DWORD)TokenNumber;

			Token = GetToken();
			item.itemPrice[n].dwAmount = (DWORD)TokenNumber;

			Token = GetToken();
			item.itemPrice[n].dwPrice = (DWORD)TokenNumber;

			Token = GetToken();
			item.itemPrice[n].dwSellRate = (DWORD)TokenNumber;

			g_SellPriceMap[PriceGuid] = &g_SellItemList[ItemGuid].itemPrice[n];

			n++;
			PriceGuid++;
		}
		item.dwPriceCount = n;
		g_SellItemList[ItemGuid] = item;
		g_SellItemMap[ItemGuid] = &g_SellItemList[ItemGuid];
		ItemGuid++;
	}
	fclose(SMDFile);

	g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[Cash Shop] CashItemList.txt loaded in (%d) msec !", GetTickCount() - TickStart);
	return TRUE;
}

void SGAnsCashPoint(int aIndex,protocol::MSG_GTOS_USER_CASH_REQ * aRecv)
{
	char szQuery[256];
	protocol::MSG_STOG_USER_CASH_ANS pMsg;

	pMsg.dwUserGuid = aRecv->dwUserGuid;
	pMsg.dwUserCash = 0;

	sprintf(szQuery,"SELECT CashPoint FROM MuCashShop_DATA WHERE memb_guid = %d",aRecv->dwUserGuid);

	if(g_CashShopDB.Exec(szQuery) == TRUE)
	{
		if( g_CashShopDB.Fetch() != SQL_NO_DATA )
		{
			pMsg.dwUserCash = g_CashShopDB.GetInt("CashPoint");
		}
		else
		{
			g_CashShopDB.Clear();
			sprintf(szQuery,"INSERT INTO MuCashShop_DATA (memb_guid,CashPoint) VALUES (%d,0)",aRecv->dwUserGuid);
			g_CashShopDB.Exec(szQuery);
		}
	}

	g_CashShopDB.Clear();
	DataSend(aIndex,(PBYTE)&pMsg,pMsg.size);
}

void SGAnsCashItemList(int aIndex)
{
	protocol::MSG_STOG_ITEM_LIST_ANS pMsg;

	int count = 0;

	for(int i = 0; i < MAX_SELL_ITEMS; i++)
	{
		if(g_SellItemList[i].dwItemGuid != 0)
		{
			memcpy(&pMsg.sellItems[count],&g_SellItemList[i],sizeof(g_SellItemList[i]));
			count++;
		}
	}

	pMsg.dwItemCount = count;
	pMsg.size = sizeof(protocol::MSG_HEADER) + sizeof(sellItem) * pMsg.dwItemCount;
	DataSend(aIndex,(PBYTE)&pMsg,pMsg.size);
}

void SGAnsBuyCashItem(int aIndex,protocol::MSG_GTOS_BUY_ITEM_REQ * aRecv)
{
	protocol::MSG_STOG_BUY_ITEM_ANS pMsg;
	tagPricePointer::iterator it;
	tagSellItemPointer::iterator _Item;

	pMsg.dwServerGuid = aRecv->dwServerGuid;
	pMsg.dwUserGuid = aRecv->dwUserGuid;
	pMsg.dwCharGuid = aRecv->dwCharGuid;

	pMsg.dwCount = 0;
	pMsg.dwResult = 10;

	int CashPoint = 0;

	char szQuery[256];
	sprintf(szQuery,"SELECT CashPoint FROM MuCashShop_DATA WHERE memb_guid = %d",aRecv->dwUserGuid);

	if(g_CashShopDB.Exec(szQuery) == TRUE && g_CashShopDB.Fetch() != SQL_NO_DATA)
	{
		CashPoint = g_CashShopDB.GetInt("CashPoint");
		g_CashShopDB.Clear();

		pMsg.dwUserCash = CashPoint;
		pMsg.dwResult = 1;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[BUY ANSWER] [%s] Start transaction POINTS: %d",aRecv->szCharName,CashPoint);

		for(int i = 0; i < aRecv->dwCount; i++)
		{
			int Guid = aRecv->dwPriceGuids[i];

			pMsg.transactions[i].dwTransactionGuid = i+1;
			pMsg.transactions[i].dwPriceGuid = Guid;

			it = g_SellPriceMap.find(Guid);

			if( it == g_SellPriceMap.end() )
			{
				pMsg.transactions[i].dwResult = 0;
				pMsg.dwResult = 0;

				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[BUY ANSWER] [%s] No Price found",aRecv->szCharName);
				break;
			}

			sellItemPrice * iPrice = it->second;
			_Item = g_SellItemMap.find((iPrice->dwItemGuid-1));

			if( _Item == g_SellItemMap.end() )
			{
				pMsg.transactions[i].dwResult = 0;
				pMsg.dwResult = 0;

				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[BUY ANSWER] [%s] No Item found",aRecv->szCharName);
				break;
			}

			sellItem * iItem = _Item->second;

			sprintf(szQuery,"SELECT memb__char FROM T_PeriodItem_Data WHERE memb__char = '%s' AND ItemCode = %d",
				aRecv->szCharName,iItem->dwItemCODE);

			if( g_CashShopDB.Exec(szQuery) == FALSE )
			{
				pMsg.dwUserCash = CashPoint;
				pMsg.transactions[i].dwResult = 0;
				pMsg.dwResult = 2;

				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[BUY ANSWER] [%s] Sql check error",aRecv->szCharName);
				break;
			}

			if( g_CashShopDB.Fetch() != SQL_NO_DATA )
			{
				pMsg.dwUserCash = CashPoint;
				pMsg.transactions[i].dwResult = 0;
				pMsg.dwResult = 2;

				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[BUY ANSWER] [%s] User already have item [%d]",aRecv->szCharName,iItem->dwItemCODE);
				break;
			}

			g_CashShopDB.Clear();

			if(CashPoint < iPrice->dwPrice)
			{
				pMsg.dwUserCash = CashPoint;
				pMsg.transactions[i].dwResult = 0;
				pMsg.dwResult = 2;

				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[BUY ANSWER] [%s] User Lack Points ITEM:(%d %d) PRICE: %d",
					aRecv->szCharName,iItem->dwItemCODE/512,iItem->dwItemCODE%512,iPrice->dwPrice);
				break;
			}
			else
			{
				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[BUY ANSWER] [%s] User Buy Request ITEM:(%d %d),TIME: %d,DUR: %d PRICE: %d",
					aRecv->szCharName,iItem->dwItemCODE/512,iItem->dwItemCODE%512,iPrice->dwUseTime,iPrice->dwAmount,iPrice->dwPrice);
			}

			CashPoint -= iPrice->dwPrice;
			pMsg.transactions[i].dwResult = 1;
			pMsg.dwCount++;
		}

		if(pMsg.dwResult == 1)
		{
			sprintf(szQuery,"UPDATE MuCashShop_DATA SET CashPoint = %d WHERE memb_guid = %d",CashPoint,pMsg.dwUserGuid);
			if( g_CashShopDB.Exec(szQuery) == TRUE )
			{
				pMsg.dwUserCash = CashPoint;
				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[Buy Answer] Buy success [%s][%d] points",aRecv->szCharName,pMsg.dwUserCash);
			}
		}
	}

	g_CashShopDB.Clear();

	DataSend(aIndex,(PBYTE)&pMsg,pMsg.size);
}


// NEW CASH SHOP PROTOCOL

typedef std::multimap<int,PACKAGE_INFO>	shopPackage;
typedef std::multimap<int,PRODUCT_INFO> shopProduct;

shopPackage	shopPackageList;
shopProduct shopProductList;

void LoadCashShopPackage()
{
	shopPackageList.clear();

	SMDFile = fopen("CashShopPackage.txt","r");

	if( SMDFile == NULL )
	{
		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop] CashShopPackage.txt not found");

		return;
	}

	int Token,category,itemIndex,itemNumber;

	PACKAGE_INFO pTmp;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		for(int i = 0; i < 8; i++)
		{
			pTmp.wProductGroup[i] = -1;
		}

		category = TokenNumber;

		Token = GetToken();
		pTmp.wGuid = TokenNumber;

		Token = GetToken();
		itemIndex = TokenNumber;

		Token = GetToken();
		itemNumber = TokenNumber;

		pTmp.aItemId = itemIndex*512 + itemNumber;

		Token = GetToken();
		pTmp.btPriceType = TokenNumber;

		Token = GetToken();
		pTmp.Price = TokenNumber;

		Token = GetToken();
		pTmp.btUseType = TokenNumber;

		Token = GetToken();
		pTmp.bGift = TokenNumber;

		while( TRUE )
		{
			Token = GetToken();

			if( !strcmp("end",TokenString) )
				break;

			int count = TokenNumber;

			if( count <= 0 || count > 8 )
			{
				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop] Failed loading cash packages! ProductCount out of bounds: %d",count);
				return;
			}

			pTmp.btProductCount = count;
			
			for(int n = 0; n < count; n++)
			{
				Token = GetToken();
				pTmp.wProductGroup[n] = TokenNumber;
			}
		}

		shopPackageList.insert(std::pair<int,PACKAGE_INFO>(category,pTmp));
	}

	fclose(SMDFile);

	g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop] Loaded %d packages!",shopPackageList.size());
}

void LoadCashShopProduct()
{
	shopProductList.clear();

	SMDFile = fopen("CashShopProduct.txt","r");

	if( SMDFile == NULL )
	{

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop] CashShopProduct.txt not found");
		return;
	}

	int Token,group,itemIndex,itemNumber;

	PRODUCT_INFO pTmp;

	while( TRUE )
	{
		Token = GetToken();
		
		if( Token == END )
			break;

		group = TokenNumber;

		Token = GetToken();
		pTmp.wProductGuid = TokenNumber;

		Token = GetToken();
		itemIndex = TokenNumber;

		Token = GetToken();
		itemNumber = TokenNumber;

		pTmp.aItemId = itemIndex * 512 + itemNumber;

		Token = GetToken();
		pTmp.Price = TokenNumber;

		Token = GetToken();
		pTmp.btDur = TokenNumber;

		Token = GetToken();
		pTmp.Duration = TokenNumber;

		Token = GetToken();
		pTmp.btItemType = TokenNumber;

#if (EX700==1)
		Token = GetToken();
		pTmp.btLevel = TokenNumber;

		Token = GetToken();
		pTmp.btOption1 = TokenNumber;

		Token = GetToken();
		pTmp.btOption2 = TokenNumber;

		Token = GetToken();
		pTmp.btOption3 = TokenNumber;

		Token = GetToken();
		pTmp.btExcOp = TokenNumber;

		Token = GetToken();
		pTmp.btAncOp = TokenNumber;
#endif

		shopProductList.insert(std::pair<int,PRODUCT_INFO>(group,pTmp));
	}

	fclose(SMDFile);

	g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop] Loaded %d products!",shopProductList.size());
}

void SGAnsCashPointEx(int aIndex,protocol::MSG_GTOS_USER_CASH_REQ_EX * aRecv)
{
	char szQuery[256];
	protocol::MSG_STOG_USER_CASH_ANS_EX pMsg;

	pMsg.dwUserGuid = aRecv->dwUserGuid;
	pMsg.dwUserCash[0] = 0;
	pMsg.dwUserCash[1] = 0;
	pMsg.dwUserCash[2] = 0;

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT WCoinC,WCoinP,WCoinG FROM GameShop_Data WHERE MemberGuid = %d",aRecv->dwUserGuid);
#else
	sprintf(szQuery,"SELECT wcoin_c,wcoin_p,wcoin_g FROM T_CashShop_DATA WHERE memb_guid = %d",aRecv->dwUserGuid);
#endif

	if(g_CashShopDB.Exec(szQuery) == TRUE)
	{
		if( g_CashShopDB.Fetch() != SQL_NO_DATA )
		{
#ifdef ZTEAMDB
			pMsg.dwUserCash[0] = g_CashShopDB.GetInt("WCoinC");
			pMsg.dwUserCash[1] = g_CashShopDB.GetInt("WCoinP");
			pMsg.dwUserCash[2] = g_CashShopDB.GetInt("WCoinG");
#else
			pMsg.dwUserCash[0] = g_CashShopDB.GetInt("wcoin_c");
			pMsg.dwUserCash[1] = g_CashShopDB.GetInt("wcoin_p");
			pMsg.dwUserCash[2] = g_CashShopDB.GetInt("wcoin_g");
#endif
		}
		else
		{
			g_CashShopDB.Clear();
		
#ifdef ZTEAMDB
			sprintf(szQuery,"INSERT INTO GameShop_Data (MemberGuid,WCoinC,WCoinP,WCoinG) VALUES (%d,0,0,0)",aRecv->dwUserGuid);
#else
			sprintf(szQuery,"INSERT INTO T_CashShop_DATA (memb_guid,wcoin_c,wcoin_p,wcoin_g) VALUES (%d,0,0,0)",aRecv->dwUserGuid);
#endif
			g_CashShopDB.Exec(szQuery);
		}
	}

	g_CashShopDB.Clear();
	DataSend(aIndex,(PBYTE)&pMsg,pMsg.size);
}

void SGAnsBuyCashItemEx(int aIndex,protocol::MSG_GTOS_BUY_ITEM_REQ_EX *lpMsg)
{
	char szQuery[512];
	protocol::MSG_STOG_BUY_ITEM_ANS_EX pMsg;

	pMsg.dwResult = 0;
	DWORD dwUserGuid = lpMsg->dwUserGuid;

	pMsg.dwServerGuid = lpMsg->dwServerGuid;
	pMsg.dwUserGuid = dwUserGuid;
	pMsg.dwCharGuid = lpMsg->dwCharGuid;

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT WCoinC,WCoinP,WCoinG FROM GameShop_Data WHERE MemberGuid = %d",dwUserGuid);
#else
	sprintf(szQuery,"SELECT wcoin_c,wcoin_p,wcoin_g FROM T_CashShop_DATA WHERE memb_guid = %d",dwUserGuid);
#endif

	if( !g_CashShopDB.Exec(szQuery) || g_CashShopDB.Fetch() == SQL_NO_DATA )
	{
		g_CashShopDB.Clear();
		pMsg.dwResult = -1;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - User not found in DB",
			lpMsg->dwUserGuid,lpMsg->szCharName);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

#ifdef ZTEAMDB
	pMsg.dwUserCash[0] = g_CashShopDB.GetInt("WCoinC");
	pMsg.dwUserCash[1] = g_CashShopDB.GetInt("WCoinP");
	pMsg.dwUserCash[2] = g_CashShopDB.GetInt("WCoinG");
#else
	pMsg.dwUserCash[0] = g_CashShopDB.GetInt("wcoin_c");
	pMsg.dwUserCash[1] = g_CashShopDB.GetInt("wcoin_p");
	pMsg.dwUserCash[2] = g_CashShopDB.GetInt("wcoin_g");
#endif

	g_CashShopDB.Clear();

	PACKAGE_INFO* lpPackage = SearchShopPackage(lpMsg->dwPackageCategory,lpMsg->dwPackageId);
	PRODUCT_INFO* lpProduct = NULL;

	if( lpPackage == NULL )
	{
		pMsg.dwResult = 4;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - Package not found! PackageInfo(Category: %d, Guid: %d, ItemId: [%d %d])",
			lpMsg->dwUserGuid,lpMsg->szCharName,lpMsg->dwPackageCategory,lpMsg->dwPackageId,lpMsg->ItemID/512,lpMsg->ItemID%512);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

	BYTE btPriceType = lpPackage->btPriceType;

	if( btPriceType < 0 || btPriceType >= 3 )
	{
		pMsg.dwResult = -1;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - Price type out of bounds: (pType: %d)",
			dwUserGuid,lpMsg->szCharName,lpPackage->btPriceType);
		return;
	}

	if( lpPackage->btUseType == 0 )
	{
		if( lpPackage->Price > pMsg.dwUserCash[btPriceType] )
		{
			pMsg.dwResult = 1;

			g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - No enough coins! PackagePrice:[%d], UserCoins:[%d]",
				lpMsg->dwUserGuid,lpMsg->szCharName,lpPackage->Price,pMsg.dwUserCash[btPriceType]);

			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
			return;
		}
	}
	else
	{
		lpProduct = SearchShopProduct(lpPackage->wProductGroup[0],lpMsg->dwProductId);

		if( lpProduct == NULL )
		{
			pMsg.dwResult = 4;

			g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - Product not found! ProductInfo(Group: %d, Guid: %d)",
				lpMsg->dwUserGuid,lpMsg->szCharName,lpPackage->wProductGroup[0],lpMsg->dwProductId);

			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
			return;
		}
		else if( lpProduct->Price > pMsg.dwUserCash[btPriceType] )
		{
			pMsg.dwResult = 1;

			g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - No enough coins! ProductPrice:[%d], UserCoins:[%d]",
				lpMsg->dwUserGuid,lpMsg->szCharName,lpProduct->Price,pMsg.dwUserCash[btPriceType]);

			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
			return;
		}
	}

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT count(*) as STORAGE_COUNT FROM GameShop_Storage WHERE memb_guid = %d AND stType = 83",dwUserGuid);
#else
	sprintf(szQuery,"SELECT count(*) as STORAGE_COUNT FROM T_CashShop_STORAGE WHERE memb_guid = %d AND stType = 83",dwUserGuid);
#endif

	if( !g_CashShopDB.Exec(szQuery) || g_CashShopDB.Fetch() == SQL_NO_DATA )
	{
		g_CashShopDB.Clear();
		pMsg.dwResult = -1;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - Could not access storage info!",
			lpMsg->dwUserGuid,lpMsg->szCharName);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

	int stCount = g_CashShopDB.GetInt("STORAGE_COUNT");
	g_CashShopDB.Clear();

	if( (lpPackage->btProductCount+stCount) > 9 )
	{
		pMsg.dwResult = 2;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - No enough space in storage!",
			lpMsg->dwUserGuid,lpMsg->szCharName);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

	if( lpPackage->btUseType == 0 )
	{
		pMsg.dwUserCash[btPriceType] -= lpPackage->Price;

		for(int i = 0; i < lpPackage->btProductCount; i++)
		{
			lpProduct = SearchShopProduct(lpPackage->wProductGroup[i],lpMsg->dwProductId);

			if( lpProduct != NULL )
			{
				ShopUserAddToStorage(lpMsg->szCharName,dwUserGuid,lpPackage->wProductGroup[i],lpProduct);
			}
			else
			{
				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - Product not found! ProductInfo(Group: %d)",
					lpMsg->dwUserGuid,lpMsg->szCharName,lpPackage->wProductGroup[i]);
			}
		}
	}
	else
	{
		pMsg.dwUserCash[btPriceType] -= lpProduct->Price;
		ShopUserAddToStorage(lpMsg->szCharName,dwUserGuid,lpPackage->wProductGroup[0],lpProduct);
	}

	UpdateUserCashPoints(dwUserGuid,pMsg.dwUserCash[0],pMsg.dwUserCash[1],pMsg.dwUserCash[2]);

	g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST] - SUCCESS [%d](%s) Category: %d, PackageID: %d, ProductID: %d, wCoin_c: %d, wCoin_p:%d, wCoin_g:%d",
		dwUserGuid,lpMsg->szCharName,lpMsg->dwPackageCategory,lpMsg->dwPackageId,lpMsg->dwProductId,pMsg.dwUserCash[0],pMsg.dwUserCash[1], pMsg.dwUserCash[2]);

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
}

PACKAGE_INFO* SearchShopPackage(int CategoryID,int Guid)
{
	int pCount = shopPackageList.count(CategoryID);

	if( !pCount )
	{
		return NULL;
	}

	shopPackage::iterator _It = shopPackageList.find(CategoryID);

	if( _It == shopPackageList.end() )
	{
		return NULL;
	}

	for(int i = 0; i < pCount; i++,_It++)
	{
		PACKAGE_INFO* lpPackage = &_It->second;

		if( lpPackage->wGuid == Guid )
		{
			return lpPackage;
		}
	}

	return NULL;
}

PRODUCT_INFO* SearchShopProduct(int groupID,int Guid)
{
	int pCount = shopProductList.count(groupID);

	if( !pCount )
	{
		return NULL;
	}

	shopProduct::iterator _It = shopProductList.find(groupID);

	if( _It == shopProductList.end() )
	{
		return NULL;
	}

	for(int i = 0; i < pCount; i++,_It++)
	{
		PRODUCT_INFO* lpProduct = &_It->second;

		if( lpProduct->wProductGuid == Guid || Guid == 0 )
		{
			return lpProduct;
		}
	}

return NULL;
}

void ShopUserAddToStorage(char* szCharName,int memb_guid,int productGroup,PRODUCT_INFO* p)
{
	char szQuery[1024];


#ifdef ZTEAMDB
#if (EX700==1)
	sprintf(szQuery,"INSERT INTO GameShop_Storage (memb_guid,productGroup,productId,productItem,productDur,productTime,productType,stType,productLevel,productOp1,productOp2,productOp3,productExcOp,productAncOp) VALUES (%d,%d,%d,%d,%d,%d,%d,83,%d,%d,%d,%d,%d,%d)",
		memb_guid,productGroup,p->wProductGuid,p->aItemId,p->btDur,p->Duration,p->btItemType,p->btLevel,p->btOption1,p->btOption2,p->btOption3,p->btExcOp,p->btAncOp);
#else
	sprintf(szQuery,"INSERT INTO GameShop_Storage (memb_guid,productGroup,productId,productItem,productDur,productTime,productType,stType) VALUES (%d,%d,%d,%d,%d,%d,%d,83)",
		memb_guid,productGroup,p->btProductGuid,p->aItemId,p->btDur,p->Duration,p->btItemType);
#endif
#else
#if (EX700==1)
	sprintf(szQuery,"INSERT INTO T_CashShop_STORAGE (memb_guid,productGroup,productId,productItem,productDur,productTime,productType,stType,productLevel,productOp1,productOp2,productOp3,productExcOp,productAncOp) VALUES (%d,%d,%d,%d,%d,%d,%d,83,%d,%d,%d,%d,%d,%d)",
		memb_guid,productGroup,p->wProductGuid,p->aItemId,p->btDur,p->Duration,p->btItemType,p->btLevel,p->btOption1,p->btOption2,p->btOption3,p->btExcOp,p->btAncOp);
#else
	sprintf(szQuery,"INSERT INTO T_CashShop_STORAGE (memb_guid,productGroup,productId,productItem,productDur,productTime,productType,stType) VALUES (%d,%d,%d,%d,%d,%d,%d,83)",
		memb_guid,productGroup,p->btProductGuid,p->aItemId,p->btDur,p->Duration,p->btItemType);
#endif
#endif
	

	if( !g_CashShopDB.Exec(szQuery) )
	{
		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][STORAGE ADD] [%d](%s) - Failed to add Group: %d, ProductId: %d",
			memb_guid,szCharName,productGroup,p->wProductGuid);
	}

	g_CashShopDB.Clear();
}

void ShopUserAddToGift(char* szCharName,int memb_guid,int productGroup,PRODUCT_INFO* p,char* szFrom,char* szText)
{
char szQuery[2048];

#ifdef ZTEAMDB
#if(EX700==1)
	sprintf(szQuery,"INSERT INTO GameShop_Storage (memb_guid,productGroup,productId,productItem,productDur,productTime,productType,stType,memb_user,item_text,productLevel,productOp1,productOp2,productOp3,productExcOp,productAncOp) VALUES (%d,%d,%d,%d,%d,%d,%d,71,'%s','%s',%d,%d,%d,%d,%d,%d)",
		memb_guid,productGroup,p->wProductGuid,p->aItemId,p->btDur,p->Duration,p->btItemType,szFrom,szText,p->btLevel,p->btOption1,p->btOption2,p->btOption3,p->btExcOp,p->btAncOp);
#else
	sprintf(szQuery,"INSERT INTO GameShop_Storage (memb_guid,productGroup,productId,productItem,productDur,productTime,productType,stType,memb_user,item_text) VALUES (%d,%d,%d,%d,%d,%d,%d,71,'%s','%s')",
		memb_guid,productGroup,p->btProductGuid,p->aItemId,p->btDur,p->Duration,p->btItemType,szFrom,szText);
#endif
#else
#if(EX700==1)
	sprintf(szQuery,"INSERT INTO T_CashShop_STORAGE (memb_guid,productGroup,productId,productItem,productDur,productTime,productType,stType,memb_user,item_text,productLevel,productOp1,productOp2,productOp3,productExcOp,productAncOp) VALUES (%d,%d,%d,%d,%d,%d,%d,71,'%s','%s',%d,%d,%d,%d,%d,%d)",
		memb_guid,productGroup,p->wProductGuid,p->aItemId,p->btDur,p->Duration,p->btItemType,szFrom,szText,p->btLevel,p->btOption1,p->btOption2,p->btOption3,p->btExcOp,p->btAncOp);
#else
	sprintf(szQuery,"INSERT INTO T_CashShop_STORAGE (memb_guid,productGroup,productId,productItem,productDur,productTime,productType,stType,memb_user,item_text) VALUES (%d,%d,%d,%d,%d,%d,%d,71,'%s','%s')",
		memb_guid,productGroup,p->btProductGuid,p->aItemId,p->btDur,p->Duration,p->btItemType,szFrom,szText);
#endif
#endif

	if( !g_CashShopDB.Exec(szQuery) )
	{
		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][STORAGE ADD] [%d](%s) - Failed to add Group: %d, ProductId: %d",
			memb_guid,szCharName,productGroup,p->wProductGuid);
	}

	g_CashShopDB.Clear();
}

void UpdateUserCashPoints(int memb_guid,int wCoin_c,int wCoin_p,int wCoin_g)
{
	char szQuery[256];

#ifdef ZTEAMDB
	sprintf(szQuery,"UPDATE GameShop_Data SET WCoinC = %d,WCoinP = %d,WCoinG = %d WHERE MemberGuid = %d",
		wCoin_c,wCoin_p,wCoin_g,memb_guid);
#else
	sprintf(szQuery,"UPDATE T_CashShop_DATA SET wcoin_c = %d,wcoin_p = %d,wcoin_g = %d WHERE memb_guid = %d",
		wCoin_c,wCoin_p,wCoin_g,memb_guid);
#endif

	if( !g_CashShopDB.Exec(szQuery) )
	{
		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][POINTS UPDATE] [%d] - Failed to update points(wCoint_c: %d, wCoin_p:%d, wCoin_g: %d)",
			memb_guid,wCoin_c,wCoin_p,wCoin_g);
	}

	g_CashShopDB.Clear();

	g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop] [Guid:%d] UpdatePoint info [%d/%d/%d]",memb_guid,wCoin_c,wCoin_p,wCoin_g);
}

void SGAndCashItemListEx(int aIndex,protocol::MSG_GTOS_ITEM_LIST_REQ_EX *lpMsg)
{
	char szQuery[256];

	protocol::MSG_STOG_ITEM_LIST_ANS_EX pMsg;

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT guid,productGroup,productId,productType FROM GameShop_Storage WHERE memb_guid = %d AND stType = 83",
		lpMsg->dwUserGuid);
#else
	sprintf(szQuery,"SELECT guid,productGroup,productId,productType FROM T_CashShop_STORAGE WHERE memb_guid = %d AND stType = 83",
		lpMsg->dwUserGuid);
#endif

	if( !g_CashShopDB.Exec(szQuery) )
	{
		g_CashShopDB.Clear();
		return;
	}

	pMsg.dwUserGuid = lpMsg->dwUserGuid;
	pMsg.dwPageId = lpMsg->dwPageId;
	pMsg.dwItemCount = 0;

	while( g_CashShopDB.Fetch() != SQL_NO_DATA )
	{

		pMsg.itemList[pMsg.dwItemCount].dwGuid = g_CashShopDB.GetInt("guid");
		pMsg.itemList[pMsg.dwItemCount].dwGroupIndex = g_CashShopDB.GetInt("productGroup");
		pMsg.itemList[pMsg.dwItemCount].dwProductIndex = g_CashShopDB.GetInt("productId");
		pMsg.itemList[pMsg.dwItemCount].dwType = g_CashShopDB.GetInt("productType");

		pMsg.itemList[pMsg.dwItemCount].dwUnk1 = 1;
		pMsg.itemList[pMsg.dwItemCount].dwUnk2 = 2;
		pMsg.itemList[pMsg.dwItemCount].Durability = 0;

		pMsg.dwItemCount++;

		if( pMsg.dwItemCount >= MAX_STORAGE_PAGE_ITEMS )
			break;
	}

	if( pMsg.dwItemCount == 0 )
	{
		pMsg.dwPageId = 0;
	}

	g_CashShopDB.Clear();
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
}

void SGAnsCashItemUseEx(int aIndex,protocol::MSG_GTOS_ITEM_USE_REQ_EX * lpMsg)
{
	char szQuery[1024];
	protocol::MSG_STOG_ITEM_USE_ANS_EX pMsg;

	pMsg.dwUserGuid = lpMsg->dwUserGuid;
	pMsg.dwResult	= 0;

	if( lpMsg->dwItemType == 80 )
	{
#ifdef ZTEAMDB
		sprintf(szQuery,"SELECT productItem,productDur,productTime,productLevel,productOp1,productOp2,productOp3,productExcOp,productAncOp FROM GameShop_Storage WHERE guid = %d AND memb_guid = %d",
			lpMsg->dwItemGuid,lpMsg->dwUserGuid);
#else
		sprintf(szQuery,"SELECT productItem,productDur,productTime,productLevel,productOp1,productOp2,productOp3,productExcOp,productAncOp FROM T_CashShop_STORAGE WHERE guid = %d AND memb_guid = %d",
			lpMsg->dwItemGuid,lpMsg->dwUserGuid);
#endif

		if( !g_CashShopDB.Exec(szQuery) || g_CashShopDB.Fetch() == SQL_NO_DATA )
		{
			pMsg.dwResult = 1;

			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
			return;
		}

		pMsg.itemInfo.dwItemGuid = lpMsg->dwItemGuid;
		pMsg.itemInfo.dwItemId = g_CashShopDB.GetInt("productItem");
		pMsg.itemInfo.dwDurability = g_CashShopDB.GetInt("productDur");
		pMsg.itemInfo.dwUseTime = g_CashShopDB.GetInt("productTime");

#if(EX700==1)
		pMsg.itemInfo.btLevel = g_CashShopDB.GetInt("productLevel");
		pMsg.itemInfo.btOption1 = g_CashShopDB.GetInt("productOp1");
		pMsg.itemInfo.btOption2 = g_CashShopDB.GetInt("productOp2");
		pMsg.itemInfo.btOption3 = g_CashShopDB.GetInt("productOp3");
		pMsg.itemInfo.btExcOp = g_CashShopDB.GetInt("productExcOp");
		pMsg.itemInfo.btAncOp = g_CashShopDB.GetInt("productAncOp");
#endif


		g_CashShopDB.Clear();
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
	}
}

void SGAnsUseCashItemNotifyEx(int aIndex,protocol::MSG_GTOS_USE_ITEM_NTF_EX *aRecv)
{
	char szQuery[512];

#ifdef ZTEAMDB
	sprintf(szQuery,"DELETE FROM GameShop_Storage WHERE guid = %d AND memb_guid = %d",
		aRecv->dwItemGuid,aRecv->dwUserGuid);
#else
	sprintf(szQuery,"DELETE FROM T_CashShop_STORAGE WHERE guid = %d AND memb_guid = %d",
		aRecv->dwItemGuid,aRecv->dwUserGuid);
#endif

	if( !g_CashShopDB.Exec(szQuery) )
	{
		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][DELETE REQUEST][UserGuid: %d] - Failed (ItemGuid:%d)",
			aRecv->dwUserGuid,aRecv->dwItemGuid);
	}

	g_CashShopDB.Clear();
}

void SGAndCashGiftListEx(int aIndex,protocol::MSG_GTOS_GIFT_LIST_REQ_EX * lpMsg)
{
	char szQuery[256];
	protocol::MSG_STOG_GIFT_LIST_ANS_EX pMsg;

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT guid,productGroup,productId,productType,memb_user,item_text FROM GameShop_Storage WHERE memb_guid = %d AND stType = 71",
		lpMsg->dwUserGuid);
#else
	sprintf(szQuery,"SELECT guid,productGroup,productId,productType,memb_user,item_text FROM T_CashShop_STORAGE WHERE memb_guid = %d AND stType = 71",
		lpMsg->dwUserGuid);
#endif

	if( !g_CashShopDB.Exec(szQuery) )
	{
		g_CashShopDB.Clear();
		return;
	}

	pMsg.dwUserGuid = lpMsg->dwUserGuid;
	pMsg.dwPageId = lpMsg->dwPageId;
	pMsg.dwItemCount = 0;

	while( g_CashShopDB.Fetch() != SQL_NO_DATA )
	{

		pMsg.itemList[pMsg.dwItemCount].dwGuid = g_CashShopDB.GetInt("guid");
		pMsg.itemList[pMsg.dwItemCount].dwGroupIndex = g_CashShopDB.GetInt("productGroup");
		pMsg.itemList[pMsg.dwItemCount].dwProductIndex = g_CashShopDB.GetInt("productId");
		pMsg.itemList[pMsg.dwItemCount].dwType = g_CashShopDB.GetInt("productType");

		pMsg.itemList[pMsg.dwItemCount].dwUnk1 = 1;
		pMsg.itemList[pMsg.dwItemCount].dwUnk2 = 2;
		pMsg.itemList[pMsg.dwItemCount].Durability = 0;

		g_CashShopDB.GetStr("memb_user",pMsg.itemList[pMsg.dwItemCount].szFrom);
		g_CashShopDB.GetStr("item_text",pMsg.itemList[pMsg.dwItemCount].szText);

		pMsg.dwItemCount++;

		if( pMsg.dwItemCount >= MAX_STORAGE_PAGE_ITEMS )
			break;
	}

	if( pMsg.dwItemCount == 0 )
	{
		pMsg.dwPageId = 0;
	}

	g_CashShopDB.Clear();
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
}

BOOL CheckSQL(char* szText)
{
	int Len = strlen(szText);

	for(int i = 0; i < Len; i++)
	{
		if( szText[i] == '\'' )
			return false;
	}

return true;
}

void SGAnsGiftCashItemEx(int aIndex,protocol::MSG_GTOS_GIFT_ITEM_REQ_EX *lpMsg)
{
	char szQuery[512];
	protocol::MSG_STOG_GIFT_ITEM_ANS_EX pMsg;

	if( !CheckSQL(lpMsg->szToName) || !CheckSQL(lpMsg->szText) )
	{
		pMsg.dwResult = -1;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][GIFT REQUEST - FAIL] [%d](%s) - SQL Check failed!",
			lpMsg->dwUserGuid,lpMsg->szCharName);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

	pMsg.dwResult = 0;
	DWORD dwUserGuid = lpMsg->dwUserGuid;

	pMsg.dwServerGuid = lpMsg->dwServerGuid;
	pMsg.dwUserGuid = dwUserGuid;
	pMsg.dwCharGuid = lpMsg->dwCharGuid;

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT WCoinC,WCoinP,WCoinG FROM GameShop_Data WHERE MemberGuid = %d",dwUserGuid);
#else
	sprintf(szQuery,"SELECT wcoin_c,wcoin_p,wcoin_g FROM T_CashShop_DATA WHERE memb_guid = %d",dwUserGuid);
#endif

	if( !g_CashShopDB.Exec(szQuery) || g_CashShopDB.Fetch() == SQL_NO_DATA )
	{
		g_CashShopDB.Clear();
		pMsg.dwResult = -1;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][GIFT REQUEST - FAIL] [%d](%s) - User not found in DB",
			lpMsg->dwUserGuid,lpMsg->szCharName);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

#ifdef ZTEAMDB
	pMsg.dwUserCash[0] = g_CashShopDB.GetInt("WCoinC");
	pMsg.dwUserCash[1] = g_CashShopDB.GetInt("WCoinP");
	pMsg.dwUserCash[2] = g_CashShopDB.GetInt("WCoinG");
#else
	pMsg.dwUserCash[0] = g_CashShopDB.GetInt("wcoin_c");
	pMsg.dwUserCash[1] = g_CashShopDB.GetInt("wcoin_p");
	pMsg.dwUserCash[2] = g_CashShopDB.GetInt("wcoin_g");
#endif

	g_CashShopDB.Clear();

	PACKAGE_INFO* lpPackage = SearchShopPackage(lpMsg->dwPackageCategory,lpMsg->dwPackageId);
	PRODUCT_INFO* lpProduct = NULL;

	if( lpPackage == NULL )
	{
		pMsg.dwResult = 5;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][GIFT REQUEST - FAIL] [%d](%s) - Package not found! PackageInfo(Category: %d, Guid: %d, ItemId: [%d %d])",
			lpMsg->dwUserGuid,lpMsg->szCharName,lpMsg->dwPackageCategory,lpMsg->dwPackageId,lpMsg->ItemID/512,lpMsg->ItemID%512);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

	if( lpPackage->bGift == FALSE )
	{
		pMsg.dwResult = 7;

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

	BYTE btPriceType = lpPackage->btPriceType;

	if( btPriceType < 0 || btPriceType >= 3 )
	{
		pMsg.dwResult = -1;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][GIFT REQUEST - FAIL] [%d](%s) - Price type out of bounds: (pType: %d)",
			dwUserGuid,lpMsg->szCharName,lpPackage->btPriceType);
		return;
	}

	if( lpPackage->btUseType == 0 )
	{
		if( lpPackage->Price > pMsg.dwUserCash[btPriceType] )
		{
			pMsg.dwResult = 1;

			g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][GIFT REQUEST - FAIL] [%d](%s) - No enough coins! PackagePrice:[%d], UserCoins:[%d]",
				lpMsg->dwUserGuid,lpMsg->szCharName,lpPackage->Price,pMsg.dwUserCash[btPriceType]);

			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
			return;
		}
	}
	else
	{
		lpProduct = SearchShopProduct(lpPackage->wProductGroup[0],lpMsg->dwProductId);

		if( lpProduct == NULL )
		{
			pMsg.dwResult = 5;

			g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - Product not found! ProductInfo(Group: %d, Guid: %d)",
				lpMsg->dwUserGuid,lpMsg->szCharName,lpPackage->wProductGroup[0],lpMsg->dwProductId);

			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
			return;
		}
		else if( lpProduct->Price > pMsg.dwUserCash[btPriceType] )
		{
			pMsg.dwResult = 1;

			g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - No enough coins! ProductPrice:[%d], UserCoins:[%d]",
				lpMsg->dwUserGuid,lpMsg->szCharName,lpProduct->Price,pMsg.dwUserCash[btPriceType]);

			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
			return;
		}
	}

	DWORD dwRecGuid;
	sprintf(szQuery,"SELECT memb_guid FROM MEMB_INFO WHERE memb___id = (SELECT AccountID FROM Character WHERE Name = '%s')",lpMsg->szToName);

	if( !g_CashShopDB.Exec(szQuery) || g_CashShopDB.Fetch() == SQL_NO_DATA )
	{
		pMsg.dwResult = 3;

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

	dwRecGuid = g_CashShopDB.GetInt("memb_guid");
	g_CashShopDB.Clear();

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT count(*) as STORAGE_COUNT FROM GameShop_Storage WHERE memb_guid = %d AND stType = 71",dwRecGuid);
#else
	sprintf(szQuery,"SELECT count(*) as STORAGE_COUNT FROM T_CashShop_STORAGE WHERE memb_guid = %d AND stType = 71",dwRecGuid);
#endif

	if( !g_CashShopDB.Exec(szQuery) || g_CashShopDB.Fetch() == SQL_NO_DATA )
	{
		g_CashShopDB.Clear();
		pMsg.dwResult = -1;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - Could not access storage info!",
			lpMsg->dwUserGuid,lpMsg->szCharName);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

	int stCount = g_CashShopDB.GetInt("STORAGE_COUNT");
	g_CashShopDB.Clear();

	if( (lpPackage->btProductCount+stCount) > 9 )
	{
		pMsg.dwResult = 2;

		g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][GIFT REQUEST - FAIL] [%d](%s) - No enough space in storage!",
			lpMsg->dwUserGuid,lpMsg->szCharName);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
		return;
	}

	if( lpPackage->btUseType == 0 )
	{
		pMsg.dwUserCash[btPriceType] -= lpPackage->Price;

		for(int i = 0; i < lpPackage->btProductCount; i++)
		{
			lpProduct = SearchShopProduct(lpPackage->wProductGroup[i],lpMsg->dwProductId);

			if( lpProduct != NULL )
			{
				ShopUserAddToGift(lpMsg->szToName,dwRecGuid,lpPackage->wProductGroup[i],lpProduct,lpMsg->szCharName,lpMsg->szText);
			}
			else
			{
				g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST - FAIL] [%d](%s) - Product not found! ProductInfo(Group: %d)",
					lpMsg->dwUserGuid,lpMsg->szCharName,lpPackage->wProductGroup[i]);
			}
		}
	}
	else
	{
		pMsg.dwUserCash[btPriceType] -= lpProduct->Price;
		ShopUserAddToGift(lpMsg->szToName,dwRecGuid,lpPackage->wProductGroup[0],lpProduct,lpMsg->szCharName,lpMsg->szText);
	}

	UpdateUserCashPoints(dwUserGuid,pMsg.dwUserCash[0],pMsg.dwUserCash[1],pMsg.dwUserCash[2]);

	g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[CashShop][BUY REQUEST] - SUCCESS [%d](%s) Category: %d, PackageID: %d, ProductID: %d, wCoin_c: %d, wCoin_p:%d, wCoin_g:%d",
		dwUserGuid,lpMsg->szCharName,lpMsg->dwPackageCategory,lpMsg->dwPackageId,lpMsg->dwProductId,pMsg.dwUserCash[0],pMsg.dwUserCash[1], pMsg.dwUserCash[2]);

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
}

void SGUpdatePointInfo(int aIndex,protocol::MSG_GTOS_UPDATE_POINT_EX * aRecv)
{
	char szQuery[512];

	DWORD dwUserGuid = aRecv->dwUserGuid;

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT WCoinC,WCoinP,WCoinG FROM GameShop_Data WHERE MemberGuid = %d",dwUserGuid);
#else
	sprintf(szQuery,"SELECT wcoin_c,wcoin_p,wcoin_g FROM T_CashShop_DATA WHERE memb_guid = %d",dwUserGuid);
#endif

	if( !g_CashShopDB.Exec(szQuery) )
	{
		g_CashShopDB.Clear();
		return;
	}

	int wCoin_c;
	int wCoin_p;
	int wCoin_g;

	if( g_CashShopDB.Fetch() == SQL_NO_DATA )
	{
		g_CashShopDB.Clear();

#ifdef ZTEAMDB
		sprintf(szQuery,"INSERT INTO GameShop_Data (MemberGuid,WCoinC,WCoinP,WCoinG) VALUES (%d,0,0,0)",dwUserGuid);
#else
		sprintf(szQuery,"INSERT INTO T_CashShop_DATA (memb_guid,wcoin_c,wcoin_p,wcoin_g) VALUES (%d,0,0,0)",dwUserGuid);
#endif
		g_CashShopDB.Exec(szQuery);

		wCoin_c = aRecv->dwAddPoint[0];
		wCoin_p = aRecv->dwAddPoint[1];
		wCoin_g = aRecv->dwAddPoint[2];
	}
	else
	{
#ifdef ZTEAMDB
		wCoin_c = g_CashShopDB.GetInt("WCoinC")+ aRecv->dwAddPoint[0];
		wCoin_p = g_CashShopDB.GetInt("WCoinP")+ aRecv->dwAddPoint[1];
		wCoin_g = g_CashShopDB.GetInt("WCoinG")+ aRecv->dwAddPoint[2];
#else
		wCoin_c = g_CashShopDB.GetInt("wcoin_c")+ aRecv->dwAddPoint[0];
		wCoin_p = g_CashShopDB.GetInt("wcoin_p")+ aRecv->dwAddPoint[1];
		wCoin_g = g_CashShopDB.GetInt("wcoin_g")+ aRecv->dwAddPoint[2];
#endif
	}

	g_Window.ServerLogAdd(ST_CASHSHOPSERVER,"[%d/%d/%d]",aRecv->dwAddPoint[0],aRecv->dwAddPoint[1],aRecv->dwAddPoint[2]);

	g_CashShopDB.Clear();

	UpdateUserCashPoints(dwUserGuid,wCoin_c,wCoin_p,wCoin_g);

	protocol::MSG_STOG_USER_CASH_ANS_EX pMsg;

	pMsg.dwUserGuid		= dwUserGuid;
	pMsg.dwUserCash[0]	= wCoin_c;
	pMsg.dwUserCash[1]	= wCoin_p;
	pMsg.dwUserCash[2]	= wCoin_g;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
}