// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------

// GS-N 0.99.60T Status : Completed
//	GS-N	1.00.18	JPN	0x00423E80	-	Completed

#include "stdafx.h"
#include "..\common\MyWinsockBase.h"
#include "wsJoinServerCli.h"
#include "GameServer.h"
#include "GameMain.h"
#include "DBSockMng.h"
#include "DSProtocol.h"
#include "logproc.h"



DBSockMng cDBSMng;	// DataServer Manager

DBSockMng::DBSockMng() 
{
	this->m_ActiveServer=DS_UNINITIALIZED;
}

DBSockMng::~DBSockMng()
{

}


void DBSockMng::Connect()
{
	
	if (this->wsCDBS[0].GetSocket()  == INVALID_SOCKET )
	{
		int State;
		this->wsCDBS[0].CreateSocket(ghWnd);
		this->wsCDBS[0].SetProtocolCore(DataServerProtocolCore);
		State=this->wsCDBS[0].Connect(DataServerIP, DataServerPort, WM_GM_DATA1_CLIENT_MSG_PROC);
		if (State == 0)
		{
			LogAdd(lMsg.Get(MSGGET(1, 147)), DataServerIP, DataServerPort);
			this->wsCDBS[0].Close();
		}
		else
		{
			LogAdd(lMsg.Get(MSGGET(1, 146)), DataServerIP, DataServerPort);	
			this->m_ActiveServer=DS_0;
			DataServerLogin(DS_0);
		}
	}
	
	if (this->wsCDBS[1].GetSocket() == INVALID_SOCKET)	
	{
		int State;
		this->wsCDBS[1].CreateSocket(ghWnd);
		this->wsCDBS[1].SetProtocolCore(DataServerProtocolCore);
		State=this->wsCDBS[1].Connect(DataServerIP2, DataServerPort2, WM_GM_DATA2_CLIENT_MSG_PROC);// Two ports more than the used1
		if (State==0)
		{
			LogAdd(lMsg.Get(MSGGET(1, 147)), DataServerIP2, DataServerPort2);
			this->wsCDBS[1].Close();
		}
		else
		{
			LogAdd(lMsg.Get(MSGGET(1, 146)), DataServerIP2, DataServerPort2);
			if (this->m_ActiveServer==-1)
			{
				this->m_ActiveServer=DS_1;
			}
			DataServerLogin(DS_1);	// Second DataServer
		}
	}
	
	if ( wsExDbCli.GetSocket()  == INVALID_SOCKET)
	{
		//LogAddC(2, "ExDB Server Check..");
		wsExDbCli.CreateSocket(ghWnd);
		ExDataServerConnect(ExDBServerIP, WM_GM_EXDATA_CLIENT_MSG_PROC);
	}
	else
	{
		if ( wsExDbCli.GetConnect() == 0 )
		{
			//LogAddC(2, "ExDB Server Check..");
			ExDataServerConnect(ExDBServerIP, WM_GM_EXDATA_CLIENT_MSG_PROC);
		}
	}
}

void DBSockMng::ActiveChange()
{
	{
		int Status;
		if (this->m_ActiveServer==DS_0)
		{
			this->m_ActiveServer=DS_1;
			Status=1;
		}
		else
		{
			this->m_ActiveServer=DS_0;
			Status=0;
		}
		LogAdd("Active DB Change");
	}
}

int DBSockMng::Send(char* buf, int len)
{
	if (this->m_ActiveServer >= 2)
	{
		return 0;
	}
	if (this->m_ActiveServer < 0)
	{
		return 0;
	}
	if ( this->wsCDBS[this->m_ActiveServer].DataSend((char*)buf, len) == 0)
	{
		this->ActiveChange();
		this->wsCDBS[this->m_ActiveServer].DataSend((char*)buf, len);
	}
	return 1;
}


int DBSockMng::Send(int server, char* buf, int len)
{
	if (server >= 2)
	{
		return 0;
	}
	if (server < 0)
	{
		return 0;
	}
	if (this->wsCDBS[server].DataSend(buf, len) == 0)
	{
		this->ActiveChange();
	}
	return 1;
}


int DBSockMng::MsgProc(int line, WPARAM wParam, LPARAM lParam)
{
	switch (lParam & 0xFFFF & 0xFFFF)
	{
	case 1: // Recv Data
		this->wsCDBS[line].DataRecv();
		break;
	case 2: // Send Data
		this->wsCDBS[line].FDWRITE_MsgDataSend();
		break;
	case 32: // Close
		this->wsCDBS[line].Close();
		break;
	}
	return 1;
}

#if (GS_CASTLE == 1)

int DBSockMng::GetActiveDS()
{
	return m_ActiveServer;
}

#endif


			





