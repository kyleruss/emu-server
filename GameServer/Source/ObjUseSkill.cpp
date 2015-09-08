#include "stdafx.h"
#include "ObjUseSkill.h"
#include "GameMain.h"
#include "ChaosCastle.h"
#include "GameServer.h"
#include "user.h"
#include "zzzmathlib.h"
#include "SkillHitBox.h"
#include "MoveCommand.h"
#include "SkillAdditionInfo.h"
#include "CastleSiege.h"
#include "BuffEffectSlot.h"
#include "LogProc.h"
#include "..\common\winutil.h"
#include "MasterSkillSystem.h"
#include "IllusionTempleEvent.h"
#include "ItemAddOption.h"
#include "MasterLevelSkillTreeSystem.h"
#include "BuffSkillEffect.h"
#include "Raklion.h"
#include "RaklionBattleUserMng.h"
#if (ENABLETEST_NEWPVP == 1)
#include "NewPVP.h"
#endif

CObjUseSkill gObjUseSkill;


CObjUseSkill::CObjUseSkill()
{
}

CObjUseSkill::~CObjUseSkill()
{
}



int  CObjUseSkill::GetTargetLinePath(int sx, int sy, int tx, int ty, int * PathX, int * PathY, int distance)
{
	int x,y;
	int delta_x,delta_y;
	int half;

	int error = 0;

	int incx = 1,incy = 1;
	int dis;
	int count;

	if(sx > tx)
	{
		incx = -1;
		delta_x = sx - tx;
	}
	else
	{
		delta_x = tx - sx;
	}

	if(sy > ty)
	{
		incy = -1;
		delta_y = sy - ty;
	}
	else
	{
		delta_y = ty - sy;
	}

	dis = (int)(sqrt((float)(delta_x * delta_x + delta_y * delta_y)));
	dis = distance - dis;

	count = 0;
	PathX[count] = sx;
	PathY[count] = sy;
	count++;

	if(delta_x > delta_y)
	{
		y = sy;
		half = delta_x / 2;

		if(incx > 0)
		{
			for(x = (sx+1); x <= (tx + dis); x++)
			{
				error += delta_y;

				if(error > half)
				{
					y += incy;
					error -= delta_x;
				}

				PathX[count] = x;
				PathY[count] = y;

				count++;
			}
		}
		else
		{
			for(x = (sx-1); x >= (tx - dis);x--)
			{
				error += delta_y;

				if(error > half)
				{
					y += incy;
					error -= delta_x;
				}

				PathX[count] = x;
				PathY[count] = y;

				count++;
			}
		}
	}
	else
	{
		x = sx;
		half = delta_y / 2;

		if(incy > 0)
		{
			for(y = (sy+1); y <= (ty + dis);y++)
			{
				error += delta_x;

				if(error > half)
				{
					x += incx;
					error -= delta_y;
				}

				PathX[count] = x;
				PathY[count] = y;
				count++;
			}
		}
		else
		{
			for(y = (sy-1);y >= (ty - dis);y--)
			{
				error += delta_x;

				if(error > half)
				{
					x += incx;
					error -= delta_y;
				}

				PathX[count] = x;
				PathY[count] = y;
				count++;
			}
		}
	}
	return count;
}




BOOL CObjUseSkill::SpeedHackCheck(int aIndex)
{
	return TRUE;
	LPOBJ lpObj = &gObj[aIndex];


	int iTimeCalc = GetTickCount() - lpObj->m_LastAttackTime;

	if(iTimeCalc < lpObj->m_DetectSpeedHackTime)
	{
		lpObj->m_DetectCount++;
		lpObj->m_SumLastAttackTime += iTimeCalc;

		if(lpObj->m_DetectCount > gHackCheckCount)
		{
			lpObj->m_DetectedHackKickCount++;

			if(gIsKickDetecHackCountLimit != 0 && lpObj->m_DetectedHackKickCount > gDetectedHackKickCount)
			{
				LogAddTD("[%s][%s] %s Kick DetecHackCountLimit Over User (%d) @%d",
					lpObj->AccountID,lpObj->Name,lMsg.Get(lpObj->Class+1900),lpObj->m_DetectedHackKickCount,lpObj->MapNumber);
				CloseClient(aIndex);
				return false;
			}

			lpObj->m_SpeedHackPenalty = gSpeedHackPenalty;
			LogAddTD("[%s][%s] %s Attack Speed Is Wrong MagicSkill (%d)(%d) Penalty %d @%d",
				lpObj->AccountID,lpObj->Name,lMsg.Get(lpObj->Class+1900),lpObj->m_DetectSpeedHackTime,lpObj->m_SumLastAttackTime / lpObj->m_DetectCount,lpObj->m_SpeedHackPenalty,lpObj->MapNumber);
		}
	}
	else
	{
		lpObj->m_SumLastAttackTime = 0;
		lpObj->m_DetectCount = 0;
	}

	lpObj->m_LastAttackTime = GetTickCount();

	if(bIsIgnorePacketSpeedHackDetect != 0 && lpObj->m_SpeedHackPenalty > 0)
	{
		lpObj->m_SpeedHackPenalty--;
		LogAddTD("[%s][%s] %s Apply Attack Speed Penalty (%d left) @%d",
			lpObj->AccountID,lpObj->Name,lMsg.Get(lpObj->Class + 1900),lpObj->m_SpeedHackPenalty,lpObj->MapNumber);
		return false;
	}
	return true;
}

BOOL CObjUseSkill::CheckWeapon(LPOBJ lpObj,int Skill)
{
	switch(Skill)
	{
	case AT_SKILL_WHEEL:
	case AT_SKILL_BLOWOFFURY:
	case AT_SKILL_STRIKE:

		if( lpObj->pInventory[0].IsItem() != TRUE &&
			lpObj->pInventory[1].IsItem() != TRUE )
		{
			return FALSE;
		}

		break;
	}
return TRUE;
}


BOOL CObjUseSkill::EnableSkill(int Skill)
{
	switch( Skill )
	{
	case AT_SKILL_STORM:
	case AT_SKILL_EVIL:
	case AT_SKILL_FLAME:
	case AT_SKILL_HELL:
	case AT_SKILL_BLAST:
	case AT_SKILL_INFERNO:
	case AT_SKILL_CROSSBOW:
	case AT_SKILL_DEVILFIRE:
	case AT_SKILL_FLASH:
	case AT_SKILL_WHEEL:
	case AT_SKILL_SWORD6:
	case AT_SKILL_KNIGHTSPEAR:
	case AT_SKILL_KNIGHTADDLIFE:
	case AT_SKILL_STRIKE:
	case AT_SKILL_ELFHARDEN:
	case AT_SKILL_PENETRATION:
	case AT_SKILL_DEFENSEDOWN:
	case AT_SKILL_MAGICDEFENSE:
	case AT_SKILL_BLOWOFFURY:
	case AT_SKILL_EXPPOISON:
	case AT_SKILL_EXPICE:
	case AT_SKILL_EXPHELL:
	case AT_SKILL_ELECTRICSPARK:
	case AT_SKILL_DARKHORSE_ATTACK:
	case AT_SKILL_BRAND_OF_SKILL:
	case AT_SKILL_STUN:
	case AT_SKILL_REMOVAL_STUN:
	case AT_SKILL_ADD_MANA:
	case AT_SKILL_INVISIBLE:
	case AT_SKILL_REMOVAL_MAGIC:
	case AT_SKILL_FENRIR_ATTACK:
	case AT_SKILL_INFINITY_ARROW:
	case AT_SKILL_FIRESCREAM:
	case AT_SKILL_DRAINLIFE:
	case AT_SKILL_CHAINLIGHTING:
	case AT_SKILL_LIGHTEXPLODE:
	case AT_SKILL_DAMAGEREFLECT:
	case AT_SKILL_BERSERKER:
	case AT_SKILL_SLEEP:
	case AT_SKILL_SAHAMUT:
	case AT_SKILL_NEIL:
	case AT_SKILL_LAGLE:
	case AT_SKILL_WEAKNESS:
	case AT_SKILL_INNOVATION:
	case AT_SKILL_LIGHTSHOCK:
	case AT_SKILL_DESTRUCT:
	case AT_SKILL_EXPWIZARDRY:
	case AT_SKILL_RECOVERY:
	case AT_SKILL_MULTISHOT:
	case AT_SKILL_FLAMESTRIKE:
	case AT_SKILL_GIGANTICSTORM:
	case AT_SKILL_CHAOTICDISEIER:
	case AT_SKILL_RAGEFIGHTER_DRAGONLORE:
	case AT_SKILL_PHOENIXSHOT:
	case AT_SKILL_RAGEFIGHTER_DARKSIDE:
		// MSKILL
	case AT_MSKILL_DK_CYCLONE1:
	case AT_MSKILL_DK_SLASH1:
	case AT_MSKILL_DK_FALLINGSLASH1:
	case AT_MSKILL_DK_LUNGE1:
	case AT_MSKILL_DK_TWISTINGSLASH1:
	case AT_MSKILL_DK_RAGEFULBLOW1:
	case AT_MSKILL_DK_TWISTINGSLASH2:
	case AT_MSKILL_DK_RAGEFULBLOW2:
	case AT_MSKILL_DK_DEATHSTAB1:
	case AT_MSKILL_DK_DEATHSTAB2:
	case AT_MSKILL_DK_DEATHSTAB3:
	case AT_MSKILL_DK_DESTRUCTION1:
	case AT_MSKILL_DK_DESTRUCTION2:
	case AT_MSKILL_DK_DESTRUCTION3:
	case AT_MSKILL_DK_BLOODSTORM:
	case AT_MSKILL_DK_BLOODSTORM1:
	case AT_MSKILL_DK_SWELLLIFE1:
	case AT_MSKILL_DK_SWELLLIFE2:
	case AT_MSKILL_DK_SWELLLIFE3:
	case AT_MSKILL_DW_FLAME1:
	case AT_MSKILL_DW_LIGHTNING1:
	case AT_MSKILL_DW_EXPWIZARDRY1:
	case AT_MSKILL_DW_INFERNO1:
	case AT_MSKILL_DW_BLAST1:
	case AT_MSKILL_DW_EXPWIZARDRY2:
	case AT_MSKILL_DW_POISON1:
	case AT_MSKILL_DW_EVILSPIRIT1:
	case AT_MSKILL_DW_DECAY1:
	case AT_MSKILL_DW_HELLFIRE1:
	case AT_MSKILL_DW_ICE1:
	case AT_MSKILL_DW_METEORIT1:
	case AT_MSKILL_DW_METEORIT2:
	case AT_MSKILL_DW_ICESTORM1:
	case AT_MSKILL_DW_ICESTORM2:
	case AT_MSKILL_DW_NOVA1:
	case AT_MSKILL_DW_MANASHIELD1:
	case AT_MSKILL_DW_MANASHIELD2:
	case AT_MSKILL_DW_MANASHIELD3:
	case AT_MSKILL_DW_EARTHPRISON:
	case AT_MSKILL_DW_EARTHPRISON1:
	case AT_MSKILL_FE_HEAL1:
	case AT_MSKILL_FE_TRIPLESHOT1:
	case AT_MSKILL_FE_PENETRATION1:
	case AT_MSKILL_FE_DEFENSEBUFF1:
	case AT_MSKILL_FE_TRIPLESHOT2:
	case AT_MSKILL_FE_ATTACKBUFF1:
	case AT_MSKILL_FE_ATTACKBUFF2:
	case AT_MSKILL_FE_DEFENSEBUFF2:
	case AT_MSKILL_FE_CURE:
	case AT_MSKILL_FE_PARTYHEAL:
	case AT_MSKILL_FE_PARTYHEAL1:
	case AT_MSKILL_FE_BLESS:
	case AT_MSKILL_FE_BLESS1:
	case AT_MSKILL_FE_ICEARROW1:
	case AT_MSKILL_FE_INFINITY1:
	case AT_MSKILL_FE_MULTISHOT1:
	case AT_MSKILL_FE_MULTISHOT2:
	case AT_MSKILL_SUM_SLEEP1:
	case AT_MSKILL_SUM_CHAINLIGHTNING1:
	case AT_MSKILL_SUM_LIGHTNSHOCK1:
	case AT_MSKILL_SUM_DRAINLIFE1:
	case AT_MSKILL_SUM_DRAINLIFE2:
	case AT_MSKILL_SUM_WEAKNESS1:
	case AT_MSKILL_SUM_INNOVATION1:
	case AT_MSKILL_SUM_BERSERK1:
	case AT_MSKILL_SUM_BERSERK2:
	case AT_MSKILL_SUM_BERSERK3:
	case AT_MSKILL_SUM_BLIND:
	case AT_MSKILL_SUM_BLIND1:
	case AT_MSKILL_MG_CYCLONE1:
	case AT_MSKILL_MG_LIGHTNING1:
	case AT_MSKILL_MG_TWISTINGSLASH1:
	case AT_MSKILL_MG_POWERSLASH1:
	case AT_MSKILL_MG_FLAME1:
	case AT_MSKILL_MG_BLAST1:
	case AT_MSKILL_MG_INFERNO1:
	case AT_MSKILL_MG_EVILSPIRIT1:
	case AT_MSKILL_MG_ICE1:
	case AT_MSKILL_MG_ICE2:
	case AT_MSKILL_MG_FIRESLASH1:
	case AT_MSKILL_MG_FIRESLASH2:
	case AT_MSKILL_MG_FLAMESTRIKE1:
	case AT_MSKILL_MG_FLAMESTRIKE2:
	case AT_MSKILL_MG_GIGANTICSTORM1:
	case AT_MSKILL_DL_FIREBURST1:
	case AT_MSKILL_DL_FORCEWAVE1:
	case AT_MSKILL_DL_CRITICALDAMAGE1:
	case AT_MSKILL_DL_EARTHSHAKE1:
	case AT_MSKILL_DL_FIREBURST2:
	case AT_MSKILL_DL_CRITICALDAMAGE2:
	case AT_MSKILL_DL_EARTHSHAKE2:
	case AT_MSKILL_DL_CRITICALDAMAGE3:
	case AT_MSKILL_DL_CRITICALDAMAGE4:
	case AT_MSKILL_DL_IRONDEFENSE:
	case AT_MSKILL_DL_IRONDEFENSE1:
	case AT_MSKILL_DL_FIRESCREAM1:
	case AT_MSKILL_DL_FIRESCREAM2:
	case AT_MSKILL_DL_ELECTRICSPARK1:
	case AT_MSKILL_DL_CHAOTICDESEAR1:
	case AT_MSKILL_RF_DARKSIDE1:
	case AT_MSKILL_RF_DARKSIDE2:
	case AT_MSKILL_RF_DRAGONRORE1:
	case AT_MSKILL_RF_DRAGONRORE2:
		return true;
	}

	return false;
}




int  CObjUseSkill::GetUseMana(int aIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj =&gObj[aIndex];
	float mana = lpObj->Mana;

	if ( lpObj->Type != OBJ_USER )
	{
		return 0;
	}

	if ( lpMagic == NULL )
	{
		return 0;
	}

	float  usemana = 0;

	if( g_MasterSkillSystem.CheckMasterLevelSkill(lpMagic->m_Skill) )
	{
		int nMana = MagicDamageC.SkillGetMana(lpMagic->m_Skill);
		usemana = nMana;
		usemana += (nMana/100.0f)*g_MasterSkillSystem.GetMasterSkillManaIncRate(lpMagic->m_Level);
		usemana += GetAddUseMana(lpObj,lpMagic);
	}
	else
	{
		usemana = (float)MagicDamageC.SkillGetMana(lpMagic->m_Skill) + GetAddUseMana(lpObj,lpMagic);
	}

	usemana -= usemana*lpObj->m_MPSkillOpt.MpsDecreaseMana/100.0f;

	if( usemana < 1 )
	{
		usemana = 1;
	}

	mana -=usemana;

	if(mana < 0.0f)
	{
		return -1;
	}
	return (int)mana;
}

int CObjUseSkill::GetAddUseMana(LPOBJ lpObj,CMagicInf * lpMagic)
{
	int iRetValue = 0;

	if(lpObj->Class == CLASS_ELF && lpObj->Type == OBJ_USER && lpObj->ChangeUP == 1 &&
		(gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW) == TRUE || gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW2)==TRUE ))
	{
		if(lpMagic->m_Skill == AT_SKILL_CROSSBOW
			|| lpMagic->m_Skill == AT_SKILL_BOW
			|| lpMagic->m_Skill == AT_SKILL_ELFHARDEN
			|| lpMagic->m_Skill == AT_SKILL_PENETRATION
			|| lpMagic->m_Skill == AT_SKILL_5CROSSBOW
			|| lpMagic->m_Skill == AT_MSKILL_FE_TRIPLESHOT1
			|| lpMagic->m_Skill == AT_MSKILL_FE_TRIPLESHOT2
			|| lpMagic->m_Skill == AT_MSKILL_FE_PENETRATION1
			|| lpMagic->m_Skill == AT_MSKILL_FE_ICEARROW1)
		{
			int iArrowLevel = 0;

			CItem * Right = &lpObj->pInventory[0];
			CItem * Left = &lpObj->pInventory[1];

			if(Right->m_Type >= ITEMGET(4,8) && Right->m_Type <= ITEMGET(4,14)
				|| Right->m_Type == ITEMGET(4,16)
				|| Right->m_Type == ITEMGET(4,18)
				|| Right->m_Type == ITEMGET(4,19))
			{
				if(Left->m_Type == ITEMGET(4,7))
				{
					iArrowLevel = Left->m_Level;
				}
			}
			else if(Left->m_Type >= ITEMGET(4,0) && Left->m_Type <= ITEMGET(4,6)
				|| Left->m_Type == ITEMGET(4,17)
				|| Left->m_Type == ITEMGET(4,20)
				|| Left->m_Type == ITEMGET(4,21)
				|| Left->m_Type == ITEMGET(4,22)
				|| Left->m_Type == ITEMGET(4,22) )
			{
				if(Right->m_Type == ITEMGET(4,15))
				{
					iArrowLevel = Right->m_Level;
				}
			}

			if(iArrowLevel == 0)
			{
				iRetValue = g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus0();
			}
			else if(iArrowLevel == 1)
			{
				iRetValue = g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus1();
			}
			else if(iArrowLevel == 2)
			{
				iRetValue = g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus2();
			}
			else if(iArrowLevel == 3)
			{
				iRetValue = g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus3();
			}
		}
	}
	return iRetValue;
}

int  CObjUseSkill::GetUseBP(int aIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];

	int bp = lpObj->BP;

	if(lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	if(lpMagic == 0)
	{
		return 0;
	}

	int usingbp = MagicDamageC.SkillGetBP(lpMagic->m_Skill);

	usingbp -= usingbp * lpObj->SetOpDecreaseAG / 100;

	if( lpObj->m_btSocketBPUseDec > 0 )
	{
		usingbp -= usingbp * lpObj->m_btSocketBPUseDec / 100;
	}

	bp -= usingbp;

	if(bp < 0)
	{
		return -1;
	}

	return bp;
}

void CObjUseSkill::UseSkill(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	int bCombo = 0;

	if(gComboAttack.CheckCombo(aIndex,lpMagic->m_Skill))
	{
		bCombo = 1;
	}

	if(lpObj->SkillHellFire2State && (lpMagic->m_Skill != AT_SKILL_EXPHELL && lpMagic->m_Skill != AT_MSKILL_DW_NOVA1) )
	{
		CMagicInf * lpMagic = gObjGetMagicSearch(lpObj,AT_MSKILL_DW_NOVA1);

		if( lpMagic == NULL )
		{
			lpMagic = gObjGetMagicSearch(lpObj,AT_SKILL_EXPHELL);
		}

		if(lpMagic != 0)
		{
			if( lpMagic->m_Skill == AT_SKILL_EXPHELL )
				gObjUseSkill.RunningSkill(lpObj->m_Index,0,lpMagic,0);
			else
				g_MasterSkillSystem.RunningSkill_MLS(lpObj,0,lpMagic,0,0,0,0,0);

		}
		return;
	}

	int usemana = GetUseMana(aIndex,lpMagic);

	if(usemana >= 0)
	{
		if(!gObj[aIndex].SkillDelay.Check(lpMagic->m_Skill))
		{
			LogAddTD("[%d] Skill delay error!",gObj[aIndex].AccountID);
			return;
		}

		int usebp = GetUseBP(aIndex,lpMagic);

		if(usebp >= 0)
		{
			if(RunningSkill(aIndex,aTargetIndex,lpMagic,bCombo)==1)
			{
				lpObj->UseMagicNumber = lpMagic->m_Skill;
				lpObj->UseMagicTime = GetTickCount();
				lpObj->UseMagicCount = 0;
				//Mana refill fix 2
				usemana = GetUseMana(aIndex,lpMagic);

				lpObj->Mana = (float)usemana;
				lpObj->BP = usebp;

				GCManaSend(aIndex,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
			}
		}
	}
}

void CObjUseSkill::UseSkill(int aIndex, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int bCombo = 0;

	if(gComboAttack.CheckCombo(aIndex,lpMagic->m_Skill))
	{
		bCombo = 1;
	}

	if(lpObj->SkillHellFire2State && lpMagic->m_Skill != AT_SKILL_EXPHELL && lpMagic->m_Skill != AT_MSKILL_DW_NOVA1)
	{
		CMagicInf * lpMagic = gObjGetMagicSearch(lpObj,AT_MSKILL_DW_NOVA1);

		if(lpMagic == NULL )
		{
			lpMagic = gObjGetMagicSearch(lpObj,AT_SKILL_EXPHELL);
		}

		if(lpMagic != 0)
		{
			if( lpMagic->m_Skill == AT_SKILL_EXPHELL )
				gObjUseSkill.RunningSkill(lpObj->m_Index,0,lpMagic,0);
			else
				g_MasterSkillSystem.RunningSkill_MLS(lpObj,0,lpMagic,0,0,0,0,0);
		}
		return;
	}

	int usemana = GetUseMana(aIndex,lpMagic);

	if( lpMagic->m_Skill == 62 )
	{
		usemana = (int)gObj[aIndex].Mana;
	}

	PMSG_DURATION_MAGIC_SEND pMsg;

	if(usemana >= 0)
	{
		if(!gObj[aIndex].SkillDelay.Check(lpMagic->m_Skill))
		{
			LogAddTD("[%s] Skill delay error!",gObj[aIndex].AccountID);
			return;
		}

		int usebp = gObjMagicBPUse(&gObj[aIndex],lpMagic);

		if(usebp >= 0)
		{
			lpObj->UseMagicNumber = lpMagic->m_Skill;
			lpObj->UseMagicTime = GetTickCount();
			lpObj->UseMagicCount = 0;

			lpObj->Mana = (float)usemana;
			lpObj->BP = usebp;

			GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);

			PHeadSetBE((PBYTE)&pMsg,0x1E,sizeof(pMsg));

			pMsg.MagicNumberH = SET_NUMBERH(lpMagic->m_Skill);
			pMsg.MagicNumberL = SET_NUMBERL(lpMagic->m_Skill);

			pMsg.X = x;
			pMsg.Y = y;
			pMsg.Dir = dir;
			pMsg.NumberH = SET_NUMBERH(aIndex);
			pMsg.NumberL = SET_NUMBERL(aIndex);

			if(lpObj->Type == OBJ_USER)
			{
				DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
			}

			if(CC_MAP_RANGE(lpObj->MapNumber))
			{
				switch(lpMagic->m_Skill)
				{
				case AT_SKILL_MAGICDEFENSE:
				case AT_SKILL_HEALING:
				case AT_SKILL_DEFENSE:
				case AT_SKILL_ATTACK:
				case AT_SKILL_KNIGHTADDLIFE:
					//MSKILL
				case AT_MSKILL_DW_MANASHIELD1:
				case AT_MSKILL_DW_MANASHIELD2:
				case AT_MSKILL_DW_MANASHIELD3:
				case AT_MSKILL_FE_HEAL1:
				case AT_MSKILL_FE_DEFENSEBUFF1:
				case AT_MSKILL_FE_DEFENSEBUFF2:
				case AT_MSKILL_FE_CURE:
				case AT_MSKILL_FE_BLESS:
				case AT_MSKILL_FE_BLESS1:
				case AT_MSKILL_FE_ATTACKBUFF1:
				case AT_MSKILL_FE_ATTACKBUFF2:
				case AT_MSKILL_DK_SWELLLIFE1:
				case AT_MSKILL_DK_SWELLLIFE2:
				case AT_MSKILL_DK_SWELLLIFE3:
					break;
				default: 
					MsgSendV2(lpObj,(PBYTE)&pMsg,pMsg.h.size);
				}
			}
			else
			{
				MsgSendV2(lpObj,(PBYTE)&pMsg,pMsg.h.size);
			}
		}

		if( g_MasterSkillSystem.CheckMasterLevelSkill(lpMagic->m_Skill) )
		{
			g_MasterSkillSystem.RunningSkill_MLS(lpObj,aTargetIndex,lpMagic,bCombo,x,y,dir,TargetPos);
			return;
		}

		if( lpMagic->m_Skill == AT_SKILL_SWORD6 )
		{
			SkillPowerSlash(aIndex,lpMagic,x,y,dir,TargetPos,aTargetIndex);
		}

		if( lpMagic->m_Skill == AT_SKILL_EXPPOISON )
		{
			if(	lpObj->Class == 561	) //Medusa Ok
			{
				SkillDeathPoisonForMeDusa(aIndex, lpMagic, x, y, aTargetIndex);
			}
			else
			{
				SkillDeathPoison(aIndex,lpMagic,x,y,dir,TargetPos,aTargetIndex);
			}
		}
		else if( lpMagic->m_Skill == AT_SKILL_EXPICE )
		{
			SkillSuddenIce(aIndex,lpMagic,x,y,dir,TargetPos,aTargetIndex);
		}
		else if( lpMagic->m_Skill == AT_SKILL_BLOWOFFURY )
		{
			SkillBlowOfFury(aIndex,lpMagic,x,y,dir,TargetPos,aTargetIndex,bCombo);
		}
		else if( lpMagic->m_Skill == AT_SKILL_WHEEL )
		{
			SkillWheel(aIndex,lpMagic,aTargetIndex,bCombo);
		}
		else if( lpMagic->m_Skill == AT_SKILL_ELECTRICSPARK )
		{
			SkillElectricSpark(aIndex,lpMagic,x,y,dir,TargetPos,aTargetIndex);
		}
		else if( lpMagic->m_Skill == AT_SKILL_DARKHORSE_ATTACK )
		{
			SkillDarkHorseAttack(lpObj->m_Index,aTargetIndex,lpMagic);
		}
		else if( lpMagic->m_Skill == AT_SKILL_STUN )
		{
			SkillStun(aIndex,aTargetIndex,lpMagic,x,y,dir,TargetPos);
		}
		else if( lpMagic->m_Skill == AT_SKILL_FENRIR_ATTACK )
		{
			SkillFenrirAttack(aIndex,aTargetIndex,lpMagic);
		}
		else if( lpMagic->m_Skill == AT_SKILL_LIGHTEXPLODE )
		{
			SkillLightExplode(aIndex,aTargetIndex,lpMagic);
		}
		else if( lpMagic->m_Skill == AT_SKILL_DRAINLIFE )
		{
			SkillDrainLife(aIndex,aTargetIndex,lpMagic);
		}
		else if( lpMagic->m_Skill == AT_SKILL_CHAINLIGHTING )
		{
			SkillChainLightning(aIndex,aTargetIndex,lpMagic);
		}
		else if( lpMagic->m_Skill == AT_SKILL_SAHAMUT || lpMagic->m_Skill == AT_SKILL_NEIL )
		{
			SkillCurse(aIndex,lpMagic,x,y,aTargetIndex);
		}
		else if( lpMagic->m_Skill == AT_SKILL_INNOVATION )
		{
			SkillInnovation(aIndex,lpMagic,x,y,aTargetIndex);
		}
		else if( lpMagic->m_Skill == AT_SKILL_WEAKNESS )
		{
			SkillWeakness(aIndex,lpMagic,x,y,aTargetIndex);
		}
		else if( lpMagic->m_Skill == AT_SKILL_LAGLE )
		{
			SkillLagle(aIndex,lpMagic,x,y,aTargetIndex,59);
		}
		else if( lpMagic->m_Skill == AT_SKILL_LIGHTSHOCK )
		{
			SkillLightShock(aIndex,aTargetIndex,lpMagic);
		}
		else if( lpMagic->m_Skill == AT_SKILL_DESTRUCT )
		{
			SkillDestruct(aIndex,lpMagic,x,y,aTargetIndex);
		}
		else if( lpMagic->m_Skill == AT_SKILL_FLAMESTRIKE )
		{
			SkillFlameStrike(aIndex,lpMagic,TargetPos,aTargetIndex);
		}
		else if( lpMagic->m_Skill == AT_SKILL_GIGANTICSTORM  )
		{
			SkillGiganticStorm(aIndex,lpMagic,x,y,aTargetIndex);
		}
		else if( lpMagic->m_Skill == AT_SKILL_MULTISHOT  )
		{
			SkillMultiShot(aIndex,lpMagic,TargetPos,aTargetIndex);
		}
		else if( lpMagic->m_Skill == AT_SKILL_CHAOTICDISEIER )
		{
			SkillChaoticDesier(aIndex,lpMagic,TargetPos,aTargetIndex);
		}
		else if (lpMagic->m_Skill == 9) // Medusa OK 
		{
			if( gObj[aIndex].Class == 561 )	//Added Leee
			{
				this->SkillEvilMonster(aIndex, aTargetIndex, lpMagic);
			}
		}
		else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DARKSIDE )
		{
			PMSG_MONK_DARKSIDE_RECV pMsg = {0};
			pMsg.NumberH = SET_NUMBERH(aTargetIndex);
			pMsg.NumberL = SET_NUMBERL(aTargetIndex);

			pMsg.MagicNumberH = SET_NUMBERH(lpMagic->m_Skill);
			pMsg.MagicNumberL = SET_NUMBERL(lpMagic->m_Skill);

			GCMonkDarkSideTargetSelect(&pMsg,aIndex);

			if( lpObj->m_btDarkSideTargetCount )
			{
				for(int i = 0; i < 5; i++)
				{
					if( lpObj->m_wDarkSideTarget[i] != 10000 )
						SkillMonkBarrageJustOneTarget(aIndex,lpMagic,lpObj->m_wDarkSideTarget[i]);
				}
			}
		}
		else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DRAGONLORE )
		{
			if( !OBJMAX_RANGE(aTargetIndex) )
			{
				LogAddTD("[InvalidTargetIndex][CObjUseSkill.UseSkill][AT_SKILL_DRAGON_ROAR] Index :%d , AccountID : %s ",
					aIndex,lpObj->AccountID);
				return;
			}

			SkillAreaMonsterAttack(aIndex,lpMagic,gObj[aTargetIndex].X,gObj[aTargetIndex].Y,aTargetIndex,3,1,0);
		}
		else if( lpMagic->m_Skill == AT_SKILL_PHOENIXSHOT )
		{
			if( !OBJMAX_RANGE(aTargetIndex) )
			{
				LogAddTD("[InvalidTargetIndex][CObjUseSkill.UseSkill][AT_SKILL_PHOENIXSHOT] Index :%d , AccountID : %s ",
					aIndex,lpObj->AccountID);
				return;
			}

			SkillAreaMonsterAttack(aIndex,lpMagic,gObj[aTargetIndex].X,gObj[aTargetIndex].Y,aTargetIndex,2,1,0);
		}
	}
}




BOOL CObjUseSkill::RunningSkill(int aIndex, int aTargetIndex, CMagicInf * lpMagic, BOOL bCombo)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int  MagicNumber = lpMagic->m_Skill;

	lpObj->m_SkillNumber = MagicNumber;

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_INVISIBILITY) == TRUE )
	{
		if( (lpObj->Authority & 32) != 32 )
		{
			RemoveCloakingEffect(lpObj->m_Index);
		}
	}

	if( g_MasterSkillSystem.CheckMasterLevelSkill(lpMagic->m_Skill) )
	{
		return g_MasterSkillSystem.RunningSkill_MLS(lpObj,aTargetIndex,lpMagic,bCombo,0,0,0,0);
	}

	switch(MagicNumber)
	{
	case AT_SKILL_BLOCKING:
		if(lpObj->pInventory[1].IsItem() == 1)
		{
			if(lpObj->pInventory[1].m_Type >= ITEMGET(6,4) && lpObj->pInventory[1].m_Type < ITEMGET(7,0))
			{
				if(lpObj->pInventory[1].m_Option1 != 0)
				{
					lpObj->m_SkillTime = GetTickCount() + 4000;
					GCActionSend(lpObj,18,lpObj->m_Index,aTargetIndex);
				}
			}
		}
		break;
	case AT_SKILL_POISON:
	case AT_SKILL_METEO:
	case AT_SKILL_THUNDER:
	case AT_SKILL_FIREBALL:
	case AT_SKILL_FLAME:
	case AT_SKILL_SLOW:
	case AT_SKILL_ENERGYBALL:
	case AT_SKILL_SWORD1:
	case AT_SKILL_SWORD2:
	case AT_SKILL_SWORD3:
	case AT_SKILL_SWORD4:
	case AT_SKILL_SWORD5:
	case AT_SKILL_CROSSBOW:
	case AT_SKILL_BOW:
	case AT_SKILL_WHEEL:
	case AT_SKILL_BLOWOFFURY:
	case AT_SKILL_KNIGHTSPEAR:
	case AT_SKILL_PENETRATION:
	case AT_SKILL_SWORD6:
		gObjAttack(lpObj,lpTargetObj,lpMagic,1,0,0,0,0,0);
		break;
	case AT_SKILL_HEALING:	
		SkillHealing(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_MAGICDEFENSE:	
		WizardMagicDefense(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_KNIGHTADDLIFE:	
		KnightSkillAddLife(aIndex,lpMagic->m_Level,lpMagic);break;
	case AT_SKILL_DEFENSE:	
		SkillDefense(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_ATTACK:	
		SkillAttack(aIndex,aTargetIndex,lpMagic);break;

	case AT_SKILL_CALLMON1:	return SkillMonsterCall(lpObj->m_Index,26,lpObj->X-1,lpObj->Y+1);
	case AT_SKILL_CALLMON2:	return SkillMonsterCall(lpObj->m_Index,32,lpObj->X-1,lpObj->Y+1);
	case AT_SKILL_CALLMON3:	return SkillMonsterCall(lpObj->m_Index,21,lpObj->X-1,lpObj->Y+1);
	case AT_SKILL_CALLMON4:	return SkillMonsterCall(lpObj->m_Index,20,lpObj->X-1,lpObj->Y+1);
	case AT_SKILL_CALLMON5:	return SkillMonsterCall(lpObj->m_Index,10,lpObj->X-1,lpObj->Y+1);
	case AT_SKILL_CALLMON6:	return SkillMonsterCall(lpObj->m_Index,150,lpObj->X-1,lpObj->Y+1);
	case AT_SKILL_CALLMON7:	return SkillMonsterCall(lpObj->m_Index,151,lpObj->X-1,lpObj->Y+1);
	//case AT_MSKILL_FE_SUMMONSATYROS: return SkillMonsterCall(lpObj->m_Index,597,lpObj->X-1,lpObj->Y+1);

	case AT_SKILL_EXPHELL:
		return SkillHellFire2(lpObj->m_Index,aTargetIndex,lpMagic);
	case AT_SKILL_STRIKE:	
		return SkillKnightBlow(lpObj->m_Index,aTargetIndex,lpMagic,bCombo);

	case AT_SKILL_SPEAR:	return SkillSpear(lpObj->m_Index,aTargetIndex,lpMagic);
	case AT_SKILL_FIREBURST:	
		return SkillFireBurst(lpObj->m_Index,aTargetIndex,lpMagic);
	case AT_SKILL_DARKHORSE_ATTACK:	return SkillDarkHorseAttack(lpObj->m_Index,aTargetIndex,lpMagic);
	case AT_SKILL_RECALL_PARTY:	SkillRecallParty(aIndex,lpMagic->m_Level);break;
	case AT_SKILL_ADD_CRITICALDAMAGE:
		SkillAddCriticalDamage(aIndex,lpMagic);break;

	case AT_SKILL_DEFENSEDOWN:	
		MaGumSkillDefenseDown(lpObj->m_Index,aTargetIndex,lpMagic);break;

	case AT_SKILL_SUMMON:SkillSummon(lpObj->m_Index,aTargetIndex,lpMagic);break;
	case AT_SKILL_IMMUNE_TO_MAGIC:	SkillImmuneToMagic(lpObj->m_Index,lpMagic);break;
	case AT_SKILL_IMMUNE_TO_HARM:	SkillImmuneToHarm(lpObj->m_Index,lpMagic);break;

	case AT_SKILL_RUSH:	SkillKnightRush(lpObj->m_Index,aTargetIndex,lpMagic);break;
	case AT_SKILL_JAVALIN:	SkillWizardJavalin(lpObj->m_Index,aTargetIndex,lpMagic);break;
	case AT_SKILL_DEEPIMPACT:	SkillElfRush(lpObj->m_Index,aTargetIndex,lpMagic);break;
	case AT_SKILL_ONE_FLASH:	SkillMagumOneFlash(lpObj->m_Index,aTargetIndex,lpMagic);break;
	case AT_SKILL_DEATH_CANNON:	SkillMagumDeathCannon(lpObj->m_Index,aTargetIndex,lpMagic);break;
	case AT_SKILL_SPACE_SPLIT:	SkillDarklordSpaceSplit(lpObj->m_Index,aTargetIndex,lpMagic);break;

	case AT_SKILL_BRAND_OF_SKILL:	SkillBrandOfSkill(aIndex,aTargetIndex,lpMagic);break;

	case AT_SKILL_REMOVAL_STUN:	SkillRemoveStun(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_ADD_MANA:	SkillAddMana(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_INVISIBLE:	SkillCloaking(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_REMOVAL_INVISIBLE:	SkillRemoveCloaking(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_REMOVAL_MAGIC:	SkillRemoveMagic(aIndex,aTargetIndex,lpMagic);break;

	case AT_SKILL_INFINITY_ARROW:	
		SkillInfinityArrow(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_DAMAGEREFLECT:	SkillDamageReflect(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_SLEEP:
		SkillSleep(aIndex,aTargetIndex,lpMagic);	break;
	case AT_SKILL_BERSERKER:
		SkillBerserker(lpObj->m_Index,lpMagic);break;
	case AT_SKILL_RECOVERY:	SkillRecovery(aIndex,aTargetIndex,lpMagic);	break;
	case AT_SKILL_EXPWIZARDRY:
		SkillExpWizard(aIndex,aTargetIndex,lpMagic);break;
	case AT_SKILL_RAGEFIGHTER_REDARMOR:
		SkillMonkBuff(aIndex,lpMagic);
		break;
	case AT_SKILL_RAGEFIGHTER_FITNESS:
	case AT_SKILL_RAGEFIGHTER_DEFRATE:
		SkillMonkBuffApplyParty(aIndex,lpMagic);
		break;
	case AT_SKILL_RAGEFIGHTER_WEAPON1:
	case AT_SKILL_RAGEFIGHTER_WEAPON2:
	case AT_SKILL_RAGEFIGHTER_CHAINDRIVE:
	//case AT_SKILL_RAGEFIGHTER_DARKSIDE:
		SkillMonkBarrageJustOneTarget(aIndex,lpMagic,aTargetIndex);
		break;
	case AT_SKILL_RAGEFIGHTER_DRAGONSLAYER:
		gObjAttack(lpObj,lpTargetObj,lpMagic,1,0,0,0,0,0);
		break;
	case AT_SKILL_RAGEFIGHTER_SIEGE:
		SkillCharge(aIndex,lpMagic,aTargetIndex);
		break;

	default:	gObjAttack(lpObj,lpTargetObj,lpMagic,1,0,0,0,0,0);break;
	}

	return true;
}




void CObjUseSkill::MaGumSkillDefenseDown(int aIndex, int aTargetIndex, CMagicInf* lpMagic)
{
	int skillSuccess = 1;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpObj->Type == OBJ_USER && lpObj->Class != CLASS_MAGUMSA)
	{
		return;
	}

	if( retCalcSkillResistance(lpTargetObj,1) )
	{
		gObjAddBuffEffect(lpTargetObj,BUFF_DEFENSEDOWN,ADD_OPTION_DEFENSEDOWN,10,0,0,10);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
	}
}




void CObjUseSkill::KnightSkillAddLife(int aIndex, int skill_level,CMagicInf* lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJ_USER && lpObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_KNIGHT && lpObj->Class != CLASS_MAGUMSA)
	{
		return;
	}

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;

	int ApplyPartyIndex[MAX_USER_IN_PARTY];

	int viewplayer = 0;
	int viewpercent = 0;

	LPOBJ lpPartyObj;

	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));

	partynum = lpObj->PartyNumber;

	if(partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if(number >= 0)
			{
				lpPartyObj = &gObj[number];

				if(lpObj->MapNumber == lpPartyObj->MapNumber)
				{
					dis = gObjCalDistance(lpObj,&gObj[number]);

					if(dis < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj->m_Index;
						viewplayer++;
					}
				}
			}
		}
	}

	if(viewplayer > 1)
	{
		if(viewplayer == 2)
		{
			viewpercent = 1;
		}
		else if(viewplayer == 3)
		{
			viewpercent = 2;
		}
		else if(viewplayer == 4)
		{
			viewpercent = 3;
		}
		else
		{
			viewpercent = 4;
		}
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}
	
	float iaddLifepower = (float)(lpInfo->m_Arg1 + (lpObj->Vitality + lpObj->AddVitality) / lpInfo->m_Arg2 + (lpObj->Energy + lpObj->AddEnergy)/lpInfo->m_Arg3 + viewpercent);

	if( iaddLifepower > lpInfo->m_Arg4 )
		iaddLifepower = (float)lpInfo->m_Arg4;

	int iLifeTime = lpInfo->m_Arg5 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg6;

	if( iLifeTime > lpInfo->m_Arg7 )
		iLifeTime = lpInfo->m_Arg7;

	int iaddlife;

	if(partynum == -1)
	{
		iaddlife = (int)(lpObj->MaxLife*iaddLifepower / 100.0f);

		/*if( gObjCheckPowerfulEffect(lpObj,BuffNumber,iaddlife,iaddmana,iaddbp) )
		{
			GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,0);
			return;
		}*/

		gObjAddBuffEffect(lpObj,BUFF_SWELL_LIFE,ADD_OPTION_LIFE,iaddlife,0,0,iLifeTime);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,skillSuccess);
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if(ApplyPartyIndex[n]!= -1)
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];
				iaddlife = (int)(lpPartyObj->MaxLife*iaddLifepower/100.0f);

				/*if( gObjCheckPowerfulEffect(lpPartyObj,BuffNumber,iaddlife,iaddmana,iaddbp) )
				{
					GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,0);
					return;
				}*/

				gObjAddBuffEffect(lpPartyObj,BUFF_SWELL_LIFE,ADD_OPTION_LIFE,iaddlife,0,0,iLifeTime);
				GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,skillSuccess);
			}
		}
	}
}




void CObjUseSkill::WizardMagicDefense(int aIndex, int aTargetIndex, CMagicInf* lpMagic)
{
	int skillSuccess = true;

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpObj->Type != OBJ_USER && lpObj->m_RecallMon== -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_WIZARD && lpObj->Class != CLASS_MAGUMSA)
	{
		return;
	}

	if(lpObj->PartyNumber != lpTargetObj->PartyNumber)
	{
		return;
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	float iShieldPower = (float)(lpInfo->m_Arg1 + (lpObj->Dexterity + lpObj->AddDexterity) / lpInfo->m_Arg2 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg3);

	if( iShieldPower > lpInfo->m_Arg4 )
		iShieldPower = (float)lpInfo->m_Arg4;

	int iShieldTime = lpInfo->m_Arg5 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg6;

	if( iShieldTime > lpInfo->m_Arg7)
		iShieldTime = lpInfo->m_Arg7;

	gObjAddBuffEffect(lpTargetObj,BUFF_MANASHIELD,ADD_OPTION_MSDECREASE,(int)iShieldPower,0,0,iShieldTime);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}




void CObjUseSkill::SkillDefense(int aIndex, int aTargetIndex, CMagicInf* lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpTargetObj->Type != OBJ_USER && lpTargetObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_ELF)
	{
		return;
	}

	if(CC_MAP_RANGE(lpObj->MapNumber) && lpObj->m_Index != lpTargetObj->m_Index)
	{
		return;
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}
	
	float skill_defense = (float)( lpInfo->m_Arg1+ (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2);
	
	if( skill_defense > lpInfo->m_Arg3 )
	{
		skill_defense = lpInfo->m_Arg3;
	}

	if(lpTargetObj->Class == CLASS_KNIGHT)
	{
		skill_defense += skill_defense/10.0f;
	}

	gObjAddBuffEffect(lpTargetObj,BUFF_ADD_DEFENSE,ADD_OPTION_DEFENSE,(int)skill_defense,0,0,lpInfo->m_Arg4);


	for(int n = 0; n < MAX_VIEWPORT; n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJ_MONSTER && lpObj->VpPlayer2[n].state != 0)
		{
			LPOBJ lpMonster = &gObj[lpObj->VpPlayer2[n].number];
			if(lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro.IncAgro(lpObj->m_Index,int(skill_defense)/10);
			}
		}
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}




void CObjUseSkill::SkillAttack(int aIndex, int aTargetIndex, CMagicInf* lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpTargetObj->Type != OBJ_USER && lpTargetObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_ELF)
	{
		return;
	}

	if(CC_MAP_RANGE(lpObj->MapNumber) && lpObj->m_Index != lpTargetObj->m_Index)
	{
		return;
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	float skill_attack = (float)lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2;

	if( skill_attack > lpInfo->m_Arg3 )
	{
		skill_attack = lpInfo->m_Arg3;
	}

	if(lpTargetObj->Class == CLASS_KNIGHT)
	{
		skill_attack += skill_attack/10.0f;
	}

	gObjAddBuffEffect(lpTargetObj,BUFF_ADD_ATTACK,ADD_OPTION_ATTACK_DAMAGE,(int)skill_attack,0,0,lpInfo->m_Arg4);


	for(int n = 0; n < MAX_VIEWPORT; n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJ_MONSTER && lpObj->VpPlayer2[n].state != 0)
		{
			LPOBJ lpMonster = &gObj[lpObj->VpPlayer2[n].number];
			if(lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro.IncAgro(lpObj->m_Index,int(skill_attack)/10);
			}
		}
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}




BOOL CObjUseSkill::SkillChangeUse(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int skill_level = -1;

	if(lpObj->Type != OBJ_USER)
	{
		return false;
	}

	if(		lpObj->pInventory[10].IsItem() 
		&&	lpObj->pInventory[10].m_Durability >= 1.0f 
		&&	!lpObj->pInventory[10].m_bExpired )
	{
		switch(lpObj->pInventory[10].m_Type)
		{
		case ITEMGET(13, 10):
			skill_level = lpObj->pInventory[10].m_Level;
			if( skill_level > 7 )
				return false;
			break;
		case ITEMGET(13, 39):
			skill_level = 372;
			break;
		case ITEMGET(13, 40):
			skill_level = 373;
			break;
		case ITEMGET(13, 41):
			skill_level = 374;
			break;
		case ITEMGET(13, 42):
			//if((lpObj->Authority & 32) != 32)
			//{
			//	return false;
			//}
			skill_level = 378;
			break;
		case ITEMGET(13, 68):
			skill_level = 477;
			break;
		case ITEMGET(13, 76):
			skill_level = 503;
			break;
		case ITEMGET(13, 77):
			skill_level = 503;
			break;
		case ITEMGET(13, 78):
			skill_level = 503;
			break;
		case ITEMGET(13, 122):
			skill_level = 548;
			break;
		case ITEMGET(13, 163):
			skill_level = 625;
			break;
		case ITEMGET(13, 164):
			skill_level = 626;
			break;
		case ITEMGET(13, 165):
			skill_level = 642;
			break;
		case ITEMGET(13, 166):
			skill_level = 646;
			break;
		}
	}

	if(		lpObj->pInventory[11].IsItem() 
		&&	skill_level == -1 
		&&	lpObj->pInventory[11].m_Durability >= 1.0f 
		&&	!lpObj->pInventory[11].m_bExpired )
	{
		switch(lpObj->pInventory[11].m_Type)
		{
		case ITEMGET(13,10):
			skill_level = lpObj->pInventory[11].m_Level;
			if( skill_level > 7 )
				return false;
			break;
		case ITEMGET(13,39):
			skill_level = 372;
			break;
		case ITEMGET(13,40):
			skill_level = 373;
			break;
		case ITEMGET(13,41):
			skill_level = 374;
			break;
		case ITEMGET(13,42):
			//if((lpObj->Authority & 32) != 32)
			//{
			//	return false;
			//}
			skill_level = 378;
			break;
		case ITEMGET(13,68):
			skill_level = 477;
			break;
		case ITEMGET(13,76):
			skill_level = 503;
			break;
		case ITEMGET(13,77):
			skill_level = 503;
			break;
		case ITEMGET(13,78):
			skill_level = 503;
			break;
		case ITEMGET(13,122):
			skill_level = 548;
			break;
		case ITEMGET(13, 163):
			skill_level = 625;
			break;
		case ITEMGET(13, 164):
			skill_level = 626;
			break;
		case ITEMGET(13, 165):
			skill_level = 642;
			break;
		case ITEMGET(13, 166):
			skill_level = 646;
			break;
		}
	}

	if( IT_MAP_RANGE(lpObj->MapNumber) )
	{
		if( g_IllusionTemple.GetState(lpObj->MapNumber) != IT_STATE_CLOSED )
		{
			int TEAM = g_IllusionTemple.GetTeam(lpObj->MapNumber,lpObj->m_Index);

			if( TEAM == 0 )
			{
				skill_level = 404;
			}
			else if( TEAM == 1 )
			{
				skill_level = 405;
			}
		}
	}

	int change = -1;

	if( skill_level > 15 )
	{
		change = skill_level;
	}
	else
	{
		switch(skill_level)
		{
		case 0: change = 2;		break;
		case 1: change = 7;		break;
		case 2: change = 14;	break;
		case 3: change = 8;		break;
		case 4:	change = 9;		break;
		case 5:	change = 41;	break;
			default: change = -1;	break;
		}
	}

	if(lpObj->m_Change >= 0 && lpObj->m_Change == change)
	{
		return false;
	}

	if(lpObj->m_Change == 41 && change != 41)
	{
		lpObj->AddLife = 0;

		GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
		GCReFillSend(lpObj->m_Index,(WORD)(lpObj->MaxLife + lpObj->AddLife),0xFE,0,lpObj->iMaxShield + lpObj->iAddShield);
	}

	if(lpObj->m_Change == 372 && change != 372)
	{
		lpObj->AddLife = 0;
		gObjCalCharacter(lpObj->m_Index);

		GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
		GCReFillSend(lpObj->m_Index,(WORD)(lpObj->MaxLife + lpObj->AddLife),0xFE,0,lpObj->iMaxShield + lpObj->iAddShield);
	}

	if(lpObj->m_Change == 374 && change != 374)
	{
		lpObj->m_AttackDamageMaxLeft -= 20;
		lpObj->m_AttackDamageMinLeft -= 20;
		lpObj->m_AttackDamageMaxRight -= 20;
		lpObj->m_AttackDamageMinRight -= 20;
		lpObj->m_MagicDamageMin -= 20;
		lpObj->m_MagicDamageMax -= 20;
	}

	lpObj->m_Change = change;
	gObjViewportListProtocolCreate(lpObj);

#if (GS_CASTLE == 1)
	g_CastleSiege.NotifySelfCsJoinSide(lpObj->m_Index);
#endif

	PMSG_KILLCOUNT pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg,0xB8,0x01,sizeof(pMsg));

	pMsg.btKillCount = lpObj->m_btKillCount;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg));
	return true;
}




void CObjUseSkill::SkillHealing(int aIndex, int aTargetIndex, CMagicInf* lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpTargetObj->Type != OBJ_USER && lpTargetObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_ELF)
	{
		return;
	}

	if(CC_MAP_RANGE(lpObj->MapNumber) && lpObj->m_Index != lpTargetObj->m_Index)
	{
		return;
	}
	

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	int addlife = (lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg2);

	lpTargetObj->Life = addlife + lpTargetObj->Life;

	if((lpTargetObj->MaxLife + lpTargetObj->AddLife) < lpTargetObj->Life)
	{
		lpTargetObj->Life = lpTargetObj->MaxLife + lpTargetObj->AddLife;
	}

	if(lpTargetObj->Type == OBJ_USER)
	{
		GCReFillSend(lpTargetObj->m_Index,(WORD)lpTargetObj->Life,0xFF,0,lpTargetObj->iShield);

		for(int n = 0; n < MAX_VIEWPORT; n++)
		{
			if(lpObj->VpPlayer2[n].type == OBJ_MONSTER && lpObj->VpPlayer2[n].state != 0)
			{
				LPOBJ lpMonster = &gObj[lpObj->VpPlayer2[n].number];
				if(lpMonster->m_iCurrentAI != 0)
				{
					lpMonster->m_Agro.IncAgro(lpObj->m_Index,(lpObj->Energy / 5)/40);
				}
			}
		}
	}
	else if(lpTargetObj->m_RecallMon >= 0)
	{
		GCRecallMonLife(lpTargetObj->m_RecallMon,(int)lpTargetObj->MaxLife,(int)lpTargetObj->Life);
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpTargetObj->m_Index,skillSuccess);
}

int CObjUseSkill::SkillMonsterCall(int aIndex, int MonsterType, int x, int y)
{
	if(gObj[aIndex].MapNumber == MAP_INDEX_ICARUS)
	{
		return false;
	}

	if(CC_MAP_RANGE(gObj[aIndex].MapNumber))
	{
		return false;
	}

	if(gObj[aIndex].m_RecallMon >= 0)
	{
		GCRecallMonLife(aIndex,60,0);
		gObjMonsterCallKill(aIndex);
		return false;
	}

	int result = gObjAddCallMon();

	if(result >= 0)
	{
		gObj[result].X = x;
		gObj[result].Y = y;
		gObj[result].MTX = x;
		gObj[result].MTY = y;
		gObj[result].Dir = 2;
		gObj[result].MapNumber = gObj[aIndex].MapNumber;
		gObjSetMonster(result,MonsterType);
		gObj[result].m_RecallMon = aIndex;
		gObj[result].m_Attribute = 100;
		gObj[result].TargetNumber = (WORD)-1;
		gObj[result].m_ActState.Emotion = 0;
		gObj[result].m_ActState.Attack = 0;
		gObj[result].m_ActState.EmotionCount = 0;
		gObj[result].PathCount = 0;

		gObj[aIndex].m_RecallMon = result;
		gObj[result].m_MoveRange = 15;

		if( gObj[aIndex].m_MPSkillOpt.MpsSummonMonster1 > 0.0f )
		{
			gObj[result].MaxLife += gObj[result].MaxLife/100.0f*gObj[aIndex].m_MPSkillOpt.MpsSummonMonster1;
			gObj[result].Life += gObj[result].Life/100.0f*gObj[aIndex].m_MPSkillOpt.MpsSummonMonster1;
		}

		if( gObj[aIndex].m_MPSkillOpt.MpsSummonMonster2 > 0.0f )
		{
			gObj[result].m_Defense += gObj[result].m_Defense/100.0f*gObj[aIndex].m_MPSkillOpt.MpsSummonMonster2;
		}

		if( gObj[aIndex].m_MPSkillOpt.MpsSummonMonster3 > 0.0f )
		{
			gObj[result].m_AttackDamageMax += gObj[result].m_AttackDamageMax/100.0f*gObj[aIndex].m_MPSkillOpt.MpsSummonMonster3;
			gObj[result].m_AttackDamageMin += gObj[result].m_AttackDamageMin/100.0f*gObj[aIndex].m_MPSkillOpt.MpsSummonMonster3;
		}

		GCRecallMonLife(gObj[result].m_RecallMon,(int)gObj[result].MaxLife,(int)gObj[result].Life);
		return true;
	}
	return false;
}




void CObjUseSkill::SkillBlowOfFury(int aIndex, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex, BOOL isCombo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;
	int count = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int HitCount = 0;
	int bAttack = 0;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 4)
					{
						bAttack = 0;

						if(HitCount > 15)
						{
							bAttack = 0;
						}

						if(HitCount >= 10)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{
							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,500,lpMagic->m_Skill,isCombo);

							if(isCombo != 0)
							{
								GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,aIndex,1);
							}
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}




void CObjUseSkill::SkillWheel(int aIndex, CMagicInf* lpMagic, int aTargetIndex, BOOL isCombo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 3)
					{
						bAttack = 0;

						if(HitCount > 15)
						{
							bAttack = 0;
						}

						if(HitCount >= 10)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{

							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,0,1,0,isCombo,0,0);

							if(isCombo != 0)
							{
								GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,gObj[tObjNum].m_Index,1);
							}
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}




void CObjUseSkill::SkillPowerSlash(int aIndex, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE targetangle, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int StartDis = 1;
	int tObjNum;
	::SkillFrustrum(targetangle,aIndex);
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex))
					{
						bAttack = 0;

						if(HitCount > 10)
						{
							bAttack = 0;
						}
						
						if(HitCount >= 5)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,1,1,0,0,0,0);
						}

						HitCount++;
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}




int CObjUseSkill::SkillDeathPoison(int aIndex, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;
	int count = 0;
	int FirstHit = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;
	LPOBJ lpTargetObj = &gObj[aTargetIndex];


	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(CalDistance(x,y,gObj[tObjNum].X,gObj[tObjNum].Y) < 4)
					{
						bAttack = 0;

						if(HitCount > 10)
						{
							bAttack = 0;
						}
						
						if(HitCount >= 5)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{
							int delay = rand()%500;
							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,delay,lpMagic->m_Skill,0);
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return true;
}




int CObjUseSkill::SkillSuddenIce(int aIndex, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;
	int count = 0;
	int FirstHit = 0;
	int HitCount = 0;
	int bAttack;
	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(CalDistance(x,y,gObj[tObjNum].X,gObj[tObjNum].Y) < 4)
					{
						bAttack = 0;

						if(HitCount > 10)
						{
							bAttack = 0;
						}
						
						if(HitCount >= 5)
						{
							if(rand()%2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{
							int delay = rand()%500;
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,1,1,0,0,0,0);
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return true;
}




BOOL CObjUseSkill::SkillHellFire2(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;

	if(lpObj->SkillHellFire2State == 0)
	{
		return SkillHellFire2Start(aIndex,lpMagic);
	}

	lpObj->SkillHellFire2State = 0;
	int count = 0;
	lpObj->SkillHellFire2Time = 0;

	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(aTargetIndex == tObjNum || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) <= 6)
					{
						gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,600,lpMagic->m_Skill,0);
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,1);
	return true;
}




BOOL CObjUseSkill::SkillHellFire2Start(int aIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->SkillHellFire2State != 0)
	{
		LogAddTD("[%s][%s] SkillHellFire2 error on start",lpObj->AccountID,lpObj->Name);
		return false;
	}

	lpObj->SkillHellFire2Time = GetTickCount();
	lpObj->SkillHellFire2State = 1;
	lpObj->SkillHellFire2Count = 0;

	GCMagicAttackNumberSend(lpObj,AT_SKILL_EXPHELL_START,aIndex,1);
	return true;
}




BOOL CObjUseSkill::SkillKnightBlow(int aIndex, int aTargetIndex, CMagicInf* lpMagic, BOOL isCombo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int StartDis = 1;
	int tObjNum;

	float fangle = (float)GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);

	SkillFrustrum(aIndex,fangle,1.5f,3.0f);

	int count = 0;
	int loopcount = 0;
	int attackcheck;
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					attackcheck = 0;

					if(loopcount == 0)
					{
						attackcheck = 1;
					}
					else if(rand()%3==0)
					{
						attackcheck = 1;
					}

					if(attackcheck != 0)
					{
						if(::SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex))
						{
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,0,1,0,isCombo,0,0);
							if(isCombo != 0)
							{
								GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,gObj[tObjNum].m_Index,1);
							}
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			loopcount++;
			count = 0;
			if(loopcount >= 3)
			{
				break;
			}
		}
	}
	return true;
}




void CObjUseSkill::SkillFrustrum(int aIndex, float fangle, float tx, float ty)
{
	LPOBJ lpObj = &gObj[aIndex];

	float p[MAX_ARRAY_FRUSTRUM][3];

	p[0][0] = -tx;
	p[0][1] = ty;
	p[0][2] = 0;
	p[1][0] = tx;
	p[1][1] = ty;
	p[1][2] = 0;
	p[2][0] = 1.0f;
	p[2][1] = 0;
	p[2][2] = 0;
	p[3][0] = -1.0f;
	p[3][1] = 0;
	p[3][2] = 0;

	float Angle[3];

	Angle[0] = 0;
	Angle[1] = 0;
	Angle[2] = fangle;

	float Matrix[3][MAX_ARRAY_FRUSTRUM];
	float vFrustrum[MAX_ARRAY_FRUSTRUM][3];

	AngleMatrix(Angle,Matrix);

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM; i++)
	{
		VectorRotate((const float *)&p[i],(const float *)&Matrix[0],(float *)&vFrustrum[i]);

		lpObj->fSkillFrustrumX[i] = (int)vFrustrum[i][0] + lpObj->X;
		lpObj->fSkillFrustrumY[i] = (int)vFrustrum[i][1] + lpObj->Y;
	}
}




void CObjUseSkill::SkillFrustrum2(int aIndex, float fangle, float tx, float ty)
{
	LPOBJ lpObj = &gObj[aIndex];


	float p[MAX_ARRAY_FRUSTRUM][3];

	p[0][0] = 0;
	p[0][1] = ty;
	p[0][2] = 0;
	p[1][0] = 0;
	p[1][1] = ty;
	p[1][2] = 0;
	p[2][0] = tx;
	p[2][1] = 1.0f;
	p[2][2] = 0;
	p[3][0] = -tx;
	p[3][1] = 1.0f;
	p[3][2] = 0;

	float Angle[3];

	Angle[0] = 0;
	Angle[1] = 0;
	Angle[2] = fangle;

	float Matrix[3][MAX_ARRAY_FRUSTRUM];
	float vFrustrum[MAX_ARRAY_FRUSTRUM][3];

	AngleMatrix(Angle,Matrix);

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM; i++)
	{
		VectorRotate((const float *)&p[i],(const float *)&Matrix[0],(float *)&vFrustrum[i]);

		lpObj->fSkillFrustrumX[i] = (int)vFrustrum[i][0] + lpObj->X;
		lpObj->fSkillFrustrumY[i] = (int)vFrustrum[i][1] + lpObj->Y;
	}
}




int  CObjUseSkill::GetAngle(int x, int y, int tx, int ty)
{
	double diffX = x - tx;
	double diffY = y - ty;

	double rad = atan2(diffY,diffX);

	int angle = (int)(rad * 180 / 3.141592741012573 + 90);

	if(angle < 0)
	{
		angle += 360;
	}
	return angle;
}




int  CObjUseSkill::CalDistance(int x, int y, int x1, int y1)
{
	if(x == x1 && y == y1)
	{
		return 0;
	}

	float tx = (float)(x - x1);
	float ty = (float)(y - y1);

	return (int)sqrt(tx*tx + ty*ty);
}




BOOL CObjUseSkill::SkillAreaCheck(int angel, int x, int y, int tx, int ty, int dis, int arc)
{
	int minangel = angel - arc;
	int maxangel = angel + arc;

	if(CalDistance(x,y,tx,ty) > dis)
	{
		return false;
	}

	int targetangle = GetAngle(x,y,tx,ty);

	if(minangel < 0)
	{
		if(targetangle > maxangel)
		{
			if(targetangle > minangel + 360)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if(maxangel < targetangle)
		{
			return false;
		}
	}
	else if(maxangel > 360)
	{
		if(targetangle < minangel)
		{
			if(targetangle < maxangel - 360)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if(maxangel < targetangle)
		{
			return false;
		}
	}
	else if(minangel > targetangle)
	{
		return false;
	}
	else if(maxangel < targetangle)
	{
		return false;
	}
	return true;
}




BOOL CObjUseSkill::SkillSpear(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int StartDis = 1;
	int tObjNum;

	int iangle = GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	int count = 0;
	int loopcount = 0;
	int attackcheck;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	if(lpObj->SkillLongSpearChange == 0)
	{
		gObjAttack(lpObj,&gObj[aTargetIndex],lpMagic,0,1,0,0,0,0);
		return true;
	}

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					attackcheck = 0;

					if(loopcount == 0)
					{
						attackcheck = 1;
					}
					else if(rand()%3==0)
					{
						attackcheck = 1;
					}

					if(attackcheck != 0)
					{
						if(SkillSpearHitBox.HitCheck(iangle,lpObj->X,lpObj->Y,gObj[tObjNum].X,gObj[tObjNum].Y))
						{
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,0,1,0,0,0,0);
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return true;
}




BOOL CObjUseSkill::SkillFireBurst(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int StartDis = 1;
	int tObjNum;
	int count = 0;
	int loopcount = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);
	gObjAttack(lpObj,&gObj[aTargetIndex],lpMagic,0,1,0,0,0,0);

	int attackcheck;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;
	int delaytime;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0 && aTargetIndex != tObjNum)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					attackcheck = 0;

					if(loopcount == 0)
					{
						attackcheck = 1;
					}
					else if(rand()%3==0)
					{
						attackcheck = 1;
					}

					if(attackcheck != 0)
					{
						if(gObjCalDistance(&gObj[aTargetIndex],&gObj[tObjNum]) < 3)
						{
							delaytime = (rand()*17)%300 + 500;

							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,delaytime,lpMagic->m_Skill,0);
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return true;
}




BOOL CObjUseSkill::SkillDarkHorseAttack(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	GCMagicAttackNumberSend(lpObj,AT_SKILL_DARKHORSE_ATTACK,lpObj->m_Index,1);

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;
				if( (gObj[tObjNum].Class < 100 || gObj[tObjNum].Class > 110) && gObj[tObjNum].Class != 523 )
				{
	
					if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}
#if (GS_CASTLE == 1)
					else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
					{
						if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
							EnableAttack = 1;
					}
#endif
					else
					{
						int CallMonIndex = gObj[tObjNum].m_Index;
	
						if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
						{
							CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
						}
	
						if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
						{
							EnableAttack = 1;
						}
					}
	
					if(EnableAttack != 0)
					{
						if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 5)
						{
							bAttack = 0;
	
							if(HitCount > 15)
							{
								bAttack = 0;
							}
	
							if(HitCount >= 10)
							{
								if(rand()%2)
								{
									bAttack = 1;
								}
							}
							else
							{
								bAttack = 1;
							}
	
							if(bAttack != 0)
							{
								gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,500,lpMagic->m_Skill,0);
							}
						}
					}
				}
			}
		}
	
		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
return true;
}




BOOL CObjUseSkill::RecallPartyCheck(int aIndex, int skill_level)
{
	LPOBJ lpObj = &gObj[aIndex];
	int number;
	int partynum = 0;

	partynum = lpObj->PartyNumber;

	int partycount;
	LPOBJ lpPartyObj;
	int recallcount = 0;

	if(partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY;n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if(number >= 0)
			{
				lpPartyObj = &gObj[number];

				if(lpPartyObj->m_Index != lpObj->m_Index)
				{
					if(lpPartyObj->MapNumber == lpObj->MapNumber)
					{
						if(lpPartyObj->SkillRecallParty_Time != 0)
						{
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}




void CObjUseSkill::SkillRecallParty(int aIndex, int skill_level)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJ_USER && lpObj->m_RecallMon == -1)
	{
		return;
	}

	if(RecallPartyCheck(aIndex,skill_level) == 0)
	{
		MsgOutput(aIndex,lMsg.Get(1238));
		return;
	}

	if(gMoveCommand.CheckMainToMove(lpObj) == 0)
	{
		char msg[255];
		wsprintf(msg,lMsg.Get(1249));
		GCServerMsgStringSend(msg,lpObj->m_Index,1);
		return;
	}

	if(gMoveCommand.CheckInterfaceToMove(lpObj) == 0)
	{
		char msg[255];
		wsprintf(msg,lMsg.Get(1239));
		GCServerMsgStringSend(msg,lpObj->m_Index,1);
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsDuel(*lpObj)) {
		//char msg[255];
		//wsprintf(msg, lMsg.Get(3433), lpObj->Name);
		//GCServerMsgStringSend(msg, lpObj->m_Index, 1);
		return;
	}
#endif

	//season 4 add-on
	if( lpObj->MapNumber == MAP_INDEX_RAKLIONBOSS &&
		lpObj->Connected == PLAYER_PLAYING &&
		(g_Raklion.GetRaklionState() == RAKLION_STATE_CLOSE_DOOR ||
		g_Raklion.GetRaklionState() == RAKLION_STATE_ALL_USER_DIE ||
		g_Raklion.GetRaklionState() == RAKLION_STATE_NOTIFY_4 ||
		g_Raklion.GetRaklionState() == RAKLION_STATE_END) ) 
	{
		char msg[255];
		wsprintf(msg,lMsg.Get(3417));
		GCServerMsgStringSend(msg,lpObj->m_Index,1);
		return;
	}

	int number;
	int partynum;
	int partycount;
	LPOBJ lpPartyObj;
	int recallcount;

	partynum = 0;
	partynum = lpObj->PartyNumber;

	GCMagicAttackNumberSend(lpObj,AT_SKILL_RECALL_PARTY,lpObj->m_Index,skillSuccess);

	recallcount = 0;

	if(partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY;n ++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if(number >= 0)
			{
				lpPartyObj = &gObj[number];

				if(lpPartyObj->m_Index != lpObj->m_Index)
				{
#if (GS_CASTLE == 1)
					if( g_CastleSiege.GetCastleState() == 7 && lpObj->m_btCsJoinSide != lpPartyObj->m_btCsJoinSide )
					{
						continue;
					}
#endif

					int movelevel = gMoveCommand.GetMoveLevel(lpObj->MapNumber,lpObj->X,lpObj->Y,lpPartyObj->Class);
					int bCheckMainToMove = gMoveCommand.CheckMainToMove(lpPartyObj);
					int bCheckEquipmentToMove = gMoveCommand.CheckEquipmentToMove(lpPartyObj,lpObj->MapNumber);
					int bCheckInterfaceToMove = gMoveCommand.CheckInterfaceToMove(lpPartyObj);

					if(lpPartyObj->Level >= movelevel
						&& movelevel != -1
						&& bCheckMainToMove != false
						&& bCheckEquipmentToMove != false
						&& bCheckInterfaceToMove != false)
					{
						int addx;
						int addy;
						int count = 50;
						int Find = 0;

						while(count--)
						{
							addx = lpObj->X + rand()%9 - 4;
							addy = lpObj->Y + rand()%9 - 4;

							if(gObjCheckTeleportArea(lpObj->m_Index,addx,addy))
							{
								Find = 1;
								break;
							}
						}

						if(Find == 0)
						{
							addx = lpObj->X;
							addy = lpObj->Y;
						}

						lpPartyObj->SkillRecallParty_Time = 7;
						lpPartyObj->SkillRecallParty_MapNumber = lpObj->MapNumber;
						lpPartyObj->SkillRecallParty_X = addx;
						lpPartyObj->SkillRecallParty_Y = addy;

						//Season4 add-on
						if(lpObj->MapNumber == MAP_INDEX_RAKLIONBOSS)
						{
							g_RaklionBattleUserMng.AddUserData(lpPartyObj->m_Index);
						}

						char msg[255];

						wsprintf(msg,lMsg.Get(1251));
						GCServerMsgStringSend(msg,lpPartyObj->m_Index,1);
						recallcount++;
					}
					else
					{
						char msg[255];
						wsprintf(msg,lMsg.Get(1252));
						GCServerMsgStringSend(msg,lpPartyObj->m_Index,1);
					}
				}
			}
		}
	}

	if(recallcount != 0)
	{
		char msg[255];
		wsprintf(msg,lMsg.Get(1244),recallcount);
		GCServerMsgStringSend(msg,lpObj->m_Index,1);
	}
}




void CObjUseSkill::SkillAddCriticalDamage(int aIndex, CMagicInf* lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];
	int number;

	if(lpObj->Type != OBJ_USER && lpObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_DARKLORD)
	{
		return;
	}


	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;

	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	LPOBJ lpPartyObj;
	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));

	partynum = lpObj->PartyNumber;

	int Buffnumber = BUFF_ADD_CRITDMG;

	if( lpMagic->m_Skill == AT_MSKILL_DL_CRITICALDAMAGE3 )
	{
		Buffnumber = BUFF_ADD_CRITDMG2;
	}
	else if( lpMagic->m_Skill == AT_MSKILL_DL_CRITICALDAMAGE4 )
	{
		Buffnumber = BUFF_ADD_CRITDMG3;
	}

	if(partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if(number >= 0)
			{
				lpPartyObj = &gObj[number];

				if(lpObj->MapNumber == lpPartyObj->MapNumber)
				{
					dis = gObjCalDistance(lpObj,&gObj[number]);
					if(dis < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj->m_Index;
					}
				}
			}
		}
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return;
	}

	int addcriticaldamagevalue = (lpObj->Leadership + lpObj->AddLeadership)/lpInfo->m_Arg1 + (lpObj->Energy + lpObj->AddEnergy)/lpInfo->m_Arg2;

	if( addcriticaldamagevalue > lpInfo->m_Arg3 )
	{
		addcriticaldamagevalue = lpInfo->m_Arg3;
	}

	int SkillTime = lpInfo->m_Arg4 + (lpObj->Energy + lpObj->AddEnergy) / lpInfo->m_Arg5;

	if( SkillTime > lpInfo->m_Arg6 )
	{
		SkillTime = lpInfo->m_Arg6;
	}

	if(partynum == -1)
	{
		gObjAddBuffEffect(lpObj,Buffnumber,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,0,0,SkillTime);
		MsgOutput(aIndex,lMsg.Get(1240),SkillTime);
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,skillSuccess);
	}
	else
	{
		for(int n = 0;n < MAX_USER_IN_PARTY;n++)
		{
			if(ApplyPartyIndex[n] != -1)
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];
				gObjAddBuffEffect(lpPartyObj,Buffnumber,ADD_OPTION_CRITICALDMG,addcriticaldamagevalue,0,0,SkillTime);
				MsgOutput(aIndex,lMsg.Get(1240),SkillTime);
				GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpPartyObj->m_Index,skillSuccess);
			}
		}
	}
}




void CObjUseSkill::SkillElectricSpark(int aIndex, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int TargePosx = lpObj->X - (8 - (TargetPos & 15));
	int TargePosy = lpObj->Y - (8 - ((TargetPos & 240) >> 4));

	int tObjNum;
	int count = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int iangle = GetAngle(lpObj->X,lpObj->Y,TargePosx,TargePosy);
	int delaytime;
	int Hit = 0;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(SkillElectricSparkHitBox.HitCheck(iangle,gObj[aIndex].X,gObj[aIndex].Y,gObj[tObjNum].X,gObj[tObjNum].Y))
					{
						delaytime = 500;
						Hit = 1;

						gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,delaytime,lpMagic->m_Skill,0);
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

	if(Hit != 0)
	{
		DecPartyMemberHPandMP(lpObj);
	}
}




int  DecPartyMemberHPandMP(LPOBJ lpObj)
{
	LPOBJ lpPartyObj;
	int partynum = lpObj->PartyNumber;

	if(OBJMAX_RANGE(partynum)==false)
	{
		LogAdd("error : DecPartyMemberHPandMP %s %d",__FILE__,__LINE__);
		return false;
	}

	int partycount = gParty.m_PartyS[partynum].Count;
	int retcount = 0;

	for(int n = 0; n < MAX_USER_IN_PARTY;n++)
	{
		int number = gParty.m_PartyS[partynum].Number[n];

		if(number >= 0)
		{
			lpPartyObj = &gObj[number];

			if(lpObj->MapNumber == lpPartyObj->MapNumber && lpObj->m_Index != number)
			{
				int dis = gObjCalDistance(lpObj,&gObj[number]);
				if(dis < 10)
				{
					lpPartyObj = &gObj[number];

					int declife = (int)(lpPartyObj->Life * 20.0f / 100.0f);
					int decmana = (int)(lpPartyObj->Mana * 5.0f / 100.0f);

					lpPartyObj->Life = lpPartyObj->Life - declife;
					lpPartyObj->Mana = lpPartyObj->Mana - decmana;

					GCReFillSend(lpPartyObj->m_Index,(WORD)lpPartyObj->Life,0xFF,0,lpPartyObj->iShield);
					GCManaSend(lpPartyObj->m_Index,(WORD)lpPartyObj->Mana,0xFF,0,lpPartyObj->BP);

					retcount++;
				}
			}
		}
	}

	return retcount;
}




BOOL CObjUseSkill::SkillSummon(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex)==false)
	{
		return false;
	}

	int iSummonCount = 1;
	int iSummonMonterType = 0;
	BYTE bDoSummon = 1;
	BYTE bSummonSuccess = 0;
	BYTE cSummonX = 0;
	BYTE cSummonY = 0;

	LPOBJ lpObj = &gObj[aIndex];

	switch(lpObj->Class)
	{
	case 161:
		iSummonMonterType = rand()%2+ 147;
		bDoSummon = (rand()%15) ? false : true;
		break;
	case 181:
		iSummonMonterType = rand()%2+ 177;
		bDoSummon = (rand()%15) ? false : true;
		break;
	case 189:
		iSummonMonterType = rand()%2+ 185;
		bDoSummon = (rand()%15) ? false : true;
		break;
	case 197:
		iSummonMonterType = rand()%2+ 193;
		bDoSummon = (rand()%15) ? false : true;
		break;
	case 267:
		iSummonMonterType = rand()%2+ 263;
		bDoSummon = (rand()%15) ? false : true;
		break;
	case 275:
		iSummonMonterType = rand()%2+ 271;
		bDoSummon = (rand()%15) ? false : true;
		break;
	}

	if(bDoSummon == false)
	{
		return false;
	}

	for(int iSC = 0; iSC < iSummonCount; iSC++)
	{
		BYTE cSX = lpObj->X;
		BYTE cSY = lpObj->Y;

		if(gObjGetRandomFreeLocation(lpObj->MapNumber,(BYTE &)cSX,(BYTE &)cSY,2,2,30)==1)
		{
			int result = gObjAddMonster(lpObj->MapNumber);

			if(result >= 0)
			{
				gObj[result].m_PosNum = (WORD)-1;
				gObj[result].X = cSX;
				gObj[result].Y = cSY;
				gObj[result].MapNumber = lpObj->MapNumber;
				gObj[result].TX = gObj[result].X;
				gObj[result].TY = gObj[result].Y;
				gObj[result].m_OldX = gObj[result].X;
				gObj[result].m_OldY = gObj[result].Y;
				gObj[result].Dir = 1;
				gObj[result].StartX = gObj[result].X;
				gObj[result].StartY = gObj[result].Y;
				gObjSetMonster(result,iSummonMonterType);
				gObj[result].m_Attribute = 60;
				gObj[result].MaxRegenTime = 0;
				gObj[result].Dir = rand()%8;
				bSummonSuccess = 1;
				cSummonX = gObj[result].StartX;
				cSummonY = gObj[result].StartY;
			}
		}
	}

	GCMagicAttackNumberSend(lpObj,AT_SKILL_SUMMON,(((cSummonX & 0xFF)&0xFF) | ((cSummonY & 0xFF)&0xFF) << 8) & 0xFFFF,bSummonSuccess);
	return bSummonSuccess;
}




BOOL CObjUseSkill::SkillImmuneToMagic(int aIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];


	if( gObjCheckUsedBuffEffect(lpObj,BUFF_MONSTERMAGIC_RESIST)== FALSE &&
		gObjCheckUsedBuffEffect(lpObj,BUFF_MONCHEMISTRY_RESIST)== FALSE )
	{
		gObjAddBuffEffect(lpObj,BUFF_MONSTERMAGIC_RESIST,0,0,0,0,10);
		return TRUE;
	}
	return false;
}




BOOL CObjUseSkill::SkillImmuneToHarm(int aIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];


	if( gObjCheckUsedBuffEffect(lpObj,BUFF_MONSTERMAGIC_RESIST)== FALSE &&
		gObjCheckUsedBuffEffect(lpObj,BUFF_MONCHEMISTRY_RESIST)== FALSE )
	{
		gObjAddBuffEffect(lpObj,BUFF_MONCHEMISTRY_RESIST,0,0,0,0,10);
		return TRUE;
	}
	return false;
}




BOOL CObjUseSkill::RemoveAllCharacterInvalidMagicAndSkillState(LPOBJ lpObj)
{
	if(lpObj->Type != OBJ_USER)
	{
		return false;
	}

	lpObj->m_SkillAttackTime2 = 0;
	lpObj->m_SkillAttack2 = 0;
	return true;
}




BOOL CObjUseSkill::SkillKnightRush(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];
#if (__CUSTOM__ == 1)

#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);
	gObjAttack(lpObj,&gObj[aTargetIndex],lpMagic,0,1,0,0,0,0);
	return true;
}




BOOL CObjUseSkill::SkillWizardJavalin(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];
#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int iDelay = 500;

	gObjAddAttackProcMsgSendDelay(lpObj,50,aTargetIndex,iDelay,lpMagic->m_Skill,0);
	gObjAddAttackProcMsgSendDelay(lpObj,50,aTargetIndex,iDelay+200,lpMagic->m_Skill,0);
	return true;
}




BOOL CObjUseSkill::SkillElfRush(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);
	gObjAddAttackProcMsgSendDelay(lpObj,50,aTargetIndex,800,lpMagic->m_Skill,0);
	return true;
}




BOOL CObjUseSkill::SkillMagumOneFlash(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);
	gObjAttack(lpObj,&gObj[aTargetIndex],lpMagic,0,1,0,0,0,0);
	gObjAddAttackProcMsgSendDelay(lpObj,50,aTargetIndex,100,lpMagic->m_Skill,0);
	return true;
}




BOOL CObjUseSkill::SkillMagumDeathCannon(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);
	gObjAttack(lpObj,&gObj[aTargetIndex],lpMagic,0,1,0,0,0,0);
	return true;
}




BOOL CObjUseSkill::SkillDarklordSpaceSplit(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);
	gObjAddAttackProcMsgSendDelay(lpObj,50,aTargetIndex,500,lpMagic->m_Skill,0);
	return true;
}




BOOL CObjUseSkill::SkillBrandOfSkill(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	int count = 0;
	gObjAddBuffEffect(lpObj,BUFF_BATTLEROD,0,0,0,0,10);
	GCMagicAttackNumberSend(lpObj,AT_SKILL_BRAND_OF_SKILL,lpObj->m_Index,1);

	while( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(OBJMAX_RANGE(tObjNum))
			{
				if(lpObj->VpPlayer2[count].type == OBJ_USER)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 5)
					{
						gObjAddBuffEffect(&gObj[tObjNum],BUFF_BATTLEROD,0,0,0,0,10);
						GCMagicAttackNumberSend(lpObj,AT_SKILL_BRAND_OF_SKILL,tObjNum,1);
					}
				}
			}
		}

		count++;

		if(count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}




BOOL CObjUseSkill::SkillStun(int aIndex, int aTargetIndex, CMagicInf * lpMagic, BYTE btX, BYTE btY, BYTE btDir, BYTE btTargetPos)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

	int TargePosx = lpObj->X - (8 - (btTargetPos & 15));
	int TargePosy = lpObj->Y - (8 - ((btTargetPos & 240) >> 4));

	int iangle = GetAngle(lpObj->X,lpObj->Y,TargePosx,TargePosy);

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	int tObjNum;
	int count = 0;

	gObjSetKillCount(aIndex,0);

	while( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			if(lpObj->VpPlayer2[count].type == OBJ_USER || lpObj->VpPlayer2[count].type == OBJ_MONSTER)
			{
				tObjNum = lpObj->VpPlayer2[count].number;

				if(OBJMAX_RANGE(tObjNum))
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) <= 4)
					{
						if(SkillElectricSparkHitBox.HitCheck(iangle,lpObj->X,lpObj->Y,gObj[tObjNum].X,gObj[tObjNum].Y))
						{
							gObjAddBuffEffect(&gObj[tObjNum],BUFF_STUN,0,0,0,0,10);
							GCMagicAttackNumberSend(lpObj,AT_SKILL_STUN,tObjNum,1);
							gObjSetPosition(tObjNum,gObj[tObjNum].X,gObj[tObjNum].Y);
						}
					}
				}
			}
		}

		count++;

		if(count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}




BOOL CObjUseSkill::SkillRemoveStun(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{

	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	int tObjNum;
	int count = 0;

	gObjSetKillCount(aIndex,0);
	GCMagicAttackNumberSend(lpObj,AT_SKILL_REMOVAL_STUN,lpObj->m_Index,1);

	while( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{

			tObjNum = lpObj->VpPlayer2[count].number;
			if(OBJMAX_RANGE(tObjNum))
			{
				if(lpObj->VpPlayer2[count].type == OBJ_USER)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 6)
					{
						gObjRemoveBuffEffect(&gObj[tObjNum],BUFF_STUN);
						GCMagicAttackNumberSend(lpObj,AT_SKILL_REMOVAL_STUN,tObjNum,1);
					}
				}
			}
		}

		count++;

		if(count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
return true;
}




BOOL CObjUseSkill::SkillAddMana(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	int tObjNum;
	int count = 0;
	gObjSetKillCount(aIndex,0);

	int iSwelledMana = 0;
	iSwelledMana = (int)((lpObj->MaxMana + lpObj->AddMana) * 50.0f / 100.0f);

	gObjAddBuffEffect(lpObj,BUFF_SWELL_MANA,ADD_OPTION_MANA,iSwelledMana,0,0,60);
	GCManaSend(lpObj->m_Index,(WORD)(lpObj->MaxMana+lpObj->AddMana),0xFE,0,lpObj->MaxBP+lpObj->AddBP);

	if( (lpObj->MaxMana + lpObj->AddMana) < lpObj->Mana)
	{
		lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
		GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
	}

	GCMagicAttackNumberSend(lpObj,AT_SKILL_ADD_MANA,lpObj->m_Index,1);

	while( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(OBJMAX_RANGE(tObjNum) && lpObj->VpPlayer2[count].type == OBJ_USER)
			{
				if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 5)
				{
					if(lpObj->GuildNumber > 0 && gObj[tObjNum].GuildNumber > 0)
					{
						if(lpObj->GuildNumber != gObj[tObjNum].GuildNumber)
						{
							count++;
							continue;
						}


						iSwelledMana = (int)((gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana) * 50.0f / 100.0f);
						gObjAddBuffEffect(&gObj[tObjNum],BUFF_SWELL_MANA,ADD_OPTION_MANA,iSwelledMana,0,0,60);
						GCManaSend(tObjNum,(WORD)(gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana),0xFE,0,gObj[tObjNum].MaxBP + gObj[tObjNum].AddBP);

						if(gObj[tObjNum].Mana > gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana)
						{
							gObj[tObjNum].Mana = gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana;
							GCManaSend(tObjNum,(WORD)gObj[tObjNum].Mana,0xFF,0,gObj[tObjNum].BP);
						}
						GCMagicAttackNumberSend(lpObj,AT_SKILL_ADD_MANA,tObjNum,1);
					}
				}
			}
		}

		count++;

		if(count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}




BOOL CObjUseSkill::SkillCloaking(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	if(OBJMAX_RANGE(aTargetIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	if(lpTargetObj->Type != OBJ_USER)
	{
		LogAddTD("[ANTI-HACK][Cloaking Skill] [%s][%s] Invalid Targeting  TargetType [%d] TargetClass [%d]",
			lpObj->AccountID,lpObj->Name,lpTargetObj->Type,lpTargetObj->Class);
		return false;
	}

	gObjSetKillCount(aIndex,0);

	GCMagicAttackNumberSend(lpObj,AT_SKILL_INVISIBLE,lpObj->m_Index,1);
	gObjAddBuffEffect(lpTargetObj,BUFF_INVISIBILITY,0,0,0,0,60);
	GCMagicAttackNumberSend(lpObj,AT_SKILL_INVISIBLE,aTargetIndex,1);
	return true;
}

BOOL CObjUseSkill::RemoveCloakingEffect(int aIndex)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	gObjRemoveBuffEffect(&gObj[aIndex],BUFF_INVISIBILITY);
	return true;
}

BOOL CObjUseSkill::SkillRemoveCloaking(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( (lpTargetObj->Authority&32)== 32 && gObjCheckUsedBuffEffect(lpTargetObj,BUFF_INVISIBILITY) )
	{
		return false;
	}
 
	int tObjNum;
	int count = 0;

	gObjSetKillCount(aIndex,0);
	GCMagicAttackNumberSend(lpObj,AT_SKILL_REMOVAL_INVISIBLE,lpObj->m_Index,1);

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(OBJMAX_RANGE(tObjNum) && lpObj->VpPlayer2[count].type == OBJ_USER)
			{
				if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 5)
				{
					gObjRemoveBuffEffect(&gObj[tObjNum],BUFF_INVISIBILITY);
					GCMagicAttackNumberSend(lpObj,AT_SKILL_REMOVAL_INVISIBLE,tObjNum,1);
				}
			}
		}

		count++;

		if(count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}




BOOL CObjUseSkill::SkillRemoveMagic(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(OBJMAX_RANGE(aIndex) == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

#if (__CUSTOM__ == 1)
#else
	if(lpObj->MapNumber != MAP_INDEX_CASTLESIEGE && lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE)
	{
		return false;
	}
#endif

	int tObjNum;
	int count = 0;

	gObjSetKillCount(aIndex,0);
	GCMagicAttackNumberSend(lpObj,AT_SKILL_REMOVAL_MAGIC,lpObj->m_Index,1);

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(OBJMAX_RANGE(tObjNum) && lpObj->VpPlayer2[count].type == OBJ_USER)
			{
				if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 5)
				{
					RemoveAllCharacterInvalidMagicAndSkillState(&gObj[tObjNum]);
					GCMagicAttackNumberSend(lpObj,AT_SKILL_REMOVAL_MAGIC,tObjNum,1);
				}
			}
		}

		count++;

		if(count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

int CObjUseSkill::SkillFenrirAttack(int aIndex,int aTargetIndex,CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	if(lpObj->Level < 300)
	{
		return false;
	}

	while( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if( (gObj[tObjNum].Class < 100 || gObj[tObjNum].Class > 110) && gObj[tObjNum].Class != 523 )
				{
					if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}
#if (GS_CASTLE == 1)
					else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
					{
						if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
							EnableAttack = 1;
					}
#endif
					else
					{
						int CallMonIndex = gObj[tObjNum].m_Index;


						if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
						{
							CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
						}

						if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex]) == 1)
						{
							EnableAttack = 1;
						}
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 6)
					{
						bAttack = 0;

						if(HitCount >= 5)
						{
							bAttack = 0;
						}
						else
						{
							bAttack = 1;
						}

						if(tObjNum == aTargetIndex)
						{
							bAttack = 1;
						}

						if(bAttack != 0)
						{
							gObjAddAttackProcMsgSendDelay(lpObj,53,tObjNum,300,lpMagic->m_Skill,0);
							HitCount++;
						}
					}
				}
			}
		}

		count++;

		if(count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////

int CObjUseSkill::SkillInfinityArrow(int aIndex,int aTargetIndex,CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJ_USER)
	{
		return false;
	}

	if(lpObj->Class != CLASS_ELF || lpObj->ChangeUP == 0)
	{
		return false;
	}

	if(lpObj->m_Index != aTargetIndex)
	{
		return false;
	}

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW) == TRUE )
	{
		return false;
	}
	

	gObjAddBuffEffect(lpObj,BUFF_INFINITY_ARROW,0,0,0,0,g_SkillAdditionInfo.GetInfinityArrowSkillTime());
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,1);

	LogAddTD("[%s][%s] Use Infinity Arrow Skill (Time:%d)(Character Level : %d)(ChangeUp: %d)",
		lpObj->AccountID,lpObj->Name,g_SkillAdditionInfo.GetInfinityArrowSkillTime(),lpObj->Level,lpObj->ChangeUP);
	return true;
}

BOOL CObjUseSkill::SkillFireScream(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	if(g_SkillAdditionInfo.GetFireScreamSkill() == false)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(gObjIsConnected(lpObj->m_Index) == false && gObjIsConnected(lpTargetObj->m_Index) == false)
	{
		return false;
	}

	if(lpObj->Type != OBJ_USER)
	{
		return false;
	}

	if(lpObj->Class != CLASS_DARKLORD)
	{
		return false;
	}

	gObjAttack(lpObj,lpTargetObj,lpMagic,0,0,0,0,0,0);
	return true;
}



BOOL CObjUseSkill::FireScreamExplosionAttack(LPOBJ lpObj, LPOBJ lpTargetObj, int iAttackDamage)
{

	if(gObjIsConnected(lpObj->m_Index) == false && gObjIsConnected(lpTargetObj->m_Index) == false)
	{
		return false;
	}

	if(rand()%10000 > g_SkillAdditionInfo.GetFireScreamExplosionRate())
	{
		return false;
	}

	int iExplosionDamage = iAttackDamage / 10;
	int iTarObjNum;

	if(iExplosionDamage < 1)
	{
		return true;
	}

	int iCount = 0;
	int bEnableAttack;

	while( true )
	{
		if(lpObj->VpPlayer2[iCount].state != 0)
		{
			iTarObjNum = lpObj->VpPlayer2[iCount].number;

			if(iTarObjNum >= 0)
			{
				bEnableAttack = 0;

				if(gObj[iTarObjNum].Type == OBJ_MONSTER && gObj[iTarObjNum].m_RecallMon < 0)
				{
					bEnableAttack = 1;
				}
				else if(lpTargetObj->Type == OBJ_USER && lpObj->Type == OBJ_USER && gObj[iTarObjNum].Type != OBJ_NPC)
				{
					bEnableAttack = 1;
				}
				else if(lpTargetObj->m_cChaosCastleIndex == gObj[iTarObjNum].m_cChaosCastleIndex && lpTargetObj->m_cChaosCastleIndex != -1)
				{
					bEnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[iCount].type != OBJ_NPC && gObj[iTarObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[iTarObjNum].m_btCsJoinSide )
						bEnableAttack = 1;
				}
#endif
				if(bEnableAttack != 0)
				{
					if(lpObj->m_Index != gObj[iTarObjNum].m_Index)
					{
						int iSkillDistance = gObjCalDistance(lpTargetObj,&gObj[iTarObjNum]);

						if(iSkillDistance <= g_SkillAdditionInfo.GetFireScreamExplosionAttackDistance())
						{
							gObjAddMsgSendDelay(lpObj,54,iTarObjNum,100,iExplosionDamage);
						}
					}
				}
			}
		}

		iCount++;

		if(iCount > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return true;
}


void CObjUseSkill::SpecificSkillAdditionTreat(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf * lpMagic, int iAttackDamage)
{
	int bySkillNum = lpMagic->m_Skill;

	if( bySkillNum == AT_SKILL_FIRESCREAM ||
		bySkillNum == AT_MSKILL_DL_FIRESCREAM1 )
	{
		gObjUseSkill.FireScreamExplosionAttack(lpObj,lpTargetObj,iAttackDamage);
	}
	else if( bySkillNum == AT_MSKILL_DL_FIRESCREAM2 )
	{
		float fSkillValue = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill),lpMagic->m_Level);
		gObjUseSkill.FireScreamExplosionAttack(lpObj,lpTargetObj,iAttackDamage+fSkillValue);
	}
	else if( bySkillNum == AT_SKILL_LIGHTSHOCK ||
		     bySkillNum == AT_MSKILL_SUM_LIGHTNSHOCK1 )
	{
		GCStateInfoSend(lpTargetObj,1,0x55);
		GCStateInfoSend(lpTargetObj,0,0x55);
	}
	else if( bySkillNum == AT_SKILL_FLAMESTRIKE ||
			 bySkillNum == AT_MSKILL_MG_FLAMESTRIKE1 ||
			 bySkillNum == AT_MSKILL_MG_FLAMESTRIKE2 )
	{
		GCStateInfoSend(lpTargetObj,1,0x53);
		GCStateInfoSend(lpTargetObj,0,0x53);
	}
	else if( bySkillNum == AT_SKILL_GIGANTICSTORM ||
		bySkillNum == AT_MSKILL_MG_GIGANTICSTORM1 )
	{
		GCStateInfoSend(lpTargetObj,1,0x54);
		GCStateInfoSend(lpTargetObj,0,0x54);
	}
	else if( bySkillNum == AT_MSKILL_DK_BLOODSTORM || bySkillNum == AT_MSKILL_DK_BLOODSTORM1 )
	{
		gObjUseSkill.BloodStormExplosionAttack(lpObj,lpTargetObj,iAttackDamage);
	}
}

int CObjUseSkill::RemoveSpecificSkillState(LPOBJ lpObj, int iSkillNumber)
{
	if(lpObj->Type != OBJ_USER)
	{
		return false;
	}

	if(iSkillNumber == AT_SKILL_ATTACK)
	{
		gObjRemoveBuffEffect(lpObj,BUFF_ADD_ATTACK);
	}
	else if(iSkillNumber == AT_SKILL_DEFENSE)
	{
		gObjRemoveBuffEffect(lpObj,BUFF_ADD_DEFENSE);
	}
	return true;
}

int CObjUseSkill::SkillLightExplode(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;
	int tObjNum;

	int Angle = GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	int count = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					tObjNum == aTargetIndex && 
					gCheckSkillDistance(aIndex,aTargetIndex,lpMagic->m_Skill) != FALSE )
				{
					gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,700,lpMagic->m_Skill,0);
					bSuccess = TRUE;
					break;
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bSuccess;
}

int CObjUseSkill::SkillLightExplode(LPOBJ lpObj,LPOBJ lpTargetObj,int SkillNumber,int AttackDamage,int Distance,int DmgPerc)
{
	if( gObjIsConnected(lpObj->m_Index) == FALSE &&
		gObjIsConnected(lpTargetObj->m_Index) == FALSE )
	{
		return false;
	}

	if( AttackDamage <= 0 )
	{
		return false;
	}

	int Damage = AttackDamage * DmgPerc / 100;
	int tObjNum;
	int count = 0;
	int EnableAttack;

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(lpTargetObj->m_cChaosCastleIndex != -1 && lpTargetObj->m_cChaosCastleIndex == gObj[tObjNum].m_cChaosCastleIndex)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else if(lpTargetObj->Type == OBJ_USER && lpObj->Type == OBJ_USER && gObj[tObjNum].Type != OBJ_NPC)
				{
					EnableAttack = 1;
				}

				if( EnableAttack != 0 &&
					lpObj->m_Index != gObj[tObjNum].m_Index )
				{
					int Dis = gObjCalDistance(lpTargetObj,&gObj[tObjNum]);

					if( Dis <= Distance )
					{
						gObjAddMsgSendDelay(lpObj,0x3A,tObjNum,300,Damage);
					}
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
return TRUE;
}

int CObjUseSkill::SkillDrainLife(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;
	int tObjNum;

	int Angle = GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	int count = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					tObjNum == aTargetIndex && 
					gCheckSkillDistance(aIndex,aTargetIndex,lpMagic->m_Skill) != FALSE )
				{
					gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,700,lpMagic->m_Skill,0);
					bSuccess = TRUE;
					break;
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bSuccess;
}

struct PMSG_SKILL_TARGET_COUNT
{
	PBMSG_HEAD2 h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD Index;
	BYTE Count;
};

struct PMSG_SKILL_TARGET
{
	WORD Target;
};

int CObjUseSkill::SkillChainLightning(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	int bSuccess = TRUE;
	int tObjNum;
	int count = 0;

	int Target[3];

	Target[0] = -1;
	Target[1] = -1;
	Target[2] = -1;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	if( gCheckSkillDistance(aIndex,aTargetIndex,lpMagic->m_Skill) == FALSE )
	{
		return FALSE;
	}

	Target[0] = aTargetIndex;

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 )
				{
					if( gObj[tObjNum].X >= (lpTargetObj->X -1) &&
						gObj[tObjNum].X <= (lpTargetObj->X +1) &&
						gObj[tObjNum].Y >= (lpTargetObj->Y -1) &&
						gObj[tObjNum].Y <= (lpTargetObj->Y +1) &&
						gObj[tObjNum].Type == OBJ_MONSTER )
					{
						Target[1] = tObjNum;

						if(Target[2] != -1)
							break;
					}
					else if( gObj[tObjNum].X >= (lpTargetObj->X -2) &&
						gObj[tObjNum].X <= (lpTargetObj->X +2) &&
						gObj[tObjNum].Y >= (lpTargetObj->Y -2) &&
						gObj[tObjNum].Y <= (lpTargetObj->Y +2) &&
						gObj[tObjNum].Type == OBJ_MONSTER )
					{
						Target[2] = tObjNum;
	
						if(Target[1] != -1)
							break;
					}
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

	if( Target[2] == -1 )
	{
		Target[2] = Target[1];
	}

	if( Target[1] == -1 )
	{
		Target[1] = Target[0];
		Target[2] = Target[1];
	}

	int lOfs = 0;
	BYTE cBUFF[256];
	PMSG_SKILL_TARGET_COUNT pMsg;

	pMsg.MagicNumberH = SET_NUMBERH(lpMagic->m_Skill);
	pMsg.MagicNumberL = SET_NUMBERL(lpMagic->m_Skill);
	pMsg.Index = aIndex;
	pMsg.Count = 3;

	lOfs += sizeof(pMsg);

	int i;
	for(i = 0; i < 3; i++)
	{
		WORD wIndex = Target[i];
		memcpy(&cBUFF[lOfs],&wIndex,2);
		lOfs+= 2;
	}

	PHeadSubSetB((LPBYTE)&pMsg,0xBF,10,lOfs);
	memcpy(&cBUFF[0],&pMsg,sizeof(pMsg));

	if( lpObj->Type == OBJ_USER )
	{
		DataSend(lpObj->m_Index,(LPBYTE)&cBUFF[0],pMsg.h.size);
	}

	MsgSendV2(lpObj,(LPBYTE)&cBUFF[0],pMsg.h.size);

	for(i = 0; i < 3; i++)
	{
		gObjAddAttackProcMsgSendDelay(lpObj,50,Target[i],200 + 200*i,lpMagic->m_Skill,i+1);
	}
return bSuccess;
}

int CObjUseSkill::SkillCurse(int aIndex,CMagicInf* lpMagic,BYTE X,BYTE Y,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;
	int count = 0;
	int tObjNum;
	int DuelIndex = lpObj->m_iDuelUser;

	int Angle = GetAngle(lpObj->X,lpObj->Y,X,Y);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);
	int Distance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);
	int EnableAttack;

	if( CalDistance(lpObj->X,lpObj->Y,X,Y) <= Distance )
	{
		while( TRUE )
		{
			if(lpObj->VpPlayer2[count].state != 0)
			{
				tObjNum = lpObj->VpPlayer2[count].number;

				if( tObjNum >= 0 )
				{
					EnableAttack = 0;

					if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}
					else
					{
						int CallMonIndex = gObj[tObjNum].m_Index;
	
						if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
						{
							CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
						}
	
						if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
						{
							EnableAttack = 1;
						}
					}
					
					if( EnableAttack != 0 )
					{
						if( gObj[tObjNum].X >= (X -2) &&
						gObj[tObjNum].X <= (X +2) &&
						gObj[tObjNum].Y >= (Y -2) &&
						gObj[tObjNum].Y <= (Y +2) )
						{
							gObj[tObjNum].lpAttackObj = lpObj;
							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,1000,lpMagic->m_Skill,0);
							bSuccess = TRUE;
						}
					}
				}
			}

			count++;

			if( count > MAX_VIEWPORT -1)
			{
				break;
			}
		}
	}
return bSuccess;
}


int CObjUseSkill::SkillDamageReflect(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if( lpTargetObj->Type != OBJ_USER || lpObj->Class != CLASS_SUMMONER )
	{
		return FALSE;
	}

	int bSuccess = FALSE;
	int tObjNum;

	int Angle = GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	int count = 0;
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}

	int Reflect = lpInfo->m_Arg1 + lpObj->Energy / lpInfo->m_Arg2;

	if( Reflect > lpInfo->m_Arg3 )
		Reflect = lpInfo->m_Arg3;

	int Time = lpInfo->m_Arg4 + lpObj->Energy / lpInfo->m_Arg5;

	if( Time > lpInfo->m_Arg6 )
		Time = lpInfo->m_Arg6;

	if( aIndex == aTargetIndex )
	{
		if( gObjCheckPowerfulEffect(lpObj,BUFF_DAMAGEREFLECT,Reflect,0) == TRUE )
		{
			GCMagicAttackNumberSend(lpObj,0xD9,lpTargetObj->m_Index,0);
			return FALSE;
		}

		gObjAddBuffEffect(lpObj,BUFF_DAMAGEREFLECT,ADD_OPTION_REFLECTDAMAGE,Reflect,0,0,Time);
		bSuccess = TRUE;
	}
	else
	{
		while( TRUE )
		{
			if( CC_MAP_RANGE(lpObj->MapNumber) )
			{
				break;
			}

			if(lpObj->VpPlayer2[count].state != 0)
			{
				tObjNum = lpObj->VpPlayer2[count].number;

				if( tObjNum >= 0 )
				{
					EnableAttack = FALSE;

					if( lpObj->VpPlayer2[count].type == OBJ_USER )
					{
						EnableAttack = TRUE;
					}
					else if( tObjNum == aTargetIndex || DuelIndex != tObjNum)
					{
						EnableAttack = TRUE;
					}

					if( EnableAttack != 0 )
					{
						if( tObjNum == aTargetIndex )
						{
							if( gCheckSkillDistance(aIndex,aTargetIndex,lpMagic->m_Skill) )
							{
								lpTargetObj->lpAttackObj = lpObj;

								if( gObjCheckPowerfulEffect(lpTargetObj,BUFF_DAMAGEREFLECT,Reflect,0) == TRUE )
								{
									GCMagicAttackNumberSend(lpTargetObj,0xD9,lpTargetObj->m_Index,0);
									return FALSE;
								}

								gObjAddBuffEffect(lpTargetObj,BUFF_DAMAGEREFLECT,ADD_OPTION_REFLECTDAMAGE,Reflect,0,0,Time);
								bSuccess = TRUE;
								break;
							}
						}
					}
				}
			}

			count++;

			if( count > MAX_VIEWPORT -1)
			{
				break;
			}
		}
	}
return bSuccess;
}

int CObjUseSkill::SkillSleep(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;
	int tObjNum;

	int Angle = GetAngle(lpObj->X,lpObj->Y,gObj[aTargetIndex].X,gObj[aTargetIndex].Y);
	int count = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE(lpObj->MapNumber) ||
						 IT_MAP_RANGE(lpObj->MapNumber) ||
						 lpObj->MapNumber == MAP_INDEX_VULCANUS )
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					tObjNum == aTargetIndex && 
					gCheckSkillDistance(aIndex,aTargetIndex,lpMagic->m_Skill) != FALSE )
				{
					gObjAttack(lpObj,&gObj[aTargetIndex],lpMagic,1,0,0,0,0,0);
					bSuccess = TRUE;
					break;
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bSuccess;
}

int CObjUseSkill::SkillWeakness(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE(lpObj->MapNumber) ||
						 IT_MAP_RANGE(lpObj->MapNumber) ||
						 lpObj->MapNumber == MAP_INDEX_VULCANUS)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
					{
						if( gObj[tObjNum].Class != 283 &&
							gObj[tObjNum].Class != 277 )
							EnableAttack = 1;
					}
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					gObjCalDistance(lpObj,&gObj[tObjNum]) < 5 )
				{
					gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,700,lpMagic->m_Skill,0);
					HitCount++;

					if(HitCount >= 5)
					{
						break;
					}
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bSuccess;
}

int CObjUseSkill::SkillInnovation(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	while( TRUE )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if( tObjNum >= 0 )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE(lpObj->MapNumber) ||
						 IT_MAP_RANGE(lpObj->MapNumber) ||
						 lpObj->MapNumber == MAP_INDEX_VULCANUS)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
					{
						if( gObj[tObjNum].Class != 283 &&
							gObj[tObjNum].Class != 277 )
							EnableAttack = 1;
					}
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != 0 &&
					gObjCalDistance(lpObj,&gObj[tObjNum]) < 5 )
				{
					gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,700,lpMagic->m_Skill,0);
					HitCount++;

					if(HitCount >= 5)
					{
						break;
					}
				}
			}
		}

		count++;

		if( count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

return bSuccess;
}

BOOL CObjUseSkill::SkillBerserker(int aIndex,CMagicInf* lpMagic)
{
	if( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}

	int iTime = lpInfo->m_Arg3 + lpObj->Energy / lpInfo->m_Arg4;

	if( iTime > lpInfo->m_Arg5 )
	{
		iTime = lpInfo->m_Arg5;
	}

	int iIncDmg = lpObj->Energy / lpInfo->m_Arg1;

	if( iIncDmg > lpInfo->m_Arg2 )
	{
		iIncDmg = lpInfo->m_Arg2;
	}

	int iDecDef = iIncDmg / 2;

	gObjAddBuffEffect(lpObj,BUFF_BERSERKER,ADD_OPTION_BERSERKMANA_PERC,iIncDmg,ADD_OPTION_BERSERKLIFE_PERC,iDecDef,iTime);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,lpObj->m_Index,TRUE);

return true;
}
void CObjUseSkill::SkillRecovery(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	int skillSuccess = true;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if(lpTargetObj->Type != OBJ_USER && lpTargetObj->m_RecallMon == -1)
	{
		return;
	}

	if(lpObj->Class != CLASS_ELF)
	{
		return;
	}

	if(CC_MAP_RANGE(lpObj->MapNumber) && lpObj->m_Index != lpTargetObj->m_Index)
	{
		return;
	}

	if(gObjDuelCheck(lpTargetObj) )
	{
		lpTargetObj = lpObj;
	}

	if( lpTargetObj->iShield >= lpTargetObj->iMaxShield+lpTargetObj->iAddShield )
	{
		GCMagicAttackNumberSend(lpObj,AT_SKILL_RECOVERY,lpTargetObj->m_Index,FALSE);
		return;
	}

	lpTargetObj->iShield += (lpObj->Energy / 4)+lpObj->Level;

	if( lpTargetObj->iShield > lpTargetObj->iMaxShield+lpTargetObj->iAddShield )
	{
		lpTargetObj->iShield = lpTargetObj->iMaxShield+lpTargetObj->iAddShield;
	}

	if( lpTargetObj->Type == OBJ_USER )
	{

		GCReFillSend(lpTargetObj->m_Index,(WORD)lpTargetObj->Life,0xFF,0,lpTargetObj->iShield);
		GCMagicAttackNumberSend(lpObj,AT_SKILL_RECOVERY,lpTargetObj->m_Index,skillSuccess);

		for(int n = 0; n < MAX_VIEWPORT; n++)
		{
			if(lpObj->VpPlayer2[n].type == OBJ_MONSTER && lpObj->VpPlayer2[n].state != 0)
			{
				LPOBJ lpMonster = &gObj[lpObj->VpPlayer2[n].number];
				if(lpMonster->m_iCurrentAI != 0)
				{
					lpMonster->m_Agro.IncAgro(lpObj->m_Index,(lpObj->Energy / 5)/40);
				}
			}
		}
	}
}

void CObjUseSkill::SkillDestruct(int aIndex,CMagicInf* lpMagic,BYTE X,BYTE Y,int aTargetIndex)
{
	SkillAreaMonsterAttack(aIndex,lpMagic,X,Y,aTargetIndex,3,1,500);
}

BOOL CObjUseSkill::SkillExpWizard(int aIndex,int aTargetIndex,CMagicInf* lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER )
	{
		return false;
	}

	if( lpObj->Class != CLASS_WIZARD )
	{
		return false;
	}

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_EXP_WIZARDRY) == TRUE ||
		gObjCheckUsedBuffEffect(lpObj,BUFF_EXP_WIZARDRY2) == TRUE ||
		gObjCheckUsedBuffEffect(lpObj,BUFF_EXP_WIZARDRY3) == TRUE )
	{
		return false;
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}

	int AddMinDmg = (lpObj->Energy/lpInfo->m_Arg1)*lpInfo->m_Arg2;

	if( AddMinDmg > lpInfo->m_Arg3 )
	{
		AddMinDmg = lpInfo->m_Arg3;
	}

	gObjAddBuffEffect(lpObj,BUFF_EXP_WIZARDRY,ADD_OPTION_MAGICDAMAGEMIN,AddMinDmg,0,0,lpInfo->m_Arg4);
	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aIndex,TRUE);
	return true;
}

BOOL CObjUseSkill::SkillLightShock(int aIndex, int aTargetIndex,CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tmpvar = 0;
	int bAttack = 0;
	int tObjNum;
	int count = 0;
	int tmpvar2 = 0;
	int tmpvar3 = 7;
	int HitCount = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int EnableAttack;
	int bForceAttack = TRUE;
	int DuelIndex = lpObj->m_iDuelUser;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0 && gObj[tObjNum].Live )
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE( lpObj->MapNumber ) || IT_MAP_RANGE( lpObj->MapNumber ) 
//					|| lpObj->MapNumber == MAP_INDEX_VULCANUS
					)
				{
					EnableAttack = 1;
				}
				else if( (tObjNum == aTargetIndex && bForceAttack == TRUE ) || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) < 7)
					{
						HitCount++;

						if(HitCount > 5)
						{
							if( rand()%2 )
							{
								bAttack = TRUE;
							}
						}
						else
						{
							bAttack = TRUE;
						}

						if(HitCount > 12)
						{
							bAttack = FALSE;
						}

						if(bAttack == TRUE)
						{
							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,250,lpMagic->m_Skill,0);
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
return TRUE;
}


void CObjUseSkill::SkillFlameStrike(int aIndex, CMagicInf * lpMagic,BYTE targetangle, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = TRUE;
	int tObjNum;

	SkillFrustrum3(aIndex,targetangle,2.0f,4.0f,5.0f,0);

	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if( tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if( SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex) )
					{
						bAttack = FALSE;

						if(HitCount > 8)
						{
							if( rand()%2 )
							{
								bAttack = TRUE;
							}
						}
						else
						{
							bAttack = TRUE;
						}

						if(bAttack != FALSE)
						{
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,0,0,0,0,0);
							gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,0,0,0,0,0);
						}

						HitCount++;
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}

}


void CObjUseSkill::SkillGiganticStorm(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex)
{
	SkillAreaMonsterAttack(aIndex,lpMagic,x,y,aTargetIndex,6,1,400);
}

void CObjUseSkill::SkillChaoticDesier(int aIndex, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = TRUE;

	SkillFrustrum3(aIndex,TargetPos,1.5f,6.0f,1.5f,0);

	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if( tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != FALSE && SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex) )
				{
					bAttack = FALSE;

					if( HitCount > 8 )
					{
						if(rand()%2)
						{
							bAttack = TRUE;
						}
					}
					else
					{
						bAttack = TRUE;
					}

					if( bAttack != FALSE )
					{
						gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,250,lpMagic->m_Skill,0);
					}

					if( OBJMAX_RANGE(aTargetIndex) && aTargetIndex == tObjNum)
					{
						gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,300,lpMagic->m_Skill,0);
					}

					HitCount++;
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

BOOL CObjUseSkill::SkillLagle(int aIndex,CMagicInf* lpMagic,BYTE x,BYTE y,int aTargetIndex,int type)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bSuccess = FALSE;

	if( type == 59 )
	{
		GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,TRUE);
	}

	bSuccess = SkillAreaMonsterAttack(aIndex,lpMagic,x,y,aTargetIndex,3,1,0);

	if( type >= 59 && type < 64 && OBJMAX_RANGE(aTargetIndex) )
	{
		gObjAddMsgSendDelay(lpObj,59,aTargetIndex,1000,((((x&0xFF)&0xFF | ((y&0xFF)&0xFF)<<8)&0xFFFF)&0xFFFF)&0xFFFF | (((type+1)&0xFFFF)&0xFFFF)<<16);
	}

return bSuccess;
}

BOOL CObjUseSkill::SkillAreaMonsterAttack(int aIndex,CMagicInf *lpMagic,BYTE x,BYTE y,int aTargetIndex,int aDistance,int bPVP,int nDelay)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bRet = FALSE;
	int count = 0;
	int tObjNum;
	int DuelIndex = lpObj->m_iDuelUser;
	int nHitCount = 0;
	int bAttack = FALSE;
	int nMinCount = 4;
	int nMaxCount = 8;

	if( lpMagic->m_Skill == AT_SKILL_GIGANTICSTORM )
	{
		nMinCount = 8;
		nMaxCount = 12;
	}

	int iSkillDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	if( CalDistance(lpObj->X,lpObj->Y,x,y) <= iSkillDistance )
	{
		while( TRUE )
		{
			if(lpObj->VpPlayer2[count].state != 0)
			{
				tObjNum = lpObj->VpPlayer2[count].number;

				if( tObjNum >= 0 )
				{
					int EnableAttack = 0;

					if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if( (tObjNum == aTargetIndex && bPVP == 1) || DuelIndex ==tObjNum )
					{
						EnableAttack = 1;
					}
#if (GS_CASTLE == 1)
					else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
					{
						if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
							EnableAttack = 1;
					}	
#endif
					else
					{
						int CallMonIndex = gObj[tObjNum].m_Index;
	
						if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
						{
							CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
						}
	
						if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
						{
							EnableAttack = 1;
						}
					}

					if( EnableAttack != 0 )
					{
						if( gObj[tObjNum].X >= (x -aDistance) &&
							gObj[tObjNum].X <= (x +aDistance) &&
							gObj[tObjNum].Y >= (y -aDistance) &&
							gObj[tObjNum].Y <= (y +aDistance) )
						{
							gObj[tObjNum].lpAttackObj = lpObj;
							nHitCount++;
	
							if( nHitCount > nMinCount )
							{
								if(rand()%2)
								{
									bAttack = TRUE;
								}
							}
							else
							{
								bAttack = TRUE;
							}
	
							if( nHitCount > nMaxCount )
							{
								bAttack = FALSE;
							}

							if( bAttack == TRUE )
							{
								if( nDelay == 0 )
								{
									if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DRAGONLORE ||
										lpMagic->m_Skill == AT_SKILL_PHOENIXSHOT )
									{
										if( lpMagic->m_Skill == AT_SKILL_PHOENIXSHOT && rand()%100 < 10 )
										{
											int nEffectValue = 0;

											if( gObj[tObjNum].Type == OBJ_MONSTER )
											{
												nEffectValue = 50;
											}
											else if( gObj[tObjNum].Type == OBJ_USER )
											{
												nEffectValue = 20;
											}
											else
											{
												nEffectValue = 0;
											}

											gObjAddBuffEffect(&gObj[tObjNum],BUFF_DECREASE_DEFRATE,SUB_OPTION_SUCCESSFULBLOCKING,nEffectValue,0,0,10);
										}

										for(int i = 1; i <= 4; i++)
										{
											if( i == 1 )
											{
												gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,TRUE,0,0,i,0);
											}
											else
											{
												gObjAttack(lpObj,&gObj[tObjNum],lpMagic,FALSE,TRUE,0,0,i,0);
											}
										}
									}
									else
									{
										gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,FALSE,0,0,0,0);
									}
								}
								else
								{
									if( lpMagic->m_Skill == AT_SKILL_GIGANTICSTORM &&
										lpObj->Class == 561 )
									{
										if( gObjCheckUsedBuffEffect(&gObj[tObjNum],BUFF_ICE) == FALSE )
										{
											gObj[tObjNum].DelayActionTime = 800;
											gObj[tObjNum].DelayLevel = 1;
											gObj[tObjNum].lpAttackObj = lpObj;
											gObjAddBuffEffect(&gObj[tObjNum],BUFF_ICE,0,0,0,0,3);
										}
									}

									gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,nDelay,lpMagic->m_Skill,0);
								}
							}

							bRet = TRUE;
						}
					}
				}
			}

			count++;

			if( count > (MAX_VIEWPORT-1) )
			{
				break;
			}
		}
	}
	return bRet;
}

void CObjUseSkill::SkillFrustrum3(int aIndex,BYTE aTargetPos,float f1,float f2,float f3,float f4)
{
	LPOBJ lpObj = &gObj[aIndex];

	float p[MAX_ARRAY_FRUSTRUM][3];

	p[0][0] = -f1;
	p[0][1] = f2;
	p[0][2] = 0;
	p[1][0] = f1;
	p[1][1] = f2;
	p[1][2] = 0;
	p[2][0] = f3;
	p[2][1] = f4;
	p[2][2] = 0;
	p[3][0] = -f3;
	p[3][1] = f4;
	p[3][2] = 0;

	float Angle[3];

	Angle[0] = 0;
	Angle[1] = 0;
	Angle[2] = (float)(aTargetPos * 360 / 255);

	float Matrix[3][MAX_ARRAY_FRUSTRUM];
	float vFrustrum[MAX_ARRAY_FRUSTRUM][3];

	AngleMatrix(Angle,Matrix);

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM; i++)
	{
		VectorRotate((const float *)&p[i],(const float *)&Matrix[0],(float *)&vFrustrum[i]);

		lpObj->fSkillFrustrumX[i] = (int)vFrustrum[i][0] + lpObj->X;
		lpObj->fSkillFrustrumY[i] = (int)vFrustrum[i][1] + lpObj->Y;
	}
}

void CObjUseSkill::SkillMultiShot(int aIndex,CMagicInf* lpMagic,BYTE targetpos,int aTargetIndex)
{
LPOBJ lpObj = &gObj[aIndex];

	SkillFrustrum3(aIndex,targetpos,6.0f,7.0f,1.0f,0);

	float p[5][3];
	float Matrix[3][MAX_ARRAY_FRUSTRUM];
	float vFrustrum[5][3];

	p[4][0] = 0;
	p[4][1] = 6.0f;
	p[4][2] = 0;

	p[0][0] = 0;
	p[0][1] = 0;
	p[0][2] = 40.0f;

	p[1][0] = 0;
	p[1][1] = 0;
	p[1][2] = 20.0f;

	p[2][0] = 0;
	p[2][1] = 0;
	p[2][2] = 340.0f;

	p[3][0] = 0;
	p[3][1] = 0;
	p[3][2] = 320.0f;

	AngleMatrix((const float *)&p[0],Matrix);
	VectorRotate((const float *)&p[4],(const float *)&Matrix[0],(float *)&vFrustrum[0]);

	AngleMatrix((const float *)&p[1],Matrix);
	VectorRotate((const float *)&p[4],(const float *)&Matrix[0],(float *)&vFrustrum[1]);

	vFrustrum[2][0] = 0;
	vFrustrum[2][1] = 6.0f;
	vFrustrum[2][2] = 0;

	AngleMatrix((const float *)&p[2],Matrix);
	VectorRotate((const float *)&p[4],(const float *)&Matrix[0],(float *)&vFrustrum[3]);

	AngleMatrix((const float *)&p[3],Matrix);
	VectorRotate((const float *)&p[4],(const float *)&Matrix[0],(float *)&vFrustrum[4]);

	p[0][0] = 0;
	p[0][1] = 0;
	p[0][2] = targetpos*360/255;

	AngleMatrix((const float *)&p[0],Matrix);

	int fSkillFrustrumX[5];
	int fSkillFrustrumY[5];

	float vFrustrum2[5][3];

	for(int i = 0; i < 5; i++)
	{
		VectorRotate(vFrustrum[i],(const float *)&Matrix[0],vFrustrum2[i]);

		fSkillFrustrumX[i] = int(vFrustrum2[i][0]) + lpObj->X;
		fSkillFrustrumY[i] = int(vFrustrum2[i][1]) + lpObj->Y;
	}

	int tObjNum;
	int count = 0;
	int tmpvar = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0 && gObj[tObjNum].Live)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if(tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if( EnableAttack != FALSE )
				{
					if( SkillTestFrustrum(gObj[tObjNum].X,gObj[tObjNum].Y,aIndex) )
					{
						for(int i = 0; i < 5; i++)
						{
							float Dis = (fSkillFrustrumY[i] - lpObj->Y)*(gObj[tObjNum].Y - lpObj->Y) - (fSkillFrustrumX[i] - lpObj->X)*(gObj[tObjNum].X - lpObj->X);
						
							if( (Dis >= -5 && Dis <= 5) || tObjNum == aTargetIndex )
							{
								gObjAttack(lpObj,&gObj[tObjNum],lpMagic,TRUE,0,0,0,0,0);
							}
						}
					}
				}
			}
		}

		count++;

		if( count > (MAX_VIEWPORT-1) )
		{
			break;
		}
	}
}

void CObjUseSkill::SkillBloodStorm(int aIndex,int aTargetIndex,CMagicInf* lpMagic,int isCombo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int tmpvar = 0;
	int bAttack = 0;
	int tObjNum;
	int count = 0;
	int HitCount = 0;

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,1);

	int EnableAttack;
	int bForceAttack = TRUE;
	int DuelIndex = lpObj->m_iDuelUser;

	while ( true )
	{
		if(lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				EnableAttack = 0;

				if(lpObj->VpPlayer2[count].type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if( CC_MAP_RANGE( lpObj->MapNumber ) || IT_MAP_RANGE( lpObj->MapNumber ) )
				{
					EnableAttack = 1;
				}
				else if( (tObjNum == aTargetIndex && bForceAttack == TRUE ) || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[count].type != OBJ_NPC && gObj[tObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[tObjNum].m_btCsJoinSide )
						EnableAttack = 1;
				}
#endif
				else
				{
					int CallMonIndex = gObj[tObjNum].m_Index;

					if(gObj[tObjNum].Type == OBJ_MONSTER && gObj[tObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[tObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = 1;
					}
				}

				if(EnableAttack != 0)
				{
					if(gObjCalDistance(lpObj,&gObj[tObjNum]) <= 3)
					{
						HitCount++;

						if(HitCount > 5)
						{
							if( rand()%2 )
							{
								bAttack = TRUE;
							}
						}
						else
						{
							bAttack = TRUE;
						}

						if(HitCount > 12)
						{
							bAttack = FALSE;
						}

						if(bAttack == TRUE)
						{
							gObjAddAttackProcMsgSendDelay(lpObj,50,tObjNum,500,lpMagic->m_Skill,isCombo);

							if(isCombo != 0)
							{
								GCMagicAttackNumberSend(lpObj,AT_SKILL_COMBO,gObj[tObjNum].m_Index,1);
							}
						}
					}
				}
			}
		}

		count++;
		if(count > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

BOOL CObjUseSkill::BloodStormExplosionAttack(LPOBJ lpObj, LPOBJ lpTargetObj, int iAttackDamage)
{

	if(gObjIsConnected(lpObj->m_Index) == false && gObjIsConnected(lpTargetObj->m_Index) == false)
	{
		return false;
	}

	if(rand()%10000 > g_SkillAdditionInfo.GetBloodStormExplosionRate())
	{
		return false;
	}

	int iExplosionDamage = iAttackDamage*g_SkillAdditionInfo.GetBloodStormExplosionDamage() / 100;
	int iTarObjNum;

	if(iExplosionDamage < 1)
	{
		return true;
	}

	int iCount = 0;
	int bEnableAttack;

	while( true )
	{
		if(lpObj->VpPlayer2[iCount].state != 0)
		{
			iTarObjNum = lpObj->VpPlayer2[iCount].number;

			if(iTarObjNum >= 0)
			{
				bEnableAttack = 0;

				if(gObj[iTarObjNum].Type == OBJ_MONSTER && gObj[iTarObjNum].m_RecallMon < 0)
				{
					bEnableAttack = 1;
				}
				else if(lpTargetObj->Type == OBJ_USER && lpObj->Type == OBJ_USER && gObj[iTarObjNum].Type != OBJ_NPC)
				{
					bEnableAttack = 1;
				}
				else if(lpTargetObj->m_cChaosCastleIndex == gObj[iTarObjNum].m_cChaosCastleIndex && lpTargetObj->m_cChaosCastleIndex != -1)
				{
					bEnableAttack = 1;
				}
#if (GS_CASTLE == 1)
				else if(lpObj->VpPlayer2[iCount].type != OBJ_NPC && gObj[iTarObjNum].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == 7)
				{
					if( lpObj->m_btCsJoinSide != gObj[iTarObjNum].m_btCsJoinSide )
						bEnableAttack = 1;
				}
#endif
				if(bEnableAttack != 0)
				{
					if(lpObj->m_Index != gObj[iTarObjNum].m_Index)
					{
						int iSkillDistance = gObjCalDistance(lpTargetObj,&gObj[iTarObjNum]);

						if(iSkillDistance <= g_SkillAdditionInfo.GetBloodStormExplosionDistance())
						{
							gObjAddMsgSendDelay(lpObj,54,iTarObjNum,800,iExplosionDamage);
						}
					}
				}
			}
		}

		iCount++;

		if(iCount > MAX_VIEWPORT -1)
		{
			break;
		}
	}
	return true;
}

void CObjUseSkill::SkillMonkBarrageJustOneTarget(int aIndex,CMagicInf * lpMagic,int aTargetIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int nTargetObjNum = 0;
	int nViewPortObjectCount = 0;
	int DuelIndex = lpObj->m_iDuelUser;

	if( !OBJMAX_RANGE(aTargetIndex) )
	{
		LogAddTD("[InvalidTargetIndex][SkillMonkBarrageJustOneTarget] Index :%d , AccountID : %s ",
			aIndex,gObj[aIndex].AccountID);
		return;
	}

	int EnableAttack;
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	while( TRUE )
	{
		if( lpObj->VpPlayer2[nViewPortObjectCount].state != 0 )
		{
			nTargetObjNum = lpObj->VpPlayer2[nViewPortObjectCount].number;

			if( nTargetObjNum >= 0 && 
				nTargetObjNum == aTargetIndex )
			{
				EnableAttack = FALSE;

				
				if(lpObj->VpPlayer2[nViewPortObjectCount].type == OBJ_USER && nTargetObjNum == aTargetIndex)
				{
					EnableAttack = TRUE;
				}
				else if( lpTargetObj->Type == OBJ_MONSTER && gObj[nTargetObjNum].m_RecallMon < 0 )
				{
					EnableAttack = TRUE;
				}
				else if( nTargetObjNum == DuelIndex )
				{
					EnableAttack = TRUE;
				}
				else
				{
					int CallMonIndex = gObj[nTargetObjNum].m_Index;

					if(gObj[nTargetObjNum].Type == OBJ_MONSTER && gObj[nTargetObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[nTargetObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = TRUE;
					}
				}

				if( EnableAttack != FALSE && 
					CalDistance(gObj[aTargetIndex].X,gObj[aTargetIndex].Y,gObj[nTargetObjNum].X,gObj[nTargetObjNum].Y) < 4 )
				{
					int nMaxBarrageCount = 0;

					if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_WEAPON1 )
					{
						nMaxBarrageCount = 5;
					}
					else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_WEAPON2 )
					{
						nMaxBarrageCount = 3;
					}
					else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_CHAINDRIVE )
					{
						nMaxBarrageCount = 5;
					}
					else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DARKSIDE )
					{
						nMaxBarrageCount = 3;
					}
					else
					{
						nMaxBarrageCount = 0;
					}

					if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_WEAPON1 && 
						rand()%100 < 10 )
					{
						gObjAddBuffEffect(lpTargetObj,BUFF_WEAKNESS,ADD_OPTION_WEAKNESS,5,0,0,10);
					}
					else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_WEAPON2 &&
						rand()%100 < 10 )
					{
						gObjAddBuffEffect(lpTargetObj,BUFF_DEFENSEDOWN,ADD_OPTION_DEFENSEDOWN,10,0,0,10);
					}
					else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_CHAINDRIVE &&
						rand()%100 < 40 )
					{
						lpTargetObj->lpAttackObj = lpObj;
						lpTargetObj->DelayActionTime = 1000;
						lpTargetObj->DelayLevel = 1;

						gObjAddBuffEffect(lpTargetObj,BUFF_COLD,ADD_OPTION_REDUCEMOVEMENT,0,0,0,10);
					}

					for(int i = 1; i < nMaxBarrageCount; i++)
					{
						if( (i%(nMaxBarrageCount-1)) == TRUE  )
						{
							gObjAttack(lpObj,lpTargetObj,lpMagic,TRUE,TRUE,0,0,i,0);
						}
						else
						{
							gObjAttack(lpObj,lpTargetObj,lpMagic,FALSE,TRUE,0,0,i,0);
						}
					}
				}
				break;
			}
		}

		nViewPortObjectCount++;

		if(nViewPortObjectCount > MAX_VIEWPORT -1)
		{
			break;
		}
	}
}

int CObjUseSkill::SkillMonkDarkSideGetTargetIndex(int aIndex, int aTargetIndex, CMagicInf * lpMagic, WORD * wTargetList)
{
	LPOBJ lpObj = &gObj[aIndex];
	int nTargetObjNum = 0;
	int nViewPortObjectCount = 0;
	int nHitTargetCount = 0;
	int DuelIndex = lpObj->m_iDuelUser;
	int EnableAttack = 0;

    if( wTargetList == NULL)
        return false;

    if( lpMagic->m_Skill != AT_SKILL_RAGEFIGHTER_DARKSIDE &&
		lpMagic->m_Skill != AT_MSKILL_RF_DARKSIDE1 &&
		lpMagic->m_Skill != AT_MSKILL_RF_DARKSIDE2 )
        return false;

    if( aTargetIndex != 0 )
    {
        while(true)
        {
            if( lpObj->VpPlayer2[nViewPortObjectCount].state != 0 )
            {
				nTargetObjNum = lpObj->VpPlayer2[nViewPortObjectCount].number;

				if( nTargetObjNum >= 0 && nTargetObjNum < OBJMAX )
				{
					LPOBJ lpTargetObj = &gObj[nTargetObjNum];

					if( lpObj->VpPlayer2[nViewPortObjectCount].type == OBJ_USER &&
						nTargetObjNum == aTargetIndex )
					{
						int nAttackRange = 0;

						if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DARKSIDE ||
							lpMagic->m_Skill == AT_MSKILL_RF_DARKSIDE1 ||
							lpMagic->m_Skill == AT_MSKILL_RF_DARKSIDE2 )
							nAttackRange = 5;
						else
							nAttackRange = 0;

						if( CalDistance(lpObj->X,lpObj->Y,gObj[nTargetObjNum].X,gObj[nTargetObjNum].Y) < nAttackRange )
						{
							wTargetList[nHitTargetCount] = nTargetObjNum;
							nHitTargetCount++;
							break;
						}
					}
				}
            }

			nViewPortObjectCount++;

			if( nViewPortObjectCount > MAX_VIEWPORT-1 )
				break;
		}

		nViewPortObjectCount = 0;
	}

	while(true)
	{
		if( lpObj->VpPlayer2[nViewPortObjectCount].state != 0 )
		{
			nTargetObjNum = lpObj->VpPlayer2[nViewPortObjectCount].number;

			if( nTargetObjNum >= 0 && nTargetObjNum < OBJMAX )
			{
				EnableAttack = FALSE;

				LPOBJ lpTargetObj = &gObj[nTargetObjNum];

				if( lpObj->VpPlayer2[nViewPortObjectCount].type == OBJ_USER &&
						nTargetObjNum == aTargetIndex )
					// here we need to add gens battle zone check )
				{
					EnableAttack = TRUE;
				}
				else if( lpTargetObj->Type == OBJ_MONSTER && lpTargetObj->m_RecallMon < 0 )
				{
					EnableAttack = TRUE;
				}
				else if( DuelIndex == nTargetObjNum )
				{
					EnableAttack = TRUE;
				}
				else
				{
					int CallMonIndex = gObj[nTargetObjNum].m_Index;

					if(gObj[nTargetObjNum].Type == OBJ_MONSTER && gObj[nTargetObjNum].m_RecallMon >= 0)
					{
						CallMonIndex = gObj[gObj[nTargetObjNum].m_RecallMon].m_Index;
					}

					if(gObjTargetGuildWarCheck(lpObj,&gObj[CallMonIndex])==1)
					{
						EnableAttack = TRUE;
					}
				}

				if( EnableAttack != FALSE && !gObjAttackQ(lpTargetObj) )
					EnableAttack = FALSE;

				if( EnableAttack != FALSE )
				{
					int nAttackRange = 0;

					if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DARKSIDE ||
						lpMagic->m_Skill == AT_MSKILL_RF_DARKSIDE1 ||
						lpMagic->m_Skill == AT_MSKILL_RF_DARKSIDE2 )
						nAttackRange = 5;
					else
						nAttackRange = 0;

					if( CalDistance(lpObj->X,lpObj->Y,gObj[nTargetObjNum].X,gObj[nTargetObjNum].Y) < nAttackRange )
					{
						wTargetList[nHitTargetCount] = nTargetObjNum;
						nHitTargetCount++;

						if( nHitTargetCount >= 5 )
							break;
					}
				}

			}
		}
		nViewPortObjectCount++;

		if( nViewPortObjectCount > MAX_VIEWPORT-1 )
			break;
	}
    return true;
}

int CObjUseSkill::SkillMonkBuff(int aIndex,CMagicInf * lpMagic)
{
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER )
		return false;

	if( lpObj->Class != CLASS_FIGHTER )
		return false;

	if( lpMagic == NULL )
		return false;

	int nEffectValue = 0;

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}

	if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_REDARMOR )
	{
		nEffectValue = lpInfo->m_Arg1 + (lpObj->Energy+lpObj->AddEnergy-lpInfo->m_Arg2)/100.0f;

		if( nEffectValue > lpInfo->m_Arg3 )
			nEffectValue = lpInfo->m_Arg3;

		int Duration = lpInfo->m_Arg4+lpObj->Energy/lpInfo->m_Arg5;

		if( Duration > lpInfo->m_Arg6 )
		{
			Duration = lpInfo->m_Arg6;
		}

		gObjAddBuffEffect(lpObj,BUFF_IGNORE_DEFENSE,ADD_OPTION_DEFENSEIGNORE,nEffectValue,0,0, Duration);
		GCMagicAttackNumberSend(lpObj,AT_SKILL_RAGEFIGHTER_REDARMOR,lpObj->m_Index,TRUE);
	}
	else
	{
		return false;
	}

	return true;
}

int CObjUseSkill::SkillMonkBuffApplyParty(int aIndex,CMagicInf * lpMagic)
{
	int skillSuccess = TRUE;
	LPOBJ lpObj = &gObj[aIndex];
	int number;

	if( lpObj->Type != OBJ_USER && lpObj->m_RecallMon == -1 )
		return FALSE;

	if( lpObj->Class != CLASS_FIGHTER )
		return FALSE;

	int partynum = 0;
	int partycount;
	int dis;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	int nEffectValue;
	memset(ApplyPartyIndex,-1,sizeof(ApplyPartyIndex));
	LPOBJ lpPartyObj;

	partynum = lpObj->PartyNumber;

	if( partynum != -1 )
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if( number >= 0 )
			{
				lpPartyObj = &gObj[number];


				if( lpPartyObj->MapNumber == lpObj->MapNumber )
				{
					dis = gObjCalDistance(lpObj,&gObj[number]);
					if( dis < 4 )
					{
						ApplyPartyIndex[n] = lpPartyObj->m_Index;
					}
				}
			}
		}
	}

	BuffSkillEffectInfo* lpInfo = g_BuffSkillEffect.GetEffect(lpMagic->m_Skill);

	if( !lpInfo )
	{
		return 0;
	}
	
	int Duration;

	if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_FITNESS )
	{
		nEffectValue = lpInfo->m_Arg1 + (lpObj->Energy+lpObj->AddEnergy-lpInfo->m_Arg2)/10.0;

		if( nEffectValue > lpInfo->m_Arg3 )
			nEffectValue = lpInfo->m_Arg3;

		Duration = lpInfo->m_Arg4+lpObj->Energy/lpInfo->m_Arg5;

		if( Duration > lpInfo->m_Arg6 )
		{
			Duration = lpInfo->m_Arg6;
		}

	}
	else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DEFRATE )
	{
		nEffectValue = lpInfo->m_Arg1 + (lpObj->Energy+lpObj->AddEnergy-lpInfo->m_Arg2)/10.0;

		if( nEffectValue > lpInfo->m_Arg3 )
			nEffectValue = lpInfo->m_Arg3;

		Duration = lpInfo->m_Arg4+lpObj->Energy/lpInfo->m_Arg5;

		if( Duration > lpInfo->m_Arg6 )
		{
			Duration = lpInfo->m_Arg6;
		}
	}
	else
	{
		return FALSE;
	}

	if( partynum == -1 )
	{
		if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_FITNESS )
		{
			gObjAddBuffEffect(lpObj,BUFF_INCREASE_VIT,ADD_OPTION_INCRVITALITY,nEffectValue,0,0,Duration);
			GCMagicAttackNumberSend(lpObj,AT_SKILL_RAGEFIGHTER_FITNESS,lpObj->m_Index,TRUE);
			gObjCalCharacter(lpObj->m_Index);
		}
		else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DEFRATE )
		{
			gObjAddBuffEffect(lpObj,BUFF_INCREASE_DEFRATE,ADD_OPTION_DEFRATE,nEffectValue,0,0,Duration);
			GCMagicAttackNumberSend(lpObj,AT_SKILL_RAGEFIGHTER_DEFRATE,lpObj->m_Index,TRUE);
		}
	}
	else
	{
		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if( ApplyPartyIndex[n] != -1 )
			{
				lpPartyObj = &gObj[ApplyPartyIndex[n]];
				if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_FITNESS )
				{
					gObjAddBuffEffect(lpPartyObj,BUFF_INCREASE_VIT,ADD_OPTION_INCRVITALITY,nEffectValue,0,0,Duration);
					GCMagicAttackNumberSend(lpPartyObj,AT_SKILL_RAGEFIGHTER_FITNESS,lpPartyObj->m_Index,TRUE);
					gObjCalCharacter(lpPartyObj->m_Index);
				}
				else if( lpMagic->m_Skill == AT_SKILL_RAGEFIGHTER_DEFRATE )
				{
					gObjAddBuffEffect(lpPartyObj,BUFF_INCREASE_DEFRATE,ADD_OPTION_DEFRATE,nEffectValue,0,0,Duration);
					GCMagicAttackNumberSend(lpPartyObj,AT_SKILL_RAGEFIGHTER_DEFRATE,lpPartyObj->m_Index,TRUE);
				}
			}
		}
	}
	return TRUE;
}

int CObjUseSkill::SkillCharge(int aIndex,CMagicInf * lpMagic,int aTargetIndex)
{
	if( !OBJMAX_RANGE(aIndex) )
		return false;

	LPOBJ lpObj = &gObj[aIndex];

	GCMagicAttackNumberSend(lpObj,lpMagic->m_Skill,aTargetIndex,TRUE);
	gObjAttack(lpObj,&gObj[aTargetIndex],lpMagic,FALSE,TRUE,0,0,0,0);

return true;

}

int CObjUseSkill::SkillDeathPoisonForMeDusa(int aIndex, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex) //Added Leee
{
	LPOBJ lpObj		= &gObj[aIndex];
	int tObjNum;
	int count		= 0;
	int FirstHit	= 0;
	int HitCount	= 0;
	int DuelIndex	= lpObj->m_iDuelUser;
	int EnableAttack;
	// ----
	if( !OBJMAX_RANGE(aTargetIndex) )
	{
		LogAddTD("[InvalidTargetIndex][SkillDeathPoisonForMeDusa] Index :%d , AccountID : %s ",
			aIndex, gObj[aIndex].AccountID);
		return 0;
	}
	// ----
	LPOBJ lpTargetObj = &gObj[aTargetIndex];
	// ----
	while(true)
	{
		if( lpObj->VpPlayer2[count].state != 0 )
		{
			tObjNum = lpObj->VpPlayer2[count].number;
			// ----
			if( tObjNum >= 0 )
			{
				EnableAttack = 0;
				// ----
				if( lpObj->VpPlayer2[count].type == OBJ_USER && gObj[tObjNum].m_RecallMon < 0 )
				{
					EnableAttack = 1;
				}
				// ----
				if( EnableAttack )
				{
					if( this->CalDistance(x, y, gObj[tObjNum].X, gObj[tObjNum].Y) < 8 )
					{
						if( lpObj->m_Index != gObj[tObjNum].m_Index )
						{
							int iSkillDistance = gObjCalDistance(lpTargetObj, &gObj[tObjNum]);
							// ----
							if( iSkillDistance <= 3 )
							{
								int delay = rand() % 500;
								gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, delay, lpMagic->m_Skill, 0);
								// ----
								if( gObjCheckUsedBuffEffect(&gObj[tObjNum], 55) )
								{
									if( !retResistance(&gObj[tObjNum], 1) )
									{
										gObj[tObjNum].lpAttackObj = lpObj;
										gObjAddBuffEffect(&gObj[tObjNum], 55, 19, 3, 0, 0, 20);
									}
								}
							}
						}
					}
				}
			}
		}
		// ----
		count++;
		// ----
		if( count > MAX_VIEWPORT -1 )
		{
			break;
		}
	}
	// ----
	return 1;
}

int CObjUseSkill::SkillEvilMonster(int aIndex, int aTargetIndex, CMagicInf * lpMagic)
{
	LPOBJ lpObj		= &gObj[aIndex];
	int tObjNum;
	int count		= 0;
	int HitCount	= 0;
	int bAttack;
	int DuelIndex	= lpObj->m_iDuelUser;
	// ----
	while(true)
	{
		if( lpObj->VpPlayer2[count].state )
		{
			tObjNum = lpObj->VpPlayer2[count].number;
			// ----
			if( tObjNum >= 0 )
			{
				int EnableAttack = false;
				// ----
				if( lpObj->VpPlayer2[count].type == 1 )
				{
					EnableAttack = true;
					// ----
					if( gObj[tObjNum].m_RecallMon >= 0 )
					{
						if( tObjNum != aTargetIndex )
						{
							EnableAttack = false;
						}
					}
				}
				// ----
				if( EnableAttack )
				{
					if( gObjCalDistance(lpObj, &gObj[tObjNum]) < 6 )
					{
						bAttack = false;
						// ----
						if( HitCount >= 5 )
						{
							bAttack = false;
						}
						else
						{
							bAttack = true;
						}
						// ----
						if( tObjNum == aTargetIndex )
						{
							bAttack = true;
						}
						// ----
						if( bAttack )
						{
							int delay = rand() % 2000;
							// ----
							gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 300, lpMagic->m_Skill, 0);
							HitCount++;
							// ----
							LPOBJ lpTargetObj = &gObj[tObjNum];
							// ----
							if( !gObjCheckUsedBuffEffect(lpTargetObj, 55) )
							{
								if( !retResistance(lpTargetObj, 1) )
								{
									lpTargetObj->lpAttackObj = lpObj;
									gObjAddBuffEffect(lpTargetObj, 55, 19, 3, 0, 0, 20);
								}
							}
						}
					}
				}
			}
		}
		// ----
		count++;
		// ----
		if( count > MAX_VIEWPORT-1 )
		{
			break;
		}
	}
	// ----
	return true;
}