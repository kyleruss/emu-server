#include "stdafx.h"
#include "ItemPrice.h"
// -------------------------------------------------------------------------------

ItemPrice g_ItemPrice;
// -------------------------------------------------------------------------------

void ItemPrice::SetPrice(LPBYTE lpData)
{
	this->m_PriceInfo.clear();
	if( this->m_PriceInfo.capacity() > 0 )
	{
		std::vector<ItemPriceInfo>().swap(this->m_PriceInfo);
	}
	// ----
	ITEMPRICE_ANS_COUNT* lpRecvCount = (ITEMPRICE_ANS_COUNT*)lpData;
	// ----
	if( lpRecvCount->Count <= 0 )
	{
		return;
	}
	// ----
	int Offset = sizeof(ITEMPRICE_ANS_COUNT);
	// ----
	for( int i = 0; i < lpRecvCount->Count; i++ )
	{
		ItemPriceInfo lpItem;
		ITEMPRICE_ANS_INFO* lpInfo = (ITEMPRICE_ANS_INFO*)&lpData[Offset];
		lpItem.m_ItemCode = lpInfo->ItemCode;
		lpItem.m_ItemLevel = lpInfo->ItemLevel;
		lpItem.m_ItemPrice = lpInfo->ItemPrice;
		lpItem.m_Divisor = lpInfo->Divisor;
		m_PriceInfo.push_back(lpItem);
		Offset += sizeof(ITEMPRICE_ANS_INFO);
	}
}
// -------------------------------------------------------------------------------

bool ItemPrice::GetPrice(WORD ItemCode, BYTE ItemLevel, DWORD& Price, BYTE& Divisor)
{
	for( int i = 0; i < this->m_PriceInfo.size(); i++ )
	{
		if( this->m_PriceInfo[i].m_ItemCode != ItemCode )
		{
			continue;
		}
		// ----
		if( this->m_PriceInfo[i].m_ItemLevel != (BYTE)-1 )
		{
			if( this->m_PriceInfo[i].m_ItemLevel != ItemLevel )
			{
				continue;
			}
		}
		// ----
		Price = this->m_PriceInfo[i].m_ItemPrice;
		Divisor = this->m_PriceInfo[i].m_Divisor;
		return true;
	}
	// ----
	return false;
}
// -------------------------------------------------------------------------------