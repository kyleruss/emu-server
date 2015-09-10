#pragma once

#include "..\common\zzzitem.h"
#include "user.h"


class CMapItem : public CItem
{

public:

	CMapItem();
	virtual ~CMapItem();

	void Init();
	void CreateItem(int type, int level, int x, int y, float dur, BYTE Option1, BYTE Option2, BYTE Option3, BYTE NOption, BYTE SOption, DWORD number, BYTE ItemEffectEx,LPBYTE SocketOption,BYTE SocketBonus);
	void DropCreateItem(int type, int level, int x, int y, float dur, BYTE Option1, BYTE Option2, BYTE Option3, BYTE NOption, BYTE SOption, DWORD number, int PetLevel, int PetExp, BYTE ItemEffectEx,LPBYTE SocketOption,BYTE SocketBonus);

public:
	
	BYTE px;	// B4
	BYTE py;	// B5
	bool live;	// B6
	bool Give;	// B7
	DWORD m_State;	// B8
	DWORD m_Time;	// BC
	int m_UserIndex;	// C0
	DWORD m_LootTime;	// C4
	VIEWPORT_PLAYER_STRUCT VpPlayer[MAX_VIEWPORT];	// C8
	int VPCount;	// 44C
};

struct _CUSTOM_DROP
{
	int m_Type;
	BYTE m_LevelMin;
	BYTE m_LevelMax;
	BYTE m_Option1;
	BYTE m_Option2;
	BYTE m_Option3;
	BYTE m_Exc;
	BYTE m_Anc;
	short m_DropLvlMin;
	short m_DropLvlMax;
	BYTE m_DropMap;
	int m_DropRate;
};
