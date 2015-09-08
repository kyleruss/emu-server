#pragma once
// ----------------------------------------------------------------------------------------------

namespace MUCharClass
{
	enum T
	{
		Wizard		= 0,
		Knight		= 1,
		Elf			= 2,
		Gladiator	= 3,
		Lord		= 4,
		Summoner	= 5,
		Fighter		= 6,
	};
};
// ----------------------------------------------------------------------------------------------

#pragma pack(push, 1)
struct ObjectItem //-> Size: 140
{
/*+0*/		DWORD	Unknown0;
/*+4*/		DWORD	ItemID;
/*+8*/		DWORD	Unknown8;
/*+12*/		short	Unknown12;
/*+14*/		short	Level;
//BYTE gap0001;
/*+16*/		//char	Unknown6;
/*+13*/		char	Unknown7;
/*+14*/		char	Unknown84;
/*+15*/		short	DamageMin;
/*+17*/		short	DamageMax;
/*+19*/		char	Unknown13;
//BYTE Unknown0w[2];
/*+20*/		short	Unknown14;
/*+22*/		short	Unknown16;
/*+24*/		char	Unknown18;
/*+25*/		char	Unknown19;
/*+26*/		short	Unknown20;
/*+28*/		char	Durability;
/*+29*/		char	ExcellentOption;
/*+30*/		char	AncientOption;
/*+31*/		short	ReqStrenght;
/*+33*/		short	ReqDextirity;
/*+35*/		short	ReqEnergy;
/*+37*/		short	ReqVitality;
/*+39*/		short	ReqCommand;
/*+41*/		short	ReqLevel;
/*+43*/		char    SpecialCount;
/*+44*/		WORD	SpecialType[8];
/*+54*/		BYTE	SpecialValue[8];
/*+62*/		int		UniqueID;
/*+66*/		char	Unknown66;
/*+67*/		BYTE 	PosX;
/*+68*/		BYTE 	PosY;
/*+69*/		WORD 	HarmonyType;
/*+71*/		short	HarmonyValue;
/*+73*/		char 	Is380Item;
/*+74*/		char	SocketOption[5];
/*+79*/		char	Unknown79;
/*+80*/		char	SocketSeedIndex[5];
/*+85*/		char	SocketSphereLevel[5];
/*+90*/		char	SocketSet;
BYTE gap01[5];
/*+96*/		char	DurabilityState;
/*+97*/		char 	PeriodItem;
/*+98*/		char 	ExpiredItem;
/*+99*/		int		ExpireDateConvert;
/*+103*/	int		Unknown103;
BYTE gap02[24];
};
#pragma pack(pop)
// ----------------------------------------------------------------------------------------------

#pragma pack(push, 1)
struct ObjectCharacter //Size: 6684, Global: 84CF3DC | 84CF3D8
{
/*+0*/		char	m_Name[11];
/*+11*/		char	m_Class;
/*+12*/		BYTE	m_Unknown12;
/*+13*/		BYTE	m_Unknown13;
/*+14*/		short	m_Level;
/*+16*/		DWORD	m_Experience;
/*+20*/		DWORD	m_NextExperience;
/*+24*/		WORD	m_Strength;
/*+26*/		WORD	m_Dexterity;
/*+28*/		WORD	m_Vitality;
/*+30*/		WORD	m_Energy;
/*+32*/		WORD	m_Leadership;
/*+34*/		WORD	m_Life;
/*+36*/		WORD	m_Mana;
/*+38*/		WORD	m_MaxLife;
/*+40*/		WORD	m_MaxMana;
/*+42*/		WORD	m_Shield;
/*+44*/		WORD	m_MaxShield;
/*+46*/		WORD	m_AttackRate;
/*+48*/		WORD	m_DefenseRate;
/*+50*/		short	m_AddStrength;
/*+52*/		short	m_AddDexterity;
/*+54*/		short	m_AddVitality;
/*+56*/		short	m_AddEnergy;
BYTE gap01[4];
/*+62*/		WORD	m_AddLeadership;
/*+64*/		WORD	m_Stamina;
/*+66*/		WORD	m_MaxStamina;
/*+68*/		BYTE	m_ItemSpecialUseFlag;
BYTE UnknownGap;
/*+70*/		WORD	m_ItemSpecialUseTime[3];
/*+76*/		WORD	m_AddPoint;
/*+78*/		WORD	m_MaxAddPoint;
/*+80*/		WORD	m_MinusPoint;
/*+82*/		WORD	m_MaxMinusPoint;
/*+84*/		WORD	m_AttackSpeed;
/*+86*/		WORD	m_DamageRate;
/*+88*/		WORD	m_DamageMin;
/*+90*/		WORD	m_DamageMax;
/*+92*/		WORD	m_Unknown92;
/*+94*/		WORD	m_Unknown94;
/*+96*/		WORD	m_MagicSpeed;
BYTE gap02[10];
/*+108*/	WORD	m_Unknown108;
/*+110*/	WORD	m_Unknown110;
BYTE gap03[4];
/*+116*/	WORD	m_LevelPoint;
/*+118*/	BYTE	m_MagicCount;
/*+119*/	BYTE	m_Unknown119;
/*+120*/	WORD	m_MagicList[650];
/*1420*/
BYTE gap04[3252];
/*+4672*/	ObjectItem m_Equipment[12];
BYTE gap05[272];
/*+6624*/	DWORD	m_MoneyInventory;
/*+6628*/	DWORD	m_MoneyWarehouse;
BYTE gap06[52];
};
#pragma pack(pop)
// ----------------------------------------------------------------------------------------------