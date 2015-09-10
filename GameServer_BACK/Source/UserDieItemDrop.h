#pragma once
// -------------------------------------------------------------------------------

#include "user.h"
// -------------------------------------------------------------------------------

class UserDieItemDrop  
{
public:
			UserDieItemDrop();
	virtual ~UserDieItemDrop();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	void	Drop(LPOBJ lpUser, LPOBJ lpKiller);
	// ----
private:
	bool	m_PvPEnabled;
	BYTE	m_PvPMinPK;
	DWORD	m_PvPRate;
	bool	m_PvMEnabled;
	BYTE	m_PvMMinPK;
	DWORD	m_PvMRate;	
	// ----
	BYTE	m_RollMaxCount;
	bool	m_RollInventory;
	bool	m_RollWeapon;
	bool	m_RollEquip;
	bool	m_RollWings;
	bool	m_RollPet;
	bool	m_RollAccessory;
	// ----
	bool	m_MapListEnabled;
	std::vector<BYTE> m_MapList;

}; extern UserDieItemDrop g_UserDieItemDrop;
// -------------------------------------------------------------------------------