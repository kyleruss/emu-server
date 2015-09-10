#include "stdafx.h"
#include "PCBangPointSystem.h"
#include "GameMain.h"
#include "BloodCastle.h"
#include "ChaosCastle.h"
#include "DevilSquare.h"
#include "IllusionTempleEvent.h"
#include "Event.h"
#include "logproc.h"
#include "RaklionUtil.h"
#include "..\include\readscript.h"
#include "..\common\winutil.h"
#include "Shop.h"
#include "BuffEffect.h"
#include "DoubleGoerEvent.h"
#include "ImperialGuardian.h"

#include "CashShop.h"
#include "PCSProtocol.h"

CGoblinPointSystem g_GoblinPointSystem;

CGoblinPointSystem::CGoblinPointSystem()
{
}

CGoblinPointSystem::~CGoblinPointSystem()
{
}

BOOL CGoblinPointSystem::LoadData()
{
	m_bEnabled = GetPrivateProfileInt("GameServerInfo","GoblinPointSystemOn",1, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	GoblinPointCommonRuleTime = GetPrivateProfileInt("GameServerInfo","GoblinPointCommonRuleTime",1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	GoblinPointCommonRuleTimePoint = GetPrivateProfileInt("GameServerInfo","GoblinPointCommonRuleTimePoint",1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	GoblinPointFeverStartHour = GetPrivateProfileInt("GameServerInfo","GoblinPointFeverStartHour",1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	GoblinPointFeverEndHour = GetPrivateProfileInt("GameServerInfo","GoblinPointFeverEndHour",1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	GoblinPointFeverMultipleValue = GetPrivateProfileInt("GameServerInfo","GoblinPointFeverMultipleValue",1, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	LogAdd("[GoblinPointSystem][SetGoblinPointInfo] SystemOn:%d", m_bEnabled);

return TRUE;
}

BOOL CGoblinPointSystem::UserProc(LPOBJ lpObj)
{
	if( lpObj->Type != OBJ_USER )
		return true;

	if( m_bEnabled == FALSE )
		return true;

	if( lpObj->GoblinPointTick <= 0 )
	{
		lpObj->GoblinPointTick = GetTickCount();
		return true;
	}

#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
	if( lpObj->bOffTrade )
	{
		return true;
	}
#endif

	int bFever;
	SYSTEMTIME pTime;
	GetLocalTime(&pTime);

	bFever = FALSE;

	if( GoblinPointFeverStartHour <= GoblinPointFeverEndHour )
	{
		if( pTime.wHour >= GoblinPointFeverStartHour && pTime.wHour < GoblinPointFeverEndHour )
		{
			bFever = TRUE;
		}
	}
	else if( pTime.wHour >= GoblinPointFeverStartHour ||
		     pTime.wHour < GoblinPointFeverEndHour )
	{
		bFever = TRUE;
	}

	DWORD tmSec = (GetTickCount() - lpObj->GoblinPointTick) / 1000;

	if( tmSec >= (DWORD)GoblinPointCommonRuleTime )
	{
		lpObj->GoblinPointTick = GetTickCount();

		int IncPt = (bFever == TRUE) ? ( GoblinPointCommonRuleTimePoint * GoblinPointFeverMultipleValue ) : GoblinPointCommonRuleTimePoint;
		IncPt += m_ObjBill[lpObj->m_Index].GetGPBonus();

		protocol::MSG_GTOS_UPDATE_POINT_EX pMsg;

		pMsg.dwUserGuid = lpObj->DBNumber;
		pMsg.dwAddPoint[0] = 0;
		pMsg.dwAddPoint[1] = 0;
		pMsg.dwAddPoint[2] = IncPt;

		g_ShopServerClient.DataSend((PCHAR)&pMsg,sizeof(pMsg));
		
		LogAddTD("[GoblinPointSystem][Add Point] [%s] PointAdd: [%d][%d]",
			lpObj->AccountID, IncPt, m_ObjBill[lpObj->m_Index].GetGPBonus());
	}

return true;
}
