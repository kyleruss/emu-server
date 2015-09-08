#include "StdAfx.h"
#include "ImperialGuardian.h"
#include "GameMain.h"
#include "logproc.h"
#include "PartyClass.h"
#include "Event.h"
#include "BuffEffectSlot.h"
#include "MonsterStatCalc.h"
#include "gObjMonster.h"
//#include "MasterLevelSystem.h"
#include "MasterSkillSystem.h"
#include "BloodCastle.h"
#include "DSProtocol.h"
#include "..\common\winutil.h"
#include "ClassCalc.h"
#include "ShopPointEx.h"

int g_nMysteriousPaperDropRate = 0;

CImperialGuardian g_ImperialGuardian;

CImperialGuardian::CImperialGuardian()
{
	Init();
}

CImperialGuardian::~CImperialGuardian()
{
	DeleteCriticalSection(&this->m_CritSection);
}

void CImperialGuardian::Init()
{
	this->m_wDayOfWeek = -1;
	this->m_iMixNeedZen = 0;
	this->m_wCheatMode = 0;
	this->m_iDayOfWeek = -1;
	this->iUnk1 = 0;
	this->m_bScriptLoaded = 0;
	this->m_bEnabled = 0;

	InitializeCriticalSection(&this->m_CritSection);
}

void CImperialGuardian::InitEventDungeon()
{
	for(int i = 0; i < IMPERIAL_GUARDIAN_MAXZONE; i++)
	{
		InitZone(i);
	}
}

void CImperialGuardian::LoadScript(char const* szFileName)
{
	int nTemp = 0;

	nTemp = GetPrivateProfileInt("GameServerInfo","IGEventOn",0,szFileName);

	if( nTemp == 1 )
		this->m_bEnabled = true;
	else
		this->m_bEnabled = false;

	this->m_iLootTime = GetPrivateProfileInt("GameServerInfo","LootTime",20,szFileName);
	this->m_iWaitPlayerTime = GetPrivateProfileInt("GameServerInfo","WaitPlayerTime",20,szFileName);
	this->m_iTimeAttackEvent = GetPrivateProfileInt("GameServerInfo","TimeAttackTime",600,szFileName);
	this->m_iMixNeedZen = GetPrivateProfileInt("GameServerInfo","MixNeedZen",1000000,szFileName);

	g_nMysteriousPaperDropRate = GetPrivateProfileInt("GameServerInfo","MysteriousPaperDropRate",0,szFileName);

	nTemp = GetPrivateProfileInt("GameServerInfo","IGPromotionOn",0,szFileName);

	if( nTemp == 1 )
		this->m_bPromotion = true;
	else
		this->m_bPromotion = false;

	this->m_RewardExp.LoadScript(gDirPath.GetNewPath(FILE_EVENT_IMPERIAL_REWARD));
	this->m_bScriptLoaded = true;

	if( this->m_bEnabled )
		InitEventDungeon();
}

void CImperialGuardian::Run()
{
	if( !this->m_bEnabled )
		return;

	for(int i = 0; i < IMPERIAL_GUARDIAN_MAXZONE; i++)
	{
		switch( this->m_ZoneInfo[i].m_iState )
		{
		case IMPERIAL_STATE_READY:
			ProcReady(i);
			break;
		case IMPERIAL_STATE_BEGINTIMEATTACK:
			ProcBeginTimeAttack(i);
			break;
		case IMPERIAL_STATE_BEGINLOOTTIME:
			ProcBeginLootTime(i);
			break;
		case IMPERIAL_STATE_BEGINWAITPLAYER:
			ProcBeginWaitPlayer(i);
			break;
		case IMPERIAL_STATE_PROCWARPNEXTZONE:
			ProcAllWarpNextZone(i);
			break;
		case IMPERIAL_STATE_KICKALL:
			ProcAllKick(i);
			break;
		case IMPERIAL_STATE_MISSIONFAIL:
			ProcMissionFail(i);
			break;
		case IMPERIAL_STATE_MISSIONCLEAR:
			ProcMissionClear(i);
			break;
		}

		ProcCheckDungeon(i);
	}

	KickInvalidUser();
}

void CImperialGuardian::ProcReady(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	if( GetPlayerCount(nZoneIndex) > 0 )
	{
		SetZoneState(nZoneIndex,IMPERIAL_STATE_KICKALL);
	}
}
void CImperialGuardian::ProcBeginTimeAttack(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	if( !this->m_ZoneInfo[nZoneIndex].m_bInitTimeAttack )
	{
		this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT3 = GetTickCount();
		this->m_ZoneInfo[nZoneIndex].m_bInitTimeAttack = TRUE;

		char szText[256] = {0};
		wsprintf(szText,lMsg.Get(MSGGET(13,107)));
		this->GCSendServerMsgAll(nZoneIndex,szText);
		this->SetAtackAbleState(nZoneIndex,525,true);
		this->SetAtackAbleState(nZoneIndex,528,true);

		int nMonsterRegenTable = this->m_ZoneInfo[nZoneIndex].m_wMonsterRegenTable;
		int nMaxLevel = this->m_ZoneInfo[nZoneIndex].m_iMaxLevel;

		this->RegenMonster(nZoneIndex,nMonsterRegenTable,nMaxLevel,false);
		this->GCNotifyRemainTickCount(nZoneIndex,2,this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC);

		LogAddTD("[IMPERIALGUARDIAN] BEGIN TIMEATTACK -> [ZONE]:%d ",(nZoneIndex+1));
		LogAddTD("[IMPERIALGUARDIAN] OBJECT COUNT -> [ZONE]:%d [COUNT]:%d",
			(nZoneIndex+1),this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.size());
	}
	else
	{
		int iTickSec = GetTickCount() - this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT3;

		if( !this->m_ZoneInfo[nZoneIndex].btUnk6 &&
			this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC <= 60000 )
		{
			char szText[256] = {0};
			wsprintf(szText,lMsg.Get(MSGGET(13,112)));
			this->GCSendServerMsgAll(nZoneIndex,szText);
			this->m_ZoneInfo[nZoneIndex].btUnk6 = TRUE;
		}

		if( iTickSec >= 1000 )
		{
			if( this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC <= iTickSec )
			{
				this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC = 0;
			}
			else
			{
				this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC -= iTickSec;
			}

			this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT3 = GetTickCount();
			this->GCNotifyRemainTickCount(nZoneIndex,2,this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC);
		}

		if( this->IsLastZone(nZoneIndex) )
		{
			if( this->GetLiveMonsterCount(nZoneIndex) < 1 )
			{
				this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC = 0;
			}
		}
		else
		{
			if( this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC >= 60000 )
			{
				char szText[256] = {0};
				int nRemainMin = this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC/60000+1;

				if( nRemainMin != this->m_ZoneInfo[nZoneIndex].m_iNotifyMinute )
				{
					wsprintf(szText,lMsg.Get(MSGGET(13,113)),nRemainMin);
					this->GCSendServerMsgAll(nZoneIndex,szText);
					this->m_ZoneInfo[nZoneIndex].m_iNotifyMinute = nRemainMin;
				}
			}
		}

		if( this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC <= 0 )
		{
			this->m_ZoneInfo[nZoneIndex].m_bInitTimeAttack = 0;

			if( this->GetLiveMonsterCount(nZoneIndex) < 1 )
			{
				if( this->IsLastZone(nZoneIndex) )
				{
					this->SetZoneState(nZoneIndex,IMPERIAL_STATE_MISSIONCLEAR);
				}
				else
				{
					this->SetZoneState(nZoneIndex,IMPERIAL_STATE_BEGINLOOTTIME);
				}
				this->m_ZoneInfo[nZoneIndex].m_bWait = TRUE;
			}
			else
			{
				this->SetZoneState(nZoneIndex,IMPERIAL_STATE_MISSIONFAIL);
				int nMonsterRegenTable = this->m_ZoneInfo[nZoneIndex].m_wMonsterRegenTable;
				this->DeleteMonster(nZoneIndex,nMonsterRegenTable);
			}

			LogAddTD("[IMPERIALGUARDIAN] END TIMEATTACK -> [ZONE]:%d ",(nZoneIndex+1));
		}
	}
}
void CImperialGuardian::ProcBeginLootTime(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	if( !this->m_ZoneInfo[nZoneIndex].m_bInitLootTime )
	{
		this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT2 = GetTickCount();
		this->m_ZoneInfo[nZoneIndex].m_bInitLootTime = TRUE;

		this->GCNotifyRemainTickCount(nZoneIndex,0,this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2);
		LogAddTD("[IMPERIALGUARDIAN] BEGIN LOOTTIME -> [ZONE]:%d ",(nZoneIndex+1));
		this->GCNotifyRemainTickCount(nZoneIndex,0,this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2);

		if( !this->IsLastZone(nZoneIndex) )
		{
			char szText[256] = {0};
			wsprintf(szText,lMsg.Get(MSGGET(13,108)));
			this->GCSendServerMsgAll(nZoneIndex,szText);
			wsprintf(szText,lMsg.Get(MSGGET(13,109)));
			this->GCSendServerMsgAll(nZoneIndex,szText);
		}
	}
	else
	{
		int iTickSec = GetTickCount() - this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT2;

		if( iTickSec >= 1000 )
		{
			if( this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2 <= iTickSec )
			{
				this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2 = 0;
			}
			else
			{
				this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2 -= iTickSec;
			}

			this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT2 = GetTickCount();
			this->GCNotifyRemainTickCount(nZoneIndex,0,this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2);
		}

		if( this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2 <= 0 )
		{
			this->m_ZoneInfo[nZoneIndex].m_bInitLootTime = FALSE;
			LogAddTD("[IMPERIALGUARDIAN] END LOOTTIME -> [ZONE]:%d ",(nZoneIndex+1));

			if( this->IsLastZone(nZoneIndex) )
			{
				this->SetZoneState(nZoneIndex,IMPERIAL_STATE_KICKALL);
			}
			else
			{
				this->SetZoneState(nZoneIndex,IMPERIAL_STATE_MISSIONFAIL);
			}

			int nMonsterRegenTable = this->m_ZoneInfo[nZoneIndex].m_wMonsterRegenTable;
			this->DeleteMonster(nZoneIndex,nMonsterRegenTable);
		}
	}
}
void CImperialGuardian::ProcBeginWaitPlayer(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	if( !this->m_ZoneInfo[nZoneIndex].m_bInitWaitPlayer )
	{
		this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT1 = GetTickCount();
		this->m_ZoneInfo[nZoneIndex].m_bInitWaitPlayer = TRUE;

		LogAddTD("[IMPERIALGUARDIAN] BEGIN WAITPLAYER -> [ZONE]:%d ",(nZoneIndex+1));

		this->GCNotifyRemainTickCount(nZoneIndex,1,this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3);
		int nRemainMin = this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3/60000;

		char szText[256] = {0};
		wsprintf(szText,lMsg.Get(MSGGET(13,106)),nRemainMin);
		this->GCSendServerMsgAll(nZoneIndex,szText);
	}
	else
	{
		int iTickSec = GetTickCount() - this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT1;

		if( iTickSec >= 1000 )
		{
			if( this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3 <= iTickSec )
			{
				this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3 = 0;
			}
			else
			{
				this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3 -= iTickSec;
			}

			this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT1 = GetTickCount();
			this->GCNotifyRemainTickCount(nZoneIndex,1,this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3);
		}

		if( this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3 <= 0 )
		{
			this->SetZoneState(nZoneIndex,IMPERIAL_STATE_BEGINTIMEATTACK);
		}
	}
}
void CImperialGuardian::ProcAllWarpNextZone(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	int nPartyNumber = this->m_ZoneInfo[nZoneIndex].m_iPartyNumber;
	int nPartyCount = gParty.m_PartyS[nPartyNumber].Count;

	if( (nZoneIndex+1) >= IMPERIAL_GUARDIAN_MAXZONE )
	{
		std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
		std::vector<int>::iterator it = vtTemp.begin();
		for( ; it != vtTemp.end(); ++it)
		{
			int nResult = *it;
			gObjMoveGate(nResult,22);
		}
	}
	else
	{
		std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
		std::vector<int>::iterator it = vtTemp.begin();
		for( ; it != vtTemp.end(); ++it)
		{
			int nResult = *it;

			if( gObj[nResult].Connected > 0 )
				gObjMoveGate(nResult,22);
		}
	}
}
void CImperialGuardian::ProcAllKick(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
	std::vector<int>::iterator it = vtTemp.begin();
	
	for( ; it != vtTemp.end(); ++it)
	{
		int nResult = *it;
		
		if( gObj[nResult].Connected > 0 )
		{
			gObjMoveGate(nResult,22);
			LogAddTD("[IMPERIALGUARDIAN] Leave Player Zone -> [ZONE]:%d [AccountID]:%s [Name]:%s ",
				nZoneIndex+1,gObj[nResult].AccountID,gObj[nResult].Name);
		}
	}

	this->InitZone(nZoneIndex);
	this->SetZoneState(nZoneIndex,IMPERIAL_STATE_READY);

	LogAddTD("[IMPERIALGUARDIAN] ALL KICK USER -> [ZONE]:%d ",(nZoneIndex+1));
}
void CImperialGuardian::ProcMissionFail(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}
	this->GCMissionFail(nZoneIndex);
	LogAddTD("[IMPERIALGUARDIAN] MISSION FAIL -> [ZONE]:%d ",(nZoneIndex+1));
	this->SetZoneState(nZoneIndex,IMPERIAL_STATE_KICKALL);
}
void CImperialGuardian::ProcMissionClear(int nZoneIndex)
{
	/*if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}*/

	this->GCNotifyAllZoneClear(nZoneIndex);

	std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
	std::vector<int>::iterator it = vtTemp.begin();

	for( ; it != vtTemp.end(); ++it)
	{
		int nResult = *it;
		
		if( gObj[nResult].Connected > 0 && IF_MAP_RANGE(gObj[nResult].MapNumber) )
		{
			LogAddTD("[IMPERIALGUARDIAN] MISSION CLEAR USER ->[AccountID]:%s [Name]:%s ",
				gObj[nResult].AccountID,gObj[nResult].Name);

			if( nZoneIndex == 3 && this->m_bPromotion == TRUE )
			{
				// Event server send info ... 
				//GEReqRegIGMissionClear(nResult);
			}
		}
	}

	this->SetZoneState(nZoneIndex,IMPERIAL_STATE_BEGINLOOTTIME);
}

bool CImperialGuardian::IsEmptyZone(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return false;
	}

	if( this->m_ZoneInfo[nZoneIndex].m_iState == IMPERIAL_STATE_READY )
	{
		if( !this->GetPlayerCount(nZoneIndex) )
		{
			return true;
		}
		return false;
	}
return false;
}

void CImperialGuardian::KickInvalidUser()
{
	for(int iCount = OBJ_STARTUSERINDEX; iCount < OBJMAX; iCount++)
	{
		if( (gObj[iCount].Connected == PLAYER_PLAYING ||
			gObj[iCount].Connected == 4 ) &&
			gObj[iCount].Type == OBJ_USER &&
			IF_MAP_RANGE(gObj[iCount].MapNumber) )
		{
			if( (gObj[iCount].Authority&2) != 2 )
			{
				if( !this->CheckValidUser(iCount) )
				{
					if( !gObj[iCount].RegenOk &&
						gObj[iCount].m_State == 2 &&
						gObj[iCount].Live == TRUE )
					{
						gObjMoveGate(iCount,22);
						LogAddTD("[IMPERIALGUARDIAN][ Invalid User ] [%s][%s] ",
							gObj[iCount].AccountID,gObj[iCount].Name);
					}
				}
				else
				{
					if( !this->m_wCheatMode )
					{
						if( gObj[iCount].PartyNumber < 0 )
						{
							gObjMoveGate(iCount,22);
							LogAddTD("[IMPERIALGUARDIAN][ Invalid User ] User is not a party [%s][%s] ",
								gObj[iCount].AccountID,gObj[iCount].Name);
						}
					}
				}
			}
		}
		else
		{
			if( this->CheckValidUser(iCount) )
			{
				this->RemoveUserInAllZone(iCount);
			}
		}
	}
}

bool CImperialGuardian::CheckValidUser(int nUserNumber)
{
	if( this->GetCurrentZoneIndex(nUserNumber) > -1 )
		return true;
return false;
}

int CImperialGuardian::GetZoneState(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
		return -1;

return this->m_ZoneInfo[nZoneIndex].m_iState;
}

bool CImperialGuardian::SetZoneState(int nZoneIndex,int nState)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
		return false;

	if( nState > IMPERIAL_STATE_MISSIONCLEAR+1 )
		return false;

	this->m_ZoneInfo[nZoneIndex].m_iState = nState;
	return true;
}

int gFreeServer = 0;

struct PMSG_ANS_ENTERPORTAL
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btFortDay;
	BYTE btFortZone;
	BYTE btTmp;
	int iTime;
};

void CImperialGuardian::CGEnterPortal(int nUserIndex,int nDestZoneIndex)
{
	if( !OBJMAX_RANGE(nUserIndex) )
		return;

	if( !IMPERIAL_ZONE_CHECK(nDestZoneIndex) )
		return;

	LPOBJ lpObj = &gObj[nUserIndex];

	if( lpObj->m_IfState.use && lpObj->m_IfState.type != 12 )
		return;

	if( gFreeServer == TRUE )
	{
		if( lpObj->Level < 150 )
		{
			PMSG_ANS_ENTERPORTAL pMsg = {0};
			pMsg.btResult = 6;
			PHeadSubSetB((LPBYTE)&pMsg,0xF7,0x02,sizeof(pMsg));
			DataSend(nUserIndex,(LPBYTE)&pMsg,pMsg.h.size);
			return;
		}
	}

	if( lpObj->m_bPShopOpen == TRUE )
		CGPShopReqClose(lpObj->m_Index);

	int nEnterItemPos = -1;

	PMSG_ANS_ENTERPORTAL pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF7,0x02,sizeof(pMsg));

	pMsg.btTmp = 0;
	pMsg.btResult = 0;
	pMsg.btFortZone = nDestZoneIndex+1;
	pMsg.iTime = this->m_ZoneInfo[nDestZoneIndex].m_dwREMAIN_MSEC + this->m_ZoneInfo[nDestZoneIndex].m_dwREMAIN_MSEC2 + this->m_ZoneInfo[nDestZoneIndex].m_dwREMAIN_MSEC3;

	if( !this->m_bEnabled )
		return;

	CTime tCurrentTime(CTime::GetTickCount());
	int nDayOfWeek = tCurrentTime.GetDayOfWeek();

	if( this->m_wCheatMode && this->m_wDayOfWeek > -1 )
		nDayOfWeek = this->m_wDayOfWeek;

	if( gParty.m_PartyS[gObj[nUserIndex].PartyNumber].Count < 1 )
	{
		pMsg.btResult = 5;
		this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));
		return;
	}

	if( this->m_wCheatMode != 1 )
	{
		if( nDestZoneIndex == 0 )
		{
			if( nDayOfWeek != 1 )
			{
				nEnterItemPos = this->CheckGaionOrderPaper(nUserIndex);

				if( nEnterItemPos >= 0 )
				{

				}
				else if( nEnterItemPos == -1 )
				{
					pMsg.btResult = 2;
					this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));
					return;
				}
			}
			else
			{
				nEnterItemPos = this->CheckFullSecromicon(nUserIndex);

				if( nEnterItemPos >= 0 )
				{

				}
				else if( nEnterItemPos == -1 )
				{
					pMsg.btResult = 2;
					this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));
					return;
				}
			}
		}
	}

	if( this->m_wCheatMode != 1 && nDestZoneIndex > 0 )
	{
		if( this->GetCurrentZoneIndex(nUserIndex) != (nDestZoneIndex-1))
			return;
	}

	if( this->GetZoneState(nDestZoneIndex) != IMPERIAL_STATE_READY && 
		this->GetZoneState(nDestZoneIndex) != IMPERIAL_STATE_BEGINWAITPLAYER )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(13,136)),nUserIndex,1);
		
		if( nDestZoneIndex != 0 )
		{
			this->RollBackUserPos(nUserIndex);
		}
		else
		{
			pMsg.btResult = 1;
			this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));
		}
		return;
	}

	if( nDestZoneIndex == 0 && this->m_wCheatMode == 0 )
	{
		char bFlag = 0;

		for(int i = 1; i < IMPERIAL_GUARDIAN_MAXZONE; i++)
		{
			if( this->m_ZoneInfo[i].m_iPartyNumber == gObj[nUserIndex].PartyNumber )
			{
				bFlag = TRUE;
				break;
			}
		}

		if( bFlag )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(13,137)),nUserIndex,1);
			pMsg.btResult = 1;
			this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));

			if( nDestZoneIndex != 0 )
			{
				this->RollBackUserPos(nUserIndex);
			}
			return;
		}
	}

	pMsg.btFortDay = nDayOfWeek-1;

	if( pMsg.btFortDay < 1 )
		pMsg.btFortDay = 7;

	if( this->m_ZoneInfo[nDestZoneIndex].m_iPartyNumber < 0 &&
		this->m_ZoneInfo[nDestZoneIndex].m_vtUserIndex.size() < 1)
	{
		if( nDestZoneIndex > 0 &&
			this->m_wCheatMode != 1 && 
			!this->m_ZoneInfo[nDestZoneIndex-1].m_bWait )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(13,138)),nUserIndex,1);
			this->RollBackUserPos(nUserIndex);
			return;
		}

		int nTest = gObj[nUserIndex].PartyNumber;
		this->m_ZoneInfo[nDestZoneIndex].m_iPartyNumber = gObj[nUserIndex].PartyNumber;

		if( nDestZoneIndex == 0 )
		{
			this->m_ZoneInfo[nDestZoneIndex].m_wMonsterRegenTable = nDayOfWeek;
		}
		else
		{
			if( this->m_ZoneInfo[nDestZoneIndex-1].m_wMonsterRegenTable > 0 && 
				this->m_wCheatMode == 0 )
			{
				this->m_ZoneInfo[nDestZoneIndex].m_wMonsterRegenTable = this->m_ZoneInfo[nDestZoneIndex-1].m_wMonsterRegenTable;
				this->m_ZoneInfo[nDestZoneIndex].m_iMaxLevel = this->m_ZoneInfo[nDestZoneIndex-1].m_iMaxLevel;
			}
			else
			{
				this->m_ZoneInfo[nDestZoneIndex].m_wMonsterRegenTable = nDayOfWeek;
			}
		}

		int nMaxLevel = 0;

		if( !gParty.m_PartyS[this->m_ZoneInfo[nDestZoneIndex].m_iPartyNumber].m_MaxLevel )
		{
			nMaxLevel = gObj[nUserIndex].Level + gObj[nUserIndex].MasterLevel;
		}
		else
		{
			nMaxLevel = gGetPartyMaxLevel(this->m_ZoneInfo[nDestZoneIndex].m_iPartyNumber);
		}

		this->m_ZoneInfo[nDestZoneIndex].m_iMaxLevel = nMaxLevel;
		this->RegAllPartyUser(nDestZoneIndex,nUserIndex);
		this->RegenMonster(nDestZoneIndex,this->m_ZoneInfo[nDestZoneIndex].m_wMonsterRegenTable,nMaxLevel,true);
		this->SetZoneState(nDestZoneIndex,IMPERIAL_STATE_BEGINWAITPLAYER);
	}
	else
	{
		if( this->m_ZoneInfo[nDestZoneIndex].m_iPartyNumber != gObj[nUserIndex].PartyNumber )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(13,139)),nUserIndex,1);
			pMsg.btResult = 1;
			this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));
			return;
		}

		if( this->m_wCheatMode == 0 && nDestZoneIndex == 0 &&
			!this->IsRegPartyUser(nDestZoneIndex,nUserIndex) )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(13,139)),nUserIndex,1);
			pMsg.btResult = 1;
			this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));
			return;
		}

		if( this->GetPlayerCount(nDestZoneIndex) > 5 )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(13,140)),nUserIndex,1);
			pMsg.btResult = 3;
			this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));
			return;
		}
	}

	pMsg.btTmp = this->iUnk1;

	if( nDestZoneIndex > 0 )
		this->RemoveUserInZone(nDestZoneIndex-1,nUserIndex);

	int nGateIndex = this->GetGateNumber(nDestZoneIndex,this->m_ZoneInfo[nDestZoneIndex].m_wMonsterRegenTable);
	if( nGateIndex != -1 )
	{
		this->AddUserInZone(nDestZoneIndex,nUserIndex);
		pMsg.btResult = 0;
		this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));

		if( gObjCheckUsedBuffEffect(lpObj,AT_NPC_HELP) == TRUE )
			gObjRemoveBuffEffect(lpObj,AT_NPC_HELP);

		gObjMoveGate(nUserIndex,nGateIndex);

		if( this->m_wCheatMode != 1 &&
			nDestZoneIndex == 0 &&
			nEnterItemPos >= 0 )
		{
			if( lpObj->pInventory[nEnterItemPos].m_Type == ITEMGET(14,102) ||
				lpObj->pInventory[nEnterItemPos].m_Type == ITEMGET(14,109) )
			{
				gObjInventoryDeleteItem(nUserIndex,nEnterItemPos);
				GCInventoryItemDeleteSend(nUserIndex,nEnterItemPos,1);
			}
			else if( lpObj->pInventory[nEnterItemPos].m_Type == ITEMGET(13,126) ||
				lpObj->pInventory[nEnterItemPos].m_Type == ITEMGET(13,127) )
			{
				lpObj->pInventory[nEnterItemPos].m_Durability -= 1.0f;

				if( lpObj->pInventory[nEnterItemPos].m_Durability < 0 )
				{
					gObjInventoryDeleteItem(nUserIndex,nEnterItemPos);
					GCInventoryItemDeleteSend(nUserIndex,nEnterItemPos,1);
				}
				else
				{
					GCItemDurSend2(nUserIndex,nEnterItemPos,(BYTE)gObj[nUserIndex].pInventory[nEnterItemPos].m_Durability,0);
				}
			}
		}

		LogAddTD("[IMPERIALGUARDIAN] Enter Zone User -> [AccountID]:%s [NAME]:%s [ZONE]:%d ",
			gObj[nUserIndex].AccountID,gObj[nUserIndex].Name,(nDestZoneIndex+1));
		LogAddTD("[IMPERIALGUARDIAN] [%d ZONE USER COUNT] : %d  ",(nDestZoneIndex+1),this->GetPlayerCount(nDestZoneIndex));
	}
}

void CImperialGuardian::RegenMonster(int nZoneIndex,int nMonsterRegenTableIndex,int nMaxUserLevel,bool bOnlyCreateGate)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("[IMPERIALGUARDIAN][RegenMonsterFunc] Invalid zone index => %d  ",nZoneIndex);
		return;
	}

	if( nMonsterRegenTableIndex < 1 ||
		nMonsterRegenTableIndex > 7 )
	{
		LogAddTD("[IMPERIALGUARDIAN][RegenMonsterFunc] Invalid MonsterRegenTable Index => %d  ",nMonsterRegenTableIndex);
		return;
	}

	int nMaxLevel = gParty.m_PartyS[this->m_ZoneInfo[nZoneIndex].m_iPartyNumber].m_MaxLevel;

	if( nMaxUserLevel != 0 )
		nMaxLevel = nMaxUserLevel;

	for(int i = 0; i < gMSetBase.m_Count; i++)
	{
		if( gMSetBase.m_Mp[i].m_ArrangeType == 5 &&
			gMSetBase.m_Mp[i].m_wZoneIndex == nZoneIndex &&
			gMSetBase.m_Mp[i].m_wMonsterRegenTableIndex == nMonsterRegenTableIndex )
		{
			if( bOnlyCreateGate )
			{
				if( gMSetBase.m_Mp[i].m_Type < 524 ||
					gMSetBase.m_Mp[i].m_Type > 528 )
				{
					continue;
				}
			}
			else
			{
				if( gMSetBase.m_Mp[i].m_Type >= 524 &&
					gMSetBase.m_Mp[i].m_Type <= 528 )
				{
					continue;
				}
			}

			int nResult = gObjAddMonster(gMSetBase.m_Mp[i].m_MapNumber);

			if( nResult >= 0 )
			{
				if( !gObjSetPosMonster(nResult,i) )
				{
					gObjDel(nResult);
					LogAdd("error : %s %d",__FILE__,__LINE__);
					continue;
				}

				if( !this->SetMonster(nResult,gMSetBase.m_Mp[i].m_Type,nMaxLevel) )
				{
					gObjDel(nResult);
					LogAdd("error : %s %d",__FILE__,__LINE__);
					continue;
				}

				gObj[nResult].m_ImperialGuardianZone = nZoneIndex;

				_stMonsterIndexInfo stMonsterIndex;

				if( gMSetBase.m_Mp[i].m_Type >= 523 &&
					gMSetBase.m_Mp[i].m_Type <= 528 )
				{
					stMonsterIndex.m_bLiveMonster = 0;
					stMonsterIndex.m_iIndex = nResult;
					stMonsterIndex.m_bAttack = 0;

					if( gMSetBase.m_Mp[i].m_Type >= 524 &&
						gMSetBase.m_Mp[i].m_Type <= 528 &&
						gMSetBase.m_Mp[i].m_Type != 526 )
					{
						LogAddTD("[IMPERIALGUARDIAN] AddGate => %s, MapNumber => %d ,Zone => %d PosX => %d, PosY => %d ",
							gObj[nResult].Name,gObj[nResult].MapNumber,gObj[nResult].m_ImperialGuardianZone+1,gObj[nResult].X,gObj[nResult].Y);
					}
				}
				else
				{
					stMonsterIndex.m_bLiveMonster = 1;
					stMonsterIndex.m_iIndex = nResult;
					stMonsterIndex.m_bAttack = 1;
					this->m_ZoneInfo[nZoneIndex].m_bMonsterSpawn = 1;

					LogAddTD("[IMPERIALGUARDIAN] AddMonster => %s, MapNumber => %d ,Zone => %d PosX => %d, PosY => %d, Connected => %d, STATE => %d ",
						gObj[nResult].Name,gObj[nResult].MapNumber,gObj[nResult].m_ImperialGuardianZone+1,gObj[nResult].X,gObj[nResult].Y,gObj[nResult].Connected,gObj[nResult].m_State);
				}

				this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.push_back(stMonsterIndex);

				if( gMSetBase.m_Mp[i].m_Type >= 524 &&
					gMSetBase.m_Mp[i].m_Type <= 528 &&
					gMSetBase.m_Mp[i].m_Type != 526 )
				{
					this->SetGateBlockState(gMSetBase.m_Mp[i].m_MapNumber,nZoneIndex,gMSetBase.m_Mp[i].m_X,gMSetBase.m_Mp[i].m_Y,1,gMSetBase.m_Mp[i].m_Dir);
				}
			}
			else
			{
				LogAddTD("[IMPERIALGUARDIAN][RegenMonsterFunc] gObjAddMonster fail (ErrorCode: %d, MonsterClass: %d)",
					nResult,gMSetBase.m_Mp[i].m_Type);
			}
		}
	}
}

void CImperialGuardian::DeleteMonster(int nZoneIndex,int nMonsterRegenTableIndex)
{
	std::vector<_stMonsterIndexInfo>::iterator it = this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin();
	int nCount = 0;

	for( ; it != this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end(); it++)
	{
		_stMonsterIndexInfo stMonsterIndex = *it;

		if( gObjDel(stMonsterIndex.m_iIndex) )
		{
			gObj[stMonsterIndex.m_iIndex].m_ImperialGuardianZone = -1;
			nCount++;
		}
	}

	this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.clear();
	LogAddTD("[IMPERIALGUARDIAN] DELETE ALL MONSTER -> [ZONE]:%d [COUNT]:%d ",nZoneIndex+1,nCount);
}

void CImperialGuardian::InitZone(int nZoneIndex)
{
	if( !this->m_bScriptLoaded )
	{
		MsgBox("[EVENTDUNGEON] script file not loaded");
		return;
	}

	this->m_ZoneInfo[nZoneIndex].m_bInitLootTime = 0;
	this->m_ZoneInfo[nZoneIndex].m_bInitWaitPlayer = 0;
	this->m_ZoneInfo[nZoneIndex].m_bInitTimeAttack = 0;
	this->m_ZoneInfo[nZoneIndex].m_bWait = 0;
	this->m_ZoneInfo[nZoneIndex].m_bMonsterSpawn = 0;
	this->m_ZoneInfo[nZoneIndex].btUnk6 = 0;

	if( nZoneIndex == 0 )
	{
		this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3 = 120000+this->m_iWaitPlayerTime*1000;
	}
	else
	{
		this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3 = this->m_iWaitPlayerTime*1000;
	}

	this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2 = this->m_iLootTime*1000;
	this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC = this->m_iTimeAttackEvent*1000;

	this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT1 = 0;
	this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT2 = 0;
	this->m_ZoneInfo[nZoneIndex].m_dwTICK_COUNT3 = 0;

	this->m_ZoneInfo[nZoneIndex].m_wMonsterRegenTable = -1;
	this->m_ZoneInfo[nZoneIndex].m_iPartyNumber = -1;
	this->m_ZoneInfo[nZoneIndex].m_iMaxLevel = 0;
	this->m_ZoneInfo[nZoneIndex].m_iState = IMPERIAL_STATE_READY;
	this->m_ZoneInfo[nZoneIndex].m_iNotifyMinute = (this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC/60000)+1;

	this->m_ZoneInfo[nZoneIndex].m_vtPartyUserIndex.clear();
	this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.clear();
	this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.clear();
	this->m_ZoneInfo[nZoneIndex].m_vtDestroyedGate.clear();
}

bool CImperialGuardian::SetMonster(int nIndex,int MonsterClass,int nMaxLevel)
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


	g_MonsterStatCalc.ConvertMonsterStat(lpObj,nMaxLevel,0);

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

int CImperialGuardian::GetGateNumber(int nZoneIndex,int nDayOfWeek)
{
	int nGateNumber = -1;

	if( nZoneIndex == 0 )
	{
		if( nDayOfWeek == 2 || nDayOfWeek == 5 )
		{
			nGateNumber = 307;
		}
		else if( nDayOfWeek == 3 || nDayOfWeek == 6 )
		{
			nGateNumber = 312;
		}
		else if( nDayOfWeek == 4 || nDayOfWeek == 7 )
		{
			nGateNumber = 317;
		}
		else if( nDayOfWeek == 1 )
		{
			nGateNumber = 322;
		}
	}
	else if( nZoneIndex == 1 )
	{
		if( nDayOfWeek == 2 || nDayOfWeek == 5 )
		{
			nGateNumber = 309;
		}
		else if( nDayOfWeek == 3 || nDayOfWeek == 6 )
		{
			nGateNumber = 314;
		}
		else if( nDayOfWeek == 4 || nDayOfWeek == 7 )
		{
			nGateNumber = 319;
		}
		else if( nDayOfWeek == 1 )
		{
			nGateNumber = 324;
		}
	}
	else if( nZoneIndex == 2 )
	{
		if( nDayOfWeek == 2 || nDayOfWeek == 5 )
		{
			nGateNumber = 311;
		}
		else if( nDayOfWeek == 3 || nDayOfWeek == 6 )
		{
			nGateNumber = 316;
		}
		else if( nDayOfWeek == 4 || nDayOfWeek == 7 )
		{
			nGateNumber = 321;
		}
		else if( nDayOfWeek == 1 )
		{
			nGateNumber = 326;
		}
	}
	else if( nZoneIndex == 3 )
	{
		if( nDayOfWeek == 1 )
		{
			nGateNumber = 328;
		}
	}
	else
	{
		LogAddTD("[IMPERIALGUARDIAN][GetGateNumberFunc] Invalid zoneIndex : %d ",(nZoneIndex+1));
	}
	return nGateNumber;
}

int CImperialGuardian::GetMapNumber(int nDayOfWeek)
{
	int nMapNumber = -1;

	if( nDayOfWeek == 2 || nDayOfWeek == 5 )
	{
		nMapNumber = MAP_INDEX_IMPERIALFORT1;
	}
	else if( nDayOfWeek == 3 || nDayOfWeek == 6 )
	{
		nMapNumber = MAP_INDEX_IMPERIALFORT2;
	}
	else if( nDayOfWeek == 4 || nDayOfWeek == 7 )
	{
		nMapNumber = MAP_INDEX_IMPERIALFORT3;
	}
	else if( nDayOfWeek == 1 )
	{
		nMapNumber = MAP_INDEX_IMPERIALFORT4;
	}

return nMapNumber;
}

void CImperialGuardian::ProcCheckDungeon(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
		return;

	CTime tCurrentTime(CTime::GetTickCount());
	int nDayOfWeek = tCurrentTime.GetDayOfWeek();

	if( this->m_iDayOfWeek != nDayOfWeek )
	{
		this->m_iDayOfWeek = nDayOfWeek;
		this->iUnk1 = rand()%4;
	}

	this->CheckLiveMonster(nZoneIndex);

	if( this->GetZoneState(nZoneIndex) )
	{
		if( this->GetZoneState(nZoneIndex) == IMPERIAL_STATE_BEGINTIMEATTACK &&
			this->m_ZoneInfo[nZoneIndex].m_bMonsterSpawn == TRUE &&
			this->GetLiveMonsterCount(nZoneIndex) < 1)
		{
			this->SetAtackAbleState(nZoneIndex,524,true);
			this->SetAtackAbleState(nZoneIndex,527,true);
			this->SetAtackAbleState(nZoneIndex,528,true);
		}

		if( this->GetPlayerCount(nZoneIndex) < 1 )
		{
			int nMonsterRegenTable = this->m_ZoneInfo[nZoneIndex].m_wMonsterRegenTable;
			this->DeleteMonster(nZoneIndex,nMonsterRegenTable);
			this->InitZone(nZoneIndex);
		}
	}
}

int CImperialGuardian::GetLiveMonsterCount(int nZoneIndex)
{
	std::vector<_stMonsterIndexInfo>::iterator it = this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin();
	int nCount = 0;

	for( ; it != this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end(); it++ )
	{
		_stMonsterIndexInfo stMonsterIndex = *it;

		if( stMonsterIndex.m_bLiveMonster == 1 )
		{
			nCount++;
		}
	}
	return nCount;
}

void CImperialGuardian::CheckLiveMonster(int nZoneIndex)
{
	std::vector<_stMonsterIndexInfo>::iterator it = this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin();

	for( ; it != this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end(); it++ )
	{
		_stMonsterIndexInfo stMonsterIndex = *it;

		if( !gObj[stMonsterIndex.m_iIndex].Live && stMonsterIndex.m_bLiveMonster==1 ||
			gObj[stMonsterIndex.m_iIndex].Connected != PLAYER_PLAYING )
		{
			it->m_bLiveMonster = 0;
		}

		if( gObj[stMonsterIndex.m_iIndex].Live && 
			gObj[stMonsterIndex.m_iIndex].m_State != 1 &&
			gObj[stMonsterIndex.m_iIndex].m_State != 2 )
		{
			LogAddTD("[IMPERIALGUARDIAN][RESTORE] [NAME]:%s [INDEX]:%d [TYPE]:%d [STATE]:%d",
				gObj[stMonsterIndex.m_iIndex].Name,stMonsterIndex.m_iIndex,gObj[stMonsterIndex.m_iIndex].Class,gObj[stMonsterIndex.m_iIndex].m_State);

			gObj[stMonsterIndex.m_iIndex].m_State = 1;
		}
	}
}

int CImperialGuardian::GetCurrentZoneIndex(int nUserNumber)
{
	int nResult = -1;

	for(int i = 0; i < IMPERIAL_GUARDIAN_MAXZONE; i++)
	{
		EnterCriticalSection(&this->m_CritSection);

		std::vector<int> vtTemp = this->m_ZoneInfo[i].m_vtUserIndex;
		std::vector<int>::iterator it = vtTemp.begin();

		for( ; it != vtTemp.end(); it++)
		{
			nResult = *it;

			if( nResult == nUserNumber )
			{
				LeaveCriticalSection(&this->m_CritSection);
				return i;
			}
		}
		LeaveCriticalSection(&this->m_CritSection);
	}
	return -1;
}

bool CImperialGuardian::AddUserInZone(int nZoneIndex,int nUserIndex)
{
	EnterCriticalSection(&this->m_CritSection);

	BYTE bAddUser = TRUE;

	std::vector<int>::iterator it = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.begin();

	for( ; it != this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++)
	{
		int nResult = *it;
		if( nResult == nUserIndex )
		{
			bAddUser = 0;
			break;
		}
	}

	if( bAddUser )
	{
		this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.push_back(nUserIndex);
		LeaveCriticalSection(&this->m_CritSection);
		return true;
	}
	else
	{
		LeaveCriticalSection(&this->m_CritSection);
		return false;
	}
}

void CImperialGuardian::RemoveUserInZone(int nZoneIndex,int nUserIndex)
{
	EnterCriticalSection(&this->m_CritSection);

	std::vector<int>::iterator it = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.begin();

	for( ; it != this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++)
	{
		int nResult = *it;

		if( nResult == nUserIndex )
		{
			this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.erase(it);
			LogAddTD("[IMPERIALGUARDIAN] Leave Player Zone [ZONE]:%d, [AccountID]:%s, [Name]:%s",
				nZoneIndex+1,gObj[nUserIndex].AccountID,gObj[nUserIndex].Name);
			break;
		}
	}

	LeaveCriticalSection(&this->m_CritSection);
}

void CImperialGuardian::RemoveUserInAllZone(int nUserNumber)
{
	for(int i = 0; i < IMPERIAL_GUARDIAN_MAXZONE; i++)
	{
		this->RemoveUserInZone(i,nUserNumber);
	}
}

int CImperialGuardian::GetPlayerCount(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
		return 0;

return this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.size();
}

int CImperialGuardian::GetTotalPlayerCount()
{
	int nCount = 0;

	for(int i = 0; i < IMPERIAL_GUARDIAN_MAXZONE; i++)
	{
		nCount += this->GetPlayerCount(i);
	}
return nCount;
}

bool CImperialGuardian::IsLastZone(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
		return false;

	if( this->m_ZoneInfo[nZoneIndex].m_wMonsterRegenTable == 1 && nZoneIndex == 3 )
	{
		return true;
	}

	if( this->m_ZoneInfo[nZoneIndex].m_wMonsterRegenTable != 1 && nZoneIndex == 2 )
	{
		return true;
	}
	return false;
}

struct PMSG_IMPERIALFORT_PLAYEND
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	int iRewardExp;
};

bool CImperialGuardian::GCNotifyZoneClear(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		return false;
	}

	PMSG_IMPERIALFORT_PLAYEND pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF7,0x06,sizeof(pMsg));
	pMsg.btResult = 1;

	std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
	std::vector<int>::iterator it = vtTemp.begin();

	for( ; it != vtTemp.end(); it++)
	{
		int nResult = *it;

		if( IF_MAP_RANGE(gObj[nResult].MapNumber) != FALSE )
		{
			this->GCSendDataToUser(nResult,(LPBYTE)&pMsg,sizeof(pMsg));
		}
	}
return true;
}

bool CImperialGuardian::GCNotifyAllZoneClear(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		return false;
	}

	PMSG_IMPERIALFORT_PLAYEND pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF7,0x06,sizeof(pMsg));
	pMsg.btResult = 2;

	std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
	std::vector<int>::iterator it = vtTemp.begin();

	for( ; it != vtTemp.end(); it++ )
	{
		int nResult = *it;

		if( IF_MAP_RANGE(gObj[nResult].MapNumber) != FALSE )
		{
#ifdef POINTEX
			g_ShopPointEx.AddEventBonus(nResult, ShopPointExEvent::IG);
#endif
			pMsg.iRewardExp = this->m_RewardExp.GetRewardExp(nResult);

			if( nZoneIndex == 3 )
				pMsg.iRewardExp *= 2;

			if( gObjCheckUsedBuffEffect(&gObj[nResult],AT_CSHOP_SEAL_OF_SUSTENANCE) ||
				gObjCheckUsedBuffEffect(&gObj[nResult],AT_PCBANG_SEAL_OF_SUSTENANCE) )
			{
				pMsg.iRewardExp = 0;
			}

			this->ImperialGuardianLevelUp(nResult,pMsg.iRewardExp);
			LogAddTD("[IMPERIALGUARDIAN] [ACCOUNTID]:%s, [NAME]:%s, [Reward Exp] : %d ",
				gObj[nResult].AccountID,gObj[nResult].Name,pMsg.iRewardExp);

			if( g_MasterExp.IsEnabled(&gObj[nResult]) == FALSE) //season3 add-on
			{
				GCSendExp(nResult, (WORD)-1, pMsg.iRewardExp, 0, 0);
			}

			this->GCSendDataToUser(nResult,(LPBYTE)&pMsg,sizeof(pMsg));

			// original code
			//if(g_MasterExp.IsMasterLevelUser(&gObj[iUserIndex]))
			//{
			//	GCSendExp_INT64(iUserIndex, -1, iRewardExp, 0, 0);
			//}

			char szText[256] = {0};
			wsprintf(szText,lMsg.Get(MSGGET(13,110)));
			GCServerMsgStringSend(szText,nResult,0);
		}
	}
	return true;
}

void CImperialGuardian::GCMissionFail(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		return;
	}

	PMSG_IMPERIALFORT_PLAYEND pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF7,0x06,sizeof(pMsg));
	pMsg.btResult = 0;

		std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
	std::vector<int>::iterator it = vtTemp.begin();

	for( ; it != vtTemp.end(); it++ )
	{
		int nResult = *it;

		if( IF_MAP_RANGE(gObj[nResult].MapNumber) != FALSE )
		{
			this->GCSendDataToUser(nResult,(LPBYTE)&pMsg,sizeof(pMsg));

			char szText[256] = {0};
			wsprintf(szText,lMsg.Get(MSGGET(13,111)));
			GCServerMsgStringSend(szText,nResult,0);
		}
	}
}

void CImperialGuardian::GCMissionFailUseDie(int nUserIndex)
{
	if( !OBJMAX_RANGE(nUserIndex) )
		return;

	PMSG_IMPERIALFORT_PLAYEND pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF7,0x06,sizeof(pMsg));
	pMsg.btResult = 0;

	this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));

	char szText[256] = {0};
	wsprintf(szText,lMsg.Get(MSGGET(13,111)));
	GCServerMsgStringSend(szText,nUserIndex,0);
}

void CImperialGuardian::GCSendDataToUser(int aIndex,LPBYTE lpMsg,int nSize)
{
	if( !OBJMAX_RANGE(aIndex) )
		return;

	if( gObj[aIndex].Connected == PLAYER_PLAYING &&
		gObj[aIndex].Type == OBJ_USER )
	{
		DataSend(aIndex,lpMsg,nSize);
	}
}

struct PMSG_IMPERIALFORT_PLAYINFO
{
	PBMSG_HEAD2 h;
	BYTE btState;
	BYTE btDayOfWeek;
	BYTE btZoneIndex;
	int iTime;
	BYTE iMonCount;
};

bool CImperialGuardian::GCNotifyRemainTickCount(int nZoneIndex,BYTE btMsgType,DWORD dwRemainTick)
{
	PMSG_IMPERIALFORT_PLAYINFO pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF7,0x04,sizeof(pMsg));

	pMsg.btState = btMsgType;
	pMsg.btZoneIndex = nZoneIndex+1;
	pMsg.iMonCount = this->GetLiveMonsterCount(nZoneIndex);
	pMsg.btDayOfWeek = this->m_ZoneInfo[nZoneIndex].m_wMonsterRegenTable-1;

	if( pMsg.btDayOfWeek < 1 )
		pMsg.btDayOfWeek = 7;

	if( btMsgType == 0 )
	{
		pMsg.iTime = this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2;
	}
	else if( btMsgType == 1 )
	{
		pMsg.iTime = this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3;
	}
	else if( btMsgType == 2 )
	{
		pMsg.iTime = this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC;
	}
	else
	{
		LogAddTD("[IMPERIALGUARDIAN][GCNotifyRemainTickCountFunc] Invalid MsgType : %d ",btMsgType);
		return false;
	}

	std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
	std::vector<int>::iterator it = vtTemp.begin();

	for( ; it != vtTemp.end(); it++)
	{
		int nResult = *it;

		if( IF_MAP_RANGE(gObj[nResult].MapNumber) != FALSE )
		{
			this->GCSendDataToUser(nResult,(LPBYTE)&pMsg,sizeof(pMsg));
		}
	}
	return true;
}

void CImperialGuardian::SetGateBlockState(int nMapNumber,int nZoneIndex,int nX,int nY,int iGateState,int nDir)
{
	if( !IMPERIAL_COORD_CHECK(nX) ||
		!IMPERIAL_COORD_CHECK(nY) )
		return;

	switch( iGateState )
	{
	default:
		return;
	case 1:
		if( nDir == 3)
		{
			for(int i = nX-3; i <= nX; i++)
			{
				for(int j = nY-2; j <= nY+2; j++)
				{
					if( i == nX && j == nY )
						continue;

					MapC[nMapNumber].m_attrbuf[j*256+i] |= 16;
				}
			}
		}
		else if( nDir == 1 )
		{
			for(int i = nX-2; i <= nX+2; i++)
			{
				for(int j = nY; j <= nY+3; j++)
				{
					if( i == nX && j == nY )
						continue;

					MapC[nMapNumber].m_attrbuf[j*256+i] |= 16;
				}
			}
		}
		else if( nDir == 5 )
		{
			for(int i = nX-2; i <= nX+2; i++)
			{
				for(int j = nY; j <= nY-3; j++)
				{
					if( i == nX && j == nY )
						continue;

					MapC[nMapNumber].m_attrbuf[j*256+i] |= 16;
				}
			}
		}
		else if( nDir == 7 )
		{
			for(int i = nX; i <= nX+3; i++)
			{
				for(int j = nY-2; j <= nY+2; j++)
				{
					if( i == nX && j == nY )
						continue;

					MapC[nMapNumber].m_attrbuf[j*256+i] |= 16;
				}
			}
		}
		break;
	case 0:
		if( nDir == 3)
		{
			for(int i = nX-3; i <= nX; i++)
			{
				for(int j = nY-2; j <= nY+2; j++)
				{
					if( i == nX && j == nY )
						continue;

					MapC[nMapNumber].m_attrbuf[j*256+i] &= ~16;
				}
			}
		}
		else if( nDir == 1 )
		{
			for(int i = nX-2; i <= nX+2; i++)
			{
				for(int j = nY; j <= nY+3; j++)
				{
					if( i == nX && j == nY )
						continue;

					MapC[nMapNumber].m_attrbuf[j*256+i] &= ~16;
				}
			}
		}
		else if( nDir == 5 )
		{
			for(int i = nX-2; i <= nX+2; i++)
			{
				for(int j = nY-3; j <= nY; j++)
				{
					if( i == nX && j == nY )
						continue;

					MapC[nMapNumber].m_attrbuf[j*256+i] &= ~16;
				}
			}
		}
		else if( nDir == 7 )
		{
			for(int i = nX; i <= nX+3; i++)
			{
				for(int j = nY-2; j <= nY+2; j++)
				{
					if( i == nX && j == nY )
						continue;

					MapC[nMapNumber].m_attrbuf[j*256+i] &= ~16;
				}
			}
		}
		break;
	}
}

void CImperialGuardian::GCSendCastleGateInfo(int nGateIndex,int nZoneIndex,int nUserIndex)
{
	BYTE cTEMP_BUF[256];

	PMSG_SETMAPATTR_COUNT * lpMsg = (PMSG_SETMAPATTR_COUNT *)cTEMP_BUF;
	PHeadSetB((LPBYTE)lpMsg, 0x46, sizeof(PMSG_SETMAPATTR_COUNT)+sizeof(PMSG_SETMAPATTR)*6);
	PMSG_SETMAPATTR * lpMsgBody = (PMSG_SETMAPATTR *)(cTEMP_BUF+7);

	lpMsg->btType = 0;
	lpMsg->btCount = 1;
	lpMsg->btMapAttr = 16;

	int iGateState;

	if( gObj[nGateIndex].Live )
	{
		iGateState = 1;
	}
	else
	{
		iGateState = 0;
	}

	int nDir = gObj[nGateIndex].Dir;
	int nX = gObj[nGateIndex].X;
	int nY = gObj[nGateIndex].Y;

	switch( iGateState )
	{
	default:
		break;
	case 1:
		if( nDir == 3 )
		{
			lpMsg->btMapSetType = 0;
			lpMsgBody[0].btX = nX-3;
			lpMsgBody[0].btY = nY-2;
			lpMsgBody[1].btX = nX;
			lpMsgBody[1].btY = nY+2;
		}
		else if( nDir == 1 )
		{
			lpMsg->btMapSetType = 0;
			lpMsgBody[0].btX = nX-2;
			lpMsgBody[0].btY = nY;
			lpMsgBody[1].btX = nX+2;
			lpMsgBody[1].btY = nY+3;
		}
		else if( nDir == 5 )
		{
			lpMsg->btMapSetType = 0;
			lpMsgBody[0].btX = nX-2;
			lpMsgBody[0].btY = nY-3;
			lpMsgBody[1].btX = nX+2;
			lpMsgBody[1].btY = nY;
		}
		else if( nDir == 7 )
		{
			lpMsg->btMapSetType = 0;
			lpMsgBody[0].btX = nX;
			lpMsgBody[0].btY = nY-2;
			lpMsgBody[1].btX = nX+3;
			lpMsgBody[1].btY = nY+2;
		}
		break;
	case 0:
		if( nDir == 3 )
		{
			lpMsg->btMapSetType = 1;
			lpMsgBody[0].btX = nX-3;
			lpMsgBody[0].btY = nY-2;
			lpMsgBody[1].btX = nX;
			lpMsgBody[1].btY = nY+2;
		}
		else if( nDir == 1 )
		{
			lpMsg->btMapSetType = 1;
			lpMsgBody[0].btX = nX-2;
			lpMsgBody[0].btY = nY;
			lpMsgBody[1].btX = nX+2;
			lpMsgBody[1].btY = nY+3;
		}
		else if( nDir == 5 )
		{
			lpMsg->btMapSetType = 1;
			lpMsgBody[0].btX = nX-2;
			lpMsgBody[0].btY = nY-3;
			lpMsgBody[1].btX = nX+2;
			lpMsgBody[1].btY = nY;
		}
		else if( nDir == 7 )
		{
			lpMsg->btMapSetType = 1;
			lpMsgBody[0].btX = nX;
			lpMsgBody[0].btY = nY-2;
			lpMsgBody[1].btX = nX+3;
			lpMsgBody[1].btY = nY+2;
		}
		break;
	}

	if( gObj[nUserIndex].Connected == PLAYER_PLAYING )
	{
		if( DataSend(nUserIndex,(LPBYTE)lpMsg,lpMsg->h.size) )
		{
			LogAddTD("[IMPERIALGUARDIAN] SEND GATE STATE -> [ZONE]:%d [AccountID]:%s, [NAME]:%s [STATE]:%d",
				nZoneIndex+1,gObj[nUserIndex].AccountID,gObj[nUserIndex].Name,iGateState);
			LogAddTD("[IMPERIALGUARDIAN] beginX : %d, beginY : %d , endX :%d , endY : %d",
				lpMsgBody[0].btX,lpMsgBody[0].btY,lpMsgBody[1].btX,lpMsgBody[1].btY);
		}
	}
}

void CImperialGuardian::DropItem(int nIndex,int nMonsterIndex)
{
	int nZoneIndex = this->GetCurrentZoneIndex(nIndex);
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
		return;

	int nMaxLevel = this->m_ZoneInfo[nZoneIndex].m_iMaxLevel;
	int nType = 0;	

	if( nMaxLevel == 0 )
	{
		nMaxLevel = gObj[nIndex].Level + gObj[nIndex].MasterLevel;
	}

	if( gObj[nMonsterIndex].Class == 504 )
	{
		LogAddTD("[IMPERIALGUARDIAN] Kill the boss monster => %s [ZONE]:%d [AccountID]:%s [Name]:%s",
			gObj[nMonsterIndex].Name,gObj[nMonsterIndex].m_ImperialGuardianZone+1,gObj[nIndex].AccountID,gObj[nIndex].Name);
		//pEventDugeonItemBagGaion->DropEventItem(nIndex,nMaxLevel);

		this->m_RewardExp.DropReward(nIndex,2);
	}
	else if( gObj[nMonsterIndex].Class == 526 )
	{
		LogAddTD("[IMPERIALGUARDIAN] Broken Statue => %s [ZONE]:%d [AccountID]:%s [Name]:%s",
			gObj[nMonsterIndex].Name,gObj[nMonsterIndex].m_ImperialGuardianZone+1,gObj[nIndex].AccountID,gObj[nIndex].Name);
		//pEventDugeonItemBagStone->DropEventItem(nIndex,nMaxLevel);

		this->m_RewardExp.DropReward(nIndex,1);
	}
	else
	{
		LogAddTD("[IMPERIALGUARDIAN] Kill the boss monster => %s [ZONE]:%d [AccountID]:%s [Name]:%s",
			gObj[nMonsterIndex].Name,gObj[nMonsterIndex].m_ImperialGuardianZone+1,gObj[nIndex].AccountID,gObj[nIndex].Name);
		//pEventDugeonItemBag->DropEventItem(nIndex,nMaxLevel);

		this->m_RewardExp.DropReward(nIndex,0);
	}

	if( this->m_ZoneInfo[nZoneIndex].m_wMonsterRegenTable != 1)
	{
		if( gObj[nMonsterIndex].Class == 508 )
		{
			nType = ItemGetNumberMake(14,103);
		}
		else if( gObj[nMonsterIndex].Class == 509 )
		{
			nType = ItemGetNumberMake(14,104);
		}
		else if( gObj[nMonsterIndex].Class == 510 )
		{
			nType = ItemGetNumberMake(14,105);
		}
		else if( gObj[nMonsterIndex].Class == 511 )
		{
			nType = ItemGetNumberMake(14,106);
		}
		else if( gObj[nMonsterIndex].Class == 507 )
		{
			nType = ItemGetNumberMake(14,107);
		}
		else if( gObj[nMonsterIndex].Class == 506 )
		{
			nType = ItemGetNumberMake(14,108);
		}
		else
		{
			return;
		}

		int nCount = 0;
		int nRand = rand()%100;

		if( nRand >= 0 && nRand < 50 )
		{
			nCount = 1;
		}
		else if( nRand >= 50 && nRand < 79 )
		{
			nCount = 2;
		}
		else
		{
			nCount = 3;
		}

		for(int i = 0; i < nCount; i++)
		{
			BYTE btDropX = gObj[nIndex].X;
			BYTE btDropY = gObj[nIndex].Y;

			if( !gObjGetRandomItemDropLocation(gObj[nIndex].MapNumber,btDropX,btDropY,2,2,10) )
			{
				btDropX = gObj[nIndex].X;
				btDropY = gObj[nIndex].Y;
			}

			ItemSerialCreateSend(nIndex,gObj[nIndex].MapNumber,btDropX,btDropY,nType,0,0,0,0,0,nIndex,0,0);
			LogAddTD("[IMPERIALGUARDIAN] Drop Item : (%d)(%d/%d) Item:(%s)%d Level:%d op1:%d op2:%d op3:%d",
				gObj[nIndex].MapNumber,btDropX,btDropY,ItemAttribute[nType].Name,nType,0,0,0,0);
		}
	}
}

int CImperialGuardian::CheckOverlapMysteriousPaper(int nIndex,int nItemLevel)
{
	for(int i = INVETORY_WEAR_SIZE; i < 204; i++)
	{
		if( gObj[nIndex].pInventory[i].IsItem() == TRUE &&
			gObj[nIndex].pInventory[i].m_Type == ITEMGET(14,101) &&
			gObj[nIndex].pInventory[i].m_Level == nItemLevel )
		{
			int nItemDur = (int)gObj[nIndex].pInventory[i].m_Durability;

			if( ((nItemDur < 0) ? FALSE : (nItemDur <= 4)) != 0 )
				return i;
		}
	}
return -1;
}

void CImperialGuardian::RollBackUserPos(int nUserNumber)
{
	if( gObj[nUserNumber].Type != OBJ_USER ||
		gObj[nUserNumber].Connected <= PLAYER_LOGGED )
		return;

	int x = gObj[nUserNumber].X;
	int y = gObj[nUserNumber].Y;
	int mapNumber = gObj[nUserNumber].MapNumber;
	int dir = gObj[nUserNumber].Dir;

	gObj[nUserNumber].RegenMapNumber = gObj[nUserNumber].MapNumber;
	gObj[nUserNumber].RegenMapX = x;
	gObj[nUserNumber].RegenMapY = y;

	gObjClearViewport(&gObj[nUserNumber]);

	GCTeleportSend(&gObj[nUserNumber],-1,mapNumber,gObj[nUserNumber].X,gObj[nUserNumber].Y,gObj[nUserNumber].Dir);

	if( gObj[nUserNumber].m_Change >= 0 )
	{
		gObjViewportListProtocolCreate(&gObj[nUserNumber]);
	}
	gObj[nUserNumber].RegenOk = TRUE;
}

int CImperialGuardian::CheckGaionOrderPaper(int nIndex)
{
	if( !OBJMAX_RANGE(nIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return -2;
	}

	LPOBJ lpObj = &gObj[nIndex];
	int nPos;

	if( lpObj->m_IfState.use > 0 )
	{
		return -3;
	}

	if( lpObj->Type != OBJ_USER || 
		lpObj->Connected <= PLAYER_LOGGED )
	{
		return -2;
	}

	for( nPos = 0; nPos < 204; nPos++)
	{
		if( lpObj->pInventory[nPos].IsItem() == TRUE &&
			lpObj->pInventory[nPos].m_Type == ITEMGET(14,102) )
		{
			return nPos;
		}
	}

	for( nPos = 0; nPos < 204; nPos++)
	{
		if( lpObj->pInventory[nPos].m_Type == ITEMGET(13,126) && 
			lpObj->pInventory[nPos].m_Durability > 0.0f )
		{
			return nPos;
		}
	}

return -1;
}

int CImperialGuardian::CheckFullSecromicon(int nIndex)
{
	if( !OBJMAX_RANGE(nIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return -2;
	}

	LPOBJ lpObj = &gObj[nIndex];
	int nPos;

	if( lpObj->m_IfState.use > 0 )
	{
		return -3;
	}

	if( lpObj->Type != OBJ_USER || 
		lpObj->Connected <= PLAYER_LOGGED )
	{
		return -2;
	}

	for( nPos = 0; nPos < 204; nPos++)
	{
		if( lpObj->pInventory[nPos].IsItem() == TRUE &&
			lpObj->pInventory[nPos].m_Type == ITEMGET(14,109) )
		{
			return nPos;
		}
	}

	for( nPos = 0; nPos < 204; nPos++)
	{
		if( lpObj->pInventory[nPos].m_Type == ITEMGET(13,127) && 
			lpObj->pInventory[nPos].m_Durability > 0.0f )
		{
			return nPos;
		}
	}

return -1;
}

bool CImperialGuardian::IsAttackAbleMonster(int nMonsterIndex)
{
	for(int i = 0; i < IMPERIAL_GUARDIAN_MAXZONE; i++)
	{
		int nSize = this->m_ZoneInfo[i].m_vtMonsterIndexInfo.size();

		for( int k = 0; k < nSize; k++)
		{
			_stMonsterIndexInfo monsterInfo = this->m_ZoneInfo[i].m_vtMonsterIndexInfo.at(k);

			if( monsterInfo.m_iIndex == nMonsterIndex )
				return monsterInfo.m_bAttack;
		}
	}
	return false;
}

void CImperialGuardian::SetAtackAbleState(int nZoneIndex,int nMonsterClass,bool bState)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	if( this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.size() < 1 )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	LPOBJ lpObj;
	std::vector<_stMonsterIndexInfo>::iterator it = this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin();

	for( ; it != this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end(); it++)
	{
		_stMonsterIndexInfo stMonsterIndex = *it;
		lpObj = &gObj[stMonsterIndex.m_iIndex];

		if( lpObj->Class == nMonsterClass )
			it->m_bAttack = bState;
	}
}

void CImperialGuardian::SetDayOfWeekGM(int nDayOfWeeks)
{
	this->m_wDayOfWeek = nDayOfWeeks;
}

void CImperialGuardian::SetCheatModeGM(short nCheatMode)
{
	this->m_wCheatMode = nCheatMode;
}

void CImperialGuardian::WarpZoneGM(int nUserIndex,int nZoneIndex)
{
	if( this->m_wCheatMode )
	{
		this->CGEnterPortal(nUserIndex,nZoneIndex);
	}
}

_stZoneInfo CImperialGuardian::GetZoneInfo(int nZoneIndex)
{
	return this->m_ZoneInfo[nZoneIndex];
}

void CImperialGuardian::SetTargetMoveAllMonster(int nZoneIndex,int nTargetNumber)
{
	std::vector<_stMonsterIndexInfo>::iterator it = this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin();
	int nCount = 0;

	if( !OBJMAX_RANGE(nTargetNumber) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	if( gObj[nTargetNumber].Connected < PLAYER_CONNECTED )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	for( ; it != this->m_ZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end(); it++)
	{
		_stMonsterIndexInfo stMonsterIndex = *it;

		if( stMonsterIndex.m_bLiveMonster == 1)
		{
			LPOBJ lpObj = &gObj[stMonsterIndex.m_iIndex];
			LPOBJ lpTargetObj = &gObj[lpObj->TargetNumber];

			if( lpObj->TargetNumber == -1 && lpObj->m_ActState.Emotion )
			{
				lpObj->TargetNumber = nTargetNumber;
				lpObj->m_ActState.Emotion = 1;
				lpObj->m_ActState.EmotionCount = 50;
			}
			else if( lpObj->TargetNumber != nTargetNumber )
			{
				if( rand()%100 < 30 )
				{
					lpObj->TargetNumber = nTargetNumber;
					lpObj->m_ActState.Emotion = 1;
					lpObj->m_ActState.EmotionCount = 50;
				}
			}
		}
	}
}

void CImperialGuardian::DestroyGate(int nZoneIndex,int nIndex,int nTargetIndex)
{
	this->SetGateBlockState(gObj[nIndex].MapNumber,nZoneIndex,gObj[nIndex].X,gObj[nIndex].Y,0,gObj[nIndex].Dir);

	int nSize = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.size();

	for( int i = 0; i < nSize; i++)
	{
		int nUserIndex = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.at(i);
		this->GCSendCastleGateInfo(nIndex,nZoneIndex,nUserIndex);
	}

	this->m_ZoneInfo[nZoneIndex].m_vtDestroyedGate.push_back(nIndex);
	LogAddTD("[IMPERIALGUARDIAN] DestroyGate -> [ZONE]:%d, [AccountID]:%s, [NAME]:%s, [GATE INDEX]:%d, [USER COUNT]:%d ",
		nZoneIndex+1,gObj[nTargetIndex].AccountID,gObj[nTargetIndex].Name,nIndex,nSize);

}

void CImperialGuardian::GCSendServerMsgAll(int nZoneIndex,char* szMsg)
{
	std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
	std::vector<int>::iterator it = vtTemp.begin();

	for( ; it != vtTemp.end(); it++)
	{
		int nResult = *it;

		if( gObj[nResult].Connected > 0 )
		{
			GCServerMsgStringSend(szMsg,nResult,0);
		}
	}
}

int CImperialGuardian::ImperialGuardianLevelUp(int iIndex,int iAddExp)
{
	if( !OBJMAX_RANGE(iIndex) )
		return 0;

	if( g_MasterExp.LevelUp(&gObj[iIndex],iAddExp) )
		return 0;

	gObjSetExpPetItem(iIndex,iAddExp);

	int iLEFT_EXP = 0;

	LogAddTD("Experience : Map[%d]-(%d,%d) [%s][%s](%d) %I64u %d ",
		gObj[iIndex].MapNumber,gObj[iIndex].X,gObj[iIndex].Y,
		gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].Level,gObj[iIndex].Experience,iAddExp);


	if( gObj[iIndex].Level >= 400 )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4,112)),iIndex,1);
		return 0;
	}

	if ( (gObj[iIndex].Experience + iAddExp) < gObj[iIndex].NextExp )
	{
		gObj[iIndex].Experience += iAddExp;
	}
	else
	{
		iLEFT_EXP = gObj[iIndex].Experience + iAddExp - gObj[iIndex].NextExp;
		gObj[iIndex].Experience = gObj[iIndex].NextExp;
		gObj[iIndex].Level++;
#if (ENABLE_CUSTOM_CLASSCALC == 1)
		gObj[iIndex].LevelUpPoint += g_ClassCalc.GetLevelPoint(&gObj[iIndex], 0, 0);
#else
		if( gObj[iIndex].Class == CLASS_DARKLORD || gObj[iIndex].Class == CLASS_MAGUMSA )
		{
			gObj[iIndex].LevelUpPoint += 7;
		}
		else
		{
			gObj[iIndex].LevelUpPoint += 5;
		}
#endif

		if( gObj[iIndex].PlusStatQuestClear != false )
		{
#if (ENABLE_CUSTOM_CLASSCALC == 1)
			gObj[iIndex].LevelUpPoint += g_ClassCalc.GetLevelPoint(&gObj[iIndex], 0, 1);
#else
			gObj[iIndex].LevelUpPoint++;
#endif
			LogAddTD("[%s][%s] LevelUp PlusStatQuest Clear AddStat %d",
				gObj[iIndex].AccountID, gObj[iIndex].Name, gObj[iIndex].LevelUpPoint);
		}

		gObj[iIndex].MaxLife += DCInfo.DefClass[gObj[iIndex].Class].LevelLife;
		gObj[iIndex].MaxMana += DCInfo.DefClass[gObj[iIndex].Class].LevelMana;
		gObj[iIndex].Life = gObj[iIndex].MaxLife;
		gObj[iIndex].Mana = gObj[iIndex].MaxMana;
		gObjNextExpCal(&gObj[iIndex]);
		gObjSetBP(gObj[iIndex].m_Index);

		GCLevelUpMsgSend(gObj[iIndex].m_Index, 1);

//		gObjCalcMaxLifePower(gObj[iIndex].m_Index);
		LogAddTD(lMsg.Get(MSGGET(2, 8)), gObj[iIndex].AccountID, gObj[iIndex].Name, gObj[iIndex].Level);

		if( gObj[iIndex].Level == 400 && gObj[iIndex].PartyNumber >= 0 )
		{
			int partynum = gObj[iIndex].PartyNumber;
			char partyLog[256];
			sprintf(partyLog,"400 LevelUp (%s)(%s) Party ",gObj[iIndex].AccountID,gObj[iIndex].Name);
			int number;

			for( int n = 0; n<MAX_USER_IN_PARTY; n++ )
			{
				number = gParty.m_PartyS[partynum].Number[n];

				if( number >= 0  )
				{
					int noffSet = strlen(partyLog);
					sprintf(&partyLog[noffSet],",(%s)(%s) ",gObj[number].AccountID,gObj[number].Name);
				}
			}
			LogAddTD(partyLog);
		}
	}
	
	GJSetCharacterInfo(&gObj[iIndex], gObj[iIndex].m_Index, 0);
	return iLEFT_EXP;
}

void CImperialGuardian::RegAllPartyUser(int nZoneIndex,int nFirstEnterUserIndex)
{
	if( gObj[nFirstEnterUserIndex].Connected < 1 )
		return;

	BYTE bAddUser = TRUE;
	int nPartyNumber = gObj[nFirstEnterUserIndex].PartyNumber;
	int nPartyCount = gParty.GetPartyCount(nPartyNumber);

	for(int i = 0; i < nPartyCount; i++)
	{
		int nUserNumber = gParty.m_PartyS[nPartyNumber].Number[i];
		this->m_ZoneInfo[nZoneIndex].m_vtPartyUserIndex.push_back(nUserNumber);
	}
}

bool CImperialGuardian::IsRegPartyUser(int nZoneIndex,int nUserIndex)
{
	bool bFlag = false;
	int nSize = this->m_ZoneInfo[nZoneIndex].m_vtPartyUserIndex.size();

	for(int i = 0; i < nSize; i++)
	{
		int nResult = this->m_ZoneInfo[nZoneIndex].m_vtPartyUserIndex.at(i);

		if( nResult == nUserIndex )
			bFlag = true;
	}

	return bFlag;
}

void CImperialGuardian::UserMonsterCountCheck()
{
	// not used debug function
}

void CImperialGuardian::MonsterBaseAct(LPOBJ lpObj)
{
	LPOBJ lpTargetObj = NULL;

	if( lpObj->TargetNumber >= 0 )
	{
		lpTargetObj = &gObj[lpObj->TargetNumber];
	}
	else
	{
		lpObj->m_ActState.Emotion = 0;
	}

	int actcode1;

	switch( lpObj->m_ActState.Emotion )
	{
	case 0:
			if ( lpObj->m_ActState.Attack )
			{
				lpObj->m_ActState.Attack = 0;
				lpObj->TargetNumber = -1;
				lpObj->NextActionTime = 500;
			}

			actcode1 = rand()%2;

			if( lpObj->m_MoveRange > 0 && !gObjCheckUsedBuffEffect(lpObj,AT_ICE_ARROW) )
				gObjMonsterMoveAction(lpObj);

			if( lpObj->m_bIsMonsterAttackFirst != 0 )
			{
				lpObj->TargetNumber = gObjMonsterSearchEnemy(lpObj,OBJ_USER);

				if( lpObj->TargetNumber >= 0 )
				{
					lpObj->m_ActState.EmotionCount = 100;
					lpObj->m_ActState.Emotion = 1;

					this->SetTargetMoveAllMonster(lpObj->m_ImperialGuardianZone,lpObj->TargetNumber);
				}
			}
		break;
	case 1:
			if( lpObj->m_ActState.EmotionCount > 0 )
			{
				lpObj->m_ActState.EmotionCount--;
			}
			else
			{
				lpObj->m_ActState.EmotionCount = 0;
			}

			if( lpObj->TargetNumber >= 0 && lpObj->PathStartEnd == 0 )
			{
				int dis = gObjCalDistance(lpObj,lpTargetObj);

				int attackRange;

				if ( lpObj->m_AttackType >= 100 )
				{
					attackRange = lpObj->m_AttackRange+2;
				}
				else
				{
					attackRange = lpObj->m_AttackRange;
				}

				if ( dis <= attackRange )
				{
					int tuser = lpObj->TargetNumber;
					int map = gObj[tuser].MapNumber;
					BYTE attr;

					if ( MapC[map].CheckWall(lpObj->X, lpObj->Y, gObj[tuser].X, gObj[tuser].Y) == TRUE )
					{
						attr = MapC[map].GetAttr(gObj[tuser].X, gObj[tuser].Y);

						if ( (attr&1) != 1 )
						{
							lpObj->m_ActState.Attack = 1;
							lpObj->m_ActState.EmotionCount = 20+rand()%30;
						}
						else
						{
							lpObj->TargetNumber = -1;
							lpObj->m_ActState.EmotionCount = 30;
							lpObj->m_ActState.Emotion = 1;
						}

						lpObj->Dir = GetPathPacketDirPos(lpTargetObj->X-lpObj->X, lpTargetObj->Y-lpObj->Y);
						lpObj->NextActionTime = lpObj->m_AttackSpeed;
					}
				}
				else
				{
					if ( gObjMonsterGetTargetPos(lpObj) == TRUE )
					{
						if ( MapC[lpObj->MapNumber].CheckWall(lpObj->X, lpObj->Y, lpObj->MTX, lpObj->MTY) == TRUE )
						{
							lpObj->m_ActState.Move = 1;
							lpObj->NextActionTime = 400;
							lpObj->Dir = GetPathPacketDirPos(lpTargetObj->X-lpObj->X, lpTargetObj->Y-lpObj->Y);
						}
						else
						{
							gObjMonsterMoveAction(lpObj);
							lpObj->m_ActState.Emotion = 3;
							lpObj->m_ActState.EmotionCount = 10;
						}
					}
					else
					{
						int tx = 0;
						int ty = 0;

						if( lpObj->Connected > PLAYER_LOGGED && gObjGetTargetPos(lpObj,lpObj->X,lpObj->Y,tx,ty) == TRUE )
						{
							if ( MapC[lpObj->MapNumber].CheckWall(lpObj->X, lpObj->Y, lpObj->MTX, lpObj->MTY) == TRUE )
							{
								lpObj->MTX = tx;
								lpObj->MTY = ty;

								lpObj->m_ActState.Move = 1;
								lpObj->NextActionTime = 400;
								lpObj->Dir = GetPathPacketDirPos(lpTargetObj->X-lpObj->X, lpTargetObj->Y-lpObj->Y);
							}
							else
							{
								gObjMonsterMoveAction(lpObj);
								lpObj->m_ActState.Emotion = 3;
								lpObj->m_ActState.EmotionCount = 10;
							}
						}
					}
				}
			}
		break;
	case 3:
			if ( lpObj->m_ActState.EmotionCount > 0 )
			{
				lpObj->m_ActState.EmotionCount--;
			}
			else
			{
				lpObj->m_ActState.Emotion = 0;
			}

			lpObj->m_ActState.Move = 0;
			lpObj->m_ActState.Attack = 0;
			lpObj->NextActionTime = 400;
		break;
	default:
		break;
	}
}

int CImperialGuardian::ChangeUserIndex(int nUserIndex,int nCurrentUserIndex,int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return 0;
	}

	EnterCriticalSection(&this->m_CritSection);

	int nTotalUserCount = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.size();

	for(int n = 0; n < nTotalUserCount; n++)
	{
		if( this->m_ZoneInfo[nZoneIndex].m_vtUserIndex[n] == nUserIndex )
		{
			this->m_ZoneInfo[nZoneIndex].m_vtUserIndex[n] = nCurrentUserIndex;
			LeaveCriticalSection(&this->m_CritSection);
			return true;
		}
	}

	LeaveCriticalSection(&this->m_CritSection);
	return 0;
}

int CImperialGuardian::SendCurStateToUser(int nUserIndex,int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return 0;
	}

	int nTotalUserCount = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex.size();

	for(int n = 0; n < nTotalUserCount; n++)
	{
		if( this->m_ZoneInfo[nZoneIndex].m_vtUserIndex[n] == nUserIndex )
		{
			CTime tCurrentTime(CTime::GetTickCount());
			int nDayOfWeek = tCurrentTime.GetDayOfWeek();
			PMSG_ANS_ENTERPORTAL pMsg = {0};
			PHeadSubSetB((LPBYTE)&pMsg,0xF7,0x02,sizeof(pMsg));

			pMsg.btTmp = this->iUnk1;
			pMsg.btResult = 0;
			pMsg.btFortZone = nZoneIndex+1;
			pMsg.iTime = this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC+this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC2+this->m_ZoneInfo[nZoneIndex].m_dwREMAIN_MSEC3;
			pMsg.btFortDay = nDayOfWeek-1;

			this->GCSendDataToUser(nUserIndex,(LPBYTE)&pMsg,sizeof(pMsg));
			return true;
		}
	}
return false;
}

int CImperialGuardian::SendCurGateInfoToUser(int nUserIndex,int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return 0;
	}

	int nSize = this->m_ZoneInfo[nZoneIndex].m_vtDestroyedGate.size();

	for(int i = 0; i < nSize; i++)
	{
		int nGateIndex = this->m_ZoneInfo[nZoneIndex].m_vtDestroyedGate.at(i);
		this->GCSendCastleGateInfo(nGateIndex,nZoneIndex,nUserIndex);
	}
return true;
}

int CImperialGuardian::GetPlayUserCountRightNow(int nZoneIndex)
{
	if( !IMPERIAL_ZONE_CHECK(nZoneIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return -1;
	}

	int nPlayUserCount = 0;

	std::vector<int> vtTemp = this->m_ZoneInfo[nZoneIndex].m_vtUserIndex;
	std::vector<int>::iterator it = vtTemp.begin();

	for( ; it != vtTemp.end(); it++)
	{
		int nResult = *it;

		if( !OBJMAX_RANGE(nResult) )
			continue;

		if( gObj[nResult].Connected == PLAYER_PLAYING &&
			IF_MAP_RANGE(gObj[nResult].MapNumber) )
		{
			nPlayUserCount++;
		}
	}
	return nPlayUserCount;
}

int gGetPartyMaxLevel(int nPartyNumber)
{
	int nResult = -1;
	int nLevel = 0;

	for(int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if( gParty.m_PartyS[nPartyNumber].Number[i] > 0 )
		{
			int nUserIndex = gParty.m_PartyS[nPartyNumber].Number[i];
			nLevel = gObj[nUserIndex].Level + gObj[nUserIndex].MasterLevel;

			if( nResult < nLevel )
			{
				nResult = nLevel;
			}
		}
	}
return nResult;
}