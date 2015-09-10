// SocketItemOption.cpp: implementation of the CSocketItems class.
///////////////////////////////////////////
#include "stdafx.h"
#include "GameMain.h"
#include "SocketItemOption.h"
#include "..\include\readscript.h"
#include "logproc.h"


// Global variables
CSocketItems g_SocketItem;

int g_SeedExtractMoney = 0;
int g_SphereCompositeMoney = 0;
int g_SphereSetMoney = 0;
int g_SphereRemoveMoney = 0;

int g_SocketRecipe_DropLevel = 0;
int g_SocketRecipe_DropRate = 0;
int g_SocketMaterial_Lv1 = 0;
int g_SocketMaterial_Lv2 = 0;

//-------------------------------------------------------
//	_SOCKET_SEED_DATA
//	Data structure for Socket Options
//-------------------------------------------------------
// Construction/Destruction

_SOCKET_SEED_DATA::_SOCKET_SEED_DATA()
{
	this->Clear();
}

_SOCKET_SEED_DATA::~_SOCKET_SEED_DATA()
{

}

//-------------------------------------------------------
// Member functions

	// Clearing the structure
void _SOCKET_SEED_DATA::Clear()
{
	this->btOption = 0;
	this->btGroup = 0;
	this->btIndex = 0;
	this->btIncType = 0;

	for(int i = 0; i < MAX_SPHERE_LEVEL; i++)
	{
		this->btIncValue[i] = 0;
	}

	this->OptionRate =0;
	this->SeedItem = 0;
}

	// Checking item seed
BYTE _SOCKET_SEED_DATA::CheckSeed(CItem* lpItem)
{
	if( lpItem == NULL )
		return false;

	if( lpItem->IsItem() == FALSE )
		return false;
	
	if( this->SeedItem == lpItem->m_Type && this->btIndex == lpItem->m_Level )
		return true;

return false;
}
// Set Options
void _SOCKET_SEED_DATA::SetOptionData(BYTE btOption, BYTE btGroup, BYTE btIndex, BYTE btIncType,LPBYTE btIncValue, int OptionRate)
{
	this->btOption = btOption;
	this->btGroup = btGroup;
	this->btIndex = btIndex;
	this->btIncType = btIncType;

	for(int i =0; i<MAX_SPHERE_LEVEL;i++)
	{
		this->btIncValue[i] = btIncValue[i];
	}

	this->OptionRate = OptionRate;

	switch( btGroup )
	{
	case 1:
		SeedItem = ITEMGET(12,60);
		break;
	case 2:
		SeedItem = ITEMGET(12,61);
		break;
	case 3:
		SeedItem = ITEMGET(12,62);
		break;
	case 4:
		SeedItem = ITEMGET(12,63);
		break;
	case 5:
		SeedItem = ITEMGET(12,64);
		break;
	case 6:
		SeedItem = ITEMGET(12,65);
		break;
	case 7:
		SeedItem = ITEMGET(12,66);
		break;
	}
}


//-------------------------------------------------------
//	_SOCKET_SPEAR_LEVEL
//	Data structure for Spear Options Level
//-------------------------------------------------------
// Construction/Destruction

_SOCKET_SPHERE_LEVEL::_SOCKET_SPHERE_LEVEL()
{

}

_SOCKET_SPHERE_LEVEL::~_SOCKET_SPHERE_LEVEL()
{

}

//-------------------------------------------------------
// Member functions

void _SOCKET_SPHERE_LEVEL::Clear()
{
	this->btLevel = 0;
	this->Value = 0;
	this->SeedItem = 0;
}

void _SOCKET_SPHERE_LEVEL::SetSphereLevel(BYTE btLevel,DWORD Value)
{
	this->btLevel = btLevel;
	this->Value = Value;

	switch( btLevel )
	{
	case 1:
		this->SeedItem = ITEMGET(12,70);
		break;
	case 2:
		this->SeedItem = ITEMGET(12,71);
		break;
	case 3:
		this->SeedItem = ITEMGET(12,72);
		break;
	case 4:
		this->SeedItem = ITEMGET(12,73);
		break;
	case 5:
		this->SeedItem = ITEMGET(12,74);
		break;
	}
}

//-------------------------------------------------------
//	_SOCKET_SPEAR_OPTION
//	Data structure for Spear Options
//-------------------------------------------------------
// Construction/Destruction

_SOCKET_SPHERE_OPTION::_SOCKET_SPHERE_OPTION()
{
	this->Clear();
}

_SOCKET_SPHERE_OPTION::~_SOCKET_SPHERE_OPTION()
{

}

//-------------------------------------------------------
// Member functions

int _SOCKET_SPHERE_OPTION::GetSphereItem()
{
	return this->SeedItem;
}

void _SOCKET_SPHERE_OPTION::Clear()
{
	this->btOptionId = 0;
	this->btOption = 0;
	this->btGroup = 0;
	this->btIndex = 0;
	this->btIncType = 0;
	this->wSphereValue = 0;
	this->SeedItem = 0;
}

BYTE _SOCKET_SPHERE_OPTION::SetSphereOption(_SOCKET_SEED_DATA* lpSeedData,BYTE btLevel)
{
	if( lpSeedData == NULL || btLevel == NULL )
	{
		return false;
	}

	this->btOptionId = btLevel * MAX_SEED_OPTION + lpSeedData->btOption;
	this->btOption = lpSeedData->btOption;
	this->btGroup = lpSeedData->btGroup;
	this->btIndex = lpSeedData->btIndex;
	this->btIncType = lpSeedData->btIncType;
	this->btLevel = btLevel;
	this->wSphereValue = lpSeedData->btIncValue[-1+btLevel];

	switch( lpSeedData->btGroup )
	{
	case 1:
		this->SeedItem = ITEMGET(12,100);
		break;
	case 2:
		this->SeedItem = ITEMGET(12,101);
		break;
	case 3:
		this->SeedItem = ITEMGET(12,102);
		break;
	case 4:
		this->SeedItem = ITEMGET(12,103);
		break;
	case 5:
		this->SeedItem = ITEMGET(12,104);
		break;
	case 6:
		this->SeedItem = ITEMGET(12,105);
		break;
	case 7:
		break;
	default:
		return false;
	}

	this->SeedItem += (this->btLevel -1)*6;

return true;
}

void _SOCKET_SPHERE_OPTION::IncValue(BYTE btValue)
{
	if( ( this->wSphereValue + btValue ) > (WORD)-2 )
	{
		return;
	}

	this->wSphereValue += btValue;
}

//-------------------------------------------------------
//	CSocketItems
//	Main class for socket options
//-------------------------------------------------------
// Construction/Destruction

CSocketItems::CSocketItems()
{

}

CSocketItems::~CSocketItems()
{

}

//-------------------------------------------------------
// Member functions

// Initialize Socket Option Data
void CSocketItems::Init()
{
	int i = 0;

	for( i = 0; i < MAX_SEED_OPTION; i++)
	{
		this->m_SeedOption[i].Clear();
	}

	for( i = 0; i < MAX_SPHERE_LEVEL; i++)
	{
		this->m_SphereLevel[i].Clear();
	}

	for( i = 0; i < MAX_BONUS_OPTION; i++)
	{
		this->m_BonusOption[i].Clear();
	}

	m_SocketType1Rnd.InitPool();
	m_OptionRnd.InitPool();

	for( i = 0; i < MAX_SOCKET_COUNT; i++)
	{
		this->m_SphereDropRate[i] = 0;
		this->m_SphereDropLevel[i] = 0;
	}

	m_SocketItemList.clear();
}

BYTE CSocketItems::LoadData(char* szFileName)
{
	this->Init();

	SMDFile = fopen(szFileName,"r");

	if( SMDFile == NULL )
	{
		MsgBox("[SocketOption] SocketOption Script file Load failed. (%s)",szFileName);
		return false;
	}

	int Token;
	int Index = 0;
	int count1 = 0;
	int count2 = 0;
	int count3 = 0;
	int count4 = 0;

	while( TRUE )
	{
		Token = GetToken();

		if( Token == END )
			break;

		if( strcmp("end",TokenString) == NULL )
			break;

		if( Token == NUMBER )
		{
			Index = (int)TokenNumber;

			while( TRUE )
			{
				if( Index == 0 )
				{
					BYTE btOption = 0;
					BYTE btGroup = 0;
					BYTE btIndex = 0;
					BYTE btIncType = 0;
					BYTE btIncValue[MAX_SPHERE_LEVEL] = {0};
					int OptionRate = 0;
					int SeedItem = 0;

					Token = GetToken();

					if( strcmp("end",TokenString) == NULL )
						break;

					btOption = (BYTE)TokenNumber;

					Token = GetToken();
					btGroup = (BYTE)TokenNumber;

					Token = GetToken();
					btIndex = (BYTE)TokenNumber;

					Token = GetToken();

					Token = GetToken();
					btIncType = (BYTE)TokenNumber;

					Token = GetToken();
					btIncValue[0] = (BYTE)TokenNumber;

					Token = GetToken();
					btIncValue[1] = (BYTE)TokenNumber;

					Token = GetToken();
					btIncValue[2] = (BYTE)TokenNumber;

					Token = GetToken();
					btIncValue[3] = (BYTE)TokenNumber;

					Token = GetToken();
					btIncValue[4] = (BYTE)TokenNumber;

					Token = GetToken();
					OptionRate = (int)TokenNumber;

					this->m_OptionRnd.AddValue(btOption,OptionRate);
					this->m_SeedOption[count1].SetOptionData(btOption,btGroup,btIndex,btIncType,&btIncValue[0],OptionRate);
					count1++;

					if( count1 > MAX_SEED_OPTION )
					{
						MsgBox("[ERROR] MAX SeedSphere Option Data Overflow! [%d]",count1);
						break;
					}
				}
				else if( Index == 1 )
				{
					_SOCKET_BONUS_DATA tmpBonus;

					Token = GetToken();

					if( strcmp("end",TokenString) == NULL )
						break;

					memset(&tmpBonus,0,sizeof(tmpBonus));

					tmpBonus.btIndex = (BYTE)TokenNumber;

					Token = GetToken();
					tmpBonus.btIndexMin = (BYTE)TokenNumber;

					Token = GetToken();
					tmpBonus.btIndexMax = (BYTE)TokenNumber;

					Token = GetToken();

					Token = GetToken();
					tmpBonus.btOptionType = (BYTE)TokenNumber;

					Token = GetToken();
					tmpBonus.btOptionValue = (BYTE)TokenNumber;

					Token = GetToken();
					tmpBonus.OptionRate = (int)TokenNumber;

					Token = GetToken();
					tmpBonus.btValue[0] = (BYTE)TokenNumber;

					Token = GetToken();
					tmpBonus.btValue[1] = (BYTE)TokenNumber;

					Token = GetToken();
					tmpBonus.btValue[2] = (BYTE)TokenNumber;

					Token = GetToken();
					tmpBonus.btValue[3] = (BYTE)TokenNumber;

					Token = GetToken();
					tmpBonus.btValue[4] = (BYTE)TokenNumber;

					for(int n = 0; n < MAX_SPHERE_LEVEL; n++)
					{
						if( tmpBonus.btValue[n] != NULL )
						{
							tmpBonus.btCount++;
						}
					}

					switch( tmpBonus.btIndex )
					{
					case 0:
						tmpBonus.btEffectType = 0;
						break;
					case 1:
						tmpBonus.btEffectType = 18;
						break;
					case 2:
						tmpBonus.btEffectType = 0;
						break;
					case 3:
						tmpBonus.btEffectType = 18;
						break;
					case 4:
						tmpBonus.btEffectType = 11;
						break;
					case 5:
						tmpBonus.btEffectType = 22;
						break;
					}

					memcpy(&this->m_BonusOption[count3],&tmpBonus,sizeof(tmpBonus));
					count3++;
				}
				else if( Index == 2 )
				{
					int iIndex = 0;
					BYTE btEffect = 0;
					BYTE btIncType = 0;
					BYTE btIncValue = 0;
					int OptionRate = 0;
					BYTE SeedCount[MAX_SEED_TYPE] = {0};

					Token = GetToken();

					if( strcmp("end",TokenString) == NULL )
						break;

					iIndex = (int)TokenNumber;

					Token = GetToken();
					btEffect = (BYTE)TokenNumber;

					Token = GetToken();

					Token = GetToken();
					btIncType = (BYTE)TokenNumber;

					Token = GetToken();
					btIncValue = (BYTE)TokenNumber;

					Token = GetToken();
					OptionRate = (int)TokenNumber;

					Token = GetToken();
					SeedCount[0] = (BYTE)TokenNumber;

					Token = GetToken();
					SeedCount[1] = (BYTE)TokenNumber;

					Token = GetToken();
					SeedCount[2] = (BYTE)TokenNumber;

					Token = GetToken();
					SeedCount[3] = (BYTE)TokenNumber;

					Token = GetToken();
					SeedCount[4] = (BYTE)TokenNumber;

					Token = GetToken();
					SeedCount[5] = (BYTE)TokenNumber;

					this->m_SetOption[count4].btIndex = count4;
					this->m_SetOption[count4].btIncType = btIncType;
					this->m_SetOption[count4].btIncValue = btIncValue;
					this->m_SetOption[count4].OptionRate = OptionRate;
					this->m_SetOption[count4].SeedCount[0] = SeedCount[0];
					this->m_SetOption[count4].SeedCount[1] = SeedCount[1];
					this->m_SetOption[count4].SeedCount[2] = SeedCount[2];
					this->m_SetOption[count4].SeedCount[3] = SeedCount[3];
					this->m_SetOption[count4].SeedCount[4] = SeedCount[4];
					this->m_SetOption[count4].SeedCount[5] = SeedCount[5];

					switch( iIndex )
					{
					case 0:
						this->m_SetOption[count4].btEffect = 44;
						break;
					case 1:
						this->m_SetOption[count4].btEffect = 45;
						break;
					}

					count4++;
				}
				else if( Index == 3 )
				{
					Token = GetToken();
					// ----
					if( !strcmp("end", TokenString) )
					{
						break;
					}
					// ----
					SocketItemInfo lpInfo;
					lpInfo.ItemType = TokenNumber;
					Token = GetToken();
					lpInfo.ItemIndex = TokenNumber;
					m_SocketItemList.push_back(lpInfo);
				}
			}
		}
	}
		
	this->LoadOptions();

	fclose(SMDFile);
return true;
}

void CSocketItems::LoadOptions()
{
	this->m_SocketType1Rnd.InitPool();
	this->m_SocketType2Rnd.InitPool();

	this->m_SocketType1Rate[0] = GetPrivateProfileInt("GameServerInfo","SocketSlotCountRate1",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SocketType1Rate[1] = GetPrivateProfileInt("GameServerInfo","SocketSlotCountRate2",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SocketType1Rate[2] = GetPrivateProfileInt("GameServerInfo","SocketSlotCountRate3",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SocketType1Rate[3] = GetPrivateProfileInt("GameServerInfo","SocketSlotCountRate4",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SocketType1Rate[4] = GetPrivateProfileInt("GameServerInfo","SocketSlotCountRate5",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	this->m_SocketType2Rate[0] = GetPrivateProfileInt("GameServerInfo","TwoHandWeaponSocketSlotCountRate1",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SocketType2Rate[1] = GetPrivateProfileInt("GameServerInfo","TwoHandWeaponSocketSlotCountRate2",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SocketType2Rate[2] = GetPrivateProfileInt("GameServerInfo","TwoHandWeaponSocketSlotCountRate3",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SocketType2Rate[3] = GetPrivateProfileInt("GameServerInfo","TwoHandWeaponSocketSlotCountRate4",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SocketType2Rate[4] = GetPrivateProfileInt("GameServerInfo","TwoHandWeaponSocketSlotCountRate5",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_SeedExtractMoney = GetPrivateProfileInt("GameServerInfo","SeedExtractMoney",1000000,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_SphereCompositeMoney = GetPrivateProfileInt("GameServerInfo","SeedSphereCompositeMoney",1000000,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_SphereSetMoney = GetPrivateProfileInt("GameServerInfo","SeedSphereSetMoney",1000000,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_SphereRemoveMoney = GetPrivateProfileInt("GameServerInfo","SeedSphereRemoveMoney",1000000,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	this->m_SocketType1Rnd.AddValue(1,this->m_SocketType1Rate[0]);
	this->m_SocketType1Rnd.AddValue(2,this->m_SocketType1Rate[1]);
	this->m_SocketType1Rnd.AddValue(3,this->m_SocketType1Rate[2]);
	this->m_SocketType1Rnd.AddValue(4,this->m_SocketType1Rate[3]);
	this->m_SocketType1Rnd.AddValue(5,this->m_SocketType1Rate[4]);


	this->m_SocketType2Rnd.AddValue(1,this->m_SocketType2Rate[0]);
	this->m_SocketType2Rnd.AddValue(2,this->m_SocketType2Rate[1]);
	this->m_SocketType2Rnd.AddValue(3,this->m_SocketType2Rate[2]);
	this->m_SocketType2Rnd.AddValue(4,this->m_SocketType2Rate[3]);
	this->m_SocketType2Rnd.AddValue(5,this->m_SocketType2Rate[4]);

	this->m_bSphereDrop = GetPrivateProfileInt("GameServerInfo","SphereDropOn",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	this->m_SphereDropRate[0] = GetPrivateProfileInt("GameServerInfo","SphereDropRate_Lv1",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SphereDropRate[1] = GetPrivateProfileInt("GameServerInfo","SphereDropRate_Lv2",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SphereDropRate[2] = GetPrivateProfileInt("GameServerInfo","SphereDropRate_Lv3",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SphereDropRate[3] = GetPrivateProfileInt("GameServerInfo","SphereDropRate_Lv4",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SphereDropRate[4] = GetPrivateProfileInt("GameServerInfo","SphereDropRate_Lv5",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	this->m_SphereDropLevel[0] = GetPrivateProfileInt("GameServerInfo","SphereDropLevel_Lv1",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SphereDropLevel[1] = GetPrivateProfileInt("GameServerInfo","SphereDropLevel_Lv2",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SphereDropLevel[2] = GetPrivateProfileInt("GameServerInfo","SphereDropLevel_Lv3",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SphereDropLevel[3] = GetPrivateProfileInt("GameServerInfo","SphereDropLevel_Lv4",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	this->m_SphereDropLevel[4] = GetPrivateProfileInt("GameServerInfo","SphereDropLevel_Lv5",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_SocketRecipe_DropLevel = GetPrivateProfileInt("GameServerInfo","SocketRecipe_DropLevel",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_SocketRecipe_DropRate = GetPrivateProfileInt("GameServerInfo","SocketRecipe_DropRate",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_SocketMaterial_Lv1 = GetPrivateProfileInt("GameServerInfo","SocketMaterial_DropRate_Lv1",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_SocketMaterial_Lv2 = GetPrivateProfileInt("GameServerInfo","SocketMaterial_DropRate_Lv2",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
}

bool CSocketItems::IsSocketItem(CItem* lpItem)
{
	if( lpItem == NULL )
	{
		return false;
	}

	if( lpItem->IsItem() == FALSE )
	{
		return false;
	}

return IsSocketItem(lpItem->m_Type);
}

bool CSocketItems::IsSocketItem(int ItemIndex)
{
	/*switch( ItemIndex )
	{
	case ITEMGET(0,26):
	case ITEMGET(0,27):
	case ITEMGET(0,28):
	case ITEMGET(0,29):
	case ITEMGET(0,30):
	case ITEMGET(0,36):
	case ITEMGET(0,37):

	case ITEMGET(2,16):
	case ITEMGET(2,17):
	case ITEMGET(2,19):
	case ITEMGET(2,20):

	case ITEMGET(3,12):

	case ITEMGET(4,23):
	case ITEMGET(4,25):
	case ITEMGET(4,26):

	case ITEMGET(5,20):
	case ITEMGET(5,30):
	case ITEMGET(5,31):
	case ITEMGET(5,32):
	case ITEMGET(5,35):
	case ITEMGET(5,37):

	case ITEMGET(6,17):
	case ITEMGET(6,18):
	case ITEMGET(6,19):
	case ITEMGET(6,20):
	case ITEMGET(6,22):
	case ITEMGET(6,23):
	case ITEMGET(6,24):
	case ITEMGET(6,25):
	case ITEMGET(6,26):

	case ITEMGET(7,44):
	case ITEMGET(7,45):
	case ITEMGET(7,46):
	case ITEMGET(7,49):
	case ITEMGET(7,50):
	case ITEMGET(7,51):
	case ITEMGET(7,52):
	case ITEMGET(7,53):
	case ITEMGET(7,54):
	case ITEMGET(7,55):
	case ITEMGET(7,56):
	case ITEMGET(7,58):
	case ITEMGET(7,74):
	case ITEMGET(7,75):

	case ITEMGET(8,44):
	case ITEMGET(8,45):
	case ITEMGET(8,46):
	case ITEMGET(8,47):
	case ITEMGET(8,48):
	case ITEMGET(8,49):
	case ITEMGET(8,50):
	case ITEMGET(8,51):
	case ITEMGET(8,52):
	case ITEMGET(8,53):
	case ITEMGET(8,54):
	case ITEMGET(8,55):
	case ITEMGET(8,56):
	case ITEMGET(8,57):
	case ITEMGET(8,58):
	case ITEMGET(8,74):
	case ITEMGET(8,75):
	case ITEMGET(8,76):

	case ITEMGET(9,44):
	case ITEMGET(9,45):
	case ITEMGET(9,46):
	case ITEMGET(9,47):
	case ITEMGET(9,48):
	case ITEMGET(9,49):
	case ITEMGET(9,50):
	case ITEMGET(9,51):
	case ITEMGET(9,52):
	case ITEMGET(9,53):
	case ITEMGET(9,54):
	case ITEMGET(9,55):
	case ITEMGET(9,56):
	case ITEMGET(9,57):
	case ITEMGET(9,58):
	case ITEMGET(9,74):
	case ITEMGET(9,75):
	case ITEMGET(9,76):

	case ITEMGET(10,44):
	case ITEMGET(10,45):
	case ITEMGET(10,46):
	case ITEMGET(10,47):
	case ITEMGET(10,48):
	case ITEMGET(10,49):
	case ITEMGET(10,50):
	case ITEMGET(10,51):
	case ITEMGET(10,52):
	case ITEMGET(10,53):
	case ITEMGET(10,54):
	case ITEMGET(10,55):
	case ITEMGET(10,56):
	case ITEMGET(10,57):
	case ITEMGET(10,58):
	case ITEMGET(10,74):
	case ITEMGET(10,75):
	case ITEMGET(10,76):

	case ITEMGET(11,44):
	case ITEMGET(11,45):
	case ITEMGET(11,46):
	case ITEMGET(11,47):
	case ITEMGET(11,48):
	case ITEMGET(11,49):
	case ITEMGET(11,50):
	case ITEMGET(11,51):
	case ITEMGET(11,52):
	case ITEMGET(11,53):
	case ITEMGET(11,54):
	case ITEMGET(11,55):
	case ITEMGET(11,56):
	case ITEMGET(11,57):
	case ITEMGET(11,58):
	case ITEMGET(11,74):
	case ITEMGET(11,75):
	case ITEMGET(11,76):
		return true;
	}

return false;*/
	for( int i = 0; i < this->m_SocketItemList.size(); i++ )
	{
		if( ITEMGET(this->m_SocketItemList[i].ItemType, this->m_SocketItemList[i].ItemIndex) == ItemIndex )
		{
			return true;
		}

	}
	return false;
}

BYTE CSocketItems::GetEmptySocketCount(CItem* lpItem)
{
	BYTE count = 0;

	for(int i = 0; i < MAX_SOCKET_COUNT; i++)
	{
		if( lpItem->m_SocketOption[i] == 0xFE )
		{
			count++;
		}
	}

return count;
}

void CSocketItems::GetItemOptions(CItem* lpItem,LPBYTE SocketOp,LPBYTE SocketId)
{
	if( lpItem == NULL )
		return;

	if( lpItem->IsItem() == FALSE || this->IsSocketItem(lpItem) == FALSE)
		return;

	for(int i = 0; i < MAX_SOCKET_COUNT; i++)
	{
		SocketOp[i] = lpItem->m_SocketOption[i];
	}

	*SocketId = lpItem->m_SocketBonus;
}

bool CSocketItems::CheckSocketOption(CItem* lpItem)
{
	if( lpItem == NULL )
	{
		return false;
	}

	if( lpItem->IsItem() == FALSE )
	{
		return false;
	}

	if( IsSocketItem(lpItem) == FALSE )
	{
		return false;
	}

	for(int n = 0; n < MAX_SOCKET_COUNT; n++)
	{
		if( lpItem->m_SocketOption[n] != (BYTE)-1 )
		{
			return true;
		}
	}
return false;
}

BYTE CSocketItems::IsSeedItem(int ItemType)
{
	switch( ItemType )
	{
	case ITEMGET(12,60):
	case ITEMGET(12,61):
	case ITEMGET(12,62):
	case ITEMGET(12,63):
	case ITEMGET(12,64):
	case ITEMGET(12,65):
		return true;
	}
	return false;
}

BYTE CSocketItems::IsSphereItem(int ItemType)
{
	switch( ItemType )
	{
	case ITEMGET(12,70):
	case ITEMGET(12,71):
	case ITEMGET(12,72):
	case ITEMGET(12,73):
	case ITEMGET(12,74):
		return true;
	}
	return false;
}

BYTE CSocketItems::IsSeedSphereItem(int ItemType)
{
	BYTE ItemIndex = 0;
	BYTE ItemNumber = 0;

	ItemNumber = ItemType % 512;
	ItemIndex = (ItemType-ItemNumber) / 512;


	if( ItemIndex == 12 )
	{
		if( ItemNumber >= 100 && ItemNumber <= 129 )
		{
			return true;
		}
	}

return false;
}

BYTE CSocketItems::GetSeedOption(int SeedItem,BYTE SeedIndex)
{
	for(int i = 0; i < MAX_SEED_OPTION; i++)
	{
		if( this->m_SeedOption[i].SeedItem == SeedItem )
		{
			if( this->m_SeedOption[i].btIndex == SeedIndex )
			{
				return this->m_SeedOption[i].btOption;
			}
		}
	}
	return -1;
}

BYTE CSocketItems::GetSphereLevel(int SpearItem)
{
	switch( SpearItem )
	{
	case ITEMGET(12,70):
		return 1;
	case ITEMGET(12,71):
		return 2;
	case ITEMGET(12,72):
		return 3;
	case ITEMGET(12,73):
		return 4;
	case ITEMGET(12,74):
		return 5;
	default:
		return 0;
	}
}

BYTE CSocketItems::CheckMountItem(CItem* lpItem,WORD Type,BYTE Level)
{
	BYTE btType = 0;
	BYTE btOption = 0;

	if( IsSocketItem(lpItem) == false)
	{
		return false;
	}

	btOption = GetSphereOption(Type,Level);
	btType = btOption%50;

	for(int i = 0; i < MAX_SOCKET_COUNT; i++)
	{
		if( lpItem->m_SocketOption[i] == 0xFF ||
			lpItem->m_SocketOption[i] == 0xFE )
		{
			continue;
		}

		if( (lpItem->m_SocketOption[i]%50) == btType )
		{
			return false;
		}
	}
return true;
}

BYTE CSocketItems::GetSphereOption(WORD Type,BYTE btIndex)
{
	BYTE btGroup = 0;
	BYTE btLevel = 0;
	BYTE btOption = 0;

	btLevel = ((Type%512)-100)/6;
	btGroup = ((Type%512)-100)%6+1;

	for(int i = 0; i < MAX_SEED_OPTION; i++)
	{
		if( this->m_SeedOption[i].btGroup == btGroup &&
			this->m_SeedOption[i].btIndex == btIndex)
		{
			btOption = btLevel*50+this->m_SeedOption[i].btOption;
			return btOption;
		}
	}

	return -1;
}

DWORD CSocketItems::GetOptionValue(BYTE Option)
{
	BYTE btOption = 0;
	BYTE btLevel = 0;

	_SOCKET_SEED_DATA* lpSeedOption = NULL;
	int btUnk2 = 0;

	_SOCKET_SPHERE_OPTION tmpOption;

	int Value = 0;

	btOption = Option % 50;
	btLevel = (Option - btOption)/50+1;

	lpSeedOption = GetSeedOption(btOption);

	if( lpSeedOption == NULL )
	{
		return false;
	}

	tmpOption.SetSphereOption(lpSeedOption,btLevel);
	Value = ItemAttribute[tmpOption.GetSphereItem()].BuyMoney;
	return Value;
}

BYTE CSocketItems::IsItemType(CItem* lpItem,BYTE SeedOption)
{
	if( IsSocketItem(lpItem) == FALSE )
	{
		return false;
	}

	BYTE btIndex = 0;
	BYTE btNumber = 0;
	BYTE btGroup = 0;

	btNumber = lpItem->m_Type%512;
	btIndex = (lpItem->m_Type - btNumber)/512;
	btGroup = GetOptionGroup(SeedOption);

	if( btIndex >= 0 && btIndex <= 5 )
	{
		switch( btGroup )
		{
		case 1:
		case 3:
		case 5:
			return true;
		}
	}
	else if( btIndex >= 6 && btIndex <= 11)
	{
		switch( btGroup )
		{
		case 2:
		case 4:
		case 6:
			return true;
		}
	}
	else
	{
		return false;
	}
return false;
}

BYTE CSocketItems::IsDropMap(int MapNumber)
{
	switch(MapNumber)
	{
	case MAP_INDEX_RAKLION:
	case MAP_INDEX_RAKLIONBOSS:
		return true;
	}
return false;
}

int CSocketItems::GetSphereDrop(int Level)
{
	int SphereItem = -1;
	int SphereMaxLevel = 0;

	if( m_bSphereDrop != TRUE )
	{
		return SphereItem;
	}

	if( this->m_SphereDropLevel[4] != 0 &&
		this->m_SphereDropLevel[4] <= Level)
	{
		SphereMaxLevel = 5;
	}
	else if( this->m_SphereDropLevel[3] != 0 &&
			 this->m_SphereDropLevel[3] <= Level)
	{
		SphereMaxLevel = 4;
	}
	else if( this->m_SphereDropLevel[2] != 0 &&
			 this->m_SphereDropLevel[2] <= Level)
	{
		SphereMaxLevel = 3;
	}
	else if( this->m_SphereDropLevel[1] != 0 &&
			 this->m_SphereDropLevel[1] <= Level)
	{
		SphereMaxLevel = 2;
	}
	else if( this->m_SphereDropLevel[0] != 0 &&
			 this->m_SphereDropLevel[0] <= Level)
	{
		SphereMaxLevel = 1;
	}
	else
	{
		return SphereItem;
	}

	for(int i = 0; i < SphereMaxLevel; i++)
	{
		if( (DWORD)m_SphereDropRate[i] >= (DWORD)rand()%10000 )
		{
			SphereItem = ITEMGET(12,70)+i;
		}
	}

return SphereItem;
}

BYTE CSocketItems::IsOptionSet(_SOCKET_SPHERE_OPTION* lpSphereData,BYTE btOption,BYTE btLevel)
{
	_SOCKET_SEED_DATA* lpSeedData = NULL;
	_SOCKET_SPHERE_OPTION* tmpSphereData = NULL;

	lpSeedData = GetSeedOption(btOption);

	if( lpSeedData == NULL )
		return false;

	if( lpSphereData->SetSphereOption(lpSeedData,btLevel) == FALSE )
		return false;
return true;
}

_SOCKET_SEED_DATA* CSocketItems::GetSeedOption(BYTE btOption)
{
	for(int i = 0; i < MAX_SEED_OPTION; i++)
	{
		if( this->m_SeedOption[i].btOption == btOption )
		{
			return &this->m_SeedOption[i];
		}
	}
return NULL;
}

_SOCKET_SPHERE_LEVEL* CSocketItems::GetSphereLevelOption(BYTE btLevel)
{
	for(int i = 0; i < MAX_SPHERE_LEVEL;i++)
	{
		if( this->m_SphereLevel[i].btLevel == btLevel )
		{
			return &this->m_SphereLevel[i];
		}
	}

return false;
}

BYTE CSocketItems::GetOptionRate()
{
	int OptionRate = 0;

	OptionRate = this->m_OptionRnd.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
return OptionRate;
}

BYTE CSocketItems::GetOptionGroup(BYTE SeedOption)
{
	BYTE btOption = 0;
	_SOCKET_SEED_DATA* lpSeedData = NULL;

	btOption = SeedOption % 50;

	lpSeedData = GetSeedOption(btOption);

	if( lpSeedData == NULL )
	{
		return false;
	}
return lpSeedData->btGroup;
}

_SOCKET_BONUS_DATA* CSocketItems::GetBonusOption(BYTE btBonusOption)
{
	for(int i = 0; i < MAX_BONUS_OPTION; i++)
	{
		if( this->m_BonusOption[i].btIndex == btBonusOption )
		{
			return &this->m_BonusOption[i];
		}
	}
return false;
}

int CSocketItems::ApplySocketOptions(LPOBJ lpObj)
{
	int OptionCount = 0;
	BYTE btSocketOption = 0;
	BYTE btOption = 0;
	BYTE btLevel = 0;

	_SOCKET_SPHERE_OPTION tmpSphere;

	for(int i = 0; i < INVETORY_WEAR_SIZE; i++)
	{
		if( IsSocketItem(&lpObj->pInventory[i]) == FALSE )
			continue;

		for(int n = 0; n < MAX_SOCKET_COUNT; n++)
		{
			btSocketOption = lpObj->pInventory[i].m_SocketOption[n];

			if( btSocketOption == 0xFF ||
				btSocketOption == 0xFE )
				continue;

			btOption = btSocketOption%50;
			btLevel = (btSocketOption-btOption)/50+1;

			if( IsOptionSet(&tmpSphere,btOption,btLevel) == FALSE )
				continue;

			SetSocketOption(lpObj,TRUE,btOption,tmpSphere.btIncType,tmpSphere.wSphereValue);
			OptionCount++;
		}
	}
return OptionCount;
}

void CSocketItems::ApplyBonusOptions(LPOBJ lpObj)
{
	BYTE btBonusOption = 0;
	_SOCKET_BONUS_DATA* lpBonusOption = NULL;

	for(int i = 0; i < INVETORY_WEAR_SIZE; i++)
	{
		if( IsSocketItem(&lpObj->pInventory[i]) == FALSE )
			continue;

		btBonusOption = lpObj->pInventory[i].m_SocketBonus;

		if( btBonusOption == 0xFF ||
			btBonusOption == 0xFE )
			continue;

		lpBonusOption = GetBonusOption(btBonusOption);

		if( lpBonusOption == NULL )
			continue;

		LogAddTD("[SocketItem] [%s][%s] Set Bonus Socket Option - OptionIndex : %d, EffectType : %d, OptionType : %d, OptionValue : %d",
			lpObj->AccountID,lpObj->Name,lpBonusOption->btIndex,lpBonusOption->btEffectType,lpBonusOption->btOptionType,lpBonusOption->btOptionValue);
		SetSocketOption(lpObj,2,lpBonusOption->btEffectType,lpBonusOption->btOptionType,lpBonusOption->btOptionValue);
	}
}

void CSocketItems::ApplySetOptions(LPOBJ lpObj)
{
	BYTE OpCount[8];
	BYTE btGroup = 0;
	BYTE btCount = 0;

	memset(&OpCount,0,sizeof(OpCount));

	for(int i = 0; i < 35; i++)
	{
		btGroup  = GetOptionGroup(lpObj->m_SocketOptions[i].btOption);

		if( btGroup == NULL )
			break;

		OpCount[-1+btGroup] += lpObj->m_SocketOptions[i].Count;
	}

	for(int n = 0; n < MAX_SET_OPTION; n++)
	{
		for(int i = 0; i < MAX_SEED_TYPE; i++)
		{
			if( this->m_SetOption[n].SeedCount[i] <= OpCount[i] )
			{
				btCount++;
			}
		}

		if( btCount == 6 )
		{
			SetSocketOption(lpObj,3,this->m_SetOption[n].btEffect,this->m_SetOption[n].btIncType,this->m_SetOption[n].btIncValue);
		}

		btCount = 0;
	}
}

BYTE CSocketItems::SetApplySocketItem(LPOBJ lpObj)
{
	int OptionCount = 0;
	if( lpObj == NULL )
		return false;

	if( lpObj->Type != OBJ_USER || lpObj->Connected < PLAYER_PLAYING )
		return false;

	InitUserOptions(lpObj);
	OptionCount= ApplySocketOptions(lpObj);

	if( OptionCount <= 0 )
		return false;

	ApplyBonusOptions(lpObj);
	ApplySetOptions(lpObj);
return true;
}

void CSocketItems::InitUserOptions(LPOBJ lpObj)
{
	if( lpObj == NULL )
		return;

	if( lpObj->Type != OBJ_USER || lpObj->Connected < PLAYER_PLAYING )
		return;

	int i = 0;

	for( i = 0; i < 35; i++)
	{
		if( lpObj->m_SocketOption[i].btOption == (BYTE)-1 )
			break;

		lpObj->m_SocketOption[i].Clear();
	}

	for( i = 0; i < 7; i++)
	{
		if( lpObj->m_SocketBonus[i].btOption == (BYTE)-1 )
			break;
		
		lpObj->m_SocketBonus[i].Clear();
	}

	for( i = 0; i < 2; i++)
	{
		if( lpObj->m_SocketSet[i].btOption == (BYTE)-1 )
			break;

		lpObj->m_SocketSet[i].Clear();
	}

	lpObj->m_wSocketAddLife = 0;
	lpObj->m_wSocketAddMana = 0;
	lpObj->m_wSocketMonsterDieLife = 0;
	lpObj->m_wSocketMonsterDieMana = 0;
	lpObj->m_btSocketBPUseDec = 0;
}

void CSocketItems::SetSocketOption(LPOBJ lpObj,int Type,BYTE btOption,BYTE IncType,WORD IncValue)
{
	if( btOption >= 0 && btOption <= 9 )
	{
		ApplyFireOption(lpObj,btOption,IncType,IncValue);
	}
	else if(btOption >= 10 && btOption <= 15)
	{
		ApplyWaterOption(lpObj,btOption,IncType,IncValue);
	}
	else if(btOption >= 16 && btOption <= 20)
	{
		ApplyIceOption(lpObj,btOption,IncType,IncValue);
	}
	else if(btOption >= 21 && btOption <= 28)
	{
		ApplyWindOption(lpObj,btOption,IncType,IncValue);
	}
	else if(btOption >= 29 && btOption <= 33)
	{
		ApplyLightningOption(lpObj,btOption,IncType,IncValue);
	}
	else if(btOption >= 34 && btOption <= 40)
	{
		ApplyEarthOption(lpObj,btOption,IncType,IncValue);
	}
	else if(btOption >= 41 && btOption <= 43)
	{
		ApplyUnk1Option(lpObj,btOption,IncType,IncValue);
	}
	else if(btOption >= 44 && btOption <= 45)
	{
		ApplySetOption(lpObj,btOption,IncType,IncValue);
	}

	for(int i = 0; i < 43; i++)
	{
		if( lpObj->m_SocketOptions[i].btOption == btOption &&
			lpObj->m_SocketOptions[i].IncType == IncType)
		{
			lpObj->m_SocketOptions[i].IncValue += IncValue;
			lpObj->m_SocketOptions[i].Count++;
			break;
		}
		else if(lpObj->m_SocketOptions[i].btOption == 0xFF)
		{
			lpObj->m_SocketOptions[i].btOption = btOption;
			lpObj->m_SocketOptions[i].IncType = IncType;
			lpObj->m_SocketOptions[i].IncValue = IncValue;
			lpObj->m_SocketOptions[i].Count = 1;
			break;
		}
	}
}

void CSocketItems::ApplyFireOption(LPOBJ lpObj,BYTE btOption,BYTE IncType,WORD IncValue)
{
	int _IncValue = 0;

	switch( btOption )
	{
	case 0:
		_IncValue = GetResultValue(lpObj->m_AttackDamageMaxLeft,IncValue,IncType);
		lpObj->m_AttackDamageMaxLeft += _IncValue;

		_IncValue = GetResultValue(lpObj->m_AttackDamageMinLeft,IncValue,IncType);
		lpObj->m_AttackDamageMinLeft += _IncValue;

		_IncValue = GetResultValue(lpObj->m_AttackDamageMaxRight,IncValue,IncType);
		lpObj->m_AttackDamageMaxRight += _IncValue;

		_IncValue = GetResultValue(lpObj->m_AttackDamageMinRight,IncValue,IncType);
		lpObj->m_AttackDamageMinRight += _IncValue;

		_IncValue = GetResultValue(lpObj->m_MagicDamageMin,IncValue,IncType);
		lpObj->m_MagicDamageMin += _IncValue;

		_IncValue = GetResultValue(lpObj->m_MagicDamageMax,IncValue,IncType);
		lpObj->m_MagicDamageMax += _IncValue;
		break;
	case 1:
		_IncValue = GetResultValue(lpObj->m_AttackSpeed,IncValue,IncType);
		lpObj->m_AttackSpeed += _IncValue;

		_IncValue = GetResultValue(lpObj->m_MagicSpeed,IncValue,IncType);
		lpObj->m_MagicSpeed += _IncValue;
		break;
	case 2:
		_IncValue = GetResultValue(lpObj->m_AttackDamageMaxLeft,IncValue,IncType);
		lpObj->m_AttackDamageMaxLeft += _IncValue;

		_IncValue = GetResultValue(lpObj->m_AttackDamageMaxRight,IncValue,IncType);
		lpObj->m_AttackDamageMaxRight += _IncValue;

		_IncValue = GetResultValue(lpObj->m_MagicDamageMax,IncValue,IncType);
		lpObj->m_MagicDamageMax += _IncValue;
		break;
	case 3:
		_IncValue = GetResultValue(lpObj->m_AttackDamageMinLeft,IncValue,IncType);
		lpObj->m_AttackDamageMinLeft += _IncValue;

		_IncValue = GetResultValue(lpObj->m_AttackDamageMinRight,IncValue,IncType);
		lpObj->m_AttackDamageMinRight += _IncValue;

		_IncValue = GetResultValue(lpObj->m_MagicDamageMin,IncValue,IncType);
		lpObj->m_MagicDamageMin += _IncValue;
		break;
	case 4:
		_IncValue = GetResultValue(lpObj->m_AttackDamageMaxLeft,IncValue,IncType);
		lpObj->m_AttackDamageMaxLeft += _IncValue;

		_IncValue = GetResultValue(lpObj->m_AttackDamageMinLeft,IncValue,IncType);
		lpObj->m_AttackDamageMinLeft += _IncValue;

		_IncValue = GetResultValue(lpObj->m_AttackDamageMaxRight,IncValue,IncType);
		lpObj->m_AttackDamageMaxRight += _IncValue;

		_IncValue = GetResultValue(lpObj->m_AttackDamageMinRight,IncValue,IncType);
		lpObj->m_AttackDamageMinRight += _IncValue;

		_IncValue = GetResultValue(lpObj->m_MagicDamageMin,IncValue,IncType);
		lpObj->m_MagicDamageMin += _IncValue;

		_IncValue = GetResultValue(lpObj->m_MagicDamageMax,IncValue,IncType);
		lpObj->m_MagicDamageMax += _IncValue;
		break;
	case 5:

		lpObj->m_btSocketBPUseDec += IncValue;

		if( lpObj->m_btSocketBPUseDec > 100 )
			lpObj->m_btSocketBPUseDec = 100;

		break;
	}
}

void CSocketItems::ApplyWaterOption(LPOBJ lpObj,BYTE btOption,BYTE IncType,WORD IncValue)
{
	int _IncValue = 0;

	switch( btOption )
	{
	case 10:
		_IncValue = GetResultValue(lpObj->m_SuccessfulBlocking,IncValue,IncType);
		lpObj->m_SuccessfulBlocking += _IncValue;
		break;
	case 11:
		_IncValue = GetResultValue(lpObj->m_Defense,IncValue,IncType);
		lpObj->m_Defense += _IncValue;

		_IncValue = GetResultValue(lpObj->m_MagicDefense,IncValue,IncType);
		lpObj->m_MagicDefense += _IncValue;
		break;
	case 12:
		lpObj->SetOpImproveSheldDefence += IncValue;
		break;
	case 13:
		lpObj->DamageMinus += IncValue;
		break;
	case 14:
		lpObj->DamageReflect += IncValue;
		break;
	}
}

void CSocketItems::ApplyIceOption(LPOBJ lpObj,BYTE btOption,BYTE IncType,WORD IncValue)
{
	int _IncValue = 0;

	switch( btOption )
	{
	case 16:
		_IncValue += GetResultValue((int)(lpObj->MaxLife+lpObj->AddLife),IncValue,IncType);
		lpObj->m_wSocketMonsterDieLife += _IncValue;
		break;
	case 17:
		_IncValue += GetResultValue((int)(lpObj->MaxMana+lpObj->AddMana),IncValue,IncType);
		lpObj->m_wSocketMonsterDieMana += _IncValue;
		break;
	case 18:
		_IncValue = GetResultValue(lpObj->SetOpAddSkillAttack,IncValue,IncType);
		lpObj->SetOpAddSkillAttack += _IncValue;
		break;
	case 19:
		lpObj->m_AttackRating += IncValue;
		break;
	}
}

void CSocketItems::ApplyWindOption(LPOBJ lpObj,BYTE btOption,BYTE IncType,WORD IncValue)
{
	int _IncValue = 0;

	switch( btOption )
	{
	case 21:
		_IncValue = GetResultValue(lpObj->MaxLife+lpObj->AddLife,IncValue,IncType);
		lpObj->m_wSocketAddLife += _IncValue;
		break;
	case 22:
		_IncValue = GetResultValue(lpObj->MaxLife+lpObj->AddLife,IncValue,IncType);
		lpObj->AddLife += _IncValue;

		GCReFillSend(lpObj->m_Index,lpObj->MaxLife+lpObj->AddLife,0xFE,0,lpObj->iMaxShield+lpObj->iAddShield);
		GCReFillSend(lpObj->m_Index,lpObj->Life,0xFF,0,lpObj->iShield);
		break;
	case 23:
		_IncValue = GetResultValue(lpObj->MaxMana+lpObj->AddMana,IncValue,IncType);
		lpObj->AddMana += _IncValue;

		GCManaSend(lpObj->m_Index,lpObj->MaxMana+lpObj->AddMana,0xFE,0,lpObj->MaxBP+lpObj->AddBP);
		GCManaSend(lpObj->m_Index,lpObj->Mana,0xFF,0,lpObj->BP);
		break;
	case 24:
		_IncValue = GetResultValue(lpObj->MaxMana+lpObj->AddMana,IncValue,IncType);
		lpObj->m_wSocketAddMana += _IncValue;
		break;
	case 25:
		_IncValue = GetResultValue(lpObj->MaxBP+lpObj->AddBP,IncValue,IncType);
		lpObj->AddBP += _IncValue;

		GCManaSend(lpObj->m_Index,lpObj->MaxMana+lpObj->AddMana,0xFE,0,lpObj->MaxBP+lpObj->AddBP);
		GCManaSend(lpObj->m_Index,lpObj->Mana,0xFF,0,lpObj->BP);
		break;
	case 26:
		_IncValue = GetResultValue(lpObj->SetOpIncAGValue,IncValue,IncType);
		lpObj->SetOpIncAGValue += _IncValue;
		break;
	case 27:
		lpObj->MonsterDieGetMoney += IncValue;
		break;
	}
}

void CSocketItems::ApplyLightningOption(LPOBJ lpObj,BYTE btOption,BYTE IncType,WORD IncValue)
{
	int _IncValue = 0;

	switch( btOption )
	{
	case 29:
		_IncValue = GetResultValue(lpObj->SetOpAddExDamage,IncValue,IncType);
		lpObj->SetOpAddExDamage += _IncValue;
		break;
	case 30:
		lpObj->m_ExcelentDamage += IncValue;
		break;
	case 31:
		_IncValue = GetResultValue(lpObj->SetOpAddCriticalDamage,IncValue,IncType);
		lpObj->SetOpAddCriticalDamage += _IncValue;
		break;
	case 32:
		lpObj->m_CriticalDamage += IncValue;
		break;
	}
}

void CSocketItems::ApplyEarthOption(LPOBJ lpObj,BYTE btOption,BYTE IncType,WORD IncValue)
{
	int _IncValue = 0;

	switch( btOption )
	{
	case 34:
		_IncValue = GetResultValue(lpObj->Strength+lpObj->AddStrength,IncValue,IncType);
		lpObj->AddStrength += _IncValue;
		break;
	case 35:
		_IncValue = GetResultValue(lpObj->Dexterity+lpObj->AddDexterity,IncValue,IncType);
		lpObj->AddDexterity += _IncValue;
		break;
	case 36:
		_IncValue = GetResultValue(lpObj->Vitality+lpObj->AddVitality,IncValue,IncType);
		lpObj->AddVitality += _IncValue;
		break;
	case 37:
		_IncValue = GetResultValue(lpObj->Energy+lpObj->AddEnergy,IncValue,IncType);
		lpObj->AddEnergy += _IncValue;
		break;
	}
}

void CSocketItems::ApplyUnk1Option(LPOBJ lpObj,BYTE btOption,BYTE IncType,WORD IncValue)
{

}

void CSocketItems::ApplySetOption(LPOBJ lpObj,BYTE btOption,BYTE IncType,WORD IncValue)
{
	switch(btOption)
	{
	case 44:
		lpObj->SetOpDoubleDamage += IncValue;
		break;
	case 45:
		lpObj->SetOpIgnoreDefense += IncValue;
		break;
	}
}

int CSocketItems::GetResultValue(int Value,WORD IncValue,BYTE IncType)
{
	int RetVal = 0;

	if( IncValue == 0 )
		return 0;

	switch( IncType )
	{
	case 1:
		RetVal = IncValue;
		break;
	case 2:
		RetVal = Value*IncValue/100;
		break;
	case 3:
		RetVal = Value/IncValue;
		break;
	case 4:
		RetVal = Value/IncValue;
		break;
	case 5:
		RetVal = Value/IncValue;
		break;
	default:
		return -1;
	}
return RetVal;
}

void CSocketItems::SetSocketOption(CItem* lpItem)
{
	if( lpItem == NULL )
		return;

	if( lpItem->IsItem() == FALSE || IsSocketItem(lpItem) == FALSE )
		return;

	BYTE btOption = 0;
	BYTE btLevel = 0;
	BYTE btSocketOpt = 0;

	_SOCKET_SPHERE_OPTION tmpSphere;

	int _IncValue = 0;

	for(int i = 0; i < MAX_SOCKET_COUNT; i++)
	{
		if( lpItem->m_SocketOption[i] == 0xFF || lpItem->m_SocketOption[i] == 0xFE )
			continue;

		btSocketOpt = lpItem->m_SocketOption[i];

		btOption = btSocketOpt % 50;
		btLevel = ((btSocketOpt - btOption)/50)+1;

		if( IsOptionSet(&tmpSphere,btOption,btLevel) == FALSE )
			continue;

		if( btOption == 38 )
		{
			_IncValue = GetResultValue(lpItem->m_HJOpStrength,tmpSphere.wSphereValue,tmpSphere.btIncType);
			lpItem->m_HJOpStrength += _IncValue;
		}
		else if(btOption == 39 )
		{
			_IncValue = GetResultValue(lpItem->m_HJOpDexterity,tmpSphere.wSphereValue,tmpSphere.btIncType);
			lpItem->m_HJOpDexterity += _IncValue;
		}
		else if(btOption == 20 )
		{
			lpItem->m_BonusDurability += tmpSphere.wSphereValue;
		}
	}
}

void CSocketItems::ShopSocketItem(CItem* lpItem,BYTE SocketCount)
{
	lpItem->InitSocketItem(SocketCount);
}

BYTE CSocketItems::GetSocketCount(int ItemType)
{
	int SocketCount = 0;
	if( ItemAttribute[ItemType].HaveItemInfo == TRUE )
	{
		if( ItemAttribute[ItemType].TwoHand == FALSE )
		{
			SocketCount = m_SocketType1Rnd.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
		}
		else
		{
			SocketCount = m_SocketType2Rnd.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
		}
	}
return SocketCount;
}

void CSocketItems::InitUserSocketOptions(LPOBJ lpObj)
{
	int i = 0;
	if( lpObj == NULL )
		return;

	if( lpObj->Type != OBJ_USER )
		return;

	for(i = 0; i < 35; i++)
	{
		lpObj->m_SocketOption[i].Clear();
	}

	for(i = 0;i <7; i++)
	{
		lpObj->m_SocketBonus[i].Clear();
	}

	for(i = 0; i < 2; i++)
	{
		lpObj->m_SocketSet[i].Clear();
	}
}

BYTE CSocketItems::ItemSetSeedSphere(CItem* lpItem,BYTE btOption,BYTE btPos)
{
	if( lpItem == NULL )
		return false;

	if( lpItem->IsItem() == NULL )
		return false;

	if( IsSocketItem(lpItem) == NULL )
		return false;

	if( btPos >= MAX_SOCKET_COUNT )
		return false;

	if( lpItem->m_SocketOption[btPos] != 0xFE )
		return false;

	lpItem->m_SocketOption[btPos] = btOption;
	SetBonusOption(lpItem);
return true;
}

BYTE CSocketItems::SetBonusOption(CItem* lpItem)
{
	if( lpItem == NULL )
		return false;

	if( lpItem->IsItem() == NULL || IsSocketItem(lpItem) == NULL)
		return false;

	BYTE OptCount[MAX_SOCKET_COUNT];

	for(int i = 0; i < MAX_SOCKET_COUNT;i++)
	{
		if( lpItem->m_SocketOption[i] != 0xFF &&
			lpItem->m_SocketOption[i] != 0xFE )
		{
			OptCount[i] = GetOptionGroup(lpItem->m_SocketOption[i]);
		}
	}

	BYTE btIndex = 0;
	WORD btNumber = 0;
	BYTE btCount = 0;

	btNumber = lpItem->m_Type%512;
	btIndex = (lpItem->m_Type-btNumber)/512;

	for(int n = 0; n < MAX_BONUS_OPTION; n++)
	{
		if( btIndex >= m_BonusOption[n].btIndexMin &&
			btIndex <= m_BonusOption[n].btIndexMax )
		{
			for(int i = 0; i < MAX_SOCKET_COUNT; i++)
			{
				if( m_BonusOption[n].btValue[i] == OptCount[i] &&
					OptCount[i] != FALSE )
				{
					btCount++;
				}

				if( btCount >= m_BonusOption[n].btCount )
				{
					if( rand()%100 < m_BonusOption[n].OptionRate )
					{
						lpItem->m_SocketBonus = m_BonusOption[n].btIndex;

						LogAdd("[SocketItem] Set Bonus Socket Option Success - OptionIndex : %d, ItemCode : %d, SocketOption[%d,%d,%d,%d,%d]",
							lpItem->m_SocketBonus,lpItem->m_Type,lpItem->m_SocketOption[0],lpItem->m_SocketOption[1],lpItem->m_SocketOption[2],lpItem->m_SocketOption[3],lpItem->m_SocketOption[4]);

						return true;
					}
					else
					{
						lpItem->m_SocketBonus = 0xFF;
						return false;
					}
				}
			}

			btCount = 0;
		}
	}

	lpItem->m_SocketBonus = 0xFF;
return false;
}

BYTE CSocketItems::ItemRemoveSeedSphere(CItem* lpItem,BYTE btPos)
{
	if( lpItem == NULL )
		return false;

	if( IsSocketItem(lpItem) == NULL )
		return false;

	if( btPos >= MAX_SOCKET_COUNT )
		return false;

	if( lpItem->m_SocketOption[btPos] == 0xFF ||
		lpItem->m_SocketOption[btPos] == 0xFE )
		return false;

	lpItem->m_SocketOption[btPos] = 0xFE;
	SetBonusOption(lpItem);

return true;
}