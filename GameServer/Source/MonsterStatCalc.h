#pragma once

#include "user.h"

struct _stMonsterStatRef
{
	_stMonsterStatRef()
	{
		m_MinReset = 0;
		m_MaxReset = 0;
		m_MinLevel = 0;
		m_MaxLevel = 0;
		m_LevelMul = 0;
		m_LifeMul = 0;
		m_ManaMul = 0;
		m_AttackMinMul = 0;
		m_AttackMaxMul = 0;
		m_DefenseMul = 0;
		m_MagicDefenseMul = 0;
		m_AttackRatingMul = 0;
		m_SuccessfulBlockingMul = 0;
	}

	int m_MinReset;
	int m_MaxReset;
	float m_MinLevel;
	float m_MaxLevel;
	float m_LevelMul;
	float m_LifeMul;
	float m_ManaMul;
	float m_AttackMinMul;
	float m_AttackMaxMul;
	float m_DefenseMul;
	float m_MagicDefenseMul;
	float m_AttackRatingMul;
	float m_SuccessfulBlockingMul;
};

class CMonsterStatCalc
{
public:
	CMonsterStatCalc();
	virtual ~CMonsterStatCalc();

	void LoadScript(LPSTR szFileName);
	int ConvertMonsterStat(LPOBJ lpObj, int nUserMaxLevel, int nUserMaxReset);

private:
	int m_bLoaded;
	std::vector<_stMonsterStatRef> m_vtMonsterStatRef;
};

extern CMonsterStatCalc g_MonsterStatCalc;