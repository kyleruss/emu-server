#pragma once
// ----------------------------------------------------------------------------------------------

#include "protocol.h"
// ----------------------------------------------------------------------------------------------

#include "Defines.h"
#include "Interface.h"
// ----------------------------------------------------------------------------------------------

struct EXPOINT_UPDATE_REQ
{
	PBMSG_HEAD2		Head;
};
// ----------------------------------------------------------------------------------------------

struct EXPOINT_UPDATE_ANS
{
	PBMSG_HEAD2		Head;
	WORD			ExPointPvP;
	WORD			ExPointEvent;
	WORD			ExPointGens;
};
// ----------------------------------------------------------------------------------------------

class ExPoint
{
public:
			ExPoint();
			~ExPoint();
	// ----
	void	Init();
	void	Load();
	// ----
	void	Request();
	void	Update(EXPOINT_UPDATE_ANS * pAnswer);
	// ----
	void	OpenBox();
	void	CloseBox();
	void	SwitchBox();
	int		DrawFormat(DWORD Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, ...);
	void	DrawBox();
	// ----
	bool	m_OnShow;
	WORD	m_ExPointPvP;
	WORD	m_ExPointEvent;
	WORD	m_ExPointGens;
	// ----
}; extern ExPoint g_ExPoint;
// ----------------------------------------------------------------------------------------------