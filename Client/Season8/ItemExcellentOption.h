#pragma once

#include <vector>

#define impGetSpecialOptionValue ((LPBYTE(__thiscall*)(LPVOID This, LPBYTE ItemStruct, int OptionIndex)) 0x0096ED52)

#pragma pack(push, 1)
struct ItemExcellentOptionItemInfo {
	short m_ItemCode;
	BYTE m_ItemOption[6];
};
#pragma pack(pop)

class ItemExcellentOption {
public:
	ItemExcellentOption() { };
	~ItemExcellentOption() { };

	void Load();
	void Init();
	void Read(const char* File);

	int getOptionScriptValue(WORD ItemCode, BYTE OptionIndex);
	static LPBYTE __fastcall getOptionValue(LPVOID This, LPVOID EDX, LPBYTE ItemStruct, int OptionIndex);

private:
	std::vector<ItemExcellentOptionItemInfo> m_ItemInfo;
};

extern ItemExcellentOption g_ItemExcellentOption;