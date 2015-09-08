#ifndef MONSTERSETBASE_H
#define MONSTERSETBASE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h"

typedef struct MONSTER_POSITION
{
	BYTE m_ArrangeType;	// 0
	BYTE m_LoadType;	// 0
	WORD m_Type;		// 2
	BYTE m_MapNumber;	// 4
	BYTE m_Dis;	// 5
	BYTE m_X;	// 6
	BYTE m_Y;	// 7
	BYTE m_Dir;	// 8
	BYTE m_W;	// 9
	BYTE m_H;	// A
	short m_wZoneIndex;
	short m_wMonsterRegenTableIndex;
	int m_PentagramMainAttribute;
} MONSTER_POSITION, * LPMONSTER_POSITION;

enum
{
	LOAD_ALLOBJ = -1,
	LOAD_NPCLIST = 0,
	LOAD_ELDORADOLIST = 1,
	LOAD_MONSTERLIST = 2,
};

class CMonsterSetBase
{
public:
	CMonsterSetBase() { Init(); };
	virtual ~CMonsterSetBase() {};

	void Init();
	void GetPentagramMainAttribute(int TableNum, int *iPentagramMainAttribute);
	void LoadSetBase(char* filename,int type);
	void MonsterSetBaseReload(int type);
	void RemoveMonsterSetBase(int iPosNum);
	void AddAllMonsters(int type);
	int GetPosition(int TableNum, short MapNumber, BYTE& x, BYTE& y);
	int GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, BYTE& mx, BYTE& my);
	int SetMonsterSetBase(int ArrangeType,int loadtype,int Type,int MapNumber,int Dis,int X,int Y,int Dir,int W,int H, int Element);
	int GetShopID(int posnum);

public:
	MONSTER_POSITION m_Mp[OBJ_MAXMONSTER];	// 4

	int m_NpcCount;
	int m_GoldenCount;
	int m_MonsterCount;
	int m_Count;	// 10FE4

private:
	std::map<int,int> m_mShopNum;
};


#endif