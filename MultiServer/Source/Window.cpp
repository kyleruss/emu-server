#include "stdafx.h"
#include "Window.h"
#include "Richedit.h"
#include "Server.h"
#include "CashShopServer.h"
// -------------------------------------------------------------------------------

Window g_Window;
// -------------------------------------------------------------------------------

Window::Window()
{
	CreateDirectory("Logs", 0);
}
// -------------------------------------------------------------------------------

Window::~Window()
{

}
// -------------------------------------------------------------------------------

void Window::LogAdd(BYTE Type, LPSTR Text, ...)
{
	char TimeBuff[100] = { 0 };
	char TextBuff[3000] = { 0 };
	HWND LogBox = GetDlgItem(g_Window.m_WindowID, IDC_RICHEDIT1);
	int	Counter	= SendMessage(LogBox, EM_GETLINECOUNT, 0, 0);
	SYSTEMTIME Time;
	GetLocalTime(&Time);
	// ----
	DWORD StartPoint = 0, EndPoint = 0;
	SendMessage(LogBox, EM_GETSEL, (WPARAM)&StartPoint, (LPARAM)&EndPoint);
	// ----
	if( Counter > 100 )
	{
		int FirstLine = SendMessage(LogBox, EM_LINEINDEX, 1, 0);
		SendMessage(LogBox, EM_SETSEL, 0, MAKELONG(FirstLine, 0));
		SendMessage(LogBox, EM_REPLACESEL, 0, (LPARAM)"");
	}
	// ----
	SendMessage(LogBox, EM_SETSEL, StartPoint, EndPoint);
	// ----
	CHARRANGE CharRange;
	CharRange.cpMin = -1;
	CharRange.cpMax = -1;
	// ----
	CHARFORMAT CharFormat = { 0 };
	CharFormat.cbSize = sizeof(CharFormat);
	CharFormat.dwMask = CFM_COLOR;
	// ----
	CHARFORMAT TimeFormat = { 0 };
	TimeFormat.cbSize = sizeof(CharFormat);
	TimeFormat.dwMask = CFM_COLOR;
	TimeFormat.crTextColor = RGB(150, 150, 150); //Gray
	// ----
	sprintf(TimeBuff, "[%02d:%02d:%02d] ", Time.wHour, Time.wMinute, Time.wSecond);
	//SendMessage(LogBox, EM_SETCHARFORMAT, (LPARAM)SCF_SELECTION, (LPARAM)&TimeFormat);
	//SendMessage(LogBox, EM_EXSETSEL, 0, (LPARAM)&CharRange);
	//SendMessage(LogBox, EM_REPLACESEL, 0, (LPARAM)TimeBuff);
	// ----
	va_list ArgPointer;
    va_start(ArgPointer, Text);
	vsprintf(TextBuff, Text, ArgPointer);
	va_end(ArgPointer);
	// ----
	switch(Type)
	{
	case Log::Info:
		{
			CharFormat.crTextColor = RGB(255, 255, 255); //White
		}
		break;
		// --
	case Log::Warning:
		{
			CharFormat.crTextColor = RGB(66, 185, 255); //Blue
		}
		break;
		// --
	case Log::Error:
		{
			CharFormat.crTextColor = RGB(255, 0, 0); //Red
		}
		break;
		// --
	case Log::Debug:
		{
			CharFormat.crTextColor = RGB(255, 216, 0); //Yellow
		}
		break;
	}
	// ----
	char LogBuff[5000] = { 0 };
	sprintf(LogBuff, "%s%s\n", TimeBuff, TextBuff);
	// ----
	//if( bIsLog )
	//{
		char LogFile[MAX_PATH] = { 0 };
		sprintf(LogFile, ".\\Logs\\%04d%02d%02d.log", Time.wYear, Time.wMonth, Time.wDay);
		FILE* Input = fopen(LogFile, "a");
		fprintf(Input, LogBuff);
		fclose(Input);
	//}
	// ----
	SendMessage(LogBox, EM_SETCHARFORMAT, (LPARAM)SCF_SELECTION, (LPARAM)&CharFormat);
	SendMessage(LogBox, EM_EXSETSEL, 0, (LPARAM)&CharRange);
	SendMessage(LogBox, EM_REPLACESEL, 0, (LPARAM)LogBuff);
	SendMessage(LogBox, WM_VSCROLL, SB_BOTTOM, 0);
	SetFocus(LogBox);
    SendMessage(LogBox, EM_SETSEL, (WPARAM)GetWindowTextLength(LogBox), (LPARAM)GetWindowTextLength(LogBox));
}
// -------------------------------------------------------------------------------

void Window::ServerLogAdd(DWORD ServerType, LPSTR Text, ...)
{
	char TextBuff[3000] = { 0 };

	va_list ArgPointer;
    va_start(ArgPointer, Text);
	vsprintf(TextBuff, Text, ArgPointer);
	va_end(ArgPointer);

	char ServerName[100] = { 0 };
	
	switch(ServerType)
	{
	case ST_JOINSERVER:
		sprintf(ServerName, "JoinServer");
		break;
	case ST_DATASERVER:
		sprintf(ServerName, "DataServer");
		break;
	case ST_EVENTSERVER:
		sprintf(ServerName, "EventServer");
		break;
	case ST_RANKINGSERVER:
		sprintf(ServerName, "RankingServer");
		break;
	case ST_CASHSHOPSERVER:
		sprintf(ServerName, "CashShopServer");
		break;
	}

	this->LogAdd(Log::Info, "[%s] %s", ServerName, TextBuff);
}
// -------------------------------------------------------------------------------

void Window::TitleSet(HWND Window, LPSTR Text, ...)
{
	char TextBuff[1000] = { 0 };
	va_list ArgPointer;
    va_start(ArgPointer, Text);
	vsprintf(TextBuff, Text, ArgPointer);
	va_end(ArgPointer);
	SetWindowText(Window, TextBuff);
}
// -------------------------------------------------------------------------------
#include "DataServer.h"
BOOL CALLBACK Dialog_Main(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	switch(Message)
	{
	case WM_INITDIALOG:
		{
			g_Window.m_WindowID = Window;
			HWND LogBox = GetDlgItem(g_Window.m_WindowID, IDC_RICHEDIT1);
			SendMessage(LogBox, EM_SETBKGNDCOLOR, 0, RGB(25, 25, 25));
			g_Window.TitleSet(Window, "%s (%s) | Main", APP_NAME, APP_VERSION);
			//SetTimer(Window, WM_UPDATER, 1000, NULL);
			CreateStatusWindow(WS_CHILD | WS_VISIBLE, _T("Online"), Window, 9000);
			ServersInit();
			g_Window.LogAdd(1, "Size: %d", sizeof(SDHP_GETCHARLIST));
		}
		break;
		// --
	case WM_COMMAND:
		{
			switch(WParam)
			{
			case IDM_EXIT:
				{
					EndDialog(Window, false);
				}
				break;
				// --
//			case ID_OPTIONS_CASHSHOPRELOAD:
//				{
//					LoadCashShopPackage();
//					LoadCashShopProduct();
//				}
//				break;
			}
		}
		break;
		// --
	case WM_CLOSE:
		{
			if( MessageBox(Window, "Close zMultiServer?", 
				"Confirmation", MB_YESNO|MB_APPLMODAL) == IDYES )
			{
				EndDialog(Window, false);
			}
		}
		break;
	}
	// ----
	return 0;
}
// -------------------------------------------------------------------------------