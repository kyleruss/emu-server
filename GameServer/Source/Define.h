#pragma once
// -------------------------------------------------------------------------------

// regional changes
#define PROTOCOL_KOR					0
#define PROTOCOL_GMO					1
#define PROTOCOL_USE					PROTOCOL_GMO

//-> Switches (Debug)
#define DEBUG_PROTOCOL					FALSE
#define DEBUG_LICENSE					FALSE

//-> Switches (Test)
#define ENABLETEST_SEASON10DOT2			FALSE
#define ENABLETEST_SEASON8DOT3			FALSE
#define ENABLETEST_MUUN					FALSE
#define ENABLETEST_ARCA					FALSE
#define ENABLETEST_ACHERON				FALSE
#define ENABLETEST_RUMMY				FALSE
#define ENABLETEST_WZINVENTORYEXPAND	TRUE
#define ENABLETEST_NEWPVP				TRUE
#define ENABLETEST_ITEMUPGRADEJEWELS	TRUE
#define ENABLETEST_MONSTERITEMDROPEX	TRUE
#define ENABLETEST_MONSTERMONEYDROP		TRUE
#define ENABLETEST_CUSTOM_JEWELS		FALSE
#define ENABLETEST_CHEATGUARD			TRUE
#define ENABLETEST_CHAOSMACHINEMNGR		FALSE
#define ENABLETEST_ZTLUA				FALSE
#define ENABLETEST_ZTUNIT				FALSE
#define ENABLETEST_FORCEOPTIMIZE		TRUE
#define ENABLETEST_ITEMEXCELLENTOPTION	TRUE

//-> Switches (Main)
#define ENABLE_WINLICENSE_SDK			FALSE
#define ENABLE_LIVEPROC_CHECK			FALSE

#define ENABLE_WZ_CHECKSUM				FALSE
#define ENABLE_ZMULTISERVER_UPDATE_3	FALSE

#define ENABLE_CUSTOM_CLASSCALC			TRUE
#define ENABLE_CUSTOM_OFFLINETRADE		TRUE
#define ENABLE_CUSTOM_HARDCORE			TRUE
#define ENABLE_CUSTOM_PVPBALANCE		TRUE
#define ENABLE_CUSTOM_MONSTERSPAWNER	TRUE

#define ENABLE_FIX_PARTYPK				TRUE
#define ENABLE_FIX_NPCTALKPK			TRUE
#define ENABLE_FIX_EVENTPK				TRUE
#define ENABLE_FIX_EVENTMASTERLEVEL		FALSE
#define ENABLE_FIX_HARMONYANCIENT		TRUE

#define ENABLE_LOG_EXPERIENCE			FALSE
#define ENABLE_SWITCH_CS_DAMAGEDEC		TRUE
#define ENABLE_SWITCH_CS_BACKSPRING		TRUE
#define ENABLE_RESET_PERSONALSHOP		FALSE
// -------------------------------------------------------------------------------

//-> Path defines
#define LOG_PATH_MAIN					"Log\\"
#define LOG_PATH_CRASH					"Log\\Crash\\"
#define LOG_PATH_DROP					".\\Log\\Drop"
#define LOG_NAME_DROP					"Drop"
#define LOG_PATH_CONNECT				".\\Log\\Connect"
#define LOG_NAME_CONNECT				"Connect"
#define LOG_PATH_KUNDUN					".\\Log\\Event"
#define LOG_NAME_KUNDUN					"Kundun"
#define LOG_PATH_RESET					".\\Log\\Reset"
#define	LOG_NAME_RESET					"Reset"
#define LOG_PATH_CHEATGUARD				".\\Log\\CheatGuard"
#define	LOG_NAME_CHEATGUARD				"CheatGuard"
#define LOG_PATH_LUA					".\\Log\\Scripting"
#define LOG_NAME_LUA					"Lua"
#define LOG_PATH_CREDITS				".\\Log\\Credits"
#define LOG_NAME_CREDITS				"Credits"
#define LOG_NAME_DEBUG					"Debug"
#define LOG_PATH_DEBUG					".\\Log\\Debug"
// ----
#define FOLDER_DATA						"..\\Data\\"
#define FOLDER_ROOT						"..\\..\\"
// ----
#define FILE_CASHSHOP_OPTION			"CashShop\\CashShopOption.dat"
#define FILE_SERVER_INFO				"ServerInfo.dat"
#define FILE_SERVER_COMMON				"CommonServer.cfg"
#define FILE_PROTECT_CHECKSUM			"CheckSum.dat"
#define FILE_PROTECT_DEC				"Dec1.dat"
#define FILE_PROTECT_ENC				"Enc2.dat"
#define FILE_MESSAGELIST				"Message.xml"
#define FILE_WHITELIST					"ConnectMember.txt"
#define FILE_MASTERLEVEL				"MasterLevel.xml"
#define FILE_GENS						"GensSystem.xml"
#define FILE_EVENTITEMBAG				"EventItemBag.xml"
#define PATH_EVENTITEMBAG				"EventItemBag\\"
#define FILE_PARTYBONUS					"PartyBonus.xml"
#define FILE_CHEATGUARD					"CheatGuard.xml"
#define FILE_CHAOSMACHINE				"ChaosMachine.xml"
#define PATH_CHAOSMACHINE				"ChaosMachine\\"
// ----
#define FILE_REPACK_CONF				"RepackConfig.ini"
// ----
#define FILE_CUSTOM_CLASSCALC			"Custom\\ClassCalc.ini"
#define FILE_CUSTOM_PREMIUM				"Custom\\Premium.xml"
#define FILE_CUSTOM_ITEMDROPRATE		"Custom\\ItemDropRate.xml"
#define FILE_CUSTOM_ITEMPRICE			"Custom\\ItemPrice.xml"
#define FILE_CUSTOM_NOTICE				"Custom\\Notice.xml"
#define FILE_CUSTOM_USERDIEITEMDROP		"Custom\\UserDieItemDrop.xml"
#define FILE_CUSTOM_MONSTERSTATINFO		"Custom\\MonsterStatInfo.xml"
#define FILE_CUSTOM_COMMANDMANAGER		"Custom\\CommandManager.xml"
#define FILE_CUSTOM_EXTRACLASSBALANCE	"Custom\\ExtraClassBalance.xml"
#define FILE_CUSTOM_MAPRATEINFO			"Custom\\MapRateInfo.xml"
#define FILE_CUSTOM_ACHIEVEMENTS		"Custom\\Achievements.xml"
#define FILE_CUSTOM_RESETSYSTEM			"Custom\\ResetSystem.xml"
#define FILE_CUSTOM_PAYMENTEX			"Custom\\PaymentEx.xml"
#define FILE_CUSTOM_OFFLINETRADE		"Custom\\OfflineTrade.xml"
#define FILE_CUSTOM_HARDCOREMODE		"Custom\\HardcoreMode.xml"
#define FILE_CUSTOM_PVPBALANCE			"Custom\\PvPBalance.xml"
#define FILE_CUSTOM_MONSTERSPAWNER		"Custom\\MonsterSpawner.xml"
#define FILE_CUSTOM_PERSONALSHOPMAPPING "Custom\\PersonalShop.xml"

// ----
#define FILE_EVENT_ARCA_DATA			"Event\\ArcaBattle.dat"
#define FILE_EVENT_BLOODCASTLE			"Event\\BloodCastle.dat"
#define FILE_EVENT_CASTLEDEEP			"Event\\CastleDeep.dat"
#define FILE_EVENT_CHAOSCASTLE			"Event\\ChaosCastle.dat"
#define FILE_EVENT_CRYWOLF				"Event\\Crywolf.dat"
#define FILE_EVENT_DEVILSQUARE			"Event\\DevilSquare.dat"
#define FILE_EVENT_DOPPELGANGER			"Event\\Doppelganger.dat"
#define FILE_EVENT_MANAGEMENT			"Event\\EventManagement.dat"
#define FILE_EVENT_ILLUSIONTEMPLE		"Event\\IllusionTemple.dat"
#define FILE_EVENT_IMPERIAL_REWARD		"Event\\ImperialGuardianReward.dat"
#define FILE_EVENT_KANTURU_MAIN			"Event\\Kanturu.dat"
#define FILE_EVENT_KANTURU_BASE			"Event\\KanturuMonsterSetBase.txt"
#define FILE_EVENT_MINING				"Event\\MiningSystem.xml"
#define FILE_EVENT_CASTLESIEGE			"Event\\CastleSiege.dat"
#define FILE_EVENT_RAKLION				"Event\\Raklion.dat"
#define FILE_EVENT_RINGATTACK			"Event\\RingAttack.dat"
#define FILE_EVENT_XMASATTACK			"Event\\XMasAttack.dat"
// ----
#define PATH_EVENTBAG_DOPPELGANGER		"EventItemBag\\Doppelganger\\"
#define PATH_EVENTBAG_IMPERIALGUARDIAN	"EventItemBag\\ImperialGuardian\\"
#define FILE_EVENTBAG_GAMBLER			"EventItemBag\\Gambler\\Gambler.txt"
#define FILE_EVENTBAG_CHAOSCARD_NORMAL	"EventItemBag\\ChaosCard\\1_Normal.txt"
#define FILE_EVENTBAG_CHAOSCARD_MINI	"EventItemBag\\ChaosCard\\2_Mini.txt"
#define FILE_EVENTBAG_CHAOSCARD_GOLD	"EventItemBag\\ChaosCard\\3_Gold.txt"
#define FILE_EVENTBAG_CHAOSCARD_RARE	"EventItemBag\\ChaosCard\\4_Rare.txt"
// ----
#define FILE_ITEM_COMMON				"Item\\Item.txt"
#define FILE_ITEM_ADDOPTION				"Item\\ItemAddOption.txt"
#define FILE_ITEM_ELEMENT				"Item\\ItemElement.txt"
#define FILE_ITEM_PENTAGRAM				"Item\\ItemPentagram.txt"
#define FILE_ITEM_SETOPTION				"Item\\ItemSetOption.txt"
#define FILE_ITEM_SETTYPE				"Item\\ItemSetType.txt"
#define FILE_ITEM_SOCKET				"Item\\ItemSocket.txt"
#define FILE_ITEM_HARMONY_OPTION		"Item\\JewelOfHarmonyOption.txt"
#define FILE_ITEM_HARMONY_SMELT			"Item\\JewelOfHarmonySmelt.txt"
#define FILE_ITEM_UPGRADE_JEWELS		"Item\\ItemUpgradeJewels.xml"
#define FILE_ITEM_WINGS_OPTION			"Item\\ItemWingsOption.xml"
#define FILE_ITEM_EXCELLENT_OPTION		"Item\\ItemExcellentOption.xml"
#define FILE_ITEM_MOVE_RULES			"Item\\ItemMoveRules.xml"
// ----
#define FILE_MONSTER_MAIN				"Monster\\Monster.xml"
#define FILE_MONSTER_AI_ELEMENT			"Monster\\MonsterAIElement.txt"
#define FILE_MONSTER_AI_GROUP			"Monster\\MonsterAIGroup.txt"
#define FILE_MONSTER_AI_UNIT			"Monster\\MonsterAIUnit.txt"
#define FILE_MONSTER_AUTOMATA			"Monster\\MonsterAutomata.txt"
#define FILE_MONSTER_BASE				"Monster\\MonsterSetBase.xml"
#define FILE_MONSTER_SKILL				"Monster\\MonsterSkill.txt"
#define FILE_MONSTER_SKILL_ELEMENT		"Monster\\MonsterSkillElement.txt"
#define FILE_MONSTER_SKILL_UNIT			"Monster\\MonsterSkillUnit.txt"
#define FILE_MONSTER_STAT				"Monster\\MonsterStat.txt"
#define FILE_MONSTER_ITEMDROPRATE		"Monster\\MonsterItemDropRate.xml"
#define FILE_MONSTER_MONEYDROP			"Monster\\MonsterMoneyDrop.xml"
// ----
#define FILE_QUEST_MAIN					"Quest\\Quest.txt"
#define FILE_QUEST_INFO					"Quest\\QuestInfo.txt"
#define FILE_QUEST_PROGRESS				"Quest\\QuestProgress.txt"
// ----


#define FILE_SKILL_COMMON				"Skill\\Skill.txt"
#define FILE_SKILL_ADDITION				"Skill\\SkillAdditionInfo.dat"
#define FILE_SKILL_BUFFEFFECT			"Skill\\BuffEffect.txt"
#define FILE_SKILL_MASTERTREE			"Skill\\MasterSkillTree.txt"
#define FILE_SKILL_BUFFSKILLEFFECT		"Skill\\BuffSkillEffect.xml"
#define FILE_SKILL_MASTERTREE_EFFECT	"Skill\\MasterSkillEffect.lua"
#define FILE_SKILL_HITBOX_ELECT			"Skill\\HitBox\\SkillElect.hit"
#define FILE_SKILL_HITBOX_SPEAR			"Skill\\HitBox\\SkillSpear.hit"
// ----
#define FILE_WORLD_MAPSERVER			"World\\MapServer.xml"
#define FILE_WORLD_MAPATTRIBUTE			"World\\MapAttribute.xml"
#define FOLDER_WORLD_TERRAIN			"World\\Terrain\\"
#define FILE_WORLD_MINIMAP				"World\\MiniMap.xml"
#define FILE_WORLD_MOVE_GATE			"World\\MoveGate.xml"
#define FILE_WORLD_MOVE_REQ				"World\\MoveList.xml"
#define FILE_WORLD_MOVE_LEVEL			"World\\MoveLevel.xml"
#define FILE_WORLD_MOVEPATH_CRYWOLF		"World\\MovePath\\Crywolf.dat"
#define FILE_WORLD_MOVEPATH_DOPPEL66	"World\\MovePath\\Doppelganger_66.dat"
#define FILE_WORLD_MOVEPATH_DOPPEL67	"World\\MovePath\\Doppelganger_67.dat"
#define FILE_WORLD_MOVEPATH_DOPPEL68	"World\\MovePath\\Doppelganger_68.dat"
#define FILE_WORLD_MOVEPATH_DOPPEL69	"World\\MovePath\\Doppelganger_69.dat"
// ----
#define FILE_LUA_CHARACTERBASICS		"Scripting\\CharacterBasics.lua"
#define FILE_LUA_SHAREDINFORMATION		"Scripting\\SharedInformation.lua"
// -------------------------------------------------------------------------------