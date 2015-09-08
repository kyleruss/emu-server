// StatGem.cpp: implementation of the StatGem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatGem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StatGem g_StatGem;

StatGem::StatGem()
{
	this->m_GemInfo[0].ItemID		= ITEMGET(14, 308);
	this->m_GemInfo[0].EffectType	= StatGemType::DoubleDamageRate;
	this->m_GemInfo[0].EffectValue	= 100;
	this->m_GemInfo[1].ItemID		= ITEMGET(14, 309);
	this->m_GemInfo[1].EffectType	= StatGemType::ExcellentDamageRate;
	this->m_GemInfo[1].EffectValue	= 100;
	this->m_GemInfo[2].ItemID		= ITEMGET(14, 310);
	this->m_GemInfo[2].EffectType	= StatGemType::DamageDecrease;
	this->m_GemInfo[2].EffectValue	= 100;
	this->m_GemInfo[3].ItemID		= ITEMGET(14, 311);
	this->m_GemInfo[3].EffectType	= StatGemType::DefenseIncrease;
	this->m_GemInfo[3].EffectValue	= 100;
	this->m_GemInfo[4].ItemID		= ITEMGET(14, 312);
	this->m_GemInfo[4].EffectType	= StatGemType::ZenHuntIncrease;
	this->m_GemInfo[4].EffectValue	= 100;
	this->m_GemInfo[5].ItemID		= ITEMGET(14, 313);
	this->m_GemInfo[5].EffectType	= StatGemType::MaxLifeIncrease;
	this->m_GemInfo[5].EffectValue	= 100;
}

StatGem::~StatGem()
{

}

bool StatGem::WithGem(LPOBJ lpUser, BYTE Type)
{
	for( int n = INVENTORY_BAG_START; n < MAIN_INVENTORY_SIZE; n++ )	
	{
		if( !lpUser->pInventory[n].IsItem() )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[n].m_Type != this->m_GemInfo[Type].ItemID )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[n].m_Durability < 1.0f )
		{
			continue;
		}
		// ----
		return true;
	}
	// ----
	return false;
}

int StatGem::GetGemCount(LPOBJ lpUser, BYTE Type)
{
	int GemCount = 0;
	// ----
	for( int n = INVENTORY_BAG_START; n < MAIN_INVENTORY_SIZE; n++ )	
	{
		if( !lpUser->pInventory[n].IsItem() )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[n].m_Type != this->m_GemInfo[Type].ItemID )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[n].m_Durability < 1.0f )
		{
			continue;
		}
		// ----
		GemCount++;
	}
	// ----
	return GemCount;
}

void StatGem::SimpleEffectAdd(LPOBJ lpUser)
{
	WORD EffectValue = 0;
	int EffectCount = 0;
	// ----
	if( this->WithGem(lpUser, StatGemType::DoubleDamageRate) )
	{
		EffectCount = this->GetGemCount(lpUser, StatGemType::DoubleDamageRate);
		EffectValue = this->m_GemInfo[StatGemType::DoubleDamageRate].EffectValue;
		lpUser->SetOpDoubleDamage += EffectValue * EffectCount;
	}
	// ----
	if( this->WithGem(lpUser, StatGemType::ExcellentDamageRate) )
	{
		EffectCount = this->GetGemCount(lpUser, StatGemType::ExcellentDamageRate);
		EffectValue = this->m_GemInfo[StatGemType::ExcellentDamageRate].EffectValue;
		lpUser->m_ExcelentDamage += EffectValue * EffectCount;
	}
	// ----
	if( this->WithGem(lpUser, StatGemType::DamageDecrease) )
	{
		EffectCount = this->GetGemCount(lpUser, StatGemType::DamageDecrease);
		EffectValue = this->m_GemInfo[StatGemType::DamageDecrease].EffectValue;
		lpUser->DamageMinus += EffectValue * EffectCount;
	}
	// ----
	if( this->WithGem(lpUser, StatGemType::DefenseIncrease) )
	{
		EffectCount = this->GetGemCount(lpUser, StatGemType::DefenseIncrease);
		EffectValue = this->m_GemInfo[StatGemType::DefenseIncrease].EffectValue;
		lpUser->m_Defense		+= (lpUser->m_Defense / 100 * EffectValue) * EffectCount;
		lpUser->m_MagicDefense	+= (lpUser->m_MagicDefense / 100 * EffectValue) * EffectCount;
	}
	// ----
	if( this->WithGem(lpUser, StatGemType::ZenHuntIncrease) )
	{
		EffectCount = this->GetGemCount(lpUser, StatGemType::ZenHuntIncrease);
		EffectValue = this->m_GemInfo[StatGemType::ZenHuntIncrease].EffectValue;
		lpUser->MonsterDieGetMoney += EffectValue * EffectCount;
	}
	// ----
	if( this->WithGem(lpUser, StatGemType::MaxLifeIncrease) )
	{
		EffectCount = this->GetGemCount(lpUser, StatGemType::MaxLifeIncrease);
		EffectValue = this->m_GemInfo[StatGemType::MaxLifeIncrease].EffectValue;
		lpUser->AddLife += (lpUser->MaxLife / 100 * EffectValue) * EffectCount;
		GCReFillSend(lpUser->m_Index,(int)(lpUser->AddLife + lpUser->MaxLife),0xFEu,0,lpUser->iAddShield + lpUser->iMaxShield);
        GCReFillSend(lpUser->m_Index, (int)lpUser->Life, 0xFFu, 0, lpUser->iShield);
	}
}