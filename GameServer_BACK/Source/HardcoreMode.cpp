#include "stdafx.h"
#include "HardcoreMode.h"
#include "..\pugixml\pugixml.hpp"
#include "GameMain.h"
#include "logproc.h"
#include "DBSockMng.h"

HardcoreMode g_HardcoreMode;
using namespace pugi;

HardcoreMode::HardcoreMode()
{

}

HardcoreMode::~HardcoreMode()
{

}

void HardcoreMode::Init()
{
	this->m_IsActive = false;
	this->m_LifeDecreaseByPvP = false;
	this->m_LifeDecreaseByPvM = false;
	this->m_LifeDefaultCount = 0;
	ZeroMemory(this->m_ManagerCommandAdd, 0);
	ZeroMemory(this->m_ManagerCommandDec, 0);
	ZeroMemory(this->m_ManagerCommandSet, 0);
	ZeroMemory(this->m_ManagerCommandInfo, 0);
}

void HardcoreMode::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CUSTOM_HARDCOREMODE));
}

void HardcoreMode::Read(LPSTR File)
{
	
	return;
	

	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if( Result.status != status_ok )
	{
		MsgBox("[HardcoreMode] File %s not found!", File);
		return;
	}

	xml_node ChildCommon = Document.child("hardcore");

	this->m_IsActive = ChildCommon.child("enabled").text().as_int(0);
	this->m_LifeDefaultCount = ChildCommon.child("defaultlifecount").text().as_int(0);

	strcpy(this->m_ManagerCommandAdd, ChildCommon.child("opcommandadd").text().as_string());
	strcpy(this->m_ManagerCommandDec, ChildCommon.child("opcommanddec").text().as_string());
	strcpy(this->m_ManagerCommandSet, ChildCommon.child("opcommandset").text().as_string());
	strcpy(this->m_ManagerCommandInfo, ChildCommon.child("opcommandinfo").text().as_string());

	this->m_LifeDecreaseByPvP = ChildCommon.child("rules").child("pvplifedec").text().as_int(0);
	this->m_LifeDecreaseByPvM = ChildCommon.child("rules").child("pvmlifedec").text().as_int(0);
}

void HardcoreMode::Activate()
{
	if( !this->m_IsActive )
	{
		return;
	}

	GD_HardcoreMode_Activate lpRequest = { 0 };
	lpRequest.Header.set((LPBYTE)&lpRequest, 0xFE, 0x00, sizeof(lpRequest));
	lpRequest.Status = this->m_IsActive;
	lpRequest.LifeCount = this->m_LifeDefaultCount;
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(lpRequest));
}

void HardcoreMode::ProcDie(LPOBJ lpUser, LPOBJ lpKiller)
{
	if( !this->m_IsActive )
	{
		return;
	}

	if( lpUser->Authority == 32 )
	{
		return;
	}

	if( lpKiller->Type == OBJ_USER && !this->m_LifeDecreaseByPvP )
	{
		return;
	}

	if( lpKiller->Type == OBJ_MONSTER && !this->m_LifeDecreaseByPvM )
	{
		return;
	}
	
	lpUser->m_HardcoreLife--;
	
	if( lpKiller->Type == OBJ_MONSTER )
	{
		LPMONSTER_ATTRIBUTE MonAttr = gMAttr.GetAttr(lpKiller->Class);
		
		if( MonAttr != NULL )
		{
			LogAddTD("[HardcoreMode] [%s][%s] (Life: %d) User has been killed by %s", 
				lpUser->AccountID, lpUser->Name, lpUser->m_HardcoreLife, MonAttr->m_Name);
		}
	}
	else if( lpKiller->Type == OBJ_USER )
	{
		LogAddTD("[HardcoreMode] [%s][%s] (Life: %d) User has been killed by %s", 
			lpUser->AccountID, lpUser->Name, lpUser->m_HardcoreLife, lpKiller->Name);
	}

	if( lpUser->m_HardcoreLife < 1 )
	{
		gObjCloseSet(lpUser->m_Index, 1);
	}
	else
	{
		char Text[100];
		sprintf(Text, "Lives left: %d", lpUser->m_HardcoreLife);
		MsgOutput(lpUser->m_Index, Text);
	}
}
