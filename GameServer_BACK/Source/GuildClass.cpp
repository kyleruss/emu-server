#include "stdafx.h"
#include "BuffEffect.h"
#include "GuildClass.h"
#include "LogProc.h"
#include "GameMain.h"
#include "protocol.h"

CGuildClass::CGuildClass()
{
	this->head = NULL;
	this->tail = NULL;
}

CGuildClass::~CGuildClass()
{
	this->AllDelete();
}

void CGuildClass::Init()
{
	this->m_GuildMap.clear();
	this->m_GuildNumberMap.clear();
}

_GUILD_INFO_STRUCT * CGuildClass::AddGuild(int number, char* guildname, GUILDMARK mark, char * mastername, int score)
{
	_GUILD_INFO_STRUCT * pNewNode;
	_GUILD_INFO_STRUCT * pSearchGuild = this->SearchGuild(guildname);

	if ( pSearchGuild != NULL )
	{
		return pSearchGuild;
	}

	pNewNode = (_GUILD_INFO_STRUCT*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(_GUILD_INFO_STRUCT));

	if ( pNewNode != NULL )
	{
		memset(pNewNode->Name, 0, sizeof(pNewNode->Name));
		strcpy(pNewNode->Name, guildname);
		strcpy(pNewNode->Names[0], mastername);

		if ( mark != NULL )
		{
			memcpy(pNewNode->Mark, mark, sizeof(GUILDMARK));
		}

		pNewNode->Number = number;
		pNewNode->Count = 0;
		pNewNode->TotalCount = 0;
		pNewNode->PlayScore = 0;
		pNewNode->TotalScore = score;
		pNewNode->next = NULL;
		pNewNode->WarDeclareState = 0;
		pNewNode->WarState = 0;
		memset(pNewNode->Notice, 0, sizeof(pNewNode->Notice));
		pNewNode->iGuildUnion = 0;
		pNewNode->iGuildRival = 0;

		for ( int i=0;i<MAX_USER_GUILD;i++)
		{
			pNewNode->Use[i] = FALSE;
			pNewNode->Index[i] = -1;
			pNewNode->pServer[i] = -1;
			pNewNode->GuildStatus[i] = -1;
		}

		this->BuildMemberTotal(pNewNode);
		this->AddTail(pNewNode);
		return pNewNode;
	}
	return NULL;
}

void CGuildClass::AddTail(_GUILD_INFO_STRUCT * pNewNode)
{
	pNewNode->back = this->tail;
	pNewNode->next = NULL;

	if ( this->head == NULL )
	{
		this->head = pNewNode;
	}
	else
	{
		this->tail->next = pNewNode;
	}

	this->tail = pNewNode;
	this->m_GuildMap[pNewNode->Name] = pNewNode;
	this->m_GuildNumberMap[pNewNode->Number] = pNewNode;
}




void CGuildClass::Print()
{
	_GUILD_INFO_STRUCT * pNode = this->head;

	while ( pNode != NULL )
	{
		pNode = pNode->next;
	}
}


void CGuildClass::PrintGuild(char* guildname)
{
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(guildname);
}



void CGuildClass::AllDelete()
{
	_GUILD_INFO_STRUCT * pNode = this->head;
	_GUILD_INFO_STRUCT * pTemp;

	while ( pNode != NULL )
	{
		pTemp = pNode;
		pNode = pNode->next;
		HeapFree(GetProcessHeap(), 0, pTemp);
	}

	this->head = NULL;
	this->tail = NULL;
	this->m_GuildMap.clear();
	this->m_GuildNumberMap.clear();
}



BOOL CGuildClass::DeleteGuild(char* guildname, char* commander)
{
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(guildname);
	_GUILD_INFO_STRUCT * pprev;
	_GUILD_INFO_STRUCT * pnext;


	if ( pNode == NULL )
	{
		return false;
	}

	std::map<std::string, _GUILD_INFO_STRUCT *>::iterator Itor = this->m_GuildMap.find(guildname);

	if( Itor != this->m_GuildMap.end() )
	{
		this->m_GuildMap.erase(Itor);
	}

	std::map<int , _GUILD_INFO_STRUCT *>::iterator _Itor = this->m_GuildNumberMap.find(pNode->Number);

	if ( _Itor != this->m_GuildNumberMap.end() )
	{
		this->m_GuildNumberMap.erase(_Itor);
	}

	pprev = pNode->back;
	pnext = pNode->next;

	if ( pprev == NULL && pnext == NULL )
	{

	}
	else if ( pprev == NULL && pnext != NULL )
	{
		pnext->back = NULL;
		this->head = pnext;
	}
	else if ( pprev != NULL && pnext == NULL )
	{
		pprev->next = NULL;
		this->tail = pprev;
	}
	else if ( pprev != NULL && pnext != NULL )
	{
		pprev->next = pnext;
		pnext->back = pprev;
	}

	HeapFree(GetProcessHeap(), 0, pNode);

	if ( pprev == NULL && pnext == NULL )
	{
		this->head=NULL;
		this->tail = NULL;
	}

	return true;
}



	

_GUILD_INFO_STRUCT * CGuildClass::SearchGuild(char* guildname)
{
	std::map<std::string, _GUILD_INFO_STRUCT *>::iterator Itor = this->m_GuildMap.find(guildname);

	if( Itor == this->m_GuildMap.end() )
	{
		return NULL;
	}

	return (*(Itor)).second;
}




_GUILD_INFO_STRUCT * CGuildClass::SearchGuild_Number(int number)
{
	std::map<int , _GUILD_INFO_STRUCT *>::iterator Itor = this->m_GuildNumberMap.find(number);

	if ( Itor == this->m_GuildNumberMap.end() )
	{
		return NULL;
	}

	return (*(Itor)).second;
}



_GUILD_INFO_STRUCT * CGuildClass::SearchGuild_NumberAndId(int number, char* name)
{
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild_Number(number);

	if ( pNode != NULL )
	{
		for ( int i=0;i<MAX_USER_GUILD;i++)
		{
			if ( strcmp(pNode->Names[i], name) == 0 )
			{
				return pNode;
			}
		}
	}

	return NULL;
}


BOOL CGuildClass::ConnectUser(_GUILD_INFO_STRUCT * lpNode, char* guild_name, char* player_name, int aIndex, int pServer)
{
	if ( lpNode == NULL )
	{
		return false;
	}

	if ( strcmp(guild_name, lpNode->Name) != 0 )
	{
		return false;
	}

	for ( int n = 0; n<MAX_USER_GUILD;n++)
	{
		if ( lpNode->Use[n] == TRUE )
		{
			if ( strcmp(lpNode->Names[n], player_name) == 0 )
			{
				lpNode->Index[n] = aIndex;
				lpNode->pServer[n] = pServer;

				if ( n == 0 )	// Case if is GuildMaster
				{
					lpNode->PlayScore = 0;
					lpNode->WarDeclareState = 0;
					lpNode->WarState = 0;
				}

				this->BuildMemberTotal(lpNode);				
				return TRUE;
			}
		}
	}
	
	this->AddMember(lpNode, player_name, aIndex, pServer, -1);
	return false;
}



_GUILD_INFO_STRUCT * CGuildClass::AddMember(char* guild_name, char* player_name, int aIndex, int totalcount, int iGuildStatus, int pServer)
{
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(guild_name);
	int blank = -1;

	if ( pNode == NULL )
	{
		return NULL;
	}

	this->BuildMemberTotal(pNode);

	if ( strcmp(player_name, pNode->Names[0] ) == 0 )
	{
		blank = 0;
	}
	else
	{
		for ( int i = 1;i<MAX_USER_GUILD ; i++)
		{
			if ( pNode->Use[i] == TRUE )
			{
				if ( strcmp(pNode->Names[i], player_name) == 0 )
				{
					if ( aIndex != -1 )
					{
						pNode->Use[i] = TRUE;
						pNode->Index[i] = aIndex;
						pNode->pServer[i] = pServer;
						pNode->GuildStatus[i] = G_MASTER;
					}

					this->BuildMemberTotal(pNode);
					return FALSE;
				}
			}
			else if ( blank < 0 )
			{
				blank = i;
			}
		}
	}

	if (blank < 0 )
	{
		return NULL;
	}

	strcpy(pNode->Names[blank], player_name);
	pNode->Use[blank] = TRUE;
	pNode->pServer[blank] = pServer;
	pNode->GuildStatus[blank] = iGuildStatus;

	if ( aIndex != -1 )
	{
		pNode->Index[blank] = aIndex;
		pNode->Count++;
	}

	if ( totalcount > 0 )
	{
		pNode->TotalCount = totalcount;
	}

	this->BuildMemberTotal(pNode);

	return pNode;
}




_GUILD_INFO_STRUCT * CGuildClass::AddMember(_GUILD_INFO_STRUCT * pNode, char* player_name, int aIndex, int totalcount, int pServer)
{
	int blank = -1;

	if ( pNode == NULL )
	{
		return NULL;
	}

	if ( strcmp(player_name, pNode->Names[0] ) == 0 )
	{
		blank = 0;
	}
	else
	{
		for ( int i = 1;i<MAX_USER_GUILD ; i++)
		{
			if ( pNode->Use[i] == TRUE )
			{
				if ( strcmp(pNode->Names[i], player_name) == 0 )
				{
					if ( aIndex != -1 )
					{
						pNode->Use[i] = TRUE;
						pNode->Index[i] = aIndex;
						pNode->pServer[i] = pServer;
					}

					this->BuildMemberTotal(pNode);
					return FALSE;
				}
			}
			else if ( blank < 0 )
			{
				blank = i;
			}
		}
	}

	if (blank < 0 )
	{
		return NULL;
	}

	strcpy(pNode->Names[blank], player_name);
	pNode->Use[blank] = TRUE;
	pNode->pServer[blank] = pServer;

	if ( aIndex != -1 )
	{
		pNode->Index[blank] = aIndex;
		pNode->Count++;
	}

	if ( totalcount > 0 )
	{
		pNode->TotalCount = totalcount;
	}

	this->BuildMemberTotal(pNode);

	return pNode;
}


int CGuildClass::GetGuildMemberStatus(char* szGuildName, char* szMemberName)
{
	int iGuildStatus = 0;
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(szGuildName);
	int iKey = -1;

	if ( pNode == NULL )
	{
		return -1;
	}

	for ( int n=0;n<MAX_USER_GUILD;n++)
	{
		if ( pNode->Use[n] == TRUE )
		{
			if ( strcmp(pNode->Names[n], szMemberName) == 0 )
			{
				iKey = n;
			}
		}
	}

	if ( iKey < 0 )
	{
		return -1;
	}

	iGuildStatus = pNode->GuildStatus[iKey];
	return iGuildStatus;
}



BOOL CGuildClass::SetGuildMemberStatus(char* szGuildName, char* szMemberName, int iGuildStatus)
{
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(szGuildName);
	int iKey = -1;

	if ( pNode == NULL )
	{
		return -1;
	}

	for ( int n=0;n<MAX_USER_GUILD;n++)
	{
		if ( pNode->Use[n] == TRUE )
		{
			if ( strcmp(pNode->Names[n], szMemberName) == 0 )
			{
				iKey = n;
			}
		}
	}

	if ( iKey < 0 )
	{
		return 0;
	}
	
	pNode->GuildStatus[iKey] = iGuildStatus;

	if ( pNode->Index[iKey] > 0 )
	{
		if ( gObjIsConnectedGP(pNode->Index[iKey]) == FALSE )
		{
			LogAddTD("@SetGuildMemberStatus() error-L2 : Index %s %d", __FILE__, __LINE__);
			return false;
		}

		if ( !OBJMAX_RANGE(pNode->Index[iKey]) )
		{
			return 0;
		}
		
		LPOBJ lpObj = &gObj[pNode->Index[iKey]];

		if ( strcmp(lpObj->Name, szMemberName) == 0 )
		{
			lpObj->GuildStatus = iGuildStatus;
			GCGuildViewportNowPaint(lpObj->m_Index, lpObj->GuildName, 0, 0);
		}

	}

	return true;
}



int CGuildClass::GetGuildType(char* szGuildName)
{
	int iGuildType = 0;
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(szGuildName);

	if (pNode == NULL )
	{
		return -1;
	}

	iGuildType = pNode->btGuildType;
	return iGuildType;
}


BOOL CGuildClass::SetGuildType(char* szGuildName, int iGuildType)
{
	
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(szGuildName);

	if (pNode == NULL )
	{
		return FALSE;
	}

	pNode->btGuildType = iGuildType;

	for ( int n=0;n<MAX_USER_GUILD;n++)
	{
		if ( pNode->Use[n] == TRUE )
		{
			if ( pNode->Index[n] > 0 )
			{
				if ( gObjIsConnectedGP(pNode->Index[n]) == FALSE )
				{
					LogAddTD("SetGuildType() error-L2 : Index %s %d", __FILE__, __LINE__);
					continue;
				}

				if ( !OBJMAX_RANGE(pNode->Index[n]) )
				{
					continue;
				}

				LPOBJ lpObj = &gObj[pNode->Index[n]];
				GCGuildViewportNowPaint(lpObj->m_Index, lpObj->GuildName, 0, 0);
			}
		}
	}

	return TRUE;
}

BOOL CGuildClass::BuildMemberTotal(_GUILD_INFO_STRUCT * lpNode)
{
	if (lpNode == NULL )
	{
		return FALSE;
	}

	lpNode->TotalCount = 0;
	lpNode->Count = 0;

	for ( int n=0;n<MAX_USER_GUILD;n++)
	{
		if ( lpNode->Use[n] == TRUE )
		{
			lpNode->TotalCount++;

			if ( lpNode->Index[n] != 0 )
			{
				lpNode->Count++;
			}
		}
	}
	return TRUE;
}


BOOL CGuildClass::DelMember(char* guild_name, char* player_name)
{
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(guild_name);

	if (pNode == NULL )
	{
		return FALSE;
	}

	for ( int n=0;n<MAX_USER_GUILD;n++)
	{
		if ( pNode->Use[n] == TRUE )
		{
			if ( strcmp(pNode->Names[n], player_name ) == 0 )
			{
				pNode->Use[n] = FALSE;
				pNode->Count--;
				this->BuildMemberTotal(pNode);
				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL CGuildClass::CloseMember(char* guild_name, char* player_name)
{
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(guild_name);

	if (pNode == NULL )
	{
		return FALSE;
	}

	for ( int n=0;n<MAX_USER_GUILD;n++)
	{
		if ( pNode->Use[n] == TRUE )
		{
			if ( strcmp(pNode->Names[n], player_name ) == 0 )
			{
				pNode->Index[n] = -1;
				pNode->Count--;
				pNode->pServer[n] = -1;

				this->BuildMemberTotal(pNode);
				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL CGuildClass::SetServer(char* guild_name, char* player_name, int pServer)
{
	_GUILD_INFO_STRUCT * pNode = this->SearchGuild(guild_name);

	if (pNode == NULL )
	{
		return FALSE;
	}

	for ( int n=0;n<MAX_USER_GUILD;n++)
	{
		if ( pNode->Use[n] == TRUE )
		{
			if ( strcmp(pNode->Names[n], player_name ) == 0 )
			{
				pNode->pServer[n] = pServer;
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL gGuildNoticeStringCheck(char* notice)
{
	int len = strlen(notice);

	for (int i =0; i<len ;i++)
	{
		if ( (notice[i] & 0x80 ) != 0 )
		{
			return TRUE;
		}
		else
		{
			if (notice[i] != ' ' )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
				
#if (__GENS_SYSTEM__ == 1)

CGensSystem	gGensSystem;

struct GENS_LEVEL_CONTRIBUTION
{
	BYTE btLevel;
	int MinContrib;
	int RankMax;
}  g_GensLevelInfo[MAX_GENS_LEVEL];
/* = {
	14,	0,		-1,
	13,	1000,	-1,
	12,	5000,	-1,
	11,	15000,	-1,
	10,	50000,	-1,
	9,	100000,	-1,
	8,	100000,	300,
	7,	100000,	200,
	6,	100000,	100,
	5,	100000,	50,
	4,	100000,	30,
	3,	100000,	10,
	2,	100000,	5,
	1,	100000,	1,
};*/

// constructor ~ destructor

#include "DBSockMng.h"
#include "EDSProtocol.h"
#include "MapClass.h"
#include "MapServerManager.h"
#include "DSProtocol.h"
#include "..\common\winutil.h"
#include "..\include\ReadScript.h"
#include "..\pugixml\pugixml.hpp"

using namespace pugi;

CGensSystem::CGensSystem()
{
	m_bENABLED = 1;
	m_iREWARD_DAYS = -1;
	m_iREWARD_DAYS_CHECK = 7;
	m_iREWARD_UPDATE_DAYS = 30;
	m_iUPDATE_REMAIN_MSEC = 0;
	m_iUPDATE_TICK_COUNT = 0;
	m_bINFO_REQUESTED = FALSE;
	_GENS_UPDATE_TIME pTmp;
	pTmp.Hour = 0;
	pTmp.Min = 0;
	m_vtUpdateTime.push_back(pTmp);
}

CGensSystem::~CGensSystem()
{

}

void CGensSystem::LoadData(char* File)
{
	this->m_Reward.clear();
	// ----
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[GensSystem] Loading fail [%s]", File);
		return;
	}
	// ----
	xml_node GensData = Document.child("gens");
	xml_node ClassTable = GensData.child("classtable");
	xml_node RewardList = GensData.child("rewardlist");
	xml_node MapList = GensData.child("battlemaplist");
	xml_node ZoneList = GensData.child("battlezonelist");
	xml_node Node;
	// ----
	this->m_StartContribution = GensData.child("options").child("startpoint").text().as_int();
	this->m_MinLevel = GensData.child("options").child("minlevel").text().as_int();
	this->m_MinReset = GensData.child("options").child("minreset").text().as_int();
	this->m_ChatPrice = GensData.child("options").child("chatprice").text().as_int();
	this->m_CheckForParty = (bool)GensData.child("requirements").child("party").text().as_int();
	this->m_CheckForGuild = (bool)GensData.child("requirements").child("guild").text().as_int();
	this->m_CheckForAlliance = (bool)GensData.child("requirements").child("alliance").text().as_int();
	this->m_FriendlyFire = (bool)GensData.child("pvp").child("friendlyfire").text().as_int();
	this->m_FriendlyKill = (bool)GensData.child("pvp").child("friendlykill").text().as_int();
	// ----
	int TableCount = 0;
	// ----
	for( Node = ClassTable.child("class"); Node; Node = Node.next_sibling() )
	{
		int Class = Node.attribute("id").as_int();
		g_GensLevelInfo[TableCount].btLevel = Class;
		g_GensLevelInfo[TableCount].MinContrib = Node.attribute("mincontribution").as_int();
		g_GensLevelInfo[TableCount].RankMax = Node.attribute("maxrank").as_int();
		TableCount++;
	}
	// ----
	for( Node = RewardList.child("reward"); Node; Node = Node.next_sibling() )
	{
		GensRewardInfo lpReward;
		lpReward.Class = Node.attribute("class").as_int();
		lpReward.Type = Node.attribute("type").as_int();
		lpReward.Index = Node.attribute("index").as_int();
		lpReward.Level = Node.attribute("level").as_int();
		lpReward.Durability = Node.attribute("dur").as_int();
		lpReward.Count = Node.attribute("count").as_int();
		this->m_Reward.push_back(lpReward);
	}
	// ----
	BYTE MapListCount = 0;
	memset(this->m_MapList, 255, 11);
	// ----
	for( Node = MapList.child("move"); Node; Node = Node.next_sibling() )
	{
		if( MapListCount > 10 )
		{
			MsgBox("[Gens] File load error\nMax map count > 11\nMap: %d", Node.attribute("id").as_int());
			continue;
		}
		// ----
		int MapNumber = Node.attribute("id").as_int();
		this->m_MapList[MapListCount] = MapNumber;
		MapListCount++;
	}
	// ----
	BYTE ZoneListCount = 0;
	memset(this->m_ZoneList, 255, 11);
	// ----
	for( Node = ZoneList.child("map"); Node; Node = Node.next_sibling() )
	{
		if( ZoneListCount > 11 )
		{
			MsgBox("[Gens] File load error\nMax zone count > 11\nZone: %d", Node.attribute("id").as_int());
			continue;
		}
		// ----
		int MoveNumber = Node.attribute("id").as_int();
		this->m_ZoneList[ZoneListCount] = MoveNumber;
		ZoneListCount++;
	}
	// ----
	/*SMDFile = fopen(lpszFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[GensSystem] Load file failed !");
		return;
	}

	int Token,Index;
	m_vtUpdateTime.clear();

	memset(m_btMapNumber,0,sizeof(m_btMapNumber));

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		Index = TokenNumber;

		while( TRUE )
		{
			Token = GetToken();

			if( strcmp("end",TokenString) == NULL )
				break;

			if( Index == 0 )
			{

				m_iREWARD_DAYS_CHECK = TokenNumber;

				Token = GetToken();
				m_iREWARD_UPDATE_DAYS = TokenNumber;
			}
			else if( Index == 1 )
			{
				_GENS_UPDATE_TIME pTmp;

				pTmp.Hour = TokenNumber;

				Token = GetToken();
				pTmp.Min = TokenNumber;

				m_vtUpdateTime.push_back(pTmp);
			}
			else if( Index == 2 )
			{
				int Map = TokenNumber;

				if( Map < 0 || Map >= MAX_NUMBER_MAP )
					continue;

				m_btMapNumber[Map] = TRUE;
			}
		}
	}

	fclose(SMDFile);

	m_bENABLED = GetPrivateProfileInt("GameServerInfo","GensSystemRun",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	if( m_bENABLED != TRUE )
		return*/
}

void CGensSystem::CheckSync()
{
	std::list<_GENS_UPDATE_TIME>::iterator Itor_;

	_GENS_UPDATE_TIME pFirstTime;
	_GENS_UPDATE_TIME pTime;

	DWORD FIRST_TIME = 0;
	DWORD CHECK_TIME = 0;
	DWORD NOW_TIME = 0;

	m_iUPDATE_TICK_COUNT = GetTickCount();

	if( m_vtUpdateTime.size() == 0)
	{
		MsgBox("Error : Blood Castle StartTime size is 0");
		return;
	}

	tm * today;
	time_t ltime;

	time(&ltime);
	today = localtime(&ltime);

	NOW_TIME = today->tm_hour * 60 + today->tm_min;

	pFirstTime = *m_vtUpdateTime.begin();

	for( Itor_ = m_vtUpdateTime.begin() ; Itor_ != m_vtUpdateTime.end(); ++Itor_ )
	{
		pTime = *Itor_;

		FIRST_TIME = pFirstTime.Hour * 60 + pFirstTime.Min;
		CHECK_TIME = pTime.Hour * 60 + pTime.Min;

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
		if( Itor_ == m_vtUpdateTime.end() )
		{
			Itor_ = m_vtUpdateTime.begin();
			pTime = *Itor_;
		}

		CHECK_TIME = pTime.Hour * 60 + pTime.Min;

		if( today->tm_hour <= pTime.Hour &&
			CHECK_TIME > NOW_TIME )
		{
			m_iUPDATE_REMAIN_MSEC = (CHECK_TIME -NOW_TIME)*60*1000;
		}
		else
		{
			m_iUPDATE_REMAIN_MSEC = (1440 - NOW_TIME + CHECK_TIME)*60*1000;
		}
	}

	DWORD Tick = GetTickCount();
	m_iUPDATE_REMAIN_MSEC -= today->tm_sec*1000;

	LogAddC(4,"[GensSystem] Sync Update Time. [%d] min remain",m_iUPDATE_REMAIN_MSEC/60000);
}

void CGensSystem::UpdateRun()
{
	if( m_bENABLED == FALSE )
		return;

	if( cDBSMng.m_ActiveServer == -1 )
		return;

	int iTICK = GetTickCount() - m_iUPDATE_TICK_COUNT;

	if( iTICK >= 10000 )
	{
		m_iUPDATE_TICK_COUNT = GetTickCount();
		m_iUPDATE_REMAIN_MSEC -= iTICK;
	}

	if( m_iUPDATE_REMAIN_MSEC <= 0 || m_bINFO_LOADED == FALSE )
	{
		if( m_bINFO_REQUESTED == FALSE )
			GDRequestServerInfo(g_MapServerManager.GetMapSvrGroup());
	}
}

struct PMSG_REQ_GENS_SERVERINFO
{
	PBMSG_HEAD2 h;
	BYTE btMapSvrCode;
	BYTE btUpdateDays;
	BYTE btPrizeDays;
};

struct PMSG_ANS_GENS_SERVERINFO
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btUpdateDays;
};

void CGensSystem::GDRequestServerInfo(int ServerCode)
{
	m_bINFO_REQUESTED = TRUE;
	m_bINFO_LOADED = FALSE;

	PMSG_REQ_GENS_SERVERINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0x40,0x00,sizeof(pMsg));

	pMsg.btMapSvrCode = ServerCode;
	pMsg.btUpdateDays = m_iREWARD_UPDATE_DAYS;
	pMsg.btPrizeDays = m_iREWARD_DAYS_CHECK;

	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));
	LogAddC(3,"[GensSystem] Request Update Info !");
}

void CGensSystem::DGAnswerServerInfo(LPBYTE aRecv)
{
	PMSG_ANS_GENS_SERVERINFO* lpMsg = (PMSG_ANS_GENS_SERVERINFO*)aRecv;

	if( lpMsg->btResult == TRUE )
	{
		m_bINFO_LOADED = TRUE;
		m_iREWARD_DAYS = lpMsg->btUpdateDays;
	}

	m_bINFO_REQUESTED = FALSE;

	LogAddC(4,"[GensSystem] Information recieved!");
	CheckSync();
}

BOOL CGensSystem::GensMap(int MapNumber)
{
	if( m_bENABLED == FALSE )
	{
		return FALSE;
	}

	if( gNonPK == TRUE )
	{
		return FALSE;
	}

	if( MapNumber < 0 || MapNumber >= MAX_NUMBER_MAP )
	{
		return FALSE;
	}
	
	for( int i = 0; i < 11; i++ )
	{
		if( this->m_ZoneList[i] == MapNumber )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

void CGensSystem::UserDie(int aIndex,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	int iAddContrib = 0;
	int iDelContrib = 3;

	if( lpObj->Level == lpTargetObj->Level )
	{
		iAddContrib = 5;
		iDelContrib = 3;
	}
	else if( lpTargetObj->Level < lpObj->Level )
	{
		int Diff = lpObj->Level-lpTargetObj->Level;

		if( Diff >= 51 )
		{
			iAddContrib = 7;
			iDelContrib = 1;
		}
		else if( Diff >= 31 && Diff <= 50 )
		{
			iAddContrib = 6;
			iDelContrib = 2;
		}
		else if( Diff >= 30 && Diff <= 11 )
		{
			iAddContrib = 6;
		}
		else if( Diff >= 1 && Diff <= 10 )
		{
			iAddContrib = 5;
		}
	}
	else if( lpTargetObj->Level > lpObj->Level )
	{
		int Diff = lpTargetObj->Level - lpObj->Level;

		if( Diff >= 51 )
		{
			iAddContrib = 1;
		}
		else if( Diff >= 31 && Diff <= 50 )
		{
			iAddContrib = 2;
		}
		else if( Diff >= 11 && Diff <= 30 )
		{
			iAddContrib = 3;
		}
		else if( Diff <= 10 )
		{
			iAddContrib = 5;
		}
	}

	lpTargetObj->m_GensInfo.iContribution += iAddContrib;
	lpObj->m_GensInfo.iContribution -= iDelContrib;

	if( lpObj->m_GensInfo.iContribution < 0 )
		lpObj->m_GensInfo.iContribution = 0;
}

GensRewardInfo* CGensSystem::GetReward(int GensClass)
{
	for( int i = 0; i < this->m_Reward.size(); i++ )
	{
		if( this->m_Reward[i].Class == GensClass )
		{
			return &this->m_Reward[i];
		}
	}

	return 0;
}

struct PMSG_GENS_VIEWPORT_SELF
{
	PBMSG_HEAD2 h;
	BYTE btFamily;
	int iRank;
	BYTE btSign;
	int iContribution;
	int iNextContribution;
};

struct PMSG_GENS_VIEWPORT
{
  PWMSG_HEAD h;
  BYTE btSubCode;
  BYTE btCount;
};

struct PMSG_GENS_VIEWPORTCREATE
{
  BYTE btFamily;
  BYTE IndexHI;
  BYTE IndexLO;
  int Rank;
  int Level;
  int Contrib;
};



struct PMSG_GENS_JOINREQUEST
{
	PBMSG_HEAD2 h;
	BYTE btSubCode;
};

struct PMSG_GENS_JOINREQUEST_ANS
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btSubCode;
};

struct PMSG_REQ_GENS_JOINREQUEST
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szRequestID[11];
	char szGuildMasterID[11];
	BYTE btGensType;
};


int CGensSystem::GetGensLevel(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_GensInfo.btFamily == GENS_NONE )
		return MAX_GENS_LEVEL;

	int iLevel = MAX_GENS_LEVEL;

	for(int i = 0; i < MAX_GENS_LEVEL; i++)
	{
		if( lpObj->m_GensInfo.iContribution >= g_GensLevelInfo[i].MinContrib )
		{
			if( g_GensLevelInfo[i].RankMax == -1 || lpObj->m_GensInfo.iRank <= g_GensLevelInfo[i].RankMax )
			{
				iLevel = g_GensLevelInfo[i].btLevel;
			}
		}
	}

return iLevel;
}

int CGensSystem::GetNextContribution(int iGensLevel)
{
	if( iGensLevel <= 9 )
		return -1;

	for(int i = 0; i < (MAX_GENS_LEVEL-1); i++)
	{
		if( g_GensLevelInfo[i].btLevel == (iGensLevel-1) )
		{
			return g_GensLevelInfo[i].MinContrib;
		}
	}

return -1;
}


void CGensSystem::CGJoinRequest(LPBYTE aRecv,int aIndex)
{

	if( !gObjIsConnected(aIndex) )
		return;

	PMSG_GENS_JOINREQUEST* lpMsg = (PMSG_GENS_JOINREQUEST*)aRecv;

	LPOBJ lpObj = &gObj[aIndex];
	PMSG_GENS_JOINREQUEST_ANS pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF8,0x02,sizeof(pMsg));

	pMsg.btSubCode = lpMsg->btSubCode;

	if( lpObj->m_GensInfo.btFamily != GENS_NONE )
	{
		pMsg.btResult = 1;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpObj->Level < this->m_MinLevel )
	{
		pMsg.btResult = 3;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpObj->iResetCount < this->m_MinReset )
	{
		pMsg.btResult = 3;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpObj->PartyNumber >= 0)
	{
		pMsg.btResult = 6;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	PMSG_REQ_GENS_JOINREQUEST pMsgReq = {0};
	PHeadSubSetB((LPBYTE)&pMsgReq,0x40,0x02,sizeof(pMsgReq));

	if( lpObj->lpGuild != NULL )
	{
		if( lpObj->GuildStatus == 0x80 )
		{
			if( lpObj->lpGuild->iGuildUnion > 0 )
			{
				pMsg.btResult = 7;
				DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
				return;
			}
		}
		else
		{
			strcpy(pMsgReq.szGuildMasterID,lpObj->lpGuild->Names[0]);
		}
	}

	pMsgReq.aIndex = aIndex;
	pMsgReq.btGensType = lpMsg->btSubCode;

	strcpy(pMsgReq.szRequestID,gObj[aIndex].Name);

	cDBSMng.Send((PCHAR)&pMsgReq,sizeof(pMsgReq));
}

struct PMSG_GENS_LEAVE_ANS
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_REQ_LEAVE_GENS
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
	BYTE btGens;
};

void CGensSystem::CGLeaveRequest(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_GENS_LEAVE_ANS pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF8,0x04,sizeof(pMsg));

	if( lpObj->m_GensInfo.btFamily == GENS_NONE )
	{
		pMsg.btResult = 1;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}
	
	if( lpObj->lpGuild != NULL && lpObj->GuildStatus == 0x80 )
	{
		pMsg.btResult = 2;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	PMSG_REQ_LEAVE_GENS pMsgReq = {0};
	PHeadSubSetB((LPBYTE)&pMsgReq,0x40,0x04,sizeof(pMsgReq));

	pMsgReq.aIndex = aIndex;

	if( lpObj->TargetShopNumber == 543 )
	{
		pMsgReq.btGens = GENS_DUPRIAN;
	}
	else
	{
		pMsgReq.btGens = GENS_VANERT;
	}

	strcpy(pMsgReq.szName,lpObj->Name);

	cDBSMng.Send((PCHAR)&pMsgReq,sizeof(pMsgReq));
}

struct PMSG_GENS_REQPRIZE
{
	PBMSG_HEAD2 h;
	BYTE btFamily;
};

void CGensSystem::CGRequestPrize(LPBYTE aRecv,int aIndex)
{
	PMSG_GENS_REQPRIZE* lpMsg = (PMSG_GENS_REQPRIZE*)aRecv;

	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use == TRUE )
		return;

	if( lpObj->TargetShopNumber != 543 && lpObj->TargetShopNumber != 544 )
		return;

	PMSG_GENS_LEAVE_ANS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xF8,0x0A,sizeof(pMsg));

	if( lpObj->m_GensInfo.btFamily == GENS_NONE )
	{
		pMsg.btResult = 6;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpObj->m_GensInfo.btFamily != lpMsg->btFamily )
	{
		pMsg.btResult = 5;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpObj->m_GensInfo.btReward == TRUE )
	{
		pMsg.btResult = 4;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( m_iREWARD_DAYS >= m_iREWARD_DAYS_CHECK ||
		m_bINFO_LOADED == FALSE )
	{
		pMsg.btResult = 1;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	int GensLevel = GetGensLevel(aIndex);

	if( GensLevel <= 0 || GensLevel > 8 )
	{
		pMsg.btResult = 2;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	GensRewardInfo* lpPrize = this->GetReward(GensLevel);

	int iEmptyCount = 0;

	for ( int x=0;x<(MAIN_INVENTORY_SIZE-INVETORY_WEAR_SIZE);x++)
	{
		if ( lpObj->pInventoryMap[x] == 0xFF )
		{
			iEmptyCount++;
		}
	}

	if( iEmptyCount < lpPrize->Count )
	{
		pMsg.btResult = 3;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	LogAddTD("[GensSystem] [%s][%s] GensLevel:[%d] Reward info: TYPE:[%d,%d] COUNT:[%d]",
		lpObj->AccountID,lpObj->Name,GensLevel,lpPrize->Type,lpPrize->Index,lpPrize->Count);

	int iCrtJewelCount = 0;
	BOOL bItemCrtOK = FALSE;

	for (int x=0;x<(MAIN_INVENTORY_SIZE-INVETORY_WEAR_SIZE);x++)
	{
		if ( lpObj->pInventoryMap[x] == 0xFF )
		{
			ItemSerialCreateSend(aIndex, 235, lpObj->X, lpObj->Y, ITEMGET(lpPrize->Type, lpPrize->Index), lpPrize->Level, lpPrize->Durability, 0, 0, 0,aIndex, 0, 0);

			iCrtJewelCount++;

			if ( lpPrize->Count <= iCrtJewelCount )
			{
				bItemCrtOK = TRUE;
				break;
			}
		}
	}

	lpObj->m_GensInfo.btReward = TRUE;

	if( bItemCrtOK == TRUE )
	{
		GDSaveGensInfo(aIndex);

		LogAddTD("[GensSystem] [%s][%s] Reward Success!",lpObj->AccountID,lpObj->Name);
		pMsg.btResult = 0;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}
	else
	{
		LogAddTD("[GensSystem] [%s][%s] Reward Fail! iCrtJewelCount: %d",lpObj->AccountID,lpObj->Name,iCrtJewelCount);
	}
}

void CGensSystem::SendGensInfo(int aIndex)
{
	PMSG_GENS_VIEWPORT_SELF pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF8,0x07,sizeof(pMsg));

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_GensInfo.btFamily == GENS_NONE )
	{
		pMsg.iNextContribution = -1;
	}
	else
	{
		pMsg.btSign = lpObj->m_GensInfo.btLevel;
		pMsg.iRank = lpObj->m_GensInfo.iRank;
		pMsg.iContribution = lpObj->m_GensInfo.iContribution;
		pMsg.iNextContribution = GetNextContribution(pMsg.btSign);
	}

	pMsg.btFamily = lpObj->m_GensInfo.btFamily;
	

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

struct PMSG_ANS_GENS_JOINREQUEST
{
	PBMSG_HEAD2 h;
	int aIndex;
	BYTE btResult;
	BYTE btGensType;
	int iRank;
};

struct PMSG_ANS_GENS_INFO
{
	PBMSG_HEAD2 h;
	int aIndex;
	int iRank;
	int iContribution;
	BYTE iReward;
	BYTE btResult;
	BYTE btGens;
};

struct PMSG_REQ_GENS_INFO
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
};

void CGensSystem::GDReqGensInfo(int aIndex)
{
	PMSG_REQ_GENS_INFO pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0x40,0x01,sizeof(pMsg));
	pMsg.aIndex = aIndex;
	strcpy(pMsg.szName,gObj[aIndex].Name);

	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));
}

struct PMSG_GENS_INFOSAVE
{
	PBMSG_HEAD2 h;
	char szName[11];
	BYTE iReward;
	int iContribution;
};

void CGensSystem::GDSaveGensInfo(int aIndex)
{
	PMSG_GENS_INFOSAVE pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0x40,0x03,sizeof(pMsg));

	strcpy(pMsg.szName,gObj[aIndex].Name);
	pMsg.iContribution = gObj[aIndex].m_GensInfo.iContribution;
	pMsg.iReward = gObj[aIndex].m_GensInfo.btReward;

	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));
}

void CGensSystem::DGAnsGensInfo(LPBYTE aRecv)
{
	PMSG_ANS_GENS_INFO* lpMsg = (PMSG_ANS_GENS_INFO*)aRecv;

	int aIndex = lpMsg->aIndex;

	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER )
		return;

	if( lpMsg->btResult == TRUE )
	{
		lpObj->m_GensInfo.btFamily = lpMsg->btGens;
		lpObj->m_GensInfo.iRank = lpMsg->iRank;
		lpObj->m_GensInfo.iContribution = lpMsg->iContribution;
		lpObj->m_GensInfo.btLevel = GetGensLevel(aIndex);
		lpObj->m_GensInfo.btReward = lpMsg->iReward;

		SendGensInfo(aIndex);
	}
}

void CGensSystem::DGAnsJoinRequest(LPBYTE aRecv)
{
	PMSG_ANS_GENS_JOINREQUEST* lpMsg = (PMSG_ANS_GENS_JOINREQUEST*) aRecv;

	int aIndex = lpMsg->aIndex;

	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER )
		return;

	PMSG_GENS_JOINREQUEST_ANS pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF8,0x02,sizeof(pMsg));
	

	if( lpMsg->btResult != 0 )
	{
		pMsg.btResult = lpMsg->btResult;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	lpObj->m_GensInfo.btFamily = lpMsg->btGensType;
	lpObj->m_GensInfo.iRank = lpMsg->iRank;
	lpObj->m_GensInfo.iContribution = this->m_StartContribution;

	pMsg.btResult = 0;
	pMsg.btSubCode = lpMsg->btGensType;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);

	BYTE GensBuffer[128] = {0};

	PMSG_GENS_VIEWPORT pViewport;
	PMSG_GENS_VIEWPORTCREATE pViewportCreate;

	int lOfsGens = sizeof(pViewport);

	pViewportCreate.btFamily = lpMsg->btGensType;
	pViewportCreate.Contrib = 0;
	pViewportCreate.Level = GetGensLevel(aIndex);
	pViewportCreate.Rank = lpMsg->iRank;

	pViewportCreate.IndexHI = SET_NUMBERH(aIndex);
	pViewportCreate.IndexLO = SET_NUMBERL(aIndex);

	memcpy(&GensBuffer[lOfsGens],&pViewportCreate,sizeof(pViewportCreate));
	lOfsGens += sizeof(pViewportCreate);

	pViewport.h.c = 0xC2;
	pViewport.h.sizeH = SET_NUMBERH(lOfsGens);
	pViewport.h.sizeL = SET_NUMBERL(lOfsGens);
	pViewport.h.headcode = 0xF8;

	pViewport.btSubCode = 0x05;
	pViewport.btCount = 1;

	memcpy(GensBuffer,&pViewport,sizeof(pViewport));

	MsgSendV2(lpObj,(LPBYTE)GensBuffer,lOfsGens);
}

struct PMSG_ANS_LEAVE_GENS
{
	PBMSG_HEAD2 h;
	int aIndex;
	BYTE btResult;
};

void CGensSystem::DGAnsLeaveRequest(LPBYTE aRecv)
{
	PMSG_ANS_LEAVE_GENS* lpMsg = (PMSG_ANS_LEAVE_GENS*)aRecv;

	int aIndex =  lpMsg->aIndex;

	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_GENS_LEAVE_ANS pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF8,0x04,sizeof(pMsg));

	pMsg.btResult = 0;

	if( lpMsg->btResult != 0 )
	{
		
		pMsg.btResult = lpMsg->btResult;

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	lpObj->m_GensInfo.btFamily = GENS_NONE;
	lpObj->m_GensInfo.iContribution = 0;
	lpObj->m_GensInfo.btLevel = 0;
	lpObj->m_GensInfo.iRank = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);


	BYTE GensBuffer[128] = {0};

	PMSG_GENS_VIEWPORT pViewport;
	PMSG_GENS_VIEWPORTCREATE pViewportCreate;

	int lOfsGens = sizeof(pViewport);

	pViewportCreate.btFamily = 0;
	pViewportCreate.Contrib = 0;
	pViewportCreate.Level = 0;
	pViewportCreate.Rank = 0;

	pViewportCreate.IndexHI = SET_NUMBERH(aIndex);
	pViewportCreate.IndexLO = SET_NUMBERL(aIndex);

	memcpy(&GensBuffer[lOfsGens],&pViewportCreate,sizeof(pViewportCreate));
	lOfsGens += sizeof(pViewportCreate);

	pViewport.h.c = 0xC2;
	pViewport.h.sizeH = SET_NUMBERH(lOfsGens);
	pViewport.h.sizeL = SET_NUMBERL(lOfsGens);
	pViewport.h.headcode = 0xF8;

	pViewport.btSubCode = 0x05;
	pViewport.btCount = 1;

	memcpy(GensBuffer,&pViewport,sizeof(pViewport));

	MsgSendV2(lpObj,(LPBYTE)GensBuffer,lOfsGens);
}

#endif