#pragma once
// -------------------------------------------------------------------------------

#include "targetver.h"
// -------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN 
// -------------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <WinSock2.h>
#include <stdlib.h>
#include <time.h>
#include <ShellAPI.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
// -------------------------------------------------------------------------------

//#define VMPROTECT
// -------------------------------------------------------------------------------

#include "VMProtectSDK.h"
#include "Defines.h"
#include "Import.h"
#include "Object.h"
#include "SecuredMemory.h"
#include "DebugOutput.h"
// -------------------------------------------------------------------------------

using namespace SecuredMemory;
using namespace DebugOutput;
// -------------------------------------------------------------------------------

#pragma comment(lib, "WSOCK32.lib")
#pragma comment(lib, "opengl32.lib")
// -------------------------------------------------------------------------------

#define __ROOT__	0
#define __DEBUG__	0
//#define __NOVUS__
#define __FIX_HP__			1
#define	__CUSTOM_WINGS__	0
#define __CUSTOM_OLYMP__	0
#define	__NEWOPTION__		0
#define ZTEAM
#define CHEATGUARD_DOWNGRADE 0
#define COMMONZ_DOWNGRADE	0
// -------------------------------------------------------------------------------