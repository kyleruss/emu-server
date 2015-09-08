#include "stdafx.h"
#include "GuildMatch.h"
#include "giocp.h"
#include "DBSockMng.h"
#include "EDSProtocol.h"
#include "GameMain.h"
// -------------------------------------------------------------------------------

GuildMatch g_GuildMatch;
// -------------------------------------------------------------------------------

GuildMatch::GuildMatch()
{

}
// -------------------------------------------------------------------------------

GuildMatch::~GuildMatch()
{

}
// -------------------------------------------------------------------------------

void GuildMatch::CGReqOfferList(int UserIndex, GUILDMATCH_CGREQ_OFFERLIST* lpData)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_OFFERLIST lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x00;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_OFFERLIST);
	lpRequest.CurrentPage	= lpData->CurrentPage;
	lpRequest.UserIndex		= UserIndex;
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(lpRequest));
}
// -------------------------------------------------------------------------------

void GuildMatch::CGReqOfferListByFilter(int UserIndex, GUILDMATCH_CGREQ_OFFERLISTFILTER* lpData)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_OFFERLISTFILTER lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x01;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_OFFERLISTFILTER);
	lpRequest.CurrentPage	= lpData->CurrentPage;
	lpRequest.UserIndex		= UserIndex;
	memcpy(lpRequest.KeyWord, lpData->KeyWord, sizeof(lpRequest.KeyWord));
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(lpRequest));
}
// -------------------------------------------------------------------------------

void GuildMatch::DGAnsOfferList(LPBYTE lpData)
{
	GUILDMATCH_DGANS_OFFERLIST_COUNT* lpRecvCount = (GUILDMATCH_DGANS_OFFERLIST_COUNT*)lpData;
	// ----
	if(		!OBJMAX_RANGE(lpRecvCount->UserIndex) 
		||	gObj[lpRecvCount->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GCANS_OFFERLIST_COUNT lpAnswerCount = { 0 };
	lpAnswerCount.h.c			= 0xC2;
	lpAnswerCount.h.headcode	= 0xED;
	lpAnswerCount.h.subcode		= 0x00;
	lpAnswerCount.h.sizeH		= SET_NUMBERH(sizeof(GUILDMATCH_GCANS_OFFERLIST_COUNT));
	lpAnswerCount.h.sizeL		= SET_NUMBERL(sizeof(GUILDMATCH_GCANS_OFFERLIST_COUNT));
	lpAnswerCount.CurrentPage	= lpRecvCount->CurrentPage;
	lpAnswerCount.MaxPage		= lpRecvCount->MaxPage;
	lpAnswerCount.OfferCount	= lpRecvCount->OfferCount;
	int Offset = sizeof(GUILDMATCH_GCANS_OFFERLIST_COUNT);
	// ----
	if( lpAnswerCount.OfferCount <= 0 )
	{
		DataSend(lpRecvCount->UserIndex, (LPBYTE)&lpAnswerCount, Offset);
		return;
	}
	// ----
	GUILDMATCH_DGANS_OFFERLIST_INFO* lpRecvInfo	= (GUILDMATCH_DGANS_OFFERLIST_INFO*)&lpData[sizeof(GUILDMATCH_DGANS_OFFERLIST_COUNT)];
	const int Size = sizeof(GUILDMATCH_GCANS_OFFERLIST_COUNT) + sizeof(GUILDMATCH_GCANS_OFFERLIST_INFO) * GUILD_MATCH_MAX_PAGELINE;
	BYTE Buffer[Size + 1] = { 0 };
	// ----
	for( int i = 0; i < lpAnswerCount.OfferCount; i++ )
	{
		GUILDMATCH_GCANS_OFFERLIST_INFO lpAnswerInfo = { 0 };
		// ----
		memcpy(lpAnswerInfo.Title, lpRecvInfo[i].Title, sizeof(lpAnswerInfo.Title));
		memcpy(lpAnswerInfo.GuildMaster, lpRecvInfo[i].GuildMaster, sizeof(lpAnswerInfo.GuildMaster));
		memcpy(lpAnswerInfo.GuildName, lpRecvInfo[i].GuildName, sizeof(lpAnswerInfo.GuildName));
		lpAnswerInfo.GuildMemberCount = lpRecvInfo[i].GuildMemberCount;
		lpAnswerInfo.GuildMasterClass = lpRecvInfo[i].GuildMasterClass / 16; //wrong...
		lpAnswerInfo.GuildInterests = lpRecvInfo[i].GuildInterests;
		lpAnswerInfo.GuildReqLevel = lpRecvInfo[i].GuildReqLevel;
		lpAnswerInfo.GuildReqClass = lpRecvInfo[i].GuildReqClass;
		lpAnswerInfo.GuildMasterLevel = lpRecvInfo[i].GuildMasterLevel;
		lpAnswerInfo.GuildGensClass = lpRecvInfo[i].GuildGensClass;
		lpAnswerInfo.OfferNumber = lpRecvInfo[i].OfferNumber;
		memcpy(&Buffer[Offset], (LPBYTE)&lpAnswerInfo, sizeof(GUILDMATCH_GCANS_OFFERLIST_INFO));
		Offset += sizeof(GUILDMATCH_GCANS_OFFERLIST_INFO);
	}
	// ----
	lpAnswerCount.h.sizeH = SET_NUMBERH(Offset);
	lpAnswerCount.h.sizeL = SET_NUMBERL(Offset);
	memcpy(Buffer, (LPBYTE)&lpAnswerCount, sizeof(GUILDMATCH_GCANS_OFFERLIST_COUNT));
	DataSend(lpRecvCount->UserIndex, Buffer, Offset);
}
// -------------------------------------------------------------------------------

void GuildMatch::CGReqOfferCreate(int UserIndex, GUILDMATCH_CGREQ_OFFERCREATE* lpData)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	if( gObj[UserIndex].GuildStatus != G_MASTER )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_OFFERCREATE lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x02;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_OFFERCREATE);
	lpRequest.UserIndex		= UserIndex;
	memcpy(lpRequest.Title, lpData->Title, 40);
	memcpy(lpRequest.GuildMaster, gObj[UserIndex].Name, 10);
	lpRequest.GuildInterests = lpData->GuildInterests;
	lpRequest.GuildReqLevel	= lpData->GuildReqLevel;
	lpRequest.GuildReqClass = lpData->GuildReqClass;	
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(GUILDMATCH_GDREQ_OFFERCREATE));
}
// -------------------------------------------------------------------------------

void GuildMatch::DGAnsOfferCreate(GUILDMATCH_DGANS_OFFERCREATE* lpData)
{
	if(		!OBJMAX_RANGE(lpData->UserIndex) 
		||	gObj[lpData->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_OFFERLIST lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x00;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_OFFERLIST);
	lpRequest.CurrentPage	= 1;
	lpRequest.UserIndex		= lpData->UserIndex;
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(lpRequest));
	// ----
	GUILDMATCH_GCANS_OFFERCREATE lpAnswer = { 0 };
	lpAnswer.h.c		= 0xC1;
	lpAnswer.h.headcode	= 0xED;
	lpAnswer.h.subcode	= 0x02;
	lpAnswer.h.size		= sizeof(GUILDMATCH_GCANS_OFFERCREATE);
	lpAnswer.Result		= 0;
	DataSend(lpData->UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_GCANS_OFFERCREATE));
}
// -------------------------------------------------------------------------------

void GuildMatch::CGReqOfferDelete(int UserIndex, GUILDMATCH_CGREQ_OFFERDELETE* lpData)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	if( gObj[UserIndex].GuildStatus != G_MASTER )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_OFFERDELETE lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x03;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_OFFERDELETE);
	memcpy(lpRequest.GuildMaster, gObj[UserIndex].Name, 10);
	lpRequest.UserIndex		= UserIndex;
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(GUILDMATCH_GDREQ_OFFERDELETE));
}
// -------------------------------------------------------------------------------

void GuildMatch::DGAnsOfferDelete(GUILDMATCH_DGANS_OFFERDELETE* lpData)
{
	if(		!OBJMAX_RANGE(lpData->UserIndex) 
		||	gObj[lpData->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_OFFERLIST lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x00;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_OFFERLIST);
	lpRequest.CurrentPage	= 1;
	lpRequest.UserIndex		= lpData->UserIndex;
	// ----
	GUILDMATCH_CGANS_OFFERDELETE lpAnswer = { 0 };
	lpAnswer.h.c		= 0xC1;
	lpAnswer.h.headcode	= 0xED;
	lpAnswer.h.subcode	= 0x03;
	lpAnswer.h.size		= sizeof(GUILDMATCH_CGANS_OFFERDELETE);
	// ----
	if( lpData->Result == 0 )
	{
		lpAnswer.Result = -3;
	}
	else if( lpData->Result == 1 )
	{
		lpAnswer.Result = 0;
		//cDBSMng.Send((PCHAR)&lpRequest, sizeof(lpRequest));
	}
	// ----
	DataSend(lpData->UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_CGANS_OFFERDELETE));
}
// -------------------------------------------------------------------------------

void GuildMatch::CGReqJoinReq(int UserIndex, GUILDMATCH_CGREQ_JOINREQ* lpData)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	if( gObj[UserIndex].GuildNumber > 0 )
	{
		GUILDMATCH_GCANS_JOINREQ lpAnswer = { 0 };
		lpAnswer.h.c		= 0xC1;
		lpAnswer.h.headcode	= 0xED;
		lpAnswer.h.subcode	= 0x04;
		lpAnswer.h.size		= sizeof(GUILDMATCH_GCANS_JOINREQ);
		lpAnswer.Result		= -3;
		DataSend(UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_GCANS_JOINREQ));
		return;
	}
	// ----
	GUILDMATCH_GDREQ_JOINREQ lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x04;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_JOINREQ);
	lpRequest.UserIndex		= UserIndex;
	memcpy(lpRequest.Sender, gObj[UserIndex].Name, 10);
	lpRequest.OfferNumber	= lpData->OfferNumber;
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(GUILDMATCH_GDREQ_JOINREQ));
}
// -------------------------------------------------------------------------------

void GuildMatch::DGAnsJoinReq(GUILDMATCH_DGANS_JOINREQ* lpData)
{
	if(		!OBJMAX_RANGE(lpData->UserIndex) 
		||	gObj[lpData->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GCANS_JOINREQ lpAnswer = { 0 };
	lpAnswer.h.c		= 0xC1;
	lpAnswer.h.headcode	= 0xED;
	lpAnswer.h.subcode	= 0x04;
	lpAnswer.h.size		= sizeof(GUILDMATCH_GCANS_JOINREQ);
	lpAnswer.Result		= lpData->Result;
	DataSend(lpData->UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_GCANS_JOINREQ));
	// ----
	if( lpData->Result == 0 )
	{
		for( int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++ )
		{
			if( gObj[i].Name != NULL && !strcmp(lpData->Recipient, gObj[i].Name) )
			{
				if( gObj[i].Connected == PLAYER_PLAYING )
				{
					GUILDMATCH_GCSET_JOINNOTIFICATION lpNotification = { 0 };
					lpNotification.h.c			= 0xC1;
					lpNotification.h.headcode	= 0xED;
					lpNotification.h.subcode	= 0x10;
					lpNotification.h.size		= sizeof(GUILDMATCH_GCSET_JOINNOTIFICATION);
					DataSend(i, (LPBYTE)&lpNotification, sizeof(GUILDMATCH_GCSET_JOINNOTIFICATION));
					break;
				}
			}
		}
	}
	
}
// -------------------------------------------------------------------------------

void GuildMatch::CGReqJoinReqDelete(int UserIndex, GUILDMATCH_CGREQ_JOINREQDELETE* lpData)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_JOINREQSTATUS lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x05;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_JOINREQSTATUS);
	lpRequest.UserIndex		= UserIndex;
	memcpy(lpRequest.Sender, gObj[UserIndex].Name, 10);
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(GUILDMATCH_GDREQ_JOINREQSTATUS));
}
// -------------------------------------------------------------------------------

void GuildMatch::DGAnsJoinReqDelete(GUILDMATCH_DGANS_OFFERDELETE* lpData)
{
	if(		!OBJMAX_RANGE(lpData->UserIndex) 
		||	gObj[lpData->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GCANS_JOINREQDELETE lpAnswer = { 0 };
	lpAnswer.h.c			= 0xC1;
	lpAnswer.h.headcode		= 0xED;
	lpAnswer.h.subcode		= 0x05;
	lpAnswer.h.size			= sizeof(GUILDMATCH_GCANS_JOINREQDELETE);
	// ----
	if( lpData->Result == 1 )
	{
		DataSend(lpData->UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_GCANS_JOINREQDELETE));
	}
}
// -------------------------------------------------------------------------------

void GuildMatch::CGReqJoinReqAnswer(int UserIndex, GUILDMATCH_CGREQ_REQUESTANSWER* lpData)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_REQUESTANSWER lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x06;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_REQUESTANSWER);
	lpRequest.UserIndex		= UserIndex;
	lpRequest.Type			= lpData->Result;
	memcpy(lpRequest.Character, lpData->Character, 10);
	memcpy(lpRequest.Guild, gObj[UserIndex].lpGuild->Name, 8);
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(GUILDMATCH_GDREQ_REQUESTANSWER));
}
// -------------------------------------------------------------------------------

void GuildMatch::DGAnsJoinReqAnswer(GUILDMATCH_DGANS_REQUESTANSWER* lpData)
{
	if(		!OBJMAX_RANGE(lpData->UserIndex)
		||	gObj[lpData->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GCANS_REQUESTANSWER lpAnswer = { 0 };
	lpAnswer.h.c			= 0xC1;
	lpAnswer.h.headcode		= 0xED;
	lpAnswer.h.subcode		= 0x06;
	lpAnswer.h.size			= sizeof(GUILDMATCH_GCANS_REQUESTANSWER);
	lpAnswer.Type			= lpData->Type;
	memcpy(lpAnswer.Character, lpData->Character, 10);
	// ----
	if( lpData->Type == 0 )
	{
		if( lpData->Result == -1 )
		{
			lpAnswer.Type = -1;
			DataSend(lpData->UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_GCANS_REQUESTANSWER));
			return;
		}
		// ----
		DataSend(lpData->UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_GCANS_REQUESTANSWER));
	}
	else if( lpData->Type == 1 )
	{
		if( lpData->Result == -1 )
		{
			lpAnswer.Type = -1;
			DataSend(lpData->UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_GCANS_REQUESTANSWER));
			return;
		}
		// ----
		if( lpData->Result == 0 ) //User already in guild
		{
			lpAnswer.Type = -1;
			DataSend(lpData->UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_GCANS_REQUESTANSWER));
			return;
		}
		// ----
		for( int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++ )
		{
			if( gObj[i].Name != NULL && !strcmp(lpData->Character, gObj[i].Name) )
			{
				if( gObj[i].Connected == PLAYER_PLAYING )
				{
					_GUILD_INFO_STRUCT* lpGuild = Guild.AddMember(gObj[lpData->UserIndex].lpGuild->Name, gObj[i].Name, i, -1, 0, gGameServerCode / 20);
					// ----
					if( lpGuild != NULL )
					{
						gObj[i].lpGuild = lpGuild;
						gObj[i].GuildStatus = 0;
						gObj[i].GuildNumber = lpGuild->Number;
						gObj[i].iGuildUnionTimeStamp = 0;
						GCGuildViewportNowPaint(i, gObj[lpData->UserIndex].lpGuild->Name, gObj[i].lpGuild->Mark, FALSE);
						break;
					}
				}
			}
		}
		// ----
		DataSend(lpData->UserIndex, (LPBYTE)&lpAnswer, sizeof(GUILDMATCH_GCANS_REQUESTANSWER));
	}
}
// -------------------------------------------------------------------------------

void GuildMatch::CGReqJoinReqList(int UserIndex, GUILDMATCH_CGREQ_REQUESTLIST* lpData)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_REQUESTLIST lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x07;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_REQUESTLIST);
	lpRequest.UserIndex		= UserIndex;
	memcpy(lpRequest.GuildMaster, gObj[UserIndex].Name, 10);
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(GUILDMATCH_GDREQ_REQUESTLIST));
}
// -------------------------------------------------------------------------------

void GuildMatch::DGAnsJoinReqList(LPBYTE lpData)
{
	GUILDMATCH_DGANS_REQUESTLIST_COUNT* lpRecvCount = (GUILDMATCH_DGANS_REQUESTLIST_COUNT*)lpData;
	// ----
	if(		!OBJMAX_RANGE(lpRecvCount->UserIndex) 
		||	gObj[lpRecvCount->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GCANS_REQUESTLIST_COUNT lpAnswer = { 0 };
	lpAnswer.h.c			= 0xC2;
	lpAnswer.h.headcode		= 0xED;
	lpAnswer.h.subcode		= 0x07;
	lpAnswer.h.sizeH		= SET_NUMBERH(sizeof(lpAnswer));
	lpAnswer.h.sizeL		= SET_NUMBERL(sizeof(lpAnswer));
	lpAnswer.RequestCount	= lpRecvCount->Count;
	// ----
	GUILDMATCH_DGANS_REQUESTLIST_INFO* lpRecvInfo = (GUILDMATCH_DGANS_REQUESTLIST_INFO*)&lpData[sizeof(GUILDMATCH_DGANS_REQUESTLIST_COUNT)];
	// ----
	for( int i = 0; i < lpAnswer.RequestCount; i++ )
	{
		memcpy(lpAnswer.m_RequestList[i].Character, lpRecvInfo[i].Character, 10);
		lpAnswer.m_RequestList[i].Class = lpRecvInfo[i].Class / 16; //wrong...
		lpAnswer.m_RequestList[i].Level = lpRecvInfo[i].Level;
	}
	// ----
	DataSend(lpRecvCount->UserIndex, (LPBYTE)&lpAnswer, sizeof(lpAnswer));
}
// -------------------------------------------------------------------------------

void GuildMatch::CGReqJoinReqStatus(int UserIndex, GUILDMATCH_CGREQ_JOINREQSTATUS* lpData)
{
	if(		!OBJMAX_RANGE(UserIndex) 
		||	gObj[UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GDREQ_JOINREQSTATUS lpRequest = { 0 };
	lpRequest.h.c			= 0xC1;
	lpRequest.h.headcode	= 0xED;
	lpRequest.h.subcode		= 0x08;
	lpRequest.h.size		= sizeof(GUILDMATCH_GDREQ_JOINREQSTATUS);
	lpRequest.UserIndex		= UserIndex;
	memcpy(lpRequest.Sender, gObj[UserIndex].Name, 10);
	cDBSMng.Send((PCHAR)&lpRequest, sizeof(GUILDMATCH_GDREQ_JOINREQSTATUS));
}
// -------------------------------------------------------------------------------

void GuildMatch::DGAnsJoinReqStatus(GUILDMATCH_DGANS_JOINREQSTATUS* lpData)
{
	if(		!OBJMAX_RANGE(lpData->UserIndex) 
		||	gObj[lpData->UserIndex].Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GUILDMATCH_GCANS_JOINREQSTATUS lpInfo = { 0 };
	lpInfo.h.c			= 0xC1;
	lpInfo.h.headcode	= 0xED;
	lpInfo.h.subcode	= 0x08;
	lpInfo.h.size		= sizeof(GUILDMATCH_GCANS_JOINREQSTATUS);
	lpInfo.Unknown14	= 0x18;
	lpInfo.Unknown23	= 0x0B;
	// ----
	if( lpData->Result != 1 )
	{
		lpInfo.Status = -2;
		DataSend(lpData->UserIndex, (LPBYTE)&lpInfo, sizeof(GUILDMATCH_GCANS_JOINREQSTATUS));
		return;
	}
	// ----
	lpInfo.Status = 0;
	memcpy(lpInfo.GuildMaster, lpData->GuildMaster, 10);
	memcpy(lpInfo.GuildName, lpData->GuildName, 8);
	DataSend(lpData->UserIndex, (LPBYTE)&lpInfo, sizeof(GUILDMATCH_GCANS_JOINREQSTATUS));
}
// -------------------------------------------------------------------------------