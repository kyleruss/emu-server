#include "stdafx.h"
#include "ShopBoard.h"
// -------------------------------------------------------------------------------

ShopBoard g_ShopBoard;
// -------------------------------------------------------------------------------

ShopBoard::ShopBoard()
{

}
// -------------------------------------------------------------------------------

ShopBoard::~ShopBoard()
{

}
// -------------------------------------------------------------------------------

void ShopBoard::CGReqItemSearch(int UserIndex, SHOPBOARD_CGREQ_ITEM* lpData)
{
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return;
	}
	// ----
	LPOBJ lpUser = &gObj[UserIndex];
	// ----
	if( lpUser->Connected < PLAYER_PLAYING )
	{
		return;
	}
	// ----
	this->GCAnsItemSearch(UserIndex, lpData->ItemID);
}
// -------------------------------------------------------------------------------

void ShopBoard::GCAnsItemSearch(int UserIndex, WORD ItemID)
{
	LPOBJ lpUser	= &gObj[UserIndex];
	const int Size	= (sizeof(SHOPBOARD_GCANS_SHOPCOUNT) + sizeof(SHOPBOARD_GCANS_SHOPINFO)) * OBJMAXUSER;
	BYTE Temp[Size]	= { 0 };
	int Offset		= sizeof(SHOPBOARD_GCANS_SHOPCOUNT);
	int Count		= 0;
	SHOPBOARD_GCANS_SHOPINFO lpInfo = { 0 };
	// ----
	for( int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++ )
	{
		if(		gObj[i].m_bPShopOpen == false 
			||	gObj[i].Connected < PLAYER_PLAYING 
			||	UserIndex == i )
		{
			continue;
		}
		// ----
		if( ItemID != (WORD)-1 )
		{
			for( int ItemPos = MAIN_INVENTORY_SIZE; ItemPos < INVENTORY_SIZE - 1; ItemPos++ )
			{
				if(		gObj[i].Inventory1[ItemPos].IsItem()
					&&	gObj[i].Inventory1[ItemPos].m_Type == ItemID )
				{
					lpInfo.UserIndexH = SET_NUMBERH(i);
					lpInfo.UserIndexL = SET_NUMBERL(i);
					memcpy(lpInfo.UserName, gObj[i].Name, MAX_ACCOUNT_LEN);
					memcpy(lpInfo.ShopName, gObj[i].m_szPShopText, 36);
					Count++;
					memcpy(&Temp[Offset], (LPBYTE)&lpInfo, sizeof(SHOPBOARD_GCANS_SHOPINFO));
					Offset += sizeof(SHOPBOARD_GCANS_SHOPINFO);
				}
			}
		}
		else
		{
			lpInfo.UserIndexH = SET_NUMBERH(i);
			lpInfo.UserIndexL = SET_NUMBERL(i);
			memcpy(lpInfo.UserName, gObj[i].Name, MAX_ACCOUNT_LEN);
			memcpy(lpInfo.ShopName, gObj[i].m_szPShopText, 36);
			Count++;
			memcpy(&Temp[Offset], (LPBYTE)&lpInfo, sizeof(SHOPBOARD_GCANS_SHOPINFO));
			Offset += sizeof(SHOPBOARD_GCANS_SHOPINFO);
		}
	}
	// ----
	SHOPBOARD_GCANS_SHOPCOUNT lpAnswer = { 0 };
	lpAnswer.h.c		= 0xC2;
	lpAnswer.h.headcode = 0xEC;
	lpAnswer.h.subcode	= 0x31;
	lpAnswer.h.sizeH	= SET_NUMBERH(Offset);
	lpAnswer.h.sizeL	= SET_NUMBERL(Offset);
	lpAnswer.Count		= Count;
	memcpy(&Temp[0], (LPBYTE)&lpAnswer, sizeof(SHOPBOARD_GCANS_SHOPCOUNT));
	// ----
	DataSend(UserIndex, Temp, Offset);
}
// -------------------------------------------------------------------------------