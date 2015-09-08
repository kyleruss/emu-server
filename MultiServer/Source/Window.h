#pragma once
// -------------------------------------------------------------------------------

#include "Resource.h"
// -------------------------------------------------------------------------------

BOOL CALLBACK Dialog_Main(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);
// -------------------------------------------------------------------------------

namespace Log
{
	enum T
	{
		Info	= 0,
		Warning = 1,
		Error	= 2,
		Debug	= 3,
	};
};
// -------------------------------------------------------------------------------

class Window
{
public:
			Window();
	virtual	~Window();
	// ----
	void	InitInstance(HINSTANCE Out)
	{
			this->m_Instance = Out;
			DialogBoxParam(this->m_Instance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, Dialog_Main, NULL);
	}
	// ----
	void	TitleSet(HWND Window, LPSTR Text, ...);
	void	LogAdd(BYTE Type, LPSTR Text, ...);
	void	ServerLogAdd(DWORD ServerType, LPSTR Text, ...);
	// ----
public:
	HWND		m_WindowID;
	HINSTANCE	m_Instance;
	// ----
}; extern Window g_Window;
// -------------------------------------------------------------------------------