#pragma once

#include "user.h"

struct OfflineTradeMapZone
{
	short MapNumber;
	BYTE SafeZone;
	BYTE PosX1;
	BYTE PosY1;
	BYTE PosX2;
	BYTE PosY2;
};

class OfflineTrade  
{
public:
			OfflineTrade();
	virtual ~OfflineTrade();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	// ----
	void	ProcStart(LPOBJ lpUser);
	bool	ProcCheck(LPOBJ lpUser);
	void	ProcRun(LPOBJ lpUser);

private:
	DWORD	m_MaxRunTime;
	std::vector<OfflineTradeMapZone> m_MapZone;

}; extern OfflineTrade g_OfflineTrade;