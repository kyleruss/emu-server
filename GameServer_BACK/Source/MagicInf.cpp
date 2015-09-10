#include "stdafx.h"
#include "MagicInf.h"
#include "LogProc.h"
#include "GameMain.h"
#include "MasterLevelSkillTreeSystem.h"
// GS-N 0.99.60T 0x0046DE00
//	GS-N	1.00.18	JPN	0x00484870	-	Completed

CMagicInf DefMagicInf[MAX_SKILL-1];




CMagicInf::CMagicInf()
{
	this->Clear();
}




CMagicInf::~CMagicInf()
{
	m_DamageMin  = 0;
	m_DamageMax  = 0;
}




void CMagicInf::Clear()
{
	m_Skill  = 0xFF;
	m_Level  = 0;
	m_bBrand = 0;
}




BOOL CMagicInf::IsMagic()
{
	if ( m_Skill == 0xFF )
	{
		return false;
	}
	return true;
}




int GetSkillNumberInex(int type, int Index, int level)
{
	int skillnumber = -1;

	if ( type == 15 )
	{
		if ( Index >= 0 && Index < 16 )
		{
			skillnumber = Index + 1;
		}
		else if( Index == 16 )
		{
			skillnumber = AT_SKILL_EXPPOISON;
		}
		else if( Index == 17 )
		{
			skillnumber = AT_SKILL_EXPICE;
		}
		else if( Index == 18 )
		{
			skillnumber = AT_SKILL_EXPHELL;
		}
		else if( Index == 19)
		{
			skillnumber = AT_SKILL_CHAINLIGHTING;
		}
		else if( Index == 20)
		{
			skillnumber = AT_SKILL_DRAINLIFE;
		}
		else if( Index == 21)
		{
			skillnumber = AT_SKILL_LIGHTSHOCK;
		}
		else if( Index == 22)
		{
			skillnumber = AT_SKILL_DAMAGEREFLECT;
		}
		else if( Index == 23)
		{
			skillnumber = AT_SKILL_BERSERKER;
		}
		else if( Index == 24)
		{
			skillnumber = AT_SKILL_SLEEP;
		}
		else if( Index == 25)
		{
			skillnumber = 0xDC;
		}
		else if( Index == 26)
		{
			skillnumber = AT_SKILL_WEAKNESS;
		}
		else if( Index == 27)
		{
			skillnumber = AT_SKILL_INNOVATION;
		}
		else if( Index == 28 )
		{
			skillnumber = AT_SKILL_EXPWIZARDRY;
		}
		else if( Index == 29 )
		{
			skillnumber = AT_SKILL_GIGANTICSTORM;
		}
		else if( Index == 30 )
		{
			skillnumber = AT_SKILL_RAGEFIGHTER_CHAINDRIVE;
		}
		else if( Index == 31 )
		{
			skillnumber = AT_SKILL_RAGEFIGHTER_DARKSIDE;
		}
		else if( Index == 32 )
		{
			skillnumber = AT_SKILL_RAGEFIGHTER_DRAGONLORE;
		}
		else if( Index == 33 )
		{
			skillnumber = AT_SKILL_RAGEFIGHTER_DRAGONSLAYER;
		}
		else if( Index == 34 )
		{
			skillnumber = AT_SKILL_RAGEFIGHTER_REDARMOR;
		}
		else if( Index == 35 )
		{
			skillnumber = AT_SKILL_RAGEFIGHTER_FITNESS;
		}
		else if( Index == 36 )
		{
			skillnumber = AT_SKILL_RAGEFIGHTER_DEFRATE;
		}
	}

	else if ( type == 12 )
	{
		switch ( Index )
		{
			case 7:		skillnumber = AT_SKILL_WHEEL;	break;
			case 8:		skillnumber = AT_SKILL_HEALING;	break;
			case 9:		skillnumber = AT_SKILL_DEFENSE;	break;
			case 10:	skillnumber = AT_SKILL_ATTACK;	break;
			case 12:	skillnumber = AT_SKILL_BLOWOFFURY;	break;
			case 13:	skillnumber = AT_SKILL_KNIGHTSPEAR;	break;
			case 14:	skillnumber = AT_SKILL_KNIGHTADDLIFE;	break;
			case 19:	skillnumber = AT_SKILL_STRIKE;	break;
			case 17:	skillnumber = AT_SKILL_PENETRATION;	break;
			case 18:	skillnumber = AT_SKILL_ELFHARDEN;	break;
			case 16:	skillnumber = AT_SKILL_DEFENSEDOWN;	break;
			case 21:	skillnumber = AT_SKILL_FIREBURST;	break;
			case 22:	skillnumber = AT_SKILL_RECALL_PARTY;	break;
			case 23:	skillnumber = AT_SKILL_ADD_CRITICALDAMAGE;	break;
			case 24:	skillnumber = AT_SKILL_ELECTRICSPARK;	break;
			case 11:	skillnumber = level + AT_SKILL_CALLMON1;	break;
			case 35:	skillnumber = AT_SKILL_FIRESCREAM;	break;
			case 44:	skillnumber = AT_SKILL_DESTRUCT; break;
			case 45:	skillnumber = AT_SKILL_MULTISHOT;	break;
			case 46:	skillnumber = AT_SKILL_RECOVERY;	break;
			case 47:	skillnumber = AT_SKILL_FLAMESTRIKE;	break;
			case 48:	skillnumber = AT_SKILL_CHAOTICDISEIER; break;
		}
	}
		
	return skillnumber;
}





int CMagicInf::Set(BYTE aType, BYTE aIndex, BYTE aLevel)
{
	if ( m_Skill != 0xFF )
	{
		LogAdd(lMsg.Get(MSGGET(1, 203)), __FILE__, __LINE__);
		return -1;
	}

	m_Level = aLevel;
	m_Skill = GetSkillNumberInex(aType, aIndex, aLevel);
	
	if ( m_Skill < 0 )
	{
		m_Skill = 1;
	}

	int damage = MagicDamageC.SkillGet(m_Skill);
	m_DamageMin = damage;
	m_DamageMax = damage + damage/2;

	return m_Skill;
}





int CMagicInf::GetDamage()
{
	int damage = 0;

	if ( m_Skill == 0xFF )
	{
		LogAdd(lMsg.Get(MSGGET(1, 202)), __FILE__, __LINE__);
		return 0;
	}

	int subd = m_DamageMax - m_DamageMin;

	__try
	{
		damage = m_DamageMin + (rand() % 1);
	}
	__except(subd=1,1)
	{
		LogAdd(lMsg.Get(MSGGET(0, 111)), __FILE__, __LINE__);
	}

	return damage;
}

int CMagicInf::SetMasterSkill(int aSkill,BYTE aLevel)
{
	if( m_Skill != 0xFF )
	{
		m_Skill = aSkill;
		m_Level = aLevel;

		int damage = MagicDamageC.SkillGet(m_Skill);
		m_DamageMin = damage;
		m_DamageMax = damage + damage/2;
		return m_Skill;
	}
	else
	{
		LogAdd("[MasterSkill] [ERROR!!] Fail - Update Master Skill:%d, Level:%d",aSkill,aLevel);
		return -1;
	}
}



int CMagicInf::Set(int aSkill, BYTE aLevel)
{
	int damage = 1;

	if ( m_Skill != 0xFF )
	{
		return -1;
	}

	m_Skill = aSkill;
	m_Level = aLevel;
	
	if( g_MasterSkillSystem.CheckMasterLevelSkill(aSkill) )
	{
		if( MagicDamageC.GetSkillUseType(aSkill) == 4 )
		{
			int nBrandSkill = MagicDamageC.GetNormalBrandOfSkill(aSkill);

			if( nBrandSkill > 0 && aSkill != nBrandSkill )
			{
				damage = MagicDamageC.SkillGet(nBrandSkill);
			}
			else
			{
				float fValue = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(this->m_Skill),this->m_Level);

				if( fValue == 0.0f )
				{
					damage = 1;
				}
				else
				{
					damage = fValue;
				}
			}
		}
		else
		{
			float fValue = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(this->m_Skill),this->m_Level);

			if( fValue == 0.0f )
			{
				damage = 1;
			}
			else
			{
				damage = fValue;
			}
		}
	}
	else
	{
		damage = MagicDamageC.SkillGet(m_Skill);
	}

	
	m_DamageMin = damage;
	m_DamageMax = damage + damage/2;

	return m_Skill;
}

int CMagicInf::UpdateMasterSkill(int iSkill,unsigned char btLevel)
{
	int damage = 1;

	if( this->m_Skill != (BYTE)-1 )
	{
		this->m_Skill = iSkill;
		this->m_Level = btLevel;
		if( MagicDamageC.GetSkillUseType(this->m_Skill) == 4 )
		{
			int nBrandSkill = MagicDamageC.GetNormalBrandOfSkill(this->m_Skill);

			if( nBrandSkill > 0 && this->m_Skill != nBrandSkill )
			{
				damage = MagicDamageC.SkillGet(nBrandSkill);
			}
			else
			{
				float fValue = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(this->m_Skill),this->m_Level);

				if( fValue == 0.0f )
				{
					damage = 1;
				}
				else
				{
					damage = fValue;
				}
			}
		}
		else
		{
			float fValue = g_MasterSkillSystem.GetMasterSkillValue(MagicDamageC.SkillGet(this->m_Skill),this->m_Level);

			if( fValue == 0.0f )
			{
				damage = 1;
			}
			else
			{
				damage = fValue;
			}
		}

		this->m_DamageMin = damage;
		this->m_DamageMax = damage + damage/2;
		return this->m_Skill;
	}
	else
	{
		LogAdd("[MasterSkill] [ERROR!!] Fail - Update Master Skill:%d, Level:%d",iSkill,btLevel);
		return -1;
	}
}


void MagicByteConvert(unsigned char* buf, CMagicInf* const Magici, int maxmagic)
{
	int n=0;
	int level;
	int skill;
	int subtype;

	for (int index = 0;index < maxmagic;index++)
	{
		skill = Magici[index].m_Skill;
		level = 0;
		subtype = 0;

		if( Magici[index].m_Skill > 765 )
		{
			skill = 0xFF;
			level = 3;
			subtype = Magici[index].m_Skill - skill*3;
		}
		else if( Magici[index].m_Skill > 510 )
		{
			skill = 0xFF;
			level = 2;
			subtype = Magici[index].m_Skill - skill*2;
		}
		else if( Magici[index].m_Skill > 255 )
		{
			skill = 0xFF;
			level = 1;
			subtype = Magici[index].m_Skill - skill;
		}

		buf[n] = skill;
		n++;
		buf[n] = Magici[index].m_Level << 3;
		buf[n] |= level & 0x07;
		n++;
		buf[n] = subtype;
		n++;
	}
}
