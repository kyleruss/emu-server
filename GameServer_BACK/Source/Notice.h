#pragma once
#include "user.h"
// -------------------------------------------------------------------------------

struct NoticeConnectInfo
{
	BYTE Type;
	char Text[100];
};
// -------------------------------------------------------------------------------

struct NoticeRotateInfo
{
	BYTE Type;
	short Delay;
	DWORD Tick;
	char Text[100];
};

struct NoticeOnlineInfo
{
	BYTE Type;
	short profile;
	char Text[100];
};

// -------------------------------------------------------------------------------

class Notice  
{
public:
			Notice();
	virtual ~Notice();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	// ----
	void	Create(LPBYTE Buffer, BYTE Type, LPSTR Text);
	void	SendOnConnect(int UserIndex);
	void	SendOnPoint();
	void	NotifyOnline(LPOBJ user);
	// ----
private:
	std::vector<NoticeConnectInfo> m_ConnectList;
	std::vector<NoticeRotateInfo> m_RotateList;
	std::vector<NoticeOnlineInfo> m_OnlineList;
	// ----
}; extern Notice g_Notice;
// -------------------------------------------------------------------------------