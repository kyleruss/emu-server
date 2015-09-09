#include "stdafx.h"
#include "MonsterMoneyDrop.h"
#include "..\pugixml\pugixml.hpp"
#include "GameMain.h"
#include "DevilSquare.h"
#include "MapRateInfo.h"
#include "gObjMonster.h"
#include <math.h>

using namespace pugi;
MonsterMoneyDrop g_MonsterMoneyDrop;

MonsterMoneyDrop::MonsterMoneyDrop()
{

}

MonsterMoneyDrop::~MonsterMoneyDrop()
{

}

void MonsterMoneyDrop::Init()
{
	this->m_DropInfo.clear();
	if( this->m_DropInfo.capacity() > 0 )
	{
		std::vector<MonsterMoneyDropInfo>().swap(this->m_DropInfo);
	}
	this->m_Enabled = false;
}

void MonsterMoneyDrop::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_MONSTER_MONEYDROP));
}

void MonsterMoneyDrop::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[MonsterMoneyDrop] File %s not found! %d", File, Result.status);
		return;
	}
	// ----
	xml_node MonsterMoneyDrop = Document.child("monstermoneydrop");
	this->m_Enabled = MonsterMoneyDrop.child("enabled").text().as_int();
	// ----
	for( xml_node Node = MonsterMoneyDrop.child("monsterlist").child("monster"); Node; Node = Node.next_sibling() )
	{
		MonsterMoneyDropInfo lpInfo;
		lpInfo.MonsterID = Node.attribute("id").as_int();
		lpInfo.MonsterMinLevel = Node.attribute("levelmin").as_int();
		lpInfo.MonsterMaxLevel = Node.attribute("levelmax").as_int();
		lpInfo.MonsterMapNumber = Node.attribute("map").as_int();
		lpInfo.MoneyMin = Node.attribute("moneymin").as_int(-1);
		lpInfo.MoneyMax = Node.attribute("moneymax").as_int(-1);
		lpInfo.Rate = Node.attribute("rate").as_int();
		this->m_DropInfo.push_back(lpInfo);
	}
}

int MonsterMoneyDrop::Run(LPOBJ lpUser, LPOBJ lpMonster)
{
	if( !this->m_Enabled )
	{
		return 0;
	}

	MonsterMoneyDropInfo* DropInfo = NULL;

	for( int i = 0; i < this->m_DropInfo.size(); i++ )
	{
		if( this->m_DropInfo[i].MonsterID != -1 )
		{
			if( this->m_DropInfo[i].MonsterID != lpMonster->Class )
			{
				continue;
			}
		}
		
		if( this->m_DropInfo[i].MonsterMinLevel != -1 )
		{
			if( lpMonster->Level < this->m_DropInfo[i].MonsterMinLevel )
			{
				continue;
			}
		}

		if( this->m_DropInfo[i].MonsterMaxLevel != -1 )
		{
			if( lpMonster->Level > this->m_DropInfo[i].MonsterMaxLevel )
			{
				continue;
			}
		}

		if( this->m_DropInfo[i].MonsterMapNumber != -1 )
		{
			if( lpMonster->MapNumber != this->m_DropInfo[i].MonsterMapNumber )
			{
				continue;
			}
		}

		DropInfo = &this->m_DropInfo[i];
		break;
	}

	if( DropInfo == NULL )
	{
		return 0;
	}

	DWORD RandomRoll = rand() % 10000;
	
	if (DropInfo->MoneyMin == -1 && DropInfo->MoneyMax == -1) 
	{
		int x = lpMonster->X;
		int y = lpMonster->Y;
		float money = (float)lpMonster->Money;
		money /= 6.0f;
		money += (money/100.0f)*lpUser->MonsterDieGetMoney;
		money *= g_MapRateInfo.GetMoney(lpUser->MapNumber);
		money *= std::ceil(lpMonster->Level / 10.0f);

		int AddZenPerc = 100;

		if( lpUser->m_Change == 503 ) {
			AddZenPerc += 50;
		}

		if( gObjUnicornSprite(lpUser) == TRUE ) {
			AddZenPerc += 50;
		}

		if( AddZenPerc != 100 ) {
			money = money* (AddZenPerc / 100);
		}

		if (m_ObjBill[lpUser->m_Index].GetZen() > 0) {
			money += money * m_ObjBill[lpUser->m_Index].GetZen() / 100;
		}

		if( money < 1.0f ) {
			money = 1.0f;
		}
		
		if( DropInfo->Rate > RandomRoll ) {
			if ( DS_MAP_RANGE(lpUser->MapNumber) ) {
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpMonster);

				if( MaxHitUser == -1 )
					return 0;

				gObj[MaxHitUser].m_nEventMoney += (int)money;
			}
			else {
				MapC[lpMonster->MapNumber].MoneyItemDrop((int)money, x, y);
			}
			return 1;
		}
	}
	else {
		if( DropInfo->MoneyMax < 0 ) {
			return 0;
		}

		short mlevel	=	lpMonster->Level;
		int scaledMin	=	std::ceil(mlevel / 10.0f);
		DWORD MoneyAmount = DropInfo->MoneyMin + (rand() % (DWORD)(DropInfo->MoneyMax - DropInfo->MoneyMin + 1));
		MoneyAmount *= std::ceil(mlevel / 10.0f);

		if( MoneyAmount > DropInfo->MoneyMax ) {
			MoneyAmount = DropInfo->MoneyMax - (rand() % 100);
		}

		if (m_ObjBill[lpUser->m_Index].GetZen() > 0) {
			MoneyAmount += MoneyAmount * (m_ObjBill[lpUser->m_Index].GetZen() / 100);
		}

		//LogAddDebug("Zen increase: %d", m_ObjBill[lpUser->m_Index].GetZen());

		LogAddC(3, "money amount: %d, random roll: %d, scaled min: %d", MoneyAmount, RandomRoll, scaledMin);
		if( DropInfo->Rate > RandomRoll ) {
			MapC[lpMonster->MapNumber].MoneyItemDrop(MoneyAmount, lpMonster->X, lpMonster->Y);
			return 1;
		}
	}

	return 2;
}