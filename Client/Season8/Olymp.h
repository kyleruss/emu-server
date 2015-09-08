#pragma once

#if (__CUSTOM_OLYMP__==1)

#include "protocol.h"


class TOlympNPCWindow
{
	struct PMSG_CLOSEWINDOW
	{
		PBMSG_HEAD2 h;
	};

	struct PMSG_REQUESTREG
	{
		PBMSG_HEAD2 h;
	};

	struct PSMG_OLP_NPCTALK
	{
		PBMSG_HEAD2 h;
		BYTE btOpen;
		BYTE btRegistered;
	};

	struct PMSG_OLP_MESSAGEBOX
	{
		PBMSG_HEAD2 h;
		BYTE btMessage;
		short aInfo1;
		short aInfo2;
	};

public:
	TOlympNPCWindow();
	~TOlympNPCWindow();

	BYTE Init(void* Class);
	void LoadTextures();
	void DeleteTextures();
	void Draw();
	void DrawJoinBtn();
	void DrawRewardBtn();
	void DrawCloseBtn();

	void ShowMessage(BYTE btMessage,BYTE lowByte,BYTE highByte);

	void ShowGUI(DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,float,float,DWORD,DWORD,DWORD,float);
	void NpcTalk(LPBYTE Info);
	void SendClose();
	void SendRequest();
	void SendRewardRequest();
	void MessageHandle(LPBYTE aRecv);
public:
	BYTE btShow;
	BYTE btJoinBtn;
	BYTE btRewardBtn;
	BYTE btCloseBtn;

	BYTE btRegistered;
	BYTE btOpen;

	//BYTE btBoxShow;
};

extern TOlympNPCWindow gw_OlympNPC;

#endif