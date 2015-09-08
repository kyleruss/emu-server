#pragma once

#include "protocol.h"

struct PacketResetDialog
{
	PBMSG_HEAD2 Head;
	BYTE Type;
	WORD NextReset;
	DWORD RewardPoint;
	WORD ReqLevel;
	DWORD ReqMoney;
	DWORD Delay;
	DWORD Credits;
	bool AllowCredits;
	BYTE WaitProc;
	DWORD WaitMin;
};

#pragma pack(push, 1)
struct PacketResetMessageBox
{
	PBMSG_HEAD2 Head;
	char Title[60];
	char Text[140];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PacketResetStart
{
	PBMSG_HEAD2 Head;
	BYTE Type;
	BYTE PayType;
	BYTE ProcWait;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PacketAnswerStart
{
	PBMSG_HEAD2 Head;
	BYTE Type;
	BYTE PayType;
	BYTE WaitProc;
	DWORD WaitMin;
	BYTE Result;
};
#pragma pack(pop)



class ResetSystem
{
public:
	void	GC_RequestDialog(PacketResetDialog* lpRequest);
	void	GC_RequestMessageBox(PacketResetMessageBox* lpRequest);
	void	CG_RequestStart();
	void	GC_AnswerStart(PacketAnswerStart* lpAnswer);

public:
	BYTE Type; //0 - normal, 1 - grand
	WORD NextReset;
	DWORD RewardPoint;
	WORD ReqLevel;
	DWORD ReqMoney;
	DWORD Delay;
	DWORD Credits;
	bool AllowCredits;
	
public:
	BYTE	PayType; //0 - money, 1 - credits
	BYTE	ProcWait; //0 - none, 1 - wait, 2 - finished
	DWORD	WaitMin;

}; extern ResetSystem g_ResetSystem;