#if !defined(AFX_DUMMYTARGET_H__04C924B6_8788_492D_99C7_82512C97CE2D__INCLUDED_)
#define AFX_DUMMYTARGET_H__04C924B6_8788_492D_99C7_82512C97CE2D__INCLUDED_
// -------------------------------------------------------------------------------

#if _MSC_VER > 1000
#pragma once
#endif
// -------------------------------------------------------------------------------

#include "user.h"
// -------------------------------------------------------------------------------

class DummyTarget  
{
public:
			DummyTarget();
	virtual	~DummyTarget();
	// ----
	void	Init();
	// ----
	bool	IsDummy(LPOBJ lpTarget);
	int		GetDefense(LPOBJ lpUser, LPOBJ lpDummy);
	// ----
private:
	WORD	m_Class;
	BYTE	m_MapNumber;
	BYTE	m_PosX;
	BYTE	m_PosY;
	// ----
}; extern DummyTarget g_DummyTarget;
// -------------------------------------------------------------------------------

#endif
// -------------------------------------------------------------------------------
