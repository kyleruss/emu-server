// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef ELEDORADOEVENT_H
#define ELEDORADOEVENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEledoradoEvent
{

public:

	CEledoradoEvent();
	virtual ~CEledoradoEvent();

	void Init();
	void SetEventState(int State);
	void Run();
	void RegenGoldGoblen();
	void RegenTitan();
	void RegenGoldDercon();
	void RegenDevilLizardKing();
	void RegenKantur();

	void RegenRabbit();
	void RegenDarkKnight();
	void RegenDevil();
	void RegenMonster();
	void RegenCrust();
	void RegenSatiros();
	void RegenTwintail();
	void RegenIronKnight();
	void RegenNeipin();
	void RegenGreatDragon();

	void CheckGoldDercon(int MapNumber);
	void SetMenualStart(BOOL bMenualStart){this->m_bMenualStart = bMenualStart;}	// line : 57
	void Start_Menual();
	void End_Menual();

public:

	int EventState;	// 4
	DWORD GoldGoblenEventStartTime;	// 8
	DWORD TitanEventStartTime;	// C
	DWORD GoldDerconEventStartTime;	// 10
	DWORD DevilLizardKingEventStartTime;	// 14
	DWORD KanturEventStartTime;	// 18

	DWORD RabbitEventStartTime;	// 1C
	DWORD DarkKnightEventStartTime;	// 20
	DWORD DevilEventStartTime;	// 24
	DWORD MonsterEventStartTime;	// 28
	DWORD CrustEventStartTime;	// 2C
	DWORD SatirosEventStartTime;	// 30
	DWORD TwintailEventStartTime;	//	34
	DWORD IronKnightEventStartTime;	//	38
	DWORD NeipinEventStartTime;	//	3C
	DWORD GreatDragonEventStartTime;	// 40

	int m_BossTitanMapNumber;	// 44
	int m_BossTitanMapX;	// 48
	int m_BossTitanMapY;	// 4C
	int m_BossDevilLizardKingMapNumber;	// 50
	int m_BossDevilLizardKingMapX;	// 54
	int m_BossDevilLizardKingMapY;	// 58
	int m_BossKanturMapNumber;	// 5C
	int m_BossKanturMapX;	// 60
	int m_BossKanturMapY;	// 64
	int m_bMenualStart;	// 68
	int m_BossGoldDerconMapNumber[3];	// 6C
};

extern CEledoradoEvent gEledoradoEvent;

#endif