#include "stdafx.h"
#include "user.h"
#include "MuunSystem.h"
#include "GameMain.h"
#include "..\\include\\ReadScript.h"
#include "..\\common\\winutil.h"
#include "DBSockMng.h"
#include "ElementalSystem.h"
#include "DSProtocol.h"
#include "NewPVP.h"
#include "Shop.h"

// extern
CMuunSystem g_CMuunSystem;

#if (ENABLETEST_MUUN == 1)
// protocol.cpp part
void CGMuunItemSellRequestRecv(PMSG_MUUN_ITEM_SELLREQUEST* lpMsg, int aIndex) {
	LPOBJ lpObj = &gObj[aIndex];
	PMSG_MUUN_ITEM_SELLRESULT pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x4E, 9, sizeof(PMSG_MUUN_ITEM_SELLRESULT));
	pResult.Result = 0;
	pResult.Money = 0;
	if (gObj[aIndex].CloseType != -1) {
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if (lpObj->m_IfState.use > 0 && lpObj->m_IfState.type != 3) {
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if (!PacketCheckTime(lpObj)) {
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if (!gObjFixMuunInventoryPointer(aIndex)) {
		LogAdd("[Fix PetInv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
		// undefined return
	}
	if (gObj[aIndex].pTransaction == 1) {
		LogAddTD("[%s][%s] CGMuunItemSellRequestRecv() Failed : Transaction == 1, IF_TYPE : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);
		return;
	}
	int tShop = lpObj->TargetShopNumber;
	if (tShop < 0 || tShop > MAX_SHOP - 1) {
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if (!gObjCanItemTouch(lpObj, 3)) {
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if (lpMsg->Pos > MUUN_INVENTORY_SIZE - 1) {
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		LogAdd(lMsg.Get(MSGGET(1, 227)), lpMsg->Pos);
		return;
	}
	if (gObjCheckSerial0ItemList(&lpObj->pInventory[lpMsg->Pos]) != 0) {
		MsgOutput(aIndex, lMsg.Get(MSGGET(13, 26)));
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		LogAddTD("[ANTI-HACK][Serial 0 Item] [Sell Item] (%s)(%s) Item(%s) Pos(%d)",
			lpObj->AccountID, lpObj->Name, lpObj->pInventory[lpMsg->Pos].GetName(), lpMsg->Pos);
		return;
	}
	//if (!IsSellToNPCItem(lpObj->pMuunInventory[lpMsg->Pos].m_Type)) {
	//	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
	//	return;
	//}
	if (!lpObj->pMuunInventory[lpMsg->Pos].IsItem()) {
		pResult.Result = 0;
		pResult.Money = lpObj->Money;
		LogAdd(lMsg.Get(484), __FILE__, __LINE__, lpObj->Name, lpMsg->Pos);
		LogAdd("ID:%s CharID:%s ", lpObj->AccountID, lpObj->Name); // like a flood
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	int sellmoney = 0;
    int type = lpObj->pMuunInventory[lpMsg->Pos].m_Type;
	if (g_CMuunSystem.IsMuunItem(type)) {
		sellmoney = 500000;
	} else {
		if (!g_CMuunSystem.IsStoneofEvolution(type)) { // triple check :D
			pResult.Result = 0;
			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
		sellmoney = 200000;
	}
	if (!gObjCheckMaxZen(aIndex, sellmoney)) {
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	lpObj->Money += sellmoney;
    pResult.Result = 1;
    pResult.Money = lpObj->Money;
	//log output
	if (!lpMsg->Pos || lpMsg->Pos == 1) {
		if (!lpMsg->Pos) {
			lpObj->m_wMuunItem = -1;
		}
		g_CMuunSystem.RemoveUserMuunEffect(lpObj, lpMsg->Pos);
		GCMuunEquipmentChange(lpObj->m_Index, lpMsg->Pos);
	}
	gObjMuunInventoryDeleteItem(aIndex, lpMsg->Pos);
    gObjMakePreviewCharSet(aIndex);
	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
}

void GCMuunInventoryItemListSend(int aIndex) {
	PMSG_INVENTORYLISTCOUNT pMsgILC;
	PMSG_INVENTORYLIST pMsgIL;
	int sOfs = sizeof(PMSG_INVENTORYLISTCOUNT);
	int pMsgILSize = sizeof(PMSG_INVENTORYLIST);
	BYTE sendBuf[4096];
	LPOBJ lpObj = &gObj[aIndex];
	int itemcount = 0;
	for (int n = 0; n < MUUN_INVENTORY_SIZE; n++) {
		if (!lpObj->pMuunInventory[n].IsItem()) {
			continue;
		}
		if (!lpObj->pMuunInventory[n].m_bItemExist) {
			continue;
		}
		pMsgIL.Pos = n;
		ItemByteConvert(pMsgIL.ItemInfo, lpObj->pInventory[n]);
		memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
		itemcount++;
		sOfs += pMsgILSize;
	}
	pMsgILC.h.c = 0xC4;
	pMsgILC.h.headcode = 0x4E;
	pMsgILC.subcode = 0x2;
	pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
	pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
	pMsgILC.Count = itemcount;
	memcpy(sendBuf, &pMsgILC, sizeof(PMSG_INVENTORYLISTCOUNT));
	DataSend(aIndex, sendBuf, sOfs);
}

void GCMuunItemDurSend(int aIndex, BYTE pos, BYTE dur) {
	PMSG_MUUNITEM_DUR pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4E, 0x3, sizeof(PMSG_MUUNITEM_DUR));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}

void GCMuunInventoryItemDeleteSend(int aIndex, BYTE pos, BYTE flag) {
	PMSG_PETINVENTORY_DELETE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4E, 0x4, sizeof(PMSG_PETINVENTORY_DELETE));
	pMsg.IPos = pos;
	pMsg.Flag = flag;
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}

// missing 2 calls
void GCMuunInventoryItemOneSend(int aIndex, int pos) {
	if (gObj[aIndex].pMuunInventory[pos].IsItem()) {
		PMSG_MUUN_INVEN_ITEM_MOD pMsg;
		PHeadSubSetB((LPBYTE)&pMsg, 0x4E, 0x5, sizeof(PMSG_MUUN_INVEN_ITEM_MOD));
		pMsg.Pos = pos;
		ItemByteConvert(pMsg.ItemInfo, gObj[aIndex].pMuunInventory[pos]);
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
	}
}

void CGMuunItemGetRequest(PMSG_MUUNITEM_GETREQUEST* lpMsg, int aIndex) {
	PMSG_MUUNITEM_GETRESULT pResult;
	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x4E;
	pResult.h.subcode = 0x0;
	pResult.h.size = sizeof(PMSG_MUUNITEM_GETRESULT);
	pResult.result = -1;
	if (!gObjIsConnected(aIndex)){
		CloseClient(aIndex);
		return;
	}
	if (gObj[aIndex].CloseType != -1) {
		return;
	}
	if (gObj[aIndex].DieRegen != 0) {
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if (g_NewPVP.IsObserver(gObj[aIndex])) {
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if (gObj[aIndex].m_IfState.use != 0 && gObj[aIndex].m_IfState.type != 3) {
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 94)), aIndex, 1);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if (!gObjFixMuunInventoryPointer(aIndex)) {
		LogAdd("[Fix PetInv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
		// undefined return
	}
	if (gObj[aIndex].pTransaction == 1) {
		LogAddTD("[%s][%s] CGMuunItemGetRequest() Failed : Transaction == 1, IF_TYPE : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);
		return;
	}
	int item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	if (!MAX_ITEM_TYPE_RANGE(item_num)) {
		LogAdd("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	int map_num = gObj[aIndex].MapNumber;
	if (!MAX_MAP_RANGE(map_num)) {
		LogAdd("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	CMapItem* lpItem = &MapC[map_num].m_cItem[item_num];
	if (lpItem == NULL) {
		LogAdd("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if (!lpItem->IsItem() || lpItem->Give || !lpItem->live) {
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	int type = lpItem->m_Type;
	int level = lpItem->m_Level;
	int special = lpItem->m_Special[0];
	int NOption = lpItem->m_NewOption;
	ITEM_ATTRIBUTE* pItemAttribute = GetItemAttr(type);
	if (pItemAttribute == NULL) {
		return;
	}
	if (pItemAttribute->ItemKindA != 12) {
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	pResult.result = gObjChkMuunInventoryEmpty(&gObj[aIndex]);
	if (pResult.result == (BYTE)-1) {
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	ItemByteConvert(pResult.Data, lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3,
		(BYTE)lpItem->m_Level, (int)lpItem->m_Durability, lpItem->m_NewOption, lpItem->m_SetOption,
		lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption,
		lpItem->m_SocketBonus);
	if (!MapC[map_num].ItemGive(aIndex, item_num, false)) {
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	BYTE Ret = gObjMuunInventoryInsertItem(aIndex, lpItem);
	if (Ret == (BYTE)-1) {
		pResult.result = -1;
	}
	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
	if (Ret == (BYTE)-1) {
		GCMuunInventoryItemListSend(aIndex);
	} else {
		if (g_CMuunSystem.IsStoneofEvolution(lpItem->m_Type)) {
			WORD wPetItemNumber = (lpItem->m_SocketOption[0] << 8) | lpItem->m_SocketOption[1];
			if (!GetItemAttr(wPetItemNumber)) {
				LogAdd("[MuunSystem][Error] pItemAttribute is NULL : %s %d", __FILE__, __LINE__);
				GCSendGetItemInfoForParty(aIndex, lpItem);
				return;
			}
		} else {
			// log output
			GCSendGetItemInfoForParty(aIndex, lpItem);
		}
		// log output
	}
}

void GCMuunEquipmentChange(int aIndex, int pos) {
	OBJECTSTRUCT* lpObj = &gObj[aIndex];
	PMSG_USER_MUUN_EQUIPMENT_CHANGED pChange;
	PHeadSubSetB((LPBYTE)&pChange, 0x4E, 0x6, sizeof(PMSG_USER_MUUN_EQUIPMENT_CHANGED));
	pChange.NumberH = SET_NUMBERH(aIndex);
	pChange.NumberL = SET_NUMBERL(aIndex);
	ItemByteConvert(pChange.ItemInfo, lpObj->pInventory[pos]);
	pChange.ItemInfo[I_OPTION] = 16 * pos;
	pChange.ItemInfo[I_OPTION] |= lpObj->pMuunInventory[pos].m_Level & MAX_ITEM_LEVEL | 16 * pos;
	MsgSendV2(lpObj, (LPBYTE)&pChange, pChange.h.size);
}

// muun info manager impletation

bool CMuunInfoMng::LoadScriptMuunSystemInfo(char* lpszFileName) {
	SMDFile = fopen(lpszFileName, "r");
	if (SMDFile == NULL) {
		MsgBox("[MuunSystem] file load error - fopen() : %s", lpszFileName);
		return false;
	}
	// initilize info vector
	this->m_CMuunInfo.clear();
	if (this->m_CMuunInfo.capacity() > 0) {
		std::vector<CMuunInfo>().swap(this->m_CMuunInfo);
	}
	int nIndex = -1;
	int nItemType = 0;
	int nItemIndex = 0;
	int nItemNum = 0;
	int nMuunType = 0;
	int nMuunRank = 0;
	int nOptIndex = 0;
	int nAddOptType = 0;
	int nAddOptVal = 0;
	int nAddOptStart = 0;
	int nAddOptEnd = 0;
	int iEvolutionMuunType = 0;
	int iEvolutionMuunIndex = 0;
	int iEvolutionMuunNum = 0;
	char szAddOptStart[11] = { 0 };
	char szAddOptEnd[11] = { 0 };
	while (true) {
		SMDToken Token = GetToken();
		if (Token == END) {
			break;
		}
		if (Token == NUMBER) {
			nIndex = (int)TokenNumber;
			Token = GetToken();
			nItemType = (int)TokenNumber;
			Token = GetToken();
			nItemIndex = (int)TokenNumber;
			nItemNum = ITEMGET(nItemType, nItemIndex);
			Token = GetToken();
			nMuunType = (int)TokenNumber;
			Token = GetToken();
			nMuunRank = (int)TokenNumber;
			Token = GetToken();
			nOptIndex = (int)TokenNumber;
			Token = GetToken();
			nAddOptType = (int)TokenNumber;
			Token = GetToken();
			nAddOptVal = (int)TokenNumber;
			Token = GetToken();
			strcpy(szAddOptStart, TokenString);
			Token = GetToken();
			strcpy(szAddOptEnd, TokenString);
			Token = GetToken();
			iEvolutionMuunType = (int)TokenNumber;
			Token = GetToken();
			iEvolutionMuunIndex = (int)TokenNumber;
			if (iEvolutionMuunType != -1) {
				iEvolutionMuunNum = ITEMGET(iEvolutionMuunIndex, iEvolutionMuunType);
			}
			// not need for vector
			/*if (nIndex > MUUN_MAX_INFO || nIndex < 0) {
				LogAddTD("[MuunSystem][LoadScriptMuunSystemInfo] Invalid Index");
				MsgBox("[MuunSystem] Invalid Index : %s", nIndex);
				return false;
			}*/
			CMuunInfo NewInfo;
			NewInfo.SetIndex(nIndex);
			NewInfo.SetItemNum(nItemNum);
			NewInfo.SetMuunType(nMuunType);
			NewInfo.SetMuunRank(nMuunRank);
			NewInfo.SetOptIndex(nOptIndex);
			NewInfo.SetAddOptType(nAddOptType);
			NewInfo.SetAddOptVal(nAddOptVal);
			nAddOptStart = ConvertStringToTime(szAddOptStart);
			nAddOptEnd = ConvertStringToTime(szAddOptEnd);
			NewInfo.SetAddOptStart(nAddOptStart);
			NewInfo.SetAddOptEnd(nAddOptEnd);
			NewInfo.SetEvolutionMuunNum(iEvolutionMuunNum);
			m_CMuunInfo.push_back(NewInfo);
		}
		fclose(SMDFile);
	}
	return true;
}

bool CMuunInfoMng::LoadScriptMuunSystemOption(char* lpszFileName) {
	SMDFile = fopen(lpszFileName, "r");
	if (SMDFile == NULL) {
		MsgBox("[MuunSystem] file load error - fopen() : %s", lpszFileName);
		return false;
	}
	CMuunOpt newOption;
	int nIndex = -1;
	int nMuunLvVal[MUUN_MAX_GRADE] = { 0 };
	while (true) {
		SMDToken Token = GetToken();
		if (Token == END) {
			break;
		}
		if (Token == NUMBER) {
			nIndex = (int)TokenNumber;
			Token = GetToken();
			int nOptType = (int)TokenNumber;
			Token = GetToken();
			for (int i = 0; i < MUUN_MAX_GRADE; i++) {
				Token = GetToken();
				nMuunLvVal[i] = (int)TokenNumber;
			}
			Token = GetToken();
			int nConditionType = (int)TokenNumber;
			Token = GetToken(); // description?
			Token = GetToken(); 
			int nConditionVal1 = (int)TokenNumber;
			Token = GetToken();
			int nConditionVal2 = (int)TokenNumber;
			Token = GetToken();
			//if (nIndex > MUUN_MAX_DATA || nIndex < 0) {
			//	LogAddTD("[MuunSystem][LoadScriptMuunSystemOption] Invalid Index");
			//	MsgBox("[MuunSystem] Invalid Index : %s", nIndex);
			//	return false;
			//}
			newOption.SetOptType(nOptType);
			newOption.SetConditionType(nConditionType);
			newOption.SetConditionVal1(nConditionVal1);
			newOption.SetConditionVal2(nConditionVal2);
			newOption.SetMuunLvVal(nMuunLvVal);
			InsertMuunOptionInfo(newOption, nIndex);
		}
	}
	fclose(SMDFile);
	return true;
}

bool CMuunInfoMng::InsertMuunOptionInfo(CMuunOpt* pCMuunOpt) {
	int nIndex = 0;
	int nOptType = 0;
	int nMuunLvVal[MUUN_MAX_GRADE] = { 0 };
	for (int i = 0; i < m_CMuunInfo.size(); i++) {
		int nOptIndex = m_CMuunInfo[i].GetOptIndex();
		if (nOptIndex < 0) {
			continue;
		}
		int nOptType = pCMuunOpt[nOptIndex].GetOptType();
		pCMuunOpt[nOptIndex].GetMuunLvVal(nMuunLvVal);
		m_CMuunInfo[i].SetOptType(nOptType);
		m_CMuunInfo[i].SetConditionType(pCMuunOpt[nOptIndex].GetConditionType());
		m_CMuunInfo[i].SetConditionVal1(pCMuunOpt[nOptIndex].GetConditionVal1());
		m_CMuunInfo[i].SetConditionVal2(pCMuunOpt[nOptIndex].GetConditionVal2());
		m_CMuunInfo[i].SetMuunLvVal(nMuunLvVal);
	}
	return true;
}

// overloading for save memory
bool CMuunInfoMng::InsertMuunOptionInfo(CMuunOpt pCMuunOpt, int OptionIndex) {
	int nIndex = 0;
	int nOptType = 0;
	int nMuunLvVal[MUUN_MAX_GRADE] = { 0 };
	for (int i = 0; i < m_CMuunInfo.size(); i++) {
		int nOptIndex = m_CMuunInfo[i].GetOptIndex();
		if (nOptIndex < 0) {
			continue;
		}
		if (OptionIndex != nOptIndex) {
			continue;
		}
		int nOptType = pCMuunOpt.GetOptType();
		pCMuunOpt.GetMuunLvVal(nMuunLvVal);
		m_CMuunInfo[i].SetOptType(nOptType);
		m_CMuunInfo[i].SetConditionType(pCMuunOpt.GetConditionType());
		m_CMuunInfo[i].SetConditionVal1(pCMuunOpt.GetConditionVal1());
		m_CMuunInfo[i].SetConditionVal2(pCMuunOpt.GetConditionVal2());
		m_CMuunInfo[i].SetMuunLvVal(nMuunLvVal);
		break;
	}
	return true;
}

long CMuunInfoMng::ConvertStringToTime(char* pchTime) {
	int iYear = 0;
	int iMonth = 0;
	int iDay = 0;
	int iHour = 0;
	int iMinute = 0;
	int iDateLength = 0;
	char chDate[20];
	char* Token;
	char seps[4] = "-: ";
	if (pchTime == NULL) {
		return 0;
	}
	memcpy(chDate, pchTime, sizeof(chDate));
	iDateLength = strlen(chDate);
	if (iDateLength <= 0 || iDateLength > 20) {
		return 0;
	}
	Token = strtok(chDate, seps);
	iYear = atoi(Token);
	Token = strtok(NULL, seps);
	iMonth = atoi(Token);
	Token = strtok(NULL, seps);
	iDay = atoi(Token);
	Token = strtok(NULL, seps);
	iHour = atoi(Token);
	Token = strtok(NULL, seps);
	iMinute = atoi(Token);
	CTime tReturnTime(iYear, iMonth, iDay, iHour, iMinute, 0, -1);
	return tReturnTime.GetTime();
}

CMuunInfo* CMuunInfoMng::GetMuunItemNumToMuunInfo(int iMuunItemNum) {
	for (int i = 0; i < m_CMuunInfo.size(); i++) {
		if (m_CMuunInfo[i].GetItemNum() == iMuunItemNum) {
			return &m_CMuunInfo[i];
		}
	}
	return NULL;
}

void CMuunInfoMng::CTimeToViewDate(long lDate) {
#if (MUUN_DEBUG == 1)
	CTime tTime(lDate);
	TRACE_LOG(" %d-%d-%d: %d:%d:%d ",
		tTime.GetSecond(), tTime.GetMinute(), tTime.GetHour(),
		tTime.GetDay(), tTime.GetMonth(), tTime.GetYear());
#endif
}

// muun system impletation

bool CMuunSystem::LoadScriptMuunSystemInfo(char* lpszFileName) { 
	m_MuunInfoMng.LoadScriptMuunSystemInfo(lpszFileName);
	return true;
}

bool CMuunSystem::LoadScriptMuunSystemOption(char* lpszFileName) {
	m_MuunInfoMng.LoadScriptMuunSystemOption(lpszFileName);
	return true;
}

void CMuunSystem::GDReqLoadMuunInvenItem(OBJECTSTRUCT& obj) {
	if (!OBJMAX_RANGE(obj.m_Index)) {
		return;
	}
	if (!gObjIsConnected(obj.m_Index)) {
		return;
	}
	SDHP_REQ_DBMUUN_INVEN_LOAD pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xF1, sizeof(SDHP_REQ_DBMUUN_INVEN_LOAD));
	memcpy(pMsg.AccountID, obj.AccountID, sizeof(pMsg.AccountID));
    memcpy(pMsg.Name, obj.Name, sizeof(pMsg.Name));
    pMsg.aIndex = (short)obj.m_Index;
    cDBSMng.Send((char*)&pMsg, sizeof(SDHP_REQ_DBMUUN_INVEN_LOAD));
}

void CMuunSystem::DGLoadMuunInvenItem(_tagSDHP_ANS_DBMUUN_INVEN_LOAD* lpMsg) {
	int aIndex = lpMsg->aIndex;
	char szId[11] = { 0 };
	memcpy(szId, gObj[aIndex].AccountID, MAX_ACCOUNT_LEN);
	int nEventInvenItemCount = 0;
	if (!OBJMAX_RANGE(aIndex)) {
		return;
	}
	if (gObj[aIndex].m_bMapSvrMoveReq) {
		LogAddTD("[DGLoadEventInvenItem] MapServerMove User. Can't Open Event Inven. return!! [%s], IP [%s] ",
			gObj[aIndex].AccountID, gObj[aIndex].Ip_addr);
		return;
	}
	if (gObj[aIndex].m_State == 32) {
		LogAddTD("[DGLoadEventInvenItem] MapServerMove User. Can't Open Event Inven. return!! [%s], IP [%s] ",
			gObj[aIndex].AccountID, gObj[aIndex].Ip_addr);
		return;
	}
	if (gObj[aIndex].m_bMapSvrMoveQuit) {
		LogAddTD( "[DGLoadEventInvenItem] MapServerMove User. Can't Open Event Inven. return!! [%s], IP [%s] ",
			gObj[aIndex].AccountID, gObj[aIndex].Ip_addr);
		return;
	}
	if (!gObjIsAccontConnect(aIndex, szId)) {
        LogAddC(2, lMsg.Get(431), szId, aIndex);
		return;
	}
	gObj[aIndex].bMuunInventoryLoad = true;
	OBJECTSTRUCT* lpObj = &gObj[aIndex];
	CItem item;
	int dbsize = 7;
	if (lpMsg->DbVersion >= 3) {
		dbsize = 16;
	} else if(lpMsg->DbVersion >= 1) {
		dbsize = 10;
	}
	for (int n = 0; n < MUUN_INVENTORY_SIZE; n++) {
		int itype = lpMsg->dbItems[dbsize * n];
		int _type;
		if (dbsize == 16) {
			itype |= (lpMsg->dbItems[n * dbsize + DBI_OPTION380_DATA] & 0xF0) << 5;
			itype |= (lpMsg->dbItems[n * dbsize + DBI_NOPTION_DATA] & 0x80) << 1;
			_type = itype;
		} else if (dbsize == 10) {
			if (((lpMsg->dbItems[n * dbsize + DBI_NOPTION_DATA] >> 7) & 0x01)) {
				itype += 256;
			}
			_type = (itype / 32 * MAX_SUBTYPE_ITEMS) + itype % 32;
		} else {
			_type = (itype / MAX_TYPE_ITEMS * MAX_SUBTYPE_ITEMS) + itype % 16;
		}
		lpObj->pMuunInventory[n].Clear();
		if (dbsize >= 16)
		{
			if (lpMsg->dbItems[n * dbsize] == (BYTE)-1 
				&& (lpMsg->dbItems[n * dbsize + DBI_OPTION380_DATA] & 0xF0) == 0xF0 
				&& (lpMsg->dbItems[n * dbsize + 7] & 0x80) == 0x80) {
				itype = -1;
			}
		} else if (dbsize >= 10) {
			if (lpMsg->dbItems[n * dbsize] == (BYTE)-1 
				&& (lpMsg->dbItems[n * dbsize + DBI_NOPTION_DATA] & 0x80) == 0x80) {
				itype = -1;
			}
		} else {
			if (lpMsg->dbItems[n*dbsize] == (BYTE)-1) {
				itype = -1;
			}
		}
		if (!IsItem(_type)) {
			itype = -1;
		}
		if (itype == -1) {
			continue;
		}
		item.m_Level = DBI_GET_LEVEL(lpMsg->dbItems[n * dbsize + DBI_OPTION_DATA]);
		if (dbsize == 7 && item.m_Level > 11) {
			item.m_Level = 0;
		}
		if (lpMsg->DbVersion < 2) {
			lpMsg->dbItems[n * dbsize + DBI_SOPTION_DATA] = -1;
			lpMsg->dbItems[n * dbsize + DBI_OPTION380_DATA] = -1;
		}
		BYTE OptionData = lpMsg->dbItems[n * dbsize + DBI_OPTION_DATA];
		item.m_Option1 = DBI_GET_SKILL(OptionData);
		item.m_Option2 = DBI_GET_LUCK(OptionData);
		item.m_Option3 = DBI_GET_OPTION(OptionData);
		item.m_Option3 |= DBI_GET_OPTION16(lpMsg->dbItems[n * dbsize + DBI_NOPTION_DATA]);
		item.m_Durability = lpMsg->dbItems[n * dbsize + DBI_DUR];
		item.m_JewelOfHarmonyOption = lpMsg->dbItems[n * dbsize + DBI_JOH_DATA];
		if (dbsize >= 10) {
			item.m_ItemOptionEx = DBI_GET_380OPTION(lpMsg->dbItems[n * dbsize + DBI_OPTION380_DATA]);
		}
		if (dbsize >= 16) {
			if (item.m_ItemOptionEx != 0) {
				item.m_Type = itype;
				if (!g_kItemSystemFor380.Is380Item(&item)) {
					item.m_ItemOptionEx = 0;
					LogAddTD("[380Item][%s][%s] Invalid 380 Item Option in Warehouse pos[%d]",
						lpObj->AccountID, lpObj->Name, n);
				}
			}
		}
		BYTE SocketOptions[MAX_SOCKET_COUNT];
		BYTE SocketBonus = 0xFF;
		memset(SocketOptions, 0xFF, MAX_SOCKET_COUNT);
		if (g_SocketItem.IsSocketItem(itype) 
			|| g_ElementalSystem.IsElementalItem(itype)
			|| IsMuunItem(itype)) {
			SocketBonus = lpMsg->dbItems[n * dbsize + 10];

			for(int i = 0; i < MAX_SOCKET_COUNT; i++) {
				SocketOptions[i] = lpMsg->dbItems[n * dbsize + 11 + i];
			}
		}
		else {
			SocketBonus = 0;
			for(int i = 0; i < MAX_SOCKET_COUNT; i++) {
				SocketOptions[i] = 0xFF;
			}
		}
		item.m_bLOCKED = ((lpMsg->dbItems[n * dbsize+DBI_SOPTION_DATA] & 0xC0) == 0xC0) ? TRUE : FALSE;
		if (lpMsg->DbVersion == 0) {
			item.Convert(itype, item.m_Option1, item.m_Option2, item.m_Option3, 0, 0, 0, 0, 0xFF, 3);
		}
		else if (lpMsg->DbVersion <= 2) {
			item.Convert(itype, item.m_Option1, item.m_Option2, item.m_Option3, DBI_GET_NOPTION(lpMsg->dbItems[n * dbsize + DBI_NOPTION_DATA]), lpMsg->dbItems[n * dbsize + DBI_SOPTION_DATA], item.m_ItemOptionEx, 0, 0xFF, 1);
		} else {
			item.Convert(itype, item.m_Option1, item.m_Option2, item.m_Option3, DBI_GET_NOPTION(lpMsg->dbItems[n * dbsize + DBI_NOPTION_DATA]), lpMsg->dbItems[n * dbsize + DBI_SOPTION_DATA], item.m_ItemOptionEx, &SocketOptions[0], SocketBonus, 3);
		}
		lpObj->pMuunInventory[n].m_Option1 = item.m_Option1;
		lpObj->pMuunInventory[n].m_Option2 = item.m_Option2;
		lpObj->pMuunInventory[n].m_Option3 = item.m_Option3;
		lpObj->pMuunInventory[n].m_JewelOfHarmonyOption = item.m_JewelOfHarmonyOption;
		lpObj->pMuunInventory[n].m_ItemOptionEx = item.m_ItemOptionEx;
		WORD hiword = MAKE_NUMBERW(lpMsg->dbItems[n * dbsize + DBI_SERIAL1], lpMsg->dbItems[n * dbsize + DBI_SERIAL2]);
		WORD loword = MAKE_NUMBERW(lpMsg->dbItems[n * dbsize + DBI_SERIAL3], lpMsg->dbItems[n * dbsize + DBI_SERIAL4]);
		item.m_Number = MAKE_NUMBERDW(hiword, loword);
		if (IsMuunItem(&item) && !item.IsMuunItmePeriodExpire()) {
			CMuunInfo* pCMuunInfo = m_MuunInfoMng.GetMuunItemNumToMuunInfo(itype);
			if (pCMuunInfo == NULL) {
				 LogAddTD("[MuunSystem][Error] [%s][%s] DGLoadMuunInvenItem() pCMuunInfo is NULL %s %d",
					 lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
			}
			if (CheckAddOptionExpireDate(pCMuunInfo->GetAddOptStart(), pCMuunInfo->GetAddOptEnd())) {
				item.SetMuunItmePeriodExpire();
			} else {
				SetMuunItemAddPeriodData(lpObj, itype, item.m_Number);
			}
			gObjMuunInventoryInsertItemPos(lpObj->m_Index, item, n);
			if (n < 2) {
				MuunItemEquipment(lpObj->m_Index, n, -1);
				if (n == 0) {
					GCMuunEquipmentChange(lpObj->m_Index, 0);
				}
			}
			nEventInvenItemCount++;
		}
	}
	// missing log
	LogAddTD("[MuunInvenItem] [%s][%s] ItemCount[%d] Muun Item Load Complete.",
		lpObj->AccountID, lpObj->Name, nEventInvenItemCount);
	GCMuunInventoryItemListSend(aIndex);
	lpObj->dwCheckMuunItemTime = GetTickCount();
	if (lpMsg->SubEquip > 0) {
		lpObj->bSubEquip = 1;
		PMSG_MUUN_INVEN_SUB_EQUIP_OPEN pMsg;
		PHeadSubSetB((LPBYTE)&pMsg, 0x4E, 0x10, sizeof(PMSG_MUUN_INVEN_SUB_EQUIP_OPEN));
		pMsg.Result = lpMsg->SubEquip;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
	}
}

void CMuunSystem::GDReqSaveMuunInvenItem(OBJECTSTRUCT& obj) {
	if (!obj.bMuunInventoryLoad) {
		return;
	}
	if (!OBJMAX_RANGE(obj.m_Index)) {
		return;
	}
	// missing log
	/*for (int n = 0; n < MUUN_INVENTORY_SIZE; n++) {
        if (!obj.pMuunInventory[n].IsItem()) {
			continue;
		}
		if (!IsStoneofEvolution(obj.pMuunInventory[n].m_Type)) {
			continue;
		}
	}*/
	LogAddTD("[MuunInvenItem] [%s][%s] Muun Item Save Complete.", obj.AccountID, obj.Name);
	_tagSDHP_REQ_DBMUUN_INVEN_SAVE pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xF2, sizeof(_tagSDHP_REQ_DBMUUN_INVEN_SAVE));
	memcpy(pMsg.AccountID, obj.AccountID, sizeof(pMsg.Name));
    memcpy(pMsg.Name, obj.Name, sizeof(pMsg.Name));
	ItemByteConvert16(pMsg.dbInventory, obj.pMuunInventory, MUUN_INVENTORY_SIZE);
    cDBSMng.Send((char*)&pMsg, sizeof(_tagSDHP_REQ_DBMUUN_INVEN_SAVE));
}

void CMuunSystem::GCSendConditionStatus(int aIndex, int iPos, int iStatus) {
	PMSG_MUUNITEM_CONDITION_STATUS pMsg;
	pMsg.btStatus = iStatus;
	pMsg.btIPos = iPos;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4E, 0x7, sizeof(PMSG_MUUNITEM_CONDITION_STATUS));
	MsgIsMuunItemActive(&gObj[aIndex], iPos);
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}

bool CMuunSystem::IsMuunItem(int iItemNum) {
	ITEM_ATTRIBUTE* pItemAttribute = GetItemAttr(iItemNum);
	if (pItemAttribute == NULL) {
		return false;
	}
	return pItemAttribute->ItemKindA == 12 && pItemAttribute->ItemKindB;
}

bool CMuunSystem::IsMuunItem(CItem* pCitem) { 
	if (pCitem == NULL) {
		return false;
	}
	ITEM_ATTRIBUTE* pItemAttribute = GetItemAttr(pCitem->m_Type);
	if (pItemAttribute == NULL) {
		return false;
	}
	return pItemAttribute->ItemKindA == 12 && pItemAttribute->ItemKindB;
}

bool CMuunSystem::IsStoneofEvolution(CItem* pCitem) {
	if (pCitem == NULL) {
		return false;
	}
	ITEM_ATTRIBUTE* pItemAttribute = GetItemAttr(pCitem->m_Type);
	if (pItemAttribute == NULL) {
		return false;
	}
	return pItemAttribute->ItemKindA == 12 && !pItemAttribute->ItemKindB;
}

bool CMuunSystem::IsStoneofEvolution(int iItemNum) {
	ITEM_ATTRIBUTE* pItemAttribute = GetItemAttr(iItemNum);
	if (pItemAttribute == NULL) {
		return false;
	}
	return pItemAttribute->ItemKindA == 12 && !pItemAttribute->ItemKindB;
}

int CMuunSystem::GetMuunRankOfMuunInfo(int iItemNum) {
	CMuunInfo* pCMuunInfo = m_MuunInfoMng.GetMuunItemNumToMuunInfo(iItemNum);
	if (pCMuunInfo == NULL) {
		LogAddTD("[MuunSystem][Error] GetMuunRankOfMuunInfo() pCMuunInfo is NULL %s %d",
			__FILE__, __LINE__);
		return 0;
	}
	return pCMuunInfo->GetMuunRank();
}

int CMuunSystem::GetEvolutionMuunItemIndex(int iItemNum) { 
	CMuunInfo* pCMuunInfo = m_MuunInfoMng.GetMuunItemNumToMuunInfo(iItemNum);
	if (pCMuunInfo == NULL) {
		LogAddTD("[MuunSystem][Error] GetMuunRankOfMuunInfo() pCMuunInfo is NULL %s %d",
			__FILE__, __LINE__);
		return 0;
	}
	return pCMuunInfo->GetEvolutionMuunNum();
}

void CMuunSystem::CGMuunInventoryUseItemRecv(PMSG_USEITEM_MUUN_INVEN* lpMsg, int aIndex) {
	int iItemUseType = lpMsg->btItemUseType;
	if (gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type != 3) {
		LogAdd("[%s][%s] CGMuunInventoryUseItemRecv()_If return %s %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, __FILE__, __LINE__);
		GCMuunInventoryUseItemResult(aIndex, iItemUseType, 1);
		return;
	}
	if (gObj[aIndex].CloseType != -1) {
		LogAdd("[%s][%s] CGMuunInventoryUseItemRecv()_CloseType return %s %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, __FILE__, __LINE__);
		GCMuunInventoryUseItemResult(aIndex, iItemUseType, 1);
		return;
	}
	if (!gObjFixMuunInventoryPointer(aIndex)) {
		LogAdd("[Fix PetInv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
		// undefined return
	}
	if (gObj[aIndex].pTransaction == 1) {
		LogAddTD("[%s][%s] CGMuunInventoryUseItemRecv() Failed : Transaction == 1, IF_TYPE : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);
		return;
	}
	if (g_NewPVP.IsObserver(gObj[aIndex])) {
		return;
	}
	int nSourcepos = lpMsg->inventoryPos;
	if (lpMsg->inventoryPos == lpMsg->invenrotyTarget) {
		LogAdd( "error-L1 : [%s][%s] CGMuunInventoryUseItemRecv()_Pos return %s %d", 
			gObj[aIndex].AccountID, gObj[aIndex].Name, __FILE__, __LINE__);
		GCMuunInventoryUseItemResult(aIndex, iItemUseType, 1);
		return;
	}
	int nResult = 0;
	switch (iItemUseType) {
			  case 1:
				  if (MuunItemLevelUp(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget)) {
					  gObj[aIndex].pMuunInventory[nSourcepos].Clear();
					  GCMuunInventoryItemDeleteSend(aIndex, nSourcepos, 1);
					  GCMuunInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				  } else {
					  LogAddTD("[MuunSystem][LevelUp] [%s][%s] [Fail]", gObj[aIndex].AccountID, gObj[aIndex].Name);
					  nResult = 1;
				  }
				  break;
			  case 2:
				  if (MuunItemEvolution(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget)) {
					  gObj[aIndex].pMuunInventory[nSourcepos].Clear();
					  GCMuunInventoryItemDeleteSend(aIndex, nSourcepos, 1);
				  } else {
					  LogAddTD("[MuunSystem] [%s][%s]Item Use Muun Item Evolution Fail",
						  gObj[aIndex].AccountID, gObj[aIndex].Name);
					  nResult = 1;
				  }
				  break;
			  case 3:
				  if (MuunItemLifeGem(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget)) {
					  gObjInventoryItemSet(aIndex, nSourcepos, -1);
					  gObj[aIndex].pInventory[nSourcepos].Clear();
					  GCInventoryItemDeleteSend(aIndex, nSourcepos, 1);
					  GCMuunItemDurSend(aIndex, lpMsg->invenrotyTarget, -1);
				  } else {
					  LogAddTD( "[MuunSystem] [%s][%s]Item Use Muun Item LifeGem Fail",
						  gObj[aIndex].AccountID, gObj[aIndex].Name);
					  nResult = 1;
				  }
				  break;
	}
	GCMuunInventoryUseItemResult(aIndex, iItemUseType, nResult);
}

void CMuunSystem::GCMuunInventoryUseItemResult(int aIndex, int iUseType, int iResult) {
	PMSG_USEITEM_MUUN_INVEN_RESULT pResult;
	pResult.btItemUseType = iUseType;
	pResult.btResult = iResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x4E, 0x8, sizeof(PMSG_USEITEM_MUUN_INVEN_RESULT));
	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
}

bool CMuunSystem::MuunItemLevelUp(OBJECTSTRUCT* lpObj, int source, int target) { 
	int aIndex = lpObj->m_Index;
	if (source < 2 || source > MUUN_INVENTORY_SIZE - 1) {
		return false;
	}
	if (target < 2 || target > MUUN_INVENTORY_SIZE - 1) {
		return false;
	}
	if (!lpObj->pMuunInventory[source].IsItem() 
		|| !lpObj->pMuunInventory[target].IsItem()) {
		return false;
	}
	if (lpObj->pMuunInventory[source].m_Level < 1
		|| lpObj->pMuunInventory[target].m_Level < 1) {
		return false;
	}
	if (lpObj->pMuunInventory[source].m_Type != lpObj->pMuunInventory[target].m_Type) {
		return false;
	}
	if (lpObj->pMuunInventory[source].m_Level != 1) {
		return false;
	}
	if (lpObj->pMuunInventory[target].GetMuunItemRank() + 1 == lpObj->pMuunInventory[target].m_Level) {
		return false;
	}
	if (lpObj->pMuunInventory[target].m_Level >= 5) { // max level define
		return false;
	}
	if (lpObj->pMuunInventory[target].GetMuunItemRank() + 1 < lpObj->pMuunInventory[target].m_Level) {
		LogAddTD("[MuunSystem][Error][%s][%s] Over Level(%d) %s %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name,
			lpObj->pMuunInventory[target].m_Level, __FILE__, __LINE__);
		return false;
	}
	int nBeforeLv = lpObj->pMuunInventory[target].m_Level;
	lpObj->pMuunInventory[target].m_Level++;
    ITEM_ATTRIBUTE* pSItemAttribute = GetItemAttr(gObj[aIndex].pMuunInventory[source].m_Type);
    ITEM_ATTRIBUTE* pTItemAttribute = GetItemAttr(gObj[aIndex].pMuunInventory[target].m_Type);
	if (pTItemAttribute || pSItemAttribute) {
		 LogAddTD("[MuunSystem][LevelUp] [%s][%s] [Success] Source: [%s] Pos[%d] Rank[%d] Level[%d] Serial[%u] - Target: [%s] Pos[%d] Rank[%d] Level[%d]/[%d] Serial[%u]",
			 gObj[aIndex].AccountID, gObj[aIndex].Name, pSItemAttribute->Name, source, gObj[aIndex].pMuunInventory[source].GetMuunItemRank(), 
			 gObj[aIndex].pMuunInventory[source].m_Level, gObj[aIndex].pMuunInventory[source].m_Number, pTItemAttribute->Name, target, 
			 gObj[aIndex].pMuunInventory[target].GetMuunItemRank(), nBeforeLv, gObj[aIndex].pMuunInventory[target].m_Level, gObj[aIndex].pMuunInventory[target].m_Number);
	}
	return true;
}

bool CMuunSystem::MuunItemEvolution(OBJECTSTRUCT* lpObj, int source, int target) {
	int aIndex = lpObj->m_Index;
	if (source < 12 || source > MUUN_INVENTORY_SIZE - 1) {
		return false;
	}
	if (target < 2 || target > MUUN_INVENTORY_SIZE - 1) {
		return false;
	}
	if (!lpObj->pInventory[source].IsItem() || !lpObj->pInventory[target].IsItem()) {
		return false;
	}
	if (!IsStoneofEvolution(lpObj->pMuunInventory[source].m_Type) || IsMuunItem(lpObj->pMuunInventory[target].m_Type)) {
		return false;
	}
	if (lpObj->pMuunInventory[target].m_Level < 1) {
		return false;
	}
	int nEvoMuunItemNum = (lpObj->pMuunInventory[source].m_SocketOption[0] << 8) | lpObj->pMuunInventory[source].m_SocketOption[1];
	if (nEvoMuunItemNum =! lpObj->pMuunInventory[target].m_Type) {
		return false;
	}
	if (lpObj->pMuunInventory[target].GetMuunItemRank() + 1 != lpObj->pMuunInventory[target].m_Level) {
		LogAddTD("[MuunSystem][Error][%s][%s] Over Level(%d) %s %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, lpObj->pMuunInventory[target].m_Level, __FILE__, __LINE__);
		return false;
	}
	int nEvolutionMuunNum = GetEvolutionMuunItemIndex(lpObj->pMuunInventory[target].m_Type);
	if (nEvolutionMuunNum == -1) {
		LogAddTD("[MuunSystem][Error][%s][%s] Not Evolution MuunItem Index - target Item Index(%d) %s %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, lpObj->pMuunInventory[target].m_Type, __FILE__, __LINE__);
		return false;
	}
	ITEM_ATTRIBUTE* pSItemAttribute = GetItemAttr(gObj[aIndex].pMuunInventory[source].m_Type);
    ITEM_ATTRIBUTE* pTItemAttribute = GetItemAttr(gObj[aIndex].pMuunInventory[target].m_Type);
	if (pTItemAttribute || pSItemAttribute) {
		LogAddTD("[MuunSystem][Evolution] [%s][%s] [Success] Source: [%s] Pos[%d] Serial[%u] - Target: [%s] Pos[%d] Rank[%d] Level[%d] Serial[%u]",
			gObj[aIndex].AccountID, gObj[aIndex].Name, pSItemAttribute->Name, source, gObj[aIndex].pMuunInventory[source].m_Number,
			pTItemAttribute->Name, target, gObj[aIndex].pMuunInventory[target].GetMuunItemRank(), gObj[aIndex].pMuunInventory[target].m_Level,
			gObj[aIndex].pMuunInventory[target].m_Number);
	}
	float fDur = gObj[aIndex].pMuunInventory[target].m_Durability;
	ClearPeriodMuunItemData(lpObj, gObj[aIndex].pMuunInventory[target].m_Type, gObj[aIndex].pMuunInventory[target].m_Number);
	GCMuunInventoryItemDeleteSend(aIndex, target, 1);
    gObj[aIndex].pMuunInventory[target].Clear();
	int iType = nEvolutionMuunNum;
	int iLevel = 0;
	int X = 0;
	int Y = 0;
	int iOption1 = 0;
	int iOption2 = 0;
	int iOption3 = 0;
	int iExOption = 0;
	ItemSerialCreateSend(lpObj->m_Index, 0xE0, X, Y, iType, iLevel, (int)fDur, iOption1, iOption2, iOption3, lpObj->m_Index, 0, 0);
	return true;
}

bool CMuunSystem::MuunItemLifeGem(OBJECTSTRUCT* lpObj, int source, int target) {
	int aIndex = lpObj->m_Index;
	if (source < 12 || source > MAIN_INVENTORY_SIZE - 1) {
		return false;
	}
	if (target < 2 || target > MUUN_INVENTORY_SIZE - 1) {
		return false;
	}
	if (!lpObj->pInventory[source].IsItem() || !lpObj->pInventory[target].IsItem()) {
		return false;
	}
	if (!IsMuunItem(lpObj->pMuunInventory[target].m_Type) || lpObj->pInventory[source].m_Type != 7184) {
		return false;
	}
	if (lpObj->pMuunInventory[target].m_Durability == 255.0f) {
		return false;
	}
	int nBeforeDur = (int)lpObj->pMuunInventory[target].m_Durability;
	lpObj->pMuunInventory[target].m_Durability = 255.0f;
	ITEM_ATTRIBUTE* pItemAttribute = GetItemAttr(gObj[aIndex].pInventory[source].m_Type);
	if (pItemAttribute) {
		LogAddTD("[MuunSystem] [%s][%s]Item Use Muun Item LifeGem Success target[%d] [%s] Dur:[%d]/[%d] Serial:%u",
			gObj[aIndex].AccountID, gObj[aIndex].Name, target, pItemAttribute->Name, nBeforeDur,
			(int)lpObj->pMuunInventory[target].m_Durability, gObj[aIndex].pInventory[source].m_Number);
	}
	return true;
}

void CMuunSystem::MuunItemDamage(OBJECTSTRUCT* lpObj, int damage) {
	if (lpObj->Type != 1 || damage <= 0) {
		return;
	}
	for (int i = 0; i < 2; i++) {
		float fdamage = (float)damage;
		if (!lpObj->pMuunInventory[i].IsItem() || lpObj->pMuunInventory[i].m_Durability <= 0.0f) {
			continue;
		}
		fdamage = fdamage + fdamage;
        fdamage = fdamage / 400.0;
        fdamage = fdamage / 400.0;
		lpObj->pMuunInventory[i].m_Durability = lpObj->pMuunInventory[i].m_Durability - fdamage;
		if (lpObj->pMuunInventory[i].m_Durability != 0.0f) {
          GCMuunItemDurSend(lpObj->m_Index, (BYTE)i, (int)lpObj->pMuunInventory[i].m_Durability);
		}
		if (lpObj->pMuunInventory[i].m_Durability < 1.0f) {
			lpObj->pMuunInventory[i].m_Durability = 0.0f;
			CalCharacterStat(lpObj->m_Index, lpObj->m_MuunEffectList[i].nOptType);
		}
		// message output
	}
}

int CMuunSystem::MuunItemEquipment(int aIndex, int iPos, int iSource) {
	OBJECTSTRUCT* lpObj = &gObj[aIndex];
	if (gObj[aIndex].Type != OBJ_USER) {
		return 0;
	}
	if (iPos >= 2) {
		if (iSource < 2 && iSource != -1 && !lpObj->pMuunInventory[iSource].IsItem()){
			if (iSource == 0) {
				lpObj->m_wMuunItem = -1;
			}
			RemoveUserMuunEffect(lpObj, iSource);
			GCSendConditionStatus(lpObj->m_Index, iSource, 0);
		}
	} else if (lpObj->pMuunInventory[iPos].IsItem()) {
		int nItemNum = lpObj->pMuunInventory[iPos].m_Type;
		lpObj->m_wMuunItem = ITEMGET(lpObj->pMuunInventory[iPos].m_Type, lpObj->pMuunInventory[iPos].m_Level);
		SetUserMuunEffect(lpObj, nItemNum, lpObj->pMuunInventory[iPos].m_Level, iPos);
        CheckEquipMuunItemCondition(lpObj);
        MsgMuunItemEffectValue(lpObj, iPos);
	}
	return 1;
}

bool CMuunSystem::SetUserMuunEffect(OBJECTSTRUCT* lpObj, int iMuunItmeNum, int iMuunLv, int iEquipPos) {
	CMuunInfo* pCMuunInfo = m_MuunInfoMng.GetMuunItemNumToMuunInfo(iMuunItmeNum);
	if (pCMuunInfo == NULL) {
		LogAddTD("[MuunSystem][Error] pCMuunInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return false;
	}
	if (iEquipPos < 1) {
		lpObj->m_MuunEffectList[0].nOptType = pCMuunInfo->GetOptType();
		lpObj->m_MuunEffectList[0].nOptValue = pCMuunInfo->GetMuunLvVal(iMuunLv) / 2;
		lpObj->m_MuunEffectList[0].nAddOptType = pCMuunInfo->GetAddOptType();
		lpObj->m_MuunEffectList[0].nAddOptValue = pCMuunInfo->GetAddOptVal();
		lpObj->m_MuunEffectList[0].nIndex = pCMuunInfo->GetIndex();
		lpObj->m_MuunEffectList[0].nMuunItemNum = iMuunItmeNum;
		lpObj->m_MuunEffectList[0].pCMuunInfo = pCMuunInfo;
		SetAddOptTypeValue(&lpObj->m_MuunEffectList[0]);
	} else if (iEquipPos == 1) {
		lpObj->m_MuunEffectList[1].nOptType = pCMuunInfo->GetOptType();
		lpObj->m_MuunEffectList[1].nOptValue = pCMuunInfo->GetMuunLvVal(iMuunLv) / 2;
		lpObj->m_MuunEffectList[1].nAddOptType = pCMuunInfo->GetAddOptType();
		lpObj->m_MuunEffectList[1].nAddOptValue = pCMuunInfo->GetAddOptVal();
		lpObj->m_MuunEffectList[1].nIndex = pCMuunInfo->GetIndex();
		lpObj->m_MuunEffectList[1].nMuunItemNum = iMuunItmeNum;
		lpObj->m_MuunEffectList[1].pCMuunInfo = pCMuunInfo;
		SetAddOptTypeValue(&lpObj->m_MuunEffectList[1]);
	}
	return true;
}

void CMuunSystem::SetAddOptTypeValue(_tagMUUN_EFFECT_LIST* pUserMuunEffect) {
	if (pUserMuunEffect->nAddOptType == 1) {
		pUserMuunEffect->nAddOptValue *= pUserMuunEffect->nOptValue;
	} else if (pUserMuunEffect->nAddOptType == 2) {
		pUserMuunEffect->nAddOptValue += pUserMuunEffect->nOptValue;
	}
}

bool CMuunSystem::RemoveUserMuunEffect(OBJECTSTRUCT* lpObj, int iEquipPos) {
	if (iEquipPos < 2) {
		CMuunInfo* pCMuunInfo = lpObj->m_MuunEffectList[iEquipPos].pCMuunInfo;
		if (pCMuunInfo == NULL) {
			LogAddTD("[MuunSystem][Error] RemoveUserMuunEffect() pCMuunInfo is NULL [%s][%s] [%s, %d]",
				lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
			return false;
		}
		lpObj->m_MuunEffectList[iEquipPos].Clear();
		CalCharacterStat(lpObj->m_Index, pCMuunInfo->GetOptType());
	}
	return true;
}

bool CMuunSystem::GetMuunItemValueOfOptType(OBJECTSTRUCT* lpObj, int iMuunOtpIndex, int* EffectValue1, int* EffectValue2) { 
	if (lpObj->Type != OBJ_USER) { // strange
		return false;
	}
	bool bRet = false;
    EffectValue1 = 0;
	for (int i = 0; i < 2; i++) {
		if (lpObj->pMuunInventory[i].IsItem() && iMuunOtpIndex == lpObj->m_MuunEffectList[i].nOptType
			&& lpObj->m_MuunEffectList[i].bOptEnable && (int)lpObj->pMuunInventory[i].m_Durability > 0) {
				if (lpObj->pMuunInventory[i].IsMuunItmePeriodExpire()) {
					EffectValue1 += lpObj->m_MuunEffectList[i].nOptValue;
				} else {
					EffectValue1 += lpObj->m_MuunEffectList[i].nAddOptValue;
				}
				bRet = true;
		}
	}
	return bRet;
}

void CMuunSystem::CalCharacterStat(int aIndex, int iOptType) {
	if (iOptType == 7 || iOptType == 1) {
		gObjCalCharacter(aIndex);
	}
}

void CMuunSystem::CalCharacterStat(int aIndex, CMuunInfo* pCMuunInfo) {
	if (pCMuunInfo == NULL) {
		LogAddTD("[MuunSystem][Error] CalCharacterStat() pCMuunInfo is NULL [%s, %d]",
			__FILE__, __LINE__);
		return;
	}
	CalCharacterStat(aIndex, pCMuunInfo->GetOptType());
}

int CMuunSystem::AddMuunItemPeriodInfo(OBJECTSTRUCT* lpObj) {
	for (int i = 0; i < 1000; i++) {
		if (!m_MuunItemPeriodData[i].bIsUsed) {
			m_MuunItemPeriodData[i].Clear();
			m_MuunItemPeriodData[i].bIsUsed = true;
			m_MuunItemPeriodData[i].lpUserObj = lpObj;
			m_MuunItemPeriodData[i].dwUserGUID = lpObj->DBNumber;
			m_MuunItemPeriodData[i].wUserIndex = lpObj->m_Index;
			m_MuunItemPeriodData[i].btUsedDataCount = 0;
			memcpy(m_MuunItemPeriodData[i].chAccountID, lpObj->Name, sizeof(m_MuunItemPeriodData[i].chAccountID));
			memcpy(m_MuunItemPeriodData[i].chCharacterName, lpObj->AccountID, sizeof(m_MuunItemPeriodData[i].chCharacterName));
			lpObj->m_iMuunItmePeriodDataIndex = i;
			return i;
		}
	}
	return (int)-1;
}

bool CMuunSystem::RemoveMuunItemPeriodInfo(OBJECTSTRUCT* lpObj) {
	int iPeriodItemSlotIndex = lpObj->m_iMuunItmePeriodDataIndex;
	if (!IsCorrectUser(lpObj)) {
		return false;
	}	
	if (iPeriodItemSlotIndex < 0 || iPeriodItemSlotIndex > 1000) {
		return false;
	}
	if (!m_MuunItemPeriodData[iPeriodItemSlotIndex].bIsUsed) {
		return false;
	}
	m_MuunItemPeriodData[iPeriodItemSlotIndex].Clear();
	return true;
}

bool CMuunSystem::ClearPeriodMuunItemData(OBJECTSTRUCT* lpObj, WORD wItemCode, DWORD dwSerial) {
	if (lpObj == NULL) {
		return false;
	}
	if (lpObj->Type != OBJ_USER || lpObj->Connected < PLAYER_LOGGED || !IsMuunItem(wItemCode)) {
		return false;
	}
	int iItemSlotIndex = lpObj->m_iMuunItmePeriodDataIndex;
	if (!m_MuunItemPeriodData[iItemSlotIndex].bIsUsed) {
		return false;
	}
	for (int i = 0; i < MUUN_INVENTORY_SIZE; i++) {
		if ( m_MuunItemPeriodData[iItemSlotIndex].ItemData[i].bIsUsed
			&& m_MuunItemPeriodData[iItemSlotIndex].ItemData[i].wItemCode == wItemCode
			&& m_MuunItemPeriodData[iItemSlotIndex].ItemData[i].dwSerial == dwSerial) {
				m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].Clear();
				return true;
		}
	}
	return false;
}

bool CMuunSystem::IsCorrectUser(OBJECTSTRUCT* lpObj) {
	int iPeriodItemSlotIndex = lpObj->m_iMuunItmePeriodDataIndex;
	if (iPeriodItemSlotIndex < 0) {
		return false;
	}
	if (!m_MuunItemPeriodData[iPeriodItemSlotIndex].bIsUsed
		|| m_MuunItemPeriodData[iPeriodItemSlotIndex].dwUserGUID != lpObj->DBNumber) {
		return false;
	}
	return true;
}

int CMuunSystem::SetMuunItemAddPeriodData(OBJECTSTRUCT* lpObj, int iMuunItemNum, DWORD dwSerial) {
	CMuunInfo* pCMuunInfo = m_MuunInfoMng.GetMuunItemNumToMuunInfo(iMuunItemNum);
	if (pCMuunInfo == NULL) {
		LogAddTD( "[MuunSystem][Error] pCMuunInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return 0;
	}
	pCMuunInfo->GetIndex();
    pCMuunInfo->GetAddOptStart();
    pCMuunInfo->GetAddOptEnd();
    pCMuunInfo->GetConditionType();
    pCMuunInfo->GetConditionVal1();
    pCMuunInfo->GetConditionVal2();
    AddMuunItmePeriodData(lpObj, iMuunItemNum, dwSerial, 0, pCMuunInfo);
	return 1;
}

int CMuunSystem::AddMuunItmePeriodData(OBJECTSTRUCT* lpObj, WORD wItemCode, DWORD dwSerial, int iDuration) {
	int iMuunItmePeriodDataIndex = lpObj->m_iMuunItmePeriodDataIndex;
	if (!IsCorrectUser(lpObj)) {
		return (int)-1;
	}
	for (int i = 0; i < MUUN_INVENTORY_SIZE; i++) {
		if (!m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].bIsUsed) {
			GetExpireDate(iDuration);
			m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].bIsUsed = true;
			m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].wItemCode = wItemCode;
			m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].dwSerial = dwSerial;
			m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].pCMuunInfo = m_MuunInfoMng.GetMuunItemNumToMuunInfo(wItemCode);
			return i;
		}
    }
	return (int)-1;
}

int CMuunSystem::AddMuunItmePeriodData(OBJECTSTRUCT* lpObj, WORD wItemCode, DWORD dwSerial, int iDuration, CMuunInfo* pCMuunInfo) {
	int iMuunItmePeriodDataIndex = lpObj->m_iMuunItmePeriodDataIndex;
	if (!IsCorrectUser(lpObj)) {
		return (int)-1;
	}
	for (int i = 0; i < MUUN_INVENTORY_SIZE; i++) {
		if (!m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].bIsUsed) {
			GetExpireDate(iDuration);
			m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].bIsUsed = true;
			m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].wItemCode = wItemCode;
			m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].dwSerial = dwSerial;
			m_MuunItemPeriodData[iMuunItmePeriodDataIndex].ItemData[i].pCMuunInfo = pCMuunInfo;
			return i;
		}
    }
	return (int)-1;
}

bool CMuunSystem::RemovePeriodMunnItemData(OBJECTSTRUCT* lpObj, WORD wItemCode, DWORD dwSerial) {
	if (lpObj == NULL) {
		return false;
	}
	if (lpObj->Type != OBJ_USER || lpObj->Connected < PLAYER_LOGGED) {
		return false;
	}
	int iInventoryPosition = GetItemFromMuunInventory(lpObj, wItemCode, dwSerial);
	if (iInventoryPosition == -1) {
		return false;
	}
	SetDisableMuunItemToExpire(lpObj, iInventoryPosition);
	return true;
}

bool CMuunSystem::SetDisableMuunItemToExpire(OBJECTSTRUCT* lpObj, int iInventoryPosition) {
	if (lpObj == NULL) {
		return false;
	}
	if (lpObj->Type != OBJ_USER || lpObj->Connected < PLAYER_LOGGED) {
		return false;
	}
	if (iInventoryPosition == -1) {
		return false;
	}
	lpObj->pMuunInventory[iInventoryPosition].SetMuunItmePeriodExpire();
	if (iInventoryPosition < 2) {
		if (lpObj->pMuunInventory[iInventoryPosition].IsItem()) {
			CalCharacterStat(lpObj->m_Index, lpObj->m_MuunEffectList[iInventoryPosition].nOptType);
        }
	}
	GCMuunInventoryItemListSend(lpObj->m_Index);
	return true;
}

int CMuunSystem::GetItemFromMuunInventory(OBJECTSTRUCT* lpObj, WORD wItemCode, DWORD dwSerial) {
	int iReturnIndex = -1;
	if (lpObj == NULL) {
		return false;
	}
	if (lpObj->Type != OBJ_USER || lpObj->Connected < PLAYER_LOGGED) {
		return false;
	}
	for (int i = 0; i < MUUN_INVENTORY_SIZE; i++) {
		if (lpObj->pMuunInventory[i].IsItem() 
			&& lpObj->pMuunInventory[i].m_Type == wItemCode
			&& lpObj->pMuunInventory[i].GetNumber() == dwSerial) {
			iReturnIndex = i;
			break;
		}
	}
	return iReturnIndex;
}

void CMuunSystem::CheckMuunItemPeriodData(OBJECTSTRUCT* lpObj) {
	if (lpObj == NULL) {
		 LogAddTD("[MuunSystem][Error] lpObj is NULL [%s, %d]", __FILE__, __LINE__);
		return;
	}
	if (GetTickCount() - lpObj->dwCheckMuunItemTime < 60000) {
		return;
	}
	lpObj->dwCheckMuunItemTime = GetTickCount();
	int iItemSlotIndex = lpObj->m_iMuunItmePeriodDataIndex;
	if (!m_MuunItemPeriodData[iItemSlotIndex].bIsUsed) {
		return;
	}
	OBJECTSTRUCT* lpObja = m_MuunItemPeriodData[iItemSlotIndex].lpUserObj;
	if (lpObja->Type != OBJ_USER || lpObja->Connected < PLAYER_LOGGED) {
		return;
	}
	CheckEquipMuunItemConditionProc(lpObja);
	for (int i = 0; i < MUUN_INVENTORY_SIZE; i++) {
		if (!m_MuunItemPeriodData[iItemSlotIndex].ItemData[i].bIsUsed) {
			continue;
		}
		CMuunInfo* pCMuunInfo = m_MuunItemPeriodData[iItemSlotIndex].ItemData[i].pCMuunInfo;
		if (pCMuunInfo == NULL) {
			LogAddTD("[MuunSystem][Error] pCMuunInfo is NULL [%s][%s] [%s, %d]",
				lpObja->AccountID, lpObja->Name, __FILE__, __LINE__);
			return;
		}
		if (CheckAddOptionExpireDate(pCMuunInfo->GetAddOptStart(), pCMuunInfo->GetAddOptEnd())) {
			RemovePeriodMunnItemData(lpObja, m_MuunItemPeriodData[iItemSlotIndex].ItemData[i].wItemCode,
				m_MuunItemPeriodData[iItemSlotIndex].ItemData[i].dwSerial);
			m_MuunItemPeriodData[iItemSlotIndex].ItemData[i].Clear();
		}
	}
}

void CMuunSystem::CheckEquipMuunItemConditionProc(OBJECTSTRUCT* lpObj) {
	for (int i = 0; i < 2; i++) {
		if (!lpObj->pMuunInventory[i].IsItem()) {
			continue;
		}
		int iRet = CheckMuunItemConditionProc(&lpObj->m_MuunEffectList[i], lpObj->m_MuunEffectList[i].pCMuunInfo);
		if (iRet > (int)-1)
		{
			GCSendConditionStatus(lpObj->m_Index, i, iRet);
			CalCharacterStat(lpObj->m_Index, lpObj->m_MuunEffectList[i].pCMuunInfo);
		}
	}
}

int CMuunSystem::CheckMuunItemConditionProc(_tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo) {
	if (pCMuunInfo == NULL) {
		 LogAddTD("[MuunSystem][Error] CheckMuunItemConditionProc() pCMuunInfo is NULL [%s, %d]",
			 __FILE__, __LINE__);
		return -1;
	}
	int iRet = -1;
	if (pCMuunInfo->GetConditionType() == 3) {
		iRet = ChkMuunOptConditionTime(pUserMuunEffect, pCMuunInfo);
	} else if (pCMuunInfo->GetConditionType() == 4) {
		iRet = ChkMuunOptConditionDay(pUserMuunEffect, pCMuunInfo);
	}
	return iRet;
}

int CMuunSystem::CheckMuunItemCondition(OBJECTSTRUCT& obj, _tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo) {
	if (pCMuunInfo == NULL) {
		LogAddTD("[MuunSystem][Error] CheckMuunItemCondition() pCMuunInfo is NULL [%s][%s] [%s, %d]",
			obj.AccountID, obj.Name, __FILE__, __LINE__);
		return -1;
	}
	int iRet = -1;
	switch (pCMuunInfo->GetConditionType()){
		case 1: iRet = ChkMuunOptConditionMap(obj, pUserMuunEffect, pCMuunInfo); break;
		case 2: iRet = ChkMuunOptConditionLevel(obj, pUserMuunEffect, pCMuunInfo); break;
		case 3: iRet = ChkMuunOptConditionTime(pUserMuunEffect, pCMuunInfo); break;
		case 4: iRet = ChkMuunOptConditionDay(pUserMuunEffect, pCMuunInfo); break;
	}
	return iRet;
}

int CMuunSystem::ChkMuunOptConditionTime(_tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo) {
	if (pCMuunInfo == NULL) {
		LogAddTD("[MuunSystem][Error] ChkMuunOptConditionTime() pCMuunInfo is NULL [%s, %d]",
			__FILE__, __LINE__);
		return -1;
	}
	SYSTEMTIME tmToDay;
	int nStartTime = pCMuunInfo->GetConditionVal1();
	int nEndTime = pCMuunInfo->GetConditionVal2();
	GetLocalTime(&tmToDay);
	if (tmToDay.wHour < nStartTime || tmToDay.wHour >= nEndTime) {
		if (pUserMuunEffect->bOptEnable) {
			pUserMuunEffect->bOptEnable = false;
			return 0;
		}
	} else if (!pUserMuunEffect->bOptEnable) {
		pUserMuunEffect->bOptEnable = true;
		return 1;
	}
	return -1;
}

int CMuunSystem::ChkMuunOptConditionDay(_tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo) {
	if (pCMuunInfo == NULL) {
		LogAddTD("[MuunSystem][Error] ChkMuunOptConditionDay() pCMuunInfo is NULL [%s, %d]",
			__FILE__, __LINE__);
		return -1;
	}
	BYTE DayOfWeek[] = { 64, 32, 16, 8, 4, 2, 1 };
	SYSTEMTIME tmToDay;
	GetLocalTime(&tmToDay);
	if ((pCMuunInfo->GetConditionVal1() & DayOfWeek[tmToDay.wDayOfWeek]) == DayOfWeek[tmToDay.wDayOfWeek]) {
		if (!pUserMuunEffect->bOptEnable) {
			pUserMuunEffect->bOptEnable = true;
			return 1;
		}
	} else if (pUserMuunEffect->bOptEnable) {
		pUserMuunEffect->bOptEnable = false;
		return 0;
	}
	return -1;
}

int CMuunSystem::ChkMuunOptConditionLevel(OBJECTSTRUCT& obj, _tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo) {
	if (pCMuunInfo == NULL) {
		LogAddTD("[MuunSystem][Error] ChkMuunOptConditionLevel() pCMuunInfo is NULL [%s, %d]",
			__FILE__, __LINE__);
		return -1;
	}
	int nMaxLv = pCMuunInfo->GetConditionVal1();
	int nMinLv = pCMuunInfo->GetConditionVal2();
	if (nMinLv > obj.Level || nMaxLv < obj.Level) {
		if (pUserMuunEffect->bOptEnable) {
			pUserMuunEffect->bOptEnable = false;
			return 0;
		}
	} else if (!pUserMuunEffect->bOptEnable) {
		pUserMuunEffect->bOptEnable = true;
		return 1;
	}
	return -1;
}

int CMuunSystem::ChkMuunOptConditionMap(OBJECTSTRUCT& obj, _tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo) {
	if (pCMuunInfo == NULL) {
		LogAddTD("[MuunSystem][Error] ChkMuunOptConditionMap() pCMuunInfo is NULL [%s, %d]",
			__FILE__, __LINE__);
		return -1;
	}
	if (pCMuunInfo->GetConditionVal1() == obj.MapNumber) {
		pUserMuunEffect->bOptEnable = true;
		return 1;
	}
    pUserMuunEffect->bOptEnable = false;
    return 0;
}

void CMuunSystem::CheckEquipMuunItemCondition(OBJECTSTRUCT* lpObj) {
	for (int i = 0; i < 2; i++) {
		if (!lpObj->pMuunInventory[i].IsItem()) {
			continue;
		}
		int iRet = CheckMuunItemCondition(*lpObj, &lpObj->m_MuunEffectList[i], lpObj->m_MuunEffectList[i].pCMuunInfo);
		if (iRet > (int)-1) {
			CalCharacterStat(lpObj->m_Index, lpObj->m_MuunEffectList[i].pCMuunInfo);
		}
		if (iRet < 0) {
			iRet = 0;
		}
		GCSendConditionStatus(lpObj->m_Index, i, iRet);
	}
}

void CMuunSystem::CheckMuunItemConditionLevelUp(OBJECTSTRUCT* lpObj) {
	int iRet = -1;
	for (int i = 0; i < 2; i++) {
		if (!lpObj->pMuunInventory[i].IsItem()) {
			continue;
		}
		CMuunInfo* pCMuunInfo = lpObj->m_MuunEffectList[i].pCMuunInfo;
		if (pCMuunInfo == NULL) {
			return;
		}
		int nMaxLv = pCMuunInfo->GetConditionVal1();
		int nMinLv = pCMuunInfo->GetConditionVal2();
		if (nMinLv > lpObj->Level || nMaxLv < lpObj->Level) {
			if (lpObj->m_MuunEffectList[i].bOptEnable) {
				lpObj->m_MuunEffectList[i].bOptEnable = false;
				iRet = 0;
			}
		} else if (!lpObj->m_MuunEffectList[i].bOptEnable) {
			lpObj->m_MuunEffectList[i].bOptEnable = true;
			iRet = 1;
		}
		GCSendConditionStatus(lpObj->m_Index, i, iRet);
		if (iRet > -1) {
			GCSendConditionStatus(lpObj->m_Index, i, iRet);
			CalCharacterStat(lpObj->m_Index, lpObj->m_MuunEffectList[i].pCMuunInfo);
		}
	}
}

void CMuunSystem::CheckMuunItemMoveMapConditionMap(OBJECTSTRUCT* lpObj, int iMapNumber) {
	for (int i = 0; i < 2; i++) {
		if (!lpObj->pMuunInventory[i].IsItem()) {
			continue;
		}
		CMuunInfo* pCMuunInfo = lpObj->m_MuunEffectList[i].pCMuunInfo;
		if (pCMuunInfo == NULL) {
			return;
		}
		if (pCMuunInfo->GetConditionType() != 1) {
			continue;
		}
		if (pCMuunInfo->GetConditionVal1() == iMapNumber) {
			if (!lpObj->m_MuunEffectList[i].bOptEnable) {
				lpObj->m_MuunEffectList[i].bOptEnable = true;
				GCSendConditionStatus(lpObj->m_Index, i, 1);
				CalCharacterStat(lpObj->m_Index,lpObj->m_MuunEffectList[i].pCMuunInfo);
			}
		} else if (lpObj->m_MuunEffectList[i].bOptEnable) {
			lpObj->m_MuunEffectList[i].bOptEnable = false;
			GCSendConditionStatus(lpObj->m_Index, i, 0);
			CalCharacterStat(lpObj->m_Index,lpObj->m_MuunEffectList[i].pCMuunInfo);
		}
	}
}
long CMuunSystem::GetCurrentDate() {
	CTime tCurrentTime = CTime::GetTickCount();
	return tCurrentTime.GetTime();
}

long CMuunSystem::GetExpireDate(int iDuration) {
	CTime tExpireTime;
	CTime tCurrentTime = CTime::GetTickCount();
	CTimeSpan tLeftTime(0, 0, 0, iDuration);
	tExpireTime = tCurrentTime + tLeftTime;
	return tExpireTime.GetTime();
}

bool CMuunSystem::CheckExpireDate(DWORD dwItemExpireDate) {
	return GetCurrentDate() > dwItemExpireDate;
}

DWORD CMuunSystem::GetLeftDate(long lExpireDate) {
	DWORD dwReturnValue = 0;
	CTime tCurrentTime = CTime::GetTickCount();
	CTime tExpireTime;
	CTimeSpan tSpanTime = tExpireTime - tCurrentTime;
	return tSpanTime.GetTotalMinutes();
}

bool CMuunSystem::CheckAddOptionExpireDate(DWORD dwStartDate, DWORD dwEndDate) {
	int dwCurrentDate = dwCurrentDate = GetCurrentDate();
	if (dwCurrentDate >= dwStartDate) {
		if (dwCurrentDate >= dwStartDate && dwCurrentDate <= dwEndDate) {
			return true;
		}
	}
	return false;
}

void CMuunSystem::MsgIsMuunItemActive(OBJECTSTRUCT* lpObj, int iPos) {
	// debug output
	return; 
}

void CMuunSystem::MsgMuunItemEffectValue(OBJECTSTRUCT* lpObj, int iPos) {
	// debug output
	return;
}

#endif