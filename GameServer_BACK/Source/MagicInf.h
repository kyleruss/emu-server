#ifndef MAGICINF_H
#define MAGICINF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicDamage.h"


#define MAX_MAGIC_INFO	3

#define	DBM_TYPE	0
#define DBM_LEVEL	1
#define DBM_TYPE2	2



class CMagicInf
{

public:

	CMagicInf();
	virtual ~CMagicInf();
	
	int IsMagic();
	void Clear();
	int Set(int aSkill, BYTE aLevel);
	int Set(BYTE aType, BYTE aIndex, BYTE aLevel);
	int GetDamage();
	int SetMasterSkill(int aSkill,BYTE aLevel);
	int UpdateMasterSkill(int iSkill,unsigned char btLevel);
	
	CMagicInf & operator = (CMagicInf & __that)
	{
		m_Level = __that.m_Level;
		m_Skill = __that.m_Skill;
		m_DamageMin = __that.m_DamageMin;
		m_DamageMax = __that.m_DamageMax;

		return *this;
	};

public:

	BYTE m_Level;	// 4
	int m_Skill;	// 8
	int m_DamageMin;	// C
	int m_DamageMax;	// 10
	BYTE m_bBrand;	// 14
};

extern CMagicInf DefMagicInf[MAX_SKILL-1];

int GetSkillNumberInex(int type, int Index, int level);
void MagicByteConvert(unsigned char* buf, CMagicInf* const Magici, int maxmagic);

#endif