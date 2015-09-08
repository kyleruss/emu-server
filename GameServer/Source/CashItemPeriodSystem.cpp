// CashItemPeriodSystem.cpp: implementation of the CCashItemPeriodSystem class.
// GS-N	1.00.18	JPN	0x00591EB0	-	Completed
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CashItemPeriodSystem.h"
#include "DBSockMng.h"
#include "LogProc.h"
#include "ItemAddOption.h"
#include "..\common\winutil.h"
#include "BuffEffectSlot.h"


CCashItemPeriodSystem g_CashItemPeriodSystem;
BOOL g_bRunningPeriodCheckThread = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCashItemPeriodSystem::CCashItemPeriodSystem()
{
	return;
}

CCashItemPeriodSystem::~CCashItemPeriodSystem()
{
	return;
}

void CCashItemPeriodSystem::Initialize()
{
	this->iItemPeriodInfoCount = 0;

	for(int i = 0; i < MAX_ITEM_PERIOD_INFO; i++)
	{
		ItemPeriodInfo[i].btUsedInfo = FALSE;
		ItemPeriodInfo[i].btUsedCount = FALSE;
		ItemPeriodInfo[i].iUserIndex = 0;
		ItemPeriodInfo[i].iUserGuid = 0;
		ItemPeriodInfo[i].dwTickCount = NULL;
		ItemPeriodInfo[i].szName[0] = NULL;

		for(int n = 0; n < SUBITEM_INFO_COUNT; n++)
		{
			ItemPeriodInfo[i].ItemPeriod[n].btUsedItem = 0;
			ItemPeriodInfo[i].ItemPeriod[n].btItemOption = 0;
			ItemPeriodInfo[i].ItemPeriod[n].btEffectType1 = 0;
			ItemPeriodInfo[i].ItemPeriod[n].btEffectType2 = 0;
			ItemPeriodInfo[i].ItemPeriod[n].iItemCode = 0;
			ItemPeriodInfo[i].ItemPeriod[n].iItemPeriodUsedTime = 0;
			ItemPeriodInfo[i].ItemPeriod[n].iItemPeriodLeftTime = 0;
		}
	}

	if (g_bRunningPeriodCheckThread == FALSE )
	{
		this->hThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CCashItemPeriodSystem::PeriodCheckThread, this->ItemPeriodInfo, 0, NULL);
		
		if ( this->hThreadHandle == NULL )
		{
			LogAdd("CreateThread() failed with error %d", GetLastError());
			return;
		}
	}

	g_bRunningPeriodCheckThread = TRUE;
	CloseHandle(this->hThreadHandle);
}

DWORD CCashItemPeriodSystem::PeriodCheckThread(void * lpPeriodInfo)
{
	ITEMPERIOD_INFO * lpPeriodItem = (ITEMPERIOD_INFO *)lpPeriodInfo;
	int i = 0;
	DWORD dwTick = 0;
	LPOBJ lpObj = NULL;
	DWORD dwTick2 = 0;
	int n = 0;

	while( true )
	{
		if( (GetTickCount() - dwTick) > 10000)
		{
			dwTick = GetTickCount();
			for(i = 0; i < MAX_ITEM_PERIOD_INFO; i++)
			{
				if(lpPeriodItem[i].btUsedInfo == TRUE)
				{
					dwTick2 = GetTickCount() - lpPeriodItem[i].dwTickCount;
					if(dwTick2 > 60000)
					{
						lpPeriodItem[i].dwTickCount = GetTickCount();
						for(n = 0; n < SUBITEM_INFO_COUNT; n++)
						{
							if(lpPeriodItem[i].ItemPeriod[n].btUsedItem == TRUE)
							{
								lpPeriodItem[i].ItemPeriod[n].iItemPeriodLeftTime--;
								lpPeriodItem[i].ItemPeriod[n].iItemPeriodUsedTime++;
								if(lpPeriodItem[i].ItemPeriod[n].iItemPeriodLeftTime <= 0)
								{
									lpObj = &gObj[lpPeriodItem[i].iUserIndex];
									g_CashItemPeriodSystem.GDReqPeriodItemUpdate(lpObj,n);
									g_CashItemPeriodSystem.ClearPeriodItemEffect(lpObj,lpPeriodItem[i].ItemPeriod[n].iItemCode);
								}
							}
						}
					}
				}
			}
		}
		else
		{
			Sleep(10);
		}
	}

	return FALSE;
}

void CCashItemPeriodSystem::GetUserPeriodItemList(LPOBJ lpObj)
{
	this->GDReqPeriodItemList(lpObj);
}



int CCashItemPeriodSystem::CalcItemUseLeftTime(LPSTR pchBuyDate)
{
	int iUsedTime = 0;
	CTime tCurrentTime = CTime::GetTickCount();
	CTime tBuyTime;
	CTimeSpan tTimeSpan;

	this->ConvertStringToTime(pchBuyDate, &tBuyTime);
	tCurrentTime - tBuyTime;
	tTimeSpan = tBuyTime - tCurrentTime;
	iUsedTime = (int)tTimeSpan.GetTotalMinutes();

	return iUsedTime;
}


CTime CCashItemPeriodSystem::CalcItemUseEndTime(int iLeftTime)
{
	CTime tExpireTime;
	CTime tCurrentTime = CTime::GetTickCount();
	CTimeSpan tLeftTime(0, 0, iLeftTime, 0);

	tExpireTime = tCurrentTime + tLeftTime;
	return tExpireTime;
}


void CCashItemPeriodSystem::ConvertStringToTime(const char * pchDate, class CTime * lpTime)
{
	int iYear=0;
	int iMonth=0;
	int iDay=0;
	int iHour=0;
	int iMinute=0;
	int iDateLength=0;
	char chDate[20];
	char* Token;
	char seps[4]="-: ";

	if ( pchDate == NULL || lpTime == NULL )
		return;

	memcpy(chDate, pchDate, sizeof(chDate));
	iDateLength = strlen(chDate);

	if ( iDateLength <= 0 || iDateLength > 20 )
		return;

	Token = strtok( chDate, seps);
	iYear = atoi(Token);
	Token = strtok( NULL, seps);
	iMonth = atoi(Token);
	Token = strtok( NULL, seps);
	iDay = atoi(Token);
	Token = strtok( NULL, seps);
	iHour = atoi(Token);
	Token = strtok( NULL, seps);
	iMinute = atoi(Token);

	CTime tReturnTime(iYear, iMonth, iDay, iHour, iMinute, 0, -1);

	*lpTime = tReturnTime;
}

void CCashItemPeriodSystem::ConvertTimeToString(const CTime * lpTime, LPSTR pchDate)
{
	int iYear=0;
	int iMonth=0;
	int iDay=0;
	int iHour=0;
	int iMinute=0;
	char chDate[20];

	iYear = lpTime->GetYear();
	iMonth = lpTime->GetMonth();
	iDay = lpTime->GetDay();
	iHour = lpTime->GetHour();
	iMinute = lpTime->GetMinute();

	wsprintf(chDate, "%d-%d-%d %d:%d", iYear, iMonth, iDay, iHour, iMinute);
	memcpy(pchDate, chDate, sizeof(chDate));
}

BOOL CCashItemPeriodSystem::SetPeriodItemEffect(LPOBJ lpObj, int iItemCode, BYTE btEffectType1, BYTE btEffectType2, DWORD dwItemPeriod)
{
	int iItemInfoIndex = -1;
	int ItemOption = 0;
	int i = 0,n = 0;


	ItemOption = g_ItemAddOption.SearchItemOption(iItemCode);

	if(ItemOption == -1)
		return false;

	ClearPeriodItem(lpObj,iItemCode);

	if(lpObj->m_iPeriodItemEffectIndex != -1)
	{
		iItemInfoIndex = lpObj->m_iPeriodItemEffectIndex;
		ItemPeriodInfo[iItemInfoIndex].btUsedCount++;

		for(n = 0; n < SUBITEM_INFO_COUNT; n++)
		{
			if(ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btUsedItem == FALSE)
			{
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btUsedItem = TRUE;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemCode = iItemCode;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btEffectType1 = btEffectType1;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btEffectType2 = btEffectType2;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btItemOption = ItemOption;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemPeriodLeftTime = dwItemPeriod;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemPeriodUsedTime = 0;

				_SetItemEffect(lpObj,iItemCode,dwItemPeriod);
				return TRUE;
			}
		}
	}
	else
	{
		for(i = 0; i < MAX_ITEM_PERIOD_INFO; i++)
		{
			if(ItemPeriodInfo[i].btUsedInfo == FALSE)
			{
				lpObj->m_iPeriodItemEffectIndex = i;
				ItemPeriodInfo[i].btUsedInfo = TRUE;
				ItemPeriodInfo[i].iUserGuid = lpObj->DBNumber;
				ItemPeriodInfo[i].iUserIndex = lpObj->m_Index;
				ItemPeriodInfo[i].dwTickCount = GetTickCount();
				ItemPeriodInfo[i].btUsedCount = 1;
				memcpy(ItemPeriodInfo[i].szName,lpObj->Name,sizeof(lpObj->AccountID));
				ItemPeriodInfo[i].ItemPeriod[0].btUsedItem = TRUE;
				ItemPeriodInfo[i].ItemPeriod[0].iItemCode = iItemCode;
				ItemPeriodInfo[i].ItemPeriod[0].btEffectType1 = btEffectType1;
				ItemPeriodInfo[i].ItemPeriod[0].btEffectType2 = btEffectType2;
				ItemPeriodInfo[i].ItemPeriod[0].btItemOption = ItemOption;
				ItemPeriodInfo[i].ItemPeriod[0].iItemPeriodLeftTime = dwItemPeriod;
				ItemPeriodInfo[i].ItemPeriod[0].iItemPeriodUsedTime = 0;

				_SetItemEffect(lpObj,iItemCode,dwItemPeriod);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CCashItemPeriodSystem::_SetItemEffect(LPOBJ lpObj,int iItemCode,int iValidTime)
{
	int bSuccess = FALSE;
	int BuffNumber = 0;

	BuffNumber = GetEffectNumber(iItemCode);

	if( BuffNumber != 0 )
	{
		int iTime = iValidTime * 60;
		bSuccess = gObjAddBuffEffect(lpObj,BuffNumber,iTime);

		switch(BuffNumber)
		{
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
			gObjCalCharacter(lpObj->m_Index);
			break;
		}
	}
	else
	{
		bSuccess = !(g_ItemAddOption.SetItemEffect(lpObj,iItemCode,iValidTime)-1);
	}

	return bSuccess;
}

int CCashItemPeriodSystem::GetEffectNumber(int iItemCode)
{
	int BuffNumber = 0;

	switch( iItemCode )
	{
	
	case ITEMGET(13,43):	BuffNumber = 40;	break;
	case ITEMGET(13,44):	BuffNumber = 41;	break;
	case ITEMGET(13,45):	BuffNumber = 42;	break;
	case ITEMGET(13,59):	BuffNumber = 43;	break;
	case ITEMGET(13,62):	BuffNumber = 87;	break;
	case ITEMGET(13,63):	BuffNumber = 88;	break;
	case ITEMGET(13,81):	BuffNumber = 99;	break;
	case ITEMGET(13,82):	BuffNumber = 100;	break;
	case ITEMGET(13,93):	BuffNumber = 101;	break;
	case ITEMGET(13,94):	BuffNumber = 102;	break;
	case ITEMGET(13,95):	BuffNumber = 103;	break;
	case ITEMGET(13,104):	BuffNumber = 113;	break;
	case ITEMGET(13,105):	BuffNumber = 114;	break;
	case ITEMGET(14,72):	BuffNumber = 44;	break;
	case ITEMGET(14,74):	BuffNumber = 46;	break;
	case ITEMGET(14,73):	BuffNumber = 45;	break;
	case ITEMGET(14,75):	BuffNumber = 47;	break;
	case ITEMGET(14,76):	BuffNumber = 48;	break;
	case ITEMGET(14,77):	BuffNumber = 49;	break;
	case ITEMGET(14,78):	BuffNumber = 50;	break;
	case ITEMGET(14,79):	BuffNumber = 51;	break;
	case ITEMGET(14,80):	BuffNumber = 52;	break;
	case ITEMGET(14,81):	BuffNumber = 53;	break;
	case ITEMGET(14,82):	BuffNumber = 54;	break;
	case ITEMGET(14,97):	BuffNumber = 89;	break;
	case ITEMGET(14,98):	BuffNumber = 90;	break;
		default:
		BuffNumber = 0;	break;
	}
	return BuffNumber;
}

void CCashItemPeriodSystem::SavePeriodInfo(LPOBJ lpObj)
{
	int iItemInfoIndex = lpObj->m_iPeriodItemEffectIndex;

	if(iItemInfoIndex < 0 || iItemInfoIndex >= MAX_ITEM_PERIOD_INFO)
		return;

	ITEMPERIOD_INFO * lpItemInfo = &ItemPeriodInfo[iItemInfoIndex];

	for(int n = 0; n < SUBITEM_INFO_COUNT; n++)
	{
		if(lpItemInfo->ItemPeriod[n].btUsedItem == TRUE)	{
			lpItemInfo->ItemPeriod[n].iItemPeriodUsedTime++;
			lpItemInfo->ItemPeriod[n].iItemPeriodLeftTime--;
			GDReqPeriodItemUpdate(lpObj,n);
		}
	}
}

BOOL CCashItemPeriodSystem::ClearPeriodItemEffect(LPOBJ lpObj,int iItemCode)
{
	int iItemInfoIndex = 0;
	iItemInfoIndex = lpObj->m_iPeriodItemEffectIndex;

	if(iItemInfoIndex < 0 || iItemInfoIndex > MAX_ITEM_PERIOD_INFO)
		return FALSE;

	if(ItemPeriodInfo[iItemInfoIndex].btUsedInfo != TRUE && ItemPeriodInfo[iItemInfoIndex].iUserIndex != lpObj->m_Index)
		return FALSE;

	ItemPeriodInfo[iItemInfoIndex].btUsedCount --;

	if(ItemPeriodInfo[iItemInfoIndex].btUsedCount == 0 || iItemCode == -1)
	{
		//SavePeriodInfo(lpObj);

		ItemPeriodInfo[iItemInfoIndex].btUsedInfo = FALSE;
		ItemPeriodInfo[iItemInfoIndex].iUserIndex = FALSE;
		ItemPeriodInfo[iItemInfoIndex].iUserGuid = FALSE;
		ItemPeriodInfo[iItemInfoIndex].btUsedCount = FALSE;
		ItemPeriodInfo[iItemInfoIndex].dwTickCount = FALSE;
		ItemPeriodInfo[iItemInfoIndex].szName[0] = 0;

		for(int n = 0; n < SUBITEM_INFO_COUNT; n++)
		{

			g_ItemAddOption.ClearItemEffect(lpObj,ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemCode,2);
			ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btUsedItem = FALSE;
			ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemCode = FALSE;
			ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btItemOption = FALSE;
			ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btEffectType1 = FALSE;
			ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btEffectType2 = FALSE;
			ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemPeriodLeftTime = FALSE;
			ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemPeriodUsedTime = FALSE;
		}

		lpObj->m_iPeriodItemEffectIndex = -1;
	}
	else
	{
		int n = 0;
		for(n = 0; n < SUBITEM_INFO_COUNT; n++)
		{
			if(ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemCode == iItemCode)
			{
				g_ItemAddOption.ClearItemEffect(lpObj,ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemCode,2);
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btUsedItem = FALSE;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemCode = FALSE;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btItemOption = FALSE;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btEffectType1 = FALSE;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].btEffectType2 = FALSE;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemPeriodLeftTime = FALSE;
				ItemPeriodInfo[iItemInfoIndex].ItemPeriod[n].iItemPeriodUsedTime = FALSE;
			}
		}
	}

	int BuffNumber = 0;
	BuffNumber = GetEffectNumber(iItemCode);

	switch(BuffNumber)
	{
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
		gObjCalCharacter(lpObj->m_Index);
		break;
	}

	return TRUE;
}

struct PMSG_REQ_PERIODITEM_INSERT
{
	PBMSG_HEAD2 head;	// C1:D0:03
	int iUserIndex;	// 4
	int iUserGuid;	// 8
	int iItemCode;	// C
	int iItemPeriodDate;	// 10
	BYTE btItemEffectType1;	// 14
	BYTE btItemEffectType2;	// 15
	char chUserID[11];	// 16
	char chExpireDate[20];	//21
	BYTE btItemOption;
};



void CCashItemPeriodSystem::GDReqPeriodItemInsert(LPOBJ lpObj, int iItemCode, int iItemEffectDate)
{
	PMSG_REQ_PERIODITEM_INSERT pMsg;

	int iEffectType1 =0;
	int iEffectType2 =0;
	CTime tExpireDate;
	int iItemPeriodDate = 0;

	if ( g_ItemAddOption.SearchItemEffectType(iItemCode, &iEffectType1, &iEffectType2, &iItemPeriodDate) == false )
		return;

	if ( iItemEffectDate == -1 )
		iItemEffectDate = iItemPeriodDate / 60;

	int iItemOption = g_ItemAddOption.SearchItemOption(iItemCode);

	if ( iItemOption == -1 )
		return;

	pMsg.btItemOption = iItemOption;
	pMsg.iUserIndex = lpObj->m_Index;
	pMsg.iUserGuid = lpObj->DBNumber;
	pMsg.iItemCode = iItemCode;
	pMsg.btItemEffectType1 = iEffectType1;
	pMsg.btItemEffectType2 = iEffectType2;
	pMsg.iItemPeriodDate = iItemEffectDate;
	memcpy(pMsg.chUserID, lpObj->Name, sizeof(pMsg.chUserID));

	tExpireDate = this->CalcItemUseEndTime(iItemEffectDate);

	wsprintf(pMsg.chExpireDate, "%d-%d-%d %d:%d", tExpireDate.GetYear(), tExpireDate.GetMonth(), tExpireDate.GetDay(), tExpireDate.GetHour(), tExpireDate.GetMinute());

	PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0x03, sizeof(PMSG_REQ_PERIODITEM_INSERT));

	cDBSMng.Send((char *)&pMsg, sizeof(PMSG_REQ_PERIODITEM_INSERT));

	LogAddTD("[CashShop][PeriodItemInsert Req] - ID : %s, Name : %s, ItemCode : %d, Effect1 : %d, Effect2 : %d, UseTime : %d, ExpireDate : %s",
		lpObj->AccountID, lpObj->Name, pMsg.iItemCode, pMsg.btItemEffectType1, pMsg.btItemEffectType2, pMsg.iItemPeriodDate, pMsg.chExpireDate);
}



struct PMSG_REQ_PERIODITEM_LIST
{
	PBMSG_HEAD2 head;	// C1:D0:05
	int iUserIndex;	// 4
	int iUserGuid;	// 8
	char chCharacterName[11];	// C
};





void CCashItemPeriodSystem::GDReqPeriodItemList(LPOBJ lpObj)
{
	PMSG_REQ_PERIODITEM_LIST pMsg;

	pMsg.iUserIndex = lpObj->m_Index;
	pMsg.iUserGuid = lpObj->DBNumber;
	memcpy(pMsg.chCharacterName, lpObj->Name, sizeof(pMsg.chCharacterName));

	PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0x05, sizeof(PMSG_REQ_PERIODITEM_LIST));

	cDBSMng.Send((char *)&pMsg, sizeof(PMSG_REQ_PERIODITEM_LIST));
}


struct PMSG_REQ_PERIODITEM_INQUIRY
{
	PBMSG_HEAD2 head;	// C1:D0:01
	int iUserIndex;	// 4
	int iUserGuid;	// 8

};

void CCashItemPeriodSystem::GDReqPeriodItemInquiry(LPOBJ lpObj)
{
	PMSG_REQ_PERIODITEM_INQUIRY pMsg;

	pMsg.iUserIndex = lpObj->m_Index;
	pMsg.iUserGuid = lpObj->DBNumber;

	PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0x01, sizeof(PMSG_REQ_PERIODITEM_INQUIRY));

	cDBSMng.Send((char *)&pMsg, sizeof(PMSG_REQ_PERIODITEM_INQUIRY));
}




struct PMSG_REQ_PERIODITEM_UPDATE
{
	PBMSG_HEAD2 head;	// C1:D0:07
	int iUserIndex;	// 4
	int iUserGuid;	// 8
	int iItemCode;	// C
	int iUsedTime;	// 10
	int iLeftTime;	// 14
	char chCharacterName[11];	// 18
};




void CCashItemPeriodSystem::GDReqPeriodItemUpdate(LPOBJ lpObj,int itemid)
{
	PMSG_REQ_PERIODITEM_UPDATE pMsg;
	ITEMPERIOD_INFO * lpItemInfo = NULL;

	if(lpObj->m_iPeriodItemEffectIndex < 0 || lpObj->m_iPeriodItemEffectIndex >= MAX_ITEM_PERIOD_INFO)
		return;

	if(itemid < 0 || itemid > SUBITEM_INFO_COUNT)
		return;

	lpItemInfo = &ItemPeriodInfo[lpObj->m_iPeriodItemEffectIndex];

	if(lpItemInfo->btUsedInfo == FALSE)
		return;

	pMsg.iUserGuid = lpObj->DBNumber;
	pMsg.iUserIndex = lpObj->m_Index;
	pMsg.iItemCode = lpItemInfo->ItemPeriod[itemid].iItemCode;
	pMsg.iLeftTime = lpItemInfo->ItemPeriod[itemid].iItemPeriodLeftTime;
	pMsg.iUsedTime = lpItemInfo->ItemPeriod[itemid].iItemPeriodUsedTime;
	memcpy(pMsg.chCharacterName, lpItemInfo->szName, sizeof(pMsg.chCharacterName));

	PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0x07, sizeof(PMSG_REQ_PERIODITEM_UPDATE));

	cDBSMng.Send((char *)&pMsg, sizeof(PMSG_REQ_PERIODITEM_UPDATE));
}





void CCashItemPeriodSystem::DGAnsPeriodItemInsert(PMSG_ANS_PERIODITEM_INSERT * aRecv)
{
	int iIndex = aRecv->iUserIndex;
	LPOBJ lpObj = &gObj[iIndex];
	BYTE bResult = 0;

	switch(aRecv->btResult)
	{
	case 0:	bResult = 1;break;
	case 1: bResult = 1;break;
	case 2: bResult = 3;break;
	default: bResult = 1;
	}

	if(bResult == 1)
	{
		SetPeriodItemEffect(lpObj,aRecv->iItemCode,aRecv->btItemEffectType1,aRecv->btItemEffectType2,aRecv->iItemPeriodDate);
		LogAddTD("[CashShop][PeriodItemInsert Ans] Success - ID : %s, Name : %s, ItemCode : %d, Effect1 : %d, Effect2 : %d, UseTime : %d",
			lpObj->AccountID,lpObj->Name,aRecv->iItemCode,aRecv->btItemEffectType1,aRecv->btItemEffectType2,aRecv->iItemPeriodDate);
	}
	else
	{
		LogAddTD("[CashShop][PeriodItemInserver Ans] Fail - ID : %s, Name : %s, ItemCode : %d, Effect1 : %d, Effect2 : %d, UseTime : %d, Result : %d",
			lpObj->AccountID,lpObj->Name,aRecv->iItemCode,aRecv->btItemEffectType1,aRecv->btItemEffectType2,aRecv->iItemPeriodDate,bResult);
	}

	GCSendPeriodItemInserResult(lpObj,bResult);
}


void CCashItemPeriodSystem::DGAnsPeriodItemList(PMSG_ANS_PERIODITEM_LIST* aRecv)
{
	if ( aRecv->iUserIndex <= 0 || aRecv->iUserIndex > OBJMAX )
		return;

	LPOBJ lpObj = &gObj[aRecv->iUserIndex];
	int UseTime = 0;
	int bResult = 0;
	bResult = aRecv->btResult;

	if(lpObj->DBNumber != aRecv->iUserGuid)
		return;

	if(bResult == -1)
	{
		LogAddTD("[CashShop][PeriodItemSelect Ans] Failed - Item over 1 item ID : %s, Name : %s",
			lpObj->AccountID,lpObj->Name);
	}
	else if(bResult == -2)
	{
		LogAddTD("[CashShop][PeriodItemSelect Ans] Failed - Item usedtime is over ID : %s, Name : %s",
			lpObj->AccountID,lpObj->Name);
	}

	int count = aRecv->btCount;

	for(int i = 0; i < count; i++)
	{
		UseTime = aRecv->Items[i].iItemPeriodLeftTime;
		if(UseTime  > 0)
		{
			SetPeriodItemEffect(lpObj,aRecv->Items[i].iItemCode,aRecv->Items[i].btItemEffectType1,aRecv->Items[i].btItemEffectType2,UseTime);
			LogAddTD("[CashShop][PeriodItemList Ans] Success - ID : %s, Name : %s, ItemCode : %d, OptionType : %d, Effect1 : %d, Effect2 : %d, UseTime : %d",
				lpObj->AccountID,lpObj->Name,aRecv->Items[i].iItemCode,aRecv->Items[i].btItemOption,aRecv->Items[i].btItemEffectType1,aRecv->Items[i].btItemEffectType2,UseTime);
		}
		else
		{
			LogAddTD("[CashShop][PeriodItemList Ans] Fail - ID : %s, Name : %s, ItemCode : %d, OptionType : %d, Effect1 : %d, Effect2 : %d, UseTime : %d",
				lpObj->AccountID,lpObj->Name,aRecv->Items[i].iItemCode,aRecv->Items[i].btItemOption,aRecv->Items[i].btItemEffectType1,aRecv->Items[i].btItemEffectType2,UseTime);
		}
	}
}



void CCashItemPeriodSystem::DGAnsPeriodItemInquiry(PMSG_ANS_PERIODITEM_INQUIRY *aRecv)
{
	return;
}



void CCashItemPeriodSystem::DGAnsPeriodItemUpdate(PMSG_ANS_PERIODITEM_UPDATE *aRecv)
{

	int iUserIndex = aRecv->iUserIndex;

	if ( iUserIndex < 0 || iUserIndex > OBJMAX )
		return;

	LPOBJ lpObj = &gObj[iUserIndex];
	ITEMPERIOD_INFO* lpItemInfo = NULL;

	if ( lpObj->m_iPeriodItemEffectIndex < 0 || lpObj->m_iPeriodItemEffectIndex >=MAX_ITEM_PERIOD_INFO )
		return;

	lpItemInfo = &ItemPeriodInfo[lpObj->m_iPeriodItemEffectIndex];

	if ( lpObj->m_iPeriodItemEffectIndex != lpItemInfo->iUserIndex )
		return;

	switch ( aRecv->btResult )
	{
		case 3:
			if ( ClearPeriodItemEffect(lpObj,aRecv->iItemCode) == TRUE )
			{		
				LogAddTD("[CashShop][PeriodItemUpdate Ans] Remove Item Effect - ID : %s, Name : %s, ItemCode : %d, Result : %d",
					lpObj->AccountID, lpObj->Name, aRecv->iItemCode, aRecv->btResult);
			}
	}

	LogAddTD("[CashShop][PeriodItemUpdate Ans] - ID : %s, Name : %s, ItemCode : %d, Result %d",
		lpObj->AccountID, lpObj->Name, aRecv->iItemCode, aRecv->btResult);
}



struct PMSG_REQ_PERIODITEM_DELETE
{
	PBMSG_HEAD2 head;	// C1:D0:09
	int iUserIndex;	// 4
	int iUserGuid;	// 8
	char chCharacterName[11];	// C
};



void CCashItemPeriodSystem::GDReqPeriodItemDelete(LPOBJ lpObj, LPSTR pchCharacterName)
{
	PMSG_REQ_PERIODITEM_DELETE pMsg;
	ITEMPERIOD_INFO* lpItemInfo = NULL;
	
	pMsg.iUserIndex = lpObj->m_Index;
	pMsg.iUserGuid = lpObj->DBNumber;
	memcpy(pMsg.chCharacterName, pchCharacterName, sizeof(pMsg.chCharacterName));

	PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0x09, sizeof(PMSG_REQ_PERIODITEM_DELETE));

	cDBSMng.Send((char*)&pMsg, sizeof(PMSG_REQ_PERIODITEM_DELETE));
}

void CCashItemPeriodSystem::DGAnsPeriodItemDelete(PMSG_ANS_PERIODITEM_DELETE *aRecv)
{
	LPOBJ lpObj = NULL;

	if ( aRecv->iUserIndex < 0 || aRecv->iUserIndex > OBJMAX )
		return;

	lpObj = &gObj[aRecv->iUserIndex];

	if ( lpObj->DBNumber != aRecv->iUserGuid )
		return;

	switch ( aRecv->btResult )
	{
	case 0:
		LogAddTD("[CashShop][PeriodItemDelete Ans] - ID : %s, Name : %s",
			lpObj->AccountID,lpObj->Name);
		break;
	}
}


void CCashItemPeriodSystem::GCSendPeriodItemInserResult(LPOBJ lpObj, BYTE btResult)
{
	PMSG_ANS_PERIODITEM_INSERT_RESULT pMsg;
	
	pMsg.btResult = btResult;

	PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0x03, sizeof(PMSG_ANS_PERIODITEM_INSERT_RESULT));

	DataSend(lpObj->m_Index, (LPBYTE)&pMsg,	sizeof(PMSG_ANS_PERIODITEM_INSERT_RESULT));
}

BYTE CCashItemPeriodSystem::ClearPeriodItem(LPOBJ lpObj,int iItemCode)
{
	int PeriodItemIndex = lpObj->m_iPeriodItemEffectIndex;
	int ItemOption =0;
	ItemOption = g_ItemAddOption.SearchItemOption(iItemCode);

	if(PeriodItemIndex == -1)
		return TRUE;

	if(ItemPeriodInfo[PeriodItemIndex].iUserGuid != lpObj->DBNumber)
		return FALSE;

	for(int i = 0; i < SUBITEM_INFO_COUNT; i++)
	{
		if(ItemPeriodInfo[PeriodItemIndex].ItemPeriod[i].btUsedItem == TRUE
			&& ItemPeriodInfo[PeriodItemIndex].ItemPeriod[i].btItemOption == ItemOption)
		{
			ClearPeriodItemEffect(lpObj,ItemPeriodInfo[PeriodItemIndex].ItemPeriod[i].iItemCode);
			return FALSE;
		}
	}
return TRUE;
}

int CCashItemPeriodSystem::CheckMoveMapBoundItem(LPOBJ lpObj)
{
	int iPeriodItemIndex = lpObj->m_iPeriodItemEffectIndex;

	if(iPeriodItemIndex == -1)
		return FALSE;

	if(ItemPeriodInfo[iPeriodItemIndex].btUsedInfo == FALSE)
		return FALSE;

	for(int i = 0; i < SUBITEM_INFO_COUNT; i++)
	{
		if(ItemPeriodInfo[iPeriodItemIndex].ItemPeriod[i].btUsedItem == TRUE)
		{
			switch(ItemPeriodInfo[iPeriodItemIndex].ItemPeriod[i].iItemCode)
			{
			case ITEMGET(13,43):
			case ITEMGET(13,44):
			case ITEMGET(13,45):
			case ITEMGET(13,52):
			case ITEMGET(13,59):
			case ITEMGET(13,62):
			case ITEMGET(13,63):
			case ITEMGET(13,93):
			case ITEMGET(13,94):
				return TRUE;
			}
		}
	}
return FALSE;
}

ITEMPERIOD_INFO * CCashItemPeriodSystem::GetPeriodInfo(LPOBJ lpObj)
{
	if(lpObj->m_iPeriodItemEffectIndex < 0)
		return FALSE;

	if(ItemPeriodInfo[lpObj->m_iPeriodItemEffectIndex].btUsedInfo == FALSE)
		return FALSE;

return &ItemPeriodInfo[lpObj->m_iPeriodItemEffectIndex];
}