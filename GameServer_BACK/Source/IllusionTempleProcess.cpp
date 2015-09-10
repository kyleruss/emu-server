#include "stdafx.h"
#include "IllusionTempleEvent.h"
#include "BloodCastle.h"
#include "ChaosCastle.h"
#include "GameMain.h"
#include "TNotice.h"
#include "logproc.h"
#include "..\common\winutil.h"
#include "BuffEffectSlot.h"
#include "MasterSkillSystem.h"
#include "DSProtocol.h"
#include "ClassCalc.h"
#include "EventItemBag.h"

// Member Functions

void CIllusionTempleProcess::Reset(int iTemple)
{
	if( this->Temple == 0xFF )
	{
		this->Temple = iTemple;
	}

	int i;
	for(i = 0; i < IT_MAXPLAYER; i++)
	{
		UserInit(i);
	}

	dwTICK_COUNT2 = GetTickCount();
	m_RELIC_PLAYER = -1;
	m_RELIC_POS = -1;
	m_RELIC_TEAM = -1;
	m_ALLIED_TEAM_COUNT = 0;
	m_ILLUSION_TEAM_COUNT = 0;
	m_ALLIED_TEAM_PTS = 0;
	m_ILLUSION_TEAM_PTS = 0;
	iNPC_TICK_COUNT = -1;
	m_WinTeam = -1;
	m_OpenState = 0;
	dwTICK_COUNT = -1;
	m_IT_END_MOVE = 0;
	m_IT_MSG_BEFORE_ENTER = 0;
	m_IT_MSG_BEFORE_START = 0;
	iUNK_SECONDS = 0;

	UserResetPoints();
	CheckSync();

	m_bIT_CHECKSYNC = FALSE;

	for(i = 0; i < IT_TEAMMAXPLAYER; i++)
	{
		m_ALLIED_TEAM[i] = 0;
		m_ILLUSION_TEAM[i] = 0;
	}

	m_IT_REWARD_SENT = 0;
}

void CIllusionTempleProcess::UserInit(int iUser)
{
	if( OBJMAX_RANGE( iUser ) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	UserMap[iUser] = 0;
	this->m_PlayerData[iUser].m_Skill1Time = 0;
	this->m_PlayerData[iUser].m_Skill2Time = 0;
	this->m_PlayerData[iUser].m_Index = -1;
	this->m_PlayerData[iUser].m_Team = -1;
	this->m_PlayerData[iUser].m_PlayerKillCount = 0;
	this->m_PlayerData[iUser].m_MonsterKillCount = 0;
	this->m_PlayerData[iUser].m_RelicSuccess = 0;
	this->m_PlayerData[iUser].m_UseSkillCnt = 0;
	this->m_PlayerData[iUser].m_RewardExp = 0;
	this->m_PlayerData[iUser].m_RankPoint = 0;
	this->m_PlayerData[iUser].m_bReward = 0;
}

void CIllusionTempleProcess::CheckSync()
{
	std::list<ILLUSION_TIME>::iterator Itor_;

	ILLUSION_TIME pFirstTime;
	ILLUSION_TIME pTime;

	DWORD FIRST_TIME = 0;
	DWORD CHECK_TIME = 0;
	DWORD NOW_TIME = 0;

	tm * today;
	time_t ltime;

	if( m_IllusionStartTime.size() == 0 )
	{
		MsgBox("Error : Illusion Temple StartTime size is 0");
		return;
	}

	time(&ltime);
	today = localtime(&ltime);

	NOW_TIME = today->tm_hour * 60 + today->tm_min;

	pFirstTime = *m_IllusionStartTime.begin();

	for( Itor_ = m_IllusionStartTime.begin() ; Itor_ != m_IllusionStartTime.end(); ++Itor_ )
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
		if( Itor_ == m_IllusionStartTime.end() )
		{
			Itor_ = m_IllusionStartTime.begin();
			pTime = *Itor_;
		}

		CHECK_TIME = pTime.iHour * 60 + pTime.iMinute;

		if( today->tm_hour <= pTime.iHour )
		{
			this->m_iRemainMinute = CHECK_TIME - NOW_TIME;
		}
		else
		{
			this->m_iRemainMinute = 1440 - NOW_TIME + CHECK_TIME;
		}

		if( this->m_iRemainMinute <= m_iIT_TIME_MIN_OPEN )
		{
			Itor_++;
			if( Itor_ != m_IllusionStartTime.end() )
			{
				pTime = *Itor_;
			}
		}
		else
		{
			break;
		}
	}

	DWORD dwTICK_COUNT = GetTickCount();
	dwTICK_COUNT3 = dwTICK_COUNT + ( 60000 - today->tm_sec * 1000 );
	LogAddTD("[Illusion Temple] (%d) Sync Open Time. [%d]min remain",Temple+1,m_iRemainMinute);
}

void CIllusionTempleProcess::Run(int iTICK_COUNT)
{
	BYTE State = this->State;

	switch( State )
	{
	case IT_STATE_CLOSED:
		ProcState_CLOSE(iTICK_COUNT);
		break;
	case IT_STATE_PREPARE:
		ProcState_PREPARE(iTICK_COUNT);
		break;
	case IT_STATE_PLAYING:
		ProcState_PLAY(iTICK_COUNT);
		break;
	case IT_STATE_PLAYEND:
		ProcState_PLAYEND(iTICK_COUNT);
		break;
	}
}

void CIllusionTempleProcess::ProcState_CLOSE(int TICK_COUNT)
{
	int iTICK_DIFF = TICK_COUNT - dwTICK_COUNT2;

	if( dwTICK_COUNT3 < TICK_COUNT )
	{
		m_iRemainMinute--;
		dwTICK_COUNT3 = TICK_COUNT + 60000;

		if( m_iRemainMinute <= m_iIT_TIME_MIN_OPEN &&
			m_iRemainMinute > 0 &&
			Temple == 0)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 0,lMsg.Get(MSGGET(13,66)),m_iRemainMinute);
			SendAllUserAnyMsg((BYTE *)&pNotice, pNotice.h.size);
		}
	}

	if( iTICK_DIFF >= 1000 )
	{
		if( dwTICK_COUNT > 0 )
		{
			dwTICK_COUNT -= iTICK_DIFF;
		}
		dwTICK_COUNT2 = TICK_COUNT;

		if( m_iRemainMinute <= (m_iIT_TIME_MIN_OPEN + 1) &&
			dwTICK_COUNT == -1)
		{
			dwTICK_COUNT = (m_iIT_TIME_MIN_OPEN + 1) * 60 * 1000;
		}

		if( m_iRemainMinute <= m_iIT_TIME_MIN_OPEN &&
			dwTICK_COUNT > 5000 &&
			m_OpenState == FALSE)
		{
			m_OpenState = TRUE;
		}

		if( dwTICK_COUNT <= 30000 &&
			dwTICK_COUNT > 0 &&
			Temple == 0 && 
			m_IT_MSG_BEFORE_ENTER == FALSE)
		{
			m_IT_MSG_BEFORE_ENTER = TRUE;

			PMSG_SET_DEVILSQUARE pMsg;
			PHeadSetB((LPBYTE)&pMsg, 0x92, sizeof(pMsg));
			pMsg.Type = 14;

			for(int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++)
			{
				if( gObj[i].Connected == PLAYER_PLAYING &&
					gObj[i].Type == OBJ_USER &&
					BC_MAP_RANGE( gObj[i].MapNumber ) == FALSE &&
					CC_MAP_RANGE( gObj[i].MapNumber ) == FALSE )
				{
					DataSend(i,(LPBYTE)&pMsg,pMsg.h.size);
				}
			}
		}

		if( dwTICK_COUNT <= 5000 && m_OpenState == TRUE)
		{
			m_OpenState = FALSE;
			if( Temple == 0)
			{
				PMSG_NOTICE pNotice;
				TNotice::MakeNoticeMsgEx(&pNotice, 0,lMsg.Get(MSGGET(13,67)));
				SendAllUserAnyMsg((BYTE *)&pNotice, pNotice.h.size);
			}
		}
	}

	if( m_iRemainMinute <= 0 &&
		dwTICK_COUNT <= 0 )
	{
		m_OpenState = FALSE;
		CheckUsers();

		if( UserCount < 2 )
		{
			for(int i = 0; i < IT_MAXPLAYER; i++)
			{
				if( m_PlayerData[i].m_Index != -1 &&
					gObj[ m_PlayerData[i].m_Index ].Connected > PLAYER_LOGGED &&
					gObj[ m_PlayerData[i].m_Index ].MapNumber == Temple + MAP_INDEX_ILLUSIONTEMPLE1 )
				{
					int Pos = m_PlayerData[i].m_TicketPos;

					if( Pos > -1 )
					{
						if( gObj[ m_PlayerData[i].m_Index ] .pInventory[Pos].m_Type == ITEMGET(13,51) )
						{
							gObj[ m_PlayerData[i].m_Index ] .pInventory[Pos].m_Durability = 1.0f;
							GCItemDurSend( gObj[ m_PlayerData[i].m_Index ].m_Index, Pos, (BYTE)gObj[ m_PlayerData[i].m_Index ] .pInventory[Pos].m_Durability, 0);
						}
						else if( gObj[ m_PlayerData[i].m_Index ] .pInventory[Pos].m_Type == ITEMGET(13,61) )
						{
							gObj[ m_PlayerData[i].m_Index ] .pInventory[Pos].m_Durability += 1.0f;
							GCItemDurSend( gObj[ m_PlayerData[i].m_Index ].m_Index, Pos, (BYTE)gObj[ m_PlayerData[i].m_Index ] .pInventory[Pos].m_Durability, 0);
						}

						LogAddTD("[Illusion Temple] (%d) Payback to User (%s)(%s) Ticket (Serial:%u)",
							Temple+1,gObj[ m_PlayerData[i].m_Index ].AccountID,gObj[ m_PlayerData[i].m_Index ].Name,gObj[ m_PlayerData[i].m_Index ].pInventory[Pos].m_Number );

					}
					else
					{
						LogAddTD("[Illusion Temple] (%d) Payback to User Failed (%s)(%s) Scroll Not Found",
							Temple+1,gObj[ m_PlayerData[i].m_Index ].AccountID,gObj[ m_PlayerData[i].m_Index ].Name );
					}

					gObjMoveGate(m_PlayerData[i].m_Index,267);
				}
			}

			LogAddTD("[Illusion Temple] (%d) Failed to Start Illusion Temple (UserCount: %d)",Temple+1,UserCount);
			SetState(IT_STATE_PLAYEND);
			return;
		}

		int Count = 0;
		int AlliedParty = -1;
		int IllusionParty = -1;

		char szAlliedTeam[256];
		char szIllusionTeam[256];

		sprintf(szAlliedTeam,"[Illusion Temple] (%d) [AlliedTeam] ",Temple+1);
		sprintf(szIllusionTeam,"[Illusion Temple] (%d) [IllusiondTeam] ",Temple+1);

		for(int i = 0; i < IT_MAXPLAYER; i++)
		{
			if( m_PlayerData[i].m_Index != -1 &&
				gObj[ m_PlayerData[i].m_Index].iTempleIndex == i )
			{
				if( gObj[m_PlayerData[i].m_Index].Class >= MAX_TYPE_PLAYER ||
					gObj[m_PlayerData[i].m_Index].Class < 0 )
				{
					continue;
				}

				int Pos = m_PlayerData[i].m_TicketPos;

				if( Pos > -1 )
				{
					if( gObj[m_PlayerData[i].m_Index].pInventory[Pos].m_Durability <= 0.0f )
					{
						if( gObj[m_PlayerData[i].m_Index].pInventory[Pos].m_Type == ITEMGET(13,51) )
						{
							gObjInventoryDeleteItem(m_PlayerData[i].m_Index, Pos);
							GCInventoryItemDeleteSend(m_PlayerData[i].m_Index,Pos,0);
						}
						else if( gObj[m_PlayerData[i].m_Index].pInventory[Pos].m_Type == ITEMGET(13,61) &&
								 gObj[m_PlayerData[i].m_Index].pInventory[Pos].m_Durability <= 0.0f)
						{
							gObjInventoryDeleteItem(m_PlayerData[i].m_Index, Pos);
							GCInventoryItemDeleteSend(m_PlayerData[i].m_Index,Pos,0);
						}
					}
				}

				if( m_ALLIED_TEAM[gObj[ m_PlayerData[i].m_Index].Class] == m_ILLUSION_TEAM[gObj[ m_PlayerData[i].m_Index].Class] )
				{
					if( m_ALLIED_TEAM_COUNT == m_ILLUSION_TEAM_COUNT )
					{
						m_PlayerData[i].m_Team = rand()%2;
					}
					else
					{
						if( m_ALLIED_TEAM_COUNT > m_ILLUSION_TEAM_COUNT )
						{
							m_PlayerData[i].m_Team = IT_ILLUSION_TEAM;
						}
						else
						{
							m_PlayerData[i].m_Team = IT_ALLIED_TEAM;
						}
					}
				}
				else
				{
					if( m_ALLIED_TEAM[gObj[ m_PlayerData[i].m_Index].Class] < m_ILLUSION_TEAM[gObj[ m_PlayerData[i].m_Index].Class] )
					{
						if( m_ALLIED_TEAM_COUNT > m_ILLUSION_TEAM_COUNT )
						{
							m_PlayerData[i].m_Team = IT_ILLUSION_TEAM;
						}
						else
						{
							m_PlayerData[i].m_Team = IT_ALLIED_TEAM;
						}
					}
					else
					{
						if( m_ALLIED_TEAM_COUNT < m_ILLUSION_TEAM_COUNT )
						{
							m_PlayerData[i].m_Team = IT_ALLIED_TEAM;
						}
						else
						{
							m_PlayerData[i].m_Team = IT_ILLUSION_TEAM;
						}
					}
				}

				if( m_PlayerData[i].m_Team == IT_ALLIED_TEAM )
				{
					m_PlayerData[i].m_Team = IT_ALLIED_TEAM;
					m_ALLIED_TEAM[gObj[ m_PlayerData[i].m_Index].Class]++;

					if( AlliedParty == -1 && gObj[m_PlayerData[i].m_Index].PartyNumber < 0 )
					{
						AlliedParty = gParty.Create(m_PlayerData[i].m_Index,gObj[m_PlayerData[i].m_Index].DBNumber,gObj[m_PlayerData[i].m_Index].Level);

						if( AlliedParty == -1 )
						{
							LogAddTD("[Illusion Temple] (%d) Party Create Error, (Team: %d)",Temple+1,m_PlayerData[i].m_Team);
						}
						gObj[m_PlayerData[i].m_Index].PartyNumber = AlliedParty;
					}
					else
					{
						int bResult = gParty.Add(AlliedParty,m_PlayerData[i].m_Index,gObj[m_PlayerData[i].m_Index].DBNumber,gObj[m_PlayerData[i].m_Index].Level);

						if( bResult >= 0 )
						{
							gObj[m_PlayerData[i].m_Index].PartyNumber = AlliedParty;
						}
						else
						{
							LogAddTD("[Illusion Temple] (%d) Party Add Error, (Team: %d)",Temple+1,m_PlayerData[i].m_Team);
						}
					}

					gObjMoveGate(m_PlayerData[i].m_Index,148 + Temple);
					gObj[m_PlayerData[i].m_Index].m_Change = 404;
					gObjViewportListProtocolCreate(&gObj[m_PlayerData[i].m_Index]);
					m_ALLIED_TEAM_COUNT++;
					Count++;

					int szLen = strlen(szAlliedTeam);
					sprintf(&szAlliedTeam[szLen],"(%s)(%s) ",gObj[m_PlayerData[i].m_Index].AccountID,gObj[m_PlayerData[i].m_Index].Name);
				}
				else
				{
					m_PlayerData[i].m_Team = IT_ILLUSION_TEAM;
					m_ILLUSION_TEAM[gObj[ m_PlayerData[i].m_Index].Class]++;

					if( IllusionParty == -1 && gObj[m_PlayerData[i].m_Index].PartyNumber < 0 )
					{
						IllusionParty = gParty.Create(m_PlayerData[i].m_Index,gObj[m_PlayerData[i].m_Index].DBNumber,gObj[m_PlayerData[i].m_Index].Level);

						if( IllusionParty == -1 )
						{
							LogAddTD("[Illusion Temple] (%d) Party Create Error, (Team: %d)",Temple+1,m_PlayerData[i].m_Team);
						}
						gObj[m_PlayerData[i].m_Index].PartyNumber = IllusionParty;
					}
					else
					{
						int bResult = gParty.Add(IllusionParty,m_PlayerData[i].m_Index,gObj[m_PlayerData[i].m_Index].DBNumber,gObj[m_PlayerData[i].m_Index].Level);

						if( bResult >= 0 )
						{
							gObj[m_PlayerData[i].m_Index].PartyNumber = IllusionParty;
						}
						else
						{
							LogAddTD("[Illusion Temple] (%d) Party Add Error, (Team: %d)",Temple+1,m_PlayerData[i].m_Team);
						}
					}

					gObjMoveGate(m_PlayerData[i].m_Index,154 + Temple);
					gObj[m_PlayerData[i].m_Index].m_Change = 405;
					gObjViewportListProtocolCreate(&gObj[m_PlayerData[i].m_Index]);
					m_ILLUSION_TEAM_COUNT++;
					Count++;

					int szLen = strlen(szIllusionTeam);
					sprintf(&szIllusionTeam[szLen],"(%s)(%s) ",gObj[m_PlayerData[i].m_Index].AccountID,gObj[m_PlayerData[i].m_Index].Name);
				}
			}
		}

		CGPartyListAll(AlliedParty);
		CGPartyListAll(IllusionParty);
		SetState(IT_STATE_PREPARE);

		LogAddTD("[Illusion Temple] (%d) Allied Team CLASS_DW(%d),CLASS_DK(%d), CLASS_FE(%d),CLASS_MG(%d), CLASS_DL(%d)",
			Temple+1,m_ALLIED_TEAM[0],m_ALLIED_TEAM[1],m_ALLIED_TEAM[2],m_ALLIED_TEAM[3],m_ALLIED_TEAM[4]);
		LogAddTD(szAlliedTeam);

		LogAddTD("[Illusion Temple] (%d) Illusion Team CLASS_DW(%d),CLASS_DK(%d), CLASS_FE(%d),CLASS_MG(%d), CLASS_DL(%d)",
			Temple+1,m_ILLUSION_TEAM[0],m_ILLUSION_TEAM[1],m_ILLUSION_TEAM[2],m_ILLUSION_TEAM[3],m_ILLUSION_TEAM[4]);
		LogAddTD(szIllusionTeam);
	}
}

void CIllusionTempleProcess::ProcState_PREPARE(int TICK_COUNT)
{
	int iTICK_COUNT = TICK_COUNT - dwTICK_COUNT2;

	if( iTICK_COUNT >= 1000 )
	{
		dwTICK_COUNT -= iTICK_COUNT;
		dwTICK_COUNT2 = TICK_COUNT;

		if( dwTICK_COUNT <= 30000 && dwTICK_COUNT > 0 && m_IT_MSG_BEFORE_START == FALSE )
		{
			m_IT_MSG_BEFORE_START = TRUE;

			PMSG_SET_DEVILSQUARE pMsg;
			PHeadSetB((LPBYTE)&pMsg, 0x92, sizeof(pMsg));
			pMsg.Type = 15;

			for(int i = 0; i < IT_MAXPLAYER; i++)
			{
				if( m_PlayerData[i].m_Index != -1 &&
					gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED &&
					gObj[m_PlayerData[i].m_Index].MapNumber == Temple+ MAP_INDEX_ILLUSIONTEMPLE1)
				{
					DataSend(m_PlayerData[i].m_Index,(LPBYTE)&pMsg,pMsg.h.size);
				}
			}
		}
	}

	if( UserCount == 0 )
	{
		SetState(IT_STATE_PLAYEND);
	}
	else if( dwTICK_COUNT <= 0 )
	{
		SetState(IT_STATE_PLAYING);
		OperateMiniMap(IT_ALLIED_TEAM);
		OperateMiniMap(IT_ILLUSION_TEAM);
	}
}

void CIllusionTempleProcess::ProcState_PLAY(int TICK_COUNT)
{
	int iTICK_COUNT = TICK_COUNT - dwTICK_COUNT2;

	if( iTICK_COUNT >= 1000 )
	{
		dwTICK_COUNT -= iTICK_COUNT;
		dwTICK_COUNT2 = TICK_COUNT;

		if( dwTICK_COUNT > 0 )
		{
			OperateMiniMap(IT_ALLIED_TEAM);
			OperateMiniMap(IT_ILLUSION_TEAM);
		}

		if( iNPC_TICK_COUNT != -1 )
		{
			if( iNPC_TICK_COUNT < TICK_COUNT )
			{
				RegetStatue();
			}
		}
	}

	if( UserCount == 0 )
	{
		SetState(IT_STATE_PLAYEND);
		return;
	}

	if( dwTICK_COUNT <= 0 ||
		m_ALLIED_TEAM_PTS >= 7 || 
		m_ILLUSION_TEAM_PTS >= 7 ||
		m_ALLIED_TEAM_COUNT == 0 ||
		m_ILLUSION_TEAM_COUNT == 0 )
	{
		if( m_ALLIED_TEAM_PTS >= 7 || m_ILLUSION_TEAM_PTS >= 7 )
		{
			if( dwTICK_COUNT <= 0 )
			{
				iUNK_SECONDS = 0;
			}
			else
			{
				iUNK_SECONDS = dwTICK_COUNT / 1000;
			}
		}

		OperateMiniMap(IT_ALLIED_TEAM);
		OperateMiniMap(IT_ILLUSION_TEAM);
		SetState(IT_STATE_PLAYEND);
	}
}

void CIllusionTempleProcess::ProcState_PLAYEND(int TICK_COUNT)
{
	int iTICK_COUNT = TICK_COUNT - dwTICK_COUNT2;

	if( iTICK_COUNT >= 1000 )
	{
		dwTICK_COUNT -= iTICK_COUNT;
		dwTICK_COUNT2 = TICK_COUNT;

		if( dwTICK_COUNT <= (m_iIT_TIME_MIN_CLOSE * 60 * 1000 - 3000) &&
			dwTICK_COUNT > 0 &&
			m_IT_END_MOVE == FALSE)
		{
			m_IT_END_MOVE = TRUE;

			for(int i = 0; i < IT_MAXPLAYER;i++)
			{
				if( m_PlayerData[i].m_Index != -1 &&
					gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED &&
					gObj[m_PlayerData[i].m_Index].MapNumber == Temple + MAP_INDEX_ILLUSIONTEMPLE1)
				{
					if( m_PlayerData[i].m_Team == IT_ALLIED_TEAM )
					{
						gObjMoveGate(m_PlayerData[i].m_Index,148+Temple);
					}
					else if( m_PlayerData[i].m_Team == IT_ILLUSION_TEAM )
					{
						gObjMoveGate(m_PlayerData[i].m_Index,154+Temple);
					}

					gObjSetPosition(m_PlayerData[i].m_Index,gObj[m_PlayerData[i].m_Index].X,gObj[m_PlayerData[i].m_Index].Y);
				}
			}

			if( m_RELIC_PLAYER >= 0 &&
				m_RELIC_POS >= 0 )
			{
				if( gObj[m_RELIC_PLAYER].pInventory[m_RELIC_POS].m_Type == ITEMGET(14,64) )
				{
					gObjInventoryDeleteItem(m_RELIC_PLAYER, m_RELIC_POS);
					GCInventoryItemDeleteSend(m_RELIC_PLAYER,m_RELIC_POS,0);
	
					m_RELIC_PLAYER = -1;
					m_RELIC_POS = -1;
					m_RELIC_TEAM = -1;
				}
			}

			CalcWinRewardExp();
		}
	}

	if( dwTICK_COUNT <= m_iIT_TIME_MIN_CLOSE * 60 * 1000 - 6000 &&
		dwTICK_COUNT > 0 &&
		m_IT_END_MOVE == TRUE &&
		m_IT_REWARD_SENT == FALSE )
	{
		SendRewardExp();
		m_IT_REWARD_SENT = TRUE;
	}

	if( dwTICK_COUNT <= 0 )
	{
		SetState(IT_STATE_CLOSED);
	}
}

void CIllusionTempleProcess::SetState(int iState)
{
	State = iState;

	switch(iState)
	{
	case IT_STATE_CLOSED:
		SetState_CLOSED();
		break;
	case IT_STATE_PREPARE:
		SetState_PREPARE();
		break;
	case IT_STATE_PLAYING:
		SetState_PLAY();
		break;
	case IT_STATE_PLAYEND:
		SetState_PLAYEND();
		break;
	}
}

BYTE CIllusionTempleProcess::GetState()
{
	return State;
}

void CIllusionTempleProcess::SetState_CLOSED()
{
	SendState(IT_STATE_CLOSED,-1);

	for(int i = 0; i < IT_MAXPLAYER;i++)
	{
		if( m_PlayerData[i].m_Index != -1 &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED &&
			gObj[m_PlayerData[i].m_Index].MapNumber == Temple + MAP_INDEX_ILLUSIONTEMPLE1)
		{
			gObjMoveGate(m_PlayerData[i].m_Index,267);
		}
	}

	Reset(Temple);
}

void CIllusionTempleProcess::SetState_PREPARE()
{
	dwTICK_COUNT = m_iIT_TIME_MIN_PREPARE * 60 * 1000;

	SendState(IT_STATE_PREPARE,-1);

	for(int i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Index != -1 &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED &&
			gObj[m_PlayerData[i].m_Index].MapNumber == Temple + MAP_INDEX_ILLUSIONTEMPLE1)
		{
			GCIllusionSendPoint(m_PlayerData[i].m_Index,10);
		}
	}
}

void CIllusionTempleProcess::SetState_PLAY()
{
	dwTICK_COUNT = m_iIT_TIME_MIN_PLAY * 60 * 1000;

	SpawnStatue();
	SpawnTeamNpc();
	SpawnMonster();
	SetStatueTime();

	SendState(IT_STATE_PLAYING,-1);
}

void CIllusionTempleProcess::SetState_PLAYEND()
{
	dwTICK_COUNT = m_iIT_TIME_MIN_CLOSE * 60 * 1000;

	SendState(IT_STATE_PLAYEND,-1);

	for(int i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Index != -1 &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED &&
			gObj[m_PlayerData[i].m_Index].MapNumber == Temple + MAP_INDEX_ILLUSIONTEMPLE1)
		{
			if( m_PlayerData[i].m_Skill1Time > 0 )
			{
				m_PlayerData[i].m_Skill1Time = 0;
				gObjRemoveBuffEffect(&gObj[m_PlayerData[i].m_Index],BUFF_IT_PROTECTION);
				GCSendUseSkill(&gObj[m_PlayerData[i].m_Index],0xD2);
			}

			if( m_PlayerData[i].m_Skill2Time > 0 )
			{
				m_PlayerData[i].m_Skill2Time = 0;
				gObjRemoveBuffEffect(&gObj[m_PlayerData[i].m_Index],BUFF_RESTRICTION);
				GCSendUseSkill(&gObj[m_PlayerData[i].m_Index],0xD3);
			}
		}
	}

	RemoveEventInfo();
}

int CIllusionTempleProcess::EnterUser(int aIndex, BYTE Temple, BYTE Pos)
{
	if( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return false;
	}

	if( GetState() >= IT_STATE_PREPARE )
	{
		return false;
	}

	if( gObj[aIndex].iTempleIndex != -1 )
	{
		LogAdd("Enter Error: IllusionTempleIndex Fail");
		return false;
	}

	int RelicPos = GetRelicPos(aIndex);

	if( RelicPos >= 0 )
	{
		if( gObj[aIndex].pInventory[RelicPos].m_Type == ITEMGET(14,64) )
		{
			gObjInventoryDeleteItem(aIndex,RelicPos);
			GCInventoryItemDeleteSend(aIndex,RelicPos,0);
		}
	}

	int bResult = FALSE;

	EnterCriticalSection(&this->m_CritSec);

	for(int i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Index == -1 )
		{
			m_PlayerData[i].m_Index = aIndex;
			UserCount++;
			gObj[aIndex].iTempleIndex = i;
			bResult = TRUE;
			m_PlayerData[i].m_TicketPos = Pos;
			break;
		}
	}

	LeaveCriticalSection(&this->m_CritSec);

return bResult;
}

int CIllusionTempleProcess::LeaveUser(int aIndex)
{
	if( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return false;
	}

	EnterCriticalSection(&this->m_CritSec);

	if( gObj[aIndex].iTempleIndex != -1 &&
		 aIndex == m_PlayerData[gObj[aIndex].iTempleIndex].m_Index )
	{
		IT_PLAYER_STRUCT* lpData = &m_PlayerData[gObj[aIndex].iTempleIndex];

		if( lpData->m_TicketPos > -1 )
		{
			CItem* lpItem = &gObj[aIndex].pInventory[lpData->m_TicketPos];

			if( lpItem != NULL && lpItem->IsItem() == TRUE && lpItem->m_Durability < 1.0f)
			{
				gObjInventoryDeleteItem(aIndex,lpData->m_TicketPos);
				GCInventoryItemDeleteSend(aIndex,lpData->m_TicketPos,0);
			}
		}

		if( m_PlayerData[gObj[aIndex].iTempleIndex].m_Skill1Time > 0 )
		{
			gObjRemoveBuffEffect(&gObj[aIndex],BUFF_IT_PROTECTION);
			GCSendUseSkill(&gObj[aIndex],0xD2);
		}

		if( m_PlayerData[gObj[aIndex].iTempleIndex].m_Skill2Time > 0 )
		{
			gObjRemoveBuffEffect(&gObj[aIndex],BUFF_RESTRICTION);
			GCSendUseSkill(&gObj[aIndex],0xD3);
		}

		if( m_PlayerData[gObj[aIndex].iTempleIndex].m_Team == IT_ALLIED_TEAM )
		{
			m_ALLIED_TEAM_COUNT--;
		}
		else if( m_PlayerData[gObj[aIndex].iTempleIndex].m_Team == IT_ILLUSION_TEAM )
		{
			m_ILLUSION_TEAM_COUNT--;
		}

		UserInit(gObj[aIndex].iTempleIndex);
		gObj[aIndex].iTempleIndex = -1;
		UserCount--;

		LogAddTD("[Illusion Temple] (%d) LeaveUser: (%s)(%s)",
			Temple+1,gObj[aIndex].AccountID,gObj[aIndex].Name);
	}

	LeaveCriticalSection(&this->m_CritSec);


	if( gObj[aIndex].PartyNumber >= 0 )
	{
		PMSG_PARTYDELUSER pMsg;
		int index = gParty.GetIndex(gObj[aIndex].PartyNumber,gObj[aIndex].m_Index,gObj[aIndex].DBNumber);

		if( index >= 0 )
		{
			pMsg.Number = index;

			CGPartyDelUser(&pMsg,gObj[aIndex].m_Index);
		}
	}

	gObj[aIndex].m_Change = -1;
	gObjViewportListProtocolCreate(&gObj[aIndex]);
	return true;
}

void CIllusionTempleProcess::CheckUsers()
{
	EnterCriticalSection(&this->m_CritSec);

	UserCount = 0;

	for(int i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Index != -1 )
		{
			if( gObjIsConnected( m_PlayerData[i].m_Index ) == FALSE )
			{
				gObj[ m_PlayerData[i].m_Index ].iTempleIndex = -1;
				UserInit(i);
			}
			else if( gObj[ m_PlayerData[i].m_Index ].MapNumber != Temple + MAP_INDEX_ILLUSIONTEMPLE1 )
			{
				gObj[ m_PlayerData[i].m_Index ].iTempleIndex = -1;
				UserInit(i);
			}
			else
			{
				UserCount++;
			}
		}
	}

	LeaveCriticalSection(&this->m_CritSec);
}

BYTE CIllusionTempleProcess::GetRemainTime()
{
	if( GetState() == IT_STATE_CLOSED && OpenState() == TRUE )
	{
		return m_iRemainMinute;
	}
return FALSE;
}

BYTE CIllusionTempleProcess::GetLeftTime()
{
	BYTE LeftTime = 0;

	if( GetState() == IT_STATE_CLOSED && OpenState() == TRUE )
	{
		LeftTime = 0;
	}
	else if( GetState() == IT_STATE_CLOSED && OpenState() == FALSE )
	{
		LeftTime = m_iRemainMinute - m_iIT_TIME_MIN_OPEN;
	}
	else 
	{
		std::list<ILLUSION_TIME>::iterator Itor_;

		ILLUSION_TIME pFirstTime;
		ILLUSION_TIME pTime;

		DWORD FIRST_TIME = 0;
		DWORD CHECK_TIME = 0;
		DWORD NOW_TIME = 0;

		tm * today;
		time_t ltime;
	
		if( m_IllusionStartTime.size() == 0 )
		{
			MsgBox("Error : Illusion Temple StartTime size is 0");
			return false;
		}
	
		time(&ltime);
		today = localtime(&ltime);
	
		NOW_TIME = today->tm_hour * 60 + today->tm_min;
	
		pFirstTime = *m_IllusionStartTime.begin();
	
		for( Itor_ = m_IllusionStartTime.begin() ; Itor_ != m_IllusionStartTime.end(); ++Itor_ )
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
			if( Itor_ == m_IllusionStartTime.end() )
			{
				Itor_ = m_IllusionStartTime.begin();
				pTime = *Itor_;
			}
	
			CHECK_TIME = pTime.iHour * 60 + pTime.iMinute;
	
			if( today->tm_hour <= pTime.iHour )
			{
				LeftTime = (BYTE)(CHECK_TIME - NOW_TIME);
			}
			else
			{
				LeftTime = (BYTE)(1440 - NOW_TIME + CHECK_TIME);
			}
	
			if( LeftTime <= m_iIT_TIME_MIN_OPEN )
			{
				Itor_++;
				if( Itor_ != m_IllusionStartTime.end() )
				{
					pTime = *Itor_;
				}
			}
			else
			{
				LeftTime -= m_iIT_TIME_MIN_OPEN;
				break;
			}
		}
		DWORD dwTICK_COUNT = GetTickCount();
		dwTICK_COUNT3 = dwTICK_COUNT + ( 60000 - today->tm_sec * 1000 );
	}
	
return LeftTime;
}

void CIllusionTempleProcess::ClickToStatus(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( lpNpc->m_State == 0 || lpNpc->m_PosNum == -1 )
	{
		return;
	}

	if( abs(lpObj->Y -lpNpc->Y) > 3 ||
		abs(lpObj->X -lpNpc->X) > 3 )
	{
		return;
	}

	lpNpc->m_PosNum = (short)-1;
	lpNpc->Life = 0.0f;

	gObjLifeCheck(lpNpc,lpObj,0,1,0,0,0,0);

	BYTE dur = 0;
	int type = 0;
	int level = 0;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;
	int unk1 = 0;
	int unk2 = 0; 
	int NewOption  =0;

	type = ItemGetNumberMake(14,64);
	Option1 = 0;
	Option2 = 0;
	Option3 = 0;
	level = 0;

	ItemSerialCreateSend(lpObj->m_Index,lpObj->MapNumber,lpNpc->X,lpNpc->Y,type,level,dur,Option1,Option2,Option3,lpObj->m_Index,NewOption,0);

	LogAddTD("[Illusion Temple] (%d) [%s][%s] Click To Status (X:%d, Y:%d)",
		Temple+1,lpObj->AccountID,lpObj->Name,lpNpc->X,lpNpc->Y);

	MonsterRegen();
}

void CIllusionTempleProcess::ClickAlliedNpc(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( m_RELIC_TEAM == (BYTE)-1 ||
		m_RELIC_TEAM != IT_ALLIED_TEAM ||
		lpObj->m_Index != m_RELIC_PLAYER )
	{
		return;
	}

	if( lpObj->pInventory[m_RELIC_POS].m_Type == ITEMGET(14,64) )
	{
		m_ALLIED_TEAM_PTS++;
		DeleteRelic(lpObj);
		m_RELIC_TEAM = (BYTE)-1;
		m_RELIC_PLAYER = -1;
		m_RELIC_POS = (BYTE)-1;

		AddSuccessRelic(lpObj->m_Index);

		LogAddTD("[Illusion Temple] (%d) Increase AlliedTeam Relics Point: (%s)(%s)(Score:%d)",
			Temple+1,lpObj->AccountID,lpObj->Name,m_ALLIED_TEAM_PTS);
	}

	SetStatueTime();
}

void CIllusionTempleProcess::ClickIllusionNpc(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( m_RELIC_TEAM == (BYTE)-1 ||
		m_RELIC_TEAM != IT_ILLUSION_TEAM ||
		lpObj->m_Index != m_RELIC_PLAYER )
	{
		return;
	}

	if( lpObj->pInventory[m_RELIC_POS].m_Type == ITEMGET(14,64) )
	{
		m_ILLUSION_TEAM_PTS++;
		DeleteRelic(lpObj);
		m_RELIC_TEAM = (BYTE)-1;
		m_RELIC_PLAYER = -1;
		m_RELIC_POS = (BYTE)-1;

		AddSuccessRelic(lpObj->m_Index);

		LogAddTD("[Illusion Temple] (%d) Increase IllusionTeam Relics Point: (%s)(%s)(Score:%d)",
			Temple+1,lpObj->AccountID,lpObj->Name,m_ILLUSION_TEAM_PTS);
	}

	SetStatueTime();
}

int CIllusionTempleProcess::GetRelicPos(int aIndex)
{
	if( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return -1;
	}

	for(int i = 0; i < INVENTORY_SIZE; i++)
	{
		if( gObj[aIndex].pInventory[i].IsItem() == TRUE &&
			gObj[aIndex].pInventory[i].m_Type == ITEMGET(14,64) )
		{
			return i;
		}
	}
return -1;
}

void CIllusionTempleProcess::DeleteRelic( LPOBJ lpObj )
{
	if( lpObj->pInventory[m_RELIC_POS].m_Type == ITEMGET(14,64) )
	{
		LogAddTD("[Illusion Temple] (%d) (%s)(%s) to Delete Relics Item [Serial:%u]",
			Temple+1,lpObj->AccountID,lpObj->Name,lpObj->pInventory[m_RELIC_POS].m_Number);

		gObjInventoryDeleteItem(lpObj->m_Index,m_RELIC_POS);
		GCInventoryItemDeleteSend(lpObj->m_Index,m_RELIC_POS,0);
	}
}

void CIllusionTempleProcess::DropRelic(int iIndex)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	if( m_RELIC_PLAYER != iIndex )
	{
		return;
	}

	LPOBJ lpObj = &gObj[iIndex];

	if( m_RELIC_POS == -1 )
	{
		return;
	}

	if( lpObj->pInventory[m_RELIC_POS].m_Type != ITEMGET(14,64) )
	{
		return;
	}

	int iItemPos = m_RELIC_POS;
	PMSG_ITEMTHROW_RESULT pResult;
	PHeadSetB((LPBYTE)&pResult, 0x23, sizeof(pResult));

	pResult.Result = TRUE;
	pResult.Ipos = iItemPos;

	int map_num = gObj[iIndex].MapNumber;
	int type = gObj[iIndex].pInventory[iItemPos].m_Type;
	int level = gObj[iIndex].pInventory[iItemPos].m_Level;
	float dur = gObj[iIndex].pInventory[iItemPos].m_Durability;
	BOOL ret = gObj[iIndex].pInventory[iItemPos].IsItem();
	BYTE Option1 = gObj[iIndex].pInventory[iItemPos].m_Option1;
	BYTE Option2 = gObj[iIndex].pInventory[iItemPos].m_Option2;
	BYTE Option3 = gObj[iIndex].pInventory[iItemPos].m_Option3;
	BYTE NOption = gObj[iIndex].pInventory[iItemPos].m_NewOption;
	DWORD s_num = gObj[iIndex].pInventory[iItemPos].m_Number;
	BYTE ItemExOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(&gObj[iIndex].pInventory[iItemPos]);
	BYTE ItemExLevel = g_kJewelOfHarmonySystem.GetItemOptionLevel(&gObj[iIndex].pInventory[iItemPos]);

	BYTE NewOption[MAX_EXOPTION_SIZE];
	::ItemIsBufExOption(NewOption, &gObj[iIndex].pInventory[iItemPos]);
	int PetLevel = gObj[iIndex].pInventory[iItemPos].m_PetItem_Level;
	int PetExp = gObj[iIndex].pInventory[iItemPos].m_PetItem_Exp;
	BYTE SOption = gObj[iIndex].pInventory[iItemPos].m_SetOption;
	BYTE ItemEffectEx = gObj[iIndex].pInventory[iItemPos].m_ItemOptionEx;
	int item_number = gObj[iIndex].pInventory[iItemPos].m_Number;
	char szItemName[50] = "Item";
	int aAntiLootIndex = -1;

	BYTE SocketOption[MAX_SOCKET_COUNT];
	SocketOption[0] = 0xFF;
	SocketOption[1] = 0xFF;
	SocketOption[2] = 0xFF;
	SocketOption[3] = 0xFF;
	SocketOption[4] = 0xFF;
	BYTE SocketBonus = 0;

	g_SocketItem.GetItemOptions(&gObj[iIndex].pInventory[iItemPos],&SocketOption[0],&SocketBonus);

	short x = 0;
	short y = 0;

	BYTE attr = MapC[map_num].GetAttr(gObj[iIndex].X,gObj[iIndex].Y);

	if( (attr & 4) == 4 || (attr&8) == 8 )
	{
		x = 168;
		y = 85;
	}
	else
	{
		x = gObj[iIndex].X;
		y = gObj[iIndex].Y;
	}

	if ( MapC[map_num].ItemDrop(type, level, dur, x, y,Option1, Option2, Option3, NOption, SOption, item_number, aAntiLootIndex, PetLevel, PetExp, ItemEffectEx,SocketOption,SocketBonus) == TRUE )
	{
		LogAddTD("[Illusion Temple] (%d) (%s)(%s) to Drop Relics Item [Serial:%u]",
			Temple+1,lpObj->AccountID,lpObj->Name,lpObj->pInventory[m_RELIC_POS].m_Number);

		::gObjInventoryDeleteItem(iIndex, iItemPos);
		pResult.Result = TRUE;

		LogAddTD(lMsg.Get(MSGGET(1, 223)), gObj[iIndex].AccountID, gObj[iIndex].Name, gObj[iIndex].MapNumber,
			gObj[iIndex].X, gObj[iIndex].Y, s_num, szItemName, type, level, Option1, Option2, Option3, (int)dur, NewOption[0],
			NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6], SOption,
			ItemEffectEx>>7, ItemExOption, ItemExLevel);
	}
	else
	{
		pResult.Result = FALSE;
	}

	m_RELIC_PLAYER = -1;
	m_RELIC_POS = (BYTE)-1;
	m_RELIC_TEAM = (BYTE)-1;

	DataSend(iIndex,(LPBYTE)&pResult,pResult.h.size);
}

void CIllusionTempleProcess::PickUpRelic(int aIndex,BYTE Pos)
{
	if( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	m_RELIC_PLAYER = aIndex;
	m_RELIC_POS = Pos;

	if( gObj[aIndex].iTempleIndex != -1 &&
		m_PlayerData[gObj[aIndex].iTempleIndex].m_Index != -1 &&
		m_PlayerData[gObj[aIndex].iTempleIndex].m_Index == aIndex)
	{
		m_RELIC_TEAM = m_PlayerData[gObj[aIndex].iTempleIndex].m_Team;
	}
}

struct PMSG_ILLUSIONTEMPLE_PICKRELICSEND
{
	PBMSG_HEAD2 h;
	WORD Index;
	char Name[10];
};

void CIllusionTempleProcess::PickUpRelicSend(LPOBJ lpObj)
{
	PMSG_ILLUSIONTEMPLE_PICKRELICSEND pMsg;

	PHeadSubSetB((LPBYTE)&pMsg,0xBF,8,sizeof(pMsg));
	pMsg.Index = lpObj->m_Index;
	memcpy(pMsg.Name,lpObj->Name,10);

	for(int i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Index != -1 &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED )
		{
			DataSend(m_PlayerData[i].m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		}
	}
}

void CIllusionTempleProcess::AddNpc(int Class,int Number)
{
	if( Class == 380 )
	{
		if( m_STATUE_NPC_COUNT > 2 )
		{
			return;
		}

		m_STATUE_NPC[m_STATUE_NPC_COUNT].m_Index = -1;
		m_STATUE_NPC[m_STATUE_NPC_COUNT].m_Num = Number;
		m_STATUE_NPC_COUNT++;
	}
	else if( Class == 383 )
	{
		m_ALLIED_NPC.m_Index = -1;
		m_ALLIED_NPC.m_Num = Number;
	}
	else if( Class == 384 )
	{
		m_ILLUSION_NPC.m_Index = -1;
		m_ILLUSION_NPC.m_Num = Number;
	}
}

void CIllusionTempleProcess::AddMonster(int Class,int Number)
{
	if( m_NPC_COUNT > IT_MAXMONSTER )
	{
		return;
	}

	m_MONSTER_NPC[m_NPC_COUNT].m_Index = -1;
	m_MONSTER_NPC[m_NPC_COUNT].m_Num = Number;
	m_NPC_COUNT++;
}

void CIllusionTempleProcess::ResetObject()
{
	int i;
	for(i = 0; i < IT_MAXSTATUE;i++)
	{
		m_STATUE_NPC[i].m_Index = -1;
		m_STATUE_NPC[i].m_Num = (WORD)-1;
	}

	m_STATUE_NPC_COUNT = 0;
	m_ALLIED_NPC.m_Index = -1;
	m_ALLIED_NPC.m_Num = (WORD)-1;
	m_ILLUSION_NPC.m_Index = -1;
	m_ILLUSION_NPC.m_Num = (WORD)-1;

	for(i = 0; i < IT_MAXMONSTER; i++)
	{
		m_MONSTER_NPC[i].m_Index = -1;
		m_MONSTER_NPC[i].m_Num = (WORD)-1;
	}
}

void CIllusionTempleProcess::SpawnStatue()
{
	for(int i = 0; i < m_STATUE_NPC_COUNT; i++)
	{
		int result = gObjAddMonster(Temple+ MAP_INDEX_ILLUSIONTEMPLE1);

		if( result >= 0 )
		{
			gObjSetPosMonster(result,m_STATUE_NPC[i].m_Num);
			gObjSetMonster(result,380);

			gObj[result].m_PosNum = (WORD)-1;
			gObj[result].Live = FALSE;
			gObj[result].DieRegen = 2;
			gObj[result].m_State = 4;
			gObj[result].RegenTime = GetTickCount();

			m_STATUE_NPC[i].m_Index = result;

			LogAddTD("[Illusion Temple] (%d) Create NPC Statue Name: %s, (%d: %d/%d)",
				Temple+1,gObj[result].Name,gObj[result].MapNumber,gObj[result].X,gObj[result].Y);
		}
		else
		{
			LogAddTD("[Illusion Temple] (%d) Add Statue Error",Temple+1);
		}
	}
}

void CIllusionTempleProcess::SpawnTeamNpc()
{
	int result = gObjAddMonster(Temple+MAP_INDEX_ILLUSIONTEMPLE1);

	if( result >= 0 )
	{
		gObjSetPosMonster(result,m_ALLIED_NPC.m_Num);
		gObjSetMonster(result,383);

		gObj[result].Dir = 1;
		gObj[result].m_PosNum = (short)-1;
		gObj[result].Live = TRUE;
		gObj[result].DieRegen = FALSE;
		gObj[result].m_State = 1;

		m_ALLIED_NPC.m_Index = result;


		LogAddTD("[Illusion Temple] (%d) Create NPC AlliedRelicsBox Name: %s, (%d: %d/%d)",
			Temple+1,gObj[result].Name,gObj[result].MapNumber,gObj[result].X,gObj[result].Y);
	}
	else
	{
		LogAddTD("[Illusion Temple] (%d) Add AlliedRelicsBox Error",Temple+1);
	}

	result = gObjAddMonster(Temple+MAP_INDEX_ILLUSIONTEMPLE1);

	if( result >= 0 )
	{
		gObjSetPosMonster(result,m_ILLUSION_NPC.m_Num);
		gObjSetMonster(result,384);

		gObj[result].Dir = 1;
		gObj[result].m_PosNum = (short)-1;
		gObj[result].Live = TRUE;
		gObj[result].DieRegen = FALSE;
		gObj[result].m_State = 1;

		m_ILLUSION_NPC.m_Index = result;


		LogAddTD("[Illusion Temple] (%d) Create NPC IllusionRelicsBox Name: %s, (%d: %d/%d)",
			Temple+1,gObj[result].Name,gObj[result].MapNumber,gObj[result].X,gObj[result].Y);
	}
	else
	{
		LogAddTD("[Illusion Temple] (%d) Add IllusionRelicsBox Error",Temple+1);
	}
}

void CIllusionTempleProcess::SpawnMonster()
{
	for(int i = 0; i < m_NPC_COUNT; i++)
	{
		int result = gObjAddMonster(Temple+MAP_INDEX_ILLUSIONTEMPLE1);

		if( result >= 0 )
		{
			gObjSetPosMonster(result,m_MONSTER_NPC[i].m_Num);
			gObjSetMonster(result,gMSetBase.m_Mp[m_MONSTER_NPC[i].m_Num].m_Type);

			gObj[result].Dir = 1;
			gObj[result].m_PosNum = (short)-1;
			gObj[result].Live = 0;
			gObj[result].DieRegen = 0;
			gObj[result].m_State = 4;
			gObj[result].RegenTime = GetTickCount();

			m_MONSTER_NPC[i].m_Index = result;


		}
		else
		{
			LogAddTD("[Illusion Temple] (%d) Add Monster Error",Temple+1);
		}
	}
}

void CIllusionTempleProcess::RemoveEventInfo()
{
	int i;
	for(i = 0; i < m_STATUE_NPC_COUNT; i++)
	{
		if( m_STATUE_NPC[i].m_Index != -1 )
		{
			LogAddTD("[Illusion Temple] (%d) RemoveStatus Name: %s, (%d: %d/%d)",
				Temple+1,gObj[m_STATUE_NPC[i].m_Index].Name,gObj[m_STATUE_NPC[i].m_Index].MapNumber,
				gObj[m_STATUE_NPC[i].m_Index].X,gObj[m_STATUE_NPC[i].m_Index].Y);

			gObjDel(m_STATUE_NPC[i].m_Index);
			m_STATUE_NPC[i].m_Index = -1;
		}
	}

	if( m_ALLIED_NPC.m_Index != -1 )
	{
		LogAddTD("[Illusion Temple] (%d) RemoveAlliedRelicsBox Name: %s, (%d: %d/%d)",
			Temple+1,gObj[m_ALLIED_NPC.m_Index].Name,gObj[m_ALLIED_NPC.m_Index].MapNumber,
			gObj[m_ALLIED_NPC.m_Index].X,gObj[m_ALLIED_NPC.m_Index].Y);
		gObjDel(m_ALLIED_NPC.m_Index);
		m_ALLIED_NPC.m_Index = -1;
	}

	if( m_ILLUSION_NPC.m_Index != -1 )
	{
		LogAddTD("[Illusion Temple] (%d) RemoveIllusionRelicsBox Name: %s, (%d: %d/%d)",
			Temple+1,gObj[m_ILLUSION_NPC.m_Index].Name,gObj[m_ILLUSION_NPC.m_Index].MapNumber,
			gObj[m_ILLUSION_NPC.m_Index].X,gObj[m_ILLUSION_NPC.m_Index].Y);
		gObjDel(m_ILLUSION_NPC.m_Index);
		m_ILLUSION_NPC.m_Index = -1;
	}

	for(i = 0; i < m_NPC_COUNT; i++)
	{
		if( m_MONSTER_NPC[i].m_Index != -1 )
		{
			gObjDel(m_MONSTER_NPC[i].m_Index);
			m_MONSTER_NPC[i].m_Index = -1;
		}
	}

	LogAddTD("[Illusion Temple] (%d) Remove All Object",Temple+1);
}

void CIllusionTempleProcess::SetStatueTime()
{
	iNPC_TICK_COUNT = GetTickCount() + 5000;
}

void CIllusionTempleProcess::RegetStatue()
{
	if( m_RELIC_PLAYER >= 0 )
		return;

	if( m_STATUE_NPC_COUNT <= 0 )
	{
		LogAddTD("[Illusion Temple] RegenStatue Failed m_STATUE_NPC_COUNT <= 0");
		return;
	}

	int StatuePos = rand()%m_STATUE_NPC_COUNT;

	if( m_STATUE_NPC[StatuePos].m_Index == -1 )
		return;

	LPOBJ lpObj = &gObj[m_STATUE_NPC[StatuePos].m_Index];
	lpObj->m_PosNum = m_STATUE_NPC[StatuePos].m_Num;
	lpObj->DieRegen = 2;
	iNPC_TICK_COUNT = -1;

	LogAddTD("[Illusion Temple] (%d) RegenStatus Name: %s, (%d: %d/%d), PosNum(%d), connected:%d",
		Temple+1,lpObj->Name,lpObj->MapNumber,lpObj->X,lpObj->Y,lpObj->m_PosNum,lpObj->Connected);
}

void CIllusionTempleProcess::MonsterRegen()
{
	for(int i = 0; i < IT_MAXMONSTER; i++)
	{
		if( m_MONSTER_NPC[i].m_Index != -1 &&
			gObj[m_MONSTER_NPC[i].m_Index].m_PosNum == -1 )
		{
			gObj[m_MONSTER_NPC[i].m_Index].m_PosNum = m_MONSTER_NPC[i].m_Num;
			gObj[m_MONSTER_NPC[i].m_Index].DieRegen = 2;
		}
	}
}

struct ILLUSIONTEMPLE_PLAYERINFO
{
	short	aIndex;
	BYTE	MapNumber;
	BYTE	X;
	BYTE	Y;
};

struct PMSG_ILLUSIONTEMPLE_INFO
{
	PBMSG_HEAD2	h;
	short Timer;
	short hIndex;
	BYTE	hX;
	BYTE	hY;

	BYTE	AlliedPts;
	BYTE	IllusionPts;
	BYTE	Team;
	BYTE	pCount;
};

void CIllusionTempleProcess::OperateMiniMap(BYTE TEAM)
{
	int lOfs = 0;
	BYTE SENDBUFF[256];
	PMSG_ILLUSIONTEMPLE_INFO pMsg;

	if( dwTICK_COUNT < 0 )
	{
		dwTICK_COUNT = 0;
	}

	pMsg.Timer = dwTICK_COUNT / 1000;

	if( m_RELIC_PLAYER != -1 )
	{
		pMsg.hIndex = m_RELIC_PLAYER;
		pMsg.hX = gObj[m_RELIC_PLAYER].X;
		pMsg.hY = gObj[m_RELIC_PLAYER].Y;
	}
	else
	{
		pMsg.hIndex = -1;
		pMsg.hX = -1;
		pMsg.hY = -1;
	}

	pMsg.AlliedPts = m_ALLIED_TEAM_PTS;
	pMsg.IllusionPts = m_ILLUSION_TEAM_PTS;
	pMsg.Team = TEAM;

	lOfs += sizeof(pMsg);
	int count = 0;

	int i;
	for(i = 0; i < IT_MAXPLAYER;i++)
	{
		ILLUSIONTEMPLE_PLAYERINFO pPlayerInfo;

		if( m_PlayerData[i].m_Index != -1 &&
			m_PlayerData[i].m_Team == TEAM &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED )
		{
			pPlayerInfo.aIndex = m_PlayerData[i].m_Index;
			pPlayerInfo.MapNumber = gObj[m_PlayerData[i].m_Index].MapNumber;
			pPlayerInfo.X = gObj[m_PlayerData[i].m_Index].X;
			pPlayerInfo.Y = gObj[m_PlayerData[i].m_Index].Y;
			count++;

			memcpy(&SENDBUFF[lOfs],&pPlayerInfo,sizeof(pPlayerInfo));
			lOfs += sizeof(pPlayerInfo);
		}
	}

	if( TEAM == IT_ALLIED_TEAM )
	{
		if( m_ALLIED_TEAM_COUNT != count )
		{
			m_ALLIED_TEAM_COUNT = count;
		}
		pMsg.pCount = m_ALLIED_TEAM_COUNT;
	}
	else
	{
		if( m_ILLUSION_TEAM_COUNT != count )
		{
			m_ILLUSION_TEAM_COUNT = count;
		}
		pMsg.pCount = m_ILLUSION_TEAM_COUNT;
	}

	PHeadSubSetB((LPBYTE)&pMsg,0xBF,1,lOfs);
	memcpy(&SENDBUFF[0],&pMsg,sizeof(pMsg));

	for( i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Index != -1 &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED &&
			m_PlayerData[i].m_Team == TEAM )
		{
			DataSend(m_PlayerData[i].m_Index,(LPBYTE)&SENDBUFF[0],pMsg.h.size);
		}
	}
}

void CIllusionTempleProcess::SendState(BYTE btState,int aIndex)
{
	PMSG_ILLUSIONTEMPLE_STATESEND pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,0x09,sizeof(pMsg));
	pMsg.Temple = Temple;
	pMsg.State = btState;

	if( aIndex != -1 )
	{
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	for(int i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Index != -1 &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED )
		{
			DataSend(m_PlayerData[i].m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		}
	}
}

BYTE CIllusionTempleProcess::GetTeam(int aIndex)
{
	if( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return -1;
	}

	if( gObj[aIndex].iTempleIndex != -1 &&
		(m_PlayerData[gObj[aIndex].iTempleIndex].m_Index == aIndex ||
		gObj[aIndex].Connected > PLAYER_LOGGED ))
	{
		return m_PlayerData[gObj[aIndex].iTempleIndex].m_Team;
	}
	return -1;
}

BYTE CIllusionTempleProcess::AddKillPts(int iIndex,BYTE Pts)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return false;
	}

	if( gObj[iIndex].iTempleIndex != -1 &&
		( m_PlayerData[gObj[iIndex].iTempleIndex].m_Index == iIndex || gObj[iIndex].Connected > PLAYER_LOGGED ) )
	{
		m_PlayerData[gObj[iIndex].iTempleIndex].m_SkillPts += Pts;
		if( m_PlayerData[gObj[iIndex].iTempleIndex].m_SkillPts > 90 )
		{
			m_PlayerData[gObj[iIndex].iTempleIndex].m_SkillPts = 90;
		}
		return m_PlayerData[gObj[iIndex].iTempleIndex].m_SkillPts;
	}
return FALSE;
}

BYTE CIllusionTempleProcess::GetKillPts(int iIndex)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return false;
	}

	if( gObj[iIndex].iTempleIndex != -1 &&
		( m_PlayerData[gObj[iIndex].iTempleIndex].m_Index == iIndex || gObj[iIndex].Connected > PLAYER_LOGGED ) )
	{
		return m_PlayerData[gObj[iIndex].iTempleIndex].m_SkillPts;
	}
return false;
}

int CIllusionTempleProcess::SetKillPts(int iIndex, BYTE Pts)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return false;
	}

	if( gObj[iIndex].iTempleIndex != -1 &&
		( m_PlayerData[gObj[iIndex].iTempleIndex].m_Index == iIndex || gObj[iIndex].Connected > PLAYER_LOGGED ) )
	{
		m_PlayerData[gObj[iIndex].iTempleIndex].m_SkillPts = Pts;
		return true;
	}
return false;
}

void CIllusionTempleProcess::UserResetPoints()
{
	for(int i = 0; i < IT_MAXPLAYER; i++)
	{
		m_PlayerData[i].m_SkillPts = 10;
	}
}

void CIllusionTempleProcess::AddPlayerKillCount(int iIndex)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	if( gObj[iIndex].iTempleIndex != -1 &&
		( m_PlayerData[gObj[iIndex].iTempleIndex].m_Index == iIndex || gObj[iIndex].Connected > PLAYER_LOGGED ) )
	{
		if( m_PlayerData[gObj[iIndex].iTempleIndex].m_PlayerKillCount >= 255 )
		{
			return;
		}

		m_PlayerData[gObj[iIndex].iTempleIndex].m_PlayerKillCount++;
	}
}

void CIllusionTempleProcess::AddMonsterKillCount(int iIndex)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	if( gObj[iIndex].iTempleIndex != -1 &&
		( m_PlayerData[gObj[iIndex].iTempleIndex].m_Index == iIndex || gObj[iIndex].Connected > PLAYER_LOGGED ) )
	{
		if( m_PlayerData[gObj[iIndex].iTempleIndex].m_MonsterKillCount >= 255 )
		{
			return;
		}

		m_PlayerData[gObj[iIndex].iTempleIndex].m_MonsterKillCount++;
	}
}

void CIllusionTempleProcess::AddSuccessRelic(int iIndex)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	if( gObj[iIndex].iTempleIndex != -1 &&
		( m_PlayerData[gObj[iIndex].iTempleIndex].m_Index == iIndex || gObj[iIndex].Connected > PLAYER_LOGGED ) )
	{
		if( m_PlayerData[gObj[iIndex].iTempleIndex].m_RelicSuccess >= 255 )
		{
			return;
		}

		m_PlayerData[gObj[iIndex].iTempleIndex].m_RelicSuccess++;
	}
}

void CIllusionTempleProcess::AddUseSkillCnt(int iIndex)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	if( gObj[iIndex].iTempleIndex != -1 &&
		( m_PlayerData[gObj[iIndex].iTempleIndex].m_Index == iIndex || gObj[iIndex].Connected > PLAYER_LOGGED ) )
	{
		if( m_PlayerData[gObj[iIndex].iTempleIndex].m_UseSkillCnt >= 255 )
		{
			return;
		}

		m_PlayerData[gObj[iIndex].iTempleIndex].m_UseSkillCnt++;
	}
}

void CIllusionTempleProcess::UseSkill(int iIndex,WORD skill,int iTargetIndex,BYTE State)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	if( OBJMAX_RANGE(iTargetIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[iIndex];
	LPOBJ lpTargetObj = &gObj[iTargetIndex];

	if( GetState() != IT_STATE_PLAYING )
	{
		GCSendUseSkill(lpObj->m_Index,lpTargetObj->m_Index,skill,FALSE);
		return;
	}

	if( lpObj->Connected <= PLAYER_LOGGED ||
		lpTargetObj->Connected <= PLAYER_LOGGED )
	{
		GCSendUseSkill(lpObj->m_Index,lpTargetObj->m_Index,skill,FALSE);
		return;
	}

	if( lpObj->MapNumber != lpTargetObj->MapNumber )
	{
		GCSendUseSkill(lpObj->m_Index,lpTargetObj->m_Index,skill,FALSE);
		return;
	}

	if( lpObj->iTempleIndex == -1 ||
		lpTargetObj->iTempleIndex == -1 )
	{
		GCSendUseSkill(lpObj->m_Index,lpTargetObj->m_Index,skill,FALSE);
		return;
	}

	int bCheck = -1;
	int bSuccess = TRUE;

	switch( skill )
	{
	case 0xD2:
		bCheck = MagicDamageC.CheckKillCount(0xD2,this->GetKillPts(lpObj->m_Index));

		if( bCheck < 0 )
		{
			bSuccess = FALSE;
			LogAddTD("[Illusion Temple] (%d) Use Skill KillCount Error (Skill:%d)",
				Temple+1,skill);
			break;
		}
		else
		{
			bSuccess = SetProtect(lpObj);
		}
		break;
	case 0xD3:
		bCheck = MagicDamageC.CheckKillCount(0xD3,this->GetKillPts(lpObj->m_Index));

		if( bCheck < 0 )
		{
			bSuccess = FALSE;
			LogAddTD("[Illusion Temple] (%d) Use Skill KillCount Error (Skill:%d)",
				Temple+1,skill);
			break;
		}
		else
		{
			bSuccess = SetImobilize(lpObj,lpTargetObj);
		}
		break;
	case 0xD4:
		bCheck = MagicDamageC.CheckKillCount(0xD4,this->GetKillPts(lpObj->m_Index));

		if( bCheck < 0 )
		{
			bSuccess = FALSE;
			LogAddTD("[Illusion Temple] (%d) Use Skill KillCount Error (Skill:%d)",
				Temple+1,skill);
			break;
		}
		else
		{
			bSuccess = SetTeleport(lpObj);
		}
		break;
	case 0xD5:
		bCheck = MagicDamageC.CheckKillCount(0xD5,this->GetKillPts(lpObj->m_Index));

		if( bCheck < 0 )
		{
			bSuccess = FALSE;
			LogAddTD("[Illusion Temple] (%d) Use Skill KillCount Error (Skill:%d)",
				Temple+1,skill);
			break;
		}
		else
		{
			bSuccess = SetShield(lpObj,lpTargetObj);
		}
		break;
	default:
		bSuccess = FALSE;
		break;
	}

	if( bSuccess == TRUE )
	{
		int bSetPts = SetKillPts(iIndex,bCheck);
		GCIllusionSendPoint(iIndex,bCheck);
		AddUseSkillCnt(iIndex);

		LogAddTD("[Illusion Temple] (%d) Use Skill (%d), (%s)(%s), (%d)",
			Temple+1,skill,lpObj->AccountID,lpObj->Name,bCheck);
	}

	if( skill != 0xD4 && bSuccess == TRUE )
	{
		GCSendUseSkill(lpObj->m_Index,lpTargetObj->m_Index,skill,TRUE);
	}
	else if( bSuccess == FALSE )
	{
		GCSendUseSkill(lpObj->m_Index,lpTargetObj->m_Index,skill,FALSE);
	}
}

void CIllusionTempleProcess::SkillProc(LPOBJ lpObj)
{
	if( m_PlayerData[lpObj->iTempleIndex].m_Index != lpObj->m_Index )
		return;

	if( m_PlayerData[lpObj->iTempleIndex].m_Skill1Time > 0 )
	{
		m_PlayerData[lpObj->iTempleIndex].m_Skill1Time--;

		if( m_PlayerData[lpObj->iTempleIndex].m_Skill1Time <= 0 )
		{
			m_PlayerData[lpObj->iTempleIndex].m_Skill1Time = 0;
			gObjRemoveBuffEffect(lpObj,BUFF_IT_PROTECTION);
			GCSendUseSkill(lpObj,0xD2);
		}
	}

	if( m_PlayerData[lpObj->iTempleIndex].m_Skill2Time > 0 )
	{
		m_PlayerData[lpObj->iTempleIndex].m_Skill2Time--;

		if( m_PlayerData[lpObj->iTempleIndex].m_Skill2Time <= 0 )
		{
			m_PlayerData[lpObj->iTempleIndex].m_Skill2Time = 0;
			gObjRemoveBuffEffect(lpObj,BUFF_RESTRICTION);
			GCSendUseSkill(lpObj,0xD3);
		}
	}
}

struct PMSG_ILLUSIONTEMPLE_USESKILLEND
{
	PBMSG_HEAD2 h;
	BYTE MagicH;
	BYTE MagicL;
	WORD Index;
};

void CIllusionTempleProcess::GCSendUseSkill(LPOBJ lpObj,int skill)
{
	PMSG_ILLUSIONTEMPLE_USESKILLEND pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,0x07,sizeof(pMsg));

	pMsg.MagicH = SET_NUMBERH(skill);
	pMsg.MagicL = SET_NUMBERL(skill);
	pMsg.Index = lpObj->m_Index;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	MsgSendV2(lpObj,(LPBYTE)&pMsg,pMsg.h.size);
}

int CIllusionTempleProcess::SetProtect(LPOBJ lpObj)
{
	m_PlayerData[lpObj->iTempleIndex].m_Skill1Time = 15;
	gObjAddBuffEffect(lpObj,BUFF_IT_PROTECTION,0,0,0,0,-10);
	return true;
}

int CIllusionTempleProcess::SetImobilize(LPOBJ lpObj,LPOBJ lpTargetObj)
{
	if( gCheckSkillDistance(lpObj->m_Index,lpTargetObj->m_Index,0xD3) == FALSE )
	{
		return false;
	}

	m_PlayerData[lpTargetObj->iTempleIndex].m_Skill2Time = 15;
	lpTargetObj->lpAttackObj = lpObj;
	gObjAddBuffEffect(lpTargetObj,BUFF_RESTRICTION,0,0,0,0,-10);
	
	lpTargetObj->PathCount = 0;
	lpTargetObj->PathStartEnd = 0;

	gObjSetPosition(lpTargetObj->m_Index,lpTargetObj->X,lpTargetObj->Y);
return true;
}

int CIllusionTempleProcess::SetTeleport(LPOBJ lpObj)
{
	if( gObjCheckUsedBuffEffect(lpObj,BUFF_STUN) == TRUE ||
		gObjCheckUsedBuffEffect(lpObj,BUFF_SLEEP) == TRUE ||
		gObjCheckUsedBuffEffect(lpObj,BUFF_FREEZE) == TRUE ||
		gObjCheckUsedBuffEffect(lpObj,BUFF_EARTHBINDS) == TRUE )
	{
		GCSendUseSkill(lpObj->m_Index,lpObj->m_Index,0xD4,FALSE);
		return false;
	}

	int RelicPlayer = m_RELIC_PLAYER;

	if( RelicPlayer == -1 )
	{
		GCSendUseSkill(lpObj->m_Index,lpObj->m_Index,0xD4,FALSE);
		return false;
	}

	if( RelicPlayer == lpObj->m_Index )
	{
		GCSendUseSkill(lpObj->m_Index,lpObj->m_Index,0xD4,FALSE);
		return false;
	}

	GCSendUseSkill(lpObj->m_Index,lpObj->m_Index,0xD4,TRUE);

	int x = 0;
	int y = 0;
	int count = 50;
	int bSuccess = FALSE;

	while( count-- != 0 )
	{
		x = gObj[RelicPlayer].X + rand()%3 - 1;
		y = gObj[RelicPlayer].Y + rand()%3 - 1;

		if( gObjCheckTeleportArea(lpObj->m_Index,x,y) )
		{
			bSuccess = TRUE;
			break;
		}
	}

	if( bSuccess == FALSE )
	{
		x = gObj[RelicPlayer].X;
		y = gObj[RelicPlayer].Y;
	}

	gObjTeleportMagicUse(lpObj->m_Index,x,y);
	return true;
}

BOOL CIllusionTempleProcess::SetShield(LPOBJ lpObj,LPOBJ lpTargetObj)
{

	if( gCheckSkillDistance(lpObj->m_Index,lpTargetObj->m_Index,0xD5) == FALSE )
	{
		return false;
	}

	lpTargetObj->lpAttackObj = lpObj;

	if( lpTargetObj->iShield <= 1 )
	{
		GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,0);
		return true;
	}

	int damage = lpTargetObj->iShield / 2;
	lpTargetObj->iShield = damage;
	GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,0,0,damage);
	return true;
}

struct PMSG_ILLUSIONTEMPLE_USESKILL
{
	PBMSG_HEAD2 h;
	BYTE result;
	BYTE MagicH;
	BYTE MagicL;
	WORD Index;
	WORD TargetIndex;
};

void CIllusionTempleProcess::GCSendUseSkill(int iIndex,int iTargetIndex,WORD skill,int bSuccess)
{
	PMSG_ILLUSIONTEMPLE_USESKILL pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,0x02,sizeof(pMsg));
	pMsg.result = bSuccess;
	pMsg.MagicH = SET_NUMBERH(skill);
	pMsg.MagicL = SET_NUMBERL(skill);
	pMsg.Index = iIndex;
	pMsg.TargetIndex = iTargetIndex;

	DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
	if( bSuccess == TRUE )
	{
		MsgSendV2(&gObj[iIndex],(LPBYTE)&pMsg,pMsg.h.size);
	}
}

void CIllusionTempleProcess::UserDie(LPOBJ lpObj)
{
	if( lpObj->iTempleIndex == -1 ||
		m_PlayerData[lpObj->iTempleIndex].m_Index != lpObj->m_Index )
	{
		return;
	}

	DropRelic(lpObj->m_Index);
	m_PlayerData[lpObj->iTempleIndex].m_SkillPts = 10;
	GCIllusionSendPoint(lpObj->m_Index,10);

	if( m_PlayerData[lpObj->iTempleIndex].m_Skill1Time > 0 )
	{
		m_PlayerData[lpObj->iTempleIndex].m_Skill1Time = 0;
		gObjRemoveBuffEffect(lpObj,BUFF_IT_PROTECTION);
		GCSendUseSkill(lpObj,0xD2);
	}

	if( m_PlayerData[lpObj->iTempleIndex].m_Skill2Time > 0 )
	{
		m_PlayerData[lpObj->iTempleIndex].m_Skill2Time = 0;
		gObjRemoveBuffEffect(lpObj,BUFF_RESTRICTION);
		GCSendUseSkill(lpObj,0xD3);
	}
}

void CIllusionTempleProcess::CalcWinRewardExp()
{
	CheckUsers();
	int AlliedReward = 0;
	int IllusionReward = 0;

	if( m_ALLIED_TEAM_PTS >= 2 && m_ALLIED_TEAM_PTS > m_ILLUSION_TEAM_PTS )
	{
		m_WinTeam = IT_ALLIED_TEAM;
		AlliedReward = (IT_REWARD_A[Temple] + (m_ALLIED_TEAM_PTS - m_ILLUSION_TEAM_PTS)*IT_REWARD_B[Temple])*2;

		if( m_ILLUSION_TEAM_PTS == 0 )
		{
			IllusionReward = IT_REWARD_A[Temple];
		}
		else
		{
			IllusionReward = IT_REWARD_A[Temple] + m_ILLUSION_TEAM_PTS*IT_REWARD_B[Temple];
		}
	}
	else if( m_ILLUSION_TEAM_PTS >= 2 && m_ILLUSION_TEAM_PTS > m_ALLIED_TEAM_PTS)
	{
		m_WinTeam = IT_ILLUSION_TEAM;
		IllusionReward = (IT_REWARD_A[Temple] + (m_ILLUSION_TEAM_PTS - m_ALLIED_TEAM_PTS)*IT_REWARD_B[Temple])*2;

		if( m_ALLIED_TEAM_PTS == 0 )
		{
			AlliedReward = IT_REWARD_A[Temple];
		}
		else
		{
			AlliedReward = IT_REWARD_A[Temple] + m_ALLIED_TEAM_PTS*IT_REWARD_B[Temple];
		}
	}
	else
	{
		if( m_ALLIED_TEAM_PTS == 0 )
		{
			AlliedReward = IT_REWARD_A[Temple];
		}
		else
		{
			AlliedReward = IT_REWARD_A[Temple] + m_ALLIED_TEAM_PTS*IT_REWARD_B[Temple];
		}

		if( m_ILLUSION_TEAM_PTS == 0 )
		{
			IllusionReward = IT_REWARD_A[Temple];
		}
		else
		{
			IllusionReward = IT_REWARD_A[Temple] + m_ILLUSION_TEAM_PTS*IT_REWARD_B[Temple];
		}
	}
	LogAddTD("[Illusion Temple] (%d) Result (WinTeam: %d)(Score:[%d][%d])",
		Temple+1,m_WinTeam,m_ALLIED_TEAM_PTS,m_ILLUSION_TEAM_PTS);

	for(int i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Team == (BYTE)-1 )
		{
			continue;
		}

		if( m_PlayerData[i].m_Index != -1 &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED &&
			gObj[m_PlayerData[i].m_Index].MapNumber == Temple + MAP_INDEX_ILLUSIONTEMPLE1 &&
			m_PlayerData[i].m_UseSkillCnt >= g_IllusionUseSkillCnt)
		{
			switch( m_PlayerData[i].m_Team )
			{
			case IT_ALLIED_TEAM:
				m_PlayerData[i].m_RewardExp = AlliedReward;
				break;
			case IT_ILLUSION_TEAM:
				m_PlayerData[i].m_RewardExp = IllusionReward;
				break;
			}

			m_PlayerData[i].m_RewardExp = m_PlayerData[i].m_RewardExp * g_IllusionRewardPerc / 100;
			CheckItemOptForGetExpEx(&gObj[m_PlayerData[i].m_Index],m_PlayerData[i].m_RewardExp,TRUE);

			int PartyCount = 0;

			if( gObj[m_PlayerData[i].m_Index].PartyNumber >= 0 )
			{
				PartyCount = gParty.GetPartyCount(gObj[m_PlayerData[i].m_Index].PartyNumber);
			}
			else
			{
				PartyCount = 1;
			}

			m_PlayerData[i].m_RankPoint = (int)(m_PlayerData[i].m_RewardExp + m_PlayerData[i].m_PlayerKillCount * IT_REWARD_C[Temple] + m_PlayerData[i].m_RelicSuccess * IT_REWARD_D[Temple] + IT_REWARD_E[PartyCount]);

			int iCAL_EXP = (int)m_PlayerData[i].m_RewardExp;
			int iRET_EXP = 0;

			while( iCAL_EXP > 0 )
			{
				iRET_EXP = iCAL_EXP;
				iCAL_EXP = LevelUp(m_PlayerData[i].m_Index,iCAL_EXP);
			}

			int iIndex = m_PlayerData[i].m_Index;
			if( g_MasterExp.IsEnabled(&gObj[iIndex]) == FALSE )
			{
				GCSendExp(iIndex,(WORD)-1,iRET_EXP,0,0);
			}
		}
	}
}

void CIllusionTempleProcess::DropReward(int iIndex)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		return;
	}

	if( m_PlayerData[gObj[iIndex].iTempleIndex].m_Index != gObj[iIndex].m_Index )
	{
		return;
	}

	if( GetState() != IT_STATE_PLAYEND )
	{
		return;
	}

	if( m_PlayerData[gObj[iIndex].iTempleIndex].m_Team == 0xFF )
	{
		return;
	}

	if( m_WinTeam == 0xFF )
	{
		return;
	}

	if( m_PlayerData[gObj[iIndex].iTempleIndex].m_Team != m_WinTeam )
	{
		return;
	}

	if( m_PlayerData[gObj[iIndex].iTempleIndex].m_bReward == TRUE )
	{
		return;
	}

	m_PlayerData[gObj[iIndex].iTempleIndex].m_bReward = TRUE;


	switch( (gObj[iIndex].MapNumber - MAP_INDEX_ILLUSIONTEMPLE1) )
	{
	case 0:
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::IllusionTemple1, iIndex, gObj[iIndex].MapNumber, gObj[iIndex].X, gObj[iIndex].Y);
		break;
	case 1:
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::IllusionTemple2, iIndex, gObj[iIndex].MapNumber, gObj[iIndex].X, gObj[iIndex].Y);
		break;
	case 2:
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::IllusionTemple3, iIndex, gObj[iIndex].MapNumber, gObj[iIndex].X, gObj[iIndex].Y);
		break;
	case 3:
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::IllusionTemple4, iIndex, gObj[iIndex].MapNumber, gObj[iIndex].X, gObj[iIndex].Y);
		break;
	case 4:
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::IllusionTemple5, iIndex, gObj[iIndex].MapNumber, gObj[iIndex].X, gObj[iIndex].Y);
		break;
	case 5:
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::IllusionTemple6, iIndex, gObj[iIndex].MapNumber, gObj[iIndex].X, gObj[iIndex].Y);
		break;
	}
}


int CIllusionTempleProcess::GetEnterItem(int aIndex)
{
	if( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return -1;
	}

	int pos = -1;


	for(int n = 0; n < INVENTORY_SIZE; n++)
	{
		CItem* lpItem = &gObj[aIndex].pInventory[n];

		if( lpItem->IsItem() == TRUE )
		{
			if( lpItem->m_Type == ITEMGET(13,51) &&
				lpItem->m_Level == Temple+1 )
			{
				if( lpItem->m_Durability > 0.0f )
				{
					pos = n;
					break;
				}
				else
				{
					gObjInventoryDeleteItem(aIndex,n);
					GCInventoryItemDeleteSend(aIndex,n,0);
				}
			}

			if( lpItem->m_Type == ITEMGET(13,61) )
			{
				if( lpItem->m_Durability > 0.0f )
				{
					if( lpItem->m_Durability > 0.0f )
					{
						pos = n;
						break;
					}
					else
					{
						gObjInventoryDeleteItem(aIndex,n);
						GCInventoryItemDeleteSend(aIndex,n,0);
					}
				}
			}
		}
	}
return pos;
}

void CIllusionTempleProcess::ItemDrop(LPOBJ lpObj)
{
	if( m_PlayerData[lpObj->iTempleIndex].m_Index != lpObj->m_Index )
		return;

	//Item drop from player kill
}

void CIllusionTempleProcess::SendAllUserAnyMsg(LPBYTE lpMsg,int size)
{
	for(int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++)
	{
		if( gObj[i].Connected == PLAYER_PLAYING &&
			gObj[i].Type == OBJ_USER)
		{
			DataSend(i,lpMsg,size);
		}
	}
}

int CIllusionTempleProcess::LevelUp(int iUserIndex,int iAddExp)
{
	if ( !OBJMAX_RANGE(iUserIndex ) )
		return 0;

	if( g_MasterExp.LevelUp(&gObj[iUserIndex],iAddExp) )
	{
		return FALSE;
	}

	gObjSetExpPetItem(iUserIndex, iAddExp);
	int iLEFT_EXP = 0;

	LogAddTD("Experience : Map[%d]-(%d,%d) [%s][%s](%d) %u %d",
		gObj[iUserIndex].MapNumber, gObj[iUserIndex].X, gObj[iUserIndex].Y,
		gObj[iUserIndex].AccountID, gObj[iUserIndex].Name,
		gObj[iUserIndex].Level, gObj[iUserIndex].Experience, iAddExp);

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
			LogAddTD("[%s][%s] LevelUp PlusStatQuest Clear AddStat %d",
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
//		gObjCalcMaxLifePower(gObj[iUserIndex].m_Index);

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

BYTE CIllusionTempleProcess::GetRemainTime2()
{
	if( GetState() == IT_STATE_CLOSED)
	{
		return m_iRemainMinute;
	}
return FALSE;
}

int CIllusionTempleProcess::Is1stSkillActive(int iEventIndex)
{
	if( IT_PLAYER_RANGE(iEventIndex) == FALSE )
	{
		return FALSE;
	}

	if( m_PlayerData[iEventIndex].m_Index != -1 &&
		m_PlayerData[iEventIndex].m_Skill1Time > 0 )
	{
		return TRUE;
	}
return FALSE;
}

int CIllusionTempleProcess::Is2ndSkillActive(int iEventIndex)
{
	if( IT_PLAYER_RANGE(iEventIndex) == FALSE )
	{
		return FALSE;
	}

	if( m_PlayerData[iEventIndex].m_Index != -1 &&
		m_PlayerData[iEventIndex].m_Skill2Time > 0 )
	{
		return TRUE;
	}
return FALSE;
}

struct ILLUSIONTEMPLE_SCOREINFO
{
	char Name[10];
	BYTE Map;
	BYTE Team;
	BYTE Class;
	int Exp;
};

struct PMSG_ILLUSIONTEMPLE_SCOREINFO
{
	PBMSG_HEAD2 h;
	BYTE Score[2];
	BYTE count;
};


void CIllusionTempleProcess::SendRewardExp()
{
	int lOfs = 0;
	BYTE SendData[256];
	PMSG_ILLUSIONTEMPLE_SCOREINFO pMsg;

	pMsg.Score[0] = m_ALLIED_TEAM_PTS;
	pMsg.Score[1] = m_ILLUSION_TEAM_PTS;
	pMsg.count = UserCount;

	lOfs += sizeof(pMsg);

	int i;
	for(i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Index != -1 &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED)
		{
			ILLUSIONTEMPLE_SCOREINFO pMsgBody;
			memcpy(pMsgBody.Name,gObj[m_PlayerData[i].m_Index].Name,10);
			pMsgBody.Map = Temple + MAP_INDEX_ILLUSIONTEMPLE1;
			pMsgBody.Team = m_PlayerData[i].m_Team;
			pMsgBody.Class = (BYTE)gObj[m_PlayerData[i].m_Index].Class;
			pMsgBody.Exp = (int)m_PlayerData[i].m_RewardExp;

			memcpy(&SendData[lOfs],&pMsgBody,sizeof(pMsgBody));
			lOfs += sizeof(pMsgBody);

			LogAddTD("[Illusion Temple] (%d) RewardExp Result  (Account:%s, Name:%s, Team:%d, Class:%d, UserKillCount:%d, RelicsMoveCount:%d, RewardExp:%d, RankingPoint: %d)",
				Temple+1,gObj[m_PlayerData[i].m_Index].AccountID,gObj[m_PlayerData[i].m_Index].Name,
				m_PlayerData[i].m_Team,gObj[m_PlayerData[i].m_Index].DbClass,m_PlayerData[i].m_PlayerKillCount,
				m_PlayerData[i].m_RelicSuccess,m_PlayerData[i].m_RewardExp,m_PlayerData[i].m_RankPoint);
		}
	}

	PHeadSubSetB((LPBYTE)&pMsg,0xBF,0x04,lOfs);
	memcpy(&SendData[0],&pMsg,sizeof(pMsg));

	for(i = 0; i < IT_MAXPLAYER; i++)
	{
		if( m_PlayerData[i].m_Team == (BYTE)-1 )
			continue;

		if( m_PlayerData[i].m_Index != -1 &&
			gObj[m_PlayerData[i].m_Index].Connected > PLAYER_LOGGED)
		{
			DataSend(m_PlayerData[i].m_Index,SendData,pMsg.h.size);
		}
	}
}

int CIllusionTempleProcess::ChangeTempleUserInformation(int iUserIndex,int iIllusionTempleIndex,int iCurIndex)
{
	EnterCriticalSection(&this->m_CritSec);

	if( this->m_PlayerData[iIllusionTempleIndex].m_Index == iUserIndex )
	{
		this->m_PlayerData[iIllusionTempleIndex].m_Index = iCurIndex;
		LeaveCriticalSection(&this->m_CritSec);
		return true;
	}

	LeaveCriticalSection(&this->m_CritSec);
	return false;
}

BYTE CIllusionTempleProcess::GetSkillPoint(int nIndex)
{
	if( !OBJMAX_RANGE(nIndex) )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return 0;
	}

	if( gObj[nIndex].iTempleIndex != -1 &&
		this->m_PlayerData[gObj[nIndex].iTempleIndex].m_Index == nIndex &&
		gObj[nIndex].Connected > PLAYER_LOGGED )
	{
		return this->m_PlayerData[gObj[nIndex].iTempleIndex].m_SkillPts;
	}
	return 0;
}