#include "stdafx.h"
#include "CustomManager.h"
#include "ClassCalc.h"
#include "ItemDropRate.h"
#include "ItemPrice.h"
#include "Notice.h"
#include "MonsterAttr.h"
#include "GameMain.h"
#include "UserDieItemDrop.h"
#include "CommandManager.h"
#include "ExtraClassBalance.h"
#include "MapRateInfo.h"
#include "BuffSkillEffect.h"
#include "Achievements.h"
#include "ItemUpgradeJewels.h"
#include "ShopPointEx.h"
#if( ENABLE_CUSTOM_OFFLINETRADE == 1 )
#include "OfflineTrade.h"
#endif//ENABLE_CUSTOM_OFFLINETRADE
#if( ENABLE_CUSTOM_HARDCORE == 1 )
#include "HardcoreMode.h"
#endif//ENABLE_CUSTOM_HARDCORE
#if( ENABLETEST_MONSTERMONEYDROP == 1 )
#include "MonsterMoneyDrop.h"
#endif//ENABLETEST_MONSTERMONEYDROP
#include "ResetSystem.h"
#if (ENABLETEST_CHEATGUARD == 1)
#include "CheatGuard.h"
#endif
#include "ChaosMachineManager.h"

#if (ENABLETEST_ACHERON == 1)
#include "AcheronEvent.h"
#endif

#if (ENABLETEST_ZTLUA == 1)
#include "ZTLuaBackup.h"
#include "ZTLuaManager.h"
#endif

#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
#include "ItemExcellentOption.h"
#endif

#if (ENABLE_CUSTOM_PVPBALANCE == 1)
#include "PvPBalance.h"
#endif

#if (ENABLE_CUSTOM_MONSTERSPAWNER == 1)
#include "MonsterSpawner.h"
#endif
// -------------------------------------------------------------------------------

CustomManager g_CustomManager;
// -------------------------------------------------------------------------------

CustomManager::CustomManager()
{

}
// -------------------------------------------------------------------------------

CustomManager::~CustomManager()
{

}
// -------------------------------------------------------------------------------

void CustomManager::Load()
{
	VM_START
#if (ENABLETEST_ITEMEXCELLENTOPTION == 1)
	g_ItemExcellentOption.Load();
#endif

#if (ENABLETEST_ACHERON == 1)
	g_AcheronGuardianEvent.LoadScriptAcheronEvent(gDirPath.GetNewPath("AcheronEventData.txt"));
#endif

#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
	g_OfflineTrade.Load();
#endif

#if( ENABLETEST_ITEMUPGRADEJEWELS == 1 )
	g_ItemUpgradeJewels.Load();
#endif
	g_ClassCalc.Load();
	g_ItemDropRate.Load();
	g_ItemPrice.Load();
	g_Notice.Load();
	g_UserDieItemDrop.Load();
	g_CommandManager.Load();
	gCMAttr.Load(gDirPath.GetNewPath(FILE_CUSTOM_MONSTERSTATINFO));
	g_MapRateInfo.Load();
	g_BuffSkillEffect.Load();
#if( ENABLETEST_CHAOSMACHINEMNGR == 1 )
	g_ChaosMachineManager.Load();
#endif
#if( __4GAMERS__ == 1 )
	g_ExtraClassBalance.Load();
	g_Achievements.Load();
#endif
#ifdef POINTEX
	g_ShopPointEx.Load();
#endif
#if( ENABLE_CUSTOM_HARDCORE == 1 )
	g_HardcoreMode.Load();
#endif//ENABLE_CUSTOM_HARDCORE
#if( ENABLETEST_MONSTERMONEYDROP == 1 )
	g_MonsterMoneyDrop.Load();
#endif//ENABLETEST_MONSTERMONEYDROP
	g_ResetSystem.Load();

#if (ENABLETEST_CHEATGUARD == 1)
	g_CheatGuard.Load();
#endif

#if (ENABLE_CUSTOM_PVPBALANCE == 1)
	g_PvPBalance.Load();
#endif

#if (ENABLE_CUSTOM_MONSTERSPAWNER == 1)
	MonsterSpawnerMng::getInstance()->Load();
#endif
	VM_END
}
// -------------------------------------------------------------------------------