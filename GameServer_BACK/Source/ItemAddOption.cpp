// ItemAddOption.cpp: implementation of the CItemAddOption class.
//	GS-N	1.00.18	JPN	0x00508C40	-	Completed
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemAddOption.h"
#include "LogProc.h"
#include "CashItemPeriodSystem.h"
#include "..\include\Readscript.h"

CItemAddOption g_ItemAddOption;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemAddOption::CItemAddOption()
{
	this->Initialize();
}

CItemAddOption::~CItemAddOption()
{
	return;
}



void CItemAddOption::Load(PCHAR chFileName)
{
	SMDToken Token;

	this->Initialize();

	SMDFile = fopen(chFileName, "r");

	if ( SMDFile == NULL )
		return;

	int iItemAddOptionIndex = 0;

	while ( true )
	{
		Token = (SMDToken)GetToken();

		if ( Token == END )
			break;

		if ( Token == NUMBER )
		{
			int iType = (int)TokenNumber;

			while ( true )
			{
				if ( iType == 0 )
				{
					Token = (SMDToken)GetToken();

					if ( strcmp("end", TokenString) == 0 )
						break;
				}

				if ( iType >= 1 )
				{
					int iItemNumber = 0;
					int iItemType = 0;
					int iItemIndex = 0;
					int iItemOption1 = 0;
					int iItemValue1 = 0;
					int iItemOption2 = 0;
					int iItemValue2 = 0;
					int iItemValidTime = 0;

					Token = (SMDToken)GetToken();

					if ( !strcmp("end", TokenString))
						break;

					iItemType = (int)TokenNumber;

					Token = (SMDToken)GetToken();
					iItemIndex = (int)TokenNumber;

					Token = (SMDToken)GetToken();
					iItemOption1 = (int)TokenNumber;					

					Token = (SMDToken)GetToken();
					iItemValue1 = (int)TokenNumber;

					Token = (SMDToken)GetToken();
					iItemOption2 = (int)TokenNumber;

					Token = (SMDToken)GetToken();
					iItemValue2 = (int)TokenNumber;

					Token = (SMDToken)GetToken();
					iItemValidTime = (int)TokenNumber;


					iItemNumber = ITEMGET(iItemType, iItemIndex);
					m_ItemAddOption[iItemAddOptionIndex].wOptionType = iType;
					m_ItemAddOption[iItemAddOptionIndex].iItemNumber = iItemNumber;
					m_ItemAddOption[iItemAddOptionIndex].iItemType = iItemType;
					m_ItemAddOption[iItemAddOptionIndex].iItemIndex = iItemIndex;
					m_ItemAddOption[iItemAddOptionIndex].wEffectType1 = iItemOption1;
					m_ItemAddOption[iItemAddOptionIndex].iEffectValue1 = iItemValue1;
					m_ItemAddOption[iItemAddOptionIndex].wEffectType2 = iItemOption2;
					m_ItemAddOption[iItemAddOptionIndex].iEffectValue2 = iItemValue2;
					m_ItemAddOption[iItemAddOptionIndex].iEffectValidTime = iItemValidTime;

					iItemAddOptionIndex++;

					if (iItemAddOptionIndex > MAX_ITEM_ADD_OPTION )
					{
						MsgBox("[ItemAddOption] Overflow ItemAddOption Table Index");
					}
				}
			}
		}
	}

	fclose(SMDFile);
}



void CItemAddOption::Initialize()
{
	for ( int i=0;i<MAX_ITEM_ADD_OPTION;i++)
	{
		m_ItemAddOption[i].iItemNumber = 0;
		m_ItemAddOption[i].iItemType = 0;
		m_ItemAddOption[i].iItemIndex = 0;
		m_ItemAddOption[i].wEffectType1 = 0;
		m_ItemAddOption[i].iEffectValue1 = 0;
		m_ItemAddOption[i].wEffectType2 = 0;
		m_ItemAddOption[i].iEffectValue2 = 0;
		m_ItemAddOption[i].iEffectValidTime = 0;
	}
}

BYTE CItemAddOption::ClearItemEffect(LPOBJ lpObj,int iItemCode, int iEffectOption)
{
	LPITEMEFFECT lpItemEffect = NULL;
	
	lpItemEffect = _SearchItemEffect(iItemCode);

	if ( lpItemEffect == NULL )
		return false;

	_ClearItemEffect(lpObj, iEffectOption, lpItemEffect->wOptionType,
		lpItemEffect->wEffectType1, lpItemEffect->iEffectValue1);

	_ClearItemEffect(lpObj, iEffectOption, lpItemEffect->wOptionType,
		lpItemEffect->wEffectType2, lpItemEffect->iEffectValue2);
	return true;
}

BYTE CItemAddOption::SetPeriodItemEffect1(LPOBJ lpObj)
{
	ITEMPERIOD_INFO * PeriodItemInfo = NULL;
	LPITEMEFFECT ItemEffect = NULL;

	PeriodItemInfo = g_CashItemPeriodSystem.GetPeriodInfo(lpObj);

	if(PeriodItemInfo == FALSE)
		return FALSE;

	for(int i = 0; i < SUBITEM_INFO_COUNT; i++)
	{
		if( PeriodItemInfo->ItemPeriod[i].btUsedItem == TRUE )
		{
			ItemEffect = _SearchItemEffect(PeriodItemInfo->ItemPeriod[i].iItemCode);

			if(ItemEffect != NULL)
			{
				switch(ItemEffect->wEffectType1)
				{
				case 4:
				case 5:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
					_SetItemEffect(lpObj,ItemEffect->wEffectType1,ItemEffect->iEffectValue1);
					break;
				}

				switch(ItemEffect->wEffectType2)
				{
				case 4:
				case 5:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
					_SetItemEffect(lpObj,ItemEffect->wEffectType2,ItemEffect->iEffectValue2);
					break;
				}
			}
		}
	}
	return TRUE;
}

BYTE CItemAddOption::SetPeriodItemEffect2(LPOBJ lpObj)
{
	ITEMPERIOD_INFO * PeriodItemInfo = NULL;
	LPITEMEFFECT ItemEffect = NULL;

	PeriodItemInfo = g_CashItemPeriodSystem.GetPeriodInfo(lpObj);

	if(PeriodItemInfo == FALSE)
		return FALSE;

	for(int i = 0; i < SUBITEM_INFO_COUNT; i++)
	{
		if( PeriodItemInfo->ItemPeriod[i].btUsedItem == TRUE )
		{
			ItemEffect = _SearchItemEffect(PeriodItemInfo->ItemPeriod[i].iItemCode);

			if(ItemEffect != NULL)
			{
				switch(ItemEffect->wEffectType1)
				{
				case 1:
				case 4:
				case 5:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
					break;
				default: _SetItemEffect(lpObj,ItemEffect->wEffectType1,ItemEffect->iEffectValue1);
				}

				switch(ItemEffect->wEffectType2)
				{
				case 1:
				case 4:
				case 5:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
					break;
				default: _SetItemEffect(lpObj,ItemEffect->wEffectType2,ItemEffect->iEffectValue2);
				}
			}
		}
	}
	return TRUE;
}

bool CItemAddOption::PrevSetItemLastEffectForHallowin(LPOBJ lpObj)
{
	LPITEMEFFECT lpItemEffect = NULL;
	int iItemEffectType = lpObj->m_wItemEffectType;

	lpItemEffect = this->_SearchItemEffect(iItemEffectType);

	if ( lpItemEffect == NULL )
		return false;

	if ( lpItemEffect->wEffectType1 == ADD_OPTION_LIFE || lpItemEffect->wEffectType1 == ADD_OPTION_MANA )
	{
		this->_SetItemEffect(lpObj, lpItemEffect->wEffectType1, lpItemEffect->iEffectValue1);
	}

	return true;
}

bool CItemAddOption::NextSetItemLastEffectForHallowin(LPOBJ lpObj)
{
	LPITEMEFFECT lpItemEffect = NULL;
	int iItemEffectType = lpObj->m_wItemEffectType;

	lpItemEffect = this->_SearchItemEffect(iItemEffectType);

	if ( lpItemEffect == NULL )
		return false;

	if ( lpItemEffect->wEffectType1 != ADD_OPTION_LIFE && lpItemEffect->wEffectType1 != ADD_OPTION_MANA )
	{
		this->_SetItemEffect(lpObj, lpItemEffect->wEffectType1, lpItemEffect->iEffectValue1);
	}

	return true;
}

void CItemAddOption::SendItemUse(LPOBJ lpObj,int iItemNumber,int iOptionType,int iEffectType,int iEffectValue,int iValidTime)
{
	if( lpObj == NULL )
		return;

	if( iItemNumber == 0 || iEffectType == 0 )
		return;

	GCSendEffectInfo(lpObj,0,iOptionType,iEffectType,iValidTime);
}

bool CItemAddOption::SetItemEffect(LPOBJ lpObj, int iItemNumber, int iItemUseTime)
{
	LPITEMEFFECT lpItemEffectBefore = NULL;
	LPITEMEFFECT lpItemEffect = NULL;

	if ( lpObj == NULL )
		return false;

	if ( lpObj->Type != OBJ_USER )
		return false;

	lpItemEffect = _SearchItemEffect(iItemNumber);

	if ( lpItemEffect ==NULL )
		return false;

	lpItemEffectBefore = GetCurrentEffectType(lpObj);

	if ( lpItemEffectBefore != NULL )
	{
		if ( lpItemEffectBefore->wOptionType == lpItemEffect->wOptionType )
		{
			ClearItemEffect(lpObj, 2);
		}
	}

	if ( iItemUseTime > 0 )
	{
		_SetItemEffect(lpObj, lpItemEffect->wEffectType1, lpItemEffect->iEffectValue1);
		SendItemUse(lpObj,iItemNumber,lpItemEffect->wOptionType,lpItemEffect->wEffectType1,lpItemEffect->iEffectValue1,iItemUseTime * 60);
		_SetItemEffect(lpObj, lpItemEffect->wEffectType2, lpItemEffect->iEffectValue2);
		SendItemUse(lpObj,iItemNumber,lpItemEffect->wOptionType,lpItemEffect->wEffectType2,lpItemEffect->iEffectValue2,iItemUseTime * 60);
	}
	else
	{
		_SetItemEffect(lpObj, lpItemEffect->wEffectType1, lpItemEffect->iEffectValue1);
		SendItemUse(lpObj,iItemNumber,lpItemEffect->wOptionType,lpItemEffect->wEffectType1,lpItemEffect->iEffectValue1,lpItemEffect->iEffectValidTime);
		_SetItemEffect(lpObj, lpItemEffect->wEffectType2, lpItemEffect->iEffectValue2);
		SendItemUse(lpObj,iItemNumber,lpItemEffect->wOptionType,lpItemEffect->wEffectType2,lpItemEffect->iEffectValue2,lpItemEffect->iEffectValidTime);
	}

	if ( lpItemEffect->wOptionType == 1  )
	{
		if(iItemUseTime > 0)
		{
			lpObj->m_wItemEffectType = lpItemEffect->iItemNumber;
			lpObj->m_iItemEffectValidTime = iItemUseTime * 60;
		}
		else
		{
			lpObj->m_wItemEffectType = lpItemEffect->iItemNumber;
			lpObj->m_iItemEffectValidTime = lpItemEffect->iEffectValidTime;
		}
	}
	return true;
}




bool CItemAddOption::_SetItemEffect(LPOBJ lpObj, int iEffectType, int iEffectValue)
{
	if ( iEffectType <= 0 )
		return false;

	switch ( iEffectType )
	{
		case ADD_OPTION_LIFE:
			lpObj->AddLife += iEffectValue;
			GCReFillSend(lpObj->m_Index, (WORD)(lpObj->MaxLife + lpObj->AddLife), -2, 0, lpObj->iMaxShield + lpObj->iAddShield);
			GCReFillSend(lpObj->m_Index, (WORD)lpObj->Life, -1, 0, lpObj->iShield);
			break;

		case ADD_OPTION_MANA:
			lpObj->AddMana += iEffectValue;
			GCManaSend(lpObj->m_Index, (WORD)(lpObj->AddMana + lpObj->MaxMana), -2, 0, lpObj->MaxBP + lpObj->AddBP);
			GCManaSend(lpObj->m_Index, (WORD)lpObj->Mana, -1, 0, lpObj->BP);
			break;

		case ADD_OPTION_ATTACK_DAMAGE:
			lpObj->m_AttackDamageMaxLeft += iEffectValue;
			lpObj->m_AttackDamageMinLeft += iEffectValue;
			lpObj->m_AttackDamageMaxRight += iEffectValue;
			lpObj->m_AttackDamageMinRight += iEffectValue;
			lpObj->m_MagicDamageMin += iEffectValue;
			lpObj->m_MagicDamageMax += iEffectValue;
			break;
		
		case ADD_OPTION_DEFENSE:
			lpObj->m_Defense += iEffectValue * 10 / 20;
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
		case ADD_OPTION_WEAPON_DAMAGE:
			lpObj->m_AttackDamageMaxLeft += iEffectValue;
			lpObj->m_AttackDamageMinLeft += iEffectValue;
			lpObj->m_AttackDamageMaxRight += iEffectValue;
			lpObj->m_AttackDamageMinRight += iEffectValue;
			break;
		case ADD_OPTION_MAGIC_DAMAGE:
			lpObj->m_MagicDamageMin += iEffectValue;
			lpObj->m_MagicDamageMax += iEffectValue;
			break;
		case ADD_OPTION_UNK:
			break;
		default:
			return false;
	}

	return true;
}




bool CItemAddOption::_ClearItemEffect(LPOBJ lpObj, int iEffectOption, int iOptionType, int iEffectType, int iEffectValue)
{
	if ( iEffectType <= 0 )
		return false;

	switch ( iEffectType )
	{
		case ADD_OPTION_LIFE:
			lpObj->AddLife -= iEffectValue;

			if ( (lpObj->AddLife + lpObj->MaxLife ) < lpObj->Life )
				lpObj->Life = lpObj->AddLife + lpObj->MaxLife;

			GCReFillSend(lpObj->m_Index, (WORD)lpObj->Life, 0xFF, 0, lpObj->iShield);
			GCReFillSend(lpObj->m_Index, (WORD)(lpObj->MaxLife + lpObj->AddLife), 0xFE, 0, lpObj->iMaxShield + lpObj->iAddShield);
			break;

		case ADD_OPTION_MANA:
			lpObj->AddMana -= iEffectValue;

			if ( (lpObj->AddMana + lpObj->MaxMana ) < lpObj->Mana )
				lpObj->Mana = lpObj->AddMana + lpObj->MaxMana;


			GCManaSend(lpObj->m_Index, (WORD)lpObj->Mana, -1, 0, lpObj->BP);
			GCManaSend(lpObj->m_Index, (WORD)(lpObj->AddMana + lpObj->MaxMana), 0xFE, 0, lpObj->MaxBP + lpObj->AddBP);
			break;

		case ADD_OPTION_ATTACK_DAMAGE:
			lpObj->m_AttackDamageMaxLeft -= iEffectValue;
			lpObj->m_AttackDamageMinLeft -= iEffectValue;
			lpObj->m_AttackDamageMaxRight -= iEffectValue;
			lpObj->m_AttackDamageMinRight -= iEffectValue;
			lpObj->m_MagicDamageMin -= iEffectValue;
			lpObj->m_MagicDamageMax -= iEffectValue;
			break;

		case ADD_OPTION_DEFENSE:
			lpObj->m_Defense -= iEffectValue * 10 / 20;
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
		case ADD_OPTION_WEAPON_DAMAGE:
			lpObj->m_AttackDamageMaxLeft -= iEffectValue;
			lpObj->m_AttackDamageMinLeft -= iEffectValue;
			lpObj->m_AttackDamageMaxRight -= iEffectValue;
			lpObj->m_AttackDamageMinRight -= iEffectValue;
			break;
		case ADD_OPTION_MAGIC_DAMAGE:
			lpObj->m_MagicDamageMin -= iEffectValue;
			lpObj->m_MagicDamageMax -= iEffectValue;
			break;
		case ADD_OPTION_UNK:
			break;
		default:
			return false;
	}

	GCSendEffectInfo(lpObj,iEffectOption,iOptionType,iEffectType,0);

	return true;
}

bool CItemAddOption::ClearItemEffect(LPOBJ lpObj, int iEffectOption)
{
	LPITEMEFFECT lpItemEffect = NULL;
	
	lpItemEffect = GetCurrentEffectType(lpObj);

	if ( lpItemEffect == NULL )
		return false;

	_ClearItemEffect(lpObj, iEffectOption, lpItemEffect->wOptionType,
		lpItemEffect->wEffectType1, lpItemEffect->iEffectValue1);

	_ClearItemEffect(lpObj, iEffectOption, lpItemEffect->wOptionType,
		lpItemEffect->wEffectType2, lpItemEffect->iEffectValue2);

	lpObj->m_wItemEffectType = 0;
	lpObj->m_iItemEffectValidTime = 0;

	return true;
}


LPITEMEFFECT CItemAddOption::GetCurrentEffectType(LPOBJ lpObj)
{
	if ( lpObj->m_wItemEffectType <= 0 )
		return NULL;

	return _SearchItemEffect(lpObj->m_wItemEffectType);
}




LPITEMEFFECT CItemAddOption::_SearchItemEffect(int iItemNumber)
{
	for ( int i=0;i<MAX_ITEM_ADD_OPTION;i++)
	{
		if ( m_ItemAddOption[i].iItemNumber == iItemNumber )
			return &m_ItemAddOption[i];
	}

	return NULL;
}



bool CItemAddOption::SearchItemEffectType(int iItemNumber, int * iEffectType1, int * iEffectType2,int * iItemPeriodDate)
{
	LPITEMEFFECT lpItemEffect = NULL;

	lpItemEffect = _SearchItemEffect(iItemNumber);

	if ( lpItemEffect == NULL )
		return false;

	*iEffectType1 = lpItemEffect->wEffectType1;
	*iEffectType2 = lpItemEffect->wEffectType2;
	*iItemPeriodDate = lpItemEffect->iEffectValidTime;

	return true;
}

int CItemAddOption::SearchItemOption(int iItemNumber)
{
	LPITEMEFFECT lpItemEffect = NULL;
	int ItemOption = -1;

	lpItemEffect = _SearchItemEffect(iItemNumber);

	if(lpItemEffect == NULL)
		return ItemOption;

	ItemOption = lpItemEffect->wOptionType;
return ItemOption;
}

BYTE CItemAddOption::IsItemEffect(int iItemCode)
{
	if( _SearchItemEffect(iItemCode) == NULL )
		return FALSE;
return TRUE;
}

LPITEMEFFECT CItemAddOption::GetItemEffect(int iItemCode)
{
	return _SearchItemEffect(iItemCode);
}