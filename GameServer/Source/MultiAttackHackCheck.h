#pragma once

#include "user.h"

#define MAX_MULTI_ATTACK_INFO 40
#define MULTI_ATTACK_SERIAL_SIZE 255

struct MULTIATTACK_HACKCHECK
{
	short number;	// 0
	BYTE Serial[MULTI_ATTACK_SERIAL_SIZE];	// 2
};


class CMultiAttackHackCheck
{

public:

	CMultiAttackHackCheck();
	virtual ~CMultiAttackHackCheck();

	void Init();
	int Insert(int aTargetIndex, BYTE skillnum, BYTE serial);
	int CheckPenetrationSkill(int aTargetIndex, BYTE skillnum, BYTE serial);
	int CheckFireScreamSkill(int iTargetIndex, BYTE btSkillnum, BYTE btSerial);


private:

	int m_iCount;	// 4
	MULTIATTACK_HACKCHECK m_Table[MAX_MULTI_ATTACK_INFO];	// 8

};

#if (ENABLETEST_FORCEOPTIMIZE == 0)
extern CMultiAttackHackCheck gMultiAttackHackCheck[OBJMAX];
#endif