#include "stdafx.h"
#include "main.h"
#include "Camera.h"
#include "Interface.h"
#include "User.h"
#include "Crack.h"
// -------------------------------------------------------------------------------

HINSTANCE hDLLInst;
// -------------------------------------------------------------------------------

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	hDLLInst = hModule;
	// ----
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_Crack.Load();
		break;
	case DLL_PROCESS_DETACH:
		KeyboardSetHook(false);
		break;
	}
	return true;
}
// -------------------------------------------------------------------------------