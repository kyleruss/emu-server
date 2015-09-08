#include "StdAfx.h"
#include "TemporaryUserManager.h"
#include "ImperialGuardian.h"
#include "GameMain.h"
#include "BloodCastle.h"
#include "DevilSquare.h"
#include "ImperialGuardian.h"
#include "IllusionTempleEvent.h"
#include "..\common\winutil.h"
#include "DBSockMng.h"

CTemporaryUserManager* CTemporaryUserManager::m_pInstance;
CTemporaryUserManager* cpDSPTempUserManager = CTemporaryUserManager::GetInstance();
CTemporaryUserManager* cpTempUserManager = CTemporaryUserManager::GetInstance();
CTemporaryUserManager* cpTimerTempUserManager = CTemporaryUserManager::GetInstance();
int g_MaintainUserConnectionSecond = 300;

CTemporaryUserManager::CTemporaryUserManager()
{
	this->InitializeTempUserData();
	InitializeCriticalSection(&this->m_CritSect);
}


CTemporaryUserManager::~CTemporaryUserManager()
{
	DeleteCriticalSection(&this->m_CritSect);
}

CTemporaryUserManager* CTemporaryUserManager::GetInstance()
{
	if( CTemporaryUserManager::m_pInstance == 0 )
	{
		CTemporaryUserManager::m_pInstance = new CTemporaryUserManager;
		return CTemporaryUserManager::m_pInstance;
	}
	else
	{
		return CTemporaryUserManager::m_pInstance;
	}
}

void CTemporaryUserManager::InitializeTempUserData()
{
	memset(this->m_TempUserData,0,sizeof(this->m_TempUserData));
}

int CTemporaryUserManager::SearchEmptyIndex()
{
	for(int i = 0; i < OBJMAXUSER; i++)
	{
		if( this->m_TempUserData[i].m_iIndex == 0 )
			return i;
	}
return -1;
}

void CTemporaryUserManager::ClearSpecificUserData(int iSlotIndex)
{
	if( !TEMP_USER_CHECK(iSlotIndex) )
		return;

	if( !OBJMAX_RANGE(this->m_TempUserData[iSlotIndex].m_iIndex) )
		return;

	gObj[this->m_TempUserData[iSlotIndex].m_iIndex].Connected = 0;
	memset(&this->m_TempUserData[iSlotIndex],0,sizeof(_TempUserData));
}

int CTemporaryUserManager::AddUserData(char * Name,int UserIndex,int PartyNumber)
{
	EnterCriticalSection(&this->m_CritSect);
	int iSlot = this->SearchEmptyIndex();
	LeaveCriticalSection(&this->m_CritSect);

	if( iSlot == -1 )
		return 0;

	this->m_TempUserData[iSlot].m_dwTICK_COUNT = GetTickCount();
	memcpy(this->m_TempUserData[iSlot].szName,Name,sizeof(this->m_TempUserData[iSlot].szName));
	this->m_TempUserData[iSlot].m_iIndex = UserIndex;
	this->m_TempUserData[iSlot].m_Change = gObj[UserIndex].m_Change;
	this->m_TempUserData[iSlot].m_PartyNumber = PartyNumber;
	this->m_TempUserData[iSlot].m_MapNumber = gObj[UserIndex].MapNumber;
	this->m_TempUserData[iSlot].m_iIllusionTempleIndex = gObj[UserIndex].iTempleIndex;
	this->m_TempUserData[iSlot].m_iImperialGuardianZoneIndex = g_ImperialGuardian.GetCurrentZoneIndex(UserIndex);
	this->m_TempUserData[iSlot].m_iBloodCastleIndex = gObj[UserIndex].m_cBloodCastleIndex;
	this->m_TempUserData[iSlot].m_iBloodCastleSubIndex = gObj[UserIndex].m_cBloodCastleSubIndex;
	this->m_TempUserData[iSlot].m_iBloodCaslteExp = gObj[UserIndex].m_iBloodCastleEXP;
	this->m_TempUserData[iSlot].m_nEventScore = gObj[UserIndex].m_nEventScore;
	this->m_TempUserData[iSlot].m_nEventExp = gObj[UserIndex].m_nEventExp;
	this->m_TempUserData[iSlot].m_nEventMoney = gObj[UserIndex].m_nEventMoney;
	this->m_TempUserData[iSlot].m_bDevilSquareIndex = gObj[UserIndex].m_bDevilSquareIndex;
	this->m_TempUserData[iSlot].m_bDevilSquareAuth = gObj[UserIndex].m_bDevilSquareAuth;
	return true;
}

int CTemporaryUserManager::AddToPriviousParty(int UserIndex,int SlotIndex)
{
	if( !TEMP_USER_CHECK(SlotIndex) )
		return 0;

	if( strcmp(this->m_TempUserData[SlotIndex].szName,gObj[UserIndex].Name) )
		return 0;

	gParty.Delete(this->m_TempUserData[SlotIndex].m_PartyNumber,this->m_TempUserData[SlotIndex].m_iIndex,gObj[UserIndex].DBNumber);
	if( gParty.Add(this->m_TempUserData[SlotIndex].m_PartyNumber,UserIndex,gObj[UserIndex].DbClass,gObj[UserIndex].Level) < 0 )
	{
		GCServerMsgStringSend("Failed to add in party!",UserIndex,1);
	}
	else
	{
		gObj[UserIndex].PartyNumber = this->m_TempUserData[SlotIndex].m_PartyNumber;
		CGPartyListAll(this->m_TempUserData[SlotIndex].m_PartyNumber);
		gParty.PartyLog(this->m_TempUserData[SlotIndex].m_PartyNumber,UserIndex,2);
	}
	this->ClearSpecificUserData(SlotIndex);
	return 1;
}

int CTemporaryUserManager::ClearOverTimeUserData(unsigned long Second)
{
	DWORD dwCurrentTickCount = GetTickCount();

	for(int i = 0; i < OBJMAXUSER; i++)
	{
		if( this->m_TempUserData[i].m_iIndex != 0 &&
			(dwCurrentTickCount-this->m_TempUserData[i].m_dwTICK_COUNT) > Second*1000 )
		{
			int PartyNumber = this->m_TempUserData[i].m_PartyNumber;

			if( !OBJMAX_RANGE(PartyNumber) )
			{
				this->TidyUpTempuserdate(i);
				continue;
			}

			if( this->CheckInEventMap(i) == 0 ||
				this->CheckInEventMap(i) == 1 || 
				this->CheckInEventMap(i) == 5 )
			{
				gParty.Delete(PartyNumber,this->m_TempUserData[i].m_iIndex,gObj[this->m_TempUserData[i].m_iIndex].DBNumber);
				gParty.UpdatePKPartyPanalty(PartyNumber);
				CGPartyListAll(PartyNumber);

				if( gParty.GetCount(PartyNumber) < 2 )
				{
					for(int n = 0; n < MAX_USER_IN_PARTY; n++)
					{
						int number = gParty.m_PartyS[PartyNumber].Number[n];

						if( number >= 0 )
						{
							gParty.Delete(PartyNumber,n);
							gObj[number].PartyNumber = -1;
							gObj[number].PartyTargetUser = -1;
							GCPartyDelUserSend(number);
						}
					}

					gParty.Destroy(PartyNumber);
				}
			}

			this->TidyUpTempuserdate(i);
		}
	}
return true;
}

struct PMSG_TEMPUSER_TIDYUP
{
	PBMSG_HEAD h;
	char szName[11];
	WORD server;
};

int CTemporaryUserManager::TidyUpTempuserdate(int SlotIndex)
{
	if( !TEMP_USER_CHECK(SlotIndex) )
		return -1;


	int nPartyNumber = this->m_TempUserData[SlotIndex].m_PartyNumber;

	PMSG_TEMPUSER_TIDYUP pMsg;
	pMsg.h.set((LPBYTE)&pMsg,0xA6,sizeof(pMsg));
	memcpy(pMsg.szName,m_TempUserData[SlotIndex].szName,11);
	pMsg.server = -1;
	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));

	this->LeaveEvent(SlotIndex);
	this->ClearSpecificUserData(SlotIndex);
	return nPartyNumber;
}

void CTemporaryUserManager::DeleteUserInParty(int SlotIndex)
{
	if( !TEMP_USER_CHECK(SlotIndex) )
		return;

	int nPartyNumber = this->m_TempUserData[SlotIndex].m_PartyNumber;

	if( !OBJMAX_RANGE(nPartyNumber) )
		return;

	gParty.Delete(nPartyNumber,this->m_TempUserData[SlotIndex].m_iIndex,gObj[this->m_TempUserData[SlotIndex].m_iIndex].DBNumber);
	gParty.UpdatePKPartyPanalty(nPartyNumber);
	CGPartyListAll(nPartyNumber);

	if( gParty.GetCount(nPartyNumber) < 2 )
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			int number = gParty.m_PartyS[nPartyNumber].Number[n];
			
			if( number >= 0 )
			{
				gParty.Delete(nPartyNumber,n);
				gObj[number].PartyNumber = -1;
				gObj[number].PartyTargetUser = -1;
				GCPartyDelUserSend(number);
			}
		}

		gParty.Destroy(nPartyNumber);
	}

	this->TidyUpTempuserdate(SlotIndex);
}

int CTemporaryUserManager::DeleteAllPartyUserData(int PartyNumber)
{
	if( !OBJMAX_RANGE(PartyNumber) )
		return 0;

	for(int i = 0; i < OBJMAXUSER; i++)
	{
		if( this->m_TempUserData[i].m_PartyNumber == PartyNumber )
		{
			this->TidyUpTempuserdate(i);
		}
	}
	return true;
}

int CTemporaryUserManager::CheckMatchCharacter(char * CharacterName)
{
	for(int i = 0; i < OBJMAXUSER; i++)
	{
		if( this->m_TempUserData[i].m_iIndex != 0 )
		{
			if( !strcmp(CharacterName,this->m_TempUserData[i].szName) )
				return i;
		}
	}
return -1;
}

int CTemporaryUserManager::CheckMatchIndex(int UserIndex)
{
	for(int i = 0; i < OBJMAXUSER; i++)
	{
		if( this->m_TempUserData[i].m_iIndex == UserIndex )
			return i;
	}
return -1;
}

int CTemporaryUserManager::CheckInEventMap(int SlotIndex)
{
	if( !TEMP_USER_CHECK(SlotIndex) )
		return -1;

	if( BC_MAP_RANGE(this->m_TempUserData[SlotIndex].m_MapNumber) )
		return 1;

	if( IT_MAP_RANGE(this->m_TempUserData[SlotIndex].m_MapNumber) )
		return 2;

	if( IF_MAP_RANGE(this->m_TempUserData[SlotIndex].m_MapNumber) )
		return 3;

	// DG map check - 4

	if( DS_MAP_RANGE(this->m_TempUserData[SlotIndex].m_MapNumber) )
		return 5;

return 0;
}

void CTemporaryUserManager::LeaveEvent(int SlotIndex)
{
	int bResult = this->CheckInEventMap(SlotIndex);

	if( bResult == 1 )
	{

	}
	else if( bResult == 2 )
	{
		g_IllusionTemple.LeaveUser(this->m_TempUserData[SlotIndex].m_iIndex,
			this->m_TempUserData[SlotIndex].m_MapNumber);
	}
	else if( bResult == 3 )
	{

	}
	else if( bResult == 4 )
	{
		// doppleganger leave
	}
	else if( bResult == 5 )
	{
		g_DevilSquare.LeaveDevilSquare(this->m_TempUserData[SlotIndex].m_bDevilSquareIndex,
			this->m_TempUserData[SlotIndex].m_iIndex);
	}
}

void CTemporaryUserManager::SendEventMapInfo(int SlotIndex,int CurUserIndex)
{
	int bResult = this->CheckInEventMap(SlotIndex);

	if( bResult == 2 )
	{
		PMSG_ILLUSIONTEMPLE_STATESEND pMsg;
		PHeadSubSetB((LPBYTE)&pMsg,0xBF,0x09,sizeof(pMsg));

		pMsg.Temple = this->m_TempUserData[SlotIndex].m_iIllusionTempleIndex;
		pMsg.State = g_IllusionTemple.GetState(this->m_TempUserData[SlotIndex].m_MapNumber);

		DataSend(CurUserIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( bResult == 1 )
	{
		if( g_BloodCastle.GetPlayUserCountRightNow(this->m_TempUserData[SlotIndex].m_iBloodCastleIndex) < 1 )
			return;

		int nCastleBlockInfo = g_BloodCastle.GetCastleBlockInfo(this->m_TempUserData[SlotIndex].m_iBloodCastleIndex);

		if( nCastleBlockInfo >= 1001 )
			g_BloodCastle.SendCastleEntranceBlockInfo(this->m_TempUserData[SlotIndex].m_iBloodCastleIndex,0);

		if( nCastleBlockInfo >= 1002 )
			g_BloodCastle.SendCastleBridgeBlockInfo(this->m_TempUserData[SlotIndex].m_iBloodCastleIndex,0);

		if( nCastleBlockInfo == 1003 )
			g_BloodCastle.SendCastleDoorBlockInfo(this->m_TempUserData[SlotIndex].m_iBloodCastleIndex,0);

		int nCastleState = g_BloodCastle.GetCurrentState(this->m_TempUserData[SlotIndex].m_iBloodCastleIndex);

		if( nCastleState == BC_STATE_PLAYING )
		{
			g_BloodCastle.SendNoticeMessageToSpecificUser(this->m_TempUserData[SlotIndex].m_iBloodCastleIndex,CurUserIndex,0);
		}
		return;
	}

	if( bResult == 4 )
	{
		// dopleganger
		return;
	}

	if( bResult == 5 )
	{
		return;
	}

	if( bResult == 3 )
	{
		if( g_ImperialGuardian.GetPlayUserCountRightNow(this->m_TempUserData[SlotIndex].m_iImperialGuardianZoneIndex) < 1 )
			return;

		g_ImperialGuardian.SendCurStateToUser(CurUserIndex,this->m_TempUserData[SlotIndex].m_iImperialGuardianZoneIndex);
		g_ImperialGuardian.SendCurGateInfoToUser(CurUserIndex,this->m_TempUserData[SlotIndex].m_iImperialGuardianZoneIndex);
	}
}

int CTemporaryUserManager::ResumeEvent(int SlotIndex,int CurUserIndex)
{
	if( !TEMP_USER_CHECK(SlotIndex) )
		return 0;

	int bResult = this->CheckInEventMap(SlotIndex);

	if( bResult == 1 )
	{
		return this->ResumeBloodCastle(SlotIndex,CurUserIndex);
	}
	else if( bResult == 2 )
	{
		return this->ResumeIllusionTemple(SlotIndex,CurUserIndex);
	}
	else if( bResult == 3 )
	{
		return this->ResumeImperialguardian(SlotIndex,CurUserIndex);
	}
	else if( bResult == 4 )
	{
		return this->ResumeDoppelganger(SlotIndex,CurUserIndex);
	}
	else if( bResult == 5 )
	{
		return this->ResumeDevilsquare(SlotIndex,CurUserIndex);
	}

	return 0;
}

int CTemporaryUserManager::ResumeIllusionTemple(int SlotIndex,int CurUserIndex)
{
	if( !TEMP_USER_CHECK(SlotIndex) )
		return 0;

	if( g_IllusionTemple.GetState(this->m_TempUserData[SlotIndex].m_MapNumber) == IT_STATE_PLAYEND )
		return 0;

	int bResult = g_IllusionTemple.ChangeTempleUserInformation(this->m_TempUserData[SlotIndex].m_iIndex,
		CurUserIndex,this->m_TempUserData[SlotIndex].m_iIllusionTempleIndex,
		this->m_TempUserData[SlotIndex].m_MapNumber);

	if( bResult == 0 )
		return 0;

	gObj[CurUserIndex].iTempleIndex = this->m_TempUserData[SlotIndex].m_iIllusionTempleIndex;

	if( g_IllusionTemple.GetState(this->m_TempUserData[SlotIndex].m_MapNumber) > IT_STATE_CLOSED )
	{
		PMSG_ILLUSIONTEMPLE_STATESEND pMsg;
		PHeadSubSetB((LPBYTE)&pMsg,0xBF,0x09,sizeof(pMsg));

		pMsg.Temple = this->m_TempUserData[SlotIndex].m_iIllusionTempleIndex;
		pMsg.State = IT_STATE_PREPARE;
		DataSend(CurUserIndex,(LPBYTE)&pMsg,pMsg.h.size);

		LogAddTD("SkillPoint : %d, Name : %s",
			g_IllusionTemple.GetSkillPoint(CurUserIndex,this->m_TempUserData[SlotIndex].m_MapNumber),
			this->m_TempUserData[SlotIndex].szName);

		GCIllusionSendPoint(CurUserIndex,g_IllusionTemple.GetSkillPoint(CurUserIndex,this->m_TempUserData[SlotIndex].m_MapNumber));
	}
	return bResult;
}

int CTemporaryUserManager::ResumeImperialguardian(int SlotIndex,int CurUserIndex)
{
	if( !TEMP_USER_CHECK(SlotIndex) )
		return 0;

	if( g_ImperialGuardian.GetPlayUserCountRightNow(this->m_TempUserData[SlotIndex].m_iImperialGuardianZoneIndex) < 1 )
		return 0;

	int bResult = g_ImperialGuardian.ChangeUserIndex(this->m_TempUserData[SlotIndex].m_iIndex,CurUserIndex,
		this->m_TempUserData[SlotIndex].m_iImperialGuardianZoneIndex);

	return bResult;
}

int CTemporaryUserManager::ResumeBloodCastle(int SlotIndex,int CurUserIndex)
{
	if( !TEMP_USER_CHECK(SlotIndex) )
		return 0;

	if( g_BloodCastle.GetPlayUserCountRightNow(this->m_TempUserData[SlotIndex].m_iBloodCastleIndex) < 1 )
		return 0;

	int bResult = g_BloodCastle.ChangeUserIndex(this->m_TempUserData[SlotIndex].m_iIndex,CurUserIndex,
		this->m_TempUserData[SlotIndex].m_iBloodCastleIndex);

	if( bResult == TRUE )
	{
		gObj[CurUserIndex].m_cBloodCastleIndex = this->m_TempUserData[SlotIndex].m_iBloodCastleIndex;
		gObj[CurUserIndex].m_cBloodCastleSubIndex = this->m_TempUserData[SlotIndex].m_iBloodCastleSubIndex;
		gObj[CurUserIndex].m_iBloodCastleEXP = this->m_TempUserData[SlotIndex].m_iBloodCaslteExp;
	}
	return bResult;
}

int CTemporaryUserManager::ResumeDoppelganger(int SlotIndex,int CurUserIndex)
{
	return 0;
}

int CTemporaryUserManager::ResumeDevilsquare(int SlotIndex,int CurUserIndex)
{
	if( !TEMP_USER_CHECK(SlotIndex) )
		return 0;

	if( g_DevilSquare.GetPlayUserCountRightNow(this->m_TempUserData[SlotIndex].m_bDevilSquareIndex) < 1 )
		return 0;

	if( g_DevilSquare.GetState() == DevilSquare_CLOSE )
		return 0;

	gObj[CurUserIndex].m_nEventScore = this->m_TempUserData[SlotIndex].m_nEventScore;
	gObj[CurUserIndex].m_nEventExp = this->m_TempUserData[SlotIndex].m_nEventExp;
	gObj[CurUserIndex].m_nEventMoney = this->m_TempUserData[SlotIndex].m_nEventMoney;
	gObj[CurUserIndex].m_bDevilSquareIndex = this->m_TempUserData[SlotIndex].m_bDevilSquareIndex;
	gObj[CurUserIndex].m_bDevilSquareAuth = this->m_TempUserData[SlotIndex].m_bDevilSquareAuth;

	int bResult = g_DevilSquare.ChangeUserIndex(this->m_TempUserData[SlotIndex].m_iIndex,CurUserIndex,gObj[CurUserIndex].m_bDevilSquareIndex);
	return bResult;
}

void TidyUpTempuserdate(int DeletedUserIndex,int PartyNumber,BYTE UserNumberInParty)
{
	if( UserNumberInParty == 0 )
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			int iUserIndex = gParty.m_PartyS[PartyNumber].Number[n];

			if( iUserIndex == -1 )
				continue;

			int iMatchUserSlotIndex = cpTempUserManager->CheckMatchIndex(iUserIndex);

			if( iMatchUserSlotIndex == -1 )
				continue;

			if( cpTempUserManager->CheckInEventMap(iMatchUserSlotIndex) == 3 ||
				cpTempUserManager->CheckInEventMap(iMatchUserSlotIndex) == 4 ||
				cpTempUserManager->CheckInEventMap(iMatchUserSlotIndex) == 2 )
				continue;

			cpTempUserManager->TidyUpTempuserdate(iMatchUserSlotIndex);
		}
	}
	else
	{
		int iSlotIndex = cpTempUserManager->CheckMatchIndex(DeletedUserIndex);

		if( iSlotIndex == -1 )
		{

		}
		else
		{
			cpTempUserManager->TidyUpTempuserdate(iSlotIndex);
		}
	}
}