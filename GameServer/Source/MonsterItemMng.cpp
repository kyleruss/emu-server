#include "stdafx.h"
#include "MonsterItemMng.h"
#include "MonsterAttr.h"
#include "GameMain.h"
#include "LogProc.h"
#include "CashShop.h"

// GS-N 0.99.60T 0x00419F60
//	GS-N	1.00.18	JPN	0x00420EF0	-	Completed

CMonsterItemMng g_MonsterItemMng;


CMonsterItemMng::CMonsterItemMng()
{
	return;
}


CMonsterItemMng::~CMonsterItemMng()
{
	this->Clear();
}


void CMonsterItemMng::Init()
{
	memset(this->m_iMonsterInvenItemCount, 0, sizeof(this->m_iMonsterInvenItemCount));

	int i;

	for( i = 0; i < MAX_LEVEL_MONSTER; i++ )
	{
		if( this->m_MonsterInvenItems[i] != NULL ) //Memleak 1
		{
			delete[] this->m_MonsterInvenItems[i];
		}

		this->m_MonsterInvenItems[i] = new CItem[MAX_ITEM_IN_MONSTER];
	}

	LPMONSTER_ATTRIBUTE lpm;

	for ( i = 0; i< MAX_MONSTER_TYPE ; i++ )
	{
		lpm = gMAttr.GetAttr(i);

		if ( lpm != NULL )
		{
			strcpy( this->MonsterName, lpm->m_Name);

			if ( lpm->m_Level != 0 )
			{
				this->gObjGiveItemSearch(lpm->m_Level, lpm->m_MaxItemLevel);
			}
		}
	}

	/*for(i = 0; i < MAX_LEVEL_MONSTER; i++)
	{
		char szTmp[256];
		sprintf(szTmp,"Info\\MonsterDrop%d.txt",i);
		FILE* File = fopen(szTmp,"w+");

		for(int n = 0; n < m_iMonsterInvenItemCount[i]; n++)
		{
			fprintf(File,"%d %d	%d\r\n",m_MonsterInvenItems[i][n].m_Type/512,m_MonsterInvenItems[i][n].m_Type%512,m_MonsterInvenItems[i][n].m_Level);
		}

		fclose(File);
	}*/
}


void CMonsterItemMng::Clear()
{
	memset(this->m_iMonsterInvenItemCount, 0, sizeof(this->m_iMonsterInvenItemCount));

	for ( int i =0 ; i< MAX_LEVEL_MONSTER ; i++ )
	{
		if( this->m_MonsterInvenItems[i] != NULL ) //Memleak 2
		{
			delete[] this->m_MonsterInvenItems[i];
		}
	}
}

BYTE CMonsterItemMng::InsertItem(int monsterlevel, int type, int index, int itemlevel, int op1, int op2 ,int op3 )
{
	if ( monsterlevel > MAX_LEVEL_MONSTER || monsterlevel < 0 )
	{
		return -1;
	}

	int itemcount = this->m_iMonsterInvenItemCount[monsterlevel];

	if ( itemcount >= MAX_ITEM_IN_MONSTER )
	{
		return -1;
	}

	CItem * item = &this->m_MonsterInvenItems[monsterlevel][itemcount];
	int item_type = (type * MAX_SUBTYPE_ITEMS) + index;
	item->m_Level = itemlevel;
	item->Convert(item_type, op1, op2, op3, 0, 0,0,0,-1, CURRENT_DB_VERSION);

	if ( type == 13 && index == 10 )
	{
		int rd = rand() % 100;
		item->m_Durability = rd + 100.0f;
	}
	else
	{
		item->m_Durability = item->m_BaseDurability;
	}

	itemcount++;
	this->m_iMonsterInvenItemCount[monsterlevel] = itemcount;
	return 0;
}


CItem * CMonsterItemMng::GetItem(int monsterlevel)
{
	if ( monsterlevel > MAX_LEVEL_MONSTER || monsterlevel < 0 )
	{
		return NULL;
	}

	int itemcount = this->m_iMonsterInvenItemCount[monsterlevel];

	if ( itemcount <= 0 )
	{
		return NULL;
	}

	int itemindex = rand() % itemcount;
	return &this->m_MonsterInvenItems[monsterlevel][itemindex];
}

void CMonsterItemMng::gObjGiveItemSearch(int monsterlevel, int maxlevel)
{
	int result;
	int incount = 0;
	int type;
	int index;

	if ( monsterlevel > MAX_LEVEL_MONSTER-1 )
	{
		LogAdd("error-L3 : Monster Level Overflow~ %s %d", __FILE__, __LINE__ );
		return;
	}

	int BallTable[22];

	BallTable[0] = 7;
	BallTable[1] = 8;
	BallTable[2] = 9;
	BallTable[3] = 10;
	BallTable[4] = 11;
	BallTable[5] = 12;
	BallTable[6] = 13;
	BallTable[7] = 14;
	BallTable[8] = 16;
	BallTable[9] = 17;
	BallTable[10] = 18;
	BallTable[11] = 19;
	BallTable[12] = 21;
	BallTable[13] = 22;
	BallTable[14] = 23;
	BallTable[15] = 24;
	BallTable[16] = 35;
	BallTable[17] = 44;
	BallTable[18] = 45;
	BallTable[19] = 46;
	BallTable[20] = 47;
	BallTable[21] = 48;


	while ( true )
	{
		if ( (rand() % 20) ==  0 ) 
		{
			if ( (rand() % 2)	!= 0 )
			{
				type = MAX_TYPE_ITEMS-1;
				index = rand() % (g_MaxItemIndexOfEachItemType[type]+1);
			}
			else
			{
				type = 12;
				index = BallTable[rand() % 22];
			}
		}
		else
		{
			type = rand() % MAX_TYPE_ITEMS;
			int iMaxItemIndex = g_MaxItemIndexOfEachItemType[type]+1;
			index = rand() % iMaxItemIndex;

			if ( type == 15 || (type == 12 && index != 15 ) )
			{
				continue;
			}
		}

		if ( type == 13 && index == 3 )
		{
			continue;
		}

		if ( (type == 13 && index == 32)
          || (type == 13 && index == 33)
          || (type == 13 && index == 34)
          || (type == 13 && index == 35)
          || (type == 13 && index == 36)
          || (type == 13 && index == 37) )
		{
			continue;
		}

        if ( (type == 14 && index == 35)
          || (type == 14 && index == 36)
          || (type == 14 && index == 37)
          || (type == 14 && index == 38)
          || (type == 14 && index == 39)
          || (type == 14 && index == 40) )
		{
			continue;
		}

        if ( IsCashItem(ITEMGET(type, index)) == TRUE )
		{
			continue;
		}

		if ( IsCashSpecialItem(ITEMGET(type,index)) == TRUE )
		{
			continue;
		}

		if ( (type == 14 && index == 45) ||
			 (type == 14 && index == 46) ||
			 (type == 14 && index == 47) ||
			 (type == 14 && index == 48) ||
			 (type == 14 && index == 49) ||
			 (type == 14 && index == 50) )
		{
			continue;
		}

		if ( (type == 13 && index == 41) )
		{
			continue;
		}
        
		if ( (type == 13 && index < 8) || 
			( (type == 14) && (index == 9 || index == 10 ||	index == 13 || index == 14 || index ==16 || index == 17 || index == 18 || index == 22 ) ) ||
			(type == 12 && index == 15) || 
			(type == 13 && index == 14) || 
			(type == 14 && index == 31 ) )
		{
			int perc = rand() % 11;
			BOOL bCheckDevil = FALSE;

			if ( type == 12 && index == 15 )
			{
				if ( monsterlevel >= 13 && monsterlevel <= 66 )
				{
					perc = rand() % 7 ;
					
					if ( perc < 3 )
					{
						perc = 0;
					}
				}
				else
				{
					perc = 1;
				}
			}

			if ( type == 14 && index == 17 )
			{
				if ( gEyesOfDevilSquareDropRate <= 0 )
				{
					gEyesOfDevilSquareDropRate = 1;
				}

				perc = rand() % gEyesOfDevilSquareDropRate;
				bCheckDevil = 1;

				if ( gDevilSquareEvent == 0 )
				{
					perc = 1;
				}
			}

			if ( type == 14 && index == 18 )
			{
				perc = rand() % gKeyOfDevilSquareDropRate;
				bCheckDevil = 1;

				if ( gDevilSquareEvent == 0 )
				{
					perc = 1;
				}

			}

			if ( perc == 0 )
			{
				if ( bCheckDevil == 1 )
				{
					int devilitemlevel = 0;

					if ( (rand() % 5) != 0 )
					{
						if ( monsterlevel < 3 )
						{
							devilitemlevel = 0;
						}
						else if ( monsterlevel < 36 ) 
						{
							devilitemlevel = 1;
						}
						else if ( monsterlevel < 47 )
						{
							devilitemlevel = 2;
						}
						else if ( monsterlevel < 60 )
						{
							devilitemlevel = 3;
						}
						else if ( monsterlevel < 70 )
						{
							devilitemlevel = 4;
						}
						else if ( monsterlevel < 80 )
						{
							devilitemlevel = 5;
						}
						else 
						{
							devilitemlevel = 6;
						}

						if ( devilitemlevel != 0 )
						{
							if ( this->InsertItem(monsterlevel, type, index, devilitemlevel, 0, 0, 0) != 0xFF )
							{
								incount++;

								if ( incount > MAX_ITEM_IN_MONSTER -1 )
								{
									return;
								}
							}
						}
					}
				}
				else if ( zzzItemLevel(type, index, monsterlevel ) == TRUE )
				{
					if ( this->InsertItem(monsterlevel, type, index, 0, 0, 0, 0) != (BYTE)-1 )
					{
						incount++;

						if ( incount > MAX_ITEM_IN_MONSTER -1 )
						{
							return;
						}
					}
				}
			}
		}
		else
		{
			result = GetLevelItem(type, index, monsterlevel);

			if ( result >= 0 )
			{
				if ( (type == 13 && index == 10) || (type == 12 && index == 11 ) )
				{
					if ( this->InsertItem(monsterlevel, type, index, result, 0,0,0) != 0xFF )
					{
						incount++;

						if ( incount > MAX_ITEM_IN_MONSTER -1 )
						{
							return;
						}
					}
				}
				else if ( result <= maxlevel )
				{
					if ( type == 12 )
					{
						if ( index != 11 )
						{
							result = 0;
						}
					}

					if ( type == 12 && index == 11 )
					{

					}
					else
					{
						if ( result > maxlevel )
						{
							result = maxlevel;
						}
					}

					if ( (type == 4 && index == 7) || (type == 4 && index == 15) )
					{
						result = 0;
					}
				

					if ( this->InsertItem(monsterlevel, type, index,result, 0,0,0) != (BYTE)-1 )
					{
						incount++;

						if ( incount > MAX_ITEM_IN_MONSTER-1 )
						{
							return;
						}
					}
				}
			}
		}
		
		if ( this->m_iMonsterInvenItemCount[monsterlevel] >= MAX_ITEM_IN_MONSTER )
		{
			return;
		}
		
	}
}
