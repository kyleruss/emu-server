#pragma once

#include <windows.h>
#include <stdio.h>

#define DEBUG_OUTPUT	1

namespace DebugOutput
{
	static void WINAPI LogAdd(LPSTR Text, ...)
	{
#if( DEBUG_OUTPUT == 0 )
#else
		char Buffer[2048];
		int BufferLen = sizeof(Buffer);
		ZeroMemory(Buffer, BufferLen);
		va_list ArgList;
		va_start(ArgList, Text);
		int EndLen = vsprintf_s(Buffer, BufferLen, Text, ArgList);
		va_end(ArgList);
		Buffer[EndLen] = '\n';
		OutputDebugStringA(Buffer);
#endif
	}
};