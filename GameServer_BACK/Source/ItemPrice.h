#pragma once
// -------------------------------------------------------------------------------

#include "..\common\winutil.h"
// -------------------------------------------------------------------------------

struct ItemPriceInfo
{
	WORD	m_ItemCode;
	BYTE	m_ItemLevel;
	DWORD	m_ItemPrice;
	BYTE	m_Divisor;
	short	m_ShopNumber;
};
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct ITEMPRICE_ANS_INFO
{
	WORD	ItemCode;
	BYTE	ItemLevel;
	DWORD	ItemPrice;
	BYTE	Divisor;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct ITEMPRICE_ANS_COUNT
{
	PWMSG_HEAD2 h;
	short Count;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

class ItemPrice  
{
public:
			ItemPrice();
	virtual ~ItemPrice();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	// ----
	bool	GetPrice(short ShopNumber, WORD ItemCode, BYTE ItemLevel);
	bool	GetPrice(short ShopNumber, WORD ItemCode, BYTE ItemLevel, DWORD& Price, BYTE& Divisor);
	bool	GetSendPrice(short ShopNumber, WORD ItemCode, BYTE ItemLevel, DWORD& Price, BYTE& Divisor);
	// ----
	void	SendPrice(int UserIndex, int ShopNumber);
private:
	std::vector<ItemPriceInfo> m_PriceInfo;
	// ----
}; extern ItemPrice g_ItemPrice;
// -------------------------------------------------------------------------------