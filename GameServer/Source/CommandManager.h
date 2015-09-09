#pragma once
// -------------------------------------------------------------------------------

#include "user.h"
// -------------------------------------------------------------------------------

struct CommandInfo
{
	int Index;
	char Text[80];
	BYTE Access;
	int MinLevel;
	int MinReset;
	BYTE PriceType;
	int Price;
	int PremiumAccess;
};
// -------------------------------------------------------------------------------

namespace Command
{
	enum T
	{
		BanChat			= 0,
		BanUser			= 1,
		BanAccount		= 2,
		Disconnect		= 3,
		PKSet			= 4,
		PKReset			= 5,
		MoneySet		= 6,
		MakeItem		= 7,
		MakeRandomSet	= 8,
		HideOn			= 9,
		HideOff			= 10,
		ClearDrop		= 11,
		ClearInventory	= 12,
		Skin			= 13,
		PartyInfo		= 14,
		FireCrack		= 15,
		Trans			= 16,
		Track			= 17,
		Trace			= 18,
		CastleSiege1	= 19,
		CastleSiege2	= 20,
		CastleSiege3	= 21,
		CastleSiege4	= 22,
		CastleSiege5	= 23,
		CastleSiege6	= 24,
		CastleSiege7	= 25,
		CastleSiege8	= 26,
		CastleSiege9	= 27,
		Crywolf1		= 28,
		Crywolf2		= 29,
		Crywolf3		= 30,
		Crywolf4		= 31,
		Request			= 32,
		PartyLeader		= 33,
		Move			= 34,
		Post			= 35,
		AddStr			= 36,
		AddAgi			= 37,
		AddVit			= 38,
		AddEne			= 39,
		AddCom			= 40,
		Offtrade		= 41,
		MultiVault		= 42,
		GuildWar		= 43,
		BattleSoccer	= 44,
		Reset			= 45,
		PKClear			= 46,
		
	};
};
// -------------------------------------------------------------------------------

class CommandManager  
{
public:
			CommandManager();
	virtual ~CommandManager();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	// ----
	void	Run(LPOBJ lpUser, LPSTR Text);
	BYTE	CheckCommand(LPOBJ lpUser, LPSTR Text); //0: not found, 1: access, 2: zen
	LPOBJ	GetUserInfo(LPOBJ lpUser, LPSTR UserName);
	LPSTR	GetTokenString();
	int		GetTokenNumber();
	CommandInfo* GetCommand(int Index);
	CommandInfo* GetCommand(LPSTR Text);
	// ----
private:
	std::vector<CommandInfo> m_CommandInfo;
	// ----
}; extern CommandManager g_CommandManager;
// -------------------------------------------------------------------------------