#include "stdafx.h"
#include "PartyClass.h"
#include "LogProc.h"
#include "Protocol.h"
#include "GameMain.h"
#include "DBSockMng.h"
#include "GuildMatch.h"
#include "..\common\winutil.h"
#include "..\pugixml\pugixml.hpp"
#include "ChaosCastle.h"
#include "IllusionTempleEvent.h"
#include "DoubleGoerEvent.h"
#include "ImperialGuardian.h"

using namespace pugi;

PartyClass::PartyClass()
{

}


PartyClass::~PartyClass()
{

}


BOOL PartyClass::IsParty(int party_number)
{
	if ( party_number < 0 || party_number >= OBJMAX )
	{
		return FALSE;
	}

	if ( this->m_PartyS[party_number].Count == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


BOOL PartyClass::Isleader(int party_number, int usernumber, int dbnumber)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return FALSE;
	}

	if ( this->m_PartyS[party_number].Number[0] == usernumber && this->m_PartyS[party_number].DbNumber[0] == dbnumber )
	{
		return TRUE;
	}

	return FALSE;
}


int PartyClass::GetCount(int party_number)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return 0;
	}

	return this->m_PartyS[party_number].Count;
}


BOOL PartyClass::GetIndexUser(int party_number, int index, int& usernumber,  int& dbnumber)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return FALSE;
	}

	if ( index <0 || index > MAX_USER_IN_PARTY-1 )
	{
		return FALSE;
	}

	usernumber = this->m_PartyS[party_number].Number[index];
	dbnumber = this->m_PartyS[party_number].DbNumber[index];

	return TRUE;
}



int PartyClass::GetIndex(int party_number, int usernumber, int dbnumber)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return 0;
	}

	for ( int n=0; n<MAX_USER_IN_PARTY;n++)
	{
		if ( this->m_PartyS[party_number].Number[n] == usernumber || this->m_PartyS[party_number].DbNumber[n] == dbnumber )
		{
			return n;
		}
	}

	return -1;
}


int PartyClass::Create(int usernumber, int dbnumber, int level)
{
	int n = this->m_PartyCount;
	int loopcount = 0;

	while ( true )
	{
		if ( this->m_PartyS[n].Count == 0 )
		{
			this->m_PartyS[n].Count = 1;
			this->m_PartyS[n].Number[0] = usernumber;
			this->m_PartyS[n].DbNumber[0] = dbnumber;

			for ( int i=1;i<MAX_USER_IN_PARTY;i++)
			{
				this->m_PartyS[n].DbNumber[i] = -1;
				this->m_PartyS[n].Number[i] = -1;
			}

			this->m_PartyCount++;
			this->m_PartyS[n].m_MaxLevel = 0;
			this->m_PartyS[n].m_MinLevel = 0;
			this->SetLevel(n, level);

			if ( this->m_PartyCount > OBJMAX-1 )
			{
				this->m_PartyCount=0;
			}

			this->m_PartyS[n].m_PkLevel[0] = gObj[usernumber].m_PK_Level;

			if( this->m_PartyS[n].m_PkLevel[0] >= 5 )
			{
				this->m_PartyS[n].m_PartyPkLevel = m_PartyS[n].m_PkLevel[0];
				this->m_PartyS[n].m_PkCount = 1;
			}
			else
			{
				this->m_PartyS[n].m_PartyPkLevel = 3;
				this->m_PartyS[n].m_PkCount = 0;
			}

			return n;
		}

		n++;

		if ( n> OBJMAX-1 )
		{
			n=0;
		}

		loopcount++;

		if ( loopcount > OBJMAX-1 )
		{
			break;
		}
	}

	return -1;
}

BOOL PartyClass::SetLevel(int party_number, int level)
{
	if ( this->m_PartyS[party_number].m_MaxLevel  < level )
	{
		this->m_PartyS[party_number].m_MaxLevel  = level;
	}

	if ( this->m_PartyS[party_number].m_MinLevel  == 0 )
	{
		this->m_PartyS[party_number].m_MinLevel = level;
	}

	if ( this->m_PartyS[party_number].m_MinLevel > level )	// In Case of Error
	{
		this->m_PartyS[party_number].m_MinLevel = level;
	}

	return TRUE;
}


BOOL PartyClass::GetLevel(int party_number, int& maxlevel, int& minlevel)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return FALSE;
	}

	maxlevel = this->m_PartyS[party_number].m_MaxLevel;
	minlevel = this->m_PartyS[party_number].m_MinLevel;

	return TRUE;
}


BOOL PartyClass::Destroy(int party_number)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return FALSE;
	}

	this->m_PartyS[party_number].Count = 0;
	this->m_PartyS[party_number].Number[0] = -1;
	this->m_PartyS[party_number].DbNumber[0] = -1;
	this->m_PartyCount--;

	return FALSE;
}


int PartyClass::Add(int party_number, int usernumber, int dbnumber, int level)
{
	if ( this->IsParty(party_number ) == FALSE )
	{
		return -1;
	}

	if ( this->m_PartyS[party_number].Count >= OBJMAX )	// Useless
	{
		return -1;
	}

	int i;
	for ( i = 0; i<MAX_USER_IN_PARTY ; i++)
	{
		if ( (this->m_PartyS[party_number].Number[i] >= 0) && (this->m_PartyS[party_number].Number[i] == usernumber) && (this->m_PartyS[party_number].DbNumber[i] == dbnumber) )
		{
			return -2;
		}
	}

	for ( i = 0; i<MAX_USER_IN_PARTY ; i++ )
	{
		if ( this->m_PartyS[party_number].Number[i] == -1 )
		{
			this->m_PartyS[party_number].Number[i] = usernumber;
			this->m_PartyS[party_number].DbNumber[i] = dbnumber;
			this->m_PartyS[party_number].Count++;
			this->SetLevel(party_number, level);

			this->m_PartyS[party_number].m_PkLevel[i] = gObj[usernumber].m_PK_Level;

			if( m_PartyS[party_number].m_PkLevel[i] >= 5 )
			{
				if( m_PartyS[party_number].m_PartyPkLevel < m_PartyS[party_number].m_PkLevel[i] )
				{
					m_PartyS[party_number].m_PartyPkLevel = m_PartyS[party_number].m_PkLevel[i];
				}

				m_PartyS[party_number].m_PkCount++;
			}

			return i;
		}
	}

	return -1;
}

int PartyClass::Delete(int party_number, int usernumber, int dbnumber)	// Kick a User from Party
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return -1;
	}

	for ( int i = 0 ; i<MAX_USER_IN_PARTY ; i++ )
	{
		if ( (this->m_PartyS[party_number].Number[i] == usernumber) && (this->m_PartyS[party_number].DbNumber[i] == dbnumber) )
		{
			this->m_PartyS[party_number].Number[i] = -1;
			this->m_PartyS[party_number].DbNumber[i] = -1;
			this->m_PartyS[party_number].Count--;
			this->m_PartyS[party_number].m_PkLevel[0] = 3;
			return i;
		}
	}

	return -1;
}


void PartyClass::Delete(int party_number, int index)	// Kick a User Again
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return;
	}

	if ( index < 0 || index > MAX_USER_IN_PARTY -1 )
	{
		return;
	}

	if ( this->m_PartyS[party_number].Number[index] >= 0 )
	{
		this->m_PartyS[party_number].Number[index] = -1;
		this->m_PartyS[party_number].DbNumber[index] = -1;
		this->m_PartyS[party_number].Count--;
		this->m_PartyS[party_number].m_PkLevel[0] = 3;
	}
}


int PartyClass::GetPartyCount(int party_number)
{
	if ( this->IsParty( party_number ) == FALSE )
	{
		return -1;
	}

	return this->m_PartyS[party_number].Count;
}


void PartyClass::Paint(int party_number)
{
	if (this->IsParty(party_number) == FALSE )
	{
		return;
	}

	int usern = 0;
	wsprintf( this->m_szTempPaint, "PartyMember(%d) :", this->m_PartyS[party_number].Count);

	for ( int i = 0; i< MAX_USER_IN_PARTY ; i++ )
	{
		if ( (this->m_PartyS[party_number].Number[i] != 0 ) && (this->m_PartyS[party_number].DbNumber[i] != 0 ) )
		{
			usern = this->m_PartyS[party_number].Number[i];

			if ( !gObjIsConnected(usern) )
				continue;

			strcat(this->m_szTempPaint, "[");
			strcat(this->m_szTempPaint, gObj[usern].AccountID);
			strcat(this->m_szTempPaint, "][");
			strcat(this->m_szTempPaint, gObj[usern].Name);
			strcat(this->m_szTempPaint, "], ");
		}
	}

	LogAddTD(this->m_szTempPaint);
}



struct PMSG_DEFAULT_COUNT
{
	PBMSG_HEAD h;
	BYTE Count;	// 3
};

struct PMSG_PARTYLIFEALL
{
	BYTE Life;
	BYTE Mana;
	char szName[10];
	BYTE btUnk;	//Channel
};


void PartyClass::PartyMemberLifeSend(int party_number)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return;
	}

	PMSG_DEFAULT_COUNT pCount;
	PMSG_PARTYLIFEALL pList;
	int usern;
	char sendbuf[1024];
	int lOfs = 0;
	int tlife = 0;

	PHeadSetB((LPBYTE)&pCount, 0x44, sizeof(pCount));
	pCount.Count = this->m_PartyS[party_number].Count;
	lOfs += sizeof(pCount);
	
	int i;
	for ( i = 0; i<MAX_USER_IN_PARTY ; i++ )
	{
		if ( (this->m_PartyS[party_number].Number[i] >= 0 ) && (this->m_PartyS[party_number].DbNumber[i] >= 0) )
		{
			usern = this->m_PartyS[party_number].Number[i];

			if ( gObjIsConnected(usern) != PLAYER_EMPTY )
			{
				memcpy(pList.szName,gObj[usern].Name,10);
				//pList.btUnk = 9;
				pList.Life = (BYTE)(((float)gObj[usern].Life / (float)(gObj[usern].MaxLife + gObj[usern].AddLife ))*100.0f);
				pList.Mana = (BYTE)(((float)gObj[usern].Mana / (float)(gObj[usern].MaxMana + gObj[usern].AddMana ))*100.0f);
				memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
			}
			else
			{
				this->m_PartyS[party_number].Number[i] = -1;
				this->m_PartyS[party_number].DbNumber[i] = -1;
				this->m_PartyS[party_number].Count--;
			}
		}
	}

	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));

	for ( i=0 ; i<MAX_USER_IN_PARTY ; i++ )
	{
		usern = this->m_PartyS[party_number].Number[i];

		if ( usern >= 0 )
		{
			DataSend(usern, (unsigned char*)sendbuf, lOfs);
		}
	}
}

char PartyClass::GetPartyPkLevel(int party_number)
{
	return this->m_PartyS[party_number].m_PartyPkLevel;
}

void PartyClass::SetPkLevel(int party_number,int usernumber,int dbnumber, BYTE PkLevel)
{
	if( this->IsParty(party_number) == FALSE )
	{
		return;
	}

	for(int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if( this->m_PartyS[party_number].Number[i] == usernumber )
		{
			if( this->m_PartyS[party_number].DbNumber[i] == dbnumber )
			{
				this->m_PartyS[party_number].m_PkLevel[i] = PkLevel;
				break;
			}
		}
	}
}

void PartyClass::SetPkCount(int party_number)
{
	int PkCount = 0;
	int Number = 0;

	for(int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		Number = this->m_PartyS[party_number].Number[i];

		if( Number >= 0 )
		{
			if( m_PartyS[party_number].m_PkLevel[i] >= 5 )
			{
				if( m_PartyS[party_number].m_PartyPkLevel < m_PartyS[party_number].m_PkLevel[i] )
				{
					m_PartyS[party_number].m_PartyPkLevel = m_PartyS[party_number].m_PkLevel[i];
				}

				PkCount++;
			}
		}
	}

	m_PartyS[party_number].m_PkCount = PkCount;

	if( PkCount = 0 )
	{
		m_PartyS[party_number].m_PartyPkLevel = 3;
	}
}

void PartyClass::ResetLeader(int party_number)
{
	int Number = 0;

	for(int i = 0; i < MAX_USER_IN_PARTY; i++ )
	{
		Number = m_PartyS[party_number].Number[i];

		if( Number >= 0 )
		{
			m_PartyS[party_number].Number[0] = m_PartyS[party_number].Number[i];
			m_PartyS[party_number].Number[i] = -1;

			m_PartyS[party_number].DbNumber[0] = m_PartyS[party_number].DbNumber[i];
			m_PartyS[party_number].DbNumber[i] = -1;

			m_PartyS[party_number].m_PkLevel[0] = m_PartyS[party_number].m_PkLevel[i];
			m_PartyS[party_number].m_PkLevel[i] = 3;
			break;
		}
	}
}

void PartyClass::ReadBonusInfo(LPSTR File) //FILE_PARTYBONUS
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[Notice] File %s not found!", File);
		return;
	}
	// ----
	this->m_BonusInfo.m_NearbyBonus.clear();
	this->m_BonusInfo.m_ClassBonus.clear();
	// ----
	xml_node BonusInfo = Document.child("partybonus");
	xml_node NearbyBonus = BonusInfo.child("partymembernearby");
	xml_node ClassBonus = BonusInfo.child("partymemberclass");
	xml_node Node;
	// ----
	for( Node = NearbyBonus.child("bonus"); Node; Node = Node.next_sibling() )
	{
		PartyBonusByNearby lpInfo;
		lpInfo.m_NearbyCount = Node.attribute("nearbycount").as_int();
		lpInfo.m_AddExp = Node.text().as_int();
		this->m_BonusInfo.m_NearbyBonus.push_back(lpInfo);
	}
	// ----
	for( Node = ClassBonus.child("bonus"); Node; Node = Node.next_sibling() )
	{
		PartyBonusByClass lpInfo;
		lpInfo.m_ClassTable[0] = Node.attribute("dw").as_int();
		lpInfo.m_ClassTable[1] = Node.attribute("dk").as_int();
		lpInfo.m_ClassTable[2] = Node.attribute("elf").as_int();
		lpInfo.m_ClassTable[3] = Node.attribute("mg").as_int();
		lpInfo.m_ClassTable[4] = Node.attribute("dl").as_int();
		lpInfo.m_ClassTable[5] = Node.attribute("sum").as_int();
		lpInfo.m_ClassTable[6] = Node.attribute("rf").as_int();
		lpInfo.m_AddExp = Node.text().as_int();
		this->m_BonusInfo.m_ClassBonus.push_back(lpInfo);
	}
}

int PartyClass::GetExpBonus(LPOBJ lpUser, LPOBJ lpMonster, int PartyNumber)
{
	if( !lpUser )
	{
		return 0;
	}
	// ----
	int NearbyCount = 0;
	int NearbyBonus = 0;
	int ClassBonus = 0;
	BYTE ClassTable[MAX_TYPE_PLAYER] = { 0 };
	// ----
	for( int i = 0; i < MAX_USER_IN_PARTY; i++ )
	{
		int UserIndex = gParty.m_PartyS[PartyNumber].Number[i];
		// ----
		if( UserIndex < 0 )
		{
			continue;
		}
		// ----
		LPOBJ lpPartyUser = &gObj[UserIndex];
		// ----
		if( !lpPartyUser )
		{
			continue;
		}
		// ----
		if( lpMonster->MapNumber != lpPartyUser->MapNumber )
		{
			continue;
		}
		// ----
		if( gObjCalDistance(lpMonster, lpPartyUser) >= 10 )
		{
			continue;
		}
		// ----
		NearbyCount++;
		ClassTable[lpPartyUser->Class] = 1;
	}
	// ----
	for( int j = 0; j < this->m_BonusInfo.m_NearbyBonus.size(); j++ )
	{
		if( this->m_BonusInfo.m_NearbyBonus[j].m_NearbyCount == NearbyCount )
		{
			NearbyBonus = this->m_BonusInfo.m_NearbyBonus[j].m_AddExp;
		}
	}
	// ----
	for( int n = 0; n < this->m_BonusInfo.m_ClassBonus.size(); n++ )
	{
		if( memcmp(this->m_BonusInfo.m_ClassBonus[n].m_ClassTable, 
			ClassTable, MAX_TYPE_PLAYER) == 0 )
		{
			ClassBonus = this->m_BonusInfo.m_ClassBonus[n].m_AddExp;
		}
	}
	// ----
	return ClassBonus + NearbyBonus;
}

void PartyClass::PartyLog(int iPartyNumber,int iUserNumber,int iFlag)
{
	if( !OBJMAX_RANGE(iPartyNumber) )
		return;

	if( !OBJMAX_RANGE(iUserNumber) )
		return;

	char strTemp[260] = {0};
	char strPartyInfo[1024] = {0};
	int i;

	if( iFlag == 0 )
	{
		wsprintf(strPartyInfo,"[Party System] PartyIndex(%d): PartyBreak",iPartyNumber);
		LogAddTD(strPartyInfo);
		return;
	}

	wsprintf(strPartyInfo,"[Party System] PartyMember(%d), PartyIndex(%d) ",this->GetCount(iPartyNumber),iPartyNumber);

	for( i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		int usern = this->m_PartyS[iPartyNumber].Number[i];

		if( usern >= 0 )
		{
			if( gObj[usern].Connected == PLAYER_PLAYING ||
				gObj[usern].Connected == 4 )
			{
				wsprintf(strTemp,"[%s][%s] ",gObj[usern].AccountID,gObj[usern].Name);
				strcat(strPartyInfo,strTemp);
			}
		}
	}

	if( iFlag == 1 )
	{
		wsprintf(strTemp,"PartyOut : [%s][%s] ",gObj[iUserNumber].AccountID,gObj[iUserNumber].Name);
		strcat(strPartyInfo,strTemp);
	}
	else if( iFlag == 2 )
	{
		wsprintf(strTemp,"PartyRejoin : [%s][%s] ",gObj[iUserNumber].AccountID,gObj[iUserNumber].Name);
		strcat(strPartyInfo,strTemp);
	}
	else
	{
		return;
	}

	LogAddTD(strPartyInfo);
}

void PartyClass::UpdatePKPartyPanalty(int party_number)
{
	int PKUserCount = 0;
	int usern = 0;
	int i;

	for( i = 0 ; i < MAX_USER_IN_PARTY; i++)
	{
		usern = this->m_PartyS[party_number].Number[i];

		if( usern >= 0 )
		{
			if( this->m_PartyS[party_number].m_PkLevel[i] >= 5 )
			{
				if( this->m_PartyS[party_number].m_PartyPkLevel < this->m_PartyS[party_number].m_PkLevel[i] )
				{
					this->m_PartyS[party_number].m_PartyPkLevel = this->m_PartyS[party_number].m_PkLevel[i];
				}

				PKUserCount++;
			}
		}
	}

	this->m_PartyS[party_number].m_PkCount = PKUserCount;

	if( PKUserCount == 0 )
	{
		this->m_PartyS[party_number].m_PartyPkLevel = 3;
	}
}

int PartyClass::GetReallyConnectPartyMemberCount(int party_number)
{
	int nValidUserCount = 0;
	int usern = 0;

	if( !this->IsParty(party_number) )
		return -1;

	for(int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		usern = this->m_PartyS[party_number].Number[i];

		if( usern >= 0 && gObj[usern].Connected == PLAYER_PLAYING )
		{
			nValidUserCount++;
		}
	}

return nValidUserCount;
}

void PartyClass::SwitchLeader(int party_number)
{
	int usern = 0;
	int tempUserNumber = 0;
	int tempDbNumber = 0;
	char tempUserPKLevel = 3;

	if( !this->IsParty(party_number) )
		return;

	for(int i = 1; i < MAX_USER_IN_PARTY; i++)
	{
		usern = this->m_PartyS[party_number].Number[i];

		if( usern >= 0 && gObj[usern].Connected == PLAYER_PLAYING )
		{
			tempUserNumber = this->m_PartyS[party_number].Number[0];
			this->m_PartyS[party_number].Number[0] = this->m_PartyS[party_number].Number[i];
			this->m_PartyS[party_number].Number[i] = tempUserNumber;

			tempDbNumber = this->m_PartyS[party_number].DbNumber[0];
			this->m_PartyS[party_number].DbNumber[0] = this->m_PartyS[party_number].DbNumber[i];
			this->m_PartyS[party_number].DbNumber[i] = tempDbNumber;

			tempUserPKLevel = this->m_PartyS[party_number].m_PkLevel[0];
			this->m_PartyS[party_number].m_PkLevel[0] = this->m_PartyS[party_number].m_PkLevel[i];
			this->m_PartyS[party_number].m_PkLevel[i] = tempUserPKLevel;
			break;
		}
	}
}

void PartyClass::ChangeLeader(int party_number)
{
	int usern = 0;

	for(int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		usern = this->m_PartyS[party_number].Number[i];

		if( usern >= 0 )
		{
			this->m_PartyS[party_number].Number[0] = this->m_PartyS[party_number].Number[i];
			this->m_PartyS[party_number].Number[i] = -1;

			this->m_PartyS[party_number].DbNumber[0] = this->m_PartyS[party_number].DbNumber[i];
			this->m_PartyS[party_number].DbNumber[i] = -1;

			this->m_PartyS[party_number].m_PkLevel[0] = this->m_PartyS[party_number].m_PkLevel[i];
			this->m_PartyS[party_number].m_PkLevel[i] = -1;
			break;
		}
	}
}

bool PartyClass::SetLeader(int OldLeader, int NewLeader)
{
	if( !OBJMAX_RANGE(OldLeader) )
		return false;

	if( !OBJMAX_RANGE(NewLeader) )
		return false;

	LPOBJ lpObj			= &gObj[OldLeader];
	LPOBJ lpTargetObj	= &gObj[NewLeader];		
	// ----
	int PartyNumber		= lpObj->PartyNumber;
	// ----
	if( !this->IsParty(PartyNumber) )
	{
		GCServerMsgStringSend("Your character is not in party", lpObj->m_Index, 1);
		return false;
	}
	// ----
	if( lpObj->PartyNumber != lpTargetObj->PartyNumber )
	{
		GCServerMsgStringSend("Target is not party member", lpObj->m_Index, 1);
		return false;
	}
	// ----
	if( !this->Isleader(PartyNumber, lpObj->m_Index, lpObj->DBNumber) )
	{
		GCServerMsgStringSend("Your character is not party leader", lpObj->m_Index, 1);
		return false;
	}
	// ----
	int TempIndex = this->GetIndex(PartyNumber, lpTargetObj->m_Index, lpTargetObj->DBNumber);
	// ----
	this->m_PartyS[PartyNumber].Number[0]					= this->m_PartyS[PartyNumber].Number[TempIndex];
	this->m_PartyS[PartyNumber].DbNumber[0]					= this->m_PartyS[PartyNumber].DbNumber[TempIndex];
	this->m_PartyS[PartyNumber].m_PkLevel[0]				= this->m_PartyS[PartyNumber].m_PkLevel[TempIndex];
	// ----
	this->m_PartyS[PartyNumber].Number[TempIndex]			= lpObj->m_Index;
	this->m_PartyS[PartyNumber].DbNumber[TempIndex]			= lpObj->DBNumber;
	this->m_PartyS[PartyNumber].m_PkLevel[TempIndex]		= lpObj->m_PK_Level;
	// ----
	char Text[100] = { 0 };
	sprintf(Text, "New party leader: %s", lpTargetObj->Name);
	// ----
	for( int i = 0; i < MAX_USER_IN_PARTY; i++ )
	{
		int UserIndex = this->m_PartyS[PartyNumber].Number[i];
		// ----
		if( UserIndex >= 0 && gObj[UserIndex].Connected == 3 )
		{
			GCServerMsgStringSend(Text, this->m_PartyS[PartyNumber].Number[i], 1);
			CGPartyList(this->m_PartyS[PartyNumber].Number[i]);
		}
	}
	return true;
}

void CGReqPartyMatchReg(PMSG_PARTYMATCH_REG_REQ* lpMsg,int aIndex)
{
	// iResult
	// 0 - Success
	// -1 -> There has been an error
	// -2 -> Included in waiting list
	// -3 -> Can only be used by party leader

	PMSG_PARTYMATCH_REG_ANS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEF,0x00,sizeof(pMsg));

	pMsg.iResult = 0;

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	if ( gObj[aIndex].CloseCount >= 0 )
	{
		pMsg.iResult = -1;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	gObj[aIndex].m_bIsPartyMatch = TRUE;

	PMSG_PARTYMATCHING_REG_REQ pDBMsg;
	PHeadSubSetB((LPBYTE)&pDBMsg,0xEF,0x00,sizeof(pDBMsg));

	memcpy(pDBMsg.szDesc,lpMsg->szDesc,sizeof(pDBMsg.szDesc));
	pDBMsg.szDesc[sizeof(pDBMsg.szDesc)-1] = '\0';

	memcpy(pDBMsg.szName,gObj[aIndex].Name,sizeof(pDBMsg.szName));
	pDBMsg.szName[sizeof(pDBMsg.szName)-1] = '\0';

	memcpy(pDBMsg.szPass,lpMsg->szPass,sizeof(pDBMsg.szPass));
	pDBMsg.szPass[sizeof(pDBMsg.szPass)-1] = '\0';

	pDBMsg.ServerCode = gGameServerCode;
	pDBMsg.aIndex = aIndex;
	pDBMsg.btAutoAccept = lpMsg->btAutoAccept;
	pDBMsg.ClassInfo = lpMsg->ClassInfo;
	pDBMsg.MaxLevel = lpMsg->MaxLevel;
	pDBMsg.MinLevel = lpMsg->MinLevel;
	pDBMsg.MoveReqId = lpMsg->MoveReqId;
	pDBMsg.Gens = gObj[aIndex].m_GensInfo.btFamily;

	cDBSMng.Send((PCHAR)&pDBMsg,sizeof(pDBMsg));
}

void CGReqPartyMatchWindow(PMSG_PARTYMATCH_INFO_REQ* lpMsg,int aIndex)
{
	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	PMSG_PARTYMATCHING_INFO_REQ pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEF,0x01,sizeof(pMsg));

	pMsg.RequestPage = lpMsg->RequestPage;
	pMsg.ReqType = lpMsg->ReqType;
	pMsg.aIndex = aIndex;
	pMsg.Class = gObj[aIndex].Class;
	pMsg.Level = gObj[aIndex].Level;

#if (__GENS_SYSTEM__ == 1)
	pMsg.Gens = gObj[aIndex].m_GensInfo.btFamily;
#else
	pMsg.Gens = 0;
#endif

	memcpy(pMsg.szName,gObj[aIndex].Name,sizeof(pMsg.szName)-1);
	pMsg.szName[sizeof(pMsg.szName)-1] = '\0';

	memcpy(pMsg.szSearch,lpMsg->szSearch,sizeof(pMsg.szSearch)-1);
	pMsg.szSearch[sizeof(pMsg.szSearch)-1] = '\0';

	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));
}

void CGReqPartyMatchJoin(PMSG_PARTYMATCH_JOIN_REQ* lpMsg,int aIndex)
{
	// -7 Request limit reached
	// -6 Cannot request while in party
	// -5 Cannot apply to your own group
	// -4 Waiting for acceptance
	// -3 No open party avaible
	// -2 Party does not exists
	// -1 The password entered is incorect
	// 0 - Successful post

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	PMSG_PARTYMATCH_JOIN_ANS pResult;
	PHeadSubSetB((LPBYTE)&pResult,0xEF,0x02,sizeof(pResult));


	int TargetIndex = gObjGetIndex(lpMsg->szLeader);

	if( !OBJMAX_RANGE(TargetIndex) ||
		gObj[TargetIndex].Connected != PLAYER_PLAYING )
	{
		pResult.iResult = -2;
		DataSend(aIndex,(LPBYTE)&pResult,pResult.h.size);
		return;
	}

	if( !strcmp(gObj[TargetIndex].Name, gObj[aIndex].Name) )
	{
		pResult.iResult = -5;
		DataSend(aIndex,(LPBYTE)&pResult,pResult.h.size);
		return;
	}

	if( gObj[aIndex].PartyNumber >= 0 )
	{
		pResult.iResult = -6;
		DataSend(aIndex,(LPBYTE)&pResult,pResult.h.size);
		return;
	}

//	if( gObj[aIndex].m_bIsPartyWaiting == TRUE )
//	{
//		pResult.iResult = -4;
//		DataSend(aIndex,(LPBYTE)&pResult,pResult.h.size);
//		return;
//	}

//	gObj[aIndex].m_bIsPartyWaiting = TRUE;

	PMSG_PARTYMATCHING_JOIN_REQ pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEF,0x02,sizeof(pMsg));

	memcpy(pMsg.szName,gObj[aIndex].Name,sizeof(pMsg.szName)-1);
	pMsg.szName[sizeof(pMsg.szName)-1] = '\0';

	memcpy(pMsg.szPartyLeader,lpMsg->szLeader,sizeof(pMsg.szPartyLeader)-1);
	pMsg.szPartyLeader[sizeof(pMsg.szPartyLeader)-1] = '\0';

	memcpy(pMsg.szPassword,lpMsg->szPassword,sizeof(pMsg.szPassword)-1);
	pMsg.szPassword[sizeof(pMsg.szPassword)-1] = '\0';

	pMsg.aIndex = aIndex;
	pMsg.RequestType = lpMsg->btType;
	pMsg.ServerCode = gGameServerCode;
	pMsg.Class = gObj[aIndex].Class;
	pMsg.Level = gObj[aIndex].Level;
#if (__GENS_SYSTEM__ == 1)
	pMsg.Gens = gObj[aIndex].m_GensInfo.btFamily;
#else
	pMsg.Gens = 0;
#endif

	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));
}

void CGReqPartyMatchWaitingList(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

//	if( !gObj[aIndex].m_bIsPartyWaiting )
//	{
//		return;
//	}

	PMSG_PARTYMATCHING_WAIT_REQ pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEF,0x03,sizeof(pMsg));

	pMsg.aIndex = aIndex;
	memcpy(pMsg.szName,gObj[aIndex].Name,sizeof(pMsg.szName)-1);
	pMsg.szName[sizeof(pMsg.szName)-1] = '\0';

	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));
}

void CGReqPartyMatchCancel(PMSG_PARTYMATCH_CANCEL_REQ* lpMsg,int aIndex)
{
	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	PMSG_PARTYMATCHING_CANCEL_REQ pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEF,0x06,sizeof(pMsg));

	pMsg.aIndex = aIndex;
	pMsg.btType = lpMsg->btType;
	memcpy(pMsg.szName,gObj[aIndex].Name,sizeof(pMsg.szName)-1);
	pMsg.szName[sizeof(pMsg.szName)-1] = '\0';

	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));
}

void DGAnsPartyMatchReg(PMSG_PARTYMATCHING_REG_ANS* lpMsg)
{
	int aIndex = lpMsg->aIndex;

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	if( strcmp(gObj[aIndex].Name,lpMsg->szName) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	switch(lpMsg->iResult)
	{
	case 0:
		break;
	default:
		gObj[aIndex].m_bIsPartyMatch = FALSE;
		break;
	}

	PMSG_PARTYMATCH_REG_ANS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEF,0x00,sizeof(pMsg));
	pMsg.iResult = lpMsg->iResult;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}


void DGAnsPartyMatchInfo(LPBYTE lpMsg)
{

	if( lpMsg == NULL )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	LPMSG_PARTYMATCHING_INFO_ANS lpMsgCount = (LPMSG_PARTYMATCHING_INFO_ANS)&lpMsg[0];
	PARTYMATCH_INFO* lpMsgBody = (PARTYMATCH_INFO*)&lpMsg[sizeof(PMSG_PARTYMATCHING_INFO_ANS)];

	int aIndex = lpMsgCount->aIndex;

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	if( strcmp(gObj[aIndex].Name,lpMsgCount->szName) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	BYTE cBUFFER[1024];
	int lOfs = sizeof(PMSG_PARTYMATCH_COUNT);

	PMSG_PARTYMATCH_COUNT* pMsgCount = (PMSG_PARTYMATCH_COUNT*)&cBUFFER[0];
	PMSG_PARTYMATCH_INFO* pMsgBody = (PMSG_PARTYMATCH_INFO*)&cBUFFER[lOfs];

	PHeadSubSetW((LPBYTE)pMsgCount,0xEF,0x01,lOfs);

	pMsgCount->count = lpMsgCount->count;
	pMsgCount->curpage = lpMsgCount->page;
	pMsgCount->totalpage = lpMsgCount->totalpage;
	pMsgCount->result = lpMsgCount->iResult;

	if( pMsgCount->result == 0 )
	{
		for(int i = 0; i < pMsgCount->count; i++)
		{
			memcpy(pMsgBody[i].szName,lpMsgBody[i].szLeader,sizeof(pMsgBody[i].szName)-1);
			pMsgBody[i].szName[sizeof(pMsgBody[i].szName)-1] = '\0';

			memcpy(pMsgBody[i].szDescription,lpMsgBody[i].szDescription,sizeof(pMsgBody[i].szDescription)-1);
			pMsgBody[i].szDescription[sizeof(pMsgBody[i].szDescription)-1] = '\0';

			pMsgBody[i].MinLevel = lpMsgBody[i].MinLevel;
			pMsgBody[i].MaxLevel = lpMsgBody[i].MaxLevel;

			pMsgBody[i].MoveReqId = lpMsgBody[i].MoveReqId; // movereqid
			pMsgBody[i].LeaderLevel = lpMsgBody[i].LeaderLevel; // leaderlevel
			pMsgBody[i].Channel = lpMsgBody[i].LeaderChannel % 20 + 1;	// channel
			pMsgBody[i].Gens = lpMsgBody[i].Gens;
			pMsgBody[i].PartyCount = lpMsgBody[i].btPartyCount; // party members

			pMsgBody[i].LeaderClass = lpMsgBody[i].LeaderClass/16;
			pMsgBody[i].Class = lpMsgBody[i].btClass;
			pMsgBody[i].IsProtected = lpMsgBody[i].btIsPassword;

			lOfs += sizeof(PMSG_PARTYMATCH_INFO);
		}

		pMsgCount->h.sizeH = SET_NUMBERH(lOfs);
		pMsgCount->h.sizeL = SET_NUMBERL(lOfs);
	}

	DataSend(aIndex,(LPBYTE)&cBUFFER[0],lOfs);
}

void DGAnsPartyMatchJoin(PMSG_PARTYMATCHING_JOIN_ANS* lpMsg)
{
	int aIndex = lpMsg->aIndex;

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	PMSG_PARTYMATCH_JOIN_ANS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEF,0x02,sizeof(pMsg));

	pMsg.iResult = lpMsg->iResult;

	switch(pMsg.iResult)
	{
	case 0:
		{
			for( int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++ )
			{
				if( gObj[i].Name != NULL && !strcmp(lpMsg->szName, gObj[i].Name) )
				{
					if( gObj[i].Connected == PLAYER_PLAYING )
					{
						PARTYMATCH_GCSET_JOINNOTIFICATION lpNotification = { 0 };
						lpNotification.h.c			= 0xC1;
						lpNotification.h.headcode	= 0xEF;
						lpNotification.h.subcode	= 0x8;
						lpNotification.h.size		= sizeof(PARTYMATCH_GCSET_JOINNOTIFICATION);
						DataSend(i, (LPBYTE)&lpNotification, sizeof(PARTYMATCH_GCSET_JOINNOTIFICATION));
						break;
					}
				}
			}
		}
		break;
//default:
//		gObj[aIndex].m_bIsPartyWaiting = FALSE;
//		break;
	}

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void DGAnsPartyMatchWaitlist(PMSG_PARTYMATCHING_WAIT_ANS* lpMsg)
{
	int aIndex = lpMsg->aIndex;

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	if( strcmp(gObj[aIndex].Name,lpMsg->szName) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	if( lpMsg->iResult == (int)-1 )
	{
		return;
	}

	PMSG_PARTYMATCH_WAIT_ANS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEF,0x03,sizeof(pMsg));

	pMsg.iResult = lpMsg->iResult;
	pMsg.iServerCode = lpMsg->ServerCode % 20 + 1;
	memcpy(pMsg.szPartyLeader,lpMsg->szPartyLeader,sizeof(pMsg.szPartyLeader)-1);
	pMsg.szPartyLeader[sizeof(pMsg.szPartyLeader)-1] = '\0';

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void DGAnsPartyMatchCancel(PMSG_PARTYMATCHING_CANCEL_ANS* lpMsg)
{
	int aIndex = lpMsg->aIndex;

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	if( strcmp(gObj[aIndex].Name,lpMsg->szName) )
	{
		LogAdd("error: %s %d",__FILE__,__LINE__);
		return;
	}

	switch( lpMsg->iResult )
	{
	case 0:
	case 1:
		if( lpMsg->btType == 0 )
		{
		}
		else if( lpMsg->btType == 1 )
		{
//			gObj[aIndex].m_bIsPartyWaiting = FALSE;
		}
		break;
	}

	PMSG_PARTYMATCH_CANCEL_ANS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEF,0x06,sizeof(pMsg));

	pMsg.Type = lpMsg->btType;
	pMsg.Result = lpMsg->iResult;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void CGReqPartyMatchAcceptList(int UserIndex)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	PARTYMATCH_GDREQ_REQUESTLIST lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xEF;
	lpRequest.h.subcode		= 0x04;
	lpRequest.h.size		= sizeof(PARTYMATCH_GDREQ_REQUESTLIST);
	lpRequest.UserIndex		= UserIndex;
	memcpy(lpRequest.PartyLeader, gObj[UserIndex].Name, 10);
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(PARTYMATCH_GDREQ_REQUESTLIST));
}

void DGAnsPartyMatchAcceptList(LPBYTE lpData)
{
	GUILDMATCH_DGANS_REQUESTLIST_COUNT* lpRecvCount = (GUILDMATCH_DGANS_REQUESTLIST_COUNT*)lpData;
	// ----
	if(		!OBJMAX_RANGE(lpRecvCount->UserIndex) 
		||	gObj[lpRecvCount->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GCANS_REQUESTLIST_COUNT lpAnswer = { 0 };
	lpAnswer.h.c			= 0xC2;
	lpAnswer.h.headcode		= 0xEF;
	lpAnswer.h.subcode		= 0x04;
	lpAnswer.h.sizeH		= SET_NUMBERH(sizeof(lpAnswer));
	lpAnswer.h.sizeL		= SET_NUMBERL(sizeof(lpAnswer));
	lpAnswer.RequestCount	= lpRecvCount->Count;
	// ----
	GUILDMATCH_DGANS_REQUESTLIST_INFO* lpRecvInfo = (GUILDMATCH_DGANS_REQUESTLIST_INFO*)&lpData[sizeof(GUILDMATCH_DGANS_REQUESTLIST_COUNT)];
	// ----
	for( int i = 0; i < lpAnswer.RequestCount; i++ )
	{
		int UserIndex = gObjGetIndex(lpRecvInfo[i].Character);
		// ----
		if(		!OBJMAX_RANGE(UserIndex) 
			||	gObj[UserIndex].Connected != PLAYER_PLAYING )
		{
			lpAnswer.RequestCount--;
			continue;
		}
		// ----
		memcpy(lpAnswer.m_RequestList[i].Character, lpRecvInfo[i].Character, 10);
		lpAnswer.m_RequestList[i].Class = lpRecvInfo[i].Class / 16; //wrong...
		lpAnswer.m_RequestList[i].Level = lpRecvInfo[i].Level;
	}
	// ----
	DataSend(lpRecvCount->UserIndex, (LPBYTE)&lpAnswer, sizeof(lpAnswer));
}

void CGReqPartyMatchRequestAnswer(PARTYMATCH_CGREQ_REQUESTANSWER* lpData, int UserIndex)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	PARTYMATCH_GDREQ_REQUESTANSWER lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xEF;
	lpRequest.h.subcode		= 0x05;
	lpRequest.h.size		= sizeof(PARTYMATCH_GDREQ_REQUESTANSWER);
	lpRequest.UserIndex		= UserIndex;
	lpRequest.Type			= lpData->Result;
	memcpy(lpRequest.Character, lpData->Character, 10);
	// ----
	if( lpData->Result == 1 ) //Accepted
	{
		int TargetIndex = gObjGetIndex(lpData->Character);
		// ----
		if( !OBJMAX_RANGE(TargetIndex) ||
			gObj[TargetIndex].Connected != PLAYER_PLAYING )
		{
			::GCServerMsgStringSend("Target user not in game", UserIndex, 1);
			return;
		}
		// ----
		LPOBJ lpTargetUser = &gObj[TargetIndex];
		// ----
		if( CC_MAP_RANGE(gObj[TargetIndex].MapNumber) 
			|| CC_MAP_RANGE(gObj[TargetIndex].MapNumber)  )
		{
			::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 198)), UserIndex, 1);
			return;
		}
		// ----
		if( IT_MAP_RANGE(gObj[UserIndex].MapNumber) 
			|| IT_MAP_RANGE(gObj[TargetIndex].MapNumber) )
		{
			::GCServerMsgStringSend("Party not allowed in Illusion Temple.", UserIndex, 1);
			return;
		}
		// ----
		if( DG_MAP_RANGE(gObj[UserIndex].MapNumber) 
			|| DG_MAP_RANGE(gObj[TargetIndex].MapNumber)  )
		{
			::GCServerMsgStringSend("Party not allowed in Doppelganger.", UserIndex, 1);
			return;
		}
		// ----
		if( IF_MAP_RANGE(gObj[UserIndex].MapNumber) 
			|| IF_MAP_RANGE(gObj[TargetIndex].MapNumber) )
		{
			::GCServerMsgStringSend("Party not allowed in Imperial Fort.", UserIndex, 1);
			return;
		}
		// ----
		if( gObj[UserIndex].CloseCount >= 0 || gObj[TargetIndex].CloseCount >= 0 )
		{
			::GCResultSend(UserIndex, 0x41, 0x00);
			return;
		}
		// ----
		if( gObj[TargetIndex].PartyNumber > 0 )
		{
			::GCServerMsgStringSend("Target user already in party", UserIndex, 1);
			return;
		}
		// ----
		if( gGensSystem.m_CheckForParty )
		{
			if( gObj[UserIndex].m_GensInfo.btFamily != GENS_NONE && gObj[UserIndex].m_GensInfo.btFamily != gObj[TargetIndex].m_GensInfo.btFamily )
			{
				::GCResultSend(UserIndex, 0x41, 0x07);
				return;
			}
			// ----
			if( gObj[UserIndex].m_GensInfo.btFamily != gObj[TargetIndex].m_GensInfo.btFamily )
			{
				::GCResultSend(UserIndex, 0x41, 0x07);
				return;
			}
		}
	}
	// ----
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(PARTYMATCH_GDREQ_REQUESTANSWER));
}

void DGAnsPartyMatchRequestAnswer(PARTYMATCH_DGANS_REQUESTANSWER* lpData)
{
	if(		!OBJMAX_RANGE(lpData->UserIndex) 
		||	gObj[lpData->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	if( lpData->Type == 0 )
	{
		CGReqPartyMatchAcceptList(lpData->UserIndex);
		return;
	}
	// ----
	int UserIndex	= lpData->UserIndex;
	int TargetIndex = gObjGetIndex(lpData->Character);
	// ----
	if(	!OBJMAX_RANGE(TargetIndex) ||
		gObj[TargetIndex].Connected != PLAYER_PLAYING )
	{
		::GCServerMsgStringSend("Target user not in game", UserIndex, 1);
		return;
	}
	// ----
	LPOBJ lpTargetUser = &gObj[TargetIndex];
	// ----
	int PartyNumber;
	char szTemp[256] = { 0 };
	// ----
	if( gObj[UserIndex].PartyNumber < 0 )
	{
		gObj[UserIndex].PartyNumber = gParty.Create(UserIndex, gObj[UserIndex].DBNumber, gObj[UserIndex].Level);
	}
	// ----
	if( gObj[UserIndex].PartyNumber >= 0  )
	{
		PartyNumber = gObj[UserIndex].PartyNumber;
		int iPartyPos = gParty.Add(gObj[UserIndex].PartyNumber, TargetIndex, gObj[TargetIndex].DBNumber, gObj[TargetIndex].Level);
		// ----
		if( iPartyPos >= 0 )
		{
			gObj[TargetIndex].PartyNumber = gObj[UserIndex].PartyNumber;
			wsprintf(szTemp, lMsg.Get(MSGGET(4, 83)), gObj[TargetIndex].Name);
			::GCServerMsgStringSend(szTemp, UserIndex, 1);
			wsprintf(szTemp, lMsg.Get(MSGGET(4, 83)), gObj[UserIndex].Name);
			::GCServerMsgStringSend(szTemp, TargetIndex, 1);
			gParty.Paint(PartyNumber);
		}
		else if( iPartyPos == -1 )
		{
			GCResultSend(UserIndex, 0x41, 2);
			GCResultSend(TargetIndex, 0x41, 2);
		}
		else if( iPartyPos == -2 )
		{
			wsprintf(szTemp, lMsg.Get(MSGGET(4, 84)), gObj[TargetIndex].Name);
			GCServerMsgStringSend(szTemp, TargetIndex, 1);
		}
	}
	// ----
	if( gObj[TargetIndex].m_IfState.use !=  0 && gObj[TargetIndex].m_IfState.type == 2)
	{
		gObj[TargetIndex].m_IfState.use = 0;
		gObj[TargetIndex].PartyTargetUser = -1;
	}
	// ----
	if( gObj[UserIndex].m_IfState.use !=  0 && gObj[UserIndex].m_IfState.type == 2)
	{
		gObj[UserIndex].m_IfState.use = 0;
		gObj[UserIndex].PartyTargetUser = -1;
	}
	// ----
	if( PartyNumber >= 0 )
	{
		CGPartyListAll(PartyNumber);
	}
	// ----
	PARTYMATCH_GDREQ_REQUESTANSWER lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xEF;
	lpRequest.h.subcode		= 0x05;
	lpRequest.h.size		= sizeof(PARTYMATCH_GDREQ_REQUESTANSWER);
	lpRequest.UserIndex		= UserIndex;
	lpRequest.Type			= 0; //Delete
	memcpy(lpRequest.Character, lpData->Character, 10);
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(PARTYMATCH_GDREQ_REQUESTANSWER));
	// ----	
	CGReqPartyMatchAcceptList(UserIndex);
}

void GDPartyMatchUpdateStatus(LPSTR Character, BYTE Status)
{
	PARTYMATCH_GDREQ_UPDATESTATUS lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xEF;
	lpRequest.h.subcode		= 0x07;
	lpRequest.h.size		= sizeof(PARTYMATCH_GDREQ_UPDATESTATUS);
	lpRequest.Status		= Status;
	memcpy(lpRequest.Character, Character, 10);
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(PARTYMATCH_GDREQ_UPDATESTATUS));
}