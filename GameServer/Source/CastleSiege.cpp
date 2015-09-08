#include "stdafx.h"
#include "CastleSiege.h"

// GS-CS 0.99.60T 0x00533AC0

#if (GS_CASTLE==1)

#include "DSProtocol.h"
#include "GameMain.h"
#include "LifeStone.h"
#include "Mercenary.h"
#include "DBSockMng.h"
#include "LogToFile.h"
#include "TNotice.h"
#include "TUnion.h"
#include "TUnionInfo.h"
#include "logproc.h"
#include "..\include\readscript.h"
#include "BuffEffectSlot.h"

CCastleSiege g_CastleSiege;

int g_iNpcUpDfLevel_CGATE[CS_MAX_UPGRADE][0x3] = {
	1,	2,	3000000,
	2,	3,	3000000,
	3,	4,	3000000
};

int g_iNpcUpDfLevel_CSTATUE[CS_MAX_UPGRADE][0x3] = {
	1,	3,	3000000,
	2,	5,	3000000,
	3,	7,	3000000,
};

int g_iNpcUpMaxHP_CGATE[0x3][CS_MAX_UPGRADE] = {
	800000,		2,	1000000,
	1500000,	3,	1000000,
	3000000,	4,	1000000,
};

int g_iNpcUpMaxHP_CSTATUE[CS_MAX_UPGRADE][0x3] = {
	750000,		3,	1000000,
	1300000,	5,	1000000,
	2500000,	7,	1000000
};

int g_iNpcUpRgLevel_CSTATUE[CS_MAX_UPGRADE][0x3] = {
	1,	3,	5000000,
	2,	5,	5000000,
	3,	7,	5000000
};


int g_iCsUserReSpawnArea[0x3][0x4] = {
	35,		17,		144,	48,
	74,		144,	115,	154,
	35,		11,		144,	48
};

int g_iNpcDefense_CGATE[CS_MAX_UPGRADE + 1] = {
	100, 180, 300, 520
};
int g_iNpcDefense_CSTATUE[CS_MAX_UPGRADE + 1] = {
	80, 180, 340, 550
};

int g_iCastleWallStepAxisY[3] = {
	113,	160,	203
};

int g_iCastleItemMixLimit = 1;

CCastleSiege::CCastleSiege()
{
	InitializeCriticalSection(&m_critScheduleData);
	InitializeCriticalSection(&m_critNpcData);
	InitializeCriticalSection(&m_critCsTotalGuildInfo);
	InitializeCriticalSection(&m_critCsBasicGuildInfo);
	InitializeCriticalSection(&m_critCsMiniMap);

	Clear();
}

CCastleSiege::~CCastleSiege()
{
	DeleteCriticalSection(&m_critScheduleData);
	DeleteCriticalSection(&m_critNpcData);
	DeleteCriticalSection(&m_critCsTotalGuildInfo);
	DeleteCriticalSection(&m_critCsBasicGuildInfo);
	DeleteCriticalSection(&m_critCsMiniMap);
}

unsigned char CCastleSiege::ScheduleStateCompFunc(_CS_SCHEDULE_DATA & A,_CS_SCHEDULE_DATA & B)
{
	if(A.m_iSTATE > B.m_iSTATE)
	{
		return false;
	}
return true;
}

void CCastleSiege::Clear()
{
	m_bCastleSiegeEnable = 1;
	m_bFileDataLoadOK = 0;
	m_bDbDataLoadOK = 0;
	m_bDoRun = 0;
	m_iMapSvrGroup = -1;
	m_iCastleSiegeCycle = 14;
	m_iCastleSiegeState = -1;
	m_bFixCastleCycleStartDate = 0;
	m_i64CastleMoney = 0;

	memset(&m_tmFixCastleCycleStartDate,0x00,sizeof(m_tmFixCastleCycleStartDate));

	m_iFixCastleSpecificState = 0;
	m_bFixCastleStateStartDate = 0;

	memset(&m_tmFixCastleStateStartDate,0x00,sizeof(m_tmFixCastleStateStartDate));

	m_iCastleDataLoadState = 0;
	m_dwDbDataLoadTickCount = 0;
	m_iCS_REMAIN_MSEC = -1;

	memset(&m_tmStateStartDate,0x00,sizeof(m_tmStateStartDate));
	memset(&m_tmLeftCastleSiegeDate,0x00,sizeof(m_tmLeftCastleSiegeDate));

	m_bCsBasicGuildInfoLoadOK = 0;
	m_bCsTotalGuildInfoLoadOK = 0;
	m_btIsSiegeGuildList = 0;
	m_btIsCastleOccupied = 0;

	memset(m_szCastleOwnerGuild,0x00,sizeof(m_szCastleOwnerGuild));

	m_bEnterHuntZone = 0;
	m_dwNPC_DBSAVE_TICK_COUNT = GetTickCount();
	m_dwCALC_LEFTSIEGE_TICK_COUNT = 0;
	m_dwEVENT_MSG_TICK_COUNT = 0;
	m_dwCHECK_GATE_ALIVE_COUNT = 0;
	m_dwCS_LEFTTIME_TICK_COUNT = 0;
	m_dwCS_STARTTIME_TICK_COUNT = 0;
	m_dwCS_JOINSIDE_REFRESH_TICK_COUNT = 0;
	m_dwCS_JOINSIDE_REFRESH_TOTAL_COUNT = 0;
	m_dwCS_MINIMAP_SEND_TICK_COUNT = 0;
	m_bCastleTowerAccessable = 0;
	m_bRegCrownAvailable = 0;
	m_iCastleCrownAccessUser = -1;
	m_btCastleCrownAccessUserX = 0;
	m_btCastleCrownAccessUserY = 0;
	m_iCastleSwitchAccessUser1 = -1;
	m_iCastleSwitchAccessUser2 = -1;
	m_dwCrownAccessTime = 0;

	memset(m_szMiddleWinnerGuild,0x00,sizeof(m_szMiddleWinnerGuild));

	EnterCriticalSection(&m_critScheduleData);
	m_vtScheduleData.clear();
	LeaveCriticalSection(&m_critScheduleData);

	EnterCriticalSection(&m_critNpcData);
	m_vtNpcData.clear();
	LeaveCriticalSection(&m_critNpcData);

	m_bDbNpcCreated = 0;

	ResetAllUserCsJoinSide();
}

int CCastleSiege::Ready(int iMapSvrGroup)
{
	if(iMapSvrGroup < 0)
		return false;
	Clear();
	ClearAllNPC();

	m_iMapSvrGroup = iMapSvrGroup;
	m_iCastleDataLoadState = 1;
return true;
}

#define MACRO1(value) (( value & 0xFF) & 0xFF)
#define MACRO2(value) (( value & 0xFFFF ) & 0xFFFF)

#define MONTH_MACRO(value) ( (( (value)   )<0)?FALSE:(( (value)  )>12)?FALSE:TRUE  )
#define DAY_MACRO(value) ( (( (value - 1)   )<0)?FALSE:(( (value - 1)  )>30)?FALSE:TRUE  )
#define HOUR_MACRO(value) ( (( (value)   )<0)?FALSE:(( (value)  )>23)?FALSE:TRUE  )
#define MINUTE_MACRO(value) ( (( (value)   )<0)?FALSE:(( (value)  )>59)?FALSE:TRUE  )

int CCastleSiege::LoadPreFixData(char * lpszFileName)
{
	if(lpszFileName == 0)
		return false;

	_SYSTEMTIME tmToDay;

	GetLocalTime(&tmToDay);

	int iTODAY_DATE_NUM = MACRO2(( MACRO1(tmToDay.wDay) | MACRO1(tmToDay.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmToDay.wYear) << 16;
	__int64 iTODAY_DATE_NUM64 = (MACRO2(tmToDay.wMinute) | MACRO2(tmToDay.wHour) << 16) | ((__int64)(MACRO2(( MACRO1(tmToDay.wDay) | MACRO1(tmToDay.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmToDay.wYear) << 16) << 0x20);

	m_bCastleSiegeEnable = GetPrivateProfileInt("GameServerInfo","CastleSiegeRun",1,lpszFileName);
	m_tmFixCastleCycleStartDate.wYear = GetPrivateProfileInt("GameServerInfo","CastleCycleStartYear",0,lpszFileName);
	m_tmFixCastleCycleStartDate.wMonth = GetPrivateProfileInt("GameServerInfo","CastleCycleStartMonth",0,lpszFileName);
	m_tmFixCastleCycleStartDate.wDay = GetPrivateProfileInt("GameServerInfo","CastleCycleStartDay",0,lpszFileName);

	m_iFixCastleSpecificState = GetPrivateProfileInt("GameServerInfo","CastleSpecificState",0,lpszFileName);

	m_tmFixCastleStateStartDate.wYear = GetPrivateProfileInt("GameServerInfo","CastleStateStartYear",0,lpszFileName);
	m_tmFixCastleStateStartDate.wMonth = GetPrivateProfileInt("GameServerInfo","CastleStateStartMonth",0,lpszFileName);
	m_tmFixCastleStateStartDate.wDay = GetPrivateProfileInt("GameServerInfo","CastleStateStartDay",0,lpszFileName);
	m_tmFixCastleStateStartDate.wHour = GetPrivateProfileInt("GameServerInfo","CastleStateStartHour",0,lpszFileName);
	m_tmFixCastleStateStartDate.wMinute = GetPrivateProfileInt("GameServerInfo","CastleStateStartMinute",0,lpszFileName);

	if(m_tmFixCastleCycleStartDate.wYear == 0
		|| m_tmFixCastleCycleStartDate.wMonth == 0
		|| m_tmFixCastleCycleStartDate.wDay == 0)
	{
		m_bFixCastleCycleStartDate = 0;
	}
	else
	{
		_SYSTEMTIME tmEndDate = m_tmFixCastleCycleStartDate;
		GetNextDay(&tmEndDate,m_iCastleSiegeCycle,0,0,0);

		int iEVENT_END_DATE_NUM = MACRO2(( MACRO1(tmEndDate.wDay) | MACRO1(tmEndDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmEndDate.wYear) << 16;

		if(iEVENT_END_DATE_NUM <= iTODAY_DATE_NUM)
		{
			m_bFixCastleCycleStartDate = 0;
			MsgBox("[CastleSiege] CCastleSiege::LoadPreFixData() - iEVENT_END_DATE_NUM (%04d-%02d-%02d) <= iTODAY_DATE_NUM (%04d-%02d-%02d)",
				tmEndDate.wYear,tmEndDate.wMonth,tmEndDate.wDay,tmToDay.wYear,tmToDay.wMonth,tmToDay.wDay);
		}
		else
		{
			m_bFixCastleCycleStartDate = 1;
		}

		if(MONTH_MACRO(m_tmFixCastleCycleStartDate.wMonth) == FALSE
		|| DAY_MACRO(m_tmFixCastleCycleStartDate.wDay) == FALSE)
		{
			m_bFixCastleCycleStartDate = 0;
		}
	}

	if(m_tmFixCastleStateStartDate.wYear == 0
		|| m_tmFixCastleStateStartDate.wMonth == 0
		|| m_tmFixCastleStateStartDate.wDay == 0)
	{
		m_bFixCastleStateStartDate = 0;
	}
	else if(m_iFixCastleSpecificState < 1 || m_iFixCastleSpecificState > 9)
	{
		m_bFixCastleStateStartDate = 0;
	}
	else
	{
		__int64 iSIEGE_DATE_NUM64 = (MACRO2(m_tmFixCastleStateStartDate.wMinute) | MACRO2(m_tmFixCastleStateStartDate.wHour) << 16) | ((__int64)(MACRO2(( MACRO1(m_tmFixCastleStateStartDate.wDay) | MACRO1(m_tmFixCastleStateStartDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(m_tmFixCastleStateStartDate.wYear) << 16) << 0x20);

		if(iSIEGE_DATE_NUM64 <= iTODAY_DATE_NUM64)
		{
			m_bFixCastleStateStartDate = 0;
			MsgBox("[CastleSiege] CCastleSiege::LoadPreFixData() - iSIEGE_DATE_NUM64 (STATE:%d, %04d-%02d-%02d %02d:%02d:%02d) <= iTODAY_DATE_NUM64 (%04d-%02d-%02d %02d:%02d:%02d)",
				m_iFixCastleSpecificState,m_tmFixCastleStateStartDate.wYear,m_tmFixCastleStateStartDate.wMonth,m_tmFixCastleStateStartDate.wDay,m_tmFixCastleStateStartDate.wHour,m_tmFixCastleStateStartDate.wMinute,m_tmFixCastleStateStartDate.wSecond,
				tmToDay.wYear,tmToDay.wMonth,tmToDay.wDay,tmToDay.wHour,tmToDay.wMinute,tmToDay.wSecond);
		}
		else
		{
			m_bFixCastleStateStartDate = 1;
		}

		if(m_iFixCastleSpecificState == 8)
		{
			MsgBox("[CastleSiege] CCastleSiege::LoadPreFixData() - FIX STATE 'CASTLESIEGE_STATE_ENDSIEGE' CAN'T BE USED");
			m_bFixCastleStateStartDate = 0;
		}

		if(MONTH_MACRO(m_tmFixCastleStateStartDate.wMonth) == FALSE
			|| DAY_MACRO(m_tmFixCastleStateStartDate.wDay) == FALSE
			|| HOUR_MACRO(m_tmFixCastleStateStartDate.wHour) == FALSE
			|| MINUTE_MACRO(m_tmFixCastleStateStartDate.wMinute) == FALSE)
		{
			m_bFixCastleStateStartDate = 0;
		}
	}

	WritePrivateProfileString("GameServerInfo","CastleCycleStartYear","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","CastleCycleStartMonth","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","CastleCycleStartDay","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","CastleSpecificState","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","CastleStateStartYear","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","CastleStateStartMonth","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","CastleStateStartDay","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","CastleStateStartHour","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","CastleStateStartMinute","0",lpszFileName);

	return m_bFixCastleCycleStartDate & m_bFixCastleStateStartDate;
}

int CCastleSiege::LoadData(char * lpszFileName)
{
using namespace std;
	if(lpszFileName == NULL || strcmp(lpszFileName,"")==NULL)
	{
		MsgBox("[CastleSiege] file load error - File Name Error");
		return false;
	}

	if(m_iCastleDataLoadState != 1)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::LoadData() - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_1 (%d)",
			m_iCastleDataLoadState);
		return false;
	}

	SMDFile = fopen(lpszFileName,"r");

	if(SMDFile == NULL)
	{
		MsgBox("[CastleSiege] file load error - fopen() : %s",
			lpszFileName);
		return false;
	}

	int type = -1;
	enum SMDToken Token;

	vector<_CS_SCHEDULE_DATA> vtScheduleData;
	vector<_CS_NPC_DATA> vtNpcData;
	map<unsigned int,int> mpNpcDataCheck;

	int iCountDfUpgrade_Gate = 0;
	int iCountDfUpgrade_Statue = 0;
	int iCountHpUpgrade_Gate = 0;
	int iCountHpUpgrade_Statue = 0;
	int iCountRgUpgrade_Statue = 0;
	int iCountDfValue_Gate = 0;
	int iCountDfValue_Statue = 0;

	while( true )
	{
		Token = GetToken();

		if(Token == END)
		{
			break;
		}

		type = (int)TokenNumber;

		while ( true )
		{
			if(type == 0)
			{
				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				int iCastleSiegeCycle = (int)TokenNumber;

				if(iCastleSiegeCycle <= 0)
				{
					LogAddC(2,"[CastleSiege] CCastleSiege::LoadData() - iCastleSiegeCycle <= 0 : %d",
						iCastleSiegeCycle);
					return false;
				}
				else
				{
					m_iCastleSiegeCycle = iCastleSiegeCycle;
				}
			}
			else if(type == 1)
			{
				int iSTATE = -1;
				int iSTART_DAY = -1;
				int iSTART_HOUR = -1;
				int iSTART_MIN = -1;

				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				iSTATE = (int)TokenNumber;
				Token = GetToken();
				iSTART_DAY = (int)TokenNumber;
				Token = GetToken();
				iSTART_HOUR = (int)TokenNumber;
				Token = GetToken();
				iSTART_MIN = (int)TokenNumber;

				if(!vtScheduleData.empty())
				{
					vector<_CS_SCHEDULE_DATA>::iterator it;
					for( it = vtScheduleData.begin(); it != vtScheduleData.end(); it++)
					{
						_CS_SCHEDULE_DATA pScheduleData = _CS_SCHEDULE_DATA(*it);

						if(pScheduleData.m_bIN_USE != 0)
						{
							if(pScheduleData.m_iSTATE == iSTATE)
							{
								LogAddC(2,"[CastleSiege] CCastleSiege::LoadData() - Same State Exist : %d",
									iSTATE);
								return false;
							}
							
							if(pScheduleData.m_iSTATE > iSTATE)
							{
								int iSTATE1_TIME = pScheduleData.m_iADD_DAY * 24 * 60 + pScheduleData.m_iADD_HOUR * 60 + pScheduleData.m_iADD_MIN;
								int iSTATE2_TIME = iSTART_DAY * 24 * 60 + iSTART_HOUR * 60 + iSTART_MIN;
	
								if(iSTATE1_TIME <= iSTATE2_TIME)
								{
									LogAddC(2,"[CastleSiege] CCastleSiege::LoadData() - Date Order is wrong : %d",
										iSTATE2_TIME);
									return false;
								}
							}
							else
							{
								int iSTATE1_TIME = pScheduleData.m_iADD_DAY * 24 * 60 + pScheduleData.m_iADD_HOUR * 60 + pScheduleData.m_iADD_MIN;
								int iSTATE2_TIME = iSTART_DAY * 24 * 60 + iSTART_HOUR * 60 + iSTART_MIN;
								if(iSTATE1_TIME >= iSTATE2_TIME)
								{
									LogAddC(2,"[CastleSiege] CCastleSiege::LoadData() - Date Order is wrong : %d",
										iSTATE2_TIME);
									return false;
								}
							}
						}
					}
				}

				_CS_SCHEDULE_DATA pScheduleDataInsert;

				pScheduleDataInsert.m_bIN_USE = 1;
				pScheduleDataInsert.m_iSTATE = iSTATE;
				pScheduleDataInsert.m_iADD_DAY = iSTART_DAY;
				pScheduleDataInsert.m_iADD_HOUR = iSTART_HOUR;
				pScheduleDataInsert.m_iADD_MIN = iSTART_MIN;

				vtScheduleData.push_back(pScheduleDataInsert);
			}
			else if(type == 2)
			{
				int iNPC_NUM = -1;
				int iNPC_INDEX = -1;
				int iNPC_SIDE = 0;
				int iNPC_DBSAVE = 0;
				int iNPC_DF_LEVEL = 0;
				int iNPC_RG_LEVEL = 0;
				int iNPC_MAXHP = 0;
				int iNPC_SX = -1;
				int iNPC_SY = -1;
				int iNPC_DX = -1;
				int iNPC_DY = -1;
				int iNPC_DIR = -1;

				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				iNPC_NUM = (int)TokenNumber;
				Token = GetToken();
				iNPC_INDEX = (int)TokenNumber;
				Token = GetToken();
				iNPC_DBSAVE = (int)TokenNumber;
				Token = GetToken();
				iNPC_SIDE = (int)TokenNumber;
				Token = GetToken();
				iNPC_DF_LEVEL = (int)TokenNumber;
				Token = GetToken();
				iNPC_RG_LEVEL = (int)TokenNumber;
				Token = GetToken();
				iNPC_MAXHP = (int)TokenNumber;
				Token = GetToken();
				iNPC_SX = (int)TokenNumber;
				Token = GetToken();
				iNPC_SY = (int)TokenNumber;
				Token = GetToken();
				iNPC_DX = (int)TokenNumber;
				Token = GetToken();
				iNPC_DY = (int)TokenNumber;
				Token = GetToken();
				iNPC_DIR = (int)TokenNumber;


				if(iNPC_NUM < 0)
				{
					MsgBox("[CastleSiege] CCastleSiege::LoadData() - Data Fail (iNPC_NUM < 0)");
					return false;
				}

				if(iNPC_INDEX < 0)
				{
					MsgBox("[CastleSiege] CCastleSiege::LoadData() - Data Fail (iNPC_INDEX < 0)");
					return false;
				}

				if(iNPC_SIDE < 0)
				{
					MsgBox("[CastleSiege] CCastleSiege::LoadData() - Data Fail (iNPC_SIDE < 0)");
					return false;
				}

				if(iNPC_DF_LEVEL < 0)
				{
					MsgBox("[CastleSiege] CCastleSiege::LoadData() - Data Fail (iNPC_DF_LEVEL < 0)");
					return false;
				}

				if(iNPC_RG_LEVEL < 0)
				{
					MsgBox("[CastleSiege] CCastleSiege::LoadData() - Data Fail (iNPC_RG_LEVEL < 0)");
					return false;
				}

				if(iNPC_MAXHP < 0)
				{
					MsgBox("[CastleSiege] CCastleSiege::LoadData() - Data Fail (iNPC_MAXHP < 0)");
					return false;
				}

				if(iNPC_SX < 0)
				{
					MsgBox("[CastleSiege] CCastleSiege::LoadData() - Data Fail (iNPC_SX < 0)");
					return false;
				}

				if(iNPC_SY < 0)
				{
					MsgBox("[CastleSiege] CCastleSiege::LoadData() - Data Fail (iNPC_SY < 0)");
					return false;
				}

				unsigned int uNpcKey = MACRO2(iNPC_INDEX) | (MACRO2(iNPC_NUM) << 16);

				map<unsigned int,int>::iterator it = mpNpcDataCheck.find(uNpcKey);

				if(it != mpNpcDataCheck.end())
				{
					MsgBox("[CastleSiege] CCastleSiege::LoadData() - Same Npc Key Exist : NUM:%d-IDX:%d",iNPC_NUM,iNPC_INDEX);
					return false;
				}

				mpNpcDataCheck.insert(pair <unsigned int, int > ( ( MACRO2(iNPC_INDEX) | (MACRO2(iNPC_NUM) << 16) ), 0));

				_CS_NPC_DATA pNpcDataInsert;

				pNpcDataInsert.m_bIN_USE = 1;
				pNpcDataInsert.m_iNPC_NUM = iNPC_NUM;
				pNpcDataInsert.m_iNPC_INDEX = iNPC_INDEX;
				pNpcDataInsert.m_iNPC_SIDE = iNPC_SIDE;
				pNpcDataInsert.m_bNPC_DBSAVE = iNPC_DBSAVE;

				pNpcDataInsert.m_iNPC_BASE_DF_LEVEL = iNPC_DF_LEVEL;
				pNpcDataInsert.m_iNPC_DF_LEVEL = pNpcDataInsert.m_iNPC_BASE_DF_LEVEL;

				pNpcDataInsert.m_iNPC_BASE_RG_LEVEL = iNPC_RG_LEVEL;
				pNpcDataInsert.m_iNPC_RG_LEVEL = pNpcDataInsert.m_iNPC_BASE_RG_LEVEL;

				pNpcDataInsert.m_iNPC_BASE_MAXHP = iNPC_MAXHP;
				pNpcDataInsert.m_iNPC_MAXHP = pNpcDataInsert.m_iNPC_BASE_MAXHP;

				pNpcDataInsert.m_iNPC_BASE_HP = iNPC_MAXHP;
				pNpcDataInsert.m_iNPC_HP = pNpcDataInsert.m_iNPC_BASE_HP;

				pNpcDataInsert.m_iNPC_BASE_SX = iNPC_SX;
				pNpcDataInsert.m_iNPC_SX = pNpcDataInsert.m_iNPC_BASE_SX;

				pNpcDataInsert.m_iNPC_BASE_SY = iNPC_SY;
				pNpcDataInsert.m_iNPC_SY = pNpcDataInsert.m_iNPC_BASE_SY;

				pNpcDataInsert.m_iNPC_BASE_DX = iNPC_DX;
				pNpcDataInsert.m_iNPC_DX = pNpcDataInsert.m_iNPC_BASE_DX;

				pNpcDataInsert.m_iNPC_BASE_DY = iNPC_DY;
				pNpcDataInsert.m_iNPC_DY = pNpcDataInsert.m_iNPC_BASE_DY;

				pNpcDataInsert.m_iNPC_BASE_DIR = iNPC_DIR;
				pNpcDataInsert.m_iNPC_DIR = pNpcDataInsert.m_iNPC_BASE_DIR;

				vtNpcData.push_back(pNpcDataInsert);
			}
			else if(type == 3)
			{
				int iLEVEL = 0;
				int iGEMCOUNT = 0;
				int iZEN = 0;

				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				iLEVEL = (int)TokenNumber;
				Token = GetToken();
				iGEMCOUNT = (int)TokenNumber;
				Token = GetToken();
				iZEN = (int)TokenNumber;

				if(iCountDfUpgrade_Gate >= CS_MAX_UPGRADE)
					break;

				g_iNpcUpDfLevel_CGATE[iCountDfUpgrade_Gate][0] = iLEVEL;
				g_iNpcUpDfLevel_CGATE[iCountDfUpgrade_Gate][1] = iGEMCOUNT;
				g_iNpcUpDfLevel_CGATE[iCountDfUpgrade_Gate][2] = iZEN;

				iCountDfUpgrade_Gate++;
			}
			else if(type == 4)
			{
				int iLEVEL = 0;
				int iGEMCOUNT = 0;
				int iZEN = 0;

				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				iLEVEL = (int)TokenNumber;
				Token = GetToken();
				iGEMCOUNT = (int)TokenNumber;
				Token = GetToken();
				iZEN = (int)TokenNumber;

				if(iCountDfUpgrade_Statue >= CS_MAX_UPGRADE)
					break;

				g_iNpcUpDfLevel_CSTATUE[iCountDfUpgrade_Statue][0] = iLEVEL;
				g_iNpcUpDfLevel_CSTATUE[iCountDfUpgrade_Statue][1] = iGEMCOUNT;
				g_iNpcUpDfLevel_CSTATUE[iCountDfUpgrade_Statue][2] = iZEN;

				iCountDfUpgrade_Statue++;
			}
			else if(type == 5)
			{
				int iHP = 0;
				int iGEMCOUNT = 0;
				int iZEN = 0;

				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				iHP = (int)TokenNumber;
				Token = GetToken();
				iGEMCOUNT = (int)TokenNumber;
				Token = GetToken();
				iZEN = (int)TokenNumber;

				if(iCountHpUpgrade_Gate >= CS_MAX_UPGRADE)
					break;

				g_iNpcUpMaxHP_CGATE[iCountHpUpgrade_Gate][0] = iHP;
				g_iNpcUpMaxHP_CGATE[iCountHpUpgrade_Gate][1] = iGEMCOUNT;
				g_iNpcUpMaxHP_CGATE[iCountHpUpgrade_Gate][2] = iZEN;

				iCountHpUpgrade_Gate++;
			}
			else if(type == 6)
			{
				int iHP = 0;
				int iGEMCOUNT = 0;
				int iZEN = 0;

				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				iHP = (int)TokenNumber;
				Token = GetToken();
				iGEMCOUNT = (int)TokenNumber;
				Token = GetToken();
				iZEN = (int)TokenNumber;

				if(iCountHpUpgrade_Statue >= CS_MAX_UPGRADE)
					break;

				g_iNpcUpMaxHP_CSTATUE[iCountHpUpgrade_Statue][0] = iHP;
				g_iNpcUpMaxHP_CSTATUE[iCountHpUpgrade_Statue][1] = iGEMCOUNT;
				g_iNpcUpMaxHP_CSTATUE[iCountHpUpgrade_Statue][2] = iZEN;

				iCountHpUpgrade_Statue++;
			}
			else if(type == 7)
			{
				int iLEVEL = 0;
				int iGEMCOUNT = 0;
				int iZEN = 0;

				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				iLEVEL = (int)TokenNumber;
				Token = GetToken();
				iGEMCOUNT = (int)TokenNumber;
				Token = GetToken();
				iZEN = (int)TokenNumber;

				if(iCountRgUpgrade_Statue >= CS_MAX_UPGRADE)
					break;

				g_iNpcUpRgLevel_CSTATUE[iCountRgUpgrade_Statue][0] = iLEVEL;
				g_iNpcUpRgLevel_CSTATUE[iCountRgUpgrade_Statue][1] = iGEMCOUNT;
				g_iNpcUpRgLevel_CSTATUE[iCountRgUpgrade_Statue][2] = iZEN;

				iCountRgUpgrade_Statue++;
			}
			else if(type == 8)
			{
				int iDFVALUE = 0;

				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				iDFVALUE = (int)TokenNumber;

				if(iCountDfValue_Gate >= CS_MAX_UPGRADE + 1)
					break;

				g_iNpcDefense_CGATE[iCountDfValue_Gate] = iDFVALUE;

				iCountDfValue_Gate++;
			}
			else if(type == 9)
			{
				int iDFVALUE = 0;

				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				iDFVALUE = (int)TokenNumber;

				if(iCountDfValue_Statue >= CS_MAX_UPGRADE + 1)
					break;

				g_iNpcDefense_CSTATUE[iCountDfValue_Statue] = iDFVALUE;

				iCountDfValue_Statue++;
			}
		}
	}

	fclose(SMDFile);

	EnterCriticalSection(&m_critScheduleData);

	if(!vtScheduleData.empty())
	{
		vector<_CS_SCHEDULE_DATA>::iterator it;
		for( it = vtScheduleData.begin(); it != vtScheduleData.end(); it++)
		{
			m_vtScheduleData.push_back(*it);
		}
	}

	sort(m_vtScheduleData.begin(),m_vtScheduleData.end(),ScheduleStateCompFunc);

	if(!m_vtScheduleData.empty())
	{
		for(DWORD iIDX = 0; iIDX < m_vtScheduleData.size(); iIDX++)
		{
			if(iIDX+1 < m_vtScheduleData.size())
			{
				int iIDX_SEC1 = m_vtScheduleData[iIDX].m_iADD_DAY * 24 * 60 * 60 + m_vtScheduleData[iIDX].m_iADD_HOUR * 60 * 60 + m_vtScheduleData[iIDX].m_iADD_MIN * 60;
				int iIDX_SEC2 = m_vtScheduleData[iIDX+1].m_iADD_DAY * 24 * 60 * 60 + m_vtScheduleData[iIDX+1].m_iADD_HOUR * 60 * 60 + m_vtScheduleData[iIDX+1].m_iADD_MIN * 60;

				int iIDX_RESULT = iIDX_SEC2 - iIDX_SEC1;

				if(iIDX_RESULT < 0)
				{
					LogAddC(2,"[CastleSiege] CCastleSiege::LoadData() - Date Order is wrong (sort fail) : %d-%d",
						m_vtScheduleData[iIDX].m_iSTATE, m_vtScheduleData[iIDX+1].m_iSTATE);
				}
				else
				{
					m_vtScheduleData[iIDX].m_iGAP_SEC = iIDX_RESULT;
				}
			}
			else
			{
				m_vtScheduleData[iIDX].m_iGAP_SEC = 0;
			}
		}
	}

	LeaveCriticalSection(&m_critScheduleData);

	EnterCriticalSection(&m_critNpcData);

	if(!vtNpcData.empty())
	{
		vector<_CS_NPC_DATA>::iterator it;
		for( it = vtNpcData.begin();it != vtNpcData.end(); it++)
		{
			m_vtNpcData.push_back(*it);
		}
	}

	LeaveCriticalSection(&m_critNpcData);

	vector<_CS_SCHEDULE_DATA>::iterator it1 = m_vtScheduleData.begin();

	while(it1 != m_vtScheduleData.end())
	{
		_CS_SCHEDULE_DATA stTEMP = *it1;
		char szTEMP[0x100] = {0};

		wsprintf(szTEMP,"STT:%d, DAY:%d, HOUR:%d, MIN:%d, GAP:%d",
			stTEMP.m_iSTATE,stTEMP.m_iADD_DAY,stTEMP.m_iADD_HOUR,stTEMP.m_iADD_MIN,stTEMP.m_iGAP_SEC);

//		OutputDebugString(szTEMP);
		it1++;
	}

	vector<_CS_NPC_DATA>::iterator it2 = m_vtNpcData.begin();

	while(it2 != m_vtNpcData.end())
	{

		_CS_NPC_DATA stTEMP = *it2;
		char szTEMP[0x100] = {0};

		wsprintf(szTEMP,"NUM:%d, IDX:%d, DBSAVE:%s, SIDE:%d, DFLEVEL:%d, RGLEVEL:%d, MXHP:%d, (%d,%d)-(%d,%d), DIR:%d",
			stTEMP.m_iNPC_NUM,stTEMP.m_iNPC_INDEX,(stTEMP.m_bNPC_DBSAVE != 0) ? "YES" : "NO",stTEMP.m_iNPC_SIDE,stTEMP.m_iNPC_DF_LEVEL,
			stTEMP.m_iNPC_RG_LEVEL,stTEMP.m_iNPC_MAXHP,stTEMP.m_iNPC_SX,stTEMP.m_iNPC_SY,stTEMP.m_iNPC_DX,stTEMP.m_iNPC_DY,stTEMP.m_iNPC_DIR);
//		OutputDebugString(szTEMP);
		it2++;
	}

	m_bFileDataLoadOK = 1;
return true;
}

void CCastleSiege::ResetAllUserCsJoinSide()
{
	EnterCriticalSection(&m_critCsTotalGuildInfo);

	for(int iUSER = OBJ_STARTUSERINDEX; iUSER < OBJMAX; iUSER++)
	{
		if(::gObjIsConnected(iUSER) == FALSE)
			continue;

		gObj[iUSER].m_btCsJoinSide = 0;
		gObj[iUSER].m_bCsGuildInvolved = 0;
		gObj[iUSER].m_btLifeStoneCount = 0;

		NotifySelfCsJoinSide(iUSER);

		gObj[iUSER].m_iAccumulatedCrownAccessTime = 0;
	}

	LeaveCriticalSection(&m_critCsTotalGuildInfo);
}

struct PMSG_ANS_NOTIFYCSJOINSIDE
{
	PBMSG_HEAD2 h;
	unsigned char btCsJoinSide;
};

void CCastleSiege::NotifySelfCsJoinSide(int iIndex)
{
	if(gObjIsConnected(iIndex) == FALSE)
		return;
	int iState = 0;
	int iViewState = 0;

	unsigned char btCsJoinSide = gObj[iIndex].m_btCsJoinSide;

	PMSG_ANS_NOTIFYCSJOINSIDE pMsg;

	if(btCsJoinSide < 0)
	{
		btCsJoinSide = 0;
	}

	if(btCsJoinSide > 4)
	{
		btCsJoinSide = 0;
	}

	switch(btCsJoinSide)
	{
	case 1:
		iState = 1;
		iViewState = 0x40000;

		if( m_iCastleSiegeState == CASTLESIEGE_STATE_STARTSIEGE ||
			m_iCastleSiegeState == CASTLESIEGE_STATE_ENDSIEGE )
		{
			gObjAddBuffEffect(&gObj[iIndex],BUFF_WATCHTOWER,0,0,0,0,-10);
		}
		else
		{
			gObjRemoveBuffEffect(&gObj[iIndex],BUFF_WATCHTOWER);
		}
		break;
	case 2:
		iState = 1;
		iViewState = 0x80000;

		if( m_iCastleSiegeState == CASTLESIEGE_STATE_STARTSIEGE ||
			m_iCastleSiegeState == CASTLESIEGE_STATE_ENDSIEGE )
		{
			gObjAddBuffEffect(&gObj[iIndex],BUFF_SIEGEGUILD1,0,0,0,0,-10);
		}
		else
		{
			gObjRemoveBuffEffect(&gObj[iIndex],BUFF_SIEGEGUILD1);
		}
		break;
	case 3:
		iState = 1;
		iViewState = 0x400000;

		if( m_iCastleSiegeState == CASTLESIEGE_STATE_STARTSIEGE ||
			m_iCastleSiegeState == CASTLESIEGE_STATE_ENDSIEGE )
		{
			gObjAddBuffEffect(&gObj[iIndex],BUFF_SIEGEGUILD2,0,0,0,0,-10);
		}
		else
		{
			gObjRemoveBuffEffect(&gObj[iIndex],BUFF_SIEGEGUILD2);
		}
		break;
	case 4:
		iState = 1;
		iViewState = 0x800000;

		if( m_iCastleSiegeState == CASTLESIEGE_STATE_STARTSIEGE ||
			m_iCastleSiegeState == CASTLESIEGE_STATE_ENDSIEGE )
		{
			gObjAddBuffEffect(&gObj[iIndex],BUFF_SIEGEGUILD3,0,0,0,0,-10);
		}
		else
		{
			gObjRemoveBuffEffect(&gObj[iIndex],BUFF_SIEGEGUILD3);
		}
		break;
	case 0:
		iState = 0;
		iViewState = 0xC0000;
		gObjClearBuffEffect(&gObj[iIndex],CLEAR_TYPE_CASTLESIEGE);
		break;
	}


	pMsg.h.set((PBYTE)&pMsg,0xB2,0x19,sizeof(pMsg));

	if(btCsJoinSide == 2)
	{
		pMsg.btCsJoinSide = gObj[iIndex].m_btCsJoinSide;
	}
	else
	{
		pMsg.btCsJoinSide = btCsJoinSide;
	}

	DataSend(iIndex,(PBYTE)&pMsg,pMsg.h.size);
}

void CCastleSiege::ClearAllNPC()
{
	for(int n = 0; n < OBJ_MAXMONSTER; n++)
	{
		if(gObj[n].m_btCsNpcType)
		{
			gObjDel(n);
		}
	}
}

void CCastleSiege::GetNextDay(_SYSTEMTIME * st,int iAfterDay,int iAfterHour,int iAfterMin,int iAfterSec)
{
	__int64 i64;
	_FILETIME fst;
	st->wDayOfWeek = 0;
	st->wMilliseconds = 0;
	st->wSecond = 0;
	SystemTimeToFileTime(st,&fst);
	i64 = ((__int64)fst.dwHighDateTime << 0x20) + fst.dwLowDateTime;
	i64 = i64 + (__int64)iAfterDay * 864000000000 + (__int64)iAfterHour * 36000000000 +(__int64) iAfterMin * 600000000 + (__int64)iAfterSec * 10000000;
	fst.dwHighDateTime = i64 >> 0x20;
	fst.dwLowDateTime = i64 & 0xFFFFFFFF;
	FileTimeToSystemTime(&fst,st);
}

int CCastleSiege::DataRequest()
{
	if(m_bFileDataLoadOK == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::DataRequest() - m_bFileDataLoadOK == FALSE");
		return false;
	}

	if(m_iMapSvrGroup < 0)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::DataRequest() - m_iMapSvrGroup < 0");
		return false;
	}

	if(m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_2)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::DataRequest() - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_2 (%d)",
			m_iCastleDataLoadState);
		return false;
	}

	int bDbRequestData = FALSE;

	if(m_dwDbDataLoadTickCount == 0)
	{
		bDbRequestData = TRUE;
		m_dwDbDataLoadTickCount = GetTickCount();
		LogAddC(3, "[CastleSiege] CCastleSiege::DataRequest() - REQUEST DATA FIRST");
	}
	else if(GetTickCount() - m_dwDbDataLoadTickCount > 3000)
	{
		bDbRequestData = TRUE;
		m_dwDbDataLoadTickCount = GetTickCount();
		LogAddC(3, "[CastleSiege] CCastleSiege::DataRequest() - REQUEST DATA AGAIN");
	}

	if(bDbRequestData != FALSE)
	{
		GS_GDReqCastleInitData(m_iMapSvrGroup,m_iCastleSiegeCycle);
	}
return true;
}

int CCastleSiege::Init()
{

	if(m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_4)
	{
		LogAddC(2, "[CastleSiege] CCastleSiege::Init() - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_4 (%d)",
			m_iCastleDataLoadState);
		MsgBox("[CastleSiege] CCastleSiege::Init() - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_4 (%d)",
			m_iCastleDataLoadState);
		return false;
	}

	if(m_bFileDataLoadOK == 0 || m_bDbDataLoadOK == 0)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::Init() - Data Load Fail (FILE:%d, DB:%d)",
			m_bFileDataLoadOK, m_bDbDataLoadOK);
		MsgBox("[CastleSiege] CCastleSiege::Init() - Data Load Fail (FILE:%d, DB:%d)",
			m_bFileDataLoadOK, m_bDbDataLoadOK);
		return false;
	}

	LogAddTD("[CastleSiege] CCastleSiege::Init() - START");

	m_iCastleDataLoadState = 0;

	CreateAllCsGateLever();

	if(CheckSync() == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::Init() - CheckSync() == FALSE");
		//MsgBox("[CastleSiege] CCastleSiege::Init() - CheckSync() == FALSE");
		return false;
	}

	m_bDoRun = TRUE;

	m_dwNPC_DBSAVE_TICK_COUNT = GetTickCount();
	m_dwCALC_LEFTSIEGE_TICK_COUNT = 0;
	m_dwEVENT_MSG_TICK_COUNT = 0;
	m_dwCHECK_GATE_ALIVE_COUNT = 0;
	m_dwCS_STARTTIME_TICK_COUNT = 0;
	m_dwCS_LEFTTIME_TICK_COUNT = 0;
	m_dwCS_JOINSIDE_REFRESH_TICK_COUNT = 0;
	m_dwCS_JOINSIDE_REFRESH_TOTAL_COUNT = 0;
	m_dwCS_MINIMAP_SEND_TICK_COUNT = 0;
	return true;
}

void CCastleSiege::CreateAllCsGateLever()
{
	EnterCriticalSection(&m_critNpcData);

	if(!m_vtNpcData.empty())
	{
		vector<_CS_NPC_DATA>::iterator it;
		for(it = m_vtNpcData.begin(); it != m_vtNpcData.end(); it++)
		{
			_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

			if(pNpcData.m_iNPC_NUM == 277)
			{
				if(pNpcData.m_iCS_GATE_LEVER_INDEX != -1 && CheckLeverAlive(pNpcData.m_iCS_GATE_LEVER_INDEX) == 1)
				{
					continue;
				}

				int iLeverIndex = CreateCsGateLever(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY + 4);

				if(OBJMAX_RANGE(iLeverIndex))
				{
					it->m_iCS_GATE_LEVER_INDEX = iLeverIndex;
				}
				else
				{
					it->m_iCS_GATE_LEVER_INDEX = -1;
					LogAddC(2,"[CastleSiege] CCastleSiege::CreateAllCsGateLever() ERROR - Failed to Create Gate Lever : GateIndex:%d",
						pNpcData.m_iNPC_INDEX);
				}
			}
		}
	}

	LeaveCriticalSection(&m_critNpcData);
}

#define XY_MACRO(value) ( (( (value)   )<0)?FALSE:(( (value)  )>255)?FALSE:TRUE  )

int CCastleSiege::CreateCsGateLever(int iX,int iY)
{
	if(XY_MACRO(iX) == FALSE || XY_MACRO(iY) == FALSE)
	{
		return -1;
	}

	int iNPC_INDEX = gObjAddMonster(MAP_INDEX_CASTLESIEGE);

	if(iNPC_INDEX >= 0)
	{
		gObjSetMonster(iNPC_INDEX,219);

		gObj[iNPC_INDEX].m_PosNum = -1;
		gObj[iNPC_INDEX].X = iX;
		gObj[iNPC_INDEX].Y = iY;
		gObj[iNPC_INDEX].MapNumber = MAP_INDEX_CASTLESIEGE;
		gObj[iNPC_INDEX].TX = gObj[iNPC_INDEX].X;
		gObj[iNPC_INDEX].TY = gObj[iNPC_INDEX].Y;
		gObj[iNPC_INDEX].m_OldX = gObj[iNPC_INDEX].X;
		gObj[iNPC_INDEX].m_OldY = gObj[iNPC_INDEX].Y;
		gObj[iNPC_INDEX].Dir = 1;
		gObj[iNPC_INDEX].StartX = gObj[iNPC_INDEX].X;
		gObj[iNPC_INDEX].StartY = gObj[iNPC_INDEX].Y;
		gObj[iNPC_INDEX].DieRegen = 0;
		gObj[iNPC_INDEX].MaxRegenTime = 0;
		gObj[iNPC_INDEX].Life = 10000.0f;
		gObj[iNPC_INDEX].MaxLife = 10000.0f;
		gObj[iNPC_INDEX].m_btCsNpcType = 2;
		gObj[iNPC_INDEX].m_iCsGateLeverLinkIndex = -1;
	return iNPC_INDEX;
	}	
return -1;
}

int CCastleSiege::CheckLeverAlive(int iLeverIndex)
{
	if(gObjIsConnected(iLeverIndex) == FALSE || gObj[iLeverIndex].Class != 219 )
	{
		return false;
	}
	return true;
}

int CCastleSiege::CheckSync()
{
	if(m_bFileDataLoadOK == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::CheckSync() - m_bFileDataLoadOK == FALSE");
		return false;
	}

	m_iCS_TICK_COUNT = GetTickCount();

	 _SYSTEMTIME tmToDay;

	 GetLocalTime(&tmToDay);
	 int iTODAY_DATE_NUM = MACRO2(( MACRO1(tmToDay.wDay) | MACRO1(tmToDay.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmToDay.wYear) << 16;
	 int iCastleInitState = 0;

	 if(m_bFixCastleCycleStartDate != 0)
	 {
		 m_tmStartDate.wYear = m_tmFixCastleCycleStartDate.wYear;
		 m_tmStartDate.wMonth = m_tmFixCastleCycleStartDate.wMonth;
		 m_tmStartDate.wDay = m_tmFixCastleCycleStartDate.wDay;

		 m_tmEndDate = m_tmStartDate;
		 GetNextDay(&m_tmEndDate,m_iCastleSiegeCycle,0,0,0);
	 }

	 int iEVENT_END_DATE_NUM = MACRO2(( MACRO1(m_tmEndDate.wDay) | MACRO1(m_tmEndDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(m_tmEndDate.wYear) << 16;

	 if(iEVENT_END_DATE_NUM <= iTODAY_DATE_NUM)
	 {
		 m_bFixCastleCycleStartDate = 0;
		 LogAddC(2,"[CastleSiege] CCastleSiege::CheckSync() - iEVENT_END_DATE_NUM (%04d-%02d-%02d) <= iTODAY_DATE_NUM (%04d-%02d-%02d)",
			 m_tmEndDate.wYear,m_tmEndDate.wMonth,m_tmEndDate.wDay,tmToDay.wYear,tmToDay.wMonth,tmToDay.wDay);
		 return false;
	 }

	 if(m_bFixCastleCycleStartDate != 0)
	 {
		 GS_GDReqSiegeDateChange(m_iMapSvrGroup,-1,m_tmStartDate.wYear,(BYTE)m_tmStartDate.wMonth,(BYTE)m_tmStartDate.wDay,
			 m_tmEndDate.wYear,(BYTE)m_tmEndDate.wMonth,(BYTE)m_tmEndDate.wDay);
	 }

	 if(m_bFixCastleStateStartDate != 0)
	 {
		 int bSpecificStateExist = FALSE;

		 EnterCriticalSection(&m_critScheduleData);

		 vector<_CS_SCHEDULE_DATA>::iterator it = m_vtScheduleData.begin();

		 while(it != m_vtScheduleData.end())
		 {
			 _CS_SCHEDULE_DATA & pScheduleData = (_CS_SCHEDULE_DATA &)*it;

			 if(pScheduleData.m_iSTATE == m_iFixCastleSpecificState)
			 {
				 bSpecificStateExist = TRUE;
				 break;
			 }

			 it++;
		 }
		 LeaveCriticalSection(&m_critScheduleData);

		 if(bSpecificStateExist != FALSE)
		 {
			 long ttSTime;
			 long ttETime;
			 tm tmETime;
			 double dResultSecond;
			 memset(&tmETime,0x00,sizeof(tmETime));

			 tmETime.tm_year = m_tmFixCastleStateStartDate.wYear - 1900;
			 tmETime.tm_mon = m_tmFixCastleStateStartDate.wMonth - 1;
			 tmETime.tm_mday = m_tmFixCastleStateStartDate.wDay;
			 tmETime.tm_hour = m_tmFixCastleStateStartDate.wHour;
			 tmETime.tm_min = m_tmFixCastleStateStartDate.wMinute;

			 time((time_t *)&ttSTime);
			 ttETime = (long)mktime(&tmETime);

			 dResultSecond = difftime(ttETime,ttSTime);

			 m_iCS_REMAIN_MSEC = (int)(dResultSecond * 1000.0);
			 m_iCastleSiegeState = m_iFixCastleSpecificState -1;
			 SetState(m_iCastleSiegeState,0);
			 return true;
		 }
		 else
		 {
			 LogAddC(2,"[CastleSiege] CCastleSiege::CheckSync() - FIXED STATE:%d NOT FOUND IN SCHEDULE (P.S.> Check File 'MuCastleData.dat', 'commonserver.cfg')",m_iFixCastleSpecificState);
			 m_bFixCastleStateStartDate = 0;
		 }
	 }

	 if(m_btIsSiegeEnded)
	 {
		 SetState(CASTLESIEGE_STATE_ENDSIEGE,1);
		 return true;
	 }

	 int iEVENT_START_DATE_NUM = MACRO2(( MACRO1(m_tmStartDate.wDay) | MACRO1(m_tmStartDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(m_tmStartDate.wYear) << 16;

	 if(iEVENT_START_DATE_NUM > iTODAY_DATE_NUM)
	 {
		 SetState(CASTLESIEGE_STATE_IDLE_1,1);
		 return true;
	 }
	 else
	 {
		 int bStateSetted = 0;
		_CS_SCHEDULE_DATA pScheData_FR;
		_CS_SCHEDULE_DATA pScheData_RR;
		vector<_CS_SCHEDULE_DATA>::iterator it;

		 EnterCriticalSection(&m_critScheduleData);

		 it = m_vtScheduleData.begin();

		 while(it != m_vtScheduleData.end())
		 {
			_CS_SCHEDULE_DATA & pScheduleData = (_CS_SCHEDULE_DATA &)*it;
			_SYSTEMTIME tmSchduleDate;

			tmSchduleDate = m_tmStartDate;
			GetNextDay(&tmSchduleDate,pScheduleData.m_iADD_DAY,0,0,0);
			tmSchduleDate.wHour = pScheduleData.m_iADD_HOUR;
			tmSchduleDate.wMinute = pScheduleData.m_iADD_MIN;

			__int64 i64ScheduleDateNum;
			__int64 i64ToDayDateNum;

			i64ScheduleDateNum = (MACRO2(tmSchduleDate.wMinute) | MACRO2(tmSchduleDate.wHour) << 16) | ((__int64)(MACRO2(( MACRO1(tmSchduleDate.wDay) | MACRO1(tmSchduleDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmSchduleDate.wYear) << 16) << 0x20);
			i64ToDayDateNum = (MACRO2(tmToDay.wMinute) | MACRO2(tmToDay.wHour) << 16) | ((__int64)(MACRO2(( MACRO1(tmToDay.wDay) | MACRO1(tmToDay.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmToDay.wYear) << 16) << 0x20);

			 if(i64ScheduleDateNum > i64ToDayDateNum)
			 {
				 bStateSetted = 1;
				 pScheData_RR = pScheduleData;
				 m_iCastleSiegeState = iCastleInitState;
				 break;
			 }

			 iCastleInitState = pScheduleData.m_iSTATE;
			 pScheData_FR = pScheduleData;

			 it++;
		 }
		 LeaveCriticalSection(&m_critScheduleData);


		 _SYSTEMTIME tmSchduleDate;
	
		long ttSTime;
		long ttETime;
		struct tm tmETime;
		double dResultSecond;

		if(bStateSetted == FALSE)
		{
			MsgBox("[CastleSiege] CCastleSiege::CheckSync() - bStateSetted == FALSE  START_DATE (%04d-%02d-%02d), END_DATE (%04d-%02d-%02d)",
				m_tmStartDate.wYear,m_tmStartDate.wMonth,m_tmStartDate.wDay,m_tmEndDate.wYear,m_tmEndDate.wMonth,m_tmEndDate.wDay);
			return false;
		}

		tmSchduleDate = m_tmStartDate;
		GetNextDay(&tmSchduleDate,pScheData_RR.m_iADD_DAY,0,0,0);
		tmSchduleDate.wHour = pScheData_RR.m_iADD_HOUR;
		tmSchduleDate.wMinute = pScheData_RR.m_iADD_MIN;

		memset(&tmETime,0x00,sizeof(tmETime));

		tmETime.tm_year = tmSchduleDate.wYear - 1900;
		tmETime.tm_mon = tmSchduleDate.wMonth - 1;
		tmETime.tm_mday = tmSchduleDate.wDay;
		tmETime.tm_hour = tmSchduleDate.wHour;
		tmETime.tm_min = tmSchduleDate.wMinute;

		time((time_t*)&ttSTime);
		ttETime = (long)mktime(&tmETime);

		dResultSecond = difftime(ttETime,ttSTime);
		m_iCS_REMAIN_MSEC = (int)(dResultSecond * 1000.0);

		if(m_iCastleSiegeState == CASTLESIEGE_STATE_STARTSIEGE)
		{
			m_iCastleSiegeState = CASTLESIEGE_STATE_READYSIEGE;
			m_iCS_REMAIN_MSEC = 900000;
		}
		else if(m_iCastleSiegeState == CASTLESIEGE_STATE_READYSIEGE)
		{
			if(m_iCS_REMAIN_MSEC < 900000)
			{
				m_iCastleSiegeState = CASTLESIEGE_STATE_READYSIEGE;
				m_iCS_REMAIN_MSEC = 900000;
			}
		}

		SetState(m_iCastleSiegeState,0);
	 }

return true;
}

int CCastleSiege::LinkCsGateLever(int iLeverIndex,int iGateIndex)
{
	if(!gObjIsConnected(iLeverIndex) || gObj[iLeverIndex].Class != 219)
		return false;
	if(!gObjIsConnected(iGateIndex) || gObj[iGateIndex].Class != 277)
		return false;

	gObj[iLeverIndex].m_iCsGateLeverLinkIndex = iGateIndex;
	gObj[iGateIndex].m_iCsGateLeverLinkIndex = iLeverIndex;
return true;
}

void CCastleSiege::SetGateBlockState(int iX,int iY,int iGateState)
{
	if(XY_MACRO(iX) == FALSE || XY_MACRO(iY) == FALSE)
		return;

	switch(iGateState)
	{
	case 0:
		{
			for(int i = iX -2; i <= iX + 3; i++)
			{
				for(int j = iY; j <= iY + 2;j++)
				{
					MapC[MAP_INDEX_CASTLESIEGE].m_attrbuf[j * 256 + i] |= 16;
				}
			}
		}
		break;
	case 1:
		{
			for(int i = iX -2; i <= iX + 3; i++)
			{
				for(int j = iY; j <= iY + 2;j++)
				{
					MapC[MAP_INDEX_CASTLESIEGE].m_attrbuf[j * 256 + i] &= ~16;
				}
			}
		}
		break;
	}
}

void CCastleSiege::CreateDbNPC()
{
	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();
	int bExist = 0;

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_bIN_USE == 1 && pNpcData.m_iNPC_LIVE == 1)
		{
			int iNPC_INDEX = gObjAddMonster(MAP_INDEX_CASTLESIEGE);

			if(iNPC_INDEX >= 0)
			{
				gObjSetMonster(iNPC_INDEX,pNpcData.m_iNPC_NUM);
				gObj[iNPC_INDEX].m_PosNum = -1;
				gObj[iNPC_INDEX].X = pNpcData.m_iNPC_SX;
				gObj[iNPC_INDEX].Y = pNpcData.m_iNPC_SY;
				gObj[iNPC_INDEX].MapNumber = MAP_INDEX_CASTLESIEGE;
				gObj[iNPC_INDEX].TX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].TY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].m_OldX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].m_OldY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].Dir = pNpcData.m_iNPC_DIR;
				gObj[iNPC_INDEX].StartX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].StartY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].DieRegen = 0;
				gObj[iNPC_INDEX].MaxRegenTime = 0;
				gObj[iNPC_INDEX].Life = (float)pNpcData.m_iNPC_HP;
				gObj[iNPC_INDEX].MaxLife = (float)pNpcData.m_iNPC_MAXHP;
				gObj[iNPC_INDEX].m_btCsNpcType = 1;

				int iNPC_DF_LEVEL = pNpcData.m_iNPC_DF_LEVEL;

				if(iNPC_DF_LEVEL < 0)
				{
					iNPC_DF_LEVEL = 0;
				}

				if(iNPC_DF_LEVEL > CS_MAX_UPGRADE)
				{
					iNPC_DF_LEVEL = CS_MAX_UPGRADE;
				}

				int iNPC_RG_LEVEL = pNpcData.m_iNPC_RG_LEVEL;

				if(iNPC_RG_LEVEL < 0)
				{
					iNPC_RG_LEVEL = 0;
				}

				if(iNPC_RG_LEVEL > CS_MAX_UPGRADE)
				{
					iNPC_RG_LEVEL = CS_MAX_UPGRADE;
				}

				switch(pNpcData.m_iNPC_NUM)
				{
				case 277:
					gObj[iNPC_INDEX].m_Defense = g_iNpcDefense_CGATE[iNPC_DF_LEVEL];
					gObj[iNPC_INDEX].m_MagicDefense = g_iNpcDefense_CGATE[iNPC_DF_LEVEL];
					gObj[iNPC_INDEX].m_btCsNpcDfLevel = iNPC_DF_LEVEL;
					break;
				case 283:
					gObj[iNPC_INDEX].m_Defense = g_iNpcDefense_CSTATUE[iNPC_DF_LEVEL];
					gObj[iNPC_INDEX].m_MagicDefense = g_iNpcDefense_CSTATUE[iNPC_DF_LEVEL];
					gObj[iNPC_INDEX].m_btCsNpcDfLevel = iNPC_DF_LEVEL;
					gObj[iNPC_INDEX].m_btCsNpcRgLevel = iNPC_RG_LEVEL;
					break;
				}

				it->m_btCsNpcExistVal1 = rand()%255+1;
				gObj[iNPC_INDEX].m_btCsNpcExistVal1 = it->m_btCsNpcExistVal1;

				it->m_btCsNpcExistVal2 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal2 = it->m_btCsNpcExistVal2;

				it->m_btCsNpcExistVal3 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal3 = it->m_btCsNpcExistVal3;

				it->m_btCsNpcExistVal4 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal4 = it->m_btCsNpcExistVal4;

				if(pNpcData.m_iNPC_NUM == 277)
				{
					SetGateBlockState(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY,1);
					gObjAddBuffEffect(&gObj[iNPC_INDEX],BUFF_GATESTATUS,0,0,0,0,-10);
					gObj[iNPC_INDEX].m_btCsGateOpen = TRUE;
					int bCreateLever = FALSE;

					if(pNpcData.m_iCS_GATE_LEVER_INDEX == -1)
					{
						bCreateLever = TRUE;
					}
					else if(CheckLeverAlive(pNpcData.m_iCS_GATE_LEVER_INDEX) == FALSE)
					{
						bCreateLever = TRUE;
					}

					if(bCreateLever != FALSE)
					{
						int iLeverIndex = CreateCsGateLever(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY+4);

						if(OBJMAX_RANGE(iLeverIndex))
						{
							it->m_iCS_GATE_LEVER_INDEX = iLeverIndex;

							int bRETVAL = LinkCsGateLever(iLeverIndex,iNPC_INDEX);

							if(bRETVAL == FALSE)
							{
								LogAddC(2,"[CastleSiege] CCastleSiege::CreateDbNPC() ERROR - Failed to Create Gate Lever : GateIndex:%d",pNpcData.m_iNPC_INDEX);
							}
						}
						else
						{
							it->m_iCS_GATE_LEVER_INDEX = -1;
							LogAddC(2,"[CastleSiege] CCastleSiege::CreateDbNPC() ERROR - Failed to Create Gate Lever : GateIndex:%d",pNpcData.m_iNPC_INDEX);
						}
					}
					else
					{
						int bRETVAL = LinkCsGateLever(pNpcData.m_iCS_GATE_LEVER_INDEX,iNPC_INDEX);

						if(bRETVAL == FALSE)
						{
							LogAddC(2,"[CastleSiege] CCastleSiege::CreateDbNPC() ERROR - Failed to Create Gate Lever : GateIndex:%d",pNpcData.m_iNPC_INDEX);
						}
					}
				}

				it->m_iNPC_OBJINDEX = iNPC_INDEX;
				it->m_iNPC_LIVE = 2;
			}
		}

		it++;
	}
	LeaveCriticalSection(&m_critNpcData);
}

void CCastleSiege::SetState(int iCastleSiegeState,int bSetRemainMsec)
{
	if(iCastleSiegeState < CASTLESIEGE_STATE_NONE || iCastleSiegeState > CASTLESIEGE_STATE_ENDCYCLE)
		return;

	m_iCastleSiegeState = iCastleSiegeState;

	switch(m_iCastleSiegeState)
	{
	case CASTLESIEGE_STATE_NONE: SetState_NONE(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_IDLE_1: SetState_IDLE_1(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_REGSIEGE: SetState_REGSIEGE(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_IDLE_2: SetState_IDLE_2(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_REGMARK: SetState_REGMARK(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_IDLE_3: SetState_IDLE_3(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_NOTIFY: SetState_NOTIFY(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_READYSIEGE: SetState_READYSIEGE(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_STARTSIEGE: SetState_STARTSIEGE(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_ENDSIEGE: SetState_ENDSIEGE(bSetRemainMsec); break;
	case CASTLESIEGE_STATE_ENDCYCLE: SetState_ENDCYCLE(bSetRemainMsec); break;
	}

	if(m_bDbNpcCreated == FALSE)
	{
		m_bDbNpcCreated = TRUE;
		CreateDbNPC();
	}

	GetLocalTime(&m_tmStateStartDate);
	m_dwEVENT_MSG_TICK_COUNT = 0;
	m_dwCHECK_GATE_ALIVE_COUNT = 0;
}

void CCastleSiege::SetState_NONE(int bSetRemainMsec)
{

}

void CCastleSiege::SetState_IDLE_1(int bSetRemainMsec)
{
	if(bSetRemainMsec != 0) {
		__int64 ttSTime;
		__int64 ttETime;
		tm tmETime;
		double dResultSecond;

		memset(&tmETime, 0x00, sizeof(tmETime));
		tmETime.tm_year = m_tmStartDate.wYear - 1900;
		tmETime.tm_mon = m_tmStartDate.wMonth - 1;
		tmETime.tm_mday = m_tmStartDate.wDay;
		ttETime = mktime(&tmETime);
		time(&ttSTime);

		dResultSecond = difftime(ttETime, ttSTime);
		m_iCS_REMAIN_MSEC = (int)(dResultSecond * 1000.0);
	}
}

void CCastleSiege::SetState_REGSIEGE(int bSetRemainMsec)
{
	if(bSetRemainMsec != FALSE)
	{
		int iGAP_SEC = GetStateGapSec(CASTLESIEGE_STATE_REGSIEGE);

		if(iGAP_SEC < 0)
		{
			m_bDoRun = 0;
			MsgBox("[CastleSiege] CCastleSiege::SetState_REGSIEGE() - iGAP_SEC < 0");
		}

		m_iCS_REMAIN_MSEC = iGAP_SEC * 1000;
	}

	GS_GDReqRestartCastleState(m_iMapSvrGroup);
}

void CCastleSiege::SetState_IDLE_2(int bSetRemainMsec)
{
	if(bSetRemainMsec != FALSE)
	{
		int iGAP_SEC = GetStateGapSec(CASTLESIEGE_STATE_IDLE_2);

		if(iGAP_SEC < 0)
		{
			m_bDoRun = 0;
			MsgBox("[CastleSiege] CCastleSiege::SetState_IDLE_2() - iGAP_SEC < 0");
		}

		m_iCS_REMAIN_MSEC = iGAP_SEC * 1000;
	}
}

void CCastleSiege::SetState_REGMARK(int bSetRemainMsec)
{
	if(bSetRemainMsec != FALSE)
	{
		int iGAP_SEC = GetStateGapSec(CASTLESIEGE_STATE_REGMARK);

		if(iGAP_SEC < 0)
		{
			m_bDoRun = 0;
			MsgBox("[CastleSiege] CCastleSiege::SetState_REGMARK() - iGAP_SEC < 0");
		}

		m_iCS_REMAIN_MSEC = iGAP_SEC * 1000;
	}
}

void CCastleSiege::SetState_IDLE_3(int bSetRemainMsec)
{
	if(bSetRemainMsec != FALSE)
	{
		int iGAP_SEC = GetStateGapSec(CASTLESIEGE_STATE_IDLE_3);

		if(iGAP_SEC < 0)
		{
			m_bDoRun = 0;
			MsgBox("[CastleSiege] CCastleSiege::SetState_IDLE_3() - iGAP_SEC < 0");
		}

		m_iCS_REMAIN_MSEC = iGAP_SEC * 1000;
	}
}

void CCastleSiege::SetState_NOTIFY(int bSetRemainMsec)
{
	if(bSetRemainMsec != FALSE)
	{
		int iGAP_SEC = GetStateGapSec(CASTLESIEGE_STATE_NOTIFY);

		if(iGAP_SEC < 0)
		{
			m_bDoRun = 0;
			MsgBox("[CastleSiege] CCastleSiege::SetState_NOTIFY() - iGAP_SEC < 0");
		}

		m_iCS_REMAIN_MSEC = iGAP_SEC * 1000;
	}

	CheckBuildCsGuildInfo();
}

void CCastleSiege::SetState_READYSIEGE(int bSetRemainMsec)
{
	if(bSetRemainMsec != FALSE)
	{
		int iGAP_SEC = GetStateGapSec(CASTLESIEGE_STATE_READYSIEGE);

		if(iGAP_SEC < 0)
		{
			m_bDoRun = 0;
			MsgBox("[CastleSiege] CCastleSiege::SetState_READYSIEGE() - iGAP_SEC < 0");
		}

		m_iCS_REMAIN_MSEC = iGAP_SEC * 1000;
	}

	CheckBuildCsGuildInfo();
}

void CCastleSiege::SetState_STARTSIEGE(int bSetRemainMsec)
{
	if(bSetRemainMsec != FALSE)
	{
		int iGAP_SEC = GetStateGapSec(CASTLESIEGE_STATE_STARTSIEGE);

		if(iGAP_SEC < 0)
		{
			m_bDoRun = 0;
			MsgBox("[CastleSiege] CCastleSiege::SetState_STARTSIEGE() - iGAP_SEC < 0");
		}

		m_iCS_REMAIN_MSEC = iGAP_SEC * 1000;
	}

	m_bCastleTowerAccessable = FALSE;
	m_bRegCrownAvailable = FALSE;
	m_iCastleCrownAccessUser = -1;
	m_btCastleCrownAccessUserX = 0;
	m_btCastleCrownAccessUserY = 0;
	m_iCastleSwitchAccessUser1 = -1;
	m_iCastleSwitchAccessUser2 = -1;
	m_dwCrownAccessTime = 0;
	m_dwCHECK_GATE_ALIVE_COUNT = 0;
	m_dwCS_STARTTIME_TICK_COUNT = 0;
	m_dwCS_LEFTTIME_TICK_COUNT = GetTickCount();
	m_dwCS_JOINSIDE_REFRESH_TICK_COUNT = GetTickCount();
	m_dwCS_JOINSIDE_REFRESH_TOTAL_COUNT = 0;
	m_dwCS_MINIMAP_SEND_TICK_COUNT = GetTickCount();

	memset(m_szMiddleWinnerGuild,0x00,sizeof(m_szMiddleWinnerGuild));

	if(CheckAttackGuildExist() == FALSE)
	{
		CheckCastleSiegeResult();
		SetState(CASTLESIEGE_STATE_ENDSIEGE,TRUE);
		LogAddC(2,"[CastleSiege] CCastleSiege::SetState_STARTSIEGE() - CheckAttackGuildExist() == FALSE");
	}
	else
	{
		StoreDbNpc();
		ClearNonDbNPC();
		CheckReviveNonDbNPC();

		if(CheckGuardianStatueExist() == FALSE)
		{
			ClearCastleTowerBarrier();
			SetCastleTowerAccessable(TRUE);
		}

		AdjustDbNpcLevel();
		SetAllCastleGateState(FALSE);
		SetAllUserCsJoinSide();
		SavePcRoomUserList();
		ReSpawnEnemyUser(TRUE);
		NotifyAllUserCsStartState(1);
	}
}

void CCastleSiege::SetState_ENDSIEGE(int bSetRemainMsec)
{
	if(bSetRemainMsec != FALSE)
	{
		long ttSTime;
		long ttETime;
		tm tmETime;
		double dResultSecond;

		memset(&tmETime,0x00,sizeof(tmETime));

		tmETime.tm_year = m_tmEndDate.wYear - 1900;
		tmETime.tm_mon = m_tmEndDate.wMonth - 1;
		tmETime.tm_mday = m_tmEndDate.wDay;

		time((time_t*)&ttSTime);
		ttETime = (long)mktime(&tmETime);

		dResultSecond = difftime(ttETime,ttSTime);
		m_iCS_REMAIN_MSEC = (int)(dResultSecond*1000.0);
	}

	ClearNonDbNPC();
	ResetAllUserCsJoinSide();
	SendAllUserAnyMsg(lMsg.Get(MSGGET(6, 84)),1);
}

/*typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
}*/

void CCastleSiege::SetState_ENDCYCLE(int bSetRemainMsec)
{
	_SYSTEMTIME tmNowDate;
	GetLocalTime(&tmNowDate);

	//tmNowDate.wDay = 0;
	tmNowDate.wHour = 0;
	tmNowDate.wMinute = 0;
	tmNowDate.wSecond = 0;
	tmNowDate.wMilliseconds = 0;

	tmNowDate = m_tmNowDate;
	m_tmStartDate = m_tmNowDate;

	GetNextDay(&tmNowDate,m_iCastleSiegeCycle,0,0,0);
	GetNextDay(&m_tmNowDate,m_iCastleSiegeCycle,0,0,0);

	m_tmEndDate = tmNowDate;

	LogAddTD("[CastleSiege] State - End Cycle Schedule : (%d-%d-%d(%d:%d:%d)) (%d-%d-%d(%d:%d:%d))",
		m_tmStartDate.wYear,m_tmStartDate.wMonth,m_tmStartDate.wDay,m_tmStartDate.wHour,m_tmStartDate.wMinute,m_tmStartDate.wSecond,
		m_tmEndDate.wYear,m_tmEndDate.wMonth,m_tmEndDate.wDay,m_tmEndDate.wHour,m_tmEndDate.wMinute,m_tmEndDate.wSecond);

	LogAddTD("[CastleSiege] State - End Cycle : Date-Changing Info (%d-%d-%d) (%d-%d-%d)",
		m_tmStartDate.wYear,m_tmStartDate.wMonth,m_tmStartDate.wDay,
		m_tmEndDate.wYear,m_tmEndDate.wMonth,m_tmEndDate.wDay);

	GS_GDReqSiegeDateChange(m_iMapSvrGroup,-1,m_tmStartDate.wYear,(BYTE)m_tmStartDate.wMonth,
		(BYTE)m_tmStartDate.wDay,m_tmEndDate.wYear,(BYTE)m_tmEndDate.wMonth,(BYTE)m_tmEndDate.wDay);
	GS_GDReqResetRegSiegeInfo(m_iMapSvrGroup);
	GS_GDReqResetSiegeGuildInfo(m_iMapSvrGroup);
	SetState(CASTLESIEGE_STATE_REGSIEGE,1);
}

int CCastleSiege::GetStateGapSec(int iCastleSiegeState)
{
	int iGAP_SEC = -1;

	EnterCriticalSection(&m_critScheduleData);

	vector<_CS_SCHEDULE_DATA>::iterator it;

	for( it = m_vtScheduleData.begin(); it != m_vtScheduleData.end(); it++)
	{
		_CS_SCHEDULE_DATA & pScheduleData = *it;

		if(pScheduleData.m_iSTATE == iCastleSiegeState)
		{
			iGAP_SEC = pScheduleData.m_iGAP_SEC;
			break;
		}
	}

	LeaveCriticalSection(&m_critScheduleData);

	return iGAP_SEC;
}

void CCastleSiege::CheckBuildCsGuildInfo()
{
	m_bCsBasicGuildInfoLoadOK = 0;
	m_bCsTotalGuildInfoLoadOK = 0;


	EnterCriticalSection(&m_critCsBasicGuildInfo);
	m_mapCsBasicGuildInfo.clear();
	LeaveCriticalSection(&m_critCsBasicGuildInfo);

	EnterCriticalSection(&m_critCsTotalGuildInfo);
	m_mapCsTotalGuildInfo.clear();
	LeaveCriticalSection(&m_critCsTotalGuildInfo);

	if(m_btIsSiegeGuildList == TRUE)
	{
		GS_GDReqCsLoadTotalGuildInfo(m_iMapSvrGroup);
	}
	else
	{
		GS_GDReqCalcRegGuildList(m_iMapSvrGroup);
	}
}

int CCastleSiege::CheckAttackGuildExist()
{
	int iRETVAL = FALSE;

	EnterCriticalSection(&m_critCsTotalGuildInfo);
	if(!m_mapCsTotalGuildInfo.empty())
	{
		iRETVAL = TRUE;
	}
	LeaveCriticalSection(&m_critCsTotalGuildInfo);

return iRETVAL;
}

int CCastleSiege::CheckCastleSiegeResult()
{
	char szMsg[0x100] = {0};
	int bRETVAL = FALSE;

	if(m_btIsCastleOccupied == TRUE && strcmp(m_szCastleOwnerGuild,"") != FALSE)
	{
		if(strcmp(m_szMiddleWinnerGuild,"") == FALSE || strcmp(m_szMiddleWinnerGuild,m_szCastleOwnerGuild) == FALSE)
		{
			m_btIsCastleOccupied = TRUE;
			wsprintf(szMsg,lMsg.Get(MSGGET(6, 197)),m_szCastleOwnerGuild);
			bRETVAL = FALSE;
		}
		else
		{
			m_btIsCastleOccupied = TRUE;
			memset(m_szCastleOwnerGuild,0x00,sizeof(m_szCastleOwnerGuild));
			memcpy(m_szCastleOwnerGuild,m_szMiddleWinnerGuild,0x08);
			wsprintf(szMsg,lMsg.Get(MSGGET(6, 198)),m_szCastleOwnerGuild);
			bRETVAL = TRUE;
		}
	}
	else
	{
		if(strcmp(m_szMiddleWinnerGuild,"") == FALSE)
		{
			m_btIsCastleOccupied = FALSE;
			wsprintf(szMsg,lMsg.Get(MSGGET(6, 199)));
			bRETVAL = FALSE;
		}
		else
		{
			m_btIsCastleOccupied = TRUE;
			memset(m_szCastleOwnerGuild,0x00,sizeof(m_szCastleOwnerGuild));
			memcpy(m_szCastleOwnerGuild,m_szMiddleWinnerGuild,0x08);
			wsprintf(szMsg,lMsg.Get(MSGGET(6, 198)),m_szCastleOwnerGuild);
			bRETVAL = TRUE;
		}
	}

	m_btIsSiegeEnded = TRUE;

	LogAddTD("[CastleSiege] Castle Final Winner Guild : %s",(strcmp(m_szCastleOwnerGuild,"") == FALSE) ? "NONE" : m_szCastleOwnerGuild);
	SendMapServerGroupMsg(szMsg);

return bRETVAL;
}

struct CSP_REQ_NPCUPDATEDATA
{
	PWMSG_HEAD h;	// C2:89
	unsigned short wMapSvrNum;	// 4
	int iCount;	// 8
};

struct CSP_NPCUPDATEDATA
{
	int iNpcNumber;	// 0
	int iNpcIndex;	// 4
	int iNpcDfLevel;	// 8
	int iNpcRgLevel;	// C
	int iNpcMaxHp;	// 10
	int iNpcHp;	// 14
	unsigned char btNpcX;	// 18
	unsigned char btNpcY;	// 19
	unsigned char btNpcDIR;	// 1A
};

void CCastleSiege::StoreDbNpc()
{
	LogAddTD("[CastleSiege] CCastleSiege::StoreDbNpc() - << START >>");

	char cBUFFER[0x1090];

	CSP_REQ_NPCUPDATEDATA * lpMsg = (CSP_REQ_NPCUPDATEDATA *)cBUFFER;
	CSP_NPCUPDATEDATA * lpMsgBody = (CSP_NPCUPDATEDATA *)&cBUFFER[sizeof(CSP_REQ_NPCUPDATEDATA)];

	int iCOUNT = 0;

	EnterCriticalSection(&m_critNpcData);
	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();
	int bExist = FALSE;

	while(it != m_vtNpcData.end())
	{
		if(iCOUNT >= 150)
			break;

		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_bNPC_DBSAVE == TRUE 
			&& gObjIsConnected(pNpcData.m_iNPC_OBJINDEX)
			&& gObj[pNpcData.m_iNPC_OBJINDEX].Class == pNpcData.m_iNPC_NUM
			&& gObj[pNpcData.m_iNPC_OBJINDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
			&& gObj[pNpcData.m_iNPC_OBJINDEX].m_iCsNpcExistVal != NULL
			&& gObj[pNpcData.m_iNPC_OBJINDEX].Live == TRUE
			&& gObj[pNpcData.m_iNPC_OBJINDEX].Life > 0)
		{
			lpMsgBody[iCOUNT].iNpcNumber = pNpcData.m_iNPC_NUM;
			lpMsgBody[iCOUNT].iNpcIndex = pNpcData.m_iNPC_INDEX;
			lpMsgBody[iCOUNT].iNpcDfLevel = pNpcData.m_iNPC_DF_LEVEL;
			lpMsgBody[iCOUNT].iNpcRgLevel = pNpcData.m_iNPC_RG_LEVEL;
			lpMsgBody[iCOUNT].iNpcMaxHp = (int)gObj[pNpcData.m_iNPC_OBJINDEX].MaxLife;
			lpMsgBody[iCOUNT].iNpcHp = (int)gObj[pNpcData.m_iNPC_OBJINDEX].Life;
			lpMsgBody[iCOUNT].btNpcX = pNpcData.m_iNPC_SX;
			lpMsgBody[iCOUNT].btNpcY = pNpcData.m_iNPC_SY;
			lpMsgBody[iCOUNT].btNpcDIR = pNpcData.m_iNPC_DIR;


			LogAddTD("[CastleSiege] CCastleSiege::StoreDbNpc() NPC INFO (CLS:%d, IDX:%d(%d), MAXHP:%d, HP:%d, DF:%d, RG:%d)",
				pNpcData.m_iNPC_NUM,pNpcData.m_iNPC_INDEX,pNpcData.m_iNPC_OBJINDEX,lpMsgBody[iCOUNT].iNpcMaxHp,lpMsgBody[iCOUNT].iNpcHp,lpMsgBody[iCOUNT].iNpcDfLevel,lpMsgBody[iCOUNT].iNpcRgLevel);
			iCOUNT++;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);

	if(iCOUNT < 0)
	{
		iCOUNT = 0;
	}

	lpMsg->wMapSvrNum = m_iMapSvrGroup;
	lpMsg->iCount = iCOUNT;
	lpMsg->h.set((PBYTE)lpMsg,0x89,iCOUNT * sizeof(CSP_NPCUPDATEDATA) + sizeof(CSP_REQ_NPCUPDATEDATA));

	cDBSMng.Send((char *)lpMsg,iCOUNT * sizeof(CSP_NPCUPDATEDATA) + sizeof(CSP_REQ_NPCUPDATEDATA));
	LogAddTD("[CastleSiege] CCastleSiege::StoreDbNpc() - << END >>");
}

void CCastleSiege::ClearNonDbNPC()
{
	for(int n = 0; n < OBJ_MAXMONSTER; n++)
	{
		if(gObjIsConnected(n)
			&& gObj[n].MapNumber == MAP_INDEX_CASTLESIEGE
			&& gObj[n].m_btCsNpcType != NULL
			&& gObj[n].m_btCsNpcType != 1)
		{
			if(gObj[n].Class == 278)
			{
				g_CsNPC_LifeStone.DeleteLifeStone(n);
			}

			if(gObj[n].Class == 287 || gObj[n].Class == 286)
			{
				g_CsNPC_Mercenary.DeleteMercenary(n);
			}

			if(gObj[n].Class == 219)
				continue;

			gObjDel(n);
		}
	}
}

void CCastleSiege::CheckReviveNonDbNPC()
{
	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_bNPC_DBSAVE == 0)
		{
			int iNPC_INDEX = gObjAddMonster(MAP_INDEX_CASTLESIEGE);

			if(iNPC_INDEX >= 0)
			{
				gObjSetMonster(iNPC_INDEX,pNpcData.m_iNPC_NUM);

				gObj[iNPC_INDEX].m_PosNum = -1;
				gObj[iNPC_INDEX].X = pNpcData.m_iNPC_SX;
				gObj[iNPC_INDEX].Y = pNpcData.m_iNPC_SY;
				gObj[iNPC_INDEX].MapNumber = MAP_INDEX_CASTLESIEGE;
				gObj[iNPC_INDEX].TX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].TY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].m_OldX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].m_OldY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].Dir = pNpcData.m_iNPC_DIR;
				gObj[iNPC_INDEX].StartX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].StartY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].DieRegen = 0;
				gObj[iNPC_INDEX].MaxRegenTime = 0;
				gObj[iNPC_INDEX].Life = (float)pNpcData.m_iNPC_HP;
				gObj[iNPC_INDEX].MaxLife = (float)pNpcData.m_iNPC_MAXHP;

				switch(pNpcData.m_iNPC_NUM)
				{
				case 215:
					gObj[iNPC_INDEX].m_btCsNpcType = 2;
					break;
				case 221:
					gObj[iNPC_INDEX].m_btCsNpcType = 3;
					break;
				case 222:
					gObj[iNPC_INDEX].m_btCsNpcType = 2;
					break;
				case 288:
					gObj[iNPC_INDEX].m_btCsNpcType = 2;
					break;
				default:
					gObj[iNPC_INDEX].m_btCsNpcType = 2;
					break;
				}

				it->m_bIN_USE = TRUE;
			}
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
}

void CCastleSiege::CheckReviveGuardianStatue()
{
	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{

		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == 283
			&& pNpcData.m_iNPC_LIVE == FALSE
			)
		{
			int iNPC_INDEX = gObjAddMonster(MAP_INDEX_CASTLESIEGE);

			if(iNPC_INDEX >= 0)
			{
				gObjSetMonster(iNPC_INDEX,pNpcData.m_iNPC_NUM);

				gObj[iNPC_INDEX].m_PosNum = -1;
				gObj[iNPC_INDEX].X = pNpcData.m_iNPC_SX;
				gObj[iNPC_INDEX].Y = pNpcData.m_iNPC_SY;
				gObj[iNPC_INDEX].MapNumber = MAP_INDEX_CASTLESIEGE;
				gObj[iNPC_INDEX].TX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].TY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].m_OldX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].m_OldY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].Dir = pNpcData.m_iNPC_DIR;
				gObj[iNPC_INDEX].StartX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].StartY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].DieRegen = 0;
				gObj[iNPC_INDEX].MaxRegenTime = 0;
				gObj[iNPC_INDEX].Life = (float)pNpcData.m_iNPC_HP;
				gObj[iNPC_INDEX].MaxLife = (float)pNpcData.m_iNPC_MAXHP;
				gObj[iNPC_INDEX].m_btCsNpcType = 1;

				int iNPC_DF_LEVEL = pNpcData.m_iNPC_BASE_DF_LEVEL;

				if(iNPC_DF_LEVEL < 0)
				{
					iNPC_DF_LEVEL = 0;
				}

				if(iNPC_DF_LEVEL > CS_MAX_UPGRADE)
				{
					iNPC_DF_LEVEL = CS_MAX_UPGRADE;
				}

				int iNPC_RG_LEVEL = pNpcData.m_iNPC_BASE_RG_LEVEL;

				if(iNPC_RG_LEVEL < 0)
				{
					iNPC_RG_LEVEL = 0;
				}

				if(iNPC_RG_LEVEL > CS_MAX_UPGRADE)
				{
					iNPC_RG_LEVEL = CS_MAX_UPGRADE;
				}

				gObj[iNPC_INDEX].m_Defense = g_iNpcDefense_CSTATUE[iNPC_DF_LEVEL];
				gObj[iNPC_INDEX].m_btCsNpcDfLevel = iNPC_DF_LEVEL;
				gObj[iNPC_INDEX].m_btCsNpcRgLevel = iNPC_RG_LEVEL;

				it->m_btCsNpcExistVal1 = rand()%255+1;
				gObj[iNPC_INDEX].m_btCsNpcExistVal1 = it->m_btCsNpcExistVal1;

				it->m_btCsNpcExistVal2 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal2 = it->m_btCsNpcExistVal2;

				it->m_btCsNpcExistVal3 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal3 = it->m_btCsNpcExistVal3;

				it->m_btCsNpcExistVal4 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal4 = it->m_btCsNpcExistVal4;

				it->m_iNPC_OBJINDEX = iNPC_INDEX;
				it->m_iNPC_LIVE = 2;
				it->m_bIN_USE = TRUE;
			}
		}
		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
}

void CCastleSiege::AdjustDbNpcLevel()
{
	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_bIN_USE == 1
			&& pNpcData.m_iNPC_LIVE > 0)
		{
			int iNPC_INDEX = pNpcData.m_iNPC_OBJINDEX;

			if(gObjIsConnected(iNPC_INDEX)
				&& gObj[iNPC_INDEX].Class == pNpcData.m_iNPC_NUM
				&& gObj[iNPC_INDEX].m_btCsNpcType != FALSE
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal != NULL)
			{
				switch(gObj[iNPC_INDEX].Class)
				{
				case 277:
					{
						int iDF_LEVEL = pNpcData.m_iNPC_DF_LEVEL;

						if(iDF_LEVEL < 0)
						{
							iDF_LEVEL = 0;
						}

						if(iDF_LEVEL > CS_MAX_UPGRADE)
						{
							iDF_LEVEL = CS_MAX_UPGRADE;
						}

						gObj[iNPC_INDEX].m_btCsNpcDfLevel = iDF_LEVEL;
						gObj[iNPC_INDEX].m_Defense = g_iNpcDefense_CGATE[iDF_LEVEL];
					}
					break;
				case 283:
					{
						int iDF_LEVEL = pNpcData.m_iNPC_DF_LEVEL;

						if(iDF_LEVEL < 0)
						{
							iDF_LEVEL = 0;
						}

						if(iDF_LEVEL > CS_MAX_UPGRADE)
						{
							iDF_LEVEL = CS_MAX_UPGRADE;
						}

						gObj[iNPC_INDEX].m_btCsNpcDfLevel = iDF_LEVEL;
						gObj[iNPC_INDEX].m_Defense = g_iNpcDefense_CSTATUE[iDF_LEVEL];
						gObj[iNPC_INDEX].m_btCsNpcRgLevel = pNpcData.m_iNPC_RG_LEVEL;
					}
					break;
				}
			}

			break;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
}

void CCastleSiege::SetAllCastleGateState(int bOpenType)
{
	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();
	int bExist = FALSE;

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == 277
			&& gObjIsConnected(pNpcData.m_iNPC_OBJINDEX)
			&& gObj[pNpcData.m_iNPC_OBJINDEX].Class == 277)
		{
			switch(bOpenType)
			{
			case 0:
				SetGateBlockState(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY,0);
				gObjRemoveBuffEffect(&gObj[pNpcData.m_iNPC_OBJINDEX],BUFF_GATESTATUS);
				gObj[pNpcData.m_iNPC_OBJINDEX].m_btCsGateOpen = 0;
				break;
			case 1:
				SetGateBlockState(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY,1);
				gObjAddBuffEffect(&gObj[pNpcData.m_iNPC_OBJINDEX],BUFF_GATESTATUS,0,0,0,0,-10);
				gObj[pNpcData.m_iNPC_OBJINDEX].m_btCsGateOpen = 1;
				break;
			}
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
}

void CCastleSiege::SetAllUserCsJoinSide()
{
	if(m_bCsTotalGuildInfoLoadOK == FALSE)
		return;

	EnterCriticalSection(&m_critCsTotalGuildInfo);

	for(int iUSER = OBJ_STARTUSERINDEX; iUSER < OBJMAX; iUSER++)
	{
		if(gObjIsConnected(iUSER) == FALSE)
			continue;

		char szGuildName[16] = {0};

		memcpy(szGuildName,gObj[iUSER].GuildName,8);

		map<basic_string<char,char_traits<char> >,_CS_TOTAL_GUILD_DATA,less<basic_string<char,char_traits<char> > > >::iterator it = m_mapCsTotalGuildInfo.find(szGuildName);

		if(it != m_mapCsTotalGuildInfo.end())
		{
			gObj[iUSER].m_btCsJoinSide = it->second.m_iCsGuildID;
			gObj[iUSER].m_bCsGuildInvolved = !(!it->second.m_bGuildInvolved);
		}

		NotifySelfCsJoinSide(iUSER);
		gObj[iUSER].m_iAccumulatedCrownAccessTime = 0;
	}

	LeaveCriticalSection(&m_critCsTotalGuildInfo);
}

void CCastleSiege::SavePcRoomUserList()
{
	// EMPTY FUNCTION
}

void CCastleSiege::ReSpawnEnemyUser(int bRefreshOwnerUser)
{
	int iTX;
	int iTY;
	unsigned char btMapAttr;

	for(int iUSER = OBJ_STARTUSERINDEX; iUSER < OBJMAX; iUSER++)
	{
		if(gObjIsConnected(iUSER) == FALSE)
			continue;

		if(gObj[iUSER].MapNumber != MAP_INDEX_CASTLESIEGE)
			continue;

		unsigned char btCsJoinSide = gObj[iUSER].m_btCsJoinSide;

		if(btCsJoinSide < 0)
		{
			btCsJoinSide = 0;
		}

		if(btCsJoinSide > 2)
		{
			btCsJoinSide = 2;
		}

		int bMoveOK = FALSE;

		switch(btCsJoinSide)
		{
		case 1:
			if(bRefreshOwnerUser != FALSE)
			{
				gObj[iUSER].RegenMapNumber = gObj[iUSER].MapNumber;
				gObj[iUSER].RegenMapX = gObj[iUSER].X;
				gObj[iUSER].RegenMapY = gObj[iUSER].Y;

				gObjClearViewport(&gObj[iUSER]);

				GCTeleportSend(&gObj[iUSER],0xFF,gObj[iUSER].MapNumber,gObj[iUSER].X,gObj[iUSER].Y,gObj[iUSER].Dir);

				if(gObj[iUSER].m_Change >= 0)
				{
					gObjViewportListProtocolCreate(&gObj[iUSER]);
				}

				gObj[iUSER].RegenOk = TRUE;
			}
			bMoveOK = FALSE;
			break;
		case 2:
			{
				for(int iCNT = 0; iCNT < 100; iCNT++)
				{
					iTX = (rand() % (g_iCsUserReSpawnArea[2][2] - g_iCsUserReSpawnArea[2][0])) + g_iCsUserReSpawnArea[2][0];
					iTY = (rand() % (g_iCsUserReSpawnArea[2][3] - g_iCsUserReSpawnArea[2][1])) + g_iCsUserReSpawnArea[2][1];

					btMapAttr = MapC[MAP_INDEX_CASTLESIEGE].m_attrbuf[iTY * 256 + iTX];

					if((btMapAttr & 4) != 4)
					{
						bMoveOK = TRUE;
						break;
					}
				}
			}
			break;
		case 0:
			{
				for(int iCNT = 0; iCNT < 100; iCNT++)
				{
					iTX = (rand()% (g_iCsUserReSpawnArea[0][2] - g_iCsUserReSpawnArea[0][0])) + g_iCsUserReSpawnArea[0][0];
					iTY = (rand()% (g_iCsUserReSpawnArea[0][3] - g_iCsUserReSpawnArea[0][1])) + g_iCsUserReSpawnArea[0][1];

					btMapAttr = MapC[MAP_INDEX_CASTLESIEGE].m_attrbuf[iTY * 256 + iTX];

					if((btMapAttr & 4) != 4)
					{
						bMoveOK = TRUE;
						break;
					}
				}
			}
			break;
		}

		if(bMoveOK != FALSE)
		{
			gObj[iUSER].m_State = 32;
			gObj[iUSER].X = iTX;
			gObj[iUSER].Y = iTY;
			gObj[iUSER].TX = iTX;
			gObj[iUSER].TX = iTY;
			gObj[iUSER].MapNumber = MAP_INDEX_CASTLESIEGE;
			gObj[iUSER].PathCount = 0;
			gObj[iUSER].Teleport = 0;

			gObjClearViewport(&gObj[iUSER]);
			GCTeleportSend(&gObj[iUSER],0xFF,MAP_INDEX_CASTLESIEGE,gObj[iUSER].X,gObj[iUSER].Y,gObj[iUSER].Dir);

			if(gObj[iUSER].m_Change >= 0)
			{
				gObjViewportListProtocolCreate(&gObj[iUSER]);
			}

			gObj[iUSER].RegenMapNumber = MAP_INDEX_CASTLESIEGE;
			gObj[iUSER].RegenMapX = iTX;
			gObj[iUSER].RegenMapY = iTY;
			gObj[iUSER].RegenOk = TRUE;
		}

		//NotifySelfCsJoinSide(iUSER);

		gObj[iUSER].m_iAccumulatedCrownAccessTime = 0;
	}
}

struct PMSG_ANS_NOTIFYCSSTART
{
	PBMSG_HEAD2 h;
	unsigned char btStartState;
};

void CCastleSiege::NotifyAllUserCsStartState(BYTE btStartState)
{
	PMSG_ANS_NOTIFYCSSTART pMsg;

	pMsg.h.set((PBYTE)&pMsg,0xB2,0x17,sizeof(pMsg));
	pMsg.btStartState = btStartState;

	for(int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++)
	{
		if(gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
		{
			DataSend(i,(PBYTE)&pMsg,pMsg.h.size);
		}
	}
}

void CCastleSiege::SendAllUserAnyMsg(char * lpszMsg,int iType)
{
	if(lpszMsg == NULL)
		return;

	PMSG_NOTICE pNotice;

	switch(iType)
	{
	case 1:
		TNotice::MakeNoticeMsg(&pNotice,0,lpszMsg);
		TNotice::SetNoticeProperty(&pNotice,10,_ARGB(255,255,200,80),1,0,20);
		TNotice::SendNoticeToAllUser(&pNotice);
		break;
	case 2:

		TNotice::MakeNoticeMsg(&pNotice,0,lpszMsg);

		for(int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++)
		{
			if(gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
			{
				DataSend(i,(PBYTE)&pNotice,pNotice.h.size);
			}
		}

		break;
	}
}

void CCastleSiege::SendMapServerGroupMsg(char * lpszMsg)
{
	GS_GDReqMapSvrMsgMultiCast(m_iMapSvrGroup,lpszMsg);
}

int CCastleSiege::GetDataLoadState()
{
	return m_iCastleDataLoadState;
}

void CCastleSiege::SetDataLoadState(int iDataLoadState)
{
	m_iCastleDataLoadState = iDataLoadState;
}

void CCastleSiege::SetDbDataLoadOK(int bDbDataLoadOK)
{
	m_bDbDataLoadOK = bDbDataLoadOK;
}

int CCastleSiege::SetCastleInitData(CSP_ANS_CSINITDATA * lpMsg)
{
	if(m_iCastleDataLoadState != 3)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::SetCastleInitData() - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_3");
		return false;
	}

	if(lpMsg == NULL)
		return false;

	memset(&m_tmStartDate,0x00,sizeof(m_tmStartDate));
	memset(&m_tmEndDate,0x00,sizeof(m_tmEndDate));

	m_tmStartDate.wYear = lpMsg->wStartYear;
	m_tmStartDate.wMonth = lpMsg->btStartMonth;
	m_tmStartDate.wDay = lpMsg->btStartDay;

	m_tmEndDate.wYear = lpMsg->wEndYear;
	m_tmEndDate.wMonth = lpMsg->btEndMonth;
	m_tmEndDate.wDay = lpMsg->btEndDay;

	m_btIsSiegeGuildList = lpMsg->btIsSiegeGuildList;
	m_btIsSiegeEnded = lpMsg->btIsSiegeEnded;
	m_btIsCastleOccupied = lpMsg->btIsCastleOccupied;

	memset(m_szCastleOwnerGuild,0x00,sizeof(m_szCastleOwnerGuild));
	memcpy(m_szCastleOwnerGuild,lpMsg->szCastleOwnGuild,sizeof(lpMsg->szCastleOwnGuild));

	m_i64CastleMoney = lpMsg->i64CastleMoney;

	m_iTaxRateChaos = lpMsg->iTaxRateChaos;
	m_iTaxRateStore = lpMsg->iTaxRateStore;
	m_iTaxHuntZone = lpMsg->iTaxHuntZone;

	int iSTART_DATE_NUM = MACRO2(( MACRO1(m_tmStartDate.wDay) | MACRO1(m_tmStartDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(m_tmStartDate.wYear) << 16;
	int iEND_DATE_NUM = MACRO2(( MACRO1(m_tmEndDate.wDay) | MACRO1(m_tmEndDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(m_tmEndDate.wYear) << 16;

	m_tmNowDate = m_tmStartDate;

	_SYSTEMTIME stTime;

	GetStateDateTime(CASTLESIEGE_STATE_ENDCYCLE,&stTime);

	LogAddTD("[CastleSiege] CCastleSiege::LoadData() - Siege Schedule Start Date (%d-%d-%d)",
		m_tmStartDate.wYear,m_tmStartDate.wMonth,m_tmStartDate.wDay);

	LogAddTD("[CastleSiege] CCastleSiege::LoadData() - Siege Schedule Date (%d-%d-%d)",
		stTime.wDay,stTime.wHour,stTime.wMinute);

	GetNextDay(&m_tmNowDate,stTime.wDay,stTime.wHour,stTime.wMinute,0);

	LogAddTD("[CastleSiege] CCastleSiege::LoadData() - Siege Schedule End Date (%d-%d-%d(%d:%d:%d)",
		m_tmNowDate.wYear,m_tmNowDate.wMonth,m_tmNowDate.wDay,m_tmNowDate.wHour,m_tmNowDate.wMinute,m_tmNowDate.wSecond);

	if(iSTART_DATE_NUM > iEND_DATE_NUM)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::SetCastleInitData() - iSTART_DATE_NUM > iEND_DATE_NUM");
		return false;
	}

	return true;
}

int CCastleSiege::SetCastleNpcData(CSP_CSINITDATA * lpMsg,int iCOUNT)
{
	if(m_iCastleDataLoadState != 3)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::SetCastleInitData() - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_3");
		return false;
	}

	if(lpMsg == NULL)
		return false;

	if(iCOUNT < 0)
		return false;

	vector<_CS_NPC_DATA>::iterator it;

	for(int iNPC_COUNT = 0; iNPC_COUNT < iCOUNT; iNPC_COUNT++)
	{
		EnterCriticalSection(&m_critNpcData);

		it = m_vtNpcData.begin();

		int bExist = FALSE;

		while(it != m_vtNpcData.end())
		{
			_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

			if(pNpcData.m_iNPC_NUM == lpMsg[iNPC_COUNT].iNpcNumber && pNpcData.m_iNPC_INDEX == lpMsg[iNPC_COUNT].iNpcIndex)
			{
				it->m_iNPC_LIVE = TRUE;
				it->m_iNPC_DF_LEVEL = lpMsg[iNPC_COUNT].iNpcDfLevel;
				it->m_iNPC_RG_LEVEL = lpMsg[iNPC_COUNT].iNpcRgLevel;
				it->m_iNPC_MAXHP = lpMsg[iNPC_COUNT].iNpcMaxHp;
				it->m_iNPC_HP = lpMsg[iNPC_COUNT].iNpcHp;
				it->m_iNPC_SX = lpMsg[iNPC_COUNT].btNpcX;
				it->m_iNPC_SY = lpMsg[iNPC_COUNT].btNpcY;
				it->m_iNPC_DX = -1;
				it->m_iNPC_DY = -1;
				it->m_iNPC_DIR = lpMsg[iNPC_COUNT].btNpcDIR;
				it->m_iNPC_SIDE = 1;
				bExist = TRUE;
				break;
			}

			it++;
		}

		LeaveCriticalSection(&m_critNpcData);

		if(bExist == FALSE)
		{
			_CS_NPC_DATA pNpcData2;

			pNpcData2.m_bIN_USE = TRUE;

			pNpcData2.m_iNPC_NUM = lpMsg[iNPC_COUNT].iNpcNumber;
			pNpcData2.m_iNPC_INDEX = lpMsg[iNPC_COUNT].iNpcIndex;

			pNpcData2.m_iNPC_LIVE = TRUE;
			pNpcData2.m_iNPC_SIDE = 1;

			pNpcData2.m_iNPC_BASE_DF_LEVEL = lpMsg[iNPC_COUNT].iNpcDfLevel;
			pNpcData2.m_iNPC_DF_LEVEL = pNpcData2.m_iNPC_BASE_DF_LEVEL;

			pNpcData2.m_iNPC_BASE_RG_LEVEL = lpMsg[iNPC_COUNT].iNpcRgLevel;
			pNpcData2.m_iNPC_RG_LEVEL = pNpcData2.m_iNPC_BASE_RG_LEVEL;


			pNpcData2.m_iNPC_BASE_MAXHP = lpMsg[iNPC_COUNT].iNpcMaxHp;
			pNpcData2.m_iNPC_MAXHP = pNpcData2.m_iNPC_BASE_MAXHP;

			pNpcData2.m_iNPC_BASE_HP = lpMsg[iNPC_COUNT].iNpcHp;
			pNpcData2.m_iNPC_HP = pNpcData2.m_iNPC_BASE_HP;

			pNpcData2.m_iNPC_BASE_SX = lpMsg[iNPC_COUNT].btNpcX;
			pNpcData2.m_iNPC_SX = pNpcData2.m_iNPC_BASE_SX;

			pNpcData2.m_iNPC_BASE_SY = lpMsg[iNPC_COUNT].btNpcY;
			pNpcData2.m_iNPC_SY = pNpcData2.m_iNPC_BASE_SY;

			pNpcData2.m_iNPC_BASE_DX = -1;
			pNpcData2.m_iNPC_DX = pNpcData2.m_iNPC_BASE_DX;

			pNpcData2.m_iNPC_BASE_DY = -1;
			pNpcData2.m_iNPC_DY = pNpcData2.m_iNPC_BASE_DY;

			pNpcData2.m_iNPC_BASE_DIR = lpMsg[iNPC_COUNT].btNpcDIR;
			pNpcData2.m_iNPC_DIR = pNpcData2.m_iNPC_BASE_DIR;

			EnterCriticalSection(&m_critNpcData);
			m_vtNpcData.push_back(pNpcData2);
			LeaveCriticalSection(&m_critNpcData);
		}
	}

	return true;
}

void CCastleSiege::ProcState_NONE()
{
	// EMPTY FUNCTION
}

void CCastleSiege::ProcState_IDLE_1()
{
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_IDLE_1()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		SetState(CASTLESIEGE_STATE_REGSIEGE,TRUE);
	}
}

void CCastleSiege::ProcState_REGSIEGE()
{
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

		if(GetTickCount() - m_dwEVENT_MSG_TICK_COUNT > 1800000)
		{
			SendAllUserAnyMsg(lMsg.Get(MSGGET(6, 78)),1);
			m_dwEVENT_MSG_TICK_COUNT = GetTickCount();
		}

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_REGSIEGE()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		SetState(CASTLESIEGE_STATE_IDLE_2,TRUE);
	}
}

void CCastleSiege::ProcState_IDLE_2()
{
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_IDLE_2()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		SetState(CASTLESIEGE_STATE_REGMARK,TRUE);
	}
}

void CCastleSiege::ProcState_REGMARK()
{
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

		if(GetTickCount() - m_dwEVENT_MSG_TICK_COUNT > 1800000)
		{
			SendAllUserAnyMsg(lMsg.Get(MSGGET(6, 79)),1);
			m_dwEVENT_MSG_TICK_COUNT = GetTickCount();
		}

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_REGMARK()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		SetState(CASTLESIEGE_STATE_IDLE_3,TRUE);
	}
}

void CCastleSiege::ProcState_IDLE_3()
{
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

		if(GetTickCount() - m_dwEVENT_MSG_TICK_COUNT > 7200000)
		{
			SendAllUserAnyMsg(lMsg.Get(MSGGET(6, 80)),1);
			m_dwEVENT_MSG_TICK_COUNT = GetTickCount();
		}

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_IDLE_3()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		SetState(CASTLESIEGE_STATE_NOTIFY,TRUE);
	}
}

void CCastleSiege::ProcState_NOTIFY()
{
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

		if(GetTickCount() - m_dwEVENT_MSG_TICK_COUNT > 1800000)
		{
			SendAllUserAnyMsg(lMsg.Get(MSGGET(6, 81)),1);
			m_dwEVENT_MSG_TICK_COUNT = GetTickCount();
		}

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_NOTIFY()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		SetState(CASTLESIEGE_STATE_READYSIEGE,TRUE);
	}
}

void CCastleSiege::ProcState_READYSIEGE()
{
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

		if(GetTickCount() - m_dwEVENT_MSG_TICK_COUNT > 1800000)
		{
			SendAllUserAnyMsg(lMsg.Get(MSGGET(6, 82)),1);
			m_dwEVENT_MSG_TICK_COUNT = GetTickCount();
		}

		if(m_iCS_REMAIN_MSEC <= 300000)
		{
			if(GetTickCount() - m_dwCS_STARTTIME_TICK_COUNT > 60000)
			{
				char szText[0x100] = {0};

				wsprintf(szText,lMsg.Get(MSGGET(6, 188)),m_iCS_REMAIN_MSEC / 60000 + 1);
				SendAllUserAnyMsg(szText,2);

				m_dwCS_STARTTIME_TICK_COUNT = GetTickCount();
			}
		}

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_READYSIEGE()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		SetState(CASTLESIEGE_STATE_STARTSIEGE,TRUE);
	}
}

void CCastleSiege::ProcState_STARTSIEGE()
{
	CheckMiddleWinnerGuild();
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

		if(GetTickCount() - m_dwCHECK_GATE_ALIVE_COUNT > 5000)
		{
			CheckCsDbNpcAlive();
			m_dwCHECK_GATE_ALIVE_COUNT = GetTickCount();
		}

		if(GetTickCount() - m_dwCS_LEFTTIME_TICK_COUNT > 180000 && m_iCS_REMAIN_MSEC > 180000)
		{
			GCAnsCsLeftTimeAlarm((m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60);
			m_dwCS_LEFTTIME_TICK_COUNT = GetTickCount();
		}

		if(m_iCS_REMAIN_MSEC <= 180000 && GetTickCount() - m_dwCS_LEFTTIME_TICK_COUNT > 60000)
		{
			GCAnsCsLeftTimeAlarm((m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60);
			m_dwCS_LEFTTIME_TICK_COUNT = GetTickCount();
		}

		/*if(m_dwCS_JOINSIDE_REFRESH_TOTAL_COUNT < 1 )//&& GetTickCount() - m_dwCS_JOINSIDE_REFRESH_TICK_COUNT > 10000)
		{
			SetAllUserCsJoinSide();
			m_dwCS_JOINSIDE_REFRESH_TICK_COUNT = GetTickCount();
			m_dwCS_JOINSIDE_REFRESH_TOTAL_COUNT++;
		}*/

		if(GetTickCount() - m_dwCS_MINIMAP_SEND_TICK_COUNT > 3000)
		{
			OperateMiniMapWork();
			m_dwCS_MINIMAP_SEND_TICK_COUNT = GetTickCount();
		}

		if(GetTickCount() - m_dwEVENT_MSG_TICK_COUNT > 1800000)
		{
			SendAllUserAnyMsg(lMsg.Get(MSGGET(6, 83)),1);
			m_dwEVENT_MSG_TICK_COUNT = GetTickCount();
		}

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_STARTSIEGE()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		if(CheckCastleSiegeResult() == 1)
		{
			GS_GDReqResetCastleTaxInfo(m_iMapSvrGroup);
		}

		ReSpawnEnemyUser(0);

		if(m_btIsCastleOccupied == 1)
		{
			GS_GDReqCastleOwnerChange(m_iMapSvrGroup,m_btIsCastleOccupied,m_szCastleOwnerGuild);
		}

		GS_GDReqSiegeEndedChange(m_iMapSvrGroup,TRUE);
		StoreDbNpc();
		NotifyAllUserCsStartState(FALSE);
		GS_GDReqResetSiegeGuildInfo(m_iMapSvrGroup);
		SetState(CASTLESIEGE_STATE_ENDSIEGE,TRUE);
	}
}

void CCastleSiege::ProcState_ENDSIEGE()
{
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_ENDSIEGE()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		SetState(CASTLESIEGE_STATE_ENDCYCLE,TRUE);
	}
}
	
void CCastleSiege::ProcState_ENDCYCLE()
{
	int iTICK_MSEC = GetTickCount() - m_iCS_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iCS_REMAIN_MSEC -= iTICK_MSEC;
		m_iCS_TICK_COUNT = GetTickCount();

#if (__CUSTOM__ != 1)
		LogAddC(3,"RUN ProcState_ENDCYCLE()	LEFT-MSEC:%d",m_iCS_REMAIN_MSEC);
#endif
	}

	if(m_iCS_REMAIN_MSEC <= 0)
	{
		SetState(CASTLESIEGE_STATE_IDLE_1,TRUE);
	}
}

int CCastleSiege::CheckMiddleWinnerGuild()
{
	if(m_dwCrownAccessTime == FALSE)
		return false;

	if(gObjIsConnected(m_iCastleCrownAccessUser) == FALSE 
		|| gObjIsConnected(m_iCastleSwitchAccessUser1) == FALSE
		|| gObjIsConnected(m_iCastleSwitchAccessUser2) == FALSE)
		return false;

	if(strcmp(gObj[m_iCastleCrownAccessUser].GuildName,"") == 0
		|| strcmp(gObj[m_iCastleSwitchAccessUser1].GuildName,"") == 0
		|| strcmp(gObj[m_iCastleSwitchAccessUser2].GuildName,"") == 0)
		return false;

	if(gObj[m_iCastleCrownAccessUser].m_btCsJoinSide >= 2
		&& gObj[m_iCastleSwitchAccessUser1].m_btCsJoinSide >= 2
		&& gObj[m_iCastleSwitchAccessUser2].m_btCsJoinSide >= 2
		&& gObj[m_iCastleCrownAccessUser].m_bCsGuildInvolved == TRUE
		&& gObj[m_iCastleCrownAccessUser].m_btCsJoinSide == gObj[m_iCastleSwitchAccessUser1].m_btCsJoinSide
		&& gObj[m_iCastleCrownAccessUser].m_btCsJoinSide == gObj[m_iCastleSwitchAccessUser2].m_btCsJoinSide
		&& (GetTickCount() - m_dwCrownAccessTime) + gObj[m_iCastleCrownAccessUser].m_iAccumulatedCrownAccessTime >= 30000)
	{
		LogAddTD("[CastleSiege] Castle Winner Has been Changed (GUILD:%s)",
			gObj[m_iCastleCrownAccessUser].GuildName);

		GCAnsCsAccessCrownState(m_iCastleCrownAccessUser,1);
		NotifyCrownState(2);
		memset(m_szMiddleWinnerGuild,0x00,sizeof(m_szMiddleWinnerGuild));
		memcpy(m_szMiddleWinnerGuild,gObj[m_iCastleCrownAccessUser].GuildName,8);
		ClearNonDbNPC_MidWin();
		ChangeWinnerGuild(gObj[m_iCastleCrownAccessUser].m_btCsJoinSide);
		NotifyAllUserCsProgState(1,gObj[m_iCastleCrownAccessUser].GuildName);
		ResetCrownAccessTickCount();
		ResetCrownUserIndex();
		SetCrownAccessUserX(NULL);
		SetCrownAccessUserY(NULL);
		ResetCrownSwitchUserIndex(217);
		ResetCrownSwitchUserIndex(218);
		m_bRegCrownAvailable = FALSE;
		ReSpawnEnemyUser(0);
		GCAnsCsLeftTimeAlarm((m_iCS_REMAIN_MSEC / 1000) / 3600,(((m_iCS_REMAIN_MSEC) / 1000) %3600) /60);
		m_dwCS_LEFTTIME_TICK_COUNT = GetTickCount();
		return true;
	}
return false;
}

void CCastleSiege::CheckCsDbNpcAlive()
{
	if(m_iCastleSiegeState != CASTLESIEGE_STATE_STARTSIEGE)
		return;
	EnterCriticalSection(&m_critNpcData);
	
	if(!m_vtNpcData.empty())
	{
		vector<_CS_NPC_DATA>::iterator it;
		for(it = m_vtNpcData.begin(); it != m_vtNpcData.end();it++)
		{
			_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

			if((pNpcData.m_iNPC_NUM == 277 || pNpcData.m_iNPC_NUM == 283)
				&& pNpcData.m_iNPC_OBJINDEX != -1)
			{
				if(gObjIsConnected(pNpcData.m_iNPC_OBJINDEX) == FALSE
					|| gObj[pNpcData.m_iNPC_OBJINDEX].m_iCsNpcExistVal != pNpcData.m_iCsNpcExistVal
					|| gObj[pNpcData.m_iNPC_OBJINDEX].m_iCsNpcExistVal == 0
					|| (gObj[pNpcData.m_iNPC_OBJINDEX].Class != 277 && gObj[pNpcData.m_iNPC_OBJINDEX].Class != 283)
					|| gObj[pNpcData.m_iNPC_OBJINDEX].Live == FALSE)
				{
					pNpcData.m_iNPC_HP = pNpcData.m_iNPC_BASE_HP;
					pNpcData.m_iNPC_MAXHP = pNpcData.m_iNPC_BASE_MAXHP;
					pNpcData.m_iNPC_RG_LEVEL = pNpcData.m_iNPC_BASE_RG_LEVEL;
					pNpcData.m_iNPC_DF_LEVEL = pNpcData.m_iNPC_BASE_DF_LEVEL;

					switch(pNpcData.m_iNPC_NUM)
					{
					case 277:
						SetGateBlockState(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY,1);
						gObjAddBuffEffect(&gObj[pNpcData.m_iNPC_OBJINDEX],BUFF_GATESTATUS,0,0,0,0,-10);
						LogAddTD("[CastleSiege] Castle Gate is Broken (CLS:%d, IDX:%d, X:%d, Y:%d)",
							pNpcData.m_iNPC_NUM,pNpcData.m_iNPC_INDEX,pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY);
						break;
					case 283:
						if(CheckGuardianStatueExist() == FALSE)
						{
							ClearCastleTowerBarrier();
							SetCastleTowerAccessable(TRUE);
						}

						LogAddTD("[CastleSiege] Defend Statue is Broken (CLS:%d, IDX:%d, X:%d, Y:%d)",
							pNpcData.m_iNPC_NUM,pNpcData.m_iNPC_INDEX,pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY);
						break;
					}

					it->m_iNPC_OBJINDEX = -1;
				}
			}
		}
	}

	LeaveCriticalSection(&m_critNpcData);
}

struct PMSG_ANS_SENDMINIMAPDATA
{
	PWMSG_HEAD h;
	int iCount;
};

struct PMSG_ANS_SENDNPCMINIMAPDATA
{
	PWMSG_HEAD h;
	int iCount;
};

struct PMSG_SENDNPCMINIMAPDATA
{
	unsigned char btNpcType;
	unsigned char btX;
	unsigned char btY;
};

typedef map<basic_string<char,char_traits<char> >,_CS_TOTAL_GUILD_DATA,less<basic_string<char,char_traits<char> > > > _CS_TOTAL_GUILD_DATA_MAP;

void CCastleSiege::OperateMiniMapWork()
{
using namespace std;
	if(m_bCsBasicGuildInfoLoadOK == FALSE)
		return;

	map<int,_CS_MINIMAP_DATA > mapMiniMapData;

	BYTE cBUFFER1[0x8D8] = {0};
	PMSG_ANS_SENDMINIMAPDATA * lpMsgSend1 = (PMSG_ANS_SENDMINIMAPDATA *)cBUFFER1;
	PMSG_SENDMINIMAPDATA * lpMsgSendBody1 = (PMSG_SENDMINIMAPDATA *)&cBUFFER1[sizeof(PMSG_ANS_SENDMINIMAPDATA)];

	BYTE cBUFFER2[0x2CA] = {0};

	PMSG_ANS_SENDNPCMINIMAPDATA * lpMsgSend2 = (PMSG_ANS_SENDNPCMINIMAPDATA *)cBUFFER2;
	PMSG_SENDNPCMINIMAPDATA * lpMsgSendBody2 = (PMSG_SENDNPCMINIMAPDATA *)&cBUFFER2[sizeof(PMSG_ANS_SENDNPCMINIMAPDATA)];

	EnterCriticalSection(&m_critCsBasicGuildInfo);

	for(_CS_TOTAL_GUILD_DATA_MAP::iterator it = m_mapCsBasicGuildInfo.begin();it != m_mapCsBasicGuildInfo.end(); it++)
	{
		_CS_TOTAL_GUILD_DATA & stTotalGuildData = it->second;
		_CS_MINIMAP_DATA stMiniMapData;

		memcpy(stMiniMapData.m_szGuildName,&it->first[0],8);

		mapMiniMapData.insert(pair<int,_CS_MINIMAP_DATA>(stTotalGuildData.m_iCsGuildID,stMiniMapData));
	}

	LeaveCriticalSection(&m_critCsBasicGuildInfo);

	if(mapMiniMapData.empty())
		return;

	EnterCriticalSection(&m_critNpcData);


	lpMsgSend2->iCount = 0;

	if(!m_vtNpcData.empty())
	{
		vector<_CS_NPC_DATA>::iterator it;
		for(it = m_vtNpcData.begin();it != m_vtNpcData.end();it++)
		{
			if(lpMsgSend2->iCount >= 150)
				break;

			_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

			if((pNpcData.m_iNPC_NUM == 277 || pNpcData.m_iNPC_NUM == 283)
				&& pNpcData.m_iNPC_OBJINDEX != -1)
			{
				if(gObjIsConnected(pNpcData.m_iNPC_OBJINDEX) == FALSE
					|| gObj[pNpcData.m_iNPC_OBJINDEX].m_iCsNpcExistVal != pNpcData.m_iCsNpcExistVal
					|| (gObj[pNpcData.m_iNPC_OBJINDEX].Class != 277 && gObj[pNpcData.m_iNPC_OBJINDEX].Class != 283))
				{
					BYTE btNpcType = 0;

					switch(pNpcData.m_iNPC_NUM)
					{
					case 277:
						btNpcType = 0;
						break;
					case 283:
						btNpcType = 1;
						break;
					default: continue;
					}

					lpMsgSendBody2[lpMsgSend2->iCount].btNpcType = btNpcType;
					lpMsgSendBody2[lpMsgSend2->iCount].btX = gObj[pNpcData.m_iNPC_OBJINDEX].X;
					lpMsgSendBody2[lpMsgSend2->iCount].btY = gObj[pNpcData.m_iNPC_OBJINDEX].Y;
					lpMsgSend2->iCount++;
				}
			}
		}
	}

	lpMsgSend2->h.set(cBUFFER2,0xBB,sizeof(PMSG_ANS_SENDNPCMINIMAPDATA) + sizeof(PMSG_SENDNPCMINIMAPDATA) * lpMsgSend2->iCount);

	LeaveCriticalSection(&m_critNpcData);

	EnterCriticalSection(&m_critCsMiniMap);
	m_mapMiniMapData.clear();

	for(map<int,_CS_MINIMAP_DATA >::iterator it2 = mapMiniMapData.begin();it2 != mapMiniMapData.end();it2++)
	{
		m_mapMiniMapData.insert(pair<int,_CS_MINIMAP_DATA>(it2->first,it2->second));
	}

	for(int iIDX = OBJ_STARTUSERINDEX; iIDX < OBJMAX;iIDX++)
	{
		if(gObjIsConnected(iIDX) == FALSE)
			continue;

		if(gObj[iIDX].MapNumber != MAP_INDEX_CASTLESIEGE)
			continue;

		if(gObj[iIDX].m_btCsJoinSide == 0)
			continue;

		map<int,_CS_MINIMAP_DATA >::iterator it3 = m_mapMiniMapData.find(gObj[iIDX].m_btCsJoinSide);

		if(it3 != m_mapMiniMapData.end())
		{
			if(it3->second.m_iMiniMapPointCount >= 1000)
				continue;

			if(gObj[iIDX].GuildStatus == 0x80
				&& strcmp(gObj[iIDX].GuildName,"") != 0
				&& strcmp(gObj[iIDX].GuildName,it3->second.m_szGuildName) == 0)
			{
				it3->second.m_iGuildMasterIndex = iIDX;
				continue;
			}

			it3->second.m_stMiniMapPoint[it3->second.m_iMiniMapPointCount].btX = gObj[iIDX].X;
			it3->second.m_stMiniMapPoint[it3->second.m_iMiniMapPointCount].btY = gObj[iIDX].Y;
			it3->second.m_iMiniMapPointCount++;
		}
	}

	for(map<int,_CS_MINIMAP_DATA >::iterator it4 = m_mapMiniMapData.begin();it4 != m_mapMiniMapData.end();it4++)
	{
		lpMsgSend1->iCount = 0;

		if(gObjIsConnected(it4->second.m_iGuildMasterIndex) == FALSE)
			continue;

		lpMsgSend1->iCount = it4->second.m_iMiniMapPointCount;

		if(lpMsgSend1->iCount > 1000)
			lpMsgSend1->iCount = 1000;

		memcpy(lpMsgSendBody1,it4->second.m_stMiniMapPoint,sizeof(PMSG_SENDMINIMAPDATA) * lpMsgSend1->iCount);

		lpMsgSend1->h.set(cBUFFER1,0xB6,sizeof(PMSG_ANS_SENDMINIMAPDATA) + (sizeof(PMSG_SENDMINIMAPDATA) * lpMsgSend1->iCount));

		DataSend(it4->second.m_iGuildMasterIndex,cBUFFER1,sizeof(PMSG_ANS_SENDMINIMAPDATA) + (sizeof(PMSG_SENDMINIMAPDATA) * lpMsgSend1->iCount));
		DataSend(it4->second.m_iGuildMasterIndex,cBUFFER2,sizeof(PMSG_ANS_SENDNPCMINIMAPDATA) + sizeof(PMSG_SENDNPCMINIMAPDATA) * lpMsgSend2->iCount);
	}

	LeaveCriticalSection(&m_critCsMiniMap);
}

void CCastleSiege::ClearNonDbNPC_MidWin()
{
	for(int n = 0; n < OBJ_MAXMONSTER; n++)
	{
		if(gObjIsConnected(n)
			&& gObj[n].MapNumber == MAP_INDEX_CASTLESIEGE
			&& gObj[n].m_btCsNpcType != 0
			&& gObj[n].m_btCsNpcType != 1)
		{
			if(gObj[n].Class == 278)
			{
				g_CsNPC_LifeStone.DeleteLifeStone(n);
			}

			if(gObj[n].Class == 278)
			{
				gObjDel(n);
			}
		}
	}
}

struct PMSG_ANS_NOTIFYCROWNSTATE
{
	PBMSG_HEAD2 h;
	unsigned char btCrownState;
};

void CCastleSiege::NotifyCrownState(BYTE btState)
{
	if(gObjIsConnected(m_iCrownIndex) == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::NotifyCrownState() - m_iCrownIndex isn't Valid");
		return;
	}

	if(gObj[m_iCrownIndex].Class != 216)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::NotifyCrownState() - m_iCrownIndex isn't Valid");
		return;
	}

	LPOBJ lpObj = &gObj[m_iCrownIndex];

	if( btState == FALSE )
	{
		gObjAddBuffEffect(lpObj,BUFF_CROWNREGISTER,0,0,0,0,-10);
	}
	else
	{
		gObjRemoveBuffEffect(lpObj,BUFF_CROWNREGISTER);
	}

	if(lpObj->VPCount2 < 1)
		return;

	PMSG_ANS_NOTIFYCROWNSTATE pMsg;

	pMsg.h.set((PBYTE)&pMsg,0xB2,0x16,sizeof(pMsg));
	pMsg.btCrownState = btState;

	int tObjNum = -1;

	for(int i = 0; i < MAX_VIEWPORT; i++)
	{
		tObjNum = lpObj->VpPlayer2[i].number;

		if(tObjNum >= 0 && gObj[tObjNum].Type == OBJ_USER && gObj[tObjNum].Live != FALSE)
		{
			DataSend(tObjNum,(PBYTE)&pMsg,pMsg.h.size);
		}
	}

	LogAddTD("[CastleSiege] Crown State Changed (%d)",btState);
}

void CCastleSiege::ResetCrownSwitchUserIndex(int iMonsterClass)
{
	switch(iMonsterClass)
	{
	case 217:
		m_iCastleSwitchAccessUser1 = -1;
		break;
	case 218:
		m_iCastleSwitchAccessUser2 = -1;
		break;
	}
}

void CCastleSiege::SetCrownAccessUserY(BYTE iY)
{
	m_btCastleCrownAccessUserY = iY;
}

void CCastleSiege::SetCrownAccessUserX(BYTE iX)
{
	m_btCastleCrownAccessUserX = iX;
}

void CCastleSiege::ResetCrownUserIndex()
{
	m_iCastleCrownAccessUser = -1;
}

void CCastleSiege::ResetCrownAccessTickCount()
{
	m_dwCrownAccessTime = 0;
}

void CCastleSiege::SetCrownAccessTickCount()
{
	m_dwCrownAccessTime = GetTickCount();
}

struct PMSG_ANS_NOTIFYCSPROGRESS
{
	PBMSG_HEAD2 h;
	unsigned char btCastleSiegeState;
	unsigned char szGuildName[0x8];
};

void CCastleSiege::NotifyAllUserCsProgState(BYTE btProgState,char * lpszGuildName)
{
	PMSG_ANS_NOTIFYCSPROGRESS pMsg;
	pMsg.h.set((LPBYTE)&pMsg,0xB2,0x18,sizeof(pMsg));
	pMsg.btCastleSiegeState = btProgState;
	memset(pMsg.szGuildName,0x00,sizeof(pMsg.szGuildName));
	memcpy(pMsg.szGuildName,lpszGuildName,sizeof(pMsg.szGuildName));

	for(int i = OBJ_STARTUSERINDEX; i < OBJMAX;i++)
	{
		if(gObj[i].Connected == PLAYER_PLAYING
			&& gObj[i].Type == OBJ_USER
			&& gObj[i].MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			DataSend(i,(LPBYTE)&pMsg,pMsg.h.size);
		}
	}
}

void CCastleSiege::ChangeWinnerGuild(int iCsJoinSide)
{
	char szGuildTo[16] = {0};
	char szGuildFrom[16] = {0};

	EnterCriticalSection(&m_critCsTotalGuildInfo);

	for(_CS_TOTAL_GUILD_DATA_MAP::iterator it = m_mapCsTotalGuildInfo.begin();it != m_mapCsTotalGuildInfo.end(); it++)
	{
		if(it->second.m_iCsGuildID == iCsJoinSide)
		{
			it->second.m_iCsGuildID = 1;

			if(it->second.m_bGuildInvolved == TRUE)
			{
				memcpy(szGuildTo,&it->first[0],8);
			}
		}
		else if(it->second.m_iCsGuildID == 1)
		{
			it->second.m_iCsGuildID = iCsJoinSide;

			if(it->second.m_bGuildInvolved == TRUE)
			{
				memcpy(szGuildFrom,&it->first[0],8);
			}
		}
	}

	for(int iUSER = OBJ_STARTUSERINDEX; iUSER < OBJMAX;iUSER++)
	{
		if(gObjIsConnected(iUSER) == FALSE)
			continue;

		char szGuildName[16] = {0};

		memcpy(szGuildName,gObj[iUSER].GuildName,8);

		_CS_TOTAL_GUILD_DATA_MAP::iterator it = m_mapCsTotalGuildInfo.find(szGuildName);

		if(it != m_mapCsTotalGuildInfo.end())
		{
			gObj[iUSER].m_btCsJoinSide = it->second.m_iCsGuildID;
			gObj[iUSER].m_bCsGuildInvolved = !(!it->second.m_bGuildInvolved);
			gObj[iUSER].m_iAccumulatedCrownAccessTime = 0;
		}
	}
	LeaveCriticalSection(&m_critCsTotalGuildInfo);
	LogAddTD("[CastleSiege] Castle Winner Guild Change From '%s' To '%s'",szGuildFrom,szGuildTo);
}

void CCastleSiege::SetCastleTowerAccessable(int bTowerAccessable)
{
	m_bCastleTowerAccessable = bTowerAccessable;
}

void CCastleSiege::ClearCastleTowerBarrier()
{
	for(int iIDX = 0; iIDX < OBJ_MAXMONSTER; iIDX++)
	{
		if(gObjIsConnected(iIDX)== FALSE)
			continue;

		if(gObj[iIDX].Class == 215)
		{
			gObjDel(iIDX);
		}
	}
}

int CCastleSiege::CheckGuardianStatueExist()
{
	for(int iIDX = 0; iIDX < OBJ_MAXMONSTER; iIDX++)
	{
		if(gObj[iIDX].Class == 283
			&& gObj[iIDX].Connected > PLAYER_LOGGED
			&& gObj[iIDX].Live == TRUE
			&& gObj[iIDX].MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			return true;
		}
	}
	return false;
}

int CCastleSiege::GetCastleState()
{
	return m_iCastleSiegeState;
}


_SYSTEMTIME CCastleSiege::GetCastleLeftSiegeDate()
{
	return m_tmLeftCastleSiegeDate;
}

int CCastleSiege::CalcCastleLeftSiegeDate()
{
	memset(&m_tmLeftCastleSiegeDate,0x00,sizeof(m_tmLeftCastleSiegeDate));

	if(m_iCastleSiegeState < CASTLESIEGE_STATE_IDLE_1 || m_iCastleSiegeState > CASTLESIEGE_STATE_ENDCYCLE)
	{
		return -3;
	}

	if(m_bDoRun == FALSE)
	{
		return -3;
	}

	if(m_iCastleSiegeState == CASTLESIEGE_STATE_STARTSIEGE)
	{
		return -1;
	}

	if(m_btIsSiegeEnded == TRUE || m_iCastleSiegeState >= CASTLESIEGE_STATE_ENDSIEGE)
	{
		return -2;
	}

	int iGAP_SEC = -1;
	int bAddStart = FALSE;

	EnterCriticalSection(&m_critScheduleData);

	vector<_CS_SCHEDULE_DATA>::iterator it;
	for(it = m_vtScheduleData.begin();it != m_vtScheduleData.end(); it++)
	{
		_CS_SCHEDULE_DATA & pScheduleData = *it;

		if(pScheduleData.m_iSTATE == 7)
		{
			break;
		}
		else
		{
			if(bAddStart != FALSE)
			{
				iGAP_SEC += pScheduleData.m_iGAP_SEC;
			}

			if(pScheduleData.m_iSTATE == m_iCastleSiegeState)
			{
				iGAP_SEC = m_iCS_REMAIN_MSEC / 1000;
				bAddStart = TRUE;
			}
		}
	}
	LeaveCriticalSection(&m_critScheduleData);

	if(iGAP_SEC >= 0)
	{
		_SYSTEMTIME tmNowDate;
		GetLocalTime(&tmNowDate);

		GetNextDay(&tmNowDate,0,0,0,iGAP_SEC);

		m_tmLeftCastleSiegeDate = tmNowDate;

		if(iGAP_SEC == 0)
		{
			LogAddC(2,"[CastleSiege] CCastleSiege::CalcCastleLeftSiegeDate() - iGAP_SEC == 0");
		}

		return iGAP_SEC;
	}
return -3;
}

int CCastleSiege::GetCastleStateTerm(_SYSTEMTIME * tmStateStartDate,_SYSTEMTIME * tmStateEndDate)
{
	if(m_iCastleSiegeState < CASTLESIEGE_STATE_IDLE_1 || m_iCastleSiegeState > CASTLESIEGE_STATE_ENDCYCLE)
	{
		return false;
	}

	if(m_bDoRun == FALSE)
	{
		return false;
	}

	*tmStateStartDate = m_tmStateStartDate;
	GetLocalTime(tmStateEndDate);
	GetNextDay(tmStateEndDate,0,0,0,m_iCS_REMAIN_MSEC / 1000);
return true;
}

int CCastleSiege::GetCurRemainSec()
{
	if(m_iCS_REMAIN_MSEC < 0)
	{
		return -1;
	}
return m_iCS_REMAIN_MSEC / 1000;
}

void CCastleSiege::ClearDbNPC()
{
	for(int n = 0; n < OBJ_MAXMONSTER; n++)
	{
		if(gObj[n].MapNumber == MAP_INDEX_CASTLESIEGE
			&& (gObj[n].Class == 277 || gObj[n].Class == 283 || gObj[n].Class == 219))
		{
			if(gObj[n].Class == 277)
			{
				SetGateBlockState(gObj[n].X,gObj[n].Y,1);
				gObjAddBuffEffect(&gObj[n],BUFF_GATESTATUS,0,0,0,0,-10);
			}

			gObjDel(n);
		}
	}
}

void CCastleSiege::CreateNonDbNPC(int bDelFirst)
{
	if(bDelFirst != FALSE)
		ClearNonDbNPC();

	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while( it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_bIN_USE == TRUE && pNpcData.m_bNPC_DBSAVE == FALSE)
		{
			int iNPC_INDEX = gObjAddMonster(MAP_INDEX_CASTLESIEGE);

			if(iNPC_INDEX >= 0)
			{
				gObjSetMonster(iNPC_INDEX,pNpcData.m_iNPC_NUM);

				gObj[iNPC_INDEX].m_PosNum = -1;
				gObj[iNPC_INDEX].X = pNpcData.m_iNPC_SX;
				gObj[iNPC_INDEX].Y = pNpcData.m_iNPC_SY;
				gObj[iNPC_INDEX].MapNumber = MAP_INDEX_CASTLESIEGE;
				gObj[iNPC_INDEX].TX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].TY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].m_OldX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].m_OldY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].Dir = pNpcData.m_iNPC_DIR;
				gObj[iNPC_INDEX].StartX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].StartY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].DieRegen = FALSE;
				gObj[iNPC_INDEX].MaxRegenTime = NULL;
				gObj[iNPC_INDEX].Life = (float)pNpcData.m_iNPC_HP;
				gObj[iNPC_INDEX].MaxLife = (float)pNpcData.m_iNPC_MAXHP;

				switch(pNpcData.m_iNPC_SIDE)
				{
				case 1:
					gObj[iNPC_INDEX].m_btCsNpcType = 2;
					break;
				case 2:
					gObj[iNPC_INDEX].m_btCsNpcType = 3;
					break;
				default:
					gObj[iNPC_INDEX].m_btCsNpcType = 3;
					break;
				}

				it->m_btCsNpcExistVal1 = rand()%255+1;
				gObj[iNPC_INDEX].m_btCsNpcExistVal1 = it->m_btCsNpcExistVal1;

				it->m_btCsNpcExistVal2 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal2 = it->m_btCsNpcExistVal2;

				it->m_btCsNpcExistVal3 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal3 = it->m_btCsNpcExistVal3;

				it->m_btCsNpcExistVal4 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal4 = it->m_btCsNpcExistVal4;

				it->m_iNPC_OBJINDEX = iNPC_INDEX;
				it->m_iNPC_LIVE = 2;
			}
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
}

int CCastleSiege::CheckAddDbNPC(int iIndex,int iNpcType,int iNpcIndex,BYTE * btResult)
{
	int iRETVAL = FALSE;

	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == iNpcType
			&& pNpcData.m_iNPC_INDEX == iNpcIndex)
		{
			if(pNpcData.m_bIN_USE == TRUE
			&&pNpcData.m_iNPC_LIVE > 0
			&& gObjIsConnected(pNpcData.m_iNPC_OBJINDEX)
			&& gObj[pNpcData.m_iNPC_OBJINDEX].Class == iNpcType
			&& gObj[pNpcData.m_iNPC_OBJINDEX].Live == TRUE
			&& gObj[pNpcData.m_iNPC_OBJINDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
			&& gObj[pNpcData.m_iNPC_OBJINDEX].m_iCsNpcExistVal != 0)
			{
				*btResult = 0;
				iRETVAL = 0;
				break;
			}

			int iBuyCost = 0;

			switch(pNpcData.m_iNPC_NUM)
			{
			case 277:
				iBuyCost = pNpcData.m_iNPC_BASE_MAXHP * 5;
				break;
			case 283:
				iBuyCost = pNpcData.m_iNPC_BASE_MAXHP * 3;
				break;
			}
	
			if(gObj[iIndex].Money < iBuyCost)
			{
				*btResult = 3;
				iRETVAL = 0;
				break;
			}
			else
			{
				GS_GDReqCastleNpcBuy(m_iMapSvrGroup,iIndex,iNpcType,iNpcIndex,
					pNpcData.m_iNPC_DF_LEVEL,pNpcData.m_iNPC_RG_LEVEL,pNpcData.m_iNPC_MAXHP,pNpcData.m_iNPC_HP,pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY,pNpcData.m_iNPC_DIR,iBuyCost);

				*btResult = 1;
				iRETVAL = 1;
				break;
			}
		}
		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
return iRETVAL;
}

int CCastleSiege::GetCrownAccessTickCount()
{
	return m_dwCrownAccessTime;
}

struct PMSG_CROWNSWITCH_INFO
{
	PBMSG_HEAD2 h;
	BYTE IndexH;
	BYTE IndexL;
	BYTE bResult;
	BYTE btCsJoinSide;

	char szGuildName[8];
	char szName[10];
};

void CCastleSiege::NotifyCrownSwitchInfo(int iIndex)
{
	if(gObjIsConnected(iIndex) == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::NotifyCrownSwitchInfo() - iCrownSwitchIndex is Invalid");
		return;
	}

	if(gObj[iIndex].Class != 217 && gObj[iIndex].Class != 218)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::NotifyCrownSwitchInfo() - iCrownSwitchIndex is Invalid");
		return;
	}

	LPOBJ lpObj = &gObj[iIndex];

	if(lpObj->VPCount2 < 1)
		return;

	PMSG_CROWNSWITCH_INFO pMsg;

	pMsg.h.set((PBYTE)&pMsg,0xB2,0x20,sizeof(pMsg));

	pMsg.IndexH = SET_NUMBERH(iIndex & 0xFFFF);
	pMsg.IndexL = SET_NUMBERL(iIndex & 0xFFFF);

	pMsg.bResult = FALSE;
	pMsg.btCsJoinSide = NULL;

	memset(pMsg.szGuildName,0x00,sizeof(pMsg.szGuildName));
	memset(pMsg.szName,0x00,sizeof(pMsg.szName));

	int iCrownUser = g_CastleSiege.GetCrownSwitchUserIndex(lpObj->Class);

	if(gObjIsConnected(iCrownUser))
	{
		LPOBJ lpCrownUser = &gObj[iCrownUser];
		pMsg.bResult = TRUE;
		pMsg.btCsJoinSide = lpCrownUser->m_btCsJoinSide;

		memcpy(pMsg.szName,lpCrownUser->Name,sizeof(pMsg.szName));
		memcpy(pMsg.szGuildName,lpCrownUser->GuildName,sizeof(pMsg.szGuildName));

		char UnionName[8];

		memset(UnionName,0x00,sizeof(UnionName));

		if(lpCrownUser->lpGuild != NULL)
		{
			if(UnionManager.GetUnionName(lpCrownUser->lpGuild->iGuildUnion,UnionName) != -1)
			{
				memcpy(pMsg.szGuildName,UnionName,sizeof(pMsg.szGuildName));
			}
		}

		DataSend(iCrownUser,(PBYTE)&pMsg,pMsg.h.size);
	}

	int tObjNum = -1;

	for(int n = 0; n < MAX_VIEWPORT; n++)
	{
		tObjNum = lpObj->VpPlayer2[n].number;

		if(tObjNum >= 0 && gObj[tObjNum].Type == OBJ_USER && gObj[tObjNum].Live != FALSE)
		{
			DataSend(tObjNum,(PBYTE)&pMsg,pMsg.h.size);
		}
	}
}

int CCastleSiege::GetCrownSwitchUserIndex(int iMonsterClass)
{
int iUserIndex = -1;

	switch(iMonsterClass)
	{
	case 217:
		iUserIndex = m_iCastleSwitchAccessUser1;
		break;
	case 218:
		iUserIndex = m_iCastleSwitchAccessUser2;
		break;
	}
	return iUserIndex;
}

int CCastleSiege::GetCrownAccessUserX()
{
	return m_btCastleCrownAccessUserX;
}

int CCastleSiege::GetCrownAccessUserY()
{
	return m_btCastleCrownAccessUserY;
}

int CCastleSiege::GetCrownUserIndex()
{
	return m_iCastleCrownAccessUser;
}

void CCastleSiege::SetRegCrownAvailable(int bRegCrownAvaible)
{
	m_bRegCrownAvailable = bRegCrownAvaible;
}

int CCastleSiege::GetRegCrownAvailable()
{
	return m_bRegCrownAvailable;
}


int CCastleSiege::RepairDbNPC(int iNpcType, int iNpcIndex, int iNpcHP, int iNpcMaxHP)
{
	int bEXIST = FALSE;

	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == iNpcType
			&& pNpcData.m_iNPC_INDEX == iNpcIndex
			&& pNpcData.m_bIN_USE == TRUE
			&& pNpcData.m_iNPC_LIVE > 0)
		{
			it->m_iNPC_HP = iNpcHP;
			it->m_iNPC_MAXHP = iNpcMaxHP;

			int iNPC_INDEX = pNpcData.m_iNPC_OBJINDEX;

			if(gObjIsConnected(iNPC_INDEX)
				&& gObj[iNPC_INDEX].m_btCsNpcType != FALSE
				&& gObj[iNPC_INDEX].Class == iNpcType
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal != FALSE)
			{
				gObj[iNPC_INDEX].Life = gObj[iNPC_INDEX].MaxLife;
				bEXIST = TRUE;
			}

			break;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
	return bEXIST;
}

#define CS_UPGRADE_RANGE(value) ( (( (value)   )< 0)?FALSE:(( (value)  )> CS_MAX_UPGRADE - 1)?FALSE:TRUE  )

int CCastleSiege::PayForUpgradeDbNPC(int iIndex, int iNpcType, int iNpcIndex, int iNpcUpType, int iNpcUpValue, int iNpcUpIndex)
{

	int iNEED_GEMOFDEFEND = 0;
	int iNEED_MONEY = 0;

	if(CS_UPGRADE_RANGE(iNpcUpIndex) == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::PayForUpgradeDbNPC() ERROR - lpMsg->iNpcUpIndex is Out of Bound : %d",
			iNpcUpIndex);
		return false;
	}

	_CS_NPC_DATA pNpcData;

	int bIsLive = GetNpcData(iNpcType,iNpcIndex,(_CS_NPC_DATA &)pNpcData);

	if(bIsLive == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::PayForUpgradeDbNPC() ERROR - bIsLive == FALSE",
			iNpcIndex);
		return false;
	}

	switch(iNpcType)
	{
	case 277:
		switch(iNpcUpType)
		{
		case 1:
			{
				iNEED_GEMOFDEFEND = g_iNpcUpDfLevel_CGATE[iNpcUpIndex][1];
				iNEED_MONEY = g_iNpcUpDfLevel_CGATE[iNpcUpIndex][2];
	
				if(gObj[iIndex].Money < iNEED_MONEY)
				{
					GCAnsNpcUpgrade(iIndex,3,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
					return false;
				}
	
				int bDelGemOfDefend = DelGemOfDefend(iIndex,iNEED_GEMOFDEFEND);
	
				if(bDelGemOfDefend == FALSE)
				{
					GCAnsNpcUpgrade(iIndex,4,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
					return false;
				}
				gObj[iIndex].Money -= iNEED_MONEY;
			}
			break;
		case 3:
			{
				iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CGATE[iNpcUpIndex][1];
				iNEED_MONEY = g_iNpcUpMaxHP_CGATE[iNpcUpIndex][2];
	
				if(gObj[iIndex].Money < iNEED_MONEY)
				{
					GCAnsNpcUpgrade(iIndex,3,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
					return false;
				}
	
				int bDelGemOfDefend = DelGemOfDefend(iIndex,iNEED_GEMOFDEFEND);
	
				if(bDelGemOfDefend == FALSE)
				{
					GCAnsNpcUpgrade(iIndex,4,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
					return false;
				}
				gObj[iIndex].Money -= iNEED_MONEY;
			}
			break;
		default:
			GCAnsNpcUpgrade(iIndex,5,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
			return false;
		}
		break;
	case 283:
		{
			switch(iNpcUpType)
			{
			case 1:
				{
					iNEED_GEMOFDEFEND = g_iNpcUpDfLevel_CSTATUE[iNpcUpIndex][1];
					iNEED_MONEY = g_iNpcUpDfLevel_CSTATUE[iNpcUpIndex][2];

					if(gObj[iIndex].Money < iNEED_MONEY)
					{
						GCAnsNpcUpgrade(iIndex,3,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
						return false;
					}

					int bDelGemOfDefend = DelGemOfDefend(iIndex,iNEED_GEMOFDEFEND);
					
					if(bDelGemOfDefend == FALSE)
					{
						GCAnsNpcUpgrade(iIndex,4,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
						return false;
					}
					gObj[iIndex].Money -= iNEED_MONEY;
				}
				break;
			case 2:
				{
					iNEED_GEMOFDEFEND = g_iNpcUpRgLevel_CSTATUE[iNpcUpIndex][1];
					iNEED_MONEY = g_iNpcUpRgLevel_CSTATUE[iNpcUpIndex][2];

					if(gObj[iIndex].Money < iNEED_MONEY)
					{
						GCAnsNpcUpgrade(iIndex,3,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
						return false;
					}

					int bDelGemOfDefend = DelGemOfDefend(iIndex,iNEED_GEMOFDEFEND);
					
					if(bDelGemOfDefend == FALSE)
					{
						GCAnsNpcUpgrade(iIndex,4,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
						return false;
					}
					gObj[iIndex].Money -= iNEED_MONEY;
				}
				break;
			case 3:
				{
					iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CSTATUE[iNpcUpIndex][1];
					iNEED_MONEY = g_iNpcUpMaxHP_CSTATUE[iNpcUpIndex][2];
					
					if(gObj[iIndex].Money < iNEED_MONEY)
					{
						GCAnsNpcUpgrade(iIndex,3,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
						return false;
					}
					
					int bDelGemOfDefend = DelGemOfDefend(iIndex,iNEED_GEMOFDEFEND);
					
					if(bDelGemOfDefend == FALSE)
					{
						GCAnsNpcUpgrade(iIndex,4,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
						return false;
					}
					gObj[iIndex].Money -= iNEED_MONEY;
				}
				break;
			default:
				GCAnsNpcUpgrade(iIndex,5,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
				return false;
			}
		}
		break;
	}

	GCMoneySend(iIndex,gObj[iIndex].Money);
	return true;
}

int CCastleSiege::UpgradeDbNPC(int iIndex, int iNpcType, int iNpcIndex, int iNpcUpType, int iNpcUpValue, int iNpcUpIndex)
{
	if(CS_UPGRADE_RANGE(iNpcUpIndex) == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::UpgradeDbNPC() ERROR - lpMsg->iNpcUpIndex is Out of Bound : %d",iNpcUpIndex);
		return false;
	}

	_CS_NPC_DATA pNpcData;

	int bIsLive = GetNpcData(iNpcType,iNpcIndex,(_CS_NPC_DATA &)pNpcData);

	if(bIsLive == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::UpgradeDbNPC() ERROR - bIsLive == FALSE");
		return false;
	}

	switch(iNpcType)
	{
	case 277:
		switch(iNpcUpType)
		{
		case 1:
			UpgradeDbNPC_DFLEVEL(iNpcType,iNpcIndex,iNpcUpValue);
			break;
		case 3:
			UpgradeDbNPC_MAXHP(iNpcType,iNpcIndex,iNpcUpValue);
			break;
		default:
			GCAnsNpcUpgrade(iIndex,5,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
			return false;
		}
		break;
	case 283:
		switch(iNpcUpType)
		{
		case 1:
			UpgradeDbNPC_DFLEVEL(iNpcType,iNpcIndex,iNpcUpValue);
			break;
		case 2:
			UpgradeDbNPC_RGLEVEL(iNpcType,iNpcIndex,iNpcUpValue);
			break;
		case 3:
			UpgradeDbNPC_MAXHP(iNpcType,iNpcIndex,iNpcUpValue);
			break;
		default:
			GCAnsNpcUpgrade(iIndex,5,iNpcType,iNpcIndex,iNpcUpType,iNpcUpValue);
			return false;
		}
		break;
	}

	return true;
}

int CCastleSiege::GetNpcData(int iNpcType,int iNpcIndex,_CS_NPC_DATA & pRetNpcData)
{
	pRetNpcData.Clear();
	int bEXIST = FALSE;

	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{

		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == iNpcType
			&& pNpcData.m_iNPC_INDEX == iNpcIndex
			&& pNpcData.m_bIN_USE == TRUE
			&& pNpcData.m_iNPC_LIVE > 0)
		{
			int iNPC_INDEX = pNpcData.m_iNPC_OBJINDEX;

			if(gObjIsConnected(iNPC_INDEX)
				&& gObj[iNPC_INDEX].m_btCsNpcType != 0
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal != 0)
			{
				pRetNpcData = pNpcData;
				bEXIST = TRUE;
			}
			break;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);

	return bEXIST;
}

int CCastleSiege::DelGemOfDefend(int iIndex,int iNeedGemOfDefend)
{
	if(gObjIsConnected(iIndex) == FALSE)
		return false;

	if(iNeedGemOfDefend <= 0)
		return false;

	int iGemOfDefendCount = 0;

	int x;
	for(x = 0; x < INVENTORY_SIZE; x++)
	{
		if(gObj[iIndex].pInventory[x].IsItem() == TRUE
			&& gObj[iIndex].pInventory[x].m_Type == ITEMGET(14,31))
		{
			iGemOfDefendCount++;
		}
	}

	if(iGemOfDefendCount < iNeedGemOfDefend)
		return false;

	int iGemOfDefendDeletCount = 0;

	for(x = 0; x < INVENTORY_SIZE; x++)
	{
		if(gObj[iIndex].pInventory[x].IsItem() == TRUE
			&& gObj[iIndex].pInventory[x].m_Type == ITEMGET(14,31))
		{
			LogAddTD("[CastleSiege] [%s][%s] Deleting Gem of Defend (GUILD:%s) - Serial:%d",
				gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].pInventory[x].m_Number);

			gObjInventoryItemSet(iIndex,x,0xFF);
			gObjInventoryDeleteItem(iIndex,x);
			GCInventoryItemDeleteSend(iIndex,x,TRUE);

			iGemOfDefendDeletCount++;

			if(iGemOfDefendDeletCount >= iNeedGemOfDefend)
				break;
		}
	}
	return true;
}

int CCastleSiege::UpgradeDbNPC_DFLEVEL(int iNpcType,int iNpcIndex,int iNpcDfLevel)
{
	int bEXIST = FALSE;
	vector<_CS_NPC_DATA>::iterator it;

	EnterCriticalSection(&m_critNpcData);

	it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == iNpcType
			&& pNpcData.m_iNPC_INDEX == iNpcIndex
			&& pNpcData.m_bIN_USE == TRUE
			&& pNpcData.m_iNPC_LIVE > 0)
		{
			int iNPC_INDEX = pNpcData.m_iNPC_OBJINDEX;

			if(gObjIsConnected(iNPC_INDEX)
				&& gObj[iNPC_INDEX].m_btCsNpcType != 0
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal != 0)
			{
				it->m_iNPC_DF_LEVEL = iNpcDfLevel;
				bEXIST = TRUE;
				gObj[iNPC_INDEX].m_Defense = g_iNpcDefense_CGATE[iNpcDfLevel];
				gObj[iNPC_INDEX].m_MagicDefense = g_iNpcDefense_CGATE[iNpcDfLevel];
				gObj[iNPC_INDEX].m_btCsNpcDfLevel = iNpcDfLevel;
			}

			break;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
	return bEXIST;
}

int CCastleSiege::UpgradeDbNPC_MAXHP(int iNpcType,int iNpcIndex,int iNpcMaxHP)
{
	int bEXIST = FALSE;
	vector<_CS_NPC_DATA>::iterator it;

	EnterCriticalSection(&m_critNpcData);

	it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == iNpcType
			&& pNpcData.m_iNPC_INDEX == iNpcIndex
			&& pNpcData.m_bIN_USE == TRUE
			&& pNpcData.m_iNPC_LIVE > 0)
		{
			int iNPC_INDEX = pNpcData.m_iNPC_OBJINDEX;

			if(gObjIsConnected(iNPC_INDEX)
				&& gObj[iNPC_INDEX].m_btCsNpcType != 0
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal != 0)
			{
				it->m_iNPC_MAXHP = iNpcMaxHP;
				it->m_iNPC_HP = iNpcMaxHP;
				bEXIST = TRUE;

				gObj[iNPC_INDEX].MaxLife = (float)iNpcMaxHP;
				gObj[iNPC_INDEX].Life = (float)iNpcMaxHP;
			}

			break;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
	return bEXIST;
}

int CCastleSiege::UpgradeDbNPC_RGLEVEL(int iNpcType,int iNpcIndex,int iNpcRgLevel)
{
	int bEXIST = FALSE;
	vector<_CS_NPC_DATA>::iterator it;

	EnterCriticalSection(&m_critNpcData);

	it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == iNpcType
			&& pNpcData.m_iNPC_INDEX == iNpcIndex
			&& pNpcData.m_bIN_USE == TRUE
			&& pNpcData.m_iNPC_LIVE > 0)
		{
			int iNPC_INDEX = pNpcData.m_iNPC_OBJINDEX;

			if(gObjIsConnected(iNPC_INDEX)
				&& gObj[iNPC_INDEX].m_btCsNpcType != 0
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
				&& gObj[iNPC_INDEX].m_iCsNpcExistVal != 0)
			{
				it->m_iNPC_RG_LEVEL = iNpcRgLevel;
				bEXIST = TRUE;
				gObj[iNPC_INDEX].m_btCsNpcRgLevel = iNpcRgLevel;
			}

			break;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);
	return bEXIST;
}

int CCastleSiege::DelNPC(int iIndex, int iNpcType, int iMonsterExistVal, int bDbSave)
{

	if(OBJMAX_RANGE(iIndex) == FALSE)
		return false;

	int bEXIST = FALSE;
	int bNPC_DB = FALSE;

	_CS_NPC_DATA pTempNpcData;
	vector<_CS_NPC_DATA>::iterator it;

	EnterCriticalSection(&m_critNpcData);

	it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);


		if(pNpcData.m_iNPC_NUM == iNpcType
			&& pNpcData.m_iNPC_OBJINDEX == iIndex
			&& pNpcData.m_iCsNpcExistVal == iMonsterExistVal
			&& pNpcData.m_bIN_USE == TRUE
			&& pNpcData.m_iNPC_LIVE > NULL)
		{
			it->SetBaseValue();
			it->m_iNPC_OBJINDEX = -1;
			it->m_iNPC_LIVE = NULL;

			if(pNpcData.m_iNPC_NUM == 277 || pNpcData.m_iNPC_NUM == 283)
			{
				pNpcData.m_iNPC_HP = pNpcData.m_iNPC_BASE_HP;
				pNpcData.m_iNPC_MAXHP = pNpcData.m_iNPC_BASE_MAXHP;
				pNpcData.m_iNPC_RG_LEVEL = pNpcData.m_iNPC_BASE_RG_LEVEL;
				pNpcData.m_iNPC_DF_LEVEL = pNpcData.m_iNPC_BASE_DF_LEVEL;
			}

			bEXIST = TRUE;
			pTempNpcData = pNpcData;

			if(pNpcData.m_iNPC_NUM == 277)
			{
				SetGateBlockState(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY,1);
				gObjAddBuffEffect(&gObj[pNpcData.m_iNPC_OBJINDEX],BUFF_GATESTATUS,0,0,0,0,-10);
				LogAddTD("[CastleSiege][ State:%d ] Delete Castle Gate ( CLS:%d, IDX:%d, X:%d. Y:%d )",
					m_iCastleSiegeState,pNpcData.m_iNPC_NUM,pNpcData.m_iNPC_INDEX,pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY);
			}

			if(pNpcData.m_iNPC_NUM == 283 &&  m_iCastleSiegeState == CASTLESIEGE_STATE_STARTSIEGE)
			{
				if(CheckGuardianStatueExist() == FALSE)
				{
					ClearCastleTowerBarrier();
					SetCastleTowerAccessable(TRUE);
				}

				LogAddTD("[CastleSiege] Defend Statue is Broken (CLS:%d, IDX:%d, X:%d, Y:%d)",
					pNpcData.m_iNPC_NUM,pNpcData.m_iNPC_INDEX,pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY);
			}

			break;

		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);

	if(bEXIST == TRUE && bDbSave == TRUE && m_iCastleSiegeState != CASTLESIEGE_STATE_STARTSIEGE)
	{
		GS_GDReqNpcRemove(m_iMapSvrGroup,pTempNpcData.m_iNPC_NUM,pTempNpcData.m_iNPC_INDEX);
	}
	return true;
}

struct PMSG_ANS_NPCDBLIST
{
	PWMSG_HEAD h;
	unsigned char btResult;
	int iCount;
};

struct PMSG_NPCDBLIST
{
	int iNpcNumber;
	int iNpcIndex;
	int iNpcDfLevel;
	int iNpcRgLevel;
	int iNpcMaxHp;
	int iNpcHp;
	unsigned char btNpcX;
	unsigned char btNpcY;
	unsigned char btNpcLive;
};

void CCastleSiege::SendNpcStateList(int iIndex, int iNpcType)
{
	if(gObjIsConnected(iIndex) == FALSE)
		return;

	char cNPC_LIST[0x584] = {0};
	int iNPC_COUNT = 0;

	PMSG_ANS_NPCDBLIST * lpMsgSend = (PMSG_ANS_NPCDBLIST *)cNPC_LIST;
	PMSG_NPCDBLIST * lpMsgSendBody = (PMSG_NPCDBLIST *)&cNPC_LIST[sizeof(PMSG_ANS_NPCDBLIST)];

	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{

		if(iNPC_COUNT >= 50)
			break;

		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == iNpcType && pNpcData.m_bIN_USE == TRUE)
		{
			int iHP = 0;
			int iMAX_HP = 0;
			unsigned char btLIVE = 0;
			int iNPC_INDEX = pNpcData.m_iNPC_OBJINDEX;

			if(gObjIsConnected(iNPC_INDEX))
			{
				if(gObj[iNPC_INDEX].Class == iNpcType
					&& gObj[iNPC_INDEX].Live == TRUE
					&& gObj[iNPC_INDEX].Life > 0.0f
					&& gObj[iNPC_INDEX].m_btCsNpcType != FALSE
					&& gObj[iNPC_INDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
					&& gObj[iNPC_INDEX].m_iCsNpcExistVal != FALSE)
				{
					iHP = (int)gObj[iNPC_INDEX].Life;
					iMAX_HP = (int)gObj[iNPC_INDEX].MaxLife;
					btLIVE = TRUE;
				}
				else
				{
					iHP = 0;
					iMAX_HP = pNpcData.m_iNPC_BASE_MAXHP;
					btLIVE = FALSE;
				}
			}
			else
			{
				iHP = 0;
				iMAX_HP = pNpcData.m_iNPC_BASE_MAXHP;
				btLIVE = FALSE;
			}

			lpMsgSendBody[iNPC_COUNT].iNpcNumber = pNpcData.m_iNPC_NUM;
			lpMsgSendBody[iNPC_COUNT].iNpcIndex = pNpcData.m_iNPC_INDEX;
			lpMsgSendBody[iNPC_COUNT].iNpcDfLevel = pNpcData.m_iNPC_DF_LEVEL;
			lpMsgSendBody[iNPC_COUNT].iNpcRgLevel = pNpcData.m_iNPC_RG_LEVEL;
			lpMsgSendBody[iNPC_COUNT].iNpcHp = iHP;
			lpMsgSendBody[iNPC_COUNT].iNpcMaxHp = iMAX_HP;
			lpMsgSendBody[iNPC_COUNT].btNpcX = pNpcData.m_iNPC_SX;
			lpMsgSendBody[iNPC_COUNT].btNpcY = pNpcData.m_iNPC_SY;
			lpMsgSendBody[iNPC_COUNT].btNpcLive = btLIVE;
			iNPC_COUNT++;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);

	lpMsgSend->btResult = TRUE;
	lpMsgSend->h.set((PBYTE)cNPC_LIST,0xB3,iNPC_COUNT * sizeof(PMSG_NPCDBLIST) + sizeof(PMSG_ANS_NPCDBLIST));
	lpMsgSend->iCount = iNPC_COUNT;

	DataSend(iIndex,(PBYTE)cNPC_LIST,iNPC_COUNT * sizeof(PMSG_NPCDBLIST) + sizeof(PMSG_ANS_NPCDBLIST));
}


void CCastleSiege::SendCsGateStateViewPort(int iGateIndex, BYTE btOperate)
{
	if(gObjIsConnected(iGateIndex) == FALSE)
		return;
	LPOBJ lpObj = &gObj[iGateIndex];

	if(lpObj->VPCount2 < 1)
		return;

	int tObjNum = -1;

	for(int i = 0; i < MAX_VIEWPORT;i++)
	{
		tObjNum = lpObj->VpPlayer2[i].number;

		if(tObjNum >= 0 && gObj[tObjNum].Type == OBJ_USER && gObjIsConnected(tObjNum))
		{
			GCAnsCsGateCurState(tObjNum,iGateIndex,btOperate);
		}
	}
}

int CCastleSiege::CheckCsGateAlive(int iGateIndex)
{
	if(gObjIsConnected(iGateIndex) == FALSE || gObj[iGateIndex].Class != 277)
	{
		return false;
	}
	return true;
}

void CCastleSiege::SetCrownIndex(int iIndex)
{
	m_iCrownIndex = iIndex;
}

int CCastleSiege::GetCastleTowerAccessable()
{
	return m_bCastleTowerAccessable;
}

void CCastleSiege::SetTaxRate(int iTaxType, int iTaxRate)
{
	int iMaxTaxRate = 0;

	switch(iTaxType)
	{
	case 1:
		iMaxTaxRate = 3;
		break;
	case 2:
		iMaxTaxRate = 3;
		break;
	case 3:
		iMaxTaxRate = 300000;
		break;
	}

	if(iTaxRate < 0 || iTaxRate > iMaxTaxRate)
		return;

	switch(iTaxType)
	{
	case 1:
		m_iTaxRateChaos = iTaxRate;
		break;
	case 2:
		m_iTaxRateStore = iTaxRate;
		break;
	case 3:
		m_iTaxHuntZone = iTaxRate;
		break;
	}

	LogAddTD("[CastleSiege] Tax Rate Setted - Type:%d, Rate:%d",iTaxType,iTaxRate);
}

int CCastleSiege::CheckCastleHasMoney(__int64 i64CastleMoney)
{
	if(m_i64CastleMoney >= i64CastleMoney)
	{
		return true;
	}
return false;
}

void CCastleSiege::SetCastleMoney(__int64 i64CastleMoney)
{
	m_i64CastleMoney = i64CastleMoney;
}

void CCastleSiege::ResetCastleTaxInfo()
{
	m_i64CastleMoney = 0;
	m_iTaxRateChaos = 0;
	m_iTaxRateStore = 0;
	m_iTaxHuntZone = 0;
}

void CCastleSiege::SetIsSiegeGuildList(int bIsSiegGuildList)
{
	m_btIsSiegeGuildList = bIsSiegGuildList;
}

void CCastleSiege::SendCastleStateSync()
{
	GS_GDReqCastleStateSync(m_iMapSvrGroup,m_iCastleSiegeState,m_iTaxRateChaos,m_iTaxRateStore,m_iTaxHuntZone,m_szCastleOwnerGuild);
}

int CCastleSiege::GetDbNpcCreated()
{
	return m_bDbNpcCreated;
}

void CCastleSiege::SetDbNpcCreated(int bDbNpcCreated)
{
	m_bDbNpcCreated = bDbNpcCreated;
}

int CCastleSiege::CheckGuildOwnCastle(char * lpszGuildName)
{
	if(lpszGuildName == NULL)
		return false;

	if(strcmp(m_szCastleOwnerGuild,"") == NULL)
		return false;

	if(strcmp(m_szCastleOwnerGuild,lpszGuildName) == NULL)
		return true;

return false;
}

void CCastleSiege::ResetCastleCycle()
{
	m_bCsBasicGuildInfoLoadOK = FALSE;
	m_bCsTotalGuildInfoLoadOK = FALSE;

	EnterCriticalSection(&m_critCsBasicGuildInfo);
	m_mapCsBasicGuildInfo.clear();
	LeaveCriticalSection(&m_critCsBasicGuildInfo);

	EnterCriticalSection(&m_critCsTotalGuildInfo);
	m_mapCsTotalGuildInfo.clear();
	LeaveCriticalSection(&m_critCsTotalGuildInfo);

	m_bCastleTowerAccessable = FALSE;
	m_bRegCrownAvailable = FALSE;
	m_btIsSiegeGuildList = FALSE;
	 m_btIsSiegeEnded = FALSE;
}

int CCastleSiege::GetHuntZoneEnter()
{
	return m_bEnterHuntZone;
}

void CCastleSiege::SetHuntZoneEnter(int bEnterHuntZone)
{
	m_bEnterHuntZone = bEnterHuntZone;
}

struct CSP_REQ_CSGUILDUNIONINFO
{
	PWMSG_HEAD h;
	unsigned short wMapSvrNum;
	int iCount;
};

void CCastleSiege::SetCalcRegGuildList(CSP_CALCREGGUILDLIST * lpMsg, int iCOUNT)
{
	if(lpMsg == NULL)
		return;

	if(iCOUNT > 100)
		iCOUNT = 100;

	if(iCOUNT <= 0)
		return;

	m_bCsBasicGuildInfoLoadOK = FALSE;
	m_bCsTotalGuildInfoLoadOK = FALSE;

	vector<_CS_REG_GUILD_DATA> vtRegGuildScore;

	int iGCNT;
	for(iGCNT = 0; iGCNT < iCOUNT; iGCNT++)
	{
		_CS_REG_GUILD_DATA pRegGuildData;
		char szGuildName[0x10] = {0};

		memcpy(szGuildName,lpMsg[iGCNT].szGuildName,8);

		pRegGuildData.m_bIN_USE = TRUE;
		pRegGuildData.m_strGuildName = szGuildName;
		pRegGuildData.m_iRegMarkCount = lpMsg[iGCNT].iRegMarkCount;
		pRegGuildData.m_iGuildMasterLevel = lpMsg[iGCNT].iGuildMasterLevel;
		pRegGuildData.m_iGuildMemberCount = lpMsg[iGCNT].iGuildMemberCount;
		pRegGuildData.m_iSeqNum = 0x100000 - lpMsg[iGCNT].iSeqNum;

		int iTotalScore =  (lpMsg[iGCNT].iRegMarkCount * 5 + lpMsg[iGCNT].iGuildMemberCount) + (lpMsg[iGCNT].iGuildMasterLevel / 4);

		pRegGuildData.m_i64TotolScore = unsigned int(__int64(lpMsg[iGCNT].iSeqNum) & 0xFFFFFFFF) | __int64(unsigned int(__int64(iTotalScore) & 0xFFFFFFFF)) << 0x20;

		vtRegGuildScore.push_back(pRegGuildData);

		LogAddTD("[CastleSiege][REG Mark Count] - [%s][%d]",szGuildName,lpMsg[iGCNT].iRegMarkCount);
	}

	if(vtRegGuildScore.empty())
		return;

	sort(vtRegGuildScore.begin(),vtRegGuildScore.end(),RegGuildScoreCompFunc);

	int iGUILD_COUNT = vtRegGuildScore.size();

	if(iGUILD_COUNT > 3)
	{
		iGUILD_COUNT = 3;
	}

	EnterCriticalSection(&m_critCsBasicGuildInfo);

	m_mapCsBasicGuildInfo.clear();

	for(iGCNT = 0; iGCNT < iGUILD_COUNT; iGCNT ++)
	{
		_CS_TOTAL_GUILD_DATA pTotalGuildData;

		pTotalGuildData.m_bIN_USE = TRUE;
		pTotalGuildData.m_iCsGuildID = iGCNT + 2;
		pTotalGuildData.m_bGuildInvolved =  TRUE;
		pTotalGuildData.m_iTotalScore = (vtRegGuildScore[iGCNT].m_iRegMarkCount * 5 + vtRegGuildScore[iGCNT].m_iGuildMemberCount) + (vtRegGuildScore[iGCNT].m_iGuildMasterLevel / 4);

		//

		m_mapCsBasicGuildInfo.insert(pair<basic_string<char,char_traits<char> >,_CS_TOTAL_GUILD_DATA>(vtRegGuildScore[iGCNT].m_strGuildName,pTotalGuildData));
	}

	if(m_btIsCastleOccupied == TRUE && strcmp(m_szCastleOwnerGuild,"") != NULL)
	{
		_CS_TOTAL_GUILD_DATA pTotalGuildData;

		pTotalGuildData.m_bIN_USE = TRUE;
		pTotalGuildData.m_iCsGuildID = 1;
		pTotalGuildData.m_bGuildInvolved = TRUE;
		pTotalGuildData.m_iTotalScore = 0;

		m_mapCsBasicGuildInfo.insert(pair<basic_string<char,char_traits<char> >,_CS_TOTAL_GUILD_DATA>(m_szCastleOwnerGuild,pTotalGuildData));
	}

	LeaveCriticalSection(&m_critCsBasicGuildInfo);

	char cBUFFER[0x820];

	CSP_REQ_CSGUILDUNIONINFO * lpMsgSend = (CSP_REQ_CSGUILDUNIONINFO *)cBUFFER;
	CSP_CSGUILDUNIONINFO * lpMsgSendBody = (CSP_CSGUILDUNIONINFO *)&cBUFFER[sizeof(CSP_REQ_CSGUILDUNIONINFO)];

	lpMsgSend->wMapSvrNum = m_iMapSvrGroup;
	lpMsgSend->iCount = 0;

	EnterCriticalSection(&m_critCsTotalGuildInfo);

	m_mapCsTotalGuildInfo.clear();

	//for(_CS_TOTAL_GUILD_DATA_MAP::iterator it = (_CS_TOTAL_GUILD_DATA_MAP::iterator)m_mapCsBasicGuildInfo.begin();it != m_mapCsBasicGuildInfo.end(); it++)
	for(_CS_TOTAL_GUILD_DATA_MAP::iterator it = m_mapCsBasicGuildInfo.begin();it != m_mapCsBasicGuildInfo.end(); it++)
	{
		if(lpMsgSend->iCount > 150)
			break;

		m_mapCsTotalGuildInfo.insert(pair<basic_string<char,char_traits<char> >,_CS_TOTAL_GUILD_DATA>(it->first,it->second));

		basic_string<char,char_traits<char> > strGuildName = it->first;

		memcpy(lpMsgSendBody[lpMsgSend->iCount].szGuildName,&strGuildName[0],8);
		lpMsgSendBody[lpMsgSend->iCount].iCsGuildID = it->second.m_iCsGuildID;
		lpMsgSend->iCount++;
	}

	LeaveCriticalSection(&m_critCsTotalGuildInfo);

	m_bCsBasicGuildInfoLoadOK = TRUE;

	lpMsgSend->h.set((PBYTE)lpMsgSend,0x86,sizeof(CSP_REQ_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * lpMsgSend->iCount);
	cDBSMng.Send((char *)lpMsgSend,sizeof(CSP_REQ_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * lpMsgSend->iCount);
}

BYTE CCastleSiege::RegGuildScoreCompFunc(_CS_REG_GUILD_DATA & A,_CS_REG_GUILD_DATA & B)
{
	if(A.m_i64TotolScore < B.m_i64TotolScore)
	{
		return false;
	}
	return true;
}

void CCastleSiege::MakeCsTotalGuildInfo(CSP_CSGUILDUNIONINFO * lpMsg, int iCOUNT)
{
	if(lpMsg == NULL)
		return;

	if(m_bCsBasicGuildInfoLoadOK == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::MakeCsTotalGuildInfo() ERROR - m_bCsBasicGuildInfoLoadOK == FALSE");
		return;
	}

	if(iCOUNT <= 0)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::MakeCsTotalGuildInfo() ERROR - iCOUNT <= 0 : %d",iCOUNT);
		return;
	}

	EnterCriticalSection(&m_critCsTotalGuildInfo);

	for(int iGCNT = 0; iGCNT < iCOUNT; iGCNT++)
	{
		char szGuildName[0x10] = {0};

		memcpy(szGuildName,lpMsg[iGCNT].szGuildName,8);

		_CS_TOTAL_GUILD_DATA_MAP::iterator it = m_mapCsTotalGuildInfo.find(szGuildName);

		if(it != m_mapCsTotalGuildInfo.end())
			continue;

		_CS_TOTAL_GUILD_DATA pTotalGuildData;

		pTotalGuildData.m_bIN_USE = TRUE;
		pTotalGuildData.m_iCsGuildID = lpMsg[iGCNT].iCsGuildID;
		pTotalGuildData.m_bGuildInvolved = FALSE;
		pTotalGuildData.m_iTotalScore = 0;

		m_mapCsTotalGuildInfo.insert(pair<basic_string<char,char_traits<char> >,_CS_TOTAL_GUILD_DATA>(szGuildName,pTotalGuildData));
	}

	LeaveCriticalSection(&m_critCsTotalGuildInfo);

	m_bCsTotalGuildInfoLoadOK = TRUE;
	SaveCsTotalGuildInfo();
	SetAllUserCsJoinSide();
}

struct CSP_REQ_CSSAVETOTALGUILDINFO
{
	PWMSG_HEAD h;
	unsigned short wMapSvrNum;
	int iCount;
};

struct CSP_CSSAVETOTALGUILDINFO
{
	char szGuildName[0x8];
	int iCsGuildID;
	int iGuildInvolved;
};

void CCastleSiege::SaveCsTotalGuildInfo()
{
	if(m_bCsTotalGuildInfoLoadOK == FALSE)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::SaveCsTotalGuildInfo() ERROR - m_bCsTotalGuildInfoLoadOK == FALSE");
		return;
	}

	char cBUFFER[0xA7C];

	CSP_REQ_CSSAVETOTALGUILDINFO * lpMsgSend = (CSP_REQ_CSSAVETOTALGUILDINFO *)cBUFFER;
	CSP_CSSAVETOTALGUILDINFO * lpMsgSendBody = (CSP_CSSAVETOTALGUILDINFO *)&cBUFFER[sizeof(CSP_REQ_CSSAVETOTALGUILDINFO)];

	lpMsgSend->wMapSvrNum = m_iMapSvrGroup;
	lpMsgSend->iCount = 0;

	EnterCriticalSection(&m_critCsTotalGuildInfo);

	for(_CS_TOTAL_GUILD_DATA_MAP::iterator it = m_mapCsTotalGuildInfo.begin();it != m_mapCsTotalGuildInfo.end(); it++)
	{
		if(lpMsgSend->iCount > 150)
			break;

		basic_string<char,char_traits<char> > strGuildName = it->first;

		memcpy(lpMsgSendBody[lpMsgSend->iCount].szGuildName,&strGuildName[0],8);
		lpMsgSendBody[lpMsgSend->iCount].iCsGuildID = it->second.m_iCsGuildID;
		lpMsgSendBody[lpMsgSend->iCount].iGuildInvolved = it->second.m_bGuildInvolved;

		lpMsgSend->iCount++;
	}

	LeaveCriticalSection(&m_critCsTotalGuildInfo);

	lpMsgSend->h.set((PBYTE)lpMsgSend,0x87,sizeof(CSP_REQ_CSSAVETOTALGUILDINFO) + sizeof(CSP_CSSAVETOTALGUILDINFO) * lpMsgSend->iCount);
	cDBSMng.Send((char *)lpMsgSend,sizeof(CSP_REQ_CSSAVETOTALGUILDINFO) + sizeof(CSP_CSSAVETOTALGUILDINFO) * lpMsgSend->iCount);

	EnterCriticalSection(&m_critCsTotalGuildInfo);

	_CS_TOTAL_GUILD_DATA_MAP::iterator it2 = m_mapCsTotalGuildInfo.begin();

	for(int iSize = m_mapCsTotalGuildInfo.size();it2 != m_mapCsTotalGuildInfo.end(); it2++)
	{
		basic_string<char,char_traits<char> > strGuildName = it2->first;

		_CS_TOTAL_GUILD_DATA stTEMP = it2->second;

		LogAddTD("[CastleSiege] Siege Guild List Save - GNAME:%s, CSGUID:%d, INVOLVED:%d",
			&strGuildName[0], stTEMP.m_iCsGuildID, stTEMP.m_bGuildInvolved);
	}

	LeaveCriticalSection(&m_critCsTotalGuildInfo);

}

void CCastleSiege::SetCsTotalGuildInfo(CSP_CSLOADTOTALGUILDINFO * lpMsg, int iCOUNT)
{
	if(lpMsg == NULL)
		return;

	if(iCOUNT <= 0)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::SetCsTotalGuildInfo() ERROR - iCOUNT <= 0 : %d", iCOUNT);
		return;
	}

	for(int iGCNT = 0; iGCNT < iCOUNT; iGCNT++)
	{
		char szGuildName[0x10] = {0};

		memcpy(szGuildName,lpMsg[iGCNT].szGuildName,8);
		_CS_TOTAL_GUILD_DATA pTotalGuildData;
		pTotalGuildData.m_bIN_USE = TRUE;
		pTotalGuildData.m_iCsGuildID = lpMsg[iGCNT].iCsGuildID;
		pTotalGuildData.m_bGuildInvolved = lpMsg[iGCNT].iGuildInvolved;

		EnterCriticalSection(&m_critCsTotalGuildInfo);

		m_mapCsTotalGuildInfo.insert(pair<basic_string<char,char_traits<char> >,_CS_TOTAL_GUILD_DATA>(szGuildName,pTotalGuildData));

		LeaveCriticalSection(&m_critCsTotalGuildInfo);

		if(lpMsg[iGCNT].iGuildInvolved == TRUE)
		{
			EnterCriticalSection(&m_critCsBasicGuildInfo);
			m_mapCsBasicGuildInfo.insert(pair<basic_string<char,char_traits<char> >,_CS_TOTAL_GUILD_DATA>(szGuildName,pTotalGuildData));
			LeaveCriticalSection(&m_critCsBasicGuildInfo);
		}
	}

	m_bCsBasicGuildInfoLoadOK = TRUE;
	m_bCsTotalGuildInfoLoadOK = TRUE;

	SetAllUserCsJoinSide();

	EnterCriticalSection(&m_critCsTotalGuildInfo);

	_CS_TOTAL_GUILD_DATA_MAP::iterator it2 = m_mapCsTotalGuildInfo.begin();

	for(int iSize = m_mapCsTotalGuildInfo.size();it2 != m_mapCsTotalGuildInfo.end(); it2++)
	{
		basic_string<char,char_traits<char> > strGuildName = it2->first;

		_CS_TOTAL_GUILD_DATA stTEMP = it2->second;

		LogAddTD("[CastleSiege] Siege Guild List Load - GNAME:%s, CSGUID:%d, INVOLVED:%d",
			&strGuildName[0], stTEMP.m_iCsGuildID, stTEMP.m_bGuildInvolved);
	}

	LeaveCriticalSection(&m_critCsTotalGuildInfo);
}

int CCastleSiege::GetCsJoinSide(char * lpszGuildName, BYTE * btCsJoinSide, BYTE * bCsGuildInvolved)
{
	*btCsJoinSide = 0;
	*bCsGuildInvolved = 0;

	if(lpszGuildName == NULL)
		return false;

	if(m_bDoRun == FALSE)
		return false;

	if(m_iCastleSiegeState < CASTLESIEGE_STATE_NOTIFY || m_iCastleSiegeState > CASTLESIEGE_STATE_STARTSIEGE)
		return false;

	if(m_bCsTotalGuildInfoLoadOK == FALSE)
		return false;

	int bRETVAL = FALSE;

	EnterCriticalSection(&m_critCsTotalGuildInfo);

	_CS_TOTAL_GUILD_DATA_MAP::iterator it = m_mapCsTotalGuildInfo.find(lpszGuildName);

	if(it != m_mapCsTotalGuildInfo.end())
	{
		*btCsJoinSide = it->second.m_iCsGuildID;
		*bCsGuildInvolved = !(!it->second.m_bGuildInvolved);
		bRETVAL = TRUE;
	}

	LeaveCriticalSection(&m_critCsTotalGuildInfo);

return bRETVAL;
}

int CCastleSiege::OperateGate(int iObjIndex, int iMonsterExistVal, int bOpenType)
{
	if(gObjIsConnected(iObjIndex) == FALSE)
		return false;

	int bRETVAL = FALSE;

	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == 277
			&& pNpcData.m_iNPC_OBJINDEX == iObjIndex
			&& pNpcData.m_bIN_USE == TRUE
			&& pNpcData.m_iNPC_LIVE == 2)
		{
			int iNPC_INDEX = pNpcData.m_iNPC_OBJINDEX;

			if(gObj[iNPC_INDEX].m_btCsNpcType != FALSE
				&& iMonsterExistVal == pNpcData.m_iCsNpcExistVal)
			{
				SetGateBlockState(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY,bOpenType);
				gObjAddBuffEffect(&gObj[iNPC_INDEX],BUFF_GATESTATUS,0,0,0,0,-10);
				gObj[iNPC_INDEX].m_btCsGateOpen = bOpenType;
				bRETVAL = TRUE;
			}

			break;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);


return bRETVAL;
}

int CCastleSiege::OperateGate(int iNpcIndex,int bOpenType)
{
	// FUNCTION NOT USED
	return false;
}

int CCastleSiege::CheckCastleOwnerMember(int iIndex)
{
	if(gObjIsConnected(iIndex) == FALSE)
		return false;

	if(strcmp(m_szCastleOwnerGuild,"") == NULL)
		return false;

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return false;

	if(strcmp(gObj[iIndex].GuildName,m_szCastleOwnerGuild) != NULL)
		return false;

return true;
}

int CCastleSiege::CheckCastleOwnerUnionMember(int iIndex)
{
	if(gObjIsConnected(iIndex) == FALSE)
		return false;

	if(strcmp(m_szCastleOwnerGuild,"") == NULL)
		return false;

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return false;

	_GUILD_INFO_STRUCT * lpGuildInfo = gObj[iIndex].lpGuild;

	if(lpGuildInfo == NULL)
		return false;

	TUnionInfo * pUnionInfo = UnionManager.SearchUnion(lpGuildInfo->iGuildUnion);

	if(pUnionInfo == NULL)
		return false;

	if(strcmp(pUnionInfo->m_szMasterGuild,m_szCastleOwnerGuild) == NULL)
		return true;

return false;
}

int CCastleSiege::CheckUnionGuildMaster(int iIndex)
{
	if(gObjIsConnected(iIndex) == FALSE)
		return false;

	_GUILD_INFO_STRUCT * lpGuildInfo = gObj[iIndex].lpGuild;

	if(lpGuildInfo == NULL)
		return false;

	if(gObj[iIndex].GuildStatus == 128)
	{
		if(lpGuildInfo->Number == lpGuildInfo->iGuildUnion || lpGuildInfo->iGuildUnion == NULL)
		{
			return true;
		}
	}
return false;
}

void CCastleSiege::SetCrownUserIndex(int iIndex)
{
	m_iCastleCrownAccessUser = iIndex;
}

void CCastleSiege::SetCrownSwitchUserIndex(int iMonsterClass, int iIndex)
{
	switch(iMonsterClass)
	{
	case 217:
		m_iCastleSwitchAccessUser1 = iIndex;
		break;
	case 218:
		m_iCastleSwitchAccessUser2 = iIndex;
		break;
	}
}

int CCastleSiege::CheckOverlapCsMarks(int iIndex)
{
	for(int x = INVETORY_WEAR_SIZE; x < MAIN_INVENTORY_SIZE; x++)
	{
		if(gObj[iIndex].pInventory[x].IsItem() == TRUE 
			&& gObj[iIndex].pInventory[x].m_Type == ITEMGET(14,21)
			&& gObj[iIndex].pInventory[x].m_Level == 3)
		{
			int iITEM_DUR = (int)gObj[iIndex].pInventory[x].m_Durability;

			if(ITEM_DUR_RANGE(iITEM_DUR) != FALSE)
				return x;
		}
	}
return -1;
}

int CCastleSiege::GetCsAttkGuildList(PMSG_CSATTKGUILDLIST * lpMsgBody, int * iCount)
{
	*iCount = 0;

	if(lpMsgBody == NULL)
		return false;

	if(m_iCastleSiegeState != CASTLESIEGE_STATE_NOTIFY && m_iCastleSiegeState != CASTLESIEGE_STATE_READYSIEGE)
		return 2;

	if(m_bCsBasicGuildInfoLoadOK == FALSE || m_bCsTotalGuildInfoLoadOK == FALSE)
		return 3;

	EnterCriticalSection(&m_critCsTotalGuildInfo);

	for(_CS_TOTAL_GUILD_DATA_MAP::iterator it = m_mapCsTotalGuildInfo.begin(); it != m_mapCsTotalGuildInfo.end(); it++)
	{
		lpMsgBody[*iCount].btCsJoinSide = it->second.m_iCsGuildID;
		lpMsgBody[*iCount].btGuildInvolved = it->second.m_bGuildInvolved;
		lpMsgBody[*iCount].iScore = it->second.m_iTotalScore;

		memset(lpMsgBody[*iCount].szGuildName,0x00,10);
		memcpy(lpMsgBody[*iCount].szGuildName,&it->first[0],8);

		*iCount += 1;
	}

	LeaveCriticalSection(&m_critCsTotalGuildInfo);
return true;
}

int CCastleSiege::CheckTeleportMagicAxisY(int iStartY, int iTargetX, int iTargetY)
{
	for(int iSTEP = 0; iSTEP < 3; iSTEP++)
	{
		if(g_iCastleWallStepAxisY[iSTEP] >= iStartY
			&& g_iCastleWallStepAxisY[iSTEP] <= iTargetY
			&& iTargetX < 144)
		{
			return false;
		}
	}
	return true;
}

void CCastleSiege::NotifyCsSelfLeftTime(int iIndex)
{
	GCAnsSelfCsLeftTimeAlarm(iIndex,(m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60);
}

void CCastleSiege::ReSpawnAllUser()
{
		int iTX;
	int iTY;
	unsigned char btMapAttr;

	for(int iUSER = OBJ_STARTUSERINDEX; iUSER < OBJMAX; iUSER++)
	{
		if(gObjIsConnected(iUSER) == FALSE)
			continue;

		if(gObj[iUSER].MapNumber != MAP_INDEX_CASTLESIEGE)
			continue;

		unsigned char btCsJoinSide = gObj[iUSER].m_btCsJoinSide;

		if(btCsJoinSide < 0)
		{
			btCsJoinSide = 0;
		}

		if(btCsJoinSide > 2)
		{
			btCsJoinSide = 2;
		}

		int bMoveOK = FALSE;

		switch(btCsJoinSide)
		{
		case 1:
			{
				for(int iCNT = 0; iCNT < 100; iCNT++)
				{
					iTX = (rand() % (g_iCsUserReSpawnArea[1][2] - g_iCsUserReSpawnArea[1][0])) + g_iCsUserReSpawnArea[1][0];
					iTY = (rand() % (g_iCsUserReSpawnArea[1][3] - g_iCsUserReSpawnArea[1][1])) + g_iCsUserReSpawnArea[1][1];
	
					btMapAttr = MapC[MAP_INDEX_CASTLESIEGE].m_attrbuf[iTY * 256 + iTX];
	
					if((btMapAttr & 4) != 4)
					{
						bMoveOK = TRUE;
						break;
					}
				}
			}
			break;
		case 2:
			{
				for(int iCNT = 0; iCNT < 100; iCNT++)
				{
					iTX = (rand() % (g_iCsUserReSpawnArea[2][2] - g_iCsUserReSpawnArea[2][0])) + g_iCsUserReSpawnArea[2][0];
					iTY = (rand() % (g_iCsUserReSpawnArea[2][3] - g_iCsUserReSpawnArea[2][1])) + g_iCsUserReSpawnArea[2][1];
	
					btMapAttr = MapC[MAP_INDEX_CASTLESIEGE].m_attrbuf[iTY * 256 + iTX];
	
					if((btMapAttr & 4) != 4)
					{
						bMoveOK = TRUE;
						break;
					}
				}
			}
			break;
		default:
			{
				for(int iCNT = 0; iCNT < 100; iCNT++)
				{
					iTX = (rand() % (g_iCsUserReSpawnArea[0][2] - g_iCsUserReSpawnArea[0][0])) + g_iCsUserReSpawnArea[0][0];
					iTY = (rand() % (g_iCsUserReSpawnArea[0][3] - g_iCsUserReSpawnArea[0][1])) + g_iCsUserReSpawnArea[0][1];
	
					btMapAttr = MapC[MAP_INDEX_CASTLESIEGE].m_attrbuf[iTY * 256 + iTX];
	
					if((btMapAttr & 4) != 4)
					{
						bMoveOK = TRUE;
						break;
					}
				}
			}
			break;
		}

		if(bMoveOK != FALSE)
		{
			gObj[iUSER].m_State = 32;
			gObj[iUSER].X = iTX;
			gObj[iUSER].Y = iTY;
			gObj[iUSER].TX = iTX;
			gObj[iUSER].TX = iTY;
			gObj[iUSER].MapNumber = MAP_INDEX_CASTLESIEGE;
			gObj[iUSER].PathCount = 0;
			gObj[iUSER].Teleport = 0;

			gObjClearViewport(&gObj[iUSER]);
			GCTeleportSend(&gObj[iUSER],0xFF,MAP_INDEX_CASTLESIEGE,gObj[iUSER].X,gObj[iUSER].Y,gObj[iUSER].Dir);

			if(gObj[iUSER].m_Change >= 0)
			{
				gObjViewportListProtocolCreate(&gObj[iUSER]);
			}

			gObj[iUSER].RegenMapNumber = MAP_INDEX_CASTLESIEGE;
			gObj[iUSER].RegenMapX = iTX;
			gObj[iUSER].RegenMapY = iTY;
			gObj[iUSER].RegenOk = TRUE;
		}
	}
}

void CCastleSiege::AddMiniMapDataReqUser(int iIndex)
{
	EnterCriticalSection(&m_critCsMiniMap);

	if(find(m_vtMiniMapReqUser.begin(),m_vtMiniMapReqUser.end(),iIndex) == m_vtMiniMapReqUser.end())
	{
		m_vtMiniMapReqUser.push_back(iIndex);
	}

	LeaveCriticalSection(&m_critCsMiniMap);
}

void CCastleSiege::DelMiniMapDataReqUser(int iIndex)
{
	EnterCriticalSection(&m_critCsMiniMap);

	vector<int>::iterator  it = find(m_vtMiniMapReqUser.begin(),m_vtMiniMapReqUser.end(),iIndex);

	if(it != m_vtMiniMapReqUser.end())	{
		m_vtMiniMapReqUser.erase(it);
	}

	LeaveCriticalSection(&m_critCsMiniMap);
}

void CCastleSiege::SendAllUserAnyData(BYTE * lpMsg, int iSize)
{
	for(int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++)
	{
		if(gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
		{
			DataSend(i,lpMsg,iSize);
		}
	}
}

void CCastleSiege::SendCsUserAnyData(BYTE * lpMsg, int iSize)
{
	for(int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++)
	{
		if(gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER && gObj[i].MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			DataSend(i,lpMsg,iSize);
		}
	}
}

void CCastleSiege::SendCsUserAnyMsg(char * lpszMsg)
{
	if(lpszMsg == NULL)
		return;

	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg(&pNotice,0,lpszMsg);

	for(int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++)
	{
		if(gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER && gObj[i].MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			DataSend(i,(PBYTE)&pNotice,pNotice.h.size);
		}
	}
}

void CCastleSiege::OperateGmCommand(int iIndex, int iGmCommand, void * lpParam)
{
	if(gObjIsConnected(iIndex) == FALSE)
		return;

	switch(iGmCommand)
	{
	case 0:
		{
			char * lpszCastleOwnerGuild = (char *)lpParam;

			if(lpszCastleOwnerGuild == NULL)
			{
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,128)));
				return;
			}

			if(strcmp(lpszCastleOwnerGuild,"") != NULL)
			{
				memset(m_szCastleOwnerGuild,0x00,sizeof(m_szCastleOwnerGuild));
				memcpy(m_szCastleOwnerGuild,lpszCastleOwnerGuild,8);
				m_btIsCastleOccupied = TRUE;

				MsgOutput(iIndex,lMsg.Get(MSGGET(12,129)),m_szCastleOwnerGuild);
			}
			else
			{
				strcpy(m_szCastleOwnerGuild,"");
				m_btIsCastleOccupied = FALSE;
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,128)));
			}
		}
		break;
	case 1:
		{
			GetLocalTime(&m_tmStartDate);
			m_tmStartDate.wHour = 0;
			m_tmStartDate.wMinute = 0;
			m_tmStartDate.wSecond = 0;

			m_tmEndDate = m_tmStartDate;

			GetNextDay(&m_tmEndDate,m_iCastleSiegeCycle,0,0,0);

			m_bFixCastleCycleStartDate = FALSE;
			m_btIsSiegeEnded = FALSE;

			//CreateDbNPC_INS();
			GS_GDReqResetRegSiegeInfo(m_iMapSvrGroup);

			if(CheckSync() == FALSE)
			{
				LogAddC(2,"[CastleSiege] CCastleSiege::Init() - CheckSync() == FALSE");
				//MsgBox("[CastleSiege] CCastleSiege::Init() - CheckSync() == FALSE");
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,130)));
				return;
			}

			m_bDoRun = TRUE;
			m_dwNPC_DBSAVE_TICK_COUNT = GetTickCount();
			m_dwCALC_LEFTSIEGE_TICK_COUNT = NULL;
			m_dwEVENT_MSG_TICK_COUNT = NULL;
			m_dwCHECK_GATE_ALIVE_COUNT = NULL;

			MsgOutput(iIndex,lMsg.Get(MSGGET(12,131)));
		}
		break;
	case 2:
		{
			SetState(CASTLESIEGE_STATE_REGMARK,TRUE);
			MsgOutput(iIndex,lMsg.Get(MSGGET(12,132)));
		}
		break;
	case 3:
		{
			m_btIsSiegeGuildList = FALSE;
			SetState(CASTLESIEGE_STATE_NOTIFY,TRUE);
			MsgOutput(iIndex,lMsg.Get(MSGGET(12,133)));
		}
		break;
	case 4:
		{
			SetState(CASTLESIEGE_STATE_STARTSIEGE,TRUE);
			MsgOutput(iIndex,lMsg.Get(MSGGET(12,134)));
		}
		break;
	case 5:
		{
			if(m_iCastleSiegeState == CASTLESIEGE_STATE_STARTSIEGE)
			{
				m_iCS_REMAIN_MSEC = NULL;
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,135)));
			}
			else
			{
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,136)));
			}
		}
		break;
	case 6:
		{
			if(strcmp(m_szCastleOwnerGuild,"") == NULL)
			{
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,137)));
			}
			else
			{
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,138)),m_szCastleOwnerGuild);
			}
		}
		break;
	case 7:
		{
			if(m_bDoRun == FALSE)
			{
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,139)));
				return;
			}

			switch(m_iCastleSiegeState)
			{
			case CASTLESIEGE_STATE_NONE:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,139)));
				}
				break;
			case CASTLESIEGE_STATE_IDLE_1:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,140)));
				}
				break;
			case CASTLESIEGE_STATE_REGSIEGE:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,141)),
						(m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60,((m_iCS_REMAIN_MSEC / 1000) % 3600) % 60);
				}
				break;
			case CASTLESIEGE_STATE_IDLE_2:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,142)),
						(m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60,((m_iCS_REMAIN_MSEC / 1000) % 3600) % 60);
				}
				break;
			case CASTLESIEGE_STATE_REGMARK:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,143)),
						(m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60,((m_iCS_REMAIN_MSEC / 1000) % 3600) % 60);
				}
				break;
			case CASTLESIEGE_STATE_IDLE_3:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,144)),
						(m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60,((m_iCS_REMAIN_MSEC / 1000) % 3600) % 60);
				}
				break;
			case CASTLESIEGE_STATE_NOTIFY:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,145)),
						(m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60,((m_iCS_REMAIN_MSEC / 1000) % 3600) % 60);
				}
				break;
			case CASTLESIEGE_STATE_READYSIEGE:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,146)),
								(m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60,((m_iCS_REMAIN_MSEC / 1000) % 3600) % 60);
				}
				break;
			case CASTLESIEGE_STATE_STARTSIEGE:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,147)),
								(m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60,((m_iCS_REMAIN_MSEC / 1000) % 3600) % 60);
				}
				break;
			case CASTLESIEGE_STATE_ENDSIEGE:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,148)),
								(m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60,((m_iCS_REMAIN_MSEC / 1000) % 3600) % 60);
				}
				break;
			case CASTLESIEGE_STATE_ENDCYCLE:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,149)));
				}
				break;
			default:
				{
					MsgOutput(iIndex,lMsg.Get(MSGGET(12,139)));
				}
				break;
			}
		}
		break;
	case 8:
		{
			char * lpszAttackGuild = (char *)lpParam;

			if(m_iCastleSiegeState != CASTLESIEGE_STATE_STARTSIEGE)
			{
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,150)));
				return;
			}

			if(lpszAttackGuild == NULL)
			{
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,150)));
				return;
			}

			if(strcmp(lpszAttackGuild,"") == NULL)
			{
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,150)));
				return;
			}

			BYTE btCsJoinSide = NULL;
			BYTE bGuildInvolved = NULL;

			int bRETVAL = GetCsJoinSide(lpszAttackGuild,&btCsJoinSide,&bGuildInvolved);

			if(bRETVAL == TRUE)
			{
				if(btCsJoinSide > 1 && bGuildInvolved == TRUE)
				{
					LogAddTD("[CastleSiege] Castle Winner Has been Changed - GM (GUILD:%s)", lpszAttackGuild);

					NotifyCrownState(2);

					memset(m_szMiddleWinnerGuild,0x00,sizeof(m_szMiddleWinnerGuild));
					memcpy(m_szMiddleWinnerGuild,lpszAttackGuild,8);

					ClearNonDbNPC_MidWin();
					ChangeWinnerGuild(btCsJoinSide);
					NotifyAllUserCsProgState(1,lpszAttackGuild);
					ResetCrownAccessTickCount();
					ResetCrownUserIndex();
					SetCrownAccessUserX(0);
					SetCrownAccessUserY(0);
					ResetCrownSwitchUserIndex(217);
					ResetCrownSwitchUserIndex(218);

					m_bRegCrownAvailable = FALSE;
					ReSpawnEnemyUser(FALSE);

					GCAnsCsLeftTimeAlarm((m_iCS_REMAIN_MSEC / 1000) / 3600,((m_iCS_REMAIN_MSEC / 1000) % 3600) / 60);
					m_dwCS_LEFTTIME_TICK_COUNT =  GetTickCount();
				}
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,151)));
			}
			else
			{
				MsgOutput(iIndex,lMsg.Get(MSGGET(12,150)));
			}
		}
		break;
	}
}

void CCastleSiege::CreateDbNPC_INS()
{
	ClearDbNPC();

	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while( it != m_vtNpcData.end() )
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_bNPC_DBSAVE == TRUE)
		{
			it->SetBaseValue();
			it->m_iNPC_LIVE = TRUE;
			it->m_iNPC_SIDE = 1;
			it->m_iNPC_OBJINDEX = -1;
			it->m_iCS_GATE_LEVER_INDEX = -1;
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);

	m_bDbNpcCreated = FALSE;
}

DWORD m_dwSIEGE_LOG_TICK = NULL;

void CCastleSiege::Run()
{
	if(m_bDoRun == FALSE)
		return;

	if(m_bCastleSiegeEnable == FALSE)
		return;

	switch(m_iCastleSiegeState)
	{
	case CASTLESIEGE_STATE_NONE:
		ProcState_NONE();
		break;
	case CASTLESIEGE_STATE_IDLE_1:
		ProcState_IDLE_1();
		break;
	case CASTLESIEGE_STATE_REGSIEGE:
		ProcState_REGSIEGE();
		break;
	case CASTLESIEGE_STATE_IDLE_2:
		ProcState_IDLE_2();
		break;
	case CASTLESIEGE_STATE_REGMARK:
		ProcState_REGMARK();
		break;
	case CASTLESIEGE_STATE_IDLE_3:
		ProcState_IDLE_3();
		break;
	case CASTLESIEGE_STATE_NOTIFY:
		ProcState_NOTIFY();
		break;
	case CASTLESIEGE_STATE_READYSIEGE:
		ProcState_READYSIEGE();
		break;
	case CASTLESIEGE_STATE_STARTSIEGE:
		ProcState_STARTSIEGE();
		break;
	case CASTLESIEGE_STATE_ENDSIEGE:
		ProcState_ENDSIEGE();
		break;
	case CASTLESIEGE_STATE_ENDCYCLE:
		ProcState_ENDCYCLE();
		break;
	}

	if(GetTickCount() - m_dwNPC_DBSAVE_TICK_COUNT > 300000)
	{
		if(m_iCastleSiegeState != CASTLESIEGE_STATE_STARTSIEGE)
		{
			StoreDbNpc();
		}

		m_dwNPC_DBSAVE_TICK_COUNT = GetTickCount();
	}

	if(GetTickCount() - m_dwCALC_LEFTSIEGE_TICK_COUNT > 10000)
	{
		m_dwCALC_LEFTSIEGE_TICK_COUNT = GetTickCount();
		CalcCastleLeftSiegeDate();
	}

	if(GetTickCount() - m_dwSIEGE_LOG_TICK > 60000)
	{
		m_dwSIEGE_LOG_TICK = GetTickCount();

		int iRET_SEC = CalcCastleLeftSiegeDate();

		if(iRET_SEC >= 0)
		{
			_SYSTEMTIME tmRetDate;
			
			tmRetDate = GetCastleLeftSiegeDate();

#if (__CUSTOM__ ==1)
			switch(m_iCastleSiegeState)
			{
			case CASTLESIEGE_STATE_IDLE_1:
				LogAddC(3,"RUN ProcState_IDLE_1()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			case CASTLESIEGE_STATE_REGSIEGE:
				LogAddC(3,"RUN ProcState_REGSIEGE()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			case CASTLESIEGE_STATE_IDLE_2:
				LogAddC(3,"RUN ProcState_IDLE_2()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			case CASTLESIEGE_STATE_REGMARK:
				LogAddC(3,"RUN ProcState_REGMARK()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			case CASTLESIEGE_STATE_IDLE_3:
				LogAddC(3,"RUN ProcState_IDLE_3()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			case CASTLESIEGE_STATE_NOTIFY:
				LogAddC(3,"RUN ProcState_NOTIFY()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			case CASTLESIEGE_STATE_READYSIEGE:
				LogAddC(3,"RUN ProcState_READYSIEGE()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			case CASTLESIEGE_STATE_STARTSIEGE:
				LogAddC(3,"RUN ProcState_STARTSIEGE()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			case CASTLESIEGE_STATE_ENDSIEGE:
				LogAddC(3,"RUN ProcState_ENDSIEGE()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			case CASTLESIEGE_STATE_ENDCYCLE:
				LogAddC(3,"RUN ProcState_ENDCYCLE()	LEFT-MSEC:%d", m_iCS_REMAIN_MSEC);
				break;
			}
#endif

			LogAddC(4,"LEFT : SEC(%d), SIEGE-DAY(%04d-%02d-%02d %02d:%02d:%02d)",
				iRET_SEC, tmRetDate.wYear, tmRetDate.wMonth, tmRetDate.wDay, 
				tmRetDate.wHour, tmRetDate.wMinute, tmRetDate.wSecond);
		}
		else
		{
			switch(iRET_SEC)
			{
			case -1:
				LogAddC(4,"[CastleSiege] LEFT-TIME : CASTLESIEGE_LEFTTIME_ONSIGE");
				break;
			case -2:
				LogAddC(4,"[CastleSiege] LEFT-TIME : CASTLESIEGE_LEFTTIME_ENDSIEGE");
				break;
			case -3:
				LogAddC(4,"[CastleSiege] LEFT-TIME : CASTLESIEGE_LEFTTIME_NOTRUNNING");
				break;
			}
		}
	}
}

struct CSP_REQ_NPCSAVEDATA
{
	PWMSG_HEAD h;
	unsigned short wMapSvrNum;
	int iCount;
};

struct CSP_NPCSAVEDATA
{
	int iNpcNumber;
	int iNpcIndex;
	int iNpcDfLevel;
	int iNpcRgLevel;
	int iNpcMaxHp;
	int iNpcHp;
	unsigned char btNpcX;
	unsigned char btNpcY;
	unsigned char btNpcDIR;
};

int CCastleSiege::FirstCreateDbNPC()
{
	if(m_iCastleDataLoadState != 4)
	{
		LogAddC(2,"[CastleSiege] CCastleSiege::FirstCreateDbNPC() - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_4");
		return false;
	}

	char cBUFFER[0x1090];

	CSP_REQ_NPCSAVEDATA * lpMsg = (CSP_REQ_NPCSAVEDATA *)cBUFFER;
	CSP_NPCSAVEDATA * lpMsgBody = (CSP_NPCSAVEDATA *)&cBUFFER[sizeof(CSP_REQ_NPCSAVEDATA)];

	int iCOUNT = NULL;

	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	int bExist = FALSE;

	while( it != m_vtNpcData.end() )
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_bNPC_DBSAVE == TRUE)
		{
			it->SetBaseValue();
			it->m_iNPC_LIVE = TRUE;
			it->m_iNPC_SIDE = 1;

			lpMsgBody[iCOUNT].iNpcNumber = pNpcData.m_iNPC_NUM;
			lpMsgBody[iCOUNT].iNpcIndex = pNpcData.m_iNPC_INDEX;
			lpMsgBody[iCOUNT].iNpcDfLevel = pNpcData.m_iNPC_BASE_DF_LEVEL;
			lpMsgBody[iCOUNT].iNpcRgLevel = pNpcData.m_iNPC_BASE_RG_LEVEL;
			lpMsgBody[iCOUNT].iNpcMaxHp = pNpcData.m_iNPC_BASE_MAXHP;
			lpMsgBody[iCOUNT].iNpcHp = pNpcData.m_iNPC_BASE_HP;
			lpMsgBody[iCOUNT].btNpcX = pNpcData.m_iNPC_BASE_SX;
			lpMsgBody[iCOUNT].btNpcY = pNpcData.m_iNPC_BASE_SY;
			lpMsgBody[iCOUNT].btNpcDIR = pNpcData.m_iNPC_BASE_DIR;
			iCOUNT++;
		}
		it++;
	}

	LeaveCriticalSection(&m_critNpcData);

	lpMsg->wMapSvrNum = m_iMapSvrGroup;
	lpMsg->iCount = iCOUNT;

	lpMsg->h.set((PBYTE)lpMsg,0x84,sizeof(CSP_REQ_NPCSAVEDATA) + sizeof(CSP_NPCSAVEDATA) * iCOUNT);
	cDBSMng.Send((char *)lpMsg,sizeof(CSP_REQ_NPCSAVEDATA) + sizeof(CSP_NPCSAVEDATA) * iCOUNT);

	return true;
}

void CCastleSiege::GetStateDateTime(int iState,_SYSTEMTIME * tm)
{
	EnterCriticalSection(&m_critScheduleData);

	vector<_CS_SCHEDULE_DATA>::iterator it;
	for(it = m_vtScheduleData.begin();it != m_vtScheduleData.end();it++)
	{
		_CS_SCHEDULE_DATA & pScheduleData = *it;

		if(pScheduleData.m_iSTATE == iState)
		{
			tm->wDay = pScheduleData.m_iADD_DAY;
			tm->wHour = pScheduleData.m_iADD_HOUR;
			tm->wMinute = pScheduleData.m_iADD_MIN;
			tm->wSecond = NULL;
			break;
		}
	}

	LeaveCriticalSection(&m_critScheduleData);
}

int CCastleSiege::AddDbNPC(int iNpcType, int iNpcIndex)
{
	int iRETVAL = FALSE;

	EnterCriticalSection(&m_critNpcData);

	vector<_CS_NPC_DATA>::iterator it = m_vtNpcData.begin();

	while(it != m_vtNpcData.end())
	{
		_CS_NPC_DATA & pNpcData = _CS_NPC_DATA(*it);

		if(pNpcData.m_iNPC_NUM == iNpcType
			&& pNpcData.m_iNPC_INDEX == iNpcIndex)
		{
			if(gObjIsConnected(pNpcData.m_iNPC_OBJINDEX)
				&& gObj[pNpcData.m_iNPC_OBJINDEX].Class == iNpcType
				&& gObj[pNpcData.m_iNPC_OBJINDEX].Live == TRUE
				&& gObj[pNpcData.m_iNPC_OBJINDEX].m_iCsNpcExistVal == pNpcData.m_iCsNpcExistVal
				&& gObj[pNpcData.m_iNPC_OBJINDEX].m_iCsNpcExistVal != FALSE)
			{
				LogAddTD("[CastleSiege] CCastleSiege::AddDbNPC() ERROR - DB NPC EXIST (CLS:%d, IDX:%d(%d), MAXHP:%d, HP:%d, DF:%d, RG:%d)",
					pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX, pNpcData.m_iNPC_OBJINDEX, gObj[pNpcData.m_iNPC_OBJINDEX].MaxLife, 
					gObj[pNpcData.m_iNPC_OBJINDEX].Life, gObj[pNpcData.m_iNPC_OBJINDEX].m_btCsNpcDfLevel, gObj[pNpcData.m_iNPC_OBJINDEX].m_btCsNpcRgLevel);
				break;
			}

			int iNPC_INDEX = gObjAddMonster(MAP_INDEX_CASTLESIEGE);

			if(iNPC_INDEX >= 0)
			{
				gObjSetMonster(iNPC_INDEX,pNpcData.m_iNPC_NUM);
				gObj[iNPC_INDEX].m_PosNum = -1;
				gObj[iNPC_INDEX].X = pNpcData.m_iNPC_SX;
				gObj[iNPC_INDEX].Y = pNpcData.m_iNPC_SY;
				gObj[iNPC_INDEX].MapNumber = MAP_INDEX_CASTLESIEGE;
				gObj[iNPC_INDEX].TX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].TY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].m_OldX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].m_OldY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].Dir = pNpcData.m_iNPC_DIR;
				gObj[iNPC_INDEX].StartX = gObj[iNPC_INDEX].X;
				gObj[iNPC_INDEX].StartY = gObj[iNPC_INDEX].Y;
				gObj[iNPC_INDEX].DieRegen = 0;
				gObj[iNPC_INDEX].MaxRegenTime = 0;
				gObj[iNPC_INDEX].Life = (float)pNpcData.m_iNPC_HP;
				gObj[iNPC_INDEX].MaxLife = (float)pNpcData.m_iNPC_MAXHP;
				gObj[iNPC_INDEX].m_btCsNpcType = 1;

				int iNPC_DF_LEVEL = pNpcData.m_iNPC_BASE_DF_LEVEL;

				if(iNPC_DF_LEVEL < 0)
				{
					iNPC_DF_LEVEL = 0;
				}

				if(iNPC_DF_LEVEL > CS_MAX_UPGRADE)
				{
					iNPC_DF_LEVEL = CS_MAX_UPGRADE;
				}

				int iNPC_RG_LEVEL = pNpcData.m_iNPC_BASE_RG_LEVEL;

				if(iNPC_RG_LEVEL < 0)
				{
					iNPC_RG_LEVEL = 0;
				}

				if(iNPC_RG_LEVEL > CS_MAX_UPGRADE)
				{
					iNPC_RG_LEVEL = CS_MAX_UPGRADE;
				}

				switch(pNpcData.m_iNPC_NUM)
				{
				case 277:
					gObj[iNPC_INDEX].m_Defense = g_iNpcDefense_CGATE[iNPC_DF_LEVEL];
					gObj[iNPC_INDEX].m_btCsNpcDfLevel = iNPC_DF_LEVEL;
					break;
				case 283:
					gObj[iNPC_INDEX].m_Defense = g_iNpcDefense_CSTATUE[iNPC_DF_LEVEL];
					gObj[iNPC_INDEX].m_btCsNpcDfLevel = iNPC_DF_LEVEL;
					gObj[iNPC_INDEX].m_btCsNpcRgLevel = iNPC_RG_LEVEL;
					break;
				}

				it->m_btCsNpcExistVal1 = rand()%255+1;
				gObj[iNPC_INDEX].m_btCsNpcExistVal1 = it->m_btCsNpcExistVal1;

				it->m_btCsNpcExistVal2 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal2 = it->m_btCsNpcExistVal2;

				it->m_btCsNpcExistVal3 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal3 = it->m_btCsNpcExistVal3;

				it->m_btCsNpcExistVal4 = rand()%256;
				gObj[iNPC_INDEX].m_btCsNpcExistVal4 = it->m_btCsNpcExistVal4;

				if(pNpcData.m_iNPC_NUM == 277)
				{
					SetGateBlockState(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY,TRUE);
					gObjAddBuffEffect(&gObj[iNPC_INDEX],BUFF_GATESTATUS,0,0,0,0,-10);

					gObj[iNPC_INDEX].m_btCsGateOpen = TRUE;

					int bCreateLever = FALSE;

					if(pNpcData.m_iCS_GATE_LEVER_INDEX == -1)
					{
						bCreateLever = TRUE;
					}
					else if(CheckLeverAlive(pNpcData.m_iCS_GATE_LEVER_INDEX) == FALSE)
					{
						bCreateLever = TRUE;
					}

					if(bCreateLever != FALSE)
					{
						int iLeverIndex = CreateCsGateLever(pNpcData.m_iNPC_SX,pNpcData.m_iNPC_SY+4);

						if(OBJMAX_RANGE(iLeverIndex))
						{
							it->m_iCS_GATE_LEVER_INDEX = iLeverIndex;

							int bRETVAL = LinkCsGateLever(iLeverIndex,iNPC_INDEX);

							if(bRETVAL == FALSE)
							{
								LogAddC(2, "[CastleSiege] CCastleSiege::CreateDbNPC() ERROR - Failed to Create Gate Lever : GateIndex:%d", 
									pNpcData.m_iNPC_INDEX);
							}
						}
						else
						{
							it->m_iCS_GATE_LEVER_INDEX = -1;
							LogAddC(2, "[CastleSiege] CCastleSiege::CreateDbNPC() ERROR - Failed to Create Gate Lever : GateIndex:%d", 
								pNpcData.m_iNPC_INDEX);
						}
					}
					else
					{
						int bRETVAL = LinkCsGateLever(pNpcData.m_iCS_GATE_LEVER_INDEX,iNPC_INDEX);
						if(bRETVAL == FALSE)
						{
							LogAddC(2,"[CastleSiege] CCastleSiege::CreateDbNPC() ERROR - Failed to Create Gate Lever : GateIndex:%d", 
								pNpcData.m_iNPC_INDEX);
						}
					}
				}

				it->m_iNPC_OBJINDEX = iNPC_INDEX;
				it->m_iNPC_LIVE = 2;
				it->m_bIN_USE = TRUE;
				iRETVAL = TRUE;

				if(pNpcData.m_iNPC_NUM == 277 || pNpcData.m_iNPC_NUM == 283)
				{
					LogAddTD("[CastleSiege][CreateDBNPC] (Index:%d, Num:%d, DF:%d, RG:%d, MaxHP:%d, HP:%d )",
						pNpcData.m_iNPC_INDEX, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_DF_LEVEL, pNpcData.m_iNPC_RG_LEVEL,
						pNpcData.m_iNPC_MAXHP, pNpcData.m_iNPC_HP);
				}

				break;
			}
		}

		it++;
	}

	LeaveCriticalSection(&m_critNpcData);

	return iRETVAL;
}

#endif