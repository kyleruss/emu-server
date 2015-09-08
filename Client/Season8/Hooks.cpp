#include "stdafx.h"
#include "main.h"
#include "Camera.h"
#include "Protocol.h"
#include "Interface.h"
#include "ExPoint.h"
#include "User.h"


/*
DrawInterface
009740E4  |. E8 AD000000    CALL game.00974196
00974196  /$ 55             PUSH EBP

drawtexture - 006FECDD  /$ 55             PUSH EBP
*/

LRESULT OldWindowProc = NULL;
LRESULT NewWindowProc = NULL;

HHOOK KeyboardHook = NULL;
HHOOK MouseHook = NULL;
HWND* MainWindow = (HWND*)(0x11F9634);

BOOL KeyboardSetHook(BOOL bState)
{
	if( bState == TRUE )	{
		if(KeyboardHook == NULL){
			KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hDLLInst, NULL);
		}
	}
	else{
		if( KeyboardHook != NULL )	{
			UnhookWindowsHookEx(KeyboardHook);
		}
	}
    return TRUE;
}

BOOL MouseSetHook(BOOL bState){
	if( bState == TRUE )	{
		if(MouseHook == NULL){
			//MouseHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, hDLLInst, NULL);
			MouseHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, hDLLInst, GetCurrentThreadId());
		}
	}
	else{
		if( MouseHook != NULL )
			UnhookWindowsHookEx(MouseHook);
	}
    return TRUE;
}

#include "resource.h"
LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch(wParam)
		{
		case IDOK:
			EndDialog(hWndDlg, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;
}


HWND hWnd;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if( (nCode == HC_ACTION) && (wParam == WM_KEYDOWN))
	{
		// This Struct gets infos on typed key
        KBDLLHOOKSTRUCT hookstruct = *((KBDLLHOOKSTRUCT*)lParam);
		// ----
		if( GetForegroundWindow() != *MainWindow )
		{
			return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
		}
		// ----
		switch(hookstruct.vkCode)
		{
		/*case VK_ESCAPE:
			{
				if( g_ExPoint.m_OnShow )
				{
					g_ExPoint.CloseBox();
				}
			}
			break;*/
			// --
		case VK_F10:
			{
//#ifdef __NOVUS__
				//g_ExPoint.SwitchBox();
				//g_Interface.OpenResetWindow();
				g_Camera.Switch();
//#endif
			}
			break;
			// --
		case VK_F11:
			{
				g_Camera.Init();
			}
			break;
			// --
		case VK_F12:
			{
				if( NewWindowProc == NULL )
					NewWindowProc = SetWindowLong(*MainWindow, GWL_WNDPROC, (long)WindowProc);

				WindowState();
			}
			break;
		}
	}
	// ----
	return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if( GetForegroundWindow() != *MainWindow )
	{
		return CallNextHookEx(MouseHook, nCode, wParam, lParam);
	}
	// ----
	MOUSEHOOKSTRUCTEX * Mouse	= (MOUSEHOOKSTRUCTEX*)lParam;
	g_Camera.Run(Mouse, wParam);

	g_Interface.EventResetWindow_Main(wParam);
	// -----
	return CallNextHookEx(MouseHook, nCode, wParam, lParam);
}

#define MY_TRAY_ICON_ID	0x444
#define MY_TRAY_ICON_MESSAGE 0x445

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;
	switch(uMsg)
	{
		case MY_TRAY_ICON_MESSAGE:
			switch(lParam)
			{
			case WM_LBUTTONDBLCLK:
				WindowState();
				break;
			}
			break;
		default: break;
	}
	return CallWindowProc((WNDPROC)NewWindowProc, hWnd, uMsg, wParam, lParam);
}

void WindowState()
{
	if(IsWindowVisible(*MainWindow) )	{
		ShowWindow( *MainWindow, SW_HIDE);
		UpdateWindow(*MainWindow);
		NotifyIcon(true);
	}
	else{
		ShowWindow( *MainWindow, SW_SHOW);
		UpdateWindow(*MainWindow);
		NotifyIcon(false);
	}
}

void NotifyIcon(int bState)
{
	NOTIFYICONDATA niData = {0};

	niData.cbSize = sizeof(NOTIFYICONDATA);
	niData.uID = MY_TRAY_ICON_ID;
	niData.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	niData.hIcon = (HICON)LoadImage( hDLLInst,MAKEINTRESOURCE(IDI_MAIN_ICON),IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR);
	niData.hWnd = *MainWindow;
	niData.uCallbackMessage = MY_TRAY_ICON_MESSAGE;
	strcpy_s(niData.szTip,sizeof(niData.szTip), "MuOnline (Minimized)");

	if( pPlayerState == 5 )	{
		char szChar[16];
		sprintf(szChar,"(%s)",*(DWORD*)(PLAYER_CONST0));
		strcat_s(niData.szTip,szChar);
	}

	

	if( bState == TRUE )	{
		Shell_NotifyIcon(NIM_ADD,&niData);
	}
	else{
		Shell_NotifyIcon(NIM_DELETE,&niData);
	}

	DeleteObject(niData.hIcon);
}
