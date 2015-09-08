// LifeStone.cpp: implementation of the CLifeStone class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "logproc.h"
#include "LifeStone.h"
#include "GameMain.h"
#include "CastleSiege.h"


#if (GS_CASTLE==1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLifeStone g_CsNPC_LifeStone;

CLifeStone::CLifeStone()
{

}

CLifeStone::~CLifeStone()
{

}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

int CLifeStone::CreateLifeStone(int iIndex)
{
	LPOBJ lpObj = &gObj[iIndex];
	int iMonsterIndex = -1;
	unsigned char cX = lpObj->X;
	unsigned char cY = lpObj->Y;

	if(g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
	{
		MsgOutput(iIndex,lMsg.Get(MSGGET(6,109)));
		return false;
	}

	if(lpObj->GuildStatus != G_MASTER)
		return false;

	if(lpObj->m_btCsJoinSide < 2)
	{
		MsgOutput(iIndex,lMsg.Get(MSGGET(6,105)));
		return false;
	}

	if(lpObj->lpGuild->lpLifeStone != NULL)
	{
		MsgOutput(iIndex,lMsg.Get(MSGGET(6,106)));
		return false;
	}

	if(cX > 150 && cX < 210 && cY > 175 && cY < 230)
	{
		MsgOutput(iIndex,lMsg.Get(MSGGET(6,108)));
		return false;
	}

	unsigned char btMapAttr = MapC[lpObj->MapNumber].GetAttr(cX,cY);

	if(gObj[iIndex].MapNumber != MAP_INDEX_CASTLESIEGE)
	{
		MsgOutput(iIndex,lMsg.Get(MSGGET(6,91)));
		return false;
	}

	iMonsterIndex = gObjAddMonster(lpObj->MapNumber);

	if(iMonsterIndex >= 0)
	{
		MONSTER_ATTRIBUTE * lpMATTR = gMAttr.GetAttr(278);

		if(lpMATTR == NULL)
		{
			gObjDel(iMonsterIndex);
			return false;
		}

		gObjSetMonster(iMonsterIndex,278);
		gObj[iMonsterIndex].Live = TRUE;
		gObj[iMonsterIndex].Life = (float)lpMATTR->m_Hp;
		gObj[iMonsterIndex].MaxLife = (float)lpMATTR->m_Hp;
		gObj[iMonsterIndex].m_PosNum = -1;
		gObj[iMonsterIndex].X = cX;
		gObj[iMonsterIndex].Y = cY;
		gObj[iMonsterIndex].MTX = cX;
		gObj[iMonsterIndex].MTY = cY;
		gObj[iMonsterIndex].TX = cX;
		gObj[iMonsterIndex].TY = cY;
		gObj[iMonsterIndex].m_OldX = cX;
		gObj[iMonsterIndex].m_OldY = cY;
		gObj[iMonsterIndex].StartX = cX;
		gObj[iMonsterIndex].StartY = cY;
		gObj[iMonsterIndex].MapNumber = lpObj->MapNumber;
		gObj[iMonsterIndex].m_MoveRange = 0;
		gObj[iMonsterIndex].Level = lpMATTR->m_Level;
		gObj[iMonsterIndex].Type = OBJ_MONSTER;
		gObj[iMonsterIndex].MaxRegenTime = 1000;
		gObj[iMonsterIndex].Dir = 1;
		gObj[iMonsterIndex].RegenTime = GetTickCount();
		gObj[iMonsterIndex].m_Attribute = 0;
		gObj[iMonsterIndex].DieRegen = FALSE;
		gObj[iMonsterIndex].m_btCsNpcType = 3;
		gObj[iMonsterIndex].m_btCsJoinSide = lpObj->m_btCsJoinSide;
		gObj[iMonsterIndex].lpGuild = lpObj->lpGuild;
		gObj[iMonsterIndex].m_btCreationState = 0;

		lpObj->lpGuild->lpLifeStone = &gObj[iMonsterIndex];

		MsgOutput(iIndex,lMsg.Get(MSGGET(6,110)));

		LogAddTD("[CastleSiege] LifeStone is created - [%s] [%s][%s] (Map:%d)(X:%d, Y:%d)",
			lpObj->lpGuild->Name,lpObj->AccountID,lpObj->Name,lpObj->MapNumber,cX,cY);

		lpObj->m_btLifeStoneCount++;

	}
	else
	{
		MsgOutput(iIndex,lMsg.Get(MSGGET(6,111)));
		return false;
	}

	return true;
}

int CLifeStone::DeleteLifeStone(int iIndex)
{
	if(iIndex < 0 || iIndex > OBJMAX)
	{
		return false;
	}

	LPOBJ lpLifeStone = &gObj[iIndex];

	if(lpLifeStone->lpGuild != NULL)
	{
		LogAddTD("[CastleSiege] LifeStone is broken - [%s]",
			lpLifeStone->lpGuild->Name);
		lpLifeStone->lpGuild->lpLifeStone = NULL;
	}
	return true;
}

int CLifeStone::SetReSpawnUserXY(int iUserIndex)
{
	if(gObjIsConnected(iUserIndex) == FALSE)
		return false;

	LPOBJ lpUser = &gObj[iUserIndex];

	if(lpUser->MapNumber != MAP_INDEX_CASTLESIEGE)
		return false;

	if(lpUser->lpGuild == NULL)
		return false;

	BYTE btCsJoinSide = lpUser->m_btCsJoinSide;

	if(lpUser->lpGuild->lpLifeStone == NULL)
		return false;

	LPOBJ lpLifeStone = lpUser->lpGuild->lpLifeStone;
	lpUser->MapNumber = MAP_INDEX_CASTLESIEGE;
	lpUser->X = lpLifeStone->X;
	lpUser->Y = lpLifeStone->Y;
	return true;
}

void CLifeStone::LifeStoneAct(int iIndex)
{
	if(gObjIsConnected(iIndex) == FALSE)
		return;

	LPOBJ lpObj = &gObj[iIndex];
	lpObj->m_iCreatedActivationTime++;

	BYTE btCreationState = lpObj->m_btCreationState;

	if(lpObj->m_iCreatedActivationTime < 60)
	{
		lpObj->m_btCreationState = lpObj->m_iCreatedActivationTime / 12;
	}
	else
	{
		lpObj->m_btCreationState = 5;
	}

	if(btCreationState != lpObj->m_btCreationState)
	{
		GCSendObjectCreationState(iIndex);
	}

	if(lpObj->m_btCreationState < 5)
		return;

	if(lpObj->VPCount < 1)
		return;

	int tObjNum = -1;

	for(int i = 0; i < MAX_VIEWPORT; i++)
	{
		tObjNum = lpObj->VpPlayer[i].number;

		if(tObjNum >= 0 
			&& gObj[tObjNum].Type == OBJ_USER
			&& gObj[tObjNum].Live != FALSE 
			&& gObj[tObjNum].m_btCsJoinSide == lpObj->m_btCsJoinSide
			&& abs(lpObj->Y - gObj[tObjNum].Y) <= 3
			&& abs(lpObj->X - gObj[tObjNum].X) <= 3)
		{
			int bLifeChange = FALSE;
			int bManaChange = FALSE;
			int bBpChange = FALSE;

			if(gObj[tObjNum].Life < gObj[tObjNum].MaxLife + gObj[tObjNum].AddLife)
			{
				gObj[tObjNum].Life += (gObj[tObjNum].MaxLife + gObj[tObjNum].AddLife) / 100.0f;// + gObj[tObjNum].Life;

				if(gObj[tObjNum].Life > gObj[tObjNum].MaxLife + gObj[tObjNum].AddLife)
				{
					gObj[tObjNum].Life = gObj[tObjNum].MaxLife + + gObj[tObjNum].AddLife;
				}

				bLifeChange = TRUE;
			}

			if(gObj[tObjNum].Mana < gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana)
			{
				gObj[tObjNum].Mana += (gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana) / 100.0f;// + gObj[tObjNum].Mana;

				if(gObj[tObjNum].Mana > gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana)
				{
					gObj[tObjNum].Mana = gObj[tObjNum].MaxMana + gObj[tObjNum].AddMana;
				}

				bManaChange = TRUE;
			}

			if(gObj[tObjNum].BP < gObj[tObjNum].MaxBP + gObj[tObjNum].AddBP)
			{
				gObj[tObjNum].BP += (gObj[tObjNum].MaxBP + gObj[tObjNum].AddBP) / 100;// + gObj[tObjNum].BP;

				if(gObj[tObjNum].BP > gObj[tObjNum].MaxBP + gObj[tObjNum].AddBP)
				{
					gObj[tObjNum].BP = gObj[tObjNum].MaxBP + gObj[tObjNum].AddBP;
				}

				bBpChange = TRUE;
			}

			if(bLifeChange != FALSE)
			{
				GCReFillSend(tObjNum,(WORD)gObj[tObjNum].Life,0xFF,1,gObj[tObjNum].iShield);
			}

			if(bManaChange != FALSE || bBpChange != FALSE)
			{
				GCManaSend(tObjNum,(WORD)gObj[tObjNum].Mana,0xFF,0,gObj[tObjNum].BP);
			}
		}
	}
}

#endif