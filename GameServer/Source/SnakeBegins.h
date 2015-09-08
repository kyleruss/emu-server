#pragma once
// -------------------------------------------------------------------------------

#include "user.h"
// -------------------------------------------------------------------------------

class SnakeBegins  
{
public:
			SnakeBegins();
	virtual	~SnakeBegins();
	// ----
	bool	DropScroll(LPOBJ lpUser, WORD ItemType);
	void	DropReward(LPOBJ lpUser, LPOBJ lpMonster);
	void	Mix(LPOBJ lpUser);
	// ----
	bool	IsSnake(int MonsterType) { return MonsterType >= 647 && MonsterType <= 650; };
	bool	IsScroll(WORD ItemType) { return ItemType >= ITEMGET(14, 210) && ItemType <= ITEMGET(14, 213); };
	// ----
private:
	BYTE	m_Grade1MixRate;
	int		m_Grade1MixMoney;
	BYTE	m_Grade2MixRate;
	int		m_Grade2MixMoney;
	BYTE	m_Grade3MixRate;
	int		m_Grade3MixMoney;
	// ----
}; extern SnakeBegins g_SnakeBegins;
// -------------------------------------------------------------------------------
