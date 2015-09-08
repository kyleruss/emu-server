
#if !defined(AFX_EDATASERVER2_H__598207D7_3BF2_466E_98D6_B90A70F463C2__INCLUDED_)
#define AFX_EDATASERVER2_H__598207D7_3BF2_466E_98D6_B90A70F463C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#define INI_FILE		".\\Settings.ini"
#define INI_SECTION		"Main"

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HWND WINAPI DoCreateTabControl(HWND hwndParent);
HWND WINAPI DoCreateDisplayWindow(HWND hwndParent);
HWND WINAPI DoCreateStatusBar(HWND hwndParent, int nStatusID, HINSTANCE hinst, int nParts);

#define WM_LOGTIMER						1000
#define WM_LOG_DATE_CHANGE				1003
#define WM_BILL_PROCTIMER					1004
#define WM_DROP_STATISTIC				1005
#define MAX_IDSTRING 10

extern HWND g_hWnd;

extern bool g_CustomHardcoreMode; //Custom
extern short g_CustomHardcoreDefaultLife; //Custom

#endif // !defined(AFX_EDATASERVER2_H__598207D7_3BF2_466E_98D6_B90A70F463C2__INCLUDED_)
