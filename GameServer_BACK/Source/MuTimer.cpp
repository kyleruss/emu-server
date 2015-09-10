// MuTimer.cpp: implementation of the CMuTimer class.
// 1.01.00
// working on it
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <Mmsystem.h>
#include "queuetimer.h"
#include "mutimer.h"
#include "EDSProtocol.h"
#include "Kanturu.h"
#include "BloodCastle.h"
#include "GameServer.h"
#include "ChaosCastle.h"
#include "Weapon.h"
#include "CashShop.h"
#include "IllusionTempleEvent.h"
#include "Raklion.h"
#include "DevilSquare.h"
#include "TMonsterAI.h"
#include "GameMain.h"
#include "EledoradoEvent.h"
#include "EventManagement.h"
#include "logproc.h"
#include "CastleSiegeSync.h"
#include "ImperialGuardian.h"
#include "CastleSiege.h"
#include "Crywolf.h"
#include "CastleDeepEvent.h"
#include "DoubleGoerEvent.h"
#include "GameEvent.h"
#include "DBSockMng.h"
#include "TemporaryUserManager.h"
#include "ConMember.h"
#if (ENABLETEST_NEWPVP == 1)
#include "NewPVP.h"
#endif

#if (ENABLE_CUSTOM_MONSTERSPAWNER == 1)
#include "MonsterSpawner.h"
#endif

VOID WINAPI	TimerProcQueue(LPVOID lpParametar, BOOL TimerOrWaitFired);
VOID WINAPI	TimerProcQueue2(LPVOID lpParametar, BOOL TimerOrWaitFired);

CMuTimer g_MuTimer;

#if (ENABLETEST_ACHERON == 1)
#include "AcheronEvent.h"
#endif
// Constructor / Destructor

CMuTimer::CMuTimer()
{
	timeBeginPeriod(1);
	this->m_bTimerEnd = 0;
	this->m_QueueTimer1.RegCallBack((WAITORTIMERCALLBACK)TimerProcQueue);
	this->m_QueueTimer2.RegCallBack((WAITORTIMERCALLBACK)TimerProcQueue2);
}

CMuTimer::~CMuTimer()
{
	timeEndPeriod(1);
}

void CMuTimer::SetMuTimer()
{
VM_START
	INT		result;

	DWORD	MuTimer1Flag = 0x20;
	DWORD	MuTimer2Flag = 0x01;

	MuTimer1Flag = 0x00010000|MuTimer1Flag;
	MuTimer2Flag = 0x00010000|MuTimer2Flag;

	result = this->m_QueueTimer1.CreateTimer(WM_SET_DATE,60000,MuTimer1Flag);
	result = this->m_QueueTimer1.CreateTimer(WM_LOG_DATE_CHANGE,60000,MuTimer1Flag);
	
	//result = this->m_QueueTimer1.CreateTimer(0x3EF,10000,MuTimer1Flag);
	result = this->m_QueueTimer1.CreateTimer(WM_CONNECT_DATASERVER,10000,MuTimer1Flag);
	
	result = this->m_QueueTimer1.CreateTimer(WM_GS_CLOSE,1000,MuTimer1Flag);
	result = this->m_QueueTimer1.CreateTimer(WM_MONSTER_AND_MSG_PROC,500,MuTimer1Flag);
	result = this->m_QueueTimer1.CreateTimer(WM_MOVE_MONSTER_PROC,300,MuTimer1Flag);
	result = this->m_QueueTimer1.CreateTimer(WM_EVENT_RUN_PROC,100,MuTimer1Flag);
	result = this->m_QueueTimer1.CreateTimer(WM_AI_MONSTER_MOVE_PROC,100,MuTimer1Flag);
	result = this->m_QueueTimer1.CreateTimer(WM_AI_MONSTER_PROC,500,MuTimer1Flag);
	result = this->m_QueueTimer1.CreateTimer(WM_GUILD_LIST_REQUEST,0,MuTimer1Flag | 8);
	result = this->m_QueueTimer1.CreateTimer(WM_SECOND_MSG_PROCESS,1000,MuTimer1Flag);

	result = this->m_QueueTimer1.CreateTimer(WM_DUEL_RUN_PROC,1000,MuTimer1Flag);

	if( g_ConnectMemberReload == true && g_ConnectMemberReloadTime > 0 )
	{
		result = this->m_QueueTimer1.CreateTimer(WM_CONNECTMEMBER_RELOAD, 1000 * 60 * g_ConnectMemberReloadTime, MuTimer1Flag);
	}

	//THE PROBLEM IS IN THESE TIMERS
	result = this->m_QueueTimer2.CreateTimer(WM_FIRST_MSG_PROCESS, 1000, MuTimer2Flag);
	result = this->m_QueueTimer2.CreateTimer(WM_VIEWPORT_UPDATE, 1000, MuTimer2Flag);
VM_END
}

VOID WINAPI	TimerProcQueue(LPVOID lpParametar, BOOL TimerOrWaitFired)
{
	if( g_MuTimer.GetTimerEnd() == TRUE )
		return;

	g_MuTimer.m_QueueTimer1.SyncLock();

	int nEventID = *(int*)(lpParametar);
	
	switch(nEventID)
	{
	case WM_GUILD_LIST_REQUEST:	DGGuildListRequest();	break;
	case WM_MONSTER_AND_MSG_PROC:
		MonsterAndMsgProc();
		//gDuelSystem.Run();
#if (ENABLETEST_NEWPVP == 1)
		g_NewPVP.Run();
#endif
		break;
	case WM_DUEL_RUN_PROC:
		//g_DuelManager.Run();
		break;
	case WM_MOVE_MONSTER_PROC:
		MoveMonsterProc();
		g_Kanturu.UserMonsterCountCheck();
		break;
	case WM_EVENT_RUN_PROC:
#if (ENABLE_CUSTOM_MONSTERSPAWNER == 1)
		MonsterSpawnerMng::getInstance()->procRun();
#endif
		g_BloodCastle.Run();
		g_RingAttackEvent.Run();
		g_ChaosCastle.Run();
		g_IllusionTemple.Run();
#if (GS_CASTLE==1)
		g_CastleSiege.Run();
		g_CastleDeepEvent.Run();
		g_CsNPC_Weapon.WeaponAttackProc();
		g_Crywolf.Run();
#endif
#if (GS_CASTLE == 0 || GS_SPEC == 1)
		g_Kanturu.Run();
		g_CsNPC_Weapon.WeaponAttackProc();
#endif
		g_Raklion.Run();
		g_XMasAttackEvent.Run();
		g_ImperialGuardian.Run();
		gDoubleGoer.Run();

#if (ENABLETEST_ACHERON == 1)
		g_AcheronGuardianEvent.Run();
#endif

#ifdef VMPROTECT
		if( VMProtectIsDebuggerPresent(true) )
		{
			ExitProcess(0);
		}
#endif
		break;
	case WM_AI_MONSTER_MOVE_PROC: TMonsterAI::MonsterMoveProc();	break;
	case WM_AI_MONSTER_PROC: TMonsterAI::MonsterAIProc();	break;
	case WM_SECOND_MSG_PROCESS:
		{
			for(int n = 0; n < MAX_NUMBER_MAP; n++ )
			{
				MapC[n].WeatherVariationProcess();
			}

			wsGServer.m_SendSec = wsGServer.m_SendSecTmp;
			wsGServer.m_RecvSec = wsGServer.m_RecvSecTmp;
			wsGServer.m_SendSecTmp = 0;
			wsGServer.m_RecvSecTmp = 0;
			DragonEvent->Run();
			AttackEvent->Run();
			gEledoradoEvent.Run();
			g_EventManager.Run();
#if (CUSTOM_OLYMP==1)
			g_CustomOlymp.Run();
#endif
		}
		break;
	case WM_CONNECTMEMBER_RELOAD:
		{
			ConMember.Load(gDirPath.GetNewPath(FILE_WHITELIST));
		}
		break;
	case WM_GS_CLOSE:
		if ( gCloseMsg != 0 )
		{
			gCloseMsgTime--;

			if ( gCloseMsgTime <= 1 )
			{
				if ( gCloseMsgTime == 1 )
				{
					AllSendServerMsg(lMsg.Get(MSGGET(1, 193)));
					LogAddC(2, lMsg.Get(MSGGET(1, 193)));
				}
			}
			else
			{
				if ( (gCloseMsgTime % 10) == 0 )
				{
					char szTemp[256];

					wsprintf( szTemp, lMsg.Get(MSGGET(1, 194)), gCloseMsgTime);
					AllSendServerMsg(szTemp);
					LogAddC(2, szTemp);
				}
			}

			if ( gCloseMsgTime < 0 )
			{
				KillTimer(ghWnd, WM_GS_CLOSE);
				gObjAllLogOut();
				gCloseMsgTime = 0;
				gCloseMsg = 0;
			}
		}
		gObjSecondProc();
#if (GS_CASTLE == 1)
		if(cDBSMng.GetActiveDS() > -1)
		{
			if(g_CastleSiege.GetDataLoadState() == 2)
			{
				g_CastleSiege.DataRequest();
			}

#if (CUSTOM_OLYMP==1)
			if(g_CustomOlymp.GetDataLoadState() == 2 )
			{
				g_CustomOlymp.DataRequest();
			}
			
#endif
		}
					
		g_CastleSiege.SendCastleStateSync();
		g_CastleSiegeSync.AdjustTributeMoney();
		g_Crywolf.CrywolfSecondAct();
#endif
#if (GS_CASTLE == 0 || GS_SPEC == 1)
		//g_CastleSiegeSync.AdjustTributeMoney();
#if(__GENS_SYSTEM__ == 1)
		gGensSystem.UpdateRun();
#endif
#endif
		//g_PeriodItemEx.CheckPeriodData(); --  need to decompile
		//g_CashShopInGame.CheckMileageUpdate(); -- need to decompile
		//g_CashShopInGame.CheckShopServerReconnect(); -- need to decompile
		//g_HacktoolBlockEx.CheckHacktoolUse(); -- need to decompile

		/*if ( DBSockMng::GetActiveDS(&cDBSMng) > -1 && !CArcaBattle::IsDataLoadWinGuild(&g_ArcaBattle) )
          CArcaBattle::GDReqWinGuildInfo(&g_ArcaBattle);
        if ( DBSockMng::GetActiveDS(&cDBSMng) > -1 && !CArcaBattle::IsDataLoadProcState(&g_ArcaBattle) )
          CArcaBattle::GDReqProcState(&g_ArcaBattle);
        if ( DBSockMng::GetActiveDS(&cDBSMng) > -1
          && !CArcaBattle::IsInitABProcMultiCast(&g_ArcaBattle)
          && CArcaBattle::IsArcaBattleServer(&g_ArcaBattle) == 1 )
          CArcaBattle::SetArcaBattleProcMultiCast(&g_ArcaBattle, 1);
        if ( DBSockMng::GetActiveDS(&cDBSMng) > -1
          && !CAcheronGuardianEvent::IsInitAEProcMultiCast(&g_AcheronGuardianEvent)
          && CArcaBattle::IsArcaBattleServer(&g_ArcaBattle) == 1 )
          CAcheronGuardianEvent::SetAcheronEventProcMultiCast(&g_AcheronGuardianEvent, 1);*/
		break;
	case WM_SET_DATE:
		gSetDate();
		WhisperCash.TimeCheckCashDelete();
		break;
	case WM_LOG_DATE_CHANGE:

		cpTimerTempUserManager->ClearOverTimeUserData(g_MaintainUserConnectionSecond);
		/*
		if( g_GensAbusingResetTime > 0 )
		{
			for(int n = OBJ_STARTUSERINDEX; n < OBJMAX; n++)
			{
				if( gObjIsConnected(n) )
					g_GensSystem.AbusingInfoReset(&gObj[n]);
			}
		}
		*/

#if (GS_CASTLE == 1)
		if(LogDateChange() == TRUE)
			g_iCastleItemMixLimit = 1;
#else
		LogDateChange()
#endif
		break;

	default:
		break;
	}

	g_MuTimer.m_QueueTimer1.SyncUnlock();
}

BOOL CMuTimer::GetTimerEnd()
{
	return this->m_bTimerEnd;
}

VOID WINAPI	TimerProcQueue2(LPVOID lpParametar, BOOL TimerOrWaitFired)
{
	if (g_MuTimer.GetTimerEnd() == TRUE)
	{
		return;
	}
	
	g_MuTimer.m_QueueTimer2.SyncLock();
		
	int nEventID = *(DWORD*)lpParametar;
	
	switch (nEventID)
	{
		case WM_FIRST_MSG_PROCESS:
		{
			int n;

			if (gDoPShopOpen != FALSE)
			{
				for (n=OBJ_STARTUSERINDEX; n<OBJMAX; n++)
				{
					PShop_ViewportListRegenarate(n);
				}
				
				for (n=OBJ_STARTUSERINDEX; n<OBJMAX; n++)
				{
					if (gObjIsConnected(n) != PLAYER_EMPTY)
					{
						gObj[n].m_bPShopItemChange = false;
					}
				}
			}
			
			for (n=OBJ_STARTUSERINDEX; n<OBJMAX; n++)
			{
				gObjUnionUpdateProc(n);
			}
			
			gObjSetState();

			GameServerInfoSend();

		} break;
		
		case WM_VIEWPORT_UPDATE:
		{
			for (int n = 0; n < OBJMAX; n++)
			{
				gObjStateSetCreate(n);
				gObjViewportListDestroy(n);
				gObjViewportListCreate(n);
				gObjViewportListProtocol(n);
			}
		} 
		break;
	}
		
	g_MuTimer.m_QueueTimer2.SyncUnlock();
}

void CMuTimer::SetTimerEnd()
{
	this->m_bTimerEnd = TRUE;
}