#pragma once
// -------------------------------------------------------------------------------

//User:
#define pPlayerState			*(int*)0x11C2320
#define oBattleMapStart			0x009EB073
#define oBattleZoneStart		0x005C13F0
#define pMaxBattleZoneCount		*(BYTE*)(0x005C1420 + 3)
#define oIsBattleServer_Call1	0x005C13E7
#define oIsBattleServer_Call2	0x00827ABC
#define oIsBattleServer_Call3	0x009EB05F
#define oUserObjectStruct		*(int*)0x84CF3DC
#define oUserObjectStruct_		*(int*)0x84CF3D8
#define pIsMasterClass			((bool(__cdecl*)(BYTE Class)) 0x0058ABC0)
// -------------------------------------------------------------------------------

//Interface:
#define oProcDrawUI_Call		0x004E0A34
#define pProcDrawUI				((void(__cdecl*)()) 0x004DCB9B)
#define pDrawGUI				((void(__cdecl*)(DWORD, float, float, float, float)) 0x0079732F)
#define pDrawColorText			((int(__cdecl*)(LPCTSTR Text, int PosX, int PosY, int Width, int Arg5, int Color, int Arg7, int Align)) 0x007CECC4)
#define pGLEnable				((void(__cdecl*)(BYTE Mode)) 0x006275FB)
#define pGLDisable				((void(__cdecl*)()) 0x00627581)
#define pMessageBoxThis			((DWORD(__cdecl*)()) 0x0098CE32)
#define pGetMessageBox			((LPVOID(__thiscall*)(LPVOID This)) 0x004A021A)
#define pDrawMessageBox			((void(__thiscall*)(LPVOID This, DWORD Arg1, DWORD Arg2, LPSTR Text, LPSTR Title, DWORD Arg5, DWORD Arg6, DWORD Arg7)) 0x009D8342)
#define pScaleFormAction		((void(__cdecl*)(DWORD Form, LPSTR ElementName, LPSTR Command)) 0x0097E579)

#define pDrawColorButton		((void(__cdecl*)(DWORD, float, float, float, float, float, float, DWORD)) 0x007976C8)
#define pGetMoneyFormat			((int(__cdecl*)(double Value, char * Buff, int Arg3)) 0x005B8D42)
#define pDrawColorText			((int(__cdecl*)(LPCTSTR Text, int PosX, int PosY, int Width, int Arg5, int Color, int Arg7, int Align)) 0x007CECC4)
#define	pCursorX				*(int*)0x8B4ACFC
#define	pCursorY				*(int*)0x8B4AD00
#define pWinWidth				*(GLsizei*)0x11C3E18
#define pWinHeight				*(GLsizei*)0x11C3E1C
#define pDrawToolTip			((int(__cdecl*)(int X, int Y, LPCSTR Text)) 0x0059599A)
// -------------------------------------------------------------------------------

//Other:
#define oVulcanusMove			(0x009EB540 + 2)
#define pGameWindow				*(HWND*)(0x11F9634)
#define pGameLoad				((int(__cdecl*)()) 0x004D0FC0
#define oServerIP				0x011C2AD2
#define oServerPort				0x11C231C
#define oClientVersion			0x11C3F28
#define oSerialOffset			0x11C3F30
// -------------------------------------------------------------------------------

//Protocol:
#define pActiveSocket			*(SOCKET*)(*(DWORD*)0x11C3F44 + 0x0c)
// -------------------------------------------------------------------------------