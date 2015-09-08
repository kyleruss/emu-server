#include "stdafx.h"
#include "MapRateInfo.h"
#include "..\pugixml\pugixml.hpp"
#include "GameMain.h"
#include "MasterSkillSystem.h"

using namespace pugi;
MapRateInfo g_MapRateInfo;

MapRateInfo::MapRateInfo()
{

}

MapRateInfo::~MapRateInfo()
{

}

void MapRateInfo::Init()
{
	this->m_RateInfo.clear();
	if( this->m_RateInfo.capacity() > 0 )
	{
		std::vector<MapRateData>().swap(this->m_RateInfo);
	}
}

void MapRateInfo::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CUSTOM_MAPRATEINFO));
}

void MapRateInfo::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[MapRateInfo] File %s not found!", File);
		return;
	}
	// ----
	xml_node MapRateInfo = Document.child("maprateinfo");
	// ----
	for( xml_node Node = MapRateInfo.child("map"); Node; Node = Node.next_sibling() )
	{
		MapRateData lpInfo;
		lpInfo.MapNumber = Node.attribute("id").as_int();
		lpInfo.ExpIncrease = Node.attribute("exp").as_int();
		lpInfo.MasterExpIncrease = Node.attribute("masterexp").as_int();
		lpInfo.MoneyIncrease = Node.attribute("money").as_int();
		this->m_RateInfo.push_back(lpInfo);
	}
}

float MapRateInfo::GetExp(short MapNumber)
{
	for( int i = 0; i < this->m_RateInfo.size(); i++ )
	{
		if( this->m_RateInfo[i].MapNumber == MapNumber 
			&& this->m_RateInfo[i].ExpIncrease != 0 )
		{
			return this->m_RateInfo[i].ExpIncrease;
		}
	}
	// ----
	return gAddExperience;
}

float MapRateInfo::GetMasterExp(short MapNumber)
{
	for( int i = 0; i < this->m_RateInfo.size(); i++ )
	{
		if( this->m_RateInfo[i].MapNumber == MapNumber
			&& this->m_RateInfo[i].MasterExpIncrease != 0 )
		{
			return this->m_RateInfo[i].MasterExpIncrease;
		}
	}
	// ----
	return g_MasterExp.m_AddExp;
}

float MapRateInfo::GetMoney(short MapNumber)
{
	for( int i = 0; i < this->m_RateInfo.size(); i++ )
	{
		if( this->m_RateInfo[i].MapNumber == MapNumber
			&& this->m_RateInfo[i].MoneyIncrease != 0 )
		{
			return this->m_RateInfo[i].MoneyIncrease;
		}
	}
	// ----
	return gAddZen;
}