// eDataServer2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Server.h"
#include "zMultiServer.h"
#include "winutil.h"
#include "resource.h"
#include "JoinServer.h"
#include "CashShopServer.h"
#include "DataServerDB.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text
HWND g_hWnd = NULL;
HWND g_hTab = NULL;
HWND g_hLog = NULL;
BYTE TabArray[MAX_SERVERTYPE] = {-1};

bool g_CustomHardcoreMode = false;
short g_CustomHardcoreDefaultLife = 1;

#include "Window.h"

typedef struct _MINIDUMP_EXCEPTION_INFORMATION 
{
    DWORD ThreadId;
    PEXCEPTION_POINTERS ExceptionPointers;
    BOOL ClientPointers;
} MINIDUMP_EXCEPTION_INFORMATION, *PMINIDUMP_EXCEPTION_INFORMATION;
// -------------------------------------------------------------------------------

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, DWORD DumpType, PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, void* UserStreamParam, void* CallbackParam);
LPTOP_LEVEL_EXCEPTION_FILTER pOldFilter;
// -------------------------------------------------------------------------------

#define LOG_PATH_CRASH					"Logs\\Crash\\"

LONG CALLBACK make_minidump(EXCEPTION_POINTERS* e)
{
	HMODULE hDbgDll = LoadLibrary("DBGHELP.DLL");

	if( hDbgDll )
	{
		MINIDUMPWRITEDUMP _MiniDumpWrite = (MINIDUMPWRITEDUMP)GetProcAddress(hDbgDll,"MiniDumpWriteDump");

		if( _MiniDumpWrite )
		{
			char szDmpName[64] = {0};
			SYSTEMTIME pTime;
			GetLocalTime(&pTime);

			sprintf(szDmpName,"%s%d-%d-%d_%dh%dm%ds.dmp",
				LOG_PATH_CRASH, pTime.wYear,pTime.wMonth,pTime.wDay,pTime.wHour,pTime.wMinute,pTime.wSecond);

			HANDLE hFile = CreateFile(szDmpName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,128,NULL);

			if( hFile != INVALID_HANDLE_VALUE )
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = e;
				ExInfo.ClientPointers = NULL;

				BOOL bDump = _MiniDumpWrite(GetCurrentProcess(),GetCurrentProcessId(),hFile,0x26,&ExInfo,NULL,NULL);

				if( bDump )
				{
					CloseHandle(hFile);
					return true;
				}
			}

			CloseHandle(hFile);
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
// -------------------------------------------------------------------------------

void SetUnhandledFilter()
{
	SetErrorMode(SEM_FAILCRITICALERRORS);
	pOldFilter = SetUnhandledExceptionFilter(&make_minidump);
}
// -------------------------------------------------------------------------------

void UnSetUnhandledFilter()
{
	SetUnhandledExceptionFilter(pOldFilter);
}
// -------------------------------------------------------------------------------




int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	

	SetUnhandledFilter();
	AfxInitRichEdit();
	g_Window.InitInstance(hInstance);




	UnSetUnhandledFilter();
	return false;
 	// TODO: Place code here.
	/*MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EDATASERVER2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:

	if(ServersInit() == FALSE)
	{
		MsgBox("ERROR: ServerInit() == FALSE");
			return FALSE;
	}

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	if( g_bIsServer[ST_DATASERVER] == TRUE )
	{
		SetTimer(g_hWnd,WM_DROP_STATISTIC,60000,NULL);
	}

	if( g_bIsServer[ST_JOINSERVER] == TRUE )
	{
		SetTimer(g_hWnd,WM_BILL_PROCTIMER,60000,NULL);
	}

	SetTimer(g_hWnd,WM_LOGTIMER,1000,NULL);
	SetTimer(g_hWnd, WM_LOG_DATE_CHANGE, 60000, NULL);

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_EDATASERVER2);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;*/
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_EDATASERVER2);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_EDATASERVER2;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   if( g_bIsServer[ST_CASHSHOPSERVER] == TRUE )
   {
	   EnableMenuItem(GetMenu(hWnd),ID_RELOAD_CASHSHOP,MF_ENABLED);
   }

   g_hWnd = hWnd;

   g_hTab = DoCreateTabControl(hWnd);
   g_hLog = DoCreateDisplayWindow(g_hTab);

   SetWindowText(hWnd,"eDataServerV2 ALFA[0.1]");

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message) 
	{
		case WM_SIZE:
			{
				RECT rc;
				GetClientRect(g_hWnd,&rc);

				MoveWindow(g_hTab,rc.left,rc.top,rc.right,rc.bottom,FALSE);

				RECT tbrc;
				TabCtrl_GetItemRect(g_hTab,0,&tbrc);

				MoveWindow(g_hLog,rc.left,rc.top,rc.right,rc.bottom - (rc.bottom - (tbrc.top -2)), FALSE);
            } 
            break;
		case WM_TIMER:
			{
				switch(wParam)
				{
				case WM_LOGTIMER:
					{

					}
					return TRUE;
				case WM_LOG_DATE_CHANGE:
					{

					}
					return true;
				case WM_BILL_PROCTIMER:
					BillCheckProc();
					break;
				case WM_DROP_STATISTIC:
					UpdateDropStatistics();
					break;
				}
			}
			break;
		case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code)
            {

            }
            return TRUE;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
					DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
					break;
				case ID_RELOAD_CASHSHOP:
//					LoadCashItemList();
					LoadCashShopPackage();
					LoadCashShopProduct();
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_CLOSE:
			if ( MessageBox(NULL, "Are you sure to Close?", "Confirmation", MB_YESNO|MB_ICONQUESTION) == IDYES )
			{
				//DestroyGIocp();
				DestroyWindow(hWnd);
				PostQuitMessage(0);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

HWND WINAPI DoCreateTabControl(HWND hwndParent) 
{ 
    RECT rcClient;
    HWND hwndTab;
    TCITEM tie;
    int i;
 
    // Get the dimensions of the parent window's client area, and 
    // create a tab control child window of that size. 
    GetClientRect(hwndParent, &rcClient); 
    InitCommonControls(); 
    hwndTab = CreateWindow(WC_TABCONTROL, "", 
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_BOTTOM,
        0, 0, rcClient.right, rcClient.bottom, 
        hwndParent, NULL, hInst, NULL); 
    if (hwndTab == NULL)
    { 
        return NULL; 
    }

	char szTabText[100];
 
    // Add tabs for each day of the week. 
    tie.mask = TCIF_TEXT | TCIF_IMAGE; 
    tie.iImage = -1; 
    tie.pszText = szTabText;

	int n = 0;
	
	for (i = 0; i < MAX_SERVERTYPE; i++)
	{
		if(g_bIsServer[i] != TRUE)	continue;

		LoadString(hInst, IDS_JOINSERVER + i,szTabText, sizeof(szTabText)/sizeof(szTabText[0]));
		if (TabCtrl_InsertItem(hwndTab, n, &tie) == -1)
		{
			DestroyWindow(hwndTab); 
			return NULL;
		}
		else
		{
			TabArray[n] = i;
			n++;
		}
    }
    return hwndTab; 
}

HWND WINAPI DoCreateDisplayWindow(HWND hwndParent) 
{ 
    HWND hwndStatic = CreateWindow(szWindowClass, "", 
        WS_CHILD | WS_VISIBLE | WS_BORDER , 
        100, 100, 100, 100,      // Position and dimensions; example only.
        hwndParent, NULL, hInst, NULL); 

    return hwndStatic; 
}