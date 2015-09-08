#include "stdafx.h"
#include "BuffSkillEffect.h"
#include "GameMain.h"
#include "..\pugixml\pugixml.hpp"
// -------------------------------------------------------------------------------

using namespace pugi;
BuffSkillEffect g_BuffSkillEffect;
// -------------------------------------------------------------------------------

BuffSkillEffect::BuffSkillEffect()
{

}
// -------------------------------------------------------------------------------

BuffSkillEffect::~BuffSkillEffect()
{

}
// -------------------------------------------------------------------------------

void BuffSkillEffect::Init()
{
	this->m_EffectInfo.clear();
	if( this->m_EffectInfo.capacity() > 0 )
	{
		std::vector<BuffSkillEffectInfo>().swap(this->m_EffectInfo);
	}
}
// -------------------------------------------------------------------------------

void BuffSkillEffect::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_SKILL_BUFFSKILLEFFECT));
}
// -------------------------------------------------------------------------------

void BuffSkillEffect::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[BuffSkillEffect] File %s not found! %d", File, Result.status);
		return;
	}
	// ----
	xml_node BuffSkillEffect = Document.child("buffskilleffect");
	xml_node EffectList = BuffSkillEffect.child("effectlist");
	// ----
	for( xml_node Node = EffectList.child("skill"); Node; Node = Node.next_sibling() )
	{
		BuffSkillEffectInfo lpInfo = { 0 };
		lpInfo.m_SkillNumber = Node.attribute("number").as_int(-1);
		lpInfo.m_Arg1 = Node.attribute("arg1").as_float(-1);
		lpInfo.m_Arg2 = Node.attribute("arg2").as_float(-1);
		lpInfo.m_Arg3 = Node.attribute("arg3").as_float(-1);
		lpInfo.m_Arg4 = Node.attribute("arg4").as_float(-1);
		lpInfo.m_Arg5 = Node.attribute("arg5").as_float(-1);
		lpInfo.m_Arg6 = Node.attribute("arg6").as_float(-1);
		lpInfo.m_Arg7 = Node.attribute("arg7").as_float(-1);
		lpInfo.m_Arg8 = Node.attribute("arg8").as_float(-1);
		lpInfo.m_Arg9 = Node.attribute("arg9").as_float(-1);
		lpInfo.m_Arg10 = Node.attribute("arg10").as_float(-1);
		lpInfo.m_Arg11 = Node.attribute("arg11").as_float(-1);
		lpInfo.m_Arg12 = Node.attribute("arg12").as_float(-1);
		this->m_EffectInfo.push_back(lpInfo);
	}
}
// -------------------------------------------------------------------------------

BuffSkillEffectInfo* BuffSkillEffect::GetEffect(int SkillNumber)
{
	for( int i = 0; i < this->m_EffectInfo.size(); i++ )
	{
		if( this->m_EffectInfo[i].m_SkillNumber == SkillNumber )
		{
			return &this->m_EffectInfo[i];
		}
	}
	// ----
	return NULL;
}
// -------------------------------------------------------------------------------