#pragma once

#include "user.h"

#define TEMP_USER_CHECK(value) (( value < 0 ) ? FALSE : (value <= (OBJMAXUSER-1)))

struct _TempUserData
{
	DWORD m_dwTICK_COUNT;
	char szName[11];
	int m_iIndex;
	int m_Change;
	int m_PartyNumber;
	BYTE m_MapNumber;
	int m_iBloodCastleIndex;
	int m_iBloodCastleSubIndex;
	int m_iBloodCaslteExp;
	int m_iIllusionTempleIndex;
	int m_iImperialGuardianZoneIndex;
	int m_nEventScore;
	int m_nEventExp;
	int m_nEventMoney;
	BYTE m_bDevilSquareIndex;
	BYTE m_bDevilSquareAuth;
};

class CTemporaryUserManager
{
public:
	CTemporaryUserManager();
	~CTemporaryUserManager();

	static CTemporaryUserManager* GetInstance();
	static CTemporaryUserManager* m_pInstance;

	int AddUserData(char * Name,int UserIndex,int PartyNumber);
	int AddToPriviousParty(int UserIndex,int SlotIndex);
	int ClearOverTimeUserData(unsigned long Second);
	int TidyUpTempuserdate(int SlotIndex);
	void DeleteUserInParty(int SlotIndex);
	int DeleteAllPartyUserData(int PartyNumber);
	int CheckMatchCharacter(char * CharacterName);
	int CheckMatchIndex(int UserIndex);
	void SendEventMapInfo(int SlotIndex,int CurUserIndex);
	int ResumeEvent(int SlotIndex,int CurUserIndex);
	int ResumeIllusionTemple(int SlotIndex,int CurUserIndex);
	int ResumeImperialguardian(int SlotIndex,int CurUserIndex);
	int ResumeBloodCastle(int SlotIndex,int CurUserIndex);
	int ResumeDoppelganger(int SlotIndex,int CurUserIndex);
	int ResumeDevilsquare(int SlotIndex,int CurUserIndex);

	int CheckInEventMap(int SlotIndex);
	void LeaveEvent(int SlotIndex);

private:

	int SearchEmptyIndex();

	void InitializeTempUserData();
	void ClearSpecificUserData(int iSlotIndex);

public:
	_TempUserData m_TempUserData[OBJMAXUSER];
	CRITICAL_SECTION m_CritSect;
};

void TidyUpTempuserdate(int DeletedUserIndex,int PartyNumber,BYTE UserNumberInParty);

extern CTemporaryUserManager* cpDSPTempUserManager;
extern CTemporaryUserManager* cpTempUserManager;
extern CTemporaryUserManager* cpTimerTempUserManager;
extern int g_MaintainUserConnectionSecond;