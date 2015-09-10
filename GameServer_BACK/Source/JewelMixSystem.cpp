// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
// Gs-N 0.99.60T 0x004E1EB0
//	GS-N	1.00.18	JPN	0x0050A790	-	Completed
#include "stdafx.h"
#include "JewelMixSystem.h"

#include "Gamemain.h"
#include "DSprotocol.h"
#include "LogProc.h"
#include "user.h"

CJewelMixSystem::CJewelMixSystem()
{
	return;
}

CJewelMixSystem::~CJewelMixSystem()
{
	return;
}

int CJewelMixSystem::GetJewelCount(int iIndex, int iJewelType)
{
	if ( !gObjIsConnected(iIndex) )
		return -1;

	int iItemType;

	switch ( iJewelType )
	{
		case 0:
			iItemType = ITEMGET(14,13);
			break;
		case 1:
			iItemType = ITEMGET(14,14);
			break;
		case 2:
			iItemType = ITEMGET(14,16);
			break;
		case 3:
			iItemType = ITEMGET(14,22);
			break;
		case 4:
			iItemType = ITEMGET(14,31);
			break;
		case 5:
			iItemType = ITEMGET(14,41);
			break;
		case 6:
			iItemType = ITEMGET(14,42);
			break;
		case 7:
			iItemType = ITEMGET(12,15);
			break;
		case 8:
			iItemType = ITEMGET(14,43);
			break;
		case 9:
			iItemType = ITEMGET(14,44);
			break;
		default:
			return -1;
	}	
	
	int iItemCount = gObjGetItemCountInIventory(iIndex, iItemType);
	return iItemCount;
}



struct JEWELMIX_INFO
{
	int m_iJewelCount;	// 0
	int m_iMixMoney;	// 4
	int m_iSourceType;	// 8
	int m_iChangeType;	// C
};

static const JEWELMIX_INFO g_JewelMixInfo[10][3] = 
{
	10, 500000, ITEMGET(14,13), ITEMGET(12,30),
	20, 1000000, ITEMGET(14,13), ITEMGET(12,30),
	30, 1500000, ITEMGET(14,13), ITEMGET(12,30),

	10, 500000, ITEMGET(14,14), ITEMGET(12,31),
	20, 1000000, ITEMGET(14,14), ITEMGET(12,31),
	30, 1500000, ITEMGET(14,14), ITEMGET(12,31),

	10, 500000, ITEMGET(14,16), ITEMGET(12,136),
	20, 1000000, ITEMGET(14,16), ITEMGET(12,136),
	30, 1500000, ITEMGET(14,16), ITEMGET(12,136),

	10, 500000, ITEMGET(14,22), ITEMGET(12,137),
	20, 1000000, ITEMGET(14,22), ITEMGET(12,137),
	30, 1500000, ITEMGET(14,22), ITEMGET(12,137),

	10, 500000, ITEMGET(14,31), ITEMGET(12,138),
	20, 1000000, ITEMGET(14,31), ITEMGET(12,138),
	30, 1500000, ITEMGET(14,31), ITEMGET(12,138),

	10, 500000, ITEMGET(14,41), ITEMGET(12,139),
	20, 1000000, ITEMGET(14,41), ITEMGET(12,139),
	30, 1500000, ITEMGET(14,41), ITEMGET(12,139),

	10, 500000, ITEMGET(14,42), ITEMGET(12,140),
	20, 1000000, ITEMGET(14,42), ITEMGET(12,140),
	30, 1500000, ITEMGET(14,42), ITEMGET(12,140),

	10, 500000, ITEMGET(12,15), ITEMGET(12,141),
	20, 1000000, ITEMGET(12,15), ITEMGET(12,141),
	30, 1500000, ITEMGET(12,15), ITEMGET(12,141),

	10, 500000, ITEMGET(14,43), ITEMGET(12,142),
	20, 1000000, ITEMGET(14,43), ITEMGET(12,142),
	30, 1500000, ITEMGET(14,43), ITEMGET(12,142),

	10, 500000, ITEMGET(14,44), ITEMGET(12,143),
	20, 1000000, ITEMGET(14,44), ITEMGET(12,143),
	30, 1500000, ITEMGET(14,44), ITEMGET(12,143),
};
									 

int CJewelMixSystem::GetJewelCountPerLevel(int iJewelType, int iJewelLevel)
{
	if ( !CHECK_LIMIT(iJewelLevel, 3) )
		return -1;

	if ( !CHECK_LIMIT(iJewelType, 10) )
		return -1;

	return g_JewelMixInfo[iJewelType][iJewelLevel].m_iJewelCount;
}


BOOL CJewelMixSystem::MixJewel( int iIndex, int iJewelType, int iMixType)
{
	if ( !gObjIsConnected(iIndex))
		return FALSE;

	if ( gObjCheckInventorySerial0Item(&gObj[iIndex]))
	{
		MsgOutput(iIndex, lMsg.Get(MSGGET(13,26)));
		GCAnsJewelMix(iIndex, 4);

		LogAddTD("[ANTI-HACK][Serial 0 Item] [Mix Jewel] (%s)(%s)",
			gObj[iIndex].AccountID, gObj[iIndex].Name);

		return FALSE;
	}

	if ( gObj[iIndex].m_IfState.type == 1 )
	{
		LogAddTD("[JewelMix] [%s][%s] Attempted ItemCopy using Trade Window",
			gObj[iIndex].AccountID, gObj[iIndex].Name);
		return FALSE;
	}

	if ( gObj[iIndex].ChaosLock == TRUE )
	{
		LogAddTD("[JewelMix] [%s][%s] Chaos Mix is already working",
			gObj[iIndex].AccountID, gObj[iIndex].Name);

		GCAnsJewelMix(iIndex, 0);
		return FALSE;
	}

	gObj[iIndex].ChaosLock = TRUE;

	if ( !CHECK_LIMIT(iJewelType, 10) )
	{
		LogAddTD("[JewelMix] [%s][%s] Mix iJewelType is out of bound : %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name, iJewelType);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelMix(iIndex, 2);
		return FALSE;
	}

	if ( !CHECK_LIMIT(iMixType, 3))
	{
		LogAddTD("[JewelMix] [%s][%s] iMixType is out of bound : %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name, iMixType);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelMix(iIndex, 3);
		return FALSE;
	}

	int iItemType = g_JewelMixInfo[iJewelType][iMixType].m_iSourceType;
	int iJewelCount = g_JewelMixInfo[iJewelType][iMixType].m_iJewelCount;
	int iMixMoney = g_JewelMixInfo[iJewelType][iMixType].m_iMixMoney;
	int iChangeType = g_JewelMixInfo[iJewelType][iMixType].m_iChangeType;

	if ( iJewelCount <= 0 )
	{
		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelMix(iIndex, 0);
		return FALSE;
	}

	int iUserJewelCount = gObjGetItemCountInIventory(iIndex, iItemType);

	if ( iJewelCount > iUserJewelCount )
	{
		LogAddTD("[JewelMix] [%s][%s] lack of jewel to mix : %d / %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name,
			iUserJewelCount, iJewelCount);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelMix(iIndex, 4);
		return FALSE;
	}

	if ( iMixMoney > gObj[iIndex].Money )
	{
		LogAddTD("[JewelMix] [%s][%s] lack of money to mix : %d / %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name,
			gObj[iIndex].Money, iMixMoney);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelMix(iIndex, 5);
		return FALSE;
	}

	int iDelJewelCount = 0;
	BOOL bItemDelOK = FALSE;


	int invSize = MAIN_INVENTORY_SIZE;

	if( gObj[iIndex].pInventoryExtend <= 4 )	{
		invSize = (MAIN_INVENTORY_SIZE)-(32*(4-gObj[iIndex].pInventoryExtend));
	}

	for ( int x= INVETORY_WEAR_SIZE ; x< invSize; x++)
	{
		if ( gObj[iIndex].pInventory[x].IsItem() == TRUE )
		{
			if ( gObj[iIndex].pInventory[x].m_Type == iItemType )
			{
				LogAddTD("[JewelMix] [%s][%s] Mix - Delete Jewel, Type:%d, Level:%d, Serial:%d",
					gObj[iIndex].AccountID, gObj[iIndex].Name,
					gObj[iIndex].pInventory[x].m_Type,
					gObj[iIndex].pInventory[x].m_Level,
					gObj[iIndex].pInventory[x].m_Number);

				gObjInventoryItemSet(iIndex, x, 0xFF);
				gObj[iIndex].pInventory[x].Clear();
				iDelJewelCount++;

				if ( iJewelCount <= iDelJewelCount )
				{
					bItemDelOK = TRUE;
					break;
				}
			}
		}
	}

	GCItemListSend(iIndex);

	if ( bItemDelOK == FALSE )
	{
		LogAddTD("[JewelMix] [%s][%s] lack of jewel to mix (in deleting) : %d / %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name,
			iDelJewelCount, iJewelCount);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelMix(iIndex, 0);
		return FALSE;
	}

	ItemSerialCreateSend(iIndex, 235, gObj[iIndex].X,
						gObj[iIndex].Y, iChangeType,
						iMixType, 0, 0, 0, 0,
						iIndex, 0, 0);

	gObj[iIndex].ChaosLock = FALSE;
	gObj[iIndex].Money -= iMixMoney;
	GCMoneySend(iIndex, gObj[iIndex].Money);
	GCAnsJewelMix(iIndex, 1);

	LogAddTD("[JewelMix] [%s][%s] jewel mix succeed : ItemType:%d, JewelCount:%d",
		gObj[iIndex].AccountID, gObj[iIndex].Name,
		iItemType, iJewelCount);

	return TRUE;
}


BOOL CJewelMixSystem::UnMixJewel(int iIndex, int iJewelType, int iJewelLevel, int iInventoryPos)
{
	if ( !gObjIsConnected(iIndex))
		return FALSE;

	if ( gObjCheckInventorySerial0Item(&gObj[iIndex]))
	{
		MsgOutput(iIndex, lMsg.Get(MSGGET(13,26)));
		GCAnsJewelUnMix(iIndex, 0);

		LogAddTD("[ANTI-HACK][Serial 0 Item] [UnMix Jewel] (%s)(%s)",
			gObj[iIndex].AccountID, gObj[iIndex].Name);

		return FALSE;
	}

	if ( gObj[iIndex].m_IfState.type == 1 )
	{
		LogAddTD("[JewelMix] [%s][%s] Attempted ItemCopy using Trade Window",
			gObj[iIndex].AccountID, gObj[iIndex].Name);
		return FALSE;
	}

	if ( gObj[iIndex].ChaosLock == TRUE )
	{
		LogAddTD("[JewelMix] [%s][%s] Chaos Mix is already working",
			gObj[iIndex].AccountID, gObj[iIndex].Name);

		GCAnsJewelUnMix(iIndex, 0);
		return FALSE;
	}

	gObj[iIndex].ChaosLock = TRUE;

	if ( !CHECK_LIMIT(iJewelType, 10) )
	{
		LogAddTD("[JewelMix] [%s][%s] UnMix iJewelType is out of bound : %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name, iJewelType);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelUnMix(iIndex, 2);
		return FALSE;
	}

	if ( !CHECK_LIMIT(iInventoryPos, MAIN_INVENTORY_SIZE))
	{
		LogAddTD("[JewelMix] [%s][%s] iInventoryPos is out of bound : %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name, iInventoryPos);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelUnMix(iIndex, 5);
		return FALSE;
	}

	int iInventoryItemType;
	int iInventoryItemLevel;

	if ( gObj[iIndex].pInventory[iInventoryPos].IsItem() == TRUE )
	{
		if ( gObj[iIndex].pInventory[iInventoryPos].m_Level == iJewelLevel )
		{
			iInventoryItemType = gObj[iIndex].pInventory[iInventoryPos].m_Type;
			iInventoryItemLevel = gObj[iIndex].pInventory[iInventoryPos].m_Level;
		}
		else
		{
			LogAddTD("[JewelMix] [%s][%s] iJewelLevel is different from request : %d / %d",
				gObj[iIndex].AccountID, gObj[iIndex].Name,
				gObj[iIndex].pInventory[iInventoryPos].m_Level, iJewelLevel);

			gObj[iIndex].ChaosLock = FALSE;
			GCAnsJewelUnMix(iIndex, 3);
			return FALSE;
		}
	}
	else
	{
		LogAddTD("[JewelMix] [%s][%s] Item to unmix is not exist",
			gObj[iIndex].AccountID, gObj[iIndex].Name);
	
		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelUnMix(iIndex, 4);
		return FALSE;
	}

	if ( !CHECK_LIMIT(iInventoryItemLevel, 3))
	{
		LogAddTD("[JewelMix] [%s][%s] iInventoryItemLevel is out of bound : %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name, iInventoryItemLevel);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelUnMix(iIndex, 3);
		return FALSE;
	}

	int iItemType = g_JewelMixInfo[iJewelType][iInventoryItemLevel].m_iChangeType;
	int iJewelCount = g_JewelMixInfo[iJewelType][iInventoryItemLevel].m_iJewelCount;
	int iChangeType = g_JewelMixInfo[iJewelType][iInventoryItemLevel].m_iSourceType;
	int iMixMoney = 1000000;

	if ( iInventoryItemType != iItemType )
	{
		LogAddTD("[JewelMix] [%s][%s] iItemType is different from request : %d / %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name,
			iInventoryItemType, iItemType);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelUnMix(iIndex, 6);
		return FALSE;
	}

	if ( iMixMoney > gObj[iIndex].Money )
	{
		LogAddTD("[JewelMix] [%s][%s] lack of money to unmix : %d / %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name,
			gObj[iIndex].Money, iMixMoney);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelUnMix(iIndex, 8);
		return FALSE;
	}

	int invSize = INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE;

	if( gObj[iIndex].pInventoryExtend <= 4 )	{
		invSize = (INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE)-(32*(4-gObj[iIndex].pInventoryExtend));
	}

	int iEmptyCount = 0;

	int x;
	for ( x=0;x<invSize;x++)
	{
		if ( gObj[iIndex].pInventoryMap[x] == 0xFF )
		{
			iEmptyCount++;
		}
	}

	if ( iEmptyCount < iJewelCount )
	{
		LogAddTD("[JewelMix] [%s][%s] lack of empty slot to unmix : %d / %d",
			gObj[iIndex].AccountID, gObj[iIndex].Name,
			iEmptyCount, iJewelCount);

		gObj[iIndex].ChaosLock = FALSE;
		GCAnsJewelUnMix(iIndex, 7);
		return FALSE;
	}

	LogAddTD("[JewelMix] [%s][%s] UnMix - Delete Jewel, Type:%d, Level:%d, Serial:%d",
		gObj[iIndex].AccountID, gObj[iIndex].Name,
		gObj[iIndex].pInventory[iInventoryPos].m_Type,
		gObj[iIndex].pInventory[iInventoryPos].m_Level,
		gObj[iIndex].pInventory[iInventoryPos].m_Number);

	gObjInventoryItemSet(iIndex, iInventoryPos, 0xFF);
	gObjInventoryDeleteItem(iIndex, iInventoryPos);
	GCInventoryItemDeleteSend(iIndex, iInventoryPos, 1);

	int iCrtJewelCount = 0;
	BOOL bItemCrtOK = FALSE;

	for ( x=0;x<invSize;x++)
	{
		if ( gObj[iIndex].pInventoryMap[x] == 0xFF )
		{
			ItemSerialCreateSend(iIndex, 235, gObj[iIndex].X, gObj[iIndex].Y,
								iChangeType, 0, 0, 0, 0, 0,
								iIndex, 0, 0);

			iCrtJewelCount++;

			if ( iJewelCount <= iCrtJewelCount )
			{
				bItemCrtOK = TRUE;
				break;
			}
		}
	}

	gObj[iIndex].ChaosLock = FALSE;

	if ( bItemCrtOK == TRUE )
	{
		LogAddTD("[JewelMix] [%s][%s] jewel unmix succeed : ItemType:%d, JewelCount:%d",
			gObj[iIndex].AccountID, gObj[iIndex].Name,
			iItemType, iJewelCount);

		gObj[iIndex].Money -= iMixMoney;
		GCMoneySend(iIndex, gObj[iIndex].Money);
		GCAnsJewelUnMix(iIndex, 1);
	}
	else
	{
		LogAddTD("[JewelMix] [%s][%s] jewel unmix failed : ItemType:%d, JewelCount:%d",
			gObj[iIndex].AccountID, gObj[iIndex].Name,
			iItemType, iJewelCount);

		GCAnsJewelUnMix(iIndex, 0);
	}

	return TRUE;
}