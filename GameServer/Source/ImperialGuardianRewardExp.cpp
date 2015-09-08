#include "StdAfx.h"
#include "ImperialGuardianRewardExp.h"
#include "GameMain.h"
#include "logproc.h"
#include "..\include\ReadScript.h"

CImperialGuardianRewardExp::CImperialGuardianRewardExp()
{
	Init();
}

CImperialGuardianRewardExp::~CImperialGuardianRewardExp()
{
	if( !m_vtRewardExpInfo.empty() )
	{
		std::vector<_stRewardExpInfo>::iterator _It = m_vtRewardExpInfo.begin();

		for( ; _It != m_vtRewardExpInfo.end(); _It++)
		{
			if( _It->pEventDugeonItemBag != NULL )
				delete _It->pEventDugeonItemBag;

			if( _It->pEventDugeonItemBagStone != NULL )
				delete _It->pEventDugeonItemBagStone;

			if( _It->pEventDugeonItemBagGaion != NULL )
				delete _It->pEventDugeonItemBagGaion;
		}
	}
}

void CImperialGuardianRewardExp::LoadScript(char* lpFileName)
{
	int Token;
	this->m_bLoaded = false;

	if( lpFileName == NULL )
	{
		LogAdd("[IMPERIALGUARDIAN][REWARDEXP] lpFileName is null!!");
		return;
	}

	SMDFile = fopen(lpFileName,"r");

	if( SMDFile == NULL )
	{
		LogAdd(lMsg.Get(MSGGET(1,197)),lpFileName);
		return;
	}

	if( !m_vtRewardExpInfo.empty() )
	{
		std::vector<_stRewardExpInfo>::iterator _It = m_vtRewardExpInfo.begin();

		for( ; _It != m_vtRewardExpInfo.end(); _It++)
		{
			if( _It->pEventDugeonItemBag != NULL )
				delete _It->pEventDugeonItemBag;

			if( _It->pEventDugeonItemBagStone != NULL )
				delete _It->pEventDugeonItemBagStone;

			if( _It->pEventDugeonItemBagGaion != NULL )
				delete _It->pEventDugeonItemBagGaion;
		}
	}

	m_vtRewardExpInfo.clear();

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		if( Token == NUMBER )
		{
			int nType = TokenNumber;

			if( nType == 0 )
			{
				while( TRUE )
				{
					Token = GetToken();

					if( Token == NAME )
					{
						if( !strcmp("end",TokenString) )
							break;
					}

					_stRewardExpInfo rewardExpInfo;

					rewardExpInfo.m_MinReset = TokenNumber;

					Token = GetToken();
					rewardExpInfo.m_MaxReset = TokenNumber;

					Token = GetToken();
					rewardExpInfo.m_MinLevel = TokenNumber;

					Token = GetToken();
					rewardExpInfo.m_MaxLevel = TokenNumber;

					Token = GetToken();
					rewardExpInfo.m_iRewardExp = TokenNumber;

					Token = GetToken();

					if( rewardExpInfo.pEventDugeonItemBag != NULL )
						delete rewardExpInfo.pEventDugeonItemBag;

					rewardExpInfo.pEventDugeonItemBag = new EventItemBag;
					if( rewardExpInfo.pEventDugeonItemBag )
					{
						char Path[MAX_PATH] = { 0 };
						sprintf(Path, "%s%s", PATH_EVENTBAG_IMPERIALGUARDIAN, TokenString);
						rewardExpInfo.pEventDugeonItemBag->Read(gDirPath.GetNewPath(Path));
					}

					Token = GetToken();
					if( rewardExpInfo.pEventDugeonItemBagStone != NULL )
						delete rewardExpInfo.pEventDugeonItemBagStone;

					rewardExpInfo.pEventDugeonItemBagStone = new EventItemBag;
					if( rewardExpInfo.pEventDugeonItemBagStone )
					{
						char Path[MAX_PATH] = { 0 };
						sprintf(Path, "%s%s", PATH_EVENTBAG_IMPERIALGUARDIAN, TokenString);
						rewardExpInfo.pEventDugeonItemBagStone->Read(gDirPath.GetNewPath(Path));
					}

					Token = GetToken();
					if( rewardExpInfo.pEventDugeonItemBagGaion != NULL )
						delete rewardExpInfo.pEventDugeonItemBagGaion;

					rewardExpInfo.pEventDugeonItemBagGaion = new EventItemBag;
					if( rewardExpInfo.pEventDugeonItemBagGaion )
					{
						char Path[MAX_PATH] = { 0 };
						sprintf(Path, "%s%s", PATH_EVENTBAG_IMPERIALGUARDIAN, TokenString);
						rewardExpInfo.pEventDugeonItemBagGaion->Read(gDirPath.GetNewPath(Path));
					}

					this->m_vtRewardExpInfo.push_back(rewardExpInfo);
				}
			}
		}
	}

	fclose(SMDFile);
	LogAdd(lMsg.Get(MSGGET(1,198)),lpFileName);
	this->m_bLoaded = true;
}

void CImperialGuardianRewardExp::Init()
{
	this->m_bLoaded = false;
	this->m_vtRewardExpInfo.clear();
}

int CImperialGuardianRewardExp::GetRewardExp(int nUserIndex)
{
	int nSize = this->m_vtRewardExpInfo.size();
	int nResult = 0;

	if( !OBJMAX_RANGE(nUserIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return 0;
	}

	LPOBJ lpObj = &gObj[nUserIndex];
	int nUserLevel = lpObj->Level + lpObj->MasterLevel;
	_stRewardExpInfo rewardExpInfo;

	for(int i = 0; i < nSize; i++)
	{
		rewardExpInfo = this->m_vtRewardExpInfo.at(i);
		
		if(		rewardExpInfo.m_MinReset < gObj[nUserIndex].iResetCount
			&&	rewardExpInfo.m_MaxReset > gObj[nUserIndex].iResetCount )
		{
			if(		rewardExpInfo.m_MinLevel < nUserLevel 
				&&	rewardExpInfo.m_MaxLevel > nUserLevel )
			{
				nResult = rewardExpInfo.m_iRewardExp;
			}
		}
	}
	return nResult;
}

void CImperialGuardianRewardExp::DropReward(int nUserIndex,int iRewardType)
{
	int nResult = 0;

	if( !OBJMAX_RANGE(nUserIndex) )
	{
		LogAdd("error : %s %d",__FILE__,__LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	int nUserLevel = lpObj->Level + lpObj->MasterLevel;
	int nSize = this->m_vtRewardExpInfo.size();
	_stRewardExpInfo rewardExpInfo;

	for(int i = 0; i < nSize; i++)
	{
		rewardExpInfo = this->m_vtRewardExpInfo.at(i);

		if(		lpObj->iResetCount >= rewardExpInfo.m_MinReset
			&&	lpObj->iResetCount <= rewardExpInfo.m_MaxReset )
		{
			if( nUserLevel >= rewardExpInfo.m_MinLevel &&
				nUserLevel <= rewardExpInfo.m_MaxLevel )
			{
				if( iRewardType == 0 && rewardExpInfo.pEventDugeonItemBag )
				{
					rewardExpInfo.pEventDugeonItemBag->Open(lpObj->m_Index,lpObj->MapNumber,lpObj->X,lpObj->Y);
				}
				else if( iRewardType == 1 )
				{
					rewardExpInfo.pEventDugeonItemBagStone->Open(lpObj->m_Index,lpObj->MapNumber,lpObj->X,lpObj->Y);
				}
				else if( iRewardType == 2 )
				{
					rewardExpInfo.pEventDugeonItemBagGaion->Open(lpObj->m_Index,lpObj->MapNumber,lpObj->X,lpObj->Y);
				}
				break;
			}
		}
	}
}