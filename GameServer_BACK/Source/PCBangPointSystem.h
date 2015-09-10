#if !defined(__PCBANGPOINTSYSTEM_H__)
#define __PCBANGPOINTSYSTEM_H__

#include "user.h"

class CGoblinPointSystem
{
public:
	CGoblinPointSystem();
	virtual ~CGoblinPointSystem();

	BOOL LoadData();
	BOOL UserProc(LPOBJ lpObj);

private:
	int m_bEnabled;
};

extern CGoblinPointSystem g_GoblinPointSystem;

static short GoblinPointCommonRuleTime;
static short GoblinPointCommonRuleTimePoint;
static short GoblinPointFeverStartHour;
static short GoblinPointFeverEndHour;
static short GoblinPointFeverMultipleValue;

#endif