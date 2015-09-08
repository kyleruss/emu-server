#include "stdafx.h"
#include "gamemain.h"
#include "..\common\winutil.h"
#include "GameServer.h"
#include "DirPath.h"
#include "logproc.h"
#include "wsJoinServerCli.h"
#include "DirPath.h"
#include "DSProtocol.h"
#include "CLoginCount.h"
#include "MapClass.h"
#include "..\common\classdef.h"
#include "..\include\prodef.h"
#include "wsGameServer.h"
#include "EledoradoEvent.h"
#include "WhisperCash.h"
#include "SProtocol.h"
#include "ChaosCastle.h"
#include "DevilSquare.h"
#include "Gate.h"
#include "user.h"
#include "Event.h"
#include "PacketCheckSum.h"
#include "DragonEvent.h"
#include "AttackEvent.h"
#include "SimpleModulus.h"
#include "MapServerManager.h"
#include "..\ggsvr\ggsvr.h"
#include "QuestInfo.h"
#include "SkillHitBox.h"
#include "EventManagement.h"
#include "..\common\SetItemOption.h"
#include "EDSProtocol.h"
#if( ENABLETEST_MONSTERITEMDROPEX == 1 )
#include "MonsterItemMngEx.h"
#else
#include "MonsterItemMng.h"
#endif//ENABLETEST_MONSTERITEMDROPEX
#include "ConMember.h"
#include "BattleSoccerManager.h"
#include "GuildClass.h"
#include "MoveCommand.h"
#include "ChaosBox.h"
#include "BloodCastle.h"
#include "Shop.h"
#include "ItemAddOption.h"
#include "CrywolfSync.h"
#include "Kanturu.h"
#include "KanturuMonsterMng.h"
#include "SkillAdditionInfo.h"
#include "TMonsterSkillElement.h"
#include "TMonsterSkillUnit.h"
#include "TMonsterSkillManager.h"
#include "TMonsterAIElement.h"
#include "TMonsterAIAutomata.h"
#include "TMonsterAIUnit.h"
#include "TMonsterAIGroup.h"
#include "TNotice.h"
#include "BuffEffect.h"
#include "CashShop.h"
#include "CashItemPeriodSystem.h"
#include "CashLotterySystem.h"
#include "CastleDeepEvent.h"
#include "CastleSiege.h"
#include "Crywolf.h"
#include "CrywolfAltar.h"
#include "BloodCastle.h"
#include "IllusionTempleEvent.h"
#include "MasterSkillSystem.h"
#include "SocketItemOption.h"
#include "Raklion.h"
#include "PCBangPointSystem.h"
#include "GameEvent.h"
#include "Gambling.h"
#include "ImperialGuardian.h"
#include "DoubleGoerEvent.h"
#include "MonsterStatCalc.h"
#include "ElementalSystem.h"
#include "MasterLevelSkillTreeSystem.h"
#include "LuckyItem.h"
#include "MiningSystem.h"
#include "MiniMap.h"
#include "CustomManager.h"
#include "EventItemBag.h"
#if (ENABLETEST_ZTLUA == 1)
#include "ZTLuaManager.h"
#endif
#if (ENABLETEST_MUUN == 1)
#include "MuunSystem.h"
#endif
#if (ENABLETEST_NEWPVP == 1)
#include "NewPVP.h"
#endif
//------------------------------------------
// GameMain.cpp Variables
//------------------------------------------

BOOL JoinServerConnected;
BOOL DataServerConnected;
BOOL GameServerCreated;
BOOL IsDevilSquareEventConnected;
BOOL IsEventChipServerConnected;
CDragonEvent * DragonEvent;
CAttackEvent * AttackEvent;

BOOL SpeedHackPlayerBlock;

bool g_ConnectMemberLoad;
bool g_ConnectMemberReload;
int g_ConnectMemberReloadTime;

int  gServerType;
int  gPartition;
BOOL gApplyHeroSystem;
int  gSpeedHackPenalty;
BOOL gEnableEventNPCTalk;
BOOL gEnableServerDivision;
int  gMonsterHp;
BOOL gMerryXMasNpcEvent;
BOOL gHappyNewYearNpcEvent;
char gHackLogServerIp[20];
char gStalkProtocolId[11];
BOOL gNonPK;
BOOL gPkLimitFree;
BOOL gNewServer;
BOOL gFireCrackerEvent;
BOOL gHeartOfLoveEvent;
BOOL gMedalEvent;
BOOL gEventChipEvent;
BOOL gDevilSquareEvent;
BOOL g_bStoneItemDrop;
BOOL g_bDoRingEvent;
BOOL g_bDoXMasEvent;
int g_iXMasItemDropRate;
int g_iXMasZenDrop;
BOOL g_bEventManagerOn;
int  g_iKundunMarkDropRate;
int  g_iMarkOfTheLord;
int  g_iDarkLordHeartDropRate;
int  g_iDarkLordHeartOffEventRate;
int g_iMysteriousBeadDropRate1;
int g_iMysteriousBeadDropRate2;
BOOL bIsIgnorePacketSpeedHackDetect;
BOOL gIsKickDetecHackCountLimit;

BOOL gIsItemDropRingOfTransform;
BOOL gIsEledoradoEvent;
BOOL gDoPShopOpen;
BOOL gDisconnectHackUser;
int g_iBlockKanturuMapEnter;
int g_iBlockCastleSiegeMapEnter;
BOOL GSInfoSendFlag;

BYTE g_MultilangCharacterName = 0;
BYTE g_MultilangGuildName = 0;

int gItemDisapearTime;

#if (GS_CASTLE==1)
BOOL gIsDropSetItemInCastleHuntZone;
int gSetItemInCastleHuntZoneDropRate;
int gSetItemInCastleHuntZoneDropLevel;
BOOL g_bDoCastleDeepEvent;
#endif

int g_bAbilityDebug = 0;

///////////////////////////////////////////////////////////////////////////////



int  GameServerPort;
int  JoinServerPort;
char JoinServerIP[256];
int  DataServerPort;
char DataServerIP[256];
int  DataServerPort2;
char DataServerIP2[256];
int  ExDbPort;
char ExDBServerIP[256];
int	RankingServerPort;
char RankingServerIP[256];
int EventServerPort;
char EventServerIP[256];


CwsGameServer wsGServer;	// line : 213GameServer
wsJoinServerCli wsJServerCli;	// line : 214 Join Server
wsJoinServerCli wsDataCli;	// line : 215 DataServer
wsJoinServerCli wsExDbCli;	// line : 239 Extra DataBase Server
wsJoinServerCli wsRServerCli;	// line : 244 Ranking Server
wsJoinServerCli wsEvenChipServerCli; // line : 246 EVENT_MU2003

CDirPath gDirPath;	// line : 248 Directory Path
MapClass MapC[MAX_NUMBER_MAP];	// line 249	// Map Manager
CMonsterAttr gMAttr;	// line 250
CMonsterSetBase gMSetBase;	// line 251
classdef DCInfo;	// line 252:
CWhisperCash WhisperCash;	// line 253
PartyClass gParty;	// line 254
CDbSave gDbSave;	// line 255
WzUdp gUdpSoc;	// line 256
CGuildClass Guild;	// line 265
NSerialCheck gNSerialCheck[OBJMAX];	// line 326
CLoginCount gLCount[3];	// line 329

DWORD gLevelExperience[MAX_CHAR_LEVEL+1];
char szGameServerExeSerial[24];
char szServerName[50];
char szAuthKey[20];

BOOL gIsDropDarkLordItem;
int  gSleeveOfLordDropRate;
int  gSleeveOfLordDropLevel;

int  gSoulOfDarkHorseDropRate;
int  gSoulOfDarkHorseropLevel;
int  gSoulOfDarkSpiritDropRate;
int  gSoulOfDarkSpiritDropLevel;
float gDarkSpiritAddExperience;
BOOL gIsDropGemOfDefend;
int  gGemOfDefendDropRate;
int  gGemOfDefendDropLevel;
int  g_iUseCharacterAutoRecuperationSystem;
int  g_iCharacterRecuperationMaxLevel;
int g_iServerGroupGuildChatting;
int g_iServerGroupUnionChatting;
BOOL g_bRibbonBoxEvent;
int g_iRedRibbonBoxDropLevelMin;
int g_iRedRibbonBoxDropLevelMax;
int g_iRedRibbonBoxDropRate;
int g_iGreenRibbonBoxDropLevelMin;
int g_iGreenRibbonBoxDropLevelMax;
int g_iGreenRibbonBoxDropRate;
int g_iBlueRibbonBoxDropLevelMin;
int g_iBlueRibbonBoxDropLevelMax;
int g_iBlueRibbonBoxDropRate;
BOOL g_bChocolateBoxEvent;
int g_iPinkChocolateBoxDropLevelMin;
int g_iPinkChocolateBoxDropLevelMax;
int g_iPinkChocolateBoxDropRate;
int g_iRedChocolateBoxDropLevelMin;
int g_iRedChocolateBoxDropLevelMax;
int g_iRedChocolateBoxDropRate;
int g_iBlueChocolateBoxDropLevelMin;
int g_iBlueChocolateBoxDropLevelMax;
int g_iBlueChocolateBoxDropRate;
BOOL g_bCandyBoxEvent;
int g_iLightPurpleCandyBoxDropLevelMin;
int g_iLightPurpleCandyBoxDropLevelMax;
int g_iLightPurpleCandyBoxDropRate;
int g_iVermilionCandyBoxDropLevelMin;
int g_iVermilionCandyBoxDropLevelMax;
int g_iVermilionCandyBoxDropRate;
int g_iDeepBlueCandyBoxDropLevelMin;
int g_iDeepBlueCandyBoxDropLevelMax;
int g_iDeepBlueCandyBoxDropRate;
BOOL g_bFenrirStuffItemDrop;
int g_iFenrirStuff_01_DropLv_Min;
int g_iFenrirStuff_01_DropLv_Max;
int g_iFenrirStuff_01_DropMap;
int g_iFenrirStuff_01_DropRate;
int g_iFenrirStuff_02_DropLv_Min;
int g_iFenrirStuff_02_DropLv_Max;
int g_iFenrirStuff_02_DropMap;
int g_iFenrirStuff_02_DropRate;
int g_iFenrirStuff_03_DropLv_Min;
int g_iFenrirStuff_03_DropLv_Max;
int g_iFenrirStuff_03_DropMap;
int g_iFenrirStuff_03_DropRate;
int g_iFenrirRepairRate;
int g_iFenrirDefaultMaxDurSmall;
int g_iFenrirElfMaxDurSmall;
int g_iFenrir_01Level_MixRate;
int g_iFenrir_02Level_MixRate;
int g_iFenrir_03Level_MixRate;
int g_iCrywolfApplyMvpBenefit;
int g_iCrywolfApplyMvpPenalty;
int g_iSkillDistanceCheck;
int g_iSkillDistanceCheckTemp;
int g_iSkillDistanceKick;
int g_iSkillDistanceKickCount;
DWORD g_iSkillDiatanceKickCheckTime;
BOOL g_bKanturuSpecialItemDropOn;
int g_iKanturuMoonStoneDropRate;
int g_iKanturuJewelOfHarmonyDropRate;

BOOL g_bHallowinDayEventOn;
int g_iHallowinDayEventItemDropRate;
int g_iHallowinDayEventJOLBlessDropRate;
int g_iHallowinDayEventJOLAngerDropRaTe;
int g_iHallowinDayEventJOLScreamDropRate;
int g_iHallowinDayEventJOLFoodDropRate;
int g_iHallowinDayEventJOLDrinkDropRate;
int g_iHallowinDayEventJOLPolymorphRingDropRate;
DWORD dwgCheckSum[MAX_CHECKSUM_KEY];
char connectserverip[20];
int  connectserverport;
short gGameServerCode;
int  gPkTime;
BOOL g_bCastleGuildDestoyLimit;
int  gItemNumberCount;
BOOL gStalkProtocol;
int  gYear;
int  g_iRingOrcKillGiftRate;

CSimpleModulus g_SimpleModulus;

int  gEledoradoGoldGoblenItemDropRate;
int  gEledoradoTitanItemDropRate;
int  gEledoradoGoldDerconItemDropRate;
int  gEledoradoDevilLizardKingItemDropRate;
int  gEledoradoDevilTantarosItemDropRate;
int  gEledoradoGoldGoblenExItemDropRate;
int  gEledoradoTitanExItemDropRate;
int  gEledoradoGoldDerconExItemDropRate;
int  gEledoradoDevilLizardKingExItemDropRate;
int  gEledoradoDevilTantarosExItemDropRate;

#ifdef BEREZNUK
bool g_AllowPKEventBC;
bool g_AllowPKEventDS;
bool g_AllowPKEventIT;
#endif

bool g_SelfDefenseOn;
int g_SelfDefenseTime;

short g_CreateCharLevelRF;
short g_CreateCharLevelSUM;
short g_CreateCharLevelDL;
short g_CreateCharLevelMG;

int  giKundunRefillHPSec;
int  giKundunRefillHP;
int  giKundunRefillHPTime;
int  giKundunHPLogSaveTime;
int g_ShieldSystemOn;
int g_iDamageDevideToSDRate;
int g_iDamageDevideToHPRate;
float g_fSuccessAttackRateOption;
int g_ShieldAutoRefillOn;
int g_ShieldAutoRefillOnSafeZone;
int g_PKLevelIncreaseOff;
int g_CompoundPotionDropOn;
int g_iCompoundPotionLv1DropRate;
int g_iCompoundPotionLv2DropRate;
int g_iCompoundPotionLv3DropRate;
int g_iCompoundPotionLv1DropLevel;
int g_iCompoundPotionLv2DropLevel;
int g_iCompoundPotionLv3DropLevel;
BOOL g_bShieldComboMissOptionOn;
int g_iShieldPotionLv1MixSuccessRate;
int g_iShieldPotionLv1MixMoney;
int g_iShieldPotionLv2MixSuccessRate;
int g_iShieldPotionLv2MixMoney;
int g_iShieldPotionLv3MixSuccessRate;
int g_iShieldPotionLv3MixMoney;
int g_iShieldGageConstA;
int g_iShieldGageConstB;
char gMapName[MAX_NUMBER_MAP][64];

////////////////////////////////////////////////////////////////////

char szGameServerVersion[12]=GAMESERVER_VERSION;
char szClientVersion[8]="000000";
BOOL bCanTrade=1;
BOOL bCanChaosBox=1;
BOOL bCanWarehouseLock=1;
// Here Appears Ring Data : 
int  MapMinUserLevel[MAX_NUMBER_MAP] = { 0, 20, 15, 10, 80, 0, 0, 60, 130, 0, 160, 10, 36, 80, 130, 170, 210,
										310, 15, 30, 100, 160, 220, 280, 15, 50, 120, 180, 240, 300, 10, 10, 0};
BOOL gEnableBattleSoccer=1;
int  gLootingTime=3;
int  gItemDropPer=10;
char gCountryName[20]="Kor";
int  gCharacterDeleteMinLevel=40;
BOOL gCreateCharacter=1;
BOOL gGuildCreate=1;
BOOL gGuildDestroy=1;
int  gGuildCreateLevel=100;
int	 g_GuildCreateReset = 0;
int  g_GuildMaxMember = 80;
int  g_GuildMaxAllianceMember = 5;
int  g_CastleMinGuildMember = 20;
BOOL gItemSerialCheck=1;
float  gAddExperience=1.0f;
float  gAddZen = 1.0f;
int  gFireCrackerDropRate=80;
int  gHeartOfLoveDropRate=80;
int  gGoldMedalDropRate=2;
int  gSilverMedalDropRate=2;;
int  gBoxOfGoldDropRate=2;
int  gEyesOfDevilSquareDropRate=2;
int  gKeyOfDevilSquareDropRate=2;
int  gDQChaosSuccessRateLevel1=75;
int  gDQChaosSuccessRateLevel2=70;
int  gDQChaosSuccessRateLevel3=65;
int  gDQChaosSuccessRateLevel4=60;
int  gDQChaosSuccessRateLevel5=55;
int  gDQChaosSuccessRateLevel6=50;
int  gDQChaosSuccessRateLevel7=45;
BOOL g_bBloodCastle=1;
int  g_iStoneDropRate=60;
int  g_iAngelKingsPaperDropRate=10;
int  g_iBloodBoneDropRate=20;
BOOL g_bChaosCastle=1;
DWORD  gAttackSpeedTimeLimit=130;
DWORD  gHackCheckCount=5;
float gDecTimePerAttackSpeed=5.33f;
int  gMinimumAttackSpeedTime=8;
int  gDetectedHackKickCount=10;

BOOL gItemDropRingOfTransform=1;
int  gQuestNPCTeleportTime=60;
DWORD gEledoradoGoldGoblenRegenTime=60;
DWORD gEledoradoTitanRegenTime=60;
DWORD gEledoradoGoldDerconRegenTime=60;
DWORD gEledoradoDevilLizardKingRegenTime=60;
DWORD gEledoradoDevilTantarosRegenTime=60;

DWORD gEledoradoGoldenRabbitRegenTime = 60;
DWORD gEledoradoGoldenDarkKnightRegenTime = 60;
DWORD gEledoradoGoldenDevilRegenTime = 60;
DWORD gEledoradoGoldenMonsterRegenTime = 60;
DWORD gEledoradoGoldenCrustRegenTime = 60;
DWORD gEledoradoGoldenSatirosRegenTime = 60;
DWORD gEledoradoGoldenTwintailRegenTime = 60;
DWORD gEledoradoGoldenIronKnightRegenTime = 60;
DWORD gEledoradoGoldenNeipinRegenTime = 60;
DWORD gEledoradoGoldenGreatDragonRegenTime = 60;

int  gZenDurationTime=30;
int gMonsterHPAdjust = 100;
BOOL gEnableCheckPenetrationSkill=TRUE;
int g_NPCBuffLevel;


BOOL g_bIllusionTemple = FALSE;
int g_OldScrollDropRate = 0;
int g_CovenantDropRate = 0;
int g_bMaxLevelIllusion = 0;
int g_bSaveIllusionRank = 0;
int g_IllusionRewardPerc = 0;
int g_IllusionUseSkillCnt = 3;

int g_bSantaRingDrop;
int g_iSantaRingDropRate;
int g_bNewYearMonsterEvent;
int g_iStoneOfCondorDropRate;
int g_bCherryBlossomEvent;
int g_iCherryBlossomDropRate;

BOOL g_bLuckyCointEvent;
int g_iLuckyCointDropRate;

int g_iML_OldScrollDropRate;
int g_iML_CovenantOfIllusionDropRate;
int g_iML_AngelKingsPaperDropRate;
int g_iML_BloodBoneDropRate;
int g_iML_EyesOfDevilSquareDropRate;
int g_iML_KeyOfDevilSquareDropRate;

BOOL g_bRaklionEvent = TRUE;

int g_bMuBotSystemOn;
int g_iMuBotSystemUserLevel;
int g_iMuBotTimeInterval;
int g_iMuBotStage1Zen;
int g_iMuBotStage2Zen;
int g_iMuBotStage3Zen;
int g_iMuBotStage4Zen;
int g_iMuBotStage5Zen;

int g_iDeathKingBoneRate;
int g_iHellMaineLeatherRate;
int g_iFlameOfPhoenixRate;
int g_iDeathBeamKnightSoulRate;
int g_MaxStatValue;

int g_MonsterLuckItemRollRate;
int g_MonsterSkillItemRollRate;
int g_MonsterOptionItemRollRate;
int g_MonsterExcellentItemRollRate;

void CheckSumFileLoad(char* szCheckSum);

void gSetDate() // Good
{
	tm *today;
	time_t ltime;

	time(&ltime);
	today=localtime(&ltime);
	
	today->tm_year=today->tm_year+1900;
	gYear=today->tm_year;
}

BOOL gJoomin15Check(char* szJN) // Status : Good
{
	int tyear=1900;
	char szTyear[3]="";
	
	if ( szJN[6] == 51 || szJN[6] == 52)
	{
		tyear = 2000;
	}

	memcpy(&szTyear[0], szJN, sizeof(szTyear)-1);

	tyear=tyear+atoi(&szTyear[0]);
	if ( (gYear-15 ) < tyear )
	{
		return 0;
	}
	return 1;
}
	
BOOL gJoominCheck(char* szJN, int iLimitAge) // Good
{

	if ( iLimitAge <0 )
	{
		return 1;
	}

	int tyear=1900;
	char szTyear[3]="";

	if ( szJN[6] == 51 || szJN[6] == 52)
	{
		tyear = 2000;
	}

	memcpy(&szTyear[0], szJN, sizeof(szTyear)-1);

	tyear=tyear+atoi(&szTyear[0]);
	if ( (gYear-iLimitAge ) < tyear )
	{
		return 0;
	}
	
	return 1;
}	




#include "ZTLuaBackup.h"

void GameMainInit(HWND hWnd)
{
VM_START
	int n;
	int LevelOver_N;
	
	gDirPath.SetFirstPath(FOLDER_DATA);

	srand((DWORD)time(NULL));
	lMsg.Load();
	ReadServerInfo();
	LogInit(TRUE);	// 1 : Enabled 0 : Disabled
	gSetDate();

	g_LuaBackup.Load();
		
	gServerType=GetPrivateProfileInt("GameServerInfo", "ServerType", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gPartition=GetPrivateProfileInt("GameServerInfo", "Partition", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	DragonEvent = new CDragonEvent;
	
	if ( DragonEvent == 0 )
	{
		MsgBox("CDragonEvent %s", lMsg.Get( MSGGET(0,110)) );	// Memory allocation error
		return;
	}

	AttackEvent = new CAttackEvent;
	
	if ( AttackEvent == 0 )
	{
		MsgBox("AttackEvent %s", lMsg.Get(MSGGET(0,110)) );	// Memory allocation error
		return;
	}

	ReadCommonServerInfo();
	g_MapServerManager.LoadData(gDirPath.GetNewPath(FILE_WORLD_MAPSERVER));
	
#if (ENABLETEST_ARCA == 1)
	/*if( g_ArcaBattle.LoadScriptArcaBattle(gDirPath.GetNewPath(FILE_EVENT_ARCA_DATA)) )
	{
		g_ArcaBattle.EnableEvent(g_bIsArcaBattleServer);
        g_ArcaBattle.SetDataLoadState(2);
	}*/
#endif

#if (GS_CASTLE ==1)

	if(g_CastleSiege.Ready(g_MapServerManager.GetMapSvrGroup()) == TRUE)
	{
		if(g_CastleSiege.LoadData(gDirPath.GetNewPath(FILE_EVENT_CASTLESIEGE)) != FALSE)
		{
			g_CastleSiege.LoadPreFixData(gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_CastleSiege.SetDataLoadState(2);
		}
	}
#endif

#if (CUSTOM_OLYMP==1)

	if( g_CustomOlymp.Ready(g_MapServerManager.GetMapSvrGroup()) == TRUE )
	{
		if( g_CustomOlymp.LoadData(gDirPath.GetNewPath("Custom\\Olymp.dat")) )
		{
			g_CustomOlymp.LoadPreFixData(gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_CustomOlymp.SetDataLoadState(2);
		}
	}
#endif

	gObjInit();
	InitBattleSoccer();
	gMAttr.LoadAttr(gDirPath.GetNewPath(FILE_MONSTER_MAIN));
	gMSetBase.LoadSetBase(gDirPath.GetNewPath(FILE_MONSTER_BASE), -1);
	
#if( ENABLETEST_MONSTERITEMDROPEX == 1 )
	g_MonsterItemMngEx.Init();
#else
	g_MonsterItemMng.Init();
#endif//ENABLETEST_MONSTERITEMDROPEX

	gLevelExperience[0]=0;

#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());

	LuaMng->Call("getExperienceTable", 0);

	for (int i = 0; i < MAX_CHAR_LEVEL + 1; i++) {
		luabridge::LuaRef ExpValue = (*LuaMng->getRef())[i];
		gLevelExperience[i] = ExpValue.cast<DWORD>();
	}

	ZTLuaManager::Delete(LuaMng);
#else
	LevelOver_N=1;

	for ( n=1;n<MAX_CHAR_LEVEL+1;n++)
	{
		gLevelExperience[n] = (((n+9)*n)*n)*10;

		if ( n > 255 )
		{
			gLevelExperience[n] += ((((LevelOver_N+9)*LevelOver_N)*LevelOver_N)*1000);
			LevelOver_N++;
		}
	}
#endif

//	g_MasterExp.SetExpTable();
	
#if (GS_CASTLE == 1)
	g_Crywolf.LoadCrywolfMapAttr(gDirPath.GetNewPath(g_MapAttrInfo.GetFile(34, 0)),0);
	g_Crywolf.LoadCrywolfMapAttr(gDirPath.GetNewPath(g_MapAttrInfo.GetFile(34, 1)),1);
	g_Crywolf.LoadCrywolfMapAttr(gDirPath.GetNewPath(g_MapAttrInfo.GetFile(34, 2)),2);
#endif
#if (GS_CASTLE == 0 || GS_SPEC == 1)
	g_Kanturu.LoadKanturuMapAttr(gDirPath.GetNewPath(g_MapAttrInfo.GetFile(39, 0)), 0);
	g_Kanturu.LoadKanturuMapAttr(gDirPath.GetNewPath(g_MapAttrInfo.GetFile(39, 1)), 1);
#endif
	DCInfo.Init();
	wsGServer.CreateSocket(hWnd);
	wsJServerCli.CreateSocket(hWnd);
	wsDataCli.CreateSocket(hWnd);
	wsRServerCli.CreateSocket(hWnd);
	wsEvenChipServerCli.CreateSocket(hWnd);
	GameServerInfoSend();
	//GameMonsterAllAdd();
	gMSetBase.AddAllMonsters(LOAD_ALLOBJ);
	ConMember.Load(gDirPath.GetNewPath(FILE_WHITELIST));
	
	g_Raklion.SetState(RAKLION_STATE_END);

VM_END
}


int GetWarehouseUsedHowMuch(int UserLevel,int MasterLevel, BOOL IsLock)
{
	int rZen=0;

	int Level = UserLevel + MasterLevel;

	rZen = (int)((Level * Level)* 0.1 * 0.4);

	if (bCanWarehouseLock == TRUE)
	{
		if ( IsLock != 0)
		{
			rZen = rZen + ( UserLevel * 2 );
		}
	}
	if ( rZen < 1 )
	{
		rZen=1;
	}

	if ( rZen >= 1000 )
	{
		rZen = (rZen/100)*100;
	}
	
	else if ( rZen >= 100 )
	{
		rZen = (rZen/10)*10;
	}
	
	if ( rZen == 0 )
	{
		rZen=1;
	}
	
	return rZen;
}

void GraphPaint(HWND hWnd)
{
	HDC hdc;
	RECT rect;
	int val[10];
	HBRUSH colBrush[4];
	int n;

	hdc = GetDC(hWnd);

	colBrush[0]=CreateSolidBrush( RGB(100, 100, 100));
	colBrush[1]=CreateSolidBrush( RGB(200, 100, 100));
	colBrush[2]=CreateSolidBrush( RGB(200, 200, 200));
	colBrush[3]=CreateSolidBrush( RGB(200, 200, 100));

	for ( n=0; n<4;n++)
	{
		val[0] = DCInfo.DefClass[n].Strength;
		val[1] = DCInfo.DefClass[n].Dexterity ;
		val[2] = DCInfo.DefClass[n].Vitality;
		val[3] = DCInfo.DefClass[n].Energy;
		val[4] = (int)(DCInfo.DefClass[n].Life);
		val[5] = (int)(DCInfo.DefClass[n].Mana);
		val[6] = (int)(DCInfo.DefClass[n].LevelLife * 10.0f);
		val[7] = (int)(DCInfo.DefClass[n].LevelMana * 10.0f);
		val[8] = (int)(DCInfo.DefClass[n].VitalityToLife * 10.0f);
		val[9] = (int)(DCInfo.DefClass[n].EnergyToMana * 10.0f);

		for (int k=0; k<10;k++)
		{
			rect.left = (k * 45) + (n * 10);
			rect.right = ((k * 45) + (n * 10))+10;
			rect.top = 0;
			rect.bottom = val[k];
			FillRect(hdc, &rect, colBrush[n]);
		}
	}
	
	ReleaseDC(hWnd, hdc);
	DeleteObject( (HGDIOBJ)colBrush[0]);
	DeleteObject( (HGDIOBJ)colBrush[1]);
	DeleteObject( (HGDIOBJ)colBrush[2]);
	DeleteObject( (HGDIOBJ)colBrush[3]);
	DeleteObject( (HGDIOBJ)val[0]);



}

void GameMonsterAllAdd()
{
	gCurPaintPlayer = 0;

	int result;

	for ( int n=0;n< gMSetBase.m_Count ;n++)
	{
		if ( DS_MAP_RANGE(gMSetBase.m_Mp[n].m_MapNumber) != FALSE )
		{
			continue;
		}

		if ( BC_MAP_RANGE(gMSetBase.m_Mp[n].m_MapNumber) != FALSE )
		{
			if ( gMSetBase.m_Mp[n].m_Type != 232 )
			{
				continue;
			}
		}

		if (DG_MAP_RANGE(gMSetBase.m_Mp[n].m_MapNumber) != FALSE )
		{
			continue;
		}

		if ( CC_MAP_RANGE(gMSetBase.m_Mp[n].m_MapNumber) != FALSE )
		{
			continue;
		}

		if ( IT_MAP_RANGE(gMSetBase.m_Mp[n].m_MapNumber) != FALSE )
		{
			if( gMSetBase.m_Mp[n].m_Type != 381 && gMSetBase.m_Mp[n].m_Type != 382 )
			{
				if( gMSetBase.m_Mp[n].m_Type == 380 ||
					gMSetBase.m_Mp[n].m_Type == 383 ||
					gMSetBase.m_Mp[n].m_Type == 384 )
				{
					g_IllusionTemple.AddNpc(gMSetBase.m_Mp[n].m_MapNumber,gMSetBase.m_Mp[n].m_Type,n);
				}
				else if( gMSetBase.m_Mp[n].m_Type >= 386 ||
						 gMSetBase.m_Mp[n].m_Type <= 403 )
				{
					g_IllusionTemple.AddMonster(gMSetBase.m_Mp[n].m_MapNumber,gMSetBase.m_Mp[n].m_Type,n);
				}
				continue;
			}
		}		
			
		result = gObjAddMonster(gMSetBase.m_Mp[n].m_MapNumber);

		if ( result >= 0 )
		{
			gObjSetPosMonster(result, n);
			gObjSetMonster(result, gMSetBase.m_Mp[n].m_Type);
			gCurPaintPlayer++;

			if ( BC_MAP_RANGE(gObj[result].MapNumber) )
			{
				if ( gObj[result].Class == 232 )
				{
					gObj[result].m_cBloodCastleIndex = g_BloodCastle.GetBridgeLevel(gObj[result].MapNumber);
				}
			}

#if (GS_CASTLE == 1)
			if( gObj[result].Class == 216 )
			{
				g_CastleSiege.SetCrownIndex(result);
			}

			if( ((gObj[result].MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE) ? TRUE : FALSE) && gObj[result].Type == OBJ_NPC)
			{
				if( gObj[result].Class == 406 || gObj[result].Class == 407 )
				{

				}
				else if(CRYWOLF_NPC_CLASS_RANGE(gObj[result].Class) != FALSE)
				{
					g_Crywolf.m_ObjSpecialNPC.AddObj(result);
				}
				else
				{
					g_Crywolf.m_ObjCommonNPC.AddObj(result);
				}
			}

			if( ((gObj[result].MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE) ? TRUE : FALSE) && gObj[result].Type == OBJ_MONSTER)
			{
				g_Crywolf.m_ObjCommonMonster.AddObj(result);
			}
#endif
		}
	}

#if (GS_CASTLE == 1 && GS_SPEC == 0)
	gDevilSquareEvent = FALSE;
	g_bChaosCastle  = FALSE;
#endif

	g_DevilSquare.Init();
	g_BloodCastle.LoadItemDropRate();

#if (GS_CASTLE == 1 && GS_SPEC == 0)
	g_bBloodCastle = FALSE;
#endif

	if ( g_bBloodCastle != FALSE )
	{
		g_BloodCastle.Init(TRUE);
	}
	else
	{
		g_BloodCastle.Init(FALSE);
	}

	if ( g_bChaosCastle != FALSE )
	{
		g_ChaosCastle.Init(true);
	}
	else
	{
		g_ChaosCastle.Init(false);
	}

	if( g_bIllusionTemple )
	{
		g_IllusionTemple.Init();
	}

	g_Raklion.SetState(RAKLION_STATE_END);
}







void GameMonsterAllCloseAndReLoad()
{
	for ( int n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Type == OBJ_MONSTER || gObj[n].Type == OBJ_NPC )
		{
#if (GS_CASTLE == 1)
			if ( gObj[n].m_btCsNpcType != FALSE )
				continue;
#endif
#if (GS_CASTLE == 0 || GS_SPEC == 1)
			if ( gObj[n].MapNumber == MAP_INDEX_KANTURU_BOSS )
			{
				continue;
			}
#endif

			if ( gObj[n].m_iCurrentAI )
			{
				gObj[n].Live = FALSE;
				gObjViewportListProtocolDestroy(&gObj[n]);
				gObjViewportClose(&gObj[n]);
			}
			
			gObjDel(n);
		}
	}
#if (GS_CASTLE == 1)
	g_Crywolf.m_ObjCommonNPC.Reset();
	g_Crywolf.m_ObjSpecialNPC.Reset();
	g_Crywolf.m_ObjCommonMonster.Reset();
	g_Crywolf.m_ObjSpecialMonster.Reset();
#endif

	gMAttr.LoadAttr(gDirPath.GetNewPath(FILE_MONSTER_MAIN));
	gMSetBase.LoadSetBase(gDirPath.GetNewPath(FILE_MONSTER_BASE), -1);

	g_IllusionTemple.ResetObject();

#if( ENABLETEST_MONSTERITEMDROPEX == 1 )
	g_MonsterItemMngEx.Init();
#else
	g_MonsterItemMng.Init();
#endif//ENABLETEST_MONSTERITEMDROPEX

	gObjMonCount = 0;
	//GameMonsterAllAdd();
	gMSetBase.AddAllMonsters(LOAD_ALLOBJ);
	g_Raklion.SetState(RAKLION_STATE_END);
}





void GameMainFree()
{
	gDbSave.End();
	GDGameServerInfoSave();
	gObjEnd();
	ClearBattleSoccer();
	LogClose();
}


BOOL GMJoinServerConnect(LPSTR ConnectServer, DWORD wMsg)
{
	wsJServerCli.SetProtocolCore(SProtocolCore);
	int result = wsJServerCli.Connect(ConnectServer, JoinServerPort, wMsg);

	if ( result == 0 )
	{
		return FALSE;
	}

	GJServerLogin();

	LogAdd( lMsg.Get( MSGGET(1, 145)), ConnectServer);
	return TRUE;
}

BOOL GMRankingServerConnect(char* RankingServer, DWORD wMsg)
{
	return false;
	/*wsRServerCli.SetProtocolCore(DevilSquareProtocolCore);
	int result = wsRServerCli.Connect(RankingServer, RankingServerPort, wMsg);
	if ( result == 0 )
	{
		return FALSE;
	}
	LogAdd("RankingServer Connect IP [ %s ]", RankingServer);
	return TRUE;*/
}

BOOL GMEventChipServerConnect(char* ServerIP, DWORD wMsg)
{
	wsEvenChipServerCli.SetProtocolCore(EventChipEventProtocolCore);
	int result = wsEvenChipServerCli.Connect(ServerIP, EventServerPort, wMsg);
	if ( result == 0 )
	{
		return FALSE;
	}

	LogAdd("EventChip Connect IP [ %s ]", ServerIP);
	return TRUE;
}


BOOL GMDataServerConnect(char* ConnectServer, WPARAM wMsg)
{
	wsDataCli.SetProtocolCore(DataServerProtocolCore);
	int result = wsDataCli.Connect(ConnectServer, DataServerPort, wMsg);
	if ( result == 0 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 147)), ConnectServer, DataServerPort);
		return FALSE;
	}
	DataServerLogin(0);
	LogAdd(lMsg.Get(MSGGET(1, 146)), ConnectServer, DataServerPort);
	return TRUE;
}

BOOL ExDataServerConnect(char* ConnectServer, DWORD wMsg)
{
	wsExDbCli.SetProtocolCore(ExDataServerProtocolCore);
	int result = wsExDbCli.Connect(ConnectServer, ExDbPort, wMsg);
	if ( result == 0 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 147)), ConnectServer, ExDbPort);
		return FALSE;
	}

	ExDataServerLogin();
//	LogAddC(2, "ExDB Server Online");
	return TRUE;
}

BOOL GameMainServerCreate(DWORD sMsg, DWORD cMsg)
{
	gDbSave.Begin();
	LogAdd( lMsg.Get( MSGGET(1, 148)), GameServerPort);
	return TRUE;
}


void GMServerMsgProc( WPARAM wParam, LPARAM lParam)
{
	{
		int unk =lParam & 0xFFFF & 0xFFFF;	// HERE is a switch here goes a macro
	}
}

void GMClientMsgProc(WPARAM wParam, LPARAM lParam)
{
	int wp = wParam;
	int lp = lParam;

	switch ( lParam & 0xFFFF & 0xFFFF)
	{
		SOCKET socket;

		case 1:
			wsGServer.DataRecv(wParam);
			break;

		case 2:
			wsGServer.FDWRITE_MsgDataSend(wParam);
			break;

		case 32:
			socket = gObjGetSocket(wParam);
			break;

	}
}




void GMJoinClientMsgProc(WPARAM wParam, LPARAM lParam)
{
	switch ( lParam & 0xFFFF & 0xFFFF )
	{
		case 1:
			wsJServerCli.DataRecv();
			break;

		case 2:
			wsJServerCli.FDWRITE_MsgDataSend();
			break;

		case 32:
			LogAddC(2, lMsg.Get(MSGGET(1, 149)));
			SendMessage(ghWnd, WM_CLOSE, 0, 0);
	}
}






void GMRankingClientMsgProc(WPARAM wParam, LPARAM lParam)
{
	switch ( lParam & 0xFFFF & 0xFFFF )
	{
		case 1:
			wsRServerCli.DataRecv();
			break;

		case 2:
			wsRServerCli.FDWRITE_MsgDataSend();
			break;

		case 32:
			IsDevilSquareEventConnected = FALSE;
	}
}






void GMEventChipClientMsgProc(WPARAM wParam, LPARAM lParam)
{
	switch ( lParam & 0xFFFF & 0xFFFF )
	{
		case 1:
			wsEvenChipServerCli.DataRecv();
			break;

		case 2:
			wsEvenChipServerCli.FDWRITE_MsgDataSend();
			break;

		case 32:
			IsEventChipServerConnected = FALSE;
	}
}




void ExDataClientMsgProc(WPARAM wParam, LPARAM lParam)
{
	switch ( lParam & 0xFFFF & 0xFFFF )
	{
		case 1:
			wsExDbCli.DataRecv();
			break;

		case 2:
			wsExDbCli.FDWRITE_MsgDataSend();
			break;

		case 32:
			LogAddC(2, lMsg.Get(MSGGET(1, 150)));
			wsExDbCli.Close();

			for ( int i =0;i<OBJMAX;i++)
			{
				if ( gObjIsConnectedGP(i) == TRUE )
				{
					PMSG_FRIEND_STATE pMsg;
					
					pMsg.h.set((LPBYTE)&pMsg, 0xC4 , sizeof(pMsg) );
					pMsg.State = -4;

					DataSend(i, (unsigned char*)&pMsg, sizeof(pMsg) );

					gObj[i].m_FriendServerOnline = FRIEND_SERVER_STATE_OFFLINE;
				}
			}

			LogAddTD("Error-L1 : Friend Server Down (State Send Ok)");
			LogAddC(2, "Error-L1 : Friend Server Down (State Send Ok)");
			break;
	}
}






void GMDataClientMsgProc(WPARAM wParam, LPARAM lParam)
{
	switch ( lParam & 0xFFFF & 0xFFFF )
	{
		case 1:
			wsDataCli.DataRecv();
			break;

		case 2:
			wsDataCli.FDWRITE_MsgDataSend();
			break;

		case 32:
			LogAddC(2, lMsg.Get(MSGGET(1, 150)));
	}
}





void ReadServerInfo()
{
	char StringTemp[256];
	LPSTR ClientVersion;

	if (!IsFile(gDirPath.GetNewPath(FILE_SERVER_INFO)))
	{
		MsgBox("ServerInfo.dat file not found");
	}

	GetPrivateProfileString("GameServerInfo","ServerName","",szServerName,50,gDirPath.GetNewPath(FILE_SERVER_INFO));
	gGameServerCode=GetPrivateProfileInt("GameServerInfo","ServerCode",0,gDirPath.GetNewPath(FILE_SERVER_INFO));
	g_ConnectMemberLoad=GetPrivateProfileInt("GameServerInfo","ConnectMemberLoad",0,gDirPath.GetNewPath(FILE_SERVER_INFO));
	g_ConnectMemberReload=GetPrivateProfileInt("GameServerInfo","ConnectMemberReload",0,gDirPath.GetNewPath(FILE_SERVER_INFO));
	g_ConnectMemberReloadTime=GetPrivateProfileInt("GameServerInfo","ConnectMemberReloadTime",0,gDirPath.GetNewPath(FILE_SERVER_INFO));
	gNonPK = GetPrivateProfileInt("GameServerInfo","NonPK",0, gDirPath.GetNewPath(FILE_SERVER_INFO));
	gPkLimitFree = GetPrivateProfileInt("GameServerInfo","PkLimitFree",0, gDirPath.GetNewPath(FILE_SERVER_INFO));
	gNewServer = GetPrivateProfileInt("GameServerInfo","NewServer",0, gDirPath.GetNewPath(FILE_SERVER_INFO));
	
	GetPrivateProfileString("GameServerInfo", "ClientExeVersion", "1.02.15", StringTemp,8,gDirPath.GetNewPath(FILE_SERVER_INFO));
	GetPrivateProfileString("GameServerInfo", "ClientExeSerial","", szGameServerExeSerial, 20, gDirPath.GetNewPath(FILE_SERVER_INFO));

	ClientVersion = strtok(StringTemp, ".");
	szClientVersion[0] = ClientVersion[0];
	ClientVersion = strtok(NULL, ".");
	szClientVersion[1] = ClientVersion[0];
	szClientVersion[2] = ClientVersion[1];
	ClientVersion = strtok(NULL, ".");
	szClientVersion[3] = ClientVersion[0];
	szClientVersion[4] = ClientVersion[1];

	GetPrivateProfileString("ConnectServerInfo", "IP", "", connectserverip, 20, gDirPath.GetNewPath(FILE_SERVER_INFO));
	GetPrivateProfileString("ConnectServerInfo", "PORT", "", StringTemp, 10, gDirPath.GetNewPath(FILE_SERVER_INFO));
	connectserverport = atoi(StringTemp);
	//LogAddTD(lMsg.Get(MSGGET(1, 158)), connectserverip, connectserverport);
}

void ReadOptionsInfo()
{
	char StringTemp[256];

	if( !IsFile(gDirPath.GetNewPath(FILE_SERVER_COMMON)) )
	{
		MsgBox("[CommonServer.cfg] file not found");
	}

	gStalkProtocol = GetPrivateProfileInt("GameServerInfo","StalkProtocol",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	GetPrivateProfileString("GameServerInfo", "StalkProtocolId", "0", gStalkProtocolId, 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	
	GetPrivateProfileString("GameServerInfo", "CreateCharacter", "1", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gCreateCharacter = atoi(StringTemp);
	
	GetPrivateProfileString("GameServerInfo", "CharacterDeleteMinLevel", "40", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gCharacterDeleteMinLevel = atoi(StringTemp);

	int MaxUserCount = GetPrivateProfileInt("GameServerInfo", "NumberOfMaxUser", 1000, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	if( MaxUserCount >= 0 && MaxUserCount <= OBJMAXUSER )
	{
		gServerMaxUser = MaxUserCount;
		LogAddTD("[ReadOptionsInfo] MaxUser: %d", gServerMaxUser);
	}

	g_iServerGroupGuildChatting = GetPrivateProfileInt("GameServerInfo","ServerGroupGuildChatting",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iServerGroupUnionChatting = GetPrivateProfileInt("GameServerInfo","ServerGroupUnionChatting",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	
	GetPrivateProfileString("GameServerInfo", "GuildCreate", "1", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gGuildCreate = atoi(StringTemp);
	LogAdd(lMsg.Get(MSGGET(2, 57)), gGuildCreate);

	GetPrivateProfileString("GameServerInfo", "GuildDestroy", "1", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gGuildDestroy = atoi(StringTemp);
	LogAdd(lMsg.Get(MSGGET(2, 58)), gGuildDestroy);
	
	GetPrivateProfileString("GameServerInfo", "GuildCreateLevel", "100", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gGuildCreateLevel = atoi(StringTemp);
	LogAdd(lMsg.Get(MSGGET(2, 59)), gGuildCreateLevel);
	
	g_GuildCreateReset = GetPrivateProfileInt("GameServerInfo","GuildCreateReset",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_GuildMaxMember = GetPrivateProfileInt("GameServerInfo","GuildMaxMember",80, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_GuildMaxAllianceMember = GetPrivateProfileInt("GameServerInfo","GuildMaxAllianceMember",5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_CastleMinGuildMember = GetPrivateProfileInt("GameServerInfo","CastleMinGuildMember",20, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_bCastleGuildDestoyLimit = GetPrivateProfileInt("GameServerInfo", "CastleOwnerGuildDestroyLimit", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	GetPrivateProfileString("GameServerInfo", "SpeedHackPlayerBlock", "0", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	SpeedHackPlayerBlock = atoi(StringTemp);
	GetPrivateProfileString("GameServerInfo", "ItemSerialCheck", "1", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gItemSerialCheck = atoi(StringTemp);
	GetPrivateProfileString("GameServerInfo", "AddExperience", "1", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gAddExperience = (float)atof(StringTemp);	
	GetPrivateProfileString("GameServerInfo", "AddZen", "1", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gAddZen = (float)atof(StringTemp);	
	GetPrivateProfileString("GameServerInfo", "Trade", "1", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	bCanTrade = atoi(StringTemp);

	if( bCanTrade != 0 )
	{
		LogAddTD(lMsg.Get(407));
	}
	else
	{
		LogAddTD(lMsg.Get(408));
	}

	bCanChaosBox = GetPrivateProfileInt("GameServerInfo","ChaosBox",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	LogAdd("[Option] ChaosBox = %d", bCanChaosBox);

	GetPrivateProfileString("GameServerInfo", "PKTIME", "1", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gPkTime = atoi(StringTemp);

	if( gPkTime != 0 )
	{
		LogAddTD(lMsg.Get(409), gPkTime);
	}

	GetPrivateProfileString("GameServerInfo", "MonsterHp", "0", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gMonsterHp = atoi(StringTemp);

	if( gMonsterHp != 0 )
	{
		LogAddTD(lMsg.Get(410), gMonsterHp);
	}

	GetPrivateProfileString("GameServerInfo", "LootingTime", "3", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_INFO));
	gLootingTime = atoi(StringTemp);

	if( gLootingTime != 0 )
	{
		LogAddTD("[Option] Auto Looting Time Set : %d", gLootingTime);
	}

	GetPrivateProfileString("GameServerInfo", "ItemDropPer", "10", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gItemDropPer = atoi(StringTemp);
	LogAddTD(lMsg.Get(413), gItemDropPer);

	gZenDurationTime = GetPrivateProfileInt("GameServerInfo","ZenDurationTime",30, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gEventChipEvent = GetPrivateProfileInt("GameServerInfo","EventChipEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gBoxOfGoldDropRate = GetPrivateProfileInt("GameServerInfo","BoxOfGoldDropRate",2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
#ifdef BEREZNUK
	g_AllowPKEventBC = GetPrivateProfileInt("GameServerInfo", "AllowPKEventBC", true, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_AllowPKEventDS = GetPrivateProfileInt("GameServerInfo", "AllowPKEventDS", true, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_AllowPKEventIT = GetPrivateProfileInt("GameServerInfo", "AllowPKEventIT", true, gDirPath.GetNewPath(FILE_SERVER_COMMON));
#endif

	g_SelfDefenseOn = GetPrivateProfileInt("GameServerInfo", "SelfDefenseOn", true, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_SelfDefenseTime = GetPrivateProfileInt("GameServerInfo", "SelfDefenseTime", 60, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	gAttackSpeedTimeLimit = GetPrivateProfileInt("GameServerInfo","AttackSpeedTimeLimit",800, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	bIsIgnorePacketSpeedHackDetect = GetPrivateProfileInt("GameServerInfo","IsIgnorePacketHackDetect",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gHackCheckCount = GetPrivateProfileInt("GameServerInfo","HackCheckCount",5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gMinimumAttackSpeedTime = GetPrivateProfileInt("GameServerInfo","MinimumAttackSpeedTime",200, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gDetectedHackKickCount = GetPrivateProfileInt("GameServerInfo","DetectedHackKickCount",10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gIsKickDetecHackCountLimit = GetPrivateProfileInt("GameServerInfo","IsKickDetecHackCountLimit",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Ring of Transform
	gIsItemDropRingOfTransform = GetPrivateProfileInt("GameServerInfo","IsItemDropRingOfTransform",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gItemDropRingOfTransform = GetPrivateProfileInt("GameServerInfo","ItemDropRingOfTransform",1, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Hack Settinhs
	gDisconnectHackUser = GetPrivateProfileInt("GameServerInfo","DisconnectHackUser",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	GetPrivateProfileString("GameServerInfo", "DecTimePerAttackSpeed", "5.33", StringTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gDecTimePerAttackSpeed = (float)atof(StringTemp);
	gSpeedHackPenalty = GetPrivateProfileInt("GameServerInfo","SpeedHackPenalty",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Devil Square Event
	gDQChaosSuccessRateLevel1 = GetPrivateProfileInt("GameServerInfo","DQChaosSuccessRateLevel1",75, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gDQChaosSuccessRateLevel2 = GetPrivateProfileInt("GameServerInfo","DQChaosSuccessRateLevel2",70, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gDQChaosSuccessRateLevel3 = GetPrivateProfileInt("GameServerInfo","DQChaosSuccessRateLevel3",65, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gDQChaosSuccessRateLevel4 = GetPrivateProfileInt("GameServerInfo","DQChaosSuccessRateLevel4",60, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gDQChaosSuccessRateLevel5 = GetPrivateProfileInt("GameServerInfo","DQChaosSuccessRateLevel5",55, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gDQChaosSuccessRateLevel6 = GetPrivateProfileInt("GameServerInfo","DQChaosSuccessRateLevel6",50, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gDQChaosSuccessRateLevel7 = GetPrivateProfileInt("GameServerInfo","DQChaosSuccessRateLevel7",45, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Marlon Teleport
	gQuestNPCTeleportTime = GetPrivateProfileInt("GameServerInfo","QuestNPCTeleportTime",900, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Mark Lord
	g_iMarkOfTheLord = GetPrivateProfileInt("GameServerInfo","MarkOfTheLord",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Ribbon Box Event
	g_bRibbonBoxEvent = GetPrivateProfileInt("GameServerInfo","RibbonBoxEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Red Ribbon Box
	g_iRedRibbonBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","RedRibbonBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iRedRibbonBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","RedRibbonBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iRedRibbonBoxDropRate = GetPrivateProfileInt("GameServerInfo","RedRibbonBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	// Green Ribbon Box
	g_iGreenRibbonBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","GreenRibbonBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iGreenRibbonBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","GreenRibbonBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iGreenRibbonBoxDropRate = GetPrivateProfileInt("GameServerInfo","GreenRibbonBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	// Blue Ribbon Box
	g_iBlueRibbonBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","BlueRibbonBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iBlueRibbonBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","BlueRibbonBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iBlueRibbonBoxDropRate = GetPrivateProfileInt("GameServerInfo","BlueRibbonBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Chocolate Event
	g_bChocolateBoxEvent = GetPrivateProfileInt("GameServerInfo","ChocolateEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Pink Chocolate Box
	g_iPinkChocolateBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","PinkChocolateBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iPinkChocolateBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","PinkChocolateBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iPinkChocolateBoxDropRate = GetPrivateProfileInt("GameServerInfo","PinkChocolateBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Red Chocolate Box
	g_iRedChocolateBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","RedChocolateBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iRedChocolateBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","RedChocolateBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iRedChocolateBoxDropRate = GetPrivateProfileInt("GameServerInfo","RedChocolateBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Blue Chocolate Box
	g_iBlueChocolateBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","BlueChocolateBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iBlueChocolateBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","BlueChocolateBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iBlueChocolateBoxDropRate = GetPrivateProfileInt("GameServerInfo","BlueChocolateBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Candy Event
	g_bCandyBoxEvent = GetPrivateProfileInt("GameServerInfo","CandyBoxEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// LightPurple Candy Box
	g_iLightPurpleCandyBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","LightPurpleCandyBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iLightPurpleCandyBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","LightPurpleCandyBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iLightPurpleCandyBoxDropRate = GetPrivateProfileInt("GameServerInfo","LightPurpleCandyBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Vermilion Candy Box
	g_iVermilionCandyBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","VermilionCandyBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iVermilionCandyBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","VermilionCandyBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iVermilionCandyBoxDropRate = GetPrivateProfileInt("GameServerInfo","VermilionCandyBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// DeepBlue Candy Box
	g_iDeepBlueCandyBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","DeepBlueCandyBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iDeepBlueCandyBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","DeepBlueCandyBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iDeepBlueCandyBoxDropRate = GetPrivateProfileInt("GameServerInfo","DeepBlueCandyBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// KUNDUN Settings
	giKundunRefillHPSec = GetPrivateProfileInt("GameServerInfo","KundunRefillHPSec",500, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	giKundunRefillHP = GetPrivateProfileInt("GameServerInfo","KundunRefillHP",10000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	giKundunRefillHPTime = GetPrivateProfileInt("GameServerInfo","KundunRefillHPTime",600, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	giKundunHPLogSaveTime = GetPrivateProfileInt("GameServerInfo","KundunHPLogSaveTime",120, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Fenrir Settings
	g_bFenrirStuffItemDrop = GetPrivateProfileInt("GameServerInfo","FenrirStuffItemDrop", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Fenrir Stuff 1
	g_iFenrirStuff_01_DropLv_Min = GetPrivateProfileInt("GameServerInfo","FenrirStuff_01_DropLv_Min", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirStuff_01_DropLv_Max = GetPrivateProfileInt("GameServerInfo","FenrirStuff_01_DropLv_Max", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirStuff_01_DropMap = GetPrivateProfileInt("GameServerInfo","FenrirStuff_01_DropMap", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirStuff_01_DropRate = GetPrivateProfileInt("GameServerInfo","FenrirStuff_01_DropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Fenrir Stuff 2
	g_iFenrirStuff_02_DropLv_Min = GetPrivateProfileInt("GameServerInfo","FenrirStuff_02_DropLv_Min", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirStuff_02_DropLv_Max = GetPrivateProfileInt("GameServerInfo","FenrirStuff_02_DropLv_Max", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirStuff_02_DropMap = GetPrivateProfileInt("GameServerInfo","FenrirStuff_02_DropMap", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirStuff_02_DropRate = GetPrivateProfileInt("GameServerInfo","FenrirStuff_02_DropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Fenrir Stuff 3
	g_iFenrirStuff_03_DropLv_Min = GetPrivateProfileInt("GameServerInfo","FenrirStuff_03_DropLv_Min", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirStuff_03_DropLv_Max = GetPrivateProfileInt("GameServerInfo","FenrirStuff_03_DropLv_Max", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirStuff_03_DropMap = GetPrivateProfileInt("GameServerInfo","FenrirStuff_03_DropMap", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirStuff_03_DropRate = GetPrivateProfileInt("GameServerInfo","FenrirStuff_03_DropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Fenrir Durability Settings
	g_iFenrirRepairRate = GetPrivateProfileInt("GameServerInfo","FenrirRepairRate", 4000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirDefaultMaxDurSmall = GetPrivateProfileInt("GameServerInfo","FenrirDefaultMaxDurSmall", 200, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrirElfMaxDurSmall = GetPrivateProfileInt("GameServerInfo","FenrirElfMaxDurSmall", 160, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Fenrir Mix Rate
	g_iFenrir_01Level_MixRate = GetPrivateProfileInt("GameServerInfo","Fenrir_01Level_MixRate", 70, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrir_02Level_MixRate = GetPrivateProfileInt("GameServerInfo","Fenrir_02Level_MixRate", 50, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFenrir_03Level_MixRate = GetPrivateProfileInt("GameServerInfo","Fenrir_03Level_MixRate", 30, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Crywolf Benefit
	g_iCrywolfApplyMvpBenefit = GetPrivateProfileInt("GameServerInfo","CrywolfApplyMvpBenefit",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	int iCrywolfPlusChaosRateBenefit = GetPrivateProfileInt("GameServerInfo","CrywolfPlusChaosRateBenefit",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	int iCrywolfMonHPRateBenefit = GetPrivateProfileInt("GameServerInfo","CrywolfMonHPRateBenefit", 100, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	int iCrywolfKundunHPRefillState = GetPrivateProfileInt("GameServerInfo","CrywolfKundunHPRefillBenefit", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_CrywolfSync.SetKundunHPRefillState(iCrywolfKundunHPRefillState);
	g_CrywolfSync.SetPlusChaosRate(iCrywolfPlusChaosRateBenefit);
	g_CrywolfSync.SetMonHPBenefitRate(iCrywolfMonHPRateBenefit);

	// Crywolf Penalty
	g_iCrywolfApplyMvpPenalty = GetPrivateProfileInt("GameServerInfo","CrywolfApplyMvpPenalty",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	int iCrywolfGemDropPenaltyRate = GetPrivateProfileInt("GameServerInfo","CrwyolfGemDropPenaltyRate",100, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	int iCrywolfGettingExpPenaltyRate = GetPrivateProfileInt("GameServerInfo","CrwyolfGettingExpPenaltyRate",100, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_CrywolfSync.SetGemDropPenaltiyRate(iCrywolfGemDropPenaltyRate);
	g_CrywolfSync.SetGettingExpPenaltyRate(iCrywolfGettingExpPenaltyRate);

	// Kanturu Event
	g_iBlockKanturuMapEnter = GetPrivateProfileInt("GameServerInfo","BlockKanturuMapEnter",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iBlockCastleSiegeMapEnter = GetPrivateProfileInt("GameServerInfo","BlockCastleSiegeMapEnter",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	int iKanturuEnableValue = GetPrivateProfileInt("GameServerInfo","KanturuEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_Kanturu.SetKanturuEnable(iKanturuEnableValue);

	// Map Enter Settings
	g_iBlockKanturuMapEnter = GetPrivateProfileInt("GameServerInfo","BlockKanturuMapEnter",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iBlockCastleSiegeMapEnter = GetPrivateProfileInt("GameServerInfo","BlockCastleSiegeMapEnter",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Special Drop
	g_bKanturuSpecialItemDropOn = GetPrivateProfileInt("GameServerInfo","KanturuSpecialItemDropOn",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Other
	g_iKanturuMoonStoneDropRate = GetPrivateProfileInt("GameServerInfo","KanturuMoonStoneDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iKanturuJewelOfHarmonyDropRate = GetPrivateProfileInt("GameServerInfo","KanturuJewelOfHarmonyDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Raklion Event
	int iRaklionEnable = GetPrivateProfileInt("GameServerInfo","RaklionEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_Raklion.SetRaklionEnable(iRaklionEnable);

	// Hallowin Event
	g_bHallowinDayEventOn = GetPrivateProfileInt("GameServerInfo","HallowinEventOn",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iHallowinDayEventItemDropRate = GetPrivateProfileInt("GameServerInfo","HallowinEventPumpkinOfLuckDropRate",100, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iHallowinDayEventJOLBlessDropRate = GetPrivateProfileInt("GameServerInfo","HallowinEventJOLBlessDropRate",10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iHallowinDayEventJOLAngerDropRaTe = GetPrivateProfileInt("GameServerInfo","HallowinEventJOLAngerDropRate",15, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iHallowinDayEventJOLScreamDropRate = GetPrivateProfileInt("GameServerInfo","HallowinEventJOLScreamDropRate",15, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iHallowinDayEventJOLFoodDropRate = GetPrivateProfileInt("GameServerInfo","HallowinEventJOLFoodDropRate",30, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iHallowinDayEventJOLDrinkDropRate = GetPrivateProfileInt("GameServerInfo","HallowinEventJOLDrinkDropRate",25, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iHallowinDayEventJOLPolymorphRingDropRate = GetPrivateProfileInt("GameServerInfo","HallowinEventJOLPolymorphRingDropRate", 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_bSantaRingDrop				= GetPrivateProfileInt("GameServerInfo","SantaPolymorphRingDropOn",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iSantaRingDropRate			= GetPrivateProfileInt("GameServerInfo","SantaPolymorphRingDropRate",10,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_bNewYearMonsterEvent			= GetPrivateProfileInt("GameServerInfo","NewYearLuckyBagMonsterEventOn",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_iStoneOfCondorDropRate		= GetPrivateProfileInt("GameServerInfo","CondorFlameDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_bCherryBlossomEvent			= GetPrivateProfileInt("GameServerInfo","CherryBlossomEventOn", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iCherryBlossomDropRate		= GetPrivateProfileInt("GameServerInfo","CherryBlossomEventItemDropRate", 8000, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	BOOL g_bLuckyCointEvent			= GetPrivateProfileInt("GameServerInfo","LuckyCoinEventOn", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	int g_iLuckyCointDropRate		= GetPrivateProfileInt("GameServerInfo","LuckyCoinDropRate", 1000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	
	g_iML_OldScrollDropRate			= GetPrivateProfileInt("GameServerInfo","ML_OldScrollDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iML_CovenantOfIllusionDropRate		= GetPrivateProfileInt("GameServerInfo","ML_CovenantOfIllusionDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iML_AngelKingsPaperDropRate	= GetPrivateProfileInt("GameServerInfo","ML_AngelKingsPaperDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iML_BloodBoneDropRate			= GetPrivateProfileInt("GameServerInfo","ML_BloodBoneDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iML_EyesOfDevilSquareDropRate = GetPrivateProfileInt("GameServerInfo","ML_EyesOfDevilSquareDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iML_KeyOfDevilSquareDropRate	= GetPrivateProfileInt("GameServerInfo","ML_KeyOfDevilSquareDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	
	g_NPCBuffLevel					= GetPrivateProfileInt("GameServerInfo", "ShadowPhantomMaxLevel", 220, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gApplyHeroSystem				= GetPrivateProfileInt("GameServerInfo","ApplyHeroSystem", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gEnableBattleSoccer				= GetPrivateProfileInt("GameServerInfo","EnableBattleSoccer", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gEnableEventNPCTalk				= GetPrivateProfileInt("GameServerInfo","EnableEventNPCTalk", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gEnableServerDivision			= GetPrivateProfileInt("GameServerInfo","EnableServerDivision", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gMonsterHPAdjust				= GetPrivateProfileInt("GameServerInfo","MonsterHPAdjust", 100, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gEnableCheckPenetrationSkill	= GetPrivateProfileInt("GameServerInfo","EnableCheckPenetrationSkill", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_ShieldSystemOn				= GetPrivateProfileInt("GameServerInfo","ShieldSystemOn", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iDamageDevideToSDRate			= GetPrivateProfileInt("GameServerInfo","DamageDevideToSD", 90, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iDamageDevideToHPRate			= GetPrivateProfileInt("GameServerInfo","DamageDevideToHP", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_fSuccessAttackRateOption		= (float)(GetPrivateProfileInt("GameServerInfo","SuccessAttackRateOption", 20000, gDirPath.GetNewPath(FILE_SERVER_COMMON)) / 10000 );
	g_ShieldAutoRefillOn			= GetPrivateProfileInt("GameServerInfo","ShieldAutoRefillOn", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_ShieldAutoRefillOnSafeZone	= GetPrivateProfileInt("GameServerInfo","ShieldAutoRefilOnSafeZone", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_PKLevelIncreaseOff			= GetPrivateProfileInt("GameServerInfo","PKLevelIncreaseOff", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_CompoundPotionDropOn			= GetPrivateProfileInt("GameServerInfo","CompoundPotionDropOn", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iCompoundPotionLv1DropRate	= GetPrivateProfileInt("GameServerInfo","CompoundPotionLv1DropRate", 100, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iCompoundPotionLv2DropRate	= GetPrivateProfileInt("GameServerInfo","CompoundPotionLv2DropRate", 80, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iCompoundPotionLv3DropRate	= GetPrivateProfileInt("GameServerInfo","CompoundPotionLv3DropRate", 50, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iCompoundPotionLv1DropLevel	= GetPrivateProfileInt("GameServerInfo","CompoundPotionLv1DropLevel", 68, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iCompoundPotionLv2DropLevel	= GetPrivateProfileInt("GameServerInfo","CompoundPotionLv2DropLevel", 96, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iCompoundPotionLv3DropLevel	= GetPrivateProfileInt("GameServerInfo","CompoundPotionLv3DropLevel", 118, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_bShieldComboMissOptionOn		= GetPrivateProfileInt("GameServerInfo","ShieldComboMissOptionOn", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_iShieldPotionLv1MixSuccessRate		= GetPrivateProfileInt("GameServerInfo","ShieldPotionLv1MixSuccessRate", 50, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iShieldPotionLv1MixMoney		= GetPrivateProfileInt("GameServerInfo","ShieldPotionLv1MixMoney", 100000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iShieldPotionLv2MixSuccessRate		= GetPrivateProfileInt("GameServerInfo","ShieldPotionLv2MixSuccessRate", 30, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iShieldPotionLv2MixMoney		= GetPrivateProfileInt("GameServerInfo","ShieldPotionLv2MixMoney", 500000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iShieldPotionLv3MixSuccessRate		= GetPrivateProfileInt("GameServerInfo","ShieldPotionLv3MixSuccessRate", 30, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iShieldPotionLv3MixMoney		= GetPrivateProfileInt("GameServerInfo","ShieldPotionLv3MixMoney", 1000000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	
	g_iShieldGageConstA				= GetPrivateProfileInt("GameServerInfo","ShieldGageConstA", 12, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iShieldGageConstB				= GetPrivateProfileInt("GameServerInfo","ShieldGageConstB", 30, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	gDoPShopOpen					= GetPrivateProfileInt("GameServerInfo","PersonalShopOpen", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_iUseCharacterAutoRecuperationSystem	= GetPrivateProfileInt("GameServerInfo","UseCharacterAutoRecuperationSystem", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iCharacterRecuperationMaxLevel		= GetPrivateProfileInt("GameServerInfo","CharacterRecuperationMaxLevel", 100, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_iSkillDistanceCheck			= GetPrivateProfileInt("GameServerInfo","SkillDistanceCheck", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iSkillDistanceCheckTemp		= GetPrivateProfileInt("GameServerInfo","SkillDistanceCheckTemp", 2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iSkillDistanceKick			= GetPrivateProfileInt("GameServerInfo","SkillDistanceKick", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iSkillDistanceKickCount		= GetPrivateProfileInt("GameServerInfo","SkillDistanceKickCount", 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iSkillDiatanceKickCheckTime	= GetPrivateProfileInt("GameServerInfo","SkillDistanceKickCheckTime", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	gItemDisapearTime				= GetPrivateProfileInt("GameServerInfo","ItemDisappearTime", 120, gDirPath.GetNewPath(FILE_SERVER_INFO));
	
	g_bMuBotSystemOn				= GetPrivateProfileInt("GameServerInfo","MuBotSystemOn", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iMuBotSystemUserLevel			= GetPrivateProfileInt("GameServerInfo","MuBotUseLevel", 80, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iMuBotTimeInterval			= GetPrivateProfileInt("GameServerInfo","MuBotIntervalCheck", 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iMuBotStage1Zen				= GetPrivateProfileInt("GameServerInfo","MuBotStage1Zen", 10000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iMuBotStage2Zen				= GetPrivateProfileInt("GameServerInfo","MuBotStage2Zen", 20000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iMuBotStage3Zen				= GetPrivateProfileInt("GameServerInfo","MuBotStage3Zen", 30000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iMuBotStage4Zen				= GetPrivateProfileInt("GameServerInfo","MuBotStage4Zen", 40000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iMuBotStage5Zen				= GetPrivateProfileInt("GameServerInfo","MuBotStage5Zen", 50000, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_iDeathKingBoneRate			= GetPrivateProfileInt("GameServerInfo","DeathKingBone_DropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iHellMaineLeatherRate			= GetPrivateProfileInt("GameServerInfo","HellMaineLeather_DropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iFlameOfPhoenixRate			= GetPrivateProfileInt("GameServerInfo","PhoenixFlame_DropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_iDeathBeamKnightSoulRate		= GetPrivateProfileInt("GameServerInfo","DeathBeamKnightSoul_DropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_MaxStatValue					= GetPrivateProfileInt("GameServerInfo", "MaxStatValue", 32000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	
	g_CreateCharLevelRF				= GetPrivateProfileInt("GameServerInfo", "CreateCharLevelRF", 32000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_CreateCharLevelDL				= GetPrivateProfileInt("GameServerInfo", "CreateCharLevelDL", 32000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_CreateCharLevelMG				= GetPrivateProfileInt("GameServerInfo", "CreateCharLevelMG", 32000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_CreateCharLevelSUM			= GetPrivateProfileInt("GameServerInfo", "CreateCharLevelSUM", 32000, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_MultilangCharacterName		= GetPrivateProfileInt("GameServerInfo", "MultilangCharacterName", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_MultilangGuildName			= GetPrivateProfileInt("GameServerInfo", "MultilangGuildName", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_MonsterLuckItemRollRate		= GetPrivateProfileInt("GameServerInfo", "MonsterLuckItemRollRate", 30, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_MonsterSkillItemRollRate		= GetPrivateProfileInt("GameServerInfo", "MonsterSkillItemRollRate", 50, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_MonsterOptionItemRollRate		= GetPrivateProfileInt("GameServerInfo", "MonsterOptionItemRollRate", 30, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_MonsterExcellentItemRollRate	= GetPrivateProfileInt("GameServerInfo", "MonsterExcellentItemRollRate", 30, gDirPath.GetNewPath(FILE_SERVER_COMMON));
}

void ReadCommonServerInfo()
{
	lMsg.Load();
	ReadServerInfo();
	ReadOptionsInfo();

	gGateC.Load(gDirPath.GetNewPath(FILE_WORLD_MOVE_GATE));

#if (ENABLE_WZ_CHECKSUM == 1)
	CheckSumFileLoad(gDirPath.GetNewPath(FILE_PROTECT_CHECKSUM));
#endif

	g_MonsterStatCalc.LoadScript(gDirPath.GetNewPath(FILE_MONSTER_STAT));

	SetMapName();
	OpenItemScript(gDirPath.GetNewPath(FILE_ITEM_COMMON));

#if (CUSTOM_EXOPT==1)
	gNewOptions.LoadData(gDirPath.GetNewPath("Custom\\ExOptions.txt"));
#endif

	MagicDamageC.LogSkillList(gDirPath.GetNewPath(FILE_SKILL_COMMON));

	SkillSpearHitBox.Load(gDirPath.GetNewPath(FILE_SKILL_HITBOX_SPEAR));
	SkillElectricSparkHitBox.Load(gDirPath.GetNewPath(FILE_SKILL_HITBOX_ELECT));

	gSetItemOption.LoadOptionInfo(gDirPath.GetNewPath(FILE_ITEM_SETOPTION));
	gSetItemOption.LoadTypeInfo(gDirPath.GetNewPath(FILE_ITEM_SETTYPE));

	g_kJewelOfHarmonySystem.LoadScript(gDirPath.GetNewPath(FILE_ITEM_HARMONY_OPTION));
	g_kJewelOfHarmonySystem.LoadScriptOfSmelt(gDirPath.GetNewPath(FILE_ITEM_HARMONY_SMELT));
	g_kItemSystemFor380.Load380ItemOptionInfo(gDirPath.GetNewPath(FILE_ITEM_ADDOPTION));
	g_ItemAddOption.Load(gDirPath.GetNewPath(FILE_ITEM_ADDOPTION));

	g_SocketItem.LoadData(gDirPath.GetNewPath(FILE_ITEM_SOCKET));
	g_ElementalSystem.LoadElementalInfo(gDirPath.GetNewPath(FILE_ITEM_ELEMENT));
	g_ElementalSystem.LoadPentagramOption(gDirPath.GetNewPath(FILE_ITEM_PENTAGRAM));

	gMoveCommand.Load(gDirPath.GetNewPath(FILE_WORLD_MOVE_REQ));
	gMoveCommand.LoadMoveLevel(gDirPath.GetNewPath(FILE_WORLD_MOVE_LEVEL));

	ConMember.Load(gDirPath.GetNewPath(FILE_WHITELIST));

	g_QuestInfo.LoadQuestInfo(gDirPath.GetNewPath(FILE_QUEST_MAIN));

#if( __QUEST_SYSTEM__== 1 )
	gQuestInfoEx.LoadQuestProgress(gDirPath.GetNewPath(FILE_QUEST_PROGRESS));
	gQuestInfoEx.LoadQuestInfo(gDirPath.GetNewPath(FILE_QUEST_INFO));
#endif

	TMonsterSkillElement::LoadData(gDirPath.GetNewPath(FILE_MONSTER_SKILL_ELEMENT));
	TMonsterSkillUnit::LoadData(gDirPath.GetNewPath(FILE_MONSTER_SKILL_UNIT));
	TMonsterSkillManager::LoadData(gDirPath.GetNewPath(FILE_MONSTER_SKILL));
	TMonsterAIElement::LoadData(gDirPath.GetNewPath(FILE_MONSTER_AI_ELEMENT));
	TMonsterAIAutomata::LoadData(gDirPath.GetNewPath(FILE_MONSTER_AUTOMATA));
	TMonsterAIUnit::LoadData(gDirPath.GetNewPath(FILE_MONSTER_AI_UNIT));
	TMonsterAIGroup::LoadData(gDirPath.GetNewPath(FILE_MONSTER_AI_GROUP));

	g_Crywolf.LoadData(gDirPath.GetNewPath(FILE_EVENT_CRYWOLF));

	g_SkillAdditionInfo.Load(FILE_SKILL_ADDITION);

	g_Kanturu.LoadData(gDirPath.GetNewPath(FILE_EVENT_KANTURU_MAIN));
	g_KanturuMonsterMng.LoadData(gDirPath.GetNewPath(FILE_EVENT_KANTURU_BASE));

	g_GoblinPointSystem.LoadData();
	g_Raklion.LoadData(gDirPath.GetNewPath(FILE_EVENT_RAKLION));
	
	g_DevilSquare.SetClose();
	g_IllusionTemple.LoadData();

	g_MasterSkillSystem.Load(gDirPath.GetNewPath(FILE_SKILL_MASTERTREE));
	g_MasterExp.LoadInfo();

#if (ENABLETEST_MUUN == 1)
    g_CMuunSystem.LoadScriptMuunSystemInfo(gDirPath.GetNewPath("MuunInfo.txt"));
	g_CMuunSystem.LoadScriptMuunSystemOption(gDirPath.GetNewPath("MuunOption.txt"));
#endif

	LogAdd("User Object Size = %d", sizeof(OBJECTSTRUCT));

	char szDiplayGSInfo[512] = {0};
	wsprintfA(szDiplayGSInfo, "%s %s :: [%s] :: [%c.%c%c.%c%c] :: [%s]", GAMESERVER_NAME, GAMESERVER_VERSION, szServerName, szClientVersion[0], szClientVersion[1], szClientVersion[2], szClientVersion[3], szClientVersion[4], szGameServerExeSerial);
	SetWindowText(ghWnd, szDiplayGSInfo);
	
	g_EventItemBagManager.Load();

	ReadEventInfo(MU_EVENT_ALL);

	// Eledorado Event Start
	gEledoradoEvent.SetEventState(gIsEledoradoEvent);
	gEledoradoEvent.Init();

	gPacketCheckSum.Init();

	g_CashShop.CashShopOptioNReload();

	g_CashItemPeriodSystem.Initialize();
	g_Lottery.LoadLottery("ChaosCard",gDirPath.GetNewPath(FILE_EVENTBAG_CHAOSCARD_NORMAL));
	g_Lottery.LoadLottery("ChaosCardMini",gDirPath.GetNewPath(FILE_EVENTBAG_CHAOSCARD_MINI));
	g_Lottery.LoadLottery("ChaosCardGold",gDirPath.GetNewPath(FILE_EVENTBAG_CHAOSCARD_GOLD));
	g_Lottery.LoadLottery("ChaosCardRare",gDirPath.GetNewPath(FILE_EVENTBAG_CHAOSCARD_RARE));
	g_BuffScript.Load(gDirPath.GetNewPath(FILE_SKILL_BUFFEFFECT));

	gGamblingEvent.Init(FILE_EVENTBAG_GAMBLER);
	
	g_ImperialGuardian.LoadScript(gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_MiningSystem.LoadData(gDirPath.GetNewPath(FILE_EVENT_MINING));
	g_Minimap.Load();

#if (ENABLETEST_NEWPVP == 1)
	g_NewPVP.LoadData();
#endif
	g_CustomManager.Load();
//	g_MiniMap.LoadMinimapInfo(gDirPath.GetNewPath(FILE_WORLD_MINIMAP));
	gUdpSoc.SendSet(connectserverip, connectserverport);

	if ( gUdpSoc.CreateSocket() == 0)
	{
		MsgBox("UDP Socket create error");
	}

#if (__CUSTOM__ == 1)
	ReadCustomServerInfo();
#endif
	g_MapAttrInfo.Load();
	gParty.ReadBonusInfo(gDirPath.GetNewPath(FILE_PARTYBONUS));
}



void GameServerInfoSendStop()
{
	GSInfoSendFlag = 0;
}

void GameServerInfoSendStart()
{
	GSInfoSendFlag = 1;
}





struct PMSG_SERVERINFO
{
	PBMSG_HEAD h;	// C1:01
	short ServerCode;	// 4
	BYTE Percent;	// 6
	short UserCount;	// 8
	short AccountCount;	// A
	short PCbangCount;	// C
	short MaxUserCount;	// E
};


void GameServerInfoSend()
{
	PMSG_SERVERINFO pMsg;

	if ( gDisconnect == TRUE )
	{
		return;
	}

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x01;
	pMsg.h.size = sizeof( pMsg );

	if ( gObjTotalUser == 0 )
	{
		pMsg.Percent = 0;
	}
	else
	{
		pMsg.Percent = (gObjTotalUser * 100)/gServerMaxUser;
	}

	pMsg.ServerCode = gGameServerCode;
	pMsg.UserCount = gObjTotalUser;
	pMsg.AccountCount = gLCount[0].Get();
	pMsg.PCbangCount = gLCount[1].Get();
	pMsg.MaxUserCount = gServerMaxUser;

	gUdpSoc.SendData((LPBYTE)&pMsg, pMsg.h.size);
}

void CheckSumFileLoad(char * szCheckSum)
{
	HANDLE hFile=CreateFile(szCheckSum,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE,NULL);

	if ( hFile == NULL )
	{
		MsgBox("Can't Open CheckSum.dat [%d]", GetLastError());
		return;
	}

	int iFileSize = GetFileSize(hFile, NULL);
	DWORD Bytes;

	ReadFile(hFile, dwgCheckSum, iFileSize, &Bytes, NULL);
	CloseHandle(hFile);
}

void SetMapName()
{
	strcpy_s(gMapName[MAP_INDEX_RORENCIA], sizeof(gMapName[0]),	lMsg.Get(MSGGET(7,208)));
	strcpy_s(gMapName[MAP_INDEX_NORIA], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,209)));
	strcpy_s(gMapName[MAP_INDEX_DEVIAS], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,210)));
	strcpy_s(gMapName[MAP_INDEX_NORIA], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,211)));
	strcpy_s(gMapName[MAP_INDEX_LOSTTOWER], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,212)));
	strcpy_s(gMapName[MAP_INDEX_RESERVED], sizeof(gMapName[0]), "Reserved");
	strcpy_s(gMapName[MAP_INDEX_BATTLESOCCER], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,214)));
	strcpy_s(gMapName[MAP_INDEX_ATHLANSE], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,215)));
	strcpy_s(gMapName[MAP_INDEX_TARKAN], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,216)));
	strcpy_s(gMapName[MAP_INDEX_DEVILSQUARE], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,217)));
	strcpy_s(gMapName[MAP_INDEX_ICARUS], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,218)));

	// BloodCastle Maps
	strcpy_s(gMapName[MAP_INDEX_BLOODCASTLE1], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,219)));
	strcat_s(gMapName[MAP_INDEX_BLOODCASTLE1], "1");
	strcpy_s(gMapName[MAP_INDEX_BLOODCASTLE2], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,219)));
	strcat_s(gMapName[MAP_INDEX_BLOODCASTLE2], "2");
	strcpy_s(gMapName[MAP_INDEX_BLOODCASTLE3], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,219)));
	strcat_s(gMapName[MAP_INDEX_BLOODCASTLE3], "3");
	strcpy_s(gMapName[MAP_INDEX_BLOODCASTLE4], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,219)));
	strcat_s(gMapName[MAP_INDEX_BLOODCASTLE4], "4");
	strcpy_s(gMapName[MAP_INDEX_BLOODCASTLE5], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,219)));
	strcat_s(gMapName[MAP_INDEX_BLOODCASTLE5], "5");
	strcpy_s(gMapName[MAP_INDEX_BLOODCASTLE6], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,219)));
	strcat_s(gMapName[MAP_INDEX_BLOODCASTLE6], "6");
	strcpy_s(gMapName[MAP_INDEX_BLOODCASTLE7], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,219)));
	strcat_s(gMapName[MAP_INDEX_BLOODCASTLE7], "7");
	strcpy_s(gMapName[MAP_INDEX_BLOODCASTLE8], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,219)));
	strcat_s(gMapName[MAP_INDEX_BLOODCASTLE8], "8");
	// ChaosCastle Maps
	strcpy_s(gMapName[MAP_INDEX_CHAOSCASTLE1], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,220)));
	strcat_s(gMapName[MAP_INDEX_CHAOSCASTLE1], "1");
	strcpy_s(gMapName[MAP_INDEX_CHAOSCASTLE2], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,220)));
	strcat_s(gMapName[MAP_INDEX_CHAOSCASTLE2], "2");
	strcpy_s(gMapName[MAP_INDEX_CHAOSCASTLE3], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,220)));
	strcat_s(gMapName[MAP_INDEX_CHAOSCASTLE3], "3");
	strcpy_s(gMapName[MAP_INDEX_CHAOSCASTLE4], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,220)));
	strcat_s(gMapName[MAP_INDEX_CHAOSCASTLE4], "4");
	strcpy_s(gMapName[MAP_INDEX_CHAOSCASTLE5], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,220)));
	strcat_s(gMapName[MAP_INDEX_CHAOSCASTLE5], "5");
	strcpy_s(gMapName[MAP_INDEX_CHAOSCASTLE6], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,220)));
	strcat_s(gMapName[MAP_INDEX_CHAOSCASTLE6], "6");
	strcpy_s(gMapName[MAP_INDEX_CHAOSCASTLE7], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,220)));
	strcat_s(gMapName[MAP_INDEX_CHAOSCASTLE7], "7");
	// Kalima Maps
	strcpy_s(gMapName[MAP_INDEX_KALIMA1], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,221)));
	strcat_s(gMapName[MAP_INDEX_KALIMA1], "1");
	strcpy_s(gMapName[MAP_INDEX_KALIMA2], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,221)));
	strcat_s(gMapName[MAP_INDEX_KALIMA2], "2");
	strcpy_s(gMapName[MAP_INDEX_KALIMA3], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,221)));
	strcat_s(gMapName[MAP_INDEX_KALIMA3], "3");
	strcpy_s(gMapName[MAP_INDEX_KALIMA4], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,221)));
	strcat_s(gMapName[MAP_INDEX_KALIMA4], "4");
	strcpy_s(gMapName[MAP_INDEX_KALIMA5], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,221)));
	strcat_s(gMapName[MAP_INDEX_KALIMA5], "5");
	strcpy_s(gMapName[MAP_INDEX_KALIMA6], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,221)));
	strcat_s(gMapName[MAP_INDEX_KALIMA6], "6");
	strcpy_s(gMapName[MAP_INDEX_KALIMA7], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,221)));
	strcat_s(gMapName[MAP_INDEX_KALIMA7], "7");

	strcpy_s(gMapName[MAP_INDEX_CASTLESIEGE], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,222)));
	strcpy_s(gMapName[MAP_INDEX_CASTLEHUNTZONE], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,223)));
	strcpy_s(gMapName[MAP_INDEX_DEVILSQUARE2], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,217)));
	strcpy_s(gMapName[MAP_INDEX_AIDA], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,224)));
	strcpy_s(gMapName[MAP_INDEX_CRYWOLF_FIRSTZONE], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,225)));
	strcpy_s(gMapName[MAP_INDEX_CRYWOLF_SECONDZONE], sizeof(gMapName[0]), "Reserved");
	strcpy_s(gMapName[MAP_INDEX_KANTURU1], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,226)));
	strcpy_s(gMapName[MAP_INDEX_KANTURU2], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,227)));
	strcpy_s(gMapName[MAP_INDEX_KANTURU_BOSS], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,228)));
	strcpy_s(gMapName[MAP_INDEX_GMMAP], sizeof(gMapName[0]), "Reserved");
	strcpy_s(gMapName[MAP_INDEX_BARRACKS], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,229)));
	strcpy_s(gMapName[MAP_INDEX_REFUGE], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,230)));

	// Illusion Temple Maps
	strcpy_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE1], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,231)));
	strcat_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE1], "1");
	strcpy_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE2], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,231)));
	strcat_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE2], "2");
	strcpy_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE3], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,231)));
	strcat_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE3], "3");
	strcpy_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE4], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,231)));
	strcat_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE4], "4");
	strcpy_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE5], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,231)));
	strcat_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE5], "5");
	strcpy_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE6], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,231)));
	strcat_s(gMapName[MAP_INDEX_ILLUSIONTEMPLE6], "6");

	strcpy_s(gMapName[MAP_INDEX_ELBELAND], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,232)));
	strcpy_s(gMapName[MAP_INDEX_SWAMPOFCALMNESS], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,233)));
	strcpy_s(gMapName[MAP_INDEX_RAKLION], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,234)));
	strcpy_s(gMapName[MAP_INDEX_RAKLIONBOSS], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,235)));
	strcpy_s(gMapName[MAP_INDEX_SANTAVILLAGE], sizeof(gMapName[0]), lMsg.Get(MSGGET(7,236)));
}


struct ST_EVENT_FLAG
{
	BOOL bDevilSquare : 1;
	BOOL bBloodCastle : 1;
	BOOL			  : 6;
	BOOL bXMaxEvent   : 1;
	BOOL bFireCracker : 1;
	BOOL bHeartOfLove : 1;
	BOOL bMedalEvent  : 1;
	BOOL bRingEvent   : 1;
	BOOL bEventChip   : 1;
	BOOL bEledorado   : 1;
	BOOL			  : 1;
	BOOL bNPGGChecksum: 1;
};

union STU_EVENT_FLAG
{
	ST_EVENT_FLAG EventData;
	DWORD AllData;
};


// Check what functions are enabled to send 
// Authentication Server (KOREA NON_REGISTERED SERVERS)
int GetEventFlag()
{
	STU_EVENT_FLAG flag;

	flag.AllData = 0;

	if (gDevilSquareEvent!=0)
	{
		flag.EventData.bDevilSquare=1;
	}
	if (g_bBloodCastle!=0)
	{
		flag.EventData.bBloodCastle=1;
	}
	if (gFireCrackerEvent!=0)
	{
		flag.EventData.bFireCracker=1;
	}
	if (gHeartOfLoveEvent!=0)
	{
		flag.EventData.bHeartOfLove=1;
	}
	if (gMedalEvent!=0)
	{
		flag.EventData.bMedalEvent=1;
	}
	if (g_bDoRingEvent!=0)
	{
		flag.EventData.bRingEvent=1;
	}
	if (gEventChipEvent!=0)
	{
		flag.EventData.bEventChip=1;
	}
	if (gIsEledoradoEvent!=0)
	{
		flag.EventData.bEledorado=1;
	}

	return flag.AllData;	
}


void ReadEventInfo(MU_EVENT_TYPE eEventType)
{
	char szTemp[256];
	switch(eEventType)
	{
		case 0: //All Events
			g_DevilSquare.Load(gDirPath.GetNewPath(FILE_EVENT_DEVILSQUARE));
			gDevilSquareEvent = GetPrivateProfileInt("GameServerInfo","DevilSquareEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEyesOfDevilSquareDropRate = GetPrivateProfileInt("GameServerInfo","EyesOfDevilSquareDropRate", 2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gKeyOfDevilSquareDropRate = GetPrivateProfileInt("GameServerInfo","KeyOfDevilSquareDropRate", 2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_BloodCastle.Load(gDirPath.GetNewPath(FILE_EVENT_BLOODCASTLE));
			g_bBloodCastle = GetPrivateProfileInt("GameServerInfo","BloodCastleEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iAngelKingsPaperDropRate = GetPrivateProfileInt("GameServerInfo","AngelKingsPaperDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iBloodBoneDropRate = GetPrivateProfileInt("GameServerInfo","BloodBoneDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iStoneDropRate = GetPrivateProfileInt("GameServerInfo","StoneDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gIsDropDarkLordItem = GetPrivateProfileInt("GameServerInfo","IsDropDarkLordItem", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSleeveOfLordDropRate = GetPrivateProfileInt("GameServerInfo","SleeveOfLordDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSleeveOfLordDropLevel = GetPrivateProfileInt("GameServerInfo","SleeveOfLordDropLevel", 86, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSoulOfDarkHorseDropRate = GetPrivateProfileInt("GameServerInfo","SoulOfDarkHorseDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSoulOfDarkHorseropLevel = GetPrivateProfileInt("GameServerInfo","SoulOfDarkHorseropLevel", 125, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iAngelKingsPaperDropRate = GetPrivateProfileInt("GameServerInfo","AngelKingsPaperDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSoulOfDarkSpiritDropRate = GetPrivateProfileInt("GameServerInfo","SoulOfDarkSpiritDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSoulOfDarkSpiritDropLevel = GetPrivateProfileInt("GameServerInfo","SoulOfDarkSpiritDropLevel", 96, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			GetPrivateProfileString("GameServerInfo", "DarkSpiritAddExperience", "1", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gDarkSpiritAddExperience = (float)atof(szTemp);

			gIsDropGemOfDefend = GetPrivateProfileInt("GameServerInfo","IsDropGemOfDefend", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGemOfDefendDropRate = GetPrivateProfileInt("GameServerInfo","GemOfDefendDropRate", 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGemOfDefendDropLevel = GetPrivateProfileInt("GameServerInfo","GemOfDefendDropLevel", 75, gDirPath.GetNewPath(FILE_SERVER_COMMON));

#if (GS_CASTLE==1)

			gIsDropSetItemInCastleHuntZone = GetPrivateProfileInt("GameServerInfo","IsDropSetItemInCastleHuntZone", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSetItemInCastleHuntZoneDropRate = GetPrivateProfileInt("GameServerInfo","SetItemInCastleHuntZoneDropRate", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSetItemInCastleHuntZoneDropLevel = GetPrivateProfileInt("GameServerInfo","SetItemInCastleHuntZoneDropLevel", 75, gDirPath.GetNewPath(FILE_SERVER_COMMON));
#endif
			gIsEledoradoEvent = GetPrivateProfileInt("GameServerInfo","IsEledoradoEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldGoblenRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldGoblenRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoTitanRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoTitanRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldDerconRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldDerconRegenTime", 720, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilLizardKingRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoDevilLizardKingRegenTime", 360, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilTantarosRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoDevilTantarosRegenTime", 360, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldGoblenItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoGoldGoblenItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoTitanItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoTitanItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldDerconItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoGoldDerconItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilLizardKingItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoDevilLizardKingItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilTantarosItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoDevilTantarosItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldGoblenExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoGoldGoblenExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoTitanExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoTitanExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldDerconExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoGoldDerconExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilLizardKingExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoDevilLizardKingExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilTantarosExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoDevilTantarosExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			gEledoradoGoldenRabbitRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenRabbitRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldenDarkKnightRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenDarkKnightRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldenDevilRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenDevilRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldenMonsterRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenMonsterRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldenCrustRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenCrustRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldenSatirosRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenSatirosRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldenTwintailRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenTwintailRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldenIronKnightRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenIronKnightRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldenNeipinRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenNeipinRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldenGreatDragonRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldenGreatDragonRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			GetPrivateProfileString("GameServerInfo", "RingAttackEvent", "0", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_bDoRingEvent = atoi(szTemp);
			GetPrivateProfileString("GameServerInfo", "RingOrcKillGiftRate", "10000", szTemp, 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iRingOrcKillGiftRate = atoi(szTemp);
			g_RingAttackEvent.Load(gDirPath.GetNewPath(FILE_EVENT_RINGATTACK));
			g_RingAttackEvent.EnableEvent(g_bDoRingEvent);

			g_bDoXMasEvent = GetPrivateProfileInt("GameServerInfo","XMasAttackEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iXMasItemDropRate = GetPrivateProfileInt("GameServerInfo","XMasAttackEvent_DropRate", 50, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iXMasZenDrop = GetPrivateProfileInt("GameServerInfo","XMasAttackEvent_DropZen", 10000, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			g_XMasAttackEvent.Load(gDirPath.GetNewPath(FILE_EVENT_XMASATTACK));
			g_XMasAttackEvent.SetEnable(g_bDoXMasEvent);


#if (GS_CASTLE==1)
			GetPrivateProfileString("GameServerInfo", "CastleDeepEvent", "0", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_bDoCastleDeepEvent = atoi(szTemp);
			g_CastleDeepEvent.Load(gDirPath.GetNewPath(FILE_EVENT_CASTLEDEEP));
			g_CastleDeepEvent.EnableEvent(g_bDoCastleDeepEvent);
#endif

			gFireCrackerEvent = GetPrivateProfileInt("GameServerInfo","FireCrackerEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gFireCrackerDropRate = GetPrivateProfileInt("GameServerInfo","FireCrackerDropRate", 5000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gMedalEvent = GetPrivateProfileInt("GameServerInfo","MedalEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGoldMedalDropRate = GetPrivateProfileInt("GameServerInfo","GoldMedalDropRate", 2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSilverMedalDropRate = GetPrivateProfileInt("GameServerInfo","SilverMedalDropRate", 2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gHeartOfLoveEvent = GetPrivateProfileInt("GameServerInfo","HeartOfLoveEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gHeartOfLoveDropRate = GetPrivateProfileInt("GameServerInfo","HeartOfLoveDropRate", 5000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			GetPrivateProfileString("GameServerInfo", "HappyNewYearTalkNpc", "0", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gHappyNewYearNpcEvent = atoi(szTemp);

			if(gHappyNewYearNpcEvent != 0)
			{
				// (Option) Happy-new-year NPC speaks
				LogAddTD(lMsg.Get(MSGGET(2, 61)));
			}

			GetPrivateProfileString("GameServerInfo", "MerryXMasTalkNpc", "0", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gMerryXMasNpcEvent = atoi(szTemp);

			if(gMerryXMasNpcEvent != 0)
			{
				// (Option) Christmas NPC speaks
				LogAddTD(lMsg.Get(MSGGET(2, 60)));
			}

			g_ChaosCastle.Load(gDirPath.GetNewPath(FILE_EVENT_CHAOSCASTLE));
			g_bChaosCastle = GetPrivateProfileInt("GameServerInfo","ChaosCastleEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			g_iKundunMarkDropRate = GetPrivateProfileInt("GameServerInfo","KundunMarkDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			g_iMysteriousBeadDropRate1 = GetPrivateProfileInt("GameServerInfo","MysteriouseBeadDropRate1", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iMysteriousBeadDropRate2 = GetPrivateProfileInt("GameServerInfo","MysteriouseBeadDropRate2", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
     
			GetPrivateProfileString("GameServerInfo", "EventManagerOn", "0", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_bEventManagerOn = atoi(szTemp);
			g_EventManager.Load(gDirPath.GetNewPath(FILE_EVENT_MANAGEMENT));
			g_EventManager.RegEvent(CEventManagement::EVENT_ID_DRAGONHERD, DragonEvent);
			g_EventManager.RegEvent(CEventManagement::EVENT_ID_UNDERTROOP, AttackEvent);
			g_EventManager.RegEvent(CEventManagement::EVENT_ID_ELDORADO, &gEledoradoEvent);
			g_EventManager.RegEvent(CEventManagement::EVENT_ID_WHITEMAGE, &g_RingAttackEvent);
			g_EventManager.Init(g_bEventManagerOn);

			g_iDarkLordHeartDropRate = GetPrivateProfileInt("GameServerInfo","DarkLordHeartDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			// Ribbon Box Event
			g_bRibbonBoxEvent = GetPrivateProfileInt("GameServerInfo","RibbonBoxEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Red Ribbon Box
				g_iRedRibbonBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","RedRibbonBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iRedRibbonBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","RedRibbonBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iRedRibbonBoxDropRate = GetPrivateProfileInt("GameServerInfo","RedRibbonBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Green Ribbon Box
				g_iGreenRibbonBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","GreenRibbonBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iGreenRibbonBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","GreenRibbonBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iGreenRibbonBoxDropRate = GetPrivateProfileInt("GameServerInfo","GreenRibbonBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				
				// Blue Ribbon Box
				g_iBlueRibbonBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","BlueRibbonBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iBlueRibbonBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","BlueRibbonBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iBlueRibbonBoxDropRate = GetPrivateProfileInt("GameServerInfo","BlueRibbonBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			// Chocolate Event
			g_bChocolateBoxEvent = GetPrivateProfileInt("GameServerInfo","ChocolateEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Pink Chocolate Box
				g_iPinkChocolateBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","PinkChocolateBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iPinkChocolateBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","PinkChocolateBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iPinkChocolateBoxDropRate = GetPrivateProfileInt("GameServerInfo","PinkChocolateBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Red Chocolate Box
				g_iRedChocolateBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","RedChocolateBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iRedChocolateBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","RedChocolateBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iRedChocolateBoxDropRate = GetPrivateProfileInt("GameServerInfo","RedChocolateBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Blue Chocolate Box
				g_iBlueChocolateBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","BlueChocolateBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iBlueChocolateBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","BlueChocolateBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iBlueChocolateBoxDropRate = GetPrivateProfileInt("GameServerInfo","BlueChocolateBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			// Candy Event
			g_bCandyBoxEvent = GetPrivateProfileInt("GameServerInfo","CandyBoxEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// LightPurple Candy Box
				g_iLightPurpleCandyBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","LightPurpleCandyBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iLightPurpleCandyBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","LightPurpleCandyBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iLightPurpleCandyBoxDropRate = GetPrivateProfileInt("GameServerInfo","LightPurpleCandyBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Vermilion Candy Box
				g_iVermilionCandyBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","VermilionCandyBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iVermilionCandyBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","VermilionCandyBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iVermilionCandyBoxDropRate = GetPrivateProfileInt("GameServerInfo","VermilionCandyBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// DeepBlue Candy Box
				g_iDeepBlueCandyBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","DeepBlueCandyBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iDeepBlueCandyBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","DeepBlueCandyBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iDeepBlueCandyBoxDropRate = GetPrivateProfileInt("GameServerInfo","DeepBlueCandyBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				gDoubleGoer.LoadData(gDirPath.GetNewPath(FILE_EVENT_DOPPELGANGER));

#if ( __GENS_SYSTEM__ == 1 )
				gGensSystem.LoadData(gDirPath.GetNewPath(FILE_GENS));
#endif
			break;

		case 1: //Devil Square
			g_DevilSquare.Load(gDirPath.GetNewPath(FILE_EVENT_DEVILSQUARE));
			gDevilSquareEvent = GetPrivateProfileInt("GameServerInfo","DevilSquareEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEyesOfDevilSquareDropRate = GetPrivateProfileInt("GameServerInfo","EyesOfDevilSquareDropRate", 2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gKeyOfDevilSquareDropRate = GetPrivateProfileInt("GameServerInfo","KeyOfDevilSquareDropRate", 2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			break;

		case 2: //Blood Castle
			g_BloodCastle.Load(gDirPath.GetNewPath(FILE_EVENT_BLOODCASTLE));
			g_bBloodCastle = GetPrivateProfileInt("GameServerInfo","BloodCastleEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iAngelKingsPaperDropRate = GetPrivateProfileInt("GameServerInfo","AngelKingsPaperDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iBloodBoneDropRate = GetPrivateProfileInt("GameServerInfo","BloodBoneDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_iStoneDropRate = GetPrivateProfileInt("GameServerInfo","StoneDropRate", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			break;

		case 4: //Eledorado Event
			gIsEledoradoEvent = GetPrivateProfileInt("GameServerInfo","IsEledoradoEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldGoblenRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldGoblenRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoTitanRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoTitanRegenTime", 180, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldDerconRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoGoldDerconRegenTime", 720, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilLizardKingRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoDevilLizardKingRegenTime", 360, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilTantarosRegenTime = GetPrivateProfileInt("GameServerInfo","EledoradoDevilTantarosRegenTime", 360, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldGoblenItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoGoldGoblenItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoTitanItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoTitanItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldDerconItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoGoldDerconItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilLizardKingItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoDevilLizardKingItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilTantarosItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoDevilTantarosItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldGoblenExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoGoldGoblenExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoTitanExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoTitanExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoGoldDerconExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoGoldDerconExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilLizardKingExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoDevilLizardKingExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gEledoradoDevilTantarosExItemDropRate = GetPrivateProfileInt("GameServerInfo","EledoradoDevilTantarosExItemDropRate", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			break;

		case 5: //Ring Event
			{
				char szTemp[256];

				GetPrivateProfileString("GameServerInfo", "RingAttackEvent", "0", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_bDoRingEvent = atoi(szTemp);
				GetPrivateProfileString("GameServerInfo", "RingOrcKillGiftRate", "10000", szTemp, 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iRingOrcKillGiftRate = atoi(szTemp);
				g_RingAttackEvent.Load(gDirPath.GetNewPath(FILE_EVENT_RINGATTACK));
				g_RingAttackEvent.EnableEvent(g_bDoRingEvent);
			}
			break;

		case 6: //Event 1
			break;

		case 7: //Fire Cracker
			gFireCrackerEvent = GetPrivateProfileInt("GameServerInfo","FireCrackerEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gFireCrackerDropRate = GetPrivateProfileInt("GameServerInfo","FireCrackerDropRate", 5000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			break;

		case 8: //Medal Event
			gMedalEvent = GetPrivateProfileInt("GameServerInfo","MedalEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGoldMedalDropRate = GetPrivateProfileInt("GameServerInfo","GoldMedalDropRate", 2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gSilverMedalDropRate = GetPrivateProfileInt("GameServerInfo","SilverMedalDropRate", 2, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			break;

		case 10: //Heart Of Love Event
			gHeartOfLoveEvent = GetPrivateProfileInt("GameServerInfo","HeartOfLoveEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gHeartOfLoveDropRate = GetPrivateProfileInt("GameServerInfo","HeartOfLoveDropRate", 5000, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			break;

		case 11: //Happy New Year Event
			GetPrivateProfileString("GameServerInfo", "HappyNewYearTalkNpc", "0", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gHappyNewYearNpcEvent = atoi(szTemp);

			if(gHappyNewYearNpcEvent != 0)
			{
				// (Option) Happy-new-year NPC speaks
				LogAddTD(lMsg.Get(MSGGET(2, 61)));
			}

			break;

		case 12: //Merry  XMax Event NPC talk
			GetPrivateProfileString("GameServerInfo", "MerryXMasTalkNpc", "0", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gMerryXMasNpcEvent = atoi(szTemp);

			if(gMerryXMasNpcEvent != 0)
			{
				// (Option) Christmas NPC speaks
				LogAddTD(lMsg.Get(MSGGET(2, 60)));
			}

			break;

		case 13: //Chaos Castle
			g_ChaosCastle.Load(gDirPath.GetNewPath(FILE_EVENT_CHAOSCASTLE));
			g_bChaosCastle = GetPrivateProfileInt("GameServerInfo","ChaosCastleEvent", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			break;

		case 14:
			// Ribbon Box Event
			g_bRibbonBoxEvent = GetPrivateProfileInt("GameServerInfo","RibbonBoxEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Red Ribbon Box
				g_iRedRibbonBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","RedRibbonBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iRedRibbonBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","RedRibbonBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iRedRibbonBoxDropRate = GetPrivateProfileInt("GameServerInfo","RedRibbonBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				// Green Ribbon Box
				g_iGreenRibbonBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","GreenRibbonBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iGreenRibbonBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","GreenRibbonBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iGreenRibbonBoxDropRate = GetPrivateProfileInt("GameServerInfo","GreenRibbonBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Blue Ribbon Box
				g_iBlueRibbonBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","BlueRibbonBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iBlueRibbonBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","BlueRibbonBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iBlueRibbonBoxDropRate = GetPrivateProfileInt("GameServerInfo","BlueRibbonBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				break;

		case 15:
			// Chocolate Event
			g_bChocolateBoxEvent = GetPrivateProfileInt("GameServerInfo","ChocolateEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Pink Chocolate Box
				g_iPinkChocolateBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","PinkChocolateBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iPinkChocolateBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","PinkChocolateBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iPinkChocolateBoxDropRate = GetPrivateProfileInt("GameServerInfo","PinkChocolateBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			
				// Red Chocolate Box
				g_iRedChocolateBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","RedChocolateBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iRedChocolateBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","RedChocolateBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iRedChocolateBoxDropRate = GetPrivateProfileInt("GameServerInfo","RedChocolateBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// Blue Chocolate Box
				g_iBlueChocolateBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","BlueChocolateBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iBlueChocolateBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","BlueChocolateBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iBlueChocolateBoxDropRate = GetPrivateProfileInt("GameServerInfo","BlueChocolateBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				break;

		case 16:
			// Candy Event
			g_bCandyBoxEvent = GetPrivateProfileInt("GameServerInfo","CandyBoxEvent",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// LightPurple Candy Box
				g_iLightPurpleCandyBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","LightPurpleCandyBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iLightPurpleCandyBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","LightPurpleCandyBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iLightPurpleCandyBoxDropRate = GetPrivateProfileInt("GameServerInfo","LightPurpleCandyBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
		
				// Vermilion Candy Box
				g_iVermilionCandyBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","VermilionCandyBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iVermilionCandyBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","VermilionCandyBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iVermilionCandyBoxDropRate = GetPrivateProfileInt("GameServerInfo","VermilionCandyBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

				// DeepBlue Candy Box
				g_iDeepBlueCandyBoxDropLevelMin = GetPrivateProfileInt("GameServerInfo","DeepBlueCandyBoxDropLv_Min",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iDeepBlueCandyBoxDropLevelMax = GetPrivateProfileInt("GameServerInfo","DeepBlueCandyBoxDropLv_Max",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				g_iDeepBlueCandyBoxDropRate = GetPrivateProfileInt("GameServerInfo","DeepBlueCandyBoxDropRate",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
				break;
	}
}


void ReadGameEtcInfo(MU_ETC_TYPE eGameEtcType)
{
	char szTemp[256];

	switch ( eGameEtcType )
	{
		case 0:
			GetPrivateProfileString("GameServerInfo", "CreateCharacter", "1", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gCreateCharacter = atoi(szTemp);

			GetPrivateProfileString("GameServerInfo", "GuildCreate", "1", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGuildCreate = atoi(szTemp);
			LogAdd(lMsg.Get(MSGGET(2, 57)), gGuildCreate);
			GetPrivateProfileString("GameServerInfo", "GuildDestroy", "1", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGuildDestroy = atoi(szTemp);
			LogAdd(lMsg.Get(MSGGET(2, 58)), gGuildDestroy);
			GetPrivateProfileString("GameServerInfo", "GuildCreateLevel", "100", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGuildCreateLevel = atoi(szTemp);
			LogAdd(lMsg.Get(MSGGET(2, 59)), gGuildCreateLevel);
			
			g_GuildCreateReset = GetPrivateProfileInt("GameServerInfo","GuildCreateReset",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_GuildMaxMember = GetPrivateProfileInt("GameServerInfo","GuildMaxMember",80, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_GuildMaxAllianceMember = GetPrivateProfileInt("GameServerInfo","GuildMaxAllianceMember",5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_CastleMinGuildMember = GetPrivateProfileInt("GameServerInfo","CastleMinGuildMember",20, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			g_bCastleGuildDestoyLimit = GetPrivateProfileInt("GameServerInfo", "CastleOwnerGuildDestroyLimit", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			GetPrivateProfileString("GameServerInfo", "Trade", "1", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			bCanTrade = atoi(szTemp);

			bCanChaosBox = GetPrivateProfileInt("GameServerInfo", "ChaosBox", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			GetPrivateProfileString("GameServerInfo", "ItemDropPer", "10", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gItemDropPer = atoi(szTemp);

			gDoPShopOpen = GetPrivateProfileInt("GameServerInfo", "PersonalShopOpen", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			gAttackSpeedTimeLimit = GetPrivateProfileInt("GameServerInfo", "AttackSpeedTimeLimit", 800, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			bIsIgnorePacketSpeedHackDetect = GetPrivateProfileInt("GameServerInfo", "IsIgnorePacketHackDetect", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gHackCheckCount = GetPrivateProfileInt("GameServerInfo", "HackCheckCount", 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gMinimumAttackSpeedTime = GetPrivateProfileInt("GameServerInfo", "MinimumAttackSpeedTime", 200 , gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gDetectedHackKickCount = GetPrivateProfileInt("GameServerInfo", "DetectedHackKickCount", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gIsKickDetecHackCountLimit = GetPrivateProfileInt("GameServerInfo", "IsKickDetecHackCountLimit", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			break;

		case 1:
			GetPrivateProfileString("GameServerInfo", "CreateCharacter", "1", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gCreateCharacter = atoi(szTemp);

			break;

		case 2:
			GetPrivateProfileString("GameServerInfo", "GuildCreate", "1", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGuildCreate = atoi(szTemp);
			LogAdd(lMsg.Get(MSGGET(2, 57)), gGuildCreate);
			GetPrivateProfileString("GameServerInfo", "GuildDestroy", "1", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGuildDestroy = atoi(szTemp);
			LogAdd(lMsg.Get(MSGGET(2, 58)), gGuildDestroy);
			GetPrivateProfileString("GameServerInfo", "GuildCreateLevel", "100", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gGuildCreateLevel = atoi(szTemp);
			LogAdd(lMsg.Get(MSGGET(2, 59)), gGuildCreateLevel);
			g_GuildCreateReset = GetPrivateProfileInt("GameServerInfo","GuildCreateReset",0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_GuildMaxMember = GetPrivateProfileInt("GameServerInfo","GuildMaxMember",80, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_GuildMaxAllianceMember = GetPrivateProfileInt("GameServerInfo","GuildMaxAllianceMember",5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_CastleMinGuildMember = GetPrivateProfileInt("GameServerInfo","CastleMinGuildMember",20, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			g_bCastleGuildDestoyLimit = GetPrivateProfileInt("GameServerInfo", "CastleOwnerGuildDestroyLimit", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			break;

		case 3:
			GetPrivateProfileString("GameServerInfo", "Trade", "1", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			bCanTrade = atoi(szTemp);

			break;
		case 4:
			bCanChaosBox = GetPrivateProfileInt("GameServerInfo", "ChaosBox", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			break;

		case 5:
			gDoPShopOpen = GetPrivateProfileInt("GameServerInfo", "PersonalShopOpen", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			break;


		case 7:
			GetPrivateProfileString("GameServerInfo", "ItemDropPer", "10", szTemp, 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gItemDropPer = atoi(szTemp);

			break;

		case 8:
			gAttackSpeedTimeLimit = GetPrivateProfileInt("GameServerInfo", "AttackSpeedTimeLimit", 800, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			bIsIgnorePacketSpeedHackDetect = GetPrivateProfileInt("GameServerInfo", "IsIgnorePacketHackDetect", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gHackCheckCount = GetPrivateProfileInt("GameServerInfo", "HackCheckCount", 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gMinimumAttackSpeedTime = GetPrivateProfileInt("GameServerInfo", "MinimumAttackSpeedTime", 200 , gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gDetectedHackKickCount = GetPrivateProfileInt("GameServerInfo", "DetectedHackKickCount", 10, gDirPath.GetNewPath(FILE_SERVER_COMMON));
			gIsKickDetecHackCountLimit = GetPrivateProfileInt("GameServerInfo", "IsKickDetecHackCountLimit", 0, gDirPath.GetNewPath(FILE_SERVER_COMMON));

			break;

		case 9:

			break;

	}


}

#if (__CUSTOM__ == 1)

int gc_bBlockFullExcellent;
int gc_bBlockFullAncient;

int gc_bPersonalID;

int gc_ChaosMixPlusItemLevel_1_1;
int gc_ChaosMixPlusItemLevel_2_1;
int gc_ChaosMixPlusItemLevel_3_1;
int gc_ChaosMixPlusItemLevel_1_2;
int gc_ChaosMixPlusItemLevel_2_2;
int gc_ChaosMixPlusItemLevel_3_2;
int gc_ChaosMixPlusItemLevel_1_3;
int gc_ChaosMixPlusItemLevel_2_3;
int gc_ChaosMixPlusItemLevel_3_3;
int gc_ChaosMixPlusItemLevel_Luck;
int gc_ChaosMixPlusItemLevel_Max;

int gc_ChaosMixCondor_Div;
int gc_ChaosMixCondor_Max;
int gc_ChaosMix3rdWing_Div;
int gc_ChaosMix3rdWing_Max;

int g_RenaCount;
int g_StoneCount;

int g_ElfBuffDefenseDiv;
int g_ElfBuffDefenseStart;
int g_ElfBuffAttackDiv;
int g_ElfBuffAttackStart;
int g_ElfBuffTime;
int g_ExcDropPerc;

void ReadCustomServerInfo()
{
	g_ExcDropPerc = GetPrivateProfileInt("GameServerInfo", "ExcDropPerc", 2000, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// PersonalID
	gc_bPersonalID = GetPrivateProfileInt("GameServerInfo","IsPersonalID",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// GameServerInfo Settings
	gc_bBlockFullExcellent = GetPrivateProfileInt("GameServerInfo","BlockFullExcellent",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_bBlockFullAncient = GetPrivateProfileInt("GameServerInfo","BlockFullAncient",0,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	// Chaos Mixes
	gc_ChaosMixPlusItemLevel_1_1 = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_TYPE1_1",60,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_2_1 = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_TYPE2_1",50,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_3_1 = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_TYPE3_1",40,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_1_2 = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_TYPE1_2",55,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_2_2 = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_TYPE2_2",45,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_3_2 = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_TYPE3_2",35,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_1_3 = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_TYPE1_3",20,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_2_3 = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_TYPE2_3",10,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_3_3 = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_TYPE3_3",5,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_Luck = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_LUCK",20,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixPlusItemLevel_Max = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_PLUSITEMLEVEL_MAXVAL",75,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	gc_ChaosMixCondor_Div = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_CONDOR_DIV",300000,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMixCondor_Max = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_CONDOR_MAXVAL",60,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMix3rdWing_Div = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_3RDWING_DIV",3000000,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	gc_ChaosMix3rdWing_Max = GetPrivateProfileInt("GameServerInfo","CHAOSMIX_3RDWING_MAXVAL",40,gDirPath.GetNewPath(FILE_SERVER_COMMON));

	gCustomExp.LoadResetExp(gDirPath.GetNewPath("custom\\resetexp.txt"));

#if (__CUSTOM_QUEST__ == 1)
	g_MyQuest.LoadData(gDirPath.GetNewPath("Custom\\CustomQuest.txt"));
#endif

#ifndef SILVER
	LoadPremiumSettings(gDirPath.GetNewPath(FILE_CUSTOM_PREMIUM));
#endif

	g_RenaCount = GetPrivateProfileInt("GameServerInfo", "GoldenArcherRenaCount", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_StoneCount = GetPrivateProfileInt("GameServerInfo", "GoldenArcherStoneCount", 1, gDirPath.GetNewPath(FILE_SERVER_COMMON));

	g_ElfBuffDefenseDiv = GetPrivateProfileInt("GameServerInfo", "EB_DefenseLevelDiv", 5, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_ElfBuffDefenseStart = GetPrivateProfileInt("GameServerInfo", "EB_DefenseStart", 50, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_ElfBuffAttackDiv = GetPrivateProfileInt("GameServerInfo", "EB_AttackLevelDiv", 3, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_ElfBuffAttackStart = GetPrivateProfileInt("GameServerInfo", "EB_AttackStart", 45, gDirPath.GetNewPath(FILE_SERVER_COMMON));
	g_ElfBuffTime = GetPrivateProfileInt("GameServerInfo", "EB_BuffTime", 1800, gDirPath.GetNewPath(FILE_SERVER_COMMON));

#if (CUSTOM_PETS==1)
	gCustomPets.Load(gDirPath.GetNewPath("Custom\\CustomPets.txt"));
#endif
}

#endif

void ReadCommonlocInfo()
{
	char szTemp[10];
	char* cvstr;

	GetPrivateProfileString("GameServerInfo","ClientExeVersion","1.02.15",szTemp,8,gDirPath.GetNewPath(FILE_SERVER_INFO));
	GetPrivateProfileString("GameServerInfo","ClientExeSerial","",szGameServerExeSerial,20,gDirPath.GetNewPath(FILE_SERVER_INFO));
	LogAddC(4,"[VERSION RELOAD] Serial:[%s] Version:[%s]",szGameServerExeSerial,szTemp);

	cvstr = strtok(szTemp, ".");
	szClientVersion[0] = cvstr[0];
	cvstr = strtok(NULL, ".");
	szClientVersion[1] = cvstr[0];
	szClientVersion[2] = cvstr[1];
	cvstr = strtok(NULL, ".");
	szClientVersion[3] = cvstr[0];
	szClientVersion[4] = cvstr[1];
}