#pragma once

#include "TRandomPoolMgr.h"

struct _GAMBLING_ITEMOPTION
{
	_GAMBLING_ITEMOPTION()
	{
		btItemIndex = 0;
		btItemType = 0;
		btItemLevelMin = 0;
		btItemLevelMax = 0;
		btSkillOption = 0;
		btLuckOption = 0;
		btAddOption = 0;
		btExOption = 0;
	}

	BYTE btItemIndex;	// 0
	BYTE btItemType;	// 1
	BYTE btItemLevelMin;	// 2
	BYTE btItemLevelMax;	// 3
	BYTE btSkillOption;	// 4
	BYTE btLuckOption;	// 5
	BYTE btAddOption;	// 6
	BYTE btExOption;	// 7
};

struct _GAMBLING_RATE_STRUCT
{
	_GAMBLING_RATE_STRUCT()
	{
		iDropRate = 0;
		iSkillOptionRate = 0;
		iLuckOptionRate = 0;
		iAddOptionRate = 0;
		iExOptionRate = 0;
	}

	int iSkillOptionRate;
	int iLuckOptionRate;
	int iAddOptionRate;
	int iExOptionRate;
	int iDropRate;
};

#define MAX_GAMBLING_OPTION1	4
#define MAX_GAMBLING_OPTION2	5
#define MAX_GAMBLING_OPTION3	4
#define MAX_GAMBLING_OPTION4	6
#define MAX_GAMBLING_OPTION5	6
#define MAX_GAMBLING_GROUP	25


class CGamblingEvent
{
public:
	CGamblingEvent();
	virtual ~CGamblingEvent();

	void Init(char*szFileName);
	void LoadData(char* szFileName);

	BYTE GetLevel(int Index);
	BYTE GetExcOption(int Number);
	BYTE GetExcOption();
	int GetItemCount(){ return m_iCount; }

	BYTE BuyRequest(int aIndex,BYTE MapNumber,BYTE X,BYTE Y,BYTE Type);

protected:
	int m_bLoaded;

	_GAMBLING_RATE_STRUCT m_RateInfo[MAX_GAMBLING_GROUP];

	DWORD m_OptionRate[MAX_GAMBLING_OPTION1];
	DWORD m_Option2Rate[MAX_GAMBLING_OPTION2];
	DWORD m_Option3Rate[MAX_GAMBLING_OPTION3];
	DWORD m_Option4Rate[MAX_GAMBLING_OPTION4];
	DWORD m_Option5Rate[MAX_GAMBLING_OPTION5];

	int m_iCount;

	int m_iGroupItemCount[MAX_GAMBLING_GROUP];
	int m_iGroupCount;

	_GAMBLING_ITEMOPTION m_ItemList[150];

};

extern CGamblingEvent gGamblingEvent;