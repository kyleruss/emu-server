#pragma once

#include "BuffScriptLoader.h"
#include "user.h"

#define BUFF_DAMAGE_EFFECT	18
#define BUFF_POISON_EFFECT	19
#define BUFF_ABSORB_EFFECT	104

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBuffEffect  
{
public:
	CBuffEffect();
	virtual ~CBuffEffect();

	void SetBuffEffect(LPOBJ lpObj,BYTE EffectType,int EffectValue);
	void ClearBuffEffect(LPOBJ lpObj,BYTE EffectType,int EffectValue);
	void SetActiveBuffEffect(LPOBJ lpObj,BYTE EffectType,int EffectValue);
	void GiveDamageEffect(LPOBJ lpObj,int Damage);
	void PoisonEffect(LPOBJ lpObj,BYTE PoisonRate);
	void AbsorbHP(LPOBJ lpObj,int iREDUCE_HP);
	void SetPrevEffect(LPOBJ lpObj);
	void SetNextEffect(LPOBJ lpObj);
	void ClearAllBuffEffect(LPOBJ lpObj);
};

extern CBuffEffect g_BuffEffect;