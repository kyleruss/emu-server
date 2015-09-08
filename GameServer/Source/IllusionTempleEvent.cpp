#include "stdafx.h"
#include "IllusionTempleEvent.h"
#include "GameMain.h"
#include "ChaosBox.h"
#include "Crywolf.h"
#include "CrywolfSync.h"
#include "CastleSiegeSync.h"
#include "DSProtocol.h"
#include "logproc.h"
#include "..\include\readscript.h"
#include "..\common\winutil.h"
#if( __4GAMERS__ == 1 )
#include "Achievements.h"
#endif

CIllusionTempleEvent g_IllusionTemple;

std::list<ILLUSION_TIME> m_IllusionStartTime;

int m_iIT_TIME_MIN_OPEN;
int m_iIT_TIME_MIN_PREPARE;
int m_iIT_TIME_MIN_PLAY;
int m_iIT_TIME_MIN_CLOSE;


// Constructor / Destructor

CIllusionTempleEvent::CIllusionTempleEvent()
{

}

CIllusionTempleEvent::~CIllusionTempleEvent()
{

}

// Member functions

BOOL CIllusionTempleEvent::Init()
{
	for(int i = 0; i < IT_MAXTEMPLE; i++)
	{
		m_TempleData[i].Reset(i);
	}
return TRUE;
}

extern int g_OldScrollDropRate;
extern int g_CovenantDropRate;
extern int g_bMaxLevelIllusion;
extern int g_bSaveIllusionRank;
extern int g_IllusionUseSkillCnt;

void CIllusionTempleEvent::LoadData()
{
	LoadFile(gDirPath.GetNewPath(FILE_EVENT_ILLUSIONTEMPLE));

	g_bIllusionTemple = GetPrivateProfileInt("GameServerInfo","IllusionTempleEvent",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_OldScrollDropRate = GetPrivateProfileInt("GameServerInfo","OldScrollDropRate",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_CovenantDropRate = GetPrivateProfileInt("GameServerInfo","CovenantOfIllusionDropRate",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_bMaxLevelIllusion = GetPrivateProfileInt("GameServerInfo","UseMaxLevelIllusionTemple",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_bSaveIllusionRank = GetPrivateProfileInt("GameServerInfo","SaveIllusionTempleRankingPoint",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_IllusionRewardPerc = GetPrivateProfileInt("GameServerInfo","IllusionTempleRewardPercent",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_IllusionUseSkillCnt = GetPrivateProfileInt("GameServerInfo","IllusionTempleMinUseSkillCnt",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
}

BOOL CIllusionTempleEvent::LoadFile(char* szFileName)
{
	SMDFile = fopen(szFileName,"r");

	if(SMDFile == NULL)
	{
		MsgBox("[Illusion Temple] Info file Load Fail [%s]",szFileName);
		return FALSE;
	}

	int Token,Index;

	ILLUSION_TIME pTmp;

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

				m_iIT_TIME_MIN_OPEN = (int)TokenNumber;

				Token = GetToken();
				m_iIT_TIME_MIN_PREPARE = (int)TokenNumber;

				Token = GetToken();
				m_iIT_TIME_MIN_PLAY = (int)TokenNumber;

				Token = GetToken();
				m_iIT_TIME_MIN_CLOSE = (int)TokenNumber;
			}
			else if(Index == 1)
			{
				Token = GetToken();

				if( strcmp("end",TokenString) == NULL )
					break;

				pTmp.iHour = (int)TokenNumber;

				Token = GetToken();
				pTmp.iMinute = (int)TokenNumber;

				m_IllusionStartTime.push_back(pTmp);
			}
		}
	}

	fclose(SMDFile);

	LogAdd("%s file load!",szFileName);

return TRUE;
}

void CIllusionTempleEvent::Run()
{
	if( g_bIllusionTemple == FALSE )
		return;

	int iTICK_COUNT = GetTickCount();
	int bCheckSync = FALSE;

	if( m_TempleData[0].GetRemainTime2() == 10 && m_TempleData[0].GetCheckSync() == FALSE )
	{
		bCheckSync = TRUE;
		m_TempleData[0].SetCheckSyncTrue();
	}

	for(int i = 0; i < IT_MAXTEMPLE;i++)
	{
		if( bCheckSync == TRUE )
		{
			m_TempleData[i].CheckSync();
		}

		m_TempleData[i].Run(iTICK_COUNT);
	}
}

int CIllusionTempleProcess::GetCheckSync()
{
	return m_bIT_CHECKSYNC;
}

void CIllusionTempleProcess::SetCheckSyncTrue()
{
	this->m_bIT_CHECKSYNC = TRUE;
}

void CIllusionTempleEvent::PickupRelic(BYTE MapNumber,int aIndex,BYTE Pos)
{
	if( IT_TEMPLE_RANGE(MapNumber - MAP_INDEX_ILLUSIONTEMPLE1) == FALSE )
		return;

	m_TempleData[MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].PickUpRelic(aIndex,Pos);
}

void CIllusionTempleEvent::PickupRelicSend(LPOBJ lpObj)
{
	if( IT_TEMPLE_RANGE(lpObj->MapNumber - MAP_INDEX_ILLUSIONTEMPLE1) == FALSE )
		return;

	m_TempleData[lpObj->MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].PickUpRelicSend(lpObj);
}

struct PMSG_ILLUSIONTEMPLE_ENTER_RESULT
{
	PBMSG_HEAD2 h;
	BYTE result;
};

int CIllusionTempleEvent::Enter(int aIndex,BYTE Temple,BYTE Pos)
{
	if( g_bIllusionTemple == FALSE )
	{
		return FALSE;
	}

	if( IT_TEMPLE_RANGE(Temple) == FALSE )
	{
		return FALSE;
	}

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_ILLUSIONTEMPLE_ENTER_RESULT pMsg;

	PHeadSubSetB((LPBYTE)&pMsg,0xBF,0,sizeof( pMsg ));
	pMsg.result = 0;

	if( lpObj->Type != OBJ_USER || lpObj->Connected <= PLAYER_LOGGED )
	{
		return FALSE;
	}

	if ( lpObj->m_IfState.use != 0 && lpObj->m_IfState.type != 12 )
	{
		return FALSE;
	}

	if ( lpObj->m_bPShopOpen == TRUE )
	{
		CGPShopReqClose(lpObj->m_Index);
	}

	int bFail = FALSE;
	int iLevel;
	int iSerial;

#if (ENABLE_FIX_PARTYPK == 0)

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

	if( bFail == TRUE )
	{
		pMsg.result = 7;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return FALSE;
	}

#endif

	if( IsTransformEquipment(aIndex) )
	{
		pMsg.result = 8;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return FALSE;
	}

	if( m_TempleData[Temple].GetState() != IT_STATE_CLOSED ||
		m_TempleData[Temple].OpenState() == FALSE )
	{
		pMsg.result = 2;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return FALSE;
	}

	if( gObj[aIndex].pInventory[Pos].IsItem() == TRUE )
	{
		if( gObj[aIndex].pInventory[Pos].m_Type == ITEMGET(13,51) )
		{
			iLevel = gObj[aIndex].pInventory[Pos].m_Level;
			iSerial = gObj[aIndex].pInventory[Pos].m_Number;

			if( IT_TEMPLE_RANGE(iLevel-1) == FALSE && gObj[aIndex].pInventory[Pos].m_Durability != 1.0f )
			{
				pMsg.result = 1;
				DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
				return FALSE;
			}
		}
		else if( gObj[aIndex].pInventory[Pos].m_Type == ITEMGET(13,61) )
		{
			iSerial = gObj[aIndex].pInventory[Pos].m_Number;
			iLevel = GetEnterLevel(aIndex);

			if( iLevel <= 0 )
			{
				pMsg.result = 1;
				DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
				return FALSE;
			}
		}
		else
		{
			pMsg.result = 1;
			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
			return FALSE;
		}
	}
	else
	{
		pMsg.result = 1;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return FALSE;
	}

	if( RankingCheck(aIndex,Temple,Pos,iLevel) != FALSE )
	{
		return TRUE;
	}

	if( CheckCanEnter(aIndex,iLevel) == FALSE )
	{
		pMsg.result = 3;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return FALSE;
	}

	m_TempleData[iLevel-1].CheckUsers();

	int bResult = m_TempleData[iLevel-1].EnterUser(aIndex,Temple,Pos);

	if( bResult == TRUE )
	{
		if( lpObj->PartyNumber >= 0 )
		{
			PMSG_PARTYDELUSER pMsg;
			int PartyIndex;
			PartyIndex = gParty.GetIndex(lpObj->PartyNumber,lpObj->m_Index,lpObj->DBNumber);

			if( PartyIndex >= 0 )
			{
				
				pMsg.Number = PartyIndex;

				CGPartyDelUser(&pMsg,lpObj->m_Index);
			}
		}

		if( lpObj->pInventory[Pos].m_Type == ITEMGET(13,51) )
		{
			lpObj->pInventory[Pos].m_Durability = 0;
			GCItemDurSend(lpObj->m_Index,Pos,(BYTE)lpObj->pInventory[Pos].m_Durability,0);
		}
		else if( lpObj->pInventory[Pos].m_Type == ITEMGET(13,61) )
		{
			if( lpObj->pInventory[Pos].m_Durability > 0.0f )
			{
				lpObj->pInventory[Pos].m_Durability -= 1.0f;
				GCItemDurSend(lpObj->m_Index,Pos,(BYTE)lpObj->pInventory[Pos].m_Durability,0);
			}
		}

		LogAddTD("[Illusion Temple] (%d) EnterUser: (%s)(%s) class:%d (Serial:%u).",
			iLevel,lpObj->AccountID,lpObj->Name,lpObj->DbClass,lpObj->pInventory[Pos].m_Number);

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		gObjMoveGate(lpObj->m_Index,141 + iLevel);
	}

	m_TempleData[Temple].SendState(0,lpObj->m_Index);
	SendInfo();
return TRUE;
}

int CIllusionTempleProcess::OpenState()
{
	return m_OpenState;
}

BOOL CIllusionTempleEvent::RankingCheck(int aIndex,BYTE Temple,BYTE Pos,int Level)
{
	return FALSE;
}

BOOL CIllusionTempleEvent::LeaveUser(int aIndex,BYTE MapNumber)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return FALSE;
	}

	m_TempleData[MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].LeaveUser(aIndex);
	SendInfo();
	return TRUE;
}

int CIllusionTempleEvent::CheckCanEnter(int aIndex,int Level)
{
	int Temple = -1;

	if( gObj[aIndex].Level < 220 )
	{
		Temple = -1;
	}
	else if( gObj[aIndex].Level < 271 )
	{
		Temple = 0;
	}
	else if( gObj[aIndex].Level < 321 )
	{
		Temple = 1;
	}
	else if( gObj[aIndex].Level < 351 )
	{
		Temple = 2;
	}
	else if( gObj[aIndex].Level < 381 )
	{
		Temple = 3;
	}
	else if( gObj[aIndex].Level < 400 )
	{
		Temple = 4;
	}
	else if( gObj[aIndex].Level == 400 && gObj[aIndex].ChangeUP2 == FALSE)
	{
		Temple = 4;
	}
#if( ENABLE_FIX_EVENTMASTERLEVEL == 1 )
	else if( gObj[aIndex].ChangeUP2 != FALSE )
#else
	else if( gObj[aIndex].Level >= 400 && gObj[aIndex].ChangeUP2 != FALSE )
#endif
	{
		if( g_bMaxLevelIllusion != 0 )
		{
			Temple = 5;
		}
		else
		{
			Temple = 4;
		}
	}
	else
	{
		Temple = -1;
	}

	if( Temple+1 == Level )
	{
		return TRUE;
	}
return FALSE;
}

void CIllusionTempleEvent::ClickToStatus(BYTE MapNumber,LPOBJ lpNpc,LPOBJ lpObj)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return;
	}

	m_TempleData[MapNumber-MAP_INDEX_ILLUSIONTEMPLE1].ClickToStatus(lpNpc,lpObj);
}

void CIllusionTempleEvent::ClickAlliedNpc(BYTE MapNumber,LPOBJ lpNpc,LPOBJ lpObj)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1) == FALSE )
	{
		return;
	}

	m_TempleData[MapNumber-MAP_INDEX_ILLUSIONTEMPLE1].ClickAlliedNpc(lpNpc,lpObj);
}

void CIllusionTempleEvent::ClickIllusionNpc(BYTE MapNumber,LPOBJ lpNpc,LPOBJ lpObj)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return;
	}

	m_TempleData[MapNumber-MAP_INDEX_ILLUSIONTEMPLE1].ClickIllusionNpc(lpNpc,lpObj);
}

void CIllusionTempleEvent::ClickEnterNpc(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( lpObj->m_IfState.use != 0 && lpObj->m_IfState.type != 12 )
	{
		return;
	}

#if (ENABLE_FIX_NPCTALKPK == 1)
	if( lpObj->m_PK_Level >= 4 )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(13,72)),lpObj->m_Index,1);
		return;
	}
#endif

	int iTemple = -1;

	if( lpObj->Level < 220 )
	{
		iTemple = -1;
	}
	else if(lpObj->Level < 271)
	{
		iTemple = 0;
	}
	else if(lpObj->Level < 321)
	{
		iTemple = 1;
	}
	else if(lpObj->Level < 351)
	{
		iTemple = 2;
	}
	else if(lpObj->Level < 381)
	{
		iTemple = 3;
	}
	else if(lpObj->Level < 400)
	{
		iTemple = 4;
	}
	else if( lpObj->Level == 400 && lpObj->ChangeUP2 == FALSE)
	{
		iTemple = 4;
	}
	else if( lpObj->Level == 400 && lpObj->ChangeUP2 != FALSE )
	{
		if( g_bMaxLevelIllusion != 0 )
		{
			iTemple = 5;
		}
		else
		{
			iTemple = 4;
		}
	}
	else
	{
		iTemple = -1;
	}

	if( IT_TEMPLE_RANGE( iTemple ) == FALSE )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(13,73)),lpObj->m_Index,1);
		return;
	}

	int EnterItem = m_TempleData[iTemple].GetEnterItem(lpObj->m_Index);

	if( EnterItem == -1 )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(13,74)),lpObj->m_Index,1);
		return;
	}

	if( m_TempleData[iTemple].GetState() != IT_STATE_CLOSED )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(13,75)),lpObj->m_Index,1);
		return;
	}

	if( m_TempleData[iTemple].GetState() == IT_STATE_CLOSED && m_TempleData[iTemple].OpenState() == FALSE )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(13,76)),lpObj->m_Index,1);
		return;
	}

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x14;
	pMsg.level1 = m_TempleData[iTemple].GetRemainTime();
	pMsg.level2 = m_TempleData[iTemple].GetUserCount();

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
}

int CIllusionTempleProcess::GetUserCount()
{
	return 0;
}

void CIllusionTempleEvent::DeleteRelic(BYTE MapNumber,LPOBJ lpObj)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return;
	}

	m_TempleData[MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].DeleteRelic(lpObj);
}

void CIllusionTempleEvent::DropRelic(BYTE MapNumber,int aIndex)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return;
	}

	m_TempleData[MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].DropRelic(aIndex);
}

BYTE CIllusionTempleEvent::GetState(BYTE MapNumber)
{
	return m_TempleData[MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].GetState();
}

void CIllusionTempleEvent::AddNpc(BYTE MapNumber,int Class,int Number)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return;
	}

	m_TempleData[MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].AddNpc(Class,Number);
}

void CIllusionTempleEvent::AddMonster(BYTE MapNumber,int Class, int Number)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return;
	}

	m_TempleData[MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].AddMonster(Class,Number);
}

void CIllusionTempleEvent::ResetObject()
{
	for(int i = 0; i < IT_MAXTEMPLE; i++)
	{
		m_TempleData[i].ResetObject();
	}
}

BYTE CIllusionTempleEvent::GetTeam(BYTE MapNumber,int aIndex)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return -1;
	}

	return m_TempleData[MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].GetTeam(aIndex);
}

void CIllusionTempleEvent::ChaosMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg,0x86,sizeof(pMsg));

	pMsg.Result = 0;
	int iIndex = lpObj->m_Index;
	int iLuckCharmCount = 0;

	int iItemResult = ChaosMixItemGet(iIndex);
	iLuckCharmCount = CharmOfLuckCount(iIndex);

	if( iLuckCharmCount > 10 )
	{
		iItemResult = 15;
	}

	if( IT_MAP_RANGE( iItemResult + (MAP_INDEX_ILLUSIONTEMPLE1 - 1) ) != FALSE )
	{
		if( TicketMix(iIndex,iItemResult,iLuckCharmCount) == FALSE )
		{
			lpObj->ChaosLock = FALSE;
			return;
		}
	}

	switch( iItemResult )
	{
	case 9:
		pMsg.Result = 10;
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		break;
	case 10:
		pMsg.Result = 10;
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		break;
	case 11:
		pMsg.Result = 10;
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		break;
	case 12:
		pMsg.Result = 8;
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		break;
	case 13:
		pMsg.Result = 11;
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		break;
	case 14:
		pMsg.Result = 9;
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		break;
	case 15:
		pMsg.Result = 0xF0;
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		break;
	default:
		lpObj->ChaosLock = FALSE;
		break;
	}
}

BYTE CIllusionTempleEvent::TicketMix(int iIndex, int iLevel, int iCharmCount)
{
	if( OBJMAX_RANGE( iIndex ) == FALSE )
	{
		return FALSE;
	}

	if( IT_MAP_RANGE( iLevel + (MAP_INDEX_ILLUSIONTEMPLE1 - 1)) == FALSE )
	{
		return FALSE;
	}

	int Empty = 0;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg,0x86,sizeof(pMsg));

	pMsg.Result = 0;
	gObj[iIndex].ChaosLock = TRUE;

	LogAddTD("[Illusion Temple] Mix Chaos Mix Start (Account:%s, Name:%s, Level:%d)",
		gObj[iIndex].AccountID,gObj[iIndex].Name,iLevel);

	char szTmp[23];

	wsprintf(szTmp,"IllusionTemple Mix,%d",iLevel);
	g_ChaosBox.LogChaosItem(&gObj[iIndex],szTmp);

	int MixRate = IT_TicketRate[-1 + iLevel];

	if( MixRate < 0 || MixRate > 100 )
	{
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		LogAddTD("[Illusion Temple] Mix Chaos Mix Failed - MixRate Out of Bound (Account:%s, Name:%s, Level:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,iLevel);
		return FALSE;
	}

	if ( g_CrywolfSync.GetOccupationState() == 0 && g_iCrywolfApplyMvpBenefit )
	{	
		MixRate += g_CrywolfSync.GetPlusChaosRate();
	}

	int iNeedChaosMoney = IT_TicketZen[-1 + iLevel];

	int iChaosTaxMoney = (int)((__int64)iNeedChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(iIndex) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	iNeedChaosMoney += iChaosTaxMoney;

	if ( iNeedChaosMoney < 0 )
	{
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		LogAddTD("[Illusion Temple] Mix Chaos Mix Failed - MixMoney< 0 (Account:%s, Name:%s, Level:%d).",
			gObj[iIndex].AccountID,gObj[iIndex].Name,iLevel);
		return false;
	}

	if ( ( gObj[iIndex].Money - iNeedChaosMoney ) < 0 )
	{
		pMsg.Result = 11;
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
		LogAddTD("[Illusion Temple] Mix Chaos Mix Failed - Not Enough Money (Account:%s, Name:%s, Level:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,iLevel);
		return false;
	}

	gObj[iIndex].Money -= iNeedChaosMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(iIndex, gObj[iIndex].Money);

	MixRate += iCharmCount;

	if( rand() % 100 < MixRate )
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(&gObj[iIndex], true);
#endif
		int Item = ITEMGET(13,51);
		ItemSerialCreateSend(iIndex,0xFF,0,0,Item,iLevel,0,0,0,0,-1,0,0);
		LogAddTD("[Illusion Temple] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,MixRate,gObj[iIndex].Money,iNeedChaosMoney,iCharmCount);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(&gObj[iIndex], false);
#endif
		g_ChaosBox.ChaosBoxInit(&gObj[iIndex]);
		GCUserChaosBoxSend(&gObj[iIndex], 0);
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);

		LogAddTD("[Illusion Temple] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,MixRate,gObj[iIndex].Money,iNeedChaosMoney,iCharmCount);
		return FALSE;
	}

	gObjInventoryCommit(iIndex);
	return TRUE;
}

BYTE CIllusionTempleEvent::PointAdd(int iIndex,BYTE MapNumber,BYTE Type)
{

	if( OBJMAX_RANGE( iIndex ) == FALSE )
	{
		return FALSE;
	}

	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return FALSE;
	}

	BYTE AddPt = 0;

	if( Type == 1 )
	{
		m_TempleData[MapNumber-MAP_INDEX_ILLUSIONTEMPLE1].AddPlayerKillCount(iIndex);
		AddPt = 5;
	}
	else
	{
		m_TempleData[MapNumber-MAP_INDEX_ILLUSIONTEMPLE1].AddMonsterKillCount(iIndex);
		AddPt = 2;
	}

return m_TempleData[MapNumber-MAP_INDEX_ILLUSIONTEMPLE1].AddKillPts(iIndex,AddPt);
}

void CIllusionTempleEvent::UseSkill(int iIndex,WORD Skill,int iTargetIndex,BYTE State)
{
	if( OBJMAX_RANGE( iIndex ) == FALSE )
	{
		return;
	}

	if( IT_MAP_RANGE( gObj[iIndex].MapNumber ) == FALSE )
	{
		return;
	}

	m_TempleData[ gObj[iIndex].MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ].UseSkill(iIndex,Skill,iTargetIndex,State);
}

void CIllusionTempleEvent::SkillProc(LPOBJ lpObj)
{
	m_TempleData[ lpObj->MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].SkillProc(lpObj);
}

void CIllusionTempleEvent::UserDie(LPOBJ lpObj)
{
	m_TempleData[ lpObj->MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ].UserDie(lpObj);
}

void CIllusionTempleEvent::RegenUser(LPOBJ lpObj)
{
	if( OBJMAX_RANGE( lpObj->m_Index ) == FALSE )
	{
		return;
	}

	if( IT_MAP_RANGE( lpObj->MapNumber ) == FALSE )
	{
		return;
	}

	if( m_TempleData[ lpObj->MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ].GetState() != 2 )
	{

	}
	else
	{
		m_TempleData[ lpObj->MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ].SetProtect(lpObj);
	}
}

void CIllusionTempleEvent::SetMummyNpc(int iIndex)
{
	iEnterNpc = iIndex;
}

struct PMSG_ITSEND_ENTER_INFO
{
	PBMSG_HEAD2 h;	// 4
	BYTE UserCnt[IT_MAXTEMPLE];
};

void CIllusionTempleEvent::SendInfo()
{
	if( m_TempleData[1].GetState() == IT_STATE_CLOSED &&
		m_TempleData[1].OpenState() == TRUE )
	{
		PMSG_ITSEND_ENTER_INFO pMsg;

		PHeadSubSetB((LPBYTE)&pMsg,0xBF,3,sizeof(pMsg));

		pMsg.UserCnt[0] = m_TempleData[0].GetUserCount();
		pMsg.UserCnt[1] = m_TempleData[1].GetUserCount();
		pMsg.UserCnt[2] = m_TempleData[2].GetUserCount();
		pMsg.UserCnt[3] = m_TempleData[3].GetUserCount();
		pMsg.UserCnt[4] = m_TempleData[4].GetUserCount();

		if( g_bMaxLevelIllusion != 0 )
		{
			pMsg.UserCnt[5] = m_TempleData[5].GetUserCount();
		}
		else
		{
			pMsg.UserCnt[5] = 0;
		}

		MsgSendV2(&gObj[this->iEnterNpc],(LPBYTE)&pMsg,pMsg.h.size);
	}
}

int CIllusionTempleEvent::TeleportLock(int iIndex)
{
	if( gObjIsConnected(iIndex) == FALSE )
	{
		return FALSE;
	}

	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		return FALSE;
	}

	if( IT_MAP_RANGE( gObj[iIndex].MapNumber ) == FALSE || m_TempleData[ gObj[iIndex].MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ].GetState() != 2)
	{
		return FALSE;
	}

	if( iIndex == m_TempleData[ gObj[iIndex].MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ].GetRelicPlayer() )
	{
		return TRUE;
	}
return FALSE;
}

int CIllusionTempleProcess::GetRelicPlayer()
{
	return -1;
}

void CIllusionTempleEvent::ItemDrop(LPOBJ lpObj)
{
	if( IT_MAP_RANGE( lpObj->MapNumber ) == FALSE )
	{
		return;
	}

	m_TempleData[ lpObj->MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ].ItemDrop(lpObj);
}

void CIllusionTempleEvent::DropReward(int iIndex)
{
	if( IT_MAP_RANGE( gObj[iIndex].MapNumber ) == FALSE )
	{
		return;
	}

	m_TempleData[ gObj[iIndex].MapNumber - MAP_INDEX_ILLUSIONTEMPLE1].DropReward(iIndex);
}

BYTE CIllusionTempleEvent::GetLeftTime()
{
	BYTE LeftTime = 0;
	if( g_bIllusionTemple )
	{
		LeftTime = m_TempleData[0].GetLeftTime();
	}
	else
	{
		LeftTime = 0;
	}
return LeftTime;
}

int CIllusionTempleEvent::Is1stSkillActive(int iEventIndex,BYTE MapNumber)
{
	if( IT_MAP_RANGE( MapNumber ) == FALSE )
	{
		return FALSE;
	}

return m_TempleData[ MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ].Is1stSkillActive(iEventIndex);
}

int CIllusionTempleEvent::Is2ndSkillActive(int iEventIndex,BYTE MapNumber)
{
	if( IT_MAP_RANGE( MapNumber ) == FALSE )
	{
		return FALSE;
	}

return m_TempleData[ MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ].Is2ndSkillActive(iEventIndex);
}

int CIllusionTempleEvent::GetEnterLevel(int aIndex)
{
	int TempleLevel = 0;
	LPOBJ lpObj = &gObj[aIndex];

	if( OBJMAX_RANGE( aIndex ) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return -1;
	}

	if( lpObj->Level < 220 )
	{
		return 0;
	}

	if( lpObj->Level < 271 )
	{
		return 1;
	}

	if( lpObj->Level < 321 )
	{
		return 2;
	}

	if( lpObj->Level < 351 )
	{
		return 3;
	}

	if( lpObj->Level < 381 )
	{
		return 4;
	}

	if( lpObj->Level < 400 )
	{
		return 5;
	}

	if( lpObj->Level == 400 )
	{
		if( lpObj->ChangeUP == FALSE )
		{
			return 5;
		}
	}

	if( lpObj->Level == 400 )
	{
		if( lpObj->ChangeUP != FALSE )
		{
			if( g_bMaxLevelIllusion != 0 )
			{
				return 6;
			}
			else
			{
				return 5;
			}
		}
	}
return -1;
}

int CIllusionTempleEvent::ChaosMixItemGet(int iIndex)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		return FALSE;
	}

	int Empty = FALSE;
	int bChaos = FALSE;
	int bScroll = FALSE;
	int bPotion = FALSE;
	int bOtherItem = FALSE;
	int iItemCount = 0;
	int iScrollLevel = 0;
	int iPotionLevel = 0;
	int iCharmOfLuckCount = 0;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( gObj[iIndex].pChaosBox[n].IsItem() == TRUE )
		{
			if( gObj[iIndex].pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				bChaos = TRUE;
			}
			else if( gObj[iIndex].pChaosBox[n].m_Type == ITEMGET(13,49) )
			{
				int iLevel = gObj[iIndex].pChaosBox[n].m_Level;
				iItemCount++;
				bScroll = TRUE;
				iScrollLevel = iLevel;
			}
			else if( gObj[iIndex].pChaosBox[n].m_Type == ITEMGET(13,50) )
			{
				int iLevel = gObj[iIndex].pChaosBox[n].m_Level;
				iItemCount++;
				bPotion = TRUE;
				iPotionLevel = iLevel;
			}
			else if( gObj[iIndex].pChaosBox[n].m_Type == ITEMGET(14,53) )
			{
				iCharmOfLuckCount += (int)gObj[iIndex].pChaosBox[n].m_Durability;
			}
			else
			{
				bOtherItem = TRUE;
			}
		}
	}

	if( bOtherItem != FALSE )
	{
		return 8;
	}

	if( bScroll == FALSE && bPotion == FALSE )
	{
		return 0;
	}

	if( bScroll == FALSE || bPotion == FALSE )
	{
		return 11;
	}

	if( iItemCount > 2 )
	{
		return 12;
	}

	if( iScrollLevel != iPotionLevel )
	{
		return 9;
	}

	if( IT_TEMPLE_RANGE( iScrollLevel - 1 ) == FALSE )
	{
		return 9;
	}

	if( IT_TEMPLE_RANGE( iPotionLevel - 1 ) == FALSE )
	{
		return 9;
	}

	if( bChaos == FALSE )
	{
		return 10;
	}

	if( gObj[iIndex].Level < 10 )
	{
		return 14;
	}

	if( bChaos != FALSE && bScroll != FALSE && bPotion != FALSE )
	{
		return iScrollLevel;
	}

return FALSE;
}

int CIllusionTempleEvent::CharmOfLuckCount(int iIndex)
{
	if( OBJMAX_RANGE(iIndex ) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return -1;
	}

	LPOBJ lpObj = &gObj[iIndex];
	int iCharmCount = 0;

	for(int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if( lpObj->pChaosBox[n].IsItem() == TRUE &&
			lpObj->pChaosBox[n].m_Type == ITEMGET(14,53) )
		{
			iCharmCount += (int)lpObj->pChaosBox[n].m_Durability;
		}
	}
return iCharmCount;
}


void CIllusionTempleEvent::SetStatueTime(BYTE MapNumber)
{
	if( IT_TEMPLE_RANGE( MapNumber - MAP_INDEX_ILLUSIONTEMPLE1 ) == FALSE )
	{
		return;
	}

	m_TempleData[MapNumber-MAP_INDEX_ILLUSIONTEMPLE1].SetStatueTime();
}

int CIllusionTempleEvent::ChangeTempleUserInformation(int iIndex,int iCurUserIndex,int iIllusionTempleIndex,unsigned char bMapNumber)
{
	return m_TempleData[bMapNumber-MAP_INDEX_ILLUSIONTEMPLE1].ChangeTempleUserInformation(iIndex,iIllusionTempleIndex,iCurUserIndex);
}

BYTE CIllusionTempleEvent::GetSkillPoint(int nIndex,unsigned char bMapNumber)
{
	return m_TempleData[bMapNumber-MAP_INDEX_ILLUSIONTEMPLE1].GetSkillPoint(nIndex);
}