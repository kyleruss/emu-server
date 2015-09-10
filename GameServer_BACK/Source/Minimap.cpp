#include "stdafx.h"
#include "Minimap.h"
#include "MapClass.h"
#include "GameMain.h"
#include "SummonSpot.h"
#include "..\common\winutil.h"
#include "..\pugixml\pugixml.hpp"

using namespace pugi;
Minimap g_Minimap;

Minimap::Minimap()
{
	
}

Minimap::~Minimap()
{

}

void Minimap::Init()
{
	this->m_vtSendNpcInfo.clear();
	if( this->m_vtSendNpcInfo.capacity() > 0 )
	{
		std::vector<_stSendNPCInfo>().swap(this->m_vtSendNpcInfo);
	}
	this->m_vtSendPortalInfo.clear();
	if( this->m_vtSendPortalInfo.capacity() > 0 )
	{
		std::vector<_stSendPortalInfo>().swap(this->m_vtSendPortalInfo);
	}
}

void Minimap::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_WORLD_MINIMAP));
}

void Minimap::Read(LPSTR File)
{
	xml_document doc;
	xml_parse_result result = doc.load_file(File);

	if( result.status != status_ok )
	{
		LogAdd("%s - Minimap info load failed!", File);
		return;
	}

	xml_node minimap = doc.child("minimap");
	int TempMapNumber = 0;
	int NpcIndex = 0;
	int GateIndex = 0;

	for( xml_node map = minimap.child("map"); map; map = map.next_sibling())
	{
		int MapNumber = map.attribute("id").as_int(-1);

		if( !MAX_MAP_RANGE(MapNumber) )
		{
			MsgBox("[CMiniMap][LoadMinimapInfo] -> Invalid map number id - %d",MapNumber);
			return;
		}

		if (TempMapNumber != MapNumber)
		{
			TempMapNumber = MapNumber;
			NpcIndex = 0;
			GateIndex = 0;
		}

		xml_node npclist = map.child("npclist");
		
		for( xml_node npc = npclist.child("npc"); npc; npc = npc.next_sibling() )
		{
			_stSendNPCInfo lpNpc;
			lpNpc.btMapNumber = MapNumber;
			lpNpc.btPosX = npc.attribute("x").as_int(0);
			lpNpc.btPosY = npc.attribute("y").as_int(0);
			lpNpc.nNpcClass = npc.attribute("class").as_int(0);
			lpNpc.btSync = npc.attribute("sync").as_int(1);
			lpNpc.btTag = npc.attribute("tag").as_int(0);
			lpNpc.nIdentNo = NpcIndex;
			strncpy(lpNpc.szName, npc.text().as_string(), sizeof(lpNpc.szName) - 1);
			NpcIndex++;
			this->m_vtSendNpcInfo.push_back(lpNpc);
		}

		xml_node gatelist = map.child("gatelist");

		for( xml_node gate = gatelist.child("gate"); gate; gate = gate.next_sibling() )
		{
			_stSendPortalInfo lpGate;
			lpGate.btMapNumber = MapNumber;
			lpGate.btPosX = gate.attribute("x").as_int(0);
			lpGate.btPosY = gate.attribute("y").as_int(0);
			lpGate.nIdentNo = GateIndex;
			strncpy(lpGate.szName, gate.text().as_string(), sizeof(lpGate.szName) - 1);
			GateIndex++;
			this->m_vtSendPortalInfo.push_back(lpGate);
		}

//		LogAddTD("[MINIMAP] Map: %d loaded %d NPC / %d GATE objects!",MapNumber,
//			m_NpcObjects.count(MapNumber), m_GateObjects.count(MapNumber) );
	}

//	m_bLoaded = true;
//	LogAdd("[MINIMAP] File loaded successfully!");
}	

void Minimap::CGReqSendMemberPosInfoStart(int UserIndex)
{
	if (!OBJMAX_RANGE(UserIndex) || !gObjIsConnected(UserIndex))
	{
		return;
	}

	if (gObj[UserIndex].PartyNumber < 0)
	{
		return;
	}

	gObj[UserIndex].m_bSendMemberPos = true;
}

void Minimap::CGReqSendMemberPosInfoStop(int UserIndex)
{
	if (!OBJMAX_RANGE(UserIndex) || !gObjIsConnected(UserIndex))
	{
		return;
	}

	if (gObj[UserIndex].PartyNumber < 0)
	{
		return;
	}

	gObj[UserIndex].m_bSendMemberPos = false;
}

void Minimap::GCReqSendNpcPosInfo(_PMSG_REQ_NPC_POSITION* lpRecv, int UserIndex)
{
	if (!OBJMAX_RANGE(UserIndex) || !gObjIsConnected(UserIndex))
	{
		return;
	}

	if (gObj[UserIndex].PartyNumber < 0)
	{
		return;
	}

	if (lpRecv->btType)
	{
		gObj[UserIndex].m_bSendNpcPos = 0;
	}
    else
	{
		gObj[UserIndex].m_bSendNpcPos = 1;
	}
}

void Minimap::GCPartyMemberPosition(int nIndex)
{
	if (!OBJMAX_RANGE(nIndex) || !gObjIsConnected(nIndex))
	{
		return;
	}

	if (gObj[nIndex].PartyNumber < 0)
	{
		return;
	}

	if (!gObj[nIndex].m_bSendMemberPos)
	{
		return;
	}

	PMSG_MEMBER_POS_INFO_SEND pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE7, 1, sizeof(PMSG_MEMBER_POS_INFO_SEND));

	int nPartyNumber = gObj[nIndex].PartyNumber;

	if (!OBJMAX_RANGE(nPartyNumber))
	{
		return;
	}
	
	int nMemberCount = gParty.m_PartyS[nPartyNumber].Count;
    pMsg.btCount = 0;

	for (int i = 0; i < 5; i++)
    {
		int nTargetIndex = gParty.m_PartyS[nPartyNumber].Number[i];

		if (!OBJMAX_RANGE(nTargetIndex))
		{
			continue;
		}

		memcpy(pMsg.stPosInfo[pMsg.btCount].szName, gObj[nTargetIndex].Name, 10);
        pMsg.stPosInfo[pMsg.btCount].btMapNumber = gObj[nTargetIndex].MapNumber;
        pMsg.stPosInfo[pMsg.btCount].btPosX = gObj[nTargetIndex].X;
        pMsg.stPosInfo[pMsg.btCount].btPosY = gObj[nTargetIndex].Y;
        pMsg.btCount++;
	}
	
	DataSend(nIndex, (LPBYTE)&pMsg, pMsg.h.size);
}

void Minimap::SendNpcCoordinate(int nDestUserIndex, BYTE btMapIndex, BYTE btAddType)
{
	if (!OBJMAX_RANGE(nDestUserIndex))
	{
		return;
	}

	_stSendNPCInfo sendNpcInfo;
	_PMSG_ANS_NPC_POSITION pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE7, 3, sizeof(_PMSG_ANS_NPC_POSITION));

	for (int i = 0; i < this->m_vtSendNpcInfo.size(); i++)
    {
		sendNpcInfo = m_vtSendNpcInfo[i];

		if (sendNpcInfo.btMapNumber == btMapIndex && sendNpcInfo.btSync == 1)
		{
			pMsg.btPosX = sendNpcInfo.btPosX;
			pMsg.btPosY = sendNpcInfo.btPosY;
			pMsg.btTag = sendNpcInfo.btTag;
			pMsg.btIdentNo = sendNpcInfo.nIdentNo;
			pMsg.btType = btAddType;
			pMsg.btIsNpc = 1;
			memcpy(pMsg.szName, sendNpcInfo.szName, sizeof(pMsg.szName) - 1);
			DataSend(nDestUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		}
		else if (sendNpcInfo.btMapNumber == btMapIndex && sendNpcInfo.btSync == 2)
        {
			for (int n = 0; n < OBJ_MAXMONSTER; n++)
			{
				if (gObj[n].Type != OBJ_USER
					&& gObj[n].MapNumber == sendNpcInfo.btMapNumber
					&& gObj[n].Class == sendNpcInfo.nNpcClass)
				{
					pMsg.btPosX = gObj[n].X;
					pMsg.btPosY = gObj[n].Y;
					pMsg.btTag = sendNpcInfo.btTag;
					pMsg.btType = btAddType;
					pMsg.btIdentNo = sendNpcInfo.nIdentNo;
					pMsg.btIsNpc = 1;
					memcpy(pMsg.szName, sendNpcInfo.szName, sizeof(pMsg.szName) - 1);
					DataSend(nDestUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
					break;
				}
			}
		}
	}
}

void Minimap::SendPortalCoordinateMoveNpc(BYTE btBeforeMapNumber, BYTE btAfterMapNumber, BYTE btBeforeX, BYTE btBeforeY, BYTE btX, BYTE btY, int nNpcClass)
{
	_stSendNPCInfo sendNpcInfo;
	_PMSG_ANS_NPC_POSITION pDeleteMsg;
	pDeleteMsg.h.set((LPBYTE)&pDeleteMsg, 0xE7, 3, sizeof(_PMSG_ANS_NPC_POSITION));
	_PMSG_ANS_NPC_POSITION pInsertMsg;
	pInsertMsg.h.set((LPBYTE)&pInsertMsg, 0xE7, 3, sizeof(_PMSG_ANS_NPC_POSITION));

	for (int i = 0; i < this->m_vtSendNpcInfo.size(); i++)
    {
		sendNpcInfo = this->m_vtSendNpcInfo[i];

		if (sendNpcInfo.btMapNumber == btBeforeMapNumber && sendNpcInfo.btSync == 2 
			&& sendNpcInfo.nNpcClass == nNpcClass)
		{
			pDeleteMsg.btPosX = btBeforeX;
			pDeleteMsg.btPosY = btBeforeY;
			pDeleteMsg.btTag = sendNpcInfo.btTag;
			pDeleteMsg.btIdentNo = sendNpcInfo.nIdentNo;
			pDeleteMsg.btType = 1;
			pDeleteMsg.btIsNpc = 1;
			memcpy(pDeleteMsg.szName, sendNpcInfo.szName, sizeof(pDeleteMsg.szName) - 1);
		}

		if (sendNpcInfo.btMapNumber == btAfterMapNumber && sendNpcInfo.btSync == 2
			&& sendNpcInfo.nNpcClass == nNpcClass)
		{

			pInsertMsg.btPosX = btX;
			pInsertMsg.btPosY = btY;
			pInsertMsg.btTag = sendNpcInfo.btTag;
			pInsertMsg.btType = 0;
			pInsertMsg.btIdentNo = LOBYTE(sendNpcInfo.nIdentNo);
			pInsertMsg.btIsNpc = 1;
			memcpy(pInsertMsg.szName, sendNpcInfo.szName, 0x1Eu);
		}
	}

	for (int n = OBJ_STARTUSERINDEX; n < OBJMAX; n++)
	{
		if (gObj[n].Connected != 3)
		{
			continue;
		}

		if (gObj[n].MapNumber == btBeforeMapNumber)
		{
			DataSend(n, (LPBYTE)&pDeleteMsg, pDeleteMsg.h.size);
		}
		else if (gObj[n].MapNumber == btAfterMapNumber)
		{
			DataSend(n, (LPBYTE)&pInsertMsg, pInsertMsg.h.size);
		}
    }
}

void Minimap::SendPortalCoordinate(int nDestUserIndex, BYTE btMapNumber)
{
	if (!OBJMAX_RANGE(nDestUserIndex))
	{
		return;
	}

	_stSendPortalInfo sendPortalInfo;
	_PMSG_ANS_NPC_POSITION pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE7, 3, sizeof(_PMSG_ANS_NPC_POSITION));

	for (int i = 0; i < this->m_vtSendPortalInfo.size(); i++)
    {
		sendPortalInfo = this->m_vtSendPortalInfo[i];

		if (sendPortalInfo.btMapNumber == btMapNumber)
		{
			pMsg.btPosX = sendPortalInfo.btPosX;
			pMsg.btPosY = sendPortalInfo.btPosY;
			pMsg.btTag = 1;
			pMsg.btType = 0;
			pMsg.btIdentNo = sendPortalInfo.nIdentNo;
			pMsg.btIsNpc = 0;
			memcpy(pMsg.szName, sendPortalInfo.szName, sizeof(pMsg.szName) - 1);
			DataSend(nDestUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		}
	}

}

/*CMiniMap g_MiniMap;

// Constructor / Destructor 

CMiniMap::CMiniMap()
{
	Init();
}

CMiniMap::~CMiniMap()
{

}

// MiniMap Class

void CMiniMap::Init()
{
	m_bLoaded = false;
}

void CMiniMap::LoadMinimapInfo(const char* lpszFileName)
{
	xml_document doc;
	xml_parse_result result = doc.load_file(lpszFileName);

	if( result.status != status_ok )
	{
		LogAdd("%s - Minimap info load failed!",lpszFileName);
		return;
	}

	m_NpcObjects.clear();
	m_GateObjects.clear();

	xml_node minimap = doc.child("minimap");

	for( xml_node map = minimap.child("map"); map; map = map.next_sibling())
	{
		int MapNumber = map.attribute("id").as_int(-1);

		if( !MAX_MAP_RANGE(MapNumber) )
		{
			MsgBox("[CMiniMap][LoadMinimapInfo] -> Invalid map number id - %d",MapNumber);
			return;
		}

		xml_node npclist = map.child("npclist");

		for( xml_node npc = npclist.child("npc"); npc; npc = npc.next_sibling() )
		{
			MINIMAP_OBJECT tmpObj;
			memset(&tmpObj,0,sizeof(MINIMAP_OBJECT));

			tmpObj.btType = npc.attribute("type").as_int();
			tmpObj.btX = npc.attribute("x").as_int();
			tmpObj.btY = npc.attribute("y").as_int();

			strncpy(tmpObj.szDescription, npc.text().as_string(), sizeof(tmpObj.szDescription)-1);

			m_NpcObjects.insert(std::pair<int,MINIMAP_OBJECT>(MapNumber,tmpObj));
		}

		xml_node gatelist = map.child("gatelist");

		for( xml_node gate = gatelist.child("gate"); gate; gate = gate.next_sibling() )
		{
			MINIMAP_OBJECT tmpObj;
			memset(&tmpObj,0,sizeof(MINIMAP_OBJECT));

			tmpObj.btType = gate.attribute("type").as_int();
			tmpObj.btX = gate.attribute("x").as_int();
			tmpObj.btY = gate.attribute("y").as_int();

			strncpy(tmpObj.szDescription, gate.text().as_string(), sizeof(tmpObj.szDescription)-1);

			m_GateObjects.insert(std::pair<int,MINIMAP_OBJECT>(MapNumber,tmpObj));
		}

		LogAddTD("[MINIMAP] Map: %d loaded %d NPC / %d GATE objects!",MapNumber,
			m_NpcObjects.count(MapNumber), m_GateObjects.count(MapNumber) );
	}

	m_bLoaded = true;
	LogAdd("[MINIMAP] File loaded successfully!");
}

void CMiniMap::SendMinimap(int aIndex)
{
	if( !OBJMAX_RANGE(aIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if( !gObjIsConnected(lpObj->m_Index) )
	{
		LogAddTD("[MINIMAP] [%d] SendMinimap info failed - user not connected",aIndex);
		return;
	}

	SetCloseMinimap(aIndex);
	
#if( __NOVUS__ == 1 )
	g_SummonSpot.SendSpotInfo(lpObj);
#endif

	if( m_NpcObjects.count(lpObj->MapNumber) == 0 &&
		m_GateObjects.count(lpObj->MapNumber) == 0 )
		return;

	std::pair<mapMinimap::iterator,mapMinimap::iterator> _Ret = m_NpcObjects.equal_range(lpObj->MapNumber);
	mapMinimap::iterator _It = _Ret.first;

	PMSG_MINIMAP_SEND_NPC pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xE7, 0x03, sizeof(PMSG_MINIMAP_SEND_NPC));

	int id = 0;

	for( ; _It != _Ret.second; _It++, id++ )
	{
		MINIMAP_OBJECT* tmpObj = &_It->second;

		pMsg.btNum = id;
		pMsg.btGroup = 1;
		pMsg.btAction = 0;
		pMsg.btType = tmpObj->btType;
		pMsg.btX = tmpObj->btX;
		pMsg.btY = tmpObj->btY;

		memcpy(pMsg.szName,tmpObj->szDescription,sizeof(pMsg.szName));
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
	}

	_Ret = m_GateObjects.equal_range(lpObj->MapNumber);
	_It = _Ret.first;

	for( ; _It != _Ret.second; _It++, id++ )
	{
		MINIMAP_OBJECT* tmpObj = &_It->second;

		pMsg.btNum = id;
		pMsg.btGroup = 0;
		pMsg.btAction = 0;
		pMsg.btType = tmpObj->btType;
		pMsg.btX = tmpObj->btX;
		pMsg.btY = tmpObj->btY;

		memcpy(pMsg.szName,tmpObj->szDescription,sizeof(pMsg.szName));
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
	}
}

void CMiniMap::SendParty(int aIndex)
{
	if( !OBJMAX_RANGE(aIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if( !gObjIsConnected(lpObj->m_Index) )
	{
		LogAddTD("[MINIMAP] [%d] SendParty info failed - user not connected",aIndex);
		return;
	}

	int party_num = lpObj->PartyNumber;

	if( party_num < 0 )
	{
		LogAddTD("[MINIMAP] [%d] (%s)(%s) SendParty info failed - not it party",aIndex,
			lpObj->AccountID,lpObj->Name);
		return;
	}

	PMSG_MINIMAP_SEND_PARTY pMsg;
	memset(&pMsg,0x00,sizeof(pMsg));
	PHeadSubSetB((LPBYTE)&pMsg,0xE7,0x01,sizeof(pMsg));

	pMsg.btCount = 0;

	for(int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		int number = gParty.m_PartyS[party_num].Number[i];

		if( !OBJMAX_RANGE(number) )
			continue;

		pMsg.User[pMsg.btCount].btMapNumber = gObj[number].MapNumber;
		pMsg.User[pMsg.btCount].btX = gObj[number].X;
		pMsg.User[pMsg.btCount].btY = gObj[number].Y;

		strncpy(pMsg.User[pMsg.btCount].szName,gObj[number].Name,sizeof(pMsg.User->szName));
		pMsg.btCount++;
	}

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void CMiniMap::SetOpenMinimap(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
	{
		return;
	}

	if( gObj[aIndex].Type != OBJ_USER )
	{
		return;
	}

	gObj[aIndex].m_bOpenMinimap = true;
}

void CMiniMap::SetCloseMinimap(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
	{
		return;
	}

	if( gObj[aIndex].Type != OBJ_USER )
	{
		return;
	}

	gObj[aIndex].m_bOpenMinimap = false;
}*/