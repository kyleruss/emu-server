#include "stdafx.h"
#include "Gambling.h"
#include "GameMain.h"
#include "..\include\readscript.h"
#include "logproc.h"
#include "DSProtocol.h"
#include "LargeRand.h"

CGamblingEvent gGamblingEvent;

CGamblingEvent::CGamblingEvent()
{

}

CGamblingEvent::~CGamblingEvent()
{

}

/*-------------------------------------------------------*/

void CGamblingEvent::Init(char* szFileName)
{
	m_bLoaded = FALSE;
	m_iCount = 0;
	m_iGroupCount = 0;

	int i;
	for(i = 0; i < MAX_GAMBLING_OPTION1; i++)
	{
		m_OptionRate[i] = 0;
	}

	for(i = 0; i < MAX_GAMBLING_OPTION2; i++)
	{
		m_Option2Rate[i] = 0;
	}

	for(i = 0; i < MAX_GAMBLING_OPTION3; i++)
	{
	}

	for(i = 0; i < MAX_GAMBLING_OPTION4; i++)
	{
	}

	for(i = 0; i < MAX_GAMBLING_GROUP; i++)
	{
		m_iGroupItemCount[i] = 0;
	}

	LoadData(gDirPath.GetNewPath(szFileName));
}

void CGamblingEvent::LoadData(char* szFileName)
{
	m_bLoaded = FALSE;
	m_iCount = FALSE;

	SMDFile = fopen(szFileName,"r");

	if( SMDFile == NULL )
	{
		LogAdd(lMsg.Get(MSGGET(1,197)),szFileName);
		return;
	}

	int Token,iCount = 0,iGroupCount = 0,iRateCount = 0,Index;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		if( Token == NUMBER )
		{
			Index = (int)TokenNumber;

			if( Index == 0 )
			{
				iRateCount = 0;

				while( TRUE )
				{
					Token = GetToken();

					if( Token == NAME )
					{
						if( strcmp("end",TokenString) == NULL )
							break;
					}

					if( iRateCount > MAX_GAMBLING_GROUP )
					{
						MsgBox("Load Script Error %s",szFileName);
						break;
					}

					m_RateInfo[iRateCount].iSkillOptionRate = (int)TokenNumber;

					Token = GetToken();
					m_RateInfo[iRateCount].iLuckOptionRate = (int)TokenNumber;

					Token = GetToken();
					m_RateInfo[iRateCount].iAddOptionRate = (int)TokenNumber;

					Token = GetToken();
					m_RateInfo[iRateCount].iExOptionRate = (int)TokenNumber;

					Token = GetToken();
					m_RateInfo[iRateCount].iDropRate = (int)TokenNumber;

					iRateCount++;
				}
			}
			else if( Index == 1 )
			{
				iRateCount = 0;
				while( TRUE )
				{
					Token = GetToken();

					if( Token == NAME )
					{
						if( strcmp("end",TokenString) == NULL )
							break;
					}

					if( iRateCount > MAX_GAMBLING_OPTION1 )
					{
						MsgBox("Load Script Error %s",szFileName);
						break;
					}

					m_OptionRate[iRateCount] = (int)TokenNumber;
					iRateCount++;
				}
			}
			else if( Index == 2 )
			{
				iRateCount = 0;
				while( TRUE )
				{
					Token = GetToken();

					if( Token == NAME )
					{
						if( strcmp("end",TokenString) == NULL )
							break;
					}

					if( iRateCount > MAX_GAMBLING_OPTION2 )
					{
						MsgBox("Load Script Error %s",szFileName);
						break;
					}

					m_Option2Rate[iRateCount] = (int)TokenNumber;
					iRateCount++;
				}
			}
			else if( Index == 3 )
			{
				iRateCount = 0;
				while( TRUE )
				{
					Token = GetToken();

					if( Token == NAME )
					{
						if( strcmp("end",TokenString) == NULL )
							break;
					}

					if( iRateCount > MAX_GAMBLING_OPTION3 )
					{
						MsgBox("Load Script Error %s",szFileName);
						break;
					}

					m_Option3Rate[iRateCount] = (int)TokenNumber;
					iRateCount++;
				}
			}
			else if( Index == 4 )
			{
				iRateCount = 0;
				while( TRUE )
				{
					Token = GetToken();

					if( Token == NAME )
					{
						if( strcmp("end",TokenString) == NULL )
							break;
					}

					if( iRateCount > MAX_GAMBLING_OPTION5 )
					{
						MsgBox("Load Script Error %s",szFileName);
						break;
					}

					m_Option4Rate[iRateCount] = (int)TokenNumber;

					Token = GetToken();
					m_Option5Rate[iRateCount] = (int)TokenNumber;
					iRateCount++;
				}
			}
			else if( Index >= 5 )
			{
				iCount = 0;

				while( TRUE )
				{
					Token = GetToken();

					if( Token == NAME )
					{
						if(strcmp("end",TokenString) == NULL)
						{
							m_iGroupItemCount[iGroupCount] = iCount;
							break;
						}
					}

					m_ItemList[m_iCount].btItemIndex = (BYTE)TokenNumber;

					Token = GetToken();
					m_ItemList[m_iCount].btItemType = (BYTE)TokenNumber;

					Token = GetToken();
					m_ItemList[m_iCount].btItemLevelMin = (BYTE)TokenNumber;

					Token = GetToken();
					m_ItemList[m_iCount].btItemLevelMax = (BYTE)TokenNumber;

					Token = GetToken();
					m_ItemList[m_iCount].btSkillOption = (BYTE)TokenNumber;

					Token = GetToken();
					m_ItemList[m_iCount].btLuckOption = (BYTE)TokenNumber;

					Token = GetToken();
					m_ItemList[m_iCount].btAddOption = (BYTE)TokenNumber;

					Token = GetToken();
					m_ItemList[m_iCount].btExOption = (BYTE)TokenNumber;

					if( m_ItemList[m_iCount].btItemLevelMin > m_ItemList[m_iCount].btItemLevelMax )
					{
						MsgBox("Load Script Error %s",szFileName);
						return;
					}

					iCount++;
					m_iCount++;

					if( m_iCount > 149 )
					{
						break;
					}
				}

				iGroupCount++;

				if( iGroupCount > (MAX_GAMBLING_GROUP-1) )
				{
					break;
				}

				m_iGroupCount = iGroupCount;
			}
		}
	}

	fclose(SMDFile);
	LogAdd(lMsg.Get(MSGGET(1,198)),szFileName);
	m_bLoaded = TRUE;
}

BYTE CGamblingEvent::GetLevel(int Index)
{
	if( Index < 0 || Index > 149 )
		return false;

	if( m_ItemList[Index].btItemLevelMin == m_ItemList[Index].btItemLevelMax )
	{
		return m_ItemList[Index].btItemLevelMin;
	}

	int sub = (m_ItemList[Index].btItemLevelMax - m_ItemList[Index].btItemLevelMin) + 1;
	int level = m_ItemList[Index].btItemLevelMin + (rand()%sub);

	return level;
}

BYTE CGamblingEvent::BuyRequest(int aIndex,BYTE MapNumber,BYTE cX,BYTE cY,BYTE Type)
{
	if( m_bLoaded == FALSE )
		return false;

	BYTE Dur = 0;
	int Item = 0;
	int Level = 0;
	int X = 0;
	int Y = 0;
	int Option1 = 0;
	int Option2 = 0; 
	int Option3 = 0;
	int Number = 0;
	int SetOption = 0;
	int ExOption = 0;

	int SocketCount = 0;

	BYTE SocketBonus = (BYTE)-1;
	BYTE SocketOption[5];

	LPOBJ lpObj = &gObj[aIndex];

	if( GetItemCount() > 0 )
	{
		if( m_Option2Rate[Type] >= GetLargeRand()%1000000   )
		{
			Level = 0;
			Dur = 0;
			Option1 = 1;
			Option2 = 0;

			DWORD _iRandOp = GetLargeRand()%1000000;

			if( _iRandOp <= m_OptionRate[0] )
			{
				Option3 = 1;
			}
			else if( _iRandOp <= m_OptionRate[0]+m_OptionRate[1] )
			{
				Option3 = 2;
			}
			else if( _iRandOp <= m_OptionRate[0]+m_OptionRate[1]+m_OptionRate[2] )
			{
				Option3 = 3;
			}
			else if( _iRandOp <= m_OptionRate[0]+m_OptionRate[1]+m_OptionRate[2]+m_OptionRate[3] )
			{
				Option3 = 4;
			}

			DWORD _iRandExOp = GetLargeRand()%1000000;

			if( _iRandExOp <= m_Option3Rate[0] )
			{
				ExOption = GetExcOption(1);
			}
			else if( _iRandExOp <= m_Option3Rate[0]+m_Option3Rate[1] )
			{
				ExOption = GetExcOption(2);
			}
			else if( _iRandExOp <= m_Option3Rate[0]+m_Option3Rate[1]+m_Option3Rate[2] )
			{
				ExOption = GetExcOption(3);
			}
			else if( _iRandExOp <= m_Option3Rate[0]+m_Option3Rate[1]+m_Option3Rate[2]+m_Option3Rate[3] )
			{
				ExOption = GetExcOption(4);
			}

			switch( Type )
			{
			case 0:
				Item = ItemGetNumberMake(3,11);
				break;
			case 1:
				Item = ItemGetNumberMake(5,33);
				break;
			case 2:
				Item = ItemGetNumberMake(4,24);
				break;
			case 3:
				Item = ItemGetNumberMake(2,18);
				break;
			case 4:
				Item = ItemGetNumberMake(5,34);
				break;
			}
		}
		else
		{
			int iCount = 0;
			int iTotalCountStart = 0;
			int iTotalCountEnd = 0;
			int StartIndex = 0;

			switch( Type )
			{
			case 0:	StartIndex = 0;	break;
			case 1:	StartIndex = 5;	break;
			case 2:	StartIndex = 10;	break;
			case 3:	StartIndex = 15;	break;
			case 4:	StartIndex = 20;	break;
			}

			DWORD _iRate = GetLargeRand()%1000000;
			DWORD _iItemRate = 0;

			int i; 

			for( i = StartIndex; i < (StartIndex+5);i++)
			{
				_iItemRate += m_RateInfo[i].iDropRate; 

				if( _iRate <= _iItemRate )
				{
					StartIndex = i;
					break;
				}
			}

			while( TRUE )
			{
				iTotalCountEnd += m_iGroupItemCount[iCount];

				if( StartIndex <= iCount )
					break;

				iTotalCountStart += m_iGroupItemCount[iCount];
				iCount++;
			}

			Number = GetLargeRand()%(iTotalCountEnd-iTotalCountStart) + iTotalCountStart;

			Dur = 0;
			X = 0;
			Y = 0;

			Level = GetLevel(Number);
			Item = ItemGetNumberMake(m_ItemList[Number].btItemIndex,m_ItemList[Number].btItemType);

			if( Item == -1 )
				return false;

			if( m_ItemList[Number].btSkillOption )
			{
				if( (DWORD)m_RateInfo[StartIndex].iSkillOptionRate >= GetLargeRand()%1000000 )
				{
					Option1 = 1;
				}
			}

			if( m_ItemList[Number].btLuckOption )
			{
				if( (DWORD)m_RateInfo[StartIndex].iLuckOptionRate >= GetLargeRand()%1000000 )
				{
					Option2 = 1;
				}
			}

			if( m_ItemList[Number].btAddOption )
			{
				if( (DWORD)m_RateInfo[StartIndex].iAddOptionRate >= GetLargeRand()%1000000 )
				{
					DWORD _iRate = GetLargeRand()%1000000;

					if( _iRate <= m_OptionRate[0] )
					{
						Option3 = 1;
					}
					else if( _iRate <= m_OptionRate[0]+m_OptionRate[1] )
					{
						Option3 = 2;
					}
					else if( _iRate <= m_OptionRate[0]+m_OptionRate[1]+m_OptionRate[2] )
					{
						Option3 = 3;
					}
					else if( _iRate <= m_OptionRate[0]+m_OptionRate[1]+m_OptionRate[2]+m_OptionRate[3] )
					{
						Option3 = 4;
					}
				}
			}

			if( m_ItemList[Number].btExOption )
			{
				if( (DWORD)m_RateInfo[StartIndex].iExOptionRate >= GetLargeRand()%1000000 )
				{
					ExOption = GetExcOption();
					Option1 = 1;
					Option2 = 0;
					Level = 0;
				}
			}
		}

		if( g_SocketItem.IsSocketItem(Item) )
		{
			SocketCount = 1+rand()%3;

			for(int i = 0; i < 5; i++)
			{
				if( i < SocketCount )
				{
					SocketOption[i] = (BYTE)-2;
				}
				else
				{
					SocketOption[i] = (BYTE)-1;
				}
			}
		}

		ItemSerialCreateSend(lpObj->m_Index,MapNumber,X,Y,Item,Level,Dur,Option1,Option2,Option3,lpObj->m_Index,ExOption,0,SocketBonus,SocketOption);
		LogAddTD("[ GAMBLING ][ TryGambling ] %s(%s) Try Item : [%s]%d Level:%d op1:%d op2:%d op3:%d ExOp:%d",
			lpObj->AccountID,lpObj->Name,ItemAttribute[Item].Name,Item,Level,Option1,Option2,Option3,ExOption);
	}
return true;
}

BYTE CGamblingEvent::GetExcOption(int Number)
{
	BYTE ExOption = 0;

	int OpIndex = 0;
	int Count = 0;

	while( TRUE )
	{
		if( (DWORD)m_Option4Rate[OpIndex] >= GetLargeRand()%1000000 )
		{
			BYTE ExcValue = 1<<OpIndex;
			if( !(ExOption&ExcValue) )
			{
				ExOption |= ExcValue;
				Count++;
			}
		}

		OpIndex++;

		if( Count < Number )
		{
			if( OpIndex >= 6 )
			{
				OpIndex = 0;
			}
		}
		else if( Count >= Number )
		{
			break;
		}
	}

return ExOption;
}

BYTE CGamblingEvent::GetExcOption()
{
	BYTE ExOption = 0;

	int OpIndex = 0;
	int Count = 0;

	while( TRUE )
	{
		if( (DWORD)m_Option5Rate[OpIndex] >= GetLargeRand()%1000000 )
		{
			BYTE ExcValue = 1<<OpIndex;
			if( !(ExOption&ExcValue) )
			{
				ExOption |= ExcValue;
				Count++;
			}
		}

		OpIndex++;

		if( Count == 0 )
		{
			if( OpIndex >= 6 )
			{
				OpIndex = 0;
			}
		}
		else if( Count == 1 )
		{
			if( OpIndex >= 6 )
			{
				break;
			}
		}
		else if( Count >= 2 )
		{
			break;
		}
	}

return ExOption;
}