#ifndef ___SHOP_H
#define ___SHOP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\zzzitem.h"

#define MAX_SHOP 500 //need delete, migrate to vector
#define MAX_ITEM_IN_SHOP 120

class CShop
{

public:

	CShop();
	virtual ~CShop();

	void Init();
	BOOL LoadShopItem(char* filename);
	int InsertItem(int type, int index, int level, int dur, int op1,  int op2, int op3, int exc, int anc, int socket, int element, int creditprice);
	int InentoryMapCheck(int sx, int sy, int width,  int height);


public:
	int m_ShopNumber;
	bool m_Loaded;
	BYTE ShopInventoryMap[MAX_ITEM_IN_SHOP];	// 4
	int ItemCount;	// 7C
	CItem m_item[MAX_ITEM_IN_SHOP];	// 80
	BYTE SendItemData[MAX_ITEM_IN_SHOP*(MAX_ITEM_INFO+1)];	// 5300
	int SendItemDataLen;	// 56C0
	bool m_CreditPrice;
	short m_CreditPriceTable[MAX_ITEM_IN_SHOP];
};

class ShopManager
{
public:
			ShopManager();
	virtual	~ShopManager();

	void	Init();
	void	Reload();
	void	Read(int ShopNumber);
	CShop*	Get(int ShopNumber);

private:
	std::vector<CShop> m_ShopList;
}; extern ShopManager g_ShopManager;


//BOOL ShopDataLoad();
//extern CShop ShopC[MAX_SHOP];

#endif