#include "stdafx.h"
#include "Message.h"
#include "logproc.h"
#include "GameMain.h"
#include "..\pugixml\pugixml.hpp"

using namespace pugi;

Message lMsg;

Message::Message()
{

}

Message::~Message()
{

}

void Message::Init()
{
	this->m_MessageList.clear();
}

void Message::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_MESSAGELIST));
}

void Message::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[Message] Loading fail [%s]", File);
		return;
	}
	// ----
	xml_node Translation = Document.child("translation");
	// ----
	for( xml_node Category = Translation.child("category"); Category; Category = Category.next_sibling() )
	{
		for( xml_node Message = Category.child("message"); Message; Message = Message.next_sibling() )
		{
			MessageBody lpMessage;
			lpMessage.Category = Category.attribute("id").as_int();
			lpMessage.Index = Message.attribute("index").as_int();
			strcpy(lpMessage.MessageText, Message.text().as_string());
			this->m_MessageList.push_back(lpMessage);
		}
	}
}

LPSTR Message::Get(int MessageCode)
{
	char DefaultString[50];
	ZeroMemory(DefaultString, sizeof(DefaultString));
	// ----
	for( int i = 0; i < this->m_MessageList.size(); i++ )
	{
		if( MSGGET(this->m_MessageList[i].Category, this->m_MessageList[i].Index) == MessageCode )
		{
			return &this->m_MessageList[i].MessageText[0];
		}
	}
	// ----
	sprintf(DefaultString, "MsgError: %d (%d-%d)", MessageCode, MessageCode / 256, MessageCode % 256);
	return &DefaultString[0];
}