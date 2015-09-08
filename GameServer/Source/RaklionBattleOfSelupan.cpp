// RaklionBattleOfSelupan.cpp: implementation of the CRaklionBattleOfSelupan class.
// GS-N 1.00.90  - finished
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaklionBattleOfSelupan.h"
#include "RaklionBattleUserMng.h"
#include "LogProc.h"
#include "RaklionUtil.h"
#include "User.h"
#include "GameMain.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaklionBattleOfSelupan::CRaklionBattleOfSelupan()
{
	this->ResetAllData();
}

CRaklionBattleOfSelupan::~CRaklionBattleOfSelupan()
{

}

void CRaklionBattleOfSelupan::ResetAllData()
{
	this->m_004 = 0;
	this->m_dwSkillDelay		= 0;
	this->m_dwSkillDelayTick	= 0;
	this->m_bIsUseFirstSkill = 0;

	memset(this->m_PatternSkill,0,sizeof(this->m_PatternSkill)); //0x1C size

	for(int i = 0 ; i< MAX_STATE_PATTERN_COUNT ; i++)
	{
		 this->m_PatternSkill[i].bIsSkillEnabled[SKILL_POISON_ATTACK]		= TRUE;
		 this->m_PatternSkill[i].bIsSkillEnabled[SKILL_ICESTORM]			= TRUE;
		 this->m_PatternSkill[i].bIsSkillEnabled[SKILL_ICESTRIKE]			= TRUE;
		 this->m_PatternSkill[i].bIsSkillEnabled[SKILL_TELEPORT]			= TRUE;
	}

	this->m_PatternSkill[STATE_PATTERN_2].bIsSkillEnabled[SKILL_FREEZE]			= TRUE;//D10
	this->m_PatternSkill[STATE_PATTERN_3].bIsSkillEnabled[SKILL_FREEZE]			= TRUE;//D34

	this->m_PatternSkill[STATE_PATTERN_4].bIsSkillEnabled[SKILL_FREEZE]			= TRUE;//D58
	this->m_PatternSkill[STATE_PATTERN_4].bIsSkillEnabled[SKILL_HEAL]			= TRUE;//D54

	this->m_PatternSkill[STATE_PATTERN_5].bIsSkillEnabled[SKILL_SUMMON]			= TRUE;//D74
	this->m_PatternSkill[STATE_PATTERN_5].bIsSkillEnabled[SKILL_FREEZE]			= TRUE;//D7C
	this->m_PatternSkill[STATE_PATTERN_5].bIsSkillEnabled[SKILL_HEAL]			= TRUE;//D78

	this->m_PatternSkill[STATE_PATTERN_6].bIsSkillEnabled[SKILL_SUMMON]			= TRUE;//D98
	this->m_PatternSkill[STATE_PATTERN_6].bIsSkillEnabled[SKILL_FREEZE]			= TRUE;//DA0
	this->m_PatternSkill[STATE_PATTERN_6].bIsSkillEnabled[SKILL_HEAL]			= TRUE;//D9C

	this->m_PatternSkill[STATE_PATTERN_7].bIsSkillEnabled[SKILL_SUMMON]			= TRUE;//DBC
	this->m_PatternSkill[STATE_PATTERN_7].bIsSkillEnabled[SKILL_FREEZE]			= TRUE;//DC4
	this->m_PatternSkill[STATE_PATTERN_7].bIsSkillEnabled[SKILL_HEAL]			= TRUE;//DC0
	this->m_PatternSkill[STATE_PATTERN_7].bIsSkillEnabled[SKILL_INVINCIBILITY]	= TRUE;//DCC
	
	this->SetSuccessValue(FALSE);
	this->SetBattleOfSelupanState(SELUPAN_STATE_NONE);
}

int CRaklionBattleOfSelupan::Init() //00643540 
{
	//need fix size
	return 1;
}

void CRaklionBattleOfSelupan::Run()
{
	if ( this->m_RaklionSelupan.GetSelupanLife() <= 0 && this->GetBattleOfSelupanState() != SELUPAN_STATE_DIE  )
	{
		this->SetState(SELUPAN_STATE_DIE);
		return;
	}

	if ( this->m_dwSkillDelay > GetTickCount() - this->m_dwSkillDelayTick )
	{
		return;
	}

	switch ( this->m_iBattleOfSelupanState )
	{
	case SELUPAN_STATE_NONE:		this->ProcState_NONE();		break;
	case SELUPAN_STATE_STANDBY:		this->ProcState_STANDBY();	break;
	case SELUPAN_STATE_PATTERN_1:	this->ProcState_PATTERN_1();break;
	case SELUPAN_STATE_PATTERN_2:	this->ProcState_PATTERN_2();break;
	case SELUPAN_STATE_PATTERN_3:	this->ProcState_PATTERN_3();break;
	case SELUPAN_STATE_PATTERN_4:	this->ProcState_PATTERN_4();break;
	case SELUPAN_STATE_PATTERN_5:	this->ProcState_PATTERN_5();break;
	case SELUPAN_STATE_PATTERN_6:	this->ProcState_PATTERN_6();break;
	case SELUPAN_STATE_PATTERN_7:	this->ProcState_PATTERN_7();break;
	case SELUPAN_STATE_DIE:			this->ProcState_DIE();		break;
	}
}

void CRaklionBattleOfSelupan::SetState(int iBattleOfSelupanState)
{
	if ( this->m_dwSkillDelay > GetTickCount() - this->m_dwSkillDelayTick )
	{
		return;
	}
		
	switch ( iBattleOfSelupanState )
	{
	case SELUPAN_STATE_NONE:		this->SetState_NONE();		break;
	case SELUPAN_STATE_STANDBY:		this->SetState_STANDBY();	break;
	case SELUPAN_STATE_PATTERN_1:	this->SetState_PATTERN_1();	break;
	case SELUPAN_STATE_PATTERN_2:	this->SetState_PATTERN_2();	break;
	case SELUPAN_STATE_PATTERN_3:	this->SetState_PATTERN_3();	break;
	case SELUPAN_STATE_PATTERN_4:	this->SetState_PATTERN_4();	break;
	case SELUPAN_STATE_PATTERN_5:	this->SetState_PATTERN_5();	break;
	case SELUPAN_STATE_PATTERN_6:	this->SetState_PATTERN_6();	break;
	case SELUPAN_STATE_PATTERN_7:	this->SetState_PATTERN_7();	break;
	case SELUPAN_STATE_DIE:			this->SetState_DIE();		break;
	}

	g_RaklionUtil.NotifyRaklionChangeState(11, this->GetBattleOfSelupanState());
}

void CRaklionBattleOfSelupan::SetState_NONE()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> NONE", this->m_iBattleOfSelupanState);
	
	this->m_RaklionSelupan.SelupanAct_BerserkCansel();
	
	this->SetBattleOfSelupanState(SELUPAN_STATE_NONE);

}

void CRaklionBattleOfSelupan::SetState_STANDBY()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> STANDBY", this->m_iBattleOfSelupanState);
	
	this->m_RaklionSelupan.InitSelupanObjIndex();
	this->m_RaklionSelupan.CreateSelupan();
	this->m_RaklionSelupan.SelupanAct_BerserkCansel();
	
	this->SetBattleOfSelupanState(SELUPAN_STATE_STANDBY);
}

void CRaklionBattleOfSelupan::SetState_PATTERN_1()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> PATTERN_1 // Life(%d/4800000)", this->m_iBattleOfSelupanState,this->m_RaklionSelupan.GetSelupanLife());
	
	this->m_RaklionSelupan.SelupanAct_BerserkCansel();
	
	this->SetBattleOfSelupanState(SELUPAN_STATE_PATTERN_1);
}

void CRaklionBattleOfSelupan::SetState_PATTERN_2()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> PATTERN_2 // Life(%d/4800000)", this->m_iBattleOfSelupanState,this->m_RaklionSelupan.GetSelupanLife());

	this->m_RaklionSelupan.SelupanAct_Berserk1();  
	
	this->SetBattleOfSelupanState(SELUPAN_STATE_PATTERN_2);
}

void CRaklionBattleOfSelupan::SetState_PATTERN_3()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> PATTERN_3 // Life(%d/4800000)", this->m_iBattleOfSelupanState,this->m_RaklionSelupan.GetSelupanLife());
	
	this->m_RaklionSelupan.SelupanAct_Berserk2();
	
	this->SetBattleOfSelupanState(SELUPAN_STATE_PATTERN_3);
}

void CRaklionBattleOfSelupan::SetState_PATTERN_4()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> PATTERN_4 // Life(%d/4800000)", this->m_iBattleOfSelupanState,this->m_RaklionSelupan.GetSelupanLife());
	
	if ( this->m_RaklionSelupan.GetBerserkIndex() != 2 )
	{
		this->m_RaklionSelupan.SelupanAct_Berserk2();
	}
	
	this->SetBattleOfSelupanState(SELUPAN_STATE_PATTERN_4);
}

void CRaklionBattleOfSelupan::SetState_PATTERN_5()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> PATTERN_5 // Life(%d/4800000)", this->m_iBattleOfSelupanState,this->m_RaklionSelupan.GetSelupanLife());
	
	this->m_RaklionSelupan.SelupanAct_Berserk3();
	
	this->SetBattleOfSelupanState(SELUPAN_STATE_PATTERN_5);
}

void CRaklionBattleOfSelupan::SetState_PATTERN_6()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> PATTERN_6 // Life(%d/4800000)", this->m_iBattleOfSelupanState,this->m_RaklionSelupan.GetSelupanLife());
	
	this->m_RaklionSelupan.SelupanAct_Berserk4();
	
	this->SetBattleOfSelupanState(SELUPAN_STATE_PATTERN_6);
}

void CRaklionBattleOfSelupan::SetState_PATTERN_7()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> PATTERN_7 // Life(%d/4800000)", this->m_iBattleOfSelupanState,this->m_RaklionSelupan.GetSelupanLife());
	
	if ( this->m_RaklionSelupan.GetBerserkIndex() != 4 )
	{
		this->m_RaklionSelupan.SelupanAct_Berserk4();
	}

	this->SetBattleOfSelupanState(SELUPAN_STATE_PATTERN_7);
}

void CRaklionBattleOfSelupan::SetState_DIE()
{
	LogAddC(5, "[ RAKLION ][ BattleOfSelupan ] State(%d) -> DIE", this->m_iBattleOfSelupanState);
	
	this->m_RaklionSelupan.SelupanAct_BerserkCansel();
	
	this->DeleteSummonMonster();
	
	int aIndex = this->m_RaklionSelupan.GetSelupanTargetIndex();

	LPOBJ lpObj = &gObj[aIndex];

	g_RaklionUtil.SendMsgAllUser(lMsg.Get(3412),lpObj->Name);

	time_t ltime;
	time(&ltime);
	tm * today=localtime(&ltime);

	for ( int i = 0; i < g_RaklionBattleUserMng.GetBattleUserCount(); ++i )
	{
		LogAdd("[Raklion][SelupanDie] %d/%d/%d %d:%d:%d %dth Winner : %s (%s)",
			today->tm_year+1900,today->tm_mon+1,today->tm_mday,today->tm_hour,
			today->tm_min,today->tm_sec,i+1,gObj[g_RaklionBattleUserMng.GetBattleUserIndex(i)].Name,gObj[g_RaklionBattleUserMng.GetBattleUserIndex(i)].AccountID);
	}

	this->SetBattleOfSelupanState(SELUPAN_STATE_DIE);
}

void CRaklionBattleOfSelupan::ProcState_NONE()
{

}

void CRaklionBattleOfSelupan::ProcState_STANDBY()
{

}

void CRaklionBattleOfSelupan::ProcState_PATTERN_1()
{
	int skill = 0;

	while(this->m_PatternSkill[STATE_PATTERN_1].bIsSkillEnabled[skill] == 0)
	{
		skill = rand() % 9;
	}
	
	this->PatternStateSkillUseProc(skill);
}

void CRaklionBattleOfSelupan::ProcState_PATTERN_2()
{
	int skill = 0;

	while(this->m_PatternSkill[STATE_PATTERN_2].bIsSkillEnabled[skill] == 0)
	{
		skill = rand() % 9;
	}
	
	this->PatternStateSkillUseProc(skill);
}

void CRaklionBattleOfSelupan::ProcState_PATTERN_3()
{
	int skill = 0;

	while(this->m_PatternSkill[STATE_PATTERN_3].bIsSkillEnabled[skill] == 0)
	{
		skill = rand() % 9;
	}
	
	this->PatternStateSkillUseProc(skill);
}

void CRaklionBattleOfSelupan::ProcState_PATTERN_4()
{
	int skill = 0;

	while(this->m_PatternSkill[STATE_PATTERN_4].bIsSkillEnabled[skill] == 0)
	{
		skill = rand() % 9;
	}
	
	this->PatternStateSkillUseProc(skill);
}

void CRaklionBattleOfSelupan::ProcState_PATTERN_5()
{
	int skill = 0;

	while(this->m_PatternSkill[STATE_PATTERN_5].bIsSkillEnabled[skill] == 0)
	{
		skill = rand() % 9;
	}
	
	this->PatternStateSkillUseProc(skill);
}

void CRaklionBattleOfSelupan::ProcState_PATTERN_6()
{
	int skill = 0;

	while(this->m_PatternSkill[STATE_PATTERN_6].bIsSkillEnabled[skill] == 0)
	{
		skill = rand() % 9;
	}
	
	this->PatternStateSkillUseProc(skill);
}

void CRaklionBattleOfSelupan::ProcState_PATTERN_7()
{
	int skill = 0;

	while(this->m_PatternSkill[STATE_PATTERN_7].bIsSkillEnabled[skill] == 0)
	{
		skill = rand() % 9;
	}
	
	this->PatternStateSkillUseProc(skill);
}

void CRaklionBattleOfSelupan::ProcState_DIE()
{

}

void CRaklionBattleOfSelupan::PatternStateSkillUseProc(int SkillIndex)
{
	int iSelupanIndex = this->m_RaklionSelupan.GetSelupanObjIndex();

	LPOBJ lpObj = &gObj[iSelupanIndex];

	if ( lpObj->Teleport != 0 || lpObj->m_ActState.Move != FALSE )
	{
		return;
	}

	if ( SkillIndex < 0 || SkillIndex >= 9)
	{
		return;
	}

	switch ( SkillIndex )
	{
	case SKILL_FIRSTSKILL:		this->m_RaklionSelupan.SelupanAct_FirstSkill();		break;
	case SKILL_POISON_ATTACK:	this->m_RaklionSelupan.SelupanAct_PoisonAttack();	break;
	case SKILL_ICESTORM:		this->m_RaklionSelupan.SelupanAct_IceStorm();		break;
	case SKILL_ICESTRIKE:		this->m_RaklionSelupan.SelupanAct_IceStrike();		break;
	case SKILL_SUMMON:			this->m_RaklionSelupan.SelupanAct_SummonMonster();	break;
	case SKILL_HEAL:			this->m_RaklionSelupan.SelupanAct_Heal();			break;
	case SKILL_FREEZE:			this->m_RaklionSelupan.SelupanAct_Freeze();			break;
	case SKILL_TELEPORT:		this->m_RaklionSelupan.SelupanAct_Teleport();		break;
	case SKILL_INVINCIBILITY:	this->m_RaklionSelupan.SelupanAct_Invincibility();	break;
	}
	
	this->m_dwSkillDelayTick	= GetTickCount();
	this->m_dwSkillDelay		= this->m_RaklionSelupan.GetSelupanSkillDelay();
}

void CRaklionBattleOfSelupan::SetPatternCondition(int iPattern,int iCondition)
{
	if ( iPattern < 0 || iCondition < 0 )
	{
		LogAddC(2,"[RAKLION][SetPatternCondition] error : PatternNum(%d), Condition(%d)",iPattern,iCondition);
	}

	this->m_PatternCondition[iPattern] = iCondition;
}

void CRaklionBattleOfSelupan::SelupanLifeCheck()
{
	int aIndex = 0;
	int MaxLife = 0;
	int Percent = 0;
	LPOBJ lpObj = 0;

	aIndex = this->m_RaklionSelupan.GetSelupanObjIndex();
	
	lpObj = &gObj[aIndex];
	
	if ( lpObj->DieRegen != 0)
	{
		return;
	}

	MaxLife = lpObj->AddLife + lpObj->MaxLife;
	
	if (MaxLife <= 0)
	{
		LogAddC(2,"[RAKLION] SelupanMAXLife error : Index(%d), MaxLife(%d)",lpObj->m_Index,lpObj->MaxLife);
		return;
	}	

	Percent = this->m_RaklionSelupan.GetSelupanLife() * 100 / MaxLife;
	
	if ( Percent < 0 )
	{
		LogAddC(2,"[RAKLION] SelupanLifePercent error : Index(%d), MaxLife(%d)",lpObj->m_Index,Percent);
		return;
	}
	
	if ( this->m_bIsUseFirstSkill == FALSE )
    {
		this->PatternStateSkillUseProc(SKILL_FIRSTSKILL);
		this->m_bIsUseFirstSkill = 1;
		return;
	}
	
	if ( Percent > this->m_PatternCondition[0] )
	{
		if(this->GetBattleOfSelupanState() != SELUPAN_STATE_PATTERN_1)
		{
			this->SetState(SELUPAN_STATE_PATTERN_1);
		}
	}
	else if ( Percent > this->m_PatternCondition[1] )
	{
		if ( this->GetBattleOfSelupanState() != SELUPAN_STATE_PATTERN_2 )
		{
			this->SetState(SELUPAN_STATE_PATTERN_2);
		}
	}
	else if ( Percent > this->m_PatternCondition[2] )
	{
		if ( this->GetBattleOfSelupanState() != SELUPAN_STATE_PATTERN_3 )
		{
			this->SetState(SELUPAN_STATE_PATTERN_3);
		}
	}
	else if ( Percent > this->m_PatternCondition[3] )
	{
		if ( this->GetBattleOfSelupanState() != SELUPAN_STATE_PATTERN_4 )
		{
			this->SetState(SELUPAN_STATE_PATTERN_4);
		}
	}
	else if ( Percent > this->m_PatternCondition[4] )
	{
		if ( this->GetBattleOfSelupanState() != SELUPAN_STATE_PATTERN_5 )
		{
			this->SetState(SELUPAN_STATE_PATTERN_5);
		}
	}
	else if ( Percent > this->m_PatternCondition[5] )
	{
		if ( this->GetBattleOfSelupanState() != SELUPAN_STATE_PATTERN_6 )
		{
			this->SetState(SELUPAN_STATE_PATTERN_6);
		}
	}
	else if ( Percent <= this->m_PatternCondition[5] )
	{
		if ( this->GetBattleOfSelupanState() != SELUPAN_STATE_PATTERN_7 )
		{
			this->SetState(SELUPAN_STATE_PATTERN_7);
		}
	}
}

int CRaklionBattleOfSelupan::GetBattleUserCount()
{
	return g_RaklionBattleUserMng.GetBattleUserCount();
}

void CRaklionBattleOfSelupan::CreateSelupan()
{
	this->m_RaklionSelupan.CreateSelupan();
}

void CRaklionBattleOfSelupan::DeleteSelupan()
{
	this->m_RaklionSelupan.DeleteSelupan();
}

int CRaklionBattleOfSelupan::GetSelupanObjIndex()
{
	return this->m_RaklionSelupan.GetSelupanObjIndex();
}

void CRaklionBattleOfSelupan::DeleteSummonMonster()
{
	this->m_RaklionSelupan.DeleteSummonMonster();
}

void CRaklionBattleOfSelupan::SetSelupanSkillDelay(int SkillDelay)
{
	this->m_RaklionSelupan.SetSelupanSkillDelay(SkillDelay);
}

void CRaklionBattleOfSelupan::GmCommandSelupanPattern(int StatePattern)
{
	if ( StatePattern < 2 || StatePattern > 8)
	{
		LogAddC(2,"[RAKLION] [GmCommandSelupanPattern] Invalid pattern number : %d",StatePattern);
		return;
	}
	
	switch(StatePattern)
	{
	case 2: this->m_RaklionSelupan.SetSelupanLife(100);	break;
	case 3: this->m_RaklionSelupan.SetSelupanLife(80);	break;
	case 4:	this->m_RaklionSelupan.SetSelupanLife(60);	break;
	case 5:	this->m_RaklionSelupan.SetSelupanLife(50);	break;
	case 6:	this->m_RaklionSelupan.SetSelupanLife(40);	break;
	case 7:	this->m_RaklionSelupan.SetSelupanLife(20);	break;
	case 8:	this->m_RaklionSelupan.SetSelupanLife(10);	break;
	}
}