// RaklionBattleUserMng.cpp: implementation of the CRaklionBattleUserMng class.
// GS	1.00.90	JPN	0xXXXXXXXX	-	Completed
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaklionBattleUserMng.h"
#include "LogProc.h"
#include "User.h"
#include "Raklion.h"
#include "RaklionUtil.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRaklionBattleUserMng g_RaklionBattleUserMng;

CRaklionBattleUserMng::CRaklionBattleUserMng()
{
	this->ResetAllData();
}

CRaklionBattleUserMng::~CRaklionBattleUserMng()
{

}

void CRaklionBattleUserMng::ResetAllData() //006449C0 (identical)
{
	while((BYTE)this->m_vtUserInfo.empty() == 0) //same as TRUE
	{
		this->m_vtUserInfo.pop_back();
		//maybe LogAddTD for debug because after there is a clear lol
	}

	this->m_vtUserInfo.clear(); //good
}

BOOL CRaklionBattleUserMng::AddUserData(int iIndex) //00644A10 (identical)
{
	if(gObjIsConnected(iIndex) == FALSE)
	{
		LogAddTD("[ RAKLION ][ BattleUser ] Add User Fail - Disconnect User [%s][%s]",
			gObj[iIndex].AccountID, gObj[iIndex].Name);
		return FALSE;
	}

	this->m_vtUserInfo.push_back(iIndex);

	g_RaklionUtil.NotifyRaklionCurrentState(iIndex, g_Raklion.GetRaklionState(), g_Raklion.GetRaklionDetailState());

	LogAddTD("[ RAKLION ][ Battle User ] Add User [%s][%s] Current Battle User:%d",
				gObj[iIndex].AccountID, gObj[iIndex].Name, this->m_vtUserInfo.size());

	return TRUE;
}

BOOL CRaklionBattleUserMng::DeleteUserData(int iIndex) //00644B20 (identical)
{
	if( iIndex < 0 || iIndex > OBJMAX-1 )
	{
		LogAddC(2, "[ RAKLION ][ BattleUser ] Delete User Fail - Unvalid Index:%d", iIndex);
		return FALSE;
	}

	for( int i = 0; i < this->m_vtUserInfo.size(); i++)
	{
		if(this->m_vtUserInfo[i] == iIndex)
		{
			this->m_vtUserInfo.erase(this->m_vtUserInfo.begin()+i);

			LogAddTD("[ RAKLION ][ BattleUser ] Delete User - [%s][%s] Current Battle User:%d",
					gObj[iIndex].AccountID, gObj[iIndex].Name, this->m_vtUserInfo.size());
				return TRUE;
		}
	}

	return FALSE;
}

void CRaklionBattleUserMng::CheckUserState() {
	for (int iCount = 0; iCount < m_vtUserInfo.size(); iCount++) {
		int nIndex = m_vtUserInfo[iCount];
		if (!gObjIsConnected(nIndex)) {
			LogAddTD("[ RAKLION ][ BattleUser ] Delete User - Disconnect [%s][%s]",
				gObj[nIndex].AccountID, gObj[nIndex].Name);
			DeleteUserData(nIndex);
		}
		if (gObj[nIndex].MapNumber != MAP_INDEX_RAKLIONBOSS && gObj[nIndex].m_State == 2 
			&& gObj[nIndex].Live == 1 && gObj[nIndex].SkillRecallParty_MapNumber != MAP_INDEX_RAKLIONBOSS) {
			LogAddTD("[ RAKLION ][ BattleUser ] Delete User - Map Move [%s][%s]",
				gObj[nIndex].AccountID, gObj[nIndex].Name);
			DeleteUserData(nIndex);
		}
	}
}

BOOL CRaklionBattleUserMng::MoveAllUser(int iGateNumber) //00644EB0 (identical)
{
	BOOL bMoveGateSuccess = FALSE; //loc2

	for ( int iCount=0; iCount < this->m_vtUserInfo.size(); iCount++)
	{
		bMoveGateSuccess = gObjMoveGate(this->m_vtUserInfo[iCount], iGateNumber);

		if ( bMoveGateSuccess == TRUE )
		{
			LogAddTD("[ RAKLION ][ BattleUser ] [%s][%s] MoveGate(%d)",
				gObj[this->m_vtUserInfo[iCount]].AccountID, 
				gObj[this->m_vtUserInfo[iCount]].Name, iGateNumber);
		}
		else
		{
			this->DeleteUserData(this->m_vtUserInfo[iCount]);

			LogAddTD( "[ RAKLION ][ BattleUser ] [%s][%s] MoveGate Fail (%d)",
				gObj[this->m_vtUserInfo[iCount]].AccountID, 
				gObj[this->m_vtUserInfo[iCount]].Name, iGateNumber);

			gObjMoveGate(this->m_vtUserInfo[iCount], 286); //Raklion Entrance
		}
	}

	return TRUE;
}

int CRaklionBattleUserMng::GetBattleUserCount() //00645070 (identical)
{
	return this->m_vtUserInfo.size();
}

int CRaklionBattleUserMng::CheckBattleUsers() //006450A0 (identical)
{
	//back = wrong
	//begin = wrong
	//end = maybe wrong
	//front = wrong
	//clear = wrong
	//capacity = wrong
	return (BYTE)this->m_vtUserInfo.empty();
}

void CRaklionBattleUserMng::SetBattleUserIndex(int arg1) //006450D0 (identical) 
{
#if (DEBUG_RAKLION == 1)
	this->m_vtUserInfo.push_back(arg1);	//
#endif
}

int CRaklionBattleUserMng::GetBattleUserIndex(int iIndex) //006450F0 (identical)
{
	int UserIndex = this->m_vtUserInfo[iIndex];
	return UserIndex;
}

BOOL CRaklionBattleUserMng::IsBattleUser(int iIndex) //00645130 (identical)
{
	for(int i = 0; i < this->GetBattleUserCount(); i++)
	{
		if(iIndex == this->m_vtUserInfo[i])
		{
			return TRUE;
		}
	}

	return FALSE;
}

//2 new function here but it's used by the vector (generated automatically :D)