#pragma once

#include "Defines.h"

#define MAX_OBJECT			100
#define MAX_WIN_WIDTH		640
#define MAX_WIN_HEIGHT		480

enum ObjectID
{
	eRESET_MAIN,
	eRESET_TITLE,
	eRESET_FRAME,
	eRESET_FOOTER,
	eRESET_DIV,
	eRESET_INFOBG,
	eRESET_MONEYBG,
	eRESET_CLOSE,
	eRESET_FINISH,
	eRESET_CREDITS,
};

enum ColorMacro
{
	eShinyGreen		= Color4f(172, 255, 56, 255),
	eGold			= Color4f(255, 189, 25, 255),
	eWhite			= Color4f(255, 255, 255, 255),
	eWhite180		= Color4f(255, 255, 255, 180),
	eOrange			= Color4f(255, 105, 25, 255),
	eGray100		= Color4f(50, 50, 50, 100),
	eGray150		= Color4f(50, 50, 50, 150),
	eBlowPink		= Color4f(220, 20, 60, 255),
	eRed			= Color4f(225, 0, 0, 255),
	eExcellent		= Color4f(0, 225, 139, 255),
	eAncient		= Color4f(1, 223, 119, 255),
	eSocket			= Color4f(153, 102, 204, 255),
	eBlue			= Color4f(36, 242, 252, 255),
	eYellow			= Color4f(255, 255, 0, 255),
	eBrown			= Color4f(69, 39, 18, 255),
};

struct InterfaceObject
{
	DWORD	ModelID;
	float	Width;
	float	Height;
	float	X;
	float	Y;
	float	MaxX;
	float	MaxY;
	DWORD	EventTick;
	bool	OnClick;
	bool	OnShow;
	BYTE	Attribute;
};

class Interface
{
public:
	void	Load();

	static void	ProcUI();
	void BindObject(short ObjectID, DWORD ModelID, float Width, float Height, float X, float Y);
	
	void OpenResetWindow() { this->Data[eRESET_MAIN].OnShow = true; *(DWORD*)(0x8B4AC46) = false; };
	void CloseResetWindow() { this->Data[eRESET_MAIN].OnShow = false; };
	bool CheckResetWindow() { return this->Data[eRESET_MAIN].OnShow; };
	void DrawResetWindow();
	bool EventResetWindow_Main(DWORD Event);
	bool EventResetWindow_Close(DWORD Event);
	bool EventResetWindow_Finish(DWORD Event);
	bool EventResetWindow_Credits(DWORD Event);

	void DrawGUI(short ObjectID, float PosX, float PosY);
	float DrawRepeatGUI(short ObjectID, float X, float Y, int Count);
	void DrawColoredGUI(short ObjectID, float X, float Y, DWORD Color);
	int DrawFormat(DWORD Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, ...);
	int	DrawToolTip(int X, int Y, LPCSTR Text, ...);

	void NotifyBox(LPSTR Title, LPSTR Text);
	static void ScaleFormAction(DWORD Form, LPSTR ElementName, LPSTR Command, ...);

	bool IsWorkZone(short ObjectID);
	bool IsWorkZone(float X, float Y, float MaxX, float MaxY);
	float GetResizeX(short ObjectID);

public:
	InterfaceObject Data[MAX_OBJECT];

}; extern Interface g_Interface;

void _BlockInterface4();
void _BlockInterface3();
void _BlockInterface2();
void _BlockInterface1();

class MUText
{
public:
	static MUText* GetInstance() { return (MUText*)0x84CF3F0; };
	const LPSTR GetLine(int Number);
};