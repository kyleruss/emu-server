#pragma once

#include "SkillDelay.h"
#include "ComboAttack.h"
#include "giocp.h"
#include "..\ggsvr\ggsvr.h"
#include "MagicInf.h"
#include "..\common\zzzitem.h"
#include "..\common\classdef.h"
#include "GuildClass.h"
#include "TDurMagicKeyChecker.h"
#include "TMonsterAIAgro.h"
#include "TMonsterSkillElementInfo.h"
#include "JewelOfHarmonySystem.h"
#include "ItemSystemFor380.h"
#include "SocketItemOption.h"
#include "protocol.h"
#include "PCBangUserSystem.h"
#if (ENABLETEST_MUUN == 1)
#include "MuunSystem.h"
#endif


#if (GS_SPEC == 0)
#if (GS_CASTLE == 1)

#define OBJMAX 4000
#define OBJMAXUSER 1500
#define OBJ_MAXMONSTER 1500

#else

#define OBJMAX 7400
#define OBJMAXUSER 1000
#define OBJ_MAXMONSTER 5800

#endif
#else

//-> 15~20
//#define OBJMAX 9000
//#define OBJMAXUSER 1000
//#define OBJ_MAXMONSTER 8000

//-> 15~25
//#define OBJMAX 10000
//#define OBJMAXUSER 1000
//#define OBJ_MAXMONSTER 9000

//-> 15~29
//#define OBJMAX			11000
//#define OBJMAXUSER		1000
//#define OBJ_MAXMONSTER	10000

//-> 17~30
#define OBJMAX			11500
#define OBJMAXUSER		1000
#define OBJ_MAXMONSTER	10500

/*
#define OBJMAX			12400
#define OBJMAXUSER		1000
#define OBJ_MAXMONSTER	10400*/
#endif

#define OBJ_STARTUSERINDEX (OBJMAX - OBJMAXUSER)
#define OBJ_CALLMONCOUNT (OBJ_STARTUSERINDEX - OBJ_MAXMONSTER)
#define MAX_MONSTER_SEND_MSG	20
#define MAX_MONSTER_SEND_ATTACK_MSG	50
#define MAX_MONSTER_TYPE 1024 //changed by season 7.2
#define MAX_GUILD_LEN 8
#define MAX_ACCOUNT_LEN 10
#define INVENTORY_SIZE 237
#define WAREHOUSE_SIZE 240
#define TRADE_BOX_SIZE 32
#define MAGIC_SIZE 150
#define INVETORY_WEAR_SIZE	12
#define INVENTORY_EXT_SIZE	32
#define INVENTORY_BAG_START INVETORY_WEAR_SIZE
#define MAIN_INVENTORY_SIZE 204	// Not included PersonalSh
#define INVENTORY_MAP_SIZE 224	// Not included weared iems
#define TRADE_BOX_MAP_SIZE 32
#define PSHOP_SIZE 32
#define PSHOP_MAP_SIZE 32
#define MAX_CHAR_LEVEL	400
#define MAX_SELF_DEFENSE 7
#define MAX_ST_HIT_DAMAGE 40
#define MAX_ZEN 2000000000
#if ( __CUSTOM__ == 1)
#define MAX_WAREHOUSE_ZEN	2000000000
#else
#define MAX_WAREHOUSE_ZEN	100000000
#endif
#define ACTIVE_ITEM_SEARCH 0
#define CHAOS_BOX_SIZE 32
#define CHAOS_BOX_MAP_SIZE 32
#define ELEMENT_SOCKET	236
#define MAX_IDSTRING	10

#define MAX_EFFECT_INFO	32

enum MAP_INDEX
{
	MAP_INDEX_RORENCIA = 0x0,
	MAP_INDEX_DUNGEON = 0x1,
	MAP_INDEX_DEVIAS = 0x2,
	MAP_INDEX_NORIA = 0x3,
	MAP_INDEX_LOSTTOWER = 0x4,
	MAP_INDEX_RESERVED = 0x5,
	MAP_INDEX_BATTLESOCCER = 0x6,
	MAP_INDEX_ATHLANSE = 0x7,
	MAP_INDEX_TARKAN = 0x8,
	MAP_INDEX_DEVILSQUARE = 0x9,
	MAP_INDEX_ICARUS = 0xa,
	MAP_INDEX_BLOODCASTLE1 = 0xb,
	MAP_INDEX_BLOODCASTLE2 = 0xc,
	MAP_INDEX_BLOODCASTLE3 = 0xd,
	MAP_INDEX_BLOODCASTLE4 = 0xe,
	MAP_INDEX_BLOODCASTLE5 = 0xf,
	MAP_INDEX_BLOODCASTLE6 = 0x10,
	MAP_INDEX_BLOODCASTLE7 = 0x11,
	MAP_INDEX_BLOODCASTLE8 = 0x34,
	MAP_INDEX_CHAOSCASTLE1 = 0x12,
	MAP_INDEX_CHAOSCASTLE2 = 0x13,
	MAP_INDEX_CHAOSCASTLE3 = 0x14,
	MAP_INDEX_CHAOSCASTLE4 = 0x15,
	MAP_INDEX_CHAOSCASTLE5 = 0x16,
	MAP_INDEX_CHAOSCASTLE6 = 0x17,
	MAP_INDEX_CHAOSCASTLE7 = 0x35,
	MAP_INDEX_KALIMA1 = 0x18,
	MAP_INDEX_KALIMA2 = 0x19,
	MAP_INDEX_KALIMA3 = 0x1a,
	MAP_INDEX_KALIMA4 = 0x1b,
	MAP_INDEX_KALIMA5 = 0x1c,
	MAP_INDEX_KALIMA6 = 0x1d,
	MAP_INDEX_CASTLESIEGE = 0x1e,
	MAP_INDEX_CASTLEHUNTZONE = 0x1f,
	MAP_INDEX_DEVILSQUARE2 = 0x20,
	MAP_INDEX_AIDA = 0x21,
	MAP_INDEX_CRYWOLF_FIRSTZONE = 0x22,
	MAP_INDEX_CRYWOLF_SECONDZONE = 0x23,
	MAP_INDEX_KALIMA7 = 0x24,
	MAP_INDEX_KANTURU1 = 0x25,
	MAP_INDEX_KANTURU2 = 0x26,
	MAP_INDEX_KANTURU_BOSS = 0x27,
	MAP_INDEX_GMMAP = 0x28,
	MAP_INDEX_BARRACKS = 0x29,
	MAP_INDEX_REFUGE = 0x2A,

	MAP_INDEX_ILLUSIONTEMPLE1 = 45,
	MAP_INDEX_ILLUSIONTEMPLE2 = 46,
	MAP_INDEX_ILLUSIONTEMPLE3 = 47,
	MAP_INDEX_ILLUSIONTEMPLE4 = 48,
	MAP_INDEX_ILLUSIONTEMPLE5 = 49,
	MAP_INDEX_ILLUSIONTEMPLE6 = 50,

	MAP_INDEX_ELBELAND = 0x33,
	MAP_INDEX_SWAMPOFCALMNESS = 0x38,
	MAP_INDEX_RAKLION = 0x39,
	MAP_INDEX_RAKLIONBOSS = 0x3A,
	MAP_INDEX_SANTAVILLAGE = 0x3E,

	MAP_INDEX_VULCANUS = 0x3F,
	MAP_INDEX_PKFIELD = 0x40,
	MAP_INDEX_DOUBLEGOER1 = 0x41,
	MAP_INDEX_DOUBLEGOER2 = 0x42,
	MAP_INDEX_DOUBLEGOER3 = 0x43,
	MAP_INDEX_DOUBLEGOER4 = 0x44,

	MAP_INDEX_IMPERIALFORT1 = 0x45,
	MAP_INDEX_IMPERIALFORT2 = 0x46,
	MAP_INDEX_IMPERIALFORT3 = 0x47,
	MAP_INDEX_IMPERIALFORT4 = 0x48,

	MAP_INDEX_LOREN_MARKET = 0x4F,
	MAP_INDEX_KARUTAN1 = 0x50,
	MAP_INDEX_KARUTAN2 = 0x51,

	MAP_INDEX_ACHERON = 91,
	MAP_INDEX_ACHERON_ARCABATTLE = 92,

	MAP_INDEX_DEBENTER = 95,
	MAP_INDEX_DEBENTER_ARCABATTLE = 96,

};

enum EXP_GETTING_EVENT_TYPE {
	EVENT_TYPE_NONE = 0x0,
	EVENT_TYPE_PARTY = 0x1,
	EVENT_TYPE_DEVILSQUARE = 0x2,
	EVENT_TYPE_CHAOSCASTLE = 0x3,
	EVENT_TYPE_BLOODCASTLE = 0x4,
	EVENT_TYPE_CRYWOLF = 0x5,
	EVENT_TYPE_ARCAWAR = 0x6,
};

#define MAX_RESISTENCE_TYPE 7

#define R_ICE			0
#define R_POISON		1
#define R_LIGHTNING		2
#define R_FIRE			3
#define R_EARTH			4
#define R_WIND			5
#define R_WATER			6


#define OBJ_EMPTY -1
#define OBJ_MONSTER 2
#define OBJ_USER 1
#define OBJ_NPC	3
#define MAX_PARTY_LEVEL_DIFFERENCE	130
#define MAX_MAGIC MAGIC_SIZE
#define MAX_VIEWPORT 75

#if (GS_CASTLE == 1)
#define MAX_VIEWPORT_MONSTER 75	
#else
#define MAX_VIEWPORT_MONSTER 20
#endif

#define MAX_ROAD_PATH_TABLE 16
#define MAX_ARRAY_FRUSTRUM 4
#define PLAYER_EMPTY  0
#define PLAYER_CONNECTED 1
#define PLAYER_LOGGED 2
#define PLAYER_PLAYING 3
#define MAX_CHECKSUM_KEY 1024
#define OBJMAX_RANGE(aIndex) ( ((aIndex) < 0 )?FALSE:( (aIndex) > OBJMAX-1 )?FALSE:TRUE   )
#define FRIEND_SERVER_STATE_LOGIN_FAIL	0
#define FRIEND_SERVER_STATE_OFFLINE		1
#define FRIEND_SERVER_STATE_ONLINE		2

#define MAIN_INVENTORY_RANGE(x) (((x)<0)?FALSE:((x)>MAIN_INVENTORY_SIZE-1)?FALSE:TRUE )
#define INVENTORY_RANGE(x) (((x)<0)?FALSE:((x)>INVENTORY_SIZE-1)?FALSE:TRUE )
#define WAREHOUSE_RANGE(x) (((x)<0)?FALSE:((x)>WAREHOUSE_SIZE-1)?FALSE:TRUE )
#define TRADE_BOX_RANGE(x) (((x)<0)?FALSE:((x)>TRADE_BOX_SIZE-1)?FALSE:TRUE )
#define OBJMON_RANGE(x) ( ((x)<0)?FALSE:((x)>OBJ_MAXMONSTER-1)?FALSE:TRUE)
#define PSHOP_RANGE(x) ( (((x)-MAIN_INVENTORY_SIZE)<0)?FALSE:(((x)-MAIN_INVENTORY_SIZE)>(INVENTORY_SIZE-MAIN_INVENTORY_SIZE-1))?FALSE:TRUE )
#define ATTRIBUTE_RANGE(x) (((x)<51)?FALSE:((x)>58)?FALSE:TRUE )

#define CURRENT_DB_VERSION	3

#define CHAR_SET_SIZE		18

	#define CS_CLASS		0

	#define CS_WEAPON1_TYPE	1
	#define CS_WEAPON2_TYPE	2

	#define CS_WEAPON1_DATA	12
	#define CS_WEAPON2_DATA	13

	#define CS_HELMET1		13
	#define CS_HELMET2		9
	#define CS_HELMET3		3

	#define CS_ARMOR1		14
	#define CS_ARMOR2		9
	#define CS_ARMOR3		3

	#define CS_PANTS1		14
	#define CS_PANTS2		9
	#define CS_PANTS3		4

	#define CS_GLOVES1		15
	#define CS_GLOVES2		9
	#define CS_GLOVES3		4

	#define CS_BOOTS1		15
	#define CS_BOOTS2		9
	#define CS_BOOTS3		5



#define CHECK_LIMIT(value, limit) ( ((value)<0)?FALSE:((value)>((limit)-1))?FALSE:TRUE  )
#define CHECK_CLASS(value, type) ( ((value)!=(type))?FALSE:TRUE )


#define CS_GET_CLASS(x) ( (((x)>>4)<<5)&(0xE0) )
#define CS_GET_CHANGEUP(x) ( ((x) & 0x07) )
#define CS_SET_CLASS(x) ( ((x)<<5)& 0xE0 )
#define CS_SET_CHANGEUP(x) ( ((x) << 4) & 0x10 )

#define CS_SET_HELMET1(x) ( ((x) & 0x1E0) >> 5 )
#define CS_SET_HELMET2(x) ( ((x) & 0x10 ) << 3 )
#define CS_SET_HELMET3(x) ( ((x) & 0x0F ) << 4 )

#define CS_SET_ARMOR1(x) ( ((x) & 0x1E0) >> 1 )
#define CS_SET_ARMOR2(x) ( ((x) & 0x10 ) << 2 )
#define CS_SET_ARMOR3(x) ( ((x) & 0x0F )      )

#define CS_SET_PANTS1(x) ( ((x) & 0x1E0) >> 5 )
#define CS_SET_PANTS2(x) ( ((x) & 0x10 ) << 1 )
#define CS_SET_PANTS3(x) ( ((x) & 0x0F ) << 4 )

#define CS_SET_GLOVES1(x) ( ((x) & 0x1E0) >> 1 )
#define CS_SET_GLOVES2(x) ( ((x) & 0x10 )      )
#define CS_SET_GLOVES3(x) ( ((x) & 0x0F )      )

#define CS_SET_BOOTS1(x) ( ((x) & 0x1E0) >> 5 )
#define CS_SET_BOOTS2(x) ( ((x) & 0x10 ) >> 1 )
#define CS_SET_BOOTS3(x) ( ((x) & 0x0F ) << 4 )

#define CS_SET_WING(x)  ( ((x) & 0x03 ) << 2 )
#define CS_SET_HELPER(x) ( ((x) & 0x03 )      )

#define CS_SET_SMALLLEVEL_RH(x)		( (x)       )
#define CS_SET_SMALLLEVEL_LH(x)		( (x) << 3  ) 
#define CS_SET_SMALLLEVEL_HELMET(x)	( (x) << 6  )
#define CS_SET_SMALLLEVEL_ARMOR(x)	( (x) << 9  )
#define CS_SET_SMALLLEVEL_PANTS(x)	( (x) << 12 )
#define CS_SET_SMALLLEVEL_GLOVES(x)	( (x) << 15 )
#define CS_SET_SMALLLEVEL_BOOTS(x)	( (x) << 18 )

#define CS_SET_SMALLLEVEL1(x)		( ((x) >> 16) & 0xFF )
#define CS_SET_SMALLLEVEL2(x)		( ((x) >> 8 ) & 0xFF )
#define CS_SET_SMALLLEVEL3(x)		((x) & 0xFF )





#include "ViewPortGuild.h"





typedef union tagPATHTABLE
{
	short sPathTable[16];
	char cPathTable[32];
} PATHTABLE, * LPPATHTABLE;



typedef struct tagActionState
{
	unsigned long Rest:1;	// 0
	unsigned long Attack:1;	// 1
	unsigned long Move:1;	// 2
	unsigned long Escape:1;	// 3
	unsigned long Emotion:4;	// 4
	unsigned long EmotionCount:8;	// 8

} ACTION_STATE, * LPACTION_STATE;



typedef struct tagInterfaceState
{
	DWORD use	: 2;
	DWORD state : 4;
	DWORD type	: 10;

} INTERFACE_STATE, * LPINTERFACE_STATE;

extern short RoadPathTable[MAX_ROAD_PATH_TABLE];
extern int  gServerMaxUser;	
extern BOOL g_EnergyCheckOff;

struct HITDAMAGE_STRUCT
{
	short number;	// 0
	int HitDamage;	// 4
	DWORD LastHitTime;	// 8
};

struct VIEWPORT_STRUCT
{
	char state;	// 0
	short number;	// 2
	unsigned char type;	// 4
	short index;	// 6
	int dis;	// 8
};

struct VIEWPORT_PLAYER_STRUCT
{
	char state;	// 0
	short number;	// 2
	unsigned char type;	// 4
	short index;	// 6
	int dis;	// 8

};


struct MessageStateMachine
{
	int MsgCode;	// 0
	int SendUser;	// 4
	DWORD MsgTime;	// 8
	int SubCode;	// C

};


struct ExMessageStateMachine
{
	int MsgCode;	// 0
	int SendUser;	// 4
	DWORD MsgTime;	// 8
	int SubCode;	// C
	int SubCode2;	// 10
};



typedef struct tagSEND_MONSTER_ATTACK_MENSSAGE
{
	int iMsgCode;	// unk0
	int iIndex;	// unk4
	DWORD dwDelay;	// unk8
	int iSubCode;	// unkC
	int iSubCode2;	// unk10
}SEND_MONSTER_ATTACK_MENSSAGE;

extern ExMessageStateMachine gSMAttackProcMsg[OBJMAX][MAX_MONSTER_SEND_ATTACK_MSG];
extern MessageStateMachine gSMMsg[OBJMAX][MAX_MONSTER_SEND_MSG];

#define MAX_EFFECT 100

struct EFFECT_INFO
{
	BYTE btEffectNum;
	BYTE btOptionType;
	BYTE btEffectType1;
	BYTE btEffectType2;
	BYTE btEffectType3;
	int iEffectValue1;
	int iEffectValue2;
	int iEffectValue3;
	DWORD dwTICK_COUNT;
	int iTimeValue;
};

struct QUEST_MONSTER_KILL_INFO
{
	int m_MonsterIndex;
	int m_KillCount;
};


#if ( __QUEST_SYSTEM__ == 1 )

#define MAX_QUESTEX_GROUP		20

#pragma pack(push)
#pragma pack(1)
struct QUESTEX_USER
{
	BYTE	btStartQuest;
	BYTE	btCurQuest;
	BYTE	btState;
	WORD	wQuestInfo[5];
};
#pragma pack(pop)

struct USER_QUESTEX_INFO
{
	BYTE btInit;
	QUESTEX_USER	Quest[MAX_QUESTEX_GROUP];
};

#endif

typedef struct _MASTERLEVEL_PASSIVE_SKILL_OPTION 
{
	float MpsAttackSuccessRate; // 00
	float MpsBlockingRate;		// 04
	float MpsPVPAttackDmgRate;	// 08
	float MpsPVPBlockingRate;	// 0C
	float MpsDownDur1;			// 10
	BYTE btMpsDownDur1Level;	// 14
	float MpsDownDur2;			// 18
	float MpsDownDur3;			// 1C
	float MpsResistancePoison;	// 20
	float MpsResistanceThunder;	// 24
	float MpsResistanceIce;		// 28
	float MpsAutoRecoverLife;	// 2C
	float MpsAutoRecoverMana;	// 30
	float MpsAutoRecoverAG;		// 34
	float MpsWeaponMastery;		// 38 dk, mg, dl
	float ukn_3C;				// 3C
	float MpsTwoHandSwordStrength; // 40
	float MpsOneHandSwordStrength; // 44
	float MpsMaceStrength;		// 48
	float MpsSpearStrength;		// 4C
	float MpsTwoHandSwordMastery;// 50
	float MpsOneHandSwordMastery;// 54
	float MpsIgnoreDef;				// 58
	float MpsSpearMastery;		// 5C
	float MpsMaceMastery;		// 60
	float MpsMaxMPMul;				// 64
	float MpsMagicMastery;		// 68 dw, mg
	float MpsTwoHandStaffStrength; // 6C
	float MpsOneHandStaffStrength; // 70
	float MpsShieldStrength;	// 74
	float MpsOneHandStaffMastery;// 78
	float MpsTwoHandStaffMastery;// 7C
	float MpsShieldMastery;		// 80
	float MpsBowStrength;		// 84
	float MpsBowMastery;		// 88
	float MpsCrossbowStrength;	// 8C
	float MpsCrossbowMastery;	// 90
	float MpsShieldStrength2;	// 94
	float MpsShieldMastery2;				// 98
	float MpsWeaponMastery2;	// 9C Elf Weapon Mastery
	float MpsSummonMonster1;	// A0
	float MpsSummonMonster2;	// A4
	float MpsSummonMonster3;	// A8
	float MpsMagicMastery2;		// AC summoner 
	float MpsFireTomeStrength;	// B0
	float MpsWindTomeStrength;	// B4
	float MpsLightTomeStrength;	// B8
	float MpsFireTomeMastery;	// BC
	float MpsWindTomeMastery;	// C0
	float MpsLightTomeMastery;	// C4
	float MpsStickStrength;		// C8
	float MpsTomeStrength;		// CC
	float MpsStickMastery;		// D0
	float MpsTomeMastery;		// D4
	float MpsBerserkInc1;		// D8
	float MpsBerserkInc2;		// DC
	float MpsBerserkInc3;		// E0
	float MpsMinWizCurseInc;	// E4
	float MpsDarkHorseStrength;	// E8
	float ukn_EC;				// EC
	float MpsDarkSpiritStr1;	// F0
	float MpsDarkSpiritStr2;	// F4
	float MpsDarkSpiritStr3;	// F8
	float MpsScepterStrength;	// FC
	float MpsScepterMastery;	// 100
	float MpsShieldStrength3;	// 104 dl
	float MpsShieldMastery3;	// 108 dl
	float MpsUseScepterPetStr;	// 10C dl
	float MpsCommandAttackInc;	// 110
	float MpsMinAttackInc;		// 114
	float MpsMaxAttackInc;		// 118
	float MpsExpWizardryCrit;	// 11C
	float MpsAddZen;			// 120
	float MpsDefence;			// 124
	float MpsMaxHP;				// 128
	float MpsMaxBP;				// 12C
	float MpsMaxMP;				// 130
	float MpsMonsterDieGetMana;	// 134
	float MpsMonsterDieGetLife;	// 138
	float MpsMonsterDieGetSD;	// 13C
	float MpsAddMLExp;				// 140
	float MpsMaxSD;				// 144
	float MpsSDSpeed;			// 148 auto rec
	float MpsMaxAttackDamage;	// 14C
	float MpsMinAttackDamage;	// 150
	float MpsDecreaseMana;		// 154
	float MpsMaxMagicDamage;	// 158
	float MpsMinMagicDamage;	// 15C
	float MpsPetDurDownSpeed;	// 160
	float MpsMgMaxMagicDamage;	// 164
	float MpsMgMinMagicDamage;	// 168
	float MpsFistStrength;		// 16C
	float MpsFistMastery;		// 170
	float MpsArmorSetBonusIncrease;	// 174
	float MpsReflectDamage;			// 178
	float MpsEnergyIncrease;		// 17C
	float MpsVitalityIncrease;		// 180
	float MpsDexterityIncrease;		// 184
	float MpsStrengthIncrease;		// 188
	float MpsLeadershipIncrease;	// 18C
	float MpsWingDefenseIncrease;	// 190
	float MpsWingAttackIncrease;	// 194
	float MpsComboAttackIncrease;	// 198
	float MpsCriticalRateIncrease;	// 19C
	float MpsExcelentRateIncrease;	// 1A0
	float MpsRestoreFullHP;			// 1A4
	float MpsRestoreFullMP;			// 1A8
	float MpsRestoreFullSD;			// 1AC
	float MpsDoubleDamageRate;		// 1B0
	float MpsSwellLifeAddBP;		// 1B4
	float MpsManaShieldAddMana;		// 1B8
	float MpsCritBuffExcDamage;		// 1BC
	float MpsDarkSpiritStr4;		// 1C0
	float MpsDarkSpiritStr5;		// 1C4
	float MpsDarkSpiritStr6;		// 1C8

}MASTERLEVEL_PASSIVE_SKILL_OPTION;
// <size 0x16C>

struct ERTEL_USER
{
	void Clear()
	{
		btType = -1;
		btAttribute = -1;

		for(int i = 0; i < 5; i++)	{
			Option[i] = 0x0F;
			Level[1] = 0x0F;
		}
	};

	BYTE btType;
	BYTE btAttribute;
	BYTE Option[5];
	BYTE Level[5];
};

struct ELEMENTAL_OPTIONS
{
	int m_AddFirePercDmg;
	int m_AddWaterPercDmg;
	int m_AddEarthPercDmg;
	int m_AddWindPercDmg;
	int m_AddDarkPercDmg;
	int m_AddDmgPVP;
	int m_AddDmgPVE;
	int m_AddRangedPVP;
	int m_AddMaleePVP;
	int m_AddCritRatePVP;
	int m_AddCritRatePVE;
	int m_AddFirePercDef;
	int m_AddWaterPercDef;
	int m_AddEarthPercDef;
	int m_AddWindPercDef;
	int m_AddDarkPercDef;
	int m_AddDefPVP;
	int m_AddDefPVE;
	int m_AddRangedDefPVP;
	int m_AddMaleeDefPVP;
	int m_DecreasePVP;
	int m_DecreasePVE;
	int m_AddAttackRate;
	int m_AddDefenseRate;
	int m_IncreasePVP;
	int m_IncreasePVE;
	int m_DebuffRate;
	int m_ExcRatePVP;
	int m_ExcRatePVE;
	int m_ElementalRate;
	int m_AddPentagramDamage;
	int m_AddPentagramDefense;
	int m_AddCritRate;
	int m_DefToElement;
	int m_AttToElement;
	int m_ResistStrongElement;
	int m_ResistElement;
};

struct OBJECTSTRUCT
{
//Share object start
	int		m_Index;
	int		Connected;
	short	m_AccountMaxLevel;
//Share object end
//User object start
	char	LoginMsgSnd;
	char	LoginMsgCount;
	char	CloseCount;
	char	CloseType;
	BOOL	bEnableDelCharacter;
	_PER_SOCKET_CONTEXT* PerSocketContext;
	DWORD	m_socket;
	char	Ip_addr[16];
	int		UserNumber;
	int		DBNumber;
	BYTE	Magumsa;
	DWORD	AutoSaveTime;
	DWORD	ConnectCheckTime;
	BYTE	CheckTickCount;
	BYTE	m_TimeCount;
	DWORD	m_dwPKTimer;
	short	CheckSumTableNum;
	DWORD	CheckSumTime;
//User object end
//Share object start
	BYTE	Type;
	BYTE	Live;
//Share object end
//User object start
	char	AccountID[11];
//User object end
//Share object start
	char	Name[11];
//Share object end
//User object start
	char	LastJoominNumber[20];
	bool	PlusStatQuestClear;
	bool	ComboSkillquestClear;
	ComboSkillData comboSkill;
//User object end
//Share object start
	WORD	Class;
//Share object end
//User object start
	BYTE	DbClass;
	BYTE	ChangeUP;
	BYTE	ChangeUP2;
//User object end
//Share object start
	short	Level;
//Share object end
//User object start
	int		LevelUpPoint;
	int		iFruitPoint;
	DWORD	Experience;
	DWORD	NextExp;
	short	MasterLevel;
	__int64 MasterExperience;
	__int64 MasterNextExp;
	int		MasterPoint;
	int		MasterPointSpent;
	int		bMasterInfoLoaded;
//User object end
//Share object start
	int		Money;
//Share object end
//User object start
#if( FIX_MAXSTAT == 1 )
	WORD	Strength;
	WORD	Dexterity;
	WORD	Vitality;
	WORD	Energy;
#else
	short	Strength;
	short	Dexterity;
	short	Vitality;
	short	Energy;
#endif
//User  object end
//Share object start
	float	Life;
	float	MaxLife;
//Share object end
//Monster object start
	int		m_iScriptMaxLife;
//Monster object end
//Share object start
	float	FillLife;
	float	FillLifeMax;
	float	Mana;
	float	MaxMana;
//Share object end
//User object start
	WORD	Leadership;
	WORD	AddLeadership;
	WORD	ChatLimitTime;
	BYTE	ChatLimitTimeSec;
	BYTE	FillLifeCount;
#if( FIX_MAXSTAT == 1 )
	WORD	AddStrength;
	WORD	AddDexterity;
	WORD	AddVitality;
	WORD	AddEnergy;
#else
	short	AddStrength;
	short	AddDexterity;
	short	AddVitality;
	short	AddEnergy;
#endif
	int		BP;
	int		MaxBP;
	int		AddBP;
	float	VitalityToLife;
	float	EnergyToMana;
	char	m_PK_Count;
	char	m_PK_Level;
	int		m_PK_Time;
//User object end
//Share object start
	BYTE	X;
	BYTE	Y;
	BYTE	Dir;
	BYTE	MapNumber;
	int		AddLife;
	int		AddMana;
//Share object end
//User object start
	int		iShield;
	int		iMaxShield;
	int		iAddShield;
	int		iFillShieldMax;
	int		iFillShield;
	int		iFillShieldCount;
	DWORD	dwShieldAutoRefillTimer;
	BYTE	DamageMinus;
	BYTE	DamageReflect;
	short	MonsterDieGetMoney;
	BYTE	MonsterDieGetLife;
	BYTE	MonsterDieGetMana;
//User object end
//Share object start
	BYTE	StartX;
	BYTE	StartY;
	BYTE	m_OldX;
	BYTE	m_OldY;
	BYTE	TX;
	BYTE	TY;
	BYTE	MTX;
	BYTE	MTY;
	int		PathCount;
	int		PathCur;
	char	PathStartEnd;
	short	PathOri[15];
	BYTE	PathX[15];
	BYTE	PathY[15];
	char	PathDir[15];
	DWORD	PathTime;
//Share object end
//User object start
	BYTE	m_MoveGateNumber;
	BYTE	DeathX;
	BYTE	DeathY;
	BYTE	DeathMap;
	BYTE	BindX;
	BYTE	BindY;
	BYTE	BindMap;
	DWORD	Authority;
	DWORD	Penalty;
	BYTE	m_cAccountItemBlock;
//User object end
//Monster object start
	ACTION_STATE m_ActState;
//Monster object end
//Share object start
	BYTE	m_ActionNumber;
	DWORD	m_State;
	BYTE	m_Rest;
	char	m_ViewState;
	BYTE	btEffectCount;
	EFFECT_INFO pEffectInfo[MAX_EFFECT_INFO];
	DWORD	m_LastMoveTime;
//Share object end
//User object start
	DWORD	m_LastAttackTime;
	BYTE	m_FriendServerOnline;
	int		m_DetectSpeedHackTime;
	DWORD	m_SumLastAttackTime;
	DWORD	m_DetectCount;
	int		m_DetectedHackKickCount;
	int		m_SpeedHackPenalty;
	BYTE	m_AttackSpeedHackDetectedCount;
	DWORD	m_PacketCheckTime;
	BYTE	m_ShopTime;
//User object end
//Share object start
	DWORD	TeleportTime;
	char	Teleport;
	char	KillerType;
	char	DieRegen;
	char	RegenOk;
	BYTE	RegenMapNumber;
	BYTE	RegenMapX;
	BYTE	RegenMapY;
	DWORD	RegenTime;
	DWORD	MaxRegenTime;
	short	m_PosNum;
	DWORD	CurActionTime;
	DWORD	NextActionTime;
	DWORD	DelayActionTime;
	char	DelayLevel;
//Share object end
//Monster object start
	char	m_iMonsterBattleDelay;
//Monster object end
//User object start
	char	m_cKalimaGateExist;
	int		m_iKalimaGateIndex;
	char	m_cKalimaGateEnterCount;
//User object end
//Share object start
	OBJECTSTRUCT* lpAttackObj;
//Share object end
//User object start
	short	m_SkillNumber;
	DWORD	m_SkillTime;
	bool	m_bAttackerKilled;
	char	m_ManaFillCount;
	char	m_LifeFillCount;
	int		SelfDefense[MAX_SELF_DEFENSE];
	DWORD	SelfDefenseTime[MAX_SELF_DEFENSE];
	DWORD	MySelfDefenseTime;
	char	m_Drink;
	int		m_SkillAttack2;
	char	m_SkillAttackTime2;
	int		PartyNumber;
	int		PartyTargetUser;
	int		GuildNumber;
	_GUILD_INFO_STRUCT* lpGuild;
	char	GuildName[11];
	int		GuildStatus;
	int		iGuildUnionTimeStamp;
	int		m_RecallMon;
//User object end
//Share object start
	int		m_Change;
	short	TargetNumber;
//Share object end
//Object user start
	short	TargetShopNumber;
//Object user end
//Object monster start
	short	ShopNumber;
//Object monster end
//Share object start
	short	LastAttackerID;
	int		m_AttackDamageMin;
	int		m_AttackDamageMax;
	int		m_MagicDamageMin;
	int		m_MagicDamageMax;
	int		m_AttackDamageLeft;
	int		m_AttackDamageRight;
//Share object end
//User object start
	int		m_AttackDamageMaxLeft;
	int		m_AttackDamageMinLeft;
	int		m_AttackDamageMaxRight;
	int		m_AttackDamageMinRight;
//User object end
//Share object start
	int		m_AttackRating;
	int		m_AttackSpeed;
	int		m_MagicSpeed;
	int		m_Defense;
	int		m_MagicDefense;
	int		m_SuccessfulBlocking;
//Share object end
//User object start
	int		m_CurseDamageBook;
	int		m_CurseDamageMin;
	int		m_CurseDamageMax;
//User object end
//Share object start
	short	m_MoveSpeed;
	short	m_MoveRange;
	short	m_AttackRange;
	short	m_AttackType;
	short	m_ViewRange;
	short	m_Attribute;
//Share object end
//Monster object start
	short	m_ItemRate;
	short	m_MoneyRate;
//Monster object end
//User object start
	int		m_CriticalDamage;
	int		m_ExcelentDamage;
#if (CUSTOM_EXOPT == 1)
	int		m_CriticalDamageMul;
	int		m_ExcelentDamageMul;
#endif
	CMagicInf* m_lpMagicBack;
//User object end
//Share object start
	CMagicInf* Magic;
//Share object end
//User object start
	char	MagicCount;
//User object end
//Share object start
	WORD	UseMagicNumber;
	DWORD	UseMagicTime;
	char	UseMagicCount;
//Share object end
//User object start
	short	OSAttackSerial; //!
	MASTERLEVEL_PASSIVE_SKILL_OPTION m_MPSkillOpt;
//User object end
//Share object start
	char	CharSet[18];
	char	m_Resistance[MAX_RESISTENCE_TYPE];
//Share object end
//User object start
	char	m_AddResistance[MAX_RESISTENCE_TYPE];
//User object end
//Share object start
	int		FrustrumX[MAX_ARRAY_FRUSTRUM];
	int		FrustrumY[MAX_ARRAY_FRUSTRUM];
	VIEWPORT_STRUCT VpPlayer[MAX_VIEWPORT];
	VIEWPORT_PLAYER_STRUCT VpPlayer2[MAX_VIEWPORT];
	int		VPCount;
	int		VPCount2;
//Share object end
//Monster object start
	HITDAMAGE_STRUCT sHD[MAX_ST_HIT_DAMAGE];
	short	sHDCount;
//Monster object end
//User object start
	tagInterfaceState m_IfState;
	DWORD	m_InterfaceTime;
//User object end
//Share object start
	CItem*	pInventory;
	char*	pInventoryCount;
//Share object end
//User object start
	LPBYTE	pInventoryMap;
	char	pTransaction;
	BYTE	pInventoryExtend;
	CItem*	Inventory1;
	LPBYTE  InventoryMap1;
	char	InventoryCount1;
	CItem*	Inventory2;
	LPBYTE InventoryMap2;
	char	InventoryCount2;
	CItem*	Trade;	
	LPBYTE	TradeMap;
	int		TradeMoney;
	bool	TradeOk;
	CItem*	pWarehouse;
	LPBYTE	pWarehouseMap;
	BYTE	pWarehouseExtend;
	char	WarehouseCount;
	short	WarehousePW;
	BYTE	WarehouseLock;
	int		WarehouseMoney;
	int		WarehouseSave;
	CItem*	pChaosBox;
	LPBYTE	pChaosBoxMap;
	int		ChaosMoney;
	int		ChaosSuccessRate;
	BOOL	ChaosLock;
	DWORD	m_Option;
	int		m_nEventScore;
	int		m_nEventExp;
	int		m_nEventMoney;
	BYTE	m_bDevilSquareIndex;
	BYTE	m_bDevilSquareAuth;
	char	m_cBloodCastleIndex;
	char	m_cBloodCastleSubIndex;
	int		m_iBloodCastleEXP;
	bool	m_bBloodCastleComplete;
	char	m_cChaosCastleIndex;
	char	m_cChaosCastleSubIndex;
	int		m_iChaosCastleBlowTime;
	char	m_cKillUserCount;
	char	m_cKillMonsterCount;
	int		m_iDuelUserReserved;
	int		m_iDuelUserRequested;
	int		m_iDuelUser;
	BYTE	m_btDuelScore;
	int		m_iDuelTickCount;
	bool	m_bPShopOpen;
	bool	m_bPShopTransaction;
	bool	m_bPShopItemChange;
	bool	m_bPShopRedrawAbs;
	char	m_szPShopText[36];
	bool	m_bPShopWantDeal;
	int		m_iPShopDealerIndex;
	char	m_szPShopDealerName[10];
	CRITICAL_SECTION m_critPShopTrade;
	int		m_iVpPShopPlayer[MAX_VIEWPORT];
	WORD	m_wVpPShopPlayerCount;
	bool	IsInBattleGround;
	short	EventChipCount;
	int		MutoNumber;
	BOOL	UseEventServer;
	bool	LoadWareHouseInfo;
	int		iStoneCount;
	BOOL	m_MonsterKillInfoLoaded;
	BOOL	m_MonsterKillQuestIndex;
	QUEST_MONSTER_KILL_INFO m_MonsterKill[5];
	BYTE	m_Quest[50];
	bool	m_SendQuestInfo;
//User object end
//Monster object start
	int		m_SkyBossMonSheildLinkIndex;
	int		m_SkyBossMonSheild;
	int		m_SkyBossMonSheildTime;
//Monster object end
//Share object start
	int		m_CheckLifeTime;
//Share object end
//User object start
	BYTE	m_MoveOtherServer;
	char	m_BackName[11];
//User object end
//Monster object start
	char	m_BossGoldDerconMapNumber;
//Monster object end
//User object start
	BYTE	m_InWebzen;
	char	m_LastTeleportTime;
	BYTE	m_ClientHackLogCount;
//User object end
//Monster object start
	BOOL	m_bIsInMonsterHerd;
	BOOL	m_bIsMonsterAttackFirst;
	class MonsterHerd* m_lpMonsterHerd;
//Monster object end
//User object start
	DWORD	m_NPggCheckSumSendTime;
//User object end
//Share object start
	int		fSkillFrustrumX[MAX_ARRAY_FRUSTRUM];
	int		fSkillFrustrumY[MAX_ARRAY_FRUSTRUM];
	BYTE	SkillHellFire2State;
	BYTE	SkillHellFire2Count;
	DWORD	SkillHellFire2Time;
//Share object end
//User object start
	BYTE	m_ReqWarehouseOpen;
	short	SetOpAddSkillAttack;
	short	SetOpAddExDamage;
	short	SetOpAddExDamageSuccessRate;
	short	SetOpAddCriticalDamage;
	short	SetOpAddCriticalDamageSuccessRate;
	short	SetOpIncAGValue;
	short	SetOpAddDamage;
	short	SetOpAddMinAttackDamage;
	short	SetOpAddMaxAttackDamage;
	short	SetOpAddAttackDamage;
	short	SetOpAddDefence;
	short	SetOpAddDefenceRate;
	short	SetOpAddMagicPower;
	BYTE	SetOpIgnoreDefense;
	BYTE	SetOpDoubleDamage;
	BYTE	SetOpTwoHandSwordImproveDamage;
	BYTE	SetOpImproveSuccessAttackRate;
	BYTE	SetOpReflectionDamage;
	BYTE	SetOpImproveSheldDefence;
	BYTE	SetOpDecreaseAG;
	BYTE	SetOpImproveItemDropRate;
	bool	IsFullSetItem;
	TDurMagicKeyChecker DurMagicKeyChecker;
	WORD	SkillRecallParty_Time;
	BYTE	SkillRecallParty_MapNumber;
	BYTE	SkillRecallParty_X;
	BYTE	SkillRecallParty_Y;
	bool	bIsChaosMixCompleted;
	bool	SkillLongSpearChange;
//User object end
//Share object 
	CSkillDelay SkillDelay;
//Share object end
//Monster object start
	int		iObjectSecTimer;
//Monster object end
//User object start
	bool	m_bMapSvrMoveQuit;
	bool	m_bMapSvrMoveReq;
	DWORD	m_dwMapSvrQuitTick;
	short	m_sPrevMapSvrCode;
	short	m_sDestMapNumber;
	BYTE	m_btDestX;
	BYTE	m_btDestY;
//User object end
#if (GS_CASTLE == 1)
//Monster object start
	union
	{
		struct
		{
			BYTE m_btCsNpcExistVal1;
			BYTE m_btCsNpcExistVal2;
			BYTE m_btCsNpcExistVal3;
			BYTE m_btCsNpcExistVal4;
		};
		int m_iCsNpcExistVal;
	};
	BYTE	m_btCsNpcType;
	BYTE	m_btCsGateOpen;
	int		m_iCsGateLeverLinkIndex;
	BYTE	m_btCsNpcDfLevel;
	BYTE	m_btCsNpcRgLevel;
//Monster object end
//User object start
	BYTE	m_btCsJoinSide;
	BYTE	m_bCsGuildInvolved;
#endif
	bool	m_bIsCastleNPCUpgradeCompleted;
//User object end
//Monster object start
	BYTE	m_btWeaponState;
	int		m_iWeaponUser;
//Monster object end
//User object start
	BYTE	m_btKillCount;
//User object end
//Share object start
	int		m_iAccumulatedDamage;
//Share object end
#if (GS_CASTLE == 1)
//User object start
	BYTE	m_btLifeStoneCount;
//User object end
//Monster object start
	BYTE	m_btCreationState;
	int		m_iCreatedActivationTime;
//Monster object end
#endif
//User object start
	int		m_iAccumulatedCrownAccessTime;
//User object end
//Monster object start
	TMonsterSkillElementInfo m_MonsterSkillElementInfo;
	int		m_iBasicAI;
	int		m_iCurrentAI;
	int		m_iCurrentAIState;
	DWORD	m_iLastAIRunTime;	
	int		m_iGroupNumber;
	int		m_iSubGroupNumber;
	int		m_iGroupMemberGuid;
	int		m_iRegenType;
	TMonsterAIAgro m_Agro;
	DWORD	m_iLastAutomataRuntime;
	DWORD	m_iLastAutomataDelay;
//Monster object end
//User object start
	int		m_iCrywolfMVPScore;
//User object end
//Share object start
	DWORD	m_dwLastCheckTick;
//Share object end
//User object start
	int		m_iAutoRecuperationTime;
	int		m_iSkillDistanceErrorCount;
	DWORD	m_dwSkillDistanceErrorTick;
	JEWELOFHARMONY_ITEM_EFFECT m_JewelOfHarmonyEffect;
	ITEMOPTION_FOR380ITEM_EFFECT m_ItemOptionExFor380;
	WORD	m_wItemEffectType;
	int		m_iItemEffectValidTime;
	int		m_wcCashPoint;
	int		m_wpCashPoint;
	int		m_wgCashPoint;
	DWORD	GoblinPointTick;
	int		iTempleIndex;
	bool	iOptionRecv;
	int		m_iPeriodItemEffectIndex;
	BYTE	m_iSummonerCreateFlag;
	union
	{
		struct
		{
			_SOCKET_USER_DATA m_SocketOption[35];
			_SOCKET_USER_DATA m_SocketBonus[7];
			_SOCKET_USER_DATA m_SocketSet[2];
		};
		_SOCKET_USER_DATA m_SocketOptions[44];
	};
	WORD	m_wSocketAddLife;
	WORD	m_wSocketAddMana;
	WORD	m_wSocketMonsterDieLife;
	WORD	m_wSocketMonsterDieMana;
	BYTE	m_btSocketBPUseDec;
	int		m_iDuelRoom;
	// New struct Added By lee
	BYTE	m_btSculptPos;
	BYTE	m_btCharmPos;
	BYTE	m_btArtifactPos;
#if (GS_CASTLE == 0 || GS_SPEC == 1)
	BOOL	m_bKanturuEntranceByNPC;
#endif
#if (__QUEST_SYSTEM__ == 1)
	USER_QUESTEX_INFO m_QuestEx;
#endif
#if (__GENS_SYSTEM__ == 1)
	GENS_USER m_GensInfo;
#endif
	int		m_ImperialGuardianZone;
	float	m_InfinityArrowDmgMul;
	char	m_btDarkSideTargetCount;
	short	m_wDarkSideTarget[5];
	BYTE	m_bIsMaintainParty;
	BYTE	m_bIsPartyMatch;
	BYTE	m_bIsPartyWaiting;
	int		m_bMUBOT;
	int		m_iMUBOT_STAGE;
	int		m_iMUBOT_TIME;
	ERTEL_USER* pErtelList1;
	ERTEL_USER* pErtelList2;
//User object end
//Share object start
	BYTE	m_btAttribute;
	int		m_ElementalDamageMin;
	int		m_ElementalDamageMax;
	int		m_ElementalDefense;
	int		m_ElementalAttackRate;
	int		m_ElementalDefenseRate;
	ELEMENTAL_OPTIONS* m_ElementalOptions; //Maybe better only for user
	BYTE	m_bOreUsed;
	int		m_iMiningLevel;
	int		m_iMiningJewelId;
	int		m_iMiningObjectId;
//Share object end
//User object strt
//bool	m_bOpenMinimap;
	
	bool	m_bSendNpcPos;
	bool	m_bSendMemberPos;

	char	m_EnableUseChangeSkin;
	DWORD	m_ShopPointExTime;
	DWORD	m_ShopPointExTimeBackup;
	BYTE	RecvSendWare;
#if (__CUSTOM__ == 1)
	int		iResetCount;
	BYTE	m_ResetWait;
	DWORD	m_ResetWaitTick;
#if (ANTI_CHEAT==1)
	int		m_iCHECK_COUNT;
#endif
#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
	BYTE	OffTradeWaitItem;
	BYTE	bOffTrade;
	int		iOffTradeTime;
#endif
#if (CUSTOM_OLYMP==1)
	BYTE	m_btChangedStats;
	short	m_OldLvlUpPts;
#if( FIX_MAXSTAT == 1 )
	WORD	m_OldStrength;
	WORD	m_OldDexterity;
	WORD	m_OldVitality;
	WORD	m_OldEnergy;
	WORD	m_OldLeadership;
#else
	short	m_OldStrength;
	short	m_OldDexterity;
	short	m_OldVitality;
	short	m_OldEnergy;
	short	m_OldLeadership;
#endif
#endif
	///
#ifdef __NOVUS__
	WORD	m_ExPointPvP;
	WORD	m_ExPointEvent;
	WORD	m_ExPointGens;
	DWORD	m_SummonTick;
	int		m_SummonIndex;
	BYTE	m_SummonID;
#endif
#endif

//#ifdef STREAM
	DWORD	m_Credits;
//#endif

//#if( ENABLE_CUSTOM_HARDCORE == 1 )
	short	m_HardcoreLife;
//#endif

#if (ENABLETEST_MUUN == 1) // need compare calls
	WORD m_wMuunItem;
    CItem* pMuunInventory;
    CItem* pMuunInventory1;
    CItem* pMuunInventory2;
    bool bMuunInventoryLoad;
    int bSubEquip;
    int m_iMuunItmePeriodDataIndex;
    _tagMUUN_EFFECT_LIST m_MuunEffectList[MUUN_MAX_ACTIVEUSE];
    DWORD dwCheckMuunItemTime;
#endif
};
// -------------------------------------------------------------------------------

typedef OBJECTSTRUCT* LPOBJ;
extern LPOBJ MainObj;
extern LPOBJ gObj;

class CObjectStruct
{
public:
	CObjectStruct()
	{
		MainObj = new OBJECTSTRUCT[OBJMAX+1];
		gObj = &MainObj[1];
	};
	virtual ~CObjectStruct(){};
};

// Optimization test start
#if (ENABLETEST_ZTUNIT == 1)
class ZTObjectMonsterInfo {
public:
	int	m_iScriptMaxLife;
	ACTION_STATE m_ActState;
	char m_iMonsterBattleDelay;
	short ShopNumber;
	short m_ItemRate;
	short m_MoneyRate;
	HITDAMAGE_STRUCT sHD[MAX_ST_HIT_DAMAGE];
	short sHDCount;
	int	m_SkyBossMonSheildLinkIndex;
	int	m_SkyBossMonSheild;
	int	m_SkyBossMonSheildTime;
	char m_BossGoldDerconMapNumber;
	BOOL m_bIsInMonsterHerd;
	BOOL m_bIsMonsterAttackFirst;
	class MonsterHerd* m_lpMonsterHerd;
	int	iObjectSecTimer;
	union
	{
		struct
		{
			BYTE m_btCsNpcExistVal1;
			BYTE m_btCsNpcExistVal2;
			BYTE m_btCsNpcExistVal3;
			BYTE m_btCsNpcExistVal4;
		};
		int m_iCsNpcExistVal;
	};
	BYTE	m_btCsNpcType;
	BYTE	m_btCsGateOpen;
	int		m_iCsGateLeverLinkIndex;
	BYTE	m_btCsNpcDfLevel;
	BYTE	m_btCsNpcRgLevel;
	BYTE	m_btWeaponState;
	int		m_iWeaponUser;
	BYTE	m_btCreationState;
	int		m_iCreatedActivationTime;
	TMonsterSkillElementInfo m_MonsterSkillElementInfo;
	int		m_iBasicAI;
	int		m_iCurrentAI;
	int		m_iCurrentAIState;
	DWORD	m_iLastAIRunTime;	
	int		m_iGroupNumber;
	int		m_iSubGroupNumber;
	int		m_iGroupMemberGuid;
	int		m_iRegenType;
	TMonsterAIAgro m_Agro;
	DWORD	m_iLastAutomataRuntime;
	DWORD	m_iLastAutomataDelay;
	BYTE	m_bOreUsed;
};

class ZTObjectPlayerInfo {
public:
	char AccountID[11];
	short m_AccountMaxLevel;
	char LoginMsgSnd;
	char LoginMsgCount;
	char CloseCount;
	char CloseType;
	BOOL bEnableDelCharacter;
	_PER_SOCKET_CONTEXT* PerSocketContext;
	DWORD m_socket;
	char Ip_addr[16];
	int	UserNumber;
	int	DBNumber;
	BYTE Magumsa;
	DWORD AutoSaveTime;
	DWORD ConnectCheckTime;
	BYTE CheckTickCount;
	BYTE m_TimeCount;
	DWORD m_dwPKTimer;
	short CheckSumTableNum;
	DWORD CheckSumTime;
	char LastJoominNumber[20];
	bool PlusStatQuestClear;
	bool ComboSkillquestClear;
	ComboSkillData comboSkill;
	BYTE DbClass;
	BYTE ChangeUP;
	BYTE ChangeUP2;
	int	LevelUpPoint;
	int	iFruitPoint;
	DWORD Experience;
	DWORD NextExp;
	short MasterLevel;
	__int64 MasterExperience;
	__int64 MasterNextExp;
	int	MasterPoint;
	int	MasterPointSpent;
	int	bMasterInfoLoaded;
#if (FIX_MAXSTAT == 1)
	WORD Strength;
	WORD Dexterity;
	WORD Vitality;
	WORD Energy;
#else
	short Strength;
	short Dexterity;
	short Vitality;
	short Energy;
#endif
	WORD Leadership;
	WORD AddLeadership;
	WORD ChatLimitTime;
	BYTE ChatLimitTimeSec;
	BYTE FillLifeCount;
#if (FIX_MAXSTAT == 1)
	WORD AddStrength;
	WORD AddDexterity;
	WORD AddVitality;
	WORD AddEnergy;
#else
	short AddStrength;
	short AddDexterity;
	short AddVitality;
	short AddEnergy;
#endif
	int BP;
	int MaxBP;
	int AddBP;
	float VitalityToLife;
	float EnergyToMana;
	char m_PK_Count;
	char m_PK_Level;
	int m_PK_Time;
	int	iShield;
	int	iMaxShield;
	int	iAddShield;
	int	iFillShieldMax;
	int	iFillShield;
	int	iFillShieldCount;
	DWORD dwShieldAutoRefillTimer;
	BYTE DamageMinus;
	BYTE DamageReflect;
	short MonsterDieGetMoney;
	BYTE MonsterDieGetLife;
	BYTE MonsterDieGetMana;
	BYTE MoveCheck;
	BYTE BindX;
	BYTE BindY;
	BYTE BindMap;
	DWORD Authority;
	DWORD Penalty;
	BYTE m_cAccountItemBlock;
	DWORD m_LastAttackTime;
	BYTE m_FriendServerOnline;
	int	m_DetectSpeedHackTime;
	DWORD m_SumLastAttackTime;
	DWORD m_DetectCount;
	int m_DetectedHackKickCount;
	int	m_SpeedHackPenalty;
	BYTE m_AttackSpeedHackDetectedCount;
	DWORD m_PacketCheckTime;
	BYTE m_ShopTime;
	char m_cKalimaGateExist;
	int m_iKalimaGateIndex;
	char m_cKalimaGateEnterCount;
	short m_SkillNumber;
	DWORD m_SkillTime;
	bool m_bAttackerKilled;
	char m_ManaFillCount;
	char m_LifeFillCount;
	int	SelfDefense[MAX_SELF_DEFENSE];
	DWORD SelfDefenseTime[MAX_SELF_DEFENSE];
	DWORD MySelfDefenseTime;
	char m_Drink;
	int	m_SkillAttack2;
	char m_SkillAttackTime2;
	int	PartyNumber;
	int	PartyTargetUser;
	int	GuildNumber;
	_GUILD_INFO_STRUCT* lpGuild;
	char GuildName[11];
	int	GuildStatus;
	int	iGuildUnionTimeStamp;
	int	m_RecallMon;
	short TargetShopNumber;
	int	m_AttackDamageMaxLeft;
	int	m_AttackDamageMinLeft;
	int	m_AttackDamageMaxRight;
	int	m_AttackDamageMinRight;
	int	m_CurseDamageBook;
	int	m_CurseDamageMin;
	int	m_CurseDamageMax;
	int	m_CriticalDamage;
	int	m_ExcelentDamage;
#if (CUSTOM_EXOPT == 1)
	int	m_CriticalDamageMul;
	int	m_ExcelentDamageMul;
#endif
	short OSAttackSerial; //!
	MASTERLEVEL_PASSIVE_SKILL_OPTION m_MPSkillOpt;
	char m_AddResistance[MAX_RESISTENCE_TYPE];
	tagInterfaceState m_IfState;
	DWORD m_InterfaceTime;
	LPBYTE pInventoryMap;
	char pTransaction;
	BYTE pInventoryExtend;
	CItem* Inventory1;
	LPBYTE InventoryMap1;
	char InventoryCount1;
	CItem* Inventory2;
	LPBYTE InventoryMap2;
	char InventoryCount2;
	CItem* Trade;	
	LPBYTE TradeMap;
	int	TradeMoney;
	bool TradeOk;
	CItem* pWarehouse;
	LPBYTE pWarehouseMap;
	BYTE pWarehouseExtend;
	char WarehouseCount;
	short WarehousePW;
	BYTE WarehouseLock;
	int WarehouseMoney;
	int WarehouseSave;
	CItem* pChaosBox;
	LPBYTE pChaosBoxMap;
	int	ChaosMoney;
	int	ChaosSuccessRate;
	BOOL ChaosLock;
	DWORD m_Option;
	int	m_nEventScore;
	int	m_nEventExp;
	int	m_nEventMoney;
	BYTE m_bDevilSquareIndex;
	BYTE m_bDevilSquareAuth;
	char m_cBloodCastleIndex;
	char m_cBloodCastleSubIndex;
	int	m_iBloodCastleEXP;
	bool m_bBloodCastleComplete;
	char m_cChaosCastleIndex;
	char m_cChaosCastleSubIndex;
	int	m_iChaosCastleBlowTime;
	char m_cKillUserCount;
	char m_cKillMonsterCount;
	int	m_iDuelUserReserved;
	int	m_iDuelUserRequested;
	int	m_iDuelUser;
	BYTE m_btDuelScore;
	int m_iDuelTickCount;
	bool m_bPShopOpen;
	bool m_bPShopTransaction;
	bool m_bPShopItemChange;
	bool m_bPShopRedrawAbs;
	char m_szPShopText[36];
	bool m_bPShopWantDeal;
	int	m_iPShopDealerIndex;
	char m_szPShopDealerName[10];
	CRITICAL_SECTION m_critPShopTrade;
	int	m_iVpPShopPlayer[MAX_VIEWPORT];
	WORD m_wVpPShopPlayerCount;
	bool IsInBattleGround;
	short EventChipCount;
	int	MutoNumber;
	BOOL UseEventServer;
	bool LoadWareHouseInfo;
	int	iStoneCount;
	BOOL m_MonsterKillInfoLoaded;
	BOOL m_MonsterKillQuestIndex;
	QUEST_MONSTER_KILL_INFO m_MonsterKill[5];
	BYTE m_Quest[50];
	bool m_SendQuestInfo;
	BYTE m_MoveOtherServer;
	char m_BackName[11];
	BYTE m_InWebzen;
	char m_LastTeleportTime;
	BYTE m_ClientHackLogCount;
	DWORD m_NPggCheckSumSendTime;
	BYTE m_ReqWarehouseOpen;
	short SetOpAddSkillAttack;
	short SetOpAddExDamage;
	short SetOpAddExDamageSuccessRate;
	short SetOpAddCriticalDamage;
	short SetOpAddCriticalDamageSuccessRate;
	short SetOpIncAGValue;
	short SetOpAddDamage;
	short SetOpAddMinAttackDamage;
	short SetOpAddMaxAttackDamage;
	short SetOpAddAttackDamage;
	short SetOpAddDefence;
	short SetOpAddDefenceRate;
	short SetOpAddMagicPower;
	BYTE SetOpIgnoreDefense;
	BYTE SetOpDoubleDamage;
	BYTE SetOpTwoHandSwordImproveDamage;
	BYTE SetOpImproveSuccessAttackRate;
	BYTE SetOpReflectionDamage;
	BYTE SetOpImproveSheldDefence;
	BYTE SetOpDecreaseAG;
	BYTE SetOpImproveItemDropRate;
	bool IsFullSetItem;
	TDurMagicKeyChecker DurMagicKeyChecker;
	WORD SkillRecallParty_Time;
	BYTE SkillRecallParty_MapNumber;
	BYTE SkillRecallParty_X;
	BYTE SkillRecallParty_Y;
	bool bIsChaosMixCompleted;
	bool SkillLongSpearChange;
	bool m_bMapSvrMoveQuit;
	bool m_bMapSvrMoveReq;
	DWORD m_dwMapSvrQuitTick;
	short m_sPrevMapSvrCode;
	short m_sDestMapNumber;
	BYTE m_btDestX;
	BYTE m_btDestY;
	BYTE m_btCsJoinSide;
	BYTE m_bCsGuildInvolved;
	bool m_bIsCastleNPCUpgradeCompleted;
	BYTE m_btKillCount;
	BYTE m_btLifeStoneCount;
	int	m_iAccumulatedCrownAccessTime;
	int	m_iAccumulatedDamage;
	int	m_iCrywolfMVPScore;
	int	m_iAutoRecuperationTime;
	int	m_iSkillDistanceErrorCount;
	DWORD m_dwSkillDistanceErrorTick;
	JEWELOFHARMONY_ITEM_EFFECT m_JewelOfHarmonyEffect;
	ITEMOPTION_FOR380ITEM_EFFECT m_ItemOptionExFor380;
	WORD m_wItemEffectType;
	int	m_iItemEffectValidTime;
	int	m_wcCashPoint;
	int	m_wpCashPoint;
	int	m_wgCashPoint;
	DWORD GoblinPointTick;
	int	iTempleIndex;
	bool iOptionRecv;
	int	m_iPeriodItemEffectIndex;
	BYTE m_iSummonerCreateFlag;
	union
	{
		struct
		{
			_SOCKET_USER_DATA m_SocketOption[35];
			_SOCKET_USER_DATA m_SocketBonus[7];
			_SOCKET_USER_DATA m_SocketSet[2];
		};
		_SOCKET_USER_DATA m_SocketOptions[44];
	};
	WORD m_wSocketAddLife;
	WORD m_wSocketAddMana;
	WORD m_wSocketMonsterDieLife;
	WORD m_wSocketMonsterDieMana;
	BYTE m_btSocketBPUseDec;
	int	m_iDuelRoom;
	BYTE m_btSculptPos;
	BYTE m_btCharmPos;
	BYTE m_btArtifactPos;
#if (GS_CASTLE == 0 || GS_SPEC == 1)
	BOOL m_bKanturuEntranceByNPC;
#endif
#if (__QUEST_SYSTEM__ == 1)
	USER_QUESTEX_INFO m_QuestEx;
#endif
#if (__GENS_SYSTEM__ == 1)
	GENS_USER m_GensInfo;
#endif
	int	m_ImperialGuardianZone;
	float m_InfinityArrowDmgMul;
	char m_btDarkSideTargetCount;
	short m_wDarkSideTarget[5];
	BYTE m_bIsMaintainParty;
	BYTE m_bIsPartyMatch;
	BYTE m_bIsPartyWaiting;
	int	m_bMUBOT;
	int	m_iMUBOT_STAGE;
	int	m_iMUBOT_TIME;
	ERTEL_USER* pErtelList1;
	ERTEL_USER* pErtelList2;
	ELEMENTAL_OPTIONS* m_ElementalOptions;
	int	m_iMiningLevel;
	int	m_iMiningJewelId;
	int	m_iMiningObjectId;
	bool m_bSendNpcPos;
	bool m_bSendMemberPos;
	char m_EnableUseChangeSkin;
	DWORD m_ShopPointExTime;
	DWORD m_ShopPointExTimeBackup;
	BYTE RecvSendWare;
	int	iResetCount;
	BYTE m_ResetWait;
	DWORD m_ResetWaitTick;
#if (ANTI_CHEAT==1)
	int	m_iCHECK_COUNT;
#endif
#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
	BYTE OffTradeWaitItem;
	BYTE bOffTrade;
	int	iOffTradeTime;
#endif
#if (CUSTOM_OLYMP==1)
	BYTE m_btChangedStats;
	short m_OldLvlUpPts;
#if (FIX_MAXSTAT == 1)
	WORD m_OldStrength;
	WORD m_OldDexterity;
	WORD m_OldVitality;
	WORD m_OldEnergy;
	WORD m_OldLeadership;
#else
	short m_OldStrength;
	short m_OldDexterity;
	short m_OldVitality;
	short m_OldEnergy;
	short m_OldLeadership;
#endif
#endif
	WORD m_ExPointPvP;
	WORD m_ExPointEvent;
	WORD m_ExPointGens;
	DWORD m_SummonTick;
	int	m_SummonIndex;
	BYTE m_SummonID;
	DWORD m_Credits;
#if (ENABLE_CUSTOM_HARDCORE == 1)
	short m_HardcoreLife;
#endif
};

class ZTObjectUnitInfo {
public:
	int	Money;
	short Level;
	char Name[11];
	int m_Index;
	int Connected;
	BYTE Type;
	BYTE Live;
	WORD Class;
	float Life;
	float MaxLife;
	float FillLife;
	float FillLifeMax;
	float Mana;
	float MaxMana;
	BYTE X;
	BYTE Y;
	BYTE Dir;
	BYTE MapNumber;
	int	AddLife;
	int	AddMana;
	BYTE StartX;
	BYTE StartY;
	BYTE m_OldX;
	BYTE m_OldY;
	BYTE TX;
	BYTE TY;
	BYTE MTX;
	BYTE MTY;
	BYTE DeathX;
	BYTE DeathY;
	BYTE DeathMap;
	int PathCount;
	int	PathCur;
	char PathStartEnd;
	short PathOri[15];
	BYTE PathX[15];
	BYTE PathY[15];
	char PathDir[15];
	DWORD PathTime;
	BYTE m_ActionNumber;
	DWORD m_State;
	BYTE m_Rest;
	char m_ViewState;
	BYTE btEffectCount;
	EFFECT_INFO pEffectInfo[MAX_EFFECT_INFO];
	DWORD m_LastMoveTime;
	DWORD TeleportTime;
	char Teleport;
	char KillerType;
	char DieRegen;
	char RegenOk;
	BYTE RegenMapNumber;
	BYTE RegenMapX;
	BYTE RegenMapY;
	DWORD RegenTime;
	DWORD MaxRegenTime;
	short m_PosNum;
	DWORD CurActionTime;
	DWORD NextActionTime;
	DWORD DelayActionTime;
	char DelayLevel;
	class ZTObjectUnit* lpAttackObj;
	int	m_Change;
	short TargetNumber;
	short LastAttackerID;
	int	m_AttackDamageMin;
	int	m_AttackDamageMax;
	int	m_MagicDamageMin;
	int	m_MagicDamageMax;
	int	m_AttackDamageLeft;
	int	m_AttackDamageRight;
	int	m_AttackRating;
	int	m_AttackSpeed;
	int	m_MagicSpeed;
	int	m_Defense;
	int	m_MagicDefense;
	int	m_SuccessfulBlocking;
	short m_MoveSpeed;
	short m_MoveRange;
	short m_AttackRange;
	short m_AttackType;
	short m_ViewRange;
	short m_Attribute;
	CMagicInf* m_lpMagicBack;
	CMagicInf* Magic;
	char MagicCount;
	WORD UseMagicNumber;
	DWORD UseMagicTime;
	char UseMagicCount;
	char CharSet[18];
	char m_Resistance[MAX_RESISTENCE_TYPE];
	int	FrustrumX[MAX_ARRAY_FRUSTRUM];
	int	FrustrumY[MAX_ARRAY_FRUSTRUM];
	VIEWPORT_STRUCT VpPlayer[MAX_VIEWPORT];
	VIEWPORT_PLAYER_STRUCT VpPlayer2[MAX_VIEWPORT];
	int	VPCount;
	int	VPCount2;
	CItem* pInventory;
	char* pInventoryCount;
	int	m_CheckLifeTime;
	int	fSkillFrustrumX[MAX_ARRAY_FRUSTRUM];
	int	fSkillFrustrumY[MAX_ARRAY_FRUSTRUM];
	BYTE SkillHellFire2State;
	BYTE SkillHellFire2Count;
	DWORD SkillHellFire2Time;
	CSkillDelay SkillDelay;
	DWORD m_dwLastCheckTick;
	BYTE m_btAttribute;
	int	m_ElementalDamageMin;
	int	m_ElementalDamageMax;
	int	m_ElementalDefense;
	int	m_ElementalAttackRate;
	int	m_ElementalDefenseRate;
};

class ZTObjectUnit : public ZTObjectUnitInfo {
public:
	ZTObjectPlayerInfo* m_PlayerInfo;
	ZTObjectMonsterInfo* m_MonsterInfo;

	ZTObjectPlayerInfo* getPlayer() {
		if (m_Index < OBJ_STARTUSERINDEX) {
			LogAddDebug("[ZTObjectUnit] %d invalid object type for return player info",
				m_Index);
			return 0;
		}

		if (!m_PlayerInfo) {
			LogAddDebug("[ZTObjectUnit] %d object not have player info",
				m_Index);
			return 0;
		}

		return m_PlayerInfo;
	}

	ZTObjectMonsterInfo* getMonster() {
		if (m_Index >= OBJ_STARTUSERINDEX) {
			LogAddDebug("[ZTObjectUnit] %d invalid object type for return monster info",
				m_Index);
			return 0;
		}

		if (!m_MonsterInfo) {
			LogAddDebug("[ZTObjectUnit] %d object not have monster info",
				m_Index);
			return 0;
		}

		return m_MonsterInfo;
	}
};
#endif
// Optimization test end

#define MAX_PAYCODE 3

class BILL_CLASS
{

private:

	int PayCode;	// 1
	int EndTime;	// 4
	char EndsDays[13];	// 8

	int AddExp;
	int AddMasterExp;
	int AddDrop;
	int AddZen;
	int ResetDiscount;
	int GPBonus;

public:

	BILL_CLASS()	// line : 253
	{
		this->Init();
	}


	void Init()	// line : 219
	{
		this->PayCode=-1;
		this->EndTime=0;
		this->EndsDays[0]=0;

		this->AddExp = 0;
		this->AddZen = 0;
		this->AddDrop = 0;
		this->AddMasterExp = 0;
		this->ResetDiscount = 0;
		this->GPBonus = 0;
	}


	BOOL SetBill(int pay, int end_time,char* end_days)	// line : 228
	{
		BOOL bRet=0;

		if ( this->PayCode == -1 )
		{
			bRet = 1;
		}

		this->PayCode = pay;
		this->EndTime = end_time;
		strcpy_s(EndsDays, sizeof(EndsDays), end_days);
		return bRet;
	}	// line : 237

	//int __thiscall IsEndTime();
	int GetPayCode() {return this->PayCode;}	// line : 252
	char* GetEndsDays() {return &this->EndsDays[0];}	// line : 253
	int GetEndTime() { return this->EndTime;}	// line : 254

	void SetExp(int Exp) { AddExp = Exp; };
	void SetMasterExp(int Exp) { AddMasterExp = Exp; };
	void SetDrop(int Drop) { AddDrop = Drop; };
	void SetZen(int Zen) {AddZen = Zen; };
	void SetResetDiscount(int Discount) {ResetDiscount = Discount; };
	void SetGPBonus(int Bonus) {GPBonus = Bonus; };

	int GetExp() {return AddExp;};
	int GetMasterExp() {return AddMasterExp;};
	int GetDrop() {return AddDrop;};
	int GetZen() {return AddZen; };
	int GetResetDiscount() { return ResetDiscount; };
	int GetGPBonus() { return GPBonus; };
};

typedef struct
{
	int AddExp;
	int AddMasterExp;
	int AddDrop;
	int AddZen;
	int ResetDiscount;
	int GPBonus;
}PREMIUM_SETTINGS;


extern BILL_CLASS m_ObjBill[OBJMAX];
extern std::map<int,PREMIUM_SETTINGS> gPremiumData;
extern BYTE gPremiumServer[4];

#ifndef SILVER
void LoadPremiumSettings(char* szFileName);
#endif

extern int gObjCSFlag;
extern int gItemLoop;
extern int gItemLoop2;
extern int gItemLoopMax;
extern int gObjTotalUser;
extern int gDisconnect;
extern int gCurConnectUser; 
extern int skillSuccess;
extern int GuildInfoOfs;
extern int GuildInfoCount;
extern int GuilUserOfs;
extern int GuildUserCount;
extern int lOfsChange;
extern int ChangeCount;

extern int gObjCallMonCount;
extern int gObjMonCount;
extern int gObjCount;

void gObjCalCharacter(int aIndex);
void gObjSkillUseProcTime500(LPOBJ lpObj);
void MonsterAndMsgProc();
void MoveMonsterProc();
void gObjInit();
void gObjEnd();
void gObjClearViewportOfMine(LPOBJ lpObj);
void gObjClearViewport(LPOBJ lpObj);
void gObjCloseSet(int aIndex, int Flag);
void gObjCharTradeClear(LPOBJ lpObj);
void gObjCharZeroSet(int aIndex);
int gObjGetSocket(SOCKET socket);
void gObjSetTradeOption(int aIndex, int option);
void gObjSetDuelOption(int aIndex, int option);
bool IsDuelEnable(int aIndex);
bool IsOnDuel(int aIndex1, int aIndex2);
int GetMapMoveLevel(LPOBJ lpObj, int mapnumber, int max_over);
void DbItemSetInByte(LPOBJ lpObj, struct SDHP_DBCHAR_INFORESULT* lpMsg, int ItemDbByte, bool* bAllItemExist);
void gObjSetBP(int aIndex);
int gObjSetCharacter(unsigned char* lpdata, int aIndex);
int gObjCanItemTouch(LPOBJ lpObj, int type);
void gObjMagicTextSave(LPOBJ lpObj);
void ItemIsBufExOption(unsigned char* buf, class CItem* lpItem);
void gObjStatTextSave(LPOBJ lpObj);
void gObjItemTextSave(LPOBJ lpObj);
void gObjWarehouseTextSave(LPOBJ lpObj);
void gObjAuthorityCodeSet(LPOBJ lpObj);
int gObjSetPosMonster(int aIndex, int PosTableNum);
int gObjSetMonster(int aIndex, int MonsterClass);
void gObjDestroy(SOCKET aSocket, int client);
short gObjAddSearch(unsigned int aSocket, char* ip);
short gObjAdd(SOCKET aSocket, char* ip, int aIndex);
short gObjMonsterRecall(int iMapNumber);
short gObjAddMonster(int iMapNumber);
short gObjAddCallMon();
void gObjUserKill(int aIndex);
void gObjAllLogOut();
void gObjAllDisconnect();
void gObjTradeSave(LPOBJ lpObj, int index);
void gObjBillRequest(LPOBJ lpObj);
short gObjMemFree(int index);
int gObjGameClose(int aIndex);
short gObjDel(int index);
short gObjSometimeClose(int index);
int gObjIsGamePlaing(LPOBJ lpObj);
int gObjIsConnectedGP(int aIndex, char* CharName);
int gObjIsConnectedGP(int aIndex);
int gObjIsConnected(LPOBJ lpObj, int dbnumber);
int gObjIsConnected(int aIndex);
int gObjIsConnected(LPOBJ lpObj);
int gObjIsAccontConnect(int aIndex, char* accountid);
int gObjJoominCheck(int aIndex, char* szInJN);
int gObjCheckXYMapTile(LPOBJ lpObj, int iDbgName);
int gObjSetAccountLogin(int aIndex, char* szId, int aUserNumber, int aDBNumber, char* JoominNumber);
int gObjGetUserNumber(int aIndex);
int gObjGetNumber(int aIndex, int& UN, int& DBN);
LPSTR gObjGetAccountId(int aIndex);
int gObjGetIndex(char* szId);
int gObjUserIdConnectCheck(char* szId, int index);
unsigned char GetPathPacketDirPos(int px, int py);
int gObjCalDistance(LPOBJ lpObj1, LPOBJ lpObj2);
int gObjPositionCheck(LPOBJ lpObj);
int gObjCheckTileArea(int aIndex, int x, int y, int dis);
int ExtentCheck(int x, int y, int w, int h);
void gObjSetInventory1Pointer(LPOBJ lpObj);
void gObjSetInventory2Pointer(LPOBJ lpObj);
#if (ENABLETEST_SEASON8DOT3 == 1)
//mu rummy gObjSetEventInventory1Pointer
//mu rummy gObjSetEventInventory2Pointer
#endif
#if (ENABLETEST_MUUN == 1)
void gObjSetMuunInventory1Pointer(LPOBJ lpObj);
void gObjSetMuunInventory2Pointer(LPOBJ lpObj);
#endif
void gObjAddMsgSend(LPOBJ lpObj, int aMsgCode, int aIndex, int SubCode);
void gObjAddMsgSendDelay(LPOBJ lpObj, int aMsgCode, int aIndex, int delay, int SubCode);
void gObjAddMsgSendDelayInSpecificQPos(LPOBJ lpObj, int aMsgCode, int aIndex, int delay, int SubCode, int iQPosition);
void gObjAddAttackProcMsgSendDelay(LPOBJ lpObj, int aMsgCode, int aIndex, int delay, int SubCode, int SubCode2);
void gObjMsgProc(LPOBJ lpObj);
void gObjStateProc(LPOBJ lpObj, int aMsgCode, int aIndex, int SubCode);
void gObjStateAttackProc(LPOBJ lpObj, int aMsgCode, int aIndex, int SubCode, int SubCode2);
int gObjBackSpring(LPOBJ lpObj, LPOBJ lpTargetObj);
int BackSpringCheck(int& x, int& y, int& dir, unsigned char map);
int gObjBackSpring2(LPOBJ lpObj, LPOBJ lpTargetObj, int count);
bool gObjLevelUp(LPOBJ lpObj, __int64* addexp, int iMonsterType, int iEventType);
int gObjLevelUpPointAdd(unsigned char type, LPOBJ lpObj);
void gObjMonsterItemLoadScript(LPOBJ lpObj, char* script_file);
BOOL InventoryExtentCheck(int x, int y, int w, int h);
void gObjGiveItemSearch(LPOBJ lpObj, int maxlevel);
unsigned char gObjWarehouseInsertItem(LPOBJ lpObj, int type, int index, int level, int pos);
void gObjGiveItemWarehouseSearch(LPOBJ lpObj, int maxlevel);
int gObjGuildMasterCapacityTest(LPOBJ lpObj);
void gObjNextExpCal(LPOBJ lpObj);
int retResistance(LPOBJ lpObj, int Resistance_Type);
int retCalcSkillResistance(LPOBJ lpTargetObj, int isDouble);
int gObjAttackQ(LPOBJ lpObj);
void gObjPlayerKiller(LPOBJ lpObj, LPOBJ lpTargetObj,int MSBDamage);
void gObjUserDie(LPOBJ lpObj, LPOBJ lpTargetObj);
void gObjInterfaceTimeCheck(LPOBJ lpObj);
void gObjPkDownTimeCheck(LPOBJ lpObj, int TargetLevel);
int gObjAngelSprite(LPOBJ lpObj);
int gObjSatanSprite(LPOBJ lpObj);
int gObjWingSprite(LPOBJ lpObj);
int gObjUniriaSprite(LPOBJ lpObj);
void gObjSpriteDamage(LPOBJ lpObj, int damage);
void gObjSecondDurDown(LPOBJ lpObj);
void gObjChangeDurProc(LPOBJ lpObj);
void gObjWingDurProc(LPOBJ lpObj);
void gObjPenaltyDurDown(LPOBJ lpObj, LPOBJ lpTargetObj);
void gObjWeaponDurDown(LPOBJ lpObj, LPOBJ lpTargetObj, int type);
void gObjArmorRandomDurDown(LPOBJ lpObj, LPOBJ lpAttackObj);
bool gObjIsSelfDefense(LPOBJ lpObj, int aTargetIndex);
void gObjCheckSelfDefense(LPOBJ lpObj, int aTargetIndex);
void gObjTimeCheckSelfDefense(LPOBJ lpObj);
void gObjAttack(LPOBJ lpObj, LPOBJ lpTargetObj, class CMagicInf* lpMagic, int magicsend, unsigned char MSBFlag, int AttackDamage, BOOL bCombo, BYTE byBarrageCount, BYTE byReflect);
void gObjMonsterExpDivision(LPOBJ lpMonObj, LPOBJ lpObj, int AttackDamage, int MSBFlag);
int gObjMonsterExpSingle(LPOBJ lpObj, LPOBJ lpTargetObj, int dmg, int tot_dmg, bool& bSendExp);
void gObjExpParty(LPOBJ lpObj, LPOBJ lpTargetObj, int AttackDamage, int MSBFlag);
void gObjMonsterDieLifePlus(LPOBJ lpObj, LPOBJ lpTartObj);
void gObjLifeCheck(LPOBJ lpTargetObj, LPOBJ lpObj, int AttackDamage, int DamageSendType, int MSBFlag, int MSBDamage,WORD Skill, int iShieldDamage);
int gObjInventoryTrans(int aIndex);
int gObjInventoryCommit(int aIndex);
int gObjInventoryRollback(int aIndex);
void gObjInventoryItemSet(int aIndex, int itempos, unsigned char set_byte);
void gObjInventoryItemBoxSet(int aIndex, int itempos, int xl, int yl, unsigned char set_byte);
bool gObjFixInventoryPointer(int aIndex);
void gObjInventoryItemSet_PShop(int aIndex, int itempos, unsigned char set_byte);
void gObjInventoryItemBoxSet_PShop(int aIndex, int itempos, int xl, int yl, unsigned char set_byte);
BOOL CheckOutOfInventory(int aIndex, int sy, int height);	//-> 1.01.00
BYTE gObjInventoryDeleteItem(int aIndex, int itempos);
BYTE gObjWarehouseDeleteItem(int aIndex, int itempos);
BYTE gObjChaosBoxDeleteItem(int aIndex, int itempos);
#if (ENABLETEST_MUUN == 1)
bool gObjFixMuunInventoryPointer(int aIndex);
bool gObjMuunInventoryDeleteItem(int aIndex, int itempos);
BYTE gObjMuunInvenMove(OBJECTSTRUCT* lpObj, int& durSsend, int& durTsend, BYTE source, BYTE target, LPBYTE siteminfo);
BYTE gObjMuunInventoryInsertItem(int aIndex, CMapItem* item);
BYTE gObjMuunInventoryInsertItem(int aIndex, CItem item);
BYTE gObjMuunInventoryInsertItemPos(int aIndex, CItem item, int pos);
BYTE gObjChkMuunInventoryEmpty(OBJECTSTRUCT* lpObj);
#endif
unsigned char gObjInventoryInsertItem(LPOBJ lpObj, int type, int index, int level, int iSerial, int iDur);
DWORD gGetItemNumber();
void gPlusItemNumber();
unsigned char gObjInventoryInsertItem(LPOBJ lpObj, int type, int index, int level, int iSerial, int iDur);
unsigned char gObjOnlyInventoryInsertItem(int aIndex, class CItem item);
unsigned char gObjInventoryInsertItem(LPOBJ lpObj, int type, int index, int level);
int gObjSearchItem(LPOBJ lpObj, int item_type, int add_dur, int nLevel);
int gObjSearchItemMinus(LPOBJ lpObj, int pos, int m_dur);
unsigned char gObjShopBuyInventoryInsertItem(LPOBJ lpObj, int type, int index, int level, int iSerial, int iDur);
unsigned char gObjShopBuyInventoryInsertItem(int aIndex, class CItem item);
unsigned char gObjInventoryInsertItemTemp(LPOBJ lpObj, class CMapItem* Item);
unsigned char gObjInventoryInsertItem(int aIndex, class CMapItem* item);
unsigned char gObjInventoryInsertItem(int aIndex, class CItem item);
unsigned char gObjMonsterInventoryInsertItem(LPOBJ lpObj, int type, int index, int level, int op1, int op2, int op3);
unsigned char gObjInventoryRectCheck(int aIndex, int sx, int sy, int width, int height);
unsigned char gObjOnlyInventoryRectCheck(int aIndex, int sx, int sy, int width, int height);
unsigned char gObjExtendedInventoryRectCheck(int aIndex, int sx, int sy, int width, int height,int id);
int CheckInventoryEmptySpace(LPOBJ lpObj, int iItemHeight, int iItemWidth);
int gObjIsItemPut(LPOBJ lpObj, class CItem* lpItem, int pos);
unsigned char gObjWerehouseRectCheck(int aIndex, int sx, int sy, int width, int height);
unsigned char gObjMapRectCheck(unsigned char* lpMapBuf, int sx, int sy, int ex, int ey, int width, int height);
void gObjWarehouseItemBoxSet(int aIndex, int itempos, int xl, int yl, unsigned char set_byte);
void gObjWarehouseItemSet(int aIndex, int itempos, unsigned char set_byte);
void gObjChaosItemBoxSet(int aIndex, int itempos, int xl, int yl, unsigned char set_byte);
void gObjChaosItemSet(int aIndex, int itempos, unsigned char set_byte);
unsigned char gObjChaosBoxInsertItemPos(int aIndex, class CItem item, int pos, int source);
unsigned char gObjWarehouseInsertItemPos(int aIndex, class CItem item, int pos, int source);
unsigned char gObjInventoryInsertItemPos(int aIndex, class CItem item, int pos, int RequestCheck);
int gObjInventorySearchSerialNumber(LPOBJ lpObj, unsigned long serial);
int gObjWarehouseSearchSerialNumber(LPOBJ lpObj, unsigned long sirial);
unsigned char gObjInventoryMoveItem(int aIndex, unsigned char source, unsigned char target, int& durSsend, int& durTsend, unsigned char sFlag, unsigned char tFlag, unsigned char* siteminfo);
unsigned char gObjTradeRectCheck(int aIndex, int sx, int sy, int width, int height);
int gObjTradeItemBoxSet(int aIndex, int itempos, int xl, int yl, unsigned char set_byte);
unsigned char gObjTradeInventoryMove(LPOBJ lpObj, unsigned char source, unsigned char target);
unsigned char gObjInventoryTradeMove(LPOBJ lpObj, unsigned char source, unsigned char target);
unsigned char gObjTradeTradeMove(LPOBJ lpObj, unsigned char source, unsigned char target);
void gObjTradeCancel(int aIndex);
void gObjTempInventoryItemBoxSet(unsigned char* TempMap, int itempos, int xl, int yl, unsigned char set_byte);
unsigned char gObjTempInventoryRectCheck(int aIndex, unsigned char* TempMap, int sx, int sy, int width, int height);
unsigned char gObjExtendTempInventoryRectCheck(BYTE * TempMap, int sx, int sy, int width, int height,int id);
unsigned char gObjTempInventoryInsertItem(LPOBJ lpObj, class CItem item, unsigned char* TempMap);
int TradeItemInventoryPutTest(int aIndex);
int TradeitemInventoryPut(int aIndex);
void gObjTradeOkButton(int aIndex);
unsigned char LevelSmallConvert(int level);
unsigned char LevelSmallConvert(int aIndex, int inventoryindex);
void gObjAttackDamageCalc(int aIndex);
void gObjMakePreviewCharSet(int aIndex);
void gObjViewportPaint(HWND hWnd, /*<regrel ebp+0xc>*/ /*|0x2|*/ short aIndex);
void InitFrustrum();
void CreateFrustrum(int x, int y, int aIndex);
bool TestFrustrum2(int x, int y, int aIndex);
int gObjCheckViewport(int aIndex, int x, int y);
void gObjViewportClose(LPOBJ lpObj);
void gObjViewportListCreate(short aIndex);
int ViewportAdd(int aIndex, int aAddIndex, int aType);
int ViewportAdd2(int aIndex, int aAddIndex, int aType);
void ViewportDel(short aIndex, int aDelIndex);
void Viewport2Del(short aIndex, int aDelIndex);
void gObjViewportAllDel(short aIndex);
void gObjViewportListDestroy(short aIndex);
void PShop_ViewportListRegenarate(short aIndex);
bool PShop_CheckInventoryEmpty(short aIndex);
void gObjStateSetCreate(int aIndex);
void gObjSetState();
void gObjSecondProc();
void gObjManaPotionFill(LPOBJ lpObj);
void gObjRestPotionFill(LPOBJ lpObj);
void gObjUseDrink(LPOBJ lpObj, int level);
int gObjCurMoveMake(unsigned char* const path, LPOBJ lpObj);
void gObjViewportListProtocolDestroy(LPOBJ lpObj);
void gObjViewportListProtocolCreate(LPOBJ lpObj);
void gObjViewportListProtocol(short aIndex);
void gObjSkillUseProc(LPOBJ lpObj);
//void gObjSkillBeAttackProc(LPOBJ lpObj);
void gObjTeleportMagicUse(int aIndex, unsigned char x, unsigned char y);
int gObjMoveGate(int aIndex, int gt);
void gObjTeleport(int aIndex, int map, int x, int y);
void gObjMoveDataLoadingOK(int aIndex);
class CMagicInf* gObjGetMagic(LPOBJ lpObj, int mIndex);
class CMagicInf* gObjGetMagicSearch(LPOBJ lpObj,int skillnumber);
int gObjMagicManaUse(LPOBJ lpObj, class CMagicInf* lpMagic);
int gObjMagicBPUse(LPOBJ lpObj, class CMagicInf* lpMagic);
int gObjPosMagicAttack(LPOBJ lpObj, class CMagicInf* lpMagic, unsigned char x, unsigned char y);
void gObjMagicAddEnergyCheckOnOff(int flag);
int gObjMagicAdd(LPOBJ lpObj, int aSkill, unsigned char Level);
int gObjMagicDel(LPOBJ lpObj, int aSkill, unsigned char Level);
int gObjMagicAdd(LPOBJ lpObj, unsigned char Type, unsigned char Index, unsigned char Level, WORD& SkillNumber);
int gObjWeaponMagicAdd(LPOBJ lpObj, WORD aSkill, BYTE Level);
int gObjMonsterMagicAdd(LPOBJ lpObj, WORD aSkill, BYTE Level);
int gObjMagicEnergyCheck(LPOBJ lpObj, WORD aSkill);
int gObjItemLevelUp(LPOBJ lpObj, int source, int target);
int gObjItemLevelUpPickAxe(LPOBJ lpObj,int source,int target);
int gObjItemRandomLevelUp(LPOBJ lpObj, int source, int target);
int gObjItemRandomOption3Up(LPOBJ lpObj, int source, int target);
int gObjItemDurUp(LPOBJ lpObj, int source, int target);
int gObjItemStrenghtenUp(LPOBJ lpObj, int source, int target);

#if (ENABLETEST_CUSTOM_JEWELS==1)

BOOL gObjJewel1Enchant(LPOBJ lpObj, int source, int target);
BOOL gObjJewel2Enchant(LPOBJ lpObj, int source, int target);
BOOL gObjJewel3Enchant(LPOBJ lpObj, int source, int target);
BOOL gObjJewel4Enchant(LPOBJ lpObj, int source, int target);
BOOL gObjJewel5Enchant(LPOBJ lpObj, int source, int target);

#endif

void gObjAbilityReSet(LPOBJ lpObj);
int gObjTargetGuildWarCheck(LPOBJ lpObj, LPOBJ lpTargetObj);
void gObjGuildWarEndSend(struct _GUILD_INFO_STRUCT* lpGuild1, struct _GUILD_INFO_STRUCT* lpGuild2, unsigned char Result1, unsigned char Result2);
void gObjGuildWarEndSend(LPOBJ lpObj, unsigned char Result1, unsigned char Result2);
void gObjGuildWarEnd(struct _GUILD_INFO_STRUCT* lpGuild, struct _GUILD_INFO_STRUCT* lpTargetGuild);
int gObjGuildWarProc(struct _GUILD_INFO_STRUCT* lpGuild1, struct _GUILD_INFO_STRUCT* lpGuild2, int score);
int gObjGuildWarCheck(LPOBJ lpObj, LPOBJ lpTargetObj);
int gObjGuildWarMasterClose(LPOBJ lpObj);
int gObjGuildWarItemGive(struct _GUILD_INFO_STRUCT* lpWinGuild, struct _GUILD_INFO_STRUCT* lpLoseGuild);
int gObjGetPkTime(LPOBJ lpObj, int& hour, int& min);
int gObjMonsterCall(int aIndex, int MonsterType, int x, int y);
void gObjMonsterCallKill(int aIndex);
int gObjCheckTeleportArea(int aIndex, unsigned char x, unsigned char y);
int gObjCheckAttackAreaUsedPath(int aIndex, int TarObjIndex);
int gObjCheckattackAreaUsedViewPort(int aIndex, int TarObjIndex);
int gObjCheckAttackArea(int aIndex, int TarObjIndex);
int gUserFindDevilSquareInvitation(int aIndex);
int gUserFindDevilSquareKeyEyes(int aIndex);
void gObjSendUserStatistic(int aIndex, int startLevel, int endLevel);
LPOBJ gObjFind(char* targetcharname);
int gObjFind10EventChip(int aIndex);
int gObjDelete10EventChip(int aIndex);
void gObjSetPosition(int aIndex, int x, int y);
int gObjGetItemCountInEquipment(int aIndex, int itemtype, int itemindex, int itemlevel);
int gObjGetItemCountInIventory(int aIndex, int itemnum);
int gObjGetItemCountInIventory(int aIndex, int itemtype, int itemindex, int itemlevel);
int gObjGetManaItemPos(int aIndex);
void gObjDelteItemCountInInventory(int aIndex, int itemtype, int itemindex, int count);
void gObjDelteItemCountInInventory(int aIndex, int itemtype, int itemindex,int itemlevel, int count);
void gObjGetStatPointState(int aIndex, short& AddPoint, short& MaxAddPoint, short& MinusPoint, short& MaxMinusPoint);
int gObjCheckStatPointUp(int aIndex);
int gObjCheckStatPointDown(int aIndex);
void gObjUseCircle(int aIndex, int pos);
void gObjUsePlusStatFruit(int aIndex, int pos);
void gObjUseMinusStatFruit(int aIndex, int pos);
void gObjDelayLifeCheck(int aIndex);
int gObjDuelCheck(LPOBJ lpObj, LPOBJ lpTargetObj);
int gObjDuelCheck(LPOBJ lpObj);
void gObjResetDuel(LPOBJ lpObj);
void gObjSendDuelEnd(LPOBJ lpObj);
void gObjCheckAllUserDuelStop();
void gObjNProtectGGCheckSum(int aIndex);
void SkillFrustrum(unsigned char bangle, int aIndex);
int SkillTestFrustrum(int x, int y, int aIndex);
int gObjCheckMaxZen(int aIndex, int nAddZen);
void MakeRandomSetItem(int aIndex);
void MakeRewardSetItem(int aIndex, BYTE cDropX, BYTE cDropY, int iRewardType, int iMapnumber);
void gObjRecall(int aIndex, int mapnumber, int x, int y);
void gObjSetExpPetItem(int aIndex, int exp);
int gObjGetRandomItemDropLocation(int iMapNumber, BYTE & cX, BYTE & cY, int iRangeX, int iRangeY, int iLoopCount);
int gObjGetRandomFreeLocation(int iMapNumber, BYTE& cX, BYTE& cY, int iRangeX, int iRangeY, int iLoopCount);
int gObjCheckAttackTypeMagic(int iClass, int iSkill);
int gObjGetGuildUnionNumber(LPOBJ lpObj);
void gObjGetGuildUnionName(LPOBJ lpObj, char* szUnionName, int iUnionNameLen);
int gObjCheckRival(LPOBJ lpObj, LPOBJ lpTargetObj);
int gObjGetRelationShip(LPOBJ lpObj, LPOBJ lpTargetObj);
void gObjNotifyUpdateUnionV1(LPOBJ lpObj);
void gObjNotifyUpdateUnionV2(LPOBJ lpObj);
void gObjUnionUpdateProc(int aIndex);
void gObjSetKillCount(int aIndex, int iOption);
void gObjNotifyUseWeaponV1(LPOBJ lpOwnerObj, LPOBJ lpWeaponObj, int iTargetX, int iTargetY);
void gObjNotifyUseWeaponDamage(LPOBJ lpWeaponObj, int iTargetX, int iTargetY);
void gObjUseBlessAndSoulPotion(int aIndex, int iItemLevel);
void gObjWeaponDurDownInCastle(LPOBJ lpObj, LPOBJ lpTargetObj, int iDecValue);
void gObjReady4Relife(LPOBJ lpObj);
void gObjCheckTimeOutValue(LPOBJ lpObj, DWORD& rNowTick);
void MsgOutput(int aIndex, char* msg, ...);
void gProcessAutoRecuperation(LPOBJ lpObj);
void gObjShieldAutoRefill(LPOBJ lpObj);
int gObjCheckOverlapItemUsingDur(int iUserIndex, int iMaxOverlapped, int iItemType, int iItemLevel);
int gObjOverlapItemUsingDur(class CItem* lpItem, int iMapNumber, int iItemNumber, int iUserIndex, int iMaxOverlapped, int iItemType, int iItemLevel);
int gObjCheckSerial0ItemList(class CItem* lpItem);
int gObjCheckInventorySerial0Item(LPOBJ lpObj);
BOOL gCheckSkillDistance(int aIndex, int aTargetIndex, int iSkillNum);
void gObjSaveChaosBoxItemList(LPOBJ lpObj);
int gObjGetRandomFreeArea(int iMapNumber,unsigned char & cX,unsigned char & cY,int iSX,int iSY,int iDX,int iDY,int iLoopCount);
void ExMinusStatFruit(LPOBJ lpObj,int pos);
void UseCashItems(LPOBJ lpObj,int pos);
void UsePKRemoveItem(LPOBJ lpObj,int pos);
void UseExCashPotions(LPOBJ lpObj,int pos);
void UseMediumElitePotion(LPOBJ lpObj,int pos,double mul);
BOOL gObjSearchItemPlus(LPOBJ lpObj,int iItemCode, int iItemLevel,int AddDur, int MaxDur);
BOOL gObjDemonSprite(LPOBJ lpObj);
BOOL gObjGuardianSprice(LPOBJ lpObj);
BOOL gObjPandaSprite(LPOBJ lpObj);
BOOL gObjUnicornSprite(LPOBJ lpObj);
BOOL gObjSkeletonSprite(LPOBJ lpObj);
BOOL isGM(LPOBJ lpObj);
BYTE CheckAuthority(int AuthCode,LPOBJ lpObj);
BOOL gObjCheckMapBind(int MapNumber);
bool IsTransformCharcterSkin(int pChange);	//1.01.06
bool IsTransformRing(WORD ItemCode);
bool IsTransformEquipment(int UserIndex);
void gObjMuBotProc(LPOBJ lpObj);

BOOL gObjFindInventoryEquipment(int aIndex);	//ok
BOOL gObjSetInventoryEquipment(LPOBJ lpObj, BYTE btPos);	//ok
BOOL gObjInventoryEquipment(LPOBJ lpObj);	//ok