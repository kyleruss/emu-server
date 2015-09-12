#include "stdafx.h"
#include "MoveCommand.h"
#include "Gate.h"
#include "..\include\readscript.h"
#include "logproc.h"
#include "..\common\WzMemScript.h"
#include "DevilSquare.h"
#include "KalimaGate.h"
#include "ChaosCastle.h"
#include "CastleSiegeSync.h"
#include "CashShop.h"
#include "CashItemPeriodSystem.h"
#include "IllusionTempleEvent.h"
#include "DoubleGoerEvent.h"
#include "ImperialGuardian.h"
#include "GameEvent.h"
#include "Minimap.h"
#include "CastleSiege.h"
#include "RaklionBattleUserMng.h"
#include "..\pugixml\pugixml.hpp"

using namespace pugi;
CMoveCommand gMoveCommand;


CMoveCommand::CMoveCommand()
{
	return;
}


CMoveCommand::~CMoveCommand()
{
	return;
}


void CMoveCommand::Init()
{
	memset(this->m_MoveCommandData, 0, sizeof(this->m_MoveCommandData));

	for (int i =0;i<MAX_MOVE_COMMAND;i++)
	{
		this->m_MoveCommandData[i].Index = -1;
	}

	this->m_MoveLevel.clear();
	if( this->m_MoveLevel.capacity() > 0 )
	{
		std::vector<MOVE_MOVE_LEVEL>().swap(this->m_MoveLevel);
	}

}


int CMoveCommand::Load(char* filename)
{
	int MapCount = 0;
	xml_document Document;
	xml_parse_result Result = Document.load_file(filename);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[MoveList] Loading fail [%s]", filename);
		return 0;
	}
	// ----
	xml_node MoveList = Document.child("movelist");
	// ----
	for( xml_node Move = MoveList.child("move"); Move; Move = Move.next_sibling() )
	{
		this->m_MoveCommandData[MapCount].Index			= Move.attribute("id").as_int();
		this->m_MoveCommandData[MapCount].NeedZen		= Move.attribute("money").as_int();
		this->m_MoveCommandData[MapCount].NeedLevel		= Move.attribute("level").as_int();
		this->m_MoveCommandData[MapCount].GateNumber	= Move.attribute("gate").as_int();
		strcpy(this->m_MoveCommandData[MapCount].Name, Move.text().as_string());
		strcpy(this->m_MoveCommandData[MapCount].EngName, Move.text().as_string());
		MapCount++;
	}
	// ----
	return 1;
}


int CMoveCommand::LoadMoveLevel(char* filename)
{
	int MapCount = 0;
	xml_document Document;
	xml_parse_result Result = Document.load_file(filename);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[MoveLevel] Loading fail [%s]", filename);
		return 0;
	}
	// ----
	xml_node MoveLevel = Document.child("movelevel");
	// ----
	for( xml_node Move = MoveLevel.child("move"); Move; Move = Move.next_sibling() )
	{
		MOVE_MOVE_LEVEL lpInfo;
		lpInfo.MoveLevel = Move.attribute("level").as_int();
		lpInfo.MapNumber = Move.attribute("map").as_int();
		lpInfo.X = Move.attribute("x1").as_int();
		lpInfo.Y = Move.attribute("y1").as_int();
		lpInfo.TX = Move.attribute("x2").as_int();
		lpInfo.TY = Move.attribute("y2").as_int();
		this->m_MoveLevel.push_back(lpInfo);
	}
	return 1;
	/*int Token;
	int counter;

	memset(this->m_MoveLevel, -1, sizeof(this->m_MoveLevel));

	SMDFile=fopen(filename, "r");

	if ( SMDFile == 0 )
	{
		MsgBox("MoveLevel %s File Load Error", filename);
		return 0;
	}

	counter=0;

	while ( true )
	{
		Token=GetToken();

		if ( Token == 2)
		{
			break;
		}
		
		
		if ( Token == 1)
		{
			this->m_MoveLevel[counter].MoveLevel = (int)TokenNumber;

			Token = GetToken();
			this->m_MoveLevel[counter].MapNumber = (int)TokenNumber;

			Token = GetToken();
			this->m_MoveLevel[counter].X = (int)TokenNumber;

			Token = GetToken();
			this->m_MoveLevel[counter].Y = (int)TokenNumber;

			Token = GetToken();
			this->m_MoveLevel[counter].TX = (int)TokenNumber;

			Token = GetToken();
			this->m_MoveLevel[counter].TY = (int)TokenNumber;

			counter++;
		}
		else
		{
			this->Init();
			fclose(SMDFile);
			MsgBox("MoveLevel script load fail");
			return FALSE;
		}
	}
	fclose(SMDFile);
	return TRUE;*/
}



int  CMoveCommand::GetMoveLevel(int mapnumber, int x, int y, int Class)
{
	for( int i=0;i< this->m_MoveLevel.size();i++)
	{
		if ( this->m_MoveLevel[i].MapNumber == mapnumber )
		{
			if ( x >= this->m_MoveLevel[i].X && x <= this->m_MoveLevel[i].TX && y >= this->m_MoveLevel[i].Y && y <= this->m_MoveLevel[i].TY )
			{
				if ( Class == CLASS_DARKLORD || Class == CLASS_MAGUMSA || Class == CLASS_FIGHTER )
				{
					return this->m_MoveLevel[i].MoveLevel * 2 / 3;
				}
				else
				{
					return this->m_MoveLevel[i].MoveLevel;
				}
			}
		}
	}

	return -1;
}



BOOL CMoveCommand::CheckMainToMove(LPOBJ lpObj)
{
	if ( BC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( CC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( KALIMA_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( DS_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS )
	{
		return FALSE;
	}

	if ( lpObj->MapNumber == MAP_INDEX_BARRACKS
		|| lpObj->MapNumber == MAP_INDEX_REFUGE )
	{
		return FALSE;
	}

	if ( IT_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( DG_MAP_RANGE(lpObj->MapNumber) )
	{
		return FALSE;
	}

	if ( IF_MAP_RANGE(lpObj->MapNumber) )
	{
		return FALSE;
	}

	return TRUE;
}





BOOL CMoveCommand::CheckEquipmentToMove(LPOBJ lpObj, int iTargetMapNumber)
{
	if ( iTargetMapNumber == MAP_INDEX_ATHLANSE )
	{
		if ( lpObj->pInventory[8].IsItem() != FALSE )
		{
			if ( lpObj->pInventory[8].m_Type == ITEMGET(13,2) ) // Uniria
			{
				return FALSE;
			}

			if ( lpObj->pInventory[8].m_Type == ITEMGET(13,3) ) // Dino
			{
				return FALSE;
			}
		}
	}

	if ( iTargetMapNumber == MAP_INDEX_ICARUS )
	{
		if ( (lpObj->pInventory[8].m_Type != ITEMGET(13,3) &&
			 lpObj->pInventory[7].m_Type != ITEMGET(13,30) &&
			 lpObj->pInventory[7].m_Type != ITEMGET(12,49) &&
			 lpObj->pInventory[7].m_Type != ITEMGET(12,50) &&
			 lpObj->pInventory[8].m_Type != ITEMGET(13,37) &&
			 (lpObj->pInventory[7].m_Type < ITEMGET(12,0) || lpObj->pInventory[7].m_Type > ITEMGET(12,6)) &&
			 (lpObj->pInventory[7].m_Type < ITEMGET(12,36) || lpObj->pInventory[7].m_Type > ITEMGET(12,43)) &&
			 (lpObj->pInventory[7].m_Type < ITEMGET(12,130) || lpObj->pInventory[7].m_Type > ITEMGET(12,135)) &&
			 (lpObj->pInventory[7].m_Type < ITEMGET(12,262) || lpObj->pInventory[7].m_Type > ITEMGET(12,267)) ) ||
			 lpObj->pInventory[8].m_Type == ITEMGET(13,2) ||
			 lpObj->pInventory[11].m_Type == ITEMGET(13,10) ||
			 lpObj->pInventory[10].m_Type == ITEMGET(13,10) )
		{
#if( CUSTOM_WINGS == 1)
			if( lpObj->pInventory[7].m_Type >= ITEMGET(12,440) && lpObj->pInventory[7].m_Type <= ITEMGET(12,445))
			{

			}
			else
			{
				return FALSE;
			}
#else
			return FALSE;
#endif
		}

		if ( lpObj->m_RecallMon >= 0 )
		{
			GCRecallMonLife(lpObj->m_Index, 60, 0);
			gObjMonsterCallKill(lpObj->m_Index);
		}
	}

	return TRUE;
}





BOOL CMoveCommand::CheckInterfaceToMove(LPOBJ lpObj)
{
	if ( lpObj->m_IfState.use == 1 )
	{
		return FALSE;
	}

	if ( lpObj->m_bPShopOpen == true )
	{
		return FALSE;
	}

	return TRUE;
}




int  CMoveCommand::FindIndex(LPSTR mapname)
{
	for ( int i=0;i<MAX_MOVE_COMMAND;i++)
	{
		if ( this->m_MoveCommandData[i].Index == -1 )
		{
			return -1;
		}

		if ( strcmp(mapname, this->m_MoveCommandData[i].Name) == 0 || _strcmpi(mapname, this->m_MoveCommandData[i].EngName) == 0  )
		{
			return i;
		}
	}

	return -1;
}

int  CMoveCommand::FindIndex(int moveindex)
{
	for ( int i=0;i<MAX_MOVE_COMMAND;i++)
	{
		if ( this->m_MoveCommandData[i].Index == -1 )
		{
			return -1;
		}

		if ( this->m_MoveCommandData[i].Index == moveindex )
		{
			return i;
		}
	}

	return -1;
}


BOOL CMoveCommand::CheckMoveMapBound(int iMapIndex)
{
	switch ( iMapIndex )
	{
	case 0:	return FALSE;
	}
	
	return TRUE;
}




BOOL CMoveCommand::Move(LPOBJ lpObj, LPSTR mapname)
{
	int m_number = -1;
	int overlevel = 0;
	int subMoney = 2000;
	int index = FindIndex(mapname);

	if ( index == -1 )
	{
		return FALSE;
	}

	if ( g_bUseMoveMapBound == TRUE )
	{
		if ( g_CashItemPeriodSystem.CheckMoveMapBoundItem(lpObj) == FALSE )
		{
			if ( !CheckMoveMapBound(index))
			{
				MsgOutput(lpObj->m_Index,"Only players with Seal effect are allowed to this map.");
				return FALSE;
			}
		}
	}

	int NeedLevel = this->m_MoveCommandData[index].NeedLevel;
	int NeedZen   = this->m_MoveCommandData[index].NeedZen;
	int GateNumber= this->m_MoveCommandData[index].GateNumber;

	if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER )
	{
		if ( NeedLevel > 0 )
		{
			NeedLevel = NeedLevel *2 / 3;
		}
	}

	if ( GateNumber >= 0 )
	{
		if ( NeedLevel <= lpObj->Level )
		{
			if ( (lpObj->Money - NeedZen ) >= 0 )
			{

#if (__GENS_SYSTEM__ == 1)

		BYTE mapNumber;
		BYTE dir;
		BYTE x;
		BYTE y;
		short level;

		int result = gGateC.GetGate(GateNumber,x,y,(BYTE &)mapNumber,(BYTE &)dir,(short &)level);

		if( result < 0 )
		{
			LogAddTD("1[%s][%s] Use [%s %s] Fail GateNumber = %d",
				lpObj->AccountID, lpObj->Name,
				lMsg.Get(MSGGET(11, 185)), mapname, GateNumber);
			return FALSE;
		}

		if( gGensSystem.GensMap(mapNumber) && lpObj->m_GensInfo.btFamily == GENS_NONE )
		{
			GCServerMsgStringSend("You have not joined the gens and cannot enter the battle zone.",lpObj->m_Index,1);
			return FALSE;
		}

#endif

				if ( gObjMoveGate(lpObj->m_Index, GateNumber ) != FALSE )
				{
					lpObj->m_bPShopRedrawAbs = true;
					lpObj->Money -= NeedZen;

					LogAddTD("[%s][%s] Use [%s %s] Success : reduces money %d - %d"
						, lpObj->AccountID, lpObj->Name,
						lMsg.Get(MSGGET(11, 185)), mapname, lpObj->Money, NeedLevel);

					GCMoneySend(lpObj->m_Index, lpObj->Money );
					return TRUE;
				}
			}
			else
			{
				::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 102)), lpObj->m_Index, 1);
			}
		}
		else
		{
			char szTemp[256];

			wsprintf(szTemp, lMsg.Get(MSGGET(4, 93)), NeedLevel);
			::GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);
		}
	}

	LogAddTD("2[%s][%s] Use [%s %s] Fail GateNumber = %d",
		lpObj->AccountID, lpObj->Name,
		lMsg.Get(MSGGET(11, 185)), mapname, GateNumber);
	
	return FALSE;

}

BOOL CMoveCommand::Move(LPOBJ lpObj, int moveindex)
{
	int m_number = -1;
	int overlevel = 0;
	int subMoney = 2000;

	int index = FindIndex(moveindex);

	if ( index == -1 )
	{
		return FALSE;
	}

	if ( g_bUseMoveMapBound == TRUE )
	{
		if ( g_CashItemPeriodSystem.CheckMoveMapBoundItem(lpObj) == FALSE )
		{
			if ( !CheckMoveMapBound(index))
			{
				MsgOutput(lpObj->m_Index,"Only players with Seal effect are allowed to this map.");
				return FALSE;
			}
		}
	}

	/*if (lpObj->m_bPShopOpen)
	{
		::GCServerMsgStringSend("Please close your shop before changing locations", lpObj->m_Index, 1);
		return FALSE;
	}*/

	int NeedLevel = this->m_MoveCommandData[index].NeedLevel;
	int NeedZen   = this->m_MoveCommandData[index].NeedZen;
	int GateNumber = this->m_MoveCommandData[index].GateNumber;

	if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER )
	{
		if ( NeedLevel > 0 )
		{
			NeedLevel = NeedLevel *2 / 3;
		}
	}

	if ( GateNumber >= 0 )
	{
		if ( NeedLevel <= lpObj->Level )
		{
			if ( (lpObj->Money - NeedZen ) >= 0 )
			{

#if (__GENS_SYSTEM__ == 1)

		BYTE mapNumber = 0;
		BYTE dir;
		BYTE x;
		BYTE y;
		short level;

		int result = gGateC.GetGate(GateNumber,x,y,(BYTE &)mapNumber,(BYTE &)dir,(short &)level);

		if( result < 0 )
		{
			LogAddTD("3[%s][%s] Use [%s %s] Fail GateNumber = %d",
				lpObj->AccountID, lpObj->Name,
				lMsg.Get(MSGGET(11, 185)), m_MoveCommandData[index].EngName, GateNumber);
			return FALSE;
		}

		if( gGensSystem.GensMap(mapNumber) && lpObj->m_GensInfo.btFamily == GENS_NONE )
		{
			GCServerMsgStringSend("You have not joined the gens and cannot enter the battle zone.",lpObj->m_Index,1);
			return FALSE;
		}

#endif

				if ( gObjMoveGate(lpObj->m_Index, GateNumber ) != FALSE )
				{
					lpObj->m_bPShopRedrawAbs = true;
					lpObj->Money -= NeedZen;

					LogAddTD("[%s][%s] Use [%s %s] Success : reduces money %d - %d"
						, lpObj->AccountID, lpObj->Name,
						lMsg.Get(MSGGET(11, 185)), m_MoveCommandData[index].EngName, lpObj->Money, NeedLevel);

					GCMoneySend(lpObj->m_Index, lpObj->Money );

					if(lpObj->MapNumber == MAP_INDEX_RAKLIONBOSS)//Season 4.5 addon
					{
						g_RaklionBattleUserMng.DeleteUserData(lpObj->m_Index);
					}
					return TRUE;
				}
			}
			else
			{
				::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 102)), lpObj->m_Index, 1);
			}
		}
		else
		{
			char szTemp[256];

			wsprintf(szTemp, lMsg.Get(MSGGET(4, 93)), NeedLevel);
			::GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);
		}
	}

	LogAddTD("4[%s][%s] Use [%s %s] Fail GateNumber = %d",
		lpObj->AccountID, lpObj->Name,
		lMsg.Get(MSGGET(11, 185)), m_MoveCommandData[index].EngName, GateNumber);

	LogAddTD("user map: %d, teleport: %d, move gate: %d", lpObj->MapNumber, lpObj->Teleport, lpObj->m_MoveGateNumber);
	return FALSE;

}






int  GetKalimaGateLevel(LPOBJ lpObj)
{
	if ( lpObj->Class == 3 || lpObj->Class == 4 )
	{
		for ( int i=0;i<KALIMA_FLOORS;i++)
		{
			if ( lpObj->Level >= g_sttKALIMAGATE_LEVEL[i].iLOWER_BOUND_MAGUMSA && lpObj->Level <= g_sttKALIMAGATE_LEVEL[i].iUPPER_BOUND_MAGUMSA )
			{
				return i;
			}
		}
	}
	else
	{
		for ( int i=0;i<KALIMA_FLOORS;i++)
		{
			if ( lpObj->Level >= g_sttKALIMAGATE_LEVEL[i].iLOWER_BOUND && lpObj->Level <= g_sttKALIMAGATE_LEVEL[i].iUPPER_BOUND )
			{
				return i;
			}
		}
	}

	return -1;
}




BOOL CMoveCommand::MoveFree2Kalima(LPOBJ lpObj)
{
	int iIdx = GetKalimaGateLevel(lpObj);

	if ( iIdx >= 6 )
		iIdx = 5;

	if ( iIdx < 0 )
	{
		return FALSE;
	}

	int iGateNumber = iIdx + 88;

	return gObjMoveGate(lpObj->m_Index, iGateNumber);
}

MOVE_COMMAND_DATA* CMoveCommand::GetMoveCommandData(int nMapIndex) {
	int nIndex = -1;
	for(int i = 0; i < MAX_MOVE_COMMAND; i++) {
		if(this->m_MoveCommandData[i].Index == nMapIndex) {
			nIndex = i;
			break;
		}
	}
	if (nIndex >= 0){
		return &this->m_MoveCommandData[nIndex];
	}
	return NULL;
}