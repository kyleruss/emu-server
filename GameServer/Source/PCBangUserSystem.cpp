#include "stdafx.h"
#include "PCBangUserSystem.h"


int _PCBANG_TIME_CLASS::GetDayOfTheWeek(int iYear,int iMonth,int iDay)
{
	if( iMonth < 3 )
	{
		iYear--;
		iMonth += 12;
	}

	int YEAR_HIGH = iYear / 100;
	int YEAR_LOW = iYear % 100;


	int DayOfTheWeek = -1 + ( ((YEAR_HIGH*21)/4) + ((YEAR_LOW*5)/4) + iDay  + ((iMonth+1)*26)/10 );
	DayOfTheWeek = DayOfTheWeek%7;
return DayOfTheWeek;
}

BYTE _PCBANG_TIME_CLASS::GetEnable(int Level)
{
	BOOL bRet;
	if( Level%400 != 0 && ( (Level%4)!= 0 || (Level%100) == 0) )
	{
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}
return bRet;
}

int _PCBANG_TIME_CLASS::GetDayCount(int iYear,int iMonth)
{
	switch(iMonth)
	{
	case 2:
		return !(!GetEnable(iYear))+28;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	default:
		return 31;
	}
}

void _PCBANG_TIME_CLASS::CalcChangeDate(int iResetDayOfWeek,int& iYear,int& iMonth,int& iDay)
{
	int iDayOfTheWeek = GetDayOfTheWeek(iYear,iMonth,iDay);
	int iNewDayOfTheWeek = (iDayOfTheWeek+7-iResetDayOfWeek)%7;

	if( iDay > iNewDayOfTheWeek )
	{
		iDay-= iNewDayOfTheWeek;
		return;
	}

	iNewDayOfTheWeek -= iDay;

	if( iMonth == 1 )
	{
		iMonth = 12;
		iYear -= 1;
	}
	else
	{
		iMonth -= 1;
	}

	iDay = GetDayCount(iYear,iMonth)-iNewDayOfTheWeek;
}

int _PCBANG_TIME_CLASS::CompareDate(int Year1,int Month1,int Day1,int Hour1,int Year2,int Month2,int Day2,int Hour2)
{
	if( Year1 == Year2 &&
		Month1 == Month2 &&
		Day1 == Day2 &&
		Hour1 == Hour2 )
	{
		return false;
	}

	if( (Year1 == Year2 && Month1 == Month2 && Day1 == Day2 && Hour1 > Hour2) ||
		(Year1 == Year2 && Month1 == Month2 && Day1 > Day2) ||
		(Year1 == Year2 && Month1 > Month2) ||
		(Year1 > Year2) )
	{
		return true;
	}
	else
	{
		return -1;
	}
}

// CONSTRUCTOR / DESTRUCTOR

CPCBangPointUserSystem::CPCBangPointUserSystem(): m_iYear(2000), m_iMonth(1), m_iDay(1), m_iResetDayOfWeek(0), pFunction(0), m_bSwitch(0), m_iIndex(0)
{

}

CPCBangPointUserSystem::~CPCBangPointUserSystem()
{

}


void CPCBangPointUserSystem::Set(int iYear,int iMonth,int iDay,int iHour,int iResetDayOfWeek,int iResetHour,int Function,int aIndex)
{
	m_iYear = iYear;
	m_iMonth = iMonth;
	m_iDay = iDay;
	m_iHour = iHour;

	m_iResetDayOfWeek = iResetDayOfWeek;
	m_iResetHour = iResetHour;

	pFunction = (void(*)(int))Function;
	m_iIndex = aIndex;
}

void CPCBangPointUserSystem::SetSwitch0()
{
	SetSwitch(0);
}

int CPCBangPointUserSystem::SetChange()
{
	int iYear;
	int iMonth;
	int iDay;
	int iHour;

	if( m_bSwitch == 0 )
	{
		return true;
	}

	SYSTEMTIME pTime;
	GetLocalTime(&pTime);

	if( pSystime.wYear == pTime.wYear &&
		pSystime.wMonth == pTime.wMonth &&
		pSystime.wDay == pTime.wDay &&
		pSystime.wHour == pTime.wHour )
	{
		return true;
	}

	pSystime = pTime;

	iYear = pTime.wYear;
	iMonth = pTime.wMonth;
	iDay = pTime.wDay;
	iHour = m_iResetHour;

	if( m_iResetHour > pTime.wHour )
	{
		iDay--;
	}

	this->CalcChangeDate(this->m_iResetDayOfWeek,iYear,iMonth,iDay);

	if( this->CompareDate(this->m_iYear,this->m_iMonth,this->m_iDay,this->m_iHour,iYear,iMonth,iDay,iHour) < 0 )
	{
		m_iYear = pTime.wYear;
		m_iMonth = pTime.wMonth;
		m_iDay = pTime.wDay;
		m_iHour = pTime.wHour;
		return 2;
	}
	else
	{
		return 1;
	}

}
