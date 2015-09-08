// Giocp.cpp
//------------------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
//------------------------------------------

// GS-N 0.99.60T 0x00473020 - Status : Completed :)
//	GS-N	1.00.18	JPN	0x00489FD0	-	Completed
#include "stdafx.h"
#include <winsock2.h>


#include "giocp.h"
#include "logproc.h"
#include "GameMain.h"
#include "GameServer.h"
#include "user.h"
#include "GameEvent.h"

#if( ENABLETEST_CHEATGUARD == 1 ) 
#include "CheatGuard.h"
#endif

CCriticalSec::CCriticalSec()
{
	#if _WIN32_WINNT >= 500
		InitializeCriticalSectionAndSpinCount(&sec, 4000 );
	#else 
		InitializeCriticalSection(&sec);	
	#endif	
}

CCriticalSec::~CCriticalSec()
{
	DeleteCriticalSection(&sec); 
}


CCSLock::CCSLock(CCriticalSec* pCriticalSec)
{
	if( !pCriticalSec )
		return;

	m_pCriticalSec = pCriticalSec;

	m_pCriticalSec->Lock();	
}

CCSLock::~CCSLock()
{
	m_pCriticalSec->UnLock();
}

char	ErrorMsg[512];
void	GetLastErrorMsg()
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	strcpy_s(ErrorMsg, sizeof(ErrorMsg), (LPCTSTR)lpMsgBuf);
	LocalFree( lpMsgBuf );
}



enum {
	RECV_IO,
	SEND_IO
} SOCKET_FLAG;


CCriticalSec	 g_CS;
CCriticalSec	 g_SendLock;

DWORD           g_dwThreadCount = 0;        //worker thread count 
HANDLE          g_ThreadHandles[MAX_WORKER_THREAD]; 
SOCKET			g_Listen = INVALID_SOCKET;
int				g_ServerPort;
HANDLE			g_CompletionPort=NULL;
HANDLE			g_IocpThreadHandle;

unsigned char* ExSendBuf;

void GiocpInit()
{
	ExSendBuf=new unsigned char[MAX_EXSENDBUF_SIZE];
}

void GiocpDelete()
{
	delete[] ExSendBuf;
}

BOOL IocpServerStart()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
	return TRUE;
}

int CreateGIocp(int server_port)
{
VM_START
	g_ServerPort = server_port;

	DWORD ThreadID;

	g_IocpThreadHandle = CreateThread( NULL, 0,(LPTHREAD_START_ROUTINE)IocpServerWorker,0, 0, &ThreadID);
	
VM_END
	
	if ( g_IocpThreadHandle == NULL )
	{
		LogAdd("CreateThread() failed with error %d", GetLastError());
		return FALSE;
	}

	return TRUE;
}

void DestroyGIocp()
{
	for(DWORD dwCPU = 0; dwCPU < g_dwThreadCount; dwCPU++)
	{
		TerminateThread(g_ThreadHandles[dwCPU], 0);
	}
	TerminateThread(g_IocpThreadHandle, 0);

	if( g_CompletionPort )
	{
		CloseHandle(g_CompletionPort);
		g_CompletionPort = NULL;
	}
}

BOOL CreateListenSocket(void) 
{	
	SOCKADDR_IN InternetAddr;
	int			nRet;

	CCSLock cs(&g_CS);
	
	g_Listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	if (g_Listen == INVALID_SOCKET)
	{
		LogAdd("WSASocket() failed with error %d", WSAGetLastError());
		return FALSE;
	}

	InternetAddr.sin_family			= AF_INET;
	InternetAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	InternetAddr.sin_port			= htons(g_ServerPort);

	nRet = bind(g_Listen, (PSOCKADDR) &InternetAddr, sizeof(InternetAddr));

	if( nRet == SOCKET_ERROR )
	{
		MsgBox(lMsg.Get(MSGGET(2, 55)));
		SendMessage(ghWnd, WM_CLOSE, 0,0);
		return FALSE;
	}
	
	nRet = listen(g_Listen, SOMAXCONN);

	if (nRet == SOCKET_ERROR)
	{
		LogAdd("listen() failed with error %d", WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}


DWORD IocpServerWorker(void)
{
	SYSTEM_INFO SystemInfo;
	DWORD		ThreadID;
	SOCKET		Accept;
	int			nRet;
	int			ClientIndex;
	SOCKADDR_IN cAddr;
	IN_ADDR		cInAddr;
	int			cAddrlen = sizeof( cAddr );

	LPPER_SOCKET_CONTEXT lpPerSocketContext = NULL; 

	DWORD RecvBytes;
	DWORD Flags=0;

	GetSystemInfo(&SystemInfo);
	g_dwThreadCount = SystemInfo.dwNumberOfProcessors * 2;

//	VMBEGIN
/*	if( g_CheckedLicense.getData() != 1 )
	{
		return 0;
	}*/
//	VMEND
	
    __try  
    { 
		g_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if ( g_CompletionPort == NULL)
		{
			//////g_CS.Lock();
			LogAdd("CreateIoCompletionPort failed with error: %d", GetLastError());
			//////g_CS.UnLock();
			__leave;
		}
		
		for(DWORD dwCPU = 0; dwCPU < g_dwThreadCount; dwCPU++)
		{
			HANDLE ThreadHandle;
			
			ThreadHandle = CreateThread(NULL, 0, IOCPWorkerThread, g_CompletionPort, 0, &ThreadID);
			if ( ThreadHandle == NULL)
			{
				//////g_CS.Lock();
				LogAdd("CreateThread() failed with error %d", GetLastError());
				//////g_CS.UnLock();
				__leave;
			}
			g_ThreadHandles[dwCPU] = ThreadHandle;
		}
		
		if (!CreateListenSocket() )
			__leave;

		while(TRUE)		
		{
			Accept = WSAAccept(g_Listen, (LPSOCKADDR)&cAddr, &cAddrlen, NULL, 0);

			if (Accept==SOCKET_ERROR)
			{				
				//////g_CS.Lock();
				GetLastErrorMsg();				
				LogAdd("WSAAccept() failed with error %d, %s", WSAGetLastError(), ErrorMsg);
				//////g_CS.UnLock();
				__leave;
			}

			memcpy( &cInAddr, &cAddr.sin_addr.s_addr, sizeof(cInAddr) );

			ClientIndex = gObjAddSearch(Accept, inet_ntoa(cInAddr));

			if( ClientIndex == -1 ) 
			{
				//////g_CS.Lock();
				LogAddL("error-L2 : ClientIndex = -1");
				closesocket(Accept);
				//////g_CS.UnLock();
				continue;
			}

			if( UpdateCompletionPort(Accept, ClientIndex, TRUE) == FALSE )
			{
				//////g_CS.Lock();
				GetLastErrorMsg();
				LogAddL("CreateIoCompletionPort failed with error %d, %s", GetLastError(), ErrorMsg);
				closesocket(Accept);
				//////g_CS.UnLock();
				continue;
			}

			if( gObjAdd(Accept,inet_ntoa(cInAddr), ClientIndex) == -1 )
			{
				//////g_CS.Lock();
				LogAddL("error-L1 : %d %d gObjAdd() failed with error %d", Accept, ClientIndex, GetLastError() );
				closesocket(Accept);
				//////g_CS.UnLock();
				continue;
			}
			
			memset(&gObj[ClientIndex].PerSocketContext->IOContext[0].Overlapped, 0, sizeof(OVERLAPPED));
			memset(&gObj[ClientIndex].PerSocketContext->IOContext[1].Overlapped, 0, sizeof(OVERLAPPED));
			gObj[ClientIndex].PerSocketContext->IOContext[0].wsabuf.buf = gObj[ClientIndex].PerSocketContext->IOContext[0].Buffer;
			gObj[ClientIndex].PerSocketContext->IOContext[0].wsabuf.len = MAX_BUFF_SIZE;
			gObj[ClientIndex].PerSocketContext->IOContext[0].nTotalBytes = 0;
			gObj[ClientIndex].PerSocketContext->IOContext[0].nSentBytes = 0;
			gObj[ClientIndex].PerSocketContext->IOContext[0].nWaitIO    = 0;
			gObj[ClientIndex].PerSocketContext->IOContext[0].nSecondOfs = 0;
			gObj[ClientIndex].PerSocketContext->IOContext[0].IOOperation = RECV_IO;
			
			gObj[ClientIndex].PerSocketContext->IOContext[1].wsabuf.buf = gObj[ClientIndex].PerSocketContext->IOContext[0].Buffer;
			gObj[ClientIndex].PerSocketContext->IOContext[1].wsabuf.len = MAX_BUFF_SIZE;
			gObj[ClientIndex].PerSocketContext->IOContext[1].nTotalBytes= 0;
			gObj[ClientIndex].PerSocketContext->IOContext[1].nSentBytes = 0;
			gObj[ClientIndex].PerSocketContext->IOContext[1].nWaitIO    = 0;
			gObj[ClientIndex].PerSocketContext->IOContext[1].nSecondOfs = 0;
			gObj[ClientIndex].PerSocketContext->IOContext[1].IOOperation = SEND_IO;
			gObj[ClientIndex].PerSocketContext->m_socket = Accept;
			gObj[ClientIndex].PerSocketContext->nIndex   = ClientIndex;

			nRet = WSARecv(Accept, &(gObj[ClientIndex].PerSocketContext->IOContext[0].wsabuf), 1, &RecvBytes, &Flags,
				&(gObj[ClientIndex].PerSocketContext->IOContext[0].Overlapped), NULL);
			
			if( nRet == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING )
			{
				//////g_CS.Lock();
				GetLastErrorMsg();
				LogAdd("WSARecv() failed with error %d, %s", WSAGetLastError(), ErrorMsg);
				//////g_CS.UnLock();
				
				gObj[ClientIndex].PerSocketContext->IOContext[0].nWaitIO = 4;
				CloseClient(gObj[ClientIndex].PerSocketContext, FALSE);
				continue;
			}
			else
			{
				gObj[ClientIndex].PerSocketContext->IOContext[0].nWaitIO = 1;
				gObj[ClientIndex].PerSocketContext->dwIOCount++;

				SCPJoinResultSend(ClientIndex, 1);
			}
		}	// while
	}	// __try
	__finally  
	{
		if( g_CompletionPort )
		{
			for (DWORD i = 0; i < g_dwThreadCount; i++) 
				PostQueuedCompletionStatus(g_CompletionPort, 0, 0, NULL); 
		}
		if( g_CompletionPort )
		{
			CloseHandle(g_CompletionPort);
			g_CompletionPort = NULL;
		}
		if (g_Listen != INVALID_SOCKET)  
		{ 
            closesocket(g_Listen);  
            g_Listen = INVALID_SOCKET; 
		} 
	}
	return TRUE;
}

void IOCPRecvSignal(LPPER_SOCKET_CONTEXT lpPerSocketContext, LPPER_IO_CONTEXT lpIOContext, DWORD dwIoSize)
{	
	DWORD	RecvBytes = 0;
	lpIOContext->nSentBytes += dwIoSize;
	
	if( gObj[lpPerSocketContext->nIndex].Connected < 1 )
	{
		LogAdd("IOCPRecvSignal -> User disconnected - [%d](%s)",
			lpPerSocketContext->nIndex,gObj[lpPerSocketContext->nIndex].Ip_addr);
		return;
	}


	if( !ParsingRecvData(lpIOContext, lpPerSocketContext->nIndex) )
	{
		CloseClient( lpPerSocketContext, 0 );
		lpIOContext->nWaitIO =0;
		return;
	}
	
	lpIOContext->nWaitIO = 0;

	ZeroMemory(&(lpIOContext->Overlapped), sizeof(OVERLAPPED));		
	lpIOContext->wsabuf.len		= MAX_BUFF_SIZE-lpIOContext->nSentBytes;
	lpIOContext->wsabuf.buf		= lpIOContext->Buffer+lpIOContext->nSentBytes;
	lpIOContext->IOOperation	= RECV_IO;		

	DWORD Flags = 0;
	int nRet = WSARecv(lpPerSocketContext->m_socket, &(lpIOContext->wsabuf), 1, &RecvBytes, &Flags,
		&(lpIOContext->Overlapped), NULL);

	if( nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING) )
	{
#if(ENABLE_CUSTOM_OFFLINETRADE == 1)
		if( !gObj[lpPerSocketContext->nIndex].bOffTrade )
		{
			LogAdd("WSARecv() failed with error %d", WSAGetLastError());
			CloseClient( lpPerSocketContext, FALSE);
		}
#else
		LogAdd("WSARecv() failed with error %d", WSAGetLastError());
		CloseClient( lpPerSocketContext, FALSE);
#endif
	}
	else
	{	
		InterlockedIncrement(&lpPerSocketContext->dwIOCount);
		lpIOContext->nWaitIO = 1;
	}
}

void IOCPSendSignal(LPPER_SOCKET_CONTEXT lpPerSocketContext, LPPER_IO_CONTEXT lpIOContext, DWORD dwIoSize)
{
	int aIndex = lpPerSocketContext->nIndex;

	if( aIndex < OBJ_STARTUSERINDEX || aIndex >= OBJMAX ) return;

	lpIOContext->nSentBytes += dwIoSize;
	if( lpIOContext->nSentBytes >= lpIOContext->nTotalBytes )
	{
		lpIOContext->nWaitIO = 0;
		if( lpIOContext->nSecondOfs > 0 )
		{
			IoSendSecond(lpPerSocketContext);
		}
	}
	else
	{
		IoMoreSend(lpPerSocketContext);
	}
}

void ProcessIOCPSignal(DWORD ClientIndex,  DWORD dwIoSize, LPPER_IO_CONTEXT lpOverlapped)
{	
	LPPER_SOCKET_CONTEXT	lpPerSocketContext = NULL;
	LPPER_IO_CONTEXT		lpIOContext = NULL;

	if( ClientIndex < OBJ_STARTUSERINDEX || ClientIndex >= OBJMAX ) return;

	CCSLock cs(&g_SendLock);

	lpPerSocketContext = gObj[ClientIndex].PerSocketContext;
	
	InterlockedDecrement(&lpPerSocketContext->dwIOCount);

	if( 0 == dwIoSize )
	{
			////g_CS.Lock();
		LogAdd("Connection Closed, dwIoSize == 0 (Index:%d)", lpPerSocketContext->nIndex);
		////g_CS.UnLock();
		CloseClient(lpPerSocketContext, FALSE);
		return;
	}

	lpIOContext = (LPPER_IO_CONTEXT)lpOverlapped;

	if( lpIOContext->IOOperation == SEND_IO )
	{
		IOCPSendSignal(lpPerSocketContext, lpIOContext, dwIoSize);
	}
	else if( lpIOContext->IOOperation == RECV_IO )
	{
		IOCPRecvSignal(lpPerSocketContext, lpIOContext, dwIoSize);
	}
}

DWORD WINAPI IOCPWorkerThread(LPVOID CompletionPortID)
{
	HANDLE	CompletionPort = (HANDLE) CompletionPortID;
	DWORD	dwIoSize;
		
	DWORD	dwSendNumBytes = 0;
	BOOL	bSuccess = FALSE;
	int     ClientIndex;
	
	LPOVERLAPPED			lpOverlapped = NULL; 	

	while(TRUE)
	{
		bSuccess = GetQueuedCompletionStatus(
			CompletionPort,
			&dwIoSize,
			(LPDWORD)&ClientIndex,
			&lpOverlapped,
			INFINITE
		);

		if( !bSuccess )
		{
			if( lpOverlapped != NULL ) 
			{
				int aError = GetLastError();
				if ( (aError != ERROR_NETNAME_DELETED) && 
					 (aError != ERROR_CONNECTION_ABORTED) && 
					 (aError != ERROR_OPERATION_ABORTED) &&
					 (aError != ERROR_SEM_TIMEOUT)  )
				{	
					////g_CS.Lock();
					GetLastErrorMsg();					
					LogAdd("IOCPWorkerThread: GetQueueCompletionStatus( %d , %s)\n", GetLastError(), ErrorMsg);
					////g_CS.UnLock();

					CloseClient( ClientIndex );
					continue;
				}
			}
		}

		ProcessIOCPSignal(ClientIndex, dwIoSize, (LPPER_IO_CONTEXT)lpOverlapped);
	}
	return TRUE;
}

BOOL ParsingRecvData(LPPER_IO_CONTEXT	lpIOContext, int uIndex)
{
	if( lpIOContext->nSentBytes < 3 )
	{
		return FALSE;
	}
	LPBYTE	recvbuf;
	int		lOfs=0;
	int		size=0;
	BYTE	headcode;
	BYTE	xcode;
	BYTE	byDec[7024];
	
	recvbuf = (LPBYTE)lpIOContext->Buffer;

	while(TRUE)
	{
#if (CUSTOM_ENCRYPT == 1)

		recvbuf[lOfs] ^= 0x20;

		if( recvbuf[lOfs] == 0xC1 || recvbuf[lOfs] == 0xC3 )
		{
			recvbuf[lOfs+1] ^= 0x10;
			recvbuf[lOfs+2] ^= 0x40;

			PBMSG_HEAD* lphead = (PBMSG_HEAD*)(recvbuf+lOfs);
			size				= lphead->size;
			headcode			= lphead->headcode;
			xcode				= lphead->c;
		}
		else if( recvbuf[lOfs] == 0xC2 || recvbuf[lOfs] == 0xC4 ) 
		{
			recvbuf[lOfs+1] ^= 0x11;
			recvbuf[lOfs+2] ^= 0x41;
			recvbuf[lOfs+3] ^= 0x31;

			PWMSG_HEAD* lphead	= (PWMSG_HEAD*)(recvbuf+lOfs);
			size				= ((WORD)(lphead->sizeH)<<8);
			size			   |= (WORD)(lphead->sizeL);
			headcode			= lphead->headcode;
			xcode				= lphead->c;
		}
		else if( recvbuf[lOfs] == 0xC6 )
		{
			PBMSG_HEAD* lphead = (PBMSG_HEAD*)(recvbuf+lOfs);
			size				= lphead->size;
			headcode			= lphead->headcode;
			xcode				= lphead->c;
		}
		else
		{
			////g_CS.Lock();
			LogAdd("error-L1 : Header(%02X) error (%s %d)lOfs:%d, size:%d",recvbuf[lOfs],__FILE__, __LINE__,lOfs,lpIOContext->nSentBytes);
			LogAddHeadHexFunc(1,(char*)&recvbuf[lOfs],lOfs);
			////g_CS.UnLock();
			lpIOContext->nSentBytes = 0;
			return FALSE;
		}

#else

		if( recvbuf[lOfs] == 0xC1 || recvbuf[lOfs] == 0xC3 )
		{
			PBMSG_HEAD* lphead = (PBMSG_HEAD*)(recvbuf+lOfs);
			size				= lphead->size;
			headcode			= lphead->headcode;
			xcode				= lphead->c;
		}
		else if( recvbuf[lOfs] == 0xC2 || recvbuf[lOfs] == 0xC4 ) 
		{
			PWMSG_HEAD* lphead	= (PWMSG_HEAD*)(recvbuf+lOfs);
			size				= ((WORD)(lphead->sizeH)<<8);
			size			   |= (WORD)(lphead->sizeL);
			headcode			= lphead->headcode;
			xcode				= lphead->c;
		}
		else
		{
			////g_CS.Lock();
			LogAdd("error-L1 : Header error (%s %d)lOfs:%d, size:%d",__FILE__, __LINE__,lOfs,lpIOContext->nSentBytes);
			LogAddHeadHexFunc(1,(char*)&recvbuf[lOfs],lOfs);
			////g_CS.UnLock();
			lpIOContext->nSentBytes = 0;
			return FALSE;
		}

#endif


		if( size <= 0 )
		{
			////g_CS.Lock();			
			LogAdd("error-L1 : size %d",size);
			////g_CS.UnLock();
			return FALSE;
		}

		if( size <= lpIOContext->nSentBytes )
		{
			if ( xcode == 0xC3 )
			{
				int ret = g_SimpleModulus.Decrypt(&byDec[2],&recvbuf[lOfs+2],size-2);

				if ( ret < 0 )
				{

				}
				else
				{
					unsigned char* pDecBuf = &byDec[1];

					headcode = pDecBuf[2];
					BYTE subhead = byDec[2];
					byDec[1] = 0xC1;
					byDec[2] = (ret&0xFF);

					//g_SimpleModulus.PacketFix(pDecBuf,byDec[3],ret);

					CStreamPacketEngine_Server PacketStream;
					PacketStream.Clear();

					if ( PacketStream.AddData(&byDec[1], ret +2) == 0 )
					{
						////g_CS.Lock();
						LogAddC(2, "error-L1 : CStreamPacketEngine Adding Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
							gObj[uIndex].Ip_addr,gObj[uIndex].AccountID, gObj[uIndex].Name,headcode,__FILE__, __LINE__,gObj[uIndex].Connected);
						////g_CS.UnLock();
						return FALSE;
					}

					if ( PacketStream.ExtractPacket(byDec) != 0 )
					{
						////g_CS.Lock();
						LogAddC(2, "error-L1 : CStreamPacketEngine ExtractPacket Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
							gObj[uIndex].Ip_addr, gObj[uIndex].AccountID, gObj[uIndex].Name, headcode, __FILE__, __LINE__, gObj[uIndex].Connected);
						////g_CS.UnLock();
						return FALSE;
					}
	
					ProtocolCore(headcode, byDec, ret, uIndex, 1, subhead);
				}
			}

			else if ( xcode == 0xC4 )
			{
				int ret = g_SimpleModulus.Decrypt(&byDec[3], &recvbuf[lOfs+3], size-3);
				if ( ret < 0 )
				{

				}
				else
				{
					unsigned char* pDecBuf = &byDec[1];
					
					
					BYTE hcode = byDec[3];
					byDec[1] = 0xC2;
					WORD wSize = (ret &0xFFFF)+3;
					byDec[2] = SET_NUMBERH(wSize);
					byDec[3] = SET_NUMBERL(wSize);

					//g_SimpleModulus.PacketFix(pDecBuf,hcode,ret);

					CStreamPacketEngine_Server PacketStream;
					PacketStream.Clear();

					if ( PacketStream.AddData(&byDec[1], ret+3) == 0 )
					{
						////g_CS.Lock();
						LogAddC(2, "error-L1 : CStreamPacketEngine Adding Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
							gObj[uIndex].Ip_addr, gObj[uIndex].AccountID, gObj[uIndex].Name, headcode, __FILE__, __LINE__, gObj[uIndex].Connected);
						////g_CS.UnLock();
						return FALSE;
					}

					if ( PacketStream.ExtractPacket(byDec) != 0 )
					{
						////g_CS.Lock();
						LogAddC(2, "error-L1 : CStreamPacketEngine ExtractPacket Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
							gObj[uIndex].Ip_addr, gObj[uIndex].AccountID, gObj[uIndex].Name, headcode, __FILE__, __LINE__, gObj[uIndex].Connected);
						////g_CS.UnLock();
						return FALSE;
					}

					headcode = pDecBuf[2];
	
					ProtocolCore(headcode, byDec, ret, uIndex, 1, hcode);
				}
			}
#if (ANTI_CHEAT==1)
			else if( xcode == 0xC6 )
			{
				switch( headcode )
				{
				case 0x10:
					{
#if( ENABLETEST_CHEATGUARD == 1 )
						if (!g_CheatGuard.CG_LiveProc(&gObj[uIndex],(PacketCheatGuardCGClientInfo*)(recvbuf+lOfs),headcode,size))
#else
						if (!gObjSpeedCheck(uIndex,(PMSG_CHECK_ANTICHEAT*)(recvbuf+lOfs),headcode,size))
#endif
						{
							return false;
						}
					}
					break;
				}
			}
#endif
			else
			{
				//g_SimpleModulus.PacketFix(&recvbuf[lOfs],headcode,size);

				CStreamPacketEngine_Server ps;
				ps.Clear();
				
				if ( ps.AddData(&recvbuf[lOfs], size) == 0 )
				{
					////g_CS.Lock();
					LogAddC(2, "error-L1 : CStreamPacketEngine Adding Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
						gObj[uIndex].Ip_addr, gObj[uIndex].AccountID, gObj[uIndex].Name, headcode, __FILE__, __LINE__, gObj[uIndex].Connected);
					////g_CS.UnLock();
					return FALSE;
				}

				if ( ps.ExtractPacket(byDec) != 0 )
				{
					////g_CS.Lock();
					LogAddC(2, "error-L1 : CStreamPacketEngine ExtractPacket Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
						gObj[uIndex].Ip_addr, gObj[uIndex].AccountID, gObj[uIndex].Name, headcode, __FILE__, __LINE__, gObj[uIndex].Connected);
					////g_CS.UnLock();
					return FALSE;
				}

				ProtocolCore(headcode, byDec, size, uIndex, 0, -1);
			}

			lOfs += size;
			lpIOContext->nSentBytes -= size;			
			if( lpIOContext->nSentBytes <= 0 ) break;
		}			
		else 
		{
			if( lOfs > 0 )
			{
				if( lpIOContext->nSentBytes < 1 ) 
				{
					////g_CS.Lock();
					LogAdd("error-L1 : recvbuflen 1 %s %d", __FILE__, __LINE__);
					////g_CS.UnLock();
					return false;
				}
				else 
				{
					if( lpIOContext->nSentBytes < MAX_BUFF_SIZE )
					{
					#if (CUSTOM_ENCRYPT == 1)
						LogAddHeadHexFunc(1,(char*)recvbuf,lpIOContext->nSentBytes);
						if( recvbuf[lOfs] == 0xC1 || recvbuf[lOfs] == 0xC3 )
						{
							recvbuf[lOfs+1] ^= 0x10;
							recvbuf[lOfs+2] ^= 0x40;
						}
						else if( recvbuf[lOfs] == 0xC2 || recvbuf[lOfs] == 0xC4 ) 
						{
							recvbuf[lOfs+1] ^= 0x11;
							recvbuf[lOfs+2] ^= 0x41;
							recvbuf[lOfs+3] ^= 0x31;
						}
						recvbuf[lOfs] ^= 0x20;
					#endif

						memcpy(recvbuf, (recvbuf+lOfs), lpIOContext->nSentBytes);
						LogAdd("Message copy %d, packet size: %d", lpIOContext->nSentBytes,size);
					}
					break;
				}
			}
			break;
		}
	}
	return TRUE;
}

int DataSend(int aIndex, LPBYTE lpMsg, DWORD dwSize)
{
	unsigned char * SendBuf;

	if( aIndex < OBJ_STARTUSERINDEX || aIndex >= OBJMAX ) return FALSE;

#if (ENABLE_CUSTOM_OFFLINETRADE == 1)

	if( gObj[aIndex].bOffTrade )
		return FALSE;

#endif

	CCSLock cs(&g_SendLock);


	DWORD SendBytes;
	LPPER_SOCKET_CONTEXT lpPerSocketContext;	

	if( gObj[aIndex].Connected < 1)
	{
		return FALSE;
	}

	if ( lpMsg[0] == 0xC3 || lpMsg[0] == 0xC4 )
	{
		int ret;
		BYTE btsize;

		if ( lpMsg[0] == 0xC3 )
		{
			btsize = lpMsg[1];
			lpMsg[1]=gNSerialCheck[aIndex].GetSendSerial();

			ret = g_SimpleModulus.Encrypt(&ExSendBuf[2], &lpMsg[1], dwSize-1);
			ExSendBuf[0] = 0xC3;
			ExSendBuf[1] = ret + 2;
			SendBuf = ExSendBuf;

			dwSize = ret + 2;
			lpMsg[1] = btsize;
		}
		else
		{
			btsize = lpMsg[2];
			lpMsg[2] = gNSerialCheck[aIndex].GetSendSerial();

			ret = g_SimpleModulus.Encrypt(&ExSendBuf[3], &lpMsg[2], dwSize-2);
			ExSendBuf[0] = 0xC4;
			ExSendBuf[1] = SET_NUMBERH(ret+3);
			ExSendBuf[2] = SET_NUMBERL(ret+3);
			SendBuf = ExSendBuf;
			dwSize = ret + 3;
			lpMsg[2] = btsize;
		}
	}
	else
	{
		SendBuf = lpMsg;
	}

	lpPerSocketContext = gObj[aIndex].PerSocketContext;
	
	if( dwSize > MAX_BUFF_SIZE )
	{
		////g_CS.Lock();		
		LogAdd("Error : Max msg(%d) %s %d", dwSize, __FILE__, __LINE__);
		LogAddHeadHexFunc(1,(char*)lpPerSocketContext->IOContext[1].BufferSecond[0],MAX_BUFF_SIZE);
		////g_CS.UnLock();
		CloseClient( aIndex );
		return FALSE;
	}

	LPPER_IO_CONTEXT	lpIoCtxt = (LPPER_IO_CONTEXT)&lpPerSocketContext->IOContext[1];	

	if( lpIoCtxt->nWaitIO > 0 )
	{	
		if( (lpIoCtxt->nSecondOfs+dwSize) > MAX_BUFF_SIZE-1)
		{
			////g_CS.Lock();			
			//LogAddHeadHexFunc(1,(char*)&lpIoCtxt->BufferSecond[0],MAX_BUFF_SIZE);
			LogAdd("(%d)error-L2 MAX BUFFER OVER %d %d %d [%s][%s]", aIndex, lpIoCtxt->nTotalBytes, lpIoCtxt->nSecondOfs, dwSize, gObj[aIndex].AccountID, gObj[aIndex].Name);
			////g_CS.UnLock();
			lpIoCtxt->nWaitIO = 0;
			CloseClient( aIndex );			
			return TRUE;
		}
		else
		{
			memcpy(lpIoCtxt->BufferSecond+lpIoCtxt->nSecondOfs, SendBuf, dwSize);
			lpIoCtxt->nSecondOfs+=dwSize;
			return TRUE;
		}
	}
	else
	{
		lpIoCtxt->nTotalBytes = 0;

		if( lpIoCtxt->nSecondOfs > 0 )
		{
			memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
			lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
			lpIoCtxt->nSecondOfs = 0;
		}

		if( (lpIoCtxt->nTotalBytes+dwSize) > MAX_BUFF_SIZE-1)
		{
			////g_CS.Lock();			
			//LogAddHeadHexFunc(1,&lpIoCtxt->Buffer[0],MAX_BUFF_SIZE);
			LogAdd("(%d)error-L2 MAX BUFFER OVER %d %d [%s][%s]", aIndex, lpIoCtxt->nTotalBytes, dwSize, gObj[aIndex].AccountID, gObj[aIndex].Name);
			////g_CS.UnLock();
			lpIoCtxt->nWaitIO = 0;
			CloseClient( aIndex );			
			return FALSE;
		}
		else
		{
			memcpy(lpIoCtxt->Buffer+lpIoCtxt->nTotalBytes, SendBuf, dwSize);
			lpIoCtxt->nTotalBytes += dwSize;
		}
	}

	lpIoCtxt->wsabuf.buf	= (char*)lpIoCtxt->Buffer;
	lpIoCtxt->wsabuf.len	= lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes	= 0;
	lpIoCtxt->IOOperation	= SEND_IO;

	if (WSASend(gObj[aIndex].m_socket, &(lpIoCtxt->wsabuf), 1, &SendBytes, 0,
		&(lpIoCtxt->Overlapped), NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			////g_CS.Lock();			
			GetLastErrorMsg();
			
			if ( lpIoCtxt->wsabuf.buf[0] == 0xC1 )
			{
				LogAdd("(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, aIndex, (BYTE)lpIoCtxt->wsabuf.buf[0],
					(BYTE)lpIoCtxt->wsabuf.buf[2], WSAGetLastError(), gObj[aIndex].Ip_addr);
			}
			else if ( lpIoCtxt->wsabuf.buf[0] == 0xC2 )
			{
				LogAdd("(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, aIndex, (BYTE)lpIoCtxt->wsabuf.buf[0],
					(BYTE)lpIoCtxt->wsabuf.buf[3], WSAGetLastError(), gObj[aIndex].Ip_addr);
			}

			////g_CS.UnLock();
			CloseClient( aIndex );			
			return FALSE;
		}
	}
	else 
	{
		InterlockedIncrement(&lpPerSocketContext->dwIOCount);		
	}
	lpIoCtxt->nWaitIO = 1;	
	return TRUE;
}

int IoSendSecond(LPPER_SOCKET_CONTEXT lpPerSocketContext)
{		
	DWORD SendBytes;
	int aIndex = lpPerSocketContext->nIndex;

	if( gObj[aIndex].Connected < 1 )
	{
		return FALSE;
	}

	LPPER_IO_CONTEXT	lpIoCtxt = (LPPER_IO_CONTEXT)&lpPerSocketContext->IOContext[1];	

	if( lpIoCtxt->nWaitIO > 0 )	
	{
		return FALSE;
	}
	
	lpIoCtxt->nTotalBytes = 0;
	if( lpIoCtxt->nSecondOfs > 0 )
	{
		memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
		lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
		lpIoCtxt->nSecondOfs = 0;
	}
	else 
	{	
		return FALSE;
	}

	lpIoCtxt->wsabuf.buf	= (char*)lpIoCtxt->Buffer;
	lpIoCtxt->wsabuf.len	= lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes	= 0;
	lpIoCtxt->IOOperation	= SEND_IO;

	if (WSASend(gObj[aIndex].m_socket, &(lpIoCtxt->wsabuf), 1, &SendBytes, 0,
		&(lpIoCtxt->Overlapped), NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			////g_CS.Lock();
			GetLastErrorMsg();
			LogAdd("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), gObj[aIndex].Ip_addr);
			////g_CS.UnLock();
			CloseClient( aIndex );			
			return FALSE;
		}
	}
	else 
	{
		InterlockedIncrement(&lpPerSocketContext->dwIOCount);
	}
	lpIoCtxt->nWaitIO = 1;	
	return TRUE;
}

int IoMoreSend(LPPER_SOCKET_CONTEXT lpPerSocketContext)
{		
	DWORD SendBytes;

	int aIndex = lpPerSocketContext->nIndex;	

	LPPER_IO_CONTEXT	lpIoCtxt = (LPPER_IO_CONTEXT)&lpPerSocketContext->IOContext[1];
	
	if( (lpIoCtxt->nTotalBytes-lpIoCtxt->nSentBytes) < 0 ) 
	{
		return FALSE;
	}
	
	////g_CS.Lock();	
	printf("Error : File : [%s], Line : [%d]\n", __FILE__, __LINE__);
	printf("%d packet len\n", lpIoCtxt->nTotalBytes-lpIoCtxt->nSentBytes);
	////g_CS.UnLock();

	lpIoCtxt->wsabuf.buf	= (char*)lpIoCtxt->Buffer+lpIoCtxt->nSentBytes;
	lpIoCtxt->wsabuf.len	= lpIoCtxt->nTotalBytes-lpIoCtxt->nSentBytes;
	lpIoCtxt->IOOperation	= SEND_IO;

	if (WSASend(gObj[aIndex].m_socket, &(lpIoCtxt->wsabuf), 1, &SendBytes, 0,
		&(lpIoCtxt->Overlapped), NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			////g_CS.Lock();
			GetLastErrorMsg();			
			LogAdd("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), gObj[aIndex].Ip_addr);
			////g_CS.UnLock();
			CloseClient( aIndex );			
			return FALSE;
		}
	}
	else 
	{
		InterlockedIncrement(&lpPerSocketContext->dwIOCount);
	}
	lpIoCtxt->nWaitIO = 1;
	return TRUE;
}


BOOL UpdateCompletionPort( SOCKET sd, int ClientIndex, BOOL bAddToList ) 
{
    LPPER_SOCKET_CONTEXT lpPerSocketContext = NULL;
	
	HANDLE CompletionPort;

	CompletionPort = CreateIoCompletionPort((HANDLE)sd, g_CompletionPort, (DWORD)ClientIndex, 0); 
    
	if ( CompletionPort == NULL)
	{
		printf("Error CPHandle = %d, SOCKET [%d]\n", CompletionPort, g_Listen);
		////g_CS.Lock();

		GetLastErrorMsg();		

		LogAdd("CreateIoCompletionPort: %d", GetLastError() );

		////g_CS.UnLock();
        return FALSE;
	}
	g_CompletionPort = CompletionPort;
	InterlockedExchange(&gObj[ClientIndex].PerSocketContext->dwIOCount, 0);	
    return TRUE;	
}

void CloseClient ( LPPER_SOCKET_CONTEXT lpPerSocketContext, BOOL bGraceful ) 
{ 	
	int index = lpPerSocketContext->nIndex;
	if( index < OBJ_STARTUSERINDEX || index >= OBJMAX ) return;

	if( gObj[index].Connected == 0 ) return;

	CCSLock cs(&g_SendLock);

	if( INVALID_SOCKET != gObj[index].m_socket )
	{
		if( closesocket(gObj[index].m_socket) == SOCKET_ERROR )
		{
			if(WSAGetLastError() != 10038)
			{
				return;
			}
		}

		gObj[index].m_socket = INVALID_SOCKET;
	}

	gObjDel(index);	
}

void CloseClient ( int index )
{ 	
	if( index < OBJ_STARTUSERINDEX || index >= OBJMAX ) return;

	if( gObj[index].Connected == 0 ) return;

	CCSLock cs(&g_SendLock);

	if( INVALID_SOCKET != gObj[index].m_socket )
	{
		closesocket(gObj[index].m_socket);
		gObj[index].m_socket = INVALID_SOCKET;
	}
}
