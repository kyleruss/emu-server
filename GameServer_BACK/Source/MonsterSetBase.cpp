#include "stdafx.h"
#include "MonsterSetBase.h"
#include "GameMain.h"
#include "MapServerManager.h"
#include "LogProc.h"
#include "LargeRand.h"
#include "Crywolf.h"
#include "IllusionTempleEvent.h"
#if( ENABLETEST_MONSTERITEMDROPEX == 1 )
#include "MonsterItemMngEx.h"
#else
#include "MonsterItemMng.h"
#endif//ENABLETEST_MONSTERITEMDROPEX
#include "Raklion.h"
#include "GameServer.h"
#include "BloodCastle.h"
#include "DevilSquare.h"
#include "DoubleGoerEvent.h"
#include "ImperialGuardian.h"
#include "ChaosCastle.h"
#include "CastleSiege.h"
#include "CrywolfAltar.h"
#include "..\include\readscript.h"
#include "..\common\WzMemScript.h"
#include "..\pugixml\pugixml.hpp"

using namespace pugi;

void CMonsterSetBase::Init()
{
	this->m_mShopNum.clear();

	for(int i = 0; i < OBJ_MAXMONSTER; i++)
		this->m_Mp[i].m_ArrangeType = (BYTE)-1;

	this->m_Count = 0;
	this->m_GoldenCount = 0;
	this->m_MonsterCount = 0;
}

int CMonsterSetBase::SetMonsterSetBase(int ArrangeType,int LoadType,int Type,int MapNumber,int Dis,int X,int Y,int Dir,int W,int H, int Element)
{
	for(int i = 0; i < OBJ_MAXMONSTER; i++)
	{
		if( this->m_Mp[i].m_ArrangeType != (BYTE)-1 )
			continue;

		this->m_Mp[i].m_ArrangeType = ArrangeType;
		this->m_Mp[i].m_LoadType = LoadType;
		this->m_Mp[i].m_Type = Type;
		this->m_Mp[i].m_MapNumber = MapNumber;
		this->m_Mp[i].m_Dis = Dis;
		this->m_Mp[i].m_X = X;
		this->m_Mp[i].m_Y = Y;
		this->m_Mp[i].m_Dir = Dir;
		this->m_Mp[i].m_W = W;
		this->m_Mp[i].m_H = H;
		this->m_Mp[i].m_PentagramMainAttribute = Element;
		return i;
	}
return -1;
}

void CMonsterSetBase::RemoveMonsterSetBase(int iPosNum)
{
	if( iPosNum < 0 || iPosNum >= OBJ_MAXMONSTER )
		return;

	if( this->m_Mp[iPosNum].m_ArrangeType != (BYTE)-1 )
	{
		this->m_Mp[iPosNum].m_ArrangeType = (BYTE)-1;
		this->m_Count--;
	}
}

void CMonsterSetBase::GetPentagramMainAttribute(int TableNum, int *iPentagramMainAttribute)
{
	if( TableNum < 0 || TableNum >= OBJ_MAXMONSTER )
		return;
	
	if( this->m_Mp[TableNum].m_PentagramMainAttribute > 0 )
	{
		*iPentagramMainAttribute = this->m_Mp[TableNum].m_PentagramMainAttribute;
	}
}

BOOL CMonsterSetBase::GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, BYTE&mx, BYTE&my)
{
	int count = 100 ; 
	int w;
	int h;
	int tx; 
	int ty;
	BYTE attr;

	while ( count-- != 0)
	{
		w = aw - ax;
		h = ah - ay;

		__try
		{
			tx = ax + (rand()%w);
			ty = ay + (rand()%h);
		}
		__except (w=1,h=1,1)
		{

		}

		attr = MapC[mapnumber].GetAttr(tx, ty);

		if ( ((attr&1) != 1) && ((attr &4) != 4) && ((attr&8) != 8) )
		{
			mx = tx;
			my = ty;
			return TRUE;
		}
	}

	return false;
}

BOOL CMonsterSetBase::GetPosition(int TableNum, short MapNumber, BYTE& x, BYTE& y)
{
	int count = 100;
	BYTE attr;
	int tx;
	int ty;
	int w;
	int h;

	if ( TableNum < 0 || TableNum > OBJ_MAXMONSTER-1 )
	{
		LogAdd("ERROR : %s %d", __FILE__, __LINE__ );
		return false;
	}

	if ( this->m_Mp[TableNum].m_ArrangeType == 1 
		|| this->m_Mp[TableNum].m_ArrangeType == 3 
		|| this->m_Mp[TableNum].m_ArrangeType == 6 )
	{
		while ( count-- != 0 )
		{
			w = this->m_Mp[TableNum].m_W - this->m_Mp[TableNum].m_X;
			h = this->m_Mp[TableNum].m_H - this->m_Mp[TableNum].m_Y;

			if ( w < 1 )
			{
				w = 1;
			}

			if ( h < 1 )
			{
				h = 1;
			}

			int iRX = GetLargeRand()%w;
			int iRY = GetLargeRand()%h;

			tx = this->m_Mp[TableNum].m_X +  iRX;
			ty = this->m_Mp[TableNum].m_Y +  iRY;
			attr = MapC[MapNumber].GetAttr(tx, ty);

			if ( ((attr&1) != 1) && ((attr &4) != 4) && ((attr&8) != 8) )
			{
				x = tx;
				y = ty;
				return TRUE;
			}
		}
	}
	else if ( this->m_Mp[TableNum].m_ArrangeType == 0 )
	{
		x = this->m_Mp[TableNum].m_X;
		y = this->m_Mp[TableNum].m_Y;
		return true;
	}
	else if ( this->m_Mp[TableNum].m_ArrangeType == 2 )
	{
		while ( count-- != 0 )
		{
			int Orx = this->m_Mp[TableNum].m_X -3;
			int Ory = this->m_Mp[TableNum].m_Y -3;

			Orx += rand() % 7;
			Ory += rand() % 7;

			attr = MapC[MapNumber].GetAttr(Orx, Ory);

			if ( ((attr&1) != 1) && ((attr &4) != 4) && ((attr&8) != 8) )
			{
				x = Orx;
				y = Ory;
				return TRUE;
			}
		}
	}
	else if ( this->m_Mp[TableNum].m_ArrangeType == 4 ||
				this->m_Mp[TableNum].m_ArrangeType == 5 )
	{
		x = this->m_Mp[TableNum].m_X;
		y = this->m_Mp[TableNum].m_Y;
		return TRUE;
	}

	return false;
}

#include "Shop.h"

void CMonsterSetBase::LoadSetBase(char* filename,int type)
{
	g_ShopManager.Init();
	xml_document doc;
	xml_parse_result result = doc.load_file(filename);

	if( result.status != status_ok )
	{
		MsgBox("file open error %s\nError description: %s", filename,result.description());
		return;
	}

	int id;
	int map;
	int dis;
	int x;
	int y;
	int w;
	int h;
	int dir;
	int count;
	int izoneindex;
	int imonstertableindex;
	int iPosNum;

	xml_node objectlist = doc.child("monstersetbase");

	/*if( type == LOAD_ALLOBJ || type == LOAD_NPCLIST )
	{
		this->m_mShopNum.clear();

		for (xml_node npclist = objectlist.child("shoplist"); npclist; npclist = npclist.next_sibling())
		{
			for( xml_node npc = npclist.child("shop"); npc; npc = npc.next_sibling()){
				id = npc.attribute("id").as_int();
				map = npc.attribute("map").as_int();
				dis = npc.attribute("dis").as_int();
				x = npc.attribute("x").as_int();
				y = npc.attribute("y").as_int();
				dir = npc.attribute("dir").as_int();
				shopid = npc.attribute("shopid").as_int();
				w = 0;
				h = 0;
				int Element = -1;

				iPosNum = this->SetMonsterSetBase(0,LOAD_NPCLIST,id,map,dis,x,y,dir,w,h, Element);

				if( iPosNum == -1 )
				{
					MsgBox("Failed to add NPC(%d) Map(%d) Dis(%d) X(%d) Y(%d)",id,map,dis,x,y);
					continue;
				}
				
				if( shopid != -1 )
					this->m_mShopNum.insert(std::pair<int,int>(iPosNum,shopid));

				this->m_NpcCount++;
			}
		}
	}*/

	if( type == LOAD_ALLOBJ || type == LOAD_ELDORADOLIST )
	{
		xml_node eldoradolist = objectlist.child("golden");
	
			for( xml_node monster = eldoradolist.child("monster"); monster; monster = monster.next_sibling())
			{
				id = monster.attribute("id").as_int();
				map = monster.attribute("map").as_int();
				dis = monster.attribute("dis").as_int();
				x = monster.attribute("x").as_int();
				y = monster.attribute("y").as_int();
				w = monster.attribute("w").as_int();
				h = monster.attribute("h").as_int();
				dir = monster.attribute("dir").as_int();
				count = monster.attribute("count").as_int();
				int Element = -1;

				for(int n = 0; n < count; n++)
				{
					iPosNum = this->SetMonsterSetBase(3,LOAD_ELDORADOLIST,id,map,dis,x,y,dir,w,h, Element);

					if( iPosNum == -1 )
					{
						MsgBox("Failed to add GoldenMonster1(%d) Map(%d) Dis(%d) X(%d) Y(%d)",id,map,dis,x,y);
						continue;
					}

					this->m_GoldenCount++;
				}
			}
		
	}

	if( type == LOAD_ALLOBJ || type == LOAD_MONSTERLIST )
	{
		for (xml_node monsterlist = objectlist.child("list"); monsterlist; monsterlist = monsterlist.next_sibling())
		{
			int attr = monsterlist.attribute("type").as_int(-1);
			xml_node monster;

			switch( attr )
			{
			case 0:
				monster = monsterlist.child("npc");
				break;
			case 1:
			case 2:
			case 4:
			case 5:
			case 6:
				monster = monsterlist.child("monster");
				break;
			default:
				printf("Unknown attr(%d)\n",attr);
				continue;
			}

			for( ; monster; monster = monster.next_sibling())
			{
				id = monster.attribute("id").as_int();
				map = monster.attribute("map").as_int();
				dis = monster.attribute("dis").as_int();
				x = monster.attribute("x").as_int();
				y = monster.attribute("y").as_int();
				dir = monster.attribute("dir").as_int();
				int Shop = -1;
				izoneindex = -1;
				imonstertableindex = -1;
				w = 0;
				h = 0;
				count = 1;
				int Element = -1;
				
				if( attr == 0 )
				{
					Shop = monster.attribute("shop").as_int(-1);
				}
				else if( attr == 1 )
				{
					w = monster.attribute("w").as_int();
					h = monster.attribute("h").as_int();
					count = monster.attribute("count").as_int();
				}
				else if( attr == 2 )
				{
					w = 0;
					h = 0;
					int tmpw = x - 3;
					int tmph = y - 3;
					tmpw += rand() % 7;
					tmph += rand() % 7;
					w = tmpw;
					h = tmph;
				}
				else if( attr == 5 )
				{
					izoneindex = monster.attribute("area").as_int();
					imonstertableindex = monster.attribute("day").as_int();
				}
				else if( attr == 6 )
				{
					Element = monster.attribute("element").as_int();
				}

				for(int n = 0; n < count; n++)
				{
					iPosNum = this->SetMonsterSetBase(attr,LOAD_MONSTERLIST,id,map,dis,x,y,dir,w,h, Element);

					if( iPosNum == -1 )
					{
						MsgBox("Failed to add GoldenMonster2(%d) Map(%d) Dis(%d) X(%d) Y(%d)",id,map,dis,x,y);
						continue;
					}

					if( Shop != -1 )
					{
						this->m_mShopNum.insert(std::pair<int,int>(iPosNum, Shop));
						g_ShopManager.Read(Shop);
					}

					this->m_Mp[iPosNum].m_wZoneIndex = izoneindex;
					this->m_Mp[iPosNum].m_wMonsterRegenTableIndex = imonstertableindex;
					this->m_MonsterCount++;
				}
			}
		}
	}

	this->m_Count = this->m_GoldenCount+this->m_MonsterCount;
	LogAdd("MonsterSetBase added - %d objects",this->m_Count);
}

void CMonsterSetBase::MonsterSetBaseReload(int type)
{
	std::map<int,int>::iterator _It;
	int iPosNum;

	if( type == LOAD_ALLOBJ)
	{
		for(int n = 0; n < OBJ_MAXMONSTER; n++)
		{
			if ( gObj[n].Type == OBJ_MONSTER || gObj[n].Type == OBJ_NPC )
			{
#if (GS_CASTLE == 1)
				if ( gObj[n].m_btCsNpcType != FALSE )
					continue;
#endif

				if ( gObj[n].m_iCurrentAI )
				{
					gObj[n].Live = FALSE;
					gObjViewportListProtocolDestroy(&gObj[n]);
					gObjViewportClose(&gObj[n]);
				}

				gObjDel(n);
			}
		}

		Init();
		this->LoadSetBase(gDirPath.GetNewPath(FILE_MONSTER_BASE), LOAD_ALLOBJ);
	}
	else if( type == LOAD_NPCLIST )
	{
		for(int n = 0; n < OBJ_MAXMONSTER; n++)
		{
			if ( gObj[n].Type == OBJ_MONSTER || gObj[n].Type == OBJ_NPC )
			{
#if (GS_CASTLE == 1)
				if ( gObj[n].m_btCsNpcType != FALSE )
					continue;
#endif

				if( gObj[n].m_PosNum == -1 )
					continue;

				iPosNum = gObj[n].m_PosNum;

				if( this->m_Mp[iPosNum].m_LoadType != type )
					continue;

				if ( gObj[n].m_iCurrentAI )
				{
					gObj[n].Live = FALSE;
					gObjViewportListProtocolDestroy(&gObj[n]);
					gObjViewportClose(&gObj[n]);
				}

				gObjDel(n);
				RemoveMonsterSetBase(iPosNum);
			}
		}

		this->LoadSetBase(gDirPath.GetNewPath(FILE_MONSTER_BASE),LOAD_NPCLIST);
	}
	else if( type == LOAD_ELDORADOLIST )
	{
		for(int n = 0; n < OBJ_MAXMONSTER; n++)
		{
			if ( gObj[n].Type == OBJ_MONSTER || gObj[n].Type == OBJ_NPC )
			{
#if (GS_CASTLE == 1)
				if ( gObj[n].m_btCsNpcType != FALSE )
					continue;
#endif

				if( gObj[n].m_PosNum == -1 )
					continue;

				iPosNum = gObj[n].m_PosNum;

				if( this->m_Mp[iPosNum].m_LoadType != type )
					continue;

				if ( gObj[n].m_iCurrentAI )
				{
					gObj[n].Live = FALSE;
					gObjViewportListProtocolDestroy(&gObj[n]);
					gObjViewportClose(&gObj[n]);
				}

				gObjDel(n);
				RemoveMonsterSetBase(iPosNum);
			}
		}

		this->m_GoldenCount = 0;
		this->LoadSetBase(gDirPath.GetNewPath(FILE_MONSTER_BASE),LOAD_ELDORADOLIST);
	}
	else if( type == LOAD_MONSTERLIST )
	{
		for(int n = 0; n < OBJ_MAXMONSTER; n++)
		{
			if ( gObj[n].Type == OBJ_MONSTER || gObj[n].Type == OBJ_NPC )
			{
#if (GS_CASTLE == 1)
				if ( gObj[n].m_btCsNpcType != FALSE )
					continue;
#endif

				if( gObj[n].m_PosNum == -1 )
					continue;

				iPosNum = gObj[n].m_PosNum;
				
				if( this->m_Mp[iPosNum].m_LoadType != type )
					continue;

				if ( gObj[n].m_iCurrentAI )
				{
					gObj[n].Live = FALSE;
					gObjViewportListProtocolDestroy(&gObj[n]);
					gObjViewportClose(&gObj[n]);
				}

				gObjDel(n);
				RemoveMonsterSetBase(iPosNum);
			}
		}

		this->m_MonsterCount = 0;
		this->LoadSetBase(gDirPath.GetNewPath(FILE_MONSTER_BASE), LOAD_MONSTERLIST);
	}

#if (GS_CASTLE == 1)

	if( type == LOAD_ALLOBJ || 
		type == LOAD_NPCLIST )
	{
		g_Crywolf.m_ObjCommonNPC.Reset();
		g_Crywolf.m_ObjSpecialNPC.Reset();
	}

	if( type == LOAD_ALLOBJ ||
		type == LOAD_MONSTERLIST )
	{
		g_Crywolf.m_ObjCommonMonster.Reset();
		g_Crywolf.m_ObjSpecialMonster.Reset();
	}

#endif

	if( type == LOAD_ALLOBJ )
	{
		gMAttr.LoadAttr(gDirPath.GetNewPath(FILE_MONSTER_MAIN));

#if( ENABLETEST_MONSTERITEMDROPEX == 1 )
		g_MonsterItemMngEx.Init();
#else
		g_MonsterItemMng.Init();
#endif//ENABLETEST_MONSTERITEMDROPEX

		gObjMonCount = 0;
	}

	g_IllusionTemple.ResetObject();
	AddAllMonsters(type);
}

void CMonsterSetBase::AddAllMonsters(int type)
{
	gCurPaintPlayer = 0;

	int result;

	for ( int n=0;n< OBJ_MAXMONSTER;n++)
	{
		if ( this->m_Mp[n].m_ArrangeType == (BYTE)-1 )
			continue;

		if ( type != LOAD_ALLOBJ && type != this->m_Mp[n].m_LoadType )
			continue;


		if ( DS_MAP_RANGE(this->m_Mp[n].m_MapNumber) != FALSE )
		{
			continue;
		}

		if ( BC_MAP_RANGE(this->m_Mp[n].m_MapNumber) != FALSE )
		{
			if ( this->m_Mp[n].m_Type != 232 )
			{
				continue;
			}
		}

		if (DG_MAP_RANGE(this->m_Mp[n].m_MapNumber) != FALSE )
		{
			continue;
		}

		if ( CC_MAP_RANGE(this->m_Mp[n].m_MapNumber) != FALSE )
		{
			continue;
		}

		if ( IF_MAP_RANGE(this->m_Mp[n].m_MapNumber) != FALSE )
		{
			continue;
		}

		if ( IT_MAP_RANGE(this->m_Mp[n].m_MapNumber) != FALSE )
		{
			if( this->m_Mp[n].m_Type != 381 && this->m_Mp[n].m_Type != 382 )
			{
				if( this->m_Mp[n].m_Type == 380 ||
					this->m_Mp[n].m_Type == 383 ||
					this->m_Mp[n].m_Type == 384 )
				{
					g_IllusionTemple.AddNpc(this->m_Mp[n].m_MapNumber,this->m_Mp[n].m_Type,n);
				}
				else if( this->m_Mp[n].m_Type >= 386 ||
						 this->m_Mp[n].m_Type <= 403 )
				{
					g_IllusionTemple.AddMonster(this->m_Mp[n].m_MapNumber,this->m_Mp[n].m_Type,n);
				}
				continue;
			}
		}

		//if( this->m_Mp[n].m_MapNumber == MAP_INDEX_RAKLIONBOSS && (this->m_Mp[n].m_Type == 459 || this->m_Mp[n].m_Type == 457 || this->m_Mp[n].m_Type == 458 ))
			//continue;

		result = gObjAddMonster(this->m_Mp[n].m_MapNumber);

		if ( result >= 0 )
		{
			gObjSetPosMonster(result, n);
			gObjSetMonster(result, this->m_Mp[n].m_Type);
			gCurPaintPlayer++;

			if ( BC_MAP_RANGE(gObj[result].MapNumber) )
			{
				if ( gObj[result].Class == 232 )
				{
					gObj[result].m_cBloodCastleIndex = g_BloodCastle.GetBridgeLevel(gObj[result].MapNumber);
				}
			}

#if (GS_CASTLE == 1)
			if( gObj[result].Class == 216 )
			{
				g_CastleSiege.SetCrownIndex(result);
			}

			if( ((gObj[result].MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE) ? TRUE : FALSE) && gObj[result].Type == OBJ_NPC)
			{
				if( gObj[result].Class == 406 || gObj[result].Class == 407 )
				{

				}
				else if(CRYWOLF_NPC_CLASS_RANGE(gObj[result].Class) != FALSE)
				{
					g_Crywolf.m_ObjSpecialNPC.AddObj(result);
				}
				else
				{
					g_Crywolf.m_ObjCommonNPC.AddObj(result);
				}
			}

			if( ((gObj[result].MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE) ? TRUE : FALSE) && gObj[result].Type == OBJ_MONSTER)
			{
				g_Crywolf.m_ObjCommonMonster.AddObj(result);
			}
#endif
		}
	}

#if (GS_CASTLE == 1 && GS_SPEC == 0)
	gDevilSquareEvent = FALSE;
	g_bChaosCastle  = FALSE;
#endif

	g_DevilSquare.Init();
	g_BloodCastle.LoadItemDropRate();

#if (GS_CASTLE == 1 && GS_SPEC == 0)
	g_bBloodCastle = FALSE;
#endif

	if ( g_bBloodCastle != FALSE )
	{
		g_BloodCastle.Init(TRUE);
	}
	else
	{
		g_BloodCastle.Init(FALSE);
	}

	if ( g_bChaosCastle != FALSE )
	{
		g_ChaosCastle.Init(true);
	}
	else
	{
		g_ChaosCastle.Init(false);
	}

	if( g_bIllusionTemple )
	{
		g_IllusionTemple.Init();
	}

	g_Raklion.SetState(RAKLION_STATE_END);
}


int CMonsterSetBase::GetShopID(int posnum)
{
	if( posnum != -1 )
	{
		std::map<int,int>::iterator _It = this->m_mShopNum.find(posnum);

		if( _It != this->m_mShopNum.end() )
			return _It->second;
	}
return -1;
}