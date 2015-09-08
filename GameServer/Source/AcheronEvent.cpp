#include "stdafx.h"
#include "AcheronEvent.h"
#include "GameMain.h"
#include "..\common\readscript.h"
#include "LargeRand.h"
#include "TNotice.h"
#include "ArcaBattle.h"
#include "MapServerManager.h"
#include "DBSockMng.h"
#include "DSProtocol.h"
#include "EventItemBag.h"
#include "..\pugixml\pugixml.hpp"

using namespace pugi;
CAcheronGuardianEvent g_AcheronGuardianEvent;

CAcheronGuardianEvent::CAcheronGuardianEvent()
{
	this->m_iREMAIN_MSEC = 0;
	this->m_iTICK_COUNT = 0;
	this->m_iState = 1;
	this->m_bAE_PLAY_START = 0;
//	this->m_pAcheronEventRewardItem = 0;
	this->m_bAEInitState = 0;
	this->m_bWindowClose = 0;
}

CAcheronGuardianEvent::~CAcheronGuardianEvent()
{
	//del m_pAcheronEventRewardItem
}

int CAcheronGuardianEvent::LoadScriptAcheronEvent(LPSTR lpszFileName)
{
	if (lpszFileName == NULL || !strcmp(lpszFileName, ""))
	{
		MsgBox("[AcheronGuardianEvent] file load error - File Name Error");
		return 0;
	}

	SMDFile = fopen(lpszFileName, "r");

	if (!SMDFile)
	{
		MsgBox("[AcheronGuardianEvent] file load error - fopen() : %s");
		return 0;
	}

	int type = -1;
	int iDayOfWeekCycleCnt = 0;

	while (true)
	{
		SMDToken Token = GetToken();

		if (Token == END)
		{
			break;
		}

		type = TokenNumber;

		if (Token == NUMBER)
		{
			if (type == 1)
			{
				while (true)
				{
					Token = GetToken();

					if (!strcmp("end", TokenString))
					{
						break;
					}

					this->m_iDayOfWeekCycle[iDayOfWeekCycleCnt] = TokenNumber;
					iDayOfWeekCycleCnt++;
				}
			}
			else if (type == 2)
			{
				while (true)
				{
					Token = GetToken();

					if (!strcmp("end", TokenString))
					{
						break;
					}
			
					int iAB_PlayTimeMin = TokenNumber;
					Token = GetToken();
					int iAB_ChCloseMin = TokenNumber;
					this->m_iAB_PlayTimeMin = iAB_PlayTimeMin;
					this->m_iAB_ChCloseMin = iAB_ChCloseMin;
				}
			}
			else if (type == 3)
			{
				while (true)
				{
					Token = GetToken();

					if (!strcmp("end", TokenString))
					{
						break;
					}

					_tagAcheronEventTime pRET;
					pRET.m_iHour = TokenNumber;
					Token = GetToken();
					pRET.m_iMinute = TokenNumber;
					Token = GetToken();
					pRET.m_btEvent = TokenNumber;
					this->m_vtAcheronEventTime.push_back(pRET);
				}
			}
			else if (type == 4)
			{
				while (true)
				{
					Token = GetToken();

					if (!strcmp("end", TokenString))
					{
						break;
					}

					int nGourpNumber = (int)TokenNumber - 1;

					if (nGourpNumber < 0 || nGourpNumber > 5)
					{
						LogAdd("[ArcaBattle][MonsterGroupRegen] Invalid Aura groupNumber");
						return 0;
					}

					this->m_stObeliskInfo[nGourpNumber].m_iGroupNumber = TokenNumber;
					this->m_stObeliskInfo[nGourpNumber].m_iAttrKind = 1;
					Token = GetToken();
					this->m_stObeliskInfo[nGourpNumber].m_iMapIndex = TokenNumber;
					Token = GetToken();
					this->m_stObeliskInfo[nGourpNumber].m_iPosX = TokenNumber;
					Token = GetToken();
					this->m_stObeliskInfo[nGourpNumber].m_iPosY = TokenNumber;
				}
			}
			else if (type == 5)
			{
				while (true)
				{
					Token = GetToken();

					if (!strcmp("end", TokenString))
					{
						break;
					}
					
					int nTempGourpNumber = TokenNumber;

					if (nTempGourpNumber < 0 || nTempGourpNumber > 50)
					{
						LogAdd("[AcheronGuardianEvent][MonsterGroupRegen] Invalid monster groupNumber");
						return 0;
					}

					this->m_stMonsterGroupInfo[nTempGourpNumber].m_nGroupNumber = nTempGourpNumber;
					Token = GetToken();
					this->m_stMonsterGroupInfo[nTempGourpNumber].m_nMapNumber = TokenNumber;
					Token = GetToken();
					this->m_stMonsterGroupInfo[nTempGourpNumber].m_nCnt = TokenNumber;
					Token = GetToken();
					this->m_stMonsterGroupInfo[nTempGourpNumber].m_nRegenTimeMin = TokenNumber;
				}
			}
			else if (type == 6)
			{
				int nAreaCnt = 0;
				int nTempGroupNumber = 0;

				while (true)
				{
					Token = GetToken();

					if (!strcmp("end", TokenString))
					{
						break;
					}

					int nGroupNumber = TokenNumber;
					
					if (nTempGroupNumber != nGroupNumber)
					{
						nTempGroupNumber = nGroupNumber;
						nAreaCnt = 0;
					}

					if (nGroupNumber < 0 || nGroupNumber > 50)
					{
						LogAdd("[AcheronGuardianEvent][MonsterGroupRegen] Invalid monster groupNumber");
						return 0;
					}

					this->m_stMonsterPosition[nGroupNumber].m_nGroupNumber = nGroupNumber;
					Token = GetToken();
					this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nAreaCnt].m_nBeginX = TokenNumber;
					Token = GetToken();
					this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nAreaCnt].m_nBeginY = TokenNumber;
					Token = GetToken();
					this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nAreaCnt].m_nEndX = TokenNumber;
					Token = GetToken();
					this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nAreaCnt].m_nEndY = TokenNumber;
					nAreaCnt++;
					this->m_stMonsterPosition[nGroupNumber].m_nRegenAreaCnt = nAreaCnt;
				}
			}
			else if (type == 7)
			{
				int nArrayIndex = 0;
				int nTempGroupNumber = 0;

				while (true)
				{
					Token = GetToken();

					if (!strcmp("end", TokenString))
					{
						break;
					}
						
					int nGroupNumber = TokenNumber;
					
					if (nTempGroupNumber != nGroupNumber)
					{
						nTempGroupNumber = nGroupNumber;
						nArrayIndex = 0;
					}

					if (nGroupNumber < 0 || nGroupNumber > 50)
					{
						LogAdd("[AcheronGuardianEvent][MonsterGroupRegen] Invalid monster groupNumber");
						return 0;
					}

					if (this->m_stMonsterPosition[nGroupNumber].m_nGroupNumber < 0)
					{
						LogAdd("[AcheronGuardianEvent][MonsterGroupRegen] Invalid groupNumber(%d)", nGroupNumber);
						return 0;
					}

					Token = GetToken();
					this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[nArrayIndex].m_nMonsterIndex = TokenNumber;
					Token = GetToken();
					this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[nArrayIndex].m_nGenSqe = TokenNumber;
					Token = GetToken();
					this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[nArrayIndex].m_nMonAttr = TokenNumber;
					nArrayIndex++;

					if (nArrayIndex >= 20)
					{
						LogAdd("[AcheronGuardianEvent][MonsterGroupRegen] Out of Max MonsterKindIndex ");
						break;
					}

					this->m_stMonsterPosition[nGroupNumber].m_nMonsterKindCnt = nArrayIndex;
				}
			}
			else if (type == 8)
			{
				int iCount = 0;

				while (true)
				{
					Token = GetToken();

					if (!strcmp("end", TokenString))
					{
						break;
					}

					this->m_stItemDropCntRate[iCount].iCnt = TokenNumber;
					Token = GetToken();
					this->m_stItemDropCntRate[iCount].iRate = TokenNumber;
					iCount++;
				}
			}
		}
	}
	
	fclose(SMDFile);
	return 0;
}

void CAcheronGuardianEvent::SetAcheronEventProcMultiCast(int bABInitState)
{
	this->m_bAEInitState = bABInitState;
	this->GDReqAcheronEventProcMultiCast(this->IsPlayStart());
}

int CAcheronGuardianEvent::GetDropItemCnt()
{
	DWORD dwRandTmp = GetLargeRand() % 1000000;
	DWORD dwRandRate = 0;
	  
	for (int i = 0; i < 5; i++)
	{
		dwRandRate += this->m_stItemDropCntRate[i].iRate;

		if (dwRandTmp <= dwRandRate)
		{
			return this->m_stItemDropCntRate[i].iCnt;
		}
	}

	return -1;
}

void CAcheronGuardianEvent::Clear()
{
	for (int i = 0; i < 5; i++)
	{
		this->m_stObeliskInfo[i].Clear();
	}
	
	for (int nGroupNumber = 0; nGroupNumber < 5; nGroupNumber++)
	{
		this->m_stMonsterGroupInfo[nGroupNumber].m_nGenTotCnt = 0;
	}
	
	this->m_bAE_PLAY_START = 0;
	this->m_bWindowClose = 0;
}

int CAcheronGuardianEvent::CheckSync()
{
//	if (!g_ArcaBattle.IsArcaBattleServer())
//	{
//		return 0;
//	}

	_SYSTEMTIME tmToDay;
	GetLocalTime(&tmToDay);
	int iTODAY_DATE_NUM = MAKELONG(MAKEWORD(tmToDay.wDay, tmToDay.wMonth), tmToDay.wYear);
	WORD wDayOfWeek = tmToDay.wDayOfWeek;

	if (wDayOfWeek >= 7)
	{
		LogAddC(2, "[AcheronEvent] Error DayOfWeek -  wDayOfWeek [%d]", wDayOfWeek);
		return 0;
	}

	if (this->m_iDayOfWeekCycle[wDayOfWeek] != 1)
	{
		LogAddTD("[AcheronEvent] Disabled for today");
		return 0;
	}

	if (this->m_vtAcheronEventTime.empty())
	{
		LogAddC(2, "[AcheronEvent] No Schedule Time Data -  (m_vtAcheronEventTime.empty())");
		return 0;
	}

	_SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
    int iMIN_HOUR = 24;
    int iMIN_MINUTE = 60;
    int bTIME_CHANGED = 0;
	std::vector<_tagAcheronEventTime>::iterator it = this->m_vtAcheronEventTime.begin();
	_tagAcheronEventTime* pRET;

	while (it != this->m_vtAcheronEventTime.end())
	{
		//pRET = (_tagAcheronEventTime*)it;

		if (sysTime.wMinute + 60 * sysTime.wHour < it->m_iMinute + 60 * it->m_iHour)
		{
			if (iMIN_MINUTE + 60 * iMIN_HOUR > it->m_iMinute + 60 * it->m_iHour)
			{
                bTIME_CHANGED = 1;
                iMIN_HOUR = it->m_iHour;
				iMIN_MINUTE = it->m_iMinute;
			}
		}

		it++;
	}

	if (bTIME_CHANGED == 0)
	{
		iMIN_HOUR = 24;
		iMIN_MINUTE = 60;
		it = this->m_vtAcheronEventTime.begin();

		while (it != this->m_vtAcheronEventTime.end())
		{
			//pRET = (_tagAcheronEventTime*)it;
			
			if (iMIN_MINUTE + 60 * iMIN_HOUR > it->m_iMinute + 60 * it->m_iHour)
			{
                bTIME_CHANGED = 2;
                iMIN_HOUR = it->m_iHour;
                iMIN_MINUTE = it->m_iMinute;
			}

			it++;
		}
	}

	if (bTIME_CHANGED == 1)
	{
		this->m_iREMAIN_MSEC = 1000 * (60 * iMIN_MINUTE + 3600 * iMIN_HOUR - (sysTime.wSecond + 60 * sysTime.wMinute + 3600 * sysTime.wHour));
		this->SetProcState(2);
	}
	else if (bTIME_CHANGED == 2)
	{
		this->m_iREMAIN_MSEC = -100;
	}
	else
	{
		LogAddC(2, "[AcheronEvent] No Schedule Time Data - Chaos Castle Terminated (bTIME_CHANGED=%d)", bTIME_CHANGED);
		return 0;
	}

	this->m_iTICK_COUNT = GetTickCount();
	//TRACE_LOG("m_iREMAIN_MSEC %d", this->m_iTICK_COUNT);
	//LogAddTD("[AcheronEvent] Sync Start Time. [%d] min remain (START HOUR:%d, MIN:%d)",
	//	this->m_iREMAIN_MSEC / 60000, iMIN_HOUR, iMIN_MINUTE);
	return 1;
}

void CAcheronGuardianEvent::Run()
{
//	if (!g_ArcaBattle.IsArcaBattleServer())
//	{
//		return;
//	}
	//LogAddC(2, "[DEBUG] Acheron State: %d", this->m_iState);

	switch (this->m_iState)
	{
	  case AE_STATE_NONE:
		this->CheckSync();
		break;
	  case AE_STATE_CLOSED:
		this->ProcStateClosed();
		break;
	  case AE_STATE_READY:
		this->ProcStateReady();
		break;
	  case AE_STATE_PLAYING:
		this->ProcStatePlaying();
		break;
	  case AE_STATE_CLOSE:
		this->ProcStateChannelClose();
		break;
	  case AE_STATE_END:
		this->ProcStateEnd();
		break;
	}
}

void CAcheronGuardianEvent::ProcStateEnd()
{
	PMSG_NOTICE pMsg;
	TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(MSGGET(13, 186)));
	TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(0xFF, 0xFF, 0xC8, 0x50), 1, 0, 20);
	TNotice::SendNoticeToAllUser(&pMsg);
	this->DelObelisk();
	this->DeleteArcaBattleAllMonster();
	this->m_bAE_PLAY_START = 0;
	this->GDReqAcheronEventProcMultiCast(this->m_bAE_PLAY_START);
	this->m_iREMAIN_MSEC = 60000 * this->m_iAB_ChCloseMin;
	this->m_iTICK_COUNT = GetTickCount();
	this->SetProcState(5);
}

void CAcheronGuardianEvent::ProcStateClosed()
{
	if (this->m_iREMAIN_MSEC != -100)
	{
		int iTICK_MSEC = GetTickCount() - this->m_iTICK_COUNT;
		
		if (iTICK_MSEC >= 1000)
		{
			this->m_iREMAIN_MSEC -= iTICK_MSEC;
			this->m_iTICK_COUNT = GetTickCount();

			if (this->m_iREMAIN_MSEC <= 0)
			{
				this->SetProcState(3);
			}
		}
	}
}

void CAcheronGuardianEvent::ProcStateReady()
{
	this->m_iREMAIN_MSEC = 60000 * this->m_iAB_PlayTimeMin;
	this->m_iTICK_COUNT = GetTickCount();
	this->m_bAE_PLAY_START = 1;
	this->GDReqAcheronEventProcMultiCast(this->m_bAE_PLAY_START);
	this->m_iGroupNum = rand() % 3;

	if (this->m_iGroupNum < 0 || this->m_iGroupNum > 2)
	{
		LogAddTD("[AcheronGuardianEvent] Error Obelisk GroupNum [%d]", this->m_iGroupNum);
		this->m_iGroupNum = 0;
	}

	LogAddTD("[AcheronGuardianEvent] Obelisk GroupNum %d", this->m_iGroupNum);
	
	this->SetRandomObeliskAttr();
	this->GenObelisk(this->m_iGroupNum);
	this->RegenMonster(this->m_iGroupNum, 1, 10);
	this->ResetGenTime();

	char szTemp[256] = { 0 };
	sprintf(szTemp, lMsg.Get(MSGGET(13, 183)));
	this->SendMapServerGroupMsg(szTemp);
	this->SetProcState(4);
}

void CAcheronGuardianEvent::ProcStatePlaying()
{
	DWORD iTICK_MSEC = GetTickCount() - this->m_iTICK_COUNT;

	if (iTICK_MSEC >= 1000)
	{
		this->m_iREMAIN_MSEC -= iTICK_MSEC;
		this->m_iTICK_COUNT = GetTickCount();
		this->RegenMonsterRun();
	}

	if (this->m_iREMAIN_MSEC <= 0)
	{
		char szTemp[256] = { 0 };
		sprintf(szTemp, lMsg.Get(MSGGET(13, 185)));
		this->SendMapServerGroupMsg(szTemp);
		this->m_iREMAIN_MSEC = 0;
		this->DeleteArcaBattleAllMonster();
		this->SetProcState(6);
	}
}

void CAcheronGuardianEvent::ProcStatePlayEnd()
{
	//not filled
}

void CAcheronGuardianEvent::ProcStateChannelClose()
{
	DWORD iTICK_MSEC = GetTickCount() - this->m_iTICK_COUNT;

	if (iTICK_MSEC >= 1000)
	{
		this->m_iREMAIN_MSEC -= iTICK_MSEC;
		this->m_iTICK_COUNT = GetTickCount();

		if (!this->m_bWindowClose && this->m_iREMAIN_MSEC <= 10000 && this->m_iREMAIN_MSEC > 0)
		{
			_tagPMSG_SEND_WINDOW_CLOSE pMsg;
			pMsg.h.set((LPBYTE)&pMsg, 0xF8, 0x4D, sizeof(_tagPMSG_SEND_WINDOW_CLOSE));

			for (int n = OBJ_STARTUSERINDEX; n < OBJMAX; n++)
			{
				if (gObj[n].Connected == PLAYER_PLAYING && gObj[n].Type == OBJ_USER)
				{
					DataSend(gObj[n].m_Index, (LPBYTE)&pMsg, pMsg.h.size);
				}
			}

			LogAddTD("[AcheronGuardianEvent] Send Close Window Sec[%d]", this->m_iREMAIN_MSEC / 1000);
		}

		if (this->m_iREMAIN_MSEC <= 0)
		{
			for (int aIndex = OBJ_STARTUSERINDEX; aIndex < OBJMAX; aIndex++)
			{
				if (gObj[aIndex].Connected == PLAYER_PLAYING && gObj[aIndex].Type == OBJ_USER)
				{
					gObjMoveGate(aIndex, 27);
				}
			}

			this->Clear();
			this->SetProcState(1);
		}
	}
}

void CAcheronGuardianEvent::CGReqAcheronEventEnter(_tagPMSG_REQ_ACHERON_EVENT_ENTER* lpMsg, int iIndex)
{
	if (!OBJMAX_RANGE(iIndex))
	{
		return;
	}
	
	LPOBJ lpObj = &gObj[iIndex];

	if (!gObjIsConnected(iIndex))
	{
		return;
	}

	if (lpObj->Teleport)
    {
		return;
	}

	//missing m_ActState checks
	BYTE attr = MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y);
	
	if (attr & 1)
    {
		if (lpObj->m_bIsPartyMatch)
		{
			_PMSG_ANS_NOTI_ENTER_EVENTMAP_ERROR stEnterResult;
			stEnterResult.h.set((LPBYTE)&stEnterResult, 0xEF, 0x09, sizeof(_PMSG_ANS_NOTI_ENTER_EVENTMAP_ERROR));
			stEnterResult.nResult = -1;
			DataSend(iIndex, (LPBYTE)&stEnterResult, stEnterResult.h.size);
			return;
		}

		_tagPMSG_ANS_ACHERON_EVENT_ENTER pMsg;
		pMsg.h.set((LPBYTE)&pMsg, 0xF8, 0x4C, sizeof(_tagPMSG_ANS_ACHERON_EVENT_ENTER));
		pMsg.btResult = 1;

		if (this->IsPlayStart())
		{
			LogAddTD("[AcheronGuardianEvent] [%s][%s] Acheron Event Enter",
				lpObj->AccountID, lpObj->Name);
			gObjMoveGate(iIndex, 426);
			return;
		}

		DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
	}
}

void CAcheronGuardianEvent::GDReqAcheronEventProcMultiCast(BYTE btPlay)
{
	_tagPMSG_REQ_AE_PLAY_DS pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xF8, 0xF0, sizeof(_tagPMSG_REQ_AE_PLAY_DS));
	pMsg.btPlay = btPlay;
	pMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}

void CAcheronGuardianEvent::DGAnsAcheronEventProcMultiCast(_tagPMSG_ANS_AE_PLAY_DS* lpMsg)
{
	if (!g_ArcaBattle.IsArcaBattleServer())
	{
		this->SetPlayStart(lpMsg->btPlay);
	}

	LogAddTD("[AcheronGuardianEvent] Acheron Event Play [%d]", lpMsg->btPlay);
}

void CAcheronGuardianEvent::SetRandomObeliskAttr()
{
	int iArrObeliskAttr[] = { 0, 0, 0, 0, 0 };
	int iArrMonsterIndex[] = { 627, 628, 629, 630, 631 };
	
	for(int iObeliskAttrCnt = 0; iObeliskAttrCnt < AE_MAX_BATTLEPOS; iObeliskAttrCnt++)
	{
		int iRand = rand() % AE_MAX_BATTLEPOS + 1;
		bool bflag = false;

		for (int i = 0; i < AE_MAX_BATTLEPOS; i++)
		{
			if (iArrObeliskAttr[i] == iRand)
			{
				bflag = true;
			}
		}

		if (!bflag)
		{
			iArrObeliskAttr[iObeliskAttrCnt] = iRand;
		}
	}

	for (int j = 0; j < AE_MAX_BATTLEPOS; j++)
	{
		this->m_stObeliskInfo[j].m_iAttrKind = iArrObeliskAttr[j];
		this->m_stObeliskInfo[j].m_iMonsterNum = iArrMonsterIndex[iArrObeliskAttr[j]];
	}
}

void CAcheronGuardianEvent::GenObelisk(int iGroupNum)
{
	int result = gObjAddMonster(this->m_stObeliskInfo[iGroupNum].m_iMapIndex);

	if (result > 0)
	{
		gObjSetMonster(result, this->m_stObeliskInfo[iGroupNum].m_iMonsterNum);
		this->m_stObeliskInfo[iGroupNum].m_iMonIndex = result;
		gObj[result].Class = this->m_stObeliskInfo[iGroupNum].m_iMonsterNum;
		gObj[result].MapNumber = this->m_stObeliskInfo[iGroupNum].m_iMapIndex;
		gObj[result].X = this->m_stObeliskInfo[iGroupNum].m_iPosX;
		gObj[result].Y = this->m_stObeliskInfo[iGroupNum].m_iPosY;
		gObj[result].TX = this->m_stObeliskInfo[iGroupNum].m_iPosX;
		gObj[result].TY = this->m_stObeliskInfo[iGroupNum].m_iPosY;
		gObj[result].m_OldX = this->m_stObeliskInfo[iGroupNum].m_iPosX;
		gObj[result].m_OldY = this->m_stObeliskInfo[iGroupNum].m_iPosY;
		gObj[result].StartX = this->m_stObeliskInfo[iGroupNum].m_iPosX;
		gObj[result].StartY = this->m_stObeliskInfo[iGroupNum].m_iPosY;
		gObj[result].Dir = 1;
		gObj[result].Live = 1;
		gObj[result].DieRegen = 0;
		gObj[result].m_State = 1;
		gObj[result].MaxRegenTime = 0;
		LogAddTD("[AcheronGuardianEvent] (%d) Obelisk be created.", result);
	}
}

void CAcheronGuardianEvent::DelObelisk()
{
	for (int iCnt = 0; iCnt < AE_MAX_BATTLEPOS; iCnt++)
	{
		int iIndex = this->m_stObeliskInfo[iCnt].m_iMonIndex;

		if (iIndex >= 0)
		{
			LogAdd("[AcheronGuardianEvent][Obelisk] Delete Monster - %d ", iIndex);
			//TRACE_LOG("[AcheronGuardianEvent][Obelisk] Delete Monster - %d ", iIndex);
			gObjDel(iIndex);
		}
	}
}

void CAcheronGuardianEvent::DestoryObelisk(LPOBJ lpObj, BYTE btMapNumber, BYTE cX, BYTE cY, int nMaxHitUser)
{
	int iItemCnt = this->GetDropItemCnt();

	if (iItemCnt <= 0 || iItemCnt > AE_REWARD_MAX_ITEMCOUNT)
	{
		LogAddTD("[AcheronGuardianEvent][Error] Rand Drop Item Over - [%s][%s] ItemCnt[%d]",
			lpObj->AccountID, lpObj->Name, iItemCnt);
		iItemCnt = 1;
	}

	LogAddTD("[AcheronGuardianEvent] Destoryed an Obelisk. [%s][%s] Item Drop Cnt [%d]",
		lpObj->AccountID, lpObj->Name, iItemCnt);
	//LogAddEvent("AcheronGuardianEvent:Destoryed an Obelisk", lpObj, ":ItemCnt[%d]", iItemCnt);

	for (int i = 0; i < iItemCnt; i++)
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::Acheron, nMaxHitUser, btMapNumber, cX, cY);
		//this->m_pAcheronEventRewardItem->DropEventItemEx()
	}
	
	char szTempMsg[256];
	sprintf(szTempMsg, lMsg.Get(MSGGET(13, 204)), lpObj->Name);
	this->SendMapServerGroupMsg(szTempMsg);
	
	char szTemp[256];
	sprintf(szTemp, lMsg.Get(MSGGET(13, 184)));
	this->SendMapServerGroupMsg(szTemp);

	this->SetProcState(6);
}

void CAcheronGuardianEvent::RegenMonsterRun()
{
	DWORD dwTick = GetTickCount();

	if (this->m_stMonsterGroupInfo[this->m_iGroupNum].m_nGroupNumber > -1)
	{
		if (dwTick >= this->m_stMonsterGroupInfo[this->m_iGroupNum].m_dwNextRegenTime)
		{
			this->DeleteMonster(this->m_iGroupNum);
			this->RegenMonster(this->m_iGroupNum, 2, 0);
			this->m_stMonsterGroupInfo[this->m_iGroupNum].m_dwNextRegenTime = GetTickCount() + 60000 * this->m_stMonsterGroupInfo[this->m_iGroupNum].m_nRegenTimeMin;
		}
	}
}

void CAcheronGuardianEvent::RegenMonster(int nGroupNumber, int iGenSeq, int iGenMonCnt)
{
	if (nGroupNumber < 0 || nGroupNumber > AE_MAX_MONGROUP - 1)
	{
		//LogAddTD(89EBB1);
		return;
	}

	int nMapNumber = this->m_stMonsterGroupInfo[nGroupNumber].m_nMapNumber;
    int nMonsterArrayIndex = this->m_stMonsterGroupInfo[nGroupNumber].m_nGenTotCnt;
    int nMonsterCount = this->m_stMonsterGroupInfo[nGroupNumber].m_nCnt;
    int nMonsterKindCnt = this->m_stMonsterPosition[nGroupNumber].m_nMonsterKindCnt;

    if (iGenMonCnt)
	{
		nMonsterCount = iGenMonCnt;
	}

	for (int n = 0; n < nMonsterCount; n++)
    {
		int nRegenArea = rand() % this->m_stMonsterPosition[nGroupNumber].m_nRegenAreaCnt;
		int nBeginX = this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nRegenArea].m_nBeginX;
		int nBeginY = this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nRegenArea].m_nBeginY;
		int nEndX = this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nRegenArea].m_nEndX;
		int nEndY = this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nRegenArea].m_nEndY;
		BYTE Element = (BYTE)-1;

		if (nMapNumber == -1)
		{
			break;
		}
        
		int nResult = gObjAddMonster(nMapNumber);
		int nMonsterIndex = 0;

		for (int i = 0; i < nMonsterKindCnt; i++)
		{
			if (this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[i].m_nGenSqe == iGenSeq
				&& this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[i].m_nMonAttr == this->m_stObeliskInfo[this->m_iGroupNum].m_iAttrKind)
			{
				nMonsterIndex = this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[i].m_nMonsterIndex;
				Element = this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[i].m_nMonAttr;
				break;
			}
		}

		this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[nMonsterArrayIndex].m_nIndex = nResult;

		if (nResult < 0)
		{
			LogAddTD("[AcheronGuardianEvent][MonsterGroupRegen] AddMonster fail!! [GROUP]:%d, [INDEX]:%d",
				nGroupNumber, nMonsterIndex);
			continue;
		}

		if (!this->SetPosMonster(nResult, nMapNumber, nBeginX, nBeginY, nEndX, nEndY, Element))
		{
			gObjDel(nResult);
			LogAdd("[AcheronGuardianEvent] error : %s %d", __FILE__, __LINE__);
			continue;
		}
	
		if (!gObjSetMonster(nResult, nMonsterIndex))
		{
			gObjDel(nResult);
            LogAdd("[AcheronGuardianEvent] error : %s %d", __FILE__, __LINE__);
			continue;
		}

		nMonsterArrayIndex++;
        LogAddTD("[AcheronGuardianEvent][MonsterGroupRegen] Group: %d, MonsterIndex: %d, MapNumber: %d, X: %d, Y: %d ",
			nGroupNumber, nMonsterIndex, nMapNumber, gObj[nResult].X, gObj[nResult].Y);
	}

	this->m_stMonsterGroupInfo[nGroupNumber].m_nGenTotCnt = nMonsterArrayIndex;
    LogAddTD("[AcheronGuardianEvent][MonsterGroupRegen] Group: %d, MonsterTotCnt: %d ",
		nGroupNumber, this->m_stMonsterGroupInfo[nGroupNumber].m_nGenTotCnt);
}

int CAcheronGuardianEvent::SetPosMonster(int aIndex, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY, BYTE Element)
{
	if (!OBJMAX_RANGE(aIndex))
	{
		LogAdd("[AcheronGuardianEvent] error : %s %d", __FILE__, __LINE__);
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

    lpObj->m_PosNum = -1;
    lpObj->MapNumber = nMapNumber;

    this->GetBoxPosition(nMapNumber, nBeginX, nBeginY, nEndX, nEndY, lpObj->X, lpObj->Y);

    lpObj->TX = lpObj->X;
    lpObj->TY = lpObj->Y;
    lpObj->m_OldX = lpObj->X;
    lpObj->m_OldY = lpObj->Y;
    lpObj->Dir = rand() % 8;
    lpObj->StartX = lpObj->X;
    lpObj->StartY = lpObj->Y;
    lpObj->DieRegen = 0;
    lpObj->MaxRegenTime = 0;

	if (Element != (BYTE)-1)
	{
		lpObj->m_btAttribute = Element;
	}

	return 1;
}

int CAcheronGuardianEvent::GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, BYTE& mx, BYTE& my/*short& mx, short& my*/)
{
	int count = 99 ; 
	int w;
	int h;
	int tx; 
	int ty;
	BYTE attr;

	while (count-- != 0)
	{
		w = aw - ax;
		h = ah - ay;

		__try
		{
			tx = ax + (rand() % w);
			ty = ay + (rand() % h);
		}
		__except (w = 1, h = 1, 1)
		{

		}

		attr = MapC[mapnumber].GetAttr(tx, ty);

		//if (attr) 
		//{
			mx = tx;
			my = ty;
		//}
		return 1;
	}

	return 0;
}

void CAcheronGuardianEvent::ResetGenTime()
{
	for (int nGroupNumber = 0; nGroupNumber < AE_REWARD_MAX_ITEMCOUNT; nGroupNumber++)
	{
		if (this->m_stMonsterGroupInfo[nGroupNumber].m_nGroupNumber >= 0)
		{
			if (nGroupNumber < 0 || nGroupNumber > AE_MAX_BATTLEPOS - 1)
			{
				return;
			}
			
			this->m_stMonsterGroupInfo[nGroupNumber].m_dwNextRegenTime = GetTickCount() + 60000 * this->m_stMonsterGroupInfo[nGroupNumber].m_nRegenTimeMin;
		}
	}
}

void CAcheronGuardianEvent::DeleteMonster(int nGroupNumber)
{
	if (nGroupNumber < 0 || nGroupNumber > AE_MAX_BATTLEPOS - 1)
	{
		return;
	}

    for (int i = 0; i < AE_MAX_MONGROUP; i++)
    {
		if (this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex > -1)
		{
			int nIndex = this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex;

			if (!gObj[nIndex].Live)
			{
				gObjDel(nIndex);
				LogAdd("[AcheronGuardianEvent][MonsterGroupRegen] Delete Monster - %d ", nIndex);
				this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex = -1;
			}
		}
	}
}

void CAcheronGuardianEvent::DeleteArcaBattleAllMonster()
{
	for (int nGroupNumber = 0; nGroupNumber < AE_MAX_BATTLEPOS; nGroupNumber++)
	{
		if (this->m_stMonsterGroupInfo[nGroupNumber].m_nGroupNumber >= 0)
		{
			/*if (nGroupNumber < 0 || nGroupNumber > AE_MAX_BATTLEPOS - 1)
			{
				return;
			}*/
			for (int i = 0; i < AE_MAX_MONGROUP; i++)
			{
				if (this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex > -1)
				{
					int nIndex = this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex;

					gObjDel(nIndex);
					LogAdd("[AcheronGuardianEvent][MonsterGroupRegen] Delete Monster - %d ", nIndex);
					this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex = -1;
				}
			}

			this->m_stMonsterGroupInfo[nGroupNumber].m_dwNextRegenTime = 0;
		}
	}
}

void CAcheronGuardianEvent::ProbabilityLoadData()
{
	//load itembag
}

void CAcheronGuardianEvent::SendMapServerGroupMsg(LPSTR lpszMsg)
{
	GS_GDReqMapSvrMsgMultiCast(g_MapServerManager.GetMapSvrGroup(), lpszMsg);
}