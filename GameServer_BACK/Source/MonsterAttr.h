#pragma once

#include "user.h"

typedef struct MONSTER_ATTRIBUTE
{
  int m_Index;	// 0
  int m_Rate;	// 4
  char m_Name[60];	// 8
  int m_Level;	// 1C
  int m_iAINumber;	// 20
  int m_iScriptHP;	// 24
  int m_Hp;	// 28
  int m_Mp;	// 2C
  int m_DamageMin;	// 30
  int m_DamageMax;	// 34
  int m_Defense;	// 38
  int m_MagicDefense;	// 3C
  int m_AttackRating;	// 40
  int m_Successfulblocking;	// 44

  int m_ElementDmgMin;
  int m_ElementDmgMax;
  int m_ElementDefense;
  int m_ElementAttackRating;
  int m_ElementBlockRating;

  int m_MoveRange;	// 48
  int m_AttackRange;	// 4C
  int m_AttackType;	// 50
  int m_ViewRange;	// 54
  int m_MoveSpeed;	// 58
  int m_AttackSpeed;	// 5C
  int m_RegenTime;	// 60
  int m_Attribute;	// 64
  int m_ItemRate;	// 68
  int m_MoneyRate;	// 6C
  int m_MaxItemLevel;	// 70
  BYTE m_Resistance[MAX_RESISTENCE_TYPE];	// 74
  DWORD m_MonsterSkill;	// 7C

} MONSTER_ATTRIBUTE, * LPMONSTER_ATTRIBUTE;


class CMonsterAttr
{
public:
	CMonsterAttr();
	virtual ~CMonsterAttr();

	LPMONSTER_ATTRIBUTE GetAttr(int aClass);
	void LoadAttr(LPSTR File);

private:
	//MONSTER_ATTRIBUTE m_MonsterAttr[700];
	std::vector<MONSTER_ATTRIBUTE> m_MonsterAttr;
};



struct CUSTOM_MONSTER_ATTR
{
	float HPMul;
	float DmgMul;
	float DefMul;
	float ARateMul;
	float DRateMul;
};

class CCustomMonsterAttr
{
public:
	CCustomMonsterAttr();
	virtual ~CCustomMonsterAttr();

	void Load(char* szFileName);

public:
	CUSTOM_MONSTER_ATTR m_DefaultData;
	std::map<int,CUSTOM_MONSTER_ATTR*> m_MonsterData;
}; extern CCustomMonsterAttr gCMAttr;
