// CrywolfUtil.cpp: implementation of the CCrywolfUtil class.
//	GS-N	1.00.18	JPN	0x0056F8E0
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrywolfUtil.h"
#include "DSProtocol.h"
#include "MapServerManager.h"
#include "TNotice.h"
#include "..\common\classdef.h"
#include "LogProc.h"
#include "Gamemain.h"
#include "MasterSkillSystem.h"
#include "ClassCalc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrywolfUtil::CCrywolfUtil()
{
	return;
}

CCrywolfUtil::~CCrywolfUtil()
{
	return;
}


void CCrywolfUtil::SendMapServerGroupMsg(LPSTR lpszMsg, ...)
{
	if ( !lpszMsg )
		return;

	char szBuffer[512] = "";
	va_list	pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	GS_GDReqMapSvrMsgMultiCast(g_MapServerManager.GetMapSvrGroup(), szBuffer);
}


void CCrywolfUtil::SendAllUserAnyData(LPBYTE lpMsg, int iSize)
{
	for ( int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
	{
		if ( gObj[i].Connected == PLAYER_PLAYING )
		{
			if ( gObj[i].Type == OBJ_USER )
			{
				DataSend(i, lpMsg, iSize);
			}
		}
	}
}



void CCrywolfUtil::SendAllUserAnyMsg(int iType, LPSTR lpszMsg, ...)
{
	if ( !lpszMsg )
		return;

	char szBuffer[512] = "";
	va_list	pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	PMSG_NOTICE pNotice;

	switch ( iType )
	{
		case 1:
			TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
			TNotice::SetNoticeProperty(&pNotice, 10, _ARGB(255, 255, 200, 80), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pNotice);
			break;

		case 2:
			TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
			TNotice::SendNoticeToAllUser(&pNotice);
			break;
	}
}



void CCrywolfUtil::SendCrywolfUserAnyData(LPBYTE lpMsg, int iSize)
{
	for ( int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
	{
		if ( gObj[i].Connected == PLAYER_PLAYING )
		{
			if ( gObj[i].Type == OBJ_USER )
			{
				if ( gObj[i].MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
				{
					DataSend(i, lpMsg, iSize);
				}
			}
		}
	}
}



void CCrywolfUtil::SendCrywolfUserAnyMsg(int iType, LPSTR lpszMsg, ...)
{
	if ( !lpszMsg )
		return;

	char szBuffer[512] = "";
	va_list	pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	PMSG_NOTICE pNotice;

	switch ( iType )
	{
		case 1:
			TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
			TNotice::SetNoticeProperty(&pNotice, 10, _ARGB(255, 255, 200, 80), 1, 0, 20);
			break;

		case 2:
			TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
			break;
	}

	for ( int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
	{
		if ( gObj[i].Connected == PLAYER_PLAYING )
		{
			if ( gObj[i].Type == OBJ_USER )
			{
				if ( gObj[i].MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
				{
					DataSend(i, (LPBYTE)&pNotice, pNotice.h.size);
				}
			}
		}
	}

}

#pragma warning ( disable : 4101 )
void CCrywolfUtil::SendCrywolfChattingMsg(int iObjIndex, LPSTR lpszMsg, ...)
{
	return;

	if( !OBJMAX_RANGE(iObjIndex) )
		return;

	LPOBJ lpObj = &gObj[iObjIndex];
	char szBuffer[512];
	va_list pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	char szChat[60];

	strncpy(szChat,szBuffer,59);

	for (int i;i<MAX_VIEWPORT_MONSTER;i++)
	{
		if ( lpObj->VpPlayer2[i].state != FALSE )
		{
			int iTargetNumber = lpObj->VpPlayer2[i].number;

			if ( OBJMAX_RANGE(iTargetNumber) != FALSE )
			{
				ChatTargetSend(lpObj, szChat, iTargetNumber);
			}
		}
	}
}
#pragma warning ( default : 4101 )



int CCrywolfUtil::CrywolfMVPLevelUp(int iUserIndex,int iAddExp)
{
	if ( !OBJMAX_RANGE(iUserIndex ) )
		return 0;

	LogAddTD("[ Crywolf ][MVP Exp.] : [%s][%s](%d) %u %d",
		gObj[iUserIndex].AccountID, gObj[iUserIndex].Name,
		gObj[iUserIndex].Level, gObj[iUserIndex].Experience,
		iAddExp);

	if( g_MasterExp.LevelUp(&gObj[iUserIndex],iAddExp) )
	{
		return FALSE;
	}

	gObjSetExpPetItem(iUserIndex, iAddExp);
	int iLEFT_EXP = 0;

	LogAddTD("Experience : Map[%d]-(%d,%d) [%s][%s](%d) %u %d MonsterIndex : %d, EventType : %d",
		gObj[iUserIndex].MapNumber, gObj[iUserIndex].X, gObj[iUserIndex].Y,
		gObj[iUserIndex].AccountID, gObj[iUserIndex].Name,
		gObj[iUserIndex].Level, gObj[iUserIndex].Experience, iAddExp, 0, EVENT_TYPE_CRYWOLF);

	if ( gObj[iUserIndex].Level >= MAX_CHAR_LEVEL )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 112)), gObj[iUserIndex].m_Index, 1);
		return 0;
	}

	if ( (gObj[iUserIndex].Experience + iAddExp) < gObj[iUserIndex].NextExp )
	{
		gObj[iUserIndex].Experience += iAddExp;
	}
	else
	{
		iLEFT_EXP = gObj[iUserIndex].Experience + iAddExp - gObj[iUserIndex].NextExp;
		gObj[iUserIndex].Experience = gObj[iUserIndex].NextExp;
		gObj[iUserIndex].Level++;
#if (ENABLE_CUSTOM_CLASSCALC == 1)
		gObj[iUserIndex].LevelUpPoint += g_ClassCalc.GetLevelPoint(&gObj[iUserIndex], 0, 0);
#else
		if( gObj[iUserIndex].Class == CLASS_DARKLORD )
		{
			gObj[iUserIndex].LevelUpPoint += 7;
		}
		else if( gObj[iUserIndex].Class == CLASS_MAGUMSA )
		{
			gObj[iUserIndex].LevelUpPoint += 7;
		}
		else if( gObj[iUserIndex].Class == CLASS_FIGHTER )
		{
			gObj[iUserIndex].LevelUpPoint += 7;
		}
		else
		{
			gObj[iUserIndex].LevelUpPoint += 5;
		}
#endif

		if( gObj[iUserIndex].PlusStatQuestClear != false )
		{
#if (ENABLE_CUSTOM_CLASSCALC == 1)
			gObj[iUserIndex].LevelUpPoint += g_ClassCalc.GetLevelPoint(&gObj[iUserIndex], 0, 1);
#else
			gObj[iUserIndex].LevelUpPoint += 1;
#endif
			LogAddTD("[ Crywolf ][MVP Exp.] [%s][%s] LevelUp PlusStatQuest Clear AddStat %d",
				gObj[iUserIndex].AccountID, gObj[iUserIndex].Name,
				gObj[iUserIndex].LevelUpPoint);
		}

		gObj[iUserIndex].MaxLife += DCInfo.DefClass[gObj[iUserIndex].Class].LevelLife;
		gObj[iUserIndex].MaxMana += DCInfo.DefClass[gObj[iUserIndex].Class].LevelMana;
		gObj[iUserIndex].Life = gObj[iUserIndex].MaxLife;
		gObj[iUserIndex].Mana = gObj[iUserIndex].MaxMana;
		gObjNextExpCal(&gObj[iUserIndex]);
		gObjSetBP(gObj[iUserIndex].m_Index);
		GCLevelUpMsgSend(gObj[iUserIndex].m_Index, 1);
		LogAddTD(lMsg.Get(MSGGET(2, 8)), gObj[iUserIndex].AccountID,
			gObj[iUserIndex].Name, gObj[iUserIndex].Level);


		if( gObj[iUserIndex].Level == 400 && gObj[iUserIndex].PartyNumber >= 0 )
		{
			int partynumber = gObj[iUserIndex].PartyNumber;
			char szTmp[256];

			sprintf(szTmp,"400 LevelUp (%s)(%s) Party ",gObj[iUserIndex].AccountID,gObj[iUserIndex].Name);

			int tObjNum;

			for(int i = 0; i < 5; i++)
			{
				tObjNum = gParty.m_PartyS[partynumber].Number[i];
				if( tObjNum >= 0 )
				{
					int len = strlen(szTmp);
					sprintf(&szTmp[len],",(%s)(%s) ",gObj[tObjNum].AccountID,gObj[tObjNum].Name);
				}
			}

			LogAddTD(szTmp);
		}
	}

	return iLEFT_EXP;
}
	




