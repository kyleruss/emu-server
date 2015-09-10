#pragma once

#include "user.h"

struct ExtraClassAttack
{
	BYTE Class;
	short Reset;
	short Damage;
	short Absorb;
};

class ExtraClassBalance  
{
public:
			ExtraClassBalance();
	virtual ~ExtraClassBalance();
	
	void	Init();
	void	Load();
	void	Read(LPSTR File);

	void	Apply(LPOBJ lpUser);

private:
	std::vector<ExtraClassAttack> m_AttackInfo;
}; extern ExtraClassBalance g_ExtraClassBalance;