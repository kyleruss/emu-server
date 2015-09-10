// ExtraClassBalance.cpp: implementation of the ExtraClassBalance class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExtraClassBalance.h"
#include "..\pugixml\pugixml.hpp"
#include "GameMain.h"

using namespace pugi;
ExtraClassBalance g_ExtraClassBalance;

ExtraClassBalance::ExtraClassBalance()
{

}

ExtraClassBalance::~ExtraClassBalance()
{

}

void ExtraClassBalance::Init()
{
	this->m_AttackInfo.clear();
	if( this->m_AttackInfo.capacity() > 0 )
	{
		std::vector<ExtraClassAttack>().swap(this->m_AttackInfo);
	}
}

void ExtraClassBalance::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CUSTOM_EXTRACLASSBALANCE));
}

void ExtraClassBalance::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[ExtraClassBalance] File %s not found!", File);
		return;
	}
	// ----
	xml_node ExtraClassBalance = Document.child("extraclassbalance");
	xml_node AttackList = ExtraClassBalance.child("attackincrease");
	// ----
	for( xml_node Node = AttackList.child("increase"); Node; Node = Node.next_sibling() )
	{
		ExtraClassAttack lpAttackInfo;
		lpAttackInfo.Class = Node.attribute("class").as_int();
		lpAttackInfo.Reset = Node.attribute("reset").as_int();
		lpAttackInfo.Damage = Node.attribute("damage").as_int();
		lpAttackInfo.Absorb = Node.attribute("absorb").as_int();
		this->m_AttackInfo.push_back(lpAttackInfo);
	}
}

void ExtraClassBalance::Apply(LPOBJ lpUser)
{
	for( int i = 0; i < this->m_AttackInfo.size(); i++ )
	{
		if( lpUser->Class == this->m_AttackInfo[i].Class
			&& lpUser->iResetCount == this->m_AttackInfo[i].Reset )
		{
			if( this->m_AttackInfo[i].Absorb )
			{
				lpUser->DamageMinus += this->m_AttackInfo[i].Absorb;
			}
			if( this->m_AttackInfo[i].Damage )
			{
				lpUser->m_AttackDamageMaxLeft += lpUser->m_AttackDamageMaxLeft * this->m_AttackInfo[i].Damage / 100;
				lpUser->m_AttackDamageMinLeft += lpUser->m_AttackDamageMinLeft * this->m_AttackInfo[i].Damage / 100;
				lpUser->m_AttackDamageMaxRight += lpUser->m_AttackDamageMaxRight * this->m_AttackInfo[i].Damage / 100;
				lpUser->m_AttackDamageMinRight += lpUser->m_AttackDamageMinRight * this->m_AttackInfo[i].Damage / 100;
				lpUser->m_MagicDamageMin += lpUser->m_MagicDamageMin * this->m_AttackInfo[i].Damage / 100;
				lpUser->m_MagicDamageMax += lpUser->m_MagicDamageMax * this->m_AttackInfo[i].Damage / 100;
				lpUser->m_CurseDamageMin += lpUser->m_CurseDamageMin * this->m_AttackInfo[i].Damage / 100;
				lpUser->m_CurseDamageMax += lpUser->m_CurseDamageMax * this->m_AttackInfo[i].Damage / 100;
			}
		}
	}
}