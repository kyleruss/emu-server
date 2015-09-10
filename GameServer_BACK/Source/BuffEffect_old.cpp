#include "stdafx.h"
#include "BuffEffect.h"
#include "GameMain.h"
#include "ItemAddOption.h"
#include "MasterSkillSystem.h"
#include "GameEvent.h"
#include "..\include\readscript.h"
#include "..\common\winutil.h"
#include "logproc.h"


CEffectOption g_EffectOption;

CEffectOption::CEffectOption()
{

}

CEffectOption::~CEffectOption()
{

}

void CEffectOption::_SetEffect(LPOBJ lpObj,BYTE btEffectType,int iEffectValue)
{
	if( lpObj == NULL || btEffectType < 0 )
	{
		return;
	}

	if( lpObj->Connected < PLAYER_PLAYING )
	{
		return;
	}

	switch( btEffectType )
	{
	case ADD_OPTION_ATTACK_SPEED:
		lpObj->m_AttackSpeed += iEffectValue;
		lpObj->m_MagicSpeed += iEffectValue;
		break;
	case ADD_OPTION_DEFENSE:
		lpObj->m_Defense += iEffectValue;
		lpObj->m_MagicDefense += iEffectValue;
		break;
	case ADD_OPTION_LIFE:
		lpObj->AddLife += iEffectValue;
		GCReFillSend(lpObj->m_Index, (WORD)(lpObj->MaxLife + lpObj->AddLife), -2, 0, lpObj->iMaxShield + lpObj->iAddShield);
		GCReFillSend(lpObj->m_Index, (WORD)(lpObj->Life), -1, 0, lpObj->iShield);
		break;
	case ADD_OPTION_MANA:
		lpObj->AddMana += iEffectValue;
		GCManaSend(lpObj->m_Index, (WORD)(lpObj->AddMana + lpObj->MaxMana), -2, 0, lpObj->MaxBP + lpObj->AddBP);
		GCManaSend(lpObj->m_Index, (WORD)(lpObj->Mana), -1, 0, lpObj->BP);
		break;
	case ADD_OPTION_STRENGTH:
		lpObj->AddStrength += iEffectValue;
		break;
	case ADD_OPTION_DEXTERITY:
		lpObj->AddDexterity += iEffectValue;
		break;
	case ADD_OPTION_VITALITY:
		lpObj->AddVitality += iEffectValue;
		break;
	case ADD_OPTION_ENERGY:
		lpObj->AddEnergy += iEffectValue;
		break;
	case ADD_OPTION_LEADERSHIP:
		lpObj->AddLeadership += iEffectValue;
		break;
	case ADD_OPTION_MAGICDEFENSE:
		lpObj->m_MagicDefense += iEffectValue;
		break;
	case ADD_OPTION_REFLECTDAMAGE:
		lpObj->DamageReflect += iEffectValue;
		break;
	case ADD_OPTION_ATTACKRATE:
		lpObj->m_AttackRating += iEffectValue;
		break;
	case ADD_OPTION_BERSERKMANA_PERC:
		lpObj->AddMana += int(lpObj->MaxMana*iEffectValue/100.0f);
		GCManaSend(lpObj->m_Index,(WORD)(lpObj->MaxMana + lpObj->AddMana),0xFE,0,lpObj->MaxBP+ lpObj->AddBP);
		GCManaSend(lpObj->m_Index,(WORD)(lpObj->Mana),0xFF,0,lpObj->BP);
		break;
	case ADD_OPTION_BERSERKLIFE_PERC:
		{
			float declife = 0;
			float fVal = (40.0f - iEffectValue)/100.0f;
			fVal = (fVal > 0.1f) ? fVal : 0.1f;

			declife = lpObj->MaxLife*fVal;
			lpObj->AddLife = lpObj->AddLife - int(declife);

			lpObj->Life = (double(lpObj->Life) < lpObj->MaxLife + lpObj->AddLife) ? float(double(lpObj->Life)): lpObj->MaxLife + lpObj->AddLife;

			GCReFillSend(lpObj->m_Index,(WORD)(lpObj->MaxLife+lpObj->AddLife),0xFE,0,lpObj->iMaxShield+lpObj->iAddShield);
			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
		}
		break;
	case ADD_OPTION_MAGICDAMAGEMIN:
		lpObj->m_MagicDamageMin += iEffectValue;
		break;
	case ADD_OPTION_MAGICDAMAGEMAX:
		lpObj->m_MagicDamageMax += iEffectValue;
		break;
	case ADD_OPTION_SANTABUFF:
		lpObj->m_AttackDamageMaxLeft += iEffectValue;
		lpObj->m_AttackDamageMinLeft += iEffectValue;

		lpObj->m_AttackDamageMaxRight += iEffectValue;
		lpObj->m_AttackDamageMinRight += iEffectValue;

		lpObj->m_MagicDamageMin += iEffectValue;
		lpObj->m_MagicDamageMax += iEffectValue;

		lpObj->m_CurseDamageMax += iEffectValue;
		lpObj->m_CurseDamageMin += iEffectValue;
		break;
	case 58:
		lpObj->m_Defense += iEffectValue;
		break;
	case ADD_OPTION_BP:
		lpObj->AddBP += iEffectValue;
		GCManaSend(lpObj->m_Index, (WORD)(lpObj->AddMana + lpObj->MaxMana), -2, 0, lpObj->MaxBP + lpObj->AddBP);
		GCManaSend(lpObj->m_Index, (WORD)lpObj->Mana, -1, 0, lpObj->BP);
		break;
	case ADD_OPTION_ALLSTATS:
		lpObj->AddStrength += iEffectValue;
		lpObj->AddDexterity += iEffectValue;
		lpObj->AddVitality += iEffectValue;
		lpObj->AddEnergy += iEffectValue;
		break;
	}
}

void CEffectOption::_ClearItemEffect(LPOBJ lpObj,BYTE btEffectType,int iEffectValue)
{
	if( lpObj == NULL || btEffectType < 0 )
	{
		return;
	}

	if( lpObj->Connected < PLAYER_PLAYING )
	{
		return;
	}

	switch( btEffectType )
	{
	case ADD_OPTION_ATTACK_SPEED:
		lpObj->m_AttackSpeed -= iEffectValue;
		lpObj->m_MagicSpeed -= iEffectValue;
		break;
	case ADD_OPTION_DEFENSE:
		lpObj->m_Defense -= iEffectValue;
		lpObj->m_MagicDefense -= iEffectValue;
		break;
	case ADD_OPTION_LIFE:
		lpObj->AddLife -= iEffectValue;
		GCReFillSend(lpObj->m_Index, (WORD)(lpObj->MaxLife + lpObj->AddLife), -2, 0, lpObj->iMaxShield + lpObj->iAddShield);
		GCReFillSend(lpObj->m_Index, (WORD)(lpObj->Life), -1, 0, lpObj->iShield);
		break;
	case ADD_OPTION_MANA:
		lpObj->AddMana -= iEffectValue;
		GCManaSend(lpObj->m_Index, (WORD)(lpObj->AddMana + lpObj->MaxMana), -2, 0, lpObj->MaxBP + lpObj->AddBP);
		GCManaSend(lpObj->m_Index, (WORD)lpObj->Mana, -1, 0, lpObj->BP);
		break;
	case ADD_OPTION_STRENGTH:
		lpObj->AddStrength -= iEffectValue;
		break;
	case ADD_OPTION_DEXTERITY:
		lpObj->AddDexterity -= iEffectValue;
		break;
	case ADD_OPTION_VITALITY:
		lpObj->AddVitality -= iEffectValue;
		break;
	case ADD_OPTION_ENERGY:
		lpObj->AddEnergy -= iEffectValue;
		break;
	case ADD_OPTION_LEADERSHIP:
		lpObj->AddLeadership -= iEffectValue;
		break;
	case ADD_OPTION_MAGICDEFENSE:
		lpObj->m_MagicDefense -= iEffectValue;
		break;
	case ADD_OPTION_REFLECTDAMAGE:
		lpObj->DamageReflect -= iEffectValue;
		break;
	case ADD_OPTION_ATTACKRATE:
		lpObj->m_AttackRating -= iEffectValue;
		break;
	case ADD_OPTION_BERSERKMANA_PERC:
		{
			lpObj->AddMana -= (int)(lpObj->MaxMana * iEffectValue / 100.0f);
			
			if (lpObj->Mana < lpObj->MaxMana + lpObj->AddMana )
			{
				lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
			}

			GCManaSend(lpObj->m_Index,(WORD)(lpObj->MaxMana + lpObj->AddMana),0xFE,0,lpObj->MaxBP+ lpObj->AddBP);
			GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
		}
		break;
	case ADD_OPTION_BERSERKLIFE_PERC:
		{
			float addlife = 0;
			float fVal = (40.0f - iEffectValue)/100.0f;

			if(fVal < 0.1f)
			{
				fVal = 0.1f;
			}

			addlife = lpObj->MaxLife*fVal;
			lpObj->AddLife = (int)(lpObj->AddLife + addlife);

			GCReFillSend(lpObj->m_Index,(WORD)(lpObj->MaxLife+lpObj->AddLife),0xFE,0,lpObj->iMaxShield+lpObj->iAddShield);
			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
		}
		break;
	case ADD_OPTION_MAGICDAMAGEMIN:
		lpObj->m_MagicDamageMin -= iEffectValue;
		break;
	case ADD_OPTION_MAGICDAMAGEMAX:
		lpObj->m_MagicDamageMax -= iEffectValue;
		break;
	case ADD_OPTION_SANTABUFF:
		lpObj->m_AttackDamageMaxLeft -= iEffectValue;
		lpObj->m_AttackDamageMinLeft -= iEffectValue;

		lpObj->m_AttackDamageMaxRight -= iEffectValue;
		lpObj->m_AttackDamageMinRight -= iEffectValue;

		lpObj->m_MagicDamageMin -= iEffectValue;
		lpObj->m_MagicDamageMax -= iEffectValue;

		lpObj->m_CurseDamageMax -= iEffectValue;
		lpObj->m_CurseDamageMin -= iEffectValue;
		break;
	case 58:
		lpObj->m_Defense -= iEffectValue;
		break;
	case ADD_OPTION_BP:
		lpObj->AddBP -= iEffectValue;
		GCManaSend(lpObj->m_Index,(WORD)(lpObj->AddMana + lpObj->MaxMana), -2, 0, lpObj->MaxBP + lpObj->AddBP);
		GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana, -1, 0, lpObj->BP);
		break;
	case ADD_OPTION_ALLSTATS:
		lpObj->AddStrength -= iEffectValue;
		lpObj->AddDexterity -= iEffectValue;
		lpObj->AddVitality -= iEffectValue;
		lpObj->AddEnergy -= iEffectValue;
		break;
	}
}

void CEffectOption::BeAttackEffect(LPOBJ lpObj,BYTE btEffectType,int iEffectValue)
{
	if( btEffectType <= 0)
		return;

	switch( btEffectType )
	{
	case ADD_OPTION_REDUCE_HP:
		ReduceHP(lpObj,iEffectValue);
		break;
	case ADD_OPTION_REDUCE_HPPERC:
		ReduceHPPerc(lpObj,iEffectValue);
		break;
	case ADD_OPTION_ABSORBHP:
		AbsorbHP(lpObj,iEffectValue);
		break;
	}
}

void CEffectOption::ReduceHP(LPOBJ lpObj,int iREDUCE_HP)
{
	int ReduceHP = 0;
	int ReduceShield = 0;

	ReduceHP = iREDUCE_HP;
	ReduceShield = ReduceHP;

	if( lpObj->Live == FALSE )
	{
		return;
	}
	
	if( lpObj->lpAttackObj != NULL &&
		lpObj->Type == OBJ_USER &&
		lpObj->lpAttackObj->Type == OBJ_USER )
	{
		ReduceShield = ReduceHP * 90 / 100;
		ReduceHP -= ReduceShield;
		if( lpObj->iShield - ReduceShield > 0 )
		{
			lpObj->iShield -= ReduceShield;
			lpObj->Life -= ReduceHP;
		}
		else
		{
			ReduceHP = ReduceHP + (ReduceShield - lpObj->iShield);
			ReduceShield = lpObj->iShield;
			lpObj->Life -= ReduceHP;
			lpObj->iShield = 0;
		}
	}
	else
	{
		ReduceShield = 0;
		lpObj->Life -= ReduceHP;
	}

	if( lpObj->Life < 0.0f)
	{
		lpObj->Life = 0;
	}

	if( lpObj->lpAttackObj != NULL )
	{
		gObjLifeCheck(lpObj,lpObj->lpAttackObj,ReduceHP,3,0,0,0,ReduceShield);
	}	
}

void CEffectOption::AbsorbHP(LPOBJ lpObj,int iREDUCE_HP)
{
	int ReduceHP = 0;
	int ReduceShield = 0;

	ReduceHP = iREDUCE_HP;
	ReduceShield = ReduceHP;

	if( lpObj->Live == FALSE )
	{
		return;
	}
	
	if( lpObj->lpAttackObj != NULL &&
		lpObj->Type == OBJ_USER &&
		lpObj->lpAttackObj->Type == OBJ_USER )
	{
		ReduceShield = ReduceHP * 90 / 100;
		ReduceHP -= ReduceShield;
		if( lpObj->iShield - ReduceShield > 0 )
		{
			lpObj->iShield -= ReduceShield;
			lpObj->Life -= ReduceHP;
		}
		else
		{
			ReduceHP = ReduceHP + (ReduceShield - lpObj->iShield);
			ReduceShield = lpObj->iShield;
			lpObj->Life -= ReduceHP;
			lpObj->iShield = 0;
		}

		lpObj->lpAttackObj->Life += ReduceHP;

		if( lpObj->lpAttackObj->Life > (lpObj->lpAttackObj->MaxLife + lpObj->lpAttackObj->AddLife) )
			lpObj->lpAttackObj->Life = (lpObj->lpAttackObj->MaxLife + lpObj->lpAttackObj->AddLife);

		GCReFillSend(lpObj->lpAttackObj->m_Index, (WORD)lpObj->lpAttackObj->Life, -1, 0, lpObj->lpAttackObj->iShield);
	}
	else
	{
		ReduceShield = 0;
		lpObj->Life -= ReduceHP;
	}

	if( lpObj->Life < 0.0f)
	{
		lpObj->Life = 0;
	}

	if( lpObj->lpAttackObj != NULL )
	{
		gObjLifeCheck(lpObj,lpObj->lpAttackObj,ReduceHP,3,0,0,0,ReduceShield);
	}	
}

void CEffectOption::ReduceHPPerc(LPOBJ lpObj,BYTE iREDUCE_PERC)
{
	int ReduceHP = 0;
	int ReduceShield = 0;

	ReduceHP = (int)(lpObj->Life * iREDUCE_PERC / 100.0f);
	ReduceShield = ReduceHP;

	if( lpObj->lpAttackObj != NULL &&
		lpObj->Type == OBJ_USER &&
		lpObj->lpAttackObj->Type == OBJ_USER )
	{
		if( lpObj->iShield - ReduceShield > 0 )
		{
			lpObj->iShield -= ReduceShield;
			ReduceHP = 0;
		}
		else
		{
			ReduceHP = ReduceShield - lpObj->iShield;
			ReduceShield = lpObj->iShield;
			lpObj->Life -= ReduceHP;
			lpObj->iShield = 0;
		}
	}
	else
	{
		lpObj->Life -= ReduceHP;
		ReduceShield = 0;
	}

	if( lpObj->Life < 0.0f)
	{
		lpObj->Life = 0;
	}

	if( lpObj->lpAttackObj != NULL )
	{
		gObjLifeCheck(lpObj,lpObj->lpAttackObj,ReduceHP,2,0,0,1,ReduceShield);
	}	
}



void CEffectOption::SetEffectInfo1(LPOBJ lpObj)
{
	if( lpObj == NULL )
		return;

	int count = 0;

	for(int i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum == FALSE )
		{
			continue;
		}

		switch( lpObj->pEffectInfo[i].btEffectType1 )
		{
		case ADD_OPTION_LIFE:
		case ADD_OPTION_MANA:
		case ADD_OPTION_STRENGTH:
		case ADD_OPTION_DEXTERITY:
		case ADD_OPTION_VITALITY:
		case ADD_OPTION_ENERGY:
		case ADD_OPTION_LEADERSHIP:
		case ADD_OPTION_REFLECTDAMAGE:
		case ADD_OPTION_BP:
		case ADD_OPTION_ALLSTATS:
		//se ADD_OPTION_BERSERKMANA_PERC:
		//case ADD_OPTION_BERSERKLIFE_PERC:
			count++;
			this->_SetEffect(lpObj,lpObj->pEffectInfo[i].btEffectType1,lpObj->pEffectInfo[i].iEffectValue1);
			break;
		}

		switch( lpObj->pEffectInfo[i].btEffectType2 )
		{
		case ADD_OPTION_LIFE:
		case ADD_OPTION_MANA:
		case ADD_OPTION_STRENGTH:
		case ADD_OPTION_DEXTERITY:
		case ADD_OPTION_VITALITY:
		case ADD_OPTION_ENERGY:
		case ADD_OPTION_LEADERSHIP:
		case ADD_OPTION_REFLECTDAMAGE:
		case ADD_OPTION_BP:
		case ADD_OPTION_ALLSTATS:
		//case ADD_OPTION_BERSERKMANA_PERC:
		//case ADD_OPTION_BERSERKLIFE_PERC:
			count++;
			this->_SetEffect(lpObj,lpObj->pEffectInfo[i].btEffectType2,lpObj->pEffectInfo[i].iEffectValue2);
			break;
		}

		switch( lpObj->pEffectInfo[i].btEffectType3 )
		{
		case ADD_OPTION_LIFE:
		case ADD_OPTION_MANA:
		case ADD_OPTION_STRENGTH:
		case ADD_OPTION_DEXTERITY:
		case ADD_OPTION_VITALITY:
		case ADD_OPTION_ENERGY:
		case ADD_OPTION_LEADERSHIP:
		case ADD_OPTION_REFLECTDAMAGE:
		case ADD_OPTION_BP:
		case ADD_OPTION_ALLSTATS:
		//case ADD_OPTION_BERSERKMANA_PERC:
		//case ADD_OPTION_BERSERKLIFE_PERC:
			count++;
			this->_SetEffect(lpObj,lpObj->pEffectInfo[i].btEffectType3,lpObj->pEffectInfo[i].iEffectValue3);
			break;
		}
	}
}

void CEffectOption::SetEffectInfo2(LPOBJ lpObj)
{
	if( lpObj == NULL )
		return;

	int count = 0;

	for(int i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum == FALSE )
		{
			continue;
		}

		switch( lpObj->pEffectInfo[i].btEffectType1 )
		{
		case ADD_OPTION_LIFE:
		case ADD_OPTION_MANA:
		case ADD_OPTION_STRENGTH:
		case ADD_OPTION_DEXTERITY:
		case ADD_OPTION_VITALITY:
		case ADD_OPTION_ENERGY:
		case ADD_OPTION_LEADERSHIP:
		case ADD_OPTION_REFLECTDAMAGE:
		case ADD_OPTION_BP:
		case ADD_OPTION_ALLSTATS:
		//case ADD_OPTION_BERSERKMANA_PERC:
		//case ADD_OPTION_BERSERKLIFE_PERC:
			break;
		default:
			count++;
			this->_SetEffect(lpObj,lpObj->pEffectInfo[i].btEffectType1,lpObj->pEffectInfo[i].iEffectValue1);
			break;
		}

		switch( lpObj->pEffectInfo[i].btEffectType2 )
		{
		case ADD_OPTION_LIFE:
		case ADD_OPTION_MANA:
		case ADD_OPTION_STRENGTH:
		case ADD_OPTION_DEXTERITY:
		case ADD_OPTION_VITALITY:
		case ADD_OPTION_ENERGY:
		case ADD_OPTION_LEADERSHIP:
		case ADD_OPTION_REFLECTDAMAGE:
		case ADD_OPTION_BP:
		case ADD_OPTION_ALLSTATS:
		//case ADD_OPTION_BERSERKMANA_PERC:
		//case ADD_OPTION_BERSERKLIFE_PERC:
			break;
		default:
			count++;
			this->_SetEffect(lpObj,lpObj->pEffectInfo[i].btEffectType2,lpObj->pEffectInfo[i].iEffectValue2);
			break;
		}

		switch( lpObj->pEffectInfo[i].btEffectType3 )
		{
		case ADD_OPTION_LIFE:
		case ADD_OPTION_MANA:
		case ADD_OPTION_STRENGTH:
		case ADD_OPTION_DEXTERITY:
		case ADD_OPTION_VITALITY:
		case ADD_OPTION_ENERGY:
		case ADD_OPTION_LEADERSHIP:
		case ADD_OPTION_REFLECTDAMAGE:
		case ADD_OPTION_BP:
		case ADD_OPTION_ALLSTATS:
		//case ADD_OPTION_BERSERKMANA_PERC:
		//case ADD_OPTION_BERSERKLIFE_PERC:
			break;
		default:
			count++;
			this->_SetEffect(lpObj,lpObj->pEffectInfo[i].btEffectType3,lpObj->pEffectInfo[i].iEffectValue3);
			break;
		}
	}
}

void CEffectOption::ClearEffect(LPOBJ lpObj)
{
	for(int i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum == FALSE )
		{
			continue;
		}
		
		_ClearItemEffect(lpObj,lpObj->pEffectInfo[i].btEffectType1,lpObj->pEffectInfo[i].iEffectValue1);
		_ClearItemEffect(lpObj,lpObj->pEffectInfo[i].btEffectType2,lpObj->pEffectInfo[i].iEffectValue2);
		_ClearItemEffect(lpObj,lpObj->pEffectInfo[i].btEffectType3,lpObj->pEffectInfo[i].iEffectValue3);
	}
}

CEffectAddOption g_EffectAddOption;

CEffectAddOption::CEffectAddOption()
{

}

CEffectAddOption::~CEffectAddOption()
{

}

int CEffectAddOption::AddEffect(LPOBJ lpObj,int EffectNumber,BYTE btEffectType1,BYTE btEffectType2,BYTE btEffectType3,int iEffectValue1,int iEffectValue2,int iEffectValue3,int iTimeValue)
{
	BUFF_EFFECT_INFO* lpBuff = NULL;
	BUFF_EFFECT_INFO* lpCBuff = NULL;

	int Type1Count = 0;
	int Type2Count = 0;
	int retEffect = 0;

	if( lpObj == NULL )
		return -1;

	if( lpObj->Connected < PLAYER_PLAYING )
		return -1;

	if( g_BuffEffect.IsBuff(EffectNumber) == FALSE )
		return -1;

	lpBuff = g_BuffEffect.SearchBuff(EffectNumber);

	if( lpBuff == NULL )
		return -1;

	int i;
	for(i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum != NULL )
		{
			lpCBuff = g_BuffEffect.SearchBuff(lpObj->pEffectInfo[i].btEffectNum);

			if( lpCBuff == NULL )
				continue;

			if( lpCBuff->btOptionType == lpBuff->btOptionType )
			{
				ClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum);
				retEffect = lpCBuff->btNumber;
			}

			if( lpCBuff->btType == 0 )
			{
				Type1Count++;
			}
			else if( lpCBuff->btType == 1 )
			{
				Type2Count++;
			}

			if( i > lpObj->btEffectCount )
			{
				break;
			}
		}
	}

	for(i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum == NULL )
		{
			lpObj->pEffectInfo[i].btEffectNum = EffectNumber;
			lpObj->pEffectInfo[i].btOptionType = lpBuff->btOptionType;
			lpObj->pEffectInfo[i].btEffectType1 = btEffectType1;
			lpObj->pEffectInfo[i].btEffectType2 = btEffectType2;
			lpObj->pEffectInfo[i].btEffectType3 = btEffectType3;
			lpObj->pEffectInfo[i].iEffectValue1 = iEffectValue1;
			lpObj->pEffectInfo[i].iEffectValue2 = iEffectValue2;
			lpObj->pEffectInfo[i].iEffectValue3 = iEffectValue3;
			lpObj->pEffectInfo[i].iTimeValue = iTimeValue;
			lpObj->pEffectInfo[i].dwTICK_COUNT = GetTickCount();

			g_EffectOption._SetEffect(lpObj,btEffectType1,iEffectValue1);
			g_EffectOption._SetEffect(lpObj,btEffectType2,iEffectValue2);
			g_EffectOption._SetEffect(lpObj,btEffectType3,iEffectValue3);
			break;
		}
	}

	return retEffect;
}

BYTE CEffectAddOption::ClearEffect(LPOBJ lpObj,int BuffNumber)
{
	BYTE btResult = 0;
	BYTE bCalCharacter = 0;

	if( lpObj == NULL )
		return FALSE;

	if( lpObj->Connected < PLAYER_PLAYING )
		return FALSE;

	if( g_BuffEffect.IsBuff( BuffNumber ) == FALSE )
		return FALSE;

	for(int i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( btResult == 1 &&
			lpObj->pEffectInfo[i].btEffectNum != NULL &&
			i > 0 )
		{
			memcpy(&lpObj->pEffectInfo[i-1],&lpObj->pEffectInfo[i],sizeof(lpObj->pEffectInfo[i]));
			memset(&lpObj->pEffectInfo[i],0,sizeof(lpObj->pEffectInfo[i]));
		}

		if( lpObj->pEffectInfo[i].btEffectNum == BuffNumber )
		{
			g_EffectOption._ClearItemEffect(lpObj,lpObj->pEffectInfo[i].btEffectType1,lpObj->pEffectInfo[i].iEffectValue1);
			g_EffectOption._ClearItemEffect(lpObj,lpObj->pEffectInfo[i].btEffectType2,lpObj->pEffectInfo[i].iEffectValue2);
			g_EffectOption._ClearItemEffect(lpObj,lpObj->pEffectInfo[i].btEffectType3,lpObj->pEffectInfo[i].iEffectValue3);
			memset(&lpObj->pEffectInfo[i],0,sizeof(lpObj->pEffectInfo[i]));
			btResult = TRUE;
		}
	}
return btResult;
}

BYTE CEffectAddOption::IsEffectActive(LPOBJ lpObj,int BuffNumber)
{
	if( g_BuffEffect.IsBuff(BuffNumber) == FALSE )
	{
		return FALSE;
	}

	for(int i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum == BuffNumber )
		{
			return TRUE;
		}
	}
return FALSE;
}

void CEffectAddOption::RemoveBuffEffect(LPOBJ lpObj,int BuffNumber)
{
	switch( BuffNumber )
	{
	case 56:
		lpObj->DelayActionTime = 0;
		lpObj->DelayLevel = 0;
		break;
	case 130:
	case BUFF_BLESS:
	case BUFF_BLESS2:
	case BUFF_GLADIATOR_HONOR:
	case 171:
	case 172:
	case 173:
		gObjCalCharacter(lpObj->m_Index);
		break;
	}
}

int CEffectAddOption::GetClearCount(LPOBJ lpObj,int Type)
{
	BUFF_EFFECT_INFO* lpBuff = NULL;
	int ClearCount = 0;

	for(int i = 0; i < MAX_EFFECT_INFO;i++)
	{
		if( IsEffectActive(lpObj,lpObj->pEffectInfo[i].btEffectNum) == TRUE )
		{
			lpBuff = g_BuffEffect.SearchBuff(lpObj->pEffectInfo[i].btEffectNum);

			if( lpBuff == NULL )
				continue;

			if( lpBuff->btClearType == Type )
			{
				if( ClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum) == TRUE )
				{
					ClearCount++;
					i--;
				}
			}

			if( Type == 2 )
			{
				if( ClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum) == TRUE )
				{
					ClearCount++;
					i--;
				}
			}

			if( Type == 1 )
			{
				if( ClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum) == TRUE )
				{
					ClearCount++;
					i--;
				}
			}
		}
	}
return ClearCount;
}

BUFF_EFFECT_INFO* CEffectAddOption::GetEffectInfo(int BuffNumber)
{
	BUFF_EFFECT_INFO* lpBuff = FALSE;
	if( g_BuffEffect.IsBuff(BuffNumber) == FALSE )
	{
		return FALSE;
	}

	lpBuff = g_BuffEffect.SearchBuff(BuffNumber);
return lpBuff;
}

int CEffectAddOption::GetEffectClearType(int BuffNumber)
{
	BUFF_EFFECT_INFO* lpBuff = FALSE;
	if( g_BuffEffect.IsBuff(BuffNumber) == FALSE )
	{
		return -1;
	}

	lpBuff = g_BuffEffect.SearchBuff(BuffNumber);
return lpBuff->btClearType;
}

void gObjSkillBeAttackProc(LPOBJ lpObj)
{
	if( lpObj == NULL )
	{
		return;
	}

	if( lpObj->m_Index < 0 || lpObj->m_Index > OBJMAX-1)
	{
		return;
	}

	if( (lpObj->Type == OBJ_USER && lpObj->Connected >= PLAYER_PLAYING) ||
		lpObj->Type == OBJ_MONSTER )
	{
		for(int i = 0; i < MAX_EFFECT_INFO;i++)
		{
			if( lpObj->pEffectInfo[i].iTimeValue == -10 )
			{
				continue;
			}

			if( lpObj->pEffectInfo[i].btEffectNum == FALSE )
				continue;

			switch( lpObj->pEffectInfo[i].btEffectNum )
			{
			case BUFF_POISON:
				if( lpObj->pEffectInfo[i].iTimeValue % 3 == FALSE )
				{
					g_EffectOption.BeAttackEffect(lpObj,lpObj->pEffectInfo[i].btEffectType1,lpObj->pEffectInfo[i].iEffectValue1);
					g_EffectOption.BeAttackEffect(lpObj,lpObj->pEffectInfo[i].btEffectType2,lpObj->pEffectInfo[i].iEffectValue2);
					g_EffectOption.BeAttackEffect(lpObj,lpObj->pEffectInfo[i].btEffectType3,lpObj->pEffectInfo[i].iEffectValue3);
				}
				break;
			default:
				g_EffectOption.BeAttackEffect(lpObj,lpObj->pEffectInfo[i].btEffectType1,lpObj->pEffectInfo[i].iEffectValue1);
				g_EffectOption.BeAttackEffect(lpObj,lpObj->pEffectInfo[i].btEffectType2,lpObj->pEffectInfo[i].iEffectValue2);
				g_EffectOption.BeAttackEffect(lpObj,lpObj->pEffectInfo[i].btEffectType3,lpObj->pEffectInfo[i].iEffectValue3);
				break;
			}

			lpObj->pEffectInfo[i].iTimeValue--;

			if( lpObj->pEffectInfo[i].iTimeValue <= 0 )
			{
				gObjClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum);
			}
		}
	}
}

BYTE gObjAddEffect(LPOBJ lpObj,int BuffNumber)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->m_Index < 0 || lpObj->m_Index > OBJMAX-1 )
		return FALSE;

	int Item = 0;
	LPITEMEFFECT lpEffect = 0;
	BUFF_EFFECT_INFO* lpBuff = 0;

	lpBuff = g_EffectAddOption.GetEffectInfo(BuffNumber);

	if( lpBuff == NULL )
		return FALSE;

	Item = lpBuff->btItemIndex * 512 + lpBuff->btItemNumber;

	if( g_ItemAddOption.IsItemEffect(Item) == FALSE )
	{
		return FALSE;
	}

	lpEffect = g_ItemAddOption.GetItemEffect(Item);
	int Buff = 0;
	
	Buff = g_EffectAddOption.AddEffect(lpObj,BuffNumber,(BYTE)lpEffect->wEffectType1,(BYTE)lpEffect->wEffectType2,0,lpEffect->iEffectValue1,lpEffect->iEffectValue2,0,lpEffect->iEffectValidTime);

	if( Buff != -1 )
	{
		if( Buff > 0 )
		{
			GCSendBuffInfo(lpObj,Buff,2,lpEffect->wOptionType,0,0);
		}
		else
		{
			lpObj->btEffectCount++;
		}

		GCSendBuffInfo(lpObj,BuffNumber,0,lpEffect->wOptionType,lpEffect->wEffectType1,lpEffect->iEffectValidTime);
		return TRUE;
	}
return FALSE;
}

BYTE gObjAddEffect(LPOBJ lpObj,int BuffNumber,BYTE btEffect1,int iEffectValue1,BYTE btEffect2,int iEffectValue2,BYTE btEffect3,int iEffectValue3,int iEffectTime)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->m_Index < 0 || lpObj->m_Index > OBJMAX-1 )
		return FALSE;

	BUFF_EFFECT_INFO* lpBuff = 0;
	lpBuff = g_EffectAddOption.GetEffectInfo(BuffNumber);
	int Buff = 0;
	
	Buff = g_EffectAddOption.AddEffect(lpObj,BuffNumber,btEffect1,btEffect2,btEffect3,iEffectValue1,iEffectValue2,iEffectValue3,iEffectTime);

	if( Buff != -1 )
	{
		if( Buff > 0 )
		{
			GCSendBuffInfo(lpObj,Buff,2,lpBuff->btOptionType,0,0);
		}
		else
		{
			lpObj->btEffectCount++;
		}

		GCSendBuffInfo(lpObj,BuffNumber,0,lpBuff->btOptionType,btEffect1,iEffectTime,iEffectValue1);
		return TRUE;
	}
return FALSE;
}

BYTE gObjAddEffect(LPOBJ lpObj,int BuffNumber,int iTimeValue)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->m_Index < 0 || lpObj->m_Index > OBJMAX-1 )
		return FALSE;

	int Item = 0;
	LPITEMEFFECT lpEffect = 0;
	BUFF_EFFECT_INFO* lpBuff = 0;

	lpBuff = g_EffectAddOption.GetEffectInfo(BuffNumber);

	if( lpBuff == NULL )
		return FALSE;

	Item = lpBuff->btItemIndex * 512 + lpBuff->btItemNumber;

	if( g_ItemAddOption.IsItemEffect(Item) == FALSE )
	{
		return FALSE;
	}

	lpEffect = g_ItemAddOption.GetItemEffect(Item);
	int Buff = 0;
	
	Buff = g_EffectAddOption.AddEffect(lpObj,BuffNumber,(BYTE)lpEffect->wEffectType1,(BYTE)lpEffect->wEffectType2,0,lpEffect->iEffectValue1,lpEffect->iEffectValue2,0,iTimeValue);

	if( Buff != -1 )
	{
		if( Buff > 0 )
		{
			GCSendBuffInfo(lpObj,Buff,2,lpEffect->wOptionType,0,0);
		}
		else
		{
			lpObj->btEffectCount++;
		}

		GCSendBuffInfo(lpObj,BuffNumber,0,lpEffect->wOptionType,lpEffect->wEffectType1,iTimeValue);
		return TRUE;
	}
return FALSE;
}

BYTE gObjClearEffect(LPOBJ lpObj,int BuffNumber)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->m_Index < 0 || lpObj->m_Index > OBJMAX-1 )
		return FALSE;

	BUFF_EFFECT_INFO* lpBuff = 0;
	lpBuff = g_EffectAddOption.GetEffectInfo(BuffNumber);

	if( lpBuff == NULL )
		return FALSE;

	if( g_EffectAddOption.ClearEffect(lpObj,BuffNumber) == TRUE )
	{
		g_EffectAddOption.RemoveBuffEffect(lpObj,BuffNumber);
		lpObj->btEffectCount--;
		GCSendBuffInfo(lpObj,BuffNumber,1,0,lpBuff->btOptionType,0);
		return TRUE;
	}
return FALSE;
}

BYTE gObjClearGroupEffect(LPOBJ lpObj,int Group)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->m_Index < 0 || lpObj->m_Index > OBJMAX -1)
		return FALSE;

	int RemoveCount = 0;
	int i = 0;

	switch( Group )
	{
	case 7:
		for( i = 0 ; i < MAX_EFFECT_INFO; i++)
		{
			switch( lpObj->pEffectInfo[i].btEffectNum )
			{
			case BUFF_WATCHTOWER:
			case BUFF_SIEGEGUILD1:
			case BUFF_SIEGEGUILD2:
			case BUFF_SIEGEGUILD3:
				if( gObjClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum) == TRUE )
				{
					RemoveCount++;
					i--;
				}
				break;
			}
		}
		break;
	case 5:
		for( i = 0; i < MAX_EFFECT_INFO; i++)
		{
			switch( lpObj->pEffectInfo[i].btEffectNum )
			{
			case BUFF_SEAL_ASCENSION:
			case BUFF_SEAL_WEALTH:
			case BUFF_SEAL_SUSTENANCE:
			case BUFF_SEAL_STRENGTH:
				if( gObjClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum) == TRUE )
				{
					RemoveCount++;
					i--;
				}
				break;
			}
		}
		break;
	case 6:
		for( i = 0; i < MAX_EFFECT_INFO; i++)
		{
			switch( lpObj->pEffectInfo[i].btEffectNum )
			{
			case BUFF_SEAL_ASCENSION:
			case BUFF_SEAL_WEALTH:
			case BUFF_SEAL_SUSTENANCE:
			case BUFF_SEAL_ASCENSION2:
			case BUFF_SEAL_WEALTH2:
			case BUFF_SEAL_SUSTENANCE2:
			case BUFF_SEAL_MOBILITY:
			case BUFF_SCROLL_QUICKNESS:
			case BUFF_SCROLL_DEFENSE:
			case BUFF_SCROLL_WRATH:
			case BUFF_SCROLL_WIZARDRY:
			case BUFF_SCROLL_HEALTH:
			case BUFF_SCROLL_MANA:
			case BUFF_ELIXIR_STR:
			case BUFF_ELIXIR_DEX:
			case BUFF_ELIXIR_VIT:
			case BUFF_ELIXIR_ENE:
			case BUFF_ELIXIR_COM:
			case BUFF_SEAL_HEALING:
			case BUFF_SEAL_DIVINITY:
			case BUFF_SCROLL_BATTLE:
			case BUFF_SCROLL_STRENGTH:
			case BUFF_TALISMAN_GUARDIAN:
			case BUFF_TALISMAN_ITEMPROTECT:
			case BUFF_SEAL_ASCENSIONM:
			case BUFF_SEAL_WEALTHM:
			case BUFF_SEAL_STRENGTH:
				break;
			default:
				if( gObjClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum) == TRUE )
				{
					RemoveCount++;
					i--;
				}
				break;
			}
		}
		break;
	case 2:
		for( i = 0; i < MAX_EFFECT_INFO; i++)
		{
			if( gObjClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum) == TRUE )
			{
				RemoveCount++;
				i--;
			}
		}
		lpObj->btEffectCount = 0;
		break;
	case 3:
		for( i = 0; i < MAX_EFFECT_INFO; i++)
		{
			switch( lpObj->pEffectInfo[i].btEffectNum )
			{
			case BUFF_WEAKNESS:
			case BUFF_WEAKNESS2:
			case BUFF_INNOVATION	:
			case BUFF_INNOVATION2:
			case BUFF_BLIND:
			case BUFF_BLIND2:
			case BUFF_DECREASE_DEFRATE:
			case BUFF_DECREASE_DEFRATE2:
			case BUFF_CONFUSION:
				if( gObjClearEffect(lpObj,lpObj->pEffectInfo[i].btEffectNum) == TRUE )
				{
					RemoveCount++;
					i--;
				}
				break;
			}
		}
		break;
	default:
		RemoveCount = g_EffectAddOption.GetClearCount(lpObj,Group);
		break;
	}

	if( lpObj->btEffectCount < 0 || lpObj->btEffectCount > MAX_EFFECT_INFO )
	{
		lpObj->btEffectCount = 0;
	}

	gObjSendAllEffect(lpObj);
	return TRUE;
}

BYTE gObjIsActiveEffect(LPOBJ lpObj,int BuffNumber)
{
	if( lpObj == NULL )
		return FALSE;
	if( lpObj->m_Index < 0 || lpObj->m_Index > OBJMAX-1)
		return FALSE;

return g_EffectAddOption.IsEffectActive(lpObj,BuffNumber);
}

BYTE gObjAddEffectTime(LPOBJ lpObj,int BuffNumber,int iTimeValue)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->Connected < PLAYER_PLAYING )
		return FALSE;

	int iTICK_COUNT = GetTickCount();

	for(int i = 0; i < MAX_EFFECT_INFO;i++)
	{
		if( i > lpObj->btEffectCount )
		{
			return FALSE;
		}

		if( lpObj->pEffectInfo[i].btEffectNum == BuffNumber )
		{
			lpObj->pEffectInfo[i].iTimeValue += iTimeValue;

			if( lpObj->pEffectInfo[i].iTimeValue < 0 )
			{
				gObjClearEffect(lpObj,BuffNumber);
			}
			return TRUE;
		}
	}
return FALSE;
}

int gObjGetEffectList(LPOBJ lpObj,LPBYTE BuffList)
{
	if( lpObj == NULL )
		return -1;

	if( lpObj->Connected < PLAYER_PLAYING )
		return -1;

	int EffectCount = 0;

	for(int i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( EffectCount >= MAX_EFFECT_INFO ||
			EffectCount >= lpObj->btEffectCount )
		{
			break;
		}

		if( lpObj->pEffectInfo[i].btEffectNum == NULL )
			continue;
		switch( lpObj->pEffectInfo[i].btEffectNum )
		{
		case 0:
			break;
		default:
			BuffList[EffectCount] = lpObj->pEffectInfo[i].btEffectNum;
			EffectCount++;
			break;
		}
	}
return EffectCount;
}

int gObjGetEffectValue(LPOBJ lpObj,int EffectType)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->Connected < PLAYER_PLAYING )
		return FALSE;
	int EffectValue = 0;

	for(int i = 0; i < MAX_EFFECT_INFO;i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum == NULL )
			continue;

		if( lpObj->pEffectInfo[i].btEffectType1 == EffectType )
		{
			EffectValue += lpObj->pEffectInfo[i].iEffectValue1;
		}

		if( lpObj->pEffectInfo[i].btEffectType2 == EffectType )
		{
			EffectValue += lpObj->pEffectInfo[i].iEffectValue2;
		}

		if( lpObj->pEffectInfo[i].btEffectType3 == EffectType )
		{
			EffectValue += lpObj->pEffectInfo[i].iEffectValue3;
		}
	}
return EffectValue;
}

BYTE gObjGetEffectValue(LPOBJ lpObj,int BuffNumber,int* EffectValue1,int* EffectValue2,int* EffectValue3)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->Connected < PLAYER_PLAYING )
		return FALSE;

	for(int i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum == BuffNumber )
		{
			*EffectValue1 = lpObj->pEffectInfo[i].iEffectValue1;

			if( EffectValue2 != 0 )
			{
				*EffectValue2 = lpObj->pEffectInfo[i].iEffectValue2;
			}

			if( EffectValue3 != 0 )
			{
				*EffectValue3 = lpObj->pEffectInfo[i].iEffectValue3;
			}

			return TRUE;
		}
	}

	return FALSE;
}

void GCSendBuffInfo(LPOBJ lpObj,BYTE EffectNumber,BYTE bUseType,WORD wOptionType,WORD iEffectType,int iTimeValue,int iEffectValue)
{
	PMSG_SENDEFFECT pMsg;
	PHeadSetB((LPBYTE)&pMsg,0x2D,sizeof(pMsg));

	pMsg.iEffect = EffectNumber;
	pMsg.byEffectOption = bUseType;
	pMsg.wOptionType = wOptionType;
	pMsg.wEffectType = iEffectType;
	pMsg.iLeftTime = iTimeValue;
	pMsg.iEffectValue = iEffectValue;

	if( lpObj->Type == OBJ_USER )
	{
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	}

	if( bUseType == 0 )
	{
		GCStateInfoSend(lpObj,1,EffectNumber);
	}
	else
	{
		GCStateInfoSend(lpObj,0,EffectNumber);
	}
}

BYTE gObjCheckEffect(LPOBJ lpObj,int BuffNumber,int EffectValue1,int EffectValue2,int EffectValue3)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->Connected < PLAYER_PLAYING )
		return FALSE;

	for(int i = 0; i < MAX_EFFECT_INFO;i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum == BuffNumber )
		{
			if( lpObj->pEffectInfo[i].iEffectValue1 >= EffectValue1 )
			{
				return TRUE;
			}

			if( lpObj->pEffectInfo[i].iEffectValue2 >= EffectValue2 )
			{
				return TRUE;
			}

			if( lpObj->pEffectInfo[i].iEffectValue3 >= EffectValue3 )
			{
				return TRUE;
			}
		}
	}

return FALSE;
}

BOOL gObjSendGroupEffect(LPOBJ lpObj,int Dis,int& Count,int BuffNumber,int BuffEffect1,int EffectVal1,int BuffEffect2,int EffectVal2,int BuffEffect3,int EffectVal3,int Time)
{
	if( lpObj == NULL )
		return false;

	for(int n = OBJ_STARTUSERINDEX; n < OBJMAX; n++)
	{
		LPOBJ lpTargetObj = &gObj[n];

		if( lpTargetObj->Type == OBJ_USER && 
			gObjIsConnected(n) && 
			lpTargetObj->MapNumber == lpObj->MapNumber && 
			gObjCalDistance(lpObj,lpTargetObj) < Dis)
		{
			gObjAddEffect(lpTargetObj,BuffNumber,BuffEffect1,EffectVal1,BuffEffect2,EffectVal2,BuffEffect3,EffectVal3,Time);
			Count++;
		}
	}
return true;
}

void gObjSendAllEffect(LPOBJ lpObj)
{
	if( lpObj == NULL )
		return;
	if( lpObj->Type != OBJ_USER )
		return;

	if( lpObj->btEffectCount <= 0 )
		return;

	for(int i = 0; i < MAX_EFFECT_INFO; i++)
	{
		if( lpObj->pEffectInfo[i].btEffectNum != NULL )
		{
			GCStateInfoSend(lpObj,1,lpObj->pEffectInfo[i].btEffectNum);
		}
	}
}

void gObjExpHandle(LPOBJ lpObj,__int64* Exp,int bMaster)
{
	__int64 TmpExp = *Exp;

	int iEffectValue = 100;

	if( gObjIsActiveEffect(lpObj,BUFF_SEAL_SUSTENANCE2) || gObjIsActiveEffect(lpObj,BUFF_SEAL_SUSTENANCE) )
	{
		TmpExp = 0;
	}
	else if( gObjIsActiveEffect(lpObj,BUFF_SEAL_ASCENSION2) ||
			 gObjIsActiveEffect(lpObj,BUFF_SEAL_WEALTH2) ||
			 gObjIsActiveEffect(lpObj,BUFF_SEAL_ASCENSION) ||
			 gObjIsActiveEffect(lpObj,BUFF_SEAL_WEALTH) ||
			 gObjIsActiveEffect(lpObj,BUFF_SEAL_HEALING) ||
			 gObjIsActiveEffect(lpObj,BUFF_SEAL_ASCENSIONM) ||
			 gObjIsActiveEffect(lpObj,BUFF_SEAL_WEALTHM) )
	{

		iEffectValue = gObjGetEffectValue(lpObj,ADD_OPTION_EXPERIENCE);

		int bEnabledMaster = g_MasterExp.IsEnabled(lpObj);

		if( bEnabledMaster )
		{
			if( gObjIsActiveEffect(lpObj,BUFF_SEAL_ASCENSIONM) ||
				gObjIsActiveEffect(lpObj,BUFF_SEAL_WEALTHM) )
			{
				if( bMaster != 0 )
				{
					iEffectValue = 100;
				}
			}
			else
			{
				iEffectValue = 100;
			}
		}
		else
		{
			if( gObjIsActiveEffect(lpObj,BUFF_SEAL_ASCENSIONM) ||
				gObjIsActiveEffect(lpObj,BUFF_SEAL_WEALTHM) )
			{
				iEffectValue = 100;
			}
			else if( bMaster != 0 )
			{
				iEffectValue = 100;
			}
		}
	}


#if (CUSTOM_OLYMP!=1)
	if( gObjIsActiveEffect(lpObj,BUFF_GLADIATOR_HONOR) && lpObj->MapNumber == MAP_INDEX_VULCAN )
	{
		int bEnabledMaster = g_MasterExp.IsEnabled(lpObj);

		if( !bEnabledMaster )
		{
			iEffectValue += 20;
		}
	}
#endif

	if( gObjPandaSprite(lpObj) == TRUE )
	{
		iEffectValue += 50;
	}
	else if( gObjSkeletonSprite(lpObj) == TRUE )
	{
		if( lpObj->m_Change == 548 )
		{
			iEffectValue += 30;
		}

		iEffectValue += 30;
	}

	if( iEffectValue != 100 )
	{
		TmpExp = TmpExp * iEffectValue / 100;
	}

#if ( __CUSTOM__ == 1)
	TmpExp = gCustomExp.GetResetExp(lpObj,TmpExp);
#endif

	*Exp = TmpExp;
}

// BUFF_EFFECT_CLASS 

CBuffEffectInfo g_BuffEffect;

CBuffEffectInfo::CBuffEffectInfo()
{

}

CBuffEffectInfo::~CBuffEffectInfo()
{

}

void CBuffEffectInfo::Init()
{
	this->m_Count = 0;

	for(int i = 0; i < MAX_BUFF_EFFECT; i++)
	{
		this->m_BuffData[i].btNumber = -1;
		this->m_BuffData[i].btType = 0;
		this->m_BuffData[i].btUnk = 0;
		this->m_BuffData[i].btClearType = 0;
		this->m_BuffData[i].BuffComment[0] = 0;
	}
}

BYTE CBuffEffectInfo::LoadData(char* szFileName)
{
	Init();

	SMDFile = fopen(szFileName,"r");

	if( SMDFile == NULL )
	{
		return FALSE;
	}

	int Token;
	int Index = 0;

	BYTE Number = 0;
	BYTE Option = 0;
	BYTE ItemIndex = 0;
	BYTE ItemNumber = 0;
	BYTE Type = 0;
	BYTE Type2 = 0;
	BYTE ClearType = 0;

	char BuffName[20];
	char BuffComment[100];

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		if( Token == NUMBER )
		{
			Index = (int)TokenNumber;

			while( TRUE )
			{
				Token = GetToken();

				if( strcmp("end",TokenString) == NULL )
				{
					break;
				}

				Number = (BYTE)TokenNumber;

				Token = GetToken();
				Option = (BYTE)TokenNumber;

				Token = GetToken();
				ItemIndex = (BYTE)TokenNumber;
				
				Token = GetToken();
				ItemNumber = (BYTE)TokenNumber;

				Token = GetToken();
				memcpy(BuffName,TokenString,sizeof(BuffName));

				Token = GetToken();
				Type = (BYTE)TokenNumber;

				Token = GetToken();
				Type2 = (BYTE)TokenNumber;

				Token = GetToken();
				ClearType = (BYTE)TokenNumber;

				Token = GetToken();
				memcpy(BuffComment,TokenString,sizeof(BuffComment));

				AddBuffInfo(Number,Option,ItemIndex,ItemNumber,BuffName,Type,Type2,ClearType,BuffComment);
			}
		}
	}

	fclose(SMDFile);
return TRUE;
}

BYTE CBuffEffectInfo::AddBuffInfo(BYTE btNumber,BYTE btOptionType,BYTE btItemIndex,BYTE btItemNumber,char* szName,BYTE btType,BYTE btType2,BYTE btClearType,char* szComment)
{
	this->m_BuffData[m_Count].btNumber = btNumber;
	this->m_BuffData[m_Count].btOptionType = btOptionType;
	this->m_BuffData[m_Count].btItemIndex = btItemIndex;
	this->m_BuffData[m_Count].btItemNumber = btItemNumber;
	this->m_BuffData[m_Count].btType = btType;
	this->m_BuffData[m_Count].btUnk = btType2;
	this->m_BuffData[m_Count].btClearType = btClearType;

	memcpy(this->m_BuffData[m_Count].BuffName,szName,sizeof(this->m_BuffData->BuffName));
	memcpy(this->m_BuffData[m_Count].BuffComment,szComment,sizeof(this->m_BuffData->BuffComment));

	m_Count++;

	return TRUE;
}

BYTE CBuffEffectInfo::IsBuff(BYTE BuffNumber)
{
	if( BuffNumber < 0 || BuffNumber > MAX_BUFF_EFFECT )
	{
		return FALSE;
	}
return TRUE;
}

BUFF_EFFECT_INFO* CBuffEffectInfo::SearchBuff(BYTE BuffNumber)
{
	int szTmp = 0;

	if( IsBuff(BuffNumber) == FALSE )
	{
		return NULL;
	}

	for(int i = 0; i < MAX_BUFF_EFFECT; i++)
	{
		if( this->m_BuffData[i].btNumber == BuffNumber )
		{
			return &this->m_BuffData[i];
		}
	}
return NULL;
}
