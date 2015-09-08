#include "stdafx.h"
#include "GameMain.h"
#include "GameEvent.h"
#include "logproc.h"
#include "TNotice.h"
#include "gObjMonster.h"
#include "..\common\winutil.h"
#include "..\include\readscript.h"
#include "CashShop.h"
#include "BuffEffectSlot.h"
#include "CashItemPeriodSystem.h"
#include "DSProtocol.h"



#if (__CUSTOM__ == 1)

#if (ANTI_CHEAT == 1)

BOOL gObjSpeedCheck(int uIndex,PMSG_CHECK_ANTICHEAT* lpMsg,int headcode,int size)
{
	if( headcode != 0x10 )
	{
		LogAddC(2, "[ANTI-CHEAT][%s][%s] 0xC6 wrong headcode ![%x]", gObj[uIndex].AccountID,gObj[uIndex].Name,headcode);
		return false;
	}

	if( size != sizeof(PMSG_CHECK_ANTICHEAT) )
	{
		LogAddC(2, "[ANTI-CHEAT][%s][%s] 0xC6 wrong size ![%d]/[%d]", gObj[uIndex].AccountID,gObj[uIndex].Name,size,sizeof(PMSG_CHECK_ANTICHEAT));
		return false;
	}

	WORD tmpCheck[2];

	tmpCheck[0] = (lpMsg->type==1) ? (lpMsg->wWord1^0x8AFC) : (lpMsg->wWord2^0x8AFC);
	tmpCheck[1] = (lpMsg->type==1) ? ( (((lpMsg->wWord2^0xA8CF)/1024)*16) | ((lpMsg->wWord2^0xA8CF)&0x0F)) : ( (((lpMsg->wWord1^0xA8CF)/1024)*16) | ((lpMsg->wWord1^0xA8CF)&0x0F));

	if( tmpCheck[0] != tmpCheck[1] || tmpCheck[0] == 0 || tmpCheck[1] == 0)
	{
		LogAddC(2, "[ANTI-CHEAT][%s][%s] Check codes are wrong ! [%d] / [%d]",
			gObj[uIndex].AccountID,gObj[uIndex].Name,(tmpCheck[0]&0xFFFF),(tmpCheck[1]&0xFFFF));
		return false;
	}

	if ( gObj[uIndex].Connected > PLAYER_CONNECTED )
	{
		gObj[uIndex].ConnectCheckTime = GetTickCount();

		if( gObj[uIndex].Connected == PLAYER_PLAYING )
		{
			int aSpeed1 = lpMsg->wAttackSpeed1^0xDEFA;
			int aSpeed2 = lpMsg->wAttackSpeed2^0xAFDA;

			if( aSpeed1 > gObj[uIndex].m_AttackSpeed )
			{
				int delta = aSpeed1 - gObj[uIndex].m_AttackSpeed;

				if( delta > 30 )
				{
					LogAddC(2, "[ANTI-CHEAT][%s][%s] AttackSpeed is wrong ! [%d] / [%d]", gObj[uIndex].AccountID,gObj[uIndex].Name,aSpeed1,gObj[uIndex].m_AttackSpeed);
					gObj[uIndex].m_AttackSpeedHackDetectedCount++;
					if( gObj[uIndex].m_AttackSpeedHackDetectedCount > 3 )
						return false;
				}
				else
				{
					gObj[uIndex].m_AttackSpeedHackDetectedCount = 0;
				}
			}
			else if( aSpeed2 > gObj[uIndex].m_MagicSpeed )
			{
				int delta = aSpeed2 - gObj[uIndex].m_MagicSpeed;

				if( delta > 30 )
				{
					LogAddC(2, "[ANTI-CHEAT][%s][%s] MagicSpeed is wrong ! [%d] / [%d]", gObj[uIndex].AccountID,gObj[uIndex].Name,aSpeed2,gObj[uIndex].m_MagicSpeed);
					gObj[uIndex].m_AttackSpeedHackDetectedCount++;
					if( gObj[uIndex].m_AttackSpeedHackDetectedCount > 3 )
						return false;
				}
				else
				{
					gObj[uIndex].m_AttackSpeedHackDetectedCount = 0;
				}
			}
		}
	}

	gObj[uIndex].m_iCHECK_COUNT = 0;

return true;
}

#endif

TCustomExp gCustomExp;

TCustomExp::TCustomExp()
{

}

TCustomExp::~TCustomExp()
{

}

/*------------------------------------------------------*/

void TCustomExp::LoadData(char* szFileName)
{
	SMDFile = fopen(szFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[CUSTOM EXP] Could not open[%s]",szFileName);
		return;
	}

	m_ExpMap.clear();
	m_ZenMap.clear();
	m_JewelDropMap.clear();

	int Token,Index, Map, JewelDrop;
	float Exp,Zen;

	while( TRUE )
	{	
		Token = GetToken();

		if( Token == END )
			break;

		Index = (int)TokenNumber;

		while( TRUE )
		{
			Token = GetToken();

			if( strcmp(TokenString,"end") == NULL )
				break;

			Map = (int)TokenNumber;

			if( Map < 0 || Map > MAX_NUMBER_MAP )
			{
				MsgBox("[CUSTOM EXP] Wrong MapNumber (%d)",Map);
				break;
			}

			if( Index == 0 )
			{
				Token = GetToken();
				Exp = (float)atof(TokenString);

				m_ExpMap[Map] = Exp;
			}
			else if( Index == 1 )
			{
				Token = GetToken();
				Zen = (float)atof(TokenString);

				m_ZenMap[Map] = Zen;
			}
			else if( Index == 2 )
			{
				Token = GetToken();
				JewelDrop = atol(TokenString);

				m_JewelDropMap[Map] = JewelDrop;
			}
		}
	}

	fclose(SMDFile);

	LogAdd("[CUSTOM EXP] File loaded (%s) EXP(%d) ZEN(%d) JEWEL(%d)].",
		szFileName,m_ExpMap.size(),m_ZenMap.size(),m_JewelDropMap.size());
}

void TCustomExp::LoadResetExp(char* szFileName)
{
	SMDFile = fopen(szFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[CUSTOM RESET EXP] Could not open[%s]",szFileName);
		return;
	}

	m_ResetExpMap.clear();

	int Token, ResetNumber;
	float ExpPerc;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		if( strcmp(TokenString,"end") == NULL )
			break;

		ResetNumber = (int)TokenNumber;

		Token = GetToken();
		ExpPerc = TokenNumber;

		m_ResetExpMap[ResetNumber] = ExpPerc;
	}

	fclose(SMDFile);
	LogAdd("[CUSTOM RESET EXP] File loaded (%s) %d.",szFileName, m_ResetExpMap.size());
}

float TCustomExp::GetExp(int MapNumber)
{
	if( !m_ExpMap.empty() )
	{
		std::map<int,float>::iterator _It = m_ExpMap.find(MapNumber);

		if( _It != m_ExpMap.end() )
		{
			return _It->second;
		}
	}

	return gAddExperience;
}

float TCustomExp::GetZen(int MapNumber)
{
	if( !m_ZenMap.empty() )
	{
		std::map<int,float>::iterator _It = m_ZenMap.find(MapNumber);

		if( _It != m_ZenMap.end() )
		{
			return _It->second;
		}
	}

return gAddZen;
}

int TCustomExp::GetJewelDrop(int MapNumber)
{
	if( !m_JewelDropMap.empty() )
	{
		std::map<int,int>::iterator _It = m_JewelDropMap.find(MapNumber);

		if( _It != m_JewelDropMap.end() )
		{
			return _It->second;
		}
	}

return 100;
}

__int64 TCustomExp::GetResetExp(LPOBJ lpObj,__int64 Exp)
{
	__int64 TmpExp = Exp;

	if( !m_ResetExpMap.empty() )
	{
		std::map<int,float>::iterator _It = m_ResetExpMap.find(lpObj->iResetCount);

		if( _It != m_ResetExpMap.end() )
		{
			TmpExp = __int64((TmpExp * _It->second)/100.0f);
		}
	}

	return TmpExp;
}

#if (__CUSTOM_QUEST__ == 1)

#include "GameMain.h"
#include "TNotice.h"
#include "DSProtocol.h"
#include "Event.h"
#include "..\common\winutil.h"

CMyQuest g_MyQuest;

void CMyQuest::LoadData(char* szFile)
{
	SMDFile = fopen(szFile,"r");

	if( SMDFile == NULL )
	{
		MsgBox("load error %s",szFile);
		return;
	}

	int Token,Index;
	int q_count;

	while( TRUE )
	{

		Token = GetToken();

		if( Token == END )
			break;

		Index = TokenNumber;

		if( Index < 0 || Index >= MAX_QUEST_GROUP )
		{
			MsgBox("[CUSTOM QUEST] Group Index Out of Bound - %d",Index);
			break;
		}

		QuestGroup[Index].m_Index = Index;

		while( TRUE )
		{
			MyQuest_QUEST tmpStruct;

			q_count = 0;
			
			Token = GetToken();

			if( strcmp(TokenString,"end") == NULL )
				break;

			tmpStruct.m_RewardCP = TokenNumber;

			Token = GetToken();
			tmpStruct.m_RewardItem = TokenNumber;

			Token = GetToken();
			tmpStruct.m_RewardPts = TokenNumber;

			while( TRUE )
			{
				Token = GetToken();

				if( strcmp(TokenString,"end") == NULL )
					break;

				if( q_count < 0 || q_count >= MAX_QUEST_ITEM )
				{
					break;
				}

				tmpStruct.m_Item[q_count].m_IIndex = q_count;
				strcpy(tmpStruct.m_Item[q_count].m_szItemName,TokenString);

				Token = GetToken();
				tmpStruct.m_Item[q_count].m_ICount = TokenNumber;

				Token = GetToken();
				tmpStruct.m_Item[q_count].m_MIndex = TokenNumber;

				Token = GetToken();
				tmpStruct.m_Item[q_count].m_MDropRate = TokenNumber;

				Token = GetToken();
				strcpy(tmpStruct.m_Item[q_count].m_szQuestInfo,TokenString);
				q_count++;
			}

			QuestGroup[Index].m_Quest.push_back( tmpStruct );
			QuestGroup[Index].m_QuestCount++;
		}
	}

	fclose(SMDFile);

	m_bLoaded = TRUE;
}


void CMyQuest::NpcTalk(LPOBJ lpObj,LPOBJ lpNpc)
{
	if( !gObjIsConnected(lpObj->m_Index) )
	{
		return;
	}

	if( lpObj->MapNumber != lpNpc->MapNumber )
	{
		return;
	}

	if( gObjCalDistance(lpObj,lpNpc) > 3 )
	{
		return;
	}

	int i;
	MyQuest_USER* lpQuestUser = GetUser(lpObj->m_Index);

	if( lpQuestUser == NULL || lpQuestUser->QuestLoaded == FALSE )
	{
		return;
	}

	if( lpQuestUser->QuestState == TRUE )
	{
		MyQuest_QUEST* lpQuest = FindQuest(lpQuestUser->QuestGroup,lpQuestUser->QuestIndex);

		if( lpQuest == NULL )
		{
			TCustomNotice::SendNotice(lpObj->m_Index,3,"No quest have been found");
			lpQuestUser->QuestState = FALSE;
			return;
		}

		if( CheckCondition(lpQuestUser,lpQuest) == FALSE )
		{
			for(i = 0; i < MAX_QUEST_ITEM; i++)
			{
				if( lpQuest->m_Item[i].m_IIndex == (BYTE)-1 )
					continue;

				TCustomNotice::SendNotice(lpObj->m_Index,3,"Find [ %d/%d ]-[%s] %s",
					lpQuestUser->QuestItem[i],
					lpQuest->m_Item[i].m_ICount,
					lpQuest->m_Item[i].m_szItemName,
					lpQuest->m_Item[i].m_szQuestInfo);
			}
			TCustomNotice::SendNotice(lpObj->m_Index,3,"Come back when you find all items!");
		}
		else
		{
			GiveReward(lpQuest,lpObj);
			TCustomNotice::SendNotice(lpObj->m_Index,3,"Quest finished!");
			lpQuestUser->QuestState = FALSE;
		}
	}
	else
	{
		int bState = StartQuest(lpQuestUser,lpObj->m_Index);

		if( bState == FALSE )
		{
			TCustomNotice::SendNotice(lpObj->m_Index,3,"You have completed all available quests");
		}
		else
		{
			TCustomNotice::SendNotice(lpObj->m_Index,3,"Quest started !");
		}
	}
}


BOOL CMyQuest::CheckCondition(MyQuest_USER* lpQuestUser,MyQuest_QUEST* lpQuest)
{
	if( lpQuestUser == NULL ||
		lpQuest == NULL )
	{
		return false;
	}

	for(int i = 0; i < MAX_QUEST_ITEM; i++)
	{

		if( lpQuest->m_Item[i].m_IIndex != (BYTE)-1 && 
			lpQuestUser->QuestItem[i] < lpQuest->m_Item[i].m_ICount )
		{
			return FALSE;
		}
	}
return TRUE;
}

BOOL CMyQuest::StartQuest(MyQuest_USER* lpQuestUser,int aIndex)
{
	MyQuest_QUEST* lpQuest = NULL;
	int _QuestGroup = lpQuestUser->QuestGroup;
	int _QuestIndex = lpQuestUser->QuestIndex;

	if( _QuestGroup == MAX_QUEST_GROUP )
	{
		return FALSE;
	}

	if( _QuestGroup < 0)
	{
		_QuestGroup = 0;
	}

	if( _QuestIndex < 0 || _QuestIndex >= QuestGroup[_QuestGroup].m_QuestCount )
	{
		_QuestIndex = 0;
	}
	else
	{
		_QuestIndex++;
	}

	int bFound = FALSE;

	while( TRUE )
	{
		lpQuest = FindQuest(_QuestGroup,_QuestIndex);

		if( lpQuest == NULL )
		{
			_QuestIndex = 0;
			_QuestGroup++;

			if( _QuestGroup >= MAX_QUEST_GROUP )
			{
				break;
			}
		}
		else
		{
			bFound = TRUE;
			break;
		}
	}

	lpQuestUser->QuestGroup = _QuestGroup;
	lpQuestUser->QuestIndex = _QuestIndex;

	lpQuestUser->QuestItem[0] = 0;
	lpQuestUser->QuestItem[1] = 0;
	lpQuestUser->QuestItem[2] = 0;

	if( bFound == FALSE )
	{
		return FALSE;
	}

	for(int i = 0; i < MAX_QUEST_ITEM; i++)
	{
		if( lpQuest->m_Item[i].m_IIndex == (BYTE)-1 )
				continue;

		TCustomNotice::SendNotice(aIndex,3,"Find [ 0/%d ]-[%s] %s",
			lpQuest->m_Item[i].m_ICount,
			lpQuest->m_Item[i].m_szItemName,
			lpQuest->m_Item[i].m_szQuestInfo);
	}

	lpQuestUser->QuestState = TRUE;

return TRUE;
}

MyQuest_QUEST* CMyQuest::FindQuest(int Group,int Index)
{
	if( Group < 0 || Group >= MAX_QUEST_GROUP )
	{
		return NULL;
	}

	if( QuestGroup[Group].m_Index == -1 )
	{
		return NULL;
	}

	Quest_Info::iterator _It = &QuestGroup[Group].m_Quest[Index];

	if( _It == QuestGroup[Group].m_Quest.end() )
	{
		return NULL;
	}

return _It;
}

void CMyQuest::GiveReward(MyQuest_QUEST* lpQuest,LPOBJ lpObj)
{
	if( lpQuest->m_RewardCP != -1 )
	{
		PMSG_ADD_CP pMsg;
		PHeadSubSetB((LPBYTE)&pMsg,0x65,10,sizeof(pMsg));

		pMsg.DBNumber = lpObj->DBNumber;
		pMsg.iPoints = lpQuest->m_RewardCP;
		memcpy(pMsg.Name,lpObj->Name,10);
		pMsg.Name[10] = '\0';

		wsEvenChipServerCli.DataSend((PCHAR)&pMsg,sizeof(pMsg));
	}

	if( lpQuest->m_RewardItem != -1 )
	{
		ItemSerialCreateSend(lpObj->m_Index,lpObj->MapNumber,lpObj->X,lpObj->Y,lpQuest->m_RewardItem,0,0,0,0,0,lpObj->m_Index,0,0);
	}

	if( lpQuest->m_RewardPts != -1 )
	{
		lpObj->LevelUpPoint += lpQuest->m_RewardPts;
		GCLevelUpMsgSend(lpObj->m_Index,1);
	}

	SendFirework(lpObj);
}

void CMyQuest::MonsterDie(LPOBJ lpObj,LPOBJ lpNpc)
{
	if( !gObjIsConnected(lpObj->m_Index) )
	{
		return;
	}

	MyQuest_USER* lpQuestUser = GetUser(lpObj->m_Index);

	if( lpQuestUser == NULL )
	{
		return;
	}

	if( lpQuestUser->QuestState == FALSE )
	{
		return;
	}

	MyQuest_QUEST* lpQuest = FindQuest(lpQuestUser->QuestGroup,lpQuestUser->QuestIndex);

	if( lpQuest == NULL )
	{
		return;
	}

	for(int i = 0; i < MAX_QUEST_ITEM; i++)
	{
		if( lpQuest->m_Item[i].m_IIndex == (BYTE)-1 )
			continue;

		if( lpQuest->m_Item[i].m_MIndex != lpNpc->Class )
			continue;

		if( lpQuestUser->QuestItem[i] < lpQuest->m_Item[i].m_ICount )
		{
			if( rand()%100 < lpQuest->m_Item[i].m_MDropRate )
			{
				lpQuestUser->QuestItem[i]++;

				TCustomNotice::SendNotice(lpObj->m_Index,3,"Quest updated - [%s] [%d/%d]",
					lpQuest->m_Item[i].m_szItemName,
					lpQuestUser->QuestItem[i],
					lpQuest->m_Item[i].m_ICount);
				break;
			}
		}
	}
}

// User System

MyQuest_USER* CMyQuest::GetUser(int UserIndex)
{
	if( UserIndex < OBJ_STARTUSERINDEX || UserIndex >= OBJMAX )
	{
		return false;
	}
return &QuestUser[UserIndex-OBJ_STARTUSERINDEX];
}

void CMyQuest::InitUser(int UserIndex)
{
	MyQuest_USER* lpQuestUser = GetUser(UserIndex);

	if( lpQuestUser == NULL )
	{
		return;
	}

	lpQuestUser->QuestLoaded = FALSE;
	lpQuestUser->QuestGroup = -1;
	lpQuestUser->QuestIndex = -1;
	for(int i = 0; i < MAX_QUEST_ITEM; i++)
	{
		lpQuestUser->QuestItem[i] = 0;
	}
}

struct PMSG_CUSTOMQUEST_SAVEUSER
{
	PBMSG_HEAD2	h;
	char szUserName[10];
	short QuestGroup;
	short QuestIndex;
	short QuestState;
	short QuestItem[3];
};

void CMyQuest::SaveUser(int UserIndex)
{
	MyQuest_USER* lpQuestUser = GetUser(UserIndex);

	if( lpQuestUser == NULL || lpQuestUser->QuestLoaded == FALSE)
	{
		return;
	}

	PMSG_CUSTOMQUEST_SAVEUSER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x65;
	pMsg.h.subcode = 0x02;

	memcpy(pMsg.szUserName,gObj[UserIndex].Name,10);
	pMsg.QuestGroup = lpQuestUser->QuestGroup;
	pMsg.QuestIndex = lpQuestUser->QuestIndex;
	pMsg.QuestState = lpQuestUser->QuestState;

	for(int i = 0; i < MAX_QUEST_ITEM;i++)
	{
		pMsg.QuestItem[i] = lpQuestUser->QuestItem[i];
	}

	wsEvenChipServerCli.DataSend((PCHAR)&pMsg,sizeof(pMsg));

	LogAddTD("[CUSTOM QUEST] Save Info [%s][%s]",gObj[UserIndex].AccountID,gObj[UserIndex].Name);
}

struct PMSG_CUSTOMQUEST_REQUSER
{
	PBMSG_HEAD2 h;
	char szUserName[10];
	int aIndex;
};

void CMyQuest::ReqUserInfo(int UserIndex)
{
	MyQuest_USER* lpQuestUser = GetUser(UserIndex);

	if( lpQuestUser == NULL || lpQuestUser->QuestLoaded != FALSE)
	{
		return;
	}

	PMSG_CUSTOMQUEST_REQUSER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x65;
	pMsg.h.subcode = 0x00;

	memcpy(pMsg.szUserName,gObj[UserIndex].Name,10);
	pMsg.aIndex = UserIndex;

	wsEvenChipServerCli.DataSend((PCHAR)&pMsg,sizeof(pMsg));

	LogAddTD("[CUSTOM QUEST] Request Info [%s][%s]",gObj[UserIndex].AccountID,gObj[UserIndex].Name);
}

struct PMSG_CUSTOMQUEST_ANSUSER
{
	PBMSG_HEAD2	h;
	BYTE result;
	char szUserName[10];
	int aIndex;
	short QuestGroup;
	short QuestIndex;
	short QuestState;
	short QuestItem[3];
};

void CMyQuest::AnsUserInfo(LPBYTE aRecv)
{
	PMSG_CUSTOMQUEST_ANSUSER* lpMsg = (PMSG_CUSTOMQUEST_ANSUSER*)aRecv;

	if( lpMsg == NULL )
	{
		return;
	}

	if( lpMsg->result != TRUE )
	{
		return;
	}

	if( !gObjIsConnected(lpMsg->aIndex) )
	{
		return;
	}

	int aIndex = lpMsg->aIndex;

	MyQuest_USER* lpQuestUser = GetUser(aIndex);

	if( lpQuestUser == NULL )
	{
		return;
	}

	lpQuestUser->QuestGroup = lpMsg->QuestGroup;
	lpQuestUser->QuestIndex = lpMsg->QuestIndex;
	lpQuestUser->QuestState = lpMsg->QuestState;

	for(int i = 0; i < MAX_QUEST_ITEM; i++)
	{
		lpQuestUser->QuestItem[i] = lpMsg->QuestItem[i];
	}

	lpQuestUser->QuestLoaded = TRUE;

	LogAddTD("[CUSTOM QUEST] Info Loaded [%s][%s] Group:[%d],Index:[%d] - Item1:[%d], Item2[%d], Item3[%d]",
		gObj[aIndex].AccountID,gObj[aIndex].Name,lpMsg->QuestGroup,lpMsg->QuestIndex,
		lpMsg->QuestItem[0],lpMsg->QuestItem[1],lpMsg->QuestItem[2]);
}

#endif


#if (CUSTOM_PETS == 1)

// global variables...

TCustomPets	gCustomPets;

// Constructor / Destructor of the class

TCustomPets::TCustomPets()
{
	m_i64Exp[0]=0;
	int LevelOver_N=1;

	for (int n=1;n<CUSTOM_PET_MAXLEVEL+1;n++)
	{
		m_i64Exp[n] = (((n+9)*n)*n)*10;

		if ( n > 255 )
		{
			m_i64Exp[n] += ((((LevelOver_N+9)*LevelOver_N)*LevelOver_N)*1000);
			LevelOver_N++;
		}
	}
}

TCustomPets::~TCustomPets()
{

}

// member functions

void TCustomPets::Load(char* lpszFileName)
{
	SMDFile = fopen(lpszFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[CUSTOM PETS] Failed to load - %s!",lpszFileName);
		return;
	}

	m_PetInfo.clear();

	int Index,Token;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		Index = TokenNumber;

		while( TRUE )
		{
			if( Index == 0 )
			{
				Token = GetToken();

				if( !strcmp("end",TokenString) )
					break;

				m_ExpPerc = TokenNumber;
			}
			else if( Index == 1 )
			{
				Token = GetToken();

				if( !strcmp("end",TokenString) )
					break;

				strcpy(szCommand,TokenString);

				Token = GetToken();
				strcpy(szInfoCommand,TokenString);
			}
			else if( Index == 2 )
			{
				Token = GetToken();

				if( !strcmp("end",TokenString) )
					break;

				char szMobSummon[32] = {0};
				_CUSTOM_PET_INFO pTmp = {0};

				strcpy(szMobSummon,TokenString);

				Token = GetToken();
				pTmp.monId = TokenNumber;

				Token = GetToken();
				pTmp.reqLevel = TokenNumber;

				Token = GetToken();
				pTmp.reqItem_Index = TokenNumber;

				Token = GetToken();
				pTmp.reqItem_Number = TokenNumber;

				Token = GetToken();
				pTmp.reqItem_Count = TokenNumber;

				Token = GetToken();
				pTmp.MaxLife = TokenNumber;

				Token = GetToken();
				pTmp.DmgMin = TokenNumber;

				Token = GetToken();
				pTmp.DmgMax = TokenNumber;

				Token = GetToken();
				pTmp.AttRate = TokenNumber;

				Token = GetToken();
				pTmp.AttSpeed = TokenNumber;

				Token = GetToken();
				pTmp.Defense = TokenNumber;

				Token = GetToken();
				pTmp.Defrate = TokenNumber;

				m_PetInfo.insert( std::pair<std::string,_CUSTOM_PET_INFO>( szMobSummon, pTmp) );
			}
		}
	}

	fclose(SMDFile);

	cCustomManager.cCommand.Add(szCommand,8,1|GM_VALUE|MM_VALUE);
	cCustomManager.cCommand.Add(szInfoCommand,9,1|GM_VALUE|MM_VALUE);

	LogAddTD("[CUSTOM PETS] Loaded - %d pets !",m_PetInfo.size());
}

void TCustomPets::SummonRequest(int aIndex,char* szSummonId)
{
	if( szSummonId == NULL )
		return;

	if( m_PetInfo.size() == 0 )
		return;

	std::map<std::string,_CUSTOM_PET_INFO>::iterator _It = m_PetInfo.find( (std::string)szSummonId );

	if( _It == m_PetInfo.end() )
	{
		MsgOutput(aIndex,"Pet not found!");
		return;
	}

	_CUSTOM_PET_INFO* pPetInfo = &_It->second;

	if( aIndex < OBJ_STARTUSERINDEX || aIndex >= OBJMAX )
		return;

	LPOBJ lpObj = &gObj[aIndex];
	_CUSTOM_PET_USERINFO* lpPetInfo = &m_UserInfo[aIndex-OBJ_STARTUSERINDEX];

	if( !lpPetInfo->m_bLoaded )
	{
		MsgOutput(aIndex,"Pet info not loaded, please contact administrator!");
		return;
	}

	if( lpPetInfo->m_Level < pPetInfo->reqLevel )
	{
		MsgOutput(aIndex,"You dont have enogh experience to summon this pet!");
		return;
	}

	if( pPetInfo->reqItem_Index != -1 && pPetInfo->reqItem_Number != -1 )
	{
		int count = gObjGetItemCountInIventory(aIndex,pPetInfo->reqItem_Index,pPetInfo->reqItem_Number,-1);

		if( count < pPetInfo->reqItem_Count )
		{
			MsgOutput(aIndex,"You dont have enough items to summon pet!");
			return;
		}
	}

	if( lpObj->m_RecallMon >= 0 )
	{
		gObjMonsterCallKill(aIndex);
	}

	gObjMonsterCall(aIndex,pPetInfo->monId,lpObj->X,lpObj->Y);

	if( lpObj->m_RecallMon >= 0 )
	{
		if( pPetInfo->reqItem_Index != -1 && pPetInfo->reqItem_Number != -1 )
			gObjDelteItemCountInInventory(lpObj->m_Index,pPetInfo->reqItem_Index,pPetInfo->reqItem_Number,-1,pPetInfo->reqItem_Count);

		int CallMonIndex = lpObj->m_RecallMon;

		gObj[CallMonIndex].MaxLife = pPetInfo->MaxLife;
		gObj[CallMonIndex].Life = pPetInfo->MaxLife;
		gObj[CallMonIndex].m_AttackDamageMin = pPetInfo->DmgMin;
		gObj[CallMonIndex].m_AttackDamageMax = pPetInfo->DmgMax;
		gObj[CallMonIndex].m_AttackSpeed = pPetInfo->AttSpeed;
		gObj[CallMonIndex].m_AttackRating = pPetInfo->AttRate;
		gObj[CallMonIndex].m_Defense = pPetInfo->Defense;
		gObj[CallMonIndex].m_SuccessfulBlocking = pPetInfo->Defrate;

		lpPetInfo->m_bSummoned = TRUE;

	}
}

void TCustomPets::RequestSummonInfo(int aIndex,char* szName)
{
	PBMSG_REQSUMMONINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0x66,0,sizeof(pMsg));

	pMsg.aIndex = aIndex;
	memcpy(pMsg.szName,szName,10);

	wsEvenChipServerCli.DataSend((PCHAR)&pMsg,sizeof(pMsg));
}

void TCustomPets::SummoInfoAnswer(LPBMSG_ANSSUMMONINFO lpMsg)
{
	if( lpMsg == NULL )
		return;

	if( !gObjIsConnected(lpMsg->aIndex) )
	{
		LogAddC(2,"[CUSTOM PETS] Pet info load fail ! User not connected (%d, %s)",
			lpMsg->aIndex,lpMsg->szName);
		return;
	}

	if( lpMsg->aIndex < OBJ_STARTUSERINDEX || lpMsg->aIndex >= OBJMAX )
	{
		LogAddC(2,"[CUSTOM PETS] Pet info load fail ! Not OBJ_USER (%d, %s)",
			lpMsg->aIndex,lpMsg->szName);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->aIndex];

	if( strcmp(lpObj->Name,lpMsg->szName) )
	{
		LogAddC(2,"[CUSTOM PETS] Pet info load fail ! Wrong index ! (%d, [%s / %s]",
			lpMsg->aIndex,lpMsg->szName,lpObj->Name);
		return;
	}

	_CUSTOM_PET_USERINFO* lpUserInfo = &m_UserInfo[lpMsg->aIndex-OBJ_STARTUSERINDEX];
	lpUserInfo->m_bLoaded = TRUE;
	lpUserInfo->m_Level = lpMsg->Level;
	lpUserInfo->m_Exp = lpMsg->Exp;

	LogAddC(3,"[CUSTOM PETS] User [%s] info loaded pet level: [%d] Exp:[%I64d]",
		lpObj->Name,lpMsg->Level,lpMsg->Exp);
}

void TCustomPets::SaveSummonInfo(int aIndex,char* szName)
{
	if( aIndex < OBJ_STARTUSERINDEX || aIndex >= OBJMAX )
	{
		LogAddC(2,"[CUSTOM PETS] Failed to save info, Index not on range! (%d,%s)",
			aIndex,szName);
		return;
	}

	_CUSTOM_PET_USERINFO* lpUserInfo = &m_UserInfo[aIndex-OBJ_STARTUSERINDEX];

	PBMSG_REQSAVESUMMON pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0x66,1,sizeof(pMsg));

	strcpy(pMsg.szName,szName);

	pMsg.level = lpUserInfo->m_Level;
	pMsg.exp = lpUserInfo->m_Exp;

	wsEvenChipServerCli.DataSend((PCHAR)&pMsg,sizeof(pMsg));
}

void TCustomPets::gObjLevelUp(int aIndex,__int64 exp)
{
	if( aIndex < OBJ_STARTUSERINDEX || aIndex >= OBJMAX )
		return;

	_CUSTOM_PET_USERINFO* lpUserInfo = &m_UserInfo[aIndex-OBJ_STARTUSERINDEX];

	if( !lpUserInfo->m_bLoaded || !lpUserInfo->m_bSummoned || gObj[aIndex].m_RecallMon < 0 )
		return;

	if( lpUserInfo->m_Level >= CUSTOM_PET_MAXLEVEL )
		return;

	lpUserInfo->m_Exp += exp * m_ExpPerc;

	if( lpUserInfo->m_Exp >= m_i64Exp[lpUserInfo->m_Level+1] )
	{
		lpUserInfo->m_Exp = m_i64Exp[lpUserInfo->m_Level+1];
		lpUserInfo->m_Level++;

		GCSendEffectInfo(gObj[aIndex].m_RecallMon, 0x10);
		MsgOutput(aIndex,"Your pet level is now [%d]",lpUserInfo->m_Level);
	}
}

void TCustomPets::SetSummon(int aIndex,BOOL bSummon)
{
	if( aIndex < OBJ_STARTUSERINDEX || aIndex >= OBJMAX )
		return;

	m_UserInfo[aIndex-OBJ_STARTUSERINDEX].m_bSummoned = bSummon;
}

void TCustomPets::SummonInfo(int aIndex)
{
	if( aIndex < OBJ_STARTUSERINDEX || aIndex >= OBJMAX )
		return;

	_CUSTOM_PET_USERINFO* lpUserInfo = &m_UserInfo[aIndex-OBJ_STARTUSERINDEX];

	if( !lpUserInfo->m_bLoaded )
	{
		MsgOutput(aIndex,"Your pet info is not loaded!");
		return;
	}

	MsgOutput(aIndex,"PetLevel: [%d] | Exp:[%I64d]",
		lpUserInfo->m_Level,lpUserInfo->m_Exp);
}

#endif // END CUSTOM PETS


#if (CUSTOM_OLYMP==1)

#include "DuelSystem.h"
#include "MapServerManager.h"

CCustomOlymp g_CustomOlymp;

unsigned char CCustomOlymp::ScheduleStateCompFunc(_OLYMP_SCHEDULE_DATA & A,_OLYMP_SCHEDULE_DATA & B)
{
	if(A.m_iSTATE > B.m_iSTATE)
	{
		return false;
	}
return true;
}

CCustomOlymp::CCustomOlymp()
{
	InitializeCriticalSection(&m_critScheduleData);
}

CCustomOlymp::~CCustomOlymp()
{
	DeleteCriticalSection(&m_critScheduleData);
}

void CCustomOlymp::Clear()
{
	m_bOlympEnable = FALSE;
	m_bFileDataLoadOK = FALSE;
	m_bDbDataLoadOK = FALSE;
	m_iOlympCycle = 7;
	m_iOlympState = -1;
	m_bFixOlympCycleStartDate = FALSE;
	m_iFIGHT_NOTIFY_COUNT = 0;

	m_iFSTART_HOUR = -1;
	m_iFSTART_MIN = -1;

	m_iFEND_HOUR = -1;
	m_iFEND_MIN = -1;

	for(int i = 0 ; i < MAX_OLYMP_RANK; i++)	{
		m_RewardPts[i] = 0;
	}

	memset(&m_tmFixOlympCycleStartDate,0x00,sizeof(m_tmFixOlympCycleStartDate));

	EnterCriticalSection(&m_critScheduleData);
	m_vtScheduleData.clear();
	LeaveCriticalSection(&m_critScheduleData);
}

void CCustomOlymp::Reset()
{
	for(int i = 0; i < MAX_OLYMP_USER; i++)
	{
		m_UserData[i].m_Index = -1;
		m_UserData[i].m_btROOM = -1;
	}

	m_UserCount = 0;
}

void CCustomOlymp::GetNextDay(_SYSTEMTIME * st,int iAfterDay,int iAfterHour,int iAfterMin,int iAfterSec)
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

#define MACRO1(value) (( value & 0xFF) & 0xFF)
#define MACRO2(value) (( value & 0xFFFF ) & 0xFFFF)

#define MONTH_MACRO(value) ( (( (value)   )<0)?FALSE:(( (value)  )>12)?FALSE:TRUE  )
#define DAY_MACRO(value) ( (( (value - 1)   )<0)?FALSE:(( (value - 1)  )>30)?FALSE:TRUE  )
#define HOUR_MACRO(value) ( (( (value)   )<0)?FALSE:(( (value)  )>23)?FALSE:TRUE  )
#define MINUTE_MACRO(value) ( (( (value)   )<0)?FALSE:(( (value)  )>59)?FALSE:TRUE  )

BOOL CCustomOlymp::Ready(int iMapSvrGroup)
{
	if( iMapSvrGroup < 0 )
		return false;

	Clear();

	m_iMapSvrGroup = iMapSvrGroup;
	SetDataLoadState(1);
return true;
}

void CCustomOlymp::SetDataLoadState(int iDataLoadState)
{
	m_iOlympDataLoadState = iDataLoadState;
}

int CCustomOlymp::GetDataLoadState()
{
	return m_iOlympDataLoadState;
}

BOOL CCustomOlymp::LoadPreFixData(char* lpszFileName)
{
	_SYSTEMTIME tmToDay;
	GetLocalTime(&tmToDay);

	int iTODAY_DATE_NUM = MACRO2(( MACRO1(tmToDay.wDay) | MACRO1(tmToDay.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmToDay.wYear) << 16;
	__int64 iTODAY_DATE_NUM64 = (MACRO2(tmToDay.wMinute) | MACRO2(tmToDay.wHour) << 16) | ((__int64)(MACRO2(( MACRO1(tmToDay.wDay) | MACRO1(tmToDay.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmToDay.wYear) << 16) << 0x20);

	m_bOlympEnable = GetPrivateProfileInt("GameServerInfo","OlympRun",1,lpszFileName);

	m_tmFixOlympCycleStartDate.wYear = GetPrivateProfileInt("GameServerInfo","OlympCycleStartYear",0,lpszFileName);
	m_tmFixOlympCycleStartDate.wMonth = GetPrivateProfileInt("GameServerInfo","OlympCycleStartMonth",0,lpszFileName);
	m_tmFixOlympCycleStartDate.wDay = GetPrivateProfileInt("GameServerInfo","OlympCycleStartDay",0,lpszFileName);

	m_iMinResetCount = GetPrivateProfileInt("GameServerInfo","OlympMinResetCount",0,lpszFileName);

	if(m_tmFixOlympCycleStartDate.wYear == 0
		|| m_tmFixOlympCycleStartDate.wMonth == 0
		|| m_tmFixOlympCycleStartDate.wDay == 0)
	{
		m_bFixOlympCycleStartDate = 0;
	}
	else
	{
		_SYSTEMTIME tmEndDate = m_tmFixOlympCycleStartDate;
		GetNextDay(&tmEndDate,m_iOlympCycle,0,0,0);

		int iEVENT_END_DATE_NUM = MACRO2(( MACRO1(tmEndDate.wDay) | MACRO1(tmEndDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmEndDate.wYear) << 16;

		if(iEVENT_END_DATE_NUM <= iTODAY_DATE_NUM)
		{
			m_bFixOlympCycleStartDate = 0;
			MsgBox("[CustomOlymp] CCustomOlymp::LoadPreFixData() - iEVENT_END_DATE_NUM (%04d-%02d-%02d) <= iTODAY_DATE_NUM (%04d-%02d-%02d)",
				tmEndDate.wYear,tmEndDate.wMonth,tmEndDate.wDay,tmToDay.wYear,tmToDay.wMonth,tmToDay.wDay);
		}
		else
		{
			m_bFixOlympCycleStartDate = 1;
		}

		if(MONTH_MACRO(m_tmFixOlympCycleStartDate.wMonth) == FALSE
		|| DAY_MACRO(m_tmFixOlympCycleStartDate.wDay) == FALSE)
		{
			m_bFixOlympCycleStartDate = 0;
		}
	}

	WritePrivateProfileString("GameServerInfo","OlympCycleStartYear","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","OlympCycleStartMonth","0",lpszFileName);
	WritePrivateProfileString("GameServerInfo","OlympCycleStartDay","0",lpszFileName);

return m_bFixOlympCycleStartDate;
}

BOOL CCustomOlymp::LoadData(char* lpszFileName)
{
using namespace std;
	SMDFile = fopen(lpszFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[CustomOlymp] file load error - File Name Error");
		return false;
	}

	if( GetDataLoadState() != 1 )
	{
		MsgBox("[CustomOlymp] GetDataLoadState() != 1");
		return false;
	}

	int type = -1;
	enum SMDToken Token;

	vector<_OLYMP_SCHEDULE_DATA> vtScheduleData;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		type = TokenNumber;

		while( TRUE )
		{
			if( type == 0 )
			{
				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				int iOlympCycle = (int)TokenNumber;

				if(iOlympCycle <= 0)
				{
					LogAddC(2,"[CustomOlymp] CCustomOlymp::LoadData() - iOlympCycle <= 0 : %d",iOlympCycle);
					return false;
				}
				else
				{
					m_iOlympCycle = iOlympCycle;
				}

				Token = GetToken();
				m_iFSTART_HOUR = TokenNumber;

				Token = GetToken();
				m_iFSTART_MIN = TokenNumber;

				Token = GetToken();
				m_iFEND_HOUR = TokenNumber;

				Token = GetToken();
				m_iFEND_MIN = TokenNumber;
			}
			else if( type == 1 )
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
					for(_OLYMP_SCHEDULE_DATA * it = vtScheduleData.begin(); it != vtScheduleData.end(); it++)
					{
						_OLYMP_SCHEDULE_DATA pScheduleData = _OLYMP_SCHEDULE_DATA(*it);

						if(pScheduleData.m_bIN_USE != 0)
						{
							if(pScheduleData.m_iSTATE == iSTATE)
							{
								LogAddC(2,"[CustomOlymp] CCustomOlymp::LoadData() - Same State Exist : %d",iSTATE);
								return false;
							}
							
							if(pScheduleData.m_iSTATE > iSTATE)
							{
								int iSTATE1_TIME = pScheduleData.m_iADD_DAY * 24 * 60 + pScheduleData.m_iADD_HOUR * 60 + pScheduleData.m_iADD_MIN;
								int iSTATE2_TIME = iSTART_DAY * 24 * 60 + iSTART_HOUR * 60 + iSTART_MIN;
	
								if(iSTATE1_TIME <= iSTATE2_TIME)
								{
									LogAddC(2,"[CustomOlymp] CCustomOlymp::LoadData() - Date Order is wrong : %d",iSTATE2_TIME);
									return false;
								}
							}
							else
							{
								int iSTATE1_TIME = pScheduleData.m_iADD_DAY * 24 * 60 + pScheduleData.m_iADD_HOUR * 60 + pScheduleData.m_iADD_MIN;
								int iSTATE2_TIME = iSTART_DAY * 24 * 60 + iSTART_HOUR * 60 + iSTART_MIN;
								if(iSTATE1_TIME >= iSTATE2_TIME)
								{
									LogAddC(2,"[CustomOlymp] CCustomOlymp::LoadData() - Date Order is wrong : %d",iSTATE2_TIME);
									return false;
								}
							}
						}
					}
				}

				_OLYMP_SCHEDULE_DATA pScheduleDataInsert;

				pScheduleDataInsert.m_bIN_USE = 1;
				pScheduleDataInsert.m_iSTATE = iSTATE;
				pScheduleDataInsert.m_iADD_DAY = iSTART_DAY;
				pScheduleDataInsert.m_iADD_HOUR = iSTART_HOUR;
				pScheduleDataInsert.m_iADD_MIN = iSTART_MIN;

				vtScheduleData.push_back(pScheduleDataInsert);
			}
			else if(type == 2)
			{
				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				int iClass = TokenNumber;

				if( iClass < 0 || iClass >= MAX_TYPE_PLAYER )
				{
					LogAddC(2,"[CustomOlymp] CCustomOlymp::LoadData() - iClass not in range : %d",iClass);
					return false;
				}

				Token = GetToken();
				m_StatData[iClass].aStr = TokenNumber;

				Token = GetToken();
				m_StatData[iClass].aDex = TokenNumber;

				Token = GetToken();
				m_StatData[iClass].aVit = TokenNumber;

				Token = GetToken();
				m_StatData[iClass].aEne = TokenNumber;

				Token = GetToken();
				m_StatData[iClass].aLea = TokenNumber;
			}
			else if( type == 3 )
			{
				Token = GetToken();
				if(strcmp("end",TokenString)==NULL)
				{
					break;
				}

				int rank = TokenNumber;

				if( rank < 0 || rank >= MAX_OLYMP_RANK )
				{
					LogAddC(2,"[CustomOlymp] CCustomOlymp::LoadData() - rank not in range : %d",rank);
					return false;
				}

				Token = GetToken();
				m_RewardPts[rank] = TokenNumber;
			}
		}
	}

	fclose(SMDFile);

	EnterCriticalSection(&m_critScheduleData);

	if(!vtScheduleData.empty())
	{
		for(_OLYMP_SCHEDULE_DATA * it = vtScheduleData.begin(); it != vtScheduleData.end(); it++)
		{
			m_vtScheduleData.push_back(*it);
		}
	}

	sort(m_vtScheduleData.begin(),m_vtScheduleData.end(),ScheduleStateCompFunc);

	if(!m_vtScheduleData.empty())
	{
		for(int iIDX = 0; iIDX < m_vtScheduleData.size(); iIDX++)
		{
			if(iIDX+1 < m_vtScheduleData.size())
			{
				int iIDX_SEC1 = m_vtScheduleData[iIDX].m_iADD_DAY * 24 * 60 * 60 + m_vtScheduleData[iIDX].m_iADD_HOUR * 60 * 60 + m_vtScheduleData[iIDX].m_iADD_MIN * 60;
				int iIDX_SEC2 = m_vtScheduleData[iIDX+1].m_iADD_DAY * 24 * 60 * 60 + m_vtScheduleData[iIDX+1].m_iADD_HOUR * 60 * 60 + m_vtScheduleData[iIDX+1].m_iADD_MIN * 60;

				int iIDX_RESULT = iIDX_SEC2 - iIDX_SEC1;

				if(iIDX_RESULT < 0)
				{
					LogAddC(2,"[CustomOlymp] CCustomOlymp::LoadData() - Date Order is wrong (sort fail) : %d-%d",
						m_vtScheduleData[iIDX].m_iSTATE,m_vtScheduleData[iIDX+1].m_iSTATE);
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
	m_bFileDataLoadOK = 1;
return true;
}

void CCustomOlymp::GetStateDateTime(int iState,_SYSTEMTIME * tm)
{
	EnterCriticalSection(&m_critScheduleData);

	for(_OLYMP_SCHEDULE_DATA * it = m_vtScheduleData.begin();it != m_vtScheduleData.end();it++)
	{
		_OLYMP_SCHEDULE_DATA & pScheduleData = *it;

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

int CCustomOlymp::GetStateGapSec(int iOlympState)
{
	int iGAP_SEC = -1;

	EnterCriticalSection(&m_critScheduleData);

	for(_OLYMP_SCHEDULE_DATA * it = m_vtScheduleData.begin(); it != m_vtScheduleData.end(); it++)
	{
		_OLYMP_SCHEDULE_DATA & pScheduleData = *it;

		if(pScheduleData.m_iSTATE == iOlympState)
		{
			iGAP_SEC = pScheduleData.m_iGAP_SEC;
			break;
		}
	}

	LeaveCriticalSection(&m_critScheduleData);

	return iGAP_SEC;
}

BOOL CCustomOlymp::IsRegistered(int aIndex)
{
	for(int i = 0; i < MAX_OLYMP_USER; i++)	{
		if( m_UserData[i].m_Index == aIndex )
			return true;
	}
return false;
}

BOOL CCustomOlymp::RegUser(int aIndex,int pts)
{
	for(int i = 0; i < MAX_OLYMP_USER; i++)	{
		if( m_UserData[i].m_Index == -1 )	{
			m_UserData[i].m_Index = aIndex;
			m_UserData[i].m_Score = pts;
			m_UserCount++;
			return true;
		}
	}
return false;
}

void CCustomOlymp::CheckUsers()
{
	for(int i = 0; i < MAX_OLYMP_USER; i++)	{

		int aIndex = m_UserData[i].m_Index;

		if( aIndex == -1 )
			continue;

		if( !gObjIsConnected(aIndex) )
		{
			m_UserData[i].m_Index = -1;
			m_UserData[i].m_btROOM = -1;
			m_UserCount--;

			LogAddC(4,"[CustomOlymp] Delete user(%d)",aIndex);
			continue;
		}

		if( m_bFIGHT_START )
		{
			if( gObj[aIndex].MapNumber != MAP_INDEX_PKFIELD )
			{
				/*if( gObj[aIndex].m_btChangedStats )
					PrepareUser(aIndex);*/

				m_UserData[i].m_Index = -1;
				m_UserData[i].m_btROOM = -1;
				m_UserCount--;

				LogAddC(4,"[CustomOlymp] Not on map user(%d)",aIndex);
				continue;
			}
		}
	}
}

BOOL CCustomOlymp::DataRequest()
{
	if(m_bFileDataLoadOK == FALSE)
	{
		LogAddC(2,"[CustomOlymp] CCustomOlymp::DataRequest() - m_bFileDataLoadOK == FALSE");
		return false;
	}

	if(m_iMapSvrGroup < 0)
	{
		LogAddC(2,"[CustomOlymp] CCustomOlymp::DataRequest() - m_iMapSvrGroup < 0");
		return false;
	}

	if(m_iOlympDataLoadState != 2)
	{
		LogAddC(2,"[CustomOlymp] CCustomOlymp::DataRequest() - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_2 (%d)",m_iOlympDataLoadState);
		return false;
	}

	int bDbRequestData = FALSE;

	if(m_dwDbDataLoadTickCount == 0)
	{
		bDbRequestData = TRUE;
		m_dwDbDataLoadTickCount = GetTickCount();
		LogAddC(3,"[CustomOlymp] CCustomOlymp::DataRequest() - REQUEST DATA FIRST");
	}
	else if(GetTickCount() - m_dwDbDataLoadTickCount > 3000)
	{
		bDbRequestData = TRUE;
		m_dwDbDataLoadTickCount = GetTickCount();
		LogAddC(3,"[CustomOlymp] CCustomOlymp::DataRequest() - REQUEST DATA AGAIN");
	}


	if(bDbRequestData != FALSE)
	{
		GS_GDReqOlympInitData();
	}

return true;
}

int CCustomOlymp::CheckSync()
{
	if(m_bFileDataLoadOK == FALSE)
	{
		LogAddC(2,"[CustomOlymp] CCustomOlymp::CheckSync() - m_bFileDataLoadOK == FALSE");
		return false;
	}

	m_iOLYMP_TICK_COUNT = GetTickCount();

	 _SYSTEMTIME tmToDay;

	 GetLocalTime(&tmToDay);
	 int iTODAY_DATE_NUM = MACRO2(( MACRO1(tmToDay.wDay) | MACRO1(tmToDay.wMonth) << 8 ) & 0xFFFF ) | MACRO2(tmToDay.wYear) << 16;
	 int iOlympInitState = 0;

	 if(m_bFixOlympCycleStartDate != 0)
	 {
		 m_tmStartDate.wYear = m_tmFixOlympCycleStartDate.wYear;
		 m_tmStartDate.wMonth = m_tmFixOlympCycleStartDate.wMonth;
		 m_tmStartDate.wDay = m_tmFixOlympCycleStartDate.wDay;

		 m_tmEndDate = m_tmStartDate;
		 GetNextDay(&m_tmEndDate,m_iOlympCycle,0,0,0);
	 }

	 int iEVENT_END_DATE_NUM = MACRO2(( MACRO1(m_tmEndDate.wDay) | MACRO1(m_tmEndDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(m_tmEndDate.wYear) << 16;

	 if(iEVENT_END_DATE_NUM <= iTODAY_DATE_NUM)
	 {
		 m_bFixOlympCycleStartDate = 0;
		 LogAddC(2,"[CCustomOlymp] CCustomOlymp::CheckSync() - iEVENT_END_DATE_NUM (%04d-%02d-%02d) <= iTODAY_DATE_NUM (%04d-%02d-%02d)",
			 m_tmEndDate.wYear,m_tmEndDate.wMonth,m_tmEndDate.wDay,tmToDay.wYear,tmToDay.wMonth,tmToDay.wDay);
		 return false;
	 }

	 if(m_bFixOlympCycleStartDate != 0)
	 {
		 GS_GDReqOlympDateChange(m_iMapSvrGroup,-1,m_tmStartDate.wYear,m_tmStartDate.wMonth,m_tmStartDate.wDay,
			 m_tmEndDate.wYear,m_tmEndDate.wMonth,m_tmEndDate.wDay);
	 }

	 /*if(m_btIsSiegeEnded)
	 {
		 SetState(CASTLESIEGE_STATE_ENDSIEGE,1);
		 return true;
	 }*/

	 int iEVENT_START_DATE_NUM = MACRO2(( MACRO1(m_tmStartDate.wDay) | MACRO1(m_tmStartDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(m_tmStartDate.wYear) << 16;

	 if(iEVENT_START_DATE_NUM > iTODAY_DATE_NUM)
	 {
		 SetState(OLYMP_STATE_IDLE,1);
		 return true;
	 }
	 else
	 {
		 int bStateSetted = 0;
		_OLYMP_SCHEDULE_DATA pScheData_FR;
		_OLYMP_SCHEDULE_DATA pScheData_RR;
		_OLYMP_SCHEDULE_DATA * it;

		 EnterCriticalSection(&m_critScheduleData);

		 it = m_vtScheduleData.begin();

		 while(it != m_vtScheduleData.end())
		 {
			_OLYMP_SCHEDULE_DATA & pScheduleData = (_OLYMP_SCHEDULE_DATA &)*it;
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
				 m_iOlympState = iOlympInitState;
				 break;
			 }

			 iOlympInitState = pScheduleData.m_iSTATE;
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
			LogAddC(2,"[CastleSiege] CCastleSiege::CheckSync() - bStateSetted == FALSE  START_DATE (%04d-%02d-%02d), END_DATE (%04d-%02d-%02d)",
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

		time(&ttSTime);
		ttETime = mktime(&tmETime);

		dResultSecond = difftime(ttETime,ttSTime);
		m_iOLYMP_REMAIN_MSEC = dResultSecond * (double)1000.0;

		SetState(m_iOlympState,0);
	 }

return true;
}

BOOL CCustomOlymp::CheckOpen()
{
	DWORD CHECK_TIME = 0;
	DWORD NOW_TIME = 0;
	DWORD OPEN_TIME = 0;
	DWORD CLOSE_TIME = 0;

	tm * today;
	time_t ltime;

	time(&ltime);
	today = localtime(&ltime);

	NOW_TIME = today->tm_hour * 60 + today->tm_min;
	OPEN_TIME = m_iFSTART_HOUR*60 + m_iFSTART_MIN;
	CLOSE_TIME = m_iFEND_HOUR*60 + m_iFEND_MIN;

	if( today->tm_hour <= m_iFSTART_HOUR &&
		OPEN_TIME > NOW_TIME )
	{
		m_bOPEN = FALSE;
		m_iFIGHT_REMAIN_MSEC = (OPEN_TIME -NOW_TIME)*60*1000;
		LogAddTD("[CustomOlymp] Sync Open Time. [%d] min remain",m_iFIGHT_REMAIN_MSEC/60000);
	}
	else if( today->tm_hour <= m_iFEND_HOUR &&
			 CLOSE_TIME > NOW_TIME )
	{
		m_bOPEN = TRUE;
		m_iFIGHT_REMAIN_MSEC = (CLOSE_TIME -NOW_TIME)*60*1000;
		LogAddTD("[CustomOlymp] Sync Close Time. [%d] min remain",m_iFIGHT_REMAIN_MSEC/60000);
	}
	else
	{
		m_bOPEN = FALSE;
		m_iFIGHT_REMAIN_MSEC = (1440 - NOW_TIME + OPEN_TIME)*60*1000;
		LogAddTD("[CustomOlymp] Sync Open Time. [%d] min remain",m_iFIGHT_REMAIN_MSEC/60000);
	}

	m_iFIGHT_REMAIN_MSEC -= today->tm_sec*1000;
return true;
}

void CCustomOlymp::Run()
{
	if( !m_bOlympEnable )
		return;

	if( !m_bDoRun )
		return;

	switch(m_iOlympState)
	{
	case OLYMP_STATE_IDLE:
		ProcState_IDLE();
		break;
	case OLYMP_STATE_FIGHTING:
		ProcState_FIGHTING();
		break;
	case OLYMP_STATE_ENDING:
		ProcState_ENDING();
		break;
	case OLYMP_STATE_ENDCYCLE:
		ProcState_ENDCYCLE();
		break;
	}
}

const struct DUEL_ARENA_GATES
{
	int UserGate01;
	int UserGate02;
	int LearnerGate;
} 
g_DuelArenaGates[MAX_DUEL_ROOMS] = 
{
	{295, 296, 303},
	{297, 298, 304},
	{299, 300, 305},
	{301, 302, 306},
};

void CCustomOlymp::ProcState_IDLE()
{
	int iTICK_MSEC = GetTickCount() - m_iOLYMP_TICK_COUNT;

	if(iTICK_MSEC >= 1000)	{
		m_iOLYMP_REMAIN_MSEC -= iTICK_MSEC;
		m_iOLYMP_TICK_COUNT = GetTickCount();

	}

	if(m_iOLYMP_REMAIN_MSEC <= 0)	{
		SetState(OLYMP_STATE_FIGHTING,TRUE);
	}
}

void CCustomOlymp::ProcState_FIGHTING()
{
	int iTICK_MSEC = GetTickCount() - m_iOLYMP_TICK_COUNT;

	if(iTICK_MSEC >= 1000)	{
		m_iOLYMP_REMAIN_MSEC -= iTICK_MSEC;
		m_iFIGHT_REMAIN_MSEC -= iTICK_MSEC;
		m_iOLYMP_TICK_COUNT = GetTickCount();

		CheckUsers();

		if( m_UserCount == 0 )
		{
			m_dwFIGHT_START = -1;
			m_bFIGHT_START = FALSE;
		}

		if( !m_bFIGHT_START )
		{
			if( m_dwFIGHT_START > 0 )
			{
				m_dwFIGHT_START -= iTICK_MSEC;

				if( m_dwFIGHT_START <= 0 )
				{
					Start();
					m_bFIGHT_START = TRUE;
				}
			}
			else if( m_UserCount == MAX_OLYMP_USER && /*gDuelSystem.GetFreeRoomCount()*/ //g_DuelManager.GetFreeRoomIndexCount() == 4 )
			{
				m_dwFIGHT_START = 10000;

				std::set<OLYMP_USER*,CCustomOlympSort> m_listUser;

				for(int i = 0; i < MAX_OLYMP_USER; i++)	{
					if( m_UserData[i].m_Index != -1 )	{
						m_listUser.insert(&m_UserData[i]);
						MsgOutput(m_UserData[i].m_Index,"Fight will start in 60 seconds!");
						MsgOutput(m_UserData[i].m_Index,"Please cancel all current interactions!");
					}
				}

				std::set<OLYMP_USER*,CCustomOlympSort>::iterator _It = m_listUser.begin();

				int iRoom = 0;

				for( i = 1 ; _It != m_listUser.end(); _It++,i++ )	{
					OLYMP_USER* pUser = *_It;
					pUser->m_btROOM = iRoom;
					if( !(i%2) )
						iRoom++;
				}
			}
		}

		if( m_bOPEN == FALSE && m_iFIGHT_REMAIN_MSEC <= 900000 && m_iFIGHT_NOTIFY_COUNT != (m_iFIGHT_REMAIN_MSEC/60000) )
		{
			m_iFIGHT_NOTIFY_COUNT = (m_iFIGHT_REMAIN_MSEC/60000);

			if( !((m_iFIGHT_NOTIFY_COUNT+1)%5) && m_iFIGHT_NOTIFY_COUNT > 0 )
			{
				char szTmp[256];
				sprintf(szTmp,"Olympic fights registration will open in %d minute(s)",m_iFIGHT_NOTIFY_COUNT+1);
				AllSendServerMsg(szTmp);
			}
		}

		if( m_iFIGHT_REMAIN_MSEC <= 0 )
		{
			CheckOpen();

			if( m_bOPEN == FALSE )
				AllSendServerMsg("Olympic fights registration is now closed!");
			else
				AllSendServerMsg("Olympic fights registration is now open!");
		}
	}

	if(m_iOLYMP_REMAIN_MSEC <= 0)	{
		SetState(OLYMP_STATE_ENDING,TRUE);
	}
}

void CCustomOlymp::ProcState_ENDING()
{
	int iTICK_MSEC = GetTickCount() - m_iOLYMP_TICK_COUNT;

	if(iTICK_MSEC >= 1000)	{
		m_iOLYMP_REMAIN_MSEC -= iTICK_MSEC;
		m_iOLYMP_TICK_COUNT = GetTickCount();
	}

	if(m_iOLYMP_REMAIN_MSEC <= 0)	{
		SetState(OLYMP_STATE_ENDCYCLE,TRUE);
	}
}

void CCustomOlymp::ProcState_ENDCYCLE()
{
	int iTICK_MSEC = GetTickCount() - m_iOLYMP_TICK_COUNT;

	if(iTICK_MSEC >= 1000)	{
		m_iOLYMP_REMAIN_MSEC -= iTICK_MSEC;
		m_iOLYMP_TICK_COUNT = GetTickCount();
	}

	if(m_iOLYMP_REMAIN_MSEC <= 0)	{
		SetState(OLYMP_STATE_IDLE,TRUE);
	}
}

void CCustomOlymp::SetState(int iState,int bSetRemainMsec)
{
	m_iOlympState = iState;

	switch(m_iOlympState)
	{
	case OLYMP_STATE_IDLE:
		SetState_IDLE(bSetRemainMsec);
		break;
	case OLYMP_STATE_FIGHTING:
		SetState_FIGHTING(bSetRemainMsec);
		break;
	case OLYMP_STATE_ENDING:
		SetState_ENDING(bSetRemainMsec);
		break;
	case OLYMP_STATE_ENDCYCLE:
		SetState_ENDCYCLE(bSetRemainMsec);
		break;
	}
}

void CCustomOlymp::SetState_IDLE(int bSetRemainMsec)
{
	LogAddTD("[CustomOlymp] SET SetState_IDLE_1()");

	if(bSetRemainMsec)
	{
		long ttSTime;
		long ttETime;
		tm tmETime;
		double dResultSecond;

		memset(&tmETime,0x00,sizeof(tmETime));

		tmETime.tm_year = m_tmStartDate.wYear - 1900;
		tmETime.tm_mon = m_tmStartDate.wMonth - 1;
		tmETime.tm_mday = m_tmStartDate.wDay;

		ttETime = mktime(&tmETime);
		time(&ttSTime);

		dResultSecond = difftime(ttETime,ttSTime);
		m_iOLYMP_REMAIN_MSEC = dResultSecond * (double)1000.0;
	}
}

void CCustomOlymp::SetState_FIGHTING(int bSetRemainMsec)
{
	LogAddTD("[CustomOlymp] SET SetState_FIGHTING()");

	if(bSetRemainMsec)
	{
		int iGAP_SEC = GetStateGapSec(OLYMP_STATE_FIGHTING);

		if(iGAP_SEC < 0)
		{
			m_bDoRun = 0;
			MsgBox("[CustomOlymp] CCustomOlymp::SetState_FIGHTING() - iGAP_SEC < 0");
		}

		m_iOLYMP_REMAIN_MSEC = iGAP_SEC * 1000;
	}

	Reset();
	CheckOpen();
	//GS_GDReqRestartCastleState(m_iMapSvrGroup);
}

void CCustomOlymp::SetState_ENDING(int bSetRemainMsec)
{
	LogAddTD("[CustomOlymp] SET SetState_ENDING()");

	if(bSetRemainMsec)
	{
		long ttSTime;
		long ttETime;
		tm tmETime;
		double dResultSecond;

		memset(&tmETime,0x00,sizeof(tmETime));

		tmETime.tm_year = m_tmEndDate.wYear - 1900;
		tmETime.tm_mon = m_tmEndDate.wMonth - 1;
		tmETime.tm_mday = m_tmEndDate.wDay;

		time(&ttSTime);
		ttETime = mktime(&tmETime);

		dResultSecond = difftime(ttETime,ttSTime);
		m_iOLYMP_REMAIN_MSEC = dResultSecond * (double)1000.0;
	}

	m_bOPEN = FALSE;

	//SendAllUserAnyMsg(lMsg.Get(MSGGET(6, 84)),1);
}

void CCustomOlymp::SetState_ENDCYCLE(int iRemainSec)
{
	LogAddTD("[CustomOlymp] SET SetState_ENDCYCLE()");

	_SYSTEMTIME tmNowDate;
	GetLocalTime(&tmNowDate);

	tmNowDate.wDay = 0;
	tmNowDate.wHour = 0;
	tmNowDate.wMinute = 0;
	tmNowDate.wSecond = 0;
	tmNowDate.wMilliseconds = 0;

	tmNowDate = m_tmNowDate;
	m_tmStartDate = m_tmNowDate;

	GetNextDay(&tmNowDate,m_iOlympCycle,0,0,0);
	GetNextDay(&m_tmNowDate,m_iOlympCycle,0,0,0);

	m_tmEndDate = tmNowDate;

	LogAddTD("[CustomOlymp] State - End Cycle Schedule : (%d-%d-%d(%d:%d:%d)) (%d-%d-%d(%d:%d:%d))",
		m_tmStartDate.wYear,m_tmStartDate.wMonth,m_tmStartDate.wDay,m_tmStartDate.wHour,m_tmStartDate.wMinute,m_tmStartDate.wSecond,
		m_tmEndDate.wYear,m_tmEndDate.wMonth,m_tmEndDate.wDay,m_tmEndDate.wHour,m_tmEndDate.wMinute,m_tmEndDate.wSecond);

	LogAddTD("[CustomOlymp] State - End Cycle : Date-Changing Info (%d-%d-%d) (%d-%d-%d)",
		m_tmStartDate.wYear,m_tmStartDate.wMonth,m_tmStartDate.wDay,
		m_tmEndDate.wYear,m_tmEndDate.wMonth,m_tmEndDate.wDay);

	GS_GDReqOlympDateChange(m_iMapSvrGroup,-1,m_tmStartDate.wYear,m_tmStartDate.wMonth,m_tmStartDate.wDay,m_tmEndDate.wYear,m_tmEndDate.wMonth,m_tmEndDate.wDay);
	GS_GDReqResetOlympInfo(m_iMapSvrGroup);

	SetState(OLYMP_STATE_FIGHTING,TRUE);
}

int CCustomOlymp::Init()
{

	if(m_iOlympDataLoadState != 3)
	{
		LogAddC(2,"[CustomOlymp] CCustomOlymp::Init() - m_iOlympDataLoadState != 3 (%d)",m_iOlympDataLoadState);
		MsgBox("[CustomOlymp] CCustomOlymp::Init() - m_iOlympDataLoadState != 3 (%d)",m_iOlympDataLoadState);
		return false;
	}

	if(m_bFileDataLoadOK == 0 || m_bDbDataLoadOK == 0)
	{
		LogAddC(2,"[CustomOlymp] CCustomOlymp::Init() - Data Load Fail (FILE:%d, DB:%d)",
			m_bFileDataLoadOK,m_bDbDataLoadOK);
		MsgBox("[CustomOlymp] CCustomOlymp::Init() - Data Load Fail (FILE:%d, DB:%d)",
			m_bFileDataLoadOK,m_bDbDataLoadOK);
		return false;
	}

	LogAddTD("[CustomOlymp] CCustomOlymp::Init() - START");

	m_iOlympDataLoadState = 0;

	if(CheckSync() == FALSE)
	{
		LogAddC(2,"[CustomOlymp] CCustomOlymp::Init() - CheckSync() == FALSE");
		return false;
	}

	m_bDoRun = TRUE;
	return true;
}

BOOL CCustomOlymp::SetOlympInitData(OLP_ANS_OLYMPINITDATA* lpMsg)
{
	if(m_iOlympDataLoadState != 3)
	{
		LogAddC(2,"[CustomOlymp] CCustomOlymp::SetOlympInitData() - m_iOlympDataLoadState != 3");
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

	int iSTART_DATE_NUM = MACRO2(( MACRO1(m_tmStartDate.wDay) | MACRO1(m_tmStartDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(m_tmStartDate.wYear) << 16;
	int iEND_DATE_NUM = MACRO2(( MACRO1(m_tmEndDate.wDay) | MACRO1(m_tmEndDate.wMonth) << 8 ) & 0xFFFF ) | MACRO2(m_tmEndDate.wYear) << 16;

	m_tmNowDate = m_tmStartDate;

	_SYSTEMTIME stTime;

	GetStateDateTime(OLYMP_STATE_ENDCYCLE,&stTime);

	LogAddTD("[CustomOlymp] CCustomOlymp::LoadData() - Siege Schedule Start Date (%d-%d-%d)",
		m_tmStartDate.wYear,m_tmStartDate.wMonth,m_tmStartDate.wDay);

	LogAddTD("[CustomOlymp] CCustomOlymp::LoadData() - Siege Schedule Date (%d-%d-%d)",
		stTime.wDay,stTime.wHour,stTime.wMinute);

	GetNextDay(&m_tmNowDate,stTime.wDay,stTime.wHour,stTime.wMinute,0);

	LogAddTD("[CustomOlymp] CCustomOlymp::LoadData() - Siege Schedule End Date (%d-%d-%d(%d:%d:%d)",
		m_tmNowDate.wYear,m_tmNowDate.wMonth,m_tmNowDate.wDay,m_tmNowDate.wHour,m_tmNowDate.wMinute,m_tmNowDate.wSecond);

	if(iSTART_DATE_NUM > iEND_DATE_NUM)
	{
		LogAddC(2,"[CustomOlymp] CCustomOlymp::SetOlympInitData() - iSTART_DATE_NUM > iEND_DATE_NUM");
		return false;
	}

	return true;
}

void CCustomOlymp::CGRequestReg(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use != TRUE ||
		lpObj->m_IfState.type != 21 )
	{
		LogAddC(2,"[CustomOlymp] User not used NPC [%s][%s]",
			lpObj->AccountID,lpObj->Name);
		return;
	}

	lpObj->m_IfState.use = FALSE;

	PMSG_OLP_MESSAGEBOX pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xFE,0x01,sizeof(pMsg));
	pMsg.btMessage = 0;

	/*if( gObjCheckUsedBuffEffect(lpObj,BUFF_GLADIATOR_HONOR) )
	{
		pMsg.btMessage = 14;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}*/

	if( m_UserCount >= MAX_OLYMP_USER )
	{
		pMsg.btMessage = 1;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpObj->iResetCount < this->m_iMinResetCount )
	{
		pMsg.aInfo1 = m_iMinResetCount;
		pMsg.btMessage = 8;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( IsRegistered(aIndex) )
	{
		pMsg.btMessage = 2;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( m_bFIGHT_START == TRUE )
	{
		pMsg.btMessage = 4;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	OLP_REQ_REGFIGHT pMsg2;
	PHeadSubSetB((LPBYTE)&pMsg2,0x68,0x02,sizeof(pMsg2));

	pMsg2.wMapSvrNum = m_iMapSvrGroup;
	pMsg2.iIndex = aIndex;
	pMsg2.iClass = lpObj->Class;
	memcpy(pMsg2.szName,lpObj->Name,10);

	wsEvenChipServerCli.DataSend((PCHAR)&pMsg2,pMsg2.h.size);
}

void CCustomOlymp::CGRequestReward(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use != TRUE ||
		lpObj->m_IfState.type != 21 )
	{
		LogAddC(2,"[CustomOlymp] User not used NPC [%s][%s]",
			lpObj->AccountID,lpObj->Name);
		return;
	}

	OLP_REQ_REWARD pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0x68,0x05,sizeof(pMsg));

	pMsg.wMapSvrNum = m_iMapSvrGroup;
	pMsg.iIndex = aIndex;
	memcpy(pMsg.szName,lpObj->Name,10);

	wsEvenChipServerCli.DataSend((PCHAR)&pMsg,pMsg.h.size);
}

void CCustomOlymp::Start()
{
	for(int i = 0; i < MAX_DUEL_ROOMS; i++)	{

		int aIndex1 = -1;
		int aIndex2 = -1;

		for(int n = 0; n < MAX_OLYMP_USER; n++)
		{
			if( aIndex1 == -1 )
			{
				if( m_UserData[n].m_btROOM == i )
				{
					aIndex1 = m_UserData[n].m_Index;
				}
			}
			else if( aIndex2 == -1 )
			{
				if( m_UserData[n].m_btROOM == i && m_UserData[n].m_Index != aIndex1 )
				{
					aIndex2 = m_UserData[n].m_Index;
					break;
				}
			}
		}

			
		if( !gObjIsConnected(aIndex1) || !gObjIsConnected(aIndex2) )
			continue;

		if( (gObj[aIndex1].m_IfState.use&&gObj[aIndex1].m_IfState.type!=21) || (gObj[aIndex2].m_IfState.use&&gObj[aIndex2].m_IfState.type!=21) )
		{
			continue;
		}

		LPOBJ lpObj1 = &gObj[aIndex1];
		LPOBJ lpObj2 = &gObj[aIndex2];

		if( lpObj1->PartyNumber >= 0 )
		{
			PMSG_PARTYDELUSER pMsg;
			int PartyIndex;
			PartyIndex = gParty.GetIndex(lpObj1->PartyNumber,lpObj1->m_Index,lpObj1->DBNumber);

			if( PartyIndex >= 0 )
			{
				pMsg.Number = PartyIndex;
				CGPartyDelUser(&pMsg,lpObj1->m_Index);
			}
		}

		if( lpObj2->PartyNumber >= 0 )
		{
			PMSG_PARTYDELUSER pMsg;
			int PartyIndex;
			PartyIndex = gParty.GetIndex(lpObj2->PartyNumber,lpObj2->m_Index,lpObj2->DBNumber);

			if( PartyIndex >= 0 )
			{
				pMsg.Number = PartyIndex;
				CGPartyDelUser(&pMsg,lpObj2->m_Index);
			}
		}

		gObjClearBuffEffect(lpObj1,CLEAR_TYPE_NON_PCS_ITEM_EFFECT);
		gObjClearBuffEffect(lpObj2,CLEAR_TYPE_NON_PCS_ITEM_EFFECT);
		
		lpObj1->m_IfState.use = 0;
		lpObj1->m_IfState.type = 0;

		lpObj2->m_IfState.use = 0;
		lpObj2->m_IfState.type = 0;

		//int iDuelRoom = gDuelSystem.GetFreeRoom();
		int iDuelRoom = //g_DuelManager.GetFreeRoomIndex();

		if( iDuelRoom != -1 )
		{
			//PrepareUser(aIndex1);
			//PrepareUser(aIndex2);

			//gDuelSystem.AddRoomInfo(iDuelRoom,aIndex1,aIndex2);
			//g_DuelManager.AddRoomInfoForOlymp(iDuelRoom,lpObj1,lpObj2);

			lpObj1->m_iDuelRoom = iDuelRoom;
			lpObj2->m_iDuelRoom = iDuelRoom;

			gObjMoveGate(aIndex1,g_DuelArenaGates[iDuelRoom].UserGate01);
			gObjMoveGate(aIndex2,g_DuelArenaGates[iDuelRoom].UserGate02);

			lpObj1->m_iDuelUserRequested = -1;
			lpObj1->m_iDuelUserReserved = -1;
			lpObj1->m_btDuelScore = 0;
			lpObj1->m_iDuelUser = aIndex2;
			lpObj1->m_iDuelTickCount = GetTickCount();

			lpObj2->m_iDuelUserRequested = -1;
			lpObj2->m_iDuelUserReserved = -1;
			lpObj2->m_btDuelScore = 0;
			lpObj2->m_iDuelUser = aIndex1;
			lpObj2->m_iDuelTickCount = GetTickCount();

			PMSG_DUEL_START pMsgSend = {0};
			PHeadSubSetB((LPBYTE)&pMsgSend,0xAA,0x01,sizeof(pMsgSend));

			pMsgSend.bDuelStart = 0;

			memcpy(pMsgSend.szName,gObj[aIndex1].Name,10);
			pMsgSend.NumberH = SET_NUMBERH(aIndex1);
			pMsgSend.NumberL = SET_NUMBERL(aIndex1);

			DataSend(aIndex2,(LPBYTE)&pMsgSend,pMsgSend.h.size);

			memcpy(pMsgSend.szName,gObj[aIndex2].Name,10);
			pMsgSend.NumberH = SET_NUMBERH(aIndex2);
			pMsgSend.NumberL = SET_NUMBERL(aIndex2);

			DataSend(aIndex1,(LPBYTE)&pMsgSend,pMsgSend.h.size);

			//GCSendDuelScore(aIndex1, aIndex2);
			//g_DuelManager.UpdateDuelScore(iDuelRoom);
		}
	}

	LogAddC(4,"[CustomOlymp] Fights start!");
}

void CCustomOlymp::PrepareUser(int aIndex)
{
	if( !OBJMAX_RANGE(aIndex) )
		return;

	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_btChangedStats == FALSE )
	{
		lpObj->m_btChangedStats = TRUE;
		lpObj->m_OldLvlUpPts = lpObj->LevelUpPoint;
		lpObj->m_OldStrength = lpObj->Strength;
		lpObj->m_OldDexterity = lpObj->Dexterity;
		lpObj->m_OldVitality = lpObj->Vitality;
		lpObj->m_OldEnergy = lpObj->Energy;
		lpObj->m_OldLeadership = lpObj->Leadership;
	
		lpObj->LevelUpPoint = 0;
		lpObj->Strength = m_StatData[lpObj->Class].aStr;
		lpObj->Dexterity = m_StatData[lpObj->Class].aDex;
		lpObj->Vitality = m_StatData[lpObj->Class].aVit;
		lpObj->Energy = m_StatData[lpObj->Class].aEne;
		lpObj->Leadership = m_StatData[lpObj->Class].aLea;

		LogAddTD("[CustomOlymp] Stats changed for (%s)(%s)",
			lpObj->AccountID,lpObj->Name);

		lpObj->MaxLife = DCInfo.DefClass[lpObj->Class].Life + ((lpObj->Vitality - DCInfo.DefClass[lpObj->Class].Vitality ) * DCInfo.DefClass[lpObj->Class].VitalityToLife);

		if( lpObj->Life > lpObj->MaxLife + lpObj->AddLife )
		{
			lpObj->Life = lpObj->MaxLife;
		}

		lpObj->MaxMana = DCInfo.DefClass[lpObj->Class].Mana + ((lpObj->Energy - DCInfo.DefClass[lpObj->Class].Energy ) * DCInfo.DefClass[lpObj->Class].EnergyToMana);

		if( lpObj->Mana > lpObj->MaxMana + lpObj->AddMana )
		{
			lpObj->Mana = lpObj->MaxMana;
		}
	}
	else
	{
		lpObj->m_btChangedStats = FALSE;

		lpObj->LevelUpPoint = lpObj->m_OldLvlUpPts;
		lpObj->Strength = lpObj->m_OldStrength;
		lpObj->Dexterity = lpObj->m_OldDexterity;
		lpObj->Vitality = lpObj->m_OldVitality;
		lpObj->Energy = lpObj->m_OldEnergy;
		lpObj->Leadership = lpObj->m_OldLeadership;

		lpObj->m_OldLvlUpPts = 0;
		lpObj->m_OldStrength = 0;
		lpObj->m_OldDexterity = 0;
		lpObj->m_OldVitality = 0;
		lpObj->m_OldEnergy = 0;
		lpObj->m_OldLeadership = 0;

		LogAddTD("[CustomOlymp] Stats restored for (%s)(%s)",
			lpObj->AccountID,lpObj->Name);

		lpObj->MaxLife = DCInfo.DefClass[lpObj->Class].Life + (lpObj->Level + lpObj->MasterLevel - 1) * DCInfo.DefClass[lpObj->Class].LevelLife  + ((lpObj->Vitality - DCInfo.DefClass[lpObj->Class].Vitality ) * DCInfo.DefClass[lpObj->Class].VitalityToLife);

		if( lpObj->Life > lpObj->MaxLife + lpObj->AddLife )
		{
			lpObj->Life = lpObj->MaxLife;
		}

		lpObj->MaxMana = DCInfo.DefClass[lpObj->Class].Mana + (lpObj->Level + lpObj->MasterLevel - 1) * DCInfo.DefClass[lpObj->Class].LevelMana  + ((lpObj->Energy - DCInfo.DefClass[lpObj->Class].Energy ) * DCInfo.DefClass[lpObj->Class].EnergyToMana);

		if( lpObj->Mana > lpObj->MaxMana + lpObj->AddMana )
		{
			lpObj->Mana = lpObj->MaxMana;
		}
	}

	gObjSetBP(lpObj->m_Index);
	gObjCalCharacter(lpObj->m_Index);

	PMSG_SETSTATS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xFE,0xFE,sizeof(pMsg));

	pMsg.Str = lpObj->Strength;
	pMsg.Dex = lpObj->Dexterity;
	pMsg.Vit = lpObj->Vitality;
	pMsg.Ene = lpObj->Energy;
	pMsg.Lea = lpObj->Leadership;

	pMsg.MaxLife = lpObj->MaxLife + lpObj->AddLife;
	pMsg.MaxMana = lpObj->MaxMana + lpObj->AddMana;
	pMsg.MaxBP = lpObj->MaxBP + lpObj->AddBP;
	pMsg.wMaxShield = lpObj->iMaxShield+lpObj->iAddShield;
	pMsg.LvlUpPt = lpObj->LevelUpPoint;

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
}

void CCustomOlymp::EndFight(int iWinner,int iLooser,int iFail)
{
	if( !OBJMAX_RANGE(iWinner) ||
		!OBJMAX_RANGE(iLooser) )
	{
		LogAddC(2,"[CustomOlymp] EndFight Index not in range User1[%d] User2[%d] iEndState[%d]",
			iWinner,iLooser,iFail);
		return;
	}

	if( !IsRegistered(iWinner) ||
		!IsRegistered(iLooser) )
	{
		LogAddC(2,"[CustomOlymp] EndFight User not registered User1[%d] User2[%d] iEndState[%d]",
			iWinner,iLooser,iFail);
		return;
	}

	for(int i = 0; i < MAX_OLYMP_USER; i++)	{
		if( m_UserData[i].m_Index == iLooser ||
			m_UserData[i].m_Index == iWinner )
		{
			//PrepareUser(m_UserData[i].m_Index);
			m_UserData[i].m_btROOM = -1;
			m_UserData[i].m_Index = -1;
			m_UserData[i].m_Score = 0;
			m_UserCount--;
		}
	}

	OLP_ENFIGHT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0x68,0x03,sizeof(pMsg));

	pMsg.wMapSvrNum = m_iMapSvrGroup;
	pMsg.iEndResult = iFail;
	memcpy(pMsg.szWinner,gObj[iWinner].Name,10);
	memcpy(pMsg.szLooser,gObj[iLooser].Name,10);

	wsEvenChipServerCli.DataSend((char*)&pMsg,pMsg.h.size);

	LogAddC(4,"[CustomOlymp] End fight User1[%s] User2[%s] iEndState[%d]",
		gObj[iWinner].Name,gObj[iLooser].Name,iFail);
}

void CCustomOlymp::GS_GDReqOlympInitData()
{
	if( m_iMapSvrGroup < 0 )
		return;

	OLP_REQ_OLYMPINITDATA pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x68,0, sizeof(OLP_REQ_OLYMPINITDATA));
	pMsg.wMapSvrNum = m_iMapSvrGroup;
	pMsg.iOlympEventCycle = m_iOlympCycle;

	wsEvenChipServerCli.DataSend((PCHAR)&pMsg,sizeof(pMsg));
}

void CCustomOlymp::GS_DGAnsOlympInitData(LPBYTE lpRecv)
{
	OLP_ANS_OLYMPINITDATA* lpMsg = (OLP_ANS_OLYMPINITDATA*)lpRecv;

	if( lpMsg == NULL )
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CustomOlymp] PACKET-ERROR [0x68][0x00] GS_DGAnsOlympInitData() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if(GetDataLoadState() != 2)
	{
		LogAddC(2,"[CustomOlymp] OLYMP DATA SETTING FAILED [0x68][0x00] - m_iCastleDataLoadState != 2 (%d)",GetDataLoadState());
		return;
	}

	SetDataLoadState(3);

	if(lpMsg->iResult == 0)
	{
		LogAddC(2,"[CustomOlymp] OLYMP DATA SETTING FAILED [0x68][0x00] - lpMsg->iResult == 0");
		return;
	}

	int bRET_VAL = FALSE;

	bRET_VAL = SetOlympInitData(lpMsg);

	if(bRET_VAL == FALSE)
	{
		LogAddC(2,"[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - g_CastleSiege.SetCastleInitData() == FALSE");
		return;
	}

	m_bDbDataLoadOK = TRUE;
	Init();
}

void CCustomOlymp::GS_GDReqOlympDateChange(int iMapSvrGroup, int aIndex, WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay)
{
	if(iMapSvrGroup < 0)
		return;

	OLP_REQ_SDEDCHANGE pMsg;

	pMsg.h.set((PBYTE)&pMsg,0x68,1,sizeof(pMsg));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.wStartYear = wStartYear;
	pMsg.btStartMonth = btStartMonth;
	pMsg.btStartDay = btStartDay;
	pMsg.wEndYear = wEndYear;
	pMsg.btEndMonth = btEndMonth;
	pMsg.btEndDay = btEndDay;

	wsEvenChipServerCli.DataSend((char*)&pMsg,pMsg.h.size);
}

void CCustomOlymp::GS_DGAnsOlympDateChange(LPBYTE lpRecv)
{
	OLP_ANS_SDEDCHANGE * lpMsg = (OLP_ANS_SDEDCHANGE*)lpRecv;

	if(lpMsg == NULL)
		return;

	if(lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		LogAddC(2,"[CustomOlymp] PACKET-ERROR [0x68][0x01] GS_DGAnsOlympDateChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}
}

void CCustomOlymp::GS_DGAnsOlympReg(LPBYTE lpRecv)
{
	OLP_ANS_REGFIGHT* lpMsg = (OLP_ANS_REGFIGHT*)lpRecv;

	if( lpMsg == NULL )
		return;

	int aIndex = lpMsg->iIndex;

	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( memcmp(lpObj->Name,lpMsg->szName,10) )
		return;

	PMSG_OLP_MESSAGEBOX pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xFE,0x01,sizeof(pMsg));
	pMsg.btMessage = 10;

	if( lpMsg->btResult == 0 )	{
		pMsg.btMessage = 3;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpMsg->iBattleCount == -1 && 
		lpMsg->iBattlePts == -1 )
	{
		pMsg.btMessage = 14;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpMsg->iBattleCount <= 0 )	{
		pMsg.btMessage = 5;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpMsg->iBattlePts <= 0 )	{
		pMsg.btMessage = 6;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpMsg->iLastRegTime < 1 )	{
		pMsg.btMessage = 7;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( !RegUser(aIndex,lpMsg->iBattlePts) )
	{
		pMsg.btMessage = 3;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void CCustomOlymp::GS_GDReqResetOlympInfo(int iMapSvrGroup)
{
	OLP_REQ_RESET pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0x68,0x04,sizeof(pMsg));

	pMsg.wMapSvrNum = m_iMapSvrGroup;
	wsEvenChipServerCli.DataSend((char*)&pMsg,pMsg.h.size);
}

void CCustomOlymp::GS_DGAnsOlympReward(LPBYTE aRecv)
{
	OLP_ANS_REWARD* lpMsg = (OLP_ANS_REWARD*)aRecv;

	if( lpMsg == NULL )
		return;

	int aIndex = lpMsg->iIndex;

	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( memcmp(lpObj->Name,lpMsg->szName,10) )
		return;

	PMSG_OLP_MESSAGEBOX pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xFE,0x01,sizeof(pMsg));

	if( lpMsg->btResult != 1 || !(lpMsg->btRank >= 1 && lpMsg->btRank <= 10) )	{
		pMsg.btMessage = 11;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpMsg->btReward != FALSE )
	{
		pMsg.btMessage = 12;
		pMsg.aInfo1 = lpMsg->btRank;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	int rank = lpMsg->btRank-1;

	if( rank == 0 )
	{
		long ttSTime;
		long ttETime;
		struct tm tmETime;
		double dResultSecond;

		memset(&tmETime,0x00,sizeof(tmETime));

		tmETime.tm_year = m_tmEndDate.wYear - 1900;
		tmETime.tm_mon = m_tmEndDate.wMonth - 1;
		tmETime.tm_mday = m_tmEndDate.wDay;
		tmETime.tm_hour = m_tmEndDate.wHour;
		tmETime.tm_min = m_tmEndDate.wMinute;

		time(&ttSTime);
		ttETime = mktime(&tmETime);

		dResultSecond = difftime(ttETime,ttSTime);

		g_CashItemPeriodSystem.GDReqPeriodItemInsert(lpObj, ITEMGET(13,95), (dResultSecond/60));
	}

	protocol::MSG_GTOS_UPDATE_POINT_EX pCashMsg;

	pCashMsg.dwUserGuid = lpObj->DBNumber;
	pCashMsg.dwAddPoint[0] = m_RewardPts[rank];
	pCashMsg.dwAddPoint[1] = 0;
	pCashMsg.dwAddPoint[2] = 0;
	g_ShopServerClient.DataSend((PCHAR)&pCashMsg,sizeof(pCashMsg));

	pMsg.btMessage = 13;
	pMsg.aInfo1 = lpMsg->btRank;
	pMsg.aInfo2 = m_RewardPts[rank];
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void CCustomOlymp::GS_GDReqOlympStats(int aIndex)
{
	if( gObj[aIndex].iResetCount < this->m_iMinResetCount )
	{
		MsgOutput(aIndex,"You are not eligible for olympic fights!");
		return;
	}


	OLP_REQ_STATS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0x68,0x06,sizeof(pMsg));

	pMsg.iIndex = aIndex;
	pMsg.wMapSvrNum = this->m_iMapSvrGroup;

	memcpy(pMsg.szName,gObj[aIndex].Name,10);
	wsEvenChipServerCli.DataSend((char*)&pMsg,pMsg.h.size);
}

void CCustomOlymp::GS_DGAnsOlympStats(LPBYTE aRecv)
{
	OLP_ANS_STATS* lpMsg = (OLP_ANS_STATS*)aRecv;

	if( lpMsg == NULL )
		return;

	int aIndex = lpMsg->iIndex;

	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( memcmp(lpObj->Name,lpMsg->szName,10) )
		return;

	if( lpMsg->btResult == FALSE )
	{
		MsgOutput(aIndex,"You have not registered for olympic fights!");
	}
	else
	{
		MsgOutput(aIndex,"You have [%d] point(s) and [%d] battle(s) !",
			lpMsg->iPts,lpMsg->iBattles);
	}
}

#endif

#if (CUSTOM_EXOPT == 1)

CNewOptions gNewOptions;

CNewOptions::CNewOptions()
{

}

CNewOptions::~CNewOptions()
{

}

void CNewOptions::LoadData(char* lpszFileName)
{
	SMDFile = fopen(lpszFileName,"r");

	if( !SMDFile )
	{
		MsgBox("Failed to load ExOptions.txt");
		return;
	}

	int Token,Index;

	m_vtEnchantOpt.clear();
	m_vtExcellentOpt.clear();
	m_vtOptList.clear();

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		Index = TokenNumber;

		if( Index == 0 )
		{
			while( TRUE )
			{
				Token = GetToken();

				if( Token == NAME )
					if( !strcmp(TokenString,"end") )
						break;

				CUSTOM_OPTIONS_NAME pTmp;

				pTmp.iIndex = TokenNumber;

				Token = GetToken();
				strcpy(pTmp.szName,TokenString);
			}
		}
		else if( Index == 1 )
		{
			while( TRUE )
			{
				Token = GetToken();

				if( Token == NAME )
					if( !strcmp(TokenString,"end") )
						break;

				CUSTOM_OPTIONS_ENCHANT pstruct;

				pstruct.iId = TokenNumber;

				for(int i = 0; i < 6; i++)
				{
					Token = GetToken();
					pstruct.iIndex1[i] = TokenNumber;

					Token = GetToken();
					pstruct.iValue1[i] = TokenNumber;

					Token = GetToken();
					pstruct.iIndex2[i] = TokenNumber;

					Token = GetToken();
					pstruct.iValue2[i] = TokenNumber;
				}


				m_vtEnchantOpt.insert(std::pair<int,CUSTOM_OPTIONS_ENCHANT>(pstruct.iId,pstruct));
			}
		}
		else if( Index == 2 )
		{
			while( TRUE )
			{
				Token = GetToken();

				if( Token == NAME )
					if( !strcmp(TokenString,"end") )
						break;

				CUSTOM_OPTIONS_EXCELLENT pstruct;

				pstruct.iId = TokenNumber;

				Token = GetToken();
				pstruct.iValue[0] = TokenNumber;

				Token = GetToken();
				pstruct.iValue[1] = TokenNumber;

				Token = GetToken();
				pstruct.iValue[2] = TokenNumber;

				Token = GetToken();
				pstruct.iValue[3] = TokenNumber;

				Token = GetToken();
				pstruct.iValue[4] = TokenNumber;

				Token = GetToken();
				pstruct.iValue[5] = TokenNumber;

				m_vtExcellentOpt.insert(std::pair<int,CUSTOM_OPTIONS_EXCELLENT>(pstruct.iId,pstruct));
			}
		}
		else if( Index == 3 )
		{
			while( TRUE )
			{
				Token = GetToken();

				if( Token == NAME )
					if( !strcmp(TokenString,"end") )
						break;

				CUSTOM_OPTIONS_LIST pstruct;

				int iIndex = TokenNumber;

				Token = GetToken();
				int iNumber = TokenNumber;

				pstruct.Item = iIndex*512+iNumber;

				Token = GetToken();
				pstruct.EnchantId = TokenNumber;

				Token = GetToken();
				pstruct.ExcellentId = TokenNumber;

				m_vtOptList.insert(std::pair<int,CUSTOM_OPTIONS_LIST>(pstruct.Item,pstruct));
			}
		}
	}

	fclose(SMDFile);
}

int CNewOptions::GetExcellentId(int ItemID)
{
	if( ItemID < 0 || ItemID >= MAX_ITEMS )
		return -1;

	vtOptList::iterator _ItItem = m_vtOptList.find(ItemID);

	if( _ItItem == m_vtOptList.end() )
		return -1;

	return _ItItem->second.ExcellentId;
}

int CNewOptions::GetEnchantId(int ItemID)
{
	if( ItemID < 0 || ItemID >= MAX_ITEMS )
		return -1;

	vtOptList::iterator _ItItem = m_vtOptList.find(ItemID);

	if( _ItItem == m_vtOptList.end() )
		return -1;

	return _ItItem->second.EnchantId;
}


BYTE CNewOptions::GetExcellentOption(int OptId,int OptionID)
{
	if( OptId == -1 )
		return 0;

	if( OptionID < 0 || OptionID >= 6 )
		return 0;

	vtExcellentOpt::iterator _It = m_vtExcellentOpt.find(OptId);

	if( _It == m_vtExcellentOpt.end() )
		return 0;

	return _It->second.iValue[OptionID];
}

void CNewOptions::SetEnchantOptions(LPOBJ lpObj)
{
	for(int i = 0; i < INVETORY_WEAR_SIZE; i++)
	{
		if( lpObj->pInventory[i].IsItem() != TRUE )
			continue;

		int type = lpObj->pInventory[i].m_Type;

		int enchantop = GetEnchantId(type);

		if( enchantop == -1 )
			continue;

		int oplevel = lpObj->pInventory[i].m_Level;

		if( oplevel < 10 || oplevel > 15 )
			continue;

		vtEnchantOpt::iterator _It = m_vtEnchantOpt.find(enchantop);

		if( _It == m_vtEnchantOpt.end() )
			continue;

		oplevel -= 10;

		CUSTOM_OPTIONS_ENCHANT* popt = &_It->second;

		for(int n = 0; n <= oplevel; n++)
		{
			if( _It->second.iIndex1[n] != -1 )
				SetEnchantOption(lpObj,popt->iIndex1[n],popt->iValue1[n]);

			if( _It->second.iIndex2[n] != -1 )
				SetEnchantOption(lpObj,popt->iIndex2[n],popt->iValue2[n]);
		}
	}
}

void CNewOptions::SetEnchantOption(LPOBJ lpObj,int Option,int Value)
{
	switch( Option )
	{
	case ENCHANT_OPTION_CRITDMG:
		lpObj->m_CriticalDamageMul += Value;
		break;
	case ENCHANT_OPTION_EXCLDMG:
		lpObj->m_ExcelentDamageMul += Value;
		break;
	case ENCHANT_OPTION_DBLDMG:
		lpObj->SetOpDoubleDamage += Value;
		break;
	case ENCHANT_OPTION_IGNDMG:
		lpObj->SetOpIgnoreDefense += Value;
		break;
	}
}

#endif

#endif


