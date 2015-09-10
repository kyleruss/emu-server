#include "stdafx.h"
#include "DummyTarget.h"
// -------------------------------------------------------------------------------

DummyTarget::DummyTarget()
{
	this->Init();
}
// -------------------------------------------------------------------------------

DummyTarget::~DummyTarget()
{

}
// -------------------------------------------------------------------------------

void DummyTarget::Init()
{
	this->m_Class		= -1;
	this->m_MapNumber	= -1;
	this->m_PosX		= -1;
	this->m_PosY		= -1;
}
// -------------------------------------------------------------------------------

bool DummyTarget::IsDummy(LPOBJ lpTarget)
{
	if(		lpTarget->Class == this->m_Class
		&&	lpTarget->MapNumber == this->m_MapNumber
		&&	lpTarget->X == this->m_PosX
		&&	lpTarget->Y == this->m_PosY )
	{
		return true;
	}
	// ----
	return false;

}
// -------------------------------------------------------------------------------

int DummyTarget::GetDefense(LPOBJ lpUser, LPOBJ lpDummy)
{
	return lpUser->m_Defense;
}
// -------------------------------------------------------------------------------