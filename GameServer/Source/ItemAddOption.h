// ItemAddOption.h: interface for the CItemAddOption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMADDOPTION_H__96FEC421_39C3_40DD_9D47_EE1D86DD8A21__INCLUDED_)
#define AFX_ITEMADDOPTION_H__96FEC421_39C3_40DD_9D47_EE1D86DD8A21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ITEM_ADD_OPTION	100

#include "user.h"

#define ADD_OPTION_ATTACK_SPEED			1
#define ADD_OPTION_ATTACK_DAMAGE		2
#define ADD_OPTION_DEFENSE				3
#define ADD_OPTION_LIFE					4
#define ADD_OPTION_MANA					5
#define ADD_OPTION_EXPERIENCE			6
#define ADD_OPTION_DROP_RATE			7
#define ADD_OPTION_REMOVEEXP			8
#define ADD_OPTION_STRENGTH				9
#define ADD_OPTION_DEXTERITY			10
#define ADD_OPTION_VITALITY				11
#define ADD_OPTION_ENERGY				12
#define ADD_OPTION_LEADERSHIP			13
#define ADD_OPTION_WEAPON_DAMAGE		14
#define ADD_OPTION_MAGIC_DAMAGE			15
#define ADD_OPTION_UNK					16
#define ADD_OPTION_CRITICALDMG			17
#define ADD_OPTION_REDUCE_HP			18
#define ADD_OPTION_REDUCE_HPPERC		19
#define ADD_OPTION_REDUCEMOVEMENT		20
#define ADD_OPTION_MSDECREASE			21
#define ADD_OPTION_MAGICDEFENSE			22
#define ADD_OPTION_DEFENSEDOWN			23
#define ADD_OPTION_REFLECTDAMAGE		24
#define ADD_OPTION_SLEEPTIME			25
#define ADD_OPTION_ATTACKRATE			26
#define ADD_OPTION_MSTIME				28
#define ADD_OPTION_WEAKNESS				29
#define	ADD_OPTION_INNOVATION			30
#define ADD_OPTION_BERSERKMANA_PERC		31
#define ADD_OPTION_BERSERKLIFE_PERC		32
#define ADD_OPTION_MAGICDAMAGEMIN		33
#define ADD_OPTION_MAGICDAMAGEMAX		34
#define ADD_OPTION_REFILLHPPERC			53
#define ADD_OPTION_REFILLMPPERC			54
#define ADD_OPTION_INCCRITDMG			55
#define ADD_OPTION_INCEXCDMG			56
#define ADD_OPTION_SANTABUFF			57
#define ADD_OPTION_SANTADEFENSE			58
#define ADD_OPTION_SANTABPRECOVERY		59
#define ADD_OPTION_CRITICALRATE			60
#define ADD_OPTION_EXCRATE				61
#define ADD_OPTION_MAXAGBYLEVEL			66
#define ADD_OPTION_MAXSDBYLEVEL			67
#define ADD_OPTION_DEFENSEIGNORE		72
#define ADD_OPTION_INCRVITALITY			73
#define ADD_OPTION_DEFRATE				74
#define ADD_OPTION_MINWIZDMGPERC		90
#define ADD_OPTION_ADDAGRECOVERY		91
#define ADD_OPTION_IGNOREDEF			101
#define ADD_OPTION_ALLSTATS				103
#define ADD_OPTION_ABSORBHP				104
#define ADD_OPTION_DECDEFRATE			105
#define ADD_OPTION_DECATTACKRATE		106

#define ADD_OPTION_SPEED		 1
#define ADD_OPTION_ATTACK_DAMAGE 2
#define ADD_OPTION_DEFENSE		 3
#define ADD_OPTION_LIFE			 4
#define ADD_OPTION_MANA			 5
#define ADD_OPTION_EXPERIENCE	 6
#define ADD_OPTION_DROP_RATE	 7
#define ADD_OPTION_SUSTENANCE	 8
#define ADD_OPTION_STRENGTH		 9
#define ADD_OPTION_DEXTERITY	10
#define ADD_OPTION_VITALITY		11
#define ADD_OPTION_ENERGY		12
#define ADD_OPTION_LEADERSHIP	13
#define ADD_OPTION_WRATH		14
#define ADD_OPTION_WIZARDRY		15
#define ADD_OPTION_MOBILITY		16
#define ADD_OPTION_WIZARD_DEFENSE			21
#define ADD_OPTION_MAGIC_DEFENSE			22
#define ADD_OPTION_REDUCE_DEFENSE			23
#define ADD_OPTION_REFLECT					24
#define SUB_OPTION_ATTACK_RATE				26
#define ADD_OPTION_WIZARD_DEFENSE_DURATION	28
#define ADD_OPTION_BERSERKMANA				31
#define ADD_OPTION_BERSERKLIFE				32
#define ADD_OPTION_MAGICDAMAGEMIN			33
#define ADD_OPTION_MAGICDAMAGEMAX			34
#define ADD_OPTION_ATTACK_NORMAL_DAMAGE		51
#define ADD_OPTION_MONSTER_DAMAGE_ABSORB	52
#define ADD_OPTION_LIFE_AUTO_RECOVERY		53
#define ADD_OPTION_MANA_AUTO_RECOVERY		54
#define ADD_OPTION_CRITICAL_DAMAGE		55
#define ADD_OPTION_EXCELLENT_DAMAGE		56
#define ADD_OPTION_XMASATTACK			57
#define ADD_OPTION_XMASDEFENSE			58
#define ADD_OPTION_LEVEL_BP				66
#define ADD_OPTION_LEVEL_SD				67
#define ADD_OPTION_BP					71
#define ADD_OPTION_SD					70
#define ADD_OPTION_SUCCESSFULBLOCKING	74
#define SUB_OPTION_SUCCESSFULBLOCKING	75

typedef struct ITEMEFFECT
{
	WORD wOptionType;	// 0
	WORD wEffectType1;	// 2
	WORD wEffectType2;	// 4
	int iItemNumber;	// 8
	int iItemType;	// C
	int iItemIndex;	// 10
	int iEffectValue1;	// 14
	int iEffectValue2;	// 18
	int iEffectValidTime;	// 1C

} ITEMEFFECT, * LPITEMEFFECT;

class CItemAddOption
{

public:

	CItemAddOption();
	virtual ~CItemAddOption();

	void Load(PCHAR chFileName);
	void Initialize();
	LPITEMEFFECT GetCurrentEffectType(LPOBJ lpObj);
	bool PrevSetItemLastEffectForHallowin(LPOBJ lpObj);
	bool NextSetItemLastEffectForHallowin(LPOBJ lpObj);
	void SendItemUse(LPOBJ lpObj, int iItemNumber, int iOptionType, int iEffectType, int iEffectValue, int iValidTime);
	bool SetItemEffect(LPOBJ lpObj, int iItemNumber, int iItemUseTime);
	bool ClearItemEffect(LPOBJ lpObj, int iEffectOption);
	BYTE ClearItemEffect(LPOBJ lpObj, int iItemCode,int iEffectOption);
	bool SearchItemEffectType(int iItemNumber, int* iEffectType1, int* iEffectType2, int* iItemPeriodDate);
	int SearchItemOption(int iItemNumber);
	BYTE SetPeriodItemEffect1(LPOBJ lpObj);
	BYTE SetPeriodItemEffect2(LPOBJ lpObj);
	BYTE IsItemEffect(int iItemCode);
	LPITEMEFFECT GetItemEffect(int iItemCode);

private:

	LPITEMEFFECT _SearchItemEffect(int iItemNumber);
	bool _SetItemEffect(LPOBJ lpObj, int iEffectType, int iEffectValue);
	bool _ClearItemEffect(LPOBJ lpObj, int iEffectOption, int iOptionType, int iEffectType, int iEffectValue);
	
private:

	ITEMEFFECT m_ItemAddOption[MAX_ITEM_ADD_OPTION];
};

extern CItemAddOption g_ItemAddOption;

#endif // !defined(AFX_ITEMADDOPTION_H__96FEC421_39C3_40DD_9D47_EE1D86DD8A21__INCLUDED_)
