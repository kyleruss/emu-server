// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// GS-N 0.99.60T 0x004E5740 - Status Completed :)
// GS-N	1.00.18	JPN	0x0051E110	-	Completed

#include "StdAfx.h"
#include <iostream>
#include <iomanip>
#include "zzzitem.h"
#include "SetItemOption.h"
#include "..\SOURCE\logproc.h"
#include "WzMemScript.h"
#include "..\include\Readscript.h"
#include "..\SOURCE\JewelOfHarmonySystem.h"
#include "..\SOURCE\ItemSystemFor380.h"
#include "..\SOURCE\user.h"
#include "..\SOURCE\GameMain.h"
#include "..\SOURCE\ElementalSystem.h"
#include "..\SOURCE\GameEvent.h"

#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
#include "..\Source\ItemExcellentOption.h"
#endif

#if (ENABLETEST_MUUN ==1)
#include "..\Source\MuunSystem.h"
#endif

ITEM_ATTRIBUTE ItemAttribute[MAX_ITEMS];
CPetItemExp gPetItemExp;
int g_MaxItemIndexOfEachItemType[MAX_TYPE_ITEMS];

CItem::CItem()
{
	Clear();
}

void CItem::Clear()	// Fine
{
	m_Type =-1;
	m_Level =0;
	m_Part=0;
	m_Class=0;
	m_TwoHand=0;
	m_AttackSpeed=0;
	m_DamageMin=0;
	m_DamageMax=0;
	m_SuccessfulBlocking=0;
	m_Defense=0;
	m_MagicDefense =0;
	m_Durability=0;
	m_SpecialNum=0;
	m_Value=0;
	m_Option1 =0;
	m_Option2 =0;
	m_Option3 =0;
	m_NewOption =0;
	m_Number =0;
	m_DurabilitySmall =0;
	m_iPShopValue=-1;
	m_PShopBlessValue = -1;
	m_PShopSoulValue = -1;
	m_PShopChaosValue = -1;
	m_bItemExist=true;
	m_CurrentDurabilityState=-1;
	m_SetOption=0;
	m_QuestItem=false;


	memset(this->m_Special, 0, sizeof(this->m_Special));
	memset(this->m_SpecialValue, 0, sizeof(this->m_SpecialValue));
	memset(this->m_Resistance, 0, sizeof(this->m_Resistance));

	m_IsLoadPetItemInfo=0;
	m_PetItem_Level =1;
	m_PetItem_Exp =0;
	m_Leadership=0;
	m_JewelOfHarmonyOption = 0;
	m_ItemOptionEx = 0;

	m_SocketBonus = 0xFF;
	memset(m_SocketOption,0xFF,sizeof(m_SocketOption));
	m_BonusDurability = 0;
	m_bLuckySet = 0;

	m_bPeriodItem = 0;
	m_bExpired = 0;

	m_bLOCKED = 0;
}

BOOL CItem::IsItem() // Good
{
	if (m_Type < 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int CItem::IsSetItem() // Good
{
	if (m_Type < 0)
	{
		return 0;
	}
	else
	{
		return m_SetOption & 3;	// Limit of the SetItem
	}
}

void CItem::Convert(int type, BYTE Option1, BYTE Option2, BYTE Option3,BYTE Attribute2, BYTE SetOption, BYTE ItemEffectEx,LPBYTE SocketOption,BYTE SocketBonus, BYTE DbVersion)
{
	int _type;
	int ItemLevel;
	LPITEM_ATTRIBUTE p;
	int SOption;
	int SOptionStatValue;

	_type = type;

	if ( (DbVersion ) == 0)
	{
		_type=(type/MAX_TYPE_ITEMS*MAX_SUBTYPE_ITEMS)+(type % MAX_TYPE_ITEMS);

	}
	else if ( DbVersion <= 2 )
	{
		_type = (type/32*MAX_SUBTYPE_ITEMS) + (type % 32 );
	}

	if ( _type >MAX_ITEMS-1)
	{
		LogAddC(2, "error-L1 : ItemIndex error %d", _type);
	}

	p=&ItemAttribute[_type];
	m_serial=ItemAttribute[_type].Serial ;
	m_Type=type;

	if ( (DbVersion ) <= 2)
	{
		this->m_Type=_type;
	}

	m_NewOption =Attribute2;
	
	if (p->OptionFlag  == 0)
	{
		m_NewOption=0;
	}

	if ( (_type >= ITEMGET(12,3) ) && (_type <= ITEMGET(12,6) )	||
	     _type == ITEMGET(13,30)	||
		 _type == ITEMGET(12,41)	||
		 _type == ITEMGET(12,42)	||
		 _type == ITEMGET(12,43)	||
		 ( _type >= ITEMGET(12,36) ) && (_type <= ITEMGET(12,40) )	||
		 _type == ITEMGET(12,49) ||
		 _type == ITEMGET(12,50) ||
		 (_type  >= ITEMGET(12,262) && _type <= ITEMGET(12,267) )

#if (CUSTOM_WINGS == 1)
		 || ( _type >= ITEMGET(12,440))  && (_type <= ITEMGET(12,445) )
#endif
	)

	{
		Attribute2=0;
	}

	if( _type == ITEMGET(13,37) )
	{
		Attribute2=0;
	}

	SetOption = SetOption & 15;
		
	SOption=SetOption & 3;
	SOptionStatValue=0;

	if (SOption != 1 && SOption != 2)
	{
		SOption=0;
	}

	if ( (( (SetOption>>2) & 3) != 1) && ( ((SetOption>>2) & 3) != 2) )
	{
		SOption=0;
	}

	if ( gSetItemOption.IsSetItem(_type) != 0 && SetOption != (BYTE)-1 && SOption != 0)
	{
		m_SetOption = SetOption;
		if ( m_SetOption != 0 )
		{
			// Disable this line to enable full exc-anc items
#if (__CUSTOM__ == 1)
#else
			m_NewOption = 0;
#endif
			Attribute2 = 0;
			SOptionStatValue = (SetOption>>2)&3;
		}
	}
	else
	{
		m_SetOption = 0;
	}

	m_ItemOptionEx = ItemEffectEx;

	if( g_SocketItem.IsSocketItem(this) == true )
	{
		if( SocketOption != NULL )
		{
			m_SocketBonus = SocketBonus;

			for(int n = 0; n < MAX_SOCKET_COUNT; n++)
			{
				m_SocketOption[n] = SocketOption[n];
			}
		}

		if( g_SocketItem.CheckSocketOption(this) == true )
		{
#if (__CUSTOM__ == 1)

#else
			m_ItemOptionEx = 0;
			m_SetOption = 0;
			Attribute2 = 0;
			m_NewOption = 0;
#endif
			m_JewelOfHarmonyOption = 0;
			
		}
		else
		{
			m_SocketBonus = 0;
		}
	}
	else if( g_ElementalSystem.IsElementalItem(this) )
	{
		if( SocketOption != NULL )
		{
			m_SocketBonus = SocketBonus;

			for(int n = 0; n < MAX_SOCKET_COUNT; n++)
			{
				m_SocketOption[n] = SocketOption[n];
			}
		}
	}
#if (ENABLETEST_MUUN == 1)
	else if (g_CMuunSystem.IsMuunItem(this)) {
		if (SocketOption != 0) {
			m_SocketBonus = SocketBonus;
			for (int n = 0; n < MAX_SOCKET_COUNT; n++) {
				m_SocketOption[n] = SocketOption[n];
			}
		}
		m_ItemOptionEx = 0;
		m_SetOption = 0;
		Attribute2 = 0;
		m_NewOption = 0;
		m_JewelOfHarmonyOption = 0;
	}
	else if (g_CMuunSystem.IsStoneofEvolution(this)) {
		if (SocketOption != 0) {
			m_SocketBonus = 0;
			for (int n = 0; n < MAX_SOCKET_COUNT; n++) {
				m_SocketOption[n] = SocketOption[n];
			}
		}
		m_ItemOptionEx = 0;
		m_SetOption = 0;
		Attribute2 = 0;
		m_NewOption = 0;
		m_JewelOfHarmonyOption = 0;
	}
#endif
	else
	{
		m_SocketBonus = 0;

		for(int n = 0; n < MAX_SOCKET_COUNT; n++)
		{
			m_SocketOption[n] = (BYTE)-1;
		}
	}

	memset( m_SkillResistance , 0, sizeof(m_SkillResistance) );
	
	if ( _type == ITEMGET(0,19) || _type == ITEMGET(4,18) || _type == ITEMGET(5,10) || _type == ITEMGET(5,36) || _type == ITEMGET(2,13))
	{
		Attribute2=0;
	}

	int iChaosItem = 0;

	if ( m_Type == ITEMGET(2,6) )
	{
		iChaosItem = 15;
	}
	else if ( m_Type == ITEMGET(5,7) )
	{
		iChaosItem = 25;
	}
	else if ( m_Type == ITEMGET(4,6) )
	{
		iChaosItem = 30;
	}

	if ( ItemGetDurability(m_Type, 0, 0, 0) != 0 )
	{
		m_BaseDurability = (float)ItemGetDurability(m_Type, m_Level, Attribute2 & 0x7F, m_SetOption);
	}
	else
	{
		m_BaseDurability = 0;
	}

	m_DurabilityState[0] = m_BaseDurability * 0.5f;
	m_DurabilityState[1] = m_BaseDurability * 0.3f;
	m_DurabilityState[2] = m_BaseDurability * 0.2f;
	m_Value = p->Value;
	m_AttackSpeed = p->AttackSpeed;
	m_TwoHand = p->TwoHand;

	m_DamageMin = p->DamageMin;
	m_DamageMax = p->DamageMax;

	m_SuccessfulBlocking = p->SuccessfulBlocking;
	m_Defense = p->Defense;
	m_MagicDefense = p->MagicDefense;
	m_WalkSpeed = p->WalkSpeed;

	if( (_type >> 9) == 5 && p->ItemSlot == 1)
	{
		m_Magic = 0;
		m_CurseMagic = p->MagicPW;
	}
	else
	{
		m_Magic = p->MagicPW;
		m_CurseMagic = 0;
	}

	m_serial = p->Serial;
	m_QuestItem = (bool)p->QuestItem;
	m_RequireLeaderShip = 0;

	if (m_BaseDurability > 10.0f )
	{
		if( _type != ITEMGET(13,20) &&
			_type != ITEMGET(13,38) )
		{
			if( m_Durability > m_BaseDurability )
			{
				m_Durability = m_BaseDurability;
			}
		}
	}

	if (m_Durability == 0 )
	{
		m_CurrentDurabilityState = 1;
	}
	else if ( m_Durability < m_DurabilityState[2] )
	{
		m_CurrentDurabilityState = (float)0.5;
	}
	else if ( m_Durability < m_DurabilityState[1] )
	{
		m_CurrentDurabilityState = (float)0.3;
	}
	else if ( m_Durability < m_DurabilityState[0] )
	{
		m_CurrentDurabilityState = (float)0.2;
	}
	else
	{
		m_CurrentDurabilityState = 0.0;
	}

	memcpy(m_RequireClass, p->RequireClass , sizeof(m_RequireClass));

	for ( int i = 0; i<MAX_ITEM_SPECIAL_ATTRIBUTE ; i++ )
	{
		m_Resistance[i] = p->Resistance[i] * m_Level;
	}

	ItemLevel = p->Level;

	if ( ( Attribute2 & 0x3F) > 0 )
	{
		ItemLevel = p->Level + 25;
	}
	else if (m_SetOption != 0 )
	{
		ItemLevel = p->Level + 25;
	}

	if( g_ElementalSystem.IsPentagram(m_Type) )
	{
		m_DamageMin = p->RequireLevel;
		m_DamageMax = p->RequireEnergy;
		m_Defense = p->Defense;

		m_RequireLevel = p->Level + 4*m_Level;

		m_RequireLeaderShip = 0;
		m_RequireVitality = 0;
		m_RequireEnergy = 0;
		m_RequireDexterity = 0;
		m_RequireStrength = 0;

		m_CurrentDurabilityState = 0;
	}
	else
	{
		if ( p->RequireStrength != 0 )
		{
			m_RequireStrength = ((p->RequireStrength * (ItemLevel + m_Level * 3 ) ) * 3) / 100 + 20;
		}
		else
		{
			m_RequireStrength = 0;
		}


		if ( p->RequireDexterity != 0 )
		{
			m_RequireDexterity = ((p->RequireDexterity * (ItemLevel + m_Level * 3 ) ) * 3) / 100 + 20;
		}
		else
		{
			m_RequireDexterity  = 0;
		}

		if ( p->RequireEnergy != 0 )
		{
			m_RequireEnergy = ((p->RequireEnergy * (ItemLevel + m_Level * 3 ) ) * 4) / 100 + 20;
		}
		else
		{
			m_RequireEnergy = 0;
		}
		
		if ( p->RequireEnergy != 0 )
		{
			if( (_type >> 9) == 5 && p->ItemSlot == 1 )
			{
				m_RequireEnergy = (p->RequireEnergy * (ItemLevel + m_Level) * 3 ) / 100 + 20;
			}
		}
		else
		{
			m_RequireEnergy = 0;
		}

		if ( _type >= ITEMGET(0,0) && _type < ITEMGET(12,0) )
		{
			if ( p->RequireVitality != 0 )
			{
				m_RequireVitality = ((p->RequireVitality * (ItemLevel + m_Level * 3 ) ) * 3) / 100 + 20;
			}
			else
			{
				m_RequireVitality = 0;
			}
	
			if ( p->RequireLeadership != 0 )
			{
				m_RequireLeaderShip = ((p->RequireVitality * (ItemLevel + m_Level * 3 ) ) * 3) / 100 + 20;
			}
			else
			{
				m_RequireLeaderShip = 0;
			}
		}
	
		if ( _type == ITEMGET(13,5) )
		{
			m_RequireLeaderShip = m_PetItem_Level * 15 + 185;
			p->RequireLeadership = m_PetItem_Level * 15 + 185;
		}
	
		if ( g_kJewelOfHarmonySystem.IsActive(this) == TRUE )
		{
			BYTE btOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(this);
			int iType = g_kJewelOfHarmonySystem._GetItemType(this);
	
			if( iType == 1 )
			{
				if ( btOption == AT_JEWELOFHARMONY_WEAPON_DECREASE_REQUIRE_STR )
					m_HJOpStrength = g_kJewelOfHarmonySystem.GetItemEffectValue(this,
					AT_JEWELOFHARMONY_WEAPON_DECREASE_REQUIRE_STR);
				else if ( btOption == AT_JEWELOFHARMONY_WEAPON_DECREASE_REQUIRE_DEX )
					m_HJOpDexterity = g_kJewelOfHarmonySystem.GetItemEffectValue(this,
					AT_JEWELOFHARMONY_WEAPON_DECREASE_REQUIRE_DEX);
			}
			else if( iType == 2 )
			{
				if ( btOption == AT_JEWELOFHARMONY_WEAPON_IMPROVE_MAXATTACKDAMAGE )
					m_HJOpStrength = g_kJewelOfHarmonySystem.GetItemEffectValue(this,
					AT_JEWELOFHARMONY_WEAPON_IMPROVE_MAXATTACKDAMAGE);
				else if ( btOption == AT_JEWELOFHARMONY_WEAPON_DECREASE_REQUIRE_STR )
					m_HJOpDexterity = g_kJewelOfHarmonySystem.GetItemEffectValue(this,
					AT_JEWELOFHARMONY_WEAPON_DECREASE_REQUIRE_STR);
			}
		}
		else
		{
			m_HJOpStrength = 0;
			m_HJOpDexterity  = 0;
		}
	
		g_SocketItem.SetSocketOption(this);
	
		if ( p->RequireLevel != 0 )
		{
			if ( _type == ITEMGET(13,4) )	// DarkHorse
			{
				m_RequireLevel = m_PetItem_Level * 2 + 218;
			}
	#if ( CUSTOM_WINGS == 1 )
			else if ( _type >= ITEMGET(12,440) && _type <= ITEMGET(12,445) )
			{
				m_RequireLevel = p->RequireLevel;
			}
	#endif
			else if ( _type >= ITEMGET(12,36) && _type <= ITEMGET(12,40) )
			{
				m_RequireLevel = p->RequireLevel;
			}
			else if ( _type >= ITEMGET(12,3) && _type <= ITEMGET(12,6) )
			{
				m_RequireLevel = p->RequireLevel + (m_Level * 5);
			}
			else if ( _type == ITEMGET(12,42) )
			{
				m_RequireLevel = p->RequireLevel + (m_Level * 5);
			}
			else if ( _type == ITEMGET(12,43) )
			{
				m_RequireLevel = p->RequireLevel;
			}
			else if ( _type == ITEMGET(12,50) )
			{
				m_RequireLevel = p->RequireLevel;
			}
			else if ( _type >= ITEMGET(12,7) && _type <= ITEMGET(12,24) )
			{
				m_RequireLevel = p->RequireLevel;
			}
			else if ( _type >= ITEMGET(0,0) && _type < ITEMGET(12,0) )
			{
				if ( p->RequireLevel )
					m_RequireLevel = p->RequireLevel;
			}
			else if ( _type == ITEMGET(12,266) || _type == ITEMGET(12,267) )
			{
				m_RequireLevel = p->RequireLevel;
			}
			else
			{
				m_RequireLevel = p->RequireLevel + m_Level * 4;
			}
		}
		else
		{
			m_RequireLevel = 0;
		}
	}

	if ( m_Type == ITEMGET(13,10) )
	{
		if ( ItemLevel <= 2 )
		{
			m_RequireLevel = 20;
		}
		else
		{
			m_RequireLevel = 50;
		}
	}

	if ( (Attribute2 & 0x3F) > 0 )
	{
		if ( m_RequireLevel > 0 )
		{
			if ( _type < ITEMGET(0,0) || _type >= ITEMGET(12,0) )
				m_RequireLevel += 20;
		}
	}

	if ( m_SetOption != 0 )
	{
		ItemLevel = p->Level + 30;
	}
	
	m_Leadership = 0;

	if ( m_DamageMax > 0 )
	{
		if ( m_SetOption != 0 && ItemLevel != 0 )
		{
			m_DamageMax += (m_DamageMin * 25) / p->Level + 5;
			m_DamageMax += ItemLevel / 40 + 5;
		}
		else
		{
			if ( (Attribute2 & 0x3F)  > 0 )
			{
				if ( iChaosItem != 0 )
				{
					m_DamageMax += iChaosItem;
				}
				else if ( p->Level != 0 )
				{
					m_DamageMax += (m_DamageMin * 25) / p->Level + 5;
				}
			}
		}

/*		m_DamageMax += m_Level*3;

		if( m_Level >= 10 )
		{
			m_DamageMax += (m_Level - 9) * (m_Level - 8) / 2;
		}*/

		int tmpLevel = (m_Level>9) ? 9 : m_Level;
		m_DamageMax += tmpLevel*3;

		switch(m_Level-9)
		{
		case 6: m_DamageMax += 9;
		case 5:	m_DamageMax += 8;
		case 4:	m_DamageMax += 7;
		case 3:	m_DamageMax += 6;
		case 2:	m_DamageMax += 5;
		case 1:	m_DamageMax += 4;
		}
	}

	if ( m_DamageMin > 0 )	
	{
		if ( m_SetOption != 0 && ItemLevel != 0 )
		{
			m_DamageMin += (m_DamageMin * 25) / p->Level + 5;
			m_DamageMin += ItemLevel / 40 + 5;
		}
		else
		{
			if ( (Attribute2 & 0x3F)  > 0 )
			{
				if ( iChaosItem != 0 )
				{
					m_DamageMin += iChaosItem;
				}
				else if ( p->Level != 0 )
				{
					m_DamageMin += (m_DamageMin * 25) / p->Level + 5;
				}
			}
		}

		/*m_DamageMin += m_Level* 3;

		if( m_Level >= 10 )
		{
			m_DamageMin += (m_Level - 9) * (m_Level - 8) / 2;
		}*/
		int tmpLevel = (m_Level>9) ? 9 : m_Level;
		m_DamageMin += tmpLevel*3;

		switch(m_Level-9)
		{
		case 6: m_DamageMin += 9;
		case 5:	m_DamageMin += 8;
		case 4:	m_DamageMin += 7;
		case 3:	m_DamageMin += 6;
		case 2:	m_DamageMin += 5;
		case 1:	m_DamageMin += 4;
		}
	}

	if ( m_Magic > 0 )	
	{
		if ( m_SetOption != 0 && ItemLevel != 0 )
		{
			m_Magic += (m_Magic * 25) / p->Level + 5;
			m_Magic += ItemLevel / 60 + 2;
		}
		else
		{
			if ( (Attribute2 & 0x3F)  > 0 )
			{
				if ( iChaosItem != 0 )
				{
					m_Magic += iChaosItem;
				}
				else if ( p->Level != 0 )
				{
					m_Magic += (m_Magic * 25) / p->Level + 5;
				}
			}
		}

		/*m_Magic += m_Level* 3;

		if( m_Level >= 10 )
		{
			m_Magic += (m_Level - 9) * (m_Level - 8) / 2;
		}*/

		int tmpLevel = (m_Level>9) ? 9 : m_Level;
		m_Magic += tmpLevel*3;

		switch(m_Level-9)
		{
		case 6: m_Magic += 9;
		case 5:	m_Magic += 8;
		case 4:	m_Magic += 7;
		case 3:	m_Magic += 6;
		case 2:	m_Magic += 5;
		case 1:	m_Magic += 4;
		}
	}

	if ( m_CurseMagic > 0 )
	{
		if ( m_SetOption != 0 && ItemLevel != 0 )
		{
			m_CurseMagic += (m_CurseMagic * 25) / p->Level + 5;
			m_CurseMagic += ItemLevel / 60 + 2;
		}
		else
		{
			if ( (Attribute2 & 0x3F)  > 0 )
			{
				if ( iChaosItem != 0 )
				{
					m_CurseMagic += iChaosItem;
				}
				else if ( p->Level != 0 )
				{
					m_CurseMagic += (m_CurseMagic * 25) / p->Level + 5;
				}
			}
		}

		/*m_CurseMagic += m_Level* 3;

		if( m_Level >= 10 )
		{
			m_CurseMagic += (m_Level - 9) * (m_Level - 8) / 2;
		}*/
		int tmpLevel = (m_Level>9) ? 9 : m_Level;
		m_CurseMagic += tmpLevel*3;

		switch(m_Level-9)
		{
		case 6: m_CurseMagic += 9;
		case 5:	m_CurseMagic += 8;
		case 4:	m_CurseMagic += 7;
		case 3:	m_CurseMagic += 6;
		case 2:	m_CurseMagic += 5;
		case 1:	m_CurseMagic += 4;
		}
	}

	if ( p->SuccessfulBlocking > 0 )	
	{
		if ( m_SetOption != 0 && ItemLevel != 0 )
		{
			m_SuccessfulBlocking += (m_SuccessfulBlocking * 25) / p->Level + 5;
			m_SuccessfulBlocking += ItemLevel / 40 + 5;
		}
		else
		{
			if ( (Attribute2 & 0x3F)  > 0 )
			{
				if ( p->Level != 0 )
				{
					m_SuccessfulBlocking += (m_SuccessfulBlocking * 25) / p->Level + 5;
				}
			}
		}

		/*m_SuccessfulBlocking += m_Level* 3;

		if ( m_Level >= 10 )
		{
			m_SuccessfulBlocking += (m_Level - 9) * (m_Level - 8) / 2;
		}*/

		int tmpLevel = (m_Level>9) ? 9 : m_Level;
		m_SuccessfulBlocking += tmpLevel*3;

		switch(m_Level-9)
		{
		case 6: m_SuccessfulBlocking += 9;
		case 5:	m_SuccessfulBlocking += 8;
		case 4:	m_SuccessfulBlocking += 7;
		case 3:	m_SuccessfulBlocking += 6;
		case 2:	m_SuccessfulBlocking += 5;
		case 1:	m_SuccessfulBlocking += 4;
		}
	}

	if ( p->Defense > 0 )	
	{
		if ( m_Type >= ITEMGET(6,0) && m_Type < ITEMGET(7,0) )
		{
			m_Defense += m_Level;

			if ( m_SetOption != 0 && ItemLevel != 0 )
			{
				m_Defense += (m_Defense * 20) / ItemLevel + 2;
			}
		}
		else 
		{
			if ( m_SetOption != 0 && ItemLevel != 0 )
			{
				m_Defense += (m_Defense * 12) / p->Level + (p->Level / 5 ) + 4;
				m_Defense += (m_Defense * 3) / ItemLevel  + ( ItemLevel / 30 ) + 2;
			}
			else if ( (Attribute2 & 0x3F) > 0 )
			{
				if ( p->Level != 0 )
				{
					m_Defense += (m_Defense * 12) / p->Level + (p->Level / 5 ) + 4;
				}
			}

			if ( (_type >= ITEMGET(12,3) && _type <= ITEMGET(12,6)) ||
				 _type == ITEMGET(12,42) ||
				 _type == ITEMGET(13,4) )
			{
				int tmpLevel = (m_Level>9) ? 9 : m_Level;
				m_Defense += tmpLevel*2;
			}
			else if( _type == ITEMGET(13,30) ||	_type == ITEMGET(12,49) )
			{
				int tmpLevel = (m_Level>9) ? 9 : m_Level;
				m_Defense += tmpLevel*2;
			}
			else if ( (_type >= ITEMGET(12,36) && _type <= ITEMGET(12,40)) || _type == ITEMGET(12,43) || _type == ITEMGET(12,50) )
			{
				int tmpLevel = (m_Level>9) ? 9 : m_Level;
				m_Defense += tmpLevel*4;
			}
#if (CUSTOM_WINGS == 1)
			else if ( _type >= ITEMGET(12,440) && _type <= ITEMGET(12,445) )
			{
				int tmpLevel = (m_Level>9) ? 9 : m_Level;
				m_Defense += tmpLevel*5;
			}
#endif
			else
			{
				int tmpLevel = (m_Level>9) ? 9 : m_Level;
				m_Defense += tmpLevel*3;
			}

			if ( (_type >= ITEMGET(12,36) && _type <= ITEMGET(12,40)) || _type == ITEMGET(12,43) || _type == ITEMGET(12,50) )
			{
				switch(m_Level-9)
				{
				case 6: m_Defense += 10;
				case 5:	m_Defense += 9;
				case 4:	m_Defense += 8;
				case 3:	m_Defense += 7;
				case 2:	m_Defense += 6;
				case 1:	m_Defense += 5;
				}
			}
			else
			{
				switch(m_Level-9)
				{
				case 6: m_Defense += 9;
				case 5:	m_Defense += 8;
				case 4:	m_Defense += 7;
				case 3:	m_Defense += 6;
				case 2:	m_Defense += 5;
				case 1:	m_Defense += 4;
				}
			}
		}
	}

	if ( _type == ITEMGET(13,30) )
	{
		m_Defense += 15;
	}

	if ( p->MagicDefense > 0 )
	{
		/*
		m_MagicDefense += m_Level* 3;

		if( m_Level >= 10 )
		{
			m_MagicDefense += (m_Level - 9) * (m_Level - 8) / 2;
		}*/

		int tmpLevel = (m_Level>9) ? 9 : m_Level;
		m_MagicDefense += tmpLevel*3;

		switch(m_Level-9)
		{
		case 6: m_MagicDefense += 9;
		case 5:	m_MagicDefense += 8;
		case 4:	m_MagicDefense += 7;
		case 3:	m_MagicDefense += 6;
		case 2:	m_MagicDefense += 5;
		case 1:	m_MagicDefense += 4;
		}
	}

	m_Level &= 15;

	memset(m_Special, 0, sizeof(m_Special));
	memset(m_SpecialValue, 0, sizeof(m_SpecialValue));

	m_SpecialNum = 0;
	m_Option1 = 0;
	m_Option2 = 0;
	m_Option3 = 0;
	m_SkillChange = FALSE;

	if ( Option1 != 0 )
	{
		if ( p->SkillType != 0 )
		{
			if ( p->SkillType == AT_SKILL_LONGSPEAR )
			{
				m_SkillChange = TRUE;
				m_Special[m_SpecialNum] = 0;
				m_Option1 = 1;
			}
			else
			{
				m_Special[m_SpecialNum] = p->SkillType;
				m_Option1 = 1;
			}
		}
	}

	if ( m_Type == ITEMGET(13,3) )	// Dinorant
	{
		m_Option1 = TRUE;
		m_Special[m_SpecialNum] = AT_SKILL_KNIGHTDINORANT;
	}

	if ( m_Type == ITEMGET(13,4) )	//Dark Horse
	{
		m_Option1 = TRUE;
		m_Special[m_SpecialNum] = AT_SKILL_DARKHORSE_ATTACK;
	}

	if ( m_Type == ITEMGET(13,37) )
	{
		m_Option1 = TRUE;
		m_Special[m_SpecialNum] = AT_SKILL_FENRIR_ATTACK;
	}

	m_SpecialNum++;

	if ( Option2 != 0 )
	{
		if ( _type >= ITEMGET(0,0) && _type < ITEMGET(12,0) )
		{
			m_Special[m_SpecialNum] = 84;
			m_Option2 = 1;
		}


#if ( CUSTOM_WINGS == 1 )

		if ( _type >= ITEMGET(12,440) && _type <= ITEMGET(12,445) )
		{
			m_Special[m_SpecialNum] = 84;
			m_Option2 = 1;
		}

#endif

		if ( _type >= ITEMGET(12,0) && _type <= ITEMGET(12,6) )	// Wings
		{
			m_Special[m_SpecialNum] = 84;
			m_Option2 = 1;
		}

		if ( _type == ITEMGET(13,30) )	// Cape of Lord
		{
			m_Special[m_SpecialNum] = 84;
			m_Option2 = 1;
		}
		
		if ( _type >= ITEMGET(12,36) && _type <= ITEMGET(12,40) )
		{
			m_Special[m_SpecialNum] = 84;
			m_Option2 = 1;
		}
		
		if ( _type >= ITEMGET(12,41) && _type <= ITEMGET(12,43) )
		{
			m_Special[m_SpecialNum] = 84;
			m_Option2 = 1;
		}

		if ( _type == ITEMGET(12,49) || _type == ITEMGET(12,50) )
		{
			m_Special[m_SpecialNum] = 84;
			m_Option2 = 1;
		}

		if ( _type >= ITEMGET(12,262) && _type <= ITEMGET(12,267) )
		{
			m_Special[m_SpecialNum] = 84;
			m_Option2 = 1;
		}
	}

	m_SpecialNum++;

	if ( Option3 != 0 )
	{
		if ( _type >= ITEMGET(0,0) && _type < ITEMGET(5,0) )
		{
			m_Special[m_SpecialNum] = 80;
			m_Option3 = Option3;
			//m_RequireStrength +=  Option3 * 4;
		}

		if ( _type >= ITEMGET(5,0) && _type < ITEMGET(6,0) )
		{
			if( p->ItemSlot == 1 )
			{
				m_Special[m_SpecialNum] = 113;
			}
			else
			{
				m_Special[m_SpecialNum] = 81;
			}

			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;
		}
		
		if ( _type >= ITEMGET(6,0) && _type < ITEMGET(7,0) )
		{
			m_Special[m_SpecialNum] = 82;
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;
		}

		if ( _type >= ITEMGET(7,0) && _type < ITEMGET(12,0) )
		{
			m_Special[m_SpecialNum] = 83;
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 5;
		}

		/*if ( (_type >= ITEMGET(13,8) && _type < ITEMGET(13,14)) || (_type >= ITEMGET(13,20) && _type <= ITEMGET(13,28)) )
		{
			if ( _type == ITEMGET(13,24) ) // Ring of MAgic
			{
				m_Special[m_SpecialNum] = 172;
				m_Option3 = Option3;
			}
			else if ( _type == ITEMGET(13,28) ) // Pendant of abilit
			{
				m_Special[m_SpecialNum] = 173;
				m_Option3 = Option3;
			}
			else  
			{
				m_Special[m_SpecialNum] = 85;
				m_Option3 = Option3;
			}
		}*/
		if ((_type >= ITEMGET(13,8) && _type < ITEMGET(13,14)) || (_type >= ITEMGET(13,20) && _type <= ITEMGET(13,28)) || (_type >= ITEMGET(13,109) && _type <= ITEMGET(13,115)))
		{
			if (_type == ITEMGET(13,24))
			{
				m_Special[m_SpecialNum] = 172;
				m_Option3 = Option3;
			}
			else if (_type == ITEMGET(13,28))
			{
				m_Special[m_SpecialNum] = 173;
				m_Option3 = Option3;
			}
			else  
			{
				m_Special[m_SpecialNum] = 85;
				m_Option3 = Option3;
			}
		}
		
		if ( _type == ITEMGET(13,30) ) // Cape of lord 
		{
			m_Special[m_SpecialNum] = 80;
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;
		}

		if ( _type == ITEMGET(12,0) )	// Wing elf
		{
			m_Special[m_SpecialNum] = 85;
			m_Option3 = Option3;
		}
		else if ( _type == ITEMGET(12,1) ) // winf Heaven
		{
			m_Special[m_SpecialNum] = 81;
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;
		}
		else if ( _type == ITEMGET(12,2) ) // wing devil
		{
			m_Special[m_SpecialNum] = 80;
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;
		}
		else if ( _type == ITEMGET(12,3) ) // wing spitits
		{
			m_Special[m_SpecialNum] = 80;
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;

			if ( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 85;
			}
			else
			{
				m_Special[m_SpecialNum] = 80;
			}

		}
		else if ( _type == ITEMGET(12,4) ) // wing soul
		{
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;

			if ( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 81;
			}
			else
			{
				m_Special[m_SpecialNum] = 85;
			}

		}
		else if ( _type == ITEMGET(12,5) ) // wing dragon
		{
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;

			if ( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else
			{
				m_Special[m_SpecialNum] = 85;
			}

		}			
		else if ( _type == ITEMGET(12,6) ) // wing darkness
		{
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;

			if ( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else
			{
				m_Special[m_SpecialNum] = 81;
			}

		}
		else if ( _type == ITEMGET(12,49) ) 
		{
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;

			if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else
			{
				m_Special[m_SpecialNum] = 85;
			}
		}
		else if ( _type == ITEMGET(13,3) ) // dinorant
		{
			m_Option3 = Option3;
	
			if ( (m_Option3&0x02) != 0 )
			{
				m_Special[m_SpecialNum] = 0x67;
				m_SpecialNum++;
			}
		
			if ( (m_Option3&0x04) != 0 )
			{
				m_Special[m_SpecialNum] = 0x61;
				m_SpecialNum++;
				m_AttackSpeed+=5;
			}
		
			if ( (m_Option3&0x01) != 0 )
			{
				m_Special[m_SpecialNum] = 0x68;
			}
		}

		if ( _type == ITEMGET(12,36) ) // wings of storm
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 83;
			}
		}
		if ( _type == ITEMGET(12,37) ) // wings of redemption
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 81;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 83;
			}
		}
		if ( _type == ITEMGET(12,38) ) // wings of fortitude
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 83;
			}
		}
		if ( _type == ITEMGET(12,39) ) // wings of hurricane
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 81;
			}
		}

#if ( CUSTOM_WINGS == 1)
		if ( _type == ITEMGET(12,440) || _type == ITEMGET(12,441) || _type == ITEMGET(12,444) )
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 83;
			}
		}
		else if ( _type == ITEMGET(12,442) )
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 81;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 83;
			}
		}
		else if ( _type == ITEMGET(12,443) )
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 81;
			}
		}
		else if ( _type == ITEMGET(12,445)	)
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 81;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 0x71;
			}
		}
#endif
		
		if ( _type == ITEMGET(12,40) )
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 83;
			}
		}
		else if ( _type == ITEMGET(12,41) ) // summoner 1st wings
		{
			m_Special[m_SpecialNum] = 81;
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;
		}
		else if ( _type == ITEMGET(12,42)	) // summoner 2nd wings
		{
			m_Option3 = Option3;
			m_RequireStrength +=  Option3 * 4;

			if ( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 81;
			}
			else
			{
				m_Special[m_SpecialNum] = 0x71;
			}
		}
		else if ( _type == ITEMGET(12,43)	)
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 81;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 0x71;
			}
		}
		else if ( _type == ITEMGET(12,50) )
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
			else if( (m_NewOption&0x20) != 0 )
			{
				m_Special[m_SpecialNum] = 83;
			}
		}
		else if ( _type == ITEMGET(12,262) ||
				  _type == ITEMGET(12,263) ||
				  _type == ITEMGET(12,265) )	// DL / RF / DK / ELF/MG	2.5
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 80;
			}
		}
		else if ( _type == ITEMGET(12,264) )	// DW/SUM/MG 2.5
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;

			if ( (m_NewOption&0x10) != 0 )
			{
				m_Special[m_SpecialNum] = 81;
			}
		}
		else if ( _type == ITEMGET(12,266) )
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;
		}
		else if ( _type == ITEMGET(12,267) )
		{
			m_Option3 = Option3;
			m_Special[m_SpecialNum] = 85;
		}
	}

	m_SpecialNum++;

	if ( (_type >=ITEMGET(6,0) && _type <ITEMGET(12,0) ) || ( _type >=ITEMGET(13,8) && _type <= ITEMGET(13,9)) || (_type>=ITEMGET(13,21) && _type <= ITEMGET(13,24) ) )
	{
		if ( ((m_NewOption>>5)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x56;
			m_SpecialNum++;
		}

		if ( ((m_NewOption>>4)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x57;
			m_SpecialNum++;
		}

		if ( ((m_NewOption>>3)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x58;
			m_SpecialNum++;
		}

		if ( ((m_NewOption>>2)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x59;
			m_SpecialNum++;
		}

		if ( ((m_NewOption>>1)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x5A;
			m_SpecialNum++;
		}

		if ( ((m_NewOption)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x5B;
			m_SpecialNum++;
		}
	}

	/*if ( (_type >=ITEMGET(0,0) && _type <ITEMGET(6,0) ) || ( _type >=ITEMGET(13,12) && _type <= ITEMGET(13,13)) || (_type>=ITEMGET(13,25) && _type <= ITEMGET(13,28) ) )
	{
		if ( ((m_NewOption>>5)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x5C;
			m_SpecialNum++;
		}

		if ( (_type >=ITEMGET(5,0) && _type <ITEMGET(6,0) ) || (_type ==ITEMGET(13,12) ) || (_type == ITEMGET(13,25)) || (_type == ITEMGET(13,27) ) )
		{
			if ( ((m_NewOption>>4)&1) != 0 )
			{
				m_Special[m_SpecialNum] = 0x5F;
				m_SpecialNum++;
			}
			if ( ((m_NewOption>>3)&1) != 0 )
			{
				m_Special[m_SpecialNum] = 0x60;
				m_SpecialNum++;
			}
		}
		else
		{
			if ( ((m_NewOption>>4)&1) != 0 )
			{
				m_Special[m_SpecialNum] = 0x5D;
				m_SpecialNum++;
			}
			if ( ((m_NewOption>>3)&1) != 0 )
			{
				m_Special[m_SpecialNum] = 0x5E;
				m_SpecialNum++;
			}
		}

		if ( ((m_NewOption>>2)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x61;
			m_SpecialNum++;
		}

		if ( ((m_NewOption>>1)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x62;
			m_SpecialNum++;
		}

		if ( ((m_NewOption)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x63;
			m_SpecialNum++;
		}
	}*/

	if ((_type >= ITEMGET(0,0)    && _type <ITEMGET(6,0) )    ||
		(_type >= ITEMGET(13,12)  && _type <= ITEMGET(13,13)) ||
		(_type >= ITEMGET(13,25)  && _type <= ITEMGET(13,28))	||
		_type == ITEMGET(13,109) || _type == ITEMGET(13,113) || 
		_type == ITEMGET(13,114) || _type == ITEMGET(13,115))
	{
		if (((m_NewOption>>5)&1) != 0)
		{
			m_Special[m_SpecialNum] = 0x5C;
			m_SpecialNum++;
		}
		
		if ((_type >= ITEMGET(5,0)    &&  _type <ITEMGET(6,0))      ||
			(_type == ITEMGET(13,12)) || (_type == ITEMGET(13,25))  ||
			(_type == ITEMGET(13,27)) || (_type == ITEMGET(13,109)) ||
			(_type == ITEMGET(13,115)))
		{
			if (((m_NewOption>>4)&1) != 0)
			{
				m_Special[m_SpecialNum] = 0x5F;
				m_SpecialNum++;
			}
			
			if (((m_NewOption>>3)&1) != 0)
			{
				m_Special[m_SpecialNum] = 0x60;
				m_SpecialNum++;
			}
		}
		else
		{
			if (((m_NewOption>>4)&1) != 0)
			{
				m_Special[m_SpecialNum] = 0x5D;
				m_SpecialNum++;
			}
			if (((m_NewOption>>3)&1) != 0)
			{
				m_Special[m_SpecialNum] = 0x5E;
				m_SpecialNum++;
			}
		}
		
		if (((m_NewOption>>2)&1) != 0)
		{
			m_Special[m_SpecialNum]		= 0x61;
			m_SpecialNum++;
			m_AttackSpeed					   += 7;
		}
		
		if (((m_NewOption>>1)&1) != 0)
		{
			m_Special[m_SpecialNum]		= 0x62;
			m_SpecialNum++;
		}
		
		if (((m_NewOption)&1) != 0)
		{
			m_Special[m_SpecialNum]		= 0x63;
			m_SpecialNum++;
		}
	}

	if ( ((_type >= ITEMGET(12,3)) && (_type <= ITEMGET(12,6))) || _type ==ITEMGET(13,30) || _type == ITEMGET(12,42) || _type == ITEMGET(12,49) )
	{
		if ( ((m_NewOption)&1) != 0 )
		{
			m_Special[m_SpecialNum] = 0x64;
			m_SpecialNum++;
		}

		if ( ((m_NewOption)&2) != 0 )
		{
			m_Special[m_SpecialNum] = 0x65;
			m_SpecialNum++;
		}

		if ( ((m_NewOption)&4) != 0 )
		{
			m_Special[m_SpecialNum] = 0x66;
			m_SpecialNum++;
		}

		if ( ((m_NewOption)&8) != 0 )
		{
			m_Special[m_SpecialNum] = 0x69;
			m_SpecialNum++;
		}
	}

	if ( _type == ITEMGET(13,37) )
	{
		if ( (m_NewOption &1) )
		{
			m_Special[m_SpecialNum] = 0x6A;
			m_SpecialNum++;
		}
		else if ( (m_NewOption &2) )
		{
			m_Special[m_SpecialNum] = 0x6B;
			m_SpecialNum++;
		}
		else if ( (m_NewOption &4) )
		{
			m_Special[m_SpecialNum] = 0x70;
			m_SpecialNum++;
		}
	}

	if ( (_type >= ITEMGET(12,36) && _type <= ITEMGET(12,40)) || _type == ITEMGET(12,43) || _type == ITEMGET(12,50) )
	{
		if ( (m_NewOption &1) )
		{
			m_Special[m_SpecialNum] = 0x6C;
			m_SpecialNum++;
		}

		if ( (m_NewOption &2) )
		{
			m_Special[m_SpecialNum] = 0x6D;
			m_SpecialNum++;
		}

		if ( (m_NewOption &4) )
		{
			m_Special[m_SpecialNum] = 0x6E;
			m_SpecialNum++;
		}

		if ( (m_NewOption &8) )
		{
			m_Special[m_SpecialNum] = 0x6F;
			m_SpecialNum++;
		}
	}

#if (CUSTOM_WINGS == 1)
	if ( _type >= ITEMGET(12,440) && _type <= ITEMGET(12,445) )
	{
		if ( (m_NewOption &1) )
		{
			m_Special[m_SpecialNum] = 0x6C;
			m_SpecialNum++;
		}

		if ( (m_NewOption &2) )
		{
			m_Special[m_SpecialNum] = 0x6D;
			m_SpecialNum++;
		}

		if ( (m_NewOption &4) )
		{
			m_Special[m_SpecialNum] = 0x6E;
			m_SpecialNum++;
		}

		if ( (m_NewOption &8) )
		{
			m_Special[m_SpecialNum] = 0xC8;
			m_SpecialNum++;
		}
	}
#endif

	if ( _type >= ITEMGET(12,262) && _type <= ITEMGET(12,265) )
	{
		if ( (m_NewOption &1) )
		{
			m_Special[m_SpecialNum] = 0x6C;
			m_SpecialNum++;
		}

		if ( (m_NewOption &2) )
		{
			m_Special[m_SpecialNum] = 0x6E;
			m_SpecialNum++;
		}
	}

	if ( SOptionStatValue != 0 )
	{
		if (p->SetAttr != 0 )
		{
			m_Special[m_SpecialNum] = p->SetAttr + 0xC3;
			m_SetAddStat = SOptionStatValue * 5;
			m_SpecialNum++;
		}
	}

	m_Part = p->ItemSlot;
	Value();

	if ( m_Type != ITEMGET(4,7) && m_Type != ITEMGET(4,15) ) // arrow and bolts
	{
		m_DamageMinOrigin = m_DamageMin;
		m_DefenseOrigin = m_Defense;
		m_DamageMin -= (int)(m_DamageMin * m_CurrentDurabilityState);
		m_DamageMax -= (int)(m_DamageMax * m_CurrentDurabilityState);
		m_Defense -= (int)(m_Defense * m_CurrentDurabilityState);
		m_SuccessfulBlocking -= (int)(m_SuccessfulBlocking * m_CurrentDurabilityState);

		if ( m_Durability < 1.0f )
		{
			m_AttackSpeed = 0;
		}
	}
	
	if ( m_Durability == 0.0f )
	{
		memset(m_Resistance , 0, sizeof(m_Resistance));
	}

	int ItemIndex = m_Type/MAX_SUBTYPE_ITEMS;
	int ItemNumber = m_Type%MAX_SUBTYPE_ITEMS;

	m_bLuckySet = FALSE;

	if( ItemIndex >= 7 && ItemIndex <= 11 )
	{
		if( ItemNumber >= 62 && ItemNumber <= 72 )
		{
			m_bLuckySet = TRUE;
		}
	}
}

 
int CItem::GetLevel() 
{
	return ItemAttribute[m_Type].Level;
}

DWORD CItem::GetNumber()
{
	return m_Number;
}

void CItem::Value()
{
	if ( m_Type == -1 )
	{
		return;
	}

	LPITEM_ATTRIBUTE p = &ItemAttribute[m_Type];

	if( p->BuyMoney != 0 )
	{
		m_SellMoney = p->BuyMoney;
		m_BuyMoney = p->BuyMoney;
		m_SellMoney /= 3;
		
		if ( m_SellMoney >= 1000 )
		{
			m_SellMoney = m_SellMoney/100*100;
		}
		else if ( m_SellMoney >= 100 )
		{
			m_SellMoney = m_SellMoney/10*10;
		}

		if ( m_BuyMoney >= 1000 )
		{
			m_BuyMoney = m_BuyMoney/100*100;
		}
		else if ( m_BuyMoney >= 100 )
		{
			m_BuyMoney = m_BuyMoney/10*10;
		}

		return;
	}

	int Level = sqrt(double(m_Level));
	__int64 Gold = 0;
	int Type = (m_Type)/MAX_SUBTYPE_ITEMS;
	int Level2 = p->Level + m_Level*3;
	int excellent = 0;

	for (int i=0;i<m_SpecialNum;i++)
	{
		switch ( m_Special[i] )
		{
			case 0x56:	// 86.
			case 0x57:
			case 0x58:
			case 0x59:
			case 0x5A:
			case 0x5B:
			case 0x5C:
			case 0x5D:
			case 0x5E:
			case 0x5F:
			case 0x60:
			case 0x61:
			case 0x62:
			case 0x63:	// 99.
				excellent = 1;
		}
	}

	if ( excellent != 0 )
	{
		Level2 += 25;
	}

	if ( m_Type == ITEMGET(4,15) ) // Arrow
	{
		int lc10 = 70;

		if ( m_Level == 1 )
		{
			lc10 = 1200;
		}
		else if ( m_Level == 2 )
		{
			lc10 = 2000;
		}
		else if ( m_Level == 3 )
		{
			lc10 = 2800;
		}

		if ( m_Durability > 0.0f )
		{
			Gold = lc10 * m_Durability/p->Durability;
		}
	}
	else if ( m_Type == ITEMGET(4,7) ) // Arrow of Crossbow
	{
		int lc11;

		if ( m_Durability > 0.0f )
		{
			lc11 = 100;

			if ( m_Level == 1 )
			{
				lc11 = 1400;
			}
			else if ( m_Level == 2 )
			{
				lc11 = 2200;
			}
			else if ( m_Level == 3 )
			{
				lc11 = 3000;
			}
		
			Gold = lc11 * m_Durability/p->Durability;
		}
	}
	else if ( m_Type == ITEMGET(14,13) ) // Bless
	{
		Gold = 4500000;
	}
	else if ( m_Type == ITEMGET(14,14) ) // soul
	{
		Gold = 3000000;
	}
	else if ( m_Type == ITEMGET(12,15) ) // chaos
	{
		Gold = 810000;
	}
	else if ( m_Type == ITEMGET(14,16) ) // Life
	{
		Gold = 22500000;
	}
	else if ( m_Type == ITEMGET(14,22) ) // Creation
	{
		Gold = 18000000;
	}
	else if ( m_Type == ITEMGET(12,30) ) //Pack Of Bless
	{
		Gold = (m_Level+1)*4500000*10;
	}
	else if ( m_Type == ITEMGET(12,31) ) //Pack Of Soul
	{
		Gold = (m_Level+1)*3000000*10;
	}
	else if ( m_Type == ITEMGET(13,15) ) //Fruits
	{
		Gold = 33000000;
	}
	else if ( m_Type == ITEMGET(13,14) ) //Loch Feather
	{
		if ( m_Level == 1 )
		{
			Gold = 7500000;	// Crest ofMonarch
		}
		else
		{
			Gold = 180000;	// Blue Feather
		}
	}
	else if ( m_Type == ITEMGET(14,31) ) // Jewel of Guardian
	{
		Gold = 30000000;
	}
	else if ( m_Type == ITEMGET(14,7) ) // Siege Potion
	{
		if ( m_Level == 0 )
		{
			Gold = (int)(m_Durability) * 900000;
		}
		else if ( m_Level == 1 )
		{
			Gold = (int)(m_Durability) * 450000;
		}
	}
	else if ( m_Type == ITEMGET(13,11) ) // Order(Guardian/Life Stone)
	{
		if ( m_Level == 0 )
		{
			Gold = 1000000;
		}
		else if ( m_Level == 1 )
		{
			Gold = 2400000;
		}
	}
	else if ( m_Type == ITEMGET(13,7) ) // Contract(Summon)
	{
		if ( m_Level == 0 )
		{
			Gold = 1500000;
		}
		else if ( m_Level == 1 )
		{
			Gold = 1200000;
		}
	}
	else if ( m_Type == ITEMGET(13,32) ) 
	{
		Gold = (int)m_Durability * 150;
	}
	else if ( m_Type == ITEMGET(13,33) ) 
	{
		Gold = (int)m_Durability * 300;
	}
	else if ( m_Type == ITEMGET(13,34) ) 
	{
		Gold = (int)m_Durability * 3000;
	}
	else if ( m_Type == ITEMGET(13,35) ) 
	{
		Gold = 30000;
	}
	else if ( m_Type == ITEMGET(13,36) ) 
	{
		Gold = 90000;
	}
	else if ( m_Type == ITEMGET(13,37) ) 
	{
		Gold = 150000;
	}
	else if ( m_Type == ITEMGET(14,35) ) 
	{
		Gold = (int)m_Durability * 2000;
	}
	else if ( m_Type == ITEMGET(14,36) ) 
	{
		Gold = (int)m_Durability * 4000;
	}
	else if ( m_Type == ITEMGET(14,37) ) 
	{
		Gold = (int)m_Durability * 6000;
	}
	else if ( m_Type == ITEMGET(14,38) ) 
	{
		Gold = (int)m_Durability * 2500;
	}
	else if ( m_Type == ITEMGET(14,39) ) 
	{
		Gold = (int)m_Durability * 5000;
	}
	else if ( m_Type == ITEMGET(14,40) ) 
	{
		Gold = (int)m_Durability * 7500;
	}
	else if (m_Type >= ITEMGET(13,109) && m_Type <= ITEMGET(13,115))
	{
		Gold = 3000;
	}
	else if ( m_Type == ITEMGET(13,3) ) // Dinorant
	{
		Gold = 960000;

		for ( int k=0;k<3;k++)
		{
			if ( (m_Option3 & (1<<k)) != 0 )
			{
				Gold += 300000;
			}
		}
	}
	else if ( m_Type == ITEMGET(14,17) ) // Devil Eye
	{
		switch ( m_Level )
		{
			case 1:
				Gold = 10000;
				break;
			case 2:
				Gold = 50000;
				break;
			case 3:
				Gold = 100000;
				break;
			case 4:
				Gold = 300000;
				break;
			case 5:
				Gold = 500000;
				break;
			case 6:
				Gold = 800000;
				break;
			case 7:
				Gold = 1000000;
				break;
			default:
				Gold = 30000;
				break;
		}
	}
	else if ( m_Type == ITEMGET(14,18) ) // Devil Key
	{
		switch ( m_Level )
		{
			case 1:
				Gold = 15000;
				break;
			case 2:
				Gold = 75000;
				break;
			case 3:
				Gold = 150000;
				break;
			case 4:
				Gold = 450000;
				break;
			case 5:
				Gold = 750000;
				break;
			case 6:
				Gold = 1200000;
				break;
			case 7:
				Gold = 1500000;
				break;
			default:
				Gold = 30000;
				break;
		}
	}
	else if ( m_Type == ITEMGET(14,19) ) // Devil's Invitation
	{
		switch ( m_Level )
		{
			case 1:
				Gold = 60000;
				break;
			case 2:
				Gold = 84000;
				break;
			case 3:
				Gold = 120000;
				break;
			case 4:
				Gold = 180000;
				break;
			case 5:
				Gold = 240000;
				break;
			case 6:
				Gold = 300000;
				break;
			case 7:
				Gold = 180000;
				break;
			default:
				Gold = 120000;
				break;
		}
	}
	else if ( m_Type == ITEMGET(14,20) ) // Remedy Of Love
	{
		Gold = 900;
	}
	else if ( m_Type == ITEMGET(14,21) ) // Rena
	{
		switch ( m_Level )
		{
			case 0:	// Rena
				Gold = 9000;
				m_Durability = m_BaseDurability;
			case 1:	// Stone
				Gold = 9000;
				m_Durability = m_BaseDurability;
				break;
			case 3:	// Mark of Lord
				Gold = (int)(m_Durability) * 3900;
				break;
			default:
				Gold = 9000;
				m_Durability = m_BaseDurability;
		}
	}
	else if ( m_Type == ITEMGET(14,9) && m_Level == 1) // Ale
	{
		Gold = 1000;
	}
	else if ( m_Type == ITEMGET(13,18) ) // Invisibility Cloak
	{
		Gold = 200000 + ((m_Level-1) * 20000);

		if( m_Level == 1 )
		{
			Gold = 50000;
		}

		Gold *= 3;
	}
	else if ( m_Type == ITEMGET(13,16) || m_Type == ITEMGET(13,17) ) // Blood and Paper of BloodCastle
	{
		switch ( m_Level )
		{
			case 1:
				Gold = 10000;
				break;
			case 2:
				Gold = 50000;
				break;
			case 3:
				Gold = 100000;
				break;
			case 4:
				Gold = 300000;
				break;
			case 5:
				Gold = 500000;
				break;
			case 6:
				Gold = 800000;
				break;
			case 7:
				Gold = 1000000;
				break;
			case 8:
				Gold = 1200000;
				break;
			default:
				Gold = 0;
				break;
		}
	}
	else if ( m_Type == ITEMGET(13,29) ) // Armor of Guardman
	{
		Gold = 5000;
	}
	else if ( m_Type == ITEMGET(13,20) ) // Wizards Ring
	{
		switch ( m_Level )
		{
			case 0:
				Gold =30000;
				break;
			default:
				Gold = 0;
		}
	}
	else if ( m_Type == ITEMGET(13,31) ) // Spirit
	{
		switch ( m_Level )
		{
			case 0:
				Gold = 30000000;
				break;

			case 1:
				Gold = 15000000;
		}
	}
	else if ( m_Type == ITEMGET(14,28) ) // Lost Map
	{
		Gold = 600000;
	}
	else if ( m_Type == ITEMGET(14,29) ) // Symbol of Kundun
	{
		Gold = ((10000.0f) * m_Durability) * 3.0f;
	}
	else if ( m_Type == ITEMGET(14,45) ||
		      m_Type == ITEMGET(14,46) ||
			  m_Type == ITEMGET(14,47) ||
			  m_Type == ITEMGET(14,48) ||
			  m_Type == ITEMGET(14,49) ||
			  m_Type == ITEMGET(14,50) )
	{
		Gold = ((50.0f) * m_Durability) * 3.0f;
	}
	else if ( m_Type == ITEMGET(12,26) )
	{
		switch ( m_Level )
		{
			case 0:
				Gold = 60000;
				break;
		}
	}
	else if ( m_Type == ITEMGET(14,51) )
	{
		Gold = 200000;
	}
	else if ( m_Type == ITEMGET(14,55) ||
			  m_Type == ITEMGET(14,56) ||
			  m_Type == ITEMGET(14,57) )
	{
		Gold = 9000;
	}
	else if ( m_Type == ITEMGET(13,49) ||
			  m_Type == ITEMGET(13,50) ||
			  m_Type == ITEMGET(13,51) )
	{
		switch( m_Level )
		{
		case 1:
			Gold = 500000;
			break;
		case 2:
			Gold = 600000;
			break;
		case 3:
			Gold = 800000;
			break;
		case 4:
			Gold = 1000000;
			break;
		case 5:
			Gold = 1200000;
			break;
		case 6:
			Gold = 1400000;
			break;
		default:
			Gold = 9000;
			break;
		}
	}
	else if ( m_Type == ITEMGET(13,52) ||
			  m_Type == ITEMGET(13,53) )
	{
		Gold = 3000000;
	}
	else if( m_Type >= ITEMGET(13,71) && m_Type <= ITEMGET(13,75) )
	{
		Gold = 1000000;
	}
	else if ( m_Type == ITEMGET(14,23) ||
			  m_Type == ITEMGET(14,24) ||
			  m_Type == ITEMGET(14,25) ||
			  m_Type == ITEMGET(14,26) ||
			  m_Type == ITEMGET(14,65) ||
			  m_Type == ITEMGET(14,66) ||
			  m_Type == ITEMGET(14,67) ||
			  m_Type == ITEMGET(14,68) )
	{
		Gold = 9000;
	}
	else if( m_Type == ITEMGET(12,136) ) // life boundle
	{
		Gold = (m_Level+1)*22500000*10;
	}
	else if( m_Type == ITEMGET(12,137) ) // creation bundle
	{
		Gold = (m_Level+1)*18000000*10;
	}
	else if( m_Type == ITEMGET(12,138) ) // guardian bundle
	{
		Gold = (m_Level+1)*30000000*10;
	}
	else if( m_Type == ITEMGET(12,139) ) // gemstone bundle
	{
		Gold = (m_Level+1)*18600*10;
	}
	else if( m_Type == ITEMGET(12,140) ) // harmony boundle
	{
		Gold = (m_Level+1)*18600*10;
	}
	else if( m_Type == ITEMGET(12,141) ) // chaos bundle
	{
		Gold = (m_Level+1)*810000*10;
	}
	else if( m_Type == ITEMGET(12,142) )
	{
		Gold = (m_Level+1)*18600*10;
	}
	else if( m_Type == ITEMGET(12,143) )
	{
		Gold = (m_Level+1)*18600*10;
	}
	else if( g_ElementalSystem.IsPentagram(m_Type) )
	{
		Gold = 100000 + (p->Level*(m_Level+50))*(m_Level*10);
		Gold = Gold*(g_ElementalSystem.GetSlotCount(this) * 0.8f);
	}
	else if( g_ElementalSystem.IsErrtel(m_Type) )
	{
		int level = 0;

		for(int i = 0; i < 5; i++)	{
			if( m_SocketOption[i] == (BYTE)-1 )
				continue;

			level += m_SocketOption[i]/16;
		}

		int glevel = p->Level+level*5;
		Gold = 2*g_ElementalSystem.GetSlotCount(this)*(100 + (glevel+40)*glevel*(glevel/8));
	}
	else if( m_Type == ITEMGET(12,144) || m_Type == ITEMGET(12,146) )
	{
		Gold = 300000 * (int)m_Durability;
	}
	else if( m_Type == ITEMGET(12,145) ||
			 m_Type == ITEMGET(12,147) ||
			 m_Type == ITEMGET(12,148) ||
			 m_Type == ITEMGET(12,149) )
	{
		Gold = 1500000;
	}
	else if( m_Type == ITEMGET(13,145) )
	{
		Gold = 30000 * (int)m_Durability;
	}
	else if( m_Type == ITEMGET(13,146) )
	{
		Gold = 600000;
	}
	else if ( p->Value > 0 )
	{
		Gold += (p->Value * p->Value * 10)/12;

		if ( m_Type >= ITEMGET(14,0) && m_Type <= ITEMGET(14,8) )	// Potions + Antidote
		{

			if ( m_Type == ITEMGET(14,3) || m_Type == ITEMGET(14,6) )
			{
				Gold *= 2;
			}

			if ( m_Level > 0 )
			{
				Gold *= (__int64)(pow(2.0f, Level)) ;
			}

			Gold = Gold/10*10;
			Gold *= (__int64)m_Durability;
			m_BuyMoney = Gold;
			m_SellMoney = Gold/3;
			m_SellMoney = m_SellMoney/10*10;
			return;
		}
	}
	else if ( Type == 12 && ( 
								( m_Type > ITEMGET(12,6) && m_Type < ITEMGET(12,36) ) || 
								( m_Type > ITEMGET(12,43) && m_Type < ITEMGET(12,440) ) ||
								( m_Type > ITEMGET(12,445) ) &&
								( m_Type != ITEMGET(12,50) )
							)
			|| Type == 13 || Type == 15 )
	{
		Gold = Level2*Level2*Level2+100;

		for (int u=0;u<m_SpecialNum;u++)
		{
			switch ( m_Special[u] )
			{
				case 0x55:
					Gold += Gold*m_Option3;
			}
		}
	}
	else if( m_Type == ITEMGET(14,63) )
	{
		Gold = 200000;
	}
	else if( m_Type == ITEMGET(14,85) ||
			 m_Type == ITEMGET(14,86) ||
			 m_Type == ITEMGET(14,87) ||
			 m_Type == ITEMGET(14,90) )
	{
		Gold = (int)m_Durability * 300;
	}
	else if( m_Type == ITEMGET(14,110) )
	{
		Gold = (int)m_Durability * 30000;
	}
	else if( m_Type == ITEMGET(14,111) )
	{
		Gold = 600000;
	}
	else
	{
		switch ( m_Level )
		{
			case  5:	Level2 +=   4;	break;
			case  6:	Level2 +=  10;	break;
			case  7:	Level2 +=  25;	break;
			case  8:	Level2 +=  45;	break;
			case  9:	Level2 +=  65;	break;
			case 10:	Level2 +=  95;	break;
			case 11:	Level2 += 135;	break;
			case 12:	Level2 += 185;	break;
			case 13:	Level2 += 245;	break;
			case 14:	Level2 += 305;	break;
			case 15:	Level2 += 365;	break;
		}

		if ( Type == 12 && m_Type <= ITEMGET(12,6) )	// Wings
		{
			Gold = (Level2+40)*Level2*Level2*11+40000000;
		}
#if (CUSTOM_WINGS == 1)
		else if( Type == 12 && (m_Type >= ITEMGET(12,440) && m_Type <= ITEMGET(12,445)) )
		{
			Gold = (Level2+40)*Level2*Level2*11+40000000;
		}
#endif
		else if( Type == 12 && (m_Type >= ITEMGET(12,36) && m_Type <= ITEMGET(12,40)) )
		{
			Gold = (Level2+40)*Level2*Level2*11+40000000;
		}
		else if( Type == 12 && (m_Type >= ITEMGET(12,41) && m_Type <= ITEMGET(12,43)) )
		{
			Gold = (Level2+40)*Level2*Level2*11+40000000;
		}
		else if( Type == 12 && m_Type == ITEMGET(12,50) )
		{
			Gold = (Level2+40)*Level2*Level2*11+40000000;
		}
		else
		{
			Gold = (Level2+40)*Level2*Level2/8+100;
		}

		if ( Type >= 0 && Type <=  6 )
		{
			if ( p->TwoHand == 0 )
			{
				Gold = Gold*80/100;
			}
		}

		for ( int g=0;g<m_SpecialNum;g++)
		{
			switch ( m_Special[g] )
			{
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 24:
				case 56:
					Gold += (__int64)(Gold * 1.5);
					break;

				case 80:
				case 81:
				case 83:
				case 85:
				case 113:

					switch ( m_Option3 )
					{
						case 1:
							Gold += (__int64)(Gold * 6.0 / 10.0);
							break;

						case 2:
							Gold += (__int64)(Gold * 14.0 / 10.0);
							break;

						case 3:
							Gold += (__int64)(Gold * 28.0 / 10.0);
							break;

						case 4:
							Gold += (__int64)(Gold * 56.0 / 10.0);
							break;
					}
					break;

				case 82:

					switch ( m_Option3 )
					{
					case 1:
							Gold += (__int64)(Gold * 6.0 / 10.0);
							break;

						case 2:
							Gold += (__int64)(Gold * 14.0 / 10.0);
							break;

						case 3:
							Gold += (__int64)(Gold * 28.0 / 10.0);
							break;

						case 4:
							Gold += (__int64)(Gold * 56.0 / 10.0);
							break;
					}
					break;

				case 84:
					Gold += (__int64)(Gold * 25.0 / 100.0);
					break;

				case 86:
				case 87:
				case 88:
				case 89:
				case 90:
				case 91:
				case 92:
				case 93:
				case 94:
				case 95:
				case 96:
				case 97:
				case 98:
				case 99:
					Gold += Gold;
					break;

				case 100:
				case 101:
				case 102:
				case 103:
				case 104:
				case 108:
				case 109:
				case 110:
				case 111:
					Gold += (__int64)(Gold * 25.0 / 100.0);
					break;

			}
		}
	}

	if ( g_kItemSystemFor380.Is380OptionItem(this) == TRUE )
	{
		Gold += (__int64)(Gold * 16.0 / 100.0);
	}

	if ( g_SocketItem.CheckSocketOption(this) == TRUE )
	{
		int count = 0;
		int unk = 0;
		DWORD addzen = 0;

		for(int n = 0; n < MAX_SOCKET_COUNT; n++)
		{
			if( m_SocketOption[n] != (BYTE)-1  )
			{
				if( m_SocketOption[n] != (BYTE)-2 )
				{
					addzen += g_SocketItem.GetOptionValue(m_SocketOption[n]);
				}

				count++;
			}
		}

		Gold = Gold + DWORD( Gold * float(count<<3) / 10.0f);
		Gold = Gold + addzen;
	}

	if ( Gold > 3000000000 )
	{
		Gold = 3000000000;
	}

	m_BuyMoney = (DWORD)Gold;
	m_SellMoney = (DWORD)Gold;
	m_SellMoney = m_SellMoney/3;

	if ( (m_Type < ITEMGET(14,0) || m_Type >  ITEMGET(14,8)) &&
		  m_Type != ITEMGET(13,20) &&
		  m_Type != ITEMGET(14,28) &&
		  m_Type != ITEMGET(14,29) &&
		  m_Type != ITEMGET(14,21) &&
		  m_Type != ITEMGET(13,15) &&
		  m_Type != ITEMGET(13,16) &&
		  m_Type != ITEMGET(13,17) &&
		  m_Type != ITEMGET(13,18) &&
		  m_Type != ITEMGET(13,11) &&
		  m_Type != ITEMGET(13,7)  &&
		  m_Type != ITEMGET(13,32) &&
		  m_Type != ITEMGET(13,33) &&
		  m_Type != ITEMGET(13,34) &&
		  m_Type != ITEMGET(13,35) &&
		  m_Type != ITEMGET(13,36) &&
		  m_Type != ITEMGET(13,37) &&
		  m_Type != ITEMGET(14,45) &&
		  m_Type != ITEMGET(14,46) &&
		  m_Type != ITEMGET(14,47) &&
		  m_Type != ITEMGET(14,48) &&
		  m_Type != ITEMGET(14,49) &&
		  m_Type != ITEMGET(14,50) &&
		  m_Type != ITEMGET(14,51) &&
		  m_Type != ITEMGET(14,100) &&
		  m_Type == ITEMGET(14,85) &&
		  m_Type == ITEMGET(14,86) &&
		  m_Type == ITEMGET(14,87) &&
		  m_Type == ITEMGET(14,90) &&
		  
		  (m_Type <  ITEMGET(13,109) || m_Type > ITEMGET(13,115))) //New Rings / NeckLaces
	{
		float lc15 = 1.0f-(m_Durability / m_BaseDurability);
		int lc16 = m_SellMoney*0.6*lc15;
		m_SellMoney -= lc16;
	}

	if ( m_SellMoney >= 1000 )
	{
		m_SellMoney = m_SellMoney/100*100;
	}
	else if ( m_SellMoney >= 100 )
	{
		m_SellMoney = m_SellMoney/10*10;
	}

	if ( m_BuyMoney >= 1000 )
	{
		m_BuyMoney = m_BuyMoney/100*100;
	}
	else if ( m_BuyMoney >= 100 )
	{
		m_BuyMoney = m_BuyMoney/10*10;
	}
}



void CItem::OldValue()
{
	if ( m_Type == -1 )
	{
		return;
	}

	int Gold = 0;

	if ( m_Type == ITEMGET(14,13) ) // Bless
	{
		Gold = 100000;
	}
	else if ( m_Type == ITEMGET(14,14) ) // Soul
	{
		Gold = 70000;
	}
	else if ( m_Type == ITEMGET(12,15) ) // Chaos
	{
		Gold = 40000;
	}
	else if ( m_Type == ITEMGET(14,22) ) // Creation
	{
		Gold = 450000;
	}
	else if ( m_Type == ITEMGET(14,16) ) // Life
	{
		Gold = 450000;
	}
	else
	{
		m_OldBuyMoney = m_BuyMoney;
		return;
	}

	m_OldBuyMoney = Gold;

	if ( m_OldBuyMoney >= 1000 )
	{
		m_OldBuyMoney = m_OldBuyMoney /100*100;
	}
	else if ( m_OldBuyMoney >= 100 )
	{
		m_OldBuyMoney = m_OldBuyMoney / 10 * 10;
	}
}

BOOL CItem::GetSize(int & w, int & h)
{
	w=ItemAttribute[m_Type].Width  ;
	h=ItemAttribute[m_Type].Height  ;

	return 1;
}


int CItem::ItemDamageMin()
{
	int damagemin;
	
	if (m_Type < 0)
	{
		return 0;
	}
	else
	{
		damagemin=m_DamageMinOrigin;
		PlusSpecial(&damagemin, 80);
		return damagemin;
	}
}


int CItem::ItemDefense()
{
	int Defense;
	
	if (m_Type < 0)
	{
		return 0;
	}
	else
	{
		if (m_IsValidItem == false)
		{
			return 0;
		}
		else if (m_bExpired)
		{
			return 0;
		}
		else
		{
			Defense=m_Defense;
			PlusSpecial(&Defense, 83);
			return Defense;
		}
	}
}

int CItem::ItemCurseDamage()
{
	int CurseDamage;

	if( m_Type < 0 )
	{
		return FALSE;
	}

	if( m_IsValidItem == FALSE )
	{
		return FALSE;
	}

	CurseDamage = m_CurseMagic;
return CurseDamage;
}






BOOL CItem::IsClass(char aClass, int ChangeUP,int ChangeUP2)
{
	if ((aClass < 0) || (aClass >= MAX_TYPE_PLAYER ))
	{
		LogAdd("클래스 에러 : %d (%s %d)", aClass, __FILE__, __LINE__); // 클래스 에러 : %d (%s %d)
		return 0;
	}
	
	int requireclass=m_RequireClass[aClass];
	if (requireclass == 0)
	{
		return 0;
	}

	if ( requireclass >= 3 )
	{
		if( requireclass != ChangeUP2 + 2 )
		{
			return false;
		}
	}
	else if (requireclass > 1)
	{
		if ( requireclass  != (ChangeUP+1) )
		{
			return false;
		}
	}
	return 1;
}

LPSTR CItem::GetName()
{
	return &ItemAttribute[m_Type].Name[0];
}



void CItem::PlusSpecialSetRing(BYTE * Value)
{
	if ( m_Type < 0 )
	{
		return;
	}

	if ( IsSetItem() == FALSE )
	{
		return;
	}

	if ( m_Durability == 0.0f)
	{
		return;
	}

	if ( m_IsValidItem == false )
	{
		return;
	}

	LPITEM_ATTRIBUTE p = &ItemAttribute[m_Type];

	if ( p->ResistanceType == -1 )
	{
		return;
	}

	BYTE iValue = 5;
	Value[p->ResistanceType] += iValue - (BYTE)( iValue * m_CurrentDurabilityState);

}



void CItem::PlusSpecial(int * Value, int Special)
{
	if ( m_Type < 0 )
	{
		return;
	}

	if ( m_Durability == 0.0f)
	{
		return;
	}

	if ( m_IsValidItem == false )
	{
		return;
	}

	for (int i =0;i<m_SpecialNum;i++)
	{
		if ( m_Special[i] == Special )
		{
			switch ( Special )
			{
				case 82:
					{
						int lc3 = m_Option3 * 5;
						*Value += lc3 - (int)( lc3 * m_CurrentDurabilityState );
					}
					break;

				case 84:
					* Value += 4;
					break;

				case 81:
				case 80:
				case 83:
				case 113:
					{
						int lc4 = m_Option3*4;
						*Value += lc4 - (int)( lc4 * m_CurrentDurabilityState );
					}
					break;

				case 100:
				case 101:
					*Value =  *Value + m_Level * 5 + 50;
					break;

				case 103:
					*Value += 50;
					break;

				case 105:
					*Value =  *Value + m_Level * 5 + 10;
					break;

				case 200:
					*Value = *Value + (m_Level+1) * 100;
					break;

				default:
					*Value = *Value + (m_Option3*4);
					break;
			}
		}
	}
}

void CItem::PlusSpecialPercent(int * Value, int Special, WORD Percent)
{
	if ( m_Type < 0 )
	{
		return;
	}

	for (int i=0;i<m_SpecialNum;i++)
	{
		if ( m_Special[i] == Special )
		{
			*Value += (*Value * Percent)/ 100;
		}
	}
}

void CItem::PlusSpecialPercentEx(int * Value, int SourceValue, int Special)
{
	if ( m_Type < 0 )
	{
		return;
	}

	if ( m_Durability == 0.0f )
	{
		return;
	}

	if ( m_IsValidItem == false  )
	{
		return;
	}

	for ( int i =0;i<m_SpecialNum;i++)
	{
		if ( m_Special[i] == Special )
		{
			int lc3 = SourceValue * m_Option3/100;
			*Value += lc3 - (int)(lc3 * m_CurrentDurabilityState );
		}
	}
}

#if( FIX_MAXSTAT == 1 )
void CItem::SetItemPlusSpecialStat(WORD * Value, int Special)
#else
void CItem::SetItemPlusSpecialStat(short * Value, int Special)
#endif
{
	if ( m_Type < 0 )
	{
		return;
	}

	if ( m_Durability == 0.0f )
	{
		return;
	}

	for ( int i =0;i<m_SpecialNum;i++)
	{
		if ( m_Special[i] == Special )
		{
			switch ( Special )
			{
				case 0xC4:
				case 0xC5:
				case 0xC6:
				case 0xC7:
					{
						int lc3 = m_SetAddStat;
						*Value += lc3 - (int)(lc3 * m_CurrentDurabilityState);
					}
					break;

				default:
					*Value += m_Option3*3;
			}
		}
	}
}

int CItem::GetAddStatType()
{
	LPITEM_ATTRIBUTE p;
	p = &ItemAttribute[m_Type];
	return p->SetAttr;
}

int CItem::GetWeaponType()
{
	if ( m_Type <= ITEMGET(5,0) )
	{
		return 1;
	}

	if ( m_Type <= ITEMGET(6,0) )
	{
		return 2;
	}
	return 0;
}



void CItem::SetPetItemInfo(int petlevel, int petexp)
{
	if ( m_Type == ITEMGET(13,4) )
	{
		m_PetItem_Level = petlevel;
		m_PetItem_Exp = petexp;

		if ( m_PetItem_Exp < gPetItemExp.m_DarkHorseExpTable[m_PetItem_Level] || m_PetItem_Exp >= gPetItemExp.m_DarkHorseExpTable[1+m_PetItem_Level] )
		{
			m_PetItem_Exp = gPetItemExp.m_DarkHorseExpTable[m_PetItem_Level];
		}

		Convert(m_Type, m_Option1, m_Option2,
			m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
		PetValue();
		m_IsLoadPetItemInfo = 1;
	}
	else if ( m_Type == ITEMGET(13,5) )
	{
		m_PetItem_Level = petlevel;
		m_PetItem_Exp = petexp;

		if ( m_PetItem_Exp < gPetItemExp.m_DarkSpiritExpTable[m_PetItem_Level] || m_PetItem_Exp >= gPetItemExp.m_DarkSpiritExpTable[1+m_PetItem_Level] )
		{
			m_PetItem_Exp = gPetItemExp.m_DarkSpiritExpTable[m_PetItem_Level];
		}

		Convert(m_Type, m_Option1, m_Option2,
			m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);

		PetValue();
		m_IsLoadPetItemInfo = 1;
	}
}




BOOL CItem::AddPetItemExp(int petexp)
{
	if ( m_Durability <= 0.0f )
	{
		return FALSE;
	}

	if ( m_IsLoadPetItemInfo == 0 )
	{
		return FALSE;
	}

	if ( m_PetItem_Level < 1 )
	{
		m_PetItem_Level = 1;
		m_PetItem_Exp = 0;
		Convert(m_Type, m_Option1, m_Option2,
			m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
		PetValue();

		return FALSE;
	}

	if ( m_Type == ITEMGET(13,4) )
	{
		if ( m_PetItem_Level >= MAX_PET_LEVEL )
		{
			return FALSE;
		}

		BOOL ret = FALSE;
		int maxexp = gPetItemExp.m_DarkHorseExpTable[1+m_PetItem_Level];
		m_PetItem_Exp += petexp;

		while ( m_PetItem_Exp >= maxexp )
		{
			m_PetItem_Level++;

			if(m_PetItem_Level >= MAX_PET_LEVEL)
				break;

			maxexp = gPetItemExp.m_DarkHorseExpTable[1+m_PetItem_Level];
			ret = TRUE;
		}

		if ( ret != FALSE )
		{
		Convert(m_Type, m_Option1, m_Option2,
			m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx,0,-1, CURRENT_DB_VERSION);
		PetValue();

		return TRUE;
		}
	}
	else if ( m_Type == ITEMGET(13,5) )
	{
		if ( m_PetItem_Level >= MAX_PET_LEVEL )
		{
			return FALSE;
		}

		BOOL ret = FALSE;
		int maxexp = gPetItemExp.m_DarkSpiritExpTable[1+m_PetItem_Level];
		m_PetItem_Exp += petexp;

		while ( m_PetItem_Exp >= maxexp )
		{
			m_PetItem_Level++;

			if(m_PetItem_Level >= MAX_PET_LEVEL)
				break;

			maxexp = gPetItemExp.m_DarkSpiritExpTable[1+m_PetItem_Level];
			ret = TRUE;
		}

		if ( ret != FALSE )
		{
		Convert(m_Type, m_Option1, m_Option2,
			m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
		PetValue();

		return TRUE;
		}
	}

	return FALSE;
}


BOOL CItem::DecPetItemExp(int percent)
{
	if ( m_IsLoadPetItemInfo == 0 )
	{
		return FALSE;
	}

	if ( m_PetItem_Level < 1 )
	{
		m_PetItem_Level = 1;
		m_PetItem_Exp = 0;
		Convert(m_Type, m_Option1, m_Option2,
			m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
		PetValue();

		return FALSE;
	}

	if ( m_Type == ITEMGET(13,4) )
	{
		int exp = gPetItemExp.m_DarkHorseExpTable[1+m_PetItem_Level] - gPetItemExp.m_DarkHorseExpTable[m_PetItem_Level];
		exp = exp * percent/100;
		m_PetItem_Exp -= exp;

		if ( m_PetItem_Exp < 0 )
		{
			m_PetItem_Level = 1;
			m_PetItem_Exp = 0;
			Convert(m_Type, m_Option1, m_Option2,
				m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
			PetValue();
		}
		else if ( m_PetItem_Exp < gPetItemExp.m_DarkHorseExpTable[m_PetItem_Level] )
		{
			m_PetItem_Level--;
			Convert(m_Type, m_Option1, m_Option2,
				m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
			PetValue();
			
			return TRUE;
		}
	}
	else if ( m_Type == ITEMGET(13,5) )
	{
		int exp = gPetItemExp.m_DarkSpiritExpTable[1+m_PetItem_Level] - gPetItemExp.m_DarkSpiritExpTable[m_PetItem_Level];
		exp = exp * percent/100;
		m_PetItem_Exp -= exp;

		if ( m_PetItem_Exp < 0 )
		{
			m_PetItem_Level = 1;
			m_PetItem_Exp = 0;
			Convert(m_Type, m_Option1, m_Option2,
				m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
			PetValue();
		}
		else if ( m_PetItem_Exp < gPetItemExp.m_DarkSpiritExpTable[m_PetItem_Level] )
		{
			m_PetItem_Level--;
			Convert(m_Type, m_Option1, m_Option2,
				m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
			PetValue();
			
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CItem::PetItemLevelDown(int exp)
{
	if ( m_IsLoadPetItemInfo == FALSE )
	{
		return FALSE;
	}

	if ( m_PetItem_Level < 2 )
	{
		return FALSE;
	}

	m_PetItem_Exp -= exp;
	m_PetItem_Level--;

	if ( m_Type == ITEMGET(13,4) )
	{
		int petlevel = m_PetItem_Level;
		int petexp = gPetItemExp.m_DarkHorseExpTable[petlevel];

		while ( m_PetItem_Exp <= petexp )
		{
			if ( petlevel <= 1 )
			{
				break;
			}

			petlevel--;
			petexp = gPetItemExp.m_DarkHorseExpTable[petlevel];
		}

		m_PetItem_Level = petlevel;
	}
	else if ( m_Type == ITEMGET(13,5) )
	{
		int petlevel = m_PetItem_Level;
		int petexp = gPetItemExp.m_DarkSpiritExpTable[petlevel];

		while ( m_PetItem_Exp <= petexp )
		{
			if ( petlevel <= 1 )
			{
				break;
			}

			petlevel--;
			petexp = gPetItemExp.m_DarkSpiritExpTable[petlevel];
		}

		m_PetItem_Level = petlevel;
	}

	Convert(m_Type, m_Option1, m_Option2,
		m_Option3, m_NewOption, m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
	PetValue();

	return TRUE;
}




void CItem::PetValue()
{
	if ( m_Type == ITEMGET(13,4) )
	{
		m_SellMoney = m_PetItem_Level * 2000000;
	}
	else if ( m_Type == ITEMGET(13,5) )
	{
		m_SellMoney = m_PetItem_Level * 1000000;
	}	

	m_BuyMoney = m_SellMoney;
	m_SellMoney /= 3;

	if ( m_SellMoney >= 1000 )
	{
		m_SellMoney = m_SellMoney / 100 * 100;
	}
	else if ( m_SellMoney >= 100 )
	{
		m_SellMoney = m_SellMoney / 10 * 10;
	}

	if ( m_BuyMoney >= 1000 )
	{
		m_BuyMoney = m_BuyMoney / 100 * 100;
	}
	else if ( m_BuyMoney >= 100 )
	{
		m_BuyMoney = m_BuyMoney / 10 * 10;
	}
}



BOOL CItem::IsExtItem()
{
	if ( m_Type == ITEMGET(13,37) )
	{
		return FALSE;
	}

	if ( (m_NewOption & 0x7F)!= 0 )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CItem::IsExtLifeAdd() {
	if ((m_NewOption & 0x20) == 32) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 0);
		if (OptionValue == (int)-1) {
			return 4; // default value
		} else {
			return OptionValue;
		}
#else
		return 4;
#endif
	}
	return 0;
}

BOOL CItem::IsExtManaAdd() {
	if ((m_NewOption & 0x10) == 0x10) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 1);
		if (OptionValue == (int)-1) {
			return 4; // default value
		} else {
			return OptionValue;
		}
#else
		return 4;
#endif
	}
	return 0;
}

BOOL CItem::IsExtDamageMinus() {
	if ((m_NewOption & 8) == 8) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 2);
		if (OptionValue == (int)-1) {
			return 4; // default value
		} else {
			return OptionValue;
		}
#else
		return 4;
#endif
	}
	return 0;
}

BOOL CItem::IsExtDamageReflect() {
	if ((m_NewOption & 4) == 4) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 3);
		if (OptionValue == (int)-1) {
			return 5; // default value
		} else {
			return OptionValue;
		}
#else
		return 5;
#endif
	}
	return 0;
}

BOOL CItem::IsExtDefenseSuccessfull() {
	if ((m_NewOption & 2) == 2) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 4);
		if (OptionValue == (int)-1) {
			return 10; // default value
		} else {
			return OptionValue;
		}
#else
		return 10;
#endif
	}
	return 0;
}

BOOL CItem::IsExtMonsterMoney() {
	if ((m_NewOption & 1) == 1) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 5);
		if (OptionValue == (int)-1) {
			return 30; // default value
		} else {
			return OptionValue;
		}
#else
		return 30;
#endif
	}
	return 0;
}

BOOL CItem::IsExtExcellentDamage() {
	if ((m_Type >= ITEMGET(6, 0) ) && (m_Type < ITEMGET(7, 0))) {
		return 0;
	}

	if (( m_NewOption & 0x20 ) == 0x20) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 0);
		if (OptionValue == (int)-1) {
			return 10; // default value
		} else {
			return OptionValue;
		}
#else
		return 10;
#endif
	}
	return 0;
}

BOOL CItem::IsExtAttackRate() {
	if ((m_NewOption & 0x10) == 0x10) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 1);
		if (OptionValue == (int)-1) {
			return 20; // default value
		} else {
			return OptionValue;
		}
#else
		return 20;
#endif
	}
	return 0;
}

BOOL CItem::IsExtAttackRate2() {
	if ((m_NewOption & 8) == 8) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 2);
		if (OptionValue == (int)-1) {
			return 2; // default value
		} else {
			return OptionValue;
		}
#else
		return 2;
#endif
	}
	return 0;
}

BOOL CItem::IsExtAttackSpeed() {
	if ((m_NewOption & 4) == 4) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 3);
		if (OptionValue == (int)-1) {
			return 7; // default value
		} else {
			return OptionValue;
		}
#else
		return 7;
#endif
	}
	return 0;
}

BOOL CItem::IsExtMonsterDieLife() {
	if ((m_NewOption & 2) == 2) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 4);
		if (OptionValue == (int)-1) {
			return 8; // default value
		} else {
			return OptionValue;
		}
#else
		return 8;
#endif
	}
	return 0;
}

BOOL CItem::IsExtMonsterDieMana() {
	if (m_Type >= ITEMGET(6, 0) && m_Type < ITEMGET(7, 0)) {
		return 0;
	}

	if ((m_NewOption & 1) == 1) {
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
		int OptionValue = g_ItemExcellentOption.getOptionValue(m_Type, 5);
		if (OptionValue == (int)-1) {
			return 8; // default value
		} else {
			return OptionValue;
		}
#else
		return 8;
#endif
	}
	return 0;
}

BOOL CItem::IsWingOpGetOnePercentDamage()
{
	if ( m_Durability  < 1.0f )
	{
		return 0;
	}
	
	if( m_Type >= ITEMGET(12,3) && m_Type <= ITEMGET(12,6) || m_Type == ITEMGET(13,30) || m_Type == ITEMGET(12,42) || m_Type == ITEMGET(12,49) )
	{
		if ( (m_NewOption & 4)!= 0 )
		{
			return 3;
		}
	}
	return 0;
}

BOOL CItem::IsWingOpGetManaToMoster()
{
	if ( m_Durability < 1.0f )
	{
		return 0;
	}

	if( m_Type >= ITEMGET(12,3) && m_Type <= ITEMGET(12,6) || m_Type == ITEMGET(13,30) || m_Type == ITEMGET(12,42) )
	{
		if ( (m_NewOption & 16 ) == 1 )
		{
			return 10;
		}
	}
	return 0;
}

BOOL CItem::IsDinorantReduceAttackDamaege()
{
	if ( m_Durability  < 1.0f )
	{
		return 0;
	}

	if ( (m_NewOption & 1 ) == 1 )
	{
		return 5;
	}
	return 0;
}


int CItem::IsFenrirIncLastAttackDamage()
{
	if ( m_Durability < 1.0f )
	{
		return 0;
	}

	if ( (m_NewOption&1) == 1 )
	{
		return 10;
	}

	return 0;
}

int CItem::IsFenrirDecLastAttackDamage()
{
	if ( m_Durability < 1.0f )
	{
		return 0;
	}

	if ( (m_NewOption&2) == 2 )
	{
		return 10;
	}

	return 0;
}

int CItem::IsFenrirSpecial()
{
	if ( m_Durability < 1.0f )
	{
		return 0;
	}

	if ( (m_NewOption&4) == 4 )
	{
		return 1;
	}

	return 0;
}

int CItem::Is3rdWingOpGetOnePercentDamage()
{
	if( (m_Type >= ITEMGET(12,36) && m_Type <= ITEMGET(12,40)) || m_Type == ITEMGET(12,43) )
	{
		if( m_Durability < 1.0f )
		{
			return 0;
		}

		if( (m_NewOption & 1) == 1 )
		{
			return 5;
		}
	}
return 0;
}

int CItem::Is25WingOpGetOnePercentDamage()
{
	if( m_Type >= ITEMGET(12,262) && m_Type <= ITEMGET(12,265) )
	{
		if( m_Durability < 1.0f )
		{
			return 0;
		}

		if( (m_NewOption & 1) == 1 )
		{
			return 3;
		}
	}
return 0;
}

int CItem::SimpleDurabilityDown(int iDecValue)
{
	m_Durability -= 1.0f ;

	if ( m_Durability < 0 )
	{
		m_Durability = 0.0f;
		return 0;
	}

	if ( CheckDurabilityState() != 0 )
	{
		return 2;
	}

	return m_Durability;
}


int CItem::DurabilityDown(int dur, int iIndex)
{
	if ( m_Durability <= 0 )
	{
		return 0;
	}

	m_DurabilitySmall += dur;
	int DurCheck = 564;

	if ( gObj[iIndex].m_MPSkillOpt.MpsDownDur2 > 0.0f )
	{
		DurCheck += DurCheck*gObj[iIndex].m_MPSkillOpt.MpsDownDur2/100.0f;
	}
	
	if ( m_DurabilitySmall > DurCheck )
	{
		m_DurabilitySmall = 0;
		m_Durability -= 1.0f;

		if ( this ->m_Durability < 0.0f )
		{
			m_Durability = 0;
		}

		if ( CheckDurabilityState() != FALSE )
		{
			return 2;
		}
		return 1;
	}
	
	return m_Durability;
}




int CItem::DurabilityDown2(int dur, int iIndex)
{
	if ( m_Durability <= 0 )
	{
		return 0;
	}

	m_DurabilitySmall += dur;
	int DurCheck = 1000;

	if( gObj[iIndex].m_MPSkillOpt.MpsDownDur1 > 0.0f )
	{
		DurCheck += DurCheck*gObj[iIndex].m_MPSkillOpt.MpsDownDur1/100.0f;
	}
	
	if ( m_DurabilitySmall > DurCheck )
	{
		m_DurabilitySmall = 0;
		m_Durability -= 1.0f;

		if ( m_Durability < 0.0f )
		{
			m_Durability = 0;
		}

		if ( CheckDurabilityState() != FALSE )
		{
			return 2;
		}
		return 1;
	}
	
	return 0;
}




int CItem::NormalWeaponDurabilityDown(int defense, int aIndex)
{
	if ( m_Durability <= 0 )
	{
		return 0;
	}

	int damagemin = m_DamageMinOrigin;
	int plusdamage = 0;
	PlusSpecial(&plusdamage, 80);

	if ( damagemin == 0 )
	{
		LogAdd("damagemin = 0!");
		return 0;
	}

	int div = damagemin + damagemin/2 + plusdamage;

	if ( div == 0 )
	{
		return 0;
	}

	int DecreaseDur = (defense*2)/div;

	DecreaseDur -= (DecreaseDur * m_BonusDurability)/100;

	m_DurabilitySmall += DecreaseDur;

	int DurCheck = 564;

	if ( gObj[aIndex].m_MPSkillOpt.MpsDownDur1 > 0.0f )
	{
		DurCheck += DurCheck*gObj[aIndex].m_MPSkillOpt.MpsDownDur1/100.0f;
	}

	if ( m_DurabilitySmall > DurCheck )
	{
		m_DurabilitySmall = 0;
		m_Durability -= 1.0f;

		if ( m_Durability < 0.0f )
		{
			m_Durability = 0;
		}

		if ( CheckDurabilityState() != 0 )
		{
			return 2;
		}
		
		return 1;
		
	}
	return 0;
}



int CItem::BowWeaponDurabilityDown(int defense, int aIndex)
{
	if ( m_Durability <= 0 )
	{
		return 0;
	}

	int damagemin = m_DamageMinOrigin;
	int plusdamage = 0;
	PlusSpecial(&plusdamage, 80);
	
	int div = damagemin + damagemin/2 + plusdamage;

	if ( div == 0 )
	{
		return 0;
	}

	int DecreaseDur = (defense*2)/div;
	DecreaseDur -= DecreaseDur * m_BonusDurability / 100;
	m_DurabilitySmall += DecreaseDur;

	int DurCheck = 780;

	if ( gObj[aIndex].m_MPSkillOpt.MpsDownDur1 > 0.0f )
	{
		DurCheck += DurCheck*gObj[aIndex].m_MPSkillOpt.MpsDownDur1/100.0f;
	}

	if ( m_DurabilitySmall > DurCheck )
	{
		m_DurabilitySmall = 0;
		m_Durability -= 1.0f;

		if ( m_Durability < 0.0f )
		{
			m_Durability = 0;
		}
		
		if ( CheckDurabilityState() != 0 )
		{
			return 2;
		}
		
		return 1;
		
	}
	return 0;
}

int CItem::StaffWeaponDurabilityDown(int defence, int aIndex)
{
	if ( m_Durability <= 0 )
	{
		return 0;
	}

	int magic = m_Magic/2 + m_Level*2;
	int plusmagic = 0;
	PlusSpecial(&plusmagic, 81);
	int div = magic + magic/3 + plusmagic;

	if ( div == 0 )
	{
		return 0;
	}

	int DecreaseDur = defence/div;
	DecreaseDur -= DecreaseDur * m_BonusDurability / 100;
	m_DurabilitySmall += DecreaseDur;

	int DurCheck = 1050;

	if ( gObj[aIndex].m_MPSkillOpt.MpsDownDur1 > 0.0f )
	{
		DurCheck += DurCheck*gObj[aIndex].m_MPSkillOpt.MpsDownDur1/100.0f;
	}

	if ( m_DurabilitySmall > DurCheck )
	{
		m_DurabilitySmall = 0;
		m_Durability -= 1.0f;

		if ( m_Durability < 0.0f )
		{
			m_Durability = 0;
		}
		
		if ( CheckDurabilityState() != 0 )
		{
			return 2;
		}
		return 1;
		
	}
	return 0;
}




int CItem::ArmorDurabilityDown(int damagemin, int aIndex)
{
	if ( m_Durability <= 0 )
	{
		return 0;
	}

	if ( m_Type == -1 )
	{
		return 0;
	}

	int def = m_DefenseOrigin;
	int plusdef = 0;
	int DecreaseDur;

	if ( def == 0 )
	{
		LogAdd("def이 0이다");
		return 0;
	}

	if ( m_Type >= ITEMGET(6,0) && m_Type < ITEMGET(7,0) )
	{
		PlusSpecial(&plusdef, 82);
		DecreaseDur = damagemin/(def * 5+ plusdef);
	}
	else if ( m_RequireClass[0] != 0 )	// Dark Wize
	{
		PlusSpecial(&plusdef, 83);
		DecreaseDur = damagemin/(def * 3 + plusdef);
	}
	else if ( m_RequireClass[1] != 0 )	// Dark Knight
	{
		PlusSpecial(&plusdef, 83);
		DecreaseDur = damagemin/(def * 3 + plusdef);
	}
	else if ( m_RequireClass[2] != 0 )	// Elf
	{
		PlusSpecial(&plusdef, 83);
		DecreaseDur = damagemin/(def * 2 + plusdef);
	}
	else if ( m_RequireClass[3] != 0 )	// MAgic Gladiator ( aka Magunsa )
	{
		PlusSpecial(&plusdef, 83);
		DecreaseDur = damagemin/(def * 7 + plusdef);
	}
	else if ( m_RequireClass[4] != 0 )	// Dark Lord
	{
		PlusSpecial(&plusdef, 83);
		DecreaseDur = damagemin/(def * 6 + plusdef);
	}
	else if ( m_RequireClass[5] != 0 )
	{
		PlusSpecial(&plusdef, 83);
		DecreaseDur = damagemin/(def * 3 + plusdef);
	}
	else if ( m_RequireClass[6] != 0 )
	{
		PlusSpecial(&plusdef, 83);
		DecreaseDur = damagemin/(def * 3 + plusdef);
	}

	m_DurabilitySmall += DecreaseDur;

	int DurCheck = 69;

	if ( gObj[aIndex].m_MPSkillOpt.MpsDownDur1 > 0.0f )
	{
		DurCheck += DurCheck*gObj[aIndex].m_MPSkillOpt.MpsDownDur1/100.0f;
	}

	if ( m_DurabilitySmall > DurCheck )
	{
		m_DurabilitySmall = 0;
		m_Durability -= 1.0f;

		if ( m_Durability < 0.0f )
		{
			m_Durability = 0;
		}
		
		if ( CheckDurabilityState() != 0 )
		{
			return 2;
		}
		
		return 1;
		
	}
	return 0;
}





BOOL CItem::CheckDurabilityState()
{
	if ( m_Durability == 0.0f )
	{
		if ( m_CurrentDurabilityState != 1.0f )
		{
			m_CurrentDurabilityState = 1.0f;
			Convert(m_Type, m_Option1, m_Option2, m_Option3, m_NewOption,
				m_SetOption, m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
			return true;
		}
	}
	else if ( m_Durability < m_DurabilityState[2] )
	{
		if ( m_CurrentDurabilityState != 0.5f )
		{
			m_CurrentDurabilityState = 0.5f;
			Convert(m_Type, m_Option1, m_Option2, m_Option3, m_NewOption,
				m_SetOption,m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
			return true;
		}
	}
	else if ( m_Durability < m_DurabilityState[1] )
	{
		if ( m_CurrentDurabilityState != 0.3f )
		{
			m_CurrentDurabilityState = 0.3f;
			Convert(m_Type, m_Option1, m_Option2, m_Option3, m_NewOption,
				m_SetOption,m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
			return true;
		}
	}
	else if ( m_Durability < m_DurabilityState[0] )
	{
		if ( m_CurrentDurabilityState != 0.2f )
		{
			m_CurrentDurabilityState = 0.2f;
			Convert(m_Type, m_Option1, m_Option2, m_Option3, m_NewOption,
				m_SetOption,m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
			return true;
		}
	}
	else if ( m_CurrentDurabilityState != 0.0f )
	{
		m_CurrentDurabilityState = 0;
		Convert(m_Type, m_Option1, m_Option2, m_Option3, m_NewOption,
			m_SetOption,m_ItemOptionEx, 0,-1,CURRENT_DB_VERSION);
		return true;
	}

	return false;
}

void CItem::InitSocketItem(BYTE btCount)
{
	if( g_SocketItem.IsSocketItem(this) == FALSE )
	{
		return;
	}

	for(int n = 0; n < btCount; n++)
	{
		m_SocketOption[n] = 0xFE;
	}

	m_SocketBonus = 0xFF;
}

#if (ENABLETEST_MUUN == 1)
bool CItem::IsMuunItmePeriodExpire() {
	if (m_SocketBonus & 0x10) {
		return true;
	}
    return false;
}

void CItem::SetMuunItmePeriodExpire() {
	m_SocketBonus |= 0x10;
}

int CItem::GetMuunItemRank() {
	return m_SocketBonus & 0xF;
}
#endif
//-----------------------------------------------------------
// End of CItem class
//-----------------------------------------------------------


void BufferItemtoConvert3(unsigned char* buf, int& type, BYTE& level, BYTE& op1, BYTE& op2, BYTE& op3, BYTE& dur)
{
	type = buf[0] + ((buf[3]&0x80)*2)+((buf[5]&0xF0)<<5);
	level = (buf[1]>>3) & 15;
	op1 = (buf[1]>>7) & 1;
	op2 = (buf[1]>>2) & 1;
	op3 = (buf[1] & 3 );
	dur = buf[2];

	op3 |= (buf[3] & 0x40) >> 4;
}




void ItemByteConvert7(unsigned char * buf, CItem * const item, int maxitem)
{
	int n = 0;
	WORD hiWord;
	WORD loWord;

	for ( int index = 0; index < maxitem ; index++ )
	{
		buf[n] = item[index].m_Type;
		n++;
		buf[n] = 0;
		buf[n] |= item[index].m_Level * 8;
		buf[n] |= item[index].m_Option1 * 128;
		buf[n] |= item[index].m_Option2 * 4;
		buf[n] |= item[index].m_Option3;
		n++;
		buf[n] = item[index].m_Durability;
		n++;
		hiWord = item[index].m_Number >> 16;
		loWord = item[index].m_Number & 0xFFFF;
		buf[n] = SET_NUMBERH(hiWord);
		n++;
		buf[n] = SET_NUMBERL(hiWord);
		n++;
		buf[n] = SET_NUMBERH(loWord);
		n++;
		buf[n] = SET_NUMBERL(loWord);
		n++;
	}
}




void ItemByteConvert10(unsigned char * buf, CItem * const item, int maxitem)
{
	int n = 0;
	WORD hiWord;
	WORD loWord;
	int index;

	for (index = 0; index < maxitem ; index++ )
	{
		if ( item[index].m_Type == ITEMGET(13,19) )
		{
			if ( (item[index].m_Level < 0)? FALSE : ( item[index].m_Level > 2)? FALSE : TRUE  )
			{
				buf[n] = -1;
				buf[n+1] = -1;
				buf[n+2] = -1;
				buf[n+3] = -1;
				buf[n+4] = -1;
				buf[n+5] = -1;
				buf[n+6] = -1;
				buf[n+7] = -1;
				buf[n+8] = -1;
				buf[n+9] = -1;
				n += 16;
				continue;
			}
		}
		
		if ( item[index].m_Type <  0 )
		{
			buf[n] = -1;
			buf[n+1] = -1;
			buf[n+2] = -1;
			buf[n+3] = -1;
			buf[n+4] = -1;
			buf[n+5] = -1;
			buf[n+6] = -1;
			buf[n+7] = -1;
			buf[n+8] = -1;
			buf[n+9] = -1;
			n += 16;
		}
		else
		{
			buf[n] = (BYTE)item[index].m_Type % 256 ;
			n++;
			buf[n] = 0;
			buf[n] |= item[index].m_Level * 8;
			buf[n] |= item[index].m_Option1 * 128;
			buf[n] |= item[index].m_Option2 * 4;
			buf[n] |= item[index].m_Option3 & 3;
			n++;
			buf[n] = item[index].m_Durability;
			n++;
			hiWord = item[index].m_Number >> 16;
			loWord = item[index].m_Number  & 0xFFFF;
			buf[n] = SET_NUMBERH(hiWord);
			n++;
			buf[n] = SET_NUMBERL(hiWord);
			n++;
			buf[n] = SET_NUMBERH(loWord);
			n++;
			buf[n] = SET_NUMBERL(loWord);
			n++;
			buf[n] = 0;

			if ( item[index].m_Type > 255 )
			{
				buf[n] = 0x80;	// Second Type of Item
			}

			if ( item[index].m_Option3 > 3 )
			{
				buf[n] |= 0x40;	// Duration ???
			}

			buf[n] |= item[index].m_NewOption;
			n++;
			buf[n] = item[index].m_SetOption;
			n++;
			n++;
		}
	}
}





void ItemByteConvert(unsigned char* buf, CItem item)
{
	int n=0;

	memset(buf, 0, 12);
	buf[n] = item.m_Type & 0xFF;
	n++;
	buf[n] = 0;
	buf[n] |= item.m_Level * 8;
	buf[n] |= item.m_Option1 * 128;
	buf[n] |= item.m_Option2 * 4;
	buf[n] |= item.m_Option3 & 3;
	n++;
	buf[n] = item.m_Durability;
	n++;
	buf[n] = 0;
	buf[n] |= ((item.m_Type & 0x100) >> 1);

	if ( item.m_Option3 > 3 )
	{
		buf[n] |= 0x40; // item +16 option
	}

	buf[n] |= item.m_NewOption;
	n++;
	buf[n] = item.m_SetOption;


	if( item.m_bLOCKED == TRUE )
	{
		buf[n] |= 0xC0;
	}


	n++;
	buf[n] |= (item.m_Type & 0x1E00 ) >> 5;

	BYTE btItemEffectFor380 = 0;
	btItemEffectFor380 = (item.m_ItemOptionEx & 0x80 ) >> 4;
	buf[n] |= btItemEffectFor380;

	if( item.m_bPeriodItem == 1)
	{
		buf[n] |= item.m_bPeriodItem << 1;
		buf[n] |= item.m_bExpired << 2;
	}

	n++;
	buf[n] = item.m_JewelOfHarmonyOption;

	if (g_SocketItem.IsSocketItem(&item) || g_ElementalSystem.IsElementalItem(&item)) {
		buf[n] = item.m_SocketBonus;
	}

#if (ENABLETEST_MUUN == 1)
	if (g_CMuunSystem.IsMuunItem(&item)) {
		buf[n] = item.m_SocketBonus;
	}
	if (g_CMuunSystem.IsStoneofEvolution(&item)) { // T_T
		buf[n] = 0;
	}
#endif

	n++;
	buf[n] = item.m_SocketOption[0];
	n++;
	buf[n] = item.m_SocketOption[1];
	n++;
	buf[n] = item.m_SocketOption[2];
	n++;
	buf[n] = item.m_SocketOption[3];
	n++;
	buf[n] = item.m_SocketOption[4];
}





void ItemByteConvert(unsigned char* buf, int type, BYTE Option1, BYTE Option2, BYTE Option3, BYTE level, BYTE dur, BYTE Noption, BYTE SetOption, BYTE JewelOfHarmonyOption, BYTE ItemEffectEx,LPBYTE SocketOption,BYTE SocketBonus)
{
	memset(buf, 0, 12);
	int n = 0;

	buf[n] = type & 0xFF;
	n++;
	buf[n] = 0;
	buf[n] |= level * 8;
	buf[n] |= Option1 * 128;
	buf[n] |= Option2 * 4;
	buf[n] |= Option3 & 3;
	n++;
	buf[n] = dur;
	n++;
	buf[n] = 0;
	buf[n] |= ((type& 0x100) >> 1);

	if ( Option3 > 3 )
	{
		buf[n] |= 0x40;
	}

	buf[n] |= Noption;
	n++;
	buf[n] = SetOption;
	n++;
	buf[n] |= (type & 0x1E00 ) >> 5;

	BYTE btItemEffectFor380 = 0;
	btItemEffectFor380 = (ItemEffectEx  & 0x80 ) >> 4;
	buf[n] |= btItemEffectFor380;

	n++;
	buf[n] = JewelOfHarmonyOption;
	
	if( SocketOption != NULL )
	{
		buf[n] = SocketBonus;
		n++;
		buf[n] = SocketOption[0];
		n++;
		buf[n] = SocketOption[1];
		n++;
		buf[n] = SocketOption[2];
		n++;
		buf[n] = SocketOption[3];
		n++;
		buf[n] = SocketOption[4];
	}
	else
	{
		n++;
		buf[n] = 0xFF;
		n++;
		buf[n] = 0xFF;
		n++;
		buf[n] = 0xFF;
		n++;
		buf[n] = 0xFF;
		n++;
		buf[n] = 0xFF;
	}
}
// ///////////////////////////////////////////
// START REVIEW HERE

void ItemByteConvert16(LPBYTE buf, CItem * const item , int maxitem)
{
	int n=0;
	WORD hiWord, loWord;

	for (int index=0; index < maxitem ;index++)
	{
		if ( item[index].m_Type == ITEMGET(13,19) )
		{
			if ( CHECK_LIMIT(item[index].m_Level, 3) != FALSE )
			{
				buf[n+0]=-1;
				buf[n+1]=-1;
				buf[n+2]=-1;
				buf[n+3]=-1;
				buf[n+4]=-1;
				buf[n+5]=-1;
				buf[n+6]=-1;
				buf[n+7]=-1;
				buf[n+8]=-1;
				buf[n+9]=-1;
				buf[n+10]=-1;
				buf[n+11]=-1;
				buf[n+12]=-1;
				buf[n+13]=-1;
				buf[n+14]=-1;
				buf[n+15]=-1;
				n+=16;

				continue;
			}
		}

		if ( item[index].m_Type == ITEMGET(14,64) )
		{
			buf[n+0]=-1;
			buf[n+1]=-1;
			buf[n+2]=-1;
			buf[n+3]=-1;
			buf[n+4]=-1;
			buf[n+5]=-1;
			buf[n+6]=-1;
			buf[n+7]=-1;
			buf[n+8]=-1;
			buf[n+9]=-1;
			buf[n+10]=-1;
			buf[n+11]=-1;
			buf[n+12]=-1;
			buf[n+13]=-1;
			buf[n+14]=-1;
			buf[n+15]=-1;
			n+=16;

			continue;
		}

		if ( item[index].m_Type < ITEMGET(0,0) )
		{
			buf[n+0]=-1;
			buf[n+1]=-1;
			buf[n+2]=-1;
			buf[n+3]=-1;
			buf[n+4]=-1;
			buf[n+5]=-1;
			buf[n+6]=-1;
			buf[n+7]=-1;
			buf[n+8]=-1;
			buf[n+9]=-1;
			buf[n+10]=-1;
			buf[n+11]=-1;
			buf[n+12]=-1;
			buf[n+13]=-1;
			buf[n+14]=-1;
			buf[n+15]=-1;
			n+=16;
		}
		else
		{
			memset(buf+n, 0, 16);

			buf[n] = (BYTE)item[index].m_Type % 256;
			n++;
			buf[n]=0;
			buf[n] |= item[index].m_Level * 8;
			buf[n] |= item[index].m_Option1 * 128;
			buf[n] |= item[index].m_Option2 * 4;
			buf[n] |= item[index].m_Option3 & 3;
			n++;
			buf[n] = item[index].m_Durability;
			n++;
			hiWord = item[index].m_Number >> 16;
			loWord = item[index].m_Number  & 0xFFFF;
			buf[n] = SET_NUMBERH(hiWord);
			n++;
			buf[n] = SET_NUMBERL(hiWord);
			n++;
			buf[n] = SET_NUMBERH(loWord);
			n++;
			buf[n] = SET_NUMBERL(loWord);
			n++;
			buf[n] = 0;
			BYTE btItemType=0;
			btItemType |= (item[index].m_Type & 0x1E00 ) >> 5;
			buf[n] |= ((item[index].m_Type & 0x100) >> 1);

			if ( item[index].m_Option3 > 3 )
			{
				buf[n] |= 0x40; // item +16 option
			}

			buf[n] |= item[index].m_NewOption;
			n++;
			buf[n] = item[index].m_SetOption;


			if( item[index].m_bLOCKED == TRUE )
			{
				buf[n] |= 0xC0;
			}

			n++;
			buf[n] = 0;
			buf[n] |= btItemType;
			BYTE btItemEffectFor380 = 0;
			btItemEffectFor380 = (item[index].m_ItemOptionEx & 0x80 ) >> 4;
			buf[n] |= btItemEffectFor380;

			buf[n] |= item[index].m_bPeriodItem<<1;
			buf[n] |= item[index].m_bExpired<<2;

			n++;

			if( item[index].m_JewelOfHarmonyOption == (BYTE)-1 )
			{
				item[index].m_JewelOfHarmonyOption = 0;
			}

			buf[n] = item[index].m_JewelOfHarmonyOption;

			if (g_SocketItem.IsSocketItem(&item[index]) || g_ElementalSystem.IsElementalItem(&item[index])) {
				buf[n] = item[index].m_SocketBonus;
			}

#if (ENABLETEST_MUUN == 1)
			if (g_CMuunSystem.IsMuunItem(&item[index])) {
				buf[n] = item[index].m_SocketBonus;
			}
#endif

			n++;
			buf[n] = item[index].m_SocketOption[0];
			n++;
			buf[n] = item[index].m_SocketOption[1];
			n++;
			buf[n] = item[index].m_SocketOption[2];
			n++;
			buf[n] = item[index].m_SocketOption[3];
			n++;
			buf[n] = item[index].m_SocketOption[4];
			n++;
		}
	}
}
	
// ItemAttribute Begin:9439368 END 9446B68 Array[512]



int ItemGetNumberMake(int type, int index)
{
	int make;

	make = type*MAX_SUBTYPE_ITEMS + index;

	if (ItemAttribute[make].Width < 1 ||  ItemAttribute[make].Height < 1)
	{
		return -1;
	}
	return make;
}

void ItemGetSize(int index, int & width, int & height)
{
	width = ItemAttribute[index].Width;
	height = ItemAttribute[index].Height;
}


BOOL HasItemDurability(int index)
{
	if ( ItemAttribute[index].Durability == 0 && ItemAttribute[index].MagicDurability == 0)
	{
		return FALSE; 
	}
	
	return TRUE;
}

int ItemGetDurability(int index, int itemLevel, int ExcellentItem, int SetItem)
{

	if ( index == ITEMGET(14,21) && itemLevel == 3 )	// Mark Lord
		itemLevel=0;
	
	if ( index == ITEMGET(14,29) )
		return 1;

	if ( index == ITEMGET(14,100) )
		return 1;

	if ( index == ITEMGET(12,200) || index == ITEMGET(12,201) )
		return 5;

	int dur=0;

	if ( itemLevel < 5)
	{
		dur= ItemAttribute[index].Durability + itemLevel;
	}
	else if ( itemLevel >= 5 )
	{
		if ( itemLevel == 10 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2-3;
		}
		else if (itemLevel == 11 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2-1;
		}
		else if (itemLevel == 12 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2+2;
		}
		else if (itemLevel == 13 )
		{
			dur=ItemAttribute[index].Durability + itemLevel*2+6;
		}
		else if (itemLevel == 14)
		{
			dur=ItemAttribute[index].Durability + itemLevel*2+11;
		}
		else if (itemLevel == 15)
		{
			dur=ItemAttribute[index].Durability + itemLevel*2+17;
		}
		else
		{
			dur=ItemAttribute[index].Durability + itemLevel*2-4;
		}
	}
	
	if ( index == ITEMGET(13,51) )
	{
		dur = 1;
	}

	if ( (index < ITEMGET(12,3) || index > ITEMGET(12,6) ) &&
		  index != ITEMGET(0,19) &&
		  index != ITEMGET(4,18) &&
		  index != ITEMGET(5,10) &&
		  index != ITEMGET(2,13) &&
		  index != ITEMGET(13,30) && 
		  index != ITEMGET(12,36) &&
		  index != ITEMGET(12,37) && 
		  index != ITEMGET(12,38) && 
		  index != ITEMGET(12,39) && 
		  index != ITEMGET(12,40) && 
		  index != ITEMGET(12,41) &&
		  index != ITEMGET(12,42) && 
		  index != ITEMGET(12,43) )
	{
		if ( SetItem != 0 )
			dur +=20;
		else if ( ExcellentItem != 0 ) // Prevent duple if items
			dur +=15;
	}

	if ( dur > 255 )
		dur = 255;

	return dur;
}



int ItemGetAttackDurability(int index)
{
	return ItemAttribute[index].AttackDur;
}


int ItemGetDefenseDurability(int index)
{
	return ItemAttribute[index].DefenceDur;
}


float GetRepairItemRate(int index)
{
	return ItemAttribute[index].RepaireMoneyRate;
}
// ItemAttribute Begin:9439368 END 9446B68 Array[512]

float GetAllRepairItemRate(int index)
{
	return ItemAttribute[index].AllRepaireMoneyRate;
}

void CalRepairRate( int itemtype, int itemsubtype, LPITEM_ATTRIBUTE p)
{
	p->RepaireMoneyRate = (float)0.1;
	p->AllRepaireMoneyRate = (float)0.4;
}


BOOL OpenItemScript(char* FileName) // Save Item(kor) Values Into ItemAttribute
{
	int loop_count;
	int n;
	int Token;

	SMDFile = fopen(FileName, "r");

	if ( SMDFile == 0 )
	{
		return FALSE;
	}

	memset ( ItemAttribute, 0, sizeof(ItemAttribute) );
	
	for ( n=0;n<MAX_ITEMS;n++)
	{
		ItemAttribute[n].Level = -1;
		ItemAttribute[n].RepaireMoneyRate  = 0;
		ItemAttribute[n].AllRepaireMoneyRate  = 0;
	}

	
	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}
		if ( Token == 1 )
		{
			int ItemType;

			ItemType = TokenNumber;
			loop_count = 0;
			while ( true )
			{
				int ItemSubType;
				LPITEM_ATTRIBUTE p;
				
				Token = GetToken();
				ItemSubType = TokenNumber;

				if ( ItemType >= 0 && ItemType < MAX_TYPE_ITEMS )
				{
					if ( g_MaxItemIndexOfEachItemType[ItemType] <= ItemSubType )
					{
						g_MaxItemIndexOfEachItemType[ItemType] = ItemSubType;
					}
				}

				
				if ( Token == 0 )
				{
					if ( strcmp("end", TokenString ) == 0 )
					{
						break;
					}
				}
					
				p = &ItemAttribute[ItemType*MAX_SUBTYPE_ITEMS + ItemSubType];

				Token = GetToken();
				p->ItemSlot = TokenNumber;

				Token = GetToken();
				p->SkillType = TokenNumber;

				Token = GetToken();
				p->Width  = TokenNumber;

				Token = GetToken();
				p->Height  = TokenNumber;

				Token = GetToken();
				p->Serial = TokenNumber;

				Token = GetToken();
				p->OptionFlag = TokenNumber;

				Token = GetToken();
				p->MondownFlag = TokenNumber;

				Token = GetToken();
				strcpy(&p->Name[0], TokenString );

				if ( ItemType >= 0 && ItemType <= 5 )
				{
					Token = GetToken();
					p->Level = TokenNumber;

					Token = GetToken();
					p->DamageMin = TokenNumber;

					Token = GetToken();
					p->DamageMax = TokenNumber;

					Token = GetToken();
					p->AttackSpeed = TokenNumber;

					Token = GetToken();
					p->Durability = TokenNumber;

					Token = GetToken();
					p->MagicDurability = TokenNumber;

					Token = GetToken();
					p->MagicPW = TokenNumber;

					Token = GetToken();
					p->RequireLevel = TokenNumber;

					Token = GetToken();
					p->RequireStrength = TokenNumber;

					Token = GetToken();
					p->RequireDexterity = TokenNumber;

					Token = GetToken();
					p->RequireEnergy = TokenNumber;

					Token = GetToken();
					p->RequireVitality = TokenNumber;

					Token = GetToken();
					p->RequireLeadership = TokenNumber;

					if ( p->Width  >= 2)
					{
						p->TwoHand = TRUE; // Review this later
					}

					(BYTE)p->Durability += (BYTE)p->MagicDurability;
				}

				if ( ItemType >= 6 && ItemType <= 11 )
				{
					Token = GetToken();
					p->Level = TokenNumber;
					
					if ( ItemType == 6 )
					{

						Token = GetToken();
						p->Defense = TokenNumber;

						Token = GetToken();
						p->SuccessfulBlocking  = TokenNumber;

					}
					else if ( ItemType >= 7 && ItemType <= 9 )
					{

						Token = GetToken();
						p->Defense  = TokenNumber;

						Token = GetToken();
						p->MagicDefense = TokenNumber;
					}
					else if ( ItemType == 10 )
					{
						Token = GetToken();
						p->Defense = TokenNumber;

						Token = GetToken();
						p->AttackSpeed = TokenNumber;
					}
					else if ( ItemType == 11 )
					{
						Token = GetToken();
						p->Defense  = TokenNumber;

						Token = GetToken();
						p->WalkSpeed = TokenNumber;
					}
					
					Token = GetToken();
					p->Durability = TokenNumber;

					Token = GetToken();
					p->RequireLevel = TokenNumber;

					Token = GetToken();
					p->RequireStrength = TokenNumber;

					Token = GetToken();
					p->RequireDexterity = TokenNumber;

					Token = GetToken();
					p->RequireEnergy = TokenNumber;

					Token = GetToken();
					p->RequireVitality = TokenNumber;

					Token = GetToken();
					p->RequireLeadership = TokenNumber;
				}

				if ( ItemType == 13 )
				{
					Token = GetToken();
					p->Level = TokenNumber;

					Token = GetToken();
					p->Durability = TokenNumber;

					Token = GetToken();
					p->Resistance[0] = TokenNumber;

					Token = GetToken();
					p->Resistance[1] = TokenNumber;

					Token = GetToken();
					p->Resistance[2] = TokenNumber;

					Token = GetToken();
					p->Resistance[3] = TokenNumber;

					Token = GetToken();
					p->Resistance[4] = TokenNumber;

					Token = GetToken();
					p->Resistance[5] = TokenNumber;

					Token = GetToken();
					p->Resistance[6] = TokenNumber;

					p->ResistanceType = -1;
					
					int n;

					for (n=0;n<MAX_ITEM_SPECIAL_ATTRIBUTE-1;n++)
					{
						if ( (p->Resistance[n]) != 0 )
						{
							p->ResistanceType=n;
						}
					}

					p->RequireLevel = p->Level ;
					
				}

				if ( ItemType == 14 )
				{
					Token = GetToken();
					p->Value = TokenNumber;

					Token = GetToken();
					p->Level = TokenNumber;

					p->Durability = 1;
				}

				if ( ItemType == 12 )
				{
					Token = GetToken();
					p->Level = TokenNumber;

					Token = GetToken();
					p->Defense = TokenNumber;

					Token = GetToken();
					p->Durability = TokenNumber;

					Token = GetToken();
					p->RequireLevel = TokenNumber;

					Token = GetToken();
					p->RequireEnergy = TokenNumber;

					Token = GetToken();
					p->RequireStrength = TokenNumber;

					Token = GetToken();
					p->RequireDexterity = TokenNumber;

					Token = GetToken();
					p->RequireLeadership = TokenNumber;

					Token = GetToken();
					p->BuyMoney = TokenNumber;

				}
				
				if ( ItemType == 15 )
				{

					Token = GetToken();
					p->Level = TokenNumber;

					Token = GetToken();
					p->RequireLevel  = TokenNumber;

					Token = GetToken();
					p->RequireEnergy  = TokenNumber;

					Token = GetToken();
					p->BuyMoney  = TokenNumber;

					p->DamageMin = p->Level;
					p->DamageMax = p->Level + p->Level/2; 
				}

				if ( ItemType <= 11 || ItemType == 13)
				{
					Token = GetToken();
				}

				p->SetAttr = TokenNumber;

				if ( ItemType <= 13 || ItemType == 15 )
				{

					Token = GetToken();
					p->RequireClass[0]  = TokenNumber;

					Token = GetToken();
					p->RequireClass[1]  = TokenNumber;

					Token = GetToken();
					p->RequireClass[2]  = TokenNumber;

					Token = GetToken();
					p->RequireClass[3]  = TokenNumber;

					Token = GetToken();
					p->RequireClass[4]  = TokenNumber;

					Token = GetToken();
					p->RequireClass[5]	= TokenNumber;

					Token = GetToken();
					p->RequireClass[6]  = TokenNumber;
				}

				CalRepairRate(ItemType, ItemSubType, p);

				p->HaveItemInfo = TRUE;

				loop_count++;

				if ( loop_count > MAX_SUBTYPE_ITEMS )
				{
					MsgBox("Error : Item Data fail. (LoopCount:%d) Item Type:%d Index %d", loop_count, ItemType, ItemSubType);
					break;
				}
			}

			if ( loop_count > MAX_SUBTYPE_ITEMS )
			{
				break;
			}
			
		}


	}

	fclose(SMDFile);
	return TRUE;
}


BOOL OpenItemNameScript(char* FileName)	// To Change Name of Kor To Language Selected
{
	
	int Token;

	SMDFile = fopen(FileName, "r");

	if (SMDFile == 0 )
	{
		return FALSE;
	}

	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}
		if ( Token == 1 )
		{
			int ItemType;

			ItemType = TokenNumber;

			while ( true )
			{
				int ItemSubType;
				LPITEM_ATTRIBUTE p;

				Token = GetToken();
				ItemSubType = TokenNumber;

				if ( Token == 0 )
				{
					if ( strcmp("end", TokenString ) == 0 )
					{
						break;
					}
				}
					
				p = &ItemAttribute[ItemType*MAX_SUBTYPE_ITEMS + ItemSubType];

				Token = GetToken();
				strcpy(&p->Name[0], TokenString );

				
			}
		}
	}
	fclose(SMDFile);
	return TRUE;
}

BOOL OpenItemScript(char* Buffer, int iSize) // Save Item(kor) Values Into ItemAttribute
{
	CWzMemScript WzMemScript;
	int n;
	int loop_count;

	int Token;

	WzMemScript.SetBuffer(Buffer, iSize);

	memset ( ItemAttribute, 0, sizeof(ItemAttribute) );
	
	for ( n=0;n<MAX_ITEMS;n++)
	{
		ItemAttribute[n].Level = -1;
		ItemAttribute[n].RepaireMoneyRate  = 0;
		ItemAttribute[n].AllRepaireMoneyRate  = 0;
	}

	while ( true )
	{
		Token = WzMemScript.GetToken();

		if ( Token == 2 )
		{
			break;
		}
		
		if ( Token == 1 )
		{
			int ItemType;

			ItemType = WzMemScript.GetNumber();
			loop_count = 0;
			while ( true )
			{
				int ItemSubType;
				LPITEM_ATTRIBUTE p;

				
				Token = WzMemScript.GetToken();
				ItemSubType = WzMemScript.GetNumber();

				if ( ItemType >= 0 && ItemType < MAX_TYPE_ITEMS )
				{
					if ( g_MaxItemIndexOfEachItemType[ItemType] <= ItemSubType )
					{
						g_MaxItemIndexOfEachItemType[ItemType] = ItemSubType;
					}
				}
				
				if ( Token == 0 )
				{
					if ( strcmp("end", WzMemScript.GetString() ) == 0 )
					{
						break;
					}
				}
					
				p = &ItemAttribute[ItemType*MAX_SUBTYPE_ITEMS + ItemSubType];

				Token = WzMemScript.GetToken();
				p->ItemSlot = WzMemScript.GetNumber();

				Token = WzMemScript.GetToken();
				p->SkillType = WzMemScript.GetNumber();

				Token = WzMemScript.GetToken();
				p->Width  = WzMemScript.GetNumber();

				Token = WzMemScript.GetToken();
				p->Height  = WzMemScript.GetNumber();

				Token = WzMemScript.GetToken();
				p->Serial = WzMemScript.GetNumber();

				Token = WzMemScript.GetToken();
				p->OptionFlag = WzMemScript.GetNumber();

				Token = WzMemScript.GetToken();
				p->MondownFlag = WzMemScript.GetNumber();

				Token = WzMemScript.GetToken();
				strcpy(&p->Name[0], WzMemScript.GetString() );

				if ( ItemType >= 0 && ItemType <= 5 )
				{
					Token = WzMemScript.GetToken();
					p->Level = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->DamageMin = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->DamageMax = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->AttackSpeed = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Durability = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->MagicDurability = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->MagicPW = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireLevel = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireStrength = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireDexterity = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireEnergy = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireVitality = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireLeadership = WzMemScript.GetNumber();

					if ( p->Width  >= 2)
					{
						p->TwoHand = TRUE; // Review this later
					}

					(BYTE)p->Durability += (BYTE)p->MagicDurability;
				}

				if ( ItemType >= 6 && ItemType <= 11 )
				{
					Token = WzMemScript.GetToken();
					p->Level = WzMemScript.GetNumber();
					
					if ( ItemType == 6 )
					{

						Token = WzMemScript.GetToken();
						p->Defense = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						p->SuccessfulBlocking  = WzMemScript.GetNumber();

					}
					else if ( ItemType >= 7 && ItemType <= 9 )
					{

						Token = WzMemScript.GetToken();
						p->Defense  = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						p->MagicDefense = WzMemScript.GetNumber();
					}
					else if ( ItemType == 10 )
					{
						Token = WzMemScript.GetToken();
						p->Defense = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						p->AttackSpeed = WzMemScript.GetNumber();
					}
					else if ( ItemType == 11 )
					{
						Token = WzMemScript.GetToken();
						p->Defense  = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						p->WalkSpeed = WzMemScript.GetNumber();
					}
					
					Token = WzMemScript.GetToken();
					p->Durability = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireLevel = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireStrength = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireDexterity = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireEnergy = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireVitality = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireLeadership = WzMemScript.GetNumber();
				}

				if ( ItemType == 13 )
				{
					Token = WzMemScript.GetToken();
					p->Level = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Durability = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Resistance[0] = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Resistance[1] = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Resistance[2] = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Resistance[3] = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Resistance[4] = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Resistance[5] = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Resistance[6] = WzMemScript.GetNumber();

					p->ResistanceType = -1;
					
					int n;

					for (n=0;n<MAX_ITEM_SPECIAL_ATTRIBUTE-1;n++)
					{
						if ( (p->Resistance[n]) != 0 )
						{
							p->ResistanceType=n;
						}
					}

					p->RequireLevel = p->Level ;
					
				}

				if ( ItemType == 14 )
				{
					Token = WzMemScript.GetToken();
					p->Value = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Level = WzMemScript.GetNumber();

					p->Durability = 1;
				}

				if ( ItemType == 12 )
				{
					Token = WzMemScript.GetToken();
					p->Level = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Defense = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->Durability = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireLevel = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireEnergy = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireStrength = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireDexterity = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireLeadership = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->BuyMoney = WzMemScript.GetNumber();

				}
				
				if ( ItemType == 15 )
				{

					Token = WzMemScript.GetToken();
					p->Level = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireLevel  = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireEnergy  = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->BuyMoney  = WzMemScript.GetNumber();

					p->DamageMin = p->Level;
					p->DamageMax = p->Level + p->Level/2; 
				}

				if ( ItemType <= 11 || ItemType == 13)
				{
					Token = WzMemScript.GetToken();
				}

				p->SetAttr = WzMemScript.GetNumber();

				if ( ItemType <= 13 || ItemType == 15 )
				{

					Token = WzMemScript.GetToken();
					p->RequireClass[0]  = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireClass[1]  = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireClass[2]  = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireClass[3]  = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					p->RequireClass[4]  = WzMemScript.GetNumber();
				}

				CalRepairRate(ItemType, ItemSubType, p);

				p->HaveItemInfo = TRUE;

				loop_count++;

				if ( loop_count > MAX_SUBTYPE_ITEMS )
				{
					MsgBox("Error : Item Data fail. (LoopCount:%d) Item Type:%d Index %d", loop_count, ItemType, ItemSubType);
					break;
				}
			}
		}
	}
	
	return TRUE;

}

BOOL OpenItemNameScript(char* Buffer, int iSize)	// To Change Name of Kor To Language Selected
{
	CWzMemScript WzMemScript;
	int Token;

	WzMemScript.SetBuffer(Buffer, iSize);

	while ( true )
	{
		Token = WzMemScript.GetToken();

		if ( Token == 2 )
		{
			break;
		}

		if ( Token == 1 )
		{
			int ItemType;

			ItemType = WzMemScript.GetNumber();

			while ( true )
			{
				int ItemSubType;
				LPITEM_ATTRIBUTE p;

				Token = WzMemScript.GetToken();
				ItemSubType = WzMemScript.GetNumber();
				
				if ( Token == 0 )
				{
					if ( strcmp("end", WzMemScript.GetString() ) == 0 )
					{
						break;
					}
				}
					
				p = &ItemAttribute[ItemType*MAX_SUBTYPE_ITEMS + ItemSubType];

				Token = WzMemScript.GetToken();
				strcpy(&p->Name[0], WzMemScript.GetString() );

				
			}
		}
	}
	return TRUE;
}



int zzzItemLevel(int type, int index, int level)
{
	int item_num = (type*MAX_SUBTYPE_ITEMS)+index ;

	if (level < 0 )
	{
		level = 0;
	}
	
	if ( ItemAttribute[item_num].Level == (BYTE)-1 )
	{
		return 0;
	}

	if ( ItemAttribute[item_num].Level == 0 )
	{
		return 0;
	}

	if ( ItemAttribute[item_num].Level < level )
	{
		return 1;
	}

	return 0;
}





int GetLevelItem(int type, int index, int level)
{
	int item_num;
	int itemlevel;

	if ( level < 0 )
	{
		level = 0;
	}

	item_num = (type * MAX_SUBTYPE_ITEMS) + index;

	if (ItemAttribute[item_num].MondownFlag  == 0)
	{
		return -1;
	}

	if ( ItemAttribute[item_num].Level == 0xFF )
	{
		return -1;
	}

	if ( ItemAttribute[item_num].Level == 0 )
	{
		return -1;
	}

	if ( type == 14 )
	{
		itemlevel = ItemAttribute[item_num].Level;

		if ( index == 15 )
		{
			return -1;
		}

		if ( itemlevel >= (level-8) )
		{
			if ( itemlevel <= level )
			{
				return 0;
			}
		}

		return -1;
	}

	if ( type == 13 && index == 10  )
	{
		int ilevel;

		if ( (rand()%10) == 0 )
		{
			ilevel = 0;

			if ( level < 0 )
			{
				level = 0;
			}

			ilevel= level/10;

			if ( ilevel > 0 )
			{
				ilevel--;
			}

			if ( ilevel > 5)
			{
				ilevel = 5;
			}

			return ilevel;
		}

		return -1;
	}

	if ( type == 12 && index ==  11 )
	{
		int ilevel;

		if ( (rand()%10) == 0 )
		{
			ilevel = 0;

			if ( level < 0 )
			{
				level = 0;
			}

			ilevel= level/10;

			if ( ilevel > 0 )
			{
				ilevel--;
			}

			if ( ilevel > 6)
			{
				ilevel = 6;
			}

			return ilevel;
		}

		return -1;
	}	

	itemlevel = ItemAttribute[item_num].Level;
	
	if ( itemlevel >= level - 18 && itemlevel <= level)
	{
		if ( type == 15 )
		{
			return 0;
		}

		itemlevel = (level - itemlevel)/3;

		if ( type == 13 )
		{
			if ( index == 8 || index == 9 || index == 12 || index == 13 || index == 20 || index == 21 || index == 22 || index == 23 || index == 24 || index == 25 || index == 26 || index == 27 || index == 28 )
			{
				if ( itemlevel > 4 )
				{
					itemlevel=4;
				}
			}
		}

		return itemlevel;
		
	}
	return -1;
}


int GetSerialItem(int type)
{
	int item_num = type;

	if ( ItemAttribute[item_num].Level == 0xFF )
	{
		return -1;
	}
	if ( ItemAttribute[item_num].Serial  == 0 )
	{
		return 0;
	}
	return 1;
}




int IsItem(int item_num)
{
	if ( item_num < 0 || item_num >= MAX_ITEMS )
	{
		return 0;
	}

	return ItemAttribute[item_num].HaveItemInfo;
}




LPITEM_ATTRIBUTE GetItemAttr(int item_num)
{
	if ( item_num < 0 || item_num >= MAX_ITEMS )
	{
		return NULL;
	}

	return &ItemAttribute[item_num];
}

BOOL CItem::Is3rdLevelWingReflect()
{
	if ( m_Type < ITEMGET(12,36) ||
		 m_Type > ITEMGET(12,40) )
	{

	}
	else if ( m_Durability < 1.0f )
	{

	}
	else if ( (m_NewOption & 2) == 2 ) return 5;
return 0;
}

BOOL CItem::Is3rdLevelWingHPRestore()
{
	if ( m_Type < ITEMGET(12,36) ||
		 m_Type > ITEMGET(12,40) )
	{

	}
	else if ( m_Durability < 1.0f )
	{

	}
	else if ( (m_NewOption & 4) == 4 ) return 5;
return 0;
}

BOOL CItem::Is25LevelWingHPRestore()
{
	if ( m_Type < ITEMGET(12,262) ||
		 m_Type > ITEMGET(12,265) )
	{

	}
	else if ( m_Durability < 1.0f )
	{

	}
	else if ( (m_NewOption & 2) == 2 ) return 5;
return 0;
}

BOOL CItem::Is3rdLevelWingMPRestore()
{
	if ( m_Type < ITEMGET(12,36) ||
		 m_Type > ITEMGET(12,40) )
	{

	}
	else if ( m_Durability < 1.0f )
	{

	}
	else if ( (m_NewOption & 8) == 8 ) return 5;
return 0;
}

int CItem::GetExcOptCount()
{
	if( (m_NewOption & 0x7F) == 0 )
		return 0;

	int excop = m_NewOption;

	if( m_Type >= ITEMGET(12,0) && m_Type <= ITEMGET(12,6) )
		excop &= 0x1F;

	if( m_Type >= ITEMGET(12,36) && m_Type <= ITEMGET(12,40) )
		excop &= 0x0F;

	int count = 0;

	for(int i = 0; i < 6; i++)
	{
		if(excop & (1 << i))
		{
			excop -= (1 << i);
			count++;
		}
	}
return count;
}

int CItem::IsFullAncient()
{
	if( m_SetOption & 3 )
	{
		if( m_Level >= 15 && m_Option3 == 7 )
		{
			return TRUE;
		}
	}
return FALSE;
}

bool CItem::IsFullExcellent() {
	int ExcellentCount = 0;

	for (int i = 0; i < 6; i++) {
		if((m_NewOption >> i) & 1) {
			ExcellentCount++;
		}
	}

	if (ExcellentCount >= 5) {
		return true;
	}

	return false;
}

int CItem::GetDetailItemType()
{
	ITEM_ATTRIBUTE *p; // [sp+54h] [bp-8h]@1
	int nSum; // [sp+50h] [bp-Ch]@26
	signed int i; // [sp+4Ch] [bp-10h]@26

	p = &ItemAttribute[m_Type];

	if ( this->m_Type < 0 )
		return -1;


	if ( this->m_Type < ITEMGET(1,0) )
	{
		if(this->m_TwoHand == 1)
		{
			return 0;
		}
		else
		{
			if( this->m_Type >= ITEMGET(0,32) && this->m_Type <= ITEMGET(0,35) )
				return 12;
			else
				return 1;
		}
	}
	else if( m_Type < ITEMGET(2,0) )
	{

	}
	else if( m_Type < ITEMGET(3,0) )
	{

		if ( this->m_Type == ITEMGET(2,5) )
			return 0;

		if ( this->m_Type == ITEMGET(2,6) )
			return -1;

		// force wave skill
		if ( p->SkillType == 66 )
			return 11;
		else
			return 2;
	}
	else if(m_Type < ITEMGET(4,0) )
	{
		return 3;
	}
	else if(m_Type < ITEMGET(5,0) )
	{
		if ( m_TwoHand == 1 )
		{
			if ( p->ItemSlot == 0)
				return 8;
			else
				return 7;
		}
	}
	else if ( m_Type < ITEMGET(6,0) )
	{
		nSum = 0;
		for ( i = 0; i < MAX_TYPE_PLAYER; ++i )
		{
			if ( this->m_RequireClass[i] > 0 )
				nSum += i;
		}

		if ( nSum == 5 )
		{
			if ( this->m_Type != ITEMGET(5,0) )
			{
				if ( p->ItemSlot == 0 )
					return 9;

				if ( p->ItemSlot == 1 )
					return 10;
			}
		}
		else
		{
			if ( this->m_TwoHand == 1 )
				return 5;
			else
				return 4;
		}
	}
	else if ( m_Type < ITEMGET(7,0) )
	{
		return 6;
	}

	switch( m_Type )
	{
	case ITEMGET(12,36):	return 20;
	case ITEMGET(12,37):	return 21;
	case ITEMGET(12,38):	return 22;
	case ITEMGET(12,39):	return 23;
	case ITEMGET(12,40):	return 24;
	case ITEMGET(12,43):	return 25;
	}
	
	return -1;
}

// ItemAttribute Begin:9439368 END 9446B68 Array[512]