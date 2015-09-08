#include "stdafx.h"
#include "ObjCalCharacter.h"
#include "Gamemain.h"
#include "DarkSpirit.h"
#include "ItemAddOption.h"
#include "..\common\SetItemOption.h"
#include "LogProc.h"
#include "CastleSiegeSync.h"
#include "BuffEffectSlot.h"
#include "ElementalSystem.h"
#include "GameEvent.h"
#include "MasterLevelSkillTreeSystem.h"
#include "ClassCalc.h"
#include "ExtraClassBalance.h"
#ifdef __NOVUS__
#include "StatGem.h"
#endif

#if (ENABLETEST_ZTLUA == 1)
#include "ZTLuaManager.h"
#endif

#if (ENABLETEST_MUUN == 1)
#include "MuunSystem.h"
#endif

void gObjCalCharacter(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int Strength = 0;
	int Dexterity = 0;
	int Vitality = 0;
	int Energy = 0;
	CItem * Right = &lpObj->pInventory[0];
	CItem * Left  = &lpObj->pInventory[1];
	CItem * Gloves = &lpObj->pInventory[5];
	CItem * Amulet = &lpObj->pInventory[9];
	CItem * Helper = &lpObj->pInventory[8];

	lpObj->AddLife = 0;
	lpObj->AddMana = 0;
	lpObj->MonsterDieGetMoney = 0;
	lpObj->MonsterDieGetLife = 0;
	lpObj->MonsterDieGetMana = 0;
	lpObj->DamageReflect = 0;
	lpObj->DamageMinus = 0;
	lpObj->SkillLongSpearChange = false;

#if (CUSTOM_EXOPT==1)
	lpObj->m_CriticalDamageMul = 0;
	lpObj->m_ExcelentDamageMul = 0;
#endif

	if ( lpObj->m_iItemEffectValidTime > 0 )
	{
		g_ItemAddOption.PrevSetItemLastEffectForHallowin(lpObj);
	}

	int iItemIndex;
	BOOL bIsChangeItem;	// lc34

	for ( iItemIndex=0; iItemIndex<MAX_PLAYER_EQUIPMENT;iItemIndex++)
	{
		if ( lpObj->pInventory[iItemIndex].IsItem() != FALSE )
		{
			lpObj->pInventory[iItemIndex].m_IsValidItem = true;
		}
	}

	do
	{
		lpObj->SetOpAddMaxAttackDamage = 0;
		lpObj->SetOpAddMinAttackDamage = 0;
		lpObj->SetOpAddDamage = 0;
		lpObj->SetOpIncAGValue = 0;
		lpObj->SetOpAddCriticalDamageSuccessRate = 0;
		lpObj->SetOpAddCriticalDamage = 0;
		lpObj->SetOpAddExDamageSuccessRate = 0;
		lpObj->SetOpAddExDamage = 0;
		lpObj->SetOpAddSkillAttack = 0;
		lpObj->AddStrength = 0;
		lpObj->AddDexterity = 0;
		lpObj->AddVitality = 0;
		lpObj->AddEnergy = 0;
		lpObj->AddLeadership = 0;
		lpObj->AddBP = 0;
		lpObj->iAddShield = 0;
		lpObj->SetOpAddAttackDamage = 0;
		lpObj->SetOpAddDefence = 0;
		lpObj->SetOpAddMagicPower = 0;
		lpObj->SetOpAddDefenceRate = 0;
		lpObj->SetOpIgnoreDefense = 0;
		lpObj->SetOpDoubleDamage = 0;
		lpObj->SetOpTwoHandSwordImproveDamage = 0;
		lpObj->SetOpImproveSuccessAttackRate = 0;
		lpObj->SetOpReflectionDamage = 0;
		lpObj->SetOpImproveSheldDefence = 0;
		lpObj->SetOpDecreaseAG = 0;
		lpObj->SetOpImproveItemDropRate = 0;
		lpObj->IsFullSetItem = false;

		memset(lpObj->m_AddResistance, 0, sizeof(lpObj->m_AddResistance));
		bIsChangeItem = 0;

		g_ItemAddOption.SetPeriodItemEffect1(lpObj); //Why disabled?
		g_BuffEffect.SetPrevEffect(lpObj);

		gObjCalcSetItemStat(lpObj);
		gObjCalcSetItemOption(lpObj);

		g_MasterSkillSystem.SetItemMLPassiveSkill(lpObj,lpObj->pInventory[0].GetDetailItemType());
		g_MasterSkillSystem.SetItemMLPassiveSkill(lpObj,lpObj->pInventory[1].GetDetailItemType());
		g_MasterSkillSystem.SetItemMLPassiveSkill(lpObj,lpObj->pInventory[7].GetDetailItemType());
		g_MasterSkillSystem.SetPetItemMLPassiveSkill(lpObj,lpObj->pInventory[8].m_Type);

		if( gObjCheckUsedBuffEffect(lpObj,BUFF_INCREASE_VIT) == TRUE )
		{
			int nIncreasedVitality = 0;
			gObjGetValueOfBuffIndex(lpObj,BUFF_INCREASE_VIT,&nIncreasedVitality,0);
			lpObj->AddVitality += nIncreasedVitality; 
		}

		if( gObjCheckUsedBuffEffect(lpObj,BUFF_BLESS) == TRUE ||
			gObjCheckUsedBuffEffect(lpObj,BUFF_BLESS2) == TRUE )
		{
			int nIncreaseStats = gObjGetTotalValueOfEffect(lpObj,ADD_OPTION_ALLSTATS);
			lpObj->AddStrength += nIncreaseStats;
			lpObj->AddDexterity += nIncreaseStats;
			lpObj->AddVitality += nIncreaseStats;
			lpObj->AddEnergy += nIncreaseStats;
			lpObj->AddLeadership += nIncreaseStats;
		}

		lpObj->AddStrength += lpObj->m_MPSkillOpt.MpsStrengthIncrease;
		lpObj->AddDexterity += lpObj->m_MPSkillOpt.MpsDexterityIncrease;
		lpObj->AddVitality += lpObj->m_MPSkillOpt.MpsVitalityIncrease;
		lpObj->AddEnergy += lpObj->m_MPSkillOpt.MpsEnergyIncrease;
		lpObj->AddLeadership += lpObj->m_MPSkillOpt.MpsLeadershipIncrease;

		for (iItemIndex=0;iItemIndex<MAX_PLAYER_EQUIPMENT;iItemIndex++)
		{
			if ( lpObj->pInventory[iItemIndex].IsItem() != FALSE && lpObj->pInventory[iItemIndex].m_IsValidItem != false )
			{
				if ( gObjValidItem( lpObj, &lpObj->pInventory[iItemIndex], iItemIndex) != FALSE )
				{
					lpObj->pInventory[iItemIndex].m_IsValidItem = true;
				}
				else
				{
					lpObj->pInventory[iItemIndex].m_IsValidItem  = false;
					//g_BuffEffect.ClearAllBuffEffect(lpObj); //bugged if use active buff & have wrong item
					bIsChangeItem = TRUE;
				}
			}
		}
	}
	while ( bIsChangeItem != FALSE );

	Strength = lpObj->Strength + lpObj->AddStrength;
	Dexterity = lpObj->Dexterity + lpObj->AddDexterity;
	Vitality = lpObj->Vitality + lpObj->AddVitality;
	Energy = lpObj->Energy + lpObj->AddEnergy;

	if ( g_bAbilityDebug == TRUE )
	{
		char szNotice[256];
		sprintf(szNotice,"Strength: %d,%d    Dexterity: %d,%d",
			lpObj->Strength,lpObj->AddStrength,lpObj->Dexterity,lpObj->AddDexterity);
		GCServerMsgStringSend(szNotice,lpObj->m_Index,1);
		memset(szNotice,0,sizeof(szNotice));

		sprintf(szNotice,"Vitality: %d,%d    Energy: %d,%d",
			lpObj->Vitality,lpObj->AddVitality,lpObj->Energy,lpObj->AddEnergy);
		GCServerMsgStringSend(szNotice,lpObj->m_Index,1);

		memset(szNotice,0,sizeof(szNotice));
		sprintf(szNotice,"Leadership: %d,%d",lpObj->Leadership,lpObj->AddLeadership);
		GCServerMsgStringSend(szNotice,lpObj->m_Index,1);
	}

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	g_ClassCalc.GetAttackDamage(lpObj);
#else
	if ( lpObj->Class == CLASS_ELF ) // Elf
	{
		if ( (Right->m_Type >= ITEMGET(4,8) && Right->m_Type < ITEMGET(4,15) ) ||
			 (Left->m_Type >= ITEMGET(4,0) && Left->m_Type < ITEMGET(4,7)) ||
			  Right->m_Type == ITEMGET(4,16) ||
			  Left->m_Type == ITEMGET(4,20) ||
			  Left->m_Type == ITEMGET(4,21) ||
			  Left->m_Type == ITEMGET(4,22) ||
			  Left->m_Type == ITEMGET(4,23) ||
			  Right->m_Type == ITEMGET(4,18) ||
			  Right->m_Type == ITEMGET(4,19) ||
			  Left->m_Type == ITEMGET(4,17) )
		{
			if ( (Right->IsItem() != FALSE && Right->m_IsValidItem == false) || (Left->IsItem() != FALSE && Left->m_IsValidItem == false) )
			{
				lpObj->m_AttackDamageMinRight = (Dexterity + Strength ) / 7;
				lpObj->m_AttackDamageMaxRight = (Dexterity + Strength ) / 4;
				lpObj->m_AttackDamageMinLeft = (Dexterity + Strength ) / 7;
				lpObj->m_AttackDamageMaxLeft = (Dexterity + Strength ) / 4;
			}
			else
			{
				lpObj->m_AttackDamageMinRight = (Dexterity / 7) + (Strength / 14);
				lpObj->m_AttackDamageMaxRight = (Dexterity / 4) + (Strength / 8 );
				lpObj->m_AttackDamageMinLeft = (Dexterity / 7) + (Strength / 14);
				lpObj->m_AttackDamageMaxLeft = (Dexterity / 4) + (Strength / 8 );
			}
		}
		else
		{
			lpObj->m_AttackDamageMinRight = (Dexterity + Strength) / 7;
			lpObj->m_AttackDamageMaxRight = (Dexterity + Strength) / 4;
			lpObj->m_AttackDamageMinLeft = (Dexterity + Strength) / 7;
			lpObj->m_AttackDamageMaxLeft = (Dexterity + Strength) / 4;
		}
	}
	else if ( lpObj->Class == CLASS_KNIGHT ) // Dark Knight
	{
		lpObj->m_AttackDamageMinRight = Strength / 6;
		lpObj->m_AttackDamageMaxRight = Strength / 4;
		lpObj->m_AttackDamageMinLeft = Strength / 6;
		lpObj->m_AttackDamageMaxLeft = Strength / 4;
	}
	else if (lpObj->Class == CLASS_MAGUMSA ) // MAgic Gladiator
	{
		lpObj->m_AttackDamageMinRight = (Strength / 6) + (Energy / 12);
		lpObj->m_AttackDamageMaxRight = (Strength / 4) + (Energy / 8);
		lpObj->m_AttackDamageMinLeft = (Strength / 6) + (Energy / 12);
		lpObj->m_AttackDamageMaxLeft = (Strength / 4) + (Energy / 8);
	}
	else if ( lpObj->Class == CLASS_DARKLORD ) // Dark Lord
	{
		lpObj->m_AttackDamageMinRight = (Strength / 7) + (Energy / 14);
		lpObj->m_AttackDamageMaxRight = (Strength / 5) + (Energy / 10);
		lpObj->m_AttackDamageMinLeft = (Strength / 7) + (Energy / 14);
		lpObj->m_AttackDamageMaxLeft = (Strength / 5) + (Energy / 10);
	}
	else if ( lpObj->Class == CLASS_SUMMONER )
	{
		lpObj->m_AttackDamageMinRight = (Strength + Dexterity) / 7;
		lpObj->m_AttackDamageMaxRight = (Strength + Dexterity) / 4;
		lpObj->m_AttackDamageMinLeft = (Strength + Dexterity) / 7;
		lpObj->m_AttackDamageMaxLeft = (Strength + Dexterity) / 4;
	}
	else if ( lpObj->Class == CLASS_FIGHTER )
	{
		lpObj->m_AttackDamageMinRight = (Strength / 7) + (Vitality / 15);
		lpObj->m_AttackDamageMaxRight = (Strength / 5) + (Vitality / 12);
		lpObj->m_AttackDamageMinLeft = (Strength / 7) + (Vitality / 15);
		lpObj->m_AttackDamageMaxLeft = (Strength / 5) + (Vitality / 12);
	}
	else
	{
		lpObj->m_AttackDamageMinRight = (Strength / 8);
		lpObj->m_AttackDamageMaxRight = (Strength / 4);
		lpObj->m_AttackDamageMinLeft = (Strength / 8);
		lpObj->m_AttackDamageMaxLeft = (Strength / 4);
	}
#endif

	if( lpObj->Class == CLASS_KNIGHT ||
		lpObj->Class == CLASS_ELF ||
		lpObj->Class == CLASS_DARKLORD || 
		lpObj->Class == CLASS_FIGHTER )
	{
		lpObj->m_AttackDamageMaxRight += lpObj->m_MPSkillOpt.MpsMaxAttackInc;
		lpObj->m_AttackDamageMaxLeft += lpObj->m_MPSkillOpt.MpsMaxAttackInc;

		lpObj->m_AttackDamageMinRight += lpObj->m_MPSkillOpt.MpsMinAttackInc;
		lpObj->m_AttackDamageMinLeft += lpObj->m_MPSkillOpt.MpsMinAttackInc;
	}

	lpObj->pInventory[7].PlusSpecial(&lpObj->m_AttackDamageMinRight, 80);
	lpObj->pInventory[7].PlusSpecial(&lpObj->m_AttackDamageMaxRight, 80);
	lpObj->pInventory[7].PlusSpecial(&lpObj->m_AttackDamageMinLeft, 80);
	lpObj->pInventory[7].PlusSpecial(&lpObj->m_AttackDamageMaxLeft, 80);

	int AddLeadership = 0;

	if ( lpObj->pInventory[7].IsItem() != FALSE && lpObj->pInventory[7].m_IsValidItem != false)
	{
		AddLeadership += lpObj->pInventory[7].m_Leadership;
	}

	if ( Right->m_Type != -1 )
	{
		if ( Right->m_IsValidItem != false )
		{
			if ( Right->m_Type >= ITEMGET(5,0) && Right->m_Type < ITEMGET(6,0) )
			{
				lpObj->m_AttackDamageMinRight += Right->m_DamageMin / 2;
				lpObj->m_AttackDamageMaxRight += Right->m_DamageMax / 2;
			}
			else
			{
				lpObj->m_AttackDamageMinRight += Right->m_DamageMin;
				lpObj->m_AttackDamageMaxRight += Right->m_DamageMax;
			}
		}

		if ( lpObj->pInventory[0].m_SkillChange != FALSE )
		{
			lpObj->SkillLongSpearChange = true;
		}

		lpObj->pInventory[0].PlusSpecial(&lpObj->m_AttackDamageMinRight, 80);
		lpObj->pInventory[0].PlusSpecial(&lpObj->m_AttackDamageMaxRight, 80);
	}
		
	if ( Left->m_Type != -1 )
	{
		if ( Left->m_IsValidItem != false)
		{
			lpObj->m_AttackDamageMinLeft += Left->m_DamageMin;
			lpObj->m_AttackDamageMaxLeft += Left->m_DamageMax;
		}

		lpObj->pInventory[1].PlusSpecial(&lpObj->m_AttackDamageMinLeft, 80);
		lpObj->pInventory[1].PlusSpecial(&lpObj->m_AttackDamageMaxLeft, 80);
	}

	lpObj->m_CriticalDamage = 0;
	lpObj->m_ExcelentDamage = 0;

	lpObj->pInventory[0].PlusSpecial(&lpObj->m_CriticalDamage, 84);
	lpObj->pInventory[1].PlusSpecial(&lpObj->m_CriticalDamage, 84);
	lpObj->pInventory[2].PlusSpecial(&lpObj->m_CriticalDamage, 84);
	lpObj->pInventory[3].PlusSpecial(&lpObj->m_CriticalDamage, 84);
	lpObj->pInventory[4].PlusSpecial(&lpObj->m_CriticalDamage, 84);
	lpObj->pInventory[5].PlusSpecial(&lpObj->m_CriticalDamage, 84);
	lpObj->pInventory[6].PlusSpecial(&lpObj->m_CriticalDamage, 84);
	lpObj->pInventory[7].PlusSpecial(&lpObj->m_CriticalDamage, 84);


	if( gObjCheckUsedBuffEffect(lpObj,BUFF_ADD_CRITDMG3) == TRUE )
	{
		lpObj->m_ExcelentDamage += lpObj->m_MPSkillOpt.MpsCritBuffExcDamage;
	}

	/*lpObj->m_CriticalDamage += gObjGetTotalValueOfEffect(lpObj,ADD_OPTION_INCCRITDMG);
	iValue = 0;
	gObjGetValueOfBuffIndex(lpObj,BUFF_ICE,&iValue,0);
	lpObj->m_ExcelentDamage += iValue;*/

	if( lpObj->m_MPSkillOpt.MpsCriticalRateIncrease > 0.0f )
	{
		lpObj->m_CriticalDamage += lpObj->m_MPSkillOpt.MpsCriticalRateIncrease;
	}

	if( lpObj->m_MPSkillOpt.MpsExcelentRateIncrease > 0.0f )
	{
		lpObj->m_ExcelentDamage += lpObj->m_MPSkillOpt.MpsExcelentRateIncrease;
	}

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	g_ClassCalc.GetMagicDamage(lpObj);
#else
	lpObj->m_MagicDamageMin = Energy / 9;
	lpObj->m_MagicDamageMax = Energy / 4;

	if( lpObj->Class == CLASS_SUMMONER )
	{
		lpObj->m_MagicDamageMin = Energy / 9;
		lpObj->m_MagicDamageMax = (int)(double(0.015) + Energy / 4);
		lpObj->m_CurseDamageMin = Energy / 9;
		lpObj->m_CurseDamageMax = (int)(double(0.015) + Energy / 4);

		lpObj->m_MagicDamageMax += lpObj->m_MPSkillOpt.MpsMaxMagicDamage;
		lpObj->m_MagicDamageMin += lpObj->m_MPSkillOpt.MpsMinMagicDamage;
	}
#endif

	if( lpObj->Class == CLASS_WIZARD || lpObj->Class == CLASS_MAGUMSA )
	{
		lpObj->m_MagicDamageMax += lpObj->m_MPSkillOpt.MpsMaxMagicDamage;
		lpObj->m_MagicDamageMin += lpObj->m_MPSkillOpt.MpsMinMagicDamage;
		lpObj->m_MagicDamageMax += lpObj->m_MPSkillOpt.MpsMagicMastery;
		lpObj->m_MagicDamageMin += lpObj->m_MPSkillOpt.MpsMagicMastery;
	}

	lpObj->pInventory[7].PlusSpecial(&lpObj->m_MagicDamageMin, 81);
	lpObj->pInventory[7].PlusSpecial(&lpObj->m_MagicDamageMax, 81);
	lpObj->pInventory[7].PlusSpecial(&lpObj->m_CurseDamageMin, 113);
	lpObj->pInventory[7].PlusSpecial(&lpObj->m_CurseDamageMax, 113);

	if ( Right->m_Type != -1 )
	{
		if ( lpObj->pInventory[0].m_Type == ITEMGET(0,31) ||
			 lpObj->pInventory[0].m_Type == ITEMGET(0,21) ||
			 lpObj->pInventory[0].m_Type == ITEMGET(0,23) ||
			 lpObj->pInventory[0].m_Type == ITEMGET(0,25) ||
			 lpObj->pInventory[0].m_Type == ITEMGET(0,28) ) 
		{
			lpObj->pInventory[0].PlusSpecial(&lpObj->m_MagicDamageMin, 80);
			lpObj->pInventory[0].PlusSpecial(&lpObj->m_MagicDamageMax, 80);
		}
		else
		{
			lpObj->pInventory[0].PlusSpecial(&lpObj->m_MagicDamageMin, 81);
			lpObj->pInventory[0].PlusSpecial(&lpObj->m_MagicDamageMax, 81);
		}
	}

	if ( Left->m_Type != -1 )
	{
		lpObj->pInventory[1].PlusSpecial(&lpObj->m_CurseDamageMin, 113);
		lpObj->pInventory[1].PlusSpecial(&lpObj->m_CurseDamageMax, 113);
	}

	lpObj->m_AttackRating = (Strength + Dexterity) / 2;
	lpObj->m_AttackRating += lpObj->pInventory[5].ItemDefense();

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	g_ClassCalc.GetAttackSpeed(lpObj);
#else
	if ( lpObj->Class == CLASS_ELF ) // Elf
	{
		lpObj->m_AttackSpeed = Dexterity / 50;
		lpObj->m_MagicSpeed = Dexterity / 50;
	}
	else if ( lpObj->Class == CLASS_KNIGHT || lpObj->Class == CLASS_MAGUMSA) // Dark Knigh and MG
	{
		lpObj->m_AttackSpeed = Dexterity / 15;
		lpObj->m_MagicSpeed = Dexterity / 20;
	}
	else if ( lpObj->Class == CLASS_DARKLORD ) // DarkLord
	{
		lpObj->m_AttackSpeed = Dexterity / 10;
		lpObj->m_MagicSpeed = Dexterity / 10;
	}
	else if ( lpObj->Class == CLASS_SUMMONER )
	{
		lpObj->m_AttackSpeed = Dexterity / 20;
		lpObj->m_MagicSpeed = Dexterity / 20;
	}
	else if ( lpObj->Class == CLASS_FIGHTER )
	{
		lpObj->m_AttackSpeed = Dexterity / 9;
		lpObj->m_MagicSpeed = Dexterity / 9;
	}
	else
	{
		lpObj->m_AttackSpeed = Dexterity / 20;
		lpObj->m_MagicSpeed = Dexterity / 10;
	}
#endif

	bool bRight = false;
	bool bLeft = false;

	if ( Right->m_Type != ITEMGET(4,7) && Right->m_Type != ITEMGET(4,15) && Right->m_Type >= ITEMGET(0,0) && Right->m_Type < ITEMGET(6,0) )
	{
		if ( Right->m_IsValidItem != false &&
			 Right->m_Durability > 0.0f)
		{
			bRight = true;
		}
	}

	if ( Left->m_Type != ITEMGET(4,7) && Left->m_Type != ITEMGET(4,15) && Left->m_Type >= ITEMGET(0,0) && Left->m_Type < ITEMGET(6,0) )
	{
		if ( Left->m_IsValidItem != false &&
			 Left->m_Durability > 0.0f)
		{
			bLeft = true;
		}
	}

	if ( bRight != false && bLeft != false )
	{
		lpObj->m_AttackSpeed += (Right->m_AttackSpeed + Left->m_AttackSpeed)/2;
		lpObj->m_MagicSpeed += (Right->m_AttackSpeed + Left->m_AttackSpeed)/2;
	}
	else if ( bRight!= false )
	{
		lpObj->m_AttackSpeed += Right->m_AttackSpeed;
		lpObj->m_MagicSpeed += Right->m_AttackSpeed;
	}
	else if ( bLeft != false )
	{
		lpObj->m_AttackSpeed += Left->m_AttackSpeed;
		lpObj->m_MagicSpeed += Left->m_AttackSpeed;
	}

	if ( Gloves->m_Type != -1 )
	{
		if ( Gloves->m_IsValidItem != false )
		{
			lpObj->m_AttackSpeed += Gloves->m_AttackSpeed;
			lpObj->m_MagicSpeed += Gloves->m_AttackSpeed;
		}
	}

	if ( Helper->m_Type != -1 )
	{
		if ( Helper->m_IsValidItem != false )
		{
			lpObj->m_AttackSpeed += Helper->m_AttackSpeed;
			lpObj->m_MagicSpeed += Helper->m_AttackSpeed;
		}
	}

	if ( Amulet->m_Type != -1 )
	{
		if ( Amulet->m_IsValidItem != false )
		{
			lpObj->m_AttackSpeed += Amulet->m_AttackSpeed;
			lpObj->m_MagicSpeed += Amulet->m_AttackSpeed;
		}
	}

	// Wizard Ring effect
	if ( (lpObj->pInventory[10].IsItem() == TRUE && lpObj->pInventory[10].m_Type == ITEMGET(13,20) && lpObj->pInventory[10].m_Level == 0 && lpObj->pInventory[10].m_Durability > 0.0f ) ||
		 (lpObj->pInventory[11].IsItem() == TRUE && lpObj->pInventory[11].m_Type == ITEMGET(13,20) && lpObj->pInventory[11].m_Level == 0 && lpObj->pInventory[11].m_Durability > 0.0f ) )
	{
		lpObj->m_AttackDamageMinRight += lpObj->m_AttackDamageMinRight * 10 / 100;
		lpObj->m_AttackDamageMaxRight += lpObj->m_AttackDamageMaxRight * 10 / 100;
		lpObj->m_AttackDamageMinLeft += lpObj->m_AttackDamageMinLeft * 10 / 100;
		lpObj->m_AttackDamageMaxLeft += lpObj->m_AttackDamageMaxLeft * 10 / 100;
		lpObj->m_MagicDamageMin += lpObj->m_MagicDamageMin * 10 / 100;
		lpObj->m_MagicDamageMax += lpObj->m_MagicDamageMax * 10 / 100;
		lpObj->m_AttackSpeed += 10;
		lpObj->m_MagicSpeed += 10;
	}

	if ( (lpObj->pInventory[10].IsItem() == TRUE && lpObj->pInventory[10].m_Type == ITEMGET(13,107) && lpObj->pInventory[10].m_Level == 0 && lpObj->pInventory[10].m_Durability > 0.0f ) ||
		 (lpObj->pInventory[11].IsItem() == TRUE && lpObj->pInventory[11].m_Type == ITEMGET(13,107) && lpObj->pInventory[11].m_Level == 0 && lpObj->pInventory[11].m_Durability > 0.0f ) )
	{
		lpObj->m_AttackDamageMinRight += lpObj->m_AttackDamageMinRight * 15 / 100;
		lpObj->m_AttackDamageMaxRight += lpObj->m_AttackDamageMaxRight * 15 / 100;
		lpObj->m_AttackDamageMinLeft += lpObj->m_AttackDamageMinLeft * 15 / 100;
		lpObj->m_AttackDamageMaxLeft += lpObj->m_AttackDamageMaxLeft * 15 / 100;
		lpObj->m_MagicDamageMin += lpObj->m_MagicDamageMin * 15 / 100;
		lpObj->m_MagicDamageMax += lpObj->m_MagicDamageMax * 15 / 100;
		lpObj->m_AttackSpeed += 10;
		lpObj->m_MagicSpeed += 10;
	}

	if ( lpObj->pInventory[8].m_Type == ITEMGET(13,123) &&
		 lpObj->pInventory[8].m_Durability > 0.0f )
	{
		lpObj->m_AttackDamageMaxLeft += lpObj->m_AttackDamageMaxLeft*0.2;
		lpObj->m_AttackDamageMinLeft += lpObj->m_AttackDamageMinLeft*0.2;
		lpObj->m_AttackDamageMaxRight += lpObj->m_AttackDamageMaxRight*0.2;
		lpObj->m_AttackDamageMinRight += lpObj->m_AttackDamageMinRight*0.2;
		lpObj->m_MagicDamageMin += lpObj->m_MagicDamageMin*0.2;
		lpObj->m_MagicDamageMax += lpObj->m_MagicDamageMax*0.2;
		lpObj->m_CurseDamageMin += lpObj->m_CurseDamageMin*0.2;
		lpObj->m_CurseDamageMax += lpObj->m_CurseDamageMax*0.2;
		lpObj->m_AttackSpeed += 10;
		lpObj->m_MagicSpeed += 10;
	}

	if ( (lpObj->pInventory[10].IsItem() == TRUE && lpObj->pInventory[10].m_Type == ITEMGET(13,122) && lpObj->pInventory[10].m_Level == 0 && lpObj->pInventory[10].m_Durability > 0.0f ) ||
		 (lpObj->pInventory[11].IsItem() == TRUE && lpObj->pInventory[11].m_Type == ITEMGET(13,122) && lpObj->pInventory[11].m_Level == 0 && lpObj->pInventory[11].m_Durability > 0.0f ) )
	{
		lpObj->m_AttackDamageMinRight += 40;
		lpObj->m_AttackDamageMaxRight += 40;
		lpObj->m_AttackDamageMinLeft += 40;
		lpObj->m_AttackDamageMaxLeft += 40;
		lpObj->m_MagicDamageMin += 40;
		lpObj->m_MagicDamageMax += 40;
		lpObj->m_CurseDamageMin += 40;
		lpObj->m_CurseDamageMax += 40;
	}

	if ( lpObj->Class == CLASS_WIZARD ) // Dark Wizard;
	{
		lpObj->m_DetectSpeedHackTime = (int)(gAttackSpeedTimeLimit - (lpObj->m_MagicSpeed*2 * gDecTimePerAttackSpeed) );
	}
	else
	{
		lpObj->m_DetectSpeedHackTime = (int)(gAttackSpeedTimeLimit - (lpObj->m_AttackSpeed * gDecTimePerAttackSpeed) );
	}

	if ( lpObj->m_DetectSpeedHackTime < gMinimumAttackSpeedTime )
	{
		lpObj->m_DetectSpeedHackTime = gMinimumAttackSpeedTime;
	}

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	g_ClassCalc.GetSuccessfulBlocking(lpObj);
#else
	if ( lpObj->Class == CLASS_ELF ) // elf
	{
		lpObj->m_SuccessfulBlocking = Dexterity / 4;
	}
	else if ( lpObj->Class == CLASS_DARKLORD ) // DL
	{
		lpObj->m_SuccessfulBlocking = Dexterity / 7;
	}
	else if ( lpObj->Class == CLASS_SUMMONER )
	{
		lpObj->m_SuccessfulBlocking = Dexterity / 4;
	}
	else if ( lpObj->Class == CLASS_FIGHTER )
	{
		lpObj->m_SuccessfulBlocking = Dexterity / 10;
	}
	else
	{
		lpObj->m_SuccessfulBlocking = Dexterity / 3;
	}
#endif

	if ( Left->m_Type != -1 )
	{
		if ( Left->m_IsValidItem != false )
		{
			lpObj->m_SuccessfulBlocking += Left->m_SuccessfulBlocking;
			lpObj->pInventory[1].PlusSpecial(&lpObj->m_SuccessfulBlocking, 82);
		}
	}

	bool Success = true;

	if ( lpObj->Class == CLASS_MAGUMSA ) // MG
	{
		for ( int j=3;j<=6;j++)
		{
			if ( lpObj->pInventory[j].m_Type == -1 )
			{
				Success = false;
				break;
			}

			if ( lpObj->pInventory[j].m_IsValidItem == false )
			{
				Success = false;
				break;
			}
		}
	}
	else
	{
		for  ( int k=2;k<=6;k++)
		{
			if ( lpObj->Class == CLASS_FIGHTER && k == 5 )
				continue;

			if ( lpObj->pInventory[k].m_Type == -1 )
			{
				Success = false;
				break;
			}

			if ( lpObj->pInventory[k].m_IsValidItem == false )
			{
				Success = false;
				break;
			}
		}
	}

	int Level11Count = 0;
	int Level10Count = 0;
	int Level12Count = 0;
	int Level13Count = 0;
	int Level14Count = 0;
	int Level15Count = 0;

	if ( Success != false )
	{
		int in;

		if ( lpObj->Class == CLASS_MAGUMSA ) // MG
		{
			in = lpObj->pInventory[3].m_Type % MAX_SUBTYPE_ITEMS;

			if ( in != ITEMGET(0,15) &&
				 in != ITEMGET(0,20) &&
				 in != ITEMGET(0,23) &&
				 in != ITEMGET(0,33) &&
				 in != ITEMGET(0,32) &&
				 in != ITEMGET(0,37) &&
				 in != ITEMGET(0,47) &&
				 in != ITEMGET(0,48) )
			{
				Success = false;
			}
			else
			{
				Level15Count++;

				for (int m=3;m<=6;m++)
				{
					if ( in != ( lpObj->pInventory[m].m_Type % MAX_SUBTYPE_ITEMS) )
					{
						Success = false;
					}

					if( lpObj->pInventory[m].m_Level > 14 )
					{
						Level15Count++;
					}
					else if ( lpObj->pInventory[m].m_Level > 13 )
					{
						Level14Count++;
					}
					else if ( lpObj->pInventory[m].m_Level > 12 )
					{
						Level13Count++;
					}
					else if ( lpObj->pInventory[m].m_Level > 11 )
					{
						Level12Count++;
					}
					else if ( lpObj->pInventory[m].m_Level > 10 )
					{
						Level11Count++;
					}
					else if ( lpObj->pInventory[m].m_Level > 9 )
					{
						Level10Count++;
					}
				}
			}
		}
		else
		{
			in = lpObj->pInventory[2].m_Type % MAX_SUBTYPE_ITEMS;

			if ( lpObj->Class == CLASS_FIGHTER )
			{
				Level15Count++;
			}

			for (int m=2;m<=6;m++)
			{
				if ( lpObj->Class == CLASS_FIGHTER && m == 5 )
					continue;

				if ( in !=  ( lpObj->pInventory[m].m_Type % MAX_SUBTYPE_ITEMS) )
				{
					Success = false;
				}

				if( lpObj->pInventory[m].m_Level > 14 )
				{
					Level15Count++;
				}
				else if ( lpObj->pInventory[m].m_Level > 13 )
				{
					Level14Count++;
				}
				else if ( lpObj->pInventory[m].m_Level > 12 )
				{
					Level13Count++;
				}
				else if ( lpObj->pInventory[m].m_Level > 11 )
				{
					Level12Count++;
				}
				else if ( lpObj->pInventory[m].m_Level > 10 )
				{
					Level11Count++;
				}
				else if ( lpObj->pInventory[m].m_Level > 9 )
				{
					Level10Count++;
				}
			}
		}

		if ( Success != false )	// #warning unuseful if
		{
			lpObj->m_SuccessfulBlocking += lpObj->m_SuccessfulBlocking / 10;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsBlockingRate > 0.0f )
	{
		lpObj->m_SuccessfulBlocking += lpObj->m_SuccessfulBlocking/100.0f*lpObj->m_MPSkillOpt.MpsBlockingRate;
	}

	lpObj->m_SuccessfulBlocking += lpObj->m_MPSkillOpt.MpsShieldMastery;
	lpObj->m_SuccessfulBlocking += lpObj->m_MPSkillOpt.MpsShieldMastery2;
	lpObj->m_SuccessfulBlocking += lpObj->m_MPSkillOpt.MpsShieldMastery3;

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	g_ClassCalc.GetDefense(lpObj);
#else
	if ( lpObj->Class == CLASS_ELF ) // Elf
	{
		lpObj->m_Defense = Dexterity / 10;
	}
	else if ( lpObj->Class == 1 ) // DK
	{
		lpObj->m_Defense = Dexterity / 3;
	}
	else if ( lpObj->Class == 4 ) // DL
	{
		lpObj->m_Defense = Dexterity / 7;
	}
	else if ( lpObj->Class == CLASS_SUMMONER )
	{
		lpObj->m_Defense = Dexterity / 3;
	}
	else if ( lpObj->Class == CLASS_FIGHTER )
	{
		lpObj->m_Defense = Dexterity / 8;
	}
	else
	{
		lpObj->m_Defense = Dexterity / 4;
	}
#endif

	lpObj->m_Defense += lpObj->pInventory[2].ItemDefense();
	lpObj->m_Defense += lpObj->pInventory[3].ItemDefense();
	lpObj->m_Defense += lpObj->pInventory[4].ItemDefense();
	lpObj->m_Defense += lpObj->pInventory[5].ItemDefense();
	lpObj->m_Defense += lpObj->pInventory[6].ItemDefense();
	lpObj->m_Defense += lpObj->pInventory[1].ItemDefense();
	lpObj->m_Defense += lpObj->pInventory[7].ItemDefense();

	if ( lpObj->pInventory[8].IsItem() != FALSE )
	{
		if ( lpObj->pInventory[8].m_Type == ITEMGET(13,4) && lpObj->pInventory[8].m_Durability > 0.0f )	// Dark Horse
		{
			lpObj->m_Defense += INT( Dexterity / 20 + 5 + Helper->m_PetItem_Level * 2 );
		}
	}

	if ( lpObj->pInventory[8].IsItem() && lpObj->pInventory[8].m_Type == ITEMGET(13,37) )
	{
		if( lpObj->pInventory[8].IsFenrirSpecial() )
		{
			int Level = lpObj->Level + lpObj->MasterLevel;

			lpObj->AddLife += Level / 2;
			lpObj->AddMana += Level / 2;

			lpObj->m_AttackDamageMinRight += Level / 12;
			lpObj->m_AttackDamageMaxRight += Level / 12;
			lpObj->m_AttackDamageMinLeft += Level / 12;
			lpObj->m_AttackDamageMaxLeft += Level / 12;
			lpObj->m_MagicDamageMax += Level / 25;
			lpObj->m_MagicDamageMin += Level / 25;
		}
	}

	if ( (Level15Count+Level14Count+Level13Count+Level12Count+Level11Count+Level10Count) >= 5 )
	{
		if ( Level15Count == 5 )
		{
			lpObj->m_Defense += lpObj->m_Defense * 30 / 100;
		}
		else if ( Level14Count == 5 || (Level14Count+Level15Count)== 5)
		{
			lpObj->m_Defense += lpObj->m_Defense * 25 / 100;
		}
		else if ( Level13Count == 5 || (Level13Count+Level14Count+Level15Count) == 5)
		{
			lpObj->m_Defense += lpObj->m_Defense * 20 / 100;
		}
		else if ( Level12Count == 5 || (Level12Count+Level13Count+Level14Count+Level15Count) == 5 )
		{
			lpObj->m_Defense += lpObj->m_Defense * 15 / 100;
		}
		else if ( Level11Count == 5 || (Level11Count+Level12Count+Level13Count+Level14Count+Level15Count) == 5 )
		{
			lpObj->m_Defense += lpObj->m_Defense * 10 / 100;
		}
		else if ( Level10Count == 5 || (Level10Count+Level11Count+Level12Count+Level13Count+Level14Count+Level15Count) == 5)
		{
			lpObj->m_Defense += lpObj->m_Defense * 5 / 100;
		}
	}

	if( Success == true )
	{
		lpObj->m_Defense += lpObj->m_MPSkillOpt.MpsArmorSetBonusIncrease;
	}

	lpObj->m_Defense += lpObj->m_MPSkillOpt.MpsDefence;
	lpObj->m_Defense = lpObj->m_Defense * 10 / 20;

	if ( lpObj->m_Change == 9 )
	{

	}
	else if ( lpObj->m_Change == 41 )
	{
		lpObj->AddLife = ((int)(lpObj->MaxLife * 20.0f))/100;
	}
	else if ( lpObj->m_Change == 372 )
	{
		lpObj->m_Defense += lpObj->m_Defense / 10;
		lpObj->AddLife += (lpObj->Level + lpObj->MasterLevel);
	}
	else if ( lpObj->m_Change == 374 )
	{
		lpObj->m_AttackDamageMaxLeft += 20;
		lpObj->m_AttackDamageMinLeft += 20;
		lpObj->m_AttackDamageMaxRight += 20;
		lpObj->m_AttackDamageMinRight += 20;
		lpObj->m_MagicDamageMin += 20;
		lpObj->m_MagicDamageMax += 20;
	}
	else if ( lpObj->m_Change == 503 )
	{
		lpObj->m_AttackDamageMaxLeft += 30;
		lpObj->m_AttackDamageMinLeft += 30;
		lpObj->m_AttackDamageMaxRight += 30;
		lpObj->m_AttackDamageMinRight += 30;
		lpObj->m_MagicDamageMin += 30;
		lpObj->m_MagicDamageMax += 30;
		lpObj->m_CurseDamageMin += 30;
		lpObj->m_CurseDamageMax += 30;
	}
	else if( lpObj->m_Change == 625 )
	{
		lpObj->m_Defense += 10;
		lpObj->m_AttackDamageMaxLeft += 30;
		lpObj->m_AttackDamageMinLeft += 30;
		lpObj->m_AttackDamageMaxRight += 30;
		lpObj->m_AttackDamageMinRight += 30;
		lpObj->m_MagicDamageMin += 30;
		lpObj->m_MagicDamageMax += 30;
		lpObj->m_CurseDamageMin += 30;
		lpObj->m_CurseDamageMax += 30;
	}
	else if( lpObj->m_Change == 626 )
	{
		lpObj->m_AttackSpeed += 7;
		lpObj->m_MagicSpeed += 7;
		lpObj->m_AttackDamageMaxLeft += 30;
		lpObj->m_AttackDamageMinLeft += 30;
		lpObj->m_AttackDamageMaxRight += 30;
		lpObj->m_AttackDamageMinRight += 30;
		lpObj->m_MagicDamageMin += 30;
		lpObj->m_MagicDamageMax += 30;
		lpObj->m_CurseDamageMin += 30;
		lpObj->m_CurseDamageMax += 30;
	}
	else if( lpObj->m_Change == 642 )
	{
		lpObj->m_AttackDamageMaxLeft += 40;
		lpObj->m_AttackDamageMinLeft += 40;
		lpObj->m_AttackDamageMaxRight += 40;
		lpObj->m_AttackDamageMinRight += 40;
		lpObj->m_MagicDamageMin += 40;
		lpObj->m_MagicDamageMax += 40;
		lpObj->m_CurseDamageMin += 40;
		lpObj->m_CurseDamageMax += 40;
	}

	if ( gObjDemonSprite(lpObj) == TRUE )
	{
		lpObj->m_AttackDamageMaxLeft += lpObj->m_AttackDamageMaxLeft*40/100;
		lpObj->m_AttackDamageMinLeft += lpObj->m_AttackDamageMinLeft*40/100;
		lpObj->m_AttackDamageMaxRight += lpObj->m_AttackDamageMaxRight*40/100;
		lpObj->m_AttackDamageMinRight += lpObj->m_AttackDamageMinRight*40/100;
		lpObj->m_MagicDamageMin += lpObj->m_MagicDamageMin*40/100;
		lpObj->m_MagicDamageMax += lpObj->m_MagicDamageMax*40/100;
		lpObj->m_CurseDamageMin += lpObj->m_CurseDamageMin*40/100;
		lpObj->m_CurseDamageMax += lpObj->m_CurseDamageMax*40/100;
		lpObj->m_AttackSpeed += 10;
		lpObj->m_MagicSpeed += 10;
	}
	else if ( gObjGuardianSprice(lpObj) == TRUE )
	{
		lpObj->AddLife += 50;
	}
	else if( gObjPandaSprite(lpObj) == TRUE )
	{
		lpObj->m_Defense += 50;
	}
	else if( gObjUnicornSprite(lpObj) == TRUE )
	{
		lpObj->m_Defense += 50;
	}
	else if ( lpObj->pInventory[8].m_Type == ITEMGET(13,0) )
	{
		lpObj->AddLife += 50;
	}

	int addlife = 0;
	int addmana = 0;

	lpObj->pInventory[7].PlusSpecial(&addlife, 100);
	lpObj->pInventory[7].PlusSpecial(&addmana, 101);
	lpObj->pInventory[7].PlusSpecial(&AddLeadership, 105);

#if (CUSTOM_WINGS == 1)
	lpObj->pInventory[7].PlusSpecial(&addlife, 200);
#endif

	lpObj->AddLeadership += AddLeadership;
	lpObj->AddLife += addlife;
	lpObj->AddMana += addmana;

	if ( lpObj->pInventory[8].m_Type == ITEMGET(13,3) )
	{
		lpObj->pInventory[8].PlusSpecial(&lpObj->AddBP, 103 );
	}

	if ( lpObj->Type == OBJ_USER )
	{
		gDarkSpirit[lpObj->m_Index].Set(lpObj->m_Index, &lpObj->pInventory[1]);
	}

#if (ENABLETEST_MUUN == 1)
	int nMuunItemEffectValue = 0;
	if ( g_CMuunSystem.GetMuunItemValueOfOptType(lpObj, 1, &nMuunItemEffectValue, 0)) {
		lpObj->m_AttackDamageMaxLeft += nMuunItemEffectValue;
		lpObj->m_AttackDamageMinLeft += nMuunItemEffectValue;
		lpObj->m_AttackDamageMaxRight += nMuunItemEffectValue;
		lpObj->m_AttackDamageMinRight += nMuunItemEffectValue;
		lpObj->m_MagicDamageMin += nMuunItemEffectValue;
		lpObj->m_MagicDamageMax += nMuunItemEffectValue;
		lpObj->m_CurseDamageMin += nMuunItemEffectValue;
		lpObj->m_CurseDamageMax += nMuunItemEffectValue;
	}
	if (g_CMuunSystem.GetMuunItemValueOfOptType(lpObj, 7, &nMuunItemEffectValue, 0)) {
		lpObj->m_AttackDamageMinLeft += nMuunItemEffectValue;
		lpObj->m_AttackDamageMinRight += nMuunItemEffectValue;
		lpObj->m_AttackDamageMaxLeft += nMuunItemEffectValue;
		lpObj->m_AttackDamageMaxRight += nMuunItemEffectValue;
		lpObj->m_MagicDamageMin += nMuunItemEffectValue;
		lpObj->m_MagicDamageMax += nMuunItemEffectValue;
		lpObj->m_CurseDamageMin += nMuunItemEffectValue;
		lpObj->m_CurseDamageMax += nMuunItemEffectValue;
	}
#endif

	lpObj->pInventory[9].PlusSpecialPercentEx(&lpObj->AddBP, lpObj->MaxBP, 173);
	lpObj->pInventory[10].PlusSpecialPercentEx(&lpObj->AddMana, (int)lpObj->MaxMana, 172);
	lpObj->pInventory[11].PlusSpecialPercentEx(&lpObj->AddMana, (int)lpObj->MaxMana, 172);

	if( lpObj->Class == CLASS_SUMMONER )
	{
		lpObj->m_CurseDamageBook = lpObj->pInventory[1].ItemCurseDamage();
	}

	CItem * rItem[3];
	int comparecount = 0;
	
	rItem[0] = &lpObj->pInventory[10];
	rItem[1] = &lpObj->pInventory[11];
	rItem[2] = &lpObj->pInventory[9];

#define GET_MAX_RESISTANCE(x,y,z) ( ( ( ( (x) > (y) ) ? (x) : (y) ) > (z) ) ? ( ( (x) > (y) ) ? (x) : (y) ) : (z) )	

	lpObj->m_Resistance[1] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[1], rItem[1]->m_Resistance[1], rItem[2]->m_Resistance[1]);
	lpObj->m_Resistance[0] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[0], rItem[1]->m_Resistance[0], rItem[2]->m_Resistance[0]);
	lpObj->m_Resistance[2] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[2], rItem[1]->m_Resistance[2], rItem[2]->m_Resistance[2]);
	lpObj->m_Resistance[3] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[3], rItem[1]->m_Resistance[3], rItem[2]->m_Resistance[3]);
	lpObj->m_Resistance[4] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[4], rItem[1]->m_Resistance[4], rItem[2]->m_Resistance[4]);
	lpObj->m_Resistance[5] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[5], rItem[1]->m_Resistance[5], rItem[2]->m_Resistance[5]);
	lpObj->m_Resistance[6] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[6], rItem[1]->m_Resistance[6], rItem[2]->m_Resistance[6]);

	lpObj->m_Resistance[R_POISON] += lpObj->m_MPSkillOpt.MpsResistancePoison;
	lpObj->m_Resistance[R_LIGHTNING] += lpObj->m_MPSkillOpt.MpsResistanceThunder;
	lpObj->m_Resistance[R_ICE] += lpObj->m_MPSkillOpt.MpsResistanceIce;

	g_kItemSystemFor380.ApplyFor380Option(lpObj);
	g_kJewelOfHarmonySystem.SetApplyStrengthenItem(lpObj); 
	g_SocketItem.SetApplySocketItem(lpObj);
	g_ElementalSystem.ApplyElementItem(lpObj);
	gObjInventoryEquipment(lpObj);

	lpObj->AddLife += lpObj->m_MPSkillOpt.MpsMaxHP;
	lpObj->AddBP += lpObj->m_MPSkillOpt.MpsMaxBP;

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_SWELL_LIFE3) == TRUE )
	{
		lpObj->AddBP += lpObj->m_MPSkillOpt.MpsSwellLifeAddBP;
	}

	lpObj->iAddShield += lpObj->m_MPSkillOpt.MpsMaxSD;
	lpObj->AddMana += lpObj->m_MPSkillOpt.MpsMaxMP;
	lpObj->AddMana += lpObj->MaxMana/100*lpObj->m_MPSkillOpt.MpsMaxMPMul;

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_MANASHIELD2) == TRUE )
	{
		lpObj->AddMana += lpObj->m_MPSkillOpt.MpsManaShieldAddMana;
	}

	GObjExtItemApply(lpObj);
	gObjSetItemApply(lpObj);

#if (CUSTOM_EXOPT == 1)
	gNewOptions.SetEnchantOptions(lpObj);
#endif

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_BERSERKER4) )
	{
		lpObj->m_CurseDamageMin += lpObj->m_MPSkillOpt.MpsBerserkInc3;
		lpObj->m_CurseDamageMax += lpObj->m_MPSkillOpt.MpsBerserkInc3;
		lpObj->m_AttackDamageMin += lpObj->m_MPSkillOpt.MpsBerserkInc3;
		lpObj->m_AttackDamageMax += lpObj->m_MPSkillOpt.MpsBerserkInc3;
		lpObj->m_MagicDamageMin += lpObj->m_MPSkillOpt.MpsBerserkInc3;
		lpObj->m_MagicDamageMax += lpObj->m_MPSkillOpt.MpsBerserkInc3;
	}

	gObjNextExpCal(lpObj);

	if ( (Left->m_Type >= ITEMGET(4,0) && Left->m_Type < ITEMGET(4,7) ) ||
		  Left->m_Type == ITEMGET(4,17) ||
		  Left->m_Type == ITEMGET(4,20) ||
		  Left->m_Type == ITEMGET(4,21) ||
		  Left->m_Type == ITEMGET(4,22) ||
		  Left->m_Type == ITEMGET(4,23) ||
		  Left->m_Type == ITEMGET(4,24) )
	{
		if ( Right->m_Type == ITEMGET(4,15) && Right->m_Level == 1 )
		{
			lpObj->m_AttackDamageMinLeft += (WORD)(lpObj->m_AttackDamageMinLeft * 0.03f + 1.0f);
			lpObj->m_AttackDamageMaxLeft += (WORD)(lpObj->m_AttackDamageMaxLeft * 0.03f + 1.0f);
		}
		else if ( Right->m_Type == ITEMGET(4,15) && Right->m_Level == 2 )
		{
			lpObj->m_AttackDamageMinLeft += (WORD)(lpObj->m_AttackDamageMinLeft * 0.05f + 1.0f);
			lpObj->m_AttackDamageMaxLeft += (WORD)(lpObj->m_AttackDamageMaxLeft * 0.05f + 1.0f);
		}
		else if ( Right->m_Type == ITEMGET(4,15) && Right->m_Level == 3 )
		{
			lpObj->m_AttackDamageMinLeft += (WORD)(lpObj->m_AttackDamageMinLeft * 0.1f + 1.0f);
			lpObj->m_AttackDamageMaxLeft += (WORD)(lpObj->m_AttackDamageMaxLeft * 0.1f + 1.0f);
		}
	}
	else if ( (Right->m_Type >= ITEMGET(4,8) && Right->m_Type < ITEMGET(4,15) ) ||
		      (Right->m_Type >= ITEMGET(4,16) && Right->m_Type < ITEMGET(5,0)) )
	{
		if ( Left->m_Type == ITEMGET(4,7) && Left->m_Level == 1 )
		{
				lpObj->m_AttackDamageMinRight += (WORD)(lpObj->m_AttackDamageMinRight * 0.03f + 1.0f);
				lpObj->m_AttackDamageMaxRight += (WORD)(lpObj->m_AttackDamageMaxRight * 0.03f + 1.0f);
		}
		else if ( Left->m_Type == ITEMGET(4,7) && Left->m_Level == 2 )
		{
				lpObj->m_AttackDamageMinRight += (WORD)(lpObj->m_AttackDamageMinRight * 0.05f + 1.0f);
				lpObj->m_AttackDamageMaxRight += (WORD)(lpObj->m_AttackDamageMaxRight * 0.05f + 1.0f);
		}
		else if ( Left->m_Type == ITEMGET(4,7) && Left->m_Level == 3 )
		{
				lpObj->m_AttackDamageMinRight += (WORD)(lpObj->m_AttackDamageMinRight * 0.1f + 1.0f);
				lpObj->m_AttackDamageMaxRight += (WORD)(lpObj->m_AttackDamageMaxRight * 0.1f + 1.0f);
		}
	}

	if ( lpObj->Class == CLASS_KNIGHT || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_FIGHTER  )
	{
		if ( Right->m_Type != -1 && Left->m_Type != -1 )
		{
			if ( Right->m_Type >= ITEMGET(0,0) && Right->m_Type < ITEMGET(4,0) && Left->m_Type >= ITEMGET(0,0) && Left->m_Type < ITEMGET(4,0) )
			{
				lpObj->m_AttackDamageMinRight = lpObj->m_AttackDamageMinRight * 55 / 100;
				lpObj->m_AttackDamageMaxRight = lpObj->m_AttackDamageMaxRight * 55 / 100;
				lpObj->m_AttackDamageMinLeft = lpObj->m_AttackDamageMinLeft * 55 / 100;
				lpObj->m_AttackDamageMaxLeft = lpObj->m_AttackDamageMaxLeft * 55 / 100;
			}
		}
	}

#if (CUSTOM_OLYMP==1)
	if( gObjCheckUsedBuffEffect(lpObj,BUFF_GLADIATOR_HONOR) )
	{
		lpObj->SetOpDoubleDamage += 10;
		lpObj->SetOpTripleDamage += 5;
	}
#endif

	g_ItemAddOption.SetPeriodItemEffect2(lpObj);
	g_BuffEffect.SetNextEffect(lpObj);

	if ( lpObj->m_iItemEffectValidTime > 0 )
	{
		g_ItemAddOption.NextSetItemLastEffectForHallowin(lpObj);
	}

	gObjCalcShieldPoint(lpObj);
	gObjCalcMLSkillItemOption(lpObj);

	if( lpObj->iShield > (lpObj->iMaxShield+lpObj->iAddShield) )
	{
		lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
	}

#if(__NOVUS__ == 1)
	g_StatGem.SimpleEffectAdd(lpObj);
#endif

#if( __4GAMERS__ == 1 )
	g_ExtraClassBalance.Apply(lpObj);
#endif

	GCReFillSend(lpObj->m_Index, (WORD)lpObj->Life, 0xFF, 0, lpObj->iShield);
	GCReFillSend(lpObj->m_Index, (WORD)(lpObj->MaxLife + lpObj->AddLife), 0xFE, 0, lpObj->iMaxShield + lpObj->iAddShield);
	GCManaSend(lpObj->m_Index, (WORD)(lpObj->MaxMana + lpObj->AddMana), 0xFE, 0, lpObj->MaxBP + lpObj->AddBP);
	GCAttackSpeedSend(lpObj->m_Index);
}








void GObjExtItemApply(LPOBJ lpObj)
{

	int n;
	int i;
	int max_count = 0;
	int temp_n[3] = {0, 1, 9};
	int temp_n2[10] = {2, 3, 4, 5, 6, 7, 8, 10, 11, 1};
	int maxn2_loop = 9;

	for ( i=0;i<3;i++ )
	{
		n = temp_n[i];

		if ( lpObj->pInventory[n].m_Type >= ITEMGET(6, 0) && lpObj->pInventory[n].m_Type < ITEMGET(7, 0) )
			continue;

		if ( lpObj->pInventory[n].IsItem() == TRUE )
		{
			if ( lpObj->pInventory[n].m_Durability != 0.0f )
			{
				if ( lpObj->pInventory[n].m_IsValidItem == true )
				{
					if ( lpObj->pInventory[n].IsExtAttackRate() )
					{
						if ( ( lpObj->pInventory[n].m_Type >= ITEMGET(5, 0) && lpObj->pInventory[n].m_Type < ITEMGET(6, 0) ) ||
							lpObj->pInventory[n].m_Type == ITEMGET(13,12) ||
							lpObj->pInventory[n].m_Type == ITEMGET(13,25) ||
							lpObj->pInventory[n].m_Type == ITEMGET(13,27) )
						{
							lpObj->m_MagicDamageMax += (lpObj->Level + lpObj->MasterLevel) / lpObj->pInventory[n].IsExtAttackRate();
							lpObj->m_MagicDamageMin += (lpObj->Level + lpObj->MasterLevel) / lpObj->pInventory[n].IsExtAttackRate();
						}
						else 
						{
							if ( n == 0 || n == 9 )
							{
								lpObj->m_AttackDamageMinRight += (lpObj->Level + lpObj->MasterLevel) / lpObj->pInventory[n].IsExtAttackRate();
								lpObj->m_AttackDamageMaxRight += (lpObj->Level + lpObj->MasterLevel) / lpObj->pInventory[n].IsExtAttackRate();
							}
							
							if ( n == 1 || n == 9 )
							{
								lpObj->m_AttackDamageMinLeft += (lpObj->Level + lpObj->MasterLevel) / lpObj->pInventory[n].IsExtAttackRate();
								lpObj->m_AttackDamageMaxLeft += (lpObj->Level + lpObj->MasterLevel) / lpObj->pInventory[n].IsExtAttackRate();
							}
						}
					}

					if ( lpObj->pInventory[n].IsExtAttackRate2() )
					{
						if ( ( lpObj->pInventory[n].m_Type >= ITEMGET(5, 0) && lpObj->pInventory[n].m_Type < ITEMGET(6, 0) ) ||
							lpObj->pInventory[n].m_Type == ITEMGET(13,12) ||
							lpObj->pInventory[n].m_Type == ITEMGET(13,25) ||
							lpObj->pInventory[n].m_Type == ITEMGET(13,27) )
						{
							lpObj->m_MagicDamageMax += lpObj->m_MagicDamageMax * lpObj->pInventory[n].IsExtAttackRate2() / 100;
							lpObj->m_MagicDamageMin += lpObj->m_MagicDamageMin * lpObj->pInventory[n].IsExtAttackRate2() / 100;
						}
						else 
						{
							if ( n == 0 || n == 9 )
							{
								lpObj->m_AttackDamageMinRight += lpObj->m_AttackDamageMinRight * lpObj->pInventory[n].IsExtAttackRate2() / 100;
								lpObj->m_AttackDamageMaxRight += lpObj->m_AttackDamageMaxRight * lpObj->pInventory[n].IsExtAttackRate2() / 100;
							}
							
							if ( n == 1 || n == 9 )
							{
								lpObj->m_AttackDamageMinLeft += lpObj->m_AttackDamageMinLeft * lpObj->pInventory[n].IsExtAttackRate2() / 100;
								lpObj->m_AttackDamageMaxLeft += lpObj->m_AttackDamageMaxLeft * lpObj->pInventory[n].IsExtAttackRate2() / 100;
							}
						}
					}

					lpObj->m_ExcelentDamage += lpObj->pInventory[n].IsExtExcellentDamage();
					lpObj->m_AttackSpeed += lpObj->pInventory[n].IsExtAttackSpeed();
					lpObj->m_MagicSpeed += lpObj->pInventory[n].IsExtAttackSpeed();
					lpObj->MonsterDieGetLife += lpObj->pInventory[n].IsExtMonsterDieLife();
					lpObj->MonsterDieGetMana += lpObj->pInventory[n].IsExtMonsterDieMana();
				}
			}
		}
	}
		
	if ( lpObj->pInventory[1].IsItem() == TRUE )
	{
		if ( lpObj->pInventory[1].m_Type >= ITEMGET(6, 0) && lpObj->pInventory[1].m_Type < ITEMGET(7, 0) )
		{
			maxn2_loop = 10;
		}
	}

	for ( i=0;i<maxn2_loop;i++)
	{
		n = temp_n2[i];

		if ( max_count < 8 )
		{
			if ( lpObj->pInventory[n].IsItem() == TRUE &&
				 lpObj->pInventory[n].IsExtItem() &&
				 lpObj->pInventory[n].m_Durability != 0.0f &&
				 lpObj->pInventory[n].m_IsValidItem == true )
			{
				if ( n != 7 )	// #warning this will unselect ONLY wings
				{
					lpObj->AddLife += (short)(lpObj->MaxLife * lpObj->pInventory[n].IsExtLifeAdd()  / 100.0f);	// #stat
					lpObj->AddMana += (short)(lpObj->MaxMana * lpObj->pInventory[n].IsExtManaAdd() / 100.0f);	// #stat
					lpObj->m_SuccessfulBlocking += lpObj->m_SuccessfulBlocking * lpObj->pInventory[n].IsExtDefenseSuccessfull() / 100;
					lpObj->MonsterDieGetMoney += lpObj->pInventory[n].IsExtMonsterMoney();
					lpObj->DamageMinus += lpObj->pInventory[n].IsExtDamageMinus();
					lpObj->DamageReflect += lpObj->pInventory[n].IsExtDamageReflect();
					max_count++;
				}
			}
		}
	}
}



void gObjCalcSetItemStat(LPOBJ lpObj)
{
	for ( int i=0;i<INVETORY_WEAR_SIZE;i++)
	{
		if ( lpObj->pInventory[i].IsSetItem() && lpObj->pInventory[i].m_IsValidItem && lpObj->pInventory[i].m_Durability != 0.0f )
		{
			int AddStatType = lpObj->pInventory[i].GetAddStatType();

			switch ( AddStatType )
			{
				case 1:
					lpObj->pInventory[i].SetItemPlusSpecialStat(&lpObj->AddStrength, 196);
					break;
				case 2:
					lpObj->pInventory[i].SetItemPlusSpecialStat(&lpObj->AddDexterity, 197);
					break;
				case 3:
					lpObj->pInventory[i].SetItemPlusSpecialStat(&lpObj->AddEnergy, 198);
					break;
				case 4:
					lpObj->pInventory[i].SetItemPlusSpecialStat(&lpObj->AddVitality, 199);
					break;
			}
		}
	}
}





void gObjGetSetItemOption(LPOBJ lpObj, LPBYTE pSetOptionTable , LPBYTE pSetOptionCountTable , int * pSetOptionCount)
{
	*pSetOptionCount = 0;
	int RightSetOptionIndex = -1;
	int RightRingSetOptionIndex = -1;

	for ( int i=0;i<INVETORY_WEAR_SIZE;i++)
	{
		int iSetItemType = lpObj->pInventory[i].IsSetItem();

		if ( iSetItemType )
		{
			if ( lpObj->pInventory[i].m_IsValidItem )
			{
				int iSetItemOption = gSetItemOption.GetOptionTableIndex(lpObj->pInventory[i].m_Type, iSetItemType );

				if ( iSetItemOption != -1 )
				{
					if ( i == 0 )
					{
						RightSetOptionIndex = iSetItemOption;
					}
					else if ( i == 1  )
					{
						if (  RightSetOptionIndex == iSetItemOption )
						{
							if ( lpObj->pInventory[0].GetWeaponType() == lpObj->pInventory[1].GetWeaponType() && lpObj->pInventory[0].IsSetItem() )
							{
								LogAddTD("양손에 같은 세트가 적용되었다 무시...");
								continue;
							}
						}
					}
					
					if ( i == 10 )
					{
						RightRingSetOptionIndex = iSetItemOption;
					}
					else if ( i == 11   )
					{
						if ( lpObj->pInventory[10].m_Type == lpObj->pInventory[11].m_Type && lpObj->pInventory[10].IsSetItem() )
						{
							LogAddTD("반지에 같은 세트가 적용되었다 무시...");
							continue;
						}
					}

					BOOL bFound = FALSE;

					for ( int n=0;n<*pSetOptionCount;n++)
					{
						if ( pSetOptionTable[n] == iSetItemOption )
						{
							pSetOptionCountTable[n]++;
							bFound = TRUE;
							break;
						}
					}

					if ( bFound == FALSE )
					{
						pSetOptionTable[*pSetOptionCount] = iSetItemOption;
						pSetOptionCountTable[*pSetOptionCount]++;
						*pSetOptionCount += 1;
					}
				}
			}
		}
	}
}
void gObjCalcSetItemOption(LPOBJ lpObj)
{
	BYTE SetOptionTable[29];
	BYTE SetOptionCountTable[29];
	int SetOptionCount = 0;
	int op1;
	int op2;
	int op3;
	int op4;
	int op5;
	int opvalue1;
	int opvalue2;
	int opvalue3;
	int opvalue4;
	int opvalue5;

	memset(SetOptionTable, 0, sizeof(SetOptionTable));
	memset(SetOptionCountTable, 0, sizeof(SetOptionCountTable));
	gObjGetSetItemOption(lpObj, SetOptionTable, SetOptionCountTable, &SetOptionCount);

	int optioncount;
	for ( optioncount=0;optioncount<SetOptionCount;optioncount++)
	{
		int OptionTableIndex = SetOptionTable[optioncount];
		int OptionTableCount = SetOptionCountTable[optioncount];

		if ( OptionTableCount >= 2 )
		{
			gSetItemOption.IsRequireClass(OptionTableIndex, lpObj->Class, lpObj->ChangeUP);

			for ( int tablecnt=0;tablecnt<(OptionTableCount-1);tablecnt++)
			{
				if ( gSetItemOption.GetSetOption(OptionTableIndex, tablecnt,
					op1, op2, opvalue1, opvalue2, lpObj->Class, lpObj->ChangeUP) )
				{
					gObjSetItemStatPlusSpecial(lpObj, op1, opvalue1);
					gObjSetItemStatPlusSpecial(lpObj, op2, opvalue2);
				}
			}

			if ( gSetItemOption.GetMaxSetOptionCount(OptionTableIndex) < OptionTableCount )
			{
				lpObj->IsFullSetItem = true;
				LogAddC(2, "풀옵션 적용");

				gSetItemOption.GetGetFullSetOption(OptionTableIndex,
					op1, op2, op3, op4, op5,
					opvalue1, opvalue2, opvalue3, opvalue4, opvalue5,
					lpObj->Class, lpObj->ChangeUP);

				gObjSetItemStatPlusSpecial(lpObj, op1, opvalue1);
				gObjSetItemStatPlusSpecial(lpObj, op2, opvalue2);
				gObjSetItemStatPlusSpecial(lpObj, op3, opvalue3);
				gObjSetItemStatPlusSpecial(lpObj, op4, opvalue4);
				gObjSetItemStatPlusSpecial(lpObj, op5, opvalue5);
			}		
		}
	}

	for ( optioncount=0;optioncount<SetOptionCount;optioncount++)
	{
		int OptionTableIndex = SetOptionTable[optioncount];
		int OptionTableCount = SetOptionCountTable[optioncount];

		if ( OptionTableCount >= 2 )
		{
			LogAdd("[%s][%s] Ancient Set: %s",
				lpObj->AccountID, lpObj->Name, gSetItemOption.GetSetOptionName(OptionTableIndex));

			for ( int tablecnt=0;tablecnt<(OptionTableCount-1);tablecnt++)
			{
				if ( gSetItemOption.GetSetOption(OptionTableIndex, tablecnt,
					op1, op2, opvalue1, opvalue2, lpObj->Class, lpObj->ChangeUP) )
				{
					gObjSetItemPlusSpecial(lpObj, op1, opvalue1);
					gObjSetItemPlusSpecial(lpObj, op2, opvalue2);
				}
			}
				
			if ( gSetItemOption.GetMaxSetOptionCount(OptionTableIndex) < OptionTableCount )
			{
				LogAddC(2, "Full Set Item");
				lpObj->IsFullSetItem = true;

				gSetItemOption.GetGetFullSetOption(OptionTableIndex,
					op1, op2, op3, op4, op5,
					opvalue1, opvalue2, opvalue3, opvalue4, opvalue5,
					lpObj->Class, lpObj->ChangeUP);

				gObjSetItemPlusSpecial(lpObj, op1, opvalue1);
				gObjSetItemPlusSpecial(lpObj, op2, opvalue2);
				gObjSetItemPlusSpecial(lpObj, op3, opvalue3);
				gObjSetItemPlusSpecial(lpObj, op4, opvalue4);
				gObjSetItemPlusSpecial(lpObj, op5, opvalue5);
			}		
		}
	}
}




void gObjSetItemStatPlusSpecial(LPOBJ lpObj, int option, int ivalue)
{
	if ( option == -1 )
		return;

	switch ( option )	// #warning Add AT_SET_OPTION_IMPROVE_LEADERSHIP
	{
		case AT_SET_OPTION_IMPROVE_STRENGTH:
			lpObj->AddStrength += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_DEXTERITY:
			lpObj->AddDexterity += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_ENERGY:
			lpObj->AddEnergy += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_VITALITY:
			lpObj->AddVitality += ivalue;
			break;
	}
}



void gObjSetItemPlusSpecial(LPOBJ lpObj, int option, int ivalue)
{
	if ( option == -1 )
		return;

	switch ( option )
	{
		case AT_SET_OPTION_DOUBLE_DAMAGE:
			lpObj->SetOpDoubleDamage += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_SHIELD_DEFENCE:
			lpObj->SetOpImproveSheldDefence += ivalue;
			break;
		case AT_SET_OPTION_TWO_HAND_SWORD_IMPROVE_DAMAGE:
			lpObj->SetOpTwoHandSwordImproveDamage += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_MINATTACKDAMAGE:
			lpObj->SetOpAddMinAttackDamage += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_MAXATTACKDAMAGE:
			lpObj->SetOpAddMaxAttackDamage += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_MAGICDAMAGE:
			lpObj->SetOpAddMagicPower += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_DAMAGE:
			lpObj->SetOpAddDamage += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_ATTACKRATE:
			lpObj->SetOpImproveSuccessAttackRate += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_DEFENCE:
			lpObj->SetOpAddDefence += ivalue;
			break;
		case AT_SET_OPTION_DEFENCE_IGNORE:
			lpObj->SetOpIgnoreDefense += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_MAXLIFE:
			lpObj->AddLife += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_MAXMANA:
			lpObj->AddMana += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_MAXAG:
			lpObj->AddBP += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_AG_VALUE:
			lpObj->SetOpIncAGValue += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_CRITICALDAMAGE:
			lpObj->SetOpAddCriticalDamage += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_CRITICALDAMAGE_SUCCESS:
			lpObj->SetOpAddCriticalDamageSuccessRate += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_EX_DAMAGE:
			lpObj->SetOpAddExDamage += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_EX_DAMAGE_SUCCESS:
			lpObj->SetOpAddExDamageSuccessRate += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_SKILLDAMAGE:
			lpObj->SetOpAddSkillAttack += ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_ATTACKDAMAGE_WITH_DEX:
			lpObj->SetOpAddAttackDamage += ( lpObj->Dexterity + lpObj->AddDexterity ) / ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_ATTACKDAMAGE_WITH_STR:
			lpObj->SetOpAddAttackDamage += ( lpObj->Strength + lpObj->AddStrength ) / ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_MAGICDAMAGE_WITH_ENERGY:
			lpObj->SetOpAddMagicPower += ( lpObj->Energy + lpObj->AddEnergy ) / ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_DEFENCE_WITH_DEX:
			lpObj->SetOpAddDefence += ( lpObj->Dexterity + lpObj->AddDexterity )  / ivalue;
			break;
		case AT_SET_OPTION_IMPROVE_DEFENCE_WITH_VIT:
			lpObj->SetOpAddDefence += ( lpObj->Vitality + lpObj->AddVitality ) / ivalue;
			break;
		case AT_SET_OPTION_FIRE_MASTERY:
			lpObj->m_AddResistance[R_FIRE] += ivalue;
			break;
		case AT_SET_OPTION_ICE_MASTERY:
			lpObj->m_AddResistance[R_ICE] += ivalue;
			break;
		case AT_SET_OPTION_THUNDER_MASTERY:
			lpObj->m_AddResistance[R_LIGHTNING] += ivalue;
			break;
		case AT_SET_OPTION_POSION_MASTERY:
			lpObj->m_AddResistance[R_POISON] += ivalue;
			break;
		case AT_SET_OPTION_WATER_MASTERY:
			lpObj->m_AddResistance[R_WATER] += ivalue;
			break;
		case AT_SET_OPTION_WIND_MASTERY:
			lpObj->m_AddResistance[R_WIND] += ivalue;
			break;
		case AT_SET_OPTION_EARTH_MASTERY:
			lpObj->m_AddResistance[R_EARTH] += ivalue;
			break;
	}
}




void gObjSetItemApply(LPOBJ lpObj)
{
	lpObj->m_AttackDamageMinLeft += lpObj->SetOpAddAttackDamage;
	lpObj->m_AttackDamageMaxLeft += lpObj->SetOpAddAttackDamage;
	lpObj->m_AttackDamageMinRight += lpObj->SetOpAddAttackDamage;
	lpObj->m_AttackDamageMaxRight += lpObj->SetOpAddAttackDamage;

	lpObj->m_AttackDamageMinLeft += lpObj->SetOpAddMinAttackDamage;
	lpObj->m_AttackDamageMinRight += lpObj->SetOpAddMinAttackDamage;

	lpObj->m_AttackDamageMaxLeft += lpObj->SetOpAddMaxAttackDamage;
	lpObj->m_AttackDamageMaxRight += lpObj->SetOpAddMaxAttackDamage;

	lpObj->m_MagicDamageMin += lpObj->m_MagicDamageMin * lpObj->SetOpAddMagicPower / 100;
	lpObj->m_MagicDamageMax += lpObj->m_MagicDamageMax * lpObj->SetOpAddMagicPower / 100;

	lpObj->AddLife += INT(lpObj->AddVitality * DCInfo.DefClass[lpObj->Class].VitalityToLife);
	lpObj->AddMana += INT(lpObj->AddEnergy * DCInfo.DefClass[lpObj->Class].EnergyToMana);

	lpObj->m_CriticalDamage += lpObj->SetOpAddCriticalDamageSuccessRate;
	lpObj->m_ExcelentDamage += lpObj->SetOpAddExDamageSuccessRate;

	if ( lpObj->pInventory[10].IsSetItem() )
	{
		lpObj->pInventory[10].PlusSpecialSetRing((LPBYTE)lpObj->m_AddResistance);
	}

	if ( lpObj->pInventory[11].IsSetItem() )
	{
		lpObj->pInventory[11].PlusSpecialSetRing((LPBYTE)lpObj->m_AddResistance);
	}

	if ( lpObj->pInventory[9].IsSetItem() )
	{
		lpObj->pInventory[9].PlusSpecialSetRing((LPBYTE)lpObj->m_AddResistance);
	}

	if ( lpObj->SetOpTwoHandSwordImproveDamage )
	{
		if ( !lpObj->pInventory[1].IsItem() && !lpObj->pInventory[0].IsItem() )
		{
			lpObj->SetOpTwoHandSwordImproveDamage = 0;
		}

		if ( lpObj->pInventory[1].IsItem() && lpObj->pInventory[1].m_TwoHand == FALSE )
		{
			lpObj->SetOpTwoHandSwordImproveDamage = 0;
		}

		if ( lpObj->pInventory[0].IsItem() && lpObj->pInventory[0].m_TwoHand == FALSE )
		{
			lpObj->SetOpTwoHandSwordImproveDamage = 0;
		}
	}

	if ( (lpObj->MaxLife + lpObj->AddLife ) < lpObj->Life )
	{
		lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
		GCReFillSend(lpObj->m_Index, (WORD)lpObj->Life, 0xFF, 0, lpObj->iShield);
	}

	gObjSetBP(lpObj->m_Index);

	if ( (lpObj->MaxMana + lpObj->AddMana ) < lpObj->Mana )
	{
		lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
		GCManaSend(lpObj->m_Index, (WORD)lpObj->Mana, 0xFF, 0, lpObj->BP);
	}

	lpObj->m_Defense += lpObj->SetOpAddDefence * 10 / 20;
	lpObj->m_Defense += lpObj->m_Defense * lpObj->SetOpAddDefenceRate / 100;

	if (lpObj->pInventory[1].m_Type >= ITEMGET(6, 0) && lpObj->pInventory[1].m_Type < ITEMGET(7, 0))
	{
		lpObj->m_Defense += lpObj->m_Defense * lpObj->SetOpImproveSheldDefence / 100;
	}
}





BOOL gObjValidItem(LPOBJ lpObj, CItem * lpItem, int pos)
{
	LPITEM_ATTRIBUTE p = &ItemAttribute[lpItem->m_Type];
	
	if ( p->RequireStrength != 0 )
	{
		if ( (lpObj->Strength + lpObj->AddStrength) < ( lpItem->m_RequireStrength - lpItem->m_HJOpStrength ) )
		{
			return FALSE;
		}
	}

	if ( p->RequireDexterity != 0 )
	{
		if ( (lpObj->Dexterity + lpObj->AddDexterity) < ( lpItem->m_RequireDexterity - lpItem->m_HJOpDexterity ) )
		{
			return FALSE;
		}
	}


	if ( p->RequireEnergy != 0 )
	{
		if ( (lpObj->Energy + lpObj->AddEnergy) < lpItem->m_RequireEnergy )
		{
			return FALSE;
		}
	}

	if ( p->RequireLevel != 0 )
	{
		if ( (lpObj->Level) < lpItem->m_RequireLevel )
		{
			return FALSE;
		}
	}

	if ( lpItem->m_Type >= ITEMGET(0,0) && lpItem->m_Type <= ITEMGET(11, 0) )
	{
		if ( p->RequireVitality )
		{
			if ( (lpObj->Vitality + lpObj->AddVitality ) < lpItem->m_RequireVitality )
			{
				return FALSE;
			}
		}

		if ( p->RequireLeadership )
		{
			if ( (lpObj->Leadership + lpObj->AddLeadership ) < lpItem->m_RequireLeaderShip )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
	


void gObjCalcShieldPoint(LPOBJ lpObj)
{
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("setShieldPool", lpObj->m_Index);
	ZTLuaManager::Delete(LuaMng);
#else
	int iMaxShieldPoint = 0;
	int iExpressionA = ( lpObj->Strength + lpObj->AddStrength ) + ( lpObj->Dexterity + lpObj->AddDexterity ) + ( lpObj->Vitality + lpObj->AddVitality ) + (lpObj->Energy + lpObj->AddEnergy );

	if ( lpObj->Class == CLASS_DARKLORD )
	{
		iExpressionA += lpObj->Leadership + lpObj->AddLeadership;
	}

	if ( g_iShieldGageConstB == 0 )
		g_iShieldGageConstB = 30;

	int iExpressionB = ( lpObj->Level * lpObj->Level) / g_iShieldGageConstB;

	if ( g_iShieldGageConstA == 0 )
		g_iShieldGageConstA = 12;

	iMaxShieldPoint = ( iExpressionA * g_iShieldGageConstA ) / 10 + iExpressionB  + lpObj->m_Defense;
	lpObj->iMaxShield = iMaxShieldPoint;
#endif
}

void gObjCalcMLSkillItemOption(LPOBJ lpObj)
{
	if( lpObj->m_MPSkillOpt.MpsTwoHandSwordStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 0 &&
			lpObj->pInventory[1].GetDetailItemType() != 0 )
		{
			lpObj->m_MPSkillOpt.MpsTwoHandSwordStrength = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsTwoHandSwordMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 0 &&
			lpObj->pInventory[1].GetDetailItemType() != 0 )
		{
			lpObj->m_MPSkillOpt.MpsTwoHandSwordMastery = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsTwoHandStaffStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 5 ||
			lpObj->pInventory[1].GetDetailItemType() == 5 )
		{
			lpObj->m_MagicDamageMin += lpObj->m_MPSkillOpt.MpsTwoHandStaffStrength;
			lpObj->m_MagicDamageMax += lpObj->m_MPSkillOpt.MpsTwoHandStaffStrength;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsOneHandStaffStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 4 ||
			lpObj->pInventory[1].GetDetailItemType() == 4 )
		{
			lpObj->m_MagicDamageMin += lpObj->m_MPSkillOpt.MpsOneHandStaffStrength;
			lpObj->m_MagicDamageMax += lpObj->m_MPSkillOpt.MpsOneHandStaffStrength;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsStickStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 9 ||
			lpObj->pInventory[1].GetDetailItemType() == 9 )
		{
			lpObj->m_MagicDamageMin += lpObj->m_MPSkillOpt.MpsStickStrength;
			lpObj->m_MagicDamageMax += lpObj->m_MPSkillOpt.MpsStickStrength;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsShieldStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 6 ||
			lpObj->pInventory[1].GetDetailItemType() == 6 )
		{
			lpObj->m_Defense += lpObj->m_MPSkillOpt.MpsShieldStrength;
			lpObj->m_Defense += lpObj->m_MPSkillOpt.MpsShieldStrength2;	// bug ? 
		}
	}

	if( lpObj->m_MPSkillOpt.MpsShieldStrength2 > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 6 ||
			lpObj->pInventory[1].GetDetailItemType() == 6 )
		{
			lpObj->m_Defense += lpObj->m_MPSkillOpt.MpsShieldStrength2;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsShieldStrength3 > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 6 ||
			lpObj->pInventory[1].GetDetailItemType() == 6 )
		{
			lpObj->m_Defense += lpObj->m_MPSkillOpt.MpsShieldStrength3;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsOneHandStaffMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 4 ||
			lpObj->pInventory[1].GetDetailItemType() == 4 )
		{
			lpObj->m_AttackSpeed += lpObj->m_MPSkillOpt.MpsOneHandStaffMastery;
			lpObj->m_MagicSpeed += lpObj->m_MPSkillOpt.MpsOneHandStaffMastery;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsTomeMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 10 ||
			lpObj->pInventory[1].GetDetailItemType() == 10 )
		{
			lpObj->m_AttackSpeed += lpObj->m_MPSkillOpt.MpsTomeMastery;
			lpObj->m_MagicSpeed += lpObj->m_MPSkillOpt.MpsTomeMastery;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsTomeStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 10 ||
			lpObj->pInventory[1].GetDetailItemType() == 10 )
		{
			lpObj->m_CurseDamageMin += lpObj->m_MPSkillOpt.MpsTomeStrength;
			lpObj->m_CurseDamageMax += lpObj->m_MPSkillOpt.MpsTomeStrength;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsTwoHandStaffMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 5 &&
			lpObj->pInventory[1].GetDetailItemType() != 5 )
		{
			lpObj->m_MPSkillOpt.MpsTwoHandStaffMastery = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsCrossbowMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 8 &&
			lpObj->pInventory[1].GetDetailItemType() != 8 )
		{
			lpObj->m_MPSkillOpt.MpsCrossbowMastery = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsStickMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 9 &&
			lpObj->pInventory[1].GetDetailItemType() != 9 )
		{
			lpObj->m_MPSkillOpt.MpsStickMastery = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsScepterMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 11 &&
			lpObj->pInventory[1].GetDetailItemType() != 11 )
		{
			lpObj->m_MPSkillOpt.MpsScepterMastery = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsShieldMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 6 &&
			lpObj->pInventory[1].GetDetailItemType() != 6 )
		{
			lpObj->m_MPSkillOpt.MpsShieldMastery = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsShieldMastery2 > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 6 &&
			lpObj->pInventory[1].GetDetailItemType() != 6 )
		{
			lpObj->m_MPSkillOpt.MpsShieldMastery2 = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsShieldMastery3 > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 6 &&
			lpObj->pInventory[1].GetDetailItemType() != 6 )
		{
			lpObj->m_MPSkillOpt.MpsShieldMastery3 = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsOneHandSwordMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 1 ||
			lpObj->pInventory[1].GetDetailItemType() == 1 )
		{
			lpObj->m_AttackSpeed += lpObj->m_MPSkillOpt.MpsOneHandSwordMastery;
			lpObj->m_MagicSpeed += lpObj->m_MPSkillOpt.MpsOneHandSwordMastery;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsOneHandSwordStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 1 &&
			lpObj->pInventory[1].GetDetailItemType() != 1 )
		{
			lpObj->m_MPSkillOpt.MpsOneHandSwordStrength = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsMaceStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 2 &&
			lpObj->pInventory[1].GetDetailItemType() != 2 )
		{
			lpObj->m_MPSkillOpt.MpsMaceStrength = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsMaceMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 2 &&
			lpObj->pInventory[1].GetDetailItemType() != 2 )
		{
			lpObj->m_MPSkillOpt.MpsMaceMastery = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsSpearStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 3 &&
			lpObj->pInventory[1].GetDetailItemType() != 3 )
		{
			lpObj->m_MPSkillOpt.MpsSpearStrength = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsSpearMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 3 &&
			lpObj->pInventory[1].GetDetailItemType() != 3 )
		{
			lpObj->m_MPSkillOpt.MpsSpearMastery = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsBowStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 7 &&
			lpObj->pInventory[1].GetDetailItemType() != 7 )
		{
			lpObj->m_MPSkillOpt.MpsBowStrength = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsScepterStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 11 &&
			lpObj->pInventory[1].GetDetailItemType() != 11 )
		{
			lpObj->m_MPSkillOpt.MpsScepterStrength = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsUseScepterPetStr > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 11 &&
			lpObj->pInventory[1].GetDetailItemType() != 11 )
		{
			lpObj->m_MPSkillOpt.MpsUseScepterPetStr = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsCommandAttackInc > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 11 &&
			lpObj->pInventory[1].GetDetailItemType() != 11 )
		{
			lpObj->m_MPSkillOpt.MpsCommandAttackInc = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsBowMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() == 7 ||
			lpObj->pInventory[1].GetDetailItemType() == 7 )
		{
			lpObj->m_AttackSpeed += lpObj->m_MPSkillOpt.MpsBowMastery;
			lpObj->m_MagicSpeed += lpObj->m_MPSkillOpt.MpsBowMastery;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsCrossbowStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 8 &&
			lpObj->pInventory[1].GetDetailItemType() != 8 )
		{
			lpObj->m_MPSkillOpt.MpsCrossbowStrength = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsDarkHorseStrength > 0.0f )
	{
		if( lpObj->pInventory[8].m_Type == ITEMGET(13,4) )
		{
			lpObj->m_Defense += lpObj->m_MPSkillOpt.MpsDarkHorseStrength;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsFistStrength > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 12 &&
			lpObj->pInventory[1].GetDetailItemType() != 12 )
		{
			lpObj->m_MPSkillOpt.MpsFistStrength = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsFistMastery > 0.0f )
	{
		if( lpObj->pInventory[0].GetDetailItemType() != 12 &&
			lpObj->pInventory[1].GetDetailItemType() != 12 )
		{
			lpObj->m_MPSkillOpt.MpsFistMastery = 0;
		}
	}

	if( lpObj->m_MPSkillOpt.MpsWingDefenseIncrease > 0.0f )
	{
		if( lpObj->Class == CLASS_KNIGHT )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 20 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_WIZARD )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 21 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_ELF )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 22 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_MAGUMSA )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 23 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_DARKLORD )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 24 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_SUMMONER )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 25 )
			{
				lpObj->m_MPSkillOpt.MpsWingDefenseIncrease = 0;
			}
		}
	}

	if( lpObj->m_MPSkillOpt.MpsWingAttackIncrease > 0.0f )
	{
		if( lpObj->Class == CLASS_KNIGHT )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 20 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_WIZARD )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 21 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_ELF )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 22 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_MAGUMSA )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 23 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_DARKLORD )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 24 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = 0;
			}
		}
		else if( lpObj->Class == CLASS_SUMMONER )
		{
			if( lpObj->pInventory[7].GetDetailItemType() != 25 )
			{
				lpObj->m_MPSkillOpt.MpsWingAttackIncrease = 0;
			}
		}
	}
}