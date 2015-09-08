#if !defined(__PCBANGUSERSYSTEM_H__)
#define __PCBANGUSERSYSTEM_H__

class _PCBANG_TIME_CLASS
{
public:
	_PCBANG_TIME_CLASS() { };
	virtual ~_PCBANG_TIME_CLASS() {};

	int GetDayOfTheWeek(int,int,int);
	BYTE GetEnable(int Level);
	int GetDayCount(int,int);
	void CalcChangeDate(int,int&,int&,int&);
	int CompareDate(int,int,int,int,int,int,int,int);
};

class CPCBangPointUserSystem: public _PCBANG_TIME_CLASS
{
public:

	CPCBangPointUserSystem();
	virtual ~CPCBangPointUserSystem();

	void Set(int m_iYear,int m_iMonth,int m_iDay,int m_iHour,int m_iResetDayOfWeek,int m_iResetHour,int pFunction,int aIndex);
	void SetSwitch(BOOL bEnable) { m_bSwitch = bEnable;}
	void SetSwitch0();
	int SetChange();

public:

	int m_iYear;
	int m_iMonth;
	int m_iDay;
	int m_iHour;

	int m_iResetDayOfWeek;
	int m_iResetHour;

	void (*pFunction)(int);

	int m_bSwitch;
	SYSTEMTIME pSystime;

	int m_iIndex;
};

#endif