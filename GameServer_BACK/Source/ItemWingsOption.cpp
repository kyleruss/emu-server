#include "stdafx.h"
#include "ItemWingsOption.h"
#include "..\pugixml\pugixml.hpp"

ItemWingsOption g_ItemWingsOption;
using namespace pugi;

void ItemWingsOption::Load() {
	Init();
	Read(FILE_ITEM_WINGS_OPTION);
}

void ItemWingsOption::Init() {
	m_OptionInfo.clear();
	if (m_OptionInfo.capacity() > 0) {
		std::vector<ItemWingsOptionInfo>().swap(m_OptionInfo);
	}
}

void ItemWingsOption::Read(const char* File) {
	
}

