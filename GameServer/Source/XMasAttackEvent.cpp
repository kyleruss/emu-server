#include "stdafx.h"
#include "XMasAttackEvent.h"
#include "gObjMonster.h"
#include "GameMain.h"
#include "DSProtocol.h"
#include "protocol.h"
#include "LogProc.h"
#include "Event.h"
#include "BuffEffectSlot.h"
#include "..\include\readscript.h"
#include "..\common\winutil.h"

CXMasMonsterHerd::CXMasMonsterHerd()
{
	return;
}



CXMasMonsterHerd::~CXMasMonsterHerd()
{
	return;
}

BOOL CXMasMonsterHerd::Start()
{
	return this->MonsterHerd::Start();
}


BOOL CXMasMonsterHerd::MonsterHerdItemDrop(LPOBJ lpObj)
{
	if( lpObj->Class != 476 && lpObj->Class != 466 )
	{
		return false;
	}

	if( lpObj->Class == 476 )
	{
		int Count = 0;
		int _Type = rand()%3;

		switch( _Type )
		{
		case 0:
			{
			int AddDmg = 0;
			int AddDef = 0;

			if( lpObj->Level <= 180 )
			{
				AddDmg = lpObj->Level / 3 + 45;
				AddDef = lpObj->Level / 5 + 50;
			}
			else
			{
				AddDmg = 105;
				AddDef = 86;
			}

			gObjAddBuffEffectWideArea(lpObj,10,Count,0x5B,2,AddDmg,3,AddDef,1800);
			}
			break;
		case 1:
			gObjAddBuffEffectWideArea(lpObj,10,Count,0x5C,4,500,0,0,1800);
			break;
		case 2:
			gObjAddBuffEffectWideArea(lpObj,10,Count,0x5D,5,500,0,0,1800);
			break;
		}

		LogAddTD("[XMasAttackEvent] White Wizard Killed, MapNumber:%d",lpObj->MapNumber);
		return true;
	}
	else if(lpObj->Class == 466 )
	{
		if( rand()%100 < g_iXMasItemDropRate )
		{
			int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
			int ItemType = ItemGetNumberMake(13,66);

			ItemSerialCreateSend(lpObj->m_Index,lpObj->MapNumber,lpObj->X,lpObj->Y,ItemType,0,10,0,0,0,iMaxHitUser,0,0);
			return true;
		}
		else
		{
			MapC[lpObj->MapNumber].MoneyItemDrop(g_iXMasZenDrop,(BYTE)lpObj->X,(BYTE)lpObj->Y);
			return true;
		}
	}
return false;
}

CXMasAttackEvent::CXMasAttackEvent()
{

}

CXMasAttackEvent::~CXMasAttackEvent()
{

}

BOOL CXMasAttackEvent::Load(char* lpszFileName)
{

	SMDFile = fopen(lpszFileName,"r");

	if(SMDFile == NULL )
	{
		MsgBox("[XMasAttackEvent] Info file Load Fail [%s]",lpszFileName);
		return false;
	}

	Clear();

	int Token,Index = -1;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		Index = (int)TokenNumber;

		while( TRUE )
		{
			if( Index == 0 )
			{
				Token = GetToken();
				if( strcmp("end",TokenString) == NULL )
					break;

				m_iTIME_OPEN = (int)TokenNumber*60000;

				Token = GetToken();
				m_iTIME_PLAY = (int)TokenNumber*60000;

				Token = GetToken();
				m_iTIME_CLOSE = (int)TokenNumber*60000;
			}
			else if( Index == 1 )
			{
				Token = GetToken();
				if( strcmp("end",TokenString) == NULL )
					break;

				this->m_iMOVE_RAND_SIZE = (int)TokenNumber;

				Token = GetToken();
				this->m_iRADIUS_MIN = (int)TokenNumber;

				Token = GetToken();
				this->m_iRADIUS_MAX = (int)TokenNumber;
			}
			else if( Index == 2 )
			{
				Token = GetToken();
				if( strcmp("end",TokenString) == NULL )
					break;

				MONSTER_EVENT_DATA tmp;

				tmp.m_iType = (int)TokenNumber;

				Token = GetToken();
				tmp.m_bDoRegen = (int)TokenNumber;

				Token = GetToken();
				tmp.m_bDoAttackFirst = (int)TokenNumber;

				EnterCriticalSection(&m_critsec);

				m_EventMonster.push_back(tmp);

				LeaveCriticalSection(&m_critsec);
			}
			else if( Index == 3 )
			{
				Token = GetToken();
				if( strcmp("end",TokenString) == NULL )
					break;

				MONSTER_EVENT_TIME tmp;

				tmp.m_iHour = (int)TokenNumber;

				Token = GetToken();
				tmp.m_iMinute = (int)TokenNumber;

				m_EventTime.push_back( tmp );
			}
			else if( Index == 4 )
			{
				Token = GetToken();
				if( strcmp("end",TokenString) == NULL )
					break;

				XMASMONSTER_MAP_DATA tmp;

				tmp.MapNumber = (int)TokenNumber;

				Token = GetToken();
				tmp.X1 = (int)TokenNumber;

				Token = GetToken();
				tmp.Y1 = (int)TokenNumber;

				Token = GetToken();
				tmp.X2 = (int)TokenNumber;

				Token = GetToken();
				tmp.Y2 = (int)TokenNumber;

				m_EventMapData.push_back( tmp );
			}
		}
	}

	fclose(SMDFile);
	LogAdd("[XMasAttackEvent] - %s file load!",lpszFileName);
	this->m_bDoHasData = TRUE;
	return true;
}

void CXMasAttackEvent::StartEvent()
{
	if( m_bDoEvent == FALSE )
		return;

	if( m_bDoHasData == FALSE )
		return;

	if( m_EventMonster.empty() )
		return;

	RemoveMonsterHerd();
	CreateMonsterHerd();

	for(DWORD n = 0; n < m_EventMapData.size(); n++)
	{
		XMASMONSTER_MAP_DATA tmpMapData = m_EventMapData[n];
		CXMasMonsterHerd* lpMonsterHerd = m_MonsterHerdData[n];
		int loop = 1000;
		int X = 0;
		int Y = 0;

		while( loop-- != 0 )
		{
			X = rand()%(tmpMapData.X2-tmpMapData.X1)+tmpMapData.X1;
			Y = rand()%(tmpMapData.Y2-tmpMapData.Y1)+tmpMapData.Y1;

			if( lpMonsterHerd->SetTotalInfo(tmpMapData.MapNumber,3,X,Y) == TRUE )
				break;
		}

		if( loop == 0 )
		{

		}
		else if( m_EventMonster.empty() )
		{
			LogAdd("[XMasAttackEvent] - Error : No Monster Data Exist");
			continue;
		}
		else
		{
			LogAdd("[XMasAttackEvent] - Monster Start Position MapNumber:%d, X:%d, Y:%d",
				tmpMapData.MapNumber,X,Y);
		}

		AddMonsters(lpMonsterHerd);
		lpMonsterHerd->Start();
	}

}

void CXMasAttackEvent::StopEvent()
{
	for(std::vector<CXMasMonsterHerd*>::iterator _It = m_MonsterHerdData.begin(); _It != m_MonsterHerdData.end(); _It++)
	{
		CXMasMonsterHerd* lpMonsterHerd = *_It;
		lpMonsterHerd->Stop();
	}

	m_bMonsterMove = FALSE;
}

void CXMasAttackEvent::SetEnable(int bEnable)
{
	this->m_bDoEvent = bEnable;

	if( m_bDoEvent != 0 )
	{
		SetState(ATTACKEVENT_STATE_CLOSED);
	}
	else
	{
		SetState(ATTACKEVENT_STATE_PLAYING);
	}
}

void CXMasAttackEvent::SetState(int nState)
{
	if( nState < ATTACKEVENT_STATE_NONE && nState > ATTACKEVENT_STATE_PLAYING )
	{
		LogAdd("%s	%s	%s	%s	%d",
			"(nSate >= ATTACKEVENT_STATE_NONE) || (nSate <= ATTACKEVENT_STATE_PLAYING)","","NULL",__FILE__,__LINE__);
		return;
	}

	if( m_nEventState < ATTACKEVENT_STATE_NONE && m_nEventState > ATTACKEVENT_STATE_PLAYING )
	{
		LogAdd("%s	%s	%s	%s	%d",
			"(m_nEventState >= ATTACKEVENT_STATE_NONE) || (m_nEventState <= ATTACKEVENT_STATE_PLAYING)","","NULL",__FILE__,__LINE__);
		return;
	}

	m_nEventState = nState;

	switch( m_nEventState )
	{
	case ATTACKEVENT_STATE_NONE:
		SetState_NONE();
		break;
	case ATTACKEVENT_STATE_CLOSED:
		SetState_CLOSED();
		break;
	case ATTACKEVENT_STATE_PLAYING:
		SetState_PLAYING();
		break;
	}

}

void CXMasAttackEvent::ProcState_NONE()
{

}

void CXMasAttackEvent::ProcState_CLOSED()
{
	if( m_bDoEvent == FALSE )
	{
		SetState(ATTACKEVENT_STATE_NONE);
		LogAdd("%s	%s	%s	%s	%d",
			"m_bDoEvent","","SetState(ATTACKEVENT_STATE_NONE)",__FILE__,__LINE__);
		return;
	}

	int TICK_COUNT = m_TimeCounter.GetMiliSeconds();

	if( TICK_COUNT >= 1000 )
	{
		if( TICK_COUNT/60000 != m_iLEFT_MIN )
		{
			m_iLEFT_MIN = TICK_COUNT/60000;

			if( m_iLEFT_MIN+1 == m_iTIME_OPEN / 60000 )
			{
				LogAdd("[XMasAttackEvent] - Before %d minutes - Advertise",m_iLEFT_MIN+1);
			}
		}
	}

	if( TICK_COUNT == 0 )
	{
		SendEffect(lMsg.Get(MSGGET(10,23)),1);

		if( m_bDoEvent != 0 )
		{
			SetState(ATTACKEVENT_STATE_PLAYING);
		}
		else
		{
			SetState(ATTACKEVENT_STATE_NONE);
		}

		LogAdd("[XMasAttackEvent] - Event Started");
	}
}

struct PMSG_ANS_CL_EFFECT
{
	PBMSG_HEAD h;	// C1:9E
	WORD wEffectNum;	// 4
};

void CXMasAttackEvent::SendEffect(LPSTR szMsg,int Effect)
{
	char szBuff[256];
	wsprintf(szBuff,szMsg);
	AllSendServerMsg(szBuff);

	PMSG_ANS_CL_EFFECT pMsg;
	PHeadSetB((LPBYTE)&pMsg,0x9E, sizeof(pMsg));

	pMsg.wEffectNum = Effect;

	DataSendAll((LPBYTE)&pMsg,pMsg.h.size);
}

int PlaySeconds = -1;

static int g_XMasMapDestPosition[3][8] = {	133, 79, 87, 126,
														133, 178, 180, 126,
														164, 42, 221, 85,
														164, 42, 221, 85,
														160, 45, 160, 45, 
														152, 117, 209, 133 };

void CXMasAttackEvent::ProcState_PLAYING()
{
	int TICK_COUNT = m_TimeCounter.GetMiliSeconds();
	int Seconds = TICK_COUNT / 1000;

	if( TICK_COUNT >= 1000 && rand()%20 == 0)
	{
		Move();
	}

	if( Seconds / 30 != PlaySeconds && Seconds % 30 == 0)
	{
		PlaySeconds = Seconds / 30;

		for(DWORD n = 0; n < m_MonsterHerdData.size(); n++)
		{
			CXMasMonsterHerd* lpMonsterHerd = m_MonsterHerdData[n];
			int bLive = FALSE;
	
			for(int i = 0; i < OBJ_MAXMONSTER; i++)
			{
				if( gObj[i].Class == 476 &&
					gObj[i].m_bIsInMonsterHerd != 0 &&
					gObj[i].Live != 0 &&
					gObj[i].MapNumber == lpMonsterHerd->GetMap() )
				{
					bLive = TRUE;
					break;
				}
			}
	
			LogAdd("[XMasAttackEvent] - per 30'S Inform MapNumber:%d, X:%d, Y:%d, Boss Live:%d",
				lpMonsterHerd->GetMap(),lpMonsterHerd->GetX(),lpMonsterHerd->GetY(),bLive);
			Notify("",lpMonsterHerd->GetMap(),lpMonsterHerd->GetX(),lpMonsterHerd->GetY());
		}

		LogAdd("[XMasAttackEvent] - NotifySec: %d",PlaySeconds);
	}

	if( m_bMonsterMove == FALSE )
	{
		if( TICK_COUNT <= m_iTIME_CLOSE )
		{
			for(DWORD n = 0; n < m_MonsterHerdData.size(); n++)
			{
				CXMasMonsterHerd* lpMonsterHerd = m_MonsterHerdData[n];

				BYTE ucRAND;
				BYTE iRandX;
				BYTE iRandY;

				ucRAND = rand()%4;
				iRandX = g_XMasMapDestPosition[n][ucRAND*2];
				iRandY = g_XMasMapDestPosition[n][ucRAND*2+1];

				if( lpMonsterHerd->MoveHerd(iRandX,iRandY) )
				{
					LogAdd("[XMasAttackEvent] - Monster Herd Move MapNumber:%d, X:%d, Y:%d",
						lpMonsterHerd->GetMap(),iRandX,iRandY);
					Notify("",lpMonsterHerd->GetMap(),iRandX,iRandY);

					
				}
			}

			m_bMonsterMove = TRUE;
		}
	}

	if( TICK_COUNT == 0 )
	{
		if( m_bDoEvent != 0 )
		{
			SetState(ATTACKEVENT_STATE_CLOSED);
		}
		else
		{
			SetState(ATTACKEVENT_STATE_NONE);
		}

		LogAdd("[XMasAttackEvent] - Event Ended");
	}
}

void CXMasAttackEvent::SetState_NONE()
{
	StopEvent();
}

void CXMasAttackEvent::SetState_CLOSED()
{
	StopEvent();

	if( m_bDoEvent != 0 )
	{
		CheckSync();
	}
	else
	{
		SetState(ATTACKEVENT_STATE_NONE);
	}
}

void CXMasAttackEvent::SetState_PLAYING()
{
	m_TimeCounter.SetMiliSeconds(m_iTIME_PLAY);
	StartEvent();
}

void CXMasAttackEvent::Move()
{
	if( m_bDoEvent == 0 )
		return;

	if( m_bMonsterMove != 0 )
		return;

	for(DWORD n = 0; n < m_MonsterHerdData.size(); n++)
	{
		XMASMONSTER_MAP_DATA tmpMapData = m_EventMapData[n];
		CXMasMonsterHerd* lpMonsterHerd = m_MonsterHerdData[n];

		if( CheckLocation(lpMonsterHerd,&tmpMapData) == FALSE )
			break;

		int DiffRadius = m_iRADIUS_MAX - m_iRADIUS_MIN;

		if( DiffRadius <= 0 )
			DiffRadius = 1;

		int MaxRadius = DiffRadius + m_iRADIUS_MIN;

		if( MaxRadius <= 3 )
			MaxRadius = 3;

		lpMonsterHerd->SetRadius(rand()%(m_iRADIUS_MAX - m_iRADIUS_MIN) + m_iRADIUS_MIN);
	}
}

void CXMasAttackEvent::Notify(LPSTR szFormat,int Map,BYTE X,BYTE Y)
{

}

void CXMasAttackEvent::CreateMonsterHerd()
{
	EnterCriticalSection(&m_critsec);

	for(std::vector<XMASMONSTER_MAP_DATA>::iterator _It = m_EventMapData.begin(); _It != m_EventMapData.end(); _It++)
	{
		m_MonsterHerdData.push_back(new CXMasMonsterHerd);
	}

	LeaveCriticalSection(&m_critsec);
}

void CXMasAttackEvent::AddMonsters(CXMasMonsterHerd* lpMonsterHerd)
{
	EnterCriticalSection(&m_critsec);

	for(std::vector<MONSTER_EVENT_DATA>::iterator _It = m_EventMonster.begin(); _It != m_EventMonster.end(); _It++)
	{
		MONSTER_EVENT_DATA* lpMonster = &(*_It);
		int iCount = 100;

		while( iCount-- != 0 )
		{
			if( lpMonsterHerd->AddMonster(lpMonster->m_iType,lpMonster->m_bDoRegen,lpMonster->m_bDoAttackFirst) )
			{
				break;
			}
		}
	}

	LeaveCriticalSection(&m_critsec);
}

void CXMasAttackEvent::RemoveMonsterHerd()
{
	EnterCriticalSection(&m_critsec);

	std::vector<CXMasMonsterHerd*>::iterator _It = m_MonsterHerdData.begin();

	while( _It != m_MonsterHerdData.end() )
	{
		CXMasMonsterHerd* lpMonster = *_It;
		m_MonsterHerdData.erase(_It);
		delete [] lpMonster;
	}

	LeaveCriticalSection(&m_critsec);
}

BOOL CXMasAttackEvent::CheckLocation(CXMasMonsterHerd* lpMonsterHerd,XMASMONSTER_MAP_DATA* MapInfo)
{
	int iCount = 1000;
	BYTE X;
	BYTE Y;
	BYTE cX;
	BYTE cY;

	if( lpMonsterHerd->GetCurrentLocation(cX,cY) == FALSE )
		return false;

	while( iCount -- )
	{
		X = (rand()%m_iMOVE_RAND_SIZE * (((rand()%3)-1)*-1)) + cX;
		Y = (rand()%m_iMOVE_RAND_SIZE * (((rand()%3)-1)*-1)) + cY;

		if( X < MapInfo->X1 || X > MapInfo->X2 )
			continue;

		if( Y < MapInfo->Y1 || Y > MapInfo->Y2 )
			continue;

		if( lpMonsterHerd->CheckLocation(X,Y) )
			break;
	}

	lpMonsterHerd->MoveHerd(X,Y);
	return true;
}

void CXMasAttackEvent::Run()
{
	if( m_bManualStart != FALSE )
		return;

	if( m_bDoEvent == FALSE )
		return;

	switch( m_nEventState )
	{
	case ATTACKEVENT_STATE_NONE:
		ProcState_NONE();
		break;
	case ATTACKEVENT_STATE_CLOSED:
		ProcState_CLOSED();
		break;
	case ATTACKEVENT_STATE_PLAYING:
		ProcState_PLAYING();
		break;
	}
}

void CXMasAttackEvent::CheckSync()
{
	if( m_EventTime.empty() )
	{
		LogAddC(2,"[XMasAttackEvent] No Schedule Time Data");
		SetState(ATTACKEVENT_STATE_NONE);
		return;
	}

	BOOL bTIME_CHANGED = FALSE;
	int iMIN_HOUR = 24;
	int iMIN_MINUTE = 60;

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	std::vector<MONSTER_EVENT_TIME>::iterator it = this->m_EventTime.begin();
	
	for ( ; it != this->m_EventTime.end() ; it++ )
	{
		MONSTER_EVENT_TIME & pRET = *it;

		if ( (sysTime.wHour * 60 + sysTime.wMinute) < (pRET.m_iHour * 60 + pRET.m_iMinute ))
		{
			if ( (iMIN_HOUR * 60 + iMIN_MINUTE) > (pRET.m_iHour * 60 + pRET.m_iMinute) )
			{
				bTIME_CHANGED = TRUE;
				iMIN_HOUR = pRET.m_iHour;
				iMIN_MINUTE = pRET.m_iMinute;
			}
		}
	}

	if ( bTIME_CHANGED == FALSE )
	{
		iMIN_HOUR = 24;
		iMIN_MINUTE = 60;
		it = this->m_EventTime.begin();

		for ( ; it != this->m_EventTime.end() ; it++ )
		{
			MONSTER_EVENT_TIME & pRET = *it;

			if ( (iMIN_HOUR * 60 + iMIN_MINUTE) > (pRET.m_iHour * 60 + pRET.m_iMinute) )
			{
				bTIME_CHANGED = 2;
				iMIN_HOUR = pRET.m_iHour;
				iMIN_MINUTE = pRET.m_iMinute;
			}
		}
	}

	switch( bTIME_CHANGED )
	{
	case 1:
		{
			int bTmpSec = (iMIN_HOUR * 60 * 60 + iMIN_MINUTE * 60) - (sysTime.wHour * 60 * 60 + sysTime.wMinute * 60 + sysTime.wSecond);
			this->m_TimeCounter.SetSeconds(bTmpSec);
		}
		break;
	case 2:
		{
			int bTmpSec = ((24+iMIN_HOUR) * 60 * 60 + iMIN_MINUTE * 60) - (sysTime.wHour * 60 * 60 + sysTime.wMinute * 60 + sysTime.wSecond);
			this->m_TimeCounter.SetSeconds(bTmpSec);
		}
		break;
	default:
		LogAddC(2,"[XMasAttackEvent] No Schedule Time Data");
		SetState(ATTACKEVENT_STATE_NONE);
		return;
	}

	LogAddTD("[XMasAttackEvent] Sync Start Time. [%d] min remain (START HOUR:%d, MIN:%d)",
		m_TimeCounter.GetMinutes(),iMIN_HOUR,iMIN_MINUTE);

	m_iLEFT_MIN = m_iTIME_OPEN;
}








