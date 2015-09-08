#include "stdafx.h"
#include "Interface.h"
#include "Camera.h"
#include "User.h"
#include "ExPoint.h"
#include "ResetSystem.h"

Interface g_Interface;

void Interface::Load()
{
	SetOp((LPVOID)oProcDrawUI_Call, this->ProcUI, CALL);

	this->BindObject(eRESET_MAIN, 0x7A5A, 222, 303, -1, -1);
	this->BindObject(eRESET_TITLE, 0x7A63, 230, 67, -1, -1);
	this->BindObject(eRESET_FRAME, 0x7A58, 230, 15, -1, -1);
	this->BindObject(eRESET_FOOTER, 0x7A59, 230, 50, -1, -1);
	this->BindObject(eRESET_DIV, 0x7A62, 223, 21, -1, -1);
	this->BindObject(eRESET_CLOSE, 0x7FD2, 36, 29, -1, -1);
	this->BindObject(eRESET_INFOBG, 0x7AA3, 170, 21, -1, -1);
	this->BindObject(eRESET_MONEYBG, 0x7A89, 170, 26, -1, -1);
	this->BindObject(eRESET_FINISH, 0x7A5E, 108, 29, -1, -1);
	this->BindObject(eRESET_CREDITS, 0x7A5F, 64, 29, -1, -1);

	SetOp((LPVOID)0x0084A9E5, _BlockInterface1, JMP);
	SetOp((LPVOID)0x007D2C10, _BlockInterface2, JMP);
	SetOp((LPVOID)0x007D1842, _BlockInterface3, JMP);
	SetOp((LPVOID)0x0099038F, _BlockInterface4, JMP);
}

void Interface::ProcUI()
{
	g_Camera.Rotate();
	g_User.Refresh();


	if( (User::GetChar()->m_Class & 7) == 6 || (User::GetChar()->m_Class & 7) == 2 )
	{
		SetByte((PVOID)(0x00554B6A + 6), 15);
	}
	//if( (User::GetChar()->m_Class & 7) == 2 )
	//{
	//	SetByte((PVOID)(0x00554B6A + 6), 0);
	//}
	else
	{
		SetByte((PVOID)(0x00554B6A + 6), 2);
	}

	pProcDrawUI();

	pGLEnable(1);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//Custom addons
	g_ExPoint.DrawBox();
	g_Interface.DrawResetWindow();

	pGLDisable();
}

void Interface::BindObject(short ObjectID, DWORD ModelID, float Width, float Height, float X, float Y)
{
	this->Data[ObjectID].EventTick	= 0;
	this->Data[ObjectID].OnClick	= false;
	this->Data[ObjectID].OnShow	= false;
	this->Data[ObjectID].ModelID	= ModelID;
	this->Data[ObjectID].Width		= Width;
	this->Data[ObjectID].Height	= Height;
	this->Data[ObjectID].X			= X;
	this->Data[ObjectID].Y			= Y;
	this->Data[ObjectID].MaxX		= X + Width;
	this->Data[ObjectID].MaxY		= Y + Height;
	this->Data[ObjectID].Attribute	= 0;
}

void Interface::DrawResetWindow()
{
	if( !this->Data[eRESET_MAIN].OnShow )
	{
		return;
	}
	// ----
	DWORD ItemNameColor		= eWhite;
	float MainWidth			= 230.0;
	float MainHeight		= 313.0;
	float StartY			= 100.0;
	float StartX			= (MAX_WIN_WIDTH / 2) - (MainWidth / 2);
	float MainCenter		= StartX + (MainWidth / 3);
	float Button1X			= StartX + (MainWidth / 2) - 54;
	float Button2X			= StartX + 140;
	// ----
	this->DrawGUI(eRESET_MAIN, StartX, StartY + 2);
	this->DrawGUI(eRESET_TITLE, StartX, StartY);
	StartY = this->DrawRepeatGUI(eRESET_FRAME, StartX, StartY + 67.0, 13);
	this->DrawGUI(eRESET_FOOTER, StartX, StartY);
	this->DrawGUI(eRESET_CLOSE, StartX + MainWidth - this->Data[eRESET_CLOSE].Width, this->Data[eRESET_TITLE].Height + this->Data[eRESET_CLOSE].Height + 2);
	// ----
	if( this->IsWorkZone(eRESET_CLOSE) )
	{
		DWORD Color = eGray100;
		// ----
		if( this->Data[eRESET_CLOSE].OnClick )
		{
			Color = eGray150;
		}
		// ----
		this->DrawColoredGUI(eRESET_CLOSE, this->Data[eRESET_CLOSE].X, this->Data[eRESET_CLOSE].Y, Color);
		this->DrawToolTip(this->Data[eRESET_CLOSE].X + 5, this->Data[eRESET_CLOSE].Y + 25, "Close");
	}
	// ----
	this->DrawFormat(eGold, StartX + 10, 110, 210, 3, "Reset Master");
	// ----
	this->DrawGUI(eRESET_INFOBG, StartX + 30, 140);
	this->DrawFormat(eGold, StartX + 35, 145, 70, 3, "Next Reset:");
	this->DrawFormat(eWhite, StartX + 135, 145, 70, 1, "%d", g_ResetSystem.NextReset);
	this->DrawGUI(eRESET_INFOBG, StartX + 30, 160);
	this->DrawFormat(eGold, StartX + 35, 165, 70, 3, "Reward Points:");
	this->DrawFormat(eWhite, StartX + 135, 165, 70, 1, "%d", g_ResetSystem.RewardPoint);
	// ----
	this->DrawGUI(eRESET_DIV, StartX, 185);
	// ----
	this->DrawFormat(eGold, StartX + 30, 215, 210, 1, "Required Level:");
	// ----
	if( User::GetChar()->m_Level < g_ResetSystem.ReqLevel )
	{
		this->DrawFormat(eRed, StartX + 30, 225, 210, 1, "- %d (Your: %d)",
			g_ResetSystem.ReqLevel, User::GetChar()->m_Level);
	}
	else
	{
		this->DrawFormat(eWhite, StartX + 30, 225, 210, 1, "- %d",
			g_ResetSystem.ReqLevel);
	}
	// ----
	this->DrawFormat(eGold, StartX + 30, 240, 210, 1, "(Optional) Required Zen:");
	this->DrawGUI(eRESET_MONEYBG, StartX + 30, 250);
	// ----
	char MoneyBuff[50], MoneyBuff2[50];
	ZeroMemory(MoneyBuff, sizeof(MoneyBuff));
	ZeroMemory(MoneyBuff2, sizeof(MoneyBuff2));
	// ----
	if( User::GetChar()->m_MoneyInventory 
		< g_ResetSystem.ReqMoney )
	{
		pGetMoneyFormat((double)g_ResetSystem.ReqMoney , 
			MoneyBuff, 0);
		pGetMoneyFormat((double)g_ResetSystem.ReqMoney - User::GetChar()->m_MoneyInventory, 
			MoneyBuff2, 0);
		this->DrawFormat(eRed, StartX + 70, 254, 170, 1, "%s\n(Need still %s)", 
			MoneyBuff, MoneyBuff2);
	}
	else
	{
		pGetMoneyFormat((double)g_ResetSystem.ReqMoney, 
			MoneyBuff, 0);
		this->DrawFormat(eGold, StartX + 70, 257, 70, 1, "%s", 
			MoneyBuff);
	}

	this->DrawGUI(eRESET_FINISH, Button1X, this->Data[eRESET_FOOTER].Y + 10);
	
	this->Data[eRESET_FINISH].Attribute = true;
	this->Data[eRESET_CREDITS].Attribute = true;
	
	//this->DrawGUI(eRESET_CREDITS, Button2X, this->Data[eRESET_FOOTER].Y + 10);
	//this->DrawFormat(eWhite, Button2X, this->Data[eRESET_FOOTER].Y + 20, 64, 3, "For %d credits", 
	//	g_ResetSystem.Credits);
	
	this->DrawGUI(eRESET_DIV, StartX, this->Data[eRESET_FOOTER].Y - 10);

	/*if (!g_ResetSystem.AllowCredits)
	{
		this->Data[eRESET_CREDITS].Attribute = false;
		this->DrawColoredGUI(eRESET_CREDITS, this->Data[eRESET_CREDITS].X, this->Data[eRESET_CREDITS].Y, eGray150);
	}*/

	this->DrawFormat(eWhite, Button1X , this->Data[eRESET_FOOTER].Y + 20, 108, 3, "For Zen");

	if( IsWorkZone(eRESET_FINISH) )
	{
		this->DrawToolTip(this->Data[eRESET_FINISH].X, this->Data[eRESET_FINISH].Y - 15, "Delay: %d (min.)", 
			g_ResetSystem.Delay);
	}

	if( IsWorkZone(eRESET_FINISH) && this->Data[eRESET_FINISH].Attribute)
	{
		DWORD Color = eGray100;
		// ----
		if( this->Data[eRESET_FINISH].OnClick )
		{
			Color = eGray150;
		}
		// ----
		this->DrawColoredGUI(eRESET_FINISH, this->Data[eRESET_FINISH].X, this->Data[eRESET_FINISH].Y, Color);
	}

	if( IsWorkZone(eRESET_CREDITS) )
	{
		this->DrawToolTip(this->Data[eRESET_CREDITS].X, this->Data[eRESET_CREDITS].Y - 15, "Without delay & zen");
	}

	if( IsWorkZone(eRESET_CREDITS) && this->Data[eRESET_CREDITS].Attribute)
	{
		DWORD Color = eGray100;
		// ----
		if( this->Data[eRESET_CREDITS].OnClick )
		{
			Color = eGray150;
		}
		// ----
		this->DrawColoredGUI(eRESET_CREDITS, this->Data[eRESET_CREDITS].X, this->Data[eRESET_CREDITS].Y, Color);
	}
}

bool Interface::EventResetWindow_Main(DWORD Event)
{
	switch(Event)
	{
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
		{
			this->EventResetWindow_Close(Event);
			this->EventResetWindow_Finish(Event);
			this->EventResetWindow_Credits(Event);
		}
		break;
	}
	
	return true;
}

bool Interface::EventResetWindow_Close(DWORD Event)
{
	DWORD CurrentTick	= GetTickCount();
	DWORD Delay			= (CurrentTick - this->Data[eRESET_CLOSE].EventTick);
	// ----
	if( !this->Data[eRESET_MAIN].OnShow || !IsWorkZone(eRESET_CLOSE) )
	{
		return false;
	}
	// ----
	if( Event == WM_LBUTTONDOWN )
	{
		this->Data[eRESET_CLOSE].OnClick = true;
		return true;
	}
	// ----
	this->Data[eRESET_CLOSE].OnClick = false;
	// ----
	if( Delay < 500 )
	{
		return false;
	}
	// ----
	this->Data[eRESET_CLOSE].EventTick = GetTickCount();
	this->CloseResetWindow();
	// ----
	return false;
}

bool Interface::EventResetWindow_Finish(DWORD Event)
{
	DWORD CurrentTick	= GetTickCount();
	DWORD Delay			= (CurrentTick - this->Data[eRESET_FINISH].EventTick);
	// ----
	if( !this->Data[eRESET_MAIN].OnShow || !IsWorkZone(eRESET_FINISH) 
		|| !this->Data[eRESET_FINISH].Attribute )
	{
		return false;
	}
	// ----
	if( Event == WM_LBUTTONDOWN )
	{
		this->Data[eRESET_FINISH].OnClick = true;
		return true;
	}
	// ----
	this->Data[eRESET_FINISH].OnClick = false;
	// ----
	if( Delay < 500 )
	{
		return false;
	}
	// ----
	this->Data[eRESET_FINISH].EventTick = GetTickCount();
	g_ResetSystem.PayType = 0;
	// ----
	g_ResetSystem.CG_RequestStart();
	// ----
	return false;
}

bool Interface::EventResetWindow_Credits(DWORD Event)
{
	DWORD CurrentTick	= GetTickCount();
	DWORD Delay			= (CurrentTick - this->Data[eRESET_CREDITS].EventTick);
	// ----
	if( !this->Data[eRESET_MAIN].OnShow || !IsWorkZone(eRESET_CREDITS) 
		|| !this->Data[eRESET_CREDITS].Attribute )
	{
		return false;
	}
	// ----
	if( Event == WM_LBUTTONDOWN )
	{
		this->Data[eRESET_CREDITS].OnClick = true;
		return true;
	}
	// ----
	this->Data[eRESET_CREDITS].OnClick = false;;
	// ----
	if( Delay < 500 )
	{
		return false;
	}
	// ----
	
	this->Data[eRESET_CREDITS].EventTick = GetTickCount();
	g_ResetSystem.PayType = 1;
	g_ResetSystem.ProcWait = 0;
	g_ResetSystem.CG_RequestStart();
	// ----
	return false;
}

void Interface::DrawGUI(short ObjectID, float PosX, float PosY)
{
	if( this->Data[ObjectID].X == -1 || this->Data[ObjectID].Y == -1 )
	{
		this->Data[ObjectID].X		= PosX;
		this->Data[ObjectID].Y		= PosY;
		this->Data[ObjectID].MaxX	= PosX + this->Data[ObjectID].Width;
		this->Data[ObjectID].MaxY	= PosY + this->Data[ObjectID].Height;
	}
	// ----
	pDrawGUI(this->Data[ObjectID].ModelID, PosX, PosY, 
		this->Data[ObjectID].Width, this->Data[ObjectID].Height);
}

float Interface::DrawRepeatGUI(short MonsterID, float X, float Y, int Count)
{
	float StartY = Y;
	// ----
	for( int i = 0; i < Count; i++ )
	{
		pDrawGUI(this->Data[MonsterID].ModelID, X, StartY,
			this->Data[MonsterID].Width, this->Data[MonsterID].Height);
		// ----
		StartY += this->Data[MonsterID].Height;
	}
	// ----
	return StartY;
}

void Interface::DrawColoredGUI(short ObjectID, float X, float Y, DWORD Color)
{
	if( this->Data[ObjectID].X == -1 || this->Data[ObjectID].Y == -1 )
	{
		this->Data[ObjectID].X		= X;
		this->Data[ObjectID].Y		= Y;
		this->Data[ObjectID].MaxX	= X + this->Data[ObjectID].Width;
		this->Data[ObjectID].MaxY	= Y + this->Data[ObjectID].Height;
	}
	// ----
	pDrawColorButton(this->Data[ObjectID].ModelID, X, Y, 
		this->Data[ObjectID].Width, this->Data[ObjectID].Height, 0, 0, Color);
}

int Interface::DrawFormat(DWORD Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, ...)
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

int Interface::DrawToolTip(int X, int Y, LPCSTR Text, ...)
{
	char Buff[2048];
	int BuffLen	= sizeof(Buff);
	ZeroMemory(Buff, BuffLen);
	// ----
	va_list args;
	va_start(args, Text);
	int Len	= vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);
	// ----
	return pDrawToolTip(X, Y, Buff);
}

bool Interface::IsWorkZone(short ObjectID)
{
	float PosX = this->Data[ObjectID].X;
	float MaxX = PosX + this->Data[ObjectID].Width;
	// ----
	/*if( ObjectID == eSAMPLEBUTTON 
		|| ObjectID == eCAMERA_MAIN 
		|| ObjectID == eCAMERA_BUTTON1
		|| ObjectID == eCAMERA_BUTTON2 )
	{
		PosX = this->GetResizeX(ObjectID);
		MaxX = PosX + this->Data[ObjectID].Width;
	}*/
	// ----
	if( (pCursorX < PosX || pCursorX > MaxX) 
		|| (pCursorY < this->Data[ObjectID].Y || pCursorY > this->Data[ObjectID].MaxY) )
	{
		return false;
	}
	// ----
	return true;
}

bool Interface::IsWorkZone(float X, float Y, float MaxX, float MaxY)
{
	if( (pCursorX < X || pCursorX > MaxX) 
		|| (pCursorY < Y || pCursorY > MaxY) )
	{
		return false;
	}
	// ----
	return true;
}

float Interface::GetResizeX(short ObjectID)
{
	if( pWinWidth == 800 )
	{
		return this->Data[ObjectID].X + 16.0;
	}
	else if( pWinWidth != 1024 )
	{
		return this->Data[ObjectID].X - 16.0;
	}
	// ----
	return this->Data[ObjectID].X;
}

void Interface::NotifyBox(LPSTR Title, LPSTR Text)
{
	pDrawMessageBox(pGetMessageBox((LPVOID)pMessageBoxThis()), 0, 0, Text, Title, 0, 0, 0);
}

__declspec(naked) void Interface::ScaleFormAction(DWORD Form, LPSTR ElementName, LPSTR Command, ...)
{
	ASM_JMP(0x0097E579);
}

BOOL IsBlockedInterface()
{
	if( g_Interface.CheckResetWindow() )
	{
		return true;
	}
	// ----
	return false;
}

DWORD _BlockInterface1_MOVOK =0x0084A9FB;
DWORD _BlockInterface1_MOV = 0x0084A9EB;

void __declspec( naked ) _BlockInterface1()
{
	__asm
	{
		CALL IsBlockedInterface
		TEST EAX,EAX
		JE move
		JMP DWORD PTR DS:[_BlockInterface1_MOVOK]
move:
		MOV EAX,DWORD PTR SS:[EBP-4]
		CMP DWORD PTR DS:[EAX],0
		JMP DWORD PTR DS:[_BlockInterface1_MOV]
	}
}

DWORD _BlockInterface2_MOV = 0x007D2C16;

void __declspec( naked ) _BlockInterface2()
{
	__asm
	{
		CALL IsBlockedInterface
		TEST EAX,EAX
		JE move
		MOV AL,1
		JMP DWORD PTR DS:[_BlockInterface2_MOV]
move:
		MOV EAX,DWORD PTR SS:[EBP-4]
		MOV AL,BYTE PTR DS:[EAX+0x10]
		JMP DWORD PTR DS:[_BlockInterface2_MOV]
	}
}

DWORD _BlockInterface3_MOVOK =0x007D1851;
DWORD _BlockInterface3_MOV = 0x007D1848;

void __declspec( naked ) _BlockInterface3()
{
	__asm
	{
		CALL IsBlockedInterface
		TEST EAX,EAX
		JE move
		JMP DWORD PTR DS:[_BlockInterface3_MOVOK]
move:
		MOV EAX,DWORD PTR SS:[EBP-0x21D8]
		JMP DWORD PTR DS:[_BlockInterface3_MOV]
	}
}

DWORD _BlockInterface4_MOVOK =0x0099039A;
DWORD _BlockInterface4_MOV = 0x00990395;

void __declspec( naked ) _BlockInterface4()
{
	__asm
	{
		CALL IsBlockedInterface
		TEST EAX,EAX
		JE move
		JMP DWORD PTR DS:[_BlockInterface4_MOVOK]
move:
		MOV EAX,DWORD PTR SS:[EBP-4]
		MOV ECX,DWORD PTR DS:[EAX+0x70]
		JMP DWORD PTR DS:[_BlockInterface4_MOV]
	}
}