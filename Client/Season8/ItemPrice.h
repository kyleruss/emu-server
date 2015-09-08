#pragma once
// -------------------------------------------------------------------------------

#include "Protocol.h"
// -------------------------------------------------------------------------------

struct ItemPriceInfo
{
	WORD	m_ItemCode;
	BYTE	m_ItemLevel;
	DWORD	m_ItemPrice;
	BYTE	m_Divisor;
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
	void	SetPrice(LPBYTE lpData);
	bool	GetPrice(WORD ItemCode, BYTE ItemLevel, DWORD& Price, BYTE& Divisor);
	// ----
private:
	std::vector<ItemPriceInfo> m_PriceInfo;
	// ----
}; extern ItemPrice g_ItemPrice;
// -------------------------------------------------------------------------------