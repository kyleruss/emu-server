#include "stdafx.h"
#include "ResetSystem.h"
#include "Interface.h"
#include "main.h"

ResetSystem g_ResetSystem;

void ResetSystem::GC_RequestDialog(PacketResetDialog* lpRequest)
{
	this->Type = lpRequest->Type;
	this->NextReset = lpRequest->NextReset;
	this->RewardPoint = lpRequest->RewardPoint;
	this->ReqLevel = lpRequest->ReqLevel;
	this->ReqMoney = lpRequest->ReqMoney;
	this->Delay = lpRequest->Delay;
	this->Credits = lpRequest->Credits;
	this->AllowCredits = lpRequest->AllowCredits;

	if (this->Delay < 1)
	{
		this->ProcWait = 2;
	}

	g_Interface.OpenResetWindow();
}

void ResetSystem::GC_RequestMessageBox(PacketResetMessageBox* lpRequest)
{
	g_Interface.CloseResetWindow();
	g_Interface.NotifyBox(lpRequest->Title, lpRequest->Text);
}

void ResetSystem::CG_RequestStart()
{
	BYTE Buff[4];
	PacketResetStart lpRequest;
	lpRequest.Head.c			= 0xC1;
	lpRequest.Head.size		= sizeof(lpRequest);
	lpRequest.Head.headcode	= 0xFE;
	lpRequest.Head.subcode	= 0x05;
	lpRequest.Type = this->Type;
	lpRequest.PayType = this->PayType;
	lpRequest.ProcWait = this->ProcWait;
	// ----
	CStreamPacketEngine_Client ps;
	ps.AddData(&lpRequest, sizeof(lpRequest));
	ps.InsertPacket(Buff);
	// ----
	if( g_s != INVALID_SOCKET )
	{
		g_MainClass.MySend(g_s, (const char*)&Buff, sizeof(PacketResetStart), 0);
	}
}

void ResetSystem::GC_AnswerStart(PacketAnswerStart* lpAnswer)
{
	g_ResetSystem.PayType = lpAnswer->PayType;
	g_ResetSystem.ProcWait = lpAnswer->WaitProc;
	g_ResetSystem.WaitMin = lpAnswer->WaitMin;

	if (!lpAnswer->Result)
	{
		g_Interface.CloseResetWindow();
		return;
	}
}