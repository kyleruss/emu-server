#pragma once
// -------------------------------------------------------------------------------

#include "..\common\winutil.h"
// -------------------------------------------------------------------------------

#define GUILD_MATCH_MAX_PAGELINE	9
#define GUILD_MATCH_MAX_REQLIST		80
// -------------------------------------------------------------------------------

#define GUILD_INTEREST_LEVEL	1
#define GUILD_INTEREST_EVENT	2
#define GUILD_INTEREST_PVP		4
#define GUILD_INTEREST_ARCA		8
#define GUILD_INTEREST_SIEGE	16
#define GUILD_REQLEVEL_1_100	1
#define GUILD_REQLEVEL_101_200	2
#define GUILD_REQLEVEL_201_300	4
#define GUILD_REQLEVEL_301_400	8
#define GUILD_REQLEVEL_400		16
#define GUILD_REQCLASS_KNIGHT	1
#define GUILD_REQCLASS_ELF		2
#define GUILD_REQCLASS_WIZARD	4
#define GUILD_REQCLASS_MAGUMSA	8
#define GUILD_REQCLASS_DARKLORD	16
#define GUILD_REQCLASS_SUMMONER	32
#define GUILD_REQCLASS_MONK		64
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_CGREQ_OFFERLIST //-> 8 [0xC1 -> 0xED -> 0x00]
{
	PBMSG_HEAD2 h;			//+0
	int CurrentPage;		//+4
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCANS_OFFERLIST_COUNT //-> [0xC2 -> 0xED -> 0x00]
{
	PWMSG_HEAD2 h;
	WORD Unknown5;
	BYTE Unknown7;
	int CurrentPage;
	int MaxPage;
	int OfferCount;
	int Unknown16;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCANS_OFFERLIST_INFO
{
	char Title[24];			//+0
	BYTE Gap0[17];			//+24
	char GuildMaster[11];	//+41
	char GuildName[9];		//+52
	BYTE GuildMemberCount;	//+61
	BYTE GuildMasterClass;	//+62
	BYTE GuildInterests;	//+63
	BYTE GuildReqLevel;		//+64
	BYTE GuildReqClass;		//+65
	WORD Unknown66;			//+66
	int GuildMasterLevel;	//+68
	DWORD Unknown72;		//+72 (maybe row id with info)
	DWORD OfferNumber;		//+76
	BYTE GuildGensClass;	//+80
	BYTE Gap3[3];
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_CGREQ_OFFERLISTFILTER //-> 19 [0xC1 -> 0xED -> 0x01]
{
	PBMSG_HEAD2 h;			//+0
	int CurrentPage;		//+4
	char KeyWord[11];		//+8
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_CGREQ_OFFERCREATE //-> 48 [0xC1 -> 0xED -> 0x02]
{
	PBMSG_HEAD2 h;			//+0
	char Title[41];			//+4
	BYTE GuildInterests;	//+45
	BYTE GuildReqLevel;		//+46
	BYTE GuildReqClass;		//+47
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCANS_OFFERCREATE  //-> 8 [0xC1 -> 0xED -> 0x02]
{
	PBMSG_HEAD2 h;			//+0
	int Result;				//+4 (0: created)
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_CGREQ_OFFERDELETE //-> 4 [0xC1 -> 0xED -> 0x03]
{
	PBMSG_HEAD2 h;			//+0
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_CGANS_OFFERDELETE //-> 8 [0xC1 -> 0xED -> 0x03]
{
	PBMSG_HEAD2 h;			//+0
	int Result;				//+4 (0: deleted, -3: not have ad)
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_CGREQ_JOINREQ //-> 8 [0xC1 -> 0xED -> 0x04]
{
	PBMSG_HEAD2 h;			//+0
	int OfferNumber;		//+4
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCANS_JOINREQ //-> 8 [0xC1 -> 0xED -> 0x04]
{
	PBMSG_HEAD2 h;			//+0
	int Result;				//+4 (-3: already in guild, -2: PK, -1: already wait, 0: sended)
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_CGREQ_JOINREQDELETE //-> 4 [0xC1 -> 0xED -> 0x05]
{
	PBMSG_HEAD2 h;			//+0
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCANS_JOINREQDELETE //-> 8 [0xC1 -> 0xED -> 0x05]
{
	PBMSG_HEAD2 h;			//+0
	int Result;				//+4
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_CGREQ_REQUESTANSWER //-> 19 [0xC1 -> 0xED -> 0x06]
{
	PBMSG_HEAD2 h;			//+0
	int Result;				//+4 (0: aborted, 1: accepted)
	char Character[11];		//+8
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCANS_REQUESTANSWER //-> 24 [0xC1 -> 0xED -> 0x06]
{
	PBMSG_HEAD2 h;			//+0
	char Character[10];		//+4
	WORD Unknown01;			//+14 (7C 76, like a UserIndex or OfferID)
	int Type;				//+16 (0: aborted, 1: accepted)
	int Unknown02;			//+20
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

struct GUILDMATCH_CGREQ_REQUESTLIST //-> 4 [0xC1 -> 0xED -> 0x07]
{
	PBMSG_HEAD2 h;			//+0
};
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCANS_REQUESTLIST_INFO
{
	char Character[11];
	BYTE Class;
	int Level;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCANS_REQUESTLIST_COUNT //-> 16 [0xC2 -> 0xED -> 0x07]
{
	PWMSG_HEAD2 h;			//+0
	WORD Unknown01;			//+5
	BYTE Unknown02;			//+7
	int RequestCount;		//+8
	int Unknown03;			//+12
	GUILDMATCH_GCANS_REQUESTLIST_INFO m_RequestList[GUILD_MATCH_MAX_REQLIST];
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

struct GUILDMATCH_CGREQ_JOINREQSTATUS //-> 4 [0xC1 -> 0xED -> 0x08]
{
	PBMSG_HEAD2 h;			//+0
};
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCANS_JOINREQSTATUS //-> 28 [0xC1 -> 0xED -> 0x08]
{
	PBMSG_HEAD2 h;			//+0
	char GuildMaster[10];	//+4
	BYTE Unknown14;			//+13
	char GuildName[8];		//+14
	BYTE Unknown23;			//+24
	DWORD Status;			//+25 (0: wait, -2: without data)
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct GUILDMATCH_GCSET_JOINNOTIFICATION //-> 8 [0xC1 -> 0xED -> 0x10]
{
	PBMSG_HEAD2 h;			//+0
	int Result;				//+4
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

//DS Protocol start

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

class GuildMatch  
{
public:
			GuildMatch();
	virtual ~GuildMatch();
	
	void	CGReqOfferList(int UserIndex, GUILDMATCH_CGREQ_OFFERLIST* lpData);
	void	CGReqOfferListByFilter(int UserIndex, GUILDMATCH_CGREQ_OFFERLISTFILTER* lpData);
	void	DGAnsOfferList(LPBYTE lpData);
	void	CGReqOfferCreate(int UserIndex, GUILDMATCH_CGREQ_OFFERCREATE* lpData);
	void	DGAnsOfferCreate(GUILDMATCH_DGANS_OFFERCREATE* lpData);
	void	CGReqOfferDelete(int UserIndex, GUILDMATCH_CGREQ_OFFERDELETE* lpData);
	void	DGAnsOfferDelete(GUILDMATCH_DGANS_OFFERDELETE* lpData);
	void	CGReqJoinReq(int UserIndex, GUILDMATCH_CGREQ_JOINREQ* lpData);
	void	DGAnsJoinReq(GUILDMATCH_DGANS_JOINREQ* lpData);
	void	CGReqJoinReqDelete(int UserIndex, GUILDMATCH_CGREQ_JOINREQDELETE* lpData);
	void	DGAnsJoinReqDelete(GUILDMATCH_DGANS_OFFERDELETE* lpData);
	void	CGReqJoinReqAnswer(int UserIndex, GUILDMATCH_CGREQ_REQUESTANSWER* lpData);
	void	DGAnsJoinReqAnswer(GUILDMATCH_DGANS_REQUESTANSWER* lpData);
	void	CGReqJoinReqList(int UserIndex, GUILDMATCH_CGREQ_REQUESTLIST* lpData);
	void	DGAnsJoinReqList(LPBYTE lpData);
	void	CGReqJoinReqStatus(int UserIndex, GUILDMATCH_CGREQ_JOINREQSTATUS* lpData);
	void	DGAnsJoinReqStatus(GUILDMATCH_DGANS_JOINREQSTATUS* lpData);

private:
	std::vector<GUILDMATCH_GCANS_OFFERLIST_INFO> m_OfferData;

}; extern GuildMatch g_GuildMatch;
// -------------------------------------------------------------------------------