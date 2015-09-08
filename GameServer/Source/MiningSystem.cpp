#include "stdafx.h"
#include "GameMain.h"
#include "MiningSystem.h"
#include "LargeRand.h"
#include "DSProtocol.h"
#include "Event.h"
#include "..\pugixml\pugixml.hpp"

using namespace pugi;

CMiningSystem g_MiningSystem;

// Constructor / Destructor

CMiningSystem::CMiningSystem()
{
	Init();
}

CMiningSystem::~CMiningSystem()
{

}

void CMiningSystem::Init()
{
	this->m_bEnabled = false;
	this->m_bLoaded = false;

	this->m_JewelRnd.InitPool();
	this->m_vtJewelList.clear();
}

void CMiningSystem::LoadData(char* script_file)
{
	this->Init();

	xml_document doc;
	xml_parse_result result = doc.load_file(script_file);

	if( result.status != status_ok )
	{
		LogAdd("%s - Item Load Failed!",script_file);
		return;
	}

	xml_node system = doc.child("miningsystem");
	xml_node jewellist = system.child("jewellist");

	int jewelcount = 0;

	for( xml_node jewel = jewellist.child("jewel"); jewel; jewel = jewel.next_sibling())
	{
		int tmprate;
		_MINING_JEWEL pTmp;

		pTmp.iJewelID = jewel.attribute("id").as_int();
		pTmp.iPackID = jewel.attribute("packid").as_int();
		tmprate = jewel.attribute("rate").as_int(1);

		this->m_JewelRnd.AddValue(m_vtJewelList.size(),tmprate);
		m_vtJewelList.push_back(pTmp);

		LogAddTD("[MINING SYSTEM] Added JewelID:[%d] PackID:[%d]",pTmp.iJewelID,pTmp.iPackID);
	}

	for( xml_node object = system.child("object"); object; object = object.next_sibling())
	{
		_MINING_OBJECT pTmp;

		int classid = object.attribute("class").as_int(-1);

		if( classid == -1 )
			continue;

		pTmp.iFinalRate = object.attribute("final_rate").as_int();
		pTmp.iFinalCount = object.attribute("final_jewels").as_int();
		pTmp.iDurDec = object.attribute("dur_dec").as_int();

		for( xml_node level = object.child("level"); level; level = level.next_sibling())
		{
			int id = level.attribute("id").as_int(-1);

			if( id < 0 || id >= MAX_MINING_LEVEL )
				continue;

			pTmp.Level[id].iRate = level.attribute("rate").as_int();
			pTmp.Level[id].iCount = level.attribute("jewels").as_int();

			LogAddTD("[MINING SYSTEM] OBJECT[%d] Level - %d, Rate - %d, Count - %d",
				classid,id+1,pTmp.Level[id].iRate,pTmp.Level[id].iCount);
		}

		this->m_mapObjectList.insert(std::pair<int,_MINING_OBJECT>(classid,pTmp));

		LogAddTD("[MINING SYSTEM] ADD OBJECT[%d] FinalRate - %d, FinalCount - %d, DurDec - %d",
			classid,pTmp.iFinalRate,pTmp.iFinalCount,pTmp.iDurDec);
	}

	LogAdd(lMsg.Get(MSGGET(1,198)),script_file);
	this->m_bLoaded = true;
}

void CMiningSystem::ResetMiningInfo(LPOBJ lpObj)
{
	lpObj->m_bOreUsed = 0;
	lpObj->m_iMiningLevel = 0;
	lpObj->m_iMiningJewelId = -1;
	lpObj->m_iMiningObjectId = -1;
}

_MINING_OBJECT* CMiningSystem::GetObject(int ObjClass)
{
	std::map<int,_MINING_OBJECT>::iterator _It = this->m_mapObjectList.find(ObjClass);

	if( _It == this->m_mapObjectList.end() )
		return NULL;

return &_It->second;
}

int CMiningSystem::CheckMining(LPOBJ lpObj,int ObjId,int ObjClass,int Level)
{

	if( lpObj->m_Change >= 0 )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Transformation used",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID);
		return 10;
	}

	int iEmptyCount = 0;
	int x;
	int invSize = INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE;

	if( lpObj->pInventoryExtend <= 4 )	{
		invSize = (INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE)-(32*(4-lpObj->pInventoryExtend));
	}

	for ( x=0;x<invSize;x++)
	{
		if ( lpObj->pInventoryMap[x] == 0xFF )
		{
			iEmptyCount++;
		}
	}

	if( iEmptyCount < 40 )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) No enough empty space",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID);

		return 11;
	}

	if( lpObj->m_IfState.use == TRUE && lpObj->m_IfState.type != 22)
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Try to mine while busy state",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID);
		return 12;
	}

	if( lpObj->m_bPShopOpen == TRUE )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Try to mine with open personal shop",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID);

		return 13;
	}

	if( Level != lpObj->m_iMiningLevel)
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Level mismatch - [%d/%d]",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,Level,lpObj->m_iMiningLevel);
		return 17;
	}

	if( Level != 0 && ObjId != lpObj->m_iMiningObjectId )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Object Id mismatch - [%d/%d]",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,ObjId,lpObj->m_iMiningObjectId);
		return 17;
	}

	_MINING_OBJECT* pObject = this->GetObject(ObjClass);

	if( pObject == NULL )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Object Class incorect - %d",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,ObjClass);
		return 17;
	}

	if( !lpObj->pInventory[0].IsItem() || lpObj->pInventory[0].m_Type != ITEMGET(0,41) )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Not using pandora pick axe",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID);
		return 18;
	}
	
	if( lpObj->pInventory[0].m_Durability < pObject->iDurDec )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Low durability - [%d/%d]",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,pObject->iDurDec,lpObj->pInventory[0].m_Durability);
		return 18;
	}

return true;
}

void CMiningSystem::MiningRequest(LPOBJ lpObj,int ObjId,int ObjClass,int Level)
{
	lpObj->m_IfState.use = TRUE;
	lpObj->m_IfState.type = 22;

	PMSG_MINING_ANSWER pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0x4C,0x00,sizeof(pMsg));

	if( !gObj[ObjId].Live )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) OreNPC dead status!",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID);

		pMsg.iResult = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( !MINING_LEVEL(Level) )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Level not in range - %d",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,Level);

		pMsg.iResult = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( gObj[ObjId].m_bOreUsed == TRUE && ObjId != lpObj->m_iMiningObjectId )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Ore used - %d",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,Level);

		pMsg.iResult = 3;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	_MINING_OBJECT* pObject = this->GetObject(ObjClass);

	if( pObject == NULL )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Object not found - %d",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,ObjClass);

		pMsg.iResult = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}	

	if( !lpObj->pInventory[0].IsItem() || lpObj->pInventory[0].m_Type != ITEMGET(0,41) )
	{
		LogAddC(2,"[MINING SYSTEM] (Fail) [%d] (%s)(%s) Pick axe not found!",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID);

		pMsg.iResult = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	lpObj->pInventory[0].m_Durability -= pObject->iDurDec;

	if( lpObj->pInventory[0].m_Durability < 1 )
		lpObj->pInventory[0].m_Durability = 0;

	GCItemDurSend2(lpObj->m_Index,0,lpObj->pInventory[0].m_Durability,0);

	if( Level == 0 )
	{
		lpObj->m_iMiningObjectId = ObjId;
		lpObj->m_iMiningJewelId = this->m_JewelRnd.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);

		if( this->m_vtJewelList.size() > 1 )
		{
			//std::vector<_MINING_JEWEL>::iterator _It = &this->m_vtJewelList.at(lpObj->m_iMiningJewelId);
			MsgOutput(lpObj->m_Index,"You have found %s ore.",
				ItemAttribute[this->m_vtJewelList.at(lpObj->m_iMiningJewelId).iJewelID].Name);
		}
	}

	int iRate = g_MersenneRand.IRandom(1000000);

	if( iRate >= pObject->Level[Level].iRate )
	{
		LogAddC(4,"[MINING SYSTEM] (Fail) [%d](%s)(%s) - Fail by rate - [%d/%d]",
			lpObj->m_Index,lpObj->AccountID,lpObj->Name,pObject->Level[Level].iRate,iRate);

		pMsg.iResult = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	gObj[ObjId].m_bOreUsed = TRUE;
	lpObj->m_iMiningLevel++;

	pMsg.aIndex = lpObj->m_Index;
	pMsg.tIndex = ObjId;
	pMsg.tClass = ObjClass;
	pMsg.MapNumber = lpObj->MapNumber;
	pMsg.Level = lpObj->m_iMiningLevel;
	pMsg.iCount = pObject->Level[Level].iCount;
	pMsg.iResult = 0;

	if( lpObj->m_iMiningLevel == MAX_MINING_LEVEL )
	{
		pMsg.iResult = 1;
	}

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);

	LogAddC(4,"[MINING SYSTEM] (Success) [%d](%s)(%s) - Level: %d, JewelType: %d",
		lpObj->m_Index,lpObj->AccountID,lpObj->Name,lpObj->m_iMiningLevel,lpObj->m_iMiningJewelId);
}

void CMiningSystem::RewardRequest(LPOBJ lpObj,int ObjId,int ObjClass,int Level)
{

	PMSG_MINING_REWARD_ANSWER pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0x4C,0x01,sizeof(pMsg));

	if( !MINING_LEVEL((lpObj->m_iMiningLevel-1)) )
	{
		LogAddC(2,"[MINING SYSTEM] [REWARD] (Fail) [%d] (%s)(%s) Level not in range - %d #1",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,Level);

		pMsg.iResult = 0;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	_MINING_OBJECT* pObject = this->GetObject(ObjClass);

	if( pObject == NULL )
	{
		LogAddC(2,"[MINING SYSTEM] [REWARD] (Fail) [%d] (%s)(%s) Object not found - %d",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,ObjClass);

		pMsg.iResult = 1;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( this->m_vtJewelList.size() <= lpObj->m_iMiningJewelId )
	{
		LogAddC(2,"[MINING SYSTEM] [REWARD] (Fail) [%d] (%s)(%s) JewelID error - %d",
			lpObj->m_Index,lpObj->Name,lpObj->AccountID,lpObj->m_iMiningJewelId);

		pMsg.iResult = 2;
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}
	_MINING_JEWEL* pJewel = &this->m_vtJewelList.at(lpObj->m_iMiningJewelId);

	int bGift = false;
	int bInventory = true;
	int count = pObject->Level[lpObj->m_iMiningLevel-1].iCount;
	int jCount = 0;
	int pCount_lv0 = 0;
	int pCount_lv1 = 0;
	int pCount_lv2 = 0;
	int i = 0;

	if( lpObj->m_iMiningLevel == MAX_MINING_LEVEL )
	{
		PMSG_SERVERCMD ServerCmd = { 0 };
		PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
		ServerCmd.CmdType = 0;
		ServerCmd.X = lpObj->X;
		ServerCmd.Y = lpObj->Y;
		MsgSendV2(lpObj, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
		DataSend(lpObj->m_Index, (LPBYTE)&ServerCmd, sizeof(ServerCmd));

		int iRate = g_MersenneRand.IRandom(1000000);
		if( iRate < pObject->iFinalRate )
		{
			bGift = true;
			count = pObject->iFinalCount;
		}
	}

	this->GetRewardCount(count,30,pCount_lv2,count);
	this->GetRewardCount(count,20,pCount_lv1,count);
	this->GetRewardCount(count,10,pCount_lv0,count);
	this->GetRewardCount(count,1,jCount,count);

	int iEmptyCount = 0;
	int x;
	int invSize = INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE;

	if( lpObj->pInventoryExtend <= 4 )	{
		invSize = (INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE)-(32*(4-lpObj->pInventoryExtend));
	}

	for ( x=0;x<invSize;x++)
	{
		if ( lpObj->pInventoryMap[x] == 0xFF )
		{
			iEmptyCount++;
		}
	}

	BYTE cX = 0;
	BYTE cY = 0;
	int MapNumber = 235;

	if( iEmptyCount < (jCount + pCount_lv0 + pCount_lv1 + pCount_lv2) )
	{
		bInventory = false;
		MapNumber = lpObj->MapNumber;
	}

	for(i = 0; i < pCount_lv2; i++)
	{	
		if( bInventory == false )
		{
			if( !gObjGetRandomItemDropLocation(MapNumber,cX,cY,2,2,10) )
			{
				cX = lpObj->X;
				cY = lpObj->Y;
			}
		}

		ItemSerialCreateSend(lpObj->m_Index,MapNumber,cX,cY,pJewel->iPackID,2,1,0,0,0,lpObj->m_Index,0,0,0,0);
	}

	for(i = 0; i < pCount_lv1; i++)
	{	
		if( bInventory == false )
		{
			if( !gObjGetRandomItemDropLocation(MapNumber,cX,cY,2,2,10) )
			{
				cX = lpObj->X;
				cY = lpObj->Y;
			}
		}

		ItemSerialCreateSend(lpObj->m_Index,MapNumber,cX,cY,pJewel->iPackID,1,1,0,0,0,lpObj->m_Index,0,0,0,0);
	}

	for(i = 0; i < pCount_lv0; i++)
	{	
		if( bInventory == false )
		{
			if( !gObjGetRandomItemDropLocation(MapNumber,cX,cY,2,2,10) )
			{
				cX = lpObj->X;
				cY = lpObj->Y;
			}
		}

		ItemSerialCreateSend(lpObj->m_Index,MapNumber,cX,cY,pJewel->iPackID,0,1,0,0,0,lpObj->m_Index,0,0,0,0);
	}

	for(i = 0; i < jCount; i++)
	{	
		if( bInventory == false )
		{
			if( !gObjGetRandomItemDropLocation(MapNumber,cX,cY,2,2,10) )
			{
				cX = lpObj->X;
				cY = lpObj->Y;
			}
		}

		ItemSerialCreateSend(lpObj->m_Index,MapNumber,cX,cY,pJewel->iJewelID,0,1,0,0,0,lpObj->m_Index,0,0,0,0);
	}

	pMsg.aIndex = lpObj->m_Index;
	pMsg.tIndex = ObjId;
	pMsg.tClass = ObjClass;
	pMsg.MapNumber = lpObj->MapNumber;
	pMsg.Level = Level;
	pMsg.iResult = 3;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);


	if( bGift == TRUE )
	{
		char szTmp[256];
		sprintf(szTmp,"[%s] have just recieved - %d of %s",
			lpObj->Name,pObject->iFinalCount,ItemAttribute[pJewel->iJewelID].Name);

		AllSendServerMsg(szTmp);
	}

	LogAddTD("[MINING SYSTEM] [REWARD] [%d](%s)(%s) Level: %d - JewelType: %s, Count - %d",
		lpObj->m_Index,lpObj->AccountID,lpObj->Name,lpObj->m_iMiningLevel,
		ItemAttribute[pJewel->iJewelID].Name,(bGift==TRUE) ? pObject->iFinalCount : pObject->Level[lpObj->m_iMiningLevel-1].iCount);

	gObj[ObjId].Life = 0.0f;
	gObj[ObjId].m_bOreUsed = FALSE;
	gObjLifeCheck(&gObj[ObjId],lpObj,0,1,0,0,0,0);
	lpObj->m_iMiningObjectId = -1;
}

void CMiningSystem::GetRewardCount(int count,int div,int& itemcount,int& left)
{
	itemcount = count/div;
	left = count%div;
}

void CGMiningRequest(PMSG_MINING_REQUEST* lpMsg,int aIndex)
{
	if( !OBJMAX_RANGE(aIndex) )
	{
		LogAdd("Index not in range: %s %d",__FILE__,__LINE__);
		return;
	}

	int aTargetIndex = lpMsg->tIndex;

	if( !OBJMAX_RANGE(aTargetIndex) )
	{
		LogAdd("TIndex not in range: %s %d",__FILE__,__LINE__);
		return;
	}

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("User not connected: %d (%s)(%s)",
			aIndex,gObj[aIndex].AccountID,gObj[aIndex].Name);
		return;
	}

	if( lpMsg->aIndex != aIndex )
	{
		LogAddTD("Request index incorrect: ObjectIndex:[%d] RequestedIndex:[%d]",aIndex,lpMsg->aIndex);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[lpMsg->tIndex];

	PMSG_MINING_ANSWER pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0x4C,0x00,sizeof(pMsg));

	int bResult = g_MiningSystem.CheckMining(lpObj,lpMsg->tIndex,lpMsg->tClass,lpMsg->Level);

	if( bResult != TRUE )
	{
		pMsg.iResult = bResult;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	g_MiningSystem.MiningRequest(lpObj,lpMsg->tIndex,lpMsg->tClass,lpMsg->Level);
}

void CGRewardRequest(PMSG_MINING_REWARD_REQUEST* lpMsg,int aIndex)
{
	if( !OBJMAX_RANGE(aIndex) )
	{
		LogAdd("Index not in range: %s %d",__FILE__,__LINE__);
		return;
	}

	int aTargetIndex = lpMsg->tIndex;

	if( !OBJMAX_RANGE(aTargetIndex) )
	{
		LogAdd("TIndex not in range: %s %d",__FILE__,__LINE__);
		return;
	}

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("User not connected: %d (%s)(%s)",
			aIndex,gObj[aIndex].AccountID,gObj[aIndex].Name);
		return;
	}

	if( lpMsg->aIndex != aIndex )
	{
		LogAddTD("Request index incorrect: ObjectIndex:[%d] RequestedIndex:[%d]",aIndex,lpMsg->aIndex);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use != TRUE || lpObj->m_IfState.type != 22 )
	{
		LogAddC(2,"[MINING SYSTEM] (End Request) [%d](%s)(%s) User not in mining state!",
			lpObj->m_Index,lpObj->AccountID,lpObj->Name);
		return;
	}

	if( lpObj->m_iMiningObjectId != lpMsg->tIndex )
	{
		LogAddC(2,"[MINING SYSTEM] [REWARD] [%d](%s)(%s) Mining Object index mismatch [%d/%d]!",
			lpObj->m_Index,lpObj->AccountID,lpObj->Name,lpObj->m_iMiningObjectId,lpMsg->tIndex);
		return;
	}

	g_MiningSystem.RewardRequest(lpObj,lpMsg->tIndex,lpMsg->tClass,lpMsg->Level);
	g_MiningSystem.ResetMiningInfo(lpObj);

	lpObj->m_IfState.use = FALSE;
	lpObj->m_IfState.type = 0;
}

void CGMiningEnd(PMSG_MINING_END* lpMsg,int aIndex)
{
	if( !OBJMAX_RANGE(aIndex) )
	{
		LogAdd("Index not in range: %s %d",__FILE__,__LINE__);
		return;
	}

	int aTargetIndex = lpMsg->tIndex;

	if( !OBJMAX_RANGE(aTargetIndex) )
	{
		LogAdd("TIndex not in range: %s %d",__FILE__,__LINE__);
		return;
	}

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("User not connected: %d (%s)(%s)",
			aIndex,gObj[aIndex].AccountID,gObj[aIndex].Name);
		return;
	}

	if( lpMsg->aIndex != aIndex )
	{
		LogAddTD("Request index incorrect: ObjectIndex:[%d] RequestedIndex:[%d]",aIndex,lpMsg->aIndex);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use != TRUE || lpObj->m_IfState.type != 22 )
	{
		LogAddC(2,"[MINING SYSTEM] (End Request) [%d](%s)(%s) User not in mining state!",
			lpObj->m_Index,lpObj->AccountID,lpObj->Name);
		return;
	}

	if( lpObj->m_iMiningObjectId != lpMsg->tIndex )
	{
		LogAddC(2,"[MINING SYSTEM] (End Request) [%d](%s)(%s) Mining Object index mismatch [%d/%d]!",
			lpObj->m_Index,lpObj->AccountID,lpObj->Name,lpObj->m_iMiningObjectId,lpMsg->tIndex);
		return;
	}

	LPOBJ lpTargetObj = &gObj[lpMsg->tIndex];
	lpTargetObj->m_bOreUsed = FALSE;

	lpObj->m_IfState.use = FALSE;
	lpObj->m_IfState.type = 0;

	lpTargetObj->Life = 0.0f;
	gObjLifeCheck(lpTargetObj,lpObj,0,1,0,0,0,0);
	lpObj->m_iMiningObjectId = -1;

	g_MiningSystem.ResetMiningInfo(lpObj);

	LogAddTD("[MiningInfo] [END_REQUEST] [%d](%s)(%s) - Type:[%d] (%s)",
		aIndex,lpObj->AccountID,lpObj->Name,lpTargetObj->Class,lpTargetObj->Name);
}