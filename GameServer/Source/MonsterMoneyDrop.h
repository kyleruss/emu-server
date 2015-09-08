#pragma once

#include "User.h"

struct MonsterMoneyDropInfo
{
	short	MonsterID;
	short	MonsterMinLevel;
	short	MonsterMaxLevel;
	short	MonsterMapNumber;
	int		MoneyMin;
	int		MoneyMax;
	DWORD	Rate;
};

class MonsterMoneyDrop  
{
public:
	MonsterMoneyDrop();
	virtual ~MonsterMoneyDrop();

	void	Init();
	void	Load();
	void	Read(LPSTR File);

	int		Run(LPOBJ lpUser, LPOBJ lpMonster); //0: Not found, 1: Good roll, 2: Bad roll

private:
	bool	m_Enabled;
	std::vector<MonsterMoneyDropInfo> m_DropInfo;

}; extern MonsterMoneyDrop g_MonsterMoneyDrop;