// Giocp.h
//------------------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
//------------------------------------------

#ifndef GIOCP_H
#define GIOCP_H


#if (__LICENSED__ == 1)
#pragma comment( lib , "..\\..\\spe\\release\\GSAuth.lib" )
#include "..\..\spe\spe.h"
#else
#include "spe.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define MAX_EXSENDBUF_SIZE	20000
#define MAX_BUFF_SIZE		20000
#define MAX_WORKER_THREAD	100

typedef struct _PER_IO_CONTEXT
{
	OVERLAPPED					Overlapped;
	WSABUF						wsabuf;
	CHAR						Buffer[MAX_BUFF_SIZE];
	CHAR						BufferSecond[MAX_BUFF_SIZE];
	int							nSecondOfs;
	int							nTotalBytes;
	int							nSentBytes;
	int							IOOperation;
	int							nWaitIO;
} PER_IO_CONTEXT, * LPPER_IO_CONTEXT;

typedef struct _PER_SOCKET_CONTEXT
{
	SOCKET			m_socket;
	int				nIndex;
	PER_IO_CONTEXT	IOContext[2];
	long			dwIOCount;
} PER_SOCKET_CONTEXT, *LPPER_SOCKET_CONTEXT;

void GiocpInit();
BOOL IocpServerStart();
int  SocketInit();
int  CreateGIocp(int server_port);
int  DataSend(int aIndex, LPBYTE lpMsg, DWORD size);
BOOL UpdateCompletionPort( SOCKET sd, int ClientIndex, BOOL bAddToList ) ;
void CloseClient ( int index );
void DestroyGIocp();

BOOL	ParsingRecvData(LPPER_IO_CONTEXT	lpIOContext, int uIndex);
void	CloseClient ( LPPER_SOCKET_CONTEXT lpPerSocketContext, BOOL bGraceful ) ;
int		IoSendSecond(LPPER_SOCKET_CONTEXT lpPerSocketContext);
int		IoMoreSend(LPPER_SOCKET_CONTEXT lpPerSocketContext);

DWORD	IocpServerWorker(void);
DWORD   WINAPI IOCPWorkerThread(LPVOID CompletionPortID);
void GiocpDelete();

class CCriticalSec  
{
	CRITICAL_SECTION	sec;

public:

	CCriticalSec();
	~CCriticalSec();

	inline void Lock(){ EnterCriticalSection(&sec); }
	inline void UnLock(){ LeaveCriticalSection(&sec); }
};

class CCSLock  
{
	CCriticalSec*	m_pCriticalSec;
public:
	
	CCSLock(CCriticalSec* pCriticalSec);
	~CCSLock();		
};

#endif