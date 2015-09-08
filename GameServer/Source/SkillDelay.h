// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
#ifndef SKILLDELAY_H
#define SKILLDELAY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicDamage.h"

class CSkillDelay
{

public:

	CSkillDelay();
	virtual ~CSkillDelay();

	void Init();
	BOOL Check(BYTE skill);

private:

	DWORD LastSkillUseTime[MAX_SKILL];	// 4
};

#endif