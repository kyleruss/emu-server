#pragma once
// -------------------------------------------------------------------------------

#include "user.h"
// -------------------------------------------------------------------------------

struct CLASSCALC_DATA
{
	WORD LevelPoints;
	WORD MasterLevelPoints;
	WORD MarlonBonusPoints;
	WORD AttackDamageMinRightDiv1;
	WORD AttackDamageMaxRightDiv1;
	WORD AttackDamageMinLeftDiv1;
	WORD AttackDamageMaxLeftDiv1;
	WORD AttackDamageMinRightDiv2;
	WORD AttackDamageMaxRightDiv2;
	WORD AttackDamageMinLeftDiv2;
	WORD AttackDamageMaxLeftDiv2;
	WORD AttackDamageMinRightDiv3;
	WORD AttackDamageMaxRightDiv3;
	WORD AttackDamageMinLeftDiv3;
	WORD AttackDamageMaxLeftDiv3;
	WORD MagicDamageMinDiv;
	WORD MagicDamageMaxDiv;
	WORD CurseDamageMinDiv;
	WORD CurseDamageMaxDiv;
	WORD AttackSpeedDiv;
	WORD MagicSpeedDiv;
	WORD DefenseDiv;
	WORD PvPAttackRateFact1;
	float PvPAttackRateFact1RF;
	WORD PvPAttackRateFact2;
	WORD PvPAttackRateDiv;
	WORD PvPDefenseRateFact;
	float PvPDefenseRateFactRF;
	WORD PvPDefenseRateDiv;
	WORD SuccessBlockDiv;
	WORD PetAttackDamageMinFact;
	WORD PetAttackDamageMinDiv;
	WORD PetAttackDamageMinStart;
	WORD PetAttackDamageMaxFact;
	WORD PetAttackDamageMaxDiv;
	WORD PetAttackDamageMaxStart;
	WORD PetCriticalAttackRate;
	WORD PetAttackSpeedFact;
	WORD PetAttackSpeedDiv1;
	WORD PetAttackSpeedDiv2;
	WORD PetAttackSpeedStart;
	WORD PetSuccessAttackRateDiv;
	WORD PetSuccessAttackRateStart;
};
// -------------------------------------------------------------------------------

class ClassCalc
{
public:
			ClassCalc();
			~ClassCalc();
	// ----
	void	Load();
	void	ReadData(LPSTR File);
	// ----
	WORD	GetLevelPoint(LPOBJ lpUser, BYTE OnlyMaster, BYTE OnlyMarlon);
	void	GetAttackDamage(LPOBJ lpUser);
	void	GetMagicDamage(LPOBJ lpUser);
	void	GetAttackSpeed(LPOBJ lpUser);
	void	GetSuccessfulBlocking(LPOBJ lpUser);
	void	GetDefense(LPOBJ lpUser);
	float	GetAttackRate(LPOBJ lpUser);
	float	GetDefenseRate(LPOBJ lpUser);
	int		GetPetAttackDamageMinFact()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackDamageMinFact;
	};
	int		GetPetAttackDamageMinDiv()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackDamageMinDiv;
	};
	int		GetPetAttackDamageMinStart()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackDamageMinStart;
	};
	int		GetPetAttackDamageMaxFact()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackDamageMaxFact;
	}
	int		GetPetAttackDamageMaxDiv()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackDamageMaxDiv;
	};
	int		GetPetAttackDamageMaxStart()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackDamageMaxStart;
	};
	int		GetPetCriticalAttackRate()
	{
		return this->m_Data[CLASS_DARKLORD].PetCriticalAttackRate;
	};
	int		GetPetAttackSpeedFact()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackSpeedFact;
	};
	int		GetPetAttackSpeedDiv1()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackSpeedDiv1;
	};
	int		GetPetAttackSpeedDiv2()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackSpeedDiv2;
	};
	int		GetPetAttackSpeedStart()
	{
		return this->m_Data[CLASS_DARKLORD].PetAttackSpeedStart;
	};
	int		GetPetSuccessAttackRateDiv()
	{
		return this->m_Data[CLASS_DARKLORD].PetSuccessAttackRateDiv;
	};
	int		GetPetSuccessAttackRateStart()
	{
		return this->m_Data[CLASS_DARKLORD].PetSuccessAttackRateStart;
	};
	// ----
private:
	CLASSCALC_DATA m_Data[7];
	// ----
}; extern ClassCalc g_ClassCalc;
// -------------------------------------------------------------------------------