#include "stdafx.h"
#include "OfflineTrade.h"
#include "..\pugixml\pugixml.hpp"
#include "GameMain.h"

using namespace pugi;
OfflineTrade g_OfflineTrade;

OfflineTrade::OfflineTrade()
{

}

OfflineTrade::~OfflineTrade()
{

}


void OfflineTrade::Init()
{
	
}

void OfflineTrade::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CUSTOM_OFFLINETRADE));
}

void OfflineTrade::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if( Result.status != status_ok )
	{
		MsgBox("[OfflineTrade] File %s not found! %d", File, Result.status);
		return;
	}

	xml_node ChildCommon = Document.child("offlinetrade");
	xml_node Node;

	this->m_MaxRunTime = ChildCommon.child("maxruntime").text().as_uint(0);

	for( Node = ChildCommon.child("mapfilter").child("map"); Node; Node = Node.next_sibling() )
	{
		OfflineTradeMapZone lpItem;
		lpItem.MapNumber = (short)Node.attribute("number").as_int((short)-1);
		lpItem.SafeZone = Node.attribute("number").as_int(0);
		lpItem.PosX1 = Node.attribute("x1").as_int((BYTE)-1);
		lpItem.PosY1 = Node.attribute("y1").as_int((BYTE)-1);
		lpItem.PosX2 = Node.attribute("x2").as_int((BYTE)-1);
		lpItem.PosY2 = Node.attribute("y2").as_int((BYTE)-1);
		this->m_MapZone.push_back(lpItem);
	}
}

void OfflineTrade::ProcStart(LPOBJ lpUser)
{
	if( !this->ProcCheck(lpUser) )
	{
		return;
	}

	if( lpUser->PartyNumber >= 0 )
	{
		int iPartyIndex = gParty.GetIndex(lpUser->PartyNumber, lpUser->m_Index, lpUser->DBNumber);

		if( iPartyIndex >= 0 )
		{
			PMSG_PARTYDELUSER pMsg;
			pMsg.Number = iPartyIndex;
			CGPartyDelUser(&pMsg, lpUser->m_Index);
		}
	}

	lpUser->iOffTradeTime = 0;
	lpUser->bOffTrade = 1;
	CloseClient(lpUser->m_Index);
}

bool OfflineTrade::ProcCheck(LPOBJ lpUser)
{
	if( !lpUser->m_bPShopOpen )
	{
		MsgOutput(lpUser->m_Index, "You need open Person Shop before use Offtrade");
		return false;
	}

	OfflineTradeMapZone* Zone = NULL;

	for( int i = 0; i < this->m_MapZone.size(); i++ )
	{
		if( this->m_MapZone[i].MapNumber == lpUser->MapNumber
			|| this->m_MapZone[i].MapNumber == (short)-1 )
		{
			Zone = &this->m_MapZone[i];
			break;
		}
	}

	if( !Zone )
	{
		return false;
	}

	BYTE btAttr = MapC[lpUser->MapNumber].GetAttr(lpUser->X, lpUser->Y);

	if( (btAttr&1) == FALSE && Zone->SafeZone == 1 )
	{
		MsgOutput(lpUser->m_Index, "You must have safe zone position before use Offtrade");
		return false;
	}

	return true;
}

void OfflineTrade::ProcRun(LPOBJ lpUser)
{
	if( lpUser->bOffTrade )
	{
		lpUser->iOffTradeTime++;

		if( this->m_MaxRunTime != 0 && lpUser->iOffTradeTime > this->m_MaxRunTime * 60 )
		{
			gObjDel(lpUser->m_Index);
		}
	}
}
