// CashShop.h: interface for the CCashShop class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASHSHOP_H__B57A9271_3D21_40E0_B132_82F87B122CB4__INCLUDED_)
#define AFX_CASHSHOP_H__B57A9271_3D21_40E0_B132_82F87B122CB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "PCSProtocol.h"
#include "..\common\zzzitem.h"
#include "protocol.h"
#include "wsShopServerCli.h"

struct _VER_STRUCT
{
	WORD Ver1;
	WORD Ver2;
	WORD Ver3;
};

struct PMSG_CASHSHOP_VERSION
{
	PBMSG_HEAD2 h;
	_VER_STRUCT ver;
};

struct PMSG_REQ_CASHSHOPOPEN
{
	PBMSG_HEAD2 head;
	BYTE btShopOpenType;
};

struct PMSG_ANS_CASHSHOPOPEN
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_CASHSHOP_REQINFO
{
	PBMSG_HEAD2 h;
	int Id;
	BYTE Type;
};

struct PMSG_CASHSHOP_ANSINFO
{
	PBMSG_HEAD2 h;
	WORD Info1;
	WORD Info2;
	WORD Info3;
	WORD Info4;
};

struct PMSG_REQ_CASHITEM_BUY
{
	PBMSG_HEAD2 h;
	int itemGuid;
	int itemCategory;
	int productId;
	int itemId;
	WORD wUnk1;
};

struct PMSG_ANS_CASHITEM_BUY
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PSMG_REQ_CASHITEM_USE
{
	PBMSG_HEAD2 h;
	int itemGuid;
	int itemUnk;
	short ItemID;
	BYTE btType;
};

struct PMSG_ANS_CASHITEM_USE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_CASHITEM_STATUS
{
	PBMSG_HEAD2 h;
	short ItemID;
	short ItemPos;
	DWORD ItemTime;
};

#pragma pack(1)

struct PMSG_REQ_CASHITEM_GIFT
{
	PBMSG_HEAD2 h;
	int itemGuid;
	int itemCategory;
	int productId;
	int itemId;
	WORD wUnk1;
	char szName[11];
	char szText[200];
};
#pragma pack()

struct PMSG_ANS_CASHITEM_GIFT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct CASHSHOP_ITEM_STATUS
{
	BYTE btItemType;	// 0
	WORD wItemIndex;	// 1
	BYTE btItemLevel;	// 3
	BYTE btDurability;	// 4
	BYTE btSkillOption;	// 5
	BYTE btLuckOption;	// 6
	BYTE btAddOption;	// 7
	BYTE btExOption;	// 8
};

class CCashShop
{
public:
	CCashShop();
	virtual ~CCashShop();

	void Initialize();
	void LoadShopOption(LPSTR pchFilename);

	void CheckShopServerConnectState();
	BOOL ConnectShopServer(LPSTR pchIpAddress, int iPortNumber);
	BOOL ReConnectShopServer();
	void CashShopOptioNReload();

	BOOL AddUser(LPOBJ lpObj);
	BOOL DeleteUser(LPOBJ lpObj);
	LPOBJ SearchUser(int iUserGuid);

	BOOL CGCashShopOpen(LPOBJ lpObj, PMSG_REQ_CASHSHOPOPEN* lpMsg);
	BOOL CGCashPoint(LPOBJ lpObj);
	BOOL CGReqInfo(LPBYTE aRecv,int aIndex);

	void GCCashPoint(LPOBJ lpObj);
	void GCCashItemBuyResult(LPOBJ lpObj, BYTE btResult);
	void GCCashItemUseResult(LPOBJ lpObj, BYTE btResult);
	void GCCashGiftSendResult(LPOBJ lpObj, BYTE btResult);
	void GCAnsStorageInfo(LPOBJ lpObj,protocol::MSG_STOG_ITEM_LIST_ANS_EX * lpMsg);
	void GCAnsGiftInfo(LPOBJ lpObj,protocol::MSG_STOG_GIFT_LIST_ANS_EX * lpMsg);
	void GCSendAllItemInfo(LPOBJ lpObj);
	void GCSendItemInfo(int aIndex,int ItemID, int ItemPos, DWORD ItemTime);
	void CGCashItemBuy(LPOBJ lpObj,  PMSG_REQ_CASHITEM_BUY* lpMsg);
	void CGCashGiftSend(LPOBJ lpObj,PMSG_REQ_CASHITEM_GIFT* lpMsg);
	void CGReqUseItem(LPOBJ lpObj, PSMG_REQ_CASHITEM_USE* lpMsg);

	void GSReqCashItemList(DWORD dwUserGuid,DWORD dwPageId);
	void GSReqCashGiftList(DWORD dwUserGuid,DWORD dwPageId);
	void GSReqCashPoint(DWORD dwUserGuid);
	void GSReqBuyCashItem(DWORD dwServerGuid, LPOBJ lpObj,PMSG_REQ_CASHITEM_BUY* lpMsg );
	void GSReqGiftCashItem(DWORD dwServerGuid, LPOBJ lpObj, PMSG_REQ_CASHITEM_GIFT* lpMsg );
	void GSReqUseCashItem(DWORD dwUserGuid,DWORD itemGuid, DWORD dwUnkInfo, DWORD itemID, DWORD itemType);
	void GSNtfUseItem(DWORD dwUserGuid,DWORD dwItemGuid);

	void GCScriptVersionSend(int aIndex);
	void GCBannerVersionSend(int aIndex);

	BOOL CheckPeriodItem(int iItemCode);
	BOOL IsGetAmountFromShopItem(int iItemCode);
	BOOL GiveBuyItemToInventory(LPOBJ lpObj,int iItemCode,protocol::useInfoEx* pItem);
	BOOL CheckInventoryEmptySpace(LPOBJ lpObj, int iItemCode);
	void ItemCheckWorker();

	static long  CCashShop::ParentWndProc(HWND hWnd, unsigned int iMessage, unsigned int wParam, long lParam);
	static WNDPROC m_lpOldProc;

private:
	BOOL bCashItemListReload;
	_VER_STRUCT m_ScriptVer;
	_VER_STRUCT m_BannerVer;
	std::map<int,LPOBJ> MapUserObject;
	DWORD dwCheckShopServerConnectStatePeriod;
};

extern BOOL g_bUseMoveMapBound;
extern CCashShop g_CashShop;
extern wsShopServerCli g_ShopServerClient;

void ShopServerProtocolCore(DWORD protoNum, LPBYTE aRecv, int aLen);
void SGAnsCashPoint(protocol::MSG_STOG_USER_CASH_ANS_EX *aRecv);
void SGAnsCashItemList(protocol::MSG_STOG_ITEM_LIST_ANS_EX *aRecv);
void SGAnsUseCashItem(protocol::MSG_STOG_ITEM_USE_ANS_EX *aRecv);
void SGAnsBuyCashItem(protocol::MSG_STOG_BUY_ITEM_ANS_EX *aRecv);
void SGAnsCashGiftList(protocol::MSG_STOG_GIFT_LIST_ANS_EX *aRecv);
void SGAnsGiftCashItem(protocol::MSG_STOG_GIFT_ITEM_ANS_EX *aRecv);

BOOL IsCashItem(int iItemCode);
BOOL IsCashSpecialItem(int ItemType);


#endif // !defined(AFX_CASHSHOP_H__B57A9271_3D21_40E0_B132_82F87B122CB4__INCLUDED_)
