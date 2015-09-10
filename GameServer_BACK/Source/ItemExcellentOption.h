#pragma once

#include <vector>

struct ItemExcellentOptionInfo {
	ItemExcellentOptionInfo() {
		m_OptionIndex = (BYTE)-1;
		m_OptionValue = (int)-1;
	}
	BYTE m_OptionIndex;
	int m_OptionValue;
};


struct ItemExcellentOptionItemInfo {
	BYTE m_ItemType;
	short m_ItemIndex;
	std::vector<ItemExcellentOptionInfo> m_OptionInfo;
};

class ItemExcellentOption {
public:
	ItemExcellentOption() { };
	~ItemExcellentOption() { };

	void Load();
	void Init();
	void Read(const char* File);

	int getOptionValue(WORD ItemCode, BYTE OptionIndex);

private:
	std::vector<ItemExcellentOptionItemInfo> m_ItemInfo;
};

extern ItemExcellentOption g_ItemExcellentOption;