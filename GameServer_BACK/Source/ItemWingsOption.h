#pragma once

#include <vector>

struct ItemWingsOptionInfo {
	BYTE m_ItemCategory;
	short m_ItemIndex;
	
	BYTE m_ChanceIgnoreOpDefense; //ObjBaseAttack.cpp:950
	
};

class ItemWingsOption {
public:
	ItemWingsOption() { };
	~ItemWingsOption() { };

	void Load();
	void Init();
	void Read(const char* File);

	bool isWings(WORD ItemCode);
	ItemWingsOptionInfo* getWings(WORD ItemCode);
private:
	std::vector<ItemWingsOptionInfo> m_OptionInfo;
};

extern ItemWingsOption g_ItemWingsOption;