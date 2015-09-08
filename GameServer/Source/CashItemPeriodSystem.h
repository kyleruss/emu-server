// CashItemPeriodSystem.h: interface for the CCashItemPeriodSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASHITEMPERIODSYSTEM_H__11ADB3C6_2988_4060_913A_824DCFA381E8__INCLUDED_)
#define AFX_CASHITEMPERIODSYSTEM_H__11ADB3C6_2988_4060_913A_824DCFA381E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "user.h"

#define MAX_ITEM_PERIOD_INFO	OBJMAXUSER
#define SUBITEM_INFO_COUNT 32

struct PERIODITEM_LIST
{
	BYTE btUsedItem;
	int iItemCode;
	BYTE btItemOption;
	BYTE btItemEffectType1;
	BYTE btItemEffectType2;
	int iItemPeriodUsedTime;
	int iItemPeriodLeftTime;
};

struct PMSG_ANS_PERIODITEM_LIST
{
	PWMSG_HEAD2 head;	// 0
	int iUserIndex;	// 4
	int iUserGuid;	// 8
	BYTE btCount;	// C
	char btResult;	// D
	//int test;
	PERIODITEM_LIST Items[SUBITEM_INFO_COUNT];
};

struct PMSG_ANS_PERIODITEM_INSERT
{
	PBMSG_HEAD2 head;
	int iUserIndex;	// 4
	int iItemCode;	// 8
	int iItemPeriodDate;	// C
	BYTE btItemEffectType1;	// 10
	BYTE btItemEffectType2;	// 11
	BYTE btResult;	// 12
};


struct PMSG_ANS_PERIODITEM_INSERT_RESULT
{
	PBMSG_HEAD2 head;	// C1:D0:03
	BYTE btResult;	// 4
};

struct SUB_ITEMPERIOD_INFO
{
	BYTE btUsedItem;
	BYTE btItemOption;
	BYTE btEffectType1;
	BYTE btEffectType2;
	int iItemCode;
	DWORD iItemPeriodUsedTime;
	DWORD iItemPeriodLeftTime;
};

struct ITEMPERIOD_INFO
{
	BYTE btUsedInfo;	// 0
	BYTE btUsedCount;
	WORD iUserIndex;
	int iUserGuid;	// 8
	DWORD dwTickCount;
	char szName[11];

	SUB_ITEMPERIOD_INFO ItemPeriod[SUBITEM_INFO_COUNT];
};

struct PMSG_ANS_PERIODITEM_INQUIRY
{
	PBMSG_HEAD2 head;
	int iUserIndex;	// 4
	BYTE btResult;	// 8
};


struct PMSG_ANS_PERIODITEM_UPDATE
{
	PBMSG_HEAD2 head;
	int iUserIndex;	// 4
	int iItemCode;	// 8
	BYTE btResult;	// C
};


struct PMSG_ANS_PERIODITEM_DELETE
{
	PBMSG_HEAD2 head;
	int iUserIndex;	// 4
	int iUserGuid;	// 8
	BYTE btResult;	// C
};



class CCashItemPeriodSystem
{

public:

	CCashItemPeriodSystem();
	virtual ~CCashItemPeriodSystem();

	void Initialize();
	void GetUserPeriodItemList(LPOBJ lpObj);
	int SetPeriodItemEffect(LPOBJ lpObj, int iItemCode,BYTE btEffectType1, BYTE btEffectType2, DWORD dwItemPeriod);
	BYTE ClearPeriodItem(LPOBJ lpObj,int itemid);
	int ClearPeriodItemEffect(LPOBJ lpObj,int itemid);
	void GDReqPeriodItemList(LPOBJ lpObj);
	void GDReqPeriodItemInsert(LPOBJ lpObj, int iItemCode, int iItemEffectDate);
	void GDReqPeriodItemInquiry(LPOBJ lpObj);
	void GDReqPeriodItemUpdate(LPOBJ lpObj,int itemid);
	void DGAnsPeriodItemList(PMSG_ANS_PERIODITEM_LIST* aRecv);
	void DGAnsPeriodItemInsert(PMSG_ANS_PERIODITEM_INSERT* aRecv);
	void DGAnsPeriodItemInquiry(PMSG_ANS_PERIODITEM_INQUIRY* aRecv);
	void DGAnsPeriodItemUpdate(PMSG_ANS_PERIODITEM_UPDATE* aRecv);
	void GDReqPeriodItemDelete(LPOBJ lpObj, LPSTR pchCharacterName);
	void DGAnsPeriodItemDelete(PMSG_ANS_PERIODITEM_DELETE* aRecv);
	void GCSendPeriodItemInserResult(LPOBJ lpObj, BYTE btResult);
	int CalcItemUseLeftTime(LPSTR pchBuyDate);
	class CTime CalcItemUseEndTime(int iLeftTime);
	void ConvertStringToTime(const char * pchDate,class CTime* lpTime);
	void ConvertTimeToString(const class CTime* lpTime, LPSTR pchDate);
	int CheckMoveMapBoundItem(LPOBJ lpObj);
	ITEMPERIOD_INFO * GetPeriodInfo(LPOBJ lpObj);
	void SavePeriodInfo(LPOBJ lpObj);
	int _SetItemEffect(LPOBJ lpObj,int iItemCode,int iValidTime);
	int GetEffectNumber(int iItemCode);


	static DWORD WINAPI CCashItemPeriodSystem::PeriodCheckThread(void* lpPeriodInfo);

private:

	int iItemPeriodInfoCount;	// 4
	ITEMPERIOD_INFO ItemPeriodInfo[MAX_ITEM_PERIOD_INFO];	// 8
	HANDLE hThreadHandle;	// 130B8
};


extern CCashItemPeriodSystem g_CashItemPeriodSystem;

#endif // !defined(AFX_CASHITEMPERIODSYSTEM_H__11ADB3C6_2988_4060_913A_824DCFA381E8__INCLUDED_)
