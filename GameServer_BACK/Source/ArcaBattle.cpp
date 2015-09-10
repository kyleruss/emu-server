#include "stdafx.h"
#include "ArcaBattle.h"
#include "..\\Include\\ReadScript.h"
#include "LargeRand.h"
#include "BuffEffectSlot.h"
#include "GameMain.h"
#include "GuildClass.h"
#include "TNotice.h"
#include "Gate.h"
#include "DBSockMng.h"
#include "MapServerManager.h"
#include "DSProtocol.h"
#include "MasterSkillSystem.h"
// -------------------------------------------------------------------------------

CArcaBattle g_ArcaBattle; //Line: 30
// -------------------------------------------------------------------------------

bool g_bIsArcaBattleServer;	//Gamemain.obj
// -------------------------------------------------------------------------------

//0071b6b0
CArcaBattle::CArcaBattle()
{
	this->m_bReqABWinGuildInfo		= 0;
	this->m_iJoinGuildMemberCnt		= 0;
	this->m_dwAB_TICK_COUNT_SEC		= 0;
	//m_vtArcaBattleOpenTime
	this->m_iCurUserCount			= 0;
	this->m_iObeliskCnt				= 0;
	this->m_iArcaBattleMapIndex		= 0;
	this->m_iBootyMixCnt			= 0;
	this->m_iArcaBattleLoadState	= 0;
	this->m_dwDbDataLoadTickCount	= 0;
	this->m_iAB_RegTimeMin			= 0;
	this->m_iAB_PlayTimeMin			= 0;
	this->m_iAB_WaitTimeMin			= 0;
	this->m_iAB_ChannelTimeMin		= 0;
	this->m_dwAB_TICK_COUNT			= 0;
}
// -------------------------------------------------------------------------------

//0071b930
void CArcaBattle::Clear()
{
	this->m_iCurUserCount		= 0;
	ZeroMemory(&this->m_tmStartDate, sizeof(SYSTEMTIME));
	this->m_iAB_REMAIN_MSEC		= 0;
	this->m_iAB_TICK_COUNT		= 0;
	this->m_iAB_NOTIFY_COUNT	= 0;
	this->m_dwAB_TICK_COUNT_SEC = 0;
	this->m_bAB_CAN_ENTER		= 0;
	this->m_bAB_MSG_BEFORE_PLAY = 0;
	this->m_bAB_MSG_BEFORE_END	= 0;
	this->m_bAB_PLAY_START		= 0;
	this->m_bAns_PLAY_START		= 0;
	// ----
	this->SetRandomObeliskAttr();
	// ----
	int i = 0;
	// ----
	for( i = 0; i < AB_MAX_PLAYER; i++ )
	{
		this->m_UserData[i].Clear();
	}
	// ----
	for( i = 0; i < AB_MAX_INFOLOG; i++ )
	{
		this->m_stGuildMemberInfoLog[i].Clear();
	}
	// ----
	for( i = 0; i < AB_MAX_GATE; i++ )
	{
		this->m_stGMasterGateNum[i].Clear();
	} 
}
// -------------------------------------------------------------------------------

//0071d470
CArcaBattle::~CArcaBattle()
{
//	if( this->m_pAcheronProbability )
//	{
//		delete this->m_pAcheronProbability;
//	}
}
// -------------------------------------------------------------------------------

//0071d500
BOOL CArcaBattle::LoadScriptBootyMix(LPSTR lpszFileName)
{
	SMDFile = fopen(lpszFileName, "r");
	// ----
	if( !SMDFile )
	{
		MsgBox("[ArcaBattle][BootyExchange] %s File Load Error", lpszFileName);
		return false;
	}
	// ----
	int i = 0;
	SMDToken Token;
	int iCount = 0;
	int Type;
	// ----
	while(true)
	{
		Token = GetToken();
		// ----
		if( Token == END )
		{
			break;
		}
		// ----
		if( Token == NUMBER )
		{
			Type = (int)TokenNumber;
			// ----
			if( Type == 0 )
			{
				while(true)
				{
					if( Token == END )
					{
						if( !strcmp("end", TokenString) == 0 )
						{
							break;
						}
					}
					// ----
					this->m_stBootyMix[this->m_iBootyMixCnt].iItemLink	= (int)TokenNumber;
					Token = GetToken();
					this->m_stBootyMix[this->m_iBootyMixCnt].iMinCnt	= (int)TokenNumber;
					Token = GetToken();
					this->m_stBootyMix[this->m_iBootyMixCnt].iMaxCnt	= (int)TokenNumber;
					Token = GetToken();
					this->m_stBootyMix[this->m_iBootyMixCnt].iBaseRate	= (int)TokenNumber;
					Token = GetToken();
					this->m_stBootyMix[this->m_iBootyMixCnt++].iAddRate = (int)TokenNumber;
					// ----
					this->m_iBootyMixCnt++;
				}
			}
			else if( Type > 0 )
			{
				iCount = 0;
				// ----
				if( this->m_iBootyMixCnt < Type )
				{
					MsgBox("[ArcaBattle] BootyMix %s File Load Error - BootyMixCnt Over", lpszFileName);
					break;
				}
				// ----
				while(true)
				{
					Token = GetToken();
					// ----
					if( Token == END )
					{
						if( !strcmp("end", TokenString) == 0 )
						{
							break;
						}
					}
					// ----
					this->m_stBootyMix[Type - 1].stBootyReward[iCount].iRate		= (int)TokenNumber;
					Token = GetToken();
					this->m_stBootyMix[Type - 1].stBootyReward[iCount].iDurab		= (int)TokenNumber;
					Token = GetToken();
					this->m_stBootyMix[Type - 1].stBootyReward[iCount].iItemLevel	= (int)TokenNumber;
					Token = GetToken();
					this->m_stBootyMix[Type - 1].stBootyReward[iCount].iItemIndex	= (int)TokenNumber;
					Token = GetToken();
					this->m_stBootyMix[Type - 1].stBootyReward[iCount].iItemType	= (int)TokenNumber;
					// ----
					this->m_stBootyMix[Type - 1].stBootyReward[iCount].iItemLink	= Type;
					this->m_stBootyMix[Type - 1].iBootyRewardCnt++;
					iCount++;
				}
			}
		}
	}
	// ----
	fclose(SMDFile);
	return true;
}
// -------------------------------------------------------------------------------

//0071dc10
void CArcaBattle::BootyExchange(LPOBJ lpObj)
{
	return;
}
// -------------------------------------------------------------------------------

//0071c480
int CArcaBattle::GetBootyRewardItem(int iBootyCnt, _tagARCA_BATTLE_BOOTY_REWARD * pBootyRewardItem)
{
	int iTotRate, iBaseRate, iAddRate;
	int iRewardCnt;
	int i;
	int iLinkNum;
	int iRand;
	int iRewardItem;
	int iBootyRateCnt;
	// ----
	iLinkNum	= 0;
	i			= 0;
	iRewardCnt	= 0;
	iRewardItem = 0;
	_tagARCA_BATTLE_BOOTY_MIX * pBootMix = NULL;
	// ----
	for( i = 0; i < this->m_iBootyMixCnt; i++ )
	{
		if( this->m_stBootyMix[i].iMinCnt <= iBootyCnt && this->m_stBootyMix[i].iMaxCnt >= iBootyCnt )
		{
			pBootMix = &this->m_stBootyMix[i];
			break;
		}
	}
	// ----
	if( pBootMix == NULL )
	{
		LogAddTD("[ArcaBattle][BootyExchange] Error GetBootyRewardItem() NULL");
		return 0;
	}
	// ----
	iBaseRate		= pBootMix->iBaseRate;
	iAddRate		= pBootMix->iAddRate;
	iBootyRateCnt	= iBootyCnt - pBootMix->iMinCnt;
	iTotRate		= iAddRate * iBootyRateCnt + iBaseRate;
	iRand			= GetLargeRand() % 100;
	// ----
	if( iRand <= iTotRate )
    {
		int iSumRate;
		int iRewardCnt;
		int iRewardRand;
		// ----
		iRewardRand		= GetLargeRand() % 1000000;
		iRewardCnt		= pBootMix->iBootyRewardCnt;
		iSumRate		= 0;
		// ----
		for( int RateCnt = 0; RateCnt < iRewardCnt; RateCnt++ )
		{
			iSumRate += pBootMix->stBootyReward[RateCnt].iRate; //Need check
			// ----
			if( iRewardRand <= iSumRate )
			{
				iRewardItem = RateCnt;
				break;
			}
		}
	}
	else
	{
		return 2;
	}
	// ----
	if( iRewardItem >= 20 )
    {
		LogAddTD("[ArcaBattle][BootyExchange] Error Overflow iRewardItem");
		return 0;
	}
	// -----
	pBootyRewardItem->iItemLink		= pBootMix->stBootyReward[iRewardItem].iItemLink;
	pBootyRewardItem->iItemType		= pBootMix->stBootyReward[iRewardItem].iItemType;
	pBootyRewardItem->iItemIndex	= pBootMix->stBootyReward[iRewardItem].iItemIndex;
	pBootyRewardItem->iItemLevel	= pBootMix->stBootyReward[iRewardItem].iItemLevel;
	pBootyRewardItem->iDurab		= pBootMix->stBootyReward[iRewardItem].iDurab;
	pBootyRewardItem->iRate			= pBootMix->stBootyReward[iRewardItem].iRate;
	// ----
	return 1;
}
// -------------------------------------------------------------------------------

//0071e370
BOOL CArcaBattle::LoadScriptArcaBattle(LPSTR lpszFileName)
{
	return 0;
}
// -------------------------------------------------------------------------------

//0071ef70
void CArcaBattle::SendArcaBattlePlayInfo(OBJECTSTRUCT& obj, WORD wGuildGroupNum)
{
	if( gObj[obj.m_Index].Connected < PLAYER_LOGGED )
	{
		return;
	}
	// ----
	_tagPMSG_ARCA_BATTLE_PLAY_INFO pMsg;
	// ----
	pMsg.h.c			= 0xC1;
    pMsg.h.size			= sizeof(_tagPMSG_ARCA_BATTLE_PLAY_INFO);
    pMsg.h.headcode		= 0xF8;
    pMsg.h.subcode		= 0x39;
    pMsg.wGuildGroupNum = wGuildGroupNum;
	// ----
	for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
	{
		pMsg.m_stObeliskInfo[i].iGroupNumber	= this->m_stObeliskInfo[i].m_iGroupNumber;
		pMsg.m_stObeliskInfo[i].iAttrKind		= this->m_stObeliskInfo[i].m_iAttrKind;
		pMsg.m_stObeliskInfo[i].iMapIndex		= this->m_stObeliskInfo[i].m_iMapIndex;
		pMsg.m_stObeliskInfo[i].iPosX			= this->m_stObeliskInfo[i].m_iPosX;
		pMsg.m_stObeliskInfo[i].iPosY			= this->m_stObeliskInfo[i].m_iPosY;
		// ----
		LogAdd("Send ObeliskInfo iGroupNumber %d iAttr %d iMapIndex %d iPosX %d iPosY %d",
			pMsg.m_stObeliskInfo[i].iGroupNumber, pMsg.m_stObeliskInfo[i].iAttrKind, 
			pMsg.m_stObeliskInfo[i].iMapIndex, pMsg.m_stObeliskInfo[i].iPosX, pMsg.m_stObeliskInfo[i].iPosY);
		// ----
		for( int j = 0; j < AB_MAX_AURAPOS; j++ )
		{
			pMsg.m_stObeliskInfo[i].m_StAuraInfo[j].m_iAuraPosX = this->m_stObeliskInfo[i].m_stAuraInfo[j].m_iAuraPosX;
			pMsg.m_stObeliskInfo[i].m_StAuraInfo[j].m_iAuraPosY = this->m_stObeliskInfo[i].m_stAuraInfo[j].m_iAuraPosY;
			// ----
			 LogAdd("Send ObeliskInfo iGroupNumber m_iAuraPosX %d m_iAuraPosY %d",
				pMsg.m_stObeliskInfo[i].m_StAuraInfo[j].m_iAuraPosX,
				pMsg.m_stObeliskInfo[i].m_StAuraInfo[j].m_iAuraPosY);
		}
	}
	// ----
	LogAdd("[ArcaBattle] Send Play Data");
    DataSend(obj.m_Index, (LPBYTE)&pMsg, pMsg.h.size);
}
// -------------------------------------------------------------------------------

//0071f240
void CArcaBattle::WinGuildMemberAddBuff(LPOBJ lpObj, WORD wObeliskAttr)
{
//	_tagPeriodBuffEffectInfo * lpPeriBuff;
	WORD wBuffIndex;
	int iDuration	= 604800;
	wBuffIndex		= this->GetObeliskAttrToRewardBuffIndex(wObeliskAttr);
//	lpPeriBuff		= 0;
//	lpPeriBuff		= g_BuffEffectSlot.GetPeriodBuffInfo(wBuffIndex);
	// ----
//	if( lpPeriBuff == NULL )
//	{
//		return;
//	}
	// ----
//    gObjAddPeriodBuffEffect(lpObj, lpPeriBuff, iDuration);
}
// -------------------------------------------------------------------------------

//0071f2a0
void CArcaBattle::GiveRewardBuffWinningGuild()
{
	WORD wBuffIndex = 0;
	int iGuildCnt	= 0;
	// ----
	for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
	{
		if( this->m_stObeliskState[i].m_iObeliskState == AB_OBSTATE_OCCUPY )
		{
			wBuffIndex = this->GetObeliskAttrToRewardBuffIndex(this->m_stObeliskInfo[i].m_iAttrKind);
			// ----
			if( wBuffIndex == 0 )
			{
				LogAddTD("[ArcaBattle] Error Obelisk Attribute %d", this->m_stObeliskInfo[i].m_iAttrKind);
			}
			// ----
			_GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(this->m_stObeliskState[i].m_szOccupyGuildName);
			// ----
			if( lpGuild == NULL )
			{
				LogAddTD("[ArcaBattle][Error] GiveRewardBuffWinningGuild() GuildInfo is NULL GuildName : %s ObeliskAttr : %d ObeliskGroup %d",
					this->m_stObeliskState[i].m_szOccupyGuildName, this->m_stObeliskInfo[i].m_iAttrKind, this->m_stObeliskInfo[i].m_iGroupNumber);
				continue;
			}
			// ----
//			_tagPeriodBuffInfo BuffInfo;
			int iDuration = AB_REW_BUFF_TIME;
			// ----
//			BuffInfo.btEffectType1	= 0;
//			BuffInfo.btEffectType2	= 0;
//			BuffInfo.wBuffIndex		= wBuffIndex;
//			BuffInfo.lDuration		= iDuration;
			// ----
//			g_BuffEffect.RequestGuildPeriodBuffInsert(this->m_stObeliskState[i].m_szOccupyGuildName, &BuffInfo); //New function in BuffEffect
			LogAddTD("[ArcaBattle] Reward GuildBuff GuildName[%s] BuffIndex[%d]", this->m_stObeliskState[i].m_szOccupyGuildName, wBuffIndex);
		}
	}
}
// -------------------------------------------------------------------------------

//0071f400
void CArcaBattle::ReqRemoveRewardGuildBuff(LPSTR szGuildName)
{
	WORD wBuffIndex = 0;
	// ----
	for( int i = 0; i < this->m_iWinGuildCnt; i++ )
	{
		if( !strcmp(this->m_stABWinGuildInfo[i].szGuildName, szGuildName) )
		{
			wBuffIndex = this->GetObeliskAttrToRewardBuffIndex(this->m_stABWinGuildInfo[i].wOccupyObelisk);
		}
	}
	// ----
//	g_BuffEffect.RequestGuildPeriodBuffDelete(&wBuffIndex, 1);
	LogAddTD("[ArcaBAttle] Request ExDB Delete Reward Guild Buff");
}
// -------------------------------------------------------------------------------

//0071f4a0
void CArcaBattle::ReqRemoveRewardBuffWinningGuild()
{
	WORD wBuffIndex[AB_MAX_BATTLEPOS] = { 179, 180, 181, 182, 183 };
//	g_BuffEffect.RequestGuildPeriodBuffDelete(wBuffIndex, AB_MAX_BATTLEPOS);
	LogAddTD("[ArcaBAttle] Request ExDB Delete All Reward Guild Buff");
	// ----
	_tagPMSG_REQ_REMOVE_ALL_GUILD_BUFF_DS pMsg;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_REQ_REMOVE_ALL_GUILD_BUFF_DS);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x47;
	pMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();
	// ----
	cDBSMng.Send((PCHAR)&pMsg, sizeof(_tagPMSG_REQ_REMOVE_ALL_GUILD_BUFF_DS));
	LogAddTD("[ArcaBAttle] Request MultiCast Remove All Reward Guild Buff");
}
// -------------------------------------------------------------------------------

//0071f550
void CArcaBattle::DGAnsRemoveRewardBuff(_tagPMSG_ANS_REMOVE_GUILD_BUFF_DS * lpMsg)
{
	WORD wBuffIndex = 0;
	// ----
	for( int i = 0; i < this->m_iWinGuildCnt; i++ )
	{
		if( !strcmp(this->m_stABWinGuildInfo[i].szGuildName, lpMsg->szGuildName) )
		{
			wBuffIndex = this->GetObeliskAttrToRewardBuffIndex(this->m_stABWinGuildInfo[i].wOccupyObelisk);
			this->RemoveGuildBuff(this->m_stABWinGuildInfo[i].szGuildName, wBuffIndex);
			this->m_stABWinGuildInfo[i].Clear();
		}
	}
}
// -------------------------------------------------------------------------------

//0071f600
void CArcaBattle::RemoveRewardBuff(LPSTR szGuildName)
{
	WORD wBuffIndex = 0;
	// ----
	for( int i = 0; i < this->m_iWinGuildCnt; i++ )
	{
		if( !strcmp(this->m_stABWinGuildInfo[i].szGuildName, szGuildName) )
		{
			wBuffIndex = this->GetObeliskAttrToRewardBuffIndex(this->m_stABWinGuildInfo[i].wOccupyObelisk);
			this->RemoveGuildBuff(this->m_stABWinGuildInfo[i].szGuildName, wBuffIndex);
			this->m_stABWinGuildInfo[i].Clear();
		}
	}
}
// -------------------------------------------------------------------------------

//0071f6b0
void CArcaBattle::DGAnsRemoveAllRewardBuff()
{
	WORD wBuffIndex = 0;
	// ----
	for( int i = 0; i < this->m_iWinGuildCnt; i++ )
	{
		wBuffIndex = this->GetObeliskAttrToRewardBuffIndex(this->m_stABWinGuildInfo[i].wOccupyObelisk);
		this->RemoveGuildBuff(this->m_stABWinGuildInfo[i].szGuildName, wBuffIndex);
		LogAddTD("[ArcaBattle] Remove All Reward Guild Buff GuildName[%s] BuffIndex[%d]", this->m_stABWinGuildInfo[i].szGuildName, wBuffIndex);
		this->m_stABWinGuildInfo[i].Clear();
	}
}
// -------------------------------------------------------------------------------

//0071f760
void CArcaBattle::RemoveGuildBuff(LPSTR szGuildName, WORD wBuffIndex)
{
	_GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(szGuildName);
	// ----
	if( lpGuild == NULL )
	{
		LogAddTD("[ArcaBattle] Error Can Not Find Guild RemoveGuildBuff() lpGuild NULL GuildName : %s", szGuildName);
		return;
	}
	// ----
	for( int nGuildCnt = 0; nGuildCnt < lpGuild->Count; nGuildCnt++ )
    {
		int iIndex = lpGuild->Index[nGuildCnt];
		// ----
		if( gObjIsConnected(iIndex) == 1 && gObjCheckUsedBuffEffect(&gObj[iIndex], wBuffIndex) == true )
		{
			gObjRemoveBuffEffect(&gObj[iIndex], wBuffIndex);
			gObjCalCharacter(iIndex);
			LogAddTD("[ArcaBattle] Remove Buff [%s][%s][%s] GuildName [%s] BuffIndex [%d]",
				gObj[iIndex].AccountID, gObj[iIndex].Name, gObj[iIndex].GuildName, szGuildName, wBuffIndex);
		}
	}
}
// -------------------------------------------------------------------------------

//0071f890
void CArcaBattle::ReqGuildMemberDelBuffDel(LPOBJ lpObj, LPSTR szGuildName)
{
	for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
	{
		if( !strcmp(this->m_stABWinGuildInfo[i].szGuildName, szGuildName) )
		{
			WORD wBuffIndex = this->GetObeliskAttrToRewardBuffIndex(this->m_stABWinGuildInfo[i].wOccupyObelisk);
			// ----
			if( gObjCheckUsedBuffEffect(lpObj, wBuffIndex) == true )
			{
//				g_BuffEffect.RequestPeriodBuffDelete(lpObj, wBuffIndex);
				gObjRemoveBuffEffect(lpObj, wBuffIndex);
				gObjCalCharacter(lpObj->m_Index);
			}
		}
	}
}
// -------------------------------------------------------------------------------

//0071f950
WORD CArcaBattle::GetObeliskAttrToRewardBuffIndex(int iObeliskAttr)
{
	WORD wBuffIndex = 0;
	// ----
	switch(iObeliskAttr)
	{
	case AB_OBELISK_FIRE:
		wBuffIndex = 179;
		break;
	case AB_OBELISK_WATER:
		wBuffIndex = 180;
		break;
	case AB_OBELISK_WIND:
		wBuffIndex = 181;
		break;
	case AB_OBELISK_EARTH:
		wBuffIndex = 182;
		break;
	case AB_OBELISK_DARKNESS:
		wBuffIndex = 183;
		break;
	}
	// ----
	return wBuffIndex;
}
// -------------------------------------------------------------------------------

//0071f9e0
void CArcaBattle::AddArcaBattleWinGuildInfo(_stABWinGuildInfoDS * pABWinGuildInfoDS, int iGuildCnt)
{
	for( int i = 0; i < this->m_iWinGuildCnt; i++ )
	{
		memcpy(this->m_stABWinGuildInfo[i].szGuildName, pABWinGuildInfoDS[i].szGuildName, 9); //MAX_GUILD_NAME define
		this->m_stABWinGuildInfo[i].dwGuild			= pABWinGuildInfoDS[i].dwGuild;
		this->m_stABWinGuildInfo[i].wOccupyObelisk	= pABWinGuildInfoDS[i].wOccupyObelisk;
		this->m_stABWinGuildInfo[i].wObeliskGroup	= pABWinGuildInfoDS[i].wObeliskGroup;
		// ----
		_GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(this->m_stABWinGuildInfo[i].szGuildName);
		// ----
		if( lpGuild != NULL )
		{
			for( int nGuildCnt = 0; nGuildCnt < lpGuild->Count; nGuildCnt++ )
			{
				if( gObjIsConnected(lpGuild->Index[nGuildCnt]) == 1 )
				{
					this->WinGuildMemberAddBuff(&gObj[lpGuild->Index[nGuildCnt]], this->m_stABWinGuildInfo[i].wOccupyObelisk);
				}
			}
		}
		// ----
		LogAddTD("[ArcaBattle] Add Winning Guild Info GuildName[%s] ObeliskAttr[%d] ObeliskGroup[%d]",
			this->m_stABWinGuildInfo[i].szGuildName, this->m_stABWinGuildInfo[i].wOccupyObelisk,
			this->m_stABWinGuildInfo[i].wObeliskGroup);
	}
}
// -------------------------------------------------------------------------------

//0071fb90
BOOL CArcaBattle::IsArcaBattleWinGuild(OBJECTSTRUCT & obj)
{
	if( obj.lpGuild == NULL )
	{
		return false;
	}
	// ----
	for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
	{
		if( !strcmp(obj.lpGuild->Name, this->m_stABWinGuildInfo[i].szGuildName) )
		{
			if( obj.lpGuild->Number == this->m_stABWinGuildInfo[i].dwGuild )
			{
				return true;
			}
		}
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

//0071fc20
BOOL CArcaBattle::IsArcaBattleOccupyZone(OBJECTSTRUCT & obj, int gt)
{
	if( obj.lpGuild == NULL )
	{
		return false;
	}
	// ---
	if( gt != 418 && gt != 422 )
    {
		return false;
	}
	// ----
	for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
	{
		if( !strcmp(obj.lpGuild->Name, this->m_stABWinGuildInfo[i].szGuildName) )
		{
			if(		gt == 418 && this->m_stABWinGuildInfo[i].wObeliskGroup == 1 
				||	gt == 422 && this->m_stABWinGuildInfo[i].wObeliskGroup == 2 )	//Maybe it obelist attr (dark, earth...)
			{
				return true;
			}
		}
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

//0071fcf0
void CArcaBattle::SendArcaBattleOccupyZone(OBJECTSTRUCT & obj, int gt)
{
	_tagPMSG_SEND_AB_WIN_GUILD_INFO pMsg;
	bool bSend = false;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_SEND_AB_WIN_GUILD_INFO);
	pMsg.h.headcode	= 0xF8;
	pMsg.h.subcode	= 0x3D;
	// ----
	if( gt == 418 || gt == 422 )
	{
		for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
		{
			if(		gt == 418 && this->m_stABWinGuildInfo[i].wObeliskGroup == 1 
				||	gt == 422 && this->m_stABWinGuildInfo[i].wObeliskGroup == 2 )	//Maybe it obelist attr (dark, earth...)
			{
				memcpy(pMsg.szGuildName, this->m_stABWinGuildInfo[i].szGuildName, 9); //MAX_GUILD_NAME define
				DataSend(obj.m_Index, (LPBYTE)&pMsg, sizeof(_tagPMSG_SEND_AB_WIN_GUILD_INFO));
				break;
			}
		}
	}
}
// -------------------------------------------------------------------------------

//0071fdd0
void CArcaBattle::EnableEvent(BOOL bDoEvent)
{
	//state defines!!
	this->m_bDoEvent = bDoEvent;
	// ----
	if( bDoEvent == 1 )
	{
		this->SetState(1);	
		this->GDReqArcaBattleProcMultiCast(1);
	}
	else
	{
		this->SetState(0);
		this->GDReqArcaBattleProcMultiCast(0);
	}
}
// -------------------------------------------------------------------------------

//0071fe20
BOOL CArcaBattle::CheckSync()
{
	return 0;
}
// -------------------------------------------------------------------------------

//00720190
void CArcaBattle::SetState(int iAB_State)	//Need compare strange switch
{
	this->m_iAB_State = iAB_State;
	// ----
	if( !this->IsArcaBattleServer() )
	{
		return;
	}
	// ----
	this->SendArcaBattleStateAll(this->m_iAB_State);
	// ----
	switch(this->m_iAB_State)
	{
	case AB_STATE_CLOSED:
		this->SetStateClosed();
		break;
	case AB_STATE_READY:
		this->SetStateReady();
		break;
	case AB_STATE_END:
		this->SetStatePlayEnd();
		break;
	}
}
// -------------------------------------------------------------------------------

//00720210
void CArcaBattle::SetStateReady()
{
	this->GDReqArcaBattleJoinMemberUnderReq();
}
// -------------------------------------------------------------------------------

//00720230
void CArcaBattle::SetStateClosed()
{
	//Unfilled
}
// -------------------------------------------------------------------------------

//00720240
void CArcaBattle::SetStatePlayEnd()
{
	this->m_bAB_CAN_ENTER = false;
	// ----
	for( int i = 0; i < AB_MAX_MONGROUP; i++ )
	{
		if( this->m_stMonsterGroupInfo[i].m_nGroupNumber > -1 )
		{
			this->DeleteMonster(i);
		}
	}
}
// -------------------------------------------------------------------------------

//007202a0
int CArcaBattle::GetState()
{
	return this->m_iAB_State;
}
// -------------------------------------------------------------------------------

//007202c0
int CArcaBattle::GetDataLoadState()
{
	return this->m_iArcaBattleLoadState;
}
// -------------------------------------------------------------------------------

//007202e0
void CArcaBattle::SetDataLoadState(int iDataLoadState)
{
	this->m_iArcaBattleLoadState = iDataLoadState;
}
// -------------------------------------------------------------------------------

//00720300
BOOL CArcaBattle::DBDataRequest()
{
	BOOL bDbRequestData = false;
	// ----
	if( !this->m_dwDbDataLoadTickCount )
	{
		bDbRequestData = true;
		this->m_dwDbDataLoadTickCount = GetTickCount();
		LogAddC(3, "[ArcaBattle] CArcaBattle::DBDataRequest() - REQUEST DATA FIRST");
	}
	else if( GetTickCount() - this->m_dwDbDataLoadTickCount > 3000 )
	{
		bDbRequestData = true;
		this->m_dwDbDataLoadTickCount = GetTickCount();
		LogAddC(2, "[ArcaBattle] CArcaBattle::DBDataRequest() - REQUEST DATA AGAIN");
	}
	// ----
	return true;
}
// -------------------------------------------------------------------------------

//00720390
void CArcaBattle::ProcStateClosed()
{
	int iTICK_MSEC;
	char szTemp[256] = { 0 };	//xor by inizilization?
	// ----
	if( this->m_iAB_REMAIN_MSEC == -100 )
	{
		return;
	}
	// ----
	iTICK_MSEC = GetTickCount() - this->m_iAB_TICK_COUNT;
	// ----
	if( iTICK_MSEC >= 1000 )
	{
		this->m_iAB_REMAIN_MSEC -= iTICK_MSEC;
		this->m_iAB_TICK_COUNT	= GetTickCount();
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 0 )
		{
			this->GDReqDeleteArcaBattleInfo();
			this->ReqRemoveRewardBuffWinningGuild();
			// ----
			this->m_bAB_CAN_ENTER		= true;
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
			// ----
			memset(szTemp, 0, sizeof(szTemp));
			wsprintf(szTemp, lMsg.Get(3490)); //#TEXT
			this->SendMapServerGroupMsg(szTemp);
			// ----
			this->m_iAB_REMAIN_MSEC		= 60000 * this->m_iAB_MasterRegMin;
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
			// ----
			LogAddTD("[ArcaBattle] ArcaBattle Start");
			this->SetState(AB_STATE_START);
			this->GDReqArcaBattleProcMultiCast(AB_STATE_START);
		}
	}
}
// -------------------------------------------------------------------------------

//00720530
void CArcaBattle::ProcStateMasterReg()
{
	int iTICK_MSEC = GetTickCount() - this->m_iAB_TICK_COUNT;
	// ----
	if( iTICK_MSEC >= 1000 )
	{
		this->m_iAB_REMAIN_MSEC -= iTICK_MSEC;
		this->m_iAB_TICK_COUNT	= GetTickCount();
		int iAB_REMAIN_MIN		= this->m_iAB_REMAIN_MSEC / 60000;
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 60000 * this->m_iAB_MasterRegMin )
		{
			if( GetTickCount() - this->m_dwAB_TICK_COUNT > 60000 )
			{
				this->m_iAB_NOTIFY_COUNT	= iAB_REMAIN_MIN;
				this->m_dwAB_TICK_COUNT		= GetTickCount();
				// ----
				if( this->m_iAB_REMAIN_MSEC > 900000 )
				{
					if( this->m_iAB_REMAIN_MSEC > 900000 && ((iAB_REMAIN_MIN + 1) % 5) == 0 )
					{
						char szTemp[256];
						memset(szTemp, 0, sizeof(szTemp));
						wsprintf(szTemp, lMsg.Get(3491), this->m_iAB_NOTIFY_COUNT + 1);
						this->SendMapServerGroupMsg(szTemp);
						LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ -ö¦õLõ ¦þ¬¦ -+-ãL¦ %d¦¦ ¦--T-L+¦+-.", this->m_iAB_NOTIFY_COUNT + 1);
					}
				}
				else
				{
					char szTemp[256];
					memset(szTemp, 0, sizeof(szTemp));
					wsprintf(szTemp, lMsg.Get(3491), this->m_iAB_NOTIFY_COUNT + 1);
					this->SendMapServerGroupMsg(szTemp);
					LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ -ö¦õLõ ¦þ¬¦ -+-ãL¦ %d¦¦ ¦--T-L+¦+-.", this->m_iAB_NOTIFY_COUNT + 1);
				}
			}
		}
	}
	// ----
	if( this->m_iAB_REMAIN_MSEC <= 0 )
	{
		this->m_dwAB_TICK_COUNT		= 0;
		this->m_iAB_REMAIN_MSEC		= 60000 * this->m_iAB_RegTimeMin;
		this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
		this->SetState(AB_STATE_REG);
		this->GDReqArcaBattleProcMultiCast(AB_STATE_REG);
		LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ -ö¦õ¬° ¦þ¬¦ -+L-");
	}
}
// -------------------------------------------------------------------------------

//007207f0
void CArcaBattle::ProcStateReg()
{
	int iTICK_MSEC = GetTickCount() - this->m_iAB_TICK_COUNT;
	// ----
	if( iTICK_MSEC >= 1000 )
	{
		this->m_iAB_REMAIN_MSEC	-= iTICK_MSEC;
		this->m_iAB_TICK_COUNT	= GetTickCount();
		// ----
		if( GetTickCount() - this->m_dwAB_TICK_COUNT > 60000 )
		{
			this->m_dwAB_TICK_COUNT		= GetTickCount();
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
			// ----
			char szTemp[256];
			memset(szTemp, 0, sizeof(szTemp));
			wsprintf(szTemp, lMsg.Get(3492), this->m_iAB_NOTIFY_COUNT + 1);
			this->SendMapServerGroupMsg(szTemp);
			LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ -ö¦õ¬° ¦þ¬¦ -+-ãL¦ %d¦¦ ¦--T-L+¦+-.", this->m_iAB_NOTIFY_COUNT + 1);
			// ----
			if( this->m_iAB_NOTIFY_COUNT == 9 || this->m_iAB_NOTIFY_COUNT == 4 )
			{
				this->GDReqJoinMemberUnder();
#if( AB_DEBUG_TRACE == 1 )
				TRACE_LOG("-ö¦õ¬° T¹¬é ¦¦¬+");
#endif
			}
		}
	}
	// ----
	if( this->m_iAB_REMAIN_MSEC <= 0 )
	{
		this->m_dwAB_TICK_COUNT		= 0;
		this->m_iAB_REMAIN_MSEC		= 60000 * this->m_iAB_WaitTimeMin;
		this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
		this->SetState(AB_STATE_READY);
		this->GDReqArcaBattleProcMultiCast(AB_STATE_READY);
		LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ ¦þ¬¦ ¬¦¬ñ");
	}
}
// -------------------------------------------------------------------------------

//00720a20
void CArcaBattle::ProcStateReady()
{
	int iTICK_MSEC = GetTickCount() - this->m_iAB_TICK_COUNT;
	// ----
	if( iTICK_MSEC >= 1000 )
	{
		this->m_iAB_NOTIFY_COUNT = this->m_iAB_REMAIN_MSEC / 1000;
		// ----
		if( this->m_iAB_NOTIFY_COUNT > 0 )
		{
			char szTemp[256];
			memset(szTemp, 0, sizeof(szTemp));
			wsprintf(szTemp, lMsg.Get(3493), this->m_iAB_NOTIFY_COUNT);
			this->SendMapServerGroupMsg(szTemp);
			LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ -+L--þ+¡ %d+¦ ¦--T-L+¦+-. -LL¹¦T +¡¬ê¬á-í +û-ò¦+ +ã-¦-+¬ô.", this->m_iAB_NOTIFY_COUNT);
		}
		// ----
		this->m_iAB_REMAIN_MSEC -= iTICK_MSEC;
		this->m_iAB_TICK_COUNT	= GetTickCount();
	}
	// ----
	if( this->m_iAB_REMAIN_MSEC <= 0 )
	{
		this->m_dwAB_TICK_COUNT		= 0;
		this->m_iAB_REMAIN_MSEC		= 60000 * this->m_iAB_WaitPartyTimeMin;
		this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
		this->SetState(AB_STATE_PATY);
		this->GDReqArcaBattleProcMultiCast(AB_STATE_PATY);
	}
}
// -------------------------------------------------------------------------------

//00720bc0
void CArcaBattle::ProcStatePaty()
{
	int iTICK_MSEC = GetTickCount() - this->m_iAB_TICK_COUNT;
	// ----
	if( iTICK_MSEC >= 1000 )
	{
		this->m_iAB_REMAIN_MSEC -= iTICK_MSEC;
		this->m_iAB_TICK_COUNT	= GetTickCount();
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 60000 * this->m_iAB_WaitPartyTimeMin && GetTickCount() - this->m_dwAB_TICK_COUNT > 60000 )
		{
			this->m_dwAB_TICK_COUNT		= GetTickCount();
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
			// ----
			char szTemp[256];
			memset(szTemp, 0, sizeof(szTemp));
			wsprintf(szTemp, lMsg.Get(3494));
			this->SendMapServerGroupMsg(szTemp);
			LogAddTD("[ArcaBattle] -¦¬ã-ëLÿL¦ ¬í¬L-L+¦+-. ¬¦-¦¦¦-ö NPC¬æ +û¦+ LLLõL¦ -á+ã¦-+¦+-.");
			// ----
			PMSG_NOTICE pMsg;	//Need check conversion
			TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3505));
			TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pMsg);
		}
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 30000 && this->m_iAB_REMAIN_MSEC >= 0 )
		{
			this->m_dwAB_TICK_COUNT		= GetTickCount();
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 1000;
			// ----
			PMSG_NOTICE pMsg;	//Need check conversion
			TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3506));
			TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pMsg);
		}
	}
	// ----
	if( this->m_iAB_REMAIN_MSEC <= 0 )
	{
		this->m_dwAB_TICK_COUNT			= 0;
		this->m_iAB_PlayNotifyTimeMin	= AB_TIME_PLAY_NOT;
		this->m_iAB_REMAIN_MSEC			= 60000 * this->m_iAB_PlayTimeMin;
		this->m_iAB_NOTIFY_COUNT		= this->m_iAB_REMAIN_MSEC / 60000;
		this->SetState(AB_STATE_PLAY);
		this->GDReqArcaBattleProcMultiCast(AB_STATE_PLAY);
		this->SetObelisk();
		LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ ¦-¦- +û-ò ¬¦¬ñ");
	}
}
// -------------------------------------------------------------------------------

//00720ef0
void CArcaBattle::ProcStatePlaying()
{
	int iTICK_MSEC = GetTickCount() - this->m_iAB_TICK_COUNT;
	// ----
	if( iTICK_MSEC >= 1000 )
	{
		this->m_iAB_REMAIN_MSEC -= iTICK_MSEC;
		this->m_iAB_TICK_COUNT	= GetTickCount();
		// ----
		if(		this->m_iAB_REMAIN_MSEC >= 60000 * (this->m_iAB_PlayTimeMin - this->m_iAB_PlayNotifyTimeMin) 
			&&	GetTickCount() - this->m_dwAB_TICK_COUNT_SEC >= 70000 )
		{
			this->m_dwAB_TICK_COUNT_SEC = GetTickCount();
			// ----
			PMSG_NOTICE pMsg;
			TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3495));
			TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pMsg);
			LogAddTD("[ArcaBattle]¬÷¦ã+=¬á ¬L¦ç¬î-¦+é-á ¬¤-¦¦¦-·-L+¦+-. ¬L¦ç¬î-¦+é¬æ +á¬ã¦+--¬¬ -¦¬ã-ëLÿ¬á-í -T¬î¦T -¡ Lã-L+¦+-.");
		}
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 60000 * this->m_iAB_PlayTimeMin && GetTickCount() - this->m_dwAB_TICK_COUNT >= 60000 )
		{
			this->m_dwAB_TICK_COUNT		= GetTickCount();
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
			// ----
			PMSG_NOTICE pMsg;
			TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3496));
			TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pMsg);
			LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ +-¬ñ-þ+¡ %d¦¦ ¦--T-L+¦+-.", this->m_iAB_NOTIFY_COUNT + 1);
		}
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 60000 && this->m_iAB_REMAIN_MSEC >= 0 )
		{
			this->m_dwAB_TICK_COUNT		= GetTickCount();
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 1000;
			// ----
			PMSG_NOTICE pMsg;
			TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3497));
			TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pMsg);
			LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ +-¬ñ-þ+¡ %d+¦ ¦--T-L+¦+-.", this->m_iAB_NOTIFY_COUNT + 1);
		}
		// ----
		this->ChkAuraUserHover();
		this->RegenMonsterRun();
		this->SendObeliskLife();
		this->SendObeliskState();
		this->RegenObelisk();
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 0 )
		{
			this->m_dwAB_TICK_COUNT		= 0;
			this->m_iAB_REMAIN_MSEC		= AB_TIME_REMAIN;
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
			// ----
			this->SetState(AB_STATE_RESULT);
			this->GDReqArcaBattleProcMultiCast(AB_STATE_RESULT);
			this->NotifyArcaBattle(4);	//#DEFINE
			this->DeleteArcaBattleAllMonster();
		}
	}
}
// -------------------------------------------------------------------------------

//007212e0
void CArcaBattle::ProcStateResult()
{
	int iTICK_MSEC = GetTickCount() - this->m_iAB_TICK_COUNT;
	// ----
	if( iTICK_MSEC >= 1000 )
	{
		this->m_iAB_REMAIN_MSEC -= iTICK_MSEC;
		this->m_iAB_TICK_COUNT	= GetTickCount();
		// ----
		if( this->m_iAB_REMAIN_MSEC > 0 )
		{
			this->m_iAB_NOTIFY_COUNT = this->m_iAB_REMAIN_MSEC / 1000;
			// ----
			PMSG_NOTICE pMsg;
			TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3498));
			TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pMsg);
			LogAddTD("[ArcaBattle] -¦¬ã-ëLÿL¦ +-¬ñ¦¦-·-L+¦+-. %d+¦ L- ¬¦L¬¬+ L¦¦¬ ¦-+¦+-.", this->m_iAB_NOTIFY_COUNT + 1);
		}
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 0 )
		{
			this->m_dwAB_TICK_COUNT		= 0;
			this->m_iAB_REMAIN_MSEC		= 60000 * this->m_iAB_ChannelTimeMin;
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
			// ---- Only for position:
			int iMapNumber;	//ebp-11c
			short sX;	//ebp-120
			BYTE btDIR;	//ebp-121
			short sLVL;	//ebp-128
			int iGateNum;	//ebp-12c
			short sY;	//ebp-130
			BYTE btMAPNUM;	//ebp-131
			// ----
			iGateNum	= 426;
			btMAPNUM	= 92;
			//iMapNumber	= gGateC.GetGate(iGateNum, (short&)sX, (short&)sY, (BYTE&)btMAPNUM, (BYTE&)btDIR, (short&)sLVL);
			iMapNumber	= gGateC.GetGate(iGateNum, (BYTE&)sX, (BYTE&)sY, (BYTE&)btMAPNUM, (BYTE&)btDIR, (short&)sLVL);
			// ----
			if( iMapNumber >= 0 )
			{
				for( int n = OBJ_STARTUSERINDEX; n < OBJMAX; n++ )
				{
					if( gObj[n].Connected == PLAYER_PLAYING )
					{
						if( gObj[n].Type == OBJ_USER )
						{
							gObjTeleport(n, btMAPNUM, sX, sY);
						}
					}
				}
			}
			// ----
			this->GDReqWinGuildInfoInsert();
			this->SendPlayResult();
			this->GiveRewardBuffWinningGuild();
			this->SetState(AB_STATE_CLOSE);
			this->GDReqArcaBattleProcMultiCast(AB_STATE_CLOSE);
		}
	}
}
// -------------------------------------------------------------------------------

//007215a0
void CArcaBattle::ProcStateChannelClose()
{
	int iTICK_MSEC = GetTickCount() - this->m_iAB_TICK_COUNT;
	// ----
	if( iTICK_MSEC >= 1000 )
	{
		this->m_iAB_REMAIN_MSEC -= iTICK_MSEC;
		this->m_iAB_TICK_COUNT	= GetTickCount();
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 60000 * this->m_iAB_ChannelTimeMin && GetTickCount() - this->m_dwAB_TICK_COUNT >= 60000 )
		{
			this->m_dwAB_TICK_COUNT		= GetTickCount();
			this->m_iAB_NOTIFY_COUNT	= this->m_iAB_REMAIN_MSEC / 60000;
			// ----
			PMSG_NOTICE pMsg;
			TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3499));
			TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pMsg);
			LogAddTD("-¦¬ã-ëLÿ +ä¦+ +-¬ñ -þ+¡ %d¦¦ ¦--T-L+¦+-. +-¬å +ä¦+¬+ L¦¦¬¦+ +ã-¦-+¬L.", this->m_iAB_NOTIFY_COUNT + 1);
		}
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 30000 && this->m_iAB_REMAIN_MSEC >= 0 )
		{
			this->m_iAB_NOTIFY_COUNT = this->m_iAB_REMAIN_MSEC / 1000;
			// ----
			PMSG_NOTICE pMsg;
			TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3500));
			TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pMsg);
			LogAddTD("-¦¬ã-ëLÿ +ä¦+ +-¬ñ -þ+¡ %d+¦ ¦--T-L+¦+-.+-¬å +ä¦+¬+ L¦¦¬¦+ +ã-¦-+¬L.", this->m_iAB_NOTIFY_COUNT + 1);
		}
		// ----
		if( this->m_iAB_REMAIN_MSEC <= 0 )
		{
			this->Clear();
			// ----
			for( int n = 9000; n < 10000; n++ )
			{
				if( gObj[n].Connected == PLAYER_PLAYING )
				{
					if( gObj[n].Type == OBJ_USER )
					{
						gObjMoveGate(n, 27);	//Lorencia / Devias?
					}
				}
			}
			// ----
			this->SetState(AB_STATE_NONE);
			this->GDReqArcaBattleProcMultiCast(AB_STATE_NONE);
		}
	}
}
// -------------------------------------------------------------------------------

//007218a0
void CArcaBattle::ProcStatePlayEnd() //[BAD POSITION]
{
	int iTICK_MSEC = GetTickCount() - this->m_iAB_TICK_COUNT; //ebp-4
	// ----
	if( iTICK_MSEC >= 1000 )
	{
		this->m_iAB_REMAIN_MSEC -= iTICK_MSEC;
		this->m_iAB_TICK_COUNT	= GetTickCount();
	}
	// ----
	if( this->m_iAB_REMAIN_MSEC <= 0 )
	{
		for( int i = 0; i < AB_MAX_BATTLEPOS; i++ ) //ebp-8
		{
			CTime tExpireTime;	//ebp-18
			CTime tCurrentTime(CTime::GetTickCount()); //ebp-10 [BAD POSITION]
			CTimeSpan tLeftTime(7, 0, 0, 0);	//ebp-20
			tExpireTime = tCurrentTime - tLeftTime;
			time_t tReturnTime = tExpireTime.GetTime();		//ebp-28
			// ----
			this->m_stArcaBattleResultInfo[i].iGuildNum			= this->m_stObeliskState[i].m_iOccupyGuildNum;
			this->m_stArcaBattleResultInfo[i].iObeliskAttrKind	= this->m_stObeliskInfo[i].m_iAttrKind;
		}
	}
}
// -------------------------------------------------------------------------------

//007219b0
void CArcaBattle::DeleteArcaBattleAllMonster()
{
	for( int nGroupNumber = 0; nGroupNumber < AB_MAX_MONGROUP; nGroupNumber++ )
	{
		if( this->m_stMonsterGroupInfo[nGroupNumber].m_nGroupNumber < 0 )
		{
			continue;
		}
		// ----
		if( !AB_CHECKMONGROUP(nGroupNumber) ) //ahah, paranoid wz
		{
			return;
		}
		// ----
		for( int i = 0; i < AB_MAX_MONCOUNT; i++ )
		{
			if( this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex > -1 )
			{
				int nIndex = this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex;
				gObjDel(nIndex);
				LogAdd("[ArcaBattle][MonsterGroupRegen] Delete Monster - %d ", nIndex);
				this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex	= -1;
				this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].bItemDrop	= 0;
				this->m_stMonsterGroupInfo[i].m_dwNextRegenTime = GetTickCount();
			}
		}
	}
}
// -------------------------------------------------------------------------------

//00721b00
void CArcaBattle::Run()
{
	if( !this->IsArcaBattleServer() )
	{
		return;
	}
	// ----
	switch(this->m_iAB_State)
	{
	case AB_STATE_NONE:
		this->CheckSync();
		break;
	case AB_STATE_CLOSED:
		this->ProcStateClosed();
		break;
	case AB_STATE_START:
		this->ProcStateMasterReg();
		break;
	case AB_STATE_REG:
		this->ProcStateReg();
		break;
	case AB_STATE_READY:
		this->ProcStateReady();
		break;
	case AB_STATE_PATY:
		this->ProcStatePaty();
		break;
	case AB_STATE_PLAY:
		this->ProcStatePlaying();
		break;
	case AB_STATE_RESULT:
		this->ProcStateResult();
		break;
	case AB_STATE_CLOSE:
		this->ProcStateChannelClose();
		break;
	case AB_STATE_END:
		this->ProcStatePlayEnd();
		break;
	}
}
// -------------------------------------------------------------------------------

//00721bd0
void CArcaBattle::SendMapServerGroupMsg(LPSTR lpszMsg)
{
//	GS_GDReqMapSvrMsgMultiCast(g_MapServerManager.GetMapSvrGroup(), lpszMsg);
}
// -------------------------------------------------------------------------------

//00721c00
int CArcaBattle::GetJoinMemberCnt()
{
	return this->m_iJoinGuildMemberCnt;
}
// -------------------------------------------------------------------------------

//00721c20
BOOL CArcaBattle::EnterArcaBattleEvent(int aIndex)
{
	if( !AB_CHECKSLOT(this->m_iCurUserCount) )
	{
		LogAddTD("[ArcaBattle] Entered User Over UserCount [%d]", this->m_iCurUserCount);
		return false;
	}
	// ----
	for( int i = 0; i < this->m_iCurUserCount; i++ )
	{
		if( !strcmp(this->m_UserData[i].szCharName, gObj[aIndex].Name) )
		{
			LogAddTD("[ArcaBattle] Entered ArcaBattle [%s] [%s] UserCnt [%d]",
				gObj[aIndex].AccountID, gObj[aIndex].Name, this->m_iCurUserCount);
			return false;
		}
	}
	// ----
	memcpy(this->m_UserData[this->m_iCurUserCount].szCharName, gObj[aIndex].Name, 11);
	this->m_iCurUserCount++;
    LogAddTD("[ArcaBattle] Enter ArcaBattle [%s] [%s] UserCnt [%d]", 
		gObj[aIndex].AccountID, gObj[aIndex].Name, this->m_iCurUserCount);
	return true;
}
// -------------------------------------------------------------------------------

//00721db0
_tagArcaBattleUserInfo * CArcaBattle::GetUserInfo(LPSTR szName)
{
	for( int i = 0; i < this->m_iCurUserCount; i++ )
	{
		if( !strcmp(this->m_UserData[i].szCharName, szName) )
		{
			return &this->m_UserData[i];
		}
	}
	// ----
	return 0;
}
// -------------------------------------------------------------------------------

//00721e20
void CArcaBattle::SendObeliskLife()
{
	_tagPMSG_SEND_OBELISK_LIFE pMsg;
	// ----
	for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
	{
		int iObeliskIndex = this->m_stObeliskState[i].m_iMonIndex;
		// ----
		if( this->m_stObeliskState[i].m_iMonIndex < 0 )
		{
			continue;
		}
		// ----
		if( !OBJMAX_RANGE(iObeliskIndex) )
		{
			return;
		}

		if( !gObj[iObeliskIndex].Live )
		{
			pMsg.m_stObeliskLife[i].btLife		= 0;
			pMsg.m_stObeliskLife[i].btAttribute = (BYTE)this->m_stObeliskInfo[i].m_iAttrKind;
		}
		else
		{
			pMsg.m_stObeliskLife[i].btLife		= (int)ceil(gObj[iObeliskIndex].Life / (gObj[iObeliskIndex].MaxLife / 10.0)) & 0xF;	//Need check
			pMsg.m_stObeliskLife[i].btAttribute = (BYTE)this->m_stObeliskInfo[i].m_iAttrKind;
		}
	}
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_SEND_OBELISK_LIFE);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x3F;
	// ----
	for( int n = OBJ_STARTUSERINDEX; n < OBJMAX; n++ )
	{
		LPOBJ lpObj = &gObj[n];
		// ----
		if( gObjIsConnected(n) == 1 && lpObj->Type == OBJ_USER )
		{
			DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		}
	}
}
// -------------------------------------------------------------------------------

//00721fe0
void CArcaBattle::SendObeliskState()
{
	_tagPMSG_SEND_OBELISK_STATE pMsg;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_SEND_OBELISK_STATE);
	pMsg.h.headcode	= 0xF8;
	pMsg.h.subcode	= 0x3A;
	// ----
	for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
	{
		int n;
		BYTE AuraRelease;
		LPOBJ obj;
		int iObeliskIndex = this->m_stObeliskState[i].m_iMonIndex;
		// ----
		if ( this->m_stObeliskState[i].m_iMonIndex >= 0 )
		{
			obj = &gObj[iObeliskIndex];
			pMsg.btObeliskState		= gObj[iObeliskIndex].Life > 0.0;
			pMsg.btObeliskAttribute = (BYTE)this->m_stObeliskInfo[i].m_iAttrKind;
			AuraRelease				= 2;	//#DEFINE
			// ----
			for( int j = 0; j < AB_MAX_AURAPOS; j++ )
			{
				if( this->m_stObeliskState[i].m_stAuraState[j].m_btAuraState != 2 ) //#DEFINE
				{
					AuraRelease = 0;
					break;
				}
			}
			// ----
			pMsg.btAuraState = AuraRelease;
			// ----
			for( n = 0; n < MAX_VIEWPORT; n++ )
			{
				if( obj->VpPlayer2[n].type == OBJ_USER && obj->VpPlayer2[n].state != 0 )
				{
					int iTarObjIndex = gObj[obj->VpPlayer2[n].number].m_Index;
					DataSend(iTarObjIndex, (LPBYTE)&pMsg, sizeof(_tagPMSG_SEND_OBELISK_STATE));
				}
			}
		}
	}
}
// -------------------------------------------------------------------------------

//00722170
BOOL CArcaBattle::RegenObelisk()
{
	for( int i = 0; i < this->m_iObeliskCnt; i++ )
	{
		int iObeliskIndex = this->m_stObeliskState[i].m_iMonIndex;
		// ----
		if( !OBJMAX_RANGE(iObeliskIndex) )
		{
			return false;
		}
		// ----
		if( gObj[iObeliskIndex].Live == 1 )
		{
			this->m_stObeliskInfo[i].m_dwTick = GetTickCount();
		}
		else
		{
			if( GetTickCount() - this->m_stObeliskInfo[i].m_dwTick >= this->m_stObeliskInfo[i].m_dwRegenTime )
			{
				int result = gObjAddMonster(this->m_stObeliskInfo[i].m_iMapIndex);
				// ----
				if( result >= 0 )
				{
					gObjSetMonster(result, this->m_stObeliskInfo[i].m_iMonsterNum);
					// ----
					gObj[result].Class			= (WORD)this->m_stObeliskInfo[i].m_iMonsterNum;
					gObj[result].MapNumber		= (BYTE)this->m_stObeliskInfo[i].m_iMapIndex;
					gObj[result].X				= (WORD)this->m_stObeliskInfo[i].m_iPosX;
					gObj[result].Y				= (WORD)this->m_stObeliskInfo[i].m_iPosY;
					gObj[result].TX				= (WORD)this->m_stObeliskInfo[i].m_iPosX;
					gObj[result].TY				= (WORD)this->m_stObeliskInfo[i].m_iPosY;
					gObj[result].m_OldX			= (WORD)this->m_stObeliskInfo[i].m_iPosX;
					gObj[result].m_OldY			= (WORD)this->m_stObeliskInfo[i].m_iPosY;
					gObj[result].StartX			= (BYTE)this->m_stObeliskInfo[i].m_iPosX;
					gObj[result].StartY			= (BYTE)this->m_stObeliskInfo[i].m_iPosY;
					gObj[result].Dir			= 1;
					gObj[result].Live			= true;
					gObj[result].DieRegen		= 0;
					gObj[result].MaxRegenTime	= 0;
					gObj[result].MaxLife		= (double)this->m_stObeliskInfo[i].m_iOccupyHP;
					gObj[result].Life			= (double)this->m_stObeliskInfo[i].m_iOccupyHP;
					// ----
					this->m_stObeliskState[i].m_iMonIndex = result;
					LogAddTD("[ArcaBattle] (%d) Obelisk be Regen.", result);
				}
			}
		}
	}
	// ----
	return true;
}
// -------------------------------------------------------------------------------

//007224f0
void CArcaBattle::SetRandomObeliskAttr()
{
	int iObeliskAttrCnt;
	int iArrObeliskAttr[AB_MAX_BATTLEPOS];
	int iArrMonsterIndex[AB_MAX_BATTLEPOS];
	bool bflag;
	int iRand;
	int iObeliskAttr;
	// ----
	iArrObeliskAttr[0]	= 0;
	iArrObeliskAttr[1]	= 0;
	iArrObeliskAttr[2]	= 0;
	iArrObeliskAttr[3]	= 0;
	iArrObeliskAttr[4]	= 0;
	iArrMonsterIndex[0] = 598;
	iArrMonsterIndex[1] = 599;
	iArrMonsterIndex[2] = 600;
	iArrMonsterIndex[3] = 601;
	iArrMonsterIndex[4] = 602;
	iObeliskAttrCnt		= 0;
	// ----
	do	//need check
	{
		iRand = rand() % AB_MAX_BATTLEPOS + 1;
		bflag = false;
		// ----
		for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
		{
			if( iArrObeliskAttr[i] == iRand )
			{
				bflag = true;
			}
		}
		// ----
		if( !bflag )
		{
			iArrObeliskAttr[iObeliskAttrCnt] = iRand;
			iObeliskAttrCnt++;
		}

	} while( iObeliskAttrCnt != AB_MAX_BATTLEPOS );
	// ----
	for( int j = 0; j < AB_MAX_BATTLEPOS; j++ )
	{
		iObeliskAttr = iArrObeliskAttr[j];
		this->m_stObeliskInfo[j].m_iAttrKind	= iObeliskAttr;
		this->m_stObeliskInfo[j].m_iMonsterNum	= iArrMonsterIndex[iObeliskAttr];
	}
}
// -------------------------------------------------------------------------------

//00722600
BOOL CArcaBattle::SetObelisk()
{
	for( int iCnt = 0; iCnt < AB_MAX_BATTLEPOS; iCnt++ )
	{
		if ( this->m_stObeliskState[iCnt].m_iMonIndex >= 0 )
		{
			int iIndex = this->m_stObeliskState[iCnt].m_iMonIndex;
			LogAdd("[ArcaBattle][Obelisk] Delete Monster - %d ", this->m_stObeliskState[iCnt].m_iMonIndex);
#if( AB_DEBUG_TRACE == 1 )
			TRACE_LOG("[ArcaBattle][Obelisk] Delete Monster - %d ", this->m_stObeliskState[iCnt].m_iMonIndex);
#endif
			gObjDel(iIndex);
		}
		// ----
		this->m_stObeliskState[iCnt].Clear();
	}
	// ----
	for( int i = 0; i < this->m_iObeliskCnt; i++ )
	{
		int result = gObjAddMonster(this->m_stObeliskInfo[i].m_iMapIndex);
		// ----
		if( result >= 0 )
		{
			gObjSetMonster(result, this->m_stObeliskInfo[i].m_iMonsterNum);
			// ----
			gObj[result].Class			= (WORD)this->m_stObeliskInfo[i].m_iMonsterNum;
			gObj[result].MapNumber		= (BYTE)this->m_stObeliskInfo[i].m_iMapIndex;
			gObj[result].X				= (WORD)this->m_stObeliskInfo[i].m_iPosX;
			gObj[result].Y				= (WORD)this->m_stObeliskInfo[i].m_iPosY;
			gObj[result].TX				= (WORD)this->m_stObeliskInfo[i].m_iPosX;
			gObj[result].TY				= (WORD)this->m_stObeliskInfo[i].m_iPosY;
			gObj[result].m_OldX			= (WORD)this->m_stObeliskInfo[i].m_iPosX;
			gObj[result].m_OldY			= (WORD)this->m_stObeliskInfo[i].m_iPosY;
			gObj[result].StartX			= (BYTE)this->m_stObeliskInfo[i].m_iPosX;
			gObj[result].StartY			= (BYTE)this->m_stObeliskInfo[i].m_iPosY;
			gObj[result].Dir			= 1;
			gObj[result].Live			= true;
			gObj[result].DieRegen		= 0;
			gObj[result].m_State		= 1;
			gObj[result].MaxRegenTime	= 0;
			gObj[result].MaxLife		= (double)this->m_stObeliskInfo[i].m_iHP;
			gObj[result].Life			= (double)this->m_stObeliskInfo[i].m_iHP;
			// ----
			this->m_stObeliskState[i].m_iMonIndex = result;
			LogAddTD("[ArcaBattle] (%d) Obelisk be created.", result);
		}

		// ----
		for( int nAura = 0; nAura < AB_MAX_AURAPOS; nAura++ )
		{
			int nResult = gObjAddMonster(this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iMapIndex);
			// ----
			if( nResult >= 0 )
			{
				gObjSetMonster(nResult, 603);
				// ----
				gObj[nResult].Class			= 603;
				gObj[nResult].MapNumber		= (BYTE)this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iMapIndex;
				gObj[nResult].X				= (WORD)this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iAuraPosX;
				gObj[nResult].Y				= (WORD)this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iAuraPosY;
				gObj[nResult].TX			= (WORD)this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iAuraPosX;
				gObj[nResult].TY			= (WORD)this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iAuraPosX;
				gObj[nResult].m_OldX		= (WORD)this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iAuraPosX;
				gObj[nResult].m_OldY		= (WORD)this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iAuraPosX;
				gObj[nResult].StartX		= (BYTE)this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iAuraPosX;
				gObj[nResult].StartY		= (BYTE)this->m_stObeliskInfo[i].m_stAuraInfo[nAura].m_iAuraPosX;
				gObj[nResult].Dir			= 1;
				gObj[nResult].Live			= true;
				gObj[nResult].DieRegen		= 0;
				gObj[nResult].m_State		= 1;
				gObj[nResult].MaxRegenTime	= 0;
				gObj[nResult].MaxLife		= 1.0;
				gObj[nResult].Life			= 1.0;
				// ----
				this->m_stObeliskState[i].m_stAuraState[nAura].m_iIndex = nResult;
				LogAddTD("[ArcaBattle] (%d) Aura be created.", nResult);
			}
		}
	}
	// ----
	return true;
}
// -------------------------------------------------------------------------------

//00722c90
void CArcaBattle::ChkAuraUserHover()	//Bad positions
{
	int i; //ebp-4
	int n; //ebp-8
	LPOBJ lpObj; //ebp-0c
	_tagPMSG_ARCA_BATTLE_OBELISK_INFO pMsg; //ebp-18
	LPOBJ obj; //ebp-1c
	int j; //ebp-20
	int iHoverUserIndex_2; //ebp-24
	int iAurReleaseCnt; //ebp-28
	int iHoverUserIndex_1; //ebp-2c
	int iUserIndex; //ebp-30
	int iObeliskIndex; //ebp-34
	int nAura; //ebp-38
	//int iIndex; //ebp-3c
	//int n; //ebp-40
	//int iTarObjIndex; //ebp-44
	// ----
	n = 0;
	i = 0;
	j = 0;
	lpObj = NULL;
	// ----
	for( i = 0; i < this->m_iObeliskCnt; i++ )
	{
		this->m_stObeliskState[i].m_iAuraState = 0;	//#DEFINE
		// ----
		for( j = 0; j < AB_MAX_AURAPOS; j++ )
		{
			this->m_stObeliskState[i].m_stAuraState[j].m_btAuraState		= 0;
			this->m_stObeliskState[i].m_stAuraState[j].m_iHoverUserIndex	= 0;
		}
	}
	// ----
	for( n = OBJ_STARTUSERINDEX; n < OBJMAX; n++ )
	{
		lpObj = &gObj[n];
		// ----
		if( gObjIsConnected(n) == 1 && lpObj->Type == OBJ_USER )
		{
			for( i = 0; i < this->m_iObeliskCnt; i++ )
			{
				for( j = 0; j < AB_MAX_AURAPOS; j++ )
				{
					if(		this->m_stObeliskInfo[i].m_stAuraInfo[j].m_iAuraPosX == lpObj->X 
						&&	this->m_stObeliskInfo[i].m_stAuraInfo[j].m_iAuraPosY == lpObj->Y )
					{
						this->m_stObeliskState[i].m_stAuraState[j].m_btAuraState		= 1;	//#DEFINE
						this->m_stObeliskState[i].m_stAuraState[j].m_iHoverUserIndex	= n;
						LogAddTD("[ArcaBattle] Obelisk foot hold [on][%s][%s][%s] : Pos X[%d] Y[%d]",
							lpObj->AccountID, lpObj->Name, lpObj->GuildName, lpObj->X, lpObj->Y);
					}
				}
			}
		}
	}
	// ----
	for( i = 0; i < this->m_iObeliskCnt; i++ )
	{
		iAurReleaseCnt		= 0;
		iHoverUserIndex_1	= 0;
		iHoverUserIndex_2	= 0;
		// ----
		for( j = 0; j < AB_MAX_AURAPOS; j++ )
		{
			if( this->m_stObeliskState[i].m_stAuraState[j].m_btAuraState == 1 )
			{
				iAurReleaseCnt++;
			}
		}
		// ----
		if( iAurReleaseCnt == 3 )
		{
			this->m_stObeliskState[i].m_iAuraState = 2; //#DEFINE
			// ----
			for( j = 0; j < AB_MAX_AURASTATE; j++ )
			{
				iHoverUserIndex_1 = this->m_stObeliskState[i].m_stAuraState[j].m_iHoverUserIndex;
				iHoverUserIndex_2 = this->m_stObeliskState[i].m_stAuraState[j + 1].m_iHoverUserIndex;
				// ----
				if( gObj[iHoverUserIndex_1].GuildNumber != gObj[iHoverUserIndex_2].GuildNumber )
				{
					this->m_stObeliskState[i].m_iAuraState = 1;
				}
			}
			// ----
			if( this->m_stObeliskState[i].m_iAuraState == 2 )
			{
				iUserIndex = this->m_stObeliskState[i].m_stAuraState[0].m_iHoverUserIndex;
				// ----
				if( this->m_stObeliskState[i].m_iAuraReleaseGuildNum != gObj[iUserIndex].GuildNumber )
				{
					iObeliskIndex = this->m_stObeliskState[i].m_iMonIndex;
					// ----
					if( gObj[iObeliskIndex].Live == 1 )
					{
						if( this->m_stObeliskState[i].m_iObeliskState == 1 )
						{
#if( AB_DEBUG_TRACE == 1 )
							TRACE_LOG("Obelisk OCCUPY HP Refill");
#endif
							gObj[iObeliskIndex].Life = (double)this->m_stObeliskInfo[i].m_iOccupyHP;
						}
						else
						{
#if( AB_DEBUG_TRACE == 1 )
							TRACE_LOG("Obelisk HP Refill");
#endif
							gObj[iObeliskIndex].Life = (double)this->m_stObeliskInfo[i].m_iHP;
						}
					}
				}
				// ----
				this->m_stObeliskState[i].m_iAuraReleaseGuildNum = gObj[iUserIndex].GuildNumber;
				// ----
				for( j = 0; j < AB_MAX_AURAPOS; j++ )
				{
					this->m_stObeliskState[i].m_stAuraState[j].m_btAuraState = 2;
				}
			}
		}
		else
		{
			if( iAurReleaseCnt > 0 )
			{
				this->m_stObeliskState[i].m_iAuraState = 1;
			}
		}
	}
	// ----
	//pMsg constructor
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_ARCA_BATTLE_OBELISK_INFO);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x37;
	// ----
	for( i = 0; i < this->m_iObeliskCnt; i++ )
	{
		for( nAura = 0; nAura < AB_MAX_AURAPOS; nAura++ )
		{
			int iIndex	= this->m_stObeliskState[i].m_stAuraState[nAura].m_iIndex;
			obj			= &gObj[iIndex];
			// ----
			for( int n = 0; n < MAX_VIEWPORT; n++ )
			{
				if( obj->VpPlayer2[n].type == OBJ_USER && obj->VpPlayer2[n].state != 0 )
				{
					int iTarObjIndex			= obj->VpPlayer2[n].number;
					pMsg.m_btObeliskAttribute	= (BYTE)this->m_stObeliskInfo[i].m_iAttrKind;
					// ----
					for( j = 0; j < AB_MAX_AURAPOS; j++ )
					{
						pMsg.m_btAuraState[j] = this->m_stObeliskState[i].m_stAuraState[j].m_btAuraState;
					}
					// ----
					DataSend(gObj[iTarObjIndex].m_Index, (LPBYTE)&pMsg, sizeof(_tagPMSG_ARCA_BATTLE_OBELISK_INFO));
				}
			}
		}
	}
}
// -------------------------------------------------------------------------------

//007232f0
BOOL CArcaBattle::IsEnableAttackObelisk(OBJECTSTRUCT & obj, int iMonNumber)
{
	int i;
	_tagOBELISK_STATE * pObelskState = NULL;
	// ----
	for( i = 0; i < this->m_iObeliskCnt; i++ )
	{
		if( this->m_stObeliskInfo[i].m_iMonsterNum == iMonNumber )
		{
			pObelskState = &this->m_stObeliskState[i];
		}
	}
	// ----
	if( pObelskState == NULL )
	{
		return true;
	}
	// ----
	if( this->GetState() != AB_STATE_PLAY )
	{
		return false;
	}
	// ----
	if( pObelskState->m_iOccupyGuildNum == obj.GuildNumber )
	{
		return false;
	}
	// ----
	if( pObelskState->m_iAuraState == 2 && pObelskState->m_iAuraReleaseGuildNum == obj.GuildNumber )
	{
		return true;
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

//007233b0
BOOL CArcaBattle::IsPkEnable(OBJECTSTRUCT & obj, OBJECTSTRUCT & Tarobj)
{
	_GUILD_INFO_STRUCT * lpGuildInfo	= obj.lpGuild;
	_GUILD_INFO_STRUCT * lpTarGuildInfo	= Tarobj.lpGuild;
	// ----
	if( g_bIsArcaBattleServer == false )
	{
		return true;
	}
	// ----
	if( this->GetState() != AB_STATE_PLAY )
	{
		return true;
	}
	// ----
	if( lpGuildInfo == NULL )
	{
		LogAddC(2, "[ArcaBattle] Error lpGuildInfo NULL");
		return false;
	}
	// ----
	if( lpTarGuildInfo == NULL )
	{
		LogAddC(2, "[ArcaBattle] Error lpTarGuildInfo NULL");
		return false;
	}
	// ----
	if(		this->GetArcaBattleMapIndex() != obj.MapNumber 
		||	this->GetArcaBattleMapIndex() != Tarobj.MapNumber )
	{
		return false;
	}
	// ----
	if( lpGuildInfo->Number != lpTarGuildInfo->Number )
	{
		return false;
	}
	// ----
	return true;
}
// -------------------------------------------------------------------------------

//00723480
BOOL CArcaBattle::IsArcaBattleServer()
{
	return 1;//g_bIsArcaBattleServer;
}
// -------------------------------------------------------------------------------

//00723490
BOOL CArcaBattle::IsArcaBattleMap(int iMapNumber)
{
	if( iMapNumber == this->GetArcaBattleMapIndex() )
	{
		return true;
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

//007234c0
void CArcaBattle::SendArcaBattleStateAll(int iState)
{
	int n;
	LPOBJ lpObj;
	lpObj = NULL;
	n = 0;
	// ----
	_tagPMSG_ARCA_BATTLE_STATE pMsg;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_ARCA_BATTLE_STATE);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x38;
	pMsg.m_btArcaBattleState = iState;
	// ----
	for( n = OBJ_STARTUSERINDEX; n < OBJMAX; n++ )
	{
		 lpObj = &gObj[n];
		 // ----
		 if( gObjIsConnected(n) == 1 && lpObj->Type == OBJ_USER )
		 {
			 DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		 }
	}
}
// -------------------------------------------------------------------------------

//00723560
void CArcaBattle::SendArcaBattleCurrentState(OBJECTSTRUCT & obj)
{
	_tagPMSG_ARCA_BATTLE_STATE pMsg;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_ARCA_BATTLE_STATE);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x38;
	pMsg.m_btArcaBattleState = this->GetState();
	// ----
	DataSend(obj.m_Index, (LPBYTE)&pMsg, pMsg.h.size);
}
// -------------------------------------------------------------------------------

//007235b0
BOOL CArcaBattle::IsOccupyObelisk(OBJECTSTRUCT & obj, int iMonIndex)
{
	for( int i = 0; i < this->m_iObeliskCnt; i++ )
	{
		if( iMonIndex == this->m_stObeliskInfo[i].m_iMonsterNum )
		{
			PMSG_NOTICE pMsg;	//Need check conversion
			TNotice::MakeNoticeMsgEx(&pMsg, 0, "¬L¦ç¬î-¦+é-á +á¬ã ¦¦-·-L+¦+-.");
			TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pMsg);
			// ----
			this->m_stObeliskState[i].m_iObeliskState	= AB_OBSTATE_OCCUPY;
			this->m_stObeliskState[i].m_iOccupyGuildNum = obj.GuildNumber;
			this->m_stObeliskState[i].m_iGroupNumber	= this->m_stObeliskInfo[i].m_iGroupNumber;
			memcpy(this->m_stObeliskState[i].m_szOccupyGuildName, obj.GuildName, 9);
			// ----
			LogAddTD("[ArcaBattle] Occupy Obelisk In ArcaBattle Progress  [%s][%s][%s] ObeliskGroup:[%d]",
				obj.AccountID, obj.Name, obj.GuildName, this->m_stObeliskState[i].m_iGroupNumber);
			return true;
		}
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

//00723750
void CArcaBattle::NotifyArcaBattle(int iNotifyNum)
{
	if( iNotifyNum == 3 )
	{
		PMSG_NOTICE pMsg;	//Need check conversion
		TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3502));
		TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
		TNotice::SendNoticeToAllUser(&pMsg);
		LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ -+L-");
	}
	else if( iNotifyNum == 4 )
	{
		PMSG_NOTICE pMsg;	//Need check conversion
		TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3503));
		TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(-1, -1, 0xC8, 0x50), 1, 0, 20);
		TNotice::SendNoticeToAllUser(&pMsg);
		LogAddTD("[ArcaBattle] -¦¬ã-ëLÿ +-¬ñ");
	}
}
// -------------------------------------------------------------------------------

//00723880
void CArcaBattle::GDReqWinGuildInfo()
{
	_tagPMSG_REQ_AB_WIN_GUILD_INFO_DS pMsg;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_REQ_AB_WIN_GUILD_INFO_DS);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x39;
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
	// ----
	this->m_bReqABWinGuildInfo = true;
}
// -------------------------------------------------------------------------------

//007238d0
void CArcaBattle::GDReqProcState()
{
	if( this->IsArcaBattleServer() == false )
	{
		return;
	}
	// ----
	_tagPMSG_REQ_AB_PROC_STATE_DS pMsg;
	// ----
	pMsg.h.c		= 0xC1;
    pMsg.h.size		= sizeof(_tagPMSG_REQ_AB_PROC_STATE_DS);
    pMsg.h.headcode = 0xF8;
    pMsg.h.subcode	= 0x3F;
    cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
	// ----
    this->SetDataLoadProcState(1);
}
// -------------------------------------------------------------------------------

//00723920
void CArcaBattle::GDReqWinGuildInfoInsert()
{
	_tagPMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS pMsg;
	int iGuildCnt = 0;
	// ----
	for( int i = 0; i < AB_MAX_BATTLEPOS; i++ )
	{
		if( this->m_stObeliskState[i].m_iObeliskState == AB_OBSTATE_OCCUPY )
		{
			pMsg.h.c		= 0xC1;
			pMsg.h.size		= sizeof(_tagPMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS);
			pMsg.h.headcode = 0xF8;
			pMsg.h.subcode	= 0x38;
			// ----
			pMsg.m_stABWinGuildInfoDS[iGuildCnt].dwGuild		= this->m_stObeliskState[i].m_iOccupyGuildNum;
			pMsg.m_stABWinGuildInfoDS[iGuildCnt].wOccupyObelisk = (WORD)this->m_stObeliskInfo[i].m_iAttrKind;
			pMsg.m_stABWinGuildInfoDS[iGuildCnt].wObeliskGroup	= (WORD)this->m_stObeliskState[i].m_iGroupNumber;
			memcpy(pMsg.m_stABWinGuildInfoDS[iGuildCnt].szGuildName, this->m_stObeliskState[i].m_szOccupyGuildName, 9);
			// ----
			LogAddTD("[ArcaBattle] Request Win Guild Info Insert GuildName : %s(%d) OccupyObelisk : %d ObeliskGroup : %d",
				pMsg.m_stABWinGuildInfoDS[iGuildCnt].szGuildName, pMsg.m_stABWinGuildInfoDS[iGuildCnt].dwGuild,
				pMsg.m_stABWinGuildInfoDS[iGuildCnt].wOccupyObelisk, pMsg.m_stABWinGuildInfoDS[iGuildCnt].wObeliskGroup);
			iGuildCnt++;
		}
	}
	// ----
	if( iGuildCnt > 0 )
	{
		pMsg.btGuildCnt = iGuildCnt;
		pMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();
		cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
	}
}
// -------------------------------------------------------------------------------

//00723aa0
void CArcaBattle::SendPlayResult()
{
	int i;
	LPOBJ lpObj;
	_tagPMSG_AB_RESULT pMsg;
	i = 0;
	int iCnt = 0;
	// ----
	for( i = 0; i < AB_MAX_BATTLEPOS; i++ )
	{
		if( this->m_stObeliskState[i].m_iGroupNumber == -1 )
		{
			continue;
		}
		// ----
		pMsg.m_stABObeliskOccupy[iCnt].wObeliskKind		= (WORD)this->m_stObeliskInfo[i].m_iAttrKind;
		pMsg.m_stABObeliskOccupy[iCnt].szGuildName[8]	= 0; //Need check
		memcpy(pMsg.m_stABObeliskOccupy[iCnt].szGuildName, this->m_stObeliskState[i].m_szOccupyGuildName, 8);
		// ----
		LogAddTD("[ArcaBattle] SendPlayResult Occupy Guild Name : [%s] GroupNumber [%d] ObeliskAttribute [%d]",
			pMsg.m_stABObeliskOccupy[iCnt].szGuildName, this->m_stObeliskState[i].m_iGroupNumber,
			pMsg.m_stABObeliskOccupy[iCnt].wObeliskKind);
		iCnt++;
	}
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_AB_RESULT);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x3B;
	// ----
	for( int n = OBJ_STARTUSERINDEX; n < OBJMAX; n++ )
	{
		lpObj = &gObj[n];
		// ----
		if( gObjIsConnected(n) == 1 && lpObj->Type == OBJ_USER )
		{
			if( lpObj->lpGuild == NULL )
			{
				LogAddTD("[ArcaBattle] Error SendPlayResult() lpObj->lpGuild NULL %s %s", 
					lpObj->AccountID, lpObj->Name);
				continue;
			}
			// ----
			pMsg.btABResult = 0;
			// ----
			for( i = 0; i < AB_MAX_BATTLEPOS; i++ )
			{
				if( lpObj->lpGuild->Number == this->m_stObeliskState[i].m_iOccupyGuildNum )
				{
					pMsg.btABResult = 1;
				}
			}
			// ----
			__int64 iRewardExp;
			_tagArcaBattleUserInfo * pUserInfo = this->GetUserInfo(lpObj->Name);
			// ----
			if( pUserInfo == NULL )
			{
				LogAddTD("[ArcaBattle] Error SendPlayResult() pUserInfo NULL %s %s GuildName %s",
					lpObj->AccountID, lpObj->Name, lpObj->GuildName);
				continue;
			}
			// ----
			iRewardExp = 0;
			this->CalcRewardExp(*lpObj, iRewardExp);
			// ----
			if( gObjCheckUsedBuffEffect(&gObj[n], AT_CSHOP_SEAL_OF_SUSTENANCE) 
				|| gObjCheckUsedBuffEffect(&gObj[n], AT_PCBANG_SEAL_OF_SUSTENANCE) )
			{
				iRewardExp = 0;
			}
			// ----
			gObj[n].Experience += iRewardExp;
			gObjLevelUp(&gObj[n], &iRewardExp, 0, 7); //7 = ArcaBattle event type?
			// ----
			//if( !g_MasterLevelSystem.IsMasterLevelUser(&gObj[n]) )
			if( !g_MasterExp.IsEnabled(&gObj[n]) )
			{
				//GCSendExp_INT64(n, -1, iRewardExp, 0, 0);
				GCSendExp(n, (WORD)-1, iRewardExp, 0, 0);
			}
			// ----
//			g_GensSystem.AddContributePoint(lpObj, pUserInfo->m_stAcquiredPoints.dwContributePoints);
			lpObj->m_GensInfo.iContribution += pUserInfo->m_stAcquiredPoints.dwContributePoints;
			pMsg.wBootyCnt			= (WORD)pUserInfo->m_stAcquiredPoints.dwBootyCnt;
			pMsg.dwContributePoint	= pUserInfo->m_stAcquiredPoints.dwContributePoints;
			pMsg.dwKillPoint		= pUserInfo->m_stAcquiredPoints.dwKillPoints;
			pMsg.dwRewardExp		= iRewardExp;
			// ----
			LogAddTD("[ArcaBattle] SendPlayResult [%s][%s] BootyCnt[%d] ContributePoint[%d] KillPoint[%d] RewardExp[%d]",
				gObj[n].AccountID, gObj[n].Name, pMsg.wBootyCnt, pMsg.dwContributePoint, iRewardExp);
			DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		}
	}
}
// -------------------------------------------------------------------------------

//00723e50
void CArcaBattle::RegenMonsterRun()
{
	DWORD dwTick = GetTickCount();
	// ----
	for( int i = 0; i < AB_MAX_MONGROUP; i++ )
	{
		if( this->m_stMonsterGroupInfo[i].m_nGroupNumber > -1 )
		{
			if( dwTick >= this->m_stMonsterGroupInfo[i].m_dwNextRegenTime )
			{
				this->DeleteMonster(i);
				this->RegenMonster(i);
				this->m_stMonsterGroupInfo[i].m_dwNextRegenTime = GetTickCount() + (60 * 1000) * this->m_stMonsterGroupInfo[i].m_nRegenTimeMin; //need check
			}
		}
	}
}
// -------------------------------------------------------------------------------

//00723f10
void CArcaBattle::DeleteMonster(int nGroupNumber)
{
	if( !AB_CHECKMONGROUP(nGroupNumber) )
	{
		return;
	}
	// ----
	for( int i = 0; i < AB_MAX_MONCOUNT; i++ )
    {
		if( this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex > -1 )
		{
			int nIndex = this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex;
			// ----
			if( !gObj[nIndex].Live )
			{
				gObjDel(nIndex);
				LogAdd("[ArcaBattle][MonsterGroupRegen] Delete Monster - %d ",
					this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex);
				this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex = -1;
			}
		}
	}
}
// -------------------------------------------------------------------------------

//00724020
void CArcaBattle::RegenMonster(int nGroupNumber)
{
	int nMapNumber;
	int nMonsterArrayIndex;
	int nBeginX;
	int nRegenArea;
	int nEndX;
	int nMonsterKindCnt;
	int nMonsterCount;
	int nBeginY;
	int nEndY;
	// ----
	if( !AB_CHECKMONGROUP(nGroupNumber) )
	{
		LogAddTD(""); //need check
		return;
	}
	// ----
	nRegenArea			= rand() % this->m_stMonsterPosition[nGroupNumber].m_nRegenAreaCnt;
	nMapNumber			= this->m_stMonsterGroupInfo[nGroupNumber].m_nMapNumber;
	nMonsterArrayIndex	= 0;
	nMonsterCount		= this->m_stMonsterGroupInfo[nGroupNumber].m_nCnt;
    nMonsterKindCnt		= this->m_stMonsterPosition[nGroupNumber].m_nMonsterKindCnt;
    nBeginX				= this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nRegenArea].m_nBeginX;
    nBeginY				= this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nRegenArea].m_nBeginY;
    nEndX				= this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nRegenArea].m_nEndX;
    nEndY				= this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nRegenArea].m_nEndY;
	// ----
	for( int n = 0; n < nMonsterCount; n++ )
    {
		int nMonsterIndex, nResult, nIndex;
		// ----
		if( nMapNumber == -1 )
		{
			break;
		}
		// ----
		nIndex = this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[n].m_nIndex;
		// ----
		if( nIndex > -1 && gObj[nIndex].Live == 1 )
		{
			continue;
		}
		// ----
		nResult			= gObjAddMonster(nMapNumber);
        nMonsterIndex	= this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[rand() % nMonsterKindCnt].m_nMonsterIndex;
        this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[n].m_nIndex = nResult;
		// ----
		if( nResult >= 0 )
        {
			if( !this->SetPosMonster(nResult, nMapNumber, nBeginX, nBeginY, nEndX, nEndY) )
			{
				gObjDel(nResult);
				LogAdd("[ArcaBattle] error : %s %d", __FILE__, __LINE__); //Line: 3256
				continue;
			}
			// ----
			if( !gObjSetMonster(nResult, nMonsterIndex) )
			{
				gObjDel(nResult);
				LogAdd("[ArcaBattle] error : %s %d", __FILE__, __LINE__); //Line: 3263
				continue;
			}		
		}
		else
		{
			LogAddTD("[ArcaBattle][MonsterGroupRegen] AddMonster fail!! [GROUP]:%d, [INDEX]:%d",
				nGroupNumber, nMonsterIndex);
			continue;
		}
		// ----
		nMonsterArrayIndex++;
		LogAddTD("[ArcaBattle][MonsterGroupRegen] Group: %d, MonsterIndex: %d, MapNumber: %d, X: %d, Y: %d ",
			nGroupNumber, nMonsterIndex, nMapNumber, gObj[nResult].X, gObj[nResult].Y);
	}
}
// -------------------------------------------------------------------------------

//00724310
BOOL CArcaBattle::SetPosMonster(int aIndex, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY)
{
	if( !OBJMAX_RANGE(aIndex) )	//9999
	{
		LogAdd("[ArcaBattle] error : %s %d", __FILE__, __LINE__); //Line: 3291
		return false;
	}
	// ----
	LPOBJ lpObj = &gObj[aIndex];
	// ----
	lpObj->m_PosNum		= -1;
	lpObj->MapNumber	= nMapNumber;
//	this->GetBoxPosition(nMapNumber, nBeginX, nBeginY, nEndX, nEndY, lpObj->X, lpObj->Y);
	lpObj->TX			= lpObj->X;
    lpObj->TY			= lpObj->Y;
    lpObj->m_OldX		= lpObj->X;
    lpObj->m_OldY		= lpObj->Y;
	lpObj->Dir			= rand() % 8;	//need check
	lpObj->StartX		= (BYTE)lpObj->X;
    lpObj->StartY		= (BYTE)lpObj->Y;
    lpObj->DieRegen		= 0;
    lpObj->MaxRegenTime = 0;
	// ----
	return true;
}
// -------------------------------------------------------------------------------

//00724470
BOOL CArcaBattle::GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, BYTE & mx, BYTE & my) //Need compare
{
	int count = 99; 
	int w;
	int h;
	int tx; 
	int ty;
	BYTE attr;
	// ----
	while( count-- != 0 )
	{
		w = aw - ax;
		h = ah - ay;
		// ----
		__try
		{
			tx = ax + (rand()%w);
			ty = ay + (rand()%h);
		}
		__except( w = 1, h = 1, 1 )
		{

		}
		// ----
		attr = MapC[mapnumber].GetAttr(tx, ty);
		// ----
		if( ((attr&1) != 1) && ((attr &4) != 4) && ((attr&8) != 8) )
		{
			mx = tx;
			my = ty;
			return true;
		}
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

//00724590
void CArcaBattle::AddContributePoint(OBJECTSTRUCT & obj, OBJECTSTRUCT & Tarobj)
{
	_tagArcaBattleUserInfo * pUserInfo;
	int iSubValue;
	int iAddValue;
	// ----
	iAddValue = 0;
	iSubValue = 0;
	// ----
	if( obj.Type != OBJ_USER )
	{
		return;
	}
	// ----
	char szName[11] = { 0 };
	memcpy(szName, obj.Name, 11);
	pUserInfo = this->GetUserInfo(szName);
	// ----
	if( pUserInfo == NULL )
	{
		return;
	}
	// ----
	if( Tarobj.Type == OBJ_MONSTER )
	{
		if(		Tarobj.Class == 592
			||	Tarobj.Class == 593
			||	Tarobj.Class == 594
			||	Tarobj.Class == 595
			||	Tarobj.Class == 596 )
		{
			pUserInfo->m_stAcquiredPoints.dwContributePoints += AB_REW_GENS_BOSS;
			LogAddTD("[ArcaBattle] Boss Monster Kill [%s] ContributePoints %d",
				pUserInfo->szCharName, pUserInfo->m_stAcquiredPoints.dwContributePoints);
			//return;
		}
	}
	else
	{
		if( obj.Level == Tarobj.Level )
        {
			iAddValue = AB_REW_GENS_USER1;
			iSubValue = 3;
		}
		else if( obj.Level < Tarobj.Level )
        {
			int iCalcLv = Tarobj.Level - obj.Level;
			// ----
			if( iCalcLv >= 51 )
            {
				iAddValue = AB_REW_GENS_USER2;
				iSubValue = 3;
			}
			else if( iCalcLv >= 31 && iCalcLv <= 50 )
			{
				iAddValue = AB_REW_GENS_USER3;
				iSubValue = 3;
			}
			else if( iCalcLv >= 11 && iCalcLv <= 30 )
			{
				iAddValue = AB_REW_GENS_USER4;
				iSubValue = 3;
			}
			else if( iCalcLv > 0 && iCalcLv <= 10 )
			{
				iAddValue = AB_REW_GENS_USER5;
				iSubValue = 3;
			}
		}
		else if( obj.Level > Tarobj.Level )
		{
			int iCalcLv = obj.Level - Tarobj.Level;
			// ----
			if( iCalcLv >= 51 )
            {
				iAddValue = AB_REW_GENS_USER6;
				iSubValue = 1;
			}
			else if( iCalcLv >= 31 && iCalcLv <= 50 )
			{
				iAddValue = AB_REW_GENS_USER7;
				iSubValue = 2;
			}
			else if( iCalcLv >= 11 && iCalcLv <= 30 )
			{
				iAddValue = AB_REW_GENS_USER8;
				iSubValue = 3;
			}
			else if( iCalcLv > 0 && iCalcLv <= 10 )
			{
				iAddValue = AB_REW_GENS_USER9;
				iSubValue = 3;
			}
		}
		// ----
		pUserInfo->m_stAcquiredPoints.dwContributePoints += 3 * iAddValue; //wtf... iSubValue unused
		LogAddTD("[ArcaBattle] [%s][%s] User Kill ContributePoint[%d] - Result : Lv [%d] / Lv [%d][%s][%s]",
			obj.AccountID, obj.Name, pUserInfo->m_stAcquiredPoints.dwContributePoints, obj.Level,
			Tarobj.Level, Tarobj.AccountID, Tarobj.Name);
	}
}
// -------------------------------------------------------------------------------

//00724870
void CArcaBattle::AddKillPoint(OBJECTSTRUCT & obj, OBJECTSTRUCT & Tarobj)
{
	if( obj.Type != OBJ_USER )
	{
		return;
	}
	// ----
	char szName[11] = { 0 };
	memcpy(szName, obj.Name, 11);
	_tagArcaBattleUserInfo * pUserInfo = this->GetUserInfo(szName);
	// ----
	if( pUserInfo == NULL )
	{
		return;
	}
	// ----
	if( Tarobj.Type == OBJ_MONSTER )
    {
		if(		Tarobj.Class == 598
			||	Tarobj.Class == 599
			||	Tarobj.Class == 600
			||	Tarobj.Class == 601
			||	Tarobj.Class == 602 )
		{
			pUserInfo->m_stAcquiredPoints.dwKillPoints += AB_REW_KILL_OBELISK;
			LogAddTD("[ArcaBattle] Obelisk Occupy Add KillPoints [%s][%s] Point[%d]",
				obj.AccountID, obj.Name, pUserInfo->m_stAcquiredPoints.dwKillPoints);
		}
		else if(Tarobj.Class == 592
			||	Tarobj.Class == 593
			||	Tarobj.Class == 594
			||	Tarobj.Class == 595
			||	Tarobj.Class == 596 )
		{
			for( int i = 0; i < AB_MAX_MONGROUP; i++ )
			{
				int nMapNumber = this->m_stMonsterGroupInfo[i].m_nMapNumber;
				// ----
				if( nMapNumber < 0 )
				{
					break;
				}
				// ----
				for( int j = 0; j < AB_MAX_MONGROUP; j++ )
				{
					if( Tarobj.m_Index == this->m_stMonsterGroupInfo[i].m_stMonsterAccountNumInfo[j].m_nIndex )
					{
						if( this->m_stMonsterGroupInfo[i].m_stMonsterAccountNumInfo[j].bItemDrop == false )
						{
							this->m_stMonsterGroupInfo[i].m_stMonsterAccountNumInfo[j].bItemDrop = true;
							//ItemSerialCreateSend(obj.m_Index, obj.MapNumber, (BYTE)obj.X, (BYTE)obj.Y,
							//	AB_REW_KILL_TROPHY, 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0);
							ItemSerialCreateSend(obj.m_Index, obj.MapNumber, (BYTE)obj.X, (BYTE)obj.Y,
								AB_REW_KILL_TROPHY, 0, 1, 0, 0, 0, -1, 0, 0);
							LogAddTD("[ArcaBattle] Monster Dropped ArcaBattle Item Booty To [%s][%s]", 
								obj.AccountID, obj.Name);
						}
					}
				}
			}
			// ----
			pUserInfo->m_stAcquiredPoints.dwKillPoints += AB_REW_KILL_MONSTER;
			LogAddTD("[ArcaBattle] Monster Kill Add KillPoints [%s][%s] Point[%d]", 
				obj.AccountID, obj.Name, pUserInfo->m_stAcquiredPoints.dwKillPoints);
		}
	}
	else if( Tarobj.Type == OBJ_USER )
	{
		pUserInfo->m_stAcquiredPoints.dwKillPoints += AB_REW_KILL_USER;
		LogAddTD("[ArcaBattle] User Kill Add KillPoints [%s][%s] Point[%d]",
			obj.AccountID, obj.Name, pUserInfo->m_stAcquiredPoints.dwKillPoints);
		// ----
		char szTarName[11] = { 0 };
		memcpy(szTarName, Tarobj.Name, 11);
		_tagArcaBattleUserInfo * pTarUserInfo = this->GetUserInfo(szTarName);
		// ----
		if( pTarUserInfo == NULL )
		{
			return;
		}
		// ----
		if( pTarUserInfo->bBootyDrop == false )
		{
			pTarUserInfo->bBootyDrop = true;
			//ItemSerialCreateSend(obj.m_Index, obj.MapNumber, (BYTE)obj.X, (BYTE)obj.Y,
			//	AB_REW_KILL_TROPHY, 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0);
			ItemSerialCreateSend(obj.m_Index, obj.MapNumber, (BYTE)obj.X, (BYTE)obj.Y,
				AB_REW_KILL_TROPHY, 0, 1, 0, 0, 0, -1, 0, 0);
			 LogAddTD("[ArcaBattle] [%s][%s] User Dropped ArcaBattle Item Booty [%s][%s]",
                obj.AccountID, obj.Name, Tarobj.AccountID, Tarobj.Name);
		}
	}
}
// -------------------------------------------------------------------------------

//00724c50
void CArcaBattle::BootyItemGetCnt(OBJECTSTRUCT & obj)
{
	if( obj.Type != OBJ_USER )
	{
		return;
	}
	// ----
	char szName[11] = { 0 };
	memcpy(szName, obj.Name, 11);
	_tagArcaBattleUserInfo * pUserInfo = this->GetUserInfo(szName);
	// ----
	if( pUserInfo == NULL )
	{
		return;
	}
	// ----
	pUserInfo->m_stAcquiredPoints.dwBootyCnt++;
	LogAddTD("[ArcaBattle] BootyItemGetCnt [%s][%s] Total BootyCnt[%d]",
		obj.AccountID, obj.Name, pUserInfo->m_stAcquiredPoints.dwBootyCnt);
}
// -------------------------------------------------------------------------------

//00724d00
void CArcaBattle::CalcRewardExp(OBJECTSTRUCT & obj, __int64 & iRewardExp)
{
	if( obj.Type != OBJ_USER )
	{
		return;
	}
	// ----
	char szName[11] = { 0 };
	memcpy(szName, obj.Name, 11);
	_tagArcaBattleUserInfo * pUserInfo = this->GetUserInfo(szName);
	// ----
	if( pUserInfo == NULL )
	{
		return;
	}
	// ----
	DWORD dwKillPoints = pUserInfo->m_stAcquiredPoints.dwKillPoints;
	// ----
	if( dwKillPoints >= 0 && dwKillPoints <= 10 )
	{
		iRewardExp = AB_REW_KILL_EXP1;
	}
	else if( dwKillPoints >= 11 && dwKillPoints <= 50 )
	{
		iRewardExp = AB_REW_KILL_EXP2;
	}
	else if( dwKillPoints >= 51 && dwKillPoints <= 100 )
	{
		iRewardExp = AB_REW_KILL_EXP3;
	}
	else if( dwKillPoints >= 101 && dwKillPoints <= 300 )
	{
		iRewardExp = AB_REW_KILL_EXP4;
	}
	else if( dwKillPoints >= 301 && dwKillPoints <= 500 )
	{
		iRewardExp = AB_REW_KILL_EXP5;
	}
	else if( dwKillPoints >= 501 )
	{
		iRewardExp = AB_REW_KILL_EXP6;
	}
	// ----
	LogAddTD("[ArcaBattle] CalcRewardExp [%s][%s] Total KillPoints[%d] Exp[%U]",
		obj.AccountID, obj.Name, dwKillPoints, iRewardExp);
}
// -------------------------------------------------------------------------------

//00724e70
BOOL CArcaBattle::DropItem(LPOBJ lpObj, BYTE btMapNumber, BYTE cX, BYTE cY, int nMaxHitUser)
{
	if( btMapNumber != MAP_INDEX_ACHERON )
	{
		return false;
	}
	// ----
	if( gObjCheckUsedBuffEffect(lpObj, 187) == false )	//Reward?
	{
		return false;
	}
	// ----
//	if( this->m_pAcheronProbability.DropArcaBattleRewardSetItem(lpObj->m_Index, btMapNumber, cX, cY) == true )
//	{
//		return true;
//	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------

//00724ee0
void CArcaBattle::ProbabilityLoadData()
{
/*	if( this->m_pAcheronProbability != false )	
	{
		delete this->m_pAcheronProbability;
	}
	// ----
	this->m_pAcheronProbability = new CProbabilityItemBag;
	// ----
	if( this->m_pAcheronProbability == NULL )
	{
		MsgBox( "[ArcaBattle] AcheronProbability.txt %s", lMsg.Get(MSGGET(0, 110)));
		return;
	}
	// ----
	this->m_pAcheronProbability->ProbabilityItemBagInit(this->m_pAcheronProbability, AB_REW_ITEMBAG);*/
}
// -------------------------------------------------------------------------------

//00724fe0
void CArcaBattle::GDReqDeleteArcaBattleInfo()
{
	_tagPMSG_REQ_ARCA_BATTLE_INFO_DS pMsg;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_REQ_ARCA_BATTLE_INFO_DS);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x3D;
	// ----
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
// -------------------------------------------------------------------------------

//00725010
void CArcaBattle::GDReqArcaBattleProcMultiCast(int iState)
{
	_tagPMSG_REQ_AB_PROC_INSERT_DS pMsg;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_REQ_AB_PROC_INSERT_DS);
	pMsg.h.headcode	= 0xF8;
	pMsg.h.subcode	= 0x3E;
	pMsg.btProcState= iState;
	pMsg.wMapSvrNum	= g_MapServerManager.GetMapSvrGroup();
	// ----
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
// -------------------------------------------------------------------------------

//00725060
void CArcaBattle::GDReqJoinMemberUnder()
{
	_tagPMSG_REQ_AB_JOIN_MEMBER_UNDER_DS pMsg;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_REQ_AB_JOIN_MEMBER_UNDER_DS);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x41;
	pMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();
	// ----
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
// -------------------------------------------------------------------------------

//007250a0
void CArcaBattle::GDReqArcaBattleJoinMemberUnderReq()
{
	_tagPMSG_REQ_AB_JOIN_CANCEL_DS pMsg;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_REQ_AB_JOIN_CANCEL_DS);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x43;
	pMsg.btMinGuildMemNum = this->GetJoinMemberCnt();
	pMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();
	// ----
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
// -------------------------------------------------------------------------------

//007250f0
void CArcaBattle::DGAnsArcaBattleJoinMemberUnderReq(_tagPMSG_ANS_AB_JOIN_CANCEL_DS * lpMsg)
{
	_tagPMSG_SEND_AB_JOIN_CANCEL pMsg;
	int iGuildCnt = lpMsg->btGuildCnt;
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_SEND_AB_JOIN_CANCEL);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x40;
	// ----
	for( int i = 0; i < iGuildCnt; i++ )
	{
#if( AB_DEBUG_TRACE == 1 )
		TRACE_LOG("Cancel Guild [%s]", lpMsg->CancelGuildNames[i].szGuildNames);
#endif
		// ----
		_GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(lpMsg->CancelGuildNames[i].szGuildNames);
		// ----
		if( lpGuild != NULL )
		{
			for( int n = 0; n < MAX_USER_GUILD; n++ )
			{
				if( gObjIsConnected(n) == 1 )
				{
					int aIndex = lpGuild->Index[n];
					DataSend(aIndex, (LPBYTE)&pMsg, sizeof(_tagPMSG_SEND_AB_JOIN_CANCEL));
					LogAddTD("[ArcaBattle] Canceled With Less Than Guild Member - GuildName[%s]", lpMsg->CancelGuildNames[i].szGuildNames);
				}
			}
		}
	}
}
// -------------------------------------------------------------------------------

//007251f0
void CArcaBattle::DGAnsRegisteredMemberCnt(_tagPMSG_ANS_AB_REG_MEMBER_CNT_DS * lpMsg)
{
	int aIndex;
	_tagPMSG_ANS_AB_REG_MEMBER_CNT pMsg;
	// ----
	int nResult = 0;
	aIndex		= lpMsg->wNumber;
	// ----
	if( !lpMsg->btRegMemCnt )
	{
		nResult = 2;
	}
	// ----
	pMsg.h.c		= 0xC1;
	pMsg.h.size		= sizeof(_tagPMSG_ANS_AB_REG_MEMBER_CNT);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode	= 0x42;
	pMsg.btRegMemCnt= lpMsg->btRegMemCnt;
	pMsg.btResult	= nResult;
	// ----
	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(_tagPMSG_ANS_AB_REG_MEMBER_CNT));
}
// -------------------------------------------------------------------------------

//00725260
BOOL CArcaBattle::GetGuildMasterPos(OBJECTSTRUCT & obj, int nGateNum, BYTE & x, BYTE & y)
{
	if( obj.lpGuild == NULL )
	{
		return false;
	}
	// ----
	BYTE attr = MapC[obj.MapNumber].GetAttr(obj.X, obj.Y);
	// ----
	if( obj.GuildStatus == GUILD_MASTER )
	{
		this->SetGuildMasterGateNum(nGateNum, obj.GuildNumber);
		return false;
	}
	// ----
	if( gObjCheckUsedBuffEffect(&obj, 184) == true ) //Reward?
	{
		return false;
	}
	// ----
	if( !(attr & 1) )
	{
		return false;
	}
	// ----
	for( int n = 0; n < obj.lpGuild->Count; n++ )
	{
		if( obj.lpGuild->GuildStatus[n] == GUILD_MASTER )
		{
			int iMasterIndex = obj.lpGuild->Index[n];
			// ----
			if( gObjIsConnected(iMasterIndex) == 1 )
			{
				attr = MapC[gObj[iMasterIndex].MapNumber].GetAttr(gObj[iMasterIndex].X, gObj[iMasterIndex].Y);
				// ----
				if( (attr & 1) )
				{
					if( this->GetGuildMasterGateNum(obj.GuildNumber) != nGateNum )
					{
						return false;
					}
					// ----
					this->GetBoxPosition(gObj[iMasterIndex].MapNumber, gObj[iMasterIndex].X, gObj[iMasterIndex].Y, 
						gObj[iMasterIndex].X + 3, gObj[iMasterIndex].Y + 3, x, y);
					return true;
				}
				else if( !(attr & 1) )
				{
					this->SetGuildMasterGateNum(0, obj.GuildNumber);
					return false;
				}
			}
		}
	}
	// ----
	return true;
}
// -------------------------------------------------------------------------------

//007254d0
void CArcaBattle::SetGuildMasterGateNum(int nGateNum, int nGuildNum)
{
	int nBlank = -1;
	// ----
	for( int i = 0; i < AB_MAX_GATE; i++ )
	{
		if( this->m_stGMasterGateNum[i].nGuildNum == nGuildNum )
		{
			this->m_stGMasterGateNum[i].nGateNum = nGateNum;
			return;
		}
		// ----
		if( nBlank < 0 )
		{
			nBlank = i;
		}
		// ----
		if( this->m_stGMasterGateNum[i].nGuildNum == -1 )
		{
			this->m_stGMasterGateNum[i].nGuildNum	= nGuildNum;
			this->m_stGMasterGateNum[i].nGateNum	= nGateNum;
			return;
		}
	}
}
// -------------------------------------------------------------------------------

//00725570
int CArcaBattle::GetGuildMasterGateNum(int nGuildNum)
{
	for( int i = 0; i < AB_MAX_GATE; i++ )
	{
		if( this->m_stGMasterGateNum[i].nGuildNum == nGuildNum )
		{
			return this->m_stGMasterGateNum[i].nGateNum;
		}
	}
	// ----
	return -1;
}
// -------------------------------------------------------------------------------

//007255c0
BOOL CArcaBattle::GetUserPos(int nGateNum, BYTE & nOutX, BYTE & nOutY)
{
	int nMapNumber;
	int nBeginX;
	int nEndX;
	int nGroupNumber;
	int nBeginY;
	int nArea;
	int nEndY;
	// ----
	nGroupNumber = 0;
	// ----
	if (nGateNum == 427)
	{
		nGroupNumber = 0;
	}
	else if (nGateNum == 431)
	{
		nGroupNumber = 1;
	}
    else if (nGateNum == 439)
    {
		nGroupNumber = 2;
	}
	else
	{
		return 1;
	}
	// ----
	nArea		= rand() % this->m_stAB_UserPos[nGroupNumber].area_cnt;
	nMapNumber	= this->m_stAB_UserPos[nGroupNumber].stUserPos[nArea].map_number;
	nBeginX		= this->m_stAB_UserPos[nGroupNumber].stUserPos[nArea].beginX;
	nBeginY		= this->m_stAB_UserPos[nGroupNumber].stUserPos[nArea].beginY;
	nEndX		= this->m_stAB_UserPos[nGroupNumber].stUserPos[nArea].endX;
	nEndY		= this->m_stAB_UserPos[nGroupNumber].stUserPos[nArea].endY;
	// ----
	if( nMapNumber == -1 )
	{
		return false;
	}
	// ----
	if( this->GetBoxPosition(nMapNumber, nBeginX, nBeginY, nEndX, nEndY, nOutX, nOutY) == false )
	{
		return false;
	}
	// ----
	return true;
}
// -------------------------------------------------------------------------------

void CArcaBattle::SetUserMapInfo(LPOBJ lpObj, int nGateNum)
{
//	for ( i = 0; i < this->m_iABJoinUserCnt; ++i )
 // {
 //   if ( !strcmp(lpObj->Name, this->m_stABJoinUserInfo[i].szUserName) )
//      this->m_stABJoinUserInfo[i].wGuildMasterGateNum = nGateNum;
 // }
}

//00725700
void CArcaBattle::PrintGuildMemberLog(_GUILD_INFO_STRUCT * lpGuild)
{
	if( lpGuild == NULL )
	{
		LogAddTD("[ArcaBattle] Error : lpGuild Is NULL - PrintGuildMemberLog()");
		return;
	}
	// ----
	int nBlank = -1;
	// ----
	for( int i = 0; i < AB_MAX_INFOLOG; i++ )
    {
		if( this->m_stGuildMemberInfoLog[i].nGuildNum >= 0 )
		{
			if( this->m_stGuildMemberInfoLog[i].nGuildNum == lpGuild->Number )
			{
				return;
			}
		}
		// ----
		if( nBlank < 0 )
		{
			nBlank = i;
		}
		// ----
		this->m_stGuildMemberInfoLog[nBlank].nGuildNum = lpGuild->Number;
		// ----
		LogAddTD("************************************************************************");
		LogAddTD("[ArcaBattle] Guild Name [%s] GuildMemberCnt [%d]", lpGuild->Name, lpGuild->Count);
		for( int n = 0; n < MAX_USER_GUILD; n++ )
		{
			if( lpGuild->Use[n] )
			{
				LogAddTD("[ArcaBattle] [%s] Guild Member (%d)[%s]", lpGuild->Name, n + 1, lpGuild->Names[n]);
			}
		}
		LogAddTD("************************************************************************");
	}
}
// -------------------------------------------------------------------------------