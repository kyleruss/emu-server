#if !defined(XMAS_ATTACK_EVENT_H)
#define XMAS_ATTACK_EVENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonsterHerd.h"

class CXMasMonsterHerd : public MonsterHerd
{

public:

	CXMasMonsterHerd();
	virtual ~CXMasMonsterHerd();

	BYTE GetX(){return this->m_iCUR_X;}
	BYTE GetY(){return this->m_iCUR_Y;}
	int GetMap(){return this->m_iMapNumber;}

	virtual int Start();
	virtual int MonsterHerdItemDrop(LPOBJ lpObj);
};

struct MONSTER_EVENT_DATA
{
	int m_iType;
	BOOL m_bDoRegen;
	BOOL m_bDoAttackFirst;
};

struct MONSTER_EVENT_TIME
{
	int m_iHour;
	int m_iMinute;
};

struct XMASMONSTER_MAP_DATA
{
	BYTE MapNumber;
	int X1;
	int Y1;
	int X2;
	int Y2;
};

#define ATTACKEVENT_STATE_NONE		0
#define ATTACKEVENT_STATE_CLOSED	1
#define ATTACKEVENT_STATE_PLAYING	2

class MonsterAttackTime
{
public:
	MonsterAttackTime() {TICK_COUNT = GetTickCount();};
	~MonsterAttackTime() {};

	void SetSeconds(int Seconds)
	{
		TICK_COUNT = GetTickCount() + Seconds*1000;
	}

	int GetSeconds()
	{
		int TICK_CURENT = GetTickCount();
		int TICK_LEFT = TICK_COUNT - TICK_CURENT;

		if( TICK_LEFT < 0 )
		{
			TICK_COUNT = TICK_CURENT;
			return false;
		}
	return (TICK_LEFT+999)/1000;
	}

	void SetMinutes(int Minutes)
	{
		TICK_COUNT = GetTickCount()+Minutes*60000;
	}

	int GetMinutes()
	{
		int TICK_CURENT = GetTickCount();
		int TICK_LEFT = TICK_COUNT - TICK_CURENT;

		if( TICK_LEFT < 0 )
		{
			TICK_COUNT = TICK_CURENT;
			return false;
		}
	return (TICK_LEFT+59999)/60000;
	}

	void SetMiliSeconds(int MSec)
	{
		TICK_COUNT = GetTickCount() + MSec;
	}

	int GetMiliSeconds()
	{
		int TICK_CURENT = GetTickCount();
		int TICK_LEFT = TICK_COUNT - TICK_CURENT;

		if( TICK_LEFT < 0 )
		{
			TICK_COUNT = TICK_CURENT;
			return false;
		}
	return TICK_LEFT;
	}

	DWORD TICK_COUNT;
};

class MonsterAttackEvent
{
public:
	MonsterAttackEvent() : m_bDoEvent(0), m_bDoHasData(0), m_bManualStart(0)
	{
		InitializeCriticalSection(&m_critsec);
	};
	virtual ~MonsterAttackEvent() { DeleteCriticalSection(&m_critsec);};

	void Clear()
	{
		EnterCriticalSection(&m_critsec);

		m_EventMonster.erase( m_EventMonster.begin(), m_EventMonster.end() );
		m_EventTime.erase(m_EventTime.begin(), m_EventTime.end() );

		LeaveCriticalSection(&m_critsec);

		m_bDoHasData = FALSE;
	};

public:
	BOOL m_bDoEvent;
	BOOL m_bDoHasData;
	BOOL m_bManualStart;

	CRITICAL_SECTION m_critsec;
	std::vector<MONSTER_EVENT_TIME> m_EventTime;
	std::vector<MONSTER_EVENT_DATA> m_EventMonster;
	std::vector<CXMasMonsterHerd*> m_MonsterHerdData;
	MonsterAttackTime	m_TimeCounter;
};

class CXMasAttackEvent: public MonsterAttackEvent
{

public:

	CXMasAttackEvent();
	virtual ~CXMasAttackEvent();

	virtual int Load(char* lpszFileName);
	virtual void StartEvent();
	virtual void StopEvent();
	virtual void SetEnable(int bEnable);
	virtual void SetManualStart(int bValue) { m_bManualStart = bValue; }
	void SetState(int State);

	void SetState_NONE();
	void SetState_CLOSED();
	void SetState_PLAYING();

	void Move();
	void Notify(LPSTR szFormat,int Map,BYTE X,BYTE Y);
	void RemoveMonsterHerd();
	void AddMonsters(CXMasMonsterHerd* lpMonsterHerd);
	void CreateMonsterHerd();
	BOOL CheckLocation(CXMasMonsterHerd* lpMonsterHerd,XMASMONSTER_MAP_DATA* MapInfo);

	void Run();
	void ProcState_NONE();
	void ProcState_CLOSED();
	void ProcState_PLAYING();

	void SendEffect(LPSTR szMsg,int Effect);
	void CheckSync();

private:

	std::vector<XMASMONSTER_MAP_DATA> m_EventMapData;
	int m_nEventState;		// 6C
	int m_bMonsterMove;		// 70
	int m_iMOVE_RAND_SIZE;	// 74
	int m_iRADIUS_MIN;		// 78
	int m_iRADIUS_MAX;		// 7C

	int m_iTIME_OPEN;		// 80
	int m_iTIME_PLAY;		// 84
	int m_iTIME_CLOSE;		// 88

	int m_iLEFT_MIN;		// 8C
	int m_bManualStart;		// 90
};

#endif
