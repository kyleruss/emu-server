#include "stdafx.h"
#include "Achievements.h"
#include "GameMain.h"
#include "logproc.h"
#include "DBSockMng.h"
#include "..\pugixml\pugixml.hpp"
// -------------------------------------------------------------------------------

using namespace pugi;
#if( __4GAMERS__ == 1 )
CLogToFile g_AchievLog("Achievements", ".\\Log\\Achievements", true);
#endif
Achievements g_Achievements;
// -------------------------------------------------------------------------------

Achievements::Achievements()
{

}
// -------------------------------------------------------------------------------

Achievements::~Achievements()
{

}
// -------------------------------------------------------------------------------

void Achievements::Init()
{
	this->m_MonsterList.clear();
	if( this->m_MonsterList.capacity() > 0 )
	{
		std::vector<AchievMonsterInfo>().swap(this->m_MonsterList);
	}
}
// -------------------------------------------------------------------------------

void Achievements::Load()
{
	this->Init();
	this->ReadMonsterData(gDirPath.GetNewPath(FILE_CUSTOM_ACHIEVEMENTS));
}
// -------------------------------------------------------------------------------

void Achievements::ReadMonsterData(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[Achievements] [Monster] File %s not found! %d", File, Result.status);
		return;
	}
	// ----
	xml_node Achievements = Document.child("achievements");
	xml_node MonsterList = Achievements.child("monsterlist");
	// ----
	for( xml_node Node = MonsterList.child("monster"); Node; Node = Node.next_sibling() )
	{
		AchievMonsterInfo lpInfo = { 0 };
		lpInfo.m_MonsterID = Node.attribute("id").as_int();
		lpInfo.m_MapNumber = Node.attribute("map").as_int(-1);;
		this->m_MonsterList.push_back(lpInfo);
	}
}
// -------------------------------------------------------------------------------

void Achievements::MonsterProc(LPOBJ lpUser, LPOBJ lpMonster)
{
#if( __4GAMERS__ == 1 )
	AchievMonsterInfo* lpMonsterInfo = this->MonsterGet(lpMonster);
	// ----
	if( !lpMonsterInfo )
	{
		return;
	}
	// ----
	g_AchievLog.Output("[%s] [%s] Updated data for monster (id: %d)",
		lpUser->AccountID, lpUser->Name, 
		lpMonster->Class);
	LogAddTD("[%s] [%s] [Achievements] Updated data for monster (id: %d)",
		lpUser->AccountID, lpUser->Name,
		lpMonster->Class);
	// ----
	this->GD_UpdateMonsterData(lpUser, lpMonster);
#endif
}
// -------------------------------------------------------------------------------

AchievMonsterInfo* Achievements::MonsterGet(LPOBJ lpMonster)
{
	for( int i = 0; i < this->m_MonsterList.size(); i++ )
	{
		if( this->m_MonsterList[i].m_MonsterID == lpMonster->Class )
		{
			if( this->m_MonsterList[i].m_MapNumber != (short)-1 )
			{
				if( this->m_MonsterList[i].m_MapNumber != lpMonster->MapNumber )
				{
					continue;
				}
			}
			// ----
			return &this->m_MonsterList[i];
		}
	}
	// ----
	return 0;
}
// -------------------------------------------------------------------------------

void Achievements::GD_UpdateMonsterData(LPOBJ lpUser, LPOBJ lpMonster)
{
	if( !gObjIsConnected(lpUser->m_Index) )
	{
		return;
	}

	AchievGDUpdateMonster lpData = { 0 };
	lpData.Head.set((LPBYTE)&lpData, 0xA6, 0x00, sizeof(lpData));
	lpData.MonsterID = lpMonster->Class;
	strcpy(lpData.Name, lpUser->Name);
	cDBSMng.Send((PCHAR)&lpData, sizeof(lpData));
}
// -------------------------------------------------------------------------------

void Achievements::GD_UpdateEventDataBC(LPOBJ lpUser, int BC_Exp, int BC_ExtExp, int BC_Score)
{
	if( !gObjIsConnected(lpUser->m_Index) )
	{
		return;
	}

	AchievGDUpdateEvent lpData = { 0 };
	lpData.Head.set((LPBYTE)&lpData, 0xA6, 0x01, sizeof(AchievGDUpdateEvent));
	strcpy(lpData.Name, lpUser->Name);

	lpData.EventType	= AchievEventType::BloodCastle;
	lpData.BC_Exp		= BC_Exp;
	lpData.BC_ExtExp	= BC_ExtExp;
	lpData.BC_Score		= BC_Score;
#if( __4GAMERS__ == 1 )
	g_AchievLog.Output("[%s] [%s] Updated data for BloodCastle (Exp: %d, ExtExp: %d, Score: %d)",
		lpUser->AccountID, lpData.Name, BC_Exp, BC_ExtExp, BC_Score);
#endif
	LogAddTD("[%s] [%s] [Achievements] Updated data for BloodCastle (Exp: %d, ExtExp: %d, Score: %d)",
		lpUser->AccountID, lpUser->Name, BC_Exp, BC_ExtExp, BC_Score);

	cDBSMng.Send((PCHAR)&lpData, sizeof(lpData));
}
// -------------------------------------------------------------------------------

void Achievements::GD_UpdateEventDataDS(LPOBJ lpUser, int DS_Exp, int DS_Point)
{
	if( !gObjIsConnected(lpUser->m_Index) )
	{
		return;
	}

	AchievGDUpdateEvent lpData = { 0 };
	lpData.Head.set((LPBYTE)&lpData, 0xA6, 0x01, sizeof(AchievGDUpdateEvent));
	strcpy(lpData.Name, lpUser->Name);

	lpData.EventType	= AchievEventType::DevilSquare;
	lpData.DS_Exp		= DS_Exp;
	lpData.DS_Point		= DS_Point;
#if( __4GAMERS__ == 1 )
	g_AchievLog.Output("[%s] [%s] Updated data for DevilSquare (Exp: %d, Point: %d)",
		lpUser->AccountID, lpUser->Name, DS_Exp, DS_Point);
#endif
	LogAddTD("[%s] [%s] [Achievements] Updated data for DevilSquare (Exp: %d, Point: %d)",
		lpUser->AccountID, lpUser->Name, DS_Exp, DS_Point);

	cDBSMng.Send((PCHAR)&lpData, sizeof(lpData));
}
// -------------------------------------------------------------------------------

void Achievements::GD_UpdateEventDataCC(LPOBJ lpUser, int CC_UKill, int CC_MKill, int CC_TExp, int CC_RExp)
{
	if( !gObjIsConnected(lpUser->m_Index) )
	{
		return;
	}

	AchievGDUpdateEvent lpData = { 0 };
	lpData.Head.set((LPBYTE)&lpData, 0xA6, 0x01, sizeof(AchievGDUpdateEvent));
	strcpy(lpData.Name, lpUser->Name);

	lpData.EventType	= AchievEventType::ChaosCastle;
	lpData.CC_UKill		= CC_UKill;
	lpData.CC_MKill		= CC_MKill;
	lpData.CC_TExp		= CC_TExp;
	lpData.CC_RExp		= CC_RExp;
#if( __4GAMERS__ == 1 )
	g_AchievLog.Output("[%s] [%s] Updated data for ChaosCastle (UKill: %d, MKill: %d, TExp: %d, RExp: %d)",
		lpUser->AccountID, lpUser->Name, CC_UKill, CC_MKill, CC_TExp, CC_RExp);
#endif
	LogAddTD("[%s] [%s] [Achievements] Updated data for ChaosCastle (UKill: %d, MKill: %d, TExp: %d, RExp: %d)",
		lpUser->AccountID, lpUser->Name, CC_UKill, CC_MKill, CC_TExp, CC_RExp);

	cDBSMng.Send((PCHAR)&lpData, sizeof(lpData));
}
// -------------------------------------------------------------------------------

void Achievements::GD_UpdateMixData(LPOBJ lpUser, bool Success)
{
	if( !gObjIsConnected(lpUser->m_Index) )
	{
		return;
	}

	AchievGDUpdateMix lpData = { 0 };
	lpData.Head.set((LPBYTE)&lpData, 0xA6, 0x02, sizeof(AchievGDUpdateMix));
	strcpy(lpData.Name, lpUser->Name);
	lpData.Success = Success;

	LogAddTD("[%s] [%s] [Achievements] Updated data for ChaosMachine (Result: %d)",
		lpUser->AccountID, lpUser->Name, Success);

	cDBSMng.Send((PCHAR)&lpData, sizeof(lpData));
}
// -------------------------------------------------------------------------------