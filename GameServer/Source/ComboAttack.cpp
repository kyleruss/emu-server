#include "stdafx.h"
#include "ComboAttack.h"
#include "user.h"
#include "MasterSkillSystem.h"
// GS-N 0.99.60T 0x004A42C0 : Completed
//	GS-N	1.00.18	JPN	0x004C2270	-	Completed

CComboAttack gComboAttack;


CComboAttack::CComboAttack()
{
	return;
}

CComboAttack::~CComboAttack()
{
	return;
}

int CComboAttack::GetSkillPos(int skillnum)
{
	switch (skillnum)
	{
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			return 0;
		case 41:
		case 42:
		case 43:
		case AT_SKILL_DESTRUCT:
			return 1;
		case AT_MSKILL_DK_CYCLONE1:
		case AT_MSKILL_DK_SLASH1:
		case AT_MSKILL_DK_FALLINGSLASH1:
		case AT_MSKILL_DK_LUNGE1:
			return 0;
		case AT_MSKILL_DK_TWISTINGSLASH1:
		case AT_MSKILL_DK_TWISTINGSLASH2:
		case AT_MSKILL_DK_RAGEFULBLOW1:
		case AT_MSKILL_DK_RAGEFULBLOW2:
		case AT_MSKILL_DK_DEATHSTAB1:
		case AT_MSKILL_DK_DEATHSTAB2:
		case AT_MSKILL_DK_DEATHSTAB3:
		case AT_MSKILL_DK_DESTRUCTION1:
		case AT_MSKILL_DK_DESTRUCTION2:
		case AT_MSKILL_DK_DESTRUCTION3:
		case AT_MSKILL_DK_BLOODSTORM:
		case AT_MSKILL_DK_BLOODSTORM1:
			return 1;
	}
return -1;
}


BOOL CComboAttack::CheckCombo(int aIndex, int skillnum)
{
	LPOBJ lpObj = &gObj[aIndex];
	
	if ( lpObj->ComboSkillquestClear  != false )
	{
		int sp = this->GetSkillPos(skillnum);

		if ( sp == 0 )
		{
			lpObj->comboSkill.ProgressIndex  = 0;
			lpObj->comboSkill.dwTime = GetTickCount() + 3000;
			lpObj->comboSkill.Skill[0]  = skillnum;
		}
		else if ( sp == 1 )
		{
			if ( lpObj->comboSkill.Skill[0]  == 0xFF )
			{
				lpObj->comboSkill.Init();
				return 0;
			}

			int Time =GetTickCount();

			if (lpObj->comboSkill.dwTime < GetTickCount() )
			{
				lpObj->comboSkill.Init();
				return 0;
			}

			if ( lpObj->comboSkill.ProgressIndex == 0 )
			{
				lpObj->comboSkill.ProgressIndex = 1;
				lpObj->comboSkill.dwTime = GetTickCount() + 3000;
				lpObj->comboSkill.Skill[1] = skillnum;
			}
			else if ( lpObj->comboSkill.Skill[1]  != skillnum )
			{
				lpObj->comboSkill.Init();
				return 1;
			}
			else
			{
				lpObj->comboSkill.Init();
			}
		}
		else
		{
			lpObj->comboSkill.ProgressIndex= -1;
			lpObj->comboSkill.dwTime = 0;
			lpObj->comboSkill.Skill[0] = -1;
		}
	}

	return 0;
}
			


