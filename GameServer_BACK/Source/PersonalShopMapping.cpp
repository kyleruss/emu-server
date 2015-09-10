#include "stdafx.h"
#include "PersonalShopMapping.h"
#include "dirpath.h"
#include "..\pugixml\pugixml.hpp"

using namespace pugi;
PersonalShopMapping pShop;


PersonalShopMapping::PersonalShopMapping()
{
	this->Load();
}

PersonalShopMapping::~PersonalShopMapping()
{

}

void PersonalShopMapping::Init()
{
	this->maps.clear();
	this->allmaps	=	true;
	this->enabled	=	true;
}

void PersonalShopMapping::Load()
{
	this->Init();
	CDirPath path;
	path.SetFirstPath(FOLDER_DATA);

	this->Read(path.GetNewPath(FILE_CUSTOM_PERSONALSHOPMAPPING));
}

void PersonalShopMapping::Read(LPSTR file)
{
	xml_document doc;
	xml_parse_result doc_result	=	doc.load_file(file);

	if(doc_result.status != status_ok)
	{
		MsgBox("[PersonalShopMapping] file %s not found", file);
		return;
	}

	xml_node pstore		=	doc.child("pstore");
	this->enabled		=	pstore.child("enabled").text().as_int(0);
	this->allmaps		=	pstore.child("allmaps").text().as_int(0);

	xml_node maplist	=	pstore.child("maplist");
	xml_node node;

	for(node = maplist.child("map"); node; node = node.next_sibling())
	{
		PersonalShopMap current;

		current.enabled		=	node.attribute("enabled").as_int(1);
		current.id			=	(node.attribute("id").as_int(1) & 0xFF);
		current.name[0]		=	(char) node.text().as_string();

		maps.push_back(current);
	}
}

bool PersonalShopMapping::CanOpenShopAtMap(BYTE map)
{
	if(!this->enabled || (!this->allmaps && this->maps.empty())) 
		return false;
	else if(this->allmaps && this->enabled)
		return true;
	else
	{
		PersonalShopMap* found	=	this->FindShopAtMap(map);
		return found != NULL;
	}
}

PersonalShopMap* PersonalShopMapping::FindShopAtMap(BYTE map)
{
	BYTE map_b	=	map & 0xFF;
	LogAddC(2, "map byte: %d", map_b);

	for(std::vector<PersonalShopMap>::iterator iter = maps.begin(); iter != maps.end(); iter++)
	{
		PersonalShopMap current	=	*iter;
		if(current.id == map_b) return &current;
	}

	return NULL;
}

bool PersonalShopMapping::isEnabled()
{
	return this->enabled;
}

bool PersonalShopMapping::isAllmaps()
{
	return this->allmaps;
}

