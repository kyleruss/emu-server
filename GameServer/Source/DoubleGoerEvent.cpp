#include "stdafx.h"
#include "DoubleGoerEvent.h"

#include "..\include\readscript.h"
#include "logproc.h"
#include "GameMain.h"
#include "gObjMonster.h"
#include "TMonsterAIUtil.h"
#include "TMonsterAIElement.h"
#include "BuffEffectSlot.h"
#include "MonsterItemMng.h"
#include "Event.h"
#include "protocol.h"
#include "BloodCastle.h"
#include "MonsterStatCalc.h"
#include "ShopPointEx.h"

// CONSTRUCTOR / DESTRUCTOR

int g_DoubleGoerEvent_Gates[DOUBLEGOER_MAXMAP] = {329,330,331,332};
struct DOUBLEGOER_DEFPOINTS
{
	BYTE stX;
	BYTE stY;
	BYTE ptX;
	BYTE ptY;

	float distDiv;
	BYTE EndDist;
} g_DoubleGoerEvent_DefPts[DOUBLEGOER_MAXMAP] = {
	// Ice Map
	197,27,224,101,3.545f,78,
	// Lava Map
	137,67,114,180,5.045f,111,
	// Atlans Map
	110,56,110,150,4.0909f,90,
	// Kantru Map
	95,11,44,108,4.818f,106,
};

struct DOUBLEGOER_BLOCK
{
	BYTE stX;
	BYTE stY;
	BYTE ptX;
	BYTE ptY;
} g_DoubleGoerEvent_BlockPts[DOUBLEGOER_MAXMAP] = {
	// Ice Map
	191,33,200,36,
	// Lava Map
	134,76,141,79,
	// Atlans Map
	101,65,113,68,
	// Kantru Map
	92,23,98,26
};

CDoubleGoerEvent gDoubleGoer;

CDoubleGoerEvent::CDoubleGoerEvent()
{
	Init();
	InitializeCriticalSection(&m_sCrit);
}

CDoubleGoerEvent::~CDoubleGoerEvent()
{

	if( !m_vtReward.empty() )
	{
		vtReward::iterator _It = m_vtReward.begin();

		for( ; _It != m_vtReward.end(); _It++)
		{
			if( _It->m_SilverBox != NULL )
				delete _It->m_SilverBox;

			if( _It->m_GoldenBox != NULL )
				delete _It->m_GoldenBox;
		}
	}

	DeleteCriticalSection(&m_sCrit);
}

// MEMBER FUNCTIONS

void CDoubleGoerEvent::Init()
{
	int i;
	for(i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
	{
		m_PlayerInfo[i].aIndex = -1;
	}

	m_iLeaderIndex = -1;
	m_iPlayerCount = 0;

	for(i = 0; i < DOUBLEGOER_MAXMONSTER; i++)
	{
		m_MonsterInfo[i] = -1;
	}

	for(i = 0; i < DOUBLEGOER_MAXBOX; i++)
	{
		m_BoxInfo[i] = -1;
	}

	m_iMonsterPassed = 0;
	m_bSEND_PLAY_START = FALSE;
	m_bMSG_BEFORE_START = FALSE;

	m_bSUMMON_1ST_BOSS = FALSE;
	m_bSUMMON_2ND_BOSS = FALSE;
	m_bSUMMON_3RD_BOSS = FALSE;

	m_iBossIndex = -1;

	m_iWaveDist = 0;
	m_iCurentMap = -1;

	m_iMONSTER_MINUTE_SPAWN = m_iEVENT_TIME;
}

void CDoubleGoerEvent::LoadData(char* lpszFileName)
{
	SMDFile = fopen(lpszFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[Doppelganger] Load File failed(%s)",lpszFileName);
		return;
	}

	m_iLoadedInfo = 0;

	if( !m_vtReward.empty() )
	{
		vtReward::iterator _It = m_vtReward.begin();

		for( ; _It != m_vtReward.end(); _It++)
		{
			if( _It->m_SilverBox != NULL )
				delete _It->m_SilverBox;

			if( _It->m_GoldenBox != NULL )
				delete _It->m_GoldenBox;
		}
	}

	m_vtReward.clear();

	try
	{
		int Token,Index;

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

					m_iEVENT_WAIT = (int)TokenNumber;

					Token = GetToken();
					m_iEVENT_TIME = (int)TokenNumber;

					Token = GetToken();
					m_iEVENT_PLAYEND = (int)TokenNumber;

					m_iMONSTER_MINUTE_SPAWN = m_iEVENT_TIME;
				}
				else if( Index == 1 )
				{
					Token = GetToken();

					if( strcmp("end",TokenString) == NULL )
						break;

					m_iMONSTER_COUNT = (int)TokenNumber;

					Token = GetToken();
					m_iMONSTER_PER_MINUTE = (int)TokenNumber;
				}
				else if( Index == 2 )
				{
					Token = GetToken();

					if( strcmp("end",TokenString) == NULL )
						break;

					_st_reward pTmp;

					pTmp.m_ResetMin = TokenNumber;

					Token = GetToken();
					pTmp.m_ResetMax = TokenNumber;

					Token = GetToken();
					pTmp.m_LevelMin = TokenNumber;

					Token = GetToken();
					pTmp.m_LevelMax = TokenNumber;

					Token = GetToken();
					pTmp.m_SilverBox = new EventItemBag;
					if( pTmp.m_SilverBox != NULL )
					{
						char Path[MAX_PATH] = { 0 };
						sprintf(Path, "%s%s", PATH_EVENTBAG_DOPPELGANGER, TokenString);
						pTmp.m_SilverBox->Read(gDirPath.GetNewPath(Path));
					}
					else
					{
						MsgBox("[Doppelganger] Failed to init silver box!");
					}


					Token = GetToken();
					pTmp.m_GoldenBox = new EventItemBag;
					if( pTmp.m_GoldenBox != NULL )
					{
						char Path[MAX_PATH] = { 0 };
						sprintf(Path, "%s%s", PATH_EVENTBAG_DOPPELGANGER, TokenString);
						pTmp.m_GoldenBox->Read(gDirPath.GetNewPath(Path));
					}
					else
					{
						MsgBox("[Doppelganger] Failed to init golden box!");
					}

					m_vtReward.push_back(pTmp);
				}
			}
		}

	}
	catch( DWORD )
	{
		MsgBox("[Doppelganger] - Loading Exception Error (%s) File. ", lpszFileName);
	}

	LogAddTD("[Doppelganger] LevelInfo loaded [%d] levels",m_iLoadedInfo);

	bLoaded = TRUE;
	bEnabled = GetPrivateProfileInt("GameServerInfo","DoubleGoerEventRun",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_iMirrorDropRate = GetPrivateProfileInt("GameServerInfo","MirrorMarkDropRate",100, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	LogAddTD("[Doppelganger] File loaded! (%s)",lpszFileName);
}

void CDoubleGoerEvent::EnterRequest(PMSG_DOUBLEGOER_REQ_ENTER* lpMsg,int aIndex)
{
	PMSG_DOUBLEGOER_ANS_ENTER pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,14,sizeof(pMsg));
	pMsg.btResult = 1;

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use == 0 || lpObj->m_IfState.type != 21 )
	{
		LogAddTD("[Doppelganger][ERROR] Not talked with Lugard [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	int invPos = lpMsg->btPos+INVENTORY_BAG_START;

	if( !MAIN_INVENTORY_RANGE(invPos) )
	{
		LogAddTD("[Doppelganger][ERROR] Inventory range error - [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpObj->pInventory[invPos].IsItem() == FALSE || 
		lpObj->pInventory[invPos].m_Type != ITEMGET(14,111) )
	{
		LogAddTD("[Doppelganger][ERROR] Inventory item error - [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	EnterCriticalSection(&m_sCrit);

	if( m_iState != DOUBLEGOER_STATE_CLOSED )
	{
		int bFail = TRUE;

		if( m_iState == DOUBLEGOER_STATE_WAITING )
		{
			bFail = FALSE;
			int PartyNumber = lpObj->PartyNumber;

			if( PartyNumber == -1 || m_iLeaderIndex == -1 || gObj[m_iLeaderIndex].PartyNumber != PartyNumber )
			{
				bFail = TRUE;
			}
		}

		if( bFail == TRUE )
		{
			LogAddTD("[Doppelganger][%d](%s)(%s) Enter request failed - event on going",aIndex,lpObj->AccountID,lpObj->Name);
			pMsg.btResult = 2;
			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
			LeaveCriticalSection(&m_sCrit);
			return;
		}
	}


	int bFail = FALSE;

#if (ENABLE_FIX_PARTYPK == 1)

	if( lpObj->m_PK_Level >= 4 )
	{
		bFail = TRUE;
	}

#else
	if ( lpObj->PartyNumber >= 0 )
	{
		if( gParty.GetPartyPkLevel(lpObj->PartyNumber) >= 5 )
		{
			bFail = TRUE;
		}
	}
	else if( lpObj->m_PK_Level >= 4 )
	{
		bFail = TRUE;
	}
#endif

	if( bFail == TRUE )
	{
		LogAddTD("[Doppelganger][%d](%s)(%s) Enter request failed - pk status",aIndex,lpObj->AccountID,lpObj->Name);
		pMsg.btResult = 3;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		LeaveCriticalSection(&m_sCrit);
		return;
	}

	lpObj->m_IfState.use = 0;
	lpObj->m_IfState.type = 0;

	gObjInventoryItemSet(aIndex, invPos, -1);
	gObj[aIndex].pInventory[invPos].Clear();
	GCInventoryItemDeleteSend(aIndex, invPos, 1);

	if( m_iLeaderIndex == -1 )
	{
		m_iLeaderIndex = lpObj->m_Index;
		SetState(DOUBLEGOER_STATE_WAITING);
	}

	AddUser(lpObj->m_Index);
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
	LeaveCriticalSection(&m_sCrit);
}

void CDoubleGoerEvent::CheckUser()
{
	for(int i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
	{
		int aIndex = m_PlayerInfo[i].aIndex;

		if( OBJMAX_RANGE(aIndex) )
		{
			if( !gObjIsConnected(aIndex) )
			{
				DelUser(aIndex);
				LogAddTD("[Doppelganger] [USER REMOVE] User not connected [%d](%s)(%s)",aIndex,
					gObj[aIndex].AccountID,gObj[aIndex].Name);
			}
			else if( gObj[aIndex].MapNumber != m_iCurentMap )
			{
				if( m_iState == DOUBLEGOER_STATE_PLAYING )
				{
					PMSG_DOUBLEGOER_EVENTEND pMsg;
					PHeadSubSetB((LPBYTE)&pMsg,0xBF,19,sizeof(pMsg));
					pMsg.btResult = 1;

					DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
				}

				DelUser(aIndex);
				LogAddTD("[Doppelganger] [USER REMOVE] User not on map [%d](%s)(%s) CurMap:[%d]-UserMap:[%d]",aIndex,
					gObj[aIndex].AccountID,gObj[aIndex].Name,m_iCurentMap,gObj[aIndex].MapNumber);
			}
		}
	}

	if( !OBJMAX_RANGE(m_iLeaderIndex) || !gObjIsConnected(m_iLeaderIndex) || gObj[m_iLeaderIndex].MapNumber != m_iCurentMap )
	{
		m_iLeaderIndex = -1;
	}
}

void CDoubleGoerEvent::Run()
{
	if( bEnabled == FALSE || bLoaded == FALSE || m_iState == DOUBLEGOER_STATE_CLOSED )
	{
		return;
	}

	CheckUser();

	switch( m_iState )
	{
	case DOUBLEGOER_STATE_WAITING:
		ProcState_WAITING();
		break;
	case DOUBLEGOER_STATE_PLAYING:
		ProcState_PLAYING();
		break;
	case DOUBLEGOER_STATE_PLAYEND:
		ProcState_PLAYEND();
		break;
	default:
		break;
	}
}

void CDoubleGoerEvent::ProcState_WAITING()
{
	DWORD dwTICK = GetTickCount() - m_dwTICK_COUNT;

	if( dwTICK >= 500 )
	{
		m_dwTICK_COUNT = GetTickCount();
		m_iREMAIN_TIME -= dwTICK;

		if( m_bMSG_BEFORE_START == 0 && m_iREMAIN_TIME <= 30000 )
		{
			m_bMSG_BEFORE_START = TRUE;

			PMSG_SET_DEVILSQUARE pMsg;
			PHeadSetB((LPBYTE)&pMsg, 0x92, sizeof(pMsg));
			pMsg.Type = 17;

			SendData((LPBYTE)&pMsg,pMsg.h.size);
		}
	}

	if( m_iREMAIN_TIME <= 0 )
	{
		SetState(DOUBLEGOER_STATE_PLAYING);
	}
	else if( m_iPlayerCount <= 0 || m_iLeaderIndex == -1 )
	{
		SetState(DOUBLEGOER_STATE_CLOSED);
	}
}

void CDoubleGoerEvent::ProcState_PLAYING()
{
	DWORD dwTICK = GetTickCount() - m_dwTICK_COUNT;

	if( dwTICK >= 500 )
	{
		m_dwTICK_COUNT = GetTickCount();
		m_iREMAIN_TIME -= dwTICK;

		if( m_bSEND_PLAY_START == FALSE )
		{
			m_iREMAIN_TIME = m_iEVENT_TIME*60*1000;

			SummonMonster();
			SendStart();
			SendMonsterCount();

			for(int i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
			{
				if( OBJMAX_RANGE(m_PlayerInfo[i].aIndex) )
				{
					SendBlockInfo(m_PlayerInfo[i].aIndex,m_iCurentMap - MAP_INDEX_DOUBLEGOER1,1);
				}
			}
		}

		if( m_bSEND_PLAY_START == TRUE )
		{
			CalcDistance();
			SendPlayInfo();
			SendWaveInfo();
			SendBossInfo(m_iBossIndex);

			if( m_bSUMMON_1ST_BOSS == FALSE && m_iREMAIN_TIME <= ((m_iEVENT_TIME*60*1000)-60000) )
			{
				SummonMonster(529,m_iCurentMap,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptX,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptY,m_EventLevel, m_EventReset);
				m_bSUMMON_1ST_BOSS = TRUE;
			}
			else if( m_bSUMMON_2ND_BOSS == FALSE && m_iREMAIN_TIME <= ((m_iEVENT_TIME*60*1000)-3*60000) )
			{
				SummonMonster(530,m_iCurentMap,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptX,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptY,m_EventLevel, m_EventReset);
				m_bSUMMON_2ND_BOSS = TRUE;
			}
			else if( m_bSUMMON_3RD_BOSS == FALSE && m_iREMAIN_TIME <= ((m_iEVENT_TIME*60*1000)-6*60000) )
			{
				m_iBossIndex = SummonMonster(531,m_iCurentMap,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptX,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptY,m_EventLevel, m_EventReset);
				m_bSUMMON_3RD_BOSS = TRUE;
			}

			if( m_iMONSTER_MINUTE_SPAWN != (m_iREMAIN_TIME/60000) )
			{
				m_iMONSTER_MINUTE_SPAWN = (m_iREMAIN_TIME/60000);

				for(int i = 0; i < m_iMONSTER_PER_MINUTE; i++)
				{
					SummonMonster(533+rand()%7,m_iCurentMap,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptX,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptY,m_EventLevel, m_EventReset);
				}
			}

			if( m_bMSG_BEFORE_TIMEOUT == FALSE && m_iREMAIN_TIME <= 30000 )
			{
				m_bMSG_BEFORE_TIMEOUT = TRUE;

				PMSG_SET_DEVILSQUARE pMsg;
				PHeadSetB((LPBYTE)&pMsg, 0x92, sizeof(pMsg));
				pMsg.Type = 18;

				SendData((LPBYTE)&pMsg,pMsg.h.size);
			}
		}
	}

	if( m_iREMAIN_TIME <= 0 || m_iMonsterPassed >= 3 )
	{
		SetState(DOUBLEGOER_STATE_PLAYEND);
	}
	else if( m_iPlayerCount <= 0 || m_iLeaderIndex == -1 )
	{
		SetState(DOUBLEGOER_STATE_CLOSED);
	}
}

void CDoubleGoerEvent::ProcState_PLAYEND()
{
	DWORD dwTICK = GetTickCount() - m_dwTICK_COUNT;

	if( dwTICK >= 1000 )
	{
		m_dwTICK_COUNT = GetTickCount();
		m_iREMAIN_TIME -= dwTICK;

		if( m_bMSG_BEFORE_END == FALSE && m_iREMAIN_TIME <= 30000 )
		{
			m_bMSG_BEFORE_END = TRUE;

			PMSG_SET_DEVILSQUARE pMsg;
			PHeadSetB((LPBYTE)&pMsg, 0x92, sizeof(pMsg));
			pMsg.Type = 19;

			SendData((LPBYTE)&pMsg,pMsg.h.size);
		}
	}

	if( m_iREMAIN_TIME <= 0 )
	{
		SetState(DOUBLEGOER_STATE_CLOSED);
	}
}

void CDoubleGoerEvent::SetState(int iState)
{
	switch( iState )
	{
	case DOUBLEGOER_STATE_CLOSED:
		SetState_CLOSED();
		break;
	case DOUBLEGOER_STATE_WAITING:
		SetState_WAITING();
		break;
	case DOUBLEGOER_STATE_PLAYING:
		SetState_PLAYING();
		break;
	case DOUBLEGOER_STATE_PLAYEND:
		SetState_PLAYEND();
		break;
	default:
		break;
	}

	m_iState = iState;
}

void CDoubleGoerEvent::SetState_CLOSED()
{
	for(int i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
	{
		if( m_PlayerInfo[i].aIndex != -1 )
		{
			gObjMoveGate(m_PlayerInfo[i].aIndex,17);
		}
	}

	RemoveMonster();
	Init();
}

void CDoubleGoerEvent::SetState_WAITING()
{
	LogAddTD("[Doppelganger] SetState_WAITING()");
	m_iREMAIN_TIME = m_iEVENT_WAIT*60*1000;
	m_dwTICK_COUNT = GetTickCount();

	m_iCurentMap = MAP_INDEX_DOUBLEGOER1 + rand()%DOUBLEGOER_MAXMAP;
}

void CDoubleGoerEvent::SetState_PLAYING()
{
	LogAddTD("[Doppelganger] SetState_PLAYING()");
	m_iREMAIN_TIME = m_iEVENT_TIME*60*1000;
	m_dwTICK_COUNT = GetTickCount();
}

void CDoubleGoerEvent::SetState_PLAYEND()
{
	LogAddTD("[Doppelganger] SetState_PLAYEND()");
	m_iREMAIN_TIME = m_iEVENT_PLAYEND*60*1000;
	m_dwTICK_COUNT = GetTickCount();

	RemoveMonster();

	PMSG_DOUBLEGOER_EVENTEND pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,19,sizeof(pMsg));

	if(  m_iMonsterPassed < 3 && m_iPlayerCount > 0  && m_iLeaderIndex != -1 )
	{
#ifdef POINTEX
		for(int i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
		{
			int aIndex = m_PlayerInfo[i].aIndex;
			if(  OBJMAX_RANGE(aIndex) )
			{
				g_ShopPointEx.AddEventBonus(aIndex, ShopPointExEvent::DG);
			}
		}
#endif
		

		SummonMonster(542,m_iCurentMap,gObj[m_iLeaderIndex].X+(-1+rand()%3),gObj[m_iLeaderIndex].Y+(-1+rand()%3),m_EventLevel, m_EventReset);
		pMsg.btResult = 0;

		LogAddTD("[Doppelganger] Event - Success -> MonsterPassed: %d, PlayerCount: %d, LeaderIndex: %d",
			m_iMonsterPassed,m_iPlayerCount,m_iLeaderIndex);
	}
	else
	{
		pMsg.btResult = 2;
		LogAddTD("[Doppelganger] Event - Fail -> MonsterPassed: %d, PlayerCount: %d, LeaderIndex: %d",
			m_iMonsterPassed,m_iPlayerCount,m_iLeaderIndex);
	}

	SendData((LPBYTE)&pMsg,pMsg.h.size);
}

BYTE CDoubleGoerEvent::GetOpenTime()
{
	if( m_iState == DOUBLEGOER_STATE_PLAYING )
	{
		return m_iREMAIN_TIME/60000 + m_iEVENT_PLAYEND+1;
	}
	else if( m_iState == DOUBLEGOER_STATE_PLAYEND )
	{
		return m_iREMAIN_TIME/60000+1;
	}

return 0;
}

BOOL CDoubleGoerEvent::AddUser(int aIndex)
{
	for(int i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
	{
		if( m_PlayerInfo[i].aIndex == -1 )
		{
			int MapIndex = m_iCurentMap-MAP_INDEX_DOUBLEGOER1;
			gObjMoveGate(aIndex,g_DoubleGoerEvent_Gates[MapIndex]);

			SendBlockInfo(aIndex,MapIndex,0);

			m_PlayerInfo[i].aIndex = aIndex;
			m_iPlayerCount++;
			return true;
		}
	}
return false;
}

BOOL CDoubleGoerEvent::DelUser(int aIndex)
{
	for(int i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
	{
		if( m_PlayerInfo[i].aIndex == aIndex )
		{
			m_PlayerInfo[i].aIndex = -1;
			m_iPlayerCount--;
			return true;
		}
	}
return false;
}

BOOL CDoubleGoerEvent::AddMonster(int aIndex)
{
	for(int i = 0; i < DOUBLEGOER_MAXMONSTER; i++)
	{
		if( m_MonsterInfo[i] == -1 )
		{
			m_MonsterInfo[i] = aIndex;
			return true;
		}
	}
return false;
}

BOOL CDoubleGoerEvent::AddBox(int aIndex)
{
	for(int i = 0; i < DOUBLEGOER_MAXBOX; i++)
	{
		if( m_BoxInfo[i] == -1 )
		{
			m_BoxInfo[i] = aIndex;
			return true;
		}
	}
return false;
}

BOOL CDoubleGoerEvent::DelMonster(int aIndex)
{
	for(int i = 0; i < DOUBLEGOER_MAXMONSTER; i++)
	{
		if( m_MonsterInfo[i] == aIndex )
		{
			m_MonsterInfo[i] = -1;
			gObjDel(aIndex);
			return true;
		}
	}
return false;
}

BOOL CDoubleGoerEvent::DelBox(int aIndex)
{
	for(int i = 0; i < DOUBLEGOER_MAXBOX; i++)
	{
		if( m_BoxInfo[i] == aIndex )
		{
			m_BoxInfo[i] = -1;
			return true;
		}
	}
return false;
}

void CDoubleGoerEvent::SendData(LPBYTE pMsg,int size)
{
	for(int i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
	{
		int aIndex = m_PlayerInfo[i].aIndex;
		if(  OBJMAX_RANGE(aIndex) )
		{
			DataSend(aIndex,pMsg,size);
		}
	}
}

void CDoubleGoerEvent::SendStart()
{
	m_bSEND_PLAY_START = TRUE;
	m_iREMAIN_TIME = m_iEVENT_TIME*60*1000;

	PMSG_DOUBLEGOER_SET_START pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,16,sizeof(pMsg));
	pMsg.btResult = 2;

	for(int i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
	{
		int aIndex = m_PlayerInfo[i].aIndex;
		if( OBJMAX_RANGE(aIndex) )
		{
			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		}
	}
}

void CDoubleGoerEvent::SendPlayInfo()
{
	PMSG_DOUBLEGOER_PLAYINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,18,sizeof(pMsg));

	pMsg.EventTime = m_iREMAIN_TIME/1000;
	pMsg.btCount = 0;

	for(int i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
	{
		int aIndex = m_PlayerInfo[i].aIndex;

		if( OBJMAX_RANGE(aIndex) )
		{
			pMsg.m_Info[pMsg.btCount].aIndex = aIndex;
			pMsg.m_Info[pMsg.btCount].Pos = (BYTE)(m_PlayerInfo[i].Dist/g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].distDiv);
			pMsg.btCount++;
		}
	}

	SendData((LPBYTE)&pMsg,pMsg.h.size);
}

void CDoubleGoerEvent::SendMonsterCount()
{
	PMSG_DOUBLEGOER_MONSTERSET pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,20,sizeof(pMsg));

	pMsg.btMaxMon = 3;
	pMsg.btCurMon = m_iMonsterPassed;

	SendData((LPBYTE)&pMsg,pMsg.h.size);
}

void CDoubleGoerEvent::SendWaveInfo()
{
	PMSG_DOUBLEGOER_WAVEINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,15,sizeof(pMsg));

	pMsg.btDist = m_iWaveDist;

	SendData((LPBYTE)&pMsg,pMsg.h.size);
}

void CDoubleGoerEvent::SendBossInfo(int iBossIndex)
{
	if( !OBJMAX_RANGE(iBossIndex) )
		return;

	PMSG_DOUBLEGOER_BOSSINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,17,sizeof(pMsg));

	if( !gObj[iBossIndex].Live )
		pMsg.btResult = 1;

	pMsg.btResult = 0;
	pMsg.btPos = (BYTE)(CalcDistance(gObj[iBossIndex].X,gObj[iBossIndex].Y,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptX,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptY)/g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].distDiv);

	SendData((LPBYTE)&pMsg,pMsg.h.size);
}

int CDoubleGoerEvent::CalcDistance(int x1,int y1, int x2, int y2)
{
	float tx = (float)(x1 - x2);
	float ty = (float)(y1 - y2);

return (int)sqrt( (tx*tx)+(ty*ty) );
}

void CDoubleGoerEvent::CalcDistance()
{
	int i;
	for(i = 0; i < DOUBLEGOER_MAXPLAYER; i++)
	{
		int aIndex = m_PlayerInfo[i].aIndex;

		if( OBJMAX_RANGE(aIndex) )
		{
			m_PlayerInfo[i].Dist = CalcDistance(gObj[aIndex].X,gObj[aIndex].Y,
				g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptX,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptY);
		}
	}

	m_iWaveDist = 0;

	for(i = 0;i < DOUBLEGOER_MAXMONSTER; i++)
	{
		if( OBJMAX_RANGE(m_MonsterInfo[i]) )
		{
			LPOBJ monObj = &gObj[m_MonsterInfo[i]];

			int Dist = CalcDistance(monObj->X,monObj->Y,g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptX,
				g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].ptY);

			if( (Dist+1) >= g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].EndDist )
			{
				GCStateInfoSend(monObj,1,105);

				if( monObj->Class == 529 ||
					monObj->Class == 530 ||
					monObj->Class == 531 )
				{
					m_iMonsterPassed = 3;
				}
				else
				{
					m_iMonsterPassed++;
				}

				DelMonster(monObj->m_Index);
				SendMonsterCount();
				continue;
			}

			if( (Dist/g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].distDiv) > m_iWaveDist )
			{
				m_iWaveDist = (int)(Dist/g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].distDiv);
			}
		}
	}
}

void CDoubleGoerEvent::SummonMonster()
{
	int result = -1;

	if( !OBJMAX_RANGE(m_iLeaderIndex) )
	{
		return;
	}

	LPOBJ lpObj = &gObj[m_iLeaderIndex];
	m_iPartyCount = 1;

	int MaxLevel = 0;
	int MaxReset = 0;

	if( lpObj->PartyNumber >= 0 )
	{
		int PartyNum = lpObj->PartyNumber;
		m_iPartyCount = 0;

		for(int i = 0; i < MAX_USER_IN_PARTY; i++)
		{
			int number = gParty.m_PartyS[PartyNum].Number[i];

			if( number >= 0 )
			{
				if( gObj[number].Level > MaxLevel )
				{
					MaxLevel = gObj[number].Level + gObj[number].MasterLevel;
				}

				if( gObj[number].iResetCount > MaxReset )
				{
					MaxReset = gObj[number].iResetCount;
				}
				m_iPartyCount++;
			}
		}
	}
	else
	{
		MaxLevel = lpObj->Level + lpObj->MasterLevel;
		MaxReset = lpObj->iResetCount;
	}

	m_EventLevel = MaxLevel;
	m_EventReset = MaxReset;

	int mapIndex = m_iCurentMap-MAP_INDEX_DOUBLEGOER1;

	for(int i = 0; i < m_iMONSTER_COUNT; i++)
	{

		int sx = g_DoubleGoerEvent_DefPts[mapIndex].ptX;
		int sy = g_DoubleGoerEvent_DefPts[mapIndex].ptY;

		SummonMonster(533+rand()%7,m_iCurentMap,sx,sy,m_EventLevel, m_EventReset);
	}
}

int CDoubleGoerEvent::SummonMonster(int Id,int map,int x,int y,int MaxLevel, int MaxReset)
{
	int result = gObjAddMonster(map);

	if ( result >= 0 )
	{
		LPOBJ lpObj = &gObj[result];

		lpObj->m_PosNum = -1;
		lpObj->X = x;
		lpObj->Y = y;
		lpObj->MapNumber = map;
		lpObj->TX = x;
		lpObj->TY = y;
		lpObj->m_OldX = x;
		lpObj->m_OldY = y;
		lpObj->StartX = x;
		lpObj->StartY = y;

		//if( gObjSetMonster(result, Id) == FALSE )

		if( !this->SetMonster(result, Id, MaxLevel, MaxReset) )
		{
			gObjDel(result);
			LogAddTD("[Doppelganger] Failed to set monster info ID: %d",Id);
			return false;
		}

		if( Id == 541 || Id == 542 )
		{
			if( AddBox(result) == FALSE )
			{
				gObjDel(result);
				LogAddTD("[Doppelganger] Failed to summon silver box ID: %d",Id);
				return false;
			}

			gObj[result].Type = OBJ_NPC;

			LogAddTD("[Doppelganger] SummonMonster [%d](%s) Map: %d, X: %d, Y: %d",
				lpObj->Class,lpObj->Name,lpObj->MapNumber,lpObj->X,lpObj->Y);
		}
		else if( AddMonster(result) == FALSE )
		{
			gObjDel(result);
			LogAddTD("[Doppelganger] Failed to summon monster ID: %d",Id);
			return false;
		}

		LogAddTD("[Doppelganger] SummonMonster [%d](%s) Map: %d, X: %d, Y: %d",
			lpObj->Class,lpObj->Name,lpObj->MapNumber,lpObj->X,lpObj->Y);
	}
return result;
}

BOOL CDoubleGoerEvent::SetMonster(int nIndex, int MonsterClass, int nMaxLevel, int nMaxReset)
{
	if( !OBJMAX_RANGE(nIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return false;
	}

	LPOBJ lpObj = &gObj[nIndex];
	lpObj->ConnectCheckTime = GetTickCount();
	lpObj->ShopNumber = -1;
	lpObj->TargetNumber = -1;
	lpObj->m_RecallMon = -1;
	lpObj->Connected = PLAYER_PLAYING;
	lpObj->Live = TRUE;
	lpObj->m_State = 1;
	lpObj->DieRegen = 0;
	lpObj->Type = OBJ_MONSTER;
	lpObj->Class = MonsterClass;
	lpObj->LastAttackerID = -1;

	g_MonsterStatCalc.ConvertMonsterStat(lpObj, nMaxLevel, nMaxReset);

	*lpObj->pInventoryCount = 0;

	if( lpObj->m_AttackType )
	{
		gObjMonsterMagicAdd(lpObj,lpObj->m_AttackType,1);
	}

	gObjMonsterHitDamageInit(lpObj);
	gObjSetInventory1Pointer(lpObj);
	CreateFrustrum(lpObj->X,lpObj->Y,nIndex);

	MapC[lpObj->MapNumber].SetStandAttr(lpObj->X,lpObj->Y);

	lpObj->m_OldX = lpObj->X;
	lpObj->m_OldY = lpObj->Y;
	return true;
}

void CDoubleGoerEvent::RemoveMonster()
{
	/*for(int i = 0; i < OBJ_MAXMONSTER; i++)
	{
		if( DG_MAP_RANGE(gObj[i].MapNumber) )
		{
			gObjDel(i);
		}
	}*/

	int i;
	for(i = 0; i < DOUBLEGOER_MAXMONSTER; i++)
	{
		if( m_MonsterInfo[i] != -1 )
		{
			gObjDel(m_MonsterInfo[i]);
			m_MonsterInfo[i] = -1;
		}
	}

	for(i = 0; i < DOUBLEGOER_MAXBOX; i++)
	{
		if( m_BoxInfo[i] != -1 )
		{
			gObjDel(m_BoxInfo[i]);
			m_BoxInfo[i] = -1;
		}
	}
}

void CDoubleGoerEvent::gObjMonsterMove(LPOBJ lpObj)
{
	if( gObjCheckUsedBuffEffect(lpObj,BUFF_ICEARROW) == TRUE ||
		gObjCheckUsedBuffEffect(lpObj,BUFF_STUN) == TRUE || 
		gObjCheckUsedBuffEffect(lpObj,BUFF_SLEEP) == TRUE ||
		gObjCheckUsedBuffEffect(lpObj,BUFF_FREEZE) == TRUE ||
		gObjCheckUsedBuffEffect(lpObj,BUFF_EARTHBINDS) == TRUE )
	{
		return;
	}

	int maxmoverange = lpObj->m_MoveRange*2+1;
	int searchc=10;
	BYTE tpx;
	BYTE tpy;

	BOOL bFindXY = FALSE;
	int iTargetX = g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].stX;
	int iTargetY = g_DoubleGoerEvent_DefPts[m_iCurentMap-MAP_INDEX_DOUBLEGOER1].stY;

	int iTargetDistance = (int)(sqrt( float((lpObj->X - iTargetX)*(lpObj->X - iTargetX)+(lpObj->Y - iTargetY)*(lpObj->Y - iTargetY))));
	
	if ( TMonsterAIElement::s_MonsterAIMovePath[lpObj->MapNumber].m_bDataLoad )
	{
		if ( iTargetDistance > 4 )
		{
			int iMinCost = 1000000;
			int iMidX = -1;
			int iMidY = -1;
			int iSpotNum = -1;

			for ( int i=0;i<MAX_MONSTER_AI_MOVE_PATH;i++)
			{
				TMonsterAIMovePathInfo & PathInfo = TMonsterAIElement::s_MonsterAIMovePath[lpObj->MapNumber].m_MovePathInfo[i];
				float fDistX = (float)(lpObj->X - PathInfo.m_iPathX);
				float fDistY = (float)(lpObj->Y - PathInfo.m_iPathY);
				int iPathSpotDist =  (int)sqrt( (fDistX*fDistX) + (fDistY*fDistY) );

				if ( iPathSpotDist < 5 )
				{
					fDistX = (float)(iTargetX - PathInfo.m_iPathX);
					fDistY = (float)(iTargetY - PathInfo.m_iPathY);
					int iMidDist = (int)sqrt( (fDistX*fDistX) + (fDistY*fDistY) );

					if ( iMinCost > iMidDist )
					{
						if ( iMidDist )
						{
							iMinCost = iMidDist;
							iMidX = PathInfo.m_iPathX;
							iMidY = PathInfo.m_iPathY;
							iSpotNum = i;
						}
					}
				}
			}

			if ( iMinCost != 1000000 )
			{
				iTargetX = iMidX;
				iTargetY = iMidY;
				bFindXY = TRUE;
			}
		}
	}

	while ( searchc-- != 0 )
	{
		__try
		{
			if( bFindXY == FALSE )
			{
				tpx = (lpObj->X - lpObj->m_MoveRange) + (rand()%maxmoverange);
				tpy = (lpObj->Y - lpObj->m_MoveRange) + (rand()%maxmoverange);
			}
			else
			{
				tpx = iTargetX;
				tpy = iTargetY;
			}
		}
		__except(maxmoverange=1, 1)
		{

		}


		lpObj->TargetNumber = -1;
		lpObj->m_ActState.Attack = 0;
		lpObj->NextActionTime = 1500;
		lpObj->m_ActState.Emotion = 0;
		lpObj->MTX = tpx;
		lpObj->MTY = tpy;
		lpObj->m_ActState.Move = 1;
	}
}


void CDoubleGoerEvent::MonsterDieHandle(LPOBJ lpObj)
{
	if( lpObj->Class == 529 || lpObj->Class == 530 )
	{
		SummonMonster(541,m_iCurentMap,lpObj->X-1,lpObj->Y+(-1+rand()%3),m_EventLevel, m_EventReset);
		SummonMonster(541,m_iCurentMap,lpObj->X,lpObj->Y,m_EventLevel, m_EventReset);
		SummonMonster(541,m_iCurentMap,lpObj->X+(-1+rand()%3),lpObj->Y+1,m_EventLevel, m_EventReset);
	}
	else if( lpObj->Class == 531 )
	{
		SetState(DOUBLEGOER_STATE_PLAYEND);
		LogAddTD("[Doppelganger] Boss Killed -> EVENT_END");
	}

	DelMonster(lpObj->m_Index);
}

void CDoubleGoerEvent::CompensationBox(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( !lpNpc->Live )
		return;

	int bReward = FALSE;

	EventItemBag* pReward = GetRewardBox(0, lpObj);

	if( pReward != NULL )
	{
		if( rand()%3 == 0 )
		{
			for(int i = 0; i < 3; i++)
			{
				BYTE cDropX = lpNpc->X;
				BYTE cDropY = lpNpc->Y;

				if ( !gObjGetRandomItemDropLocation(lpNpc->MapNumber, cDropX, cDropY, 3, 3, 10))
				{
					cDropX = lpNpc->X;
					cDropY = lpNpc->Y;
				}

				pReward->Open(lpObj->m_Index,lpObj->MapNumber,cDropX,cDropY);
			}
			bReward = TRUE;
		}
	}

	for(int i = 0; i < DOUBLEGOER_MAXBOX; i++)
	{
		int aIndex = m_BoxInfo[i];

		if( OBJMAX_RANGE(aIndex) )
		{
			if( gObj[aIndex].Class == 541 )
			{
				gObj[aIndex].Life = 0.0f;
				gObjLifeCheck(&gObj[aIndex],lpObj,0,1,0,0,0,0);
				DelBox(aIndex);

				if( bReward == FALSE )
				{
					SummonMonster(532,m_iCurentMap,gObj[aIndex].X,gObj[aIndex].Y,m_EventLevel, m_EventReset);
				}
			}
		}
	}
}

void CDoubleGoerEvent::GoldCompensationBox(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( !lpNpc->Live )
		return;

	int i;

	EventItemBag* pReward = GetRewardBox(1, lpObj);

	if( pReward != NULL )
	{
		for(i = 0; i < 10; i++)
		{
			BYTE cDropX = lpNpc->X;
			BYTE cDropY = lpNpc->Y;

			if ( !gObjGetRandomItemDropLocation(lpNpc->MapNumber, cDropX, cDropY, 3, 3, 10))
			{
				cDropX = lpNpc->X;
				cDropY = lpNpc->Y;
			}

			pReward->Open(lpObj->m_Index,lpObj->MapNumber,cDropX,cDropY);
		}
	}

	for(i = 0; i < DOUBLEGOER_MAXBOX; i++)
	{
		int aIndex = m_BoxInfo[i];

		if( OBJMAX_RANGE(aIndex) )
		{
			if( gObj[aIndex].Class == 542 )
			{
				gObj[aIndex].Life = 0.0f;
				gObjLifeCheck(&gObj[aIndex],lpObj,0,1,0,0,0,0);
				DelBox(aIndex);
			}
		}
	}
}

EventItemBag* CDoubleGoerEvent::GetRewardBox(int type, LPOBJ lpUser)
{
	if( this->m_vtReward.empty() )
		return NULL;

	vtReward::iterator _It = m_vtReward.begin();

	for( ; _It != m_vtReward.end(); _It++)
	{
		if( lpUser->Level >= _It->m_LevelMin &&
			lpUser->Level <= _It->m_LevelMax )
		{
			if( lpUser->iResetCount >= _It->m_ResetMin &&
				lpUser->iResetCount <= _It->m_ResetMax )
			{
				if( type == 0 )
					return _It->m_SilverBox;
				if( type == 1 )
					return _It->m_GoldenBox;

				break;
			}
		}
	}
	return NULL;
}

void CDoubleGoerEvent::SendBlockInfo(int iIndex,int MapIndex,int bType)
{
	if( MapIndex < 0 || MapIndex >= 4 )
		return;

	char cTEMP_BUF[256] = {0};
	PMSG_SETMAPATTR_COUNT * lpMsg = (PMSG_SETMAPATTR_COUNT *)cTEMP_BUF;

	PHeadSetB((LPBYTE)lpMsg, 0x46, sizeof(PMSG_SETMAPATTR_COUNT)+sizeof(PMSG_SETMAPATTR)*6);
	PMSG_SETMAPATTR * lpMsgBody = (PMSG_SETMAPATTR *)&cTEMP_BUF[7];

	lpMsg->btType = 0;
	lpMsg->btCount = 1;
	lpMsg->btMapAttr = 16;
	lpMsg->btMapSetType=bType;

	lpMsgBody[0].btX = g_DoubleGoerEvent_BlockPts[MapIndex].stX;
	lpMsgBody[0].btY = g_DoubleGoerEvent_BlockPts[MapIndex].stY;
	lpMsgBody[1].btX   = g_DoubleGoerEvent_BlockPts[MapIndex].ptX;
	lpMsgBody[1].btY   = g_DoubleGoerEvent_BlockPts[MapIndex].ptY;

	DataSend(iIndex,(LPBYTE)lpMsg,lpMsg->h.size);
}
