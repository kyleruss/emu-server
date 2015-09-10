#pragma once
// -------------------------------------------------------------------------------

#include "..\common\zzzitem.h"
#include "..\common\zzzpath.h"
#include "MapItem.h"
// -------------------------------------------------------------------------------

#define MAX_NUMBER_MAP			100
#define MAX_MAPITEM				300
#define MAX_MAP_RANGE(x)		(((x)<0)?FALSE:((x)>MAX_NUMBER_MAP-1)?FALSE:TRUE)
#define MAX_ITEM_TYPE_RANGE(x)	(((x)<0)?FALSE:((x)>MAX_MAPITEM-1)?FALSE:TRUE)
// -------------------------------------------------------------------------------

class MapClass
{
public:
	void	SaveItemInfo();
	void	GetLevelPos(BYTE& ox, BYTE& oy);
	void	GetMapPos(BYTE& ox, BYTE& oy);
	void	GetRandomLengthPos(BYTE& x, BYTE& y, int length);
	void	SearchStandAttr(BYTE& x, BYTE& y);
	BOOL	AttrLoad(char* filename);
	void	LoadMapAttr(char* filename, int MapNumber);
	BOOL	CheckWall(int sx1, int sy1, int sx2, int sy2);
	BYTE	CheckWall2(int sx1, int sy1, int sx2, int sy2);
	void	ItemInit();
	int		ItemGive(int aIndex, int item_num, bool bFailNotSend);
	int		ClearItem(LPOBJ lpObj, int distance);
	int		ItemDrop(int type, int level, float dur,  int x, int y, BYTE Option1, BYTE Option2, BYTE Option3, BYTE NOption, BYTE SOption, DWORD number, int aIndex, int PetLevel, int PetExp, BYTE ItemEffectEx,LPBYTE SocketOption,BYTE SocketBonus);
	int		MonsterItemDrop(int type, int level, float dur,  int x, int y, BYTE Option1, BYTE Option2, BYTE Option3, BYTE NOption, BYTE SOption,  int aIndex, DWORD number, BYTE ItemEffectEx,LPBYTE SocketOption, BYTE SocketBonus);
	int		MoneyItemDrop(int money, int x, int y);
	void	WeatherVariationProcess();
	BYTE	GetWeather();
	void	SetWeather(BYTE a_weather, BYTE a_variation);
	void	StateSetDestroy();
	bool	PathFinding2(int sx, int sy, int tx, int ty, PATH_t* a);
	bool	PathFinding3(int sx, int sy, int tx, int ty, PATH_t* a);
	bool	PathFinding4(int sx, int sy, int tx, int ty, PATH_t* a);
	void	SetStandAttr(int x, int y);
	void	ClearStandAttr(int x, int y);
	int		GetStandAttr(int x, int y);
	BYTE	GetAttr(int x, int y);
	void	SetRegentRect(int map);
	// ----
	void init()
	{
		this->m_attrbuf = 0;
		this->path = new PATH;
		this->m_width = 0;
		this->m_height = 0;
	}
	// ----
	void free()
	{
		if( this->m_attrbuf != 0 )
		{
			GlobalFree(this->m_attrbuf);
			this->m_attrbuf = 0;
		}
		// ----
		if( this->path != 0 )
		{
			delete this->path;
			this->path = 0;
		}
	}
	// ----
	MapClass();
	virtual ~MapClass();
	// ----
public:
	BYTE	m_Weather;
	BYTE	m_WeatherVariation;
	DWORD	m_WeatherTimer;
	DWORD	m_NextWeatherTimer;
	LPBYTE	m_attrbuf;
	int		m_width;
	int		m_height;
	PATH*	path;
	CMapItem m_cItem[MAX_MAPITEM];
	int		m_ItemCount;
	RECT	gRegenRect;
	int		thisMapNumber;
};
// -------------------------------------------------------------------------------

BOOL MapNumberCheck(int map);
// -------------------------------------------------------------------------------

struct MapAttrInfo
{
	int		MapNumber;
	BYTE	State;
	char	FileName[MAX_PATH];
};
// -------------------------------------------------------------------------------

class MapAttribute
{
public:
			MapAttribute();
			~MapAttribute();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	// ----
	LPSTR	GetFile(int MapNumber, BYTE EventState);
	// ----
private:
	std::vector<MapAttrInfo> m_Attribute;
	// ----
}; extern MapAttribute g_MapAttrInfo;
