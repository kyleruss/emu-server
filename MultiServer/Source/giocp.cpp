// Giocp.cpp
//------------------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
//------------------------------------------

// GS-N 0.99.60T 0x00473020 - Status : Completed :)
//	GS-N	1.00.18	JPN	0x00489FD0	-	Completed
#include "stdafx.h"
#include "WzQueue.h"
#include "Server.h"
#include "zMultiServer.h"
#include "CashShopServer.h"
#include "Window.h"

WZQueue	WzRecvQ;

HANDLE g_CompletionPort[MAX_SERVERTYPE];
DWORD g_dwThreadCount[MAX_SERVERTYPE];
CRITICAL_SECTION criti[MAX_SERVERTYPE];
CRITICAL_SECTION scriti;
HANDLE g_ThreadHandles[MAX_SERVERTYPE][MAX_IO_THREAD_HANDLES];
int gSObjPort[MAX_SERVERTYPE];
HANDLE g_IocpThreadHandle[MAX_SERVERTYPE];
SOCKET g_Listen[MAX_SERVERTYPE] = {INVALID_SOCKET,
									INVALID_SOCKET,
									INVALID_SOCKET,
									INVALID_SOCKET,};

BOOL IocpServerStart()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	InitializeCriticalSection(&scriti);

	return TRUE;
}



BOOL CreateGIocp(int server_port, DWORD S_TYPE)
{
	DWORD dwPar = S_TYPE;

	DWORD ThreadID;
	gSObjPort[S_TYPE] = server_port;
	g_IocpThreadHandle[S_TYPE] = CreateThread( NULL, 0,
											(LPTHREAD_START_ROUTINE)IocpServerWorker,
											(LPVOID)dwPar, 0, &ThreadID);

	if ( g_IocpThreadHandle[S_TYPE] == NULL )
	{
		g_Window.ServerLogAdd(S_TYPE,"[IOCP] CreateThread() failed with error %d - ServerType : %d", GetLastError(), S_TYPE);
		return FALSE;
	}

	return TRUE;
}


void DestroyGIocp()
{
	DeleteCriticalSection(&scriti);

	for(DWORD I=0;I<MAX_SERVERTYPE;I++)
	{
		closesocket(g_Listen[I]);

		for (DWORD dwCPU=0; dwCPU < g_dwThreadCount[I];dwCPU++ )
			TerminateThread(g_ThreadHandles[I][dwCPU] , 0);

		TerminateThread(g_IocpThreadHandle[I], 0);

		if ( g_CompletionPort[I] != NULL )
		{
			CloseHandle(g_CompletionPort[I]);
			g_CompletionPort[I]=NULL;
		} 
	}
}


BOOL CreateListenSocket(DWORD S_TYPE)
{
	sockaddr_in InternetAddr;
	int nRet;

	g_Listen[S_TYPE] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if ( g_Listen[S_TYPE] == INVALID_SOCKET )
	{
		g_Window.ServerLogAdd(S_TYPE,"WSASocket() failed with error %d", WSAGetLastError() );
		return FALSE;
	}

	InternetAddr.sin_family=AF_INET;
	InternetAddr.sin_addr.S_un.S_addr=htonl(0);
	InternetAddr.sin_port=htons(gSObjPort[S_TYPE]);
	nRet=bind(g_Listen[S_TYPE], (sockaddr*)&InternetAddr, 16);
	
	if ( nRet == -1 )
	{
//		MsgBox("bind error : eDataServer can't be launched twice");
		SendMessage(g_hWnd, WM_CLOSE, 0,0);	// Kill aplication
		return FALSE;
	}

	nRet=listen(g_Listen[S_TYPE], 5);
	if (nRet == -1)
	{
		g_Window.ServerLogAdd(S_TYPE,"listen() failed with error %d", WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}


unsigned long __stdcall IocpServerWorker(DWORD pIocpServerParameter)
{
	SYSTEM_INFO SystemInfo;
	DWORD ThreadID;
	SOCKET Accept;
	int nRet;
	int ClientIndex;
	sockaddr_in cAddr;
	in_addr cInAddr;
	int cAddrlen;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	int RecvBytes;
	unsigned long Flags;

	DWORD S_TYPE = pIocpServerParameter;
	
	cAddrlen=16;
	lpPerSocketContext=0;
	Flags=0;

	InitializeCriticalSection(&criti[S_TYPE]);
	GetSystemInfo(&SystemInfo);

	g_dwThreadCount[S_TYPE] = SystemInfo.dwNumberOfProcessors * 2;

	if( g_dwThreadCount[S_TYPE] > MAX_IO_THREAD_HANDLES )
	{
		g_dwThreadCount[S_TYPE] = MAX_IO_THREAD_HANDLES;
	}

	__try
	{

		g_CompletionPort[S_TYPE] = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

		if ( g_CompletionPort[S_TYPE] == NULL )
		{
			g_Window.ServerLogAdd(S_TYPE,"CreateIoCompletionPort failed with error: %d", GetLastError());
			__leave;
		}

		for ( DWORD n = 0; n<g_dwThreadCount[S_TYPE]; n++ )
		{
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerWorkerThread, (LPVOID)g_CompletionPort[S_TYPE], 0, &ThreadID);

			if ( hThread == NULL )
			{
				g_Window.ServerLogAdd(S_TYPE,"CreateThread() failed with error %d", GetLastError() );
				__leave;
			}

			g_ThreadHandles[S_TYPE][n] = hThread;
			CloseHandle(hThread);
		}

		if ( CreateListenSocket(S_TYPE) == FALSE )
		{

		}
		else
		{
			while ( true )
			{
				Accept = WSAAccept(g_Listen[S_TYPE], (sockaddr*)&cAddr, &cAddrlen, NULL, 0 );

				if ( Accept == -1 )
				{
					EnterCriticalSection(&criti[S_TYPE]);
					g_Window.ServerLogAdd(S_TYPE,"WSAAccept() failed with error %d", WSAGetLastError() );
					LeaveCriticalSection(&criti[S_TYPE]);
					continue;
				}

				EnterCriticalSection(&scriti);
				EnterCriticalSection(&criti[S_TYPE]);

				memcpy(&cInAddr, &cAddr.sin_addr  , sizeof(cInAddr) );

				ClientIndex = gSObjAddSearch();

				if ( ClientIndex == -1 )
				{
					g_Window.ServerLogAdd(S_TYPE,"error-L2 : ServerIndex = -1");
					closesocket(Accept);
					LeaveCriticalSection(&criti[S_TYPE]);
					LeaveCriticalSection(&scriti);
					continue;
				}

				if (UpdateCompletionPort(Accept, ClientIndex, S_TYPE) == 0 )
				{
					g_Window.ServerLogAdd(S_TYPE,"error-L1 : %d %d CreateIoCompletionPort failed with error %d", Accept, ClientIndex, GetLastError() );
					closesocket(Accept);
					LeaveCriticalSection(&criti[S_TYPE]);
					LeaveCriticalSection(&scriti);
					continue;
				}

				if (gSObjAdd(Accept, inet_ntoa(cInAddr), ClientIndex, S_TYPE ) == -1 )
				{
					g_Window.ServerLogAdd(S_TYPE,"error-L1 : %d %d gSObjAdd() failed with error %d", Accept, ClientIndex, GetLastError() );
					closesocket(Accept);
					LeaveCriticalSection(&criti[S_TYPE]);
					LeaveCriticalSection(&scriti);
					continue;
				}

				memset(&gSObj[ClientIndex].PerSocketContext.IOContext[0].Overlapped, 0, sizeof(WSAOVERLAPPED));
				memset(&gSObj[ClientIndex].PerSocketContext.IOContext[1].Overlapped, 0, sizeof(WSAOVERLAPPED));

				gSObj[ClientIndex].PerSocketContext.IOContext[0].wsabuf.buf = (char*)&gSObj[ClientIndex].PerSocketContext.IOContext[0].Buffer;
				gSObj[ClientIndex].PerSocketContext.IOContext[0].wsabuf.len = MAX_IO_BUFFER_SIZE;
				gSObj[ClientIndex].PerSocketContext.IOContext[0].nTotalBytes = 0;
				gSObj[ClientIndex].PerSocketContext.IOContext[0].nSentBytes = 0;
				gSObj[ClientIndex].PerSocketContext.IOContext[0].nWaitIO = 0;
				gSObj[ClientIndex].PerSocketContext.IOContext[0].nSecondOfs = 0;
				gSObj[ClientIndex].PerSocketContext.IOContext[0].IOOperation = 0;
				gSObj[ClientIndex].PerSocketContext.IOContext[1].wsabuf.buf = (char*)gSObj[ClientIndex].PerSocketContext.IOContext[0].Buffer;
				gSObj[ClientIndex].PerSocketContext.IOContext[1].wsabuf.len = MAX_IO_BUFFER_SIZE;
				gSObj[ClientIndex].PerSocketContext.IOContext[1].nTotalBytes = 0;
				gSObj[ClientIndex].PerSocketContext.IOContext[1].nSentBytes = 0;
				gSObj[ClientIndex].PerSocketContext.IOContext[1].nWaitIO = 0;
				gSObj[ClientIndex].PerSocketContext.IOContext[1].nSecondOfs = 0;
				gSObj[ClientIndex].PerSocketContext.IOContext[1].IOOperation = 1;
				gSObj[ClientIndex].PerSocketContext.m_socket = Accept;
				gSObj[ClientIndex].PerSocketContext.nIndex = ClientIndex;

				nRet = WSARecv(Accept, &gSObj[ClientIndex].PerSocketContext.IOContext[0].wsabuf , 1, (unsigned long*)&RecvBytes, &Flags, 
						&gSObj[ClientIndex].PerSocketContext.IOContext[0].Overlapped, NULL);

				if ( nRet == -1 )
				{
					if ( WSAGetLastError() != WSA_IO_PENDING )
					{
						g_Window.ServerLogAdd(S_TYPE,"error-L1 : WSARecv() failed with error %d", WSAGetLastError() );
						gSObj[ClientIndex].PerSocketContext.IOContext[0].nWaitIO = 4;
						CloseClient(&gSObj[ClientIndex].PerSocketContext, 0);
						LeaveCriticalSection(&criti[S_TYPE]);
						LeaveCriticalSection(&scriti);
						continue;
					}
				}

				gSObj[ClientIndex].PerSocketContext.IOContext[0].nWaitIO  = 1;
				gSObj[ClientIndex].PerSocketContext.dwIOCount++;

				LeaveCriticalSection(&criti[S_TYPE]);
				LeaveCriticalSection(&scriti);
			}


		}
	}
	__finally
	{
		
		if ( g_CompletionPort[S_TYPE] != NULL )
		{
			for ( DWORD i = 0 ; i < g_dwThreadCount[S_TYPE] ; i++ )
			{
				PostQueuedCompletionStatus( g_CompletionPort[S_TYPE] , 0, 0, 0);
			}
		}

		if ( g_CompletionPort[S_TYPE] != NULL )
		{
			CloseHandle(g_CompletionPort[S_TYPE]);
			g_CompletionPort[S_TYPE] = NULL;
		}
		if ( g_Listen[S_TYPE] != INVALID_SOCKET )
		{
			closesocket( g_Listen[S_TYPE]);
			g_Listen[S_TYPE] = INVALID_SOCKET;
		}

		DeleteCriticalSection(&criti[S_TYPE]);
	}
	return TRUE;
}


unsigned long __stdcall ServerWorkerThread(HANDLE CompletionPortID)
{
	HANDLE CompletionPort;
	DWORD dwIoSize;
	unsigned long RecvBytes;
	unsigned long Flags;
	DWORD dwSendNumBytes;
	BOOL bSuccess;
	int nRet;
	DWORD ClientIndex;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	LPOVERLAPPED lpOverlapped;
	_PER_IO_CONTEXT * lpIOContext;
	DWORD S_TYPE;
	
	CompletionPort=CompletionPortID;
	dwSendNumBytes=0;
	bSuccess=0;
	lpPerSocketContext=0;
	lpOverlapped=0;
	lpIOContext=0;
	
	while ( true )
	{
		bSuccess=GetQueuedCompletionStatus( CompletionPort, &dwIoSize, &ClientIndex, &lpOverlapped, -1); // WAIT_FOREVER

		S_TYPE = gSObj[ClientIndex].SType;

		if (bSuccess == FALSE)
		{
			if (lpOverlapped != NULL)
			{
				DWORD aError = GetLastError();
				if ( (aError != ERROR_NETNAME_DELETED) && 
					 (aError != ERROR_CONNECTION_ABORTED) && 
					 (aError != ERROR_OPERATION_ABORTED) && 
					 (aError != ERROR_SEM_TIMEOUT) )
				{
					EnterCriticalSection(&criti[S_TYPE]);
					//EnterCriticalSection(&scriti);
					g_Window.ServerLogAdd(S_TYPE,"Error Thread : GetQueueCompletionStatus( %d )", GetLastError());
					LeaveCriticalSection(&criti[S_TYPE]);
					//LeaveCriticalSection(&scriti);
					return 0;
				}
			}
		}

		EnterCriticalSection(&criti[S_TYPE]);
		//EnterCriticalSection(&scriti);

		lpPerSocketContext= &gSObj[ClientIndex].PerSocketContext;
		lpPerSocketContext->dwIOCount --;
				
		if ( dwIoSize == 0 )
		{
			g_Window.ServerLogAdd(S_TYPE,"Connection Closed, dwIoSize == 0 (Index:%d)", lpPerSocketContext->nIndex);
			CloseClient(lpPerSocketContext, 0);
			LeaveCriticalSection(&criti[S_TYPE]);
			//LeaveCriticalSection(&scriti);
			continue;
		}

		lpIOContext = (_PER_IO_CONTEXT *)lpOverlapped;

		if ( lpIOContext == 0 )
		{
			continue;
		}

		if ( lpIOContext->IOOperation == 1 )
		{
			lpIOContext->nSentBytes += dwIoSize;


			if ( lpIOContext->nSentBytes >= lpIOContext->nTotalBytes )
			{
				lpIOContext->nWaitIO = 0;
						
				if ( lpIOContext->nSecondOfs > 0)
				{
					IoSendSecond(lpPerSocketContext);
				}
			}
			else
			{
				IoMoreSend(lpPerSocketContext);
			}
			
		}
		else if ( lpIOContext->IOOperation == 0 )
		{
			RecvBytes = 0;
			lpIOContext->nSentBytes += dwIoSize;

			if ( RecvDataParse(lpIOContext, lpPerSocketContext->nIndex ) == 0 )
			{
				g_Window.ServerLogAdd(S_TYPE,"error-L1 : Socket Header error %d, %d", WSAGetLastError(), lpPerSocketContext->nIndex);
				CloseClient(lpPerSocketContext, 0);
				LeaveCriticalSection(&criti[S_TYPE]);
				//LeaveCriticalSection(&scriti);
				continue;
			}

			lpIOContext->nWaitIO = 0;
			Flags = 0;
			memset(&lpIOContext->Overlapped, 0, sizeof (WSAOVERLAPPED));
			lpIOContext->wsabuf.len = MAX_IO_BUFFER_SIZE - lpIOContext->nSentBytes;
			lpIOContext->wsabuf.buf = (char*)&lpIOContext->Buffer[lpIOContext->nSentBytes];
			lpIOContext->IOOperation = 0;

			nRet = WSARecv(lpPerSocketContext->m_socket, &lpIOContext->wsabuf, 1, &RecvBytes, &Flags,
						&lpIOContext->Overlapped, NULL);

			if ( nRet == -1 )
			{
				if ( WSAGetLastError() != WSA_IO_PENDING)
				{
					g_Window.ServerLogAdd(S_TYPE,"WSARecv() failed with error %d", WSAGetLastError() );
					CloseClient(lpPerSocketContext, 0);
					LeaveCriticalSection(&criti[S_TYPE]);
					//LeaveCriticalSection(&scriti);
					continue;
				}
			}

			lpPerSocketContext->dwIOCount ++;
			lpIOContext->nWaitIO = 1;
		}
		LeaveCriticalSection(&criti[S_TYPE]);
		//LeaveCriticalSection(&scriti);
		

	}


	return 1;
}



BOOL RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex)	
{
	unsigned char* recvbuf;
	int lOfs;
	int size;
	int headcode;
	BYTE xcode;

	// Check If Recv Data has More thatn 3 BYTES
	if ( lpIOContext->nSentBytes < 3 )
	{
		return TRUE;
	}

	// Initialize Variables
	lOfs=0;
	size=0;
	xcode=0;
	recvbuf = lpIOContext->Buffer;

	DWORD S_TYPE = gSObj[uIndex].SType;

	// Start Loop
	while ( true )
	{
		// Select packets with
		// C1 or C2 as HEader
		if ( recvbuf[lOfs] == 0xC1 ||
			 recvbuf[lOfs] == 0xC3 )
		{
			size = recvbuf[lOfs+1];
			headcode = recvbuf[lOfs+2];
			xcode = recvbuf[lOfs];
		}
		else if ( recvbuf[lOfs] == 0xC2 ||
			      recvbuf[lOfs] == 0xC4 )
		{
			size = recvbuf[lOfs+1] * 256;
			size |= recvbuf[lOfs+2];
			headcode = recvbuf[lOfs+3];
			xcode = recvbuf[lOfs];
		}
		// If HEader Differs - Second Generation Protocols
		else
		{
			protocol::MSG_HEADER * pHeader = (protocol::MSG_HEADER *)&recvbuf[lOfs];
			size = pHeader->size;
			headcode = pHeader->id;
		}

		// Check Size is leess thant 0
		if ( size <= 0 )
		{
			g_Window.ServerLogAdd(S_TYPE,"error-L1 : size %d",size);
			return false;
		}

		// Check if Size is On Range
		if ( size <= lpIOContext->nSentBytes )
		{
			gSObj[uIndex].ProtocolCore(uIndex, headcode, &recvbuf[lOfs], size);

			lOfs += size;
			lpIOContext->nSentBytes  -= size;

			if ( lpIOContext->nSentBytes <= 0 )
			{
				break;
			}
		}
		else if ( lOfs > 0 )
		{
			if ( lpIOContext->nSentBytes < 1 )
			{
				g_Window.ServerLogAdd(S_TYPE,"error-L1 : recvbuflen 1 %s %d", __FILE__, __LINE__);
				break;
			}

			if ( lpIOContext->nSentBytes < MAX_IO_BUFFER_SIZE ) 
			{
				memcpy(recvbuf, &recvbuf[lOfs], lpIOContext->nSentBytes);
				g_Window.ServerLogAdd(S_TYPE,"Message copy %d", lpIOContext->nSentBytes);
			}
			break;
		
		}
		else
		{
			break;
		}
		
	}

	return true;
}











BOOL DataSend(int aIndex, unsigned char* lpMsg, DWORD dwSize)
{
	unsigned long SendBytes;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	unsigned char * SendBuf;
	DWORD S_TYPE = gSObj[aIndex].SType;

	EnterCriticalSection(&criti[S_TYPE]);

	if ( aIndex < 0 || aIndex > (MAX_SERVEROBJECT-1) )
	{
		g_Window.ServerLogAdd(S_TYPE,"error-L2 : Index(%d) %x %x %x ", dwSize, lpMsg[0], lpMsg[1], lpMsg[2]);
		LeaveCriticalSection(&criti[S_TYPE]);
		return false;
	}

	SendBuf = lpMsg;

	if ( gSObj[aIndex].Connected < 1 )
	{
		LeaveCriticalSection(&criti[S_TYPE]);
		return FALSE;
	}

	lpPerSocketContext= &gSObj[aIndex].PerSocketContext;

	if ( dwSize > sizeof(lpPerSocketContext->IOContext[0].Buffer))
	{
		g_Window.ServerLogAdd(S_TYPE,"Error : Max msg(%d) %s %d", dwSize, __FILE__, __LINE__);
		CloseClient(aIndex);
		LeaveCriticalSection(&criti[S_TYPE]);
		return false;
	}

	_PER_IO_CONTEXT  * lpIoCtxt;

	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if ( lpIoCtxt->nWaitIO > 0 )
	{
		if ( ( lpIoCtxt->nSecondOfs + dwSize ) > MAX_IO_BUFFER_SIZE-1 )
		{
			g_Window.ServerLogAdd(S_TYPE,"(%d)error-L2 MAX BUFFER OVER %d %d %d",
				aIndex, lpIoCtxt->nTotalBytes, lpIoCtxt->nSecondOfs, dwSize);

			lpIoCtxt->nWaitIO = 0;
			CloseClient(aIndex);
			LeaveCriticalSection(&criti[S_TYPE]);
			return true;
		}

		memcpy( &lpIoCtxt->BufferSecond[lpIoCtxt->nSecondOfs], SendBuf, dwSize);
		lpIoCtxt->nSecondOfs += dwSize;
		LeaveCriticalSection(&criti[S_TYPE]);
		return true;
	}

	lpIoCtxt->nTotalBytes = 0;
	
	if ( lpIoCtxt->nSecondOfs > 0 )
	{
		memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
		lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
		lpIoCtxt->nSecondOfs = 0;
	}

	if ( (lpIoCtxt->nTotalBytes+dwSize) > MAX_IO_BUFFER_SIZE-1 )
	{
		g_Window.ServerLogAdd(S_TYPE,"(%d)error-L2 MAX BUFFER OVER %d %d %d",
			aIndex, lpIoCtxt->nTotalBytes, dwSize);

		lpIoCtxt->nWaitIO = 0;
		CloseClient(aIndex);
		LeaveCriticalSection(&criti[S_TYPE]);
		return FALSE;
	}

	memcpy( &lpIoCtxt->Buffer[lpIoCtxt->nTotalBytes], SendBuf, dwSize);
	lpIoCtxt->nTotalBytes += dwSize;
	lpIoCtxt->wsabuf.buf = (char*)&lpIoCtxt->Buffer;
	lpIoCtxt->wsabuf.len = lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes = 0;
	lpIoCtxt->IOOperation = 1;
	

	if ( WSASend( gSObj[aIndex].m_socket, &lpIoCtxt->wsabuf , 1, &SendBytes, 0, &lpIoCtxt->Overlapped, NULL) == -1 )
	{

		if ( WSAGetLastError() != WSA_IO_PENDING )	
		{
			lpIoCtxt->nWaitIO = 0;
			

			if ( lpIoCtxt->wsabuf.buf[0] == 0xC1 )
			{
				g_Window.ServerLogAdd(S_TYPE,"(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, aIndex, (BYTE)lpIoCtxt->wsabuf.buf[0],
					(BYTE)lpIoCtxt->wsabuf.buf[2], WSAGetLastError(), gSObj[aIndex].Ip);
			}
			else if ( lpIoCtxt->wsabuf.buf[0] == 0xC2 )
			{
				g_Window.ServerLogAdd(S_TYPE,"(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, aIndex, (BYTE)lpIoCtxt->wsabuf.buf[0],
					(BYTE)lpIoCtxt->wsabuf.buf[3], WSAGetLastError(), gSObj[aIndex].Ip);
			}
			else
			{
				g_Window.ServerLogAdd(S_TYPE,"(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, aIndex, (BYTE)lpIoCtxt->wsabuf.buf[0],
					(BYTE)lpIoCtxt->wsabuf.buf[3], WSAGetLastError(), gSObj[aIndex].Ip);
			}
			CloseClient(aIndex);
			LeaveCriticalSection(&criti[S_TYPE]);
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount ++;
	}
	
	
	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti[S_TYPE]);
	return true;
}





BOOL IoSendSecond(_PER_SOCKET_CONTEXT * lpPerSocketContext)
{
	unsigned long SendBytes;
	int aIndex;
	_PER_IO_CONTEXT * lpIoCtxt;

	aIndex = lpPerSocketContext->nIndex;

	DWORD S_TYPE = gSObj[aIndex].SType;

	EnterCriticalSection(&criti[S_TYPE]);


	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if ( lpIoCtxt->nWaitIO > 0 )
	{
		LeaveCriticalSection(&criti[S_TYPE]);
		return false;
	}

	lpIoCtxt->nTotalBytes = 0;
	if ( lpIoCtxt->nSecondOfs > 0 )
	{
		memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
		lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
		lpIoCtxt->nSecondOfs = 0;
	}
	else
	{
		LeaveCriticalSection(&criti[S_TYPE]);
		return false;
	}

	lpIoCtxt->wsabuf.buf = (char*)&lpIoCtxt->Buffer;
	lpIoCtxt->wsabuf.len = lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes = 0;
	lpIoCtxt->IOOperation = 1;

	if ( WSASend(gSObj[aIndex].m_socket, &lpIoCtxt->wsabuf, 1, &SendBytes, 0, &lpIoCtxt->Overlapped, NULL) == -1 )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
		{
			g_Window.ServerLogAdd(S_TYPE,"WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), gSObj[aIndex].Ip);
			CloseClient(aIndex);
			LeaveCriticalSection(&criti[S_TYPE]);
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount ++;
	}
	
	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti[S_TYPE]);
	
	return true;
}


BOOL IoMoreSend(_PER_SOCKET_CONTEXT * lpPerSocketContext)
{
	unsigned long SendBytes;
	int aIndex;
	_PER_IO_CONTEXT * lpIoCtxt;

	aIndex = lpPerSocketContext->nIndex;

	DWORD S_TYPE = gSObj[aIndex].SType;

	EnterCriticalSection(&criti[S_TYPE]);
	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if ( (lpIoCtxt->nTotalBytes - lpIoCtxt->nSentBytes) < 0 )
	{
		LeaveCriticalSection(&criti[S_TYPE]);
		return false;
	}

	lpIoCtxt->wsabuf.buf = (char*)&lpIoCtxt->Buffer[lpIoCtxt->nSentBytes];
	lpIoCtxt->wsabuf.len = lpIoCtxt->nTotalBytes - lpIoCtxt->nSentBytes;
	lpIoCtxt->IOOperation = 1;

	if ( WSASend(gSObj[aIndex].m_socket, &lpIoCtxt->wsabuf, 1, &SendBytes, 0, &lpIoCtxt->Overlapped, NULL) == -1 )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
		{
			g_Window.ServerLogAdd(S_TYPE,"WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), gSObj[aIndex].Ip);
			CloseClient(aIndex);
			LeaveCriticalSection(&criti[S_TYPE]);
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount ++;
	}
	
	
	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti[S_TYPE]);
	return true;
}


BOOL UpdateCompletionPort(SOCKET sd, int ClientIndex, DWORD S_TYPE)
{
	_PER_SOCKET_CONTEXT * lpPerSocketContext = NULL;
	HANDLE cp = CreateIoCompletionPort((HANDLE) sd, g_CompletionPort[S_TYPE], ClientIndex, 0);

	if ( cp == NULL )
	{
		g_Window.ServerLogAdd(S_TYPE,"CreateIoCompletionPort: %d", GetLastError() );
		return FALSE;
	}

	gSObj[ClientIndex].PerSocketContext.dwIOCount = 0;
	return TRUE;
}


void CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result)
{
	int index = -1;
	index = lpPerSocketContext->nIndex ;

	if ( index >= 0 && index <= MAX_SERVEROBJECT )
	{
		if ( gSObj[index].m_socket != INVALID_SOCKET )
		{
			if (closesocket(gSObj[index].m_socket) == -1 )
			{
				if ( WSAGetLastError() != WSAENOTSOCK )
				{
					return;
				}
			}

			gSObj[index].m_socket = INVALID_SOCKET;
		}

		gSObjDel(index);
	}
}



void CloseClient(int index)
{
	if ( index < 0 || index >= MAX_SERVEROBJECT )
	{
//		MsgBox("error-L1 : CloseClient index error");
		return;
	}

	DWORD S_TYPE = gSObj[index].SType;

	if ( gSObj[index].Connected == 0 )
	{
		g_Window.ServerLogAdd(S_TYPE,"error-L1 : CloseClient connect error");
		return;
	}

	EnterCriticalSection(&criti[S_TYPE]);

	if ( gSObj[index].m_socket != INVALID_SOCKET )
	{
		closesocket(gSObj[index].m_socket );
		gSObj[index].m_socket = INVALID_SOCKET;
	}
	else
	{
		g_Window.ServerLogAdd(S_TYPE,"error-L1 : CloseClient INVALID_SOCKET");
	}

	LeaveCriticalSection(&criti[S_TYPE]);
}

void ResponErrorCloseClient(int index)
{
	if ( index < 0 || index >= MAX_SERVEROBJECT )
	{
//		MsgBox("error-L1 : CloseClient index error");
		return;
	}

	DWORD S_TYPE = gSObj[index].SType;

	if ( gSObj[index].Connected == 0 )
	{
		g_Window.ServerLogAdd(S_TYPE,"error-L1 : CloseClient connect error");
		return;
	}

	EnterCriticalSection(&criti[S_TYPE]);
	closesocket(gSObj[index].m_socket);
	gSObj[index].m_socket = INVALID_SOCKET;

	if ( gSObj[index].m_socket == INVALID_SOCKET )
	{
		g_Window.ServerLogAdd(S_TYPE,"error-L1 : CloseClient INVALID_SOCKET");
	}

	gSObjDel(index);
	LeaveCriticalSection(&criti[S_TYPE]);
}


