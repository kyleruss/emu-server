#include "stdafx.h"
#include "GameServer.h"
#include "logproc.h"
#include "GameMain.h"
#include "MapServerManager.h"
#include "gObjMonster.h"
#include "ObjCalCharacter.h"
#include "ObjUseSkill.h"
#include "DarkSpirit.h"
#include "DSProtocol.h"
#include "DevilSquare.h"
#include "protocol.h"
#include "BattleGround.h"
#include "MonsterAttr.h"
#include "ChaosBox.h"
#include "PacketChecksum.h"
#include "QuestInfo.h"
#include "EledoradoEvent.h"
#include "TNotice.h"
#include "Notice.h"
#include "zzzmathlib.h"
#include "Gate.h"
#include "ObjAttack.h"
#include "SProtocol.h"
#include "MultiAttackHAckCheck.h"
#include "BattleSoccer.h"
#include "BattleSoccerManager.h"
#include "TUnion.h"
#include "TMonsterAIElement.h"
#include "..\common\winutil.h"
#include "Weapon.h"
#include "CrywolfSync.h"
#include "Kanturu.h"
#include "CashShop.h"
#include "Mercenary.h"
#include "KalimaGate.h"
#include "KanturuBattleUserMng.h"
#include "Guardian.h"
#include "SProtocol.h"
#include "MoveCommand.h"
#include "ItemAddOption.h"
#include "StatMng.h"
#include "..\common\SetItemOption.h"
#include "SkillAdditionInfo.h"
#include "CashItemPeriodSystem.h"
#include "TMonsterSkillManager.h"
#include "user.h"
#include "CastleSiege.h"
#include "Crywolf.h"
#include "CrywolfAltar.h"
#include "GuardianStatue.h"
#include "LifeStone.h"
#include "CannonTower.h"
#include "CastleCrown.h"
#include "CastleCrownSwitch.h"
#include "IllusionTempleEvent.h"
#include "Event.h"
#include "CastleSiegeSync.h"
#include "BuffEffectSlot.h"
#include "MasterSkillSystem.h"
#include "SocketItemOption.h"
#include "Raklion.h"
#include "RaklionUtil.h"
#include "RaklionBattleUserMng.h"
#include "PCBangPointSystem.h"
#include "GameEvent.h"
#include "DoubleGoerEvent.h"
#include "ImperialGuardian.h"
#include "ElementalSystem.h"
#include "MasterLevelSkillTreeSystem.h"
#include "TemporaryUserManager.h"
#include "MiningSystem.h"
#include "PersonalShopMapping.h"
#include "MiniMap.h"
#include "ClassCalc.h"
#ifdef __NOVUS__
#include "SummonSpot.h"
#endif
#include "..\pugixml\pugixml.hpp"
#include "MapRateInfo.h"
#include "ShopPointEx.h"

#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
#include "OfflineTrade.h"
#endif

#if( ENABLE_CUSTOM_HARDCORE == 1 )
#include "HardcoreMode.h"
#endif

#if( ENABLETEST_CHEATGUARD == 1 )
#include "CheatGuard.h"
#endif

#if (ENABLETEST_ZTLUA == 1)
#include "ZTLuaManager.h"
#endif

#if (ENABLETEST_ARCA == 1)
#include "ArcaBattle.h"
#endif

#if (ENABLETEST_ACHERON == 1)
#include "AcheronEvent.h"
#endif

#if (ENABLETEST_MUUN == 1)
#include "MuunSystem.h"
#endif

#if (ENABLETEST_NEWPVP == 1)
#include "NewPVP.h"
#endif

#if (ENABLE_CUSTOM_MONSTERSPAWNER == 1)
#include "MonsterSpawner.h"
#endif

using namespace pugi;
int ChangeCount; 
int lOfsChange;
int GuildUserCount;
int GuildUserOfs;
int GuildInfoCount;
int GuildInfoOfs;

#if(__GENS_SYSTEM__ == 1)
int GensCount;
int lOfsGens;
#endif

int skillSuccess;
int gCurConnectUser;
int gDisconnect;
int gObjTotalUser;
int gItemLoopMax;
int gItemLoop;
int gItemLoop2;
int gObjCSFlag;

#include "ChaosCastle.h"
#include "..\include\readscript.h"

CViewportGuild ViewGuildMng;	// line : 130

LPOBJ MainObj;
LPOBJ gObj;

int gObjCount;
int gObjMonCount;
int gObjCallMonCount;

MessageStateMachine gSMMsg[OBJMAX][MAX_MONSTER_SEND_MSG];	
ExMessageStateMachine gSMAttackProcMsg[OBJMAX][MAX_MONSTER_SEND_ATTACK_MSG];


BILL_CLASS m_ObjBill[OBJMAX];	// line : 193



HANDLE hThread_gObjMove;

struct PWMSG_COUNT {
	PWMSG_HEAD h;
	BYTE count;
};

struct PBMSG_COUNT {
  PBMSG_HEAD h;
  BYTE count;
};

struct PMSG_ITEMVIEWPORTCREATE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE px;
	BYTE py;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_VIEWPORTCREATE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE X;
	BYTE Y;
	BYTE CharSet[18];
	char Id[10];
	BYTE TX;
	BYTE TY;
	BYTE DirAndPkLevel;
	BYTE btElement;
#if (ENABLETEST_SEASON8DOT3 == 1)
	BYTE MuunItemH;
    BYTE MuunItemL;
    BYTE LevelH;
    BYTE LevelL;
#endif
#if (ENABLETEST_SEASON8DOT3 == 0)
	BYTE HPNumberH;
	BYTE HPNumberL;
#endif
	BYTE MAXHP_HIGHH;
	BYTE MAXHP_LOWH;
	BYTE MAXHP_HIGHL;
	BYTE MAXHP_LOWL;
	BYTE HP_HIGHH;
	BYTE HP_LOWH;
	BYTE HP_HIGHL;
	BYTE HP_LOWL;
	BYTE EffectCount;
	BYTE EffectList[MAX_EFFECT_INFO];
};

struct PMSG_VIEWPORTCREATE_CHANGE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE X;
	BYTE Y;
	BYTE SkinH;
	BYTE SkinL;
	char Id[10];
	BYTE TX;
	BYTE TY;
	BYTE DirAndPkLevel;
	BYTE CharSet[18];
	BYTE btElement;
#if (ENABLETEST_SEASON8DOT3 == 1)
	BYTE MuunItemH;
    BYTE MuunItemL;
    BYTE LevelH;
    BYTE LevelL;
#endif
#if (ENABLETEST_SEASON8DOT3 == 0)
	BYTE HPNumberH;
	BYTE HPNumberL;
#endif
	BYTE MAXHP_HIGHH;
	BYTE MAXHP_LOWH;
	BYTE MAXHP_HIGHL;
	BYTE MAXHP_LOWL;
	BYTE HP_HIGHH;
	BYTE HP_LOWH;
	BYTE HP_HIGHL;
	BYTE HP_LOWL;
	BYTE EffectCount;
	BYTE EffectList[MAX_EFFECT_INFO];
};

struct PMSG_VIEWPORTDESTROY {
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_MONSTER_VIEWPORTCREATE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE Type_HI;
	BYTE Type_LO;
	BYTE X;
	BYTE Y;
	BYTE TX;
	BYTE TY;
	BYTE Path;
	BYTE btElement;
#if (ENABLETEST_SEASON8DOT3 == 1)
	BYTE LevelH;
    BYTE LevelL;
#endif
#if (ENABLETEST_SEASON8DOT3 == 0)
	BYTE HPNumberH;
	BYTE HPNumberL;
#endif
	BYTE MAXHP_HIGHH;
	BYTE MAXHP_LOWH;
	BYTE MAXHP_HIGHL;
	BYTE MAXHP_LOWL;
	BYTE HP_HIGHH;
	BYTE HP_LOWH;
	BYTE HP_HIGHL;
	BYTE HP_LOWL;
	BYTE EffectCount;
	BYTE EffectList[MAX_EFFECT_INFO];
};

struct PMSG_CALLMONSTER_VIEWPORTCREATE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE Type_HI;
	BYTE Type_LO;
	BYTE X;
	BYTE Y;
	BYTE TX;
	BYTE TY;
	BYTE Path;
	BYTE Id[10];
	BYTE btElement;
#if (ENABLETEST_SEASON8DOT3 == 1)
	BYTE LevelH;
    BYTE LevelL;
#endif
#if (ENABLETEST_SEASON8DOT3 == 0)
	BYTE HPNumberH;
	BYTE HPNumberL;
#endif
	BYTE MAXHP_HIGHH;
	BYTE MAXHP_LOWH;
	BYTE MAXHP_HIGHL;
	BYTE MAXHP_LOWL;
	BYTE HP_HIGHH;
	BYTE HP_LOWH;
	BYTE HP_HIGHL;
	BYTE HP_LOWL;
	BYTE EffectCount;
	BYTE EffectList[MAX_EFFECT_INFO];
};

struct PMSG_GUILDVIEWPORT_USER {
	BYTE NumberH;
	BYTE NumberL;
	BYTE GNumberH;
	BYTE GNumberL;
};

struct PMSG_GUILDVIEWPORT {
	BYTE NumberH;
	BYTE NumberL;
	char GuildName[8];
	BYTE Mark[32];
};

#if (__GENS_SYSTEM__ == 1)

struct PMSG_GENS_VIEWPORT
{
  PWMSG_HEAD h;
  BYTE btSubCode;
  BYTE btCount;
};

struct PMSG_GENS_VIEWPORTCREATE
{
  BYTE btFamily;
  BYTE IndexHI;
  BYTE IndexLO;
  int Rank;
  int Level;
  int Contrib;
};


PMSG_GENS_VIEWPORTCREATE	pGensViewportCreate;

#endif

BYTE GuildInfoBuf[10000];
BYTE GuildUserBuf[10000];
BYTE SendGBufChange[5000];

PMSG_VIEWPORTCREATE pViewportCreate;
PMSG_VIEWPORTCREATE_CHANGE pViewportCreateChange;
PMSG_VIEWPORTDESTROY pViewportDestroy;
PMSG_MONSTER_VIEWPORTCREATE pMonsterViewportCreate;
PMSG_CALLMONSTER_VIEWPORTCREATE pCallMonsterViewportCreate;

#pragma pack(1)

PWMSG_COUNT pItemCount;
PMSG_ITEMVIEWPORTCREATE pItemViewportCreate;
PMSG_VIEWPORTDESTROY pItemViewportDestroy;

#pragma pack()



short RoadPathTable[MAX_ROAD_PATH_TABLE] = { -1, -1, 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0 };	// THIS VARIABLE HAS SOME STRANGE THINGS ONT ITS UBICATION
int  gServerMaxUser=500;	// THIS VARIABLE HAS SOME STRANGE THINGS ONT ITS UBICATION
BOOL g_EnergyCheckOff=1; // THIS VARIABLE HAS SOME STRANGE THINGS ONT ITS UBICATION



struct PMSG_EX_SKILL_COUNT {
  PBMSG_HEAD h;	// C1:BA
  BYTE NumberH;
  BYTE NumberL;
  WORD Type;
  BYTE Count;
};



void gObjSkillUseProcTime500(LPOBJ lpObj)
{
	CMagicInf* lpMagic;

	if ( lpObj->SkillHellFire2State != 0 )
	{
		unsigned long dwCurrentTick = GetTickCount();
		lpObj->SkillHellFire2Count = (BYTE)((dwCurrentTick - lpObj->SkillHellFire2Time) / 500);

		if ( lpObj->SkillHellFire2Count > 12 )
		{
			lpObj->SkillHellFire2Count = 12;
		}

		lpMagic = gObjGetMagicSearch(lpObj,AT_MSKILL_DW_NOVA1);

		if( lpMagic == NULL )
		{
			lpMagic = gObjGetMagicSearch(lpObj,AT_SKILL_EXPHELL);
		}

		if(dwCurrentTick >= (lpObj->SkillHellFire2Time + 6000))
		{
			if(lpMagic != 0)
			{
				if( lpMagic->m_Skill == AT_SKILL_EXPHELL )
					gObjUseSkill.RunningSkill(lpObj->m_Index,0,lpMagic,0);
				else
					g_MasterSkillSystem.RunningSkill_MLS(lpObj,0,lpMagic,0,0,0,0,0);
			}
		}
		else
		{
			int mana = MagicDamageC.SkillGetMana(40);
			mana = mana * 20 / 100;
			mana -= mana*lpObj->m_MPSkillOpt.MpsDecreaseMana/100.0f;

			if( mana < 1 )
				mana = 1;

			BOOL bSuccess = TRUE;

			if ( mana > lpObj->Mana )
			{
				bSuccess = FALSE;
				int count = 0;

				while ( true )
				{
					count++;

					if ( gObjIsConnected(lpObj) == FALSE )
					{
						LogAdd("★[CHECK_LOG_INFINITY] gObjIsConnected() error %s %d", __FILE__, __LINE__);
						break;
					}

					if ( count > 100 )
					{
						LogAdd("★[CHECK_LOG_INFINITY] ( _count > 100 ) error %s %d", __FILE__, __LINE__);
						break;
					}

					int pos = gObjGetManaItemPos(lpObj->m_Index);

					if ( pos == -1 )
					{
						break;
					}

					PMSG_USEITEM pMsg;
					pMsg.inventoryPos = pos;
					pMsg.invenrotyTarget = 0;
					CGUseItemRecv(&pMsg, lpObj->m_Index);

					if ( mana <= lpObj->Mana )
					{
						bSuccess = TRUE;
						break;
					}
				}
			}
			
			if ( bSuccess == FALSE )
			{	
				if (lpMagic != NULL )
				{
					gObjUseSkill.RunningSkill(lpObj->m_Index, 0, lpMagic, 0);
				}
			}
			else
			{
				lpObj->Mana -= mana;

				if ( lpObj->Mana < 0.0f )
				{
					lpObj->Mana = 0.0f;
				}

				GCManaSend(lpObj->m_Index,(short)lpObj->Mana, -1, 0, lpObj->BP);

				if( lpMagic )
				{
					PMSG_EX_SKILL_COUNT pMsg;
					PHeadSetB( (LPBYTE)&pMsg, 0xBA, sizeof(pMsg));
					pMsg.Type = lpMagic->m_Skill;
					pMsg.Count = lpObj->SkillHellFire2Count;
					pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
					pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);

					DataSend(lpObj->m_Index, (UCHAR *)&pMsg, pMsg.h.size);
					MsgSendV2(lpObj, (UCHAR *)&pMsg, pMsg.h.size);
				}
			}
		}
	}
}


void MonsterAndMsgProc()
{
	LPOBJ lpObj;
	int n;
	int aIndex;

	for ( n=0;n<OBJMAX;n++)
	{
		lpObj = &gObj[n];

		if ( lpObj->Connected == PLAYER_PLAYING  )
		{
			if ( lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC) 
			{
				if(lpObj->m_iCurrentAI != 0)
				{
					continue;
				}
				gObjMonsterProcess(lpObj);
			}
			else
			{
				gObjSkillUseProcTime500(lpObj);
				gObjMsgProc(lpObj);
				CreateFrustrum(lpObj->X, lpObj->Y, n);
			}

			if ( lpObj->Type == OBJ_USER )
			{
				gDarkSpirit[n].Run();
			}
		}
		else if ( lpObj->Connected >= PLAYER_LOGGED )
		{
			if ( lpObj->Type == OBJ_USER )
			{
				gObjMsgProc(lpObj);

				if ( lpObj->Connected == PLAYER_PLAYING )
				{
					CreateFrustrum(lpObj->X, lpObj->Y, n);
				}
			}
		}
	}

	for ( n=0;n<OBJMAXUSER;n++)
	{
		lpObj = &gObj[n+OBJ_STARTUSERINDEX];

		if ( lpObj->Connected == PLAYER_PLAYING  )
		{
			aIndex = lpObj->m_Index;

			for ( int i=0;i<MAX_MONSTER_SEND_ATTACK_MSG;i++)
			{
				if ( gSMAttackProcMsg[n][i].MsgCode >= 0 )
				{
					if ( GetTickCount() > gSMAttackProcMsg[n][i].MsgTime )
					{
						gObjStateAttackProc(lpObj, gSMAttackProcMsg[n][i].MsgCode, gSMAttackProcMsg[n][i].SendUser,
							gSMAttackProcMsg[n][i].SubCode, gSMAttackProcMsg[n][i].SubCode2);
						gSMAttackProcMsg[n][i].MsgCode = -1;
					}
				}
			}
		}
	}
	TMonsterSkillManager::MonsterSkillProc();
}

void MoveMonsterProc()
{
	DWORD MoveTime;
	int DelayTime;
	LPOBJ lpObj;

	for(int n = 0; n < OBJMAX;n++)
	{
		lpObj = &gObj[n];

		if(lpObj->m_iCurrentAI != 0)
		{
			if(lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
			{
				continue;
			}
		}

		if(lpObj->Connected == PLAYER_PLAYING)
		{
			if(lpObj->Type == OBJ_MONSTER)
			{
#if (GS_CASTLE == 1)
				if(ATTRIBUTE_RANGE(lpObj->m_Attribute) != FALSE
					|| lpObj->Class == 277
					|| lpObj->Class == 283
					|| lpObj->Class == 288
					|| lpObj->Class == 278
					|| lpObj->Class == 216
					|| lpObj->Class == 217
					|| lpObj->Class == 218
					|| lpObj->Class == 219
					|| (CRYWOLF_ALTAR_CLASS_RANGE(lpObj->Class)
					|| ((lpObj->Class == 204) ? TRUE : FALSE)))
				{
					CreateFrustrum(lpObj->X,lpObj->Y,n);
					continue;
				}
#endif
			}
			
			if(lpObj->m_State == 2)
			{
				if( lpObj->PathCount != 0 &&
					gObjCheckUsedBuffEffect(lpObj,BUFF_ICEARROW) == FALSE &&
					gObjCheckUsedBuffEffect(lpObj,BUFF_STUN) == FALSE &&
					gObjCheckUsedBuffEffect(lpObj,BUFF_SLEEP) == FALSE &&
					gObjCheckUsedBuffEffect(lpObj,BUFF_EARTHBINDS) == FALSE &&
					gObjCheckUsedBuffEffect(lpObj,BUFF_FREEZE) == FALSE)
				{
					if(lpObj->DelayLevel != 0)
					{
						DelayTime = 300;
					}
					else
					{
						DelayTime = 0;
					}
	
					if(lpObj->Type == OBJ_MONSTER && lpObj->m_RecallMon >= 100)
					{
						lpObj->m_MoveSpeed = 200;
					}
					else
					{
						lpObj->m_MoveSpeed = 400;
					}
	
					if(lpObj->PathDir[lpObj->PathCur]%2 == 0)
					{
						MoveTime = (DWORD)((lpObj->m_MoveSpeed + DelayTime)*1.3);
					}
					else
					{
						MoveTime = lpObj->m_MoveSpeed + DelayTime;
					}
	
					if( (GetTickCount() - lpObj->PathTime) > MoveTime && lpObj->PathCur < 14)
					{
						int nextX = lpObj->PathX[lpObj->PathCur];
						int nextY = lpObj->PathY[lpObj->PathCur];
						BYTE mapnumber = lpObj->MapNumber;
						BYTE attr = MapC[mapnumber].GetAttr(nextX,nextY);
	
						if( lpObj->Type == OBJ_USER &&
							( (attr & 4) == 4 || (attr & 8) == 8) )
						{
							LogAddTD("[ CHECK POSITION ] MoveMosterProc [%s][%s] Map[%d]-(%d,%d) User(%d,%d) Can not Move Position Attr[%d]",
								lpObj->AccountID,lpObj->Name,lpObj->MapNumber,nextX,nextY,lpObj->X,lpObj->Y,attr);
							for(n = 0; n < 15;n++)
							{
								lpObj->PathX[n] = 0;
								lpObj->PathY[n] = 0;
								lpObj->PathOri[n] = 0;
							}
	
							lpObj->PathCount = 0;
							lpObj->PathCur = 0;
							lpObj->PathTime = GetTickCount();

							if(lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
							{
								lpObj->PathStartEnd = 0;
							}
	
							if(lpObj->Type == OBJ_USER)
							{
								gObjSetPosition(lpObj->m_Index,lpObj->X,lpObj->Y);
							}
						}
						else
						{
							lpObj->X = lpObj->PathX[lpObj->PathCur];
							lpObj->Y = lpObj->PathY[lpObj->PathCur];
							lpObj->Dir = lpObj->PathDir[lpObj->PathCur];

							lpObj->PathTime = GetTickCount();
							lpObj->PathCur++;

							if(lpObj->PathCur >= lpObj->PathCount)
							{
								lpObj->PathCur = 0;
								lpObj->PathCount = 0;

								if(lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
								{
									lpObj->PathStartEnd = 0;
								}
							}
						}
					}
				}
				CreateFrustrum(lpObj->X,lpObj->Y,n);
			}
		}
	}
}



void gObjInit()
{
	//LogAddDebug("[OBJECTSTRUCT] Size: %d, Monsters: %d, Players: %d", 
	//	OBJMAX, OBJ_MAXMONSTER, OBJMAXUSER);

	gObjCount = OBJ_STARTUSERINDEX;
	gObjMonCount = 0;
	gObjCallMonCount = OBJ_MAXMONSTER;

	CItem* pTempInventory = new CItem[INVENTORY_SIZE];
	BYTE* pTempInventoryMap = new BYTE [INVENTORY_MAP_SIZE];
	ERTEL_USER* pTempErtel = new ERTEL_USER[100];
	ELEMENTAL_OPTIONS* pTmpElement = new ELEMENTAL_OPTIONS;

	for ( int n = 0; n < OBJMAX ; n++)
	{
		memset(&gObj[n], 0, sizeof(OBJECTSTRUCT));
		gObj[n].Type = (BYTE)OBJ_EMPTY;
		gObj[n].m_socket = INVALID_SOCKET;
		gObj[n].PathTime = GetTickCount();
		gObj[n].m_RecallMon = -1;
		gObj[n].m_lpMagicBack = new CMagicInf[MAGIC_SIZE];
		gObj[n].Magic = new CMagicInf[MAGIC_SIZE];
		
		
		if ( n >= OBJ_STARTUSERINDEX )
		{
			gObj[n].Inventory1 = new CItem[INVENTORY_SIZE];
			gObj[n].Inventory2 = new CItem[INVENTORY_SIZE];
			gObj[n].InventoryMap1 = new BYTE[INVENTORY_MAP_SIZE];
			gObj[n].InventoryMap2 = new BYTE[INVENTORY_MAP_SIZE];
			gObj[n].pErtelList1 = new ERTEL_USER[100];
			gObj[n].pErtelList2 = new ERTEL_USER[100];
			gObj[n].m_ElementalOptions = new ELEMENTAL_OPTIONS;
		}
		else
		{
			gObj[n].Inventory1 = pTempInventory;
			gObj[n].Inventory2 = pTempInventory;
			gObj[n].InventoryMap1 = pTempInventoryMap;
			gObj[n].InventoryMap2 = pTempInventoryMap;
			gObj[n].pErtelList1 = pTempErtel;
			gObj[n].pErtelList2 = pTempErtel;
			gObj[n].m_ElementalOptions = pTmpElement;
		}

		if ( n >= OBJ_STARTUSERINDEX )
		{
			gObj[n].Trade = new CItem[TRADE_BOX_SIZE];
			gObj[n].TradeMap = new BYTE[TRADE_BOX_MAP_SIZE];
			gObj[n].pWarehouse = new CItem[WAREHOUSE_SIZE];
			gObj[n].pWarehouseMap = new BYTE[WAREHOUSE_SIZE];
		}
		
		gObj[n].WarehouseCount = 0;
		gObj[n].pChaosBox = NULL;
		
		if ( n >= OBJ_STARTUSERINDEX )
		{
			gObj[n].PerSocketContext = new _PER_SOCKET_CONTEXT;
		}

		gObj[n].m_bIsMaintainParty = TRUE;

		InitializeCriticalSection(&gObj[n].m_critPShopTrade);
	}

	InitFrustrum();
}






void gObjEnd()
{
	gObjAllDisconnect();
	gObjAllLogOut();

	for ( int n=0; n< OBJMAX; n++)
	{
		if ( gObj[n].m_lpMagicBack != NULL )
		{
			delete [] gObj[n].m_lpMagicBack;
		}

		if ( n >= OBJ_STARTUSERINDEX )
		{
			if ( gObj[n].Inventory1 != NULL )
			{
				delete gObj[n].Inventory1;
			}

			if ( gObj[n].Inventory2 != NULL )
			{
				delete gObj[n].Inventory2;
			}

			if ( gObj[n].InventoryMap1 != NULL )
			{
				delete gObj[n].InventoryMap1;
			}

			if ( gObj[n].InventoryMap2 != NULL )
			{
				delete gObj[n].InventoryMap2;
			}

			if ( gObj[n].PerSocketContext != NULL )
			{
				delete gObj[n].PerSocketContext;
			}

			if ( gObj[n].Trade != NULL )
			{
				delete gObj[n].Trade;
			}

			if ( gObj[n].TradeMap != NULL )
			{
				delete gObj[n].TradeMap;
			}

			if ( gObj[n].pWarehouse != NULL )
			{
				delete gObj[n].pWarehouse;
			}

			if ( gObj[n].pWarehouseMap != NULL )
			{
				delete gObj[n].pWarehouseMap;
			}

			if( gObj[n].pErtelList1 )
			{
				delete gObj[n].pErtelList1;
			}

			if( gObj[n].pErtelList2 )
			{
				delete gObj[n].pErtelList2;
			}

			if( gObj[n].m_ElementalOptions )
			{
				delete gObj[n].m_ElementalOptions;
			}
		}

		DeleteCriticalSection( &gObj[n].m_critPShopTrade );
	}
}






void gObjClearViewportOfMine(LPOBJ lpObj)
{
	for (int i=0;i<MAX_VIEWPORT;i++)
	{
		lpObj->VpPlayer[i].state =0;
		lpObj->VpPlayer[i].number =-1;
	}

	lpObj->VPCount=0;
}







void gObjClearViewport(LPOBJ lpObj)
{
	for ( int i=0;i<MAX_VIEWPORT;i++)
	{
		lpObj->VpPlayer[i].state=0;
		lpObj->VpPlayer[i].number =-1;
		lpObj->VpPlayer2[i].state=0;
		lpObj->VpPlayer2[i].number=-1;
	}

	lpObj->VPCount = 0;
	lpObj->VPCount2 = 0;
}







void gObjCloseSet(int aIndex, int Flag)
{
	if ( aIndex < 0 || aIndex > OBJMAX-1 )
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->CloseCount > 0 )
	{
		return;
	}

	if ( lpObj->Connected == PLAYER_PLAYING )
	{
		
		if ( BC_MAP_RANGE(lpObj->MapNumber) )
		{
			g_BloodCastle.SearchUserDropQuestItem(aIndex);
		}

		if (g_SelfDefenseOn)
		{
			if ((GetTickCount() - lpObj->MySelfDefenseTime) < g_SelfDefenseTime * 1000)
			{
				GCServerMsgStringSend(lMsg.Get(MSGGET(4, 109)), lpObj->m_Index, 1);
				GCCloseMsgSend(aIndex, -1);
				return;
			}
		}

		if ( (lpObj->m_IfState.use != 0 ) && ( ( lpObj->m_IfState.type == 1 ) || (lpObj->m_IfState.type == 6) || (lpObj->m_IfState.type == 13) || (lpObj->m_IfState.type == 7)  ) ) 
		{
			gObjSaveChaosBoxItemList(lpObj);
			switch ( lpObj->m_IfState.type )
			{
				case 1:
					GCServerMsgStringSend(lMsg.Get(MSGGET(4, 110)), lpObj->m_Index, 1);
					break;

				case 6:
					GCServerMsgStringSend(lMsg.Get(MSGGET(4, 111)), lpObj->m_Index, 1);
					break;

				case 7:
					GCServerMsgStringSend(lMsg.Get(MSGGET(4, 127)), lpObj->m_Index, 1);
					break;

				case 13:
					GCServerMsgStringSend(lMsg.Get(MSGGET(4, 229)), lpObj->m_Index, 1);
					break;
			}

			return;
		}
	}

	
	lpObj->CloseCount = 6;
	lpObj->CloseType = Flag;
	lpObj->bEnableDelCharacter = 1;
	
}







void gObjCharTradeClear(LPOBJ lpObj)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return;
	}

	memset(lpObj->TradeMap, 0xFF , TRADE_BOX_SIZE);

	for ( int i=0 ; i< TRADE_BOX_SIZE ; i++)
	{
		lpObj->Trade[i].Clear();
	}

	lpObj->TradeMoney = 0;
	lpObj->TradeOk = false;
}






void gObjCharZeroSet(int aIndex)
{
	int i;
	LPOBJ lpObj = &gObj[aIndex];
	int n;

	memset( lpObj->Name, 0, MAX_ACCOUNT_LEN);
	memset( lpObj->GuildName, 0, sizeof (lpObj->GuildName ));

	lpObj->m_Option = 3;
	lpObj->m_iScriptMaxLife = 0;
	lpObj->Level = 0;

	memset( lpObj->m_BackName, 0, MAX_ACCOUNT_LEN);

	lpObj->MasterLevel = 0;
	lpObj->MasterExperience = 0;
	lpObj->MasterNextExp = 0;
	lpObj->MasterPoint = 0;
	lpObj->bMasterInfoLoaded =0;
	lpObj->Life = 0;
	lpObj->MaxLife = 0;
	lpObj->AddLife = 0;
	lpObj->AddMana = 0;
	lpObj->Live = FALSE;
	lpObj->Dir = 0;
	lpObj->X = 0;
	lpObj->Y = 0;
	lpObj->TX = 0;
	lpObj->TY = 0;
	lpObj->MTX = 0;
	lpObj->MTY = 0;
	lpObj->MapNumber = MAP_INDEX_RORENCIA;
	lpObj->RegenTime = 0;
	lpObj->m_State = 0;
	lpObj->RegenOk = 0;
	lpObj->lpAttackObj = 0;
	lpObj->PartyTargetUser = -1;
	lpObj->m_MoveGateNumber = -1;
	lpObj->CloseCount = -1;
	lpObj->CloseType = -1;
	lpObj->m_IfState.state = -1;
	lpObj->m_IfState.type = -1;
	lpObj->m_IfState.use = 0;
	lpObj->m_InterfaceTime = 0;
	lpObj->m_ActionNumber = 0;
	lpObj->m_State = 0;
	lpObj->m_ViewState = 0;
	lpObj->m_Rest = 0;
	lpObj->DeathX = 0;
	lpObj->DeathY = 0;
	lpObj->DeathMap = 0;
	lpObj->BindX = 0;
	lpObj->BindY = 0;
	lpObj->BindMap = 0;
	lpObj->iMaxShield = 0;
	lpObj->iShield = 0;
	lpObj->iAddShield = 0;
	lpObj->iFillShield = 0;
	lpObj->iFillShieldMax = 0;
	lpObj->iFillShieldCount = 0;
	lpObj->dwShieldAutoRefillTimer = 0;
	lpObj->m_wItemEffectType = 0;
	lpObj->m_iItemEffectValidTime = 0;

	lpObj->m_wcCashPoint = 0;
	lpObj->m_wpCashPoint = 0;
	lpObj->m_wgCashPoint = 0;

	lpObj->GoblinPointTick = GetTickCount();
	lpObj->m_iPeriodItemEffectIndex = -1;

	gObjClearBuffEffect(lpObj,CLEAR_TYPE_LOGOUT);

	lpObj->m_Attribute = 0;
	lpObj->m_iMonsterBattleDelay = 0;
	lpObj->m_cKalimaGateExist = FALSE;
	lpObj->m_iKalimaGateIndex = -1;
	lpObj->m_cKalimaGateEnterCount = 0;
	lpObj->TradeOk = false;


	lpObj->sHDCount = 0;
	lpObj->PathCount = 0;
	lpObj->PathCur = 0;
	lpObj->PathStartEnd = 0;
	lpObj->VPCount = 0;
	lpObj->VPCount2 = 0;
	lpObj->m_ShopTime = 0;
	lpObj->m_ManaFillCount = 0;
	lpObj->FillLife = 0;
	lpObj->iObjectSecTimer = 0;
	lpObj->m_FriendServerOnline = FALSE;
	lpObj->SetOpAddMaxAttackDamage = 0;
	lpObj->SetOpAddMinAttackDamage = 0;
	lpObj->SetOpAddDamage = 0;
	lpObj->SetOpIncAGValue = 0;
	lpObj->SetOpAddCriticalDamageSuccessRate = 0;
	lpObj->SetOpAddCriticalDamage = 0;
	lpObj->SetOpAddExDamageSuccessRate = 0;
	lpObj->SetOpAddExDamage = 0;
	lpObj->SetOpAddSkillAttack = 0;
	lpObj->AddStrength = 0;
	lpObj->AddDexterity = 0;
	lpObj->AddVitality = 0;
	lpObj->AddEnergy = 0;
	lpObj->SetOpAddAttackDamage = 0;
	lpObj->SetOpAddDefence = 0;
	lpObj->SetOpAddMagicPower = 0;
	lpObj->SetOpAddDefenceRate = 0;
	lpObj->SetOpIgnoreDefense = 0;
	lpObj->SetOpDoubleDamage = 0;
	lpObj->SetOpTwoHandSwordImproveDamage = 0;
	lpObj->SetOpImproveSuccessAttackRate = 0;
	lpObj->SetOpReflectionDamage = 0;
	lpObj->SetOpImproveSheldDefence = 0;
	lpObj->SetOpDecreaseAG = 0;
	lpObj->SetOpImproveItemDropRate = 0;

	memset( lpObj->m_AddResistance, 0, sizeof(lpObj->m_AddResistance));
	lpObj->IsFullSetItem = false;

	lpObj->DamageMinus = 0;
	lpObj->DamageReflect = 0;
	lpObj->MonsterDieGetMoney = 0;
	lpObj->MonsterDieGetLife = 0;
	lpObj->MonsterDieGetMana = 0;
	lpObj->m_CriticalDamage = 0;
	lpObj->m_ExcelentDamage = 0;
	lpObj->MagicCount = 0;
	lpObj->UseMagicTime = 0;
	lpObj->UseMagicCount = 0;
	lpObj->OSAttackSerial = -1;

	lpObj->m_AttackSpeed = 0;
	lpObj->m_MagicSpeed = 0;
	lpObj->CheckSumTime = GetTickCount() + 10000;
	lpObj->CheckSumTableNum = -1;
	lpObj->m_SumLastAttackTime = 0;
	lpObj->m_DetectCount = 0;
	lpObj->m_DetectSpeedHackTime = 0;
	lpObj->m_SpeedHackPenalty = 0;
	lpObj->m_DetectedHackKickCount = 0;
	lpObj->m_CurseDamageBook = 0;
	lpObj->IsInBattleGround = false;
	lpObj->m_dwPKTimer = GetTickCount();
	lpObj->CheckTickCount = 0;
	lpObj->m_AttackSpeedHackDetectedCount = 0;

	for ( n =0;n<MAX_VIEWPORT ;n++)
	{
		lpObj->VpPlayer[n].state = 0;
		lpObj->VpPlayer2[n].state = 0;
		lpObj->VpPlayer[n].number = -1;
		lpObj->VpPlayer2[n].number = -1;
	}

	lpObj->GuildNumber = 0;
	lpObj->lpGuild = NULL;
	lpObj->PartyNumber = -1;
	lpObj->PartyTargetUser = -1;
	lpObj->m_Drink = 0;
	lpObj->Authority = 0;
	lpObj->Penalty = 0;
	lpObj->m_RecallMon = -1;
	lpObj->m_Change = -1;
	lpObj->GuildStatus = -1;

	lpObj->LastAttackerID = -1;
	lpObj->m_cChaosCastleIndex= -1;
	lpObj->m_cChaosCastleSubIndex = -1;
	lpObj->m_iChaosCastleBlowTime = 0;
	lpObj->m_cKillUserCount = 0;
	lpObj->m_cKillMonsterCount = 0;
	lpObj->m_InWebzen = false;
	memset( lpObj->CharSet, 0, sizeof(lpObj->CharSet));

	for ( i =0; i<MAX_SELF_DEFENSE;i++)
	{
		lpObj->SelfDefense[i] = -1;
		lpObj->SelfDefenseTime[i] = 0;
	}

	lpObj->MySelfDefenseTime = 0;
	lpObj->m_MagicDefense = 0;
	lpObj->m_SkillAttack2 = 0;
	lpObj->m_SkillAttackTime2 = 0;
	lpObj->LoadWareHouseInfo = false;
	lpObj->m_SkyBossMonSheildLinkIndex = -1;
	lpObj->m_SkyBossMonSheild = 0;
	lpObj->m_SkyBossMonSheildTime = 0;
	lpObj->m_bAttackerKilled = false;
	lpObj->m_MoveOtherServer = 0;
	lpObj->m_BossGoldDerconMapNumber = -1;
	lpObj->TargetNumber =	0;
	lpObj->TargetShopNumber = 0;
	lpObj->WarehouseMoney = 0;
	lpObj->WarehouseSave = 0;
	lpObj->WarehouseLock = -1;
	lpObj->WarehouseCount = 0;
	lpObj->WarehousePW = 0;
	lpObj->m_bDevilSquareAuth = false;
	lpObj->m_TimeCount = 0;
	lpObj->EventChipCount = 0;
	lpObj->MutoNumber = 0;
	lpObj->m_ReqWarehouseOpen = 0;
	lpObj->iStoneCount = 0;

	lpObj->m_iDuelRoom = -1;
	lpObj->m_iDuelUserReserved = -1;
	lpObj->m_iDuelUserRequested = -1;
	lpObj->m_iDuelUser = -1;
	//New values Lee added
	lpObj->m_btSculptPos = 0;
	lpObj->m_btCharmPos = 0;
	lpObj->m_btArtifactPos = 0;

	lpObj->m_btDuelScore = 0;
	lpObj->m_iDuelTickCount = 0;
	lpObj->AddBP = 0;
	lpObj->m_CheckLifeTime = 0;
	memset(lpObj->m_Quest, (BYTE)-1, sizeof(lpObj->m_Quest));
	lpObj->m_SendQuestInfo = 0;
	gPacketCheckSum.ClearCheckSum(lpObj->m_Index);
	lpObj->PlusStatQuestClear = false;
	lpObj->ComboSkillquestClear = false;
	lpObj->m_LastTeleportTime = 0;
	lpObj->m_ClientHackLogCount = 0;
	lpObj->UseEventServer = FALSE;
	lpObj->m_EnableUseChangeSkin = false; //1.01.06
	
	lpObj->RecvSendWare = 0;

	if ( lpObj->Type == OBJ_USER )
	{
		memset(lpObj->pWarehouseMap, (BYTE)-1, WAREHOUSE_SIZE);
		memset(lpObj->TradeMap, (BYTE)-1, TRADE_BOX_SIZE);
	}

	for ( i=0;i<MAX_MAGIC ; i++)
	{
		lpObj->Magic[i].Clear();
	}

	lpObj->MagicCount = 0;

	for ( i=0;i<MAX_MONSTER_SEND_MSG;i++)
	{
		gSMMsg[aIndex][i].MsgCode = -1;
	}

	lpObj->m_bIsInMonsterHerd = false;
	lpObj->m_bIsMonsterAttackFirst = true;
	lpObj->m_lpMonsterHerd = NULL;

	if ( (aIndex-OBJ_STARTUSERINDEX) >= 0 )
	{
		for ( i=0;i<MAX_MONSTER_SEND_ATTACK_MSG;i++)
		{
			gSMAttackProcMsg[aIndex-OBJ_STARTUSERINDEX][i].MsgCode = -1;
		}
	}

	lpObj->SkillHellFire2State = 0;
	lpObj->SkillHellFire2Count = 0;
	lpObj->Leadership = 0;
	lpObj->AddLeadership = 0;
	lpObj->ChatLimitTime = 0;
	lpObj->ChatLimitTimeSec = 0;
	lpObj->SkillRecallParty_Time = 0;
	lpObj->SkillLongSpearChange = false;
	lpObj->SkillDelay.Init();

	lpObj->bEnableDelCharacter = 1;

	if ( lpObj->Type == OBJ_USER )
	{
		for (i=0;i<WAREHOUSE_SIZE;i++)
		{
			lpObj->pWarehouse[i].Clear();
		}
	}

#if (ENABLETEST_FORCEOPTIMIZE == 0)
	gMultiAttackHackCheck[aIndex].Init();
#endif

	lpObj->pTransaction = 0;
	lpObj->pInventoryExtend = 0;
	gObjMonsterHitDamageInit(lpObj);
	gObjSetInventory1Pointer(&gObj[aIndex]);

	if ( lpObj->Type == OBJ_USER )
	{
		memset(lpObj->pInventoryMap, (BYTE)-1, INVENTORY_MAP_SIZE);
	}

	lpObj->m_bPShopOpen = false;
	lpObj->m_bPShopTransaction = false;
	lpObj->m_bPShopItemChange = false;
	lpObj->m_bPShopRedrawAbs = false;
	memset(lpObj->m_szPShopText, 0, sizeof(lpObj->m_szPShopText));
	lpObj->m_bPShopWantDeal = false;
	lpObj->m_iPShopDealerIndex = -1;
	memset(lpObj->m_szPShopDealerName, 0, sizeof(lpObj->m_szPShopDealerName));
	memset(lpObj->m_iVpPShopPlayer, 0, sizeof(lpObj->m_iVpPShopPlayer));
	lpObj->m_wVpPShopPlayerCount =0;
	lpObj->iGuildUnionTimeStamp = 0;
	::gObjCharTradeClear(lpObj);

	if ( lpObj->Type == OBJ_USER )
	{
		for ( i=0;i<WAREHOUSE_SIZE;i++)
		{
			lpObj->pWarehouse[i].Clear();
		}

		for ( i=0;i<INVENTORY_SIZE;i++)
		{
			lpObj->Inventory1[i].Clear();
		}

		for ( i=0;i<INVENTORY_SIZE;i++)
		{
			lpObj->Inventory2[i].Clear();
		}

		for ( i=0;i<TRADE_BOX_SIZE;i++)
		{
			lpObj->Trade[i].Clear();
		}

		for ( i=0;i<MAX_ERTEL;i++)
		{
			lpObj->pErtelList1[i].Clear();
			lpObj->pErtelList2[i].Clear();
		}
#if (ENABLETEST_MUUN == 1)
		lpObj->m_wMuunItem = -1;
		lpObj->bSubEquip = 0;
		gObjSetMuunInventory1Pointer(&gObj[aIndex]);
		for (i = 0; i < MUUN_INVENTORY_SIZE; i++) {
			lpObj->pMuunInventory1[i].Clear();
			lpObj->pMuunInventory2[i].Clear();
		}
		for (i = 0; i < MUUN_MAX_ACTIVEUSE; i++) {
			lpObj->m_MuunEffectList[i].Clear();
		}
		lpObj->bMuunInventoryLoad = 0;
		lpObj->dwCheckMuunItemTime = 0;
#endif
	}

	lpObj->bIsChaosMixCompleted = false;
	lpObj->m_bMapSvrMoveQuit = false;

#if (GS_CASTLE == 1)

	lpObj->m_iCsNpcExistVal = 0;
	lpObj->m_btCsNpcType = 0;
	lpObj->m_btCsGateOpen = 0;
	lpObj->m_iCsGateLeverLinkIndex = -1;
	lpObj->m_btCsNpcDfLevel = 0;
	lpObj->m_btCsNpcRgLevel = 0;
	lpObj->m_btCsJoinSide = 0;
	lpObj->m_bCsGuildInvolved = 0;

#endif

	lpObj->m_bIsCastleNPCUpgradeCompleted = false;
	lpObj->m_btWeaponState = 0;
	lpObj->m_btKillCount = 0;
	lpObj->m_iAccumulatedDamage = 0;

#if (GS_CASTLE == 1)

	lpObj->m_iCreatedActivationTime = 0;
	lpObj->m_btLifeStoneCount = 0;

#endif

	lpObj->m_iAccumulatedCrownAccessTime = 0;

	lpObj->m_MonsterSkillElementInfo.Reset();
	lpObj->m_iCrywolfMVPScore = 0;
	lpObj->m_iSkillDistanceErrorCount = 0;
	lpObj->m_dwSkillDistanceErrorTick = 0;
	g_kJewelOfHarmonySystem.InitEffectValue(&lpObj->m_JewelOfHarmonyEffect);
	g_kItemSystemFor380.InitEffectValue(&lpObj->m_ItemOptionExFor380);
	g_SocketItem.InitUserSocketOptions(lpObj);
#if (GS_CASTLE == 0 || GS_SPEC == 1)
	lpObj->m_bKanturuEntranceByNPC = 0;
#endif

	lpObj->iTempleIndex = -1;
	lpObj->m_MonsterKillInfoLoaded = 0;
	lpObj->m_MonsterKillQuestIndex = -1;

	for(i = 0;i < 5; i++)
	{
		lpObj->m_MonsterKill[i].m_MonsterIndex = -1;
		lpObj->m_MonsterKill[i].m_KillCount = -1;
	}

	lpObj->iOptionRecv = 0;
	g_MasterSkillSystem.InitPassiveSkillData(lpObj);

	lpObj->m_wSocketAddLife = 0;
	lpObj->m_wSocketAddMana = 0;
	lpObj->m_wSocketMonsterDieLife = 0;
	lpObj->m_wSocketMonsterDieMana = 0;
	lpObj->m_btSocketBPUseDec = 0;
	lpObj->m_bIsMaintainParty = TRUE;

	::gObjClearViewport(&gObj[aIndex]);

#if (__GENS_SYSTEM__ == 1)
	lpObj->m_GensInfo.btFamily = GENS_NONE;
	lpObj->m_GensInfo.btLevel = 0;
	lpObj->m_GensInfo.iRank = 0;
	lpObj->m_GensInfo.iContribution = 0;
	lpObj->m_GensInfo.btReward = 0;
#endif

	lpObj->m_bMUBOT = FALSE;
	lpObj->m_iMUBOT_TIME = 0;
	lpObj->m_iMUBOT_STAGE = 0;

	lpObj->m_InfinityArrowDmgMul = 0;

	g_ElementalSystem.InitElementOptions(lpObj);
	g_MiningSystem.ResetMiningInfo(lpObj);

	//lpObj->m_bOpenMinimap = false;
	lpObj->m_bSendMemberPos = false;
	lpObj->m_bSendNpcPos = false;

	lpObj->m_ShopPointExTime = 0;
	lpObj->m_ShopPointExTimeBackup = 0;

#if (__CUSTOM__ ==1)
	lpObj->iResetCount = 0;
	lpObj->m_ResetWait = 0;
	lpObj->m_ResetWaitTick = 0;

#if (__CUSTOM_QUEST__ == 1)
	g_MyQuest.InitUser(lpObj->m_Index);
#endif

#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
	lpObj->OffTradeWaitItem = 0;
	lpObj->bOffTrade = 0;
	lpObj->iOffTradeTime = 0;
#endif

#if (ANTI_CHEAT==1)
	lpObj->m_iCHECK_COUNT = 0;
#endif

#if (CUSTOM_OLYMP==1)
	lpObj->m_btChangedStats = 0;
#endif

#endif
}






int gObjGetSocket(SOCKET socket)
{
	for (int n=OBJ_STARTUSERINDEX;n<OBJMAX;n++)
	{
		if (gObj[n].Connected != PLAYER_EMPTY)
		{
			if (gObj[n].m_socket == socket)
			{
				return n;
			}
		}
	}
	return -1;
}






void gObjSetTradeOption( int aIndex, int option)
{
	if ( gObjIsConnected(aIndex) == TRUE )
	{
		if ( option == 0 )
		{
			gObj[aIndex].m_Option = 0;
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 97)), aIndex, 1);
		}
		else
		{
			gObj[aIndex].m_Option |= 1;
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 96)), aIndex, 1);
		}
	}
}





void gObjSetDuelOption(int aIndex, int option)
{
	if ( gObjIsConnected(aIndex) == TRUE )
	{
		if ( option == 0 )
		{
			gObj[aIndex].m_Option = 0;
		}
		else
		{
			gObj[aIndex].m_Option |= 2;
		}
	}
}






bool IsDuelEnable(int aIndex)
{
	if ( OBJMAX_RANGE(aIndex ) == FALSE)
	{
		return false;
	}

	if ( gObjIsConnected(aIndex) == TRUE )
	{
		if ( (gObj[aIndex].m_Option&2) == 2 )
		{
			return true;
		}
	}

	return false;
}







bool IsOnDuel(int aIndex1, int aIndex2 )
{
	if ( OBJMAX_RANGE(aIndex1) == FALSE || OBJMAX_RANGE(aIndex2) == FALSE )
	{
		return false;
	}

	if ( gObj[aIndex1].Type != OBJ_USER || gObj[aIndex2].Type != OBJ_USER )
	{
		return false;
	}

	if ( gObj[aIndex1].m_iDuelUser == aIndex2 && gObj[aIndex2].m_iDuelUser == aIndex1 )
	{
		return true;
	}

	return false;
}






int GetMapMoveLevel(LPOBJ lpObj, int mapnumber, int max_over)
{
	int overLevel = 0;

	if ( mapnumber > MAX_NUMBER_MAP-1 )
	{
		mapnumber = 0;
		LogAdd("error-L3 : map number not %s %d", __FILE__, __LINE__);
		return 0;
	}

	if ( max_over != 0 )
	{
		overLevel = max_over;
	}
	else
	{
		max_over = MapMinUserLevel[mapnumber];
		overLevel = MapMinUserLevel[mapnumber];
	}
	
	if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER )
	{
		if ( overLevel > 0 && max_over > 0 )
		{
			overLevel = (max_over/3)*2;
		}
	}

	return overLevel;
}

void DbItemSetInByte(LPOBJ lpObj, struct SDHP_DBCHAR_INFORESULT* lpMsg, int ItemDbByte, bool* bAllItemExist)
{
	int n;
	int itype;
	int _type;
	CItem item;
	BYTE OptionData;
	WORD hiWord;
	WORD loWord;
	bool bIsItemExist = true;

	time_t curTime;
	curTime = time(NULL);

	for(n = 0; n < INVENTORY_SIZE;n++)
	{
		bIsItemExist = true;
		lpObj->pInventory[n].Clear();
		itype = lpMsg->dbInventory[n*ItemDbByte];

		
		if(lpMsg->dbInventory[n*ItemDbByte] == 0xFF
			&& (lpMsg->dbInventory[n*ItemDbByte+DBI_NOPTION_DATA] & 0x80) == 0x80
			&& (lpMsg->dbInventory[n*ItemDbByte+DBI_OPTION380_DATA] & 0xF0) == 0xF0)
		{
			continue;
		}

		itype |= (lpMsg->dbInventory[n*ItemDbByte+DBI_OPTION380_DATA] & 0xF0) * 32;
		itype |= (lpMsg->dbInventory[n*ItemDbByte+DBI_NOPTION_DATA] & 0x80) * 2;
		_type = itype;

		if(lpMsg->dbInventory[n*ItemDbByte] == 0xFF
			&& (lpMsg->dbInventory[n*ItemDbByte+DBI_NOPTION_DATA] & 0x80) == 0x80
			&& (lpMsg->dbInventory[n*ItemDbByte+DBI_OPTION380_DATA] & 0xF0) == 0xF0)
		{
			itype = -1;
		}

		if(IsItem(_type) == false)
		{
			bIsItemExist = 0;

			if(bAllItemExist != 0)
			{
				*bAllItemExist = 0;
			}
		}

		if(_type == ITEMGET(14,17) || _type == ITEMGET(14,18))
		{
			if(lpMsg->dbInventory[n*ItemDbByte+DBI_DUR]==0)
			{
				itype = -1;
			}
		}

		if(_type == ITEMGET(14,19))
		{
			if(lpMsg->dbInventory[n*ItemDbByte+DBI_DUR]==(BYTE)-1)
			{
				itype = -1;
			}
		}

		if(_type == ITEMGET(14,100))
		{
			if(lpMsg->dbInventory[n*ItemDbByte+DBI_DUR]==0)
			{
				itype = -1;
			}
		}

		if(itype != -1)
		{
			item.m_Level = DBI_GET_LEVEL(lpMsg->dbInventory[n*ItemDbByte+DBI_OPTION_DATA]);

			if(_type != ITEMGET(14,11))
			{
				if(item.m_Level > MAX_ITEM_LEVEL)
				{
					item.m_Level = 0;
				}
			}

			OptionData = lpMsg->dbInventory[n*ItemDbByte+DBI_OPTION_DATA];
			item.m_Option1 = DBI_GET_SKILL(OptionData)&1;
			item.m_Option2 = DBI_GET_LUCK(OptionData);
			item.m_Option3 = DBI_GET_OPTION(OptionData);
			item.m_Option3 |= DBI_GET_OPTION16(lpMsg->dbInventory[n*ItemDbByte+DBI_NOPTION_DATA]);
			item.m_Durability = lpMsg->dbInventory[n*ItemDbByte+DBI_DUR];
			item.m_JewelOfHarmonyOption = lpMsg->dbInventory[n*ItemDbByte+DBI_JOH_DATA];
			item.m_ItemOptionEx = DBI_GET_380OPTION(lpMsg->dbInventory[n*ItemDbByte+DBI_OPTION380_DATA]);

			if(item.m_ItemOptionEx)
			{
				item.m_Type = itype;
					
				if(g_kItemSystemFor380.Is380Item(&item)==false)
				{
					item.m_ItemOptionEx = 0;
					LogAddTD("[380Item][%s][%s] Invalid 380 Item Option in Inventory pos[%d]",
						lpObj->AccountID,lpObj->Name,n);
				}
			}

			item.m_bPeriodItem = (lpMsg->dbInventory[n*ItemDbByte+DBI_OPTION380_DATA]&2)>>1;
			item.m_bExpired = (lpMsg->dbInventory[n*ItemDbByte+DBI_OPTION380_DATA]&4)>>2;

			if( item.m_bExpired )
			{
				LogAddTD("[Period Item][%s][%s] Deleting expired item Item[%d], Pos[%d]",
					lpObj->AccountID,lpObj->Name,n,item.m_Type);
				continue;
			}

			BYTE SocketOptions[MAX_SOCKET_COUNT];
			BYTE SocketBonus = 0xFF;

			memset(SocketOptions,0xFF,MAX_SOCKET_COUNT);

			if( g_SocketItem.IsSocketItem(itype) == TRUE ||
				g_ElementalSystem.IsElementalItem(itype) == TRUE )
			{
				for(int i = 0; i < MAX_SOCKET_COUNT; i++)
				{
					SocketOptions[i] = lpMsg->dbInventory[n*ItemDbByte+DBI_JOH_DATA+1+i];
				}

				SocketBonus = lpMsg->dbInventory[n*ItemDbByte+DBI_JOH_DATA];
			}
			else
			{
				for(int i = 0; i < MAX_SOCKET_COUNT; i++)
				{
					SocketOptions[i] = 0xFF;
				}

				SocketBonus = 0;
			}

			/*&if( itype == ITEMGET(12,200) )
			{
				SocketBonus = 0x01;
				SocketOptions[0] = 0xFE;
				SocketOptions[1] = 0xFF;
				SocketOptions[2] = 0xFF;
				SocketOptions[3] = 0xFF;
				SocketOptions[4] = 0xFF;

				item.m_JewelOfHarmonyOption = 0x01;
			}*/

			/*if( itype == ITEMGET(12,221) ||
				itype == ITEMGET(12,231) ||
				itype == ITEMGET(12,241) ||
				itype == ITEMGET(12,251) ||
				itype == ITEMGET(12,261) )
			{
				SocketBonus = 0x01;
				SocketOptions[0] = 0x1 | rand()%11*16;
				SocketOptions[1] = 0x1 | rand()%11*16;
				SocketOptions[2] = 0x1 | rand()%11*16;
				SocketOptions[3] = 0x1 | rand()%11*16;
				SocketOptions[4] = 0x1 | rand()%11*16;

				item.m_JewelOfHarmonyOption = 0x01;
			}*/

			item.m_bLOCKED = ( (lpMsg->dbInventory[n*ItemDbByte+DBI_SOPTION_DATA]&0xC0) == 0xC0 ) ? TRUE : FALSE;
			item.Convert(itype,item.m_Option1,item.m_Option2,item.m_Option3,DBI_GET_NOPTION(lpMsg->dbInventory[n*ItemDbByte+DBI_NOPTION_DATA]),lpMsg->dbInventory[n*ItemDbByte+DBI_SOPTION_DATA],item.m_ItemOptionEx,&SocketOptions[0],SocketBonus,3);

			if(_type == ITEMGET(14,7))
			{

			}
			else
			{
				if(_type >= ITEMGET(14,0) && _type <= ITEMGET(14,8)
					|| _type >= ITEMGET(14,38) && _type <= ITEMGET(14,40))
				{
					if(item.m_Durability == 0.0f)
					{
						item.m_Durability = 1.0f;
					}

					if(item.m_Durability > 255.0f)
					{
						item.m_Durability = 255.0f;
					}
				}
				
				if(_type == ITEMGET(14,70) || _type == ITEMGET(14,71))
				{
					if( item.m_Durability == 0.0f)
					{
						item.m_Durability = 1.0f;
					}

					if( item.m_Durability > 255.0f)
					{
						item.m_Durability = 255.0f;
					}
				}
				
				if(_type == ITEMGET(14,94))
				{
					if( item.m_Durability == 0.0f )
					{
						item.m_Durability = 1.0f;
					}
					
					if( item.m_Durability > 100.0f )
					{
						item.m_Durability = 100.0f;
					}
				}
				
				if(_type == ITEMGET(14,78) ||
						_type == ITEMGET(14,79) ||
						_type == ITEMGET(14,80) ||
						_type == ITEMGET(14,81) ||
						_type == ITEMGET(14,82) )
				{
					if( item.m_Durability == 0.0f )
					{
						item.m_Durability = 1.0f;
					}

					if( item.m_Durability > 2.0f )
					{
						item.m_Durability = 2.0f;
					}
				}
				else if(_type >= ITEMGET(14,46) && _type <= ITEMGET(14,50))
				{
					if(item.m_Durability == 0.0f)
					{
						item.m_Durability = 1.0f;
					}

					if(item.m_Durability > 255.0f)
					{
						item.m_Durability = 255.0f;
					}
				}
				else if(_type >= ITEMGET(14,35) && _type <= ITEMGET(14,37))
				{
					if(item.m_Durability == 0.0f)
					{
						item.m_Durability = 1.0f;
					}

					if(item.m_Durability > 255.0f)
					{
						item.m_Durability = 255.0f;
					}
				}
				else if(_type != ITEMGET(13, 10)
					&& _type != ITEMGET(14, 29)
					&& _type != ITEMGET(14, 21)
					&& item.m_Level == 3
					&& _type != ITEMGET(14, 110)
					&& _type != ITEMGET(14, 101)
					&& _type != ITEMGET(13, 145) )
				{
					if(item.m_Durability > item.m_BaseDurability && bIsItemExist == 1)
					{
						item.m_Durability = item.m_BaseDurability;
					}
				}
				else if(_type == ITEMGET(13,70))
				{
					if( item.m_Durability == 1 )
					{
						item.m_Durability = item.m_BaseDurability;
					}
				}
			}

			lpObj->pInventory[n].m_Option1 = item.m_Option1;
			lpObj->pInventory[n].m_Option2 = item.m_Option2;
			lpObj->pInventory[n].m_Option3 = item.m_Option3;
			lpObj->pInventory[n].m_JewelOfHarmonyOption = item.m_JewelOfHarmonyOption;
			lpObj->pInventory[n].m_ItemOptionEx = item.m_ItemOptionEx;
			lpObj->pInventory[n].m_SocketBonus = item.m_SocketBonus;

			for(int i = 0; i < MAX_SOCKET_COUNT; i++)
			{
				lpObj->pInventory[n].m_SocketOption[i] = item.m_SocketOption[i];
			}

			hiWord = ((lpMsg->dbInventory[n*ItemDbByte+DBI_SERIAL2])&0xFF)&0xFF | (((lpMsg->dbInventory[n*ItemDbByte+DBI_SERIAL1])&0xFF)&0xFF) * 256;
			loWord = ((lpMsg->dbInventory[n*ItemDbByte+DBI_SERIAL4])&0xFF)&0xFF | (((lpMsg->dbInventory[n*ItemDbByte+DBI_SERIAL3])&0xFF)&0xFF) * 256;

			item.m_Number = ((loWord &0xFFFF)&0xFFFF) | ((hiWord & 0xFFFF)&0xFFFF) << 16;

			if( item.m_bPeriodItem )
			{
				if( item.m_Number <= curTime )
				{
					item.m_bExpired = TRUE;
					LogAddTD("[Period Item][%s][%s] Deleting expired item Item[%d], Pos[%d]",
						lpObj->AccountID,lpObj->Name,n,item.m_Type);
					continue;
				}
			}

			gObjInventoryInsertItemPos(lpObj->m_Index,item,n,0);

			if(bIsItemExist == false)
			{
				lpObj->pInventory[n].m_bItemExist = 0;

				LogAddTD("error-L2 : Unknown Item found [%s][%s] (Type:%d, LEV:%d, DUR:%d OP1:%d, OP2:%d, OP3:%d, NEWOP:%d, SET:%d)",
					lpObj->AccountID,lpObj->Name,lpObj->pInventory[n].m_Type,lpObj->pInventory[n].m_Level,
					lpObj->pInventory[n].m_Durability,lpObj->pInventory[n].m_Option1,
					lpObj->pInventory[n].m_Option2,lpObj->pInventory[n].m_Option3,
					lpObj->pInventory[n].m_NewOption,lpObj->pInventory[n].m_SetOption);
			}
		}
	}
	gObjRequestPetItemInfo(lpObj->m_Index, 0);
}




void gObjSetBP(int aIndex)
{
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("setStaminaPool", aIndex);
	ZTLuaManager::Delete(LuaMng);
#else
	int Strength = gObj[aIndex].Strength + gObj[aIndex].AddStrength;
	int Dexterity = gObj[aIndex].Dexterity + gObj[aIndex].AddDexterity;
	int Vitality = gObj[aIndex].Vitality + gObj[aIndex].AddVitality;
	int Energy = gObj[aIndex].Energy + gObj[aIndex].AddEnergy;
	int Leadership = gObj[aIndex].Leadership + gObj[aIndex].AddLeadership;

	switch ( gObj[aIndex].Class )
	{
		case 0:
			gObj[aIndex].MaxBP = (int)((Strength * 0.2) + (Dexterity * 0.4) + (Vitality * 0.3) + (Energy * 0.2));
			break;

		case 1:
			gObj[aIndex].MaxBP = (int)((Strength * 0.15) + (Dexterity * 0.2) + (Vitality * 0.3) + (Energy * 1.0));
			break;

		case 2:
			gObj[aIndex].MaxBP = (int)((Strength * 0.3) + (Dexterity * 0.2) + (Vitality * 0.3) + (Energy * 0.2));
			break;

		case 3:
			gObj[aIndex].MaxBP = (int)((Strength * 0.2) + (Dexterity * 0.25) + (Vitality * 0.3) + (Energy * 0.15));
			break;

		case 4:
			gObj[aIndex].MaxBP = (int)((Strength * 0.3) + (Dexterity * 0.2) + (Vitality * 0.1) + (Energy * 0.15) + (Leadership * 0.3));
			break;
		case 5:
			gObj[aIndex].MaxBP = (int)((Strength * 0.2) + (Dexterity * 0.4) + (Vitality * 0.3) + (Energy * 0.2));
			break;
		case 6:
			gObj[aIndex].MaxBP = (int)((Strength * 0.5) + (Dexterity * 0.2) + (Vitality * 0.15) + (Energy * 0.3));
			break;
	}
#endif
}





BOOL gObjSetCharacter(LPBYTE lpdata, int aIndex)
{
	int itype;
	int n;
	char szCharName[MAX_ACCOUNT_LEN+1];
	SDHP_DBCHAR_INFORESULT * lpMsg = (SDHP_DBCHAR_INFORESULT *)lpdata;

	// Check in position
	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->Connected < PLAYER_LOGGED )
	{
		return FALSE;
	}

	if ( gObj[aIndex].Connected == PLAYER_PLAYING )
	{
		LogAddC(2, lMsg.Get(MSGGET(1, 242)), gObj[aIndex].AccountID, gObj[aIndex].Name);
		return FALSE;
	}

	if( lpMsg->Class == 0 || lpMsg->Class == 16 || lpMsg->Class == 32 || lpMsg->Class == 48 ||
		lpMsg->Class == 1 || lpMsg->Class == 17 || lpMsg->Class == 33 || lpMsg->Class == 64 ||
		lpMsg->Class == 3 || lpMsg->Class == 19 || lpMsg->Class == 35 || lpMsg->Class == 50 || lpMsg->Class == 66 ||
		lpMsg->Class == 80 || lpMsg->Class == 81 || lpMsg->Class == 83 ||
		lpMsg->Class == 96 || lpMsg->Class == 98 )
	{

	}
	else
	{
		return FALSE;
	}


	gObjCharZeroSet(aIndex);

	if ( g_ChaosBox.ChaosBoxInit(lpObj) == FALSE )
	{
		LogAdd("error-L1 : ChaosBox Init error %s %d", __FILE__, __LINE__);
	}

//#ifdef STREAM
	lpObj->m_Credits = lpMsg->m_Credits;
//#endif

#if( ENABLE_CUSTOM_HARDCORE == 1 )
	lpObj->m_HardcoreLife = lpMsg->HardcoreLife;
#endif

	lpObj->m_PosNum = 0;
	lpObj->Level = lpMsg->Level;

	lpObj->LevelUpPoint = lpMsg->LevelUpPoint;
	lpObj->DbClass = lpMsg->Class;
	lpObj->Class = lpMsg->Class >> 4;

	int ChangeUP = lpMsg->Class & 0x07;	// Set Second Type of Character
	lpObj->ChangeUP = ChangeUP & 1;
	lpObj->ChangeUP2 = (ChangeUP & 2) >> 1;

	szCharName[MAX_ACCOUNT_LEN] = 0;
	memcpy(szCharName, lpMsg->Name , MAX_ACCOUNT_LEN);
	strcpy_s(lpObj->Name, sizeof(lpObj->Name), szCharName);

	if ( lpObj->Level < 1 )
	{
		LogAddC(2, "error : %s Level Zero", lpMsg->Name);
		return FALSE;
	}

	lpObj->X = lpMsg->MapX;
	lpObj->Y = lpMsg->MapY;
	lpObj->MapNumber = lpMsg->MapNumber;
	lpObj->StartX = lpObj->X;
	lpObj->StartY = lpObj->Y;
	
	lpObj->pInventoryExtend = lpMsg->btExInventory;

	if ( MAX_MAP_RANGE(lpObj->MapNumber) == FALSE )
	{
		LogAdd("error : Map Number max over. %s %d", __FILE__, __LINE__);
		lpObj->MapNumber = MAP_INDEX_RORENCIA;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
	}

	if ( lpObj->Level < 6 || DS_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		if ( lpObj->Class == CLASS_ELF )
		{
			lpObj->MapNumber = MAP_INDEX_NORIA;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
		}
		else if( lpObj->Class == CLASS_SUMMONER )
		{
			lpObj->MapNumber = MAP_INDEX_ELBELAND;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
		}
		else
		{
			lpObj->MapNumber = MAP_INDEX_RORENCIA;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
		}
	}

	if ( DS_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		int iTempUserDataSlotIndex = cpTempUserManager->CheckMatchCharacter(lpObj->Name);

		if( iTempUserDataSlotIndex != -1 )
		{
			int bResult = cpTempUserManager->ResumeEvent(iTempUserDataSlotIndex,aIndex);

			if( bResult == 0 )
			{
				lpObj->MapNumber = MAP_INDEX_RORENCIA;
				MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
			}
		}
		else
		{
			lpObj->MapNumber = MAP_INDEX_RORENCIA;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
		}
	}
	
	if ( BC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		int iTempUserDataSlotIndex = cpTempUserManager->CheckMatchCharacter(lpObj->Name);

		if( iTempUserDataSlotIndex != -1 )
		{
			int bResult = cpTempUserManager->ResumeEvent(iTempUserDataSlotIndex,aIndex);

			if( bResult == 0 )
			{
				lpObj->MapNumber = MAP_INDEX_DEVIAS;
				MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);

				lpObj->m_cBloodCastleIndex = -1;
				lpObj->m_cBloodCastleSubIndex = -1;
				lpObj->m_iBloodCastleEXP = 0;
			}
		}
		else
		{
			lpObj->MapNumber = MAP_INDEX_DEVIAS;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);

			lpObj->m_cBloodCastleIndex = -1;
			lpObj->m_cBloodCastleSubIndex = -1;
			lpObj->m_iBloodCastleEXP = 0;
		}
	}
	else
	{
		lpObj->m_cBloodCastleIndex = -1;
		lpObj->m_cBloodCastleSubIndex = -1;
		lpObj->m_iBloodCastleEXP = 0;
	}

	lpObj->m_bBloodCastleComplete = 0;
	
	if ( lpObj->MapNumber == MAP_INDEX_SANTAVILLAGE )
	{
		lpObj->MapNumber = MAP_INDEX_DEVIAS;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
	}

	if ( CC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		lpObj->MapNumber = MAP_INDEX_DEVIAS;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
	}
	
	lpObj->m_cChaosCastleIndex = -1;
	lpObj->m_cChaosCastleSubIndex = -1;
	lpObj->m_iChaosCastleBlowTime = 0;
	lpObj->m_cKillUserCount = 0;
	lpObj->m_cKillMonsterCount = 0;

	if ( (lpMsg->CtlCode & 32) != 32 && lpObj->MapNumber == MAP_INDEX_GMMAP )
	{
		lpObj->MapNumber = MAP_INDEX_DEVIAS;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
	}
	else if( IT_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		int iTempUserDataSlotIndex = cpTempUserManager->CheckMatchCharacter(lpObj->Name);

		if( iTempUserDataSlotIndex != -1 )
		{
			int bResult = cpTempUserManager->ResumeEvent(iTempUserDataSlotIndex,aIndex);

			if( bResult == 0 )
			{
				lpObj->MapNumber = MAP_INDEX_ELBELAND;
				MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
			}
		}
		else
		{
			lpObj->MapNumber = MAP_INDEX_ELBELAND;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
		}
	}
	else if( IF_MAP_RANGE(lpObj->MapNumber) )
	{
		int iTempUserDataSlotIndex = cpTempUserManager->CheckMatchCharacter(lpObj->Name);

		if( iTempUserDataSlotIndex != -1 )
		{
			int bResult = cpTempUserManager->ResumeEvent(iTempUserDataSlotIndex,aIndex);

			if( bResult == 0 )
			{
				BYTE sX;
				BYTE sY;
				short sLVL;
				BYTE btMAPNUM = lpObj->MapNumber;
				BYTE btDIR;

				int iMapNumber = gGateC.GetGate(22,sX,sY,btMAPNUM,btDIR,sLVL);

				if( iMapNumber >= 0 )
				{
					lpObj->MapNumber = btMAPNUM;
					lpObj->X = sX;
					lpObj->Y = sY;
				}
			}
		}
		else
		{
			BYTE sX;
			BYTE sY;
			short sLVL;
			BYTE btMAPNUM = lpObj->MapNumber;
			BYTE btDIR;

			int iMapNumber = gGateC.GetGate(22,sX,sY,btMAPNUM,btDIR,sLVL);

			if( iMapNumber >= 0 )
			{
				lpObj->MapNumber = btMAPNUM;
				lpObj->X = sX;
				lpObj->Y = sY;
			}
		}
	}
	else if( DG_MAP_RANGE(lpObj->MapNumber) )
	{
		lpObj->MapNumber = MAP_INDEX_ELBELAND;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
	}

#if (GS_CASTLE == 1)
	if ( lpObj->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE )
	{
		BYTE sX;
		BYTE sY;
		short sLVL;
		unsigned char btMAPNUM = lpObj->MapNumber;
		unsigned char btDIR;

		int iMapNumber = gGateC.GetGate(100,sX,sY,(BYTE &)btMAPNUM,(BYTE &)btDIR,(short &)sLVL);

		if(iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
			lpObj->m_sDestMapNumber = btMAPNUM;
			lpObj->m_btDestX = sX;
			lpObj->m_btDestY = sY;
		}
	}

	if ( lpObj->MapNumber == MAP_INDEX_CASTLEHUNTZONE )
	{
		BYTE sX;
		BYTE sY;
		short sLVL;
		unsigned char btMAPNUM = lpObj->MapNumber;
		unsigned char btDIR;

		int iMapNumber = gGateC.GetGate(106,sX,sY,(BYTE &)btMAPNUM,(BYTE &)btDIR,(short &)sLVL);

		if(iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
			lpObj->m_sDestMapNumber = btMAPNUM;
			lpObj->m_btDestX = sX;
			lpObj->m_btDestY = sY;
		}
	}

	if( lpObj->MapNumber == MAP_INDEX_BARRACKS ||
		lpObj->MapNumber == MAP_INDEX_REFUGE )
	{
		int GateNumber = 258;
		BYTE sX;
		BYTE sY;
		short sLVL;
		unsigned char btMAPNUM = lpObj->MapNumber;
		unsigned char btDIR;

		int iMapNumber = gGateC.GetGate(GateNumber,sX,sY,(BYTE &)btMAPNUM,(BYTE &)btDIR,(short &)sLVL);

		if(iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
		}
	}
#endif
#if (GS_CASTLE == 0 || GS_SPEC == 1)

	if ( lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS )
	{
		BYTE sX;
		BYTE sY;
		short sLVL;
		unsigned char btMAPNUM = lpObj->MapNumber;
		unsigned char btDIR;

		int iMapNumber = gGateC.GetGate(137,sX,sY,(BYTE &)btMAPNUM,(BYTE &)btDIR,(short &)sLVL);

		if(iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
		}
	}
#endif

	if ( lpObj->MapNumber == MAP_INDEX_RAKLIONBOSS )
	{
		BYTE sX;
		BYTE sY;
		short sLVL;
		unsigned char btMAPNUM = lpObj->MapNumber;
		unsigned char btDIR;

		int iMapNumber = gGateC.GetGate(287,sX,sY,(BYTE &)btMAPNUM,(BYTE &)btDIR,(short &)sLVL);

		if(iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
			lpObj->m_sDestMapNumber = btMAPNUM;
			lpObj->m_btDestX = sX;
			lpObj->m_btDestY = sY;
		}
	}

#if (ENABLETEST_NEWPVP == 1)
	//if (gNonPK) {
	if (g_NewPVP.IsVulcanusMap(lpObj->MapNumber) || g_NewPVP.IsPKFieldMap(lpObj->MapNumber)) {
		int iGateNumber = 17;
		short sX;
		short sY;
		short sLVL;
		BYTE btMAPNUM	= lpObj->MapNumber;
		BYTE btDIR;
	
		int iMapNumber = gGateC.GetGate(iGateNumber, (BYTE &)sX, (BYTE &)sY, (BYTE &)btMAPNUM, (BYTE &)btDIR, (short &)sLVL);
	
		if (iMapNumber >= 0) {
			lpObj->MapNumber	= btMAPNUM;
			lpObj->X			= sX;
			lpObj->Y			= sY;
		}
	}
#else
	if (lpObj->MapNumber == MAP_INDEX_PKFIELD) {
		lpObj->MapNumber = MAP_INDEX_VULCANUS;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
	}
#endif
	if ( lpObj->MapNumber == MAP_INDEX_ACHERON )
	{
	//MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
	}

	if( lpObj->MapNumber == 95/*MAP_INDEX_DEBENTER*/ )
	{
		MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
	}
	
	if ( lpObj->m_bMapSvrMoveReq == true )
	{
		short sSVR_CODE = ::g_MapServerManager.CheckMoveMapSvr(lpObj->m_Index, lpObj->m_sDestMapNumber, lpObj->m_sPrevMapSvrCode);

		if ( sSVR_CODE == gGameServerCode )
		{
			lpObj->MapNumber = (BYTE)lpObj->m_sDestMapNumber;
			lpObj->X = lpObj->m_btDestX;
			lpObj->Y = lpObj->m_btDestY;
			lpObj->StartX = lpObj->X;
			lpObj->StartY = lpObj->Y;
		}
		else	
		{
			LogAddC(2, "[MapServerMng] Dest Map Server doesn't have Map [%s][%s] : %d",
				lpObj->AccountID, lpObj->Name, lpObj->m_sDestMapNumber);
			gObjDel(lpObj->m_Index);

			return FALSE;
		}
	}
	
	BYTE attr = MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y);
	int result;
	BYTE x;
	BYTE y;
	BYTE mapNumber;
	BYTE dir;
	short level;
	int gt;
	int userlevel;

	if ( (attr&4) == 4 || (attr&8) == 8 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 243)));
		MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
	}

	mapNumber = MAP_INDEX_RORENCIA;
	userlevel = gObj[aIndex].Level;
	x = lpObj->X;
	y = lpObj->Y;
	dir = lpObj->Dir;
	level = lpObj->Level;
	mapNumber = lpObj->MapNumber;
	gt = 0;

	if(g_iBlockKanturuMapEnter == 1)
	{
		if(gObj[aIndex].MapNumber >= MAP_INDEX_KANTURU1 && gObj[aIndex].MapNumber <= MAP_INDEX_KANTURU_BOSS)
		{
			LogAddTD("[ KANTURU ][ Forced Move From Kanturu to Lorencia ] (%s)(%s) (BeforeMap:%d(%d/%d))",
				gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[aIndex].MapNumber,gObj[aIndex].X,gObj[aIndex].Y);
			lpObj->MapNumber = MAP_INDEX_RORENCIA;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
		}
	}

	if(g_iBlockCastleSiegeMapEnter == 1)
	{
		if(gObj[aIndex].MapNumber == MAP_INDEX_CASTLESIEGE
			|| gObj[aIndex].MapNumber == MAP_INDEX_CASTLEHUNTZONE
			|| gObj[aIndex].MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE
			|| gObj[aIndex].MapNumber == MAP_INDEX_CRYWOLF_SECONDZONE)
		{
			LogAddTD("[ KANTURU ][ Forced Move From CastleSiegeMap to Lorencia ] (%s)(%s) (BeforeMap:%d(%d/%d))",
				gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[aIndex].MapNumber,gObj[aIndex].X,gObj[aIndex].Y);
			lpObj->MapNumber = MAP_INDEX_RORENCIA;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
		}
	}

	if(lpObj->MapNumber == MAP_INDEX_RAKLIONBOSS &&
		(g_Raklion.GetRaklionState() == RAKLION_STATE_CLOSE_DOOR ||
		g_Raklion.GetRaklionState() == RAKLION_STATE_ALL_USER_DIE ||
		g_Raklion.GetRaklionState() == RAKLION_STATE_NOTIFY_4 ||
		g_Raklion.GetRaklionState() == RAKLION_STATE_END)) //season4 add-on
	{
		gt = 286;
	}
	
	if(lpObj->MapNumber == MAP_INDEX_RAKLION) //season4 add-on
	{
		g_RaklionUtil.NotifyRaklionCurrentState(lpObj->m_Index, g_Raklion.GetRaklionState(), g_Raklion.GetRaklionDetailState());
	}

	if ( lpObj->MapNumber == MAP_INDEX_DEVIAS )
	{
		gt = 22;
	}
	else if ( lpObj->MapNumber == MAP_INDEX_NORIA )
	{
		gt = 27;
	}
	else if ( lpObj->MapNumber == MAP_INDEX_LOSTTOWER )
	{
		gt = 42;
	}

	if ( gt > 0 )
	{
		result = gGateC.GetGate(gt, x, y, mapNumber, dir, level);

		if ( result >= 0 )
		{
			if ( mapNumber != MAP_INDEX_SWAMPOFCALMNESS )
			{
				if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER )
				{
					if ( level > 0 )
					{
						level = level / 3 * 2;
					}
				}
			}

			if ( level != 0 && lpObj->Level < level)
			{
				if ( lpObj->Class == 2 ) // Elf
				{
					lpObj->MapNumber = MAP_INDEX_NORIA;
					MapC[lpObj->MapNumber].GetMapPos( lpObj->X, lpObj->Y);
				}
				else
				{
					lpObj->MapNumber = MAP_INDEX_RORENCIA;
					MapC[lpObj->MapNumber].GetMapPos(lpObj->X, lpObj->Y);
				}
			}
		}
		else
		{
			LogAdd("error : %d (%s %d)", result, __FILE__, __LINE__);
		}
	}

	lpMsg->MapX = lpObj->X;
	lpMsg->MapY = lpObj->Y;
	lpObj->TX = lpMsg->MapX;
	lpObj->TY = lpMsg->MapY;
	lpObj->m_OldX = lpObj->X;
	lpObj->m_OldY = lpObj->Y;
	lpObj->Experience = lpMsg->Exp;
	lpObj->Strength = lpMsg->Str ;
	lpObj->Dexterity = lpMsg->Dex ;
	lpObj->Vitality = lpMsg->Vit ;
	lpObj->Energy = lpMsg->Energy ;
	lpObj->Life = lpMsg->Life;
	lpObj->Life /= 10.0f;
	lpObj->MaxLife = lpMsg->MaxLife;
	lpObj->MaxLife /= 10.0f;
	lpObj->Mana = lpMsg->Mana;
	lpObj->Mana /= 10.0f;
	lpObj->MaxMana = lpMsg->MaxMana;
	lpObj->MaxMana /= 10.0f;
	lpObj->Money = lpMsg->Money;
	lpObj->Leadership = lpMsg->Leadership;
	lpObj->ChatLimitTime = lpMsg->ChatLitmitTime;
	lpObj->iFruitPoint = lpMsg->iFruitPoint;

	if ( lpObj->Mana < 1.0f )
	{
		lpObj->Mana = 1.0f;
	}

	if ( lpObj->Life < 1.0f )
	{
		lpObj->Life = 1.0f;
	}

#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());
	LuaMng->Call("setLifePool", lpObj->m_Index);
	LuaMng->Call("setManaPool", lpObj->m_Index);
	ZTLuaManager::Delete(LuaMng);
#else
	lpObj->MaxLife = DCInfo.DefClass[lpObj->Class].Life + (lpObj->Level + lpObj->MasterLevel - 1) * DCInfo.DefClass[lpObj->Class].LevelLife  + ((lpObj->Vitality - DCInfo.DefClass[lpObj->Class].Vitality ) * DCInfo.DefClass[lpObj->Class].VitalityToLife);
	lpObj->MaxMana = DCInfo.DefClass[lpObj->Class].Mana + (lpObj->Level + lpObj->MasterLevel - 1) * DCInfo.DefClass[lpObj->Class].LevelMana  + ((lpObj->Energy - DCInfo.DefClass[lpObj->Class].Energy ) * DCInfo.DefClass[lpObj->Class].EnergyToMana);
#endif

	lpObj->VitalityToLife = DCInfo.DefClass[lpObj->Class].VitalityToLife;
	lpObj->EnergyToMana = DCInfo.DefClass[lpObj->Class].EnergyToMana;

	::gObjSetBP(aIndex);
	lpObj->BP = lpObj->MaxBP / 2;

	lpObj->m_PK_Count = lpMsg->PkCount;
	lpObj->m_PK_Level = lpMsg->PkLevel;
	lpObj->m_PK_Time  = lpMsg->PkTime;

	if ( ::gApplyHeroSystem != FALSE )
	{
		if ( lpObj->m_PK_Level <= 3 )
		{
			lpObj->m_PK_Level = 2;
		}
	}

	if ( lpObj->m_PK_Level == 3 )
	{
		lpObj->m_PK_Count = 0;
		lpObj->m_PK_Time = 0;
	}

	lpObj->MaxRegenTime = 4000;	// Resurrection of the player if die
	lpObj->m_MoveSpeed = 1000;

	memcpy(lpObj->m_Quest, lpMsg->dbQuest, sizeof(lpObj->m_Quest));

	if ( lpObj->m_Quest[0] == 0 )
	{
		memset(lpObj->m_Quest, (BYTE)-1, sizeof(lpObj->m_Quest));
	}

	if ( ::g_QuestInfo.GetQuestState(lpObj, 2) == 2 )	// Ring of Honor
	{
		lpObj->PlusStatQuestClear = true;
	}

	if ( ::g_QuestInfo.GetQuestState(lpObj, 3) == 2 )	// Dark Stone
	{
		lpObj->ComboSkillquestClear = true;
	}

	::gObjSetInventory1Pointer(lpObj);
	lpObj->GuildNumber = 0;
	lpObj->lpGuild = NULL;

	if ( lpObj->Class == CLASS_WIZARD || lpObj->Class == CLASS_ELF || lpObj->Class == CLASS_KNIGHT || lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_SUMMONER || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER)
	{
		BOOL bEnergyBall = TRUE;
		BOOL bForceSkill = TRUE;
		BOOL IsGetInitinityArrowSkill = FALSE;// DarkLord

		gObjMagicAddEnergyCheckOnOff(0);

		for ( n=0;n<MAX_MAGIC;n++)
		{
			itype = lpMsg->dbMagicList[n*MAX_MAGIC_INFO+DBM_TYPE];

			if ( (lpMsg->dbMagicList[n*MAX_MAGIC_INFO+DBM_LEVEL] & 7) > 0 )
			{
				itype = itype * (lpMsg->dbMagicList[n*MAX_MAGIC_INFO+DBM_LEVEL] & 7) + lpMsg->dbMagicList[n*MAX_MAGIC_INFO+DBM_TYPE2];
			}

			if ( itype != (BYTE)-1 )
			{
				if ( g_MasterSkillSystem.CheckMasterLevelSkill(itype) )
				{
					BYTE level = lpMsg->dbMagicList[n*MAX_MAGIC_INFO+DBM_LEVEL] >> 3;

					if( gObjMagicAdd(lpObj,itype,level) > -1 )
					{
						g_MasterSkillSystem.CalcPassiveSkillData(lpObj,itype,level);
					}
				}
				else
				{
					if ( itype == AT_SKILL_ENERGYBALL )
					{
						bEnergyBall = FALSE;
					}

					if ( itype == AT_SKILL_SPEAR )
					{
						bForceSkill = FALSE;
					}

					if ( itype == AT_SKILL_INFINITY_ARROW )
					{
						IsGetInitinityArrowSkill = 1;
						gObjMagicAdd(lpObj,itype,lpMsg->dbMagicList[n*MAX_MAGIC_INFO+DBM_LEVEL]&0xF);
					}

					// Main Engine of Add Skills
					if ( itype != 18 &&
						 itype != 19 &&
						 itype != 20 &&
						 itype != 21 &&
						 itype != 22 &&
						 itype != 23 &&
						 itype != 24 &&
						 itype != 25 &&
						 itype != 49 &&
						 itype != 56 &&
						 itype != 62 &&
						 itype != 76 &&
						 itype != AT_SKILL_SAHAMUT &&
						 itype != AT_SKILL_NEIL &&
						 itype != AT_SKILL_LAGLE &&
						 itype != 54 &&
						 itype != AT_SKILL_RAGEFIGHTER_WEAPON1 &&
						 itype != AT_SKILL_RAGEFIGHTER_WEAPON2 &&
						 itype != AT_SKILL_RAGEFIGHTER_SIEGE &&
						 itype != AT_SKILL_PHOENIXSHOT )
					{
						gObjMagicAdd(lpObj, itype, lpMsg->dbMagicList[n*3+DBM_LEVEL] & 0x0F);
					}
				}
			}
		}

		if ( lpObj->Class == CLASS_WIZARD )	// Wizard
		{
			if ( bEnergyBall == TRUE )
			{
				gObjMagicAdd(lpObj, AT_SKILL_ENERGYBALL, 0);
				bEnergyBall = FALSE;
			}
		}

		if ( lpObj->Class == CLASS_DARKLORD )	// Dark Lord
		{
			if ( bForceSkill == TRUE )
			{
				gObjMagicAdd(lpObj, AT_SKILL_SPEAR, 0);
				bForceSkill = FALSE;
			}
		}

		if ( IsGetInitinityArrowSkill == 0 && lpObj->Class == 2)
		{
			if(lpObj->Level >= g_SkillAdditionInfo.GetInfinityArrowUseLevel())
			{
				if(lpObj->ChangeUP == 1)
				{
					int iAddSkillPosition = gObjMagicAdd(lpObj,AT_SKILL_INFINITY_ARROW,0);

					if(iAddSkillPosition > 0)
					{
						LogAddTD("[%s][%s] Add Infinity Arrow Skill (Character Level : %d)(ChangeUp: %d)",
							lpObj->AccountID,lpObj->Name,lpObj->Level,lpObj->ChangeUP);
					}
				}
			}
		}

		if ( lpObj->Class == CLASS_WIZARD )
		{
			gObjMagicAdd(lpObj, AT_SKILL_JAVALIN, 0);
		}
		else if ( lpObj->Class == CLASS_KNIGHT )
		{
			gObjMagicAdd(lpObj, AT_SKILL_RUSH, 0);
		}
		else if ( lpObj->Class == CLASS_ELF )
		{
			gObjMagicAdd(lpObj, AT_SKILL_DEEPIMPACT, 0);
		}
		else if ( lpObj->Class == CLASS_MAGUMSA )	// Magic Gladiator
		{
			gObjMagicAdd(lpObj, AT_SKILL_ONE_FLASH, 0);
			gObjMagicAdd(lpObj, AT_SKILL_DEATH_CANNON, 0);
		}
		else if ( lpObj->Class == CLASS_DARKLORD )	// Dark Lord
		{
			gObjMagicAdd(lpObj, AT_SKILL_SPACE_SPLIT, 0);	// Fire Blast
			gObjMagicAdd(lpObj, AT_SKILL_BRAND_OF_SKILL, 0);	// Sacrifice - BUG Apply Fix in Skill.vtm coz is not created
		}
		else if ( lpObj->Class == CLASS_SUMMONER )
		{
			gObjMagicAdd(lpObj, AT_SKILL_JAVALIN, 0);
		}
		else if ( lpObj->Class == CLASS_FIGHTER )
		{
			gObjMagicAdd(lpObj, AT_SKILL_RAGEFIGHTER_SIEGE, 0);
		}

		gObjMagicAdd(lpObj,AT_SKILL_STUN,0);
		gObjMagicAdd(lpObj,AT_SKILL_REMOVAL_STUN,0);
		gObjMagicAdd(lpObj,AT_SKILL_ADD_MANA,0);
		gObjMagicAdd(lpObj,AT_SKILL_INVISIBLE,0);
		gObjMagicAdd(lpObj,AT_SKILL_REMOVAL_INVISIBLE,0);
		gObjMagicAdd(lpObj,AT_SKILL_REMOVAL_MAGIC,0);

		::gObjMagicAddEnergyCheckOnOff(TRUE);
	}

	bool bAllItemExist = true;

	if ( lpMsg->DbVersion == 0 ) 
	{
		::DbItemSetInByte(lpObj, lpMsg, 7, &bAllItemExist);

		for ( int i=MAIN_INVENTORY_SIZE;i<INVENTORY_SIZE;i++)
		{
			lpObj->Inventory1[i].Clear();
		}

		memset(lpObj->InventoryMap1+(INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE), (BYTE)-1, PSHOP_MAP_SIZE);	
	}
	else if(lpMsg->DbVersion < 3)
	{
		::DbItemSetInByte(lpObj, lpMsg, 10, &bAllItemExist);
	}
	else
	{
		::DbItemSetInByte(lpObj, lpMsg, 16, &bAllItemExist);
	}


#if (ENABLE_RESET_PERSONALSHOP == 1)
	if ( lpObj->Level <=5 )
	{
		for ( int i=MAIN_INVENTORY_SIZE;i<INVENTORY_SIZE;i++)
		{
			BYTE NewOption[MAX_EXOPTION_SIZE];

			if ( lpObj->Inventory1[i].IsItem() == TRUE )
			{
				for ( int j=0;j<MAX_EXOPTION_SIZE;j++)
				{
					NewOption[j] = 0;
				}

				if ( ( lpObj->Inventory1[i].m_NewOption & 0x20 ) != 0 )
				{
					NewOption[0] = TRUE;
				}

				if ( ( lpObj->Inventory1[i].m_NewOption & 0x10 ) != 0 )
				{
					NewOption[1] = TRUE;
				}

				if ( ( lpObj->Inventory1[i].m_NewOption & 0x08 ) != 0 )
				{
					NewOption[2] = TRUE;
				}

				if ( ( lpObj->Inventory1[i].m_NewOption & 0x04 ) != 0 )
				{
					NewOption[3] = TRUE;
				}

				if ( ( lpObj->Inventory1[i].m_NewOption & 0x02 ) != 0 )
				{
					NewOption[4] = TRUE;
				}

				if ( ( lpObj->Inventory1[i].m_NewOption & 0x01 ) != 0 )
				{
					NewOption[5] = TRUE;
				}

				LogAddTD("[PShop] PShop Item Delete LV <= 5 [%s][%s] [LV:%d] : serial:%d [%s][%d][%d][%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d]",
					lpObj->AccountID, lpObj->Name, lpObj->Level, lpObj->Inventory1[i].m_Number, 
					ItemAttribute[lpObj->Inventory1[i].m_Type].Name, lpObj->Inventory1[i].m_Level,
					lpObj->Inventory1[i].m_Option1, lpObj->Inventory1[i].m_Option2,
					lpObj->Inventory1[i].m_Option3, NewOption[0], NewOption[1], NewOption[2], NewOption[3],
					NewOption[4], NewOption[5], NewOption[6], lpObj->Inventory1[i].m_SetOption);
			}

			lpObj->Inventory1[i].Clear();
		}

		memset(lpObj->InventoryMap1+(INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE), (BYTE)-1, PSHOP_MAP_SIZE);
	}
#endif

	lpObj->Live = TRUE;
	lpObj->Type = OBJ_USER;
	lpObj->TargetNumber = -1;
	lpObj->Connected = PLAYER_PLAYING;
	gObjFindInventoryEquipment(aIndex);	//Lee Added
	::gObjMakePreviewCharSet(aIndex);
	CreateFrustrum(lpObj->X, lpObj->Y, aIndex);
	MapC[lpObj->MapNumber].SetStandAttr(lpObj->X, lpObj->Y);
	lpObj->m_OldX = lpObj->X;
	lpObj->m_OldY = lpObj->Y;
	lpMsg->NextExp = lpObj->NextExp;


	if ( lpObj->Life == 0.0f )
	{
		lpObj->Live = TRUE;
		lpObj->m_State = 4;
		lpObj->DieRegen = TRUE;
		lpObj->RegenTime = GetTickCount();
	}
	else
	{
		lpObj->m_State = 1;
	}

	if ( (lpMsg->CtlCode & 8) == 8 )
		lpObj->Authority = 2;
	
	else 
		lpObj->Authority = 1;
	

	if ( (lpMsg->CtlCode & 0x10 ) == 0x10 )
	{
		lpObj->Authority = 0x20;
		LogAddC(2, "(%s)(%s) Set Event GM", lpObj->AccountID, lpObj->Name);

		//g_Notice.NotifyOnline(lpObj);
	}

	if ( (lpMsg->CtlCode & 0x20 ) == 0x20 )
	{
		lpObj->Authority = 0x20;
		LogAddC(2, "(%s)(%s) Set Event GM", lpObj->AccountID, lpObj->Name);

		//g_Notice.NotifyOnline(lpObj);
	}

	if ( lpObj->Authority == 32 )
	{
		gObjAddBuffEffect(lpObj,BUFF_GM,0,0,0,0,-10);
	}

	lpObj->Penalty = 0;
	g_Notice.NotifyOnline(lpObj);

	if ( lpObj->m_cAccountItemBlock != 0 )
	{
		lpObj->Penalty |= 8;
	}

	if ( (lpMsg->CtlCode & 2) == 2 )
	{
		lpObj->Penalty |= 4;
		LogAddL("Penalty : Item Don't touch  %s %s", lpObj->AccountID, lpObj->Name);
	}

	LogAddTD(lMsg.Get(MSGGET(1, 246)), lpObj->m_Index, lpObj->AccountID, lpObj->Name);

	BYTE weather = MapC[lpObj->MapNumber].GetWeather();

	if ( (weather >> 4) > 0 )
	{
		CGWeatherSend(aIndex, weather);
	}
#if( __4GAMERS__ == 0 )
	if ( DragonEvent->GetState() > 0 )
	{
		if ( DragonEvent->GetMapNumber() == lpObj->MapNumber )
		{
			::GCMapEventStateSend(lpObj->MapNumber, 1, 1);
		}
	}
#endif

#if (GS_CASTLE == 1)
	if ( lpObj->MapNumber == MAP_INDEX_CASTLESIEGE )
	{
		GCAnsCsNotifyStart(lpObj->m_Index,(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE) ? TRUE : FALSE);

		if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			g_CastleSiege.NotifyCsSelfLeftTime(lpObj->m_Index);
		}
	}
	else if ( lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
	{
		if( g_Crywolf.GetCrywolfState() != CRYWOLF_STATE_NONE )
		{
			g_Crywolf.NotifyCrywolfCurrentState();
		}
	}
#endif

	::gEledoradoEvent.CheckGoldDercon(lpObj->MapNumber);

	if ( bAllItemExist == false )
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx((TNotice *)&pNotice, 1, "없는 아이템 발견 !!!");	// Deathway need Translation
		DataSend(lpObj->m_Index, (UCHAR *)&pNotice, pNotice.h.size);
	}

	if ( lpObj->Level <= g_iCharacterRecuperationMaxLevel )
	{
		lpObj->m_iAutoRecuperationTime = GetTickCount();
	}
	else
	{
		lpObj->m_iAutoRecuperationTime = 0;
	}

	lpObj->dwShieldAutoRefillTimer = GetTickCount();

	LogAddTD("[ShieldSystem][CalcSDPoint] [%s][%s] user SD Gage : %d", lpObj->AccountID,lpObj->Name,lpObj->iMaxShield+lpObj->iAddShield);

	g_CashShop.AddUser(lpObj);
	g_CashItemPeriodSystem.GDReqPeriodItemList(lpObj);
#if (ENABLETEST_MUUN == 1)
	g_CMuunSystem.AddMuunItemPeriodInfo(lpObj);
#endif
	//g_DuelManager.RemoveSpectatorAtLogOn(lpObj);

#if (__CUSTOM__ == 1)
	lpObj->iResetCount = lpMsg->iResetCount;
#if( __4GAMERS__ == 1 )
	g_ResetDebug.Output("[%s] [%s] Load character, reset: %d",lpObj->AccountID, lpObj->Name, lpObj->iResetCount);
#endif
#endif

#ifdef __NOVUS__	//Test
	lpObj->m_ExPointPvP		= 500;
	lpObj->m_ExPointEvent	= 500;
	lpObj->m_ExPointGens	= 500;
#endif
	return TRUE;
}


BOOL gObjCanItemTouch(LPOBJ lpObj, int type )
{
	if ( (lpObj->Penalty &4) == 4 )
	{
		return false;
	}

	if ( (lpObj->Penalty &8) == 8 )
	{
		if ( type == 6 || type == 8 )
		{
			return true;
		}
		return false;
	}
	return true;
}








void gObjMagicTextSave(LPOBJ lpObj)
{
	int total=0;

	for ( int n=0;n<MAGIC_SIZE;n++)
	{
		if ( lpObj->Magic[n].IsMagic() == TRUE )
		{
			LogAddTD(lMsg.Get(MSGGET(1, 247)), lpObj->AccountID, lpObj->Name, n, lpObj->Magic[n].m_Skill);
			total++;
		}
	}
}








void ItemIsBufExOption(BYTE * buf, CItem * lpItem)
{
	for ( int i=0;i<MAX_EXOPTION_SIZE;i++ )
	{
		buf[i] = 0;
	}

	if ( lpItem->IsExtLifeAdd() != FALSE )
	{
		buf[0] = TRUE;
	}

	if ( lpItem->IsExtManaAdd() != FALSE )
	{
		buf[1] = TRUE;
	}

	if ( lpItem->IsExtDamageMinus() != FALSE )
	{
		buf[2] = TRUE;
	}

	if ( lpItem->IsExtDamageReflect() != FALSE )
	{
		buf[3] = TRUE;
	}

	if ( lpItem->IsExtDefenseSuccessfull() != FALSE )
	{
		buf[4] = TRUE;
	}

	if ( lpItem->IsExtMonsterMoney() != FALSE )
	{
		buf[5] = TRUE;
	}

}







void gObjStatTextSave(LPOBJ lpObj)
{
	LogAddTD("[%s][%s] CharInfoSave : Class=%d Level=%d LVPoint=%d Exp=%u Str=%d Dex=%d Vit=%d Energy=%d Leadership:%d Map=%d Pk=%d",
		lpObj->AccountID, lpObj->Name, lpObj->DbClass,lpObj->Level, lpObj->LevelUpPoint, lpObj->Experience, 
		lpObj->Strength, lpObj->Dexterity, lpObj->Vitality, lpObj->Energy, lpObj->Leadership, lpObj->MapNumber,
		lpObj->m_PK_Level);
}







void gObjItemTextSave(LPOBJ lpObj)
{
	BYTE NewOption[MAX_EXOPTION_SIZE];
	int n;

	for ( n=0;n<INVENTORY_SIZE;n++)
	{
		if ( lpObj->pInventory[n].IsItem() == TRUE )
		{
			if ( lpObj->pInventory[n].m_serial != 0 )
			{
				ItemIsBufExOption(NewOption, &lpObj->pInventory[n]);
				LogAddTD(lMsg.Get(MSGGET(1, 248)), lpObj->AccountID, lpObj->Name, n, lpObj->pInventory[n].GetName(),
					lpObj->pInventory[n].m_Level, lpObj->pInventory[n].m_Option1,
					lpObj->pInventory[n].m_Option2, lpObj->pInventory[n].m_Option3,
					lpObj->pInventory[n].m_Number, (BYTE)lpObj->pInventory[n].m_Durability,
					NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6],
					lpObj->pInventory[n].m_SetOption,lpObj->pInventory[n].m_ItemOptionEx >> 7,g_kJewelOfHarmonySystem.GetItemStrengthenOption(&lpObj->pInventory[n]),g_kJewelOfHarmonySystem.GetItemOptionLevel(&lpObj->pInventory[n]));
			}
		}
	}

	LogAddTD(lMsg.Get(MSGGET(1, 249)), lpObj->AccountID, lpObj->Name, lpObj->Money);
}








void gObjWarehouseTextSave(LPOBJ lpObj)
{
	BYTE NewOption[MAX_EXOPTION_SIZE];
	int n;

	for ( n=0;n<WAREHOUSE_SIZE;n++)
	{
		if ( lpObj->pWarehouse[n].IsItem() == TRUE )
		{
			if ( lpObj->pWarehouse[n].m_serial != 0 )
			{
				ItemIsBufExOption(NewOption, &lpObj->pWarehouse[n]);
				LogAddTD(lMsg.Get(MSGGET(1, 250)), lpObj->AccountID, lpObj->Name, n, lpObj->pWarehouse[n].GetName(),
					lpObj->pWarehouse[n].m_Level, lpObj->pWarehouse[n].m_Option1,
					lpObj->pWarehouse[n].m_Option2, lpObj->pWarehouse[n].m_Option3,
					lpObj->pWarehouse[n].m_Number, (BYTE)lpObj->pWarehouse[n].m_Durability,
					NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6],
					lpObj->pWarehouse[n].m_SetOption, lpObj->pWarehouse[n].m_ItemOptionEx>>7,
					g_kJewelOfHarmonySystem.GetItemStrengthenOption(&lpObj->pWarehouse[n]),
					g_kJewelOfHarmonySystem.GetItemOptionLevel(&lpObj->pWarehouse[n]));
			}
		}
	}

	LogAddTD(lMsg.Get(MSGGET(1, 251)), lpObj->AccountID, lpObj->Name, lpObj->WarehouseMoney);
}


BOOL gObjSetPosMonster(int aIndex, int PosTableNum)
{
	if ( (  (aIndex<0)? FALSE: (aIndex > OBJMAX-1)? FALSE: TRUE ) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	
	if ( (  (PosTableNum<0)? FALSE: (PosTableNum > OBJ_MAXMONSTER-1)? FALSE: TRUE ) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	LPOBJ lpObj = &gObj[aIndex];
	lpObj->m_PosNum = PosTableNum;
	lpObj->X = gMSetBase.m_Mp[PosTableNum].m_X;
	lpObj->Y = gMSetBase.m_Mp[PosTableNum].m_Y;
	lpObj->MapNumber = gMSetBase.m_Mp[PosTableNum].m_MapNumber;
	lpObj->TX = lpObj->X;
	lpObj->TY = lpObj->Y;
	lpObj->m_OldX = lpObj->X;
	lpObj->m_OldY = lpObj->Y;
	lpObj->Dir = gMSetBase.m_Mp[PosTableNum].m_Dir;
	lpObj->StartX = lpObj->X;
	lpObj->StartY = lpObj->Y;

	if ( 
#if( __4GAMERS__ == 0 )	
		lpObj->Class == 44 ||  
#endif
		lpObj->Class == 53 ||  lpObj->Class == 54 ||  lpObj->Class == 55 ||  lpObj->Class == 56)
	{

	}
	else if (  lpObj->Class >= 78 &&  lpObj->Class <= 83)
	{

	}
	else if ( gMSetBase.GetPosition(PosTableNum, lpObj->MapNumber, lpObj->X, lpObj->Y) == FALSE )
	{
//		LogAdd(lMsg.Get(MSGGET(1, 252)), __FILE__, __LINE__);
//		LogAdd("error-L2 : %d class : %d", PosTableNum, lpObj->Class);

//		LogAdd("%d %d %d %d",
//			gMSetBase.m_Mp[PosTableNum].m_Type,gMSetBase.m_Mp[PosTableNum].m_MapNumber,
//			gMSetBase.m_Mp[PosTableNum].m_X,gMSetBase.m_Mp[PosTableNum].m_Y);
		return FALSE;
	}

	lpObj->TX = lpObj->X;
	lpObj->TY = lpObj->Y;
	lpObj->m_OldX = lpObj->X;
	lpObj->m_OldY = lpObj->Y;
	lpObj->Dir = gMSetBase.m_Mp[PosTableNum].m_Dir;
	lpObj->StartX = lpObj->X;
	lpObj->StartY = lpObj->Y;
	return true;
}









BOOL gObjSetMonster(int aIndex, int MonsterClass)
{
	int Level;
	LPOBJ lpObj;

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return false;
	}

	lpObj = &gObj[aIndex];
	lpObj->ConnectCheckTime = GetTickCount();
	lpObj->ShopNumber = -1;
	lpObj->TargetNumber = -1;
	lpObj->m_RecallMon = -1;

	int ShopID = gMSetBase.GetShopID(gObj[aIndex].m_PosNum);

	if( ShopID == -1 )
	{
		switch ( MonsterClass )
		{
			case 200:
				gBSGround[0]->m_BallIndex = aIndex;
				break;
			case 251:
				lpObj->ShopNumber = 0;
				break;
			case 255:
				lpObj->ShopNumber = 1;
				break;
			case 254:
				lpObj->ShopNumber = 2;
				break;
			case 250:
				lpObj->ShopNumber = 3;
				break;
			case 248:
				lpObj->ShopNumber = 4;
				break;
			case 239:
			case 253:
				lpObj->ShopNumber = 5;
				break;
			case 244:
				lpObj->ShopNumber = 6;
				break;
			case 245:
				lpObj->ShopNumber = 7;
				break;
			case 246:
				lpObj->ShopNumber = 8;
				break;
			case 243:
				lpObj->ShopNumber = 9;
				break;
			case 242:
				lpObj->ShopNumber = 10;
				break;
			case 231:
				lpObj->ShopNumber = 12;
				break;
			case 230:
				lpObj->ShopNumber = 11;
				break;
			case 240:
				lpObj->ShopNumber = 100;
				break;
			case 238:
				lpObj->ShopNumber = 101;
				break;
			case 236:
				lpObj->ShopNumber = 102;
				break;
			case 259:
				lpObj->ShopNumber = 5;
				break;
			case 368:
			case 369:
			case 370:
				lpObj->ShopNumber = 101;
				break;
			case 376:
				lpObj->ShopNumber = 5;
				break;
			case 377:
				lpObj->ShopNumber = 5;
				break;
			case 379:
				lpObj->ShopNumber = 13;
				break;
			case 415:
				lpObj->ShopNumber = 15;
				break;
			case 416:
				lpObj->ShopNumber = 16;
				break;
			case 417:
				lpObj->ShopNumber = 17;
				break;
			case 450:
				lpObj->ShopNumber = 101;
				break;
			case 452:
				lpObj->ShopNumber = 101;
				break;
			case 453:
				lpObj->ShopNumber = 101;
				break;
			case 478:
				lpObj->ShopNumber = 101;
				break;
			case 492:
				lpObj->ShopNumber = 18;
				break;
			case 577:
				lpObj->ShopNumber = 19;
				break;
			case 578:
				lpObj->ShopNumber = 20;
				break;
			case 583:
				lpObj->ShopNumber = 21;
				break;
			case 604:
				lpObj->ShopNumber = 22;
				break;
		}
	}
	else
	{
		lpObj->ShopNumber = ShopID;
	}

	lpObj->Connected = PLAYER_PLAYING;
	lpObj->Live = FALSE;
	lpObj->m_State = 0;

	if ( 
#if( __4GAMERS__ == 0 )
		MonsterClass == 44 ||
#endif
		 MonsterClass == 54
		|| MonsterClass == 53
		|| MonsterClass == 55
		|| MonsterClass == 56
		|| MonsterClass == 76
		|| MonsterClass == 78
		|| MonsterClass == 79
		|| MonsterClass == 80
		|| MonsterClass == 81
		|| MonsterClass == 82
		|| MonsterClass == 83
		|| MonsterClass == 493
		|| MonsterClass == 494
		|| MonsterClass == 495
		|| MonsterClass == 496
		|| MonsterClass == 497
		|| MonsterClass == 498
		|| MonsterClass == 499
		|| MonsterClass == 500
		|| MonsterClass == 501
		|| MonsterClass == 502)
	{

	}
	else
	{
		lpObj->Live = TRUE;
		lpObj->m_State = 1;
	}

	if ( MonsterClass == 365 )
	{
		if( g_bNewYearMonsterEvent != FALSE )
		{
			lpObj->Live = TRUE;
			lpObj->m_State = 1;
		}
		else
		{
			lpObj->Live = FALSE;
			lpObj->m_State = 0;
		}
	}

	if ( MonsterClass >= 204 && MonsterClass <= 259
		|| MonsterClass >= 367 && MonsterClass <= 371
		|| MonsterClass >= 468 && MonsterClass <= 475 )
	{
		lpObj->Type = OBJ_NPC;
	}
	else
	{
		lpObj->Type = OBJ_MONSTER;
	}

	switch(MonsterClass)
	{
	case 375:
	case 376:
	case 377:
	case 379:
	case 380:
	case 381:
	case 382:
	case 383:
	case 384:
	case 385:
	case 406:
	case 407:
	case 408:
	case 415:
	case 416:
	case 417:
	case 450:
	case 452:
	case 453:
	case 465:
	case 467:
	case 478:
	case 479:
	case 492:
	case 522:
	case 540:
	case 541:
	case 542:
	case 543:
	case 544:
	case 547:
	case 566:
	case 579:
	case 580:
	case 581:
	case 582:
	case 583:
	case 584:
	case 604:
	case 651:
		lpObj->Type = OBJ_NPC;
		break;
	}

	if( MonsterClass == 385 )
	{
		g_IllusionTemple.SetMummyNpc(aIndex);
	}

	if ( MonsterClass == 77 )
	{
		int iSL = gObjAddMonster(MAP_INDEX_ICARUS);

		if ( iSL >= 0 )
		{
			if ( gObjSetMonster(iSL, 76) == FALSE )
			{
				MsgBox("천공보스몹 설정 실패");
				return false;
			}

			lpObj->m_SkyBossMonSheildLinkIndex = iSL;
		}
		else
		{
			MsgBox("천공보스몹 설정 실패");
			return false;
		}
	}

	lpObj->Class = MonsterClass;

	Level = lpObj->Level;
	LPMONSTER_ATTRIBUTE lpm = gMAttr.GetAttr(lpObj->Class);

	if ( lpm == NULL )
	{
		MsgBox(lMsg.Get(MSGGET(1, 253) ), lpObj->Class, __FILE__, __LINE__);
		return false;
	}

	strncpy(gObj[aIndex].Name, lpm->m_Name , 10 );

	if (lpm->m_Level == 0)
	{
		LogAdd(lMsg.Get( MSGGET(1, 254)), lpObj->Class, __FILE__, __LINE__);
	}

	lpObj->Level = lpm->m_Level;
	lpObj->m_AttackSpeed = lpm->m_AttackSpeed;
	lpObj->m_AttackDamageMin = lpm->m_DamageMin;
	lpObj->m_AttackDamageMax = lpm->m_DamageMax;
	lpObj->m_Defense = lpm->m_Defense;
	lpObj->m_MagicDefense = lpm->m_MagicDefense;
	lpObj->m_AttackRating = lpm->m_AttackRating;
	lpObj->m_SuccessfulBlocking = lpm->m_Successfulblocking;
	lpObj->Life = (float)lpm->m_Hp;
	lpObj->MaxLife = (float)lpm->m_Hp;
	lpObj->Mana = (float)lpm->m_Mp;
	lpObj->MaxMana = (float)lpm->m_Mp;
	lpObj->m_MoveRange = lpm->m_MoveRange;
	lpObj->m_AttackSpeed = lpm->m_AttackSpeed;
	lpObj->m_MoveSpeed = lpm->m_MoveSpeed;
	lpObj->MaxRegenTime = lpm->m_RegenTime * 1000;
	lpObj->m_AttackRange = lpm->m_AttackRange;
	lpObj->m_ViewRange = lpm->m_ViewRange;
	lpObj->m_Attribute = lpm->m_Attribute;
	lpObj->m_AttackType = lpm->m_AttackType;
	lpObj->m_ItemRate = lpm->m_ItemRate;
	lpObj->m_MoneyRate = lpm->m_MoneyRate;
	lpObj->Dexterity = 0;
	
	int iPentagramMainAttribute = 0;
	gMSetBase.GetPentagramMainAttribute(lpObj->m_PosNum, &iPentagramMainAttribute);

	if( iPentagramMainAttribute > 0 )
	{
		lpObj->m_btAttribute = iPentagramMainAttribute;
		lpObj->m_ElementalDamageMin = lpm->m_ElementDmgMin;
		lpObj->m_ElementalDamageMax = lpm->m_ElementDmgMax;
		lpObj->m_ElementalDefense = lpm->m_ElementDefense;
		lpObj->m_ElementalDefense /= 2;
		lpObj->m_ElementalAttackRate = lpm->m_ElementAttackRating;
		lpObj->m_ElementalDefenseRate = lpm->m_ElementBlockRating;
	}

	/*if( lpm->m_btElement == (BYTE)-1 )
	{
		//lpObj->m_btAttribute = ELEMENT_NONE;
		//lpObj->m_ElementalDamageMin = 0;
		//lpObj->m_ElementalDamageMax = 0;
		//lpObj->m_ElementalDefense = lpm->m_Defense;
		//lpObj->m_ElementalAttackRate = lpm->m_AttackRating;
		//lpObj->m_ElementalDefenseRate = lpm->m_Successfulblocking;
	}
	else
	{
		lpObj->m_btAttribute = (lpm->m_btElement == ELEMENT_NONE) ? (1+rand()%5) : lpm->m_btElement;
		lpObj->m_ElementalDamageMin = lpm->m_ElementDmgMin;
		lpObj->m_ElementalDamageMax = lpm->m_ElementDmgMax;
		lpObj->m_ElementalDefense = lpm->m_ElementDefense;
		lpObj->m_ElementalDefense /= 2;
		lpObj->m_ElementalAttackRate = lpm->m_ElementAttackRating;
		lpObj->m_ElementalDefenseRate = lpm->m_ElementBlockRating;
	}*/

	lpObj->m_iScriptMaxLife = lpm->m_iScriptHP;
	lpObj->m_iBasicAI = lpm->m_iAINumber;
	lpObj->m_iCurrentAI = lpm->m_iAINumber;
	lpObj->m_iCurrentAIState = 0;
	lpObj->m_iLastAIRunTime = 0;
	lpObj->m_iGroupNumber = 0;
	lpObj->m_iSubGroupNumber = 0;
	lpObj->m_iGroupMemberGuid = -1;
	lpObj->m_iRegenType = 0;

	std::map<int,CUSTOM_MONSTER_ATTR*>::iterator _It = gCMAttr.m_MonsterData.find(MonsterClass);

	if( _It == gCMAttr.m_MonsterData.end() )
	{
		lpObj->Life *= gCMAttr.m_DefaultData.HPMul;
		lpObj->MaxLife *= gCMAttr.m_DefaultData.HPMul;

		lpObj->m_AttackDamageMin = (int)(lpObj->m_AttackDamageMin*gCMAttr.m_DefaultData.DmgMul);
		lpObj->m_AttackDamageMax = (int)(lpObj->m_AttackDamageMax*gCMAttr.m_DefaultData.DmgMul);
		//lpObj->m_ElementalDamageMin *= gCMAttr.m_DefaultData.DmgMul;
		//lpObj->m_ElementalDamageMax *= gCMAttr.m_DefaultData.DmgMul;

		lpObj->m_Defense = (int)(lpObj->m_Defense*gCMAttr.m_DefaultData.DefMul);
		lpObj->m_MagicDefense = (int)(lpObj->m_MagicDefense*gCMAttr.m_DefaultData.DefMul);
		//lpObj->m_ElementalDefense *= gCMAttr.m_DefaultData.DefMul;

		lpObj->m_AttackRating = (int)(lpObj->m_AttackRating*gCMAttr.m_DefaultData.ARateMul);
		//lpObj->m_ElementalAttackRate *= gCMAttr.m_DefaultData.ARateMul;
		lpObj->m_SuccessfulBlocking = (int)(lpObj->m_SuccessfulBlocking*gCMAttr.m_DefaultData.DRateMul);
		//lpObj->m_ElementalDefenseRate *= gCMAttr.m_DefaultData.DRateMul;
	}
	else
	{
		CUSTOM_MONSTER_ATTR* pMonInfo = _It->second;

		lpObj->Life *= pMonInfo->HPMul;
		lpObj->MaxLife *= pMonInfo->HPMul;

		lpObj->m_AttackDamageMin = (int)(lpObj->m_AttackDamageMin*pMonInfo->DmgMul);
		lpObj->m_AttackDamageMax = (int)(lpObj->m_AttackDamageMax*pMonInfo->DmgMul);
		//lpObj->m_ElementalDamageMin *= pMonInfo->DmgMul;
		//lpObj->m_ElementalDamageMax *= pMonInfo->DmgMul;

		lpObj->m_Defense = (int)(lpObj->m_Defense*pMonInfo->DefMul);
		lpObj->m_MagicDefense = (int)(lpObj->m_MagicDefense*pMonInfo->DefMul);
		//lpObj->m_ElementalDefense *= pMonInfo->DefMul;

		lpObj->m_AttackRating = (int)(lpObj->m_AttackRating*pMonInfo->ARateMul);
		//lpObj->m_ElementalAttackRate *= pMonInfo->ARateMul;
		lpObj->m_SuccessfulBlocking = (int)(lpObj->m_SuccessfulBlocking*pMonInfo->DRateMul);
		//lpObj->m_ElementalDefenseRate *= pMonInfo->DRateMul;
	}

	lpObj->m_Agro.ResetAll();

	lpObj->m_iLastAutomataRuntime = 0;
	lpObj->m_iLastAutomataDelay = 0;

	lpObj->pInventoryCount[0] = 0;
	memcpy(lpObj->m_Resistance, lpm->m_Resistance, MAX_RESISTENCE_TYPE );
	gObjSetInventory1Pointer(lpObj);

	if ( lpObj->m_AttackType != 0 )
	{
		if ( lpObj->m_AttackType >= 100 )
		{
			gObjMonsterMagicAdd(lpObj, (BYTE)(lpObj->m_AttackType - 100), 1);
		}
		else
		{
			gObjMonsterMagicAdd(lpObj, (BYTE)(lpObj->m_AttackType), 1);
		}

		if ( lpObj->Class == 144 || lpObj->Class == 174 || lpObj->Class == 182 || lpObj->Class == 190 || lpObj->Class == 260 || lpObj->Class == 268 )
		{
			gObjMonsterMagicAdd(lpObj, 17, 1);
		}

		if ( lpObj->Class == 145 
			|| lpObj->Class == 175 
			|| lpObj->Class == 183 
			|| lpObj->Class == 191 
			|| lpObj->Class == 261 
			|| lpObj->Class == 269 
			|| lpObj->Class == 146 
			|| lpObj->Class == 176 
			|| lpObj->Class == 184 
			|| lpObj->Class == 192 
			|| lpObj->Class == 262 
			|| lpObj->Class == 270 
			|| lpObj->Class == 147 
			|| lpObj->Class == 177 
			|| lpObj->Class == 185 
			|| lpObj->Class == 193 
			|| lpObj->Class == 263 
			|| lpObj->Class == 271 
			|| lpObj->Class == 148
			|| lpObj->Class == 178
			|| lpObj->Class == 186
			|| lpObj->Class == 194
			|| lpObj->Class == 264
			|| lpObj->Class == 272
			|| lpObj->Class == 160
			|| lpObj->Class == 180
			|| lpObj->Class == 188
			|| lpObj->Class == 196
			|| lpObj->Class == 266
			|| lpObj->Class == 274 )
		{
			gObjMonsterMagicAdd(lpObj, 17, 1);
		}

		if ( lpObj->Class == 161 || lpObj->Class == 181 || lpObj->Class == 189 || lpObj->Class == 197 || lpObj->Class == 267 || lpObj->Class == 275 )
		{
			gObjMonsterMagicAdd(lpObj, 200, 1);
			gObjMonsterMagicAdd(lpObj, 17, 1);
			gObjMonsterMagicAdd(lpObj, 1, 1);
			gObjMonsterMagicAdd(lpObj, 201, 1);
			gObjMonsterMagicAdd(lpObj, 202, 1);
			gObjMonsterMagicAdd(lpObj, 55, 1);
		}

		if (  lpObj->Class == 149 || lpObj->Class == 179 || lpObj->Class == 187 || lpObj->Class == 195 || lpObj->Class == 265 || lpObj->Class == 273 )
		{
			gObjMonsterMagicAdd(lpObj, 17, 1);
			gObjMonsterMagicAdd(lpObj, 1, 1);
		}

		if ( lpObj->Class == 66 || lpObj->Class == 73 || lpObj->Class == 77 || lpObj->Class == 89 || lpObj->Class == 95 || lpObj->Class == 112 || lpObj->Class == 118 || lpObj->Class == 124 || lpObj->Class == 130 || lpObj->Class == 143 || lpObj->Class == 433 || lpObj->Class == 163 || lpObj->Class == 165 || lpObj->Class == 167 || lpObj->Class == 169 || lpObj->Class == 171 || lpObj->Class == 173 || lpObj->Class == 427)
		{
			gObjMonsterMagicAdd(lpObj, 17, 1);
		}
		
		if( lpObj->Class == 89 || lpObj->Class == 95 || lpObj->Class == 112 || lpObj->Class == 118 || lpObj->Class == 124 || lpObj->Class == 130 || lpObj->Class == 143 || lpObj->Class == 433 )
		{
			gObjMonsterMagicAdd(lpObj, 3, 1);
		}

		if( lpObj->Class == 561 )	//Medusa Added Leee
		{
			gObjMonsterMagicAdd(lpObj, 238, 1);
			gObjMonsterMagicAdd(lpObj, 237, 1);
			gObjMonsterMagicAdd(lpObj, 38, 1);
			gObjMonsterMagicAdd(lpObj, 9, 1);
		}
	}

	gObjGiveItemSearch(lpObj, lpm->m_MaxItemLevel);
	gObjGiveItemWarehouseSearch(lpObj, lpm->m_MaxItemLevel);
	CreateFrustrum(lpObj->X, lpObj->Y, aIndex);
	MapC[lpObj->MapNumber].SetStandAttr(lpObj->X, lpObj->Y);
	lpObj->m_OldX = lpObj->X;
	lpObj->m_OldY = lpObj->Y;
	lpObj->LastAttackerID = -1;
	return true;
}


void gObjDestroy(unsigned int aSocket,int client)
{
	if ( client < 0 || client > OBJMAX-1)
	{
		return;
	}

	gObj[client].Connected = PLAYER_EMPTY;
}


short gObjAddSearch(SOCKET aSocket, char* ip)
{
	int count;
	int totalcount = 0;

	if ( gDisconnect == 1 )
	{
		return -1;
	}

	count = gObjCount;
	
	while ( true )
	{
		if ( gObj[count].Connected == PLAYER_EMPTY )
		{
			return count;
		}

		count++;

		if ( count >= OBJMAX )
		{
			count = OBJ_STARTUSERINDEX;
		}
		totalcount++;
		if ( totalcount >= OBJMAXUSER )
		{
			break;
		}
	}
	return -1;
}

short gObjAdd(SOCKET aSocket, char* ip, int aIndex)
{
	if ( gObj[aIndex].Connected != PLAYER_EMPTY )
	{
		return -1;
	}

	gObjCharZeroSet(aIndex);
	gNSerialCheck[aIndex].init();
	gObj[aIndex].LoginMsgSnd = FALSE;
	gObj[aIndex].LoginMsgCount = 0;
	memset(gObj[aIndex].AccountID, 0, sizeof(gObj[0].AccountID)-1);
	gObj[aIndex].m_Index = aIndex;
	gObj[aIndex].m_socket = aSocket;
	gObj[aIndex].ConnectCheckTime = GetTickCount();
	gObj[aIndex].AutoSaveTime = gObj[aIndex].ConnectCheckTime;
	gObj[aIndex].Connected = PLAYER_CONNECTED;
	gObj[aIndex].LoginMsgCount = 0;
	gObj[aIndex].m_AccountMaxLevel = 0;
	gObj[aIndex].Magumsa = 0;
	gObj[aIndex].Type = OBJ_USER;
	gObj[aIndex].pWarehouseExtend = 0;

	gObj[aIndex].m_iDuelRoom = -1;

	m_ObjBill[aIndex].Init();
	strcpy_s(gObj[aIndex].Ip_addr, sizeof(gObj[aIndex].Ip_addr), ip);
	LogAddTD("connect : [%d][%s]", aIndex, ip);
	gObjCount++;
	
	if ( gObjCount >= OBJMAX )
	{
		gObjCount = OBJ_STARTUSERINDEX;
	}

	return aIndex;
}


short gObjMonsterRecall(int iMapNumber)
{
	int number = gObjAddMonster(iMapNumber);

	if ( number < 0 )
	{
		return -1;
	}
	return -1;
}


short gObjAddMonster(int iMapNumber)
{

	if (g_MapServerManager.CheckMapCanMove(iMapNumber) == FALSE )
	{
		return -1;
	}

	int count;
	int totalcount=0;

	count = gObjMonCount;

	while ( true )
	{
		if ( gObj[count].Connected == PLAYER_EMPTY )
		{
			gObjCharZeroSet(count);
			gObj[count].m_Index = count;
			gObj[count].Connected = PLAYER_CONNECTED;
			gObjMonCount++;

			if ( gObjMonCount >= OBJ_MAXMONSTER )
			{
				gObjMonCount = 0;
			}
			return count;
		}

		count ++;
		
		if ( count >= OBJ_MAXMONSTER )
		{
			count = 0;
		}

		totalcount++;

		if ( totalcount >= OBJ_MAXMONSTER )
		{
			LogAdd( lMsg.Get( MSGGET ( 1, 255 )), __FILE__, __LINE__ );
			return -1;
		}
	}
	return -1;
}







short gObjAddCallMon()
{
	int count;
	int totalcount = 0;

	count = gObjCallMonCount;

	while ( true )
	{
		if ( gObj[count].Connected == PLAYER_EMPTY )
		{
			gObjCharZeroSet(count);
			gObj[count].m_Index = count;
			gObj[count].Connected = PLAYER_CONNECTED;
			gObjCallMonCount++;

			if ( gObjCallMonCount >= OBJ_STARTUSERINDEX )
			{
				gObjCallMonCount = OBJ_MAXMONSTER;

			}
			return count;
		}

		count++;

		if ( count >= OBJ_STARTUSERINDEX )
		{
			count = OBJ_MAXMONSTER;
		}

		totalcount++;

		if ( totalcount >= OBJ_CALLMONCOUNT )
		{
			LogAdd(lMsg.Get(MSGGET(1, 255)), __FILE__, __LINE__);
			return -1;
		}
	}

	return -1;

}







void gObjUserKill(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->CloseCount < 1 )
	{
		lpObj->CloseCount = 6;
		lpObj->CloseType = 0;
	}
}






void gObjAllLogOut()
{
	static BOOL bAllLogOut = FALSE;

	if ( bAllLogOut == 0 )
	{
		bAllLogOut = 1;
		gObjAllDisconnect();
		Sleep(1000);
		LogAddC(2, lMsg.Get( MSGGET(2, 0)));
	}
}






void gObjAllDisconnect()
{
	gDisconnect = TRUE;

	for ( int n=OBJ_STARTUSERINDEX; n< OBJMAX; n++)
	{
		if ( gObj[n].Connected > PLAYER_EMPTY )
		{
			if ( gObj[n].Type == OBJ_USER )
			{
#if( ENABLE_CUSTOM_OFFLINETRADE )
				if( gObj[n].bOffTrade )
				{
					gObjDel(n);
				}
				else
				{
					CloseClient(n);
				}
#else
				CloseClient(n);
#endif
			}
		}
	}
}






void gObjTradeSave(LPOBJ lpObj, int index)
{
	if ( lpObj->m_IfState.use != 0 && lpObj->m_IfState.type == 1 )
	{
		int tObjNum = lpObj->TargetNumber;

		if ( tObjNum > 0 )
		{
			char szTemp[256];

			CGTradeResult(tObjNum, 0);
			wsprintf(szTemp, lMsg.Get(MSGGET(4, 77)), lpObj->Name);
			GCServerMsgStringSend((char*)szTemp, tObjNum, 1);
			gObjTradeCancel(tObjNum);
			LogAdd(lMsg.Get(MSGGET(2, 1)), gObj[tObjNum].AccountID, gObj[tObjNum].Name);
		}

		gObjTradeCancel( index );
		LogAdd(lMsg.Get(MSGGET(2,2)), lpObj->AccountID, lpObj->Name);
	}
}






void gObjBillRequest(LPOBJ lpObj)
{
#ifndef SILVER
	char szMsg[128];

	int PayCode = m_ObjBill[lpObj->m_Index].GetPayCode();

	if ( PayCode != 0 )
	{
		char szYear[5] = "";
		char szMonth[3] = "";
		char szDay[3] = "";
		char szHour[3] = "";
		char szMin[3] = "";

		strncpy(szYear, m_ObjBill[lpObj->m_Index].GetEndsDays(), 4);
		strncpy(szMonth, m_ObjBill[lpObj->m_Index].GetEndsDays()+4, 2);
		strncpy(szDay, m_ObjBill[lpObj->m_Index].GetEndsDays()+6, 2);
		strncpy(szHour, m_ObjBill[lpObj->m_Index].GetEndsDays()+8, 2);
		strncpy(szMin, m_ObjBill[lpObj->m_Index].GetEndsDays()+10, 2);

		wsprintf(szMsg, lMsg.Get(MSGGET(5, 220)), m_ObjBill[lpObj->m_Index].GetEndTime(), szYear, szMonth,
			szDay, szHour, szMin);

		LogAddTD("[%s][%s] BillType : (Time) RemainPoint : (%d)",
			lpObj->AccountID, lpObj->Name, m_ObjBill[lpObj->m_Index].GetEndTime());
	}
	else
	{
		wsprintf(szMsg, lMsg.Get(MSGGET(5, 222)));
		LogAddTD("[%s][%s] BillType : (NoCharge)", lpObj->AccountID, lpObj->Name);
	}

	std::map<int,PREMIUM_SETTINGS>::iterator _It = gPremiumData.find(PayCode);

	if( _It != gPremiumData.end() )
	{
		m_ObjBill[lpObj->m_Index].SetExp(_It->second.AddExp);
		m_ObjBill[lpObj->m_Index].SetMasterExp(_It->second.AddMasterExp);
		m_ObjBill[lpObj->m_Index].SetDrop(_It->second.AddDrop);
		m_ObjBill[lpObj->m_Index].SetZen(_It->second.AddZen);
		m_ObjBill[lpObj->m_Index].SetResetDiscount(_It->second.ResetDiscount);
		m_ObjBill[lpObj->m_Index].SetGPBonus(_It->second.GPBonus);
	}

	LogAdd(szMsg);
	GCServerMsgStringSend(szMsg, lpObj->m_Index, 1);
#endif
}






short gObjMemFree(int index)
{
	int i = 0;
	LPOBJ lpObj;

	if ( gDisconnect == TRUE )
	{
		return -1;
	}

	if ( index < 0 || index > OBJMAX )
	{
		LogAdd("(%s)(%d) = index over error (%d)", __FILE__, __LINE__, index);
		return false;
	}

	lpObj = &gObj[index];

	if ( lpObj->Connected < PLAYER_CONNECTED )
	{
		return false;
	}

	gObjTradeSave(lpObj, index);

	if ( lpObj->Connected >= PLAYER_LOGGED )
	{
		if ( lpObj->Type == OBJ_USER )
		{
			if ( lpObj->Connected == PLAYER_PLAYING )
			{
				if (  BC_MAP_RANGE(lpObj->MapNumber) )
				{
					g_BloodCastle.SearchUserDropQuestItem(lpObj->m_Index);
				}

				GJSetCharacterInfo(lpObj, index, 0);
			}
			
			GJPUserClose(lpObj);
		}
	}

	gObjViewportClose(lpObj);
	lpObj->Connected = PLAYER_EMPTY;
	return 1;
}








BOOL gObjGameClose(int aIndex)
{
	if ( aIndex < 0 || aIndex >= OBJMAX )
	{
		LogAdd("(%s)(%d) = index over error (%d)", __FILE__, __LINE__, aIndex);

		return FALSE;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->Connected != PLAYER_PLAYING )
	{
		return FALSE;
	}

	if ( lpObj->Type != OBJ_USER )
	{
		LogAddTD(lMsg.Get(MSGGET(2, 3)), lpObj->AccountID, lpObj->Name);
		return FALSE;
	}

#if (CUSTOM_OLYMP==1)

	if( lpObj->m_btChangedStats == TRUE )	{
		lpObj->LevelUpPoint = lpObj->m_OldLvlUpPts;
		lpObj->Strength = lpObj->m_OldStrength;
		lpObj->Dexterity = lpObj->m_OldDexterity;
		lpObj->Vitality = lpObj->m_OldVitality;
		lpObj->Energy = lpObj->m_OldEnergy;
		lpObj->Leadership = lpObj->m_OldLeadership;
	}

#endif

	if ( lpObj->m_RecallMon >= 0 )
	{
		gObjMonsterCallKill(lpObj->m_Index);
	}

	gObjMagicTextSave(lpObj);

	if ( (   (lpObj->m_IfState.use != 0 )
		&&  (lpObj->m_IfState.type == 7 )) 
		|| ((lpObj->m_IfState.use != 0)  
		&& (lpObj->m_IfState.type == 13)))
	{
		if ( lpObj->bIsChaosMixCompleted == true || lpObj->m_bIsCastleNPCUpgradeCompleted == true)
		{
			gObjInventoryCommit(aIndex);
		}
		else
		{
			gObjInventoryRollback(aIndex);
		}
	}

	gObjTradeSave(lpObj, aIndex);

	if ( (lpObj->m_IfState.use != 0 )
		&&  (lpObj->m_IfState.type == 10 ) 
		&& lpObj->TargetShopNumber == 229)
	{
		gQeustNpcTeleport.TalkRefDel();
	}

#if (CUSTOM_OLYMP == 1)

	if ( gObjDuelCheck(lpObj) )
	{
		if( OBJMAX_RANGE(lpObj->m_iDuelUser) )	{
			g_CustomOlymp.EndFight(lpObj->m_iDuelUser,lpObj->m_Index,1);
		}
	}

#endif


#if (ENABLETEST_NEWPVP == 1)
	g_NewPVP.Reset(*lpObj); //1.00.92	
#else
	if ( OBJMAX_RANGE(lpObj->m_iDuelUserRequested) )
	{
		gObjSendDuelEnd(&gObj[lpObj->m_iDuelUserRequested]);
	}

	if ( gObjDuelCheck(lpObj) != FALSE )
	{
		gObjSendDuelEnd(&gObj[lpObj->m_iDuelUser]);
	}
#endif

	gObjResetDuel(lpObj);

	if ( lpObj->PartyNumber >= 0 )
	{
		int iPartyIndex= gParty.GetIndex(lpObj->PartyNumber, lpObj->m_Index, lpObj->DBNumber);

		if ( iPartyIndex >= 0 )
		{
			PMSG_PARTYDELUSER pMsg;
			pMsg.Number = iPartyIndex;

			if( lpObj->m_bMapSvrMoveQuit == TRUE )
			{	
				CGPartyDelUser(&pMsg, lpObj->m_Index);
			}
			else if( !lpObj->m_bIsMaintainParty  )
			{
				pMsg.Number = iPartyIndex;

				if( IF_MAP_RANGE(lpObj->MapNumber) ||
					IT_MAP_RANGE(lpObj->MapNumber) )
				{
					//CGPartyDelUserAsExitGameByForce(&pMsg, lpObj->m_Index);
					CGPartyDelUser(&pMsg, lpObj->m_Index);
				}
				else
				{
					CGPartyDelUser(&pMsg, lpObj->m_Index);
				}
			}
			else 
			{
				/*if( gParty.GetReallyConnectPartyMemberCount(lpObj->PartyNumber) > 1 )
					cpTempUserManager->AddUserData(lpObj->Name,lpObj->m_Index,lpObj->PartyNumber);
				else
					cpTempUserManager->DeleteAllPartyUserData(lpObj->PartyNumber);
				*/
				//CGPartyDelUserAsExitGameByForce(&pMsg, lpObj->m_Index);
				CGPartyDelUser(&pMsg, lpObj->m_Index);
			}
		}
	}


	if ( lpObj->GuildNumber > 0 )
	{
		if ( lpObj->lpGuild->WarState == 1 )
		{
			gObjGuildWarMasterClose(lpObj);
		}

		GDCharClose(0, lpObj->lpGuild->Name, lpObj->Name);
	}
	else
	{
		GDCharClose(1, NULL, lpObj->Name);
	}

	if ( lpObj->WarehouseSave == 1 )
	{
		GDSetWarehouseList(lpObj->m_Index);
	}

	if ( DS_MAP_RANGE(lpObj->MapNumber) )
	{
		if( cpTempUserManager->CheckMatchIndex(lpObj->m_Index) == -1 )
		{
			g_DevilSquare.DelUser(lpObj->m_bDevilSquareIndex,lpObj->m_Index);
		}
	}

	if ( BC_MAP_RANGE(lpObj->MapNumber) )
	{
		g_BloodCastle.SearchUserDropQuestItem(lpObj->m_Index);
	}

	if ( IT_MAP_RANGE(lpObj->MapNumber) )
	{
		g_IllusionTemple.DropRelic(lpObj->MapNumber,lpObj->m_Index);

		if( cpTempUserManager->CheckMatchIndex(lpObj->m_Index) == -1 )
		{
			g_IllusionTemple.LeaveUser(lpObj->m_Index,lpObj->MapNumber);
		}
	}

	if ( OBJMAX_RANGE(lpObj->m_iMiningObjectId) )
	{
		LPOBJ OreObj = &gObj[lpObj->m_iMiningObjectId];

		if( OreObj->Class >= 605 && OreObj->Class <= 607 )
		{
			OreObj->m_bOreUsed = FALSE;
		}
	}

	gObjSaveChaosBoxItemList(lpObj);

	if ( gObjFixInventoryPointer(aIndex) == false )
	{
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}
	

	GDPartyMatchUpdateStatus(lpObj->Name, 0);
	g_CashItemPeriodSystem.ClearPeriodItemEffect(lpObj,-1);
	gObjClearBuffEffect(lpObj,CLEAR_TYPE_LOGOUT);
	GJSetCharacterInfo(lpObj, aIndex, 0);
	gObjViewportClose(lpObj);

#if(__QUEST_SYSTEM__== 1 )
	gQuestInfoEx.GDSaveQuestInfo(lpObj);
#endif

#if(__GENS_SYSTEM__ == 1)
	if( lpObj->m_GensInfo.btFamily != GENS_NONE )
	{
		gGensSystem.GDSaveGensInfo(lpObj->m_Index);
	}
#endif

	LogAddTD(lMsg.Get(MSGGET(2, 4)), lpObj->m_Index, lpObj->AccountID, lpObj->Name);

	if(lpObj->Name)
	{
		WhisperCash.DelCash(lpObj->Name);
	}

	g_CashShop.DeleteUser(lpObj);

	if(lpObj->m_iPeriodItemEffectIndex != -1)
	{
		g_CashItemPeriodSystem.ClearPeriodItemEffect(lpObj,-1);
	}

#if (ENABLETEST_MUUN == 1)
	g_CMuunSystem.RemoveMuunItemPeriodInfo(lpObj);
#endif

#if (__CUSTOM_QUEST__ == 1)
	g_MyQuest.SaveUser(lpObj->m_Index);
#endif

#if (CUSTOM_PETS == 1)
	gCustomPets.SaveSummonInfo(lpObj->m_Index,lpObj->Name);
#endif

	//memcpy(lpObj->BackName, lpObj->Name, sizeof(lpObj->Name)-1);
	//lpObj->BackName[10] = 0;


	memset(lpObj->Name, 0, sizeof(lpObj->Name)-1);
	lpObj->Connected = PLAYER_LOGGED;

/*	if ( m_ObjBill[aIndex].GetCertify() >= 0 && m_ObjBill[aIndex].GetCertify() < 3)
	{
		gLCount[m_ObjBill[aIndex].GetCertify()].Delete();
	}*/

	return TRUE;
}








short gObjDel(int index)
{
	int i = 0;

	if ( index < 0 || index > OBJMAX )
	{
		LogAdd ("(%s)(%d) = index over error (%d)", __FILE__, __LINE__, index);
		return 0;
	}

	LPOBJ lpObj = &gObj[index];

	if ( lpObj->Connected < PLAYER_CONNECTED )
	{
		return 0;
	}

	if ( MAX_MAP_RANGE(lpObj->MapNumber) )
	{
		MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX, lpObj->m_OldY);
		MapC[lpObj->MapNumber].ClearStandAttr(lpObj->X, lpObj->Y);
	}

	int UserConnectState = lpObj->Connected;

	if ( lpObj->Type == OBJ_USER )
	{
		gObjGameClose(index);

		if ( UserConnectState >= PLAYER_LOGGED )
		{
			if ( lpObj->m_bMapSvrMoveQuit == false )
			{
				GJPUserClose(lpObj);
				LogAddTD("(%d)logout-send : %s [%s]", index, lpObj->AccountID, lpObj->Ip_addr);
			}
			else
			{
				LogAddTD("[MapServerMng] User Quit without JoinServer logout-send [%s][%s]",
					lpObj->AccountID, lpObj->Name);
			}

			memset(lpObj->AccountID, 0, sizeof(lpObj->AccountID) -1);
			memset(lpObj->LastJoominNumber, 0, sizeof(lpObj->LastJoominNumber));
		}

		LogAddTD("(%d)logout : %s [%s]", index, lpObj->AccountID, lpObj->Ip_addr);
		IpCache.DelIp(lpObj->Ip_addr);
	}

	lpObj->Connected = PLAYER_EMPTY;
	return 1;
}






short gObjSometimeClose(int index)
{
	if ( gDisconnect == 1 )
	{
		return -1;
	}

	if ( index < 0 || index > OBJMAX )	// deathway fix needed
	{
		LogAdd("(%s)(%d) = index over error (%d)", __FILE__, __LINE__, index);
		return 0;
	}

	LPOBJ lpObj = &gObj[index];
	gObjTradeSave(lpObj, index);

	if ( lpObj->Connected > PLAYER_CONNECTED )
	{
		if ( lpObj->Type == OBJ_USER )
		{
			if ( lpObj->Connected == PLAYER_PLAYING )
			{
				if ( lpObj->PartyNumber >= 0 )
				{
					PMSG_PARTYDELUSER pMsg;
					int iPartyIndex = gParty.GetIndex(lpObj->PartyNumber, lpObj->m_Index, lpObj->DBNumber);

					if ( iPartyIndex >= 0 )
					{
						pMsg.Number = iPartyIndex;
						CGPartyDelUser(&pMsg, lpObj->m_Index);
					}
				}
			}
		}
	}

	return 1;
}







BOOL gObjIsGamePlaing(LPOBJ lpObj)
{
	if ( lpObj->Connected < PLAYER_PLAYING )
	{
		return false;
	}

	return TRUE;
}





BOOL gObjIsConnectedGP(int aIndex)
{
	if (aIndex < 0 || aIndex > OBJMAX-1 )
	{
		return FALSE;
	}
	
	if ( gObj[aIndex].Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( gObj[aIndex].Connected < PLAYER_PLAYING )
	{
		return FALSE;
	}

	if ( gObj[aIndex].CloseCount > 0 )
	{
		return FALSE;
	}

	/*if ( gObj[aIndex].Connected == 4 )
	{
		return FALSE;
	}*/

	return TRUE;
}




BOOL gObjIsConnectedGP(int aIndex, char * CharName)
{
	if (aIndex < 0 || aIndex > OBJMAX-1 )
	{
		return FALSE;
	}
	
	if ( gObj[aIndex].Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( gObj[aIndex].Connected < PLAYER_PLAYING )
	{
		return FALSE;
	}

	/*if ( gObj[aIndex].Connected == 4 )
	{
		return FALSE;
	}*/

	if ( gObj[aIndex].CloseCount > 0 )
	{
		return FALSE;
	}

	if ( strcmp(gObj[aIndex].Name, CharName) != 0 )
	{
		return FALSE;
	}

	return TRUE;
}








BOOL gObjIsConnected(LPOBJ lpObj)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( lpObj->Connected < PLAYER_PLAYING )
	{
		return FALSE;
	}

	if ( lpObj->CloseCount > 0 )
	{
		return FALSE;
	}

	/*if ( lpObj->Connected == 4 )
	{
		return FALSE;
	}*/

	return TRUE;
}







BOOL gObjIsConnected(int aIndex) 
{
	if ( aIndex < 0 || aIndex > OBJMAX-1)
	{
		return FALSE;
	}
	if ( gObj[aIndex].Connected < PLAYER_PLAYING )
	{
		return FALSE;
	}

	/*if ( gObj[aIndex].Connected == 4 )
	{
		return FALSE;
	}*/

	return TRUE;
}






BOOL gObjIsConnected(LPOBJ lpObj, int dbnumber)
{
	if (lpObj->Connected < PLAYER_PLAYING)
	{
		return 0;
	}

	if (lpObj->DBNumber != dbnumber)
	{
		return 0;
	}

	/*if( lpObj->Connected == 4 )
	{
		return 0;
	}*/

	return 1;
}











BOOL gObjIsAccontConnect(int aIndex, char* accountid)
{
	if (aIndex <0 || aIndex > OBJMAX-1) // Bad
	{
		return 0;
	}

	if (gObj[aIndex].Connected< PLAYER_CONNECTED)
	{
		return 0;
	}

	if (strlen(&gObj[aIndex].AccountID[0]) < 1 )
	{
		return 0;
	}

	if ( strcmp(accountid, &gObj[aIndex].AccountID[0]) != 0)
	{
		return 0;
	}
	return 1;
}









BOOL gObjJoominCheck(int aIndex, char* szInJN)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( strcmp(szInJN, &lpObj->LastJoominNumber[0]) == 0 )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL gObjCheckXYMapTile(LPOBJ lpObj, int iDbgName)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return false;
	}

	if ( (lpObj->Authority&2 ) == 2 )
	{
		return FALSE;
	}

	if ( CC_MAP_RANGE(lpObj->MapNumber) )
	{
		int iFloor = g_ChaosCastle.GetFloor(lpObj->MapNumber);
		if ( g_ChaosCastle.GetCurrentState(iFloor) == 2 )
		{
			return FALSE;
		}
	}

	int x = lpObj->X;
	int y = lpObj->Y;
	int mapnumber = lpObj->MapNumber;

	if ( mapnumber > MAX_NUMBER_MAP-1 )
	{
		mapnumber = MAP_INDEX_RORENCIA;
	}

	int mapminlevel = MapMinUserLevel[mapnumber];
	mapminlevel = 0;

	if ( lpObj->Class == 4 || lpObj->Class == 3 )
	{
		if ( mapminlevel > 0 )
		{
			if ( MapMinUserLevel[mapnumber] > 0 )
			{
				mapminlevel = MapMinUserLevel[mapnumber] - (MapMinUserLevel[mapnumber] / 3 * 2);
			}
		}
	}

	if ( lpObj->Class == 2 && lpObj->Level < 10 )
	{
		if ( lpObj->MapNumber != MAP_INDEX_NORIA )
		{
			LogAddTD(lMsg.Get(MSGGET(2, 5)), lpObj->Name, lpObj->MapNumber, lpObj->X, lpObj->Y);
			PMSG_TELEPORT pMsg;
			pMsg.MoveNumber = 27;
			gObjMoveGate(lpObj->m_Index, pMsg.MoveNumber);
		}
	}
	else
	{
		if ( mapminlevel > lpObj->Level )
		{
			LogAddTD(lMsg.Get(MSGGET(2, 5)), lpObj->Name, lpObj->MapNumber, lpObj->X, lpObj->Y);
			PMSG_TELEPORT pMsg;
			pMsg.MoveNumber = 17;
			gObjMoveGate(lpObj->m_Index, pMsg.MoveNumber);
		}
	}

	BYTE attr = MapC[mapnumber].GetAttr(x, y);

	if ( (attr&4) == 4 || (attr&8) == 8 )
	{
		BYTE attr2 = MapC[mapnumber].GetAttr(x+2, y);
		BYTE attr3 = MapC[mapnumber].GetAttr(x, y+2);
		BYTE attr4 = MapC[mapnumber].GetAttr(x-2, y);
		BYTE attr5 = MapC[mapnumber].GetAttr(x, y-2);

		if ( attr2 > 1 && attr3 > 1 && attr4 > 1 && attr5 > 1 )
		{
			LogAddTD("[ CHECK POSITION ] DbgName[%d] [%s][%s] Map[%d]-(%d,%d) Invalid location causes to force to move",iDbgName,lpObj->AccountID, lpObj->Name, lpObj->MapNumber, lpObj->X, lpObj->Y);
			PMSG_TELEPORT pMsg;
			pMsg.MoveNumber = 17;
			gObjMoveGate(lpObj->m_Index, pMsg.MoveNumber);

			return TRUE;
		}
	}

	return FALSE;

}







BOOL gObjSetAccountLogin(int aIndex, char * szId, int aUserNumber, int aDBNumber, char* JoominNumber)
{
	if ( !OBJMAX_RANGE(aIndex) )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return 0;
	}

	if ( gObj[aIndex].Connected != PLAYER_CONNECTED )
	{
		LogAddTD(lMsg.Get(MSGGET(2, 6)), aIndex, szId, aDBNumber, __FILE__, __LINE__);
		return 0;
	}

	if ( strcmp(gObj[aIndex].AccountID, szId) != 0 )
	{
		LogAdd(lMsg.Get(MSGGET(2, 7)), szId);
		return 0;
	}

	gObj[aIndex].UserNumber = aUserNumber;
	gObj[aIndex].DBNumber = aDBNumber;
	gObj[aIndex].Connected = PLAYER_LOGGED;
	strcpy_s(gObj[aIndex].LastJoominNumber, sizeof(gObj[aIndex].LastJoominNumber), JoominNumber);


#if (__CUSTOM__ == 1)
	GCJoinBillCheckSend(szId,aIndex);
#endif

	return TRUE;

}







BOOL gObjGetUserNumber(int aIndex)
{
	int usernumber;

	if ( !OBJMAX_RANGE(aIndex))
	{
		LogAdd("error : %s %d",__FILE__, __LINE__);
		return 0;
	}

	usernumber=gObj[aIndex].UserNumber;
	return usernumber;
}








BOOL gObjGetNumber(int aIndex, int& UN, int& DBN)
{
	if ( aIndex < 0 || aIndex > OBJMAX-1)
	{
		LogAdd("error : %s %d",__FILE__, __LINE__);
		return 0;
	}
	UN=gObj[aIndex].UserNumber;
	DBN=gObj[aIndex].DBNumber;
	return 1;
}







// gObj Address 653DBF8
char* gObjGetAccountId(int aIndex)
{
	if ( aIndex < 0 || aIndex > OBJMAX-1)
	{
		LogAdd("error : %s %d",__FILE__, __LINE__);
		return 0;
	}

	if ( gObj[aIndex].Connected == 2)
	{
		return &gObj[aIndex].AccountID[0];
	}
	return 0;
}








int gObjGetIndex(char* szId)
{
	int n;

	for (n=OBJ_STARTUSERINDEX;n<OBJMAX; n++)
	{
		if ( gObj[n].Connected >= PLAYER_PLAYING)
		{
			if ( szId != NULL && gObj[n].Name[0] == *szId )
			{
				if ( strcmp(&gObj[n].Name[0] , szId) == 0)
				{
					return n;
				}
			}
		}
	}
	return -1;
}








BOOL gObjUserIdConnectCheck(char* szId,int index)
{
	if ( index < 0 || index > OBJMAX)	// Deathway Fix Here
	{
		LogAdd("error : index error !! %s %d",__FILE__, __LINE__);
		return 0;
	}

	if ( gObj[index].Connected < PLAYER_PLAYING)
	{
		return 0;
	}
	if( szId == NULL )
	{
		return 0;
	}
	if (gObj[index].Name[0] != *szId)
	{
		return 0;
	}

	if ( strcmp(gObj[index].Name , szId) == 0)
	{
		return 1;
	}
	return 0;
}








BYTE GetPathPacketDirPos(int px, int py)
{
	BYTE pos = 0;

	if (px <= -1 && py <= -1)
	{
		pos=0;
	}
	else if (px <= -1 && py == 0)
	{
		pos=7;
	}
	else if ( px <= -1 && py >= 1)
	{
		pos=6;
	}
	else if ( px == 0 && py <= -1)
	{
		pos= 1;
	}
	else if ( px == 0 && py >= 1)
	{
		pos = 5;
	}
	else if ( px >= 1 && py <=-1)
	{
		pos=2;
	}
	else if ( px >= 1 && py == 0)
	{
		pos = 3;
	}
	else if ( px >=1  && py >= 1)
	{
		pos = 4;
	}

	return pos;
}








int  gObjCalDistance(LPOBJ lpObj1, LPOBJ lpObj2)
{
	if ( lpObj1->X == lpObj2->X && lpObj1->Y == lpObj2->Y )
	{
		return 0;
	}

	float tx = (float)(lpObj1->X - lpObj2->X);
	float ty = (float)(lpObj1->Y - lpObj2->Y);

	return (int)sqrt( (tx*tx)+(ty*ty) );
}








BOOL gObjPositionCheck(LPOBJ lpObj)
{
	int ix = lpObj->m_OldX - lpObj->TX;
	int iy = lpObj->m_OldY - lpObj->TY;

	if ( ix <= -15 ||  ix >= 15 )
	{
		return FALSE;
	}

	if ( iy <= -15 ||  iy >= 15 )
	{
		return FALSE;
	}

	return TRUE;
}








BOOL gObjCheckTileArea(int aIndex, int x, int y, int dis)
{
	LPOBJ lpObj = & gObj[aIndex];

	if ( x < (lpObj->X - dis) || x > (lpObj->X + dis) || y < (lpObj->Y - dis) || y > (lpObj->Y + dis) )
	{
		return FALSE;
	}
	return TRUE;
}








BOOL ExtentCheck(int x, int y, int w, int h)
{
	if ( (x >=0) && (x<w) && (y >=0) && (y<h) )
	{
		return 1;
	}
	return 0;
}






void gObjSetInventory1Pointer(LPOBJ lpObj)
{
	lpObj->pInventory = lpObj->Inventory1;
	lpObj->pInventoryMap = lpObj->InventoryMap1;
	lpObj->pInventoryCount  = &lpObj->InventoryCount1;
}

void gObjSetInventory2Pointer(LPOBJ lpObj)
{
	lpObj->pInventory = lpObj->Inventory2;
	lpObj->pInventoryMap = lpObj->InventoryMap2;
	lpObj->pInventoryCount  = &lpObj->InventoryCount2;
}

#if (ENABLETEST_MUUN == 1)
void gObjSetMuunInventory1Pointer(OBJECTSTRUCT* lpObj) {
	lpObj->pMuunInventory = lpObj->pMuunInventory1;
}

void gObjSetMuunInventory2Pointer(OBJECTSTRUCT* lpObj) {
	lpObj->pMuunInventory = lpObj->pMuunInventory2;
}
#endif

void gObjAddMsgSend(LPOBJ lpObj, int aMsgCode, int aIndex, int  SubCode)
{
	int n;
	int empty = -1;

	MessageStateMachine* pSMMsg = gSMMsg[lpObj->m_Index];

	for ( n=0;n<MAX_MONSTER_SEND_MSG;n++)	{
		if( empty == -1 && pSMMsg[n].MsgCode < 0 )	{
			empty = n;
			break;
		}
		else if( pSMMsg[n].MsgCode == aMsgCode && pSMMsg[n].SendUser == aIndex )	{
			return;
		}
	}

	if( empty == -1 )	{
		for ( n=0;n<MAX_MONSTER_SEND_MSG;n++)	{
			if (pSMMsg[n].MsgCode == 2 || pSMMsg[n].MsgCode == 6 || pSMMsg[n].MsgCode == 7 )	{
				pSMMsg[n].MsgCode = aMsgCode;
				pSMMsg[n].MsgTime = GetTickCount();
				pSMMsg[n].SendUser= aIndex;
				pSMMsg[n].SubCode = SubCode;
				return;
			}
		}

		if( lpObj->Type == OBJ_MONSTER )	{
			LogAddTD("[ERROR] [MSG_SEND] Monster[%s] MsgCode[%d]",lpObj->Name,aMsgCode);
			for ( n=0;n<MAX_MONSTER_SEND_MSG;n++)	{
				LogAddTD("[ACTION] %d | %u | %d | %d",
					pSMMsg[n].MsgCode,pSMMsg[n].MsgTime,pSMMsg[n].SendUser,pSMMsg[n].SubCode);
			}
		}
	}
	else{
		pSMMsg[empty].MsgCode = aMsgCode;
		pSMMsg[empty].MsgTime = GetTickCount();
		pSMMsg[empty].SendUser= aIndex;
		pSMMsg[empty].SubCode = SubCode;
	}
}

void gObjAddMsgSendDelay(LPOBJ lpObj, int aMsgCode, int aIndex, int delay, int SubCode)
{
	int n;
	int empty = -1;

	MessageStateMachine* pSMMsg = gSMMsg[lpObj->m_Index];

	for ( n=0;n<MAX_MONSTER_SEND_MSG;n++)	{
		if( empty == -1 && pSMMsg[n].MsgCode < 0 )	{
			empty = n;
			break;
		}
		else if( pSMMsg[n].MsgCode == aMsgCode && pSMMsg[n].SendUser == aIndex )	{
			return;
		}
	}

	if( empty == -1 )	{
		for ( n=0;n<MAX_MONSTER_SEND_MSG;n++)	{
			if (pSMMsg[n].MsgCode == 2 || pSMMsg[n].MsgCode == 6 || pSMMsg[n].MsgCode == 7 )	{
				pSMMsg[n].MsgCode = aMsgCode;
				pSMMsg[n].MsgTime = GetTickCount()+delay;
				pSMMsg[n].SendUser= aIndex;
				pSMMsg[n].SubCode = SubCode;
				return;
			}
		}

		if( lpObj->Type == OBJ_MONSTER )	{
			LogAddTD("[ERROR] [MSG_SEND_DELAY] Monster[%s] MsgCode[%d]",lpObj->Name,aMsgCode);
			for ( n=0;n<MAX_MONSTER_SEND_MSG;n++)	{
				LogAddTD("[ACTION] %d | %u | %d | %d",
					pSMMsg[n].MsgCode,pSMMsg[n].MsgTime,pSMMsg[n].SendUser,pSMMsg[n].SubCode);
			}
		}
	}
	else{
		pSMMsg[empty].MsgCode = aMsgCode;
		pSMMsg[empty].MsgTime = GetTickCount()+delay;
		pSMMsg[empty].SendUser= aIndex;
		pSMMsg[empty].SubCode = SubCode;
	}
}


void gObjAddMsgSendDelayInSpecificQPos(LPOBJ lpObj, int aMsgCode, int aIndex, int delay, int SubCode, int iQPosition)
{
	if ( iQPosition >= 0 &&  iQPosition < MAX_MONSTER_SEND_MSG )
	{
		gSMMsg[lpObj->m_Index][iQPosition].MsgCode = aMsgCode;
		gSMMsg[lpObj->m_Index][iQPosition].MsgTime = GetTickCount();
		gSMMsg[lpObj->m_Index][iQPosition].SendUser = aIndex;
		gSMMsg[lpObj->m_Index][iQPosition].SubCode = SubCode;
	}

}






void gObjAddAttackProcMsgSendDelay(LPOBJ lpObj, int aMsgCode, int aIndex, int delay, int SubCode, int SubCode2)
{
	for ( int n=0;n<MAX_MONSTER_SEND_ATTACK_MSG;n++)
	{
		int iIndex = lpObj->m_Index - OBJ_STARTUSERINDEX;

		if ( gSMAttackProcMsg[iIndex][n].MsgCode < 0 )
		{
			gSMAttackProcMsg[iIndex][n].MsgCode = aMsgCode;
			gSMAttackProcMsg[iIndex][n].MsgTime = GetTickCount() + delay;
			gSMAttackProcMsg[iIndex][n].SendUser = aIndex;
			gSMAttackProcMsg[iIndex][n].SubCode = SubCode;
			gSMAttackProcMsg[iIndex][n].SubCode2 = SubCode2;

			return;
		}
	}

}





void gObjMsgProc(LPOBJ lpObj)
{
	int aIndex = lpObj->m_Index;

	for ( int n=0;n<MAX_MONSTER_SEND_MSG;n++)
	{
		if ( gSMMsg[aIndex][n].MsgCode >= 0 )
		{
			if ( GetTickCount() > gSMMsg[aIndex][n].MsgTime )
			{
				if ( lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC )
				{
					gObjMonsterStateProc(lpObj, gSMMsg[aIndex][n].MsgCode, gSMMsg[aIndex][n].SendUser, gSMMsg[aIndex][n].SubCode);
				}
				else
				{
					gObjStateProc(lpObj, gSMMsg[aIndex][n].MsgCode, gSMMsg[aIndex][n].SendUser, gSMMsg[aIndex][n].SubCode);
				}

				gSMMsg[aIndex][n].MsgCode = -1;
			}
		}
	}
}






void gObjStateProc(LPOBJ lpObj, int aMsgCode, int aIndex, int SubCode)
{
	if ( (  (aIndex<0)?FALSE:(aIndex > OBJMAX-1)?FALSE:TRUE ) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__ );
		return;
	}

	switch ( aMsgCode )
	{
		case  2:
			gObjBackSpring(lpObj, &gObj[aIndex]);
			break;

		case  3:
			gObjMonsterDieLifePlus(lpObj, &gObj[aIndex]);
			break;

		case 1000:
			gObjBillRequest(lpObj);
			break;

		case  4:
			if ( lpObj->lpGuild != NULL && lpObj->lpGuild->lpTargetGuildNode != NULL )
			{
				gObjGuildWarEnd(lpObj->lpGuild, lpObj->lpGuild->lpTargetGuildNode);
			}
			break;

		case  5:
			if ( lpObj->lpGuild != NULL && lpObj->lpGuild->WarState != NULL && lpObj->lpGuild->WarType == 1 )
			{
				BattleSoccerGoalStart(0);
			}
			break;

		case  7:
			if ( lpObj->lpGuild != NULL && lpObj->lpGuild->WarState != NULL && lpObj->lpGuild->WarType == 1)
			{
				if ( lpObj->lpGuild->BattleGroundIndex >= 0 )
				{
					GCManagerGuildWarEnd(lpObj->lpGuild->Name);
				}
			}
			break;

		case 10:
			if ( lpObj->DamageReflect )
			{
				gObjAttack(lpObj, &gObj[aIndex], 0, 0, 0, SubCode, 0,0,TRUE);
			}
			break;
		case 54:
			if( lpObj->Live )
			{
				CMagicInf * lpMagic = &DefMagicInf[79];
				gObjAttack(lpObj, &gObj[aIndex], lpMagic, 1, 0, SubCode, 0,0,0);
			}
			break;
		case 12:
			gObjAttack(lpObj, &gObj[aIndex], 0, 0, 0, SubCode, 0,0,0);
			break;
		case 13:
			{
				lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
				GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
			}
			break;
		case 14:
			{
				lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
				GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
			}
			break;
		case 15:
			{
				lpObj->iShield = lpObj->iMaxShield+lpObj->iAddShield;
				GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
			}
			break;
		case 58:
			if( lpObj->Live )
			{
				CMagicInf * lpMagic = &DefMagicInf[216];
				gObjAttack(lpObj, &gObj[aIndex], lpMagic, 1, 0, SubCode, 0,0,0);
			}
			break;
		case 59:
			{
				BYTE x = ((SubCode&0xFFFF)&0xFFFF)&0xFF;
				BYTE y = ((SubCode&0xFFFF)&0xFFFF)>>8;
				BYTE type = SubCode>>16;
				CMagicInf* lpMagic = &DefMagicInf[AT_SKILL_LAGLE];

				gObjUseSkill.SkillLagle(lpObj->m_Index,lpMagic,x,y,aIndex,type);
			}
			break;
	}
}







void gObjStateAttackProc(LPOBJ lpObj, int aMsgCode, int aIndex, int SubCode, int SubCode2)
{
	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__ );
		return;
	}

	switch ( aMsgCode )
	{
		case 50:
			if ( lpObj->Live != FALSE )
			{
				CMagicInf * lpMagic = gObjGetMagicSearch(lpObj, SubCode);
				gObjAttack(lpObj, &gObj[aIndex], lpMagic, 0, 1, 0, SubCode2,0,0);
			}
			break;
		case 52:
			if ( lpObj->Live != FALSE )
			{
				CMagicInf * lpMagic = gObjGetMagicSearch(lpObj, SubCode);
				gObjAttack(lpObj, &gObj[aIndex], lpMagic, 0, 32, 0, SubCode2,0,0);
			}
			break;
		case 53:
			if( lpObj->Live )
			{
				CMagicInf * lpMagic = gObjGetMagicSearch(lpObj, SubCode);
				gObjAttack(lpObj, &gObj[aIndex], lpMagic, 1, 1, 0, 0,0,0);
			}
			break;
		case 51:
			gDarkSpirit[lpObj->m_Index].Attack(lpObj, &gObj[aIndex], NULL, SubCode, SubCode2);
			break;
	}

}






BOOL gObjBackSpring(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	int tdir;
#if (GS_CASTLE == 1)
	int iClass;
	int iIndex;
#endif
	int Effect;
	int iEffect;
	int x;
	int y;
	BYTE attr;
	PMSG_POSISTION_SET pMove;

	if ( MAX_MAP_RANGE(lpObj->MapNumber) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	if ( lpObj->Type == OBJ_USER )
	{
		if ( lpObj->m_LastTeleportTime > 0 )
		{
			return FALSE;
		}
	}

#if (GS_CASTLE == 1)

	if ( lpObj->Type == OBJ_USER && (lpObj->Authority & 2) == 2)
	{
		return false;
	}

#endif

	if ( lpObj->Class >= 131 && lpObj->Class <= 134 )
	{
		return FALSE;
	}

#if (GS_CASTLE == 1)

	if ( lpObj->Class == 277 
		|| lpObj->Class == 283
		|| lpObj->Class == 288
		|| lpObj->Class == 278
		|| lpObj->Class == 215
		|| lpObj->Class == 216
		|| lpObj->Class == 217
		|| lpObj->Class == 218
		|| lpObj->Class == 219 )
	{
		return FALSE;
	}

	if ( lpObj->Class == 524 || 
		 lpObj->Class == 525 ||
		 lpObj->Class == 526 ||
		 lpObj->Class == 527 ||
		 lpObj->Class == 528 )
	{
		return FALSE;
	}

#if( ENABLE_SWITCH_CS_BACKSPRING == 1 )
	if( g_CastleSiege.GetCrownUserIndex() == lpObj->m_Index)
	{
		if(lpObj->pInventory[8].m_Type == ITEMGET(13,4))
		{
			return FALSE;
		}
	}
#endif//ENABLE_SWITCH_CS_BACKSPRING

	if(g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_READY || g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_START)
	{
		if(lpObj->Type == OBJ_USER)
		{
			for(iClass = 205; iClass <= 209; iClass++)
			{
				iIndex = g_CrywolfNPC_Altar.GetAltarUserIndex(iClass);

				if(iIndex != -1 && iIndex == lpObj->m_Index)
				{
					return FALSE;
				}
			}
		}
	}

	if(CRYWOLF_ALTAR_CLASS_RANGE(lpObj->Class) != FALSE || ((lpObj->Class == 204) ? TRUE : FALSE) != FALSE)
	{
		return FALSE;
	}

	if(lpObj->Class == 348)
	{
		return FALSE;
	}

#endif

	if(lpObj->Class == 275)
	{
		return false;
	}

	if( lpObj->Class == 459 ||
		lpObj->Class == 460 ||
		lpObj->Class == 461 ||
		lpObj->Class == 462 )
	{
		return false;
	}

	Effect = -1;

	for(iEffect = 0; iEffect < lpObj->btEffectCount; iEffect++)
	{
		Effect = lpObj->pEffectInfo[iEffect].btEffectNum;
		if( Effect == 0x39 ||
			Effect == 0x3D || 
			Effect == 0x41 )
		{
			return FALSE;
		}
	}

	if ( (rand()%3) == 0 )
	{
		if ( lpTargetObj->Dir < 4 )
		{
			tdir = lpTargetObj->Dir + 4;
		}
		else
		{
			tdir = lpTargetObj->Dir - 4;
		}

		tdir *= 2;
	}
	else
	{
		tdir = lpTargetObj->Dir * 2;
	}

	x = lpObj->X;
	y = lpObj->Y;
	x += RoadPathTable[tdir];
	y += RoadPathTable[1+tdir];
	attr = MapC[lpObj->MapNumber].GetAttr(x, y);

#if (GS_CASTLE == 1)
	if ( (attr&1)==1 || (attr&2)==2 || (attr&4)== 4 || (attr&8)== 8  || (attr&16)==16)
	{
		return FALSE;
	}
#else
	if ( (attr&1)==1 || (attr&2)==2 || (attr&4)== 4 || (attr&8)== 8  )
	{
		return FALSE;
	}
#endif

	
	pMove.h.c = 0xC1;
#if (__ENG__ == 1)
	pMove.h.headcode = 0x15;
#else
	// KOR PROTOCOL
	pMove.h.headcode = 0x10;
#endif
	pMove.h.size = sizeof(pMove);
	pMove.X = x;
	pMove.Y = y;

	lpObj->m_Rest = 0;
	MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX, lpObj->m_OldY);
	MapC[lpObj->MapNumber].SetStandAttr(x, y);
	lpObj->m_OldX = x;
	lpObj->m_OldY = y;
	RecvPositionSetProc(&pMove, lpObj->m_Index);

	if ( lpObj->Type == OBJ_USER )
	{
		lpObj->PathCount = 0;
	}

	return TRUE;
}







BOOL BackSpringCheck(int & x, int & y, int & dir, BYTE map)
{
	BYTE attr;
	int tx = x;
	int ty = y;
	int tdir = dir/2;

	tx = tx + RoadPathTable[dir];
	ty = ty + RoadPathTable[1+dir];

	attr = MapC[map].GetAttr(tx, ty);

	if  (  (attr&1)==1 || (attr&4)== 4 || (attr&8)== 8 )
	{
		tdir += 4;

		if ( tdir > 7 )
		{
			tdir -= 8;
		}

		dir = tdir * 2;
		return FALSE;
	}

	x = tx;
	y = ty;

	return TRUE;
}

BOOL gObjBackSpring2(LPOBJ lpObj, LPOBJ lpTargetObj, int count)
{
	int tdir;
#if (GS_CASTLE == 1)
	int iClass;
	int iIndex;
#endif
	int Effect;
	int iEffect;
	int x;
	int y;

	if ( MAX_MAP_RANGE(lpObj->MapNumber) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

#if (GS_CASTLE == 1)
	if( lpObj->Type == OBJ_USER && (lpObj->Authority & 2) == 2)
	{
		return FALSE;
	}
#endif

	if ( lpObj->Class == 287 || lpObj->Class == 286 )
	{
		return FALSE;
	}

#if (GS_CASTLE == 1)
	if ( lpObj->Class == 278 )
	{
		return FALSE;
	}
#endif

	if ( lpObj->Class >= 131 && lpObj->Class <= 134 )
	{
		return FALSE;
	}

#if (GS_CASTLE == 1)

	if ( lpObj->Class == 277 
		|| lpObj->Class == 283
		|| lpObj->Class == 288
		|| lpObj->Class == 278
		|| lpObj->Class == 215
		|| lpObj->Class == 216
		|| lpObj->Class == 217
		|| lpObj->Class == 218
		|| lpObj->Class == 219 )
	{
		return FALSE;
	}

#if( ENABLE_SWITCH_CS_BACKSPRING == 1 )
	if( g_CastleSiege.GetCrownUserIndex() == lpObj->m_Index)
	{
		if(lpObj->pInventory[8].m_Type == ITEMGET(13,4))
		{
			return FALSE;
		}
	}
#endif//ENABLE_SWITCH_CS_BACKSPRING

	if(g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_READY || g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_START)
	{
		if(lpObj->Type == OBJ_USER)
		{
			for(iClass = 205; iClass <= 209; iClass++)
			{
				iIndex = g_CrywolfNPC_Altar.GetAltarUserIndex(iClass);

				if(iIndex != -1 && iIndex == lpObj->m_Index)
				{
					return FALSE;
				}
			}
		}
	}

	if(CRYWOLF_ALTAR_CLASS_RANGE(lpObj->Class) != FALSE || ((lpObj->Class == 204) ? TRUE : FALSE) != FALSE)
	{
		return FALSE;
	}

	if(lpObj->Class == 348)
	{
		return FALSE;
	}

#endif
	

	if(lpObj->Class == 275)
	{
		return FALSE;
	}

	if(lpObj->Class == 459 ||
		lpObj->Class == 460 ||
		lpObj->Class == 461 ||
		lpObj->Class == 462 )
	{
		return false;
	}

	if ( lpObj->Class == 524 || 
		 lpObj->Class == 525 ||
		 lpObj->Class == 526 ||
		 lpObj->Class == 527 ||
		 lpObj->Class == 528 )
	{
		return FALSE;
	}

	Effect = -1;

	for(iEffect = 0; iEffect < lpObj->btEffectCount; iEffect++)
	{
		Effect = lpObj->pEffectInfo[iEffect].btEffectNum;
		if( Effect == 0x39 ||
			Effect == 0x3D || 
			Effect == 0x41 )
		{
			return FALSE;
		}
	}

	tdir = GetPathPacketDirPos(lpObj->X - lpTargetObj->X, lpObj->Y - lpTargetObj->Y) * 2;
	x = lpObj->X;
	y = lpObj->Y;

	for ( int n=0;n<count;n++)
	{
		if ( n >= 2 )
		{
			if ( lpObj->Class == 4 || lpObj->Class == 3 || lpObj->Class == 1 )
			{
				BackSpringCheck(x, y, tdir, lpObj->MapNumber);
			}
		}
		else
		{
			BackSpringCheck(x, y, tdir, lpObj->MapNumber);
		}
	}

	PMSG_POSISTION_SET pMove;


	pMove.h.c = 0xC1;
#if (__ENG__ == 1)
	pMove.h.headcode = 0x15;
#else
	// KOR
	pMove.h.headcode = 0x10;
#endif
	pMove.h.size = sizeof(pMove);
	pMove.X = x;
	pMove.Y = y;

	lpObj->m_Rest = 0;
	MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX, lpObj->m_OldY);
	MapC[lpObj->MapNumber].SetStandAttr(x, y);
	lpObj->m_OldX = x;
	lpObj->m_OldY = y;
	RecvPositionSetProc(&pMove, lpObj->m_Index);

	return TRUE;
}

bool gObjLevelUp(LPOBJ lpObj, __int64* addexp, int iMonsterType, int iEventType)
{
	if( g_MasterExp.LevelUp(lpObj,*addexp) )
	{
		return FALSE;
	}

#if(ENABLE_LOG_EXPERIENCE == 1)
	LogAddTD("Experience : Map[%d]-(%d,%d) [%s][%s](%d) %u %d MonsterIndex : %d, EventType : %d",
		lpObj->MapNumber,lpObj->X,lpObj->Y,lpObj->AccountID,lpObj->Name,lpObj->Level,lpObj->Experience - *addexp,*addexp,iMonsterType,iEventType);
#endif

	if ( lpObj->Level >= MAX_CHAR_LEVEL && lpObj->Experience >= gLevelExperience[-1+lpObj->Level] )
	{
		lpObj->Experience = gLevelExperience[-1+lpObj->Level];
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 112)), lpObj->m_Index, 1);
		return false;
	}

	gObjSetExpPetItem(lpObj->m_Index, (int)*addexp);

#if (CUSTOM_PETS == 1)
	gCustomPets.gObjLevelUp(lpObj->m_Index,*addexp);
#endif

	if ( lpObj->Experience < lpObj->NextExp )
	{
		return true;
	}

	*addexp = 0;
	lpObj->Experience = lpObj->NextExp;
	lpObj->Level++;

#if (ENABLE_CUSTOM_CLASSCALC == 1)
	lpObj->LevelUpPoint += g_ClassCalc.GetLevelPoint(lpObj, 0, 0);
#else
	if( lpObj->Class == CLASS_DARKLORD )
	{
		lpObj->LevelUpPoint += 7;
	}
	else if( lpObj->Class == CLASS_MAGUMSA )
	{
		lpObj->LevelUpPoint += 7;
	}
	else if( lpObj->Class == CLASS_FIGHTER )
	{
		lpObj->LevelUpPoint += 7;
	}
	else
	{
		lpObj->LevelUpPoint += 5;
	}
#endif

	if( lpObj->PlusStatQuestClear != false && lpObj->Level >= QUEST_MINLEVEL_PLUSSTAT)
	{
#if (ENABLE_CUSTOM_CLASSCALC == 1)
		lpObj->LevelUpPoint += g_ClassCalc.GetLevelPoint(lpObj, 0, 1);
#else
		lpObj->LevelUpPoint += 1;
#endif
		LogAddTD("[%s][%s] LevelUp PlusStatQuest Clear AddStat %d",
			lpObj->AccountID, lpObj->Name, lpObj->LevelUpPoint);
	}

	gObjCalCharacter(lpObj->m_Index);
#if (ENABLETEST_MUUN == 1)
	g_CMuunSystem.CheckMuunItemConditionLevelUp(lpObj);
#endif

	lpObj->MaxLife += DCInfo.DefClass[lpObj->Class].LevelLife;
	lpObj->MaxMana += DCInfo.DefClass[lpObj->Class].LevelMana;

	lpObj->Life = lpObj->MaxLife;
	lpObj->Mana = lpObj->MaxMana;
	
	lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
	lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
	
	gObjCalcShieldPoint(lpObj);
	lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
	GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield); //double, calc call to this

	//GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
	
	//Mana refill fix 1
	GCManaSend(lpObj->m_Index,(int)(lpObj->AddMana + lpObj->MaxMana),0xFEu,0,lpObj->AddBP + lpObj->MaxBP);
    GCManaSend(lpObj->m_Index, (int)lpObj->Mana, 0xFFu, 0, lpObj->BP);

	gObjNextExpCal(lpObj);	
	LogAddTD(lMsg.Get(MSGGET(2, 8)), lpObj->AccountID, lpObj->Name, lpObj->Level);

	if(lpObj->Class == CLASS_ELF)
	{
		if(lpObj->Level >= g_SkillAdditionInfo.GetInfinityArrowUseLevel() && lpObj->ChangeUP == 1)
		{
			int iAddSkillPosition = gObjMagicAdd(lpObj,0x4D,0);

			if(iAddSkillPosition >= 0)
			{
				GCMagicListOneSend(lpObj->m_Index,iAddSkillPosition,0x4D,0xDC,0,0);
				LogAddTD("[%s][%s] Add Infinity Arrow Skill (Character Level : %d)(ChangeUp: %d)",
					lpObj->AccountID,lpObj->Name,lpObj->Level,lpObj->ChangeUP);
			}
			else
			{
				LogAddTD("[%s][%s] Fail - Add Infinity Arrow Skill (Character Level : %d)(ChangeUp: %d)",
					lpObj->AccountID,lpObj->Name,lpObj->Level,lpObj->ChangeUP);
			}
		}
	}
	
#if (__CUSTOM__ == 1)
	GJSetCharacterInfo(lpObj, lpObj->m_Index, 0, FALSE);
#else
	GJSetCharacterInfo(lpObj, lpObj->m_Index, 0);
#endif

	

	gObjSetBP(lpObj->m_Index);
	GCLevelUpMsgSend(lpObj->m_Index, 1);

	if( lpObj->Level == 400 && lpObj->PartyNumber >= 0)
	{
		int PartyNumber = lpObj->PartyNumber;
		char szTmp[256];
		sprintf(szTmp,"400 LevelUp (%s)(%s) Party ",
			lpObj->AccountID,lpObj->Name);

		int tObjNum;

		for(int i = 0; i < MAX_USER_IN_PARTY;i++)
		{
			tObjNum = gParty.m_PartyS[PartyNumber].Number[i];

			if( tObjNum >= 0 )
			{
				int len = strlen(szTmp);
				sprintf(&szTmp[len],",(%s)(%s) ",gObj[tObjNum].AccountID,gObj[tObjNum].Name);
			}
		}

		LogAddTD(szTmp);
	}

	return true;
}







BOOL gObjLevelUpPointAdd(BYTE type, LPOBJ lpObj)	// type : [1:Strength][2:Agility][3:Vitality][4:Energy][5:Leadership]
{
	if ( lpObj->Type > OBJ_NPC )
	{
		LogAddC(2, lMsg.Get(MSGGET(2, 9)), lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return FALSE;
	}

	if ( lpObj->LevelUpPoint < 1 )
	{
		return FALSE;
	}

	switch ( type )
	{
		case 0:
			if( lpObj->Strength < g_MaxStatValue )	
			{
				lpObj->Strength++;
			}
			else	
			{
				return false;
			}
			break;

		case 1:
			if( lpObj->Dexterity < g_MaxStatValue )
				lpObj->Dexterity++;
			else
				return false;
			break;

		case 2:

			if( lpObj->Vitality < g_MaxStatValue )
				lpObj->Vitality++;
			else
				return false;

			lpObj->MaxLife += lpObj->VitalityToLife;
			break;

		case 3:
			if( lpObj->Energy < g_MaxStatValue )
				lpObj->Energy++;
			else
				return false;

			lpObj->MaxMana += lpObj->EnergyToMana;
			break;

		case 4:
			if( lpObj->Leadership < g_MaxStatValue )
				lpObj->Leadership++;
			else
				return false;
			break;
	}

	lpObj->LevelUpPoint--;
	gObjCalCharacter(lpObj->m_Index);

	return TRUE;
}


void gObjMonsterItemLoadScript(LPOBJ lpObj, char* script_file)
{
	int Token;
	int type;
	int index;
	int level;
	int op1;
	int op2;
	int op3;
	int incount;
	
	SMDFile=fopen(script_file, "r");

	if (SMDFile == 0)
	{
		MsgBox("file open error %s", script_file);
		return;
	}
	incount=0;

	while ( true )
	{
		Token=GetToken();
		
		if (Token == 2)
		{
			break;
		}

		if (Token == 1)
		{
			type=(int)TokenNumber;

			Token=GetToken();
			index=(int)TokenNumber;

			Token=GetToken();
			level=(int)TokenNumber;

			Token=GetToken();
			op1=(int)TokenNumber;

			Token=GetToken();
			op2=(int)TokenNumber;

			Token=GetToken();
			op3=(int)TokenNumber;

			if (  gObjMonsterInventoryInsertItem(lpObj, type, index, level, op1, op2, op3)  != 0xFF)
			{
				incount++;

				if (incount > 75)
				{
					break;
				}
			}
		}
		
	}


	fclose(SMDFile);
}


void gObjGiveItemSearch(LPOBJ lpObj, int maxlevel)
{
#if ACTIVE_ITEM_SEARCH == 0
	//goto lavelend;
#endif

//	int count;
	//while (--count)
	{

	}

#if ACTIVE_ITEM_SEARCH == 0
	//labelend:
#endif
}





BYTE gObjWarehouseInsertItem(LPOBJ lpObj, int type, int index, int level, int pos)
{
	CItem item;

	int item_type = ItemGetNumberMake(type, index);
	item.m_Level =level;
	item.Convert(item_type, 0 , 0, 0, 0, 0,0,0,-1, CURRENT_DB_VERSION);
	lpObj->pWarehouse[pos] = item;
	return pos;
}







void gObjGiveItemWarehouseSearch(LPOBJ lpObj, int maxlevel)
{
#if ACTIVE_ITEM_SEARCH == 0
	//goto lavelend;
#endif

	

#if ACTIVE_ITEM_SEARCH == 0
	//labelend:
#endif
}






BOOL gObjGuildMasterCapacityTest(LPOBJ lpObj)
{
	if (lpObj->Level < gGuildCreateLevel)
	{
		return 0;
	}
	
	if (lpObj->iResetCount < g_GuildCreateReset)
	{
		return 0;
	}

	return 1;
}







void gObjNextExpCal(LPOBJ lpObj)
{
	lpObj->NextExp = gLevelExperience[lpObj->Level];
	if (lpObj->Level > 1)
	{
		if (lpObj->Experience < gLevelExperience[lpObj->Level - 1])
			lpObj->Experience = gLevelExperience[lpObj->Level - 1];
	}
}







int  retResistance(LPOBJ lpObj, int Resistance_Type)
{
	BYTE r = lpObj->m_Resistance[Resistance_Type];

	if( r == 0xFF)
	{
		return 1;
	}

	if( (lpObj->Authority & 32) == 32 )
	{
		if( lpObj->pInventory[10].m_Type == ITEMGET(13,42) ||
			lpObj->pInventory[11].m_Type == ITEMGET(13,42) )
		{
			return 1;
		}
	}

	if ( r > 0 && gObjCheckUsedBuffEffect(lpObj,BUFF_POT_SOUL) == TRUE && (Resistance_Type == 2 || Resistance_Type == 0 ))
	{
		r += r * 50 / 100;
	}

	if ( r  == 0 )
	{
		return 0;
	}

	if (  (rand()%(r+1)) == 0 )
	{
		return 0;
	}

	return 1;
}






BOOL retCalcSkillResistance(LPOBJ lpTargetObj, BOOL isDouble)
{
	if ( isDouble != FALSE )
	{
		if ( (rand()%100) < 45 )
		{
			return TRUE;
		}
	}
	else
	{
		if ( (rand()%100) < 40 )
		{
			return TRUE;
		}
	}

	return FALSE;
}






BOOL gObjAttackQ(LPOBJ lpObj)
{
	if ( ATTRIBUTE_RANGE(lpObj->m_Attribute) )
	{
		return FALSE;
	}

	if ( lpObj->Class == 221 || lpObj->Class == 222 )
	{
		return FALSE;
	}

#if (GS_CASTLE == 1)
	if ( lpObj->Class == 277 || lpObj->Class == 283 )
	{
		if(g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
		{
			return FALSE;
		}
	}

	if ( lpObj->Class == 221 || lpObj->Class == 222 )
	{
		return FALSE;
	}

	if ( lpObj->Class == 277 && lpObj->m_btCsGateOpen == 1)
	{
		return FALSE;
	}

#endif

	if ( BC_MAP_RANGE(lpObj->MapNumber) )
	{
		if ( lpObj->Type >= OBJ_MONSTER )
		{
			int Bridge = g_BloodCastle.GetBridgeLevel(lpObj->MapNumber);

			if ( lpObj->Class == 131 && g_BloodCastle.CheckMonsterKillSuccess(Bridge) == false)
			{
				return FALSE;
			}

			if ( (((lpObj->Class - 132) < 0)?FALSE:((lpObj->Class-132) > 2)?FALSE:TRUE) !=FALSE && g_BloodCastle.CheckBossKillSuccess(Bridge) == false )
			{
				return FALSE;
			}
		}
	}

	if ( lpObj->Class != 247 && lpObj->Class != 249 && lpObj->Class  >= 100 && lpObj->Class <= 110 )
	{
		return FALSE;
	}

	if ( lpObj->Class == 523 )
	{
		return FALSE;
	}

	if ( lpObj->m_State != 2 )
	{
		return FALSE;
	}

	if( lpObj->Live == false )
	{
		return false;
	}

	if ( lpObj->Teleport == 1 )
	{
		return FALSE;
	}

	return TRUE;
}






void gObjPlayerKiller(LPOBJ lpObj, LPOBJ lpTargetObj,int MSBDamage)
{
	if (lpObj->Type != OBJ_USER) {
		return;
	}

#if (__GENS_SYSTEM__ == 1)
	if (gGensSystem.GensMap(lpObj->MapNumber)) {
		if (lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER) {
			if (lpObj->m_GensInfo.btFamily == lpTargetObj->m_GensInfo.btFamily) {
				if (!gGensSystem.GetFriendlyKill()) {
					return;
				}
			}
			
			if (lpObj->m_GensInfo.btFamily != lpTargetObj->m_GensInfo.btFamily) {
				return;
			}
		}
	}
#endif

#ifdef BEREZNUK
	if( lpTargetObj->Type == OBJ_USER && lpObj->Type == OBJ_USER )
	{
		if(lpObj->MapNumber == 79)
		{
			if( (lpObj->X >= 124 && lpObj->Y <= 132) && (lpObj->X <= 132 && lpObj->Y >= 123) )
			{
				return;
			}
		}
	}
#endif

	if (gObjGetRelationShip(lpObj, lpTargetObj) == 2) {
		if (lpObj->lpGuild!= NULL && lpTargetObj->lpGuild != NULL) {
			LogAddTD("[U.System][Rival][Player Kill] ( Killer [%s][%s] / %s / U:%d / R:%d ) vs ( Victim [%s][%s] / %s / U:%d / R:%d )",
				lpObj->AccountID, lpObj->Name, lpObj->lpGuild->Name, lpObj->lpGuild->iGuildUnion, lpObj->lpGuild->iGuildRival,
				lpTargetObj->AccountID, lpTargetObj->Name, lpTargetObj->lpGuild->Name, lpTargetObj->lpGuild->iGuildUnion, lpTargetObj->lpGuild->iGuildRival);
		} else {
			LogAddTD("[U.System][Rival][Player Kill][★ERROR : Can't find GuildInfo] (  [%s][%s] ) vs ( [%s][%s] )",
				lpObj->AccountID,lpObj->Name,lpTargetObj->AccountID,lpTargetObj->Name);
		}
		return;
	}

	if (CC_MAP_RANGE(lpTargetObj->MapNumber)){
		if (g_ChaosCastle.GetCurrentState(g_ChaosCastle.GetFloor(lpTargetObj->MapNumber)) == 2) {
			return;
		}
	}

#if (GS_CASTLE == 1)
	if (lpTargetObj->MapNumber == MAP_INDEX_CASTLESIEGE) {
		if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE) {
			if(lpObj->m_btCsJoinSide != 0) {
				return;
			}
		} else if(lpObj->Y > 113 && g_CastleSiege.CheckCastleOwnerUnionMember(lpObj->m_Index) == 1) {
			return;
		}
		
		if(lpTargetObj->m_PK_Level >= 6) {
			return;
		}
	}
#endif

	if (IT_MAP_RANGE(lpObj->MapNumber) || IT_MAP_RANGE(lpTargetObj->MapNumber)) {
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsVulcanusMap(lpObj->MapNumber) && g_NewPVP.IsVulcanusMap(lpTargetObj->MapNumber)) {
		return;
	}
#else
	if( lpObj->MapNumber == MAP_INDEX_VULCANUS )
		return;
#endif

	int PartyNumber = lpObj->PartyNumber;
	int bTmp = -1;

	for (int n=0;n<MAX_SELF_DEFENSE;n++) {
		if (lpTargetObj->SelfDefense[n] >= 0) {
			if ( lpTargetObj->SelfDefense[n] == lpObj->m_Index ) {
				return;
			}

			if (PartyNumber >= 0) {
				if (gObj[lpTargetObj->SelfDefense[n]].PartyNumber == PartyNumber) {
					return;
				}
			}
		}
	}

	int iOriginPkLevel = lpObj->m_PK_Level;
	int iOriginPkCount = lpObj->m_PK_Count;

	if (MSBDamage == 4) {
		return;
	}

	if (g_PKLevelIncreaseOff == 1) {
		return;
	}

	char PkLevel = 3;

#if (ENABLE_FIX_PARTYPK == 1)
	PkLevel = lpTargetObj->m_PK_Level;
#else

	if ( lpTargetObj->PartyNumber >= 0 )
	{
		if( gParty.GetPartyPkLevel(lpTargetObj->PartyNumber) >= 5 )
		{
			PkLevel = gParty.GetPartyPkLevel(lpTargetObj->PartyNumber);
		}
		else
		{
			PkLevel = lpTargetObj->m_PK_Level;
		}
	}
	else
	{
		PkLevel = lpTargetObj->m_PK_Level;
	}
#endif

	if ( lpObj->m_PK_Level == 3 )
	{
		if(PkLevel == 3)
		{
			lpObj->m_PK_Count = 1;
		}
		else if(PkLevel < 3)
		{
			lpObj->m_PK_Count = 1;
		}
		else if(PkLevel == 4)
		{
			lpObj->m_PK_Count = 1;
		}
		else
		{
			if(PkLevel >= 6 && lpTargetObj->Level > 20)
			{
				lpObj->m_PK_Count--;

				if(lpObj->m_PK_Count < -3)
				{
					lpObj->m_PK_Count = (BYTE)-3;
					return;
				}
			}
			else
			{
				return;
			}
		}
	}
	else if( lpObj->m_PK_Level > 3)
	{
		if(PkLevel <= 4)
		{
			lpObj->m_PK_Count++;
			if(lpObj->m_PK_Count > 100)
			{
				lpObj->m_PK_Count = 100;
			}
		}
		else
		{
			return;
		}
	}
	else if( lpObj->m_PK_Level < 3)
	{
		if(PkLevel == 3)
		{
			lpObj->m_PK_Count = 1;
		}
		else if(PkLevel < 3)
		{
			lpObj->m_PK_Count = 1;
		}
		else if(PkLevel == 4)
		{
			lpObj->m_PK_Count = 1;
		}
		else
		{
			if(PkLevel >= 6 && lpTargetObj->Level > 20)
			{
				lpObj->m_PK_Count--;

				if(lpObj->m_PK_Count < -3)
				{
					lpObj->m_PK_Count = (BYTE)-3;
				}
			}
			else
			{
				return;
			}
		}
	}

	if(lpObj->m_PK_Count == 0)
	{
		lpObj->m_PK_Level = 3;
		lpObj->m_PK_Time = 0;
	}
	else if(lpObj->m_PK_Count == 1)
	{
		lpObj->m_PK_Level = 4;
		lpObj->m_PK_Time = 0;
	}
	else if(lpObj->m_PK_Count == 2)
	{
		lpObj->m_PK_Level = 5;
		lpObj->m_PK_Time = 0;

		if( lpObj->PartyNumber >= 0 )
		{
			gParty.SetPkLevel(lpObj->PartyNumber,lpObj->m_Index,lpObj->DBNumber,lpObj->m_PK_Level);
			gParty.SetPkCount(lpObj->PartyNumber);
		}
	}
	else if(lpObj->m_PK_Count >= 3)
	{
		lpObj->m_PK_Level = 6;
		lpObj->m_PK_Time = 0;

		if( lpObj->PartyNumber >= 0 )
		{
			gParty.SetPkLevel(lpObj->PartyNumber,lpObj->m_Index,lpObj->DBNumber,lpObj->m_PK_Level);
			gParty.SetPkCount(lpObj->PartyNumber);
		}
	}
	else if(lpObj->m_PK_Count <= -3)
	{
		lpObj->m_PK_Level = 1;
		lpObj->m_PK_Time = 0;
	}
	
	GCPkLevelSend(lpObj->m_Index, lpObj->m_PK_Level);

	if (lpObj->m_PK_Level == 4) {
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 113)), lpObj->m_Index, 1);
	}

	LogAddTD("[%s][%s] PK Info Set (Killed Player) Origin(PkLevel:%d, PkCount:%d) Changed(PkLevel:%d, PkCount:%d)",
		lpObj->AccountID, lpObj->Name, iOriginPkLevel, iOriginPkCount, lpObj->m_PK_Level, lpObj->m_PK_Count);
	

}


#include "UserDieItemDrop.h"



void gObjUserDie(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	int count = 76;
	int itemdrop = 1;
	int number = 0;
	int dropresult = 0;

	if(lpObj->Type != OBJ_USER)
	{
		return;
	}

#if( ENABLE_CUSTOM_HARDCORE == 1 )
	g_HardcoreMode.ProcDie(lpObj, lpTargetObj);
#endif//ENABLE_CUSTOM_HARDCORE

	gObjClearBuffEffect(lpObj,CLEAR_TYPE_NON_PCS_ITEM_EFFECT);
	gObjSetKillCount(lpObj->m_Index,0);
	gObjUseSkill.RemoveAllCharacterInvalidMagicAndSkillState(lpObj);

	//Added By Will
	//g_DuelManager.PlayerScore(lpTargetObj);

	if(gObjTargetGuildWarCheck(lpObj,lpTargetObj) == 1)
	{
		return;
	}

	if(DS_MAP_RANGE(lpObj->MapNumber))
	{
		if(lpTargetObj->Type == OBJ_MONSTER)
		{

			LPMONSTER_ATTRIBUTE mAttr = gMAttr.GetAttr(lpTargetObj->Class);

			if(mAttr != 0)
			{
				LogAddTD("[DevilSquare] Dead In DevilSquare [%s][%s][%s]",lpObj->AccountID,lpObj->Name,mAttr->m_Name);
			}
		}

		return;
	}
	
	if(BC_MAP_RANGE(lpObj->MapNumber))
	{
		g_BloodCastle.SetUserState(lpObj->m_Index,1);

		int iBridge = g_BloodCastle.GetBridgeLevel(lpObj->MapNumber);

		if(g_BloodCastle.GetCurrentState(iBridge) == BC_STATE_PLAYING)
		{
			LogAddTD("[Blood Castle] (%d) Try to drop Ultimate Weapon [%s][%s]",iBridge+1,lpObj->AccountID,lpObj->Name);
			g_BloodCastle.SearchUserDropQuestItem(lpObj->m_Index);
		}
		else
		{
			LogAddTD("[Blood Castle] (%d) Try to delete Ultimate Weapon [%s][%s]",iBridge+1,lpObj->AccountID,lpObj->Name);
			g_BloodCastle.SearchUserDeleteQuestItem(lpObj->m_Index);
		}

		if(lpTargetObj->Type == OBJ_MONSTER)
		{
			LPMONSTER_ATTRIBUTE mAttr = gMAttr.GetAttr(lpTargetObj->Class);

			if(mAttr != 0)
			{
				LogAddTD("[Blood Castle] (%d) Dead In Blood Castle, Killed by Monster [%s][%s][%s]",iBridge+1,lpObj->AccountID,lpObj->Name,mAttr->m_Name);
				g_BloodCastle.SearchUserDropQuestItem(lpObj->m_Index);
			}
		}
		else if(lpTargetObj->Type == OBJ_USER)
		{
			LogAddTD("[Blood Castle] (%d) Dead In Blood Castle, Killed by Other User [%s][%s][%s]",iBridge+1,lpObj->AccountID,lpObj->Name,lpTargetObj->Name);
			g_BloodCastle.SearchUserDropQuestItem(lpObj->m_Index);
		}
		return;
	}
#if (ENABLETEST_ARCA == 1)
	else if (lpObj->MapNumber == 97 && g_ArcaBattle.IsArcaBattleServer() && !g_AcheronGuardianEvent.IsPlayStart())
	{
		if (g_ArcaBattle.GetState() == 7)
		{
			g_ArcaBattle.AddContributePoint(*lpTargetObj, *lpObj);
            g_ArcaBattle.AddKillPoint(*lpTargetObj, *lpObj);
		}
	}
#endif
	else if(IT_MAP_RANGE(lpObj->MapNumber))
	{
		g_IllusionTemple.UserDie(lpObj);
	}
	else if(CC_MAP_RANGE(lpObj->MapNumber))
	{
		return;
	}
	else if(DG_MAP_RANGE(lpObj->MapNumber))
	{
		if(lpTargetObj->Type == OBJ_MONSTER)
		{
			LPMONSTER_ATTRIBUTE mAttr = gMAttr.GetAttr(lpTargetObj->Class);

			if(mAttr != 0)
			{
				LogAddTD("[Doppelganger] Dead In Doppelganger, Killed by Monster [%s][%s][%s]",lpObj->AccountID,lpObj->Name,mAttr->m_Name);
			}
		}
		else if(lpTargetObj->Type == OBJ_USER)
		{
			LogAddTD("[Doppelganger] Dead In Doppelganger, Killed by Other User [%s][%s][%s]",lpObj->AccountID,lpObj->Name,lpTargetObj->Name);
		}
		return;
	}
	else if( IF_MAP_RANGE(lpObj->MapNumber) )
	{
		if(lpTargetObj->Type == OBJ_MONSTER)
		{
			LPMONSTER_ATTRIBUTE mAttr = gMAttr.GetAttr(lpTargetObj->Class);

			if(mAttr != 0)
			{
				LogAddTD("[ImperialFort] Dead In Imperial Fort, Killed by Monster [%s][%s][%s]",lpObj->AccountID,lpObj->Name,mAttr->m_Name);
			}
		}
		else if(lpTargetObj->Type == OBJ_USER)
		{
			LogAddTD("[ImperialFort] Dead In Imperial Fort, Killed by Other User [%s][%s][%s]",lpObj->AccountID,lpObj->Name,lpTargetObj->Name);
		}
		return;
	}
#if (GS_CASTLE == 1 )
	else if(g_CastleSiege.GetCastleState() == 7 && lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
	{
		return;
	}
#endif
#if (__GENS_SYSTEM__ == 1)
	else if(gGensSystem.GensMap(lpObj->MapNumber) &&
			lpObj->m_GensInfo.btFamily != GENS_NONE &&
			lpTargetObj->m_GensInfo.btFamily != GENS_NONE &&
			lpObj->m_GensInfo.btFamily != lpTargetObj->m_GensInfo.btFamily)
	{

		int iOldContrib1 = lpObj->m_GensInfo.iContribution;
		int iOldContrib2 = lpTargetObj->m_GensInfo.iContribution;

		gGensSystem.UserDie(lpObj->m_Index,lpTargetObj->m_Index);

		LogAddTD("[GensSystem] [%s][%s] Contrib:[%d/%d] Dead In BattleZone[%d], Killed by Other User [%s][%s] Contrib:[%d/%d]",
			lpObj->AccountID,lpObj->Name,lpObj->m_GensInfo.iContribution,iOldContrib1,lpObj->MapNumber,lpTargetObj->AccountID,lpTargetObj->Name,lpTargetObj->m_GensInfo.iContribution,iOldContrib2);
		return;
	}
#endif
	else if( (lpObj->Authority&16) || (lpTargetObj->Authority&16) )
	{
		return;
	}
	else if( gObjCheckUsedBuffEffect(lpObj, BUFF_TALISMAN_GUARDIAN) )
	{
		return;
	}


	if(lpTargetObj->Type == OBJ_MONSTER)
	{
		LogAddTD("[%s][%s] Killed [%s][%s] Pos:(%d,%d,%d)","Monster",lpTargetObj->Name,lpObj->AccountID,lpObj->Name,lpObj->MapNumber,lpObj->X,lpObj->Y);
	}
	else
	{
		LogAddTD("[%s][%s] (PkLevel = %d) Killed [%s][%s] (PkLevel = %d) Pos:(%d,%d,%d)",lpTargetObj->AccountID,lpTargetObj->Name,lpTargetObj->m_PK_Level,lpObj->AccountID,lpObj->Name,lpObj->m_PK_Level,lpObj->MapNumber,lpObj->X,lpObj->Y);
		LogAddTD("Killed by User State Victim:[%s][%s], Murderer:[%s][%s], Dead State (Duel:%d, GuildWar:%d, SeldDefense:%d)",lpObj->AccountID,lpObj->Name,lpTargetObj->AccountID,lpTargetObj->Name,!!IsOnDuel(lpTargetObj->m_Index,lpObj->m_Index),!!gObjTargetGuildWarCheck(lpObj,lpTargetObj),!!gObjIsSelfDefense(lpTargetObj,lpObj->m_Index));
	}

	/*if(gPkLimitFree == 0)
	{
		if(lpObj->m_PK_Level == 2)
		{
			itemdrop = rand()%32;
		}
		else if(lpObj->m_PK_Level == 3)
		{
			itemdrop = rand()%16;
		}
		else if(lpObj->m_PK_Level == 4)
		{
			itemdrop = rand()%8;
		}
		else if(lpObj->m_PK_Level == 5)
		{
			itemdrop = rand()%4;
		}
		else if(lpObj->m_PK_Level >= 6)
		{
			itemdrop = rand()%2;
		}
		else
		{
			itemdrop = rand()%64;
		}
	}
	else
	{
		if(lpObj->m_PK_Level == 2)
		{
			itemdrop = rand()%128;
		}
		else if(lpObj->m_PK_Level == 3)
		{
			itemdrop = rand()%64;
		}
		else if(lpObj->m_PK_Level == 4)
		{
			itemdrop = rand()%16;
		}
		else if(lpObj->m_PK_Level == 5)
		{
			itemdrop = rand()%4;
		}
		else if(lpObj->m_PK_Level >= 6)
		{
			itemdrop = rand()%2;
		}
		else
		{
			itemdrop = rand()%256;
		}
	}

	if(gPkItemDrop == 0)
	{
		if(lpTargetObj->m_PK_Level >= 4 && gPkLimitFree == 0)
		{
			itemdrop = 1;
		}
	}

	if(lpTargetObj->Type == OBJ_USER && gPkLimitFree == 0)
	{
		itemdrop = 1;
	}

	if(gObjCanItemTouch(lpObj,0)==0)
	{
		itemdrop = 1;
	}

	if(lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER)
	{
		if(IsOnDuel(lpObj->m_Index,lpTargetObj->m_Index))
		{
			itemdrop = 1;
		}
	}

#if (GS_CASTLE == 1)
	if( g_Crywolf.GetCrywolfState() == 4 &&
		((lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE) ? TRUE : FALSE) )
	{
		itemdrop = 1;
	}
#endif

	if(itemdrop == 0)
	{
		if(lpObj->m_PK_Level >= 4)
		{
			count = 24;

			while(count-- != 0)
			{
				number = rand()%12;

				if(lpObj->pInventory[number].IsItem()==1)
				{
					if(g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(&lpObj->pInventory[number])==0)
					{
						PMSG_ITEMTHROW lpMsg;
						lpMsg.Ipos = number;
						lpMsg.px = lpObj->X;
						lpMsg.py = lpObj->Y;
						if(CGItemDropRequest(&lpMsg,lpObj->m_Index,1)==1)
						{
							dropresult = 1;
							break;
						}
					}
				}
			}
		}

		if(dropresult == 0)
		{
			count = INVENTORY_MAP_SIZE;

			while(count-- != 0)
			{
				number = rand()%INVENTORY_MAP_SIZE + 12;

				if(lpObj->pInventory[number].IsItem()==1)
				{
					if(g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(&lpObj->pInventory[number])==0)
					{
						if(lpObj->pInventory[number].m_Type >= ITEMGET(13,20) && (lpObj->pInventory[number].m_Level >= 1 && lpObj->pInventory[number].m_Level <= 2 ))
						{
							continue;
						}

						PMSG_ITEMTHROW lpMsg;
						lpMsg.Ipos = number;
						lpMsg.px = lpObj->X;
						lpMsg.py = lpObj->Y;

						if(CGItemDropRequest(&lpMsg,lpObj->m_Index,1)==1)
						{
							dropresult = 1;
							LogAddTD("[%s][%s] User Die Item Drop, ItemName:%s",lpObj->AccountID,lpObj->Name,lpObj->pInventory[number].GetName());
							break;
						}
					}
				}
			}
		}
	}*/
	g_UserDieItemDrop.Drop(lpObj, lpTargetObj);

#if (ENABLETEST_NEWPVP == 1)
	if(lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER ) 
	{
		if (IsOnDuel(lpObj->m_Index,lpTargetObj->m_Index))
		{
			//lpTargetObj->m_btDuelScore += 1;
			//GCSendDuelScore(lpObj->m_Index,lpTargetObj->m_Index);
			g_NewPVP.SetScore(*lpTargetObj);
		}
		return;
	}
#endif

	__int64 minexp = (DWORD)gLevelExperience[-1 + lpObj->Level];
	__int64 maxexp = (DWORD)gLevelExperience[lpObj->Level];
	__int64 subexp = 0;
	__int64 decexp = 0;
	int decexprate = 0;

	char PkLevel = 3;

#if (ENABLE_FIX_PARTYPK == 1)

	PkLevel = lpObj->m_PK_Level;

#else
	if( lpObj->PartyNumber >= 0 )
	{
		if( gParty.GetPartyPkLevel(lpObj->PartyNumber) >= 5 )
		{
			PkLevel = gParty.GetPartyPkLevel(lpObj->PartyNumber);
		}
		else
		{
			PkLevel = lpObj->m_PK_Level;
		}
	}
	else
	{
		PkLevel = lpObj->m_PK_Level;
	}
#endif

	if( g_MasterExp.IsEnabled(lpObj) )
	{
		minexp = g_MasterExp.GetMaxExp(lpObj->MasterLevel);
		maxexp = g_MasterExp.GetMaxExp(lpObj->MasterLevel+1);
		decexprate = g_MasterExp.GetDecExp(lpObj);
	}
	else
	{
		if(lpObj->Level <= 10)
		{
			decexprate = 0;
		}
		else if(lpObj->Level <= 150)
		{
			if(PkLevel == 2)
			{
				decexprate = 20;
			}
			else if(PkLevel == 3)
			{
				decexprate = 10;
			}
			else if(PkLevel == 4)
			{
				decexprate = 50;
			}
			else if(PkLevel == 5)
			{
				decexprate = 60;
			}
			else if(PkLevel >= 6)
			{
				decexprate = 70;
			}
			else
			{
				decexprate = 20;
			}
		}
		else if(lpObj->Level <= 220)
		{
			if(PkLevel == 2)
			{
				decexprate = 10;
			}
			else if(PkLevel == 3)
			{
				decexprate = 10;
			}
			else if(PkLevel == 4)
			{
				decexprate = 40;
			}
			else if(PkLevel == 5)
			{
				decexprate = 50;
			}
			else if(PkLevel >= 6)
			{
				decexprate = 60;
			}
			else
			{
				decexprate = 10;
			}
		}
		else
		{
			if(lpObj->m_PK_Level == 2)
			{
				decexprate = 10;
			}
			else if(PkLevel == 3)
			{
				decexprate = 10;
			}
			else if(PkLevel == 4)
			{
				decexprate = 30;
			}
			else if(PkLevel == 5)
			{
				decexprate = 40;
			}
			else if(PkLevel >= 6)
			{
				decexprate = 50;
			}
			else
			{
				decexprate = 10;
			}
		}
	}

	decexp = 0;

	if( decexprate > 0 )
	{
		decexp = (maxexp - minexp) * decexprate / 1000;
	}

#if (GS_CASTLE == 1)

	if( g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_START && lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
	{
		decexp = 0;
	}

#endif

	if( g_MasterExp.IsEnabled(lpObj) )
	{
		subexp = lpObj->MasterExperience - decexp;
	}
	else
	{
		subexp = (DWORD)lpObj->Experience - decexp;
	}

	if( subexp < minexp )
	{
		subexp = minexp;
	}

	LogAddTD("[%s][%s] Death reduces Experience %u - %u",lpObj->AccountID,lpObj->Name,subexp,decexp);

	DWORD submoney = 0;
	int subrate = 0;

	if(lpObj->Money > 0)
	{
		 subrate = g_MasterExp.GetSubMoney(lpObj);

		if( subrate < 0 )
		{
			if(lpObj->Level > 10 && lpObj->Level <= 99)
			{
				subrate = 1;
			}
			else if(lpObj->Level >= 100 && lpObj->Level <= 199)
			{
				subrate = 2;
			}
			else if(lpObj->Level > 200)
			{
				subrate = 3;
			}
		}

		if( subrate > 0 )
		{
			submoney = (DWORD)lpObj->Money * subrate / 100;
		}

		lpObj->Money -= submoney;

		if( lpObj->Money < 0 )
		{
			lpObj->Money = 0;
		}

		LogAddTD(lMsg.Get(574),lpObj->AccountID,lpObj->Name,lpObj->Money,submoney);
	}

	if(lpObj->WarehouseMoney > 0)
	{
		submoney = 0;
		subrate = g_MasterExp.GetSubMoney(lpObj);

		if( subrate < 0 )
		{
			if(lpObj->Level > 10 && lpObj->Level <= 99)
			{
				subrate = 1;
			}
			else if(lpObj->Level >= 100 && lpObj->Level <= 199)
			{
				subrate = 2;
			}
			else if(lpObj->Level > 200)
			{
				subrate = 3;
			}
		}

		if( subrate > 0 )
		{
			submoney = (DWORD)lpObj->Money * subrate / 100;
		}
	
		lpObj->WarehouseMoney = lpObj->WarehouseMoney - submoney;
		
		if(lpObj->WarehouseMoney < 0)
		{
			lpObj->WarehouseMoney = 0;
		}

		GDSetWarehouseMoney(lpObj->m_Index);

		LogAddTD(lMsg.Get(575),lpObj->AccountID,lpObj->Name,lpObj->WarehouseMoney,submoney);
	}

	if( g_MasterExp.IsEnabled(lpObj) )
	{
		lpObj->MasterExperience = subexp;
	}
	else
	{
		lpObj->Experience = (DWORD)subexp;
	}
}






void gObjInterfaceTimeCheck(LPOBJ lpObj)
{
	int tObjNum;
	BOOL TimeCheck = FALSE;

	if ( lpObj->m_IfState.use == 0 )
	{
		return;
	}

	if ( (GetTickCount() - lpObj->m_InterfaceTime) < 5000 )
	{
		return;
	}

	if ( lpObj->m_IfState.state == 1 )
	{
		return;
	}

	tObjNum = lpObj->TargetNumber;

	if ( tObjNum < 0 )
	{
		return;
	}

	if ( lpObj->m_IfState.type == 1 )
	{
		CGTradeResult(lpObj->m_Index, 3);
		CGTradeResult(tObjNum, 3);

		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 114)), tObjNum, 1);
		TimeCheck = TRUE;
	}
	else
	{
		if ( lpObj->m_IfState.type == 2 )
		{
			lpObj->PartyTargetUser = -1;
			gObj[tObjNum].PartyTargetUser = -1;
			GCResultSend(tObjNum, 0x41, FALSE );
			TimeCheck = TRUE;
		}
	}

	if ( TimeCheck != FALSE )
	{
		lpObj->m_IfState.use = 0;
		lpObj->TargetNumber = -1;
		gObj[tObjNum].m_IfState.use = 0;
		gObj[tObjNum].TargetNumber = -1;
	}

}






void gObjPkDownTimeCheck(LPOBJ lpObj, int TargetLevel)
{

	if(lpObj->m_PK_Level == 3)
	{
		return;
	}

#if (ENABLE_CUSTOM_OFFLINETRADE==1)
	if(lpObj->bOffTrade)
		return;
#endif

	unsigned int dwtime = ((GetTickCount() - lpObj->m_dwPKTimer) / 1000) * gPkTime;

	if(dwtime > 5)
	{
		dwtime = 2;
		lpObj->m_dwPKTimer = GetTickCount();
	}
	else
	{
		lpObj->m_dwPKTimer = dwtime * 1000;
	}

	if(TargetLevel < 2)
	{
		lpObj->m_PK_Time += dwtime;
	}
	else
	{
		lpObj->m_PK_Time += TargetLevel/2;
	}

	if(lpObj->m_PK_Level < 3)
	{
		if(lpObj->m_PK_Time > 3600)
		{
			lpObj->m_PK_Time = 0;
			lpObj->m_PK_Count += 1;

			if(lpObj->m_PK_Count > 100)
			{
				lpObj->m_PK_Count = 100;
			}

			lpObj->m_PK_Level += 1;

			GCPkLevelSend(lpObj->m_Index,lpObj->m_PK_Level);
		}
	}
	else if(lpObj->m_PK_Level > 3)
	{
		if(lpObj->m_PK_Count > 3)
		{
			if(lpObj->m_PK_Time > 3600)
			{
				lpObj->m_PK_Time = 0;
				lpObj->m_PK_Count -= 1;

				if(lpObj->m_PK_Count <= 0)
				{
					lpObj->m_PK_Count = 0;
					lpObj->m_PK_Level = 3;
				}
				else if(lpObj->m_PK_Count == 2)
				{
					lpObj->m_PK_Level = 5;
				}
				else if(lpObj->m_PK_Count == 1)
				{
					lpObj->m_PK_Level = 4;
				}

				if(lpObj->m_PK_Level <= 5)
				{
					GCPkLevelSend(lpObj->m_Index,lpObj->m_PK_Level);
				}

				if(lpObj->m_PK_Level == 4)
				{
					GCServerMsgStringSend(lMsg.Get(MSGGET(4,71)),lpObj->m_Index,1);	
				}

				if( lpObj->PartyNumber >= 0 )
				{
					gParty.SetPkLevel(lpObj->PartyNumber,lpObj->m_Index,lpObj->DBNumber,lpObj->m_PK_Level);
					gParty.SetPkCount(lpObj->PartyNumber);
				}
			}
		}
		else if(lpObj->m_PK_Time > 10800)
		{
			lpObj->m_PK_Time = 0;
			lpObj->m_PK_Count -= 1;

			if(lpObj->m_PK_Count <= 0)
			{
				lpObj->m_PK_Count = 0;
				lpObj->m_PK_Level = 3;
			}
			else if(lpObj->m_PK_Count == 2)
			{
				lpObj->m_PK_Level = 5;
			}
			else if(lpObj->m_PK_Count == 1)
			{
				lpObj->m_PK_Level = 4;
			}

			if(lpObj->m_PK_Level <= 5)
			{
				GCPkLevelSend(lpObj->m_Index,lpObj->m_PK_Level);
			}

			if(lpObj->m_PK_Level == 4)
			{
				GCServerMsgStringSend(lMsg.Get(1137),lpObj->m_Index,1);
			}

			if( lpObj->PartyNumber >= 0 )
			{
				gParty.SetPkLevel(lpObj->PartyNumber,lpObj->m_Index,lpObj->DBNumber,lpObj->m_PK_Level);
				gParty.SetPkCount(lpObj->PartyNumber);
			}
		}
	}
}







BOOL gObjAngelSprite(LPOBJ lpObj)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem * Angel = &lpObj->pInventory[8];

	if ( Angel->m_Type == ITEMGET(13,0) && Angel->m_Durability > 0.0f )
	{
		if( Angel->m_bExpired )
		{
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}






BOOL gObjSatanSprite(LPOBJ lpObj)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem * Satan = &lpObj->pInventory[8];

	if ( Satan->m_Type == ITEMGET(13,1) && Satan->m_Durability > 0.0f )
	{
		if( Satan->m_bExpired )
		{
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}

BOOL gObjWingSprite(LPOBJ lpObj)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( lpObj->pInventory[7].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem * Wing = &lpObj->pInventory[7];

	if ( (Wing->m_Type >= ITEMGET(12,0) && Wing->m_Type <= ITEMGET(12,6)) ||
		 (Wing->m_Type >= ITEMGET(12,36) && Wing->m_Type <= ITEMGET(12,40)) ||
		 (Wing->m_Type >= ITEMGET(12,41) && Wing->m_Type <= ITEMGET(12,43) ) ||
		 (Wing->m_Type >= ITEMGET(12,130) && Wing->m_Type <= ITEMGET(12,135)) ||
		Wing->m_Type == ITEMGET(12,49) ||
		Wing->m_Type == ITEMGET(12,50) ||
		Wing->m_Type == ITEMGET(13,30) ||
		 (Wing->m_Type >= ITEMGET(12,262) && Wing->m_Type <= ITEMGET(12,267) )
#if (CUSTOM_WINGS == 1)

		|| (Wing->m_Type >= ITEMGET(12,440) && Wing->m_Type <= ITEMGET(12,445) )

#endif
		)
	{
		if( Wing->m_bExpired )
		{
			return FALSE;
		}

		if ( Wing->m_Durability > 0.0f )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}






BOOL gObjUniriaSprite(LPOBJ lpObj)
{
	if ( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem * Uniria = &lpObj->pInventory[8];

	if ( Uniria->m_Type == ITEMGET(13,2) && Uniria->m_Durability > 0.0f )
	{
		if( Uniria->m_bExpired )
		{
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}



typedef struct tagITEM_BYTE_DB
{
	BYTE btType;
	BYTE btOption;
	BYTE btDurability;
	BYTE btExcellentOption;
	BYTE btAncientOption;

} ITEM_BYTE_DB, * LPITEM_BYTE_DB;


void gObjSpriteDamage(LPOBJ lpObj, int damage)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return;
	}

	int send_dur = 0;

	if ( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return;
	}

	float fN = 10.0f;
	CItem * sprite = &lpObj->pInventory[8];
	float fdamage = (float)damage;

	if( sprite->m_bPeriodItem )
	{
		return;
	}

	if ( sprite->m_Type == ITEMGET(13,0) || sprite->m_Type == ITEMGET(13,67) ) 
	{
		if( lpObj->m_MPSkillOpt.MpsDownDur3 > 0.0f )
		{
			fdamage = damage/lpObj->m_MPSkillOpt.MpsDownDur3;
		}

		fdamage = fdamage*3/10.0f;
		fdamage /= fN;

		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}
	else if ( sprite->m_Type == ITEMGET(13,1) )
	{
		if( lpObj->m_MPSkillOpt.MpsDownDur3 > 0.0f )
		{
			fdamage = damage/lpObj->m_MPSkillOpt.MpsDownDur3;
		}

		fdamage = (fdamage*2)/10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}
	else if( sprite->m_Type == ITEMGET(13,67) )
	{
		fdamage = (damage*3)/10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	if ( sprite->m_Type == ITEMGET(13,64) ||
		 sprite->m_Type == ITEMGET(13,65) ||
		 sprite->m_Type == ITEMGET(13,80)
		 || sprite->m_Type == ITEMGET(13,106)
		 || sprite->m_Type == ITEMGET(13,123)
		 )
	{
		fN += 10.0f;
	}

	if ( sprite->m_Type == ITEMGET(13,64) )
	{
		fdamage = (damage*3)/10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}
	else if (	sprite->m_Type == ITEMGET(13,65) ||
				sprite->m_Type == ITEMGET(13,80)
				|| sprite->m_Type == ITEMGET(13,106)
				|| sprite->m_Type == ITEMGET(13,123)
				)
	{
		fdamage = (damage*2)/10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}
	else if ( sprite->m_Type == ITEMGET(13,2) )
	{
		if( lpObj->m_MPSkillOpt.MpsDownDur3 > 0.0f )
		{
			fdamage = damage/lpObj->m_MPSkillOpt.MpsDownDur3;
		}

		fdamage = fdamage/10.0f;
		fdamage /= 10.0f;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}
	else if ( sprite->m_Type == ITEMGET(13,3) )
	{
		if( lpObj->m_MPSkillOpt.MpsDownDur3 > 0.0f )
		{
			fdamage = damage/lpObj->m_MPSkillOpt.MpsDownDur3;
		}

		fdamage /= 10.0f;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}		
	else if ( sprite->m_Type == ITEMGET(13,4) )
	{
		if ( sprite->m_Durability < 1.0f )
		{
			return;
		}

		fdamage = (damage*2)/10.0f;
		fdamage /= 10.0f;
		fdamage += 1.0f;
		sprite->m_DurabilitySmall += (int)fdamage;

		int SmallDur = 1500;

		if ( sprite->m_DurabilitySmall > (SmallDur+lpObj->m_MPSkillOpt.MpsPetDurDownSpeed) )
		{
			sprite->m_Durability -= 1.0f;
			sprite->m_DurabilitySmall = 0;
			send_dur = 1;

			if ( sprite->m_Durability < 1.0f )
			{
				sprite->m_Durability = 0;

				int iPetExperience = lpObj->pInventory[8].m_PetItem_Exp;

				if ( sprite->DecPetItemExp(10) != FALSE )
				{
					LogAddTD("[%s][%s][PetItemExpDown] [%s] Level:[%d]Exp:[%d]DecExp[%d]",
						lpObj->AccountID, lpObj->Name, lpObj->pInventory[8].GetName(),
						lpObj->pInventory[8].m_PetItem_Level, lpObj->pInventory[8].m_PetItem_Exp,
						iPetExperience - lpObj->pInventory[1].m_PetItem_Exp);
					CDarkSpirit::SendLevelmsg(lpObj->m_Index, 8, 1, (BYTE)-1);
				}

				LogAddTD("[%s][%s] Pet Item is Broken because durability is exhausted [%d]",
						lpObj->AccountID,lpObj->Name,lpObj->pInventory[8].m_Number);
			}

			GCItemDurSend(lpObj->m_Index, 8, (BYTE)sprite->m_Durability, 0);
		}
	
		return;
	}
	else if(sprite->m_Type == ITEMGET(13,37))
	{
		if ( sprite->m_Durability < 1.0f )
		{
			return;
		}

		int iMaxDurSmall = g_iFenrirDefaultMaxDurSmall;
		int iMultipleDmg = 2;

		if(lpObj->Class == CLASS_WIZARD)
		{

		}
		else if(lpObj->Class == CLASS_KNIGHT)
		{

		}
		else if(lpObj->Class == CLASS_ELF)
		{
			iMaxDurSmall = g_iFenrirElfMaxDurSmall;
			iMultipleDmg = 2;
		}

		if( lpObj->m_MPSkillOpt.MpsDownDur3 > 0.0f )
		{
			fdamage = damage/lpObj->m_MPSkillOpt.MpsDownDur3;
		}

		fdamage = fdamage*iMultipleDmg / 10.0f;
		fdamage /= 10.0f;
		fdamage += 1.0f;

		sprite->m_DurabilitySmall += (int)fdamage;

		if(sprite->m_DurabilitySmall > iMaxDurSmall)
		{
			sprite->m_Durability -= 1.0f;
			sprite->m_DurabilitySmall = 0;

			if(sprite->m_Durability < 1.0f)
			{
				sprite->m_Durability = 0;
			}

			send_dur = 1;
		}
	}

	if ( send_dur != FALSE )
	{
		GCItemDurSend(lpObj->m_Index, 8, (BYTE)sprite->m_Durability, 0);

		if ( sprite->m_Durability < 1.0f )
		{
			int itemnum = sprite->m_Type;

			LogAddTD(lMsg.Get(MSGGET(2, 10)), lpObj->AccountID, lpObj->Name,
				lpObj->pInventory[8].GetName(), lpObj->pInventory[8].m_Level);
			gObjInventoryDeleteItem(lpObj->m_Index, 8);
			GCInventoryItemDeleteSend(lpObj->m_Index, 8, 0);
			gObjMakePreviewCharSet(lpObj->m_Index);
			GCEquipmentChange(lpObj->m_Index, 8);

			if ( BC_MAP_RANGE(lpObj->MapNumber) || CC_MAP_RANGE(lpObj->MapNumber) )
			{
				if ( itemnum == ITEMGET(13,0) || itemnum == ITEMGET(13,1) ||
					 itemnum == ITEMGET(13,64) || itemnum == ITEMGET(13,65) )
				{
					int pos = -1;

					for ( int i=MAX_PLAYER_EQUIPMENT;i<MAIN_INVENTORY_SIZE;i++)
					{
						if ( lpObj->pInventory[i].IsItem() == TRUE )
						{
							if ( lpObj->pInventory[i].m_Type == itemnum )
							{
								pos = i;
								break;
							}
						}
					}

					if ( pos != -1 )
					{
						BYTE ItemInfo[MAX_ITEM_INFO];
						BOOL DurSend;
						BOOL DurTargetSend;

						ItemByteConvert(ItemInfo, lpObj->pInventory[pos]);
						gObjInventoryMoveItem(lpObj->m_Index, pos, 8, DurSend, DurTargetSend, 0, 0, ItemInfo);
						GCItemMoveResultSend(lpObj->m_Index, 0, 8, ItemInfo);
						GCInventoryItemDeleteSend(lpObj->m_Index, pos, 0);
						GCServerMsgStringSend(lMsg.Get(MSGGET(4, 158)), lpObj->m_Index, 1);
					}
				}
			}
				
			if ( lpObj->MapNumber == MAP_INDEX_ICARUS && itemnum == ITEMGET(13,3) || lpObj->MapNumber == MAP_INDEX_ICARUS && itemnum == ITEMGET(13,37))
			{
				int pos = -1;

				for ( int i=MAX_PLAYER_EQUIPMENT;i<MAIN_INVENTORY_SIZE;i++)
				{
					if ( lpObj->pInventory[i].IsItem() == TRUE )
					{
						if ( lpObj->pInventory[i].m_Type == ITEMGET(13,3) )
						{
							pos = i;
							break;
						}
					}
				}

				if ( pos == -1 )
				{
					if ( lpObj->pInventory[7].IsItem() == FALSE )
					{
						gObjMoveGate(lpObj->m_Index, 22);
					}
				}
				else
				{
					BYTE ItemInfo[MAX_ITEM_INFO];
					BOOL DurSend;
					BOOL DurTargetSend;

					ItemByteConvert(ItemInfo, lpObj->pInventory[pos]);
					gObjInventoryMoveItem(lpObj->m_Index, pos, 8, DurSend, DurTargetSend, 0, 0, ItemInfo);
					GCItemMoveResultSend(lpObj->m_Index, 0, 8, ItemInfo);
					GCInventoryItemDeleteSend(lpObj->m_Index, pos, 0);
					GCServerMsgStringSend(lMsg.Get(MSGGET(4, 158)), lpObj->m_Index, 1);
				}
			}
		}

	}
}






void gObjSecondDurDown(LPOBJ lpObj)
{
	lpObj->m_TimeCount +=1;

	int ret = 0;
	int reCalCharacter = 0;

	if(lpObj->m_Change >= 0)
	{
		gObjChangeDurProc(lpObj);
	}

	BYTE attr = MapC[lpObj->MapNumber].GetAttr(lpObj->X,lpObj->Y);

	if(lpObj->m_TimeCount % 10 != 0)
	{
		return;
	}

#ifdef POINTEX
	if( !lpObj->bOffTrade )
	{
		lpObj->m_ShopPointExTime++;
		lpObj->m_ShopPointExTimeBackup++;
		g_ShopPointEx.AddTimeBonus(lpObj->m_Index);
	}
#endif

	for(int i = 2; i < 7; i++)	{
		if( lpObj->pInventory[i].IsItem() && lpObj->pInventory[i].m_bLuckySet )
		{
			CItem* lpItem = &lpObj->pInventory[i];

			int m_Durability = (int)lpItem->m_Durability;
			lpItem->m_Durability -= 0.00417f;

			if(m_Durability > (int)lpItem->m_Durability)
			{
				if( lpItem->m_Durability < 1)
				{
					lpItem->m_Durability = 0;
				}

				BYTE dur = (BYTE)lpItem->m_Durability;
				GCItemDurSend(lpObj->m_Index,i,dur,0);

				if(dur == 0)
				{
					LogAddTD(lMsg.Get(544),lpObj->AccountID,lpObj->Name,lpItem->GetName(),lpItem->m_Level,dur);
					lpItem->Clear();
					GCInventoryItemDeleteSend(lpObj->m_Index,i,0);
				}
			}
		}
	}

	if(lpObj->pInventory[7].IsItem() == 1 )
	{
		ret = lpObj->pInventory[7].DurabilityDown(1,lpObj->m_Index);

		if(ret != 0)
		{
			GCItemDurSend2(lpObj->m_Index,7,(BYTE)lpObj->pInventory[7].m_Durability,0);
		}

		if(ret == 2)
		{
			reCalCharacter = 1;
		}
	}

	if((attr&1)==0)
	{
		if(lpObj->pInventory[10].IsItem() == 1)
		{
			if(lpObj->pInventory[10].m_Type == ITEMGET(13,20) && lpObj->pInventory[10].m_Level == 0)
			{
				ret = lpObj->pInventory[10].DurabilityDown(70,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,10,(BYTE)lpObj->pInventory[10].m_Durability,0);
				}

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
			else if(lpObj->pInventory[10].m_Type == ITEMGET(13,20) && lpObj->pInventory[10].m_Level == 3)
			{
				ret = lpObj->pInventory[10].DurabilityDown(70,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,10,(BYTE)lpObj->pInventory[10].m_Durability,0);
				}

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
			else if(lpObj->pInventory[10].m_Type == ITEMGET(13,38))
			{
				ret = lpObj->pInventory[10].DurabilityDown(63,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,10,(BYTE)lpObj->pInventory[10].m_Durability,0);
				}
			}
			else if(lpObj->pInventory[10].m_Type == ITEMGET(13,107) )
			{
				ret = lpObj->pInventory[10].DurabilityDown(70,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,10,(BYTE)lpObj->pInventory[10].m_Durability,0);
				}

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
			else if(lpObj->pInventory[10].m_Type != ITEMGET(13,10))
			{
				ret = lpObj->pInventory[10].DurabilityDown(1,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,10,(BYTE)lpObj->pInventory[10].m_Durability,0);
				}

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
		}

		if(lpObj->pInventory[11].IsItem() == 1)
		{
			if(lpObj->pInventory[11].m_Type == ITEMGET(13,20) && lpObj->pInventory[11].m_Level == 0)
			{
				ret = lpObj->pInventory[11].DurabilityDown(70,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,11,(BYTE)lpObj->pInventory[11].m_Durability,0);
				}

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
			else if(lpObj->pInventory[11].m_Type == ITEMGET(13,20) && lpObj->pInventory[11].m_Level == 3)
			{
				ret = lpObj->pInventory[11].DurabilityDown(70,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,11,(BYTE)lpObj->pInventory[11].m_Durability,0);
				}

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
			else if(lpObj->pInventory[11].m_Type == ITEMGET(13,38))
			{
				ret = lpObj->pInventory[11].DurabilityDown(63,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,11,(BYTE)lpObj->pInventory[11].m_Durability,0);
				}
			}
			else if(lpObj->pInventory[11].m_Type == ITEMGET(13,107) )
			{
				ret = lpObj->pInventory[11].DurabilityDown(70,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,11,(BYTE)lpObj->pInventory[11].m_Durability,0);
				}

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
			else if(lpObj->pInventory[11].m_Type != ITEMGET(13,10))
			{
				ret = lpObj->pInventory[11].DurabilityDown(1,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,11,(BYTE)lpObj->pInventory[11].m_Durability,0);
				}

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
		}

		if(lpObj->pInventory[9].IsItem() == 1)
		{
			if(lpObj->pInventory[9].m_Type != ITEMGET(13,10))
			{
				ret = lpObj->pInventory[9].DurabilityDown(1,lpObj->m_Index);

				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,9,(BYTE)lpObj->pInventory[9].m_Durability,0);
				}

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
		}
	}

	if(reCalCharacter != 0)
	{
		gObjCalCharacter(lpObj->m_Index);
	}
}






void gObjChangeDurProc(LPOBJ lpObj)
{

	if( lpObj->pInventory[10].IsItem() )
	{
		switch( lpObj->pInventory[10].m_Type )
		{
		case ITEMGET(13,10):
		case ITEMGET(13,39):
		case ITEMGET(13,40):
		case ITEMGET(13,41):
		case ITEMGET(13,68):
		case ITEMGET(13,76):
		case ITEMGET(13,77):
		case ITEMGET(13,78):
		case ITEMGET(13,122):
			{
				CItem* lpItem = &lpObj->pInventory[10];

				if( lpItem->m_bPeriodItem )
				{
					return;
				}

				int m_Durability = (int)lpItem->m_Durability;

				lpItem->m_Durability -= (float)0.02;

				if(m_Durability > (int)lpItem->m_Durability)
				{
					if( lpItem->m_Durability < 1)
					{
						lpItem->m_Durability = 0;
					}

					BYTE dur = (BYTE)lpItem->m_Durability;

					GCItemDurSend(lpObj->m_Index,10,dur,0);

					if(dur == 0)
					{
						gObjUseSkill.SkillChangeUse(lpObj->m_Index);
						LogAddTD(lMsg.Get(544),lpObj->AccountID,lpObj->Name,lpItem->GetName(),lpItem->m_Level,dur);
						lpItem->Clear();
						GCInventoryItemDeleteSend(lpObj->m_Index,10,0);
					}
				}
			}
			return;
		}
	}


	if( lpObj->pInventory[11].IsItem() )
	{
		switch( lpObj->pInventory[11].m_Type )
		{
		case ITEMGET(13,10):
		case ITEMGET(13,39):
		case ITEMGET(13,40):
		case ITEMGET(13,41):
		case ITEMGET(13,68):
		case ITEMGET(13,76):
		case ITEMGET(13,77):
		case ITEMGET(13,78):
		case ITEMGET(13,122):
			{
				CItem* lpItem = &lpObj->pInventory[11];

				if( lpItem->m_bPeriodItem )
				{
					return;
				}

				int m_Durability = (int)lpItem->m_Durability;

				lpItem->m_Durability -= (float)0.02;

				if(m_Durability > (int)lpItem->m_Durability)
				{
					if( lpItem->m_Durability < 1)
					{
						lpItem->m_Durability = 0;
					}

					BYTE dur = (BYTE)lpItem->m_Durability;

					GCItemDurSend(lpObj->m_Index,11,dur,0);

					if(dur == 0)
					{
						gObjUseSkill.SkillChangeUse(lpObj->m_Index);
						LogAddTD(lMsg.Get(544),lpObj->AccountID,lpObj->Name,lpItem->GetName(),lpItem->m_Level,dur);
						lpItem->Clear();
						GCInventoryItemDeleteSend(lpObj->m_Index,11,0);
					}
				}
			}
			return;
		}
	}
}




#pragma warning ( disable : 4101 )

void gObjWingDurProc(LPOBJ lpObj) 
{
	BYTE send_dur=0; // HERE GOES A MACRO
	{
		int dur;
	}
}

#pragma warning ( default : 4101 )




void gObjPenaltyDurDown(LPOBJ lpObj, LPOBJ lpTargetObj)	// Functio n Not USed
{
	int ret = 0;
	int reCalCharacter = 0;

	int irand = rand()%2;

	CItem * Weapon = &lpObj->pInventory[irand];
	int decdur = 1;

	switch(lpObj->Class)
	{
	case 0:
		decdur = lpObj->Level - (lpTargetObj->Level * 38) / 20;
		break;
	case 1:
		decdur = lpObj->Level - (lpTargetObj->Level * 37) / 20;
		break;
	case 2:
		decdur = lpObj->Level - (lpTargetObj->Level * 36) / 20;
		break;
	case 3:
	case 4:
		decdur = lpObj->Level - (lpTargetObj->Level * 37) / 20;
		break;
	default : break;
	}

	decdur /= 10;

	if(Weapon->m_Type != ITEMGET(4,7) && Weapon->m_Type != ITEMGET(4,15))
	{
		if(Weapon->m_Type < ITEMGET(6,0) && Weapon->m_Type >= ITEMGET(7,0) )
		{
			if ( Weapon->IsItem() )
			{
				ret = Weapon->DurabilityDown2(decdur,lpObj->m_Index);
			}
		}
		else
		{
			CItem * Weapon = &lpObj->pInventory[irand];

			if(Weapon->IsItem())
			{
				ret = Weapon->DurabilityDown2(decdur,lpObj->m_Index);
			}
		}
	}

	if(ret != 0)
	{
		GCItemDurSend2(lpObj->m_Index,irand,(BYTE)lpObj->pInventory[irand].m_Durability,0);
	}

	if(ret == 2)
	{
		reCalCharacter = 1;
	}

	int item_num[7] = {0,1,2,3,4,5,6};
	int item_pos = rand()%7;

	CItem * DurItem = &lpObj->pInventory[item_num[item_pos]];

	if(DurItem->m_Type != ITEMGET(4,7) && DurItem->m_Type != ITEMGET(4,15))
	{
		if(item_pos < 2)
		{
			if(DurItem->m_Type >= ITEMGET(6,0) && DurItem->m_Type < ITEMGET(7,0))
			{
				ret = DurItem->DurabilityDown2(decdur,lpObj->m_Index);
	
				if(ret != 0)
				{
					GCItemDurSend2(lpObj->m_Index,item_num[item_pos],(BYTE)DurItem->m_Durability,0);
	
					if(ret == 2)
					{
						reCalCharacter = 1;
					}
				}
			}
		}
		else
		{
			ret = DurItem->DurabilityDown2(decdur,lpObj->m_Index);

			if(ret != 0)
			{
				GCItemDurSend2(lpObj->m_Index,item_num[item_pos],(BYTE)DurItem->m_Durability,0);

				if(ret == 2)
				{
					reCalCharacter = 1;
				}
			}
		}
	}

	CItem * Wing = &lpObj->pInventory[7];

	if(Wing->IsItem())
	{
		ret = DurItem->DurabilityDown2(decdur,lpObj->m_Index);

		if(ret != 0)
		{
			GCItemDurSend2(lpObj->m_Index,item_num[item_pos],(BYTE)DurItem->m_Durability,0);

			if(ret == 2)
			{
				reCalCharacter = 1;
			}
		}
	}

	if(reCalCharacter != 0)
	{
		gObjCalCharacter(lpObj->m_Index);
	}
}






void gObjWeaponDurDown(LPOBJ lpObj, LPOBJ lpTargetObj, int type)
{
	if(lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
	{
		return;
	}

	int itargetdefence = 0;
	CItem * Right = &lpObj->pInventory[0];
	CItem * Left = &lpObj->pInventory[1];
	int ret = 0;

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_TALISMAN_ITEMPROTECT) )
	{
		return;
	}

	if(type == 0)
	{

		itargetdefence = lpTargetObj->m_Defense;

		if((lpObj->Class == 1 || lpObj->Class == 3 || lpObj->Class == 4)
			&& (Right->m_Type >= 0 && Right->m_Type < ITEMGET(4,0) 
				&& Left->m_Type >= 0 && Left->m_Type < ITEMGET(4,0)))
		{
			if(rand()%2==0)
			{
				ret = Left->NormalWeaponDurabilityDown(itargetdefence,lpObj->m_Index);
				
				if(ret != 0)
				{
					GCItemDurSend(lpObj->m_Index,1,(BYTE)Left->m_Durability,0);
					if(ret == 2)
					{
						gObjCalCharacter(lpObj->m_Index);
					}
				}
			}
			else
			{
				ret = Right->NormalWeaponDurabilityDown(itargetdefence,lpObj->m_Index);
				if(ret != 0)
				{
					GCItemDurSend(lpObj->m_Index,0,(BYTE)Right->m_Durability,0);
					if(ret==2)
					{
						gObjCalCharacter(lpObj->m_Index);
					}
				}
			}

			return;
		}
		else if(Left->m_Type >= ITEMGET(4,0) && Left->m_Type < ITEMGET(4,7) || Left->m_Type == ITEMGET(4,20) || Left->m_Type == ITEMGET(4,21))
		{
			ret = Left->BowWeaponDurabilityDown(itargetdefence,lpObj->m_Index);

			if(ret != 0)
			{
				GCItemDurSend(lpObj->m_Index,1,(BYTE)Left->m_Durability,0);
				if(ret==2)
				{
					gObjCalCharacter(lpObj->m_Index);
				}
			}
		}
		else if(Right->m_Type >= ITEMGET(4,8) && Right->m_Type < ITEMGET(4,15) || Right->m_Type >= ITEMGET(4,16) && Right->m_Type < ITEMGET(5,0))
		{
			ret = Right->BowWeaponDurabilityDown(itargetdefence,lpObj->m_Index);

			if(ret != 0)
			{
				GCItemDurSend(lpObj->m_Index,0,(BYTE)Right->m_Durability,0);
				if(ret==2)
				{
					gObjCalCharacter(lpObj->m_Index);
				}
			}
		}
		else if(Right->m_Type >= ITEMGET(0,0) && Right->m_Type < ITEMGET(4,0))
		{
			ret = Right->NormalWeaponDurabilityDown(itargetdefence,lpObj->m_Index);

			if(ret != 0)
			{
				GCItemDurSend(lpObj->m_Index,0,(BYTE)Right->m_Durability,0);
				if(ret==2)
				{
					gObjCalCharacter(lpObj->m_Index);
				}
			}
		}
		else if(Right->m_Type >= ITEMGET(5,0) && Right->m_Type < ITEMGET(6,0))
		{
			ret = Right->StaffWeaponDurabilityDown(itargetdefence,lpObj->m_Index);

			if(ret != 0)
			{
				GCItemDurSend(lpObj->m_Index,0,(BYTE)Right->m_Durability,0);
				if(ret==2)
				{
					gObjCalCharacter(lpObj->m_Index);
				}
			}
		}
	return;
	}


	if(lpTargetObj->Type == OBJ_MONSTER || lpTargetObj->Type == OBJ_NPC)
	{
		itargetdefence = lpTargetObj->m_MagicDefense;
	}
	else
	{
		itargetdefence = lpTargetObj->m_Defense;
	}

	if( lpObj->Class == CLASS_SUMMONER )
	{
		if( Right->m_Type >= ITEMGET(0,0) && Right->m_Type >= ITEMGET(5,0) && Right->m_Type < ITEMGET(6,0) &&
			Left->m_Type >= ITEMGET(0,0) && Left->m_Type >= ITEMGET(5,0) && Left->m_Type < ITEMGET(6,0) )
		{
			if( rand()%2 == FALSE )
			{
				ret = Right->StaffWeaponDurabilityDown(itargetdefence,lpObj->m_Index);

				if(ret != 0)
				{
					GCItemDurSend(lpObj->m_Index,0,(BYTE)Right->m_Durability,0);
					if(ret==2)
					{
						gObjCalCharacter(lpObj->m_Index);
					}
				}
			}
			else
			{
				ret = Left->StaffWeaponDurabilityDown(itargetdefence,lpObj->m_Index);

				if(ret != 0)
				{
					GCItemDurSend(lpObj->m_Index,1,(BYTE)Left->m_Durability,0);
					if(ret==2)
					{
						gObjCalCharacter(lpObj->m_Index);
					}
				}
			}
		}
		else if( Right->m_Type >= ITEMGET(0,0) &&
				Right->m_Type >= ITEMGET(5,0) && Right->m_Type < ITEMGET(6,0) )
		{
			GCItemDurSend(lpObj->m_Index,1,(BYTE)Left->m_Durability,0);
			
			if(ret==2)
			{
				gObjCalCharacter(lpObj->m_Index);
			}
			return;
		}
		else if( Left->m_Type >= ITEMGET(0,0) &&
				Left->m_Type >= ITEMGET(5,0) && Left->m_Type < ITEMGET(6,0) )
		{
			ret = Left->StaffWeaponDurabilityDown(itargetdefence,lpObj->m_Index);

			if(ret != 0)
			{
				GCItemDurSend(lpObj->m_Index,1,(BYTE)Left->m_Durability,0);
				if(ret==2)
				{
					gObjCalCharacter(lpObj->m_Index);
				}
			}
		}
	}
	else if(Right->m_Type >= ITEMGET(5,0) && Right->m_Type < ITEMGET(6,0))
	{
		ret = Right->StaffWeaponDurabilityDown(itargetdefence,lpObj->m_Index);
		if(ret != 0)
		{
			GCItemDurSend(lpObj->m_Index,0,(BYTE)Right->m_Durability,0);
			if(ret==2)
			{
				gObjCalCharacter(lpObj->m_Index);
			}
		}
	}
}






void gObjArmorRandomDurDown(LPOBJ lpObj, LPOBJ lpAttackObj)
{
	int item_num[7] = {0,1,2,3,4,5,6};
	int item_pos = rand()%7;

	if(lpObj->Type != OBJ_USER)
	{
		return;
	}

	CItem * DurItem = &lpObj->pInventory[item_num[item_pos]];
	int damagemin = lpAttackObj->m_AttackDamageMin;
	int ret;
	short type = DurItem->m_Type;

	if( gObjCheckUsedBuffEffect(lpObj,BUFF_TALISMAN_ITEMPROTECT) )
	{
		if( type >= 0 && type < ITEMGET(12,0) )
		{
			return;
		}

		if( type >= ITEMGET(12,00) && type < ITEMGET(13,0) )
		{
			if( (type%512) >= 0 && (type%512) <= 16 ||
				(type%512) >= 36 && (type%512) <= 43 )
			{
				return;
			}
		}
	}

	if( DurItem->m_bLuckySet == TRUE )
		return;

	if(item_pos < 2)
	{
		if(DurItem->m_Type >= ITEMGET(6,0) && DurItem->m_Type < ITEMGET(7,0))
		{
			ret = DurItem->ArmorDurabilityDown(damagemin,lpObj->m_Index);

			if(ret != 0)
			{
				GCItemDurSend(lpObj->m_Index,item_num[item_pos],(BYTE)DurItem->m_Durability,0);
				if(ret == 2)
				{
					gObjCalCharacter(lpObj->m_Index);
				}
			}
			return;
		}
	}
	else
	{
		if(DurItem->m_Type != ITEMGET(4,7) && DurItem->m_Type != ITEMGET(4,15))
		{
			ret = DurItem->ArmorDurabilityDown(damagemin,lpObj->m_Index);

			if(ret != 0)
			{
				GCItemDurSend(lpObj->m_Index,item_num[item_pos],(BYTE)DurItem->m_Durability,0);
				if(ret == 2)
				{
					gObjCalCharacter(lpObj->m_Index);
				}
			}
		}
	}
}






bool gObjIsSelfDefense(LPOBJ lpObj, int aTargetIndex)
{
	if ( !OBJMAX_RANGE(aTargetIndex) )
	{
		return false;
	}

	for ( int n=0;n<MAX_SELF_DEFENSE;n++)
	{
		if ( lpObj->SelfDefense[n] >= 0 )
		{
			if ( lpObj->SelfDefense[n] == aTargetIndex )
			{
				return true;
			}
		}
	}

	return false;
}






void gObjCheckSelfDefense(LPOBJ lpObj, int aTargetIndex)
{
	if(!OBJMAX_RANGE(aTargetIndex))
	{
		return;
	}

	if(gObj[aTargetIndex].m_PK_Level > 4)
	{
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if( g_NewPVP.IsDuel(*lpObj, aTargetIndex) )
	{
		return;
	}
#endif

	int n;
	int PartyNum = lpObj->PartyNumber;
	int empty = -1;

	for( n = 0; n < MAX_SELF_DEFENSE;n++)
	{
		if(gObj[aTargetIndex].SelfDefense[n] >= 0)
		{
			if(gObj[aTargetIndex].SelfDefense[n]==lpObj->m_Index)
			{
				return;
			}
			else if( PartyNum >= 0 )
			{
				if( gObj[gObj[aTargetIndex].SelfDefense[n]].PartyNumber == PartyNum )
				{
					return;
				}
			}
		}
	}

	int blank = -1;

	for(n = 0; n < MAX_SELF_DEFENSE;n++)
	{
		if(lpObj->SelfDefense[n] >= 0)
		{
			if(lpObj->SelfDefense[n] == aTargetIndex)
			{
				lpObj->SelfDefenseTime[n] = GetTickCount()+60000;
				return;
			}
		}
		else
		{
			blank = n;
		}
	}

	if(blank < 0)
	{
		LogAdd(lMsg.Get(523));
		return;
	}

	lpObj->MySelfDefenseTime = GetTickCount();
	lpObj->SelfDefense[blank] = aTargetIndex;
	lpObj->SelfDefenseTime[blank] = GetTickCount() + 60000;

	char szTemp[64];

	wsprintf(szTemp,lMsg.Get(1114),lpObj->Name,gObj[aTargetIndex].Name);

	PartyNum = gObj[aTargetIndex].PartyNumber;

	if( PartyNum >= 0 )
	{
		int number = -1;
		for( n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[PartyNum].Number[n];

			if( number >= 0 )
			{
				GCServerMsgStringSend(szTemp,number,1);
			}
		}
	}
	else
	{
		GCServerMsgStringSend(szTemp,aTargetIndex,1);
	}

	GCServerMsgStringSend(szTemp,lpObj->m_Index,1);

	LogAddTD("[%s][%s] Set SelfDefence [%s][%s]",lpObj->AccountID,lpObj->Name,gObj[aTargetIndex].AccountID,gObj[aTargetIndex].Name);
}






void gObjTimeCheckSelfDefense(LPOBJ lpObj)
{
	char szTemp[64];

	for ( int n=0;n<MAX_SELF_DEFENSE;n++)
	{
		if ( lpObj->SelfDefense[n] >= 0 )
		{
			if ( GetTickCount() > lpObj->SelfDefenseTime[n] )
			{
				wsprintf(szTemp, lMsg.Get(MSGGET(4, 91)), lpObj->Name);
				GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);
				GCServerMsgStringSend(szTemp, lpObj->SelfDefense[n], 1);
				lpObj->SelfDefense[n] = -1;
				
				LogAddTD("[%s][%s] ReSet SelfDefence [%s][%s]",
					lpObj->AccountID, lpObj->Name,
					gObj[lpObj->SelfDefense[n]].AccountID, gObj[lpObj->SelfDefense[n]].Name);
			}
		}
	}
}



#include "SoulSystem.h"


void gObjAttack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf * lpMagic, BOOL magicsend, BYTE MSBFlag, int AttackDamage, BOOL bCombo, BYTE byBarrageCount, BYTE byReflect)
{
	//if(g_DuelManager.CheckIfHasWinner(lpObj,lpTargetObj) == TRUE)
	//{
	//	return;
	//}
	
#if (ENABLE_CUSTOM_MONSTERSPAWNER == 1)
	if (MonsterSpawnerMng::getInstance()->isEventMonster(lpTargetObj->m_Index)) {
		if (MonsterSpawnerMng::getInstance()->isPrivate(lpObj->m_Index, lpTargetObj->m_Index)) {
			return;
		}
	}
#endif

	gclassObjAttack.Attack(lpObj, lpTargetObj, lpMagic, magicsend, MSBFlag, AttackDamage, bCombo, byBarrageCount, byReflect);
	g_ElementalSystem.Attack(lpObj, lpTargetObj, lpMagic);
	//return gclassObjAttack.Attack(lpObj, lpTargetObj, lpMagic, magicsend, MSBFlag, AttackDamage, bCombo);
}


void gObjMonsterExpDivision(LPOBJ lpMonObj, LPOBJ lpObj, int AttackDamage, int MSBFlag)
{
	gObjMonsterHitDamageUserDel(lpMonObj);
	lpMonObj->Money = 0;


	int tObjNum;
	__int64 exp;
	LPOBJ lpTargetObj;

	for(int n = 0; n < MAX_ST_HIT_DAMAGE;n++)
	{
		if(lpMonObj->sHD[n].number >= 0)
		{
			tObjNum = lpMonObj->sHD[n].number;
			lpTargetObj = &gObj[tObjNum];

			if(lpMonObj->sHD[n].HitDamage > 0)
			{
				bool bSendExp = 1;

				exp = gObjMonsterExpSingle(lpTargetObj,lpMonObj,lpMonObj->sHD[n].HitDamage,(int)lpMonObj->MaxLife,bSendExp);

				if(bSendExp == 1)
				{
					if(lpTargetObj->m_Index == lpObj->m_Index)
					{
						GCKillPlayerExpSend(lpTargetObj->m_Index,lpMonObj->m_Index,(int)exp,AttackDamage,MSBFlag);
					}
					else
					{
						GCKillPlayerExpSend(lpTargetObj->m_Index,lpMonObj->m_Index,(int)exp,0,MSBFlag);
					}
				}
			}
		}
	}
}


int gObjMonsterExpSingle(LPOBJ lpObj, LPOBJ lpTargetObj, int dmg, int tot_dmg, bool& bSendExp)
{
	if( g_MasterExp.CheckMonsterLvl(lpObj,lpTargetObj) == FALSE )
	{
		bSendExp = FALSE;
		return FALSE;
	}

	if(DS_MAP_RANGE(lpObj->MapNumber))
	{
		return g_DevilSquare.gObjMonsterExpSingle(lpObj,lpTargetObj,dmg,tot_dmg);
	}

	__int64 exp;
	__int64 maxexp = 0;

	int level = (lpTargetObj->Level+25)*lpTargetObj->Level/3;

	if((lpTargetObj->Level+10) < lpObj->Level + lpObj->MasterLevel )
	{
		level = level*(lpTargetObj->Level+10)/(lpObj->Level+lpObj->MasterLevel);
	}

	if(lpTargetObj->Level >= 65)
	{
		level = level + (lpTargetObj->Level-64)*(lpTargetObj->Level/4);
	}

	if(level > 0)
	{
		maxexp = level/2;
	}
	else
	{
		level = 0;
	}

	if(maxexp < 1)
	{
		exp = level;
	}
	else
	{
		exp = level + rand()%maxexp;
	}

	exp = dmg * exp / tot_dmg;

	if(BC_MAP_RANGE(lpObj->MapNumber))
	{
		exp = exp * 50 / 100;
	}

	DWORD mymaxexp = 0;

	if( g_MasterExp.IsEnabled(lpObj) )
	{
		mymaxexp = (DWORD)lpObj->MasterNextExp;
	}
	else
	{
		mymaxexp = gLevelExperience[lpObj->Level];
	}

	lpTargetObj->Money += (int)exp;

	if( g_MasterExp.IsEnabled(lpObj) == FALSE )
	{
		exp += (exp * m_ObjBill[lpObj->m_Index].GetExp()) / 100;
		exp = int(exp * g_MapRateInfo.GetExp(lpObj->MapNumber));
	}

	if ( g_CrywolfSync.GetOccupationState() == 1 && g_iCrywolfApplyMvpPenalty != FALSE)
	{
		exp =  (exp * g_CrywolfSync.GetGettingExpPenaltyRate()) / 100;
	}

	if(exp > 0)
	{
		if(lpObj->Type == OBJ_USER)
		{
			CheckItemOptForGetExpEx(lpObj,exp,0);

			lpObj->Experience += (DWORD)exp;


			if(gObjLevelUp(lpObj,&exp,lpTargetObj->Class,0) == 0)
			{
				bSendExp = 0;
			}
		}
	}
	return (int)exp;
}



void gObjExpParty(LPOBJ lpObj , LPOBJ lpTargetObj, int AttackDamage, int MSBFlag )
{
	if(DS_MAP_RANGE(lpObj->MapNumber))
	{
		g_DevilSquare.gObjExpParty(lpObj,lpTargetObj,AttackDamage,MSBFlag);
		return;
	}

	int n;
	__int64 exp;
	__int64 maxexp = 0;
	__int64 totalexp;
	int level = (lpTargetObj->Level+25)*lpTargetObj->Level / 3;
	int number;
	int partynum = 0;
	int totallevel = 0;
	int partylevel;
	int partycount;
	int dis[6];
	int viewplayer = 0;
	int viewpercent = 100;
	LPOBJ lpPartyObj;

	partynum = lpObj->PartyNumber;

	int toplevel = 0;
	int distance;

	for(n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[partynum].Number[n];

		if(number >= 0)
		{
			lpPartyObj = &gObj[number];

			distance = gObjCalDistance(lpObj,lpPartyObj);

			if( distance < 10 )
			{
				if(lpPartyObj->Level > toplevel)
				{
					toplevel = lpPartyObj->Level;
				}
			}

			if( distance < 10 )
			{
				if(lpPartyObj->Level + lpPartyObj->MasterLevel > toplevel)
				{
					toplevel = lpPartyObj->Level + lpPartyObj->MasterLevel;
				}
			}
		}
	}

	if(!OBJMAX_RANGE(partynum))
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	partycount = gParty.m_PartyS[partynum].Count;

	for(n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[partynum].Number[n];

		if(number >= 0)
		{
			lpPartyObj = &gObj[number];

			if(lpTargetObj->MapNumber == lpPartyObj->MapNumber)
			{
				dis[n] = gObjCalDistance(lpTargetObj,&gObj[number]);

				if(dis[n] < 10)
				{
					lpPartyObj = &gObj[number];

					if(toplevel >= (lpPartyObj->Level+200))
					{
						totallevel += lpPartyObj->Level+200;
					}
					else
					{
						totallevel += lpPartyObj->Level;
					}
					viewplayer += 1;
				}
			}
		}
	}

	viewpercent += gParty.GetExpBonus(lpObj, lpTargetObj, partynum);

	if(viewplayer > 1)
	{
		partylevel = totallevel / viewplayer;
	}
	else
	{
		partylevel = totallevel;
	}

	if((lpTargetObj->Level+10) < partylevel)
	{
		level = level * (lpTargetObj->Level+10) / partylevel;
	}

	if(lpTargetObj->Level >= 65)
	{
		level = level + (lpTargetObj->Level-64)*(lpTargetObj->Level/4);
	}

	if(level > 0)
	{
		maxexp = level / 2;
	}
	else
	{
		level = 0;
	}

	if(maxexp < 1)
	{
		totalexp = level;
	}
	else
	{
		totalexp = level + rand()%maxexp;
	}

	if(lpTargetObj->Type == OBJ_MONSTER)
	{
		lpTargetObj->Money = (int)totalexp;
	}

	for(n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[partynum].Number[n];

		if(number >= 0)
		{
			lpPartyObj = &gObj[number];

			if(lpTargetObj->MapNumber == lpPartyObj->MapNumber && dis[n] < 10)
			{
				if( g_MasterExp.CheckMonsterLvl(lpPartyObj,lpTargetObj) == FALSE )
				{
					continue;
				}

				DWORD mymaxexp = gLevelExperience[lpPartyObj->Level];

				exp = (totalexp * viewpercent * lpPartyObj->Level)/totallevel / 100;

				if(BC_MAP_RANGE(lpPartyObj->MapNumber))
				{
					exp = exp * 50 / 100;
				}

				if(exp > mymaxexp)
				{
					exp = mymaxexp;
				}

				if(lpPartyObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER)
				{
					exp = 0;
				}

				if( g_MasterExp.IsEnabled(lpPartyObj) == FALSE )
				{
					exp += (exp * m_ObjBill[lpPartyObj->m_Index].GetExp()) / 100;
					exp = int(exp * g_MapRateInfo.GetExp(lpPartyObj->MapNumber));
				}

				if ( g_CrywolfSync.GetOccupationState() == 1 && g_iCrywolfApplyMvpPenalty != FALSE)
				{
					exp =  (exp * g_CrywolfSync.GetGettingExpPenaltyRate()) / 100;
				}

				if(exp > 0)
				{
					if(lpPartyObj->Type == OBJ_USER)
					{
						CheckItemOptForGetExpEx(lpPartyObj,exp,0);
						lpPartyObj->Experience += (DWORD)exp;


						if(gObjLevelUp(lpPartyObj,&exp,lpTargetObj->Class,1) == 0)
						{
							continue;
						}
					}
				}

				if(lpPartyObj->Type == OBJ_USER && g_MasterExp.IsEnabled(lpPartyObj) == FALSE )
				{
					GCKillPlayerExpSend(lpPartyObj->m_Index,lpTargetObj->m_Index,(int)exp,AttackDamage,MSBFlag);
				}
			}
		}
	}
}






void gObjMonsterDieLifePlus(LPOBJ lpObj, LPOBJ lpTartObj)
{
	if( lpTartObj->Level < 0 || lpTartObj->Level > 255 )
	{
		return;
	}

	lpObj->Life += lpTartObj->Level;

	if( lpObj->MonsterDieGetLife != 0 )
	{
		lpObj->Life = lpObj->Life + ((lpObj->MaxLife + lpObj->AddLife)/lpObj->MonsterDieGetLife);
	}

	if( lpObj->MonsterDieGetMana != 0 )
	{
		lpObj->Mana = lpObj->Mana + ((lpObj->MaxMana + lpObj->AddMana)/lpObj->MonsterDieGetMana);
	}

	if( lpObj->m_wSocketMonsterDieLife > 0 )
	{
		lpObj->Life += lpObj->m_wSocketMonsterDieLife;
	}

	if( lpObj->m_wSocketMonsterDieMana > 0 )
	{
		lpObj->Mana += lpObj->m_wSocketMonsterDieMana;
	}

	if( lpObj->m_MPSkillOpt.MpsMonsterDieGetMana > 0.0f )
	{
		lpObj->Mana = lpObj->Mana + ((lpObj->MaxMana + lpObj->AddMana)/lpObj->m_MPSkillOpt.MpsMonsterDieGetMana);
	}

	if( lpObj->m_MPSkillOpt.MpsMonsterDieGetLife > 0.0f )
	{
		lpObj->Life = lpObj->Life + ((lpObj->MaxLife + lpObj->AddLife)/lpObj->m_MPSkillOpt.MpsMonsterDieGetLife);
	}

	if( lpObj->m_MPSkillOpt.MpsMonsterDieGetSD > 0.0f )
	{
		lpObj->iShield = lpObj->iShield + ((lpObj->iMaxShield + lpObj->iAddShield)/lpObj->m_MPSkillOpt.MpsMonsterDieGetSD);
	}

	if( lpObj->m_MPSkillOpt.MpsMonsterDieGetSD > 0.0f )
	{
		if( lpObj->iShield > (lpObj->iMaxShield+lpObj->iAddShield) )
			lpObj->iShield = lpObj->iMaxShield+lpObj->iAddShield;
	}

	if(lpObj->Life > (lpObj->MaxLife + lpObj->AddLife))
	{
		lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
	}

	GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,255,0,lpObj->iShield);

	if( lpObj->MonsterDieGetMana != 0 || lpObj->m_MPSkillOpt.MpsMonsterDieGetMana > 0.0f ||lpObj->m_wSocketMonsterDieMana > 0 )
	{
		if(lpObj->Mana > (lpObj->MaxMana + lpObj->AddMana))
		{
			lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
		}

		GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,255,0,lpObj->BP);
	}
}






void gObjLifeCheck(LPOBJ lpTargetObj, LPOBJ lpObj, int AttackDamage, int DamageSendType, int MSBFlag, int MSBDamage,WORD Skill, int iShieldDamage)
{
	if(lpObj->Connected < PLAYER_PLAYING)
	{
		return;
	}

	if(ATTRIBUTE_RANGE(lpObj->m_Attribute) || ATTRIBUTE_RANGE(lpTargetObj->m_Attribute))
	{
		return;
	}

	LPOBJ lpCallObj = lpObj;

	if(lpObj->Type == OBJ_MONSTER && lpObj->m_RecallMon >= 0)
	{
		lpCallObj = &gObj[lpObj->m_RecallMon];
	}

	if(AttackDamage != 0 && lpTargetObj->Type == OBJ_MONSTER)
	{
		if(lpTargetObj->m_RecallMon >= 0)
		{
			GCRecallMonLife(lpTargetObj->m_RecallMon,(int)lpTargetObj->MaxLife,(int)lpTargetObj->Life);
		}

		gObjMonsterSetHitDamage(lpTargetObj, lpCallObj->m_Index, AttackDamage);
	}

	if(lpTargetObj->Life <= 0 && lpTargetObj->Live != 0)
	{
		
/*#if (ENABLETEST_ZTLUA == 1)
		if (lpTargetObj->Type == OBJ_MONSTER) {
			ZTLuaManager* LuaMng = new ZTLuaManager("ZTLuaTest.lua");
			ZTLuaUnit::Bind(LuaMng->getLua());
			ZTLuaItem::Bind(LuaMng->getLua());

			LuaMng->Call("TestMonsterDie", lpObj->m_Index, lpTargetObj->m_Index);
			std::string ChatOutput = LuaMng->getRef()->cast<std::string>();

			if (!ChatOutput.empty()) {
				ChatTargetSend(lpTargetObj, (char*)ChatOutput.c_str(), lpObj->m_Index);
			}
			else {
				ChatTargetSend(lpTargetObj, "Lua error!", lpObj->m_Index);
				LogAddC(2, "[DEBUG] [LUA] String not returned");
			}

			ZTLuaManager::Delete(LuaMng);
		}
#endif*/
	
		if(lpTargetObj->Class >= 605 && lpTargetObj->Class <= 607 )
		{
			lpTargetObj->m_bOreUsed = FALSE;
		}

		if(lpObj->Type == OBJ_MONSTER)
		{
			gObjAddMsgSend(lpObj,3,lpTargetObj->m_Index,0);
			CreateFrustrum(lpObj->X,lpObj->Y,lpObj->m_Index);
			lpTargetObj->KillerType = 1;
		}

		if(lpObj->Type == OBJ_USER)
		{
			gDarkSpirit[lpObj->m_Index].ReSetTarget(lpTargetObj->m_Index);
		}

		if(lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER)
		{
			gObjSetKillCount(lpObj->m_Index,1);

			if( IT_MAP_RANGE(lpObj->MapNumber) )
			{
				if( g_IllusionTemple.GetState(lpObj->MapNumber) == IT_STATE_PLAYING )
				{
					BYTE Point = g_IllusionTemple.PointAdd(lpObj->m_Index,lpObj->MapNumber,1);
					GCIllusionSendPoint(lpObj->m_Index,Point);
					LogAddTD("[Illusion Temple] (%d) (%s)(%s) Character Killed, (point: %d)",
						lpObj->MapNumber - (MAP_INDEX_ILLUSIONTEMPLE1-1),lpObj->AccountID,lpObj->Name,Point);
				}
			}
		}

		if(lpTargetObj->Type == OBJ_MONSTER)
		{
			if( IT_MAP_RANGE(lpObj->MapNumber) )
			{
				if( g_IllusionTemple.GetState(lpObj->MapNumber) == IT_STATE_PLAYING )
				{
					BYTE Point = g_IllusionTemple.PointAdd(lpObj->m_Index,lpObj->MapNumber,2);
					GCIllusionSendPoint(lpObj->m_Index,Point);
					lpTargetObj->m_PosNum = -1;
					LogAddTD("[Illusion Temple] (%d) (%s)(%s) Monster Killed, (point: %d)",
						lpObj->MapNumber - (MAP_INDEX_ILLUSIONTEMPLE1-1),lpObj->AccountID,lpObj->Name,Point);
				}
			}

#if (GS_CASTLE==1)
	
			if(lpTargetObj->Class == 275 || lpTargetObj->Class == 295 || lpTargetObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
			{
				gObjAddMsgSendDelayInSpecificQPos(lpTargetObj,1,lpObj->m_Index,800,0,0);
			}
			else
			{
				gObjAddMsgSendDelay(lpTargetObj,1,lpObj->m_Index,800,0);
			}

			g_Crywolf.CrywolfMonsterDieProc(lpTargetObj->m_Index,lpObj->m_Index);
#else
			if(lpTargetObj->Class == 275 || lpTargetObj->Class == 295 )
			{
				gObjAddMsgSendDelayInSpecificQPos(lpTargetObj,1,lpObj->m_Index,800,0,0);
			}
			else
			{
				gObjAddMsgSendDelay(lpTargetObj,1,lpObj->m_Index,800,0);
			}
#endif

#if (GS_CASTLE==1)
			if(lpTargetObj->m_Attribute == 62)
			{

				char szMsg[256];

				switch(lpTargetObj->Class)
				{
				case 300:// Hero Mobs
				case 301:
				case 302:
				case 303:
					{
						PMSG_NOTICE pNotice;

						ItemSerialCreateSend(lpObj->m_Index,lpObj->MapNumber,lpObj->X,lpObj->Y,ItemGetNumberMake(14,13),0,0,0,0,0,lpObj->m_Index,0,0);

						if(gObjIsConnected(lpObj->m_Index))
						{
							wsprintf(szMsg,lMsg.Get(MSGGET(12, 155)),lpTargetObj->Name,lpObj->Name);
							LogAddTD("[CastleDeep Event] [%s][%s] Boss Monster Killed : %s",lpObj->AccountID,lpObj->Name,lpTargetObj->Name);
						}
						else
						{
							wsprintf(szMsg,lMsg.Get(MSGGET(12, 156)),lpTargetObj->Name);
							LogAddTD("[CastleDeep Event] Boss Monster Killed : %s",lpTargetObj->Name);
						}

						TNotice::MakeNoticeMsg(&pNotice,0,szMsg);

						for(int i = OBJ_STARTUSERINDEX; i < OBJMAX;i++)
						{
							if(gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
							{
								DataSend(i,(PBYTE)&pNotice,pNotice.h.size);
							}
						}
					}
					break;
				case 295: // Erohim
					{
						PMSG_NOTICE pNotice;

						if(gObjIsConnected(lpObj->m_Index))
						{
							wsprintf(szMsg,lMsg.Get(MSGGET(12, 157)),lpObj->Name);
							LogAddTD("[CastleDeep Event] [%s][%s] Boss Monster Killed : %s",lpObj->AccountID,lpObj->Name,lpTargetObj->Name);
						}
						else
						{
							wsprintf(szMsg,lMsg.Get(MSGGET(12, 158)));
							LogAddTD("[CastleDeep Event] Boss Monster Killed : %s",lpTargetObj->Name);
						}

						TNotice::MakeNoticeMsg(&pNotice,0,szMsg);

						for(int i = OBJ_STARTUSERINDEX; i < OBJMAX;i++)
						{
							if(gObj[i].Connected == PLAYER_PLAYING && gObj[i].Type == OBJ_USER)
							{
								DataSend(i,(PBYTE)&pNotice,pNotice.h.size);
							}
						}
					}
					break;
				}
			}
#endif
#if (GS_CASTLE == 0 || GS_SPEC == 1)
			g_Kanturu.KanturuMonsterDieProc(lpTargetObj->m_Index,lpObj->m_Index);
#endif

			if(BC_MAP_RANGE(lpTargetObj->MapNumber))
			{
				int iBridge = g_BloodCastle.GetBridgeLevel(lpTargetObj->MapNumber);

				if(lpTargetObj->Class == 131)
				{
					int TopHitUser = gObjMonsterTopHitDamageUser(lpTargetObj);

					if(TopHitUser != -1)
					{
						char szMsg[256];

						wsprintf(szMsg,lMsg.Get(1178),gObj[TopHitUser].Name);

						g_BloodCastle.SendNoticeMessage(iBridge,szMsg);

						g_BloodCastle.m_BridgeData[iBridge].m_iExtraEXP_Kill_Door_Party = gObj[TopHitUser].PartyNumber;
						g_BloodCastle.m_BridgeData[iBridge].m_iExtraEXP_Kill_Door_Index = TopHitUser;

						memcpy(g_BloodCastle.m_BridgeData[iBridge].m_szKill_Door_CharName,gObj[TopHitUser].Name,10);
						memcpy(g_BloodCastle.m_BridgeData[iBridge].m_szKill_Door_AccountID,gObj[TopHitUser].AccountID,10);

						g_BloodCastle.m_BridgeData[iBridge].m_szKill_Door_CharName[10] = 0;
						g_BloodCastle.m_BridgeData[iBridge].m_szKill_Door_AccountID[10] = 0;

						LogAddTD("[Blood Castle] (%d) Door Terminated -> %d [%s][%s]",
							iBridge+1,g_BloodCastle.m_BridgeData[iBridge].m_iBC_BOSS_MONSTER_KILL_COUNT,gObj[TopHitUser].AccountID,gObj[TopHitUser].Name);
					}
					else
					{
						g_BloodCastle.SendNoticeMessage(iBridge,lMsg.Get(1169));
						LogAddTD("[Blood Castle] (%d) Door Terminated -> %d)",
							iBridge+1,g_BloodCastle.m_BridgeData[iBridge].m_iBC_BOSS_MONSTER_KILL_COUNT);
					}
				}


				if(BC_STATUE_RANGE(lpTargetObj->Class - 132))
				{
					int iBridge = g_BloodCastle.GetBridgeLevel(lpObj->MapNumber);
					int TopHitUser = gObjMonsterTopHitDamageUser(lpTargetObj);

					if(TopHitUser != -1)
					{
						char szMsg[256];

						wsprintf(szMsg,lMsg.Get(1179),gObj[TopHitUser].Name);

						g_BloodCastle.SendNoticeMessage(iBridge,szMsg);


						g_BloodCastle.m_BridgeData[iBridge].m_iExtraEXP_Kill_Statue_Party = gObj[TopHitUser].PartyNumber;
						g_BloodCastle.m_BridgeData[iBridge].m_iExtraEXP_Kill_Statue_Index = gObj[TopHitUser].m_Index;

						memcpy(g_BloodCastle.m_BridgeData[iBridge].m_szKill_Status_CharName,gObj[TopHitUser].Name,10);
						memcpy(g_BloodCastle.m_BridgeData[iBridge].m_szKill_Status_AccountID,gObj[TopHitUser].AccountID,10);

						g_BloodCastle.m_BridgeData[iBridge].m_szKill_Status_CharName[10] = 0;
						g_BloodCastle.m_BridgeData[iBridge].m_szKill_Status_AccountID[10] = 0;

						LogAddTD("[Blood Castle] (%d) a stone statue Of Saint Terminated -> %d [%s][%s]",
							iBridge+1,g_BloodCastle.m_BridgeData[iBridge].m_iBC_BOSS_MONSTER_KILL_COUNT,gObj[TopHitUser].AccountID,gObj[TopHitUser].Name);
					}
					else
					{
						g_BloodCastle.SendNoticeMessage(iBridge,lMsg.Get(1170));
						LogAddTD("[Blood Castle] (%d) a stone statue Of Saint Terminated -> %d",
							iBridge+1,g_BloodCastle.m_BridgeData[iBridge].m_iBC_BOSS_MONSTER_KILL_COUNT);
					}
				}

				for(int n = 0; n < MAX_BLOOD_CASTLE_SUB_BRIDGE; n++)
				{
					if(g_BloodCastle.m_BridgeData[iBridge].m_UserData[n].m_iIndex != -1)
					{
						if(gObj[g_BloodCastle.m_BridgeData[iBridge].m_UserData[n].m_iIndex].Connected > PLAYER_LOGGED)
						{
							g_BloodCastle.m_BridgeData[iBridge].m_UserData[n].m_bLiveWhenDoorBreak = 1;
						}
					}
				}
			}

			if(CC_MAP_RANGE(lpTargetObj->MapNumber))
			{
				if(rand()%100 < 50)
				{
					g_ChaosCastle.SearchNBlowObjs(lpTargetObj->MapNumber,lpTargetObj->X,lpTargetObj->Y);
				}

				g_ChaosCastle.DelMonsterList(g_ChaosCastle.GetFloor(lpTargetObj->MapNumber),lpTargetObj->m_Index);

				int TopHitUser = gObjMonsterTopHitDamageUser(lpTargetObj);

				if(TopHitUser != -1)
				{
					if(CC_MAP_RANGE(gObj[TopHitUser].MapNumber))
					{
						if(gObjIsConnected(TopHitUser))
						{
							gObj[TopHitUser].m_cKillMonsterCount += 1;
						}
					}
				}
			}

			gObjPkDownTimeCheck(lpObj,lpTargetObj->Level);

			if(lpObj->Type == OBJ_USER)
			{
				gObjAddMsgSendDelay(lpObj,3,lpTargetObj->m_Index,2000,0);
			}
		}
		else if(lpTargetObj->Type == OBJ_USER)
		{
			if(BC_MAP_RANGE(lpTargetObj->MapNumber))
			{
				g_BloodCastle.SetUserState(lpTargetObj->m_Index,1);
				g_BloodCastle.SearchUserDropQuestItem(lpTargetObj->m_Index);
			}

			if(IT_MAP_RANGE(lpTargetObj->MapNumber))
			{
				g_IllusionTemple.DropRelic(lpTargetObj->MapNumber,lpTargetObj->m_Index);
			}

			if(CC_MAP_RANGE(lpTargetObj->MapNumber))
			{
				g_ChaosCastle.SetUserState(lpTargetObj->m_Index,1);

				if(CC_MAP_RANGE(lpObj->MapNumber))
				{
					if(gObjIsConnected(lpObj->m_Index))
					{
						lpObj->m_cKillUserCount += 1;
					}
				}

				g_ChaosCastle.SendFailMessage(g_ChaosCastle.GetFloor(lpTargetObj->MapNumber),lpTargetObj->m_Index);

				LogAddTD("[Chaos Castle] (%d) [%s][%s] User Dead In Chaos Castle : Killed by [%s][%s]",
					g_ChaosCastle.GetFloor(lpTargetObj->MapNumber)+1,lpTargetObj->AccountID,lpTargetObj->Name,lpObj->AccountID,lpObj->Name);
			}

			lpTargetObj->KillerType = 0;

			gObjUserDie(lpTargetObj,lpCallObj);

			int iGuildWar = gObjGuildWarCheck(lpCallObj,lpTargetObj);
			int iDuel = gObjDuelCheck(lpCallObj,lpTargetObj);

			if(iGuildWar == 0 && iDuel == 0)
			{
				if(lpTargetObj->m_bAttackerKilled == 0)
				{
					gObjPlayerKiller(lpCallObj,lpTargetObj, MSBDamage);
				}
			}

			lpTargetObj->m_bAttackerKilled = 0;

			if(lpTargetObj->m_RecallMon >= 0)
			{
				gObjMonsterCallKill(lpTargetObj->m_Index);
			}
		}

		lpTargetObj->Live = 0;
		lpTargetObj->m_State = 4;
		lpTargetObj->RegenTime = GetTickCount();
		lpTargetObj->DieRegen = 1;
		lpTargetObj->PathCount = 0;
		if(gObjTargetGuildWarCheck(lpTargetObj,lpCallObj)==1)
		{
			lpTargetObj->KillerType = 2;
		}


		if (gObjDuelCheck(lpCallObj,lpTargetObj)) {
#if (ENABLETEST_NEWPVP == 1)
			lpTargetObj->KillerType = 3;
			g_NewPVP.CheckScore(*lpCallObj, *lpTargetObj);
#else
			//g_DuelManager.KillUserProc(lpObj,lpTargetObj);
#endif
		}


		if(lpTargetObj->Teleport == 1)
		{
			lpTargetObj->Teleport = 0;
		}

		GCDiePlayerSend(lpTargetObj,lpTargetObj->m_Index,Skill,lpObj->m_Index);

		if( lpTargetObj->Class >= 524 && lpTargetObj->Class <= 528 && lpTargetObj->Class != 526 )
			g_ImperialGuardian.DestroyGate(lpTargetObj->m_ImperialGuardianZone,lpTargetObj->m_Index,lpObj->m_Index);

		if( lpObj->Class == 523 )
		{
			LogAddTD("[IMPERIALGUARDIAN] Player Killed by Trap [AccountID]:%s, [NAME]:%s, [ZONE]:%d",
				lpTargetObj->AccountID,lpTargetObj->Name,lpObj->m_ImperialGuardianZone+1);
		}

		if( IF_MONSTER_RANGE(lpObj->Class) )
		{
			LogAddTD("[IMPERIALGUARDIAN] Player Dead [Zone]:%d [AccountID]:%s, [NAME]:%s by Monster => %s(%d)",
				lpObj->m_ImperialGuardianZone,lpTargetObj->AccountID,lpTargetObj->Name,lpObj->Name,lpObj->m_Index);
		}

		if( IF_MONSTER_RANGE(lpTargetObj->Class) )
		{
			LogAddTD("[IMPERIALGUARDIAN] Monster Dead [TYPE]: %d,[NAME]: %s, [ZONE]:%d",
				lpTargetObj->Class,lpTargetObj->Name,lpTargetObj->m_ImperialGuardianZone);
		}

		if( lpTargetObj->Class >= 504 && lpTargetObj->Class <= 511 || lpTargetObj->Class == 526 )
		{
			g_ImperialGuardian.DropItem(lpObj->m_Index,lpTargetObj->m_Index);
		}

		if( lpTargetObj->MapNumber == MAP_INDEX_RAKLIONBOSS && lpTargetObj->Type >= OBJ_MONSTER)
		{
			if( lpTargetObj->Class == 460 || lpTargetObj->Class == 461 || lpTargetObj->Class == 462 )
			{
				g_Raklion.BossEggDie();
				g_RaklionUtil.NotifyRaklionUserMonsterCount(g_Raklion.GetBossEggCount(), 0);
			}
		}

		if(lpTargetObj->Class == 79)
		{
			gEledoradoEvent.m_BossGoldDerconMapNumber[lpTargetObj->m_BossGoldDerconMapNumber] = -1;
			gEledoradoEvent.CheckGoldDercon(lpObj->MapNumber);
		}

		if(lpTargetObj->m_RecallMon >= 0)
		{
			return;
		}

		if(lpTargetObj->Class == 340 || lpTargetObj->Class == 348 || lpTargetObj->Class == 349)
		{
			return;
		}

		if(lpTargetObj->Class == 287 || lpTargetObj->Class == 286)
		{
			return;
		}

		if(lpTargetObj->Class == 275)
		{
			return;
		}
		
		if(lpTargetObj->Class == 288)
		{
			return;
		}

		if(lpTargetObj->Class == 295)
		{
			return;
		}

#if (GS_CASTLE == 0 || GS_SPEC == 1)
		if(lpTargetObj->Class == 362 || lpTargetObj->Class == 363 || lpTargetObj->Class == 361)
		{
			return;
		}
#endif
		if(lpTargetObj->Class == 412)
		{
			return;
		}

		if( lpTargetObj->Class == 504 || lpTargetObj->Class == 506 || lpTargetObj->Class == 507 ||
			lpTargetObj->Class == 508 || lpTargetObj->Class == 509 || lpTargetObj->Class == 510 ||
			lpTargetObj->Class == 511 )
		{
			return;
		}

		if(lpCallObj->Type == OBJ_USER)
		{
			if(lpCallObj->PartyNumber >= 0)
			{
				gObjExpParty(lpCallObj,lpTargetObj,AttackDamage,MSBFlag);
			}
			else if(lpTargetObj->Type == OBJ_MONSTER)
			{
				gObjMonsterExpDivision(lpTargetObj,lpCallObj,AttackDamage,MSBFlag);
			}

			if(lpTargetObj->Type == OBJ_MONSTER && DS_MAP_RANGE(lpTargetObj->MapNumber))
			{
				g_DevilSquare.gObjMonsterScoreDivision(lpTargetObj,lpCallObj,AttackDamage,MSBFlag);
			}
		}
		if(lpTargetObj->Type == OBJ_MONSTER)
		{
			if(lpTargetObj->m_bIsInMonsterHerd != 0 && lpTargetObj->m_lpMonsterHerd != 0)
			{
				lpTargetObj->m_lpMonsterHerd->MonsterDieAction(lpTargetObj);
			}
		}
		//return;
	}

	if(DamageSendType == 0)
	{
		GCDamageSend(lpCallObj->m_Index,lpTargetObj->m_Index,AttackDamage,MSBFlag,MSBDamage,iShieldDamage);
	}
	else if(DamageSendType == 1)
	{
		GCDamageSend(lpTargetObj->m_Index,AttackDamage,iShieldDamage);
	}
	else if(DamageSendType == 2)
	{
		GCDamageSend(lpCallObj->m_Index,lpTargetObj->m_Index,AttackDamage,MSBFlag,5,iShieldDamage);
	}
	else if(DamageSendType == 3)
	{
		GCDamageSend(lpCallObj->m_Index,lpTargetObj->m_Index,AttackDamage,MSBFlag,6,iShieldDamage);
	}
	else if(DamageSendType == 4)
	{
		GCDamageSend(lpCallObj->m_Index,lpTargetObj->m_Index,AttackDamage,MSBFlag);
	}

	GCLifeSendAll(lpTargetObj);
}


BOOL gObjInventoryTrans(int aIndex)
{
	int n;

	if ( ((aIndex< 0)?FALSE:(aIndex > OBJMAX-1)?FALSE:TRUE) == FALSE )
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	if ( gObj[aIndex].pTransaction == 1 )
	{
		LogAddTD("[%s][%s] error-L3 : pTransaction(%d) status error %s %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name,
			gObj[aIndex].pTransaction, __FILE__, __LINE__);
		return FALSE;
	}

	for ( n=0;n<MAX_MAGIC;n++)
	{
		gObj[aIndex].m_lpMagicBack[n] = gObj[aIndex].Magic[n]; 
	}

	for ( n=0;n<MAIN_INVENTORY_SIZE;n++)
	{
		gObj[aIndex].Inventory2[n] = gObj[aIndex].Inventory1[n];
	}

	memcpy(gObj[aIndex].InventoryMap2, gObj[aIndex].InventoryMap1, (INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE));
	gObj[aIndex].InventoryCount2 = gObj[aIndex].InventoryCount1;
	gObjSetInventory2Pointer(&gObj[aIndex]);
	gObj[aIndex].pTransaction = 1;
	
	LogAddL("Trade Transaction (%s)", gObj[aIndex].Name);

	return TRUE;
}

// CMagicInf::operator= 4c4d90






int gObjInventoryCommit(int aIndex)
{

	if(!OBJMAX_RANGE(aIndex))
	{
		LogAddTD("error : gObjInventoryCommit() - aIndex out of bound %s %d",__FILE__,__LINE__);
		return false;
	}

	if(gObj[aIndex].pTransaction != 1)
	{
		LogAddTD("[%s][%s] error-L3 : pTransaction(%d) status2",gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[aIndex].pTransaction);
		return false;
	}

	for(int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		gObj[aIndex].Inventory1[n] = gObj[aIndex].Inventory2[n];
	}

	memcpy(gObj[aIndex].InventoryMap1,gObj[aIndex].InventoryMap2,(INVENTORY_MAP_SIZE-PSHOP_MAP_SIZE));

	gObj[aIndex].InventoryCount1 = gObj[aIndex].InventoryCount2;

	gObjSetInventory1Pointer(&gObj[aIndex]);

	gObj[aIndex].pTransaction = 2;

	LogAddL("Trade Commit (%s)",gObj[aIndex].Name);

	return true;
}






int gObjInventoryRollback(int aIndex)
{
	int n;

	if(!OBJMAX_RANGE(aIndex))
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return false;
	}

	if(gObj[aIndex].pTransaction != 1)
	{
		LogAddTD("[%s][%s] error-L3 : pTransaction(%d) status2",gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[aIndex].pTransaction);
		return false;
	}

	for(n = 0; n < MAX_MAGIC; n++)
	{
		gObj[aIndex].Magic[n] = gObj[aIndex].m_lpMagicBack[n];
	}

	for(n = 0; n < INVENTORY_SIZE; n++)
	{
		gObj[aIndex].Inventory2[n].Clear();
	}

	gObjSetInventory1Pointer(&gObj[aIndex]);
	gObj[aIndex].pTransaction = 3;
	LogAddL("Trade Rollback (%s)",gObj[aIndex].Name);

	return true;
}


#ifdef __NOVUS__
#include "StatGem.h"
#endif

void gObjInventoryItemSet(int aIndex, int itempos, BYTE set_byte)
{
	int width;
	int height;
	if(itempos < INVETORY_WEAR_SIZE || itempos == ELEMENT_SOCKET)
	{
		return;
	}
	else if(itempos > (INVENTORY_SIZE-1))
	{
		return;
	}

	if(gObj[aIndex].pInventory[itempos].GetSize((int&)width,(int &)height)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return;
	}
	
#ifdef __NOVUS__
	if( g_StatGem.IsGem(gObj[aIndex].pInventory[itempos].m_Type) == true )
	{
		gObjCalCharacter(aIndex);
	}
#endif

	gObjInventoryItemBoxSet(aIndex,itempos,width,height,set_byte);
}



void gObjInventoryItemBoxSet(int aIndex, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos - INVETORY_WEAR_SIZE)%8;
	int itemposy = (itempos - INVETORY_WEAR_SIZE)/8;

	int xx,yy;
	
#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
	if( CheckOutOfInventory(aIndex, itemposy, yl) != 1 )	//OK
	{
		return;
	}
#endif

	for(int y = 0; y < yl; y ++)
	{
		yy = itemposy + y;

		for(int x = 0; x < xl; x++)
		{
			xx = itemposx + x;
#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
			if( InventoryExtentCheck(xx, yy, 8, 28) == 1 )
#else
			if(ExtentCheck(xx,yy,8,28)==1)
#endif
			{
				*(BYTE*)(gObj[aIndex].pInventoryMap + (itemposy + y)*8+(itemposx + x)) = set_byte;
			}
			else
			{
				LogAdd("error : %s %d",__FILE__,__LINE__);
				return;
			}
		}
	}
}


bool gObjFixInventoryPointer(int aIndex)
{

	if(gObjIsConnected(aIndex)==0)
	{
		LogAdd("[Fix Inv.Ptr] [%s][%s] - disconnected",gObj[aIndex].AccountID,gObj[aIndex].Name);
		return false;
	}


	if(gObj[aIndex].pInventory == gObj[aIndex].Inventory1)
	{
		return true;
	}

	if(gObj[aIndex].pInventory == gObj[aIndex].Inventory2)
	{
		if(gObj[aIndex].pTransaction == 1)
		{
			LogAdd("[Fix Inv.Ptr] [%s][%s] - Transaction == 1, Do not change Pointer",gObj[aIndex].AccountID,gObj[aIndex].Name);
			return false;
		}
		else
		{
			LogAdd("[Fix Inv.Ptr] [%s][%s] - Inventory Pointer was 2",gObj[aIndex].AccountID,gObj[aIndex].Name);

			for(int n = 0; n < INVENTORY_SIZE; n++)
			{
				gObj[aIndex].Inventory2[n].Clear();
			}
		}
	}
	else
	{
		LogAdd("[Fix Inv.Ptr] [%s][%s] - Inventory Pointer was Wrong",gObj[aIndex].AccountID,gObj[aIndex].Name);
	}

	gObjSetInventory1Pointer(&gObj[aIndex]);

	return false;
}






void gObjInventoryItemSet_PShop(int aIndex, int itempos, BYTE set_byte)
{
	int width;
	int height;
	if(itempos < INVETORY_WEAR_SIZE || itempos == ELEMENT_SOCKET)
	{
		return;
	}
	else if(itempos > (INVENTORY_SIZE-1))
	{
		return;
	}

	if(gObj[aIndex].Inventory1[itempos].GetSize((int&)width,(int &)height)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return;
	}

	gObjInventoryItemBoxSet_PShop(aIndex,itempos,width,height,set_byte);
}




void gObjInventoryItemBoxSet_PShop(int aIndex, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos - INVETORY_WEAR_SIZE)%8;
	int itemposy = (itempos - INVETORY_WEAR_SIZE)/8;

	int xx,yy;

#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
	if( CheckOutOfInventory(aIndex, itemposy, yl) != 1 )
	{
		return;
	}
#endif

	for(int y = 0; y < yl; y ++)
	{
		yy = itemposy + y;

		for(int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
			if( InventoryExtentCheck(xx, yy, 8, 28) == 1 )
#else
			if( ExtentCheck(xx,yy,8,28) == 1 )
#endif
			{
				*(BYTE*)(gObj[aIndex].InventoryMap1 + (itemposy + y)*8+(itemposx + x)) = set_byte;
			}
			else
			{
				LogAdd("error : %s %d",__FILE__,__LINE__);
				return;
			}
		}
	}
}






BYTE gObjInventoryDeleteItem(int aIndex ,int itempos)
{
	if(itempos < INVETORY_WEAR_SIZE || itempos == ELEMENT_SOCKET)
	{
		int MagicDel = gObjMagicDel(&gObj[aIndex],gObj[aIndex].pInventory[itempos].m_Special[0],gObj[aIndex].pInventory[itempos].m_Level);

		if(MagicDel >= 0)
		{
			GCMagicListOneDelSend(aIndex,MagicDel,
				gObj[aIndex].pInventory[itempos].m_Special[0],
				gObj[aIndex].pInventory[itempos].m_Level,0,0);
		}

		gObj[aIndex].pInventory[itempos].Clear();
	}
	else
	{
		gObjInventoryItemSet(aIndex,itempos,255);
		gObj[aIndex].pInventory[itempos].Clear();
	}

	return true;
}






BYTE gObjWarehouseDeleteItem(int aIndex, int itempos)
{
	gObjWarehouseItemSet(aIndex, itempos, -1);
	gObj[aIndex].pWarehouse[itempos].Clear();

	return true;
}

BYTE gObjChaosBoxDeleteItem(int aIndex, int itempos)
{
	gObjChaosItemSet(aIndex, itempos, -1);
	gObj[aIndex].pChaosBox[itempos].Clear();

	return true;
}


#if (ENABLETEST_MUUN == 1)
bool gObjFixMuunInventoryPointer(int aIndex) {
	if (!gObjIsConnected(aIndex)) {
		LogAdd("[Fix Muun Inv.Ptr] [%s][%s] - disconnected", 
			gObj[aIndex].AccountID, gObj[aIndex].Name);
		return false;
	}
	if (gObj[aIndex].pMuunInventory == gObj[aIndex].pMuunInventory1) {
		return true;
	}
	if (gObj[aIndex].pMuunInventory == gObj[aIndex].pMuunInventory2) {
		if (gObj[aIndex].pTransaction == 1) {
			LogAdd("[Fix Muun Inv.Ptr] [%s][%s] - Transaction == 1, Do not change Pointer", 
				gObj[aIndex].AccountID, gObj[aIndex].Name);
			return false;
		}
		LogAdd("[Fix Muun Inv.Ptr] [%s][%s] - Muun Inventory Pointer was 2", 
			gObj[aIndex].AccountID, gObj[aIndex].Name);
		for (int n = 0; n < MUUN_INVENTORY_SIZE; n++) {
			gObj[aIndex].pMuunInventory2[n].Clear();
		}
	} else {
		LogAdd("[Fix Muun Inv.Ptr] [%s][%s] - Muun Inventory Pointer was Wrong", 
			gObj[aIndex].AccountID, gObj[aIndex].Name);
	}
	gObjSetMuunInventory1Pointer(&gObj[aIndex]);
	return false;
}

bool gObjMuunInventoryDeleteItem(int aIndex, int itempos) {
	g_CMuunSystem.ClearPeriodMuunItemData(&gObj[aIndex], gObj[aIndex].pMuunInventory[itempos].m_Type,
		gObj[aIndex].pMuunInventory[itempos].m_Number);
	gObj[aIndex].pMuunInventory[itempos].Clear();
	return true;
}

// missing 1 call
BYTE gObjMuunInvenMove(OBJECTSTRUCT* lpObj, int& durSsend, int& durTsend, BYTE source, BYTE target, LPBYTE siteminfo) {
	if (source < 0 || source > MUUN_INVENTORY_SIZE - 1) {
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return -1;
	}
	if (target < 0 || target > MUUN_INVENTORY_SIZE - 1) {
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return -1;
	}
	if (source == target) {
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return -1;
	}
	if (target == 1 && !lpObj->bSubEquip) {
		LogAddTD("[MuunSystem] [%s][%s] Sub Equip Not Open Muun Item Try Move", lpObj->AccountID, lpObj->Name);
		return -1;
	}
	if (!lpObj->pMuunInventory[source].IsItem() || lpObj->pMuunInventory[target].IsItem()) {
		return -1;
	}
	memcpy(&lpObj->pMuunInventory[target], &lpObj->pMuunInventory[source], sizeof(lpObj->pMuunInventory[target]));
	lpObj->pMuunInventory[source].Clear();
	ItemByteConvert(siteminfo, lpObj->pMuunInventory[target]);
	if (target < 2 || source < 2) {
		ITEM_ATTRIBUTE* pItemAttribute = GetItemAttr(lpObj->pMuunInventory[target].m_Type);
		if (pItemAttribute == NULL) {
			return -1;
		}
		if (pItemAttribute->ItemKindA != 12) {
			return -1;
		}
		if (pItemAttribute->ItemKindA == 12 && !pItemAttribute->ItemKindB) {
			return -1;
		}
		g_CMuunSystem.MuunItemEquipment(lpObj->m_Index, target, source);
		if (target == 0 || source == 0) {
			GCMuunEquipmentChange(lpObj->m_Index, 0);
		}
	}
	return 22; // :D
}

BYTE gObjMuunInventoryInsertItem(int aIndex, CMapItem* item) {
	CItem copyitem;
	int iwidth, iheight;
	if (!item->GetSize(iwidth, iheight)) {
		LogAdd(lMsg.Get(527), __FILE__, __LINE__);
		return (BYTE)-1;
	}
	BYTE Ret = gObjChkMuunInventoryEmpty(&gObj[aIndex]);
	if (Ret == (BYTE)-1) {
		return (BYTE)-1;
	}
	if (!gObj[aIndex].pMuunInventory[Ret].IsItem()) {
		return (BYTE)-1;
	}
	copyitem.m_Level = item->m_Level;
	copyitem.m_Durability = item->m_Durability;
	copyitem.Convert(item->m_Type, item->m_Option1, item->m_Option2, item->m_Option3, item->m_NewOption,
		item->m_SetOption, item->m_ItemOptionEx, item->m_SocketOption, item->m_SocketBonus, CURRENT_DB_VERSION);
	copyitem.m_Number = item->m_Number;
	memcpy(&gObj[aIndex].pMuunInventory[Ret], &copyitem, sizeof(gObj[aIndex].pMuunInventory[Ret]));
	if (g_CMuunSystem.IsMuunItem(item->m_Type)) {
		g_CMuunSystem.SetMuunItemAddPeriodData(&gObj[aIndex], item->m_Type, item->m_Number);
	}
	return Ret;
}

// missing 2 calls
BYTE gObjMuunInventoryInsertItem(int aIndex, CItem item) {
	int iwidth, iheight;
	if (!item.GetSize(iwidth, iheight)) {
		LogAdd(lMsg.Get(527), __FILE__, __LINE__);
		return (BYTE)-1;
	}
	BYTE Ret = gObjChkMuunInventoryEmpty(&gObj[aIndex]);
	if (Ret == (BYTE)-1) {
		return (BYTE)-1;
	}
	if (!gObj[aIndex].pMuunInventory[Ret].IsItem()) {
		return (BYTE)-1;
	}
	memcpy(&gObj[aIndex].pMuunInventory[Ret], &item, sizeof(gObj[aIndex].pMuunInventory[Ret]));
	if (g_CMuunSystem.IsMuunItem(item.m_Type)) {
		g_CMuunSystem.SetMuunItemAddPeriodData(&gObj[aIndex], item.m_Type, item.m_Number);
	}
	return Ret;
}

BYTE gObjMuunInventoryInsertItemPos(int aIndex, CItem item, int pos) {
	if (pos < 0 || pos > MUUN_INVENTORY_SIZE) {
		OBJECTSTRUCT* lpObj = &gObj[aIndex];
		if (!gObj[aIndex].pMuunInventory[pos].IsItem()) {
			return (BYTE)-1;
		}
		if (item.IsItem()) {
			memcpy(&lpObj->pMuunInventory[pos], &item, sizeof(lpObj->pMuunInventory[pos]));
			return pos;
		}
	}
	return (BYTE)-1;
}

BYTE gObjChkMuunInventoryEmpty(OBJECTSTRUCT* lpObj) {
	for (int i = 2; i < MUUN_INVENTORY_SIZE; i++) {
		if (!lpObj->pMuunInventory[i].IsItem()) {
			return i;
		}
	}
	return (BYTE)-1;
}
#endif

BYTE gObjInventoryInsertItem(int aIndex, int item_num)
{
	int x,w,h,map_num,iwidth,iheight;
	BYTE blank;

	blank = 0;
	map_num = gObj[aIndex].MapNumber;

	if(MapC[map_num].m_cItem[item_num].live == 0)
	{
		return -1;
	}

	if(MapC[map_num].m_cItem[item_num].GetSize((int &)iwidth,(int &)iheight) == 0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

#if (ENABLETEST_WZINVENTORYEXPAND == 1)
	for(h = 0; h < 24; h++)
#else
	for(h = 0; h < 8; h++)
#endif
	{
		for(w = 0; w < 8; w++)
		{
			if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(aIndex,w,h,iwidth,iheight);

				if(blank == 254)
				{
#if (ENABLETEST_WZINVENTORYEXPAND == 1)
					goto GOTO_EndFunc;
#else
					goto GOTO_ExtendCheck;
#endif
				}

				if(blank != 255)
				{
					gObj[aIndex].pInventory[blank] = MapC[map_num].m_cItem[item_num];
					gObjInventoryItemSet(aIndex,blank,1);
					return blank;
				}
			}
		}
	}
#if (ENABLETEST_WZINVENTORYEXPAND == 0)
GOTO_ExtendCheck:

	if( gObj[aIndex].pInventoryExtend > 0 )
	{
		for(x = 0; x < gObj[aIndex].pInventoryExtend; x++ )
		{
			for(h = 8+(x*4); h < 12+(x*4); h++)
			{
				for(w = 0; w < 8; w++)
				{
					if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
					{
						blank = gObjExtendedInventoryRectCheck(aIndex,w,h,iwidth,iheight,x);

						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}

						if(blank != 255)
						{
							gObj[aIndex].pInventory[blank] = MapC[map_num].m_cItem[item_num];
							gObjInventoryItemSet(aIndex,blank,1);
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
#else
GOTO_EndFunc:
#endif
	return -1;

}






DWORD gGetItemNumber()	
{
	return gItemNumberCount;
}






void gPlusItemNumber() 
{
	return;	// Maybe here Apply deathway fix (gItemNumberCount++)
}






BYTE gObjInventoryInsertItem(int aIndex, CItem item)
{
	int x,w,h,iwidth,iheight;
	BYTE blank = 0;

	if(item.GetSize((int&)iwidth,(int&)iheight)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

#if (ENABLETEST_WZINVENTORYEXPAND == 1)
	for(h = 0; h < 24; h++)
#else
	for(h = 0; h < 8; h++)
#endif
	{
		for( w = 0; w < 8; w++)
		{
			if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(aIndex,w,h,iwidth,iheight);

				if(blank == 254)
				{
#if (ENABLETEST_WZINVENTORYEXPAND == 1)
					goto GOTO_EndFunc;
#else
					goto GOTO_ExtendCheck;
#endif
				}

				if(blank != 255)
				{
					if(gObjCheckSerial0ItemList(&item)!=0)
					{
						MsgOutput(aIndex,lMsg.Get(3354));
						return -1;
					}

					if(gObjInventorySearchSerialNumber(&gObj[aIndex],item.m_Number) == 0)
					{
						return -1;
					}

					gObj[aIndex].pInventory[blank] = item;

					gObjInventoryItemSet(aIndex,blank,1);
					return blank;
				}
			}
		}
	}
#if (ENABLETEST_WZINVENTORYEXPAND == 0)
GOTO_ExtendCheck:

	if( gObj[aIndex].pInventoryExtend > 0 )
	{
		for( x = 0; x < gObj[aIndex].pInventoryExtend; x++ )
		{
			for(h = 8+(x*4); h < 12+(x*4); h++)
			{
				for( w = 0; w < 8; w++)
				{
					if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
					{
						blank = gObjExtendedInventoryRectCheck(aIndex,w,h,iwidth,iheight,x);

						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}

						if(blank != 255)
						{
							if(gObjCheckSerial0ItemList(&item)!=0)
							{	
								MsgOutput(aIndex,lMsg.Get(3354));
								return -1;
							}

							if(gObjInventorySearchSerialNumber(&gObj[aIndex],item.m_Number) == 0)
							{
								return -1;
							}

							gObj[aIndex].pInventory[blank] = item;

							gObjInventoryItemSet(aIndex,blank,1);
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
#else
GOTO_EndFunc:
#endif
	return -1;
}






BYTE gObjOnlyInventoryInsertItem(int aIndex, CItem item)
{
	int x,w,h,iwidth,iheight;
	BYTE blank = 0;

	if(item.GetSize((int&)iwidth,(int&)iheight)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

#if (ENABLETEST_WZINVENTORYEXPAND == 1)
	for(h = 0; h < 24; h++)
#else
	for(h = 0; h < 8; h++)
#endif
	{
		for( w = 0; w < 8; w++)
		{
			if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(aIndex,w,h,iwidth,iheight);

				if(blank == 254)
				{
#if (ENABLETEST_WZINVENTORYEXPAND == 1)
					goto GOTO_EndFunc;
#else
					goto GOTO_ExtendCheck;
#endif
				}

				if(blank != 255)
				{
					if(gObjCheckSerial0ItemList(&item)!=0)
					{
						MsgOutput(aIndex,lMsg.Get(3354));
						return -1;
					}

					if(gObjInventorySearchSerialNumber(&gObj[aIndex],item.m_Number) == 0)
					{
						return -1;
					}

					gObj[aIndex].pInventory[blank] = item;

					gObjInventoryItemSet(aIndex,blank,1);
					return blank;
				}
			}
		}
	}
#if (ENABLETEST_WZINVENTORYEXPAND == 0)
GOTO_ExtendCheck:

	if( gObj[aIndex].pInventoryExtend > 0 )
	{
		for(x = 0; x < gObj[aIndex].pInventoryExtend; x++ )
		{
			for(h = 8+(4*x); h < 12+(4*x); h++)
			{
				for( w = 0; w < 8; w++)
				{
					if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
					{
						blank = gObjExtendedInventoryRectCheck(aIndex,w,h,iwidth,iheight,x);

						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}

						if(blank != 255)
						{
							if(gObjCheckSerial0ItemList(&item)!=0)
							{
								MsgOutput(aIndex,lMsg.Get(3354));
								return -1;
							}

							if(gObjInventorySearchSerialNumber(&gObj[aIndex],item.m_Number) == 0)
							{
								return -1;
							}

							gObj[aIndex].pInventory[blank] = item;
							gObjInventoryItemSet(aIndex,blank,1);
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
#else
GOTO_EndFunc:
#endif
	return -1;
}






BYTE gObjInventoryInsertItem(int aIndex, CMapItem * item)
{
	int x,w,h,iwidth,iheight;
	BYTE blank = 0;

	CItem copyitem;


	if(item->GetSize((int &)iwidth,(int &)iheight)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

#if (ENABLETEST_WZINVENTORYEXPAND == 1)
	for(h = 0; h < 24; h++)
#else
	for(h = 0; h < 8; h++)
#endif
	{
		for(w = 0; w < 8; w++)
		{
			if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(aIndex,w,h,iwidth,iheight);

				if(blank == 254)
				{
#if (ENABLETEST_WZINVENTORYEXPAND == 1)
					goto GOTO_EndFunc;
#else
					goto GOTO_ExtendCheck;
#endif
				}
	
				if(blank != 255)
				{
					copyitem.m_Level = item->m_Level;
					copyitem.m_Durability = item->m_Durability;
	
					copyitem.Convert(item->m_Type,item->m_Option1,item->m_Option2,item->m_Option3,item->m_NewOption,item->m_SetOption,item->m_ItemOptionEx,&item->m_SocketOption[0],item->m_SocketBonus,CURRENT_DB_VERSION);
					copyitem.SetPetItemInfo(item->m_PetItem_Level,item->m_PetItem_Exp);
	
					copyitem.m_Number = item->m_Number;
	
					gObj[aIndex].pInventory[blank] = copyitem;
		
					gObjInventoryItemSet(aIndex,blank,1);
					return blank;
				}
			}
		}
	}
#if (ENABLETEST_WZINVENTORYEXPAND == 0)
GOTO_ExtendCheck:

	if( gObj[aIndex].pInventoryExtend > 0 )
	{
		for(x = 0; x < gObj[aIndex].pInventoryExtend; x++)
		{
			for(h = 8+(x*4); h < 12+(x*4); h++)
			{
				for(w = 0; w < 8; w++)
				{
					if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
					{
						blank = gObjExtendedInventoryRectCheck(aIndex,w,h,iwidth,iheight,x);

						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}

						if(blank != 255)
						{
							copyitem.m_Level = item->m_Level;
							copyitem.m_Durability = item->m_Durability;	
							copyitem.Convert(item->m_Type,item->m_Option1,item->m_Option2,item->m_Option3,item->m_NewOption,item->m_SetOption,item->m_ItemOptionEx,&item->m_SocketOption[0],item->m_SocketBonus,CURRENT_DB_VERSION);
							copyitem.SetPetItemInfo(item->m_PetItem_Level,item->m_PetItem_Exp);
							copyitem.m_Number = item->m_Number;
							gObj[aIndex].pInventory[blank] = copyitem;
							gObjInventoryItemSet(aIndex,blank,1);
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
#else
GOTO_EndFunc:
#endif
	return -1;
}






BOOL gObjSearchItem(LPOBJ lpObj, int item_type, int add_dur, int nLevel)
{
	for(int n = INVETORY_WEAR_SIZE; n < MAIN_INVENTORY_SIZE; n++)
	{
		if(lpObj->pInventory[n].IsItem() == 1)
		{
			if(lpObj->pInventory[n].m_Type == item_type)
			{
				if(lpObj->pInventory[n].m_Level == nLevel)
				{
					if((int)lpObj->pInventory[n].m_Durability + add_dur <= 3)
					{
						lpObj->pInventory[n].m_Durability += add_dur;
						GCItemDurSend(lpObj->m_Index,n,(BYTE)lpObj->pInventory[n].m_Durability,0);
						return true;
					}
				}
			}
		}
	}
	return false;
}

BOOL gObjSearchItemPlus(LPOBJ lpObj,int iItemCode, int iItemLevel,int AddDur, int MaxDur)
{
	for(int n = INVETORY_WEAR_SIZE; n < MAIN_INVENTORY_SIZE; n++)
	{
		if(lpObj->pInventory[n].IsItem() == TRUE
			&& lpObj->pInventory[n].m_Type == iItemCode
			&& lpObj->pInventory[n].m_Level == iItemLevel)
		{
			if(lpObj->pInventory[n].m_Durability + AddDur <= MaxDur)
			{
				lpObj->pInventory[n].m_Durability += AddDur;
				GCItemDurSend(lpObj->m_Index,n,(BYTE)lpObj->pInventory[n].m_Durability,0);
				return true;
			}
		}
	}
return false;
}



BOOL gObjSearchItemMinus(LPOBJ lpObj, int pos, int m_dur)
{
	int n = pos;
	if(n < 0 || n > (MAIN_INVENTORY_SIZE-1))
	{
		LogAdd("error-L4 : %s %d",__FILE__,__LINE__);
		return 1;
	}

	if(lpObj->pInventory[n].IsItem() == 1)
	{
		if((int)lpObj->pInventory[n].m_Durability - m_dur > 0)
		{
			lpObj->pInventory[n].m_Durability -= m_dur;
			GCItemDurSend(lpObj->m_Index,n,(BYTE)lpObj->pInventory[n].m_Durability,1);
			return 1;
		}
	}
	return 0;
}




BYTE gObjShopBuyInventoryInsertItem(int aIndex, CItem item)
{
	int x,w,h,iwidth,iheight;
	BYTE blank = 0;

	if(item.GetSize(iwidth,iheight)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

#if (ENABLETEST_WZINVENTORYEXPAND == 1)
	for(h = 0; h < 24; h++)
#else
	for(h = 0; h < 8; h++)
#endif
	{
		for( w = 0; w < 8; w++)
		{
			if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(aIndex,w,h,iwidth,iheight);

				if(blank == 254)
				{
#if (ENABLETEST_WZINVENTORYEXPAND == 1)
					goto GOTO_EndFunc;
#else
					goto GOTO_ExtendCheck;
#endif
				}

				if(blank != 255)
				{
					gObj[aIndex].pInventory[blank] = item;

					if(item.m_serial == 0)
					{
						gObj[aIndex].pInventory[blank].m_Number = 0;
					}
					else
					{
						gObj[aIndex].pInventory[blank].m_Number = gGetItemNumber();
						gPlusItemNumber();
					}

					gObjInventoryItemSet(aIndex,blank,1);
					return blank;
				}
			}
		}
	}
#if (ENABLETEST_WZINVENTORYEXPAND == 0)
GOTO_ExtendCheck:

	if( gObj[aIndex].pInventoryExtend > 0 )
	{
		for(x = 0; x < gObj[aIndex].pInventoryExtend; x++)
		{
			for(h = (8+x*4); h < (12+x*4); h++)
			{
				for( w = 0; w < 8; w++)
				{
					if(*(BYTE*)(gObj[aIndex].pInventoryMap+h*8+w) == 255)
					{
						blank = gObjExtendedInventoryRectCheck(aIndex,w,h,iwidth,iheight,x);
		
						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}
		
						if(blank != 255)
						{
							gObj[aIndex].pInventory[blank] = item;
		
							if(item.m_serial == 0)
							{
								gObj[aIndex].pInventory[blank].m_Number = 0;
							}
							else
							{
								gObj[aIndex].pInventory[blank].m_Number = gGetItemNumber();
								gPlusItemNumber();
							}	
			
							gObjInventoryItemSet(aIndex,blank,1);
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
#else
GOTO_EndFunc:
#endif
	return -1;
}


BYTE gObjShopBuyInventoryInsertItem(LPOBJ lpObj, int type, int index, int level, int iSerial, int iDur)
{
	int w,h,iwidth,iheight;
	BYTE blank = 0;
	CItem item;

	int item_type = ItemGetNumberMake(type,index);
	item.m_Level = level;
	item.m_Durability = (float)iDur;

	item.Convert(item_type,0,0,0,0,0,0,0,-1,CURRENT_DB_VERSION);

	if(item.GetSize((int &)iwidth,(int &)iheight)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

	for(h = 0; h < 8 + (lpObj->pInventoryExtend * 4); h++)
	{
		for(w = 0; w < 8; w++)
		{
			if(*(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight);

				if(blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if(blank != 255)
				{
					lpObj->pInventory[blank] = item;

					if ( !item.m_serial )
					{
						lpObj->pInventory[blank].m_Number = 0;
					}
					else
					{
						lpObj->pInventory[blank].m_Number = gGetItemNumber();
						gPlusItemNumber();
					}
					
					gObjInventoryItemSet(lpObj->m_Index,blank,1);
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

BYTE gObjInventoryInsertItemTemp(LPOBJ lpObj, CMapItem * Item) //Revised
{
#if (ENABLETEST_WZINVENTORYEXPAND == 1)
	CItem item;
	int w,h,iwidth,iheight;
	BYTE blank = 0;

	if(Item->GetSize((int &)iwidth,(int &)iheight)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

	for(h = 0; h < 24; h++)
	{
		for(w = 0; w < 8; w++)
		{
			if( *(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255 )
			{
				blank = gObjInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight);

				if(blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if(blank != 255)
				{
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
#else
	CItem item;
	int x,w,h,iwidth,iheight;
	BYTE blank = 0;

	if(Item->GetSize((int &)iwidth,(int &)iheight)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

	for(h = 0; h < 8; h++)
	{
		for(w = 0; w < 8; w++)
		{
			if(*(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight);

				if(blank == 254)
				{
					goto GOTO_ExtendCheck;
				}

				if(blank != 255)
				{
					return blank;
				}
			}
		}
	}
GOTO_ExtendCheck:

	if( lpObj->pInventoryExtend > 0 )
	{
		for(x = 0; x < lpObj->pInventoryExtend; x++)
		{
			for(h = (8+x*4); h < (12+x*4); h++)
			{
				for( w = 0; w < 8; w++)
				{
					if(*(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255)
					{
						blank = gObjExtendedInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight,x);
		
						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}
		
						if(blank != 255)
						{
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
	return -1;
#endif
}







BYTE gObjInventoryInsertItem(LPOBJ lpObj, int type, int index, int level)
{
	CItem item;
	int x,w,h,iwidth,iheight;
	BYTE blank = 0;

	int item_type = ItemGetNumberMake(type,index);

	item.m_Level = level;

	item.Convert(item_type,0,0,0,0,0,0,0,-1,CURRENT_DB_VERSION);

	if(item.GetSize((int &)iwidth,(int &)iheight)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

#if (ENABLETEST_WZINVENTORYEXPAND == 1)
	for(h = 0; h < 24; h++)
#else
	for(h = 0; h < 8; h++)
#endif
	{
		for( w = 0; w < 8; w++)
		{

			if(*(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight);
	
				if(blank == 254)
				{
#if (ENABLETEST_WZINVENTORYEXPAND == 1)
					goto GOTO_EndFunc;
#else
					goto GOTO_ExtendCheck;
#endif
				}
	
				if(blank != 255)
				{
					lpObj->pInventory[blank] = item;
					lpObj->pInventory[blank].m_Number = gGetItemNumber();
					gPlusItemNumber();

					gObjInventoryItemSet(lpObj->m_Index,blank,1);
					return blank;
				}
			}
		}
	}
#if (ENABLETEST_WZINVENTORYEXPAND == 0)
GOTO_ExtendCheck:

	if( lpObj->pInventoryExtend > 0 )
	{
		for( x = 0; x < lpObj->pInventoryExtend; x++ )
		{
			for(h = 8+(x*4); h < 12+(x*4); h++)
			{
				for( w = 0; w < 8; w++)
				{
					if(*(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255)
					{
						blank = gObjExtendedInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight,x);
						
						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}

						if(blank != 255)
						{
							lpObj->pInventory[blank] = item;
							lpObj->pInventory[blank].m_Number = gGetItemNumber();
							gPlusItemNumber();
							gObjInventoryItemSet(lpObj->m_Index,blank,1);
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
#else
GOTO_EndFunc:
#endif
	return -1;
}






BYTE gObjInventoryInsertItem(LPOBJ lpObj, int type, int index, int level, int iSerial, int iDur)
{
	CItem item;
	int x,w,h,iwidth,iheight;
	BYTE blank = 0;

	int item_type = ItemGetNumberMake(type,index);
	item.m_Level = level;
	item.m_Durability = (float)iDur;

	item.Convert(item_type,0,0,0,0,0,0,0,-1,CURRENT_DB_VERSION);

	if(item.GetSize((int &)iwidth,(int &)iheight)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return -1;
	}

#if (ENABLETEST_WZINVENTORYEXPAND == 1)
	for(h = 0; h < 24; h++)
#else
	for(h = 0; h < 8; h++)
#endif
	{
		for(w = 0; w < 8; w++)
		{
			if(*(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight);

				if(blank == 254)
				{
#if (ENABLETEST_WZINVENTORYEXPAND == 1)
					goto GOTO_EndFunc;
#else
					goto GOTO_ExtendCheck;
#endif
				}

				if(blank != 255)
				{
					lpObj->pInventory[blank] = item;
					lpObj->pInventory[blank].m_Number = iSerial;
					gPlusItemNumber();

					gObjInventoryItemSet(lpObj->m_Index,blank,1);
					return blank;
				}
			}
		}
	}
#if (ENABLETEST_WZINVENTORYEXPAND == 0)
GOTO_ExtendCheck:

	if( lpObj->pInventoryExtend > 0 )
	{
		for( x = 0; x < lpObj->pInventoryExtend; x++ )
		{
			for(h = 8+(x*4); h < 12+(x*4); h++)
			{
				for(w = 0; w < 8; w++)
				{
					if(*(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255)
					{
						blank = gObjExtendedInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight,x);

						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}

						if(blank != 255)
						{
							lpObj->pInventory[blank] = item;
							lpObj->pInventory[blank].m_Number = iSerial;
							gPlusItemNumber();

							gObjInventoryItemSet(lpObj->m_Index,blank,1);
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
#else
GOTO_EndFunc:
#endif
	return -1;
}






BYTE gObjMonsterInventoryInsertItem(LPOBJ lpObj, int type, int index, int level, int op1, int op2, int op3)
{
	CItem item;
	int x,w,h,iwidth,iheight;
	BYTE blank = 0;
	int item_type;
	int a = 0;

	item_type = ItemGetNumberMake(type,index);
	item.m_Level = level;

	if(type == 13 && index == 10)
	{
		item.m_Durability = rand()%100+100.0f;
	}

	item.Convert(item_type,op1,op2,op3,0,0,0,0,-1,CURRENT_DB_VERSION);

	lpObj->pInventory[blank] = item;
	iwidth = 1;
	iheight = 1;

#if (ENABLETEST_WZINVENTORYEXPAND == 1)
	for(h = 0; h < 24; h++)
#else
	for(h = 0; h < 8; h++)
#endif
	{
		for(w = 0; w <8; w++)
		{
			if(*(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight);

				if(blank == 254)
				{
#if (ENABLETEST_WZINVENTORYEXPAND == 1)
					goto GOTO_EndFunc;
#else
					goto GOTO_ExtendCheck;
#endif
				}

				if(blank != 255)
				{
					lpObj->pInventory[blank] = item;
					*lpObj->pInventoryCount += 1;

					gObjInventoryItemBoxSet(lpObj->m_Index,blank,iwidth,iwidth,1);
					return blank;
				}
			}
		}
	}
#if (ENABLETEST_WZINVENTORYEXPAND == 0)
GOTO_ExtendCheck:

	if( lpObj->pInventoryExtend > 0 )
	{
		for( x = 0; x < lpObj->pInventoryExtend; x++ )
		{
			for(h = 8+(x*4); h < 12+(x*4); h++)
			{
				for(w = 0; w <8; w++)
				{
					if(*(BYTE*)(lpObj->pInventoryMap+h*8+w) == 255)
					{
						blank = gObjExtendedInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight,x);

						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}

						if(blank != 255)
						{
							lpObj->pInventory[blank] = item;
							*lpObj->pInventoryCount += 1;
							gObjInventoryItemBoxSet(lpObj->m_Index,blank,iwidth,iwidth,1);
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
#else
GOTO_EndFunc:
#endif
	return -1;
}






BYTE gObjInventoryRectCheck(int aIndex, int sx, int sy, int width, int height)
{
	int x,y;
	int blank = 0;

	if(sx + width > 8)
	{
		return -1;
	}

	if(sy + height > 28)
	{
		return -2;
	}

	int xx,yy;
#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
	int bResult = CheckOutOfInventory(aIndex, sy, height);

	if( !bResult )
	{
		if( bResult == -1 )
		{
			return -2;
		}

		return -1;
	}
#endif

	for(y = 0; y < height; y ++)
	{

		yy = sy+y;

		for(x = 0; x < width; x++)
		{
			xx = sx + x;
#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
			if( InventoryExtentCheck(xx, yy, 8, 28) == 1 )
#else
			if( ExtentCheck(xx,yy,8,28) == 1 )
#endif
			{
				if(*(BYTE*)(gObj[aIndex].pInventoryMap+(sy+y)*8+(sx+x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				LogAdd("error : %s %d",__FILE__,__LINE__);
				return -1;
			}
		}
	}

	if(blank == 0)
	{
		return sx+sy*8+12;
	}
	return  -1;
}


BOOL InventoryExtentCheck(int x, int y, int w, int h) //Decompiled ex702
{
	int nEndOfRange;
	int nStartOfRange;

	nStartOfRange = 0;
	nEndOfRange = 0;

	if( y < 0 )
	{
		return 0;
	}

	if( y < 8 )
	{
		nStartOfRange = 0;
		nEndOfRange = 8;
	}
	else if( y < 12 )
	{
		nStartOfRange = 8;
		nEndOfRange = 12;
	}
	else if( y < 16 )
	{
		nStartOfRange = 12;
        nEndOfRange = 16;
	}
	else if( y < 20 )
	{
		nStartOfRange = 16;
        nEndOfRange = 20;
	}
	else if( y < 24 )
	{
		nStartOfRange = 20;
        nEndOfRange = 24;
	}
	else if( y < 28 )
	{
		nStartOfRange = 24;
        nEndOfRange = 28;
	}
	else
	{
		return 0;
	}

	if( nEndOfRange > h )
	{
		nEndOfRange = h;
	}

	if( x >= 0 && x < w && y >= nStartOfRange && y < nEndOfRange )
	{
		return 1;
	}
	
	return 0;
}

int CheckOutOfInventory(int aIndex, int sy, int height) //Decompiled ex702
{
	int nEndofItemY = height + sy - 1;
	
	if( sy >= gObj[aIndex].pInventoryExtend * 4 + 8 )
	{
		if( sy >= 24 && sy <= 27 )
		{
			
		}
		else
		{
			return -1;
		}
	}

	if( sy < 0 )
	{
		return 0;
	}
	else if( sy < 8 )
	{
		if( nEndofItemY >= 8 )
		{
			return 0;
		}
	}
	else if( sy < 12 )
	{
		if( nEndofItemY >= 12 )
		{
			return 0;
		}
	}
	else if( sy < 16 )
	{
		if( nEndofItemY >= 16 )
		{
			return 0;
		}
	}
	else if( sy < 20 )
	{
		if( nEndofItemY >= 20 )
		{
			return 0;
		}
	}
	else if( sy < 24 )
	{
		if( nEndofItemY >= 24 )
		{
			return 0;
		}
	}
	else if( sy < 28 )
	{
		if( nEndofItemY >= 28 )
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	return 1;
}

//005664f0	-> 100% (1.01.00) [NEW]
BOOL CheckOutOfWarehouse(int aIndex, int sy, int height)
{
	if( sy >= 15 * gObj[aIndex].pWarehouseExtend + 15 )
	{
		return false;
	}
	// ----
	int nEndofItemY = sy + height - 1;
	// ----
	if( sy < 0 )
	{
		return false;
	}
	// ----
	if( sy >= 15 )
	{
		if( sy >= 30 )
		{
			return false;
		}
		// ----
		if( nEndofItemY >= 30 )
		{
			return false;
		}
	}
	else
	{
		if( nEndofItemY >= 15 )
		{
			return false;
		}
	}
	// ----
	return true;
}




BYTE gObjOnlyInventoryRectCheck(int aIndex, int sx, int sy, int width, int height)
{
	int x,y;
	int blank = 0;

	if(sx + width > 8)
	{
		return -1;
	}

	if(sy + height > 8 + (gObj[aIndex].pInventoryExtend * 4))
	{
		return -2;
	}
	
#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
	int bResult = CheckOutOfInventory(aIndex, sy, height);
	
	if ( !bResult )
	{
		if ( bResult == -1 )
		{
			return -2;
		}
		return -1;
	}
#endif

	int xx,yy;

	for(y = 0; y < height; y ++)
	{

		yy = sy+y;

		for(x = 0; x < width; x++)
		{
			xx = sx + x;
#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
			if( InventoryExtentCheck(xx,yy,8,24) == 1 )
#else
			if( ExtentCheck(xx,yy,8,8+(gObj[aIndex].pInventoryExtend * 4)) == 1 )
#endif
			{
				if(*(BYTE*)(gObj[aIndex].pInventoryMap+(sy+y)*8+(sx+x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				LogAdd("error : %s %d",__FILE__,__LINE__);
				return -1;
			}
		}
	}

	if(blank == 0)
	{
		return sx+sy*8+12;
	}
	return  -1;
}

BYTE gObjExtendedInventoryRectCheck(int aIndex, int sx, int sy, int width, int height,int id)
{
	int x,y;
	int blank = 0;

	if(sx + width > 8)
	{
		return -1;
	}

	if(sy + height > 12+(id*4) )
	{
		return -2;
	}

	/*bool bResult = CheckOutOfInventory(aIndex, sy, height);
	// ----
	if( !bResult )
	{
		if( bResult == -1 )
		{
			return -2;
		}
		// ----
		return -1;
	}*/

	int xx,yy;

	for(y = 0; y < height; y ++)
	{

		yy = sy+y;

		for(x = 0; x < width; x++)
		{
			xx = sx + x;

			if(ExtentCheck(xx,yy,8,12+(id*4))==1)
			{
				if(*(BYTE*)(gObj[aIndex].pInventoryMap+(sy+y)*8+(sx+x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				LogAdd("error : %s %d",__FILE__,__LINE__);
				return -1;
			}
		}
	}

	if(blank == 0)
	{
		return sx+sy*8+12;
	}
	return  -1;
}


BOOL CheckInventoryEmptySpace(LPOBJ lpObj, int iItemHeight, int iItemWidth)
{
	int h=0;
	int w=0;
	BYTE blank = 0;
	
#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
	for( h=0; h < 8 + (lpObj->pInventoryExtend * 4); h++ )
#else
	for( h=0; h < 24; h++ )
#endif
	{
		for(w=0;w<8;w++)
		{
			if(*(BYTE*)(lpObj->pInventoryMap+(h)*8+(w)) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(lpObj->m_Index, w, h, iItemWidth, iItemHeight);

				if ( blank == 0xFE )
					return FALSE;

				if ( blank != 0xFF )
					return TRUE;
			}
		}
	}

	return FALSE;
}



BOOL gObjIsItemPut(LPOBJ lpObj, CItem * lpItem, int pos )
{
	if(lpItem->m_TwoHand == 1)
	{
		if(pos == 0 || pos == 1)
		{
			if(lpObj->pInventory[0].IsItem() == 1)
			{
				if(lpObj->pInventory[0].m_Type == ITEMGET(4,7) || lpObj->pInventory[0].m_Type == ITEMGET(4,15))
				{
					
				}
				else
				{
					return false;
				}
			}

			if(lpObj->pInventory[1].IsItem() == 1)
			{
				if(lpObj->pInventory[1].m_Type == ITEMGET(4,7) || lpObj->pInventory[1].m_Type == ITEMGET(4,15))
				{
					
				}
				else
				{
					return false;
				}
			}
		}
	}

	if(pos == 1)
	{
		if(lpObj->pInventory[0].IsItem() == 1)
		{
			if(lpItem->m_Type == ITEMGET(4,7) || lpItem->m_Type == ITEMGET(4,15))
			{
					
			}
			else
			{
				if(lpObj->pInventory[0].m_TwoHand)
				{
					return false;
				}
			}
		}
	}

	if( lpObj->Class == CLASS_DARKLORD &&
		lpItem->m_Type >> 9 == 2 &&
		lpItem->m_Part != pos)
	{
		return FALSE;
	}

	if( lpObj->Class == CLASS_WIZARD &&
		lpItem->m_Type >> 9 == 5 &&
		lpItem->m_Part != pos)
	{
		return FALSE;
	}

	if( lpObj->Class == CLASS_SUMMONER &&
		lpItem->m_Type >> 9 == 5 &&
		lpItem->m_Part != pos)
	{
		return FALSE;
	}


	if(lpItem->m_Part != pos)
	{
		if(pos == 1 || pos == 0)
		{
			if(lpItem->m_Part == 1 || lpItem->m_Part == 0)
			{

			}
			else
			{
				return false;
			}
		}
		else if(pos == 11)
		{
			if(lpItem->m_Part == 10)
			{

			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	if((lpObj->Strength + lpObj->AddStrength) < (lpItem->m_RequireStrength - lpItem->m_HJOpStrength))
	{
		return false;
	}

	if((lpObj->Dexterity + lpObj->AddDexterity) < (lpItem->m_RequireDexterity - lpItem->m_HJOpDexterity))
	{
		return false;
	}

	if(lpObj->Level < lpItem->m_RequireLevel)
	{
		return false;
	}

	if(lpItem->m_Type >= 0 && lpItem->m_Type < ITEMGET(12,0))
	{
		if((lpObj->Vitality + lpObj->AddVitality) < lpItem->m_RequireVitality)
		{
			return false;
		}

		if((lpObj->Energy + lpObj->AddEnergy) < lpItem->m_RequireEnergy)
		{
			return false;
		}
	}

	if((lpObj->Leadership + lpObj->AddLeadership) < lpItem->m_RequireLeaderShip)
	{
		return false;
	}

	if(lpItem->m_Type >= ITEMGET(12,0) && lpItem->m_Type <= ITEMGET(12,6))
	{
		if(lpObj->Level < lpItem->m_RequireLevel)
		{
			return false;
		}
	}
	
	if( lpItem->m_Type == ITEMGET(12,41) )
	{
		if(lpObj->Level < lpItem->m_RequireLevel)
		{
			return false;
		}
	}
	
	if( lpItem->m_Type == ITEMGET(12,42) ||
			 lpItem->m_Type == ITEMGET(12,43) )
	
	{
		if(lpObj->Level < lpItem->m_RequireLevel)
		{
			return false;
		}
	}
	else if(lpItem->m_Type >= ITEMGET(13,0) && lpItem->m_Type <= ITEMGET(13,15))
	{
		if(lpItem->m_Type == ITEMGET(13,10))
		{
			if(lpItem->m_Level <= 2)
			{
				if(lpObj->Level < 20)
				{
					return false;
				}
			}
			else
			{
				if(lpObj->Level < 50)
				{
					return false;
				}
			}
		}
		else if(lpItem->m_Type == ITEMGET(13,2))
		{
			if(lpObj->MapNumber == 7 || lpObj->Level < lpItem->m_RequireLevel)
			{
				return false;
			}
		}
		else if(lpItem->m_Type == ITEMGET(13,3))
		{
			if(lpObj->MapNumber == 7 || lpObj->Level < lpItem->m_RequireLevel)
			{
				return false;
			}
		}
		else
		{
			if(lpObj->Level < lpItem->m_RequireLevel)
			{
				return false;
			}
		}
	}

	if( IT_MAP_RANGE(lpObj->MapNumber) )
	{
		if( IsTransformRing(lpItem->m_Type) )
		{
			return FALSE;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,20) && lpItem->m_Level == 0)
	{
		int count = gObjGetItemCountInEquipment(lpObj->m_Index,13,20,0);

		if(count != 0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,40) )
	{
		int count = gObjGetItemCountInEquipment(lpObj->m_Index,13,40,0);

		if(count > 0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,41) )
	{
		int count = gObjGetItemCountInEquipment(lpObj->m_Index,13,40,0);

		if(count > 0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,42))
	{
		if(CC_MAP_RANGE(lpObj->MapNumber))
		{
			return false;
		}

		int count = gObjGetItemCountInEquipment(lpObj->m_Index,13,42,0);

		if(count > 0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,68) )
	{
		int count = gObjGetItemCountInEquipment(lpObj->m_Index,13,68,0);

		if(count > 0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,20) && lpItem->m_Level == 3)
	{
		int count = gObjGetItemCountInEquipment(lpObj->m_Index,13,20,3);

		if(count != 0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,10))
	{
		if(CC_MAP_RANGE(lpObj->MapNumber))
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,37))
	{
		if(CC_MAP_RANGE(lpObj->MapNumber))
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,38))
	{
		int count = gObjGetItemCountInEquipment(lpObj->m_Index,13,38,0);

		if(count != 0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,39))
	{
		if(CC_MAP_RANGE(lpObj->MapNumber))
		{
			return false;
		}

		int count = 0;

		for(int i = 0; i < 7; i++)
		{
			count = gObjGetItemCountInEquipment(lpObj->m_Index,13,10,i);

			if(count!=0)
			{
				return false;
			}
		}

		count = gObjGetItemCountInEquipment(lpObj->m_Index,13,39,0);

		if(count!=0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,10))
	{
		int count = 0;
		for(int i = 0; i < 7; i++)
		{
			count = gObjGetItemCountInEquipment(lpObj->m_Index,13,10,i);

			if(count!=0)
			{
				return false;
			}
		}
		count = gObjGetItemCountInEquipment(lpObj->m_Index,13,39,0);
		if(count!=0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,40))
	{
		if(CC_MAP_RANGE(lpObj->MapNumber))
		{
			return false;
		}

		int count = 0;
		count = gObjGetItemCountInEquipment(lpObj->m_Index,13,40,0);

		if(count > 0)
		{
			return false;
		}
	}

	if(lpItem->m_Type == ITEMGET(13,41))
	{
		if(CC_MAP_RANGE(lpObj->MapNumber))
		{
			return false;
		}

		int count = 0;
		count = gObjGetItemCountInEquipment(lpObj->m_Index,13,41,0);

		if(count > 0)
		{
			return false;
		}
	}
	else if(lpItem->m_Type == ITEMGET(13,107))
	{
		int count = gObjGetItemCountInEquipment(lpObj->m_Index,13,107,0);

		if(count != 0)
		{
			return false;
		}
	}
	else if(lpItem->m_Type >= ITEMGET(14,00))
	{
		return false;
	}

	if( lpItem->m_bPeriodItem && lpItem->m_bExpired )
	{
		return false;
	}

	return true;
}






BYTE gObjWerehouseRectCheck(int aIndex, int sx, int sy, int width, int height)
{
	int x,y;
	int blank = 0;

	if(sx + width > 8)
	{
		return -1;
	}

	if(sx + width < 0)
	{
		return -1;
	}

	if(sy + height > (15 + gObj[aIndex].pWarehouseExtend*15))
	{
		return -1;
	}

	if(sy + height < 0)
	{
		return -1;
	}

	int xx,yy;

	LPOBJ lpObj = &gObj[aIndex];

	for(y = 0; y < height; y ++)
	{

		yy = sy+y;

		for(x = 0; x < width; x++)
		{
			xx = sx + x;

			if(ExtentCheck(xx,yy,8,(15 + gObj[aIndex].pWarehouseExtend*15))==1)
			{
				if(*(BYTE*)(gObj[aIndex].pWarehouseMap+(sy+y)*8+(sx+x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				LogAdd("error : %s %d",__FILE__,__LINE__);
				return -1;
			}
		}
	}

	if(blank == 0)
	{
		return sx+sy*8;
	}
	return  -1;
}






BYTE gObjMapRectCheck(BYTE * lpMapBuf, int sx, int sy, int ex, int ey, int width, int height)
{
	int x,y;
	int blank = 0;

	if(sx + width > ex || sx + width < 0)
	{
		return -1;
	}

	if(sy + height > ey || sy + height < 0)
	{
		return -1;
	}

	int xx,yy;

	for(y = 0; y < height; y++)
	{
		yy = sy + y;

		for(x = 0; x < width; x++)
		{
			xx = sx + x;

			if(ExtentCheck(xx,yy,ex,ey)==1)
			{
				if(*(BYTE*)(lpMapBuf + (sy + y)*ex + (sx+x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				LogAdd("error : %s %d",__FILE__,__LINE__);
				return -1;
			}
		}
	}

	if(blank == 0)
	{
		return sy*ex+sx;
	}
	return  -1;

	return -1;
}






void gObjWarehouseItemBoxSet(int aIndex, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos)%8;
	int itemposy = (itempos)/8;

	int xx,yy;

	for(int y = 0; y < yl; y ++)
	{
		yy = itemposy + y;

		for(int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

			if(ExtentCheck(xx,yy,8,30)==1)
			{
				*(BYTE*)(gObj[aIndex].pWarehouseMap + (itemposy + y)*8+(itemposx + x)) = set_byte;
			}
			else
			{
				LogAdd("error : %d/%d  %s %d",xx,yy,__FILE__,__LINE__);
				return;
			}
		}
	}
}






void gObjWarehouseItemSet(int aIndex, int itempos, BYTE set_byte)
{
	int width;
	int height;
	if(itempos < 0)
	{
		return;
	}
	else if(itempos > (WAREHOUSE_SIZE-1))
	{
		return;
	}

	if(gObj[aIndex].pWarehouse[itempos].GetSize((int&)width,(int &)height)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return;
	}

	gObjWarehouseItemBoxSet(aIndex,itempos,width,height,set_byte);
}






void gObjChaosItemBoxSet(int aIndex, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos)%8;
	int itemposy = (itempos)/8;

	int xx,yy;

	for(int y = 0; y < yl; y ++)
	{
		yy = itemposy + y;

		for(int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

			if(ExtentCheck(xx,yy,8,4)==1)
			{
				*(BYTE*)(gObj[aIndex].pChaosBoxMap + (itemposy + y)*8+(itemposx + x)) = set_byte;
			}
			else
			{
				LogAdd("error : %d/%d  %s %d",xx,yy,__FILE__,__LINE__);
				return;
			}
		}
	}
}






void gObjChaosItemSet(int aIndex, int itempos, BYTE set_byte)
{
	int width;
	int height;
	if(itempos < 0)
	{
		return;
	}
	else if(itempos > (CHAOS_BOX_SIZE-1))
	{
		return;
	}

	if(gObj[aIndex].pChaosBox[itempos].GetSize((int&)width,(int &)height)==0)
	{
		LogAdd(lMsg.Get(527),__FILE__,__LINE__);
		return;
	}

	gObjChaosItemBoxSet(aIndex,itempos,width,height,set_byte);
}






BYTE gObjChaosBoxInsertItemPos(int aIndex, CItem item, int pos, int source)
{

	LPOBJ lpObj;
	BYTE TempMap[CHAOS_BOX_MAP_SIZE];

	int w,h,iwidth,iheight;

	int blank,useClass;

	if(pos < 0 || pos > (CHAOS_BOX_SIZE-1))
	{
		return -1;
	}

	useClass = 0;

	lpObj = &gObj[aIndex];

	if(lpObj->pChaosBox[pos].IsItem() == 1)
	{
		return -1;
	}

	if(item.IsItem() == 0)
	{
		return -1;
	}

	w = pos%8;
	h = pos/8;

	if(ExtentCheck(w,h,8,4) == 0)
	{
		return -1;
	}

	item.GetSize((int &)iwidth,(int &)iheight);

	if(source >= 0)
	{
		memcpy(TempMap,lpObj->pChaosBoxMap,CHAOS_BOX_MAP_SIZE);
		gObjChaosItemBoxSet(lpObj->m_Index,source,iwidth,iheight,255);
	}

	if(*(BYTE*)(lpObj->pChaosBoxMap + h * 8 + w) != 255)
	{
		if(source >= 0)
		{
			memcpy(lpObj->pChaosBoxMap,TempMap,CHAOS_BOX_MAP_SIZE);
		}
		return -1;
	}

	blank = gObjMapRectCheck(lpObj->pChaosBoxMap,w,h,8,4,iwidth,iheight);

	if(blank == 255)
	{
		if(source >= 0)
		{
			memcpy(lpObj->pChaosBoxMap,TempMap,CHAOS_BOX_MAP_SIZE);
		}
		return -1;
	}

	lpObj->pChaosBox[pos] = item;
	return pos;
}






BYTE gObjWarehouseInsertItemPos(int aIndex, CItem item, int pos, int source)
{
	LPOBJ lpObj;
	BYTE TempMap[WAREHOUSE_SIZE];

	int w,h,iwidth,iheight;

	int blank,useClass;

	if(pos < 0 || pos > (WAREHOUSE_SIZE-1))
	{
		return -1;
	}

	useClass = 0;

	lpObj = &gObj[aIndex];

	if(lpObj->pWarehouse[pos].IsItem() == 1)
	{
		return -1;
	}

	if(item.IsItem() == 0)
	{
		return -1;
	}

	w = pos%8;
	h = pos/8;

	if(ExtentCheck(w,h,8,15+(15*lpObj->pWarehouseExtend)) == 0)
	{
		return -1;
	}

	item.GetSize((int &)iwidth,(int &)iheight);

	if(source >= 0)
	{
		memcpy(TempMap,lpObj->pWarehouseMap,WAREHOUSE_SIZE);
		gObjWarehouseItemBoxSet(lpObj->m_Index,source,iwidth,iheight,255);
	}

	if(*(BYTE*)(lpObj->pWarehouseMap + h * 8 + w) != 255)
	{
		if(source >= 0)
		{
			memcpy(lpObj->pWarehouseMap,TempMap,WAREHOUSE_SIZE);
		}
		return -1;
	}

	blank = gObjWerehouseRectCheck(lpObj->m_Index,w,h,iwidth,iheight);

	if(blank == 255)
	{
		if(source >= 0)
		{
			memcpy(lpObj->pWarehouseMap,TempMap,WAREHOUSE_SIZE);
		}
		return -1;
	}

	lpObj->pWarehouse[pos] = item;
	return pos;
}



BYTE gObjInventoryInsertItemPos(int aIndex, CItem item, int pos, BOOL RequestCheck)
{
	if(pos < 0 || pos > INVENTORY_SIZE)
	{
		return -1;
	}

	LPOBJ lpObj;
	BYTE TempInventoryMap[INVENTORY_MAP_SIZE];

	int w,h,iwidth,iheight;

	int blank,useClass = 0;

	lpObj = &gObj[aIndex];

	if(lpObj->pInventory[pos].IsItem() == 1)
	{
		return -1;
	}

	if(item.IsItem() == 0)
	{
		return -1;
	}

	if(pos < INVETORY_WEAR_SIZE || pos == ELEMENT_SOCKET)
	{
		useClass = item.IsClass((char)lpObj->Class,lpObj->ChangeUP,lpObj->ChangeUP2);

		if(useClass == 0)
		{
			LogAdd(lMsg.Get(528));
			return -1;
		}

		if(RequestCheck != 0)
		{
			if(gObjIsItemPut(lpObj,&item,pos) == 0)
			{
				return -1;
			}
		}
	}
	else
	{
		w = (pos - 12)%8;
		h = (pos - 12)/8;
		item.GetSize((int &)iwidth,(int &)iheight);

#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
		int bResult = CheckOutOfInventory(aIndex, h, iheight);

		if( !bResult )
		{
			if( bResult == -1 )
			{
				return -2;
			}

			return -1;
		}

		if( InventoryExtentCheck(w,h,8,28) == 0 )
		{
			return -1;
		}
#else
		if(ExtentCheck(w,h,8,28)==0)
		{
			return -1;
		}
#endif
		memcpy(TempInventoryMap,lpObj->pInventoryMap,INVENTORY_MAP_SIZE);
		

		if(*(BYTE*)(lpObj->pInventoryMap + h * 8 + w) != 255 )
		{
			memcpy(lpObj->pInventoryMap,TempInventoryMap,INVENTORY_MAP_SIZE);
			return -1;
		}

		blank = gObjInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight);

		if(blank >= 254)
		{
			memcpy(lpObj->pInventoryMap,TempInventoryMap,INVENTORY_MAP_SIZE);
			return false;
		}
	}

	lpObj->pInventory[pos] = item;

	if(pos > 11)
	{
		gObjInventoryItemSet(aIndex,pos,1);
	}

	if(pos < 12)
	{
		CItem * lpItem = &lpObj->pInventory[pos];

		if(lpItem->m_Option1)
		{
			int result = gObjWeaponMagicAdd(lpObj,lpItem->m_Special[0],lpItem->m_Level);

			if(result >= 0)
			{
				GCMagicListOneSend(lpObj->m_Index,result,lpItem->m_Special[0],lpItem->m_Level,0,0);
			}
		}
	}

	return pos;
}






BOOL gObjInventorySearchSerialNumber(LPOBJ lpObj, DWORD serial)
{
	int s_num;
	int count = 0;
	int n;

	if(gItemSerialCheck == 0)
	{
		return true;
	}

	if(serial == 0)
	{
		return true;
	}

	for(n = 0; n < INVENTORY_SIZE; n++)
	{
		s_num = lpObj->pInventory[n].GetNumber();

		if(s_num != 0 && s_num == serial)
		{
			count++;
		}
	}

	if(count <= 1)
	{
		return true;
	}

	for(n = 0; n < INVENTORY_SIZE; n++)
	{
		s_num = lpObj->pInventory[n].GetNumber();

		if(s_num != 0 && s_num == serial)
		{
			LogAddTD(lMsg.Get(529),lpObj->AccountID,lpObj->Name,lpObj->pInventory[n].GetName(),n,s_num);
			GCServerMsgStringSend(lMsg.Get(1102),lpObj->m_Index,1);
			gObjUserKill(lpObj->m_Index);
		}
	}

	return false;
}






BOOL gObjWarehouseSearchSerialNumber(LPOBJ lpObj, DWORD sirial)
{
int s_num;
int count = 0;
int n;

	if(gItemSerialCheck == 0)
	{
		return true;
	}

	if(sirial == 0)
	{
		return true;
	}

	for(n = 0; n < WAREHOUSE_SIZE; n++)
	{
		s_num = lpObj->pWarehouse[n].GetNumber();

		if(s_num != 0 && s_num == sirial)
		{
			count++;
		}
	}

	if(count <= 1)
	{
		return true;
	}

	for(n = 0; n < WAREHOUSE_SIZE; n++)
	{
		s_num = lpObj->pWarehouse[n].GetNumber();

		if(s_num != 0 && s_num == sirial)
		{
			LogAddTD(lMsg.Get(530),lpObj->AccountID,lpObj->Name,lpObj->pWarehouse[n].GetName(),n,s_num);
			GCServerMsgStringSend(lMsg.Get(1102),lpObj->m_Index,1);
			gObjUserKill(lpObj->m_Index);
		}
	}

	return false;
}






BYTE gObjInventoryMoveItem(int aIndex, unsigned char source, unsigned char target, int& durSsend, int& durTsend, unsigned char sFlag, unsigned char tFlag, unsigned char* siteminfo)
{
	LPOBJ lpObj;
	BYTE TempInventoryMap[INVENTORY_MAP_SIZE];
	int w,h;
	int iwidth,iheight;
	int blank;
	int s_num;

	CItem * sitem;
	CItem * titem;

	int bPersonalShopTrans = 0;
	int bSourceIsPShop = 0;

	durSsend = 0;
	durTsend = 0;

	lpObj = &gObj[aIndex];

	int useClass = 0;
	__try
	{
		if(sFlag == 2 || tFlag == 2)
		{
			if(lpObj->m_IfState.type != 6)
			{
				LogAdd(lMsg.Get(531),lpObj->AccountID,lpObj->Name);
				return -1;
			}

			if(lpObj->m_IfState.state == 0)
			{
				LogAdd(lMsg.Get(531),lpObj->AccountID,lpObj->Name);
				return -1;
			}
		}

		switch(sFlag)
		{
		case 0:
			if( (source < 0 || source > (MAIN_INVENTORY_SIZE-1)) && source != ELEMENT_SOCKET)
			{
				LogAdd("error-L1 : %s %d",__FILE__,__LINE__);
				return -1;
			}

			if(lpObj->pInventory[source].IsItem() == 0)
			{
				LogAdd(lMsg.Get(532),lpObj->AccountID,lpObj->Name,__LINE__);
				return -1;
			}

			sitem = &lpObj->pInventory[source];

			if(lpObj->MapNumber == MAP_INDEX_ICARUS)
			{
				if(source == 8)
				{
					if(sitem->m_Type == ITEMGET(13,3))
					{
						if(lpObj->pInventory[7].IsItem() == 0)
						{
							return -1;
						}
					}
					else if(sitem->m_Type == ITEMGET(13,37))
					{
						if(lpObj->pInventory[7].IsItem() == 0)
						{
							return -1;
						}
					}
					else if(sitem->m_Type == ITEMGET(13,4))
					{
						if(lpObj->pInventory[7].IsItem() == 0)
						{
							return -1;
						}
					}
				}
				else if(source == 7)
				{
					if(lpObj->pInventory[8].m_Type != ITEMGET(13,3) &&
					   lpObj->pInventory[8].m_Type != ITEMGET(13,37) &&
					   lpObj->pInventory[8].m_Type != ITEMGET(13,4) )
					{
						return -1;
					}
				}
			}

			break;
		case 2:
			if(source < 0 || source > (WAREHOUSE_SIZE-1))
			{
				LogAdd("error-L1 : %s %d",__FILE__,__LINE__);
				return -1;
			}

			if(lpObj->pWarehouse[source].IsItem() == 0)
			{
				LogAdd(lMsg.Get(532),lpObj->AccountID,lpObj->Name,__LINE__);
				return -1;
			}

			sitem = &lpObj->pWarehouse[source];
			break;
		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			if(source < 0 || source > (CHAOS_BOX_SIZE -1))
			{
				LogAdd("error-L1 : %s %d",__FILE__,__LINE__);
				return -1;
			}

			if(lpObj->pChaosBox[source].IsItem() == 0)
			{
				LogAdd(lMsg.Get(532),lpObj->AccountID,lpObj->Name,__LINE__);
				return -1;
			}

			sitem = &lpObj->pChaosBox[source];
			break;
		case 4:
			if(gObj[aIndex].m_bPShopOpen == 1)
			{
				return -1;
			}

			EnterCriticalSection(&gObj[aIndex].m_critPShopTrade);

			if(gObj[aIndex].m_bPShopTransaction == 1)
			{
				LogAddTD("[PShop] [%s][%s] PShop Item Move Request Failed : Already Trade With Other",gObj[aIndex].AccountID,gObj[aIndex].Name);
				LeaveCriticalSection(&lpObj->m_critPShopTrade);
				return -1;
			}

			lpObj->m_bPShopTransaction = 1;
			bPersonalShopTrans = 1;
			bSourceIsPShop = 1;

			LeaveCriticalSection(&lpObj->m_critPShopTrade);

			if(source < MAIN_INVENTORY_SIZE || source > (INVENTORY_SIZE - 1))
			{
				LogAdd("error-L1 : %s %d",__FILE__,__LINE__);
				return -1;
			}

			if(lpObj->pInventory[source].IsItem() == 0)
			{
				LogAdd(lMsg.Get(532),lpObj->AccountID,lpObj->Name,__LINE__);
				return -1;
			}

			sitem = &lpObj->pInventory[source];

			break;
		default: return -1;
		}

		BYTE level;
		BYTE op1;
		BYTE op2;
		BYTE op3;
		BYTE dur;
		int type;

		BufferItemtoConvert3(siteminfo,(int &)type,(BYTE &)level,(BYTE &)op1,(BYTE &)op2,(BYTE &)op3,(BYTE &)dur);

		if(sitem->m_Type != type || sitem->m_Level != level || sitem->m_Option1 != op1 || sitem->m_Option2 != op2 || sitem->m_Option3 != op3)
		{
			LogAddC(2,lMsg.Get(533),type,level,op1,op2,op3,sitem->m_Type,sitem->m_Level,sitem->m_Option1,sitem->m_Option2,sitem->m_Option3);
			return -1;
		}

		switch(tFlag)
		{
		case 0:
			if((target < 0 || target > (MAIN_INVENTORY_SIZE - 1)) && target != ELEMENT_SOCKET)
			{
				LogAdd("error-L1 : %s %d",__FILE__,__LINE__);
				return -1;
			}

			titem = &lpObj->pInventory[target];
			break;
		case 2:
			if(target < 0 || target > (WAREHOUSE_SIZE - 1))
			{
				LogAdd("error-L1 : %s %d",__FILE__,__LINE__);
				return -1;
			}

			if( lpObj->pInventory[source].m_bPeriodItem )
			{
				return -1;
			}

			if( lpObj->pInventory[source].m_bLuckySet )
			{
				return -1;
			}

			titem = &lpObj->pWarehouse[target];
			break;
		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			if(target < 0 || target > (CHAOS_BOX_SIZE -1))
			{
				LogAdd("error-L1 : %s %d",__FILE__,__LINE__);
				return -1;
			}

			titem = &lpObj->pChaosBox[target];

			if(sFlag == 0)
			{
				if( sitem->m_bLuckySet )
				{
					if( tFlag == 16 || tFlag == 8 )
					{

					}
					else
					{
						return -1;
					}
				}

				if(tFlag == 5)
				{
					if(sitem->m_Type == ITEMGET(12,15)	||
						sitem->m_Type == ITEMGET(14,13) ||
						sitem->m_Type == ITEMGET(14,14) ||
						sitem->m_Type == ITEMGET(14,22) ||
						sitem->m_Type == ITEMGET(13,31) ||
						sitem->m_Type == ITEMGET(14,53) ||
						sitem->m_Type == ITEMGET(13,83) ||
						sitem->m_Type == ITEMGET(13,84) ||
						sitem->m_Type == ITEMGET(13,85) ||
						sitem->m_Type == ITEMGET(13,86) ||
						sitem->m_Type == ITEMGET(13,87) ||
						sitem->m_Type == ITEMGET(13,88) ||
						sitem->m_Type == ITEMGET(13,89) ||
						sitem->m_Type == ITEMGET(13,90) ||
						sitem->m_Type == ITEMGET(13,91) ||
						sitem->m_Type == ITEMGET(13,92) ||
						sitem->m_Type == ITEMGET(14, 189) ||
						sitem->m_Type == ITEMGET(14, 190) ||
						sitem->m_Type == ITEMGET(14, 121) ||
						sitem->m_Type == ITEMGET(14, 122) ||
						sitem->m_Type == ITEMGET(14, 112) ||
						sitem->m_Type == ITEMGET(14, 113) ||
						sitem->m_Type == ITEMGET(14, 210) ||
						sitem->m_Type == ITEMGET(14, 211) ||
						sitem->m_Type == ITEMGET(14, 212)
#ifdef __NOVUS__
						|| sitem->m_Type == ITEMGET(12, 138)
						|| sitem->m_Type >= ITEMGET(14, 58) && sitem->m_Type <= ITEMGET(14, 62)
						|| sitem->m_Type >= ITEMGET(14, 145) && sitem->m_Type <= ITEMGET(14, 150)
#endif
						)
					{

					}
					else
					{
						return -1;
					}
				}
				else if(tFlag == 6)
				{
					if(g_kJewelOfHarmonySystem.IsJewelOfHarmonyOriginal(sitem->m_Type) == 0)
					{
						return -1;
					}
				}
				else if(tFlag == 7)
				{
					if(g_kJewelOfHarmonySystem.IsEnableToMakeSmeltingStoneItem(sitem) == 0)
					{
						GCServerMsgStringSend(lMsg.Get(3377),lpObj->m_Index,1);
						return -1;
					}
				}
				else if(tFlag == 8)
				{
					if(g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(sitem) == 0)
					{
						return -1;
					}
				}
				else if(tFlag == 11)
				{

				}
				else if(tFlag == 12)
				{

				}
				else if(tFlag == 13)
				{

				}
				else if(tFlag == 14)
				{

				}
				else if( tFlag == 15 )
				{
					if( sitem->m_Type >= ITEMGET(13,135) && sitem->m_Type <= ITEMGET(13,144) )
					{

					}
					else
					{
						return -1;
					}
				}
				else if( tFlag == 16 )
				{
					if( sitem->m_bLuckySet == TRUE )
					{

					}
					else
					{
						return -1;
					}
				}
				else if(tFlag == 9)
				{
					if(sitem->m_Type == ITEMGET(14,54) ||
					   sitem->m_Type == ITEMGET(14,92) ||
					   sitem->m_Type == ITEMGET(14,93) ||
					   sitem->m_Type == ITEMGET(14,95) )
					{

					}
					else
					{
						return -1;
					}
				}
				else if(tFlag == 10)
				{
					if(sitem->m_Type != ITEMGET(14,88) &&
						sitem->m_Type != ITEMGET(14,89) &&
						sitem->m_Type != ITEMGET(14,90) )
					{
						return -1;
					}
				}
				else
				{
					if(sitem->m_Level < 4 && sitem->m_Option3*4 < 4)
					{
						if(sitem->m_Type == ITEMGET(12,15) || sitem->m_Type == ITEMGET(14,13) || sitem->m_Type == ITEMGET(14,14) || sitem->m_Type == ITEMGET(14,22) || sitem->m_Type == ITEMGET(13,14))
						{
							
						}
						else if( sitem->m_Type >= ITEMGET(12,0) && sitem->m_Type <= ITEMGET(12,2) )
						{

						}
						else if( (sitem->m_Type >= ITEMGET(12,3) && sitem->m_Type <= ITEMGET(12,6)) ||
								 sitem->m_Type == ITEMGET(12,42) ||
								 sitem->m_Type == ITEMGET(12,49) ||
								 sitem->m_Type == ITEMGET(13,30) )
						{

						}
						else if( sitem->m_Type == ITEMGET(12,41) )
						{

						}
						else if(sitem->m_Type == ITEMGET(14,17) || sitem->m_Type == ITEMGET(14,18))
						{

						}
						else if(sitem->m_Type == ITEMGET(13,2) && sitem->m_Durability == 255.0f)
						{

						}
						else if(sitem->m_Type == ITEMGET(13,16) || sitem->m_Type == ITEMGET(13,17))
						{

						}
						else if(sitem->m_Type == ITEMGET(12,30) || sitem->m_Type == ITEMGET(12,31))
						{

						}
						else if(sitem->m_Type == ITEMGET(14,31))
						{

						}
						else if(sitem->m_Type == ITEMGET(12,26))
						{

						}
						else if(
							sitem->m_Type == ITEMGET(13,32) ||
							sitem->m_Type == ITEMGET(13,33) ||
							sitem->m_Type == ITEMGET(13,34) ||
							sitem->m_Type == ITEMGET(13,35) ||
							sitem->m_Type == ITEMGET(13,36) ||
							sitem->m_Type == ITEMGET(13,37) ||
							sitem->m_Type == ITEMGET(14,16))
						{

						}
						else if(sitem->m_Type == ITEMGET(14,3) || sitem->m_Type == ITEMGET(14,38) || sitem->m_Type == ITEMGET(14,39))
						{

						}
						else if(g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(sitem->m_Type) == 1)
						{

						}
						else if(sitem->m_Type == ITEMGET(14,31))
						{

						}
						else if(sitem->m_Type == ITEMGET(14,53))
						{

						}
						else if(sitem->m_Type == ITEMGET(14,96))
						{

						}
						else if(sitem->m_Type == ITEMGET(13,49) || sitem->m_Type == ITEMGET(13,50))
						{

						}
						else if( g_ChaosBox.IsCondorItem(sitem->m_Type) == TRUE )
						{

						}
						else if( 
							sitem->m_Type == ITEMGET(13,83) ||
							sitem->m_Type == ITEMGET(13,84) ||
							sitem->m_Type == ITEMGET(13,85) ||
							sitem->m_Type == ITEMGET(13,86) ||
							sitem->m_Type == ITEMGET(13,87) ||
							sitem->m_Type == ITEMGET(13,88) ||
							sitem->m_Type == ITEMGET(13,89) ||
							sitem->m_Type == ITEMGET(13,90) ||
							sitem->m_Type == ITEMGET(13,91) ||
							sitem->m_Type == ITEMGET(13,92) )
						{

						}
						else if( sitem->m_Type == ITEMGET(14,103) ||
							sitem->m_Type == ITEMGET(14,104) ||
							sitem->m_Type == ITEMGET(14,105) ||
							sitem->m_Type == ITEMGET(14,106) ||
							sitem->m_Type == ITEMGET(14,107) ||
							sitem->m_Type == ITEMGET(14,108) ||
							sitem->m_Type == ITEMGET(14, 121) ||
							sitem->m_Type == ITEMGET(14, 122) ||
							sitem->m_Type == ITEMGET(14, 112) ||
							sitem->m_Type == ITEMGET(14, 113) ||
							sitem->m_Type == ITEMGET(14, 210) ||
							sitem->m_Type == ITEMGET(14, 211) ||
							sitem->m_Type == ITEMGET(14, 212)
#ifdef __NOVUS__
							|| sitem->m_Type == ITEMGET(12, 138)
							|| sitem->m_Type >= ITEMGET(14, 58) && sitem->m_Type <= ITEMGET(14, 62)
							|| sitem->m_Type >= ITEMGET(14, 145) && sitem->m_Type <= ITEMGET(14, 150)
#endif
							)
						{

						}
						else if( sitem->m_Type >= ITEMGET(14,176) && sitem->m_Type <= ITEMGET(14,179) )
						{

						}
						else if( sitem->m_Type >= ITEMGET(14,180) && sitem->m_Type <= ITEMGET(14,190) )
						{

						}
						else if( sitem->m_Type >= ITEMGET(14,191) && sitem->m_Type <= ITEMGET(14,202) )
						{

						}
						else if( sitem->m_Type == ITEMGET(12,148) || sitem->m_Type == ITEMGET(12,149) ||
								 sitem->m_Type == ITEMGET(12,145) || sitem->m_Type == ITEMGET(12,147) ||
								 sitem->m_Type == ITEMGET(12,150) || sitem->m_Type == ITEMGET(12,151) ||
								 (sitem->m_Type >= ITEMGET(12,200) && sitem->m_Type <= ITEMGET(12,214) ) ||
								 sitem->m_Type == ITEMGET(12,221) ||
								 sitem->m_Type == ITEMGET(12,231) ||
								 sitem->m_Type == ITEMGET(12,241) ||
								 sitem->m_Type == ITEMGET(12,251) ||
								 sitem->m_Type == ITEMGET(12,261) )
						{

						}
						else
						{
							return -1;
						}
					}
					else if(sitem->m_Type == ITEMGET(14,11))
					{
						return -1;
					}
				}
			}
			break;
		case 4:
			if(gDoPShopOpen == 0)
			{
				return -1;
			}

			if(gObj[aIndex].m_bPShopOpen == 1)
			{
				return -1;
			}

#if (__CUSTOM__ == 1)
			if( sitem->m_bLOCKED == TRUE )
			{
				return -1;
			}
#endif

			if(sitem->m_Type == ITEMGET(14,11) && sitem->m_Level == 13)
			{
				return -1;
			}

			if(g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(sitem) == 1)
			{
				GCServerMsgStringSend(lMsg.Get(3367),lpObj->m_Index,1);
				return -1;
			}

			if(IsCashItem(sitem->m_Type) == 1)
			{
				switch( sitem->m_Type )
				{
				case ITEMGET(13,83):
				case ITEMGET(13,84):
				case ITEMGET(13,85):
				case ITEMGET(13,86):
				case ITEMGET(13,87):
				case ITEMGET(13,88):
				case ITEMGET(13,89):
				case ITEMGET(13,90):
				case ITEMGET(13,91):
				case ITEMGET(13,92):
					break;
				default:
					return -1;
				}
			}

			if(IsCashSpecialItem(sitem->m_Type) == 1)
			{
				return -1;
			}

			if(sitem->m_Type == ITEMGET(13,38))
			{
				GCServerMsgStringSend(lMsg.Get(MSGGET(13,62)),lpObj->m_Index,1);
				return -1;
			}

			if(sitem->m_Type == ITEMGET(13,39))
			{
				return -1;
			}

			if(sitem->m_Type == ITEMGET(13,20) &&
				(sitem->m_Level == 1 || sitem->m_Level == 2 || sitem->m_Level == 0) )
			{
				return -1;
			}

			if(sitem->m_QuestItem )
			{
				return -1;
			}

			if( lpObj->pInventory[source].m_Type == ITEMGET(13,51) &&
				lpObj->pInventory[source].m_Durability == 0.0f)
			{
				return -1;
			}

			if( lpObj->pInventory[source].m_Type == ITEMGET(13,107) )
			{
				return -1;
			}

			if( g_ElementalSystem.IsPentagram(sitem->m_Type) )
			{
				if( sitem->m_Durability < 1.0f )
				{
					return -1;
				}
			}

			EnterCriticalSection(&gObj[aIndex].m_critPShopTrade);

			if(gObj[aIndex].m_bPShopTransaction == 1)
			{
				if(bSourceIsPShop == 1)
				{
					lpObj->m_bPShopTransaction = 1;
					bPersonalShopTrans = 1;
				}
				else
				{
					LogAddTD("[PShop] [%s][%s] PShop Item Move Request Failed : Already Trade With Other",gObj[aIndex].AccountID,gObj[aIndex].Name);
					LeaveCriticalSection(&lpObj->m_critPShopTrade);
					return -1;
				}
			}
			else
			{
				lpObj->m_bPShopTransaction = 1;
				bPersonalShopTrans = 1;
			}

			LeaveCriticalSection(&lpObj->m_critPShopTrade);

			if(target < MAIN_INVENTORY_SIZE || target > (INVENTORY_SIZE - 1))
			{
				LogAdd("error-L1 : %s %d",__FILE__,__LINE__);
				return -1;
			}

			titem = &lpObj->pInventory[target];

			break;
		default: return -1;
		}

		s_num = sitem->GetNumber();

		if(gObjCheckSerial0ItemList(sitem))
		{
			MsgOutput(lpObj->m_Index,lMsg.Get(3354));
			LogAddTD("[ANTI-HACK][Serial 0 Item] [MoveItem] (%s)(%s) Item(%s) Pos(%d)",lpObj->AccountID,lpObj->Name,sitem->GetName(),source);
			return -1;
		}

		if( !sitem->m_bPeriodItem )
		{
			if(gObjInventorySearchSerialNumber(lpObj,s_num) == 0)
			{
				return -1;
			}

			if(gObjWarehouseSearchSerialNumber(lpObj,s_num) == 0)
			{
				return -1;
			}
		}

		if(titem->IsItem() == 1)
		{
			int max_count = 0;

			if(sitem->m_Type == ITEMGET(13,32) && titem->m_Durability < 20.0f)
			{
				max_count = 20;
			}

			if(sitem->m_Type == ITEMGET(13,33) && titem->m_Durability < 20.0f)
			{
				max_count = 20;
			}

			if(sitem->m_Type == ITEMGET(13,34) && titem->m_Durability < 10.0f)
			{
				max_count = 10;
			}

			if(sitem->m_Type == ITEMGET(14,35) || sitem->m_Type == ITEMGET(14,36) || sitem->m_Type == ITEMGET(14,37))
			{
				if(titem->m_Durability < 25.0f )
				{
					max_count = 25;
				}
			}

#if (GS_CASTLE == 1)
			if(sitem->m_Type == ITEMGET(14,63) || sitem->m_Type == ITEMGET(14,64))
			{
				if(titem->m_Durability < 100.0f)
				{
					max_count = 100;
				}
			}
#endif

			if(sitem->m_Type >= ITEMGET(14,46) && sitem->m_Type <= ITEMGET(14,50))
			{
				if(titem->m_Durability < 3.0f)
				{
					max_count = 3;
				}
			}

			if(sitem->m_Type == ITEMGET(14,29))
			{
				if(sitem->m_Level == titem->m_Level)
				{
					max_count = 5;
				}
			}

			if( sitem->m_Type == ITEMGET(14,110) || sitem->m_Type == ITEMGET(14,101) )
			{
				if( titem->m_Durability < 5.0f )
				{
					max_count = 5;
				}
			}
		   
			if(sitem->m_Type == ITEMGET(14,7))
			{
				if(titem->m_Durability < 250.0f)
				{
					max_count = 250;
				}
			}
			else if(sitem->m_Type >= ITEMGET(14,0) && sitem->m_Type <= ITEMGET(14,8) || sitem->m_Type >= ITEMGET(14,38) && sitem->m_Type <= ITEMGET(14,40))
			{
				if(titem->m_Durability < 25.0f)
				{
					max_count = 25;
				}
			}
			else if(sitem->m_Type == ITEMGET(4,15) || sitem->m_Type == ITEMGET(4,7))
			{
				if(titem->m_Durability < 255.0f)
				{
					max_count = 255;
				}
			}
			else if( (sitem->m_Type == ITEMGET(14,70) || sitem->m_Type == ITEMGET(14,71) || sitem->m_Type == ITEMGET(14,94) )
				&& titem->m_Durability < 50.0f)
			{
				max_count = 50;
			}
			else if(sitem->m_Type == ITEMGET(14,88) && titem->m_Durability < 10.0f)
			{
				max_count = 10;
			}
			else if(sitem->m_Type == ITEMGET(14,89) && titem->m_Durability < 30.0f)
			{
				max_count = 30;
			}
			else if(sitem->m_Type == ITEMGET(14,90) && titem->m_Durability < 255.0f)
			{
				max_count = 255;
			}
			else if(sitem->m_Type == ITEMGET(14,85) && titem->m_Durability < 3.0f)
			{
				max_count = 3;
			}
			else if(sitem->m_Type == ITEMGET(14,86) && titem->m_Durability < 3.0f)
			{
				max_count = 3;
			}
			else if(sitem->m_Type == ITEMGET(14,87) && titem->m_Durability < 3.0f)
			{
				max_count = 3;
			}
			else if(sitem->m_Type == ITEMGET(12,144) && titem->m_Durability < 5.0f)
			{
				if( sitem->m_SocketBonus == titem->m_SocketBonus )
					max_count = 5;
			}
			else if(sitem->m_Type == ITEMGET(12,146) && titem->m_Durability < 5.0f)
			{
				max_count = 5;
			}
			else if(sitem->m_Type == ITEMGET(13, 145) && titem->m_Durability < 5.0f)
			{
				max_count = 5;
			}

			if(max_count != 0)
			{
				if(sFlag != 0)
				{
					return -1;
				}

				if(titem->m_Type == sitem->m_Type && titem->m_Level == sitem->m_Level)
				{
					int t_dur = (int)titem->m_Durability;

					if(t_dur < max_count)
					{
						int dif_dur = max_count - int(titem->m_Durability);

						if(dif_dur > sitem->m_Durability)
						{
							dif_dur = (int)sitem->m_Durability;
						}

						titem->m_Durability += dif_dur;
						sitem->m_Durability -= dif_dur;

						if(titem->m_Type == ITEMGET(14,29) && max_count <= titem->m_Durability)
						{
							titem->m_Durability -= max_count;

							if(titem->m_Durability == 0)
							{
								gObjInventoryItemSet(aIndex,target,(BYTE)-1);
								gObj[aIndex].pInventory[target].Clear();
								GCInventoryItemDeleteSend(aIndex,target,1);
								durTsend = 0;
							}
							else
							{
								durTsend = 1;
							}

							ItemSerialCreateSend(aIndex,235,gObj[aIndex].X,gObj[aIndex].Y,ItemGetNumberMake(14,28),sitem->m_Level,0,0,0,0,aIndex,0,0);
						}
						else if(titem->m_Type == ITEMGET(14,110) && max_count <= titem->m_Durability )
						{
							titem->m_Durability -= max_count;

							if(titem->m_Durability == 0)
							{
								gObjInventoryItemSet(aIndex,target,(BYTE)-1);
								gObj[aIndex].pInventory[target].Clear();
								GCInventoryItemDeleteSend(aIndex,target,1);
								durTsend = 0;
							}
							else
							{
								durTsend = 1;
							}

							ItemSerialCreateSend(aIndex,235,gObj[aIndex].X,gObj[aIndex].Y,ItemGetNumberMake(14,111),0,0,0,0,0,aIndex,0,0);
						}
						else if(titem->m_Type == ITEMGET(14,101) && max_count <= titem->m_Durability )
						{
							titem->m_Durability -= max_count;

							if(titem->m_Durability == 0)
							{
								gObjInventoryItemSet(aIndex,target,(BYTE)-1);
								gObj[aIndex].pInventory[target].Clear();
								GCInventoryItemDeleteSend(aIndex,target,1);
								durTsend = 0;
							}
							else
							{
								durTsend = 1;
							}

							ItemSerialCreateSend(aIndex,235,gObj[aIndex].X,gObj[aIndex].Y,ItemGetNumberMake(14,102),0,0,0,0,0,aIndex,0,0);
						}
						else if(titem->m_Type == ITEMGET(12,144) && max_count <= titem->m_Durability )
						{
							BYTE SocketBonus = titem->m_SocketBonus;
							BYTE SocketOptions[5] = {-1};
							titem->m_Durability -= max_count;

							if(titem->m_Durability == 0)
							{
								gObjInventoryItemSet(aIndex,target,(BYTE)-1);
								gObj[aIndex].pInventory[target].Clear();
								GCInventoryItemDeleteSend(aIndex,target,1);
								durTsend = 0;
							}
							else
							{
								durTsend = 1;
							}

							ItemSerialCreateSend(aIndex,235,gObj[aIndex].X,gObj[aIndex].Y,ITEMGET(12,148),0,0,0,0,0,aIndex,0,0,SocketBonus,SocketOptions);
						}
						else if(titem->m_Type == ITEMGET(12,146) && max_count <= titem->m_Durability )
						{
							titem->m_Durability -= max_count;

							if(titem->m_Durability == 0)
							{
								gObjInventoryItemSet(aIndex,target,(BYTE)-1);
								gObj[aIndex].pInventory[target].Clear();
								GCInventoryItemDeleteSend(aIndex,target,1);
								durTsend = 0;
							}
							else
							{
								durTsend = 1;
							}

							ItemSerialCreateSend(aIndex,235,gObj[aIndex].X,gObj[aIndex].Y,ITEMGET(12,149),0,0,0,0,0,aIndex,0,0);
						}
						else if(titem->m_Type == ITEMGET(13, 145) && max_count <= titem->m_Durability )
						{
							titem->m_Durability -= max_count;

							if(titem->m_Durability == 0)
							{
								gObjInventoryItemSet(aIndex,target,(BYTE)-1);
								gObj[aIndex].pInventory[target].Clear();
								GCInventoryItemDeleteSend(aIndex,target,1);
								durTsend = 0;
							}
							else
							{
								durTsend = 1;
							}

							ItemSerialCreateSend(aIndex,235,gObj[aIndex].X,gObj[aIndex].Y,ITEMGET(13, 146),0,0,0,0,0,aIndex,0,0);
						}

						if(sitem->m_Durability > 0)
						{
							durSsend = 1;
							if( sitem->m_Type != ITEMGET(14, 29) &&
								sitem->m_Type != ITEMGET(14, 110) && 
								sitem->m_Type != ITEMGET(14, 101) &&
								sitem->m_Type != ITEMGET(13, 145) )
							{
								durTsend = 1;
							}
						}
						else
						{
							switch(sFlag)
							{
							case 0:
								gObjInventoryItemSet(lpObj->m_Index,source,-1);
								sitem->Clear();
								GCInventoryItemDeleteSend(lpObj->m_Index,source,0);
								durTsend = 1;
							default:	return 0;
							}
						}
					}
				}
			}
			return -1;
		}

		switch(tFlag)
		{
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			if( lpObj->pInventory[source].m_Type == ITEMGET(13,42) )
			{
				return -1;
			}

			if( lpObj->pInventory[source].m_Type == ITEMGET(14,64) )
			{
				return -1;
			}

			break;
		case 0:
			if( IT_MAP_RANGE(lpObj->MapNumber) )
			{
				if( lpObj->pInventory[source].m_Type == ITEMGET(14,64) )
				{
					g_IllusionTemple.PickupRelic(lpObj->MapNumber,aIndex,target);
				}
			}
			break;
		}

		switch(tFlag)
		{
		case 0:
		case 4:
			if(target < INVETORY_WEAR_SIZE || target == ELEMENT_SOCKET )
			{
				if(lpObj->MapNumber == MAP_INDEX_ICARUS || lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS)
				{
					if( target == 10 && IsTransformRing(lpObj->pInventory[source].m_Type) )
					{
						return -1;
					}
					else if( target == 11 && IsTransformRing(lpObj->pInventory[source].m_Type) )
					{
						return -1;
					}
				}

				useClass = sitem->IsClass((char)lpObj->Class,lpObj->ChangeUP,lpObj->ChangeUP2);

				if(useClass == 0)
				{
					return -1;
				}

				if(gObjIsItemPut(lpObj,sitem,target) == 0)
				{
					return -1;
				}
			}
			else 
			{
				if(target >= MAIN_INVENTORY_SIZE)
				{
					if(lpObj->pInventory[source].m_Type == ITEMGET(12,26))
					{
						if(lpObj->pInventory[source].m_Level == 1 ||
							lpObj->pInventory[source].m_Level == 2 ||
							lpObj->pInventory[source].m_Level == 3 ||
							lpObj->pInventory[source].m_Level == 4 ||
							lpObj->pInventory[source].m_Level == 5)
						{
							return -1;
						}
					}	
				}

				w = (target - INVETORY_WEAR_SIZE)%8;
				h = (target - INVETORY_WEAR_SIZE)/8;

				if(ExtentCheck(w,h,8,28) == 0)
				{
					return -1;
				}

				sitem->GetSize((int &)iwidth,(int &)iheight);
				memcpy(TempInventoryMap,lpObj->pInventoryMap,INVENTORY_MAP_SIZE);

				switch(sFlag)
				{
				case 0:
				case 4:
					if(source > (INVETORY_WEAR_SIZE - 1) && source != ELEMENT_SOCKET)
					{
						gObjInventoryItemBoxSet(lpObj->m_Index,source,iwidth,iheight,255);
					}
					break;
				default: break;
				}
			
				if(*(BYTE*)(gObj[aIndex].pInventoryMap + h * 8 + w) != 255)
				{
					memcpy(lpObj->pInventoryMap,TempInventoryMap,INVENTORY_MAP_SIZE);
					return -1;
				}
	
				blank = gObjInventoryRectCheck(lpObj->m_Index,w,h,iwidth,iheight);
	
				if(blank >= 254)
				{
					memcpy(lpObj->pInventoryMap,TempInventoryMap,INVENTORY_MAP_SIZE);
					return -1;
				}
			}

			s_num = sitem->GetNumber();
			titem = sitem;

				switch(sFlag)
				{
				case 0:
				case 4:
					{
						lpObj->pInventory[target] = lpObj->pInventory[source];
						int bChangeEquipment = FALSE;

						gObjSetInventoryEquipment(lpObj, target);	//OK

						if( ((source == 11 || source == 10) && sitem->m_Type == ITEMGET(13,20) && sitem->m_Level == 3) ||
							((target == 11 || target == 10) && titem->m_Type == ITEMGET(13,20) && titem->m_Level == 3) )
						{
							bChangeEquipment = TRUE;
						}

						gObjInventoryDeleteItem(aIndex,source);

						if( bChangeEquipment != FALSE )
						{
							GCEquipmentSend(aIndex);
						}
					}
					break;
				case 2:
					lpObj->pInventory[target] = lpObj->pWarehouse[source];
					gObjWarehouseDeleteItem(aIndex,source);
					g_ElementalSystem.MoveErtel(lpObj,&lpObj->pInventory[target],ERTEL_INVENTORY,siteminfo);
					break;
				case 3:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
					lpObj->pInventory[target] = lpObj->pChaosBox[source];
					gObjChaosBoxDeleteItem(aIndex,source);
					break;
				default : break;
				}

				if(target > (INVETORY_WEAR_SIZE - 1) && target != ELEMENT_SOCKET)
				{
					gObjInventoryItemSet(aIndex,target,1);
				}

				if(target < INVETORY_WEAR_SIZE || target==ELEMENT_SOCKET)
				{
					CItem * lpItem = &lpObj->pInventory[target];
					if(lpItem->m_Option1)
					{
						int s_pos = gObjWeaponMagicAdd(&gObj[aIndex],lpItem->m_Special[0],lpItem->m_Level);
						if(s_pos >= 0)
						{
							GCMagicListOneSend(aIndex,s_pos,lpItem->m_Special[0],lpItem->m_Level,0,0);
						}
					}
				}
			break;
		case 2:
			BYTE res_1;
				switch(sFlag)
				{
				case 0:
					if(lpObj->pInventory[source].m_Type == ITEMGET(13,20))
					{
						if(lpObj->pInventory[source].m_Level == 0 ||
							lpObj->pInventory[source].m_Level == 1 ||
							lpObj->pInventory[source].m_Level == 2 )
						{
							return -1;
						}
					}

					if(lpObj->pInventory[source].m_Type == ITEMGET(13,107))
					{
						return -1;
					}

					if(lpObj->pInventory[source].m_Type == ITEMGET(14,11))
					{
						if(lpObj->pInventory[source].m_Level == 13)
						{
							return -1;
						}
					}

					if(lpObj->pInventory[source].m_Type == ITEMGET(12,26))
					{
						if(lpObj->pInventory[source].m_Level == 1 ||
							lpObj->pInventory[source].m_Level == 2 ||
							lpObj->pInventory[source].m_Level == 3 ||
							lpObj->pInventory[source].m_Level == 4 ||
							lpObj->pInventory[source].m_Level == 5)
						{
							return -1;
						}
					}

					if(lpObj->pInventory[source].m_Type == ITEMGET(13,39))
					{
						return -1;
					}

					if(lpObj->pInventory[source].m_Type == ITEMGET(13,42))
					{
						return -1;
					}

					if(lpObj->pInventory[source].m_QuestItem )
					{
						return -1;
					}

					res_1 = gObjWarehouseInsertItemPos(aIndex,lpObj->pInventory[source],target,-1);
					break;
				case 2:
					res_1 = gObjWarehouseInsertItemPos(aIndex,lpObj->pWarehouse[source],target,source);
					break;
				default : break;
				}

				if(res_1 == 255)
				{
					return -1;
				}

				switch(sFlag)
				{
				case 0:
					lpObj->pWarehouse[target] = lpObj->pInventory[source];
					gObjInventoryDeleteItem(aIndex,source);
					g_ElementalSystem.MoveErtel(lpObj,&lpObj->pWarehouse[target],ERTEL_WAREHOUSE,siteminfo);
					break;
				case 2:
					lpObj->pWarehouse[target] = lpObj->pWarehouse[source];
					gObjWarehouseDeleteItem(aIndex,source);
					break;
				default : break;
				}

				gObjWarehouseItemSet(aIndex,target,1);

				if(sFlag == 0)
				{
					if(source < INVETORY_WEAR_SIZE || source == ELEMENT_SOCKET )
					{
						if(source == 10 || source == 11)
						{
							if( IsTransformRing(lpObj->pWarehouse[target].m_Type) )
							{
								gObjUseSkill.SkillChangeUse(aIndex);
							}
						}

						gObjMakePreviewCharSet(aIndex);
						GCEquipmentChange(aIndex,source);
					}
				}
			return 2;
		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			BYTE res_2;
				switch(sFlag)
				{
				case 0:
					res_2 = gObjChaosBoxInsertItemPos(aIndex,lpObj->pInventory[source],target,-1);
					break;
				case 3:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
					res_2 = gObjChaosBoxInsertItemPos(aIndex,lpObj->pChaosBox[source],target,source);
					break;
				default : break;
				}

				if(res_2 == 255)
				{
					return -1;
				}

				switch(sFlag)
				{
				case 0:
					lpObj->pChaosBox[target] = lpObj->pInventory[source];
					gObjInventoryDeleteItem(aIndex,source);
					break;
				case 3:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
					lpObj->pChaosBox[target] = lpObj->pChaosBox[source];
					gObjChaosBoxDeleteItem(aIndex,source);
					break;
				default : break;
				}

				gObjChaosItemSet(aIndex,target,1);

				if(sFlag == 0)
				{
					if(source < INVETORY_WEAR_SIZE || source == ELEMENT_SOCKET )
					{
						if(source == 10 || source == 11)
						{
							if( IsTransformRing(lpObj->pChaosBox[target].m_Type) )
							{
								gObjUseSkill.SkillChangeUse(aIndex);
							}
						}

						gObjMakePreviewCharSet(aIndex);
						GCEquipmentChange(aIndex,source);
					}
				}
			return tFlag;
		default: break;
		}

		if(sFlag == 0 && (source < INVETORY_WEAR_SIZE || source == ELEMENT_SOCKET))
		{
			if(lpObj->pInventory[source].IsItem() == 1)
			{
				if( IsTransformRing(lpObj->pInventory[source].m_Type) )
				{
					gObjUseSkill.SkillChangeUse(aIndex);
				}
			}
			else
			{
				if(source == 10 || source == 11)
				{
					if( IsTransformRing(lpObj->pInventory[target].m_Type) )
					{
						gObjUseSkill.SkillChangeUse(aIndex);
					}
				}
			}

			gObjMakePreviewCharSet(aIndex);
			GCEquipmentChange(aIndex,source);
		}

		if(tFlag == 0 && (target < INVETORY_WEAR_SIZE || target == ELEMENT_SOCKET))
		{
			if( IsTransformRing(lpObj->pInventory[target].m_Type) )
			{
				gObjUseSkill.SkillChangeUse(aIndex);
			}
			
			gObjMakePreviewCharSet(aIndex);
			GCEquipmentChange(aIndex,target);
		}
	}
	__finally
	{
		if(bPersonalShopTrans == 1)
		{
			gObj[aIndex].m_bPShopTransaction = 0;
		}
	}
	return false;
}






BYTE gObjTradeRectCheck(int aIndex, int sx, int sy, int width, int height )
{
	int x;
	int y;
	int blank = 0;
	int xx;
	int yy;

	if ( ( sx + width) > 8 )
	{
		return -1;
	}

	if ( (sy + height) > 4 )
	{
		return -1;
	}

	for ( y=0;y<height;y++)
	{
		yy = sy + y;
		
		for ( x=0;x<width;x++)
		{
			xx = sx + x;

			if ( ::ExtentCheck(xx, yy, 8, 4) == TRUE )
			{
				if ( *(BYTE*)(gObj[aIndex].TradeMap + yy * 8 + xx) != (BYTE)-1)
				{
					blank++;
					return -1;
				}
			}
			else
			{
				LogAdd("error : %s %d", __FILE__ ,__LINE__);
				return -1;
			}
		}
	}

	if ( blank == 0 )
	{
		return (sy * 8 + sx);
	}

	return -1;
}






BOOL gObjTradeItemBoxSet(int aIndex, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos)%8;
	int itemposy = (itempos)/8;

	int xx,yy;

	for(int y = 0; y < yl; y ++)
	{
		yy = itemposy + y;

		for(int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

			if(ExtentCheck(xx,yy,8,4)==1)
			{
				*(BYTE*)(gObj[aIndex].TradeMap + yy*8+xx) = set_byte;
			}
			else
			{
				LogAdd("error : %s %d",__FILE__,__LINE__);
				return false;
			}
		}
	}
	return true;
}






BYTE gObjTradeInventoryMove(LPOBJ lpObj, BYTE source, BYTE target)
{
	int iwidth,iheight,s_num;
	BYTE itembuf[MAX_ITEM_INFO];
	if(source > TRADE_BOX_SIZE)
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return -1;
	}

	if(target > MAIN_INVENTORY_SIZE)
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return -1;
	}

	if(lpObj->TargetNumber < 0)
	{
		return -1;
	}

	if(lpObj->Trade[source].IsItem() == 0)
	{
		return -1;
	}

	if(lpObj->m_IfState.use == 0 || lpObj->m_IfState.type != 1)
	{
		return -1;
	}

	if(gObjInventoryInsertItemPos(lpObj->m_Index,lpObj->Trade[source],target,1) == 255)
	{
		return -1;
	}

	s_num = 0;
	s_num = lpObj->Trade[source].m_Number;

	lpObj->Trade[source].GetSize((int &)iwidth,(int &)iheight);

	gObjTradeItemBoxSet(lpObj->m_Index,source,iwidth,iheight,255);
	lpObj->Trade[source].Clear();

	ItemByteConvert(itembuf,lpObj->Trade[source]);

	GCTradeOtherDel(lpObj->TargetNumber,source);

	if(gObjCheckSerial0ItemList(&lpObj->Trade[source]) != 0)
	{
		MsgOutput(lpObj->m_Index,lMsg.Get(3354));
		LogAddTD("[ANTI-HACK][Serial 0 Item] [Trade] (%s)(%s) Item(%s) Pos(%d)",lpObj->AccountID,lpObj->Name,lpObj->Trade[source].GetName(),source);
		return -1;
	}

	if(gObjInventorySearchSerialNumber(lpObj,s_num) == 0)
	{
		return -1;
	}

	if(target < INVETORY_WEAR_SIZE || target == ELEMENT_SOCKET)
	{
		if(lpObj->pInventory[target].IsItem() == 1)
		{
			if( IsTransformRing(lpObj->pInventory[target].m_Type) )
			{
				gObjUseSkill.SkillChangeUse(lpObj->m_Index);
			}
		}

		gObjMakePreviewCharSet(lpObj->m_Index);
		GCEquipmentChange(lpObj->m_Index,target);
	}

	return false;
}






BYTE gObjInventoryTradeMove(LPOBJ lpObj, BYTE source, BYTE target)
{
	int h,w,iwidth,iheight,s_num,blank;


	if(source > MAIN_INVENTORY_SIZE)
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return -1;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return -1;
	}

	if(lpObj->TargetNumber < 0)
	{
		return -1;
	}

	if(lpObj->m_IfState.use == 0 || lpObj->m_IfState.type != 1)
	{
		return -1;
	}

#if (__CUSTOM__ == 1)

	if( lpObj->pInventory[source].m_bLOCKED == TRUE )
	{
		return -1;
	}

#endif

	if(lpObj->pInventory[source].m_Type == ITEMGET(13,20))
	{
		if(lpObj->pInventory[source].m_Level == 1 )
		{
			return -1;
		}
	}

	if(lpObj->pInventory[source].m_Type == ITEMGET(13,20))
	{
		if(lpObj->pInventory[source].m_Level == 2)
		{
			return -1;
		}
	}

	if(lpObj->pInventory[source].m_Type == ITEMGET(14,11))
	{
		if(lpObj->pInventory[source].m_Level == 13)
		{
			return -1;
		}
	}

	if(lpObj->pInventory[source].m_Type == ITEMGET(13,42))
	{
		return -1;
	}

	if( (lpObj->Authority & 32) != 32 &&
		lpObj->pInventory[source].m_Type == ITEMGET(14,52) )
	{
		return -1;
	}

	if( lpObj->pInventory[source].m_Type == ITEMGET(13,51) )
	{
		if( lpObj->pInventory[source].m_Durability == 0.0f )
		{
			return -1;
		}
	}

	if( lpObj->pInventory[source].m_Type == ITEMGET(14,64) )
	{
		return -1;
	}

	if(lpObj->pInventory[source].m_Type == ITEMGET(12,26))
	{
		if(lpObj->pInventory[source].m_Level == 1 ||
			lpObj->pInventory[source].m_Level == 2 ||
			lpObj->pInventory[source].m_Level == 3 ||
			lpObj->pInventory[source].m_Level == 4 ||
			lpObj->pInventory[source].m_Level == 5)
		{
			return -1;
		}
	}

	if(lpObj->pInventory[source].m_Type == ITEMGET(13,38))
	{
		return -1;
	}

	if(lpObj->pInventory[source].m_Type == ITEMGET(13,39))
	{
		return -1;
	}

	if( lpObj->pInventory[source].m_Type == ITEMGET(13,20) )
	{
		return -1;
	}

	if( lpObj->pInventory[source].m_Type == ITEMGET(13,107) )
	{
		return -1;
	}

	if( IsCashItem(lpObj->pInventory[source].m_Type) == 1)
	{
		return -1;
	}

	if( lpObj->pInventory[source].m_QuestItem )
	{
		return -1;
	}

	if( lpObj->pInventory[source].m_bPeriodItem )
	{
		return -1;
	}

	if( lpObj->pInventory[source].m_bLuckySet )
	{
		return -1;
	}

	if( g_ElementalSystem.IsPentagram(lpObj->pInventory[source].m_Type) )
	{
		if( lpObj->pInventory[source].m_Durability < 1.0f )
			return -1;
	}

	lpObj->pInventory[source].GetSize((int &)iwidth,(int &)iheight);
	s_num = lpObj->pInventory[source].GetNumber();

	if(gObjCheckSerial0ItemList(&lpObj->pInventory[source]) != 0)
	{
		MsgOutput(lpObj->m_Index,lMsg.Get(3354));
		LogAddTD("[ANTI-HACK][Serial 0 Item] [Trade] (%s)(%s) Item(%s) Pos(%d)",lpObj->AccountID,lpObj->Name,lpObj->pInventory[source].GetName(),source);
		return -1;
	}

	if(gObjInventorySearchSerialNumber(lpObj,s_num) == 0)
	{
		return -1;
	}

	w = target % 8;
	h = target / 8;

	if(ExtentCheck(w,h,8,4) == 0)
	{
		return -1;
	}

	if(*(BYTE*)(lpObj->TradeMap + h * 8 + w) == 255)
	{
		BYTE itembuf[MAX_ITEM_INFO];
		blank = gObjTradeRectCheck(lpObj->m_Index,w,h,iwidth,iheight);

		if(blank == 255)
		{
			return -1;
		}

		lpObj->Trade[blank] = lpObj->pInventory[source];
		gObjInventoryDeleteItem(lpObj->m_Index,source);
		gObjTradeItemBoxSet(lpObj->m_Index,blank,iwidth,iheight,1);

		if( g_ElementalSystem.IsPentagram(lpObj->Trade[blank].m_Type) )
		{
			g_ElementalSystem.SendTradeElementalItem(lpObj->m_Index,lpObj->TargetNumber);
		}

		ItemByteConvert(itembuf,lpObj->Trade[blank]);
		GCTradeOtherAdd(lpObj->TargetNumber,blank,itembuf);

		if(source < INVETORY_WEAR_SIZE || source == ELEMENT_SOCKET)
		{
			if(source == 10 || source == 11)
			{
				if( IsTransformRing(lpObj->Trade[blank].m_Type) )
				{
					gObjUseSkill.SkillChangeUse(lpObj->m_Index);
				}
			}
			gObjMakePreviewCharSet(lpObj->m_Index);
			GCEquipmentChange(lpObj->m_Index,source);
		}
		return true;
	}
	return -1;
}






BYTE gObjTradeTradeMove(LPOBJ lpObj, BYTE source, BYTE target)
{
	int h,w;
	int iwidth,iheight;
	BYTE TempTradeMap[TRADE_BOX_MAP_SIZE];
	int blank;

	if(source > TRADE_BOX_SIZE)
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return -1;
	}
	if(source == target)
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return -1;
	}
	if(lpObj->Trade[source].IsItem() == 0)
	{
		return -1;
	}
	if(lpObj->Trade[target].IsItem() == 1)
	{
		return -1;
	}
	if(lpObj->TargetNumber < 0)
	{
		return -1;
	}
	if(lpObj->m_IfState.use == 0 || lpObj->m_IfState.type != 1)
	{
		return -1;
	}

	lpObj->Trade[source].GetSize((int &)iwidth,(int &)iheight);
	memcpy(TempTradeMap,lpObj->TradeMap,TRADE_BOX_MAP_SIZE);
	gObjTradeItemBoxSet(lpObj->m_Index,source,iwidth,iheight,255);

	w = target % 8;
	h = target / 8;

	if(ExtentCheck(w,h,8,4) == 0)
	{
		memcpy(lpObj->TradeMap,TempTradeMap,TRADE_BOX_MAP_SIZE);
		return -1;
	}

	if(*(BYTE*)(lpObj->TradeMap + h * 8 + w) == 255)
	{
		BYTE itembuf[MAX_ITEM_INFO];
		blank = gObjTradeRectCheck(lpObj->m_Index,w,h,iwidth,iheight);

		if(blank == 255)
		{
			memcpy(lpObj->TradeMap,TempTradeMap,TRADE_BOX_MAP_SIZE);
			return -1;
		}

		lpObj->Trade[blank] = lpObj->Trade[source];
		lpObj->Trade[source].Clear();
		GCTradeOtherDel(lpObj->TargetNumber,source);
		gObjTradeItemBoxSet(lpObj->m_Index,blank,iwidth,iheight,1);
		ItemByteConvert(itembuf,lpObj->Trade[blank]);
		GCTradeOtherAdd(lpObj->TargetNumber,blank,itembuf);
		return true;
	}

	memcpy(lpObj->TradeMap,TempTradeMap,TRADE_BOX_MAP_SIZE);
	return -1;
}






void gObjTradeCancel(int aIndex)
{
	if(OBJMAX_RANGE(aIndex) == 0)
	{
		LogAdd("error : index error %s %d",__FILE__,__LINE__);
		return;
	}
	if(gObj[aIndex].Type != OBJ_USER || gObj[aIndex].Connected != PLAYER_PLAYING)
	{
		LogAdd(lMsg.Get(535),gObj[aIndex].AccountID,gObj[aIndex].Name);
		return;
	}
	if(gObj[aIndex].m_IfState.use != 1)
	{
		LogAdd(lMsg.Get(536),gObj[aIndex].AccountID,gObj[aIndex].Name);
		return;
	}
	if(gObj[aIndex].m_IfState.type != 1)
	{
		LogAdd(lMsg.Get(537),gObj[aIndex].AccountID,gObj[aIndex].Name);
		return;
	}
	gObjInventoryRollback(aIndex);
	gObj[aIndex].TargetNumber = -1;
	gObj[aIndex].m_IfState.use = 0;
	gObjCharTradeClear(&gObj[aIndex]);
	GCMoneySend(aIndex,gObj[aIndex].Money);
	GCItemListSend(aIndex);
	GCEquipmentSend(aIndex);
	GCMagicListMultiSend(&gObj[aIndex],0);
	gObjUseSkill.SkillChangeUse(aIndex);

	if(gObj[aIndex].GuildNumber > 0)
	{
		GCGuildViewportNowPaint(aIndex,gObj[aIndex].GuildName,0,0);
		gObjNotifyUpdateUnionV1(&gObj[aIndex]);
		gObjNotifyUpdateUnionV2(&gObj[aIndex]);
	}
	LogAddTD(lMsg.Get(514),gObj[aIndex].AccountID,gObj[aIndex].Name);
}






void gObjTempInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos - INVETORY_WEAR_SIZE) % 8;
	int itemposy = (itempos - INVETORY_WEAR_SIZE) / 8;

	for(int y = 0 ; y < yl ; y++)
	{
		for(int x = 0; x < xl; x++)
		{
			*(BYTE*)(TempMap + (itemposy +y) * 8 + (itemposx + x)) = set_byte;
		}
	}
}





BYTE gObjTempInventoryRectCheck(int aIndex, BYTE * TempMap, int sx, int sy, int width, int height)
{
	int x,y,blank = 0;

	if(sx + width > 8)
	{
		return -1;
	}
	if(sy + height > 24)
	{
		return -2;
	}

#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
	int bResult = CheckOutOfInventory(aIndex, sy, height);

	if( !bResult )
	{
		if( bResult == -1 )
		{
			return -2;
		}
		return -1;
	}
#endif

	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
			if( InventoryExtentCheck(x + sx, y + sy, 8, 24) && *(BYTE*)(TempMap + (sy + y) * 8 + (sx + x)) != 255 )
#else
			if(*(BYTE*)(TempMap + (sy+y)*8 + (sx+x))!= 255)
#endif
			{
				blank += 1;
				return -1;
			}
		}
	}
	if(blank == 0)
	{
		return sx+sy*8+INVETORY_WEAR_SIZE;
	}
	return -1;
}


BYTE gObjExtendTempInventoryRectCheck(BYTE * TempMap, int sx, int sy, int width, int height,int id)
{
	int x,y,blank = 0;

	if(sx + width > 8)
	{
		return -1;
	}
	if(sy + height > 12+(id*4))
	{
		return -2;
	}

	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			if(*(BYTE*)(TempMap + (sy+y)*8 + (sx+x))!= 255)
			{
				blank += 1;
				return -1;
			}
		}
	}
	if(blank == 0)
	{
		return sx+sy*8+INVETORY_WEAR_SIZE;
	}
	return -1;
}





BYTE gObjTempInventoryInsertItem(LPOBJ lpObj, CItem item, BYTE * TempMap)
{
	int x,w,h,iwidth,iheight;
	BYTE blank = 0;

	if(item.GetSize(iwidth,iheight) == 0)
	{
		LogAdd(lMsg.Get(532),__FILE__,__LINE__,item.m_Type);
		return -1;
	}

#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
	int nInventoyHeightofCurUser = lpObj->pInventoryExtend * 4 + 8;
	for(h = 0; h < nInventoyHeightofCurUser; h++)
#else
	for(h = 0; h < 8; h++)
#endif
	{
		for(w = 0; w < 8; w++)
		{
			if(*(BYTE*)(TempMap + h * 8 + w) == 255)
			{
				blank = gObjTempInventoryRectCheck(lpObj->m_Index, TempMap,w,h,iwidth,iheight);

				if(blank == 254)
				{
#if( ENABLETEST_WZINVENTORYEXPAND == 1 )
					goto GOTO_EndFunc;
#else
					goto GOTO_ExtendCheck;
#endif
				}
				if(blank != 255)
				{
					gObjTempInventoryItemBoxSet(TempMap,blank,iwidth,iheight,1);
					return blank;
				}
			}
		}
	}

#if( ENABLETEST_WZINVENTORYEXPAND == 0 ) 
GOTO_ExtendCheck:

	if( lpObj->pInventoryExtend > 0 )
	{
		for( x = 0; x < lpObj->pInventoryExtend; x++ )
		{
			for(h = 8+(x*4); h < 12+(x*4); h++)
			{
				for(w = 0; w < 8; w++)
				{
					if(*(BYTE*)(TempMap + h * 8 + w) == 255)
					{
						blank = gObjExtendTempInventoryRectCheck(TempMap,w,h,iwidth,iheight,x);

						if(blank == 254)
						{
							goto GOTO_ExtendNext;
						}
						if(blank != 255)
						{
							gObjTempInventoryItemBoxSet(TempMap,blank,iwidth,iheight,1);
							return blank;
						}
					}
				}
			}
GOTO_ExtendNext:
			continue;
		}
	}
#else
GOTO_EndFunc:
#endif
	return -1;
}






BOOL TradeItemInventoryPutTest(int aIndex)
{
	BYTE TempInventoryMap[INVENTORY_MAP_SIZE];
	int number,result = 1;
	number = gObj[aIndex].TargetNumber;

	if(gObjIsConnected(number) == 0)
	{
		return false;
	}
	memcpy(TempInventoryMap,gObj[aIndex].pInventoryMap,INVENTORY_MAP_SIZE);

	for(int n = 0; n < TRADE_BOX_SIZE; n++)
	{
		if(gObj[number].Trade[n].IsItem() == 1)
		{
			if(gObjTempInventoryInsertItem(&gObj[aIndex],gObj[number].Trade[n],TempInventoryMap) == 255)
			{
				return false;
			}
			result = 1;
		}
	}
	if(result == 1)
	{
		return true;
	}
	return false;
}






BOOL TradeitemInventoryPut(int aIndex)
{
	int number;
	BYTE insert;
	char szItemName[50] = "Item";
	BYTE ExOption[8];

	number = gObj[aIndex].TargetNumber;

	if(gObjIsConnected(number) == 0)
	{
		return false;
	}

	for(int n = 0; n < TRADE_BOX_SIZE; n++)
	{
		if(gObj[number].Trade[n].IsItem() == 1)
		{
			insert = gObjInventoryInsertItem(aIndex,gObj[number].Trade[n]);
			if(insert != 255)
			{
				strcpy_s(szItemName, sizeof(szItemName), gObj[number].Trade[n].GetName());
				ItemIsBufExOption(ExOption,&gObj[number].Trade[n]);

				g_ElementalSystem.TradeErtel(&gObj[number],&gObj[aIndex],&gObj[aIndex].pInventory[insert]);

				LogAddTD(lMsg.Get(538),
					gObj[number].AccountID,gObj[number].Name,gObj[number].MapNumber,gObj[number].X,gObj[number].Y,
					gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[aIndex].MapNumber,gObj[aIndex].X,gObj[aIndex].Y,
					szItemName,gObj[number].Trade[n].m_Number,gObj[number].Trade[n].m_Level,gObj[number].Trade[n].m_Option1,gObj[number].Trade[n].m_Option2,gObj[number].Trade[n].m_Option3,
					ExOption[0],ExOption[1],ExOption[2],ExOption[3],ExOption[4],ExOption[5],ExOption[6],gObj[number].Trade[n].m_SetOption);

			}
			else
			{
				LogAdd("error : 이건 큰일나는일!!");
				return false;
			}
		}
	}

	g_ElementalSystem.SendElementalItems(&gObj[aIndex],ERTEL_INVENTORY);
	return true;
}






void gObjTradeOkButton(int aIndex)
{
	BYTE result = 1;
	int number = gObj[aIndex].TargetNumber;

	if(number < 0)
	{
		return;
	}

	if(gObjIsConnected(number) == 0)
	{
		return;
	}

	if(gObj[number].TargetNumber != aIndex)
	{
		LogAdd(lMsg.Get(539),gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[number].TargetNumber,gObj[number].TargetNumber);
		return;
	}

	if(gObj[aIndex].TradeOk != 1 || gObj[number].TradeOk != 1)
	{
		return;
	}

	if(TradeItemInventoryPutTest(aIndex) == 1)
	{
		if(TradeItemInventoryPutTest(number) == 1)
		{
			result = 1;
		}
		else
		{
			result = 2;
		}
	}
	else
	{
		result = 2;
	}

	if(gObjCheckMaxZen(aIndex,gObj[number].TradeMoney) == 0)
	{
		result = 2;
	}

	if(gObjCheckMaxZen(number,gObj[aIndex].TradeMoney) == 0)
	{
		result = 2;
	}

	if(g_kJewelOfHarmonySystem.IsEnableToTrade(&gObj[number]) == 0 || g_kJewelOfHarmonySystem.IsEnableToTrade(&gObj[aIndex]) == 0)
	{
		result = 4;
		gObjTradeCancel(aIndex);
		gObjTradeCancel(number);
		CGTradeResult(aIndex,result);
		CGTradeResult(number,result);
		return;
	}

	if(result == 1)
	{
		TradeitemInventoryPut(aIndex);
		TradeitemInventoryPut(number);
		gObjInventoryCommit(aIndex);
		gObjInventoryCommit(number);
		gObjMakePreviewCharSet(aIndex);
		gObjMakePreviewCharSet(number);

		gObj[aIndex].Money -= gObj[aIndex].TradeMoney;
		if(gObj[aIndex].Money < 0)
		{
			gObj[aIndex].Money = 0;
		}
		gObj[number].Money -= gObj[number].TradeMoney;
		if(gObj[number].Money < 0)
		{
			gObj[number].Money = 0;
		}

		gObj[aIndex].Money += gObj[number].TradeMoney;
		gObj[number].Money += gObj[aIndex].TradeMoney;

		LogAddTD(lMsg.Get(540),gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[number].AccountID,gObj[number].Name,gObj[aIndex].TradeMoney,gObj[number].TradeMoney);
		GJSetCharacterInfo(&gObj[aIndex],aIndex,0);
		GJSetCharacterInfo(&gObj[number],number,0);
	}
	else
	{
		result = 2;
		gObjTradeCancel(aIndex);
		gObjTradeCancel(number);
		CGTradeResult(aIndex,result);
		CGTradeResult(number,result);
		return;
	}

	gObj[aIndex].TargetNumber = -1;
	gObj[aIndex].m_IfState.use = 0;
	gObj[number].TargetNumber = -1;
	gObj[number].m_IfState.use = 0;
	gObj[aIndex].TradeOk = 0;
	gObj[number].TradeOk = 0;
	gObj[aIndex].TradeMoney = 0;
	gObj[number].TradeMoney = 0;

	GCMoneySend(aIndex,gObj[aIndex].Money);
	GCMoneySend(number,gObj[number].Money);
	CGTradeResult(aIndex,result);
	CGTradeResult(number,result);

	GCItemListSend(aIndex);
	GCItemListSend(number);

	for(int n = 0; n < TRADE_BOX_SIZE; n++)
	{
		gObj[aIndex].Trade[n].Clear();
		gObj[number].Trade[n].Clear();
	}

	gObjNotifyUpdateUnionV1(&gObj[aIndex]);
	gObjNotifyUpdateUnionV2(&gObj[aIndex]);
}






BYTE LevelSmallConvert(int aIndex, int inventoryindex)
{
	int level = gObj[aIndex].pInventory[inventoryindex].m_Level;

	if(level == 15)
	{
		return 7;
	}

	if(level >= 13 && level <= 14)
	{
		return 6;
	}

	if(level >= 11 && level <= 12)
	{
		return 5;
	}

	if(level >= 9 && level <= 10)
	{
		return 4;
	}

	if(level >= 7 && level <= 8)
	{
		return 3;
	}

	if(level >= 5 && level <= 6)
	{
		return 2;
	}

	if(level >= 3 && level <= 4)
	{
		return 1;
	}
	
	return 0;
}






BYTE LevelSmallConvert(int level)
{
	if( level == 15 )
	{
		return 7;
	}

	if(level >= 13 && level <= 14)
	{
		return 6;
	}

	if(level >= 11 && level <=12)
	{
		return 5;
	}

	if(level >= 9 && level <= 10)
	{
		return 4;
	}

	if(level >= 7 && level <= 8)
	{
		return 3;
	}

	if(level >= 5 && level <= 6)
	{
		return 2;
	}

	if(level >= 3 && level <= 4)
	{
		return 1;
	}
	
	return 0;
}






void gObjAttackDamageCalc(int aIndex) 
{
	return;	// Here goes a Macro
}






void gObjMakePreviewCharSet(int aIndex)
{
	if ( !OBJMAX_RANGE(aIndex ))
	{
		LogAdd("error : %s %d", __FILE__, __LINE__);
		return;
	}

	BYTE index;
	LPOBJ lpObj = &gObj[aIndex];
	memset(lpObj->CharSet, 0, sizeof(lpObj->CharSet));

	lpObj->CharSet[CS_CLASS] = CS_SET_CLASS(lpObj->Class);
	lpObj->CharSet[CS_CLASS] |= CS_SET_CHANGEUP(lpObj->ChangeUP);
	lpObj->CharSet[CS_CLASS] |= (lpObj->ChangeUP2 << 3) & 8;

	// Set Class
	if ( lpObj->m_ActionNumber == 0x80 )
	{
		lpObj->CharSet[CS_CLASS] |= 2;
	}
	else if ( lpObj->m_ActionNumber == 0x81 )
	{
		lpObj->CharSet[CS_CLASS] |= 3;
	}
	else
	{
		lpObj->CharSet[CS_CLASS] = lpObj->CharSet[CS_CLASS];
	}

	// Set Righth HAnd Item
	if ( lpObj->pInventory[0].m_Type < ITEMGET(0,0) )
	{
		lpObj->CharSet[CS_WEAPON1_DATA] |= (BYTE)DBI_GET_TYPE(-1);
		lpObj->CharSet[CS_WEAPON1_TYPE] = (BYTE)DBI_GET_INDEX(-1);
	}
	else
	{
		lpObj->CharSet[CS_WEAPON1_DATA] |= DBI_GET_TYPE(lpObj->pInventory[0].m_Type);
		lpObj->CharSet[CS_WEAPON1_TYPE] = DBI_GET_INDEX(lpObj->pInventory[0].m_Type);
	}

	// Set Left Hand
	if ( lpObj->pInventory[1].m_Type < ITEMGET(0,0) )
	{
		lpObj->CharSet[CS_WEAPON2_DATA] |= (BYTE)DBI_GET_TYPE(-1);
		lpObj->CharSet[CS_WEAPON2_TYPE] = (BYTE)DBI_GET_INDEX(-1);
	}
	else
	{
		lpObj->CharSet[CS_WEAPON2_DATA] |= DBI_GET_TYPE(lpObj->pInventory[1].m_Type);
		lpObj->CharSet[CS_WEAPON2_TYPE] = DBI_GET_INDEX(lpObj->pInventory[1].m_Type);
	}

	// Set Helmet
	if ( lpObj->pInventory[2].m_Type < ITEMGET(0,0) )
	{
		lpObj->CharSet[13] |= CS_SET_HELMET1(-1);
		lpObj->CharSet[9]  |= CS_SET_HELMET2(-1);
		lpObj->CharSet[3]  |= CS_SET_HELMET3(-1);
	}
	else
	{
		lpObj->CharSet[13] |= CS_SET_HELMET1(lpObj->pInventory[2].m_Type);
		lpObj->CharSet[9]  |= CS_SET_HELMET2(lpObj->pInventory[2].m_Type);
		lpObj->CharSet[3]  |= CS_SET_HELMET3(lpObj->pInventory[2].m_Type);
	}

	// Set Armor
	if ( lpObj->pInventory[3].m_Type < ITEMGET(0,0) )
	{
		lpObj->CharSet[14] |= CS_SET_ARMOR1(-1);
		lpObj->CharSet[9]  |= CS_SET_ARMOR2(-1);
		lpObj->CharSet[3]  |= CS_SET_ARMOR3(-1);
	}
	else
	{
		lpObj->CharSet[14] |= CS_SET_ARMOR1(lpObj->pInventory[3].m_Type);
		lpObj->CharSet[9]  |= CS_SET_ARMOR2(lpObj->pInventory[3].m_Type);
		lpObj->CharSet[3]  |= CS_SET_ARMOR3(lpObj->pInventory[3].m_Type);
	}

	// Set Pants
	if ( lpObj->pInventory[4].m_Type < ITEMGET(0,0) )
	{
		lpObj->CharSet[14] |= CS_SET_PANTS1(-1);
		lpObj->CharSet[9]  |= CS_SET_PANTS2(-1);
		lpObj->CharSet[4]  |= CS_SET_PANTS3(-1);
	}
	else
	{
		lpObj->CharSet[14] |= CS_SET_PANTS1(lpObj->pInventory[4].m_Type);
		lpObj->CharSet[9]  |= CS_SET_PANTS2(lpObj->pInventory[4].m_Type);
		lpObj->CharSet[4]  |= CS_SET_PANTS3(lpObj->pInventory[4].m_Type);
	}

	// Set Gloves
	if ( lpObj->pInventory[5].m_Type < ITEMGET(0,0) )
	{
		lpObj->CharSet[15] |= CS_SET_GLOVES1(-1);
		lpObj->CharSet[9]  |= CS_SET_GLOVES2(-1);
		lpObj->CharSet[4]  |= CS_SET_GLOVES3(-1);
	}
	else
	{
		lpObj->CharSet[15] |= CS_SET_GLOVES1(lpObj->pInventory[5].m_Type);
		lpObj->CharSet[9]  |= CS_SET_GLOVES2(lpObj->pInventory[5].m_Type);
		lpObj->CharSet[4]  |= CS_SET_GLOVES3(lpObj->pInventory[5].m_Type);
	}

	// Set Boots
	if ( lpObj->pInventory[6].m_Type < ITEMGET(0,0) )
	{
		lpObj->CharSet[15] |= CS_SET_BOOTS1(-1);
		lpObj->CharSet[9]  |= CS_SET_BOOTS2(-1);
		lpObj->CharSet[5]  |= CS_SET_BOOTS3(-1);
	}
	else
	{
		lpObj->CharSet[15] |= CS_SET_BOOTS1(lpObj->pInventory[6].m_Type);
		lpObj->CharSet[9]  |= CS_SET_BOOTS2(lpObj->pInventory[6].m_Type);
		lpObj->CharSet[5]  |= CS_SET_BOOTS3(lpObj->pInventory[6].m_Type);
	}

	index = 0;

	// Set Part onf Wings
	if ( lpObj->pInventory[7].m_Type < ITEMGET(0,0) )
	{
		index |= CS_SET_WING(0);
	}
	else
	{
		if( lpObj->pInventory[7].m_Type >= ITEMGET(12,0) && lpObj->pInventory[7].m_Type <= ITEMGET(12,2) )
		{
			index |= CS_SET_WING(1);
			lpObj->CharSet[9] |= (lpObj->pInventory[7].m_Type + 1) & 0x03;
		}
		else if( lpObj->pInventory[7].m_Type == ITEMGET(12,41) )
		{
			index |= CS_SET_WING(1);
			lpObj->CharSet[9] |= 4;
		}
		else if( lpObj->pInventory[7].m_Type == ITEMGET(12,266) )
		{
			index |= CS_SET_WING(1);
			lpObj->CharSet[9] |= 5;
		}
		else if( lpObj->pInventory[7].m_Type == ITEMGET(12,267) )
		{
			index |= CS_SET_WING(1);
			lpObj->CharSet[9] |= 6;
		}
	}

	if ( lpObj->pInventory[8].m_Type != ITEMGET(13,4) )
	{
		if ( lpObj->pInventory[8].m_Type < ITEMGET(0,0) )
		{
			index |= CS_SET_HELPER(-1);
		}
		else if( lpObj->pInventory[8].m_Type == ITEMGET(13,67) ||
				 lpObj->pInventory[8].m_Type == ITEMGET(13,123) )
		{

		}
		else
		{
			index |= CS_SET_HELPER(lpObj->pInventory[8].m_Type);
		}
	}
	else
	{
		index |= CS_SET_HELPER(-1);
	}

	lpObj->CharSet[5] |= index;

	int levelindex = CS_SET_SMALLLEVEL_RH(LevelSmallConvert(aIndex, 0));
	levelindex |= CS_SET_SMALLLEVEL_LH(LevelSmallConvert(aIndex, 1));
	levelindex |= CS_SET_SMALLLEVEL_HELMET(LevelSmallConvert(aIndex, 2));
	levelindex |= CS_SET_SMALLLEVEL_ARMOR(LevelSmallConvert(aIndex, 3));
	levelindex |= CS_SET_SMALLLEVEL_PANTS(LevelSmallConvert(aIndex, 4));
	levelindex |= CS_SET_SMALLLEVEL_GLOVES(LevelSmallConvert(aIndex, 5));
	levelindex |= CS_SET_SMALLLEVEL_BOOTS(LevelSmallConvert(aIndex, 6));

	lpObj->CharSet[6] = CS_SET_SMALLLEVEL1(levelindex);
	lpObj->CharSet[7] = CS_SET_SMALLLEVEL2(levelindex);
	lpObj->CharSet[8] = CS_SET_SMALLLEVEL3(levelindex);

	if ( (lpObj->pInventory[7].m_Type >= ITEMGET(12,3) && lpObj->pInventory[7].m_Type <= ITEMGET(12,6)) ||
		  lpObj->pInventory[7].m_Type == ITEMGET(13,30) || 
		  lpObj->pInventory[7].m_Type == ITEMGET(12,42) || lpObj->pInventory[7].m_Type == ITEMGET(12,49) )
	{
		lpObj->CharSet[5] |= CS_SET_WING(2);
		
		if ( lpObj->pInventory[7].m_Type == ITEMGET(13,30) )
		{
			lpObj->CharSet[9] |= 5;
		}
		else if( lpObj->pInventory[7].m_Type == ITEMGET(12,42) )
		{
			lpObj->CharSet[9] |= 6;
		}
		else if( lpObj->pInventory[7].m_Type == ITEMGET(12,49) )
		{
			lpObj->CharSet[9] |= 7;
		}
		else
		{
			lpObj->CharSet[9] |= (lpObj->pInventory[7].m_Type - 2) & 0x07;
		}
	}

	// Set Excellent Items
	lpObj->CharSet[10] = 0;

	if ( lpObj->pInventory[2].IsExtItem() != FALSE )
		lpObj->CharSet[10] = (char)0x80;

	if ( lpObj->pInventory[3].IsExtItem() != FALSE )
		lpObj->CharSet[10] |= 0x40;

	if ( lpObj->pInventory[4].IsExtItem() != FALSE )
		lpObj->CharSet[10] |= 0x20;

	if ( lpObj->pInventory[5].IsExtItem() != FALSE )
		lpObj->CharSet[10] |= 0x10;

	if ( lpObj->pInventory[6].IsExtItem() != FALSE )
		lpObj->CharSet[10] |= 0x8;

	if ( lpObj->pInventory[0].IsExtItem() != FALSE )
		lpObj->CharSet[10] |= 0x04;

	if ( lpObj->pInventory[1].IsExtItem() != FALSE )
		lpObj->CharSet[10] |= 0x02;

	// Set Set Items
	lpObj->CharSet[11] = 0;

	if ( lpObj->pInventory[2].IsSetItem() != FALSE )
		lpObj->CharSet[11] = (char)0x80;

	if ( lpObj->pInventory[3].IsSetItem() != FALSE )
		lpObj->CharSet[11] |= 0x40;

	if ( lpObj->pInventory[4].IsSetItem() != FALSE )
		lpObj->CharSet[11] |= 0x20;

	if ( lpObj->pInventory[5].IsSetItem() != FALSE )
		lpObj->CharSet[11] |= 0x10;

	if ( lpObj->pInventory[6].IsSetItem() != FALSE )
		lpObj->CharSet[11] |= 0x8;

	if ( lpObj->pInventory[0].IsSetItem() != FALSE )
		lpObj->CharSet[11] |= 0x04;

	if ( lpObj->pInventory[1].IsSetItem() != FALSE )
		lpObj->CharSet[11] |= 0x02;

	gObjCalCharacter(aIndex);

	if ( lpObj->IsFullSetItem != false )
		lpObj->CharSet[11] |= 0x01;

	if ( (lpObj->pInventory[8].m_Type & 0x03) != 0 )
	{
		if ( lpObj->pInventory[8].m_Type > 0 )
		{
			lpObj->CharSet[10] |= 0x01;
		}
	}

	if ( lpObj->pInventory[8].m_Type == ITEMGET(13,4) )	// Dark Horse
	{
		lpObj->CharSet[12] |= 0x01;
	}

	if ( (  lpObj->pInventory[10].IsItem() == TRUE &&
			lpObj->pInventory[10].m_Type == ITEMGET(13,20) &&
			lpObj->pInventory[10].m_Level == 3 && 
			lpObj->pInventory[10].m_Durability > 0.0f ) ||
		 (  lpObj->pInventory[11].IsItem() == TRUE &&
			lpObj->pInventory[11].m_Type == ITEMGET(13,20) &&
			lpObj->pInventory[11].m_Level == 3 && 
			lpObj->pInventory[11].m_Durability > 0.0f ) )
	{
		lpObj->CharSet[12] |= 0x02;
	}

	lpObj->CharSet[16] = 0;
	lpObj->CharSet[17] = 0;

	if ( lpObj->pInventory[7].m_Type >= ITEMGET(12,262) && lpObj->pInventory[7].m_Type <= ITEMGET(12,265) )
	{
		lpObj->CharSet[5] |= CS_SET_WING(2);
		lpObj->CharSet[16] |= ((lpObj->pInventory[7].m_Type - ITEMGET(12,261)) & 7)<<2;
	}

	if ( lpObj->pInventory[8].m_Type == ITEMGET(13,37) )	// Fenrir
	{
		lpObj->CharSet[10] &= 0xFE;
		lpObj->CharSet[12] &= 0xFE;
		lpObj->CharSet[12] |= 0x04;

		if ( lpObj->pInventory[8].IsFenrirIncLastAttackDamage() != FALSE )
		{
			lpObj->CharSet[16] |= 0x01;
		}

		if ( lpObj->pInventory[8].IsFenrirDecLastAttackDamage() != FALSE )
		{
			lpObj->CharSet[16] |= 0x02;
		}

		if ( lpObj->pInventory[8].IsFenrirSpecial() != FALSE )
		{
			lpObj->CharSet[17] |= 0x01;
		}
	}

	if ( lpObj->pInventory[7].m_Type >= ITEMGET(12,36) && lpObj->pInventory[7].m_Type <= ITEMGET(12,40) 
		|| lpObj->pInventory[7].m_Type == ITEMGET(12,43) 
		|| lpObj->pInventory[7].m_Type == ITEMGET(12,50) )
	{
		lpObj->CharSet[5] |= CS_SET_WING(3);
		lpObj->CharSet[9] |= (lpObj->pInventory[7].m_Type - 0x23) & 7;

		if( lpObj->pInventory[7].m_Type == ITEMGET(12,43) )
		{
			lpObj->CharSet[9] |= 0x06;
		}
		else if( lpObj->pInventory[7].m_Type == ITEMGET(12,50) )
		{
			lpObj->CharSet[9] |= 0x07;
		}
	}

	if( lpObj->pInventory[7].m_Type >= ITEMGET(12,130) && lpObj->pInventory[7].m_Type <= ITEMGET(12,135) )
	{
		lpObj->CharSet[5] |= CS_SET_WING(3);
		lpObj->CharSet[17] |= ((lpObj->pInventory[7].m_Type-1)&7)*32;
	}

	BYTE Helper = 0;

	switch( lpObj->pInventory[8].m_Type )
	{
	case ITEMGET(13,64):
		Helper = 0x20;
		break;
	case ITEMGET(13,65):
		Helper = 0x40;
		break;
	case ITEMGET(13,67):
		Helper = 0x80;
		break;
	case ITEMGET(13,80):
		Helper = 0xE0;
		break;
	case ITEMGET(13,106):
		Helper = 0xA0;
		break;
	case ITEMGET(13,123):
		Helper = 0x60;
		break;
	}

	if( Helper != 0 )
	{
		lpObj->CharSet[10] &= 0xFE;
		lpObj->CharSet[12] &= 0xFE;

		lpObj->CharSet[16] |= Helper;
	}

#if (CUSTOM_WINGS == 1)

	if( lpObj->pInventory[7].m_Type >= ITEMGET(12,440) &&
		lpObj->pInventory[7].m_Type <= ITEMGET(12,445) )
	{
		lpObj->CharSet[17] |= 0x10+ (lpObj->pInventory[7].m_Type%512)%440;
	}

#endif
}








void gObjViewportPaint(HWND hWnd, short aIndex)
{

	int n;
	HDC hdc;
	char szTemp[256];
	int count = 0;
	int count2 = 0;
	int count3 = 0;
	int playerc = 0;
	int totalplayer = 0;
	int offtraders = 0;

	if ( !OBJMAX_RANGE(aIndex))
		return;

	hdc = GetDC(hWnd);

	for ( n=0;n<OBJMAX;n++)
	{
		if ( gObj[n].Live != FALSE )
		{
			if ( gObj[n].Connected != PLAYER_EMPTY )
			{
				if ( gCurPaintMapNumber == gObj[n].MapNumber )
				{
					if ( gObj[n].Type == OBJ_USER )
					{
						playerc++;
					}
				}
			}
		}

		if ( gObj[n].Type == OBJ_USER && gObj[n].Connected != PLAYER_EMPTY )
		{
			totalplayer++;
#if (ZEONWINDOW_STAT == TRUE)
#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
			if( gObj[n].bOffTrade == 1 )
			{
				offtraders++;
			}
#endif
#endif
		}
		else if ( gObj[n].Connected != PLAYER_EMPTY )
		{
			count++;
		}
	}				

	if ( gCurPaintType == 1 )
	{
		RECT rect;
		int iStartX = 100;
		int iStartY = 50;
		int iWidth	= 1;
		int iHeight	= 1;
		int iMagnifying = 3;
		int x;
		int y;

		hdc = GetDC(hWnd);

		HBRUSH hCharacterBrush = CreateSolidBrush(RGB(255, 0, 0));
		HBRUSH hMonsterBrush = CreateSolidBrush(RGB(128, 128, 128));
		HBRUSH hNpcBrush = CreateSolidBrush(RGB(0, 255, 255));
		HBRUSH hItemBrush = CreateSolidBrush(RGB(0, 0, 255));
		HBRUSH hCrywolfMovePath = CreateSolidBrush(RGB(0, 255, 0));
		int iOldBkMode = SetBkMode(hdc,TRANSPARENT);
		
		// Set a Gray backgraound on non-walkable areas
		if ( MapC[gCurPaintMapNumber].m_attrbuf != NULL )
		{
			for ( y=0;y<255;y++)
			{
				for (x=0;x<255;x++)
				{
					if ( (MapC[gCurPaintMapNumber].m_attrbuf[y*256+x] & 4) ==4 || (MapC[gCurPaintMapNumber].m_attrbuf[y*256+x]&8) == 8)
					{
						rect.left = iStartX+y*iWidth*iMagnifying;
						rect.right = (iStartX+y*iWidth*iMagnifying) + (iWidth*iMagnifying);
						rect.top = iStartY+x*iHeight*iMagnifying;
						rect.bottom = (iStartY+x*iHeight*iMagnifying) + (iHeight*iMagnifying);

						FillRect(hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
					}
				}
			}
		}

		if( TMonsterAIElement::s_MonsterAIMovePath[gCurPaintMapNumber].m_bDataLoad != FALSE )
		{
			TMonsterAIMovePath & MovePath = TMonsterAIElement::s_MonsterAIMovePath[gCurPaintMapNumber];

			for ( int i=0;i<MovePath.m_iMovePathSpotCount;i++)
			{
				rect.left = iStartX+MovePath.m_MovePathInfo[i].m_iPathY*iWidth*iMagnifying;
				rect.right = (iStartX+MovePath.m_MovePathInfo[i].m_iPathY*iWidth*iMagnifying) + (iWidth*iMagnifying);
				rect.top = iStartY+MovePath.m_MovePathInfo[i].m_iPathX*iHeight*iMagnifying;
				rect.bottom = (iStartY+MovePath.m_MovePathInfo[i].m_iPathX*iHeight*iMagnifying) + (iHeight*iMagnifying);

				FillRect(hdc, &rect, hCrywolfMovePath);

				CString szDesc;
				szDesc.Format("(%d,%d)", MovePath.m_MovePathInfo[i].m_iPathX, MovePath.m_MovePathInfo[i].m_iPathY);
			}
		}

		for ( n=0;n<OBJMAX;n++)
		{
			if ( gObj[n].Live != FALSE )
			{
				if ( gObj[n].Connected != PLAYER_EMPTY )
				{
					if ( gCurPaintMapNumber == gObj[n].MapNumber )
					{
						if ( gObj[n].Type == OBJ_USER )
						{
							rect.left = iStartX+gObj[n].Y*iWidth*iMagnifying;
							rect.right = (iStartX+gObj[n].Y*iWidth*iMagnifying) + ((iWidth+2)*iMagnifying);
							rect.top = iStartY+gObj[n].X*iHeight*iMagnifying;
							rect.bottom = (iStartY+gObj[n].X*iHeight*iMagnifying) + ((iHeight+2)*iMagnifying);

							FillRect(hdc, &rect, hCharacterBrush);

							CString szName;
							szName.Format("%s (%d,%d)", gObj[n].Name, gObj[n].X, gObj[n].Y);
							TextOut(hdc, rect.left, rect.bottom, szName, szName.GetLength());
						}
						else if ( gObj[n].Type == OBJ_MONSTER )
						{
							rect.left = iStartX+gObj[n].Y*iWidth*iMagnifying;
							rect.right = (iStartX+gObj[n].Y*iWidth*iMagnifying) + ((iWidth)*iMagnifying);
							rect.top = iStartY+gObj[n].X*iHeight*iMagnifying;
							rect.bottom = (iStartY+gObj[n].X*iHeight*iMagnifying) + ((iHeight)*iMagnifying);

							FillRect(hdc, &rect, hMonsterBrush);

							if ( gObj[n].m_iCurrentAI != 0 )
							{
								if ( gObj[n].m_iGroupMemberGuid == 0 )
								{
									FillRect(hdc, &rect, hCharacterBrush);

									CString szDesc;
									szDesc.Format("G(%d,%d)", gObj[n].m_iGroupNumber, gObj[n].m_iGroupMemberGuid );
									TextOut(hdc, rect.left, rect.bottom, szDesc, szDesc.GetLength());
								}
							}
						}
						else if ( gObj[n].Type == OBJ_NPC )
						{
							rect.left = iStartX+gObj[n].Y*iWidth*iMagnifying;
							rect.right = (iStartX+gObj[n].Y*iWidth*iMagnifying) + ((iWidth)*iMagnifying);
							rect.top = iStartY+gObj[n].X*iHeight*iMagnifying;
							rect.bottom = (iStartY+gObj[n].X*iHeight*iMagnifying) + ((iHeight)*iMagnifying);

							FillRect(hdc, &rect, hNpcBrush);
						}
					}
				}
			}
		}

		for ( int n=0;n<MAX_MAPITEM;n++)
		{
			if ( MapC[gCurPaintMapNumber].m_cItem[n].IsItem() )
			{
				rect.left = iStartX+MapC[gCurPaintMapNumber].m_cItem[n].py*iWidth*iMagnifying;
				rect.right = (iStartX+MapC[gCurPaintMapNumber].m_cItem[n].py*iWidth*iMagnifying) + (iWidth*iMagnifying);
				rect.top = iStartY+MapC[gCurPaintMapNumber].m_cItem[n].px*iHeight*iMagnifying;
				rect.bottom = (iStartY+MapC[gCurPaintMapNumber].m_cItem[n].px*iHeight*iMagnifying) + (iHeight*iMagnifying);

				FillRect(hdc, &rect, hItemBrush);

				CString szDesc;
				szDesc.Format("%s (%d,%d)", MapC[gCurPaintMapNumber].m_cItem[n].GetName(),
					MapC[gCurPaintMapNumber].m_cItem[n].px, MapC[gCurPaintMapNumber].m_cItem[n].py);
			}
		}

		DeleteObject((HGDIOBJ)hCharacterBrush);
		DeleteObject((HGDIOBJ)hMonsterBrush);
		DeleteObject((HGDIOBJ)hNpcBrush);
		DeleteObject((HGDIOBJ)hItemBrush);
		DeleteObject((HGDIOBJ)hCrywolfMovePath);
		SetBkMode(hdc, iOldBkMode);
	}

	gObjTotalUser = totalplayer;

#if (ZEONWINDOW_STAT == FALSE)
	wsprintf(szTemp, "COUNT:%d  TotalPlayer : %d  Player(%d):%d VpCount:%d(%d/%d) : item count:%d max:%d", count, totalplayer, aIndex, playerc, gObj[aIndex].VPCount, count3, count2, gItemLoop, gServerMaxUser);
#else
	wsprintf(szTemp, "Monsters: [%d] :: Online: [%d/%d] :: OffTraders: [%d] :: Items: [%d]", count, totalplayer, gServerMaxUser, offtraders, gItemLoop);
#endif

#if (ZEONWINDOW_STAT == FALSE)
	if ( gFireCrackerEvent )
		strcat(szTemp, ":FireCracker[ON]");
	else
		strcat(szTemp, ":FireCracker[OFF]");

	if ( gHeartOfLoveEvent )
		strcat(szTemp, ":HeartOfLove[ON]");
	else
		strcat(szTemp, ":HeartOfLove[OFF]");

	if ( gMedalEvent )
		strcat(szTemp, ":MedalEvent[ON]");
	else
		strcat(szTemp, ":MedalEvent[OFF]");
#endif

	TextOut(hdc, GAMESERVER_WIDTH / 2 - 200, 0, szTemp, strlen(szTemp));
	ReleaseDC(hWnd, hdc);
	ReleaseDC(hWnd, hdc);
}
		



static int FrustrumX[MAX_ARRAY_FRUSTRUM];
static int FrustrumY[MAX_ARRAY_FRUSTRUM];

void InitFrustrum()
{
	float CameraViewFar = 2400.0;
	float CameraViewNear = CameraViewFar * 0.19f;
	float CameraViewTarget = CameraViewFar * 0.53f;
	float WidthFar = 1190.0;
	float WidthNear =  550.0;

	float p[4][3];

	p[0][0] = -WidthFar;
	p[0][1] = CameraViewFar-CameraViewTarget;
	p[0][2] = 0;
	p[1][0] = WidthFar;
	p[1][1] = CameraViewFar-CameraViewTarget;
	p[1][2] = 0;
	p[2][0] = WidthNear;
	p[2][1] = CameraViewNear - CameraViewTarget;
	p[2][2] = 0;
	p[3][0]= -WidthNear;
	p[3][1] = CameraViewNear - CameraViewTarget;
	p[3][2] = 0;

	float Angle[3];
	Angle[0] = 0;
	Angle[1] = 0;
	Angle[2] = 45.0f;

	float Matrix[3][4];
	float Frustrum[4][3];

	AngleMatrix(Angle,Matrix);

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM;i++)
	{
		VectorRotate((const float *)&p[i],(const float *)&Matrix[0],(float *)&Frustrum[i]);
		FrustrumX[i] = (int)(Frustrum[i][0]*0.01f);
		FrustrumY[i] = (int)(Frustrum[i][1]*0.01f);

	}
}








void CreateFrustrum(int x, int y, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	for ( int i=0;i<MAX_ARRAY_FRUSTRUM;i++)
	{
		lpObj->FrustrumX[i] = FrustrumX[i] + x;
		lpObj->FrustrumY[i] = FrustrumY[i] + y;
	}
}







bool TestFrustrum2(int x, int y, int aIndex)
{
	int j = 3;
	LPOBJ lpObj = &gObj[aIndex];

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM;j = i,i++)
	{
		if((lpObj->FrustrumX[i]- x) * (lpObj->FrustrumY[j]-y) - (lpObj->FrustrumX[j]-x) * (lpObj->FrustrumY[i]-y) < 0.0f)
		{
			return false;
		}
	}
	return true;
}






BOOL gObjCheckViewport(int aIndex, int x, int y)
{
	LPOBJ lpObj = &gObj[aIndex];

	if(x < lpObj->X - 25  || x > lpObj->X + 25 || y < lpObj->Y - 25 || y > lpObj->Y + 25)
	{
		return false;
	}

	int j = 3;

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM; j = i, i++)
	{
		int frustrum = (lpObj->FrustrumX[i]- x) * (lpObj->FrustrumY[j]-y) - (lpObj->FrustrumX[j]-x) * (lpObj->FrustrumY[i]-y);
		if(frustrum < 0)
		{
			return false;
		}
	}
	return true;
}


void gObjViewportClose(LPOBJ lpObj)
{
	int tObjNum,i;
	int MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	if(lpObj->Connected == PLAYER_PLAYING)
	{
		for(int n = 0; n < MVL; n++)
		{
			if(lpObj->VpPlayer2[n].state != 0)
			{
				if(lpObj->VpPlayer2[n].type != 5)
				{
					tObjNum = lpObj->VpPlayer2[n].number;

					i = 0;

					while( true )
					{
						if(gObj[tObjNum].VpPlayer[i].number == lpObj->m_Index)
						{
							if(gObj[tObjNum].VpPlayer[i].type == lpObj->Type)
							{
								gObj[tObjNum].VpPlayer[i].state = 3;
								break;
							}
						}

						if(i < MVL-1)
						{
							i++;
						}
						else
						{
							break;
						}
					}

					i = 0;

					while( true )
					{
						if(gObj[tObjNum].VpPlayer2[i].number == lpObj->m_Index)
						{
							if(gObj[tObjNum].VpPlayer2[i].type == lpObj->Type)
							{
								gObj[tObjNum].VpPlayer2[i].state = 0;
								break;
							}
						}

						if(i < MVL-1)
						{
							i++;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}
	gObjClearViewport(lpObj);
}






void gObjViewportListCreate(short aIndex)
{
	int result,n;
	LPOBJ lpObj;
	int mapnum;

	if(OBJMAX_RANGE(aIndex) == 0)
	{
		return;
	}

	lpObj = &gObj[aIndex];

	if(lpObj->Connected < PLAYER_PLAYING)
	{
		return;
	}

	if(lpObj->RegenOk > 0)
	{
		return;
	}

	mapnum = lpObj->MapNumber;
	gItemLoop2 = 0;

	if(lpObj->Type == OBJ_USER)
	{
		MapClass * lpMap = &MapC[mapnum];

		for(n = 0; n < MAX_MAPITEM; n++)
		{
			if(lpMap->m_cItem[n].live)
			{
				gItemLoop2++;

				if(lpMap->m_cItem[n].m_State == 1 || lpMap->m_cItem[n].m_State == 2)
				{
					if(gObjCheckViewport(aIndex,lpMap->m_cItem[n].px,lpMap->m_cItem[n].py) == 1)
					{
						result = ViewportAdd(aIndex,n,5);
					}
				}
			}
		}
	}

	if(aIndex == 0)
	{
		gItemLoopMax = gItemLoop2;
		gItemLoop = gItemLoop2;
		gCurPaintPlayer = aIndex;
	}

	int a = 1;
	LPOBJ lpTempObj;

	if(lpObj->Type == OBJ_USER)
	{
		for(n = 0; n < OBJMAX; n++)
		{
			lpTempObj = &gObj[n];

			if(lpTempObj->Connected == PLAYER_PLAYING && aIndex != n)
			{
				if(lpTempObj->m_State == 1 || lpTempObj->m_State == 2)
				{
					if(mapnum == lpTempObj->MapNumber)
					{
						if(gObjCheckViewport(aIndex,lpTempObj->X,lpTempObj->Y) == 1)
						{
							result = ViewportAdd(aIndex,n,lpTempObj->Type);
							result = ViewportAdd2(n,aIndex,gObj[aIndex].Type);
						}
					}
				}
			}
		}
	}
	else if(lpObj->Type == OBJ_MONSTER
#if (GS_CASTLE == 1)
		|| lpObj->Type == OBJ_NPC
#endif
		)
	{
		for(n = OBJ_MAXMONSTER; n < OBJMAX; n++)
		{
			lpTempObj = &gObj[n];

			if(lpTempObj->Connected == PLAYER_PLAYING && aIndex != n)
			{
				if(lpTempObj->m_State == 1 || lpTempObj->m_State == 2)
				{
					if(mapnum == lpTempObj->MapNumber)
					{
						if(gObjCheckViewport(aIndex, lpTempObj->X, lpTempObj->Y) == 1)
						{
							result = ViewportAdd(aIndex,n,gObj[n].Type);
							result = ViewportAdd2(n,aIndex,gObj[aIndex].Type);
						}
					}
				}
			}
		}
	}
}






int  ViewportAdd(int aIndex, int aAddIndex, int aType)
{
	int blank = -1;
	int count = 0;
	LPOBJ lpObj = &gObj[aIndex];
	int MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	for(int n = 0; n < MVL; n++)
	{
		if(lpObj->VpPlayer[n].state != 0)
		{
			if(lpObj->VpPlayer[n].number == aAddIndex)
			{
				if(lpObj->VpPlayer[n].type == aType)
				{
					return -1;
				}
			}
		}
		else
		{
			if(blank < 0)
			{
				blank = n;
			}
		}
	}

	if(blank < 0)
	{
		return -1;
	}


	lpObj->VpPlayer[blank].state = 1;
	lpObj->VpPlayer[blank].number = aAddIndex;
	lpObj->VpPlayer[blank].type = aType;
	lpObj->VPCount ++;

	if(lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
	{
		if(aType == 1)
		{
			if(lpObj->m_iCurrentAI != 0)
			{
				lpObj->m_Agro.SetAgro(aAddIndex,1000);
			}
		}
	}

	return blank;
}






int  ViewportAdd2(int aIndex, int aAddIndex, int aType)
{
	int blank = -1;
	int count = 0;
	LPOBJ lpObj = &gObj[aIndex];
	int MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	for(int n = 0; n < MVL; n++)
	{
		if(lpObj->VpPlayer2[n].state != 0)
		{
			if(lpObj->VpPlayer2[n].number == aAddIndex)
			{
				if(lpObj->VpPlayer2[n].type == aType)
				{
					return -1;
				}
			}
		}
		else
		{
			if(blank < 0)
			{
				blank = n;
			}
		}
	}

	if(blank < 0)
	{
		return -1;
	}


	lpObj->VpPlayer2[blank].state = 1;
	lpObj->VpPlayer2[blank].number = aAddIndex;
	lpObj->VpPlayer2[blank].type = aType;
	lpObj->VPCount2 ++;

	return blank;
}






void ViewportDel(short aIndex, int aDelIndex)
{
	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(gObjIsConnected(aDelIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	int MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	for(int n = 0; n < MVL; n++)
	{
		if(lpObj->VpPlayer[n].state == 1 || lpObj->VpPlayer[n].state == 2)
		{
			if(lpObj->VpPlayer[n].number == aDelIndex)
			{
				lpObj->VpPlayer[n].state = 0;
				lpObj->VpPlayer[n].number = -1;
				lpObj->VPCount --;
			}
		}
	}
}






void Viewport2Del(short aIndex, int aDelIndex)
{
	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(gObjIsConnected(aDelIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	int MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	for(int n = 0; n < MVL; n++)
	{
		if(lpObj->VpPlayer2[n].state == 1 || lpObj->VpPlayer2[n].state == 2)
		{
			if(lpObj->VpPlayer2[n].number == aDelIndex)
			{
				lpObj->VpPlayer2[n].state = 0;
				lpObj->VpPlayer2[n].number = -1;
				lpObj->VPCount2 --;
			}
		}
	}
}






void gObjViewportAllDel(short aIndex)
{
	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	int MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	for(int n = 0; n < MVL; n++)
	{
		if(lpObj->VpPlayer[n].state == 1 || lpObj->VpPlayer[n].state == 2)
		{
			Viewport2Del(lpObj->VpPlayer[n].number,aIndex);
		}

		if(lpObj->VpPlayer2[n].state == 1 || lpObj->VpPlayer2[n].state == 2)
		{
			ViewportDel(lpObj->VpPlayer2[n].number,aIndex);
		}
	}
}






void gObjViewportListDestroy(short aIndex)
{
	short tObjNum;
	int n;
	LPOBJ lpObj;
	int MVL;

	if(OBJMAX_RANGE(aIndex) == 0)
	{
		return;
	}

	if(gObj[aIndex].Connected < PLAYER_PLAYING)
	{
		return;
	}

	lpObj = &gObj[aIndex];
	MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	for(n = 0; n < MVL; n++)
	{
		int mapnum;
		if(lpObj->VpPlayer[n].state == 1 || lpObj->VpPlayer[n].state == 2)
		{
			tObjNum = lpObj->VpPlayer[n].number;

			switch(lpObj->VpPlayer[n].type)
			{
			case 5:
				mapnum = lpObj->MapNumber;

				if(MapC[mapnum].m_cItem[tObjNum].IsItem() == 1)
				{
					if(MapC[mapnum].m_cItem[tObjNum].m_State == 8)
					{
						lpObj->VpPlayer[n].state = 3;
					}
					else
					{
						if(gObjCheckViewport(aIndex,MapC[mapnum].m_cItem[tObjNum].px,MapC[mapnum].m_cItem[tObjNum].py) == 0)
						{
							lpObj->VpPlayer[n].state = 3;
						}
					}
				}
				else
				{
					if(MapC[mapnum].m_cItem[tObjNum].live == 0 || MapC[mapnum].m_cItem[tObjNum].Give == 0)
					{
						lpObj->VpPlayer[n].state = 3;
					}
					else
					{
						lpObj->VpPlayer[n].state = 3;
					}
				}
				break;
			default : 
				if(gObj[tObjNum].m_State == 1)
				{
					if(gObj[tObjNum].Live == 0)
					{
						lpObj->VpPlayer[n].state = 3;
					}
				}

				if(gObj[tObjNum].Connected  == PLAYER_EMPTY ||
					gObj[tObjNum].m_State == 8 ||
					gObj[tObjNum].Teleport != 0 ||
					gObj[tObjNum].m_State == 32)
				{
					lpObj->VpPlayer[n].state = 3;
				}
				else
				{
					if(gObjCheckViewport(aIndex,gObj[tObjNum].X,gObj[tObjNum].Y) == 0)
					{
						lpObj->VpPlayer[n].state = 3;

						if(lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
						{
							if(gObj[tObjNum].Type == OBJ_USER)
							{
								if(lpObj->m_iCurrentAI != 0)
								{
									lpObj->m_Agro.DelAgro(tObjNum);
								}
							}
						}
					}
				}
				break;
			}
		}

		if(lpObj->VpPlayer2[n].state == 1 || lpObj->VpPlayer2[n].state == 2)
		{
			tObjNum = lpObj->VpPlayer2[n].number;

			if(gObj[tObjNum].Connected < PLAYER_PLAYING)
			{
				lpObj->VpPlayer2[n].state = 0;
				lpObj->VpPlayer2[n].number = -1;
				lpObj->VPCount2 --;
			}
			else
			{
				if(gObjCheckViewport(tObjNum,lpObj->X,lpObj->Y) == 0)
				{
					lpObj->VpPlayer2[n].state = 0;
					lpObj->VpPlayer2[n].number = -1;
					lpObj->VPCount2 --;
				}
			}
		}
	}
}

struct PMSG_ANS_PSHOP_VIEWPORT_NOTIFY {
	struct PWMSG_HEAD2 h;
	unsigned char btCount;
};

struct PMSG_PSHOP_VIEWPORT_NOTIFY {
  unsigned char btNumberH;
  unsigned char btNumberL;
  char szPShopText[36];
};

void PShop_ViewportListRegenarate(short aIndex)
{
	if(gObjIsConnected(aIndex) == 0)
	{
		return ;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJ_USER)
	{
		return;
	}

	if(lpObj->RegenOk > 0)
	{
		return;
	}

	if(lpObj->CloseCount > -1)
	{
		return;
	}

	BYTE cBUFFER[6000];
	int iVpOpenCount = 0;
	int iVpAddCount = 0;

	PMSG_ANS_PSHOP_VIEWPORT_NOTIFY * lpMsg = (PMSG_ANS_PSHOP_VIEWPORT_NOTIFY *)&cBUFFER;
	PMSG_PSHOP_VIEWPORT_NOTIFY * lpMsgBody = (PMSG_PSHOP_VIEWPORT_NOTIFY *)&cBUFFER[6];

	int iPShopOpenIndex[MAX_VIEWPORT];
	int n;

	if(lpObj->m_bPShopRedrawAbs != 0)
	{
		memset(lpObj->m_iVpPShopPlayer,0x00,sizeof(lpObj->m_iVpPShopPlayer));
		lpObj->m_wVpPShopPlayerCount = 0;
		lpObj->m_bPShopRedrawAbs = 0;
	}

	for(n = 0; n < MAX_VIEWPORT; n++)
	{
		if(lpObj->VpPlayer[n].state == 2 && lpObj->VpPlayer[n].type == OBJ_USER && gObj[lpObj->VpPlayer[n].number].m_bPShopOpen == 1)
		{
			iPShopOpenIndex[iVpOpenCount] = lpObj->VpPlayer[n].number;
			iVpOpenCount++;

			if(lpObj->m_wVpPShopPlayerCount > 0 && lpObj->m_wVpPShopPlayerCount <= MAX_VIEWPORT)
			{
				int found = 0;

				for(int i = 0; i < lpObj->m_wVpPShopPlayerCount; i++)
				{
					if(lpObj->m_iVpPShopPlayer[i] == lpObj->VpPlayer[n].number)
					{
						found = 1;
						break;
					}
				}

				if(found == 0)
				{
					lpMsgBody[iVpAddCount].btNumberL = SET_NUMBERL(WORD(lpObj->VpPlayer[n].number));
					lpMsgBody[iVpAddCount].btNumberH = SET_NUMBERH(WORD(lpObj->VpPlayer[n].number));
					memcpy(lpMsgBody[iVpAddCount].szPShopText,gObj[lpObj->VpPlayer[n].number].m_szPShopText,sizeof(gObj[lpObj->VpPlayer[n].number].m_szPShopText));
					iVpAddCount++;
				}
			}
			else if(lpObj->m_wVpPShopPlayerCount == 0)
			{
				lpMsgBody[iVpAddCount].btNumberL = SET_NUMBERL(WORD(lpObj->VpPlayer[n].number));
				lpMsgBody[iVpAddCount].btNumberH = SET_NUMBERH(WORD(lpObj->VpPlayer[n].number));
				memcpy(lpMsgBody[iVpAddCount].szPShopText,gObj[lpObj->VpPlayer[n].number].m_szPShopText,sizeof(gObj[lpObj->VpPlayer[n].number].m_szPShopText));
				iVpAddCount++;
			}
			else
			{
				LogAddTD("[PShop] ERROR : lpObj->m_wVpPShopPlayerCount is OUT of BOUND : %d",lpObj->m_wVpPShopPlayerCount);
			}
		}
	}

	memset(lpObj->m_iVpPShopPlayer,0x00,sizeof(lpObj->m_iVpPShopPlayer));
	lpObj->m_wVpPShopPlayerCount = 0;

	if(iVpOpenCount > 0 && iVpOpenCount <= MAX_VIEWPORT)
	{
		for(n = 0; n < iVpOpenCount; n++)
		{
			lpObj->m_iVpPShopPlayer[n] = iPShopOpenIndex[n];
		}
		lpObj->m_wVpPShopPlayerCount = iVpOpenCount;
	}
	else if(iVpOpenCount != 0)
	{
		LogAddTD("[PShop] ERROR : iVpOpenCount is OUT of BOUND: %d",iVpOpenCount);
	}

	if(iVpAddCount > 0 && iVpAddCount <= MAX_VIEWPORT)
	{
		lpMsg->btCount = iVpAddCount;
		PHeadSubSetW((unsigned char *)lpMsg,0x3F,0,sizeof(PMSG_ANS_PSHOP_VIEWPORT_NOTIFY)+sizeof(PMSG_PSHOP_VIEWPORT_NOTIFY)*iVpAddCount);
		DataSend(aIndex,(unsigned char *)lpMsg,((lpMsg->h.sizeL & 0xFF)& 0xFF | ((lpMsg->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);
	}
	else if(iVpAddCount != 0)
	{
		LogAddTD("[PShop] ERROR : iVpAddCount is OUT of BOUND: %d",iVpAddCount);
	}

	if(gObj[aIndex].m_bPShopWantDeal == 1)
	{
		if(gObjIsConnected(gObj[aIndex].m_iPShopDealerIndex) != 0)
		{
			if(gObj[gObj[aIndex].m_iPShopDealerIndex].Type == OBJ_USER)
			{
				if(gObj[gObj[aIndex].m_iPShopDealerIndex].m_bPShopOpen == 0)
				{
					gObj[aIndex].m_bPShopWantDeal = 0;
					gObj[aIndex].m_iPShopDealerIndex = -1;
					memset(gObj[aIndex].m_szPShopDealerName,0,sizeof(gObj[aIndex].m_szPShopDealerName));

					CGPShopAnsDealerClosedShop(aIndex,gObj[aIndex].m_iPShopDealerIndex);
				}
				else if(gObj[gObj[aIndex].m_iPShopDealerIndex].CloseCount >= 0 || gObj[aIndex].CloseCount >= 0)
				{
					gObj[aIndex].m_bPShopWantDeal = 0;
					gObj[aIndex].m_iPShopDealerIndex = -1;
					memset(gObj[aIndex].m_szPShopDealerName,0,sizeof(gObj[aIndex].m_szPShopDealerName));

					CGPShopAnsDealerClosedShop(aIndex,gObj[aIndex].m_iPShopDealerIndex);
				}
				else if(gObj[gObj[aIndex].m_iPShopDealerIndex].m_bPShopItemChange == 1)
				{
					CGPShopAnsBuyList(aIndex,gObj[aIndex].m_iPShopDealerIndex,1,1);
				}
			}
			else
			{
				gObj[aIndex].m_bPShopWantDeal = 0;
				gObj[aIndex].m_iPShopDealerIndex = -1;
				memset(gObj[aIndex].m_szPShopDealerName,0,sizeof(gObj[aIndex].m_szPShopDealerName));

				CGPShopAnsDealerClosedShop(aIndex,gObj[aIndex].m_iPShopDealerIndex);
			}
		}
		else
		{
			gObj[aIndex].m_bPShopWantDeal = 0;
			gObj[aIndex].m_iPShopDealerIndex = -1;
			memset(gObj[aIndex].m_szPShopDealerName,0,sizeof(gObj[aIndex].m_szPShopDealerName));

			CGPShopAnsDealerClosedShop(aIndex,gObj[aIndex].m_iPShopDealerIndex);
		}
	}
}

bool PShop_CheckInventoryEmpty(short aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	for(int i = MAIN_INVENTORY_SIZE; i < (INVENTORY_SIZE-1); i++)
	{
		if(lpObj->Inventory1[i].IsItem() == 1)
		{
			return false;
		}
	}
	return true;
}






void gObjStateSetCreate(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Connected < PLAYER_PLAYING)
	{
		return;
	}

	DWORD dwNowTick = GetTickCount();
	
	if(lpObj->DieRegen == 1)
	{
		if(dwNowTick - lpObj->RegenTime > lpObj->MaxRegenTime + 1000)
		{
			gObjClearBuffEffect(lpObj,CLEAR_TYPE_NON_PCS_ITEM_EFFECT);
			lpObj->DieRegen = 2;
			lpObj->m_State = 8;
		}
	}

	if(lpObj->Type == OBJ_USER)
	{
		if(lpObj->m_State == 8)
		{
			gObjReady4Relife(lpObj);
		}
		else
		{
			gObjCheckTimeOutValue(lpObj,(DWORD &)dwNowTick);
		}
	}

	if(lpObj->Teleport == 1)
	{
		if(lpObj->DieRegen != 0)
		{
			lpObj->Teleport = 0;
		}
		else if(GetTickCount() - lpObj->TeleportTime > 100)
		{
			lpObj->Teleport = 2;
		}
	}
	else if(lpObj->Teleport == 2)
	{
		lpObj->Teleport = 3;

		if(lpObj->Type == OBJ_USER)
		{
			lpObj->TX = lpObj->X;
			lpObj->TY = lpObj->Y;

			CreateFrustrum(lpObj->X,lpObj->Y,aIndex);
			GCTeleportSend(&gObj[aIndex],0,lpObj->MapNumber,lpObj->X,lpObj->Y,lpObj->Dir);
			gObjViewportListProtocolCreate(lpObj);

#if (GS_CASTLE == 1)
			if( lpObj->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				g_CastleSiege.NotifySelfCsJoinSide(aIndex);
				g_CastleSiege.NotifyCsSelfLeftTime(aIndex);
			}
#endif

			lpObj->Teleport = 0;
		}
		else if(lpObj->Type == OBJ_NPC && lpObj->Class == 229)
		{
			int x = lpObj->X;
			int y = lpObj->Y;
			
			lpObj->X = x;
			lpObj->Y = y;

			lpObj->TX = lpObj->X;
			lpObj->TY = lpObj->Y;

			CreateFrustrum(lpObj->X,lpObj->Y,aIndex);

			lpObj->m_State = 1;
			lpObj->PathStartEnd = 0;
		}
		else if(lpObj->Type == OBJ_MONSTER)
		{
			lpObj->TX = lpObj->X;
			lpObj->TY = lpObj->Y;

			CreateFrustrum(lpObj->X,lpObj->Y,aIndex);

			gObjViewportListCreate(lpObj->m_Index);
			gObjViewportListProtocolCreate(lpObj);

			lpObj->Teleport = 0;
			lpObj->PathStartEnd = 0;
			lpObj->PathCur = 0;
			lpObj->PathCount = 0;
		}
		else 
		{
			int x,y;

			if(lpObj->m_RecallMon >= 0)
			{
				x = lpObj->X;
				y = lpObj->Y;
			}
			else
			{
				x = lpObj->X + rand()%6 - 3;
				y = lpObj->Y + rand()%6 - 3;
			}

			lpObj->X = x;
			lpObj->Y = y;

			lpObj->TX = lpObj->X;
			lpObj->TY = lpObj->Y;

			if(lpObj->m_RecallMon >= 0)
			{
				gObjViewportListProtocolCreate(lpObj);
				lpObj->PathStartEnd = 0;
			}
		}
	}

	if(lpObj->RegenOk == 2 && lpObj->Type == OBJ_USER)
	{
		lpObj->m_State = 1;
		lpObj->RegenOk = 3;
		lpObj->X = lpObj->RegenMapX;
		lpObj->Y = lpObj->RegenMapY;
		lpObj->MapNumber = lpObj->RegenMapNumber;

		lpObj->TX = lpObj->X;
		lpObj->TY = lpObj->Y;

		CreateFrustrum(lpObj->X,lpObj->Y,aIndex);
#if( __4GAMERS__ == 0 )
		if(DragonEvent->GetState() > 0)
		{
			if(DragonEvent->GetMapNumber() == lpObj->MapNumber)
			{
				GCMapEventStateSend(lpObj->MapNumber,1,1);
			}
		}
#endif
#if (GS_CASTLE == 1)
		if( lpObj->MapNumber == MAP_INDEX_CASTLESIEGE )
		{
			GCAnsCsNotifyStart(lpObj->m_Index,(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE) ? TRUE : FALSE);

			if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				g_CastleSiege.NotifySelfCsJoinSide(lpObj->m_Index);
				g_CastleSiege.NotifyCsSelfLeftTime(lpObj->m_Index);
			}
		}
#endif

		gEledoradoEvent.CheckGoldDercon(lpObj->MapNumber);

	}
}

struct PMSG_CHARREGEN {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE MapX;
	BYTE MapY;
	BYTE MapNumber;
	BYTE Dir;
	WORD Life;
	WORD Mana;
	WORD wShield;
	WORD BP;
	BYTE Exp[8];
	DWORD Money;

#if (FIX_HP == 1)
	int iLife;
	int iMana;
	int iShield;
#endif
};

void gObjSetState()
{
	int n;
	LPOBJ lpObj;

#ifdef __NOVUS__
	DWORD SpotKey = 0;
#endif

	for(n = 0; n < OBJMAX; n++)
	{
		lpObj = &gObj[n];
		// ----
		if(lpObj->Connected > PLAYER_LOGGED) {
#ifdef __NOVUS__
			if( g_SummonSpot.CheckSpotState(lpObj) )
			{
				continue;
			}
#endif
			if(lpObj->m_State == 1)
			{
				lpObj->m_State = 2;

				if(lpObj->RegenOk == 3)
				{
					lpObj->RegenOk = 0;
				}
			}
	
			if(lpObj->Teleport == 3)
			{
				lpObj->Teleport = 0;
			}
	
			if(lpObj->DieRegen == 2)
			{
				if( DG_MAP_RANGE(lpObj->MapNumber) )
				{
					if(lpObj->Type == OBJ_NPC )
					{
#if (IMPERIAL_FORT_DEBUG==1)
						IMPERIAL_TEST_LOG.Output("[DG DEBUG] MonsterDelete(%d): [%d (%d/%d)] %d, %d",
							lpObj->Class,lpObj->MapNumber,lpObj->X,lpObj->Y,lpObj->m_State,lpObj->DieRegen);
#else
						LogAddC(4,"[DG DEBUG] MonsterDelete(%d): [%d (%d/%d)] %d, %d",
							lpObj->Class,lpObj->MapNumber,lpObj->X,lpObj->Y,lpObj->m_State,lpObj->DieRegen);
#endif
						gObjDel(lpObj->m_Index);
						continue;
					}
				}

				int mapnum = lpObj->MapNumber;
				int x = lpObj->X;
				int y = lpObj->Y;

				lpObj->DeathX = lpObj->X;
				lpObj->DeathY = lpObj->Y;
				lpObj->DeathMap = lpObj->MapNumber;

				lpObj->m_bAttackerKilled = 0;
	
				if(DS_MAP_RANGE(lpObj->MapNumber))
				{
					if(lpObj->Type == OBJ_MONSTER)
					{
						g_DevilSquare.gDevilSquareMonsterRegen(lpObj);
						continue;
					}
				}

#ifdef __NOVUS__
			//	if( lpObj->m_SummonTick > 0 )
			//	{
			//		g_SummonSpot.RegenProc(lpObj);
			//		continue;
			//	}
#endif

#if (GS_CASTLE == 0 || GS_SPEC == 1)
				if(lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS && lpObj->Type == OBJ_MONSTER)
				{
					continue;
				}
#endif
				if(lpObj->m_iCurrentAI != 0 && lpObj->m_iRegenType != 0)
				{
					continue;
				}
	
#if (GS_CASTLE == 1)

				if(lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
				{
					if(g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_READY
						&& lpObj->Type >= OBJ_MONSTER)
					{
						continue;
					}
				}
#endif
				
				if( (lpObj->Class >= 504 && lpObj->Class <= 521 ) ||
					(lpObj->Class >= 523 && lpObj->Class <= 528 ) )
				{
					continue;
				}

				if(lpObj->m_bIsInMonsterHerd != 0)
				{
					if(lpObj->m_lpMonsterHerd != 0)
					{
						lpObj->m_lpMonsterHerd->MonsterRegenAction(lpObj);
					}
					continue;
				}
	
				if(BC_MAP_RANGE(lpObj->MapNumber) && lpObj->Type >= OBJ_MONSTER)
				{
					int iBridge = g_BloodCastle.GetBridgeLevel(lpObj->MapNumber);
					if(lpObj->Class == 131)
					{
						g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_bCASTLE_DOOR_LIVE = 0;
						g_BloodCastle.m_BridgeData[lpObj->m_cBloodCastleIndex].m_bBC_DOOR_TERMINATE_COMPLETE = 1;
						g_BloodCastle.m_BridgeData[iBridge].m_iBC_BOSS_MONSTER_MAX_COUNT = g_BloodCastle.GetCurrentLiveUserCount(iBridge) * 2;
	
						g_BloodCastle.m_BridgeData[iBridge].m_iBC_BOSS_MONSTER_KILL_COUNT = 0;
	
						if(g_BloodCastle.m_BridgeData[iBridge].m_iBC_BOSS_MONSTER_MAX_COUNT > 10)
						{
							g_BloodCastle.m_BridgeData[iBridge].m_iBC_BOSS_MONSTER_MAX_COUNT = 10;
						}
	
						g_BloodCastle.SendCastleDoorBlockInfo(lpObj->m_cBloodCastleIndex,0);
						g_BloodCastle.ReleaseCastleDoor(lpObj->m_cBloodCastleIndex);
						g_BloodCastle.SetCastleBlockInfo(lpObj->m_cBloodCastleIndex,1003);
	
						if(g_BloodCastle.GetCurrentState(lpObj->m_cBloodCastleIndex) == 2)
						{
							g_BloodCastle.SetBossMonster(lpObj->m_cBloodCastleIndex);
						}
					}
	
					if(BC_STATUE_RANGE(lpObj->Class - 132) && BC_MAP_RANGE(lpObj->MapNumber))
					{
						int TopHitUser = gObjMonsterTopHitDamageUser(lpObj);
						int level = lpObj->Class - 132;
	
						int type = ItemGetNumberMake(13,19);

						int MapNumber;

						switch( lpObj->MapNumber )
						{
						case 11:	MapNumber = 0xF6;	break;
						case 12:	MapNumber = 0xF7;	break;
						case 13:	MapNumber = 0xF8;	break;
						case 14:	MapNumber = 0xF9;	break;
						case 15:	MapNumber = 0xFA;	break;
						case 16:	MapNumber = 0xFB;	break;
						case 17:	MapNumber = 0xFC;	break;
						case 52:	MapNumber = 0xFD;	break;
						}
	
						ItemSerialCreateSend(lpObj->m_Index,MapNumber,lpObj->X,lpObj->Y,type,level,0,0,0,0,TopHitUser,0,0);
	
						g_BloodCastle.m_BridgeData[iBridge].m_btBC_QUEST_ITEM_NUMBER = level;
	
						LogAddTD("[Blood Castle] (%d) Ultimate Weapon Dropped -> (%s)",
							iBridge+1, (level !=0)?((level==1)?"Sword":"Crossbow"):"Staff");
					}

					if(lpObj->m_PosNum == -1)
					{
						gObjDel(lpObj->m_Index);
						continue;
					}

					if(lpObj->Type == OBJ_MONSTER)
					{
						if(lpObj->Class != 131 && BC_STATUE_RANGE(lpObj->Class - 132)== 0)
						{
							g_BloodCastle.ChangeMonsterState(iBridge,lpObj->m_Index);
						}
					}
				}

				if( IT_MAP_RANGE(lpObj->MapNumber) )
				{
					if( lpObj->Class == 380 ||
						(lpObj->Class >= 386 && lpObj->Class <= 403) )
					{
						if(lpObj->m_PosNum == -1)
						{
							continue;
						}
					}
				}

				if(lpObj->m_Attribute == 60)
				{
					gObjDel(lpObj->m_Index);
					continue;
				}

#if (GS_CASTLE == 1)

				if(lpObj->m_Attribute == 61)
				{
					gObjDel(lpObj->m_Index);
					continue;
				}

				if(lpObj->m_Attribute == 62)
				{
					switch(lpObj->Class)
					{
					case 300:
					case 301:
					case 302:
					case 303:
						gObjDel(lpObj->m_Index);
						continue;
					}

					if(lpObj->m_dwLastCheckTick + 600000 < GetTickCount())
					{
						gObjDel(lpObj->m_Index);
						continue;
					}
				}
#endif

				if(lpObj->Type >= OBJ_MONSTER)
				{
					if(lpObj->Class == 287 || lpObj->Class == 286)
					{
						g_CsNPC_Mercenary.DeleteMercenary(lpObj->m_Index);
						gObjDel(lpObj->m_Index);
						continue;
					}
				}

#if (GS_CASTLE == 1)

				if(lpObj->Type >= OBJ_MONSTER && lpObj->Class == 278)
				{
					g_CsNPC_LifeStone.DeleteLifeStone(lpObj->m_Index);
					gObjDel(lpObj->m_Index);
					continue;
				}
#endif

				int State = 1;

				if(lpObj->Type >= OBJ_MONSTER)
				{
#if( __4GAMERS__ == 0 )
					if(lpObj->Class == 44 && DragonEvent->GetState() == 0)
					{
						State = 0;
					}
#endif
					if(lpObj->Class == 53	||
						lpObj->Class == 54	||
						lpObj->Class == 55	||
						lpObj->Class == 56)
					{
						State = 0;
						lpObj->Life = lpObj->MaxLife;
						lpObj->DieRegen = 0;
					}
	
					if(lpObj->Class >= 78 && lpObj->Class <= 83)
					{
						State = 0;
						lpObj->Life = lpObj->MaxLife;
						lpObj->DieRegen = 0;
					}

					if(lpObj->Class >= 493 && lpObj->Class <= 502)
					{
						State = 0;
						lpObj->Life = lpObj->MaxLife;
						lpObj->DieRegen = 0;
					}
	
					if(lpObj->Class == 77)
					{
						int i_shield = lpObj->m_SkyBossMonSheildLinkIndex;
	
						if(i_shield != -1)
						{	
							gObj[i_shield].Life = gObj[i_shield].MaxLife;
						}	
					}

					if( lpObj->Class == 560 || lpObj->Class == 561 )	//Medusa Added Leee
					{
						State = 0;
						lpObj->DieRegen = 0;
					}
				}

				PMSG_CHARREGEN pMsg;

				if(State != 0)
				{
					lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
					lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
					lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;

					lpObj->Live = 1;

					gObjRemoveBuffEffect(lpObj,BUFF_POISON);
					gObjRemoveBuffEffect(lpObj,BUFF_ICE);
					gObjRemoveBuffEffect(lpObj,BUFF_ICEARROW);

					lpObj->m_ViewState = 0;
					lpObj->Teleport = 0;

					for(int i = 0; i < MAX_SELF_DEFENSE; i++)
					{
						lpObj->SelfDefenseTime[i] = 0;
					}

					gObjTimeCheckSelfDefense(lpObj);
					lpObj->MySelfDefenseTime = FALSE;
					gObjClearBuffEffect(lpObj,CLEAR_TYPE_NON_PCS_ITEM_EFFECT);
					gObjClearViewport(lpObj);

					if(lpObj->Type == OBJ_USER)
					{
						if(g_iUseCharacterAutoRecuperationSystem != 0 && lpObj->Level <= g_iCharacterRecuperationMaxLevel)
						{
							lpObj->m_iAutoRecuperationTime = GetTickCount();
						}
						else
						{
							lpObj->m_iAutoRecuperationTime = 0;
						}

						lpObj->dwShieldAutoRefillTimer = GetTickCount();
						gDarkSpirit[lpObj->m_Index].SetMode(CDarkSpirit::ePetItem_Mode::PetItem_Mode_Normal,-1);

						if(lpObj->KillerType == 3)
						{
#if (ENABLETEST_NEWPVP == 1)
							if( !g_NewPVP.IsDuel(*lpObj) )
							{
								MapC[lpObj->MapNumber].GetRandomLengthPos((BYTE &)lpObj->X,(BYTE &)lpObj->Y,18);
							}
#else
							MapC[lpObj->MapNumber].GetRandomLengthPos((BYTE &)lpObj->X,(BYTE &)lpObj->Y,18);
#endif
						}
						else if(lpObj->KillerType == 2)
						{
							int w_Type = 0;

							if(lpObj->lpGuild != 0)
							{
								if(lpObj->lpGuild->WarType == 1)
								{
									w_Type = 1;
								}
							}

							if(w_Type == 0)
							{
								MapC[lpObj->MapNumber].GetRandomLengthPos(lpObj->X,lpObj->Y,18);
							}
							else if(lpObj->lpGuild->WarState)
							{
								int BattleGroundIndex = lpObj->lpGuild->BattleGroundIndex;
								gBattleGetTeamPosition(BattleGroundIndex,lpObj->lpGuild->BattleTeamCode,(short &)lpObj->X,(short &)lpObj->Y);
							}
						}
						else if(lpObj->MapNumber == MAP_INDEX_RORENCIA || lpObj->MapNumber == MAP_INDEX_DUNGEON)
						{
							lpObj->MapNumber = MAP_INDEX_RORENCIA;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
						}
						else if(DS_MAP_RANGE(lpObj->MapNumber))
						{
							g_DevilSquare.DieProcDevilSquare(lpObj);
							lpObj->MapNumber = MAP_INDEX_NORIA;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
						}
						else if(lpObj->MapNumber == MAP_INDEX_ICARUS)
						{
							lpObj->MapNumber = MAP_INDEX_DEVIAS;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
						}
						else if(BC_MAP_RANGE(lpObj->MapNumber))
						{
							lpObj->MapNumber = MAP_INDEX_DEVIAS;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
						}
						else if(CC_MAP_RANGE(lpObj->MapNumber))
						{
							lpObj->MapNumber = MAP_INDEX_DEVIAS;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
						}
						else if(KALIMA_MAP_RANGE(lpObj->MapNumber))
						{
							lpObj->MapNumber = MAP_INDEX_DEVIAS;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
						}
#if (GS_CASTLE == 1)
						else if(lpObj->MapNumber == MAP_INDEX_CASTLEHUNTZONE)
						{
							int mgt = 106;
							BYTE x,y;
							short level;
							BYTE map = lpObj->MapNumber,dir;
							int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

							if(result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}
						else if(lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
						{
							if(g_CsNPC_LifeStone.SetReSpawnUserXY(lpObj->m_Index) == FALSE)
							{
								int mgt = 100;
								BYTE x,y;
								short level;

								if(lpObj->m_btCsJoinSide == 1)
								{
									mgt = 105;
								}
								else
								{
									mgt = 100;
								}

								BYTE map = lpObj->MapNumber,dir;

								int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

								if(result >= 0)
								{
									lpObj->MapNumber = map;
									lpObj->X = x;
									lpObj->Y = y;
								}
							}
						}
#endif
						else if(lpObj->MapNumber == MAP_INDEX_AIDA)
						{
							MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
						}
						else if( lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
						{
							if(g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_START)
							{
								int mgt = 118;
								BYTE x,y;
								short level;
								BYTE map = lpObj->MapNumber,dir;
								int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);
	
								if(result >= 0)
								{
									lpObj->MapNumber = map;
									lpObj->X = x;
									lpObj->Y = y;
								}
								else
								{
									MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
								}
							}
							else
							{
								MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
							}
						}
#if (GS_CASTLE == 0 || GS_SPEC == 1)
						else if(lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS)
						{
							int mgt = 137;
							BYTE x,y;
							short level;
							BYTE map = lpObj->MapNumber,dir;
							int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

							if(result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}

							g_KanturuBattleUserMng.DeleteUserData(lpObj->m_Index);
						}
						else if(lpObj->MapNumber == MAP_INDEX_KANTURU1)
						{
							int mgt = 138;
							BYTE x,y;
							short level;
							BYTE map = lpObj->MapNumber,dir;

							int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

							if(result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}
						else if(lpObj->MapNumber == MAP_INDEX_KANTURU2)
						{
							int mgt = 139;
							BYTE x,y;
							short level;
							BYTE map = lpObj->MapNumber,dir;

							int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

							if(result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}
#endif
						else if( lpObj->MapNumber == MAP_INDEX_BARRACKS ||
								 lpObj->MapNumber == MAP_INDEX_REFUGE )
						{
							int mgt = 258;
							BYTE x,y;
							short level;
							BYTE map = lpObj->MapNumber,dir;
							int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

							if(result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}
						else if( IT_MAP_RANGE(lpObj->MapNumber) )
						{
							BYTE Team = g_IllusionTemple.GetTeam(lpObj->MapNumber,lpObj->m_Index);

							if( Team == (BYTE)-1)
							{
								lpObj->MapNumber = MAP_INDEX_DEVIAS;
								MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
							}
							else if( Team == 0 )
							{
								int mgt = lpObj->MapNumber + 103;
								BYTE x,y;
								short level;
								BYTE map = lpObj->MapNumber,dir;
								int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

								if(result >= 0)
								{
									lpObj->MapNumber = map;
									lpObj->X = x;
									lpObj->Y = y;
								}
								g_IllusionTemple.RegenUser(lpObj);
							}
							else if( Team == 1 )
							{
								int mgt = lpObj->MapNumber + 109;
								BYTE x,y;
								short level;
								BYTE map = lpObj->MapNumber,dir;
								int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

								if(result >= 0)
								{
									lpObj->MapNumber = map;
									lpObj->X = x;
									lpObj->Y = y;
								}

								g_IllusionTemple.RegenUser(lpObj);
							}
							else
							{
								lpObj->MapNumber = MAP_INDEX_DEVIAS;
								MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
							}
						}
						else if( lpObj->MapNumber == MAP_INDEX_SWAMPOFCALMNESS )
						{
							int mgt = 273;
							BYTE x,y;
							short level;
							BYTE map = lpObj->MapNumber,dir;
							int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

							if(result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}
						else if( lpObj->MapNumber == MAP_INDEX_RAKLION ||
								 lpObj->MapNumber == MAP_INDEX_RAKLIONBOSS )
						{
							int mgt = 286;
							BYTE x,y;
							short level;
							BYTE map = lpObj->MapNumber,dir;
							int result = gGateC.GetGate(mgt,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

							if(result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}
						else if( DG_MAP_RANGE(lpObj->MapNumber) )
						{
							lpObj->MapNumber = MAP_INDEX_ELBELAND;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
						}
						else if( IF_MAP_RANGE(lpObj->MapNumber) )
						{
							BYTE x,y;
							short level;
							BYTE map = lpObj->MapNumber,dir;
							int result = gGateC.GetGate(22,x,y,(BYTE &)map,(BYTE &)dir,(short &)level);

							if(result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}

							g_ImperialGuardian.GCMissionFailUseDie(lpObj->m_Index);
						}
						else
						{
							MapC[lpObj->MapNumber].GetMapPos(lpObj->X,lpObj->Y);
						}

						short m_Result = g_MapServerManager.CheckMoveMapSvr(lpObj->m_Index,lpObj->MapNumber,lpObj->m_sPrevMapSvrCode);

						if(m_Result != gGameServerCode )
						{
							if(m_Result == -1)
							{
								LogAddC(2,"[MapServerMng] Map Server Move Fail : CheckMoveMapSvr() == -1 [%s][%s] (%d)",lpObj->AccountID,lpObj->Name,lpObj->m_Index);
								continue;
							}

							int _map = lpObj->MapNumber;
							int _x = lpObj->X;
							int _y = lpObj->Y;

							lpObj->MapNumber = mapnum;
							lpObj->X = x;
							lpObj->Y = y;

							GJReqMapSvrMove(lpObj->m_Index,m_Result,_map,_x,_y);
							LogAddTD("[MapServerMng] Request to Move Map Server : (%d) - [%s][%s] (%d)",m_Result,lpObj->AccountID,lpObj->Name,lpObj->m_Index);
							continue;
						}					

						lpObj->TX = lpObj->X;
						lpObj->TY = lpObj->Y;

						CreateFrustrum(lpObj->X,lpObj->Y,n);

						pMsg.h.c = 0xC3;
						pMsg.h.headcode = 0xF3;
						pMsg.h.size = sizeof( pMsg );
						pMsg.subcode = 4;
						pMsg.Dir = lpObj->Dir;
						pMsg.MapX = lpObj->X;
						pMsg.MapY = lpObj->Y;
						pMsg.Life = (WORD)lpObj->Life;
						pMsg.Mana = (WORD)lpObj->Mana;

						__int64 Exp = lpObj->Experience;

						if( g_MasterExp.IsEnabled(lpObj) )
						{
							Exp = lpObj->MasterExperience;
						}
						else
						{
							Exp = lpObj->Experience;
						}

						pMsg.Exp[0] = DWORD((DWORD(Exp >> 32) >> 16) & 0xFFFF) >> 8;
						pMsg.Exp[1] = DWORD((DWORD(Exp >> 32) >> 16) & 0xFFFF) & 0xFF;
						pMsg.Exp[2] = DWORD((DWORD(Exp >> 32) & 0xFFFF) & 0xFFFF) >> 8;
						pMsg.Exp[3] = DWORD((DWORD(Exp >> 32) & 0xFFFF) & 0xFFFF) & 0xFF;

						pMsg.Exp[4] = DWORD((DWORD(Exp & 0xFFFFFFFF) >> 16) & 0xFFFF) >> 8;
						pMsg.Exp[5] = DWORD((DWORD(Exp & 0xFFFFFFFF) >> 16) & 0xFFFF) & 0xFF;
						pMsg.Exp[6] = DWORD((DWORD(Exp & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) >> 8;
						pMsg.Exp[7] = DWORD((DWORD(Exp & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) & 0xFF;

						pMsg.MapNumber = lpObj->MapNumber;
						pMsg.Money = lpObj->Money;

						pMsg.wShield = lpObj->iShield;
						pMsg.BP = lpObj->BP;

#if (FIX_HP == 1)
						pMsg.iLife = lpObj->Life;
						pMsg.iMana = lpObj->Mana;
						pMsg.iShield = lpObj->iShield;
#endif

						lpObj->PathCount = 0;

						DataSend(n,(unsigned char *)&pMsg,pMsg.h.size);

						gObjViewportListProtocolCreate(lpObj);
						g_Minimap.SendNpcCoordinate(n, lpObj->MapNumber, 0);
						g_Minimap.SendPortalCoordinate(n, lpObj->MapNumber);

#if( __4GAMERS__ == 0 )
						if(DragonEvent->GetState())
						{
							if(DragonEvent->GetMapNumber() == lpObj->MapNumber)
							{
								GCMapEventStateSend(lpObj->MapNumber,1,1);
							}
						}
#endif
						gEledoradoEvent.CheckGoldDercon(lpObj->MapNumber);

						if( lpObj->Authority == 32 )
						{
							gObjAddBuffEffect(lpObj,BUFF_GM,0,0,0,0,-10);
						}

#if (GS_CASTLE == 1)
						if(lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
						{
							GCAnsCsNotifyStart(lpObj->m_Index,(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE) ? TRUE : FALSE);

							if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
							{
								g_CastleSiege.NotifySelfCsJoinSide(lpObj->m_Index);
								g_CastleSiege.NotifyCsSelfLeftTime(lpObj->m_Index);
							}
						}
#endif
					}
					else
					{
						if(gObjMonsterRegen(lpObj)==0)
						{
							continue;
						}
						CreateFrustrum(lpObj->X,lpObj->Y,n);

						if( IT_MAP_RANGE(lpObj->MapNumber) )
						{
							if( lpObj->Class == 380 )
							{
								LogAddTD("[Illusion Temple] (%d) Status Regen OK (%d: %d/%d)",
									lpObj->MapNumber -(MAP_INDEX_ILLUSIONTEMPLE1-1),lpObj->MapNumber,lpObj->X,lpObj->Y);
							}
						}
					}

					lpObj->DieRegen = 0;
					lpObj->m_State = 1;

					//g_MiniMap.SendMinimap(lpObj->m_Index);
				}
			}
		}
	}

	for(n = 0; n < MAX_NUMBER_MAP; n++)
	{
		MapC[n].StateSetDestroy();
	}

}



void gObjSecondProc()
{
	if(gDisconnect == 1)
		return;

	int n;
	LPOBJ lpObj;

	for(n = 0; n < OBJMAX;n++)
	{
		lpObj = &gObj[n];

		if(lpObj->Connected > PLAYER_LOGGED)
		{
			if(lpObj->m_bMapSvrMoveQuit == 1)
			{
				if(GetTickCount() - lpObj->m_dwMapSvrQuitTick > 30000)
				{
					gObjDel(lpObj->m_Index);
					continue;
				}
			}

			gObjSkillUseProc(lpObj);
			gObjCheckBuffEffectList(lpObj);
			
#if (ENABLETEST_MUUN == 1)
			if (lpObj->Type == 1) {
				g_CMuunSystem.CheckMuunItemPeriodData(lpObj);
			}
#endif
			if(lpObj->Type == OBJ_NPC && lpObj->Class == 229)
			{
				gQeustNpcTeleport.Run(n);
			}

			/*if(lpObj->Type == OBJ_NPC && lpObj->Class == 568)
			{
				gQeustExpNpcTeleport.Run(n);
			}*/

			if(lpObj->Type == OBJ_MONSTER)
			{
				if(lpObj->Class == 275)
				{
					gObjRefillMonsterHP(lpObj,giKundunRefillHPSec);
				}
#if (GS_CASTLE == 1)

				if(lpObj->Class == 283)
				{
					g_CsNPC_GuardianStatue.GuardianStatueAct(lpObj->m_Index);
					continue;
				}

				if(lpObj->Class == 278)
				{
					g_CsNPC_LifeStone.LifeStoneAct(lpObj->m_Index);
					continue;
				}
#endif

				if(lpObj->Class == 285)
				{
					g_CsNPC_Guardian.GuardianAct(lpObj->m_Index);
					continue;
				}
#if (GS_CASTLE == 1)
				if(lpObj->Class == 288)
				{
					g_CsNPC_CannonTower.CannonTowerAct(lpObj->m_Index);
					continue;
				}

				g_Crywolf.CrywolfMonsterAct(lpObj->m_Index);
#endif
			}

			if(lpObj->Type == OBJ_NPC)
			{
#if (GS_CASTLE == 1)
				if(CRYWOLF_NPC_CLASS_RANGE(lpObj->Class) != FALSE)
				{
					g_Crywolf.CrywolfNpcAct(lpObj->m_Index);
				}

				if(lpObj->Class == 216)
				{
					g_CsNPC_CastleCrown.CastleCrownAct(lpObj->m_Index);
					continue;
				}

				if(CROWNSWITCH_CLASS_RANGE(lpObj->Class) != FALSE)
				{
					g_CsNPC_CastleCrownSwitch.CastleCrownSwitchAct(lpObj->m_Index);
					continue;
				}
#endif

				if(lpObj->Class == 221 || lpObj->Class == 222)
				{
					g_CsNPC_Weapon.WeaponAct(lpObj->m_Index);
				}
			}

			if(lpObj->Type == OBJ_USER)
			{
				gObjManaPotionFill(lpObj);
				gObjRestPotionFill(lpObj);

				gObjCheckXYMapTile(lpObj,3);

				if(lpObj->ChatLimitTime > 0)
				{
					lpObj->ChatLimitTimeSec++;

					if(lpObj->ChatLimitTimeSec > 60)
					{
						lpObj->ChatLimitTimeSec = 0;
						lpObj->ChatLimitTime--;

						if(lpObj->ChatLimitTime < 1)
						{
							lpObj->ChatLimitTime = 0;
							MsgOutput(lpObj->m_Index,lMsg.Get(1141));
						}
					}
				}

				if(g_iUseCharacterAutoRecuperationSystem != 0)
				{
					if(lpObj->Level <= g_iCharacterRecuperationMaxLevel)
					{
						gProcessAutoRecuperation(lpObj);
					}
				}

				if( lpObj->Type == OBJ_USER )
				{
					gObjShieldAutoRefill(lpObj);
				}
	
				if( lpObj->Type == OBJ_USER )
				{
					if(lpObj->m_LastTeleportTime > 0)
					{
						lpObj->m_LastTeleportTime--;
					}

				}
	
				gObjDelayLifeCheck(n);
				gObjSecondDurDown(lpObj);
	
				if(lpObj->PartyNumber >= 0)
				{
					gParty.PartyMemberLifeSend(lpObj->PartyNumber);

					if (lpObj->m_bSendMemberPos)
					{
						g_Minimap.GCPartyMemberPosition(n);
					}
					//	g_MiniMap.SendParty(lpObj->m_Index);
				}
	
				if(lpObj->m_ShopTime >= 1 && lpObj->m_ShopTime < 255)
				{
					lpObj->m_ShopTime++;
				}
	
				int BattleGround = 1;
				int m_BattleTimer = (int)gCheckBattleGroundTimer();
	
				if((lpObj->Authority & 2) == 2)
				{
					BattleGround = 0;
					GCTimeViewSend(lpObj->m_Index,m_BattleTimer);
				}
	
				if(lpObj->MapNumber == MAP_INDEX_BATTLESOCCER && BattleGround == 1)
				{
					BattleGround = 1;
	
					if(lpObj->lpGuild != 0)
					{
						if(lpObj->lpGuild->WarState != 0)
						{
							int CheckBattleGround = gCheckBattleGround(lpObj);
	
							if(CheckBattleGround != lpObj->lpGuild->BattleGroundIndex)
							{
								if(lpObj->lpGuild->WarType == 1)
								{
									BattleGround = 0;
								}
								else
								{
									BattleGround = 0;
								}
							}
							else
							{
								BattleGround = 0;
							}
						}
					}
					else
					{
						BattleGround = 1;
					}
	
					if(BattleGround != 0)
					{
						int CheckBattleGround = gCheckBattleGround(lpObj);
		
						if(CheckBattleGround >= 0)
						{
							gObjMoveGate(lpObj->m_Index,17);
						}	
					}
				}

				gObjMuBotProc(lpObj);
			}
		}

		if(lpObj->Connected >= PLAYER_LOGGED && 
				lpObj->Type == OBJ_USER &&
				lpObj->CloseCount > 0)
		{
			if(lpObj->CloseCount == 1)
			{
				if(lpObj->CloseType == 1)
				{
					if(gObjGameClose(lpObj->m_Index) == 1)
					{
						GCCloseMsgSend(lpObj->m_Index,1);
					}
				}
				else if(lpObj->CloseType == 0)
				{
					GCCloseMsgSend(lpObj->m_Index,0);
					CloseClient(lpObj->m_Index);
				}
				else if(lpObj->CloseType == 2)
				{
					GCCloseMsgSend(lpObj->m_Index,2);
				}
			}
			else
			{
				char Msg[128];
				wsprintf(Msg,lMsg.Get(1116),lpObj->CloseCount-1);
				GCServerMsgStringSend(Msg,lpObj->m_Index,1);
			}
			lpObj->CloseCount--;
		}

		if(lpObj->Connected > PLAYER_LOGGED &&
			lpObj->Type == OBJ_USER)
		{
			if(GetTickCount() - lpObj->AutoSaveTime > 600000)
			{
				GJSetCharacterInfo(lpObj,n,0);
				lpObj->AutoSaveTime = GetTickCount();
			}

			/*if(lpObj->CheckSumTime > 0 && GetTickCount() - lpObj->CheckSumTime > 10000)
			{
				LogAddTD("[%s][%s] CheckSumTime Error",lpObj->AccountID,lpObj->Name);
				CloseClient(n);
			}*/

			gObjPkDownTimeCheck(lpObj,1);
			gObjInterfaceTimeCheck(lpObj);
			gObjTimeCheckSelfDefense(lpObj);
			g_GoblinPointSystem.UserProc(lpObj);
		}

		if(lpObj->Connected == PLAYER_CONNECTED ||
			lpObj->Connected == PLAYER_LOGGED ||
			lpObj->Connected == PLAYER_PLAYING)
		{

#if (ENABLE_CUSTOM_OFFLINETRADE==1)
			if(lpObj->Type == OBJ_USER && !lpObj->bOffTrade)
#else
			if(lpObj->Type == OBJ_USER )
#endif
			{
				if(lpObj->Connected >= PLAYER_LOGGED)
				{
					if(GetTickCount() - lpObj->ConnectCheckTime > 60000)
					{
#if (__NEW_GIOCP__ == 1)
						CloseClient(lpObj->PerSocketContext, 0);
#else
						ResponErrorCloseClient(n);
#endif
						LogAddTD(lMsg.Get(542),lpObj->m_Index,lpObj->AccountID,lpObj->Name,lpObj->Ip_addr);
					}

#if (ANTI_CHEAT==1)
					else
					{
#if( ENABLETEST_CHEATGUARD == 1 )
						g_CheatGuard.ProcCheckLive(lpObj);
#else
						
						lpObj->m_iCHECK_COUNT++;

						if( lpObj->m_iCHECK_COUNT >= 60 )
						{
							LogAddTD("[HACK ATEMPT!] [%s][%s] CheckCount >= 60 -> Disconnect!",lpObj->AccountID,lpObj->Name,lpObj->m_iCHECK_COUNT);
							CloseClient(lpObj->PerSocketContext, 0);
						}
#endif
					}
#endif
				}
				else
				{
					if(GetTickCount() - lpObj->ConnectCheckTime > 30000)
					{
#if (__NEW_GIOCP__ == 1)
						CloseClient(lpObj->PerSocketContext, 0);
#else
						ResponErrorCloseClient(n);
#endif
						LogAddTD(lMsg.Get(543),lpObj->m_Index,lpObj->AccountID,lpObj->Name,lpObj->Ip_addr);
					}
				}

			}
#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
			else if( lpObj->Type == OBJ_USER && lpObj->bOffTrade)
			{
				if(lpObj->Connected >= PLAYER_LOGGED)
				{
					g_OfflineTrade.ProcRun(lpObj);
				}
			}
#endif
		}
	}
}






void gObjManaPotionFill(LPOBJ lpObj)
{
	lpObj->m_ManaFillCount++;

	float tmana;
	int ChangeMana = 0;
	float mpmul;
	int addmana;

	if(lpObj->m_ManaFillCount > 2)
	{
		lpObj->m_ManaFillCount = 0;

		int BP,AddBP;
		BYTE MapAttr;
		int rBP,CurBP;

		if(lpObj->MaxMana + lpObj->AddMana > lpObj->Mana)
		{
			mpmul = 0.037f;

			if( BC_MAP_RANGE(lpObj->MapNumber) == FALSE &&
				CC_MAP_RANGE(lpObj->MapNumber) == FALSE &&
				KALIMA_MAP_RANGE(lpObj->MapNumber) == FALSE &&
				DS_MAP_RANGE(lpObj->MapNumber) == FALSE &&
				IT_MAP_RANGE(lpObj->MapNumber) == FALSE &&
				lpObj->MapNumber != MAP_INDEX_CASTLESIEGE &&
				lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE )
			{
				addmana = gObjGetTotalValueOfEffect(lpObj,0x36);

				if( addmana > 0 )
				{
					mpmul += addmana / 100.0f;
				}
			}

			if( mpmul > 0.0f && lpObj->m_MPSkillOpt.MpsAutoRecoverMana > 0.0f )
			{
				mpmul += lpObj->m_MPSkillOpt.MpsAutoRecoverMana/100.0f;
			}

			tmana = lpObj->Mana;
			tmana += (lpObj->MaxMana + lpObj->AddMana)*mpmul;
			tmana += lpObj->m_JewelOfHarmonyEffect.HJOpAddRefillMP;

			if( lpObj->m_wSocketAddMana > 0 )
			{
				tmana += lpObj->m_wSocketAddMana;
			}


			if(lpObj->MaxMana + lpObj->AddMana < tmana)
			{
				tmana = lpObj->MaxMana + lpObj->AddMana;
			}

			lpObj->Mana = tmana;
			ChangeMana = 1;
		}

		BP = lpObj->MaxBP + lpObj->AddBP;

		if(lpObj->BP < BP)
		{

			MapAttr = MapC[lpObj->MapNumber].GetAttr((short &)lpObj->X,(short &)lpObj->Y);
			rBP = 2;

			if((MapAttr & 1) != 0)
			{
				rBP = 5;
			}

			if( gObjCheckUsedBuffEffect(lpObj,BUFF_SANTA_FORTUNE) == TRUE )
			{
				rBP = 5;

				if( (MapAttr & 1) != 0 )
				{
					rBP = 10;
				}
			}

			if(lpObj->Class == CLASS_KNIGHT)
			{
				AddBP = rBP +(BP * 5 / 100);
			}
			else
			{
				AddBP = rBP +(BP * 3 / 100);
			}

			if( lpObj->m_MPSkillOpt.MpsAutoRecoverAG > 0.0f )
			{
				AddBP += (BP*lpObj->m_MPSkillOpt.MpsAutoRecoverAG)/100.0f;
			}

			if( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_SOUL) == TRUE )
			{
				AddBP += 8;
			}

			AddBP += lpObj->SetOpIncAGValue;

			if(AddBP < 1)
			{
				AddBP = 1;
			}

			CurBP = lpObj->BP;
			CurBP += AddBP;

			if(CurBP > BP)
			{
				CurBP = BP;
			}

			lpObj->BP = CurBP;

			ChangeMana = 1;
		}

		if(ChangeMana != 0)
		{
			GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,255,0,lpObj->BP);
		}
	}
}






void gObjRestPotionFill(LPOBJ lpObj)
{
	if(lpObj->m_Rest != 0)
	{
		float tlife;
		float tmana;
		int percent = 3;
		lpObj->m_LifeFillCount++;

		if(lpObj->m_LifeFillCount > 4)
		{
			lpObj->m_LifeFillCount = 0;

			tlife = lpObj->Life;

			if(lpObj->pInventory[10].IsItem() == 1)
			{
				if(lpObj->pInventory[10].m_Option3 != 0 && lpObj->pInventory[10].m_Type != ITEMGET(13,24))
				{
					percent += lpObj->pInventory[10].m_Option3;
				}
			}
			else if(lpObj->pInventory[11].IsItem() == 1)
			{
				if(lpObj->pInventory[11].m_Option3 != 0 && lpObj->pInventory[11].m_Type != ITEMGET(13,24))
				{
					percent += lpObj->pInventory[11].m_Option3;
				}
			}
			else if(lpObj->pInventory[9].IsItem() == 1)
			{
				if(lpObj->pInventory[9].m_Option3 != 0 && lpObj->pInventory[9].m_Type != ITEMGET(13,28))
				{
					percent += lpObj->pInventory[9].m_Option3;
				}
			}

			if( percent > 0.0f && lpObj->m_MPSkillOpt.MpsAutoRecoverLife > 0.0f )
			{
				percent += lpObj->m_MPSkillOpt.MpsAutoRecoverLife;
			}

			int AddPercent = gObjGetTotalValueOfEffect(lpObj,ADD_OPTION_REFILLHPPERC);

			if( AddPercent > 0 )
			{
				percent += AddPercent;
			}

			tlife += (lpObj->MaxLife + lpObj->AddLife)*percent / 100.0f;
			tlife += lpObj->m_JewelOfHarmonyEffect.HJOpAddRefillHP;

			if( lpObj->m_wSocketAddLife > 0 )
			{
				tlife += lpObj->m_wSocketAddLife;
			}
			
			if((lpObj->MaxLife + lpObj->AddLife) < tlife)
			{
				tlife = lpObj->MaxLife + lpObj->AddLife;
			}

			lpObj->Life = tlife;
	
			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,255,0,lpObj->iShield);
	
			int percentmana = 3;
			int addpercentmana = gObjGetTotalValueOfEffect(lpObj,ADD_OPTION_REFILLMPPERC);
	
			if( addpercentmana > 0 )
			{
				percentmana += addpercentmana;
			}
	
			tmana = lpObj->Mana + lpObj->AddMana;
			tmana += (lpObj->MaxMana + lpObj->AddMana) * percentmana / 100.0f;
			tmana += lpObj->m_JewelOfHarmonyEffect.HJOpAddRefillMP;
	
			if((lpObj->MaxMana + lpObj->AddMana) < tmana)
			{
				tmana = lpObj->MaxMana + lpObj->AddMana;
			}
	
			lpObj->Mana = tmana;
	
			GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,255,0,lpObj->BP);
		}
	}
	else
	{
		int percent = 0;
		int addpercent;
		float tlife;
		if(lpObj->pInventory[10].IsItem() == 1)
		{
			if(lpObj->pInventory[10].m_Option3 != 0 && lpObj->pInventory[10].m_Durability != 0.0f && lpObj->pInventory[10].m_Type != ITEMGET(13,24))
			{
				percent += lpObj->pInventory[10].m_Option3;
			}
		}

		if(lpObj->pInventory[11].IsItem() == 1)
		{
			if(lpObj->pInventory[11].m_Option3 != 0 && lpObj->pInventory[11].m_Durability != 0.0f && lpObj->pInventory[11].m_Type != ITEMGET(13,24))
			{
				percent += lpObj->pInventory[11].m_Option3;
			}
		}

		if(lpObj->pInventory[9].IsItem() == 1)
		{
			if(lpObj->pInventory[9].m_Option3 != 0 && lpObj->pInventory[9].m_Durability != 0.0f && lpObj->pInventory[9].m_Type != ITEMGET(13,28))
			{
				percent += lpObj->pInventory[9].m_Option3;
			}
		}

		if(lpObj->pInventory[7].IsItem() == 1)
		{
			if(lpObj->pInventory[7].m_Option3 != 0 && lpObj->pInventory[7].m_Special[2] == 85 && lpObj->pInventory[7].m_Durability != 0.0f)
			{
				percent += lpObj->pInventory[7].m_Option3;
			}
		}

		if( !BC_MAP_RANGE(lpObj->MapNumber) &&
			!CC_MAP_RANGE(lpObj->MapNumber) &&
			!KALIMA_MAP_RANGE(lpObj->MapNumber) &&
			!DS_MAP_RANGE(lpObj->MapNumber) &&
			!IT_MAP_RANGE(lpObj->MapNumber) &&
			lpObj->MapNumber != MAP_INDEX_CASTLESIEGE &&
			lpObj->MapNumber != MAP_INDEX_CASTLEHUNTZONE )
		{
			addpercent = gObjGetTotalValueOfEffect(lpObj,0x35);

			if( addpercent > 0 )
			{
				percent += addpercent;
			}
		}

		if( lpObj->m_MPSkillOpt.MpsAutoRecoverLife > 0.0f )
		{
			percent += lpObj->m_MPSkillOpt.MpsAutoRecoverLife;
		}

		if(percent != 0)
		{

			lpObj->m_LifeFillCount++;

			if(lpObj->m_LifeFillCount > 6)
			{
				lpObj->m_LifeFillCount = 0;
				tlife = lpObj->Life;

				tlife += (lpObj->MaxLife +lpObj->AddLife) * percent / 100.f;
				tlife += lpObj->m_JewelOfHarmonyEffect.HJOpAddRefillHP;

				if( lpObj->m_wSocketAddLife > 0 )
				{
					tlife += lpObj->m_wSocketAddLife;
				}

				if((lpObj->MaxLife + lpObj->AddLife) < tlife)
				{
					tlife = lpObj->MaxLife + lpObj->AddLife;
				}

				lpObj->Life = tlife;

				GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,255,0,lpObj->iShield);
			}
		}
	}

	if(lpObj->iFillShield > 0)
	{
		int iFillSDPoint = 0;
		if(lpObj->iFillShieldCount <= 0)
		{
			lpObj->iFillShieldCount = 1;
		}

		iFillSDPoint = lpObj->iFillShieldMax / lpObj->iFillShieldCount;

		lpObj->iShield += iFillSDPoint;

		lpObj->iFillShield -= iFillSDPoint;

		if(lpObj->iFillShield < iFillSDPoint)
		{
			iFillSDPoint = lpObj->iFillShield;
			lpObj->iFillShield = 0;

			if(iFillSDPoint < 0)
			{
				iFillSDPoint = 0;
			}
		}

		if(lpObj->iShield > (lpObj->iMaxShield + lpObj->iAddShield))
		{
			lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;

			lpObj->iFillShield = 0;
			lpObj->iFillShieldMax = 0;
		}

		GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,255,0,lpObj->iShield);
	}

	if(lpObj->FillLife > 0)
	{
		float cur = lpObj->FillLifeMax * 34.0f / 100.0f;

		if(lpObj->FillLifeCount == 2)
		{
			cur = lpObj->FillLifeMax * 54.0f / 100.0f;
		}

		if(lpObj->FillLifeCount == 4)
		{
			cur = lpObj->FillLifeMax * 25.0f / 100.0f;
		}

		if(lpObj->FillLife < cur)
		{
			cur = lpObj->FillLife;
			lpObj->FillLife = 0;

			if(cur < 0)
			{
				cur = 0;
			}
		}
		else
		{
			lpObj->FillLife -= cur;
		}

		lpObj->Life += cur;

		if((lpObj->MaxLife + lpObj->AddLife) < lpObj->Life)
		{
			lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
			lpObj->FillLife = 0;
		}

		GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,255,0,lpObj->iShield);
	}
}






void gObjUseDrink(LPOBJ lpObj, int level)
{
	int time;

	if ( level == 2 )
	{
		time = 90;
#ifdef __NOVUS__
		time = 300;
#endif
		GCItemUseSpecialTimeSend(lpObj->m_Index, 1, time);
		lpObj->m_SkillAttack2 = 1;
		lpObj->m_SkillAttackTime2 = time;
	}
	else if ( level == 1 )
	{
		time = 180;
		GCItemUseSpecialTimeSend(lpObj->m_Index, 0, time);
	}
	else if ( level == 0x4D )
	{
		time = 60;
		GCItemUseSpecialTimeSend(lpObj->m_Index, 2, time);
	}
	else
	{
		time = 80;
		GCItemUseSpecialTimeSend(lpObj->m_Index, 0, time);
	}
}






int  gObjCurMoveMake(BYTE * const path , LPOBJ lpObj)	// Func not used
{
	// Lacking Full Code Here
	return 0;
}

void gObjViewportListProtocolDestroy(LPOBJ lpObj)
{
	BYTE sendBuf[5000];
	int lOfs,count;
	count = 0;
	lOfs = 4;

	PBMSG_COUNT pCount;

	pViewportDestroy.NumberH = SET_NUMBERH(lpObj->m_Index);
	pViewportDestroy.NumberL = SET_NUMBERL(lpObj->m_Index);


	memcpy(&sendBuf[lOfs],&pViewportDestroy,2);

	lOfs += 2;
	count += 1;

	pCount.h.c = 0xC1;
	pCount.h.headcode = 20;
	pCount.h.size = lOfs;
	pCount.count = count;

	memcpy(sendBuf,&pCount,sizeof(pCount));

	if(count < 1)	{
		return;
	}

	MsgSendV2(lpObj,sendBuf,lOfs);
}

void gObjViewportListProtocolCreate(LPOBJ lpObj)
{
	int n,tObjNum;
	BYTE sendBuf[5000];
	int lOfs,count,moncount;
	PWMSG_COUNT pwCount;

	count = 0;
	moncount = 0;
	lOfs = sizeof(pwCount);

	int EffectCount = 0;

	int MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	if(lpObj->Type == OBJ_USER)
	{
		if(		lpObj->m_Change >= 0
			&&	(lpObj->m_EnableUseChangeSkin 
			||	!IsTransformCharcterSkin(lpObj->m_Change)) )	//1.01.06
		{
			pViewportCreateChange.NumberH = SET_NUMBERH(lpObj->m_Index);
			pViewportCreateChange.NumberL = SET_NUMBERL(lpObj->m_Index);
			pViewportCreateChange.NumberH |= 0x80;
			pViewportCreateChange.X = lpObj->X;
			pViewportCreateChange.Y = lpObj->Y;
			pViewportCreateChange.TX = lpObj->TX;
			pViewportCreateChange.TY = lpObj->TY;
			pViewportCreateChange.SkinH = SET_NUMBERH((lpObj->m_Change & 0xFFFF) & 0xFFFF);
			pViewportCreateChange.SkinL = SET_NUMBERL((lpObj->m_Change & 0xFFFF) & 0xFFFF);

			if( gObjCheckUsedBuffEffect(lpObj,BUFF_GM) == TRUE )
			{
				gObjAddBuffEffect(lpObj,BUFF_GM,0,0,0,0,-10);
			}

			pViewportCreateChange.DirAndPkLevel = lpObj->Dir << 4;
			pViewportCreateChange.DirAndPkLevel |= lpObj->m_PK_Level & 0x0F;

			lpObj->CharSet[0] &= 0xF8;
			lpObj->CharSet[0] |= lpObj->m_ViewState & 7;

			memcpy(pViewportCreateChange.CharSet,lpObj->CharSet,sizeof(pViewportCreateChange.CharSet));
			memcpy(pViewportCreateChange.Id,lpObj->Name,sizeof(pViewportCreateChange.Id));

			pViewportCreateChange.btElement = lpObj->m_btAttribute;

#if (ENABLETEST_SEASON8DOT3 == 1)
			WORD TestItem = ITEMGET(13, 220);
			pViewportCreateChange.MuunItemH = SET_NUMBERH(TestItem);
			pViewportCreateChange.MuunItemL = SET_NUMBERL(TestItem);
			pViewportCreateChange.LevelH = SET_NUMBERH(lpObj->Level);
			pViewportCreateChange.LevelL = SET_NUMBERL(lpObj->Level);
#endif

			int MaxLife = int(lpObj->MaxLife + lpObj->AddLife);
			int Life = (int)lpObj->Life;

#if (ENABLETEST_SEASON8DOT3 == 0)
			pViewportCreateChange.HPNumberH = SET_NUMBERH(lpObj->m_Index);
			pViewportCreateChange.HPNumberL = SET_NUMBERL(lpObj->m_Index);
#endif

			pViewportCreateChange.MAXHP_HIGHH = ((MaxLife>>16)>>8);
			pViewportCreateChange.MAXHP_HIGHL = ((MaxLife>>16)&0xFF);
			pViewportCreateChange.MAXHP_LOWH = ((MaxLife&0xFFFF)>>8);
			pViewportCreateChange.MAXHP_LOWL = ((MaxLife&0xFFFF)&0xFF);

			pViewportCreateChange.HP_HIGHH = ((Life>>16)>>8);
			pViewportCreateChange.HP_HIGHL = ((Life>>16)&0xFF);
			pViewportCreateChange.HP_LOWH = ((Life&0xFFFF)>>8);
			pViewportCreateChange.HP_LOWL = ((Life&0xFFFF)&0xFF);

			EffectCount = gObjMakeViewportState(lpObj,&pViewportCreateChange.EffectList[0]);
			pViewportCreateChange.EffectCount = EffectCount;

			memcpy(&sendBuf[lOfs],&pViewportCreateChange,sizeof(pViewportCreateChange) - (MAX_EFFECT_INFO -EffectCount));

			lOfs += sizeof(pViewportCreateChange) - (MAX_EFFECT_INFO -EffectCount);
			count += 1;

			if(count < 1)
			{
				return;
			}

			pwCount.h.headcode = 0x45;
		}
		else
		{
			pViewportCreate.NumberH = SET_NUMBERH(lpObj->m_Index);
			pViewportCreate.NumberL = SET_NUMBERL(lpObj->m_Index);

			lpObj->CharSet[0] &= 0xF8;

			if(lpObj->m_State == 1 && lpObj->Teleport==0)
			{
				pViewportCreate.NumberH |= 0x80;
			}

			lpObj->CharSet[0] |= lpObj->m_ViewState & 7;

			pViewportCreate.X = lpObj->X;
			pViewportCreate.Y = lpObj->Y;
			pViewportCreate.TX = lpObj->TX;
			pViewportCreate.TY = lpObj->TY;

			pViewportCreate.DirAndPkLevel = lpObj->Dir << 4;
			pViewportCreate.DirAndPkLevel |= lpObj->m_PK_Level & 0x0F;

			memcpy(pViewportCreate.CharSet,lpObj->CharSet,sizeof(pViewportCreate.CharSet));
			memcpy(pViewportCreate.Id,lpObj->Name,sizeof(pViewportCreate.Id));

			pViewportCreate.btElement = lpObj->m_btAttribute;

#if (ENABLETEST_SEASON8DOT3 == 1)
			WORD TestItem = ITEMGET(13, 220);
			pViewportCreate.MuunItemH = SET_NUMBERH(TestItem);
			pViewportCreate.MuunItemL = SET_NUMBERL(TestItem);
			pViewportCreate.LevelH = SET_NUMBERH(lpObj->Level);
			pViewportCreate.LevelL = SET_NUMBERL(lpObj->Level);
#endif

			int MaxLife = (int)(lpObj->MaxLife + lpObj->AddLife);
			int Life = (int)lpObj->Life;
#if (ENABLETEST_SEASON8DOT3 == 0)
			pViewportCreate.HPNumberH = SET_NUMBERH(lpObj->m_Index);
			pViewportCreate.HPNumberL = SET_NUMBERL(lpObj->m_Index);
#endif
			pViewportCreate.MAXHP_HIGHH = ((MaxLife>>16)>>8);
			pViewportCreate.MAXHP_HIGHL = ((MaxLife>>16)&0xFF);
			pViewportCreate.MAXHP_LOWH = ((MaxLife&0xFFFF)>>8);
			pViewportCreate.MAXHP_LOWL = ((MaxLife&0xFFFF)&0xFF);

			pViewportCreate.HP_HIGHH = ((Life>>16)>>8);
			pViewportCreate.HP_HIGHL = ((Life>>16)&0xFF);
			pViewportCreate.HP_LOWH = ((Life&0xFFFF)>>8);
			pViewportCreate.HP_LOWL = ((Life&0xFFFF)&0xFF);

			EffectCount = gObjMakeViewportState(lpObj, pViewportCreate.EffectList);
			pViewportCreate.EffectCount = EffectCount;

			memcpy(&sendBuf[lOfs],&pViewportCreate,sizeof(pViewportCreate));

			lOfs += sizeof(pViewportCreate) - (MAX_EFFECT_INFO - EffectCount);
			count += 1;

			if(count < 1)
			{
				return;
			}

			pwCount.h.headcode = 0x12;
		}

		pwCount.h.c = 0xC2;
		pwCount.h.sizeH = SET_NUMBERH(lOfs);
		pwCount.h.sizeL = SET_NUMBERL(lOfs);
		pwCount.count = count;

		memcpy(sendBuf,&pwCount,sizeof(pwCount));

		DataSend(lpObj->m_Index,sendBuf,lOfs);

		if(lpObj->GuildNumber != 0)
		{
			GuildInfoOfs = 0;
			GuildInfoOfs = 5;

			PMSG_SIMPLE_GUILDVIEWPORT pGuild;

			pGuild.GuildNumber = lpObj->lpGuild->Number;
			pGuild.NumberH = SET_NUMBERH(lpObj->m_Index) & 0x7F;
			pGuild.NumberL = SET_NUMBERL(lpObj->m_Index);

			if(strcmp(lpObj->lpGuild->Names[0],lpObj->Name) == 0)
			{
				pGuild.NumberH |= 0x80;
			}

			pGuild.btGuildType = lpObj->lpGuild->btGuildType;
			pGuild.btGuildStatus = lpObj->GuildStatus;

			if( g_CastleSiege.CheckCastleOwnerMember(lpObj->m_Index) == TRUE ||
				g_CastleSiege.CheckCastleOwnerUnionMember(lpObj->m_Index) == TRUE )
			{
				pGuild.btCastleState = TRUE;
			}
			else
			{
				pGuild.btCastleState = FALSE;
			}

			memcpy(&GuildInfoBuf[GuildInfoOfs],&pGuild,sizeof(pGuild));

			GuildInfoOfs += sizeof(pGuild);

			PMSG_SIMPLE_GUILDVIEWPORT_COUNT pwCount;

			pwCount.h.c = 0xC2;
			pwCount.h.headcode = 0x65;
			pwCount.h.sizeH = SET_NUMBERH(GuildInfoOfs);
			pwCount.h.sizeL = SET_NUMBERL(GuildInfoOfs);
			pwCount.Count = 1;

			memcpy(GuildInfoBuf,&pwCount,sizeof(pwCount));

			DataSend(lpObj->m_Index,GuildInfoBuf,GuildInfoOfs);
		}

#if (__GENS_SYSTEM__ == 1)
		gGensSystem.SendGensInfo(lpObj->m_Index);
#endif

		for(n = 0; n < MVL; n++)
		{
			if(lpObj->VpPlayer2[n].state != 0)
			{
				tObjNum = lpObj->VpPlayer2[n].number;
	
				if(tObjNum >= 0)
				{
					if(lpObj->VpPlayer2[n].type == OBJ_USER)
					{
						DataSend(tObjNum,sendBuf,lOfs);
	
						if(GuildInfoCount != 0)
						{
							DataSend(tObjNum,GuildInfoBuf,GuildInfoOfs);
						}
	
						if(GuildUserCount != 0)
						{
							DataSend(tObjNum,GuildUserBuf,GuildUserOfs);
						}
					}
				}
			}
		}
	}
	else
	{
		count = 0;
	
		if(lpObj->m_RecallMon >= 0)
		{
			pCallMonsterViewportCreate.NumberH = SET_NUMBERH(lpObj->m_Index);
			pCallMonsterViewportCreate.NumberL = SET_NUMBERL(lpObj->m_Index);
	
			if(lpObj->m_State == 1)
			{
				pCallMonsterViewportCreate.NumberH |= 0x80;
			}
	
			pCallMonsterViewportCreate.Type_HI = SET_NUMBERH(lpObj->Class);
			pCallMonsterViewportCreate.Type_LO = SET_NUMBERL(lpObj->Class);
	
			pCallMonsterViewportCreate.X = lpObj->X;
			pCallMonsterViewportCreate.Y = lpObj->Y;
	
			pCallMonsterViewportCreate.TX = lpObj->TX;
			pCallMonsterViewportCreate.TY = lpObj->TY;
	
			pCallMonsterViewportCreate.Path = lpObj->Dir << 4;
	
			if(lpObj->m_RecallMon >= 0 && lpObj->m_RecallMon < OBJMAX-1)
			{
				memcpy(pCallMonsterViewportCreate.Id,gObj[lpObj->m_RecallMon].Name,sizeof(pCallMonsterViewportCreate.Id));
			}
			else
			{
				memset(pCallMonsterViewportCreate.Id,0x00,sizeof(pCallMonsterViewportCreate.Id));
			}

			pCallMonsterViewportCreate.btElement = lpObj->m_btAttribute;
#if (ENABLETEST_SEASON8DOT3 == 1)
			pCallMonsterViewportCreate.LevelH = SET_NUMBERH(lpObj->Level);
			pCallMonsterViewportCreate.LevelL = SET_NUMBERL(lpObj->Level);
#endif
			int MaxLife = (int)(lpObj->MaxLife + lpObj->AddLife);
			int Life = (int)lpObj->Life;
#if (ENABLETEST_SEASON8DOT3 == 0)
			pCallMonsterViewportCreate.HPNumberH = SET_NUMBERH(lpObj->m_Index);
			pCallMonsterViewportCreate.HPNumberL = SET_NUMBERL(lpObj->m_Index);
#endif
			pCallMonsterViewportCreate.MAXHP_HIGHH = ((MaxLife>>16)>>8);
			pCallMonsterViewportCreate.MAXHP_HIGHL = ((MaxLife>>16)&0xFF);
			pCallMonsterViewportCreate.MAXHP_LOWH = ((MaxLife&0xFFFF)>>8);
			pCallMonsterViewportCreate.MAXHP_LOWL = ((MaxLife&0xFFFF)&0xFF);

			pCallMonsterViewportCreate.HP_HIGHH = ((Life>>16)>>8);
			pCallMonsterViewportCreate.HP_HIGHL = ((Life>>16)&0xFF);
			pCallMonsterViewportCreate.HP_LOWH = ((Life&0xFFFF)>>8);
			pCallMonsterViewportCreate.HP_LOWL = ((Life&0xFFFF)&0xFF);
	
			EffectCount = gObjMakeViewportState(lpObj,pCallMonsterViewportCreate.EffectList);
			pCallMonsterViewportCreate.EffectCount = EffectCount;

			memcpy(&sendBuf[lOfs],&pCallMonsterViewportCreate,sizeof(pCallMonsterViewportCreate));
	
			lOfs += sizeof(pCallMonsterViewportCreate) - (MAX_EFFECT_INFO - EffectCount);
			count += 1;
		}
		else
		{
			pMonsterViewportCreate.NumberH = SET_NUMBERH(lpObj->m_Index);
			pMonsterViewportCreate.NumberL = SET_NUMBERL(lpObj->m_Index);
	
			if(lpObj->m_State == 1 && lpObj->Teleport == 0)
			{
				pMonsterViewportCreate.NumberH |= 0x80;
				pMonsterViewportCreate.NumberH |= 0x40;
			}
	
			pMonsterViewportCreate.Type_HI = SET_NUMBERH(lpObj->Class);
			pMonsterViewportCreate.Type_LO = SET_NUMBERL(lpObj->Class);

			pMonsterViewportCreate.X = lpObj->X;
			pMonsterViewportCreate.Y = lpObj->Y;
			pMonsterViewportCreate.TX = lpObj->TX;
			pMonsterViewportCreate.TY = lpObj->TY;
	
			pMonsterViewportCreate.Path = lpObj->Dir << 4;

			pMonsterViewportCreate.btElement = lpObj->m_btAttribute;
#if (ENABLETEST_SEASON8DOT3 == 1)
			pMonsterViewportCreate.LevelH = SET_NUMBERH(lpObj->Level);
			pMonsterViewportCreate.LevelL = SET_NUMBERL(lpObj->Level);
#endif
			int MaxLife = (int)(lpObj->MaxLife + lpObj->AddLife);
			int Life = (int)lpObj->Life;
#if (ENABLETEST_SEASON8DOT3 == 0)
			pMonsterViewportCreate.HPNumberH = SET_NUMBERH(lpObj->m_Index);
			pMonsterViewportCreate.HPNumberL = SET_NUMBERL(lpObj->m_Index);
#endif
			pMonsterViewportCreate.MAXHP_HIGHH = ((MaxLife>>16)>>8);
			pMonsterViewportCreate.MAXHP_HIGHL = ((MaxLife>>16)&0xFF);
			pMonsterViewportCreate.MAXHP_LOWH = ((MaxLife&0xFFFF)>>8);
			pMonsterViewportCreate.MAXHP_LOWL = ((MaxLife&0xFFFF)&0xFF);

			pMonsterViewportCreate.HP_HIGHH = ((Life>>16)>>8);
			pMonsterViewportCreate.HP_HIGHL = ((Life>>16)&0xFF);
			pMonsterViewportCreate.HP_LOWH = ((Life&0xFFFF)>>8);
			pMonsterViewportCreate.HP_LOWL = ((Life&0xFFFF)&0xFF);
	
			EffectCount = gObjMakeViewportState(lpObj,pMonsterViewportCreate.EffectList);
			pMonsterViewportCreate.EffectCount = EffectCount;

			memcpy(&sendBuf[lOfs],&pMonsterViewportCreate,sizeof(pMonsterViewportCreate));
			lOfs += sizeof(pMonsterViewportCreate) - (MAX_EFFECT_INFO - EffectCount);
			moncount += 1;
		}
	
		if(count > 0)
		{
			PWMSG_COUNT pwCount;
	
			pwCount.h.c = 0xC2;
			pwCount.h.headcode = 0x1F;
			pwCount.h.sizeH = SET_NUMBERH(lOfs);
			pwCount.h.sizeL = SET_NUMBERL(lOfs);
			pwCount.count = count;
	
			memcpy(sendBuf,&pwCount,sizeof(pwCount));
			MsgSendV2(lpObj,sendBuf,lOfs);
		}
	
		if(moncount > 0)
		{
			PWMSG_COUNT pwCount;
	
			pwCount.h.c = 0xC2;
			pwCount.h.headcode = 0x13;
			pwCount.h.sizeH = SET_NUMBERH(lOfs);
			pwCount.h.sizeL = SET_NUMBERL(lOfs);
			pwCount.count = moncount;
	
			memcpy(sendBuf,&pwCount,sizeof(pwCount));
			MsgSendV2(lpObj,sendBuf,lOfs);
		}
	}
}



void gObjViewportListProtocol(short aIndex)
{
	if(gObj[aIndex].Connected < PLAYER_PLAYING)
	{
		return;
	}

	int n;
	PBMSG_COUNT pCount;
	LPOBJ lpTargetObj,lpObj = &gObj[aIndex];
	int lOfs_Item,count_Item,ItemStructSize = sizeof(PMSG_ITEMVIEWPORTCREATE),lOfs,MonlOfs,callMonlOfs;
	int pMsgSize;
	short tObjNum;
	BYTE count,monstercount,callmonstercount;

	BYTE sendBuf[5000];
	BYTE MonstersendBuf[5000];
	BYTE callMonstersendBuf[5000];
	BYTE ItemBuf[5000];

#if (__GENS_SYSTEM__ == 1)
	BYTE GensBuff[5000];

	GensCount = 0;
	lOfsGens = sizeof(PMSG_GENS_VIEWPORT);
#endif

	GuildInfoOfs = 0;
	GuildInfoCount = 0;
	GuildUserOfs = 0;
	GuildUserCount = 0;
	ChangeCount = 0;
	GuildInfoOfs = 5;
	int EffectCount = 0;
	GuildUserOfs = 5;
	lOfsChange = 5;

	lOfs = 4;
	lOfs_Item = 5;
	count = 0;
	count_Item = 0;

	if(lpObj->Type == OBJ_USER)
	{
		for(n = 0; n < MAX_VIEWPORT;n++)
		{
			if(lpObj->VpPlayer[n].state == 3)
			{
				tObjNum = lpObj->VpPlayer[n].number;

				if(tObjNum >= 0)
				{
					switch(lpObj->VpPlayer[n].type)
					{
					case 1:
					case 2:
					case 3:
							pViewportDestroy.NumberH = SET_NUMBERH(tObjNum);
							pViewportDestroy.NumberL = SET_NUMBERL(tObjNum);

							memcpy(&sendBuf[lOfs],&pViewportDestroy,sizeof(pViewportDestroy));
							lOfs += sizeof(pViewportDestroy);
							count += 1;
						break;
					case 5:
						pItemViewportDestroy.NumberH = SET_NUMBERH(tObjNum);
						pItemViewportDestroy.NumberL = SET_NUMBERL(tObjNum);

						memcpy(&ItemBuf[lOfs_Item],&pItemViewportDestroy,sizeof(pItemViewportDestroy));

						lOfs_Item += sizeof(pItemViewportDestroy);
						count_Item += 1;
						break;
					}
				}

				lpObj->VpPlayer[n].state = 0;
				lpObj->VpPlayer[n].number = -1;
				lpObj->VPCount -= 1;
			}
		}
	}
	else if(lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
	{
		for(n = 0; n < MAX_VIEWPORT_MONSTER;n++)
		{
			if(lpObj->VpPlayer[n].state == 3)
			{
				lpObj->VpPlayer[n].state = 0;
				lpObj->VpPlayer[n].number = -1;
				lpObj->VPCount -= 1;
			}
		}
	}

	if(lpObj->Type == OBJ_USER)
	{
		if(count > 0)
		{
			pCount.h.c = 0xC1;
			pCount.h.headcode = 0x14;
			pCount.h.size = lOfs;
			pCount.count = count;

			memcpy(sendBuf,&pCount,sizeof(pCount));
			DataSend(aIndex,sendBuf,lOfs);
		}

		if(count_Item > 0)
		{
			pItemCount.h.c = 0xC2;
			pItemCount.h.headcode = 0x21;
			pItemCount.h.sizeH = SET_NUMBERH(lOfs_Item);
			pItemCount.h.sizeL = SET_NUMBERL(lOfs_Item);
			pItemCount.count = count_Item;

			memcpy(ItemBuf,&pItemCount,sizeof(pItemCount));
			DataSend(aIndex,ItemBuf,lOfs_Item);
		}
	}

	if(lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
	{
		for(n = 0; n < MAX_VIEWPORT_MONSTER;n++)
		{
			if(lpObj->VpPlayer[n].state == 1)
			{
				lpObj->VpPlayer[n].state = 2;
			}
		}
	}
	else
	{
		count = 0;
		monstercount = 0;
		count_Item = 0;
		callmonstercount = 0;
		lOfs = 5;
		MonlOfs = lOfs;
		callMonlOfs = MonlOfs;
		lOfs_Item = 5;
		pMsgSize = sizeof(pViewportCreate);

		if(lpObj->Type == OBJ_USER)
		{
			for(n = 0; n < MAX_VIEWPORT;n++)
			{
				if(lpObj->VpPlayer[n].state == 1)
				{
					tObjNum = lpObj->VpPlayer[n].number;

					if( gObj[tObjNum].Authority == 2 )
						continue;

					if( gObjCheckUsedBuffEffect(&gObj[tObjNum],BUFF_INVISIBILITY) == TRUE )
					{
						continue;
					}
#if (ENABLETEST_NEWPVP == 1)
					if( g_NewPVP.IsObserver(gObj[tObjNum]) )
					{
						continue;
					}
#endif
					if(tObjNum >= 0)
					{
						switch(lpObj->VpPlayer[n].type)
						{
						case OBJ_USER:
							lpTargetObj = &gObj[tObjNum];
		
							if(		lpTargetObj->m_Change >= 0
								&&	(lpTargetObj->m_EnableUseChangeSkin 
								||	!IsTransformCharcterSkin(lpTargetObj->m_Change)) )	//1.01.06
							{
								pViewportCreateChange.NumberH = SET_NUMBERH(tObjNum);
								pViewportCreateChange.NumberL = SET_NUMBERL(tObjNum);
		
								lpTargetObj->CharSet[0] &= 0xF8;
	
								if(lpTargetObj->m_State == 1 && lpTargetObj->Teleport == 0)
								{
									pViewportCreateChange.NumberH |= 0x80;
								}

								pViewportCreateChange.X = lpTargetObj->X;
								pViewportCreateChange.Y = lpTargetObj->Y;
								pViewportCreateChange.TX = lpTargetObj->TX;
								pViewportCreateChange.TY = lpTargetObj->TY;
								pViewportCreateChange.SkinH = SET_NUMBERH((lpTargetObj->m_Change & 0xFFFF) & 0xFFFF);
								pViewportCreateChange.SkinL = SET_NUMBERL((lpTargetObj->m_Change & 0xFFFF) & 0xFFFF);

								if( (lpTargetObj->Authority & 32) == 32 )
								{
									gObjAddBuffEffect(lpTargetObj,BUFF_GM,0,0,0,0,-10);
								}

								pViewportCreateChange.DirAndPkLevel = lpTargetObj->Dir << 4;
								pViewportCreateChange.DirAndPkLevel |= lpTargetObj->m_PK_Level & 0x0F;

								lpObj->CharSet[0] &= 0xF8;
								lpObj->CharSet[0] |= lpObj->m_ViewState & 7;

								memcpy(pViewportCreateChange.CharSet,lpTargetObj->CharSet,sizeof(pViewportCreateChange.CharSet));
								memcpy(pViewportCreateChange.Id,lpTargetObj->Name,sizeof(pViewportCreateChange.Id));

								pViewportCreateChange.btElement = lpTargetObj->m_btAttribute;
#if (ENABLETEST_SEASON8DOT3 == 1)
								WORD TestItem = ITEMGET(13, 220);
								pViewportCreateChange.MuunItemH = SET_NUMBERH(TestItem);
								pViewportCreateChange.MuunItemL = SET_NUMBERL(TestItem);
								pViewportCreateChange.LevelH = SET_NUMBERH(lpTargetObj->Level);
								pViewportCreateChange.LevelL = SET_NUMBERL(lpTargetObj->Level);
#endif
								int MaxLife = (int)(lpTargetObj->MaxLife + lpTargetObj->AddLife);
								int Life = (int)lpTargetObj->Life;
#if (ENABLETEST_SEASON8DOT3 == 0)
								pViewportCreateChange.HPNumberH = SET_NUMBERH(tObjNum);
								pViewportCreateChange.HPNumberL = SET_NUMBERL(tObjNum);
#endif
								pViewportCreateChange.MAXHP_HIGHH = ((MaxLife>>16)>>8);
								pViewportCreateChange.MAXHP_HIGHL = ((MaxLife>>16)&0xFF);
								pViewportCreateChange.MAXHP_LOWH = ((MaxLife&0xFFFF)>>8);
								pViewportCreateChange.MAXHP_LOWL = ((MaxLife&0xFFFF)&0xFF);

								pViewportCreateChange.HP_HIGHH = ((Life>>16)>>8);
								pViewportCreateChange.HP_HIGHL = ((Life>>16)&0xFF);
								pViewportCreateChange.HP_LOWH = ((Life&0xFFFF)>>8);
								pViewportCreateChange.HP_LOWL = ((Life&0xFFFF)&0xFF);

								EffectCount = gObjMakeViewportState(lpTargetObj,pViewportCreateChange.EffectList);
								pViewportCreateChange.EffectCount = EffectCount;

								memcpy(&SendGBufChange[lOfsChange],&pViewportCreateChange,sizeof(pViewportCreateChange));
								lOfsChange += sizeof(pViewportCreateChange) - (MAX_EFFECT_INFO - EffectCount);
								ChangeCount +=1;
							}
							else
							{
								pViewportCreate.NumberH = SET_NUMBERH(tObjNum);
								pViewportCreate.NumberL = SET_NUMBERL(tObjNum);
		
								lpTargetObj->CharSet[0] &= 0xF8;
		
								if(lpTargetObj->m_State == 1 && lpTargetObj->Teleport == 0)
								{
									pViewportCreate.NumberH |= 0x80;
								}
		
								lpTargetObj->CharSet[0] |= lpTargetObj->m_ViewState & 0x0F;
			
								pViewportCreate.X = lpTargetObj->X;
								pViewportCreate.Y = lpTargetObj->Y;
								pViewportCreate.TX = lpTargetObj->TX;
								pViewportCreate.TY = lpTargetObj->TY;
		
								pViewportCreate.DirAndPkLevel = lpTargetObj->Dir << 4;
								pViewportCreate.DirAndPkLevel |= lpTargetObj->m_PK_Level & 0x0F;

								memcpy(pViewportCreate.CharSet,lpTargetObj->CharSet,sizeof(pViewportCreate.CharSet));
								memcpy(pViewportCreate.Id,lpTargetObj->Name,sizeof(pViewportCreate.Id));

								pViewportCreate.btElement = lpTargetObj->m_btAttribute;
#if (ENABLETEST_SEASON8DOT3 == 1)
								WORD TestItem = ITEMGET(13, 220);
								pViewportCreate.MuunItemH = SET_NUMBERH(TestItem);
								pViewportCreate.MuunItemL = SET_NUMBERL(TestItem);
								pViewportCreate.LevelH = SET_NUMBERH(lpTargetObj->Level);
								pViewportCreate.LevelL = SET_NUMBERL(lpTargetObj->Level);
#endif
								int MaxLife = (int)(lpTargetObj->MaxLife + lpTargetObj->AddLife);
								int Life = (int)lpTargetObj->Life;
#if (ENABLETEST_SEASON8DOT3 == 0)
								pViewportCreate.HPNumberH = SET_NUMBERH(tObjNum);
								pViewportCreate.HPNumberL = SET_NUMBERL(tObjNum);
#endif
								pViewportCreate.MAXHP_HIGHH = ((MaxLife>>16)>>8);
								pViewportCreate.MAXHP_HIGHL = ((MaxLife>>16)&0xFF);
								pViewportCreate.MAXHP_LOWH = ((MaxLife&0xFFFF)>>8);
								pViewportCreate.MAXHP_LOWL = ((MaxLife&0xFFFF)&0xFF);

								pViewportCreate.HP_HIGHH = ((Life>>16)>>8);
								pViewportCreate.HP_HIGHL = ((Life>>16)&0xFF);
								pViewportCreate.HP_LOWH = ((Life&0xFFFF)>>8);
								pViewportCreate.HP_LOWL = ((Life&0xFFFF)&0xFF);

								EffectCount = gObjMakeViewportState(lpTargetObj,pViewportCreate.EffectList);
								pViewportCreate.EffectCount = EffectCount;

								memcpy(&sendBuf[lOfs],&pViewportCreate,sizeof(pViewportCreate));
		
								lOfs += sizeof(pViewportCreate) - (MAX_EFFECT_INFO - EffectCount);							
								count += 1;
							}
		
							if(lpObj->Type == OBJ_USER)
							{
								if(lpTargetObj->lpGuild != 0)
								{
									PMSG_SIMPLE_GUILDVIEWPORT pGuild;
		
									pGuild.GuildNumber = lpTargetObj->lpGuild->Number;
									pGuild.NumberH = SET_NUMBERH(lpTargetObj->m_Index) & 0x7F;
									pGuild.NumberL = SET_NUMBERL(lpTargetObj->m_Index);
		
									pGuild.btGuildStatus = lpTargetObj->GuildStatus;
									pGuild.btGuildType = lpTargetObj->lpGuild->btGuildType;
		
									if(lpObj->lpGuild != 0)
									{
										pGuild.btGuildRelationShip = gObjGetRelationShip(lpObj,lpTargetObj);
									}
									else
									{
										pGuild.btGuildRelationShip = 0;
									}
		
									if(strcmp(lpTargetObj->lpGuild->Names[0],lpTargetObj->Name)==0)
									{
										pGuild.NumberH |= 0x80;
									}
		
									memcpy(&GuildInfoBuf[GuildInfoOfs],&pGuild,sizeof(pGuild));
									GuildInfoOfs += sizeof(pGuild);
									GuildInfoCount += 1;
								}
		
								if(false)
								{

									PMSG_GUILDVIEWPORT_USER pGuildUserViewport;

									if(ViewGuildMng.Add(lpTargetObj->GuildNumber,lpTargetObj->m_Index)==1 && lpTargetObj->lpGuild != 0)
									{
										PMSG_GUILDVIEWPORT pGuildViewport;
		
										pGuildViewport.NumberH = SET_NUMBERH(lpTargetObj->lpGuild->Number);
										pGuildViewport.NumberL = SET_NUMBERL(lpTargetObj->lpGuild->Number);
		
										strcpy_s(pGuildViewport.GuildName, sizeof(pGuildViewport.GuildName), lpTargetObj->lpGuild->Name);
										memcpy(pGuildViewport.Mark,lpTargetObj->lpGuild->Mark,sizeof(pGuildViewport.Mark));
										memcpy(&GuildInfoBuf[GuildInfoOfs],&pGuildViewport,sizeof(pGuildViewport));
		
										GuildInfoOfs += sizeof(pGuildViewport);
										GuildInfoCount += 1;
									}
		
									pGuildUserViewport.NumberH = SET_NUMBERH(lpTargetObj->m_Index) & 0x7F;
									pGuildUserViewport.NumberL = SET_NUMBERL(lpTargetObj->m_Index);
		
									if(lpTargetObj->lpGuild != 0 && strcmp(lpTargetObj->lpGuild->Names[0],lpTargetObj->Name)==0)
									{
										pGuildUserViewport.NumberH |= 0x80;
									}
		
									if(lpTargetObj->lpGuild != 0)
									{
										pGuildUserViewport.GNumberH = SET_NUMBERH(lpTargetObj->lpGuild->Number);
										pGuildUserViewport.GNumberL = SET_NUMBERL(lpTargetObj->lpGuild->Number);
		
										memcpy(&GuildUserBuf[GuildUserOfs],&pGuildUserViewport,sizeof(pGuildUserViewport));
		
										GuildUserOfs += sizeof(pGuildUserViewport);
										GuildUserCount += 1;
									}
								}

#if (__GENS_SYSTEM__ == 1)
								if( lpTargetObj->m_GensInfo.btFamily != GENS_NONE )
								{
									pGensViewportCreate.btFamily = lpTargetObj->m_GensInfo.btFamily;
									pGensViewportCreate.IndexHI = SET_NUMBERH(lpTargetObj->m_Index);
									pGensViewportCreate.IndexLO = SET_NUMBERL(lpTargetObj->m_Index);
									pGensViewportCreate.Rank = lpTargetObj->m_GensInfo.iRank;
									pGensViewportCreate.Level = lpTargetObj->m_GensInfo.btLevel;
									pGensViewportCreate.Contrib = lpTargetObj->m_GensInfo.iContribution;

									memcpy(&GensBuff[lOfsGens],&pGensViewportCreate,sizeof(pGensViewportCreate));
									lOfsGens += sizeof(pGensViewportCreate);
									GensCount++;
								}
#endif
							}
							break;
						case 2:
						case 3:
							if(lpObj->Type == OBJ_USER)
							{
								lpTargetObj = &gObj[tObjNum];

								if( lpTargetObj->Class == 523 )
								{
									continue;
								}
		
								if(lpTargetObj->m_RecallMon >= 0)
								{
									pCallMonsterViewportCreate.NumberH = SET_NUMBERH(tObjNum);
									pCallMonsterViewportCreate.NumberL = SET_NUMBERL(tObjNum);
		
									if(lpTargetObj->m_State == 1)
									{
										pCallMonsterViewportCreate.NumberH |= 0x80;
									}
		
									pCallMonsterViewportCreate.Type_HI = SET_NUMBERH(lpTargetObj->Class);
									pCallMonsterViewportCreate.Type_LO = SET_NUMBERL(lpTargetObj->Class);
		
									pCallMonsterViewportCreate.X = lpTargetObj->X;
									pCallMonsterViewportCreate.Y = lpTargetObj->Y;
									pCallMonsterViewportCreate.TX = lpTargetObj->TX;
									pCallMonsterViewportCreate.TY = lpTargetObj->TY;
									pCallMonsterViewportCreate.Path = lpTargetObj->Dir << 4;
		
									if(lpTargetObj->m_RecallMon >= 0 && lpTargetObj->m_RecallMon < OBJMAX-1)
									{
										memcpy(pCallMonsterViewportCreate.Id,gObj[lpTargetObj->m_RecallMon].Name,sizeof(pCallMonsterViewportCreate.Id));
									}
									else
									{
										memset(pCallMonsterViewportCreate.Id,0x00,sizeof(pCallMonsterViewportCreate.Id));
									}

									pCallMonsterViewportCreate.btElement = lpTargetObj->m_btAttribute;
#if (ENABLETEST_SEASON8DOT3 == 1)
									pCallMonsterViewportCreate.LevelH = SET_NUMBERH(lpTargetObj->Level);
									pCallMonsterViewportCreate.LevelL = SET_NUMBERL(lpTargetObj->Level);
#endif
									int MaxLife = (int)(lpTargetObj->MaxLife + lpTargetObj->AddLife);
									int Life = (int)lpTargetObj->Life;
#if (ENABLETEST_SEASON8DOT3 == 0)
									pCallMonsterViewportCreate.HPNumberH = SET_NUMBERH(tObjNum);
									pCallMonsterViewportCreate.HPNumberL = SET_NUMBERL(tObjNum);
#endif
									pCallMonsterViewportCreate.MAXHP_HIGHH = ((MaxLife>>16)>>8);
									pCallMonsterViewportCreate.MAXHP_HIGHL = ((MaxLife>>16)&0xFF);
									pCallMonsterViewportCreate.MAXHP_LOWH = ((MaxLife&0xFFFF)>>8);
									pCallMonsterViewportCreate.MAXHP_LOWL = ((MaxLife&0xFFFF)&0xFF);
									pCallMonsterViewportCreate.HP_HIGHH = ((Life>>16)>>8);
									pCallMonsterViewportCreate.HP_HIGHL = ((Life>>16)&0xFF);
									pCallMonsterViewportCreate.HP_LOWH = ((Life&0xFFFF)>>8);
									pCallMonsterViewportCreate.HP_LOWL = ((Life&0xFFFF)&0xFF);

									EffectCount = gObjMakeViewportState(lpTargetObj,pCallMonsterViewportCreate.EffectList);
									pCallMonsterViewportCreate.EffectCount = EffectCount;

									memcpy(&callMonstersendBuf[callMonlOfs],&pCallMonsterViewportCreate,sizeof(pCallMonsterViewportCreate));
									callMonlOfs += sizeof(pCallMonsterViewportCreate) - (MAX_EFFECT_INFO - EffectCount);
									callmonstercount += 1;
								}
								else
								{
									pMonsterViewportCreate.NumberH = SET_NUMBERH(tObjNum);
									pMonsterViewportCreate.NumberL = SET_NUMBERL(tObjNum);
		
									if(lpTargetObj->m_State == 1)
									{
										pMonsterViewportCreate.NumberH |= 0x80;
		
										if(lpTargetObj->Teleport != 0)
										{
											pMonsterViewportCreate.NumberH |= 0x40;
										}
									}
		
									pMonsterViewportCreate.Type_HI = SET_NUMBERH(lpTargetObj->Class);
									pMonsterViewportCreate.Type_LO = SET_NUMBERL(lpTargetObj->Class);

#if(GS_CASTLE == 1)
									if(lpTargetObj->Class == 278)
									{
										if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE
											&& lpObj->m_btCsJoinSide == lpTargetObj->m_btCsJoinSide)
										{
											pMonsterViewportCreate.Type_HI |= 0x80;
										}

										if(lpObj->lpGuild != 0
											&& lpTargetObj->lpGuild != 0
											&& lpObj->lpGuild == lpTargetObj->lpGuild)
										{
											pMonsterViewportCreate.Type_HI |= 0x80;
										}

										pMonsterViewportCreate.Type_HI |= (lpTargetObj->m_btCreationState * 16) & 0x70;
									}
#endif

									pMonsterViewportCreate.X = lpTargetObj->X;
									pMonsterViewportCreate.Y = lpTargetObj->Y;
									pMonsterViewportCreate.TX = lpTargetObj->TX;
									pMonsterViewportCreate.TY = lpTargetObj->TY;
									pMonsterViewportCreate.Path = lpTargetObj->Dir << 4;

#if (GS_CASTLE == 1)
									if(lpTargetObj->Class == 277)
									{
										if(lpTargetObj->m_btCsGateOpen == 0)
										{
											gObjRemoveBuffEffect(lpTargetObj,BUFF_GATESTATUS);
										}
										else
										{
											gObjAddBuffEffect(lpTargetObj,BUFF_GATESTATUS);
										}
									}

									if(lpTargetObj->Class == 216)
									{
										if(g_CastleSiege.GetRegCrownAvailable() == FALSE)
										{
											gObjRemoveBuffEffect(lpTargetObj,BUFF_CROWNREGISTER);
										}
										else
										{
											gObjAddBuffEffect(lpTargetObj,BUFF_CROWNREGISTER);
										}
									}
#endif

									pMonsterViewportCreate.btElement = lpTargetObj->m_btAttribute;

#if (ENABLETEST_SEASON8DOT3 == 1)
									pMonsterViewportCreate.LevelH = SET_NUMBERH(lpTargetObj->Level);
									pMonsterViewportCreate.LevelL = SET_NUMBERL(lpTargetObj->Level);
#endif
									int MaxLife = (int)(lpTargetObj->MaxLife + lpTargetObj->AddLife);
									int Life = (int)lpTargetObj->Life;
#if (ENABLETEST_SEASON8DOT3 == 0)
									pMonsterViewportCreate.HPNumberH = SET_NUMBERH(tObjNum);
									pMonsterViewportCreate.HPNumberL = SET_NUMBERL(tObjNum);
#endif
									pMonsterViewportCreate.MAXHP_HIGHH = ((MaxLife>>16)>>8);
									pMonsterViewportCreate.MAXHP_HIGHL = ((MaxLife>>16)&0xFF);
									pMonsterViewportCreate.MAXHP_LOWH = ((MaxLife&0xFFFF)>>8);
									pMonsterViewportCreate.MAXHP_LOWL = ((MaxLife&0xFFFF)&0xFF);
									pMonsterViewportCreate.HP_HIGHH = ((Life>>16)>>8);
									pMonsterViewportCreate.HP_HIGHL = ((Life>>16)&0xFF);
									pMonsterViewportCreate.HP_LOWH = ((Life&0xFFFF)>>8);
									pMonsterViewportCreate.HP_LOWL = ((Life&0xFFFF)&0xFF);

									EffectCount = gObjMakeViewportState(lpTargetObj,pMonsterViewportCreate.EffectList);
									pMonsterViewportCreate.EffectCount = EffectCount;

									memcpy(&MonstersendBuf[MonlOfs],&pMonsterViewportCreate,sizeof(pMonsterViewportCreate));
									MonlOfs += sizeof(pMonsterViewportCreate) - (MAX_EFFECT_INFO - EffectCount);
									monstercount += 1;
								}
							}
							break;
						case 5:
							if(lpObj->Type == OBJ_USER)
							{
								pItemViewportCreate.NumberH = SET_NUMBERH(tObjNum);
								pItemViewportCreate.NumberL = SET_NUMBERL(tObjNum);
		
								if(MapC[lpObj->MapNumber].m_cItem[tObjNum].m_State == 1)
								{
									pItemViewportCreate.NumberH |= 0x80;
								}
		
								pItemViewportCreate.px = MapC[lpObj->MapNumber].m_cItem[tObjNum].px;
								pItemViewportCreate.py = MapC[lpObj->MapNumber].m_cItem[tObjNum].py;
		
								if(MapC[lpObj->MapNumber].m_cItem[tObjNum].m_Type == ITEMGET(14,15))
								{
									WORD MoneyHW = SET_NUMBERHW(MapC[lpObj->MapNumber].m_cItem[tObjNum].m_BuyMoney);
									WORD MoneyLW = SET_NUMBERLW(MapC[lpObj->MapNumber].m_cItem[tObjNum].m_BuyMoney);
		
									pItemViewportCreate.ItemInfo[0] = BYTE(MapC[lpObj->MapNumber].m_cItem[tObjNum].m_Type)%255;
									pItemViewportCreate.ItemInfo[1] = SET_NUMBERL(MoneyHW);
									pItemViewportCreate.ItemInfo[2] = SET_NUMBERH(MoneyLW);
									pItemViewportCreate.ItemInfo[4] = SET_NUMBERL(MoneyLW);
									pItemViewportCreate.ItemInfo[3] = 0;
									pItemViewportCreate.ItemInfo[5] = (MapC[lpObj->MapNumber].m_cItem[tObjNum].m_Type & 0x1E00) >> 5;
									pItemViewportCreate.ItemInfo[6] = 0;
		
									memcpy(&ItemBuf[lOfs_Item],&pItemViewportCreate,sizeof(pItemViewportCreate));
									lOfs_Item += ItemStructSize;
								}
								else
								{
									ItemByteConvert(pItemViewportCreate.ItemInfo, (MapC[lpObj->MapNumber].m_cItem[tObjNum]));
									memcpy(&ItemBuf[lOfs_Item],&pItemViewportCreate,sizeof(pItemViewportCreate));
									lOfs_Item += ItemStructSize;
								}

								count_Item += 1;
							}
							break;
						}
					}
					lpObj->VpPlayer[n].state = 2;
				}
			}
		}

		if(lpObj->Type == OBJ_USER)
		{
			if(count > 0)
			{
				PWMSG_COUNT pCount;
	
				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x12;
				pCount.h.sizeH = SET_NUMBERH(lOfs);
				pCount.h.sizeL = SET_NUMBERL(lOfs);
				pCount.count = count;
	
				memcpy(sendBuf,&pCount,sizeof(pCount));
				DataSend(aIndex,(unsigned char *)&sendBuf,lOfs);
			}
	
			if(ChangeCount > 0)
			{
				PWMSG_COUNT pCount;
	
				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x45;
				pCount.h.sizeH = SET_NUMBERH(lOfsChange);
				pCount.h.sizeL = SET_NUMBERL(lOfsChange);
				pCount.count = ChangeCount;
	
				memcpy(SendGBufChange,&pCount,sizeof(pCount));
				DataSend(aIndex,(unsigned char *)&SendGBufChange,lOfsChange);
			}
	
			if(monstercount > 0)
			{
				PWMSG_COUNT pCount;
	
				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x13;
				pCount.h.sizeH = SET_NUMBERH(MonlOfs);
				pCount.h.sizeL = SET_NUMBERL(MonlOfs);
				pCount.count = monstercount;
	
				memcpy(MonstersendBuf,&pCount,sizeof(pCount));
				DataSend(aIndex,(unsigned char *)&MonstersendBuf,MonlOfs);
			}	
	
			if(callmonstercount > 0)
			{
				PWMSG_COUNT pCount;
	
				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x1F;
				pCount.h.sizeH = SET_NUMBERH(callMonlOfs);
				pCount.h.sizeL = SET_NUMBERL(callMonlOfs);
				pCount.count = callmonstercount;
				
				memcpy(callMonstersendBuf,&pCount,sizeof(pCount));
				DataSend(aIndex,(unsigned char *)&callMonstersendBuf,callMonlOfs);
			}
	
			if(count_Item > 0)
			{
				PWMSG_COUNT pCount;
	
				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x20;
				pCount.count = count_Item;
				pCount.h.sizeH = SET_NUMBERH(lOfs_Item);
				pCount.h.sizeL = SET_NUMBERL(lOfs_Item);
				
				memcpy(ItemBuf,&pCount,sizeof(pCount));
				DataSend(aIndex,(unsigned char *)&ItemBuf,lOfs_Item);
			}
	
			if(GuildInfoCount != 0)
			{
				PWMSG_COUNT pCount;
	
				pCount.h.headcode = 0x65;
				pCount.h.c = 0xC2;
				pCount.h.sizeH = SET_NUMBERH(GuildInfoOfs);
				pCount.h.sizeL = SET_NUMBERL(GuildInfoOfs);
				pCount.count = GuildInfoCount;
				
				memcpy(GuildInfoBuf,&pCount,sizeof(pCount));
				DataSend(aIndex,(unsigned char *)&GuildInfoBuf,GuildInfoOfs);
			}
#if (__GENS_SYSTEM__ == 1)
			if( GensCount > 0 )
			{
				PMSG_GENS_VIEWPORT pCount;
				pCount.h.headcode = 0xF8;
				pCount.h.c = 0xC2;
				pCount.h.sizeH = SET_NUMBERH(lOfsGens);
				pCount.h.sizeL = SET_NUMBERL(lOfsGens);
				pCount.btSubCode = 0x05;
				pCount.btCount = GensCount;

				memcpy(GensBuff,&pCount,sizeof(pCount));
				DataSend(aIndex,(unsigned char*)&GensBuff[0],lOfsGens);
			}
#endif
		}
	}
}






void gObjSkillUseProc(LPOBJ lpObj)
{
	if( lpObj->m_SkillAttackTime2 > 0 )
	{
		lpObj->m_SkillAttackTime2--;

		if( lpObj->m_SkillAttackTime2 <= 0 )
		{
			lpObj->m_SkillAttack2 = 0;
			lpObj->m_SkillAttackTime2 = 0;
		}
	}

	if(lpObj->SkillRecallParty_Time != 0 && lpObj->Type == OBJ_USER)
	{
		lpObj->SkillRecallParty_Time--;

		if(lpObj->SkillRecallParty_Time <= 0)
		{
			lpObj->SkillRecallParty_Time = 0;

			int m_check1 = gMoveCommand.CheckMainToMove(lpObj);
			int m_check2 = gMoveCommand.CheckEquipmentToMove(lpObj,lpObj->SkillRecallParty_MapNumber);
			int m_check3 = gMoveCommand.CheckInterfaceToMove(lpObj);

			if(m_check1 == false
				|| m_check2 == false
				|| m_check3 == false)
			{
				char szMsg[256];
				wsprintf(szMsg,lMsg.Get(1254));

				GCServerMsgStringSend(szMsg,lpObj->m_Index,1);
			}
			else
			{
				gObjRecall(lpObj->m_Index,lpObj->SkillRecallParty_MapNumber,lpObj->SkillRecallParty_X,lpObj->SkillRecallParty_Y);
			}
		}
		else if(lpObj->SkillRecallParty_Time <= 5)
		{
			char szMsg[256];
			wsprintf(szMsg,lMsg.Get(1243),lpObj->SkillRecallParty_Time);

			GCServerMsgStringSend(szMsg,lpObj->m_Index,1);
		}
	}

	TMonsterSkillElementInfo::CheckSkillElementInfoProc(lpObj);

	if(lpObj->Type == OBJ_USER && lpObj->m_iItemEffectValidTime > 0)
	{
		lpObj->m_iItemEffectValidTime--;

		if(lpObj->m_iItemEffectValidTime <= 0)
		{
			lpObj->m_iItemEffectValidTime = 0;

			g_ItemAddOption.ClearItemEffect(lpObj,1);

			GCReFillSend(lpObj->m_Index,(WORD)(lpObj->MaxLife+lpObj->AddLife),0xFE,0,lpObj->iMaxShield+lpObj->iAddShield);
			GCManaSend(lpObj->m_Index,(WORD)(lpObj->MaxMana+lpObj->AddMana),0xFE,0,lpObj->MaxBP);
		}
	}

	if( IT_MAP_RANGE(lpObj->MapNumber) )
	{
		if( lpObj->iTempleIndex != -1 )
		{
			g_IllusionTemple.SkillProc(lpObj);
		}
	}

	if(lpObj->m_SkillNumber < 1)
	{
		return;
	}

	if(lpObj->m_SkillNumber == 18)
	{
		if(GetTickCount() > lpObj->m_SkillTime)
		{
			lpObj->m_SkillNumber = 0;
		}
	}
}



void gObjTeleportMagicUse(int aIndex, BYTE x, BYTE y)	// Check if x and y are BYTE
{

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Teleport != 0)
	{
		return;
	}

	lpObj->TeleportTime = GetTickCount();
	lpObj->PathCount = 0;
	lpObj->Teleport = 1;
	lpObj->m_ViewState = 1;

	lpObj->X = x;
	lpObj->Y = y;
	lpObj->TX = x;
	lpObj->TY = y;

	MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX,lpObj->m_OldY);
	MapC[lpObj->MapNumber].SetStandAttr(lpObj->TX,lpObj->TY);

	lpObj->m_OldX = lpObj->TX;
	lpObj->m_OldY = lpObj->TY;

	gObjViewportListProtocolDestroy(lpObj);
}






BOOL gObjMoveGate(int aIndex, int gt)
{
	LPOBJ lpObj = &gObj[aIndex];
	int result;
	BYTE x,y;
	BYTE mapNumber = 0,dir;
	short level;
	int userlevel = gObj[aIndex].Level;
	int movefail = 0;

	if(gObj[aIndex].SkillRecallParty_Time != 0)
	{
		char szMsg[255];
		gObj[aIndex].SkillRecallParty_Time = 0;
		wsprintf(szMsg,lMsg.Get(1254));
		GCServerMsgStringSend(szMsg,aIndex,1);
	}

	if(lpObj->Type == OBJ_USER)
	{
		gDarkSpirit[lpObj->m_Index].SetMode(CDarkSpirit::ePetItem_Mode::PetItem_Mode_Normal,-1);
	}

	if(BC_MAP_RANGE(lpObj->MapNumber))
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeLevel(lpObj->MapNumber);
		if(g_BloodCastle.GetCurrentState(iBridgeIndex) == BC_STATE_PLAYING)
		{
			g_BloodCastle.SearchUserDropQuestItem(lpObj->m_Index);
		}
		else
		{
			g_BloodCastle.SearchUserDeleteQuestItem(lpObj->m_Index);
		}
	}

	if(gObj[aIndex].RegenOk != 0)
	{
		LogAdd("Regen OK %s %d",__FILE__,__LINE__);
		x = lpObj->X;
		y = lpObj->Y;
		mapNumber = lpObj->MapNumber;
		dir = lpObj->Dir;
		movefail = 1;

		lpObj->RegenMapNumber = lpObj->MapNumber;
		lpObj->RegenMapX = x;
		lpObj->RegenMapY = y;

		gObjClearViewport(&gObj[aIndex]);

		GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);

		if(lpObj->m_Change >= 0)
		{
			gObjViewportListProtocolCreate(&gObj[aIndex]);
		}

		gObj[aIndex].RegenOk = 1;

		return false;
	}


	if(gGateC.IsGate(gt) == false)
	{

		LogAdd(lMsg.Get(545),gt,__FILE__,__LINE__);

		x = lpObj->X;
		y = lpObj->Y;
		mapNumber = lpObj->MapNumber;
		dir = lpObj->Dir;
		movefail = 1;

		lpObj->RegenMapNumber = lpObj->MapNumber;
		lpObj->RegenMapX = x;
		lpObj->RegenMapY = y;

		gObjClearViewport(&gObj[aIndex]);

		GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);

		if(lpObj->m_Change >= 0)
		{
			gObjViewportListProtocolCreate(&gObj[aIndex]);
		}

		gObj[aIndex].RegenOk = 1;

		return false;
	}

	if( (gt >= 58 && gt <= 61) ||
		 gt == 111 || gt == 112 || gt == 270 )
	{
		if(gObj[aIndex].m_bDevilSquareAuth == 0)
		{
			LogAddTD("[DevilSquare] [%s][%s] Not Used DevilSquare Invitation In DevilSquare",gObj[aIndex].AccountID,gObj[aIndex].Name);
			return false;
		}
		else
		{
			gObj[aIndex].m_bDevilSquareAuth = 0;
		}
	}

	LogAdd("Move gate after: %d", gObj[aIndex].m_MoveGateNumber);
	BYTE gateBefore = lpObj->m_MoveGateNumber;

	result = gGateC.GetGate(gt,x,y,(BYTE &)mapNumber,(BYTE &)dir,(short &)level);
	lpObj->m_MoveGateNumber = gt;

	if(result < 0)
	{
		LogAdd("error : %d (%s %d)",result,__FILE__,__LINE__);
		x = lpObj->X;
		y = lpObj->Y;
		mapNumber = lpObj->MapNumber;
		dir = lpObj->Dir;
		movefail = 1;
	}

#if (ENABLETEST_MUUN == 1)
	g_CMuunSystem.CheckMuunItemMoveMapConditionMap(lpObj, mapNumber);
#endif


	LogAdd("[user] Attempting to move, map: %d ", mapNumber);
	if (lpObj->m_bPShopOpen) //&& !pShop.CanWarp(mapNumber, aIndex))
	{
		int bMove = 1;

		if (!pShop.CheckAndCloseShop(mapNumber, aIndex))
		{
			::GCServerMsgStringSend("Please close your personal store before changing maps", aIndex, 1);
			return false;
		}

		else ::GCServerMsgStringSend("Your personal shop has been automatically closed", aIndex, 1);

		if(lpObj->MapNumber == MAP_INDEX_CASTLESIEGE && mapNumber == MAP_INDEX_RORENCIA)
		{
			bMove = 0;
		}

		if(lpObj->MapNumber == MAP_INDEX_RORENCIA && mapNumber == MAP_INDEX_CASTLESIEGE)
		{
			bMove = 0;
		}

		if(bMove == 0)
		{
			LogAddTD("[PSHOP ITEM CLONNING] Move Failed : %d (%s %s)",result,lpObj->AccountID,lpObj->Name);

			x = lpObj->X;
			y = lpObj->Y;
			mapNumber = lpObj->MapNumber;
			dir = lpObj->Dir;
			movefail = 1;

			lpObj->RegenMapX = x;
			lpObj->RegenMapY = y;
			lpObj->RegenMapNumber = mapNumber;

			lpObj->RegenMapNumber = lpObj->MapNumber;
			lpObj->RegenMapX = x;
			lpObj->RegenMapY = y;
	
			gObjClearViewport(&gObj[aIndex]);
	
			GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
	
			if(lpObj->m_Change >= 0)
			{
				gObjViewportListProtocolCreate(&gObj[aIndex]);
			}
	
			gObj[aIndex].RegenOk = 1;
		return false;
		}
	}

#if (__GENS_SYSTEM__ == 1)

	if( gGensSystem.GensMap(mapNumber) && lpObj->m_GensInfo.btFamily == GENS_NONE )
	{
		GCServerMsgStringSend("You have not joined the gens and cannot enter the battle zone.",lpObj->m_Index,1);

		x = lpObj->X;
		y = lpObj->Y;
		mapNumber = lpObj->MapNumber;
		dir = lpObj->Dir;
		movefail = 1;

		lpObj->RegenMapNumber = lpObj->MapNumber;
		lpObj->RegenMapX = x;
		lpObj->RegenMapY = y;

		gObjClearViewport(&gObj[aIndex]);

		GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);

		if(lpObj->m_Change >= 0)
		{
			gObjViewportListProtocolCreate(&gObj[aIndex]);
		}

		gObj[aIndex].RegenOk = 1;

		return false;
	}

#endif

	if(BC_MAP_RANGE(mapNumber)
		|| CC_MAP_RANGE(mapNumber)
		|| DS_MAP_RANGE(mapNumber)
		|| IT_MAP_RANGE(mapNumber) 
		|| DG_MAP_RANGE(mapNumber) )
	{
		if( gObjCheckUsedBuffEffect(lpObj,BUFF_NPC_PHANTOM) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_NPC_PHANTOM);
		}

		if( gObjCheckUsedBuffEffect(lpObj,BUFF_ADD_ATTACK) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_ADD_ATTACK);
		}

		if( gObjCheckUsedBuffEffect(lpObj,BUFF_ADD_DEFENSE) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_ADD_DEFENSE);
		}

		if( gObjCheckUsedBuffEffect(lpObj,BUFF_ADD_CRITDMG) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_ADD_CRITDMG);
		}

		if( gObjCheckUsedBuffEffect(lpObj,BUFF_SWELL_LIFE) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_SWELL_LIFE);
		}

		if( gObjCheckUsedBuffEffect(lpObj,BUFF_MANASHIELD) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_MANASHIELD);
		}
	}

	if(DS_MAP_RANGE(lpObj->MapNumber) )
	{
		if( mapNumber != lpObj->MapNumber )
		{
			g_DevilSquare.DelUser(lpObj->m_bDevilSquareIndex,lpObj->m_Index);
		}
	}

	if(IT_MAP_RANGE(lpObj->MapNumber) )
	{
		if( mapNumber != lpObj->MapNumber )
		{
			g_IllusionTemple.DropRelic(lpObj->MapNumber,lpObj->m_Index);
			g_IllusionTemple.LeaveUser(lpObj->m_Index,lpObj->MapNumber);
		}
	}
#if (GS_CASTLE == 1)

	if(result == 97)
	{
		if( g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE )
		{
			if(lpObj->m_btCsJoinSide != 1 && g_CastleSiege.GetCastleTowerAccessable() == FALSE)
			{
				GCServerMsgStringSend(lMsg.Get(MSGGET(6,88)),lpObj->m_Index,1);
				x = lpObj->X;
				y = lpObj->Y;
				mapNumber = lpObj->MapNumber;
				dir = lpObj->Dir;
				movefail = 1;
	
				lpObj->RegenMapNumber = lpObj->MapNumber;
				lpObj->RegenMapX = x;
				lpObj->RegenMapY = y;
	
				gObjClearViewport(&gObj[aIndex]);
				GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
	
				if(lpObj->m_Change >= 0)
				{
					gObjViewportListProtocolCreate(&gObj[aIndex]);
				}
				gObj[aIndex].RegenOk = 1;
				return false;
			}
			else if(lpObj->m_btCsJoinSide == 0)
			{
				GCServerMsgStringSend(lMsg.Get(MSGGET(6,89)),lpObj->m_Index,1);
				x = lpObj->X;
				y = lpObj->Y;
				mapNumber = lpObj->MapNumber;
				dir = lpObj->Dir;
				movefail = 1;
	
				lpObj->RegenMapNumber = lpObj->MapNumber;
				lpObj->RegenMapX = x;
				lpObj->RegenMapY = y;
	
				gObjClearViewport(&gObj[aIndex]);
				GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
	
				if(lpObj->m_Change >= 0)
				{
					gObjViewportListProtocolCreate(&gObj[aIndex]);
				}
				gObj[aIndex].RegenOk = 1;
				return false;
			}
		}
		else if(g_CastleSiege.CheckCastleOwnerUnionMember(lpObj->m_Index) == FALSE)
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(6,90)),lpObj->m_Index,1);
			x = lpObj->X;
			y = lpObj->Y;
			mapNumber = lpObj->MapNumber;
			dir = lpObj->Dir;
			movefail = 1;
			lpObj->RegenMapNumber = lpObj->MapNumber;
			lpObj->RegenMapX = x;
			lpObj->RegenMapY = y;
	
			gObjClearViewport(&gObj[aIndex]);
			GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
			if(lpObj->m_Change >= 0)
			{
				gObjViewportListProtocolCreate(&gObj[aIndex]);
			}
			gObj[aIndex].RegenOk = 1;
			return false;
		}
	}
#endif

	if(mapNumber == MAP_INDEX_ATHLANSE)
	{
		if(lpObj->pInventory[8].IsItem())
		{
			if(lpObj->pInventory[8].m_Type == ITEMGET(13,2))
			{
				GCServerMsgStringSend(lMsg.Get(702),lpObj->m_Index,1);

				x = lpObj->X;
				y = lpObj->Y;
				mapNumber = lpObj->MapNumber;
				dir = lpObj->Dir;
				movefail = 1;

				lpObj->RegenMapNumber = lpObj->MapNumber;
				lpObj->RegenMapX = x;
				lpObj->RegenMapY = y;
				gObjClearViewport(&gObj[aIndex]);
				GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
				if(lpObj->m_Change >= 0)
				{
					gObjViewportListProtocolCreate(&gObj[aIndex]);
				}
				gObj[aIndex].RegenOk = 1;
				return false;
			}

			if(lpObj->pInventory[8].m_Type == ITEMGET(13,3))
			{
				GCServerMsgStringSend(lMsg.Get(1604),lpObj->m_Index,1);

				x = lpObj->X;
				y = lpObj->Y;
				mapNumber = lpObj->MapNumber;
				dir = lpObj->Dir;
				movefail = 1;

				lpObj->RegenMapNumber = lpObj->MapNumber;
				lpObj->RegenMapX = x;
				lpObj->RegenMapY = y;
				gObjClearViewport(&gObj[aIndex]);
				GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
				if(lpObj->m_Change >= 0)
				{
					gObjViewportListProtocolCreate(&gObj[aIndex]);
				}
				gObj[aIndex].RegenOk = 1;
				return false;
			}
		}
	}

	if(mapNumber == MAP_INDEX_ICARUS)
	{
		if(!(lpObj->pInventory[8].m_Type == ITEMGET(13,3)
			|| lpObj->pInventory[7].m_Type == ITEMGET(13,30)
			|| lpObj->pInventory[7].m_Type == ITEMGET(12,49)
			|| lpObj->pInventory[8].m_Type == ITEMGET(13,37)
			|| lpObj->pInventory[8].m_Type == ITEMGET(13,4)
			|| lpObj->pInventory[7].IsItem() != FALSE )
			|| lpObj->pInventory[8].m_Type == ITEMGET(13,2)
			|| IsTransformEquipment(lpObj->m_Index) )
		{
			GCServerMsgStringSend(lMsg.Get(1604),lpObj->m_Index,1);

			x = lpObj->X;
			y = lpObj->Y;
			mapNumber = lpObj->MapNumber;
			dir = lpObj->Dir;
			movefail = 1;

			lpObj->RegenMapNumber = lpObj->MapNumber;
			lpObj->RegenMapX = x;
			lpObj->RegenMapY = y;
			
			GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
			gObjClearViewportOfMine(lpObj);

			if(lpObj->m_Change >= 0)
			{
				gObjViewportListProtocolCreate(&gObj[aIndex]);
			}
			gObj[aIndex].RegenOk = 1;
			return false;
		}

		if(lpObj->m_RecallMon >= 0)
		{
			GCRecallMonLife(aIndex,60,0);
			gObjMonsterCallKill(aIndex);
		}
	}

	if(!KALIMA_MAP_RANGE(mapNumber) && KALIMA_MAP_RANGE(lpObj->MapNumber))
	{
		int iKalimaGateIndex = lpObj->m_iKalimaGateIndex;

		if(g_KalimaGate.DeleteKalimaGate(lpObj->m_Index))
		{
			LogAddTD("[Kalima] [%s][%s] Kalima Gate Vanished - User MoveGate (SummonIndex:%d, EnterCount:%d)",lpObj->AccountID,lpObj->Name,iKalimaGateIndex,gObj[iKalimaGateIndex].m_cKalimaGateEnterCount);
		}
	}
#if (GS_CASTLE != 1 || GS_SPEC == 1)
	if(mapNumber == MAP_INDEX_KANTURU_BOSS)
	{
		if(!(lpObj->pInventory[8].m_Type == ITEMGET(13,3)
			|| lpObj->pInventory[7].m_Type == ITEMGET(13,30)
			|| lpObj->pInventory[7].m_Type == ITEMGET(12,49)
			|| lpObj->pInventory[8].m_Type == ITEMGET(13,37)
			|| lpObj->pInventory[7].IsItem() != FALSE )
			|| IsTransformEquipment(lpObj->m_Index) )
		{
			GCServerMsgStringSend(lMsg.Get(1604),lpObj->m_Index,1);

			x = lpObj->X;
			y = lpObj->Y;
			mapNumber = lpObj->MapNumber;
			dir = lpObj->Dir;
			movefail = 1;

			lpObj->RegenMapNumber = lpObj->MapNumber;
			lpObj->RegenMapX = x;
			lpObj->RegenMapY = y;
			
			GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
			gObjClearViewportOfMine(lpObj);

			if(lpObj->m_Change >= 0)
			{
				gObjViewportListProtocolCreate(&gObj[aIndex]);
			}
			gObj[aIndex].RegenOk = 1;
			return false;
		}
	}
#endif

	if(g_iBlockKanturuMapEnter == 1)
	{
		x = lpObj->X;
		y = lpObj->Y;
		mapNumber = lpObj->MapNumber;
		dir = lpObj->Dir;
		level = lpObj->Level;

		if(gGateC.GetGate(gt,x,y,(BYTE &)mapNumber,(BYTE &)dir,(short &)level) != -1)
		{
			if(mapNumber >= MAP_INDEX_KANTURU1 && mapNumber <= MAP_INDEX_KANTURU_BOSS)
			{
				GCServerMsgStringSend(lMsg.Get(1604),lpObj->m_Index,1);
				x = lpObj->X;
				y = lpObj->Y;
				mapNumber = lpObj->MapNumber;
				dir = lpObj->Dir;
				movefail = 1;

				lpObj->RegenMapNumber = lpObj->MapNumber;
				lpObj->RegenMapX = x;
				lpObj->RegenMapY = y;
				
				GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
				gObjClearViewportOfMine(lpObj);

				if(lpObj->m_Change >= 0)
				{
					gObjViewportListProtocolCreate(&gObj[aIndex]);
				}
				gObj[aIndex].RegenOk = 1;
				return false;
			}
		}
	}

	if(g_iBlockCastleSiegeMapEnter == TRUE)
	{
		x = lpObj->X;
		y = lpObj->Y;
		mapNumber = lpObj->MapNumber;
		dir = lpObj->Dir;
		level = lpObj->Level;

		if(gGateC.GetGate(gt,x,y,(BYTE &)mapNumber,(BYTE &)dir,(short &)level) != -1)
		{
			if(mapNumber == MAP_INDEX_CASTLESIEGE
				|| mapNumber == MAP_INDEX_CASTLEHUNTZONE
				|| mapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE
				|| mapNumber == MAP_INDEX_CRYWOLF_SECONDZONE)
			{
				GCServerMsgStringSend(lMsg.Get(1604),lpObj->m_Index,1);
				x = lpObj->X;
				y = lpObj->Y;
				mapNumber = lpObj->MapNumber;
				dir = lpObj->Dir;
				movefail = 1;

				lpObj->RegenMapNumber = lpObj->MapNumber;
				lpObj->RegenMapX = x;
				lpObj->RegenMapY = y;
				
				GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
				gObjClearViewportOfMine(lpObj);

				if(lpObj->m_Change >= 0)
				{
					gObjViewportListProtocolCreate(&gObj[aIndex]);
				}
				gObj[aIndex].RegenOk = 1;
				return false;
			}
		}
	}

	if( mapNumber == MAP_INDEX_RAKLIONBOSS )
	{
		if( g_Raklion.GetRaklionState() >= RAKLION_STATE_CLOSE_DOOR )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(13,87)),lpObj->m_Index,1);
			x = lpObj->X;
			y = lpObj->Y;
			mapNumber = lpObj->MapNumber;
			dir = lpObj->Dir;
			movefail = 1;

			lpObj->RegenMapNumber = lpObj->MapNumber;
			lpObj->RegenMapX = x;
			lpObj->RegenMapY = y;
				
			GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
			gObjClearViewportOfMine(lpObj);

			if(lpObj->m_Change >= 0)
			{
				gObjViewportListProtocolCreate(&gObj[aIndex]);
			}
			gObj[aIndex].RegenOk = 1;
			return false;
		}
	}

	if( gt == 290 )
	{
		//g_RaklionUser.AddUser(aIndex);
		g_RaklionBattleUserMng.AddUserData(aIndex);
	}

	if( mapNumber == MAP_INDEX_RAKLION )
	{
		if( gt == 292 )
		{
			if( g_Raklion.GetRaklionState() >= RAKLION_STATE_CLOSE_DOOR )
			{
				GCServerMsgStringSend(lMsg.Get(MSGGET(13,88)),lpObj->m_Index,1);
				x = lpObj->X;
				y = lpObj->Y;
				mapNumber = lpObj->MapNumber;
				dir = lpObj->Dir;
				movefail = 1;
	
				lpObj->RegenMapNumber = lpObj->MapNumber;
				lpObj->RegenMapX = x;
				lpObj->RegenMapY = y;
					
				GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
				gObjClearViewportOfMine(lpObj);
	
				if(lpObj->m_Change >= 0)
				{
					gObjViewportListProtocolCreate(&gObj[aIndex]);
				}
				gObj[aIndex].RegenOk = 1;
				return false;
			}

			g_RaklionBattleUserMng.DeleteUserData(aIndex);
		}
		else
		{
			g_RaklionUtil.NotifyRaklionCurrentState(aIndex, g_Raklion.GetRaklionState(), g_Raklion.GetRaklionDetailState());
		}
	}

#if (ENABLETEST_ARCA)
	if ((mapNumber == MAP_INDEX_ACHERON_ARCABATTLE || mapNumber == MAP_INDEX_DEBENTER_ARCABATTLE)
		&& g_AcheronGuardianEvent.GetProcState() < 3)
    {
		if (g_ArcaBattle.GetState() < AB_STATE_PLAY)
		{
			x = lpObj->X;
			y = lpObj->Y;
			mapNumber = lpObj->MapNumber;
			dir = lpObj->Dir;
			movefail = 1;
			lpObj->RegenMapNumber = lpObj->MapNumber;
			lpObj->RegenMapX = x;
			lpObj->RegenMapY = y;

			GCTeleportSend(lpObj, gt, mapNumber, lpObj->X, lpObj->Y, lpObj->Dir);
			gObjClearViewportOfMine(lpObj);

			if (lpObj->m_Change >= 0)
			{
				gObjViewportListProtocolCreate(lpObj);
			}

			lpObj->RegenOk = 1;
			return 0;
		}
		
		BYTE mx = 0;
		BYTE my = 0;

		if (g_ArcaBattle.GetGuildMasterPos(*lpObj, gt, mx, my))
		{
			x = mx;
			y = my;
		}
		else if (g_ArcaBattle.GetUserPos(gt, mx, my))
		{
			x = mx;
			y = my;
		}
		  
		g_ArcaBattle.SetUserMapInfo(lpObj, gt);
    }
#endif

	if(mapNumber != MAP_INDEX_SWAMPOFCALMNESS )
	{

		if(lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER )
		{
			if(level > 0)
			{
				level = (level / 3)*2;
			}
		}
	}

	if(level != 0)
	{
		if(userlevel < level)
		{
			char szTemp[256];
			wsprintf(szTemp,lMsg.Get(1117),level);
			GCServerMsgStringSend(szTemp,aIndex,1);

			x = lpObj->X;
			y = lpObj->Y;
			mapNumber = lpObj->MapNumber;
			dir = lpObj->Dir;
			movefail = 1;
		}
	}

	if(gObj[aIndex].m_IfState.use != 0)
	{
		x = lpObj->X;
		y = lpObj->Y;
		mapNumber = lpObj->MapNumber;
		dir = lpObj->Dir;
		movefail = 1;
	}

	if(gObj[aIndex].DieRegen != 0)
	{
		x = lpObj->X;
		y = lpObj->Y;
		mapNumber = lpObj->MapNumber;
		dir = lpObj->Dir;
		movefail = 1;
	}

	lpObj->m_State = 32;

	if(movefail == 0)
	{
		if(gGateC.GetGate(gt,x,y,(BYTE &)mapNumber,(BYTE &)dir,(short &)level) != -1)
		{
			LogAdd("Gate not -1");
			if(gObj[aIndex].MapNumber != mapNumber)
			{
				if(MapNumberCheck(mapNumber)== 0)
				{
					LogAdd("error-L3 : %s %d",__FILE__,__LINE__);
					return false;
				}

				LogAdd("Attempting to get svr code");
				short sSvrCode = g_MapServerManager.CheckMoveMapSvr(lpObj->m_Index,mapNumber,lpObj->m_sPrevMapSvrCode);

				if(sSvrCode != gGameServerCode)
				{
					if(sSvrCode == -1)
					{
						LogAddC(2,"[MapServerMng] Map Server Move Fail : CheckMoveMapSvr() == -1 [%s][%s] (%d)",lpObj->AccountID,lpObj->Name,lpObj->m_Index);
						return false;
					}

					if(gGateC.CheckGateLevel(aIndex,gt) == 0)
					{
						LogAdd("Gate check fail");
						x = lpObj->X;
						y = lpObj->Y;
						mapNumber = lpObj->MapNumber;
						dir = lpObj->Dir;
						movefail = 1;

						lpObj->RegenMapNumber = lpObj->MapNumber;
		
						lpObj->RegenMapX = x;
						lpObj->RegenMapY = y;
				
						gObjClearViewport(&gObj[aIndex]);
				
						GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
				
						if(lpObj->m_Change >= 0)
						{
							gObjViewportListProtocolCreate(&gObj[aIndex]);
						}
	
						gObj[aIndex].RegenOk = 1;

#if (GS_CASTLE == 1)
						if(gObj[aIndex].MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
						{
							g_CastleSiege.NotifySelfCsJoinSide(aIndex);
							g_CastleSiege.NotifyCsSelfLeftTime(aIndex);
						}
#endif

						return false;
					}

					LogAdd("reached GJReqMapSvrMove");
					GJReqMapSvrMove(lpObj->m_Index,sSvrCode,mapNumber,x,y);
					LogAddTD("[MapServerMng] Request to Move Map Server : (%d) - [%s][%s] (%d)",sSvrCode,lpObj->AccountID,lpObj->Name,lpObj->m_Index);
					return false;
				}
			}
		}

		LogAddTD("[MoveMap][%s][%s] : (Before:Map:%d X:%d Y:%d) -> (New Map:%d X:%d Y:%d)",lpObj->AccountID,lpObj->Name,lpObj->MapNumber,lpObj->X,lpObj->Y,mapNumber,x,y);
		
		int nCurrentMap = gObj[aIndex].MapNumber;
		gObj[aIndex].X = x;
		gObj[aIndex].Y = y;
		gObj[aIndex].TX = x;
		gObj[aIndex].TY = y;
		gObj[aIndex].MapNumber = mapNumber;
		gObj[aIndex].Dir = dir;
		gObj[aIndex].PathCount = 0;
		gObj[aIndex].Teleport = 0;

		gObjClearViewport(&gObj[aIndex]);
		GCTeleportSend(&gObj[aIndex],gt,mapNumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);
		gObjViewportListProtocolCreate(&gObj[aIndex]);
		gObjClearViewport(&gObj[aIndex]);

//		g_MiniMap.SendMinimap(aIndex);

		if(lpObj->m_Change >= 0)
		{
			gObjViewportListProtocolCreate(&gObj[aIndex]);
		}

		gObj[aIndex].RegenMapNumber = mapNumber;
		gObj[aIndex].RegenMapX = x;
		gObj[aIndex].RegenMapY = y;
		gObj[aIndex].RegenOk = 1;

		
		if (nCurrentMap != mapNumber)
		{
			g_Minimap.SendNpcCoordinate(aIndex, mapNumber, 0);
			g_Minimap.SendPortalCoordinate(aIndex, mapNumber);
		}
	}

	else
	{
		GCTeleportSend(&gObj[aIndex], gateBefore, gObj[aIndex].MapNumber, gObj[aIndex].X, gObj[aIndex].Y, gObj[aIndex].Dir);
		gObj[aIndex].RegenOk = 1;

#if (GS_CASTLE == 1)
		if(lpObj->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			g_CastleSiege.NotifySelfCsJoinSide(aIndex);
			g_CastleSiege.NotifyCsSelfLeftTime(aIndex);
		}
#endif
	}

	if(movefail != 0)
	{
		return false;
	}

	if(gObj[aIndex].Type == OBJ_USER)
	{
		gObj[aIndex].m_LastTeleportTime = 10;
	}

#if (ENABLETEST_NEWPVP == 1)
	g_NewPVP.Reset(*lpObj); // 1.00.92
#endif

	return true;
}






void gObjTeleport(int aIndex, int map, int x, int y)
{

	LogAdd("[gObjTeleport] attempting to move: %d", map);
	if(aIndex < 0 || aIndex > OBJMAX - 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	
	if(MapNumberCheck(map)==0)
	{
		LogAdd("error-L3 : %s %d",__FILE__,__LINE__);
		return;
	}

	if(gObj[aIndex].MapNumber != map)
	{
		LogAdd("[gObjTeleport] attempting to move: %d", map);
		short sSvrCode = g_MapServerManager.CheckMoveMapSvr(lpObj->m_Index,map,lpObj->m_sPrevMapSvrCode);

		if(sSvrCode != gGameServerCode)
		{
			if(sSvrCode == -1)
			{
				LogAdd("[gObjTeleport] svrcode == -1");
				LogAddC(2,"[MapServerMng] Map Server Move Fail : CheckMoveMapSvr() == -1 [%s][%s] (%d)",lpObj->AccountID,lpObj->Name,lpObj->m_Index);
				return;
			}

			GJReqMapSvrMove(lpObj->m_Index,sSvrCode,map,x,y);
			LogAddTD("[MapServerMng] Request to Move Map Server : (%d) - [%s][%s] (%d)",sSvrCode,lpObj->AccountID,lpObj->Name,lpObj->m_Index);
			return;
		}
	}

	if(lpObj->m_IfState.use == 1 && lpObj->m_IfState.type == 1)
	{
		CGTradeCancelButtonRecv(lpObj->m_Index);
	}

	if(lpObj->m_IfState.use == 1 && lpObj->m_IfState.type == 6)
	{
		CGWarehouseUseEnd(lpObj->m_Index);
	}

	lpObj->m_State = 32;
	gObj[aIndex].X = x;
	gObj[aIndex].Y = y;

	gObj[aIndex].TX = x;
	gObj[aIndex].TY = y;

	gObj[aIndex].MapNumber = map;
	gObj[aIndex].PathCount = 0;
	gObj[aIndex].Teleport = 0;

	gObjClearViewport(&gObj[aIndex]);

	GCTeleportSend(&gObj[aIndex],1,map,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);

	if(lpObj->m_Change >= 0)
	{
		gObjViewportListProtocolCreate(&gObj[aIndex]);
	}

	gObj[aIndex].RegenMapNumber = map;
	gObj[aIndex].RegenMapX = x;
	gObj[aIndex].RegenMapY = y;
	gObj[aIndex].RegenOk = 1;
}






void gObjMoveDataLoadingOK(int aIndex)
{
	if(gObj[aIndex].RegenOk == 1)
	{
		gObj[aIndex].RegenOk = 2;
	}
}






class CMagicInf* gObjGetMagic(LPOBJ lpObj, int mIndex)
{
	if ( mIndex < 0 || mIndex > MAX_MAGIC-1 )
	{
		LogAdd(lMsg.Get(MSGGET(2, 34)), __FILE__, __LINE__);
		return NULL;
	}

	if ( lpObj->Magic[mIndex].IsMagic() == FALSE )
	{
		return NULL;
	}

	return &lpObj->Magic[mIndex];
}


CMagicInf * gObjGetMagicSearch(LPOBJ lpObj, int skillnumber)
{

	if(skillnumber == 58)
	{
		skillnumber = 40;
	}

	if(skillnumber == 395 )
	{
		skillnumber = AT_MSKILL_DW_NOVA1;
	}

	for(int n = 0; n < MAGIC_SIZE;n++)
	{
		if(lpObj->Magic[n].IsMagic() == 1)
		{
			if(lpObj->Magic[n].m_Skill == skillnumber)
			{
				if(gObjMagicEnergyCheck(lpObj,lpObj->Magic[n].m_Skill) == 0)
				{
					GCServerMsgStringSend(lMsg.Get(1255),lpObj->m_Index,1);
					return false;
				}
				else
				{
					return &lpObj->Magic[n];
				}
			}
		}
	}
	return false;
}






int	gObjMagicManaUse(LPOBJ lpObj, CMagicInf * lpMagic)
{
	float mana = lpObj->Mana;

	if(lpObj->Type != OBJ_USER)
	{
		return false;
	}

	if(lpMagic == 0)
	{
		return false;
	}

	float usemana = 0;

	if( g_MasterSkillSystem.CheckMasterLevelSkill(lpMagic->m_Skill) )
	{
		int nMana = MagicDamageC.SkillGetMana(lpMagic->m_Skill);
		usemana = nMana;
		usemana += (nMana/100.0f)*g_MasterSkillSystem.GetMasterSkillManaIncRate(lpMagic->m_Level);
	}
	else
	{
		usemana = (float)MagicDamageC.SkillGetMana(lpMagic->m_Skill);
	}

	usemana -= usemana*lpObj->m_MPSkillOpt.MpsDecreaseMana/100.0f;

	if( usemana < 1 )
		usemana = 1;

	mana -= usemana;

	if(mana < 0)
	{
		return -1;
	}

	return (int)mana;
}






int  gObjMagicBPUse(LPOBJ lpObj, CMagicInf * lpMagic)
{
	int bp = lpObj->BP;

	if( lpObj->Type != OBJ_USER )
	{
		return false;
	}

	if( lpMagic == 0 )
	{
		return false;
	}

	int usingbp = MagicDamageC.SkillGetBP(lpMagic->m_Skill);
	usingbp -= usingbp * lpObj->SetOpDecreaseAG / 100;

	if( lpObj->m_btSocketBPUseDec > 0 )
	{
		usingbp -= usingbp * lpObj->m_btSocketBPUseDec / 100;
	}

	bp -= usingbp;

	if(bp < 0)
	{
		return -1;
	}

	return bp;
}



struct PMSG_POSMAGIC_RESULT {
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_POSMAGIC_COUNT {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE MapX;
	BYTE MapY;
	BYTE Count;
};

int gObjPosMagicAttack(LPOBJ lpObj, CMagicInf * lpMagic, BYTE x, BYTE y)	// Not used
{
	/*
	int tObjNum;
	int dis;
	int tx;
	int ty;
	int MagicDistance;
	unsigned char sbuf[256];
	int lOfs;
	int MagicNumber;
	PMSG_POSMAGIC_COUNT pCount;
	PMSG_POSMAGIC_RESULT pResult;
	int MVL;
	int n;

	MagicDistance = 3;
	lOfs = 0;
	MagicNumber = lpMagic->m_Skill;

	switch(MagicNumber)
	{
	case 9: MagicDistance = 3;
		break;
	default: MagicDistance = 3;
		break;
	}

	lOfs = sizeof(pCount);
	pCount.h.c = 0xC1;
	pCount.h.headcode = 0x1A;
	pCount.h.size = 0;

	pCount.MapX = x;
	pCount.MapY = y;

	pCount.MagicNumberH = SET_NUMBERH(MagicNumber);
	pCount.MagicNumberL = SET_NUMBERL(MagicNumber);

	pCount.NumberH = SET_NUMBERH(lpObj->m_Index);
	pCount.NumberL = SET_NUMBERL(lpObj->m_Index);
	pCount.Count = 0;

	MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	for(n = 0; n < MVL; n++)
	{
		if(lpObj->VpPlayer[n].state)
		{
			tObjNum = lpObj->VpPlayer[n].number;

			if(tObjNum >= 0)
			{
				if(gObj[tObjNum].Live)
				{
					tx = lpObj->X - gObj[tObjNum].X;
					ty = lpObj->Y - gObj[tObjNum].Y;

					dis = sqrt(tx*tx + ty*ty);

					if(dis <= MagicDistance)
					{
						if(gObjAttack(lpObj,&gObj[tObjNum],lpMagic,0,0,0,0) == 1)
						{
							pResult.NumberH = SET_NUMBERH(tObjNum);
							pResult.NumberL = SET_NUMBERL(tObjNum);

							memcpy(&sbuf[lOfs],&pResult,sizeof(pResult));
							pCount.Count++;
							lOfs+=sizeof(pResult);

							if(lOfs > 250)
							{
								LogAdd(lMsg.Get(547),__FILE__,__LINE__);
								return false;
							}
						}
					}
				}
			}
		}
	}

	pCount.h.size = lOfs;

	memcpy(sbuf,&pCount,sizeof(pCount));

	if(lpObj->Type == OBJ_USER)
	{
		DataSend(lpObj->m_Index,sbuf,lOfs);
	}

	MsgSendV2(lpObj,sbuf,lOfs);*/
	return true;
}






void gObjMagicAddEnergyCheckOnOff(int flag)
{
	g_EnergyCheckOff=flag;
}






int  gObjMagicAdd(LPOBJ lpObj, BYTE Type, BYTE Index, BYTE Level, WORD & SkillNumber)
{

	int skill = -1,n,reqeng,reqleadership;

	skill = GetSkillNumberInex(Type,Index,Level);

	if(skill < 0)
	{
		LogAdd(lMsg.Get(548));
		return -1;
	}

	if(g_EnergyCheckOff == 1)
	{
		reqeng = MagicDamageC.SkillGetRequireEnergy(skill);

		if(reqeng < 0)
		{
			LogAdd(lMsg.Get(549));
			return -1;
		}

		if((lpObj->Energy + lpObj->AddEnergy) < reqeng)
		{
			LogAdd(lMsg.Get(549));
			return -1;
		}
	}

	reqleadership = MagicDamageC.GetskillRequireLeadership(skill);

	if(reqleadership < 0)
	{
		LogAdd("error-L2: Skill Leadership not enough");
		return -1;
	}

	if((lpObj->Leadership + lpObj->AddLeadership) < reqleadership)
	{
		LogAdd("error-L2: Skill Leadership not enough");
		return -1;
	}

	if(MagicDamageC.SkillGetRequireClass(lpObj->Class,lpObj->ChangeUP,lpObj->ChangeUP2,skill) < 1)
	{
		LogAdd(lMsg.Get(550));
		return -1;
	}

	if( g_MasterSkillSystem.IsExistMLSBasedSkill(lpObj,skill) == TRUE )
	{
		return -1;
	}

	for(n = 0; n < MAGIC_SIZE; n++)
	{
		if(lpObj->Magic[n].IsMagic() == 1)
		{
			skill = GetSkillNumberInex(Type,Index,Level);

			if(skill < 0)
			{
				LogAdd(lMsg.Get(551),__FILE__,__LINE__);
				return -1;
			}

			if(lpObj->Magic[n].m_Skill == skill)
			{
				LogAdd(lMsg.Get(552));
				return -1;
			}
		}
	}

	skill = -1;

	for(n = 0; n < MAGIC_SIZE; n++)
	{
		if(lpObj->Magic[n].IsMagic() == 0)
		{
			skill = lpObj->Magic[n].Set(Type,Index,Level);
			if(skill < 0)
			{
				LogAdd(lMsg.Get(553),__FILE__,__LINE__);
				return -1;
			}
			else
			{
				lpObj->MagicCount++;
				SkillNumber = skill;
				return n;
			}
		}
	}


	LogAdd(lMsg.Get(554),__FILE__,__LINE__);
	return -1;
}






int  gObjMagicDel(LPOBJ lpObj, int aSkill, BYTE Level)
{

	for(int n = 0; n < MAGIC_SIZE; n++)
	{
		if(lpObj->Magic[n].IsMagic() == 1)
		{
			if(lpObj->Magic[n].m_Skill == aSkill)
			{
				lpObj->Magic[n].Clear();
				return n;
			}
		}
	}
	return -1;
}






int  gObjMagicAdd(LPOBJ lpObj, int aSkill, BYTE Level)
{
	if( g_MasterSkillSystem.MLS_WeaponSkillDel(lpObj,aSkill,Level) )
		return -1;

	int skill = -1,n,reqeng;

	skill = -1;

	if(g_EnergyCheckOff == 1)
	{
		reqeng = MagicDamageC.SkillGetRequireEnergy(aSkill);

		if(reqeng < 0)
		{
			LogAdd(lMsg.Get(549));
			return -1;
		}

		if((lpObj->Energy + lpObj->AddEnergy) < reqeng)
		{
			LogAdd(lMsg.Get(549));
			return -1;
		}
	}

	if(MagicDamageC.SkillGetRequireClass(lpObj->Class,lpObj->ChangeUP,lpObj->ChangeUP2,aSkill) < 1)
	{
		LogAdd(lMsg.Get(550));
		return -1;
	}

	if(g_MasterSkillSystem.IsExistMLSBasedSkill(lpObj,skill) == TRUE )
	{
		return -1;
	}

	for(n = 0; n < MAGIC_SIZE; n++)
	{
		if(lpObj->Magic[n].IsMagic() == 1)
		{
			if(lpObj->Magic[n].m_Skill == aSkill)
			{
				LogAdd(lMsg.Get(555),lpObj->Magic[n].m_Skill,aSkill);
				return -1;
			}
		}
	}

	for(n = 0; n < MAGIC_SIZE; n++)
	{
		if(lpObj->Magic[n].IsMagic() == 0)
		{
			skill = lpObj->Magic[n].Set(aSkill,Level);
			if(skill < 0)
			{
				LogAdd(lMsg.Get(553),__FILE__,__LINE__);
				return -1;
			}
			else
			{
				lpObj->MagicCount++;
				return n;
			}
		}
	}

	LogAdd(lMsg.Get(554),__FILE__,__LINE__);
	return -1;
}






int gObjWeaponMagicAdd(LPOBJ lpObj, WORD aSkill, BYTE Level)
{
	int skill = -1,n;
	skill = -1;

	if(MagicDamageC.SkillGetRequireClass(lpObj->Class,lpObj->ChangeUP,lpObj->ChangeUP2,aSkill) < 1)
	{
		return -1;
	}

	if( g_MasterSkillSystem.MLS_WeaponSkillAdd(lpObj,aSkill,Level) )
		return -1;

	for(n = 0; n < MAGIC_SIZE; n++)
	{
		if(lpObj->Magic[n].IsMagic() == 0)
		{
			skill = lpObj->Magic[n].Set(aSkill,Level);
			if(skill < 0)
			{
				LogAdd(lMsg.Get(553),__FILE__,__LINE__);
				return -1;
			}
			else
			{
				lpObj->MagicCount++;
				return n;
			}
		}
	}
	LogAdd(lMsg.Get(554),__FILE__,__LINE__);
	return -1;
}






int gObjMonsterMagicAdd(LPOBJ lpObj, WORD aSkill, BYTE Level)
{
	int skill = -1;

	for (int n=0; n<MAGIC_SIZE ; n++)
	{
		if ( lpObj->Magic[n].IsMagic() == FALSE )
		{
			skill = lpObj->Magic[n].Set(aSkill, Level);

			if ( skill < 0 )
			{
				LogAdd(lMsg.Get( MSGGET(2, 41)),__FILE__, __LINE__);
				return -1;
			}

			lpObj->MagicCount++;
			return n;
		}
	}

	LogAdd(lMsg.Get( MSGGET(2, 42)), __FILE__, __LINE__);
	return -1;
}






BOOL gObjMagicEnergyCheck(LPOBJ lpObj , WORD aSkill)
{
	int reqeng,reqlevel;

	reqeng = MagicDamageC.SkillGetRequireEnergy(aSkill);

	if(reqeng < 0)
	{
		return false;
	}

	if((lpObj->Energy + lpObj->AddEnergy) < reqeng)
	{
		return false;
	}

	reqlevel = MagicDamageC.SkillGetRequireLevel(aSkill);

	if(lpObj->Level < reqlevel)
	{
		return false;
	}
return true;
}






BOOL gObjItemLevelUp(LPOBJ lpObj, int source, int target)
{
	if(source < 0 || source > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(target < 0 || target > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	if( lpObj->pInventory[target].m_Type == ITEMGET(0,41) )
	{
		if ( gObjItemLevelUpPickAxe(lpObj, source, target) == TRUE )
		{
			return true;
		}
		else
		{
			GCReFillSend(lpObj->m_Index, (WORD)lpObj->Life, 0xFD, 1, lpObj->iShield);
			return false;
		}
	}

	if(lpObj->pInventory[target].m_Type == ITEMGET(13,37))
	{
		CItem * ItemFenrir = &lpObj->pInventory[target];

		if(ItemFenrir->m_Durability >= 255)
		{
			return false;
		}

		if(rand()% 10000 < g_iFenrirRepairRate)
		{
			int iAddDur = rand()%150 + 50;

			if((ItemFenrir->m_Durability + iAddDur) > 255)
			{
				ItemFenrir->m_Durability = 255.0f;
			}
			else
			{
				ItemFenrir->m_Durability += iAddDur;
			}

			MsgOutput(lpObj->m_Index,lMsg.Get(3342),int(ItemFenrir->m_Durability));

			LogAddTD("[FENRIR REPAIR][SUCCESS] [%s][%s] - %d/255 (+%d)",lpObj->AccountID,lpObj->Name,ItemFenrir->m_Durability,iAddDur);
		}
		else
		{
			MsgOutput(lpObj->m_Index,lMsg.Get(3343));

			LogAddTD("[FENRIR REPAIR][FAILED] [%s][%s] - %d/255 (+%d)",lpObj->AccountID,lpObj->Name,ItemFenrir->m_Durability);
		}

		return true;
	}

	CItem * _target = &lpObj->pInventory[target];

	if( _target->m_bLuckySet == TRUE )
		return false;

	if( _target->m_Type >= ITEMGET(12,36) && 
		_target->m_Type <= ITEMGET(12,40)	)
	{

	}
#if (CUSTOM_WINGS == 1)
	else if( _target->m_Type >= ITEMGET(12,440) && _target->m_Type <= ITEMGET(12,445) )
	{

	}
#endif
	else if( _target->m_Type >= ITEMGET(12,41) &&
			 _target->m_Type <= ITEMGET(12,43) )
	{

	}
	else if( _target->m_Type == ITEMGET(12,49) ||
			 _target->m_Type == ITEMGET(12,50) )
	{

	}
	else if( _target->m_Type >= ITEMGET(12,200) && _target->m_Type <= ITEMGET(12,214) )
	{

	}
	else if( _target->m_Type >= ITEMGET(12,262) &&
			 _target->m_Type <= ITEMGET(12,265) )
	{

	}
	else if(!(_target->m_Type < ITEMGET(12,7)
		|| _target->m_Type == ITEMGET(13,30) )
		|| _target->m_Type == ITEMGET(4,7)
		|| _target->m_Type == ITEMGET(4,15))
	{
		return false;
	}

	LogAddTD(lMsg.Get(556),lpObj->AccountID,lpObj->Name,lpObj->pInventory[target].GetName(),lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Level);

	lpObj->pInventory[target].m_Level++;

	if(lpObj->pInventory[target].m_Level > 6)
	{
		lpObj->pInventory[target].m_Level = 6;
	}

	gObjMakePreviewCharSet(lpObj->m_Index);

	float levelitemdur = (float)ItemGetDurability(lpObj->pInventory[target].m_Type,lpObj->pInventory[target].m_Level,lpObj->pInventory[target].IsExtItem(),lpObj->pInventory[target].IsSetItem());

	lpObj->pInventory[target].m_Durability = levelitemdur * lpObj->pInventory[target].m_Durability / lpObj->pInventory[target].m_BaseDurability;

	lpObj->pInventory[target].Convert(
			lpObj->pInventory[target].m_Type,
			lpObj->pInventory[target].m_Option1,
			lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,
			lpObj->pInventory[target].m_NewOption,
			lpObj->pInventory[target].m_SetOption,
			lpObj->pInventory[target].m_ItemOptionEx,0,-1,
			CURRENT_DB_VERSION);

	LogAddTD(lMsg.Get(557),lpObj->AccountID,lpObj->Name,lpObj->pInventory[source].m_Number,
			lpObj->pInventory[target].GetName(),lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Level);
	return true;
}

BOOL gObjItemLevelUpPickAxe(LPOBJ lpObj, int source, int target)
{
	if( source < 0 || source > MAIN_INVENTORY_SIZE-1 )
	{
		return false;
	}

	if( target < 0 || target > MAIN_INVENTORY_SIZE-1 )
	{
		return false;
	}

	if( lpObj->pInventory[source].IsItem() == 0 )
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	CItem* PickAxe = &lpObj->pInventory[target];

	if( PickAxe->m_Type != ITEMGET(0,41) )
	{
		return false;
	}

	CItem* Jewel = &lpObj->pInventory[source];

	if( PickAxe->m_Durability >= 255 )
	{
		return false;
	}

	int oDur = PickAxe->m_Durability;
	int iAddDurability = 4;

	if( Jewel->m_Type == ITEMGET(12,30) )
	{
		iAddDurability = 40*(lpObj->pInventory[source].m_Level+1);
	}	

	if( (PickAxe->m_Durability+iAddDurability) >= 255 )
	{
		PickAxe->m_Durability = 255;
	}
	else
	{
		PickAxe->m_Durability += iAddDurability;
	}

	LogAddTD("(%s)(%s) PickAxe repair - Item[%d] Level[%d] [%d -> %d]",
		lpObj->AccountID,lpObj->Name,Jewel->m_Type,Jewel->m_Level,oDur,(int)PickAxe->m_Durability);
	return true;
}


BOOL gObjItemRandomLevelUp(LPOBJ lpObj, int source, int target)
{
	if(source < 0 || source > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(target < 0 || target > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	CItem * _target = &lpObj->pInventory[target];

	if( _target->m_bLuckySet == TRUE )
		return false;

	if( _target->m_Type >= ITEMGET(12,36) &&
		_target->m_Type <= ITEMGET(12,40) )
	{

	}
#if (CUSTOM_WINGS == 1)
	else if( _target->m_Type >= ITEMGET(12,440) && _target->m_Type <= ITEMGET(12,445) )
	{

	}
#endif
	else if( _target->m_Type >= ITEMGET(12,41) &&
			 _target->m_Type <= ITEMGET(12,43) )
	{

	}
	else if( _target->m_Type == ITEMGET(12,49) ||
			 _target->m_Type == ITEMGET(12,50) )
	{

	}
	else if( _target->m_Type >= ITEMGET(12,200) && _target->m_Type <= ITEMGET(12,214) )
	{

	}
	else if( _target->m_Type >= ITEMGET(12,262) &&
			 _target->m_Type <= ITEMGET(12,265) )
	{

	}
	else if(!(_target->m_Type < ITEMGET(12,7)
		|| _target->m_Type == ITEMGET(13,30) )
		|| _target->m_Type == ITEMGET(4,7)
		|| _target->m_Type == ITEMGET(4,15))
	{
		return false;
	}

	if(lpObj->pInventory[target].m_Level >= 9)
	{
		return false;
	}

	LogAddTD(lMsg.Get(558),
		lpObj->AccountID,lpObj->Name,lpObj->pInventory[target].GetName(),
		lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Level);

	int _r = rand()%100;

	if(lpObj->pInventory[target].m_Option2)
	{
		if(_r < 75)
		{
			lpObj->pInventory[target].m_Level++;
		}
		else
		{
			if(lpObj->pInventory[target].m_Level >= 7)
			{
				lpObj->pInventory[target].m_Level = 0;
			}
			else
			{
				lpObj->pInventory[target].m_Level--;

				if(lpObj->pInventory[target].m_Level < 0)
				{
					lpObj->pInventory[target].m_Level = 0;
				}
			}
		}
	}
	else
	{
		if(_r < 50)
		{
			lpObj->pInventory[target].m_Level++;
		}
		else
		{
			if(lpObj->pInventory[target].m_Level >= 7)
			{
				lpObj->pInventory[target].m_Level = 0;
			}
			else
			{
				lpObj->pInventory[target].m_Level--;

				if(lpObj->pInventory[target].m_Level < 0)
				{
					lpObj->pInventory[target].m_Level = 0;
				}
			}
		}
	}

	if(lpObj->pInventory[target].m_Level > 9)
	{
		lpObj->pInventory[target].m_Level = 9;
	}
	else
	{
		gObjMakePreviewCharSet(lpObj->m_Index);
	}

	float levelitemdur = (float)ItemGetDurability(lpObj->pInventory[target].m_Type,lpObj->pInventory[target].m_Level,lpObj->pInventory[target].IsExtItem(),lpObj->pInventory[target].IsSetItem());

	lpObj->pInventory[target].m_Durability = levelitemdur * lpObj->pInventory[target].m_Durability / lpObj->pInventory[target].m_BaseDurability;

	lpObj->pInventory[target].Convert(
			lpObj->pInventory[target].m_Type,
			lpObj->pInventory[target].m_Option1,
			lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,
			lpObj->pInventory[target].m_NewOption,
			lpObj->pInventory[target].m_SetOption,
			lpObj->pInventory[target].m_ItemOptionEx,0,-1,
			CURRENT_DB_VERSION);

	LogAddTD(lMsg.Get(559),lpObj->AccountID,lpObj->Name,lpObj->pInventory[source].m_Number,
			lpObj->pInventory[target].GetName(),lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Level);

	if(g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(&lpObj->pInventory[target])== 1)
	{
		if(g_kJewelOfHarmonySystem.IsActive(&lpObj->pInventory[target]) == 0)
		{
			GCServerMsgStringSend(lMsg.Get(3370),lpObj->m_Index,1);
		}
	}

	return true;
}






BOOL gObjItemRandomOption3Up(LPOBJ lpObj, int source, int target)
{
	if(source < 0 || source > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(target < 0 || target > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	CItem * _target = &lpObj->pInventory[target];


	if( _target->m_Type >= ITEMGET(12,36) &&
		_target->m_Type <= ITEMGET(12,40) )
	{

	}
#if (CUSTOM_WINGS == 1)
	else if( _target->m_Type >= ITEMGET(12,440) &&
			 _target->m_Type <= ITEMGET(12,445) )
	{

	}
#endif
	else if( _target->m_Type >= ITEMGET(12,41) &&
			 _target->m_Type <= ITEMGET(12,43) )
	{

	}
	else if( _target->m_Type == ITEMGET(12,49) ||
			 _target->m_Type == ITEMGET(12,50) )
	{

	}
	else if( _target->m_Type >= ITEMGET(12,262) && _target->m_Type <= ITEMGET(12,265) )
	{

	}
	else if(!(_target->m_Type < ITEMGET(12,7)
		|| _target->m_Type == ITEMGET(13,30) )
		|| _target->m_Type == ITEMGET(4,7)
		|| _target->m_Type == ITEMGET(4,15))
	{
		return false;
	}

	LogAddTD(lMsg.Get(576),
		lpObj->AccountID,lpObj->Name,
		lpObj->pInventory[target].GetName(),
		lpObj->pInventory[target].m_Number,
		lpObj->pInventory[target].m_Option3);

	int _r = rand()%100;

	if(lpObj->pInventory[target].m_Option3 == 0)
	{
		if(lpObj->pInventory[target].m_Type >= ITEMGET(12,3) && lpObj->pInventory[target].m_Type <= ITEMGET(12,6) ||
			lpObj->pInventory[target].m_Type == ITEMGET(12,49) ||
			lpObj->pInventory[target].m_Type == ITEMGET(12,42) )
		{
			lpObj->pInventory[target].m_NewOption &= 0xDF;

			if(rand()%2)
			{
				lpObj->pInventory[target].m_NewOption |= 0x20;
			}
		}
		
		if( lpObj->pInventory[target].m_Type >= ITEMGET(12,36) &&
			lpObj->pInventory[target].m_Type <= ITEMGET(12,40) ||
			lpObj->pInventory[target].m_Type == ITEMGET(12,43) ||
			lpObj->pInventory[target].m_Type == ITEMGET(12,50)
#if (CUSTOM_WINGS == 1)
			|| 
			lpObj->pInventory[target].m_Type >= ITEMGET(12,440) &&
			lpObj->pInventory[target].m_Type <= ITEMGET(12,445)
#endif
			)
		{
			lpObj->pInventory[target].m_NewOption &= 0xDF;

			if(rand()%3)
			{
				lpObj->pInventory[target].m_NewOption |= 0x20;
			}
			else if(rand()%3)
			{
				lpObj->pInventory[target].m_NewOption |= 0x10;
			}
		}

		if( lpObj->pInventory[target].m_Type >= ITEMGET(12,262) &&
			lpObj->pInventory[target].m_Type <= ITEMGET(12,265) )
		{
			lpObj->pInventory[target].m_NewOption &= 0xDF;

			if( rand()%2 )
			{
				lpObj->pInventory[target].m_NewOption |= 0x10;
			}
		}
	}

	if(lpObj->pInventory[target].m_Option3 < 7)
	{
		if(_r < 50)
		{
			lpObj->pInventory[target].m_Option3++;
		}
		else
		{
			lpObj->pInventory[target].m_Option3 = 0;
		}
	}
	else
	{
		return false;
	}

	gObjMakePreviewCharSet(lpObj->m_Index);

	float levelitemdur = (float)ItemGetDurability(lpObj->pInventory[target].m_Type,lpObj->pInventory[target].m_Level,lpObj->pInventory[target].IsExtItem(),lpObj->pInventory[target].IsSetItem());

	lpObj->pInventory[target].m_Durability = levelitemdur * lpObj->pInventory[target].m_Durability / lpObj->pInventory[target].m_BaseDurability;

	lpObj->pInventory[target].Convert(
			lpObj->pInventory[target].m_Type,
			lpObj->pInventory[target].m_Option1,
			lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,
			lpObj->pInventory[target].m_NewOption,
			lpObj->pInventory[target].m_SetOption,
			lpObj->pInventory[target].m_ItemOptionEx,0,-1,
			CURRENT_DB_VERSION);

	LogAddTD(lMsg.Get(577),lpObj->AccountID,lpObj->Name,lpObj->pInventory[source].m_Number,
			lpObj->pInventory[target].GetName(),lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Option3);

	unsigned char NewOption[8];

	ItemIsBufExOption(NewOption,&lpObj->pInventory[target]);

	LogAddTD("[%s][%s] JewelofLife Result [%d,%s,%d,%d,%d,%d]serial:[%d] dur:[%d] Ex:[%d,%d,%d,%d,%d,%d,%d]",
			lpObj->AccountID,lpObj->Name,target,lpObj->pInventory[target].GetName(),
			lpObj->pInventory[target].m_Level,lpObj->pInventory[target].m_Option1,lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,lpObj->pInventory[target].m_Number,
			BYTE(lpObj->pInventory[target].m_Durability),NewOption[0],NewOption[1],NewOption[2],NewOption[3],
			NewOption[4],NewOption[5],NewOption[6]);

	return true;
}

int gObjItemDurUp(LPOBJ lpObj, int source, int target)
{
	if(source < 0 || source > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(target < 0 || target > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	CItem * _target = &lpObj->pInventory[target];

	if( _target->m_bLuckySet )
	{

	}
	else
	{
		return false;
	}

	if( _target->m_Durability == 255 )
	{
		return false;
	}

	LogAddTD("[%s][%s] JewelOfExtension Before using jewel: (%s) serial:[%d] Dur:[%d]",
		lpObj->AccountID,lpObj->Name,_target->GetName(),_target->m_Number,(int)_target->m_Durability);

	_target->m_Durability = 255;
	_target->Convert(_target->m_Type,_target->m_Option1,_target->m_Option2,_target->m_Option3,_target->m_NewOption,_target->m_SetOption,_target->m_ItemOptionEx,0,-1,CURRENT_DB_VERSION);

	LogAddTD("[%s][%s] JewelOfExtension After using jewel: (%s) serial:[%d] Dur:[%d]",
		lpObj->AccountID,lpObj->Name,_target->GetName(),_target->m_Number,(int)_target->m_Durability);

return true;
}

int gObjItemStrenghtenUp(LPOBJ lpObj, int source, int target)
{
	if ( source < 0 || source > MAIN_INVENTORY_SIZE-1 )
		return FALSE;

	if ( target < 0 || target > MAIN_INVENTORY_SIZE-1 )
		return FALSE;

	if ( lpObj->pInventory[source].IsItem() == FALSE )
		return FALSE;

	if ( lpObj->pInventory[target].IsItem() == FALSE )
		return FALSE;

	CItem * pSource = &lpObj->pInventory[source];
	CItem * pTarget = &lpObj->pInventory[target];

	if ( g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(pTarget) == TRUE )
	{
		LogAddTD("[JewelOfElevation][Strengten Item] Already Strengtened [%s][%s]",
			lpObj->AccountID, lpObj->Name);
		return FALSE;
	}

	if (!pTarget->m_bLuckySet )
	{
		LogAddTD("[JewelOfElevation][Strengten Item] SetItem not enable to Strengtened [%s][%s]",
			lpObj->AccountID, lpObj->Name);
		return FALSE;
	}

	int iItemType = g_kJewelOfHarmonySystem._GetItemType(pTarget);

	if ( iItemType == JEWELOFHARMONY_ITEM_TYPE_NULL )
	{
		LogAddTD("[JewelOfElevation][Strengten Item] Strenghten Fail [%s][%s] Name[%s] Type[%d] Serial[%d] Invalid ItemType[%d]",
			lpObj->AccountID, lpObj->Name, pTarget->GetName(), pTarget->m_Type,
			pTarget->m_Number, iItemType);
		
		return FALSE;
	}

	int iItemOption = g_kJewelOfHarmonySystem._GetSelectRandomOption(pTarget, iItemType);

	if ( iItemOption == AT_JEWELOFHARMONY_NOT_STRENGTHEN_ITEM )
	{
		LogAddTD("[JewelOfElevation][Strengten Item] Strenghten Fail - NOT OPTION [%s][%s] Name[%s] Type[%d] Serial[%d] ItemType[%d]",
			lpObj->AccountID, lpObj->Name, pTarget->GetName(), pTarget->m_Type,
			pTarget->m_Number, iItemType);
		
		return FALSE;
	}

	int iItemOptionLevel = g_kJewelOfHarmonySystem.m_itemOption[iItemType][iItemOption].iRequireLevel;
	int iItemLevel = pTarget->m_Level;

	if( pTarget->m_Level >= MAX_JOH_ITEM_OPTION )
		iItemLevel = MAX_JOH_ITEM_OPTION-1;

	iItemOptionLevel += rand()%(1+(iItemLevel-iItemOptionLevel));

	int iSuccessRate = rand() % 100;

	if ( iSuccessRate >= g_kJewelOfHarmonySystem.m_iRateStrengthenSuccess )
	{
		LogAddTD("[JewelOfElevation][Strengten Item] Strenghten Fail [%s][%s] Name[%s] Type[%d] Serial[%d]  Rate (%d/%d)",
			lpObj->AccountID, lpObj->Name, pTarget->GetName(), pTarget->m_Type,
			pTarget->m_Number, iSuccessRate, g_kJewelOfHarmonySystem.m_iRateStrengthenSuccess);
		GCServerMsgStringSend(lMsg.Get(MSGGET(13, 45)), lpObj->m_Index, 1);
		return TRUE;
	}


	g_kJewelOfHarmonySystem._MakeOption(pTarget, iItemOption, iItemOptionLevel);

	LogAddTD("[JewelOfElevation][Strengten Item] Strenghten Success [%s][%s] Name[%s] Type[%d] Serial[%d] Rate (%d/%d) Option %d OptionLevel %d",
		lpObj->AccountID, lpObj->Name, pTarget->GetName(), pTarget->m_Type,
		pTarget->m_Number, iSuccessRate, g_kJewelOfHarmonySystem.m_iRateStrengthenSuccess,
		iItemOption, iItemOptionLevel);

	GCServerMsgStringSend(lMsg.Get(MSGGET(13, 46)), lpObj->m_Index, 1);

	gObjMakePreviewCharSet(lpObj->m_Index);

	float levelitemdur = (float)ItemGetDurability(lpObj->pInventory[target].m_Type,
		lpObj->pInventory[target].m_Level, lpObj->pInventory[target].IsExtItem(),
		lpObj->pInventory[target].IsSetItem());

	lpObj->pInventory[target].m_Durability = levelitemdur * lpObj->pInventory[target].m_Durability / lpObj->pInventory[target].m_BaseDurability;

	lpObj->pInventory[target].Convert(lpObj->pInventory[target].m_Type,
		lpObj->pInventory[target].m_Option1, lpObj->pInventory[target].m_Option2,
		lpObj->pInventory[target].m_Option3, lpObj->pInventory[target].m_NewOption,
		lpObj->pInventory[target].m_SetOption, lpObj->pInventory[target].m_ItemOptionEx,0,-1, 3);


	return TRUE;
}


#if (ENABLETEST_CUSTOM_JEWELS == 1)

BOOL gObjJewel1Enchant(LPOBJ lpObj, int source, int target)
{

#if (__NEKIT__ == 1)

	if(source < 0 || source > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(target < 0 || target > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	CItem * _target = &lpObj->pInventory[target];


	int max_exc = 4;

	if(_target->m_Type < ITEMGET(12,3) &&
		_target->m_Type != ITEMGET(4,7) &&
		_target->m_Type != ITEMGET(4,15) )
	{

	}
	else if((_target->m_Type >= ITEMGET(12,3) && _target->m_Type <= ITEMGET(12,6) ) ||
			_target->m_Type == ITEMGET(12,42) ||
			_target->m_Type == ITEMGET(13,30) )
	{
		max_exc = 4;
	}
	else if( _target->m_Type >= ITEMGET(12,36) && _target->m_Type <= ITEMGET(12,40) || _target->m_Type == ITEMGET(12,43) )
	{

	}
	else if( _target->m_Type == ITEMGET(12,49) || _target->m_Type == ITEMGET(12,50) )
	{

	}
#if (CUSTOM_WINGS == 1)
	else if( _target->m_Type >= ITEMGET(12,440) &&
			 _target->m_Type <= ITEMGET(12,445) )
	{

	}
#endif
	else
	{
		return false;
	}

	if( _target->m_Option2 != 0 )
	{
		return false;
	}

#if (__NEKIT__ == 1 )

	if( _target->GetExcOptCount() >= max_exc )
	{
		return false;
	}

#endif

	LogAddTD("[%s][%s] JewelOfLuck Before using jewel: (%s) serial:[%d]Op2:[%d]",
		lpObj->AccountID,lpObj->Name,lpObj->pInventory[target].GetName(),
		lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Option2);

#if (__NEKIT__ == 1)

	int rate = 30;

	if( _target->m_Level >= 0 && _target->m_Level < 11 )
	{
		rate = 50;
	}
	else if( _target->m_Level >= 11 && _target->m_Level < 15 )
	{
		rate = 40;
	}

	if( rand()%100 < rate )
	{
		_target->m_Option2 = TRUE;
	}

#endif

	gObjMakePreviewCharSet(lpObj->m_Index);

	lpObj->pInventory[target].Convert(
			lpObj->pInventory[target].m_Type,
			lpObj->pInventory[target].m_Option1,
			lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,
			lpObj->pInventory[target].m_NewOption,
			lpObj->pInventory[target].m_SetOption,
			lpObj->pInventory[target].m_ItemOptionEx,0,-1,
			CURRENT_DB_VERSION);

	LogAddTD("[%s][%s] JewelOfLuck After using jewel: (%s) serial:[%d]Op2:[%d]",
		lpObj->AccountID,lpObj->Name,lpObj->pInventory[target].GetName(),
		lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Option2);

	unsigned char NewOption[8];
	ItemIsBufExOption(NewOption,&lpObj->pInventory[target]);

	LogAddTD("[%s][%s] JewelofLuck Result [%d,%s,%d,%d,%d,%d]serial:[%d] dur:[%d] Ex:[%d,%d,%d,%d,%d,%d,%d]",
			lpObj->AccountID,lpObj->Name,target,lpObj->pInventory[target].GetName(),
			lpObj->pInventory[target].m_Level,lpObj->pInventory[target].m_Option1,lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,lpObj->pInventory[target].m_Number,
			BYTE(lpObj->pInventory[target].m_Durability),NewOption[0],NewOption[1],NewOption[2],NewOption[3],
			NewOption[4],NewOption[5],NewOption[6]);

	return true;
#endif
	return false;
}

BOOL gObjJewel2Enchant(LPOBJ lpObj, int source, int target)
{
#if (__NEKIT__ == 1)

	if(source < 0 || source > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(target < 0 || target > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	CItem * _target = &lpObj->pInventory[target];

	int bJewelry = FALSE;

	if(_target->m_Type < ITEMGET(12,0) &&
		_target->m_Type != ITEMGET(4,7) &&
		_target->m_Type != ITEMGET(4,15) )
	{

	}
	else if(_target->m_Type == ITEMGET(13,8) ||
			_target->m_Type == ITEMGET(13,9) ||
			_target->m_Type == ITEMGET(13,12) ||
			_target->m_Type == ITEMGET(13,13) ||
			_target->m_Type == ITEMGET(13,21) ||
			_target->m_Type == ITEMGET(13,22) ||
			_target->m_Type == ITEMGET(13,23) ||
			_target->m_Type == ITEMGET(13,24) ||
			_target->m_Type == ITEMGET(13,25) ||
			_target->m_Type == ITEMGET(13,26) ||
			_target->m_Type == ITEMGET(13,27) ||
			_target->m_Type == ITEMGET(13,28) )
	{
		bJewelry = TRUE;
	}
	else
	{
		return false;
	}

	if( _target->m_NewOption != 0 )
	{
		return false;
	}

	if( _target->m_SetOption )
	{
		if( bJewelry == TRUE )
		{

		}
		else
		{
			return false;
		}
	}

	LogAddTD("[%s][%s] JewelOfExcellent Before using jewel: (%s) serial:[%d]Op1:[%d]",
		lpObj->AccountID,lpObj->Name,lpObj->pInventory[target].GetName(),
		lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Option1);

	int rate = 10;

	if( _target->m_Level >= 0 && _target->m_Level < 9 )
	{
		rate = 40;
	}
	else if(_target->m_Level >= 9 && _target->m_Level < 11)
	{
		rate = 35;
	}
	else if(_target->m_Level >= 11 && _target->m_Level < 13 )
	{
		rate = 30;
	}
	else if(_target->m_Level >= 13 && _target->m_Level < 15 )
	{
		rate = 20;
	}

	if( rand()%100 < rate )
	{
		_target->m_NewOption |= 1<<rand()%6;
	}

	gObjMakePreviewCharSet(lpObj->m_Index);

	lpObj->pInventory[target].Convert(
			lpObj->pInventory[target].m_Type,
			lpObj->pInventory[target].m_Option1,
			lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,
			lpObj->pInventory[target].m_NewOption,
			lpObj->pInventory[target].m_SetOption,
			lpObj->pInventory[target].m_ItemOptionEx,0,-1,
			CURRENT_DB_VERSION);

	LogAddTD("[%s][%s] JewelOfExcellent After using jewel: (%s) serial:[%d]Op1:[%d]",
		lpObj->AccountID,lpObj->Name,lpObj->pInventory[target].GetName(),
		lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Option1);

	unsigned char NewOption[8];
	ItemIsBufExOption(NewOption,&lpObj->pInventory[target]);

	LogAddTD("[%s][%s] JewelofExcellent Result [%d,%s,%d,%d,%d,%d]serial:[%d] dur:[%d] Ex:[%d,%d,%d,%d,%d,%d,%d]",
			lpObj->AccountID,lpObj->Name,target,lpObj->pInventory[target].GetName(),
			lpObj->pInventory[target].m_Level,lpObj->pInventory[target].m_Option1,lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,lpObj->pInventory[target].m_Number,
			BYTE(lpObj->pInventory[target].m_Durability),NewOption[0],NewOption[1],NewOption[2],NewOption[3],
			NewOption[4],NewOption[5],NewOption[6]);

	return true;
#endif
	return false;
}

BOOL gObjJewel3Enchant(LPOBJ lpObj, int source, int target)
{
#if (__NEKIT__ == 1)

	int bWings = FALSE;

	if(source < 0 || source > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(target < 0 || target > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	CItem * _target = &lpObj->pInventory[target];

	if( _target->m_Type < ITEMGET(12,0) &&
		_target->m_Type != ITEMGET(4,7) &&
		_target->m_Type != ITEMGET(4,15) )
	{

	}
	else if( _target->m_Type >= ITEMGET(12,3) && _target->m_Type <= ITEMGET(12,6) )
	{
		bWings = TRUE;
	}
	else if( _target->m_Type >= ITEMGET(12,36) && _target->m_Type <= ITEMGET(12,40) )
	{
		bWings = TRUE;
	}
	else if( _target->m_Type == ITEMGET(12,42) || _target->m_Type == ITEMGET(12,43) )
	{
		bWings = TRUE;
	}
	else if( _target->m_Type == ITEMGET(12,49) || _target->m_Type == ITEMGET(12,50) )
	{
		bWings = TRUE;
	}
#if (CUSTOM_WINGS == 1)
	else if( _target->m_Type >= ITEMGET(12,440) && _target->m_Type <= ITEMGET(12,445) )
	{
		bWings = TRUE;
	}
#endif
	else if( _target->m_Type == ITEMGET(13,30) )
	{
		bWings = TRUE;
	}
	else if(_target->m_Type == ITEMGET(13,8) ||
			_target->m_Type == ITEMGET(13,9) ||
			_target->m_Type == ITEMGET(13,12) ||
			_target->m_Type == ITEMGET(13,13) ||
			_target->m_Type == ITEMGET(13,21) ||
			_target->m_Type == ITEMGET(13,22) ||
			_target->m_Type == ITEMGET(13,23) ||
			_target->m_Type == ITEMGET(13,24) ||
			_target->m_Type == ITEMGET(13,25) ||
			_target->m_Type == ITEMGET(13,26) ||
			_target->m_Type == ITEMGET(13,27) ||
			_target->m_Type == ITEMGET(13,28) )
	{

	}
	else
	{
		return false;
	}

	if( bWings == FALSE  )
	{
		int exc_count = _target->GetExcOptCount();

		if( exc_count <= 0 || exc_count >= 4 )
		{
			return false;
		}
	}
	else
	{
		int exc = _target->m_NewOption;
		int exc_count = 0;

		exc &= ~16;
		exc &= ~32;

		for(int i = 0; i < 4; i++)
		{
			int _t = 1 << i;
			if( (exc&_t) == _t )
				exc_count++;
		}

		if( exc_count <= 0 || exc_count >= 4 )
			return false;
	}

	LogAddTD("[%s][%s] Before using Jewel of Excellent : (%s)serial:(%d)(%d)",
		lpObj->AccountID,lpObj->Name,lpObj->pInventory[target].GetName(),
		lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Option2);

	int rate = 10;

	if( _target->m_Level >= 0 && _target->m_Level < 9 )
	{
		rate = 40;
	}
	else if(_target->m_Level >= 9 && _target->m_Level < 11)
	{
		rate = 35;
	}
	else if(_target->m_Level >= 11 && _target->m_Level < 13 )
	{
		rate = 30;
	}
	else if(_target->m_Level >= 13 && _target->m_Level < 15 )
	{
		rate = 20;
	}

	if( _target->m_Type >= ITEMGET(12,240) && _target->m_Type <= ITEMGET(12,245) )
	{
		rate = 20;
	}

	if(rand()%100 < rate )
	{
		int iExcCount = 1;
		int RandOpt = 6;

		if( bWings == TRUE )
			RandOpt = 4;

		while( iExcCount > 0 )
		{
			int iExcelPosition = rand()%RandOpt;

			BYTE btExcelValue = 1 << iExcelPosition;

			if((lpObj->pInventory[target].m_NewOption & btExcelValue) == FALSE)
			{
				lpObj->pInventory[target].m_NewOption |= btExcelValue;
				iExcCount --;
			}
		}
	}

	gObjMakePreviewCharSet(lpObj->m_Index);

	float levelitemdur = ItemGetDurability(lpObj->pInventory[target].m_Type,lpObj->pInventory[target].m_Level,lpObj->pInventory[target].IsExtItem(),lpObj->pInventory[target].IsSetItem());

	lpObj->pInventory[target].m_Durability = levelitemdur * lpObj->pInventory[target].m_Durability / lpObj->pInventory[target].m_BaseDurability;

	lpObj->pInventory[target].Convert(
			lpObj->pInventory[target].m_Type,
			lpObj->pInventory[target].m_Option1,
			lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,
			lpObj->pInventory[target].m_NewOption,
			lpObj->pInventory[target].m_SetOption,
			lpObj->pInventory[target].m_ItemOptionEx,0,-1,
			CURRENT_DB_VERSION);

	LogAddTD("[%s][%s]After using Jewel of Excellent (%d) : (%s) serial:(%d)(%d)",lpObj->AccountID,lpObj->Name,lpObj->pInventory[source].m_Number,
			lpObj->pInventory[target].GetName(),lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Option2);

	return true;

#endif

return false;
}

BOOL gObjJewel4Enchant(LPOBJ lpObj, int source, int target)
{
#if (__NEKIT__ == 1)
	if(source < 0 || source > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(target < 0 || target > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	CItem * _target = &lpObj->pInventory[target];

	if( !gSetItemOption.IsSetItem(_target->m_Type) )
	{
		return false;
	}
	else if( _target->m_SetOption )
	{
		return false;
	}

	int bJewelry = FALSE;

	if(_target->m_Type < ITEMGET(12,0) &&
		_target->m_Type != ITEMGET(4,7) &&
		_target->m_Type != ITEMGET(4,15) )
	{

	}
	else if(_target->m_Type == ITEMGET(13,8) ||
			_target->m_Type == ITEMGET(13,9) ||
			_target->m_Type == ITEMGET(13,12) ||
			_target->m_Type == ITEMGET(13,13) ||
			_target->m_Type == ITEMGET(13,21) ||
			_target->m_Type == ITEMGET(13,22) ||
			_target->m_Type == ITEMGET(13,23) ||
			_target->m_Type == ITEMGET(13,24) ||
			_target->m_Type == ITEMGET(13,25) ||
			_target->m_Type == ITEMGET(13,26) ||
			_target->m_Type == ITEMGET(13,27) ||
			_target->m_Type == ITEMGET(13,28) )
	{
		bJewelry = TRUE;
	}
	else
	{
		return false;
	}

	if( _target->m_NewOption )
	{
		if( bJewelry == TRUE )
		{

		}
		else
		{
			return false;
		}
	}


	LogAddTD("[%s][%s] JewelOfAncient Before using jewel: (%s) serial:[%d]Exc:[%d]",
		lpObj->AccountID,lpObj->Name,lpObj->pInventory[target].GetName(),
		lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_NewOption);

	int rate = 5;

	if( _target->m_Level >= 0 && _target->m_Level < 9 )
	{
		rate = 40;
	}
	else if(_target->m_Level >= 9 && _target->m_Level < 11)
	{
		rate = 30;
	}
	else if(_target->m_Level >= 11 && _target->m_Level < 13 )
	{
		rate = 20;
	}
	else if(_target->m_Level >= 13 && _target->m_Level < 15 )
	{
		rate = 10;
	}

	if( rand()%100 < rate )
	{
		_target->m_SetOption = gSetItemOption.GenSetOption(_target->m_Type);
	}

	gObjMakePreviewCharSet(lpObj->m_Index);

	lpObj->pInventory[target].Convert(
			lpObj->pInventory[target].m_Type,
			lpObj->pInventory[target].m_Option1,
			lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,
			lpObj->pInventory[target].m_NewOption,
			lpObj->pInventory[target].m_SetOption,
			lpObj->pInventory[target].m_ItemOptionEx,0,-1,
			CURRENT_DB_VERSION);

	LogAddTD("[%s][%s] JewelOfAncient After using jewel: (%s) serial:[%d]Op1:[%d]",
		lpObj->AccountID,lpObj->Name,lpObj->pInventory[target].GetName(),
		lpObj->pInventory[target].m_Number,lpObj->pInventory[target].m_Option1);

	unsigned char NewOption[8];
	ItemIsBufExOption(NewOption,&lpObj->pInventory[target]);

	LogAddTD("[%s][%s] JewelofAncient Result [%d,%s,%d,%d,%d,%d]serial:[%d] dur:[%d] Ex:[%d,%d,%d,%d,%d,%d,%d]",
			lpObj->AccountID,lpObj->Name,target,lpObj->pInventory[target].GetName(),
			lpObj->pInventory[target].m_Level,lpObj->pInventory[target].m_Option1,lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,lpObj->pInventory[target].m_Number,
			BYTE(lpObj->pInventory[target].m_Durability),NewOption[0],NewOption[1],NewOption[2],NewOption[3],
			NewOption[4],NewOption[5],NewOption[6]);

	return true;
#endif

return true;
}

BOOL gObjJewel5Enchant(LPOBJ lpObj, int source, int target)
{
#if (__NEKIT__ == 1)
	if(source < 0 || source > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(target < 0 || target > MAIN_INVENTORY_SIZE -1)
	{
		return false;
	}

	if(lpObj->pInventory[source].IsItem() == 0)
	{
		return false;
	}

	if(lpObj->pInventory[target].IsItem() == 0)
	{
		return false;
	}

	CItem * _target = &lpObj->pInventory[target];

	switch( _target->m_Type )
	{
	case ITEMGET(13,8):
	case ITEMGET(13,9):
	case ITEMGET(13,12):
	case ITEMGET(13,13):
	case ITEMGET(13,21):
	case ITEMGET(13,22):
	case ITEMGET(13,23):
	case ITEMGET(13,24):
	case ITEMGET(13,25):
	case ITEMGET(13,26):
	case ITEMGET(13,27):
	case ITEMGET(13,28):
		break;
	default:
		return false;
	}

	if(lpObj->pInventory[target].m_Level >= 15)
	{
		return false;
	}

	if( rand()%100 < Jewel_ofPend_Rate )
	{
		lpObj->pInventory[target].m_Level++;
	}
	else
	{
		if( lpObj->pInventory[target].m_Level > 7 )
			lpObj->pInventory[target].m_Level = 0;
		else if( lpObj->pInventory[target].m_Level > 0 )
			lpObj->pInventory[target].m_Level--;
	}

	gObjMakePreviewCharSet(lpObj->m_Index);

	float levelitemdur = ItemGetDurability(lpObj->pInventory[target].m_Type,lpObj->pInventory[target].m_Level,lpObj->pInventory[target].IsExtItem(),lpObj->pInventory[target].IsSetItem());

	lpObj->pInventory[target].m_Durability = levelitemdur * lpObj->pInventory[target].m_Durability / lpObj->pInventory[target].m_BaseDurability;

	lpObj->pInventory[target].Convert(
			lpObj->pInventory[target].m_Type,
			lpObj->pInventory[target].m_Option1,
			lpObj->pInventory[target].m_Option2,
			lpObj->pInventory[target].m_Option3,
			lpObj->pInventory[target].m_NewOption,
			lpObj->pInventory[target].m_SetOption,
			lpObj->pInventory[target].m_ItemOptionEx,0,-1,
			CURRENT_DB_VERSION);

	return true;
#endif

	return false;
}

#endif


void gObjAbilityReSet(LPOBJ lpObj)
{
	lpObj->LevelUpPoint = (lpObj->Level - 1) * 5;

	lpObj->Strength = DCInfo.DefClass[lpObj->Class].Strength;
	lpObj->Dexterity = DCInfo.DefClass[lpObj->Class].Dexterity;
	lpObj->Vitality = DCInfo.DefClass[lpObj->Class].Vitality;
	lpObj->Energy = DCInfo.DefClass[lpObj->Class].Energy;

	lpObj->MaxLife = DCInfo.DefClass[lpObj->Class].MaxLife;
	lpObj->MaxMana = DCInfo.DefClass[lpObj->Class].MaxMana;

	if(lpObj->Class == CLASS_WIZARD)
	{
		lpObj->MaxLife += (lpObj->Level -1)*0.5f;
		lpObj->MaxMana += 2.0f * (lpObj->Level -1);

		lpObj->Life = lpObj->MaxLife;
		lpObj->Mana = lpObj->MaxMana;
	}
	else
	{
		lpObj->MaxLife += 2.0f * (lpObj->Level - 1);
		lpObj->MaxMana += (lpObj->Level - 1)*0.5f;

		lpObj->Life = lpObj->MaxLife;
		lpObj->Mana = lpObj->MaxMana;
	}

	CloseClient(lpObj->m_Index);
}






BOOL gObjTargetGuildWarCheck(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	if(lpObj->Type != OBJ_USER)
	{
		return false;
	}

	if(lpTargetObj->Type != OBJ_USER)
	{
		return false;
	}

	if(lpObj->GuildNumber < 1 || lpTargetObj->GuildNumber < 1)
	{
		return false;
	}

	if(lpObj->lpGuild->WarState != 1 || lpTargetObj->lpGuild->WarState != 1)
	{
		return false;
	}

	if(strcmp(lpObj->lpGuild->TargetGuildName,lpTargetObj->lpGuild->Name))
	{
		return false;
	}
	return true;
}






void gObjGuildWarEndSend(LPOBJ lpObj, BYTE Result1, BYTE Result2)
{
	int n;

	if(lpObj == 0)
	{
		LogAdd("error-L3 : %s %d",__FILE__,__LINE__);
		return;
	}

	if(lpObj->lpGuild == 0)
	{
		LogAdd("error-L3 : %s %d",__FILE__,__LINE__);
		return;
	}

	if(lpObj->lpGuild->WarState != 1)
	{
		LogAdd("error-L3 : %s %d",__FILE__,__LINE__);
		return;
	}

	if(lpObj->lpGuild->lpTargetGuildNode == 0)
	{
		LogAdd("error-L3 : %s %d",__FILE__,__LINE__);
		return;
	}

	if(Result1 == 1)
	{
		gObjGuildWarItemGive(lpObj->lpGuild,lpObj->lpGuild->lpTargetGuildNode);
	}
	else if(Result1 == 3)
	{
		gObjGuildWarItemGive(lpObj->lpGuild->lpTargetGuildNode,lpObj->lpGuild);
	}
	else if(Result1 == 1)
	{
		gObjGuildWarItemGive(lpObj->lpGuild->lpTargetGuildNode,lpObj->lpGuild);
	}

	for(n = 0; n < MAX_USER_GUILD;n++)
	{
		if(lpObj->lpGuild->Use[n] && lpObj->lpGuild->Index[n] >= 0)
		{
			gObj[lpObj->lpGuild->Index[n]].IsInBattleGround = 0;
			GCGuildWarEnd(lpObj->lpGuild->Index[n],Result1,lpObj->lpGuild->lpTargetGuildNode->Name);
		}
	}

	for(n = 0; n < MAX_USER_GUILD;n++)
	{
		if(lpObj->lpGuild->lpTargetGuildNode->Use[n] && lpObj->lpGuild->lpTargetGuildNode->Index[n] >= 0)
		{
			gObj[lpObj->lpGuild->lpTargetGuildNode->Index[n]].IsInBattleGround = 0;
			GCGuildWarEnd(lpObj->lpGuild->lpTargetGuildNode->Index[n],Result2,lpObj->lpGuild->Name);
		}
	}
}

void gObjGuildWarEndSend(_GUILD_INFO_STRUCT * lpGuild1, _GUILD_INFO_STRUCT * lpGuild2, BYTE Result1, BYTE Result2)
{
	int n;

	if(lpGuild1 == 0 || lpGuild2 == 0)
	{
		return;
	}

	if(lpGuild1->WarState != 1 || lpGuild2->WarState != 1)
	{
		return;
	}

	for(n = 0; n < MAX_USER_GUILD;n++)
	{
		if(lpGuild1->Use[n] && lpGuild1->Index[n] >= 0)
		{
			gObj[lpGuild1->Index[n]].IsInBattleGround = 0;
			GCGuildWarEnd(lpGuild1->Index[n],Result1,lpGuild2->Name);
		}
	}

	for(n = 0; n < MAX_USER_GUILD;n++)
	{
		if(lpGuild2->Use[n] && lpGuild2->Index[n] >= 0)
		{
			gObj[lpGuild2->Index[n]].IsInBattleGround = 0;
			GCGuildWarEnd(lpGuild2->Index[n],Result2,lpGuild1->Name);
		}
	}
}






void gObjGuildWarEnd(_GUILD_INFO_STRUCT * lpGuild, _GUILD_INFO_STRUCT * lpTargetGuild)
{
	if(lpGuild == 0 || lpTargetGuild == 0)
	{
		return;
	}

	lpGuild->WarDeclareState = 0;
	lpGuild->WarState = 0;

	if(lpTargetGuild != 0)
	{
		lpTargetGuild->WarDeclareState = 0;
		lpTargetGuild->WarState = 0;

		if(lpGuild->lpTargetGuildNode != 0)
		{
			_GUILD_INFO_STRUCT * lpTguild = lpGuild->lpTargetGuildNode;

			lpGuild->lpTargetGuildNode = 0;
			lpTguild->lpTargetGuildNode = 0;
		}
	}
	else
	{
		lpGuild->lpTargetGuildNode = 0;
	}
}






BOOL gObjGuildWarProc(_GUILD_INFO_STRUCT * lpGuild1, _GUILD_INFO_STRUCT * lpGuild2, int score)
{
	int totalscore;
	int n;
	int maxscore;
	char szTemp[0x100];

	if(lpGuild1 == 0 || lpGuild2 == 0)
	{
		return false;
	}

	totalscore = 1;
	maxscore = 20;

	lpGuild1->PlayScore += score;

	wsprintf(szTemp,"%s ( %d ) VS %s ( %d )",lpGuild1->Name,lpGuild1->PlayScore,lpGuild2->Name,lpGuild2->PlayScore);
	LogAddTD(szTemp);

	if(lpGuild1->WarType == 1)
	{
		SetBattleTeamScore(lpGuild1->BattleGroundIndex,lpGuild1->BattleTeamCode,lpGuild1->PlayScore);
		maxscore = 100;
	}
	else
	{
		maxscore = 20;
	}

//	cManager.BattleInfoSend(GetBattleTeamName(0,0),GetBattleTeamScore(0,0),GetBattleTeamName(0,1),GetBattleTeamScore(0,1));

	if(lpGuild1->PlayScore >= maxscore)
	{
		if(lpGuild1->PlayScore > maxscore && lpGuild2->PlayScore == 0)
		{
			totalscore = 3;
		}
		else if(lpGuild1->PlayScore > maxscore && lpGuild2->PlayScore <= 10)
		{
			totalscore = 2;
		}

		lpGuild1->TotalScore += totalscore;
		DGGuildScoreUpdate(lpGuild1->Name,lpGuild1->TotalScore);
		DGGuildScoreUpdate(lpGuild2->Name,lpGuild2->TotalScore);
		return true;
	}

	for(n = 0; n < MAX_USER_GUILD; n++)
	{
		if(lpGuild1->Use[n] && lpGuild1->Index[n] >= 0)
		{
			GCGuildWarScore(lpGuild1->Index[n]);
		}
	}

	for(n = 0; n < MAX_USER_GUILD; n++)
	{
		if(lpGuild2->Use[n] && lpGuild2->Index[n] >= 0)
		{
			GCGuildWarScore(lpGuild2->Index[n]);
		}
	}
	return false;
}






BOOL gObjGuildWarCheck(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	int score = 1;

	if(gObjTargetGuildWarCheck(lpObj,lpTargetObj)==0)
	{
		return false;
	}

	if(strcmp(lpTargetObj->Name,lpTargetObj->lpGuild->Names[0])==0)
	{
		score = 2;
	}

	_GUILD_INFO_STRUCT * lpGuild = lpObj->lpGuild;
	_GUILD_INFO_STRUCT * lpTargetGuild = lpTargetObj->lpGuild;

	if(gObjGuildWarProc(lpGuild,lpTargetGuild,score)==1)
	{
		if(lpGuild->WarType == 1)
		{
			gObjGuildWarEndSend(lpGuild,lpTargetGuild,1,0);
			gBattleGroundEnable(lpGuild->BattleGroundIndex,0);
			gObjGuildWarEnd(lpGuild,lpTargetGuild);
//			cManager.BattleInfoSend(GetBattleTeamName(0,0),255,GetBattleTeamName(0,1),255);
		}
		else
		{
			gObjGuildWarEndSend(lpObj,1,0);
			gObjAddMsgSendDelay(lpObj,4,lpObj->m_Index,2000,0);
		}
	}
	return true;
}






BOOL gObjGuildWarMasterClose(LPOBJ lpObj)
{
	if(lpObj->GuildNumber < 1)
	{
		return false;
	}

	if(strcmp(lpObj->lpGuild->Names[0],lpObj->Name))
	{
		return false;
	}

	gObjGuildWarEndSend(lpObj,3,2);

	if(lpObj->lpGuild->WarType == 1 && lpObj->lpGuild->BattleGroundIndex < 1)
	{
		gBattleGroundEnable(lpObj->lpGuild->BattleGroundIndex,0);
//		cManager.BattleInfoSend(GetBattleTeamName(0,0),255,GetBattleTeamName(0,1),255);
	}
	gObjGuildWarEnd(lpObj->lpGuild,lpObj->lpGuild->lpTargetGuildNode);
	return true;
}






int gObjGuildWarItemGive(_GUILD_INFO_STRUCT * lpWinGuild, _GUILD_INFO_STRUCT * lpLoseGuild)
{
	return true;
	int n;
	int r_userindex[MAX_USER_GUILD];
	int r_usercount;

	for(n = 0; n < MAX_USER_GUILD; n++)
	{
		if(lpLoseGuild->Use[n] > 0)
		{
			r_userindex[r_usercount] = lpLoseGuild->Index[n];
			r_usercount++;
		}
	}

	if(r_usercount < 1)
	{
		LogAdd(lMsg.Get(560),__FILE__,__LINE__);
		return false;
	}

	int lose_user = r_userindex[rand()%r_usercount];

	if(lose_user < 0)
	{
		LogAdd(lMsg.Get(560),__FILE__,__LINE__);
		return false;
	}

	if(gObj[lose_user].Connected < PLAYER_PLAYING)
	{
		LogAdd(lMsg.Get(560),__FILE__,__LINE__);
		return false;
	}

	r_usercount = 0;

	for(n = 0; n < MAX_USER_GUILD;n++)
	{
		if(lpWinGuild->Use[n] > 0)
		{
			r_userindex[r_usercount] = lpWinGuild->Index[n];
			r_usercount++;
		}
	}

	if(r_usercount < 1)
	{
		LogAdd(lMsg.Get(561));
		return false;
	}

	int win_user = r_userindex[rand()%r_usercount];

	if(win_user < 0)
	{
		LogAdd(lMsg.Get(561));
		return false;
	}

	if(gObj[win_user].Connected < PLAYER_PLAYING)
	{
		LogAdd(lMsg.Get(561));
		return false;
	}

	LPOBJ lpObj = &gObj[lose_user];

	int count = 24;

	int number;

	while(count--)
	{
		number = rand()%12;

		if(lpObj->pInventory[number].IsItem()==1)
		{
			if(gObjInventoryInsertItem(win_user,lpObj->pInventory[number]) != 0xFF)
			{
				return true;
			}
			break;
		}
	}

	count = 64;

	while(count--)
	{
		number = rand()%64+12;

		if(lpObj->pInventory[number].IsItem()==1)
		{
			if(gObjInventoryInsertItem(win_user,lpObj->pInventory[number]) != 0xFF)
			{
				return true;
			}
			break;
		}
	}
	return true;
}






int gObjGetPkTime(LPOBJ lpObj, int& hour, int& min)
{
	int pktime = 0;

	if(lpObj->m_PK_Time > 0)
	{
		pktime = lpObj->m_PK_Time / 60;
	}

	if(lpObj->m_PK_Level == 4)
	{
		hour = (180 - pktime) / 60;
		min = (180 - pktime) % 60;
		return true;
	}

	if(lpObj->m_PK_Level == 5)
	{
		hour = (360 - pktime) / 60;
		min = (360 - pktime) % 60;
		return true;
	}

	if(lpObj->m_PK_Level >= 6)
	{
		hour = (((lpObj->m_PK_Count - 3) * 60 + 540) - pktime) / 60;
		min = (((lpObj->m_PK_Count - 3) * 60 + 540) - pktime) % 60;
		return true;
	}

	return false;
}






BOOL gObjMonsterCall(int aIndex, int MonsterType, int x, int y)
{

	if(gObj[aIndex].MapNumber == MAP_INDEX_ICARUS)
	{
		return false;
	}

	if(gObj[aIndex].m_RecallMon >= 0)
	{

		GCRecallMonLife(aIndex,60,0);
		gObjMonsterCallKill(aIndex);
		return false;
	}

	int result = gObjAddCallMon();

	if(result >= 0)
	{
		gObj[result].X = x;
		gObj[result].Y = y;

		gObj[result].MTX = x;
		gObj[result].MTY = y;
		gObj[result].Dir = 2;

		gObj[result].MapNumber = gObj[aIndex].MapNumber;

		gObjSetMonster(result,MonsterType);

		gObj[result].m_RecallMon = aIndex;
		gObj[result].m_Attribute = 100;

		gObj[result].TargetNumber = -1;
		gObj[result].m_ActState.Emotion = 0;
		gObj[result].m_ActState.Attack = 0;
		gObj[result].m_ActState.EmotionCount = 0;

		gObj[result].PathCount = 0;

		gObj[aIndex].m_RecallMon = result;
		gObj[result].m_MoveRange = 15;

		GCRecallMonLife(gObj[result].m_RecallMon,(int)gObj[result].MaxLife,(int)gObj[result].Life);
		LogAddTD("[Summon Monster] [%s][%s] Try to Summon Monster - Succeed (SummonIndex:%d)",gObj[aIndex].AccountID,gObj[aIndex].Name,result);
		return true;
	}
	return false;
}






void gObjMonsterCallKill(int aIndex)
{

	LogAddTD("[Summon Monster] [%s][%s] Try to Kill Summoned Monster",gObj[aIndex].AccountID,gObj[aIndex].Name);

	if(gObj[aIndex].m_RecallMon < 0)
	{
		return;
	}

	int callmon = gObj[aIndex].m_RecallMon;

	if(callmon < 0 || callmon > OBJMAX -1 )
	{
		return;
	}

	if(gObj[callmon].Type != OBJ_MONSTER)
	{
		return;
	}

	if(gObj[callmon].m_RecallMon != aIndex)
	{
		return;
	}

	if(gObj[aIndex].m_RecallMon >= 0)
	{
		if(gObjIsConnected(gObj[aIndex].m_RecallMon)==1)
		{
			LogAddTD("[Summon Monster] [%s][%s] Try to Kill Summoned Monster - Found Summoned Monster (SummonIndex:%d)",gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[aIndex].m_RecallMon);
			gObj[gObj[aIndex].m_RecallMon].m_RecallMon = -1;
		}
	}

	gObj[aIndex].m_RecallMon = -1;
	gObjDel(callmon);

#if (CUSTOM_PETS==1)
	gCustomPets.SetSummon(aIndex,FALSE);
#endif

	LogAddTD("[Summon Monster] [%s][%s] Try to Kill Summoned Monster - Finished to Kill Summoned Monster (SummonIndex:%d)",gObj[aIndex].AccountID,gObj[aIndex].Name,callmon);
}






BOOL gObjCheckTeleportArea(int aIndex, BYTE x, BYTE y)
{

	if(x < (gObj[aIndex].X - 8)
		|| x > (gObj[aIndex].X + 8)
		|| y < (gObj[aIndex].Y - 8)
		|| y > (gObj[aIndex].Y + 8))
	{
		return false;
	}

	int mapnumber = gObj[aIndex].MapNumber;

	if(mapnumber < 0 || mapnumber >= MAX_NUMBER_MAP)
	{
		return false;
	}

	unsigned char attr = MapC[mapnumber].GetAttr(x,y);

	if(attr)
	{
		return false;
	}

	attr = MapC[mapnumber].GetAttr(gObj[aIndex].X,gObj[aIndex].Y);

	if(attr & 1)
	{
		return false;
	}

	return true;
}






BOOL gObjCheckAttackAreaUsedPath(int aIndex, int TarObjIndex)
{
	int X,Y;
	if(gObj[aIndex].PathCount > 1)
	{
		 X = gObj[aIndex].PathX[gObj[aIndex].PathCount-1];
		 Y = gObj[aIndex].PathY[gObj[aIndex].PathCount-1];
	}
	else
	{
		return false;
	}

	if(gObj[TarObjIndex].X < (X - 10)
		|| gObj[TarObjIndex].X > (X + 10)
		|| gObj[TarObjIndex].Y < (Y - 10)
		|| gObj[TarObjIndex].Y > (Y + 10))
	{
		return false;
	}
	return true;
}






BOOL gObjCheckattackAreaUsedViewPort(int aIndex, int TarObjIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[TarObjIndex];

	if(lpTargetObj->m_Index == lpObj->m_Index)
	{
		return true;
	}

	int MVL = MAX_VIEWPORT;

	if(lpObj->Type == OBJ_MONSTER)
	{
		MVL = MAX_VIEWPORT_MONSTER;
	}

	for(int n = 0; n < MVL;n++)
	{
		if(lpObj->VpPlayer[n].state)
		{
			if(lpObj->VpPlayer[n].number == lpTargetObj->m_Index)
			{
				return true;
			}
		}

		if(lpObj->VpPlayer2[n].state)
		{
			if(lpObj->VpPlayer2[n].number == lpTargetObj->m_Index)
			{
				return true;
			}
		}
	}
	return false;
}






int  gObjCheckAttackArea(int aIndex, int TarObjIndex)
{

	if(gObjCheckattackAreaUsedViewPort(aIndex,TarObjIndex)==0)
	{
		return 1;
	}

	int mapnumber = gObj[aIndex].MapNumber;

	if(mapnumber < 0 || mapnumber >= MAX_NUMBER_MAP)
	{
		return 2;
	}

	int tarmapnumber = gObj[TarObjIndex].MapNumber;

	if(tarmapnumber < 0 || tarmapnumber >= MAX_NUMBER_MAP)
	{
		return 3;
	}

	if(tarmapnumber != mapnumber)
	{
		return 4;
	}

	unsigned char attr;

#if (GS_CASTLE == 1)
	attr = 0;
#else
	attr = MapC[mapnumber].GetAttr(gObj[TarObjIndex].X,gObj[TarObjIndex].Y);

	if(attr && !(attr&2))
	{
		return 5;
	}

#endif

	attr = MapC[mapnumber].GetAttr(gObj[aIndex].X,gObj[aIndex].Y);

	if(attr && !(attr&2))
	{
		return 6;
	}

	return false;
}






BOOL gUserFindDevilSquareInvitation(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	for(int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if(lpObj->pInventory[n].IsItem() == 1)
		{
			if(lpObj->pInventory[n].m_Type == ITEMGET(14,19))
			{
				return true;
			}

			if(lpObj->pInventory[n].m_Type == ITEMGET(13,46))
			{
				return true;
			}
		}
	}
	return false;
}






BOOL gUserFindDevilSquareKeyEyes(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int bFindKey = 0;
	int bFindEyes = 0;

	for(int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if(lpObj->pInventory[n].IsItem() == 1)
		{
			if(lpObj->pInventory[n].m_Type == ITEMGET(14,17))
			{
				bFindEyes = 1;
			}

			if(lpObj->pInventory[n].m_Type == ITEMGET(14,18))
			{
				bFindKey = 1;
			}
		}
	}

	if(bFindKey != 0 && bFindEyes != 0)
	{
		return true;
	}
	return false;
}






void gObjSendUserStatistic(int aIndex, int startLevel, int endLevel)
{
	if(gDisconnect == 1)
	{
		return;
	}

	if(endLevel < startLevel)
	{
		endLevel = MAX_CHAR_LEVEL;
	}

	int n;
	LPOBJ lpObj;
	int sClassCount[5];

	sClassCount[CLASS_WIZARD] = 0;
	sClassCount[CLASS_KNIGHT] = 0;
	sClassCount[CLASS_ELF] = 0;
	sClassCount[CLASS_MAGUMSA] = 0;
	sClassCount[4] = 0;

	for(n = OBJ_STARTUSERINDEX; n < OBJMAX; n++)
	{
		lpObj = &gObj[n];

		if(lpObj->Connected > PLAYER_LOGGED)
		{
			if(lpObj->Level >= startLevel && lpObj->Level <= endLevel)
			{
				sClassCount[lpObj->Class]++;
			}

			sClassCount[4]++;
		}
	}

	MsgOutput(gObj[aIndex].m_Index,"[%d]-[%d] Total %d %s %d %s %d %s %d %s %d",startLevel,endLevel,sClassCount[4],
		lMsg.Get(1900),sClassCount[CLASS_WIZARD],
		lMsg.Get(1901),sClassCount[CLASS_KNIGHT],
		lMsg.Get(1902),sClassCount[CLASS_ELF],
		lMsg.Get(1903),sClassCount[CLASS_MAGUMSA]);
}






LPOBJ gObjFind(char * targetcharname)
{
	if ( gDisconnect == TRUE )
	{
		return NULL;
	}

	int n;
	LPOBJ lpObj;

	for ( n = OBJ_STARTUSERINDEX ; n<OBJMAX;n++)
	{
		lpObj = &gObj[n];

		if ( lpObj->Connected > PLAYER_LOGGED )
		{
			if ( lpObj->Name[0] == targetcharname[0] )
			{
				if ( strcmp(lpObj->Name, targetcharname) == 0 )
				{
					return lpObj;
				}
			}
		}
	}

	return NULL;

}






BOOL gObjFind10EventChip(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;

	for(int n = 0; n < MAIN_INVENTORY_SIZE;n++)
	{
		if(lpObj->pInventory[n].IsItem() == 1)
		{
			if(lpObj->pInventory[n].m_Type == ITEMGET(14,21))
			{
				count++;

				if(count >= 10)
				{
					return true;
				}
			}
		}
	}
	return false;
}






BOOL gObjDelete10EventChip(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;

	for(int n = 0; n < MAIN_INVENTORY_SIZE;n++)
	{
		if(lpObj->pInventory[n].IsItem() == 1)
		{
			if(lpObj->pInventory[n].m_Type == ITEMGET(14,21))
			{
				gObjInventoryDeleteItem(aIndex,n);
				GCInventoryItemDeleteSend(aIndex,n,1);
				LogAddTD("[EventChip] [%s][%s] Delete Rena (%d)",lpObj->AccountID,lpObj->Name,n);
				count++;

				if(count >= 10)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void gObjSetPosition(int aIndex, int x, int y)
{

	LPOBJ lpObj = &gObj[aIndex];
	PMSG_POSISTION_SET pMove;

	// KOR PROTOCOL
	pMove.h.c = 0xC1;
#if (__ENG__ == 1)
	pMove.h.headcode = 0x15;
#else
	pMove.h.headcode = 0x10;
#endif
	pMove.h.size = sizeof(pMove);

	pMove.X = x;
	pMove.Y = y;

	lpObj->m_Rest = 0;


	MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX,lpObj->m_OldY);
	MapC[lpObj->MapNumber].SetStandAttr(x,y);

	lpObj->m_OldX = x;
	lpObj->m_OldY = y;

	RecvPositionSetProc(&pMove,lpObj->m_Index);
}






int  gObjGetItemCountInEquipment(int aIndex, int itemtype, int itemindex, int itemlevel)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;

	for ( int n =0;n<MAX_PLAYER_EQUIPMENT ; n++ )
	{
		if ( lpObj->pInventory[n].IsItem() == TRUE )
		{
			if ( lpObj->pInventory[n].m_Type == ((itemtype * MAX_SUBTYPE_ITEMS) + itemindex) )
			{
				if ( itemlevel != -1 )
				{
					if (  lpObj->pInventory[n].m_Level != itemlevel )
					{
						continue;
					}
				}
				
				count++;
			}
		}
	}

	return count;
}







int  gObjGetItemCountInIventory(int aIndex, int itemtype, int itemindex, int itemlevel)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;

	for ( int n =0;n<MAIN_INVENTORY_SIZE ; n++ )
	{
		if ( lpObj->pInventory[n].IsItem() == TRUE )
		{
			if ( lpObj->pInventory[n].m_Type == ((itemtype * MAX_SUBTYPE_ITEMS) + itemindex) )
			{
				if ( itemlevel != -1 )
				{
					if (  lpObj->pInventory[n].m_Level != itemlevel )
					{
						continue;
					}
				}
				
				count++;
			}
		}
	}

	return count;
}






int gObjGetItemCountInIventory(int aIndex, int itemnum)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;

	for ( int n=0;n<MAIN_INVENTORY_SIZE;n++)
	{
		if ( lpObj->pInventory[n].IsItem() == TRUE )
		{
			if ( lpObj->pInventory[n].m_Type == itemnum )
			{
				count++;
			}
		}
	}

	return count;

}






int  gObjGetManaItemPos(int aIndex)	// [Failure : -1]
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	
	for ( int n=0;n<MAIN_INVENTORY_SIZE;n++)
	{
		if ( lpObj->pInventory[n].IsItem() == TRUE )
		{
			if ( lpObj->pInventory[n].m_Type == ITEMGET(14,4) || lpObj->pInventory[n].m_Type == ITEMGET(14,5) || lpObj->pInventory[n].m_Type == ITEMGET(14,6) )
			{
				return n;
			}
		}
	}

	return -1;
}






void gObjDelteItemCountInInventory(int aIndex, int itemtype, int itemindex,int itemlevel, int count)
{
	LPOBJ lpObj = &gObj[aIndex];
	int delcount = 0;

	for(int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if(lpObj->pInventory[n].IsItem() == 1)
		{
			if(lpObj->pInventory[n].m_Type == ITEMGET(itemtype,itemindex) )
			{
				if(itemlevel == -1 || lpObj->pInventory[n].m_Level == itemlevel)
				{
					gObjInventoryDeleteItem(aIndex,n);
					GCInventoryItemDeleteSend(aIndex,n,1);
					LogAddTD("[DeleteItem] [%s][%s] Delete (%d,%d)(%d)",lpObj->AccountID,lpObj->Name,itemtype,itemindex,n);
					delcount++;

					if(delcount >= count)
					{
						return;
					}
				}
			}
		}
	}
}


void gObjDelteItemCountInInventory(int aIndex, int itemtype, int itemindex, int count)
{
	LPOBJ lpObj = &gObj[aIndex];
	int delcount = 0;

	for(int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if(lpObj->pInventory[n].IsItem() == 1)
		{
			if(lpObj->pInventory[n].m_Type == ITEMGET(itemtype,itemindex) )
			{
				gObjInventoryDeleteItem(aIndex,n);
				GCInventoryItemDeleteSend(aIndex,n,1);
				LogAddTD("[DeleteItem] [%s][%s] Delete (%d,%d)(%d)",lpObj->AccountID,lpObj->Name,itemtype,itemindex,n);
				delcount++;

				if(delcount >= count)
				{
					return;
				}
			}
		}
	}
}




void gObjGetStatPointState(int aIndex, short& AddPoint, short& MaxAddPoint, short& MinusPoint, short& MaxMinusPoint)
{
	if(gObj[aIndex].Level <= 5)
	{
		AddPoint = 0;
		MaxAddPoint = 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	int total_point = lpObj->Strength + lpObj->Dexterity + lpObj->Vitality + lpObj->Energy + lpObj->Leadership + lpObj->LevelUpPoint;
	int ori_point;

	if(lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER)
	{
		ori_point = DCInfo.GetDefPoint(lpObj->Class) + (lpObj->Level - 1) * 7;
	}
	else
	{
		ori_point = DCInfo.GetDefPoint(lpObj->Class) + (lpObj->Level - 1) * 5;
	}

	if(g_QuestInfo.GetQuestState(lpObj,0) == 2)
	{
		ori_point+= 10;
	}

	if(g_QuestInfo.GetQuestState(lpObj,1) == 2)
	{
		ori_point+= 10;
	}

	if(g_QuestInfo.GetQuestState(lpObj,2) == 2)
	{
		ori_point+= lpObj->Level - 220;
	}

	if(g_QuestInfo.GetQuestState(lpObj,4) == 2)
	{
		ori_point+= 20;
	}

	if(g_QuestInfo.GetQuestState(lpObj,5) == 2)
	{
		ori_point+= 20;
	}

	if(g_QuestInfo.GetQuestState(lpObj,6) == 2)
	{
		ori_point+= 30;
	}

	int addpoint = ori_point;

	addpoint += gStatMng.GetMaxStat(lpObj->Level,lpObj->Class);

	AddPoint = total_point - ori_point;
	MaxAddPoint = gStatMng.GetMaxStat(lpObj->Level,lpObj->Class);
	MinusPoint = lpObj->iFruitPoint;
	MaxMinusPoint = gStatMng.GetMaxMinusStat(lpObj->Level,lpObj->Class);
}






BOOL gObjCheckStatPointUp(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int total_point = lpObj->Strength + lpObj->Dexterity + lpObj->Vitality + lpObj->Energy + lpObj->Leadership + lpObj->LevelUpPoint;
	int ori_point;

	if(lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER)
	{
		ori_point = DCInfo.GetDefPoint(lpObj->Class) + (lpObj->Level - 1) * 7;
	}
	else
	{
		ori_point = DCInfo.GetDefPoint(lpObj->Class) + (lpObj->Level - 1) * 5;
	}

	if(g_QuestInfo.GetQuestState(lpObj,0) == 2)
	{
		ori_point+= 10;
	}

	if(g_QuestInfo.GetQuestState(lpObj,1) == 2)
	{
		ori_point+= 10;
	}

	if(g_QuestInfo.GetQuestState(lpObj,2) == 2)
	{
		ori_point+= lpObj->Level - 220;
	}

	if(g_QuestInfo.GetQuestState(lpObj,4) == 2)
	{
		ori_point+= 20;
	}

	if(g_QuestInfo.GetQuestState(lpObj,5) == 2)
	{
		ori_point+= 20;
	}

	if(g_QuestInfo.GetQuestState(lpObj,6) == 2)
	{
		ori_point+= 30;
	}

	ori_point += gStatMng.GetMaxStat(lpObj->Level,lpObj->Class);

	if(total_point < ori_point)
	{
		return true;
	}
	return false;
}






BOOL gObjCheckStatPointDown(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int total_point = lpObj->Strength + lpObj->Dexterity + lpObj->Vitality + lpObj->Energy + lpObj->LevelUpPoint;
	int ori_point;

	if(lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA)
	{
		ori_point = DCInfo.GetDefPoint(lpObj->Class) + (lpObj->Level - 1) * 7;
	}
	else
	{
		ori_point = DCInfo.GetDefPoint(lpObj->Class) + (lpObj->Level - 1) * 5;
	}

	if(g_QuestInfo.GetQuestState(lpObj,0) == 2)
	{
		ori_point+= 10;
	}

	if(g_QuestInfo.GetQuestState(lpObj,1) == 2)
	{
		ori_point+= 10;
	}

	ori_point += gStatMng.GetMaxStat(lpObj->Level,lpObj->Class);

	if(total_point > ori_point)
	{
		return true;
	}
	return false;
}






void gObjUseCircle(int aIndex, int pos)
{
	int level = gObj[aIndex].pInventory[pos].m_Level;
	int iSerial = gObj[aIndex].pInventory[pos].m_Number;

	PMSG_DEFRESULT pResult;

	PHeadSetB((LPBYTE)&pResult,0x2C,sizeof(pResult));

	pResult.result = 0;

	if(gObj[aIndex].Level <= 10 || level > 3)
	{
		pResult.result |= 0xC0;
		DataSend(aIndex,(unsigned char*)&pResult,pResult.h.size);
		return;
	}

	if(gObjCheckStatPointUp(aIndex)==0)
	{
		pResult.result |= 0xC0;
		DataSend(aIndex,(unsigned char*)&pResult,pResult.h.size);
		return;
	}

	LogAddTD("[StatUp] [%s][%s] Use StatUpItem Level:%d Pos:%d serial:%d",gObj[aIndex].AccountID,gObj[aIndex].Name,level,pos,iSerial);

	gObjInventoryItemSet(aIndex,pos,0xFF);
	gObj[aIndex].pInventory[pos].Clear();
	GCInventoryItemDeleteSend(aIndex,pos,1);

	short AddPoint;
	short MaxAddPoint;
	short MinusPoint;
	short MaxMinusPoint;

	gObjGetStatPointState(aIndex,(short &)AddPoint,(short &)(MaxAddPoint),(short &)MinusPoint,(short &)MaxMinusPoint);

	int iSuccessRate = 0;

	if(AddPoint <= 10)
	{
		iSuccessRate = 100;
	}
	else if((AddPoint - 10) < MaxAddPoint * (double)0.1)
	{
		iSuccessRate = 90;
	}
	else if((AddPoint - 10) < MaxAddPoint * (double)0.3)
	{
		iSuccessRate = 80;
	}
	else if((AddPoint - 10) < MaxAddPoint * (double)0.5)
	{
		iSuccessRate = 70;
	}
	else if((AddPoint - 10) < MaxAddPoint * (double)0.8)
	{
		iSuccessRate = 60;
	}
	else
	{
		iSuccessRate = 50;
	}

	int incvalue = 0;

	if(rand()%100 < iSuccessRate)
	{
		iSuccessRate = rand()%100;

		int AddStat = 0;

		if(iSuccessRate < 70)
		{
			AddStat = 1;
		}
		else if(iSuccessRate < 95)
		{
			AddStat = 2;
		}
		else
		{
			AddStat = 3;
		}

		if((AddStat + AddPoint) > MaxAddPoint)
		{
			AddStat = 1;
		}

		pResult.result |= level * 16;
		pResult.result |= AddStat;

		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);

		incvalue = AddStat;

		LogAddTD("[StatUp] [%s][%s] Success [%d][%d] %d",gObj[aIndex].AccountID,gObj[aIndex].Name,level,incvalue,iSerial);
	}
	else
	{
		pResult.result |= 0x40;
		pResult.result |= level * 16;
		pResult.result = pResult.result;

		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		LogAddTD("[StatUp] [%s][%s] Fail [%d] %d",gObj[aIndex].AccountID,gObj[aIndex].Name,level,iSerial);
		return;
	}

	switch(level)
	{
	case 3:
		gObj[aIndex].Strength += incvalue;
		break;
	case 2:
		gObj[aIndex].Dexterity += incvalue;
		break;
	case 1:
		gObj[aIndex].Vitality += incvalue;
		gObj[aIndex].MaxLife += gObj[aIndex].VitalityToLife * incvalue;
		GCReFillSend(gObj[aIndex].m_Index,(WORD)(gObj[aIndex].MaxLife + gObj[aIndex].AddLife),0xFE,0,gObj[aIndex].iMaxShield+gObj[aIndex].iAddShield);
		break;
	case 0:
		gObj[aIndex].Energy += incvalue;
		gObj[aIndex].MaxMana += gObj[aIndex].EnergyToMana * incvalue;
		gObjSetBP(aIndex);
		GCManaSend(gObj[aIndex].m_Index,(WORD)(gObj[aIndex].MaxMana + gObj[aIndex].AddMana),0xFE,0,gObj[aIndex].MaxBP);
		break;
	default: break;
	}

	gObjCalCharacter(aIndex);
}

struct PMSG_USE_STAT_FRUIT {

  // static data ------------------------------------

  // non-static data --------------------------------
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x3>*/ /*|0x1|*/ unsigned char result;
  /*<thisrel this+0x4>*/ /*|0x1|*/ unsigned short wStatValue;
  /*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btFruitType;

  // base classes -----------------------------------

  // friends ----------------------------------------

  // static functions -------------------------------

  // non-virtual functions --------------------------

  // virtual functions ------------------------------
};

void gObjUsePlusStatFruit(int aIndex,int pos)
{
	if(!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if(gObj[aIndex].Class >= MAX_TYPE_PLAYER || gObj[aIndex].Class < 0)
	{
		return;
	}

	int iItemLevel = gObj[aIndex].pInventory[pos].m_Level;
	int iItemSerial = gObj[aIndex].pInventory[pos].m_Number;
	int iCharacterClass = gObj[aIndex].Class;

	PMSG_USE_STAT_FRUIT pResult;

	PHeadSetB((LPBYTE)&pResult,0x2C,sizeof(pResult));

	pResult.result = 2;
	pResult.btFruitType = iItemLevel;
	pResult.wStatValue = 0;

	if(gObj[aIndex].Level <= 9 || iItemLevel > 4)
	{
		pResult.result = 2;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		return;
	}

	if(iItemLevel == 4 && gObj[aIndex].Class != 4)
	{
		pResult.result = 2;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		return;
	}

	if( gObj[aIndex].pInventory[ELEMENT_SOCKET].IsItem() )
	{
		pResult.result = 16;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		return;
	}

	for(int iItemIndex = 0; iItemIndex < INVETORY_WEAR_SIZE; iItemIndex++)
	{
		if(gObj[aIndex].pInventory[iItemIndex].IsItem())
		{
			pResult.result = 16;
			DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
			return;
		}
	}

	if(gObjCheckStatPointUp(aIndex) == false)
	{
		pResult.result = 33;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		return;
	}

	LogAddTD("[StatUp] [%s][%s] Use StatUpItem Level:%d Pos:%d serial:%d",
		gObj[aIndex].AccountID,gObj[aIndex].Name,iItemLevel,pos,iItemSerial);

	gObjInventoryItemSet(aIndex,pos,0xFF);
	gObj[aIndex].pInventory[pos].Clear();
	GCInventoryItemDeleteSend(aIndex,pos,1);

	short AddPoint;
	short MaxAddPoint;
	short MinusPoint;
	short MaxMinusPoint;

	gObjGetStatPointState(aIndex,(short &)AddPoint,(short &)MaxAddPoint,(short &)MinusPoint,(short &)MaxMinusPoint);

	int iSuccessRate = 0;

	if(AddPoint <= 10)
	{
		iSuccessRate = 100;
	}
	else if((AddPoint - 10) < (MaxAddPoint * (double)0.1))
	{
		iSuccessRate = 90;
	}
	else if((AddPoint - 10) < (MaxAddPoint * (double)0.3))
	{
		iSuccessRate = 80;
	}
	else if((AddPoint - 10) < (MaxAddPoint * (double)0.5))
	{
		iSuccessRate = 70;
	}
	else if((AddPoint - 10) < (MaxAddPoint * (double)0.8))
	{
		iSuccessRate = 60;
	}
	else
	{
		iSuccessRate = 50;
	}

	int iIncStat = 0;
	int bSuccess = 0;

	if(rand()%100 < iSuccessRate)
	{
		bSuccess = 1;
	}

	if(bSuccess != 0)
	{
		iSuccessRate = rand()%100;

		int AddStat = 0;

		if(iSuccessRate < 70)
		{
			AddStat = 1;
		}
		else if(iSuccessRate < 95)
		{
			AddStat = 2;
		}
		else
		{
			AddStat = 3;
		}

		if((AddStat + AddPoint) > MaxAddPoint)
		{
			AddStat = 1;
		}

		pResult.result = 0;
		pResult.wStatValue = AddStat;
		pResult.btFruitType = iItemLevel;

		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		iIncStat = AddStat;

		LogAddTD("[StatUp] [%s][%s] Success [%d][%d] %d",
			gObj[aIndex].AccountID,gObj[aIndex].Name,iItemLevel,iIncStat,iItemSerial);
	}
	else
	{
		pResult.result = 1;
		pResult.wStatValue = 0;
		pResult.btFruitType = iItemLevel;

		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);

		LogAddTD("[StatUp] [%s][%s] Fail [%d] %d",
			gObj[aIndex].AccountID,gObj[aIndex].Name,iItemLevel,iItemSerial);
		return;
	}

	switch(iItemLevel)
	{
	case 4: gObj[aIndex].Leadership += iIncStat;	break;
	case 3: gObj[aIndex].Strength += iIncStat;	break;
	case 2: gObj[aIndex].Dexterity += iIncStat;	break;
	case 1:
		gObj[aIndex].Vitality += iIncStat;
		gObj[aIndex].MaxLife += gObj[aIndex].VitalityToLife * iIncStat;
		break;
	case 0:
		gObj[aIndex].Energy += iIncStat;
		gObj[aIndex].MaxMana += gObj[aIndex].EnergyToMana * iIncStat;
		gObjSetBP(aIndex);
		break;
	default : break;
	}

	gObjCalCharacter(aIndex);
	GCReFillSend(gObj[aIndex].m_Index,(WORD)(gObj[aIndex].MaxLife + gObj[aIndex].AddLife),0xFE,0,gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield);
	gObjSetBP(aIndex);
	GCManaSend(gObj[aIndex].m_Index,(WORD)(gObj[aIndex].MaxMana + gObj[aIndex].AddMana),0xFE,0,gObj[aIndex].MaxBP + gObj[aIndex].AddBP);
}



void gObjUseMinusStatFruit(int aIndex, int pos)
{
	if(!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if(gObj[aIndex].Class >= MAX_TYPE_PLAYER || gObj[aIndex].Class < 0)
	{
		return;
	}

	int iItemLevel = gObj[aIndex].pInventory[pos].m_Level;
	int iItemSerial = gObj[aIndex].pInventory[pos].m_Number;
	int iCharacterClass = gObj[aIndex].Class;

	PMSG_USE_STAT_FRUIT pResult;

	PHeadSetB((LPBYTE)&pResult,0x2C,sizeof(pResult));

	pResult.result = 5;
	pResult.btFruitType = iItemLevel;
	pResult.wStatValue = 0;

	if(gObj[aIndex].Level <= 9 || iItemLevel > 4)
	{
		pResult.result = 5;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		return;
	}

	if(iItemLevel == 4 && gObj[aIndex].Class != 4)
	{
		pResult.result = 5;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		return;
	}

	if( gObj[aIndex].pInventory[ELEMENT_SOCKET].IsItem() )
	{
		pResult.result = 16;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		return;
	}

	for(int iItemIndex = 0; iItemIndex < INVETORY_WEAR_SIZE; iItemIndex++)
	{
		if(gObj[aIndex].pInventory[iItemIndex].IsItem())
		{
			pResult.result = 16;
			DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
			return;
		}
	}

	int iDefaultStat = 0;
	int iPresentStat = 0;
	int bEnableUseFruit = 1;

	if(iItemLevel == 0)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Energy;
		iPresentStat = gObj[aIndex].Energy;

		if(iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}
	else if(iItemLevel == 1)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Vitality;
		iPresentStat = gObj[aIndex].Vitality;

		if(iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}
	else if(iItemLevel == 2)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Dexterity;
		iPresentStat = gObj[aIndex].Dexterity;

		if(iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}
	else if(iItemLevel == 3)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Strength;
		iPresentStat = gObj[aIndex].Strength;

		if(iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}
	else if(iItemLevel == 4)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Leadership;
		iPresentStat = gObj[aIndex].Leadership;

		if(iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}

	if(bEnableUseFruit == 0)
	{
		pResult.result = 38;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		return;
	}

	short AddPoint;
	short MaxAddPoint;
	short MinusPoint;
	short MaxMinusPoint = 0;

	gObjGetStatPointState(aIndex,(short &)AddPoint,(short &)MaxAddPoint,(short &)MinusPoint,(short &)MaxMinusPoint);

	if(MinusPoint >= MaxMinusPoint || MinusPoint < 0)
	{
		pResult.result = 37;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		return;
	}

	LogAddTD("[StatDown] [%s][%s] Use StatDownItem Level:%d Pos:%d serial:%d",
		gObj[aIndex].AccountID,gObj[aIndex].Name,iItemLevel,pos,iItemSerial);

	gObjInventoryItemSet(aIndex,pos,0xFF);
	gObj[aIndex].pInventory[pos].Clear();
	GCInventoryItemDeleteSend(aIndex,pos,1);

	int iDecStat = 0;
	int bSuccess = 0;

	int iSuccessRate = 100 - gObj[aIndex].Level / 6;

	if(iSuccessRate < 0)
	{
		iSuccessRate = 0;
	}

	if(rand()%100 < iSuccessRate)
	{
		bSuccess = 1;
	}

	if(MinusPoint <= 10)
	{
		bSuccess = 1;
	}

	if(bSuccess != 0)
	{
		iSuccessRate = rand()%100;

		if(iSuccessRate < 50)
		{
			iDecStat = 1;
		}
		else if(iSuccessRate < 75)
		{
			iDecStat = 3;
		}
		else if(iSuccessRate < 91)
		{
			iDecStat = 5;
		}
		else if(iSuccessRate < 98)
		{
			iDecStat = 7;
		}
		else
		{
			iDecStat = 9;
		}

		if((MinusPoint + iDecStat) >= MaxMinusPoint)
		{
			iDecStat = MaxMinusPoint - MinusPoint;
		}

		if((iPresentStat - iDecStat) < iDefaultStat)
		{
			iDecStat = iPresentStat - iDefaultStat;
		}

		pResult.result = 3;
		pResult.btFruitType = iItemLevel;
		pResult.wStatValue = iDecStat;

		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		LogAddTD("[StatDown] [%s][%s] Success [%d][%d] %d",
			gObj[aIndex].AccountID,gObj[aIndex].Name,iItemLevel,iDecStat,iItemSerial);
	}
	else
	{
		pResult.result = 4;
		pResult.btFruitType = iItemLevel;
		pResult.wStatValue = 0;
		DataSend(aIndex,(unsigned char *)&pResult,pResult.h.size);
		LogAddTD("[StatDown] [%s][%s] Fail [%d] %d",
			gObj[aIndex].AccountID,gObj[aIndex].Name,iItemLevel,iItemSerial);
		return;
	}

	switch(iItemLevel)
	{
	case 4:	gObj[aIndex].Leadership -= iDecStat;	break;
	case 3: gObj[aIndex].Strength -= iDecStat;	break;
	case 2: gObj[aIndex].Dexterity -= iDecStat;	break;
	case 1:

		gObj[aIndex].Vitality -= iDecStat;
		gObj[aIndex].MaxLife -= gObj[aIndex].VitalityToLife *iDecStat;
		break;

	case 0:
		gObj[aIndex].Energy -= iDecStat;
		gObj[aIndex].MaxMana -= gObj[aIndex].EnergyToMana * iDecStat;
		gObjSetBP(aIndex);
		break;

	default:
		break;
	}

	int iOldLevelUpPoint = gObj[aIndex].LevelUpPoint;
	gObj[aIndex].LevelUpPoint += iDecStat;

	int iOldFruitPoint = gObj[aIndex].LevelUpPoint;
	gObj[aIndex].iFruitPoint += iDecStat;

	LogAddTD("[StatDown] [%s][%s] MinusStat[%d] -> LevelUpPoint Old(%d)/New(%d)  FruitPoint Old(%d)/New(%d)",
		gObj[aIndex].AccountID,gObj[aIndex].Name,iDecStat,iOldLevelUpPoint,gObj[aIndex].LevelUpPoint,iOldFruitPoint,gObj[aIndex].iFruitPoint);

	gObjCalCharacter(aIndex);
	GCReFillSend(gObj[aIndex].m_Index,(WORD)(gObj[aIndex].MaxLife + gObj[aIndex].AddLife),0xFE,0,gObj[aIndex].iMaxShield+gObj[aIndex].iAddShield);
	gObjSetBP(aIndex);
	GCManaSend(gObj[aIndex].m_Index,(WORD)(gObj[aIndex].MaxMana + gObj[aIndex].AddMana),0xFE,0,gObj[aIndex].MaxBP);
}

void gObjDelayLifeCheck(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->m_CheckLifeTime > 0)
	{
		lpObj->m_CheckLifeTime--;

		if(lpObj->m_CheckLifeTime <= 0)
		{
			lpObj->m_CheckLifeTime = 0;

			if(lpObj->Life < 0)
			{
				lpObj->Life = 0;
			}

			if(lpObj->lpAttackObj != 0)
			{
				gObjLifeCheck(lpObj,lpObj->lpAttackObj,0,1,0,0,0,0);
			}
		}
	}
}






BOOL gObjDuelCheck(LPOBJ lpObj)
{
	BOOL bRetVal = FALSE;

	if ( lpObj->Type == OBJ_USER )
	{
		int iDuelIndex = lpObj->m_iDuelUser;
		if (  OBJMAX_RANGE(iDuelIndex) )
		{
			bRetVal = TRUE;
		}
	}

	return bRetVal;

	/*BOOL bRetVal = FALSE;
	
	if ( lpObj->Type == OBJ_USER )
	{
		if (  g_DuelManager.DuelCheck(lpObj) )
		{
			bRetVal = TRUE;
		}
	}
	
	return bRetVal;*/
}






BOOL gObjDuelCheck(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	BOOL bRetVal = FALSE;

	if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER)
	{
		if ( lpObj->m_iDuelUser == lpTargetObj->m_Index && lpTargetObj->m_iDuelUser == lpObj->m_Index)
		{
			bRetVal = TRUE;
		}
	}

	return bRetVal;

	/*BOOL bRetVal = FALSE;
	
	if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER)
	{
		if ( g_DuelManager.DuelCheck(lpObj, lpTargetObj))
		{
			bRetVal = TRUE;
		}
	}
	
	return bRetVal;*/

}





void gObjResetDuel(LPOBJ lpObj)
{
	return;

	int iDuelIndex = lpObj->m_iDuelUser;

	if ( OBJMAX_RANGE(iDuelIndex))
	{
		lpObj->m_iDuelUser = -1;
		lpObj->m_iDuelUserReserved = -1;
		lpObj->m_btDuelScore = 0;
		lpObj->m_iDuelTickCount = 0;
		gDarkSpirit[lpObj->m_Index].ReSetTarget(iDuelIndex);
		gDarkSpirit[iDuelIndex].ReSetTarget(lpObj->m_Index);
		gObj[iDuelIndex].m_iDuelUser = -1;
		gObj[iDuelIndex].m_iDuelUserReserved = -1;
		gObj[iDuelIndex].m_iDuelUserRequested = -1;
		gObj[iDuelIndex].m_iDuelTickCount = 0;
	}
}






void gObjSendDuelEnd(LPOBJ lpObj)
{
	return;

	PMSG_ANS_END_DUEL pMsg;
	if(gObjIsConnected(lpObj->m_Index)==0)
	{
		return;
	}

	if(lpObj->Type == OBJ_MONSTER)
	{
		return;
	}

	if(lpObj->CloseCount >= 0)
	{
		return;
	}

	lpObj->m_iDuelUser = -1;
	lpObj->m_iDuelUserReserved = -1;
	lpObj->m_iDuelUserRequested = -1;
	lpObj->m_btDuelScore = 0;
	lpObj->m_iDuelTickCount = 0;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xAA;
	pMsg.h.subcode = 0x03;
	pMsg.h.size = sizeof(pMsg);
	pMsg.bResult = FALSE;

	pMsg.NumberH = SET_NUMBERH(lpObj->m_iDuelUser);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_iDuelUser);

	memcpy(pMsg.szName,gObj[lpObj->m_iDuelUser].Name,10);
	DataSend(lpObj->m_Index,(unsigned char *)&pMsg,pMsg.h.size);

	/*if( lpObj->m_iDuelRoom != -1 )
	{
		gDuelSystem.EndRoom(lpObj->m_iDuelRoom);
	}*/
}

void gObjCheckAllUserDuelStop()
{
	for(int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++)
	{
		if(gObj[i].Connected == PLAYER_PLAYING)
		{
			if(gObj[i].Type == OBJ_USER)
			{
				if(OBJMAX_RANGE(gObj[i].m_iDuelUser))
				{
#if (CUSTOM_OLYMP == 1)
					if(GetTickCount() - gObj[i].m_iDuelTickCount > (60000*5))
#else
					if(GetTickCount() - gObj[i].m_iDuelTickCount > 60000)
#endif
					{
#if (CUSTOM_OLYMP == 1)

						int Type = 0;
						int iWinner = i;
						int iLooser = gObj[i].m_iDuelUser;

						if( gObj[iWinner].m_btDuelScore < gObj[iLooser].m_btDuelScore )	{
							iWinner = iLooser;
							iLooser = i;
						}
						else if( gObj[iWinner].m_btDuelScore == gObj[iLooser].m_btDuelScore )	{
							Type = 2;
						}


						g_CustomOlymp.EndFight(iWinner,iLooser,Type);

						if( Type != 2 && gObjIsConnected(iWinner) && gObjIsConnected(iLooser) )
						{
							/*PMSG_ANS_END_DUEL pMsg;
							PHeadSubSetB((LPBYTE)pMsg,0xAA,0x03,sizeof(pMsg));
							
							pMsg.bResult = 0;
							pMsg.NumberH = SET_NUMBERH(iWinner);
							pMsg.NumberL = SET_NUMBERL(iWinner);
							memcpy(pMsg.szName,gObj[iWinner].Name,10);
							DataSend(iLooser,(unsigned char *)&pMsg,pMsg.h.size);
							pMsg.NumberH = SET_NUMBERH(iLooser);
							pMsg.NumberL = SET_NUMBERL(iLooser);
							memcpy(pMsg.szName,gObj[iLooser].Name,10);
							DataSend(iWinner,(unsigned char *)&pMsg,pMsg.h.size);*/

							//g_DuelManager.SendEndDuel(&gObj[iLooser]);
							gObj[iLooser].m_iDuelUser = -1;
							
							//g_DuelManager.SendEndDuel(&gObj[iWinner]);
							gObj[iWinner].m_iDuelUser = -1;

							if( gObj[i].m_iDuelRoom != -1 )
							{
								//g_DuelManager.SendEndDuelNotification(&gObj[iWinner],gObj[iWinner].Name,gObj[iLooser].Name);
								//g_DuelManager.SendEndDuelNotification(&gObj[iLooser],gObj[iWinner].Name,gObj[iLooser].Name);
								//gDuelSystem.SendEndNotify(gObj[i].m_iDuelRoom,iWinner,iLooser);
							}

							gDarkSpirit[iWinner].ReSetTarget(iLooser);
							gDarkSpirit[iLooser].ReSetTarget(iWinner);

							char szMsg[256];
							wsprintf(szMsg,lMsg.Get(1216),gObj[iLooser].Name);
							GCServerMsgStringSend(szMsg,iWinner,1);
							wsprintf(szMsg,lMsg.Get(1217),gObj[iWinner].Name);
							GCServerMsgStringSend(szMsg,iLooser,1);

							PMSG_SERVERCMD ServerCmd;

							PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
							ServerCmd.CmdType = 0;
							ServerCmd.X = gObj[iWinner].X;
							ServerCmd.Y = gObj[iWinner].Y;

							MsgSendV2(&gObj[iWinner],(unsigned char *)&ServerCmd,sizeof(ServerCmd));
							DataSend(iWinner,(unsigned char *)&ServerCmd,sizeof(ServerCmd));

							LogAdd("[Duel] [TimeExpired] [%s][%s] Win Duel, Loser [%s][%s]",gObj[iWinner].AccountID,gObj[iWinner].Name,gObj[iLooser].AccountID,gObj[iLooser].Name);
						}
						else
						{
							gObjSendDuelEnd(&gObj[gObj[i].m_iDuelUser]);
							gObjSendDuelEnd(&gObj[i]);
							gObjResetDuel(&gObj[i]);
						}
#else
						gObjSendDuelEnd(&gObj[gObj[i].m_iDuelUser]);
						gObjSendDuelEnd(&gObj[i]);
						gObjResetDuel(&gObj[i]);
#endif
					}
				}
			}
		}
	}
}






void gObjNProtectGGCheckSum(int aIndex)
{
	/*if(gUseNPGGChecksum==0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->m_InWebzen)
	{
		return;
	}

	if(lpObj->m_bSentGGAuth == 0)
	{
		if((GetTickCount() - lpObj->m_NPggCheckSumSendTime) > 300000)
		{
			lpObj->m_NPggCheckSumSendTime = GetTickCount();

			unsigned long dwGGErrCode = lpObj->NPggCSAuth.GetAuthQuery();

			if(dwGGErrCode != 0)
			{
				LogAddTD("[NPgg] Failed Send Checksum %x [%s][%s] errorcode: %d",dwGGErrCode,lpObj->AccountID,lpObj->Name,dwGGErrCode);
				lpObj->m_bSentGGAuth = 0;
				gObjCloseSet(aIndex,0);
			}
			else
			{
				LogAddTD("[NPgg] Send Checksum %x [%s][%s]",dwGGErrCode,lpObj->AccountID,lpObj->Name);
				GCNPggSendCheckSum(aIndex,&lpObj->NPggCSAuth.m_AuthQuery);
				lpObj->m_bSentGGAuth = 1;
			}
		}
	}
	else
	{
		if((GetTickCount() - lpObj->m_NPggCheckSumSendTime) > 30000)
		{
			LogAddTD("[NPgg] Checksum Time Out [%s][%s]",lpObj->AccountID,lpObj->Name);

			PMSG_NOTICE pNotice;

			TNotice::SetNoticeProperty(&pNotice,10,_ARGB(0xFF,0xFE,0x51,0x51),1,0,20);

			TNotice::MakeNoticeMsg(&pNotice,10,lMsg.Get(1201));
			TNotice::SendNoticeToUser(aIndex,&pNotice);

			TNotice::MakeNoticeMsg(&pNotice,10,lMsg.Get(1202));
			TNotice::SendNoticeToUser(aIndex,&pNotice);

			TNotice::MakeNoticeMsg(&pNotice,10,lMsg.Get(1203));
			TNotice::SendNoticeToUser(aIndex,&pNotice);

			gObjCloseSet(aIndex,0);
		}
	}
	*/
}








void SkillFrustrum(BYTE bangle, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	float p[MAX_ARRAY_FRUSTRUM][3];

	p[0][0] = -6;
	p[0][1] = 6;
	p[0][2] = 0;
	p[1][0] = 6;
	p[1][1] = 6;
	p[1][2] = 0;
	p[2][0] = 1;
	p[2][1] = 0;
	p[2][2] = 0;
	p[3][0] = -1;
	p[3][1] = 0;
	p[3][2] = 0;

	float Angle[0x3];

	Angle[0] = 0;
	Angle[1] = 0;
	Angle[2] = (float)(bangle * 360 / 255);

	float Matrix[3][MAX_ARRAY_FRUSTRUM];

	AngleMatrix(Angle,Matrix);

	float vFrustrum[MAX_ARRAY_FRUSTRUM][3];

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM; i++)
	{
		VectorRotate((const float *)&p[i],(const float *)&Matrix[0],(float *)&vFrustrum[i]);

		lpObj->fSkillFrustrumX[i] = (int)vFrustrum[i][0] + lpObj->X;
		lpObj->fSkillFrustrumY[i] = (int)vFrustrum[i][1] + lpObj->Y;
	}
}






BOOL SkillTestFrustrum(int x, int y,int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int j = 3;

	for(int i = 0; i < MAX_ARRAY_FRUSTRUM; j = i,i++)
	{
		if((lpObj->fSkillFrustrumX[i]- x) * (lpObj->fSkillFrustrumY[j]-y) - (lpObj->fSkillFrustrumX[j]-x) * (lpObj->fSkillFrustrumY[i]-y) < 0.0f)
		{
			return false;
		}
	}
	return true;
}





BOOL gObjCheckMaxZen(int aIndex, int nAddZen)
{
	if ( !OBJMAX_RANGE(aIndex) )
		return FALSE;

	LPOBJ lpObj = &gObj[aIndex];
	
	__int64 _Money = 0;
	_Money = (__int64)lpObj->Money + (__int64)nAddZen;

	if ( _Money > (__int64)MAX_ZEN )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 226)), aIndex, 1);
		return FALSE;
	}

	return TRUE;
}






void MakeRandomSetItem(int aIndex)
{
	int itemnum = gSetItemOption.GenRandomItemNum();

	int SetOption = gSetItemOption.GenSetOption(itemnum);

	int option1rand;
	int option2rand;
	int option3rand;
	int optionc;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	option1rand = 6;
	option2rand = 4;

	option3rand = rand()%100;
	optionc = rand()%3;

	if(rand()%100 < option2rand)
	{
		Option2 = 1;
	}

	switch(optionc)
	{
	case 0: 
		if(option3rand < 4)
		{
			Option3 = 3;
		}
		break;
	case 1:
		if(option3rand < 8)
		{
			Option3 = 2;
		}
		break;
	case 2: 
		if(option3rand < 12)
		{
			Option3 = 1;
		}
		break;
	default: break;
	}

	Option1 = 1;

	ItemSerialCreateSend(aIndex,gObj[aIndex].MapNumber,gObj[aIndex].X,gObj[aIndex].Y,itemnum,0,0,Option1,Option2,Option3,aIndex,0,SetOption);
	LogAddTD("[Chaos Castle] [%s][%s] Winner Set Item itemnum:[%d] skill:[%d] luck:[%d] option:[%d] SetOption:[%d]",gObj[aIndex].AccountID,gObj[aIndex].Name,
		itemnum,Option1,Option2,Option3,SetOption);
}






void MakeRewardSetItem(int aIndex, BYTE cDropX, BYTE cDropY, int iRewardType, int iMapnumber)
{
	int itemnum = gSetItemOption.GenRandomItemNum();

	int SetOption = gSetItemOption.GenSetOption(itemnum);

	int option1rand;
	int option2rand;
	int option3rand;
	int optionc;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	option1rand = 6;
	option2rand = 4;

	option3rand = rand()%100;
	optionc = rand()%3;

	if(rand()%100 < option2rand)
	{
		Option2 = 1;
	}

	switch(optionc)
	{
	case 0: 
		if(option3rand < 4)
		{
			Option3 = 3;
		}
		break;
	case 1:
		if(option3rand < 8)
		{
			Option3 = 2;
		}
		break;
	case 2: 
		if(option3rand < 12)
		{
			Option3 = 1;
		}
		break;
	default: break;
	}

	Option1 = 1;

	if(cDropX == 0 && cDropY == 0)
	{
		cDropX = gObj[aIndex].X;
		cDropY = gObj[aIndex].Y;
	}

	ItemSerialCreateSend(aIndex,iMapnumber,cDropX,cDropY,itemnum,0,0,Option1,Option2,Option3,aIndex,0,SetOption);

	if(iRewardType == 1)
	{
		LogAddTD("[★☆Reward][KUNDUN] [%s][%s] Set Item itemnum:[%d] skill:[%d] luck:[%d] option:[%d] SetOption:[%d]",
			gObj[aIndex].AccountID,gObj[aIndex].Name,itemnum,Option1,Option2,Option3,SetOption);
	}
	else
	{
		LogAddTD("[Reward][Etc] [%s][%s] Set Item itemnum:[%d] skill:[%d] luck:[%d] option:[%d] SetOption:[%d]",
			gObj[aIndex].AccountID,gObj[aIndex].Name,itemnum,Option1,Option2,Option3,SetOption);
	}		
}






void gObjRecall(int aIndex, int mapnumber, int x, int y)
{
	if(gObj[aIndex].MapNumber != mapnumber)
	{
		gObj[aIndex].X = x;
		gObj[aIndex].Y = y;

		gObj[aIndex].TX = x;
		gObj[aIndex].TY = y;

		gObj[aIndex].MapNumber = mapnumber;

		gObj[aIndex].PathCount = 0;
		gObj[aIndex].Teleport = 0;

		gObjClearViewport(&gObj[aIndex]);
		GCTeleportSend(&gObj[aIndex],-1,mapnumber,gObj[aIndex].X,gObj[aIndex].Y,gObj[aIndex].Dir);

		if(gObj[aIndex].m_Change >= 0)
		{
			gObjViewportListProtocolCreate(&gObj[aIndex]);
		}

		gObj[aIndex].RegenMapNumber = mapnumber;
		gObj[aIndex].RegenMapX = x;
		gObj[aIndex].RegenMapY = y;
		gObj[aIndex].RegenOk = 1;
	}
	else
	{
		PMSG_MAGICATTACK_RESULT pAttack;

		PHeadSetBE((unsigned char *)&pAttack,0x19,sizeof(pAttack));

		pAttack.MagicNumberH = SET_NUMBERH(AT_SKILL_TELEPORT);
		pAttack.MagicNumberL = SET_NUMBERL(AT_SKILL_TELEPORT);

		pAttack.SourceNumberH = SET_NUMBERH(aIndex);
		pAttack.SourceNumberL = SET_NUMBERL(aIndex);
		pAttack.TargetNumberH = SET_NUMBERH(aIndex);
		pAttack.TargetNumberL = SET_NUMBERL(aIndex);

		DataSend(aIndex,(unsigned char *)&pAttack,pAttack.h.size);
		MsgSendV2(&gObj[aIndex],(unsigned char*)&pAttack,pAttack.h.size);
		gObjTeleportMagicUse(aIndex,x,y);
	}
	
//	g_MiniMap.SendMinimap(aIndex);
}






void gObjSetExpPetItem(int aIndex, int exp)
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Class != 4)	
	{
		return;
	}

	exp = (int)(exp*gDarkSpiritAddExperience);

	if(lpObj->pInventory[8].m_Type == ITEMGET(13,4) && lpObj->pInventory[1].m_Type == ITEMGET(13,5))
	{
		int addexp = exp * 10 / 100;

		if(lpObj->pInventory[1].AddPetItemExp(addexp))
		{
			if(gObjIsItemPut(lpObj,&lpObj->pInventory[1],1) == 0)
			{
				lpObj->pInventory[1].PetItemLevelDown(addexp);
				MsgOutput(aIndex,lMsg.Get(1245));
			}
			else
			{
				LogAddTD("[%s][%s][PetItemLevelUp] [%s] Level:[%d]Exp:[%d]AddExp:[%d]",
					lpObj->AccountID,lpObj->Name,lpObj->pInventory[1].GetName(),lpObj->pInventory[1].m_PetItem_Level,lpObj->pInventory[1].m_PetItem_Exp,addexp);
				CDarkSpirit::SendLevelmsg(lpObj->m_Index,1,0,0xFE);
			}
		}
		else
		{
			CDarkSpirit::SendLevelmsg(lpObj->m_Index,1,0,0);
		}

		if(lpObj->pInventory[8].AddPetItemExp(addexp))
		{
			if(gObjIsItemPut(lpObj,&lpObj->pInventory[8],8) == 0)
			{
				lpObj->pInventory[8].PetItemLevelDown(addexp);
				MsgOutput(aIndex,lMsg.Get(1246));
			}
			else
			{
				LogAddTD("[%s][%s][PetItemLevelUp] [%s] Level:[%d]Exp:[%d]AddExp:[%d]",
					lpObj->AccountID,lpObj->Name,lpObj->pInventory[8].GetName(),lpObj->pInventory[8].m_PetItem_Level,lpObj->pInventory[8].m_PetItem_Exp,addexp);
				CDarkSpirit::SendLevelmsg(lpObj->m_Index,8,1,0xFE);
			}
		}
		else
		{
			CDarkSpirit::SendLevelmsg(lpObj->m_Index,8,1,0);
		}
	}
	else if(lpObj->pInventory[8].m_Type == ITEMGET(13,4))
	{
		int addexp = exp * 20 / 100;

		if(lpObj->pInventory[8].AddPetItemExp(addexp))
		{
			if(gObjIsItemPut(lpObj,&lpObj->pInventory[8],8) == 0)
			{
				lpObj->pInventory[8].PetItemLevelDown(addexp);
				MsgOutput(aIndex,lMsg.Get(1246));
			}
			else
			{
				LogAddTD("[%s][%s][PetItemLevelUp] [%s] Level:[%d]Exp:[%d]AddExp:[%d]",
					lpObj->AccountID,lpObj->Name,lpObj->pInventory[8].GetName(),lpObj->pInventory[8].m_PetItem_Level,lpObj->pInventory[8].m_PetItem_Exp,addexp);
				CDarkSpirit::SendLevelmsg(lpObj->m_Index,8,1,0xFE);
			}
		}
		else
		{
			CDarkSpirit::SendLevelmsg(lpObj->m_Index,8,1,0);
		}
	}
	else if(lpObj->pInventory[1].m_Type == ITEMGET(13,5))
	{
		int addexp = exp * 20 / 100;

		if(lpObj->pInventory[1].AddPetItemExp(addexp))
		{
			if(gObjIsItemPut(lpObj,&lpObj->pInventory[1],1) == 0)
			{
				lpObj->pInventory[1].PetItemLevelDown(addexp);
				MsgOutput(aIndex,lMsg.Get(1245));
			}
			else
			{
				LogAddTD("[%s][%s][PetItemLevelUp] [%s] Level:[%d]Exp:[%d]AddExp:[%d]",
					lpObj->AccountID,lpObj->Name,lpObj->pInventory[1].GetName(),lpObj->pInventory[1].m_PetItem_Level,lpObj->pInventory[1].m_PetItem_Exp,addexp);
				CDarkSpirit::SendLevelmsg(lpObj->m_Index,1,0,0xFE);
			}
		}
		else
		{
			CDarkSpirit::SendLevelmsg(lpObj->m_Index,1,0,0);
		}
	}
}






BOOL gObjGetRandomItemDropLocation(int iMapNumber, BYTE& cX, BYTE& cY, int iRangeX, int iRangeY, int iLoopCount)
{
	int iUX = cX;
	int iUY = cY;

	if ( iRangeX <= 0 )
	{
		iRangeX = 1;
	}

	if ( iRangeY <= 0 )
	{
		iRangeY = 1;
	}

	if ( iLoopCount <= 0 )
	{
		iLoopCount = 1;
	}

	while ( iLoopCount-- > 0 )
	{
		cX = ( rand() % (iRangeX+1) ) * (((rand()%2==0)?-1:1)) + iUX;
		cY = ( rand() % (iRangeY+1) ) * (((rand()%2==0)?-1:1)) + iUY;

		BYTE attr = MapC[iMapNumber].GetAttr(cX, cY);

		if ( (attr&4) != 4 && (attr&8) != 8)
		{
			return TRUE;
		}
	}

	return FALSE;
}







BOOL gObjGetRandomFreeLocation(int iMapNumber, BYTE &cX, BYTE & cY, int iRangeX, int iRangeY, int iLoopCount)
{
	int iUX = cX;
	int iUY = cY;

	if ( iRangeX <= 0 )
	{
		iRangeX = 1;
	}

	if ( iRangeY <= 0 )
	{
		iRangeY = 1;
	}

	if ( iLoopCount <= 0 )
	{
		iLoopCount = 1;
	}

	while ( iLoopCount-- > 0 )
	{
		cX = ( rand() % (iRangeX+1) ) * (((rand()%2==0)?-1:1)) + iUX;
		cY = ( rand() % (iRangeY+1) ) * (((rand()%2==0)?-1:1)) + iUY;

		BYTE attr = MapC[iMapNumber].GetAttr(cX, cY);

		if ( attr == 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}







int gObjCheckAttackTypeMagic(int iClass, int iSkill)
{
	if ( iSkill == 0 )
	{
		return 0;
	}

	return MagicDamageC.GetSkillType(iSkill);
}







int gObjGetGuildUnionNumber(LPOBJ lpObj)
{
	int iUnion=0;

	if ( lpObj->lpGuild != NULL )
	{
		iUnion = (lpObj->lpGuild->iGuildUnion == 0)? lpObj->lpGuild->Number : lpObj->lpGuild->iGuildUnion;
	}

	return iUnion;

}






void gObjGetGuildUnionName(LPOBJ lpObj, char* szUnionName, int iUnionNameLen)
{
	szUnionName[0] = 0;
	
	if ( lpObj->lpGuild == NULL )
	{
		return;
	}

	if ( lpObj->lpGuild->iGuildUnion == 0 )
	{
		return;
	}

	TUnionInfo * pUnionInfo = UnionManager.SearchUnion(lpObj->lpGuild->iGuildUnion);

	if ( pUnionInfo != NULL )
	{
		memcpy(szUnionName, pUnionInfo->m_szMasterGuild, iUnionNameLen);
	}
}






BOOL gObjCheckRival(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	if(lpTargetObj->Type != OBJ_USER)
	{
		return false;
	}

	if(lpObj->Type != OBJ_USER)
	{
		return false;
	}

	if(lpTargetObj->lpGuild == 0 || lpObj->lpGuild == 0)
	{
		return false;
	}

	if(lpTargetObj->lpGuild->iGuildRival == 0 || lpObj->lpGuild->iGuildRival == 0)
	{
		return false;
	}

	int iUnion = !lpObj->lpGuild->iGuildUnion ? lpObj->lpGuild->Number : lpObj->lpGuild->iGuildUnion;

	if(UnionManager.GetGuildRelationShip(iUnion,lpTargetObj->lpGuild->Number) == 2)
	{
		return true;
	}
	return false;
}






int gObjGetRelationShip(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	if ( lpObj == NULL || lpTargetObj == NULL )
	{
		return false;
	}

	if ( lpObj->lpGuild == NULL || lpTargetObj->lpGuild == NULL )
	{
		return 0;
	}

	int iUnion = (!lpObj->lpGuild->iGuildUnion) ? lpObj->lpGuild->Number : lpObj->lpGuild->iGuildUnion;

	return UnionManager.GetGuildRelationShip(iUnion, lpTargetObj->lpGuild->Number);
}

struct PMSG_UNION_VIEWPORT_NOTIFY_COUNT {

  // static data ------------------------------------

  // non-static data --------------------------------
  /*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btCount;

  // base classes -----------------------------------

  // friends ----------------------------------------

  // static functions -------------------------------

  // non-virtual functions --------------------------

  // virtual functions ------------------------------
};
// <size 0x5>

struct PMSG_UNION_VIEWPORT_NOTIFY {

  // static data ------------------------------------

  // non-static data --------------------------------
  /*<thisrel this+0x0>*/ /*|0x1|*/ unsigned char btNumberH;
  /*<thisrel this+0x1>*/ /*|0x1|*/ unsigned char btNumberL;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iGuildNumber;
  /*<thisrel this+0x8>*/ /*|0x1|*/ unsigned char btGuildRelationShip;
  /*<thisrel this+0x9>*/ /*|0x8|*/ char szUnionName[8];

  // base classes -----------------------------------

  // friends ----------------------------------------

  // static functions -------------------------------

  // non-virtual functions --------------------------

  // virtual functions ------------------------------
};
// <size 0x14>

void gObjNotifyUpdateUnionV1(LPOBJ lpObj)
{
	if(lpObj == 0)
	{
		return;
	}

	char cBUFFER_V1[6000];
	int iVp1Count = 0;

	memset(cBUFFER_V1,0x00,sizeof(cBUFFER_V1));

	PMSG_UNION_VIEWPORT_NOTIFY_COUNT * lpMsg = (PMSG_UNION_VIEWPORT_NOTIFY_COUNT *)&cBUFFER_V1;
	PMSG_UNION_VIEWPORT_NOTIFY * lpMsgBody = (PMSG_UNION_VIEWPORT_NOTIFY * )&cBUFFER_V1[sizeof(PMSG_UNION_VIEWPORT_NOTIFY_COUNT)];


	for(int n = 0; n < MAX_VIEWPORT; n++)
	{
		if(!OBJMAX_RANGE(lpObj->VpPlayer[n].number))
		{
			continue;
		}

		if(lpObj->VpPlayer[n].state == 2 && lpObj->VpPlayer[n].type == OBJ_USER)
		{
			LPOBJ lpTargetObj = &gObj[lpObj->VpPlayer[n].number];

			if(lpTargetObj == 0)
			{
				continue;
			}

			if(lpTargetObj->lpGuild == 0)
			{
				continue;
			}

			gObjGetGuildUnionName(lpTargetObj,lpMsgBody[iVp1Count].szUnionName,sizeof(lpMsgBody[iVp1Count].szUnionName));

			lpMsgBody[iVp1Count].btGuildRelationShip = gObjGetRelationShip(lpObj,lpTargetObj);
			lpMsgBody[iVp1Count].btNumberL = SET_NUMBERL(WORD(lpTargetObj->m_Index));
			lpMsgBody[iVp1Count].btNumberH = SET_NUMBERH(WORD(lpTargetObj->m_Index));
			lpMsgBody[iVp1Count].iGuildNumber = lpTargetObj->lpGuild->Number;
			iVp1Count++;
		}
	}

	if(iVp1Count > 0 && iVp1Count <= MAX_VIEWPORT)
	{
		lpMsg->btCount = iVp1Count;

		PHeadSetW((unsigned char *)lpMsg,0x67,iVp1Count * sizeof(PMSG_UNION_VIEWPORT_NOTIFY) + sizeof(PMSG_UNION_VIEWPORT_NOTIFY_COUNT));

		DataSend(lpObj->m_Index,(unsigned char *)lpMsg,((lpMsg->h.sizeL & 0xFF) & 0xFF | ((lpMsg->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);
	}
	else
	{
		if(iVp1Count != 0)
		{
			LogAddTD("[Union ViewPort] ERROR : iVp1Count is OUT of BOUND: %d",iVp1Count);
		}
	}
}







void gObjNotifyUpdateUnionV2(LPOBJ lpObj)
{
	if(lpObj == 0)
	{
		return;
	}

	if(lpObj->lpGuild == 0)
	{
		return;
	}

	char cBUFFER_V2[100] = {0};

	PMSG_UNION_VIEWPORT_NOTIFY_COUNT * lpMsg2 = (PMSG_UNION_VIEWPORT_NOTIFY_COUNT *)&cBUFFER_V2;
	PMSG_UNION_VIEWPORT_NOTIFY * lpMsgBody2 = (PMSG_UNION_VIEWPORT_NOTIFY *)&cBUFFER_V2[sizeof(PMSG_UNION_VIEWPORT_NOTIFY_COUNT)];

	gObjGetGuildUnionName(lpObj,lpMsgBody2->szUnionName,sizeof(lpMsgBody2->szUnionName));
	lpMsgBody2->btNumberL = SET_NUMBERL(WORD(lpObj->m_Index));
	lpMsgBody2->btNumberH = SET_NUMBERH(WORD(lpObj->m_Index));

	lpMsgBody2->iGuildNumber = lpObj->lpGuild->Number;
	lpMsgBody2->btGuildRelationShip = 0;
	lpMsg2->btCount = 1;

	PHeadSetW((unsigned char *)lpMsg2,0x67,sizeof(lpMsg2) + sizeof(lpMsgBody2[0])+1);

	DataSend(lpObj->m_Index,(unsigned char *)lpMsg2,((lpMsg2->h.sizeL & 0xFF) & 0xFF | ((lpMsg2->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);

	for(int n = 0; n < MAX_VIEWPORT; n++)
	{
		if(lpObj->VpPlayer2[n].type == OBJ_USER && lpObj->VpPlayer2[n].state != 0)
		{
			LPOBJ lpTargetObj = &gObj[lpObj->VpPlayer2[n].number];

			if(lpTargetObj->lpGuild != 0)
			{
				lpMsgBody2->btGuildRelationShip = gObjGetRelationShip(lpTargetObj,lpObj);
			}

			if(lpMsgBody2->btGuildRelationShip != 1)
			{
				DataSend(lpObj->VpPlayer2[n].number,(unsigned char *)lpMsg2,((lpMsg2->h.sizeL & 0xFF) & 0xFF | ((lpMsg2->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);
			}
		}
	}
}







void gObjUnionUpdateProc(int iIndex)
{

	if ( gObjIsConnected(iIndex) == FALSE )
	{
		return;
	}

	LPOBJ lpObj = &gObj[iIndex];

	if ( lpObj->Type != OBJ_USER )
	{
		return;
	}

	if ( lpObj->RegenOk > 0 )
	{
		return;
	}

	if ( lpObj->CloseCount > -1 )
		return;

	if ( lpObj->lpGuild == NULL )
	{
		return;
	}

	if ( lpObj->lpGuild->CheckTimeStamp(lpObj->iGuildUnionTimeStamp) != FALSE )
	{
		return;
	}

	lpObj->iGuildUnionTimeStamp = (int)lpObj->lpGuild->GetTimeStamp();
	gObjNotifyUpdateUnionV1(lpObj);
	gObjNotifyUpdateUnionV2(lpObj);

}

// _GUILD_INFO_STRUCT 0x4e0a10




void gObjSetKillCount(int aIndex, int iOption)	// Option : [0 : SetToZero] [1 : Increase] [2 : Decrease]
{
	if ( gObjIsConnected(aIndex) == FALSE )
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( iOption== 1 )
	{
		if ( lpObj->m_btKillCount <= 254 )
		{
			lpObj->m_btKillCount++;
		}
	}
	else if ( iOption == 2 )
	{
		if ( lpObj->m_btKillCount > 0 )
		{
			lpObj->m_btKillCount--;
		}
	}
	else if ( iOption == 0 )
	{
		lpObj->m_btKillCount = 0;
	}

	PMSG_KILLCOUNT pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg, 0xB8,0x01, sizeof(pMsg));
	pMsg.btKillCount = lpObj->m_btKillCount;
	
	DataSend( aIndex, (UCHAR*)&pMsg, sizeof(pMsg));
}

struct PMSG_NOTIFY_REGION_OF_WEAPON 
{

  struct PBMSG_HEAD2 h;
  unsigned char btWeaponType;
  unsigned char btPointX;
  unsigned char btPointY;
};

struct PMSG_NOTIFY_TARGET_OF_WEAPON {

  // static data ------------------------------------

  // non-static data --------------------------------
  /*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char NumberH;
  /*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char NumberL;
  /*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btWeaponType;

  // base classes -----------------------------------

  // friends ----------------------------------------

  // static functions -------------------------------

  // non-virtual functions --------------------------

  // virtual functions ------------------------------
};
// <size 0x7>

struct PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT {

  // static data ------------------------------------

  // non-static data --------------------------------
  /*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btCount;

  // base classes -----------------------------------

  // friends ----------------------------------------

  // static functions -------------------------------

  // non-virtual functions --------------------------

  // virtual functions ------------------------------
};
// <size 0x5>

struct PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY {

  // static data ------------------------------------

  // non-static data --------------------------------
  /*<thisrel this+0x0>*/ /*|0x1|*/ unsigned char btObjType;
  /*<thisrel this+0x1>*/ /*|0x1|*/ unsigned char btObjClassH;
  /*<thisrel this+0x2>*/ /*|0x1|*/ unsigned char btObjClassL;
  /*<thisrel this+0x3>*/ /*|0x1|*/ unsigned char btObjIndexH;
  /*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btObjIndexL;
  /*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btX;
  /*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btY;
  /*<thisrel this+0x7>*/ /*|0x11|*/ unsigned char CharSet[17];
  /*<thisrel this+0x18>*/ /*|0x4|*/ unsigned long dwViewSkillState;

  // base classes -----------------------------------

  // friends ----------------------------------------

  // static functions -------------------------------

  // non-virtual functions --------------------------

  // virtual functions ------------------------------
};
// <size 0x1c>

void gObjNotifyUseWeaponV1(LPOBJ lpOwnerObj, LPOBJ lpWeaponObj, int iTargetX, int iTargetY)
{
	if(lpOwnerObj == 0)
	{
		return;
	}

	if(lpWeaponObj == 0)
	{
		return;
	}

	PMSG_NOTIFY_REGION_OF_WEAPON pNotifyRegionMsg = {0};

	PHeadSubSetB((unsigned char *)&pNotifyRegionMsg,0xB7,2,sizeof(pNotifyRegionMsg));

	pNotifyRegionMsg.btPointX = iTargetX;
	pNotifyRegionMsg.btPointY = iTargetY;

	PMSG_NOTIFY_TARGET_OF_WEAPON pNotifyTargetMsg = {0};

	PHeadSubSetB((unsigned char *)&pNotifyTargetMsg,0xB7,3,sizeof(pNotifyTargetMsg));

	if(lpWeaponObj->Class == 0xDD)
	{
		pNotifyRegionMsg.btWeaponType = 1;
		pNotifyTargetMsg.btWeaponType = 1;
	}
	else if(lpWeaponObj->Class == 0xDE)
	{
		pNotifyRegionMsg.btWeaponType = 2;
		pNotifyTargetMsg.btWeaponType = 2;
	}

	char cBUFFER_V1[6000];
	int iVp1Count = 0;

	memset(cBUFFER_V1,0x00,sizeof(cBUFFER_V1));

	PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT * lpMsg = (PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT *)&cBUFFER_V1;
	PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY * lpMsgBody = (PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY *)&cBUFFER_V1[sizeof(PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT)];

	for(int n = 0; n < OBJMAX;n++)
	{
		LPOBJ lpTargetObj = &gObj[n];

		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		if(lpOwnerObj->MapNumber != lpTargetObj->MapNumber)
		{
			continue;
		}

		if(abs(lpTargetObj->X - iTargetX) > 6)
		{
			continue;
		}

		if(abs(lpTargetObj->Y - iTargetY) > 6)
		{
			continue;
		}

		lpMsgBody[iVp1Count].btObjClassH = SET_NUMBERH(lpTargetObj->Class);
		lpMsgBody[iVp1Count].btObjClassL = SET_NUMBERL(lpTargetObj->Class);
		lpMsgBody[iVp1Count].btObjIndexH = SET_NUMBERH(lpTargetObj->m_Index);
		lpMsgBody[iVp1Count].btObjIndexH = SET_NUMBERL(lpTargetObj->m_Index);
		// lpMsgBody[iVp1Count].btObjIndexL = SET_NUMBERL(lpTargetObj->m_Index); need some testing here 
		lpMsgBody[iVp1Count].btX = lpTargetObj->X;
		lpMsgBody[iVp1Count].btY = lpTargetObj->Y;
		//lpMsgBody[iVp1Count].dwViewSkillState = lpTargetObj->m_ViewSkillState;

		if(lpTargetObj->Type == OBJ_USER)
		{
			lpMsgBody[iVp1Count].btObjType = OBJ_USER;
			lpMsgBody[iVp1Count].btObjClassH = CS_SET_CLASS(lpTargetObj->Class);
			lpMsgBody[iVp1Count].btObjClassL = 0;

			memcpy(lpMsgBody[iVp1Count].CharSet,&lpTargetObj->CharSet[1],sizeof(lpMsgBody[iVp1Count].CharSet));
		}
		else if(lpTargetObj->Type == OBJ_MONSTER)
		{
			lpMsgBody[iVp1Count].btObjType = OBJ_MONSTER;
			memset(lpMsgBody[iVp1Count].CharSet,0x00,sizeof(lpMsgBody[iVp1Count].CharSet));
		}
		else
		{
			lpMsgBody[iVp1Count].btObjType = OBJ_NPC;
			memset(lpMsgBody[iVp1Count].CharSet,0x00,sizeof(lpMsgBody[iVp1Count].CharSet));
		}

		iVp1Count++;

		if(lpTargetObj->Type == OBJ_USER)
		{
			DataSend(lpTargetObj->m_Index,(unsigned char *)&pNotifyRegionMsg,sizeof(pNotifyRegionMsg));

			if(abs(lpTargetObj->X - iTargetX) > 3)
			{
				continue;
			}

			if(abs(lpTargetObj->Y - iTargetY) > 3)
			{
				continue;
			}

			pNotifyTargetMsg.NumberH = SET_NUMBERH(lpTargetObj->m_Index);
			pNotifyTargetMsg.NumberL = SET_NUMBERL(lpTargetObj->m_Index);
		}
	}

	if(iVp1Count > 0 && iVp1Count <= 100)
	{
		lpMsg->btCount = iVp1Count;
		PHeadSetW((unsigned char *)lpMsg,0x68,iVp1Count * sizeof(PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY) + sizeof(PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT));

		DataSend(lpOwnerObj->m_Index,(unsigned char *)lpMsg,((lpMsg->h.sizeL & 0xFF) & 0xFF | ((lpMsg->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);
	}
}







void gObjNotifyUseWeaponDamage(LPOBJ lpWeaponObj, int iTargetX, int iTargetY)
{
	if ( lpWeaponObj->Class == 221 || lpWeaponObj->Class == 222)
	{
		for ( int i=0;i<OBJMAX;i++)
		{
			LPOBJ lpObj = &gObj[i];

			if ( lpWeaponObj->MapNumber != lpObj->MapNumber )
			{
				continue;
			}

			if ( abs(lpObj->X - iTargetX) > 3 )
			{
				continue;
			}

			if ( abs(lpObj->Y - iTargetY) > 3 )
			{
				continue;
			}

			g_CsNPC_Weapon.AddWeaponDamagedTargetInfo(lpWeaponObj->m_Index, i, 100);
		}
	}

}







void gObjUseBlessAndSoulPotion(int aIndex, int iItemLevel)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( iItemLevel == 0 )	// Bless Potion
	{
		gObjAddBuffEffect(lpObj,BUFF_POT_BLESS,0,0,0,0,120);
	}
	else if ( iItemLevel == 1 ) // Soul Potion
	{
		gObjAddBuffEffect(lpObj,BUFF_POT_SOUL,0,0,0,0,60);
		gObjUseDrink(lpObj, 0x4D);
	}
}








void gObjWeaponDurDownInCastle(LPOBJ lpObj, LPOBJ lpTargetObj, int iDecValue)
{
	if(lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
	{
		return;
	}

	int itargetdefence = 0;

	CItem * Right = &lpObj->pInventory[0];
	CItem * Left = &lpObj->pInventory[1];

	int bIsRightDurDown = 0;
	int bIsLeftDurDown = 0;


	if(lpObj->Class == 1 || lpObj->Class == 3 || lpObj->Class == 4)
	{
		if(Right->m_Type >= ITEMGET(0,0) && Right->m_Type < ITEMGET(4,0)
			&& Left->m_Type >= ITEMGET(0,0) && Left->m_Type < ITEMGET(4,0))
		{
			bIsRightDurDown = 1;
			bIsLeftDurDown = 1;
		}
	}

	if(Left->m_Type >= ITEMGET(4,0) && Left->m_Type < ITEMGET(4,7)
		|| Left->m_Type == ITEMGET(4,20)
		|| Left->m_Type == ITEMGET(4,21))
	{
		bIsLeftDurDown = 1;
	}
	else if(Right->m_Type >= ITEMGET(4,8) && Right->m_Type < ITEMGET(4,15) || Right->m_Type >= ITEMGET(4,16) && Right->m_Type < ITEMGET(5,0))
	{
		bIsRightDurDown = 1;
	}
	else if(Right->m_Type >= 0 && Right->m_Type < ITEMGET(4,0))
	{
		bIsRightDurDown = 1;
	}
	else if(Right->m_Type >= ITEMGET(5,0) && Right->m_Type < ITEMGET(6,0))
	{
		bIsRightDurDown = 1;
	}

	if(bIsRightDurDown != 0)
	{
		int iRet = Right->SimpleDurabilityDown(iDecValue);

		if(iRet != 0)
		{
			GCItemDurSend(lpObj->m_Index,0,(BYTE)Right->m_Durability,0);

			if(iRet == 2)
			{
				gObjCalCharacter(lpObj->m_Index);
			}
		}
	}

	if(bIsLeftDurDown != 0)
	{
		int iRet = Left->SimpleDurabilityDown(iDecValue); // again a stupid webzen error ? 

		if(iRet != 0)
		{
			GCItemDurSend(lpObj->m_Index,1,(BYTE)Left->m_Durability,0);

			if(iRet == 2)
			{
				gObjCalCharacter(lpObj->m_Index);
			}
		}
	}
}




void gObjReady4Relife(LPOBJ lpObj)
{
	gObjCalCharacter(lpObj->m_Index);
	GCReFillSend(lpObj->m_Index,(WORD)(lpObj->MaxLife + lpObj->AddLife),0xFE,0,lpObj->iMaxShield + lpObj->iAddShield);
}

void gObjCheckTimeOutValue(LPOBJ lpObj, DWORD& rNowTick)
{
	long lTick;
	long lSecond;

	lTick = rNowTick - lpObj->m_dwLastCheckTick;
	lSecond = lTick / 1000;

	if(lSecond == 0)
	{
		return;
	}

	lpObj->m_dwLastCheckTick = rNowTick;
}




void MsgOutput(int aIndex, char* msg, ...) 
{
	char szBuffer[512]="";
	va_list pArguments;

	va_start(pArguments, msg );
	vsprintf(&szBuffer[0], msg, pArguments);
	va_end(pArguments);
	
	GCServerMsgStringSend(&szBuffer[0], aIndex, 1);
}







void gProcessAutoRecuperation(LPOBJ lpObj)
{
	if(lpObj->Life == (lpObj->MaxLife + lpObj->AddLife)
		&& lpObj->Mana == (lpObj->MaxMana + lpObj->AddMana)
		&& lpObj->BP == (lpObj->MaxBP + lpObj->AddBP) )
	{
		lpObj->m_iAutoRecuperationTime = GetTickCount();
		return;
	}

	int addbp = gObjGetTotalValueOfEffect(lpObj,ADD_OPTION_ADDAGRECOVERY);

	if(( GetTickCount() - lpObj->m_iAutoRecuperationTime ) >= 10000 && ( GetTickCount() - lpObj->m_iAutoRecuperationTime ) < 15000)
	{
		if(lpObj->Life < ( lpObj->MaxLife + lpObj->AddLife ) )
		{
			lpObj->Life += 1.0f;

			if(lpObj->Life > (lpObj->MaxLife + lpObj->AddLife))
			{
				lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
			}

			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
		}

		if(lpObj->Mana < ( lpObj->MaxMana + lpObj->AddMana ) || lpObj->BP < ( lpObj->MaxBP + lpObj->AddBP) )
		{
			lpObj->Mana += 1.0f;

			if(lpObj->Mana > (lpObj->MaxMana + lpObj->AddMana))
			{
				lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
			}

			lpObj->BP += 1 + addbp;

			if(lpObj->BP > (lpObj->MaxBP + lpObj->AddBP))
			{
				lpObj->BP = lpObj->MaxBP + lpObj->AddBP;
			}

			GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
		}
	}
	else if(( GetTickCount() - lpObj->m_iAutoRecuperationTime ) >= 15000 && ( GetTickCount() - lpObj->m_iAutoRecuperationTime ) < 25000)
	{
		if(lpObj->Life < ( lpObj->MaxLife + lpObj->AddLife ) )
		{
			lpObj->Life += 5.0f;

			if(lpObj->Life > (lpObj->MaxLife + lpObj->AddLife))
			{
				lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
			}

			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
		}

		if(lpObj->Mana < ( lpObj->MaxMana + lpObj->AddMana ) || lpObj->BP < ( lpObj->MaxBP + lpObj->AddBP) )
		{
			lpObj->Mana += 5.0f;

			if(lpObj->Mana > (lpObj->MaxMana + lpObj->AddMana))
			{
				lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
			}

			lpObj->BP += 5 + addbp;

			if(lpObj->BP > (lpObj->MaxBP + lpObj->AddBP))
			{
				lpObj->BP = lpObj->MaxBP + lpObj->AddBP;
			}

			GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
		}
	}
	else if(( GetTickCount() - lpObj->m_iAutoRecuperationTime ) >= 25000)
	{
		if(lpObj->Life < ( lpObj->MaxLife + lpObj->AddLife ) )
		{
			lpObj->Life += 10.0f;

			if(lpObj->Life > (lpObj->MaxLife + lpObj->AddLife))
			{
				lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
			}

			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
		}

		if(lpObj->Mana < ( lpObj->MaxMana + lpObj->AddMana ) || lpObj->BP < ( lpObj->MaxBP + lpObj->AddBP) )
		{
			lpObj->Mana += 10.0f;

			if(lpObj->Mana > (lpObj->MaxMana + lpObj->AddMana))
			{
				lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
			}

			lpObj->BP += 10 + addbp;

			if(lpObj->BP > (lpObj->MaxBP + lpObj->AddBP))
			{
				lpObj->BP = lpObj->MaxBP + lpObj->AddBP;
			}

			GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
		}
	}
}


void gObjShieldAutoRefill(LPOBJ lpObj)
{
	int iRefillPoint = 0;
	int iShieldRefillOption = 0;

	if(g_ShieldSystemOn == 0)
	{
		return;
	}

	if(g_ShieldAutoRefillOn == 0)
	{
		return;
	}

	if(g_ShieldAutoRefillOnSafeZone == 1)
	{
		unsigned char btMapAttribute = MapC[lpObj->MapNumber].GetAttr(lpObj->X,lpObj->Y);

		if((btMapAttribute & 1) != 1 && lpObj->m_ItemOptionExFor380.OpRefillOn == 0)
		{
			lpObj->dwShieldAutoRefillTimer = GetTickCount();
			return;
		}
	}

	if(lpObj->iShield >= (lpObj->iMaxShield + lpObj->iAddShield))
	{
		lpObj->dwShieldAutoRefillTimer = GetTickCount();
		return;
	}

	iShieldRefillOption = lpObj->m_ItemOptionExFor380.OpAddRefillSD;

	int iRefillExpression = 0;

	float fRefillExpressionA = (float)((lpObj->iMaxShield + lpObj->iAddShield) / 30);
	float fRefillExpressionB = (float)(iShieldRefillOption + 100);

	iRefillExpression = (int)(((fRefillExpressionA * fRefillExpressionB) / 100.0f) / 25.0f);

	unsigned long dwTick = GetTickCount() - lpObj->dwShieldAutoRefillTimer;

	if(dwTick >= 25000)
	{
		iRefillPoint = iRefillExpression * 3; 
	}
	else if(dwTick >= 15000)
	{
		iRefillPoint = (iRefillExpression * 25) / 10;
	}
	else if(dwTick >= 10000)
	{
		iRefillPoint = iRefillExpression * 2;
	}
	else
	{
		return;
	}

	iRefillPoint++;

	if( iRefillPoint == 0 )
		return;

	iRefillPoint += iRefillPoint*lpObj->m_MPSkillOpt.MpsSDSpeed/100.0f;

	lpObj->iShield += iRefillPoint;

	if(lpObj->iShield > (lpObj->iMaxShield + lpObj->iAddShield))
	{
		lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
	}

	GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
}

int gObjCheckOverlapItemUsingDur(int iUserIndex, int iMaxOverlapped, int iItemType, int iItemLevel)
{
	for(int x = INVETORY_WEAR_SIZE; x < MAIN_INVENTORY_SIZE; x++)
	{
		if(gObj[iUserIndex].pInventory[x].IsItem() == 1
			&& gObj[iUserIndex].pInventory[x].m_Type == (short)iItemType
			&& gObj[iUserIndex].pInventory[x].m_Level == (short)iItemLevel)
		{
			int iITEM_DUR = (int)gObj[iUserIndex].pInventory[x].m_Durability;

			if((((iITEM_DUR)<0)?FALSE:((iITEM_DUR)>iMaxOverlapped-1)?FALSE:TRUE ))
			{
				return x;
			}
		}
	}
return -1;
}

int gObjOverlapItemUsingDur(class CItem* lpItem, int iMapNumber, int iItemNumber, int iUserIndex, int iMaxOverlapped, int iItemType, int iItemLevel)
{
	for(int iLoop = 0; iLoop < MAIN_INVENTORY_SIZE; iLoop ++)
	{
		int iInventoryIndex = gObjCheckOverlapItemUsingDur(iUserIndex,iMaxOverlapped,iItemType,iItemLevel);
		if(MAIN_INVENTORY_RANGE(iInventoryIndex))
		{
			int iItemDur = (int)(gObj[iUserIndex].pInventory[iInventoryIndex].m_Durability + lpItem->m_Durability);

			if(iItemDur <= iMaxOverlapped)
			{
				if(MapC[iMapNumber].ItemGive(iUserIndex,iItemNumber,1) == 1)
				{
					return iInventoryIndex;
				}
			}
			else
			{
				lpItem->m_Durability = (float)(iItemDur - iMaxOverlapped);
				gObj[iUserIndex].pInventory[iInventoryIndex].m_Durability = (float)iMaxOverlapped;

				GCItemDurSend(iUserIndex,iInventoryIndex,(BYTE)gObj[iUserIndex].pInventory[iInventoryIndex].m_Durability,0);
			}
		}
		else
		{
			return -1;
		}
	}
	return -1;
}

int gObjCheckSerial0ItemList(class CItem* lpItem)
{
	if(gItemSerialCheck == 0)
	{
		return true;
	}

	if(lpItem->m_Type == ITEMGET(14,13)
		|| lpItem->m_Type == ITEMGET(14,14)
		|| lpItem->m_Type == ITEMGET(14,16)
		|| lpItem->m_Type == ITEMGET(14,22)
		|| lpItem->m_Type == ITEMGET(12,15)
		|| lpItem->m_Type == ITEMGET(12,30)
		|| lpItem->m_Type == ITEMGET(12,31)
		|| lpItem->m_Type == ITEMGET(14,31))
	{
		if(lpItem->GetNumber() == 0)
		{
			//return true;
			return false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

int gObjCheckInventorySerial0Item(LPOBJ lpObj)
{
int iItemSerial;
int iCount;
int i;

	if(gItemSerialCheck == 0)
	{
		return true;
	}

	iCount = 0;

	for(i = 0; i < INVENTORY_SIZE; i++)
	{
		if(lpObj->pInventory[i].IsItem() == 0)
		{
			continue;
		}

		if(gObjCheckSerial0ItemList(&lpObj->pInventory[i])==0)
		{
			continue;
		}

		iItemSerial = lpObj->pInventory[i].GetNumber();

		if(iItemSerial == 0)
		{
			LogAddTD("[ANTI-HACK][Serial 0 Item] (%s)(%s) Item(%s) Pos(%d)",
				lpObj->AccountID,lpObj->Name,lpObj->pInventory[i].GetName(),i);
			iCount++;
		}
	}


	if(iCount > 0)
	{
		return true;
	}
return false;
}





BOOL gCheckSkillDistance(int aIndex, int aTargetIndex, int iSkillNum)
{
	if ( !g_iSkillDistanceCheck  )
		return TRUE;

	if ( iSkillNum == 40 )
		return TRUE;

	int iSkillDistance = MagicDamageC.GetSkillDistance(iSkillNum);

	if ( iSkillDistance == -1 )
		return FALSE;

	iSkillDistance += g_iSkillDistanceCheckTemp;
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[aTargetIndex];

	if ( abs(lpObj->X - lpTargetObj->X) > iSkillDistance ||
		 abs(lpObj->Y - lpTargetObj->Y) > iSkillDistance )
	{
		LogAddTD("[SKILL DISTANCE CHECK] [%s][%s] Invalid Skill attacker(%d, %d), defender(%d, %d), skillnum:%d skilldistance:%d",
			lpObj->AccountID, lpObj->Name,
			lpObj->X, lpObj->Y, 
			lpTargetObj->X, lpTargetObj->Y,
			iSkillNum, iSkillDistance);

		return FALSE;
	}

	return TRUE;
}


void gObjSaveChaosBoxItemList(LPOBJ lpObj)
{
	unsigned char ExOption[0x8];
	for(int n = 0; n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->pChaosBox[n].IsItem() == 1)
		{
			g_ElementalSystem.ClearErtel(lpObj,&lpObj->pChaosBox[n]);
			ItemIsBufExOption(ExOption,&lpObj->pChaosBox[n]);

			LogAddTD("[ChaosBoxItemList][Lost ItemList] [%s][%s] [%d,%s,%d,%d,%d,%d] Serial:[%d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d] 380:[%d] HO:[%d,%d]",
				lpObj->AccountID,lpObj->Name,n,lpObj->pChaosBox[n].GetName(),lpObj->pChaosBox[n].m_Level,lpObj->pChaosBox[n].m_Option1,lpObj->pChaosBox[n].m_Option2,lpObj->pChaosBox[n].m_Option3,lpObj->pChaosBox[n].m_Number,
				(int)lpObj->pChaosBox[n].m_Durability,ExOption[0],ExOption[1],ExOption[2],ExOption[3],ExOption[4],ExOption[5],ExOption[6],lpObj->pChaosBox[n].m_SetOption,
				lpObj->pChaosBox[n].m_ItemOptionEx >> 7,g_kJewelOfHarmonySystem.GetItemStrengthenOption(&lpObj->pChaosBox[n]),g_kJewelOfHarmonySystem.GetItemOptionLevel(&lpObj->pChaosBox[n]));
		}
	}
}

void ExMinusStatFruit(LPOBJ lpObj,int pos)
{
	if( lpObj->Class >= MAX_TYPE_PLAYER || lpObj->Class < 0 )
		return;

	int type = 0;
	int serial = lpObj->pInventory[pos].m_Number;
	int dur = (int)(lpObj->pInventory[pos].m_Durability * 10.0f);
	int user_class = lpObj->Class;

	PMSG_USE_STAT_FRUIT pMsg;

	switch(lpObj->pInventory[pos].m_Type)
	{
	case ITEMGET(13,54): type = 3; break;
	case ITEMGET(13,55): type = 2; break;
	case ITEMGET(13,56): type = 1; break;
	case ITEMGET(13,57): type = 0; break;
	case ITEMGET(13,58): type = 4; break;
	}

	PHeadSetB((PBYTE)&pMsg,0x2C,sizeof(pMsg));
	pMsg.result = 5;
	pMsg.btFruitType = type;
	pMsg.wStatValue = 0;

	if( lpObj->pInventory[ELEMENT_SOCKET].IsItem() )
	{
		pMsg.result = 16;
		DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
		return;
	}

	for(int i = 0; i < INVETORY_WEAR_SIZE; i++)
	{
		if(lpObj->pInventory[i].IsItem())
		{
			pMsg.result = 16;
			DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
			return;
		}
	}

	int def_val = 0;
	int cur_val = 0;
	int bDoMinus = TRUE;

	if(type == 0)
	{
		def_val = DCInfo.DefClass[user_class].Energy;
		cur_val = lpObj->Energy;

		if(cur_val - dur < def_val)
			bDoMinus = FALSE;
	}
	else if(type == 1)
	{
		def_val = DCInfo.DefClass[user_class].Vitality;
		cur_val = lpObj->Vitality;

		if(cur_val - dur < def_val)
			bDoMinus = FALSE;
	}
	else if(type == 2)
	{
		def_val = DCInfo.DefClass[user_class].Dexterity;
		cur_val = lpObj->Dexterity;

		if(cur_val - dur < def_val)
			bDoMinus = FALSE;
	}
	else if(type == 3)
	{
		def_val = DCInfo.DefClass[user_class].Strength;
		cur_val = lpObj->Strength;

		if(cur_val - dur < def_val)
			bDoMinus = FALSE;
	}
	else if(type == 4)
	{
		def_val = DCInfo.DefClass[user_class].Leadership;
		cur_val = lpObj->Leadership;

		if(cur_val - dur < def_val)
			bDoMinus = FALSE;
	}

	if(bDoMinus == FALSE)
	{
		pMsg.result = 38;
		DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);
		return;
	}

	LogAddTD("[CashShop][ExMinusStatFruit] [%s][%s] Use StatDownItem Level:%d,Pos:%d,Dur:%d,Serial:%d",
		lpObj->AccountID,lpObj->Name,type,pos,lpObj->pInventory[pos].m_Durability,serial);

	gObjInventoryItemSet(lpObj->m_Index,pos,0xFF);
	lpObj->pInventory[pos].Clear();
	GCInventoryItemDeleteSend(lpObj->m_Index,pos,1);

	pMsg.result = 3;
	pMsg.btFruitType = type;
	pMsg.wStatValue = dur;

	DataSend(lpObj->m_Index,(PBYTE)&pMsg,pMsg.h.size);

	switch(type)
	{
	case 4:
		lpObj->Leadership -= dur;
		break;
	case 3:
		lpObj->Strength -= dur;
		break;
	case 2:
		lpObj->Dexterity -= dur;
		break;
	case 1:
		lpObj->Vitality -= dur;
		lpObj->MaxLife -= dur * lpObj->VitalityToLife;
		break;
	case 0:
		lpObj->Energy -= dur;
		lpObj->MaxMana -= dur * lpObj->EnergyToMana;
		gObjSetBP(lpObj->m_Index);
		break;
	}

	int oldpts = lpObj->LevelUpPoint;
	lpObj->LevelUpPoint += dur;

	LogAddTD("[CashShop][ExMinusStatFruit] [%s][%s] MinusStat[%d] -> LevelUpPoint Old(%d)/New(%d)",
		lpObj->AccountID,lpObj->Name,dur,oldpts,lpObj->LevelUpPoint);

	gObjCalCharacter(lpObj->m_Index);
	GCReFillSend(lpObj->m_Index,(WORD)(lpObj->MaxLife + lpObj->AddLife),0xFE,0,lpObj->iMaxShield + lpObj->iAddShield);
	GCManaSend(lpObj->m_Index,(WORD)(lpObj->MaxMana + lpObj->AddMana),0xFE,0,lpObj->MaxBP + lpObj->AddBP);
}

#if (GS_CASTLE==1)

int gObjGetRandomFreeArea(int iMapNumber,unsigned char & cX,unsigned char & cY,int iSX,int iSY,int iDX,int iDY,int iLoopCount)
{
	if(iSX <= 0)
		iSX = 1;

	if(iSY <= 0)
		iSY = 1;

	if(iDX <= 0)
		iDX = 1;

	if(iDY <= 0)
		iDY = 1;

	if(iSX >= 256)
		iSX = 255;

	if(iSY >= 256)
		iSY = 255;

	if(iDX >= 256)
		iDX = 255;

	if(iDY >= 256)
		iDY = 255;

	if(iLoopCount <= 0)
		iLoopCount = 1;

	while(iLoopCount-- > 0)
	{
		cX = rand()%(iDX-iSX) + iSX;
		cY = rand()%(iDY-iSY) + iSY;

		BYTE btMapAttr = MapC[iMapNumber].GetAttr(cX,cY);

		if(!btMapAttr)
			return TRUE;
	}

	return FALSE;
}

#endif

void UseCashItems(LPOBJ lpObj,int pos)
{
	if(lpObj == NULL)
		return;

	if(lpObj->Type != OBJ_USER)
		return;

	int itemcode = 0;
	itemcode = lpObj->pInventory[pos].m_Type;

	if(itemcode == ITEMGET(14,82))
	{
		if(lpObj->Class != CLASS_DARKLORD)
		{
			GCItemDurSend(lpObj->m_Index,pos,(BYTE)lpObj->pInventory[pos].m_Durability,1);
			return;
		}
	}

	g_CashItemPeriodSystem.GDReqPeriodItemInsert(lpObj,itemcode,-1);

	lpObj->pInventory[pos].m_Durability -= 1.0f;

	if(lpObj->pInventory[pos].m_Durability > 0.0f)
	{
		GCItemDurSend(lpObj->m_Index,pos,(BYTE)lpObj->pInventory[pos].m_Durability,1);
	}
	else
	{
		gObjInventoryItemSet(lpObj->m_Index,pos,0xFF);
		lpObj->pInventory[pos].Clear();
		GCInventoryItemDeleteSend(lpObj->m_Index,pos,1);
	}
}

void UsePKRemoveItem(LPOBJ lpObj,int pos)
{
	if(lpObj->pInventory[pos].m_Type != ITEMGET(14,72))
		return;

	if(lpObj->pInventory[pos].m_Durability <= 0.0f)
		return;

	int itemdur = 0;
	itemdur = (int)lpObj->pInventory[pos].m_Durability * 60;

	if(lpObj->m_PK_Level <= 3)
		return;

	gObjPkDownTimeCheck(lpObj,itemdur);

	gObjInventoryItemSet(lpObj->m_Index,pos,0xFF);
	lpObj->pInventory[pos].Clear();
	GCInventoryItemDeleteSend(lpObj->m_Index,pos,1);
}

void UseExCashPotions(LPOBJ lpObj,int pos)
{
	if(lpObj == NULL)
		return;

	if(lpObj->Type != OBJ_USER)
		return;

	if(lpObj->pInventory[pos].m_Type == ITEMGET(14,70))
	{
		lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
		GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,1,lpObj->iShield);
	}
	else
	{
		lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
		GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
	}

	lpObj->pInventory[pos].m_Durability -= 1.0f;

	if(lpObj->pInventory[pos].m_Durability > 0.0f)
	{
		GCItemDurSend(lpObj->m_Index,pos,(BYTE)lpObj->pInventory[pos].m_Durability,1);
	}
	else
	{
		gObjInventoryItemSet(lpObj->m_Index,pos,0xFF);
		lpObj->pInventory[pos].Clear();
		GCInventoryItemDeleteSend(lpObj->m_Index,pos,1);
	}
}

std::map<int, PREMIUM_SETTINGS> gPremiumData;
BYTE gPremiumServer[4];

#ifndef SILVER
void LoadPremiumSettings(char* szFileName)
{
	gPremiumData.clear();
	gPremiumServer[1] = 0;
	gPremiumServer[2] = 0;
	gPremiumServer[3] = 0;
	// ----
	xml_document Document;
	xml_parse_result Result = Document.load_file(szFileName);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[Premium] Loading fail [%s]", szFileName);
		return;
	}
	// ----
	xml_node Premium = Document.child("premium");
	PREMIUM_SETTINGS Bronze, Silver, Gold;
	// ----
	gPremiumServer[1] = Premium.child("serverchannel").attribute("bronze").as_int();
	gPremiumServer[2] = Premium.child("serverchannel").attribute("silver").as_int();
	gPremiumServer[3] = Premium.child("serverchannel").attribute("gold").as_int();
	// ----
	Bronze.AddExp = Premium.child("bronze").attribute("exp").as_int();
	Bronze.AddMasterExp = Premium.child("bronze").attribute("masterexp").as_int();
	Bronze.AddDrop = Premium.child("bronze").attribute("drop").as_int();
	Bronze.AddZen = Premium.child("bronze").attribute("zen").as_int(-1);
	Bronze.ResetDiscount = Premium.child("bronze").attribute("resetdiscount").as_int();
	Bronze.GPBonus = Premium.child("bronze").attribute("gpbonus").as_int();

	Silver.AddExp = Premium.child("silver").attribute("exp").as_int();
	Silver.AddMasterExp = Premium.child("silver").attribute("masterexp").as_int();
	Silver.AddDrop = Premium.child("silver").attribute("drop").as_int();
	Silver.AddZen = Premium.child("silver").attribute("zen").as_int(-1);
	Silver.ResetDiscount = Premium.child("silver").attribute("resetdiscount").as_int();
	Silver.GPBonus = Premium.child("silver").attribute("gpbonus").as_int();

	Gold.AddExp = Premium.child("gold").attribute("exp").as_int();
	Gold.AddMasterExp = Premium.child("gold").attribute("masterexp").as_int();
	Gold.AddDrop = Premium.child("gold").attribute("drop").as_int();
	Gold.AddZen = Premium.child("gold").attribute("zen").as_int();
	Gold.ResetDiscount = Premium.child("gold").attribute("resetdiscount").as_int();
	Gold.GPBonus = Premium.child("gold").attribute("gpbonus").as_int();
	// ----
	gPremiumData.insert(std::pair<int, PREMIUM_SETTINGS>(1, Bronze));
	gPremiumData.insert(std::pair<int, PREMIUM_SETTINGS>(2, Silver));
	gPremiumData.insert(std::pair<int, PREMIUM_SETTINGS>(3, Gold));
}
#endif

BOOL gObjDemonSprite(LPOBJ lpObj)
{
	if( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem* lpItem = &lpObj->pInventory[8];

	if( lpItem->m_Type == ITEMGET(13,64) && lpItem->m_Durability > 0.0f)
	{
		if( lpItem->m_bExpired )
		{
			return FALSE;
		}
		return TRUE;
	}
return FALSE;
}

BOOL gObjGuardianSprice(LPOBJ lpObj)
{
	if( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem* lpItem = &lpObj->pInventory[8];

	if( lpItem->m_Type == ITEMGET(13,65) && lpItem->m_Durability > 0.0f)
	{
		if( lpItem->m_bExpired )
		{
			return FALSE;
		}
		return TRUE;
	}
return FALSE;
}

BOOL gObjPandaSprite(LPOBJ lpObj)
{
	if( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem* lpItem = &lpObj->pInventory[8];

	if( lpItem->m_Type == ITEMGET(13,80) && lpItem->m_Durability > 0.0f)
	{
		if( lpItem->m_bExpired )
		{
			return FALSE;
		}
		return TRUE;
	}
return FALSE;
}


BOOL gObjUnicornSprite(LPOBJ lpObj)
{
	if( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem* lpItem = &lpObj->pInventory[8];

	if( lpItem->m_Type == ITEMGET(13,106) && lpItem->m_Durability > 0.0f)
	{
		if( lpItem->m_bExpired )
		{
			return FALSE;
		}
		return TRUE;
	}
return FALSE;
}

BOOL gObjSkeletonSprite(LPOBJ lpObj)
{
	if( lpObj->Type != OBJ_USER )
	{
		return FALSE;
	}

	if( lpObj->pInventory[8].IsItem() == FALSE )
	{
		return FALSE;
	}

	CItem* lpItem = &lpObj->pInventory[8];

	if( lpItem->m_Type == ITEMGET(13,123) && lpItem->m_Durability > 0.0f)
	{
		if( lpItem->m_bExpired )
		{
			return FALSE;
		}
		return TRUE;
	}

return FALSE;
}

void UseMediumElitePotion(LPOBJ lpObj,int pos,double mul)
{
	if( lpObj == NULL )
		return;

	if( lpObj->pInventory[pos].m_Type == ITEMGET(14,94) )
	{
		float maxhp = lpObj->MaxLife + lpObj->Life;
		lpObj->Life += (float)(maxhp * mul);

		if( lpObj->Life > maxhp )
		{
			lpObj->Life = maxhp;
		}

		GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,1,lpObj->iShield);
	}
	else
	{
		float maxmana = lpObj->MaxMana + lpObj->Mana;
		lpObj->Mana += (float)(maxmana * mul);

		if( lpObj->Mana > maxmana )
		{
			lpObj->Mana = maxmana;
		}

		GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,1,lpObj->BP);
	}

	lpObj->pInventory[pos].m_Durability--;

	if( lpObj->pInventory[pos].m_Durability > 0.0f )
	{
		GCItemDurSend(lpObj->m_Index,pos,(BYTE)lpObj->pInventory[pos].m_Durability,1);
	}
	else
	{
		gObjInventoryItemSet(lpObj->m_Index, pos, -1);
		lpObj->pInventory[pos].Clear();
		GCInventoryItemDeleteSend(lpObj->m_Index, pos, 1);
	}
}

BYTE CheckAuthority(int AuthCode,LPOBJ lpObj)
{
	return ((lpObj->Authority & AuthCode) == lpObj->Authority);
}

BOOL isGM(LPOBJ lpObj)
{
	const int GM_CODE	=	32;
	return CheckAuthority(GM_CODE, lpObj) == 1;
}	

BOOL gObjCheckMapBind(int MapNumber)
{
	switch( MapNumber )
	{
	case MAP_INDEX_RORENCIA:
	case MAP_INDEX_DUNGEON:
	case MAP_INDEX_DEVIAS:
	case MAP_INDEX_NORIA:
	case MAP_INDEX_LOSTTOWER:
	case MAP_INDEX_ATHLANSE:
	case MAP_INDEX_TARKAN:
	case MAP_INDEX_ICARUS:
	case MAP_INDEX_AIDA:
	case MAP_INDEX_KANTURU1:
	case MAP_INDEX_KANTURU2:
	case MAP_INDEX_ELBELAND:
	case MAP_INDEX_SWAMPOFCALMNESS:
	case MAP_INDEX_RAKLION:
		return true;
	default:
		return false;
	}
}

void gObjMuBotProc(LPOBJ lpObj)
{
	if( g_bMuBotSystemOn == FALSE )
	{
		return;
	}

	if( lpObj->m_bMUBOT == TRUE )
	{
		lpObj->m_iMUBOT_TIME ++;

		PMSG_MUBOT_USE pMsg;
		PHeadSubSetB((LPBYTE)&pMsg,0xBF,0x51,sizeof(pMsg));

		if( lpObj->m_iMUBOT_TIME >= 60000 )
		{
			pMsg.Status = 1;
			DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
			return;
		}

		if( g_iMuBotTimeInterval < 1 )
		{
			return;
		}

		if( (lpObj->m_iMUBOT_TIME%(g_iMuBotTimeInterval*60)) == 0 )
		{
			lpObj->m_iMUBOT_STAGE = 1+lpObj->m_iMUBOT_TIME/(60*200);

			if( lpObj->m_iMUBOT_STAGE > 5 )
			{
				lpObj->m_iMUBOT_STAGE = 5;
			}

			int RequireZen = g_iMuBotStage1Zen;

			switch( lpObj->m_iMUBOT_STAGE )
			{
			case 5:	RequireZen = g_iMuBotStage5Zen;	break;
			case 4:	RequireZen = g_iMuBotStage4Zen;	break;
			case 3:	RequireZen = g_iMuBotStage3Zen;	break;
			case 2:	RequireZen = g_iMuBotStage2Zen;	break;
			case 1:	RequireZen = g_iMuBotStage1Zen;	break;
			default: 
				{
					return;
				}
			}

			if( lpObj->Money < RequireZen )
			{
				pMsg.Status = 2;
				pMsg.Time = 0;
				pMsg.Money = 0;
			}
			else
			{
				lpObj->Money -= RequireZen;
				GCMoneySend(lpObj->m_Index,lpObj->Money);

				pMsg.Status = 0;
				pMsg.Money = RequireZen;
				pMsg.Time = lpObj->m_iMUBOT_TIME/60;
			}
			
			DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
		}
	}
}

bool IsTransformCharcterSkin(int pChange)
{
	switch(pChange)
	{
	case 2:		//Budge Dragon
	case 7:		//Giant
	case 8:		//Poison Bull Fighter
	case 9:		//Thunder Lich
	case 14:	//Skeleton
	case 41:	//Death Cow
	case 372:	//Elite Skull Soldier
	case 373:	//Jack Olantern
	case 374:	//Santa
	case 378:	//GameMaster
	case 477:	//Transformed Snowman
	case 503:	//Transformed Panda
	case 548:	//Transformed Skeleton
	case 625:	//Robot Knight Transformation Ring
	case 626:	//Mini Robot Transformation Ring
	case 642:	//Great Heavenly Mage Transformation Ring
	case 646:	//Brown Panda Transformation Ring
		return true;
		break;
	}
	return false;
}

bool IsTransformRing(WORD ItemCode)
{
	switch(ItemCode)
	{
	case ITEMGET(13, 10):
	case ITEMGET(13, 39):
	case ITEMGET(13, 40):
	case ITEMGET(13, 41):
	case ITEMGET(13, 42):
	case ITEMGET(13, 68):
	case ITEMGET(13, 76):
	case ITEMGET(13, 77):
	case ITEMGET(13, 78):
	case ITEMGET(13, 122):
	case ITEMGET(13, 163):
	case ITEMGET(13, 164):
	case ITEMGET(13, 165):
	case ITEMGET(13, 166):
		return true;
		break;
	}
	return false;
}

bool IsTransformEquipment(int UserIndex)
{
	if( !gObjIsConnected(UserIndex) )
	{
		return true;
	}
	// ----
	if(		gObj[UserIndex].pInventory[10].IsItem() 
		&&	IsTransformRing(gObj[UserIndex].pInventory[10].m_Type) )
	{
		return true;
	}
	// ----
	if(		gObj[UserIndex].pInventory[11].IsItem() 
		&&	IsTransformRing(gObj[UserIndex].pInventory[11].m_Type) )
	{
		return true;
	}
	// ----
	return false;
}

//OK
BOOL gObjFindInventoryEquipment(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	// ----
	if( gObj[aIndex].Type != 1 )
	{
		return false;
	}
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		gObjSetInventoryEquipment(lpObj, i);
	}
	// ----
	return true;
}

//OK
BOOL gObjSetInventoryEquipment(LPOBJ lpObj, BYTE btPos)
{
	if( lpObj->Type != 1 )
	{
		return false;
	}
	// ----
	if( lpObj->pInventory[btPos].IsItem() )
	{
		switch(lpObj->pInventory[btPos].m_Type)
		{
		case ITEMGET(13, 128):
		case ITEMGET(13, 129):
			{
				if( lpObj->pInventory[btPos].m_Durability == 254.0
					&& (lpObj->m_btSculptPos < MAIN_INVENTORY_SIZE 
					|| lpObj->m_btSculptPos >= MAIN_INVENTORY_SIZE) )
				{
					lpObj->m_btSculptPos = btPos;
				}
			}
			break;
		case ITEMGET(13, 130):
		case ITEMGET(13, 131):
		case ITEMGET(13, 132):
		case ITEMGET(13, 133):
			{
				if( lpObj->pInventory[btPos].m_Durability == 254.0
					&& (lpObj->m_btCharmPos < MAIN_INVENTORY_SIZE 
					|| lpObj->m_btCharmPos >= MAIN_INVENTORY_SIZE) )
				{
					lpObj->m_btCharmPos = btPos;
				}
			}
			break;
		case ITEMGET(13, 134):
			{
				if( lpObj->pInventory[btPos].m_Durability == 254.0
					&& (lpObj->m_btArtifactPos < MAIN_INVENTORY_SIZE 
					|| lpObj->m_btArtifactPos >= MAIN_INVENTORY_SIZE) )
				{
					lpObj->m_btArtifactPos = btPos;
				}
			}
			break;
		}
	}
	// ----
	return true;
}

//OK
BOOL gObjInventoryEquipment(LPOBJ lpObj)
{
	if( lpObj->Type != 1 )
	{
		return false;
	}
	// ----
	BUFF_EFFECT_DATE * lpBuffData	= 0;
	ITEMEFFECT * lpItemEffect		= 0;	//???
	// ----
	if(		lpObj->m_btSculptPos >= INVETORY_WEAR_SIZE 
		&&	lpObj->m_btSculptPos < MAIN_INVENTORY_SIZE )
	{
		lpBuffData = g_BuffEffectSlot.GetEffectDataFromItemCode(lpObj->pInventory[lpObj->m_btSculptPos].m_Type);
		// ----
		if( !lpBuffData )
		{
			return false;
		}
		// ----
		if( lpObj->pInventory[lpObj->m_btSculptPos].m_Durability == 254.0 )
		{
			gObjAddBuffEffect(lpObj, lpBuffData->btIndex);
		}
		else
		{
			lpObj->m_btSculptPos = 0;
			gObjRemoveBuffEffect(lpObj, lpBuffData->btIndex);
		}
	}
	// ----
	if(		lpObj->m_btCharmPos >= INVETORY_WEAR_SIZE 
		&&	lpObj->m_btCharmPos < MAIN_INVENTORY_SIZE )
	{
		lpBuffData = g_BuffEffectSlot.GetEffectDataFromItemCode(lpObj->pInventory[lpObj->m_btCharmPos].m_Type);
		// ----
		if( !lpBuffData )
		{
			return false;
		}
		// ----
		if( lpObj->pInventory[lpObj->m_btCharmPos].m_Durability == 254.0 )
		{
			gObjAddBuffEffect(lpObj, lpBuffData->btIndex);
		}
		else
		{
			lpObj->m_btCharmPos = 0;
			gObjRemoveBuffEffect(lpObj, lpBuffData->btIndex);
		}
	}
	// ----
	if(		lpObj->m_btArtifactPos >= INVETORY_WEAR_SIZE 
		&&	lpObj->m_btArtifactPos < MAIN_INVENTORY_SIZE )
	{
		lpBuffData = g_BuffEffectSlot.GetEffectDataFromItemCode(lpObj->pInventory[lpObj->m_btArtifactPos].m_Type);
		// ----
		if( !lpBuffData )
		{
			return false;
		}
		// ----
		if( lpObj->pInventory[lpObj->m_btArtifactPos].m_Durability == 254.0 )
		{
			gObjAddBuffEffect(lpObj, lpBuffData->btIndex);
		}
		else
		{
			lpObj->m_btArtifactPos = 0;
			gObjRemoveBuffEffect(lpObj, lpBuffData->btIndex);
		}
	}
	// ----
	return true;
}