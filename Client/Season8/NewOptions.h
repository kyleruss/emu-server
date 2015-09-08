#pragma once

#define MAX_ITEMS	16*512

typedef LPBYTE(__thiscall*pMainGetExcOpt)(LPBYTE,LPBYTE,int);
extern pMainGetExcOpt MainGetExcOpt;

#if (__NEWOPTION__ == 1)

struct __ENCHANT_BONUS
{
	DWORD Offset15;
	DWORD Offset14;
	DWORD Offset13;
	DWORD Offset12;
	DWORD Offset11;
	DWORD Offset10;
};

#pragma pack(push)
#pragma pack(1)

struct CUSTOM_OPTIONS_EXHEADER
{
	int iOptionCount;
	int iEnchantOptCount;
	int iExcellentOptCount;
	int iItemCount;
};

struct CUSTOM_OPTIONS_NAME
{
	int iIndex;
	char szName[60];
};

struct CUSTOM_OPTIONS_ENCHANT
{
	int iId;
	int iIndex1[6];
	int iValue1[6];
	int iIndex2[6];
	int iValue2[6];
};

struct CUSTOM_OPTIONS_EXCELLENT
{
	int iId;
	int iValue[6];
};

struct CUSTOM_OPTIONS_LIST
{
	int Item;
	short EnchantId;
	short ExcellentId;
};

#pragma pack(pop)

class CNewOptions
{
public:
	CNewOptions();
	virtual ~CNewOptions();

public:
	BOOL LoadData();


	CUSTOM_OPTIONS_EXCELLENT* ExcellentOptions(int iItemID);
	CUSTOM_OPTIONS_ENCHANT* EnchantOptions(int iItemID);
	BOOL IsChangeOption(WORD wOption);
	BOOL GetNewOption(int iItemID,char* lpszText,WORD wOption);
	LPBYTE GetExcellentOption(LPBYTE ItemStruct,int OptionID);
};
extern CNewOptions gNewOptions;

void __FixWizRise();
void __FixWizRise2();
void __GetLvl15Option();

extern __ENCHANT_BONUS pBonusOffsets[8];

#endif