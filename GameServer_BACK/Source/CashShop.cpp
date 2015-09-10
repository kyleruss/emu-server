#include "stdafx.h"
#include "CashShop.h"
#include "CashItemPeriodSystem.h"
#include "wsShopServerCli.h"
#include "GameServer.h"
#include "GameMain.h"
#include "LogProc.h"
#include "DSProtocol.h"
#include "..\include\Readscript.h"
#include "..\common\winutil.h"
#include "ObjUseSkill.h"


BOOL g_bUseCashShop = FALSE;
BOOL g_bConnectShopServer = FALSE;
BOOL g_bShopServerConnectState = FALSE;
char g_ShopServerIP[16] ={0};
BOOL g_bUseMoveMapBound = FALSE;

WNDPROC CCashShop::m_lpOldProc = NULL;

CCashShop g_CashShop;
wsShopServerCli g_ShopServerClient;
int g_ShopserverPort = 0;


void ShopServerProtocolCore(DWORD protoNum, LPBYTE aRecv, int aLen)
{
	switch ( protoNum )
	{
		case 0x200:SGAnsCashPoint((protocol::MSG_STOG_USER_CASH_ANS_EX *)aRecv);break;
		case 0x400:SGAnsCashItemList((protocol::MSG_STOG_ITEM_LIST_ANS_EX *)aRecv);break;
		case 0x600:SGAnsUseCashItem((protocol::MSG_STOG_ITEM_USE_ANS_EX *)aRecv);break;
		case 0x800:SGAnsCashGiftList((protocol::MSG_STOG_GIFT_LIST_ANS_EX *) aRecv); break;
		case 0xA00:SGAnsBuyCashItem((protocol::MSG_STOG_BUY_ITEM_ANS_EX *)aRecv);break;
		case 0xE00:SGAnsGiftCashItem((protocol::MSG_STOG_GIFT_ITEM_ANS_EX *)aRecv);break;

		/*case 0x400:SGAnsCashItemList((protocol::MSG_STOG_ITEM_LIST_ANS *)aRecv);break;
		case 0x600:SGAnsPackageItemList((protocol::MSG_STOG_PACKAGE_LIST_ANS *)aRecv);break;
		case 0x800:SGAnsBranchItemList((protocol::MSG_STOG_BRANCH_ITEM_LIST_ANS *)aRecv);break;
		case 0xE00:g_CashShop.CashShopOptioNReload();break;
		case 0xA00:SGAnsBuyCashItem((protocol::MSG_STOG_BUY_ITEM_ANS *)aRecv);break;*/
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCashShop::CCashShop()
{

}

CCashShop::~CCashShop()
{

}

void CCashShop::Initialize()
{
	this->bCashItemListReload = FALSE;
	//this->MapUserObject.clear();
}

void CCashShop::LoadShopOption(LPSTR pchFilename)
{
	LogAddTD("[CashShop][ShopOption] - Shop Option Reload '%s'", pchFilename);

	g_bUseCashShop = GetPrivateProfileInt("CashShopOption", "UseCashShop", 0, pchFilename);
	g_bConnectShopServer = GetPrivateProfileInt("CashShopOption", "ConnectShopServer", 0, pchFilename);
	GetPrivateProfileString("CashShopOption", "ShopServerIP", "0", g_ShopServerIP, 16, pchFilename);
	g_ShopserverPort = GetPrivateProfileInt("CashShopOption", "ShopServerPort", 0, pchFilename);
	g_bUseMoveMapBound = GetPrivateProfileInt("CashShopOption", "MapMoveBoundCheck", 0, pchFilename);

	char szTemp[32];
	char* cvstr;
	GetPrivateProfileString("CashShopOption","ScriptVersion","000.0000.000",szTemp,sizeof(szTemp),pchFilename);

	cvstr = strtok(szTemp, ".");

	m_ScriptVer.Ver1 = atoi(cvstr);
	cvstr = strtok(NULL, ".");
	m_ScriptVer.Ver2 = atoi(cvstr);
	cvstr = strtok(NULL, ".");
	m_ScriptVer.Ver3 = atoi(cvstr);

	GetPrivateProfileString("CashShopOption","BannerVersion","000.0000.000",szTemp,sizeof(szTemp),pchFilename);

	cvstr = strtok(szTemp, ".");

	m_BannerVer.Ver1 = atoi(cvstr);
	cvstr = strtok(NULL, ".");
	m_BannerVer.Ver2 = atoi(cvstr);
	cvstr = strtok(NULL, ".");
	m_BannerVer.Ver3 = atoi(cvstr);
	
	this->Initialize();

	if ( g_bConnectShopServer == TRUE && g_bShopServerConnectState == FALSE)
	{
		g_bShopServerConnectState = this->ConnectShopServer(g_ShopServerIP, g_ShopserverPort);

		if ( g_bShopServerConnectState == FALSE )
		{
			MsgBox("[CashShop] Shop Server Connect Failed!!");
		}
	}
}


void CCashShop::CashShopOptioNReload()
{
	bCashItemListReload = TRUE;
	LogAddTD("[CashShop][OptionLoad] - CashShop Option Load Begin");

	this->LoadShopOption(gDirPath.GetNewPath(FILE_CASHSHOP_OPTION));
	this->ReConnectShopServer();

	LogAddTD("[CashShop][OptionLoad] - CashShop Option Load finish");

	bCashItemListReload = FALSE;
}

BOOL CCashShop::ConnectShopServer(LPSTR pchIpAddress, int iPortNumber)
{
	if ( g_ShopServerClient.CreateSocket(ghWnd) == FALSE )
		return FALSE;

	if ( !g_ShopServerClient.SetProtocolCore(ShopServerProtocolCore) )
		return FALSE;

	CCashShop::m_lpOldProc = (WNDPROC)SetWindowLong(ghWnd, GWL_WNDPROC, (LONG)CCashShop::ParentWndProc);

	if ( !g_ShopServerClient.Connect(pchIpAddress, iPortNumber, 0x41E) )
		return FALSE;

	return TRUE;
}

BOOL CCashShop::ReConnectShopServer()
{
	if ( g_ShopServerClient.GetConnect() == TRUE )
		return TRUE;

	g_ShopServerClient.Close();

	if ( g_ShopServerClient.CreateSocket(ghWnd) == FALSE )
		return FALSE;

	if ( !g_ShopServerClient.SetProtocolCore(ShopServerProtocolCore) )
		return FALSE;

	if ( !g_ShopServerClient.Connect(g_ShopServerIP, g_ShopserverPort, 0x41E) )
		return FALSE;

	return TRUE;
}

long CCashShop::ParentWndProc(HWND hWnd, UINT iMessage, UINT wParam, long lParam)
{
	switch( iMessage )
	{
		case 0x41E:
			switch ( lParam & 0xFFFF & 0xFFFF )
			{
				case 1:
					g_ShopServerClient.DataRecv();
					return 0;

				case 16:
					LogAddTD("[ShopServer] - Connect To Shop Server!!");
					g_bShopServerConnectState = TRUE;
					return 0;

				case 32:
					g_ShopServerClient.Close();
					LogAddC(2, "[ShopServer] - Disconnect From Shop Server!!");
					g_bShopServerConnectState = FALSE;
					return 0;

			}
			return 0;

		default:
			return CallWindowProc(CCashShop::m_lpOldProc, hWnd, iMessage, wParam, lParam);
	}
}

void CCashShop::CheckShopServerConnectState()
{
	if ( g_bConnectShopServer == FALSE )
		return;

	if ( (GetTickCount()-this->dwCheckShopServerConnectStatePeriod) > 10000 )
	{
		this->dwCheckShopServerConnectStatePeriod = GetTickCount();

		if ( g_bShopServerConnectState == FALSE )
		{
			g_bShopServerConnectState = this->ReConnectShopServer();

			if ( g_bShopServerConnectState == FALSE )
			{
				LogAddC(2, "[CashShop][ShopServer] - Disconnect From Shop Server!!");
			}
		}
	}
}

BOOL CCashShop::AddUser(LPOBJ lpObj)
{
	if ( this->SearchUser(lpObj->DBNumber) )
		return FALSE;

	lpObj->m_wcCashPoint = 0;
	lpObj->m_wpCashPoint = 0;
	lpObj->m_wgCashPoint = 0;

	lpObj->GoblinPointTick = GetTickCount();

	MapUserObject.insert(std::pair<int, LPOBJ>(lpObj->DBNumber, lpObj));

	GCScriptVersionSend(lpObj->m_Index);
	GCBannerVersionSend(lpObj->m_Index);
	return TRUE;
}



BOOL CCashShop::DeleteUser(LPOBJ lpObj)
{
	std::map<int, LPOBJ>::iterator Iter = this->MapUserObject.find(lpObj->DBNumber);
	
	if ( Iter == this->MapUserObject.end() )
	{
		return FALSE;
	}


	this->MapUserObject.erase(Iter);
	return TRUE;
}


LPOBJ CCashShop::SearchUser(int iUserGuid)
{
	LPOBJ lpReturnObj = NULL;

	std::map<int, LPOBJ>::iterator Iter;
	Iter = this->MapUserObject.find(iUserGuid);

	if ( Iter == this->MapUserObject.end() )
		return FALSE;

	lpReturnObj = Iter->second;

	return lpReturnObj;
}

BOOL CCashShop::CGCashShopOpen(LPOBJ lpObj, PMSG_REQ_CASHSHOPOPEN* lpMsg)
{
	BYTE btResult = TRUE;

	if ( bCashItemListReload == TRUE )
	{
		btResult = FALSE;
	}

	if ( g_bConnectShopServer == TRUE && g_bShopServerConnectState == FALSE )
	{
		btResult = FALSE;
	}

	if ( !gObjIsAccontConnect(lpObj->m_Index, lpObj->AccountID))
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 175)), lpObj->AccountID, lpObj->m_Index);
		return FALSE;
	}

	if ( lpObj->Connected <= PLAYER_LOGGED || lpObj->CloseCount != -1 )
		return FALSE;

	if ( lpMsg->btShopOpenType == 0 )
	{
		if ( lpObj->m_IfState.use > 0 )
		{
			btResult = FALSE;
		}

		if ( btResult == TRUE )
		{
			lpObj->m_IfState.use = 1;
			lpObj->m_IfState.type = 18;
			lpObj->m_IfState.state = 1;
		}
	}
	else if ( lpMsg->btShopOpenType == TRUE )
	{
		lpObj->m_IfState.use = 0;
		lpObj->m_IfState.type = 0;
		lpObj->m_IfState.state = 0;

		btResult = FALSE;
	}

	PMSG_ANS_CASHSHOPOPEN pMsg;

	pMsg.btResult = btResult;

	PHeadSubSetB((LPBYTE)&pMsg, 0xD2, 0x02, sizeof(PMSG_ANS_CASHSHOPOPEN));

	DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
return TRUE;
}

BOOL CCashShop::CGCashPoint(LPOBJ lpObj)
{
	DWORD dwUserGuid = 0;
	
	if ( g_bConnectShopServer == TRUE && g_bShopServerConnectState == FALSE )
	{
		return FALSE;
	}

	if ( !gObjIsAccontConnect(lpObj->m_Index, lpObj->AccountID))
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 175)), lpObj->AccountID, lpObj->m_Index);
		return FALSE;
	}

	if ( lpObj->Connected !=PLAYER_PLAYING && lpObj->Type != OBJ_USER )
		return FALSE;

	dwUserGuid = lpObj->DBNumber;
	GSReqCashPoint(dwUserGuid);

	return TRUE;
}


#pragma pack(1)
struct PMSG_ANS_CASHPOINT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	double pTmp;
	double wCoin;
	double pCoin;
	double pTmp2;
	double gCoin;
};


struct PMSG_ANS_STORAGEINFO
{
	PBMSG_HEAD2 h;
	int itemGuid;
	int unk1;
	int unk2_type;
	int groupId;
	int itemId;
	double value;
	BYTE type;
};

struct PMSG_ANS_GIFTINFO
{
	PBMSG_HEAD2 h;
	int itemGuid;
	int unk1;
	int unk2_type;
	int groupId;
	int itemId;
	double value;
	BYTE type;

	char szUser[11];
	char szText[200];
};

#pragma pack()

void CCashShop::GCCashPoint(LPOBJ lpObj)
{
	if ( !gObjIsAccontConnect(lpObj->m_Index, lpObj->AccountID))
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 175)), lpObj->AccountID, lpObj->m_Index);
		return;
	}

	PMSG_ANS_CASHPOINT pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xD2, 0x01, sizeof(PMSG_ANS_CASHPOINT));

	pMsg.wCoin = lpObj->m_wcCashPoint;
	pMsg.pCoin = lpObj->m_wpCashPoint;
	pMsg.gCoin = lpObj->m_wgCashPoint;

	DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
}

BOOL CCashShop::CGReqInfo(LPBYTE aRecv,int aIndex)
{
	PMSG_CASHSHOP_REQINFO* lpMsg = (PMSG_CASHSHOP_REQINFO*)aRecv;

	LPOBJ lpObj = &gObj[aIndex];
	DWORD dwUserGuid = 0;
	DWORD dwReqPage = lpMsg->Id;
	DWORD dwReqType = lpMsg->Type;
	
	if ( g_bConnectShopServer == TRUE && g_bShopServerConnectState == FALSE )
	{
		return FALSE;
	}

	if ( !gObjIsAccontConnect(lpObj->m_Index, lpObj->AccountID))
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 175)), lpObj->AccountID, lpObj->m_Index);
		return FALSE;
	}

	if ( lpObj->Connected !=PLAYER_PLAYING && lpObj->Type != OBJ_USER )
		return FALSE;

	dwUserGuid = lpObj->DBNumber;

	if( dwReqType == 0x53 )
	{
		GSReqCashItemList(dwUserGuid,dwReqPage);
	}
	else if( dwReqType == 0x47 )
	{
		GSReqCashGiftList(dwUserGuid,dwReqPage);
	}
	return TRUE;

	/*PMSG_CASHSHOP_REQINFO* lpMsg = (PMSG_CASHSHOP_REQINFO*)aRecv;

	PMSG_CASHSHOP_ANSINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xD2,0x06,sizeof(pMsg));

	pMsg.Info1 = 0;
	pMsg.Info2 = 0;
	pMsg.Info3 = 0;
	pMsg.Info4 = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);*/

}

void CCashShop::GCScriptVersionSend(int aIndex)
{
	PMSG_CASHSHOP_VERSION pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xD2,0x0C,sizeof(pMsg));

	pMsg.ver.Ver1 = m_ScriptVer.Ver1;
	pMsg.ver.Ver2 = m_ScriptVer.Ver2;
	pMsg.ver.Ver3 = m_ScriptVer.Ver3;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}


void CCashShop::GCBannerVersionSend(int aIndex)
{
	PMSG_CASHSHOP_VERSION pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xD2,0x15,sizeof(pMsg));

	pMsg.ver.Ver1 = m_BannerVer.Ver1;
	pMsg.ver.Ver2 = m_BannerVer.Ver2;
	pMsg.ver.Ver3 = m_BannerVer.Ver3;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void CCashShop::GCAnsStorageInfo(LPOBJ lpObj,protocol::MSG_STOG_ITEM_LIST_ANS_EX * lpMsg)
{
	if( !gObjIsConnected(lpObj->m_Index) )
		return;

	if( lpObj->Type != OBJ_USER )
		return;

	PMSG_CASHSHOP_ANSINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xD2,0x06,sizeof(pMsg));

	pMsg.Info1 = 1;
	pMsg.Info2 = 1;
	pMsg.Info3 = (WORD)lpMsg->dwPageId;
	pMsg.Info4 = (WORD)lpMsg->dwPageId;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

	PMSG_ANS_STORAGEINFO pItem;
	PHeadSubSetB((LPBYTE)&pItem,0xD2,0x0D,sizeof(pItem));

	for(DWORD i = 0; i < lpMsg->dwItemCount; i++)
	{
		pItem.itemGuid = lpMsg->itemList[i].dwGuid;
		pItem.unk1 = lpMsg->itemList[i].dwUnk1;
		pItem.unk2_type = lpMsg->itemList[i].dwUnk2;
		pItem.groupId = lpMsg->itemList[i].dwGroupIndex;
		pItem.itemId = lpMsg->itemList[i].dwProductIndex;
		pItem.value = lpMsg->itemList[i].Durability;
		pItem.type = (BYTE)lpMsg->itemList[i].dwType;

		DataSend(lpObj->m_Index,(LPBYTE)&pItem,pItem.h.size);
	}
}

void CCashShop::GCAnsGiftInfo(LPOBJ lpObj,protocol::MSG_STOG_GIFT_LIST_ANS_EX * lpMsg)
{
	if( !gObjIsConnected(lpObj->m_Index) )
		return;

	if( lpObj->Type != OBJ_USER )
		return;

	PMSG_CASHSHOP_ANSINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xD2,0x06,sizeof(pMsg));

	pMsg.Info1 = (WORD)lpMsg->dwItemCount;
	pMsg.Info2 = (WORD)lpMsg->dwItemCount;
	pMsg.Info3 = (WORD)lpMsg->dwPageId;
	pMsg.Info4 = (WORD)lpMsg->dwPageId;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

	PMSG_ANS_GIFTINFO pItem;
	PHeadSubSetB((LPBYTE)&pItem,0xD2,0x0E,sizeof(pItem));

	for(DWORD i = 0; i < lpMsg->dwItemCount; i++)
	{
		pItem.itemGuid = lpMsg->itemList[i].dwGuid;
		pItem.unk1 = lpMsg->itemList[i].dwUnk1;
		pItem.unk2_type = lpMsg->itemList[i].dwUnk2;
		pItem.groupId = lpMsg->itemList[i].dwGroupIndex;
		pItem.itemId = lpMsg->itemList[i].dwProductIndex;
		pItem.value = lpMsg->itemList[i].Durability;
		pItem.type = (BYTE)lpMsg->itemList[i].dwType;

		strcpy(pItem.szUser,lpMsg->itemList[i].szFrom);
		strcpy(pItem.szText,lpMsg->itemList[i].szText);

		DataSend(lpObj->m_Index,(LPBYTE)&pItem,pItem.h.size);
	}
}

void CCashShop::GCSendAllItemInfo(LPOBJ lpObj)
{
	for(int n = 0; n < INVENTORY_SIZE; n++)
	{
		if( !lpObj->pInventory[n].IsItem() )
			continue;

		CItem* lpItem = &lpObj->pInventory[n];

		if( lpItem->m_bPeriodItem && !lpItem->m_bExpired )
		{
			GCSendItemInfo(lpObj->m_Index,lpItem->m_Type,n,lpItem->m_Number);
		}
	}
}

void CCashShop::GCSendItemInfo(int aIndex,int ItemID, int ItemPos, DWORD ItemTime)
{
	PMSG_CASHITEM_STATUS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg,0xD2,0x12,sizeof(pMsg));

	pMsg.ItemID = ItemID;
	pMsg.ItemPos = ItemPos;
	pMsg.ItemTime = ItemTime;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void CCashShop::CGCashItemBuy(LPOBJ lpObj,PMSG_REQ_CASHITEM_BUY* lpMsg)
{
	BYTE btResult = 0;
	int iItemCode = (WORD)lpMsg->itemId;

	if ( g_bConnectShopServer == TRUE && g_bShopServerConnectState == FALSE )
	{
		btResult = -2;
	}

	if ( !gObjIsAccontConnect(lpObj->m_Index, lpObj->AccountID))
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 175)), lpObj->AccountID, lpObj->m_Index);
		btResult = -2;
	}

	if ( lpObj->Connected !=PLAYER_PLAYING && lpObj->Type != OBJ_USER )
	{
		btResult = -2;
	}

	if ( iItemCode < 0 || iItemCode >= ITEMGET(16,0) )
	{
		btResult = -2;
	}
	else
	{
		LogAddTD("[CashShop][Buy Request] User(ID:%s, Name:%s) Item(Name:%s,Guid:%d,Category:%d) Result:%d",
			lpObj->AccountID, lpObj->Name, ItemAttribute[iItemCode].Name, lpMsg->itemGuid, lpMsg->itemCategory, btResult);
	}


	if ( btResult == 0 )
	{
		GSReqBuyCashItem(gGameServerCode, lpObj, lpMsg );
	}
	else
	{
		GCCashItemBuyResult(lpObj, btResult);
	}
}

void CCashShop::GCCashItemBuyResult(LPOBJ lpObj, BYTE btResult)
{
	PMSG_ANS_CASHITEM_BUY pMsg;

	pMsg.btResult = btResult;
	PHeadSubSetB((LPBYTE)&pMsg, 0xD2, 0x03, sizeof(PMSG_ANS_CASHITEM_BUY));

	DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void CCashShop::CGCashGiftSend(LPOBJ lpObj,PMSG_REQ_CASHITEM_GIFT* lpMsg)
{
	BYTE btResult = 0;
	int iItemCode = lpMsg->wUnk1;

	if ( g_bConnectShopServer == TRUE && g_bShopServerConnectState == FALSE )
	{
		btResult = -2;
	}

	if ( !gObjIsAccontConnect(lpObj->m_Index, lpObj->AccountID))
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 175)), lpObj->AccountID, lpObj->m_Index);
		btResult = -2;
	}

	if ( lpObj->Connected !=PLAYER_PLAYING && lpObj->Type != OBJ_USER )
	{
		btResult = -2;
	}

	LogAddTD("[CashShop][Gift Request] User(ID:%s, Name:%s) Item(Name:%s,Guid:%d,Category:%d) Result:%d",
		lpObj->AccountID, lpObj->Name, ItemAttribute[iItemCode].Name, lpMsg->itemGuid, lpMsg->itemCategory, btResult);

	if ( btResult == 0 )
	{
		GSReqGiftCashItem(gGameServerCode, lpObj, lpMsg );
	}
	else
	{
		GCCashGiftSendResult(lpObj, btResult);
	}
}

void CCashShop::GCCashGiftSendResult(LPOBJ lpObj, BYTE btResult)
{
	PMSG_ANS_CASHITEM_GIFT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xD2,0x04,sizeof(pMsg));

	pMsg.btResult = btResult;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
}

void CCashShop::CGReqUseItem(LPOBJ lpObj, PSMG_REQ_CASHITEM_USE* lpMsg)
{
	BYTE btResult = 0;
	int iItemCode = lpMsg->ItemID;

	if ( g_bConnectShopServer == TRUE && g_bShopServerConnectState == FALSE )
	{
		btResult = -2;
	}

	if ( !gObjIsAccontConnect(lpObj->m_Index, lpObj->AccountID))
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 175)), lpObj->AccountID, lpObj->m_Index);
		btResult = -2;
	}

	if ( lpObj->Connected !=PLAYER_PLAYING && lpObj->Type != OBJ_USER )
	{
		btResult = -2;
	}

	if ( CheckPeriodItem(iItemCode) == TRUE )
	{
		g_CashItemPeriodSystem.ClearPeriodItem(lpObj,iItemCode);
	}

	LogAddTD("[CashShop][Use Request] User(ID:%s, Name:%s) Item(Name:%s,Guid:%d) Result:%d",
		lpObj->AccountID, lpObj->Name, ItemAttribute[iItemCode].Name, lpMsg->itemGuid,  btResult);


	if ( btResult == 0 )
	{
		GSReqUseCashItem(lpObj->DBNumber,lpMsg->itemGuid,lpMsg->itemUnk,iItemCode,lpMsg->btType);
	}
	else
	{
		GCCashItemUseResult(lpObj, btResult);
	}
}

void CCashShop::GCCashItemUseResult(LPOBJ lpObj, BYTE btResult)
{
	PMSG_ANS_CASHITEM_USE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xD2,0x0B,sizeof(pMsg));

	pMsg.btResult = btResult;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
}

BOOL CCashShop::GiveBuyItemToInventory(LPOBJ lpObj, int iItemCode,protocol::useInfoEx* pItemInfo)
{
	BOOL bEmptySpace = FALSE;
	CItem pItem;
	BYTE ExOption[MAX_EXOPTION_SIZE];

	pItem.Convert(iItemCode, pItemInfo->btOption1, pItemInfo->btOption2, pItemInfo->btOption3,
		pItemInfo->btExcOp, pItemInfo->btAncOp, 0, 0, -1, CURRENT_DB_VERSION);

	pItem.m_Level = pItemInfo->btLevel;

	if( pItemInfo->dwDurability != 0 )
	{
		pItem.m_Durability = (float)pItemInfo->dwDurability;
	}
	else
	{
		pItem.m_Durability = (float)ItemGetDurability(iItemCode,pItem.m_Level,pItem.IsExtItem(),pItem.IsSetItem());
	}

	if( pItemInfo->dwUseTime != 0 )
	{
		time_t pTime;
		pTime = time(NULL);
		pItem.m_Number = (DWORD)pTime + pItemInfo->dwUseTime*60;
		pItem.m_bPeriodItem = TRUE;
		pItem.m_bExpired = FALSE;
	}

	BYTE btItemPos = gObjOnlyInventoryInsertItem(lpObj->m_Index, pItem);

	ItemIsBufExOption(ExOption, &pItem);

	if( btItemPos == (BYTE)-1 )
	{
		return FALSE;
	}

	GCInventoryItemOneSend(lpObj->m_Index, btItemPos);

	if( pItem.m_bPeriodItem )
	{
		GCSendItemInfo(lpObj->m_Index,pItem.m_Type,btItemPos,pItem.m_Number);
	}

	LogAddTD("[CashShop][Buy Item Create in Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,Skill:%d,Luck:%d,Add:%d,Ex(%d:%d:%d:%d:%d:%d))",
		lpObj->AccountID, lpObj->Name, ItemAttribute[iItemCode].Name, iItemCode, 
		pItem.m_Option1, pItem.m_Option2, pItem.m_Option3, ExOption[0], ExOption[1], ExOption[2],
		ExOption[3], ExOption[4], ExOption[5]);

	return TRUE;
}


BOOL CCashShop::CheckInventoryEmptySpace(LPOBJ lpObj, int iItemCode)
{
	int h = 0;
	int w = 0;
	unsigned char blank = 0;
	int iItemHeight = 0;
	int iItemWidth = 0;

	ItemGetSize(iItemCode,iItemWidth,iItemHeight);

	for ( h = 0; h < 8 + (lpObj->pInventoryExtend * 4); h++ )
	{
		for ( w=0;w<8;w++)
		{
			if ( *(BYTE *)(lpObj->pInventoryMap + h*8 + w ) == 0xFF )
			{
				blank = gObjOnlyInventoryRectCheck(lpObj->m_Index, w, h, iItemWidth, iItemHeight);

				if ( blank == 0xFE )
					return FALSE;

				if ( blank != 0xFF )
					return TRUE;
			}
		}
	}

	return FALSE;
}

void CCashShop::ItemCheckWorker()
{
	time_t curTime;
	curTime = time(NULL);

	int bCalCharacter;

	for(int n = OBJ_STARTUSERINDEX; n < OBJMAX; n++)
	{
		if( !gObjIsConnected(n) )
			continue;

		LPOBJ lpObj = &gObj[n];

		bCalCharacter = FALSE;

		for(int i = 0; i < INVENTORY_SIZE; i++)
		{
			if( !lpObj->pInventory[i].IsItem() || !lpObj->pInventory[i].m_bPeriodItem )
				continue;

			if( lpObj->pInventory[i].m_bExpired )
				continue;

			if( lpObj->pInventory[i].m_Number < curTime )
			{
				lpObj->pInventory[i].m_bExpired = TRUE;

				if( i == 10 || i == 11 )
				{
					if( IsTransformRing(lpObj->pInventory[i].m_Type) )
					{
						gObjUseSkill.SkillChangeUse(lpObj->m_Index);
						LogAddTD(lMsg.Get(544),lpObj->AccountID,lpObj->Name,lpObj->pInventory[i].GetName(),lpObj->pInventory[i].m_Level,0);
					}
				}

				lpObj->pInventory[i].Clear();
				GCInventoryItemDeleteSend(lpObj->m_Index,i,0);
				bCalCharacter = TRUE;
			}
		}

		if( bCalCharacter == TRUE )
		{
			gObjCalCharacter(lpObj->m_Index);
		}
	}
}

BOOL CCashShop::IsGetAmountFromShopItem(int iItemCode)
{
	switch ( iItemCode )
	{
	case ITEMGET(14,71):
	case ITEMGET(14,70):

	case ITEMGET(13,58):
	case ITEMGET(13,57):
	case ITEMGET(13,56):
	case ITEMGET(13,55):
	case ITEMGET(13,54):
	case ITEMGET(13,53):

	case ITEMGET(13,46):
	case ITEMGET(13,47):
	case ITEMGET(13,48):
		
	case ITEMGET(14,53):

	case ITEMGET(14,78):
	case ITEMGET(14,79):
	case ITEMGET(14,80):
	case ITEMGET(14,81):
	case ITEMGET(14,82):
	case ITEMGET(14,94):
		return TRUE;
	}
return FALSE;
}

BOOL CCashShop::CheckPeriodItem(int iItemCode)
{

	switch( iItemCode )
	{
	case ITEMGET(13,43):
	case ITEMGET(13,44):
	case ITEMGET(13,45):
	case ITEMGET(13,59):
	case ITEMGET(13,62):
	case ITEMGET(13,63):
	case ITEMGET(13,93):
	case ITEMGET(13,94):
	case ITEMGET(13,104):
	case ITEMGET(13,105):
	case ITEMGET(14,72):
	case ITEMGET(14,73):
	case ITEMGET(14,74):
	case ITEMGET(14,75):
	case ITEMGET(14,76):
	case ITEMGET(14,77):
	case ITEMGET(14,97):
	case ITEMGET(14,98):
		return TRUE;
	}
	return FALSE;
}

// GameServer -> CashShopServer

void CCashShop::GSReqCashPoint(DWORD dwUserGuid)
{
	protocol::MSG_GTOS_USER_CASH_REQ_EX pMsg;

	pMsg.dwUserGuid = dwUserGuid;
	g_ShopServerClient.DataSend((PCHAR)&pMsg, sizeof(protocol::MSG_GTOS_USER_CASH_REQ_EX));
}

void CCashShop::GSReqCashItemList(DWORD dwUserGuid,DWORD dwPageId)
{
	protocol::MSG_GTOS_ITEM_LIST_REQ_EX pMsg;

	pMsg.dwUserGuid = dwUserGuid;
	pMsg.dwPageId = dwPageId;

	g_ShopServerClient.DataSend((PCHAR)&pMsg, sizeof(protocol::MSG_GTOS_ITEM_LIST_REQ_EX));
}

void CCashShop::GSReqCashGiftList(DWORD dwUserGuid,DWORD dwPageId)
{
	protocol::MSG_GTOS_GIFT_LIST_REQ_EX pMsg;

	pMsg.dwUserGuid = dwUserGuid;
	pMsg.dwPageId = dwPageId;

	g_ShopServerClient.DataSend((PCHAR)&pMsg, sizeof(protocol::MSG_GTOS_GIFT_LIST_REQ_EX));
}

void CCashShop::GSReqBuyCashItem(DWORD dwServerGuid, LPOBJ lpObj,PMSG_REQ_CASHITEM_BUY* lpMsg )
{
	protocol::MSG_GTOS_BUY_ITEM_REQ_EX pMsg;

	pMsg.dwServerGuid = dwServerGuid;
	pMsg.dwUserGuid = lpObj->DBNumber;
	pMsg.dwCharGuid = lpObj->m_Index;
	pMsg.dwPackageCategory = lpMsg->itemCategory;
	pMsg.dwPackageId = lpMsg->itemGuid;
	pMsg.dwProductId = lpMsg->productId;
	pMsg.ItemID = lpMsg->itemId;

	memset(pMsg.szCharName,0,sizeof(pMsg.szCharName));
	strcpy(pMsg.szCharName,lpObj->Name);

	g_ShopServerClient.DataSend((PCHAR)&pMsg, sizeof(protocol::MSG_GTOS_BUY_ITEM_REQ_EX));
}

void CCashShop::GSReqGiftCashItem(DWORD dwServerGuid,LPOBJ lpObj,PMSG_REQ_CASHITEM_GIFT* lpMsg )
{
	protocol::MSG_GTOS_GIFT_ITEM_REQ_EX pMsg;

	pMsg.dwServerGuid = dwServerGuid;
	pMsg.dwUserGuid = lpObj->DBNumber;
	pMsg.dwCharGuid = lpObj->m_Index;
	pMsg.dwPackageCategory = lpMsg->itemCategory;
	pMsg.dwPackageId = lpMsg->itemGuid;
	pMsg.dwProductId = lpMsg->productId;
	pMsg.ItemID = lpMsg->itemId;

	memset(pMsg.szCharName,0,sizeof(pMsg.szCharName));
	strcpy(pMsg.szCharName,lpObj->Name);

	memset(pMsg.szToName,0,sizeof(pMsg.szToName));
	memset(pMsg.szText,0,sizeof(pMsg.szText));

	strcpy(pMsg.szToName,lpMsg->szName);
	strcpy(pMsg.szText,lpMsg->szText);

	g_ShopServerClient.DataSend((PCHAR)&pMsg, sizeof(protocol::MSG_GTOS_GIFT_ITEM_REQ_EX));
}

void CCashShop::GSReqUseCashItem(DWORD dwUserGuid,DWORD itemGuid, DWORD dwUnkInfo, DWORD itemID, DWORD itemType)
{
	protocol::MSG_GTOS_ITEM_USE_REQ_EX pMsg;

	pMsg.dwUserGuid = dwUserGuid;
	pMsg.dwItemGuid = itemGuid;
	pMsg.dwUnkInfo = dwUnkInfo;
	pMsg.dwItemId = itemID;
	pMsg.dwItemType = itemType;

	g_ShopServerClient.DataSend((PCHAR)&pMsg, sizeof(protocol::MSG_GTOS_ITEM_USE_REQ_EX));
}

void CCashShop::GSNtfUseItem(DWORD dwUserGuid,DWORD dwItemGuid)
{
	protocol::MSG_GTOS_USE_ITEM_NTF_EX pMsg;

	pMsg.dwUserGuid = dwUserGuid;
	pMsg.dwItemGuid = dwItemGuid;

	g_ShopServerClient.DataSend((PCHAR)&pMsg,sizeof(protocol::MSG_GTOS_USE_ITEM_NTF_EX));
}

// CashShopServer -> GameServer

void SGAnsCashPoint(protocol::MSG_STOG_USER_CASH_ANS_EX* aRecv)
{
	LPOBJ lpObj = NULL;

	lpObj = g_CashShop.SearchUser(aRecv->dwUserGuid);

	if ( lpObj == NULL )
		return;

	lpObj->m_wcCashPoint = aRecv->dwUserCash[0];
	lpObj->m_wpCashPoint = aRecv->dwUserCash[1];
	lpObj->m_wgCashPoint = aRecv->dwUserCash[2];

	g_CashShop.GCCashPoint(lpObj);
}

void SGAnsCashItemList(protocol::MSG_STOG_ITEM_LIST_ANS_EX *aRecv)
{
	LPOBJ lpObj = NULL;

	lpObj = g_CashShop.SearchUser(aRecv->dwUserGuid);

	if ( lpObj == NULL )
		return;

	g_CashShop.GCAnsStorageInfo(lpObj,aRecv);
}

void SGAnsUseCashItem(protocol::MSG_STOG_ITEM_USE_ANS_EX *aRecv)
{
	LPOBJ lpObj = NULL;
	lpObj = g_CashShop.SearchUser(aRecv->dwUserGuid);

	int iResult = TRUE;
	int iItemCode = 0;
	int iItemUsePeriod = 0;

	//protocol::MSG_GTOS_BUY_ITEM_NTF pMsg;

	if ( lpObj == NULL )
		return;

	if( aRecv->dwResult != 0 )
	{
		g_CashShop.GCCashItemUseResult(lpObj,(BYTE)aRecv->dwResult);
		return;
	}

	iItemCode = aRecv->itemInfo.dwItemId;

	if ( g_CashShop.CheckPeriodItem(iItemCode) == TRUE )
	{
		g_CashItemPeriodSystem.GDReqPeriodItemInsert(lpObj, iItemCode, aRecv->itemInfo.dwUseTime);
	}
	else if(iItemCode == ITEMGET(14,78)
		|| iItemCode == ITEMGET(14,79) 
		|| iItemCode == ITEMGET(14,80)
		|| iItemCode == ITEMGET(14,81)
		|| iItemCode == ITEMGET(14,82))
	{
		if(gObjSearchItemPlus(lpObj,iItemCode,0,aRecv->itemInfo.dwDurability,3) == FALSE)
		{
			if(g_CashShop.GiveBuyItemToInventory(lpObj,iItemCode,&aRecv->itemInfo)== FALSE)
			{
				iResult = 0;
			}
		}
	}
	else if(iItemCode == ITEMGET(14,70)
			|| iItemCode == ITEMGET(14,71)
			|| iItemCode == ITEMGET(14,94) )
	{
		if(gObjSearchItemPlus(lpObj,iItemCode,0,aRecv->itemInfo.dwDurability,50) == FALSE)
		{
			if(g_CashShop.GiveBuyItemToInventory(lpObj,iItemCode,&aRecv->itemInfo) == FALSE)
			{
				iResult = 0;
			}
		}
	}
	else if(iItemCode == ITEMGET(14,91) )
	{
		DGSetSummonerCreate(lpObj,lpObj->m_Index);
	}
	else if ( g_CashShop.GiveBuyItemToInventory(lpObj, iItemCode,&aRecv->itemInfo) == FALSE )
	{
		iResult = 0;
	}


	switch( iResult )
	{
	case 0: iResult = 15;	break;
	case 1: g_CashShop.GSNtfUseItem(lpObj->DBNumber,aRecv->itemInfo.dwItemGuid); iResult = 0; break;
	}

	LogAddTD("[CashShop][Use Answer] User(ID:%s, Name:%s) Result:%d",
		lpObj->AccountID, lpObj->Name, iResult);

	g_CashShop.GCCashItemUseResult(lpObj,iResult);
}



void SGAnsBuyCashItem(protocol::MSG_STOG_BUY_ITEM_ANS_EX *aRecv)
{
	LPOBJ lpObj;

	lpObj = g_CashShop.SearchUser(aRecv->dwUserGuid);

	if ( lpObj == NULL )
		return;

	if ( aRecv->dwResult == 0 )
	{
		lpObj->m_wcCashPoint = aRecv->dwUserCash[0];
		lpObj->m_wpCashPoint = aRecv->dwUserCash[1];
		lpObj->m_wgCashPoint = aRecv->dwUserCash[2];
	}

	g_CashShop.GCCashItemBuyResult(lpObj,(BYTE)aRecv->dwResult);
}

void SGAnsGiftCashItem(protocol::MSG_STOG_GIFT_ITEM_ANS_EX *aRecv)
{
	LPOBJ lpObj;

	lpObj = g_CashShop.SearchUser(aRecv->dwUserGuid);

	if ( lpObj == NULL )
		return;

	if ( aRecv->dwResult == 0 )
	{
		lpObj->m_wcCashPoint = aRecv->dwUserCash[0];
		lpObj->m_wpCashPoint = aRecv->dwUserCash[1];
		lpObj->m_wgCashPoint = aRecv->dwUserCash[2];
	}

	g_CashShop.GCCashGiftSendResult(lpObj,(BYTE)aRecv->dwResult);
}

void SGAnsCashGiftList(protocol::MSG_STOG_GIFT_LIST_ANS_EX *aRecv)
{
	LPOBJ lpObj = NULL;

	lpObj = g_CashShop.SearchUser(aRecv->dwUserGuid);

	if ( lpObj == NULL )
		return;

	g_CashShop.GCAnsGiftInfo(lpObj,aRecv);
}

BOOL IsCashItem(int iItemCode) {
	switch (iItemCode) {
		case ITEMGET(13, 54):
		case ITEMGET(13, 55):
		case ITEMGET(13, 56):
		case ITEMGET(13, 57):
		case ITEMGET(13, 58):
		case ITEMGET(13, 59):
		case ITEMGET(13, 60):
		case ITEMGET(13, 61):
		case ITEMGET(13, 43):
		case ITEMGET(13, 44):
		case ITEMGET(13, 45):
		case ITEMGET(13, 46):
		case ITEMGET(13, 47):
		case ITEMGET(13, 48):
		case ITEMGET(13, 93):
		case ITEMGET(13, 94):
		case ITEMGET(13, 69):
		case ITEMGET(13, 70):
		case ITEMGET(13, 76):
		case ITEMGET(13, 77):
		case ITEMGET(13, 78):
		case ITEMGET(13, 163):
		case ITEMGET(13, 164):
		case ITEMGET(13, 165):
		case ITEMGET(13, 166):
		case ITEMGET(14, 60):
		case ITEMGET(14, 54):
		case ITEMGET(14, 53):
		case ITEMGET(13, 62):
		case ITEMGET(13, 63):
		case ITEMGET(13, 64):
		case ITEMGET(13, 65):
		case ITEMGET(13, 106):
		case ITEMGET(13, 122):
		case ITEMGET(13, 123):
		case ITEMGET(14, 58):
		case ITEMGET(14, 59):
		case ITEMGET(14, 61): 
		case ITEMGET(14, 62): 
		case ITEMGET(14, 70):
		case ITEMGET(14, 71):
		case ITEMGET(14, 72):
		case ITEMGET(14, 73):
		case ITEMGET(14, 74):
		case ITEMGET(14, 75):
		case ITEMGET(14, 76):
		case ITEMGET(14, 77):
		case ITEMGET(14, 78):
		case ITEMGET(14, 79):
		case ITEMGET(14, 80):
		case ITEMGET(14, 81):
		case ITEMGET(14, 82): 
		case ITEMGET(14, 83):
		case ITEMGET(14, 94):
		case ITEMGET(14, 96):
		case ITEMGET(14, 97):
		case ITEMGET(14, 98):
		case ITEMGET(14, 162):
		case ITEMGET(14, 163):
			return TRUE;
	}
	return FALSE;
}

BOOL IsCashSpecialItem(int ItemType)
{
	switch( ItemType )
	{
	case ITEMGET(13,64):
	case ITEMGET(13,65):
	case ITEMGET(14,54):
	case ITEMGET(14,96):
	case ITEMGET(13,123):
		return true;
	default:
		return false;
	}
}
