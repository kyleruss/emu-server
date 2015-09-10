#pragma once
// ------------------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
//#define VMPROTECT
// ------------------------------------------------------------------------------------------------

#include <afx.h>
#include <windows.h>
#include <winbase.h>
#include <winsock2.h>
#include "COMMCTRL.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
#include <math.h>
#include <process.h>
#include <io.h>
#include <map>
#include <string>
#include <ios>
#include <algorithm>
#include <vector>
#include <set>
#include <list>
#include <stack>
#include <IPHlpApi.h>
// ------------------------------------------------------------------------------------------------

#include "Message.h"
#include "Define.h"
#include "VMProtectSDK.h"
#include "SecureEngineSDK.h"
#include "ZTHelper.hpp"
// ------------------------------------------------------------------------------------------------

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "COMCTL32.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "..\\lua\\lib\\lua5.1.lib")
#pragma comment(lib, "..\\crypto\\cryptlib.lib")
// ------------------------------------------------------------------------------------------------

#define GAMESERVER_VERSION	"1.0.1.0"
#define GAMESERVER_NAME		"zGameServer"
// ------------------------------------------------------------------------------------------------

#define GS_CASTLE			1
#define GS_SPEC				1
#define __CUSTOM__			1
#define __ENG__				1
// ------------------------------------------------------------------------------------------------

//#define __NOVUS__			0
#define __4GAMERS__			0
#define __CUSTOM_QUEST__	0
#define CUSTOM_WINGS		0
#define CUSTOM_ENCRYPT		1
#define CUSTOM_OLYMP		0
#define CUSTOM_EXOPT		0
#define ANTI_CHEAT			1
#define __NEW_GIOCP__		1
#define FIX_HP				1
#define FIX_MAXSTAT			1
// ------------------------------------------------------------------------------------------------

#define ZEONWINDOW_STAT		TRUE
#define ZEONWINDOW			TRUE
#define GAMESERVER_WIDTH	750
#define GAMESERVER_HEIGHT	600
#define __QUEST_SYSTEM__	1
#define __GENS_SYSTEM__		1
//#define POINTEX
//#define BEREZNUK
//#define STREAM
//#define OPTIMIZE
//#define SILVER
// ------------------------------------------------------------------------------------------------