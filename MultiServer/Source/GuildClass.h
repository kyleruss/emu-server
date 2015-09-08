#pragma once
// -------------------------------------------------------------------------------

#include "CQuery.h"
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GDREQ_OFFERLIST
{
	PBMSG_HEAD2 h;			//+0
	int UserIndex;			//+4
	int CurrentPage;		//+8
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GDREQ_OFFERLISTFILTER
{
	PBMSG_HEAD2 h;			//+0
	int UserIndex;			//+4
	int CurrentPage;		//+8
	char KeyWord[11];		//+12
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_DGANS_OFFERLIST_INFO
{
	char Title[24];
	char GuildMaster[11];
	char GuildName[9];
	BYTE GuildMemberCount;
	BYTE GuildMasterClass;
	BYTE GuildInterests;
	BYTE GuildReqLevel;
	BYTE GuildReqClass;
	int GuildMasterLevel;
	BYTE GuildGensClass;
	int OfferNumber;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_DGANS_OFFERLIST_COUNT
{
	PWMSG_HEAD2 h;			//+0
	int UserIndex;			//+5
	int CurrentPage;		//+9
	int MaxPage;			//+13
	int OfferCount;			//+17
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GDREQ_OFFERCREATE
{
	PBMSG_HEAD2 h;
	int UserIndex;
	char Title[41];
	char GuildMaster[11];
	BYTE GuildInterests;
	BYTE GuildReqLevel;
	BYTE GuildReqClass;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_DGANS_OFFERCREATE
{
	PBMSG_HEAD2 h;
	int UserIndex;
	int Result;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GDREQ_OFFERDELETE
{
	PBMSG_HEAD2 h;
	char GuildMaster[11];
	int UserIndex;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_DGANS_OFFERDELETE
{
	PBMSG_HEAD2 h;
	int UserIndex;
	int Result;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GDREQ_JOINREQ
{
	PBMSG_HEAD2 h;
	int UserIndex;
	int Sender[11];
	int OfferNumber;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_DGANS_JOINREQ
{
	PBMSG_HEAD2 h;
	int UserIndex;
	char Recipient[11];
	int Result;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GDREQ_JOINREQSTATUS
{
	PBMSG_HEAD2 h;
	int UserIndex;
	int Sender[11];
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_DGANS_JOINREQSTATUS
{
	PBMSG_HEAD2 h;
	int UserIndex;
	int Result;
	char GuildMaster[11];
	char GuildName[9];
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GDREQ_REQUESTLIST
{
	PBMSG_HEAD2 h;
	int UserIndex;
	char GuildMaster[11];
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_DGANS_REQUESTLIST_COUNT
{
	PWMSG_HEAD2 h;
	int UserIndex;
	int Count;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_DGANS_REQUESTLIST_INFO
{
	char Character[11];
	BYTE Class;
	int Level;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GDREQ_REQUESTANSWER
{
	PBMSG_HEAD2 h;
	int UserIndex;
	char Character[11];
	char Guild[9];
	int Type;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_DGANS_REQUESTANSWER
{
	PBMSG_HEAD2 h;
	int UserIndex;
	char Character[11];
	int Type;
	int Result;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

class GuildClass  
{
public:
			GuildClass();
	virtual ~GuildClass();

	void	GDReqOfferList(int aIndex, GUILDMATCH_GDREQ_OFFERLIST* lpData);
	void	GDReqOfferListByFilter(int aIndex, GUILDMATCH_GDREQ_OFFERLISTFILTER* lpData);
	void	GDReqOfferCreate(int aIndex, GUILDMATCH_GDREQ_OFFERCREATE* lpData);
	void	GDReqOfferDelete(int aIndex, GUILDMATCH_GDREQ_OFFERDELETE* lpData);
	void	GDReqJoinReqAdd(int aIndex, GUILDMATCH_GDREQ_JOINREQ* lpData);
	void	GDReqJoinReqAnswer(int aIndex, GUILDMATCH_GDREQ_REQUESTANSWER* lpData);
	void	GDReqJoinReqDelete(int aIndex, GUILDMATCH_GDREQ_JOINREQSTATUS* lpData);
	void	GDReqJoinReqList(int aIndex, GUILDMATCH_GDREQ_REQUESTLIST* lpData);
	void	GDReqJoinReqStatus(int aIndex, GUILDMATCH_GDREQ_JOINREQSTATUS* lpData);

}; extern GuildClass g_GuildClass;
// -------------------------------------------------------------------------------

extern CQuery g_GuildMatchDB;
// -------------------------------------------------------------------------------