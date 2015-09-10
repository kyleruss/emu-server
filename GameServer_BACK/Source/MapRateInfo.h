#pragma once

struct MapRateData
{
	short MapNumber;
	float ExpIncrease;
	float MasterExpIncrease;
	float MoneyIncrease;
};

class MapRateInfo  
{
public:
			MapRateInfo();
	virtual ~MapRateInfo();
	
	void	Init();
	void	Load();
	void	Read(LPSTR File);

	float	GetExp(short MapNumber);
	float	GetMasterExp(short MapNumber);
	float	GetMoney(short MapNumber);

private:
	std::vector<MapRateData> m_RateInfo;

}; extern MapRateInfo g_MapRateInfo;