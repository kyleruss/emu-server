#ifndef __DATASERVER_H__
#define __DATASERVER_H__

#include "Server.h"
#include "PartyClass.h"
#include "GuildClass.h"
#include "zMultiServer.h"


#define INVENTORY_BINARY_SIZE 3792

#define DS_INISECTION "DataServer"

typedef struct
{
	int bCharCreate;
	WORD TCPPort;
	char dbDSN[64];
	int bResetCol;
	char dbResetCol[64];
	char CreditTable[64];
	char CreditColumnCredits[64];
	char CreditColumnAccount[64];
}DS_INFO, *LPDS_INFO;
extern DS_INFO g_DSInfo;

// Protocol definition

// 0x01
typedef struct
{
	PBMSG_HEAD h;	// C1:01
	char Id[10];	// 3
	short Number;	// E
}SDHP_GETCHARLIST, *LPSDHP_GETCHARLIST;

typedef struct 
{
	PWMSG_HEAD h;
	short Number;	// 4
	BYTE Count;	// 6
	int DbNumber;	// 8
	BYTE Magumsa;	// C
	char AccountId[MAX_IDSTRING+1];	// D
	BYTE MoveCnt;	// 18
	BYTE btExtendVault;
}SDHP_CHARLISTCOUNT, *LPSDHP_CHARLISTCOUNT;


typedef struct
{	BYTE Index;	// 0
	char Name[MAX_IDSTRING];	// 1
	WORD Level;	// C
	BYTE Class;	// E
	BYTE CtlCode;	// F
	BYTE dbInventory[48];	// 10
	BYTE DbVersion;	// 40
	BYTE btGuildStatus;	// 41
}SDHP_CHARLIST, *LPSDHP_CHARLIST;

// 0x04

typedef struct
{
	PBMSG_HEAD h;	// C1:04
	int UserNumber;	// 4
	int DBNumber;	// 8
	short Number;	// C
	char AccountId[MAX_IDSTRING];	// E
	char Name[MAX_IDSTRING];	// 18
	BYTE ClassSkin;	// 22
}SDHP_CREATECHAR, *LPSDHP_CREATECHAR;

typedef struct 
{
	PBMSG_HEAD h;	// C1:04
	unsigned char Result;	// 3
	short Number;	// 4
	char AccountId[MAX_IDSTRING];	// 6
	char Name[MAX_IDSTRING];	// 10
	BYTE Pos;	// 1A
	BYTE ClassSkin;	// 1B
	BYTE Equipment[24];	// 1C
	WORD Level;	// 34
}SDHP_CREATECHARRESULT, *LPSDHP_CREATECHARRESULT;


// 0x05
typedef struct 
{
	PBMSG_HEAD h;	// C1:05
	short Number;	// 4
	char AccountID[10];	// 6
	char Name[10];	// 10
	BYTE Guild;	// [0:NoGuild] [1:Master] [2:Member] 1A
	char GuildName[8];	// 1B
}SDHP_CHARDELETE, *LPSDHP_CHARDELETE;

typedef struct
{
	PBMSG_HEAD h;	// C1:05
	BYTE Result;	// 3
	short Number;	// 4
	char AccountID[10];	// 6
}SDHP_CHARDELETERESULT, *LPSDHP_CHARDELETERESULT;

void DataServerInit();
void DSProtocolCore(int aIndex, DWORD headcode, LPBYTE aRecv, int Len);

// 0x06

typedef struct
{
	PBMSG_HEAD h;	// C1:06
	char AccountID[11];	// 3
	char Name[11];	// E
	short Number;	// 1A
}SDHP_DBCHARINFOREQUEST, *LPSDHP_DBCHARINFOREQUEST;

typedef struct
{
	PBMSG_HEAD h;
	int Index;
	char szName[11];
}SDHP_DBMASTERINFOREQUEST, *LPSDHP_DBMASTERINFOREQUEST;


typedef struct
{
	PWMSG_HEAD h;
	unsigned char result;	// 4
	short Number;	// 6
	char AccountID[10];	// 8
	char Name[11];	// 12
	BYTE Class;	// 1D
	short Level;	// 1E
	int LevelUpPoint;	// 20
	int Exp;	// 24
	int NextExp;	// 28
	int Money;	// 2C
#if( FIX_MAXSTAT == 1 )
	WORD Str;	// 30
	WORD Dex;	// 32
	WORD Vit;	// 34
	WORD Energy;	// 36
#else
	short Str;	// 30
	short Dex;	// 32
	short Vit;	// 34
	short Energy;	// 36
#endif
	WORD Life;	// 38
	WORD MaxLife;	// 3A
	WORD Mana;	// 3C
	WORD MaxMana;	// 3E
	BYTE dbInventory[INVENTORY_BINARY_SIZE];	// 40
	BYTE dbMagicList[450];	// 700
	BYTE MapNumber;	// 7B4
	BYTE MapX;	// 7B5
	BYTE MapY;	// 7B6
	BYTE Dir;	// 7B7
	int PkCount;	// 7B8
	int PkLevel;	// 7BC
	int PkTime;	// 7C0
	BYTE CtlCode;	// 7C4
	BYTE DbVersion;	// 7C5
	BYTE AccountCtlCode;	// 7C6
	BYTE dbQuest[50];	// 7C7
	WORD Leadership;	// 7FA
	WORD ChatLitmitTime;	// 7FC
	int iFruitPoint;	// 800
	int Reset;
	short HardcoreLife;
#if (S6EP2==1 || EX700 ==1)
	BYTE btExInventory;
#endif
	DWORD Credits;
}SDHP_DBCHAR_INFORESULT, *LPSDHP_DBCHAR_INFORESULT;

typedef struct
{
	PBMSG_HEAD h;
	int aIndex;	// 4
	char Name[10];	// 8
	BYTE SkillKeyBuffer[20];	// 12
	BYTE GameOption;	// 1C
	BYTE QkeyDefine;	// 1D
	BYTE WkeyDefine;	// 1E
	BYTE EkeyDefine;	// 1F
	BYTE ChatWindow;	// 20
	BYTE RkeyDefine;
	DWORD QWERLevel;
}SDHP_SKILLKEYDATA_SEND, *LPSDHP_SKILLKEYDATA_SEND;

// 0x07

typedef struct
{
	PWMSG_HEAD h;	// C2:07
	char Name[11];	// 4
	short Level;	// 10
	BYTE Class;	// 12
	int LevelUpPoint;	// 14
	int Exp;	// 18
	int NextExp;	// 1C
	int Money;	// 20
#if( FIX_MAXSTAT == 1 )
	WORD Str;	// 24
	WORD Dex;	// 26
	WORD Vit;	// 28
	WORD Energy;	// 2A
#else
	short Str;	// 24
	short Dex;	// 26
	short Vit;	// 28
	short Energy;	// 2A
#endif
	WORD Life;	// 2C
	WORD MaxLife;	// 2E
	WORD Mana;	// 30
	WORD MaxMana;	// 32
	BYTE dbInventory[INVENTORY_BINARY_SIZE];	// 34
	BYTE dbMagicList[450];	// 6F4
	BYTE MapNumber;	// 7A8
	BYTE MapX;	// 7A9
	BYTE MapY;	// 7AA
	BYTE Dir;	// 7AB
	int PkCount;	// 7AC
	int PkLevel;	// 7B0
	int PkTime;	// 7B4
	BYTE dbQuest[50];	// 7B8
	BYTE CharInfoSave;	// 7EA
	WORD Leadership;	// 7EC
	WORD ChatLitmitTime;	// 7EE
	int iFruitPoint;	// 7F0
	int iReset;
	short HardcoreLife;
	DWORD Credits;
}SDHP_DBCHAR_INFOSAVE, *LPSDHP_DBCHAR_INFOSAVE;


// 0x08

typedef struct
{
	PWMSG_HEAD h;
	char AccountID[10];	// 4
	short aIndex;	// E
	int Money;	// 10
#if (S6EP2==1 || EX700==1)
	BYTE dbItems[3840];	// 14
#else
	BYTE dbItems[1920];	// 14
#endif
	BYTE DbVersion;	// 794
	short pw;	// 796
}SDHP_GETWAREHOUSEDB_SAVE, *LPSDHP_GETWAREHOUSEDB_SAVE;


typedef struct
{
	PBMSG_HEAD h;
	char AccountID[10];	// 3
	short aIndex;	// E
}SDHP_GETWAREHOUSEDB_RESULT, *LPSDHP_GETWAREHOUSEDB_RESULT, *LPSDHP_GETWAREHOUSEDB;

// 0x11

typedef struct
{
	PWMSG_HEAD h;	// C2:11
	char Name[10];	// 4
#if (S6EP2==1)
	BYTE dbInventory[3776];	// E
#else
#if (EX700==1)
	BYTE dbInventory[3792];	// E
#else
	BYTE dbInventory[1728];	// E
#endif
#endif
}SDHP_DBCHAR_ITEMSAVE, *LPSDHP_DBCHAR_ITEMSAVE;

// 0x52

typedef struct
{
	PBMSG_HEAD h;	// C1:XX
	BYTE x;	// 3
	BYTE y;	// 4
	BYTE MapNumber;	// 5 [235:Mu_2nd_Aniv], [236:CashShop]
	short Type;	// 6
	BYTE Level;	// 8
	BYTE Dur;	// 9
	BYTE Op1;	// A
	BYTE Op2;	// B
	BYTE Op3;	// C
	BYTE NewOption;	// D
	int aIndex;	// 10
	short lootindex;	// 14
	BYTE SetOption;	// 16
#if (EX700==1)
	BYTE SocketBonus;
	BYTE SocketOptions[5];
#endif
}SDHP_ITEMCREATE, *LPSDHP_ITEMCREATE;

typedef struct
{
	PBMSG_HEAD h;
	BYTE x;	// 3
	BYTE y;	// 4
	BYTE MapNumber;	// 5 [235:Mu_2nd_Aniv], [236:CashShop]
	DWORD m_Number;	// 8
	short Type;	// C
	BYTE Level;	// E
	BYTE Dur;	// F
	BYTE Op1;	// 10
	BYTE Op2;	// 11
	BYTE Op3;	// 12
	BYTE NewOption;	// 13
	int aIndex;	// 14
	short lootindex;	// 18
	BYTE SetOption;	// 1A
#if (EX700==1)
	BYTE SocketBonus;
	BYTE SocketOptions[5];
#endif
}SDHP_ITEMCREATERECV, *LPSDHP_ITEMCREATERECV;

// 0x56

typedef struct
{
	PWMSG_HEAD h;	// C2:56
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
}SDHP_REQUEST_PETITEM_INFO, *LPSDHP_REQUEST_PETITEM_INFO;




typedef struct 
{
	BYTE nPos;	// 0
	int nSerial;	// 4
	BYTE Level;	// 8
	int Exp;	// C
}Recv_PetItem_Info, *LPRecv_PetItem_Info;


typedef struct
{
	PWMSG_HEAD h;
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
}SDHP_RECV_PETITEM_INFO, *LPSDHP_RECV_PETITEM_INFO;

struct Request_PetItem_Info
{
	BYTE nPos;	// 0
	int nSerial;	// 4
};

typedef struct 
{
	int nSerial;	// 0
	BYTE Level;	// 3
	int Exp;	// 8
}Save_PetItem_Info, *LPSave_PetItem_Info;

// 0x57

typedef struct
{
	PWMSG_HEAD h;
	BYTE nCount;	// 4
}SDHP_SAVE_PETITEM_INFO, *LPSDHP_SAVE_PETITEM_INFO;

// 0x60

typedef struct
{
	PBMSG_HEAD h;	// C1:60
	char Name[MAX_IDSTRING];	// 3
	BYTE SkillKeyBuffer[20];	// D
	BYTE GameOption;	// 17
	BYTE QkeyDefine;	// 18
	BYTE WkeyDefine;	// 19
	BYTE EkeyDefine;	// 1A
	BYTE ChatWindow;	// 1B
	BYTE RkeyDefine;
	DWORD QWERLevel;
}SDHP_SKILLKEYDATA, *LPSDHP_SKILLKEYDATA;

// 0x80:01

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
}CSP_REQ_OWNERGUILDMASTER, *LPCSP_REQ_OWNERGUILDMASTER;

typedef struct
{
	PBMSG_HEAD2 h; // C1:80:1
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szCastleOwnGuild[0x8];	// 10
	char szCastleOwnGuildMaster[MAX_IDSTRING];	// 18
}CSP_ANS_OWNERGUILDMASTER, *LPCSP_ANS_OWNERGUILDMASTER;

// 0x80:03

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcDfLevel;	// 14
	int iNpcRgLevel;	// 18
	int iNpcMaxHp;	// 1C
	int iNpcHp;	// 20
	BYTE btNpcX;	// 24
	BYTE btNpcY;	// 25
	BYTE btNpcDIR;	// 26
	int iBuyCost;	// 28
}CSP_REQ_NPCBUY, *LPCSP_REQ_NPCBUY;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:03
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iNpcNumber;	// 10
	int iNpcIndex;	// 14
	int iBuyCost;	// 18
}CSP_ANS_NPCBUY, *LPCSP_ANS_NPCBUY;

// 0x80:04

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iRepairCost;	// 14
}CSP_REQ_NPCREPAIR, *LPCSP_REQ_NPCREPAIR;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:04
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iNpcNumber;	// 10
	int iNpcIndex;	// 14
	int iNpcMaxHp;	// 18
	int iNpcHpl;	// 1C
	int iRepairCost;	// 20
}CSP_ANS_NPCREPAIR, *LPCSP_ANS_NPCREPAIR;

// 0x80:05

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcUpType;	// 14
	int iNpcUpValue;	// 18
	int iNpcUpIndex;	// 1C
}CSP_REQ_NPCUPGRADE, *LPCSP_REQ_NPCUPGRADE;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:05
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iNpcNumber;	// 10
	int iNpcIndex;	// 14
	int iNpcUpType;	// 18
	int iNpcUpValue;	// 1C
	int iNpcUpIndex;	// 20
}CSP_ANS_NPCUPGRADE, *LPCSP_ANS_NPCUPGRADE;

// 0x80:06

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 3
	int iIndex;	// 8
}CSP_REQ_TAXINFO, *LPCSP_REQ_TAXINFO;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:06
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	__int64 i64CastleMoney;	// 10
	int iTaxRateChaos;	// 18
	int iTaxRateStore;	// 1C
	int iTaxHuntZone;	// 20
}CSP_ANS_TAXINFO, *LPCSP_ANS_TAXINFO;

// 0x80:07

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iTaxKind;	// C
	int iTaxRate;	// 10
}CSP_REQ_TAXRATECHANGE, *LPCSP_REQ_TAXRATECHANGE;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:07
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iTaxKind;	// 10
	int iTaxRate;	// 14
}CSP_ANS_TAXRATECHANGE, *LPCSP_ANS_TAXRATECHANGE;

// 0x80:09

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	WORD wStartYear;	// C
	BYTE btStartMonth;	// E
	BYTE btStartDay;	// F
	WORD wEndYear;	// 10
	BYTE btEndMonth;	// 12
	BYTE btEndDay;	// 13
}CSP_REQ_SDEDCHANGE, *LPCSP_REQ_SDEDCHANGE;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:09
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	unsigned short wStartYear;	// 10
	unsigned char btStartMonth;	// 12
	unsigned char btStartDay;	// 13
	unsigned short wEndYear;	// 14
	unsigned char btEndMonth;	// 16
	char btEndDay;	// 17
}CSP_ANS_SDEDCHANGE, *LPCSP_ANS_SDEDCHANGE;

// 0x80:0A

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
}CSP_REQ_GUILDREGINFO, *LPCSP_REQ_GUILDREGINFO;


typedef struct 
{
	PBMSG_HEAD2 h;	// C1:80:0A
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[0x8];	// 10
	int iRegMarkCount;	// 18
	unsigned char bIsGiveUp;	// 1C
	unsigned char btRegRank;	// 1D
}CSP_ANS_GUILDREGINFO, *LPCSP_ANS_GUILDREGINFO;

// 0x80:0B

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsSiegeEnded;	// 8
}CSP_REQ_SIEGEENDCHANGE, *LPCSP_REQ_SIEGEENDCHANGE;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:0B
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int bIsSiegeEnded;	// C
}CSP_ANS_SIEGEENDCHANGE, *LPCSP_ANS_SIEGEENDCHANGE;

// 0x80:0C

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsCastleOccupied;	// 8
	char szOwnerGuildName[8];	// C
}CSP_REQ_CASTLEOWNERCHANGE, *LPCSP_REQ_CASTLEOWNERCHANGE;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:0C
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int bIsCastleOccupied;	// C
	char szOwnerGuildName[0x8];	// 10
}CSP_ANS_CASTLEOWNERCHANGE, *LPCSP_ANS_CASTLEOWNERCHANGE;

// 0x80:0D

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szEnemyGuildName[8];	// C
}CSP_REQ_REGATTACKGUILD, *LPCSP_REQ_REGATTACKGUILD;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:0D
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szEnemyGuildName[0x8];	// 10
}CSP_ANS_REGATTACKGUILD, *LPCSP_ANS_REGATTACKGUILD;

// 0x80:0E

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
}CSP_REQ_CASTLESIEGEEND, *LPCSP_REQ_CASTLESIEGEEND;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:0E
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
}CSP_ANS_CASTLESIEGEEND, *LPCSP_ANS_CASTLESIEGEEND;

// 0x80:0F

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
}CSP_REQ_MAPSVRMULTICAST, *LPCSP_REQ_MAPSVRMULTICAST;

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
}CSP_ANS_MAPSVRMULTICAST, *LPCSP_ANS_MAPSVRMULTICAST;

// 0x80:10

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	int iItemPos;	// 14
}CSP_REQ_GUILDREGMARK, *LPCSP_REQ_GUILDREGMARK;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:10
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[0x8];	// 10
	int iItemPos;	// 18
	int iRegMarkCount;	// 1C
}CSP_ANS_GUILDREGMARK, *LPCSP_ANS_GUILDREGMARK;

// 0x80:11

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
}CSP_REQ_GUILDRESETMARK, *LPCSP_REQ_GUILDRESETMARK;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:11
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[0x8];	// 10
	int iRegMarkCount;	// 18
}CSP_ANS_GUILDRESETMARK, *LPCSP_ANS_GUILDRESETMARK;

// 0x80:12

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	BOOL bIsGiveUp;	// 14
}CSP_REQ_GUILDSETGIVEUP, *LPCSP_REQ_GUILDSETGIVEUP;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:12
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[0x8];	// 10
	int bIsGiveUp;	// 18
	int iRegMarkCount;	// 1C
}CSP_ANS_GUILDSETGIVEUP, *LPCSP_ANS_GUILDSETGIVEUP;

// 0x80:16

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iNpcNumber;	// 8
	int iNpcIndex;	// C
}CSP_REQ_NPCREMOVE, *LPCSP_REQ_NPCREMOVE;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:16
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
}CSP_ANS_NPCREMOVE, *LPCSP_ANS_NPCREMOVE;

// 0x80:17

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
}CSP_REQ_CASTLESTATESYNC, *LPCSP_REQ_CASTLESTATESYNC;

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
}CSP_ANS_CASTLESTATESYNC, *LPCSP_ANS_CASTLESTATESYNC;

// 0x80:18

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleTributeMoney;	// 8
}CSP_REQ_CASTLETRIBUTEMONEY,*LPCSP_REQ_CASTLETRIBUTEMONEY;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:08
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iMoneyChanged;	// 10
	__int64 i64CastleMoney;	// 18
} CSP_ANS_MONEYCHANGE,*LPCSP_ANS_MONEYCHANGE;

// 0x80:19

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
}CSP_REQ_RESETCASTLETAXINFO, *LPCSP_REQ_RESETCASTLETAXINFO;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:19
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
}CSP_ANS_RESETCASTLETAXINFO, *LPCSP_ANS_RESETCASTLETAXINFO;

// 0x80:1A

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
}CSP_REQ_RESETSIEGEGUILDINFO, *LPCSP_REQ_RESETSIEGEGUILDINFO;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:1A
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
}CSP_ANS_RESETSIEGEGUILDINFO, *LPCSP_ANS_RESETSIEGEGUILDINFO;

// 0x80:1B

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 2
}CSP_REQ_RESETREGSIEGEINFO, *LPCSP_REQ_RESETREGSIEGEINFO;

typedef struct
{
	PBMSG_HEAD2 h;	// C1:80:1B
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
}CSP_ANS_RESETREGSIEGEINFO, *LPCSP_ANS_RESETREGSIEGEINFO;

// 0x81

#define MAX_CS_NPC 200

typedef struct
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCastleEventCycle;	// 8
}CSP_REQ_CSINITDATA, *LPCSP_REQ_CSINITDATA;

typedef struct
{
	PWMSG_HEAD h;	// C2:81
	int iResult;	// 0x04
	unsigned short wMapSvrNum;	// 0x08
	unsigned short wStartYear;	// 0x0A
	unsigned char btStartMonth;	// 0x0C
	unsigned char btStartDay;	// 0x0D
	unsigned short wEndYear;	// 0x0E
	unsigned char btEndMonth;	// 0x10
	unsigned char btEndDay;		// 0x11
	unsigned char btIsSiegeGuildList;	// 0x12
	unsigned char btIsSiegeEnded;	// 0x13
	unsigned char btIsCastleOccupied; // 0x14
	char szCastleOwnGuild[0x8];	// 0x15
	__int64 i64CastleMoney;	// 0x20
	int iTaxRateChaos;	// 0x28
	int iTaxRateStore;	// 0x2C
	int iTaxHuntZone;	// 0x30
	int iFirstCreate;	// 0x34
	int iCount;	// 0x38
}CSP_ANS_CSINITDATA, *LPCSP_ANS_CSINITDATA;

typedef struct 
{
	int iNpcNumber;	// 0
	int iNpcIndex;	// 4
	int iNpcDfLevel;// 8
	int iNpcRgLevel;// C
	int iNpcMaxHp;	// 10
	int iNpcHp;	// 14
	unsigned char btNpcX;//18
	unsigned char btNpcY;//19
	unsigned char btNpcDIR;//1A
}CSP_CSINITDATA, *LPCSP_CSINITDATA;

// 0x83
#define MAX_CS_GUILDLIST 100
typedef struct
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
}CSP_REQ_ALLGUILDREGINFO, *LPCSP_REQ_ALLGUILDREGINFO;

typedef struct
{
	PWMSG_HEAD h;	// C2:83
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iIndex;	// C
	int iCount;	// 10
}CSP_ANS_ALLGUILDREGINFO, *LPCSP_ANS_ALLGUILDREGINFO;

typedef struct
{
	char szGuildName[0x8];	// 0
	int iRegMarkCount;	// 8
	unsigned char bIsGiveUp;	// C
	unsigned char btRegRank;	// D
}CSP_GUILDREGINFO, *LPCSP_GUILDREGINFO;

// 0x84

typedef struct
{
	PWMSG_HEAD h;	// C1:84
	unsigned short wMapSvrNum;	// 4
	int iCount;	// 8
}CSP_REQ_NPCSAVEDATA, *LPCSP_REQ_NPCSAVEDATA;

typedef struct
{
	int iNpcNumber;	// 0
	int iNpcIndex;	// 4
	int iNpcDfLevel;	// 8
	int iNpcRgLevel;	// C
	int iNpcMaxHp;	// 10
	int iNpcHp;	// 14
	unsigned char btNpcX;	// 18
	unsigned char btNpcY;	// 19
	unsigned char btNpcDIR;	// 1A
}CSP_NPCSAVEDATA, *LPCSP_NPCSAVEDATA;

typedef struct
{
	PBMSG_HEAD h;	// C1:84
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
}CSP_ANS_NPCSAVEDATA, *LPCSP_ANS_NPCSAVEDATA;

// 0x85

typedef struct
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
}CSP_REQ_CALCREGGUILDLIST, *LPCSP_REQ_CALCREGGUILDLIST;

typedef struct
{
	PWMSG_HEAD h;	// C1:85
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iCount;	// C
}CSP_ANS_CALCREGGUILDLIST, *LPCSP_ANS_CALCREGGUILDLIST;

typedef struct
{
	char szGuildName[0x8];	// 0
	int iRegMarkCount;	// 8
	int iGuildMemberCount;	// C
	int iGuildMasterLevel;	// 10
	int iSeqNum;	// 14
}CSP_CALCREGGUILDLIST, *LPCSP_CALCREGGUILDLIST;

// 0x86

typedef struct
{
	PWMSG_HEAD h;	// C2:86
	unsigned short wMapSvrNum;	// 4
	int iCount;	// 8
}CSP_REQ_CSGUILDUNIONINFO, *LPCSP_REQ_CSGUILDUNIONINFO;

typedef struct
{
	char szGuildName[0x8];	// 0
	int iCsGuildID;	// 8
}CSP_CSGUILDUNIONINFO, *LPCSP_CSGUILDUNIONINFO;

typedef struct
{
	PWMSG_HEAD h;	// C2:86
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iCount;	// C
}CSP_ANS_CSGUILDUNIONINFO, *LPCSP_ANS_CSGUILDUNIONINFO;

// 0x87

typedef struct
{
	PWMSG_HEAD h;	// C2:87
	unsigned short wMapSvrNum;	// 4
	int iCount;	// 8
}CSP_REQ_CSSAVETOTALGUILDINFO, *LPCSP_REQ_CSSAVETOTALGUILDINFO;

typedef struct
{
	char szGuildName[0x8];	// 0
	int iCsGuildID;	// 8
	int iGuildInvolved;	// C
}CSP_CSSAVETOTALGUILDINFO, *LPCSP_CSSAVETOTALGUILDINFO;

typedef struct
{
	PBMSG_HEAD h;	// C1:87
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
}CSP_ANS_CSSAVETOTALGUILDINFO, *LPCSP_ANS_CSSAVETOTALGUILDINFO;


// 0x88

typedef struct
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
}CSP_REQ_CSLOADTOTALGUILDINFO, *LPCSP_REQ_CSLOADTOTALGUILDINFO;

typedef struct
{
	PWMSG_HEAD h;	// C2:88
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int iCount;	// C
}CSP_ANS_CSLOADTOTALGUILDINFO, *LPCSP_ANS_CSLOADTOTALGUILDINFO;

typedef struct
{
	char szGuildName[0x8];	// 0

	int iCsGuildID;	// 8
	int iGuildInvolved;	// C
}CSP_CSLOADTOTALGUILDINFO, *LPCSP_CSLOADTOTALGUILDINFO;

// 0x89

typedef struct
{
	PWMSG_HEAD h;	// C2:89
	unsigned short wMapSvrNum;	// 4
	int iCount;	// 8
}CSP_REQ_NPCUPDATEDATA, *LPCSP_REQ_NPCUPDATEDATA;

typedef struct 
{
	PBMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
}CSP_ANS_NPCUPDATEDATA, *LPCSP_ANS_NPCUPDATEDATA;

typedef struct
{
	int iNpcNumber;	// 0
	int iNpcIndex;	// 4
	int iNpcDfLevel;	// 8
	int iNpcRgLevel;	// C
	int iNpcMaxHp;	// 10
	int iNpcHp;	// 14
	unsigned char btNpcX;	// 18
	unsigned char btNpcY;	// 19
	unsigned char btNpcDIR;	// 1A
}CSP_NPCUPDATEDATA, *LPCSP_NPCUPDATEDATA;

// 0xB0

typedef struct
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
}CWP_REQ_CRYWOLFSYNC, *LPCWP_REQ_CRYWOLFSYNC;

typedef struct
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
}CWP_ANS_CRYWOLFSYNC, *LPCWP_ANS_CRYWOLFSYNC;

// 0xB1

typedef struct
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
}CWP_REQ_CRYWOLFINFOLOAD, *LPCWP_REQ_CRYWOLFINFOLOAD;

typedef struct
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
}CWP_ANS_CRYWOLFINFOLOAD, *LPCWP_ANS_CRYWOLFINFOLOAD;

// 0xB2

typedef struct
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
}CWP_REQ_CRYWOLFINFOSAVE,*LPCWP_REQ_CRYWOLFINFOSAVE;

struct CWP_ANS_CRYWOLFINFOSAVE
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
};

// 0xD0:03

typedef struct 
{
	PBMSG_HEAD2 head;	// C1:D0:03
	int iUserIndex;	// 4
	int iUserGuid;	// 8
	int iItemCode;	// C
	int iItemPeriodDate;	// 10
	BYTE btItemEffectType1;	// 14
	BYTE btItemEffectType2;	// 15
	char chUserID[11];	// 16
	char chExpireDate[20];	//21
	BYTE btItemOption;
}PMSG_REQ_PERIODITEM_INSERT, *LPMSG_REQ_PERIODITEM_INSERT;

typedef struct
{
  PBMSG_HEAD2 head;
  int iUserIndex;
  int iItemCode;
  int iItemPeriodDate;
  unsigned char btItemEffectType1;
  unsigned char btItemEffectType2;
  unsigned char btResult;
}PMSG_ANS_PERIODITEM_INSERT, *LPMSG_ANS_PERIODITEM_INSERT;

// 0xD0:05

typedef struct 
{
  PBMSG_HEAD2 head;
  int iUserIndex;
  int iUserGuid;
  char chCharacterName[11];
}PMSG_REQ_PERIODITEM_LIST, *LPMSG_REQ_PERIODITEM_LIST;

#define SUBITEM_INFO_COUNT 32

typedef struct 
{
	BYTE btUsedItem;
	int iItemCode;
	BYTE btItemOption;
	BYTE btItemEffectType1;
	BYTE btItemEffectType2;
	int iItemPeriodUsedTime;
	int iItemPeriodLeftTime;
}PERIODITEM_LIST, *LPERIODITEM_LIST;

typedef struct 
{
	PWMSG_HEAD2 head;	// 0
	int iUserIndex;	// 4
	int iUserGuid;	// 8
	BYTE btCount;	// C
	char btResult;	// D
	//int test;
	PERIODITEM_LIST Items[SUBITEM_INFO_COUNT];
}PMSG_ANS_PERIODITEM_LIST, *LPMSG_ANS_PERIODITEM_LIST;

// 0xD0:07

typedef struct  {
  PBMSG_HEAD2 head;
  int iUserIndex;
  int iUserGuid;
  int iItemCode;
  int iUsedTime;
  int iLeftTime;
  char chCharacterName[11];
}PMSG_REQ_PERIODITEM_UPDATE, *LPMSG_REQ_PERIODITEM_UPDATE;

typedef struct{
  PBMSG_HEAD2 head;
  int iUserIndex;
  int iItemCode;
  unsigned char btResult;
}PMSG_ANS_PERIODITEM_UPDATE , *LPMSG_ANS_PERIODITEM_UPDATE ;

// 0xD0:09

typedef struct
{
  PBMSG_HEAD2 head;
  int iUserIndex;
  int iUserGuid;
  unsigned char btResult;
}PMSG_ANS_PERIODITEM_DELETE, *LPMSG_ANS_PERIODITEM_DELETE;

typedef struct 
{
  PBMSG_HEAD2 head;
  int iUserIndex;
  int iUserGuid;
  char chCharacterName[11];
}PMSG_REQ_PERIODITEM_DELETE, *LPMSG_REQ_PERIODITEM_DELETE;

typedef struct
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iMoneyChanged;	// C
}CSP_REQ_MONEYCHANGE, *LPCSP_REQ_MONEYCHANGE;

typedef struct
{
	PBMSG_HEAD h;
	BYTE result;
	int Index;
	char szName[MAX_IDSTRING+1];
	short MasterLevel;
	__int64 MasterExp;
	__int64 MasterNextExp;
	short MasterPoint;
}PMSG_ANS_MASTERINFO, *LPMSG_ANS_MASTEREINFO;

typedef struct
{
	PBMSG_HEAD h;
	int Index;
	char szName[11];
}PMSG_REQ_QUESTMONSTERKILLINFO, *LPMSG_REQ_QUESTMONSTERKILLINFO;

struct QUEST_MONSTERKILL_INFO
{
	int m_MonIndex;
	int m_KillCount;
};

typedef struct
{
	PBMSG_HEAD h;
	int Index;
	char szName[MAX_IDSTRING+1];
	int QuestIndex;
	QUEST_MONSTERKILL_INFO KillInfo[5];
}PMSG_ANS_MONSTERKILLINFO, *LPMSG_ANS_MONSTERKILLINFO;

typedef struct
{
	PBMSG_HEAD h;
	int Index;
	char szName[MAX_IDSTRING+1];
	int QuestIndex;
	QUEST_MONSTERKILL_INFO KillInfo[5];
}PMSG_SAVE_MONSTERKILLINFO, *LPMSG_SAVE_MONSTERKILLINFO;

typedef struct
{
	PBMSG_HEAD h;
	char szName[MAX_IDSTRING+1];
	WORD MasterLevel;
	__int64 MasterExp;
	__int64 MasterNextExp;
	WORD MasterPoints;
}PMSG_SAVE_MASTERINFO, *LPMSG_SAVE_MASTERINFO;

typedef struct
{
	PBMSG_HEAD h;
	char szAccount[MAX_IDSTRING];
	short Number;
	BYTE Result;
}PMSG_ANS_SUMMONER_CREATE;


struct CSHOP_SET_SUMMONER_CREATE
{
	PBMSG_HEAD h;
	char szId[11];
	WORD Index;
};


#pragma pack(push)
#pragma pack(1)
struct QUESTEX_USER
{
	BYTE	btStartQuest;
	BYTE	btCurQuest;
	BYTE	btState;
	WORD	btQuestInfo[5];
};
#pragma pack(pop)

struct PMSG_REQ_QUESTEXINFO_SAVE
{
	PWMSG_HEAD h;
	char szName[11];

	QUESTEX_USER	QuestInfo[20];
};

struct PMSG_REQ_QUESTEXINFO
{
	PBMSG_HEAD h;
	int aIndex;
	char szName[11];
};

struct PMSG_ANS_QUESTEXINFO
{
	PWMSG_HEAD h;
	int aIndex;
	char szName[11];

	BYTE btResult;
	QUESTEX_USER	QuestInfo[20];
};

struct PMSG_REQ_GENS_JOINREQUEST
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szRequestID[11];
	char szGuildMasterID[11];
	BYTE btGensType;
};

struct PMSG_ANS_GENS_JOINREQUEST
{
	PBMSG_HEAD2 h;
	int aIndex;
	BYTE btResult;
	BYTE btGensType;
	int iRank;
};

struct PMSG_REQ_GENS_INFO
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
};

struct PMSG_ANS_GENS_INFO
{
	PBMSG_HEAD2 h;
	int aIndex;
	int iRank;
	int iContribution;
	BYTE iReward;
	BYTE btResult;
	BYTE btGens;
};

struct PMSG_GENS_INFOSAVE
{
	PBMSG_HEAD2 h;
	char szName[11];
	BYTE iReward;
	int iContribution;
};

struct PMSG_REQ_LEAVE_GENS
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[11];
	BYTE btGens;
};

struct PMSG_ANS_LEAVE_GENS
{
	PBMSG_HEAD2 h;
	int aIndex;
	BYTE btResult;
};

struct PMSG_REQ_GENS_SERVERINFO
{
	PBMSG_HEAD2 h;
	BYTE btMapSvrCode;
	BYTE btUpdateDays;
	BYTE btPrizeDays;
};

struct PMSG_ANS_GENS_SERVERINFO
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btUpdateDays;
};

typedef struct
{
	PBMSG_HEAD h;
	BYTE btType;
	BYTE btCount;
	char szAccountID[11];
	char szName[11];
}CSHOP_SET_EXPANDINVENTORY, *LPCSHOP_SET_EXPANDINVENTORY;

typedef struct
{
	PWMSG_HEAD h;
	char szName[11];
	BYTE btDATA[257];
}MUBOT_SETTINGS_REQ_SAVE, *LPMUBOT_SETTINGS_REQ_SAVE;

typedef struct
{
	PWMSG_HEAD h;
	int aIndex;
	char szName[11];
	BYTE btDATA[257];
}MUBOT_SETTINGS_SEND, *LPMUBOT_SETTINGS_SEND;

typedef struct
{
	PBMSG_HEAD h;
	int aIndex;
	char szAccount[10];
	char szName[10];
}PMSG_REQ_ERTELLIST, *LPMSG_REQ_ERTELLIST;

typedef struct
{
	PWMSG_HEAD h;
	int aIndex;
	char szName[10];

	BYTE ErtelList1[700];
	BYTE ErtelList2[700];
}PMSG_ANS_ERTELLIST, *LPMSG_ANS_ERTELLIST;

typedef struct
{
	PWMSG_HEAD h;
	char szAccount[10];
	char szName[10];
	BYTE ErtelList1[700];
	BYTE ErtelList2[700];
}PMSG_SAVE_ERTELLIST, *LPMSG_SAVE_ERTELLIST;

struct AchievGDUpdateMonster
{
	PBMSG_HEAD2 Head;
	char	Name[11];
	short	MonsterID;
};

struct AchievGDUpdateEvent
{
	PBMSG_HEAD2 Head;
	char	Name[11];
	BYTE	EventType;
	int		BC_Exp;
	int		BC_ExtExp;
	int		BC_Score;
	int		DS_Exp;
	int		DS_Point;
	int		CC_UKill;
	int		CC_MKill;
	int		CC_TExp;
	int		CC_RExp;
};


struct AchievGDUpdateMix
{
	PBMSG_HEAD2 Head;
	char	Name[11];
	bool	Success;
};

#pragma pack(push, 1)
typedef struct
{
	PBMSG_HEAD2 h;
	int aIndex;
	char chAccountID[11];
	BYTE VaultID;

}PMSG_VAULT_INFO ,	*LPMSG_VAULT_INFO;
#pragma pack(pop)

// ----------------------------------------------------------------
struct PMSG_VAULT_RESULT
{
	PBMSG_HEAD2 h;
	int aIndex;
	BYTE Result;
};

#pragma pack(push, 1)
struct GD_HardcoreMode_Activate
{
	PBMSG_HEAD2 Header;
	BYTE		Status;
	short		LifeCount;
};
#pragma pack(pop)
// Protocol functions

void DataServerLogin(LPSDHP_SERVERINFO lpMsg,int aIndex);
void JGPGetCharList(LPSDHP_GETCHARLIST lpMsg,int aIndex);
void JGCharacterCreateRequest(LPSDHP_CREATECHAR lpMsg,int aIndex);
void JGCharDelRequest(LPSDHP_CHARDELETE lpMsg,int aIndex);
void JGGetCharacterInfo(LPSDHP_DBCHARINFOREQUEST lpMsg,int aIndex);
void GJSetCharacterInfo(LPSDHP_DBCHAR_INFOSAVE lpMsg,int aIndex);
void DGGetWarehouseList(LPSDHP_GETWAREHOUSEDB lpMsg,int aIndex);
void GDSetWarehouseList(LPSDHP_GETWAREHOUSEDB_SAVE lpMsg,int aIndex);
void GDUserItemSave(LPSDHP_DBCHAR_ITEMSAVE lpMsg,int aIndex);
void ItemSerialCreateRecv(LPSDHP_ITEMCREATE lpMsg,int aIndex);
void PetItemSerialCreateRecv(LPSDHP_ITEMCREATE lpMsg,int aIndex);
void DGRecvPetItemInfo(LPSDHP_REQUEST_PETITEM_INFO lpMsg,int aIndex);
void GDSavePetItemInfo(LPSDHP_SAVE_PETITEM_INFO lpMsg,int aIndex);
void DGOptionDataRecv(LPSDHP_SKILLKEYDATA lpMsg,int aIndex);
void DGAnsOwnerGuildMaster(LPCSP_REQ_OWNERGUILDMASTER lpMsg,int aIndex);
void DGAnsCastleNpcBuy(LPCSP_REQ_NPCBUY lpMsg,int aIndex);
void DGAnsCastleNpcRepair(LPCSP_REQ_NPCREPAIR lpMsg,int aIndex);
void DGAnsCastleNpcUpgrade(LPCSP_REQ_NPCUPGRADE lpMsg,int aIndex);
void DGAnsTaxInfo(LPCSP_REQ_TAXINFO lpMsg,int aIndex);
void DGAnsTaxRateChange(LPCSP_REQ_TAXRATECHANGE lpMsg,int aIndex);
void DGAnsSiegeDateChange(LPCSP_REQ_SDEDCHANGE lpMsg,int aIndex);
void DGAnsGuildMarkRegInfo(LPCSP_REQ_GUILDREGINFO lpMsg,int aIndex);
void DGAnsSiegeEndedChange(LPCSP_REQ_SIEGEENDCHANGE lpMsg,int aIndex);
void DGAnsCastleOwnerChange(LPCSP_REQ_CASTLEOWNERCHANGE lpMsg,int aIndex);
void DGAnsRegAttackGuild(LPCSP_REQ_REGATTACKGUILD lpMsg,int aIndex);
void DGAnsRestartCastleState(LPCSP_REQ_CASTLESIEGEEND lpMsg,int aIndex);
void DGAnsMapSvrMsgMultiCast(LPCSP_REQ_MAPSVRMULTICAST lpMsg,int aIndex);
void DGAnsRegGuildMark(LPCSP_REQ_GUILDREGMARK lpMsg,int aIndex);
void DGAnsGuildMarkReset(LPCSP_REQ_GUILDRESETMARK lpMsg,int aIndex);
void DGAnsGuildSetGiveUp(LPCSP_REQ_GUILDSETGIVEUP lpMsg,int aIndex);
void DGAnsNpcRemove(LPCSP_REQ_NPCREMOVE lpMsg, int aIndex);
void DGAnsCastleStateSync(LPCSP_REQ_CASTLESTATESYNC lpMsg, int aIndex);
void DGAnsCastleTributeMoney(LPCSP_REQ_CASTLETRIBUTEMONEY lpMsg,int aIndex);
void DGAnsResetCastleTaxInfo(LPCSP_REQ_RESETCASTLETAXINFO lpMsg,int aIndex);
void DGAnsResetSiegeGuildInfo(LPCSP_REQ_RESETSIEGEGUILDINFO lpMsg,int aIndex);
void DGAnsResetRegSiegeInfo(LPCSP_REQ_RESETREGSIEGEINFO lpMsg,int aIndex);
void DGAnsCastleInitData(LPCSP_REQ_CSINITDATA lpMsg,int aIndex);
void DGAnsAllGuildMarkRegInfo(LPCSP_REQ_ALLGUILDREGINFO lpMsg,int aIndex);
void DGAnsFirstCreateNPC(LPCSP_REQ_NPCSAVEDATA lpMsg,int aIndex);
void DGAnsCalcRegGuildList(LPCSP_REQ_CALCREGGUILDLIST lpMsg,int aIndex);
void DGAnsCsGulidUnionInfo(LPCSP_REQ_CSGUILDUNIONINFO lpMsg,int aIndex);
void DGAnsCsSaveTotalGuildInfo(LPCSP_REQ_CSSAVETOTALGUILDINFO lpMsg,int aIndex);
void DGAnsCsLoadTotalGuildInfo(LPCSP_REQ_CSLOADTOTALGUILDINFO lpMsg,int aIndex);
void DGAnsCastleNpcUpdate(LPCSP_REQ_NPCUPDATEDATA lpMsg,int aIndex);
void DGAnsCrywolfSync(LPCWP_REQ_CRYWOLFSYNC lpMsg,int aIndex);
void DGAnsCrywolfInfoLoad(LPCWP_REQ_CRYWOLFINFOLOAD lpMsg,int aIndex);
void DGAnsCrywolfInfoSave(LPCWP_REQ_CRYWOLFINFOSAVE lpMsg,int aIndex);
void DGAnsPeriodItemInsert(LPMSG_REQ_PERIODITEM_INSERT lpMsg,int aIndex);
void DGAnsPeriodItemList(LPMSG_REQ_PERIODITEM_LIST lpMsg, int aIndex);
void DGAnsPeriodItemUpdate(LPMSG_REQ_PERIODITEM_UPDATE lpMsg,int aIndex);
void DGAnsPeriodItemDelete(LPMSG_REQ_PERIODITEM_DELETE lpMsg,int aIndex);
void DGAnsCastleMoneyChange(LPCSP_REQ_MONEYCHANGE lpMsg,int aIndex);
void JGGetMasterInfo(LPSDHP_DBMASTERINFOREQUEST lpMsg,int aIndex);
void DGGetMonsterKillInfo(LPMSG_REQ_QUESTMONSTERKILLINFO lpMsg,int aIndex);
void DGSaveMonsterKillInfo(LPMSG_SAVE_MONSTERKILLINFO lpMsg,int aIndex);
void DGSaveMasterInfo(LPMSG_SAVE_MASTERINFO lpMsg,int aIndex);
void DGSummonerCreate(CSHOP_SET_SUMMONER_CREATE* lpMsg);

void DGSaveQuestExInfo(PMSG_REQ_QUESTEXINFO_SAVE* lpMsg,int aIndex);
void DGReqQuestExInfo(PMSG_REQ_QUESTEXINFO* lpMsg,int aIndex);

void DGRequestJoinGens(PMSG_REQ_GENS_JOINREQUEST* lpMsg,int aIndex);
void DGRequestGensInfo(PMSG_REQ_GENS_INFO* lpMsg,int aIndex);
void DGSaveGens(PMSG_GENS_INFOSAVE* lpMsg);
void DGRequestLeaveGens(PMSG_REQ_LEAVE_GENS* lpMsg,int aIndex);
void DGRequestServerInfo(PMSG_REQ_GENS_SERVERINFO* lpMsg,int aIndex);

void DGExpandRequest(LPCSHOP_SET_EXPANDINVENTORY lpMsg);
void DGMuBotOptionRecv(LPMUBOT_SETTINGS_REQ_SAVE lpMsg,int aIndex);

void DGAnsErtelList(LPMSG_REQ_ERTELLIST lpMsg,int aIndex);
void DGSaveErtelList(LPMSG_SAVE_ERTELLIST lpMsg);

void DBRecvVaultInfo(LPMSG_VAULT_INFO lpMsg,int aIndex);

void DGAchievUpdateMonsterData(AchievGDUpdateMonster* lpData, int aIndex);
void DGAchievUpdateEventData(AchievGDUpdateEvent* lpData, int aIndex);
void DGAchievUpdateMixData(AchievGDUpdateMix* lpData, int aIndex);

void GD_HardcoreModeActivate(GD_HardcoreMode_Activate* lpData, int aIndex);
#endif