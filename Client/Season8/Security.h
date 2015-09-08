#pragma once

#define WINSOCK_CLASS_ADDR		0x11C3F44
#define WINSOCK_CLASS_DEFAULT	0x08B4B080

struct FILE_CHECK_STRUCT_
{
  char szPath[MAX_PATH];
  WORD KeyVal;
  DWORD CRC32;
};


void PUSH_3A8CharSetFix();
void CharSetFix();
void HookBYTE(LPVOID Offset,BYTE value,bool bProtect=true);
void HookWORD(LPVOID Offset,WORD value,bool bProtect=true);
void HookDWORD(LPVOID Offset,DWORD value,bool bProtect=true);
void HookDWORD64(LPVOID Offset,DWORD64 value,bool bProtect=true);
void HookSET(LPVOID Offset,BYTE value,int size,bool bProtect=true);
void HookBUFFER(LPVOID Offset,LPBYTE value,int size,bool bProtect=true);
BOOL SpeedHackCheck();
void SendHWID();
void CheckFileIntegrity();

void RemoveGG();
void InitConnection();
LONG CALLBACK make_minidump(EXCEPTION_POINTERS* e);

extern SOCKET g_s;

class TCrc32Calc
{
public:
	TCrc32Calc(WORD KeyVal)
	{
		gCount = 0;
		KEY = KeyVal;
		CRC32 = KEY*512;
	}
	virtual ~TCrc32Calc() {};

	DWORD GetCRC32()
	{
		return CRC32;
	}

	void TCrc32Calc::CrcCalcBlock(BYTE * Buffer,int size)
	{
		for(int i = 0; i <= (size - 4); i+=4,gCount+=4)
		{
			int tmpbuff;
			memcpy(&tmpbuff,&Buffer[i],sizeof(int));
			
			switch( (((gCount / 4) + KEY) % 2) )	{
			case 0:
				CRC32 ^= tmpbuff;
				break;
			case 1:
				CRC32 += tmpbuff;
				break;
			}

			if((gCount % 16) == FALSE)
				CRC32 ^= (KEY + CRC32) >> (((gCount /4) % 8) + 1);
		}
	}

public:
	WORD KEY;
	DWORD CRC32;

	LONGLONG gCount;
};