#include "stdafx.h"
#include "MonsterAttr.h"
#include "LogProc.h"
#include "..\include\readscript.h"
#include "..\common\WzMemScript.h"
#include "user.h"
#include "GameMain.h"
#include "..\pugixml\pugixml.hpp"
// -------------------------------------------------------------------------------

using namespace pugi;
// -------------------------------------------------------------------------------

CMonsterAttr::CMonsterAttr()
{

}
// -------------------------------------------------------------------------------

CMonsterAttr::~CMonsterAttr()
{

}
// -------------------------------------------------------------------------------

LPMONSTER_ATTRIBUTE CMonsterAttr::GetAttr(int aClass)
{
	for( int n = 0; n <	this->m_MonsterAttr.size(); n++ )
	{
		if( this->m_MonsterAttr[n].m_Index == aClass )
		{
			return &this->m_MonsterAttr[n];
		}
	}
	// ----
	return NULL;
}
// -------------------------------------------------------------------------------

void CMonsterAttr::LoadAttr(char* filename)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(filename);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[MonsterAttr] File %s not found!", filename);
		return;
	}
	// ----
	this->m_MonsterAttr.clear();
	xml_node MonsterAttr = Document.child("monsterinfo");
	// ----
	for( xml_node Node = MonsterAttr.child("monster"); Node; Node = Node.next_sibling() )
	{
		MONSTER_ATTRIBUTE lpInfo = { 0 };
		lpInfo.m_Index = Node.attribute("id").as_int();
		memcpy(lpInfo.m_Name, Node.attribute("name").as_string(), sizeof(lpInfo.m_Name));
		lpInfo.m_Rate = Node.attribute("rate").as_int();
		lpInfo.m_Level = Node.attribute("level").as_int();
		lpInfo.m_Hp = Node.attribute("hp").as_int();
		lpInfo.m_Mp = Node.attribute("mp").as_int();
		lpInfo.m_DamageMin = Node.attribute("damagemin").as_int();
		lpInfo.m_DamageMax = Node.attribute("damagemax").as_int();
		lpInfo.m_Defense = Node.attribute("defense").as_int();
		lpInfo.m_MagicDefense = Node.attribute("defense").as_int();
		lpInfo.m_AttackRating = Node.attribute("attackrate").as_int();
		lpInfo.m_Successfulblocking = Node.attribute("blockrate").as_int();
		lpInfo.m_MoveRange = Node.attribute("moverange").as_int();
		lpInfo.m_AttackType = Node.attribute("attacktype").as_int();
		lpInfo.m_AttackRange = Node.attribute("attackrange").as_int();
		lpInfo.m_ViewRange = Node.attribute("viewrange").as_int();
		lpInfo.m_MoveSpeed = Node.attribute("movespeed").as_int();
		lpInfo.m_AttackSpeed = Node.attribute("attackspeed").as_int();
		lpInfo.m_RegenTime = Node.attribute("regentime").as_int();
		lpInfo.m_Attribute = Node.attribute("attribute").as_int();
		lpInfo.m_ItemRate = Node.attribute("itemrate").as_int();
		lpInfo.m_MoneyRate = Node.attribute("moneyrate").as_int();
		lpInfo.m_MaxItemLevel = Node.attribute("maxitemlevel").as_int();
		lpInfo.m_MonsterSkill = Node.attribute("skill").as_int();
		lpInfo.m_Resistance[R_ICE]  = Node.attribute("resice").as_int();
		lpInfo.m_Resistance[R_POISON] = Node.attribute("respoison").as_int();
		lpInfo.m_Resistance[R_LIGHTNING] = Node.attribute("reslight").as_int();
		lpInfo.m_Resistance[R_FIRE] = Node.attribute("resfire").as_int();
		lpInfo.m_ElementDmgMin = Node.attribute("elementdamagemin").as_int();
		lpInfo.m_ElementDmgMax = Node.attribute("elementdamagemax").as_int();
		lpInfo.m_ElementDefense = Node.attribute("elementdefense").as_int();
		lpInfo.m_ElementAttackRating = Node.attribute("elementattackrate").as_int();
		lpInfo.m_ElementBlockRating = Node.attribute("elementblockrate").as_int();
		// ----
		lpInfo.m_Hp = (int)(lpInfo.m_Hp - (int)(lpInfo.m_Hp / 100.0f * gMonsterHp));
		lpInfo.m_iScriptHP = lpInfo.m_Hp;
		// ----
		if( lpInfo.m_Level <= 24 && lpInfo.m_Index < 50 )
		{
			if(		lpInfo.m_Level == 13 ||
					lpInfo.m_Level == 14 ||
					lpInfo.m_Level == 17 ||
					lpInfo.m_Level == 18 ||
					lpInfo.m_Level == 19 ||
					lpInfo.m_Level == 24 )
			{
				int modiryvalue = lpInfo.m_Hp * 20 / 100;
				modiryvalue -= modiryvalue * gMonsterHPAdjust / 100;
				lpInfo.m_Hp -= modiryvalue;
			}
			else if(	lpInfo.m_Level == 20 ||
						lpInfo.m_Level == 22 )
			{
				int modiryvalue = lpInfo.m_Hp * 30 / 100;
				modiryvalue -= modiryvalue * gMonsterHPAdjust / 100;
				lpInfo.m_Hp -= modiryvalue;
			}
			else
			{
				int modiryvalue = lpInfo.m_Hp / 2;
				modiryvalue -= modiryvalue * gMonsterHPAdjust / 100;
				lpInfo.m_Hp -= modiryvalue;
			}
		}
		// ----
		if( lpInfo.m_Level <= 19 && lpInfo.m_Index < 50 )
		{
			if(		lpInfo.m_Level == 13 ||
					lpInfo.m_Level == 14 ||
					lpInfo.m_Level == 17 ||
					lpInfo.m_Level == 18 ||
					lpInfo.m_Level == 19 )
			{
				int modiryvaluemin = lpInfo.m_DamageMin * 30 / 100;
				int modiryvaluemax = lpInfo.m_DamageMax * 30 / 100;
				modiryvaluemin -= modiryvaluemin * gMonsterHPAdjust / 100;
				modiryvaluemax -= modiryvaluemax * gMonsterHPAdjust / 100;
				lpInfo.m_DamageMin -= modiryvaluemin;
				lpInfo.m_DamageMax -= modiryvaluemax;
			}
			else
			{
				int modiryvaluemin = lpInfo.m_DamageMin / 2;
				int modiryvaluemax = lpInfo.m_DamageMax / 2;
				modiryvaluemin -= modiryvaluemin * gMonsterHPAdjust / 100;
				modiryvaluemax -= modiryvaluemax * gMonsterHPAdjust / 100;
				lpInfo.m_DamageMin -= modiryvaluemin;
				lpInfo.m_DamageMax -= modiryvaluemax;
			}
		}
		// ----
		this->m_MonsterAttr.push_back(lpInfo);
	}

	/*int Token;
	int n;

	SMDFile = fopen(filename, "r");

	if ( SMDFile == NULL )
	{
		MsgBox("load error %s", filename);
	}
	else
	{
		n= 0;

		while ( true )
		{
			Token = GetToken();

			if ( Token == 2 )
			{
				break;
			}
			else
			{
				if ( Token == 1 )
				{
					this->m_MonsterAttr[n].m_Index = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_Rate = (int)TokenNumber;

					Token = GetToken();
					strcpy(this->m_MonsterAttr[n].m_Name, TokenString);

					Token = GetToken();
					this->m_MonsterAttr[n].m_Level = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_Hp = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_Mp = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_DamageMin = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_DamageMax = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_Defense = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_MagicDefense = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_AttackRating = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_Successfulblocking = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_btElement = (BYTE)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_ElementDmgMin = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_ElementDmgMax = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_ElementDefense = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_ElementAttackRating = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_ElementBlockRating = (int)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_MoveRange = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_AttackType = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_AttackRange = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_ViewRange = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_MoveSpeed = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_AttackSpeed = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_RegenTime = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_Attribute = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_ItemRate = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_MoneyRate = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_MaxItemLevel = (int)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_MonsterSkill = (DWORD)TokenNumber;

					Token = GetToken();
					this->m_MonsterAttr[n].m_Resistance[R_ICE]  = (BYTE)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_Resistance[R_POISON] = (BYTE)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_Resistance[R_LIGHTNING] = (BYTE)TokenNumber;
					
					Token = GetToken();
					this->m_MonsterAttr[n].m_Resistance[R_FIRE] = (BYTE)TokenNumber;

					this->m_MonsterAttr[n].m_Hp = (int)(this->m_MonsterAttr[n].m_Hp - (INT)(this->m_MonsterAttr[n].m_Hp / 100.0f * gMonsterHp));

					this->m_MonsterAttr[n].m_iScriptHP = this->m_MonsterAttr[n].m_Hp;

					if ( this->m_MonsterAttr[n].m_Level <= 24 && this->m_MonsterAttr[n].m_Index < 50)
					{
						if ( this->m_MonsterAttr[n].m_Level == 13 ||
							 this->m_MonsterAttr[n].m_Level == 14 ||
							 this->m_MonsterAttr[n].m_Level == 17 ||
							 this->m_MonsterAttr[n].m_Level == 18 ||
							 this->m_MonsterAttr[n].m_Level == 19 ||
							 this->m_MonsterAttr[n].m_Level == 24 )
						{
							int modiryvalue = this->m_MonsterAttr[n].m_Hp*20/100;
							modiryvalue-= modiryvalue*gMonsterHPAdjust/100;
							this->m_MonsterAttr[n].m_Hp -= modiryvalue;
						}
						else if ( this->m_MonsterAttr[n].m_Level == 20 ||
								  this->m_MonsterAttr[n].m_Level == 22 )
						{
							int modiryvalue = this->m_MonsterAttr[n].m_Hp*30/100;
							modiryvalue-= modiryvalue*gMonsterHPAdjust/100;
							this->m_MonsterAttr[n].m_Hp -= modiryvalue;
						}
						else
						{
							int modiryvalue = this->m_MonsterAttr[n].m_Hp/2;
							modiryvalue-= modiryvalue*gMonsterHPAdjust/100;
							this->m_MonsterAttr[n].m_Hp -= modiryvalue;
						}
					}

					if ( this->m_MonsterAttr[n].m_Level  <= 19 && this->m_MonsterAttr[n].m_Index  < 50 )
					{
						if ( this->m_MonsterAttr[n].m_Level == 13 ||
							 this->m_MonsterAttr[n].m_Level == 14 ||
							 this->m_MonsterAttr[n].m_Level == 17 ||
							 this->m_MonsterAttr[n].m_Level == 18 ||
							 this->m_MonsterAttr[n].m_Level == 19 )
						 {
							int modiryvaluemin = this->m_MonsterAttr[n].m_DamageMin*30/100;
							int modiryvaluemax = this->m_MonsterAttr[n].m_DamageMax*30/100;

							modiryvaluemin -= modiryvaluemin*gMonsterHPAdjust/100;
							modiryvaluemax -= modiryvaluemax*gMonsterHPAdjust/100;

							this->m_MonsterAttr[n].m_DamageMin -= modiryvaluemin;
							this->m_MonsterAttr[n].m_DamageMax -= modiryvaluemax;
						}
						else
						{
							int modiryvaluemin = this->m_MonsterAttr[n].m_DamageMin/2;
							int modiryvaluemax = this->m_MonsterAttr[n].m_DamageMax/2;

							modiryvaluemin -= modiryvaluemin*gMonsterHPAdjust/100;
							modiryvaluemax -= modiryvaluemax*gMonsterHPAdjust/100;

							this->m_MonsterAttr[n].m_DamageMin -= modiryvaluemin;
							this->m_MonsterAttr[n].m_DamageMax -= modiryvaluemax;
						}
					}	 

					n++;

					if ( n> MAX_MONSTER_TYPE-1 )
					{
						MsgBox("Monster attribute max over %s %d", __FILE__, __LINE__);
						fclose(SMDFile);
						return;
					}
				}
			}
		}
	}

	fclose(SMDFile);*/
}
// -------------------------------------------------------------------------------

#include "BuffEffect.h"

CCustomMonsterAttr gCMAttr;

CCustomMonsterAttr::CCustomMonsterAttr()
{
	
}

CCustomMonsterAttr::~CCustomMonsterAttr()
{
	if( !m_MonsterData.empty() )
	{
		std::map<int,CUSTOM_MONSTER_ATTR*>::iterator _It;

		for(_It = m_MonsterData.begin(); _It != m_MonsterData.end(); _It++)	{
			if( _It->second )
				delete _It->second;
		}
	}
}

void CCustomMonsterAttr::Load(char* szFileName)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(szFileName);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[MonsterStatInfo] File %s not found!", szFileName);
		return;
	}
	// ----
	if( !m_MonsterData.empty() )
	{
		std::map<int,CUSTOM_MONSTER_ATTR*>::iterator _It;

		for(_It = m_MonsterData.begin(); _It != m_MonsterData.end(); _It++)	{
			if( _It->second )
				delete _It->second;
		}
	}
	m_MonsterData.clear();
	// ----
	xml_node MonsterStatInfo = Document.child("monsterstatinfo");
	// ----
	this->m_DefaultData.HPMul = MonsterStatInfo.child("default").attribute("hp").as_float();
	this->m_DefaultData.DmgMul = MonsterStatInfo.child("default").attribute("damage").as_float();
	this->m_DefaultData.DefMul = MonsterStatInfo.child("default").attribute("defense").as_float();
	this->m_DefaultData.ARateMul = MonsterStatInfo.child("default").attribute("attackrate").as_float();
	this->m_DefaultData.DRateMul = MonsterStatInfo.child("default").attribute("defenserate").as_float();
	// ----
	for( xml_node Node = MonsterStatInfo.child("specific").child("monster"); Node; Node = Node.next_sibling() )
	{
		CUSTOM_MONSTER_ATTR* pTmp = new CUSTOM_MONSTER_ATTR;
		int MonIndex = Node.attribute("id").as_int();
		pTmp->HPMul = Node.attribute("hp").as_float();
		pTmp->DmgMul = Node.attribute("damage").as_float();
		pTmp->DefMul = Node.attribute("defense").as_float();
		pTmp->ARateMul = Node.attribute("attackrate").as_float();
		pTmp->DRateMul = Node.attribute("defenserate").as_float();
		m_MonsterData.insert(std::pair<int,CUSTOM_MONSTER_ATTR*>(MonIndex,pTmp));
	}
}
