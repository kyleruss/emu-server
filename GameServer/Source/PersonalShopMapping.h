#include "StdAfx.h"
#include "..\common\winutil.h"

struct PersonalShopMap
{
	BYTE id;
	bool enabled;
	char name[50];
};

class PersonalShopMapping
{
	public:
		PersonalShopMapping();
		virtual ~PersonalShopMapping();

		void Init();
		void Load();
		void Read(LPSTR file);

		bool isEnabled();
		bool isAllmaps();
		bool CanWarp(BYTE map, int uIndex);
		bool CanOpenShopAtMap(BYTE map);
		bool CheckAndCloseShop(BYTE map, int uIndex);
		PersonalShopMap* FindShopAtMap(BYTE map);

	private:
		bool enabled;
		bool allmaps;
		bool preventwarp;
		bool autoclose;
		std::vector<PersonalShopMap> maps;
};

extern PersonalShopMapping pShop;