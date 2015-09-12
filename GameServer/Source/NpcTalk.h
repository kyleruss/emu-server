#ifndef NPCTALK_H
#define NPCTALK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_NPC_PHANTOM_BUFFER	180

#include "IllusionTempleEvent.h"

struct GC_ANS_NPC_TALK_V1
{
	PHEADB	PHeader;	// C3:30
	BYTE NpcNumber;
	BYTE Param1;
	BYTE Param2;
	BYTE Param3;
	BYTE Param4;
};

struct GC_ANS_NPC_TALK_V2
{
	PHEADB	PHeader;	// C3:30
	BYTE NpcNumber;
	BYTE Param1;
	BYTE Param2;
	BYTE Param3;
	BYTE Param4;
	BYTE Param5;
	BYTE Param6;
};

BOOL NpcTalk(LPOBJ lpNpc, LPOBJ lpObj);

int NpcBattleAnnouncer(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcFasi(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcGuildMasterTalk(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcRolensiaGuard(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcChaosGoblelin(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcRusipher(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcPosionGirl(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcDeviasWizard(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcDeviasGuard(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcDeviasWareHousemen(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcWarehouse(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcNoriaRara(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcDeviasMadam(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcEventChipNPC(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcRorensiaSmith(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcNoriJangIn(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcQuestCheck(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcServerDivision(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcRoadMerchant(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcAngelKing(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcAngelMessanger(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcRolensiaMadam(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcDeviasWeapon(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcDarkSpiritTrainer(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcJewelMixDealer(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcSiegeMachine_Attack(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcSiegeMachine_Defense(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcElderCircle(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcCastleGuard(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcCastleGateLever(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcCastleCrown(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcCastleSwitch(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcShadowPhantom(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcMainatenceMachine(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcReira(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcChaosCardMaster(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcGensMercenary(LPOBJ lpNpc, LPOBJ lpObj);

#if (GS_CASTLE == 1)

BOOL NpcCastleHuntZoneGuard(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcCrywolfAltar(LPOBJ lpNpc, LPOBJ lpObj);

#endif

BOOL NpcDrinkSellerLindsay(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcDrinkSellerHillary(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcFireWorkGirl(LPOBJ lpNpc,LPOBJ lpObj );

BOOL NpcIllusionMaster(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcIllusionHolyStatue(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcIllusionTeamForge(LPOBJ lpNpc,LPOBJ lpObj );
BOOL NpcAlliedTeamForge(LPOBJ lpNpc,LPOBJ lpObj );

BOOL NpcSilvia(LPOBJ lpNpc,LPOBJ lpObj );
BOOL NpcMarseille(LPOBJ lpNpc,LPOBJ lpObj );
BOOL NpcLeah(LPOBJ lpNpc,LPOBJ lpObj );
BOOL NpcCherryBlossom(LPOBJ lpNpc,LPOBJ lpObj );

BOOL NpcSeedMaster(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcSeedResearch(LPOBJ lpNpc,LPOBJ lpObj );

BOOL NpcSanta(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcSnowman(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcLitleSanta(LPOBJ lpNpc,LPOBJ lpObj,int Buff);

BOOL  NpcDelgado(LPOBJ lpNpc,LPOBJ lpObj );
BOOL NpcTitus(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcImperialFort(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcLugard(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcCompensationBox(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcGoldCompensationBox(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcDuprianSteward(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcVanertSteward(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcJulia(LPOBJ lpNpc, LPOBJ lpObj);
BOOL NpcDavid(LPOBJ lpNpc,LPOBJ lpObj);

BOOL NpcCaptainSlaugh(LPOBJ lpNpc,LPOBJ lpObj);
BOOL NpcAdniel(LPOBJ lpNpc,LPOBJ lpObj );
BOOL NpcSirLesnar(LPOBJ lpNpc,LPOBJ lpObj);

BOOL NpcPrivateStoreBoard(LPOBJ lpNpc,LPOBJ lpObj);

#endif