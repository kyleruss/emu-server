#include "stdafx.h"
#include "ExPoint.h"
#include "Security.h"
#include "MainClass.h"
// ----------------------------------------------------------------------------------------------

ExPoint g_ExPoint;
// ----------------------------------------------------------------------------------------------

ExPoint::ExPoint()
{
	this->Init();
}
// ----------------------------------------------------------------------------------------------

ExPoint::~ExPoint()
{

}
// ----------------------------------------------------------------------------------------------

void ExPoint::Init()
{
	this->m_OnShow			= false;
	this->m_ExPointPvP		= 0;
	this->m_ExPointEvent	= 0;
	this->m_ExPointGens		= 0;
}
// ----------------------------------------------------------------------------------------------

void ExPoint::Load()
{
	this->Init();
}
// ----------------------------------------------------------------------------------------------

void ExPoint::Request()
{
	BYTE Buff[4];
	EXPOINT_UPDATE_REQ pRequest;
	pRequest.Head.c			= 0xC1;
	pRequest.Head.size		= sizeof(pRequest);
	pRequest.Head.headcode	= 0xFE;
	pRequest.Head.subcode	= 0x03;
	// ----
	CStreamPacketEngine_Client ps;
	ps.AddData(&pRequest, sizeof(pRequest));
	ps.InsertPacket(Buff);
	// ----
	if( g_s != INVALID_SOCKET )
	{
		g_MainClass.MySend(g_s, (const char*)&Buff, sizeof(EXPOINT_UPDATE_REQ), 0);
	}
}
// ----------------------------------------------------------------------------------------------

void ExPoint::Update(EXPOINT_UPDATE_ANS * pAnswer)
{
	this->m_ExPointPvP		= pAnswer->ExPointPvP;
	this->m_ExPointEvent	= pAnswer->ExPointEvent;
	this->m_ExPointGens		= pAnswer->ExPointGens;
}
// ----------------------------------------------------------------------------------------------

void ExPoint::OpenBox()
{
	*(DWORD*)(0x8B4AC46) = FALSE;
	//this->Request();
	this->m_OnShow = true;
}
// ----------------------------------------------------------------------------------------------

void ExPoint::CloseBox()
{
	this->m_OnShow = false;
}
// ----------------------------------------------------------------------------------------------

void ExPoint::SwitchBox()
{
	if( this->m_OnShow )
	{
		this->CloseBox();
		return;
	}
	// ----
	this->OpenBox();
}
// ----------------------------------------------------------------------------------------------

int ExPoint::DrawFormat(DWORD Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, ...)
{
	char Buff[2048];
	int BuffLen	= sizeof(Buff)-1;
	ZeroMemory(Buff, BuffLen);
	// ----
	va_list args;
	va_start(args, Text);
	int Len	= vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);
	// ----
	int LineCount = 0;
	// ----
	char * Line = strtok(Buff, "\n");
	// ----
	while( Line != NULL )
	{
		pDrawColorText(Line, PosX, PosY, Width, 0, Color, 0, Align);
		PosY += 10;
		Line = strtok(NULL, "\n");
	}
	// ----
	return PosY;
}
// ----------------------------------------------------------------------------------------------

void ExPoint::DrawBox()
{
	if( !this->m_OnShow )
	{
		return;
	}
	// ----
	float MainWidth		= 230.0;
	float MainHeight	= 313.0;
	float StartY		= 100.0;
	float StartX		= (MAX_WIN_WIDTH / 2) - (MainWidth / 2);
	// ----
	pDrawGUI(0x7A5A, StartX, StartY + 2, 222, 120);
	pDrawGUI(0x7A63, StartX, StartY, 230, 67);
	pDrawGUI(0x7A58, StartX, StartY + 67, 230, 15);
	pDrawGUI(0x7A59, StartX, StartY + 67 + 15, 230, 50);
	// ----
	this->DrawFormat(eGold, StartX + 10, 110, 210, 3, "Novus MU Statistic");
	// ----
	pDrawGUI(0x7AA3, StartX + 30, 140, 170, 21);
	this->DrawFormat(eBlue, StartX + 35, 145, 70, 3, "Event Points:");
	this->DrawFormat(eWhite, StartX + 135, 145, 70, 1, "%d", this->m_ExPointEvent);
	pDrawGUI(0x7AA3, StartX + 30, 160, 170, 21);
	this->DrawFormat(eBlue, StartX + 35, 165, 70, 3, "Gens Points:");
	this->DrawFormat(eWhite, StartX + 135, 165, 70, 1, "%d", this->m_ExPointGens);
	pDrawGUI(0x7AA3, StartX + 30, 180, 170, 21);
	this->DrawFormat(eBlue, StartX + 35, 185, 70, 3, "PvP Points:");
	this->DrawFormat(eWhite, StartX + 135, 185, 70, 1, "%d", this->m_ExPointPvP);
}
// ----------------------------------------------------------------------------------------------