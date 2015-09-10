// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
// GS-N 0.99.60T 0x00453780
// GS-N	1.00.18	JPN	0x00462B60	-	Completed
#include "stdafx.h"
#include "DevilSquare.h"
#include "GameServer.h"
#include "LogProc.h"
#include "GameMain.h"
#include "TNotice.h"
#include "..\include\readscript.h"
#include "gObjMonster.h"
#include "BloodCastle.h"
#include "ChaosCastle.h"
#include "..\common\winutil.h"
#include "CrywolfSync.h"
#include "MasterSkillSystem.h"
#include "BuffEffectSlot.h"
#include "GameEvent.h"
#include "Event.h"
#include "MapRateInfo.h"


CDevilSquare g_DevilSquare;



unsigned int __stdcall DevilSquareThreadFunc(void * p)
{
	CDevilSquare * pDevilSquare = (CDevilSquare *)p;
	return pDevilSquare->Run();
}

void DevilSquareProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen)
{
	switch ( protoNum )
	{
	case 0x06:
		EGAnsDevilSquareEnter((PMSG_ANS_BLOODCASTLE_ENTER*)aRecv);
		break;
	case 0x08:
		EGAnsBloodCastleEnter((PMSG_ANS_BLOODCASTLE_ENTER*)aRecv);
		break;
	case 0x0B:
		EGAnsBloodCastleEnterCount((PMSG_ANS_BLOODCASTLE_ENTERCOUNT*)aRecv);
		break;
	default:
		break;
	}
}




void DataSendRank(char* pMsg, int size)
{
	if ( IsDevilSquareEventConnected == FALSE )
	{
		wsRServerCli.Close();
		wsRServerCli.CreateSocket(ghWnd);

		if ( GMRankingServerConnect(RankingServerIP, WM_GM_RANKING_CLIENT_MSG_PROC) == FALSE )
		{
			IsDevilSquareEventConnected = FALSE;
//			LogAddTD("Can not connect Ranking Server");

			return;
		}

		IsDevilSquareEventConnected = TRUE;
	}

	if ( IsDevilSquareEventConnected != FALSE )
	{
		wsRServerCli.DataSend(pMsg, size);
	}
}


CDevilSquare::CDevilSquare()
{
	this->m_bQuit = FALSE;
	this->m_iCloseTime = 2;
	this->m_iOpenTime = 2;
	this->m_iPlaytime = 10;
	this->m_BonusScoreTable[0][0] = 0;
	this->m_BonusScoreTable[0][1] = 0;
	this->m_BonusScoreTable[0][2] = 0;
	this->m_BonusScoreTable[0][3] = 170;
	
	this->m_BonusScoreTable[1][0] = 20;
	this->m_BonusScoreTable[1][1] = 90;
	this->m_BonusScoreTable[1][2] = 120;
	this->m_BonusScoreTable[1][3] = 400;

	this->m_BonusScoreTable[2][0] = 10;
	this->m_BonusScoreTable[2][1] = 10;
	this->m_BonusScoreTable[2][2] = 10;
	this->m_BonusScoreTable[2][3] = 200;

	this->m_BonusScoreTable[3][0] = 0;
	this->m_BonusScoreTable[3][1] = 0;
	this->m_BonusScoreTable[3][2] = 0;
	this->m_BonusScoreTable[3][3] = 0;

	this->m_BonusScoreTable[0][4] = 170;
	this->m_BonusScoreTable[0][5] = 170;

	this->m_BonusScoreTable[1][4] = 400;
	this->m_BonusScoreTable[1][5] = 400;

	this->m_BonusScoreTable[2][4] = 200;
	this->m_BonusScoreTable[2][5] = 200;

	this->m_BonusScoreTable[3][4] = 0;
	this->m_BonusScoreTable[3][5] = 0;

	this->m_BonusScoreTable[4][0] = 0;
	this->m_BonusScoreTable[4][1] = 0;
	this->m_BonusScoreTable[4][2] = 0;
	this->m_BonusScoreTable[4][3] = 0;
	this->m_BonusScoreTable[4][4] = 0;
	this->m_BonusScoreTable[4][5] = 0;

	this->m_BonusScoreTable[0][6] = 170;
	this->m_BonusScoreTable[1][6] = 400;
	this->m_BonusScoreTable[2][6] = 200;
	this->m_BonusScoreTable[3][6] = 0;
	this->m_BonusScoreTable[4][6] = 0;

	for(int i = 0; i < MAX_DEVILSQUARE_GROUND; i++)
	{
		this->m_BonusIncValue[i] = 1.0f;
	}
}




CDevilSquare::~CDevilSquare()
{
	this->m_bQuit = TRUE;
}




void CDevilSquare::Init()
{
	this->m_bQuit = TRUE;
	Sleep(500);

	this->SetState(DevilSquare_CLOSE);
	this->m_bQuit = FALSE;

	DWORD dwThreadId;

//#ifdef _DEBUG
	this->m_hThread = (UINT)_beginthreadex( 0, 0, DevilSquareThreadFunc, this, 0, (LPUINT)&dwThreadId);
//#else
//	this->m_hThread = (UINT)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DevilSquareThreadFunc, this, 0, (LPDWORD)&dwThreadId);
//#endif
}



void CDevilSquare::Load(char * filename)
{
	int i;
	for ( i=0;i<MAX_DEVILSQUARE_GROUND;i++)
	{
		this->m_DevilSquareGround[i].Init(i);
	}

	int Token;
	int number;
	int type;
	int monstertype;
	int starttime;
	int endtime;
	int x;
	int y;
	int tx;
	int ty;

	SMDFile = fopen(filename, "r");

	if ( SMDFile == NULL )
	{
		MsgBox("[DevilSquare] Info file Load Fail [%s]", filename);
		return;
	}

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
			if ( type == 0 )	// Time Sync
			{
				Token = GetToken();

				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				this->m_iCloseTime = (int)TokenNumber;

				Token = GetToken();
				this->m_iOpenTime = (int)TokenNumber;

				Token = GetToken();
				this->m_iPlaytime = (int)TokenNumber;
			}
			else if ( type == 1 )	// Normal Monster Settings
			{
				Token = GetToken();

				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				number = (int)TokenNumber;	// Devils Square Index;

				Token = GetToken();
				monstertype = (int)TokenNumber;

				Token = GetToken();
				starttime = (int)TokenNumber;

				Token = GetToken();
				endtime = (int)TokenNumber;

				this->m_DevilSquareGround[number].Set(monstertype, starttime, endtime);
			}
			else if ( type == 2 )	// Boss Monster Set
			{
				Token = GetToken();

				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				number = (int)TokenNumber;	// Devils Square Index;

				Token = GetToken();
				monstertype = (int)TokenNumber;

				Token = GetToken();
				starttime = (int)TokenNumber;

				Token = GetToken();
				x = (int)TokenNumber;

				Token = GetToken();
				y = (int)TokenNumber;

				Token = GetToken();
				tx = (int)TokenNumber;

				Token = GetToken();
				ty = (int)TokenNumber;

				this->m_DevilSquareGround[number].SetBoss(monstertype, starttime, x, y, tx, ty);
			}
			else if ( type == 3 ) // Bonus Sttings
			{
				Token = GetToken();

				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				int index;
				int rank;
				int zen;
				int exp;

				index = (int)TokenNumber;

				Token = GetToken();
				rank = (int)TokenNumber;

				Token = GetToken();
				exp = (int)TokenNumber;

				Token = GetToken();
				zen = (int)TokenNumber;

				this->m_DevilSquareGround[index].SetBonus(rank, exp, zen);
			}
			else if ( type == 4 )
			{
				DS_TIME tmp;

				Token = GetToken();

				if ( strcmp("end", TokenString) == 0 )
				{
					break;
				}

				tmp.iHour = (int)TokenNumber;
				Token = GetToken();
				tmp.iMinute = (int)TokenNumber;

				m_EventTime.push_back(tmp);
			}
			else if ( type == 5 )
			{
				Token = GetToken();

				if ( strcmp("end", TokenString) == 0 )
				{
					for( i = 0; i < MAX_DEVILSQUARE_GROUND; i++)
					{
						this->m_DevilSquareGround[i].IncBonus(m_BonusIncValue[i]);
					}
					break;
				}

				int index = (int)TokenNumber;

				if( DS_LEVEL_RANGE(index) )
				{
					Token = GetToken();
					m_BonusIncValue[index] = TokenNumber;
				}
				else
				{
					Token = GetToken();
				}
			}
		}
	}

	fclose (SMDFile );
	LogAdd("%s file load!", filename);
}







void CDevilSquare::SetState(enum eDevilSquareState eState)
{
	this->m_eState = eState;

	switch ( this->m_eState )
	{
		case DevilSquare_CLOSE:	this->SetClose();	break;
		case DevilSquare_OPEN:		this->SetOpen();	break;
		case DevilSquare_PLAYING:	this->SetPlaying();	break;
	}
}




void CDevilSquare::SetClose()
{
	if ( LogAdd != NULL )	// LoL
	{
		LogAddTD("[DevilSquare] Close");
	}

	this->ClearMonstr();
	this->CalcScore();

	for ( int i=0;i<MAX_DEVILSQUARE_GROUND;i++)
	{
		this->m_DevilSquareGround[i].Clear();
	}

	this->m_iRemainTime = this->m_iCloseTime;
	this->CheckSync();
	this->m_bFlag = 0;

	if ( this->m_iRemainTime <= 1 )
	{
		this->m_iTime = GetTickCount();
		this->m_iremainTimeSec = 60;
	}
	else
	{
		this->m_iTime = GetTickCount() + 60000;
		this->m_iremainTimeSec = -1;
	}
	
	this->m_bSendTimeCount = FALSE;
}





void CDevilSquare::SetOpen()
{
	this->m_iRemainTime = this->m_iOpenTime;

	if ( this->m_iOpenTime <= 1 )
	{
		this->m_iTime = GetTickCount();
		this->m_iremainTimeSec = 60;
	}
	else
	{
		this->m_iTime = GetTickCount() + 60000;
		this->m_iremainTimeSec = -1;
	}

	this->m_bSendTimeCount = FALSE;
}





void CDevilSquare::SetPlaying()
{
	this->gObjScoreClear();
	this->SetMonster();

	this->m_iRemainTime = this->m_iPlaytime;

	if ( this->m_iPlaytime <= 1 )
	{
		this->m_iTime = GetTickCount();
		this->m_iremainTimeSec = 60;
	}
	else
	{
		this->m_iTime = GetTickCount() + 60000;
		this->m_iremainTimeSec = -1;
	}
	
	this->m_bSendTimeCount = FALSE;
}





int  CDevilSquare::GetDevilSquareIndex(int iGateNumber)
{
	switch ( iGateNumber )
	{

		case 58:
		case 59:
		case 60:
		case 61:
			return iGateNumber - 58;	// Devil 1 - 4
			break;

		case 111:
		case 112:
			return iGateNumber - 107;	// Devil 5 - 6
			break;
		case 270:
			return 6;
		default:
			return -1;
			break;
	}
}





int  CDevilSquare::GetUserLevelToEnter(int iUserIndex, int& btMoveGate)
{
	btMoveGate = -1;

	if ( gObjIsConnected(iUserIndex) == FALSE )
	{
		return -1;
	}

	int iENTER_LEVEL = -1;

#if (__CUSTOM__ == 1)
	if ( gObj[iUserIndex].ChangeUP2 == 1 )
#else
	if ( g_MasterExp.IsEnabled(&gObj[iUserIndex]) )
#endif
	{
		iENTER_LEVEL = 6;
		btMoveGate = g_sttDEVILSQUARE_LEVEL[iENTER_LEVEL].MoveGate;
	}
	else
	{
		for ( int i=0;i<MAX_DEVILSQUARE_GROUND-1;i++)
		{
			if ( gObj[iUserIndex].Class == CLASS_DARKLORD || gObj[iUserIndex].Class == CLASS_MAGUMSA || gObj[iUserIndex].Class == CLASS_FIGHTER )
			{
				if ( gObj[iUserIndex].Level >= g_sttDEVILSQUARE_LEVEL[i].SpecialCharacterMinLevel  && gObj[iUserIndex].Level <= g_sttDEVILSQUARE_LEVEL[i].SpecialCharacterMaxLevel )
				{
					iENTER_LEVEL = i;
					btMoveGate = g_sttDEVILSQUARE_LEVEL[i].MoveGate;
					break;
				}
			}
			else
			{
				if ( gObj[iUserIndex].Level >= g_sttDEVILSQUARE_LEVEL[i].NormalCharacterMinLevel  && gObj[iUserIndex].Level <= g_sttDEVILSQUARE_LEVEL[i].NormalCharacterMaxLevel )
				{
					iENTER_LEVEL = i;
					btMoveGate = g_sttDEVILSQUARE_LEVEL[i].MoveGate;
					break;
				}
			}
		}
	}	

	return iENTER_LEVEL;
}





BOOL CDevilSquare::Run()
{
	int count = 0;

	while ( this->m_bQuit == FALSE )
	{
		if ( gDevilSquareEvent != FALSE )
		{
			switch ( this->m_eState )
			{
				case DevilSquare_CLOSE:	this->ProcClose();		break;
				case DevilSquare_OPEN:		this->ProcOpen();		break;
				case DevilSquare_PLAYING:	this->ProcPlaying();	break;
			}
		}

		Sleep(100);
		count++;

		if ( count > 600 )
		{
			this->SendLiveCall();
			count = 0;
		}
	}

	return TRUE;
}


void CDevilSquare::CheckSync()
{
	std::list<DS_TIME>::iterator Itor_;

	DS_TIME pFirstTime;
	DS_TIME pTime;

	DWORD FIRST_TIME = 0;
	DWORD CHECK_TIME = 0;
	DWORD NOW_TIME = 0;

	if( m_EventTime.size() == 0)
	{
		return;
	}

	tm * today;
	time_t ltime;

	time(&ltime);
	today = localtime(&ltime);

	NOW_TIME = today->tm_hour * 60 + today->tm_min;

	pFirstTime = *m_EventTime.begin();

	for( Itor_ = m_EventTime.begin() ; Itor_ != m_EventTime.end(); ++Itor_ )
	{
		pTime = *Itor_;

		FIRST_TIME = pFirstTime.iHour * 60 + pFirstTime.iMinute;
		CHECK_TIME = pTime.iHour * 60 + pTime.iMinute;

		if( FIRST_TIME == CHECK_TIME )
		{
			if( NOW_TIME < CHECK_TIME )
			{
				pTime = *Itor_;
				break;
			}
			continue;
		}

		if( NOW_TIME >= FIRST_TIME &&
			NOW_TIME < CHECK_TIME )
		{
			break;
		}

		pFirstTime = *Itor_;
	}

	int CHECK_COUNT = 2;

	while( CHECK_COUNT-- )
	{
		if( Itor_ == m_EventTime.end() )
		{
			Itor_ = m_EventTime.begin();
			pTime = *Itor_;
		}

		CHECK_TIME = pTime.iHour * 60 + pTime.iMinute;

		if( today->tm_hour <= pTime.iHour )
		{
			this->m_iRemainTime = CHECK_TIME - NOW_TIME;
		}
		else
		{
			this->m_iRemainTime = 1440 - NOW_TIME + CHECK_TIME;
		}

		if( this->m_iRemainTime <= m_iOpenTime )
		{
			Itor_++;
			if( Itor_ != m_EventTime.end() )
			{
				pTime = *Itor_;
			}
		}
		else
		{
			m_iRemainTime -= m_iOpenTime;
			break;
		}
	}


	LogAddTD("[DevilSquare] Sync Open Time. [%d]min remain", this->m_iRemainTime);
}

int CDevilSquare::GetRemainTime()
{
	std::list<DS_TIME>::iterator Itor_;

	DS_TIME pFirstTime;
	DS_TIME pTime;

	DWORD FIRST_TIME = 0;
	DWORD CHECK_TIME = 0;
	DWORD NOW_TIME = 0;
	tm * today;
	time_t ltime;
	int iRemainTime = 0;


	if( m_EventTime.size() == 0)
	{
		return false;
	}

	time(&ltime);
	today = localtime(&ltime);

	NOW_TIME = today->tm_hour * 60 + today->tm_min;

	pFirstTime = *m_EventTime.begin();

	for( Itor_ = m_EventTime.begin() ; Itor_ != m_EventTime.end(); ++Itor_ )
	{
		pTime = *Itor_;

		FIRST_TIME = pFirstTime.iHour * 60 + pFirstTime.iMinute;
		CHECK_TIME = pTime.iHour * 60 + pTime.iMinute;

		if( FIRST_TIME == CHECK_TIME )
		{
			if( NOW_TIME < CHECK_TIME )
			{
				pTime = *Itor_;
				break;
			}
			continue;
		}

		if( NOW_TIME >= FIRST_TIME &&
			NOW_TIME < CHECK_TIME )
		{
			break;
		}

		pFirstTime = *Itor_;
	}

	int CHECK_COUNT = 2;

	while( CHECK_COUNT-- )
	{
		if( Itor_ == m_EventTime.end() )
		{
			Itor_ = m_EventTime.begin();
			pTime = *Itor_;
		}

		CHECK_TIME = pTime.iHour * 60 + pTime.iMinute;

		if( today->tm_hour <= pTime.iHour )
		{
			iRemainTime = CHECK_TIME - NOW_TIME;
		}
		else
		{
			iRemainTime = 1440 - NOW_TIME + CHECK_TIME;
		}

		if( iRemainTime <= m_iOpenTime )
		{
			Itor_++;
			if( Itor_ != m_EventTime.end() )
			{
				pTime = *Itor_;
			}
		}
		else
		{
			iRemainTime -= m_iOpenTime;
			break;
		}
	}
return iRemainTime;
}

void CDevilSquare::ProcClose()
{
	if ( this->m_iremainTimeSec != -1 )
	{
		int lc2 = (GetTickCount() - this->m_iTime)/1000;

		if ( lc2 != 0 )
		{
			this->m_iremainTimeSec -= lc2;
			this->m_iTime += lc2 * 1000;

			if ( this->m_iremainTimeSec <= 30 && this->m_bSendTimeCount == FALSE )
			{
				PMSG_SET_DEVILSQUARE pMsg;

				PHeadSetB((LPBYTE)&pMsg, 0x92, sizeof(pMsg));
				pMsg.Type = DevilSquare_CLOSE;

				for ( int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
				{
					if ( gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
					{
						if ( BC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
						{
							if ( CC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
							{
								DataSend(i, (BYTE *)&pMsg, pMsg.h.size);
							}
						}
					}
				}


				this->m_bSendTimeCount = TRUE;
			}		

			if (this->m_iremainTimeSec < 1 )
			{
				PMSG_NOTICE pNotice;

				TNotice::MakeNoticeMsg((TNotice *)&pNotice, 0, lMsg.Get(MSGGET(2, 191)));

				for ( int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
				{
					if ( gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
					{
						if ( BC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
						{
							if ( CC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
							{
								DataSend(i, (BYTE *)&pNotice, pNotice.h.size);
							}
						}
					}
				}

				LogAddTD(pNotice.Notice);
				this->SetState(DevilSquare_OPEN);
			}
		
		}
	}
	else if ( this->m_iTime < GetTickCount() )
	{
		this->ClearMonstr();
		this->m_iRemainTime--;

		if ( this->m_bFlag == 0 )
		{
			for ( int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
			{
				if ( gObj[i].Connected == PLAYER_PLAYING )
				{
					if ( DS_MAP_RANGE(gObj[i].MapNumber) != FALSE )
					{
						gObjMoveGate(i, 27);
					}
				}
			}

			this->ItemClear();
		}
		else
		{
			this->CheckInvalidUser();
		}

		if ( this->m_iRemainTime <= 15 )
		{
			if ( (this->m_iRemainTime%5)== 0 )
			{
				PMSG_NOTICE pNotice;

				TNotice::MakeNoticeMsgEx((TNotice *)&pNotice, 0, lMsg.Get(MSGGET(2, 192)), this->m_iRemainTime);

				for ( int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
				{
					if ( gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
					{
						if ( BC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
						{
							if ( CC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
							{
								DataSend(i, (BYTE *)&pNotice, pNotice.h.size);
							}
						}
					}
				}

				LogAddTD((char*)pNotice.Notice);
			}
		}

		if ( this->m_iRemainTime == 1 && this->m_iremainTimeSec == -1 )
		{
			this->m_iremainTimeSec = 60;
			this->m_iTime = GetTickCount();
		}
		else
		{
			this->m_iTime = GetTickCount() + 60000;
		}
	}
}




void CDevilSquare::ProcOpen()
{
	if ( this->m_iremainTimeSec != -1 )
	{

		int lc2 = (GetTickCount() - this->m_iTime)/1000;

		if ( lc2 != 0 )
		{
			this->m_iremainTimeSec -= lc2;
			this->m_iTime += lc2 * 1000;

			if ( this->m_iremainTimeSec <= 30 && this->m_bSendTimeCount == FALSE )
			{
				PMSG_SET_DEVILSQUARE pMsg;

				PHeadSetB((LPBYTE)&pMsg, 0x92, sizeof(pMsg));
				pMsg.Type = DevilSquare_OPEN;

				for ( int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
				{
					if ( gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
					{
						if ( BC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
						{
							if ( CC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
							{
								DataSend(i, (BYTE *)&pMsg, pMsg.h.size);
							}
						}
					}
				}

				this->m_bSendTimeCount = TRUE;
			}

			if (this->m_iremainTimeSec < 1 )
			{
				this->SendEventStartMsg();
				this->SetState(DevilSquare_PLAYING);
			}
		
		}
	}
	else
	{
		if ( this->m_iTime < GetTickCount() )
		{
			this->ClearMonstr();
			this->m_iRemainTime--;

			if ( this->m_iRemainTime > 0 )
			{
				PMSG_NOTICE pNotice;

				TNotice::MakeNoticeMsgEx(&pNotice, 0, lMsg.Get(MSGGET(2, 193)), this->m_iRemainTime);

				for ( int i=OBJ_STARTUSERINDEX;i<OBJMAX;i++)
				{
					if ( gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
					{
						if ( BC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
						{
							if ( CC_MAP_RANGE(gObj[i].MapNumber) == FALSE )
							{
								DataSend(i, (BYTE *)&pNotice, pNotice.h.size);
							}
						}
					}
				}

				LogAddTD((char*)pNotice.Notice);

				this->m_iTime = GetTickCount();
			}

			if (this->m_iRemainTime == 1 && this->m_iremainTimeSec == -1)
			{
				this->m_iremainTimeSec = 60;
				this->m_iTime = GetTickCount();
			}
			else
			{
				this->m_iTime = GetTickCount() + 60000;
			}
		}
	}
}







void CDevilSquare::ProcPlaying()
{
	if ( this->m_iremainTimeSec != -1 )
	{

		int lc2 = (GetTickCount() - this->m_iTime)/1000;

		if ( lc2 != 0 )
		{
			this->m_iremainTimeSec -= lc2;
			this->m_iTime += lc2 * 1000;

			if ( this->m_iremainTimeSec <= 30 && this->m_bSendTimeCount == FALSE )
			{
				PMSG_SET_DEVILSQUARE pMsg;

				PHeadSetB((LPBYTE)&pMsg, 0x92, sizeof(pMsg));
				pMsg.Type = DevilSquare_PLAYING;

				for(int i = 0; i < OBJMAX; i++)
				{
					if( gObj[i].Connected == PLAYER_PLAYING &&
						gObj[i].Type == OBJ_USER &&
						DS_MAP_RANGE(gObj[i].MapNumber) )
					{
						DataSend(i,(LPBYTE)&pMsg,sizeof(pMsg));
					}
				}

				this->m_bSendTimeCount = TRUE;
			}



			if ( this->m_iremainTimeSec < 1 )
			{
				this->SetState(DevilSquare_CLOSE);
			}
		}
	}
	else
	{
		if ( this->m_iTime < GetTickCount() )
		{
			this->m_iRemainTime--;

			if ( this->m_iRemainTime > 0 )
			{
				if ( (this->m_iRemainTime%5) == 0 )
				{
					PMSG_NOTICE pNotice;

					TNotice::MakeNoticeMsgEx((TNotice *)&pNotice, 0, lMsg.Get(MSGGET(2, 194)), this->m_iRemainTime);
					LogAddTD((char*)pNotice.Notice);
					AllSendSameMapMsg((UCHAR*)&pNotice, pNotice.h.size, MAP_INDEX_DEVILSQUARE);
					AllSendSameMapMsg((UCHAR*)&pNotice, pNotice.h.size, MAP_INDEX_DEVILSQUARE2);
				}

				for ( int i=0;i<MAX_DEVILSQUARE_GROUND;i++)
				{
					this->m_DevilSquareGround[i].RegenBossMonster(this->m_iPlaytime - this->m_iRemainTime);
				}

				if ( this->m_iRemainTime == 1 && this->m_iremainTimeSec == -1 )
				{
					this->m_iremainTimeSec = 60;
					this->m_iTime = GetTickCount();
				}
				else
				{
					this->m_iTime = GetTickCount() + 60000;
				}
			}
		}
	}
}





void CDevilSquare::SetMonster()
{
	int result;
	for ( int n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( DS_MAP_RANGE(gMSetBase.m_Mp[n].m_MapNumber) != FALSE )
		{
			WORD wMonIndex = gMSetBase.m_Mp[n].m_Type;
			BYTE btDSIndex = -1;

			if ( gMSetBase.m_Mp[n].m_X == 119 )
			{
				btDSIndex = DEVIL_SQUARE_GROUND_1;
			}
			else if ( gMSetBase.m_Mp[n].m_X == 121 )
			{
				btDSIndex = DEVIL_SQUARE_GROUND_2;
			}
			else if ( gMSetBase.m_Mp[n].m_X == 49 )
			{
				btDSIndex = DEVIL_SQUARE_GROUND_3;
			}
			else if ( gMSetBase.m_Mp[n].m_X == 53 )
			{
				btDSIndex = DEVIL_SQUARE_GROUND_4;
			}
			else if ( gMSetBase.m_Mp[n].m_X == 120 )
			{
				btDSIndex = DEVIL_SQUARE_GROUND_5;
			}
			else if ( gMSetBase.m_Mp[n].m_X == 122 )
			{
				btDSIndex = DEVIL_SQUARE_GROUND_6;
			}
			else if ( gMSetBase.m_Mp[n].m_X == 50 )
			{
				btDSIndex = DEVIL_SQUARE_GROUND_7;
			}

			if ( btDSIndex == 0xFF )
			{
				LogAddTD("[DevilSquare] [%d] Invalid MonterType", wMonIndex);
				continue;
			}

			result = gObjAddMonster(gMSetBase.m_Mp[n].m_MapNumber);

			if ( result >= 0 )
			{
				gObjSetPosMonster(result, n);
				gObjSetMonster(result, wMonIndex); 
				gObj[result].MaxRegenTime = 1000;	// Rgeneration in 1 second
				gObj[result].m_bDevilSquareIndex = btDSIndex;
			}
		}
	}
}





void CDevilSquare::ClearMonstr()
{
	for ( int n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( DS_MAP_RANGE(gObj[n].MapNumber) != FALSE )
		{
			gObjDel(n);
		}
	}
}





void CDevilSquare::gDevilSquareMonsterRegen(LPOBJ lpObj)
{
	BYTE devilsquareindex = lpObj->m_bDevilSquareIndex;

	if ( devilsquareindex < 0 || devilsquareindex >= MAX_DEVILSQUARE_GROUND )
	{
		LogAddTD("[DevilSquare] Invalid DevilSquareIndex [%d]", devilsquareindex);
		return;
	}

	if ( lpObj->m_PosNum == -1 )
	{
		gObjDel(lpObj->m_Index);
		return;
	}

	WORD monstertype = this->m_DevilSquareGround[devilsquareindex].GetMonsterType(this->m_iPlaytime - this->m_iRemainTime );

	if ( monstertype == (BYTE)-1 )
	{
		LogAddTD("[DevilSquare] [%d] Invalid MonterType", monstertype);
		return;
	}

	for ( int n=0;n<MAX_MAGIC;n++)
	{
		lpObj->Magic[n].Clear();
	}

	gObjSetMonster(lpObj->m_Index, monstertype);
	lpObj->DieRegen = FALSE;
	gObjMonsterRegen(lpObj);
	CreateFrustrum(lpObj->X, lpObj->Y, lpObj->m_Index);
	lpObj->m_bDevilSquareIndex = devilsquareindex;
	lpObj->MaxRegenTime = 1000;

	LogAddTD("[DevilSquare] Monter Regen [%d][%d][%d,%d]",
		monstertype, devilsquareindex, lpObj->X, lpObj->Y);
}





void CDevilSquare::SendEventStartMsg()
{
	PMSG_NOTICE pToEventer;
	PMSG_NOTICE pWithOutEvneter;

	TNotice::MakeNoticeMsg(&pToEventer, 0, lMsg.Get(MSGGET(2, 195)));
	TNotice::MakeNoticeMsg(&pWithOutEvneter, 0, lMsg.Get(MSGGET(2, 196)));

	for ( int n=OBJ_STARTUSERINDEX;n<OBJMAX;n++)
	{
		if ( gObj[n].Connected == PLAYER_PLAYING )
		{
			if ( DS_MAP_RANGE(gObj[n].MapNumber) != FALSE )
			{
				DataSend(n, (BYTE *)&pToEventer, pToEventer.h.size);
			}
			else
			{
				DataSend(n, (BYTE *)&pWithOutEvneter, pWithOutEvneter.h.size);
			}
		}
	}

	LogAddTD("[DevilSquare] Start Event");
}





void CDevilSquare::DieProcDevilSquare(LPOBJ lpObj)
{
	char msg[255];

	wsprintf(msg, lMsg.Get(MSGGET(2, 197)), lpObj->m_nEventScore);
	GCServerMsgStringSend(msg, lpObj->m_Index, 1);

	if ( lpObj->m_nEventScore <= 0 )
		return;

	PMSG_ANS_EVENTUSERSCORE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x01;
	pMsg.h.size = sizeof(pMsg);
	pMsg.SquareNum = lpObj->m_bDevilSquareIndex;
	pMsg.Class = lpObj->Class;

	if ( lpObj->Class == 1 )
	{
		if ( lpObj->Class >= 0 && lpObj->Class < MAX_TYPE_PLAYER )
		{
			if ( DS_LEVEL_RANGE(lpObj->m_bDevilSquareIndex) != FALSE )
			{
				lpObj->m_nEventScore+= this->m_BonusScoreTable[lpObj->Class][lpObj->m_bDevilSquareIndex]/100;
				
			}
		}

		pMsg.Score = lpObj->m_nEventScore;
	}
	else
	{
		pMsg.Score = lpObj->m_nEventScore;
	}

	pMsg.ServerCode = gGameServerCode;
	memcpy(pMsg.AccountID, lpObj->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.GameID, lpObj->Name, sizeof(pMsg.GameID));

	DataSendRank((char *)&pMsg, pMsg.h.size);

	LogAddTD("[DevilSquare] Dead [%s][%s][%d][%d]",
		lpObj->AccountID, lpObj->Name, 
		lpObj->m_nEventExp, lpObj->m_nEventScore);

	lpObj->m_nEventScore = 0;
	lpObj->m_nEventMoney = 0;
	lpObj->m_nEventExp = 0;

}






int  CDevilSquare::gObjMonsterExpSingle(LPOBJ lpObj, LPOBJ lpTargetObj, int dmg, int tot_dmg)
{
	__int64 exp;
	__int64 maxexp = 0;
	int level = ((lpTargetObj->Level + 25) * lpTargetObj->Level) / 3;

	if ( (lpTargetObj->Level + 10) < lpObj->Level  )
	{
		level = (level*(lpTargetObj->Level + 10))/(lpObj->Level);
	}

	if ( lpTargetObj->Level >= 65 )
	{
		level += (lpTargetObj->Level-64)*(lpTargetObj->Level / 4);
	}

	if ( level > 0 )
	{
		maxexp = level / 2;
	}
	else
	{
		level = 0;
	}

	if ( maxexp < 1 )
	{
		exp = level;
	}
	else
	{
		exp = level + rand()%maxexp;
	}

	exp = (dmg * exp)/tot_dmg;

	DWORD mymaxexp = gLevelExperience[lpObj->Level];

	__int64 mymaxexp_ = mymaxexp;

	if ( exp > mymaxexp_ )
	{
		exp = mymaxexp;
	}

	lpTargetObj->Money += (int)exp;

	if ( g_MasterExp.IsEnabled(lpObj) == FALSE )
	{
		exp += (exp * m_ObjBill[lpObj->m_Index].GetExp()) / 100;
		exp = int(exp * g_MapRateInfo.GetExp(lpObj->MapNumber));
	}

	if ( g_CrywolfSync.GetOccupationState() == 1 && g_iCrywolfApplyMvpPenalty != FALSE)
	{
		exp =  (exp * g_CrywolfSync.GetGettingExpPenaltyRate()) / 100;
	}

	if ( exp > 0 )
	{
		if ( lpObj->Type == OBJ_USER )
		{

			CheckItemOptForGetExpEx(lpObj,exp,0);

			lpObj->Experience += (DWORD)exp;
			lpObj->m_nEventExp += (int)exp;
		
			if ( gObjLevelUp(lpObj, &exp, lpTargetObj->Class, 0) == false )
			{
				return 0;
			}
		}
	}

	return (int)exp;
}


		


void CDevilSquare::gObjExpParty(LPOBJ lpObj, LPOBJ lpTargetObj, int AttackDamage, BOOL MSBFlag)
{
	int n;
	__int64 exp;
	__int64 maxexp = 0;
	__int64 totalexp;
	int level = ((lpTargetObj->Level + 25) * lpTargetObj->Level) / 3;
	int number;
	int partynum = 0;
	int totallevel = 0;
	int partylevel;
	int partycount;
	int dis[6];
	int viewplayer = 0;
	int viewpercent = 100;
	BOOL bApplaySetParty = FALSE;
	bool bCheckSetParty[6];
	partynum = lpObj->PartyNumber;
	LPOBJ lpPartyObj;

	
	int toplevel = 0;
	int distance;

	for (n=0;n<MAX_USER_IN_PARTY;n++)
	{
		number = gParty.m_PartyS[partynum].Number[n];

		if ( number >= 0 )
		{
			lpPartyObj = &gObj[number];

			distance = gObjCalDistance(lpObj,lpPartyObj);

			if( distance < 10 )
			{
				if(lpPartyObj->Level > toplevel)
				{
					toplevel = lpPartyObj->Level;
				}
			}
		}
	}

	if ( OBJMAX_RANGE(partynum) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return;
	}

	partycount = gParty.m_PartyS[partynum].Count;

	for ( n =0;n<MAX_USER_IN_PARTY;n++)
	{
		number = gParty.m_PartyS[partynum].Number[n];

		if ( number >= 0 )
		{
			lpPartyObj = &gObj[number];

			if ( lpTargetObj->MapNumber ==lpPartyObj->MapNumber )
			{
				dis[n] = gObjCalDistance(lpTargetObj, &gObj[number]);

				if ( dis[n] < 10 )
				{
					lpPartyObj = &gObj[number];

					if ( toplevel >= (lpPartyObj->Level + 200 ) ) // #formula
					{
						totallevel = totallevel + lpPartyObj->Level + 200;
					}
					else
					{
						totallevel += lpPartyObj->Level;
					}

					viewplayer++;
					bCheckSetParty[lpPartyObj->Class] = true;
				}
			}
		}
	}

	viewpercent += gParty.GetExpBonus(lpObj, lpTargetObj, partynum);

	if ( viewplayer > 1 )
	{
		partylevel = totallevel / viewplayer;
	}
	else
	{
		partylevel = totallevel;
	}
	
	if ( (lpTargetObj->Level +10) < partylevel )
	{
		level = (level * (lpTargetObj->Level+10) ) / partylevel;
	}

	if ( lpTargetObj->Level >= 65 )
	{
		if ( viewplayer == 1 )
		{
			level += (lpTargetObj->Level - 64) * (lpTargetObj->Level/ 4);
		}
		else
		{
			level += (int)(200.0 - (lpObj->Level * 0.2));
		}
	}

	if ( level > 0 )
	{
		maxexp = level / 2;
	}
	else
	{
		level = 0;
	}

	if ( maxexp < 1 )
	{
		totalexp = level;
	}
	else
	{
		totalexp = level + rand()%maxexp;
	}

	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
		lpTargetObj->Money = (int)totalexp;
	}

	for ( n=0;n<MAX_USER_IN_PARTY;n++)
	{
		number = gParty.m_PartyS[partynum].Number[n];

		if ( number >= 0 )
		{
			lpPartyObj = &gObj[number];

			if ( lpTargetObj->MapNumber == lpPartyObj->MapNumber )
			{
				if ( dis[n] < 10 )
				{
					DWORD myexp = gLevelExperience[lpPartyObj->Level];
					exp = ((totalexp * viewpercent* lpPartyObj->Level ) / totallevel ) / 100;

					if ( exp > myexp  )
					{
						exp = myexp;
					}

					if( lpPartyObj->Type == OBJ_USER )
					{
						if ( lpTargetObj->Type == OBJ_USER )
						{
							exp = 0;
						}
					}

					if ( g_MasterExp.IsEnabled(lpPartyObj) == FALSE )
					{
						exp += (exp * m_ObjBill[lpPartyObj->m_Index].GetExp()) / 100;
						exp = int(exp * g_MapRateInfo.GetExp(lpPartyObj->MapNumber));
					}

					if ( g_CrywolfSync.GetOccupationState() == 1 && g_iCrywolfApplyMvpPenalty != FALSE)
					{
						exp =  (exp * g_CrywolfSync.GetGettingExpPenaltyRate()) / 100;
					}

					if ( exp > 0 )
					{
						if ( lpPartyObj->Type == OBJ_USER )
						{

							CheckItemOptForGetExpEx(lpPartyObj,exp,0);

							lpPartyObj->Experience += (DWORD)exp;
							lpPartyObj->m_nEventExp += (int)exp;
						
							if ( gObjLevelUp(lpPartyObj, &exp, lpTargetObj->Class, EVENT_TYPE_PARTY) == false )
							{
								continue;
							}
						}
					}

					if ( lpPartyObj->Type == OBJ_USER )
					{
						GCKillPlayerExpSend(lpPartyObj->m_Index, lpTargetObj->m_Index,(int)exp, AttackDamage, MSBFlag);
					}
				}
			}
		}
	}
}






void CDevilSquare::gObjMonsterScoreDivision(LPOBJ lpMonObj, LPOBJ lpObj, int AttackDamage, BOOL MSBFlag)
{
	::gObjMonsterHitDamageUserDel(lpMonObj);
	lpMonObj->Money = 0;

	LPOBJ lpTargetObj;
	int HitIndex;
	int LastHitObjNum = ::gObjMonsterLastHitDamageUser(lpMonObj, HitIndex);

	if ( LastHitObjNum != -1 )
	{
		lpTargetObj = &gObj[LastHitObjNum];
		int lc5 = (int)(lpMonObj->sHD[HitIndex].HitDamage / lpMonObj->MaxLife * lpMonObj->Level);
		lc5 *= lpTargetObj->m_bDevilSquareIndex + 1;
		lpTargetObj->m_nEventScore += lc5;
	}
}




void CDevilSquare::ItemClear()
{
	int CurTime = GetTickCount();

	for ( int i=0;i<MAX_MAPITEM;i++)
	{
		MapC[MAP_INDEX_DEVILSQUARE].m_cItem[i].m_State = 8;
	}
}




void CDevilSquare::CalcScore()
{
	int n;
	for ( n=0;n<MAX_DEVILSQUARE_GROUND;n++)
	{
		this->m_DevilSquareGround[n].ClearScore();
	}

	for ( n=OBJ_STARTUSERINDEX;n<OBJMAX;n++)
	{
		if ( gObj[n].Connected == PLAYER_PLAYING )
		{
			if ( DS_MAP_RANGE(gObj[n].MapNumber) != FALSE )
			{
				if ( DS_LEVEL_RANGE(gObj[n].m_bDevilSquareIndex) != FALSE )
				{
					this->m_DevilSquareGround[gObj[n].m_bDevilSquareIndex].InsertObj(&gObj[n]);
				}
			}
		}
	}

	for (  n=0;n<MAX_DEVILSQUARE_GROUND;n++)
	{
		this->m_DevilSquareGround[n].SortScore();
		this->m_DevilSquareGround[n].SendScore();
	}
}



struct PMSG_ANS_EVENTLIVE
{
	PBMSG_HEAD h;	// C1:03
};




void CDevilSquare::SendLiveCall()
{
	PMSG_ANS_EVENTLIVE pMsg;
	
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x03;
	pMsg.h.size = sizeof(pMsg);

	DataSendRank((char *)&pMsg, pMsg.h.size);
}




void CDevilSquare::gObjScoreClear()
{
	for ( int n=OBJ_STARTUSERINDEX;n<OBJMAX;n++)
	{
		if ( gObj[n].Connected == PLAYER_PLAYING )
		{
			if ( DS_MAP_RANGE(gObj[n].MapNumber) != FALSE )
			{
				gObj[n].m_nEventScore = 0;
				gObj[n].m_nEventMoney = 0;
				gObj[n].m_nEventExp = 0;
			}
		}
	}
}






void CDevilSquare::CheckInvalidUser()
{
	for ( int n=OBJ_STARTUSERINDEX;n<OBJMAX;n++)
	{
		if ( gObj[n].Connected == PLAYER_PLAYING )
		{
			if ( DS_MAP_RANGE(gObj[n].MapNumber) != FALSE )
			{
				if ( this->m_eState == DevilSquare_CLOSE )
				{
					LogAddC(2, "[DevilSquare] [%s][%s] Found user in DevilSquare [State:Close]",
						gObj[n].AccountID, gObj[n].Name);
					gObjUserKill(gObj[n].m_Index);
				}
			}
		}
	}
}


int CDevilSquare::AddUser(int nDevilSquareIndex,int UserIndex)
{
	if( DS_LEVEL_RANGE(nDevilSquareIndex) )
	{
		return this->m_DevilSquareGround[nDevilSquareIndex].AddUser(UserIndex);
	}
	return false;
}

int CDevilSquare::DelUser(int nDevilSquareIndex,int UserIndex)
{
	if( DS_LEVEL_RANGE(nDevilSquareIndex) )
	{
		return this->m_DevilSquareGround[nDevilSquareIndex].DelUser(UserIndex);
	}
	return false;
}

int CDevilSquare::ChangeUserIndex(int nExUserIndex,int nCurrentUserIndex,int nDevilSquareIndex)
{
	if( DS_LEVEL_RANGE(nDevilSquareIndex) )
	{
		return this->m_DevilSquareGround[nDevilSquareIndex].ChangeUserIndex(nExUserIndex,nCurrentUserIndex);
	}
	return false;
}

int CDevilSquare::LeaveDevilSquare(int nDevilSquareIndex,int UserIndex)
{
	return this->DelUser(nDevilSquareIndex,UserIndex);
}

int CDevilSquare::GetPlayUserCountRightNow(int nDevilSquareIndex)
{
	if( DS_LEVEL_RANGE(nDevilSquareIndex) )
	{
		return this->m_DevilSquareGround[nDevilSquareIndex].GetPlayUserCountRightNow();
	}
	return -1;
}