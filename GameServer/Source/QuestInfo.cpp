// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#include "stdafx.h"
#include "QuestInfo.h"
#include "..\include\readscript.h"
#include "..\common\WzMemScript.h"
#include "logproc.h"
#include "GameServer.h"
#include "GameMain.h"
#include "protocol.h"
#include "gObjMonster.h"
#include "DSProtocol.h"
#include "TNotice.h"
#include "..\common\winutil.h"
#include "CashShop.h"
#include "DBSockMng.h"
#include "BuffEffectSlot.h"
#include "MasterSkillSystem.h"
#include "ItemAddOption.h"

unsigned char QuestBitMask[8];
CQuestInfo g_QuestInfo;




char sQuestString[4][50] = { "None", "Accept", "Clear", "None"};	// THIS IS NOT THE PLACE OF THIS VARIABLE




CQuestInfo::CQuestInfo()
{
	QuestBitMask[0]=0xFC;
	QuestBitMask[1]=0xFC;
	QuestBitMask[2]=0xF3;
	QuestBitMask[3]=0xF3;
	QuestBitMask[4]=0xCF;
	QuestBitMask[5]=0xCF;
	QuestBitMask[6]=0x3F;
	QuestBitMask[7]=0x3F;

	this->Init();
}


CQuestInfo::~CQuestInfo()
{
	return;
}

void CQuestInfo::Init()
{
	this->m_QuestCount = 0;
	memset(this->QuestInfo, -1, sizeof(this->QuestInfo));

	for ( int i=0;i<MAX_QUEST_INFO;i++)
	{
		this->QuestInfo[i].QuestConditionCount=0;
		this->QuestInfo[i].QuestSubInfoCount= 0;
	}
}

BOOL CQuestInfo::LoadQuestInfo(char * filename)
{
	int Token;
	int n;

	SMDFile = fopen(filename, "r");

	if ( SMDFile == NULL )
	{
		MsgBox("load error %s", filename);
		return false;
	}

	this->Init();

	n = 0;

	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}
		else
		{
			if ( Token == 1 )
			{
				int Type = (int)TokenNumber;

				if ( Type == 0 )
				{
					Token = GetToken();
					int iQuestIndex = (int)TokenNumber;
					this->QuestInfo[iQuestIndex].QuestIndex = iQuestIndex;

					Token = GetToken();
					this->QuestInfo[iQuestIndex].QuestStartType = (int)TokenNumber;

					Token = GetToken();
					this->QuestInfo[iQuestIndex].QuestStartSubType = (int)TokenNumber;

					Token = GetToken();
					strcpy_s(this->QuestInfo[iQuestIndex].Name, sizeof(this->QuestInfo[iQuestIndex].Name), TokenString);

					this->QuestInfo[iQuestIndex].QuestSubInfoCount = 0;

					while ( true )
					{
						int iSubInfoCount = this->QuestInfo[iQuestIndex].QuestSubInfoCount;

						Token = GetToken();

						if ( Token == 0 )
						{
							if ( strcmp("end", TokenString) == 0 )
							{
								break;
							}
						}
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].QuestType = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedType = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedSubType = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ItemLevel = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedNumber = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMinLevel = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMaxLevel = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedDropRate = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardType = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardSubType = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardCount = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].LinkConditionIndex = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextBeforeReg = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextAfterReg = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextCompleteQuest = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextQuestClear = (int)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[0] = (BYTE)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[1] = (BYTE)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[2] = (BYTE)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[3] = (BYTE)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[4] = (BYTE)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[5] = (BYTE)TokenNumber;

						Token = GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[6] = (BYTE)TokenNumber;

						this->QuestInfo[iQuestIndex].QuestSubInfoCount++;
					}
					
					LogAdd("[Quest] (%s) Load : Index %d, SubInfoCount %d",
						this->QuestInfo[iQuestIndex].Name, iQuestIndex,
						this->QuestInfo[iQuestIndex].QuestSubInfoCount );
						this->m_QuestCount++;
				}
				else if ( Type == 1 )
				{
					Token = GetToken();
					int iQuestIndex = (int)TokenNumber;

					while ( true )
					{
						int iFailInfoCount = this->QuestInfo[iQuestIndex].QuestConditionCount;
						Token = GetToken();

						if ( Token == 0 )
						{
							if ( strcmp("end", TokenString) == 0 )
							{
								break;
							}
						}
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].Index = (int)TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].NeedQuestIndex = (int)TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].MinLevel = (int)TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].MaxLevel = (int)TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].ReqStr = (int)TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].NeedZen = (int)TokenNumber;

							Token = GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].StartContext = (int)TokenNumber;

							this->QuestInfo[iQuestIndex].QuestConditionCount++;
						
					}
				}
				else
				{
					MsgBox("Quest Info Read Fail");
					fclose(SMDFile);
					return false;
				}
			}
		}
	}

	fclose(SMDFile);
	this->InitQuestItem();
	return true;
}


BOOL CQuestInfo::LoadQuestInfo(char* Buffer, int iSize)
{

	CWzMemScript WzMemScript;
	int Token;
	int n;

	WzMemScript.SetBuffer(Buffer, iSize);
	this->Init();

	n = 0;

	while ( true )
	{
		Token = WzMemScript.GetToken();

		if ( Token == 2 )
		{
			break;
		}
		else
		{
			if ( Token == 1 )
			{
				int Type = WzMemScript.GetNumber();

				if ( Type == 0 )
				{
					Token = WzMemScript.GetToken();
					int iQuestIndex = WzMemScript.GetNumber();
					this->QuestInfo[iQuestIndex].QuestIndex = iQuestIndex;

					Token = WzMemScript.GetToken();
					this->QuestInfo[iQuestIndex].QuestStartType = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					this->QuestInfo[iQuestIndex].QuestStartSubType = WzMemScript.GetNumber();

					Token = WzMemScript.GetToken();
					strcpy_s( this->QuestInfo[iQuestIndex].Name, sizeof(this->QuestInfo[iQuestIndex].Name), WzMemScript.GetString());

					this->QuestInfo[iQuestIndex].QuestSubInfoCount = 0;

					while ( true )
					{
						int iSubInfoCount = this->QuestInfo[iQuestIndex].QuestSubInfoCount;

						Token = WzMemScript.GetToken();

						if ( Token == 0 )
						{
							if ( strcmp("end", WzMemScript.GetString()) == 0 )
							{
								break;
							}
						}
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].QuestType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedSubType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ItemLevel = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedNumber = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMinLevel = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMaxLevel = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedDropRate = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardSubType = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardCount = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].LinkConditionIndex = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextBeforeReg = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextAfterReg = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextCompleteQuest = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ContextQuestClear = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[0] = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[1] = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[2] = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[3] = WzMemScript.GetNumber();

						Token = WzMemScript.GetToken();
						this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[4] = WzMemScript.GetNumber();

						this->QuestInfo[iQuestIndex].QuestSubInfoCount++;

					
					}
					
					LogAdd("[Quest] (%s) Load : Index %d, SubInfoCount %d",
						this->QuestInfo[iQuestIndex].Name, iQuestIndex,
						this->QuestInfo[iQuestIndex].QuestSubInfoCount );
						this->m_QuestCount++;
				}
				else if ( Type == 1 )
				{
					Token = WzMemScript.GetToken();
					int iQuestIndex = WzMemScript.GetNumber();

					while ( true )
					{
						int iFailInfoCount = this->QuestInfo[iQuestIndex].QuestConditionCount;
						Token = WzMemScript.GetToken();

						if ( Token == 0 )
						{
							if ( strcmp("end", WzMemScript.GetString()) == 0 )
							{
								break;
							}
						}	
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].Index = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].NeedQuestIndex = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].MinLevel = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].MaxLevel = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].ReqStr = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].NeedZen = WzMemScript.GetNumber();

							Token = WzMemScript.GetToken();
							this->QuestInfo[iQuestIndex].QuestCondition[iFailInfoCount].StartContext = WzMemScript.GetNumber();

							this->QuestInfo[iQuestIndex].QuestConditionCount++;
						
					}
				}
				else
				{
					MsgBox("Quest Info Read Fail");
					fclose(SMDFile);
					return false;
				}
			}
		}
	}

	this->InitQuestItem();
	return true;
}


void CQuestInfo::InitQuestItem()
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;

	for (int i=0;i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		for ( int n=0;n<lpQuestInfo->QuestSubInfoCount;n++)
		{
			lpSubInfo = &lpQuestInfo->QuestSubInfo[n];

			if ( lpSubInfo != NULL )
			{
				if ( lpSubInfo->QuestType == 1)
				{
					LPITEM_ATTRIBUTE p = &ItemAttribute[ITEMGET(lpSubInfo->NeedType, lpSubInfo->NeedSubType) ];
					p->QuestItem = TRUE;
					LogAddTD("[Quest] [SetQuestItem] %s", p->Name);
				}
			}
		}
		foundquest++;
		if ( foundquest == questcount )
		{
			return;
		}
	}
}


BOOL CQuestInfo::IsQuest(int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}

	if ( this->QuestInfo[QuestIndex].QuestIndex == -1 )
	{
		return false;
	}

	return true;
}


					
int CQuestInfo::GetQuestState(LPOBJ lpObj, int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}

	int index = (QuestIndex) /4;
	int shift = (QuestIndex % 4) *2;
	return (lpObj->m_Quest[index] >> shift) &3;
}


BYTE CQuestInfo::GetQuestStateBYTE(LPOBJ lpObj, int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}
	
	int index = (QuestIndex / 4);
	return lpObj->m_Quest[index];
}


BYTE CQuestInfo::SetQuestState(LPOBJ lpObj, int QuestIndex, int State)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return -1;
	}

	int QuestState = this->GetQuestState(lpObj, QuestIndex);

	if ( QuestState == 3 || QuestState == 0)
	{
		BYTE btCond = this->QuestRunConditionCheck(lpObj, QuestIndex);

		if ( btCond != 0 )
		{
			return btCond;
		}

		this->QuestAccept(lpObj, QuestIndex);
		LogAddTD("[Quest] AcceptQuest [%s][%s] (%s)", lpObj->AccountID, lpObj->Name,
			this->GetQuestInfo(QuestIndex)->Name);
		State = 1;
	}
	else if ( QuestState == 1 )
	{
		BYTE btCond = this->QuestClearConditionCheck(lpObj, QuestIndex);

		if ( btCond != 0 )
		{
			return btCond;
		}

		this->QuestClear(lpObj, QuestIndex);
		State = 2;
		LogAddTD("[Quest] ClearQuest [%s][%s] (%s)", lpObj->AccountID, lpObj->Name,
			this->GetQuestInfo(QuestIndex)->Name);
	}
	else
	{
		return -1;
	}

	int index = (QuestIndex / 4);
	int shift =  (QuestIndex % 4) *2;
	lpObj->m_Quest[index] &= QuestBitMask[shift];
	lpObj->m_Quest[index] |= (State&3) << shift;
	return 0;
}

BYTE CQuestInfo::ReSetQuestState(LPOBJ lpObj, int QuestIndex)
{
	int State = 3;
	int index = (QuestIndex/4);
	int shift = (QuestIndex % 4)*2;
	lpObj->m_Quest[index] &= QuestBitMask[shift];
	lpObj->m_Quest[index] |= (State&3) << shift;

	return 0;
}


LPQUEST_INFO CQuestInfo::GetQuestInfo(int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}
	
	if ( this->QuestInfo[QuestIndex].QuestIndex == -1 )
	{
		return NULL;
	}

	return &this->QuestInfo[QuestIndex];
}

LPQUEST_SUB_INFO CQuestInfo::GetSubquestInfo(LPOBJ lpObj, LPQUEST_INFO lpQuestInfo, int subquestindex)
{
	if ( lpQuestInfo == NULL )
	{
		return NULL;
	}

	if ( subquestindex < 0 || subquestindex > MAX_SUBINFO_QUEST )
	{
		return NULL;
	}

	LPQUEST_SUB_INFO lpSubInfo = &lpQuestInfo->QuestSubInfo[subquestindex];
	int Class = lpObj->Class;
	int ChangeUP = lpObj->ChangeUP;
	int requireclass = lpSubInfo->RequireClass[Class];

	if ( requireclass == 0 )
	{
		return NULL;
	}

	if ( requireclass > 1 )
	{
		if ( requireclass != (ChangeUP + 1) )
		{
			return NULL;
		}
	}

	return lpSubInfo;
}

LPQUEST_CONDITION CQuestInfo::GetQuestCondition(LPQUEST_INFO lpQuestInfo, int conditionindex)
{
	if ( lpQuestInfo == NULL )
	{
		return NULL;
	}

	if ( conditionindex < 0 || conditionindex > MAX_CONDITION_QUEST )
	{
		return NULL;
	}

	return &lpQuestInfo->QuestCondition[conditionindex];
}

BYTE CQuestInfo::QuestClearConditionCheck(LPOBJ lpObj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return -1;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;
	BOOL bFoundSubQuest = FALSE;
	

	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			bFoundSubQuest = TRUE;

			if ( lpSubInfo->QuestType == 1)
			{
				int NumberItemQuestFound = gObjGetItemCountInIventory(lpObj->m_Index, lpSubInfo->NeedType, lpSubInfo->NeedSubType, lpSubInfo->ItemLevel);

				if ( NumberItemQuestFound < lpSubInfo->NeedNumber)
				{
					return 1;
				}
			}
		}
	}

	if ( bFoundSubQuest == FALSE )
	{
		return -1;
	}

	return 0;
}


BYTE CQuestInfo::QuestRunConditionCheck(LPOBJ lpObj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return -1;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;
	LPQUEST_CONDITION lpCondition;
	BOOL bFoundSubQuest = FALSE;


	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			bFoundSubQuest = TRUE;

			for ( int i=0;i<concount;i++)
			{
				lpCondition = this->GetQuestCondition(lpQuestInfo, i);

				if ( lpCondition != NULL && lpCondition->Index == -1)
				{
					if ( this->CompareCondition(lpObj, lpCondition) == FALSE )
					{
						return lpCondition->StartContext;
					}
				}
				else if ( lpCondition != NULL && lpCondition->Index == lpSubInfo->LinkConditionIndex )
				{
					if ( this->CompareCondition(lpObj, lpCondition) == 0)
					{
						return lpCondition->StartContext;
					}
				}
				else if ( lpCondition == NULL )
				{
					return -1;
				}
			}

		}
	}

	if ( bFoundSubQuest == FALSE )
	{
		return -1;
	}

	return 0;
}

BOOL CQuestInfo::CompareCondition(LPOBJ lpObj, LPQUEST_CONDITION lpCondition)
{
	if ( lpCondition->NeedQuestIndex != -1 )
	{
		if ( this->GetQuestState(lpObj, lpCondition->NeedQuestIndex) != 2 )
		{
			return FALSE;
		}
	}

	if ( lpCondition->MinLevel != 0 )
	{
		if ( lpCondition->MinLevel > lpObj->Level )
		{
			return FALSE;
		}
	}

	if ( lpCondition->MaxLevel != 0 )
	{
		if ( lpCondition->MaxLevel < lpObj->Level )
		{
			return FALSE;
		}
	}

	if ( lpCondition->NeedZen > lpObj->Money )
	{
		return FALSE;
	}

	if ( lpCondition->ReqStr > lpObj->Strength + lpObj->AddStrength)
	{
		return FALSE;
	}

	return TRUE;

}




BOOL CQuestInfo::NpcTalk(LPOBJ lpNpc, LPOBJ lpObj)
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	int queststate = -1;
	int questindex = -1;

	for ( int i = 0; i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		if ( lpQuestInfo->QuestStartType == 2 ) // NPC Click
		{
			if ( lpNpc->Class == lpQuestInfo->QuestStartSubType)
			{
				if ( queststate == -1 )
				{
					questindex = lpQuestInfo->QuestIndex;
					queststate = this->GetQuestState(lpObj, lpQuestInfo->QuestIndex);
				}
				else if ( queststate == 2 )
				{
					for ( int n=0;n<lpQuestInfo->QuestSubInfoCount ;n++)
					{
						LPQUEST_SUB_INFO lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, n);

						if ( lpSubInfo != NULL )
						{
							questindex = lpQuestInfo->QuestIndex;
							queststate = this->GetQuestState(lpObj, lpQuestInfo->QuestIndex);
						}
					}
				}
			}
		}

		foundquest++;

		if ( foundquest == questcount )
		{
			break;
		}
	}

	if ( queststate != -1 )
	{
		int bSendKillInfo = SendMonsterKillInfo(questindex,lpObj->m_Index);

		if( bSendKillInfo == FALSE )
		{
			return TRUE;
		}

		GCSendQuestInfo(lpObj->m_Index, questindex);
		lpObj->m_IfState.type = 10;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.use = 1;
		return true;
	}

	return false;
}

struct PMSG_SENDMONSTERKILLINFO
{
	PBMSG_HEAD2 h;
	BYTE Result;
	BYTE QuestIndex;
	int MonsterInfo[10];
};

int CQuestInfo::SendMonsterKillInfo(int QuestIndex,int iIndex)
{
	PMSG_SENDMONSTERKILLINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xA4,0,sizeof(pMsg));
	pMsg.Result = 0;
	pMsg.QuestIndex = QuestIndex;

	LPOBJ lpObj = &gObj[iIndex];

	int QuestState = GetQuestState(lpObj,QuestIndex);

	if( QuestState != 1 )
	{
		return TRUE;
	}

	LPQUEST_INFO lpQuest = GetQuestInfo(QuestIndex);

	if( lpQuest == FALSE )
	{
		return TRUE;
	}


	int SubInfoCount = lpQuest->QuestSubInfoCount;
	int ConditionCount = lpQuest->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubQuest;
	int bFound = 0;

	for(int i = 0; i < SubInfoCount; i++)
	{
		lpSubQuest = GetSubquestInfo(lpObj,lpQuest,i);

		if( lpSubQuest != NULL)
		{
			bFound = TRUE;

			if( lpSubQuest->QuestType == 2 )
			{
				if( lpObj->m_MonsterKillInfoLoaded == FALSE )
				{
					g_MonsterKillInfo.DGReqMonsterKillInfo(lpObj);
					return FALSE;
				}
	
				if( lpObj->m_MonsterKillInfoLoaded == TRUE && lpObj->m_MonsterKillQuestIndex == -1 )
				{
					lpObj->m_MonsterKillInfoLoaded = FALSE;
					g_MonsterKillInfo.DGReqMonsterKillInfo(lpObj);

					LogAddTD("[Quest] Error - Invalid MonsterKillInfo [%s][%s] (QuestIndex:%d/DBIndex:%d)",
						lpObj->AccountID,lpObj->Name,QuestIndex,lpObj->m_MonsterKillQuestIndex);
					return FALSE;
				}
	
				pMsg.Result = TRUE;
				pMsg.QuestIndex = lpObj->m_MonsterKillQuestIndex;
	
				int n = 0;
	
				for(int x = 0; x < 5; x ++)
				{
					pMsg.MonsterInfo[n] = lpObj->m_MonsterKill[x].m_MonsterIndex;
					n++;
					pMsg.MonsterInfo[n] = lpObj->m_MonsterKill[x].m_KillCount;
					n++;
				}
				break;
			}
		}
	}

	if( pMsg.Result == 1 )
	{
		DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
	}
return TRUE;
}

BOOL CQuestInfo::MonsterItemDrop(LPOBJ lpObj)	//1.00.90
{
	int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj); //loc1

	if ( MaxHitUser == -1 )
	{
		return false;
	}

	int partycount = gParty.GetPartyCount(gObj[MaxHitUser].PartyNumber); //loc3

	if ( partycount > 0 ) //Season 2.5 changed
	{
		int MaxHitUserInParty = this->MonsterItemDropParty(lpObj,&gObj[MaxHitUser]);
		return MaxHitUserInParty;
	}

	LPOBJ lpTarget = &gObj[MaxHitUser];
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;
	int type;
	int level;
	int x;
	int y;
	float dur = 0;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	for ( int i=0;i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		for ( int n =0;n<lpQuestInfo->QuestSubInfoCount;n++)
		{
			lpSubInfo = this->GetSubquestInfo(lpTarget, lpQuestInfo, n);
			
			if ( lpSubInfo != NULL )
			{
				if ( lpSubInfo->QuestType == 1 )
				{
					if( (lpSubInfo->NeedTargetMinLevel > 0) && (lpObj->Level >= lpSubInfo->NeedTargetMinLevel) && (lpObj->Level <= lpSubInfo->NeedTargetMaxLevel) || (lpSubInfo->NeedTargetMinLevel == -1) && (lpObj->Class == lpSubInfo->NeedTargetMaxLevel))
					{
						if ( this->GetQuestState(lpTarget, lpQuestInfo->QuestIndex) == TRUE )
						{
							if ( (rand() % ITEM_QUEST_DROP_PROBABILITY) < lpSubInfo->NeedDropRate)
							{
									int itemcount = gObjGetItemCountInIventory(MaxHitUser, lpSubInfo->NeedType,
										lpSubInfo->NeedSubType, lpSubInfo->ItemLevel);

									if ( itemcount >= lpSubInfo->NeedNumber)
									{
										continue;
									}

									dur = 0;
									x = lpObj->X;
									y = lpObj->Y;
									level = lpSubInfo->ItemLevel;
									type = ItemGetNumberMake(lpSubInfo->NeedType, lpSubInfo->NeedSubType);
									ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur, Option1,	Option2, Option3, MaxHitUser, 0, 0);
									LogAddTD("[Quest] Quest Item Drop [%s]: [%s][%s] (%s) (%d,%d)", lpObj->Name, lpTarget->AccountID, lpTarget->Name, lpQuestInfo->Name, lpSubInfo->NeedType, lpSubInfo->NeedSubType);
									return true;
							}
						}
					}
				}
			}
		}

		foundquest++;

		if ( foundquest == questcount )
		{
			break;
		}
	}

	return false;
}

BOOL CQuestInfo::MonsterItemDropParty(LPOBJ lpObj, LPOBJ lpTargetObj)	//1.00.90
{
	int questcount = this->GetQeustCount(); //loc1
	int foundquest = 0; //loc2
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;
	int type;
	int level;
	int x;
	int y;
	float dur = 0; //loc10
	int Option1 = 0; //loc11
	int Option2 = 0; //loc12
	int Option3 = 0; //loc13
	int TargetPartyNumber = lpTargetObj->PartyNumber; //loc14
	int LocalPartyNumber; //loc15

	if(TargetPartyNumber < 0)
	{
		return false;
	}

	LPOBJ loc16; //loc16

	int n; //loc17
	int i; //loc18
	int j; //loc19

	for ( n=0;n<MAX_USER_IN_PARTY;n++)
	{
		LocalPartyNumber = gParty.m_PartyS[TargetPartyNumber].Number[n];

		if(LocalPartyNumber < 0)
		{
			continue;
		}

		if (gObj[LocalPartyNumber].MapNumber != lpTargetObj->MapNumber)
		{
			continue;
		}

		loc16 = &gObj[LocalPartyNumber];

		for ( i=0;i<MAX_QUEST_INFO;i++)
		{
			lpQuestInfo = this->GetQuestInfo(i);

			if ( lpQuestInfo == NULL )
			{
				continue;
			}

			for ( j=0;j<lpQuestInfo->QuestSubInfoCount;j++)
			{
				lpSubInfo = this->GetSubquestInfo(loc16, lpQuestInfo, j);
			
				if ( lpSubInfo == NULL )
				{
					continue;
				}
				
				if ( lpSubInfo->QuestType == 1 )
				{
					if( (lpSubInfo->NeedTargetMinLevel > 0) && (lpObj->Level >= lpSubInfo->NeedTargetMinLevel) && (lpObj->Level <= lpSubInfo->NeedTargetMaxLevel) || (lpSubInfo->NeedTargetMinLevel == -1) && (lpObj->Class == lpSubInfo->NeedTargetMaxLevel))
					{
						if ( this->GetQuestState(loc16, lpQuestInfo->QuestIndex) == TRUE )
						{
							if ( (rand() % ITEM_QUEST_DROP_PROBABILITY) < lpSubInfo->NeedDropRate)
							{
								dur = 0;
								x = lpTargetObj->X;
								y = lpTargetObj->Y;
								level = lpSubInfo->ItemLevel;
								type = ItemGetNumberMake(lpSubInfo->NeedType, lpSubInfo->NeedSubType);
								ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur, Option1, Option2, Option3, lpTargetObj->m_Index, 0, 0);
								LogAddTD("[Quest] Quest Item Drop(Party) [%s]: [%s][%s] (%s) (%d,%d)", lpObj->Name,	lpTargetObj->AccountID, lpTargetObj->Name, lpQuestInfo->Name, lpSubInfo->NeedType, lpSubInfo->NeedSubType);
								return true;
							}
						}
					}					
				}
			}

			foundquest++;

			if ( foundquest == questcount )
			{
				break;
			}

		}
	}

	return false;
}

BOOL CQuestInfo::QuestAccept(LPOBJ lpObj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return 0xFF;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;
	LPQUEST_CONDITION lpCondition;
	int NeedZen = 0;

	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			for ( int n = 0;n<concount;n++)
			{
				lpCondition = this->GetQuestCondition(lpQuestInfo, n);

				if( lpCondition != 0 && lpCondition->Index == -1)
				{
					NeedZen += lpCondition->NeedZen;
				}
				else if( lpCondition != 0 )
				{
					if( lpCondition->Index == lpSubInfo->LinkConditionIndex )
					{
						NeedZen += lpCondition->NeedZen;
					}
				}
			}

			if( lpSubInfo->QuestType == 2 )
			{
				if( lpObj->m_MonsterKillQuestIndex == -1 )
				{
					lpObj->m_MonsterKillQuestIndex = QuestIndex;
				}

				for(int x = 0; x < 5; x++)
				{
					if( lpObj->m_MonsterKill[x].m_MonsterIndex == -1 )
					{
						lpObj->m_MonsterKill[x].m_MonsterIndex = lpSubInfo->NeedType;
						lpObj->m_MonsterKill[x].m_KillCount = 0;
						break;
					}
				}

				LogAddTD("[Quest] Quest Accept - MonsterKillCount Set (%s) : [%s][%s]",
					lpQuestInfo->Name,lpObj->AccountID,lpObj->Name);
			}
		}
	}

	lpObj->Money -= NeedZen;
	GCMoneySend(lpObj->m_Index, lpObj->Money);
	return true;
}

BOOL CQuestInfo::QuestClear(LPOBJ lpObj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return 0xFF;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;

	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			if ( lpSubInfo->RewardType == QUEST_COMPENSATION_CHANGEUP )
			{
				lpObj->LevelUpPoint += lpSubInfo->RewardCount;
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_STATUP, lpSubInfo->RewardCount );
				lpObj->ChangeUP = true;
				lpObj->DbClass |= 1;
				gObjMakePreviewCharSet(lpObj->m_Index);
				BYTE btClass = CS_SET_CLASS(lpObj->Class);
				btClass |= CS_SET_CHANGEUP(lpObj->ChangeUP);
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_CHANGEUP, btClass);
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d), ChangeUp",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name, lpObj->LevelUpPoint,
					lpSubInfo->RewardCount );
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_STATUP )
			{
				lpObj->LevelUpPoint += lpSubInfo->RewardCount;
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_STATUP, lpSubInfo->RewardCount );
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d)",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name, lpObj->LevelUpPoint,
					lpSubInfo->RewardCount );
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_PLUSSSTAT )
			{
				int level = lpObj->Level - QUEST_MINLEVEL_PLUSSTAT;

				if ( level <= 0 )
				{
					level = 0;
				}

				lpObj->LevelUpPoint += level;
				lpObj->PlusStatQuestClear = true;
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_PLUSSSTAT, level );
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d), PlusStat",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name, lpObj->LevelUpPoint,
					level );
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_COMBOSKILL )
			{
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_COMBOSKILL, 0 );
				lpObj->ComboSkillquestClear = true;
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] ComboSkill",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name);
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_CHANGEUP2 )
			{
				lpObj->ChangeUP2 = 1;
				lpObj->DbClass |= 2;

				gObjMakePreviewCharSet(lpObj->m_Index);

				BYTE btClass = CS_SET_CLASS(lpObj->Class);
				btClass |= CS_SET_CHANGEUP(lpObj->ChangeUP);
				btClass |= (lpObj->ChangeUP2<<3)&8;

				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_CHANGEUP2, btClass);
				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] Class:%d(%d), 3rd ChangeUp",
					lpQuestInfo->Name, lpObj->AccountID, lpObj->Name, lpObj->DbClass,btClass);
			}

			if ( lpSubInfo->QuestType == 1 )
			{
				gObjDelteItemCountInInventory(lpObj->m_Index, lpSubInfo->NeedType, lpSubInfo->NeedSubType,lpSubInfo->NeedNumber);
			}

			if ( lpSubInfo->QuestType == 2 )
			{
				lpObj->m_MonsterKillQuestIndex = -1;
				for(int i = 0; i < 5; i++)
				{
					lpObj->m_MonsterKill[i].m_MonsterIndex = -1;
					lpObj->m_MonsterKill[i].m_KillCount = -1;
				}

				LogAddTD("[Quest] Quest Clear - MonsterKillCount Reset (%s) : [%s][%s]",
					lpQuestInfo->Name,lpObj->AccountID,lpObj->Name);
			}

			if ( lpSubInfo->RewardSubType == 200 )
			{
				lpObj->LevelUpPoint += lpSubInfo->RewardCount;
				GCSendQuestPrize(lpObj->m_Index, QUEST_COMPENSATION_STATUP, lpSubInfo->RewardCount );

				LogAddTD("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d), Class:%d PlusStat",
					lpQuestInfo->Name,lpObj->AccountID,lpObj->Name,lpObj->LevelUpPoint,lpSubInfo->RewardCount,lpObj->DbClass);
			}
		}
	}
	return true;
}


void CQuestInfo::QuestInfoSave(LPOBJ lpObj)
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;

	for ( int i = 0 ; i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		LogAddTD("[Quest] QuestSave [%s][%s] index(%d) name(%s) state(%s)",
			lpObj->AccountID, lpObj->Name, i, lpQuestInfo->Name,
			sQuestString[this->GetQuestState(lpObj, i)]);
		foundquest++;

		if ( foundquest == questcount )
		{
			break;
		}
	}
}

BOOL CQuestInfo::MonsterKill(LPOBJ Npc,LPOBJ lpObj)
{
	if( lpObj->Connected < PLAYER_PLAYING && lpObj->Live != FALSE )
	{
		return FALSE;
	}

	int partynumber = lpObj->PartyNumber;

	if( partynumber < 0 )
	{
		if( lpObj->m_MonsterKillQuestIndex == -1 )
		{
			return FALSE;
		}

		for(int i = 0; i < 5; i++)
		{
			if( Npc->Class == lpObj->m_MonsterKill[i].m_MonsterIndex &&
				lpObj->m_MonsterKill[i].m_KillCount <= 50 )
			{
				lpObj->m_MonsterKill[i].m_KillCount++;
				break;
			}
		}
	}
	else
	{
		int partynumber = lpObj->PartyNumber;
		int tObjNum;
		LPOBJ lpPartyObj;
		int Distance = 0;

		for(int i = 0; i < 5; i ++)
		{
			tObjNum = gParty.m_PartyS[partynumber].Number[i];

			if( tObjNum < 0 )
				continue;

			lpPartyObj = &gObj[tObjNum];

			if( lpPartyObj->Connected < PLAYER_PLAYING &&
				lpPartyObj->Live != FALSE )
				continue;

			if(  lpObj->MapNumber != lpPartyObj->MapNumber)
				continue;

			Distance = gObjCalDistance(lpObj,lpPartyObj);

			if( Distance > 25 )
				continue;

			if( lpPartyObj->m_MonsterKillQuestIndex == -1 )
				continue;

			for(int n = 0; n < 5; n++)
			{
				if( Npc->Class == lpPartyObj->m_MonsterKill[n].m_MonsterIndex &&
					lpObj->m_MonsterKill[n].m_KillCount <= 50 )
				{
					lpPartyObj->m_MonsterKill[n].m_KillCount++;
					break;
				}
			}
		}
	}
return true;
}

struct PMSG_QUESTMOVESEND
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

void CQuestInfo::NotifyResultEnterBarracks(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj == NULL )
		return;

	PMSG_QUESTMOVESEND pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xD0,5,sizeof(pMsg));
	pMsg.btResult = FALSE;

	int NeedZen = 3000000;
	int NeedLevel = 350;

	if( lpObj->Level < 350 ||
		lpObj->Money < NeedZen )
	{
		pMsg.btResult = FALSE;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	int QuestState = GetQuestState(lpObj,5);

	if( QuestState == 1 ||
		QuestState == 2)
	{
		LPOBJ lpPartyObj;
		int partynumber = lpObj->PartyNumber;
		int tObjNum;
		int Distance;
		int bResult = 0;

		if( partynumber >= 0 )
		{
			for(int i = 0; i < MAX_USER_IN_PARTY; i++)
			{
				tObjNum = gParty.m_PartyS[partynumber].Number[i];

				if( tObjNum < 0 )
					continue;

				if(  aIndex == tObjNum )
					continue;

				lpPartyObj = &gObj[tObjNum];

				if( lpPartyObj->Level < NeedLevel )
					continue;

				if( lpObj->MapNumber == lpPartyObj->MapNumber )
				{
					Distance = gObjCalDistance(lpObj,lpPartyObj);
					if( Distance < 10 )
					{
						bResult = gObjMoveGate(tObjNum,256);

						if( bResult == FALSE )
						{
							pMsg.btResult = 0;
							DataSend(tObjNum,(LPBYTE)&pMsg,pMsg.h.size);
						}
					}
				}
			}
		}

		bResult = gObjMoveGate(aIndex,256);

		if( bResult != FALSE )
		{
			lpObj->Money -= NeedZen;
			GCMoneySend(lpObj->m_Index,lpObj->Money);
		}
		else
		{
			pMsg.btResult = 0;
			DataSend(tObjNum,(LPBYTE)&pMsg,pMsg.h.size);
		}
	}
}

void CQuestInfo::NotifyResultEnterRefuge(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj == NULL )
		return;

	PMSG_QUESTMOVESEND pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xD0,8,sizeof(pMsg));
	pMsg.btResult = FALSE;

	int NeedLevel = 350;

	if( lpObj->Level < NeedLevel )
	{
		pMsg.btResult = FALSE;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	if( lpObj->m_MonsterKillQuestIndex != 6 )
	{
		pMsg.btResult = FALSE;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	LPOBJ lpPartyObj;
	int partynumber = lpObj->PartyNumber;
	int tObjNum;
	int Distance;
	int bResult = 0;

	if( partynumber >= 0 )
	{
		for(int i = 0; i < MAX_USER_IN_PARTY; i++)
		{
			tObjNum = gParty.m_PartyS[partynumber].Number[i];

			if( tObjNum < 0 )
				continue;

			if(  aIndex == tObjNum )
				continue;

			lpPartyObj = &gObj[tObjNum];

			if( lpPartyObj->Level < NeedLevel )
				continue;

			if( lpObj->MapNumber == lpPartyObj->MapNumber )
			{
				Distance = gObjCalDistance(lpObj,lpPartyObj);
				if( Distance < 10 )
				{
					bResult = gObjMoveGate(tObjNum,257);

					if( bResult == FALSE )
					{
						pMsg.btResult = 0;
						DataSend(tObjNum,(LPBYTE)&pMsg,pMsg.h.size);
						break;
					}
				}
			}
		}
	}

	bResult = gObjMoveGate(aIndex,257);

	if( bResult == FALSE )
	{
		pMsg.btResult = 0;
		DataSend(tObjNum,(LPBYTE)&pMsg,pMsg.h.size);
	}
}

int CQuestInfo::CheckItemCount(int aIndex,short Type,short Level)
{
	if( gObj[aIndex].m_MonsterKillQuestIndex != -1 )
	{
		return FALSE;
	}

	LPOBJ lpObj = &gObj[aIndex];
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;
	int SubQuestCount;
	int QuestState = 3;

	for(int i = 0; i < m_QuestCount; i++)
	{
		QuestState = GetQuestState(lpObj,i);

		if( QuestState == 1 )
		{
			lpQuestInfo = GetQuestInfo(i);

			if( lpQuestInfo == NULL )
			{
				return FALSE;
			}

			SubQuestCount = lpQuestInfo->QuestSubInfoCount;

			for(int n = 0; n < SubQuestCount; n++)
			{
				lpSubInfo = GetSubquestInfo(lpObj,lpQuestInfo,n);

				if( lpSubInfo == NULL )
					continue;

				if( lpSubInfo->QuestType != 1 )
				{
					return FALSE;
				}

				if( Type == lpSubInfo->NeedType * 512 + lpSubInfo->NeedSubType &&
					Level == lpSubInfo->ItemLevel )
				{
					int ItemCount = gObjGetItemCountInIventory(aIndex,Type);

					if( ItemCount >= lpSubInfo->NeedNumber )
					{
						LogAddTD("[Quest] Too many has quest items [%s][%s] (%d/%d)",
							lpObj->AccountID,lpObj->Name,Type,ItemCount);
						return FALSE;
					}
					else
					{
						return TRUE;
					}
				}
			}

			return FALSE;
		}
	}
return FALSE;
}

struct PMSG_REQ_QUESTMONSTERKILLINFO
{
	PBMSG_HEAD h;
	int Index;
	char szName[11];
};

CQuestMonsterKillInfo g_MonsterKillInfo;

void CQuestMonsterKillInfo::DGReqMonsterKillInfo(LPOBJ lpObj)
{
	if( lpObj->m_MonsterKillInfoLoaded != FALSE )
	{
		return;
	}

	PMSG_REQ_QUESTMONSTERKILLINFO pMsg;
	pMsg.h.set((LPBYTE)&pMsg,0x13,sizeof(pMsg));
	pMsg.Index = lpObj->m_Index;
	memcpy(pMsg.szName,lpObj->Name,10);
	pMsg.szName[10] = 0;

	cDBSMng.Send((char*)&pMsg,pMsg.h.size);
}

typedef struct
{
	PBMSG_HEAD h;
	int Index;
	char szName[11];
	int QuestIndex;
	QUEST_MONSTER_KILL_INFO KillInfo[5];
}PMSG_ANS_MONSTERKILLINFO, *LPMSG_ANS_MONSTERKILLINFO;

void CQuestMonsterKillInfo::GDAnsMonsterKillInfo(LPBYTE aRecv)
{
	LPMSG_ANS_MONSTERKILLINFO lpMsg = (LPMSG_ANS_MONSTERKILLINFO)aRecv;

	if( gObjIsConnected(lpMsg->Index) == FALSE )
	{
		return;
	}

	int iIndex = lpMsg->Index;
	LPOBJ lpObj = &gObj[iIndex];

	if( lpObj->m_MonsterKillInfoLoaded == FALSE )
	{
		lpObj->m_MonsterKillQuestIndex = lpMsg->QuestIndex;

		lpObj->m_MonsterKill[0].m_MonsterIndex = lpMsg->KillInfo[0].m_MonsterIndex;
		lpObj->m_MonsterKill[0].m_KillCount = lpMsg->KillInfo[0].m_KillCount;

		lpObj->m_MonsterKill[1].m_MonsterIndex = lpMsg->KillInfo[1].m_MonsterIndex;
		lpObj->m_MonsterKill[1].m_KillCount = lpMsg->KillInfo[1].m_KillCount;

		lpObj->m_MonsterKill[2].m_MonsterIndex = lpMsg->KillInfo[2].m_MonsterIndex;
		lpObj->m_MonsterKill[2].m_KillCount = lpMsg->KillInfo[2].m_KillCount;

		lpObj->m_MonsterKill[3].m_MonsterIndex = lpMsg->KillInfo[3].m_MonsterIndex;
		lpObj->m_MonsterKill[3].m_KillCount = lpMsg->KillInfo[3].m_KillCount;

		lpObj->m_MonsterKill[4].m_MonsterIndex = lpMsg->KillInfo[4].m_MonsterIndex;
		lpObj->m_MonsterKill[4].m_KillCount = lpMsg->KillInfo[4].m_KillCount;

		lpObj->m_MonsterKillInfoLoaded = TRUE;

		LogAddTD("[Quest] Set MonsterKillInfo [%s][%s](%d) (%d : %d/%d, %d/%d, %d/%d, %d/%d, %d/%d)",
			lpObj->AccountID,lpObj->Name,lpObj->m_Index,lpObj->m_MonsterKillQuestIndex,
			lpObj->m_MonsterKill[0].m_MonsterIndex,lpObj->m_MonsterKill[0].m_KillCount,
			lpObj->m_MonsterKill[1].m_MonsterIndex,lpObj->m_MonsterKill[1].m_KillCount,
			lpObj->m_MonsterKill[2].m_MonsterIndex,lpObj->m_MonsterKill[2].m_KillCount,
			lpObj->m_MonsterKill[3].m_MonsterIndex,lpObj->m_MonsterKill[3].m_KillCount,
			lpObj->m_MonsterKill[4].m_MonsterIndex,lpObj->m_MonsterKill[4].m_KillCount);
	}
}

typedef struct
{
	PBMSG_HEAD h;
	int Index;
	char szName[11];
	int QuestIndex;
	QUEST_MONSTER_KILL_INFO KillInfo[5];
}PMSG_SAVE_MONSTERKILLINFO, *LPMSG_SAVE_MONSTERKILLINFO;

void CQuestMonsterKillInfo::SaveMonsterKillInfo(LPOBJ lpObj)
{
	if( lpObj->m_MonsterKillInfoLoaded == FALSE )
	{
		return;
	}

	PMSG_SAVE_MONSTERKILLINFO pMsg;
	PHeadSetB((LPBYTE)&pMsg,0x14,sizeof(pMsg));
	memcpy(pMsg.szName,lpObj->Name,10);

	pMsg.Index = lpObj->m_Index;
	pMsg.QuestIndex = lpObj->m_MonsterKillQuestIndex;
	pMsg.KillInfo[0].m_MonsterIndex = lpObj->m_MonsterKill[0].m_MonsterIndex;
	pMsg.KillInfo[0].m_KillCount = lpObj->m_MonsterKill[0].m_KillCount;
	pMsg.KillInfo[1].m_MonsterIndex = lpObj->m_MonsterKill[1].m_MonsterIndex;
	pMsg.KillInfo[1].m_KillCount = lpObj->m_MonsterKill[1].m_KillCount;
	pMsg.KillInfo[2].m_MonsterIndex = lpObj->m_MonsterKill[2].m_MonsterIndex;
	pMsg.KillInfo[2].m_KillCount = lpObj->m_MonsterKill[2].m_KillCount;
	pMsg.KillInfo[3].m_MonsterIndex = lpObj->m_MonsterKill[3].m_MonsterIndex;
	pMsg.KillInfo[3].m_KillCount = lpObj->m_MonsterKill[3].m_KillCount;
	pMsg.KillInfo[4].m_MonsterIndex = lpObj->m_MonsterKill[4].m_MonsterIndex;
	pMsg.KillInfo[4].m_KillCount = lpObj->m_MonsterKill[4].m_KillCount;

	LogAddTD("[Quest] Save MonsterKillInfo [%s][%s] (%d : %d/%d, %d/%d, %d/%d, %d/%d, %d/%d)",
		lpObj->AccountID,lpObj->Name,pMsg.QuestIndex,
			pMsg.KillInfo[0].m_MonsterIndex,pMsg.KillInfo[0].m_KillCount,
			pMsg.KillInfo[1].m_MonsterIndex,pMsg.KillInfo[1].m_KillCount,
			pMsg.KillInfo[2].m_MonsterIndex,pMsg.KillInfo[2].m_KillCount,
			pMsg.KillInfo[3].m_MonsterIndex,pMsg.KillInfo[3].m_KillCount,
			pMsg.KillInfo[4].m_MonsterIndex,pMsg.KillInfo[4].m_KillCount);

	cDBSMng.Send((char*)&pMsg,sizeof(pMsg));
}


#if (__QUEST_SYSTEM__ == 1)

CQuestInfoEx gQuestInfoEx;

CQuestInfoEx::CQuestInfoEx()
{
	
}

CQuestInfoEx::~CQuestInfoEx()
{

}

void CQuestInfoEx::LoadQuestProgress(char* lpszFileName)
{
	SMDFile = fopen(lpszFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[QuestSystem] Load file failed! (%s)",lpszFileName);
		return;
	}


	m_mapQuestProgress.clear();

	int Token,QuestGroup,QuestCount;
	QUESTEX_PROGRESS_GROUP	pTmp;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		memset(&pTmp,-1,sizeof(pTmp));
		pTmp.m_iQuestInfo = 0;

		QuestCount = 0;
		QuestGroup = TokenNumber;

		if( QuestGroup < 0 || QuestGroup >= MAX_QUESTEX_GROUP )
		{
			MsgBox("[QuestSystem] QuestGroup(%d) not in range! (%s)",QuestGroup,lpszFileName);
			break;
		}

		Token = GetToken();
		pTmp.NpcNumber1 = TokenNumber;

		Token = GetToken();
		pTmp.NpcNumber2 = TokenNumber;

		while( TRUE )
		{
			Token = GetToken();

			if( strcmp("end",TokenString) == NULL )
				break;

			if( QuestCount >= MAX_QUESTEX_PROGRESS )
				break;

			pTmp.QuestAction[QuestCount].QuestId = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].MinLevel = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].MaxLevel = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].btAction[0] = TokenNumber;
			Token = GetToken();
			pTmp.QuestAction[QuestCount].btValue[0] = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].btAction[1] = TokenNumber;
			Token = GetToken();
			pTmp.QuestAction[QuestCount].btValue[1] = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].btAction[2] = TokenNumber;
			Token = GetToken();
			pTmp.QuestAction[QuestCount].btValue[2] = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].btClass[CLASS_WIZARD] = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].btClass[CLASS_KNIGHT] = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].btClass[CLASS_ELF] = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].btClass[CLASS_MAGUMSA] = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].btClass[CLASS_DARKLORD] = TokenNumber;

			Token = GetToken();
			pTmp.QuestAction[QuestCount].btClass[CLASS_SUMMONER] = TokenNumber;

			QuestCount++;
		}

		pTmp.m_iQuestInfo = QuestCount;
		m_mapQuestProgress[QuestGroup] = pTmp;

		LogAddC(4,"[QuestSystem] QuestGroup(%d) Added (%d) quest strings",QuestGroup,QuestCount);
	}

	fclose(SMDFile);
}

void CQuestInfoEx::LoadQuestInfo(char* lpszFileName)
{
	SMDFile = fopen(lpszFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[QuestSystem] Load file failed! (%s)",lpszFileName);
		return;
	}

	m_mapQuestInfo.clear();

	int Token,Group,Type;
	QUESTEX_GROUP_INFO	pTmp;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		Group = TokenNumber;
		int Count = 0;

		memset(&pTmp,0,sizeof(pTmp));

		while( TRUE )
		{
			Token = GetToken();

			if( !strcmp("end",TokenString) )
				break;

			if( Count < 0 || Count > MAX_QUESTEX_INFO )
			{
				MsgBox("[QuestSystem] QuestCount in group exceded MAX_QUESTEX_INFO! (%s)",lpszFileName);
				return;
			}

			QUESTEX_INFO* lpQuest = &pTmp.QuestInfo[Count];

			lpQuest->QuestIndex = TokenNumber;
			int iCondition = 0,iReward = 0;

			while( TRUE )
			{
				Token = GetToken();

				if( !strcmp("end",TokenString) )
					break;

				Type = TokenNumber;

				if( Type == 0 )
				{
					if( iCondition < 0 || iCondition >= MAX_QUESTEX_CONDITION )
					{
						MsgBox("[QuestSystem] iCondition out of bounds! (%s)",lpszFileName);
						return;
					}

					Token = GetToken();
					lpQuest->QuestCondition[iCondition].Type = TokenNumber;

					Token = GetToken();
					lpQuest->QuestCondition[iCondition].reqType = TokenNumber;

					Token = GetToken();
					lpQuest->QuestCondition[iCondition].reqSubType = TokenNumber;

					Token = GetToken();
					lpQuest->QuestCondition[iCondition].reqLevel = TokenNumber;

					Token = GetToken();
					lpQuest->QuestCondition[iCondition].reqCount = TokenNumber;

					iCondition++;
				}
				else if( Type == 1 )
				{
					if( iReward < 0 || iReward >= MAX_QUESTEX_REWARD )
					{
						MsgBox("[QuestSystem] iReward out of bounds! (%s)",lpszFileName);
						return;
					}

					Token = GetToken();
					lpQuest->QuestReward[iReward].Type = TokenNumber;

					Token = GetToken();
					lpQuest->QuestReward[iReward].rewType = TokenNumber;

					Token = GetToken();
					lpQuest->QuestReward[iReward].rewSubType = TokenNumber;

					Token = GetToken();
					lpQuest->QuestReward[iReward].rewLevel = TokenNumber;

					Token = GetToken();
					lpQuest->QuestReward[iReward].rewCount = TokenNumber;
					iReward++;
				}
				else if( Type == 2 )
				{
					Token = GetToken();
					lpQuest->btNextState[0] = TokenNumber;

					Token = GetToken();
					lpQuest->btNextState[1] = TokenNumber;

					Token = GetToken();
					lpQuest->btNextState[2] = TokenNumber;

					Token = GetToken();
					lpQuest->btNextState[3] = TokenNumber;

					Token = GetToken();
					lpQuest->btNextState[4] = TokenNumber;

					Token = GetToken();
					lpQuest->btNextState[5] = TokenNumber;
				}
			}
			Count++;
		}

		pTmp.Count = Count;
		m_mapQuestInfo[Group] = pTmp;

		LogAddC(4,"[QuestSystem] Group(%d) Added [%d] quests!",Group,Count);
	}

	fclose(SMDFile);
}

struct PMSG_ANS_QUESTEXINFO
{
	PWMSG_HEAD h;
	int aIndex;
	char szName[11];

	BYTE btResult;
	QUESTEX_USER	QuestInfo[20];
};

void CQuestInfoEx::DGAnsQuestInfo(LPBYTE aRecv)
{
	if( aRecv == NULL )
		return;

	PMSG_ANS_QUESTEXINFO* lpMsg = (PMSG_ANS_QUESTEXINFO*)aRecv;

	if( !gObjIsConnected(lpMsg->aIndex))
		return;

	int aIndex = lpMsg->aIndex;

	if( strcmp(gObj[aIndex].Name,lpMsg->szName) != NULL )
		return;

	if( lpMsg->btResult != TRUE )
	{
		LogAddC(2,"[QuestSystem][ERROR] QuestInfo load failed! (%s)",lpMsg->szName);
		return;
	}
	else
	{
		gObj[aIndex].m_QuestEx.btInit = TRUE;
		memcpy(gObj[aIndex].m_QuestEx.Quest,lpMsg->QuestInfo,sizeof(lpMsg->QuestInfo));
	}


	PBMSG_HEAD2 pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xF6,0x20,sizeof(pMsg));
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.size);
}

struct PMSG_QUESTEX_INIT
{
	PBMSG_HEAD2 h;
	WORD wNpc;
	WORD wCount;
};

void CQuestInfoEx::GCQuestInit(int aIndex)
{
	if( gObj[aIndex].Class == CLASS_MAGUMSA || 
		gObj[aIndex].Class == CLASS_DARKLORD ||
		
		gObj[aIndex].Level > 1
		
#if(__CUSTOM__ == 1)
		|| gObj[aIndex].iResetCount != 0
#endif
		)
	{
		gObj[aIndex].m_QuestEx.Quest[0].btStartQuest = -1;
		gObj[aIndex].m_QuestEx.Quest[0].btCurQuest = -1;
		gObj[aIndex].m_QuestEx.Quest[1].btStartQuest = -1;
		gObj[aIndex].m_QuestEx.Quest[1].btCurQuest = -1;
		return;
	}

	BYTE btTmp[128] = {0};
	PMSG_QUESTEX_INIT* lpMsg = (PMSG_QUESTEX_INIT*)&btTmp[0];
	QUESTEX_STATE_INFO* lpMsgBody = (QUESTEX_STATE_INFO*)&btTmp[sizeof(PMSG_QUESTEX_INIT)];

	PHeadSubSetB((LPBYTE)lpMsg,0xF6,0x03,sizeof(PMSG_QUESTEX_INIT));
	lpMsg->wNpc = 0;
	lpMsg->wCount = 1;
	lpMsgBody[0].aGroup = 1;
	lpMsgBody[0].aQuest = 0;

	lpMsg->h.size += sizeof(QUESTEX_STATE_INFO)*lpMsg->wCount;

	DataSend(aIndex,(LPBYTE)lpMsg,lpMsg->h.size);
}

void CQuestInfoEx::CGRequestQuestState(int aIndex)
{
	BYTE btTmp[1024] = {0};
	PMSG_QUESTEX_ANS_STATE* lpMsg = (PMSG_QUESTEX_ANS_STATE*)&btTmp[0];
	QUESTEX_STATE_INFO* lpMsgBody = (QUESTEX_STATE_INFO*)&btTmp[sizeof(PMSG_QUESTEX_ANS_STATE)];

	PHeadSubSetB((LPBYTE)lpMsg,0xF6,0x1A,sizeof(PMSG_QUESTEX_ANS_STATE));
	lpMsg->btCount = 0;

	LPOBJ lpObj = &gObj[aIndex];
	QUESTEX_USER* lpQuestUser = &lpObj->m_QuestEx.Quest[0];

	if( lpObj->m_QuestEx.btInit == TRUE )
	{
		for(int i = 0; i < MAX_QUESTEX_GROUP; i++)
		{
			if( lpQuestUser[i].btState != QUESTEX_NONE )
			{
				lpMsgBody[lpMsg->btCount].aGroup = i;
				lpMsgBody[lpMsg->btCount].aQuest = lpQuestUser[i].btCurQuest;
				lpMsg->btCount++;
			}
		}
	}

	lpMsg->h.size = sizeof(PMSG_QUESTEX_ANS_STATE) + sizeof(QUESTEX_STATE_INFO)*lpMsg->btCount;

	DataSend(aIndex,(LPBYTE)lpMsg,lpMsg->h.size);

	if( gObj[aIndex].m_QuestEx.Quest[1].btCurQuest == 0)
	{
		GCQuestInit(aIndex);
	}
}

struct PMSG_QUESTEX_ANS_ACTION
{
	PBMSG_HEAD2 h;
	QUESTEX_STATE_INFO Quest;
	BYTE btUnk[3];
};

struct PMSG_QUESTEX_REQ_ACTION
{
	PBMSG_HEAD2 h;
	QUESTEX_STATE_INFO Quest;
	BYTE btState;
};

void CQuestInfoEx::CGRequestInitQuest(LPBYTE aRecv,int aIndex)
{
	PMSG_QUESTEX_REQ_ACTION* lpMsg = (PMSG_QUESTEX_REQ_ACTION*)aRecv;

	int QuestState = (lpMsg->btState == 0) ? 1 : (lpMsg->btState-1);

	if( QuestState < 0 || QuestState >= MAX_QUESTEX_ACTION )
	{
		return;
	}

	QUESTEX_USER* lpQuestUser;
	int QuestGroup = lpMsg->Quest.aGroup;
	int QuestId = lpMsg->Quest.aQuest;

	LPOBJ lpObj = &gObj[aIndex];
	LP_QUESTEX_ACTION lpAction = GetQuestAction(QuestGroup,QuestId,lpObj);

	if( lpAction == NULL )
	{
		QUESTEX_INFO* lpQuestInfo = GetQuestInfo(QuestGroup,QuestId);

		if( lpQuestInfo == NULL )
		{
			LogAddC(2,"[QuestSystem] QuestAction not found!(Group:%d,Id:%d)",
				QuestGroup,QuestId);
			
		}
		else
		{
			lpQuestUser = &gObj[aIndex].m_QuestEx.Quest[QuestGroup];

			if( lpQuestUser->btState == QUESTEX_START )
			{
				GCSendQuestStart(aIndex,QuestGroup,QuestId);
			}
		}
		return;
	}

	lpQuestUser = &gObj[aIndex].m_QuestEx.Quest[QuestGroup];

	if( lpQuestUser->btState == QUESTEX_NONE )
	{
		lpQuestUser->btStartQuest = QuestId;
		lpQuestUser->btState = QUESTEX_ACCEPT;
	}

	lpQuestUser->btCurQuest = QuestId;

	int Action = lpAction->btAction[0];
	int ActValue = lpAction->btValue[0];

	if( Action == ACTION_START && lpAction->btAction[1] != ACTION_START )
	{
		memset(gObj[aIndex].m_QuestEx.Quest[QuestGroup].wQuestInfo,0,5);
		GCSendQuestStart(aIndex,QuestGroup,ActValue);
	}
	else
	{
		GCSendQuestAction(aIndex,QuestGroup,QuestId);
	}
}

void CQuestInfoEx::CGRequestQuestStep(LPBYTE aRecv,int aIndex)
{
	PMSG_QUESTEX_REQ_ACTION* lpMsg = (PMSG_QUESTEX_REQ_ACTION*)aRecv;

	int QuestState = (lpMsg->btState == 0) ? 1 : (lpMsg->btState-1);

	if( QuestState < 0 || QuestState >= MAX_QUESTEX_ACTION )
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->Type != OBJ_USER )
	{
		return;
	}

	int QuestGroup = lpMsg->Quest.aGroup;
	int QuestId = lpMsg->Quest.aQuest;

	
	LP_QUESTEX_ACTION lpAction = GetQuestAction(QuestGroup,QuestId,lpObj);

	if( lpAction == NULL )
	{
		LogAddC(2,"[QuestSystem] QuestAction not found!(Group:%d,Id:%d)",
			QuestGroup,QuestId);
		return;
	}

	int Action = lpAction->btAction[QuestState];
	int ActValue = lpAction->btValue[QuestState];

	switch( Action )
	{
	case ACTION_START:
		memset(gObj[aIndex].m_QuestEx.Quest[QuestGroup].wQuestInfo,0,5);
		GCSendQuestStart(aIndex,QuestGroup,ActValue);
		break;
	case ACTION_MSG:
		GCSendQuestAction(aIndex,QuestGroup,ActValue);
		break;
	}
}

void CQuestInfoEx::GCSendQuestAction(int aIndex,int QuestGroup,int QuestIndex)
{
	PMSG_QUESTEX_ANS_ACTION pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF6,0x0B,sizeof(pMsg));

	pMsg.Quest.aGroup = QuestGroup;
	pMsg.Quest.aQuest = QuestIndex;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}



void CQuestInfoEx::GCSendQuestStart(int aIndex,int QuestGroup,int QuestIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if(!gObjIsConnected(aIndex) || lpObj->Type != OBJ_USER )
		return;

	QUESTEX_INFO* lpQuest = GetQuestInfo(QuestGroup,QuestIndex);

	if( lpQuest == NULL )
	{
		return;
	}

	QUESTEX_USER* lpQuestUser = &lpObj->m_QuestEx.Quest[QuestGroup];

	PMSG_QUESTEX_CONDITION pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF6,0x0C,sizeof(pMsg));

	pMsg.Quest.aGroup = QuestGroup;

	CheckConditions(lpQuest,lpQuestUser,aIndex);
	CreateInfoMsg(&pMsg,lpQuest,lpObj);

	pMsg.Quest.aQuest = QuestIndex;
	
	lpQuestUser->btCurQuest = QuestIndex;
	lpQuestUser->btState = QUESTEX_START;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

struct PMSG_REQ_QUESTEX_COMPLETE
{
	PBMSG_HEAD2	h;
	QUESTEX_STATE_INFO	Quest;
};

struct PMSG_ANS_QUESTEX_COMPLETE
{
	PBMSG_HEAD2	h;
	QUESTEX_STATE_INFO	Quest;
	BYTE btResult;
};

void CQuestInfoEx::CGQuestComplete(LPBYTE aRecv,int aIndex)
{
	PMSG_REQ_QUESTEX_COMPLETE* lpMsg = (PMSG_REQ_QUESTEX_COMPLETE*)aRecv;

	LPOBJ lpObj = &gObj[aIndex];

	if( !gObjIsConnected(aIndex) || lpObj->Type != OBJ_USER )
	{
		return;
	}

	if( lpObj->m_IfState.use != 0 )
		return;

	int QuestGroup = lpMsg->Quest.aGroup;
	int QuestIndex = lpMsg->Quest.aQuest;

	QUESTEX_INFO* lpQuest = GetQuestInfo(QuestGroup,QuestIndex);
	QUESTEX_USER* lpQuestUser = &lpObj->m_QuestEx.Quest[QuestGroup];

	if( lpQuest == NULL )
		return;

	if( lpQuestUser->btState != QUESTEX_START || lpQuestUser->btCurQuest != QuestIndex )
	{
		return;
	}

	if( !CheckConditions(lpQuest,lpQuestUser,aIndex) )
	{
		return;
	}
	else
	{
		GiveReward(lpQuest,aIndex);

		lpQuestUser->btStartQuest = lpQuest->btNextState[lpObj->Class];
		lpQuestUser->btCurQuest = lpQuest->btNextState[lpObj->Class];
		lpQuestUser->btState = QUESTEX_NONE;
	}

	PMSG_ANS_QUESTEX_COMPLETE pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF6,0x0D,sizeof(pMsg));

	pMsg.Quest.aGroup = QuestGroup;
	pMsg.Quest.aQuest = QuestIndex;
	pMsg.btResult = TRUE;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void CQuestInfoEx::CGQuitQuest(LPBYTE aRecv,int aIndex)
{
	PMSG_REQ_QUESTEX_COMPLETE* lpMsg = (PMSG_REQ_QUESTEX_COMPLETE*)aRecv;
	LPOBJ lpObj = &gObj[aIndex];

	int QuestGroup = lpMsg->Quest.aGroup;
	int QuestIndex = lpMsg->Quest.aQuest;

	if( QuestGroup < 0 || QuestGroup >= MAX_QUESTEX_GROUP )
		return;

	if( lpObj->m_QuestEx.Quest[QuestGroup].btCurQuest == QuestIndex )
	{

	}

	PMSG_REQ_QUESTEX_COMPLETE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xF6,0x0F,sizeof(pMsg));

	pMsg.Quest.aGroup = QuestGroup;
	pMsg.Quest.aQuest = 0;

	lpObj->m_QuestEx.Quest[QuestGroup].btState = QUESTEX_NONE;
	lpObj->m_QuestEx.Quest[QuestGroup].btCurQuest = lpObj->m_QuestEx.Quest[QuestGroup].btStartQuest;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

QUESTEX_PROGRESS_ACTION* CQuestInfoEx::GetQuestAction(int QuestGroup,int QuestId,LPOBJ lpObj)
{
	std::map<BYTE,QUESTEX_PROGRESS_GROUP>::iterator _It = m_mapQuestProgress.find(QuestGroup);

	if( _It != m_mapQuestProgress.end() )
	{
		QUESTEX_PROGRESS_GROUP* lpGroup = &_It->second;
		for(int i = 0; i < lpGroup->m_iQuestInfo; i++)
		{
			if( lpGroup->QuestAction[i].QuestId == QuestId )
			{
				if( lpObj->Level >= lpGroup->QuestAction[i].MinLevel && lpObj->Level <= lpGroup->QuestAction[i].MaxLevel )
				{
					BYTE btClass = lpGroup->QuestAction[i].btClass[lpObj->Class];
	
					if( btClass == 1 ||
						(btClass == 2 && lpObj->ChangeUP ) ||
						(btClass == 3 && lpObj->ChangeUP2 ) )
					{
						return &lpGroup->QuestAction[i];
					}
				}
			}
		}
	}

return 0;
}

QUESTEX_PROGRESS_ACTION* CQuestInfoEx::GetQuestAction(QUESTEX_PROGRESS_GROUP* lpQuestGroup,LPOBJ lpObj)
{
	if( lpQuestGroup == NULL )
		return 0;

	for(int i = 0; i < lpQuestGroup->m_iQuestInfo; i++)
	{
		if( lpObj->Level >= lpQuestGroup->QuestAction[i].MinLevel && lpObj->Level <= lpQuestGroup->QuestAction[i].MaxLevel )
		{
			BYTE btClass = lpQuestGroup->QuestAction[i].btClass[lpObj->Class];

			if( btClass == 1 ||
				(btClass == 2 && lpObj->ChangeUP ) ||
				(btClass == 3 && lpObj->ChangeUP2 ) )
			{
				return &lpQuestGroup->QuestAction[i];
			}
		}
	}

return 0;
}

QUESTEX_INFO* CQuestInfoEx::GetQuestInfo(int QuestGroup,int QuestId)
{
	std::map<BYTE,QUESTEX_GROUP_INFO>::iterator _It = m_mapQuestInfo.find(QuestGroup);

	if( _It != m_mapQuestInfo.end() )
	{
		QUESTEX_GROUP_INFO* lpQuestGroup = &_It->second;

		for(int i = 0; i < lpQuestGroup->Count; i++)
		{
			if( lpQuestGroup->QuestInfo[i].QuestIndex == QuestId )
			{
				return &lpQuestGroup->QuestInfo[i];
			}
		}
	}
return 0;
}

BOOL CQuestInfoEx::CheckConditions(QUESTEX_INFO* lpQuest,QUESTEX_USER* lpQuestUser,int aIndex)
{
	if( lpQuest == NULL || lpQuestUser == NULL)
		return FALSE;

	LPOBJ lpObj = &gObj[aIndex];

	int iCondition = 0,iReward = 0;

	for(int i = 0; i < MAX_QUESTEX_CONDITION; i++)
	{
		QUESTEX_INFO_REQUIREMENT* lpCondition = &lpQuest->QuestCondition[i];
		if( lpCondition->Type == 0 )
			continue;

		if( lpCondition->Type == 2 )
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(lpObj,lpCondition->reqType);

			if( lpMagic != NULL )
			{
				lpQuestUser->wQuestInfo[i] = TRUE;
			}
		}
		else if( lpCondition->Type == 4 )
		{
			int type = ITEMGET(lpCondition->reqType,lpCondition->reqSubType);
			lpQuestUser->wQuestInfo[iCondition] = GetItemCount(lpObj,type,lpCondition->reqLevel);
		}
		else if( lpCondition->Type == 8 )
		{
			lpQuestUser->wQuestInfo[i] = lpObj->Level;
		}
		
		if( lpQuestUser->wQuestInfo[i] < lpCondition->reqCount )
		{
			return false;
		}
	}


	return TRUE;
}

BOOL CQuestInfoEx::GiveReward(QUESTEX_INFO* lpQuest,int aIndex)
{

	LPOBJ lpObj = &gObj[aIndex];

	for(int i = 0; i < MAX_QUESTEX_REWARD; i++)
	{
		QUESTEX_INFO_REWARD* lpReward = &lpQuest->QuestReward[i];

		if( lpReward->Type == 0 )
			continue;

		switch( lpReward->Type )
		{
		case 1:
			{
				__int64 Exp = lpReward->rewCount;

				if( !g_MasterExp.IsEnabled(lpObj) )
				{
					lpObj->Experience += Exp;
				}

				int bRet = gObjLevelUp(lpObj, &Exp, 0, 0);

				if( bRet == TRUE )
				{
					GCKillPlayerExpSend(lpObj->m_Index, (WORD)-1, lpReward->rewCount, 0, 0);
				}
			}
			break;
		case 2:
			{
				lpObj->Money += lpReward->rewCount;

				if( lpObj->Money > MAX_ZEN )
					lpObj->Money = MAX_ZEN;

				GCMoneySend(aIndex,lpObj->Money);
			}
			break;
		case 4:
			{
				int map = lpObj->MapNumber;
				int w = 1,h = 1;
				int index = ITEMGET(lpReward->rewType,lpReward->rewSubType);

				int dur = 0;
				int ItemCount = lpReward->rewCount;

				if( (index >= ITEMGET(14,0) && index <= ITEMGET(14,6) ) ||
					(index >= ITEMGET(14,35) && index <= ITEMGET(14,40) ) )
				{
					ItemCount = 1;
					dur = lpReward->rewCount;
				}

				ItemGetSize(index,w,h);

				int Option1 = 1;
				int Option2 = 1;
				int Option3 = 1;

				if( index >= ITEMGET(12,0) )
				{
					Option3 = 0;
				}

				if( CheckInventoryEmptySpace(lpObj,h*lpReward->rewCount,w*lpReward->rewCount) )
				{
					map = 237;
				}

				for(int i = 0; i < ItemCount; i++)
				{
					ItemSerialCreateSend(lpObj->m_Index, map, lpObj->X,lpObj->Y,index,lpReward->rewLevel,dur,Option1,Option2,Option3,lpObj->m_Index,0,0);
				}
			}
			break;
#if (__GENS_SYSTEM__ == 1)
		case 16:
			{
				if( lpObj->m_GensInfo.btFamily != GENS_NONE )
				{
					lpObj->m_GensInfo.iContribution += lpReward->rewCount;
				}
			}
			break;
#endif
		}
	}
	return true;
}

void CQuestInfoEx::CreateInfoMsg(PMSG_QUESTEX_CONDITION* lpMsg,QUESTEX_INFO* lpQuest,LPOBJ lpObj)
{
	int iCondition = 0,iReward = 0;

	QUESTEX_USER* lpQuestUser = &lpObj->m_QuestEx.Quest[lpMsg->Quest.aGroup];

	for(int i = 0; i < MAX_QUESTEX_CONDITION; i++)
	{
		QUESTEX_INFO_REQUIREMENT* lpCondition = &lpQuest->QuestCondition[i];
		if( lpCondition->Type != 0 )
		{
			QUESTEX_CONDITION_INFO* lpMsgBody = &lpMsg->pCondition[iCondition];

			lpMsgBody->iType = lpCondition->Type;
			lpMsgBody->Count = lpCondition->reqCount;
			lpMsgBody->CurCount = lpQuestUser->wQuestInfo[i];

			if( lpCondition->Type == 4 )
			{
				CItem pItem;

				int type = ITEMGET(lpCondition->reqType,lpCondition->reqSubType);

				int Option1 = 1;
				int Option2 = 1;
				int Option3 = 1;

				if( type >= ITEMGET(12,0) )
				{
					Option3 = 0;
				}

				pItem.Convert(type,Option1,Option2,Option3,0,0,0,0,-1,CURRENT_DB_VERSION);

				pItem.m_Level = lpCondition->reqLevel;
				pItem.m_Durability = ItemGetDurability(type,pItem.m_Level,0,0);

				lpMsgBody->ReqId = type;

				ItemByteConvert(lpMsgBody->ItemInfo,pItem);
			}
			else
			{
				lpMsgBody->ReqId = lpCondition->reqType;
			}


			iCondition++;

			if( iCondition >= 5 )
				break;
		}
	}

	for (int i = 0; i < MAX_QUESTEX_REWARD; i++)
	{
		QUESTEX_INFO_REWARD* lpReward = &lpQuest->QuestReward[i];

		if( lpReward->Type != 0 )
		{
			QUESTEX_REWARD_INFO* lpMsgBody = &lpMsg->pReward[iReward];
			lpMsgBody->iType = lpReward->Type;
			lpMsgBody->Count = lpReward->rewCount;

			if( lpReward->Type == 4 )
			{
				CItem pItem;

				int type = ITEMGET(lpReward->rewType,lpReward->rewSubType);

				int Option1 = TRUE;
				int Option2 = rand()%2;
				int Option3 = rand()%7;

				if( type >= ITEMGET(12,0) )
				{
					Option3 = 0;
				}

				pItem.Convert(type,Option1,Option2,Option3,0,0,0,0,-1,CURRENT_DB_VERSION);

				pItem.m_Level = lpReward->rewLevel;
				pItem.m_Durability = ItemGetDurability(type,pItem.m_Level,0,0);

				lpMsgBody->RewardId = type;

				ItemByteConvert(lpMsgBody->ItemInfo,pItem);
			}
			else
			{
				lpMsgBody->RewardId = lpReward->rewType;
			}

			iReward++;

			if( iReward >= 5 )
				break;
		}
	}

	lpMsg->btConCount = iCondition;
	lpMsg->btRewCount = iReward;
}

struct PMSG_QUESTEX_ANS_QUESTLIST
{
	PBMSG_HEAD2	h;
	WORD wNpcId;
	WORD wCount;
};

void CQuestInfoEx::GCSendAvaibleQuest(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	BYTE btTmp[1024] = {0};
	PMSG_QUESTEX_ANS_QUESTLIST* lpMsg = (PMSG_QUESTEX_ANS_QUESTLIST*)&btTmp[0];
	QUESTEX_STATE_INFO* lpMsgBody = (QUESTEX_STATE_INFO*)&btTmp[sizeof(PMSG_QUESTEX_ANS_QUESTLIST)];

	PHeadSubSetB((LPBYTE)lpMsg,0xF6,0x0A,sizeof(PMSG_QUESTEX_ANS_QUESTLIST));
	lpMsg->wCount = 0;
	lpMsg->wNpcId = lpObj->TargetShopNumber;

	std::map<BYTE,QUESTEX_PROGRESS_GROUP>::iterator _It = m_mapQuestProgress.begin();

	for( ; _It != m_mapQuestProgress.end(); _It++ )
	{
		BYTE btQuestGroup = _It->first;
		QUESTEX_PROGRESS_GROUP* lpQuestGroup = &_It->second;

		if( lpQuestGroup->NpcNumber1 == lpObj->TargetShopNumber ||
			lpQuestGroup->NpcNumber2 == lpObj->TargetShopNumber )
		{
			if( lpObj->m_QuestEx.Quest[btQuestGroup].btStartQuest == (BYTE)-1 )
				continue;

#if ( __GENS_SYSTEM__ == 1 )

			if( lpObj->TargetShopNumber == 543 && lpObj->m_GensInfo.btFamily != GENS_DUPRIAN )
			{
				continue;
			}
			else if( lpObj->TargetShopNumber == 544 && lpObj->m_GensInfo.btFamily != GENS_VANERT )
			{
				continue;
			}

#endif

			QUESTEX_USER* lpQuestUser = &lpObj->m_QuestEx.Quest[btQuestGroup];

			if( btQuestGroup == 1 && lpQuestUser->btCurQuest < 5 )
				continue;

			if( lpQuestUser->btState == QUESTEX_START )
			{
				lpMsgBody[lpMsg->wCount].aQuest = lpQuestUser->btCurQuest;
			}
			else
			{
				LP_QUESTEX_ACTION lpAction = GetQuestAction(btQuestGroup,lpQuestUser->btCurQuest,lpObj);

				if( lpAction == NULL )
				{
					lpAction = GetQuestAction(lpQuestGroup,lpObj);

					if( lpAction == NULL )
					{
						continue;
					}
					else
					{
						lpMsgBody[lpMsg->wCount].aQuest = lpAction->QuestId;
					}
				}
				else
				{
					lpMsgBody[lpMsg->wCount].aQuest = lpAction->QuestId;
				}
			}

			lpMsgBody[lpMsg->wCount].aGroup = btQuestGroup;
			lpMsg->wCount++;
		}
	}

	lpMsg->h.size = sizeof(PMSG_QUESTEX_ANS_QUESTLIST) + lpMsg->wCount*sizeof(QUESTEX_STATE_INFO);

	DataSend(aIndex,(LPBYTE)lpMsg,lpMsg->h.size);
}

struct PMSG_QUESTEX_CLIENTQUEST
{
	PBMSG_HEAD2	h;
	QUESTEX_STATE_INFO Quest;
};

void CQuestInfoEx::CGClientAction(LPBYTE aRecv,int aIndex)
{
	if( aRecv == NULL )
		return;
	LPOBJ lpObj= &gObj[aIndex];
	PMSG_QUESTEX_CLIENTQUEST* lpMsg = (PMSG_QUESTEX_CLIENTQUEST*)aRecv;

	int QuestGroup = lpMsg->Quest.aGroup;
	int QuestIndex = lpMsg->Quest.aQuest;

	QUESTEX_INFO* lpQuestInfo = GetQuestInfo(QuestGroup,QuestIndex);

	if( lpQuestInfo == NULL )
	{
		LogAddC(2,"[QuestSystem] QuestInfo not found(%d,%d) [%s][%s]",
			QuestGroup,QuestIndex,lpObj->AccountID,lpObj->Name);
		return;
	}

	QUESTEX_USER* lpQuestUser = &lpObj->m_QuestEx.Quest[QuestGroup];

	int iCondition = 0;

	for(int i = 0; i < MAX_QUESTEX_CONDITION; i++)
	{
		if( lpQuestInfo->QuestCondition[i].Type == 0x10 )
		{
			lpQuestUser->wQuestInfo[iCondition] = TRUE;
		}

		iCondition++;

		if( iCondition >= 5 )
			break;
	}
}

void CQuestInfoEx::CGReqQuestInfo(LPBYTE aRecv,int aIndex)
{
	PMSG_QUESTEX_CLIENTQUEST* lpMsg = (PMSG_QUESTEX_CLIENTQUEST*)aRecv;

	int QuestGroup = lpMsg->Quest.aGroup;
	int QuestIndex = lpMsg->Quest.aQuest;

	QUESTEX_INFO* lpQuestInfo = GetQuestInfo(QuestGroup,QuestIndex);

	if( lpQuestInfo == NULL )
	{
		return;
	}
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_QUESTEX_CONDITION pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg,0xF6,0x1B,sizeof(pMsg));

	pMsg.Quest.aGroup = QuestGroup;

	CheckConditions(lpQuestInfo,&lpObj->m_QuestEx.Quest[QuestGroup],aIndex);
	CreateInfoMsg(&pMsg,lpQuestInfo,lpObj);

	pMsg.Quest.aQuest = QuestIndex;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

int CQuestInfoEx::GetItemCount(LPOBJ lpObj,int ItemIndex,int ItemLevel)
{
	int count = 0;

	for ( int n =INVETORY_WEAR_SIZE;n<MAIN_INVENTORY_SIZE ; n++ )
	{
		if ( lpObj->pInventory[n].IsItem() == TRUE )
		{
			if ( lpObj->pInventory[n].m_Type == ItemIndex )
			{
				if (  lpObj->pInventory[n].m_Level != ItemLevel )
				{
					continue;
				}
				
				count += lpObj->pInventory[n].m_Durability;
			}
		}
	}

	return count;
}

void CQuestInfoEx::GCSendBuff(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->Level > g_NPCBuffLevel || lpObj->ChangeUP2 != FALSE )
	{
		GCServerCmd(lpObj->m_Index, 0x0D, 0, 0);
		return;
	}

#if (__CUSTOM__ == 1)
	int Defense = lpObj->Level / g_ElfBuffDefenseDiv + g_ElfBuffDefenseStart;
	int Attack = lpObj->Level / g_ElfBuffAttackDiv + g_ElfBuffAttackStart;

	gObjAddBuffEffect(lpObj,BUFF_NPC_PHANTOM,ADD_OPTION_ATTACK_DAMAGE,Attack,ADD_OPTION_DEFENSE,Defense,g_ElfBuffTime);
#else
	int Defense = lpObj->Level / 5 + 50;
	int Attack = lpObj->Level / 3 + 45;

	gObjAddBuffEffect(lpObj,BUFF_NPC_PHANTOM,ADD_OPTION_ATTACK_DAMAGE,Attack,ADD_OPTION_DEFENSE,Defense,1800000);
#endif
}

int CQuestInfoEx::MonsterKill(LPOBJ lpNpc,LPOBJ lpObj)
{
	if( !gObjIsConnected(lpObj->m_Index) || lpObj->Type != OBJ_USER)
	{
		return false;
	}

	QUESTEX_USER* lpQuestUser = &lpObj->m_QuestEx.Quest[0];

	for(int i = 0; i < MAX_QUESTEX_GROUP; i++)
	{
		if( lpQuestUser[i].btState != QUESTEX_START )
			continue;
		
		int QuestGroup = i;
		int QuestIndex = lpQuestUser[i].btCurQuest;

		QUESTEX_INFO* lpQuestInfo = GetQuestInfo(QuestGroup,QuestIndex);

		if( lpQuestInfo == FALSE )
			continue;

		for(int n = 0; n < MAX_QUESTEX_CONDITION; n++)
		{
			if( lpQuestInfo->QuestCondition[n].Type != 1 )
				continue;

			if( lpNpc->Class == lpQuestInfo->QuestCondition[n].reqType &&
				lpQuestUser[i].wQuestInfo[n] < lpQuestInfo->QuestCondition[n].reqCount )
			{
				lpQuestUser[i].wQuestInfo[n]++;
				return true;
			}
		}
	}

return false;
}

struct PMSG_REQ_QUESTEXINFO_SAVE
{
	PWMSG_HEAD h;
	char szName[11];

	QUESTEX_USER	QuestInfo[MAX_QUESTEX_GROUP];
};

void CQuestInfoEx::GDSaveQuestInfo(LPOBJ lpObj)
{
	if( lpObj->m_QuestEx.btInit == FALSE )
		return;

	PMSG_REQ_QUESTEXINFO_SAVE pMsg = {0};
	PHeadSetW((LPBYTE)&pMsg, 0x15, sizeof(pMsg));

	strcpy_s(pMsg.szName, sizeof(pMsg.szName), lpObj->Name);
	memcpy(pMsg.QuestInfo,&lpObj->m_QuestEx.Quest[0],sizeof(pMsg.QuestInfo));

	cDBSMng.Send((char*)&pMsg,sizeof(pMsg));
}

struct PMSG_REQ_QUESTEXINFO
{
	PBMSG_HEAD h;
	int aIndex;
	char szName[11];
};

void CQuestInfoEx::GDReqQuestInfo(LPOBJ lpObj)
{
	PMSG_REQ_QUESTEXINFO pMsg = {0};
	PHeadSetB((LPBYTE)&pMsg,0x16,sizeof(pMsg));

	pMsg.aIndex = lpObj->m_Index;
	strcpy_s(pMsg.szName, sizeof(pMsg.szName), lpObj->Name);

	cDBSMng.Send((char*)&pMsg,sizeof(pMsg));

	LogAddC(4,"[QuestSystem] Request info! (%s)",lpObj->Name);
}

#endif
