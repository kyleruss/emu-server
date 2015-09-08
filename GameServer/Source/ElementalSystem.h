#if !defined(__ELEMENTALSYSTEM_H__)
#define __ELEMENTALSYSTEM_H__

#include "..\common\zzzitem.h"

#pragma pack(push)
#pragma pack(1)

struct PMSG_SEND_ERTELINFO
{
	PWMSG_HEAD2 h;
	BYTE btResult;
	BYTE btCount;
	BYTE btType;
};

struct ERTEL_OPTION_INFO
{
	BYTE OptionID;
	BYTE OptionLevel;
};

struct PMSG_ERTELINFO
{
	BYTE btType;
	BYTE btId;
	BYTE btAttribute;
	BYTE btIndex;
	WORD wNumber;
	BYTE btUnk4;
	ERTEL_OPTION_INFO Option[5];
};

#pragma pack(pop)

struct PMSG_ANS_INSERTERTEL
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	PMSG_ERTELINFO ErtelInfo;
};

struct PMSG_ERTEL_RESULT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_REQ_INSERTERTEL
{
	PBMSG_HEAD2 h;
	int iTarget;
	int iSource;
};

struct PMSG_REQ_REMOVEERTEL
{
	PBMSG_HEAD2 h;
	int iPentagramPos;
	BYTE ErtelPos;
};

struct PMSG_ANS_REMOVEERTEL
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btPos;
	BYTE btType;
};

struct PMSG_REQ_ERTELLIST
{
	PBMSG_HEAD h;
	int aIndex;
	char szAccount[10];
	char szName[10];
};

struct PMSG_ANS_ERTELLIST
{
	PWMSG_HEAD h;
	int aIndex;
	char szName[10];

	BYTE ErtelList1[700];
	BYTE ErtelList2[700];
};

struct PMSG_SAVE_ERTELLIST
{
	PWMSG_HEAD h;
	char szAccount[10];
	char szName[10];

	BYTE ErtelList1[700];
	BYTE ErtelList2[700];
};

struct PMSG_ERTEL_COMBINE
{
	PBMSG_HEAD2 h;
	BYTE MixId;
};

struct PMSG_ERTEL_LEVELUP
{
	PBMSG_HEAD2 h;
	BYTE btType;
	BYTE btLevel;
};

#define MAX_ERTEL		100
#define DB_ERTELSIZE	7
#define DB_ERTELID		0
#define DB_ATTRIBUTE	1
#define DB_ERTELOP1		2
#define DB_ERTELOP2		3
#define DB_ERTELOP3		4
#define DB_ERTELOP4		5
#define DB_ERTELOP5		6

#define ERTEL_INVENTORY	0
#define ERTEL_WAREHOUSE	1
#define ERTEL_TRADE		2
#define	ERTEL_STORE		3

enum
{
	ELEMENTAL_MIX_MITHRIL	= 1,
	ELEMENTAL_MIX_ELIXIR	= 2,
	ELEMENTAL_MIX_ERRTEL	= 3,
	ELEMENTAL_MIX_DECOMPOSE1	= 4,
	ELEMENTAL_MIX_DECOMPOSE2	= 5,
	ELEMENTAL_MIX_DECOMPOSE3	= 6,
	ELEMENTAL_MIX_DECOMPOSE4	= 7,
	ELEMENTAL_MIX_DECOMPOSE5	= 8,
	ELEMENTAL_MIX_DECOMPOSE6	= 9,
	ELEMENTAL_MIX_DECOMPOSE7	= 10,
	ELEMENTAL_MIX_DECOMPOSE8	= 11,
	ELEMENTAL_MIX_DECOMPOSE9	= 12,
	ELEMENTAL_MIX_DECOMPOSE10	= 13,
	ELEMENTAL_MIX_DECOMPOSE11	= 14,
	ELEMENTAL_MIX_DECOMPOSE12	= 15,
	ELEMENTAL_MIX_DECOMPOSE13	= 16,
	ELEMENTAL_MIX_DECOMPOSE14	= 17,
	ELEMENTAL_MIX_DECOMPOSE15	= 18,
};

enum
{
	ELEMENTAL_FAIL1 = 0xF7,
	ELEMENTAL_FAIL2 = 0xE1,
	ELEMENTAL_FAIL3	= 0xE2,
	
	ELEMENTAL_NOZEN	= 0xF9,
	ELEMENTAL_NOMATERIAL = 0xFA,
	ELEMENTAL_IMPROPER	= 0xFB,
};

enum
{
	ELEMENT_NONE = 0,
	ELEMENT_FIRE = 1,
	ELEMENT_WATER = 2,
	ELEMENT_EARTH = 3,
	ELEMENT_WIND = 4,
	ELEMENT_DARK = 5,
};

#define MAX_ERTEL_OPTION	100
#define MAX_ERTEL_LEVEL		11

#define MAX_PENTAGRAM_ITEMS		15
#define MAX_PENTAGRAM_OPTIONS	7

struct ERTEL_OPTION
{
	BYTE btId;
	WORD ItemID;
	BYTE btRank;
	BYTE btOption;
	WORD OptionRate;
	WORD wType;
	WORD Option[MAX_ERTEL_LEVEL];
	WORD Rate[MAX_ERTEL_LEVEL];
	BYTE btIncType;
};

struct PENTAGRAM_OPTION
{
	short m_itemId;
	BYTE  m_btOption[9];
};

struct PENTAGRAM_OPTION_REQ
{
	BYTE m_btErtelType;
	BYTE m_btRankReq;
	BYTE m_btLevelReq;
};

struct PENTAGRAM_OPTION_INFO
{
	BYTE m_btOptionID;
	PENTAGRAM_OPTION_REQ m_Reqs[3];
};

struct ELEMENT_CLASS_INFO
{
	WORD strMin;
	WORD strMax;
	WORD dexMin;
	WORD dexMax;
	WORD vitMin;
	WORD vitMax;
	WORD eneMin;
	WORD eneMax;
	WORD cmdMin;
	WORD cmdMax;
	WORD dexDef;

	float DefRate_LEVELMUL;
	float DefRate_MUL[5];
	float AttRate_LEVELMUL;
	float AttRate_MUL[5];

	float DefRate_LEVELMUL_PVP;
	float DefRate_MUL_PVP[5];
	float AttRate_LEVELMUL_PVP;
	float AttRate_MUL_PVP[5];
};

#include "ObjBaseAttack.h"

class CElementalSystem : public CObjBaseAttack
{
public:
	CElementalSystem();
	virtual ~CElementalSystem();

public:

	void LoadElementalInfo(char* szFileName);
	void LoadPentagramOption(char* szFileName);
	void LoadOptions();


	BOOL IsElementalItem(int iType);
	BOOL IsElementalItem(CItem* lpItem);
	BOOL IsPentagram(int iType);
	BOOL IsPentagram(CItem* lpItem);
	BOOL IsErrtel(int iType);
	BOOL IsErrtel(CItem* lpItem);

	BOOL Drop(LPOBJ lpObj,LPOBJ lpTargetObj);

	void InitElementOptions(LPOBJ lpObj);
	void ApplyElementItem(LPOBJ lpObj);
	void ApplyErtelOption(LPOBJ lpObj,int iErtelID);
	void ApplyOption(LPOBJ lpObj,int iOptionID,int Level);
	void ApplyBonusOptions(LPOBJ lpObj,CItem* lpPentagram);
	int GetIncValue(int Value,int Add,int Type);

	BOOL MissCheck(LPOBJ lpObj, LPOBJ lpTargetObj);
	BOOL MissCheckPvP(LPOBJ lpObj, LPOBJ lpTargetObj);

	//BOOL Attack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf* lpMagic);

	BOOL Attack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf* lpMagic);

	int GetTargetDefense(LPOBJ lpObj,LPOBJ lpTargetObj,CMagicInf* lpMagic,BYTE btElement);
	int GetAttackDamage(LPOBJ lpObj,LPOBJ lpTargetObj,int targetdefense,CMagicInf* lpMagic,BYTE btElement,BYTE btTargetElement);

	void SendElementalItems(LPOBJ lpObj,int Type);
	void SendShopElementalItem(int aSourceIndex,int aTargetIndex);
	void SendTradeElementalItem(int aSourceIndex,int aTargetIndex);
	void BuildErtelInfo(PMSG_ERTELINFO* lpErtelInfo,ERTEL_USER* Ertel,int Id,int Type);

	void CGInsertErtel(int aIndex,PMSG_REQ_INSERTERTEL* lpMsg);
	void CGRemoveErtel(int aIndex,PMSG_REQ_REMOVEERTEL* lpMsg);

	BYTE InsertErtel(LPOBJ lpObj,CItem* ErtelItem);
	void ClearErtel(LPOBJ lpObj,CItem* TargetItem);
	void MoveErtel(LPOBJ lpObj,CItem* TargetItem,BYTE tFlag,unsigned char* siteminfo);
	void PShopErtel(LPOBJ lpShopObj,LPOBJ lpObj,CItem* TargetItem);
	void TradeErtel(LPOBJ lpTradeObj,LPOBJ lpObj,CItem* TargetItem);
	void RemoveErtel(LPOBJ lpObj,int iErtelID,int Type);

	BYTE GetSlotCount(CItem* pItem);
	BYTE GetErtelOption(int iErtel,BYTE btRank);
	ERTEL_OPTION* GetErtelOption(int iOptionID);
	ERTEL_USER* GetErtel(LPOBJ lpObj,int iErtelID,int Type);
	PENTAGRAM_OPTION* GetPentagram(int PentagramID);
	PENTAGRAM_OPTION_INFO* GetPentagramOption(int OptionID);
	int CheckBonusRequirement(LPOBJ lpObj,CItem* lpPentagram,PENTAGRAM_OPTION_INFO* OptionInfo);

	// Mix
	void CGCombineButtonClick(PMSG_ERTEL_COMBINE* lpMsg,int aIndex);
	void CGUpgradeButtonClick(PMSG_ERTEL_LEVELUP* lpMsg,int aIndex);
	void MithrilMix(LPOBJ lpObj);
	void ElixirMix(LPOBJ lpObj);
	void ErrtelMix(LPOBJ lpObj);
	void ErrtelLevelUp(LPOBJ lpObj);
	void ErrtelRankUp(LPOBJ lpObj,BYTE btRank);
	void PentagramMix(LPOBJ lpObj);

	// DB 
	void GDReqErtelList(int aIndex);
	void DGAnsErtelList(PMSG_ANS_ERTELLIST* lpMsg);
	void GDSaveErtelList(int aIndex);

public:
	ELEMENT_CLASS_INFO m_classinfo[MAX_TYPE_PLAYER];
	BYTE m_DamageTable[5][5];
	ERTEL_OPTION m_ErtelOption[MAX_ERTEL_OPTION];
	PENTAGRAM_OPTION m_PentagramItems[MAX_PENTAGRAM_ITEMS];
	PENTAGRAM_OPTION_INFO m_PentagramOptionInfo[MAX_PENTAGRAM_OPTIONS];


	TRandomPoolMgr m_OptionRate;
	TRandomPoolMgr m_SlotCountRate;

	int m_iSpiritMap_DropLevel;
	int m_iSpiritMap_DropRate;

	int m_iMithril_DropRate;
	int m_iElixir_DropRate;

	int m_iMurenBook_DropLevel;
	int m_iMurenBook_DropRate;

	int m_iEtramuScroll_DropLevel;
	int m_iEtramuScroll_DropRate;

	BYTE m_LicenseStep3;
};

extern CElementalSystem g_ElementalSystem;

#endif