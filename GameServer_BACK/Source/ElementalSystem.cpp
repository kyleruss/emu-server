#include "stdafx.h"
#include "GameServer.h"
#include "GameMain.h"
#include "ElementalSystem.h"
#include "BuffEffectSlot.h"
#include "MasterSkillSystem.h"
#include "IllusionTempleEvent.h"
#include "DBSockMng.h"
#include "ChaosBox.h"
#include "CastleSiegeSync.h"
#include "DSProtocol.h"
#include "gObjMonster.h"
#include "..\common\winutil.h"
#include "..\common\readscript.h"
#include "CryWolf.h"
#include "BattleSoccerManager.h"
#include "DarkSpirit.h"
#include "CastleSiege.h"
#include "ChaosCastle.h"
#include "ImperialGuardian.h"
#include "ObjAttack.h"
#if( __4GAMERS__ == 1 )
#include "Achievements.h"
#endif

CElementalSystem g_ElementalSystem;

CElementalSystem::CElementalSystem()
{

	m_DamageTable[ELEMENT_FIRE][ELEMENT_FIRE] = 100;
	m_DamageTable[ELEMENT_FIRE][ELEMENT_WATER] = 80;
	m_DamageTable[ELEMENT_FIRE][ELEMENT_EARTH] = 90;
	m_DamageTable[ELEMENT_FIRE][ELEMENT_WIND] = 110;
	m_DamageTable[ELEMENT_FIRE][ELEMENT_DARK] = 120;

	m_DamageTable[ELEMENT_WATER][ELEMENT_FIRE] = 120;
	m_DamageTable[ELEMENT_WATER][ELEMENT_WATER] = 100;
	m_DamageTable[ELEMENT_WATER][ELEMENT_EARTH] = 80;
	m_DamageTable[ELEMENT_WATER][ELEMENT_WIND] = 90;
	m_DamageTable[ELEMENT_WATER][ELEMENT_DARK] = 110;

	m_DamageTable[ELEMENT_EARTH][ELEMENT_FIRE] = 110;
	m_DamageTable[ELEMENT_EARTH][ELEMENT_WATER] = 120;
	m_DamageTable[ELEMENT_EARTH][ELEMENT_EARTH] = 100;
	m_DamageTable[ELEMENT_EARTH][ELEMENT_WIND] = 80;
	m_DamageTable[ELEMENT_EARTH][ELEMENT_DARK] = 90;

	m_DamageTable[ELEMENT_WIND][ELEMENT_FIRE] = 90;
	m_DamageTable[ELEMENT_WIND][ELEMENT_WATER] = 110;
	m_DamageTable[ELEMENT_WIND][ELEMENT_EARTH] = 120;
	m_DamageTable[ELEMENT_WIND][ELEMENT_WIND] = 100;
	m_DamageTable[ELEMENT_WIND][ELEMENT_DARK] = 80;

	m_DamageTable[ELEMENT_DARK][ELEMENT_FIRE] = 80;
	m_DamageTable[ELEMENT_DARK][ELEMENT_WATER] = 90;
	m_DamageTable[ELEMENT_DARK][ELEMENT_EARTH] = 110;
	m_DamageTable[ELEMENT_DARK][ELEMENT_WIND] = 120;
	m_DamageTable[ELEMENT_DARK][ELEMENT_DARK] = 100;

	memset(this->m_ErtelOption,-1,sizeof(m_ErtelOption));
	memset(this->m_classinfo,0,sizeof(this->m_classinfo));
}

CElementalSystem::~CElementalSystem()
{

}

void CElementalSystem::LoadElementalInfo(char* szFileName)
{
	SMDFile = fopen(szFileName,"r");

	if( !SMDFile )
	{
		MsgBox("[Elemental System] Ertel file load fail (%s)",szFileName);
		return;
	}

	int Token,Index,count = 0;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		if( !strcmp(TokenString,"end") )
			break;

		Index = (int)TokenNumber;

		while( TRUE )
		{
			Token = GetToken();

			if( !strcmp(TokenString,"end") )
				break;

			if( Index == 0 )
			{
				m_ErtelOption[count].btId = (BYTE)TokenNumber;

				Token = GetToken();
				BYTE btIndex = (BYTE)TokenNumber;

				Token = GetToken();
				WORD wNumber = (WORD)TokenNumber;

				m_ErtelOption[count].ItemID = ITEMGET(btIndex,wNumber);

				Token = GetToken();
				m_ErtelOption[count].btRank = (BYTE)TokenNumber;

				Token = GetToken();
				m_ErtelOption[count].btOption = (BYTE)TokenNumber;

				Token = GetToken();
				m_ErtelOption[count].OptionRate = (BYTE)TokenNumber;

				Token = GetToken();
				m_ErtelOption[count].wType = (WORD)TokenNumber;

				int i;
				for(i = 0; i < MAX_ERTEL_LEVEL; i++)	{
					Token = GetToken();
					m_ErtelOption[count].Option[i] = (BYTE)TokenNumber;
				}

				for(i = 0; i < MAX_ERTEL_LEVEL; i++)	{
					Token = GetToken();
					m_ErtelOption[count].Rate[i] = (BYTE)TokenNumber;
				}

				Token = GetToken();
				m_ErtelOption[count].btIncType = (BYTE)TokenNumber;
				count++;
			}
			else if( Index == 1 )
			{
				int playerClass = (int)TokenNumber;

				if( playerClass < 0 || playerClass >= MAX_TYPE_PLAYER )	{
					MsgBox("[Elemental System] Damage player class out of bounds!");
					break;
				}

				Token = GetToken();
				this->m_classinfo[playerClass].strMin = (WORD)TokenNumber;

				Token = GetToken();
				this->m_classinfo[playerClass].strMax = (WORD)TokenNumber;

				Token = GetToken();
				this->m_classinfo[playerClass].dexMin = (WORD)TokenNumber;

				Token = GetToken();
				this->m_classinfo[playerClass].dexMax = (WORD)TokenNumber;

				Token = GetToken();
				this->m_classinfo[playerClass].vitMin = (WORD)TokenNumber;

				Token = GetToken();
				this->m_classinfo[playerClass].vitMax = (WORD)TokenNumber;

				Token = GetToken();
				this->m_classinfo[playerClass].eneMin = (WORD)TokenNumber;

				Token = GetToken();
				this->m_classinfo[playerClass].eneMax = (WORD)TokenNumber;

				Token = GetToken();
				this->m_classinfo[playerClass].cmdMin = (WORD)TokenNumber;

				Token = GetToken();
				this->m_classinfo[playerClass].cmdMax = (WORD)TokenNumber;
			}
			else if( Index == 2 )
			{
				int playerClass = (int)TokenNumber;

				if( playerClass < 0 || playerClass >= MAX_TYPE_PLAYER )	{
					MsgBox("[Elemental System] Damage player class out of bounds!");
					break;
				}

				Token = GetToken();
				this->m_classinfo[playerClass].dexDef = (WORD)TokenNumber;
			}
			else if( Index == 3 )
			{
				int playerClass = (int)TokenNumber;

				if( playerClass < 0 || playerClass >= MAX_TYPE_PLAYER )	{
					MsgBox("[Elemental System] Damage player class out of bounds!");
					break;
				}

				Token = GetToken();
				this->m_classinfo[playerClass].DefRate_LEVELMUL = (WORD)TokenNumber;

				for(int i = 0; i < 5; i++)	{
					Token = GetToken();
					this->m_classinfo[playerClass].DefRate_MUL[i] = (WORD)TokenNumber;
				}
			}
			else if( Index == 4 )
			{
				int playerClass = (int)TokenNumber;

				if( playerClass < 0 || playerClass >= MAX_TYPE_PLAYER )	{
					MsgBox("[Elemental System] Damage player class out of bounds!");
					break;
				}

				Token = GetToken();
				this->m_classinfo[playerClass].AttRate_LEVELMUL = (WORD)TokenNumber;

				for(int i = 0; i < 5; i++)	{
					Token = GetToken();
					this->m_classinfo[playerClass].AttRate_MUL[i] = (WORD)TokenNumber;
				}
			}
			else if( Index == 5 )
			{
				int playerClass = (int)TokenNumber;

				if( playerClass < 0 || playerClass >= MAX_TYPE_PLAYER )	{
					MsgBox("[Elemental System] Damage player class out of bounds!");
					break;
				}

				Token = GetToken();
				this->m_classinfo[playerClass].DefRate_LEVELMUL_PVP = (WORD)TokenNumber;

				for(int i = 0; i < 5; i++)	{
					Token = GetToken();
					this->m_classinfo[playerClass].DefRate_MUL_PVP[i] = (WORD)TokenNumber;
				}
			}
			else if( Index == 6 )
			{
				int playerClass = (int)TokenNumber;

				if( playerClass < 0 || playerClass >= MAX_TYPE_PLAYER )	{
					MsgBox("[Elemental System] Damage player class out of bounds!");
					break;
				}

				Token = GetToken();
				this->m_classinfo[playerClass].AttRate_LEVELMUL_PVP = (WORD)TokenNumber;

				for(int i = 0; i < 5; i++)	{
					Token = GetToken();
					this->m_classinfo[playerClass].AttRate_MUL_PVP[i] = (WORD)TokenNumber;
				}
			}
		}
	}
	fclose(SMDFile);
	LoadOptions();
}

void CElementalSystem::LoadPentagramOption(char* szFileName)
{
	SMDFile = fopen(szFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[Elemental System] Pentagram Option file load fail (%s)",szFileName);
		return;
	}

	for (int n = 0; n < MAX_PENTAGRAM_ITEMS; n++)
	{
		this->m_PentagramItems[n].m_itemId = -1;
	}

	for (int n = 0; n < MAX_PENTAGRAM_OPTIONS; n++)
	{
		this->m_PentagramOptionInfo[n].m_btOptionID = -1;
	}

	int Token,Index,Count;
	
	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		Index = TokenNumber;
		Count = 0;

		while( TRUE )
		{
			Token = GetToken();

			if( !strcmp(TokenString,"end") )
				break;

			if( Index == 0 )
			{
				if( Count >= MAX_PENTAGRAM_ITEMS )
				{
					MsgBox("[Elemental System] Pentagram Option Count >= MAX_PENTAGRAM_ITEMS");
					return;
				}

				int itemIndex = TokenNumber;

				Token = GetToken();
				int itemNumber = TokenNumber;

				this->m_PentagramItems[Count].m_itemId = ITEMGET(itemIndex,itemNumber);

				for(int i = 0; i < 9; i++)
				{
					Token = GetToken();
					this->m_PentagramItems[Count].m_btOption[i] = TokenNumber;
				}
				Count++;
			}
			else if( Index == 1 )
			{
				if( Count >= MAX_PENTAGRAM_OPTIONS )
				{
					MsgBox("[Elemental System] Pentagram Option Count >= MAX_PENTAGRAM_OPTIONS");
					return;
				}

				this->m_PentagramOptionInfo[Count].m_btOptionID = TokenNumber;

				for(int i = 0; i < 3; i++)
				{
					Token = GetToken();
					this->m_PentagramOptionInfo[Count].m_Reqs[i].m_btErtelType = TokenNumber;

					Token = GetToken();
					this->m_PentagramOptionInfo[Count].m_Reqs[i].m_btRankReq = TokenNumber;

					Token = GetToken();
					this->m_PentagramOptionInfo[Count].m_Reqs[i].m_btLevelReq = TokenNumber;
				}
				Count++;
			}
		}
	}
	fclose(SMDFile);
}

void CElementalSystem::LoadOptions()
{
	this->m_SlotCountRate.InitPool();

	m_iSpiritMap_DropLevel = GetPrivateProfileInt("GameServerInfo","SpiritMapFragment_DropLevel",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_iSpiritMap_DropRate = GetPrivateProfileInt("GameServerInfo","SpiritMapFragment_DropRate",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	m_iMithril_DropRate = GetPrivateProfileInt("GameServerInfo","MithrilFragment_DropRate",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_iElixir_DropRate = GetPrivateProfileInt("GameServerInfo","ElixirFragment_DropRate",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	m_iMurenBook_DropLevel = GetPrivateProfileInt("GameServerInfo","MurenBook_DropLevel",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_iMurenBook_DropRate = GetPrivateProfileInt("GameServerInfo","MurenBook_DropRate",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	m_iEtramuScroll_DropLevel = GetPrivateProfileInt("GameServerInfo","EtramuScroll_DropLevel",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_iEtramuScroll_DropRate = GetPrivateProfileInt("GameServerInfo","m_iEtramuScroll_DropRate",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	int tmpRate = GetPrivateProfileInt("GameServerInfo","ElementSlotCountRate1",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_SlotCountRate.AddValue(1,tmpRate);

	tmpRate = GetPrivateProfileInt("GameServerInfo","ElementSlotCountRate2",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_SlotCountRate.AddValue(2,tmpRate);

	tmpRate = GetPrivateProfileInt("GameServerInfo","ElementSlotCountRate3",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_SlotCountRate.AddValue(3,tmpRate);

	tmpRate = GetPrivateProfileInt("GameServerInfo","ElementSlotCountRate4",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_SlotCountRate.AddValue(4,tmpRate);

	tmpRate = GetPrivateProfileInt("GameServerInfo","ElementSlotCountRate5",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_SlotCountRate.AddValue(5,tmpRate);
}

BOOL CElementalSystem::Drop(LPOBJ lpObj,LPOBJ lpTargetObj)
{

	if( lpObj->Level >= m_iSpiritMap_DropLevel && rand()%10000 < m_iSpiritMap_DropRate )
	{
		int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(13, 145), 0, 1,
			0, 0, 0, iMaxHitUser, 0, 0);
		return true;
	}

	if( lpObj->MapNumber == MAP_INDEX_ACHERON )
	{
		if( rand()%10000 < m_iElixir_DropRate )
		{
			int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(12,146), 0, 1,
				0, 0, 0, iMaxHitUser, 0, 0);
			return true;
		}

		if( rand()%10000 < m_iMithril_DropRate )
		{
			int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			BYTE SocketBonus = (lpObj->m_btAttribute==ELEMENT_NONE) ? (1+rand()%5) : lpObj->m_btAttribute;
			BYTE SocketOption[5] = {-1};

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(12,144), 0, 1,
				0, 0, 0, iMaxHitUser, 0, 0,SocketBonus,SocketOption);
			return true;
		}

		if( lpObj->Level >= m_iMurenBook_DropLevel && rand()%10000 < m_iMurenBook_DropRate )
		{
			int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
			BYTE SocketBonus = (lpObj->m_btAttribute==ELEMENT_NONE) ? (1+rand()%5) : lpObj->m_btAttribute;
			BYTE SocketOption[5];

			SocketOption[0] = -1;
			SocketOption[1] = -1;
			SocketOption[2] = -1;
			SocketOption[3] = -1;
			SocketOption[4] = -1;

			int scount = this->m_SlotCountRate.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
			int count = 0;

			do
			{
				int slot = rand()%5;

				if( SocketOption[slot] == (BYTE)-1)
				{
					SocketOption[slot] = 0xFE;
					count++;
				}
			}while( scount != count );

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(12,200), 0, 1,
				0, 0, 0, iMaxHitUser, 0, 0,SocketBonus,SocketOption);
			return true;
		}

		if( lpObj->Level >= m_iEtramuScroll_DropLevel && rand()%10000 < m_iEtramuScroll_DropRate )
		{
			int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
			BYTE SocketBonus = (lpObj->m_btAttribute==ELEMENT_NONE) ? (1+rand()%5) : lpObj->m_btAttribute;
			BYTE SocketOption[5];

			SocketOption[0] = -1;
			SocketOption[1] = -1;
			SocketOption[2] = -1;
			SocketOption[3] = -1;
			SocketOption[4] = -1;

			int scount = this->m_SlotCountRate.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
			int count = 0;

			do
			{
				int slot = rand()%5;

				if( SocketOption[slot] == (BYTE)-1)
				{
					SocketOption[slot] = 0xFE;
					count++;
				}
			}while( scount != count );

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(12,201), 0, 1,
				0, 0, 0, iMaxHitUser, 0, 0,SocketBonus,SocketOption);
			return true;
		}
	}

	return false;
}

BOOL CElementalSystem::IsElementalItem(int iType)
{
	switch(iType)
	{
	case ITEMGET(12,144):
	case ITEMGET(12,145):
	case ITEMGET(12,148):
	case ITEMGET(12,200):
	case ITEMGET(12,201):
	case ITEMGET(12,202):
	case ITEMGET(12,203):
	case ITEMGET(12,204):
	case ITEMGET(12,205):
	case ITEMGET(12,206):
	case ITEMGET(12,207):
	case ITEMGET(12,208):
	case ITEMGET(12,209):
	case ITEMGET(12,210):
	case ITEMGET(12,211):
	case ITEMGET(12,212):
	case ITEMGET(12,213):
	case ITEMGET(12,214):
	case ITEMGET(12,221):
	case ITEMGET(12,231):
	case ITEMGET(12,241):
	case ITEMGET(12,251):
	case ITEMGET(12,261):
		return true;
	default:
		return false;
	}
}

BOOL CElementalSystem::IsElementalItem(CItem* lpItem)
{
	if( lpItem == NULL )
		return false;

	return IsElementalItem(lpItem->m_Type);
}

BOOL CElementalSystem::IsPentagram(int iType)
{
	switch(iType)
	{
	case ITEMGET(12,200):
	case ITEMGET(12,201):
	case ITEMGET(12,202):
	case ITEMGET(12,203):
	case ITEMGET(12,204):
	case ITEMGET(12,205):
	case ITEMGET(12,206):
	case ITEMGET(12,207):
	case ITEMGET(12,208):
	case ITEMGET(12,209):
	case ITEMGET(12,210):
	case ITEMGET(12,211):
	case ITEMGET(12,212):
	case ITEMGET(12,213):
	case ITEMGET(12,214):
		return true;
	default:
		return false;
	}
}

BOOL CElementalSystem::IsPentagram(CItem* lpItem)
{
	if( lpItem == NULL )
		return false;

	return IsPentagram(lpItem->m_Type);
}

BOOL CElementalSystem::IsErrtel(int iType)
{
	switch(iType)
	{
	case ITEMGET(12,221):
	case ITEMGET(12,231):
	case ITEMGET(12,241):
	case ITEMGET(12,251):
	case ITEMGET(12,261):
		return true;
	default:
		return false;
	}
}

BOOL CElementalSystem::IsErrtel(CItem* lpItem)
{
	if( lpItem == NULL )
		return false;

	return IsErrtel(lpItem->m_Type);
}

ERTEL_USER* CElementalSystem::GetErtel(LPOBJ lpObj,int iErtelID,int Type)
{
	if( iErtelID < 0 || iErtelID >= MAX_ERTEL )
		return 0;

	if( Type == ERTEL_INVENTORY )
		return &lpObj->pErtelList1[iErtelID];
	else if( Type == ERTEL_WAREHOUSE )
		return &lpObj->pErtelList2[iErtelID];

return 0;
}

BYTE CElementalSystem::GetSlotCount(CItem* pItem)
{
	BYTE count = 0;
	for(int i = 0; i < 5; i++)	{
		if( pItem->m_SocketOption[i] != (BYTE)-1 )
			count++;
	}

return count;
}

void CElementalSystem::RemoveErtel(LPOBJ lpObj,int iErtelID,int Type)
{
	if( iErtelID < 0 || iErtelID >= MAX_ERTEL )
		return;

	if( Type == ERTEL_INVENTORY )
		lpObj->pErtelList1[iErtelID].Clear();
	else if( Type == ERTEL_WAREHOUSE )
		lpObj->pErtelList2[iErtelID].Clear();
		
}

void CElementalSystem::InitElementOptions(LPOBJ lpObj)
{
	lpObj->m_ElementalDamageMin = 0;
	lpObj->m_ElementalDamageMax = 0;
	lpObj->m_ElementalDefense = 0;

	lpObj->m_ElementalAttackRate = 0;
	lpObj->m_ElementalDefenseRate = 0;
	lpObj->m_btAttribute = ELEMENT_NONE;

	memset(lpObj->m_ElementalOptions,0,sizeof(ELEMENTAL_OPTIONS));
}

ERTEL_OPTION* CElementalSystem::GetErtelOption(int iOptionID)
{
	for(int i = 0; i < MAX_ERTEL_OPTION; i++)	{
		if( m_ErtelOption[i].btId == (BYTE)-1 )
			continue;

		if( m_ErtelOption[i].wType == iOptionID )
			return &m_ErtelOption[i];
	}
return NULL;
}

void CElementalSystem::ApplyElementItem(LPOBJ lpObj)
{
	if( lpObj->Type != OBJ_USER || lpObj->Connected < PLAYER_PLAYING )
		return;

	ELEMENT_CLASS_INFO* classInfo = &this->m_classinfo[lpObj->Class];

	InitElementOptions(lpObj);

		if ( classInfo->strMin != 0 && classInfo->strMax != 0)	{
			lpObj->m_ElementalDamageMin += lpObj->Strength/classInfo->strMin;
			lpObj->m_ElementalDamageMax += lpObj->Strength/classInfo->strMax;
		}

		if ( classInfo->dexMin != 0 && classInfo->dexMax != 0)	{
			lpObj->m_ElementalDamageMin += lpObj->Dexterity/classInfo->dexMin;
			lpObj->m_ElementalDamageMax += lpObj->Dexterity/classInfo->dexMax;
		}

		if ( classInfo->vitMin != 0 && classInfo->vitMax != 0)	{
			lpObj->m_ElementalDamageMin += lpObj->Vitality/classInfo->vitMin;
			lpObj->m_ElementalDamageMax += lpObj->Vitality/classInfo->vitMax;
		}

		if ( classInfo->eneMin != 0 && classInfo->eneMax != 0)	{
			lpObj->m_ElementalDamageMin += lpObj->Energy/classInfo->eneMin;
			lpObj->m_ElementalDamageMax += lpObj->Energy/classInfo->eneMax;
		}

		if ( classInfo->cmdMin != 0 && classInfo->cmdMax != 0)	{
			lpObj->m_ElementalDamageMin += lpObj->Leadership/classInfo->cmdMin;
			lpObj->m_ElementalDamageMax += lpObj->Leadership/classInfo->cmdMax;
		}

		if( classInfo->dexDef != 0 )
			lpObj->m_ElementalDefense += lpObj->Dexterity/classInfo->dexDef;

		lpObj->m_ElementalDefenseRate = (lpObj->Level+lpObj->MasterLevel)*classInfo->DefRate_LEVELMUL;
		lpObj->m_ElementalDefenseRate += lpObj->Strength*classInfo->DefRate_MUL[0];
		lpObj->m_ElementalDefenseRate += lpObj->Dexterity*classInfo->DefRate_MUL[1];
		lpObj->m_ElementalDefenseRate += lpObj->Vitality*classInfo->DefRate_MUL[2];
		lpObj->m_ElementalDefenseRate += lpObj->Energy*classInfo->DefRate_MUL[3];
		lpObj->m_ElementalDefenseRate += lpObj->Leadership*classInfo->DefRate_MUL[4];

	CItem* Pentagram = &lpObj->pInventory[236];

	if( Pentagram->IsItem() && IsPentagram(Pentagram) )
	{
		ApplyBonusOptions(lpObj,Pentagram);

		lpObj->m_ElementalDefense += lpObj->m_ElementalDefense*lpObj->m_ElementalOptions->m_DefToElement/100;
		lpObj->m_ElementalDamageMin += lpObj->m_ElementalDamageMin*lpObj->m_ElementalOptions->m_AttToElement/100;
		lpObj->m_ElementalDamageMax += lpObj->m_ElementalDamageMax*lpObj->m_ElementalOptions->m_AttToElement/100;

		lpObj->m_btAttribute = Pentagram->m_SocketBonus&15;
		lpObj->m_ElementalDamageMin += ((Pentagram->m_DamageMin*(100+lpObj->m_ElementalOptions->m_AddPentagramDamage))/100);
		lpObj->m_ElementalDamageMax += ((Pentagram->m_DamageMax*(100+lpObj->m_ElementalOptions->m_AddPentagramDamage))/100);
		lpObj->m_ElementalDefense += ((Pentagram->m_Defense*(100+lpObj->m_ElementalOptions->m_AddPentagramDefense))/100);
		lpObj->m_ElementalDefense /= 2;

		for(int i = 0; i < 5; i++)	{
			if( Pentagram->m_SocketOption[i] == (BYTE)-1 ||
				Pentagram->m_SocketOption[i] == (BYTE)-2 )
				continue;

			ApplyErtelOption(lpObj,Pentagram->m_SocketOption[i]);
		}
	}
}

void CElementalSystem::ApplyErtelOption(LPOBJ lpObj,int iErtelID)
{
	if( iErtelID < 0 || iErtelID >= MAX_ERTEL )
		return;

	ERTEL_USER* lpErtel = &lpObj->pErtelList1[iErtelID];

	for(int i = 0; i < 5; i++)	{
		int iOptionID = lpErtel->btType*100 + i*10 + lpErtel->Option[i];
		int Level = lpErtel->Level[i];

		ApplyOption(lpObj,iOptionID,Level);
	}
}

void CElementalSystem::ApplyOption(LPOBJ lpObj,int iOptionID,int Level)
{
	ERTEL_OPTION* lpOption = GetErtelOption(iOptionID);

	if( lpOption == NULL )
		return;

	int _IncValue = lpOption->Option[Level];
	int _IncType = lpOption->btIncType;

	switch( iOptionID )
	{
	case 101:
		lpObj->m_ElementalDamageMin += _IncValue;
		lpObj->m_ElementalDamageMax += _IncValue;
		break;
	case 111:
	case 311:
		lpObj->m_ElementalOptions->m_AddFirePercDmg += _IncValue;
		break;
	case 112:
	case 312:
		lpObj->m_ElementalOptions->m_AddWaterPercDmg += _IncValue;
		break;
	case 113:
	case 313:
		lpObj->m_ElementalOptions->m_AddEarthPercDmg += _IncValue;
		break;
	case 114:
	case 314:
		lpObj->m_ElementalOptions->m_AddWindPercDmg += _IncValue;
		break;
	case 115:
	case 315:
		lpObj->m_ElementalOptions->m_AddDarkPercDmg += _IncValue;
		break;
	case 121:
	case 321:
		lpObj->m_ElementalOptions->m_AddDmgPVP += _IncValue;
		break;
	case 122:
	case 322:
		lpObj->m_ElementalOptions->m_AddDmgPVE += _IncValue;
		break;
	case 131:
	case 331:
		lpObj->m_ElementalOptions->m_AddRangedPVP += _IncValue;
		break;
	case 132:
	case 332:
		lpObj->m_ElementalOptions->m_AddMaleePVP += _IncValue;
		break;
	case 141:
		lpObj->m_ElementalOptions->m_AddCritRatePVP += _IncValue;
		break;
	case 142:
		lpObj->m_ElementalOptions->m_AddCritRatePVE += _IncValue;
		break;
	case 201:
		lpObj->m_ElementalDefense += _IncValue;
		break;
	case 211:
	case 411:
		lpObj->m_ElementalOptions->m_AddFirePercDef += _IncValue;
		break;
	case 212:
	case 412:
		lpObj->m_ElementalOptions->m_AddWaterPercDef += _IncValue;
		break;
	case 213:
	case 413:
		lpObj->m_ElementalOptions->m_AddEarthPercDef += _IncValue;
		break;
	case 214:
	case 414:
		lpObj->m_ElementalOptions->m_AddWindPercDef += _IncValue;
		break;
	case 215:
	case 415:
		lpObj->m_ElementalOptions->m_AddDarkPercDef += _IncValue;
		break;
	case 221:
	case 421:
		lpObj->m_ElementalOptions->m_AddDefPVP += _IncValue;
		break;
	case 222:
	case 422:
		lpObj->m_ElementalOptions->m_AddDefPVE += _IncValue;
		break;
	case 231:
	case 431:
		lpObj->m_ElementalOptions->m_AddRangedDefPVP += _IncValue;
		break;
	case 232:
	case 432:
		lpObj->m_ElementalOptions->m_AddRangedDefPVP += _IncValue;
		break;
	case 241:
	case 441:
		lpObj->m_ElementalOptions->m_DecreasePVP += _IncValue;
		break;
	case 242:
	case 442:
		lpObj->m_ElementalOptions->m_DecreasePVE += _IncValue;
		break;
	case 301:
		lpObj->m_ElementalOptions->m_AddAttackRate += _IncValue;
		break;
	case 341:
		lpObj->m_ElementalOptions->m_IncreasePVP += _IncValue;
		break;
	case 342:
		lpObj->m_ElementalOptions->m_IncreasePVE += _IncValue;
		break;
	case 401:
		lpObj->m_ElementalOptions->m_AddDefenseRate += _IncValue;
		break;
	case 501:
		lpObj->m_ElementalOptions->m_DebuffRate += _IncValue;
		break;
	case 511:
		lpObj->AddStrength += _IncValue;
		break;
	case 512:
		lpObj->AddDexterity += _IncValue;
		break;
	case 513:
		lpObj->AddEnergy += _IncValue;
		break;
	case 514:
		lpObj->AddVitality += _IncValue;
		break;
	case 521:
		lpObj->AddLife += _IncValue;
		break;
	case 522:
		lpObj->AddMana += _IncValue;
		break;
	case 523:
		lpObj->AddBP += _IncValue;
		break;
	case 524:
		lpObj->iAddShield += _IncValue;
		break;
	case 531:
		lpObj->m_ElementalOptions->m_ExcRatePVP += _IncValue;
		break;
	case 532:
		lpObj->m_ElementalOptions->m_ExcRatePVE += _IncValue;
		break;
	case 541:
		lpObj->m_ElementalOptions->m_ElementalRate += _IncValue;
		break;
	}
}

void CElementalSystem::ApplyBonusOptions(LPOBJ lpObj,CItem* lpPentagram)
{
	PENTAGRAM_OPTION* pPentagramInfo = GetPentagram(lpPentagram->m_Type);

	if( pPentagramInfo == NULL )
		return;

	for(int i = 0; i < 9; i++)
	{
		if( pPentagramInfo->m_btOption[i] != 0 )
		{
			PENTAGRAM_OPTION_INFO* pPentagramOption = this->GetPentagramOption(pPentagramInfo->m_btOption[i]);

			if( pPentagramOption == NULL )
				continue;

			if( !CheckBonusRequirement(lpObj,lpPentagram,pPentagramOption) )
				continue;

			switch( pPentagramOption->m_btOptionID )
			{
			case 1:
				lpObj->m_ElementalOptions->m_AddPentagramDamage += 10;
				break;
			case 2:
				lpObj->m_ElementalOptions->m_AddPentagramDefense += 10;
				break;
			case 3:
				lpObj->m_ElementalOptions->m_AddCritRate += 10;
				break;
			case 4:
				lpObj->m_ElementalOptions->m_DefToElement += 5;
				break;
			case 5:
				lpObj->m_ElementalOptions->m_AttToElement += 1;
				break;
			case 6:
				lpObj->m_ElementalOptions->m_ResistStrongElement += 10;
				break;
			case 7:
				lpObj->m_ElementalOptions->m_ResistElement += 10;
				break;
			}
		}
	}
}

PENTAGRAM_OPTION* CElementalSystem::GetPentagram(int PentagramID)
{
	for(int i = 0; i < MAX_PENTAGRAM_ITEMS; i++)
	{
		if( this->m_PentagramItems[i].m_itemId == PentagramID )
			return &this->m_PentagramItems[i];
	}
	return NULL;
}

PENTAGRAM_OPTION_INFO* CElementalSystem::GetPentagramOption(int OptionID)
{
	for(int i = 0; i < MAX_PENTAGRAM_OPTIONS; i++)
	{
		if( this->m_PentagramOptionInfo[i].m_btOptionID == OptionID )
			return &this->m_PentagramOptionInfo[i];
	}
	return NULL;
}

int CElementalSystem::CheckBonusRequirement(LPOBJ lpObj,CItem* lpPentagram,PENTAGRAM_OPTION_INFO* OptionInfo)
{

	for(int i = 0; i < 3; i++)
	{
		BYTE ErtelType = OptionInfo->m_Reqs[i].m_btErtelType;

		if( ErtelType == (BYTE)-1 )
			continue;

		int iErtelID = lpPentagram->m_SocketOption[ErtelType];

		if( iErtelID < 0 || iErtelID >= MAX_ERTEL )
			return false;

		BYTE btRank = OptionInfo->m_Reqs[i].m_btRankReq;
		BYTE btLevel = OptionInfo->m_Reqs[i].m_btLevelReq;

		if( lpObj->pErtelList1[iErtelID].Option[btRank] == 15 ||
			lpObj->pErtelList1[iErtelID].Level[btRank] < btLevel )
			return false;
	}

	return true;
}

#include "SoulSystem.h"

BOOL CElementalSystem::MissCheck(LPOBJ lpObj, LPOBJ lpTargetObj)
{
#if(__NOVUS__==1)
	if( lpObj->Type == OBJ_MONSTER && lpTargetObj->Type == OBJ_USER )
	{
		if( g_SoulSystem.IsMap(lpTargetObj->MapNumber) && !g_SoulSystem.WithSoul(lpTargetObj) )
		{
			LogAddC(2, "[DEBUG] [SoulSystem] [%s] Miss ignored", lpTargetObj->Name);
			return 1;
		}
	}
#endif

	__try
	{
		BOOL bAllMiss = FALSE;
		int iAttackRate = 0;
		int iDefenseRate = lpTargetObj->m_ElementalDefenseRate;
		
		if( lpTargetObj->Type == OBJ_USER )
		{
			iDefenseRate += iDefenseRate*lpTargetObj->m_ElementalOptions->m_AddDefenseRate/100;
		}

		if ( IT_MAP_RANGE(lpTargetObj->MapNumber) && g_IllusionTemple.GetState(lpTargetObj->MapNumber) == IT_STATE_PLAYING)
		{
			if( lpTargetObj->Type == OBJ_USER )
			{
				if( g_IllusionTemple.Is1stSkillActive(lpTargetObj->iTempleIndex,lpTargetObj->MapNumber) )
				{
					GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, lpObj->m_btAttribute);
					return FALSE;
				}
			}
			
			if( lpObj->PartyNumber == lpTargetObj->PartyNumber )
			{
				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, lpObj->m_btAttribute);
				return FALSE;
			}
		}	

		if ( lpObj->Type == OBJ_USER )	// Miss for Uses
		{
			int Level = lpObj->Level + lpObj->MasterLevel;
			ELEMENT_CLASS_INFO* classInfo = &this->m_classinfo[lpObj->Class];
			iAttackRate = Level*classInfo->AttRate_LEVELMUL;
			iAttackRate += (lpObj->Strength+lpObj->AddStrength)*classInfo->AttRate_MUL[0];
			iAttackRate += (lpObj->Dexterity+lpObj->AddDexterity)*classInfo->AttRate_MUL[1];
			iAttackRate += (lpObj->Vitality+lpObj->AddVitality)*classInfo->AttRate_MUL[2];
			iAttackRate += (lpObj->Energy+lpObj->AddEnergy)*classInfo->AttRate_MUL[3];
			iAttackRate += (lpObj->Leadership+lpObj->AddLeadership)*classInfo->AttRate_MUL[4];
			iAttackRate += iAttackRate*lpObj->m_ElementalOptions->m_AddAttackRate/100;
		}
		else	// Miss for Monsters
		{
			iAttackRate = lpObj->m_ElementalAttackRate;
		}

		if ( iAttackRate < iDefenseRate )
		{
			bAllMiss = TRUE;
		}

		if( iAttackRate < 1 )
		{
			iAttackRate = 1;
		}

		if ( bAllMiss != FALSE )	// When All Miss
		{
			if ( (rand()%100) >= 5 )
			{
				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, lpObj->m_btAttribute);
				return FALSE;
			}
		}
		else	// if the is a chance  ot hit the target
		{
			if ( (rand()%iAttackRate) < iDefenseRate)
			{
				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, lpObj->m_btAttribute);
				return FALSE;
			}
		}
	}
	__except (lpObj->Level=1,-1)
	{
		LogAdd(lMsg.Get(MSGGET(2, 12)), lpObj->Name);
		return FALSE;
	}

	return TRUE;
}

BOOL CElementalSystem::MissCheckPvP(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	float iAttackRate = 0;
	float iDefenseRate = 0;
	int iAttackSuccessRate = 0;

	if ( IT_MAP_RANGE(lpTargetObj->MapNumber) &&
			 g_IllusionTemple.GetState(lpTargetObj->MapNumber) == IT_STATE_PLAYING)
	{
		if( lpTargetObj->Type == OBJ_USER )
		{
			if( g_IllusionTemple.Is1stSkillActive(lpTargetObj->iTempleIndex,lpTargetObj->MapNumber) )
			{
				GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, lpObj->m_btAttribute);
				return FALSE;
			}
		}
		
		if( lpObj->PartyNumber == lpTargetObj->PartyNumber )
		{
			GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, lpObj->m_btAttribute);
			return FALSE;
		}
	}	

	int Level = lpObj->Level + lpObj->MasterLevel;

	ELEMENT_CLASS_INFO* classInfo = &this->m_classinfo[lpObj->Class];

	iAttackRate = Level*classInfo->AttRate_LEVELMUL_PVP;
	iAttackRate += (lpObj->Strength+lpObj->AddStrength)*classInfo->AttRate_MUL_PVP[0];
	iAttackRate += (lpObj->Dexterity+lpObj->AddDexterity)*classInfo->AttRate_MUL_PVP[1];
	iAttackRate += (lpObj->Vitality+lpObj->AddVitality)*classInfo->AttRate_MUL_PVP[2];
	iAttackRate += (lpObj->Energy+lpObj->AddEnergy)*classInfo->AttRate_MUL_PVP[3];
	iAttackRate += (lpObj->Leadership+lpObj->AddLeadership)*classInfo->AttRate_MUL_PVP[4];

	int TargetLevel = lpTargetObj->Level + lpTargetObj->MasterLevel;
	classInfo = &this->m_classinfo[lpTargetObj->Class];

	iDefenseRate = TargetLevel*classInfo->DefRate_LEVELMUL_PVP;
	iDefenseRate += (lpTargetObj->Strength+lpTargetObj->AddStrength)*classInfo->DefRate_MUL_PVP[0];
	iDefenseRate += (lpTargetObj->Dexterity+lpTargetObj->AddDexterity)*classInfo->DefRate_MUL_PVP[1];
	iDefenseRate += (lpTargetObj->Vitality+lpTargetObj->AddVitality)*classInfo->DefRate_MUL_PVP[2];
	iDefenseRate += (lpTargetObj->Energy+lpTargetObj->AddEnergy)*classInfo->DefRate_MUL_PVP[3];
	iDefenseRate += (lpTargetObj->Leadership+lpTargetObj->AddLeadership)*classInfo->DefRate_MUL_PVP[4];

	iAttackRate += iAttackRate*lpObj->m_ElementalOptions->m_AddAttackRate/100;
	iDefenseRate += iDefenseRate*lpTargetObj->m_ElementalOptions->m_AddDefenseRate/100;

	if ( iAttackRate <= 0.0f || iDefenseRate <= 0.0f || lpObj->Level <= 0 || lpTargetObj->Level <= 0 )
		return FALSE;

	float iExpressionA = ( iAttackRate * 10000.0f ) / ( iAttackRate + iDefenseRate );	// #formula
	float iExpressionB = ( Level * 10000.0f ) / ( Level + TargetLevel );	// #formula

	iExpressionA /= 10000.0f;
	iExpressionB /= 10000.0f;

	iAttackSuccessRate = (int)(100.0f * iExpressionA * g_fSuccessAttackRateOption * iExpressionB);

	if ( (TargetLevel - Level) >= 100 )
	{
		iAttackSuccessRate -= 5;
	}
	else if ( (TargetLevel - Level ) >= 200 )
	{
		iAttackSuccessRate -= 10;
	}
	else if ( (TargetLevel - Level) >= 300 )
	{
		iAttackSuccessRate -= 15;
	}

	int dwRate = rand() % 100;

	if ( dwRate > iAttackSuccessRate )
	{
		GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, lpObj->m_btAttribute);

		if ( g_bShieldComboMissOptionOn == TRUE )
		{
			if ( lpObj->comboSkill.ProgressIndex >= 0 )
			{
				LogAddTD("[Shield] ComboSkill Cancel! [%s][%s]",
					lpObj->AccountID, lpObj->Name);

				lpObj->comboSkill.Init();
			}
		}

		return FALSE;
	}

	return TRUE;
}

int CElementalSystem::GetTargetDefense(LPOBJ lpObj,LPOBJ lpTargetObj,CMagicInf* lpMagic,BYTE btAttackElement)
{
	int IncreaseDefense = 0;
	int targetdefense = lpTargetObj->m_ElementalDefense;

	if( lpTargetObj->Type == OBJ_USER )
	{
		switch( btAttackElement )
		{
		case ELEMENT_FIRE:
			IncreaseDefense += lpTargetObj->m_ElementalOptions->m_AddFirePercDef;
			break;
		case ELEMENT_WATER:
			IncreaseDefense += lpTargetObj->m_ElementalOptions->m_AddWaterPercDef;
			break;
		case ELEMENT_EARTH:
			IncreaseDefense += lpTargetObj->m_ElementalOptions->m_AddWaterPercDef;
			break;
		case ELEMENT_WIND:
			IncreaseDefense += lpTargetObj->m_ElementalOptions->m_AddWaterPercDef;
			break;
		case ELEMENT_DARK:
			IncreaseDefense += lpTargetObj->m_ElementalOptions->m_AddWaterPercDef;
			break;
		}

		targetdefense += targetdefense*IncreaseDefense/100;

		if( lpObj->Type == OBJ_USER )
		{
			targetdefense += lpTargetObj->m_ElementalOptions->m_AddDefPVP;

			if( MagicDamageC.GetSkillDistance(lpMagic->m_Skill) >= 3)
			{
				targetdefense += lpTargetObj->m_ElementalOptions->m_AddRangedDefPVP;
			}
			else
			{
				targetdefense += lpTargetObj->m_ElementalOptions->m_AddMaleeDefPVP;
			}
		}
		else
		{
			targetdefense += lpObj->m_ElementalOptions->m_AddDefPVE;
		}
	}

return targetdefense;
}

int CElementalSystem::GetAttackDamage(LPOBJ lpObj,LPOBJ lpTargetObj,int targetdefense,CMagicInf* lpMagic,BYTE btAttackElement,BYTE btDefenseElement)
{
	int sub = 0;
	BYTE cDamage = 0;
	int ElementalDamage = 0;
	int AddDamageMin = 0;
	int AddDamageMax = 0;
	int IncreaseDamage = (btDefenseElement==ELEMENT_NONE) ? 120 : m_DamageTable[btAttackElement][btDefenseElement];

	if( lpObj->Type == OBJ_USER )
	{
		switch( btDefenseElement )
		{
		case ELEMENT_FIRE:
			IncreaseDamage += lpObj->m_ElementalOptions->m_AddFirePercDmg;
			break;
		case ELEMENT_WATER:
			IncreaseDamage += lpObj->m_ElementalOptions->m_AddWaterPercDmg;
			break;
		case ELEMENT_EARTH:
			IncreaseDamage += lpObj->m_ElementalOptions->m_AddWaterPercDmg;
			break;
		case ELEMENT_WIND:
			IncreaseDamage += lpObj->m_ElementalOptions->m_AddWaterPercDmg;
			break;
		case ELEMENT_DARK:
			IncreaseDamage += lpObj->m_ElementalOptions->m_AddWaterPercDmg;
			break;
		}

		if( lpTargetObj->Type == OBJ_USER )
		{
			IncreaseDamage += 30;
			IncreaseDamage += lpObj->m_ElementalOptions->m_IncreasePVP;

			AddDamageMin += lpObj->m_ElementalOptions->m_AddDmgPVP;
			AddDamageMax += lpObj->m_ElementalOptions->m_AddDmgPVP;

			if( MagicDamageC.GetSkillDistance(lpMagic->m_Skill) >= 3)
			{
				AddDamageMin += lpObj->m_ElementalOptions->m_AddRangedPVP;
				AddDamageMax += lpObj->m_ElementalOptions->m_AddRangedPVP;
			}
			else
			{
				AddDamageMin += lpObj->m_ElementalOptions->m_AddMaleePVP;
				AddDamageMax += lpObj->m_ElementalOptions->m_AddMaleePVP;
			}

			if( rand()%100 < (lpObj->m_ElementalOptions->m_AddCritRatePVP+lpObj->m_ElementalOptions->m_AddCritRate) )
			{
				cDamage = 1;
			}

			if( rand()%100 < lpObj->m_ElementalOptions->m_ExcRatePVP )
			{
				cDamage = 2;
			}
		}
		else
		{
			IncreaseDamage += lpObj->m_ElementalOptions->m_IncreasePVE;

			AddDamageMin += lpObj->m_ElementalOptions->m_AddDmgPVE;
			AddDamageMax += lpObj->m_ElementalOptions->m_AddDmgPVE;

			if( rand()%100 < (lpObj->m_ElementalOptions->m_AddCritRatePVE+lpObj->m_ElementalOptions->m_AddCritRate) )
			{
				cDamage = 1;
			}

			if( rand()%100 < lpObj->m_ElementalOptions->m_ExcRatePVE )
			{
				cDamage = 2;
			}
		}
	}

	if( lpTargetObj->Type == OBJ_USER )
	{
		if( lpObj->Type == OBJ_USER )
		{
			IncreaseDamage -= lpTargetObj->m_ElementalOptions->m_DecreasePVP;
		}
		else
		{
			IncreaseDamage -= lpTargetObj->m_ElementalOptions->m_DecreasePVE;
		}
	}

	if( IncreaseDamage < 0 )
		IncreaseDamage = 0;

	__try
	{

		//MsgOutput(lpObj->m_Index,"Base: [%d~%d]",lpObj->m_ElementalDamageMin,lpObj->m_ElementalDamageMax);
		//MsgOutput(lpObj->m_Index,"Increase dmg: %d%%",IncreaseDamage);
		//MsgOutput(lpObj->m_Index,"Add: [%d~%d]",AddDamageMin,AddDamageMax);

		sub = ( lpObj->m_ElementalDamageMax + AddDamageMax) - (lpObj->m_ElementalDamageMin+ AddDamageMin);	// #formula
		ElementalDamage = (lpObj->m_ElementalDamageMin+ AddDamageMin)+(rand()%(sub+1));
	}
	__except ( sub=1, 1 )
	{

	}

	if( cDamage )
	{
		ElementalDamage = lpObj->m_ElementalDamageMax + AddDamageMax;

		if( cDamage == 2)
			ElementalDamage += ElementalDamage*20/100;
	}
	//MsgOutput(lpObj->m_Index,"ElementalDamage: [%d]",ElementalDamage);

	ElementalDamage = ElementalDamage*IncreaseDamage/100;
	ElementalDamage -= targetdefense;

return ElementalDamage;
}

BOOL CElementalSystem::Attack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf* lpMagic)
{
	LPOBJ lpCallObj;
	LPOBJ lpCallTargetObj;

	BYTE btElement = lpObj->m_btAttribute;
	BYTE btTargetElement = lpTargetObj->m_btAttribute;

	if( btElement == ELEMENT_NONE )
		return false;

	if( !lpTargetObj->Live )
		return false;

	if( lpObj->Type == OBJ_USER && lpMagic == NULL )
		return false;
	
	int skill = 0;

	if( lpMagic )
	{
		skill = lpMagic->m_Skill;
	}

	switch(skill)
	{
	case AT_SKILL_SLEEP:
	case AT_SKILL_WEAKNESS:
	case AT_SKILL_INNOVATION:
	case AT_MSKILL_SUM_SLEEP1:
	case AT_MSKILL_SUM_INNOVATION1:
	case AT_MSKILL_SUM_WEAKNESS1:
		return false;
	default:	break;
	}

	if ( (lpTargetObj->Authority&2) == 2 )
		return FALSE;

	if ( (lpObj->Authority&32) == 32 ||
		 (lpTargetObj->Authority&32) == 32 )
	{
		if( gObjCheckUsedBuffEffect(lpObj,BUFF_INVISIBILITY) == TRUE)
		{
			return false;
		}
	}

	if ( lpObj->MapNumber != lpTargetObj->MapNumber )
	{
		return false;
	}

#if (__GENS_SYSTEM__ == 1)
	if (gGensSystem.GetFriendlyFire() && gGensSystem.GensMap(lpObj->MapNumber)) {
		if (lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER
			&& lpObj->m_GensInfo.btFamily == lpTargetObj->m_GensInfo.btFamily) {
			return false;
		}
	}
#endif

	if ( lpObj->MapNumber == MAP_INDEX_PKFIELD && lpObj->m_iDuelUser == -1)
	{
		return false;
	}

#if (GS_CASTLE == 1)

	if(g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_READY || g_Crywolf.GetCrywolfState() == CRYWOLF_STATE_END)
	{
		if( ((lpTargetObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE) ? TRUE : FALSE) && lpTargetObj->Type == OBJ_MONSTER)
		{
			return FALSE;
		}
	}

#endif

	if ( lpObj->GuildNumber > 0 )
	{
		if ( lpObj->lpGuild )
		{
			if ( lpObj->lpGuild->WarState )
			{
				if ( lpObj->lpGuild->WarType == 1 )
				{
					if ( !GetBattleSoccerGoalMove(0) )
					{
						return TRUE;
					}
				}
			}

			if ( lpObj->lpGuild->WarState )
			{
				if ( lpObj->lpGuild->WarType == 0 )
				{
					if ( lpTargetObj->Type == OBJ_MONSTER )
					{
						return TRUE;
					}
				}
			}
		}
	}

	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
		if ( lpTargetObj->m_iMonsterBattleDelay > 0 )
			return TRUE;

		if ( lpTargetObj->Class == 200 )
		{
			return TRUE;
		}

		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_MONSTERMAGIC_RESIST) == TRUE )
		{
			BOOL bCheckAttackIsMagicType = gObjCheckAttackTypeMagic(lpObj->Class, skill);

			if ( bCheckAttackIsMagicType == 1 )
			{
				return TRUE;
			}
		}

		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_MONCHEMISTRY_RESIST) == TRUE  )
		{
			BOOL bCheckAttackIsMagicType = gObjCheckAttackTypeMagic(lpObj->Class, skill);

			if ( bCheckAttackIsMagicType == 0 )
			{
				return TRUE;
			}
		}


		if ( lpTargetObj->m_MonsterSkillElementInfo.m_iSkillElementImmuneTime > 0 )
		{
			if ( lpTargetObj->m_MonsterSkillElementInfo.m_iSkillElementImmuneNumber == skill )
			{
				return TRUE;
			}
		}

		if ( lpTargetObj->m_MonsterSkillElementInfo.m_iSkillElementSpecialImmuneTime > 0 )
		{
			return TRUE;
		}

		if ( lpTargetObj->Class == 523 )
			return true;

		if ( lpTargetObj->Class >= 524 && lpTargetObj->Class <= 528 && lpTargetObj->Class != 526 )
		{
			if( !g_ImperialGuardian.IsAttackAbleMonster(lpTargetObj->m_Index) )
				return true;
		}
	}

	if ( lpTargetObj->Type == OBJ_USER )
	{
		if( lpTargetObj->m_ElementalOptions->m_ResistElement != 0 )
		{
			if( rand()%100 < lpTargetObj->m_ElementalOptions->m_ResistElement )
			{
				GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,btElement);
				return FALSE;
			}
		}

		if( lpTargetObj->m_ElementalOptions->m_ResistStrongElement != 0 )
		{
			if( this->m_DamageTable[lpObj->m_btAttribute][lpTargetObj->m_btAttribute] > 100 )
			{
				if( rand()%100 < lpTargetObj->m_ElementalOptions->m_ResistStrongElement )
				{
					GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,btElement);
					return FALSE;
				}
			}
		}

		if ( !gObjIsConnected(lpTargetObj))
		{
			return FALSE;
		}
	}

	if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_MONSTER )	// PLAYER VS MONSTER
	{
		if ( lpObj->m_RecallMon >= 0 )
		{
			if ( lpObj->m_RecallMon == lpTargetObj->m_Index )
			{
				return FALSE;
			}
		}
	}

	if ( !gObjAttackQ(lpTargetObj))
		return FALSE;

	if ( lpObj->m_RecallMon >= 0 )
		gObjCallMonsterSetEnemy(lpObj, lpTargetObj->m_Index);

	if ( CheckAttackArea(lpObj, lpTargetObj) == FALSE )
		return FALSE;

	lpCallObj = lpObj;

	//Check start

	if ( lpObj->Type == OBJ_MONSTER )
	{
		if ( lpObj->m_RecallMon >= 0 )
		{
			lpCallObj = &gObj[lpObj->m_RecallMon];
		}
	}

	lpCallTargetObj = lpTargetObj;

	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
		if ( lpTargetObj->m_RecallMon >= 0 )
		{
			lpCallTargetObj = &gObj[lpTargetObj->m_RecallMon];
		}
	}

	if ( PkCheck(lpCallObj, lpTargetObj) == FALSE )
		return FALSE;

	int Strength = lpObj->Strength + lpObj->AddStrength;
	int Dexterity = lpObj->Dexterity + lpObj->AddDexterity;
	int Vitality = lpObj->Vitality + lpObj->AddVitality;
	int Energy = lpObj->Energy + lpObj->AddEnergy;
	BOOL bIsOnDuel = gObjDuelCheck(lpObj, lpTargetObj);
	
	
	if( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
	{
		if ( !MissCheckPvP(lpObj, lpTargetObj) )
		{
			GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,btElement);
			return FALSE;
		}
	}
	else if( !MissCheck(lpObj, lpTargetObj) )
	{
		
		GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,btElement);
		return FALSE;
	}

	int targetdefense = GetTargetDefense(lpObj,lpTargetObj,lpMagic,btElement);
	int AttackDamage = GetAttackDamage(lpObj,lpTargetObj, targetdefense, lpMagic,btElement,btTargetElement);

	if( AttackDamage <= 0 )
	{
		GCDamageSend(lpObj->m_Index,lpTargetObj->m_Index,0,btElement);
		return false;
	}

	int tlevel = (lpObj->Level+lpObj->MasterLevel) / 10;

	if ( AttackDamage < tlevel )
	{
		if ( tlevel < 1 )
		{
			tlevel = 1;
		}
		AttackDamage = tlevel;
	}

	gObjSpriteDamage(lpTargetObj, AttackDamage);


#if (GS_CASTLE == 1)

	if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE
		&& lpObj->Type == OBJ_USER
		&& lpTargetObj->Type == OBJ_USER
		&& lpObj->MapNumber == MAP_INDEX_CASTLESIEGE
		&& lpTargetObj->MapNumber == MAP_INDEX_CASTLESIEGE )
	{
		if(lpObj->m_btCsJoinSide == lpTargetObj->m_btCsJoinSide)
		{
			AttackDamage = AttackDamage * 20 / 100;
		}
		else
		{
			AttackDamage = AttackDamage * 40 / 100;
		}
	}
#endif

	if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_MONSTER )
	{
		if ( lpTargetObj->Class == 283 )
		{
			if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_BLESS) == TRUE )
			{
			}
			else if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_SOUL) == TRUE )
			{
			}
			else 
			{
				if ( lpObj->m_iAccumulatedDamage > 100 )
				{
					gObjWeaponDurDownInCastle(lpObj, lpTargetObj, 1);
					lpObj->m_iAccumulatedDamage = 0;
				}
				else
				{
					lpObj->m_iAccumulatedDamage += AttackDamage;
				}

				AttackDamage = AttackDamage * 5 / 100;
			}
		}

		if ( lpTargetObj->Class == 277 )
		{
			if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_BLESS) == TRUE )
			{
			}
			else if ( gObjCheckUsedBuffEffect(lpObj,BUFF_POT_SOUL) == TRUE )
			{
			}
			else 
			{
				if ( lpObj->m_iAccumulatedDamage > 100 )
				{
					gObjWeaponDurDownInCastle(lpObj, lpTargetObj, 1);
					lpObj->m_iAccumulatedDamage = 0;
				}
				else
				{
					lpObj->m_iAccumulatedDamage += AttackDamage;
				}

				AttackDamage = AttackDamage * 5 / 100;
			}
		}
	}

	if ( AttackDamage < 0 )
		AttackDamage = 0;


	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
		gObjAddMsgSendDelay(lpTargetObj, 0, lpObj->m_Index, 100, 0);
		lpTargetObj->LastAttackerID = lpObj->m_Index;

		if ( lpTargetObj->m_iCurrentAI )	
		{
			lpTargetObj->m_Agro.IncAgro(lpObj->m_Index, AttackDamage / 50);
		}
	}

	BOOL selfdefense = 0;
	lpCallObj = lpTargetObj;
	
	if ( lpTargetObj->Type == OBJ_MONSTER )
	{
		if ( lpTargetObj->m_RecallMon >= 0 )
		{
			lpCallObj = &gObj[lpTargetObj->m_RecallMon];
		}
	}

	if ( AttackDamage >= 1 )
	{
		if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
		{
			if ( gObjDuelCheck(lpObj, lpTargetObj) )
			{
				selfdefense = 0;
			}
			else if ( CC_MAP_RANGE(lpObj->MapNumber) || CC_MAP_RANGE(lpTargetObj->MapNumber) )
			{
				selfdefense = 0;
			}
			else if ( IT_MAP_RANGE(lpObj->MapNumber) || IT_MAP_RANGE(lpTargetObj->MapNumber) )
			{
				selfdefense = 0;
			}
			else if ( lpObj->MapNumber == MAP_INDEX_VULCANUS || lpTargetObj->MapNumber == MAP_INDEX_VULCANUS )
			{
				selfdefense = 0;
			}
#if (__GENS_SYSTEM__ == 1)
			else if ( gGensSystem.GensMap(lpObj->MapNumber) )
			{
				selfdefense = 0;
			}
#endif
			else
			{
				selfdefense = 1;
			}

			if ( gObjGetRelationShip(lpObj, lpTargetObj) == 2 )
			{
				selfdefense = FALSE;
			}
#if (GS_CASTLE == 1)			
			if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE && lpObj->m_btCsJoinSide > 0)
			{
				selfdefense = FALSE;
			}
#endif
		}
		else if ( lpTargetObj->Type == OBJ_MONSTER && lpObj->Type == OBJ_USER )
		{
			if ( lpTargetObj->m_RecallMon >= 0 )
			{
				selfdefense = TRUE;
			}
		}

		if( lpTargetObj->Type == OBJ_USER )
		{
			gObjArmorRandomDurDown(lpTargetObj, lpObj);
		}

		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_ICEARROW) == TRUE )
		{
			gObjAddBuffEffect(lpTargetObj,BUFF_ICEARROW,-1);

			if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_ICEARROW) == FALSE )
			{
				GCMagicCancelSend(lpTargetObj,51);
			}
		}

		if ( gObjCheckUsedBuffEffect(lpTargetObj,BUFF_SLEEP) == TRUE )
		{
			gObjRemoveBuffEffect(lpTargetObj,BUFF_SLEEP);
		}
	}

	if ( lpObj->PartyNumber >= 0 &&
		 lpCallObj->PartyNumber >= 0)
	{
		if( lpObj->PartyNumber == lpCallObj->PartyNumber )
		{
			selfdefense = 0;
		}
	}

	if ( selfdefense == TRUE )
	{
		if ( !gObjTargetGuildWarCheck(lpObj, lpCallObj) )
		{
			if( lpCallObj->PartyNumber >= 0 )
			{
				int bTmp = 0;
				int PartyNumber = lpCallObj->PartyNumber;

				if( gParty.GetPartyPkLevel(PartyNumber) < 5 )
				{
					gObjCheckSelfDefense(lpObj, lpCallObj->m_Index);
				}
			}
			else
			{
				gObjCheckSelfDefense(lpObj, lpCallObj->m_Index);
			}
		}
	}

	if ( lpTargetObj->Class == 275 )	// KUNDUN
	{
		if ( lpTargetObj->m_iMonsterBattleDelay <= 0 )
		{
			if ( (rand()%15) < 1 )
			{
				gObjAddMsgSendDelay(lpTargetObj, 4, lpObj->m_Index, 100, 0);
				lpTargetObj->m_iMonsterBattleDelay = 10;
				GCActionSend(lpTargetObj, 126, lpTargetObj->m_Index, lpObj->m_Index);
			}
		}
	}

	lpObj->m_Rest = 0;

	if ( AttackDamage > 0 )
	{

		if ( lpObj->Type == OBJ_USER && lpTargetObj->Type == OBJ_USER )
		{
			int iTempShieldDamage = gclassObjAttack.GetShieldDamage(lpObj, lpTargetObj, AttackDamage);
			lpTargetObj->iShield -= iTempShieldDamage;
			lpTargetObj->Life -= AttackDamage - iTempShieldDamage;

			if ( lpTargetObj->Life < 0.0f )
			{
				lpTargetObj->Life = 0.0f;
			}
		}
		else
		{
			lpTargetObj->Life -= AttackDamage;

			if ( lpTargetObj->Life < 0.0f )
			{
				lpTargetObj->Life = 0.0f;
			}
		}

		gObjLifeCheck(lpTargetObj,lpObj,AttackDamage,4,btElement,0,skill,0);
	}
	else
	{
		GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, btElement);
	}

	if ( lpObj->Life <= 0.0f && lpObj->Type == OBJ_USER )
	{
		if ( lpObj->m_CheckLifeTime <= 0 )
		{
			lpObj->lpAttackObj = lpTargetObj;

			if ( lpTargetObj->Type == OBJ_USER )
			{
				lpObj->m_bAttackerKilled = true;
			}
			else
			{
				lpObj->m_bAttackerKilled = false;
			}

			lpObj->m_CheckLifeTime = 3;
		}
	}
	return TRUE;
}

void CElementalSystem::GDReqErtelList(int aIndex)
{
	PMSG_REQ_ERTELLIST pMsg;
	PHeadSetB((LPBYTE)&pMsg,0xA4,sizeof(pMsg));

	pMsg.aIndex = aIndex;
	memcpy(pMsg.szAccount,gObj[aIndex].AccountID,10);
	memcpy(pMsg.szName,gObj[aIndex].Name,10);

	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));
}

void CElementalSystem::DGAnsErtelList(PMSG_ANS_ERTELLIST* lpMsg)
{
	if( lpMsg == NULL )
		return;

	char szName[11];
	int aIndex = lpMsg->aIndex;

	memcpy(szName,lpMsg->szName,10);
	szName[10] = '\0';
	

	if( !gObjIsConnected(aIndex) || strcmp(szName,gObj[aIndex].Name) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	for(int n = 0; n < MAX_ERTEL; n++)	{

		if( lpMsg->ErtelList1[DB_ERTELSIZE*n+DB_ERTELID] != (BYTE)-1 )
		{
			lpObj->pErtelList1[n].btType = lpMsg->ErtelList1[DB_ERTELSIZE*n+DB_ERTELID];
			lpObj->pErtelList1[n].btAttribute = lpMsg->ErtelList1[DB_ERTELSIZE*n+DB_ATTRIBUTE];

			for(int i = 0; i < 5; i++)	{
				lpObj->pErtelList1[n].Option[i] = lpMsg->ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP1+i]%16;
				lpObj->pErtelList1[n].Level[i] = lpMsg->ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP1+i]/16;
			}
		}

		if( lpMsg->ErtelList2[DB_ERTELSIZE*n+DB_ERTELID] != (BYTE)-1 )
		{
			lpObj->pErtelList2[n].btType = lpMsg->ErtelList2[DB_ERTELSIZE*n+DB_ERTELID];
			lpObj->pErtelList2[n].btAttribute = lpMsg->ErtelList2[DB_ERTELSIZE*n+DB_ATTRIBUTE];

			for(int i = 0; i < 5; i++)	{
				lpObj->pErtelList2[n].Option[i] = lpMsg->ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP1+i]%16;
				lpObj->pErtelList2[n].Level[i] = lpMsg->ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP1+i]/16;
			}
		}
	}

	gObjCalCharacter(lpObj->m_Index);

	SendElementalItems(lpObj,ERTEL_INVENTORY);
	SendElementalItems(lpObj,ERTEL_WAREHOUSE);
}

void CElementalSystem::GDSaveErtelList(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	PMSG_SAVE_ERTELLIST pMsg;
	PHeadSetW((LPBYTE)&pMsg,0xA5,sizeof(pMsg));

	memcpy(pMsg.szAccount,lpObj->AccountID,10);
	memcpy(pMsg.szName,lpObj->Name,10);


	for(int n = 0; n < MAX_ERTEL; n++)	{

		if( lpObj->pErtelList1[n].btType != (BYTE)-1 )
		{
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELID] = lpObj->pErtelList1[n].btType;
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ATTRIBUTE] = lpObj->pErtelList1[n].btAttribute;
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP1] = lpObj->pErtelList1[n].Level[0]*16 | lpObj->pErtelList1[n].Option[0];
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP2] = lpObj->pErtelList1[n].Level[1]*16 | lpObj->pErtelList1[n].Option[1];
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP3] = lpObj->pErtelList1[n].Level[2]*16 | lpObj->pErtelList1[n].Option[2];
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP4] = lpObj->pErtelList1[n].Level[3]*16 | lpObj->pErtelList1[n].Option[3];
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP5] = lpObj->pErtelList1[n].Level[4]*16 | lpObj->pErtelList1[n].Option[4];
		}
		else
		{
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELID] = -1;
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ATTRIBUTE] = -1;
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP1] = -1;
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP2] = -1;
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP3] = -1;
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP4] = -1;
			pMsg.ErtelList1[DB_ERTELSIZE*n+DB_ERTELOP5] = -1;
		}

		if( lpObj->pErtelList2[n].btType != (BYTE)-1 )
		{
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELID] = lpObj->pErtelList2[n].btType;
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ATTRIBUTE] = lpObj->pErtelList2[n].btAttribute;
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP1] = lpObj->pErtelList2[n].Level[0]*16 | lpObj->pErtelList2[n].Option[0];
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP2] = lpObj->pErtelList2[n].Level[1]*16 | lpObj->pErtelList2[n].Option[1];
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP3] = lpObj->pErtelList2[n].Level[2]*16 | lpObj->pErtelList2[n].Option[2];
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP4] = lpObj->pErtelList2[n].Level[3]*16 | lpObj->pErtelList2[n].Option[3];
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP5] = lpObj->pErtelList2[n].Level[4]*16 | lpObj->pErtelList2[n].Option[4];
		}
		else
		{
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELID] = -1;
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ATTRIBUTE] = -1;
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP1] = -1;
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP2] = -1;
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP3] = -1;
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP4] = -1;
			pMsg.ErtelList2[DB_ERTELSIZE*n+DB_ERTELOP5] = -1;
		}
	}

	cDBSMng.Send((PCHAR)&pMsg,sizeof(pMsg));
}

void CElementalSystem::CGInsertErtel(int aIndex,PMSG_REQ_INSERTERTEL* lpMsg)
{
	if( lpMsg == NULL )
		return;

	if( !gObjIsConnected(aIndex) )
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use )
	{
		return;
	}

	PMSG_ANS_INSERTERTEL pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEC,0x00,sizeof(pMsg));

	pMsg.btResult = FALSE;

	

	if( !MAIN_INVENTORY_RANGE(lpMsg->iSource) ||
		!MAIN_INVENTORY_RANGE(lpMsg->iTarget) )
	{
		LogAddTD("[Ertel Insert][ERROR] Inventory range error - [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	CItem* lpSource = &lpObj->pInventory[lpMsg->iSource];
	CItem* lpTarget = &lpObj->pInventory[lpMsg->iTarget];

	int Socket = -1;

	switch( lpSource->m_Type )	{
	case ITEMGET(12,221):	Socket = 0;	break;
	case ITEMGET(12,231):	Socket = 1;	break;
	case ITEMGET(12,241):	Socket = 2;	break;
	case ITEMGET(12,251):	Socket = 3;	break;
	case ITEMGET(12,261):	Socket = 4;	break;
	}

	if( Socket == -1 )	{
		LogAddTD("[Ertel Insert][ERROR] ErtelID == -1 [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpTarget->m_SocketOption[Socket] != (BYTE)-2 )
	{
		LogAddTD("[Ertel Insert][ERROR] ErtelSocket not empty ! [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( (lpSource->m_SocketBonus&0x0F) != (lpTarget->m_SocketBonus&0x0F) )
	{
		LogAddTD("[Ertel Insert][ERROR] Ertel element dont match book element ! [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	int ErtelID = InsertErtel(lpObj,lpSource);

	if( ErtelID == (BYTE)-1 )
	{
		LogAddTD("[Ertel Insert][ERROR] Ertel list is full ! [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	ERTEL_USER* lpErtel = &lpObj->pErtelList1[ErtelID];

	lpTarget->m_SocketOption[Socket] = ErtelID;

	pMsg.ErtelInfo.btType = ERTEL_INVENTORY;
	pMsg.ErtelInfo.btId = ErtelID;
	pMsg.ErtelInfo.btAttribute = lpTarget->m_SocketBonus;
	pMsg.ErtelInfo.btIndex = 12;
	pMsg.ErtelInfo.wNumber = 221 + Socket*10;
	pMsg.ErtelInfo.btUnk4 = 0;

	for(int i = 0; i < 5; i++)	{
		pMsg.ErtelInfo.Option[i].OptionID = lpErtel->Option[i];
		pMsg.ErtelInfo.Option[i].OptionLevel = lpErtel->Level[i];
	}
	pMsg.btResult = TRUE;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);

	PMSG_ERTEL_RESULT pResult;
	PHeadSubSetB((LPBYTE)&pResult,0xEC,0x04,sizeof(pResult));
	pResult.btResult = TRUE;

	gObjInventoryItemSet(aIndex, lpMsg->iSource, -1);
	lpSource->Clear();
	GCInventoryItemOneSend(aIndex, lpMsg->iTarget);

	DataSend(aIndex,(LPBYTE)&pResult,pResult.h.size);
}

void CElementalSystem::CGRemoveErtel(int aIndex,PMSG_REQ_REMOVEERTEL* lpMsg)
{
	if( lpMsg == NULL )
		return;

	if( !gObjIsConnected(aIndex) )
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use )
	{
		return;
	}

	PMSG_ANS_REMOVEERTEL pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEC,0x1,sizeof(pMsg));

	pMsg.btResult = 0;
	pMsg.btType = ERTEL_INVENTORY;
	pMsg.btPos = lpMsg->ErtelPos;

	if( !MAIN_INVENTORY_RANGE(lpMsg->iPentagramPos) )
	{
		LogAddTD("[Ertel Remove][ERROR] Inventory range error - [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	CItem* lpSource = &lpObj->pInventory[lpMsg->iPentagramPos];

	if( lpSource->m_SocketOption[pMsg.btPos] == (BYTE)-1 ||
		lpSource->m_SocketOption[pMsg.btPos] == (BYTE)-2 )
	{
		LogAddTD("[Ertel Remove][ERROR] Inventory range error - [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	ERTEL_USER* lpErtel = GetErtel(lpObj,lpSource->m_SocketOption[pMsg.btPos],ERTEL_INVENTORY);

	if( lpErtel == NULL )
	{
		LogAddTD("[Ertel Remove][ERROR] Inventory range error - [%d](%s)(%s)",aIndex,lpObj->AccountID,lpObj->Name);
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( !CheckInventoryEmptySpace(lpObj,2,2) )
	{
		pMsg.btResult = 100;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}	

	if( rand()%100 < 50 )
	{
		pMsg.btResult = 1;
		int Type = ITEMGET(12,221+10*pMsg.btPos);

		BYTE SocketBonus = lpErtel->btAttribute;
		BYTE SocketOption[5] = {-1};

		for(int i = 0; i < 5; i++)	{
			SocketOption[i] = lpErtel->Level[i]*16 | lpErtel->Option[i];
		}

		ItemSerialCreateSend(lpObj->m_Index,235,lpObj->X,lpObj->Y,Type,0,0,0,0,0,lpObj->m_Index,0,0,SocketBonus,SocketOption);

		LogAddTD("[Ertel Remove] [%d] (%s)(%s) [Success] ErrtelID: %d, Op1[%d/%d],Op2[%d/%d],Op3[%d/%d],Op4[%d/%d],Op5[%d/%d]",aIndex,lpObj->AccountID,lpObj->Name,
			lpErtel->Option[0],lpErtel->Level[0],lpErtel->Option[1],lpErtel->Level[1],
			lpErtel->Option[2],lpErtel->Level[2],lpErtel->Option[3],lpErtel->Level[3],
			lpErtel->Option[4],lpErtel->Level[4]);
	}
	else
	{
		pMsg.btResult = 2;
		LogAddTD("[Ertel Remove] [%d] (%s)(%s) [Fail] ErrtelID: %d, Op1[%d/%d],Op2[%d/%d],Op3[%d/%d],Op4[%d/%d],Op5[%d/%d]",aIndex,lpObj->AccountID,lpObj->Name,
			lpErtel->Option[0],lpErtel->Level[0],lpErtel->Option[1],lpErtel->Level[1],
			lpErtel->Option[2],lpErtel->Level[2],lpErtel->Option[3],lpErtel->Level[3],
			lpErtel->Option[4],lpErtel->Level[4]);
	}

	RemoveErtel(lpObj,lpSource->m_SocketOption[pMsg.btPos],ERTEL_INVENTORY);
	lpSource->m_SocketOption[pMsg.btPos] = (BYTE)-2;
	GCInventoryItemOneSend(aIndex, lpMsg->iPentagramPos);
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);

	PMSG_ERTEL_RESULT pResult;
	PHeadSubSetB((LPBYTE)&pResult,0xEC,0x04,sizeof(pResult));
	pResult.btResult = TRUE;
	DataSend(aIndex,(LPBYTE)&pResult,pResult.h.size);
}

BYTE CElementalSystem::InsertErtel(LPOBJ lpObj,CItem* ErtelItem)
{
	for(int i = 0; i < MAX_ERTEL; i++)	{
		if( lpObj->pErtelList1[i].btType == (BYTE)-1 )	{
			ERTEL_USER* lpErtel = &lpObj->pErtelList1[i];

			lpErtel->btType = 1+(ErtelItem->m_Type - ITEMGET(12,221))/10;
			lpErtel->btAttribute = ErtelItem->m_SocketBonus;

			lpErtel->Option[0] = ErtelItem->m_SocketOption[0]%16;
			lpErtel->Level[0] = ErtelItem->m_SocketOption[0]/16;

			lpErtel->Option[1] = ErtelItem->m_SocketOption[1]%16;
			lpErtel->Level[1] = ErtelItem->m_SocketOption[1]/16;

			lpErtel->Option[2] = ErtelItem->m_SocketOption[2]%16;
			lpErtel->Level[2] = ErtelItem->m_SocketOption[2]/16;

			lpErtel->Option[3] = ErtelItem->m_SocketOption[3]%16;
			lpErtel->Level[3] = ErtelItem->m_SocketOption[3]/16;

			lpErtel->Option[4] = ErtelItem->m_SocketOption[4]%16;
			lpErtel->Level[4] = ErtelItem->m_SocketOption[4]/16;
			return i;
		}
	}
	return -1;
}

void CElementalSystem::BuildErtelInfo(PMSG_ERTELINFO* lpErtelInfo,ERTEL_USER* Ertel,int Id,int Type)
{
	lpErtelInfo->btType = Type;
	lpErtelInfo->btId = Id;
	lpErtelInfo->btIndex = 12;
	lpErtelInfo->wNumber = 221 + (Ertel->btType-1)*10;
	lpErtelInfo->btAttribute = Ertel->btAttribute;

	lpErtelInfo->Option[0].OptionID = Ertel->Option[0];
	lpErtelInfo->Option[0].OptionLevel = Ertel->Level[0];

	lpErtelInfo->Option[1].OptionID = Ertel->Option[1];
	lpErtelInfo->Option[1].OptionLevel = Ertel->Level[1];

	lpErtelInfo->Option[2].OptionID = Ertel->Option[2];
	lpErtelInfo->Option[2].OptionLevel = Ertel->Level[2];

	lpErtelInfo->Option[3].OptionID = Ertel->Option[3];
	lpErtelInfo->Option[3].OptionLevel = Ertel->Level[3];

	lpErtelInfo->Option[4].OptionID = Ertel->Option[4];
	lpErtelInfo->Option[4].OptionLevel = Ertel->Level[4];
}

void CElementalSystem::SendElementalItems(LPOBJ lpObj,int Type)
{
	BYTE cBUFFER[2048] = {0};

	PMSG_SEND_ERTELINFO* lpMsg = (PMSG_SEND_ERTELINFO*)&cBUFFER[0];
	PMSG_ERTELINFO* lpMsgBody = (PMSG_ERTELINFO*)&cBUFFER[sizeof(PMSG_SEND_ERTELINFO)];

	PHeadSubSetW((LPBYTE)lpMsg,0xEE,0x01,sizeof(PMSG_SEND_ERTELINFO));

	int lOfs = sizeof(PMSG_SEND_ERTELINFO);
	ERTEL_USER* lpErtelList = NULL;

	if( Type == ERTEL_INVENTORY )
	{
		lpErtelList = lpObj->pErtelList1;
	}
	else if( Type == ERTEL_WAREHOUSE )
	{
		lpErtelList = lpObj->pErtelList2;
	}

	if( lpErtelList == NULL )
		return;

	int count = 0;

	for(int i = 0; i < MAX_ERTEL; i++ )	{
		if( lpErtelList[i].btType == (BYTE)-1 )
			continue;

		BuildErtelInfo(&lpMsgBody[count],&lpErtelList[i],i,Type);

		count++;
		lOfs += sizeof(PMSG_ERTELINFO);
	}

	lpMsg->btType = Type;
	lpMsg->btCount = count;
	lpMsg->h.sizeL = SET_NUMBERL(lOfs);
	lpMsg->h.sizeH = SET_NUMBERH(lOfs);

	if( count > 0 )
		lpMsg->btResult = TRUE;

	DataSend(lpObj->m_Index,(LPBYTE)lpMsg,lOfs);
}

void CElementalSystem::SendShopElementalItem(int aSourceIndex,int aTargetIndex)
{
	BYTE cBUFFER[2048] = {0};

	PMSG_SEND_ERTELINFO* lpMsg = (PMSG_SEND_ERTELINFO*)&cBUFFER[0];
	PMSG_ERTELINFO* lpMsgBody = (PMSG_ERTELINFO*)&cBUFFER[sizeof(PMSG_SEND_ERTELINFO)];

	PHeadSubSetW((LPBYTE)lpMsg,0xEE,0x01,sizeof(PMSG_SEND_ERTELINFO));

	int lOfs = sizeof(PMSG_SEND_ERTELINFO);
	ERTEL_USER* lpErtelList = NULL;

	LPOBJ lpShopObj = &gObj[aSourceIndex];

	int count = 0;

	for(int n = MAIN_INVENTORY_SIZE; n < (INVENTORY_SIZE-1); n++)	{
		if( !IsPentagram(lpShopObj->pInventory[n].m_Type) )
			continue;

		for(int i = 0; i < 5; i++)	{
			if( lpShopObj->pInventory[n].m_SocketOption[i] == (BYTE)-1 ||
				lpShopObj->pInventory[n].m_SocketOption[i] == (BYTE)-2 )
				continue;

			int ErtelID = lpShopObj->pInventory[n].m_SocketOption[i];

			BuildErtelInfo(&lpMsgBody[count],&lpShopObj->pErtelList1[ErtelID],ErtelID,ERTEL_STORE);
			count++;
			lOfs += sizeof(PMSG_ERTELINFO);
		}
	}

	if( count > 0 )
	{
		lpMsg->btType = ERTEL_STORE;
		lpMsg->btCount = count;
		lpMsg->h.sizeL = SET_NUMBERL(lOfs);
		lpMsg->h.sizeH = SET_NUMBERH(lOfs);
		lpMsg->btResult = TRUE;

		DataSend(aTargetIndex,(LPBYTE)lpMsg,lOfs);
	}
}

void CElementalSystem::SendTradeElementalItem(int aSourceIndex,int aTargetIndex)
{
	BYTE cBUFFER[2048] = {0};

	PMSG_SEND_ERTELINFO* lpMsg = (PMSG_SEND_ERTELINFO*)&cBUFFER[0];
	PMSG_ERTELINFO* lpMsgBody = (PMSG_ERTELINFO*)&cBUFFER[sizeof(PMSG_SEND_ERTELINFO)];

	PHeadSubSetW((LPBYTE)lpMsg,0xEE,0x01,sizeof(PMSG_SEND_ERTELINFO));

	int lOfs = sizeof(PMSG_SEND_ERTELINFO);
	ERTEL_USER* lpErtelList = NULL;

	LPOBJ lpTradeObj = &gObj[aSourceIndex];

	int count = 0;

	for(int n = 0; n < TRADE_BOX_SIZE; n++)	{
		if( !IsPentagram(lpTradeObj->Trade[n].m_Type) )
			continue;

		for(int i = 0; i < 5; i++)	{
			if( lpTradeObj->Trade[n].m_SocketOption[i] == (BYTE)-1 ||
				lpTradeObj->Trade[n].m_SocketOption[i] == (BYTE)-2 )
				continue;

			int ErtelID = lpTradeObj->Trade[n].m_SocketOption[i];

			BuildErtelInfo(&lpMsgBody[count],&lpTradeObj->pErtelList1[ErtelID],ErtelID,ERTEL_TRADE);
			count++;
			lOfs += sizeof(PMSG_ERTELINFO);
		}
	}

	lpMsg->btType = ERTEL_TRADE;
	lpMsg->btCount = count;
	lpMsg->h.sizeL = SET_NUMBERL(lOfs);
	lpMsg->h.sizeH = SET_NUMBERH(lOfs);
	lpMsg->btResult = TRUE;

	DataSend(aTargetIndex,(LPBYTE)lpMsg,lOfs);
}

void CElementalSystem::MoveErtel(LPOBJ lpObj,CItem* TargetItem,BYTE tFlag,unsigned char* siteminfo)
{
	if( !IsPentagram(TargetItem->m_Type) )
		return;

	ERTEL_USER* lpTargetErtel;
	ERTEL_USER* lpSourceErtel;

	if( tFlag == ERTEL_INVENTORY )	{
		lpTargetErtel = lpObj->pErtelList1;
		lpSourceErtel = lpObj->pErtelList2;
	}
	else	{
		lpTargetErtel = lpObj->pErtelList2;
		lpSourceErtel = lpObj->pErtelList1;
	}


	for(int i = 0; i < 5; i++)	{

		if( TargetItem->m_SocketOption[i] == (BYTE)-1 ||
			TargetItem->m_SocketOption[i] == (BYTE)-2 )
		{
			continue;
		}

		int ErtelID = TargetItem->m_SocketOption[i];

		for(int n = 0; n < MAX_ERTEL; n++)	{
			if( lpTargetErtel[n].btType != (BYTE)-1 )
				continue;

			TargetItem->m_SocketOption[i] = n;
			lpTargetErtel[n] = lpSourceErtel[ErtelID];
			lpSourceErtel[ErtelID].Clear();
			break;
		}

		siteminfo[7+i] = TargetItem->m_SocketOption[i];
	}

	SendElementalItems(lpObj,ERTEL_INVENTORY);
	SendElementalItems(lpObj,ERTEL_WAREHOUSE);
}

void CElementalSystem::PShopErtel(LPOBJ lpShopObj,LPOBJ lpObj,CItem* TargetItem)
{
	if( !IsPentagram(TargetItem->m_Type) )
		return;

	ERTEL_USER* lpTargetErtel;
	ERTEL_USER* lpSourceErtel;

	lpTargetErtel = lpObj->pErtelList1;
	lpSourceErtel = lpShopObj->pErtelList1;

	TargetItem->m_Durability -= 1;

	for(int i = 0; i < 5; i++)	{

		if( TargetItem->m_SocketOption[i] == (BYTE)-1 ||
			TargetItem->m_SocketOption[i] == (BYTE)-2 )
		{
			continue;
		}

		int ErtelID = TargetItem->m_SocketOption[i];

		for(int n = 0; n < MAX_ERTEL; n++)	{
			if( lpTargetErtel[n].btType != (BYTE)-1 )
				continue;

			TargetItem->m_SocketOption[i] = n;
			lpTargetErtel[n] = lpSourceErtel[ErtelID];
			lpSourceErtel[ErtelID].Clear();
			break;
		}
	}

	SendElementalItems(lpShopObj,ERTEL_INVENTORY);
	SendElementalItems(lpObj,ERTEL_INVENTORY);
}

void CElementalSystem::TradeErtel(LPOBJ lpTradeObj,LPOBJ lpObj,CItem* TargetItem)
{
	if( !IsPentagram(TargetItem->m_Type) )
		return;

	ERTEL_USER* lpTargetErtel;
	ERTEL_USER* lpSourceErtel;

	lpTargetErtel = lpObj->pErtelList1;
	lpSourceErtel = lpTradeObj->pErtelList1;

	TargetItem->m_Durability -= 1;

	for(int i = 0; i < 5; i++)	{

		if( TargetItem->m_SocketOption[i] == (BYTE)-1 ||
			TargetItem->m_SocketOption[i] == (BYTE)-2 )
		{
			continue;
		}

		int ErtelID = TargetItem->m_SocketOption[i];

		for(int n = 0; n < MAX_ERTEL; n++)	{
			if( lpTargetErtel[n].btType != (BYTE)-1 )
				continue;

			TargetItem->m_SocketOption[i] = n;
			lpTargetErtel[n] = lpSourceErtel[ErtelID];
			lpSourceErtel[ErtelID].Clear();
			break;
		}
	}
}

void CElementalSystem::ClearErtel(LPOBJ lpObj,CItem* TargetItem)
{
	if( !IsPentagram(TargetItem->m_Type) )
		return;


	ERTEL_USER* lpTargetErtel;
	lpTargetErtel = lpObj->pErtelList1;

	for(int i = 0; i < 5; i++)	{

		if( TargetItem->m_SocketOption[i] == (BYTE)-1 ||
			TargetItem->m_SocketOption[i] == (BYTE)-2 )
		{
			continue;
		}

		RemoveErtel(lpObj,TargetItem->m_SocketOption[i],ERTEL_INVENTORY);
	}

	SendElementalItems(lpObj,ERTEL_INVENTORY);
}

BYTE CElementalSystem::GetErtelOption(int iErtel,BYTE btRank)
{
	m_OptionRate.InitPool();

	int count = 0;

	for(int i = 0; i < MAX_ERTEL_OPTION; i++)	{
		if( m_ErtelOption[i].btId == (BYTE)-1 )
			continue;

		if( m_ErtelOption[i].ItemID != iErtel )
			continue;

		if( m_ErtelOption[i].btRank != btRank )
			continue;

		m_OptionRate.AddValue(i,m_ErtelOption[i].OptionRate);
		count++;
	}

	if( count == 0 )
		return (BYTE)-1;

	int iErtelId = m_OptionRate.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);

return m_ErtelOption[iErtelId].btOption;
}

void CElementalSystem::CGCombineButtonClick(PMSG_ERTEL_COMBINE* lpMsg,int aIndex)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->ChaosLock == TRUE )
	{
		LogAdd("[%s][%s] Already Used Chaos",
			lpObj->AccountID, lpObj->Name);

		pMsg.Result = CB_ALREADY_OPEN;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);

		GCServerMsgStringSend(lMsg.Get(MSGGET(2, 198)), lpObj->m_Index, 1);
		return;
	}

	if ( lpObj->m_bPShopOpen == true )
	{
		LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
			lpObj->AccountID, lpObj->Name);

		pMsg.Result = CB_PSHOP_OPEN;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);

		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);
		return;
	}

	lpObj->bIsChaosMixCompleted = true;
	int iMixType = lpMsg->MixId;

	switch ( iMixType )
	{
	case ELEMENTAL_MIX_MITHRIL:	MithrilMix(lpObj);	break;
	case ELEMENTAL_MIX_ELIXIR:	ElixirMix(lpObj);	break;
	case ELEMENTAL_MIX_ERRTEL:	ErrtelMix(lpObj);	break;
	case ELEMENTAL_MIX_DECOMPOSE1:
	case ELEMENTAL_MIX_DECOMPOSE2:
	case ELEMENTAL_MIX_DECOMPOSE3:
	case ELEMENTAL_MIX_DECOMPOSE4:
	case ELEMENTAL_MIX_DECOMPOSE5:
	case ELEMENTAL_MIX_DECOMPOSE6:
	case ELEMENTAL_MIX_DECOMPOSE7:
	case ELEMENTAL_MIX_DECOMPOSE8:
	case ELEMENTAL_MIX_DECOMPOSE9:
	case ELEMENTAL_MIX_DECOMPOSE10:
	case ELEMENTAL_MIX_DECOMPOSE11:
	case ELEMENTAL_MIX_DECOMPOSE12:
	case ELEMENTAL_MIX_DECOMPOSE13:
	case ELEMENTAL_MIX_DECOMPOSE14:
	case ELEMENTAL_MIX_DECOMPOSE15:
		PentagramMix(lpObj);	break;
	default:
		LogAddTD("[Elemental System] [%s][%s] Undefine chaosmix type detect %d",lpObj->AccountID, lpObj->Name, iMixType);
		break;
	}
}

void CElementalSystem::CGUpgradeButtonClick(PMSG_ERTEL_LEVELUP* lpMsg,int aIndex)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_IMPROPER_MATERIALS;

	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->ChaosLock == TRUE )
	{
		LogAdd("[%s][%s] Already Used Chaos",
			lpObj->AccountID, lpObj->Name);

		pMsg.Result = CB_ALREADY_OPEN;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);

		GCServerMsgStringSend(lMsg.Get(MSGGET(2, 198)), lpObj->m_Index, 1);
		return;
	}

	if ( lpObj->m_bPShopOpen == true )
	{
		LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
			lpObj->AccountID, lpObj->Name);

		pMsg.Result = CB_PSHOP_OPEN;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);

		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);
		return;
	}

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( lpObj->pChaosBox[n].m_Type == ITEMGET(12,221) ||
				lpObj->pChaosBox[n].m_Type == ITEMGET(12,231) ||
				lpObj->pChaosBox[n].m_Type == ITEMGET(12,241) ||
				lpObj->pChaosBox[n].m_Type == ITEMGET(12,251) ||
				lpObj->pChaosBox[n].m_Type == ITEMGET(12,261) )
			{
				int iActiveRank = lpObj->pChaosBox[n].m_SocketBonus/16;
				
				if( iActiveRank  <= 0 || iActiveRank > 5  )
				{
					LogAdd("[%s][%s] Ertel rank info error !",lpObj->AccountID, lpObj->Name);

					DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
					return;
				}

				int iUpdateLevel = lpObj->pChaosBox[n].m_SocketOption[iActiveRank-1]/16;

				if( lpMsg->btType == 0 )
				{
					if( (iUpdateLevel+1) != lpMsg->btLevel )
					{
						LogAdd("[%s][%s] Ertel level info error !",lpObj->AccountID, lpObj->Name);

						DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
						return;
					}
				}
				else if( lpMsg->btType == 1 )
				{
					if( iUpdateLevel < 7 || iActiveRank == 5 || lpMsg->btLevel < 1 || lpMsg->btLevel>5 )
					{
						LogAdd("[%s][%s] Ertel rank up info error !",lpObj->AccountID, lpObj->Name);

						DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
						return;
					}
				}
			}
		}
	}

	lpObj->bIsChaosMixCompleted = true;

	switch( lpMsg->btType )
	{
	case 0:	ErrtelLevelUp(lpObj);	break;
	case 1:	ErrtelRankUp(lpObj,lpMsg->btLevel);	break;
	}
}

void CElementalSystem::MithrilMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = ELEMENTAL_IMPROPER;

	int iBounchOfMithrilCount = 0;
	int iMithrilType = -1;
	int iChaosCount = 0;
	int iOtherItemCount = 0;


	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,148) )
			{
				iMithrilType = lpObj->pChaosBox[n].m_SocketBonus;
				iBounchOfMithrilCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				iChaosCount++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iBounchOfMithrilCount != 1 ||
		iChaosCount != 1 ||
		iOtherItemCount != 0 )

	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	g_ChaosBox.LogChaosItem(lpObj, "Mithril Mix");
	LogAddTD("[Elemental System] Mithril Mix Start");

	lpObj->ChaosSuccessRate = 80;
	int nChaosNeedMoney = 100000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = ELEMENTAL_NOZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int type = ITEMGET(12,145);

		BYTE SocketBonus = iMithrilType;
		BYTE SocketOption[5] = {-1};

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, type , 0, 0, 0, 0, 0, -1, 0, 0,SocketBonus,SocketOption);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Elemental System][Mithril Mix] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);

	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		g_ChaosBox.ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);

		pMsg.Result = ELEMENTAL_FAIL1;
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Elemental System][Mithril Mix] [%s][%s] CBMix Fail %d Money : %d-%d ",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);

		lpObj->ChaosLock = FALSE;
	}
}

void CElementalSystem::ElixirMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = ELEMENTAL_IMPROPER;

	int iBounchOfElixirCount = 0;
	int iChaosCount = 0;
	int iOtherItemCount = 0;


	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,149) )
			{
				iBounchOfElixirCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,15) )
			{
				iChaosCount++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iBounchOfElixirCount != 1 ||
		iChaosCount != 1 ||
		iOtherItemCount != 0 )

	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	g_ChaosBox.LogChaosItem(lpObj, "Elixir Mix");
	LogAddTD("[Elemental System] Elixir Mix Start");

	lpObj->ChaosSuccessRate = 80;
	int nChaosNeedMoney = 100000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = ELEMENTAL_NOZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int type = ITEMGET(12,147);

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, type , 0, 0, 0, 0, 0, -1, 0, 0);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Elemental System][Elixir Mix] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);

	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		g_ChaosBox.ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);

		pMsg.Result = ELEMENTAL_FAIL1;
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Elemental System][Elixir Mix] [%s][%s] CBMix Fail %d Money : %d-%d ",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);

		lpObj->ChaosLock = FALSE;
	}
}

void CElementalSystem::ErrtelMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = ELEMENTAL_IMPROPER;

	int iMithrilCount = 0;
	int iMithrilType = -1;
	int iElixirCount = 0;
	int iBlessCount = 0;
	int iOtherItemCount = 0;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,145) )
			{
				iMithrilType = lpObj->pChaosBox[n].m_SocketBonus;
				iMithrilCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,147) )
			{
				iElixirCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) )
			{
				iBlessCount++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iMithrilCount != 1 ||
		iElixirCount != 1 ||
		iBlessCount != 1 ||
		iOtherItemCount != 0 )

	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	g_ChaosBox.LogChaosItem(lpObj, "Elixir Mix");
	LogAddTD("[Elemental System] Elixir Mix Start");

	lpObj->ChaosSuccessRate = 50;
	int nChaosNeedMoney = 100000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = ELEMENTAL_NOZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		int type = ITEMGET(12,221+(rand()%5*10));

		BYTE SocketBonus = iMithrilType | 0x10;
		BYTE SocketOption[5];

		SocketOption[0] = GetErtelOption(type,1);
		SocketOption[1] = (BYTE)-1;
		SocketOption[2] = (BYTE)-1;
		SocketOption[3] = (BYTE)-1;
		SocketOption[4] = (BYTE)-1;

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, type , 0, 0, 0, 0, 0, -1, 0, 0,SocketBonus,SocketOption);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Elemental System][Elixir Mix] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		g_ChaosBox.ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);

		pMsg.Result = ELEMENTAL_FAIL1;
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Elemental System][Elixir Mix] [%s][%s] CBMix Fail %d Money : %d-%d ",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);

		lpObj->ChaosLock = FALSE;
	}
}


void CElementalSystem::ErrtelLevelUp(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = ELEMENTAL_IMPROPER;

	int iErtelCount = 0;
	CItem* Ertel = NULL;
	int iJewelCombineFrameCount = 0;
	int iBlessCount = 0;
	int iOtherItemCount = 0;
	int iPentagramLuckyCharm = 0;
	int iPentagramLuckyCharmRate = 0;
	int iPentagramChaosCharm = 0;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,221) ||
				 lpObj->pChaosBox[n].m_Type == ITEMGET(12,231) ||
				 lpObj->pChaosBox[n].m_Type == ITEMGET(12,241) ||
				 lpObj->pChaosBox[n].m_Type == ITEMGET(12,251) ||
				 lpObj->pChaosBox[n].m_Type == ITEMGET(12,261) )
			{
				Ertel = &lpObj->pChaosBox[n];
				iErtelCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,150) )
			{
				iJewelCombineFrameCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) )
			{
				iBlessCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14, 189) )
			{
				iPentagramLuckyCharm++;
				iPentagramLuckyCharmRate = ItemAttribute[lpObj->pChaosBox[n].m_Type].Level; 
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14, 190) )
			{
				iPentagramChaosCharm++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iErtelCount != 1 ||
		iJewelCombineFrameCount != 1 ||
		iBlessCount != 1 ||
		iOtherItemCount != 0 )

	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	if ( iPentagramChaosCharm > 1 )
	{
		PMSG_CHAOSMIXRESULT pResult;
		PHeadSetB((LPBYTE)&pResult, 0x86, sizeof(pResult));
		pResult.Result = 7;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if ( iPentagramLuckyCharm > 0 && iPentagramChaosCharm > 0 )
	{
		PMSG_CHAOSMIXRESULT pResult;
		PHeadSetB((LPBYTE)&pResult, 0x86, sizeof(pResult));
		pResult.Result = 7;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	g_ChaosBox.LogChaosItem(lpObj, "Ertel LevelUp Mix");
	LogAddTD("[Elemental System] Ertel LevelUp Start");

	lpObj->ChaosSuccessRate = 50;
	int nChaosNeedMoney = 400000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = ELEMENTAL_NOZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);
	lpObj->ChaosSuccessRate += iPentagramLuckyCharmRate;
	
//	LogAddC(2, "[DEBUG] Rate1: %d", iPentagramLuckyCharmRate);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		if( iPentagramChaosCharm > 0 )
		{
			g_ChaosBox.ChaosBoxSpecialItemDown(lpObj);
			GCUserChaosBoxSend(lpObj, 0);
			DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		}


		pMsg.Result = CB_SUCCESS;

		int iRank = Ertel->m_SocketBonus/16;
		int iLevel = Ertel->m_SocketOption[iRank-1]/16;

		if( iLevel++ >= 10 )
			Ertel->m_SocketBonus += 16;

		Ertel->m_SocketOption[iRank-1] += 16;

		CItem Item;

		Item.m_Level = Ertel->m_Level;
		Item.m_Durability = Ertel->m_Durability;
		Item.m_JewelOfHarmonyOption = Ertel->m_JewelOfHarmonyOption;
		Item.m_bLOCKED = Ertel->m_bLOCKED;

		BYTE SocketBonus = Ertel->m_SocketBonus;
		BYTE SocketOptions[MAX_SOCKET_COUNT];

		for(int i = 0; i < 5; i++)	{
			SocketOptions[i] = Ertel->m_SocketOption[i];
		}

		Item.Convert(Ertel->m_Type, Ertel->m_Option1, Ertel->m_Option2,
			Ertel->m_Option3, Ertel->m_NewOption, Ertel->m_SetOption, Ertel->m_ItemOptionEx, SocketOptions, SocketBonus, CURRENT_DB_VERSION);

		Item.m_Number = Ertel->m_Number;

		ItemByteConvert(pMsg.ItemInfo, Item);
		g_ChaosBox.ChaosBoxInit(lpObj);
		::gObjChaosBoxInsertItemPos(lpObj->m_Index, Item, 0, -1);
		gObjChaosItemSet(lpObj->m_Index, 0, 1);

		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
#if( __4GAMERS__ == 1 )
		g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Elemental System][Ertel LevelUp] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);
	}
	else
	{
#if( __4GAMERS__ == 1 )
		g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		g_ChaosBox.ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);

		pMsg.Result = ELEMENTAL_FAIL2;
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Elemental System][Ertel LevelUp] [%s][%s] CBMix Fail %d Money : %d-%d ",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);
	}
	lpObj->ChaosLock = FALSE;
}

void CElementalSystem::ErrtelRankUp(LPOBJ lpObj,BYTE btRank)
{
	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = ELEMENTAL_IMPROPER;

	int iErtelCount = 0;
	CItem* Ertel = NULL;
	int iSpiritPowderCount = 0;
	int iBlessCount = 0;
	int iSoulCount = 0;
	int iSetItemCount = 0;
	int iReqCount = btRank-1;
	int iOtherItemCount = 0;
	int iPentagramLuckyCharm = 0;
	int iPentagramLuckyCharmRate = 0;
	int iPentagramChaosCharm = 0;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if( lpObj->pChaosBox[n].IsSetItem() &&
				lpObj->pChaosBox[n].m_Level >= 7 &&
				lpObj->pChaosBox[n].m_Option3 >= 1 )
			{
				iSetItemCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,221) ||
				 lpObj->pChaosBox[n].m_Type == ITEMGET(12,231) ||
				 lpObj->pChaosBox[n].m_Type == ITEMGET(12,241) ||
				 lpObj->pChaosBox[n].m_Type == ITEMGET(12,251) ||
				 lpObj->pChaosBox[n].m_Type == ITEMGET(12,261) )
			{
				Ertel = &lpObj->pChaosBox[n];
				iErtelCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(12,151) )
			{
				iSpiritPowderCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,13) )
			{
				iBlessCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,14) )
			{
				iSoulCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14, 189) )
			{
				iPentagramLuckyCharm++;
				iPentagramLuckyCharmRate = ItemAttribute[lpObj->pChaosBox[n].m_Type].Level; 
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14, 190) )
			{
				iPentagramChaosCharm++;
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iErtelCount != 1 ||
		iSpiritPowderCount != iReqCount ||
		iBlessCount != iReqCount ||
		iSoulCount != iReqCount ||
		iSetItemCount != 1 ||
		iOtherItemCount != 0 )

	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if ( iPentagramChaosCharm > 1 )
	{
		PMSG_CHAOSMIXRESULT pResult;
		PHeadSetB((LPBYTE)&pResult, 0x86, sizeof(pResult));
		pResult.Result = 7;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	if ( iPentagramLuckyCharm > 0 && iPentagramChaosCharm > 0 )
	{
		PMSG_CHAOSMIXRESULT pResult;
		PHeadSetB((LPBYTE)&pResult, 0x86, sizeof(pResult));
		pResult.Result = 7;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	g_ChaosBox.LogChaosItem(lpObj, "Ertel RankUp Mix");
	LogAddTD("[Elemental System] Ertel RankUp Start");

	lpObj->ChaosSuccessRate = 50;
	int nChaosNeedMoney = 400000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = ELEMENTAL_NOZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);
	lpObj->ChaosSuccessRate += iPentagramLuckyCharmRate;
//	LogAddC(2, "[DEBUG] Rate2: %d", iPentagramLuckyCharmRate);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		if( iPentagramChaosCharm > 0 )
		{
			g_ChaosBox.ChaosBoxSpecialItemDown(lpObj);
			GCUserChaosBoxSend(lpObj, 0);
			DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		}

		pMsg.Result = CB_SUCCESS;

		Ertel->m_SocketOption[btRank-1] = GetErtelOption(Ertel->m_Type,btRank);

		CItem Item;

		Item.m_Level = Ertel->m_Level;
		Item.m_Durability = Ertel->m_Durability;
		Item.m_JewelOfHarmonyOption = Ertel->m_JewelOfHarmonyOption;
		Item.m_bLOCKED = Ertel->m_bLOCKED;

		BYTE SocketBonus = Ertel->m_SocketBonus;
		BYTE SocketOptions[MAX_SOCKET_COUNT];

		for(int i = 0; i < 5; i++)	{
			SocketOptions[i] = Ertel->m_SocketOption[i];
		}

		Item.Convert(Ertel->m_Type, Ertel->m_Option1, Ertel->m_Option2,
			Ertel->m_Option3, Ertel->m_NewOption, Ertel->m_SetOption, Ertel->m_ItemOptionEx, SocketOptions, SocketBonus, CURRENT_DB_VERSION);

		Item.m_Number = Ertel->m_Number;

		ItemByteConvert(pMsg.ItemInfo, Item);
		g_ChaosBox.ChaosBoxInit(lpObj);
		::gObjChaosBoxInsertItemPos(lpObj->m_Index, Item, 0, -1);
		gObjChaosItemSet(lpObj->m_Index, 0, 1);

		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Elemental System][Ertel RankUp] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		g_ChaosBox.ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);

		pMsg.Result = ELEMENTAL_FAIL2;
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Elemental System][Ertel RankUp] [%s][%s] CBMix Fail %d Money : %d-%d ",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);
	}
	lpObj->ChaosLock = FALSE;
}

void CElementalSystem::PentagramMix(LPOBJ lpObj)
{
	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = ELEMENTAL_IMPROPER;

	int iPentagramCount = 0;
	int iHarmonyCount = 0;
	int iOtherItemCount = 0;
	int iPentagramLuckyCharm = 0;
	int iPentagramLuckyCharmRate = 0;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			if ( lpObj->pChaosBox[n].m_Type >= ITEMGET(12,200) &&
				 lpObj->pChaosBox[n].m_Type <= ITEMGET(12,214) )
			{
				iPentagramCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14,42) )
			{
				iHarmonyCount++;
			}
			else if ( lpObj->pChaosBox[n].m_Type == ITEMGET(14, 189) )
			{
				iPentagramLuckyCharm++;
				iPentagramLuckyCharmRate = ItemAttribute[lpObj->pChaosBox[n].m_Type].Level; 
			}
			else
			{
				iOtherItemCount++;
			}
		}
	}

	if( iPentagramCount != 1 ||
		iHarmonyCount != 1 ||
		iOtherItemCount != 0 )

	{
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;

		return;
	}

	g_ChaosBox.LogChaosItem(lpObj, "Pentagram Refine");
	LogAddTD("[Elemental System] Pentagram Refine Start");

	lpObj->ChaosSuccessRate = 50;
	lpObj->ChaosSuccessRate += iPentagramLuckyCharmRate;
//	LogAddC(2, "[DEBUG] Rate3: %d", iPentagramLuckyCharmRate);

	int nChaosNeedMoney = 100000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index) / (__int64)100);

	if ( iChaosTaxMoney < 0 )
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if ( nChaosNeedMoney < 0 )
	{
		nChaosNeedMoney = 0;
	}

	if ( lpObj->Money < nChaosNeedMoney )
	{
		pMsg.Result = ELEMENTAL_NOZEN;
		
		DataSend(lpObj->m_Index, (BYTE *)&pMsg, pMsg.h.size);
		lpObj->ChaosLock = FALSE;
		return;
	}

	lpObj->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GCMoneySend(lpObj->m_Index, lpObj->Money);

	if ( (rand()%100) < lpObj->ChaosSuccessRate )
	{
		BYTE SocketBonus = -1;
		BYTE SocketOption[5] = {-1};

		int _r = rand()%5;

		int Type;

		switch( _r )
		{
		case 0:
			Type = ITEMGET(12,144);
			SocketBonus = 1+rand()%4;
			break;
		case 1:
			Type = ITEMGET(12,145);
			SocketBonus = 1+rand()%4;
			break;
		case 2:
			Type = ITEMGET(12,146);
			break;
		case 3:
			Type = ITEMGET(12,147);
			break;
		case 4:
			Type = ITEMGET(12,150);
			break;
		}

		ItemSerialCreateSend(lpObj->m_Index, -1, 0, 0, Type , 0, 0, 0, 0, 0, -1, 0, 0,SocketBonus,SocketOption);
		::gObjInventoryCommit(lpObj->m_Index);
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, true);
#endif
		LogAddTD("[Elemental System][Pentagram Refine] [%s][%s] CBMix Success %d Money : %d-%d",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);
	}
	else
	{
#if( __4GAMERS__ == 1 )
			g_Achievements.GD_UpdateMixData(lpObj, false);
#endif
		g_ChaosBox.ChaosBoxInit(lpObj);
		::GCUserChaosBoxSend(lpObj, 0);

		pMsg.Result = ELEMENTAL_FAIL1;
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		LogAddTD("[Elemental System][Pentagram Refine] [%s][%s] CBMix Fail %d Money : %d-%d ",
			lpObj->AccountID, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->Money, nChaosNeedMoney);

		lpObj->ChaosLock = FALSE;
	}
}
