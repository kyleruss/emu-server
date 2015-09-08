#ifndef __SERVER_H__
#define __SERVER_H__

#include "giocp.h"

// Headcode [0x00]
typedef struct
{
	PBMSG_HEAD h;
	unsigned char Type;
	unsigned short Port;
	char ServerName[50];
	unsigned short ServerCode;
}SDHP_SERVERINFO, *LPSDHP_SERVERINFO;

typedef struct
{
	PBMSG_HEAD	h;
	BYTE		Result;
	DWORD		ItemCount;
} SDHP_RESULT, *LPSDHP_RESULT;

extern char SQLUser[64];
extern char SQLPass[64];

enum SERVER_TYPE
{
	ST_JOINSERVER,
	ST_DATASERVER,
	ST_EVENTSERVER,
	ST_RANKINGSERVER,
	ST_CASHSHOPSERVER,
};

#define MAX_SERVERTYPE	(ST_CASHSHOPSERVER + 1)

extern char szLogPath[MAX_SERVERTYPE][20];
extern BOOL g_bIsServer[MAX_SERVERTYPE];

#define OBJECT_PER_SERVERTYPE 10
#define MAX_SERVEROBJECT	(OBJECT_PER_SERVERTYPE * MAX_SERVERTYPE)

typedef struct
{
	int  Connected;
	char Ip[20];
	WORD Port;
	BYTE Type;
	WORD ServerCode;
	BYTE Flag;
	DWORD SType;

	char ServerName[51];
	
	SOCKET m_socket;
	_PER_SOCKET_CONTEXT PerSocketContext;

	void (*ProtocolCore)(int, DWORD, LPBYTE, int);
} ServerObject_Struct, * LPServerObject_Struct;

extern short gSObjAddSearch();
extern void  gSObjInit();
extern short gSObjAdd(SOCKET socket, char *ip, int aIndex ,DWORD S_TYPE);
extern short gSObjDel(int index);
extern BOOL  gSObjSetInfo(int aIndex, WORD port, int type, char * servername);
extern BOOL ServersInit();

extern int g_ServerGroup;
extern ServerObject_Struct gSObj[MAX_SERVEROBJECT];

#endif