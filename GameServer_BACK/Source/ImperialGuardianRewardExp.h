#pragma once

#include "EventItemBag.h"

struct _stRewardExpInfo
{
	_stRewardExpInfo()
	{
		m_MinLevel = 0;
		m_MaxLevel = 0;
		m_iRewardExp = 0;

		pEventDugeonItemBagGaion = NULL;
		pEventDugeonItemBagStone = NULL;
		pEventDugeonItemBag = NULL;
	}

	int m_MinReset;
	int m_MaxReset;
	int m_MinLevel;
	int m_MaxLevel;
	int m_iRewardExp;

	EventItemBag* pEventDugeonItemBagGaion;
	EventItemBag* pEventDugeonItemBagStone;
	EventItemBag* pEventDugeonItemBag;
};

class CImperialGuardianRewardExp
{
public:
	CImperialGuardianRewardExp();
	virtual ~CImperialGuardianRewardExp();

	void Init();
	void LoadScript(char* lpFileName);
	
	int GetRewardExp(int nUserIndex);
	void DropReward(int nUsedIndex,int iRewardType);
private:
	BYTE m_bLoaded;
	std::vector<_stRewardExpInfo> m_vtRewardExpInfo;
};