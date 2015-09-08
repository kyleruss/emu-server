// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define _WIN32_WINNT 0x0500
#pragma warning ( disable : 4786 )	// Disable Warning of Large Debuf Strings ( truncated to 255 len )

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#include <afxwin.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
#include <map>
#include <Winbase.h>
#include <sql.h>
#include <sqlext.h>
#include <Commctrl.h>
#include <vector>

// Local Header Files
#include "ProDef.h"

#define APP_NAME		"zMultiServer"
#define	APP_VERSION		"1.0.0.0"

#define S6EP2				0
#define EX700				1
#define FIX_MAXSTAT			1
#define ZTEAMDB

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
