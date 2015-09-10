#include "stdafx.h"
#include "..\common\winutil.h"
#include "DBSockMng.h"
#include "MultiWareHouseSystem.h"

WareHouseSYSTEM		gWareHouseSYSTEM;

WareHouseSYSTEM::WareHouseSYSTEM()
{

}

WareHouseSYSTEM::~WareHouseSYSTEM()
{

}

// BOOL WareHouseSYSTEM::CheckRange(int aIndex)
// {
// 	LPOBJ lpObj = &gObj[aIndex];
// 
// 	int NpcData_Count = 0;
// 	int X = lpObj->X;
// 	int Y = lpObj->Y;
// 
// 	for (int n = 0; n < NpcData_Count; n++)
// 	{
// 		if (lpObj->MapNumber == this->m_NpcData[n].MapNumber)
// 		{
// 			if ((X + 3) >= this->m_NpcData[n].X && (X - 3) <= this->m_NpcData[n].X &&
// 				(Y + 3) >= this->m_NpcData[n].Y && (Y - 3) <= this->m_NpcData[n].Y)
// 			{
// 				return TRUE;
// 			}
// 		}
// 	}
// 	return FALSE;
// }

void WareHouseSYSTEM::DBSendVaultInfo(int aIndex, char * AccountID, BYTE VaultID)
{
	PMSG_VAULT_INFO pMsg = { 0 };

	PHeadSubSetB((LPBYTE)&pMsg, 0xFD, 0x04, sizeof(pMsg));

	pMsg.aIndex = aIndex;
	memcpy(pMsg.chAccountID, AccountID, 10);
	pMsg.chAccountID[10] = '\0';

	pMsg.VaultID = VaultID;

	cDBSMng.Send((PCHAR)&pMsg, sizeof(pMsg));
	LogAddTD("Requested to Change Warehouse : %s", pMsg.chAccountID);
}

void WareHouseSYSTEM::DBSendVaultResult(PMSG_VAULT_RESULT * lpMsg)
{
	LPOBJ lpObj = &gObj[lpMsg->aIndex];

	lpObj->RecvSendWare = lpMsg->Result;

	GCServerMsgStringSend("Warehouse has been changed", lpObj->m_Index, 1);
}