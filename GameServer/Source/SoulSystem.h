// SoulSystem.h: interface for the SoulSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOULSYSTEM_H__2266D767_80D6_4241_84F6_24C01C460250__INCLUDED_)
#define AFX_SOULSYSTEM_H__2266D767_80D6_4241_84F6_24C01C460250__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h"

class SoulSystem  
{
public:
	SoulSystem();
	virtual ~SoulSystem();

	DWORD IsMap(int MapNumber);
	bool WithSoul(LPOBJ lpUser);

}; extern SoulSystem g_SoulSystem;

#endif // !defined(AFX_SOULSYSTEM_H__2266D767_80D6_4241_84F6_24C01C460250__INCLUDED_)
