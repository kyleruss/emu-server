#include "stdafx.h"
#include "DevilSquareGround.h"
#include "DevilSquare.h"
#include "GameMain.h"
#include "Logproc.h"
#include "..\common\winutil.h"
#include "CrywolfSync.h"
#include "BuffEffectSlot.h"
#include "MasterSkillSystem.h"
#include "PCSProtocol.h"
#include "CashShop.h"
#include "DSProtocol.h"
#include "EventItemBag.h"
#include "ShopPointEx.h"

#if( __4GAMERS__ == 1 )
#include "Achievements.h"
#endif

bool DevilSquareScoreSort(LPOBJ const & lhs,LPOBJ const & rhs)
{
	if ( lhs->m_nEventScore == rhs->m_nEventScore )
	{
		if ( lhs->Level < rhs->Level)
		{
			return true;
		}

		return false;
	}

	if ( lhs->m_nEventScore > rhs->m_nEventScore)
	{
		return true;
	}

	return false;
}



CDevilSquareGround::CDevilSquareGround()
{
	this->Init(0);
	InitializeCriticalSection(&this->m_UserCrit);
}




CDevilSquareGround::~CDevilSquareGround()
{
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

	DeleteCriticalSection(&this->m_UserCrit);
}




void CDevilSquareGround::Init(int Index)
{
	this->m_dwObjCount = 0;
	this->m_iIndex = Index;
	memset(this->m_DevilSquareMonsterInfo, 0xFF, sizeof(this->m_DevilSquareMonsterInfo));
	memset(this->m_DevilSquareBossMonsterInfo, 0xFF, sizeof(this->m_DevilSquareBossMonsterInfo));
}




void CDevilSquareGround::Clear()
{
	this->m_dwObjCount = 0;
}





void CDevilSquareGround::Set(WORD type, int starttime, int endtime)
{
	for (int i=0;i<MAX_ST_DEVILSQUARE_MONSTER;i++)
	{
		if ( this->m_DevilSquareMonsterInfo[i].m_Type == (WORD)-1  )
		{
			this->m_DevilSquareMonsterInfo[i].m_Type = type;
			this->m_DevilSquareMonsterInfo[i].m_RegenStartTime = starttime;
			this->m_DevilSquareMonsterInfo[i].m_RegenEndTime = endtime;
			this->m_DevilSquareMonsterInfo[i].m_iDevilSquareNumber = this->m_iIndex;
			
			return;
		}
	}
}





void CDevilSquareGround::SetBoss(WORD type, int starttime, int x, int y, int tx, int ty)
{
	for (int i=0;i<MAX_ST_DEVILSQUARE_MONSTER;i++)
	{
		if ( this->m_DevilSquareBossMonsterInfo[i].m_Type == (WORD)-1  )
		{
			this->m_DevilSquareBossMonsterInfo[i].m_Type = type;
			this->m_DevilSquareBossMonsterInfo[i].m_RegenStartTime = starttime;
			this->m_DevilSquareBossMonsterInfo[i].X = x;
			this->m_DevilSquareBossMonsterInfo[i].Y = y;
			this->m_DevilSquareBossMonsterInfo[i].TX = tx;
			this->m_DevilSquareBossMonsterInfo[i].TY = ty;
			this->m_DevilSquareBossMonsterInfo[i].m_iDevilSquareNumber = this->m_iIndex;
			
			return;
		}
	}
}


void CDevilSquareGround::SetBonus(int index, int zen, int exp)
{
	this->m_Bonus[index][0] = exp;
	this->m_Bonus[index][1] = zen;
}



void CDevilSquareGround::IncObjCount()
{
	InterlockedIncrement((LPLONG)&this->m_dwObjCount);
}




long  CDevilSquareGround::GetObjCount()
{
	return this->m_dwObjCount;
}





void CDevilSquareGround::RegenBossMonster(int currtime)
{
	for ( int i=0;i<MAX_ST_DEVILSQUARE_MONSTER;i++)
	{
		if ( this->m_DevilSquareBossMonsterInfo[i].m_Type != (WORD)-1 )
		{
			if ( currtime == this->m_DevilSquareBossMonsterInfo[i].m_RegenStartTime )
			{
				int iMapNumber = (this->m_DevilSquareBossMonsterInfo[i].m_iDevilSquareNumber >= DEVIL_SQUARE_GROUND_1&& this->m_DevilSquareBossMonsterInfo[i].m_iDevilSquareNumber <= DEVIL_SQUARE_GROUND_4)?MAP_INDEX_DEVILSQUARE:(this->m_DevilSquareBossMonsterInfo[i].m_iDevilSquareNumber>=DEVIL_SQUARE_GROUND_5  && this->m_DevilSquareBossMonsterInfo[i].m_iDevilSquareNumber < 7 )?MAP_INDEX_DEVILSQUARE2:-1;

				int iMonIndex = gObjAddMonster(iMapNumber);

				if ( iMonIndex >= 0 )
				{
					gObjSetMonster(iMonIndex, this->m_DevilSquareBossMonsterInfo[i].m_Type);
					gObj[iMonIndex].MapNumber = iMapNumber;
					gMSetBase.GetBoxPosition(gObj[iMonIndex].MapNumber,
						this->m_DevilSquareBossMonsterInfo[i].X, this->m_DevilSquareBossMonsterInfo[i].Y,
						this->m_DevilSquareBossMonsterInfo[i].TX, this->m_DevilSquareBossMonsterInfo[i].TY,
						gObj[iMonIndex].X, gObj[iMonIndex].Y);
					int map = gObj[iMonIndex].MapNumber;
					int x = gObj[iMonIndex].X;
					int y = gObj[iMonIndex].Y;

					LogAddTD("[DevilSquare] Boss %d %d %d create ", map, x, y);

					gObj[iMonIndex].TX = gObj[iMonIndex].X;
					gObj[iMonIndex].TY = gObj[iMonIndex].Y;
					gObj[iMonIndex].MTX = gObj[iMonIndex].X;
					gObj[iMonIndex].MTY = gObj[iMonIndex].Y;
					gObj[iMonIndex].StartX = gObj[iMonIndex].X;
					gObj[iMonIndex].StartY = gObj[iMonIndex].Y;
					gObj[iMonIndex].m_bDevilSquareIndex = this->m_iIndex;
					gObj[iMonIndex].DieRegen = FALSE;
					gObj[iMonIndex].m_PosNum = -1;
					gObj[iMonIndex].Live = TRUE;
					gObj[iMonIndex].m_State = 1;
				}
			}
		}
	}
}






BOOL CDevilSquareGround::FindMonsterType(WORD type)
{
	for ( int i=0;i<MAX_ST_DEVILSQUARE_MONSTER;i++)
	{
		if ( this->m_DevilSquareMonsterInfo[i].m_Type == type )
		{
			return TRUE;
		}
	}

	return FALSE;
}




WORD CDevilSquareGround::GetMonsterType(int currtime)
{
	WORD sTemp[MAX_ST_DEVILSQUARE_MONSTER];
	int icount = 0;

	for ( int i=0;i<MAX_ST_DEVILSQUARE_MONSTER;i++)
	{
		if ( this->m_DevilSquareMonsterInfo[i].m_Type != (WORD)-1 )
		{
			if ( currtime >= this->m_DevilSquareMonsterInfo[i].m_RegenStartTime )
			{
				if ( currtime <= this->m_DevilSquareMonsterInfo[i].m_RegenEndTime )
				{
					sTemp[icount] = this->m_DevilSquareMonsterInfo[i].m_Type;
					icount++;
				}
			}
		}
	}

	if ( icount == 0 )
	{
		return (BYTE)-1;
	}

	return sTemp[rand()%icount];
}






void CDevilSquareGround::ClearScore()
{
	this->m_DevilSquareRankList.clear();
}





void CDevilSquareGround::SortScore()
{
	std::sort( this->m_DevilSquareRankList.begin(), this->m_DevilSquareRankList.end(), DevilSquareScoreSort);
}





void CDevilSquareGround::InsertObj(LPOBJ lpObj)
{
	if ( lpObj->Authority != 2 )
	{
		if ( lpObj->Class >= 0 && lpObj->Class < MAX_TYPE_PLAYER)
		{
			if ( DS_LEVEL_RANGE(lpObj->m_bDevilSquareIndex) != FALSE )
			{
				lpObj->m_nEventScore +=  this->m_BonusScoreTable[lpObj->Class][lpObj->m_bDevilSquareIndex] / 100;
			}
		}

		this->m_DevilSquareRankList.push_back(lpObj);
	}
}


void CDevilSquareGround::IncBonus(float IncValue)
{
	for(int i = 0; i < 4; i++)
	{
		this->m_Bonus[i][1] = (int)(this->m_Bonus[i][1]*IncValue);
	}
}

void CDevilSquareGround::SendScore()
{
	if ( this->m_DevilSquareRankList.size() < 1 )
	{
		return;
	}

	BYTE count = 1;
	int iUserCount = this->m_DevilSquareRankList.size();
	int iExp = 0;

	std::vector<LPOBJ>::iterator Itor = this->m_DevilSquareRankList.begin();

	for ( ; Itor != this->m_DevilSquareRankList.end() ; Itor++ )
	{
		memcpy(this->m_DevilSquareScoreInfoTOP10.Score[count].Name , (*(Itor))->Name, MAX_ACCOUNT_LEN);
		this->m_DevilSquareScoreInfoTOP10.Score[count].TotalScore = (*(Itor))->m_nEventScore;

		if ( iUserCount <= 6 )
		{
			if ( count < 4 )	// For DS from 0 to 3
			{
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusZen = this->m_Bonus[count-1][0];
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusExp = this->m_Bonus[count-1][1];
			}
			else
			{
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusZen = this->m_Bonus[3][0];
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusExp = this->m_Bonus[3][1];
			}
		}
		else
		{
			int lc5 = count*100/iUserCount;

			if ( count == 1 )
			{
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusZen = this->m_Bonus[0][0];
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusExp = this->m_Bonus[0][1];
			}
			else if ( lc5 <= 30 )
			{
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusZen = this->m_Bonus[1][0];
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusExp = this->m_Bonus[1][1];
			}
			else if ( lc5 <= 50 )
			{
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusZen = this->m_Bonus[2][0];
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusExp = this->m_Bonus[2][1];
			}
			else
			{
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusZen = this->m_Bonus[3][0];
				this->m_DevilSquareScoreInfoTOP10.Score[count].BonusExp = this->m_Bonus[3][1];
			}
		}

		count++;

		if ( count >= 10 )
		{
			break;
		}
	}

	this->m_DevilSquareScoreInfoTOP10.Count  = count;
	int iSize = count * 24 + 5;

	PHeadSetB((LPBYTE)&this->m_DevilSquareScoreInfoTOP10, 0x93, iSize);
	count = 1;

	Itor = this->m_DevilSquareRankList.begin();

	LogAddTD("[DevilSquare] Rank [%d]", this->m_iIndex);

	for ( ; Itor != this->m_DevilSquareRankList.end(); Itor++ )
	{
		if ( iUserCount <= 6 )
		{
			if ( count < 4 )
			{
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusZen = this->m_Bonus[count-1][0];
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp = this->m_Bonus[count-1][1];
			}
			else
			{
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusZen = this->m_Bonus[3][0];
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp = this->m_Bonus[3][1];
			}
		}
		else
		{
			int lc7 = count*100/iUserCount;

			if ( count ==1 )
			{
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusZen = this->m_Bonus[0][0];
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp = this->m_Bonus[0][1];
			}
			else if ( lc7 <= 30 )
			{
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusZen = this->m_Bonus[1][0];
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp = this->m_Bonus[1][1];
			}
			else if ( lc7 <= 50 )
			{
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusZen = this->m_Bonus[2][0];
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp = this->m_Bonus[2][1];
			}
			else
			{
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusZen = this->m_Bonus[3][0];
				this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp = this->m_Bonus[3][1];
			}
		}

		if ( g_CrywolfSync.GetOccupationState() == 1 && g_iCrywolfApplyMvpPenalty != FALSE)
		{
			this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp =  (this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp * g_CrywolfSync.GetGettingExpPenaltyRate()) / 100;
		}

		__int64 iExp64 = this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp;

		CheckItemOptForGetExpEx((*(Itor)),iExp64,TRUE);
		(*(Itor))->Experience += (DWORD)iExp64;
		bool ret = gObjLevelUp((*(Itor)), &iExp64, 0, EVENT_TYPE_DEVILSQUARE);

		(*(Itor))->m_nEventMoney = this->m_DevilSquareScoreInfoTOP10.Score[0].BonusZen;

		if ( gObjCheckMaxZen((*(Itor))->m_Index, (*(Itor))->m_nEventMoney) == FALSE )
		{
			int Zen = MAX_ZEN - (*(Itor))->Money;
			(*(Itor))->Money += Zen;
		}
		else
		{
			(*(Itor))->Money += (*(Itor))->m_nEventMoney;
		}

		GCMoneySend((*(Itor))->m_Index, (*(Itor))->Money);

		if ( ret == true )
		{
			GCKillPlayerExpSend((*(Itor))->m_Index, (WORD)-1, this->m_DevilSquareScoreInfoTOP10.Score[0].BonusExp, 0, 0);
		}

		if( count == 1 )
		{
			g_EventItemBagManager.OpenSpecial(EventBagSpecial::DevilSquare1, (*(Itor))->m_Index, (*(Itor))->MapNumber, (*(Itor))->X, (*(Itor))->Y);
		}
		else if( count == 2)
		{
			g_EventItemBagManager.OpenSpecial(EventBagSpecial::DevilSquare2, (*(Itor))->m_Index, (*(Itor))->MapNumber, (*(Itor))->X, (*(Itor))->Y);
		}
		else if( count == 3)
		{
			g_EventItemBagManager.OpenSpecial(EventBagSpecial::DevilSquare3, (*(Itor))->m_Index, (*(Itor))->MapNumber, (*(Itor))->X, (*(Itor))->Y);
		}

		this->SendRankingInfo((*(Itor)));

		LogAddTD("Rank :[%d] : [%s][%s][%d][%d][%d]", count, (*(Itor))->AccountID, (*(Itor))->Name, (*(Itor))->m_nEventMoney, (*(Itor))->m_nEventExp, (*(Itor))->m_nEventScore);
		memcpy(this->m_DevilSquareScoreInfoTOP10.Score[0].Name , (*(Itor))->Name, MAX_ACCOUNT_LEN);
		this->m_DevilSquareScoreInfoTOP10.Score[0].TotalScore = (*(Itor))->m_nEventScore;
		this->m_DevilSquareScoreInfoTOP10.MyRank = count;
		count++;

		DataSend((*(Itor))->m_Index, (BYTE *)&this->m_DevilSquareScoreInfoTOP10 , iSize);

#ifdef POINTEX
		g_ShopPointEx.AddEventBonus((*(Itor))->m_Index, ShopPointExEvent::DS);
#endif

#if( __4GAMERS__ == 1 )
		g_Achievements.GD_UpdateEventDataDS((*(Itor)), (*(Itor))->m_nEventExp, (*(Itor))->m_nEventScore);
#endif

		(*(Itor))->m_nEventScore = 0;
		(*(Itor))->m_nEventMoney = 0;
		(*(Itor))->m_nEventExp = 0;
	}
}






void CDevilSquareGround::SendRankingInfo(LPOBJ lpObj)
{
	if ( lpObj->m_nEventScore <= 0 )
	{
		return;
	}

	PMSG_ANS_EVENTUSERSCORE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x01;
	pMsg.h.size = sizeof(pMsg);
	pMsg.Score = lpObj->m_nEventScore;
	pMsg.SquareNum = lpObj->m_bDevilSquareIndex;
	pMsg.Class = lpObj->Class;
	pMsg.ServerCode = gGameServerCode;
	memcpy(pMsg.AccountID, lpObj->AccountID, MAX_ACCOUNT_LEN);
	memcpy(pMsg.GameID, lpObj->Name, MAX_ACCOUNT_LEN);

	DataSendRank((char *)&pMsg, pMsg.h.size);
}

int CDevilSquareGround::GetPlayUserCountRightNow()
{
	int nPlayUserCount = 0;

	for(int i = 0; i < MAX_DEVILSQUARE_USER; i++)
	{
		if( this->m_UserData[i] != -1 )
		{
			int nUserIndex = this->m_UserData[i];

			if( OBJMAX_RANGE(nUserIndex) &&
				gObj[nUserIndex].Type == OBJ_USER &&
				gObj[nUserIndex].Connected == PLAYER_PLAYING )
			{
				nPlayUserCount++;
			}
		}
	}
	return nPlayUserCount;
}

int CDevilSquareGround::AddUser(int sUserIndex)
{
	EnterCriticalSection(&this->m_UserCrit);

	for(int i = 0; i < MAX_DEVILSQUARE_USER; i++)
	{
		if( this->m_UserData[i] == -1 )
		{
			this->m_UserData[i] = sUserIndex;
			LeaveCriticalSection(&this->m_UserCrit);
			return true;
		}
	}
	LeaveCriticalSection(&this->m_UserCrit);
	return false;
}

int CDevilSquareGround::DelUser(int sUserIndex)
{
	EnterCriticalSection(&this->m_UserCrit);

	for(int i = 0; i < MAX_DEVILSQUARE_USER; i++)
	{
		if( this->m_UserData[i] == sUserIndex )
		{
			this->m_UserData[i] = -1;
			LeaveCriticalSection(&this->m_UserCrit);
			return true;
		}
	}
	LeaveCriticalSection(&this->m_UserCrit);
	return false;
}

int CDevilSquareGround::ChangeUserIndex(int nExUserIndex,int nCurrentUserIndex)
{
	EnterCriticalSection(&this->m_UserCrit);

	for(int i = 0; i < MAX_DEVILSQUARE_USER; i++)
	{
		if( this->m_UserData[i] == nExUserIndex )
		{
			this->m_UserData[i] = nCurrentUserIndex;
			LeaveCriticalSection(&this->m_UserCrit);
			return true;
		}
	}
	LeaveCriticalSection(&this->m_UserCrit);
	return false;
}