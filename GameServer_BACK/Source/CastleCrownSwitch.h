// CastleCrownSwitch.h: interface for the CCastleCrownSwitch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASTLECROWNSWITCH_H__E1DF5DA3_E594_4900_AF02_A71558C39951__INCLUDED_)
#define AFX_CASTLECROWNSWITCH_H__E1DF5DA3_E594_4900_AF02_A71558C39951__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (GS_CASTLE==1)


class CCastleCrownSwitch  
{

public:

	CCastleCrownSwitch();
	virtual ~CCastleCrownSwitch();

	void CastleCrownSwitchAct(int iIndex);

};

extern CCastleCrownSwitch g_CsNPC_CastleCrownSwitch;

#define CROWNSWITCH_CLASS_RANGE(x)  ( ((x)< 217 )?FALSE:((x) > 218 )?FALSE:TRUE   ) 

#endif

#endif // !defined(AFX_CASTLECROWNSWITCH_H__E1DF5DA3_E594_4900_AF02_A71558C39951__INCLUDED_)
