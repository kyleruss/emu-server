#pragma once
// -------------------------------------------------------------------------------

#include "..\common\winutil.h"
// -------------------------------------------------------------------------------

struct SHOPBOARD_CGREQ_ITEM //-> 10 [0xC1 -> 0xEC -> 0x31]
{
	PBMSG_HEAD2 h;		//+0
	DWORD Unknown4;		//+4
	WORD ItemID;		//+8
};
// -------------------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)
struct SHOPBOARD_GCANS_SHOPINFO //-> 50
{
	BYTE UserIndexH;	//+0
	BYTE UserIndexL;	//+1
	char UserName[11];	//+2
	char ShopName[37];	//+13
};
// -------------------------------------------------------------------------------

struct SHOPBOARD_GCANS_SHOPCOUNT //-> 10+ [0xC2 -> 0xEC -> 0x31]
{
	PWMSG_HEAD2 h;		//+0
	DWORD Count;		//+5
	BYTE Unknown6;		//+6
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

class ShopBoard  
{
public:
			ShopBoard();
	virtual ~ShopBoard();
	// ----
	void	CGReqItemSearch(int UserIndex, SHOPBOARD_CGREQ_ITEM* lpData);
	void	GCAnsItemSearch(int UserIndex, WORD ItemID);
	// ----
}; extern ShopBoard g_ShopBoard;
// -------------------------------------------------------------------------------
