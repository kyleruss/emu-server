#include "stdafx.h"
#include "MagicDamage.h"
#include "..\include\readscript.h"
#include "..\common\WzMemScript.h"
#include "LogProc.h"
#include "GameMain.h"
//#include "MasterSkillSystem.h"
#include "MasterLevelSkillTreeSystem.h"

CMagicDamage MagicDamageC;

CMagicDamage::CMagicDamage()
{
	return;
}


CMagicDamage::~CMagicDamage()
{
	return;
}

void CMagicDamage::Init()
{
	for ( int n=0; n< MAX_SKILL;n++)
	{
		this->m_Damage[n] = -1;
	}
}


void CMagicDamage::LogSkillList(char * filename)
{
	this->Init();


	SMDFile = fopen(filename, "r");

	if ( SMDFile == NULL )
	{
		MsgBox(lMsg.Get(MSGGET(1, 199)), filename);
		return;
	}

	int Token;
	int n=0;
	int number;
	char name[50];
	int requireLevel;
	int damage;
	int mana;
	int bp=0;
	int dis;
	int rEnergy;
	int iSkillType;
	int rLeadership;
	int rDelay;
	int attr;
	int iSkillUseType;
	int iSkillBrand;
	int iKillCount;
	int RequireStatus[MAX_REQ_SKILL_STATUS] = {0};
	BYTE RequireClass[MAX_TYPE_PLAYER];
	short nSkillRank;
	short nSkillGroup;
	short nRequireMLPoint;
	short nHP;
	short nSD;
	WORD wKeepTime;
	short nRequireStr;
	short nRequireDex;
	BYTE btIconNum;
	int nSkillType;
	BYTE btItemSkill;
	BYTE btIsDamage;

	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}

		if ( Token == 1 )
		{
			number = (int)TokenNumber;

			Token = GetToken();
			strcpy_s(name, sizeof(name), TokenString);

			Token = GetToken();
			requireLevel = (int)TokenNumber;

			Token = GetToken();
			damage = (int)TokenNumber;

			Token = GetToken();
			mana = (int)TokenNumber;

			Token = GetToken();
			bp = (int)TokenNumber;	// Agility Gauge

			Token = GetToken();
			dis = (int)TokenNumber;

			Token = GetToken();
			rDelay = (int)TokenNumber;

			Token = GetToken();
			rEnergy = (int)TokenNumber;

			Token = GetToken();
			rLeadership = (int)TokenNumber;

			Token = GetToken();
			attr = (int)TokenNumber;

			Token = GetToken();
			iSkillType = (int)TokenNumber;

			Token = GetToken();
			iSkillUseType = (int)TokenNumber;

			Token = GetToken();
			iSkillBrand = (int)TokenNumber;

			Token = GetToken();
			iKillCount = (int)TokenNumber;
			/*
			CLASS_WIZARD = 0x0,
  CLASS_KNIGHT = 0x1,
  CLASS_ELF = 0x2,
  CLASS_MAGUMSA = 0x3,
  CLASS_DARKLORD = 0x4,
  CLASS_SUMMONER = 0x5,
  CLASS_FIGHTER = 0x6,*/
			Token = GetToken();
			RequireStatus[0] = (int)TokenNumber;

			Token = GetToken();
			RequireStatus[1] = (int)TokenNumber;

			Token = GetToken();
			RequireStatus[2] = (int)TokenNumber;

			Token = GetToken();
			RequireClass[0] = (BYTE)TokenNumber;

			Token = GetToken();
			RequireClass[1] = (BYTE)TokenNumber;

			Token = GetToken();
			RequireClass[2] = (BYTE)TokenNumber;

			Token = GetToken();
			RequireClass[3] = (BYTE)TokenNumber;

			Token = GetToken();
			RequireClass[4] = (BYTE)TokenNumber;

			Token = GetToken();
			RequireClass[5] = (BYTE)TokenNumber;

			Token = GetToken();
			RequireClass[6] = (BYTE)TokenNumber;

			Token = GetToken();
			nSkillRank = (WORD)TokenNumber;

			Token = GetToken();
			nSkillGroup = (WORD)TokenNumber;

			Token = GetToken();
			nRequireMLPoint = (WORD)TokenNumber;

			Token = GetToken();
			nHP = (WORD)TokenNumber;

			Token = GetToken();
			nSD = (WORD)TokenNumber;

			Token = GetToken();
			wKeepTime = (WORD)TokenNumber;

			Token = GetToken();
			nRequireStr = (WORD)TokenNumber;

			Token = GetToken();
			nRequireDex = (WORD)TokenNumber;

			Token = GetToken();
			btIconNum = (BYTE)TokenNumber;

			Token = GetToken();
			nSkillType = TokenNumber;

			Token = GetToken();
			btItemSkill = (BYTE)TokenNumber;

			Token = GetToken();
			btIsDamage = (BYTE)TokenNumber;

			this->Set(name, number,damage, requireLevel, mana, bp, dis, attr, iSkillType, RequireClass, rEnergy, rLeadership, rDelay, btItemSkill, btIsDamage );
			this->SetEx(number, iSkillUseType, iSkillBrand, iKillCount, RequireStatus);
			this->SetMasterLevelSkillInfo(number,nSkillRank,nSkillGroup,nRequireMLPoint,nHP,nSD,wKeepTime,nRequireStr,nRequireDex);
			DefMagicInf[number].Set(number, 0);
		}
	}

	fclose(SMDFile);
	LogAdd(lMsg.Get(MSGGET(1, 200)), filename);
}


void CMagicDamage::LogSkillNameList(char* filename)
{
	int Token;
	int n;
	int number;
	char name[50];
	
	SMDFile = fopen(filename, "r");

	if ( SMDFile == NULL )
	{
		MsgBox(lMsg.Get(MSGGET(1, 199)), filename);
		return;
	}

	n=0;

	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}

		if ( Token == 1 )
		{
			number = (int)TokenNumber;

			Token = GetToken();
			strcpy_s(name, sizeof(name), TokenString);
			strcpy_s(this->m_Name[number], sizeof(this->m_Name[number]), name);
		}
	}

	fclose(SMDFile);
	LogAdd(lMsg.Get(MSGGET(1, 200)), filename);
}

void CMagicDamage::LogSkillList(char* Buffer, int iSize)
{
	/*
	this->Init();
	CWzMemScript WzMemScript;

	WzMemScript.SetBuffer(Buffer, iSize);

	int Token;
	int n=0;
	int number;
	char name[50];
	int requireLevel;
	int damage;
	int mana;
	int bp=0;
	int dis;
	int rEnergy;
	int iSkillType;
	int rLeadership;
	int rDelay;
	int attr;
	int iSkillUseType;
	int iSkillBrand;
	int iKillCount;
	int RequireStatus[MAX_REQ_SKILL_STATUS] = {0};
	BYTE RequireClass[MAX_TYPE_PLAYER];

	short m_Unk1;
	short m_Unk2;
	short m_Unk3;
	short m_Unk4;
	short m_Unk5;
	WORD m_Unk6;
	short m_Unk7;
	short m_Unk8;
	BYTE m_Unk9;
	int m_Unk10;

	while ( true )
	{
		Token = WzMemScript.GetToken();

		if ( Token == 2 )
		{
			break;
		}

		if ( Token == 1 )
		{
			number = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			strcpy(name, WzMemScript.GetString() );

			Token = WzMemScript.GetToken();
			requireLevel = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			damage = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			mana = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			bp = WzMemScript.GetNumber();	// Agility Gauge

			Token = WzMemScript.GetToken();
			dis = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			rDelay = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			rEnergy = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			rLeadership = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			attr = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			iSkillType = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			iSkillUseType = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			iSkillBrand = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			iKillCount = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			RequireStatus[0] = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			RequireStatus[1] = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			RequireStatus[2] = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			RequireClass[0] = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			RequireClass[1] = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			RequireClass[2] = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			RequireClass[3] = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			RequireClass[4] = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk1 = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk2 = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk3 = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk4 = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk5 = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk6 = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk7 = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk8 = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk9 = WzMemScript.GetNumber();

			Token = WzMemScript.GetToken();
			m_Unk10 = WzMemScript.GetNumber();


			this->Set(name, number,damage, requireLevel, mana, bp, dis, attr, iSkillType, RequireClass, rEnergy, rLeadership, rDelay);
			this->SetEx(number, iSkillUseType, iSkillBrand, iKillCount, RequireStatus);
			DefMagicInf[number].Set(number, 0);
		}
	}

	LogAdd(lMsg.Get(MSGGET(1, 200)), "Skill");*/
}





void CMagicDamage::LogSkillNameList(char* Buffer, int iSize)
{
	CWzMemScript WzMemScript;
	int Token;
	int n;
	int number;
	char name[50];
	
	WzMemScript.SetBuffer(Buffer, iSize);

	n=0;

	while ( true )
	{
		Token = WzMemScript.GetToken();

		if ( Token == 2 )
		{
			break;
		}

		if ( Token == 1 )
		{
			number = (int)TokenNumber;

			Token = WzMemScript.GetToken();	
			strcpy_s(name, sizeof(name), TokenString);

			strcpy_s(this->m_Name[number], sizeof(this->m_Name[number]), name);
		}
	}

	
	LogAdd(lMsg.Get(MSGGET(1, 200)), "Skill_Local");
}

void CMagicDamage::SetMasterLevelSkillInfo(int iSkill,int iSkillRank,int iSkillGroup,int iRequireMLPoint,int iHP,int iSD,unsigned short wKeepTime,int iRequireStr,int iRequireDex)
{
	if ( iSkill <0 || iSkill > MAX_SKILL -1 )
	{
		MsgBox(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return;
	}

	if ( this->m_Damage[iSkill] == -1 )
	{
		MsgBox(lMsg.Get(MSGGET(2, 50)), __FILE__, __LINE__);
		return;
	}

	m_wML_Rank[iSkill] = iSkillRank;
	m_wML_SkillGroup[iSkill] = iSkillGroup;
	m_wML_RequirePoint[iSkill]= iRequireMLPoint;
	m_wML_HP[iSkill] = iHP;
	m_wML_SD[iSkill] = iSD;
	m_wML_KeepTime[iSkill] = wKeepTime;
	m_wML_RequireStr[iSkill] = iRequireStr;
	m_wML_RequireDex[iSkill] = iRequireDex;
}

void CMagicDamage::SetEx(int iSkill, int iSkillUseType, int iSkillBrand, int iKillCount, int * pReqStatus)
{
	if ( iSkill <0 || iSkill > MAX_SKILL -1 )
	{
		MsgBox(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return;
	}

	if ( this->m_Damage[iSkill] == -1 )
	{
		MsgBox(lMsg.Get(MSGGET(2, 50)), __FILE__, __LINE__);
		return;
	}

	this->m_iSkillUseType[iSkill] = iSkillUseType;
	this->m_wSkillBrand[iSkill] = iSkillBrand;
	this->m_iKillCount[iSkill] = iKillCount;
	memcpy(this->m_iRequireStatus[iSkill], pReqStatus, sizeof(this->m_iRequireStatus[0]));
}

void CMagicDamage::Set(char* name, int skill, int damage,  int rlevel,  int mana,  int bp,  int dis, int Attr,  int iSkillType, unsigned char* ReqClass, int rEnergy, int rLeadership, int rdelay, BYTE byItemSkill, BYTE byIsDamage)
{
	if ( skill <0 || skill > MAX_SKILL -1 )
	{
		MsgBox(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return;
	}

	if ( this->m_Damage[skill] != -1 )
	{
		MsgBox(lMsg.Get(MSGGET(2, 50)), __FILE__, __LINE__);
		return;
	}

	strcpy_s(this->m_Name[skill], sizeof(this->m_Name[skill]), name);
	this->m_Damage[skill] = damage;
	this->m_rLevel[skill] = rlevel;
	this->m_Mana[skill] = mana;
	this->m_Distance[skill] = dis;
	this->m_RequireLeadership[skill] = rLeadership;
	this->m_Delay[skill] = rdelay;
	this->m_BrainPower[skill] = bp;
	this->m_Attr[skill] = Attr;
	this->m_iSkillType[skill] = iSkillType;
	memcpy(m_RequireClass[skill], ReqClass, MAX_TYPE_PLAYER);

	int ReqEnergy = (rEnergy * rlevel * 4);

	if( ReqEnergy > 0 )
	{
		ReqEnergy = ReqEnergy / 100;
	}
	else
	{
		ReqEnergy = 0;
	}

	// Set Level Requirements for Skills
	this->m_RequireLevel[skill] = 0;	// Required Level to use the skill


		/*if ( skill == 41 )	// Twisting Slash
		{
			this->m_RequireLevel[skill] = 80;
		}
		else if ( skill == 42 )	// Rageful Blow
		{
			this->m_RequireLevel[skill] = 170;
		}
		else if ( skill == 43 )	// Death Stab
		{
			this->m_RequireLevel[skill] = 160;
		}
		else if ( skill == 47 )	// Impale
		{
			this->m_RequireLevel[skill] = 28;
		}
		else if ( skill == 48 )	// Inner Strength
		{
			this->m_RequireLevel[skill] = 120;
		}
		else if ( skill == 52 )	
		{
			this->m_RequireLevel[skill] = 130;
		}*/

	if( ReqEnergy > 0 )
	{
		this->m_RequireEnergy[skill] =  ReqEnergy+20;
	}
	else
	{
		switch( skill )
		{
		case 30:
			this->m_RequireEnergy[skill] = 30;
			break;
		case 31:
			this->m_RequireEnergy[skill] = 60;
			break;
		case 32:
			this->m_RequireEnergy[skill] = 90;
			break;
		case 33:
			this->m_RequireEnergy[skill] = 130;
			break;
		case 34:
			this->m_RequireEnergy[skill] = 170;
			break;
		case 35:
			this->m_RequireEnergy[skill] = 210;
			break;
		case 36:
			this->m_RequireEnergy[skill] = 300;
			break;
		default:
			this->m_RequireEnergy[skill] = 0;
			break;
		}
	}
}




int CMagicDamage::Get(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return this->m_Damage[0];
	}

	if ( this->m_Damage[skill] == -1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 202)), __FILE__, __LINE__);
		return this->m_Damage[0];
	}

	return this->m_Damage[skill];
}






int CMagicDamage::SkillGet(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return this->m_Damage[0];
	}

	if ( this->m_Damage[skill] == -1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 202)), __FILE__, __LINE__);
		return this->m_Damage[0];
	}

	return this->m_Damage[skill];
}







int CMagicDamage::SkillGetMana(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return this->m_Mana[0];
	}

	if ( this->m_Damage[skill] == -1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 202)), __FILE__, __LINE__);
		return this->m_Damage[0];
	}

	return this->m_Mana[skill];
}






int CMagicDamage::SkillGetBP(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return 0;
	}

	if ( this->m_Damage[skill] == -1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 202)), __FILE__, __LINE__);
		return 0;
	}

	return this->m_BrainPower[skill];
}








int CMagicDamage::SkillGetRequireEnergy(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	if ( this->m_RequireEnergy[skill] == -1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 202)), __FILE__, __LINE__);
		return -1;
	}

	return this->m_RequireEnergy[skill];
}







BOOL CMagicDamage::SkillGetRequireClass(int Cclass, int ChangeUP,int ChangeUP2,  int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		//LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	if ( Cclass < 0 || Cclass > MAX_TYPE_PLAYER-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 202)), __FILE__, __LINE__);
		return -1;
	}

	int requireclass = this->m_RequireClass[skill][Cclass];

	if ( requireclass == 0 )
	{
		return FALSE;
	}

	if ( requireclass == 3 )
	{
		if( ChangeUP2 > 0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	if ( requireclass > 1 )
	{
		if ( requireclass != (ChangeUP+1) )
		{
			return FALSE;
		}
	}

	return TRUE;
}






int CMagicDamage::GetSkillAttr(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		return -1;
	}

	return this->m_Attr[skill];
}






int CMagicDamage::CheckSkillAttr(int skill, int attr)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		return FALSE;
	}

	return this->m_Attr[skill];
}





int CMagicDamage::GetskillRequireLeadership(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	if ( this->m_RequireLeadership[skill] == -1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 202)), __FILE__, __LINE__);
		return -1;
	}

	return this->m_RequireLeadership[skill];
}



int CMagicDamage::GetDelayTime(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return 0;
	}

	return this->m_Delay[skill];
}






int CMagicDamage::GetSkillType(int iSkill)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return 0;
	}
	
	return this->m_iSkillType[iSkill];
}





int CMagicDamage::SkillGetRequireLevel(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	return this->m_RequireLevel[skill];
}






BOOL CMagicDamage::CheckStatus(int iSkill, int iGuildStatus)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return 0;
	}

	if ( this->m_iSkillUseType[iSkill] == 0 )
	{
		return TRUE;
	}

	if ( g_MasterSkillSystem.CheckMasterLevelSkill(iSkill) )
	{
		return TRUE;
	}

	if ( iGuildStatus == 0x80 )
	{
		if ( this->m_iRequireStatus[iSkill][0] == 1 )
		{
			return TRUE;
		}
	}
	else if ( iGuildStatus == 0x40 )
	{
		if ( this->m_iRequireStatus[iSkill][1] == 1 )
		{
			return TRUE;
		}
	}
	else if ( iGuildStatus == 0x20 )
	{
		if ( this->m_iRequireStatus[iSkill][2] == 1 )
		{
			return TRUE;
		}
	}

	return FALSE;
}






BOOL CMagicDamage::CheckBrandOfSkill(int iSkill)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return 0;
	}

	if ( this->m_wSkillBrand[iSkill] > 0 )
	{
		return TRUE;
	}

	return FALSE;
}





int CMagicDamage::CheckKillCount(int iSkill, int iKillCount)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	return iKillCount - this->m_iKillCount[iSkill];
}


int CMagicDamage::GetSkillDistance(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	return this->m_Distance[skill];
}

WORD CMagicDamage::GetBrandOfSkill(int iSkill)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

return this->m_wSkillBrand[iSkill];
}

WORD CMagicDamage::GetNormalBrandOfSkill(int iSkill)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	int Cnt = 0;
	WORD nBrandOfSkill;
	WORD iBrandOfSkill = iSkill;
	
	while( nBrandOfSkill = this->GetBrandOfSkill(iBrandOfSkill) , g_MasterSkillSystem.CheckMasterLevelSkill(nBrandOfSkill) )
	{
		iBrandOfSkill = nBrandOfSkill;
		Cnt++;

		if( Cnt > 3 )
		{
			LogAdd("[GetNormalBrandOfSkill] fail!!! - %d",iSkill);
			return iSkill;
		}
	}

return nBrandOfSkill;
}

int CMagicDamage::GetSkillUseType(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		//LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

return this->m_iSkillUseType[skill];
}

int CMagicDamage::GetRLevel(int skill)
{
	if ( skill < 0 || skill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

return this->m_rLevel[skill];
}

int CMagicDamage::GetRequireStrength(int iSkill)
{
	if (iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	return this->m_wML_RequireStr[iSkill];
}

int CMagicDamage::GetRequireDexterity(int iSkill)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	return this->m_wML_RequireDex[iSkill];
}

int CMagicDamage::GetSkillRank(int iSkill)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	return this->m_wML_Rank[iSkill];
}

int CMagicDamage::GetSkillGroup(int iSkill)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	return this->m_wML_SkillGroup[iSkill];
}

BYTE CMagicDamage::IsItemAttachedSkill(int iSkill)
{
	if ( iSkill < 0 || iSkill > MAX_SKILL-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 201)), __FILE__, __LINE__);
		return -1;
	}

	return this->m_btIsItemSkill[iSkill];
}