#pragma once 

#include <vector>
#include "ItemClass.h"

#pragma pack(push, 1)
struct ItemGlowInfo {
	WORD ItemCode;
	BYTE R;
	BYTE B;
	BYTE G;
};
#pragma pack(pop)

class ItemGlow {
public:
	void Load();
	void Read(const char *File);
	void Init();
	static void Refresh();
	bool setColor(WORD ItemCode, FRGB *ColorBuffer);

//private:
	std::vector<ItemGlowInfo> m_GlowInfo;
}; extern ItemGlow g_ItemGlow;