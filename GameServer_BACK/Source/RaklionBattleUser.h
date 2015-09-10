// RaklionBattleUser.h: interface for the CRaklionBattleUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAKLIONBATTLEUSER_H__4D0FA22D_D5C5_40C5_A262_3FD0C1CE3C1B__INCLUDED_)
#define AFX_RAKLIONBATTLEUSER_H__4D0FA22D_D5C5_40C5_A262_3FD0C1CE3C1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRaklionBattleUser  
{
public:
public:
	CRaklionBattleUser()
	{
		this->ResetData();
	}

	virtual ~CRaklionBattleUser()
	{
		return;
	}
	
	void ResetData()
	{
		this->m_iIndex = -1;
		this->m_bIsUse = FALSE;
	}

	int GetIndex()
	{
		return this->m_iIndex;
	}

	void SetIndex(int iIndex)
	{
		this->m_iIndex = iIndex;
		this->m_bIsUse = TRUE;
	}

	BOOL IsUseData()
	{
		return this->m_bIsUse;
	}

private:

	int m_iIndex;
	BOOL m_bIsUse;

};

#endif // !defined(AFX_RAKLIONBATTLEUSER_H__4D0FA22D_D5C5_40C5_A262_3FD0C1CE3C1B__INCLUDED_)
