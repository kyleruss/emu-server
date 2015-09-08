#include "stdafx.h"
#include "main.h"

FILE_CHECK_STRUCT_ pCheckFile[] = {
"",0,0,
};

LPBYTE _GetTickCount = (LPBYTE)GetProcAddress(GetModuleHandle("kernel32.dll"),"GetTickCount");
LPBYTE _QueryPerformanceCounter = (LPBYTE)GetProcAddress(GetModuleHandle("kernel32.dll"),"QueryPerformanceCounter");
LPBYTE _timeGetTime = (LPBYTE)GetProcAddress(GetModuleHandle("winmm.dll"),"timeGetTime");
LPBYTE _send = (LPBYTE)GetProcAddress(GetModuleHandle("ws2_32.dll"),"send");
LPBYTE _recv = (LPBYTE)GetProcAddress(GetModuleHandle("ws2_32.dll"),"recv");


void HookBYTE(LPVOID Offset,BYTE value,bool bProtect)
{
	VMBEGIN
	DWORD OldProtect;
	VirtualProtect(Offset,sizeof(BYTE),PAGE_EXECUTE_READWRITE,&OldProtect);
	*(BYTE*)(Offset) = value;
	if( bProtect )
		VirtualProtect(Offset,1,PAGE_EXECUTE_READ,&OldProtect);
	VMEND
}

void HookWORD(LPVOID Offset,WORD value,bool bProtect)
{
	VMBEGIN
	DWORD OldProtect;
	VirtualProtect(Offset,sizeof(WORD),PAGE_EXECUTE_READWRITE,&OldProtect);
	*(WORD*)(Offset) = value;
	if( bProtect )
		VirtualProtect(Offset,sizeof(WORD),PAGE_EXECUTE_READ,&OldProtect);
	VMEND
}

void HookDWORD(LPVOID Offset,DWORD value,bool bProtect)
{
	VMBEGIN
	DWORD OldProtect;
	VirtualProtect(Offset,sizeof(DWORD),PAGE_EXECUTE_READWRITE,&OldProtect);
	*(DWORD*)(Offset) = value;
	if( bProtect )
		VirtualProtect(Offset,sizeof(DWORD),PAGE_EXECUTE_READ,&OldProtect);
	VMEND
}

void HookSET(LPVOID Offset,BYTE value,int size,bool bProtect)
{
	VMBEGIN
	DWORD OldProtect;
	VirtualProtect(Offset,size,PAGE_EXECUTE_READWRITE,&OldProtect);
	memset(Offset,value,size);
	if( bProtect )
		VirtualProtect(Offset,size,PAGE_EXECUTE_READ,&OldProtect);
	VMEND
}

void HookBUFFER(LPVOID Offset,LPBYTE value,int size,bool bProtect)
{
	VMBEGIN
	DWORD OldProtect;
	VirtualProtect(Offset,size,PAGE_EXECUTE_READWRITE,&OldProtect);
	memcpy(Offset,value,size);
	if( bProtect )
		VirtualProtect(Offset,size,PAGE_EXECUTE_READ,&OldProtect);
	VMEND
}

void HookDWORD64(LPVOID Offset,DWORD64 value,bool bProtect)
{
	VMBEGIN
	DWORD OldProtect;
	VirtualProtect(Offset,sizeof(DWORD64),PAGE_EXECUTE_READWRITE,&OldProtect);
	*(DWORD64*)(Offset) = value;
	if( bProtect )
		VirtualProtect(Offset,sizeof(DWORD64),PAGE_EXECUTE_READ,&OldProtect);
	VMEND
}

void HookNoAccess(LPVOID Offset,int size)
{
	VMBEGIN
	DWORD OldProtect;
	VirtualProtect(Offset,size,PAGE_READONLY,&OldProtect);
	VMEND
}


SOCKET g_s = INVALID_SOCKET;

int TMainClass::MySend(SOCKET s,const char* buf,int len,int flags)
{
	LPBYTE pBuff = (LPBYTE)buf;

	if( !(pBuff[0] == 0xC1 && pBuff[2] == 0xF4)  )
	{
		if( *(DWORD*)(WINSOCK_CLASS_ADDR) == WINSOCK_CLASS_DEFAULT )
		{
			VMBEGIN
			if( s != INVALID_SOCKET && (g_s == INVALID_SOCKET || g_s != s))
				g_s = s;
			VMEND

			if( pBuff[0] == 0xC1 || pBuff[0] == 0xC3 )	{
				pBuff[1] ^= 0x10;
				pBuff[2] ^= 0x40;
			}
			else	{
				pBuff[1] ^= 0x11;
				pBuff[2] ^= 0x41;
				pBuff[3] ^= 0x31;
			}
			pBuff[0] ^= 0x20;
		}
	}

	return send(s,buf,len,flags);
}

DWORD GetFileCrc(char* szFileName,DWORD KeyValue)
{
	HANDLE hFile = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

	if( hFile == INVALID_HANDLE_VALUE )
	{
		return -1;
	}

	int bResult;
	DWORD ReadSize = NULL;
	TCrc32Calc gCRC32((WORD)KeyValue);

	do
	{
		LPBYTE FileData = new BYTE[0x20000];

		bResult = ReadFile(hFile,FileData,0x20000,&ReadSize,NULL);

		gCRC32.CrcCalcBlock(FileData,ReadSize);
		delete [] FileData;
	}
	while( bResult == TRUE && GetLastError() != ERROR_HANDLE_EOF && ReadSize == 0x20000);
	CloseHandle(hFile);

	
return gCRC32.GetCRC32();
}

DWORD CheckFileTick = NULL;

void CheckFileIntegrity()
{
	if( (GetTickCount() - CheckFileTick) < 60000 )
		return;

	CheckFileTick = GetTickCount();

	VMBEGIN

	for(int i = 0; i < sizeof(pCheckFile)/sizeof(FILE_CHECK_STRUCT_); i++)
	{
		if( pCheckFile[i].szPath[0] == '\0' )
			continue;

		if( GetFileCrc(pCheckFile[i].szPath,pCheckFile[i].KeyVal) != pCheckFile[i].CRC32 )
		{
			MessageBox(NULL,"Некоторы?файл?повреждены, обновите ва?клиент!","[ERROR]",NULL);
			ExitProcess(0);
		}
	}
	VMEND
}
//0043F292
#define pMenuExitEvent			((DWORD(__cdecl*)(int Arg1)) 0x7A8880)

BOOL SpeedHackCheck()
{
	VMBEGIN
#ifdef VMPROTECT
	if( VMProtectIsDebuggerPresent(true) )
	{
		ExitProcess(0);
	}
#endif
	PMSG_CHECK_ANTICHEAT pTmp = {0};
	pTmp.c = 0xE6;
	pTmp.size = sizeof(pTmp);
	pTmp.headcode = 0x10;
	pTmp.type = rand() % 2;
	WORD wSource = 0;

#if ( __DEBUG__ == 0 )
	if( _timeGetTime != NULL )
	{
		if( _timeGetTime[0] == 0xE9 || _timeGetTime[0] == 0x90 || _timeGetTime[2] == 0xE9 )
		{
#if (CHEATGUARD_DOWNGRADE == 1)
			return true;
#else
			pTmp.IsSpeedEditor = true;
#endif
		}
	}
	else
	{
		return TRUE;
	}

	if( _QueryPerformanceCounter != NULL )
	{
		if( _QueryPerformanceCounter[0] == 0xE9 || _QueryPerformanceCounter[0] == 0x90 )
		{
#if (CHEATGUARD_DOWNGRADE == 1)
			return true;
#else
			pTmp.IsSpeedEditor = true;
#endif
		}
	}
	else
	{
		return TRUE;
	}

	if( _GetTickCount != NULL )
	{
		if( _GetTickCount[0] == 0xE9 || _GetTickCount[0] == 0x90 )
		{
#if (CHEATGUARD_DOWNGRADE == 1)
			return true;
#else
			pTmp.IsSpeedEditor = true;
#endif
		}
	}
	else
	{
		return TRUE;
	}

	if( _send != NULL )
	{
		if( _send[0] == 0xE9 || _send[0] == 0x90 )
		{
#if (CHEATGUARD_DOWNGRADE == 1)
			return true;
#else
			pTmp.IsSpeedEditor = true;
#endif
		}
	}
	else
	{
		return TRUE;
	}

	if( _recv != NULL )
	{
		if( _recv[0] == 0xE9 || _recv[0] == 0x90 )
		{
#if (CHEATGUARD_DOWNGRADE == 1)
			return true;
#else
			pTmp.IsSpeedEditor = true;
#endif
		}
	}
	else
	{
		return TRUE;
	}
#endif

	do
	{
		 wSource = rand()%1024;
	}
	while( wSource == 0 );

	WORD wRand = rand()%64;

	if( pTmp.type == 1 )	{
		pTmp.wWord1 = wSource;
		pTmp.wWord1 ^= 0x8AFC;
		pTmp.wWord2 = ((wSource & 0x3F0) * 64) | (wRand *16) | (wSource & 0x0F);
		pTmp.wWord2 ^= 0xA8CF;
	}
	else	{
		pTmp.wWord2 = wSource;
		pTmp.wWord2 ^= 0x8AFC;
		pTmp.wWord1 = ((wSource & 0x3F0) * 64) | (wRand *16) | (wSource & 0x0F);
		pTmp.wWord1 ^= 0xA8CF;
	}

	if( *(DWORD*)(PLAYER_CONST1) != NULL && pPlayerState == 5 )
	{
		//pTmp.wAttackSpeed1 = *(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x54) ^ 0xDEFA;
		//pTmp.wAttackSpeed2 = *(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x60) ^ 0xAFDA;

		if( *(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x44)&1 )
		{
			pTmp.wAttackSpeed1 = (*(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x54)-20) ^ 0xDEFA;
			pTmp.wAttackSpeed2 = (*(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x60)-20) ^ 0xAFDA;
		}
		else if( *(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x44)&8 )
		{
			pTmp.wAttackSpeed1 = (*(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x54)-20) ^ 0xDEFA;
			pTmp.wAttackSpeed2 = (*(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x60)-20) ^ 0xAFDA;
		}
		else
		{
			pTmp.wAttackSpeed1 = (*(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x54)) ^ 0xDEFA;
			pTmp.wAttackSpeed2 = (*(WORD*)(*(DWORD*)(PLAYER_CONST1)+0x60)) ^ 0xAFDA;
		}
	}

	if( g_s != INVALID_SOCKET && *(DWORD*)(WINSOCK_CLASS_ADDR) == WINSOCK_CLASS_DEFAULT )
		send(g_s,(const char*)&pTmp,pTmp.size,NULL);

	return FALSE;

	VMEND
}