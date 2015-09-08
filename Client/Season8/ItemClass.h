#pragma once

#define MAX_ITEM_INFO	12

struct NEWOPTION_TEXT
{
	WORD wOption;
	char szOptionText[70];
};

#pragma pack(push, 1)
struct FRGB
{
	float r, g, b;
};
#pragma pack(pop)

#define pLoadModel		((void(__cdecl*)(DWORD ResourceID, char * Path, char * File, int Arg4)) 0x006094FF)
#define pInitModel		((void(__cdecl*)()) 0x00615F1D)
#define oInitModelCall	0x0062528B
#define pLoadTexture	((void(__cdecl*)(int TextureID, char * Folder, int GLREPEAT, int GLNEAREST, int GLTRUE)) 0x00608F69)
#define pInitText		((void(__cdecl*)()) 0x0060BB47)
#define oInitTextCall	0x00625246

#define ITEM2(x, y)		(x * 512 + y + ITEM_START)

void LoadModels();
void LoadTextures();

void MySetSpecialEffect(LPBYTE arg1,DWORD arg2,float* arg3,LPBYTE arg4,DWORD arg5,DWORD arg6,LPBYTE arg7,LPBYTE arg8,DWORD arg9,DWORD arg10,DWORD arg11);
void MySetSpecialEffect2(LPBYTE arg1,LPBYTE arg2,DWORD arg3,LPBYTE arg4,DWORD arg5,DWORD arg6,DWORD arg7);
void ItemConvertEx(LPBYTE itemStruct,DWORD Op1,DWORD Op2,DWORD Op3);
BYTE IsItemSpecial(LPBYTE Item);
void myGlow(DWORD dwItemId, DWORD uk1, DWORD uk2, FRGB* cl, BYTE bUkn);
void myGlow2(DWORD dwItemId, DWORD uk1, DWORD uk2, FRGB* cl, BYTE bUkn);
BOOL SetItemGlow(int itemID,FRGB* cl);
BYTE GetExcText(DWORD Id,char* lpszDest,WORD wOptionId,BYTE btValue);

typedef bool (__thiscall* pMAIN_00544D37)(LPBYTE,DWORD,DWORD,float,DWORD,float,float,float,DWORD);
extern pMAIN_00544D37 MAIN_00544D37;

typedef bool (__thiscall* pMAIN_00547964)(LPBYTE,DWORD,float,DWORD,float,float,float,DWORD,DWORD);
extern pMAIN_00547964 MAIN_00547964;

typedef void(__thiscall* pMAIN_00543627)(LPBYTE pThis,DWORD pClass,float* pFloat1,float* pFloat2,DWORD dwNumber);
extern pMAIN_00543627 MAIN_00543627;

typedef void(*pMAIN_0077C1A8)(DWORD,float*,float,float*,LPBYTE,float,DWORD);
extern pMAIN_0077C1A8 MAIN_0077C1A8;

typedef void(*pMAIN_00752200)(DWORD,float*,LPBYTE,float*,DWORD,float,DWORD);
extern pMAIN_00752200 MAIN_00752200;

typedef LPBYTE (__thiscall* pGetlpItem)(DWORD pThis,DWORD ItemID);
extern pGetlpItem GetlpItem;

typedef LPBYTE(__thiscall* pItemGetInfo)(LPBYTE);
extern pItemGetInfo ItemGetInfo;

typedef LPBYTE(__thiscall* pCharSetGetStruct)(LPBYTE,DWORD);

#define impGetSpecialOptionValue ((LPBYTE(__thiscall*)(LPVOID This, LPBYTE ItemStruct, int OptionIndex)) 0x0096ED52)
extern LPBYTE __fastcall GetSpecialOptionValue(LPVOID This, LPVOID EDX, LPBYTE ItemStruct, int OptionIndex);

void __FixExchangeSymbolPrice();
void _MySetSetEffect();
void __FixExcellentSocket();

#if (__CUSTOM_WINGS__ == 1)
	BOOL Is4thWing(DWORD ItemID);
	void MyObjectPreview(DWORD arg1,LPBYTE arg2,LPBYTE arg3,LPBYTE arg4);

	void __Fix4thWingsDisplay();
	void __Fix4thWingsPos();
	void __Fix4thWingsPrice1();
	void __Fix4thWingsPrice2();
	void __Fix4thWingsPrice3();
	void __Fix4thWingsHPOption();
#endif