#if !defined(__SOCKET_ITEM_OPTION_H__)
#define __SOCKET_ITEM_OPTION_H__

#include "..\common\zzzitem.h"
#include "TRandomPoolMgr.h"

#define MAX_SEED_TYPE		6
#define MAX_SEED_OPTION		50
#define MAX_SPHERE_LEVEL		5
#define MAX_BONUS_OPTION	6
#define MAX_SET_OPTION		2

// Seed Option Data Struct Class
struct _SOCKET_SEED_DATA
{
	BYTE btOption;
	BYTE btGroup;
	BYTE btIndex;

	BYTE btIncType;
	BYTE btIncValue[MAX_SPHERE_LEVEL];

	int SeedItem;
	int OptionRate;

	_SOCKET_SEED_DATA();
	~_SOCKET_SEED_DATA();

	void Clear();
	BYTE CheckSeed(CItem* lpItem);
	void SetOptionData(BYTE btOption, BYTE btGroup, BYTE btLevel, BYTE btIncType,LPBYTE btIncValue, int unk1);
};


// Spear Level Data Struct Class
struct _SOCKET_SPHERE_LEVEL
{
	BYTE btLevel;

	int Value;
	int SeedItem;

	_SOCKET_SPHERE_LEVEL();
	~_SOCKET_SPHERE_LEVEL();

	void Clear();
	void SetSphereLevel(BYTE btLevel,DWORD Value);
};

// Spear Option Data Struct Class
struct _SOCKET_SPHERE_OPTION
{
	BYTE btOptionId;
	BYTE btOption;
	BYTE btGroup;

	int SeedItem;

	BYTE btIndex;
	BYTE btIncType;

	WORD wSphereValue;
	BYTE btLevel;

	_SOCKET_SPHERE_OPTION();
	~_SOCKET_SPHERE_OPTION();

	int GetSphereItem();
	void Clear();
	BYTE SetSphereOption(_SOCKET_SEED_DATA* lpSeedData,BYTE btLevel);
	void IncValue(BYTE btValue);
};


// Socket Item Bonus Struct
struct _SOCKET_BONUS_DATA
{
	BYTE btIndex;
	BYTE btEffectType;
	BYTE btIndexMin;
	BYTE btIndexMax;
	BYTE btOptionType;
	BYTE btOptionValue;

	int OptionRate;

	BYTE btValue[MAX_SOCKET_COUNT];
	BYTE btCount;

	void Clear()
	{
		this->btIndex = 0;
		this->btEffectType = 0;
		this->btIndexMin = 0;
		this->btIndexMax = 0;
		this->btOptionType = 0;
		this->btOptionValue = 0;

		this->OptionRate = 0;

		for(int i = 0; i < MAX_SOCKET_COUNT; i++)
		{
			this->btValue[i] = 0;
		}

		this->btCount = 0;
	}
};

struct _SOCKET_SET_DATA
{
	BYTE btIndex;
	BYTE btIncType;
	BYTE btIncValue;
	BYTE btEffect;

	int OptionRate;

	BYTE SeedCount[MAX_SEED_TYPE];
};

struct _SOCKET_USER_DATA
{
	BYTE btOption;	// 210A
	BYTE IncType;	// 210B
	WORD IncValue;	// 210C
	BYTE Count;		// 210E

	void Clear()
	{
		btOption = -1;
		IncValue = 0;
		IncType = 0;
		Count = 0;
	}
};

struct SocketItemInfo //Custom
{
	WORD	ItemType;
	WORD	ItemIndex;
};

// Socket Option Item Class declaration
class CSocketItems
{
public:

	// Constructor / Destructor 
	CSocketItems();
	virtual ~CSocketItems();

// Member functions
	void Init();
	BYTE LoadData(char* szFileName);
	void LoadOptions();

	bool IsSocketItem(CItem* lpItem);
	bool IsSocketItem(int ItemIndex);

	BYTE GetEmptySocketCount(CItem* lpItem);
	void GetItemOptions(CItem* lpItem,LPBYTE SocketOp,LPBYTE SocketId);
	bool CheckSocketOption(CItem* lpItem);

	BYTE IsSeedItem(int ItemType);
	BYTE IsSphereItem(int ItemType);
	BYTE IsSeedSphereItem(int ItemType);
	BYTE IsItemType(CItem* lpItem,BYTE SeedOption);
	BYTE IsDropMap(int MapNumber);
	BYTE IsOptionSet(_SOCKET_SPHERE_OPTION* lpSphereData,BYTE btOption,BYTE btLevel);

	BYTE GetSeedOption(int SeedItem,BYTE SeedIndex);
	BYTE GetSphereLevel(int SpeerItem);
	BYTE GetSphereOption(WORD Type,BYTE btLevel);
	BYTE GetOptionGroup(BYTE SeedOption);
	_SOCKET_SEED_DATA* GetSeedOption(BYTE btOption);
	_SOCKET_SPHERE_LEVEL* GetSphereLevelOption(BYTE btLevel);
	_SOCKET_BONUS_DATA* GetBonusOption(BYTE btBonusOption);
	BYTE GetOptionRate();
	int GetResultValue(int Value,WORD IncValue,BYTE IncType);

	BYTE CheckMountItem(CItem* lpItem,WORD Type,BYTE btLevel);

	int ApplySocketOptions(OBJECTSTRUCT* lpObj);
	void ApplyBonusOptions(OBJECTSTRUCT* lpObj);
	void ApplySetOptions(OBJECTSTRUCT* lpObj);
	BYTE SetApplySocketItem(OBJECTSTRUCT* lpObj);
	void InitUserOptions(OBJECTSTRUCT* lpObj);
	void InitUserSocketOptions(OBJECTSTRUCT* lpObj);
	void SetSocketOption(OBJECTSTRUCT* lpObj,int Type,BYTE btOption,BYTE IncType,WORD IncValue);
	void SetSocketOption(CItem* lpItem);

	void ApplyFireOption(OBJECTSTRUCT* lpObj,BYTE btOption,BYTE IncType,WORD IncValue);
	void ApplyWaterOption(OBJECTSTRUCT* lpObj,BYTE btOption,BYTE IncType,WORD IncValue);
	void ApplyIceOption(OBJECTSTRUCT* lpObj,BYTE btOption,BYTE IncType,WORD IncValue);
	void ApplyWindOption(OBJECTSTRUCT* lpObj,BYTE btOption,BYTE IncType,WORD IncValue);
	void ApplyLightningOption(OBJECTSTRUCT* lpObj,BYTE btOption,BYTE IncType,WORD IncValue);
	void ApplyEarthOption(OBJECTSTRUCT* lpObj,BYTE btOption,BYTE IncType,WORD IncValue);
	void ApplyUnk1Option(OBJECTSTRUCT* lpObj,BYTE btOption,BYTE IncType,WORD IncValue);
	void ApplySetOption(OBJECTSTRUCT* lpObj,BYTE btOption,BYTE IncType,WORD IncValue);

	DWORD GetOptionValue(BYTE btOption);
	int GetSphereDrop(int Level);
	void ShopSocketItem(CItem* lpItem,BYTE SocketCount);
	BYTE GetSocketCount(int ItemType);
	BYTE ItemSetSeedSphere(CItem* lpItem,BYTE btOption,BYTE btPos);
	BYTE ItemRemoveSeedSphere(CItem* lpItem,BYTE btPos);
	BYTE SetBonusOption(CItem* lpItem);
public:
// Member variables
	_SOCKET_SEED_DATA		m_SeedOption[MAX_SEED_OPTION];
	_SOCKET_SPHERE_LEVEL	m_SphereLevel[MAX_SPHERE_LEVEL];
	_SOCKET_BONUS_DATA		m_BonusOption[MAX_BONUS_OPTION];
	_SOCKET_SET_DATA		m_SetOption[MAX_SET_OPTION];

	TRandomPoolMgr			m_OptionRnd;
	TRandomPoolMgr			m_SocketType1Rnd;
	TRandomPoolMgr			m_SocketType2Rnd;

	int						m_SocketType1Rate[MAX_SOCKET_COUNT];
	int						m_SocketType2Rate[MAX_SOCKET_COUNT];

	int						m_bSphereDrop;

	int						m_SphereDropRate[MAX_SOCKET_COUNT];
	int						m_SphereDropLevel[MAX_SOCKET_COUNT];

	std::vector<SocketItemInfo> m_SocketItemList;
};

extern CSocketItems g_SocketItem;

extern int g_SeedExtractMoney;
extern int g_SphereCompositeMoney;
extern int g_SphereSetMoney;
extern int g_SphereRemoveMoney;

extern int g_SocketRecipe_DropLevel;
extern int g_SocketRecipe_DropRate;
extern int g_SocketMaterial_Lv1;
extern int g_SocketMaterial_Lv2;

#endif