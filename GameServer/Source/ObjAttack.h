#ifndef OBJATTACK_H
#define OBJATTACK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicInf.h"
#include "ObjBaseAttack.h"

class CObjAttack : public CObjBaseAttack
{

public:

	CObjAttack();
	virtual ~CObjAttack();

	BOOL Attack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf* lpMagic,  int magicsend, unsigned char MSBFlag, int AttackDamage, BOOL bCombo,BYTE byBarrageCount, BYTE byReflect);
	int GetAttackDamage(LPOBJ lpObj, int targetDefense, int& effect, BOOL bIsOnDuel, CMagicInf* lpMagic);
	int GetAttackDamageWizard(LPOBJ lpObj, int targetDefense, CMagicInf* lpMagic, int& effect, BOOL bIsOnDuel);
	int GetAttackDamageSummoner(LPOBJ lpObj, int targetDefense, CMagicInf* lpMagic, int& effect, BOOL bIsOnDuel);
	int GetShieldDamage(LPOBJ lpObj, LPOBJ lpTargetObj, int iAttackDamage);

	bool GetBuffTypePhysicalIncrease(LPOBJ lpObj,int& iAttackBerserkerMin,int& iAttackBerserkerMax,int nBuffType);

};

BOOL gObjDenorantSprite(LPOBJ lpObj);
BOOL gObjDarkHorse(LPOBJ lpObj);
BOOL gObjFenrir(LPOBJ lpObj);

extern CObjAttack gclassObjAttack;

#endif