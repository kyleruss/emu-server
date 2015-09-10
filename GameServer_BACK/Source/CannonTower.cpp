// CannonTower.cpp: implementation of the CCannonTower class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CannonTower.h"
#include "GameMain.h"
#include "..\common\winutil.h"

#if (GS_CASTLE==1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCannonTower g_CsNPC_CannonTower;

CCannonTower::CCannonTower()
{

}

CCannonTower::~CCannonTower()
{

}

///////////////////////////////////////////////////////////////////////

void CCannonTower::CannonTowerAct(int iIndex)
{
	if(rand()%2 != NULL)
		return;

	LPOBJ lpObj = &gObj[iIndex];
	int tObjNum;
	int count = 0;
	PMSG_BEATTACK_COUNT pCount;
	PMSG_BEATTACK pAttack;
	unsigned char AttackSendBuff[0x100];
	int ASBOfs = 0;


	pCount.h.c = 0xC1;
#if (__ENG__==1)
	// ENG PROTOCOL
	pCount.h.headcode = 0xDB;
#else
	// KOR PROTOCOL
	pCount.h.headcode = 0xD3;
#endif
	pCount.h.size = 0;

	pCount.MagicNumberH = 0;
	pCount.MagicNumberL = 0;

	pCount.Count = 0;

	pCount.X = lpObj->X;
	pCount.Y = lpObj->Y;

	ASBOfs = sizeof(pCount);

	while( true )
	{
		if(lpObj->VpPlayer2[count].state != NULL
			&& lpObj->VpPlayer2[count].type == OBJ_USER)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if(tObjNum >= 0)
			{
				if(gObj[tObjNum].m_btCsJoinSide != 1 && gObjCalDistance(lpObj,&gObj[tObjNum]) < 7)
				{
					pAttack.NumberH = SET_NUMBERH(tObjNum);
					pAttack.NumberL = SET_NUMBERL(tObjNum);

					memcpy(&AttackSendBuff[ASBOfs],&pAttack,3);

					ASBOfs += sizeof(pAttack);
					pCount.Count++;
				}
			}
		}

		count++;

		if(count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}

	if(pCount.Count > 0)
	{
		pCount.h.size = ASBOfs;

		memcpy(AttackSendBuff,&pCount,sizeof(pCount));
		CGBeattackRecv(AttackSendBuff,lpObj->m_Index,1);

		PMSG_DURATION_MAGIC_SEND pMsg;
		PHeadSetBE((PBYTE)&pMsg,0x1E,sizeof(pMsg));

		pMsg.MagicNumberH = SET_NUMBERH(AT_SKILL_DEVILFIRE);
		pMsg.MagicNumberL = SET_NUMBERL(AT_SKILL_DEVILFIRE);

		pMsg.X = lpObj->X;
		pMsg.Y = lpObj->Y;
		pMsg.Dir = 0;
		pMsg.NumberH = SET_NUMBERH(iIndex);
		pMsg.NumberL = SET_NUMBERL(iIndex);

		MsgSendV2(lpObj,(PBYTE)&pMsg,pMsg.h.size);
	}
}


#endif