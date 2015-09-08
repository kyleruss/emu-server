#include "stdafx.h"
#include "GuildClass.h"
#include "giocp.h"
#include "winutil.h"
// -------------------------------------------------------------------------------

GuildClass	g_GuildClass;
CQuery		g_GuildMatchDB;
// -------------------------------------------------------------------------------

GuildClass::GuildClass()
{

}
// -------------------------------------------------------------------------------

GuildClass::~GuildClass()
{

}
// -------------------------------------------------------------------------------

void GuildClass::GDReqOfferList(int aIndex, GUILDMATCH_GDREQ_OFFERLIST* lpData)
{
	const int MaxSize = sizeof(GUILDMATCH_DGANS_OFFERLIST_COUNT) + sizeof(GUILDMATCH_DGANS_OFFERLIST_INFO) * 9;
	BYTE Buffer[MaxSize + 1] = { 0 };
	int Offset = sizeof(GUILDMATCH_DGANS_OFFERLIST_COUNT);
	// ----
	GUILDMATCH_DGANS_OFFERLIST_COUNT* lpAnswerCount = (GUILDMATCH_DGANS_OFFERLIST_COUNT*)Buffer;
	GUILDMATCH_DGANS_OFFERLIST_INFO* lpAnswerInfo = (GUILDMATCH_DGANS_OFFERLIST_INFO*)&Buffer[Offset];
	// ----
	lpAnswerCount->h.c = PMHC_WORD;
	lpAnswerCount->h.headcode = 0xED;
	lpAnswerCount->h.subcode = 0x00;
	lpAnswerCount->h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount->h.sizeL = SET_NUMBERL(Offset);
	lpAnswerCount->UserIndex = lpData->UserIndex;
	lpAnswerCount->CurrentPage = lpData->CurrentPage;
	// ----
	char Query[256];
	long Len = 0;
	// ----
	sprintf(Query, "EXEC GuildMatching_GetOfferList ?, %d, %d, '%s'", lpData->CurrentPage, 0, "");
	g_GuildMatchDB.BindParameterIntOutput(1, &lpAnswerCount->MaxPage, &Len);
	// ----
	if( !g_GuildMatchDB.Exec(Query) )
	{
		g_GuildMatchDB.Clear();
		DataSend(aIndex, (LPBYTE)lpAnswerCount, Offset);
		return;
	}
	// ----
	int Result, Count = 0;
	// ----
	while( (Result = g_GuildMatchDB.Fetch()) != SQL_NO_DATA && Result != SQL_ERROR )
	{
		g_GuildMatchDB.GetStr("Title", lpAnswerInfo[Count].Title);
		g_GuildMatchDB.GetStr("Master", lpAnswerInfo[Count].GuildMaster);
		g_GuildMatchDB.GetStr("GuildName", lpAnswerInfo[Count].GuildName);
		lpAnswerInfo[Count].GuildMemberCount = g_GuildMatchDB.GetInt("GuildMemberCount");
		lpAnswerInfo[Count].GuildMasterClass = g_GuildMatchDB.GetInt("GuildMasterClass");
		lpAnswerInfo[Count].GuildInterests = g_GuildMatchDB.GetInt("Interests");
		lpAnswerInfo[Count].GuildReqLevel = g_GuildMatchDB.GetInt("ReqLevel");
		lpAnswerInfo[Count].GuildReqClass = g_GuildMatchDB.GetInt("ReqClass");
		lpAnswerInfo[Count].GuildMasterLevel = g_GuildMatchDB.GetInt("GuildMasterLevel");
		lpAnswerInfo[Count].GuildGensClass = g_GuildMatchDB.GetInt("GuildGensClass");
		lpAnswerInfo[Count].OfferNumber = g_GuildMatchDB.GetInt("Number");
		Offset += sizeof(GUILDMATCH_DGANS_OFFERLIST_INFO);
		Count++;
	}
	// ----
	while( g_GuildMatchDB.MoreResults() != SQL_NO_DATA )
	{
		
	}
	// ----
	g_GuildMatchDB.Clear();
	// ----
	lpAnswerCount->h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount->h.sizeL = SET_NUMBERL(Offset);
	lpAnswerCount->OfferCount = Count;
	// ----
	DataSend(aIndex, (LPBYTE)Buffer, Offset);
}
// -------------------------------------------------------------------------------

void GuildClass::GDReqOfferListByFilter(int aIndex, GUILDMATCH_GDREQ_OFFERLISTFILTER* lpData)
{
	const int MaxSize = sizeof(GUILDMATCH_DGANS_OFFERLIST_COUNT) + sizeof(GUILDMATCH_DGANS_OFFERLIST_INFO) * 9;
	BYTE Buffer[MaxSize + 1] = { 0 };
	int Offset = sizeof(GUILDMATCH_DGANS_OFFERLIST_COUNT);
	// ----
	GUILDMATCH_DGANS_OFFERLIST_COUNT* lpAnswerCount = (GUILDMATCH_DGANS_OFFERLIST_COUNT*)Buffer;
	GUILDMATCH_DGANS_OFFERLIST_INFO* lpAnswerInfo = (GUILDMATCH_DGANS_OFFERLIST_INFO*)&Buffer[Offset];
	// ----
	lpAnswerCount->h.c = PMHC_WORD;
	lpAnswerCount->h.headcode = 0xED;
	lpAnswerCount->h.subcode = 0x00;
	lpAnswerCount->h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount->h.sizeL = SET_NUMBERL(Offset);
	lpAnswerCount->UserIndex = lpData->UserIndex;
	lpAnswerCount->CurrentPage = lpData->CurrentPage;
	// ----
	char Query[256];
	long Len = 0;
	// ----
	sprintf(Query, "EXEC GuildMatching_GetOfferList ?, %d, %d, '%s'", lpData->CurrentPage, 1, lpData->KeyWord);
	g_GuildMatchDB.BindParameterIntOutput(1, &lpAnswerCount->MaxPage, &Len);
	// ----
	if( !g_GuildMatchDB.Exec(Query) )
	{
		g_GuildMatchDB.Clear();
		DataSend(aIndex, (LPBYTE)lpAnswerCount, Offset);
		return;
	}
	// ----
	int Result, Count = 0;
	// ----
	while( (Result = g_GuildMatchDB.Fetch()) != SQL_NO_DATA && Result != SQL_ERROR )
	{
		g_GuildMatchDB.GetStr("Title", lpAnswerInfo[Count].Title);
		g_GuildMatchDB.GetStr("Master", lpAnswerInfo[Count].GuildMaster);
		g_GuildMatchDB.GetStr("GuildName", lpAnswerInfo[Count].GuildName);
		lpAnswerInfo[Count].GuildMemberCount = g_GuildMatchDB.GetInt("GuildMemberCount");
		lpAnswerInfo[Count].GuildMasterClass = g_GuildMatchDB.GetInt("GuildMasterClass");
		lpAnswerInfo[Count].GuildInterests = g_GuildMatchDB.GetInt("Interests");
		lpAnswerInfo[Count].GuildReqLevel = g_GuildMatchDB.GetInt("ReqLevel");
		lpAnswerInfo[Count].GuildReqClass = g_GuildMatchDB.GetInt("ReqClass");
		lpAnswerInfo[Count].GuildMasterLevel = g_GuildMatchDB.GetInt("GuildMasterLevel");
		lpAnswerInfo[Count].GuildGensClass = g_GuildMatchDB.GetInt("GuildGensClass");
		lpAnswerInfo[Count].OfferNumber = g_GuildMatchDB.GetInt("Number");
		Offset += sizeof(GUILDMATCH_DGANS_OFFERLIST_INFO);
		Count++;
	}
	// ----
	while( g_GuildMatchDB.MoreResults() != SQL_NO_DATA )
	{
		
	}
	// ----
	g_GuildMatchDB.Clear();
	// ----
	lpAnswerCount->h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount->h.sizeL = SET_NUMBERL(Offset);
	lpAnswerCount->OfferCount = Count;
	// ----
	DataSend(aIndex, (LPBYTE)Buffer, Offset);
}
// -------------------------------------------------------------------------------

void GuildClass::GDReqOfferCreate(int aIndex, GUILDMATCH_GDREQ_OFFERCREATE* lpData)
{
	char Query[256];
	int Result = -1;
	// ----
	GUILDMATCH_DGANS_OFFERCREATE lpAnswer = { 0 };
	// ----
	lpAnswer.h.c = PMHC_BYTE;
	lpAnswer.h.headcode = 0xED;
	lpAnswer.h.subcode = 0x02;
	lpAnswer.h.size = sizeof(GUILDMATCH_DGANS_OFFERCREATE);
	lpAnswer.UserIndex = lpData->UserIndex;
	lpAnswer.Result = Result;
	// ----
	sprintf(Query, "EXEC GuildMatching_AddOfferList '%s', '%s', %d, %d, %d",
		lpData->Title, lpData->GuildMaster, lpData->GuildInterests, 
		lpData->GuildReqLevel, lpData->GuildReqClass);
	// ----
	if( !g_GuildMatchDB.Exec(Query) || g_GuildMatchDB.Fetch() == SQL_NO_DATA )
	{
		g_GuildMatchDB.Clear();
		DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_OFFERCREATE));
		return;
	}
	// ----
	lpAnswer.Result = g_GuildMatchDB.GetInt("Result");
	g_GuildMatchDB.Clear();
	DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_OFFERCREATE));
}
// -------------------------------------------------------------------------------

void GuildClass::GDReqOfferDelete(int aIndex, GUILDMATCH_GDREQ_OFFERDELETE* lpData)
{
	char Query[256];
	int Result = -1;
	// ----
	GUILDMATCH_DGANS_OFFERDELETE lpAnswer = { 0 };
	// ----
	lpAnswer.h.c = PMHC_BYTE;
	lpAnswer.h.headcode = 0xED;
	lpAnswer.h.subcode = 0x03;
	lpAnswer.h.size = sizeof(GUILDMATCH_DGANS_OFFERDELETE);
	lpAnswer.UserIndex = lpData->UserIndex;
	lpAnswer.Result = 0;
	// ----
	sprintf(Query, "EXEC GuildMatching_DelOfferList '%s'", lpData->GuildMaster);
	// ----
	if( !g_GuildMatchDB.Exec(Query) || g_GuildMatchDB.Fetch() == SQL_NO_DATA )
	{
		g_GuildMatchDB.Clear();
		DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_OFFERDELETE));
		return;
	}
	// ----
	lpAnswer.Result = g_GuildMatchDB.GetInt("Result");
	g_GuildMatchDB.Clear();
	DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_OFFERDELETE));
}
// -------------------------------------------------------------------------------

void GuildClass::GDReqJoinReqAdd(int aIndex, GUILDMATCH_GDREQ_JOINREQ* lpData)
{
	char Query[256];
	int Result = -1;
	// ----
	GUILDMATCH_DGANS_JOINREQ lpAnswer = { 0 };
	// ----
	lpAnswer.h.c = PMHC_BYTE;
	lpAnswer.h.headcode = 0xED;
	lpAnswer.h.subcode = 0x04;
	lpAnswer.h.size = sizeof(GUILDMATCH_DGANS_JOINREQ);
	lpAnswer.UserIndex = lpData->UserIndex;
	lpAnswer.Result = Result;
	// ----
	sprintf(Query, "EXEC GuildMatching_AddRequestList '%s', %d, %d",
		lpData->Sender, lpData->OfferNumber, 0);
	// ----
	if( !g_GuildMatchDB.Exec(Query) || g_GuildMatchDB.Fetch() == SQL_NO_DATA )
	{
		g_GuildMatchDB.Clear();
		DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_JOINREQ));
		return;
	}
	// ----
	lpAnswer.Result = g_GuildMatchDB.GetInt("Result");
	// ----
	if( lpAnswer.Result == 0 )
	{
		g_GuildMatchDB.GetStr("Recipient", lpAnswer.Recipient);
	}
	// ----
	g_GuildMatchDB.Clear();
	DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_JOINREQ));
}
// -------------------------------------------------------------------------------

void GuildClass::GDReqJoinReqAnswer(int aIndex, GUILDMATCH_GDREQ_REQUESTANSWER* lpData)
{
	char Query[256];
	// ----
	GUILDMATCH_DGANS_REQUESTANSWER lpAnswer = { 0 };
	// ----
	lpAnswer.h.c = PMHC_BYTE;
	lpAnswer.h.headcode = 0xED;
	lpAnswer.h.subcode = 0x06;
	lpAnswer.h.size = sizeof(GUILDMATCH_DGANS_REQUESTANSWER);
	lpAnswer.UserIndex = lpData->UserIndex;
	memcpy(lpAnswer.Character, lpData->Character, 10);
	lpAnswer.Type = lpData->Type;
	lpAnswer.Result = -1;
	// ----
	if( lpData->Type == 0 )
	{
		sprintf(Query, "DELETE FROM GuildMatching_RequestList WHERE Sender = '%s'", lpData->Character);
		// ----
		if( !g_GuildMatchDB.Exec(Query) )
		{
			g_GuildMatchDB.Clear();
			DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_REQUESTANSWER));
			return;
		}
		// ----
		g_GuildMatchDB.Clear();
		lpAnswer.Result = 1;
		DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_REQUESTANSWER));
	}
	else if( lpData->Type == 1 )
	{
		sprintf(Query, "SELECT Name FROM GuildMember WHERE Name = '%s'", lpData->Character);
		// ----
		if( !g_GuildMatchDB.Exec(Query) )
		{
			g_GuildMatchDB.Clear();
			DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_REQUESTANSWER));
			return;
		}
		// ----
		if( g_GuildMatchDB.Fetch() != SQL_NO_DATA )
		{
			//sprintf(Query, "DELETE FROM GuildMatching_RequestList WHERE Sender = '%s'", lpData->Character);
			//g_GuildMatchDB.Exec(Query);
			//g_GuildMatchDB.Clear();
			g_GuildMatchDB.Clear();
			lpAnswer.Result = 0;
			DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_REQUESTANSWER));
			return;
		}
		// ----
		g_GuildMatchDB.Clear();
		sprintf(Query, "INSERT GuildMember (Name, G_Name) VALUES('%s', '%s')", lpData->Character, lpData->Guild);
		// ----
		if( !g_GuildMatchDB.Exec(Query) )
		{
			g_GuildMatchDB.Clear();
			DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_REQUESTANSWER));
			return;
		}
		// ----
		g_GuildMatchDB.Clear();
		sprintf(Query, "DELETE FROM GuildMatching_RequestList WHERE Sender = '%s'", lpData->Character);
		g_GuildMatchDB.Exec(Query);
		g_GuildMatchDB.Clear();
		// ----
		lpAnswer.Result = 1;
		DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_REQUESTANSWER));
	}
}
// -------------------------------------------------------------------------------

void GuildClass::GDReqJoinReqStatus(int aIndex, GUILDMATCH_GDREQ_JOINREQSTATUS* lpData)
{
	char Query[256];
	int Result = -1;
	// ----
	GUILDMATCH_DGANS_JOINREQSTATUS lpAnswer = { 0 };
	// ----
	lpAnswer.h.c = PMHC_BYTE;
	lpAnswer.h.headcode = 0xED;
	lpAnswer.h.subcode = 0x08;
	lpAnswer.h.size = sizeof(GUILDMATCH_DGANS_JOINREQSTATUS);
	lpAnswer.UserIndex = lpData->UserIndex;
	lpAnswer.Result = Result;
	// ----
	sprintf(Query, "EXEC GuildMatching_GetRequestStatus '%s'", lpData->Sender);
	// ----
	if( !g_GuildMatchDB.Exec(Query) || g_GuildMatchDB.Fetch() == SQL_NO_DATA )
	{
		g_GuildMatchDB.Clear();
		DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_JOINREQSTATUS));
		return;
	}
	// ----	
	g_GuildMatchDB.GetStr("Recipient", lpAnswer.GuildMaster);
	g_GuildMatchDB.GetStr("Guild", lpAnswer.GuildName);
	lpAnswer.Result = g_GuildMatchDB.GetInt("Result");
	g_GuildMatchDB.Clear();
	DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_JOINREQSTATUS));
}
// -------------------------------------------------------------------------------

void GuildClass::GDReqJoinReqDelete(int aIndex, GUILDMATCH_GDREQ_JOINREQSTATUS* lpData)
{
	char Query[256];
	int Result = -1;
	// ----
	GUILDMATCH_DGANS_OFFERDELETE lpAnswer = { 0 };
	// ----
	lpAnswer.h.c = PMHC_BYTE;
	lpAnswer.h.headcode = 0xED;
	lpAnswer.h.subcode = 0x05;
	lpAnswer.h.size = sizeof(GUILDMATCH_DGANS_OFFERDELETE);
	lpAnswer.UserIndex = lpData->UserIndex;
	lpAnswer.Result = Result;
	// ----
	sprintf(Query, "DELETE FROM GuildMatching_RequestList WHERE Sender = '%s'", lpData->Sender);
	// ----
	if( !g_GuildMatchDB.Exec(Query) || g_GuildMatchDB.Fetch() == SQL_NO_DATA )
	{
		g_GuildMatchDB.Clear();
		DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_OFFERDELETE));
		return;
	}
	// ----	
	g_GuildMatchDB.Clear();
	lpAnswer.Result = 1;
	DataSend(aIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_DGANS_OFFERDELETE));
}
// -------------------------------------------------------------------------------

void GuildClass::GDReqJoinReqList(int aIndex, GUILDMATCH_GDREQ_REQUESTLIST* lpData)
{
	const int MaxSize = sizeof(GUILDMATCH_DGANS_REQUESTLIST_COUNT) + sizeof(GUILDMATCH_DGANS_REQUESTLIST_INFO) * 80;
	BYTE Buffer[MaxSize + 1] = { 0 };
	int Offset = sizeof(GUILDMATCH_DGANS_REQUESTLIST_COUNT);
	// ----
	GUILDMATCH_DGANS_REQUESTLIST_COUNT* lpAnswerCount = (GUILDMATCH_DGANS_REQUESTLIST_COUNT*)Buffer;
	GUILDMATCH_DGANS_REQUESTLIST_INFO* lpAnswerInfo = (GUILDMATCH_DGANS_REQUESTLIST_INFO*)&Buffer[Offset];
	// ----
	lpAnswerCount->h.c = PMHC_WORD;
	lpAnswerCount->h.headcode = 0xED;
	lpAnswerCount->h.subcode = 0x07;
	lpAnswerCount->h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount->h.sizeL = SET_NUMBERL(Offset);
	lpAnswerCount->UserIndex = lpData->UserIndex;
	// ----
	char Query[256];
	long Len = 0;
	// ----
	sprintf(Query, "EXEC GuildMatching_GetRequestList '%s'", lpData->GuildMaster);
	// ----
	if( !g_GuildMatchDB.Exec(Query) )
	{
		g_GuildMatchDB.Clear();
		DataSend(aIndex, (LPBYTE)lpAnswerCount, Offset);
		return;
	}
	// ----
	int Result, Count = 0;
	// ----
	while( (Result = g_GuildMatchDB.Fetch()) != SQL_NO_DATA && Result != SQL_ERROR )
	{
		g_GuildMatchDB.GetStr("Sender", lpAnswerInfo[Count].Character);
		lpAnswerInfo[Count].Class = g_GuildMatchDB.GetInt("SenderClass");
		lpAnswerInfo[Count].Level = g_GuildMatchDB.GetInt("SenderLevel");
		Offset += sizeof(GUILDMATCH_DGANS_REQUESTLIST_INFO);
		Count++;
	}
	// ----
	while( g_GuildMatchDB.MoreResults() != SQL_NO_DATA )
	{
		
	}
	// ----
	g_GuildMatchDB.Clear();
	// ----
	lpAnswerCount->h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount->h.sizeL = SET_NUMBERL(Offset);
	lpAnswerCount->Count = Count;
	// ----
	DataSend(aIndex, (LPBYTE)Buffer, Offset);
}
// -------------------------------------------------------------------------------