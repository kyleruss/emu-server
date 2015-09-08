#include "stdafx.h"
#include "ItemExcellentOption.h"
#include "..\pugixml\pugixml.hpp"
#include "GameMain.h"

using namespace pugi;
ItemExcellentOption g_ItemExcellentOption;

void ItemExcellentOption::Load() {
	Init();
	Read(gDirPath.GetNewPath(FILE_ITEM_EXCELLENT_OPTION));
}

void ItemExcellentOption::Init() {
	m_ItemInfo.clear();
	if (m_ItemInfo.capacity() > 0) {
		std::vector<ItemExcellentOptionItemInfo>().swap(m_ItemInfo);
	}
}

void ItemExcellentOption::Read(const char* File) {
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok) {
		MsgBox("[ItemExcellentOption] File %s not found!", File);
		return;
	}

	xml_node FileRoot = Document.child("itemexcellentoption");

	for (xml_node Node = FileRoot.child("item"); Node; Node = Node.next_sibling()) {
		ItemExcellentOptionItemInfo lpItemInfo;
		lpItemInfo.m_ItemType = Node.attribute("type").as_int(-1);
		lpItemInfo.m_ItemIndex = Node.attribute("index").as_int(-1);
		for (xml_node SubNode = Node.child("option"); SubNode; SubNode = SubNode.next_sibling()) {
			ItemExcellentOptionInfo lpOptionInfo;
			lpOptionInfo.m_OptionIndex = (BYTE)SubNode.attribute("index").as_int((BYTE)-1);
			lpOptionInfo.m_OptionValue = SubNode.attribute("value").as_int(-1);
			lpItemInfo.m_OptionInfo.push_back(lpOptionInfo);
		}
		m_ItemInfo.push_back(lpItemInfo);
	}
}

int ItemExcellentOption::getOptionValue(WORD ItemCode, BYTE OptionIndex) {
	for (int i = 0; i < m_ItemInfo.size(); i++) {
		// select item
		if (ITEMGET(m_ItemInfo[i].m_ItemType, m_ItemInfo[i].m_ItemIndex) != ItemCode) {
			continue;
		}
		
		for (int n = 0; n < m_ItemInfo[i].m_OptionInfo.size(); n++) {
			// select option
			if (m_ItemInfo[i].m_OptionInfo[n].m_OptionIndex == OptionIndex) {
				LogAddDebug("[%d:%d] Option: %d, Value: %d",
					m_ItemInfo[i].m_ItemType, m_ItemInfo[i].m_ItemIndex,
					m_ItemInfo[i].m_OptionInfo[n].m_OptionIndex,
					m_ItemInfo[i].m_OptionInfo[n].m_OptionValue);
				return m_ItemInfo[i].m_OptionInfo[n].m_OptionValue;
			}
		}
	}
	// not found
	return -1;
}