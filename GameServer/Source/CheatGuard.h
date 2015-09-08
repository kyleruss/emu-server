#pragma once

#include "user.h"
#include "LogToFile.h"

struct PacketCheatGuardCGClientInfo
{
	BYTE HeadType;
	BYTE HeadSize;
	BYTE HeadCode;
	BYTE CheckCode;
	WORD IsSpeedEditor;
	WORD wWord1;
	WORD wAttackSpeed1;
	WORD wWord2;
	WORD wAttackSpeed2;
};

struct CheatGuardModuleSpeedHack
{
	bool Enabled;
	char Notice[256];
	short NoticeCount;
	bool Disconnect;
	bool InformPlayersAround;
	short AttackSpeedMaxDelta;
	short MagicSpeedMaxDelta;
};

class CheatGuard  
{
public:
	CheatGuard();
	virtual ~CheatGuard();

	void Init();
	void Load();
	void Read(LPSTR File);

	void ProcCheckLive(LPOBJ lpUser);
	bool ProcSpeedHack(LPOBJ lpUser, WORD AttackSpeed, WORD MagicSpeed, bool IsSpeedEditor);
	
	bool CG_LiveProc(LPOBJ lpUser, PacketCheatGuardCGClientInfo* lpData, int HeadCode, int Size);
	
	void GC_Notice(LPOBJ lpUser, LPSTR Title, LPSTR Text, ...);
	void GC_InformPlayersAround(LPOBJ lpUser);

private:
	CLogToFile* Log;
	CheatGuardModuleSpeedHack ModuleSpeedHack;
}; 

extern CheatGuard g_CheatGuard;