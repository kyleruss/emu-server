#include "stdafx.h"
#include "Notice.h"
#include "GameMain.h"
#include "..\pugixml\pugixml.hpp"
#include "..\common\winutil.h"
#include "TNotice.h"
// -------------------------------------------------------------------------------

using namespace pugi;
Notice g_Notice;
// -------------------------------------------------------------------------------

Notice::Notice()
{

}
// -------------------------------------------------------------------------------

Notice::~Notice()
{

}
// -------------------------------------------------------------------------------

void Notice::Init()
{
	this->m_ConnectList.clear();
	if( this->m_ConnectList.capacity() > 0 )
	{
		std::vector<NoticeConnectInfo>().swap(this->m_ConnectList);
	}
	this->m_RotateList.clear();
	if( this->m_RotateList.capacity() > 0 )
	{
		std::vector<NoticeRotateInfo>().swap(this->m_RotateList);
	}
}
// -------------------------------------------------------------------------------

void Notice::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CUSTOM_NOTICE));
}
// -------------------------------------------------------------------------------

void Notice::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[Notice] File %s not found!", File);
		return;
	}
	// ----
	xml_node Notice		=	Document.child("notice");
	xml_node Connect	=	Notice.child("connect");
	xml_node Rotate		=	Notice.child("rotate");
	xml_node Online		=	Notice.child("online");
	xml_node Node;

	// ----
	for( Node = Connect.child("message"); Node; Node = Node.next_sibling() )
	{
		NoticeConnectInfo lpData;
		lpData.Type = Node.attribute("type").as_int();
		strcpy(lpData.Text, Node.text().as_string());
		this->m_ConnectList.push_back(lpData);
	}

	for(Node = Online.child("message"); Node; Node = Node.next_sibling())
	{
		NoticeOnlineInfo lpData;
		lpData.profile	=	Node.attribute("profile").as_int();
		lpData.Type		=	Node.attribute("type").as_int();
		strcpy(lpData.Text, Node.text().as_string());
		this->m_OnlineList.push_back(lpData);
	}

	// ----
	for( Node = Rotate.child("message"); Node; Node = Node.next_sibling() )
	{
		NoticeRotateInfo lpData;
		lpData.Type = Node.attribute("type").as_int();
		lpData.Delay = Node.attribute("delay").as_int();
		lpData.Tick = GetTickCount() + lpData.Delay * 60000;
		strcpy(lpData.Text, Node.text().as_string());
		this->m_RotateList.push_back(lpData);
	}
}
// -------------------------------------------------------------------------------

void Notice::Create(LPBYTE Buffer, BYTE Type, LPSTR Text)
{
	if( Buffer == NULL || Text == NULL )
	{
		return;
	}
	// ----
	PMSG_CHATDATA_WHISPER* lpChat = (PMSG_CHATDATA_WHISPER*)Buffer;
	PHeadSetB((LPBYTE)lpChat, 0, sizeof(PMSG_CHATDATA_WHISPER));
	strcpy(lpChat->id, "[Notice]");
	// ----
	switch(Type)
	{
	case 0:
		TNotice::MakeNoticeMsg(Buffer, 0, Text);
		break;
	case 1:
		TNotice::MakeNoticeMsg(Buffer, 1, Text);
		break;
	case 2:
		lpChat->h.headcode = 2;
		break;
	case 3:
		lpChat->chatmsg[0] = '~';
		break;
	case 4:
		lpChat->chatmsg[0] = '@';
		break;
	case 5:
		lpChat->chatmsg[0] = '@';
		lpChat->chatmsg[1] = '@';
		break;
	case 6:
		lpChat->chatmsg[0] = '$';
		break;
	default:
		TNotice::MakeNoticeMsg(Buffer, 0, Text);
		break;
	}
	// ----
	if( Type >= 2 && Type <= 6 )
	{
		strcat(lpChat->chatmsg, Text);
	}
}
// -------------------------------------------------------------------------------

void Notice::SendOnConnect(int UserIndex)
{
	if( this->m_ConnectList.empty() || !OBJMAX_RANGE(UserIndex) )
	{
		return;
	}
	// ----
	for( int i = 0; i < this->m_ConnectList.size(); i++ )
	{
		BYTE Temp[256] = { 0 };
		this->Create(Temp, this->m_ConnectList[i].Type, this->m_ConnectList[i].Text);
		DataSend(UserIndex, Temp, ((PBMSG_HEAD*)(Temp))->size);
	}
}

void Notice::NotifyOnline(LPOBJ user)
{
	if(this->m_OnlineList.empty()) return;
	else
	{
		for(std::vector<NoticeOnlineInfo>::iterator iter = m_OnlineList.begin(); iter != m_OnlineList.end(); iter++)
		{
			NoticeOnlineInfo info	=	*iter;
			char * message			=	new char[60];
			sprintf(message, info.Text, user->Name);

			if(info.Type < 2)
			{
				if((isGM(user) && info.profile == 1))
					TNotice::AllSendServerMsg(message);
				else if(!isGM(user) && info.profile == 0)
					TNotice::AllSendServerMsg(message);
			}

			delete[] message;
		}
	}
}
// -------------------------------------------------------------------------------

void Notice::SendOnPoint()
{
	if( this->m_RotateList.empty() )
	{
		return;
	}
	// ----
	for( int i = 0; i < this->m_RotateList.size(); i++ )
	{
		if( m_RotateList[i].Tick < GetTickCount() )
		{
			BYTE Temp[256] = { 0 };
			this->Create(Temp, this->m_RotateList[i].Type, this->m_RotateList[i].Text);
			// ----
			for( int UserIndex = OBJ_STARTUSERINDEX; UserIndex < OBJMAX; UserIndex++ )
			{
				if( gObj[UserIndex].Connected == PLAYER_PLAYING )
				{
					DataSend(UserIndex, Temp, ((PBMSG_HEAD*)(Temp))->size);
				}
			}
			// ----
			this->m_RotateList[i].Tick = GetTickCount() + this->m_RotateList[i].Delay * 60000;
		}
	}
}
// -------------------------------------------------------------------------------