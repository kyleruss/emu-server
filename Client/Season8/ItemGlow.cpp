#include "stdafx.h"
#include "ItemGlow.h"
#include "ScriptEncode.h"

ItemGlow g_ItemGlow;

void ItemGlow::Load() {
	Read("Data\\Custom\\ItemGlow.z");
	//DWORD dwThreadID;
	//HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ItemGlow::Refresh, NULL, NULL, &dwThreadID);
}

void ItemGlow::Init() {
	this->m_GlowInfo.clear();
	if (this->m_GlowInfo.capacity() > 0) {
		std::vector<ItemGlowInfo>().swap(this->m_GlowInfo);
	}
}

void ItemGlow::Read(const char *File) {
	FILE *Stream = fopen("Data\\Custom\\ItemGlow.z", "rb");

	if (Stream == NULL)	{
		MessageBoxA(NULL, "ItemGlow.z not found", "ZTClient", ERROR);
		return;
	}

	int LineCount = 0;
	fread(&LineCount, sizeof(LineCount), 1, Stream);

	for (int i = 0; i < LineCount; i++) {
		ItemGlowInfo NewItem = { 0 };
		fread(&NewItem, sizeof(ItemGlowInfo), 1, Stream);
		ScriptEncode::Encode((char*)&NewItem, sizeof(ItemGlowInfo));
		m_GlowInfo.push_back(NewItem);
	}

	fclose(Stream);
}

void ItemGlow::Refresh() {
	int FailCount = 0;
	while (true) {
		FILE *Stream = fopen("Data\\Custom\\ItemGlow.z", "rb");
		if (Stream == NULL)	{
			FailCount++;
			if (FailCount < 10) {
				continue;
			}
		}

		g_ItemGlow.Init();
		FailCount = 0;
		int LineCount = 0;
		fread(&LineCount, sizeof(LineCount), 1, Stream);
		for (int i = 0; i < LineCount; i++) {
			ItemGlowInfo NewItem = { 0 };
			fread(&NewItem, sizeof(ItemGlowInfo), 1, Stream);
			ScriptEncode::Encode((char*)&NewItem, sizeof(ItemGlowInfo));
			g_ItemGlow.m_GlowInfo.push_back(NewItem);
		}
		fclose(Stream);
		Sleep(2000);
	}
}

bool ItemGlow::setColor(WORD ItemCode, FRGB *ColorBuffer) {
	for (int i = 0; i < m_GlowInfo.size(); i++) {
		if (m_GlowInfo[i].ItemCode == ItemCode) {
			ColorBuffer->r = (float)(m_GlowInfo[i].R / 255.0);
			ColorBuffer->g = (float)(m_GlowInfo[i].G / 255.0);
			ColorBuffer->b = (float)(m_GlowInfo[i].B / 255.0);
			return true;
		}
	}
	return false;
}