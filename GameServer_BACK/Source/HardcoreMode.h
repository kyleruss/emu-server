#pragma once

#include "user.h"

#pragma pack(push, 1)
struct GD_HardcoreMode_Activate
{
	PBMSG_HEAD2 Header;
	BYTE		Status;
	short		LifeCount;
};
#pragma pack(pop)

class HardcoreMode  
{
public:
			HardcoreMode();
	virtual ~HardcoreMode();
	
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	
	void	Activate();

	void	ProcDie(LPOBJ lpUser, LPOBJ lpKiller);

private:
	bool	m_IsActive;

	bool	m_LifeDecreaseByPvP;
	bool	m_LifeDecreaseByPvM;
	short	m_LifeDefaultCount;

	char	m_ManagerCommandAdd[80];
	char	m_ManagerCommandDec[80];
	char	m_ManagerCommandSet[80];
	char	m_ManagerCommandInfo[80];

}; extern HardcoreMode g_HardcoreMode;