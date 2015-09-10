#include "stdafx.h"
#include "ChaosBox.h"
#include "logproc.h"
#include "GameMain.h"
#include "GameServer.h"
#include "..\common\winutil.h"
#include "DSProtocol.h"
#include "CastleSiegeSync.h"
#include "BloodCastle.h"
#include "..\common\SetItemOption.h"
#include "JewelOfHarmonySystem.h"
#include "giocp.h"
#include "CrywolfSync.h"
#include "CashLotterySystem.h"
#include "CastleSiege.h"
#include "IllusionTempleEvent.h"
#include "Event.h"
#include "ElementalSystem.h"
#include "EventItemBag.h"
#if( __4GAMERS__ == 1 )
#include "Achievements.h"
#endif

CChaosBox g_ChaosBox;

// Constructor / Destructor 

CChaosBox::CChaosBox()
{
	iChaosJewel = ITEMGET(12,15);
	iBlessJewel = ITEMGET(14,13);
	iSoulJewel = ITEMGET(14,14);
	iCreationJewel = ITEMGET(14,22);
	iBlessPack = ITEMGET(12,30);
	iSoulPack = ITEMGET(12,31);
	iCondorFeather = ITEMGET(13,53);
	iCondorStone = ITEMGET(13,52);
	iStormWings = ITEMGET(12,36);
	iRedemptionWings = ITEMGET(12,37);
	iFortitudeWings = ITEMGET(12,38);
	iHurricaneWings = ITEMGET(12,39);
	iMonarchMantle = ITEMGET(12,40);
	iDimensionWings = ITEMGET(12,43);
	iRFCape2 = ITEMGET(12,50);
	iOptionRate = 50;
}


CChaosBox::~CChaosBox()
{

}

BOOL CChaosBox::ChaosBoxCheck(LPOBJ lpObj)
{
	if ( lpObj->pChaosBox == NULL )
	{
		return false;
	}

	return true;
}





BOOL CChaosBox::ChaosBoxInit(LPOBJ lpObj)
{
	int n;

	if ( lpObj->pChaosBox != NULL )
	{
		for (n=0;n<CHAOS_BOX_SIZE;n++)
		{
			g_ElementalSystem.ClearErtel(lpObj,&lpObj->pChaosBox[n]);
			lpObj->pChaosBox[n].Clear();
		}

		for (n=0;n<CHAOS_BOX_SIZE;n++)
		{
			lpObj->pChaosBoxMap[n] = -1;
		}
		return true;
	}

	lpObj->pChaosBox = new CItem[CHAOS_BOX_SIZE];

	if ( lpObj->pChaosBox == NULL )
	{
		return false;
	}

	lpObj->pChaosBoxMap = new unsigned char[CHAOS_BOX_SIZE];

	if ( lpObj->pChaosBoxMap == NULL )
	{
		delete lpObj->pChaosBox;
		return false;
	}

	for (n=0;n<CHAOS_BOX_SIZE;n++)
	{
		lpObj->pChaosBoxMap[n] = -1;
	}

	return true;
}



BOOL CChaosBox::ChaosBoxItemDown(LPOBJ lpObj)
{
	if ( lpObj->pChaosBox == NULL )
	{
		return FALSE;
	}

	for (int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(14,14) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,83) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,84) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,85) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,86) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,87) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,88) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,89) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,90) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,91) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(13,92) ||
			 lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )
		{
			lpObj->pChaosBox[n].Clear();
		}
		else
		{
			int op = lpObj->pChaosBox[n].m_Option1;

			if ( op > 0 )
			{
				if ( (rand() % 2) == 0 )
				{
					op--;
				}
			}

			lpObj->pChaosBox[n].m_Option1 = op;
			op = lpObj->pChaosBox[n].m_Option3;

			if ( op > 0 )
			{
				if ( (rand() % 2) == 0 )
				{
					op--;
				}
			}

			lpObj->pChaosBox[n].m_Option3 = op;

			if ( lpObj->pChaosBox[n].m_Level > 0 )
			{
				lpObj->pChaosBox[n].m_Level = rand() % lpObj->pChaosBox[n].m_Level;
			}

			float dur = (float)ItemGetDurability(lpObj->pChaosBox[n].m_Type, lpObj->pChaosBox[n].m_Level, lpObj->pChaosBox[n].IsExtItem(), lpObj->pChaosBox[n].IsSetItem() );
			
			lpObj->pChaosBox[n].m_Durability =  dur * lpObj->pChaosBox[n].m_Durability / lpObj->pChaosBox[n].m_BaseDurability;
			lpObj->pChaosBox[n].Convert(lpObj->pChaosBox[n].m_Type, lpObj->pChaosBox[n].m_Option1,
				lpObj->pChaosBox[n].m_Option2, lpObj->pChaosBox[n].m_Option3, lpObj->pChaosBox[n].m_NewOption,
				lpObj->pChaosBox[n].m_SetOption, lpObj->pChaosBox[n].m_ItemOptionEx, 0, -1, CURRENT_DB_VERSION);
		}
	}

	return TRUE;
}



int CChaosBox::ChaosBoxMix(LPOBJ lpObj, int & Result2,int & Result3)
{
	BYTE ExOption[8];
	int ChaosDiamond = 0;
	int ChaosItems = 0;

	if ( lpObj->pChaosBox == NULL )
	{
		return 0;
	}

	int value = 0;
	int add = 0;
	int nv = 0;	// NEW VALUE
	Result2 = 0;
	Result3 = -1;
	int tmpItem = -1;
	lpObj->ChaosSuccessRate = 0;
	lpObj->ChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			lpObj->pChaosBox[n].OldValue();
			add = 0;
			nv = 0;

			if ( lpObj->pChaosBox[n].m_Level >= MIN_CHAOS_ITEM_LEVEL && (lpObj->pChaosBox[n].m_Option3 *4) >= MIN_CHAOS_ITEM_LEVEL )
			{
				nv = lpObj->pChaosBox[n].m_OldBuyMoney;
				value += lpObj->pChaosBox[n].m_OldBuyMoney;
				add = 1;

				if ( lpObj->pChaosBox[n].m_Type == ITEMGET(2,6) || lpObj->pChaosBox[n].m_Type == ITEMGET(4,6) || lpObj->pChaosBox[n].m_Type == ITEMGET(5,7) )	// Chaos Items
				{
					Result2 = 1;
				}
			}

			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				value += lpObj->pChaosBox[n].m_OldBuyMoney;
				nv = lpObj->pChaosBox[n].m_OldBuyMoney;
				ChaosDiamond++;
				add = 1;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) || lpObj->pChaosBox[n].m_Type == ITEMGET(14,14) )	// Jewel of Bless, Jewel of Soul
			{
				value += lpObj->pChaosBox[n].m_OldBuyMoney;
				nv = lpObj->pChaosBox[n].m_OldBuyMoney;
				add = 1;
			}

			if( lpObj->pChaosBox[n].m_Type == ITEMGET(13,83) ||
				lpObj->pChaosBox[n].m_Type == ITEMGET(13,84) ||
				lpObj->pChaosBox[n].m_Type == ITEMGET(13,85) ||
				lpObj->pChaosBox[n].m_Type == ITEMGET(13,86) ||
				lpObj->pChaosBox[n].m_Type == ITEMGET(13,87) )
			{
				tmpItem = lpObj->pChaosBox[n].m_Type;
			}

			if( lpObj->pChaosBox[n].m_Type == ITEMGET(13,14) )
			{
				return false;
			}

			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}

			if ( add != 0  )
			{
				ChaosItems++;
			}

			ItemIsBufExOption(ExOption, &lpObj->pChaosBox[n]);
			LogAdd("[%s][%s] CBMix [%d,%s,%d,%d,%d,%d]serial:[%u][%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d]",
				lpObj->AccountID, lpObj->Name, n, lpObj->pChaosBox[n].GetName(),
				lpObj->pChaosBox[n].m_Level, lpObj->pChaosBox[n].m_Option1,
				lpObj->pChaosBox[n].m_Option2, lpObj->pChaosBox[n].m_Option3,
				lpObj->pChaosBox[n].m_Number, (int)lpObj->pChaosBox[n].m_Durability,
				nv, ExOption[0], ExOption[1], ExOption[2], ExOption[3], ExOption[4], ExOption[5],
				ExOption[6], lpObj->pChaosBox[n].m_SetOption);
		



		}
	}

	if ( ChaosDiamond == 0 )
	{
		value = 0;	// 0% of success
	}

	if ( ChaosItems < 2 )
	{
		value = 0;// 0% of success
	}

	if ( iCharmOfLuckCount > 10 )
		return FALSE;


	
	lpObj->ChaosSuccessRate = value / 10000;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	if ( lpObj->ChaosSuccessRate  > 100 )
	{
		lpObj->ChaosSuccessRate = 100;
	}

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 10000;	// Required Money to MIX anc createe a Chaos Item

	LogAddTD("[%s][%s] CBMix need Zen : %d SuccessRate : %d, CharmRate : %d",
		lpObj->AccountID, lpObj->Name, lpObj->ChaosMoney, 
		lpObj->ChaosSuccessRate, iCharmOfLuckCount);


	if( tmpItem > -1 )
	{
		int Number = tmpItem^ITEMGET(13,00);

		switch( Number )
		{
		case 83:
			Result3 = 2;
			break;
		case 84:
			Result3 = 1;
			break;
		case 85:
			Result3 = 0;
			break;
		case 86:
			Result3 = 41;
			break;
		case 87:
			Result3 = 40;
			break;
		default:
			LogAddTD("[MixSystem] WingCharmItemNum => %d , WingCharmIndex => %d",tmpItem,Number);
			break;
		}
	}

	return value;
}

void CChaosBox::DefaultChaosMix(LPOBJ lpObj)
{
	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	int aIndex = lpObj->m_Index;
	BOOL fail = TRUE;
	int MixResult2;
	lpObj->ChaosLock = TRUE;
	int MixResult3 = -1;

	if ( ChaosBoxMix(lpObj, MixResult2, MixResult3) == 0 )
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	int iChaosTaxMoney = (int)((__int64)lpObj->ChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	lpObj->ChaosMoney += iChaosTaxMoney;

	if ( lpObj->ChaosMoney < 0 )
	{
		lpObj->ChaosMoney = 0;
	}

	if ( lpObj->Money < lpObj->ChaosMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;


	}
	else 
	{
		if ( lpObj->ChaosSuccessRate > 0 )
		{
			if ( lpObj->ChaosSuccessRate >= 100 )
			{
				lpObj->ChaosSuccessRate = 100;
			}

			if ( (rand()%100) <= (lpObj->ChaosSuccessRate-1) ) 
			{
				fail = FALSE;
				pMsg.Result = CB_SUCCESS;

				int Level = rand()%5;
				int Option1 = 0;
				int Option2 = 0;
				int Option3 = 0;

				if ( (rand()%100)< (lpObj->ChaosSuccessRate/5 + 6) )
				{
					Option1 = 1;
				}

				if ( (rand()%100)< (lpObj->ChaosSuccessRate/5 + 4) )
				{
					Option2 = 1;
				}

				int OpProb;
				int OpType = rand()%3;
				OpProb = rand()%100;

				switch ( OpType )
				{
					case 0:
						if ( OpProb < (lpObj->ChaosSuccessRate / 5 + 4) ) 
						{
							Option3 = 3;
						}
						break;

					case 1:
						if ( OpProb < (lpObj->ChaosSuccessRate / 5 + 8) ) 
						{
							Option3 = 2;
						}
						break;

					case 2:
						if ( OpProb < (lpObj->ChaosSuccessRate / 5 + 12) ) 
						{
							Option3 = 1;
						}
						break;
				}

				if ( MixResult2 == TRUE )
				{
					int WingType = rand()%4;
					int WingNum = 0;

					if ( MixResult3 > -1 )
					{
						WingNum = ITEMGET(12,00) + MixResult3;
					}
					else
					{
						if ( WingType == 0 )
						{
							WingNum = ITEMGET(12,0);
						}
						else if ( WingType == 1 )
						{
							WingNum = ITEMGET(12,1);
						}
						else if ( WingType == 2 )
						{
							WingNum = ITEMGET(12,2);
						}
						else if ( WingType == 3 )
						{
							WingNum = ITEMGET(12,41);
						}
					}

					::ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, WingNum, 0, 255, Option1, Option2, Option3, -1, 0, 0);
				}
				else
				{
					int ChaosItemType = rand()%3;
					int ChaosItemNum = 0;

					if ( ChaosItemType == 0 )	// Chaos Dragon Axe
					{
						ChaosItemNum = ITEMGET(2,6);
					}
					else if ( ChaosItemType == 1 )	// Chaos Nature Bow
					{
						ChaosItemNum = ITEMGET(4,6);
					}
					else if ( ChaosItemType == 2 )	// Chaos Lighting Staff
					{
						ChaosItemNum = ITEMGET(5,7);
					}

					::ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, ChaosItemNum, Level, 255, Option1, Option2, Option3, -1, 0, 0);
				}
			}
		}


		lpObj->Money -= lpObj->ChaosMoney;
		g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
		GCMoneySend(lpObj->m_Index, lpObj->Money);

		if ( fail == TRUE )
		{
			ChaosBoxItemDown(lpObj);
			GCUserChaosBoxSend(lpObj, 0);
			DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
			LogAddTD("[%s][%s] CBMix Fail %d Money : %d-%d",
				lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate,
				lpObj->Money, lpObj->ChaosMoney);
			lpObj->ChaosLock = FALSE;
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		}
		else
		{
			LogAddTD("[%s][%s] CBMix Success Rate:%d Money : %d",
				lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate,
				lpObj->ChaosMoney);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		}
	}

	::gObjInventoryCommit(lpObj->m_Index);
}

void CChaosBox::LogDQChaosItem(LPOBJ lpObj)
{
	BYTE ExOption[MAX_EXOPTION_SIZE];

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			::ItemIsBufExOption(ExOption, &lpObj->pChaosBox[n] );

			LogAddTD("[DevilSquare,%d] [%s][%s] CBMix [%d,%s,%d,%d,%d,%d]serial:[%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d] 380Ex[%d] HO:[%d,%d]",
				lpObj->pChaosBox[n].m_Level,
				lpObj->AccountID, lpObj->Name, n, lpObj->pChaosBox[n].GetName(), lpObj->pChaosBox[n].m_Level,
				lpObj->pChaosBox[n].m_Option1, lpObj->pChaosBox[n].m_Option2, lpObj->pChaosBox[n].m_Option3, 
				lpObj->pChaosBox[n].m_Number, (int)lpObj->pChaosBox[n].m_Durability, 
				ExOption[0], ExOption[1], ExOption[2], ExOption[3], ExOption[4], ExOption[5], ExOption[6],
				lpObj->pChaosBox[n].m_SetOption, lpObj->pChaosBox[n].m_ItemOptionEx >> 7,
				g_kJewelOfHarmonySystem.GetItemStrengthenOption(&lpObj->pChaosBox[n]),
				g_kJewelOfHarmonySystem.GetItemOptionLevel(&lpObj->pChaosBox[n]));
		}
	}
}

int  CChaosBox::CheckDevilSquareItem(LPOBJ lpObj, int & eventitemcount, int & itemlevel)
{
	BOOL bChaoseGem = FALSE;
	BOOL bEyeOfDevil = FALSE;
	BOOL bKeyOfDevil = FALSE;
	eventitemcount = 0;
	BOOL FoundOtherItem = FALSE;
	int level = -1;
	BOOL bLevelCheck = FALSE;
	int iCharmOfLuckCount = 0;
	BOOL bCharmOfLuckOver = FALSE;

	LogAdd("[DevilSquare] Search DevilSquareItem");

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )	// Chaos
			{
				bChaoseGem = TRUE;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,17) )	// Eye
			{
				eventitemcount +=1;
				bEyeOfDevil = TRUE;

				if ( level != lpObj->pChaosBox[n].m_Level )
				{
					if ( level == -1 )
					{
						level = lpObj->pChaosBox[n].m_Level;
					}
					else
					{
						bLevelCheck = TRUE;
					}
				}
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,18) )	// Key
			{
				eventitemcount +=1;
				bKeyOfDevil = TRUE;

				if ( level != lpObj->pChaosBox[n].m_Level )
				{
					if ( level == -1 )
					{
						level = lpObj->pChaosBox[n].m_Level;
					}
					else
					{
						bLevelCheck = TRUE;
					}
				}
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;

				if ( iCharmOfLuckCount > 10 )
				{
					bCharmOfLuckOver = TRUE;
				}
			}
			else
			{
				FoundOtherItem = TRUE;
			}
		}
	}

	itemlevel = level;
	lpObj->ChaosSuccessRate = iCharmOfLuckCount;

	if ( FoundOtherItem != FALSE )
	{
		LogAdd("[DevilSquare] Other DQItems Found");
		return 0;
	}

	if ( bLevelCheck != FALSE )
	{
		LogAdd("[DevilSquare] DiffLevel DQItems Found");
		return 3;
	}

	if ( bCharmOfLuckOver == TRUE )
	{
		LogAdd("[DevilSquare] Charm Of Luck Count Over");
		return 4;
	}

	if ( bChaoseGem != FALSE && bEyeOfDevil != FALSE && bKeyOfDevil != FALSE )
	{
		LogDQChaosItem(lpObj);
		return 1;
	}

	if ( bEyeOfDevil != FALSE && bKeyOfDevil != FALSE )
	{
		LogDQChaosItem(lpObj);
		return 2;
	}

	LogAdd("[DevilSquare] DQItems Not Found");

	return 0;
}

BOOL CChaosBox::DevilSquareEventChaosMix(LPOBJ lpObj, BOOL bCheckType, int iItemLevel)
{
	BOOL fail = TRUE;
	int aIndex = lpObj->m_Index;
	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	lpObj->ChaosLock = TRUE;

	LogAddTD("[DevilSquare] Chaos Mix Start");
	INT nChaosNeedMoney = 0;
	int iCharmOfLuckCount = 0;
	iCharmOfLuckCount = lpObj->ChaosSuccessRate;

	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		lpObj->ChaosLock = FALSE;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
	}

	switch ( iItemLevel )
	{
		case 0:
			lpObj->ChaosSuccessRate = 60;
			nChaosNeedMoney = 100000;
			break;
		case 1:
			lpObj->ChaosSuccessRate = gDQChaosSuccessRateLevel1;
			nChaosNeedMoney = 100000;
			break;

		case 2:
			lpObj->ChaosSuccessRate = gDQChaosSuccessRateLevel2;
			nChaosNeedMoney = 200000;
			break;

		case 3:
			lpObj->ChaosSuccessRate = gDQChaosSuccessRateLevel3;
			nChaosNeedMoney = 400000;
			break;

		case 4:
			lpObj->ChaosSuccessRate = gDQChaosSuccessRateLevel4;
			nChaosNeedMoney = 700000;
			break;

		case 5:
			lpObj->ChaosSuccessRate = gDQChaosSuccessRateLevel5;
			nChaosNeedMoney = 1100000;
			break;

		case 6:
			lpObj->ChaosSuccessRate = gDQChaosSuccessRateLevel6;
			nChaosNeedMoney = 1600000;
			break;
		case 7:
			lpObj->ChaosSuccessRate = gDQChaosSuccessRateLevel7;
			nChaosNeedMoney = 2000000;
			break;

		default:
			LogAdd("[DevilSquare] [%s][%s] Invalid DQItem Level [%d]",
				lpObj->AccountID, lpObj->Name, iItemLevel);

			pMsg.Result = CB_INVALID_ITEM_LEVEL;
			DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
			lpObj->ChaosLock = FALSE;
			return 1;
			break;
	}

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( (lpObj->Money - nChaosNeedMoney) < 0 )
	{
		LogAddTD("[DevilSquare] [%s][%s] CBMix Not Enough Money [%d] need zen [%d]",
			lpObj->AccountID, lpObj->Name, lpObj->Money, nChaosNeedMoney);
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return 1;

	}

	if ( g_CrywolfSync.GetOccupationState() == 0 && g_iCrywolfApplyMvpBenefit )
	{	
		lpObj->ChaosSuccessRate += g_CrywolfSync.GetPlusChaosRate();
	}

	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	int iRate = rand() % 100;

	if ( bCheckType == TRUE )
	{
		if ( iRate < lpObj->ChaosSuccessRate )
		{
			int DevilInv = ITEMGET(14,19);	// Devil Ticket
			ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, DevilInv, iItemLevel, 0, 0, 0, 0, -1, 0, 0);
			fail = FALSE;
		}
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( fail == TRUE )
	{
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj, 0);
		DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);

		LogAddTD("[DevilSquare,%d] [%s][%s] CBMix Fail %d Money : %d-%d", iItemLevel,
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate,
			lpObj->Money, nChaosNeedMoney);
		lpObj->ChaosLock = FALSE;
	}
	else
	{
		LogAddTD("[DevilSquare,%d] [%s][%s] CBMix Success Rate:%d Money : %d", iItemLevel,
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate,
			nChaosNeedMoney);
#if( __4GAMERS__ == 1 )
		g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
	}

	::gObjInventoryCommit(lpObj->m_Index);

	return TRUE;
}

void CChaosBox::DevilSquareItemChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	int aIndex = lpObj->m_Index;
	int eventitemcount;
	int itemlevel;
	int Ret = CheckDevilSquareItem(lpObj, eventitemcount, itemlevel);

	if ( Ret != FALSE )
	{
		if ( Ret == 3 )
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
			LogAdd("[DevilSquare] DiffLevel Devil's Key or Eyes [%d]", eventitemcount);
			lpObj->ChaosLock = FALSE;
			return;
		}
		
		if ( Ret == 2 )
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
			LogAdd("[DevilSquare] Not Found Chaos Gem [%d]", eventitemcount);
			lpObj->ChaosLock = FALSE;
			
			return;
		}

		if ( Ret == 4 )
		{
			pMsg.Result = 0xF0;
			DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
			LogAdd("[DevilSquare] Charm Of Luck Over 10% [%d]", eventitemcount);
			lpObj->ChaosLock = FALSE;
			
			return;
		}

			
		if ( eventitemcount > 2 )
		{
			pMsg.Result = CB_TOO_MANY_ITEMS;
			DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
			LogAdd("[DevilSquare] Too many Devil's Key or Eyes [%d]", eventitemcount);
			lpObj->ChaosLock = FALSE;
			return;
		}

		if ( lpObj->Level < 10 )
		{
			pMsg.Result = CB_LOW_LEVEL_USER;
			DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
			lpObj->ChaosLock = FALSE;
			return;
		}

		DevilSquareEventChaosMix(lpObj, Ret, itemlevel);
		return;
	}
		
	if ( eventitemcount > 1 )
	{
		pMsg.Result = CB_LACKING_MIX_ITEMS;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->ChaosLock = FALSE;
}

void CChaosBox::LogPlusItemLevelChaosItem(LPOBJ lpObj, int iPlusMixLevel)
{
	int iMixLevel = 0;
	BYTE ExOption[MAX_EXOPTION_SIZE];

	if ( iPlusMixLevel == 3 )
		iMixLevel = 1;
	else if ( iPlusMixLevel == 4 )
		iMixLevel = 2;
	else if ( iPlusMixLevel == 22 )
		iMixLevel = 3;
	else if ( iPlusMixLevel == 23 )
		iMixLevel = 4;
	else if ( iPlusMixLevel == 49 )
		iMixLevel = 5;
	else if ( iPlusMixLevel == 50 )
		iMixLevel = 6;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			::ItemIsBufExOption(ExOption, &lpObj->pChaosBox[n]);

			LogAddTD("[PlusItemLevel,%d] [%s][%s] CBMix [%d,%s,%d,%d,%d,%d]serial:[%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d] 380Ex:[%d] HO:[%d,%d]", iMixLevel,
				lpObj->AccountID, lpObj->Name, n, lpObj->pChaosBox[n].GetName(), lpObj->pChaosBox[n].m_Level,
				lpObj->pChaosBox[n].m_Option1, lpObj->pChaosBox[n].m_Option2, lpObj->pChaosBox[n].m_Option3, 
				lpObj->pChaosBox[n].m_Number, (int)lpObj->pChaosBox[n].m_Durability, 
				ExOption[0], ExOption[1], ExOption[2], ExOption[3], ExOption[4], ExOption[5], ExOption[6], lpObj->pChaosBox[n].m_SetOption,
				lpObj->pChaosBox[n].m_ItemOptionEx >> 7,
				g_kJewelOfHarmonySystem.GetItemOptionLevel(&lpObj->pChaosBox[n]),
				g_kJewelOfHarmonySystem.GetItemStrengthenOption(&lpObj->pChaosBox[n]));
		}
	}
}





void CChaosBox::LogChaosItem(LPOBJ lpObj, LPSTR sLogType)
{
	BYTE ExOption[MAX_EXOPTION_SIZE];

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			::ItemIsBufExOption(ExOption, &lpObj->pChaosBox[n]);

			LogAddTD("[%s] [%s][%s] CBMix [%d,%s,%d,%d,%d,%d]serial:[%u][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d] 380:[%d] HO:[%d,%d] Socket[%d,%d,%d,%d,%d]",
				sLogType, lpObj->AccountID, lpObj->Name, n, lpObj->pChaosBox[n].GetName(), lpObj->pChaosBox[n].m_Level,
				lpObj->pChaosBox[n].m_Option1, lpObj->pChaosBox[n].m_Option2, lpObj->pChaosBox[n].m_Option3, 
				lpObj->pChaosBox[n].m_Number, (int)lpObj->pChaosBox[n].m_Durability, 
				ExOption[0], ExOption[1], ExOption[2], ExOption[3], ExOption[4], ExOption[5], ExOption[6], lpObj->pChaosBox[n].m_SetOption,
				lpObj->pChaosBox[n].m_ItemOptionEx >> 7,
				g_kJewelOfHarmonySystem.GetItemStrengthenOption(&lpObj->pChaosBox[n]),
				g_kJewelOfHarmonySystem.GetItemOptionLevel(&lpObj->pChaosBox[n]),
				lpObj->pChaosBox[n].m_SocketOption[0],lpObj->pChaosBox[n].m_SocketOption[1],lpObj->pChaosBox[n].m_SocketOption[2],lpObj->pChaosBox[n].m_SocketOption[3],lpObj->pChaosBox[n].m_SocketOption[4]);
		}
	}
}






BOOL CChaosBox::PlusItemLevelChaosMix(LPOBJ lpObj, int mixType)
{
	int ChaosGemCount = 0;
	int BlessGemCount = 0;
	int SoulGemCount = 0;
	int Plus9ItemCount = 0;
	int Plus10ItemCount = 0;
	int PlusItemPos = -1;
	int OtherItemFound = 0;
	int Plus11ItemCount = 0;
	int Plus12ItemCount = 0;
	int Plus13ItemCount = 0;
	int Plus14ItemCount = 0;
	int ExtraBlessGemCount = 0;
	int ExtraSoulGemCount = 0;
	int iChristmasItem = 0;
	int iCharmOfLuckCount = 0;
	BYTE btItemLuckValue = 0;
	
	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				ChaosGemCount++;
			}
			else if (  lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) )
			{
				BlessGemCount++;
			}
			else if (  lpObj->pChaosBox[n].m_Type == ITEMGET(14,14) )
			{
				SoulGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,96) )
			{
				iChristmasItem += 1;
			}
			else if ( lpObj->pChaosBox[n].m_Level == 9 )
			{
				Plus9ItemCount++;
				PlusItemPos = n;
			}
			else if ( lpObj->pChaosBox[n].m_Level == 10 )
			{
				Plus10ItemCount++;
				PlusItemPos = n;
			}
			else if ( lpObj->pChaosBox[n].m_Level == 11 )
			{
				Plus11ItemCount++;
				PlusItemPos = n;
			}
			else if ( lpObj->pChaosBox[n].m_Level == 12 )
			{
				Plus12ItemCount++;
				PlusItemPos = n;
			}
			else if ( lpObj->pChaosBox[n].m_Level == 13 )
			{
				Plus13ItemCount++;
				PlusItemPos = n;
			}
			else if ( lpObj->pChaosBox[n].m_Level == 14 )
			{
				Plus14ItemCount++;
				PlusItemPos = n;
			}
			else
			{
				OtherItemFound ++;
			}
		}
	}

	if ( iCharmOfLuckCount > 10 )
	{
		PMSG_CHAOSMIXRESULT pResult;

		PHeadSetB((LPBYTE)&pResult, 0x86, sizeof(pResult));

		LogAddTD("[PlusItemLevel] [%s][%s] CBMix Charm of luck over 10% (%d)",
			lpObj->AccountID, lpObj->Name, iCharmOfLuckCount);

		pResult.Result = 0xF0;

		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		lpObj->ChaosLock = FALSE;

		return 0;
	}

	if ( OtherItemFound != FALSE )
	{
		return FALSE;
	}

	if ( iChristmasItem > 1 )
	{
		PMSG_CHAOSMIXRESULT pResult;
		PHeadSetB((LPBYTE)&pResult, 0x86, sizeof(pResult));

		pResult.Result = 7;

		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		lpObj->ChaosLock = FALSE;
		return 0;
	}

	lpObj->ChaosMoney = 0;
	int MixType = 0;

	if ( Plus9ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 1 && SoulGemCount == 1 && Plus10ItemCount == 0 && Plus11ItemCount == 0 && Plus12ItemCount == 0 && Plus13ItemCount == 0 && Plus14ItemCount == 0 )
	{
		MixType = CHAOS_TYPE_UPGRADE_10;
		lpObj->ChaosMoney = 2000000;
		ExtraBlessGemCount = BlessGemCount - 1;
		ExtraSoulGemCount = SoulGemCount - 1;
	}
	else if ( Plus10ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 2 && SoulGemCount == 2 && Plus9ItemCount == 0 && Plus11ItemCount == 0 && Plus12ItemCount == 0 && Plus13ItemCount == 0 && Plus14ItemCount == 0 )
	{
		MixType = CHAOS_TYPE_UPGRADE_11;
		lpObj->ChaosMoney = 4000000;
		ExtraBlessGemCount = BlessGemCount - 2;
		ExtraSoulGemCount = SoulGemCount - 2;
	}
	else if ( Plus11ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 3 && SoulGemCount == 3 && Plus9ItemCount == 0 && Plus10ItemCount == 0 && Plus12ItemCount == 0 && Plus13ItemCount == 0 && Plus14ItemCount == 0 )
	{
		MixType = CHAOS_TYPE_UPGRADE_12;
		lpObj->ChaosMoney = 6000000;
		ExtraBlessGemCount = BlessGemCount - 3;
		ExtraSoulGemCount = SoulGemCount - 3;
	}
	else if ( Plus12ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 4 && SoulGemCount == 4 && Plus9ItemCount == 0 && Plus10ItemCount == 0 && Plus11ItemCount == 0 && Plus13ItemCount == 0 && Plus14ItemCount == 0 )
	{
		MixType = CHAOS_TYPE_UPGRADE_13;
		lpObj->ChaosMoney = 8000000;
		ExtraBlessGemCount = BlessGemCount - 4;
		ExtraSoulGemCount = SoulGemCount - 4;
	}
	else if ( Plus13ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 5 && SoulGemCount == 5 && Plus9ItemCount == 0 && Plus10ItemCount == 0 && Plus11ItemCount == 0 && Plus12ItemCount == 0 && Plus14ItemCount == 0)
	{
		MixType = CHAOS_TYPE_UPGRADE_14;
		lpObj->ChaosMoney = 10000000;
		ExtraBlessGemCount = BlessGemCount - 5;
		ExtraSoulGemCount = SoulGemCount - 5;
	}
	else if ( Plus14ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 6 && SoulGemCount == 6 && Plus9ItemCount == 0 && Plus10ItemCount == 0 && Plus11ItemCount == 0 && Plus12ItemCount == 0 && Plus13ItemCount == 0)
	{
		MixType = CHAOS_TYPE_UPGRADE_15;
		lpObj->ChaosMoney = 12000000;
		ExtraBlessGemCount = BlessGemCount - 6;
		ExtraSoulGemCount = SoulGemCount - 6;
	}

	if ( MixType != mixType )
	{
		MixType = 0;
	}

	if ( MixType == 0 )
	{
		return FALSE;
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	lpObj->ChaosLock = TRUE;
	LogPlusItemLevelChaosItem(lpObj, MixType);
	LogAddTD("[PlusItemLevel] Chaos Mix Start");

	int iChaosTaxMoney = (int)((__int64)lpObj->ChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	lpObj->ChaosMoney += iChaosTaxMoney;

	if ( lpObj->ChaosMoney < 0 )
	{
		lpObj->ChaosMoney = 0;
	}

	if ( lpObj->Money < lpObj->ChaosMoney )
	{
		LogAddTD("[PlusItemLevel] [%s][%s] CBMix Not Enough Money [%d] need zen [%d]",
			lpObj->AccountID, lpObj->Name, lpObj->Money, lpObj->ChaosMoney);
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		
		return TRUE;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	BYTE btRateType = 0;

	if( lpObj->pChaosBox[PlusItemPos].IsExtItem() == TRUE ||
		lpObj->pChaosBox[PlusItemPos].IsSetItem() == TRUE ||
		g_kItemSystemFor380.Is380Item(&lpObj->pChaosBox[PlusItemPos]) == TRUE )
	{
		btRateType = 2;
	}
	else
	{
		btRateType = 1;
	}

	if( g_SocketItem.CheckSocketOption(&lpObj->pChaosBox[PlusItemPos]) == TRUE )
	{
		btRateType = 3;
	}

	if( ( lpObj->pChaosBox[PlusItemPos].m_Type >= ITEMGET(12,0) && lpObj->pChaosBox[PlusItemPos].m_Type <= ITEMGET(12,6) ) ||
		( lpObj->pChaosBox[PlusItemPos].m_Type >= ITEMGET(12,36) && lpObj->pChaosBox[PlusItemPos].m_Type <= ITEMGET(12,43) ) ||
		( lpObj->pChaosBox[PlusItemPos].m_Type >= ITEMGET(12,262) && lpObj->pChaosBox[PlusItemPos].m_Type <= ITEMGET(12,267) ) || 
		( lpObj->pChaosBox[PlusItemPos].m_Type == ITEMGET(13,30) || 
		  lpObj->pChaosBox[PlusItemPos].m_Type == ITEMGET(12,49) ||
		  lpObj->pChaosBox[PlusItemPos].m_Type == ITEMGET(12,50) )
#if (CUSTOM_WINGS == 1)
		|| lpObj->pChaosBox[PlusItemPos].m_Type >= ITEMGET(12,440) && lpObj->pChaosBox[PlusItemPos].m_Type <= ITEMGET(12,445) 
#endif
		)
	{
		btRateType = 1;
	}

#if (__CUSTOM__ == 1)
	
	if( lpObj->pChaosBox[PlusItemPos].m_Level == 9 || lpObj->pChaosBox[PlusItemPos].m_Level == 10 )
	{
		if( btRateType == 1 )
		{
			lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_1_1;
		}
		else if( btRateType == 2 )
		{
			lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_2_1;
		}
		else if( btRateType == 3 )
		{
			lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_3_1;
		}
	}
	else if( lpObj->pChaosBox[PlusItemPos].m_Level == 11 || lpObj->pChaosBox[PlusItemPos].m_Level == 12 )
	{
		if( btRateType == 1 )
		{
			lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_1_2;
		}
		else if( btRateType == 2 )
		{
			lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_2_2;
		}
		else if( btRateType == 3 )
		{
			lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_3_2;
		}
	}
	else if( lpObj->pChaosBox[PlusItemPos].m_Level == 13 || lpObj->pChaosBox[PlusItemPos].m_Level == 14 )
	{
		if( btRateType == 1 )
		{
			lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_1_3;
		}
		else if( btRateType == 2 )
		{
			lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_2_3;
		}
		else if( btRateType == 3 )
		{
			lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_3_3;
		}
	}
	else
	{
		lpObj->ChaosSuccessRate = 45;
	}

	if( lpObj->pChaosBox[PlusItemPos].m_Option2 )
	{
		lpObj->ChaosSuccessRate += gc_ChaosMixPlusItemLevel_Luck;
	}

	if( lpObj->ChaosSuccessRate > gc_ChaosMixPlusItemLevel_Max )
	{
		lpObj->ChaosSuccessRate = gc_ChaosMixPlusItemLevel_Max;
	}

#else
	
	if( lpObj->pChaosBox[PlusItemPos].m_Level == 9 || lpObj->pChaosBox[PlusItemPos].m_Level == 10 )
	{
		if( btRateType == 1 )
		{
			lpObj->ChaosSuccessRate = 60;
		}
		else if( btRateType == 2 )
		{
			lpObj->ChaosSuccessRate = 50;
		}
		else if( btRateType == 3 )
		{
			lpObj->ChaosSuccessRate = 40;
		}
	}
	else if( lpObj->pChaosBox[PlusItemPos].m_Level == 11 || lpObj->pChaosBox[PlusItemPos].m_Level == 12 )
	{
		if( btRateType == 1 )
		{
			lpObj->ChaosSuccessRate = 55;
		}
		else if( btRateType == 2 )
		{
			lpObj->ChaosSuccessRate = 45;
		}
		else if( btRateType == 3 )
		{
			lpObj->ChaosSuccessRate = 35;
		}
	}
	else if( lpObj->pChaosBox[PlusItemPos].m_Level == 13 || lpObj->pChaosBox[PlusItemPos].m_Level == 14 )
	{
		if( btRateType == 1 )
		{
			lpObj->ChaosSuccessRate = 20;
		}
		else if( btRateType == 2 )
		{
			lpObj->ChaosSuccessRate = 10;
		}
		else if( btRateType == 3 )
		{
			lpObj->ChaosSuccessRate = 5;
		}
	}
	else
	{
		lpObj->ChaosSuccessRate = 45;
	}

	if( lpObj->pChaosBox[PlusItemPos].m_Option2 )
	{
		lpObj->ChaosSuccessRate += 20;
	}

	if( lpObj->ChaosSuccessRate > 75 )
	{
		lpObj->ChaosSuccessRate = 75;
	}

#endif

	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		lpObj->pChaosBox[PlusItemPos].m_Level++;
		pMsg.Result = CB_SUCCESS;

		CItem Item;

		float Dur = (float)ItemGetDurability(lpObj->pChaosBox[PlusItemPos].m_Type, lpObj->pChaosBox[PlusItemPos].m_Level,
			lpObj->pChaosBox[PlusItemPos].IsExtItem(), lpObj->pChaosBox[PlusItemPos].IsSetItem());

		Item.m_Level = lpObj->pChaosBox[PlusItemPos].m_Level;
		Item.m_Durability = Dur * lpObj->pChaosBox[PlusItemPos].m_Durability / lpObj->pChaosBox[PlusItemPos].m_BaseDurability;
		Item.m_JewelOfHarmonyOption = lpObj->pChaosBox[PlusItemPos].m_JewelOfHarmonyOption;
		Item.m_bLOCKED = lpObj->pChaosBox[PlusItemPos].m_bLOCKED;

		BYTE SocketOptions[MAX_SOCKET_COUNT];
		BYTE SocketBonus;

		if( g_SocketItem.IsSocketItem(lpObj->pChaosBox[PlusItemPos].m_Type) )
		{
			g_SocketItem.GetItemOptions(&lpObj->pChaosBox[PlusItemPos],&SocketOptions[0],&SocketBonus);
		}
		else if( g_ElementalSystem.IsPentagram(lpObj->pChaosBox[PlusItemPos].m_Type) )
		{
			SocketBonus = lpObj->pChaosBox[PlusItemPos].m_SocketBonus;
			for(int i = 0; i < 5; i++)	{
				SocketOptions[i] = lpObj->pChaosBox[PlusItemPos].m_SocketOption[i];
			}
		}

		Item.Convert(lpObj->pChaosBox[PlusItemPos].m_Type, lpObj->pChaosBox[PlusItemPos].m_Option1, lpObj->pChaosBox[PlusItemPos].m_Option2,
			lpObj->pChaosBox[PlusItemPos].m_Option3, lpObj->pChaosBox[PlusItemPos].m_NewOption, 
			lpObj->pChaosBox[PlusItemPos].m_SetOption, lpObj->pChaosBox[PlusItemPos].m_ItemOptionEx, SocketOptions, SocketBonus, CURRENT_DB_VERSION);

		Item.m_Number = lpObj->pChaosBox[PlusItemPos].m_Number;

		ItemByteConvert(pMsg.ItemInfo, Item);
		ChaosBoxInit(lpObj);
		::gObjChaosBoxInsertItemPos(lpObj->m_Index, Item, 0, -1);
		gObjChaosItemSet(lpObj->m_Index, 0, 1);

		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);

		if ( btItemLuckValue )
		{
			LogAddTD("[PlusItemLevel] [%s][%s] CBMix Success %d Money : %d-%d [%d], CharmRate : %d",
				lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate+5, lpObj->Money,
				lpObj->ChaosMoney, Item.m_Level, iCharmOfLuckCount);
		}
		else
		{
			LogAddTD("[PlusItemLevel] [%s][%s] CBMix Success %d Money : %d-%d [%d], CharmRate : %d",
				lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money,
				lpObj->ChaosMoney, Item.m_Level, iCharmOfLuckCount);
		}

#if( __4GAMERS__ == 1 )
		g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
	}
	else
	{
#if( __4GAMERS__ == 1 )
		g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		int FailLevel = lpObj->pChaosBox[PlusItemPos].m_Level+1;

		if( iChristmasItem != 0 )
		{
			ChaosBoxSpecialItemDown(lpObj);
			GCUserChaosBoxSend(lpObj, 0);
			DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		}
		else
		{
			ChaosBoxInit(lpObj);
			GCUserChaosBoxSend(lpObj, 0);
			DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		}

		if ( btItemLuckValue )
		{
			LogAddTD("[PlusItemLevel] [%s][%s] CBMix Fail %d Money : %d-%d [%d], CharmRate : %d",
				lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate+5, lpObj->Money,
				lpObj->ChaosMoney, FailLevel, iCharmOfLuckCount);
		}
		else
		{
			LogAddTD("[PlusItemLevel] [%s][%s] CBMix Fail %d Money : %d-%d [%d], CharmRate : %d",
				lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money,
				lpObj->ChaosMoney, FailLevel, iCharmOfLuckCount);
		}
	}

	lpObj->ChaosLock = FALSE;
return true;
}

void CChaosBox::ChaosBoxSpecialItemDown(LPOBJ lpObj)
{
	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			lpObj->pChaosBox[n].m_Level = 0;
			int iType = lpObj->pChaosBox[n].m_Type;

			if( IsPlusItemJewel(iType) == TRUE )
			{
				lpObj->pChaosBox[n].Clear();
				lpObj->pChaosBoxMap[n] = 0xFF;
			}
		}
	}
}

int CChaosBox::IsPlusItemJewel(int iType)
{
	switch( iType )
	{
	case ITEMGET(14, 53):
	case ITEMGET(12, 15):
	case ITEMGET(14, 13):
	case ITEMGET(14, 14):
	case ITEMGET(14, 96):
	case ITEMGET(14, 189):
	case ITEMGET(14, 190):
		return TRUE;
	default:
		return FALSE;
	}
}


BOOL CChaosBox::PegasiaChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int UniriaCount = 0;
	int ChoasGemCount = 0;
	int iCharmOfLuckCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->pChaosBox[n].IsItem() == TRUE)
		{
			if (lpObj->pChaosBox[n].m_Type == ITEMGET(13, 2) && lpObj->pChaosBox[n].m_Durability == 255.0f)	// Uniria
			{
				UniriaCount++;
			}
			else if (lpObj->pChaosBox[n].m_Type == ITEMGET(12, 15)) // Chaos Gem
			{
				ChoasGemCount++;
			}
			else if (lpObj->pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
		}
	}

	if (UniriaCount != 10 || ChoasGemCount != 1)
	{
		lpObj->ChaosLock = FALSE;
		return FALSE;
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if ( iCharmOfLuckCount > 10 )
	{
		lpObj->ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

		return 0;
	}

	int nChaosNeedMoney = 500000;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		lpObj->ChaosLock = FALSE;
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return TRUE;
	}

	LogChaosItem(lpObj, "DinorantMix");
	LogAddTD("[DinorantMix] Chaos Mix Start");
	lpObj->ChaosSuccessRate = 70;	// Succes Rate for Dinorant
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;
	lpObj->Money -= nChaosNeedMoney;

	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100 ) < lpObj->ChaosSuccessRate )
	{
		int Dinorant = ITEMGET(13,3);
		int Option3 = 0;

		if ( (rand()% 100) < 30 )
		{
			Option3 = 1 <<  ((rand()%3)) ;

			if ( (rand()%5) == 0 )
			{
				Option3 |= 1 << (rand()%3);
			}
		}

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, Dinorant, 0, 255, 1, 0, Option3, -1, 0, 0);
		gObjInventoryCommit(lpObj->m_Index);

		LogAddTD("[DinorantMix] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);

#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif

		return TRUE;
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);	// Errase Chaos Box
		GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);

		LogAddTD("[DinorantMix] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);

		lpObj->ChaosLock = FALSE;
		return FALSE;
	}
}






BOOL CChaosBox::CircleChaosMix(LPOBJ lpObj)	// Fruits
{
	lpObj->ChaosLock = TRUE;

	int CreatureGemCount = 0;
	int ChoasGemCount = 0;
	int iCharmOfLuckCount = 0;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,22) )	// Jewel of Creation
			{
				CreatureGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) ) // Chaos Gem
			{
				ChoasGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
		}
	}

	if ( CreatureGemCount != 1 || ChoasGemCount != 1 )
	{
		return FALSE;
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if ( iCharmOfLuckCount > 10 )
	{
		lpObj->ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

		return 1;
	}

	if ( lpObj->Level < 10 )
	{
		pMsg.Result = CB_LOW_LEVEL_USER;

		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return TRUE;
	}

	int nChaosNeedMoney = 3000000;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		lpObj->ChaosLock = FALSE;
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return TRUE;
	}

	LogChaosItem(lpObj, "CircleMix");
	LogAddTD("[CircleMix] Chaos Mix Start");
	lpObj->ChaosSuccessRate = 90;	// Succes Rate for Fruit
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;
	lpObj->Money -= nChaosNeedMoney;

	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100 ) < lpObj->ChaosSuccessRate )
	{
		int Fruit = ITEMGET(13,15);	// Fruit
		int FruitType;
		int RandonValue = rand() % 100;

		if ( RandonValue < 30 )
		{
			FruitType = 0;
		}
		else if ( RandonValue < 55 )
		{
			FruitType = 1;
		}
		else if ( RandonValue < 75 )
		{
			FruitType = 2;
		}
		else if ( RandonValue < 95 )
		{
			FruitType = 3;
		}
		else
		{
			FruitType = 4;
		}

		::ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, Fruit, FruitType, 255, 1, 0, 0, -1, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);

		LogAddTD("[CircleMix] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		return TRUE;
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj, 0);

		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		LogAddTD("[CircleMix] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
		lpObj->ChaosLock = FALSE;

		return FALSE;
	}
}



BOOL CChaosBox::WingChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	int WingCount = 0;
	int ChoasGemCount = 0;
	int LokesFeathersCount = 0;
	int WingIndex = -1;
	int iChaosMoney = 0;
	int WingIndex2 = -1;
	int iWingChaosMoney = 0;
	int iSleeveOfLord = 0;
	int iCharmOfLuckCount = 0;

	int n;
	for ( n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( (lpObj->pChaosBox[n].m_Type >= ITEMGET(12,3) && lpObj->pChaosBox[n].m_Type <= ITEMGET(12,6)) || lpObj->pChaosBox[n].m_Type == ITEMGET(12,42) || lpObj->pChaosBox[n].m_Type == ITEMGET(14,30) )	
			{
				lpObj->ChaosLock = FALSE;

				return FALSE;
			}
			if ( lpObj->pChaosBox[n].m_Type >= ITEMGET(12,0) && lpObj->pChaosBox[n].m_Type <= ITEMGET(12,2) ) 
			{
				WingCount++;
				WingIndex = n;
				iWingChaosMoney = lpObj->pChaosBox[n].m_BuyMoney;
			}
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,41) )
			{
				WingCount++;
				WingIndex = n;
				iWingChaosMoney = lpObj->pChaosBox[n].m_BuyMoney;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) ) // Chaos
			{
				ChoasGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,14) ) // Feather
			{
				if ( lpObj->pChaosBox[n].m_Level == 0 ) // Feather
				{
					LokesFeathersCount++;
				}
				else	// Crst of Monarch
				{
					iSleeveOfLord ++;
				}
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,88) ||
				      lpObj->pChaosBox[n].m_Type == ITEMGET(13,89) ||
					  lpObj->pChaosBox[n].m_Type == ITEMGET(13,90) ||
					  lpObj->pChaosBox[n].m_Type == ITEMGET(13,91) ||
					  lpObj->pChaosBox[n].m_Type == ITEMGET(13,92) )
			{
				WingIndex2 = lpObj->pChaosBox[n].m_Type;
			}
			else if ( lpObj->pChaosBox[n].IsExtItem() != FALSE )
			{
				if ( lpObj->pChaosBox[n].m_Level >= 4 )
				{
					iChaosMoney += lpObj->pChaosBox[n].m_BuyMoney;
				}
			}
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	

	if ( iSleeveOfLord == 1 )
	{
		if ( WingCount != 1 || ChoasGemCount != 1 || LokesFeathersCount != 0 )
		{
			lpObj->ChaosLock = FALSE;
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
			return FALSE;
		}
	}
	else if ( LokesFeathersCount == 1 )
	{
		if ( WingCount != 1 || ChoasGemCount != 1 || iSleeveOfLord != 0 )
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
			lpObj->ChaosLock = FALSE;

			return FALSE;
		}
	}
	else
	{
		lpObj->ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);

		return FALSE;
	}

	if ( iCharmOfLuckCount > 10 )
	{
		lpObj->ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
	}

	int nChaosNeedMoney = 5000000;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return TRUE;
	}

	CItem * pWing = &lpObj->pChaosBox[WingIndex];
	lpObj->ChaosSuccessRate = (DWORD)((DWORD)iWingChaosMoney / (DWORD)4000000);
	lpObj->ChaosSuccessRate += iChaosMoney / 40000;

	if ( lpObj->ChaosSuccessRate < 0 )
	{
		lpObj->ChaosSuccessRate = 100;
	}

	if ( lpObj->ChaosSuccessRate == 0 )
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return FALSE;
	}

	LogChaosItem(lpObj, "WingMix,2");
	LogAddTD("[WingMix,2] Chaos Mix Start");

	if ( lpObj->ChaosSuccessRate > 90 )
	{
		lpObj->ChaosSuccessRate = 90;
	}

	lpObj->ChaosSuccessRate += iCharmOfLuckCount;
	lpObj->Money  -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int iWingLevel = 0;
		int iItemType;
		int iItemSubType;

		if ( iSleeveOfLord != 0 )
		{
			if( rand()%2 == TRUE )
			{
				iItemType = 12;
				iItemSubType = 49;
			}
			else
			{
				iItemType = 13;
				iItemSubType = 30;
			}
		}
		else
		{
			iItemType = 12;
			int iRand = rand()%5;

			if( iRand == 4 )
			{
				iItemSubType = 42;
			}
			else
			{
				iItemSubType = iRand + 3;
			}
		}

		if( WingIndex2 > -1 )
		{
			int WingCharm = WingIndex2^ITEMGET(13,0);

			switch( WingCharm )
			{
			case 88:
				iItemSubType = 5;
				break;
			case 89:
				iItemSubType = 4;
				break;
			case 90:
				iItemSubType = 3;
				break;
			case 91:
				iItemSubType = 42;
				break;
			case 92:
				iItemSubType = 6;
				break;
			default:
				LogAddTD("[MixSystem][WingChaosMix] WingCharmItemNum => %d , WingCharmIndex => %d",
					WingIndex2,WingCharm);
				break;
			}
		}

		int iWingNum = ITEMGET(iItemType, iItemSubType);
		int iOption1 = 0;
		int iOption2 = 0;
		int iOption3 = 0;

		if ( (rand()%5) == 0 )
		{
			iOption1 = 1;
		}

		int iRandomValue = rand() % 100;
		int iRandomValue2 = rand() % 3;

		switch ( iRandomValue2 )
		{
			case 0:
				if ( iRandomValue < 4 )
				{
					iOption2 = 3;	// +12
				}
				break;

			case 1:
				if ( iRandomValue < 10 )
				{
					iOption2 = 2;	// +8
				}
				break;

			case 2:
				if ( iRandomValue < 20 )
				{
					iOption2 = 1;	// +4;
				}
		}

		//int ExOption;

		if ( iSleeveOfLord != FALSE )
		{
			if ( (rand()%5) == 0 )
			{
				iOption3 = 1 << (rand()%4);
			}
			
			iOption3 |= 0x20;
		}
		else
		{
			if ( (rand()%5) == 0 )
			{
				iOption3 = 1 << (rand()%3);
			}

			if ( (rand()%2) != 0 )
			{
				iOption3 |= 0x20;
			}
		}

		::ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, iWingNum, iWingLevel, 0, 0, iOption1, iOption2, -1, iOption3, 0);
		::gObjInventoryCommit(lpObj->m_Index);
		::LogAddTD("[WingMix,2] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		return TRUE;
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		for ( n=0;n<CHAOS_BOX_SIZE;n++)
		{
			lpObj->pChaosBox[n].Clear();
		}

		GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		::LogAddTD("[WingMix,2] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
		
		lpObj->ChaosLock = FALSE;

		return FALSE;
	}
}

void CChaosBox::BloodCastleItemChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	int aIndex = lpObj->m_Index;

	int iRET_VAL = g_BloodCastle.CheckChoasMixItem(aIndex);

	int iMapNumber = g_BloodCastle.GetEventMap(iRET_VAL-1);

	if ( BC_MAP_RANGE(iMapNumber) != FALSE )
	{
		if ( g_BloodCastle.BloodCastleChaosMix(aIndex, iRET_VAL) == false )
		{
			lpObj->ChaosLock = FALSE;
		}

		return;
	}

	switch ( iRET_VAL )
	{
		case 9:
			pMsg.Result = CB_NO_BC_CORRECT_ITEMS	;
			DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
			lpObj->m_Index;
			lpObj->ChaosLock = FALSE;
			break;

		case 10:
			pMsg.Result = CB_NO_BC_CORRECT_ITEMS	;
			DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
			lpObj->m_Index;
			lpObj->ChaosLock = FALSE;
			break;

		case 11:
			pMsg.Result = CB_NO_BC_CORRECT_ITEMS	;
			DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
			lpObj->m_Index;
			lpObj->ChaosLock = FALSE;
			break;

		case 12:
			pMsg.Result = CB_INVALID_ITEM_LEVEL	;
			DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
			lpObj->m_Index;
			lpObj->ChaosLock = FALSE;
			break;

		case 13:
			pMsg.Result = CB_BC_NOT_ENOUGH_ZEN	;
			DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
			lpObj->m_Index;
			lpObj->ChaosLock = FALSE;
			break;

		case 14:
			pMsg.Result = CB_USER_CLASS_LOW_LEVEL	;
			DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
			lpObj->m_Index;
			lpObj->ChaosLock = FALSE;
			break;
		case 15:
			pMsg.Result = 0xF0;
			DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
			lpObj->ChaosLock = FALSE;
			break;
		default:
			lpObj->ChaosLock = FALSE;
	}

}




void CChaosBox::SetItemChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	int ChoasGemCount = 0;
	int BlessGemCount = 0;
	int SoulGemCount = 0;
	int MetalOfAncientCount = 0;
	int MixItemCount = 0;
	int MixSetItemIndex = 0;
	int iMixItemChaosMoney = 0;
	int iChaosMoney = 0;
	int iMetalOfAncientItemLevel = -1;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;


	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) )
			{
				BlessGemCount++;

				if ( BlessGemCount > 3 )
				{
					iChaosMoney += lpObj->pChaosBox[n].m_BuyMoney;
				}
			}

			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,14) )
			{
				SoulGemCount++;

				if ( SoulGemCount > 3 )
				{
					iChaosMoney += lpObj->pChaosBox[n].m_BuyMoney;
				}
			}

			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,16) )
			{
				iChaosMoney += lpObj->pChaosBox[n].m_BuyMoney;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				ChoasGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Level >= 6 && lpObj->pChaosBox[n].IsExtItem() != FALSE)
			{
				if ( gSetItemOption.IsSetItem(lpObj->pChaosBox[n].m_Type) != FALSE )
				{
					MixItemCount++;
					MixSetItemIndex = n;
					iMixItemChaosMoney += lpObj->pChaosBox[n].m_BuyMoney;
				}
			}
			else if ( ((lpObj->pChaosBox[n].m_Type >= ITEMGET(13,8) && lpObj->pChaosBox[n].m_Type < ITEMGET(13,14)) || (lpObj->pChaosBox[n].m_Type >= ITEMGET(13,21) && lpObj->pChaosBox[n].m_Type < ITEMGET(13,28) ) ) && ( lpObj->pChaosBox[n].m_Option3 >= 2  && lpObj->pChaosBox[n].m_Level >= 3 ) )	
			{
				if ( gSetItemOption.IsSetItem(lpObj->pChaosBox[n].m_Type ) != FALSE )
				{
					MixItemCount++;
					MixSetItemIndex = n;
					iMixItemChaosMoney += lpObj->pChaosBox[n].m_BuyMoney;
				}
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,27) ) // Ancien Metal, Apply Deathway Fix here
			{
				MetalOfAncientCount++;
				iMetalOfAncientItemLevel = lpObj->pChaosBox[n].m_Level;
			}
			else
			{
				DataSend(lpObj->m_Index, (BYTE*)&pMsg, pMsg.h.size);
				lpObj->ChaosLock = FALSE;
			}
		}
	}


	if ( ChoasGemCount < 1 || BlessGemCount < 3 || SoulGemCount < 3 || MetalOfAncientCount != 1 || MixItemCount != 1 )
	{
		DataSend(lpObj->m_Index, (BYTE*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "SetItemMix");
	LogAddTD("[SetItemMix] Chaos Mix Start");

	if ( gSetItemOption.CheckMixContition(lpObj->pChaosBox[MixSetItemIndex].m_Type, iMetalOfAncientItemLevel ) == FALSE )
	{
		DataSend(lpObj->m_Index, (BYTE*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->ChaosSuccessRate = iMixItemChaosMoney / 40000;
	lpObj->ChaosSuccessRate = iChaosMoney / 400000;

	if ( lpObj->ChaosSuccessRate > 80 )
	{
		lpObj->ChaosSuccessRate = 80;
	}

	int nChaosNeedMoney = (lpObj->ChaosSuccessRate - 50 ) * 1000000;

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		DataSend(lpObj->m_Index, (BYTE*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	int setoption = gSetItemOption.MakeSetOption(lpObj->pChaosBox[MixSetItemIndex].m_Type, iMetalOfAncientItemLevel);

	if ( setoption == 0 )
	{
		pMsg.Result = CB_ERROR;
		DataSend(lpObj->m_Index, (BYTE*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		lpObj->pChaosBox[MixSetItemIndex].m_SetOption = setoption;

		if ( (rand()%100 ) < 80 )
		{
			lpObj->pChaosBox[MixSetItemIndex].m_SetOption |= 4;
		}
		else
		{
			lpObj->pChaosBox[MixSetItemIndex].m_SetOption |= 8;
		}

		pMsg.Result = CB_SUCCESS;
		CItem Item;

		float Dur = (float)ItemGetDurability(lpObj->pChaosBox[MixSetItemIndex].m_Type, lpObj->pChaosBox[MixSetItemIndex].m_Level,
			lpObj->pChaosBox[MixSetItemIndex].IsExtItem(), lpObj->pChaosBox[MixSetItemIndex].IsSetItem());

		Item.m_Level = lpObj->pChaosBox[MixSetItemIndex].m_Level;
		Item.m_Durability = Dur * lpObj->pChaosBox[MixSetItemIndex].m_Durability / lpObj->pChaosBox[MixSetItemIndex].m_BaseDurability;
		Item.m_JewelOfHarmonyOption = lpObj->pChaosBox[MixSetItemIndex].m_JewelOfHarmonyOption;
		Item.m_bLOCKED = lpObj->pChaosBox[MixSetItemIndex].m_bLOCKED;

		Item.Convert(lpObj->pChaosBox[MixSetItemIndex].m_Type, lpObj->pChaosBox[MixSetItemIndex].m_Option1, lpObj->pChaosBox[MixSetItemIndex].m_Option2,
			lpObj->pChaosBox[MixSetItemIndex].m_Option3, lpObj->pChaosBox[MixSetItemIndex].m_NewOption, 
			lpObj->pChaosBox[MixSetItemIndex].m_SetOption, lpObj->pChaosBox[MixSetItemIndex].m_ItemOptionEx, 0, -1, CURRENT_DB_VERSION);

		Item.m_Number = lpObj->pChaosBox[MixSetItemIndex].m_Number;

		ItemByteConvert(pMsg.ItemInfo, Item);
		ChaosBoxInit(lpObj);
		::gObjChaosBoxInsertItemPos(lpObj->m_Index, Item, 0, -1);
		gObjChaosItemSet(lpObj->m_Index, 0, 1);

		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[SetItemMix] [%s][%s] CBMix Success %d Money : %d-%d (SetName:%s)", lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate,
			lpObj->Money, lpObj->ChaosMoney, gSetItemOption.GetSetOptionName(lpObj->pChaosBox[MixSetItemIndex].m_Type, setoption));
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj, 0);

		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);

		LogAddTD("[SetItemMix] [%s][%s] CBMix Fail %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money,
			nChaosNeedMoney);

		lpObj->ChaosLock = FALSE;
	}
}



void CChaosBox::DarkHorseChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int ChoasGemCount = 0;
	int BlessGemCount = 0;
	int SoulGemCount = 0;
	int CreatureGemCount = 0;
	int SoulOfDarkHorse = 0;
	int iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	int iCharmOfLuckCount = 0;
	int iInvalidItemCount = 0;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) )
			{
				BlessGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,14) )
			{
				SoulGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,22) )	// Creation
			{
				CreatureGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )	// SUPE CHOAS HAHAHA
			{
				ChoasGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,31) && lpObj->pChaosBox[n].m_Level == 0 )	// Spirit
			{
				SoulOfDarkHorse++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
			

		}
	}

	if ( ChoasGemCount != 1 || BlessGemCount != 5 || SoulGemCount != 5 || CreatureGemCount != 1 || SoulOfDarkHorse != 1 || iInvalidItemCount > 0 )
	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return ;
	}

	LogChaosItem(lpObj, "DarkHorseMix");
	LogAddTD("[DarkHorseMix] Chaos Mix Start");
	lpObj->ChaosSuccessRate = 60;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	int nChaosNeedMoney = 5000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int Level = 0;
		int ItemType = 13;
		int ItemSubType = 4;
		int ItemNum = ITEMGET(ItemType, ItemSubType);
		int Option1 = 0;
		int Option2 = 0;
		int Option3 = 0;

		int Add = 0;

		if ( (rand()%100) < (lpObj->ChaosSuccessRate/5+6) )
		{
			Add = 1;
		}

		if ( (rand()%5) == 0 )
		{
			Option1 = 1;
		}

		int lc22 = rand()%100;	
		int lc23 = rand()%3;
		//int lc24 = lc23;

		switch ( lc23 )
		{
			case 0:
				if ( lc22 <  4 )
				{
					Option2 = 3;
				}
				break;

			case 1:
				if ( lc22 <  10 )
				{
					Option2 = 2;
				}
				break;

			case 2:
				if ( lc22 <  20 )
				{
					Option2 = 1;
				}
				break;
		}
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		::PetItemSerialCreateSend(lpObj->m_Index, -2, 0, 0, ItemNum, Level, 0,Add,  Option1, Option2, -1, 0, 0); 
		::gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[DarkHorseMix] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate,
			lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj, 1);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[DarkHorseMix] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate,
			lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
		lpObj->ChaosLock = FALSE;
	}
}






void CChaosBox::DarkSpiritChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int ChoasGemCount = 0;
	int BlessGemCount = 0;
	int SoulGemCount = 0;
	int CreatureGemCount = 0;
	int SoulOfSpirit = 0;
	int iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	int iCharmOfLuckCount = 0;
	int iInvalidItemCount = 0;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) )
			{
				BlessGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,14) )
			{
				SoulGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,22) )	// Creation
			{
				CreatureGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )	// SUPE CHOAS HAHAHA
			{
				ChoasGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,31) && lpObj->pChaosBox[n].m_Level == 1 )	// Spirit
			{
				SoulOfSpirit++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( ChoasGemCount != 1 || BlessGemCount != 2 || SoulGemCount != 2 || CreatureGemCount != 1 || SoulOfSpirit != 1 || iInvalidItemCount > 0 )
	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return ;
	}

	LogChaosItem(lpObj, "DarkSpiritMix");
	LogAddTD("[DarkSpiritMix] Chaos Mix Start");
	lpObj->ChaosSuccessRate = 60;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	int nChaosNeedMoney = 1000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int Level = 0;
		int ItemType = 13;
		int ItemSubType = 5;
		int ItemNum = ITEMGET(ItemType, ItemSubType);
		int Option1 = 0;
		int Option2 = 0;
		int Option3 = 0;

		int Add = 0;

		if ( (rand()%100) < (lpObj->ChaosSuccessRate/5+6) )
		{
			Add = 1;
		}

		if ( (rand()%5) == 0 )
		{
			Option1 = 1;
		}

		int lc22 = rand()%100;	
		int lc23 = rand()%3;
//		int lc24 = lc23;

		switch ( lc23 )
		{
			case 0:
				if ( lc22 <  4 )
				{
					Option2 = 3;
				}
				break;

			case 1:
				if ( lc22 <  10 )
				{
					Option2 = 2;
				}
				break;

			case 2:
				if ( lc22 <  20 )
				{
					Option2 = 1;
				}
				break;
		}
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		::PetItemSerialCreateSend(lpObj->m_Index, -2, 0, 0, ItemNum, Level, 0, Add, Option1, Option2, -1, 0, 0); 
		::gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[DarkSpiritMix] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate,
			lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj, 1);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[DarkSpiritMix] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate,
			lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
		lpObj->ChaosLock = FALSE;
	}
}







void CChaosBox::BlessPotionChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int iBlessGemCount = 0;
	int iChaosMoney = 0;
	int iInvalidItemCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) )
			{
				iBlessGemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iBlessGemCount == 0 )
		return;

	if ( iInvalidItemCount > 0 )
		return;

	if ( iBlessGemCount == 0 || iInvalidItemCount > 0 )
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if ( iBlessGemCount > 25 )
	{
		MsgOutput(lpObj->m_Index, lMsg.Get(MSGGET(6,201)));
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "BlessPotionMix");
	LogAddTD("[BlessPotionMix] Chaos Mix Start");
	lpObj->ChaosSuccessRate = 100;

	int nChaosNeedMoney = 100000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int ItemNum = ITEMGET(14,7);
		int Level = 0;
		int Dur = iBlessGemCount * 10;

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, ItemNum, Level, Dur, 0, 0, 0, -1, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[BlessPotionMix] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, 
			lpObj->Money, nChaosNeedMoney);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 1);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[BlessPotionMix] [%s][%s] CBMix Fail %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, 
			lpObj->Money, nChaosNeedMoney);

		lpObj->ChaosLock = FALSE;
	}
}



void CChaosBox::SoulPotionChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int iSoulGemCount = 0;
	int iChaosMoney = 0;
	int iInvalidItemCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,14) )
			{
				iSoulGemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iSoulGemCount == 0 )
		return;

	if ( iInvalidItemCount > 0 )
		return;

	if ( iSoulGemCount > 25 )
	{
		MsgOutput(lpObj->m_Index, lMsg.Get(MSGGET(6,201)));
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "SoulPotionMix");
	LogAddTD("[SoulPotionMix] Chaos Mix Start");
	lpObj->ChaosSuccessRate = 100;

	int nChaosNeedMoney = 50000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int ItemNum = ITEMGET(14,7);
		int Level = 1;
		int Dur = iSoulGemCount * 10;

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, ItemNum, Level, Dur, 0, 0, 0, -1, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[SoulPotionMix] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, 
			lpObj->Money, nChaosNeedMoney);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 1);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[SoulPotionMix] [%s][%s] CBMix Fail %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, 
			lpObj->Money, nChaosNeedMoney);

		lpObj->ChaosLock = FALSE;
	}
}





void CChaosBox::LifeStoneChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int iChoasGemCount = 0;
	int iBlessGemCount = 0;
	int iSoulGemCount = 0;
	int iProtectionGemCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;


	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) )
			{
				iBlessGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,14) )
			{
				iSoulGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,31) )	// Guardian Jewel
			{
				iProtectionGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )	// Habla pe super CHOAS
			{
				iChoasGemCount++;
			}
			else 
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iChoasGemCount != 1 || iBlessGemCount != 5 || iSoulGemCount != 5 || iProtectionGemCount != 1 || iInvalidItemCount > 0 )
	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "LifeStoneMix");
	LogAddTD("[LifeStoneMix] Chaos Mix Start");
	lpObj->ChaosSuccessRate = 100;

	int nChaosNeedMoney = 5000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, ITEMGET(13,11) , 1, 0, 0, 0, 0, -1, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[LifeStoneMix] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, 
			lpObj->Money, nChaosNeedMoney);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 1);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[LifeStoneMix] [%s][%s] CBMix Fail %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, 
			lpObj->Money, nChaosNeedMoney);

		lpObj->ChaosLock = FALSE;
	}
}






void CChaosBox::HiddenTreasureBoxItemMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int iBlueCrystal = 0;
	int iRedCrystal = 0;
	int iBlackCrystal = 0;
	int iInvalidItemCount = 0;
	int iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;


	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,26) )
			{
				switch ( lpObj->pChaosBox[n].m_Level )
				{
					case 1:
						iRedCrystal++;
						break;
					case 2:
						iBlueCrystal++;
						break;
					case 3:
						iBlackCrystal++;
						break;
				}
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	int iCrystalMixType = -1;

	if ( iInvalidItemCount == 0 )
	{
		if ( iBlueCrystal == 7 )
		{
			if ( iRedCrystal == 0 && iBlackCrystal == 0 )
			{
				iCrystalMixType = 0;
				lpObj->ChaosSuccessRate = 100;
			}
		}
		else if ( iRedCrystal == 5 )
		{
			if ( iBlueCrystal == 0 && iBlackCrystal == 0 )
			{
				iCrystalMixType = 1;
				lpObj->ChaosSuccessRate = 100;
			}
		}
		else if ( iBlackCrystal == 3 )
		{
			if ( iRedCrystal == 0 && iBlueCrystal == 0 )
			{
				iCrystalMixType = 2;
				lpObj->ChaosSuccessRate = 100;
			}
		}
		else if ( iBlueCrystal == 1 && iRedCrystal == 1 && iBlackCrystal == 1 )
		{
			{
				iCrystalMixType = 3;
				lpObj->ChaosSuccessRate = 100;
			}
		}
		else
		{
			LogAddTD("[Hidden TreasureBox Event] [%s][%s] Item Mix Failed",
				lpObj->AccountID, lpObj->Name);
		
			DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
			lpObj->ChaosLock = FALSE;
		}
	}
	else
	{
		LogAddTD("[Hidden TreasureBox Event] [%s][%s] Item Mix Failed",
			lpObj->AccountID, lpObj->Name);
	
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}


	LogChaosItem(lpObj, "Hidden TreasureBox Mix");
	LogAddTD("[Hidden TreasureBox Event] [%s][%s] Chaos Mix Start",
		lpObj->AccountID, lpObj->Name);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		switch ( iCrystalMixType )
		{
			case 0:	case 1:	case 2:
				ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, ITEMGET(12,26) , 4, 1, 0, 0, 0, -1, 0, 0);
				break;
			case 3:
				ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, ITEMGET(12,26) , 5, 1, 0, 0, 0, -1, 0, 0);
				break;
			default:
				LogAddTD("[Hidden TreasureBox Event] [%s][%s] CBMix Failed - iCrystalMixType is wrong : %d",
					lpObj->AccountID, lpObj->Name, iCrystalMixType);
				DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
				lpObj->ChaosLock = FALSE;
				return;
		}
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		::gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[Hidden TreasureBox Event] [%s][%s] CBMix Success:%d Type:%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, iCrystalMixType);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		LogAddTD("[Hidden TreasureBox Event] [%s][%s] CBMix Fail Rate:%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate);

		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 1);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		lpObj->ChaosLock = FALSE;
	}

	lpObj->ChaosLock = FALSE;
}





void CChaosBox::Fenrir_01Level_Mix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int iStuffCount_01 = 0;
	int iStuffCount_02 = 0;
	int iChaosGemCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;


	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,32) )
			{
				iStuffCount_01 += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,33) )
			{
				iStuffCount_02 += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				iChaosGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iStuffCount_01 != 20 || iStuffCount_02 != 20 || iChaosGemCount != 1 || iInvalidItemCount > 0 )
	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}
	
	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "Fenrir_01Level_Mix");
	LogAddTD("[Fenrir Mix][Level 01] Chaos Mix Start");

	lpObj->ChaosSuccessRate = g_iFenrir_01Level_MixRate;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;
	int nChaosNeedMoney = 0;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int type = ITEMGET(13,35);
		int level = 0;
		int dur = 1;

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, type , level, dur, 0, 0, 0, -1, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Fenrir Mix][Level 01] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Fenrir Mix][Level 01] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

		lpObj->ChaosLock = FALSE;
	}
}


void CChaosBox::Fenrir_02Level_Mix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int iStuffCount_01 = 0;
	int iStuffCount_02 = 0;
	int iChaosGemCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;


	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,34) )
			{
				iStuffCount_01 += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,35) )
			{
				iStuffCount_02 ++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				iChaosGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iStuffCount_01 != 10 || iStuffCount_02 != 5 || iChaosGemCount != 1 || iInvalidItemCount > 0 )
	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}
	
	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "Fenrir_02Level_Mix");
	LogAddTD("[Fenrir Mix][Level 02] Chaos Mix Start");

	lpObj->ChaosSuccessRate = g_iFenrir_02Level_MixRate;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;
	int nChaosNeedMoney = 0;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int type = ITEMGET(13,36);
		int level = 0;
		int dur = 1;

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, type , level, dur, 0, 0, 0, -1, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Fenrir Mix][Level 02] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Fenrir Mix][Level 02] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

		lpObj->ChaosLock = FALSE;
	}
}




void CChaosBox::Fenrir_03Level_Mix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int iStuffCount_01 = 0;
	int iLifeGemCount = 0;
	int iChaosGemCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;


	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,36) )
			{
				iStuffCount_01 ++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,16) )
			{
				iLifeGemCount ++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				iChaosGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iStuffCount_01 != 1 || iLifeGemCount != 3 || iChaosGemCount != 1 || iInvalidItemCount > 0 )
	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}
	
	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "Fenrir_03Level_Mix");
	LogAddTD("[Fenrir Mix][Level 03] Chaos Mix Start");

	lpObj->ChaosSuccessRate = g_iFenrir_03Level_MixRate;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;
	int nChaosNeedMoney = 10000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int type = ITEMGET(13,37);
		int level = 0;
		int dur = 255;
		int op1 = 1;

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, type , level, dur, op1, 0, 0, -1, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Fenrir Mix][Level 03] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Fenrir Mix][Level 03] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

		lpObj->ChaosLock = FALSE;
	}
}


void CChaosBox::Fenrir_04Upgrade_Mix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int iStuffCount_01 = 0;
	int iLifeGemCount = 0;
	int iChaosGemCount = 0;
	int iAttackStuffCount = 0;
	int iDefendStuffCount = 0;
	int iAttackStuffPrice = 0;
	int iDefendStuffPrice = 0;
	int iInvalidItemCount = 0;
	int iChaosMoney = 0;
	BOOL bFenrirDamageInc = FALSE;
	BOOL bFenrirDamageDec = FALSE;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;


	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(13,37) )
			{
				iStuffCount_01 ++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,16) )
			{
				iLifeGemCount ++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				iChaosGemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type >= ITEMGET(0,0) &&
					  lpObj->pChaosBox[n].m_Type < ITEMGET(6,0) &&
					  lpObj->pChaosBox[n].m_Level >= 4 &&
					  lpObj->pChaosBox[n].m_Option3 >= 1)
			{
				iAttackStuffCount++;
				iAttackStuffPrice += lpObj->pChaosBox[n].m_BuyMoney;
			}
			else if ( lpObj->pChaosBox[n].m_Type >= ITEMGET(6,0) &&
					  lpObj->pChaosBox[n].m_Type < ITEMGET(12,0) &&
					  lpObj->pChaosBox[n].m_Level >= 4 &&
					  lpObj->pChaosBox[n].m_Option3 >= 1)
			{
				iDefendStuffCount++;
				iDefendStuffPrice += lpObj->pChaosBox[n].m_BuyMoney;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iStuffCount_01 != 1 || iLifeGemCount != 5 || iChaosGemCount != 1 || ( iAttackStuffCount == 0 && iDefendStuffCount == 0 ) || iInvalidItemCount > 0 )
	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}
	
	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	if ( iAttackStuffCount > 0 )
	{
		bFenrirDamageInc = TRUE;
		iChaosMoney = iAttackStuffPrice;
	}

	if ( iDefendStuffCount > 0 )
	{
		bFenrirDamageDec = TRUE;
		iChaosMoney = iDefendStuffPrice;
	}

	if ( bFenrirDamageInc && bFenrirDamageDec )
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	if ( !bFenrirDamageInc && !bFenrirDamageDec )
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "Fenrir_04Level_Mix");
	LogAddTD("[Fenrir Mix][Level 04] Chaos Mix Start");

	int nChaosNeedMoney = 10000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( iChaosMoney > 1000000 )
		iChaosMoney = 1000000;

	lpObj->ChaosSuccessRate = iChaosMoney * 100 / 1000000;

	if ( lpObj->ChaosSuccessRate > 79 )
		lpObj->ChaosSuccessRate = 79;

	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int type = ITEMGET(13,37);
		int level = 0;
		int dur = 255;
		int op1 = 1;
		int nop = 0;

		if ( bFenrirDamageInc )
		{
			nop |= 1;
		}

		if ( bFenrirDamageDec )
		{
			nop |= 2;
		}

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, type , level, dur, op1, 0, 0, -1, nop, 0);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Fenrir Mix][Level 04] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Fenrir Mix][Level 04] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

		lpObj->ChaosLock = FALSE;
	}
}




void CChaosBox::ShieldPotionLv1_Mix(LPOBJ lpObj)
{
	int iHealthPotionCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;
	int iCharmOfLuckCount = 0;

	if ( g_ShieldSystemOn == FALSE )
		return;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,3) )
			{
				iHealthPotionCount+= (int)lpObj->pChaosBox[n].m_Durability;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iInvalidItemCount > 0 || iHealthPotionCount != 3 )
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	iChaosMixPrice = g_iShieldPotionLv1MixMoney;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / 100;

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iChaosMixPrice += iChaosTaxMoney;

	if ( iChaosMixPrice < 0 )
	{
		iChaosMixPrice = 0;
	}

	if ( lpObj->Money < iChaosMixPrice )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	LogChaosItem(lpObj, "PotionMix][ShieldPotion Lv1 Mix");
	LogAddTD("[PotionMix][ShieldPotion Lv1 Mix] - Mix Start");
	
	int iRate = rand() % 100;
	iRate -= iCharmOfLuckCount;

	if ( iRate < g_iShieldPotionLv1MixSuccessRate )
	{
		int ItemNum = ITEMGET(14,35);

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, ItemNum, 0, 1, 0, 0, 0, lpObj->m_Index, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);

		LogAddTD("[ChaosMix][Shield Potion] Lv1 Potion Mix Success [%s][%s], Money(%d-%d), CharmRate : %d",
			lpObj->AccountID, lpObj->Name, 	lpObj->Money, iChaosMixPrice, iCharmOfLuckCount);
	}
	else
	{
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[ChaosMix][Shield Potion] Lv1 Potion Mix [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, 	g_iShieldPotionLv1MixSuccessRate,
			lpObj->Money, iChaosMixPrice, iCharmOfLuckCount);
	}

	lpObj->ChaosLock = FALSE;
}




void CChaosBox::ShieldPotionLv2_Mix(LPOBJ lpObj)
{
	int iHealthPotionCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,38) )
			{
				iHealthPotionCount+= (int)lpObj->pChaosBox[n].m_Durability;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iInvalidItemCount > 0 || iHealthPotionCount != 3 )
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	iChaosMixPrice = g_iShieldPotionLv2MixMoney;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / 100;

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iChaosMixPrice += iChaosTaxMoney;

	if ( iChaosMixPrice < 0 )
	{
		iChaosMixPrice = 0;
	}

	if ( lpObj->Money < iChaosMixPrice )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	
	LogChaosItem(lpObj, "PotionMix][ShieldPotion Lv2 Mix");
	LogAddTD("[PotionMix][ShieldPotion Lv2 Mix] - Mix Start");

	GCMoneySend(lpObj->m_Index, lpObj->Money);
	int iRate = rand() % 100;
	iRate -= iCharmOfLuckCount;

	if ( iRate < g_iShieldPotionLv2MixSuccessRate )
	{
		int ItemNum = ITEMGET(14,36);

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, ItemNum, 0, 1, 0, 0, 0, lpObj->m_Index, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);

		LogAddTD("[ChaosMix][Shield Potion] Lv2 Potion Mix Success [%s][%s], Money(%d-%d), CharmRate : %d",
			lpObj->AccountID, lpObj->Name, 	lpObj->Money, iChaosMixPrice, iCharmOfLuckCount);
	}
	else
	{
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[ChaosMix][Shield Potion] Lv2 Potion Mix [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, 	g_iShieldPotionLv2MixSuccessRate,
			lpObj->Money, iChaosMixPrice, iCharmOfLuckCount);
	}

	lpObj->ChaosLock = FALSE;
}


void CChaosBox::ShieldPotionLv3_Mix(LPOBJ lpObj)
{
	int iHealthPotionCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,39) )
			{
				iHealthPotionCount+= (int)lpObj->pChaosBox[n].m_Durability;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )	// Charm Of Luck
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if ( iInvalidItemCount > 0 || iHealthPotionCount != 3 )
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	if ( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	iChaosMixPrice = g_iShieldPotionLv3MixMoney;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / 100;

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iChaosMixPrice += iChaosTaxMoney;

	if ( iChaosMixPrice < 0 )
	{
		iChaosMixPrice = 0;
	}

	if ( lpObj->Money < iChaosMixPrice )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);
	
	LogChaosItem(lpObj, "PotionMix][ShieldPotion Lv3 Mix");
	LogAddTD("[PotionMix][ShieldPotion Lv3 Mix] - Mix Start");

	int iRate = rand() % 100;
	iRate -= iCharmOfLuckCount;

	if ( iRate < g_iShieldPotionLv3MixSuccessRate )
	{
		int ItemNum = ITEMGET(14,37);

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, ItemNum, 0, 1, 0, 0, 0, lpObj->m_Index, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);

		LogAddTD("[ChaosMix][Shield Potion] Lv3 Potion Mix Success [%s][%s], Money(%d-%d), CharmRate : %d",
			lpObj->AccountID, lpObj->Name, 	lpObj->Money, iChaosMixPrice, iCharmOfLuckCount);
	}
	else
	{
		ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[ChaosMix][Shield Potion] Lv3 Potion Mix [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, 	g_iShieldPotionLv3MixSuccessRate,
			lpObj->Money, iChaosMixPrice, iCharmOfLuckCount);
	}

	lpObj->ChaosLock = FALSE;
}

int CChaosBox::IsCondorItem(int iType)
{
	if( iType == iCondorStone ||
		iType == iCondorFeather )
	{
		return TRUE;
	}
return FALSE;
}

int CChaosBox::Is1stLevelWing(int iType)
{
	if( iType == ITEMGET(12,0) ||
		iType == ITEMGET(12,1) ||
		iType == ITEMGET(12,2) ||
		iType == ITEMGET(12,41) )
	{
		return TRUE;
	}
return FALSE;
}

int CChaosBox::Is2ndLevelWing(int iType)
{
	if( iType == ITEMGET(12,3) ||
		iType == ITEMGET(12,4) ||
		iType == ITEMGET(12,5) ||
		iType == ITEMGET(12,6) ||
		iType == ITEMGET(13,30) ||
		iType == ITEMGET(12,42) ||
		iType == ITEMGET(12,49) )
	{
		return TRUE;
	}
return FALSE;
}

int CChaosBox::CheckItemOptions(CItem* lpItem,short iLevel,BYTE iOption1,BYTE iOption2,BYTE iOption3,BYTE iSetOption,BYTE iExcOption)
{
	if( lpItem == FALSE )
	{
		return false;
	}

	if( iLevel != FALSE )
	{
		if( lpItem->m_Level < iLevel )
		{
			return FALSE;
		}
	}

	if( iOption1 != FALSE )
	{
		if( lpItem->m_Option1 < iOption1 )
		{
			return FALSE;
		}
	}

	if( iOption2 != FALSE )
	{
		if( lpItem->m_Option2 < iOption2 )
		{
			return FALSE;
		}
	}

	if( iOption3 != FALSE )
	{
		if( lpItem->m_Option3 < iOption3 )
		{
			return FALSE;
		}
	}

	if( iSetOption != FALSE )
	{
		if( lpItem->m_SetOption == FALSE )
		{
			return FALSE;
		}
	}

	if( iExcOption != FALSE )
	{
		if( lpItem->m_NewOption == FALSE )
		{
			return FALSE;
		}
	}
return TRUE;
}


#if (GS_CASTLE == 1)
void CChaosBox::CastleSpecialItemMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;
	int iDefendGemCount = 0;
	int iBlessGemMixCount = 0;
	int iSoulGemMixCount = 0;
	int iInvalidItemCount =0;
	int iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((PBYTE)&pMsg,0x86,sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if(g_CastleSiege.CheckCastleOwnerMember(lpObj->m_Index) != FALSE )
	{
		if(lpObj->GuildStatus != G_MASTER)
		{
			LogAddTD("[CastleSpecialMix] [%s][%s] Item Mix Failed - No Auth",
				lpObj->AccountID,lpObj->Name);
			DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
			lpObj->ChaosLock = FALSE;
			return;
		}
	}
	else
	{
		LogAddTD("[CastleSpecialMix] [%s][%s] Item Mix Failed - No Auth",
			lpObj->AccountID,lpObj->Name);
		DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if(g_iCastleItemMixLimit <= 0)
	{
		LogAddTD("[CastleSpecialMix] [%s][%s] Item Mix Failed - Mix Count Limit Over",
			lpObj->AccountID,lpObj->Name);
		DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if(lpObj->pChaosBox[n].IsItem() == TRUE)
		{
			if(lpObj->pChaosBox[n].m_Type == ITEMGET(12,30))
			{
				iBlessGemMixCount += (lpObj->pChaosBox[n].m_Level + 1);
			}
			else if(lpObj->pChaosBox[n].m_Type == ITEMGET(12,31))
			{
				iSoulGemMixCount += (lpObj->pChaosBox[n].m_Level + 1);
			}
#ifdef __NOVUS__
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(12, 138) && lpObj->pChaosBox[n].m_Level == 2 )
			{
				iDefendGemCount++;
			}
#else
			else if(lpObj->pChaosBox[n].m_Type == ITEMGET(14,31))
			{
				iDefendGemCount++;
			}
#endif
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if(iBlessGemMixCount != 3
		|| iSoulGemMixCount != 3
#ifdef __NOVUS__
		|| iDefendGemCount != 1
#else
		|| iDefendGemCount != 30
#endif
		|| iInvalidItemCount > 0)
	{
		LogAddTD("[CastleSpecialMix] [%s][%s] Item Mix Failed - Item Error (DG:%d, BGx10:%d, SGx10:%d, Other:%d)",
			lpObj->AccountID,lpObj->Name,iBlessGemMixCount,iSoulGemMixCount,iDefendGemCount,iInvalidItemCount);
		DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj,"Castle Special Mix");
	LogAddTD("[CastleSpecialMix] [%s][%s] Chaos Mix Start",
		lpObj->AccountID,lpObj->Name);

	lpObj->ChaosSuccessRate = 100;
	int nChaosNeedMoney = 1000000000;

	if(lpObj->Money < nChaosNeedMoney)
	{
		LogAddTD("[CastleSpecialMix] [%s][%s] Item Mix Failed - Lack of Money (%d/%d)",
			lpObj->AccountID,lpObj->Name,lpObj->Money,nChaosNeedMoney);
		pMsg.Result = 2;
		DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	if(rand() % 100 < lpObj->ChaosSuccessRate)
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::CastleMix, lpObj->m_Index, 255, 0, 0);
		gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[CastleSpecialMix] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,nChaosNeedMoney);
		g_iCastleItemMixLimit--;
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj,1);
		DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
		LogAddTD("[CastleSpecialMix] [%s][%s] CBMix Fail %d Money : %d-%d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,nChaosNeedMoney);
	}

	lpObj->ChaosLock = FALSE;
}
#endif

void CChaosBox::CondorFeather_Mix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	int iWingCount = 0;
	int iSetItemCount = 0;
	int iChaosCount = 0;
	int iSoulPackCount = 0;
	int iCreationCount = 0;
	int iOtherItemCount = 0;
	int iItemValue = 0;
	int iNeedChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for(int n = 0; n < CHAOS_BOX_SIZE;n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			//Item* lpItem,short iLevel,BYTE iOption1,BYTE iOption2,BYTE iOption3,BYTE iSetOption,BYTE iExcOption)
			if( Is2ndLevelWing(lpObj->pChaosBox[n].m_Type) && CheckItemOptions(&lpObj->pChaosBox[n],9,0,0,1,0,0) )
			{
				iWingCount++;
			}
			else if( CheckItemOptions(&lpObj->pChaosBox[n],7,0,0,1,1,0)  )
			{
				iSetItemCount++;
				iItemValue += lpObj->pChaosBox[n].m_BuyMoney;
			}
			else if( lpObj->pChaosBox[n].m_Type == iChaosJewel )
			{
				iChaosCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iSoulPack )
			{
				iSoulPackCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iCreationJewel )
			{
				iCreationCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iWingCount != 1 ||
		iSetItemCount < 1 ||
		iChaosCount != 1 ||
		iSoulPackCount != 1 ||
		iCreationCount != 1 ||
		iOtherItemCount > 0 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "ThirdWingLevel1_Mix");
	LogAddTD("[ThirdWing Mix][Level 01] Chaos Mix Start");

#if (__CUSTOM__ == 1)

	if( iItemValue > 0 )
	{
		lpObj->ChaosSuccessRate = iItemValue / gc_ChaosMixCondor_Div;
	}

	if( lpObj->ChaosSuccessRate > gc_ChaosMixCondor_Max )
	{
		lpObj->ChaosSuccessRate = gc_ChaosMixCondor_Max;
	}
	else if( lpObj->ChaosSuccessRate < 1 )
	{
		lpObj->ChaosSuccessRate = 1;
	}

#else

	if( iItemValue > 0 )
	{
		lpObj->ChaosSuccessRate = iItemValue / 300000;
	}

	if( lpObj->ChaosSuccessRate > 60 )
	{
		lpObj->ChaosSuccessRate = 60;
	}
	else if( lpObj->ChaosSuccessRate < 1 )
	{
		lpObj->ChaosSuccessRate = 1;
	}

#endif

	iNeedChaosMoney = lpObj->ChaosSuccessRate * 200000;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;
	iNeedChaosMoney = lpObj->ChaosSuccessRate * 200000;

	int iChaosTaxMoney = (int)((__int64)iNeedChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iNeedChaosMoney += iChaosTaxMoney;

	if ( iNeedChaosMoney < 0 )
	{
		iNeedChaosMoney = 0;
	}

	if(lpObj->Money < iNeedChaosMoney )
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= iNeedChaosMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	if( rand() % 100 < lpObj->ChaosSuccessRate )
	{
		int Item = ITEMGET(13,53);
		int Dur = 0;
		int Level = 0;
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		ItemSerialCreateSend(lpObj->m_Index,255,0,0,Item,Level,Dur,0,0,0,-1,0,0);
		gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[ThirdWing Mix][Level 01] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxWingMixItemDown(lpObj);
		GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

		LogAddTD("[ThirdWing Mix][Level 01] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);

		lpObj->ChaosLock = FALSE;
	}
}

void CChaosBox::NewWingChaos_Mix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	int iExcItemCount = 0;
	int iCondorFeatherCount = 0;
	int iCondorStoneCount = 0;
	int iChaosCount = 0;
	int iSoulPackCount = 0;
	int iBlessPackCount = 0;
	int iCreationCount =0;
	int iOtherItemCount = 0;
	int iItemValue = 0;
	int iNeedChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( CheckItemOptions(&lpObj->pChaosBox[n],7,0,0,1,0,1) != FALSE )
			{
				iExcItemCount++;
				iItemValue += lpObj->pChaosBox[n].m_BuyMoney;
			}
			else if( lpObj->pChaosBox[n].m_Type == iCondorFeather )
			{
				iCondorFeatherCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iCondorStone )
			{
				iCondorStoneCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iChaosJewel )
			{
				iChaosCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iBlessPack )
			{
				iBlessPackCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iSoulPack )
			{
				iSoulPackCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iCreationJewel )
			{
				iCreationCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iExcItemCount < 1 || 
		iCondorFeatherCount != 1 ||
		iCondorStoneCount != 1 ||
		iChaosCount != 1 ||
		iSoulPackCount != 1 ||
		iBlessPackCount != 1 ||
		iCreationCount != 1 ||
		iOtherItemCount > 0 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "ThirdWingLevel2_Mix");
	LogAddTD("[ThirdWing Mix][Level 02] Chaos Mix Start");

#if (__CUSTOM__ == 1)

	if( iItemValue > 0 )
	{
		lpObj->ChaosSuccessRate = iItemValue / gc_ChaosMix3rdWing_Div;
	}

	if( lpObj->ChaosSuccessRate > gc_ChaosMix3rdWing_Max )
	{
		lpObj->ChaosSuccessRate = gc_ChaosMix3rdWing_Max;
	}
	else if( lpObj->ChaosSuccessRate < 1 )
	{
		lpObj->ChaosSuccessRate = 1;
	}

#else

	if( iItemValue > 0 )
	{
		lpObj->ChaosSuccessRate = iItemValue / 3000000;
	}

	if( lpObj->ChaosSuccessRate > 40 )
	{
		lpObj->ChaosSuccessRate = 40;
	}
	else if( lpObj->ChaosSuccessRate < 1 )
	{
		lpObj->ChaosSuccessRate = 1;
	}

#endif

	iNeedChaosMoney = lpObj->ChaosSuccessRate * 200000;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;
	iNeedChaosMoney = lpObj->ChaosSuccessRate * 200000;

	int iChaosTaxMoney = (int)((__int64)iNeedChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iNeedChaosMoney += iChaosTaxMoney;

	if ( iNeedChaosMoney < 0 )
	{
		iNeedChaosMoney = 0;
	}

	if(lpObj->Money < iNeedChaosMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= iNeedChaosMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	if( rand() % 100 < lpObj->ChaosSuccessRate )
	{
		int RandWing = rand()%7;
		int Item = 0;

		if( RandWing == 0 )
		{
			Item = iStormWings;
		}
		else if( RandWing == 1 )
		{
			Item = iRedemptionWings;
		}
		else if( RandWing == 2 )
		{
			Item = iFortitudeWings;
		}
		else if( RandWing == 3 )
		{
			Item = iHurricaneWings;
		}
		else if( RandWing == 4 )
		{
			Item = iMonarchMantle;
		}
		else if( RandWing == 5 )
		{
			Item = iDimensionWings;
		}
		else if( RandWing == 6 )
		{
			Item = iRFCape2;
		}

		int Option1 = 0;
		int Option2 = 0;

		if( rand() % 1000 <= iOptionRate )
		{
			Option2 = TRUE;
		}

		int Option3 = 0;
		int ExcOption = 0;

		int Rand = rand() %2;
		int Rand2;

		switch( Rand )
		{
		case 0:
			Rand2 = rand() % 1000;

			if( Rand2 < 400 )
			{
				ExcOption |= 0x10;
			}
			break;
		case 1:

			Rand2 = rand() % 1000;

			if( Rand2 < 300 )
			{
				ExcOption |= 0x20;
			}
			break;
		}

		int Rand3 = rand() % 4;
		int Rand4 = rand() % 1000;

		Option3 = 0;

		switch( Rand3 )
		{
		case 1:
			if( Rand4 < 120 )
			{
				Option3 = 1;
			}
			break;
		case 2:
			if( Rand4 < 60 )
			{
				Option3 = 2;
			}
			break;
		case 3:
			if( Rand4 < 30 )
			{
				Option3 = 3;
			}
			break;
		}

		int Rand5 = rand() % 4;
		int Rand6 = rand() % 1000;

		switch( Rand5 )
		{
		case 0:
			if( Rand6 < 40 )
			{
				ExcOption |= 0x01;
			}
			break;
		case 1:
			if( Rand6 < 20 )
			{
				ExcOption |= 0x02;
			}
			break;
		case 2:
			if( Rand6 < 70 )
			{
				ExcOption |= 0x04;
			}
			break;
		case 3:
			if( Rand6 < 70 )
			{
				ExcOption |= 0x08;
			}
			break;
		}
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		ItemSerialCreateSend(lpObj->m_Index,255,0,0,Item,0,0,Option1,Option2,Option3,-1,ExcOption,0);
		gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[ThirdWing Mix][Level 02] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxWingMixItemDown(lpObj);
		GCUserChaosBoxSend(lpObj, 0);
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

		LogAddTD("[ThirdWing Mix][Level 02] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);

		lpObj->ChaosLock = FALSE;
	}
}

void CChaosBox::ChaosBoxWingMixItemDown(LPOBJ lpObj)
{
	if( lpObj->pChaosBox == FALSE )
		return;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( Is2ndLevelWing(lpObj->pChaosBox[n].m_Type) != FALSE )
			{
				RandomLevelDown(&lpObj->pChaosBox[n]);
			}
			else if( CheckItemOptions(&lpObj->pChaosBox[n],0,0,0,0,1,0) != FALSE )
			{
				RandomLevelDown(&lpObj->pChaosBox[n]);
			}
			else if( CheckItemOptions(&lpObj->pChaosBox[n],0,0,0,0,0,1) != FALSE )
			{
				RandomLevelDown(&lpObj->pChaosBox[n]);
			}
			else
			{
				lpObj->pChaosBox[n].Clear();
			}
		}
	}
}

void CChaosBox::RandomLevelDown(CItem* lpItem)
{
	if( lpItem == FALSE )
		return;

	if( rand()%2 < 1 )
	{
		lpItem->m_Level -= 2;
	}
	else
	{
		lpItem->m_Level -= 3;
	}

	lpItem->m_Option3 = 0;
	lpItem->Convert(lpItem->m_Type,lpItem->m_Option1,lpItem->m_Option2,lpItem->m_Option3,lpItem->m_NewOption,lpItem->m_SetOption,lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_SocketBonus,3);
}

void CChaosBox::LotteryItemMix(LPOBJ lpObj)
{
	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	int iChaosCardCount = 0;
	int iOtherItemCount = 0;
	int iHeight = 0;
	int iWidth = 0;
	int iChaosCardType = 0;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,54) )
			{
				iChaosCardType = 1;
				iChaosCardCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,92) )
			{
				iChaosCardType = 2;
				iChaosCardCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,93) )
			{
				iChaosCardType = 3;
				iChaosCardCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,95) )
			{
				iChaosCardType = 4;
				iChaosCardCount++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iOtherItemCount > 0 || iChaosCardCount != 1 )
	{
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iChaosCardType == 0 )
	{
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "CashLottery");
	iHeight = 4;
	iWidth = 2;

	if( !CheckInventoryEmptySpace(lpObj,iHeight,iWidth) )
	{
		pMsg.Result = 0xF1;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	CItem LotteryItem;

	if( iChaosCardType == 1 )
	{
		g_Lottery.GetItem("ChaosCard",&LotteryItem);
	}
	else if( iChaosCardType == 2 )
	{
		g_Lottery.GetItem("ChaosCardGold",&LotteryItem);
	}
	else if( iChaosCardType == 3 )
	{
		g_Lottery.GetItem("ChaosCardRare",&LotteryItem);
	}
	else if( iChaosCardType == 4 )
	{
		g_Lottery.GetItem("ChaosCardMini",&LotteryItem);
	}

	if( LotteryItem.IsItem() == FALSE )
	{
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		LogAddTD("[ChaosMix][LotteryItemMix] Lottery Item Mix Failed. Can't Get Item from List.");
		return;
	}

	ItemSerialCreateSend(lpObj->m_Index,255,0,0,LotteryItem.m_Type,LotteryItem.m_Level,(BYTE)LotteryItem.m_Durability,LotteryItem.m_Option1,
		LotteryItem.m_Option2,LotteryItem.m_Option3,lpObj->m_Index,LotteryItem.m_NewOption,0);
	gObjInventoryCommit(lpObj->m_Index);

	LogAddTD("[ChaosMix][ChaosCardMix] Chaos Card Item Mix Success [%s][%s]",
		lpObj->AccountID,lpObj->Name);

	BYTE btExOption[MAX_EXOPTION_SIZE];

	ItemIsBufExOption(btExOption, &LotteryItem);
	
	LogAddTD("[CashShop][ChaosCardMix] - User(ID:%s,Name:%s) Item(Name:%s,Type:%d,Level:%d,Dur:%d,Skill:%d,Luck:%d,AddOption:%d,Ex:(%d,%d,%d,%d,%d,%d))",
		lpObj->AccountID, lpObj->Name, ItemAttribute[LotteryItem.m_Type].Name,
		LotteryItem.m_Type, LotteryItem.m_Level, (int)LotteryItem.m_Durability,
		LotteryItem.m_Option1, LotteryItem.m_Option2, LotteryItem.m_Option3,
		btExOption[0], btExOption[1], btExOption[2], btExOption[3], btExOption[4], btExOption[5]);
}

struct pMixTmp
{
	BYTE bOk;
	BYTE Dur;
	BYTE NeedDur;
	int m_Item;
};

void CChaosBox::CherryBlossomMix(LPOBJ lpObj)
{
	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	lpObj->ChaosLock = TRUE;

	pMixTmp pTmp = {0};

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == FALSE )
			continue;

		if( pTmp.m_Item == FALSE )
		{
			pTmp.m_Item = lpObj->pChaosBox[n].m_Type;

			if( pTmp.m_Item == ITEMGET(14,88) )
			{
				pTmp.NeedDur = 10;
			}
			else if( pTmp.m_Item == ITEMGET(14,89) )
			{
				pTmp.NeedDur = 30;
			}
			else if( pTmp.m_Item == ITEMGET(14,90) )
			{
				pTmp.NeedDur = 255;
			}
			else
			{
				pTmp.m_Item = 0;
				continue;
			}

			pTmp.bOk = TRUE;
		}

		if( pTmp.m_Item == lpObj->pChaosBox[n].m_Type )
		{
			pTmp.Dur += (int)lpObj->pChaosBox[n].m_Durability;
		}
		else
		{
			pTmp.bOk = FALSE;
			break;
		}
	}

	if( pTmp.Dur != pTmp.NeedDur )
	{
		pTmp.bOk = FALSE;
	}

	if( pTmp.m_Item == FALSE || pTmp.bOk == FALSE )
	{
		pMsg.Result = 0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	int iHeight = 4;
	int iWidth = 2;

	if( !CheckInventoryEmptySpace(lpObj, iHeight, iWidth) )
	{
		pMsg.Result = 0xF1;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	int bResult = 0;

	if( pTmp.m_Item == ITEMGET(14,88) )
	{
		bResult = g_EventItemBagManager.OpenSpecial(EventBagSpecial::CherryMixWhite, lpObj->m_Index, 0xFF, 0, 0);
	}
	else if( pTmp.m_Item == ITEMGET(14,89) )
	{
		bResult = g_EventItemBagManager.OpenSpecial(EventBagSpecial::CherryMixRed, lpObj->m_Index, 0xFF, 0, 0);
	}
	else if( pTmp.m_Item == ITEMGET(14,90) )
	{
		bResult = g_EventItemBagManager.OpenSpecial(EventBagSpecial::CherryMixGold, lpObj->m_Index, 0xFF, 0, 0);
	}

	if( bResult == false )
	{
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj,0);
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		LogAddTD("[%s][%s] CherryBlossomMix Fail %d Money : %d-%d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,lpObj->ChaosMoney);
		lpObj->ChaosLock = FALSE;
		return;
	}

	gObjInventoryCommit(lpObj->m_Index);
	lpObj->ChaosLock = FALSE;
}

void CChaosBox::IllusionTemple_Mix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	int PaperCount = 0;
	int PotionCount = 0;
	int ChaosCount = 0;
	int PotionLevel = -1;
	int PaperLevel = -1;
	int OtherItemCount = 0;
	int iCharmOfLuckCount = 0;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if(lpObj->pChaosBox[n].IsItem() == TRUE)
		{
			if(lpObj->pChaosBox[n].m_Type == ITEMGET(13,49))
			{
				PaperCount++;
				PaperLevel = lpObj->pChaosBox[n].m_Level;
			}
			else if(lpObj->pChaosBox[n].m_Type == ITEMGET(13,50))
			{
				PotionCount++;
				PotionLevel = lpObj->pChaosBox[n].m_Level;
			}
			else if(lpObj->pChaosBox[n].m_Type == ITEMGET(12,15))
			{
				ChaosCount++;
			}
			else if(lpObj->pChaosBox[n].m_Type == ITEMGET(14,53))
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				OtherItemCount++;
			}
		}
	}

	if(PaperCount != 1 || PotionCount != 1 || ChaosCount != 1 || OtherItemCount != 0 || PotionLevel != PaperLevel)
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if(PotionLevel < 1 || PotionLevel > IT_MAXTEMPLE)
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if(iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	int nChaosNeedMoney = 300000 + (PotionLevel-1) * 200000;

	if(lpObj->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	lpObj->ChaosSuccessRate = 70;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	if(rand()%100 < lpObj->ChaosSuccessRate)
	{
		ItemSerialCreateSend(lpObj->m_Index,255,0,0,ITEMGET(13,51),PotionLevel,1,0,0,0,lpObj->m_Index,0,0);
		gObjInventoryCommit(lpObj->m_Index);

		LogAddTD("[ChaosMix][Illusion Temple] Illusion Ticket Mix Success [%s][%s], Money(%d-%d), CharmRate : %d",
			lpObj->AccountID, lpObj->Name, 	lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
	}
	else
	{
		for(int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if(lpObj->pChaosBox[n].IsItem() == TRUE)
			{
				lpObj->pChaosBox[n].Clear();
			}
		}


		GCUserChaosBoxSend(lpObj,0);
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

		LogAddTD("[ChaosMix][Illusion Temple] Illusion Ticket [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID, lpObj->Name, 	lpObj->ChaosSuccessRate,lpObj->Money, nChaosNeedMoney, iCharmOfLuckCount);
	}	
	lpObj->ChaosLock = FALSE;
}

void CChaosBox::SeedExtractMix(LPOBJ lpObj)
{
	int iExcItemCount = 0;
	int iAncItemCount = 0;
	int iHarmonyCount = 0;
	int iChaosCount = 0;
	int iCreationCount = 0;
	int iOtherItemCount = 0;
	int iChaosMixRate = 0;
	DWORD iItemValue = 0;
	int iMixNeedMoney = 0;
	int iChaosTaxMoney = 0;
	int iAncItemPos = 0;
	int iExcItemPos = 0;
	int iHarmonyPos = 0;
	int iChaosPos = 0;
	int iCreationPos = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	lpObj->ChaosLock = TRUE;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( lpObj->pChaosBox[n].IsExtItem() == TRUE && lpObj->pChaosBox[n].m_Level >= 4 &&
				lpObj->pChaosBox[n].m_bLOCKED == FALSE )
			{
				iExcItemCount++;
				iItemValue += lpObj->pChaosBox[n].m_BuyMoney;
				iExcItemPos = n;
			}
			else if( lpObj->pChaosBox[n].IsSetItem() && lpObj->pChaosBox[n].m_Level >= 4 &&
				lpObj->pChaosBox[n].m_bLOCKED == FALSE )
			{
				iAncItemCount++;
				iItemValue += lpObj->pChaosBox[n].m_BuyMoney;
				iAncItemPos = n;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,42) )
			{
				iHarmonyCount++;
				iHarmonyPos = n;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				iChaosCount++;
				iChaosPos = n;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,22) )
			{
				iCreationCount++;
				iCreationPos = n;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iOtherItemCount > 0 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iExcItemCount != 1 ||
		iAncItemCount != 1 ||
		iHarmonyCount != 1 ||
		iChaosCount != 1 ||
		iCreationCount != 1 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SeedExtractMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if( iChaosTaxMoney < 0 )
		iChaosTaxMoney = 0;

	iMixNeedMoney = g_SeedExtractMoney + iChaosTaxMoney;

	if( lpObj->Money < iMixNeedMoney )
	{
		pMsg.Result = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "SeedExtract");
	lpObj->Money -= iMixNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	iChaosMixRate = 80 + (iItemValue / 2000000);

	if( iChaosMixRate > 90 )
		iChaosMixRate = 90;

	if( rand()%100 > iChaosMixRate )
	{
		lpObj->pChaosBox[iAncItemPos].m_Level -= rand()%3;
		lpObj->pChaosBox[iExcItemPos].m_Level -= rand()%3;
		lpObj->pChaosBox[iHarmonyPos].Clear();
		lpObj->pChaosBox[iChaosPos].Clear();
		lpObj->pChaosBox[iCreationPos].Clear();

		GCUserChaosBoxSend(lpObj,0);
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		LogAddTD("[%s][%s] Seed Extract Mix Failed.",lpObj->AccountID,lpObj->Name);
	}
	else
	{
		BYTE SeedRate = 0;
		SeedRate = g_SocketItem.GetOptionRate();

		_SOCKET_SEED_DATA* tmpSeed;
		tmpSeed = g_SocketItem.GetSeedOption(SeedRate);

		if( tmpSeed != NULL )
		{
			pMsg.Result = TRUE;

			ItemSerialCreateSend(lpObj->m_Index,0xFF,0,0,tmpSeed->SeedItem,tmpSeed->btIndex,1,0,0,0,lpObj->m_Index,0,0);
			gObjInventoryCommit(lpObj->m_Index);
			LogAddTD("[%s][%s] Seed Extract Mix Success - SeedInfo ( Index : %d, Type : %d, Level : %d, ItemCode : %d )",
				lpObj->AccountID,lpObj->Name,tmpSeed->btOption,tmpSeed->btGroup,tmpSeed->btIndex,tmpSeed->SeedItem);

		}
		else
		{
			lpObj->pChaosBox[iAncItemPos].m_Level -= rand()%3;
			lpObj->pChaosBox[iExcItemPos].m_Level -= rand()%3;
			lpObj->pChaosBox[iHarmonyPos].Clear();
			lpObj->pChaosBox[iChaosPos].Clear();
			lpObj->pChaosBox[iCreationPos].Clear();

			GCUserChaosBoxSend(lpObj,0);

			LogAddTD("[%s][%s] Seed Extract Mix Failed - SeedData is NULL",lpObj->AccountID,lpObj->Name);

			lpObj->ChaosLock = FALSE;
		}
	}
}

void CChaosBox::SeedSphereCompositeMix(LPOBJ lpObj)
{
	int iSeedCount = 0;
	int iSphereCount = 0;
	int iOtherItemCount = 0;
	int iChaosCount = 0;
	int iCreationCount = 0;
	int iChaosTaxMoney = 0;
	int iMixNeedMoney = 0;
	DWORD iItemValue = 0;
	int iChaosMixRate = 0;
	CItem* lpSeed = NULL;
	CItem* lpSphere = NULL;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	lpObj->ChaosLock = TRUE;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( g_SocketItem.IsSeedItem(lpObj->pChaosBox[n].m_Type) == TRUE )
			{
				iSeedCount++;
				iItemValue += lpObj->pChaosBox[n].m_BuyMoney;
				lpSeed = &lpObj->pChaosBox[n];
			}
			else if( g_SocketItem.IsSphereItem(lpObj->pChaosBox[n].m_Type) == TRUE )
			{
				iSphereCount++;
				iItemValue += lpObj->pChaosBox[n].m_BuyMoney;
				lpSphere = &lpObj->pChaosBox[n];
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				iChaosCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,22) )
			{
				iCreationCount++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iOtherItemCount > 0 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iSeedCount != 1 ||
		iSphereCount != 1 ||
		iChaosCount != 1 ||
		iCreationCount != 1 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SphereCompositeMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if( iChaosTaxMoney < 0 )
		iChaosTaxMoney = 0;

	iMixNeedMoney = g_SphereCompositeMoney + iChaosTaxMoney;

	if( lpObj->Money < iMixNeedMoney )
	{
		pMsg.Result = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj,"SeedSphereComposite");

	iChaosMixRate = 80 + (iItemValue / 200000);

	if( iChaosMixRate > 90 )
		iChaosMixRate = 90;

	lpObj->Money -= iMixNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);


	if( rand()%100 > iChaosMixRate )
	{
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj,0);
		LogAddTD("[%s][%s] SeedSphere Composite Mix Failed.",lpObj->AccountID,lpObj->Name);
	}
	else
	{
		BYTE btOption = 0;
		BYTE btLevel = 0;

		_SOCKET_SPHERE_OPTION tmpSphere;

		btOption = g_SocketItem.GetSeedOption(lpSeed->m_Type,lpSeed->m_Level);
		btLevel = g_SocketItem.GetSphereLevel(lpSphere->m_Type);

		g_SocketItem.IsOptionSet(&tmpSphere,btOption,btLevel);

		if( tmpSphere.SeedItem != 0 )
		{
			pMsg.Result = TRUE;

			ItemSerialCreateSend(lpObj->m_Index,0xFF,0,0,tmpSphere.SeedItem,tmpSphere.btIndex,1,0,0,0,lpObj->m_Index,0,0);
			gObjInventoryCommit(lpObj->m_Index);
			LogAddTD("[%s][%s] SeedSphere Composite Mix Success - SeedSphere Info ( Index : %d, Type : %d, Level : %d, ItemCode : %d )",
				lpObj->AccountID,lpObj->Name,tmpSphere.btOptionId,tmpSphere.btGroup,tmpSphere.btLevel,tmpSphere.SeedItem);
			return;
		}
		else
		{
			ChaosBoxInit(lpObj);
			GCUserChaosBoxSend(lpObj,0);
			LogAddTD("[%s][%s] SeedSphere Composite Mix Failed - SeedSphere Data is NULL",
				lpObj->AccountID,lpObj->Name);
		}
	}

	lpObj->ChaosLock = FALSE;
}

void CChaosBox::SetSeedSphereMix(LPOBJ lpObj,BYTE Pos)
{
	int iItemCount = 0;
	int iOtherItemCount = 0;
	int iSeedSphereCount = 0;
	int iChaosCount = 0;
	int iCreationCount = 0;
	CItem* lpItem = 0;
	CItem* lpSeedSphere = 0;
	CItem tmpItem;
	int iChaosTaxMoney = 0;
	int iMixNeedMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if( Pos >= MAX_SOCKET_COUNT )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->ChaosLock = TRUE;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( g_SocketItem.IsSocketItem(lpObj->pChaosBox[n].m_Type) == TRUE &&
				lpObj->pChaosBox[n].m_SocketOption[Pos] == (BYTE)-2)
			{
				iItemCount++;
				lpItem = &lpObj->pChaosBox[n];
			}
			else if( g_SocketItem.IsSeedSphereItem(lpObj->pChaosBox[n].m_Type) == TRUE )
			{
				iSeedSphereCount++;
				lpSeedSphere = &lpObj->pChaosBox[n];
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				iChaosCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,22) )
			{
				iCreationCount++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}


	if( iOtherItemCount > 0 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iItemCount != 1 ||
		iSeedSphereCount != 1 ||
		iChaosCount != 1 ||
		iCreationCount != 1 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( g_SocketItem.CheckMountItem(lpItem,lpSeedSphere->m_Type,lpSeedSphere->m_Level) == FALSE )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SphereSetMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if( iChaosTaxMoney < 0 )
		iChaosTaxMoney = 0;

	iMixNeedMoney = g_SphereSetMoney +iChaosTaxMoney;

	if( lpObj->Money < iMixNeedMoney )
	{
		pMsg.Result = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj,"SetSeedSphere");
	BYTE btOption = 0;

	btOption = g_SocketItem.GetSphereOption(lpSeedSphere->m_Type,lpSeedSphere->m_Level);

	if( btOption == (BYTE)-1 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		LogAddTD("[%s][%s] Set Seed Mix Failed - SeedSphere is NULL",
			lpObj->AccountID,lpObj->Name);
		return;
	}

	if( g_SocketItem.IsItemType(lpItem,btOption) == FALSE )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		LogAddTD("[%s][%s] Set Seed Mix Failed - Wrong Item Type for Set Socket",
			lpObj->AccountID,lpObj->Name);
		return;
	}

	lpObj->Money -= iMixNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	g_SocketItem.ItemSetSeedSphere(lpItem,btOption,Pos);
	float dur = (float)ItemGetDurability(lpItem->m_Type,lpItem->m_Level,lpItem->IsExtItem(),lpItem->IsSetItem());

	tmpItem.m_Number = lpItem->m_Number;
	tmpItem.m_Level = lpItem->m_Level;
	tmpItem.m_Durability = dur * lpItem->m_Durability / lpItem->m_BaseDurability;
	tmpItem.m_JewelOfHarmonyOption = lpItem->m_JewelOfHarmonyOption;
	tmpItem.m_bLOCKED = lpItem->m_bLOCKED;

	tmpItem.Convert(lpItem->m_Type,lpItem->m_Option1,lpItem->m_Option2,lpItem->m_Option3,lpItem->m_NewOption,lpItem->m_SetOption,lpItem->m_ItemOptionEx,&lpItem->m_SocketOption[0],lpItem->m_SocketBonus,3);
	lpObj->ChaosLock = FALSE;

	ItemByteConvert(&pMsg.ItemInfo[0],tmpItem);
	ChaosBoxInit(lpObj);
	gObjChaosBoxInsertItemPos(lpObj->m_Index,tmpItem,0,-1);
	gObjChaosItemSet(lpObj->m_Index,0,1);
	GCUserChaosBoxSend(lpObj,0);

	LogAddTD("[%s][%s] Set SeedSphere Mix Success - ItemInfo ( Name : %s, ItemCode : %d, Level : %d, SocketOption[%d,%d,%d,%d,%d], BonusOption : %d )",
		lpObj->AccountID,lpObj->Name,ItemAttribute[tmpItem.m_Type].Name,tmpItem.m_Type,tmpItem.m_Level,
		tmpItem.m_SocketOption[0],tmpItem.m_SocketOption[1],tmpItem.m_SocketOption[2],tmpItem.m_SocketOption[3],tmpItem.m_SocketOption[4],
		tmpItem.m_SocketBonus);

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
}

void CChaosBox::RemoveSeedSphereMix(LPOBJ lpObj,BYTE Pos)
{
	int iItemCount = 0;
	int iOtherItemCount = 0;
	int tmp = 0;
	int iItemPos = 0;
	int tmp2 = 0;
	int iChaosTaxMoney = 0;
	int iMixNeedMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if( Pos >= MAX_SOCKET_COUNT )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->ChaosLock = TRUE;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( g_SocketItem.IsSocketItem(lpObj->pChaosBox[n].m_Type) == TRUE &&
				lpObj->pChaosBox[n].m_SocketOption[Pos] != 0xFF &&
				lpObj->pChaosBox[n].m_SocketOption[Pos] != 0xFE )
			{
				iItemCount++;
				iItemPos = n;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iOtherItemCount > 0 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iItemCount != 1 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SphereRemoveMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if( iChaosTaxMoney < 0 )
		iChaosTaxMoney = 0;

	iMixNeedMoney = g_SphereRemoveMoney +iChaosTaxMoney;

	if( lpObj->Money < iMixNeedMoney )
	{
		pMsg.Result = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= iMixNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	LogChaosItem(lpObj,"SeedSphereRemove");
	g_SocketItem.ItemRemoveSeedSphere(&lpObj->pChaosBox[iItemPos],Pos);
	lpObj->ChaosLock = FALSE;

	gObjInventoryCommit(lpObj->m_Index);
	GCUserChaosBoxSend(lpObj,0);

	LogAddTD("[%s][%s] SeedSphere Remove Success - ItemInfo ( Name : %s, ItemCode : %d, Level : %d, SocketOption[%d,%d,%d,%d,%d], BonusOption : %d )",
		lpObj->AccountID,lpObj->Name,ItemAttribute[lpObj->pChaosBox[iItemPos].m_Type].Name,lpObj->pChaosBox[iItemPos].m_Type,lpObj->pChaosBox[iItemPos].m_Level,
		lpObj->pChaosBox[iItemPos].m_SocketOption[0],lpObj->pChaosBox[iItemPos].m_SocketOption[1],lpObj->pChaosBox[iItemPos].m_SocketOption[2],lpObj->pChaosBox[iItemPos].m_SocketOption[3],lpObj->pChaosBox[iItemPos].m_SocketOption[4],
		lpObj->pChaosBox[iItemPos].m_SocketBonus);

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
}

void CChaosBox::SecromiconMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	int iNeedChaosMoney;
	int iSecromiconPart1 = 0;
	int iSecromiconPart2 = 0;
	int iSecromiconPart3 = 0;
	int iSecromiconPart4 = 0;
	int iSecromiconPart5 = 0;
	int iSecromiconPart6 = 0;
	int iOtherItemCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,103) )
			{
				iSecromiconPart1++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,104) )
			{
				iSecromiconPart2++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,105) )
			{
				iSecromiconPart3++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,106) )
			{
				iSecromiconPart4++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,107) )
			{
				iSecromiconPart5++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,108) )
			{
				iSecromiconPart6++;
			}
			else
			{
				iOtherItemCount = 0;
			}
		}
	}

	if( iOtherItemCount != 0 ||
		iSecromiconPart1 != 1 ||
		iSecromiconPart2 != 1 ||
		iSecromiconPart3 != 1 ||
		iSecromiconPart4 != 1 ||
		iSecromiconPart5 != 1 ||
		iSecromiconPart6 != 1 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "Secromicon");
	LogAddTD("[Secromicon] Chaos Mix Start");

	lpObj->ChaosSuccessRate = 100;
	iNeedChaosMoney = 1000000;

	int iChaosTaxMoney = (int)((__int64)iNeedChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iNeedChaosMoney += iChaosTaxMoney;

	if ( iNeedChaosMoney < 0 )
	{
		iNeedChaosMoney = 0;
	}

	if(lpObj->Money < iNeedChaosMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= iNeedChaosMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	if( rand()%100 < lpObj->ChaosSuccessRate )
	{
		int Item = ITEMGET(14,109);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		ItemSerialCreateSend(lpObj->m_Index,255,0,0,Item,0,0,0,0,0,-1,0,0);
		gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[Secromicon Mix][%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj,0);

		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

		LogAddTD("[Secromicon Mix][%s][%s] CBMix Fail %d Money : %d-%d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney);
	}
	lpObj->ChaosLock = FALSE;
}

void CChaosBox::LuckySystemItemCreate(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	CItem* LuckyCard = NULL;
	int iLuckyCardCount = 0;
	int iOtherItemCount = 0;

	BYTE gear_1[MAX_TYPE_PLAYER] = {64,62,63,62,62,65,62};
	BYTE gear_2[MAX_TYPE_PLAYER] = {69,67,68,71,66,70,72};

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() )
		{
			if( lpObj->pChaosBox[n].m_Type >= ITEMGET(13,135) &&
				lpObj->pChaosBox[n].m_Type <= ITEMGET(13,144) )
			{
				LuckyCard = &lpObj->pChaosBox[n];
				iLuckyCardCount++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iLuckyCardCount != 1 ||
		iOtherItemCount != 0 )
	{
		pMsg.Result = 0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( lpObj->Class == CLASS_MAGUMSA && 
		(LuckyCard->m_Type == ITEMGET(13, 137) || LuckyCard->m_Type == ITEMGET(13, 142) ) )
	{
		pMsg.Result = 0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( lpObj->Class == CLASS_FIGHTER && 
		(LuckyCard->m_Type == ITEMGET(13,138) || LuckyCard->m_Type == ITEMGET(13,143) ) )
	{
		pMsg.Result = 0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "LuckySystem");
	LogAddTD("[LuckySystem] Chaos Mix Start");

	int ItemGroup;
	int Item;
	int Level;
	int Option1;
	int Option2;
	int Option3;

	Option1 = TRUE;
	Option2 = 0;
	Option3 = 0;
	Level = rand()%10;

	if( rand()%2 )	{
		if( rand()%2 )	{
			Option2 = TRUE;
		}
	}

	if( rand()%2 )	{
		Option3 = 1+rand()%3;
	}

	if( rand()%2 )	{
		Level = rand()%15;
	}

	switch( LuckyCard->m_Type )
	{
	case ITEMGET(13,135):
	case ITEMGET(13,140):
		ItemGroup = 8;
		break;
	case ITEMGET(13,136):
	case ITEMGET(13,141):
		ItemGroup = 9;
		break;
	case ITEMGET(13,137):
	case ITEMGET(13,142):
		ItemGroup = 7;
		break;
	case ITEMGET(13,138):
	case ITEMGET(13,143):
		ItemGroup = 10;
		break;
	case ITEMGET(13,139):
	case ITEMGET(13,144):
		ItemGroup = 11;
		break;
	}

	if( LuckyCard->m_Type >= ITEMGET(13,135) && LuckyCard->m_Type <= ITEMGET(13,139) )
	{
		Item = ITEMGET(ItemGroup,gear_1[lpObj->Class]);
	}
	else
	{
		Item = ITEMGET(ItemGroup,gear_2[lpObj->Class]);
	}

	ItemSerialCreateSend(lpObj->m_Index,255,0,0,Item,Level,0,Option1,Option2,Option3,lpObj->m_Index,0,gSetItemOption.GenSetOption(Item));
	gObjInventoryCommit(lpObj->m_Index);

	LogAddTD("[LuckySystem][ItemMix] Chaos Card Item Mix Success [%s][%s]",lpObj->AccountID,lpObj->Name);
	LogAddTD("[LuckySystem][ItemMix] - User(ID:%s,Name:%s) Item(Name:%s,Type:%d,Level:%d,Dur:%d,Skill:%d,Luck:%d,AddOption:%d)",
		lpObj->AccountID, lpObj->Name, ItemAttribute[Item].Name,
		Item, Level, (int)255,
		Option1, Option2, Option3);

	lpObj->ChaosLock = FALSE;
}

void CChaosBox::LuckySystemJewelCreate(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	int iLuckyItemCount = 0;
	float iLuckyItemDurability = 0;
	int iOtherItemCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() )
		{
			if( lpObj->pChaosBox[n].m_bLuckySet )
			{
				iLuckyItemDurability = lpObj->pChaosBox[n].m_Durability;
				iLuckyItemCount++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iLuckyItemCount != 1 ||
		iOtherItemCount != 0 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "LuckySystem");
	LogAddTD("[LuckySystem] Chaos Mix Start");

	if( iLuckyItemDurability == 255 )
	{
		lpObj->ChaosSuccessRate = 60;
	}
	else
	{
		lpObj->ChaosSuccessRate = 10;
	}

	if( rand()%100 < lpObj->ChaosSuccessRate )
	{
		int Item = ITEMGET(14,160);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		ItemSerialCreateSend(lpObj->m_Index,255,0,0,Item,0,0,0,0,0,-1,0,0);
		gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[LuckySystem][JewelOfExtension][%s][%s] CBMix Success %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj,0);
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		LogAddTD("[LuckySystem][JewelOfExtension][%s][%s] CBMix Fail %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosLock = FALSE;
}

void CChaosBox::GoldenNSilverBoxMix(LPOBJ lpObj)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	// ----
	pMsg.Result				= CB_ERROR;	//ebp-11
	int nGoldenKey			= 0;		//ebp-18
	int nSilverKey			= 0;		//ebp-1c
	int nGoldenBox			= 0;		//ebp-20
	int nSilverBox			= 0;		//ebp-24
	int nEtcItem			= 0;		//ebp-28
	int nType				= 0;		//ebp-2c
	bool bCheckSourceItem	= false;	//ebp-30
	lpObj->ChaosLock		= true;
	// ----
	for( int i = 0; i < CHAOS_BOX_SIZE; i++ )	//ebp-34
	{
		if( lpObj->pChaosBox[i].IsItem() )
		{
			if( lpObj->pChaosBox[i].m_Type == ITEMGET(14, 113) )
			{
				nGoldenKey++;
			}
			else if( lpObj->pChaosBox[i].m_Type == ITEMGET(14, 112) )
			{
				nSilverKey++;
			}
			else if( lpObj->pChaosBox[i].m_Type == ITEMGET(14, 121) )
			{
				nGoldenBox++;
			}
			else if( lpObj->pChaosBox[i].m_Type == ITEMGET(14, 122) )
			{
				nSilverBox++;
			}
			else
			{
				nEtcItem++;
			}
		}
	}
	// ----
	if( nEtcItem >= 1 || nGoldenKey != 1 && nSilverKey != 1 )
	{
		bCheckSourceItem = false;
	}
	else if( nGoldenKey == 1 && nGoldenBox == 1 && !nSilverKey && !nSilverBox )
	{
		nType = ITEMGET(14, 123);
		bCheckSourceItem = true;
	}
	else if( nSilverKey == 1 && nSilverBox == 1 && !nGoldenKey && !nGoldenBox )
	{
		nType = ITEMGET(14, 124);
		bCheckSourceItem = true;
	}
	// ----
	if( bCheckSourceItem )
	{
		ItemSerialCreateSend(lpObj->m_Index, 0xFF, 0, 0, nType, 0, 0, 0, 0, 0, -1, 0, 0);
		gObjInventoryCommit(lpObj->m_Index);
		return;
	}
	// ----
	pMsg.Result = 1;
	DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
	lpObj->ChaosLock = false;
}

void CChaosBox::SocketItemMix(LPOBJ lpObj,int iMixId)
{
	lpObj->ChaosLock = TRUE;

	int iSocketItem = -1;
	int iNeedChaosMoney;

	int iSocketItemCount = 0;
	int iRecipeCount = 0;
	int iMaterial1Index = -1;
	int iMaterial2Index = -1;
	int iMaterialCount[9] = {0};
	int iCharmOfLuckCount = 0;
	int iOtherItemCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)	{
		if( !lpObj->pChaosBox[n].IsItem() )
			continue;

		if( g_SocketItem.IsSocketItem(lpObj->pChaosBox[n].m_Type) &&
			lpObj->pChaosBox[n].m_Level >= 7 &&
			lpObj->pChaosBox[n].m_Option3 >= 1 )
		{
			iSocketItemCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,191) )
		{
			iSocketItem = ITEMGET(0,29);
			iMaterial1Index = 0;
			iMaterial2Index = 8;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,192) )
		{
			iSocketItem = ITEMGET(0,36);
			iMaterial1Index = 1;
			iMaterial2Index = 5;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,193) )
		{
			iSocketItem = ITEMGET(0,37);
			
			iMaterial1Index = 2;
			iMaterial2Index = 8;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,194) )
		{
			iSocketItem = ITEMGET(3,12);
			iMaterial1Index = 3;
			iMaterial2Index = 5;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,195) )
		{
			iSocketItem = ITEMGET(2,20);
			iMaterial1Index = 2;
			iMaterial2Index = 6;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,196) )
		{
			iSocketItem = ITEMGET(4,25);
			iMaterial1Index = 0;
			iMaterial2Index = 5;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,197) )
		{
			iSocketItem = ITEMGET(4,26);
			iMaterial1Index = 4;
			iMaterial2Index = 7;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,198) )
		{
			iSocketItem = ITEMGET(5,35);
			iMaterial1Index = 2;
			iMaterial2Index = 5;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,199) )
		{
			iSocketItem = ITEMGET(5,37);
			iMaterial1Index = 4;
			iMaterial2Index = 6;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,200) )
		{
			iSocketItem = ITEMGET(0,30);
			iMaterial1Index = 4;
			iMaterial2Index = 8;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,201) )
		{
			iSocketItem = ITEMGET(2,19);
			iMaterial1Index = 1;
			iMaterial2Index = 7;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,202) )
		{
			iSocketItem = ITEMGET(5,32);
			iMaterial1Index = 4;
			iMaterial2Index = 5;
			iRecipeCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,180) )
		{
			iMaterialCount[0]++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,181) )
		{
			iMaterialCount[1]++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,182) )
		{
			iMaterialCount[2]++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,183) )
		{
			iMaterialCount[3]++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,184) )
		{
			iMaterialCount[4]++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,185) )
		{
			iMaterialCount[5]++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,186) )
		{
			iMaterialCount[6]++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,187) )
		{
			iMaterialCount[7]++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,188) )
		{
			iMaterialCount[8]++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )
		{
			iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
		}
		else
		{
			iOtherItemCount++;
		}
	}

	if( iCharmOfLuckCount > 10 )	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iOtherItemCount != 0 ||
		iSocketItemCount != 3 ||
		iRecipeCount != 1 ||
		iMaterial1Index == -1 ||
		iMaterial2Index == -1 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	for(int i = 0; i < 9; i++)	{
		if( i == iMaterial1Index && iMaterialCount[i] == 2 )
			continue;

		if( i == iMaterial2Index && iMaterialCount[i] == 1 )
			continue;

		if( iMaterialCount[i] != 0 )	{
			pMsg.Result = 7;
			DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
			lpObj->ChaosLock = FALSE;
			return;
		}
	}

	LogChaosItem(lpObj, "SocketItemMix");
	LogAddTD("[SocketItemMix] Chaos Mix Start");

	lpObj->ChaosSuccessRate = 40;
	iNeedChaosMoney = 1000000;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	int iChaosTaxMoney = (int)((__int64)iNeedChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iNeedChaosMoney += iChaosTaxMoney;

	if ( iNeedChaosMoney < 0 )
	{
		iNeedChaosMoney = 0;
	}

	if(lpObj->Money < iNeedChaosMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= iNeedChaosMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	if( rand() % 100 < lpObj->ChaosSuccessRate )
	{
		int Option1 = TRUE;
		int Option2 = 0;

		if( rand() % 1000 <= 50 )	{
			Option2 = TRUE;
		}

		int Rand1 = rand() % 4;
		int Rand2 = rand() % 1000;

		int Option3 = 0;

		switch( Rand1 )
		{
		case 1:
			if( Rand2 < 120 )
			{
				Option3 = 1;
			}
			break;
		case 2:
			if( Rand2 < 60 )
			{
				Option3 = 2;
			}
			break;
		case 3:
			if( Rand2 < 30 )
			{
				Option3 = 3;
			}
			break;
		}
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		ItemSerialCreateSend(lpObj->m_Index,255,0,0,iSocketItem,0,0,Option1,Option2,Option3,-1,0,0);
		gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[SocketItemMix][Level 02] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj,0);

		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

		LogAddTD("[SocketItemMix] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);
		lpObj->ChaosLock = FALSE;
	}
}

void CChaosBox::Wing25Mix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	int iNeedChaosMoney;
	int iWingID = -1;
	int iMaterialID = -1;

	int iWingValue = 0;
	int iWingCount = 0;
	int iChaosCount = 0;
	int iCreationCount = 0;
	int iMaterialCount = 0;
	int iOtherItemCount = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)	{

		if( !lpObj->pChaosBox[n].IsItem() )
			continue;

		if( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
		{
			iChaosCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,22) )
		{
			iCreationCount++;
		}
		else if( (lpObj->pChaosBox[n].m_Type >= ITEMGET(12,3) && lpObj->pChaosBox[n].m_Type <= ITEMGET(12,6)) ||
			lpObj->pChaosBox[n].m_Type == ITEMGET(12,42) ||
			lpObj->pChaosBox[n].m_Type == ITEMGET(12,49) ||
			lpObj->pChaosBox[n].m_Type == ITEMGET(13,30) )
		{
			iWingID = lpObj->pChaosBox[n].m_Type;
			iWingValue = lpObj->pChaosBox[n].m_BuyMoney;
			iWingCount++;
		}
		else if( (lpObj->pChaosBox[n].m_Type >= ITEMGET(14,176) && lpObj->pChaosBox[n].m_Type <= ITEMGET(14,179)) )
		{
			iMaterialID = lpObj->pChaosBox[n].m_Type;
			iMaterialCount++;
		}
		else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )
		{
			iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
		}
		else
		{
			iOtherItemCount++;
		}

	}

	if( iCharmOfLuckCount > 10 )	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iOtherItemCount != 0 ||
		iMaterialCount != 1 ||
		iWingCount != 1 ||
		iCreationCount != 1 ||
		iChaosCount != 1 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "25WingLevel");
	LogAddTD("[25Wing Mix] Chaos Mix Start");

	int iDivValue = 9000000;

	if( iWingID == ITEMGET(13,30) || iWingID == ITEMGET(12,49) )	
	{
		iDivValue = 500000;
	}

	lpObj->ChaosSuccessRate = iWingValue/iDivValue;

	if( lpObj->ChaosSuccessRate < 0 )
		lpObj->ChaosSuccessRate = 1;

	if( lpObj->ChaosSuccessRate > 60 )
		lpObj->ChaosSuccessRate = 60;

	iNeedChaosMoney = lpObj->ChaosSuccessRate*100000;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	int iChaosTaxMoney = (int)((__int64)iNeedChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iNeedChaosMoney += iChaosTaxMoney;

	if ( iNeedChaosMoney < 0 )
	{
		iNeedChaosMoney = 0;
	}

	if(lpObj->Money < iNeedChaosMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= iNeedChaosMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	if( rand() % 100 < lpObj->ChaosSuccessRate )
	{
		int Item = ITEMGET(12,262+(iMaterialID-ITEMGET(14,176)));

		int Option1 = 0;
		int Option2 = 0;

		if( rand() % 1000 <= iOptionRate )
		{
			Option2 = TRUE;
		}

		int Option3 = 0;
		int ExcOption = 0;

		int Rand = rand() % 1000;

		if( Rand < 400 )
		{
			ExcOption |= 0x10;
		}

		int Rand2 = rand() % 4;
		int Rand3 = rand() % 1000;

		Option3 = 0;

		switch( Rand2 )
		{
		case 1:
			if( Rand3 < 120 )
				Option3 = 1;
			break;
		case 2:
			if( Rand3 < 60 )
				Option3 = 2;
			break;
		case 3:
			if( Rand3 < 30 )
				Option3 = 3;
			break;
		}

		int Rand4 = rand() % 2;
		int Rand5 = rand() % 1000;

		switch( Rand4 )
		{
		case 0:
			if( Rand5 < 40 )
			{
				ExcOption |= 0x01;
			}
			break;
		case 1:
			if( Rand5 < 20 )
			{
				ExcOption |= 0x02;
			}
			break;
		}
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		ItemSerialCreateSend(lpObj->m_Index,255,0,0,Item,0,0,Option1,Option2,Option3,-1,ExcOption,0);
		gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[25Wing Mix] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj,0);

		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

		LogAddTD("[25Wing Mix] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);

		lpObj->ChaosLock = FALSE;
	}
}

#if (CUSTOM_WINGS == 1)

void CChaosBox::Create4thWings(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	int iSocketItemCount = 0;
	int iWingCount = 0;
	int iCondorFeatherCount = 0;
	int iFeatherCount = 0;
	int iChaosCount = 0;
	int iCreationCount =0;
	int iGuardianCount =0;
	int iOtherItemCount = 0;
	int iNeedChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( g_SocketItem.IsSocketItem(lpObj->pChaosBox[n].m_Type) &&
				lpObj->pChaosBox[n].m_Level >= 9 &&
				lpObj->pChaosBox[n].m_Option3 >= 1 )
			{
				iSocketItemCount++;
			}
			else if( ( (lpObj->pChaosBox[n].m_Type >= ITEMGET(12,36) && lpObj->pChaosBox[n].m_Type <= ITEMGET(12,40)) || lpObj->pChaosBox[n].m_Type == ITEMGET(12,43) || lpObj->pChaosBox[n].m_Type == ITEMGET(12,50) ) &&
						lpObj->pChaosBox[n].m_Level >= 9 &&
						lpObj->pChaosBox[n].m_Option3 >= 1 )
			{
				iWingCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iCondorFeather )
			{
				iCondorFeatherCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(13,14) )
			{
				iFeatherCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iChaosJewel )
			{
				iChaosCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == iCreationJewel )
			{
				iCreationCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,31) )
			{
				iGuardianCount++;
			}
			else if( lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )
			{
				iCharmOfLuckCount += (int)lpObj->pChaosBox[n].m_Durability;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iCharmOfLuckCount > 10 )
	{
		pMsg.Result = 0xF0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if( iSocketItemCount != 1 || 
		iWingCount != 1 ||
		iCondorFeatherCount != 1 ||
		iFeatherCount != 1 ||
		iChaosCount != 1 ||
		iCreationCount != 1 ||
		iGuardianCount != 1 ||
		iOtherItemCount > 0 )
	{
		pMsg.Result = 7;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "4thWingLevel");
	LogAddTD("[4thWing Mix] Chaos Mix Start");

	lpObj->ChaosSuccessRate = 35;
	iNeedChaosMoney = 100000000;
	lpObj->ChaosSuccessRate += iCharmOfLuckCount;

	int iChaosTaxMoney = (int)((__int64)iNeedChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iNeedChaosMoney += iChaosTaxMoney;

	if ( iNeedChaosMoney < 0 )
	{
		iNeedChaosMoney = 0;
	}

	if(lpObj->Money < iNeedChaosMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= iNeedChaosMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index,lpObj->Money);

	if( rand() % 100 < lpObj->ChaosSuccessRate )
	{
		int Item = ITEMGET(12,440) + rand()%6;

		int Option1 = 0;
		int Option2 = 0;

		if( rand() % 1000 <= iOptionRate )
		{
			Option2 = TRUE;
		}

		int Option3 = 0;
		int ExcOption = 0;

		int Rand = rand() %2;
		int Rand2;

		switch( Rand )
		{
		case 0:
			Rand2 = rand() % 1000;

			if( Rand2 < 400 )
			{
				ExcOption |= 0x10;
			}
			break;
		case 1:

			Rand2 = rand() % 1000;

			if( Rand2 < 300 )
			{
				ExcOption |= 0x20;
			}
			break;
		}

		int Rand3 = rand() % 4;
		int Rand4 = rand() % 1000;

		Option3 = 0;

		switch( Rand3 )
		{
		case 1:
			if( Rand4 < 120 )
			{
				Option3 = 1;
			}
			break;
		case 2:
			if( Rand4 < 60 )
			{
				Option3 = 2;
			}
			break;
		case 3:
			if( Rand4 < 30 )
			{
				Option3 = 3;
			}
			break;
		}

		int Rand5 = rand() % 4;
		int Rand6 = rand() % 1000;

		switch( Rand5 )
		{
		case 0:
			if( Rand6 < 40 )
			{
				ExcOption |= 0x01;
			}
			break;
		case 1:
			if( Rand6 < 20 )
			{
				ExcOption |= 0x02;
			}
			break;
		case 2:
			if( Rand6 < 70 )
			{
				ExcOption |= 0x04;
			}
			break;
		case 3:
			if( Rand6 < 70 )
			{
				ExcOption |= 0x08;
			}
			break;
		}
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		ItemSerialCreateSend(lpObj->m_Index,255,0,0,Item,0,0,Option1,Option2,Option3,-1,ExcOption,0);
		gObjInventoryCommit(lpObj->m_Index);
		LogAddTD("[4thWing Mix][Level 02] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		ChaosBoxInit(lpObj);
		GCUserChaosBoxSend(lpObj,0);

		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

		LogAddTD("[4thWing Mix] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			lpObj->AccountID,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->Money,iNeedChaosMoney,iCharmOfLuckCount);

		lpObj->ChaosLock = FALSE;
	}
}

#endif

#ifdef __NOVUS__
void CChaosBox::RareItem(LPOBJ lpUser)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	// ----
	pMsg.Result				= CB_ERROR;
	int RareItemCount		= 0;
	int ItemType			= 0;
	int EtcItem				= 0;
	bool bCheckSourceItem	= false;
	lpUser->ChaosLock		= true;
	// ----
	for( int i = 0; i < CHAOS_BOX_SIZE; i++ )
	{
		if( lpUser->pChaosBox[i].IsItem() )
		{
			if( lpUser->pChaosBox[i].m_Type >= ITEMGET(14, 58) && lpUser->pChaosBox[i].m_Type <= ITEMGET(14, 62)
				|| lpUser->pChaosBox[i].m_Type >= ITEMGET(14, 145) && lpUser->pChaosBox[i].m_Type <= ITEMGET(14, 150) )
			{
				RareItemCount++;
				ItemType = lpUser->pChaosBox[i].m_Type;
			}
			else
			{
				EtcItem++;
			}
		}
	}
	// ----
	if( EtcItem >= 1 || RareItemCount != 1 )
	{
		bCheckSourceItem = false;
	}
	// ----
	if( bCheckSourceItem )
	{
		int MixItem = ITEMGET(14, 16);
		//switch(ItemType)
		// ----
		ItemSerialCreateSend(lpUser->m_Index, 0xFF, 0, 0, MixItem, 0, 0, 0, 0, 0, -1, 0, 0);
		gObjInventoryCommit(lpUser->m_Index);
		return;
	}
	// ----
	pMsg.Result = 1;
	DataSend(lpUser->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
	lpUser->ChaosLock = false;
}
#endif