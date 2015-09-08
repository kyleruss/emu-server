#include "stdafx.h"
#include "ItemExcellentOption.h"
#include "ScriptEncode.h"

ItemExcellentOption g_ItemExcellentOption;

void ItemExcellentOption::Load() {
	Init();
	Read("Data\\Custom\\ItemExcellentOption.z");
	SetOp((LPVOID)0x0096D7BA, getOptionValue, CALL);
	//SetOp((LPVOID)0x0096D742, getOptionValue, CALL);
	//SetOp((LPVOID)0x0096DA07, getOptionValue, CALL);
}

void ItemExcellentOption::Init() {
	m_ItemInfo.clear();
	if (m_ItemInfo.capacity() > 0) {
		std::vector<ItemExcellentOptionItemInfo>().swap(m_ItemInfo);
	}
}

void ItemExcellentOption::Read(const char* File) {
	FILE *Stream = fopen(File, "rb");

	if (Stream == NULL)	{
		MessageBoxA(NULL, "ItemExcellentOption.z not found", "ZTClient", ERROR);
		return;
	}

	int LineCount = 0;
	fread(&LineCount, sizeof(LineCount), 1, Stream);

	for (int i = 0; i < LineCount; i++) {
		ItemExcellentOptionItemInfo NewItem = { 0 };
		fread(&NewItem, sizeof(ItemExcellentOptionItemInfo), 1, Stream);
		ScriptEncode::Encode((char*)&NewItem, sizeof(ItemExcellentOptionItemInfo));
		m_ItemInfo.push_back(NewItem);
	}

	fclose(Stream);
}

int ItemExcellentOption::getOptionScriptValue(WORD ItemCode, BYTE OptionIndex) {
	for (int i = 0; i < m_ItemInfo.size(); i++) {
		if (m_ItemInfo[i].m_ItemCode == ItemCode) {
			return (int)m_ItemInfo[i].m_ItemOption[OptionIndex];
		}
	}
	return -1;
}

LPBYTE ItemExcellentOption::getOptionValue(LPVOID This, LPVOID EDX, LPBYTE ItemStruct, int OptionIndex) {
	int DefaultValue1[] = { 10, 20, 2, 7, 8, 8 };
	int DefaultValue2[] = { 4, 4, 4, 5, 10, 30 };
	
	LPBYTE ReturnStruct = impGetSpecialOptionValue(This, ItemStruct, OptionIndex);

	if (ReturnStruct == NULL) {
		return NULL;
	}

	int ItemCode = *(DWORD*)(ItemStruct + 0x04);
	int OptionValue = g_ItemExcellentOption.getOptionScriptValue(ItemCode, OptionIndex);

	if (OptionValue != (int)-1) {
		*(DWORD*)(ReturnStruct + 0x68) = OptionValue;
	} else {
		if (*(BYTE*)(ItemStruct + 0x88) == 1) {
			*(DWORD*)(ReturnStruct + 0x68) = DefaultValue1[OptionIndex];
		} else if (*(BYTE*)(ItemStruct + 0x88) == 2) {
			*(DWORD*)(ReturnStruct + 0x68) = DefaultValue2[OptionIndex];
		}
	}

	return ReturnStruct;
}