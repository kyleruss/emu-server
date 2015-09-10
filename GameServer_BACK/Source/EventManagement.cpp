#include "stdafx.h"
#include "EventManagement.h"
#include "LogProc.h"
#include "DragonEvent.h"
#include "AttackEvent.h"
#include "EledoradoEvent.h"
#include "RingAttackEvent.h"
#include "..\include\readscript.h"

CEventManagement g_EventManager;



CEventManagement::CEventManagement()
{
	this->m_bEventStart = false;
	this->m_bHasData = false;
	this->Clear();
}



CEventManagement::~CEventManagement()
{
	return;
}



void CEventManagement::Clear()
{
	this->m_vtEventTime.clear();
	this->m_mapEventObj.clear();
	this->m_wToday_Year = 0;
	this->m_wToday_Month = 0;
	this->m_wToday_Day = 0;
}




void CEventManagement::Init(BOOL bEventStart)
{
	this->m_bEventStart = bEventStart;

	if ( this->m_bHasData != false && this->m_bEventStart != false )
	{
		SYSTEMTIME sysTime;

		GetLocalTime(&sysTime);
		this->m_wToday_Year = sysTime.wYear;
		this->m_wToday_Month = sysTime.wMonth;
		this->m_wToday_Day = sysTime.wDay;

		std::map<int, void *>::iterator _Itor;
		
		_Itor = this->m_mapEventObj.find(EVENT_ID_DRAGONHERD);

		if ( _Itor != this->m_mapEventObj.end() )
		{
			CDragonEvent * lpEvent = (CDragonEvent *)_Itor->second;
			lpEvent->SetMenualStart(FALSE);

			for (std::vector<EVENT_ID_TIME>::iterator it = this->m_vtEventTime.begin() ; it != this->m_vtEventTime.end(); it++)
			{
				if ( (*(it)).m_eEventKind == EVENT_ID_DRAGONHERD )
				{
					lpEvent->SetMenualStart(TRUE);
				}
			}
		}

		_Itor = this->m_mapEventObj.find(EVENT_ID_UNDERTROOP);

		if ( _Itor != this->m_mapEventObj.end() )
		{
			CAttackEvent * lpEvent = (CAttackEvent *)_Itor->second;
			lpEvent->SetMenualStart(FALSE);

			for (std::vector<EVENT_ID_TIME>::iterator it = this->m_vtEventTime.begin() ; it != this->m_vtEventTime.end(); it++)
			{
				if ( (*(it)).m_eEventKind == EVENT_ID_UNDERTROOP )
				{
					lpEvent->SetMenualStart(TRUE);
				}
			}
		}

		_Itor = this->m_mapEventObj.find(EVENT_ID_ELDORADO);

		if ( _Itor != this->m_mapEventObj.end() )
		{
			CEledoradoEvent * lpEvent = (CEledoradoEvent *)_Itor->second;
			lpEvent->SetMenualStart(FALSE);

			for (std::vector<EVENT_ID_TIME>::iterator it = this->m_vtEventTime.begin() ; it != this->m_vtEventTime.end(); it++)
			{
				if ( (*(it)).m_eEventKind == EVENT_ID_ELDORADO )
				{
					lpEvent->SetMenualStart(TRUE);
				}
			}
		}


		_Itor = this->m_mapEventObj.find(EVENT_ID_WHITEMAGE);

		if ( _Itor != this->m_mapEventObj.end() )
		{
			CRingAttackEvent * lpEvent = (CRingAttackEvent *)_Itor->second;
			lpEvent->SetMenualStart(FALSE);

			for (std::vector<EVENT_ID_TIME>::iterator it = this->m_vtEventTime.begin() ; it != this->m_vtEventTime.end(); it++)
			{
				if ( (*(it)).m_eEventKind == EVENT_ID_WHITEMAGE )
				{
					lpEvent->SetMenualStart(TRUE);
				}
			}
		}
	}
	else
	{
		std::map<int, void *>::iterator _Itor;
		
		_Itor = this->m_mapEventObj.find(EVENT_ID_DRAGONHERD);
		
		if ( _Itor != this->m_mapEventObj.end() )
		{
			CDragonEvent * lpEvent = (CDragonEvent *)_Itor->second;
			lpEvent->SetMenualStart(FALSE);
		}

	
		_Itor = this->m_mapEventObj.find(EVENT_ID_UNDERTROOP);
		
		if ( _Itor != this->m_mapEventObj.end() )
		{
			CAttackEvent * lpEvent = (CAttackEvent *)_Itor->second;
			lpEvent->SetMenualStart(FALSE);
		}

		_Itor = this->m_mapEventObj.find(EVENT_ID_ELDORADO);
		
		if ( _Itor != this->m_mapEventObj.end() )
		{
			CEledoradoEvent * lpEvent = (CEledoradoEvent *)_Itor->second;
			lpEvent->SetMenualStart(FALSE);
		}

		_Itor = this->m_mapEventObj.find(EVENT_ID_WHITEMAGE);
		
		if ( _Itor != this->m_mapEventObj.end() )
		{
			CRingAttackEvent * lpEvent = (CRingAttackEvent *)_Itor->second;
			lpEvent->SetMenualStart(FALSE);
		}
	}
}



void CEventManagement::Run()
{
	if ( this->m_bEventStart == false || this->m_bHasData == false || this->m_vtEventTime.empty() != FALSE )
	{
		return;
	}

	SYSTEMTIME sysTime;
	std::vector<EVENT_ID_TIME>::iterator it;

	GetLocalTime(&sysTime);

	if ( this->m_wToday_Year != sysTime.wYear || this->m_wToday_Month != sysTime.wMonth || this->m_wToday_Day != sysTime.wDay )
	{
		for ( it = this->m_vtEventTime.begin() ; it != this->m_vtEventTime.end() ; it++ )
		{
			(*(it)).m_bEventStarted = false;
		}

		LogAddTD("¡Û¡Ü[Event Management] ³¯Â¥ º¯°æµÊ. %02d %02d %02d", this->m_wToday_Year, this->m_wToday_Month, this->m_wToday_Day);	// Deathway Need Translation

		this->m_wToday_Year = sysTime.wYear;
		this->m_wToday_Month = sysTime.wMonth;
		this->m_wToday_Day = sysTime.wDay;
	}

	for ( it = this->m_vtEventTime.begin() ; it != this->m_vtEventTime.end() ; it++ )
	{
		if ( sysTime.wHour == (*(it)).m_iHour && sysTime.wMinute == (*(it)).m_iMinute && (*(it)).m_bEventStarted == false )
		{
			LogAddTD("[Event Management] Event Schedule (%d) %02d %02d (state=%d)",
				(*(it)).m_eEventKind , (*(it)).m_iHour, sysTime.wMinute , (*(it)).m_bEventStarted);	// #Deathway Need Translation

			(*(it)).m_bEventStarted = true;
			this->StartEvent((*(it)).m_eEventKind);
		}
	}
}






void CEventManagement::StartEvent(int eEventKind)
{
	switch ( eEventKind )
	{
		case EVENT_ID_DRAGONHERD:
			{
				std::map<int, void *>::iterator _Itor = this->m_mapEventObj.find(EVENT_ID_DRAGONHERD);

				if ( _Itor != this->m_mapEventObj.end() )
				{
					CDragonEvent * lpEvent = (CDragonEvent *)_Itor->second;
					lpEvent->Start_Menual();
				}
			}
			break;

		case EVENT_ID_UNDERTROOP:
			{
				std::map<int, void *>::iterator _Itor = this->m_mapEventObj.find(EVENT_ID_UNDERTROOP);

				if ( _Itor != this->m_mapEventObj.end() )
				{
					CAttackEvent * lpEvent = (CAttackEvent *)_Itor->second;
					lpEvent->Start_Menual();
				}
			}
			break;

		case EVENT_ID_ELDORADO:
			{
				std::map<int, void *>::iterator _Itor = this->m_mapEventObj.find(EVENT_ID_ELDORADO);

				if ( _Itor != this->m_mapEventObj.end() )
				{
					CEledoradoEvent * lpEvent = (CEledoradoEvent *)_Itor->second;
					lpEvent->Start_Menual();
				}
			}
			break;

		case EVENT_ID_WHITEMAGE:
			{
				std::map<int, void *>::iterator _Itor = this->m_mapEventObj.find(EVENT_ID_WHITEMAGE);

				if ( _Itor != this->m_mapEventObj.end() )
				{
					CRingAttackEvent * lpEvent = (CRingAttackEvent *)_Itor->second;
					lpEvent->Start_Menual();
				}
			}
			break;
	}
}






bool CEventManagement::Load(LPSTR lpszFileName)
{
	this->m_bHasData = false;
	this->Clear();

	SMDFile = fopen(lpszFileName, "r");

	if ( SMDFile == NULL )
	{
		MsgBox("[DOTH] Info file Load Fail [%s]", lpszFileName);

		return false;
	}

	int Token;
	int type = -1;

	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}

		type = (int)TokenNumber;

		while ( true )
		{
			if ( type == 0 )
			{
				Token = GetToken();

				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				EVENT_ID_TIME EIT;

				EIT.m_eEventKind = (int)TokenNumber;

				Token = GetToken();
				EIT.m_iHour = (int)TokenNumber;

				Token = GetToken();
				EIT.m_iMinute = (int)TokenNumber;

				EIT.m_bEventStarted = false;

				this->m_vtEventTime.push_back(EIT);
			}
		}
	}

	fclose(SMDFile);
	LogAdd("[DOTH] %s file load!", lpszFileName);

	this->m_bHasData = true;

	return true;
}






void CEventManagement::RegEvent(int eEventKind, void * lpEventObj)
{
	this->m_mapEventObj.insert( std::pair<int, void *>(eEventKind, lpEventObj) );
}