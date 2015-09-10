#ifndef CHAOSBOX_H
#define CHAOSBOX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "protocol.h"
#include "..\common\zzzitem.h"


#define CHAOS_BOX_SIZE 32
#define MIN_CHAOS_ITEM_LEVEL 4

#define CHAOS_BOX_RANGE(x) (((x)<0)?FALSE:((x)>CHAOS_BOX_SIZE-1)?FALSE:TRUE )

enum CHAOS_TYPE
{
	CHAOS_TYPE_DEFAULT = 0x1,
	CHAOS_TYPE_DEVILSQUARE = 0x2,
	CHAOS_TYPE_UPGRADE_10 = 0x3,
	CHAOS_TYPE_UPGRADE_11 = 0x4,
	CHAOS_TYPE_UPGRADE_12 = 0x16,
	CHAOS_TYPE_UPGRADE_13 = 0x17,
	CHAOS_TYPE_DINORANT = 0x5,
	CHAOS_TYPE_FRUIT = 0x6,
	CHAOS_TYPE_SECOND_WING = 0x7,
	CHAOS_TYPE_BLOODCATLE = 0x8,
	CHAOS_TYPE_FIRST_WING = 0xb,
	CHAOS_TYPE_SETITEM = 0xc,
	CHAOS_TYPE_DARKHORSE = 0xd,
	CHAOS_TYPE_DARKSPIRIT = 0xe,
	CHAOS_TYPE_CLOAK = 0x18,
	CHAOS_TYPE_BLESS_POTION = 0xf,
	CHAOS_TYPE_SOUL_POTION = 0x10,
	CHAOS_TYPE_LIFE_STONE = 0x11,
	CHAOS_TYPE_CASTLE_SPECIALMIX = 0x12,
	CHAOS_TYPE_HT_BOX = 0x14,
	CHAOS_TYPE_FENRIR_01 = 0x19,
	CHAOS_TYPE_FENRIR_02 = 0x1a,
	CHAOS_TYPE_FENRIR_03 = 0x1b,
	CHAOS_TYPE_FENRIR_04 = 0x1c,
	CHAOS_TYPE_COMPOUNDPOTION_LV1 = 0x1e,
	CHAOS_TYPE_COMPOUNTPOTION_LV2 = 0x1f,
	CHAOS_TYPE_COMPOUNTPOTION_LV3 = 0x20,
	CHAOS_TYPE_JEWELOFHARMONY_PURITY = 0x21,
	CHAOS_TYPE_JEWELOFHARMONY_MIX_SMELTINGITEM = 0x22,
	CHAOS_TYPE_JEWELOFHARMONY_RESTORE_ITEM = 0x23,
	CHAOS_TYPE_380_OPTIONITEM = 0x24,
	CHAOS_TYPE_ILLUSIONMIX	= 0x25,
	CHAOS_TYPE_CONDOR_FEATHER = 0x26,
	CHAOS_TYPE_3RDWING_MIX = 0x27,
	CHAOS_TYPE_LOTTERY_MIX = 0x28,
	CHAOS_TYPE_CHERRY_BLOSSOM = 0x29,
	CHAOS_TYPE_SEEDEXTRACT = 0x2A,
	CHAOS_TYPE_SPHEREMIX = 0x2B,
	CHAOS_TYPE_SPHEREMOUNT = 0x2C,
	CHAOS_TYPE_SPHEREUNMOUNT = 0x2D,

	CHAOS_TYPE_UPGRADE_14 = 0x31,
	CHAOS_TYPE_UPGRADE_15 = 0x32,
	
	CHAOS_TYPE_DUNGEON_KEYITEM					= 46,
	CHAOS_TYPE_GOLD_AND_SILBER_BOX				= 47,

	CHAOS_TYPE_LUCKYSYSTEM1 = 0x33,
	CHAOS_TYPE_LUCKYSYSTEM2 = 0x34,
	
	CHAOS_TYPE_SNAKEBEGINS = 48,

	CHAOS_TYPE_ARCA_BOOTY = 54,
	CHAOS_TYPE_ARCA_REGMARK = 55,

	CHAOS_TYPE_25WINGMIX = 56,
	CHAOS_TYPE_SOCKETMIX1 = 57,

#if (CUSTOM_WINGS == 1)
	CHAOS_TYPE_4THWINGS		= 0x64,
#endif

#ifdef __NOVUS__
	CHAOS_TYPE_RAREITEM		= 0x67,
#endif
};


struct PMSG_CHAOSMIXRESULT
{
	PBMSG_HEAD h;	// C1:86
	BYTE Result;	// 3
	BYTE ItemInfo[MAX_ITEM_INFO];	// 4
};





struct CB_PMSG_REQ_PRESENT_RESULT
{
	PBMSG_HEAD h;
	char AccountId[10];	// 3
	char GameId[10];	// D
	int Sequence;	// 18
	char present;	// 1C
	char presentname[50];	// 1D
	int Seq_present;	// 50
};


struct CBUPS_ITEMPRIZESEND	// Confirmed size
{
	PHEADB PHeader;
	char szAccountID[10];
	char szCharName[10];
	int iGameServerCode;
	BYTE btResult;
	int unk20;

};



#define CB_ERROR					0
#define CB_SUCCESS					1
#define CB_NOT_ENOUGH_ZEN			2
#define CB_TOO_MANY_ITEMS			3
#define CB_LOW_LEVEL_USER			4
#define CB_LACKING_MIX_ITEMS		6
#define CB_INCORRECT_MIX_ITEMS		7
#define CB_INVALID_ITEM_LEVEL		8
#define CB_USER_CLASS_LOW_LEVEL		9
#define CB_NO_BC_CORRECT_ITEMS		10
#define CB_BC_NOT_ENOUGH_ZEN		11
#define CB_IMPROPER_MATERIALS		251
#define CB_PSHOP_OPEN				253
#define CB_ALREADY_OPEN				254


class CChaosBox
{
public:
	CChaosBox();
	virtual ~CChaosBox();

	BOOL ChaosBoxCheck(LPOBJ lpObj);
	BOOL ChaosBoxInit(LPOBJ lpObj);
	BOOL ChaosBoxItemDown(LPOBJ lpObj);
	int ChaosBoxMix(LPOBJ lpObj, int & Result2, int & Result3);
	void CBUPS_ItemRequest(int aIndex, char* AccountId, char* Name);
	void ChaosEventProtocolCore(BYTE protoNum, unsigned char* aRecv, int aLen);
	void CBUPR_ItemRequestRecv( CB_PMSG_REQ_PRESENT_RESULT * lpMsg);
	int  CheckDevilSquareItem(LPOBJ lpObj, int & eventitemcount, int & itemlevel);
	void LogDQChaosItem(LPOBJ lpObj);
	BOOL DevilSquareEventChaosMix(LPOBJ lpObj, BOOL bCheckType, int iItemLevel);
	void LogPlusItemLevelChaosItem(LPOBJ lpObj, int iPlusMixLevel);
	void LogChaosItem(LPOBJ lpObj, LPSTR sLogType);
	BOOL PlusItemLevelChaosMix(LPOBJ lpObj, int mixType);
	BOOL PegasiaChaosMix(LPOBJ lpObj);
	BOOL CircleChaosMix(LPOBJ lpObj);
	BOOL WingChaosMix(LPOBJ lpObj);
	void DefaultChaosMix(LPOBJ lpObj);
	void DevilSquareItemChaosMix(LPOBJ lpObj);
	void BloodCastleItemChaosMix(LPOBJ lpObj);
	void SetItemChaosMix(LPOBJ lpObj);
	void DarkHorseChaosMix(LPOBJ lpObj);
	void DarkSpiritChaosMix(LPOBJ lpObj);
	void BlessPotionChaosMix(LPOBJ lpObj);
	void SoulPotionChaosMix(LPOBJ lpObj);
	void LifeStoneChaosMix(LPOBJ lpObj);
	void HiddenTreasureBoxItemMix(LPOBJ lpObj);
	void Fenrir_01Level_Mix(LPOBJ lpObj);
	void Fenrir_02Level_Mix(LPOBJ lpObj);
	void Fenrir_03Level_Mix(LPOBJ lpObj);
	void Fenrir_04Upgrade_Mix(LPOBJ lpObj);
	void ShieldPotionLv1_Mix(LPOBJ lpObj);
	void ShieldPotionLv2_Mix(LPOBJ lpObj);
	void ShieldPotionLv3_Mix(LPOBJ lpObj);
	void CondorFeather_Mix(LPOBJ lpObj);
	void NewWingChaos_Mix(LPOBJ lpObj);
	void LotteryItemMix(LPOBJ lpObj);
	void CherryBlossomMix(LPOBJ lpObj);
	void IllusionTemple_Mix(LPOBJ lpObj);
	void SeedExtractMix(LPOBJ lpObj);
	void SeedSphereCompositeMix(LPOBJ lpObj);
	void SetSeedSphereMix(LPOBJ lpObj,BYTE Pos);
	void RemoveSeedSphereMix(LPOBJ lpObj,BYTE Pos);
	void SecromiconMix(LPOBJ lpObj);
	void LuckySystemItemCreate(LPOBJ lpObj);
	void LuckySystemJewelCreate(LPOBJ lpObj);
	void GoldenNSilverBoxMix(LPOBJ lpObj);
	void Wing25Mix(LPOBJ lpObj);
	void SocketItemMix(LPOBJ lpObj,int iMixId);

#if (CUSTOM_WINGS == 1)
	void Create4thWings(LPOBJ lpObj);
#endif

#ifdef __NOVUS__
	void RareItem(LPOBJ);
#endif

	int IsPlusItemJewel(int iType);
    void ChaosBoxSpecialItemDown(LPOBJ lpObj);
	int IsCondorItem(int iType);
	int Is1stLevelWing(int iType);
	int Is2ndLevelWing(int iType);

	int CheckItemOptions(CItem* lpItem,short iLevel,BYTE iOption1,BYTE iOption2,BYTE iOption3,BYTE iSetOption,BYTE iExcOption);
	void ChaosBoxWingMixItemDown(LPOBJ lpObj);
	void RandomLevelDown(CItem* lpItem);

#if (GS_CASTLE == 1)
	void CastleSpecialItemMix(LPOBJ lpObj);
#endif


protected:
// Member variables
	int iChaosJewel;
	int iBlessJewel;
	int iSoulJewel;
	int iCreationJewel;
	int iBlessPack;
	int iSoulPack;
	int iCondorFeather;
	int iCondorStone;
	int iStormWings;
	int iRedemptionWings;
	int iFortitudeWings;
	int iHurricaneWings;
	int iMonarchMantle;
	int iDimensionWings;
	int iRFCape2;
	int iOptionRate;
};

extern CChaosBox g_ChaosBox;

#endif