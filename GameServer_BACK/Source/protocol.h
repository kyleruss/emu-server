// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef PROTOCOL_H
#define PROTOCOL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\include\prodef.h"
#include "GuildClass.h"
#include "user.h"
#include "MapClass.h"
#include "StdAfx.h"

extern int iCount;

#define MAX_BATTLE_LIST		11

#pragma pack(push, 1)
struct USER_GET_BATTLEINFO
{
	PBMSG_HEAD2	h;
	BYTE		BattleMapList[MAX_BATTLE_LIST];
	BYTE		BattleZoneList[MAX_BATTLE_LIST];
};
#pragma pack(pop)

struct USER_REQ_POST
{
		PBMSG_HEAD h;
		char id[10];
		char chatmsg[100];
};

struct PMSG_REQ_MASTERLEVEL_SKILL
{
	PBMSG_HEAD2 h;
	int AddSkill;
};

struct PMSG_DURATION_MAGIC_SEND 
{
	PBMSG_HEAD h;
	BYTE X;
	BYTE Y;
	BYTE Dir;
	BYTE MagicNumberH;
	BYTE NumberH;
	BYTE MagicNumberL;
	BYTE NumberL;
};

struct PMSG_KILLPLAYER_EXT
{
	PBMSG_HEAD h;	// C3:9C
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	WORD ExpH;	// 6
	WORD ExpL;	// 8
#if (CUSTOM_DAMAGE==1)
	WORD DamageHW;	// A
	WORD DamageLW;	// B
#else
	BYTE DamageH;	// A
	BYTE DamageL;	// B
#endif
};


struct PMSG_ANS_END_DUEL
{
	PBMSG_HEAD2 h;	// C1:AB
	BYTE bResult;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char szName[10];	// 5
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Send Warehouse Info Packet
 *	Direction : GameServer -> Client
 *  Code      : 0xC3
 *	HeadCode  : 0x30
 */
struct PMSG_TALKRESULT
{
	PBMSG_HEAD h;
	unsigned char result;	// 3
	BYTE level1;	// 4
	BYTE level2;	// 5
	BYTE level3;	// 6
	BYTE level4;	// 7
	BYTE level5;	// 8
	BYTE level6;	// 9
	BYTE level7;
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Result Packet
 *	Direction : GameServer -> Client
 *  Code     : 0xC1
 *	HeadCode : 0xF3
 *	SubCode  : 0x02
 */
struct PMSG_RESULT
{
	PBMSG_HEAD h;
	unsigned char subcode;	// 3
	unsigned char result;	// 4
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet to Send Blod Castle State
 *	Direction : GameServer -> Client
 *	Code      : 0xC1
 *	HeadCode  : 0x9B
 */
struct PMSG_STATEBLOODCASTLE
{
	PBMSG_HEAD h;	// C1:9B
	BYTE btPlayState;		// 3
	WORD wRemainSec;	// 4
	WORD wMaxKillMonster;	// 6
	WORD wCurKillMonster;	// 8
	WORD wUserHaveWeapon;	// A
	BYTE btWeaponNum;	// C
};


struct PMSG_KILLCOUNT
{
	PBMSG_HEAD2 h;
	BYTE btKillCount;
};



struct PMSG_ATTACKRESULT
{
	PBMSG_HEAD h;	// C1:DC
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE DamageH;	// 5
	BYTE DamageL;	// 6
	BYTE DamageType;	// 7
	BYTE DamageType2;	// 8
	BYTE btShieldDamageH;	// 9
	BYTE btShieldDamageL;	// A
#if (FIX_HP==1)
	int Damage;
	int ShieldDamage;
#endif
};

struct PMSG_ATTACKRESULT_ELEMENTAL
{
	PBMSG_HEAD h;	// C1:D8
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE btElement;	// 5
	DWORD dwDamage;	// 8
};


struct PMSG_RECV_POSISTION_SET
{
	PBMSG_HEAD h;	// C1:D6
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE X;	// 5
	BYTE Y;	// 6
};


struct PMSG_USEREQUIPMENTCHANGED
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE ItemInfo[MAX_ITEM_INFO];	// 5
};


struct PMSG_SERVERCMD
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	BYTE CmdType;	// 4
	BYTE X;	// 5
	BYTE Y;	// 6
};

struct PMSG_ITEMTHROW_RESULT
{
	PBMSG_HEAD h;	// C1:23
	unsigned char Result;	// 3
	BYTE Ipos;	// 4
};


struct PMSG_GUILDCREATED_RESULT
{
	PBMSG_HEAD h;	// C1:56
	BYTE Result;	// 3
	BYTE btGuildType;	// 4
};


struct PMSG_SIMPLE_GUILDVIEWPORT_COUNT
{
	PWMSG_HEAD h;	// C2:65
	BYTE Count;	// 4
};

struct PMSG_SIMPLE_GUILDVIEWPORT
{
	int GuildNumber;	// 0
	BYTE btGuildStatus;	// 4
	BYTE btGuildType;	// 5
	BYTE btGuildRelationShip;	// 6
	BYTE NumberH;	// 7
	BYTE NumberL;	// 8
	BYTE btCastleState;
};


struct PMSG_RECVMOVE
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE X;	// 5
	BYTE Y;	// 6
	BYTE Path;	// 7
};


struct PMSG_MAGICATTACK_RESULT
{
	PBMSG_HEAD h;	// C3:19
	BYTE SourceNumberH;
	BYTE SourceNumberL;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE TargetNumberH;
	BYTE TargetNumberL;
};


struct PMSG_BEATTACK_COUNT
{
	PBMSG_HEAD h;	// C1:D7
	BYTE MagicNumberH;
	BYTE Count;
	BYTE MagicNumberL;
	BYTE X;
	BYTE Serial;
	BYTE Y;
};


struct PMSG_BEATTACK
{
	BYTE NumberH;
	BYTE MagicKey;
	BYTE NumberL;
};



struct PMSG_SET_DEVILSQUARE
{
	PBMSG_HEAD h;	// C1:92
	BYTE Type;	// 3
};


struct PMSG_GUILD_ASSIGN_STATUS_RESULT
{
	PBMSG_HEAD h;	// C1:E1
	BYTE btType;	// 3
	BYTE btResult;	// 4
	char szTagetName[10];	// 5
};

struct PMSG_GUILD_ASSIGN_TYPE_RESULT
{
	PBMSG_HEAD h;	// C1:E2
	BYTE btGuildType;	// 3
	BYTE btResult;	// 4
};

//////////////////////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//////////////////////////////////////////////////////////////////////////////


struct PMSG_CLIENTTIME
{
	PBMSG_HEAD h;
	DWORD Time;	// 4
	WORD AttackSpeed;	// 8
	WORD MagicSpeed;	// A
};



struct PMSG_CHECK_MAINEXE_RESULT
{
	PBMSG_HEAD h;
	DWORD m_dwKey;	// 4
};





struct PMSG_CHATDATA
{
	PBMSG_HEAD h;	//	
	char chatid[10];	//	3
	char chatmsg[100];	//	D
};



struct PMSG_CHATDATA_NUMBER
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char chatmsg[100];	// 5
};


struct PMSG_CHATDATA_WHISPER
{
	PBMSG_HEAD h;	// C1:02
	char id[10];	// 3
	char chatmsg[100];	// D  
};



#if (__ENG__==1)
#pragma pack(push)
#pragma pack(1)

struct PMSG_IDPASS
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	char Id[10];	// 4
	char Pass[20];	// E
	DWORD TickCount;	// 18
	BYTE CliVersion[5];	// 1E
	BYTE CliSerial[16];	// 23  
};

#pragma pack(pop)
#else
#pragma pack(push)
#pragma pack(1)

struct PMSG_IDPASS
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	char Id[10];	// 4
	char Pass[12];	// E
	DWORD TickCount;	// 18
	BYTE CliVersion[5];	// 1E
	BYTE CliSerial[16];	// 23  
};

#pragma pack(pop)
#endif


struct PMSG_CLIENTCLOSE
{
	PBMSG_HEAD h;
	BYTE SubCode;	// 3
	BYTE Flag;	// 4
};



struct PMSG_CLIENTMSG
{
	PBMSG_HEAD h;
	BYTE SubCode;	// 3
	BYTE Flag;	// 4
	BYTE subFlag;	// 5
};



struct PMSG_CHARCREATE
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	char Name[10];	// 4
	BYTE ClassSkin;	// E
};



struct PMSG_CHARDELETE
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	char Name[10];	// 4
	char LastJoominNumber[20];	// E
};



struct PMSG_CHARMAPJOIN
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	char Name[10];	// 4
};



struct PMSG_LVPOINTADD
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	BYTE Type;	// 4
};


#pragma pack(push)
#pragma pack(1)

struct PMSG_SKILLKEY
{
	PBMSG_HEAD2 h;	// C1:F3:30
	BYTE SKillKey[20];	// 4
	BYTE GameOption;	// 18
	BYTE QkeyDefine;	// 19
	BYTE WkeyDefine;	// 1A
	BYTE EkeyDefine;	// 1B
	BYTE ChatWindow;	// 1C
	BYTE RkeyDefine;	// 1D
	DWORD QWERLevel;	// 1E
};

#pragma pack(pop)



struct PMSG_ITEMGETREQUEST
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};



struct PMSG_ITEMTHROW
{
	PBMSG_HEAD h;
	BYTE px;	// 3
	BYTE py;	// 4
	BYTE Ipos;	// 5
};



struct PMSG_INVENTORYITEMMOVE
{
	PBMSG_HEAD h;
	BYTE sFlag;	// 3
	BYTE source_item_num;	// 4
	BYTE sItemInfo[MAX_ITEM_INFO];	// 5
	BYTE tFlag;	// C
	BYTE target_item_num;	// D
};



struct PMSG_TALKREQUEST
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};



struct PMSG_BUYREQUEST
{
	PBMSG_HEAD h;
	BYTE Pos;	// 3
};



struct PMSG_SELLREQUEST
{
	PBMSG_HEAD h;
	BYTE Pos;	// 3
};



struct PMSG_ITEMDURREPAIR
{
	PBMSG_HEAD h;
	BYTE Position;	// [0xFF:Repair ALL] 3
	BYTE Requestpos;	// 4
};



struct PMSG_TRADE_REQUEST
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};


struct PMSG_TRADE_RESPONSE
{
	PBMSG_HEAD h;	// C1:37
	BYTE Response;	// 3
	BYTE Id[10];	// 4
	WORD Level;	// E
	int GuildNumber;	// 10
};



struct PMSG_TRADE_GOLD
{
	PBMSG_HEAD h;
	DWORD Money;	// 4
};



struct PMSG_TRADE_OKBUTTON
{
	PBMSG_HEAD h;	// C1:3C
	BYTE Flag;	// 3
};



struct PMSG_REQ_PSHOP_SETITEMPRICE
{
	PBMSG_HEAD2 h;
	BYTE btItemPos;		// 4
	BYTE sItemPrice1;	// 5
	BYTE sItemPrice2;	// 6
	BYTE sItemPrice3;	// 7
	BYTE sItemPrice4;	// 8
	BYTE sBlessCnt1;	// 9
	BYTE sBlessCnt2;	// A
	BYTE sSoulCnt1;		// B
	BYTE sSoulCnt2;		// C
	BYTE sChaosCnt1;	// D
	BYTE sChaosCnt2;	// E
};



struct PMSG_REQ_PSHOP_OPEN
{
	PBMSG_HEAD2 h;
	char szPShopText[36];	// 4
};



struct PMSG_REQ_BUYLIST_FROM_PSHOP
{
	PBMSG_HEAD2 h;
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE btName[10];	// 6
};

struct PMSG_REQ_BUYITEM_FROM_PSHOP
{
	PBMSG_HEAD2 h;
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE btName[10];	// 6
	BYTE btItemPos;	// 10
};


struct PMSG_REQ_PSHOPDEAL_CLOSE
{
	PBMSG_HEAD2 h;
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE btName[10];	// 6
};



struct PMSG_PARTYREQUEST
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};



struct PMSG_PARTYREQUESTRESULT
{
	PBMSG_HEAD h;
	BYTE Result;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
};



struct PMSG_PARTYDELUSER
{
	PBMSG_HEAD h;
	BYTE Number;	// 3
};



struct PMSG_GUILDJOINQ
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};



struct PMSG_GUILDQRESULT
{
	PBMSG_HEAD h;
	BYTE Result;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
};



struct PMSG_GUILDDELUSER
{
	PBMSG_HEAD h;
	char Name[10];	// 3
	char JoominNumber[20];	// D
};



struct PMSG_GUILDMASTERANSWER
{
	PBMSG_HEAD h;
	BYTE Result;	// 3
};



struct PMSG_GUILDINFOSAVE
{
	PBMSG_HEAD h;
	BYTE btGuildType;	// 3
	char GuildName[8];	// 4
	BYTE Mark[32];	// C
};



struct PMSG_GUILDWARSEND_RESULT
{
	PBMSG_HEAD h;
	BYTE Result;	// 3
};



struct PMSG_WAREHOUSEMONEYINOUT
{
	PBMSG_HEAD h;
	BYTE Type;	// [0x01:Deposit] [0x02:Withdraw] 3
	int Money;	// 4
};



struct PMSG_WAREHOUSEPASSSEND
{
	PBMSG_HEAD h;
	BYTE Type;	// [0x00:Open Warehouse] [0x01:Set Pass] [0x02:Delete Pass] 3
	short Pass;	// 4
	char LastJoominNumber[20];	// 6
};


struct PMSG_CHAOSMIX
{
	PBMSG_HEAD h;
	BYTE Type;	// 3
	BYTE Pos;
};




struct PMSG_MOVE
{
	PBMSG_HEAD h;	// C1:1D
	BYTE X;	// 3
	BYTE Y;	// 4
	BYTE Path[8];	// 5
};

struct PMSG_LVPOINTADDRESULT
{
	PBMSG_HEAD h;	// C1:F3:06
	BYTE subcode;	// 3
	BYTE ResultType;	// 4
	WORD MaxLifeAndMana;	// 6
	WORD wMaxShield;	// 8
	WORD MaxBP;	// A
#if (__CUSTOM__ == 1)
	WORD NewValue;
	WORD LvlUpPt;
#endif
#if (FIX_HP==1)
	int MAXHPANDMANA;
	int MAXSD;
#endif
};

struct PMSG_POSISTION_SET
{
	PBMSG_HEAD h;
	BYTE X;	// 3
	BYTE Y;	// 4
};



struct PMSG_ATTACK
{
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE AttackAction;
	BYTE DirDis;
};



struct PMSG_ACTION
{
	PBMSG_HEAD h;
	BYTE Dir;	// 3
	BYTE ActionNumber;	// 4
	BYTE iTargetIndexH;	// 5
	BYTE iTargetIndexL;	// 6
};



struct PMSG_MAGICATTACK
{
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE MagicNumberH;
	BYTE NumberL;
	BYTE MagicNumberL;
	BYTE Dis;
};



struct PMSG_MAGICCANCEL 
{
	PBMSG_HEAD h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE NumberH;
	BYTE NumberL;
};



struct PMSG_TELEPORT
{
	PBMSG_HEAD h;
	WORD MoveNumber;	// 3
	BYTE MapX;	// 4
	BYTE MapY;	// 5
};



struct PMSG_TARGET_TELEPORT
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE MapX;	// 5
	BYTE MapY;	// 6
};



struct PMSG_DURATION_MAGIC_RECV
{
	PBMSG_HEAD h;
	BYTE X;
	BYTE MagicNumberH;
	BYTE Y;
	BYTE MagicNumberL;
	BYTE Dir;
	BYTE NumberH;
	BYTE Dis;
	BYTE NumberL;
	BYTE TargetPos;
	BYTE MagicKey;
	BYTE btUnk;
};



struct PMSG_USEITEM
{
	PBMSG_HEAD h;
	BYTE inventoryPos;	// 3
	BYTE invenrotyTarget;	// 4
	BYTE btItemUseType;	// 5
};



struct PMSG_REQ_MOVEDEVILSQUARE
{
	PBMSG_HEAD h;
	BYTE SquareNumber;	// 3
	BYTE InvitationItemPos;	// 4
};



struct PMSG_REQ_DEVILSQUARE_REMAINTIME
{
	PBMSG_HEAD h;
	BYTE hEventType;	// 3
	BYTE btItemLevel;	// 4
};



struct PMSG_PING_RESULT
{
	PBMSG_HEAD h;
};



struct PMSG_REGEVENTCHIP
{
	PBMSG_HEAD h;
	BYTE Type;	// 3
	BYTE ChipPos;	// 4
};



struct PMSG_GETMUTONUMBER
{
	PBMSG_HEAD h;
};



struct PMSG_EXCHANGE_EVENTCHIP
{
	PBMSG_HEAD h;
	BYTE btType;	// [0x00:Change Renas] [0x01:Change Stones]	3
};

struct PMSG_SETQUEST 
{
	PBMSG_HEAD h;	// C1:A1
	BYTE QuestIndex;	// 3
	BYTE State;	// 4
};



struct PMSG_REQ_MOVEBLOODCASTLE
{
	PBMSG_HEAD h;
	BYTE iBridgeNumber;	// 3
	BYTE iItemPos;	// 4
};

struct PMSG_REQ_MOVECHAOSCASTLE
{
	PBMSG_HEAD2 h;
	BYTE iChaosCastleNumber;	// 4
	BYTE iItemPos;	// 5
};


struct PMSG_RESULT_MOVEBLOODCASTLE
{
	PBMSG_HEAD h;	// C1:9A
	BYTE Result;	// 3
};

struct PMSG_REQ_REPOSUSER_IN_CC
{
	PBMSG_HEAD2 h;
	BYTE btPX;	// 4
	BYTE btPY;	// 5
};

struct PMSG_REQ_MOVEILLUSIONTEMPLE
{
	PBMSG_HEAD2 h;
	BYTE iTempleNumber;	// 3
	BYTE iItemPos;	// 4
};

struct PMSG_REQ_CL_ENTERCOUNT
{
	PBMSG_HEAD h;
	BYTE btEventType;	// 3
};



struct PMSG_REQ_2ANV_LOTTO_EVENT
{
	PBMSG_HEAD h;
	char SERIAL1[5];	// 3
	char SERIAL2[5];	// 8
	char SERIAL3[5];	// D
};



struct PMSG_REQ_MOVE_OTHERSERVER
{
	PBMSG_HEAD h;
	char LastJoominNumber[20];	// 3
};



struct PMSG_PACKETCHECKSUM
{
	PBMSG_HEAD h;
	BYTE funcindex;	// 3
	DWORD CheckSum;	// 4
};


/*
struct _GG_AUTH_DATA
{
	unsigned long dwIndex;
	unsigned long dwValue1;
	unsigned long dwValue2;
	unsigned long dwValue3;
};
*/


struct PMSG_NPROTECTGGCHECKSUM
{
	PBMSG_HEAD h;	// C3:73
	_GG_AUTH_DATA m_ggadCheckSum;	// 4
};



struct PMSG_REQ_START_DUEL
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char szName[10];	// 5
};



struct PMSG_REQ_END_DUEL
{
	PBMSG_HEAD h;
};





struct PMSG_ANS_DUEL_OK
{
	PBMSG_HEAD h;
	bool bDuelOK;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	char szName[10];	// 6
};



struct PMSG_REQUEST_PET_ITEM_COMMAND
{
	PBMSG_HEAD h;
	BYTE PetType;	// 3
	BYTE Command;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
};



struct PMSG_REQUEST_PET_ITEMINFO
{
	PBMSG_HEAD h;
	BYTE PetType;	// 3
	BYTE InvenType;	// 4
	BYTE nPos;	// 5
};



struct PMSG_REQ_GUILDVIEWPORT
{
	PBMSG_HEAD h;
	int GuildNumber;	// 4
};



struct PMSG_GUILD_ASSIGN_STATUS_REQ
{
	PBMSG_HEAD h;
	BYTE btType;	// 3
	BYTE btGuildStatus;	// 4
	char szTagetName[10];	// 5
};



struct PMSG_GUILD_ASSIGN_TYPE_REQ
{
	PBMSG_HEAD h;
	BYTE btGuildType;	// 3
};



struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ
{
	PBMSG_HEAD h;	// C1:E5
	BYTE btRelationShipType;	// 3
	BYTE btRequestType;	// 4
	BYTE btTargetUserIndexH;	// 5
	BYTE btTargetUserIndexL;	// 6
	BYTE btUnk;
	BYTE szGuild[8];
};



struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS
{
	PBMSG_HEAD h;	// C1:E6
	BYTE btRelationShipType;	// 3
	BYTE btRequestType;	// 4
	BYTE btResult;	// 5
	BYTE btTargetUserIndexH;	// 6
	BYTE btTargetUserIndexL;	// 7
};



struct PMSG_UNIONLIST_REQ {

  

  
  PBMSG_HEAD h;

  

  

  

  

  
};


struct PMSG_KICKOUT_UNIONMEMBER_REQ
{
	PBMSG_HEAD2 h;
	char szTargetGuildName[8];	// 4
};



struct PMSG_REQ_MAPSERVERAUTH
{
	PBMSG_HEAD2 h;
	char szAccountID[12];	// 4
	char szCharName[12];	// 10
	int iJoinAuthCode1;	// 1C
	int iJoinAuthCode2;	// 20
	int iJoinAuthCode3;	// 24
	int iJoinAuthCode4;	// 28
	int iTickCount;	// 2C
	BYTE btCliVersion[5];	// 30
	BYTE btCliSerial[16];	// 35

};



struct PMSG_REQ_CASTLESIEGESTATE {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};



struct PMSG_REQ_REGCASTLESIEGE {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};



struct PMSG_REQ_GIVEUPCASTLESIEGE {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btGiveUp;

  

  

  

  

  
};



struct PMSG_REQ_GUILDREGINFO {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};


struct CSP_ANS_GUILDREGINFO
{
	PBMSG_HEAD2 h;
	int iResult;	// 4
	WORD wMapSvrNum;	// 8
	int iIndex;	// C
	char szGuildName[8];	// 10
	int iRegMarkCount;	// 18
	unsigned char bIsGiveUp;	// 1C
	BYTE btRegRank;	// 1D
};



struct PMSG_REQ_REGGUILDMARK {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btItemPos;

  

  

  

  

  
};

struct CSP_ANS_GUILDREGMARK {

  // static data ------------------------------------

  // non-static data --------------------------------
  /*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
  /*<thisrel this+0x8>*/ /*|0x2|*/ unsigned short wMapSvrNum;
  /*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
  /*<thisrel this+0x10>*/ /*|0x8|*/ char szGuildName[8];
  /*<thisrel this+0x18>*/ /*|0x4|*/ int iItemPos;
  /*<thisrel this+0x1c>*/ /*|0x4|*/ int iRegMarkCount;

  // base classes -----------------------------------

  // friends ----------------------------------------

  // static functions -------------------------------

  // non-virtual functions --------------------------

  // virtual functions ------------------------------
};
// <size 0x20>

struct PMSG_REQ_NPCBUY {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iNpcNumber;
  /*<thisrel this+0x8>*/ /*|0x4|*/ int iNpcIndex;

  

  

  

  

  
};



struct PMSG_REQ_NPCREPAIR {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iNpcNumber;
  /*<thisrel this+0x8>*/ /*|0x4|*/ int iNpcIndex;

  

  

  

  

  
};



struct PMSG_REQ_NPCUPGRADE {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iNpcNumber;
  /*<thisrel this+0x8>*/ /*|0x4|*/ int iNpcIndex;
  /*<thisrel this+0xc>*/ /*|0x4|*/ int iNpcUpType;
  /*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcUpValue;

  

  

  

  

  
};



struct PMSG_REQ_TAXMONEYINFO {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};



struct PMSG_REQ_TAXRATECHANGE {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btTaxType;
  /*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btTaxRate1;
  /*<thisrel this+0x6>*/ /*|0x1|*/ BYTE btTaxRate2;
  /*<thisrel this+0x7>*/ /*|0x1|*/ BYTE btTaxRate3;
  /*<thisrel this+0x8>*/ /*|0x1|*/ BYTE btTaxRate4;

  

  

  

  

  
};



struct PMSG_REQ_MONEYDRAWOUT {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btMoney1;
  /*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btMoney2;
  /*<thisrel this+0x6>*/ /*|0x1|*/ BYTE btMoney3;
  /*<thisrel this+0x7>*/ /*|0x1|*/ BYTE btMoney4;

  

  

  

  

  
};



struct PMSG_REQ_CSGATEOPERATE {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btOperate;
  /*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btIndex1;
  /*<thisrel this+0x6>*/ /*|0x1|*/ BYTE btIndex2;

  

  

  

  

  
};



struct PMSG_REQ_MINIMAPDATA {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};



struct PMSG_REQ_STOPMINIMAPDATA {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};



struct PMSG_REQ_CSCOMMAND {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btTeam;
  /*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btX;
  /*<thisrel this+0x6>*/ /*|0x1|*/ BYTE btY;
  /*<thisrel this+0x7>*/ /*|0x1|*/ BYTE btCommand;

  

  

  

  

  
};



struct PMSG_REQ_CSHUNTZONEENTER {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btHuntZoneEnter;

  

  

  

  

  
};



struct PMSG_REQ_NPCDBLIST {

  

  
  PBMSG_HEAD h;
  /*<thisrel this+0x3>*/ /*|0x1|*/ BYTE btMonsterCode;

  

  

  

  

  
};



struct PMSG_REQ_CSREGGUILDLIST {

  

  
  PBMSG_HEAD h;

  

  

  

  

  
};



struct PMSG_REQ_CSATTKGUILDLIST {

  

  
  PBMSG_HEAD h;

  

  

  

  

  
};



struct PMSG_REQ_USEWEAPON {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btObjIndexH;
  /*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btObjIndexL;
  /*<thisrel this+0x6>*/ /*|0x1|*/ BYTE btTargetIndex;

  

  

  

  

  
};



struct PMSG_REQ_WEAPON_DAMAGE_VALUE {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btObjIndexH;
  /*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btObjIndexL;

  

  

  

  

  
};



struct PMSG_REQ_GUILDMARK_OF_CASTLEOWNER {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};



struct PMSG_REQ_JEWEL_MIX {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btJewelType;
  /*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btJewelMix;

  

  

  

  

  
};


struct PMSG_REQ_JEWEL_UNMIX {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btJewelType;
  /*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btJewelLevel;
  /*<thisrel this+0x6>*/ /*|0x1|*/ BYTE btJewelPos;

  

  

  

  

  
};


struct PMSG_REQ_CRYWOLF_INFO {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};



struct PMSG_REQ_CRYWOLF_ALTAR_CONTRACT {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btObjIndexH;
  /*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btObjIndexL;

  

  

  

  

  
};



struct PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};


struct PMSG_REQ_KANTURU_STATE_INFO {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};



struct PMSG_REQ_ENTER_KANTURU_BOSS_MAP {

  

  
  /*<thisrel this+0x0>*/ /*|0x4|*/PBMSG_HEAD2 h;

  

  

  

  

  
};

struct PMSG_SENDEFFECT
{
	PBMSG_HEAD h;	// C1:2D
	WORD wOptionType;	// 4
	WORD wEffectType;	// 6
	BYTE byEffectOption;	// 8
	int iLeftTime;	// C
	BYTE iEffect;
	int iEffectValue;
};

#if (GS_CASTLE == 1)

struct CSP_ANS_CSINITDATA
{
	struct PWMSG_HEAD h;	// C2:81
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
};
// size 0x3C

struct CSP_CSINITDATA
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
};

struct CSP_CALCREGGUILDLIST
{
	char szGuildName[0x8];	// 0
	int iRegMarkCount;	// 8
	int iGuildMemberCount;	// C
	int iGuildMasterLevel;	// 10
	int iSeqNum;	// 14
};

struct CSP_CSGUILDUNIONINFO
{
	char szGuildName[0x8];	// 0
	int iCsGuildID;	// 8
};

struct CSP_CSLOADTOTALGUILDINFO
{
	char szGuildName[0x8];	// 0

	int iCsGuildID;	// 8
	int iGuildInvolved;	// C
};

struct PMSG_CSATTKGUILDLIST
{
	BYTE btCsJoinSide;
	BYTE btGuildInvolved;
	char szGuildName[10];
	int iScore;
};

struct PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE
{
	PBMSG_HEAD2 h;
	int iPrice;
};

struct PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

#endif

struct PMSG_ILLUSION_STATEINFO
{
	PBMSG_HEAD2 h;
	BYTE Type;
	BYTE State;
};

struct PMSG_ILLUSION_REQ_SKILLUSE
{
	PBMSG_HEAD2 h;
	BYTE MagicH;
	BYTE MagicL;
	BYTE tIndexH;
	BYTE tIndexL;
	BYTE State;
};

struct PMSG_ADD_CP
{
	PBMSG_HEAD2 h;
	int DBNumber;
	int iPoints;
	char Name[11];
};

// SEASON6 ADDITIONAL CHECK ...

struct PMSG_REQ_CHAR_PASSWORD
{
	PBMSG_HEAD2 h;
	char Name[10];
};

struct PMSG_ANS_CHAR_PASSWORD
{
	PBMSG_HEAD2 h;
	char Name[10];
	BYTE btResult;
};

struct PMSG_REQ_CHECK_PASSWORD
{
	PBMSG_HEAD2 h;
	char Name[10];
	char Password[10];
};

struct PMSG_ANS_CHECK_PASSWORD
{
	PBMSG_HEAD2 h;
	char Password[10];
	char Name[10];
	BYTE btResult;
};

struct PMSG_MOVE_REQUEST
{
	PBMSG_HEAD h;
	BYTE btType;
	DWORD CHECK;
	WORD wMoveNumber;
};

struct PMSG_MOVE_ANSWER
{
	PBMSG_HEAD h;
	BYTE btResult;
	DWORD CHECK;
};

// !END SEASON 6

void ProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen, int aIndex, BOOL Encrypt, int serial);
void TestSend();
void MsgSendV2(LPOBJ lpObj, unsigned char* Msg, int size);
void CGLiveClient(PMSG_CLIENTTIME* lpClientTime, short aIndex);
void GCCheckMainExeKeySend(int aIndex);
WORD EncryptCheckSumKey(WORD wSource);
void CGCheckMainRecv(PMSG_CHECK_MAINEXE_RESULT* lpMsg, int aIndex);
void PEchoProc(unsigned char* aMsg, int aLen, short aIndex);
void GCResultSend(int aIndex, unsigned char headcode, unsigned char result);
void ChatSend(LPOBJ lpObj, char* szChat);
void vChatSend(LPOBJ lpObj, char* szChat, ...);
void AllSendServerMsg(char* chatmsg);
void DataSendAll(unsigned char* lpMsg, int iMsgSize);
void ChatTargetSend(LPOBJ lpObj, char* szChat, int senduser);
void PChatProc(PMSG_CHATDATA* lpChat, short aIndex);
void CGChatRecv(PMSG_CHATDATA_NUMBER* lpMsg, int aIndex);
void GCServerMsgSend(unsigned char msg, int aIndex);
void GCServerMsgStringSend(char* szMsg, int aIndex, BYTE type);
void GCServerMsgStringSendGuild(_GUILD_INFO_STRUCT* lpNode, char* szMsg, unsigned char type);
void GCEventStateSend(int aIndex, unsigned char state, unsigned char event);
void GCMapEventStateSend(int map, unsigned char state, unsigned char event);
void CGChatWhisperRecv(PMSG_CHATDATA_WHISPER* lpMsg, int aIndex);
void SCPJoinResultSend(int aIndex, unsigned char result);
void CSPJoinIdPassRequest(PMSG_IDPASS* lpMsg, int aIndex);
void CSPJoinIdPassRequestTEST(PMSG_IDPASS* lpMsg, int aIndex);
void GCJoinBillCheckSend(char* AccountId, int aIndex);
void GCJoinResult(unsigned char result, int aIndex);
void GCJoinSocketResult(unsigned char result, SOCKET Socket);
void CGClientCloseMsg(PMSG_CLIENTCLOSE* lpMsg, int aIndex);
void GCCloseMsgSend(int aIndex, unsigned char result);
void CGClientMsg(PMSG_CLIENTMSG* lpMsg, int aIndex);
void CGPCharacterCreate(PMSG_CHARCREATE* lpMsg, int aIndex);
void CGPCharDel(PMSG_CHARDELETE* lpMsg, int aIndex);
void CGPCharacterMapJoinRequest(PMSG_CHARMAPJOIN* lpMsg, int aIndex);
void GCLevelUpMsgSend(int aIndex, int iSendEffect);
void CGLevelUpPointAdd(PMSG_LVPOINTADD* lpMsg, int aIndex);
void GCInventoryItemOneSend(int aIndex, int pos);
void GCPkLevelSend(int aIndex, unsigned char pklevel);
void GCMagicListOneSend(int aIndex, char Pos, WORD type, unsigned char level, unsigned char skill, BYTE btListType);
void GCMagicListOneDelSend(int aIndex, char Pos, WORD type, BYTE level, BYTE skill, BYTE btListType);
void GCMagicListMultiSend(LPOBJ lpObj, BYTE btListType);
void GCEquipmentSend(int aIndex);
void GCRecallMonLife(int aIndex, int maxlife, int life);
void GCTimeViewSend(int aIndex, int second);
void GCGoalSend(int aIndex, char* Name1, unsigned char score1, char* Name2, unsigned char score2);
void GCSkillKeyRecv(PMSG_SKILLKEY* lpMsg, int aIndex);
void GCSkillKeySend(int aIndex, LPBYTE keybuffer, BYTE GO, BYTE Qk, BYTE Wk, BYTE Ek, BYTE ChatWnd,BYTE Rk,DWORD QWERLevel);
void GCMoneySend(int aIndex, unsigned long money);
void GCItemInventoryPutSend(int aIndex, unsigned char result, unsigned char iteminfo1, unsigned char iteminfo2);
void CGItemGetRequest(PMSG_ITEMGETREQUEST* lpMsg, int aIndex);
BOOL CGItemDropRequest(PMSG_ITEMTHROW* lpMsg, int aIndex, int drop_type);
void GCItemMoveResultSend(int aIndex, unsigned char result, unsigned char pos, unsigned char* const ItemInfo);
void CGInventoryItemMove(PMSG_INVENTORYITEMMOVE* lpMsg, int aIndex);
void GCEquipmentChange(int aIndex, int pos);
void CGTalkRequestRecv(PMSG_TALKREQUEST* lpMsg, int aIndex);
void GCUserWarehouseSend(LPOBJ lpObj);
void CGBuyRequestRecv(PMSG_BUYREQUEST* lpMsg, int aIndex);
void CGSellRequestRecv(PMSG_SELLREQUEST* lpMsg, int aIndex);
int  GetNeedMoneyItemDurRepaire(class CItem* DurItem, int RequestPos);
void ItemDurRepaire(LPOBJ lpObj, class CItem* DurItem, int pos, int RequestPos);
void CGModifyRequestItem(PMSG_ITEMDURREPAIR* lpMsg, int aIndex);
void CGTradeRequestSend(PMSG_TRADE_REQUEST* lpMsg, int aIndex);
void CGTradeResponseRecv(PMSG_TRADE_RESPONSE* lpMsg, int aIndex);
BOOL GCTradeResponseSend(BYTE response, int aIndex, LPSTR id, WORD level, int GuildNumber);
int GCTradeOtherDel(int aIndex, BYTE tradeindex);
int GCTradeOtherAdd(int aIndex, BYTE tradeindex, LPBYTE iteminfo);
void CGTradeMoneyRecv(PMSG_TRADE_GOLD* lpMsg, int aIndex);
BOOL GCTradeMoneyOther(int aIndex, DWORD money);
BOOL GCTradeOkButtonSend(int aIndex, unsigned char flag);
void CGTradeOkButtonRecv(PMSG_TRADE_OKBUTTON* lpMsg, int aIndex);
void CGTradeCancelButtonRecv(int aIndex);
void CGTradeResult(int aIndex, unsigned char result);
void CGPShopReqSetItemPrice(PMSG_REQ_PSHOP_SETITEMPRICE* lpMsg, int aIndex);
void CGPShopAnsSetItemPrice(int aIndex, BYTE btResult, BYTE btItemPos);
void CGPShopReqOpen(PMSG_REQ_PSHOP_OPEN* lpMsg, int aIndex);
void CGPShopAnsOpen(int aIndex, BYTE btResult);
void CGPShopReqClose(int aIndex);
void CGPShopAnsClose(int aIndex, BYTE btResult);
void CGPShopReqBuyList(PMSG_REQ_BUYLIST_FROM_PSHOP* lpMsg, int aSourceIndex);
void CGPShopAnsBuyList(int aSourceIndex, int aTargetIndex, BYTE btResult, bool bResend);
void CGPShopReqBuyItem(PMSG_REQ_BUYITEM_FROM_PSHOP* lpMsg, int aSourceIndex);
void CGPShopAnsBuyItem(int aSourceIndex, int aTargetIndex, int iItemPos, BYTE btResult);
void CGPShopReqCloseDeal(PMSG_REQ_PSHOPDEAL_CLOSE* lpMsg, int aIndex);
void CGPShopAnsSoldItem(int aSourceIndex, int aTargetIndex, int iItemPos);
void CGPShopAnsDealerClosedShop(int aSourceIndex, int aTargetIndex);
void CGPartyRequestRecv(PMSG_PARTYREQUEST* lpMsg, int aIndex);
void CGPartyRequestResultRecv(PMSG_PARTYREQUESTRESULT* lpMsg, int aIndex);
void CGPartyList(int aIndex);
void CGPartyListAll(int pnumber);
void CGPartyDelUser(PMSG_PARTYDELUSER* lpMsg, int aIndex);
void CGPartyDelUserAsExitGameByForce(PMSG_PARTYDELUSER * lpMsg, int aIndex);
void GCPartyDelUserSend(int aIndex);
void CGGuildRequestRecv(PMSG_GUILDJOINQ* lpMsg, int aIndex);
void CGGuildRequestResultRecv(PMSG_GUILDQRESULT* lpMsg, int aIndex);
void CGGuildListAll(int pnumber);
void CGGuildDelUser(PMSG_GUILDDELUSER* lpMsg, int aIndex);
void GCGuildDelUserResult(int aIndex, unsigned char Result);
void GCGuildMasterQuestionSend(int aIndex);
void CGGuildMasterAnswerRecv(PMSG_GUILDMASTERANSWER* lpMsg, int aIndex);
void GCGuildMasterManagerRun(int aIndex);
void CGGuildMasterInfoSave(int aIndex,PMSG_GUILDINFOSAVE* lpMsg);
void CGGuildMasterCreateCancel(int aIndex);
void GCGuildViewportNowPaint(int aIndex, char* guildname, unsigned char* Mark, int isGuildMaster);
void GCGuildViewportDelNow(int aIndex, int isGuildMaster);
void GCManagerGuildWarEnd(char* GuildName);
void GCManagerGuildWarSet(char* GuildName1, char* GuildName2, int type);
void GCGuildWarRequestResult(char* GuildName, int aIndex, int type);
void GCGuildWarRequestSend(char* GuildName, int aIndex, int type);
void GCGuildWarRequestSendRecv(PMSG_GUILDWARSEND_RESULT* lpMsg, int aIndex);
void GCGuildWarDeclare(int aIndex, char* _guildname);
void GCGuildWarEnd(int aIndex, unsigned char result, char* _guildname);
void GCGuildWarScore(int aIndex);
void CGWarehouseMoneyInOut(int aIndex,PMSG_WAREHOUSEMONEYINOUT* lpMsg);
void GCWarehouseInventoryMoneySend(int aIndex, unsigned char result, int money, int wmoney);
void CGWarehouseUseEnd(int aIndex);
void GCWarehouseStateSend(int aIndex, unsigned char state);
void GCWarehouseRecivePassword(int aIndex,PMSG_WAREHOUSEPASSSEND* lpMsg);
void GCUserChaosBoxSend(LPOBJ lpObj, int iChaosBoxType);
void CGChaosBoxItemMixButtonClick(PMSG_CHAOSMIX* aRecv, int aIndex);
void CGChaosBoxUseEnd(int aIndex);
void PMoveProc(PMSG_MOVE* lpMove, int aIndex);
void RecvPositionSetProc(PMSG_POSISTION_SET* lpMove, int aIndex);
void CGAttack(PMSG_ATTACK* lpMsg, int aIndex);
void GCDamageSend(int aIndex, int damage, int iShieldDamage);
void GCDamageSend(int aIndex, int TargetIndex, int AttackDamage, int MSBFlag, int MSBDamage, int iShieldDamage);
void GCDamageSend(int aIndex, int TargetIndex, int AttackDamage, BYTE btAttribute);
void GCLifeSendAll(LPOBJ lpObj);
void GCKillPlayerExpSend(int aIndex, int TargetIndex, int exp, int AttackDamage, int MSBFlag);
void GCDiePlayerSend(LPOBJ lpObj, int TargetIndex, WORD skill, int KillerIndex);
void GCActionSend(LPOBJ lpObj, BYTE ActionNumber, int aIndex, int aTargetIndex);
void CGActionRecv(PMSG_ACTION* lpMsg, int aIndex);
void CGMagicAttack(PMSG_MAGICATTACK* lpMsg, int aIndex);
void GCMagicAttackNumberSend(LPOBJ lpObj, WORD MagicNumber, int usernumber, unsigned char skillsuccess);
void GCMagicCancelSend(LPOBJ lpObj, int MagicNumber);
void GCMagicCancel(PMSG_MAGICCANCEL * lpMsg, int aIndex);
void GCSendEffect(LPOBJ lpObj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime);
void CGMagicCancel(PMSG_MAGICCANCEL* lpMsg, int aIndex);
void GCUseMonsterSkillSend(LPOBJ lpObj, LPOBJ lpTargetObj, int iSkillNumber);
void GCStateInfoSend(LPOBJ lpObj, unsigned char state, BYTE ViewSkillState);
void CGTeleportRecv(PMSG_TELEPORT* lpMsg, int aIndex);
void CGTargetTeleportRecv(PMSG_TARGET_TELEPORT* lpMsg, int aIndex);
void GCTeleportSend(LPOBJ lpObj, int MoveNumber, BYTE MapNumber, BYTE MapX, BYTE MapY, BYTE Dir);
void CGBeattackRecv(unsigned char* lpRecv, int aIndex, int magic_send);
void CGDurationMagicRecv(PMSG_DURATION_MAGIC_RECV* lpMsg, int aIndex);
void ObjectMapJoinPositionSend(short aIndex);
void CGUseItemRecv(PMSG_USEITEM* lpMsg, int aIndex);
void GCReFillSend(int aIndex, WORD Life, BYTE Ipos, unsigned char flag,  WORD wShield);
void GCManaSend(int aIndex, short Mana, BYTE Ipos, unsigned char flag,  WORD BP);
void GCInventoryItemDeleteSend(int aIndex, BYTE pos, unsigned char flag);
void GCItemUseSpecialTimeSend(int aIndex, unsigned char number, int time);
void GCItemDurSend(int aIndex, BYTE pos, unsigned char dur, unsigned char flag);
void GCItemDurSend2(int aIndex, BYTE pos, unsigned char dur, unsigned char flag);
void CGWeatherSend(int aIndex, BYTE weather);
void GCServerCmd(int aIndex, BYTE type, unsigned char Cmd1, unsigned char Cmd2);
void GCReqmoveDevilSquare(PMSG_REQ_MOVEDEVILSQUARE* lpMsg, int aIndex);
void GCReqDevilSquareRemainTime(PMSG_REQ_DEVILSQUARE_REMAINTIME* lpMsg, int aIndex);
void AllSendMsg(unsigned char* Msg, int size);
void AllSendSameMapMsg(unsigned char* Msg, int size, unsigned char mapnumber);
void GCPingSendRecv(PMSG_PING_RESULT* aRecv, int aIndex);
void GCRegEventChipRecv(PMSG_REGEVENTCHIP* lpMsg, int aIndex);
void GCGetMutoNumRecv(PMSG_GETMUTONUMBER* lpMsg, int aIndex);
void GCUseEndEventChipRescv(int aIndex);
void GCUseRenaChangeZenRecv(PMSG_EXCHANGE_EVENTCHIP* lpMsg, int aIndex);
void CGRequestQuestInfo(int aIndex);
void GCSendQuestInfo(int aIndex, int QuestIndex);
void CGSetQuestState(PMSG_SETQUEST* lpMsg, int aIndex);
void GCSendQuestPrize(int aIndex, unsigned char Type, unsigned char Count);
void CGCloseWindow(int aIndex);
void CGRequestEnterBloodCastle(PMSG_REQ_MOVEBLOODCASTLE* lpMsg, int iIndex);
void CGRequestEnterChaosCastle(PMSG_REQ_MOVECHAOSCASTLE* lpMsg, int iIndex);
void CGRequestRepositionUserInChaosCastle(PMSG_REQ_REPOSUSER_IN_CC* lpMsg, int aIndex);
void CGRequestEventEnterCount(PMSG_REQ_CL_ENTERCOUNT* lpMsg, int aIndex);
void CGRequestLottoRegister(PMSG_REQ_2ANV_LOTTO_EVENT* lpMsg, int aIndex);
void CGReqMoveOtherServer(PMSG_REQ_MOVE_OTHERSERVER* lpMsg, int aIndex);
void GCPacketCheckSumRecv(PMSG_PACKETCHECKSUM* aRecv, int aIndex);
void GCNPggSendCheckSum(int aIndex,_GG_AUTH_DATA* pggAuthData);
void GCNPggCheckSumRecv(PMSG_NPROTECTGGCHECKSUM* lpMsg, int aIndex);
void CGDuelStartRequestRecv(PMSG_REQ_START_DUEL* lpMsg, int aIndex);
void CGDuelEndRequestRecv(PMSG_REQ_END_DUEL* lpMsg, int aIndex);
void CGDuelOkRequestRecv(PMSG_ANS_DUEL_OK* lpMsg, int aIndex);
//void GCSendDuelScore(int aIndex1, int aIndex2);
void GCSendGetItemInfoForParty(int aIndex, class CMapItem* lpItem);
void GCSendEffectInfo(int aIndex, BYTE btType);
void CGRequestPetItemCommand(PMSG_REQUEST_PET_ITEM_COMMAND* lpMsg, int aIndex);
void CGRequestPetItemInfo(PMSG_REQUEST_PET_ITEMINFO* lpMsg, int aIndex);
void GCGuildViewportInfo(PMSG_REQ_GUILDVIEWPORT* aRecv, int aIndex);
void CGGuildAssignStatus(PMSG_GUILD_ASSIGN_STATUS_REQ* aRecv, int aIndex);
void CGGuildAssignType(PMSG_GUILD_ASSIGN_TYPE_REQ* aRecv, int aIndex);
void CGRelationShipReqJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ* aRecv, int aIndex);
void CGRelationShipAnsJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS* aRecv, int aIndex);
void CGUnionList(PMSG_UNIONLIST_REQ* aRecv, int aIndex);
void CGRelationShipReqKickOutUnionMember(PMSG_KICKOUT_UNIONMEMBER_REQ* aRecv, int aIndex);
void CGReqMapSvrAuth(PMSG_REQ_MAPSERVERAUTH* lpMsg, int iIndex);
void GCAnsMapSvrAuth(int iIndex, int iResult);
void CGReqCastleSiegeState(PMSG_REQ_CASTLESIEGESTATE* lpMsg, int iIndex);
void GCAnsCastleSiegeState(int iIndex, int iResult, char* lpszGuildName, char* lpszGuildMaster);
void CGReqRegCastleSiege(PMSG_REQ_REGCASTLESIEGE* lpMsg, int iIndex);
void GCAnsRegCastleSiege(int iIndex, int iResult, char* lpszGuildName);
void CGReqGiveUpCastleSiege(PMSG_REQ_GIVEUPCASTLESIEGE* lpMsg, int iIndex);
void CGReqGuildRegInfo(PMSG_REQ_GUILDREGINFO* lpMsg, int iIndex);
void GCAnsGuildRegInfo(int iIndex, int iResult,CSP_ANS_GUILDREGINFO* lpMsgResult);
void CGReqRegGuildMark(PMSG_REQ_REGGUILDMARK* lpMsg, int iIndex);
void GCAnsRegGuildMark(int iIndex, int iResult,CSP_ANS_GUILDREGMARK* lpMsgResult);
void CGReqNpcBuy(PMSG_REQ_NPCBUY* lpMsg, int iIndex);
void GCAnsNpcBuy(int iIndex, int iResult, int iNpcNumber, int iNpcIndex);
void CGReqNpcRepair(PMSG_REQ_NPCREPAIR* lpMsg, int iIndex);
void GCAnsNpcRepair(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcHP, int iNpcMaxHP);
void CGReqNpcUpgrade(PMSG_REQ_NPCUPGRADE* lpMsg, int iIndex);
void GCAnsNpcUpgrade(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue);
void CGReqTaxMoneyInfo(PMSG_REQ_TAXMONEYINFO* lpMsg, int iIndex);
void GCAnsTaxMoneyInfo(int iIndex, int iResult, BYTE btTaxRateChaos, BYTE btTaxRateStore, __int64 i64Money);
void CGReqTaxRateChange(PMSG_REQ_TAXRATECHANGE* lpMsg, int iIndex);
void GCAnsTaxRateChange(int iIndex, int iResult, BYTE btTaxType, int iTaxRate);
void CGReqMoneyDrawOut(PMSG_REQ_MONEYDRAWOUT* lpMsg, int iIndex);
void GCAnsMoneyDrawOut(int iIndex, int iResult, __int64 i64Money);
void GCAnsCsGateState(int iIndex, int iResult, int iGateIndex);
void CGReqCsGateOperate(PMSG_REQ_CSGATEOPERATE* lpMsg, int iIndex);
void GCAnsCsGateOperate(int iIndex, int iResult, int iGateIndex, int iGateOperate);
void GCAnsCsGateCurState(int iIndex, int iGateIndex, int iGateOperate);
void GCAnsCsAccessSwitchState(int iIndex, int iSwitchIndex, int iSwitchUserIndex, BYTE btSwitchState);
void GCAnsCsAccessCrownState(int iIndex, BYTE btCrownState);
void GCAnsCsNotifyStart(int iIndex, BYTE btStartState);
void GCAnsCsNotifyProgress(int iIndex, BYTE btCastleSiegeState, char* lpszGuildName);
void GCAnsCsMapSvrTaxInfo(int iIndex, BYTE btTaxType, BYTE btTaxRate);
void CGReqCsMiniMapData(PMSG_REQ_MINIMAPDATA* lpMsg, int iIndex);
void GCAnsCsMiniMapData(int iIndex, BYTE btResult);
void CGReqStopCsMiniMapData(PMSG_REQ_STOPMINIMAPDATA* lpMsg, int iIndex);
void CGReqCsSendCommand(PMSG_REQ_CSCOMMAND* lpMsg, int iIndex);
void GCAnsCsSendCommand(int iCsJoinSide, BYTE btTeam, BYTE btX, BYTE btY, BYTE btCommand);
void GCAnsCsLeftTimeAlarm(BYTE btHour, BYTE btMinute);
void GCAnsSelfCsLeftTimeAlarm(int iIndex, BYTE btHour, BYTE btMinute);
void CGReqCsSetEnterHuntZone(PMSG_REQ_CSHUNTZONEENTER* lpMsg, int iIndex);
void GCAnsCsSetEnterHuntZone(int iIndex, BYTE btResult, BYTE btEnterHuntZone);
void CGReqNpcDbList(PMSG_REQ_NPCDBLIST* lpMsg, int iIndex);
void CGReqCsRegGuildList(PMSG_REQ_CSREGGUILDLIST* lpMsg, int iIndex);
void CGReqCsAttkGuildList(PMSG_REQ_CSATTKGUILDLIST* lpMsg, int iIndex);
void CGReqWeaponUse(PMSG_REQ_USEWEAPON* aRecv, int iIndex);
void CGReqWeaponDamageValue(PMSG_REQ_WEAPON_DAMAGE_VALUE* aRecv, int iIndex);
void CGReqGuildMarkOfCastleOwner(PMSG_REQ_GUILDMARK_OF_CASTLEOWNER* aRecv, int iIndex);
void CGReqJewelMix(PMSG_REQ_JEWEL_MIX* lpMsg, int iIndex);
void GCAnsJewelMix(int iIndex, int iResult);
void CGReqJewelUnMix(PMSG_REQ_JEWEL_UNMIX* lpMsg, int iIndex);
void GCAnsJewelUnMix(int iIndex, int iResult);
void CGReqCrywolfInfo(PMSG_REQ_CRYWOLF_INFO* lpMsg, int iIndex);
void GCAnsCrywolfInfo(int iIndex, BYTE btOccupationState, BYTE btCrywolfState);
void CGReqAlatrContract(PMSG_REQ_CRYWOLF_ALTAR_CONTRACT* lpMsg, int iIndex);
void CGReqPlusChaosRate(PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE* lpMsg, int iIndex);
void CGReqKanturuStateInfo(PMSG_REQ_KANTURU_STATE_INFO* lpMsg, int iIndex);
void GCReqEnterKanturuBossMap(PMSG_REQ_ENTER_KANTURU_BOSS_MAP* lpMsg, int iIndex);
void GCAnsGiveUpCastleSiege(int iIndex, int iResult, bool bGiveUp, int iMarkCount, LPSTR lpszGuildName);
void GCSendObjectCreationState(int iIndex);
void CGReqCastleHuntZoneEntrance(PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE * lpMsg,int aIndex);
void GCReqEnterBarracks(int aIndex);
void GCReqEnterBalgassLair(int aIndex);
void GCReqmoveIllusionTemple(PMSG_REQ_MOVEILLUSIONTEMPLE * lpMsg, int aIndex);
void GCIllusionSkillUseSend(PMSG_ILLUSION_REQ_SKILLUSE* lpMsg,int aIndex);
void GCIllusionSendPoint(int aIndex,BYTE Point);
void CGIllusionTempleReward(int aIndex);
void GCSendEffectInfo(LPOBJ lpObj,int bUseType,int iOptionType,int iEffectType,int iTimeValue);
void GCAttackSpeedSend(int aIndex);


void CGOpenPCBangShop(LPBYTE aRecv,int aIndex);
void CGBuyPCBangShop(LPBYTE aRecv,int aIndex);

struct PMSG_REQ_ENTERZONE
{
	PBMSG_HEAD2 h;
	BYTE btType;
};

BOOL PacketCheckTime(LPOBJ lpObj);
void CGReqmoveDoubleGoer(LPBYTE aRecv,int aIndex);
void CGReqEnterZone(PMSG_REQ_ENTERZONE* lpMsg,int aIndex);
void GCSendExp(int aIndex,int AttackIndex,__int64 Exp,int Damage,int Type);
void CGReqmoveLorenMarket(int aIndex);
void CGReqMoveAcheron(int aIndex);

#pragma pack(1)

struct PMSG_MONK_DARKSIDE_RECV
{
	PBMSG_HEAD h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_MONK_DARKSIDE_ANS
{
	PBMSG_HEAD2 h;
	WORD MagicNumber;
	WORD Target[5];
};

#pragma pack()


void CGWindowReqMove(PMSG_MOVE_REQUEST* lpMsg,int aIndex);

void GCMonkMagicAttack(PMSG_MAGICATTACK* lpMsg,int aIndex);
void GCMonkMagicAttackNumberSend(LPOBJ lpObj,int MagicNumber,int usernumber,int skillsuccess);
void GCMonkDarkSideTargetSelect(PMSG_MONK_DARKSIDE_RECV *lpMsg, int aIndex);


struct PMSG_MUBOT_SETTINGS_RECV
{
	PWMSG_HEAD h;
	BYTE btType;
	BYTE btDATA[257];
};


void CGSaveMuBotSettings(PMSG_MUBOT_SETTINGS_RECV* lpMsg,int aIndex);
void GCSendMuBotSettings(int aIndex,BYTE* lpData);


struct PMSG_MUBOT_REQ_START
{
	PBMSG_HEAD2 h;
	BYTE btType;
};

struct PMSG_MUBOT_USE
{
	PBMSG_HEAD2 h;
	WORD Time;
	DWORD Money;
	BYTE Status;
};

void CGRequestStartMuBot(PMSG_MUBOT_REQ_START* lpMsg,int aIndex);

#pragma pack(push)
#pragma pack(1)

struct PMSG_SETSTATS
{
	PBMSG_HEAD2 h;	// C1:F3:06
	WORD Str;
	WORD Dex;
	WORD Vit;
	WORD Ene;
	WORD Lea;
	int MaxLife;
	int MaxMana;
	int wMaxShield;	// 8
	WORD MaxBP;	// A
	WORD LvlUpPt;
};
#pragma pack(pop)

struct PMSG_REQ_USE_POLYMORPH_SKIN	//1.01.06
{
	PBMSG_HEAD2 h;
	char EnableUsePolymorphSkin;
};

void CGEnableUsePolymorphSkinrRecv(PMSG_REQ_USE_POLYMORPH_SKIN* lpMsg, int aIndex);	//1.01.06

// --------------------------------------------------------------------------------------------------
struct _tagPMSG_REQ_INVENTORY_EQUIPMENT_ITEM // 0x6
{
    struct PBMSG_HEAD2 h; // +0x0(0x4)
    BYTE btItemPos; // +0x4(0x1)
    BYTE btValue; // +0x5(0x1)
};
// --------------------------------------------------------------------------------------------------
struct _tagPMSG_ANS_INVENTORY_EQUIPMENT_ITEM // 0x6
{
    struct PBMSG_HEAD2 h; // +0x0(0x4)
    BYTE btItemPos; // +0x4(0x1)
    BYTE btResult; // +0x5(0x1)
};
// --------------------------------------------------------------------------------------------------

#pragma pack(push, 1)
struct ClientTest
{
	PBMSG_HEAD2 h;
	char Text[100];
	DWORD Value;
};
#pragma pack(pop)

void ClientTestSend(int UserIndex, ClientTest* lpResult);

void CGInventoryEquipment(_tagPMSG_REQ_INVENTORY_EQUIPMENT_ITEM * lpMsg, int iIndex); //S8 Emulator OK
#endif


#pragma pack(push, 1)
struct WebPacketRequestDisconnect
{
	PBMSG_HEAD2 h;
	char AccountName[10];
};
#pragma pack(pop)