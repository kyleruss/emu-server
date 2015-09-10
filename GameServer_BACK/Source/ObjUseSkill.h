#ifndef OBJUSESKILL_H
#define OBJUSESKILL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicInf.h"
#include "user.h"

class CObjUseSkill
{

public:

	CObjUseSkill();
	virtual ~CObjUseSkill();

	int SpeedHackCheck(int aIndex);
	int EnableSkill(int Skill);
	int CheckWeapon(LPOBJ lpObj,int Skill);
	int GetUseMana(int aIndex, CMagicInf* lpMagic);
	int GetAddUseMana(LPOBJ lpObj, CMagicInf* lpMagic);
	int GetUseBP(int aIndex, CMagicInf* lpMagic);
	void UseSkill(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	void UseSkill(int aIndex, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int RunningSkill(int aIndex, int aTargetIndex, CMagicInf* lpMagic, BOOL bCombo);
	void SkillFrustrum3(int aIndex,BYTE aTargetPos,float f1,float f2,float f3,float f4);

	void MaGumSkillDefenseDown(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	void KnightSkillAddLife(int aIndex, int skill_level,CMagicInf* lpMagic);
	void WizardMagicDefense(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	void SkillDefense(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	void SkillAttack(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillChangeUse(int aIndex);
	void SkillHealing(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillMonsterCall(int aIndex, int MonsterType, int x, int y);
	int SkillDeathPoison(int aIndex, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillSuddenIce(int aIndex, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillHellFire2Start(int aIndex, CMagicInf* lpMagic);
	int SkillHellFire2(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	void SkillBlowOfFury(int aIndex, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex, BOOL isCombo);
	void SkillWheel(int aIndex, CMagicInf* lpMagic, int aTargetIndex, BOOL isCombo);
	int SkillKnightBlow(int aIndex, int aTargetIndex, CMagicInf* lpMagic, BOOL isCombo);
	void SkillPowerSlash(int aIndex, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE Targetangle, int aTargetIndex);
	void SkillFrustrum(int aIndex, float fangle, float tx, float ty);
	int GetTargetLinePath(int sx, int sy, int tx, int ty,  int* PathX,int* PathY, int distance);
	int GetAngle(int x, int y, int tx,  int ty);
	int CalDistance(int x, int y, int x1, int y1);
	void SkillFrustrum2(int aIndex, float fangle, float tx, float ty);
	int SkillAreaCheck(int angel, int x, int y,  int tx, int ty,  int dis,  int arc);
	int SkillSpear(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillFireBurst(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillDarkHorseAttack(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int RecallPartyCheck(int aIndex, int skill_level);
	void SkillRecallParty(int aIndex, int skill_level);
	void SkillAddCriticalDamage(int aIndex, CMagicInf* lpMagic);
	void SkillElectricSpark(int aIndex, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillSummon(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillImmuneToMagic(int aIndex, CMagicInf* lpMagic);
	int SkillImmuneToHarm(int aIndex, CMagicInf* lpMagic);
	int SkillKnightRush(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillWizardJavalin(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillElfRush(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillMagumOneFlash(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillMagumDeathCannon(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillDarklordSpaceSplit(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillBrandOfSkill(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveStun(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillAddMana(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillCloaking(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveCloaking(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveMagic(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int RemoveCloakingEffect(int aIndex);
	int SkillStun(int aIndex, int aTargetIndex, CMagicInf* lpMagic, BYTE btX, BYTE btY, BYTE btDir, BYTE btTargetPos);
	int RemoveAllCharacterInvalidMagicAndSkillState(LPOBJ lpObj);
	int RemoveSpecificSkillState(LPOBJ lpObj, int iSkillNumber);
	int SkillFenrirAttack(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillInfinityArrow(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int SkillFireScream(int aIndex, int aTargetIndex, CMagicInf* lpMagic);
	int FireScreamExplosionAttack(LPOBJ lpObj, LPOBJ lpTargetObj, int iAttackDamage);
	void SpecificSkillAdditionTreat(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf* lpMagic, int iAttackDamage);

	int SkillDrainLife(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	int SkillChainLightning(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	int SkillLightExplode(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	int SkillLightExplode(LPOBJ lpObj,LPOBJ lpTargetObj,int SkillNumber,int AttackDamage,int Distance,int DmgPerc);
	int SkillCurse(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex); 
	int SkillDamageReflect(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	int SkillSleep(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	int SkillBlind(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	int SkillWeakness(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex); 
	int SkillInnovation(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex); 

	BOOL SkillAreaMonsterAttack(int aIndex,CMagicInf *lpMagic,BYTE x,BYTE y,int aTargetIndex,int aDistance,int bPVP,int nDelay);
	BOOL SkillLagle(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex,int type);
	BOOL SkillBerserker(int aIndex,CMagicInf* lpMagic);
	void SkillRecovery(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	BOOL SkillExpWizard(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	void SkillDestruct(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex);
	BOOL SkillLightShock(int aIndex,int aTargetIndex, CMagicInf * lpMagic);
	void SkillFlameStrike(int aIndex, CMagicInf * lpMagic,BYTE targetangle, int aTargetIndex);
	void SkillGiganticStorm(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex);
	void SkillMultiShot(int aIndex,CMagicInf* lpMagic,BYTE targetpos,int aTargetIndex);
	void SkillChaoticDesier(int aIndex, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);

	int	SkillEvilMonster(int aIndex, int aTargetIndex, CMagicInf * lpMagic);							
	int	SkillDeathPoisonForMeDusa(int aIndex, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void SkillMonkBarrageJustOneTarget(int aIndex,CMagicInf * lpMagic,int aTargetIndex);
	int SkillMonkDarkSideGetTargetIndex(int aIndex, int aTargetIndex, CMagicInf * lpMagic, WORD * wTargetList);
	int SkillMonkBuff(int aIndex,CMagicInf * lpMagic);
	int SkillMonkBuffApplyParty(int aIndex,CMagicInf * lpMagic);
	int SkillCharge(int aIndex,CMagicInf * lpMagic,int aTargetIndex);

	void SkillBloodStorm(int aIndex,int aTargetIndex,CMagicInf* lpMagic,int bCombo);
	int BloodStormExplosionAttack(LPOBJ lpObj, LPOBJ lpTargetObj, int iAttackDamage);
	void SkillEarthPrison(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	int SkillBless(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
	void SkillPartyHeal(int aIndex,CMagicInf* lpMagic);
	void SkillIronDefense(int aIndex,int aTargetIndex,CMagicInf* lpMagic);
};

int  DecPartyMemberHPandMP(LPOBJ lpObj);
extern CObjUseSkill gObjUseSkill;

#endif