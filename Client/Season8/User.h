#pragma once
// -------------------------------------------------------------------------------

#include "Protocol.h"
// -------------------------------------------------------------------------------

#define MAX_BATTLE_LIST			11
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct USER_GET_BATTLEINFO
{
	PBMSG_HEAD2	h;
	BYTE		BattleMapList[MAX_BATTLE_LIST];
	BYTE		BattleZoneList[MAX_BATTLE_LIST];
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

class User
{
public:
	void		Load();
	// ----
	void		Refresh();
	void		GetBattleInfo(USER_GET_BATTLEINFO* lpData);
	static bool	IsBattleServer() { return true; };
	static bool IsBattleZone(int MapNumber);
	static bool	IsMasterClass(BYTE Class);
	// ----
	static ObjectCharacter* GetChar() { return &*(ObjectCharacter*)oUserObjectStruct; };
	BYTE		m_BattleZoneList[MAX_BATTLE_LIST];
	bool		m_BattleInfoLoaded;
	// ----
}; extern User g_User;
// -------------------------------------------------------------------------------