#include "stdafx.h"
#include "protocol.h"
#include "user.h"
#include "giocp.h"
#include "Event.h"
#include "logproc.h"
#include "GameMain.h"
#include "..\common\winutil.h"
#include "TNotice.h"
#include "QuestInfo.h"
#include "ConMember.h"
#include "GameServer.h"
#include "BattleSoccerManager.h"
#include "ChaosBox.h"
#include "CastleSiegeSync.h"
#include "TUnion.h"
#include "JewelMixSystem.h"
#include "Weapon.h"
#include "DBSockMng.h"
#include "KalimaGate.h"
#include "ObjUseSkill.h"
#include "ObjCalCharacter.h"
#include "NpcTalk.h"
#include "Shop.h"
#include "CashItemPeriodSystem.h"
#include "BattleSoccer.h"
#include "BattleSoccerManager.h"
#include "gObjMonster.h"
#include "ComboAttack.h"
#include "Gate.h"
#include "MultiAttackHackCheck.h"
#include "ItemAddOption.h"
#include "JewelOfHarmonySystem.h"
#include "PacketCheckSum.h"
#include "DarkSpirit.h"
#include "Crywolf.h"
#include "CrywolfAltar.h"
#include "CrywolfSync.h"
#include "DevilSquare.h"
#include "KanturuEntranceNPC.h"
#include "CashShop.h"
#include "MapServerManager.h"
#include "CastleSiege.h"
#include "LifeStone.h"
#include "BuffEffectSlot.h"
#include "Sprotocol.h"
#include "DSProtocol.h"
#include "EDSProtocol.h"
#include "MasterSkillSystem.h"
#include "MasterLevelSkillTreeSystem.h"
#include "Mercenary.h"
#include "Guardian.h"
#include "SocketItemOption.h"
#include "MoveCommand.h"
#include "PCBangPointSystem.h"
#include "Gambling.h"
#include "DoubleGoerEvent.h"
#include "ImperialGuardian.h"
#include "GameEvent.h"
#include "ElementalSystem.h"
#include "TemporaryUserManager.h"
#include "MiningSystem.h"
#include "MiniMap.h"
#include "ShopBoard.h"
#include "GuildMatch.h"
#include "EventItemBag.h"
#include "ChaosCastle.h"
#include "IllusionTempleEvent.h"
#include "SnakeBegins.h"
#include "ItemPrice.h"
#include "DSProtocol.h"
#include "PersonalShopMapping.h"
#ifdef __NOVUS__
#include "ExPoint.h"
#include "SummonSpot.h"
#endif

#if (ENABLETEST_ARCA == 1)
#include "ArcaBattle.h"
#endif

#if (ENABLETEST_ACHERON == 1)
#include "AcheronEvent.h"
#endif

#if (ENABLETEST_RUMMY == 1)
#include "MuRummy.h"
#endif

#if (ENABLETEST_ITEMUPGRADEJEWELS == 1)
#include "ItemUpgradeJewels.h"
#endif

#if (ENABLETEST_MUUN == 1)
#include "MuunSystem.h"
#endif

#if (ENABLETEST_NEWPVP == 1)
#include "NewPVP.h"
#endif

#if (ENABLE_CUSTOM_MONSTERSPAWNER == 1)
#include "MonsterSpawner.h"
#endif

#include "ResetSystem.h"
#include "ChaosMachineManager.h"

CLogToFile g_CreditLog(LOG_NAME_CREDITS, LOG_PATH_CREDITS, TRUE);

int ltesttime;
int logincounttest;
int tempindex;
int iCount;

BOOL PacketCheckTime(LPOBJ lpObj)
{
	if ( ( GetTickCount() - lpObj->m_PacketCheckTime ) < 300 )
	{
		return FALSE;
	}

	lpObj->m_PacketCheckTime = GetTickCount();
	return TRUE;
}






BOOL DataEncryptCheck(int aIndex, BYTE protoNum, BOOL Encrypt)
{
	if ( Encrypt == FALSE )
	{
		LogAdd("Error-L1 : Not Encrypt %s %d", gObj[aIndex].AccountID, protoNum);
		CloseClient(aIndex);
		return FALSE;
	}

	return TRUE;
}

void ProtocolCore(BYTE protoNum, unsigned char * aRecv, int aLen, int aIndex, BOOL Encrypt, int serial)
{
	// Check if we wanna log the packets
	if ( gStalkProtocol )
	{
		if ( gStalkProtocolId[0] == gObj[aIndex].AccountID[0] )
		{
			if ( gStalkProtocolId[1] == gObj[aIndex].AccountID[1] )
			{
				if ( !strcmp(gStalkProtocolId, gObj[aIndex].AccountID ) )
				{
					LogAddHeadHex(gObj[aIndex].AccountID, (char*)aRecv, aLen);
				}
			}
		}
	}	// ig gStalkProtocol

	// Serial Checking
	if ( serial >= 0 )
	{
		if ( gObj[aIndex].Type == OBJ_USER )
		{
			if ( gNSerialCheck[aIndex].InCheck(serial) == FALSE )
			{
				LogAdd("Error-L1 : Socket Serial %s %d o_serial:%d serial:%d ",
					gObj[aIndex].AccountID, protoNum, gNSerialCheck[aIndex].GetSerial(), serial);

				CloseClient(aIndex);
			}
		}
	}	// if Serial Checking

	if ( protoNum == 0xF1 || protoNum == 0xF3 )
	{
#if( DEBUG_PROTOCOL == 1 )
		LogAddHeadHexFunc(1, (LPSTR)aRecv, aLen);
#endif
		switch ( protoNum )
		{
			case 0x0E:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGLiveClient((PMSG_CLIENTTIME *)aRecv, aIndex);
					tempindex = aIndex;
				}
				break;
			case 0xF1:
				{
					PMSG_DEFAULT2 * lpMsg2 = (PMSG_DEFAULT2 *)aRecv;
					
					switch ( lpMsg2->subcode )
					{
						case 0x01:
							if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
							{
								CSPJoinIdPassRequest((PMSG_IDPASS *)aRecv, aIndex);
							}
							break;
						case 0x02:
							if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
							{
								CGClientCloseMsg((PMSG_CLIENTCLOSE *)aRecv, aIndex);
							}
							break;
						case 0x03:
							if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
							{
								CGClientMsg((PMSG_CLIENTMSG *)aRecv, aIndex);
							}
							break;
					}
				}
				break;
			case 0xF3:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x00:
							DataServerGetCharListRequest(aIndex);
							break;
						case 0x01:
							CGPCharacterCreate((PMSG_CHARCREATE *)aRecv, aIndex);
							break;
						case 0x02:
							CGPCharDel((PMSG_CHARDELETE *)aRecv, aIndex);
							break;
						case 0x03:
							CGPCharacterMapJoinRequest((PMSG_CHARMAPJOIN *)aRecv, aIndex);
							break;
						case 0x06:
							CGLevelUpPointAdd((PMSG_LVPOINTADD *)aRecv, aIndex);
							break;
						case 0x12:
							gObjMoveDataLoadingOK(aIndex);
							break;
						case 0x15:
							{
								PMSG_REQ_CHAR_PASSWORD* lpMsg = (PMSG_REQ_CHAR_PASSWORD*)aRecv;
								PMSG_ANS_CHECK_PASSWORD pMsg;

								PHeadSubSetB((LPBYTE)&pMsg,0xF3,0x16,sizeof(pMsg));
								memcpy(pMsg.Password,"1111111111",10);
								memcpy(pMsg.Name,lpMsg->Name,10);

								pMsg.btResult = 0;

								DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
							}
							break;
						/*case 0x15:
						  CGReqCheckChangeName
						  break;
						case 0x16:
						  CGChangeName
						  break;*/
						case 0x21:
							CGEnableUsePolymorphSkinrRecv((PMSG_REQ_USE_POLYMORPH_SKIN*)aRecv, aIndex);
						  break;
						case 0x30:
							GCSkillKeyRecv((PMSG_SKILLKEY *)aRecv, aIndex);
							break;
						case 0x31:
							//CGNotifyGuarditBreach
							break;
						case 0x52:
							g_MasterSkillSystem.CGReqGetMasterLevelSkill((PMSG_REQ_MASTERLEVEL_SKILL*)aRecv,aIndex);
							break;
					}
				}
				break;
		}
	}
	else
	{
		LPOBJ lpObj = &gObj[aIndex];

#if( DEBUG_PROTOCOL == 1 )
		LogAddHeadHexFunc(1, (LPSTR)aRecv, aLen);
#endif

		switch ( protoNum )
		{
			case 0x00:
				tempindex = aIndex;
				PChatProc((PMSG_CHATDATA *)aRecv, aIndex);
				break;
			case 0x01:
				CGChatRecv((PMSG_CHATDATA_NUMBER *)aRecv, aIndex);
				break;
			case 0x02:
				CGChatWhisperRecv((PMSG_CHATDATA_WHISPER *)aRecv, aIndex);
				break;
			case 0x03:
				CGCheckMainRecv((PMSG_CHECK_MAINEXE_RESULT *)aRecv, aIndex);
				break;
			case 0x04:
				break;
			case 0x0E:
				CGLiveClient((PMSG_CLIENTTIME *)aRecv, aIndex);
				break;
				
#if (__ENG__ == 1)
			case 0xD4:
#else
			// KOR PROTOCOL !!!
			case 0xD7:
#endif
				PMoveProc((PMSG_MOVE *)aRecv, aIndex);
				break;	
#if (__ENG__ == 1)
			case 0x15:// ENG PROTOCOL !!!
#else
			case 0x10:// KOR PROTOCOL !!!
#endif
				RecvPositionSetProc((PMSG_POSISTION_SET *)aRecv, aIndex);
				break;
#if (__ENG__ == 1)
			case 0x11:	// ENG PROTOCOL !!!
#else
			case 0xDF:	// KOR PROTOCOL !!!
#endif
				CGAttack((PMSG_ATTACK *)aRecv, aIndex);
				break;
			case 0x18:
				CGActionRecv((PMSG_ACTION *)aRecv, aIndex);
				break;
			case 0x19:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGMagicAttack((PMSG_MAGICATTACK *)aRecv, aIndex);
				}
				break;
			case 0x1B:
				CGMagicCancel((PMSG_MAGICCANCEL *)aRecv, aIndex);
				break;
			case 0x1C:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGTeleportRecv((PMSG_TELEPORT *)aRecv, aIndex);
				}
				break;
			case 0xB0:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGTargetTeleportRecv((PMSG_TARGET_TELEPORT *)aRecv, aIndex);
				}
				break;
				
#if (__ENG__ == 1)
			case 0xDB:// ENG PROTOCOL
#else
			case 0xD3: // KOR PROTOCOL
#endif
				CGBeattackRecv(aRecv, aIndex, FALSE);
				break;
			case 0x1E:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGDurationMagicRecv((PMSG_DURATION_MAGIC_RECV *)aRecv, aIndex);
				}
				break;
			case 0x22:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGItemGetRequest((PMSG_ITEMGETREQUEST *)aRecv, aIndex);
				}
				break;
			case 0x23:
				CGItemDropRequest((PMSG_ITEMTHROW *)aRecv, aIndex, 0);
				break;
			case 0x24:
				CGInventoryItemMove((PMSG_INVENTORYITEMMOVE *)aRecv, aIndex);
				break;
			case 0x26:
				CGUseItemRecv((PMSG_USEITEM *)aRecv, aIndex);
				break;
			case 0x30:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGTalkRequestRecv((PMSG_TALKREQUEST *)aRecv, aIndex);
				}
				break;
			case 0x31:
				CGCloseWindow(aIndex);
				break;
			case 0x32:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGBuyRequestRecv((PMSG_BUYREQUEST *)aRecv, aIndex);
				}
				break;
			case 0x33:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGSellRequestRecv((PMSG_SELLREQUEST *)aRecv, aIndex);
				}
				break;
			case 0x34:
				CGModifyRequestItem((PMSG_ITEMDURREPAIR *)aRecv, aIndex);
				break;
			case 0x36:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGTradeRequestSend((PMSG_TRADE_REQUEST *)aRecv, aIndex);
				}
				break;
			case 0x37:
				CGTradeResponseRecv((PMSG_TRADE_RESPONSE *)aRecv, aIndex);
				break;
			case 0x3A:
				CGTradeMoneyRecv((PMSG_TRADE_GOLD *)aRecv, aIndex);
				break;
			case 0x3C:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGTradeOkButtonRecv((PMSG_TRADE_OKBUTTON *)aRecv, aIndex);
				}
				break;
			case 0x3D:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGTradeCancelButtonRecv(aIndex);
				}
				break;
			case 0x3F:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x01:
							CGPShopReqSetItemPrice((PMSG_REQ_PSHOP_SETITEMPRICE *)aRecv, aIndex);
							break;
						case 0x02:
							CGPShopReqOpen((PMSG_REQ_PSHOP_OPEN *)aRecv, aIndex);
							break;
						case 0x03:
							CGPShopReqClose(aIndex);
							break;
						case 0x05:
							CGPShopReqBuyList((PMSG_REQ_BUYLIST_FROM_PSHOP *)aRecv, aIndex);
							break;
						case 0x06:
							CGPShopReqBuyItem((PMSG_REQ_BUYITEM_FROM_PSHOP *)aRecv, aIndex);
							break;
					}
				}
				break;
			case 0x40:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGPartyRequestRecv((PMSG_PARTYREQUEST *)aRecv, aIndex);
				}
				break;
			case 0x41:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					CGPartyRequestResultRecv((PMSG_PARTYREQUESTRESULT *)aRecv, aIndex);
				}
				break;
			case 0x42:
				CGPartyList(aIndex);
				break;
			case 0x43:
				if ( PacketCheckTime(&gObj[aIndex]) == TRUE )
				{
					if(!IT_MAP_RANGE(gObj[aIndex].MapNumber)
						&& !DG_MAP_RANGE(gObj[aIndex].MapNumber)
						&& !IF_MAP_RANGE(gObj[aIndex].MapNumber)
						)
					{
						CGPartyDelUser((PMSG_PARTYDELUSER *)aRecv, aIndex);
					}
				}
				break;
			case 0x4A:
				GCMonkMagicAttack((PMSG_MAGICATTACK*)aRecv,aIndex);
				break;
			case 0x4B:
				GCMonkDarkSideTargetSelect((PMSG_MONK_DARKSIDE_RECV*)aRecv,aIndex);
				break;
			case 0x4C:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch (lpDef->subcode)
					{
					case 0:
						CGMiningRequest((PMSG_MINING_REQUEST*)aRecv,aIndex);
						break;
					case 1:
						CGRewardRequest((PMSG_MINING_REWARD_REQUEST*)aRecv,aIndex);
						break;
					case 3:
						CGMiningEnd((PMSG_MINING_END*)aRecv,aIndex);
						break;
					}
				}
				break;
#if (ENABLETEST_RUMMY == 1)
			case 0x4D:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch (lpDef->subcode)
					{
					case 0:
					  //if ( DataEncryptCheck(aIndex, protoNum, Encrypt) )
						//CGEventItemGetRequest((PMSG_EVENTITEM_GETREQUEST *)aRecv, aIndex);
					  break;
					case 1:
					  //CGEventItemDropRequest((PMSG_EVENTITEM_THROW*)aRecv, aIndex);
					  break;
					case 0xF:
					  g_CMuRummyMng.CGReqMuRummyEventOpen((_tagPMSG_REQ_MURUMMY_EVENT_OPEN*)aRecv, aIndex);
					  break;
					case 0x10:
					  g_CMuRummyMng.CGReqMuRummyStart((_tagPMSG_REQ_MURUMMY_INFO*)aRecv, aIndex);
					  break;
					case 0x11:
					  g_CMuRummyMng.CGReqCardReveal((_tagPMSG_REQ_REVEAL_CARD*)aRecv, aIndex);
					  break;
					case 0x12:
					  g_CMuRummyMng.CGReqCardMove((_tagPMSG_REQ_CARD_MOVE*)aRecv, aIndex);
					  break;
					case 0x13:
					  g_CMuRummyMng.CGReqCardReMove((_tagPMSG_REQ_CARD_REMOVE*)aRecv, aIndex);
					  break;
					case 0x14:
					  g_CMuRummyMng.CGReqCardMatch((_tagPMSG_REQ_CARD_MATCH*)aRecv, aIndex);
					  break;
					case 0x15:
					  g_CMuRummyMng.CGReqMuRummyEnd((_tagPMSG_REQ_MURUMMY_END*)aRecv, aIndex);
					  break;
					}
				}
				break;
#endif
#if (ENABLETEST_MUUN == 1)
			case 0x4E:
				{
					PMSG_DEFAULT2* lpDef = (PMSG_DEFAULT2*)aRecv;
					switch (lpDef->subcode) 
					{
					case 0:
						if (DataEncryptCheck(aIndex, protoNum, Encrypt)) {
							CGMuunItemGetRequest((PMSG_MUUNITEM_GETREQUEST*)aRecv, aIndex);
						}
						break;
					case 8:
						g_CMuunSystem.CGMuunInventoryUseItemRecv((PMSG_USEITEM_MUUN_INVEN*)aRecv, aIndex);
						break;
					case 9:
						if (DataEncryptCheck(aIndex, protoNum, Encrypt)) {
							CGMuunItemSellRequestRecv((PMSG_MUUN_ITEM_SELLREQUEST*)aRecv, aIndex);
						}
						break;
					}
					break;
				}
				break;
#endif
			case 0x50:
				CGGuildRequestRecv((PMSG_GUILDJOINQ *)aRecv, aIndex);
				break;
			case 0x51:
				CGGuildRequestResultRecv((PMSG_GUILDQRESULT *)aRecv, aIndex);
				break;
			case 0x52:
				CGGuildListAll(aIndex);
				break;
			case 0x53:
				CGGuildDelUser((PMSG_GUILDDELUSER *)aRecv, aIndex);
				break;
			case 0x54:
				CGGuildMasterAnswerRecv((PMSG_GUILDMASTERANSWER *)aRecv, aIndex);
				break;
			case 0x55:
				CGGuildMasterInfoSave(aIndex, (PMSG_GUILDINFOSAVE *)aRecv);
				break;
			case 0x57:
				CGGuildMasterCreateCancel(aIndex);
				break;
			case 0x61:
				GCGuildWarRequestSendRecv((PMSG_GUILDWARSEND_RESULT *)aRecv, aIndex);
				break;
			case 0x66:
				GCGuildViewportInfo((PMSG_REQ_GUILDVIEWPORT *)aRecv, aIndex);
				break;
			case 0xB1:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;
					
					switch ( lpDef->subcode )
					{
						case 0x01:
							if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
							{
								CGReqMapSvrAuth((PMSG_REQ_MAPSERVERAUTH *)aRecv, aIndex);
							}
							break;
					}
				}
				break;
			case 0xB2:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x00:
							CGReqCastleSiegeState((PMSG_REQ_CASTLESIEGESTATE *)aRecv, aIndex);
							break;
						case 0x01:
							CGReqRegCastleSiege((PMSG_REQ_REGCASTLESIEGE *)aRecv, aIndex);
							break;
						case 0x02:
							CGReqGiveUpCastleSiege((PMSG_REQ_GIVEUPCASTLESIEGE *)aRecv, aIndex);
							break;
						case 0x03:
							CGReqGuildRegInfo((PMSG_REQ_GUILDREGINFO *)aRecv, aIndex);
							break;
						case 0x04:
							CGReqRegGuildMark((PMSG_REQ_REGGUILDMARK *)aRecv, aIndex);
							break;
						case 0x05:
							CGReqNpcBuy((PMSG_REQ_NPCBUY *)aRecv, aIndex);
							break;
						case 0x06:
							CGReqNpcRepair((PMSG_REQ_NPCREPAIR *)aRecv, aIndex);
							break;
						case 0x07:
							CGReqNpcUpgrade((PMSG_REQ_NPCUPGRADE *)aRecv, aIndex);
							break;
						case 0x08:
							CGReqTaxMoneyInfo((PMSG_REQ_TAXMONEYINFO *)aRecv, aIndex);
							break;
						case 0x09:
							CGReqTaxRateChange((PMSG_REQ_TAXRATECHANGE *)aRecv, aIndex);
							break;
						case 0x10:
							CGReqMoneyDrawOut((PMSG_REQ_MONEYDRAWOUT *)aRecv, aIndex);
							break;
						case 0x12:
							CGReqCsGateOperate((PMSG_REQ_CSGATEOPERATE *)aRecv, aIndex);
							break;
						case 0x1B:
							CGReqCsMiniMapData((PMSG_REQ_MINIMAPDATA *)aRecv, aIndex);
							break;
						case 0x1C:
							CGReqStopCsMiniMapData((PMSG_REQ_STOPMINIMAPDATA *)aRecv, aIndex);
							break;
						case 0x1D:
							CGReqCsSendCommand((PMSG_REQ_CSCOMMAND *)aRecv, aIndex);
							break;
						case 0x1F:
							CGReqCsSetEnterHuntZone((PMSG_REQ_CSHUNTZONEENTER *)aRecv, aIndex);
							break;
					}
				}
				break;
			case 0xB3:
				CGReqNpcDbList((PMSG_REQ_NPCDBLIST *)aRecv, aIndex);
				break;
			case 0xB4:
				CGReqCsRegGuildList((PMSG_REQ_CSREGGUILDLIST *)aRecv, aIndex);
				break;
			case 0xB5:
				CGReqCsAttkGuildList((PMSG_REQ_CSATTKGUILDLIST *)aRecv, aIndex);
				break;
			case 0xB7:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x01:
							CGReqWeaponUse((PMSG_REQ_USEWEAPON *)aRecv, aIndex);
							break;
						case 0x04:
							CGReqWeaponDamageValue((PMSG_REQ_WEAPON_DAMAGE_VALUE *)aRecv, aIndex);
							break;
					}
				}
				break;
			case 0xB9:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x02:
							CGReqGuildMarkOfCastleOwner((PMSG_REQ_GUILDMARK_OF_CASTLEOWNER *)aRecv, aIndex);
							break;
						case 0x05:
							CGReqCastleHuntZoneEntrance((PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE *)aRecv,aIndex);
							break;
					}
				}
				break;
			case 0xBC:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x00:
							CGReqJewelMix((PMSG_REQ_JEWEL_MIX *)aRecv, aIndex);
							break;
						case 0x01:
							CGReqJewelUnMix((PMSG_REQ_JEWEL_UNMIX *)aRecv, aIndex);
							break;
					}
				}
				break;
			case 0xBD:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x00:
							CGReqCrywolfInfo((PMSG_REQ_CRYWOLF_INFO *)aRecv, aIndex);
							break;
						case 0x03:
							CGReqAlatrContract((PMSG_REQ_CRYWOLF_ALTAR_CONTRACT *)aRecv, aIndex);
							break;
						case 0x09:
							CGReqPlusChaosRate((PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE *)aRecv, aIndex);
							break;
					}
				}
				break;
			case 0xBF:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x00:
							GCReqmoveIllusionTemple((PMSG_REQ_MOVEILLUSIONTEMPLE*)aRecv, aIndex);
							break;
						case 0x02:
							GCIllusionSkillUseSend((PMSG_ILLUSION_REQ_SKILLUSE*)aRecv,aIndex);
							break;
						case 0x05:
							CGIllusionTempleReward(aIndex);
							break;
						case 0x0E:
							CGReqmoveDoubleGoer(aRecv,aIndex);
							break;
						case 0x17:
							CGReqmoveLorenMarket(aIndex);
							break;
						case 0x20:	//OK
							CGInventoryEquipment((_tagPMSG_REQ_INVENTORY_EQUIPMENT_ITEM *)aRecv, aIndex);
						case 0x51:
							CGRequestStartMuBot((PMSG_MUBOT_REQ_START*)aRecv,aIndex);
							break;
						default:
							LogAddTD("[0xBF] Unknown packet BF:%d",lpDef->subcode);
							break;
					}
				}
				break;
			//case 0xBE:
			case 0xE1:
				CGGuildAssignStatus((PMSG_GUILD_ASSIGN_STATUS_REQ *)aRecv, aIndex);
				break;
			case 0xE2:
				CGGuildAssignType((PMSG_GUILD_ASSIGN_TYPE_REQ *)aRecv, aIndex);
				break;
			case 0xE5:
				CGRelationShipReqJoinBreakOff((PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ *)aRecv, aIndex);
				break;
			case 0xE6:
				CGRelationShipAnsJoinBreakOff((PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS *)aRecv, aIndex);
				break;
			case 0xE7:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
					case 1:
//						g_MiniMap.SetOpenMinimap(aIndex);
						g_Minimap.CGReqSendMemberPosInfoStart(aIndex);
						break;
					case 2:
//						g_MiniMap.SetCloseMinimap(aIndex);
						g_Minimap.CGReqSendMemberPosInfoStop(aIndex);
						break;
					default:
//						LogAddTD("[0xE7] Unknown packet BF:%d",lpDef->subcode);
						g_Minimap.GCReqSendNpcPosInfo((_PMSG_REQ_NPC_POSITION*)aRecv, aIndex);
						break;
					}
				}
				break;
			case 0xE9:
				CGUnionList((PMSG_UNIONLIST_REQ *)aRecv, aIndex);
				break;
			case 0xEC:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch( lpDef->subcode )
					{
					case 0x00:
						g_ElementalSystem.CGInsertErtel(aIndex,(PMSG_REQ_INSERTERTEL*)aRecv);
						break;
					case 0x01:
						g_ElementalSystem.CGRemoveErtel(aIndex,(PMSG_REQ_REMOVEERTEL*)aRecv);
						break;
					case 0x02:
						g_ElementalSystem.CGCombineButtonClick((PMSG_ERTEL_COMBINE*)aRecv,aIndex);
						break;
					case 0x03:
						g_ElementalSystem.CGUpgradeButtonClick((PMSG_ERTEL_LEVELUP*)aRecv,aIndex);
						break;
					case 0x31:
						g_ShopBoard.CGReqItemSearch(aIndex, (SHOPBOARD_CGREQ_ITEM*)aRecv);
						break;
					}
				}
				break;
			case 0xED:
				{
					PMSG_DEFAULT2* lpDef = (PMSG_DEFAULT2*)aRecv;
					// ----
					switch(lpDef->subcode)
					{
					case 0x00:
						g_GuildMatch.CGReqOfferList(aIndex, (GUILDMATCH_CGREQ_OFFERLIST*)aRecv);
						break;
					case 0x01:
						g_GuildMatch.CGReqOfferListByFilter(aIndex, (GUILDMATCH_CGREQ_OFFERLISTFILTER*)aRecv);
						break;
					case 0x02:
						g_GuildMatch.CGReqOfferCreate(aIndex, (GUILDMATCH_CGREQ_OFFERCREATE*)aRecv);
						break;
					case 0x03:
						g_GuildMatch.CGReqOfferDelete(aIndex, (GUILDMATCH_CGREQ_OFFERDELETE*)aRecv);
						break;
					case 0x04:
						g_GuildMatch.CGReqJoinReq(aIndex, (GUILDMATCH_CGREQ_JOINREQ*)aRecv);
						break;
					case 0x05:
						g_GuildMatch.CGReqJoinReqDelete(aIndex, (GUILDMATCH_CGREQ_JOINREQDELETE*)aRecv);
						break;
					case 0x06:
						g_GuildMatch.CGReqJoinReqAnswer(aIndex, (GUILDMATCH_CGREQ_REQUESTANSWER*)aRecv);
						break;
					case 0x07:
						g_GuildMatch.CGReqJoinReqList(aIndex, (GUILDMATCH_CGREQ_REQUESTLIST*)aRecv);
						break;
					case 0x08:
						g_GuildMatch.CGReqJoinReqStatus(aIndex, (GUILDMATCH_CGREQ_JOINREQSTATUS*)aRecv);
						break;

					}
				}
				break;

			case 0xFE:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch( lpDef->subcode )
					{
#if (CUSTOM_OLYMP==1)
					case 0x01:
						g_CustomOlymp.CGRequestReg(aIndex);
						break;
					case 0x02:
						g_CustomOlymp.CGRequestReward(aIndex);
						break;
#endif

#if(__NOVUS == 1)
					case 0x03:
						g_ExPoint.Update(aIndex);
						break;
#endif
					case 0x05:
						g_ResetSystem.CG_RequestStart(aIndex, (PacketResetStart*)aRecv);
						break;

					case 0xFE:
						{
							if( gObj[aIndex].m_IfState.type == 21 )
							{
								gObj[aIndex].m_IfState.use = 0;
								gObj[aIndex].m_IfState.type = 0;
								gObj[aIndex].m_IfState.state = 0;
							}
						}
						break;
					}

				}
				break;
			case 0xEF:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;
					
					switch( lpDef->subcode )
					{
					case 0x00:
						CGReqPartyMatchReg((PMSG_PARTYMATCH_REG_REQ*)aRecv,aIndex);
						break;
					case 0x01:
						CGReqPartyMatchWindow((PMSG_PARTYMATCH_INFO_REQ*)aRecv,aIndex);
						break;
					case 0x02:
						CGReqPartyMatchJoin((PMSG_PARTYMATCH_JOIN_REQ*)aRecv,aIndex);
						break;
					case 0x03:
						CGReqPartyMatchWaitingList(aIndex);
						break;
					case 0x04:
						CGReqPartyMatchAcceptList(aIndex);
						break;
					case 0x05:
						CGReqPartyMatchRequestAnswer((PARTYMATCH_CGREQ_REQUESTANSWER*)aRecv, aIndex);
						break;
					case 0x06:
						CGReqPartyMatchCancel((PMSG_PARTYMATCH_CANCEL_REQ*)aRecv,aIndex);
						break;
					case 0x07:
						ClientTestSend(aIndex, (ClientTest*)aRecv);
						break;
					}
				}
				//LogAddHeadHexFunc(1,(PCHAR)aRecv,aLen);
				break;
			case 0xEB:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x01:
							CGRelationShipReqKickOutUnionMember((PMSG_KICKOUT_UNIONMEMBER_REQ *)aRecv, aIndex);
							break;
					}
				}
				break;
			case 0x71:
				//GCPingSendRecv((PMSG_PING_RESULT *)aRecv, aIndex);
				break;
			case 0x72:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					GCPacketCheckSumRecv((PMSG_PACKETCHECKSUM *)aRecv, aIndex);
				}
				break;
			case 0x73:
				if ( DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE )
				{
					GCNPggCheckSumRecv((PMSG_NPROTECTGGCHECKSUM *)aRecv, aIndex);
				}
				break;
			case 0x81:
				CGWarehouseMoneyInOut(aIndex, (PMSG_WAREHOUSEMONEYINOUT *)aRecv);
				break;
			case 0x82:
				CGWarehouseUseEnd(aIndex);
				break;
			case 0x83:
				GCWarehouseRecivePassword(aIndex, (PMSG_WAREHOUSEPASSSEND *)aRecv);
				break;
			case 0x86:
				CGChaosBoxItemMixButtonClick((PMSG_CHAOSMIX *)aRecv, aIndex);
				break;
			case 0x87:
				CGChaosBoxUseEnd(aIndex);
				break;
			case 0x8E:
				CGWindowReqMove((PMSG_MOVE_REQUEST*)aRecv,aIndex);
				break;
			case 0x90:
				GCReqmoveDevilSquare((PMSG_REQ_MOVEDEVILSQUARE *)aRecv, aIndex);
				break;
			case 0x91:
				GCReqDevilSquareRemainTime((PMSG_REQ_DEVILSQUARE_REMAINTIME *)aRecv, aIndex);
				break;
			case 0x95:
				GCRegEventChipRecv((PMSG_REGEVENTCHIP *)aRecv, aIndex);
				break;
			case 0x96:
				GCGetMutoNumRecv((PMSG_GETMUTONUMBER *)aRecv, aIndex);
				break;
			case 0x97:
				GCUseEndEventChipRescv(aIndex);
				break;
			case 0x98:
				GCUseRenaChangeZenRecv((PMSG_EXCHANGE_EVENTCHIP *)aRecv, aIndex);
				break;
			case 0x99:
				CGReqMoveOtherServer((PMSG_REQ_MOVE_OTHERSERVER *)aRecv, aIndex);
				break;
			case 0xA0:
				CGRequestQuestInfo(aIndex);
				break;
			case 0xA2:
				CGSetQuestState((PMSG_SETQUEST *)aRecv, aIndex);
				break;
			case 0xA7:
				CGRequestPetItemCommand((PMSG_REQUEST_PET_ITEM_COMMAND *)aRecv, aIndex);
				break;
			case 0xA9:
				CGRequestPetItemInfo((PMSG_REQUEST_PET_ITEMINFO *)aRecv, aIndex);
				break;
#if (ENABLETEST_NEWPVP == 1)
			case 0xAA://1.00.93
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;
					WORD wPacketType = MAKEWORD(lpDef->subcode,0xAA);
					// ----
					switch(wPacketType)
					{
					case MSG_DUEL_INVITE :
						{
							LPPMSG_REQ_DUEL_INVITE lpReq = (LPPMSG_REQ_DUEL_INVITE)aRecv;
							int nRet = OnCGInviteDuel(lpReq, aIndex);
							if( nRet != ENEWPVP::E_NO_ERROR )
							{
								PMSG_ANS_DUEL_INVITE res = {0};
								res.h.c = PMHC_BYTE;
								res.h.size = sizeof(PMSG_ANS_DUEL_INVITE);
								res.h.headcode = 0xAA;
								res.h.subcode = 0x01;
								res.nResult = nRet;
								DataSend(aIndex, (LPBYTE)&res, res.h.size);
							}
						}
						break;
					case MSG_DUEL_ANSWER :
						{
							LPPMSG_ANS_DUEL_ANSWER lpReq = (LPPMSG_ANS_DUEL_ANSWER)aRecv;
							int nRet = OnCGAnswerDuel(lpReq, aIndex);
							/*if( nRet != ENEWPVP::E_NO_ERROR )
							{
							}*/
						}
						break;
					case MSG_DUEL_LEAVE :
						{
							LPPMSG_REQ_DUEL_EXIT lpReq = (LPPMSG_REQ_DUEL_EXIT)aRecv;
							int nRet = OnCGLeaveDuel(lpReq, aIndex);
							if( nRet != ENEWPVP::E_NO_ERROR )
							{
								PMSG_ANS_DUEL_EXIT res = {0};
								res.h.c = PMHC_BYTE;
								res.h.size = sizeof(PMSG_ANS_DUEL_EXIT);
								res.h.headcode = 0xAA;
								res.h.subcode = 0x03;
								res.nResult = nRet;
								DataSend(aIndex, (LPBYTE)&res, res.h.size);
							}
						}
						break;
					case MSG_DUEL_JOINCNANNEL :
						{
							int nRet = OnDuelChannelJoin((LPPMSG_REQ_DUEL_JOINCNANNEL)aRecv, aIndex);
							if( nRet != ENEWPVP::E_NO_ERROR )
							{
								PMSG_ANS_DUEL_JOINCNANNEL res = {0};
								res.h.c = PMHC_BYTE;
								res.h.size = sizeof(PMSG_ANS_DUEL_JOINCNANNEL);
								res.h.headcode = 0xAA;
								res.h.subcode = 0x07;
								res.nResult = nRet;
								DataSend(aIndex, (LPBYTE)&res, res.h.size);
							}
						}
						break;
					case MSG_DUEL_LEAVECNANNEL :
						{
							LPPMSG_REQ_DUEL_LEAVECNANNEL lpReq = (LPPMSG_REQ_DUEL_LEAVECNANNEL)aRecv;
							int nRet = OnDuelChannelLeave(lpReq, aIndex);
							if( nRet != ENEWPVP::E_NO_ERROR )
							{
								PMSG_ANS_DUEL_LEAVECNANNEL res = {0};
								res.h.c = PMHC_BYTE;
								res.h.size = sizeof(PMSG_ANS_DUEL_LEAVECNANNEL);
								res.h.headcode = 0xAA;
								res.h.subcode = 0x09;
								res.nResult = nRet;
								DataSend(aIndex, (LPBYTE)&res, res.h.size);
							}
						}
						break;
					}
				}
				break;
#else
			case 0xAA:
				{
					//g_DuelManager.ProtocolCore(lpObj, aRecv);
				}
				break;
#endif
			/*case 0xAA:
				CGDuelStartRequestRecv((PMSG_REQ_START_DUEL *)aRecv, aIndex);
				break;
			case 0xAB:
				CGDuelEndRequestRecv((PMSG_REQ_END_DUEL *)aRecv, aIndex);
				break;
			case 0xAC:
				CGDuelOkRequestRecv((PMSG_ANS_DUEL_OK *)aRecv, aIndex);
				break;*/
			case 0xAE:
				CGSaveMuBotSettings((PMSG_MUBOT_SETTINGS_RECV*)aRecv,aIndex);
				break;
			case 0x9A:
				CGRequestEnterBloodCastle((PMSG_REQ_MOVEBLOODCASTLE*)aRecv, aIndex);
				break;
			case 0x9B:
				//
				break;
			case 0x9F:
				CGRequestEventEnterCount((PMSG_REQ_CL_ENTERCOUNT *)aRecv, aIndex);
				break;
			case 0x9D:
				CGRequestLottoRegister((PMSG_REQ_2ANV_LOTTO_EVENT *)aRecv, aIndex);
				break;
			case 0xAF:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x01:
							CGRequestEnterChaosCastle((PMSG_REQ_MOVECHAOSCASTLE *)aRecv, aIndex);
							break;
						case 0x02:
							CGRequestRepositionUserInChaosCastle((PMSG_REQ_REPOSUSER_IN_CC *)aRecv, aIndex);
							break;
					}
				}
				break;
			case 0xC0:
				FriendListRequest(aIndex);
				break;
			case 0xC1:
				FriendAddRequest((PMSG_FRIEND_ADD_REQ *)aRecv, aIndex);
				break;
			case 0xC2:
				WaitFriendAddRequest((PMSG_FRIEND_ADD_SIN_RESULT *)aRecv, aIndex);
				break;
			case 0xC3:
				FriendDelRequest((PMSG_FRIEND_DEL_REQ *)aRecv, aIndex);
				break;
			case 0xC4:
				FriendStateClientRecv((PMSG_FRIEND_STATE_C *)aRecv, aIndex);
				break;
			case 0xC5:
				FriendMemoSend((PMSG_FRIEND_MEMO *)aRecv, aIndex);
				break;
			case 0xC7:
				FriendMemoReadReq((PMSG_FRIEND_READ_MEMO_REQ *)aRecv, aIndex);
				break;
			case 0xC8:
				FriendMemoDelReq((PMSG_FRIEND_MEMO_DEL_REQ *)aRecv, aIndex);
				break;
			case 0xC9:
				FriendMemoListReq(aIndex);
				break;
			case 0xCA:
				FriendChatRoomCreateReq((PMSG_FRIEND_ROOMCREATE_REQ *)aRecv, aIndex);
				break;
			case 0xCB:
				FriendRoomInvitationReq((PMSG_ROOM_INVITATION *)aRecv, aIndex);
				break;
			case 0xD0:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x07:
							GCReqEnterBarracks(aIndex);
							break;
						case 0x08:
							GCReqEnterBalgassLair(aIndex);
							break;
					}
				}
				break;
			case 0xD1:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
						case 0x00:
							CGReqKanturuStateInfo((PMSG_REQ_KANTURU_STATE_INFO *)aRecv, aIndex);
							break;
						case 0x01:
							GCReqEnterKanturuBossMap((PMSG_REQ_ENTER_KANTURU_BOSS_MAP *)aRecv, aIndex);
							break;
					}
				}
				break;
			case 0xD2:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch(lpDef->subcode)
					{
					case 0x01:
						g_CashShop.CGCashPoint(lpObj);
						break;
					case 0x02:
						g_CashShop.CGCashShopOpen(lpObj,(PMSG_REQ_CASHSHOPOPEN*)aRecv);
						break;
					case 0x03:
						g_CashShop.CGCashItemBuy(lpObj,(PMSG_REQ_CASHITEM_BUY*)aRecv);
						break;
					case 0x04:
						g_CashShop.CGCashGiftSend(lpObj,(PMSG_REQ_CASHITEM_GIFT*)aRecv);
						break;
					case 0x05:
						g_CashShop.CGReqInfo(aRecv,lpObj->m_Index);
						break;
					case 0x0B:
						g_CashShop.CGReqUseItem(lpObj,(PSMG_REQ_CASHITEM_USE*)aRecv);
						break;
					}
				}
				break;
#if (__QUEST_SYSTEM__== 1)

			case 0xF6:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch ( lpDef->subcode )
					{
					case 0x0A:
						gQuestInfoEx.CGRequestInitQuest(aRecv,aIndex);
						break;
					case 0x0B:
						gQuestInfoEx.CGRequestQuestStep(aRecv,aIndex);
						break;
					case 0x0D:
						gQuestInfoEx.CGQuestComplete(aRecv,aIndex);
						break;
					case 0x0F:
						gQuestInfoEx.CGQuitQuest(aRecv,aIndex);
						break;
					case 0x10:
						gQuestInfoEx.CGClientAction(aRecv,aIndex);
						break;
					case 0x1A:
						gQuestInfoEx.CGRequestQuestState(aIndex);
						break;
					case 0x1B:
						gQuestInfoEx.CGReqQuestInfo(aRecv,aIndex);
						break;
					case 0x21:
						break;
					case 0x30:
						gQuestInfoEx.GCSendAvaibleQuest(aIndex);
						break;
					case 0x31:
						gQuestInfoEx.GCSendBuff(aIndex);
						break;
					}
				}
				break;
#endif
			case 0xF7:
				{
					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch( lpDef->subcode )
					{
					case 0x01:
						CGReqEnterZone((PMSG_REQ_ENTERZONE*)aRecv,aIndex);
						break;
					}
				}
				break;
			case 0xF8:
				{

					//LogAddHeadHexFunc(1,(char*)aRecv,aLen);

					PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

					switch(lpDef->subcode)
					{
#if (__GENS_SYSTEM__ == 1)
					case 0x01:
						gGensSystem.CGJoinRequest(aRecv,aIndex);
						break;
					case 0x03:
						gGensSystem.CGLeaveRequest(aIndex);
						break;
					case 0x09:
						gGensSystem.CGRequestPrize(aRecv,aIndex);
						break;
					case 0x0B:
						gGensSystem.SendGensInfo(aIndex);
						break;
#endif
					case 0x20:
						CGReqMoveAcheron(aIndex);
						//CGReqAcheronEnter((_tagPMSG_REQ_ACHERON_ENTER *)aRecv, aIndex);
						break;
#if (ENABLETEST_ARCA == 1)
					case 0x30:
						//CGReqArcaBattleGuildMasterJoin((_tagPMSG_REQ_ARCA_BATTLE_JOIN *)aRecv, aIndex);
						break;
					case 0x32:
						//CGReqArcaBattleGuildMemberJoin((_tagPMSG_REQ_ARCA_BATTLE_JOIN *)aRecv, aIndex);
						break;
					case 0x34:
						//CGReqArcaBattleEnter((_tagPMSG_REQ_ARCA_BATTLE_ENTER *)aRecv, aIndex);
						break;
					case 0x36:
						//CGReqArcaBattleBootyExchange(aIndex);
						break;
					case 0x3C:
						//CGReqSpritemapExchange(aIndex);
						break;
					case 0x41:
						//CGReqRegisteredMemberCnt(aIndex);
						break;
					case 0x43:
						//g_ArcaBattle.CGReqMarkReg(aIndex);
						break;
					case 0x45:
						//g_ArcaBattle.CGReqMarkRank(aIndex);
						break;
#endif
#if (ENABLETEST_ACHERON == 1)
					case 0x4B:
						g_AcheronGuardianEvent.CGReqAcheronEventEnter((_tagPMSG_REQ_ACHERON_EVENT_ENTER*)aRecv, aIndex);
						break;
#endif
					}
				}
				break;
#if (ENABLETEST_MUUN == 1)
			case 0xF1:
				g_CMuunSystem.DGLoadMuunInvenItem((_tagSDHP_ANS_DBMUUN_INVEN_LOAD *)aRecv);
				break;
#endif
			case 0xFC:
				{
					
				}
				break;


			default:
				LogAddC(2, "error-L2 : account:%s name:%s HEAD:%x (%s,%d) State:%d",
					gObj[aIndex].AccountID, gObj[aIndex].Name, protoNum, __FILE__, __LINE__, gObj[aIndex].Connected);
				CloseClient(aIndex);


		}	
	}
}






void TestSend()
{
	PMSG_TEST pMsg;
	int size = sizeof(pMsg);
	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0xFF;	// Test Packet
	pMsg.h.sizeH = SET_NUMBERH(size);
	pMsg.h.sizeL = SET_NUMBERL(size);
	DataSend( tempindex, (LPBYTE)&pMsg, size);
	DataSend( tempindex, (LPBYTE)&pMsg, size);
	LogAdd("TEST Data Send");
}








void MsgSendV2(LPOBJ lpObj, unsigned char* Msg, int size)
{
	for ( int n=0;n<MAX_VIEWPORT;n++)
	{
		if ( lpObj->VpPlayer2[n].type == OBJ_USER )
		{
			if ( lpObj->VpPlayer2[n].state != 0 )
			{
				DataSend(lpObj->VpPlayer2[n].number, Msg, size);
			}
		}
	}
}



void CGLiveClient(PMSG_CLIENTTIME * lpClientTime, short aIndex)
{
VM_START

	if ( gObj[aIndex].Connected > PLAYER_CONNECTED )
	{
		gObj[aIndex].ConnectCheckTime = GetTickCount();
	}

	if ( gObj[aIndex].Connected == PLAYER_PLAYING )
	{
		if ( gObj[aIndex].m_AttackSpeed < lpClientTime->AttackSpeed || gObj[aIndex].m_MagicSpeed < lpClientTime->MagicSpeed )
		{
			LogAddC(2, "[%s][%s][%s] ClientHack Detected : Editing AttackSpeed [%d][%d] [%d][%d]", gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].Ip_addr, gObj[aIndex].m_AttackSpeed, lpClientTime->AttackSpeed, gObj[aIndex].m_MagicSpeed, lpClientTime->MagicSpeed);
			gObj[aIndex].m_AttackSpeedHackDetectedCount++;

			if ( gObj[aIndex].m_AttackSpeedHackDetectedCount > 3 )
			{
				CloseClient(aIndex);
			}
		}
		else
		{
			gObj[aIndex].m_AttackSpeedHackDetectedCount = 0;
		}
	}
VM_END
}



struct PMSG_CHECK_MAINEXE
{
	PBMSG_HEAD h;	// C1:03
	WORD m_wKey;	// 4
};

void GCCheckMainExeKeySend(int aIndex)
{
	PMSG_CHECK_MAINEXE pMsg;

	gObj[aIndex].CheckSumTableNum = rand() % MAX_CHECKSUM_KEY;
	gObj[aIndex].CheckSumTime = GetTickCount();
	PHeadSetB((LPBYTE)&pMsg, 0x03, sizeof(pMsg));
	pMsg.m_wKey = EncryptCheckSumKey(gObj[aIndex].CheckSumTableNum);

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}







WORD EncryptCheckSumKey(WORD wSource)
{
	WORD wRandom = rand() % 64;
	WORD wAcc = ((wSource & 0x3F0) * 64) | (wRandom *16) | (wSource & 0x0F);
	return wAcc ^ 0xB479;
}






void CGCheckMainRecv(PMSG_CHECK_MAINEXE_RESULT * lpMsg, int aIndex)
{
	/*if ( strcmp(gObj[aIndex].Ip_addr, "89.28.120.69") == 0 )	// #warning To Prevent HAckrs
	{
		gObj[aIndex].CheckSumTime = 0;
		gObj[aIndex].m_InWebzen = true;
		return;
	}*/

	gObj[aIndex].CheckSumTime = 0;
	return;

	if ( gObj[aIndex].CheckSumTableNum < 0 || gObj[aIndex].CheckSumTableNum > MAX_CHECKSUM_KEY-1 )
	{
		LogAdd("error-L1 : [%s][%s] CheckSum-Exe error ", gObj[aIndex].AccountID, gObj[aIndex].Name);
		CloseClient(aIndex);
		return;
	}

	if ( dwgCheckSum[gObj[aIndex].CheckSumTableNum] != lpMsg->m_dwKey)
	{
		LogAddTD("error-L1 : CheckSum-Exe error %d %d %d [%s]", dwgCheckSum[gObj[aIndex].CheckSumTableNum],
			lpMsg->m_dwKey, gObj[aIndex].CheckSumTableNum, gObj[aIndex].AccountID);
		CloseClient(aIndex);
		return;
	}

	gObj[aIndex].CheckSumTime = 0;
}







void PEchoProc(unsigned char * aMsg, int aLen, short aIndex)
{
	for ( int n = 0 ; n< OBJMAX ; n++)
	{
		if ( gObj[n].Connected >= PLAYER_CONNECTED )
		{
			DataSend(n, aMsg, aLen);
		}
	}
}










void GCResultSend(int aIndex, BYTE headcode, BYTE result)
{
	PMSG_DEFRESULT pResult;

	PHeadSetB((LPBYTE)&pResult, headcode, sizeof(pResult));
	pResult.result = result;
	
	DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size);
}




void ChatSend(LPOBJ lpObj, char* szChat)
{
	int len = strlen(szChat);

	if ( len < 1 )
		return;

	if ( len > MAX_CHAT_LEN-1 )
		return;

/*	char szTempMsg[4096] = {0};
	MultiByteToWideChar(1258, 0, szChat, len, (unsigned short *)szTempMsg, sizeof(szTempMsg));
	memcpy(&pMsg.szChatMsg, szTempMsg, sizeof(pMsg.szChatMsg));
	pMsg.szChatMsg[MAX_CHAT_LEN-2]=0;
	pMsg.szChatMsg[MAX_CHAT_LEN-1]=0;
	PHeadSetB((LPBYTE)&pMsg, 0x01, wcslen((unsigned short*)pMsg.szChatMsg)*2 + (sizeof(GC_CHATSEND)-sizeof(pMsg.szChatMsg)));
	pMsg.sIndex[0] = lpObj->m_Index >> 8;
	pMsg.sIndex[1] = lpObj->m_Index & 0xFF;*/

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x01, len+6);
	strcpy(pMsg.chatmsg, szChat);
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);

	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}





void vChatSend(LPOBJ lpObj, char* szChat, ...)
{
	char szBuffer[256] = "";
	va_list pArguments;

	va_start(pArguments, szChat);
	vsprintf(szBuffer, szChat, pArguments);
	va_end(pArguments);

	int len = strlen(szBuffer);

	if ( len < 1 )
		return;

	if ( len > MAX_CHAT_LEN-1 )
		return;

/*	char szTempMsg[4096] = {0};
	MultiByteToWideChar(1258, 0, szChat, len, (unsigned short*)szTempMsg, sizeof(szTempMsg));
	memcpy(&pMsg.szChatMsg, szTempMsg, sizeof(pMsg.szChatMsg));
	pMsg.szChatMsg[MAX_CHAT_LEN-2]=0;
	pMsg.szChatMsg[MAX_CHAT_LEN-1]=0;
	PHeadSetB((LPBYTE)&pMsg, 0x01, wcslen((unsigned short*)pMsg.szChatMsg)*2 + (sizeof(GC_CHATSEND)-sizeof(pMsg.szChatMsg)));
	pMsg.sIndex[0] = lpObj->m_Index >> 8;
	pMsg.sIndex[1] = lpObj->m_Index & 0xFF;*/

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x01, len+6);
	strcpy(pMsg.chatmsg, szBuffer);
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);

	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}







void AllSendServerMsg( char* chatmsg)
{
	PMSG_NOTICE pNotice;
	
	TNotice::MakeNoticeMsg((TNotice*)&pNotice, 0x00,  chatmsg);

	for ( int n = OBJ_STARTUSERINDEX ; n < OBJMAX ; n++)
	{
		if ( gObj[n].Connected == PLAYER_PLAYING )
		{
			if ( gObj[n].Type == OBJ_USER )
			{
				DataSend(n, (unsigned char*)&pNotice , pNotice.h.size );
			}
		}
	}
}





void DataSendAll(unsigned char* lpMsg, int iMsgSize)
{			
	for ( int n = OBJ_STARTUSERINDEX ; n < OBJMAX ; n++)
	{
		if ( gObj[n].Connected == PLAYER_PLAYING )
		{
			if ( gObj[n].Type == OBJ_USER )
			{
				DataSend(n, (unsigned char*)lpMsg , iMsgSize );
			}
		}
	}
}






void ChatTargetSend(LPOBJ lpObj, char * szChat, int senduser)
{
	int len = strlen(szChat);

	if ( len < 1 || len > MAX_CHAT_LEN-1)
		return;

	/*char szTempMsg[4096] = {0};
	MultiByteToWideChar(1258, 0, szChat, len, (unsigned short*)szTempMsg, sizeof(szTempMsg));
	memcpy(&pMsg.szChatMsg, szTempMsg, sizeof(pMsg.szChatMsg));
	pMsg.szChatMsg[MAX_CHAT_LEN-2]=0;
	pMsg.szChatMsg[MAX_CHAT_LEN-1]=0;
	PHeadSetB((LPBYTE)&pMsg, 0x01, wcslen((unsigned short*)pMsg.szChatMsg)*2 + (sizeof(GC_CHATSEND)-sizeof(pMsg.szChatMsg)));
	pMsg.sIndex[0] = lpObj->m_Index >> 8;
	pMsg.sIndex[1] = lpObj->m_Index & 0xFF;*/

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x01, len+6);
	strcpy(pMsg.chatmsg, szChat);
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);
	pMsg.chatmsg[len+1] = 0;

	DataSend(senduser, (UCHAR*)&pMsg, pMsg.h.size);
}

#include "CommandManager.h"

void PChatProc(PMSG_CHATDATA * lpChat, short aIndex)
{

	int n;
	LPOBJ lpObj = &gObj[aIndex];
	int number;
	int slen = strlen(lpChat->chatmsg);

	if ( slen < 1 )
		return;

	if ( slen > MAX_CHAT_LEN-1 )
	{
		LogAddTD("[Anti-HACK][PChatProc][%s][%s] Chat Message Len : %d", lpObj->AccountID, lpObj->Name, slen);
		return;
	}

	char szId[MAX_ACCOUNT_LEN+1];
	szId[MAX_ACCOUNT_LEN] = 0;

	memcpy(szId, gObj[aIndex].Name, MAX_ACCOUNT_LEN);
	memcpy(lpChat->chatid, szId, MAX_ACCOUNT_LEN);
	int szTargetNameCount = 0;

	if (!ZTHelper::isValidChatSyntax(lpChat->chatmsg)) {
		MsgOutput(aIndex, "Incorrect syntax");
		return;
	}
	
	switch ( lpChat->chatmsg[0] )
	{
		case '!':	// Global Announcement
			if ( slen > 2 )
			{
				if (CheckAuthority(34,lpObj) == 1) {
					/*MSG_SYS_BROADCAST	res = { 0 };
					res.h.set((LPBYTE)&res, 0xCD, sizeof(res));
					res.btSize = lpChat->h.size;
					CopyMemory(res.szMessage, lpChat->chatmsg, lpChat->h.size);
					cDBSMng.Send((char*)&res, sizeof(MSG_SYS_BROADCAST));
					AllSendServerMsg(&lpChat->chatmsg[1]);*/
					
					BroadCastMessageInfo lpRequest = { 0 };
					lpRequest.h.set((LPBYTE)&lpRequest, 0xCD, sizeof(lpRequest));
					lpRequest.Type = 0;
					CopyMemory(lpRequest.Text, lpChat->chatmsg, lpChat->h.size);
					DataSend(aIndex, (LPBYTE)lpChat, lpChat->h.size);
					wsJServerCli.DataSend((PCHAR)&lpRequest, sizeof(BroadCastMessageInfo));
					LogAddTD(lMsg.Get(MSGGET(1, 215)), gObj[aIndex].AccountID, gObj[aIndex].Name, &lpChat->chatmsg[1]);
					return;
				} else { // post
					char tmpCommandText[200] = { 0 };
					sprintf_s(tmpCommandText, sizeof(tmpCommandText), "/post %s", &lpChat->chatmsg[1]);
					g_CommandManager.Run(lpObj, tmpCommandText);
					return;
				}
			}
			break;
		case '/':	// Command
			if ( slen > 2 )
			{
				g_CommandManager.Run(lpObj, lpChat->chatmsg);
				return;
			}
			break;
	}

	// Other Commands
	if ( lpObj->ChatLimitTime > 0 )
	{
		MsgOutput(lpObj->m_Index, lMsg.Get(MSGGET(4, 223)), lpObj->ChatLimitTime);
		return;
	}

	if ( (lpObj->Penalty&2)==2)
		return;

	// Party Message
	if ( lpChat->chatmsg[0] == '~' || lpChat->chatmsg[0] == ']' )
	{
		if ( lpObj->PartyNumber >= 0 )
		{
			int partycount = gParty.GetPartyCount(lpObj->PartyNumber);

			if ( partycount >= 0 )
			{
				for (n=0;n<MAX_USER_IN_PARTY;n++ )
				{
					number = gParty.m_PartyS[lpObj->PartyNumber].Number[n];

					if ( number >= 0 )
					{
						DataSend(number, (LPBYTE)lpChat, lpChat->h.size);
					}
				}
			}
		}
	}
	// Guild
	else if ( lpChat->chatmsg[0] == '@' )
	{
		if ( lpObj->GuildNumber > 0 )
		{
			// Notice
			if ( lpChat->chatmsg[1] == '>' )
			{
				if ( lpObj->Name[0] == lpObj->lpGuild->Names[0][0] )
				{
					if ( !strcmp(lpObj->Name, lpObj->lpGuild->Names[0] ) )
					{
						GDGuildNoticeSave(lpObj->lpGuild->Name, &lpChat->chatmsg[2]);
						LogAdd(lMsg.Get(MSGGET(1, 216)), lpObj->lpGuild->Name, lpChat->chatmsg);
					}
				}
			}
			// Aliances
			else if ( lpChat->chatmsg[1] == '@' && lpObj->lpGuild->iGuildUnion )
			{
				int iGuildCount = 0;
				int iGuildList[MAX_UNION_GUILD] ={0};

				if ( UnionManager.GetGuildUnionMemberList(lpObj->lpGuild->iGuildUnion, iGuildCount, iGuildList) == TRUE )
				{
					for ( int i=0;i<iGuildCount;i++)
					{
						_GUILD_INFO_STRUCT *lpGuildInfo = Guild.SearchGuild_Number(iGuildList[i]);

						if ( !lpGuildInfo )
							continue;

						for (n=0;n<MAX_USER_GUILD;n++)
						{
							if ( lpGuildInfo->Use[n] )
							{
								number = lpGuildInfo->Index[n];

								if ( number >= 0 )
								{
									if ( lpGuildInfo->Names[n][0] == gObj[number].Name[0] )
									{
										if ( !strcmp(lpGuildInfo->Names[n], gObj[number].Name ))
										{
											DataSend(number, (LPBYTE)lpChat, lpChat->h.size);
										}
									}
								}
							}
						}
					}
				}

				if ( g_iServerGroupUnionChatting == TRUE )
					GDUnionServerGroupChattingSend(lpObj->lpGuild->iGuildUnion, lpChat);
			}
			// Just Guild
			else
			{
				int count = lpObj->lpGuild->Count;

				if ( count >= 0 )
				{
					for ( n=0;n<MAX_USER_GUILD;n++)
					{
						if ( lpObj->lpGuild->Use[n] )
							{
								number = lpObj->lpGuild->Index[n];

								if ( number >= 0 )
								{
									if ( lpObj->lpGuild->Names[n][0] == gObj[number].Name[0] )
									{
										if ( !strcmp(lpObj->lpGuild->Names[n], gObj[number].Name ) )
										{
											DataSend(number, (LPBYTE)lpChat, lpChat->h.size);
										}
									}
								}
							}
						}
				}

				if ( g_iServerGroupGuildChatting == TRUE )
				{
					if ( lpObj->lpGuild->Count > 1 )
					{
						GDGuildServerGroupChattingSend(lpObj->lpGuild->Number, lpChat);
					}
				}
			}
		}
	}
#if (CUSTOM_OLYMP==1)
	else if( lpChat->chatmsg[0] == '$' )
	{
		if( gObjCheckUsedBuffEffect(lpObj,BUFF_GLADIATOR_HONOR) )
		{
			for(int  n = OBJ_STARTUSERINDEX; n < OBJMAX; n++)
			{
				if( gObj[n].Connected < 3 )
					continue;

				DataSend(n, (LPBYTE)lpChat, lpChat->h.size);
			}

			if ( gWriteChatLog )
			{
				memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[1], MAX_CHAT_LEN-1);
				pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
				pChatMsg.btType = 0x06;
			}
		}
		else
		{
			MsgOutput(lpObj->m_Index,"Only heroes can use this !");
		}
	}
#endif
#if(__GENS_SYSTEM__ == 1)
	else if( lpChat->chatmsg[0] == '$' )
	{
		if( lpObj->Money < gGensSystem.GetChatPrice() )
		{
			MsgOutput(lpObj->m_Index, "Need more zen for gens chat");
			return;
		}

		lpObj->Money -= gGensSystem.GetChatPrice();
		GCMoneySend(lpObj->m_Index, lpObj->Money);

		for(int  n = OBJ_STARTUSERINDEX; n < OBJMAX; n++)
		{
			if( gObj[n].Connected < 3 )
				continue;

			if( gObj[n].m_GensInfo.btFamily == lpObj->m_GensInfo.btFamily )
			{
				DataSend(n, (LPBYTE)lpChat, lpChat->h.size);
			}
		}
	}
#else
#if (CUSTOM_OLYMP==0)
	else if( lpChat->chatmsg[0] == '$' )
	{
		MsgOutput(lpObj->m_Index,"Gens system is disabled!");
	}
#endif
#endif
	else
	{
#if (__CUSTOM__ == 1)
		DataSend(aIndex, (LPBYTE)lpChat, lpChat->h.size);

		if( lpObj->MapNumber == MAP_INDEX_PKFIELD && lpObj->m_iDuelUser == -1 )
		{
			for ( int n=0;n<MAX_VIEWPORT;n++)
			{
				if ( lpObj->VpPlayer2[n].type == OBJ_USER )
				{
					if ( lpObj->VpPlayer2[n].state != 0 )
					{
						int number = lpObj->VpPlayer2[n].number;
	
						if( OBJMAX_RANGE(number) )
						{
							if( gObj[number].m_iDuelUser != -1 )
							{
								continue;
							}
	
							DataSend(number, (LPBYTE)lpChat, lpChat->h.size);
						}
					}
				}
			}
		}
		else
		{
			MsgSendV2(lpObj,(LPBYTE)lpChat,lpChat->h.size);
		}
#else
		DataSend(aIndex, (LPBYTE)lpChat, lpChat->h.size);
		MsgSendV2(lpObj, (LPBYTE)lpChat, lpChat->h.size);
#endif
	}
}							
			


void CGChatRecv(PMSG_CHATDATA_NUMBER * lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( PacketCheckTime(lpObj) == FALSE )
	{
		return;
	}

	DataSend(aIndex, (LPBYTE)lpMsg, lpMsg->h.size);
	MsgSendV2(lpObj, (LPBYTE)lpMsg, lpMsg->h.size);
}





struct PMSG_SERVERMSG
{
	PBMSG_HEAD h;	// C1:0C
	BYTE MsgNumber;	// 3
};


void GCServerMsgSend(BYTE msg, int aIndex)
{
	PMSG_SERVERMSG pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x0C, sizeof(pMsg));
	pMsg.MsgNumber = msg;
	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}






void GCServerMsgStringSend(LPSTR  szMsg, int aIndex, BYTE type) 
{
	PMSG_NOTICE pNotice;
	
	TNotice::MakeNoticeMsg((TNotice*)&pNotice, type, szMsg);
	DataSend(aIndex, (UCHAR*)&pNotice, pNotice.h.size);
}





void GCServerMsgStringSendGuild(_GUILD_INFO_STRUCT* lpNode, LPSTR szMsg, BYTE type)
{
	if ( lpNode == NULL )
	{
		return;
	}

	for ( int n = 0; n<MAX_USER_GUILD ; n++)
	{
		if (lpNode->Use[n] > 0 && lpNode->Index[n] >= 0 )
		{
			GCServerMsgStringSend((char*)szMsg, lpNode->Index[n], type);
		}
	}
}




struct PMSG_EVENT
{
	PBMSG_HEAD h;	// C1:0B
	BYTE State;	// 3
	BYTE Event;	// 4
};


void GCEventStateSend(int aIndex, BYTE state, BYTE event)
{
	PMSG_EVENT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x0B, sizeof(pMsg));
	pMsg.Event = event;
	pMsg.State = state;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}





void GCMapEventStateSend(int map, BYTE state, BYTE event)
{
	PMSG_EVENT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x0B, sizeof(pMsg));
	pMsg.Event = event;
	pMsg.State = state;

	for ( int n=0; n<OBJMAX ; n++ )
	{
		if ( gObj[n].Connected == PLAYER_PLAYING && gObj[n].Type == OBJ_USER && map == gObj[n].MapNumber )
		{
			DataSend(n, (UCHAR*)&pMsg, pMsg.h.size);
		}
	}
}




void CGChatWhisperRecv(PMSG_CHATDATA_WHISPER* lpMsg, int aIndex)
{
	char tid[11];
	PMSG_CHATDATA_WHISPER pWhisper;
	int len;
	LPOBJ lpObj = &gObj[aIndex];
	int index;

	if ( PacketCheckTime(lpObj) == FALSE )
	{
		return;
	}

	if ( lpObj->ChatLimitTime > 0 )
	{
		MsgOutput(lpObj->m_Index, lMsg.Get(MSGGET(4, 223)), lpObj->ChatLimitTime);
		return;
	}

	if ( (lpObj->Penalty & 2) == 2 )
	{
		return;
	}

	tid[10]=0;
	memcpy(tid, lpMsg->id, sizeof(lpMsg->id));
	index = WhisperCash.AddCash(tid);

	if ( index < 0 )
	{
		GCServerMsgSend(0, aIndex);
		return;
	}

	PHeadSetB((LPBYTE)&pWhisper, 0x02, sizeof(pWhisper));
	strcpy(pWhisper.id, gObj[aIndex].Name);
	memcpy(pWhisper.chatmsg, lpMsg->chatmsg, sizeof(lpMsg->chatmsg));
	len = strlen(pWhisper.chatmsg);

	if ( len > 0 )
	{
		pWhisper.h.size -= sizeof(pWhisper.chatmsg);
		pWhisper.h.size += len + 1;
		DataSend(index, (UCHAR*)&pWhisper, pWhisper.h.size);
	}
}





struct PMSG_JOINRESULT
{
	PBMSG_HEAD h;	// C1:F1
	BYTE scode;	// 3
	BYTE result;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
	BYTE CliVersion[5];	// 7
};



void SCPJoinResultSend(int aIndex, BYTE result)
{
	PMSG_JOINRESULT pResult;

	memset(&pResult, 0, sizeof(pResult));

	pResult.h.size = sizeof(pResult);
	pResult.h.c = 0xC1;
	pResult.h.headcode = 0xF1;
	pResult.scode = 0x00;
	pResult.result = result;
	pResult.NumberH = SET_NUMBERH(aIndex);
	pResult.NumberL = SET_NUMBERL(aIndex);
	pResult.CliVersion[0] = szClientVersion[0];
	pResult.CliVersion[1] = szClientVersion[1];
	pResult.CliVersion[2] = szClientVersion[2];
	pResult.CliVersion[3] = szClientVersion[3];
	pResult.CliVersion[4] = szClientVersion[4];

	DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
	gObj[aIndex].ConnectCheckTime = GetTickCount();
}


struct SDHP_IDPASS
{
	PBMSG_HEAD h;	// C1:01	[11::LOGIN-TEST]
	char Id[10];	// 3
	char Pass[20];	// D
	short Number;	// 18
	char IpAddress[17];	// 1A
};


void CSPJoinIdPassRequest(PMSG_IDPASS* lpMsg, int aIndex)
{
	char serial[17];
	char id[11];

	if ( lpMsg->CliVersion[0] != szClientVersion[0] ||
		 lpMsg->CliVersion[1] != szClientVersion[1] ||
		 lpMsg->CliVersion[2] != szClientVersion[2] ||
		 lpMsg->CliVersion[3] != szClientVersion[3] ||
		 lpMsg->CliVersion[4] != szClientVersion[4] )
	{
		GCJoinResult(JS_BAD_CLIENT_VERSION, aIndex);
		CloseClient(aIndex);
		return;
	}

	serial[16] = 0;
	memcpy(serial, lpMsg->CliSerial, sizeof(lpMsg->CliSerial));
	id[10]=0;
	memcpy(id, lpMsg->Id, sizeof(lpMsg->Id));
	BuxConvert(id, MAX_ACCOUNT_LEN);

	if ( strcmp(serial, szGameServerExeSerial) != 0 )
	{
		LogAddC(2, "error-L1: Serial error [%s] [%s]", id, serial);
		GCJoinResult(JS_BAD_CLIENT_VERSION, aIndex);
		CloseClient(aIndex);
		return;
	}
	
	if ( g_ConnectMemberLoad == TRUE )
	{
		if ( ConMember.IsMember(id) == FALSE )
		{
			GCJoinResult(JS_ONLY_VIP_MEMBERS, aIndex);
			return;
		}
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (PacketCheckTime(lpObj) == FALSE )
	{
		LogAdd(lMsg.Get(MSGGET(1, 217)), aIndex, id);
		CloseClient(aIndex);
		return;
	}

	if ( gObj[aIndex].Connected != PLAYER_CONNECTED )
	{
		LogAdd(lMsg.Get(MSGGET(1, 218)), aIndex, id);
		CloseClient(aIndex);
		return;
	}

	if ( gObj[aIndex].LoginMsgSnd != FALSE )
	{
		if ( gObj[aIndex].LoginMsgSnd == TRUE )
		{
			LogAdd(lMsg.Get(MSGGET(1, 219)), aIndex, id, gObj[aIndex].Ip_addr );
		}
		else
		{
			LogAdd("error : %d %s %d", gObj[aIndex].LoginMsgSnd, __FILE__, __LINE__);
		}

		return;
	}

	SDHP_IDPASS spMsg;

	PHeadSetB((LPBYTE)&spMsg, 0x01, sizeof(spMsg));
	spMsg.Number = aIndex;
	memcpy(spMsg.Id, lpMsg->Id, sizeof(spMsg.Id));
	memcpy(spMsg.Pass, lpMsg->Pass, sizeof(spMsg.Pass));
	strcpy(spMsg.IpAddress, gObj[aIndex].Ip_addr);

	gObj[aIndex].ConnectCheckTime = GetTickCount();

	gObj[aIndex].LoginMsgSnd = 1;
	gObj[aIndex].LoginMsgCount++;
	gObj[aIndex].AccountID[MAX_ACCOUNT_LEN] = 0;
	memcpy(gObj[aIndex].AccountID, id, MAX_ACCOUNT_LEN);
	gObj[aIndex].m_cAccountItemBlock = 0;

	wsJServerCli.DataSend((char*)&spMsg, spMsg.h.size);
	LogAddTD("join send : (%d)%s", aIndex, gObj[aIndex].AccountID);
	lpObj->m_bMapSvrMoveReq = false;
	lpObj->m_sPrevMapSvrCode = -1; 
	lpObj->m_sDestMapNumber = -1;
	lpObj->m_btDestX = 0;
	lpObj->m_btDestY = 0;
}








	
void CSPJoinIdPassRequestTEST(PMSG_IDPASS * lpMsg, int aIndex)
{
	char szId[11];
	char szPass[11];
	LPOBJ lpObj = &gObj[aIndex];
	SDHP_IDPASS spMsg;

	PHeadSetB((LPBYTE)&spMsg, 0x11, sizeof(spMsg));
	spMsg.Number = aIndex;
	wsprintf(szId, "½¸µ¹ÀÌ%d", logincounttest);
	wsprintf(szPass, "m321", rand()%9);
	LogAdd("login send : %s %s", szId, szPass);
	
	BuxConvert(szId, MAX_ACCOUNT_LEN);
	BuxConvert(szPass, MAX_ACCOUNT_LEN);
	memcpy(spMsg.Id, szId, MAX_ACCOUNT_LEN);
	memcpy(spMsg.Pass, szPass, MAX_ACCOUNT_LEN);
	logincounttest++;
	
	if ( (GetTickCount()-ltesttime) > 1000 )
	{
		ltesttime = GetTickCount();
		logincounttest=0;
	}

	wsJServerCli.DataSend((char*)&spMsg, spMsg.h.size);
	spMsg.h.headcode = 0x01;
	wsJServerCli.DataSend((char*)&spMsg, spMsg.h.size);
}





struct SDHP_BILLSEARCH
{
	PBMSG_HEAD h;	// C1:06
	char Id[10];	// 3
	short Number;	// E
};





void GCJoinBillCheckSend(LPSTR AccountId, int aIndex)
{
	SDHP_BILLSEARCH pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x06, sizeof(pMsg));
	memcpy(pMsg.Id, AccountId, sizeof(pMsg.Id));
	pMsg.Number = aIndex;

	wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size);
}






void GCJoinResult(BYTE result, int aIndex)
{
	PMSG_RESULT  pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0xF1, 0x01, sizeof(pResult));
	pResult.result = result;
	DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size);
}




void GCJoinSocketResult(BYTE result, SOCKET Socket)
{
	PMSG_RESULT pResult;

	PHeadSubSetB((LPBYTE)&pResult , 0xF1, 0x01, sizeof(pResult) );
	pResult.result= result;

	wsGServer.DataSocketSend(Socket, (char*)&pResult, pResult.h.size);
}






void CGClientCloseMsg(PMSG_CLIENTCLOSE * lpMsg, int aIndex)
{
	switch ( lpMsg->Flag )
	{
		case 0:	// Close Game
			gObjCloseSet(aIndex, 0);

			if ( aIndex < 0 || aIndex > OBJMAX-1)
			{
				return;
			}

			LogAddTD("[UserSelClose] Go Exit [%s][%s] Map:[%d][%d/%d]",
				gObj[aIndex].AccountID, gObj[aIndex].Name,
				gObj[aIndex].MapNumber, gObj[aIndex].X, gObj[aIndex].Y);
			break;

		case 2:	// ServerList
			gObjCloseSet(aIndex, 2);

			if ( aIndex < 0 || aIndex > OBJMAX-1)
			{
				return;
			}

			LogAddTD("[UserSelClose] Go ServerList [%s][%s] Map:[%d][%d/%d]",
				gObj[aIndex].AccountID, gObj[aIndex].Name,
				gObj[aIndex].MapNumber, gObj[aIndex].X, gObj[aIndex].Y);
			break;

		case 1:	// Select Character
			gObjCloseSet(aIndex, 1);

			if ( aIndex < 0 || aIndex > OBJMAX-1)
			{
				return;
			}

			LogAddTD("[UserSelClose] Go CharSelect [%s][%s] Map:[%d][%d/%d]",
				gObj[aIndex].AccountID, gObj[aIndex].Name,
				gObj[aIndex].MapNumber, gObj[aIndex].X, gObj[aIndex].Y);
			break;
	}
}






void GCCloseMsgSend(int aIndex, BYTE result)
{
	PMSG_RESULT pMsg;

	PHeadSubSetBE((LPBYTE)&pMsg, 0xF1, 0x02, sizeof(pMsg));
	pMsg.result = result;
	
	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}





void CGClientMsg(PMSG_CLIENTMSG* lpMsg, int aIndex)
{
	if ( lpMsg->Flag )
	{
		LogAddC(2,"error-L1 : HackCheck [%s][%s] 0x%x 0x%x",
			gObj[aIndex].AccountID, gObj[aIndex].Name,
			lpMsg->Flag, lpMsg->subFlag);
	}
	else 
	{
		if ( gObj[aIndex].m_ClientHackLogCount < 100 )
		{
			LogAddC(2, "error-L1 : HackCheck [%s][%s] 0x%x 0x%x",
				gObj[aIndex].AccountID, gObj[aIndex].Name,
				lpMsg->Flag, lpMsg->subFlag);
		}

		gObj[aIndex].m_ClientHackLogCount++;

		if ( gObj[aIndex].m_ClientHackLogCount >= 100 )
			gObj[aIndex].m_ClientHackLogCount = 100;
	}
}




struct SDHP_CREATECHAR
{
	PBMSG_HEAD h;	// C1:04
	int UserNumber;	// 4
	int DBNumber;	// 8
	short Number;	// C
	char AccountId[10];	// E
	char Name[10];	// 18
	BYTE ClassSkin;	// 22
};



void CGPCharacterCreate( PMSG_CHARCREATE * lpMsg, int aIndex)
{
	if ( !PacketCheckTime(&gObj[aIndex]))
	{
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if ( gObj[aIndex].Connected < PLAYER_LOGGED )
	{
		LogAdd(lMsg.Get(MSGGET(1, 220)), aIndex, gObj[aIndex].Ip_addr);
		CloseClient(aIndex);

		return;
	}

	if ( gObj[aIndex].Connected == PLAYER_PLAYING )
	{
		LogAddTD("[Anti-HACK][CGPCharacterCreate] Attempted Character Create during GamePlay. [%s][%s]",
			gObj[aIndex].AccountID, gObj[aIndex].Name);
		CloseClient(aIndex);

		return;
	}

	if ( !gCreateCharacter )
	{
		GCServerMsgStringSend("Character creation is disabled!", aIndex, 1);
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}


	if ( lpMsg->ClassSkin == 0x00 ||
		 lpMsg->ClassSkin == 0x10 ||
		 lpMsg->ClassSkin == 0x20 ||
		 lpMsg->ClassSkin == 0x30 ||
		 lpMsg->ClassSkin == 0x40 ||
		 lpMsg->ClassSkin == 0x50 ||
		 lpMsg->ClassSkin == 0x60 )
	{

	}
	else
	{
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	

	if( lpMsg->ClassSkin == 0x30 ) // MG
	{
		if( gObj[aIndex].m_AccountMaxLevel < g_CreateCharLevelMG )
		{
			LogAddC(2, "[%s] Create character error (ReqLevel: %d, AccountLevel: %d)", 
				gObj[aIndex].AccountID, g_CreateCharLevelMG, gObj[aIndex].m_AccountMaxLevel);
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}

		if ( gObj[aIndex].Magumsa == 0 )
		{
			LogAddC(2, "error-L1: Magumsa Character create error [%s]", gObj[aIndex].AccountID);
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}
	}

	if ( lpMsg->ClassSkin == 0x40 ) // DL
	{
		if( gObj[aIndex].m_AccountMaxLevel < g_CreateCharLevelDL )
		{
			LogAddC(2, "[%s] Create character error (ReqLevel: %d, AccountLevel: %d)", 
				gObj[aIndex].AccountID, g_CreateCharLevelDL, gObj[aIndex].m_AccountMaxLevel);
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}

		if ( gObj[aIndex].Magumsa != 2 )
		{
			LogAddC(2, "error-L1: Darklord Character create error [%s]", gObj[aIndex].AccountID);
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}
	}
	
	if ( lpMsg->ClassSkin == 0x50 ) // SUM
	{
		if( gObj[aIndex].m_AccountMaxLevel < g_CreateCharLevelSUM )
		{
			LogAddC(2, "[%s] Create character error (ReqLevel: %d, AccountLevel: %d)", 
				gObj[aIndex].AccountID, g_CreateCharLevelSUM, gObj[aIndex].m_AccountMaxLevel);
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}
	}

	if ( lpMsg->ClassSkin == 0x60 ) // RF
	{
		if( gObj[aIndex].m_AccountMaxLevel < g_CreateCharLevelRF )
		{
			LogAddC(2, "[%s] Create character error (ReqLevel: %d, AccountLevel: %d)", 
				gObj[aIndex].AccountID, g_CreateCharLevelRF, gObj[aIndex].m_AccountMaxLevel);
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}
	}

	if( g_MultilangCharacterName == 0 )
	{
		for(int i = 0; i < strlen(lpMsg->Name); i++)
		{
			if( !isalnum(lpMsg->Name[i]) )	
			{
				PMSG_CHARCREATERESULT pResult = { 0 };
				pResult.h.c = 0xC1;
				pResult.h.size = sizeof(PMSG_CHARCREATERESULT);
				pResult.h.headcode = 0xF3;
				pResult.subcode = 0x01;
				pResult.Result = 2;
				DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
				return;
			}
		}
	}

	SDHP_CREATECHAR sCreate;

	memset(&sCreate, 0, sizeof(sCreate));
	PHeadSetB((LPBYTE)&sCreate, 0x04, sizeof(sCreate));
	sCreate.ClassSkin = lpMsg->ClassSkin;
	sCreate.Number = aIndex;
	memcpy(sCreate.AccountId, gObj[aIndex].AccountID, MAX_ACCOUNT_LEN);
	memcpy(sCreate.Name, lpMsg->Name, MAX_ACCOUNT_LEN);
	char szTemp[20];
	char szTemp2[20];
	szTemp[10]=0;
	szTemp2[10]=0;
	memcpy(szTemp, sCreate.Name, MAX_ACCOUNT_LEN);
	memcpy(szTemp2, sCreate.AccountId, MAX_ACCOUNT_LEN);

	LogAddTD("Character create : %s,%s", szTemp2, szTemp);

	cDBSMng.Send((char*)&sCreate, sCreate.h.size);
}






struct SDHP_CHARDELETE
{
	PBMSG_HEAD h;	// C1:05
	short Number;	// 4
	char AccountID[10];	// 6
	char Name[10];	// 10
	BYTE Guild;	// [0:NoGuild] [1:Master] [2:Member] 1A
	char GuildName[8];	// 1B
};




void CGPCharDel(PMSG_CHARDELETE * lpMsg,int aIndex)
{
	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	if ( gObj[aIndex].Connected < PLAYER_LOGGED )
	{
		CloseClient(aIndex);
		return;
	}

	if ( gObj[aIndex].Connected == PLAYER_PLAYING )
	{
		LogAddTD("[Anti-HACK][CGPCharDel] Attempted Character Delete during GamePlay. [%s][%s]",
			gObj[aIndex].AccountID, gObj[aIndex].Name);
		CloseClient(aIndex);
		return;
	}

	if ( gObj[aIndex].bEnableDelCharacter == FALSE )
	{
		LogAddTD("[Anti-HACK][CGPCharDel] Attempted Character Delete during Prevented Time. [%s][%s]",
			gObj[aIndex].AccountID, gObj[aIndex].Name);
		CloseClient(aIndex);
		return;
	}

	SDHP_CHARDELETE pCDel;
	PMSG_RESULT pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0xF3, 0x02, sizeof(pResult));
	pResult.result = 0;

	if ( gObj[aIndex].m_cAccountItemBlock )
	{
		pResult.result = 3;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( !gGuildDestroy )
	{
		pResult.result = 0;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	char szJoomin[21];
	char szTemp[20];
	char szTemp2[20];

	memset(szJoomin, 0, sizeof(szJoomin));
	memcpy(szJoomin, lpMsg->LastJoominNumber, 20);

#if (__CUSTOM__ == 1)
	if (gc_bPersonalID == TRUE && gObjJoominCheck(aIndex, szJoomin) == FALSE )
#else
	if (gObjJoominCheck(aIndex, szJoomin) == FALSE )
#endif
	{
		pResult.result = 2;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	PHeadSetB((LPBYTE)&pCDel, 0x05, sizeof(pCDel));
	pCDel.Number = aIndex;
	memcpy(pCDel.AccountID, gObj[aIndex].AccountID, sizeof(pCDel.AccountID));
	memcpy(pCDel.Name, lpMsg->Name, sizeof(pCDel.Name));
	pCDel.Guild = 0;

	if ( gObj[aIndex].GuildNumber > 0 && gObj[aIndex].lpGuild != NULL)
	{
		if ( !strcmp(gObj[aIndex].Name, gObj[aIndex].lpGuild->Names[0] ))
		{
			pCDel.Guild = 1;	// Master
		}
		else
		{
			pCDel.Guild = 2;	// Member
		}

		memcpy(pCDel.GuildName, gObj[aIndex].lpGuild->Name, MAX_GUILD_LEN);
	}

	g_CashItemPeriodSystem.GDReqPeriodItemDelete(&gObj[aIndex], lpMsg->Name);
	szTemp[10]=0;
	szTemp2[10]=0;
	memcpy(szTemp, pCDel.Name, MAX_ACCOUNT_LEN);
	memcpy(szTemp2, pCDel.AccountID, MAX_ACCOUNT_LEN);

	LogAddTD("Character delete : %s,%s", szTemp2, szTemp);

	cDBSMng.Send((PCHAR)&pCDel, pCDel.h.size);
	gObj[aIndex].Level = 0;
}





void CGPCharacterMapJoinRequest( PMSG_CHARMAPJOIN * lpMsg, int aIndex)
{
	if ( !PacketCheckTime(&gObj[aIndex]))
	{
		//JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if ( !gObjIsAccontConnect(aIndex, gObj[aIndex].AccountID))
		return;

	if ( gObj[aIndex].Connected == PLAYER_PLAYING )
		return;

	gObj[aIndex].bEnableDelCharacter = FALSE;

	char _name[MAX_ACCOUNT_LEN+1];
	SDHP_DBCHARINFOREQUEST pCRequest;

	PHeadSetB((LPBYTE)&pCRequest, 0x06, sizeof(pCRequest));
	memset(_name, 0, MAX_ACCOUNT_LEN);
	memcpy(_name, lpMsg->Name, MAX_ACCOUNT_LEN);
#if( ENABLE_ZMULTISERVER_UPDATE_3 == 0 )
	BuxConvert(_name, MAX_ACCOUNT_LEN);
#endif
	memcpy(pCRequest.Name, _name, MAX_ACCOUNT_LEN);
	strcpy(pCRequest.AccountID, gObj[aIndex].AccountID);
	pCRequest.Number = aIndex;
	cDBSMng.Send((char*)&pCRequest, pCRequest.h.size);
	
	USER_GET_BATTLEINFO lpInfo;
	PHeadSubSetB((LPBYTE)&lpInfo, 0xFD, 0x01, sizeof(USER_GET_BATTLEINFO));
	// ----
	for(int i = 0; i < 11; i++ )
	{
		lpInfo.BattleMapList[i] = gGensSystem.m_MapList[i];
		lpInfo.BattleZoneList[i] = gGensSystem.m_ZoneList[i];
	}
	// ----
	DataSend(aIndex, (LPBYTE)&lpInfo, lpInfo.h.size);
}



struct PMSG_LEVELUP
{
	PBMSG_HEAD h;	// C1:F3:05
	BYTE subcode;	// 3
	WORD Level;	// 4
	WORD LevelUpPoint;	// 6
	WORD MaxLife;	// 8
	WORD MaxMana;	// A
	WORD wMaxShield;	// C
	WORD MaxBP;	// E
	short AddPoint;	// 10
	short MaxAddPoint;	// 12
	short MinusPoint;	// 14
	short MaxMinusPoint;	// 16

#if(FIX_HP)
	int MAXHP;
	int MAXMP;
	int MAXSD;
#endif
};



void GCLevelUpMsgSend(int aIndex, int iSendEffect)
{
	if ( !OBJMAX_RANGE(aIndex))
		return;

	short AddPoint=0;
	short MaxAddPoint=0;
	short MinusPoint=0;
	short MaxMinusPoint=0;

	gObjGetStatPointState(gObj[aIndex].m_Index, AddPoint, MaxAddPoint, MinusPoint, MaxMinusPoint);
	
	PMSG_LEVELUP pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x05, sizeof(pMsg));
	pMsg.Level = gObj[aIndex].Level;
	pMsg.LevelUpPoint = gObj[aIndex].LevelUpPoint;
	pMsg.MaxLife = (WORD)(gObj[aIndex].MaxLife+gObj[aIndex].AddLife);
	pMsg.MaxMana = (WORD)(gObj[aIndex].MaxMana+gObj[aIndex].AddMana);
	pMsg.MaxBP = gObj[aIndex].MaxBP + gObj[aIndex].AddBP;
	pMsg.wMaxShield = gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield;
	pMsg.AddPoint = AddPoint;
	pMsg.MaxAddPoint = MaxAddPoint;
	pMsg.MinusPoint = MinusPoint;
	pMsg.MaxMinusPoint = MaxMinusPoint;

#if (FIX_HP==1)
	pMsg.MAXHP = (int)(gObj[aIndex].AddLife + gObj[aIndex].MaxLife);
	pMsg.MAXMP = (int)(gObj[aIndex].AddMana + gObj[aIndex].MaxMana);
	pMsg.MAXSD = gObj[aIndex].iAddShield+gObj[aIndex].iMaxShield;
#endif

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

	if ( iSendEffect == 1 )
		GCSendEffectInfo(aIndex, 0x10);
}


void CGLevelUpPointAdd(PMSG_LVPOINTADD * lpMsg, int aIndex)
{
	if ( !gObjIsConnected(aIndex))
	{
		CloseClient(aIndex);
		return;
	}

	PMSG_LVPOINTADDRESULT pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x06, sizeof(pMsg));
	pMsg.ResultType = 0;
	
	if (::gObjLevelUpPointAdd(lpMsg->Type, &gObj[aIndex]) == TRUE )
	{
		pMsg.ResultType = 0x10;
		pMsg.ResultType += lpMsg->Type;

		switch ( lpMsg->Type )
		{
			case 2:	// Vit
				pMsg.MaxLifeAndMana = (WORD)(gObj[aIndex].MaxLife + gObj[aIndex].AddLife);
#if (FIX_HP==1)
				pMsg.MAXHPANDMANA = (int)(gObj[aIndex].MaxLife+gObj[aIndex].AddLife);
#endif
				break;

			case 3:	// Energy
				pMsg.MaxLifeAndMana = (WORD)(gObj[aIndex].MaxMana + gObj[aIndex].AddMana);
#if (FIX_HP==1)
				pMsg.MAXHPANDMANA = (int)(gObj[aIndex].MaxMana+gObj[aIndex].AddMana);
#endif
				break;
		}
		pMsg.wMaxShield = gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield;
		gObjSetBP(aIndex);
		pMsg.MaxBP = gObj[aIndex].MaxBP + gObj[aIndex].AddBP;

#if (FIX_HP==1)
		pMsg.MAXSD = gObj[aIndex].iMaxShield+gObj[aIndex].iAddShield;
#endif
	}

	DataSend(aIndex, (UCHAR *)&pMsg, pMsg.h.size);

}



struct PMSG_INVENTORYITEMMODIFY
{
	PBMSG_HEAD h;	// C1:F3:14
	BYTE subcode;	// 3
	BYTE Pos;	// 4
	BYTE ItemInfo[MAX_ITEM_INFO];	// 5

};


void GCInventoryItemOneSend(int aIndex, int pos)
{
	if ( !gObj[aIndex].pInventory[pos].IsItem())
		return;

	PMSG_INVENTORYITEMMODIFY pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x14, sizeof(pMsg));
	pMsg.Pos = pos;
	ItemByteConvert(pMsg.ItemInfo, gObj[aIndex].pInventory[pos]);

	DataSend(aIndex, (UCHAR *)&pMsg, pMsg.h.size);
}



struct PMSG_PKLEVEL
{
	PBMSG_HEAD h;	// C1:F3:08
	BYTE subcode;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE PkLevel;	// 6
};


void GCPkLevelSend(int aIndex, BYTE pklevel)
{
	PMSG_PKLEVEL pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x08, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);
	pMsg.PkLevel = pklevel;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
	MsgSendV2(&gObj[aIndex], (UCHAR*)&pMsg, pMsg.h.size);
}




#pragma pack(1)
struct PMSG_MAGICLIST
{
	char Pos;	// 0
	WORD m_Skill;
	BYTE m_Level;
};
#pragma pack()


struct PMSG_MAGICLISTCOUNT
{
	PBMSG_HEAD h;	// C1:F3:11
	BYTE subcode;	// 3
	BYTE Count;	// [0xFE:AddOne] [0xFF:DelOne] 4
	BYTE btListType;	// 5
};

void GCMagicListOneSend(int aIndex, char Pos, WORD type, BYTE level, BYTE skill, BYTE btListType)
{
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];
	int lOfs = sizeof(pCount);

	PHeadSubSetB((LPBYTE)&pCount, 0xF3, 0x11, 0);
	pCount.Count = 0xFE;
	pCount.btListType = btListType;
	pList.Pos = Pos;
	pList.m_Skill = type;

	pList.m_Level = level<<3;
	pList.m_Level |= skill & 0x07;

	memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
	lOfs+= sizeof(pList);
	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));

	DataSend(aIndex, sendbuf, lOfs);
}



void GCMagicListOneDelSend(int aIndex, char Pos,WORD type, BYTE level, BYTE skill, BYTE btListType)
{
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;

	BYTE sendbuf[1000];
	int lOfs = sizeof(pCount);

	PHeadSubSetB((LPBYTE)&pCount, 0xF3, 0x11, 0);
	pCount.h.size = 0;
	pCount.Count = 0xFF;
	pCount.btListType = btListType;
	pList.Pos = Pos;

	pList.m_Skill = type;

	pList.m_Level = level<<3;
	pList.m_Level |= skill & 0x07;

	memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
	lOfs+= sizeof(pList);
	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));

	DataSend(aIndex, sendbuf, lOfs);
}



void GCMagicListMultiSend(LPOBJ lpObj, BYTE btListType)
{
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];
	int lOfs = sizeof(pCount);
	int Count = 0;
	pCount.btListType = btListType;

	PHeadSubSetB((LPBYTE)&pCount, 0xF3, 0x11, 0);

	for ( int n=0;n<MAX_MAGIC;n++)
	{
		if ( lpObj->Magic[n].IsMagic() == TRUE )
		{
			if( !g_MasterSkillSystem.CheckUsableWeaponSkill(lpObj,n) )
				continue;

			if( g_MasterSkillSystem.CheckMasterLevelSkill(lpObj->Magic[n].m_Skill) &&
				MagicDamageC.GetSkillUseType(lpObj->Magic[n].m_Skill) == 3 )
				continue;

			if( g_MasterSkillSystem.IsExistMLSBasedSkill(lpObj,lpObj->Magic[n].m_Skill) )
				continue;

			if( lpObj->Magic[n].m_bBrand )
				continue;

			pList.Pos = n;
			pList.m_Skill = lpObj->Magic[n].m_Skill;
			pList.m_Level = lpObj->Magic[n].m_Level << 3;
			pList.m_Level |= lpObj->Magic[n].m_Skill & 7;
			memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
			lOfs+= sizeof(pList);
			Count++;
		}
	}

	pCount.Count = Count;
	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));

	DataSend(lpObj->m_Index, sendbuf, lOfs);
}




struct PMSG_EQUIPMENTLIST
{
	PBMSG_HEAD h;	// C1:F3:13
	BYTE subcode;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE Equipment[CHAR_SET_SIZE];	// 6
};


void GCEquipmentSend(int aIndex)
{
	PMSG_EQUIPMENTLIST pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x13, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);
	gObjMakePreviewCharSet(aIndex);
	LPOBJ lpObj = &gObj[aIndex];

	memcpy(pMsg.Equipment, lpObj->CharSet, sizeof(lpObj->CharSet));
	MsgSendV2(lpObj, (LPBYTE)&pMsg, pMsg.h.size);
}



struct PMSG_RECALLMONLIFE
{
	PBMSG_HEAD h;	// C1:F3:20
	BYTE subcode;	// 3
	BYTE Life;	// 4
};


void GCRecallMonLife(int aIndex, int maxlife, int life)
{
	if ( maxlife <= 0 )
		return;
	
	PMSG_RECALLMONLIFE pMsg;
	BYTE per = life * 100 / maxlife;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x20, sizeof(pMsg));
	pMsg.Life = per;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}





struct PMSG_TIMEVIEW
{
	PBMSG_HEAD h;	// C1:F3:22
	BYTE subcode;	// 3
	WORD Second;	// 4
};


void GCTimeViewSend(int aIndex, int second)
{
	PMSG_TIMEVIEW pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x22, sizeof(pMsg));
	pMsg.Second = second;

	DataSend(aIndex, (UCHAR *)&pMsg, pMsg.h.size);
}




struct PMSG_GOALSEND
{
	PBMSG_HEAD h;	// C1:F3:23
	BYTE subcode;	// 3
	char RedTeamName[8];	// 4
	BYTE RedTeamScore;	// C
	char BlueTeamName[8];	// D
	BYTE BlueTeamScore;	// 15
};


void GCGoalSend(int aIndex, char* Name1, BYTE score1, char* Name2, BYTE score2)
{
	PMSG_GOALSEND pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x23, sizeof(pMsg));
	memcpy(pMsg.RedTeamName, Name1, sizeof(pMsg.RedTeamName));
	pMsg.RedTeamScore = score1;
	memcpy(pMsg.BlueTeamName, Name2, sizeof(pMsg.BlueTeamName));
	pMsg.BlueTeamScore = score2;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}




void GCSkillKeyRecv(PMSG_SKILLKEY * lpMsg, int aIndex)
{
	if ( !gObjIsGamePlaing(&gObj[aIndex]))
		return;

	if ( gObj[aIndex].iOptionRecv == TRUE )
		return;

	gObj[aIndex].iOptionRecv = TRUE;

	BYTE RkeyDefine = 0;
	RkeyDefine = lpMsg->RkeyDefine;

	DWORD QWERLevel = 0;
	QWERLevel = ntohl(lpMsg->QWERLevel);

	DGOptionDataSend(aIndex, gObj[aIndex].Name,
		lpMsg->SKillKey, lpMsg->GameOption,
		lpMsg->QkeyDefine,  lpMsg->WkeyDefine, lpMsg->EkeyDefine,lpMsg->ChatWindow,RkeyDefine,QWERLevel, gObj[aIndex].m_EnableUseChangeSkin);

	LogAddL("Option Recv %d %d %d %d %d %d", lpMsg->GameOption,
		lpMsg->QkeyDefine,  lpMsg->WkeyDefine, lpMsg->EkeyDefine, RkeyDefine, lpMsg->QWERLevel);
}








void GCSkillKeySend(int aIndex, LPBYTE keybuffer, BYTE GO, BYTE Qk, BYTE Wk, BYTE Ek, BYTE ChatWnd,BYTE Rk,DWORD QWERLevel)
{
	PMSG_SKILLKEY pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x30, sizeof(pMsg));
	memcpy(pMsg.SKillKey, keybuffer, sizeof(pMsg.SKillKey));
	pMsg.GameOption = GO;
	pMsg.QkeyDefine = Qk;
	pMsg.WkeyDefine = Wk;
	pMsg.EkeyDefine = Ek;
	pMsg.ChatWindow = ChatWnd;
	pMsg.RkeyDefine = Rk;
	pMsg.QWERLevel = QWERLevel;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
	LogAddL("Option Send %d %d %d %d %d %d", GO, Qk, Wk, Ek, Rk, QWERLevel);
}



struct PMSG_ITEMGETRESULT
{
	PBMSG_HEAD h;	// C1:22
	BYTE result;	// [0xFE:Money] 3
	BYTE Data[MAX_ITEM_INFO];	// 4
};


void GCMoneySend(int aIndex, DWORD money)
{
	PMSG_ITEMGETRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x22;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0xFE;

	LogAdd("Sending money");

	WORD hiWord = SET_NUMBERHW(money);
	WORD loWord = SET_NUMBERLW(money);
	pMsg.Data[0] = SET_NUMBERH(hiWord);
	pMsg.Data[1] = SET_NUMBERL(hiWord);
	pMsg.Data[2] = SET_NUMBERH(loWord);
	pMsg.Data[3] = SET_NUMBERL(loWord);

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}



void GCItemInventoryPutSend(int aIndex,BYTE result, BYTE iteminfo1, BYTE iteminfo2)
{
	PMSG_ITEMGETRESULT pResult;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x22;
	pResult.h.size = sizeof(pResult);
	pResult.result = result;
	pResult.Data[0] = iteminfo1;
	pResult.Data[1] = iteminfo2;
	pResult.h.size -= 2;

	DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size);
}





void CGItemGetRequest(PMSG_ITEMGETREQUEST * lpMsg, int aIndex)
{
	int item_num, map_num;
	PMSG_ITEMGETRESULT pResult;
	int type;
	int level;
	int special;
	int NOption;
	char szItemName[50];
	CMapItem * lpItem;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x22;
	pResult.h.size = sizeof(pResult);
	pResult.result = -1;

	if ( !gObjIsConnected(aIndex))
	{
		CloseClient(aIndex);
		return;
	}

	if ( gObj[aIndex].CloseType != -1 )
		return;

	if ( gObj[aIndex].DieRegen != 0 )
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if ( gObj[aIndex].m_IfState.use != 0 )
	{
		if ( gObj[aIndex].m_IfState.type != 3 )
		{
			::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 94)), aIndex, 1);
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size);

			return;
		}
	}

	if ( !::gObjFixInventoryPointer(aIndex))
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);

	if ( gObj[aIndex].pTransaction == TRUE )
	{
		LogAddTD("[%s][%s] CGItemGetRequest() Failed : Transaction == 1, IF_TYPE : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);
		return;

	}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if ( MAX_ITEM_TYPE_RANGE(item_num) == FALSE )
	{
		LogAdd("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = gObj[aIndex].MapNumber;

	if ( MAX_MAP_RANGE(map_num) == FALSE )
	{
		LogAdd("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_cItem[item_num];

	if ( lpItem->IsItem() == TRUE && lpItem->Give == false && lpItem->live == true)
	{
		if ( lpItem->m_QuestItem != false )
		{
			int bResult = g_QuestInfo.CheckItemCount(aIndex,lpItem->m_Type,lpItem->m_Level);

			if ( bResult == FALSE )
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
				return;
			}
		}

		if ( lpItem->m_Type == ITEMGET(13,20) )	// Wizard Ring
		{
			switch ( lpItem->m_Level )
			{
				case 0:
				{
					int iWRCount = gObjGetItemCountInIventory(aIndex, lpItem->m_Type/MAX_SUBTYPE_ITEMS, lpItem->m_Type%MAX_SUBTYPE_ITEMS, lpItem->m_Level);

					if ( iWRCount > 0 )
					{
						LogAdd("[Ring Event] Too many have Magician's Ring [%s][%s] (Name:%s, Count:%d)",
							gObj[aIndex].AccountID, gObj[aIndex].Name, lpItem->GetName(), iWRCount);

						pResult.result = -1;
						pResult.h.size -= sizeof(pResult.Data);

						DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
						::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 186)), aIndex, 1);

						return;
					}
					break;
				}
				case 1:

					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
					return;
				
			}
		}

		if ( lpItem->m_Type == ITEMGET(13,20) && lpItem->m_Level == 2 )	// Wizard Ring
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);			

			return;
		}

		if ( lpItem->m_Type == ITEMGET(13,38) )	// MoonStonePendant
		{
			int count = gObjGetItemCountInIventory(aIndex, lpItem->m_Type/MAX_SUBTYPE_ITEMS,
				lpItem->m_Type % MAX_SUBTYPE_ITEMS, lpItem->m_Level);

			if ( count > 0 )
			{
				LogAdd("[ MoonStonePendant ] Too many have MoonStonePendant [%s][%s] ( Name:%s, Count:%d )",
					gObj[aIndex].AccountID, gObj[aIndex].Name, lpItem->GetName(), count);

				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);	
				GCServerMsgStringSend(lMsg.Get(MSGGET(13, 56)), aIndex, 1);

				return;
			}
		}

		if ( lpItem->m_Type == ITEMGET(13,39) )	// ChangeRing
		{
			int count = gObjGetItemCountInIventory(aIndex, lpItem->m_Type/MAX_SUBTYPE_ITEMS,
				lpItem->m_Type % MAX_SUBTYPE_ITEMS, lpItem->m_Level);

			if ( count > 0 )
			{
				LogAdd("[ ChangeRing ] Too many have ChangeRing [%s][%s] ( Name:%s, Count:%d )",
					gObj[aIndex].AccountID, gObj[aIndex].Name, lpItem->GetName(), count);

				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);	
				GCServerMsgStringSend(lMsg.Get(MSGGET(13, 63)), aIndex, 1);

				return;
			}
		}

		type = lpItem->m_Type;
		level = lpItem->m_Level;
		special = lpItem->m_Special[0];
		NOption = lpItem->m_NewOption;
		strcpy(szItemName, lpItem->GetName());

		if ( lpItem->m_Type == ITEMGET(14,15) ) // Zen
		{
			if ( MapC[map_num].ItemGive(aIndex, item_num, false) == TRUE )
			{
				if ( !gObjCheckMaxZen(aIndex, lpItem->m_BuyMoney))
				{
					if ( gObj[aIndex].Money < MAX_ZEN )
					{
						gObj[aIndex].Money = MAX_ZEN;
						pResult.result = -2;
						WORD hiWord = SET_NUMBERHW(gObj[aIndex].Money);
						WORD loWord = SET_NUMBERLW(gObj[aIndex].Money);
						pResult.Data[0] = SET_NUMBERH(hiWord);
						pResult.Data[1] = SET_NUMBERL(hiWord);
						pResult.Data[2] = SET_NUMBERH(loWord);
						pResult.Data[3] = SET_NUMBERL(loWord);

						DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

						return;
					}

					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);			

					return;
				}

				gObj[aIndex].Money += lpItem->m_BuyMoney;
				pResult.result = -2;

				WORD hiWord = SET_NUMBERHW(gObj[aIndex].Money);
				WORD loWord = SET_NUMBERLW(gObj[aIndex].Money);
				pResult.Data[0] = SET_NUMBERH(hiWord);
				pResult.Data[1] = SET_NUMBERL(hiWord);
				pResult.Data[2] = SET_NUMBERH(loWord);
				pResult.Data[3] = SET_NUMBERL(loWord);
				pResult.h.size -= 3;
			}

			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		}
		else
		{
			if ( lpItem->m_Type == ITEMGET(13,32) )
			{

				int pos = gObjOverlapItemUsingDur((lpItem)?((CItem *)&lpItem->m_Number):NULL, map_num, item_num, aIndex, 20, ITEMGET(13,32), 0);

				if ( MAIN_INVENTORY_RANGE(pos) != FALSE )
				{
					pResult.result = -3;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
					gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemDurSend(aIndex, pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);

					return;
				}
			}

			if ( lpItem->m_Type == ITEMGET(13,33) )
			{
				int pos = gObjOverlapItemUsingDur((lpItem)?((CItem *)&lpItem->m_Number):NULL, map_num, item_num, aIndex, 20, ITEMGET(13,33), 0);

				if ( MAIN_INVENTORY_RANGE(pos) != FALSE )
				{
					pResult.result = -3;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
					gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemDurSend(aIndex, pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);

					return;
				}
			}

			if ( lpItem->m_Type == ITEMGET(13,34) )
			{
				int pos = gObjOverlapItemUsingDur((lpItem)?((CItem *)&lpItem->m_Number):NULL, map_num, item_num, aIndex, 10, ITEMGET(13,34), 0);

				if ( MAIN_INVENTORY_RANGE(pos) != FALSE )
				{
					pResult.result = -3;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
					gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemDurSend(aIndex, pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);

					return;
				}
			}

			if ( lpItem->m_Type == ITEMGET(14,70) ||
				 lpItem->m_Type == ITEMGET(14,71) ||
				 lpItem->m_Type == ITEMGET(14,94) )
			{
				int pos = gObjOverlapItemUsingDur((lpItem)?((CItem *)&lpItem->m_Number):NULL, map_num, item_num, aIndex, 50, lpItem->m_Type, 0);

				if ( MAIN_INVENTORY_RANGE(pos) != FALSE )
				{
					pResult.result = -3;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
					gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemDurSend(aIndex, pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);

					return;
				}
			}

			if ( lpItem->m_Type == ITEMGET(14,88) ||
				 lpItem->m_Type == ITEMGET(14,89) ||
				 lpItem->m_Type == ITEMGET(14,90) ||
				 lpItem->m_Type == ITEMGET(14,85) ||
				 lpItem->m_Type == ITEMGET(14,86) ||
				 lpItem->m_Type == ITEMGET(14,87) )
			{
				int Dur = 1;

				switch( lpItem->m_Type )
				{
				case ITEMGET(14,88):
					Dur = 10;
					break;
				case ITEMGET(14,89):
					Dur = 30;
					break;
				case ITEMGET(14,90):
					Dur = 255;
					break;
				case ITEMGET(14,85):
					Dur = 3;
					break;
				case ITEMGET(14,86):
					Dur = 3;
					break;
				case ITEMGET(14,87):
					Dur = 3;
					break;
				}

				int pos = gObjOverlapItemUsingDur((lpItem)?((CItem *)&lpItem->m_Number):NULL, map_num, item_num, aIndex, Dur, lpItem->m_Type, 0);

				if ( MAIN_INVENTORY_RANGE(pos) != FALSE )
				{
					pResult.result = -3;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
					gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemDurSend(aIndex, pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);

					return;
				}
			}
			
			if ( lpItem->m_Type == ITEMGET(14,29) ) // Symbol of Kundun
			{
				for ( int n=0;n<MAIN_INVENTORY_SIZE;n++)
				{
					int pos = ::g_KalimaGate.CheckOverlapKundunMark(aIndex, lpItem->m_Level);

					if ( MAIN_INVENTORY_RANGE(pos) )
					{
						if ( MapC[map_num].ItemGive(aIndex, item_num, true) == TRUE )
						{
							BYTE NewOption[MAX_EXOPTION_SIZE];

							::ItemIsBufExOption(NewOption, (lpItem != NULL)?(CItem*)&lpItem->m_Number:NULL);

							LogAddTD(lMsg.Get(MSGGET(1, 221)), gObj[aIndex].AccountID, gObj[aIndex].Name, map_num,
								gObj[aIndex].X, gObj[aIndex].Y, lpItem->m_Number, szItemName, type,
								level, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, (int)lpItem->m_Durability,
								NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5],
								NewOption[6], lpItem->m_SetOption, lpItem->m_ItemOptionEx>>7,
								g_kJewelOfHarmonySystem.GetItemStrengthenOption((lpItem)?((CItem *)&lpItem->m_Number):NULL), g_kJewelOfHarmonySystem.GetItemOptionLevel((lpItem)?((CItem *)&lpItem->m_Number):NULL));

							pResult.result = -3;

							DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);			

							gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;

							if ( gObj[aIndex].pInventory[pos].m_Durability >= 5.0f )
							{
								int NewDur = (int)(gObj[aIndex].pInventory[pos].m_Durability - 5.0f);
								::gObjInventoryItemSet(aIndex, pos, -1);
								gObj[aIndex].pInventory[pos].Clear();
								::GCInventoryItemDeleteSend(aIndex, pos, 1);
								::ItemSerialCreateSend(aIndex, 235, gObj[aIndex].X, gObj[aIndex].Y, ItemGetNumberMake(14, 28),
									lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0);

								LogAddTD("[Kalima] [%s][%s] Make Lost Kalima Map (Left Kundun Mark:%d)",
									gObj[aIndex].AccountID, gObj[aIndex].Name, NewDur);

								if ( NewDur > 0 )
								{
									lpItem->m_State = 2;
									lpItem->Give = false;
									lpItem->live = true;
									lpItem->m_Durability = (float)NewDur;
									continue;

								}
							}
							else
							{
								GCItemDurSend(aIndex,pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);
								
							}
							return;
						}
					}
					else
					{
						break;
					}
				}
			}

			if ( lpItem->m_Type == ITEMGET(14,21) && lpItem->m_Level == 3) // Sign of Lord
			{	
				for ( int n=0;n<MAIN_INVENTORY_SIZE;n++)
				{
					int pos = ::g_CastleSiegeSync.CheckOverlapCsMarks(aIndex);

					if ( MAIN_INVENTORY_RANGE(pos) )
					{
						int Dur = (int)(gObj[aIndex].pInventory[pos].m_Durability + lpItem->m_Durability);

						if ( Dur <= 255 )
						{
							if ( MapC[map_num].ItemGive(aIndex, item_num, true) == TRUE )
							{
								pResult.result = -3;
								DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
								gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;
								GCItemDurSend( aIndex, pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability,0);
								return;
							}
						}
						else
						{
							lpItem->m_Durability = (float)(Dur - 255);
							gObj[aIndex].pInventory[pos].m_Durability = 255.0f;
							GCItemDurSend(aIndex, pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);
						}

					}
					else
					{
						break;
					}
				}
			}


			if ( lpItem->m_Type == ITEMGET(14,110) ) // Dimension Mark
			{
				if( lpItem->m_Durability > 5 )
					lpItem->m_Durability = 1;

				for ( int n=0;n<MAIN_INVENTORY_SIZE;n++)
				{
					int pos = ::gObjCheckOverlapItemUsingDur(aIndex,5,lpItem->m_Type,0);

					if ( MAIN_INVENTORY_RANGE(pos) )
					{
						if ( MapC[map_num].ItemGive(aIndex, item_num, true) == TRUE )
						{
							BYTE NewOption[MAX_EXOPTION_SIZE];

							::ItemIsBufExOption(NewOption, (lpItem != NULL)?(CItem*)&lpItem->m_Number:NULL);

							LogAddTD(lMsg.Get(MSGGET(1, 221)), gObj[aIndex].AccountID, gObj[aIndex].Name, map_num,
								gObj[aIndex].X, gObj[aIndex].Y, lpItem->m_Number, szItemName, type,
								level, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, (int)lpItem->m_Durability,
								NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5],
								NewOption[6], lpItem->m_SetOption, lpItem->m_ItemOptionEx>>7,
								g_kJewelOfHarmonySystem.GetItemStrengthenOption((lpItem)?((CItem *)&lpItem->m_Number):NULL), g_kJewelOfHarmonySystem.GetItemOptionLevel((lpItem)?((CItem *)&lpItem->m_Number):NULL));

							pResult.result = -3;
							DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);			

							gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;

							if ( gObj[aIndex].pInventory[pos].m_Durability >= 5.0f )
							{
								int NewDur = (int)(gObj[aIndex].pInventory[pos].m_Durability - 5.0f);
								::gObjInventoryItemSet(aIndex, pos, -1);
								gObj[aIndex].pInventory[pos].Clear();
								::GCInventoryItemDeleteSend(aIndex, pos, 1);
								::ItemSerialCreateSend(aIndex, 235, gObj[aIndex].X, gObj[aIndex].Y, ItemGetNumberMake(14, 111),
									0, 0, 0, 0, 0, aIndex, 0, 0);

								LogAddTD("[Doppelganger] [%s][%s] Make Dimension Mirror (Left Dimension Mark:%d)",
									gObj[aIndex].AccountID, gObj[aIndex].Name, NewDur);

								if ( NewDur > 0 )
								{
									lpItem->m_State = 2;
									lpItem->Give = false;
									lpItem->live = true;
									lpItem->m_Durability = (float)NewDur;
									continue;

								}
							}
							else
							{
								GCItemDurSend(aIndex,pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);
								
							}
							return;
						}
					}
					else
					{
						break;
					}
				}
			}

			if ( lpItem->m_Type == ITEMGET(14,101) ) // Old Paper
			{
				for ( int n=0;n<MAIN_INVENTORY_SIZE;n++)
				{
					int pos = ::g_ImperialGuardian.CheckOverlapMysteriousPaper(aIndex, lpItem->m_Level);

					if ( MAIN_INVENTORY_RANGE(pos) )
					{
						if ( MapC[map_num].ItemGive(aIndex, item_num, true) == TRUE )
						{
							BYTE NewOption[MAX_EXOPTION_SIZE];

							::ItemIsBufExOption(NewOption, (lpItem != NULL)?(CItem*)&lpItem->m_Number:NULL);

							LogAddTD(lMsg.Get(MSGGET(1, 221)), gObj[aIndex].AccountID, gObj[aIndex].Name, map_num,
								gObj[aIndex].X, gObj[aIndex].Y, lpItem->m_Number, szItemName, type,
								level, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, (int)lpItem->m_Durability,
								NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5],
								NewOption[6], lpItem->m_SetOption, lpItem->m_ItemOptionEx>>7,
								g_kJewelOfHarmonySystem.GetItemStrengthenOption((lpItem)?((CItem *)&lpItem->m_Number):NULL), g_kJewelOfHarmonySystem.GetItemOptionLevel((lpItem)?((CItem *)&lpItem->m_Number):NULL));

							pResult.result = -3;
							DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);			

							gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;

							if ( gObj[aIndex].pInventory[pos].m_Durability >= 5.0f )
							{
								int NewDur = (int)(gObj[aIndex].pInventory[pos].m_Durability - 5.0f);
								::gObjInventoryItemSet(aIndex, pos, -1);
								gObj[aIndex].pInventory[pos].Clear();
								::GCInventoryItemDeleteSend(aIndex, pos, 1);
								::ItemSerialCreateSend(aIndex, 235, gObj[aIndex].X, gObj[aIndex].Y, ItemGetNumberMake(14, 102),
									0, 0, 0, 0, 0, aIndex, 0, 0);

								LogAddTD("[ImperialGuardianFort] [%s][%s] Make Imperial Letter (Left Old Paper:%d)",
									gObj[aIndex].AccountID, gObj[aIndex].Name, NewDur);

								if ( NewDur > 0 )
								{
									lpItem->m_State = 2;
									lpItem->Give = false;
									lpItem->live = true;
									lpItem->m_Durability = (float)NewDur;
									continue;

								}
							}
							else
							{
								GCItemDurSend(aIndex,pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);
								
							}
							return;
						}
					}
					else
					{
						break;
					}
				}
			}

			if ( lpItem->m_Type == ITEMGET(13, 145) ) // Old Paper
			{
				for ( int n=0;n<MAIN_INVENTORY_SIZE;n++)
				{
					int pos = -1;
					for(int i = INVETORY_WEAR_SIZE; i < 204; i++)
					{
						if( gObj[aIndex].pInventory[i].IsItem() == TRUE &&
							gObj[aIndex].pInventory[i].m_Type == ITEMGET(13, 145) &&
							gObj[aIndex].pInventory[i].m_Level == lpItem->m_Level )
						{
							int nItemDur = (int)gObj[aIndex].pInventory[i].m_Durability;

							if( ((nItemDur < 0) ? FALSE : (nItemDur <= 4)) != 0 )
								pos = i;
						}
					}

					if ( MAIN_INVENTORY_RANGE(pos) )
					{
						if ( MapC[map_num].ItemGive(aIndex, item_num, true) == TRUE )
						{
							BYTE NewOption[MAX_EXOPTION_SIZE];

							::ItemIsBufExOption(NewOption, (lpItem != NULL)?(CItem*)&lpItem->m_Number:NULL);

							LogAddTD(lMsg.Get(MSGGET(1, 221)), gObj[aIndex].AccountID, gObj[aIndex].Name, map_num,
								gObj[aIndex].X, gObj[aIndex].Y, lpItem->m_Number, szItemName, type,
								level, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, (int)lpItem->m_Durability,
								NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5],
								NewOption[6], lpItem->m_SetOption, lpItem->m_ItemOptionEx>>7,
								g_kJewelOfHarmonySystem.GetItemStrengthenOption((lpItem)?((CItem *)&lpItem->m_Number):NULL), g_kJewelOfHarmonySystem.GetItemOptionLevel((lpItem)?((CItem *)&lpItem->m_Number):NULL));

							pResult.result = -3;
							DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);			

							gObj[aIndex].pInventory[pos].m_Durability += lpItem->m_Durability;

							if ( gObj[aIndex].pInventory[pos].m_Durability >= 5.0f )
							{
								int NewDur = (int)(gObj[aIndex].pInventory[pos].m_Durability - 5.0f);
								::gObjInventoryItemSet(aIndex, pos, -1);
								gObj[aIndex].pInventory[pos].Clear();
								::GCInventoryItemDeleteSend(aIndex, pos, 1);
								::ItemSerialCreateSend(aIndex, 235, gObj[aIndex].X, gObj[aIndex].Y, ItemGetNumberMake(13, 146),
									0, 0, 0, 0, 0, aIndex, 0, 0);

								LogAddTD("[Acheron] [%s][%s] Make Spirit Map (Left Old Map:%d)",
									gObj[aIndex].AccountID, gObj[aIndex].Name, NewDur);

								if ( NewDur > 0 )
								{
									lpItem->m_State = 2;
									lpItem->Give = false;
									lpItem->live = true;
									lpItem->m_Durability = (float)NewDur;
									continue;

								}
							}
							else
							{
								GCItemDurSend(aIndex,pos, (BYTE)gObj[aIndex].pInventory[pos].m_Durability, 0);
								
							}
							return;
						}
					}
					else
					{
						break;
					}
				}
			}

			pResult.result = ::gObjInventoryInsertItemTemp(&gObj[aIndex], lpItem);

			if ( pResult.result != 0xFF )
			{
				ItemByteConvert((LPBYTE)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
					lpItem->m_Option3, lpItem->m_Level, (BYTE)lpItem->m_Durability, lpItem->m_NewOption,
					lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx,
					lpItem->m_SocketOption,lpItem->m_SocketBonus);

				if ( MapC[map_num].ItemGive(aIndex, item_num, false) == TRUE )
				{
					BYTE pos = ::gObjInventoryInsertItem(aIndex, lpItem);

					if ( pos == 0xFF )
						pResult.result = -1;

					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

					if ( pos == 0xFF )
					{
						::GCItemListSend(aIndex);
					}
					else
					{
						BYTE NewOption[MAX_EXOPTION_SIZE];

						::ItemIsBufExOption(NewOption, (lpItem != NULL)?(CItem*)&lpItem->m_Number:NULL);

						LogAddTD(lMsg.Get(MSGGET(1, 221)), gObj[aIndex].AccountID, gObj[aIndex].Name, map_num,
							gObj[aIndex].X, gObj[aIndex].Y, lpItem->m_Number, szItemName, type,
							level, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, (INT)lpItem->m_Durability,
							NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5],
							NewOption[6], lpItem->m_SetOption, lpItem->m_ItemOptionEx>>7,
							g_kJewelOfHarmonySystem.GetItemStrengthenOption((lpItem)?((CItem *)&lpItem->m_Number):NULL), g_kJewelOfHarmonySystem.GetItemOptionLevel((lpItem)?((CItem *)&lpItem->m_Number):NULL));
						::GCSendGetItemInfoForParty(aIndex, lpItem);

						if ( BC_MAP_RANGE(map_num) )
						{
							int Bridge = g_BloodCastle.GetBridgeLevel(map_num);
							if ( g_BloodCastle.CheckQuestItemSerial(Bridge, lpItem) )
							{
								g_BloodCastle.m_BridgeData[Bridge].m_iBC_QUEST_ITEM_USER_INDEX = gObj[aIndex].m_Index;
								int Bridge = g_BloodCastle.GetBridgeLevel(gObj[aIndex].MapNumber);

								char szTempMsg[128];

								switch ( lpItem->m_Level )
								{
									case 0:	// Staff of Archangel
										wsprintf(szTempMsg, lMsg.Get(MSGGET(4, 150)), gObj[aIndex].Name);
										g_BloodCastle.SendNoticeMessage(Bridge, szTempMsg);
										break;

									case 1:	// Sword of Archangel
										wsprintf(szTempMsg, lMsg.Get(MSGGET(4, 151)), gObj[aIndex].Name);
										g_BloodCastle.SendNoticeMessage(Bridge, szTempMsg);
										break;

									case 2:	// Crossbow of Archangel
										wsprintf(szTempMsg, lMsg.Get(MSGGET(4, 152)), gObj[aIndex].Name);
										g_BloodCastle.SendNoticeMessage(Bridge, szTempMsg);
										break;

									default:
										szTempMsg[0] = 0;
								}

								LogAddTD("[Blood Castle] (%d) (Account:%s, Name:%s) picked up Angel King's Weapon (%d)",
									Bridge+1, gObj[aIndex].AccountID, gObj[aIndex].Name, lpItem->m_Level);
							}
						}
						
						if ( IT_MAP_RANGE(map_num) && lpItem->m_Type == ITEMGET(14,64) )
						{
							g_IllusionTemple.PickupRelic(gObj[aIndex].MapNumber,aIndex,pos);
							g_IllusionTemple.PickupRelicSend(&gObj[aIndex]);
							LogAddTD("[Illusion Temple] (%d) (Account:%s, Name:%s) picked up Relics Item(serial:%u)",
								map_num - (MAP_INDEX_ILLUSIONTEMPLE1 - 1),gObj[aIndex].AccountID,gObj[aIndex].Name,lpItem->m_Number);
						}
#if (ENABLETEST_ARCA == 1)
						if( g_ArcaBattle.IsArcaBattleServer() && lpItem->m_Type == AB_REW_KILL_TROPHY )
						{
							g_ArcaBattle.BootyItemGetCnt(gObj[aIndex]);
						}
#endif
					}
				}
				else
				{
					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
				}
			}
			else
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
			}
		}
	}
	else
	{
		pResult.h.size -= sizeof(pResult.Data);
		DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size);
	}
}

BOOL CGItemDropRequest(PMSG_ITEMTHROW * lpMsg, int aIndex, BOOL drop_type)
{
	BOOL CopyItem = FALSE;
	BYTE NewOption[MAX_EXOPTION_SIZE];

	if ( !PacketCheckTime(&gObj[aIndex]))
		return FALSE;

	PMSG_ITEMTHROW_RESULT pResult;

	PHeadSetB((LPBYTE)&pResult, 0x23, sizeof(pResult));
	pResult.Result = true;
	pResult.Ipos = lpMsg->Ipos;

	if ( gObj[aIndex].m_IfState.use != 0 )
	{
		if ( gObj[aIndex].m_IfState.type != 3 )
		{
			pResult.Result = false;
			return FALSE;
		}
	}

	if ( gObj[aIndex].CloseType != -1 )
		pResult.Result = false;

	if ( gObj[aIndex].m_bMapSvrMoveQuit == true )
	{
		LogAddTD("[ANTI-HACK][Item Duplication] - Item Drop during MapServer Move [%s][%s]",
			gObj[aIndex].AccountID, gObj[aIndex].Name);
		pResult.Result = false;
	}


	if ( !::gObjFixInventoryPointer(aIndex))
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);

	if ( gObj[aIndex].pTransaction == 1 )
	{
		LogAddTD("[%s][%s] CGItemDropRequest() Failed : Transaction == 1, IF_TYPE : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);
		pResult.Result = false;
		
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpMsg->Ipos == 0xFF )	// Null Position
	{
		LogAdd("error:%s %d", __FILE__, __LINE__);
		pResult.Result = false;
	}

	if ( lpMsg->Ipos < 0 || lpMsg->Ipos > MAIN_INVENTORY_SIZE-1)	// Out of Bounds
		pResult.Result = false;

	if (!::gObjCanItemTouch(lpObj, 0))
		pResult.Result = false;

	// Icarus Preventions about Dinorant
	if ( gObj[aIndex].MapNumber == MAP_INDEX_ICARUS)
	{
		if ( lpMsg->Ipos == 8 )
		{
			if ( lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,3) ) // Dinorant
			{
				if( lpObj->pInventory[7].IsItem() == FALSE )
				{
					pResult.Result = false;
				}
			}
		}
		else if ( lpMsg->Ipos == 7 )
		{
			if ( lpObj->pInventory[8].m_Type != ITEMGET(13,3) ) // Dinorant 
			{
				pResult.Result = false;
			}
		}
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(12,30) ||
		 lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(12,31) ||
		 lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,36) ||
		 lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,37) )
	{
		pResult.Result = false;
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,42) )
	{
		pResult.Result = false;
	}

	if ( IT_MAP_RANGE( lpObj->MapNumber ) )
	{
		if( lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,51) )
		{
			pResult.Result = false;
		}
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(14,64) )
	{
		pResult.Result = false;
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type >= ITEMGET(12,36) &&
		 lpObj->pInventory[lpMsg->Ipos].m_Type <= ITEMGET(12,40) )
	{
		if ( gPkLimitFree == FALSE )
		{
			pResult.Result = false;
		}
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(12,49) ||
		 lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(12,50) )
	{
		if ( gPkLimitFree == FALSE )
		{
			pResult.Result = false;
		}
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type >= ITEMGET(12,130) &&
		 lpObj->pInventory[lpMsg->Ipos].m_Type <= ITEMGET(12,135) )
	{
		if ( gPkLimitFree == FALSE )
		{
			pResult.Result = false;
		}
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type >= ITEMGET(12,262) &&
		 lpObj->pInventory[lpMsg->Ipos].m_Type <= ITEMGET(12,267) )
	{
		if ( gPkLimitFree == FALSE )
		{
			pResult.Result = false;
		}
	}


#if (CUSTOM_WINGS == 1)

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type >= ITEMGET(12,440) &&
		 lpObj->pInventory[lpMsg->Ipos].m_Type <= ITEMGET(12,445) )
	{
		if ( gPkLimitFree == FALSE )
		{
			pResult.Result = false;
		}
	}

#endif

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,52) ||
		 lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,53) )
	{
		pResult.Result = false;
	}

	if (	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(14,13) 
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(14,14) 
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(14,16)
		||	(lpObj->pInventory[lpMsg->Ipos].m_Type >= ITEMGET(12,0) && lpObj->pInventory[lpMsg->Ipos].m_Type <= ITEMGET(12,6)) 
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(12,15) 
		||	(lpObj->pInventory[lpMsg->Ipos].m_Level >= 7  && lpObj->pInventory[lpMsg->Ipos].m_Type < ITEMGET(12,0)) 
		||	lpObj->pInventory[lpMsg->Ipos].IsSetItem() != FALSE 
		||	lpObj->pInventory[lpMsg->Ipos].IsExtItem() != FALSE
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,14)
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,19)
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,15) 
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,34) 
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(14,31)
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(14,41)
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(14,42)
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(14,43) 
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(14,44) 
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(12,41)
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(12,42)
		||	lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(12,43) )
	{
		if ( gPkLimitFree == FALSE )
		{
			pResult.Result = false;
		}
	}

	if ( g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(&lpObj->pInventory[lpMsg->Ipos]) == TRUE )
	{
		pResult.Result = false;
		GCServerMsgStringSend(lMsg.Get(MSGGET(13, 43)), lpObj->m_Index, 1);
	}

	if ( g_kJewelOfHarmonySystem.IsJewelOfHarmonyOriginal(lpObj->pInventory[lpMsg->Ipos].m_Type) ||
		 g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(lpObj->pInventory[lpMsg->Ipos].m_Type)   ||
		 g_kJewelOfHarmonySystem.IsJewelOfHarmonySmeltingItems(lpObj->pInventory[lpMsg->Ipos].m_Type) )
	{
		pResult.Result = false;
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_Type == ITEMGET(13,39) )
	{
		pResult.Result = false;
	}

	if ( IsCashItem(lpObj->pInventory[lpMsg->Ipos].m_Type) == TRUE )
	{
		pResult.Result = false;
	}

	if ( IsCashSpecialItem(lpObj->pInventory[lpMsg->Ipos].m_Type) == TRUE )
	{
		pResult.Result = false;
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_bLOCKED )
	{
		pResult.Result = false;
	}

	if ( g_ElementalSystem.IsElementalItem(lpObj->pInventory[lpMsg->Ipos].m_Type) )
	{
		pResult.Result = false;
	}

	if ( lpObj->pInventory[lpMsg->Ipos].m_QuestItem != FALSE )
	{
		return 0xFF;
	}

#if (ENABLETEST_NEWPVP == 1)
	// portal?
	if (drop_type == 0 && lpObj->pInventory[lpMsg->Ipos].m_Type == (512 * 14) + 28 && g_NewPVP.IsDuel(*lpObj)) {
		pResult.Result = 0x00;
	}
#endif

	if ( pResult.Result == 1 )
	{
		::ItemIsBufExOption(NewOption, &lpObj->pInventory[lpMsg->Ipos]);

		int map = lpObj->MapNumber;
		int type = lpObj->pInventory[lpMsg->Ipos].m_Type;
		int level = lpObj->pInventory[lpMsg->Ipos].m_Level;
		float dur = lpObj->pInventory[lpMsg->Ipos].m_Durability;
		BOOL bIsItem = lpObj->pInventory[lpMsg->Ipos].IsItem();
		BYTE Option1 = lpObj->pInventory[lpMsg->Ipos].m_Option1;
		BYTE Option2 = lpObj->pInventory[lpMsg->Ipos].m_Option2;
		BYTE Option3 = lpObj->pInventory[lpMsg->Ipos].m_Option3;
		BYTE NOption = lpObj->pInventory[lpMsg->Ipos].m_NewOption;
		DWORD serial = lpObj->pInventory[lpMsg->Ipos].m_Number;
		int PetLevel = lpObj->pInventory[lpMsg->Ipos].m_PetItem_Level;
		int PetExp = lpObj->pInventory[lpMsg->Ipos].m_PetItem_Exp;
		BYTE SOption = lpObj->pInventory[lpMsg->Ipos].m_SetOption;
		BYTE btItemEffectEx = lpObj->pInventory[lpMsg->Ipos].m_ItemOptionEx;


		BYTE SocketOptions[MAX_SOCKET_COUNT];
		BYTE SocketBonus;
		

		g_SocketItem.GetItemOptions(&lpObj->pInventory[lpMsg->Ipos],&SocketOptions[0],&SocketBonus);

		int serial2 = lpObj->pInventory[lpMsg->Ipos].m_Number;
		char szItemName[50] = "Item";
		BYTE JOHOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(&lpObj->pInventory[lpMsg->Ipos]);
		BYTE JOHOptionLevel = g_kJewelOfHarmonySystem.GetItemOptionLevel(&lpObj->pInventory[lpMsg->Ipos]);

		if ( gObjCheckSerial0ItemList(&lpObj->pInventory[lpMsg->Ipos]) != FALSE )
		{
			MsgOutput(lpObj->m_Index, lMsg.Get(MSGGET(13, 26)));
			LogAddTD("[ANTI-HACK][Serial 0 Item] [ItemDrop] (%s)(%s) Item(%s) Pos(%d)",
				lpObj->AccountID, lpObj->Name, lpObj->pInventory[lpMsg->Ipos].GetName(),
				lpMsg->Ipos);

			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		
		if ( !::gObjInventorySearchSerialNumber(lpObj, serial2))	// ANTI DUPE SYSTEM
		{
			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if ( bIsItem == TRUE )
		{
			strcpy(szItemName, lpObj->pInventory[lpMsg->Ipos].GetName());
			
			BYTE EventItemBagState = g_EventItemBagManager.OpenBox(aIndex, type, level);

			if( EventItemBagState == 1 )
			{
				::gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				pResult.Result = true;
			}
			else if( EventItemBagState == 0 )
			{
				pResult.Result = false;
				DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
				return FALSE;
			}
			else if( type == ITEMGET(14,51) || type == ITEMGET(14,63) || type == ITEMGET(14,99) )
			{
				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				PMSG_SERVERCMD ServerCmd = { 0 };
				PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
				ServerCmd.CmdType = 0;
				ServerCmd.X = lpObj->X;
				ServerCmd.Y = lpObj->Y;
				MsgSendV2(lpObj, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
				DataSend(aIndex, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
				LogAddTD("[%s][%s][%d]%d/%d Used Fireworks:%u (%s:%d/level:%d/skill:%d/op2:%d/op3:%d)",
					lpObj->AccountID, lpObj->Name, lpObj->MapNumber, lpObj->X, lpObj->Y,
					serial, szItemName, type, level, Option1, Option2, Option3);
			}
			else if ( type == ITEMGET(12,26) )
			{
				switch ( level )
				{
					case 0:
						{
							gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
							pResult.Result = true;
							int ltype = ITEMGET(12,26);
							int randor = rand()%100;
							int Level;

							if ( randor < 10 )
							{
								Level = 3;
							}
							else if ( randor < 45 )
							{
								Level = 1;
							}
							else
							{
								Level = 2;
							}

							ItemSerialCreateSend(aIndex, lpObj->MapNumber, lpObj->X, lpObj->Y,
								ltype, Level, 1, 0, 0, 0, lpObj->m_Index, 0, 0);
							
							LogAddTD("[Hidden TreasureBox Event] [%s][%s] Throw Mysterious Bead : Drop (%d) - serial:%d",
								lpObj->AccountID, lpObj->Name, Level, serial);
						}
						break;
					case 1: case 2: case 3:
						{
							gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
							pResult.Result = true;

							PMSG_SERVERCMD ServerCmd;

							PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
							ServerCmd.CmdType = 0;
							ServerCmd.X = lpObj->X;
							ServerCmd.Y = lpObj->Y;

							MsgSendV2(lpObj, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
							DataSend(lpObj->m_Index, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
													
							LogAddTD("[Hidden TreasureBox Event] [%s][%s] Throw Crystal (%d) - serial:%d",
								lpObj->AccountID, lpObj->Name, level, serial);
						}
						break;
					case 5:
						LogAddTD("[Hidden TreasureBox Event] [%s][%s] Throw Surprise Treasure Box - serial:%d",
							lpObj->AccountID, lpObj->Name,  serial);
						gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
						pResult.Result = true;

						int money = 1000000;
						MapC[lpObj->MapNumber].MoneyItemDrop(money, lpObj->X, lpObj->Y);
							
						LogAddTD("[Hidden TreasureBox Event] [%s][%s] Event ZenDrop : %d : (%d)(%d/%d)",
							lpObj->AccountID, lpObj->Name, money, lpObj->MapNumber, lpObj->X, lpObj->Y);
						break;
				}
			}
			else if ( type == ITEMGET(14,28) ) // Lost Map
			{
				if ( g_KalimaGate.CreateKalimaGate(aIndex, level, lpMsg->px, lpMsg->py))
				{
					LogAddTD("[Kalima] [%s][%s] Success to Make Kalima Gate (Lost Kalima Map Serial:%d)",
						gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].pInventory[lpMsg->Ipos].m_Number);

					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if ( type == ITEMGET(13,7) ) // Contract (Summon)
			{
				int iMonsterID = 0;

				if ( level == 0 )
					iMonsterID = 286;
				else if ( level == 1 )
					iMonsterID = 287;


				if ( g_CsNPC_Mercenary.CreateMercenary(aIndex, iMonsterID, lpMsg->px, lpMsg->py) )
				{
					::gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if ( type == ITEMGET(13,11) ) // Guardian / LifeStone
			{
				BOOL bResult = FALSE;

				if ( level == 0 )
					bResult = g_CsNPC_Guardian.CreateGuardian(aIndex);
#if (GS_CASTLE == 1)
				else if(level == 1)
					bResult = g_CsNPC_LifeStone.CreateLifeStone(aIndex);
#endif

				if ( bResult == TRUE )
					::gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				else
				{
					pResult.Result = false;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if( g_SnakeBegins.IsScroll(type) )
			{
				if( g_SnakeBegins.DropScroll(lpObj, type) )
				{
					LogAddTD("[SnakeBegins] [%s][%s] Success to create Snake",
						gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].pInventory[lpMsg->Ipos].m_Number);
					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
					return false;
				}
			}
#if (ENABLE_CUSTOM_MONSTERSPAWNER == 1)
			else if (MonsterSpawnerMng::getInstance()->isKeyItem(type)) {
				if (MonsterSpawnerMng::getInstance()->procCreate(aIndex, type)) {
					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				} else {
					pResult.Result = false;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
					return false;
				}
			}
#endif
#ifdef __NOVUS__
			else if( g_SummonSpot.IsScroll(type) )
			{
				if( g_SummonSpot.DropScroll(lpObj, type) )
				{
					LogAddTD("[SummonSpot] [%s][%s] Success to create spot",
						gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].pInventory[lpMsg->Ipos].m_Number);
					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
					return false;
				}
			}
#endif
			else
			{
				int OwnerIndex = aIndex;

				if ( (type >= ITEMGET(14,0) && type <= ITEMGET(14,10)) || (type == ITEMGET(4,7) ) || type ==ITEMGET(4,15) )
					OwnerIndex = -1;

				if ( MapC[map].ItemDrop( type, level, dur, lpMsg->px, lpMsg->py, Option1, Option2, Option3,
					NOption, SOption, serial2, OwnerIndex, PetLevel, PetExp, btItemEffectEx,&SocketOptions[0],SocketBonus) == TRUE )
				{
					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
					pResult.Result = true;

					if ( drop_type == FALSE )
					{
						LogAddTD(lMsg.Get(MSGGET(1, 223)), lpObj->AccountID, lpObj->Name, lpObj->MapNumber,
							lpObj->X, lpObj->Y, serial, szItemName, type, level, Option1, Option2,
							Option3, (int)dur, NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4],
							NewOption[5], NewOption[6], SOption, btItemEffectEx>>7, JOHOption, JOHOptionLevel);
					}
					else
					{
						LogAddTD(lMsg.Get(MSGGET(1, 224)), lpObj->AccountID, lpObj->Name, lpObj->MapNumber,
							lpObj->X, lpObj->Y, serial, szItemName, type, level, Option1, Option2,
							Option3, (int)dur, NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4],
							NewOption[5], NewOption[6], SOption, btItemEffectEx>>7, JOHOption, JOHOptionLevel );
					}
				}
				else
				{
					pResult.Result = false;
				}
			}
		}
		else	// bIsItem
		{
			LogAdd(lMsg.Get(MSGGET(1, 225)));
			pResult.Result = false;
		}
	}

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

	if ( CopyItem != FALSE )
		::GCItemListSend(aIndex);

	if ( pResult.Result == 1 )
	{
		if ( lpMsg->Ipos < 12 )
		{
			if ( lpMsg->Ipos == 10 || lpMsg->Ipos == 11 )
			{
				gObjUseSkill.SkillChangeUse(aIndex);
			}


			::gObjMakePreviewCharSet(aIndex);

			PMSG_USEREQUIPMENTCHANGED pChange;

			PHeadSetB((LPBYTE)&pChange, 0x25, sizeof(pChange));
			pChange.NumberH = SET_NUMBERH(aIndex);
			pChange.NumberL = SET_NUMBERL(aIndex);
			ItemByteConvert(pChange.ItemInfo, lpObj->pInventory[lpMsg->Ipos]);
			pChange.ItemInfo[I_OPTION] = lpMsg->Ipos <<  4;
			pChange.ItemInfo[I_OPTION] |= LevelSmallConvert(aIndex, lpMsg->Ipos) & 0x0F;

			DataSend(aIndex, (LPBYTE)&pChange, pChange.h.size);
			MsgSendV2(lpObj, (LPBYTE)&pChange, pChange.h.size);
		}
	}

	return pResult.Result;
}









struct PMSG_INVENTORYITEMMOVE_RESULT
{
	PBMSG_HEAD h;	// C3:24
	BYTE result;	// 3
	BYTE Pos;	// 4
	BYTE ItemInfo[MAX_ITEM_INFO];	// 5
};

void GCItemMoveResultSend(int aIndex, BYTE result, BYTE pos, LPBYTE const ItemInfo)
{
	PMSG_INVENTORYITEMMOVE_RESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x24;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = result;
	pMsg.Pos = pos;
	pMsg.ItemInfo[I_TYPE] = ItemInfo[I_TYPE];
	pMsg.ItemInfo[I_OPTION] = ItemInfo[I_OPTION];
	pMsg.ItemInfo[I_DUR] = ItemInfo[I_DUR];
	pMsg.ItemInfo[I_NOPTION] = ItemInfo[I_NOPTION];

	pMsg.ItemInfo[I_SOPTION] = ItemInfo[I_SOPTION];
	pMsg.ItemInfo[I_380OPTION] = ItemInfo[I_380OPTION];
	pMsg.ItemInfo[I_JOHOPTION] = ItemInfo[I_JOHOPTION];

	pMsg.ItemInfo[7] = ItemInfo[7];
	pMsg.ItemInfo[8] = ItemInfo[8];
	pMsg.ItemInfo[9] = ItemInfo[9];
	pMsg.ItemInfo[10] = ItemInfo[10];
	pMsg.ItemInfo[11] = ItemInfo[11];

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

	if( result != (BYTE)-1 )
	{
		if( (ItemInfo[I_380OPTION]&2) == 2 && (ItemInfo[I_380OPTION]&4) == 0 )
		{
			CItem* lpItem = &gObj[aIndex].pInventory[pos];

			g_CashShop.GCSendItemInfo(aIndex,lpItem->m_Type,pos,lpItem->m_Number);
		}
	}
}






void CGInventoryItemMove(PMSG_INVENTORYITEMMOVE * lpMsg, int aIndex)
{
	BYTE result;
	BYTE ItemInfo[MAX_ITEM_INFO];

	if ( !PacketCheckTime(&gObj[aIndex]))
	{
		::GCItemMoveResultSend(aIndex, -1, 0, ItemInfo);
		return;
	}

	if ( !::gObjIsConnectedGP(aIndex))
	{
		LogAddC(2,"[%s][%d] error-L3", __FILE__, __LINE__);
		::GCItemMoveResultSend(aIndex, -1, 0, ItemInfo);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	int it_type = 0;

	if ( lpObj->m_ReqWarehouseOpen != false )
	{
		LogAddC(2,"[%s][%d] error-L3", __FILE__, __LINE__);
		GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);

		return;
	}

	int source = lpMsg->source_item_num;
	int target = lpMsg->target_item_num;

	if ( lpMsg->tFlag == 2 || lpMsg->sFlag == 2 )
		it_type = 6;

	if ( lpMsg->tFlag == 0 && lpMsg->sFlag == 0 )
		it_type = 8;

	if ( (lpMsg->tFlag == 0 && lpMsg->sFlag == 4) ||
		(lpMsg->tFlag == 4 && lpMsg->sFlag == 0) ||
		(lpMsg->tFlag == 4 && lpMsg->sFlag == 4) )
		it_type = 8;

	if ( lpMsg->tFlag == lpMsg->sFlag &&
		 target == source)
	{
		GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
		return;
	}

	if (::gObjCanItemTouch(lpObj, it_type) == FALSE)
	{
		if ( lpMsg->sItemInfo[I_TYPE] == ITEMGET(4,7) || lpMsg->sItemInfo[I_TYPE] == ITEMGET(4,15) )
		{
			if ( target >= 12 || source < 12 )
			{
				::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
				return;
			}
		}
		else
		{
			::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}
	}

	if ( lpObj->DieRegen != FALSE )
	{
		::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
		return;
	}

	ItemInfo[I_TYPE] = lpMsg->sItemInfo[I_TYPE];
	ItemInfo[I_OPTION] = lpMsg->sItemInfo[I_OPTION];
	ItemInfo[I_DUR] = lpMsg->sItemInfo[I_DUR];
	ItemInfo[I_NOPTION] = lpMsg->sItemInfo[I_NOPTION];
	ItemInfo[I_SOPTION] = lpMsg->sItemInfo[I_SOPTION];
	ItemInfo[I_380OPTION] = lpMsg->sItemInfo[I_380OPTION];
	ItemInfo[I_JOHOPTION] = lpMsg->sItemInfo[I_JOHOPTION];

	ItemInfo[7] = lpMsg->sItemInfo[7];
	ItemInfo[8] = lpMsg->sItemInfo[8];
	ItemInfo[9] = lpMsg->sItemInfo[9];
	ItemInfo[10] = lpMsg->sItemInfo[10];
	ItemInfo[11] = lpMsg->sItemInfo[11];

	if ( lpMsg->sFlag == 3 || lpMsg->tFlag == 3 )
	{
		if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 7 )
		{
			::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			LogAdd("[%s][%s] error-L1 : used not Chaosbox", gObj[aIndex].AccountID, gObj[aIndex].Name);

			return;
		}

		if ( lpObj->ChaosLock == TRUE )
		{
			LogAddTD("[%s][%s] error-L3 : CBMixing", gObj[aIndex].AccountID, gObj[aIndex].Name);
			::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
		
			return;
		}
	}

	if ( lpMsg->sFlag == 5 || lpMsg->tFlag == 5 )
	{
		if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 13 )
		{
			::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			LogAdd("[%s][%s] error-L1 : used not DarkTrainerBox", gObj[aIndex].AccountID, gObj[aIndex].Name);

			return;
		}

		if ( lpObj->ChaosLock == TRUE )
		{
			LogAddTD("[%s][%s] error-L3 : DarkTrainerBoxMixing", gObj[aIndex].AccountID, gObj[aIndex].Name);
			::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
		
			return;
		}
	}

	if ( lpMsg->sFlag == 6 || lpMsg->sFlag == 7 || lpMsg->sFlag == 8 ||
		 lpMsg->tFlag == 6 || lpMsg->tFlag == 7 || lpMsg->tFlag == 8 )
	{
		if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type !=7 )
		{
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			LogAdd("[%s][%s] error-L1 : used not ChaosBox",
				gObj[aIndex].AccountID, gObj[aIndex].Name);

			return;
		}

		if ( lpObj->ChaosLock == TRUE )
		{
			LogAddTD("[%s][%s] error-L3 : JewelOfHarmony Mixing",
				gObj[aIndex].AccountID, gObj[aIndex].Name);
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);

			return;
		}
	}

	if ( lpMsg->sFlag == 9 || lpMsg->tFlag == 9 )
	{
		if ( lpObj->m_IfState.use	 < 1 || lpObj->m_IfState.type !=7 )
		{
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			LogAdd("[%s][%s] error-L1 : used not ChaosBox",
				gObj[aIndex].AccountID, gObj[aIndex].Name);

			return;
		}

		if ( lpObj->ChaosLock == TRUE )
		{
			LogAddTD("[%s][%s] error-L3 : JewelOfHarmony Mixing",
				gObj[aIndex].AccountID, gObj[aIndex].Name);
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);

			return;
		}
	}

	if ( lpMsg->sFlag == 11 || lpMsg->sFlag == 12 || lpMsg->sFlag == 13 || lpMsg->sFlag == 14 ||
		 lpMsg->tFlag == 11 || lpMsg->tFlag == 12 || lpMsg->tFlag == 13 || lpMsg->tFlag == 14 )
	{
		if ( lpObj->m_IfState.use	 < 1 || lpObj->m_IfState.type !=7 )
		{
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			LogAdd("[%s][%s] error-L1 : used not ChaosBox",
				gObj[aIndex].AccountID, gObj[aIndex].Name);

			return;
		}

		if ( lpObj->ChaosLock == TRUE )
		{
			LogAddTD("[%s][%s] error-L3 : SocketItem Mixing",
				gObj[aIndex].AccountID, gObj[aIndex].Name);
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);

			return;
		}
	}

	if ( lpMsg->sFlag == 15 || lpMsg->sFlag == 16 ||
		 lpMsg->tFlag == 15 || lpMsg->tFlag == 16 )
	{
		if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type !=7 )
		{
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			LogAdd("[%s][%s] error-L1 : used not ChaosBox",
				gObj[aIndex].AccountID, gObj[aIndex].Name);
			return;
		}

		if ( lpObj->ChaosLock == TRUE )
		{
			LogAddTD("[%s][%s] error-L3 : LuckyItem Mixing",
				gObj[aIndex].AccountID, gObj[aIndex].Name);
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}
	}

	if ( lpMsg->sFlag == 17 || lpMsg->tFlag == 17 )
	{
		if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type !=7 )
		{
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			LogAdd("[%s][%s] error-L1 : used not ChaosBox",
				gObj[aIndex].AccountID, gObj[aIndex].Name);
			return;
		}

		if ( lpObj->ChaosLock == TRUE )
		{
			LogAddTD("[%s][%s] error-L3 : Elemental Mixing",
				gObj[aIndex].AccountID, gObj[aIndex].Name);
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}
	}

	if ( lpMsg->sFlag == 2 || lpMsg->tFlag == 2 )
	{
		if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 6 )
		{
			::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			LogAdd("[%s][%s] error-L1 : used not Warehouse", gObj[aIndex].AccountID, gObj[aIndex].Name);

			return;
		}
	}

	if ( lpMsg->sFlag == 1 && lpMsg->tFlag == 0 )
	{
		if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 1 )
		{
			::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			LogAdd("[%s][%s] error-L1 : used not Trade", gObj[aIndex].AccountID, gObj[aIndex].Name);

			return;
		}
	}

	if ( lpMsg->sFlag == 2 && lpMsg->tFlag == 0 )
	{
		int money = ::GetWarehouseUsedHowMuch(lpObj->Level,lpObj->MasterLevel, lpObj->WarehousePW);

		if ( (lpObj->Money - money ) < 1 && (lpObj->WarehouseMoney - money) < 1)
		{
			::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			char szTemp[256];
			sprintf_s(szTemp, lMsg.Get(MSGGET(6, 69)), money);
			::GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);

			return;
		}

		if ( ::bCanWarehouseLock == TRUE )
		{
			if ( gObj[aIndex].WarehouseLock == 1 )
			{
				::GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
				::GCServerMsgStringSend(lMsg.Get(MSGGET(6, 70)), lpObj->m_Index, 1);

				return;
			}
		}
	}

	if( it_type != 8 )
	{
		int type = (ItemInfo[I_TYPE] + ((ItemInfo[I_NOPTION] & 0x80 )*2)) + ((ItemInfo[I_380OPTION] & 0xF0)<<5);
		LPITEM_ATTRIBUTE lpItemAttr = GetItemAttr(type);

		if ( lpItemAttr == NULL )
		{
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}

		if ( lpItemAttr->QuestItem != false )
		{
			GCItemMoveResultSend(aIndex, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}
	}

	if ( (lpMsg->sFlag == 0 && lpMsg->tFlag == 0) ||
 		 (lpMsg->sFlag == 2 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 2) ||
 		 (lpMsg->sFlag == 2 && lpMsg->tFlag == 2) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 3) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 5) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 6) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 7) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 8) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 9) ||
		 (lpMsg->sFlag == 9 && lpMsg->tFlag == 9) ||
		 (lpMsg->sFlag == 9 && lpMsg->tFlag == 0) ||

		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 10) ||
		 (lpMsg->sFlag == 10 && lpMsg->tFlag == 10) ||
		 (lpMsg->sFlag == 10 && lpMsg->tFlag == 0) ||

		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 4) ||
		 (lpMsg->sFlag == 4 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 4 && lpMsg->tFlag == 4) ||
		 (lpMsg->sFlag == 5 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 5 && lpMsg->tFlag == 5) ||
		 (lpMsg->sFlag == 6 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 6 && lpMsg->tFlag == 6) ||
		 (lpMsg->sFlag == 7 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 7 && lpMsg->tFlag == 7) ||
		 (lpMsg->sFlag == 8 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 8 && lpMsg->tFlag == 8) ||

		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 11) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 12) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 13) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 14) ||
		 (lpMsg->sFlag == 11 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 12 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 13 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 14 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 11 && lpMsg->tFlag == 11) ||
		 (lpMsg->sFlag == 12 && lpMsg->tFlag == 12) ||
		 (lpMsg->sFlag == 13 && lpMsg->tFlag == 13) ||
		 (lpMsg->sFlag == 14 && lpMsg->tFlag == 14) ||

		 (lpMsg->sFlag == 15 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 16 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 15 && lpMsg->tFlag == 15) ||
		 (lpMsg->sFlag == 16 && lpMsg->tFlag == 16) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 15) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 16) ||

		 (lpMsg->sFlag == 17 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 17 && lpMsg->tFlag == 17) ||
		 (lpMsg->sFlag == 0 && lpMsg->tFlag == 17) ||

		 (lpMsg->sFlag == 3 && lpMsg->tFlag == 0) ||
		 (lpMsg->sFlag == 3 && lpMsg->tFlag == 3) )
	{
		BOOL DurSSend;
		BOOL DurTSend;

		result = gObjInventoryMoveItem(aIndex, source, target, DurSSend, DurTSend, lpMsg->sFlag, lpMsg->tFlag, (LPBYTE)&ItemInfo);
		::GCItemMoveResultSend(aIndex, result, target, (LPBYTE)&ItemInfo);

		if ( DurSSend != FALSE )
			::GCItemDurSend(aIndex, source, (BYTE)lpObj->pInventory[source].m_Durability, FALSE);

		if ( DurTSend != FALSE )
			::GCItemDurSend(aIndex, target, (BYTE)lpObj->pInventory[target].m_Durability, FALSE);

		int type = lpMsg->sItemInfo[0] + ((lpMsg->sItemInfo[3]&0x80)*2)+((lpMsg->sItemInfo[5]&0xF0)<<5);

		if( type == ITEMGET(14, 29) ||
			type == ITEMGET(12, 144) ||
			type == ITEMGET(12, 146) )
		{
			if( lpObj->pInventory[target].m_Durability == 0.0f )
			{
				if( lpMsg->sFlag == 0 &&
					lpMsg->tFlag == 0 )
				{
					::GCInventoryItemDeleteSend(aIndex, target, FALSE);
				}
			}
		}

		if( type == ITEMGET(14,110) || type == ITEMGET(14,101) )
		{
			if( lpObj->pInventory[target].m_Durability == 0.0f )
			{
				if( lpMsg->sFlag == 0 &&
					lpMsg->tFlag == 0 )
				{
					::GCInventoryItemDeleteSend(aIndex, target, FALSE);
				}
			}
		}

		if( type == ITEMGET(13, 135) )
		{
			if( lpObj->pInventory[target].m_Durability == 0.0f )
			{
				if( lpMsg->sFlag == 0 &&
					lpMsg->tFlag == 0 )
				{
					::GCInventoryItemDeleteSend(aIndex, target, FALSE);
				}
			}
		}

		if ( result != 0xFF && lpMsg->sFlag == 2 && lpMsg->tFlag == 0)
		{
			int money = ::GetWarehouseUsedHowMuch(lpObj->Level,lpObj->MasterLevel, lpObj->WarehousePW);

			if ( (lpObj->Money - money) > 0 )
			{
				int iZen = lpObj->Money;
				lpObj->Money -= money;
				GCMoneySend(lpObj->m_Index, lpObj->Money);

				LogAdd("Pay WareHouse Money(In Inventory) : %d - %d = %d",
					iZen, money, lpObj->Money);
			}
			else if ( (lpObj->WarehouseMoney - money) > 0 )
			{
				int iZen = lpObj->WarehouseMoney;
				lpObj->WarehouseMoney -= money;
	
				LogAdd("Pay WareHouse Money(In WareHouse) : %d - %d = %d",
					iZen, money, lpObj->WarehouseMoney);

				GCWarehouseInventoryMoneySend(aIndex, 1, lpObj->Money, lpObj->WarehouseMoney);
			}
		}

		return;
	}

	if ( lpMsg->sFlag == 1 && lpMsg->tFlag == 1 )
	{
		result = gObjTradeTradeMove(lpObj, source, target);
		GCItemMoveResultSend(aIndex, result, target, (LPBYTE)&ItemInfo);

		return;
	}

	if ( lpMsg->sFlag == 1 && lpMsg->tFlag == 0 )
	{
		result = gObjTradeInventoryMove(lpObj, source, target);

		if ( result == 0xFF )
		{
			GCItemMoveResultSend(aIndex, 0xFF, target, (LPBYTE)&ItemInfo);
			CGTradeCancelButtonRecv(aIndex);
		}
		else
		{
			GCItemMoveResultSend(aIndex, result, target, (LPBYTE)&ItemInfo);
		}

		if ( result == 0xFF )
		{
			lpObj->m_IfState.state = 0;
			GCItemListSend(aIndex);
		}
		
		if ( lpObj->TargetNumber >= 0 )
		{
			lpObj->TradeOk = false;
			gObj[gObj[aIndex].TargetNumber].TradeOk = false;
			GCTradeOkButtonSend(lpObj->TargetNumber, 2);
			GCTradeOkButtonSend(aIndex, 0);
		}

		return;
	}

	if ( lpMsg->sFlag == 0 && lpMsg->tFlag == 1 )
	{
		result = gObjInventoryTradeMove(lpObj, source, target);
		GCItemMoveResultSend(aIndex, result, target, (LPBYTE)&ItemInfo);
	}
}

void GCEquipmentChange(int aIndex, int pos)
{
	PMSG_USEREQUIPMENTCHANGED pChange;
	LPOBJ lpObj = &gObj[aIndex];

	PHeadSetB((LPBYTE)&pChange, 0x25, sizeof(PMSG_USEREQUIPMENTCHANGED));
	pChange.NumberH = SET_NUMBERH(aIndex);
	pChange.NumberL = SET_NUMBERL(aIndex);
	ItemByteConvert(pChange.ItemInfo, lpObj->pInventory[pos]);
	pChange.ItemInfo[I_OPTION] = pos << 4;
	pChange.ItemInfo[I_OPTION] |= LevelSmallConvert(aIndex, pos) & MAX_ITEM_LEVEL;

	MsgSendV2(lpObj, (LPBYTE)&pChange, pChange.h.size);

}




struct PMSG_SHOPITEMCOUNT
{
	PWMSG_HEAD h;	// C2:31
	BYTE Type;	// 4
	BYTE count;	// 5
};



void CGTalkRequestRecv(PMSG_TALKREQUEST * lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int DealerNumber ;
	PMSG_TALKRESULT pResult;
	PMSG_SHOPITEMCOUNT pShopItemCount;

	if ( !PacketCheckTime(lpObj))
		return;

	if ( gObj[aIndex].CloseType != -1 )
		return;

	if ( !gObjIsConnectedGP(aIndex) )
	{
		LogAddTD("[ANTI-HACK][NPC Talk] Couldn't talk.. (%s)(%s)",
			gObj[aIndex].AccountID, gObj[aIndex].Name);

		return;
	}

	BYTE SendByte[2048];
	int lOfs = 0;

	if ( lpObj->m_IfState.use > 0 )
		return;

	DealerNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	
	if ( DealerNumber < 0 || DealerNumber > OBJMAX-1)
		return;

	if ( lpObj->MapNumber != gObj[DealerNumber].MapNumber )
		return;

	if ( (lpObj->X < (gObj[DealerNumber].X-5) )|| (lpObj->X> (gObj[DealerNumber].X+5) )||(lpObj->Y < (gObj[DealerNumber].Y-5)) ||(lpObj->Y > (gObj[DealerNumber].Y+5)))
	{
		return;
	}

	int ShopNum = gObj[DealerNumber].ShopNumber;

	if ( gObj[DealerNumber].Type == OBJ_NPC )
	{
		if ( NpcTalk(&gObj[DealerNumber], lpObj) == TRUE )
			return;
	}

	if ( ::gObjFixInventoryPointer(aIndex) == false )
	{
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	if ( gObj[DealerNumber].Class == 234 )	// Npc Server Division
	{
		pResult.h.c = 0xC3;
		pResult.h.headcode = 0x30;
		pResult.h.size = sizeof(pResult);
		pResult.result = 0;
		pResult.result = 5;

		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( gObj[DealerNumber].Type == OBJ_NPC && ShopNum < 0)
		return;

	CShop* lpShop = g_ShopManager.Get(ShopNum);

	if( lpShop )
	{
		if ( lpShop->ItemCount < TRUE )
		{
			if ( (rand()%2) != 0 )
				ChatTargetSend(&gObj[DealerNumber], lMsg.Get(MSGGET(4, 119)), aIndex);
			else
				ChatTargetSend(&gObj[DealerNumber], lMsg.Get(MSGGET(4, 120)), aIndex);

			return;
		}
	}

	if ( gPkLimitFree == FALSE )
	{
		int bPk = FALSE;

#if (ENABLE_FIX_NPCTALKPK == 1)
		if( lpObj->m_PK_Level > 4 )
		{
			bPk = TRUE;
		}
#else

		if( lpObj->PartyNumber >= 0 )
		{
			if( gParty.GetPartyPkLevel(lpObj->PartyNumber) > 4 )
			{
				bPk = TRUE;
			}
		}
		else if( lpObj->m_PK_Level > 4 )
		{
			bPk = TRUE;
		}
#endif

		if ( bPk == TRUE )
		{
			if ( (rand()%2) != 0 )
				ChatTargetSend(&gObj[DealerNumber], lMsg.Get(MSGGET(4, 121)), aIndex);
			else
				ChatTargetSend(&gObj[DealerNumber], lMsg.Get(MSGGET(4, 122)), aIndex);
			return;
		}
	}

	if ( gMerryXMasNpcEvent == TRUE )
	{
		if ( (rand()%6) == 0 )
		{
			ChatTargetSend(&gObj[DealerNumber], lMsg.Get(MSGGET(4, 123)), aIndex);
		}
	}

	if ( ::gHappyNewYearNpcEvent == TRUE )
	{
		if ( (rand()%6) == 0 )
		{
			ChatTargetSend(&gObj[DealerNumber], lMsg.Get(MSGGET(4, 124)), aIndex);
		}
	}

	lpObj->TargetShopNumber = ShopNum;
	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 3;
	lpObj->m_ShopTime = 0;
	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(pResult);
	pResult.result = 0;

	if ( ShopNum == 100 )
	{
		if ( gObj[aIndex].m_ReqWarehouseOpen != false )
			return;
		
		gObj[aIndex].m_ReqWarehouseOpen = true;
		lpObj->m_IfState.type = 6;
		lpObj->m_IfState.state = 0;
	}
	else if ( ShopNum == 101 )
	{
		if ( bCanChaosBox == TRUE )
		{
			if ( lpObj->m_bPShopOpen == true )
			{
				LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
					lpObj->AccountID, lpObj->Name);
				GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);

				return;
			}

			lpObj->m_IfState.type = 7;
			lpObj->m_IfState.state = 0;
			pResult.result = 3;

			if( gObj[DealerNumber].Class == 450 )
			{
				pResult.result = 22;
			}

			lpObj->bIsChaosMixCompleted = false;
			pResult.level1 = gDQChaosSuccessRateLevel1;
			pResult.level2 = gDQChaosSuccessRateLevel2;
			pResult.level3 = gDQChaosSuccessRateLevel3;
			pResult.level4 = gDQChaosSuccessRateLevel4;
			pResult.level5 = gDQChaosSuccessRateLevel5;
			pResult.level6 = gDQChaosSuccessRateLevel6;
			pResult.level7 = gDQChaosSuccessRateLevel7;

			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
			gObjInventoryTrans(lpObj->m_Index);

			LogAddTD("[%s][%s] Open Chaos Box", lpObj->AccountID, lpObj->Name);
			gObjItemTextSave(lpObj);
			gObjWarehouseTextSave(lpObj);
			GCAnsCsMapSvrTaxInfo( lpObj->m_Index, 1, ::g_CastleSiegeSync.GetTaxRateChaos(lpObj->m_Index));
		}
	}
	else
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
	}

	if ( ShopNum == 100 )
	{
		gObj[aIndex].WarehouseCount = 0;
		GDGetWarehouseList(aIndex, gObj[aIndex].AccountID);

		return;
	}

	if ( ShopNum == 101 )
	{
		return;
	}
	else if( lpShop )
	{
		lOfs = 0;
		lOfs += sizeof(pShopItemCount);

		int size = lOfs + lpShop->SendItemDataLen;
		PHeadSetW((LPBYTE)&pShopItemCount, 0x31, size);
		pShopItemCount.Type = 0;
		pShopItemCount.count = lpShop->ItemCount;
		memcpy(SendByte, &pShopItemCount, sizeof(pShopItemCount));
		memcpy(&SendByte[lOfs], lpShop->SendItemData, lpShop->SendItemDataLen);

		DataSend(aIndex, SendByte, size);
		GCAnsCsMapSvrTaxInfo(lpObj->m_Index,2, g_CastleSiegeSync.GetTaxRateStore(lpObj->m_Index));

		g_ItemPrice.SendPrice(aIndex, ShopNum);
	}
}






void GCUserWarehouseSend(LPOBJ lpObj)
{
	if ( lpObj->m_IfState.type != 6 )
		return;

	if ( lpObj->m_IfState.type == 6 && lpObj->m_IfState.state == 1 )
		return;

	PMSG_SHOPITEMCOUNT pMsg;
	BYTE SendByte[4096];
	int lOfs = sizeof(pMsg);

	PHeadSetW((LPBYTE)&pMsg, 0x31, 0);
	pMsg.count = 0;
	pMsg.Type = 0;

	for ( int n=0;n<WAREHOUSE_SIZE;n++)
	{
		if ( lpObj->pWarehouse[n].IsItem() == TRUE )
		{
			SendByte[lOfs] = n;
			lOfs++;
			ItemByteConvert(&SendByte[lOfs], lpObj->pWarehouse[n] );
			lOfs += MAX_ITEM_INFO;
			pMsg.count ++;
		}
	}

	pMsg.h.sizeH = SET_NUMBERH(lOfs);
	pMsg.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(SendByte, &pMsg, sizeof(pMsg));

	DataSend(lpObj->m_Index, SendByte, lOfs);
	GCWarehouseInventoryMoneySend(lpObj->m_Index, 1, lpObj->Money, lpObj->WarehouseMoney);

	if ( lpObj->m_IfState.type == 6 )
	{
		lpObj->m_IfState.state = 1;
		lpObj->WarehouseSave = TRUE;
	}
}




struct PMSG_BUYRESULT
{
	PBMSG_HEAD h;	// C1:32
	BYTE Result;	// 3
	BYTE ItemInfo[MAX_ITEM_INFO];	// 4
};



void CGBuyRequestRecv(PMSG_BUYREQUEST * lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	PMSG_BUYRESULT pResult;

	PHeadSetB((LPBYTE)&pResult, 0x32, sizeof(pResult));
	pResult.Result = -1;


	if ( !PacketCheckTime(lpObj))
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( gObj[aIndex].CloseType != -1 )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( !::gObjFixInventoryPointer(aIndex))
	{
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	if ( gObj[aIndex].m_ShopTime == 0 )
		gObj[aIndex].m_ShopTime = 1;

	if ( gObj[aIndex].m_ShopTime > 	60 )
	{
		if ( lpObj->m_IfState.use != 0 && lpObj->m_IfState.type == 3 )
		{
			pResult.Result = (BYTE)-2;
			lpObj->TargetShopNumber = -1;
			lpObj->m_IfState.use = 0;
			lpObj->m_IfState.type = 0;
		}

		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpObj->m_IfState.use > 0 )
	{
		if ( lpObj->m_IfState.type != 3 )
		{
			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}

	int tShop = lpObj->TargetShopNumber;

	if ( tShop < 0 || tShop > MAX_SHOP-1 )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpMsg->Pos > MAX_ITEM_IN_SHOP-1 )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	CShop* lpShop = g_ShopManager.Get(tShop);
	if (!lpShop) {
		return;
	}

	if (lpShop->m_CreditPrice) {
		if (!lpShop->m_item[lpMsg->Pos].IsItem()) {
			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
		int	tmpPrice = lpShop->m_CreditPriceTable[lpMsg->Pos];
		if (lpObj->m_Credits < tmpPrice) {
			pResult.Result = -1;
			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
		lpObj->m_Credits -= tmpPrice;
		pResult.Result = gObjShopBuyInventoryInsertItem(aIndex, lpShop->m_item[lpMsg->Pos]);
		if (pResult.Result != 0xFF) {
			char tmpNotice[200] = { 0 };
			sprintf(tmpNotice, "Item has been purchased, balance: %d", lpObj->m_Credits);
			GCServerMsgStringSend(tmpNotice, aIndex, 1);
			requestDbUpdateCredits(lpObj, lpObj->m_Credits, tmpPrice);
			g_CreditLog.Output("[%s] [%s] item has been purchased (%s, price: %d), balance: %d",
				lpObj->AccountID, lpObj->Name, lpShop->m_item[lpMsg->Pos].GetName(), tmpPrice, lpObj->m_Credits);
			ItemByteConvert((LPBYTE)&pResult.ItemInfo, lpShop->m_item[lpMsg->Pos]);
		}
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpShop->m_item[lpMsg->Pos].IsItem() == TRUE )
	{
		DWORD iItemValue;
		DWORD iStorePrice;
		DWORD iStoreTaxMoney = 0;
		BYTE Divisor;

		if( g_ItemPrice.GetPrice(tShop, 
			lpShop->m_item[lpMsg->Pos].m_Type, 
			lpShop->m_item[lpMsg->Pos].m_Level,
			iItemValue, Divisor) )
		{
			
		}
		else
		{
			iItemValue = lpShop->m_item[lpMsg->Pos].m_BuyMoney;
		}

		iStorePrice = iItemValue;
		iStoreTaxMoney = (int)((__int64)iItemValue * (__int64)g_CastleSiegeSync.GetTaxRateStore(lpObj->m_Index) / (__int64)100);
		iStorePrice += iStoreTaxMoney;

		/*if( g_ArcaBattle.IsArcaBattleServer() && iBuyMoney > 0 )
		{
			iBuyMoney *= 3;
		}*/
			
		if ( iStorePrice < 0 )
			iStorePrice  = 0;

		if ( iStoreTaxMoney < 0 )
			iStoreTaxMoney = 0;

		if ( lpObj->Money < iStorePrice )
			pResult.Result = -1;
		else
		{
			BOOL bNoItem = TRUE;

			if ( (lpShop->m_item[lpMsg->Pos].m_Type >= ITEMGET(14,0) && lpShop->m_item[lpMsg->Pos].m_Type <= ITEMGET(14,8)) ||
				 (lpShop->m_item[lpMsg->Pos].m_Type >= ITEMGET(14,35) && lpShop->m_item[lpMsg->Pos].m_Type <= ITEMGET(14,40)))
			{
				int dur = (int)lpShop->m_item[lpMsg->Pos].m_Durability;

				if ( dur == 0 )
					dur = 1;

				if ( ::gObjSearchItem(&gObj[aIndex], lpShop->m_item[lpMsg->Pos].m_Type,dur, lpShop->m_item[lpMsg->Pos].m_Level) == TRUE )
				{
					bNoItem = FALSE;
					lpObj->Money -= iStorePrice;
					::g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney);

					if ( lpObj->Money < 0 )
						lpObj->Money = 0;

		
					::GCMoneySend(aIndex, lpObj->Money);
				}
			}

			if ( lpShop->m_item[lpMsg->Pos].m_Type >= ITEMGET(13,71) && lpShop->m_item[lpMsg->Pos].m_Type <= ITEMGET(13,75) )
			{
				bNoItem = FALSE;

				if( CheckInventoryEmptySpace(lpObj,4,2) == FALSE )
				{
					MsgOutput(aIndex,"You have no enough empty space");
				}
				else
				{
					int type = -1;

					switch( lpShop->m_item[lpMsg->Pos].m_Type )
					{
					case ITEMGET(13,71):	type = 0;	break;
					case ITEMGET(13,72):	type = 1;	break;
					case ITEMGET(13,73):	type = 2;	break;
					case ITEMGET(13,74):	type = 3;	break;
					case ITEMGET(13,75):	type = 4;	break;
					}

					if( type != -1 )
					{
						if ( gGamblingEvent.BuyRequest(aIndex,236,0,0,type) == TRUE )
						{
							bNoItem = FALSE;
							lpObj->Money -= iStorePrice;
							::g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney);
		
							if ( lpObj->Money < 0 )
								lpObj->Money = 0;	
				
							::GCMoneySend(aIndex, lpObj->Money);
						}
					}
				}
			}

			if ( bNoItem != FALSE )
			{
				pResult.Result = gObjShopBuyInventoryInsertItem(aIndex, lpShop->m_item[lpMsg->Pos]);
				
				if ( pResult.Result != 0xFF )
				{
					ItemByteConvert((LPBYTE)&pResult.ItemInfo, lpShop->m_item[lpMsg->Pos]);
					lpObj->Money -= iStorePrice;
					::g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney);

					if ( lpObj->Money < 0 )
						lpObj->Money = 0;

					GCMoneySend(aIndex, lpObj->Money);
					int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(lpObj->m_Index);

					LogAddTD("[%s][%s] (%d) Shop buy [%d][%d][%d][%s] LEV:%d, DUR:%d, OP:[%d][%d][%d]",
						lpObj->AccountID, lpObj->Name, tShop, iItemValue, iTaxRate, iItemValue+iStoreTaxMoney, lpShop->m_item[lpMsg->Pos].GetName(),
						lpShop->m_item[lpMsg->Pos].m_Level, lpShop->m_item[lpMsg->Pos].m_Durability,
						lpShop->m_item[lpMsg->Pos].m_Option1, lpShop->m_item[lpMsg->Pos].m_Option2,
						lpShop->m_item[lpMsg->Pos].m_Option3);
				}
			}
		}
	}

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
}




struct PMSG_SELLRESULT
{
	PBMSG_HEAD h;	// C1:33
	BYTE Result;	// 3
	DWORD Money;	// 4
};




void CGSellRequestRecv(PMSG_SELLREQUEST * lpMsg, int aIndex)
{
	PMSG_SELLRESULT pResult;
	LPOBJ lpObj = &gObj[aIndex];

	PHeadSetB((LPBYTE)&pResult, 0x33, sizeof(pResult));
	pResult.Result = false;
	pResult.Money = 0;

	if ( gObj[aIndex].CloseType != -1 )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpObj->m_IfState.use > 0 && lpObj->m_IfState.type != 3 )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( !PacketCheckTime(lpObj))
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( !::gObjFixInventoryPointer(aIndex))
	{
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	if ( gObj[aIndex].pTransaction == 1 )
	{
		LogAddTD("[%s][%s] CGSellRequestRecv() Failed : Transaction == 1, IF_TYPE : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);

		return;
	}

	int tShop = lpObj->TargetShopNumber;

	if ( tShop < 0 || tShop > MAX_SHOP-1 )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( !gObjCanItemTouch(lpObj, 3))
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpMsg->Pos > MAIN_INVENTORY_SIZE-1 )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		LogAdd(lMsg.Get(MSGGET(1, 227)), lpMsg->Pos);
		return;
	}

	if ( gObjCheckSerial0ItemList(&lpObj->pInventory[lpMsg->Pos]) != FALSE )
	{
		MsgOutput(aIndex, lMsg.Get(MSGGET(13, 26)));
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		LogAddTD("[ANTI-HACK][Serial 0 Item] [Sell Item] (%s)(%s) Item(%s) Pos(%d)",
			lpObj->AccountID, lpObj->Name, lpObj->pInventory[lpMsg->Pos].GetName(), lpMsg->Pos);

		return;
	}

	if (gc_bBlockFullExcellent == 1) {
		if (lpObj->pInventory[lpMsg->Pos].IsFullExcellent()) {
			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}

	if (gc_bBlockFullAncient == 1) {
		if (lpObj->pInventory[lpMsg->Pos].IsFullAncient()) {
			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}

	if ( lpObj->pInventory[lpMsg->Pos].m_Type == ITEMGET(14,11) ) // Box of Luck
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpObj->pInventory[lpMsg->Pos].m_QuestItem != false) 
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpObj->pInventory[lpMsg->Pos].m_Type == ITEMGET(13,20) && lpObj->pInventory[lpMsg->Pos].m_Level == 1) // Wizards Ring +1
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpObj->pInventory[lpMsg->Pos].m_Type == ITEMGET(13,20) && lpObj->pInventory[lpMsg->Pos].m_Level == 2) // Wizards Ring +2
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpObj->pInventory[lpMsg->Pos].m_Type == ITEMGET(12,26) &&
		(lpObj->pInventory[lpMsg->Pos].m_Level == 1 ||
		 lpObj->pInventory[lpMsg->Pos].m_Level == 2 || 
		 lpObj->pInventory[lpMsg->Pos].m_Level == 3 || 
		 lpObj->pInventory[lpMsg->Pos].m_Level == 4 || 
		 lpObj->pInventory[lpMsg->Pos].m_Level == 5 ))
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(&lpObj->pInventory[lpMsg->Pos]) == TRUE )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(13, 37)), lpObj->m_Index, 1);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( IsCashItem(lpObj->pInventory[lpMsg->Pos].m_Type) == TRUE 
		&& !lpObj->pInventory[lpMsg->Pos].m_bExpired )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpObj->pInventory[lpMsg->Pos].m_Type == ITEMGET(14,12) && lpObj->pInventory[lpMsg->Pos].m_Level == 1) // Heart+1
	{
		pResult.Result = true;
		pResult.Money = lpObj->Money;
		gObjInventoryItemSet(aIndex, lpMsg->Pos, -1);
		lpObj->pInventory[lpMsg->Pos].Clear();
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

#if ( __CUSTOM__ == 1)

	if ( lpObj->pInventory[lpMsg->Pos].m_bLOCKED )
	{
		DataSend(lpObj->m_Index,(LPBYTE)&pResult,pResult.h.size);
		return;
	}

#endif

	if ( lpObj->pInventory[lpMsg->Pos].IsItem() == FALSE )
	{
		pResult.Result = false;
		pResult.Money = lpObj->Money;
		LogAdd(lMsg.Get(MSGGET(1, 228)), __FILE__, __LINE__, lpObj->Name, lpMsg->Pos);
		LogAdd("ID:%s CharID:%s ", lpObj->AccountID, lpObj->Name);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

	}
	else
	{
		BYTE NewOption[MAX_EXOPTION_SIZE];
		DWORD iAddZen = 0;
		int type = lpObj->pInventory[lpMsg->Pos].m_Type;
		BYTE Divisor;

		if ( type == ITEMGET(13,4) || type == ITEMGET(13,5)) // Pets
			lpObj->pInventory[lpMsg->Pos].PetValue();
		else
			lpObj->pInventory[lpMsg->Pos].Value();

		
		if( g_ItemPrice.GetPrice(tShop, type, lpObj->pInventory[lpMsg->Pos].m_Level, iAddZen, Divisor) )
		{
			iAddZen /= Divisor;
		}
		else
		{
			iAddZen = lpObj->pInventory[lpMsg->Pos].m_SellMoney;
		}

		if ( gObjCheckMaxZen(aIndex, iAddZen) == FALSE )
		{
			DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		lpObj->Money += iAddZen;
		pResult.Result =true;
		pResult.Money = lpObj->Money;
		
		ItemIsBufExOption(NewOption, &lpObj->pInventory[lpMsg->Pos]);

		LogAddTD(lMsg.Get(MSGGET(1, 229)), lpObj->AccountID, lpObj->Name, tShop,
			iAddZen, lpObj->pInventory[lpMsg->Pos].GetName(), 
			lpObj->pInventory[lpMsg->Pos].m_Level, lpObj->pInventory[lpMsg->Pos].m_Option1,
			lpObj->pInventory[lpMsg->Pos].m_Option2, lpObj->pInventory[lpMsg->Pos].m_Option3,
			lpObj->pInventory[lpMsg->Pos].m_Number, (int)lpObj->pInventory[lpMsg->Pos].m_Durability,
			NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6], 
			lpObj->pInventory[lpMsg->Pos].m_SetOption);


		g_ElementalSystem.ClearErtel(lpObj,&lpObj->pInventory[lpMsg->Pos]);
		::gObjInventoryDeleteItem(aIndex, lpMsg->Pos);

		if ( lpMsg->Pos == 10 || lpMsg->Pos == 11 )
		{
			::gObjUseSkill.SkillChangeUse(aIndex);
		}

		::gObjMakePreviewCharSet(aIndex);
	}

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
}



int GetNeedMoneyItemDurRepaire(CItem *  DurItem, BOOL RequestPos)
{
	int iMoney = 0;
	int type = DurItem->m_Type;
	float ItemDurability = DurItem->m_Durability;
	float BaseDurability = DurItem->m_BaseDurability;

	if ( type == ITEMGET(13,10) ) // Transformation Ring
		return 0;

	if ( ::HasItemDurability(type ) != FALSE )
	{
		float lc4;
		float lc5 = 0;
		lc4 = DurItem->m_BaseDurability;

		if ( ItemDurability == lc4 )
			return 0;

		float lc6 = 1.0f - ItemDurability / lc4;
		int lc7;

		if ( type == ITEMGET(13,4)|| type ==ITEMGET(13,5) ) // Pets
			lc7 = DurItem->m_BuyMoney;
		else
			lc7 = DurItem->m_BuyMoney/3;

		if ( lc7 > 400000000 ) lc7 = 400000000;

		if ( lc7 >= 1000 )
			lc7 = lc7/100*100;
		else if ( lc7 >= 100 )
			lc7 = lc7/10*10;

		float lc8 = (float)sqrt(double(lc7));
		float lc9 = (float)sqrt(sqrt(double(lc7))); 
		lc5 = 3.0f * lc8 * lc9;
		lc5 *= lc6;
		lc5 += 1.0f;

		if ( ItemDurability <= 0.0f )
		{
			if ( type == ITEMGET(13,4)|| type ==ITEMGET(13,5) ) // Pets
				lc5 *= 2;
			else
				lc5 *= ::GetAllRepairItemRate(type);

		}

		if ( RequestPos == TRUE )
		{
			lc5 += lc5 * 1.5f;
		}

		iMoney = (int)lc5;

		if ( iMoney >= 1000 )
			iMoney = iMoney/100*100;
		else if ( iMoney >= 100 )
			iMoney = iMoney/10*10;
		
	}
	else
	{
		iMoney = -1;
	}

	return iMoney;
}




struct PMSG_ITEMDURREPAIR_RESULT
{
	PBMSG_HEAD h;	// C1:34
	int Money;	// 4
};




void ItemDurRepaire(LPOBJ lpObj, CItem * DurItem, int pos, int RequestPos)
{
	PMSG_ITEMDURREPAIR_RESULT pResult;
	int result = TRUE;

	PHeadSetB((LPBYTE)&pResult, 0x34, sizeof(pResult));
	int itemtype = DurItem->m_Type;

	if ( lpObj->m_IfState.type != 13 )
	{
		if ( itemtype ==ITEMGET(13,4)  || itemtype == ITEMGET(13,5))
		{
			pResult.Money = 0;
			DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

			return;
		}
	}

	if( DurItem->m_bLuckySet == TRUE )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (itemtype == ITEMGET(13,64) || 
		itemtype == ITEMGET(13,65) ||
		itemtype == ITEMGET(13,67))
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( itemtype == ITEMGET(13,80) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( itemtype == ITEMGET(13,106) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( itemtype == ITEMGET(13,123) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if( itemtype ==ITEMGET(4,7) ||
		itemtype ==ITEMGET(4,15) ||
		itemtype == ITEMGET(13,10) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if( itemtype == ITEMGET(0,41) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	
	if ( itemtype >= ITEMGET(14,0) ||
		( itemtype >= ITEMGET(13,0) && itemtype  < ITEMGET(13,4) ) ||
		( itemtype >= ITEMGET(12,7) && itemtype < ITEMGET(12,36) ) ||
		( itemtype > ITEMGET(12,43) && itemtype < ITEMGET(12,49) ) ||
		( itemtype > ITEMGET(12,50) && itemtype < ITEMGET(12,262) ) ||
		( itemtype > ITEMGET(12,267) && itemtype < ITEMGET(13,0) ) )
	{
#if (CUSTOM_WINGS == 1)
		if( itemtype >= ITEMGET(12,440) && itemtype <= ITEMGET(12,445) )
		{

		}
		else
		{
			pResult.Money = 0;
			DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
#else
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
#endif
	}

	if ( itemtype == ITEMGET(13,38) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if ( itemtype == ITEMGET(13,39) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if ( itemtype == ITEMGET(13,40) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if ( itemtype == ITEMGET(13,41) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if ( itemtype == ITEMGET(13,42) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if ( itemtype == ITEMGET(13,68) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if ( itemtype == ITEMGET(13,66) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if ( itemtype == ITEMGET(13,76) || itemtype == ITEMGET(13,77) || itemtype == ITEMGET(13,78))
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}


	if ( itemtype == ITEMGET(13,122) )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	pResult.Money = GetNeedMoneyItemDurRepaire(DurItem, RequestPos);

	if ( pResult.Money <= 0 )
	{
		pResult.Money = 0;
		DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if ( (lpObj->Money - pResult.Money) < 1 )
		pResult.Money = 0;
	else
	{
		lpObj->Money -= pResult.Money;
		pResult.Money = lpObj->Money;
		DurItem->m_Durability = (float)((int)DurItem->m_BaseDurability);
		DurItem->Convert(DurItem->m_Type, DurItem->m_Option1, DurItem->m_Option2, DurItem->m_Option3,
			DurItem->m_NewOption, DurItem->m_SetOption,DurItem->m_ItemOptionEx,0,-1, CURRENT_DB_VERSION);
		GCItemDurSend(lpObj->m_Index, pos, (BYTE)DurItem->m_Durability, FALSE);
	}

	DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
}






void CGModifyRequestItem(PMSG_ITEMDURREPAIR * lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	PMSG_ITEMDURREPAIR_RESULT pResult;

	PHeadSetB((LPBYTE)&pResult, 0x34, sizeof(pResult));
	pResult.Money = 0;


	if ( gObj[aIndex].CloseType != -1 )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( !PacketCheckTime(lpObj))
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Requestpos == 1 && gObj[aIndex].Level < 50 )
	{
		pResult.Money = 0;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpMsg->Position == 0xFF )
	{
		for ( int n=0;n<MAIN_INVENTORY_SIZE;n++)
		{
			if ( lpObj->pInventory[n].IsItem())
			{
				if ( IsCashItem(lpObj->pInventory[n].m_Type ) == TRUE )
					continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,20) && (lpObj->pInventory[n].m_Level == 0 ||lpObj->pInventory[n].m_Level == 1 ))
					continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,20) && lpObj->pInventory[n].m_Level == 2 )
					continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,4) || lpObj->pInventory[n].m_Type == ITEMGET(13,5) )
					continue;
				
				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,37) )
					continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,38) )
					continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,39) )
					continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,40) )
					continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,68) )
					continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,76) || lpObj->pInventory[n].m_Type == ITEMGET(13,77) || lpObj->pInventory[n].m_Type == ITEMGET(13,78)) 
					continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(12,26) &&
					(lpObj->pInventory[n].m_Level == 1 ||
					 lpObj->pInventory[n].m_Level == 2 || 
					 lpObj->pInventory[n].m_Level == 3 || 
					 lpObj->pInventory[n].m_Level == 4 || 
					 lpObj->pInventory[n].m_Level == 5 ))
					 continue;

				if ( lpObj->pInventory[n].m_Type == ITEMGET(13,107) )
					 continue;

				if ( lpObj->pInventory[n].m_bLuckySet == TRUE )
					continue;
					
				ItemDurRepaire(lpObj,&lpObj->pInventory[n] , n, lpMsg->Requestpos);
			}
		}

		gObjCalCharacter(lpObj->m_Index);
		return;
	}

	if ( lpMsg->Position > MAIN_INVENTORY_SIZE-1)
	{
		pResult.Money = 0;
		return;
	}

	if ( IsCashItem(lpObj->pInventory[lpMsg->Position].m_Type ) == TRUE )
		return;


	if ( lpObj->pInventory[lpMsg->Position].m_Type == ITEMGET(13,20) && (lpObj->pInventory[lpMsg->Position].m_Level == 0 ||lpObj->pInventory[lpMsg->Position].m_Level == 1 ))
		return;

	if ( lpObj->pInventory[lpMsg->Position].m_Type == ITEMGET(13,20) && lpObj->pInventory[lpMsg->Position].m_Level == 2 )
		return;

	if ( lpObj->pInventory[lpMsg->Position].m_Type == ITEMGET(13,37) )
		return;

	if ( lpObj->pInventory[lpMsg->Position].m_Type == ITEMGET(13,38) )
		return;

	if ( lpObj->pInventory[lpMsg->Position].m_Type == ITEMGET(13,107) )
		return;

	if ( lpObj->pInventory[lpMsg->Position].m_bLuckySet == TRUE )
		return;

	ItemDurRepaire(lpObj, &lpObj->pInventory[lpMsg->Position], lpMsg->Position, lpMsg->Requestpos);
	gObjCalCharacter(lpObj->m_Index);
}





struct PMSG_TRADE_REQUESTSEND
{
	PBMSG_HEAD h;	// C3:36
	char szId[10];	// 3
};



void CGTradeRequestSend(PMSG_TRADE_REQUEST * lpMsg, int aIndex)
{
	int number;

	if (bCanTrade == FALSE )
	{
		::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 125)), aIndex, 1);
		::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 126)), aIndex, 1);
		return;
	}

	if ( gObj[aIndex].CloseType != -1 )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if ( !PacketCheckTime(lpObj))
		return;

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if ( OBJMAX_RANGE(number) == FALSE )
	{
		LogAdd("error : %s %d (%d)", __FILE__, __LINE__, number);
		return;
	}

	if ( number == aIndex )
		return;

	if ( !gObjIsConnected(number))
		return;

	if ( gObj[number].Type == OBJ_MONSTER )
		return;

	if ( gObj[number].CloseCount >= 0 )
		return;

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsDuel(gObj[aIndex]) || g_NewPVP.IsDuel(gObj[number])) {
		 GCServerMsgStringSend(lMsg.Get(MSGGET(2, 199)), aIndex, 1);
		 return;
	}

	if (g_NewPVP.IsObserver(gObj[aIndex])) {
		 GCServerMsgStringSend(lMsg.Get(MSGGET(2, 199)), aIndex, 1);
		 return;
	}
#endif

	if ( DS_MAP_RANGE(gObj[number].MapNumber) != FALSE )
	{
		::GCServerMsgStringSend(lMsg.Get(MSGGET(2, 199)), aIndex, 1);
		return;
	}

	if ( IT_MAP_RANGE(gObj[number].MapNumber) != FALSE )
	{
		::GCServerMsgStringSend("You cannot trade inside Illusion Temple.", aIndex, 1);
		return;
	}

	if ( DG_MAP_RANGE(gObj[number].MapNumber) != FALSE )
	{
		::GCServerMsgStringSend("You cannot trade inside Doppelganger.", aIndex, 1);
		return;
	}

	if ( IF_MAP_RANGE(gObj[number].MapNumber) != FALSE )
	{
		::GCServerMsgStringSend("You cannot trade inside Imperial Fort.", aIndex, 1);
		return;
	}

	if ( gObj[aIndex].m_bPShopOpen == true )
		return;

	if ( gObj[number].m_bPShopOpen == true )
		return;

	if ( CC_MAP_RANGE(gObj[number].MapNumber) != FALSE )
	{
		::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 196)), aIndex, 1);
		return;
	}

	if ( BC_MAP_RANGE(gObj[aIndex].MapNumber) != FALSE )
	{
		if ( g_BloodCastle.GetCurrentState(g_BloodCastle.GetBridgeLevel(gObj[aIndex].MapNumber)) != 1 || g_BloodCastle.CheckCanEnter(gObj[aIndex].MapNumber-MAP_INDEX_BLOODCASTLE1) == false )
		{
			::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 188)), aIndex, 1);
			return;
		}	
	}

	if ( gObj[aIndex].MapNumber == MAP_INDEX_PKFIELD )
	{
		::GCServerMsgStringSend("Trade not allowed on duel arena", aIndex, 1);
		return;
	}

	if ( (gObj[number].m_Option &1) != 1 )
	{
		::GCTradeResponseSend(0, aIndex, gObj[number].Name, 0, 0);
		return;
	}

	if ( lpObj->m_IfState.use > 0 )
	{
		::GCTradeResponseSend(3, aIndex, gObj[number].Name, 0, 0);
		return;
	}

	if (gObj[number].m_IfState.use > 0 )
	{
		::GCTradeResponseSend(2, aIndex, gObj[number].Name, 0, 0);
		return;
	}

	if ( ::gObjFixInventoryPointer(aIndex) == false )
	{
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	if ( gObj[aIndex].pTransaction == 1 )
	{
		LogAddTD("[%s][%s] CGTradeRequestSend() Failed : Transaction == 1, IF_TYPE : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);

		return;
	}

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.state = 0;
	lpObj->m_IfState.type = 1;
	lpObj->TargetNumber = number;
	gObj[number].m_IfState.use = 1;
	gObj[number].m_IfState.state = 0;
	gObj[number].m_IfState.type = 1;
	gObj[number].TargetNumber = aIndex;
	lpObj->m_InterfaceTime = GetTickCount();
	gObj[number].m_InterfaceTime = GetTickCount();

	PMSG_TRADE_REQUESTSEND pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x36;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szId, gObj[aIndex].Name, sizeof(pMsg.szId));

	DataSend(number, (LPBYTE)&pMsg, pMsg.h.size);

	LogAddTD(lMsg.Get(MSGGET(1, 230)), gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].Ip_addr,
		gObj[number].AccountID, gObj[number].Name, gObj[number].Ip_addr);
}




void CGTradeResponseRecv(PMSG_TRADE_RESPONSE * lpMsg, int aIndex)
{
	BOOL Result = TRUE;
	int number = gObj[aIndex].TargetNumber;

	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("error-L1 : %d (A_ID:%s) %s %d", number, gObj[aIndex].AccountID, __FILE__, __LINE__);
		Result = FALSE;
		lpMsg->Response = false;
	}

	if ( OBJMAX_RANGE(number) == FALSE )
	{
		if ( gObj[aIndex].m_IfState.use != FALSE )
		{
			if ( gObj[aIndex].m_IfState.type == 1 )
			{
				gObj[aIndex].m_IfState.use = FALSE;
				gObj[aIndex].TargetNumber = -1;
			}
		}

		LogAdd("error-L1 : target:%d (A_ID:%s) %s %d", number, gObj[aIndex].AccountID, __FILE__, __LINE__);
		Result = FALSE;
		lpMsg->Response = false;
	}

	if ( gObjIsConnected(number) == FALSE )
	{
		Result = FALSE;
		lpMsg->Response = false;
	}
	
	if ( gObj[aIndex].CloseType != -1 )
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if ( gObj[aIndex].m_bPShopOpen == true )
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if ( gObj[number].m_bPShopOpen == true )
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if ( gObj[aIndex].X < (gObj[number].X -2 ) || gObj[aIndex].X > (gObj[number].X +2 ) || gObj[aIndex].Y < (gObj[number].Y -2 ) || gObj[aIndex].Y > (gObj[number].Y +2 ) )
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if ( gObj[aIndex].m_IfState.use == FALSE || gObj[aIndex].m_IfState.type != 1 || gObj[aIndex].m_IfState.state != 0 )
	{
		LogAddTD("(%s)(%s) Trade Interface State Error : use:%d type:%d", gObj[aIndex].AccountID, gObj[aIndex].Name, 
			gObj[aIndex].m_IfState.use, gObj[aIndex].m_IfState.type);

		return;
	}

	if ( gObj[number].m_IfState.use == FALSE || gObj[number].m_IfState.type != 1 || gObj[number].m_IfState.state != 0 )
	{
		LogAddTD("(%s)(%s) Trade Interface State Error : use:%d type:%d", gObj[number].AccountID, gObj[number].Name, 
			gObj[number].m_IfState.use, gObj[number].m_IfState.type);

		return;
	}

	if ( gObjFixInventoryPointer(aIndex) == false )
	{
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	if ( gObj[aIndex].pTransaction == 1 )
	{
		LogAddTD("[%s][%s] CGTradeResponseRecv() Failed : Transaction == 1, IF_TYPE : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);

		return;
	}

	if ( lpMsg->Response == false )
	{
		GCTradeResponseSend(lpMsg->Response, number, gObj[aIndex].Name, 0, 0);
		Result = FALSE;
	}
	else
	{
		Result = TRUE;

		for ( int i=0;i<TRADE_BOX_SIZE;i++)
		{
			gObj[aIndex].Trade[i].Clear();
			gObj[number].Trade[i].Clear();
		}

		memset(gObj[aIndex].TradeMap, (BYTE)-1, TRADE_BOX_MAP_SIZE );
		memset(gObj[number].TradeMap, (BYTE)-1, TRADE_BOX_MAP_SIZE );
		gObj[aIndex].m_IfState.state = 1;
		gObj[number].m_IfState.state = 1;
		gObj[aIndex].TradeMoney = 0;
		gObj[number].TradeMoney = 0;

		if (::gObjInventoryTrans(aIndex) == FALSE )
			Result = 2;

		if (::gObjInventoryTrans(number) == FALSE )
			Result = 3;

		if ( Result != TRUE )
		{
			lpMsg->Response = false;
			GCTradeResponseSend(lpMsg->Response, number, gObj[aIndex].Name, 0, 0);
			GCTradeResponseSend(lpMsg->Response, aIndex, gObj[number].Name, 0, 0);

			if ( Result == 2 )
			{
				LogAddTD("[%s][%s] Made Trade Error (in Transaction) [%s][%s]",
					gObj[aIndex].AccountID, gObj[aIndex].Name,
					gObj[number].AccountID, gObj[number].Name);
			}
			else if ( Result == 3 )
			{
				LogAddTD("[%s][%s] Made Trade Error (in Transaction) [%s][%s]",
					gObj[number].AccountID, gObj[number].Name,
					gObj[aIndex].AccountID, gObj[aIndex].Name);
			}

			Result = FALSE;
		}
		else
		{
			lpMsg->Response = true;
			GCTradeResponseSend(lpMsg->Response, number, gObj[aIndex].Name, gObj[aIndex].Level, gObj[aIndex].GuildNumber);
			GCTradeResponseSend(lpMsg->Response, aIndex, gObj[number].Name, gObj[number].Level, gObj[number].GuildNumber);

			LogAddTD("[%s][%s] Trade Ready [%s][%s]", 
				gObj[number].AccountID, gObj[number].Name,
				gObj[aIndex].AccountID, gObj[aIndex].Name);
		}
	}

	if ( Result == FALSE )
	{
		if ( gObj[aIndex].m_IfState.use != FALSE && gObj[aIndex].m_IfState.type == 1 )
		{
			gObj[aIndex].m_IfState.use = FALSE;
			gObj[aIndex].TargetNumber = -1;
			LogAddTD("Interface State : %d", gObj[aIndex].m_IfState.use);
		}

		if ( gObj[number].TargetNumber == aIndex )
		{
			if ( gObj[number].m_IfState.use != FALSE && gObj[number].m_IfState.type == 1 )
			{
				gObj[number].m_IfState.use = FALSE;
				gObj[number].TargetNumber = -1;
				LogAddTD("Interface State : %d", gObj[number].m_IfState.use);
			}
		}
	}
}






BOOL GCTradeResponseSend(BYTE response, int aIndex, LPSTR id, WORD level, int GuildNumber)
{
	PMSG_TRADE_RESPONSE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x37, sizeof(pMsg));
	pMsg.Response = response;
	pMsg.Level = level;
	pMsg.GuildNumber = GuildNumber;
	memcpy(pMsg.Id, id, sizeof(pMsg.Id));

	return DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}


struct PMSG_TRADE_OTHER_DEL
{
	PBMSG_HEAD h;	// C1:38
	BYTE TradeItemIndex;	// 3
};


int GCTradeOtherDel(int aIndex, BYTE tradeindex)
{
	PMSG_TRADE_OTHER_DEL pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x38, sizeof(pMsg ));
	pMsg.TradeItemIndex = tradeindex;

	return DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}


struct PMSG_TRADE_OTHER_MOVE
{
	PBMSG_HEAD h;	// C1:39
	BYTE TradeItemIndex;	// 3
	BYTE ItemInfo[MAX_ITEM_INFO];	// 4
};



int GCTradeOtherAdd(int aIndex, BYTE tradeindex, LPBYTE iteminfo)
{
	PMSG_TRADE_OTHER_MOVE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x39, sizeof(pMsg));
	pMsg.TradeItemIndex = tradeindex;
	pMsg.ItemInfo[I_TYPE] = iteminfo[I_TYPE];
	pMsg.ItemInfo[I_OPTION] = iteminfo[I_OPTION];
	pMsg.ItemInfo[I_DUR] = iteminfo[I_DUR];
	pMsg.ItemInfo[I_NOPTION] = iteminfo[I_NOPTION];
	pMsg.ItemInfo[I_SOPTION] = iteminfo[I_SOPTION];
	pMsg.ItemInfo[5] = iteminfo[5];
	pMsg.ItemInfo[6] = iteminfo[6];

	pMsg.ItemInfo[7] = iteminfo[7];
	pMsg.ItemInfo[8] = iteminfo[8];
	pMsg.ItemInfo[9] = iteminfo[9];
	pMsg.ItemInfo[10] = iteminfo[10];
	pMsg.ItemInfo[11] = iteminfo[11];
	
	return DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}





void CGTradeMoneyRecv(PMSG_TRADE_GOLD * lpMsg, int aIndex)
{
	PMSG_DEFRESULT pMsg;
	int money = 0;
	int number;

	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	if ( !OBJMAX_RANGE(aIndex) )
	{
		LogAdd("error : %s %d (%d)", __FILE__, __LINE__, aIndex);
		return;
	}

	if ( gObj[aIndex].CloseType != -1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 231)), gObj[aIndex].AccountID, gObj[aIndex].Name);
		return;
	}

	number = gObj[aIndex].TargetNumber;

	if ( !OBJMAX_RANGE(number) )
	{
		LogAdd(lMsg.Get(MSGGET(1, 232)), gObj[aIndex].AccountID, gObj[aIndex].Name, number);
		return;
	}

	if ( gObj[number].CloseType != -1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 233)), gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[number].AccountID, gObj[number].Name);
		return;
	}

	if ( gObj[aIndex].m_IfState.use == FALSE && gObj[aIndex].m_IfState.type != 1 )
		return;

	if ( ::gObjCanItemTouch(&gObj[aIndex], 1) == FALSE )
		return;

	if ( lpMsg->Money > 1000000000 )
		return;

	money = lpMsg->Money;

	if ( (gObj[aIndex].Money - money) < 0 )
		return;

	GCMoneySend(aIndex, gObj[aIndex].Money - money);

	PHeadSetB((LPBYTE)&pMsg, 0x3A, sizeof(pMsg));
	pMsg.result = 1;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

	if ( number >= 0)
	{
		gObj[aIndex].TradeMoney = money;
		gObj[aIndex].TradeOk  = false;
		gObj[number].TradeOk  = false;

		GCTradeOkButtonSend(number, 2);
		GCTradeOkButtonSend(aIndex, 0);
		GCTradeMoneyOther(number, gObj[aIndex].TradeMoney);
	}
}



struct PMSG_TRADE_OTHER_GOLD
{
	PBMSG_HEAD h;	// C1:3B
	DWORD Money;	// 4
};



BOOL GCTradeMoneyOther(int aIndex, DWORD money)
{
	PMSG_TRADE_OTHER_GOLD pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x3B, sizeof(pMsg));
	pMsg.Money = money;

	return DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);	
}




BOOL GCTradeOkButtonSend(int aIndex, BYTE flag)
{
	PMSG_TRADE_OKBUTTON pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x3C, sizeof(pMsg));
	pMsg.Flag = flag;

	return DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}



void CGTradeOkButtonRecv(PMSG_TRADE_OKBUTTON * lpMsg, int aIndex)
{
	int number = gObj[aIndex].TargetNumber;

	if ( number < 0 || number > OBJMAX-1 )
		return;

	if ( !gObjIsConnected(&gObj[number]))
		return;

	if ( gObj[aIndex].m_IfState.use < 1 || gObj[aIndex].m_IfState.state != 1 )
		return;

	if ( gObj[number].m_IfState.use < 1 || gObj[number].m_IfState.state != 1 )
		return;

	if ( gObj[aIndex].CloseType != -1 )
		return;

	if ( gObj[number].CloseType != -1 )
		return;
	
	if ( lpMsg->Flag == 1 )
	{
		if ( gObj[aIndex].TradeOk == false )
		{
			gObj[aIndex].TradeOk = true;
			LogAddTD("[%s][%s] Trade Accept", gObj[aIndex].AccountID, gObj[aIndex].Name);
			GCTradeOkButtonSend(number, 1);
		}
	}
	else if ( lpMsg->Flag == 0 )
	{
		if ( gObj[aIndex].TradeOk  == true )
		{
			GCTradeOkButtonSend(number, 0);
			gObj[aIndex].TradeOk = false;
		}
	}

	gObjTradeOkButton(aIndex);
}



void CGTradeCancelButtonRecv(int aIndex)
{
	int number = gObj[aIndex].TargetNumber;
	gObjTradeCancel(aIndex);
	CGTradeResult(aIndex, 0);

	if ( number >= 0 )
	{
		gObjTradeCancel(number);
		CGTradeResult(number, 0);
	}
}



struct PMSG_TRADE_RESULT
{
	PBMSG_HEAD h;	// C1:3D
	BYTE Result;	// 3
};



void CGTradeResult(int aIndex , BYTE result )
{
	PMSG_TRADE_RESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x3D, sizeof(pMsg));
	pMsg.Result = result;

	DataSend(aIndex, (unsigned char*)&pMsg, pMsg.h.size);
}






struct CG_REQ_PSHOP_SET_ITEM_PRICE
{
	PHEADSUBB PHeader;
	BYTE btSourcePos;
	BYTE dwItemPrice[4];
};



void CGPShopReqSetItemPrice(PMSG_REQ_PSHOP_SETITEMPRICE * lpMsg, int aIndex)
{
	if ( gDoPShopOpen  == FALSE )
	{
		CGPShopAnsSetItemPrice(aIndex, 0, lpMsg->btItemPos);
		return;
	}

	if ( !gObjIsConnected(aIndex))
	{
		LogAddTD("[PShop] ERROR : Index is not CONNECTED : %d", aIndex);
		return;
	}

	if ( gObj[aIndex].Type != OBJ_USER )
	{
		LogAddTD("[PShop] ERROR : Index is not CHARACTER : %d", aIndex);
		return;
	}

	if ( gObj[aIndex].Level <= 5 )
	{
		LogAddTD("[PShop] [%s][%s] ERROR : Level is Under 6 : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].Level);
		CGPShopAnsSetItemPrice(aIndex, 5, lpMsg->btItemPos);

		return;
	}

	if ( INVENTORY_RANGE(lpMsg->btItemPos) == FALSE )
	{
		LogAddTD("[PShop] [%s][%s] ERROR : Item Position Out of Bound : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, lpMsg->btItemPos);
		CGPShopAnsSetItemPrice(aIndex, 2, lpMsg->btItemPos);

		return;
	}

	if ( gObj[aIndex].Inventory1[lpMsg->btItemPos].IsItem() == FALSE )
	{
		LogAddTD("[PShop] [%s][%s] ERROR : Item Does Not Exist : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, lpMsg->btItemPos);
		CGPShopAnsSetItemPrice(aIndex, 3, lpMsg->btItemPos);

		return;
	}

	CItem * sitem = &gObj[aIndex].Inventory1[lpMsg->btItemPos];
	int iItemPrice = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->sItemPrice4, lpMsg->sItemPrice3), MAKE_NUMBERW(lpMsg->sItemPrice2, lpMsg->sItemPrice1));
	short aBlessCnt = MAKE_NUMBERW(lpMsg->sBlessCnt2,lpMsg->sBlessCnt1);
	short aSoulCnt = MAKE_NUMBERW(lpMsg->sSoulCnt2,lpMsg->sSoulCnt1);
	short aChaosCnt = MAKE_NUMBERW(lpMsg->sChaosCnt2,lpMsg->sChaosCnt1);


	if ( gObjCheckSerial0ItemList(sitem) != FALSE )
	{
		MsgOutput(aIndex, lMsg.Get(MSGGET(13, 26)));
		GCReFillSend(aIndex, (WORD) gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);

		LogAddTD("[ANTI-HACK][Serial 0 Item] [PShop] (%s)(%s) Item(%s) Pos(%d)",
			gObj[aIndex].AccountID, gObj[aIndex].Name, sitem->GetName(), lpMsg->btItemPos);

		return;
	}


	if ( gObjInventorySearchSerialNumber(&gObj[aIndex], sitem->GetNumber()) == FALSE )
	{
		GCReFillSend(aIndex,(WORD)gObj[aIndex].Life, 0xFD, TRUE, gObj[aIndex].iShield);
		LogAdd("error-L2 : CopyItem [%s][%s] return %s %d", gObj[aIndex].AccountID, gObj[aIndex].Name, __FILE__, __LINE__);

		return;
	}

	if ( iItemPrice < 0 || aBlessCnt < 0 || aSoulCnt < 0 || aChaosCnt < 0 )
	{
		LogAddTD("[PShop] [%s][%s] ERROR : Personal Shop Item Price <= 0 : %d ",
			gObj[aIndex].AccountID, gObj[aIndex].Name, iItemPrice);
		CGPShopAnsSetItemPrice(aIndex, 4, lpMsg->btItemPos);
		return;
	}

	if ( (gObj[aIndex].Penalty&4) == 4 || (gObj[aIndex].Penalty&8) == 8 )
	{
		LogAddTD("[PShop] [%s][%s] ERROR : Item Price Change - Item Block",gObj[aIndex].AccountID, gObj[aIndex].Name,sitem->m_iPShopValue);	// Deathway fix Here - Last parameter can cause crash
		CGPShopAnsSetItemPrice(aIndex, 6	, lpMsg->btItemPos);

		return;
	}

	if ( sitem->m_Type < 0 || sitem->m_Type >= ITEMGET(16,0))
	{
		LogAddTD("[PShop] [%s][%s] ERROR : Item Outof Bound",gObj[aIndex].AccountID,gObj[aIndex].Name);
		return;
	}

	sitem->m_iPShopValue = iItemPrice;
	sitem->m_PShopBlessValue = aBlessCnt;
	sitem->m_PShopSoulValue = aSoulCnt;
	sitem->m_PShopChaosValue = aChaosCnt;

	ItemAttribute[sitem->m_Type].Name[31] = 0;	// Zero String Terminated

	LogAddTD("[PShop] [%s][%s] Changing Item Price : %d (B: %d, S: %d, C: %d) (Type:%d (%s), Lev:%d, Serial:%d, Dur:%d",
		gObj[aIndex].AccountID, gObj[aIndex].Name, sitem->m_iPShopValue,
		sitem->m_PShopBlessValue,sitem->m_PShopSoulValue,sitem->m_PShopChaosValue,sitem->m_Type, 
		ItemAttribute[sitem->m_Type].Name, sitem->m_Level, sitem->m_Number,
		sitem->m_Durability);

	CGPShopAnsSetItemPrice(aIndex, 1, lpMsg->btItemPos);
}




struct PMSG_ANS_PSHOP_SETITEMPRICE
{
	PBMSG_HEAD2 h;	// C1:3F:01
	BYTE btResult;	// 4
	BYTE btItemPos;	// 5
};


void CGPShopAnsSetItemPrice(int aIndex, BYTE btResult, BYTE btItemPos)
{
	PMSG_ANS_PSHOP_SETITEMPRICE pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x3F, 0x01, sizeof(pMsg));
	pMsg.btResult = btResult;
	pMsg.btItemPos = btItemPos;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}





struct PMSG_ANS_PSHOP_TEXT_CHANGED
{
	PBMSG_HEAD2 h;	// C1:3F:10
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE btPShopText[36];	// 6
	BYTE btName[10];	// 2A
};


void CGPShopReqOpen(PMSG_REQ_PSHOP_OPEN * lpMsg, int aIndex)
{
	if ( ::gDoPShopOpen == FALSE )
		return;

	if ( !gObjIsConnected(aIndex))
	{
		LogAddTD("[PShop] ERROR : Index is not CONNECTED : %d",aIndex);
		return;
	}

	if ( gObj[aIndex].Type != OBJ_USER )
	{
		LogAddTD("[PShop] ERROR : Index is not CHARACTER : %d", aIndex);
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsDuel(gObj[aIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3429), aIndex, 1);
		 return;
	}

	if (g_NewPVP.IsObserver(gObj[aIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3430), aIndex, 1);
		 return;
	}
#endif

	if ( gObj[aIndex].Level <= 5 )
	{
		LogAddTD("[PShop] [%s][%s] ERROR : Level is Under 6 : %d",
			gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].Level);
		::CGPShopAnsOpen( aIndex, 2);

		return;
	}

	if ( (gObj[aIndex].Penalty&4) == 4 || (gObj[aIndex].Penalty&8) == 8 )
	{
		LogAddTD("[PShop] [%s][%s] ERROR : Cant't Open Shop - Item Block" ,gObj[aIndex].AccountID, gObj[aIndex].Name);
		::CGPShopAnsOpen( aIndex, 3);

		return;
	}	

	if(!pShop.CanOpenShopAtMap(gObj[aIndex].MapNumber))
	{
		::GCServerMsgStringSend("You cannot open a shop in this map", aIndex, 1);
		::CGPShopAnsOpen( aIndex, 0);
		return;
	}

	/*
	if ( CC_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 197)), aIndex, 1);
		::CGPShopAnsOpen( aIndex, 0);

		return;
	}

	if ( IT_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		::GCServerMsgStringSend("You cannot open your Personal Shop within Illusion Temple.", aIndex, 1);
		::CGPShopAnsOpen( aIndex, 0);
		return;
	}

	if ( DG_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		::GCServerMsgStringSend("You cannot open your Personal Shop within Doppelganger.", aIndex, 1);
		::CGPShopAnsOpen( aIndex, 0);
		return;
	}

	if ( IF_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		::GCServerMsgStringSend("You cannot open your Personal Shop within Imperial Fort.", aIndex, 1);
		::CGPShopAnsOpen( aIndex, 0);
		return;
	} */

#if (__NOVUS__ == 1)
	if ( gObj[aIndex].MapNumber != MAP_INDEX_LOREN_MARKET )
	{
		::GCServerMsgStringSend("You can open your Personal Shop only in Market!", aIndex, 1);
		::CGPShopAnsOpen( aIndex, 0);
		return;
	}
#endif

	bool bEnablePShopOpen = false;

	if ( gObj[aIndex].m_IfState.use == FALSE )
		bEnablePShopOpen  = true;
	else
	{
		if ( gObj[aIndex].m_IfState.use == TRUE )
		{
			switch ( gObj[aIndex].m_IfState.type )
			{
				case 8:
					bEnablePShopOpen = true; break;
			}

			if ( !bEnablePShopOpen  )
			{
				LogAddTD("[PShop] [%s][%s] ERROR : m_IfState.type is Using : %d", gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);
			}
		}
		else
		{
			LogAddTD("[PShop] [%s][%s] ERROR : Unknown m_IfState.type : %d", gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);
			return;
		}
	}

	if ( !::gObjFixInventoryPointer(aIndex))
	{
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__ ,__LINE__);
	}

	if ( gObj[aIndex].pTransaction == 1 )
	{
		LogAddTD("[PShop] [%s][%s] ERROR : Transaction == 1, IF_TYPE : %d", gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_IfState.type);
		bEnablePShopOpen = false;
	}

	if ( bEnablePShopOpen )
	{
		if ( gObj[aIndex].m_bPShopOpen == false )
		{
			gObj[aIndex].m_bPShopOpen = true;
			memcpy(gObj[aIndex].m_szPShopText, lpMsg->szPShopText, sizeof(lpMsg->szPShopText));
			LogAddTD("[PShop] [%s][%s] Personal Shop Opened", gObj[aIndex].AccountID, gObj[aIndex].Name);
#if( __NOVUS__ == 1 )
			char Text[256];
			ZeroMemory(Text, sizeof(Text));
			sprintf(Text, "'%s' open new shop (%s)!", gObj[aIndex].Name, gObj[aIndex].m_szPShopText);
			AllSendServerMsg(Text);
#endif
		}
		else
		{
			memcpy(gObj[aIndex].m_szPShopText, lpMsg->szPShopText, sizeof(lpMsg->szPShopText));
			
			PMSG_ANS_PSHOP_TEXT_CHANGED pMsg;

			PHeadSubSetB((LPBYTE)&pMsg, 0x3F, 0x10, sizeof(pMsg));
			pMsg.NumberH = SET_NUMBERH(aIndex);
			pMsg.NumberL = SET_NUMBERL(aIndex);
			memcpy(pMsg.btPShopText, lpMsg->szPShopText, sizeof(pMsg.btPShopText));
			memcpy(pMsg.btName, gObj[aIndex].Name, sizeof(pMsg.btName));

			MsgSendV2(&gObj[aIndex], (LPBYTE)&pMsg, pMsg.h.size);

			LogAddTD("[PShop] [%s][%s] Personal Shop Already Opened - Changing PShop Name",
				gObj[aIndex].AccountID, gObj[aIndex].Name);
		}
	}
	else
	{
		CGPShopAnsOpen(aIndex, 0);
		return;
	}
	
	CGPShopAnsOpen(aIndex, 1);
}




struct PMSG_ANS_PSHOP_OPEN
{
	PBMSG_HEAD2 h;	// C1:3F:02
	BYTE btResult;	// 4
};


void CGPShopAnsOpen(int aIndex, BYTE btResult)
{
	PMSG_ANS_PSHOP_OPEN pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x3F, 0x02, sizeof(pMsg));
	pMsg.btResult = btResult;

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}




void CGPShopReqClose(int aIndex)
{
	if ( !gObjIsConnected(aIndex))
	{
		LogAddTD("[PShop] ERROR : Index is not CONNECTED : %d", aIndex);
		return;
	}

	if ( gObj[aIndex].Type != OBJ_USER )
	{
		LogAddTD("[PShop] ERROR : Index is not CHARACTER : %d", aIndex);
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsDuel(gObj[aIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3429), aIndex, 1);
		 return;
	}

	if (g_NewPVP.IsObserver(gObj[aIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3430), aIndex, 1);
		 return;
	}
#endif

	LogAddTD("[PShop] [%s][%s] Requested to Close PShop", gObj[aIndex].AccountID, gObj[aIndex].Name);
	gObj[aIndex].m_bPShopOpen = false;
	memset(gObj[aIndex].m_szPShopText, 0, sizeof(gObj[aIndex].m_szPShopText));
	CGPShopAnsClose(aIndex, 1);
}




struct PMSG_ANS_PSHOP_CLOSE
{
	PBMSG_HEAD2 h;	// C1:3F:03
	BYTE btResult;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
};


void CGPShopAnsClose(int aIndex, BYTE btResult)
{
	LogAddTD("[PShop] [%s][%s] Close PShop", gObj[aIndex].AccountID, gObj[aIndex].Name);

	PMSG_ANS_PSHOP_CLOSE pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x3F, 0x03, sizeof(pMsg));
	pMsg.btResult = btResult;
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));

	if ( btResult == 1 )
		MsgSendV2(&gObj[aIndex], (LPBYTE)&pMsg, pMsg.h.size);
}





void CGPShopReqBuyList(PMSG_REQ_BUYLIST_FROM_PSHOP * lpMsg, int aSourceIndex)
{
	if ( ::gDoPShopOpen == FALSE )
		return;

	if ( gObjIsConnected(MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL)) == FALSE)
	{
		LogAddTD("[PShop] [%s][%s] PShop List Request Failed : Seller did Not CONNECTED (%d)",
			gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name, MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL));
		::CGPShopAnsBuyList(aSourceIndex, -1, 2, 0);
		return;
	}

	LPOBJ lpObj = &gObj[MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL)];

	if ( lpObj->Type != OBJ_USER )
	{
		LogAddTD("[PShop] [%s][%s] PShop List Request Failed : Seller is Not CHARACTER (%d)"
			,gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name,  MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL));
		::CGPShopAnsBuyList(aSourceIndex, -1, 2, 0);
		return;
	}

	if ( lpObj->CloseCount >= 0 )	
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : Seller is Closing (%d)"
			,gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name,  MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL) );
		CGPShopAnsBuyItem(aSourceIndex, -1, 0,2);
		return;
	}

	if ( gObj[aSourceIndex].CloseCount >= 0 )	
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : Buyer is Closing (%d)",
			gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name,  MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL));
		CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsDuel(gObj[aSourceIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3429), aSourceIndex, 1);
		 return;
	}

	if(g_NewPVP.IsObserver(gObj[aSourceIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3430), aSourceIndex, 1);
		 return;
	}
#endif

	if ( lpObj->m_Index == aSourceIndex )
	{
		LogAddTD("[PShop] [%s][%s] PShop List Requested to Him(/Her)Self",
			gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name);
		::CGPShopAnsBuyList(aSourceIndex, -1, 2, 0);
		return;
	}

	if ( lpObj->m_bPShopOpen == false )
	{
		LogAddTD("[PShop] [%s][%s] PShop List Request Failed : [%s][%s] Did not Open PShop",
			gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name,
			lpObj->AccountID, lpObj->Name);
		::CGPShopAnsBuyList(aSourceIndex, -1, 3, 0);
		return;
	}

	if ( (lpObj->Penalty &4) == 4 || (lpObj->Penalty&8) == 8 )
	{
		LogAddTD("[PShop] [%s][%s] PShop List Request Failed : [%s][%s] is in Item Block",
			gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name,
			lpObj->AccountID, lpObj->Name);
		::CGPShopAnsBuyList(aSourceIndex, -1, 4, 0);
		return;
	}

	if ( !::gObjFixInventoryPointer(aSourceIndex))
	{
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	if ( gObj[aSourceIndex].pTransaction == 1 )
	{
		LogAddTD("[PShop] [%s][%s] PShop List Request Failed : Requester Transaction == 1, IF_TYPE : %d",
			gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name, gObj[aSourceIndex].m_IfState.type);
		return;
	}

	LogAddTD("[PShop] [%s][%s] is Receiving PShop List From [%s][%s]",
			gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name,
			lpObj->AccountID, lpObj->Name);

	gObj[aSourceIndex].m_bPShopWantDeal = true;
	gObj[aSourceIndex].m_iPShopDealerIndex = lpObj->m_Index;
	memcpy(gObj[aSourceIndex].m_szPShopDealerName, lpObj->Name, MAX_ACCOUNT_LEN);

	g_ElementalSystem.SendShopElementalItem(lpObj->m_Index,aSourceIndex);
	::CGPShopAnsBuyList(aSourceIndex, lpObj->m_Index, 1, false);
}




struct PMSG_BUYLIST_FROM_PSHOP
{
	BYTE Pos;	// 0
	BYTE ItemInfo[MAX_ITEM_INFO];	// 1
	int PShopItemValue;	// 8
	short PShopBlessValue;
	short PShopSoulValue;
	short PShopChaosValue;
};




struct PMSG_ANS_BUYLIST_FROM_PSHOP
{
	PWMSG_HEAD2 h;	// C2:3F:[05:13]
	BYTE Result;	// 5
	BYTE NumberH;	// 6
	BYTE NumberL;	// 7
	BYTE btName[10];	// 8
	char szPShopText[36];	// 12
	BYTE btCount;	// 36
};


void CGPShopAnsBuyList(int aSourceIndex, int aTargetIndex, BYTE btResult, bool bResend)
{
	if ( gDoPShopOpen == FALSE )
		return;
	

	PMSG_ANS_BUYLIST_FROM_PSHOP pMsgILC;	// Packet Message Item List Count
	PMSG_BUYLIST_FROM_PSHOP pMsgIL;
	BYTE sendBuf[1024];
	int sOfs = sizeof(pMsgILC);
	int pMsgILSize = sizeof(pMsgIL);

	if ( btResult != 1 )
	{
		LPOBJ lpObj = &gObj[aSourceIndex];
		
		pMsgILC.h.c = 0xC2;
		pMsgILC.h.headcode = 0x3F;

		if ( bResend == false )
			pMsgILC.h.subcode = 0x05;
		else
			pMsgILC.h.subcode = 0x13;

		pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
		pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
		pMsgILC.btCount = 0;
		pMsgILC.Result = btResult;
		pMsgILC.NumberH = SET_NUMBERH(aTargetIndex);
		pMsgILC.NumberL = SET_NUMBERL(aTargetIndex);
		memset(pMsgILC.btName, 0, sizeof(pMsgILC.btName));
		memset(pMsgILC.szPShopText, 0, sizeof(pMsgILC.szPShopText));
		memcpy(sendBuf, &pMsgILC, sizeof(pMsgILC));

		DataSend(aSourceIndex, sendBuf, sOfs);
	}
	else
	{
		LPOBJ lpObjSource = &gObj[aSourceIndex];
		LPOBJ lpObjTarget = &gObj[aTargetIndex];
		int iCount = 0;

		for ( int n=MAIN_INVENTORY_SIZE;n<(INVENTORY_SIZE-1);n++)
		{
			if ( lpObjTarget->Inventory1[n].IsItem() == TRUE )
			{
				pMsgIL.Pos = n;
				ItemByteConvert(pMsgIL.ItemInfo, lpObjTarget->Inventory1[n]);
				pMsgIL.PShopItemValue = lpObjTarget->Inventory1[n].m_iPShopValue;
				pMsgIL.PShopBlessValue = lpObjTarget->Inventory1[n].m_PShopBlessValue;
				pMsgIL.PShopSoulValue = lpObjTarget->Inventory1[n].m_PShopSoulValue;
				pMsgIL.PShopChaosValue = lpObjTarget->Inventory1[n].m_PShopChaosValue;
				memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
				iCount++;
				sOfs += pMsgILSize;
			}
		}

		pMsgILC.h.c = 0xC2;
		pMsgILC.h.headcode = 0x3F;

		if ( bResend == false )
			pMsgILC.h.subcode = 0x05;
		else
			pMsgILC.h.subcode = 0x13;

		pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
		pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
		pMsgILC.btCount = iCount;
		pMsgILC.Result = btResult;
		pMsgILC.NumberH = SET_NUMBERH(aTargetIndex);
		pMsgILC.NumberL = SET_NUMBERL(aTargetIndex);
		memcpy(pMsgILC.btName, lpObjTarget->Name, sizeof(pMsgILC.btName));
		memcpy(pMsgILC.szPShopText, lpObjTarget->m_szPShopText, sizeof(pMsgILC.szPShopText));
		memcpy(sendBuf, &pMsgILC, sizeof(pMsgILC));

		DataSend(aSourceIndex, sendBuf, sOfs);

	}
}





void CGPShopReqBuyItem(PMSG_REQ_BUYITEM_FROM_PSHOP * lpMsg, int aSourceIndex)
{
	if ( gDoPShopOpen == FALSE )
		return;

	LPOBJ lpSourceObj = &gObj[aSourceIndex];

	if ( gObjIsConnected( MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL)) == FALSE )
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : Seller did Not CONNECTED (%d)",
			lpSourceObj->AccountID, lpSourceObj->Name, MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL));
		::CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

	LPOBJ lpObj = &gObj[MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL)];
	

	if ( lpObj->Type != OBJ_USER )
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : Seller is Not CHARACTER (%d)",
			lpSourceObj->AccountID, lpSourceObj->Name,  MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL));
		::CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

	if ( lpObj->CloseCount >= 0 )	
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : Seller is Closing (%d)",
			lpSourceObj->AccountID, lpSourceObj->Name,  MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL)  );
		::CGPShopAnsBuyItem(aSourceIndex, -1, 0,2);
		return;
	}


	if ( lpSourceObj->CloseCount >= 0 )	
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : Buyer is Closing (%d)",
			lpSourceObj->AccountID, lpSourceObj->Name,  MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL));
		::CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

	if ( lpSourceObj->pInventory[0].m_Type == ITEMGET(0,41) )
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : Buyer is Mining (%d)",
			lpSourceObj->AccountID, lpSourceObj->Name,  MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL));
		::CGPShopAnsBuyItem(aSourceIndex, -1, 0, 10);
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsDuel(gObj[aSourceIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3429), aSourceIndex, 1);
		 return;
	}
	
	if (g_NewPVP.IsObserver(gObj[aSourceIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3430), aSourceIndex, 1);
		 return;
	}
#endif

	if ( lpObj->m_bPShopOpen == false )
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Did not Open PShop",
			lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name);
		::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 3);
		return;
	}

	char szName[MAX_ACCOUNT_LEN+1] = {0};
	memcpy(szName, lpMsg->btName, sizeof(lpMsg->btName));
	szName[MAX_ACCOUNT_LEN] = 0;
	int iITEM_LOG_TYPE;
	int iITEM_LOG_LEVEL;
	int iITEM_LOG_DUR;
	int iITEM_LOG_SERIAL;

	if ( strcmp(szName, lpObj->Name) )
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : Name Mismatch [%s] - [%s]",
			lpSourceObj->AccountID, lpSourceObj->Name,szName, lpObj->Name);
		::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 6);

		return;
	}

	if ( lpObj->CloseType != -1 )
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] is Closing Connection",
			lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name);
		::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0,2);

		return;
	}

	if ( (lpObj->Penalty &4) == 4 || (lpObj->Penalty &8) == 8 )
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] is in Item Block",
			lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name);
		::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0,9);

		return;
	}

	if ( (lpSourceObj->Penalty &4) == 4 || (lpSourceObj->Penalty &8) == 8 )
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] is in Item Block",
			lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name);
		::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0,9);
		return;
	}

	EnterCriticalSection(&lpObj->m_critPShopTrade);

	if ( lpObj->m_bPShopTransaction == true )
	{
		LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Already Trade With Other",
			lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name);
		::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0,4);
		LeaveCriticalSection(&lpObj->m_critPShopTrade);

	}
	else
	{
		lpObj->m_bPShopTransaction = true;
		LeaveCriticalSection(&lpObj->m_critPShopTrade);

		if ( gObjCheckSerial0ItemList(&lpObj->Inventory1[lpMsg->btItemPos]) != FALSE )
		{
			MsgOutput(lpObj->m_Index, lMsg.Get(MSGGET(13, 26)));
			GCReFillSend(aSourceIndex,(WORD)lpSourceObj->Life, 0xFD, 1, lpSourceObj->iShield);

			LogAddTD("[ANTI-HACK][Serial 0 Item] [PShop Buy] (%s)(%s) Item(%s) Pos(%d)",
				lpObj->AccountID, lpObj->Name, lpObj->Inventory1[lpMsg->btItemPos].GetName(), lpMsg->btItemPos);

			return;
		}



		// New for Check Item Serials
		if ( ::gObjInventorySearchSerialNumber(&gObj[aSourceIndex], lpObj->Inventory1[lpMsg->btItemPos].GetNumber()) == FALSE )
		{
			::GCReFillSend(aSourceIndex,(WORD)lpSourceObj->Life, -3, 1, lpSourceObj->iShield);
			LogAdd("error-L2 : CopyItem [%s][%s] return %s %d", gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name,
				__FILE__, __LINE__);

			return;
		}

		if ( ::gObjInventorySearchSerialNumber(lpObj, lpObj->Inventory1[lpMsg->btItemPos].GetNumber()) == FALSE )
		{
			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life, -3, 1, lpObj->iShield);
			LogAdd("error-L2 : CopyItem [%s][%s] return %s %d",
				lpObj->AccountID, lpObj->Name,
				__FILE__, __LINE__);

			return;
		}

		if (gObjFixInventoryPointer(aSourceIndex) == false )
		{
			LogAdd( "[Fix Inv.Ptr] False Location - %s, %d", 
				__FILE__, __LINE__);
		}

		if ( gObj[aSourceIndex].pTransaction == 1 )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : Requester Transaction == 1, IF_TYPE : %d",
				lpSourceObj->AccountID, lpSourceObj->Name, lpSourceObj->m_IfState.type);
			return;
		}

		iITEM_LOG_TYPE=0;
		iITEM_LOG_LEVEL=0;
		iITEM_LOG_DUR=0;
		iITEM_LOG_SERIAL = 0;

		__try
		{
			if ( PSHOP_RANGE(lpMsg->btItemPos) == FALSE )
			{
				::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 5);
				return;
			}

			if ( lpObj->m_bMapSvrMoveQuit == true )
			{
				CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
				return;
			}

			if ( lpObj->Inventory1[lpMsg->btItemPos].IsItem() == TRUE )
			{
				int n;
				int invSize;
				int iPShopValue = lpObj->Inventory1[lpMsg->btItemPos].m_iPShopValue;
				short aPShopJewelValue[3];
				
				aPShopJewelValue[0] = lpObj->Inventory1[lpMsg->btItemPos].m_PShopBlessValue;
				aPShopJewelValue[1] = lpObj->Inventory1[lpMsg->btItemPos].m_PShopSoulValue;
				aPShopJewelValue[2] = lpObj->Inventory1[lpMsg->btItemPos].m_PShopChaosValue;

				if ( iPShopValue < 0 ||
					 aPShopJewelValue[0] < 0 ||
					 aPShopJewelValue[1] < 0 ||
					 aPShopJewelValue[2] < 0 )
				{
					LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Did Not Choose Item Price",
						lpSourceObj->AccountID, lpSourceObj->Name,
						lpObj->AccountID, lpObj->Name);
					::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 6);

					return;
				}

				if( iPShopValue > 0 )
				{
					if ( gObj[aSourceIndex].Money < lpObj->Inventory1[lpMsg->btItemPos].m_iPShopValue )
					{
						LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Lack of Zen",
							lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name);
						::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 7);
						return;
					}

					if ( gObjCheckMaxZen(lpObj->m_Index, iPShopValue) == FALSE )
					{
						LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Exceeding Zen of the Host",
							lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name);
						::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 8);
						return;
					}
				}

				int iFreeSpaceReq = 0;
				short aPShopJewel[3];
				short aPShopJewelBoundle[3][3];
				
				for( n = 0; n < 3; n++)
				{
					int tmpJewelCnt = aPShopJewelValue[n];

					if( tmpJewelCnt > 0 )
					{
						aPShopJewelBoundle[n][0] = tmpJewelCnt/30;
						tmpJewelCnt = tmpJewelCnt%30;
						aPShopJewelBoundle[n][1] = tmpJewelCnt/20;
						tmpJewelCnt = tmpJewelCnt%20;
						aPShopJewelBoundle[n][2] = tmpJewelCnt/10;
						aPShopJewel[n] = tmpJewelCnt%10;

						iFreeSpaceReq += aPShopJewel[n] + aPShopJewelBoundle[n][0] + aPShopJewelBoundle[n][1] + aPShopJewelBoundle[n][2];
					}
				}

				if( iFreeSpaceReq > 0 )
				{
					int iEmptyCount = 0;
					invSize = MAIN_INVENTORY_SIZE;
				
					if( lpObj->pInventoryExtend < 4 )	{
						invSize = (MAIN_INVENTORY_SIZE)-(32*(4-lpObj->pInventoryExtend));
					}

					for ( int x=0; x<invSize;x++)
					{
						if ( lpObj->pInventoryMap[x] == 0xFF )
						{
							iEmptyCount++;
						}
					}

					if( iEmptyCount < iFreeSpaceReq )
					{
						LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] No Room to Store Reward",
							lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name);
						::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 17);
						return;
					}
				}

				bool bBoundle[3];
				bBoundle[0] = false;
				bBoundle[1] = false;
				bBoundle[2] = false;

				int JewelCount[3];
				JewelCount[0] = 0;
				JewelCount[1] = 0;
				JewelCount[2] = 0;
					
				invSize = MAIN_INVENTORY_SIZE;
				
				if( lpSourceObj->pInventoryExtend < 4 )	{
					invSize = (MAIN_INVENTORY_SIZE)-(32*(4-lpSourceObj->pInventoryExtend));
				}

				for( n = INVENTORY_BAG_START; n < invSize; n++)
				{
					if( !lpSourceObj->pInventory[n].IsItem() )
						continue;

					if( lpSourceObj->pInventory[n].m_Type == ITEMGET(14,13) )
					{
						JewelCount[0]++;
					}
					else if( lpSourceObj->pInventory[n].m_Type == ITEMGET(14,14) )
					{
						JewelCount[1]++;
					}
					else if( lpSourceObj->pInventory[n].m_Type == ITEMGET(12,15) )
					{
						JewelCount[2]++;
					}
					else if( lpSourceObj->pInventory[n].m_Type == ITEMGET(12,30) )
					{
						bBoundle[0] = true;
						JewelCount[0] += 10 * (lpSourceObj->pInventory[n].m_Level+1);
					}
					else if( lpSourceObj->pInventory[n].m_Type == ITEMGET(12,31) )
					{
						bBoundle[1] = true;
						JewelCount[1] += 10 * (lpSourceObj->pInventory[n].m_Level+1);
					}
					else if( lpSourceObj->pInventory[n].m_Type == ITEMGET(12,141) )
					{
						bBoundle[2] = true;
						JewelCount[2] += 10 * (lpSourceObj->pInventory[n].m_Level+1);
					}
				}

				for( n = 0; n < 3; n++)
				{
					if( JewelCount[n] < aPShopJewelValue[n] )
					{
						LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] No enough Jewels - Type: %d",
						lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name, n);
						::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 11+n);
						return;
					}
					else if( bBoundle[n] && JewelCount[n] > aPShopJewelValue[n] )
					{
						LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Boundle Jewels - Type: %d",
						lpSourceObj->AccountID, lpSourceObj->Name,lpObj->AccountID, lpObj->Name, n);
						::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 14+n);
						return;
					}
				}
				
				BYTE btNewItemPos = 0;
				btNewItemPos = ::gObjOnlyInventoryInsertItem(aSourceIndex, lpObj->Inventory1[lpMsg->btItemPos]);

				if ( btNewItemPos == 0xFF )
				{
					LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] No Room to Buy Item",
						gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name,
						lpObj->AccountID, lpObj->Name);
					::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, 0, 8);

					return;
				}
				
				g_ElementalSystem.PShopErtel(lpObj,&gObj[aSourceIndex],&gObj[aSourceIndex].pInventory[btNewItemPos]);
				
				iITEM_LOG_TYPE = lpObj->Inventory1[lpMsg->btItemPos].m_Type;
				iITEM_LOG_LEVEL = lpObj->Inventory1[lpMsg->btItemPos].m_Level;
				iITEM_LOG_DUR = (int)lpObj->Inventory1[lpMsg->btItemPos].m_Durability;
				iITEM_LOG_SERIAL = lpObj->Inventory1[lpMsg->btItemPos].m_Number;
				::gObjInventoryItemSet_PShop(lpObj->m_Index, lpMsg->btItemPos, -1);
				lpObj->Inventory1[lpMsg->btItemPos].Clear();
				::GCInventoryItemDeleteSend(lpObj->m_Index, lpMsg->btItemPos, TRUE);

				if( iPShopValue > 0 )
				{
					gObj[aSourceIndex].Money -= iPShopValue;
					lpObj->Money += iPShopValue;
					::GCMoneySend(aSourceIndex, lpSourceObj->Money);
					::GCMoneySend(lpObj->m_Index, lpObj->Money);
				}

				JewelCount[0] = 0;
				JewelCount[1] = 0;
				JewelCount[2] = 0;

				for ( n = INVETORY_WEAR_SIZE ; n< invSize; n++)
				{
					if ( !lpSourceObj->pInventory[n].IsItem() )
						continue;

					int JewelType = lpSourceObj->pInventory[n].m_Type;
					int JewelLevel = lpSourceObj->pInventory[n].m_Level;

					switch(JewelType)
					{
					case ITEMGET(14,13):
					case ITEMGET(12,30):
						if( JewelCount[0] < aPShopJewelValue[0] )
						{
							gObjInventoryItemSet(aSourceIndex, n, (BYTE)-1);
							lpSourceObj->pInventory[n].Clear();
							GCInventoryItemDeleteSend(aSourceIndex, n, TRUE);
							JewelCount[0] += (JewelType == ITEMGET(14,13) ) ? 1 : ((JewelLevel+1)*10);
						}
						break;
					case ITEMGET(14,14):
					case ITEMGET(12,31):
						if( JewelCount[1] < aPShopJewelValue[1] )
						{
							gObjInventoryItemSet(aSourceIndex, n, (BYTE)-1);
							lpSourceObj->pInventory[n].Clear();
							GCInventoryItemDeleteSend(aSourceIndex, n, TRUE);
							JewelCount[1] += (JewelType == ITEMGET(14,14) ) ? 1 : ((JewelLevel+1)*10);
						}
						break;
					case ITEMGET(12,15):
					case ITEMGET(12,141):
						if( JewelCount[2] < aPShopJewelValue[2] )
						{
							gObjInventoryItemSet(aSourceIndex, n, (BYTE)-1);
							lpSourceObj->pInventory[n].Clear();
							GCInventoryItemDeleteSend(aSourceIndex, n, TRUE);
							JewelCount[2] += (JewelType == ITEMGET(12,15) ) ? 1 : ((JewelLevel+1)*10);
						}
						break;
					}
				}


				int iBoundleId[3] = {ITEMGET(12,30), ITEMGET(12,31), ITEMGET(12,141)};
				int iJewelId[3] = {ITEMGET(14,13), ITEMGET(14,14), ITEMGET(12,15)};

				for( n = 0; n < 3; n++ )
				{
					if( aPShopJewelValue[n] > 0 )
					{
						for(int x = 0; x < aPShopJewelBoundle[n][0]; x++)
						{	
							ItemSerialCreateSend(lpObj->m_Index,235,0,0,iBoundleId[n],2,1,0,0,0,lpObj->m_Index,0,0,0,0);							
#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
							lpObj->OffTradeWaitItem++;
#endif
						}

						for(int x = 0; x < aPShopJewelBoundle[n][1]; x++)
						{	
							ItemSerialCreateSend(lpObj->m_Index,235,0,0,iBoundleId[n],1,1,0,0,0,lpObj->m_Index,0,0,0,0);
#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
							lpObj->OffTradeWaitItem++;
#endif
						}

						for(int x = 0; x < aPShopJewelBoundle[n][2]; x++)
						{	
							ItemSerialCreateSend(lpObj->m_Index,235,0,0,iBoundleId[n],0,1,0,0,0,lpObj->m_Index,0,0,0,0);
#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
							lpObj->OffTradeWaitItem++;
#endif
						}

						for(int x = 0; x < aPShopJewel[n]; x++)
						{	
							ItemSerialCreateSend(lpObj->m_Index,235,0,0,iJewelId[n],0,1,0,0,0,lpObj->m_Index,0,0,0,0);
#if (ENABLE_CUSTOM_OFFLINETRADE == 1)
							lpObj->OffTradeWaitItem++;
#endif
						}
					}
				}

				::CGPShopAnsBuyItem(aSourceIndex, lpObj->m_Index, btNewItemPos, 1);
				::CGPShopAnsSoldItem(lpObj->m_Index, aSourceIndex, lpMsg->btItemPos);

				LogAddTD("[PShop] [%s][%s][%s] PShop Item Buy Request Succeed : [%s][%s][%s] (Price=%d, ItemType:%d (%s), ItemLevel:%d, ItemDur:%d, Serial:%d",
					gObj[aSourceIndex].AccountID, gObj[aSourceIndex].Name, gObj[aSourceIndex].Ip_addr,
					lpObj->AccountID, lpObj->Name, lpObj->Ip_addr, iPShopValue, iITEM_LOG_TYPE,
					ItemAttribute[iITEM_LOG_TYPE].Name, iITEM_LOG_LEVEL, iITEM_LOG_DUR, iITEM_LOG_SERIAL);

				if (PShop_CheckInventoryEmpty(lpObj->m_Index) == true )
				{
					LogAddTD("[PShop] [%s][%s] Sold All Items - Auto Closing PShop",
						lpObj->AccountID, lpObj->Name);::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 195)), lpObj->m_Index, 1);
					lpObj->m_bPShopOpen = false;
					memset(lpObj->m_szPShopText, 0, sizeof(lpObj->m_szPShopText));
					::CGPShopAnsClose(lpObj->m_Index, 1);
#if (ENABLE_CUSTOM_OFFLINETRADE==1)
					if( lpObj->bOffTrade && lpObj->OffTradeWaitItem == 0 )
					{
						gObjDel(lpObj->m_Index);
					}
#endif
				}
				else
				{
					lpObj->m_bPShopItemChange = true;
				}

			}
		}
		__finally
		{
			lpObj->m_bPShopTransaction = false;
		}
	}
}





struct PMSG_ANS_BUYITEM_FROM_PSHOP
{
	PBMSG_HEAD2 h;	// C1:3F:06
	BYTE Result;	// 3
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
	BYTE cItemInfo[MAX_ITEM_INFO];	// 7
	BYTE btItemPos;	// E
};


void CGPShopAnsBuyItem(int aSourceIndex, int aTargetIndex, int iItemPos, BYTE btResult)
{
	PMSG_ANS_BUYITEM_FROM_PSHOP pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x3F, 0x06, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aTargetIndex);
	pMsg.NumberL = SET_NUMBERL(aTargetIndex);
	pMsg.btItemPos = iItemPos;
	pMsg.Result = btResult;
	ItemByteConvert(pMsg.cItemInfo, gObj[aSourceIndex].Inventory1[iItemPos]);

	DataSend(aSourceIndex, (LPBYTE)&pMsg, pMsg.h.size);
}




struct PMSG_ANS_SOLDITEM_FROM_PSHOP
{
	PBMSG_HEAD2 h;	// C1:3F:08
	BYTE btItemPos;	// 4
	BYTE btName[10];	// 5
};


void CGPShopReqCloseDeal(PMSG_REQ_PSHOPDEAL_CLOSE * lpMsg, int aIndex)
{
	if ( !gObjIsConnected(aIndex))
		return;

	gObj[aIndex].m_bPShopWantDeal = false;
	gObj[aIndex].m_iPShopDealerIndex =-1;
	memset(gObj[aIndex].m_szPShopDealerName , 0, MAX_ACCOUNT_LEN);
}




void CGPShopAnsSoldItem(int aSourceIndex, int aTargetIndex, int iItemPos)
{
	PMSG_ANS_SOLDITEM_FROM_PSHOP pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x3F, 0x08, sizeof(pMsg));
	pMsg.btItemPos = iItemPos;
	memcpy(pMsg.btName, gObj[aTargetIndex].Name, MAX_ACCOUNT_LEN);

	DataSend(aSourceIndex, (LPBYTE)&pMsg, pMsg.h.size);
}




struct PMSG_REQ_DEALER_CLOSED_SHOP
{
	PBMSG_HEAD2 h;	// C1:3F:12
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
};


void CGPShopAnsDealerClosedShop(int aSourceIndex, int aTargetIndex)
{
	PMSG_REQ_DEALER_CLOSED_SHOP pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x3F, 0x12, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aTargetIndex);
	pMsg.NumberL = SET_NUMBERL(aTargetIndex);

	DataSend(aSourceIndex, (LPBYTE)&pMsg, pMsg.h.size);
}




struct PMSG_PARTYREQUESTSEND
{
	PBMSG_HEAD h;	// C1:40
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};


void CGPartyRequestRecv(PMSG_PARTYREQUEST * lpMsg, int aIndex)
{
	int number =  MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	PMSG_PARTYREQUESTSEND pMsg;

	if ( number < 0 || number > OBJMAX-1)
		return;

	if (!PacketCheckTime(&gObj[aIndex]))
		return;

	if ( !gObjIsConnected(&gObj[number]))
	{
		::GCResultSend(aIndex, 0x41, 0x03);
		return;
	}

	if ( gObj[aIndex].CloseCount >= 0 || gObj[number].CloseCount >= 0 )
	{
		::GCResultSend(aIndex, 0x41, 0x00);
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsDuel(gObj[aIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3429), aIndex, 1);
		 return;
	}
	
	if (g_NewPVP.IsObserver(gObj[aIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3430), aIndex, 1);
		 return;
	}
#endif

	if ( CC_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		::GCServerMsgStringSend(lMsg.Get(MSGGET(4, 198)), aIndex, 1);
		return;
	}

	if ( IT_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		::GCServerMsgStringSend("Party not allowed in Illusion Temple.", aIndex, 1);
		return;
	}

	if ( DG_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		::GCServerMsgStringSend("Party not allowed in Doppelganger.", aIndex, 1);
		return;
	}

	if ( IF_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		::GCServerMsgStringSend("Party not allowed in Imperial Fort.", aIndex, 1);
		return;
	}

#if (__GENS_SYSTEM__ == 1 )

	/*if ( gObj[aIndex].m_GensInfo.btFamily == GENS_NONE )
	{
		::GCResultSend(aIndex, 0x41, 0x06);
		return;
	}*/
	if( gGensSystem.m_CheckForParty )
	{
		if ( gObj[aIndex].m_GensInfo.btFamily != GENS_NONE && gObj[aIndex].m_GensInfo.btFamily != gObj[number].m_GensInfo.btFamily )
		{
			::GCResultSend(aIndex, 0x41, 0x07);
			return;
		}

		if ( gObj[aIndex].m_GensInfo.btFamily != gObj[number].m_GensInfo.btFamily )
		{
			::GCResultSend(aIndex, 0x41, 0x07);
			return;
		}
	}

	//if ( gGensSystem.GensMap(gObj[aIndex].MapNumber) )
	//{
	//	return;
	//}

#endif


	if ( gObj[aIndex].m_IfState.use > 0 )
	{
		::GCResultSend(aIndex, 0x41, 0x00);
		return;
	}

	LogAddL("Party result : %d %d %d %d", gObj[aIndex].CloseCount, gObj[number].CloseCount, gObj[aIndex].Connected, gObj[number].Connected);
	LogAddL("%s %s", gObj[aIndex].Name, gObj[number].Name);
		
	if ( gObj[number].m_IfState.use > 0 )
	{
		::GCResultSend(aIndex, 0x41, 0x00);
		return;
	}

	if ( (gObj[number].m_Option &1) != TRUE )
	{
		::GCResultSend(aIndex, 0x41, 0x01);
		return;
	}

	if ( gObj[aIndex].PartyNumber >= 0 )
	{
		int iUserIndex;
		int iDbNumber;
		gParty.GetIndexUser(gObj[aIndex].PartyNumber, 0, iUserIndex, iDbNumber);
		
		if ( iUserIndex < 0 || ( iUserIndex != aIndex && gObj[aIndex].DBNumber != iDbNumber )  )
		{
			::GCResultSend(aIndex, 0x41, 0x00);
			return;

		}
	}

	if ( gObj[aIndex].PartyTargetUser >= 0 )
		return;

	if ( gObj[number].PartyNumber >= 0 )
	{
		::GCResultSend(aIndex, 0x41, 0x04);
		return;
	}

	if ( gObj[number].PartyTargetUser >= 0 )
	{
		::GCResultSend(aIndex, 0x41, 0x00);
		return;
	}

#if (ENABLETEST_ARCA == 1)
	if( g_ArcaBattle.IsArcaBattleServer()
#if (ENABLETEST_ACHERON == 1)
		&& !g_AcheronGuardianEvent.IsPlayStart()
#endif
		&& gObj[aIndex].GuildNumber != gObj[number].GuildNumber )
    {
        GCServerMsgStringSend(/*lMsg.Get(3507)*/"You can create party only with your guild", aIndex, 1);
		return;
    }
#endif

	gObj[aIndex].m_IfState.use = TRUE;
	gObj[aIndex].m_IfState.type = 2;
	gObj[aIndex].m_IfState.state = 0;
	gObj[number].m_IfState.use = TRUE;
	gObj[number].m_IfState.type = 2;
	gObj[number].m_IfState.state = 0;
	gObj[aIndex].TargetNumber = number;
	gObj[number].TargetNumber = aIndex;
	gObj[aIndex].m_InterfaceTime = GetTickCount();
	gObj[number].m_InterfaceTime = GetTickCount();
	gObj[aIndex].PartyTargetUser = number;
	
	PHeadSetB((LPBYTE)&pMsg, 0x40, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);

	DataSend(number, (LPBYTE)&pMsg, pMsg.h.size);
}





void CGPartyRequestResultRecv(PMSG_PARTYREQUESTRESULT * lpMsg, int aIndex)
{
	int number;
	BYTE result = false;
	char szTemp[256];
	int pnumber = -1;

	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if ( number < 0 || number > OBJMAX-1)
		return;

	if ( !gObjIsConnected(&gObj[aIndex]))
		return;

	if ( !gObjIsConnected(&gObj[number]))
	{
		GCResultSend(aIndex, 0x41, 0x04);
	}
	else
	{
		result = true;
	}

	if ( gObj[number].MapNumber != gObj[aIndex].MapNumber )
	{
		result = false;
		GCResultSend(number, 0x41, 0x00);
	}

	if ( CC_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 198)), aIndex, 1);
		result = false;
		::GCResultSend(number, 0x41, 0x00);
	}

	if ( IT_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		GCServerMsgStringSend("Party not allowed in Illusion Temple.", aIndex, 1);
		result = false;
		::GCResultSend(number, 0x41, 0x00);
	}

	if ( DG_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		GCServerMsgStringSend("Party not allowed in Doppelganger.", aIndex, 1);
		result = false;
		::GCResultSend(number, 0x41, 0x00);
	}

	if ( IF_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		GCServerMsgStringSend("Party not allowed in Imperial Fort.", aIndex, 1);
		result = false;
		::GCResultSend(number, 0x41, 0x00);
	}

	if ( lpMsg->Result == 0 )
	{
		result = false;
		::GCResultSend(number, 0x41, 0x01);
	}

	if ( gObj[number].m_IfState.use == 0 || gObj[number].m_IfState.type != 2)
	{
		result = false;
		::GCResultSend(number, 0x41, 0x00);
	}	
	
	if ( gObj[aIndex].m_IfState.use == 0 || gObj[aIndex].m_IfState.type != 2)
	{
		result = false;
		::GCResultSend(number, 0x41, 0x00);
	}

	if ( result == 1 )
	{
		if ( gObj[number].PartyNumber < 0 )
		{
			gObj[number].PartyNumber = gParty.Create(number, gObj[number].DBNumber, gObj[number].Level);
		}

		if ( gObj[number].PartyNumber >= 0  )
		{
			pnumber = gObj[number].PartyNumber;
			int iPartyPos = gParty.Add(gObj[number].PartyNumber, aIndex, gObj[aIndex].DBNumber, gObj[aIndex].Level);

			if ( iPartyPos >= 0 )
			{
				gObj[aIndex].PartyNumber = gObj[number].PartyNumber;
				result = true;
				wsprintf(szTemp, lMsg.Get(MSGGET(4, 83)), gObj[aIndex].Name);
				::GCServerMsgStringSend(szTemp, number, 1);
				wsprintf(szTemp, lMsg.Get(MSGGET(4, 83)), gObj[number].Name);
				::GCServerMsgStringSend(szTemp, aIndex, 1);

				gParty.Paint(pnumber);
			}
			else if ( iPartyPos == -1 )
			{
				GCResultSend(number, 0x41, 2);
				GCResultSend(aIndex, 0x41, 2);
			}
			else if ( iPartyPos == -2 )
			{
				result = 2;
				wsprintf(szTemp, lMsg.Get(MSGGET(4, 84)), gObj[aIndex].Name);
				GCServerMsgStringSend(szTemp, aIndex, 1);
			}
		}
	}

	if ( gObj[aIndex].m_IfState.use !=  0 && gObj[aIndex].m_IfState.type == 2)
	{
		gObj[aIndex].m_IfState.use = 0;
		gObj[aIndex].PartyTargetUser = -1;
	}

	if ( gObj[number].m_IfState.use !=  0 && gObj[number].m_IfState.type == 2)
	{
		gObj[number].m_IfState.use = 0;
		gObj[number].PartyTargetUser = -1;
	}

	if ( pnumber >= 0 )
	{
		CGPartyListAll(pnumber);
	}
}



struct PMSG_PARTYLIST
{
	char szId[10];	// 0
	BYTE Number;	// A
	BYTE MapNumber;	// B
	BYTE X;	// C
	BYTE Y;	// D
	int Life;	// 10
	int MaxLife;	// 14
	int ServerCode;
	int Mana;
	int MaxMana;
};


struct PMSG_PARTYLISTCOUNT
{
	PBMSG_HEAD h;	// C1:42
	BYTE Result;	// 3
	BYTE Count;	// 4
};



void CGPartyList(int aIndex)
{
	PMSG_PARTYLISTCOUNT pCount;
	PMSG_PARTYLIST pList;
	BYTE sendbuf[512];
	int lOfs = 0;

	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	PHeadSetB((LPBYTE)&pCount, 0x42, sizeof(pCount));
	pCount.Count = 0;
	pCount.Result = false;

	int number;
	int pnumber = gObj[aIndex].PartyNumber ;

	if ( pnumber >= 0 )
		pCount.Count = gParty.GetCount(pnumber);

	lOfs += sizeof(pCount);

	if ( pCount.Count != 0 )
	{
		for ( int n=0;n<MAX_USER_IN_PARTY;n++)
		{
			number = gParty.m_PartyS[pnumber].Number[n];

			if ( number >= 0 )
			{
				if ( gObjIsConnected(&gObj[number], gParty.m_PartyS[pnumber].DbNumber[n]) == TRUE )
				{
					memset(&pList, 0, sizeof(pList));
					memcpy(pList.szId, gObj[number].Name, MAX_ACCOUNT_LEN);
					pList.Number = n;
					pList.MapNumber = gObj[number].MapNumber;
					pList.X = gObj[number].X;
					pList.Y = gObj[number].Y;
					pList.Life = (int)gObj[number].Life;
					pList.MaxLife = (int)(gObj[number].MaxLife + gObj[number].AddLife);
					pList.Mana = (int)gObj[number].Mana;
					pList.MaxMana = (int)(gObj[number].MaxMana + gObj[number].AddMana);
					pList.ServerCode = gGameServerCode+1;

					memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
					lOfs += sizeof(pList);
				}
			}
		}

		pCount.Result = true;
	}

	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));
	
	DataSend(aIndex, (LPBYTE)&sendbuf, lOfs);
}




void CGPartyListAll(int pnumber)
{
	PMSG_PARTYLISTCOUNT pCount;
	PMSG_PARTYLIST pList;
	BYTE sendbuf[512];
	int lOfs = 0;
	int number;
	int n;

	if ( pnumber < 0 )
		return;

	if ( gParty.IsParty(pnumber) == FALSE )
		return;

	PHeadSetB((LPBYTE)&pCount, 0x42, sizeof(pCount));
	pCount.Result = false;
	pCount.Count = gParty.GetCount(pnumber);

	if ( pCount.Count == 0 )
		return;

	pCount.Result = true;
	lOfs += sizeof(pCount);

	for (n=0;n<MAX_USER_IN_PARTY;n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];

		if ( number >= 0 )
		{
			if ( gObjIsConnected(&gObj[number], gParty.m_PartyS[pnumber].DbNumber[n]) == TRUE )
			{
				memset(&pList, 0, sizeof(pList));
				memcpy(pList.szId, gObj[number].Name, MAX_ACCOUNT_LEN);
				pList.Number = n;
				pList.MapNumber = gObj[number].MapNumber;
				pList.X = gObj[number].X;
				pList.Y = gObj[number].Y;
				pList.Life = (int)gObj[number].Life;
				pList.MaxLife =  (int)gObj[number].MaxLife;
				pList.Mana = (int)gObj[number].Mana;
				pList.MaxMana = (int)(gObj[number].MaxMana + gObj[number].AddMana);
				pList.ServerCode = gGameServerCode % 20 + 1;

				memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
			}
		}
	}

	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));

	for (n=0;n<MAX_USER_IN_PARTY;n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];

		if ( number >= 0 )
		{
			DataSend(number, sendbuf, lOfs);
		}
	}
}





void CGPartyDelUser(PMSG_PARTYDELUSER * lpMsg, int aIndex)
{
	int usernumber = -1;
	int dbnumber = -1;

	if ( gObj[aIndex].PartyNumber < 0 )
	{
		LogAdd("No Party %s %s", gObj[aIndex].AccountID, gObj[aIndex].Name);
		return;
	}

	if ( !gParty.GetIndexUser(gObj[aIndex].PartyNumber, lpMsg->Number, usernumber, dbnumber))
	{
		LogAdd("User Not found");
		return;
	}

	if ( usernumber < 0 )
	{
		LogAdd("User Not found");
		return;
	}

	int count;
	int number;
	int pnumber = gObj[aIndex].PartyNumber;

	if ( pnumber < 0 )
	{
		LogAdd("%s %d", __FILE__, __LINE__);
		return;
	}

	count = gParty.GetCount(pnumber);

	if ( count < 1 )
	{
		LogAdd("%s %d", __FILE__, __LINE__);
		return;
	}

	if( !IF_MAP_RANGE(gObj[usernumber].MapNumber) &&
		!IT_MAP_RANGE(gObj[usernumber].MapNumber) )
	{
		TidyUpTempuserdate(usernumber,pnumber,lpMsg->Number);
	}

	if( DG_MAP_RANGE(gObj[usernumber].MapNumber) )
		return;

	if( lpMsg->Number == 0 )
	{
		int bDPUser = FALSE;

		for(int n = 1; n < MAX_USER_IN_PARTY; n++)
		{
			int nUserIndex = gParty.m_PartyS[pnumber].Number[n];

			if( nUserIndex != -1 )
			{
				if( DG_MAP_RANGE(gObj[nUserIndex].MapNumber) )
				{
					bDPUser = TRUE;
					break;
				}
			}
		}

		if( bDPUser == TRUE && count > 2 )
		{
			gParty.Delete(pnumber,lpMsg->Number);
			count = gParty.GetCount(pnumber);
			gObj[usernumber].PartyNumber = -1;
			gObj[usernumber].PartyTargetUser = -1;

			gParty.UpdatePKPartyPanalty(pnumber);
			gParty.ChangeLeader(pnumber);
			GCPartyDelUserSend(usernumber);
			CGPartyListAll(pnumber);

			LogAddTD("PartyMember(%d) : Party Out. [%s][%s]",
				usernumber,gObj[usernumber].AccountID,gObj[usernumber].Name);
			return;
		}
	}

	if( IF_MAP_RANGE(gObj[usernumber].MapNumber) )
		return;

	if( lpMsg->Number == 0 )
	{
		int bDPUser = FALSE;

		for(int n = 1; n < MAX_USER_IN_PARTY; n++)
		{
			int nUserIndex = gParty.m_PartyS[pnumber].Number[n];

			if( nUserIndex != -1 )
			{
				if( IF_MAP_RANGE(gObj[nUserIndex].MapNumber) )
				{
					bDPUser = TRUE;
					break;
				}
			}
		}

		if( bDPUser == TRUE && count > 2 )
		{
			gParty.Delete(pnumber,lpMsg->Number);
			count = gParty.GetCount(pnumber);
			gObj[usernumber].PartyNumber = -1;
			gObj[usernumber].PartyTargetUser = -1;

			gParty.UpdatePKPartyPanalty(pnumber);
			gParty.ChangeLeader(pnumber);
			GCPartyDelUserSend(usernumber);
			CGPartyListAll(pnumber);

			LogAddTD("PartyMember(%d) : Party Out. [%s][%s]",
				usernumber,gObj[usernumber].AccountID,gObj[usernumber].Name);
			return;
		}
	}

	if( gObj[usernumber].lpGuild != NULL )
	{
		if( gObj[usernumber].lpGuild->WarState == 1 &&
			gObj[usernumber].IsInBattleGround )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(13,216)),usernumber,1);
			return;
		}

		if( usernumber == gParty.m_PartyS[pnumber].Number[0] )
		{
			for(int n = 0; n < MAX_USER_IN_PARTY; n++)
			{
				number = gParty.m_PartyS[pnumber].Number[n];
				
				if( number >= 0 &&
					gObj[number].lpGuild != NULL &&
					gObj[number].lpGuild->WarState == 1 &&
					gObj[number].IsInBattleGround )
				{
					GCServerMsgStringSend(lMsg.Get(MSGGET(13,216)),usernumber,1);
					return;
				}
			}
		}
	}

	if( (lpMsg->Number == 0 || count <= 2) )
	{
		if( IT_MAP_RANGE(gObj[usernumber].MapNumber) )
		{
			if( lpMsg->Number == 0 && count > 2 )
			{
				gParty.Delete(pnumber,lpMsg->Number);
				count = gParty.GetCount(pnumber);
				gObj[usernumber].PartyNumber = -1;
				gObj[usernumber].PartyTargetUser = -1;
				gParty.UpdatePKPartyPanalty(pnumber);
				gParty.ChangeLeader(pnumber);
				GCPartyDelUserSend(usernumber);
				CGPartyListAll(pnumber);

				if( count == 0 )
					gParty.Destroy(pnumber);
				return;
			}
		}
		
		if( IT_MAP_RANGE(gObj[usernumber].MapNumber) )
		{
			if( count <= 2 )
			{
				gParty.Delete(pnumber,lpMsg->Number);
				count = gParty.GetCount(pnumber);
				gObj[usernumber].PartyNumber = -1;
				gObj[usernumber].PartyTargetUser = -1;
				gParty.UpdatePKPartyPanalty(pnumber);

				if( lpMsg->Number == 0 )
					gParty.ChangeLeader(pnumber);

				GCPartyDelUserSend(usernumber);
				CGPartyListAll(pnumber);
				return;
			}
		}

		for(int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[pnumber].Number[n];

			if( number >= 0 )
			{
				gParty.Delete(pnumber,n);
				gObj[number].PartyNumber = -1;
				gObj[number].PartyTargetUser = -1;

				GCPartyDelUserSend(number);
			}
		}

		gParty.Destroy(pnumber);
		return;
	}

	gParty.Delete(pnumber,lpMsg->Number);
	count = gParty.GetCount(pnumber);
	gObj[usernumber].PartyNumber = -1;
	gObj[usernumber].PartyTargetUser = -1;
	gParty.UpdatePKPartyPanalty(pnumber);
	GCPartyDelUserSend(usernumber);
	CGPartyListAll(pnumber);
}


void CGPartyDelUserAsExitGameByForce(PMSG_PARTYDELUSER * lpMsg, int aIndex)
{
	int usernumber = -1;
	int dbnumber = -1;
	int count;
	int number;
	int pnumber;

	if( gObj[aIndex].PartyNumber < 0 )
	{
		LogAdd("No party %s %s",gObj[aIndex].AccountID,gObj[aIndex].Name);
		return;
	}

	if( !gParty.GetIndexUser(gObj[aIndex].PartyNumber,lpMsg->Number,usernumber,dbnumber) )
	{
		LogAdd("User not found");
		return;
	}

	if( usernumber < 0 )
	{
		LogAdd("User not found");
		return;
	}

	pnumber = gObj[aIndex].PartyNumber;

	if( pnumber < 0 )
	{
		LogAdd("%s %d",__FILE__,__LINE__);
		return;
	}

	count = gParty.GetCount(pnumber);

	if( count < 1 )
	{
		LogAdd("%s %d",__FILE__,__LINE__);
		return;
	}

	if( gObj[aIndex].lpGuild != NULL )
	{
		if( gObj[aIndex].lpGuild->WarState == 1 &&
			gObj[aIndex].IsInBattleGround )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(13,216)),aIndex,1);
			return;
		}

		if( usernumber == gParty.m_PartyS[pnumber].Number[0] )
		{
			for(int n = 0; n < MAX_USER_IN_PARTY; n++)
			{
				number = gParty.m_PartyS[pnumber].Number[n];

				if( number >= 0 &&
					gObj[number].lpGuild != NULL && 
					gObj[number].lpGuild->WarState == 1 && 
					gObj[number].IsInBattleGround )
				{
					GCServerMsgStringSend(lMsg.Get(MSGGET(13,216)),aIndex,1);
					return;
				}
			}
		}
	}

	if( lpMsg->Number == 0 || count <= 2 )
	{
		int nReallyConnectPartyMemberCount = gParty.GetReallyConnectPartyMemberCount(pnumber);

		if( lpMsg->Number == 0 && 
			nReallyConnectPartyMemberCount > 1 )
		{
			if( !gObj[aIndex].m_bIsMaintainParty )
			{
				gParty.Delete(pnumber,lpMsg->Number);
			}

			gParty.UpdatePKPartyPanalty(pnumber);
			gParty.SwitchLeader(pnumber);
			CGPartyListAll(pnumber);
		}
		else if( nReallyConnectPartyMemberCount > 1 )
		{

		}
		else
		{
			for(int n = 0; n < MAX_USER_IN_PARTY; n++)
			{
				number = gParty.m_PartyS[pnumber].Number[n];

				if( number >= 0 )
				{
					gParty.Delete(pnumber,n);
					gObj[number].PartyNumber = -1;
					gObj[number].PartyTargetUser = -1;

					if( gObj[number].Connected == PLAYER_PLAYING )
					{
						GCPartyDelUserSend(number);
					}
				}
			}

			gParty.Destroy(pnumber);
		}
	}


}


struct PMSG_PARTYDELUSERSEND
{
	PBMSG_HEAD h;	// C1:43
};



void GCPartyDelUserSend(int aIndex)
{
	PMSG_PARTYDELUSERSEND pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x43, sizeof(pMsg));

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}


struct PMSG_GUILDQUESTSEND
{
	PBMSG_HEAD h;	// C1:50
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};


void CGGuildRequestRecv(PMSG_GUILDJOINQ * lpMsg, int aIndex)
{
	PMSG_GUILDQUESTSEND pMsg;
	int number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if ( number < 0 || number > OBJMAX-1 )
		return;

	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	if ( !gObjIsConnected(&gObj[number]))
	{
		GCResultSend(aIndex, 0x51, 0x03);
		return;
	}

	if ( gObj[aIndex].m_IfState.use > 0 )
	{
		GCResultSend(aIndex, 0x51, 0x06);
		return;
	}
	
	if ( gObj[number].m_IfState.use > 0 )
	{
		GCResultSend(aIndex, 0x51, 0x06);
		return;
	}

	if ( gObj[aIndex].Level < 6 )
	{
		GCResultSend(aIndex, 0x51, 0x07);
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsDuel(gObj[aIndex]) || g_NewPVP.IsDuel(gObj[number])) {
		 GCServerMsgStringSend(lMsg.Get(3426), aIndex, 1);
		 return;
	}

	if (g_NewPVP.IsObserver(gObj[aIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3427), aIndex, 1);
		 return;
	}
#endif

#if (ENABLETEST_ARCA == 1)
	int iArcaBattleState = g_ArcaBattle.GetState();

	if( iArcaBattleState > AB_STATE_CLOSED && iArcaBattleState < AB_STATE_CLOSE )
	{
		GCResultSend(aIndex, 0x51, 0xA4);
		return;
	}
#endif

	if ( gObj[aIndex].GuildNumber > 0 )
	{
		GCResultSend(aIndex, 0x51, 0x05);
		return;
	}

	if ( (gObj[number].m_Option&1 ) != 1 )
	{
		GCResultSend(aIndex, 0x51, 0x00);
		return;
	}

	if ( gObj[number].GuildNumber > 0 )
	{
		if ( strcmp(gObj[number].lpGuild->Names[0], gObj[number].Name))
		{
			GCResultSend(aIndex, 0x51, 0x04);
			return;
		}

		if ( gObj[number].lpGuild->WarState )
		{
			GCResultSend(aIndex, 0x51, 0x06);
			return;
		}

#if (__GENS_SYSTEM__ == 1)

		if( gGensSystem.m_CheckForGuild )
		{
			if ( gObj[number].m_GensInfo.btFamily == GENS_NONE )
			{
				GCResultSend(aIndex,0x51,0xA1);
				return;
			}

			if ( gObj[number].m_GensInfo.btFamily != gObj[aIndex].m_GensInfo.btFamily )
			{
				GCResultSend(aIndex,0x51,0xA2);
				return;
			}
		}

#endif

		int MaxGuildMember;

		if ( gObj[number].Class == CLASS_DARKLORD )
			MaxGuildMember = gObj[number].Level / 10 + gObj[number].Leadership / 10;
		else
			MaxGuildMember = gObj[number].Level / 10;

		if ( MaxGuildMember > g_GuildMaxMember )
			MaxGuildMember = g_GuildMaxMember;

		if ( gObj[number].lpGuild->TotalCount >= MaxGuildMember )
		{
			GCResultSend(aIndex, 0x51, 0x02);
			return;
		}
	}
	else
	{
		GCResultSend(aIndex, 0x51, 0x04);
		return;
	}
	
	gObj[aIndex].m_IfState.use = 1;
	gObj[aIndex].m_IfState.type = 4;
	gObj[aIndex].m_IfState.state = 0;

	gObj[number].m_IfState.use = 1;
	gObj[number].m_IfState.type = 4;
	gObj[number].m_IfState.state = 0;

	gObj[aIndex].TargetNumber = number;
	gObj[number].TargetNumber = aIndex;
	gObj[aIndex].m_InterfaceTime = GetTickCount();
	gObj[number].m_InterfaceTime = GetTickCount();

	PHeadSetB((LPBYTE)&pMsg, 0x50, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);

	DataSend(number, (LPBYTE)&pMsg, pMsg.h.size);

	LogAddTD("[%s][%s] Guild Request - Join : %s",
		gObj[aIndex].AccountID, gObj[aIndex].Name,
		gObj[number].lpGuild->Name);
}




void CGGuildRequestResultRecv(PMSG_GUILDQRESULT * lpMsg, int aIndex)
{
	int number;
	BYTE result=0;
	int pnumber=-1;

	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if ( number < 0 || number > OBJMAX-1)
		return;

	if ( !gObjIsConnected(&gObj[number]))
	{
		GCResultSend(aIndex, 0x51, 0x02);
	}
	else
	{
		result = 1;
	}

	if ( lpMsg->Result == 0 )
	{
		result = 0;
		GCResultSend(number, 0x51, 0x00);
	}

	if ( result == 1 )
	{
		if ( gObj[aIndex].GuildNumber > 0 )
		{
			GDGuildMemberAdd(number, gObj[aIndex].lpGuild->Name, gObj[number].Name);
		}
	}

	if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type == 4)
		gObj[aIndex].m_IfState.use = 0;
		
	if ( gObj[number].m_IfState.use && gObj[number].m_IfState.type == 4)
		gObj[number].m_IfState.use = 0;
}







struct PMSG_GUILDLIST
{
	char Name[10];	// 0
	BYTE Number;	// A
	BYTE ConnectAServer;	// B
	BYTE btGuildStatus;	// C
};


struct PMSG_GUILDLISTCOUNT
{
	PWMSG_HEAD h;	// C2:52
	BYTE Result;	// 4
	BYTE Count;	// 5
	int TotalScore;	// 8
	BYTE Score;	// C
	char szRivalGuild[9];	// D
	BYTE btCount;
	BYTE btUnk[32];
};



void CGGuildListAll(int pnumber)
{
	if ( gObj[pnumber].lpGuild == NULL )
		return;

	PMSG_GUILDLISTCOUNT pCount;
	PMSG_GUILDLIST pList;
	char sendbuf[1500];
	int lOfs = 0;
	int number;
	int n;
	int guildmaster = 0;
	
	PHeadSetW((LPBYTE)&pCount, 0x52, 0);
	pCount.Result = 0;
	pCount.Count = 0;
	lOfs += sizeof(pCount);
	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);

	if ( gObj[pnumber].GuildNumber < 1 )
	{
		DataSend(pnumber, (LPBYTE)&pCount, lOfs);
		return;
	}

	pCount.Count = gObj[pnumber].lpGuild->Count;
	pCount.TotalScore = gObj[pnumber].lpGuild->TotalScore;
	pCount.Score = gObj[pnumber].lpGuild->PlayScore;
	memset(pCount.szRivalGuild, 0, sizeof(pCount.szRivalGuild));

	if ( gObj[pnumber].lpGuild->iGuildRival )
	{
		memcpy(pCount.szRivalGuild, gObj[pnumber].lpGuild->szGuildRivalName, MAX_GUILD_LEN);
		_GUILD_INFO_STRUCT* lpRivalGuild = Guild.SearchGuild(gObj[pnumber].lpGuild->szGuildRivalName);

		if( lpRivalGuild )
		{
			memcpy(pCount.btUnk,lpRivalGuild->Mark,sizeof(lpRivalGuild->Mark));
			pCount.btCount = lpRivalGuild->Count;
		}
	}

	pCount.Result = 1;
	guildmaster = 1;

	if ( pCount.Count == 0 )
		return;

	pCount.Count = 0;

	for ( n=0;n<MAX_USER_GUILD;n++)
	{
		number = gObj[pnumber].lpGuild->Use[n];

		if ( number > 0 )
		{
			memset(&pList, 0, sizeof(pList));
			memcpy(pList.Name, gObj[pnumber].lpGuild->Names[n], sizeof(pList.Name));
			pList.Number = gObj[pnumber].lpGuild->pServer[n];
			pList.ConnectAServer = (short)gObj[pnumber].lpGuild->pServer[n] & 0x7F;

			if ( gObj[pnumber].lpGuild->pServer[n] >= 0 )
				pList.ConnectAServer |= 0x80;

			pList.btGuildStatus = gObj[pnumber].lpGuild->GuildStatus[n];
			memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
			lOfs+= sizeof(pList);
			pCount.Count++;
		}
	}

	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(sendbuf, (LPBYTE)&pCount, sizeof(pCount));

	DataSend(pnumber, (LPBYTE)sendbuf, lOfs);
}


struct PMSG_GUILDDELUSER_RESULT
{
	PBMSG_HEAD h;	// C1:53
	BYTE Result;	// 3
};


void CGGuildDelUser(PMSG_GUILDDELUSER * lpMsg, int aIndex)
{
	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	if ( gObj[aIndex].GuildNumber < 1 )
		return;

	char joomin[21];
	char memberid[11];
	char guildname[11];
	PMSG_GUILDDELUSER_RESULT pMsg;
	
	PHeadSetB((LPBYTE)&pMsg, 0x53, sizeof(pMsg));
	pMsg.Result = 3;

	if ( !gGuildDestroy )
	{
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

#if (ENABLETEST_ARCA == 1)
	int iArcaBattleState = g_ArcaBattle.GetState();

	if( iArcaBattleState > AB_STATE_CLOSED && iArcaBattleState < AB_STATE_CLOSE )
	{
		pMsg.Result = 6;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}
#endif

	memset(memberid, 0, sizeof(memberid));
	memset(guildname, 0, sizeof(guildname));
	memcpy(memberid, lpMsg->Name, MAX_ACCOUNT_LEN);
	strcpy(guildname, gObj[aIndex].lpGuild->Name);

	if ( !strcmp(gObj[aIndex].lpGuild->Names[0], gObj[aIndex].Name ))
	{
		memset(joomin, 0, sizeof(joomin));
		memcpy(joomin, lpMsg->JoominNumber, 20);

#if (__CUSTOM__ == 1)
		if ( gc_bPersonalID == FALSE || gObjJoominCheck(aIndex, joomin) == TRUE )
#else
		if ( gObjJoominCheck(aIndex, joomin) == TRUE )
#endif
		{
			if ( !strcmp(memberid, gObj[aIndex].Name ))
			{
				gObjGuildWarMasterClose(&gObj[aIndex]);
				GDGuildDestroySend(aIndex, gObj[aIndex].lpGuild->Name, gObj[aIndex].Name);
			}
			else
			{
				GDGuildMemberDel(aIndex, guildname, memberid);
			}

			LogAddTD("[%s][%s] Guild Request - Dismiss All : %s",
				gObj[aIndex].AccountID,gObj[aIndex].Name, guildname);
		}
		else
		{
			pMsg.Result = 0;
			DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}
	}
	else
	{
		if ( strcmp(memberid, gObj[aIndex].Name) )
		{
			pMsg.Result = 0;
			DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}

		memset(joomin, 0, sizeof(joomin));
		memcpy(joomin, lpMsg->JoominNumber, 20);

#if (__CUSTOM__ == 1)
		if ( gc_bPersonalID == FALSE || gObjJoominCheck(aIndex, joomin) == TRUE )
#else
		if ( gObjJoominCheck(aIndex, joomin) == TRUE )
#endif
		{
			GDGuildMemberDel(aIndex, guildname, memberid);
		}
		else
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 108)), aIndex, 1);
		}

		LogAddTD("[%s][%s] Guild Request - Leave : %s",
			gObj[aIndex].AccountID, gObj[aIndex].Name, guildname);
	}
}




void GCGuildDelUserResult(int aIndex, BYTE Result)
{
	PMSG_GUILDDELUSER_RESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x53, sizeof(pMsg));
	pMsg.Result = Result;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}




struct PMSG_GUILDMASTERQUESTION
{
	PBMSG_HEAD h;	// C1:54
};


void GCGuildMasterQuestionSend(int aIndex)
{
	if ( gObj[aIndex].m_IfState.use > 0 )
	{
		return;
	}

	PMSG_GUILDMASTERQUESTION pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x54, sizeof(pMsg));
	gObj[aIndex].m_IfState.use = 1;
	gObj[aIndex].m_IfState.type = 5;
	gObj[aIndex].m_IfState.state = 0;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}




void CGGuildMasterAnswerRecv(PMSG_GUILDMASTERANSWER * lpMsg, int aIndex)
{
	if ( lpMsg->Result == 1 )
	{
		if ( !gObjGuildMasterCapacityTest(&gObj[aIndex]))
		{
			if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type == 5 )
			{
				gObj[aIndex].m_IfState.use = 0;
			}

			return;
		}

		GCGuildMasterManagerRun(aIndex);
		return;
	}

	if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type == 5 )
	{
		gObj[aIndex].m_IfState.use = 0;
	}
}



struct PMSG_GUILDMASTERMANAGER_RUN
{
	PBMSG_HEAD h;	// C1:55
};


void GCGuildMasterManagerRun(int aIndex)
{
	PMSG_GUILDMASTERMANAGER_RUN pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x55, sizeof(pMsg));

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}




void CGGuildMasterInfoSave(int aIndex, PMSG_GUILDINFOSAVE * lpMsg)
{
	char GuildName[MAX_GUILD_LEN+1];
	GuildName[MAX_GUILD_LEN] ='\0';
	memcpy(GuildName, lpMsg->GuildName, MAX_GUILD_LEN);
	int len = strlen(GuildName);

	if ( len <= 2 || len > MAX_GUILD_LEN)
	{
		PMSG_GUILDCREATED_RESULT pMsg;

		PHeadSetB((LPBYTE)&pMsg, 0x56, sizeof(pMsg));
		pMsg.Result = 2;
		
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

		if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type == 5 )
		{
			gObj[aIndex].m_IfState.use = 0;
		}

		return;
	}

	if( g_MultilangGuildName == 0 )
	{
		for(int i = 0; i < len; i++)
		{
			if( !isalnum(GuildName[i]) )	
			{
				PMSG_GUILDCREATED_RESULT pMsg;

				PHeadSetB((LPBYTE)&pMsg, 0x56, sizeof(pMsg));
				pMsg.Result = 5;

				DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

				if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type == 5 )	
				{
					gObj[aIndex].m_IfState.use = 0;
				}
				return;
			}
		}
	}

	GDGuildCreateSend(aIndex, GuildName, gObj[aIndex].Name, lpMsg->Mark, lpMsg->btGuildType);

	if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type == 5 )
	{
		gObj[aIndex].m_IfState.use = 0;
	}
}




void CGGuildMasterCreateCancel(int aIndex)
{
	if ( gObj[aIndex].GuildNumber > 0 )
		return;

	if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type == 5 )
	{
		gObj[aIndex].m_IfState.use = 0;
	}
}



void GCGuildViewportNowPaint(int aIndex, char* guildname, BYTE* mark, BOOL isGuildMaster)
{
	_GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(guildname);

	if ( lpGuild == NULL )
		return;

	LPOBJ lpObj = &gObj[aIndex];
	BYTE _GuildInfoBuf[256] = {0};
	int _GuildInfoOfs = 5;

	if ( lpObj->lpGuild == NULL )
		return;


	PMSG_SIMPLE_GUILDVIEWPORT pMsg;
	pMsg.GuildNumber = lpObj->lpGuild->Number;
	pMsg.NumberH = SET_NUMBERH(aIndex)&0x7F;
	pMsg.NumberL = SET_NUMBERL(aIndex);

	if ( isGuildMaster != FALSE )
		pMsg.NumberH |= 0x80;


	if ( isGuildMaster != FALSE )
		pMsg.btGuildStatus = 0x80;
	else
		pMsg.btGuildStatus = lpObj->GuildStatus;

	pMsg.btGuildType = lpObj->lpGuild->btGuildType;
	pMsg.btGuildRelationShip = 0;
	lpObj->iGuildUnionTimeStamp = 0;
	memcpy(&_GuildInfoBuf[_GuildInfoOfs], &pMsg, sizeof(pMsg));
	_GuildInfoOfs += sizeof(pMsg);

	PMSG_SIMPLE_GUILDVIEWPORT_COUNT pGVCount;

	pGVCount.h.c = 0xC2;
	pGVCount.h.headcode = 0x65;
	pGVCount.h.sizeH = SET_NUMBERH(_GuildInfoOfs);
	pGVCount.h.sizeL = SET_NUMBERL(_GuildInfoOfs);
	pGVCount.Count = 1;
	memcpy(_GuildInfoBuf, &pGVCount, sizeof(pGVCount));

	if ( !CC_MAP_RANGE(gObj[aIndex].MapNumber) )
		DataSend(aIndex, _GuildInfoBuf, _GuildInfoOfs);

	MsgSendV2(lpObj, _GuildInfoBuf, _GuildInfoOfs);
}




struct PMSG_GUILDDEL_VIEWPORT_NOW
{
	PBMSG_HEAD h;	// C1:5D
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};




void GCGuildViewportDelNow(int aIndex, BOOL isGuildMaster)
{
	PMSG_GUILDDEL_VIEWPORT_NOW pMsg;

	LPOBJ lpObj = &gObj[aIndex];

	PHeadSetB((LPBYTE)&pMsg, 0x5D, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex) & 0x7F;
	pMsg.NumberL = SET_NUMBERL(aIndex);

	if ( isGuildMaster != FALSE )
	{
		pMsg.NumberH |= 0x80;
	}

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
	MsgSendV2(lpObj, (LPBYTE)&pMsg, pMsg.h.size);
}




void GCManagerGuildWarEnd(char * GuildName)
{
	_GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(GuildName);

	if ( lpNode == NULL )
	{
		return;
	}

	int n=0;
	int warmaster = -1;

	while ( true )
	{
		if ( gObj[n].Type == OBJ_USER )
		{
			if ( gObj[n].Connected > PLAYER_LOGGED )
			{
				if ( gObj[n].Name[0] == lpNode->Names[0][0] )
				{
					if ( strcmp(gObj[n].Name, lpNode->Names[0]) == 0 )
					{
						warmaster = n;
						break;
					}
				}
			}
		}

		if ( n < OBJMAX-1 )
		{
			n++;
		}
		else
		{
			break;
		}
	}

	if ( warmaster >= 1 )
	{
		BYTE Result1 = 0;
		BYTE Result2 = 0;

		if ( lpNode != NULL )
		{
			if ( lpNode->lpTargetGuildNode != NULL )
			{
				if ( lpNode->PlayScore == 0 && lpNode->lpTargetGuildNode->PlayScore == 0 )
				{
					Result1 = 6;
					Result2 = 6;
				}
				else
				{
					Result1 = 0;
					Result2 = 1;
				}
			}
		}

		if ( lpNode->WarType == 1 )
		{
			gBattleGroundEnable(lpNode->BattleGroundIndex, FALSE );
			gObjGuildWarEndSend(lpNode, lpNode->lpTargetGuildNode, Result1, Result2);
			gObjGuildWarEnd(lpNode, lpNode->lpTargetGuildNode);

			//cManager.BattleInfoSend(::GetBattleTeamName(0, 0), -1, ::GetBattleTeamName(0, 1), -1);
		}
		else
		{
			gObjGuildWarEndSend(lpNode, lpNode->lpTargetGuildNode, Result1, Result2);
			gObjGuildWarEnd(lpNode, lpNode->lpTargetGuildNode);
		}
	}
}




void GCManagerGuildWarSet(LPSTR GuildName1, LPSTR GuildName2, int type)
{
	_GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(GuildName1);

	if ( lpNode == NULL )
	{
		return;
	}

	int n=0;
	int warmaster = -1;

	while ( true )
	{
		if ( gObj[n].Type == OBJ_USER )
		{
			if ( gObj[n].Connected > PLAYER_LOGGED )
			{
				if ( gObj[n].Name[0] == lpNode->Names[0][0] )
				{
					if ( strcmp(gObj[n].Name, lpNode->Names[0]) == 0 )
					{
						warmaster = n;
						break;
					}
				}
			}
		}

		if ( n < OBJMAX-1 )
		{
			n++;
		}
		else
		{
			break;
		}
	}

	if ( warmaster >= 1 )
	{
		::GCGuildWarRequestResult(GuildName2, warmaster, type);
	}
}



struct PMSG_GUILDWARREQUEST_RESULT
{
	PBMSG_HEAD h;	// C1:60
	BYTE Result;	// 3
};


void GCGuildWarRequestResult(LPSTR GuildName, int aIndex, int type)
{
#if (GS_CASTLE == 1 && GS_SPEC != 1)
	return;
#endif
	PMSG_GUILDWARREQUEST_RESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x60, sizeof(pMsg));
	pMsg.Result = 3;

	if ( gObj[aIndex].GuildNumber < 1)
	{
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	_GUILD_INFO_STRUCT * lpMyGuild = gObj[aIndex].lpGuild;

	if ( !lpMyGuild )
	{
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if ( lpMyGuild->WarState == 1 || lpMyGuild->WarDeclareState == 1 )
	{
		pMsg.Result = 4;

		//Error on declaring war : %d %d %s
		LogAdd(lMsg.Get(MSGGET(1, 234)), lpMyGuild->WarState,
			lpMyGuild->WarDeclareState, lpMyGuild->Name);
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

		return;
	}

	if ( gPkLimitFree == FALSE && gObj[aIndex].m_PK_Level >= 6)
	{
		pMsg.Result = 4;

		LogAdd("Error on declaring war : GuildMaster Is Murderer 0x04 %s %s",
			lpMyGuild->Names[0], gObj[aIndex].Name);

		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

		return;
	}

	if ( strcmp(lpMyGuild->Names[0], gObj[aIndex].Name ) )
	{
		pMsg.Result = 5;

		LogAdd(lMsg.Get(MSGGET(1, 235)),
			lpMyGuild->Names[0], gObj[aIndex].Name);
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

		return;
	}

	char _guildname[MAX_GUILD_LEN+1];
	memset(_guildname, 0, sizeof(_guildname));
	memcpy(_guildname, GuildName, MAX_GUILD_LEN);

	if ( !strncmp(lpMyGuild->Name, GuildName, MAX_GUILD_LEN))
		return;

	_GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(_guildname);

	if ( lpNode )
	{
		if ( lpNode->WarState == 1 || lpNode->WarDeclareState == 1 )
		{
			pMsg.Result = 4;

			//Error on declaring war : %d %d %s
			LogAdd(lMsg.Get(MSGGET(1, 234)), lpNode->WarState,
				lpNode->WarDeclareState, lpNode->Name);
			DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		if ( lpMyGuild->iGuildUnion != 0 && lpMyGuild->iGuildUnion == lpNode->iGuildUnion )
		{
			LogAddTD("[U.System][Guildwar] Can't declare guildwar between Same Union (%s / %d) <-> (%s / %d)",
				lpMyGuild->Name, lpMyGuild->iGuildUnion, lpNode->Name, lpMyGuild->iGuildUnion);

			return;
		}

		int n=OBJ_STARTUSERINDEX;	// #warning Change this 0 to OBJ_STARTUSERINDEX
		int warmaster=-1;

		while ( true )
		{
			if ( gObj[n].Type == OBJ_USER )
			{
				if ( gObj[n].Connected > PLAYER_LOGGED )
				{
					if ( gObj[n].Name[0] == lpNode->Names[0][0] )
					{
						if ( !strcmp(gObj[n].Name, lpNode->Names[0]))
						{
							if ( gPkLimitFree == FALSE && gObj[n].m_PK_Level >= 6)
							{
								pMsg.Result = 4;

								LogAdd("Error on declaring war : Target GuildMaster Is Murderer 0x04 %s %s",
									lpMyGuild->Names[0], gObj[aIndex].Name);

								DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

								return;
							}

							warmaster = n;
							break;
						}
					}
				}
			}

			if ( n < OBJMAX-1 )
				n++;
			else
				break;
		}
		
		if ( warmaster >= 1 )
		{
			if ( CC_MAP_RANGE(gObj[aIndex].MapNumber) || CC_MAP_RANGE(gObj[warmaster].MapNumber) )
			{
				GCServerMsgStringSend(lMsg.Get(MSGGET(4, 199)), aIndex, 1);
				return;
			}

			if ( IT_MAP_RANGE(gObj[aIndex].MapNumber) || IT_MAP_RANGE(gObj[warmaster].MapNumber) )
			{
				GCServerMsgStringSend("Guild War cannot be started within Illusion Temple.", aIndex, 1);
				return;
			}

			if ( DG_MAP_RANGE(gObj[aIndex].MapNumber) || DG_MAP_RANGE(gObj[warmaster].MapNumber) )
			{
				GCServerMsgStringSend("Guild War cannot be started within Doppelganger.", aIndex, 1);
				return;
			}

			if ( IF_MAP_RANGE(gObj[aIndex].MapNumber) || IF_MAP_RANGE(gObj[warmaster].MapNumber) )
			{
				GCServerMsgStringSend("Guild War cannot be started within Imperial Fort.", aIndex, 1);
				return;
			}

			if ( (gObj[warmaster].m_Option&1) != 1 )
			{
				pMsg.Result = 4;
				DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

				return;
			}

			pMsg.Result = 1;
			DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
			GCGuildWarRequestSend(lpMyGuild->Name, warmaster, type);
			lpMyGuild->WarDeclareState = 1;
			lpNode->WarDeclareState = 1;
			lpMyGuild->WarType = type;
			lpNode->WarType = type;

			// Declared guild war [%s][%s] sGuild:(%s) TargetGuild:(%s)
			LogAddTD(lMsg.Get(MSGGET(1, 236)), gObj[aIndex].AccountID, gObj[aIndex].Name,
				lpMyGuild->Name, lpNode->Name);

			strcpy(lpMyGuild->TargetGuildName, lpNode->Name);
			strcpy(lpNode->TargetGuildName, lpMyGuild->Name);
			lpMyGuild->lpTargetGuildNode = lpNode;
			lpNode->lpTargetGuildNode = lpMyGuild;
		}
		else
		{
			pMsg.Result = 2;
			DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

			//return;
		}
	}
	else
	{
		pMsg.Result = 0;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

		return;
	}
}



struct PMSG_GUILDWARSEND
{
	PBMSG_HEAD h;	// C1:61
	char GuildName[8];	// 3
	BYTE Type;	// B
};


void GCGuildWarRequestSend(LPSTR GuildName, int aIndex, int type)
{
#if (GS_CASTLE == 0 || GS_SPEC == 1)
	PMSG_GUILDWARSEND pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x61, sizeof(pMsg));
	pMsg.Type = type;
	memcpy(pMsg.GuildName, GuildName, MAX_GUILD_LEN);

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

	// (%s) guild declared guild war
	LogAddTD(lMsg.Get(MSGGET(1, 237)), GuildName);
#endif
}



struct PMSG_GUILDWAR_DECLARE
{
	PBMSG_HEAD h;	// C1:62
	char GuildName[8];	// 3
	BYTE Type;	// B
	BYTE TeamCode;	// C
};



void GCGuildWarRequestSendRecv(PMSG_GUILDWARSEND_RESULT * lpMsg, int aIndex)
{
#if (GS_CASTLE == 1 && GS_SPEC != 1)
	return;
#endif
	PMSG_GUILDWAR_DECLARE pMsg;
	int count=0;
	int g_call=0;
	int n;
	
	PHeadSetB((LPBYTE)&pMsg, 0x62, sizeof(pMsg));
	pMsg.Type = 0;
	
	_GUILD_INFO_STRUCT * lpMyNode = gObj[aIndex].lpGuild;

	if ( !lpMyNode )
	{
		PMSG_GUILDWARREQUEST_RESULT pResult;

		pResult.h.c = 0xC1;
		pResult.h.headcode = 0x60;
		pResult.h.size = sizeof(pResult);
		pResult.Result = 0;

		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( lpMyNode->lpTargetGuildNode && lpMsg->Result )
	{
		int iTarGetIndex = lpMyNode->lpTargetGuildNode->Index[0];

		if ( BC_MAP_RANGE(gObj[iTarGetIndex].MapNumber) ||
			 CC_MAP_RANGE(gObj[iTarGetIndex].MapNumber) ||
			 DS_MAP_RANGE(gObj[iTarGetIndex].MapNumber) ||
			 IT_MAP_RANGE(gObj[iTarGetIndex].MapNumber) ||
			 DG_MAP_RANGE(gObj[iTarGetIndex].MapNumber) ||
			 IF_MAP_RANGE(gObj[iTarGetIndex].MapNumber)
			 )
		{
			PMSG_NOTICE pNotice;

			TNotice::MakeNoticeMsgEx(&pNotice, 1, lMsg.Get(MSGGET(4, 199)));

			DataSend(aIndex, (LPBYTE)&pNotice, pNotice.h.size);

			lpMsg->Result = 0;
		}
	}

	pMsg.Type = lpMyNode->WarType;

	if ( lpMsg->Result == 0 )
	{
		if ( lpMyNode->lpTargetGuildNode )
		{
			if ( lpMyNode->lpTargetGuildNode->WarDeclareState == 1 )
			{
				lpMyNode->lpTargetGuildNode->WarDeclareState = 0;
				lpMyNode->WarDeclareState = 0;

				PMSG_GUILDWARREQUEST_RESULT pResult;

				PHeadSetB((LPBYTE)&pResult, 0x60, sizeof(pResult));
				pResult.Result = 6;

				if ( lpMyNode->lpTargetGuildNode->Index[0] >= 0 )
				{
					DataSend(lpMyNode->lpTargetGuildNode->Index[0], (LPBYTE)&pResult, pResult.h.size);
				}
			}
		}
	}
	else //if ( lpMsg->Result == 1 )
	{
		if ( lpMyNode->WarDeclareState == 1 )
		{
			if ( lpMyNode->lpTargetGuildNode )
			{
				if (lpMyNode->lpTargetGuildNode->WarDeclareState == 1 )
				{
					PMSG_TELEPORT pTeleportMsg;

					if ( lpMyNode->WarType == 1 )
					{
						lpMyNode->BattleGroundIndex = gCheckBlankBattleGround();

						switch ( lpMyNode->BattleGroundIndex )
						{
							case 0xFF:
								lpMyNode->WarDeclareState = 0;
								lpMyNode->WarState = 0;
								lpMyNode->lpTargetGuildNode->WarDeclareState = 0;
								lpMyNode->lpTargetGuildNode->WarState = 0;

								PMSG_GUILDWARREQUEST_RESULT pResult;

								PHeadSetB((LPBYTE)&pResult, 0x60, sizeof(pResult));
								pResult.Result = 4;

								DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
								return;
						}
	
						pTeleportMsg.MoveNumber = 51;
						lpMyNode->lpTargetGuildNode->BattleGroundIndex = lpMyNode->BattleGroundIndex;

						if ( gBSGround[0]->m_BallIndex >= 0 )
							gObjMonsterRegen(&gObj[gBSGround[0]->m_BallIndex]);

						BattleSoccerGoalEnd(0);
						lpMyNode->PlayScore = 0;
						lpMyNode->lpTargetGuildNode->PlayScore = 0;
						gBattleGroundEnable(lpMyNode->BattleGroundIndex, TRUE);
						gSetBattleTeamMaster(lpMyNode->BattleGroundIndex, 0, lpMyNode->Name, lpMyNode);
						gSetBattleTeamMaster(lpMyNode->BattleGroundIndex, 1, lpMyNode->lpTargetGuildNode->Name, lpMyNode->lpTargetGuildNode);
					}

					lpMyNode->WarDeclareState = 2;
					lpMyNode->WarState = 1;
					lpMyNode->lpTargetGuildNode->WarDeclareState = 2;
					lpMyNode->lpTargetGuildNode->WarState = 1;
					lpMyNode->PlayScore = 0;
					lpMyNode->lpTargetGuildNode->PlayScore = 0;
					lpMyNode->BattleTeamCode = 0;
					lpMyNode->lpTargetGuildNode->BattleTeamCode = 1;
					memset(pMsg.GuildName, 0, sizeof(pMsg.GuildName));
					memcpy(pMsg.GuildName, lpMyNode->lpTargetGuildNode->Name, sizeof(pMsg.GuildName));
					pMsg.TeamCode = lpMyNode->BattleTeamCode;
					count = 0;

					for ( n=0;n<MAX_USER_GUILD;n++)
					{
						if ( lpMyNode->Use[n] )
						{
							if ( lpMyNode->Index[n] >= 0 )
							{
								g_call = 0;

								if ( n > 0 )
								{
									if ( lpMyNode->WarType == 1 )
									{
										if ( gObj[lpMyNode->Index[0]].PartyNumber  >= 0 )
										{
											if ( gObj[lpMyNode->Index[0]].PartyNumber == gObj[lpMyNode->Index[n]].PartyNumber )
											{
												g_call = 1;
											}
										}
									}
									else 
									{
										g_call = 1;
									}
								}
								else if ( n== 0 )
								{
									g_call = 1;
								}

								if ( g_call )
								{
									DataSend(lpMyNode->Index[n], (LPBYTE)&pMsg, pMsg.h.size);

									// Declared guild war against (%s)
									LogAddTD(lMsg.Get(MSGGET(1, 238)), lpMyNode->Name);
									GCGuildWarScore(lpMyNode->Index[n]);
									int x = 60;

									if ( lpMyNode->WarType == 1 )
									{
										gObj[lpMyNode->Index[n]].IsInBattleGround = true;
										pTeleportMsg.MoveNumber = 51;

										if ( gPkLimitFree || gObj[lpMyNode->Index[n]].m_PK_Level < 6 )
										{
											gObjTeleport(lpMyNode->Index[n], 6, x++, 153);
											count++;
										}
									}
								}
							}
						}
					}

					memset(pMsg.GuildName, 0, sizeof(pMsg.GuildName));
					memcpy(pMsg.GuildName, lpMyNode->Name, sizeof(pMsg.GuildName));

					pMsg.TeamCode = lpMyNode->lpTargetGuildNode->BattleTeamCode;
					count = 0;

					for ( n=0;n<MAX_USER_GUILD;n++)
					{
						if ( lpMyNode->lpTargetGuildNode->Use[n] )
						{
							if ( lpMyNode->lpTargetGuildNode->Index[n] >= 0 )
							{
								g_call = 0;

								if ( n > 0 )
								{
									if ( lpMyNode->WarType == 1 )
									{
										if ( gObj[lpMyNode->lpTargetGuildNode->Index[0]].PartyNumber  >= 0 )
										{
											if ( gObj[lpMyNode->lpTargetGuildNode->Index[0]].PartyNumber == gObj[lpMyNode->lpTargetGuildNode->Index[n]].PartyNumber )
											{
												g_call = 1;
											}
										}
									}
									else 
									{
										g_call = 1;
									}
								}
								else if ( n== 0 )
								{
									g_call = 1;
								}

								if ( g_call )
								{
									DataSend(lpMyNode->lpTargetGuildNode->Index[n], (LPBYTE)&pMsg, pMsg.h.size);

									// Declared guild war against (%s)
									LogAddTD(lMsg.Get(MSGGET(1, 238)), lpMyNode->lpTargetGuildNode->Name);
									GCGuildWarScore(lpMyNode->lpTargetGuildNode->Index[n]);
									int x = 59;

									if ( lpMyNode->lpTargetGuildNode->WarType == 1 )
									{

										if ( gPkLimitFree != 0 || gObj[lpMyNode->lpTargetGuildNode->Index[n]].m_PK_Level < 6 )
										{
											gObj[lpMyNode->lpTargetGuildNode->Index[n]].IsInBattleGround = true;
											gObjTeleport(lpMyNode->lpTargetGuildNode->Index[n], 6, x++, 164);
											count++;
										}
									}
								}
							}
						}
					}

//					cManager.BattleInfoSend(GetBattleTeamName(0,0),
//											GetBattleTeamScore(0,0),
//											GetBattleTeamName(0, 1),
//											GetBattleTeamScore(0,1));

					if ( lpMyNode->WarType == 1 )
					{
						gObjAddMsgSendDelay(&gObj[aIndex], 5, aIndex, 10000, 0);
						GCServerMsgStringSendGuild(lpMyNode->lpTargetGuildNode,lMsg.Get(MSGGET(4, 130)), 1);
						GCServerMsgStringSendGuild(lpMyNode,lMsg.Get(MSGGET(4, 130)), 1);
					}
				}
			}
		}
	}
}





void GCGuildWarDeclare(int aIndex, LPSTR _guildname)
{
#if (GS_CASTLE == 0 || GS_SPEC == 1)
	PMSG_GUILDWAR_DECLARE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x62, sizeof(pMsg));
	memcpy(pMsg.GuildName, _guildname, sizeof(pMsg.GuildName));

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
#endif
}



struct PMSG_GUILDWAR_END
{
	PBMSG_HEAD h;	// C1:63
	BYTE Result;	// 3
	char GuildName[8];	// 4
};


void GCGuildWarEnd(int aIndex, BYTE result, char* _guildname)
{
#if (GS_CASTLE == 0 || GS_SPEC == 1)
	PMSG_GUILDWAR_END pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x63, sizeof(pMsg));
	pMsg.Result = result;
	memcpy(pMsg.GuildName, _guildname, sizeof(pMsg.GuildName));

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
#endif
}



struct PMSG_GUILDSCORE
{
	PBMSG_HEAD h;	// C1:64
	BYTE Score1;	// 3
	BYTE Score2;	// 4
	BYTE Type;	// 5
};



void GCGuildWarScore(int aIndex)
{
#if (GS_CASTLE == 0 || GS_SPEC == 1)
	if ( gObj[aIndex].GuildNumber < 1 )
		return;

	PMSG_GUILDSCORE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x64, sizeof(pMsg));
	pMsg.Score1 = 0;
	pMsg.Score2 = 0;
	pMsg.Type = 0;

	if ( gObj[aIndex].lpGuild != NULL )
	{
		pMsg.Score1 = gObj[aIndex].lpGuild->PlayScore;
		LogAdd("Score %s %d", gObj[aIndex].Name, gObj[aIndex].lpGuild->PlayScore);
	}

	if ( gObj[aIndex].lpGuild->lpTargetGuildNode != NULL )
	{
		pMsg.Score2 = gObj[aIndex].lpGuild->lpTargetGuildNode->PlayScore;
		LogAdd("Target Score %s %d", gObj[aIndex].Name, gObj[aIndex].lpGuild->lpTargetGuildNode->PlayScore);
	}

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
#endif
}	




void CGWarehouseMoneyInOut(int aIndex, PMSG_WAREHOUSEMONEYINOUT* lpMsg)
{
	int money = 0;

	if ( !gObjIsConnected(aIndex))
	{
		LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
		GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	money = lpMsg->Money;
	
	if ( lpObj->m_IfState.state == 0 )
	{
		GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
		return;
	}

	if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 6 )
	{
		GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
		LogAdd("[%s][%s] error-L1 : used not Warehouse",
			gObj[aIndex].AccountID, gObj[aIndex].Name);

		return;
	}

	switch ( lpMsg->Type )
	{
		case 0x00:
			if ( money <= 0 || money > MAX_WAREHOUSE_ZEN )
			{
				GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
				return;
			}

			if ( (DWORD)(lpObj->WarehouseMoney+money) > MAX_WAREHOUSE_ZEN )
			{
				GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
				return;
			}

			if ( money > lpObj->Money )
			{
				GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
				return;
			}

			lpObj->Money-=money;
			lpObj->WarehouseMoney+=money;
			break;
		case 0x01:
			{
				if ( bCanWarehouseLock == TRUE)
				{
					if ( lpObj->WarehouseLock == 1)
					{
						GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
						GCServerMsgStringSend(lMsg.Get(MSGGET(6, 70)), lpObj->m_Index,1);
						return;
					}
				}

				if ( money <= 0 || money > MAX_WAREHOUSE_ZEN )
				{
					GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
					return;
				}
				
				if ( money > lpObj->WarehouseMoney )
				{
					GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
					return;
				}

				lpObj->WarehouseMoney -= money;
				lpObj->Money += money;
				int rZen = GetWarehouseUsedHowMuch(lpObj->Level,lpObj->MasterLevel, lpObj->WarehousePW);

				if ( (lpObj->Money-rZen) > 0 )
				{
					int oldmoney = lpObj->Money;
					lpObj->Money -= rZen;
					GCMoneySend(lpObj->m_Index, lpObj->Money);
					
					LogAdd("Get WareHouse Money(In Inventory) : %d - %d = %d",
						oldmoney, rZen, lpObj->Money);
				}
				else if ( (lpObj->WarehouseMoney-rZen) > 0 )
				{
					int oldmoney = lpObj->WarehouseMoney;
					lpObj->WarehouseMoney -= rZen;

					LogAdd("Get WareHouse Money(In WareHouse) : %d - %d = %d",
						oldmoney, rZen, lpObj->WarehouseMoney);

					GCWarehouseInventoryMoneySend(aIndex, 1, lpObj->Money, lpObj->WarehouseMoney);
				}
				else
				{
					lpObj->WarehouseMoney +=money;
					lpObj->Money -= money;
					GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
					char msg[255];

					wsprintf(msg, lMsg.Get(MSGGET(6, 69)), rZen);
					GCServerMsgStringSend(msg, lpObj->m_Index, 1);
					return;
				}
			}
			break;

		default:
			GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
			return;
	}

	lpObj->WarehouseCount++;
	GCWarehouseInventoryMoneySend(aIndex, 1, lpObj->Money, lpObj->WarehouseMoney);
}



struct PMSG_MONEY
{
	PBMSG_HEAD h;	// C1:81
	BYTE Result;	// 3
	int wMoney;	// 4
	int iMoney;	// 8
};


void GCWarehouseInventoryMoneySend(int aIndex, BYTE result, int money, int wmoney)
{
	PMSG_MONEY pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x81, sizeof(pMsg));
	pMsg.Result = result;
	pMsg.iMoney = money;
	pMsg.wMoney = wmoney;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}




void CGWarehouseUseEnd(int aIndex)
{
	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_DEFAULT pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x82, sizeof(pMsg));
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

	gObjItemTextSave(lpObj);
	GDSetWarehouseList(aIndex);
	GDUserItemSave(lpObj);

	if ( lpObj->m_IfState.use  && lpObj->m_IfState.type == 6 )
	{
		lpObj->m_IfState.use = 0;
		lpObj->m_IfState.state = 0;
		lpObj->WarehouseSave = FALSE;
	}
}




struct PMSG_WAREHOUSESTATE
{
	PBMSG_HEAD h;	// C1:83
	BYTE State;	// 3
};



void GCWarehouseStateSend(int aIndex, BYTE state)
{
	PMSG_WAREHOUSESTATE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x83, sizeof(pMsg));
	pMsg.State = state;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}




void GCWarehouseRecivePassword(int aIndex, PMSG_WAREHOUSEPASSSEND* lpMsg)
{
	if ( bCanWarehouseLock == FALSE )
		return;

	int pw = lpMsg->Pass;

	switch ( lpMsg->Type )
	{
		case 0x00:
			if ( gObj[aIndex].WarehouseLock )
			{
				if ( gObj[aIndex].WarehousePW == pw )
				{
					gObj[aIndex].WarehouseLock = FALSE;
					GCWarehouseStateSend(aIndex, 0x0C);
				}
				else
				{
					GCWarehouseStateSend(aIndex, 0x0A);
				}
			}
			break;
		case 0x01:
			if ( !gObj[aIndex].WarehouseLock || gObj[aIndex].WarehouseLock == 0xFF )
			{
				char szJoomin[21];

				memset(szJoomin, 0, sizeof(szJoomin));
				memcpy(szJoomin, lpMsg->LastJoominNumber, 20);

#if (__CUSTOM__ == 1)
				if ( gc_bPersonalID == TRUE && gObjJoominCheck(aIndex, szJoomin) == FALSE )
#else
				if ( gObjJoominCheck(aIndex, szJoomin) == FALSE )
#endif
				{
					GCWarehouseStateSend(aIndex, 0x0D);
					return;
				}

				gObj[aIndex].WarehousePW = pw;
				gObj[aIndex].WarehouseLock = FALSE;
				GCWarehouseStateSend(aIndex, 0x0C);
			}
			else
			{
				GCWarehouseStateSend(aIndex, 0x0B);
			}
			break;
		case 0x02:
			{
				char szJoomin[21];

				memset(szJoomin, 0, sizeof(szJoomin));
				memcpy(szJoomin, lpMsg->LastJoominNumber, 20);

#if (__CUSTOM__ == 1)
				if ( gc_bPersonalID == TRUE && gObjJoominCheck(aIndex, szJoomin) == FALSE )
#else
				if ( gObjJoominCheck(aIndex, szJoomin) == FALSE )
#endif
				{
					GCWarehouseStateSend(aIndex, 0x0D);
					return;
				}

				gObj[aIndex].WarehouseLock = FALSE;
				gObj[aIndex].WarehousePW = 0;
				GCWarehouseStateSend(aIndex, gObj[aIndex].WarehouseLock);
			}
			break;
	}
}	




void GCUserChaosBoxSend(LPOBJ lpObj, int iChaosBoxType)
{
	if ( iChaosBoxType == 1  )
	{
		if ( lpObj->m_IfState.type != 13 )
		{
			return;
		}

		if ( lpObj->m_IfState.type == 13 && lpObj->m_IfState.state == 1 )
		{
			return;
		}
	}
	else 
	{
		if ( lpObj->m_IfState.type != 7 )
		{
			return;
		}

		if ( lpObj->m_IfState.type == 7 && lpObj->m_IfState.state == 1 )
		{
			return;
		}
	}

	PMSG_SHOPITEMCOUNT pMsg;
	BYTE SendByte[2048];
	int lOfs = sizeof(pMsg);
	PHeadSetW((LPBYTE)&pMsg, 0x31, 0);
	pMsg.count = 0;

	if ( iChaosBoxType == 1 )
	{
		pMsg.Type = 5;
	}
	else
	{
		pMsg.Type = 3;
	}

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE )
		{
			SendByte[lOfs] = n;
			lOfs++;
			ItemByteConvert(&SendByte[lOfs], lpObj->pChaosBox[n] );
			lOfs += MAX_ITEM_INFO;
			pMsg.count ++;
		}
	}

	pMsg.h.sizeH = SET_NUMBERH(lOfs);
	pMsg.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(SendByte, &pMsg, sizeof(pMsg));

	DataSend(lpObj->m_Index, SendByte, lOfs);
}




void CGChaosBoxItemMixButtonClick(PMSG_CHAOSMIX* aRecv, int aIndex)
{
	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->ChaosLock == TRUE )
	{
		LogAdd("[%s][%s] Already Used Chaos",
			lpObj->AccountID, lpObj->Name);

		GCServerMsgStringSend(lMsg.Get(MSGGET(2, 198)), lpObj->m_Index, 1);
		return;
	}

	if ( lpObj->m_bPShopOpen == true )
	{
		LogAdd("[%s][%s] is Already Opening PShop, ChaosBox Failed",
			lpObj->AccountID, lpObj->Name);

		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 194)), lpObj->m_Index, 1);
		return;
	}


	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = FALSE;

	if ( aRecv->Type == CHAOS_TYPE_UPGRADE_10 ||
		 aRecv->Type == CHAOS_TYPE_UPGRADE_11 || 
		 aRecv->Type == CHAOS_TYPE_UPGRADE_12 ||
		 aRecv->Type == CHAOS_TYPE_UPGRADE_13 ||
		 aRecv->Type == CHAOS_TYPE_UPGRADE_14 ||
		 aRecv->Type == CHAOS_TYPE_UPGRADE_15 )
	{
		int iPos = 0;
		int iItemCount = 0;

		for(int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if( lpObj->pChaosBox[n].IsItem() == TRUE )
			{
				if( lpObj->pChaosBox[n].m_Level == 9 )
				{
					iPos = n;
					iItemCount++;
				}
				else if( lpObj->pChaosBox[n].m_Level == 10 )
				{
					iPos = n;
					iItemCount++;
				}
				else if( lpObj->pChaosBox[n].m_Level == 11 )
				{
					iPos = n;
					iItemCount++;
				}
				else if( lpObj->pChaosBox[n].m_Level == 12 )
				{
					iPos = n;
					iItemCount++;
				}
				else if( lpObj->pChaosBox[n].m_Level == 13 )
				{
					iPos = n;
					iItemCount++;
				}
				else if( lpObj->pChaosBox[n].m_Level == 14 )
				{
					iPos = n;
					iItemCount++;
				}
			}
		}

		if( iItemCount != 1 )
		{
			pMsg.Result = FALSE;
			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
			return;
		}

		if( lpObj->pChaosBox[iPos].m_bLuckySet )
		{
			pMsg.Result = FALSE;
			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
			return;
		}

		int iWidth = 0;
		int iHeight = 0;

		lpObj->pChaosBox[iPos].GetSize(iHeight,iWidth);

		if( CheckInventoryEmptySpace(lpObj,iHeight,iWidth) != false )
		{

		}
		else
		{
			pMsg.Result = FALSE;
			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
			return;
		}
	}


	lpObj->bIsChaosMixCompleted = true;
	int iMixType = aRecv->Type;

	for ( int n=0;n<CHAOS_BOX_SIZE;n++)
	{
		if ( lpObj->pChaosBox[n].IsItem() == TRUE
			&& lpObj->pChaosBox[n].m_bPeriodItem )
		{
			pMsg.Result = FALSE;
			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
			return;
		}
	}
#if(ENABLETEST_CHAOSMACHINEMNGR == 1)
	if (g_ChaosMachineManager.StartMix(lpObj, iMixType))
	{
		return;
	}
#endif
	switch ( iMixType )	// TEST - Good
	{
		case CHAOS_TYPE_DEVILSQUARE:
			g_ChaosBox.DevilSquareItemChaosMix(lpObj);
			break;
		case CHAOS_TYPE_UPGRADE_10:
			g_ChaosBox.PlusItemLevelChaosMix(lpObj, CHAOS_TYPE_UPGRADE_10);
			break;
		case CHAOS_TYPE_UPGRADE_11:
			g_ChaosBox.PlusItemLevelChaosMix(lpObj, CHAOS_TYPE_UPGRADE_11);
			break;
		case CHAOS_TYPE_UPGRADE_12:
			g_ChaosBox.PlusItemLevelChaosMix(lpObj, CHAOS_TYPE_UPGRADE_12);
			break;
		case CHAOS_TYPE_UPGRADE_13:
			g_ChaosBox.PlusItemLevelChaosMix(lpObj, CHAOS_TYPE_UPGRADE_13);
			break;
		case CHAOS_TYPE_UPGRADE_14:
			g_ChaosBox.PlusItemLevelChaosMix(lpObj, CHAOS_TYPE_UPGRADE_14);
			break;
		case CHAOS_TYPE_UPGRADE_15:
			g_ChaosBox.PlusItemLevelChaosMix(lpObj, CHAOS_TYPE_UPGRADE_15);
			break;
		case CHAOS_TYPE_DINORANT:
			g_ChaosBox.PegasiaChaosMix(lpObj);
			break;
		case CHAOS_TYPE_FRUIT:
			g_ChaosBox.CircleChaosMix(lpObj);
			break;
		case CHAOS_TYPE_SECOND_WING:
		case CHAOS_TYPE_CLOAK:
			g_ChaosBox.WingChaosMix(lpObj);
			break;
		case CHAOS_TYPE_BLOODCATLE:
			g_ChaosBox.BloodCastleItemChaosMix(lpObj);
			break;
		case CHAOS_TYPE_DEFAULT:
		case CHAOS_TYPE_FIRST_WING:
			g_ChaosBox.DefaultChaosMix(lpObj);
			break;
		//case CHAOS_TYPE_SETITEM:	// #warning Activate this to SetItemChaosMix
			//SetItemChaosMix(lpObj);
			//break;
		case CHAOS_TYPE_DARKHORSE:
			g_ChaosBox.DarkHorseChaosMix(lpObj);
			break;
		case CHAOS_TYPE_DARKSPIRIT:
			g_ChaosBox.DarkSpiritChaosMix(lpObj);
			break;
		case CHAOS_TYPE_BLESS_POTION:
			g_ChaosBox.BlessPotionChaosMix(lpObj);
			break;
		case CHAOS_TYPE_SOUL_POTION:
			g_ChaosBox.SoulPotionChaosMix(lpObj);
			break;
		case CHAOS_TYPE_LIFE_STONE:
			g_ChaosBox.LifeStoneChaosMix(lpObj);
			break;
#if (GS_CASTLE == 1)
		case CHAOS_TYPE_CASTLE_SPECIALMIX:
			g_ChaosBox.CastleSpecialItemMix(lpObj);
			break;
#endif
		case CHAOS_TYPE_HT_BOX:
			g_ChaosBox.HiddenTreasureBoxItemMix(lpObj);
			break;
		case CHAOS_TYPE_FENRIR_01:
			g_ChaosBox.Fenrir_01Level_Mix(lpObj);
			break;
		case CHAOS_TYPE_FENRIR_02:
			g_ChaosBox.Fenrir_02Level_Mix(lpObj);
			break;
		case CHAOS_TYPE_FENRIR_03:
			g_ChaosBox.Fenrir_03Level_Mix(lpObj);
			break;
		case CHAOS_TYPE_FENRIR_04:
			g_ChaosBox.Fenrir_04Upgrade_Mix(lpObj);
			break;
		case CHAOS_TYPE_COMPOUNDPOTION_LV1:
			g_ChaosBox.ShieldPotionLv1_Mix(lpObj);
			break;
		case CHAOS_TYPE_COMPOUNTPOTION_LV2:
			g_ChaosBox.ShieldPotionLv2_Mix(lpObj);
			break;
		case CHAOS_TYPE_COMPOUNTPOTION_LV3:
			g_ChaosBox.ShieldPotionLv3_Mix(lpObj);
			break;
		case CHAOS_TYPE_JEWELOFHARMONY_PURITY:
			g_kJewelOfHarmonySystem.PurityJewelOfHarmony(lpObj);
			break;
		case CHAOS_TYPE_JEWELOFHARMONY_MIX_SMELTINGITEM:
			g_kJewelOfHarmonySystem.MakeSmeltingStoneItem(lpObj);
			break;
		case CHAOS_TYPE_JEWELOFHARMONY_RESTORE_ITEM:
			g_kJewelOfHarmonySystem.RestoreStrengthenItem(lpObj);
			break;
		case CHAOS_TYPE_380_OPTIONITEM:
			g_kItemSystemFor380.ChaosMix380ItemOption(lpObj);
			break;
		case CHAOS_TYPE_LOTTERY_MIX:
			g_ChaosBox.LotteryItemMix(lpObj);
			break;
		case CHAOS_TYPE_ILLUSIONMIX:
			g_ChaosBox.IllusionTemple_Mix(lpObj);
			break;
		case CHAOS_TYPE_CONDOR_FEATHER:
			g_ChaosBox.CondorFeather_Mix(lpObj);
			break;
		case CHAOS_TYPE_3RDWING_MIX:
			g_ChaosBox.NewWingChaos_Mix(lpObj);
			break;
		case CHAOS_TYPE_CHERRY_BLOSSOM:
			g_ChaosBox.CherryBlossomMix(lpObj);
			break;
		case CHAOS_TYPE_SEEDEXTRACT:
			g_ChaosBox.SeedExtractMix(lpObj);
			break;
		case CHAOS_TYPE_SPHEREMIX:
			g_ChaosBox.SeedSphereCompositeMix(lpObj);
			break;
		case CHAOS_TYPE_SPHEREMOUNT:
			g_ChaosBox.SetSeedSphereMix(lpObj,aRecv->Pos);
			break;
		case CHAOS_TYPE_SPHEREUNMOUNT:
			g_ChaosBox.RemoveSeedSphereMix(lpObj,aRecv->Pos);
			break;
		case CHAOS_TYPE_DUNGEON_KEYITEM:
			g_ChaosBox.SecromiconMix(lpObj);
			break;
		case CHAOS_TYPE_GOLD_AND_SILBER_BOX:
			g_ChaosBox.GoldenNSilverBoxMix(lpObj);
			break;
		case CHAOS_TYPE_LUCKYSYSTEM1:
			g_ChaosBox.LuckySystemItemCreate(lpObj);
			break;
		case CHAOS_TYPE_LUCKYSYSTEM2:
			g_ChaosBox.LuckySystemJewelCreate(lpObj);
			break;
		case CHAOS_TYPE_SNAKEBEGINS:
			g_SnakeBegins.Mix(lpObj);
			break;
#if (ENABLETEST_ARCA == 1)
		case CHAOS_TYPE_ARCA_BOOTY:
			g_ArcaBattle.BootyExchange(lpObj);
			break;
		case CHAOS_TYPE_ARCA_REGMARK:
			//CGReqMarkRegButtonClick(lpObj);
			break;
#endif
		case CHAOS_TYPE_25WINGMIX:
			g_ChaosBox.Wing25Mix(lpObj);
			break;
		case CHAOS_TYPE_SOCKETMIX1:
			g_ChaosBox.SocketItemMix(lpObj,iMixType);
			break;

#if (CUSTOM_WINGS == 1)
		case CHAOS_TYPE_4THWINGS:
			g_ChaosBox.Create4thWings(lpObj);
			break;
#endif
#ifdef __NOVUS__
		case CHAOS_TYPE_RAREITEM:
			g_ChaosBox.RareItem(lpObj);
			break;
#endif
		default:
			LogAddTD("[%s][%s] Undefine chaosmix type detect %d",
				lpObj->AccountID, lpObj->Name, iMixType);
			break;
	}
}




void CGChaosBoxUseEnd(int aIndex)
{
	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_DEFAULT pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x87, sizeof(pMsg));
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

	if ( lpObj->m_IfState.use && lpObj->m_IfState.type == 7 )
	{
		lpObj->m_IfState.use = 0;
		lpObj->m_IfState.state = 0;
	}

	g_ChaosBox.ChaosBoxInit(lpObj);

	if(lpObj->pTransaction == 1)
	{
		gObjInventoryCommit(lpObj->m_Index);
	}

	lpObj->m_bIsCastleNPCUpgradeCompleted = false;
}




struct PMSG_ACTIONRESULT
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Dir;	// 5
	BYTE ActionNumber;	// 6
	BYTE TargetNumberH;	// 7
	BYTE TargetNumberL;	// 8
};


void PMoveProc(PMSG_MOVE* lpMove, int aIndex)
{
	if ( !OBJMAX_RANGE(aIndex))
	{
		LogAdd("error-L3 : move protocol index error %s %d", __FILE__, __LINE__);
		return;
	}


	PMSG_RECVMOVE pMove;
	short n;
	short pathtable;
	BYTE ax;
	BYTE ay;
	int sx;
	int sy;
	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->RegenOk > 0 )
		return;

	if ( lpObj->SkillRecallParty_Time )
	{
		lpObj->SkillRecallParty_Time = 0;
		char msg[255];
		wsprintf(msg, lMsg.Get(MSGGET(4, 230)));
		GCServerMsgStringSend(msg, lpObj->m_Index, 1);
	}

	if ( (GetTickCount()-lpObj->m_LastMoveTime) < 100 )
		return;

	if ( lpObj->Teleport )
		return;

	if ( gObjCheckUsedBuffEffect(lpObj,BUFF_ICEARROW) == TRUE ||
		 gObjCheckUsedBuffEffect(lpObj,BUFF_STUN) == TRUE ||
		 gObjCheckUsedBuffEffect(lpObj,BUFF_SLEEP) == TRUE ||
		 gObjCheckUsedBuffEffect(lpObj,BUFF_FREEZE) == TRUE ||
		 gObjCheckUsedBuffEffect(lpObj,BUFF_EARTHBINDS) == TRUE )
	{
		return;
	}

	lpObj->m_LastMoveTime = GetTickCount();
	lpObj->m_Rest = 0;
	lpObj->PathCur = 0;
	lpObj->Dir = lpMove->Path[0] >> 4;
	lpObj->PathCount = lpMove->Path[0] & 0x0F;

	if ( lpObj->PathCount > 15 )
	{
		LogAdd("error-L3 : Path Count error %d id:%s %s %d",
			lpObj->PathCount, lpObj->AccountID, __FILE__, __LINE__);
		return;
	}

	for ( n=0;n<15;n++)
	{
		lpObj->PathX[n]=0;
		lpObj->PathY[n]=0;
		lpObj->PathOri[n]=0;
	}

	sx = lpMove->X;
	sy = lpMove->Y;

	if ( gObjCheckXYMapTile(lpObj, 1) == TRUE )
	{
		lpObj->PathCount = 0;
		lpObj->PathCur = 0;
		lpObj->PathStartEnd = 0;
		gObjSetPosition(lpObj->m_Index, lpObj->X, lpObj->Y);
		return;
	}

	lpObj->PathX[0] = lpMove->X;
	ax = lpObj->PathX[0];
	lpObj->PathY[0] = lpMove->Y;
	ay = lpObj->PathY[0];
	lpObj->PathDir[0] = lpObj->Dir;
	lpObj->PathStartEnd = 1;

	if ( lpObj->PathCount > 0 )
	{
		lpObj->PathCur = 1;
		lpObj->PathCount++;
	}

	for (n=1;n<lpObj->PathCount;n++)
	{
		if ( (n%2) == 1 )
		{
			pathtable = lpMove->Path[(n+1) / 2] >> 4;
		}
		else
		{
			pathtable = lpMove->Path[(n+1) / 2] & 0x0F;
		}

		ax += RoadPathTable[pathtable*2];
		ay += RoadPathTable[pathtable*2+1];
		
		lpObj->PathOri[n-1] = pathtable;
		lpObj->PathDir[n]= (char)pathtable;
		lpObj->PathX[n] = ax;
		lpObj->PathY[n] = ay;
	}

	if ( lpObj->PathCount > 0 )
	{
		int nextX;
		int nextY;
		BYTE mapnumber;
		BYTE attr;
		nextX = lpObj->PathX[1];
		nextY = lpObj->PathY[1];
		mapnumber = lpObj->MapNumber;
		attr = MapC[mapnumber].GetAttr(nextX, nextY);

		if ( (((BYTE)attr & (BYTE)4) == (BYTE)4) || (((BYTE)attr & (BYTE)8) == (BYTE)8) )
		{
			for ( n=0 ; n<15 ; n++)
			{
				lpObj->PathX[n] = 0;
				lpObj->PathY[n] = 0;
				lpObj->PathOri[n] = 0;
			}

			lpObj->PathCount = 0;
			lpObj->PathCur = 0;
			lpObj->PathStartEnd = 0;
			gObjSetPosition(lpObj->m_Index, lpObj->X, lpObj->Y);
			return;
		}
	}

	lpObj->TX = (BYTE)ax;
	lpObj->TY = (BYTE)ay;

	/*if ( lpObj->Type == OBJ_USER )
	{
		int x = lpObj->X;
		int y = lpObj->Y;
		int bOk = FALSE;
		WORD Attr = 0;

		for(int i = 0; i< 256;i++)
		{
			if( x > lpObj->TX )
			{
				x--;
			}

			if( x < lpObj->TX )
			{
				x++;
			}

			if( y > lpObj->TY )
			{
				y--;
			}

			if( y < lpObj->TY )
			{
				y++;
			}

			Attr = MapC[lpObj->MapNumber].GetAttr(x,y);

			if( (Attr & 16) == 16 )
			{
				bOk = TRUE;
				break;
			}

			if( x == lpObj->TX &&
				y == lpObj->TY )
			{
				break;
			}
		}

		if( bOk == TRUE )
		{
			lpObj->m_Rest = 1;
			lpObj->PathCur = 0;
			lpObj->PathCount = 0;
			lpObj->PathStartEnd = 0;
			gObjSetPosition(lpObj->m_Index, lpObj->X, lpObj->Y);

			PMSG_ACTIONRESULT pActionResult;

			PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
			pActionResult.NumberH = SET_NUMBERH(aIndex);
			pActionResult.NumberL = SET_NUMBERL(aIndex);
			pActionResult.ActionNumber = 0x7A;
			pActionResult.Dir = lpObj->Dir;
			pActionResult.TargetNumberH = 0;
			pActionResult.TargetNumberL = 0;
			DataSend(lpObj->m_Index, (LPBYTE)&pActionResult, sizeof(pActionResult));
			return;
		}
	}*/

	if ( BC_MAP_RANGE(lpObj->MapNumber) && lpObj->Type == OBJ_USER )
	{
		int Bridge = g_BloodCastle.GetBridgeLevel(lpObj->MapNumber);
		switch ( g_BloodCastle.GetCurrentState(Bridge) )
		{
			case 0x01:
				if ( lpObj->TY > 15 )
				{
					lpObj->m_Rest = 1;
					lpObj->PathCur = 0;
					lpObj->PathCount = 0;
					lpObj->PathStartEnd = 0;
					gObjSetPosition(lpObj->m_Index, lpObj->X, lpObj->Y);

					PMSG_ACTIONRESULT pActionResult;

					PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
					pActionResult.NumberH = SET_NUMBERH(aIndex);
					pActionResult.NumberL = SET_NUMBERL(aIndex);
					pActionResult.ActionNumber = 0x7A;
					pActionResult.Dir = lpObj->Dir;
					pActionResult.TargetNumberH = 0;
					pActionResult.TargetNumberL = 0;

					DataSend(lpObj->m_Index, (LPBYTE)&pActionResult, sizeof(pActionResult));
					return;
				}
				break;
			case 0x02:
				if ( lpObj->Y > 17 && g_BloodCastle.CheckPlayStart(Bridge) == false)
				{
					gObjMoveGate(lpObj->m_Index, Bridge+0x42);
					return;
				}

				if ( lpObj->TY > 15 && g_BloodCastle.CheckPlayStart(Bridge) == false )
				{
					lpObj->m_Rest = 1;
					lpObj->PathCur = 0;
					lpObj->PathCount = 0;
					lpObj->PathStartEnd = 0;
					gObjSetPosition(lpObj->m_Index, lpObj->X, lpObj->Y);

					PMSG_ACTIONRESULT pActionResult;

					PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
					pActionResult.NumberH = SET_NUMBERH(aIndex);
					pActionResult.NumberL = SET_NUMBERL(aIndex);
					pActionResult.ActionNumber = 0x7A;
					pActionResult.Dir = lpObj->Dir;
					pActionResult.TargetNumberH = 0;
					pActionResult.TargetNumberL = 0;

					DataSend(lpObj->m_Index, (LPBYTE)&pActionResult, sizeof(pActionResult));
					return;
				}

				if ( lpObj->TY > 76 &&  g_BloodCastle.m_BridgeData[Bridge].m_bCASTLE_DOOR_LIVE )
				{
					lpObj->m_Rest = 1;
					lpObj->PathCur = 0;
					lpObj->PathCount = 0;
					lpObj->PathStartEnd = 0;
					gObjSetPosition(lpObj->m_Index, lpObj->X, lpObj->Y);

					PMSG_ACTIONRESULT pActionResult;

					PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
					pActionResult.NumberH = SET_NUMBERH(aIndex);
					pActionResult.NumberL = SET_NUMBERL(aIndex);
					pActionResult.ActionNumber = 0x7A;
					pActionResult.Dir = lpObj->Dir;
					pActionResult.TargetNumberH = 0;
					pActionResult.TargetNumberL = 0;

					DataSend(lpObj->m_Index, (LPBYTE)&pActionResult, sizeof(pActionResult));
					return;
				}
				break;
		}
	}

	
#if (__ENG__ == 1)
	PHeadSetB((LPBYTE)&pMove, 0xD4, sizeof(pMove));
#else
	// KOR PROTOCOL !!
	PHeadSetB((LPBYTE)&pMove, 0xD7, sizeof(pMove));
#endif
	pMove.NumberH = SET_NUMBERH(aIndex);
	pMove.NumberL = SET_NUMBERL(aIndex);
	pMove.X = ax;
	pMove.Y = ay;
	pMove.Path = lpObj->Dir << 4;

	if ( lpObj->Type == OBJ_USER )
	{
		if ( lpObj->m_IfState.use )
		{
			if ( lpObj->m_IfState.type == 3 )
			{
				lpObj->TargetShopNumber = -1;
				lpObj->m_IfState.type = 0;
				lpObj->m_IfState.use = 0;
			}
			else if( lpObj->m_IfState.type == 21 )
			{
				lpObj->m_IfState.type = 0;
				lpObj->m_IfState.use = 0;
			}
		}

		if ( !gObjPositionCheck(lpObj))
		{
			lpObj->PathCur = 0;
			lpObj->PathCount = 0;
			ax = lpObj->X;
			ay = lpObj->Y;
			lpObj->TX = (BYTE)ax;
			lpObj->TY = (BYTE)ay;
			pMove.X = ax;
			pMove.Y = ay;
		}

		DataSend(aIndex, (LPBYTE)&pMove, pMove.h.size);
	}

	int MVL = MAX_VIEWPORT;

	if ( lpObj->Type == OBJ_MONSTER )
		MVL = MAX_VIEWPORT_MONSTER;
		
	for (n=0;n<MVL;n++)
	{
		if ( lpObj->VpPlayer2[n].state == TRUE )
		{
			int number = lpObj->VpPlayer2[n].number;
			
			if ( number >= 0 )
			{
				if ( gObj[number].Connected > PLAYER_CONNECTED )
				{
					if ( gObj[number].Live )
					{
						if ( gObj[number].Type == OBJ_USER )
						{
							DataSend(lpObj->VpPlayer2[n].number, (LPBYTE)&pMove, pMove.h.size);
						}
					}
				}
				else
				{
					lpObj->VpPlayer2[n].number= -1;
					lpObj->VpPlayer2[n].state = 0;
					lpObj->VPCount2--;
				}
			}
		}
	}

	MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX, lpObj->m_OldY);
	MapC[lpObj->MapNumber].SetStandAttr(lpObj->TX, lpObj->TY);
	lpObj->m_OldX = lpObj->TX;
	lpObj->m_OldY = lpObj->TY;
	lpObj->X = sx;
	lpObj->Y = sy;
	lpObj->m_ViewState = 0;
}





void RecvPositionSetProc(PMSG_POSISTION_SET * lpMove, int aIndex)
{
	short n;

	if (  OBJMAX_RANGE(aIndex) == FALSE)
	{
		LogAdd("error : move protocol index error %s %d", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( ::PacketCheckTime(lpObj) == FALSE )
	{
		return;
	}

	if ( lpObj->Teleport != 0 )
	{
		return;
	}

	lpObj->X = lpMove->X;
	lpObj->Y = lpMove->Y;

	if ( CC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		if ( (GetTickCount() - lpObj->m_iChaosCastleBlowTime ) < 1000 )
		{
			return;
		}
	}

	if ( gObjCheckXYMapTile(lpObj, 2) == TRUE)
	{
		return;
	}

	PMSG_RECV_POSISTION_SET pMove;
#if (__ENG__ == 1)
	PHeadSetB((LPBYTE)&pMove, 0x15, sizeof(pMove));
#else
	// KOR PROTOCOL
	PHeadSetB((LPBYTE)&pMove, 0x10, sizeof(pMove));
#endif
	pMove.NumberH = SET_NUMBERH(aIndex);
	pMove.NumberL = SET_NUMBERL(aIndex);
	pMove.X = lpMove->X;
	pMove.Y = lpMove->Y;
	lpObj->TX = lpMove->X;
	lpObj->TY = lpMove->Y;

	if ( ::gObjPositionCheck(lpObj) == FALSE )
	{
		return;
	}

	CreateFrustrum(lpObj->X, lpObj->Y, aIndex);

	if ( lpObj->Type == OBJ_USER )
	{
		DataSend(aIndex, (UCHAR *)&pMove, pMove.h.size);
	}

	int MVL = MAX_VIEWPORT;

	if ( lpObj->Type == OBJ_MONSTER )
	{
#if (GS_CASTLE == 1)
		MVL = MAX_VIEWPORT;
#else
		MVL = MAX_VIEWPORT_MONSTER;
#endif
	}

	for ( n=0;n<MVL;n++)
	{
		if ( lpObj->VpPlayer2[n].type == OBJ_USER )
		{
			if ( lpObj->VpPlayer2[n].state == 1 )
			{
				int Index = lpObj->VpPlayer2[n].number;

				if ( gObj[Index].Connected > PLAYER_CONNECTED && gObj[Index].Live != FALSE)
				{
					DataSend(lpObj->VpPlayer2[n].number, (UCHAR *)&pMove, pMove.h.size);
				}
				else
				{
					lpObj->VpPlayer2[n].number = -1;
					lpObj->VpPlayer2[n].state = FALSE;
					lpObj->VPCount2--;
				}
			}
		}
	}

	MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX, lpObj->m_OldY);
	MapC[lpObj->MapNumber].SetStandAttr(lpObj->TX, lpObj->TY);

	lpObj->m_OldX = lpObj->TX;
	lpObj->m_OldY = lpObj->TY;

}




void CGAttack(PMSG_ATTACK* lpMsg, int aIndex)
{
	LPOBJ lpObj;
	LPOBJ lpTargetObj;
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if ( usernumber < 0 || usernumber > OBJMAX-1)
	{
		LogAdd("[CGAttack] [UserIndex Error] :%s %d %d", __FILE__, __LINE__, usernumber);
		return;
	}

	if ( !gObj[usernumber].Live )
		return;

	lpObj = &gObj[aIndex];
	int iTimeCalc = GetTickCount() - lpObj->m_LastAttackTime;

	if ( iTimeCalc < lpObj->m_DetectSpeedHackTime )
	{
		lpObj->m_DetectCount++;
		lpObj->m_SumLastAttackTime += iTimeCalc;

		if ( lpObj->m_DetectCount > gHackCheckCount && lpObj->Type == OBJ_USER )
		{
			lpObj->m_DetectedHackKickCount++;
			lpObj->m_SpeedHackPenalty = gSpeedHackPenalty;
			
			if ( gIsKickDetecHackCountLimit )
			{
				if ( lpObj->m_DetectedHackKickCount > gDetectedHackKickCount )
				{
					LogAddTD("[%s][%s] %s Kick DetecHackCountLimit Over User (%d) @%d",
						lpObj->AccountID, lpObj->Name, lMsg.Get(MSGGET(7, 108)+lpObj->Class),
						lpObj->m_DetectedHackKickCount,lpObj->MapNumber);

					CloseClient(aIndex);
					return;
				}
			}

			LogAddTD("[%s][%s] %s Attack Speed Is Wrong Normal (%d)(%d) Penalty %d @%d",
				lpObj->AccountID, lpObj->Name, lMsg.Get(MSGGET(7, 108)+lpObj->Class),
				lpObj->m_DetectSpeedHackTime, lpObj->m_SumLastAttackTime/lpObj->m_DetectCount, lpObj->m_SpeedHackPenalty,lpObj->MapNumber);
		}

		lpObj->m_LastAttackTime = GetTickCount();
	}
	else
	{
		lpObj->m_SumLastAttackTime = 0;
		lpObj->m_DetectCount = 0;
	}

	lpObj->m_LastAttackTime = GetTickCount();

	if ( bIsIgnorePacketSpeedHackDetect )
	{
		if ( lpObj->m_SpeedHackPenalty > 0 )
		{
			lpObj->m_SpeedHackPenalty--;
			
			LogAddTD("[%s][%s] %s Apply Attack Speed Penalty (%d left) @%d",
				lpObj->AccountID, lpObj->Name, lMsg.Get(MSGGET(7, 108)+lpObj->Class),
				lpObj->m_SpeedHackPenalty,lpObj->MapNumber);

			return;
		}
	}
	
	lpTargetObj = &gObj[usernumber];
	lpObj->Dir = lpMsg->DirDis;
	GCActionSend(lpObj, lpMsg->AttackAction, aIndex, usernumber);
	gComboAttack.CheckCombo(aIndex, 0);
	gObjAttack(lpObj, lpTargetObj, NULL, FALSE, FALSE, 0, FALSE,0,0);
	lpObj->UseMagicNumber = 0;
}



void GCDamageSend(int aIndex, int TargetIndex, int AttackDamage, int MSBFlag, int MSBDamage, int iShieldDamage)
{
	PMSG_ATTACKRESULT pResult;

#if (__ENG__ == 1)
	PHeadSetB((LPBYTE)&pResult, 0x11, sizeof(pResult));
#else
	// KOR PROTOCOL
	PHeadSetB((LPBYTE)&pResult, 0xDF, sizeof(pResult));
#endif
	pResult.NumberH = SET_NUMBERH(TargetIndex);
	pResult.NumberL = SET_NUMBERL(TargetIndex);

#if (FIX_HP == 1)
	pResult.Damage = AttackDamage;
	pResult.ShieldDamage = iShieldDamage;
#endif

	pResult.DamageH = SET_NUMBERH(AttackDamage);
	pResult.DamageL = SET_NUMBERL(AttackDamage);
	pResult.btShieldDamageH = SET_NUMBERH(iShieldDamage);
	pResult.btShieldDamageL = SET_NUMBERL(iShieldDamage);

	if ( MSBFlag != FALSE )
	{
		pResult.NumberH &= 0x7F;
		pResult.NumberH |= 0x80;
	}

	pResult.DamageType = (MSBDamage/0x100)&1;
	pResult.DamageType2 = MSBDamage;



	if ( gObj[TargetIndex].Type == OBJ_USER )
	{
		DataSend(TargetIndex, (LPBYTE)&pResult, pResult.h.size);
	}

//	if ( cManager.WatchTargetIndex == TargetIndex || cManager.WatchTargetIndex == aIndex )
//	{
//		cManager.DataSend((LPBYTE)&pResult, pResult.h.size);
//	}

	if ( gObj[aIndex].Type == OBJ_USER )
	{
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
	}
}



struct PMSG_DAMAGE
{
	PBMSG_HEAD h;	// C1:F3:07
	BYTE subcode;	// 3
	BYTE DamageH;	// 4
	BYTE DamageL;	// 5
	BYTE btShieldDamageH;	// 6
	BYTE btShieldDamageL;	// 7
};


void GCDamageSend(int aIndex, int damage, int iShieldDamage)
{
	if ( gObj[aIndex].Type != OBJ_USER )
	{
		return;
	}


	PMSG_DAMAGE pDamage;

	PHeadSubSetB((LPBYTE)&pDamage, 0xF3, 0x07, sizeof(pDamage));
	pDamage.DamageH = SET_NUMBERH(damage);
	pDamage.DamageL = SET_NUMBERL(damage);
	pDamage.btShieldDamageH = SET_NUMBERH(iShieldDamage);
	pDamage.btShieldDamageL = SET_NUMBERL(iShieldDamage);

	DataSend(aIndex, (LPBYTE)&pDamage, pDamage.h.size);
}

void GCDamageSend(int aIndex,int aTargetIndex,int AttackDamage,BYTE Element)
{
	PMSG_ATTACKRESULT_ELEMENTAL pMsg;
	PHeadSetB((LPBYTE)&pMsg,0xD8,sizeof(pMsg));

	pMsg.NumberH = SET_NUMBERH(aTargetIndex);
	pMsg.NumberL = SET_NUMBERL(aTargetIndex);
	pMsg.btElement = Element;
	pMsg.dwDamage = AttackDamage;

	if ( gObj[aTargetIndex].Type == OBJ_USER )
	{
		DataSend(aTargetIndex, (LPBYTE)&pMsg, pMsg.h.size);
		GCLifeSendAll(&gObj[aTargetIndex]); //temp fix
	}

//	if ( cManager.WatchTargetIndex == aTargetIndex || cManager.WatchTargetIndex == aIndex )
//	{
//		cManager.DataSend((LPBYTE)&pMsg, pMsg.h.size);
//	}

	if ( gObj[aIndex].Type == OBJ_USER )
	{
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		GCLifeSendAll(&gObj[aIndex]); //temp fix
	}
}

struct PMSG_LIFEINFO_SEND
{
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE MAXHP_HIGHH;
	BYTE MAXHP_LOWH;
	BYTE MAXHP_HIGHL;
	BYTE MAXHP_LOWL;
	BYTE HP_HIGHH;
	BYTE HP_LOWH;
	BYTE HP_HIGHL;
	BYTE HP_LOWL;
};

void GCLifeSendAll(LPOBJ lpObj)
{
	PMSG_LIFEINFO_SEND pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEC,0x10,sizeof(pMsg));

	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);

	int MaxLife = (int)(lpObj->MaxLife + lpObj->AddLife);
	int Life = (int)lpObj->Life;

	pMsg.MAXHP_HIGHH = (MaxLife>>16)>>8;
	pMsg.MAXHP_HIGHL = (MaxLife>>16)&0xFF;
	pMsg.MAXHP_LOWH = (MaxLife&0xFFFF)>>8;
	pMsg.MAXHP_LOWL = (MaxLife&0xFFFF)&0xFF;

	pMsg.HP_HIGHH = (Life>>16)>>8;
	pMsg.HP_HIGHL = (Life>>16)&0xFF;
	pMsg.HP_LOWH = (Life&0xFFFF)>>8;
	pMsg.HP_LOWL = (Life&0xFFFF)&0xFF;

	MsgSendV2(lpObj,(LPBYTE)&pMsg,pMsg.h.size);
}



struct PMSG_KILLPLAYER
{
	PBMSG_HEAD h;	// C1:16
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE ExpH;	// 5
	BYTE ExpL;	// 6
	BYTE DamageH;	// 7
	BYTE DamageL;	// 8
};



void GCKillPlayerExpSend(int aIndex, int TargetIndex, int exp, int AttackDamage, BOOL MSBFlag)
{
	/*PMSG_KILLPLAYER pkillMsg;

	PHeadSetBE((LPBYTE)&pkillMsg, 0x16, sizeof(pkillMsg));
	pkillMsg.NumberH = SET_NUMBERH(TargetIndex);
	pkillMsg.NumberL = SET_NUMBERL(TargetIndex);
	pkillMsg.ExpH = SET_NUMBERH(exp);
	pkillMsg.ExpL = SET_NUMBERL(exp);
	pkillMsg.DamageH = SET_NUMBERH(AttackDamage);
	pkillMsg.DamageL = SET_NUMBERL(AttackDamage);

	if ( MSBFlag != FALSE )
	{
		pkillMsg.NumberH &= 0x7F;
		pkillMsg.NumberH |= 0x80;
	}

	if (  BC_MAP_RANGE(gObj[aIndex].MapNumber) != FALSE )
	{
		g_BloodCastle.AddExperience(aIndex, exp);
	}

	DataSend(aIndex, (UCHAR*)&pkillMsg, pkillMsg.h.size);*/

	PMSG_KILLPLAYER_EXT pMsg;
	PHeadSetBE((LPBYTE)&pMsg,0x9C,sizeof(pMsg));

	pMsg.NumberH = SET_NUMBERH(TargetIndex);
	pMsg.NumberL = SET_NUMBERL(TargetIndex);
	pMsg.ExpH = (DWORD)exp >> 16;
	pMsg.ExpL = (DWORD)exp & 0xFFFF;

#if (CUSTOM_DAMAGE==1)
	pMsg.DamageHW = SET_NUMBERHW(AttackDamage);
	pMsg.DamageLW = SET_NUMBERLW(AttackDamage);
#else
	pMsg.DamageH = SET_NUMBERH(AttackDamage);
	pMsg.DamageL = SET_NUMBERL(AttackDamage);
#endif

	if ( MSBFlag != FALSE )
	{
		pMsg.NumberH &= 0x7F;
		pMsg.NumberH |= 0x80;
	}

	if (  BC_MAP_RANGE(gObj[aIndex].MapNumber) != FALSE )
	{
		g_BloodCastle.AddExperience(aIndex, exp);
	}

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);

}




struct PMSG_DIEPLAYER
{
	PBMSG_HEAD h;	// C1:17
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Skill;	// 5
	BYTE KillerNumberH;	// 6
	BYTE KillerNumberL;	// 7
};



void GCDiePlayerSend(LPOBJ lpObj, int TargetIndex, WORD skill, int KillerIndex)
{
	PMSG_DIEPLAYER pDieMsg;

	PHeadSetB((LPBYTE)&pDieMsg, 0x17, sizeof(pDieMsg));
	pDieMsg.NumberH = SET_NUMBERH(TargetIndex);
	pDieMsg.NumberL = SET_NUMBERL(TargetIndex);
	pDieMsg.Skill = (BYTE)skill;
	pDieMsg.KillerNumberH = SET_NUMBERH(KillerIndex);
	pDieMsg.KillerNumberL = SET_NUMBERL(KillerIndex);
	int iSize = pDieMsg.h.size;

	MsgSendV2(lpObj, (LPBYTE)&pDieMsg, iSize);

	if ( lpObj->Type == OBJ_USER )
	{
		DataSend(lpObj->m_Index, (LPBYTE)&pDieMsg, iSize);
	}
}




void GCActionSend(LPOBJ lpObj, BYTE ActionNumber, int aIndex, int aTargetIndex)
{
	PMSG_ACTIONRESULT pActionResult;

	PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
	pActionResult.NumberH = SET_NUMBERH(aIndex);
	pActionResult.NumberL = SET_NUMBERL(aIndex);
	pActionResult.ActionNumber = ActionNumber;
	pActionResult.Dir = lpObj->Dir;
	pActionResult.TargetNumberH = SET_NUMBERH(aTargetIndex);
	pActionResult.TargetNumberL = SET_NUMBERL(aTargetIndex);

	MsgSendV2(lpObj, (LPBYTE)&pActionResult, pActionResult.h.size);
}




void CGActionRecv(PMSG_ACTION * lpMsg, int aIndex)
{
	if ( !OBJMAX_RANGE(aIndex))
	{
		LogAdd("error : move protocol index error %s %d", __FILE__, __LINE__);
		return;
	}

	PMSG_ACTIONRESULT pResult;
	LPOBJ lpObj = &gObj[aIndex];

	PHeadSetB((LPBYTE)&pResult, 0x18, sizeof(pResult));
	pResult.ActionNumber = lpMsg->ActionNumber;
	pResult.NumberH = SET_NUMBERH(aIndex);
	pResult.NumberL = SET_NUMBERL(aIndex);
	lpObj->m_ActionNumber = lpMsg->ActionNumber;
	lpObj->Dir = lpMsg->Dir;
	pResult.Dir = lpObj->Dir;
	pResult.TargetNumberH = lpMsg->iTargetIndexH;
	pResult.TargetNumberL = lpMsg->iTargetIndexL;

	if ( lpObj->m_ActionNumber == 0x80)
	{
		lpObj->m_ViewState = 2;
		lpObj->m_Rest = lpObj->m_ActionNumber;
	}
	else if ( lpObj->m_ActionNumber == 0x81 )
	{
		lpObj->m_ViewState = 3;
		lpObj->m_Rest = lpObj->m_ActionNumber;
	}
	else if ( lpObj->m_ActionNumber == 0x82 )
	{
		lpObj->m_ViewState = 4;
		lpObj->m_Rest = lpObj->m_ActionNumber;
	}
	
	int MVL = MAX_VIEWPORT;

	if ( lpObj->Type == OBJ_MONSTER )
		MVL = MAX_VIEWPORT_MONSTER;

	for (int n=0;n<MVL;n++)
	{
		if ( lpObj->VpPlayer2[n].type == OBJ_USER )
		{
			if ( lpObj->VpPlayer2[n].state != 0 && lpObj->VpPlayer2[n].state != 0x10 && lpObj->VpPlayer2[n].state != 0x08)
			{
				DataSend(lpObj->VpPlayer2[n].number, (LPBYTE)&pResult, pResult.h.size);
			}
		}
	}
}




void CGMagicAttack(PMSG_MAGICATTACK* lpMsg, int aIndex)
{
	LPOBJ lpObj;
	LPOBJ lpTargetObj;
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CMagicInf * lpMagic;
	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH,lpMsg->MagicNumberL);

	if ( usernumber < 0 || usernumber > OBJMAX-1 )
	{
		LogAdd("error :%s %d %d", __FILE__, __LINE__, usernumber);
		return;
	}

	if ( gObj[usernumber].Live == 0 || gObj[usernumber].Connected  < PLAYER_LOGGED )
		return;

	if ( gObj[aIndex].CloseCount >= 0 )
		return;

	BYTE MapAttr = MapC[gObj[aIndex].MapNumber].GetAttr(gObj[aIndex].X,gObj[aIndex].Y);

	if ( (MapAttr&1)==TRUE && MagicNumber == 58 )
	{
		GCMagicAttackNumberSend(&gObj[aIndex],40,aIndex,TRUE);
		gObjSetPosition(aIndex,gObj[aIndex].X,gObj[aIndex].Y);
		return;
	}

	lpObj = &gObj[aIndex];
	lpTargetObj = &gObj[usernumber];
	
	int attackret = gObjCheckAttackArea(aIndex, usernumber);

	if ( attackret )
	{
		LogAddTD("[%s][%s] Try Attack In Not Attack Area [Protocol] (%s:%d,%d) errortype = %d",
			lpObj->AccountID, lpObj->Name, lMsg.Get(MSGGET(7, 208)+lpObj->MapNumber), lpObj->X, lpObj->Y,
			attackret);

		if ( lpObj->Class == 275 )	// Kundun
			lpObj->TargetNumber = -1;

		if ( bIsIgnorePacketSpeedHackDetect )
			return;
	}

	if ( lpObj->Type == OBJ_USER )
	{
		lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);

#if( FIX_MAXSTAT == 1 )
		WORD Energy = lpObj->Energy+ lpObj->AddEnergy;
		WORD Strength = lpObj->Strength + lpObj->AddStrength;
		WORD Dexterity = lpObj->Dexterity + lpObj->AddDexterity;
#else
		short Energy = lpObj->Energy+ lpObj->AddEnergy;
		short Strength = lpObj->Strength + lpObj->AddStrength;
		short Dexterity = lpObj->Dexterity + lpObj->AddDexterity;
#endif

		switch( MagicNumber )
		{
		case 0x33:
			if( Dexterity < 646 )
			{
				return;
			}
			break;
		}

		if ( !lpMagic )
			return;

		if ( MagicDamageC.SkillGetRequireClass(lpObj->Class, lpObj->ChangeUP,lpObj->ChangeUP2, lpMagic->m_Skill) < 1 )
			return;

#if (ENABLETEST_NEWPVP == 1)
		if (lpMagic->m_Skill == 63 && (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpTargetObj))) {
			return;
		}
#endif

		if ( MagicDamageC.CheckStatus(lpMagic->m_Skill, lpObj->GuildStatus) == 0 )
		{
			LogAddC(2, "[0x19] CGMagicAttack() - Invalid Status");
			return;
		}

		if ( MagicDamageC.CheckKillCount(lpMagic->m_Skill, lpObj->m_btKillCount) < 0 )
		{
			LogAddC(2, "[0x19] CGMagicAttack() - Invalid KillCount");
			return;
		}
	}
	else
	{
		lpMagic = gObjGetMagic(lpObj, MagicNumber);

		if ( !lpMagic )
			return;
	}

	if ( lpMagic->m_Skill != AT_SKILL_CROSSBOW && 
		 lpMagic->m_Skill != AT_MSKILL_FE_TRIPLESHOT1 &&
		 lpMagic->m_Skill != AT_MSKILL_FE_TRIPLESHOT2 )
	{
		if( gObjUseSkill.SpeedHackCheck(aIndex) == FALSE )
		{
			return;
		}
	}

	if ( !gCheckSkillDistance(aIndex, usernumber, lpMagic->m_Skill))
	{
		DWORD dwTick = GetTickCount() - lpObj->m_dwSkillDistanceErrorTick;
		
		if ( dwTick > (g_iSkillDiatanceKickCheckTime*1000) )
		{
			lpObj->m_iSkillDistanceErrorCount = 0;
			lpObj->m_dwSkillDistanceErrorTick = GetTickCount();
		}

		lpObj->m_iSkillDistanceErrorCount++;

		if ( lpObj->m_iSkillDistanceErrorCount > g_iSkillDistanceKickCount )
		{
			if ( g_iSkillDistanceKick )
			{
				LogAddTD("[SKILL DISTANCE CHECK] [%s][%s] Kick Invalid Skill Area User. count(%d)",
					lpObj->AccountID, lpObj->Name, lpObj->m_iSkillDistanceErrorCount);

				CloseClient(lpObj->m_Index);
				return;
			}

			lpObj->m_iSkillDistanceErrorCount=0;
		}

		return;
	}
	
	gObjUseSkill.UseSkill(aIndex, usernumber, lpMagic);
}




void GCMagicAttackNumberSend(LPOBJ lpObj, WORD MagicNumber, int usernumber,  unsigned char skillsuccess)
{
	PMSG_MAGICATTACK_RESULT pAttack;
	
	PHeadSetBE((LPBYTE)&pAttack, 0x19, sizeof(pAttack));

	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);

	pAttack.SourceNumberH = SET_NUMBERH(lpObj->m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(lpObj->m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(usernumber);
	pAttack.TargetNumberL = SET_NUMBERL(usernumber);
	pAttack.TargetNumberH &= 0x7F;

	if ( skillsuccess )
		pAttack.TargetNumberH |= 0x80;

	if ( lpObj->Type == OBJ_USER )
		DataSend(lpObj->m_Index, (LPBYTE)&pAttack, pAttack.h.size);

	if ( CC_MAP_RANGE(lpObj->MapNumber) )
	{
		switch ( MagicNumber )
		{
			case 16:
			case 26:
			case 27:
			case 28:
			case 48:
				// MSKILL
			case AT_MSKILL_DW_MANASHIELD1:
			case AT_MSKILL_DW_MANASHIELD2:
			case AT_MSKILL_FE_HEAL1:
			case AT_MSKILL_FE_DEFENSEBUFF1:
			case AT_MSKILL_FE_DEFENSEBUFF2:
			case AT_MSKILL_FE_CURE:
			case AT_MSKILL_FE_BLESS:
			case AT_MSKILL_FE_BLESS1:
			case AT_MSKILL_FE_ATTACKBUFF1:
			case AT_MSKILL_FE_ATTACKBUFF2:
			case AT_MSKILL_DK_SWELLLIFE1:
			case AT_MSKILL_DK_SWELLLIFE2:
			case AT_MSKILL_DK_SWELLLIFE3:
				return;
		}
	}
	
	MsgSendV2(lpObj, (LPBYTE)&pAttack, pAttack.h.size);
}





void GCMagicCancelSend(LPOBJ lpObj,int MagicNumber)
{
	PMSG_MAGICCANCEL pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x1B, sizeof(pMsg));

	pMsg.MagicNumberH = SET_NUMBERH(MagicNumber);
	pMsg.MagicNumberL = SET_NUMBERL(MagicNumber);

	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);

	if ( lpObj->Type == OBJ_USER )
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
	
	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}

void GCSendEffect(LPOBJ lpObj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime)
{
	PMSG_SENDEFFECT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x2D, sizeof(pMsg));
	pMsg.byEffectOption = iEffectUseOption;
	pMsg.wOptionType = iOptionType;
	pMsg.wEffectType = iEffectType;
	pMsg.iLeftTime = iLeftTime;
	pMsg.iEffect = iEffectType;

	if ( lpObj->Type == OBJ_USER )
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}


void CGMagicCancel(PMSG_MAGICCANCEL * lpMsg, int aIndex)
{
	if ( !OBJMAX_RANGE(aIndex))
		return;

	LPOBJ lpObj = &gObj[aIndex];

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH,lpMsg->MagicNumberL);

	if ( MagicNumber == AT_SKILL_INFINITY_ARROW )
	{
		if ( gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_INFINITY_ARROW);
		}
	}
	else if( MagicNumber == AT_SKILL_EXPWIZARDRY )
	{
		if ( gObjCheckUsedBuffEffect(lpObj,BUFF_EXP_WIZARDRY) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_EXP_WIZARDRY);
		}
	}
	else if( MagicNumber == AT_MSKILL_DW_EXPWIZARDRY1 )
	{
		if ( gObjCheckUsedBuffEffect(lpObj,BUFF_EXP_WIZARDRY2) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_EXP_WIZARDRY2);
		}
	}
	else if( MagicNumber == AT_MSKILL_DW_EXPWIZARDRY2 )
	{
		if ( gObjCheckUsedBuffEffect(lpObj,BUFF_EXP_WIZARDRY3) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_EXP_WIZARDRY3);
		}
	}
	else if( MagicNumber == AT_MSKILL_FE_INFINITY1 )
	{
		if( gObjCheckUsedBuffEffect(lpObj,BUFF_INFINITY_ARROW2) == TRUE )
		{
			gObjRemoveBuffEffect(lpObj,BUFF_INFINITY_ARROW2);
		}
	}
}


struct PMSG_USE_MONSTERSKILL
{
	PBMSG_HEAD h;	// C1:69
	BYTE btMonsterSkillNumberH;
	BYTE btMonsterSkillNumberL;	// 3
	WORD wObjIndex;	// 4
	WORD wTargetObjIndex;	// 6
};


void GCUseMonsterSkillSend(LPOBJ lpObj, LPOBJ lpTargetObj, int iSkillNumber)
{	
	PMSG_USE_MONSTERSKILL pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x69, sizeof(pMsg));

	pMsg.btMonsterSkillNumberH = SET_NUMBERH(iSkillNumber);
	pMsg.btMonsterSkillNumberL = SET_NUMBERL(iSkillNumber);


	pMsg.wObjIndex = lpObj->m_Index;
	pMsg.wTargetObjIndex = lpTargetObj->m_Index;

	if ( lpObj->Type == OBJ_USER )
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}
	

struct PMSG_VIEWSKILLSTATE
{
	PBMSG_HEAD h;	// C1:07
	BYTE State;	// 3
	BYTE NumberH;	// 8
	BYTE NumberL;	// 9
	BYTE Effect;
};


void GCStateInfoSend(LPOBJ lpObj, BYTE state, BYTE Effect)
{
	PMSG_VIEWSKILLSTATE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x07, sizeof(pMsg));
	pMsg.State = state;
	pMsg.Effect = Effect;
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);

	if ( lpObj->Type == OBJ_USER )
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

	if ( CC_MAP_RANGE(lpObj->MapNumber) != FALSE )
		pMsg.Effect = 0;

	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}



struct PMSG_TELEPORT_RESULT
{
	PBMSG_HEAD h;	// C3:1C
	WORD MoveNumber;
	BYTE MapNumber;
	BYTE MapX;
	BYTE MapY;
	BYTE Dir;
};



void CGTeleportRecv(PMSG_TELEPORT* lpMsg, int aIndex)
{
	if ( !OBJMAX_RANGE(aIndex))
		return;


	LogAdd("Attempting to move, teleportRecv, map: %d");

	if ( gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_STUN) == TRUE ||
		 gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_SLEEP) == TRUE ||
		 gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_EARTHBINDS) == TRUE ||
		 gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_FREEZE) == TRUE )
	{
		PMSG_TELEPORT_RESULT pTeleportResult;
		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.MoveNumber = 0;
		pTeleportResult.MapNumber = gObj[aIndex].MapNumber;
		pTeleportResult.MapX = gObj[aIndex].X;
		pTeleportResult.MapY = gObj[aIndex].Y;
		pTeleportResult.Dir = gObj[aIndex].Dir;
		DataSend(aIndex, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
		return;
	}

	if ( IT_MAP_RANGE( gObj[aIndex].MapNumber ) &&
		 g_IllusionTemple.GetState(gObj[aIndex].MapNumber) == 2 )
	{
		if(  g_IllusionTemple.TeleportLock(aIndex) )
		{
			PMSG_TELEPORT_RESULT pTeleportResult;
			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.MoveNumber = 0;
			pTeleportResult.MapNumber = gObj[aIndex].MapNumber;
			pTeleportResult.MapX = gObj[aIndex].X;
			pTeleportResult.MapY = gObj[aIndex].Y;
			pTeleportResult.Dir = gObj[aIndex].Dir;
			DataSend(aIndex, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
			return;
		}

		if( g_IllusionTemple.Is2ndSkillActive(gObj[aIndex].iTempleIndex,gObj[aIndex].MapNumber) )
		{
			PMSG_TELEPORT_RESULT pTeleportResult;
			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.MoveNumber = 0;
			pTeleportResult.MapNumber = gObj[aIndex].MapNumber;
			pTeleportResult.MapX = gObj[aIndex].X;
			pTeleportResult.MapY = gObj[aIndex].Y;
			pTeleportResult.Dir = gObj[aIndex].Dir;
			DataSend(aIndex, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if( IT_MAP_RANGE( gObj[aIndex].MapNumber ) )
	{
		if( g_IllusionTemple.GetState(gObj[aIndex].MapNumber) == 1 )
		{
			PMSG_TELEPORT_RESULT pTeleportResult;
			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.MoveNumber = 0;
			pTeleportResult.MapNumber = gObj[aIndex].MapNumber;
			pTeleportResult.MapX = gObj[aIndex].X;
			pTeleportResult.MapY = gObj[aIndex].Y;
			pTeleportResult.Dir = gObj[aIndex].Dir;
			DataSend(aIndex, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if ( lpMsg->MoveNumber == 0 )
	{
		CMagicInf * lpMagic;
		BYTE x;
		BYTE y;
		PMSG_MAGICATTACK_RESULT pAttack;

		PHeadSetBE((LPBYTE)&pAttack, 0x19, sizeof(pAttack));

		pAttack.MagicNumberH = SET_NUMBERH(AT_SKILL_TELEPORT);
		pAttack.MagicNumberL = SET_NUMBERL(AT_SKILL_TELEPORT);

		pAttack.SourceNumberH = SET_NUMBERH(aIndex);
		pAttack.SourceNumberL = SET_NUMBERL(aIndex);
		pAttack.TargetNumberH = SET_NUMBERH(aIndex);
		pAttack.TargetNumberL = SET_NUMBERL(aIndex);

		lpMagic = gObjGetMagicSearch(&gObj[aIndex], AT_SKILL_TELEPORT);
		x = lpMsg->MapX;
		y = lpMsg->MapY;

		if ( gObjCheckTeleportArea(aIndex, x, y) == FALSE )
		{
			LogAddC(2, "[%s][%s] Try Teleport Not Move Area [%d,%d]",
				gObj[aIndex].AccountID, gObj[aIndex].Name,
				x, y);

			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.MoveNumber = 0;
			pTeleportResult.MapNumber = gObj[aIndex].MapNumber;
			pTeleportResult.MapX = gObj[aIndex].X;
			pTeleportResult.MapY = gObj[aIndex].Y;
			pTeleportResult.Dir = gObj[aIndex].Dir;

			DataSend(aIndex, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
#if (GS_CASTLE == 1)

		if ( gObj[aIndex].MapNumber == MAP_INDEX_CASTLESIEGE )
		{
			if( g_CastleSiege.CheckTeleportMagicAxisY(gObj[aIndex].Y,x,y) == FALSE )
			{
				y = gObj[aIndex].Y;
			}
		}
#endif

		if ( lpMagic )
		{
			int usemana = gObjMagicManaUse(&gObj[aIndex], lpMagic);

			if ( usemana >= 0 )
			{
				int usebp = gObjMagicBPUse(&gObj[aIndex], lpMagic);
				
				if ( usebp >= 0 )
				{
					if ( gObj[aIndex].Type == OBJ_USER )
						DataSend(aIndex, (LPBYTE)&pAttack, pAttack.h.size);

					MsgSendV2(&gObj[aIndex], (LPBYTE)&pAttack, pAttack.h.size);
					gObjTeleportMagicUse(aIndex, x, y);
					gObj[aIndex].Mana = (float)usemana;
					gObj[aIndex].BP = usebp;
					GCManaSend(gObj[aIndex].m_Index, (WORD)gObj[aIndex].Mana, -1, 0, gObj[aIndex].BP);
				}
			}
		}
	}
	else if ( IF_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		int nZoneIndex = g_ImperialGuardian.GetCurrentZoneIndex(aIndex);

		if( nZoneIndex >= 0 )
		{
			g_ImperialGuardian.CGEnterPortal(aIndex,nZoneIndex+1);
		}
	}
	else if ( gGateC.IsInGate(aIndex, lpMsg->MoveNumber) )
	{
		LogAdd("Attempting to move ELSE IF, teleportRecv, map: %d", lpMsg->MoveNumber);
		gObjMoveGate(aIndex, lpMsg->MoveNumber);
	}
	else
	{
		gObjClearViewport(&gObj[aIndex]);
		GCTeleportSend(&gObj[aIndex], lpMsg->MoveNumber, 
			gObj[aIndex].MapNumber, gObj[aIndex].X, gObj[aIndex].Y, gObj[aIndex].Dir);

#if (GS_CASTLE == 1)
		if( gObj[aIndex].MapNumber == MAP_INDEX_CASTLESIEGE 
			&& g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			g_CastleSiege.NotifySelfCsJoinSide(aIndex);
			g_CastleSiege.NotifyCsSelfLeftTime(aIndex);
		}
#endif
	}
}




void CGTargetTeleportRecv(PMSG_TARGET_TELEPORT * lpMsg, int aIndex)
{
	if ( !OBJMAX_RANGE(aIndex))
		return;

	LogAdd("Attempting to move TargetTeleportRecv");
	if( gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_STUN) == TRUE ||
		gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_SLEEP) == TRUE ||
		gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_EARTHBINDS) == TRUE ||
		gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_FREEZE) == TRUE)
		return;

	CMagicInf * lpMagic;
	BYTE x;
	BYTE y;
	PMSG_MAGICATTACK_RESULT pAttack;

	PHeadSetBE((LPBYTE)&pAttack, 0x19, sizeof(pAttack));

	pAttack.MagicNumberH = SET_NUMBERH(AT_SKILL_TARGET_TELEPORT);
	pAttack.MagicNumberL = SET_NUMBERL(AT_SKILL_TARGET_TELEPORT);

	pAttack.SourceNumberH = SET_NUMBERH(aIndex);
	pAttack.SourceNumberL = SET_NUMBERL(aIndex);
	int iTargetIndex = MAKE_NUMBERW(lpMsg->NumberL, lpMsg->NumberH);

	pAttack.TargetNumberH = SET_NUMBERH(iTargetIndex);
	pAttack.TargetNumberL = SET_NUMBERL(iTargetIndex);
	lpMagic = gObjGetMagicSearch(&gObj[aIndex], AT_SKILL_TARGET_TELEPORT);
	x = lpMsg->MapX;
	y = lpMsg->MapY;

	if ( !gObjIsConnectedGP(iTargetIndex) )
	{
		LogAddC(2, "[%s][%s] Try Target Teleport Not Move Area [%d,%d]",
			gObj[aIndex].AccountID, gObj[aIndex].Name, x, y);

		return;
	}

	if ( IT_MAP_RANGE( gObj[iTargetIndex].MapNumber ) )
	{
		if( g_IllusionTemple.TeleportLock(gObj[iTargetIndex].m_Index) )
		{
			return;
		}
	}

	if ( gObj[aIndex].PartyNumber != gObj[iTargetIndex].PartyNumber ||
		 gObj[aIndex].PartyNumber == -1 ||
		 gObj[iTargetIndex].PartyNumber == -1)
	{
		return;
	}

	if ( !gObjCheckTeleportArea(iTargetIndex, x, y))
	{
		LogAddC(2, "[%s][%s] Try Target Teleport Not Move Area [%d,%d]",
			gObj[aIndex].AccountID, gObj[aIndex].Name, x, y);

		return;
	}

	if ( lpMagic )
	{
		int usemana = gObjMagicManaUse(&gObj[aIndex], lpMagic);

		if ( usemana >= 0 )
		{
			int usebp = gObjMagicBPUse(&gObj[aIndex], lpMagic);
			
			if ( usebp >= 0 )
			{
				if ( gObj[aIndex].Type == OBJ_USER )
					DataSend(aIndex, (LPBYTE)&pAttack, pAttack.h.size);

				MsgSendV2(&gObj[aIndex], (LPBYTE)&pAttack, pAttack.h.size);
				gObjTeleportMagicUse(iTargetIndex, x, y);
				gObj[aIndex].Mana = (float)usemana;
				gObj[aIndex].BP = usebp;
				GCManaSend(gObj[aIndex].m_Index, (WORD)gObj[aIndex].Mana, -1, 0, gObj[aIndex].BP);

				return;
			}
		}
	}
}






void GCTeleportSend(LPOBJ lpObj, int MoveNumber, BYTE MapNumber, BYTE MapX, BYTE MapY, BYTE Dir)
{
	PMSG_TELEPORT_RESULT pMsg;

	if ( lpObj->Type != OBJ_USER )
		return;

	LogAdd("[Teleport send] Attempting to TP, map number: %d", MapNumber);

	if (lpObj->m_bPShopOpen)
		LogAdd("shop is open!");

	pMsg.h.c = 0xC3;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x1C;
	pMsg.MoveNumber = MoveNumber;
	pMsg.MapNumber = MapNumber;
	pMsg.MapX = MapX;
	pMsg.MapY = MapY;
	pMsg.Dir = Dir;

	DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

/*	if ( MoveNumber == 0 )
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
	else
		DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size); */
}





void CGBeattackRecv(unsigned char* lpRecv, int aIndex, int magic_send)
{
	// Set the lpRecv to a Predeterminated Packet
	PMSG_BEATTACK_COUNT * lpCount = (PMSG_BEATTACK_COUNT *)lpRecv;

#if (__ENG__ ==1)
	if ( lpCount->h.headcode != 0xDB )
#else
	// KOR PROTOCOL
	// Check the Protocol
	if ( lpCount->h.headcode != 0xD3 )
#endif
	{
		LogAdd("error-L3 %s %d", __FILE__, __LINE__);
		return;
	}

	// Check if the count is leess than 1
	if ( lpCount->Count < 1 )
	{
		LogAdd("error-L3 %s %d", __FILE__, __LINE__);
		return;
	}

	// Set the Max limitation to 5
	if ( lpCount->Count > 5 )
	{
		lpCount->Count = 5;
	}

	int lOfs = sizeof(PMSG_BEATTACK_COUNT);
	CMagicInf * lpMagic;

	// Set the Start of the Offset
	int tNumber;
	PMSG_BEATTACK * lpMsg;

	if ( MAKE_NUMBERW(lpCount->MagicNumberH,lpCount->MagicNumberL) == NULL )
		return;

	// Set lpMagic according
	// if aIndex is OBJ_USER
	// or if it is OBJ_MONSTER
	if ( gObj[aIndex].Type == OBJ_USER )
	{
		BYTE MapAttr = MapC[gObj[aIndex].MapNumber].GetAttr(gObj[aIndex].X,gObj[aIndex].Y);

		if( (MapAttr&1) == TRUE )
		{
			gObjSetPosition(aIndex,gObj[aIndex].X,gObj[aIndex].Y);
			return;
		}
		WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH,lpCount->MagicNumberL);

		lpMagic = gObjGetMagicSearch(&gObj[aIndex], MagicNumber);
	}
	else
	{
		WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH,lpCount->MagicNumberL);
		lpMagic = gObjGetMagic(&gObj[aIndex], MagicNumber);
	}

	// Check if there is Magic
	if ( lpMagic == NULL )
	{
		LogAdd("error-L3 %s %d", __FILE__, __LINE__);
		return;
	}

	// Avoid use Skill of the following types
	if ( lpMagic->m_Skill == AT_SKILL_BLOWOFFURY ||
		 lpMagic->m_Skill == AT_SKILL_STRIKE ||
		 g_MasterSkillSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_DK_DEATHSTAB1 ||
		 g_MasterSkillSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_DK_RAGEFULBLOW1 ||
		 lpMagic->m_Skill == AT_MSKILL_DK_RAGEFULBLOW2 ||
		 lpMagic->m_Skill == AT_MSKILL_DK_TWISTINGSLASH2 ||
		 lpMagic->m_Skill == AT_SKILL_WHEEL ||
		 lpMagic->m_Skill == AT_MSKILL_DK_TWISTINGSLASH1 )	
	{
		return;
	}

	if ( gObj[aIndex].Type == OBJ_USER )
	{
		// Check if Player can use Magic
		// Acording to its Class
		if ( MagicDamageC.SkillGetRequireClass(gObj[aIndex].Class,
												gObj[aIndex].ChangeUP,
												gObj[aIndex].ChangeUP2,
												lpMagic->m_Skill) < 1 )
		{
			// It's not his magic [%s][%s] .
			LogAddC(2, lMsg.Get(MSGGET(1, 239)),
				gObj[aIndex].AccountID,
				gObj[aIndex].Name,
				lpMagic->m_Skill);	// #error Deathway change on WTFFile

			gObjUserKill(aIndex);
			return;
		}

		// Check the time cast Spell
		// No longer that 8 seconds
		if ( (GetTickCount()- gObj[aIndex].UseMagicTime) > 8000 )
		{
			LogAddC(2, "Too long time passed after casting magic. [%s][%s] (%d)(%d)",
				gObj[aIndex].AccountID,
				gObj[aIndex].Name,
				lpMagic->m_Skill,
				GetTickCount() - gObj[aIndex].UseMagicTime);

			return;
		}

		// Check if user cast to much Spells
		// Npo longer thant 4
		if ( gObj[aIndex].UseMagicCount > 4 )
		{
			return;
		}

		gObj[aIndex].UseMagicCount++;
	}

	if (	lpMagic->m_Skill != AT_SKILL_STORM
		  && lpMagic->m_Skill != AT_SKILL_EVIL
		  && lpMagic->m_Skill != AT_SKILL_HELL
		  && lpMagic->m_Skill != AT_SKILL_BLAST
		  && lpMagic->m_Skill != AT_SKILL_INFERNO
		  && lpMagic->m_Skill != AT_SKILL_FLAME
		  && lpMagic->m_Skill != AT_SKILL_CROSSBOW
		  && lpMagic->m_Skill != AT_SKILL_DEVILFIRE
		  && lpMagic->m_Skill != AT_SKILL_FLASH
		  && lpMagic->m_Skill != AT_SKILL_WHEEL
		  && lpMagic->m_Skill != AT_SKILL_KNIGHTSPEAR
		  && lpMagic->m_Skill != AT_SKILL_STRIKE
		  && lpMagic->m_Skill != AT_SKILL_BLOWOFFURY
		  && lpMagic->m_Skill != AT_SKILL_PENETRATION
		  && lpMagic->m_Skill != AT_SKILL_DEFENSEDOWN
		  && lpMagic->m_Skill != AT_SKILL_FIRESCREAM 
		  // MSKILL
		  && lpMagic->m_Skill != AT_MSKILL_DW_FLAME1
		  && lpMagic->m_Skill != AT_MSKILL_DW_BLAST1
		  && lpMagic->m_Skill != AT_MSKILL_DW_INFERNO1
		  && lpMagic->m_Skill != AT_MSKILL_DW_HELLFIRE1
		  && lpMagic->m_Skill != AT_MSKILL_DW_EVILSPIRIT1
		  && lpMagic->m_Skill != AT_MSKILL_FE_TRIPLESHOT1
		  && lpMagic->m_Skill != AT_MSKILL_FE_TRIPLESHOT2
		  && lpMagic->m_Skill != AT_MSKILL_FE_PENETRATION1
		  && lpMagic->m_Skill != AT_MSKILL_DK_TWISTINGSLASH1
		  && lpMagic->m_Skill != AT_MSKILL_DK_TWISTINGSLASH2
		  && lpMagic->m_Skill != AT_MSKILL_MG_TWISTINGSLASH1
		  && lpMagic->m_Skill != AT_MSKILL_DK_RAGEFULBLOW1
		  && lpMagic->m_Skill != AT_MSKILL_DK_RAGEFULBLOW2
		  && lpMagic->m_Skill != AT_MSKILL_MG_FLAME1
		  && lpMagic->m_Skill != AT_MSKILL_MG_BLAST1
		  && lpMagic->m_Skill != AT_MSKILL_MG_INFERNO1
		  && lpMagic->m_Skill != AT_MSKILL_MG_EVILSPIRIT1
		  && lpMagic->m_Skill != AT_MSKILL_MG_FIRESLASH1
		  && lpMagic->m_Skill != AT_MSKILL_MG_FIRESLASH2
		  && lpMagic->m_Skill != AT_MSKILL_DL_FIRESCREAM1 
		  && lpMagic->m_Skill != AT_MSKILL_DL_FIRESCREAM2 )
	{
		//LogAdd("error-L3 Skill:[%d] %s %d ", lpMagic->m_Skill, __FILE__, __LINE__);
		return;
	}

	if ( lpMagic->m_Skill == AT_SKILL_CROSSBOW || lpMagic->m_Skill == AT_SKILL_FIRESCREAM ||
		 lpMagic->m_Skill == AT_MSKILL_FE_TRIPLESHOT1 || lpMagic->m_Skill == AT_MSKILL_FE_TRIPLESHOT2 ||
		 lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM1 || lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM2 )
	{
		int NSAttackSerial = lpCount->Serial;

		if ( gObj[aIndex].OSAttackSerial >= 255 && (gObj[aIndex].OSAttackSerial - lpCount->Serial) > 50 )
		{
			gObj[aIndex].OSAttackSerial = -1;
		}
	
		if ( NSAttackSerial > gObj[aIndex].OSAttackSerial )
		{
			gObj[aIndex].OSAttackSerial = NSAttackSerial;
			LPOBJ lpObj = &gObj[aIndex];
			int iTimeCalc = GetTickCount() - lpObj->m_LastAttackTime;

			if ( iTimeCalc < lpObj->m_DetectSpeedHackTime && iTimeCalc != 0)
			{
				lpObj->m_DetectCount++;
				lpObj->m_SumLastAttackTime += iTimeCalc;

				if ( lpObj->m_DetectCount > gHackCheckCount )
				{
					lpObj->m_DetectedHackKickCount++;

					if ( gIsKickDetecHackCountLimit )
					{
						if ( lpObj->m_DetectedHackKickCount > gDetectedHackKickCount )
						{
							LogAddTD("[%s][%s] %s Kick DetecHackCountLimit Over User (%d)",
								lpObj->AccountID, lpObj->Name, lMsg.Get(MSGGET(7, 108)+lpObj->Class),
								lpObj->m_DetectedHackKickCount);

							CloseClient(aIndex);
							return;
						}
					}

					LogAddTD("[%s][%s] %s Attack Speed Is Wrong Magic3 (%d)(%d) Penalty %d",
						lpObj->AccountID, lpObj->Name, lMsg.Get(MSGGET(7, 108)+lpObj->Class),
						lpObj->m_DetectSpeedHackTime,lpObj->m_SumLastAttackTime/lpObj->m_DetectCount,
						lpObj->m_SpeedHackPenalty);
				}

				lpObj->m_LastAttackTime = GetTickCount();
			}
			else
			{
				lpObj->m_SumLastAttackTime = 0;
				lpObj->m_DetectCount = 0;
			}

			lpObj->m_LastAttackTime = GetTickCount();
		}
	}

	if ( lpMagic->m_Skill == AT_SKILL_EVIL ||
		 lpMagic->m_Skill == AT_MSKILL_DW_EVILSPIRIT1 ||
		 lpMagic->m_Skill == AT_MSKILL_MG_EVILSPIRIT1 )
	{
		if( gObj[aIndex].Class != CLASS_WIZARD &&
			gObj[aIndex].Class != CLASS_MAGUMSA )
		{
			return;
		}
	}


	if ( bIsIgnorePacketSpeedHackDetect )
	{
		LPOBJ lpObj = &gObj[aIndex];

		if ( lpObj->m_SpeedHackPenalty > 0 )
		{
			lpObj->m_SpeedHackPenalty--;
			
			LogAddTD("[%s][%s] %s Apply Attack Speed Penalty (%d left)",
				lpObj->AccountID, lpObj->Name, lMsg.Get(MSGGET(7, 108)+lpObj->Class),
				lpObj->m_SpeedHackPenalty);

			return;
		}
	}
	
	int lOfs2 = lOfs;
	int pTargetNumber[128];

	int i;
	int n;
	
	for (i=0;i<lpCount->Count;i++)
	{
		lpMsg = (PMSG_BEATTACK *)&lpRecv[lOfs2];
		pTargetNumber[i] = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
		lOfs2 += sizeof(PMSG_BEATTACK);
	}

	for (n=0;n<lpCount->Count;n++)
	{
		lpMsg = (PMSG_BEATTACK *)&lpRecv[lOfs];
		tNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
		LPOBJ lpObj = &gObj[aIndex];

		if ( lpMsg->MagicKey == FALSE && 
			(lpMagic->m_Skill == AT_SKILL_EVIL ||
			 lpMagic->m_Skill == AT_MSKILL_MG_EVILSPIRIT1 ||
			 lpMagic->m_Skill == AT_MSKILL_DW_EVILSPIRIT1 ) )
		{
			return;
		}

		if ( lpMsg->MagicKey  && 
			(lpMagic->m_Skill == AT_SKILL_EVIL ||
			 lpMagic->m_Skill == AT_MSKILL_DW_EVILSPIRIT1 ||
			 lpMagic->m_Skill == AT_MSKILL_MG_EVILSPIRIT1 ) )
		{
			if ( lpObj->DurMagicKeyChecker.IsValidDurationTime(lpMsg->MagicKey) == FALSE )
			{
				LogAddC(0, "¡Ú¡Ú¡Ú¡Ú InValid DurationTime Key = %d ( Time : %d) [%s][%s]",
					lpMsg->MagicKey, lpObj->DurMagicKeyChecker.GetValidDurationTime(lpMsg->MagicKey),
					lpObj->AccountID, lpObj->Name); 
				lOfs += sizeof(PMSG_BEATTACK);

				continue;
			}
			
			if ( lpObj->DurMagicKeyChecker.IsValidCount(lpMsg->MagicKey) == FALSE )
			{
				LogAddC(0, "¡Ú¡Ú¡Ú¡Ú InValid VailidCount = %d ( Count : %d) [%s][%s]",
					lpMsg->MagicKey, lpObj->DurMagicKeyChecker.GetValidCount(lpMsg->MagicKey),
					lpObj->AccountID, lpObj->Name); 
				lOfs += sizeof(PMSG_BEATTACK);

				continue;
			}

			if ( OBJMAX_RANGE( tNumber ) == FALSE)
			{
				continue;
			}

			int Dis = gObjCalDistance(lpObj,&gObj[tNumber]);

			if( Dis > 13 )
			{
				lOfs+= sizeof(PMSG_BEATTACK);
				continue;
			}
		}
		
		/*if ( gEnableCheckPenetrationSkill )
		{
			if ( lpMagic->m_Skill == AT_SKILL_FIRESCREAM ||
				 lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM1 ||
				 lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM2 )
			{
				if ( gMultiAttackHackCheck[gObj[aIndex].m_Index].CheckFireScreamSkill(tNumber, lpMagic->m_Skill, lpCount->Serial) == FALSE )
				{
					return;
				}
			}
			else if ( gMultiAttackHackCheck[gObj[aIndex].m_Index].CheckPenetrationSkill(tNumber, lpMagic->m_Skill, lpCount->Serial)== FALSE )
			{
				return;
			}
		}*/

		for (i=0;i<lpCount->Count;i++)
		{
			if ( n != i)
			{
				if ( pTargetNumber[i] == tNumber )
				{
					LogAddTD("[%s][%s] %s Detect Hack : Multi Attack",
						gObj[aIndex].AccountID, gObj[aIndex].Name,
						lMsg.Get(MSGGET(7, 108)+gObj[aIndex].Class));

					CloseClient(gObj[aIndex].m_Index);
					return;
				}
			}
		}

		if ( tNumber >= 0 && tNumber < OBJMAX-1 )
		{
			if ( lpMagic->m_Skill == AT_SKILL_DEFENSEDOWN )
			{
				if( gObj[aIndex].Type == OBJ_USER )
				{
					if( (gObj[aIndex].Strength + gObj[aIndex].AddStrength) >= 596 )
					{
						gObjUseSkill.MaGumSkillDefenseDown(aIndex, tNumber, lpMagic);
						gObjAttack(&gObj[aIndex], &gObj[tNumber], lpMagic, TRUE, 1, 0, FALSE,0,0);
					}
				}
				else
				{
					gObjUseSkill.MaGumSkillDefenseDown(aIndex, tNumber, lpMagic);
					gObjAttack(&gObj[aIndex], &gObj[tNumber], lpMagic, TRUE, 1, 0, FALSE,0,0);
				}
			}
			else if ( lpMagic->m_Skill == AT_SKILL_FIRESCREAM )
			{
				gObjUseSkill.SkillFireScream(aIndex, tNumber, lpMagic);
			}
			else if ( g_MasterSkillSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_MG_FIRESLASH1 )
			{
				g_MasterSkillSystem.MLS_MaGumSkillDefenseDown(aIndex,tNumber);
				gObjAttack(&gObj[aIndex], &gObj[tNumber], lpMagic, TRUE, 1, 0, FALSE,0,0);
			}
			else if ( g_MasterSkillSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_MG_FIRESLASH2 )
			{
				g_MasterSkillSystem.MLS_MaGumSkillDefenseDownMastery(aIndex,tNumber,lpMagic);
				gObjAttack(&gObj[aIndex], &gObj[tNumber], lpMagic, TRUE, 1, 0, FALSE,0,0);
			}
			else if ( g_MasterSkillSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_DL_FIRESCREAM1 )
			{
				g_MasterSkillSystem.MLS_FireScream(aIndex, tNumber, lpMagic);
			}
			else if ( magic_send )
			{
				gObjAttack(&gObj[aIndex], &gObj[tNumber], lpMagic, TRUE, 1, 0, FALSE,0,0);
			}
			else
			{
				gObjAttack(&gObj[aIndex], &gObj[tNumber], lpMagic, FALSE, 1, 0, FALSE,0,0);
			}
		}

		lOfs += sizeof(PMSG_BEATTACK);
	}
}






void CGDurationMagicRecv(PMSG_DURATION_MAGIC_RECV* lpMsg, int aIndex)
{
	CMagicInf * lpMagic;
	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->Teleport )
	{
		return;
	}

	int MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH,lpMsg->MagicNumberL);

	if ( gObj[aIndex].Type == OBJ_USER )
	{
		lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);
	}
	else
	{
		lpMagic = gObjGetMagic( lpObj, MagicNumber);
	}

	if ( lpMagic == NULL )
	{
		return;
	}

	if ( lpObj->Type == OBJ_USER )
	{
		if ( MagicDamageC.CheckStatus(lpMagic->m_Skill, lpObj->GuildStatus) == FALSE )
		{
			LogAddC(2, "[0x1E] CGDurationMagicRecv() - Invalid Status");	// #warning maybe protocol here change
			return;
		}

		if ( MagicDamageC.CheckKillCount(lpMagic->m_Skill, lpObj->m_btKillCount) < 0 )
		{
			LogAddC(2, "[0x1E] CGDurationMagicRecv() - Invalid KillCount");
			return;
		}
	}

	lpObj->UseMagicNumber = 0;

	if ( lpMagic->m_Skill != 24 || lpMagic->m_Skill != 0xEB)
	{
		if ( !gObjUseSkill.SpeedHackCheck(aIndex))
		{
			return;
		}
	}

	if ( !gObjUseSkill.EnableSkill(lpMagic->m_Skill) )
	{
		return;
	}

	if ( !gObjUseSkill.CheckWeapon(lpObj,lpMagic->m_Skill) )
	{
		return;
	}

	int aTargetIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	gObjUseSkill.UseSkill(aIndex, lpMagic,lpMsg->X, lpMsg->Y, lpMsg->Dir, lpMsg->TargetPos, aTargetIndex);

	if ( lpMsg->MagicKey )
		lpObj->DurMagicKeyChecker.SetDurationTime(lpMsg->MagicKey, GetTickCount());
}


struct PMSG_USERMAPJOIN
{
	PBMSG_HEAD h;	// C1:F3:03
	BYTE subcode;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE X;	// 6
	BYTE Y;	// 7
	BYTE MapNumber;	// 8
	BYTE Dir;	// 9
};



void ObjectMapJoinPositionSend(short aIndex)	// Not Used
{
	PMSG_USERMAPJOIN pObject;
	
	PHeadSubSetB((LPBYTE)&pObject, 0xF3, 0x03, sizeof(pObject));
	pObject.NumberH = SET_NUMBERH(aIndex);
	pObject.NumberL = SET_NUMBERL(aIndex);
	pObject.X = gObj[aIndex].X;
	pObject.Y = gObj[aIndex].Y;
	pObject.MapNumber = gObj[aIndex].MapNumber;
	pObject.Dir = gObj[aIndex].Dir;

	DataSend(aIndex, (LPBYTE)&pObject, pObject.h.size);
}


struct PMSG_ANS_EXTEND
{
	PBMSG_HEAD h;
	BYTE btResult;
};

void CGUseItemRecv(PMSG_USEITEM* lpMsg, int aIndex)
{
	int pos;
	CItem * citem;
	int iItemUseType = lpMsg->btItemUseType;


	// Check User States
	if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type != 3 )
	{
		GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, TRUE, gObj[aIndex].iShield);

		LogAdd("[%s][%s] CGUseItemRecv()_If return %s %d",
			gObj[aIndex].AccountID,
			gObj[aIndex].Name,
			__FILE__, __LINE__);

		return;
	}

	// Check if User is exiting
	if ( gObj[aIndex].CloseType != -1 )
	{
		GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, TRUE, gObj[aIndex].iShield);

		LogAdd("[%s][%s] CGUseItemRecv()_CloseType return %s %d",
			gObj[aIndex].AccountID, 
			gObj[aIndex].Name,
			__FILE__, __LINE__);

		return;
	}

	// Fix Inventory Pointer
	if ( gObjFixInventoryPointer(aIndex) == false )
	{
		LogAdd("[Fix Inv.Ptr] False Location - %s, %d",
			__FILE__, __LINE__);
		//return;
	}

	// Check if there is a Transaction
	// Dupe
	if ( gObj[aIndex].pTransaction == 1 )
	{
		LogAddTD("[%s][%s] CGUseItemRecv() Failed : Transaction == 1, IF_TYPE : %d",
			gObj[aIndex].AccountID,
			gObj[aIndex].Name,
			gObj[aIndex].m_IfState.type);

		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if (g_NewPVP.IsObserver(gObj[aIndex])) {
		 GCServerMsgStringSend(lMsg.Get(3428), aIndex, 1);
		 return;
	}
#endif

	// Set Var Pos
	pos = lpMsg->inventoryPos;

	// Check if the pos is in bounds
	if ( pos > MAIN_INVENTORY_SIZE-1 )
	{
		// error : Location is over the range. %s/%d
		LogAdd(lMsg.Get(MSGGET(1, 241)), __FILE__, __LINE__);

		GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);

		LogAdd("[%s][%s] CGUseItemRecv()_Inventory return %s %d",
			gObj[aIndex].AccountID,
			gObj[aIndex].Name,
			__FILE__, __LINE__);

		return;
	}


	// check if the item
	// have the same position
	// as the new position
	if ( lpMsg->inventoryPos == lpMsg->invenrotyTarget )
	{
		GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);

		LogAdd("error-L1 : [%s][%s] CGUseItemRecv()_Pos return %s %d",
			gObj[aIndex].AccountID,
			gObj[aIndex].Name,
			__FILE__, __LINE__);

		return;
	}

	// Set Pointer
	citem = &gObj[aIndex].pInventory[pos];


	// The item is ITEM
	if ( citem->IsItem() )
	{
		// Check if item have 0 as Serial
		if ( gObjCheckSerial0ItemList(citem) )
		{
			MsgOutput(aIndex, lMsg.Get(MSGGET(13, 26)));

			GCReFillSend(aIndex,
					(WORD)gObj[aIndex].Life,
					0xFD,
					1,
					gObj[aIndex].iShield);


			LogAddTD("[ANTI-HACK][Serial 0 Item] [UseItem] (%s)(%s) Item(%s) Pos(%d)",
				gObj[aIndex].AccountID,
				gObj[aIndex].Name,
				citem->GetName(),
				pos);

			return;
		}

		// Check the item Serial
		if ( gObjInventorySearchSerialNumber(&gObj[aIndex],
												citem->GetNumber()) == FALSE )
		{
			GCReFillSend(aIndex,
					(WORD)gObj[aIndex].Life,
					0xFD,
					1,
					gObj[aIndex].iShield);
		
			
			LogAdd("error-L2 : CopyItem [%s][%s] return %s %d",
				gObj[aIndex].AccountID,
				gObj[aIndex].Name,
				__FILE__, __LINE__);

			return;
		}

		if ( citem->m_serial && !gObjCanItemTouch(&gObj[aIndex], 1) )
		{
			GCReFillSend(aIndex, (WORD)
				gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			return;
		}

		if ( citem->m_Type == ITEMGET(14,0) || // Apple
			 citem->m_Type == ITEMGET(14,1) ||
			 citem->m_Type == ITEMGET(14,2) ||
			 citem->m_Type == ITEMGET(14,3) )
		{
			int tLife = (citem->m_Value*10) - (gObj[aIndex].Level*2);	// #formula

			if ( tLife <  0 )
			{
				tLife = 0;
			}

			int nAddRate=0;

			switch ( citem->m_Type )
			{
				case ITEMGET(14,0):	// Apple
					nAddRate = 10;
					break;
				case ITEMGET(14,1):	// 
					nAddRate = 20;
					break;
				case ITEMGET(14,2):	// 
					nAddRate = 30;
					break;
				case ITEMGET(14,3):	// 
					nAddRate = 40;
					break;
			}

			if ( citem->m_Level == 1 )	// #formula
			{
				nAddRate += 5;
			}

			tLife += ((int)gObj[aIndex].MaxLife * nAddRate) / 100;	// #formula

			if ( citem->m_Type == ITEMGET(14,0) )
			{
				if ( citem->m_Level < 2 )
				{
					gObj[aIndex].FillLife += tLife;
					tLife = 0;
				}
			}

			if ( gObj[aIndex].FillLife > 0.0f )
			{
				gObj[aIndex].Life += gObj[aIndex].FillLife;

				if ( gObj[aIndex].Life > (gObj[aIndex].MaxLife +gObj[aIndex].AddLife) )
				{
					gObj[aIndex].Life = gObj[aIndex].MaxLife + gObj[aIndex].AddLife;
					gObj[aIndex].FillLife = 0;
				}

				GCReFillSend(gObj[aIndex].m_Index,(WORD)gObj[aIndex].Life, 0xFF, FALSE, gObj[aIndex].iShield);
			}

			gObj[aIndex].FillLifeMax = (float)tLife;
			gObj[aIndex].FillLife = (float)tLife;

			if ( citem->m_Type == ITEMGET(14,0) && citem->m_Level < 2 )
			{
				gObj[aIndex].FillLifeCount = 0;
			}
			else if ( citem->m_Level == 1 )
			{
				gObj[aIndex].FillLifeCount = 2;
			}
			else
			{
				gObj[aIndex].FillLifeCount = 3;
			}

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14,4) ||	// Small Mana Potion
				 citem->m_Type == ITEMGET(14,5) ||
				 citem->m_Type == ITEMGET(14,6)  )
		{
			int tMana = (citem->m_Value*10) - (gObj[aIndex].Level);	// #formula

			if ( tMana < 0 )
			{
				tMana=0;
			}

			switch ( citem->m_Type )
			{
				case ITEMGET(14,4):	// Small Mana Potion
					tMana += ((int)(gObj[aIndex].MaxMana + gObj[aIndex].AddMana))*20/100; 
					break;
				case ITEMGET(14,5):	// Mana Potion
					tMana += ((int)(gObj[aIndex].MaxMana + gObj[aIndex].AddMana))*30/100; 
					break;
				case ITEMGET(14,6):	// Large Mana Potion
					tMana += ((int)(gObj[aIndex].MaxMana + gObj[aIndex].AddMana))*40/100; 
					break;
			}

			gObj[aIndex].Mana += tMana;

			if ( gObj[aIndex].Mana > (gObj[aIndex].MaxMana+gObj[aIndex].AddMana-1.0f) )
				gObj[aIndex].Mana = gObj[aIndex].MaxMana+gObj[aIndex].AddMana;
			
			GCManaSend(aIndex, (WORD)gObj[aIndex].Mana, 0xFF, 0, gObj[aIndex].BP);

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,38) )	// Small Compound Potion 
		{
			int iShieldGage = (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield) * 5/100;	// #formula
			int iHPGage = (int)((gObj[aIndex].MaxLife + gObj[aIndex].AddLife) * 10.0f / 100.0f );	// #formula

			if ( gObj[aIndex].iShield < 0 )
				gObj[aIndex].iShield = 0;

			if ( gObj[aIndex].FillLife > 0.0f )
			{
				gObj[aIndex].Life += gObj[aIndex].FillLife;

				if ( gObj[aIndex].Life > (gObj[aIndex].MaxLife + gObj[aIndex].AddLife ) )
					gObj[aIndex].Life = gObj[aIndex].MaxLife + gObj[aIndex].AddLife;
			}

			if ( gObj[aIndex].iFillShield > 0 )
			{
				gObj[aIndex].iShield += gObj[aIndex].iFillShield;

				if (  gObj[aIndex].iShield > (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield )  )
					gObj[aIndex].iShield = gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield;
			}

			gObj[aIndex].FillLife = (float)iHPGage;
			gObj[aIndex].FillLifeMax = (float)iHPGage;
			gObj[aIndex].FillLifeCount = 4;
			gObj[aIndex].iFillShield = iShieldGage;
			gObj[aIndex].iFillShieldMax = iShieldGage;
			gObj[aIndex].iFillShieldCount = 4;

			GCSendEffectInfo(aIndex, 3);

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			LogAddTD("[%s][%s]Use Compound Potion Lv1 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				gObj[aIndex].AccountID, gObj[aIndex].Name, 
				gObj[aIndex].iShield, gObj[aIndex].Life,
				gObj[aIndex].iShield+iShieldGage, (double)(gObj[aIndex].Life+(float)iHPGage));
		}
		else if ( citem->m_Type == ITEMGET(14,39) )	// Compound Potion 
		{
			int iShieldGage = (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield) * 10/100;	// #formula
			int iHPGage = (int)((gObj[aIndex].MaxLife + gObj[aIndex].AddLife) * 25.0f / 100.0f );	// #formula

			if ( gObj[aIndex].iShield < 0 )
				gObj[aIndex].iShield = 0;

			if ( gObj[aIndex].FillLife > 0.0f )
			{
				gObj[aIndex].Life += gObj[aIndex].FillLife;

				if ( gObj[aIndex].Life > (gObj[aIndex].MaxLife + gObj[aIndex].AddLife ) )
					gObj[aIndex].Life = gObj[aIndex].MaxLife + gObj[aIndex].AddLife;
			}

			if ( gObj[aIndex].iFillShield > 0 )
			{
				gObj[aIndex].iShield += gObj[aIndex].iFillShield;

				if (  gObj[aIndex].iShield > (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield )  )
					gObj[aIndex].iShield = gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield;
			}

			gObj[aIndex].FillLife = (float)iHPGage;
			gObj[aIndex].FillLifeMax = (float)iHPGage;
			gObj[aIndex].FillLifeCount = 4;
			gObj[aIndex].iFillShield = iShieldGage;
			gObj[aIndex].iFillShieldMax = iShieldGage;
			gObj[aIndex].iFillShieldCount = 4;

			GCSendEffectInfo(aIndex, 3);

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			LogAddTD("[%s][%s]Use Compound Potion Lv2 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				gObj[aIndex].AccountID, gObj[aIndex].Name, 
				gObj[aIndex].iShield, gObj[aIndex].Life,
				gObj[aIndex].iShield+iShieldGage, (double)(gObj[aIndex].Life+(float)iHPGage));
		}
		else if ( citem->m_Type == ITEMGET(14,40) )	// Large Compound Potion 
		{
			int iShieldGage = (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield) * 20/100;	// #formula
			int iHPGage = (int)((gObj[aIndex].MaxLife + gObj[aIndex].AddLife) * 45.0f / 100.0f );	// #formula

			if ( gObj[aIndex].iShield < 0 )
				gObj[aIndex].iShield = 0;

			if ( gObj[aIndex].FillLife > 0.0f )
			{
				gObj[aIndex].Life += gObj[aIndex].FillLife;

				if ( gObj[aIndex].Life > (gObj[aIndex].MaxLife + gObj[aIndex].AddLife ) )
					gObj[aIndex].Life = gObj[aIndex].MaxLife + gObj[aIndex].AddLife;
			}

			if ( gObj[aIndex].iFillShield > 0 )
			{
				gObj[aIndex].iShield += gObj[aIndex].iFillShield;

				if (  gObj[aIndex].iShield > (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield )  )
					gObj[aIndex].iShield = gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield;
			}

			gObj[aIndex].FillLife = (float)iHPGage;
			gObj[aIndex].FillLifeMax = (float)iHPGage;
			gObj[aIndex].FillLifeCount = 4;
			gObj[aIndex].iFillShield = iShieldGage;
			gObj[aIndex].iFillShieldMax = iShieldGage;
			gObj[aIndex].iFillShieldCount = 4;

			GCSendEffectInfo(aIndex, 3);

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			LogAddTD("[%s][%s]Use Compound Potion Lv3 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				gObj[aIndex].AccountID, gObj[aIndex].Name, 
				gObj[aIndex].iShield, gObj[aIndex].Life,
				gObj[aIndex].iShield+iShieldGage, (double)(gObj[aIndex].Life+(float)iHPGage));
		}
		else if ( citem->m_Type == ITEMGET(14,35) )
		{
			int iShieldGage = (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield) * 25 / 100;	// #formula

			if ( gObj[aIndex].iShield < 0 )
				gObj[aIndex].iShield = 0;

			if ( gObj[aIndex].iFillShield > 0 )
			{
				gObj[aIndex].iShield += gObj[aIndex].iFillShield;

				if ( gObj[aIndex].iShield > (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield ) )
					gObj[aIndex].iShield = gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield;
			}

			gObj[aIndex].iFillShieldCount = 2;
			gObj[aIndex].iFillShield = iShieldGage;
			gObj[aIndex].iFillShieldMax = iShieldGage;

			GCSendEffectInfo(aIndex, 3);

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,36) )
		{
			int iShieldGage = (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield) * 35 / 100;	// #formula

			if ( gObj[aIndex].iShield < 0 )
				gObj[aIndex].iShield = 0;

			if ( gObj[aIndex].iFillShield > 0 )
			{
				gObj[aIndex].iShield += gObj[aIndex].iFillShield;

				if ( gObj[aIndex].iShield > (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield ) )
					gObj[aIndex].iShield = gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield;
			}

			gObj[aIndex].iFillShieldCount = 2;
			gObj[aIndex].iFillShield = iShieldGage;
			gObj[aIndex].iFillShieldMax = iShieldGage;

			GCSendEffectInfo(aIndex, 3);

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,37) )
		{
			int iShieldGage = (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield) * 45 / 100; // #formula

			if ( gObj[aIndex].iShield < 0 )
				gObj[aIndex].iShield = 0;

			if ( gObj[aIndex].iFillShield > 0 )
			{
				gObj[aIndex].iShield += gObj[aIndex].iFillShield;

				if ( gObj[aIndex].iShield > (gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield ) )
					gObj[aIndex].iShield = gObj[aIndex].iMaxShield + gObj[aIndex].iAddShield;
			}

			gObj[aIndex].iFillShieldCount = 2;
			gObj[aIndex].iFillShield = iShieldGage;
			gObj[aIndex].iFillShieldMax = iShieldGage;

			GCSendEffectInfo(aIndex, 3);

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,8) ) // Antidote
		{
			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			
			if ( gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_POISON) == TRUE )
			{
				gObjRemoveBuffEffect(&gObj[aIndex],BUFF_POISON);
				gObj[aIndex].lpAttackObj = NULL;
				GCMagicCancelSend(&gObj[aIndex],1);
			}
			else if ( gObjCheckUsedBuffEffect(&gObj[aIndex],BUFF_DEFENSEDOWN) == TRUE )
			{
				gObjRemoveBuffEffect(&gObj[aIndex],BUFF_DEFENSEDOWN);
				gObj[aIndex].DelayActionTime = 0;
				gObj[aIndex].DelayLevel = 0;
				gObj[aIndex].lpAttackObj = NULL;
				GCMagicCancelSend(&gObj[aIndex], 7);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,46) )
		{
			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&gObj[aIndex],BUFF_JACK_BLESSING);
		}
		else if ( citem->m_Type == ITEMGET(14,47) )
		{
			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&gObj[aIndex],BUFF_JACK_WRATH);
		}
		else if ( citem->m_Type == ITEMGET(14,48) )
		{
			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&gObj[aIndex],BUFF_JACK_CRY);
		}
		else if ( citem->m_Type == ITEMGET(14,49) )
		{
			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&gObj[aIndex],BUFF_JACK_FOOD);
		}
		else if ( citem->m_Type == ITEMGET(14,50) )
		{
			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&gObj[aIndex],BUFF_JACK_DRINK);
		}
		else if ( citem->m_Type == ITEMGET(14,63) || citem->m_Type == ITEMGET(14,64) )
		{
			switch( citem->m_Type)
			{
			case ITEMGET(14,63):
				gObj[aIndex].Life = gObj[aIndex].MaxLife + gObj[aIndex].AddLife;
				GCReFillSend(aIndex,(WORD)gObj[aIndex].Life,0xFF,0,gObj[aIndex].iShield);
				break;
			case ITEMGET(14,64):
				gObj[aIndex].Mana = gObj[aIndex].MaxMana + gObj[aIndex].AddMana;
				GCManaSend(aIndex,(WORD)gObj[aIndex].Mana,0xFF,0,gObj[aIndex].BP);
				break;
			}

			if(gObjSearchItemMinus(&gObj[aIndex],pos,1) == FALSE)
			{
				gObjInventoryItemSet(aIndex,pos,0xFF);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex,pos,1);
			}
		}
		else if ( g_kJewelOfHarmonySystem.IsJewelOfHarmonySmeltingItems(citem->m_Type) == TRUE )
		{
			if ( g_kJewelOfHarmonySystem.SmeltItemBySmeltingStone(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if ( citem->m_Type == ITEMGET(12,30) ) // Pack of Bless
		{
			if ( gObjItemLevelUpPickAxe(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
#if( ENABLETEST_ITEMUPGRADEJEWELS == 1 )
		else if( g_ItemUpgradeJewels.IsJewel(citem->m_Type) )
		{
			g_ItemUpgradeJewels.ProcInsert(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget);
		}
#else
		else if ( g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(citem->m_Type) == TRUE )
		{
			if ( g_kJewelOfHarmonySystem.StrengthenItemByJewelOfHarmony(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,13) ) // Jewel Of Bless
		{
			if ( gObjItemLevelUp(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,14) ) // Jewel Of Soul
		{
			if ( gObjItemRandomLevelUp(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,16) ) // Jewel Of Life
		{
			if ( gObjItemRandomOption3Up(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
#endif
		else if ( citem->m_Type >= ITEMGET(15,0) ||
				  citem->m_Type == ITEMGET(12,7) ||
				 (citem->m_Type >= ITEMGET(12,8) && citem->m_Type <= ITEMGET(12,24) ) ||	// Orbs
				  citem->m_Type == ITEMGET(12,35) ||
				  citem->m_Type == ITEMGET(12,44) ||
				  citem->m_Type == ITEMGET(12,45) ||
				  citem->m_Type == ITEMGET(12,46) ||
				  citem->m_Type == ITEMGET(12,47) ||
				  citem->m_Type == ITEMGET(12,48) )
		{
			if ( (gObj[aIndex].Strength + gObj[aIndex].AddStrength) < citem->m_RequireStrength )
			{
				GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
				return;
			}
			
			if ( (gObj[aIndex].Dexterity + gObj[aIndex].AddDexterity) < citem->m_RequireDexterity )
			{
				GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
				return;
			}

			if ( citem->m_Type == ITEMGET(15,18) )	// Scroll of HellBurst
			{
				if ( g_QuestInfo.GetQuestState(&gObj[aIndex], 2) != 2 )
				{
					GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
					return;
				}
			}

			if (citem->m_Type >= ITEMGET(12,8) && citem->m_Type <= ITEMGET(12,24))
			{
				if ( gObj[aIndex].Level < citem->m_RequireLevel )	// Orbs
				{
					GCReFillSend(aIndex, (WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
					return;
				}
			}

			WORD skillnumber;

			if ( citem->IsClass((char)gObj[aIndex].Class, gObj[aIndex].ChangeUP,gObj[aIndex].ChangeUP2) == FALSE )
			{	
				GCInventoryItemDeleteSend(aIndex, -1, 1);
			}
			else
			{
				int addskill = gObjMagicAdd(&gObj[aIndex], citem->m_Type>>(9), citem->m_Type %  MAX_SUBTYPE_ITEMS,citem->m_Level, skillnumber);

				if ( addskill >= 0 )
				{
					gObjInventoryItemSet(aIndex, pos, -1);
					GCMagicListOneSend(aIndex, addskill, skillnumber, citem->m_Level, 0, 0);
					gObj[aIndex].pInventory[pos].Clear();
					GCInventoryItemDeleteSend(aIndex, pos, 1);
				}
				else
				{
					GCInventoryItemDeleteSend(aIndex, -1, 1);
				}
			}
		}
		else if ( citem->m_Type == ITEMGET(14,10) ) // Town Portal Scroll
		{
			LogAddTD("[Using Item] [Return Scroll] [%s][%s] - Current Map:[%d]",
				gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].MapNumber);

			if ( BC_MAP_RANGE(gObj[aIndex].MapNumber) )
			{
				int iBridge = g_BloodCastle.GetBridgeLevel(gObj[aIndex].MapNumber);
				if ( g_BloodCastle.GetCurrentState(iBridge) == 2 )
				{
					g_BloodCastle.SearchUserDropQuestItem(aIndex);
				}
				else
				{
					g_BloodCastle.SearchUserDeleteQuestItem(aIndex);
				}
			}

			if ( IT_MAP_RANGE(gObj[aIndex].MapNumber) )
			{
				g_IllusionTemple.DropRelic(gObj[aIndex].MapNumber,aIndex);
			}

			if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type == 3 )
			{
				gObj[aIndex].TargetShopNumber = -1;
				gObj[aIndex].m_IfState.type = 0;
				gObj[aIndex].m_IfState.use = 0;
			}

			gObjInventoryItemSet(aIndex, pos, -1);
			gObj[aIndex].pInventory[pos].Clear();
			GCInventoryItemDeleteSend(aIndex, pos, 1);

			if ( gObj[aIndex].MapNumber == MAP_INDEX_DEVIAS )
			{
				gObjMoveGate(aIndex, 22);
			}
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_NORIA )
			{
				gObjMoveGate(aIndex, 27);
			}
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_LOSTTOWER )
			{
				gObjMoveGate(aIndex, 42);
			}
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_ATHLANSE )
			{
				gObjMoveGate(aIndex, 49);
			}
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_TARKAN )
			{
				gObjMoveGate(aIndex, 57);
			}
			else if ( BC_MAP_RANGE(gObj[aIndex].MapNumber) != FALSE )
			{
				gObjMoveGate(aIndex, 22);
			}
			else if ( CC_MAP_RANGE(gObj[aIndex].MapNumber) != FALSE )
			{
				gObjMoveGate(aIndex, 22);
			}
			else if ( KALIMA_MAP_RANGE(gObj[aIndex].MapNumber) != FALSE )
			{
				gObjMoveGate(aIndex, 22);
			}
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_AIDA )
			{
				gObjMoveGate(aIndex, 27);
			}
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
			{
				gObjMoveGate(aIndex, 27);
			}
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_ELBELAND )
			{
				gObjMoveGate(aIndex,267);
			}
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_SWAMPOFCALMNESS )
			{
				gObjMoveGate(aIndex,273);
			}
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_RAKLION )
			{
				gObjMoveGate(aIndex,286);
			}
#if (ENABLETEST_NEWPVP == 1)
			else if (g_NewPVP.IsVulcanusMap(gObj[aIndex].MapNumber))
			{
				gObjMoveGate(aIndex, 294);
			}
			else if (g_NewPVP.IsPKFieldMap(gObj[aIndex].MapNumber))
			{
				g_NewPVP.Reset(gObj[aIndex]);
				gObjMoveGate(aIndex, 17);
			}
#endif
#if (GS_CASTLE == 1)
			else if ( gObj[aIndex].MapNumber == MAP_INDEX_CASTLESIEGE )
			{
				if ( g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					if( gObj[aIndex].m_btCsJoinSide == 1 )
					{
						gObjMoveGate(aIndex,101);
					}
					else
					{
						gObjMoveGate(aIndex,100);
					}
				}
				else
				{
					if( g_CastleSiege.CheckCastleOwnerMember(aIndex) == TRUE || g_CastleSiege.CheckCastleOwnerUnionMember(aIndex) == TRUE)
					{
						gObjMoveGate(aIndex,101);
					}
					else
					{
						gObjMoveGate(aIndex,100);
					}
				}
			}
#endif
			else
			{
				gObjMoveGate(aIndex, 17);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,9) ) // Ale
		{
			int level = citem->m_Level;

			gObjInventoryItemSet(aIndex, pos, -1);
			gObj[aIndex].pInventory[pos].Clear();
			GCInventoryItemDeleteSend(aIndex, pos, 1);
			gObjUseDrink(&gObj[aIndex], level);
		}
		else if ( citem->m_Type == ITEMGET(14,20) ) // Remedy of Love
		{
			if( citem->m_Level == 0 )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
				gObjUseDrink(&gObj[aIndex], 2);
			}
		}
		else if ( citem->m_Type == ITEMGET(13,15) ) // Fruits
		{
			if ( iItemUseType == 0 )
			{
				gObjUsePlusStatFruit(aIndex, pos);
			}
			else if ( iItemUseType == 1 )
			{
				gObjUseMinusStatFruit(aIndex, pos);
			}
		}
		else if ( citem->m_Type == ITEMGET(13,54) ||
				  citem->m_Type == ITEMGET(13,55) ||
				  citem->m_Type == ITEMGET(13,56) ||
				  citem->m_Type == ITEMGET(13,57) ||
				  citem->m_Type == ITEMGET(13,58) )
		{
			ExMinusStatFruit(&gObj[aIndex],pos);
		}
		else if( citem->m_Type == ITEMGET(14,78) ||
				 citem->m_Type == ITEMGET(14,79) ||
				 citem->m_Type == ITEMGET(14,80) ||
				 citem->m_Type == ITEMGET(14,81) ||
				 citem->m_Type == ITEMGET(14,82) )
		{
			UseCashItems(&gObj[aIndex],pos);
		}
		else if( citem->m_Type == ITEMGET(14,72) )
		{
			UsePKRemoveItem(&gObj[aIndex],pos);
		}
		else if( citem->m_Type == ITEMGET(14,70) || citem->m_Type == ITEMGET(14,71) )
		{
			UseExCashPotions(&gObj[aIndex],pos);
		}
		else if( citem->m_Type == ITEMGET(14,94) )
		{
			UseMediumElitePotion(&gObj[aIndex],pos,(double)0.65);
		}
		else if( citem->m_Type == ITEMGET(14,85) ||
				 citem->m_Type == ITEMGET(14,86) ||
				 citem->m_Type == ITEMGET(14,87) )
		{
			int BuffNumber = 0;

			if( citem->m_Type == ITEMGET(14,85) )
			{
				BuffNumber = BUFF_CHERRY_WINE;
			}
			else if( citem->m_Type == ITEMGET(14,86) )
			{
				BuffNumber = BUFF_CHERRY_RICECAKE;
			}
			else if( citem->m_Type == ITEMGET(14,87) )
			{
				BuffNumber = BUFF_CHERRY_FLOWERPETAL;
			}

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&gObj[aIndex],BuffNumber);
		}
		else if ( citem->m_Type == ITEMGET(14,7) )	// Siege Potion
		{
			int iLevel = citem->m_Level;
			int iSerial = citem->GetNumber();
			int iDur = (int)citem->m_Durability;

			if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			if ( gObj[aIndex].lpGuild )
			{
				LogAddTD("[CastleSiege] Using CastleSiege Potion Lv[%d]/Si[%d]/Dur[%d] - [%s][%s] - (Guild : %s)",
					iLevel, iSerial, iDur, gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].lpGuild->Name);
			}
			else
			{
				LogAddTD("[CastleSiege] Using CastleSiege Potion Lv[%d]/Si[%d]/Dur[%d] - [%s][%s]",
					iLevel, iSerial, iDur, gObj[aIndex].AccountID, gObj[aIndex].Name);
			}

			if ( iLevel == 0 )
			{
				gObjUseBlessAndSoulPotion(aIndex, iLevel);
			}
			else if ( iLevel == 1 )
			{
				gObjUseBlessAndSoulPotion(aIndex, iLevel);
			}
		}
		else if ( citem->m_Type == ITEMGET(13,48) )
		{
			if ( (gObj[aIndex].MapNumber < MAP_INDEX_KALIMA1 || gObj[aIndex].MapNumber > MAP_INDEX_KALIMA6 ) && gObj[aIndex].MapNumber != MAP_INDEX_KALIMA7 )
			{
				int iLevel = g_KalimaGate.GetKalimaGateLevel2(aIndex);
				int iSerial = citem->GetNumber();
				int iDuration = (int)citem->m_Durability;
				BOOL bKalimaGateCreateResult = FALSE;
				int iKalimaGateX = 0;
				int iKalimaGateY = 0;

				iLevel++;
				iKalimaGateX = gObj[aIndex].X + rand()%6 - 2;
				iKalimaGateY = gObj[aIndex].Y + rand()%6 - 2;

				bKalimaGateCreateResult = g_KalimaGate.CreateKalimaGate(aIndex, iLevel, iKalimaGateX, iKalimaGateY);

				if ( bKalimaGateCreateResult == TRUE )
				{
					if ( !gObjSearchItemMinus(&gObj[aIndex], pos, 1) )
					{
						gObjInventoryItemSet(aIndex, pos, -1);
						gObj[aIndex].pInventory[pos].Clear();
						GCInventoryItemDeleteSend(aIndex, pos, 1);
					}

					LogAddTD("[PCS] Use Free Kalima Ticket Success");
				}
				else
				{
					GCItemDurSend(aIndex,pos,(BYTE)gObj[aIndex].pInventory[pos].m_Durability,1);
					LogAddTD("[PCS] Use Free Kalima Ticket Failed");
				}
			}
		}
		else if( citem->m_Type == ITEMGET(13,69) )
		{
			int bMove = gMoveCommand.CheckEquipmentToMove(&gObj[aIndex],gObj[aIndex].DeathMap);

			if( !BC_MAP_RANGE(gObj[aIndex].MapNumber) &&
				!CC_MAP_RANGE(gObj[aIndex].MapNumber) &&
				!KALIMA_MAP_RANGE(gObj[aIndex].MapNumber) &&
				!DS_MAP_RANGE(gObj[aIndex].MapNumber) &&
				!IT_MAP_RANGE(gObj[aIndex].MapNumber) &&
				gObj[aIndex].MapNumber != MAP_INDEX_CASTLESIEGE &&
				gObj[aIndex].MapNumber != MAP_INDEX_CASTLEHUNTZONE
				&& !DG_MAP_RANGE(gObj[aIndex].MapNumber)
				&& !IF_MAP_RANGE(gObj[aIndex].MapNumber)
				&& bMove == TRUE )
			{
				gObjTeleport(aIndex,gObj[aIndex].DeathMap,gObj[aIndex].DeathX,gObj[aIndex].DeathY);
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);

				LogAddTD("[Using Item] [Charm of Raise] [%s][%s] Die Position Map:[%d] X:[%d] Y:[%d]",
					gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[aIndex].DeathMap,gObj[aIndex].DeathX,gObj[aIndex].DeathY);

				gObj[aIndex].DeathMap = 0;
				gObj[aIndex].DeathX = 0;
				gObj[aIndex].DeathY = 0;

				GCServerCmd(aIndex,60,0,0);
			}
			else
			{
				char szMsg[256];
				wsprintf(szMsg,lMsg.Get(MSGGET(4,228)));
				GCServerMsgStringSend(szMsg,aIndex,1);
			}
		}
		else if( citem->m_Type == ITEMGET(13,70) )
		{
			int bMove = gMoveCommand.CheckEquipmentToMove(&gObj[aIndex],gObj[aIndex].BindMap);

			if( gObjCheckMapBind(gObj[aIndex].MapNumber) == TRUE &&
				bMove == TRUE )
			{
				if( gObjSearchItemMinus(&gObj[aIndex],pos,1)== FALSE )
				{
					gObjTeleport(aIndex,gObj[aIndex].BindMap,gObj[aIndex].BindX,gObj[aIndex].BindY);

					gObjInventoryItemSet(aIndex, pos, -1);
					gObj[aIndex].pInventory[pos].Clear();
					GCInventoryItemDeleteSend(aIndex, pos, 1);

					gObj[aIndex].BindX = 0;
					gObj[aIndex].BindY = 0;
					gObj[aIndex].BindMap = 0;

					GCServerCmd(aIndex,61,0,0);


					LogAddTD("[Using Item] [Charm of Move] [%s][%s]",gObj[aIndex].AccountID,gObj[aIndex].Name);
				}
				else
				{
					gObj[aIndex].BindX = gObj[aIndex].X;
					gObj[aIndex].BindY = gObj[aIndex].Y;
					gObj[aIndex].BindMap = gObj[aIndex].MapNumber;

					LogAddTD("[Using Item] [Charm of Move - SavePosition] [%s][%s] Map:[%d] X:[%d] Y:[%d]",
						gObj[aIndex].AccountID,gObj[aIndex].Name,gObj[aIndex].BindMap,gObj[aIndex].BindX,gObj[aIndex].BindY);
				}
			}
			else
			{
				char szMsg[256];
				wsprintf(szMsg,lMsg.Get(MSGGET(4,228)));
				GCServerMsgStringSend(szMsg,aIndex,1);
			}
		}
		else if( citem->m_Type == ITEMGET(13,66) )
		{
			if( gObj[aIndex].Level < 15 )
			{
				GCServerMsgStringSend(lMsg.Get(MSGGET(13,53)),aIndex,1);
			}
			else
			{
				gObjTeleport(aIndex,62,220,20);

				if( gObjSearchItemMinus(&gObj[aIndex],pos,1) == FALSE )
				{
					gObjInventoryItemSet(aIndex, pos, -1);
					gObj[aIndex].pInventory[pos].Clear();
					GCInventoryItemDeleteSend(aIndex, pos, 1);
				}
			}
		}
		else if( citem->m_Type == ITEMGET(13,81) )
		{
			g_CashItemPeriodSystem.GDReqPeriodItemInsert(&gObj[aIndex],citem->m_Type,-1);

			gObjInventoryItemSet(aIndex, pos, -1);
			gObj[aIndex].pInventory[pos].Clear();
			GCInventoryItemDeleteSend(aIndex, pos, 1);
		}
		else if( citem->m_Type == ITEMGET(13,82) )
		{
			g_CashItemPeriodSystem.GDReqPeriodItemInsert(&gObj[aIndex],citem->m_Type,-1);

			gObjInventoryItemSet(aIndex, pos, -1);
			gObj[aIndex].pInventory[pos].Clear();
			GCInventoryItemDeleteSend(aIndex, pos, 1);
		}
		else if( citem->m_Type == ITEMGET(14,160) )
		{
			if ( gObjItemDurUp(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex,(WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if ( citem->m_Type == ITEMGET(14,161) )
		{
			if ( gObjItemStrenghtenUp(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex,(WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if( citem->m_Type == ITEMGET(14,162) )
		{
			PMSG_ANS_EXTEND pMsg;
			PHeadSetB((LPBYTE)&pMsg,0x2B,sizeof(pMsg));

			if( gObj[aIndex].pInventoryExtend >= 4 )
			{
				pMsg.btResult = 0;
				GCReFillSend(aIndex,(WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
			else
			{
				gObj[aIndex].pInventoryExtend++;
				GDExpandInventorySend(&gObj[aIndex],gObj[aIndex].pInventoryExtend);
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);

				pMsg.btResult = 1;
			}

			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		}
		else if( citem->m_Type == ITEMGET(14,163) )
		{
			PMSG_ANS_EXTEND pMsg;
			PHeadSetB((LPBYTE)&pMsg,0x2B,sizeof(pMsg));

			if( gObj[aIndex].pWarehouseExtend >= 1 )
			{
				pMsg.btResult = 0;
				GCReFillSend(aIndex,(WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
			else
			{
				gObj[aIndex].pWarehouseExtend = 1;
				GDExpandWarehouseSend(&gObj[aIndex],gObj[aIndex].pWarehouseExtend);

				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);

				pMsg.btResult = 1;
			}

			DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		}
		else if (citem->m_Type == ITEMGET(13, 152)
			|| citem->m_Type == ITEMGET(13, 156)) {
			if (g_MasterSkillSystem.CheckMasterSkillPoint(aIndex, 1)) {
				g_MasterSkillSystem.ResetMasterSkill(aIndex, 1);
				LogAddTD("[%s] [%s] [MasterLevelSkill Reset] - Passive", 
					gObj[aIndex].AccountID, gObj[aIndex].Name);
				gObjCloseSet(aIndex, 1);
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else {
				GCServerMsgStringSend("Master Skill Reset Fail", aIndex, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(13, 153)
			|| citem->m_Type == ITEMGET(13, 157)) {
			if (g_MasterSkillSystem.CheckMasterSkillPoint(aIndex, 2)) {
				g_MasterSkillSystem.ResetMasterSkill(aIndex, 2);
				LogAddTD("[%s] [%s] [MasterLevelSkill Reset] - Strengthen Skill", 
					gObj[aIndex].AccountID, gObj[aIndex].Name);
				gObjCloseSet(aIndex, 1);
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else {
				GCServerMsgStringSend("Master Skill Reset Fail", aIndex, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(13, 154)
			|| citem->m_Type == ITEMGET(13, 158)) {
			if (g_MasterSkillSystem.CheckMasterSkillPoint(aIndex, 3)) {
				g_MasterSkillSystem.ResetMasterSkill(aIndex, 3);
				LogAddTD("[%s] [%s] [MasterLevelSkill Reset] - Enhance Attack/Defense (%d)", 
					gObj[aIndex].AccountID, gObj[aIndex].Name);
				gObjCloseSet(aIndex, 1);
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else {
				GCServerMsgStringSend("Master Skill Reset Fail", aIndex, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(13, 155)
			|| citem->m_Type == ITEMGET(13, 159)) {
			if (g_MasterSkillSystem.CheckMasterSkillPoint(aIndex, 0)) {
				g_MasterSkillSystem.ResetMasterSkill(aIndex, 0);
				LogAddTD("[%s] [%s] [MasterLevelSkill Reset] - All", 
					gObj[aIndex].AccountID, gObj[aIndex].Name);
				gObjCloseSet(aIndex, 1);
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else {
				GCServerMsgStringSend("Master Skill Reset Fail", aIndex, 1);
			}
		}
#if (ENABLETEST_CUSTOM_JEWELS == 1)
		else if( citem->m_Type == ITEMGET(14,200) )
		{
			if ( gObjJewel1Enchant(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if( citem->m_Type == ITEMGET(14,201) )
		{
			if ( gObjJewel2Enchant(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if( citem->m_Type == ITEMGET(14,202) )
		{
			if ( gObjJewel3Enchant(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if( citem->m_Type == ITEMGET(14,203) )
		{
			if ( gObjJewel4Enchant(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
		else if( citem->m_Type == ITEMGET(14,204) )
		{
			if ( gObjJewel5Enchant(&gObj[aIndex], lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE )
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				gObj[aIndex].pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			}
		}
#endif
		else
		{
			LogAdd("error-L3 : %s return %s %d %d %s",
				gObj[aIndex].Name, __FILE__,__LINE__, pos,citem->GetName());

			GCReFillSend(aIndex,(WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
		}
	}
	else
	{
		GCReFillSend(aIndex,(WORD)gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
		LogAdd("error-L3 : %s return %s %d %d",
			gObj[aIndex].Name, __FILE__,__LINE__, pos);
	}
}



struct PMSG_REFILL
{
	PBMSG_HEAD h;	// C1:26
	BYTE IPos;	// 3
	BYTE LifeH;	// 4
	BYTE LifeL;	// 5
	BYTE Flag;	// 6
	BYTE btShieldH;	// 7
	BYTE btShieldL;	// 8
#if (FIX_HP==1)
	int HP;
	int SD;
#endif
};




void GCReFillSend(int aIndex, WORD Life, BYTE Ipos, unsigned char flag,  WORD wShield)
{
	PMSG_REFILL pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x26, sizeof(pMsg));
	pMsg.IPos = Ipos;
	pMsg.LifeH = SET_NUMBERH(Life);
	pMsg.LifeL = SET_NUMBERL(Life);
	pMsg.btShieldH = SET_NUMBERH(wShield);
	pMsg.btShieldL = SET_NUMBERL(wShield);
	pMsg.Flag = 0;

#if (FIX_HP==1)
	if( Ipos == 0xFF )
	{
		pMsg.HP = (int)gObj[aIndex].Life;
		pMsg.SD = gObj[aIndex].iShield;
	}
	else
	{
		pMsg.HP = (int)(gObj[aIndex].MaxLife + gObj[aIndex].AddLife);
		pMsg.SD = gObj[aIndex].iAddShield + gObj[aIndex].iMaxShield;
	}
#endif

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}



struct PMSG_MANASEND
{
	PBMSG_HEAD h;	// C1:27
	BYTE IPos;	// 3
	BYTE ManaH;	// 4
	BYTE ManaL;	// 5
	BYTE BPH;	// 6
	BYTE BPL;	// 7
#if (FIX_HP==1)
	int MP;
#endif
};


void GCManaSend(int aIndex, short Mana, BYTE Ipos, unsigned char flag,  WORD BP)
{
	if ( aIndex < 0 || aIndex > OBJMAX-1)
	{
		LogAdd("GCManaSend() return %s %d", __FILE__, __LINE__);
		return;
	}

	if ( gObj[aIndex].Type != OBJ_USER )
	{
		return;
	}

	PMSG_MANASEND pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x27, sizeof(pMsg));
	pMsg.IPos = Ipos;
	pMsg.ManaH = SET_NUMBERH(Mana);
	pMsg.ManaL = SET_NUMBERL(Mana);
	pMsg.BPH = SET_NUMBERH(BP);
	pMsg.BPL = SET_NUMBERL(BP);

#if (FIX_HP==1)
	if( Ipos == 0xFF )
		pMsg.MP = (int)gObj[aIndex].Mana;
	else
		pMsg.MP = (int)(gObj[aIndex].MaxMana+gObj[aIndex].AddMana);
#endif

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}



struct PMSG_INVENTORYDELETE
{
	PBMSG_HEAD h;	// C1:28
	BYTE IPos;	// 3
	BYTE Flag;	// 4
};


void GCInventoryItemDeleteSend(int aIndex, BYTE pos, unsigned char flag)
{
	PMSG_INVENTORYDELETE pMsg;
	
	PHeadSetB((LPBYTE)&pMsg, 0x28, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}



struct PMSG_ITEMUSESPECIALTIME
{
	PBMSG_HEAD h;	// C1:29
	BYTE Number;	// 3
	WORD Time;	// 4
};

void GCItemUseSpecialTimeSend(int aIndex, unsigned char number, int time)
{
	if ( time > 65535 )
	{
		time = 65535;
	}

	PMSG_ITEMUSESPECIALTIME pMsg;

	PHeadSetBE((LPBYTE)&pMsg, 0x29, sizeof(pMsg));
	pMsg.Number = number;
	pMsg.Time = time;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_ITEMDUR
{
	PBMSG_HEAD h;	// C1:2A
	BYTE IPos;	// 3
	BYTE Dur;	// 4
	BYTE Flag;	// 5
};



void GCItemDurSend(int aIndex, BYTE pos, BYTE dur, unsigned char flag)
{
	PMSG_ITEMDUR pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x2A, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	pMsg.Flag = flag;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}




void GCItemDurSend2(int aIndex, unsigned char pos, unsigned char dur, unsigned char flag)
{
	PMSG_ITEMDUR pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x2A, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	pMsg.Flag = flag;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}



struct PMSG_WEATHER
{
	PBMSG_HEAD h;	// C1:0F
	BYTE Weather;	// 3
};


void CGWeatherSend(int aIndex, BYTE weather)
{
	PMSG_WEATHER pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x0F, sizeof(pMsg));
	pMsg.Weather = weather;

	DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}





void GCServerCmd(int aIndex, BYTE type, BYTE Cmd1, BYTE Cmd2)
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = type;
	ServerCmd.X = Cmd1;
	ServerCmd.Y = Cmd2;

	DataSend(aIndex, (UCHAR*)&ServerCmd, ServerCmd.h.size);
}



struct PMSG_RESULT_MOVEDEVILSQUARE
{
	PBMSG_HEAD h;	// C1:90
	BYTE Result;	// 3
};


void GCReqmoveDevilSquare(PMSG_REQ_MOVEDEVILSQUARE* lpMsg, int aIndex)
{
	if ( aIndex < 0 || aIndex > OBJMAX-1 )
	{
		LogAdd("return %s %d", __FILE__, __LINE__);
		return;
	}

	BYTE cSquareNumber = lpMsg->SquareNumber;
	BYTE cInvitationItemPos = lpMsg->InvitationItemPos - INVENTORY_BAG_START;

	if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type != 12 )
		return;

	PMSG_RESULT_MOVEDEVILSQUARE pResult;

	PHeadSetB((LPBYTE)&pResult, 0x90, sizeof(pResult));
	pResult.Result = 0;

	LPOBJ lpObj = &gObj[aIndex];

	LogAddTD("[DevilSquare] [%s][%s] Request Move DevilSquare [%d][%d][%d]",
		lpObj->AccountID, lpObj->Name, lpObj->Class, lpObj->Level, cSquareNumber+1);

	int bIsPk = FALSE;

#if( ENABLE_FIX_PARTYPK == 1 )
	if ( lpObj->m_PK_Level >= 4 )
	{
		bIsPk = TRUE;
	}
#else
	if ( lpObj->PartyNumber >= 0 )
	{
		if( gParty.GetPartyPkLevel(lpObj->PartyNumber) >= 5 )
		{
			bIsPk = TRUE;
		}
	}
	else if ( lpObj->m_PK_Level >= 4 )
	{
		bIsPk = TRUE;
	}
#endif

#if( ENABLE_FIX_EVENTPK == 1 )
	if ( bIsPk == TRUE )
	{
		pResult.Result = 6;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
#endif

	if ( MAIN_INVENTORY_RANGE(cInvitationItemPos) == FALSE )
	{
		pResult.Result = 1;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		LogAddTD("[DevilSquare] [%s][%s] Move DevilSquare Fail [%d][%d][%d]",
			lpObj->AccountID, lpObj->Name, lpObj->Class, lpObj->Level, cSquareNumber+1);

		return;
	}

	if ( DS_LEVEL_RANGE(cSquareNumber) == FALSE )
	{
		pResult.Result = 1;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		LogAddTD("[DevilSquare] [%s][%s] Move DevilSquare Fail [%d][%d][%d]",
			lpObj->AccountID, lpObj->Name, lpObj->Class, lpObj->Level, cSquareNumber+1);

		return;
	}

	if ( g_DevilSquare.GetState() != DevilSquare_OPEN )
	{
		pResult.Result = 2;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		LogAddTD("[DevilSquare] [%s][%s] Move DevilSquare Fail [%d][%d][%d]",
			lpObj->AccountID, lpObj->Name, lpObj->Class, lpObj->Level, cSquareNumber+1);

		return;
	}

	if ( g_DevilSquare.m_DevilSquareGround[cSquareNumber].GetObjCount() >= MAX_DEVILSQUARE_USER )
	{
		pResult.Result = 5;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		LogAddTD("[DevilSquare] [%s][%s] Move DevilSquare Fail [%d][%d][%d]",
			lpObj->AccountID, lpObj->Name, lpObj->Class, lpObj->Level, cSquareNumber+1);

		return;
	}

	CItem * sitem = &lpObj->pInventory[cInvitationItemPos];

	if ( sitem->m_Type != ITEMGET(14,19) && ( sitem->m_Type != ITEMGET(13,46) || sitem->m_Durability <= 0.0f ) )	// Devil's Invitation
	{
		pResult.Result = 1;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		LogAddTD("[DevilSquare] [%s][%s] Move DevilSquare Fail [%d][%d][%d]",
			lpObj->AccountID, lpObj->Name, lpObj->Class, lpObj->Level, cSquareNumber+1);

		return;
	}

	int ItemLevel = sitem->m_Level;

	if ( sitem->m_Type == ITEMGET(13,46) )
		ItemLevel = 10;

	int level = lpObj->Level;

	if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA )
	{
		level = (level+1)/2*3;
	}

	if ( ItemLevel != 0 )
	{
		if ( ItemLevel != 10 )
		{
			if ( ItemLevel != (cSquareNumber+1) )
			{
				pResult.Result = 1;
				DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

				LogAddTD("[DevilSquare] [%s][%s] Move DevilSquare Invitation LevelError [%d][%d][%d][%d]",
					lpObj->AccountID, lpObj->Name, lpObj->Class, lpObj->Level, cSquareNumber+1, ItemLevel);

				return;
			}
		}
	}

	BOOL bEnterCheck = FALSE;
	int movegateindex = 61;	// #gate
	int iENTER_LEVEL = g_DevilSquare.GetUserLevelToEnter(aIndex, movegateindex);

	if ( DS_LEVEL_RANGE(iENTER_LEVEL) == FALSE )
	{
		LogAddTD("[DevilSquare] [%s][%s] GetUserLevelToEnter() failed",
			lpObj->AccountID, lpObj->Name);

		return;
	}

	if ( ItemLevel == 10 )
		cSquareNumber = iENTER_LEVEL;

	if ( cSquareNumber != iENTER_LEVEL )
	{
		if ( cSquareNumber > iENTER_LEVEL )
		{
			bEnterCheck = 2;
		}
		else
		{
			bEnterCheck = 1;
		}
	}

	if ( bEnterCheck == 1 )
	{
		pResult.Result = 3;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		LogAddTD("[DevilSquare] [%s][%s] Move DevilSquare Fail [%d][%d][%d][%d]",
			lpObj->AccountID, lpObj->Name, lpObj->Class, lpObj->Level, cSquareNumber, ItemLevel);

		return;
	}

	if ( bEnterCheck == 2 )
	{
		pResult.Result = 4;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

		LogAddTD("[DevilSquare] [%s][%s] Move DevilSquare Fail [%d][%d][%d][%d]",
			lpObj->AccountID, lpObj->Name, lpObj->Class, lpObj->Level, cSquareNumber, ItemLevel);

		return;
	}

	LogAddTD("[DevilSquare] [%s][%s] Move DevilSquare success [%d][%d] RemoveItem[%s][%d][%d][%d]",
		lpObj->AccountID, lpObj->Name, lpObj->Level, cSquareNumber, lpObj->pInventory[cInvitationItemPos].GetName(),
		cInvitationItemPos, ItemLevel, lpObj->pInventory[cInvitationItemPos].m_Number);

	if ( sitem->m_Type == ITEMGET(14,19) || (sitem->m_Type == ITEMGET(13,46) && sitem->m_Durability == 1.0f) )
	{
		gObjInventoryDeleteItem(aIndex, cInvitationItemPos);
		GCInventoryItemDeleteSend(aIndex, cInvitationItemPos, 1);
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
	}
	else if ( sitem->m_Type == ITEMGET(13,46) && sitem->m_Durability > 1.0f )
	{
		sitem->m_Durability -= 1.0f;
		GCItemDurSend2(lpObj->m_Index, cInvitationItemPos,(BYTE)sitem->m_Durability, 0);
	}

	lpObj->m_nEventExp = 0;
	lpObj->m_nEventScore = 0;
	lpObj->m_nEventMoney = 0;
	lpObj->m_bDevilSquareIndex = g_DevilSquare.GetDevilSquareIndex(movegateindex);
	lpObj->m_bDevilSquareAuth = true;
	lpObj->m_IfState.state = 0;
	lpObj->m_IfState.type = 0;
	lpObj->m_IfState.use = 0;

	gObjMoveGate(lpObj->m_Index, movegateindex);
	g_DevilSquare.m_DevilSquareGround[cSquareNumber].IncObjCount();
}


struct PMSG_RESULT_DEVILSQUARE_REMAINTIME
{
	PBMSG_HEAD h;	// C1:91
	BYTE hEventType;	// 3
	BYTE RemainTime;	// 4
	BYTE EnteredUser;	// 5
	BYTE RemainTime_LOW;	// 6
};



void GCReqDevilSquareRemainTime(PMSG_REQ_DEVILSQUARE_REMAINTIME* lpMsg, int aIndex)
{
	PMSG_RESULT_DEVILSQUARE_REMAINTIME pResult;

	PHeadSetB((LPBYTE)&pResult, 0x91, sizeof(pResult));
	pResult.RemainTime = 0;

	switch ( lpMsg->hEventType )
	{
		case 1:
#if (ENABLE_FIX_EVENTPK == 1)
			if ( gObj[aIndex].m_PK_Level >= 4 )
			{
				GCServerCmd(aIndex, 0x37, 0, 0);

				LogAddTD("[PK User][DevilSquare] [%s][%s] Move Fail [PK Level:%d]",
					gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_PK_Level);

				return;
			}
#endif

			pResult.hEventType = 1;

			if ( g_DevilSquare.GetState() == DevilSquare_CLOSE )
			{
				pResult.RemainTime = g_DevilSquare.GetRemainTime();
			}
			else if ( g_DevilSquare.GetState() == DevilSquare_OPEN )
			{
				pResult.RemainTime = 0;
			}
			else
			{
				pResult.RemainTime = g_DevilSquare.GetRemainTime() + g_DevilSquare.m_iCloseTime;
			}
			break;

		case 2:
#if( ENABLE_FIX_EVENTPK == 1 )
			if ( gObj[aIndex].m_PK_Level >= 4 )
			{
				GCServerCmd(aIndex, 0x38, 0, 0);

				LogAddTD("[PK User][BloodCastle] [%s][%s] Move Fail [PK Level:%d]",
					gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_PK_Level);

				return;
			}
#endif

			pResult.hEventType = 2;

			if ( g_BloodCastle.GetCurrentState(lpMsg->btItemLevel-1) == 1 )
			{
				if ( g_BloodCastle.CheckCanEnter(lpMsg->btItemLevel-1) != false )
				{
					pResult.RemainTime = 0;
				}
				else
				{
					pResult.RemainTime = g_BloodCastle.GetRemainTime(lpMsg->btItemLevel-1);
				}
			}
			else
			{
				pResult.RemainTime = g_BloodCastle.GetRemainTime(lpMsg->btItemLevel-1);
			}
			break;

		case 4:
			{
#if( ENABLE_FIX_EVENTPK == 1 )
#else
			if ( gObj[aIndex].m_PK_Level >= 4 )
			{
				GCServerCmd(aIndex, 0x39, 0, 0);

				LogAddTD("[PK User][ChaosCastle] [%s][%s] Move Fail [PK Level:%d]",
					gObj[aIndex].AccountID, gObj[aIndex].Name, gObj[aIndex].m_PK_Level);

				return;
			}
#endif

			int iENTER_LEVEL =  g_ChaosCastle.GetUserLevelToEnter(aIndex);

			if ( iENTER_LEVEL == -1 )
				return;

			pResult.hEventType = 4;

			if ( g_ChaosCastle.GetCurrentState(iENTER_LEVEL) == 1 )
			{
				if ( g_ChaosCastle.CheckCanEnter(iENTER_LEVEL) != false )
				{
					pResult.RemainTime = 0;
					pResult.RemainTime_LOW = 0;
					pResult.EnteredUser = g_ChaosCastle.GetCurEnteredUser(iENTER_LEVEL);
				}
				else
				{
					WORD wREMAIN_TIME = g_ChaosCastle.GetRemainTime(iENTER_LEVEL);
					pResult.RemainTime = SET_NUMBERH(wREMAIN_TIME);
					pResult.RemainTime_LOW = SET_NUMBERL(wREMAIN_TIME);
					pResult.EnteredUser = g_ChaosCastle.GetCurEnteredUser(iENTER_LEVEL);
				}
			}
			else
			{
				WORD wREMAIN_TIME = g_ChaosCastle.GetRemainTime(iENTER_LEVEL);
				pResult.RemainTime = SET_NUMBERH(wREMAIN_TIME);
				pResult.RemainTime_LOW = SET_NUMBERL(wREMAIN_TIME);
				pResult.EnteredUser = g_ChaosCastle.GetCurEnteredUser(iENTER_LEVEL);
			}
			}
			break;

		case 5:

			pResult.hEventType = 5;
			pResult.RemainTime = g_IllusionTemple.GetLeftTime();
			pResult.EnteredUser = 0;
			pResult.RemainTime_LOW = 0;
			break;
	}

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
}




void AllSendMsg(LPBYTE Msg, int size)
{
	for ( int n=OBJ_STARTUSERINDEX;n<OBJMAX;n++)
	{
		if ( gObj[n].Connected == PLAYER_PLAYING )
		{
			if ( gObj[n].Type == OBJ_USER )
			{
				DataSend(n, Msg, size);
			}
		}
	}
}




void AllSendSameMapMsg(UCHAR * Msg, int size, BYTE mapnumber)
{
	for ( int n=0;n<OBJMAX;n++)
	{
		if ( gObj[n].Connected == PLAYER_PLAYING )
		{
			if ( gObj[n].Type == OBJ_USER )
			{
				if ( gObj[n].MapNumber == mapnumber )
				{
					DataSend(n, Msg, size);
				}
			}
		}
	}
}


struct PMSG_REQ_REGISTER_EVENTCHIP
{
	PBMSG_HEAD h;	// C1:02
	int iINDEX;	// 4
	BYTE Pos;	// 8
	char szUID[11];	// 9
};


struct PMSG_REQ_REGISTER_STONES
{
	PBMSG_HEAD h;	// C1:06
	int iINDEX;	// 4
	BYTE iPosition;	// 8
	char szUID[11];	// 9
};



void GCRegEventChipRecv(PMSG_REGEVENTCHIP* lpMsg, int aIndex)
{
#if (__CUSTOM__ == 1)
	if ( !EVENCHIP_TYPE_RANGE(lpMsg->Type) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->m_IfState.type == 1 )
	{
		LogAddTD("[EventChip] [%s][%s] Attempted ItemCopy using Trade Window",
			lpObj->AccountID, lpObj->Name);

		return;
	}

	if ( lpObj->m_IfState.type == 7 )
	{
		LogAddTD("[EventChip] [%s][%s] Attempted ItemCopy using ChaosBox Window",
			lpObj->AccountID, lpObj->Name);

		return;
	}

	if ( lpObj->UseEventServer != FALSE )
	{
		PMSG_REGEVENTCHIP_RESULT Result;

		PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
		Result.ChipCount = -1;
		Result.Type = lpMsg->Type;

		LogAddTD("[EventChip] [%s][%s] Not Found EventChip (RegEventchip) #1 %d",
			lpObj->AccountID, lpObj->Name, lpMsg->ChipPos);

		DataSend(aIndex, (LPBYTE)&Result, Result.h.size);

		return;
	}

	int Pos = lpMsg->ChipPos + INVENTORY_BAG_START;
	CItem * sitem = &lpObj->pInventory[Pos];

	if ( !sitem->IsItem() )
	{
		PMSG_REGEVENTCHIP_RESULT Result;

		PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
		Result.ChipCount = -1;
		Result.Type = lpMsg->Type;

		LogAddTD("[EventChip] [%s][%s] Not Found EventChip (RegEventchip) #2 %d",
			lpObj->AccountID, lpObj->Name, lpMsg->ChipPos);

		DataSend(aIndex, (LPBYTE)&Result, Result.h.size);

		return;
	}

	lpObj->UseEventServer = TRUE;

	switch ( lpMsg->Type )
	{
		case 0x00:
			{
				int count = gObjGetItemCountInIventory(lpObj->m_Index,14,21,0);	// Rena

				if(count >= g_RenaCount)
				{
					gObjDelteItemCountInInventory(lpObj->m_Index,14,21,0,g_RenaCount);
					g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenArcher, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
				}
				else
				{
					MsgOutput(lpObj->m_Index,"You lack items to exchange!");
				}
			}
			break;
		case 0x01:
			{
				int count = gObjGetItemCountInIventory(lpObj->m_Index,14,21,1);	// Stone

				if(count >= g_StoneCount)
				{
					gObjDelteItemCountInInventory(lpObj->m_Index,14,21,1,g_StoneCount);
					g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenArcher, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
				}
				else
				{
					MsgOutput(lpObj->m_Index,"You lack items to exchange!");
				}
			}
			break;
	}

	lpObj->UseEventServer = FALSE;

#endif
}


struct PMSG_REQ_REGISTER_MUTONUM
{
	PBMSG_HEAD h;	// C1:03
	int iINDEX;	// 4
	char szUID[11];	// 8
};


void GCGetMutoNumRecv(PMSG_GETMUTONUMBER* lpMsg, int aIndex)
{
	if ( gObj[aIndex].MutoNumber != 0 )
	{
		char msg[255];
		wsprintf(msg, "ÀÌ¹Ì ·ç°¡µåÀÇ ¼ýÀÚ°¡ ÀÖ½À´Ï´Ù");
		GCServerMsgStringSend(msg, aIndex, 1);
		return;
	}

	if ( gObj[aIndex].UseEventServer != FALSE )
		return;

	gObj[aIndex].UseEventServer = TRUE;

	if ( !gObjFind10EventChip(aIndex) )
	{
		PMSG_GETMUTONUMBER_RESULT Result;

		PHeadSetB((LPBYTE)&Result, 0x96, sizeof(Result));
		Result.MutoNum[0] = -1;
		Result.MutoNum[1] = 0;
		Result.MutoNum[2] = 0;

		DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
		gObj[aIndex].UseEventServer = FALSE;

		return;
	}

	PMSG_REQ_REGISTER_MUTONUM pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x03, sizeof(pMsg));
	pMsg.iINDEX = aIndex;
	strcpy(pMsg.szUID, gObj[aIndex].AccountID);

	DataSendEventChip((PCHAR)&pMsg, sizeof(pMsg));

	LogAddTD("[EventChip] [%s][%s] Request MutoNumber",
		gObj[aIndex].AccountID, gObj[aIndex].Name);
}




void GCUseEndEventChipRescv(int aIndex)
{
	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type == 9 )
	{
		gObj[aIndex].m_IfState.state = 0;
		gObj[aIndex].m_IfState.type = 0;
		gObj[aIndex].m_IfState.use = 0;
	}
}


struct PMSG_REQ_RESET_EVENTCHIP
{
	PBMSG_HEAD h;	// C1:[04:Rena] [09:Stone]
	int iINDEX;	// 4
	char szUID[11];	// 8
};


void GCUseRenaChangeZenRecv(PMSG_EXCHANGE_EVENTCHIP* lpMsg, int aIndex)
{
	if ( gObj[aIndex].UseEventServer )
		return;

	gObj[aIndex].UseEventServer = TRUE;

	PMSG_REQ_RESET_EVENTCHIP pMsg;

	if ( lpMsg->btType == 1 )	// Stone?
		PHeadSetB((LPBYTE)&pMsg, 0x09, sizeof(pMsg));
	else
		PHeadSetB((LPBYTE)&pMsg, 0x04, sizeof(pMsg));

	pMsg.iINDEX = aIndex;
	strcpy(pMsg.szUID, gObj[aIndex].AccountID);

	DataSendEventChip((PCHAR)&pMsg, sizeof(pMsg));

	if ( lpMsg->btType == 0x01 )
		LogAddTD("[EventChip] [%s][%s] Request Change Stones", gObj[aIndex].AccountID, gObj[aIndex].Name);
	else
		LogAddTD("[EventChip] [%s][%s] Request Change Rena", gObj[aIndex].AccountID, gObj[aIndex].Name);
}


struct PMSG_SEND_QEUSTINFO
{
	PBMSG_HEAD h;	// C1:A0
	BYTE Count;	// 3
	BYTE State[50];	// 4
};


void CGRequestQuestInfo(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->m_SendQuestInfo != 0 )
		return;

	int questcount = g_QuestInfo.GetQeustCount();
	int foundquest = 0;

	PMSG_SEND_QEUSTINFO pMsg;
	LPQUEST_INFO lpQuestInfo;

	memcpy(pMsg.State, lpObj->m_Quest, sizeof(pMsg.State));

	int i;
	for (i=0;i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = g_QuestInfo.GetQuestInfo(i);

		if ( lpQuestInfo )
		{
			foundquest++;

			if ( foundquest == questcount )
			{
				break;
			}
		}
	}

	int iSize = i/4+5;

	PHeadSetB((LPBYTE)&pMsg, 0xA0, iSize);
	pMsg.Count = i;
	DataSend(aIndex, (LPBYTE)&pMsg, iSize);
	lpObj->m_SendQuestInfo = true;

}


struct MONQUEST_INFO
{
	int m_Id;
	int m_Count;
};

struct PMSG_MONQUEST_INFO
{
	PBMSG_HEAD h;
	int QuestIndex;
};


void GCSendQuestInfo(int aIndex, int QuestIndex)
{
	CGRequestQuestInfo(aIndex);

	LPOBJ lpObj = &gObj[aIndex];
	PMSG_SETQUEST pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0xA1, sizeof(pMsg));
	pMsg.QuestIndex = QuestIndex;
	pMsg.State = g_QuestInfo.GetQuestStateBYTE(lpObj, QuestIndex);

	if ( pMsg.State != 0 )
		DataSend(aIndex, (UCHAR*)&pMsg, sizeof(pMsg));
}



struct PMSG_SETQUEST_RESULT
{
	PBMSG_HEAD h;	// C1:A2
	BYTE QuestIndex;	// 3
	BYTE Result;	// 4
	BYTE State;	// 5
};



void CGSetQuestState(PMSG_SETQUEST* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	PMSG_SETQUEST_RESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0xA2, sizeof(pMsg));
	pMsg.QuestIndex = lpMsg->QuestIndex;
	pMsg.Result = g_QuestInfo.SetQuestState(lpObj, lpMsg->QuestIndex, lpMsg->State);
	pMsg.State = g_QuestInfo.GetQuestStateBYTE(lpObj, lpMsg->QuestIndex);

	DataSend(aIndex, (UCHAR*)&pMsg, sizeof(pMsg));
}



struct PMSG_SETQUEST_PRIZE
{
	PBMSG_HEAD h;	// C1:A3
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Type;	// 5
	BYTE Count;	// 6
};



void GCSendQuestPrize(int aIndex, BYTE Type, BYTE Count)
{
	CGRequestQuestInfo(aIndex);
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_SETQUEST_PRIZE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0xA3, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);
	pMsg.Type = Type;
	pMsg.Count = Count;

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
	MsgSendV2(lpObj, (LPBYTE)&pMsg, sizeof(pMsg));
}




void CGCloseWindow(int aIndex)
{
	if ( gObj[aIndex].m_IfState.use && gObj[aIndex].m_IfState.type )
	{
		if ( gObj[aIndex].m_IfState.type == 7 )
			return;

		if ( gObj[aIndex].m_IfState.type == 10 && gObj[aIndex].TargetShopNumber == 229 )	// Marlon
		{
			gQeustNpcTeleport.TalkRefDel();
			gObj[aIndex].TargetShopNumber = -1;
		}

		if ( gObj[aIndex].m_IfState.type == 3 )
		{
			gObj[aIndex].TargetShopNumber = -1;
		}

		if ( gObj[aIndex].m_IfState.type == 1 )
		{
			CGTradeCancelButtonRecv(aIndex);
		}

		if ( gObj[aIndex].m_IfState.type == 6 )
		{
			CGWarehouseUseEnd(aIndex);
		}

		if ( gObj[aIndex].m_IfState.type == 13 )
		{
			g_ChaosBox.ChaosBoxInit(&gObj[aIndex]);
			gObjInventoryCommit(aIndex);
		}

		gObj[aIndex].m_IfState.state = 0;
		gObj[aIndex].m_IfState.type = 0;
		gObj[aIndex].m_IfState.use = 0;
	}
}


void CGRequestEnterBloodCastle(PMSG_REQ_MOVEBLOODCASTLE* lpMsg, int iIndex)
{
	if ( !OBJMAX_RANGE(iIndex))
	{
		LogAdd("return %s %d", __FILE__, __LINE__);
		return;
	}
	
	BYTE btBridgeNumber = lpMsg->iBridgeNumber - 1;
	BYTE btInvisibleCourtItemPos = lpMsg->iItemPos;
	int iITEM_LEVEL = 0;
	DWORD dwITEM_SERIAL = 0;
	PMSG_RESULT_MOVEBLOODCASTLE pResult;

	PHeadSetB((LPBYTE)&pResult, 0x9A, sizeof(pResult));
	pResult.Result = 0;

	if ( gObj[iIndex].Type != OBJ_USER || gObj[iIndex].Connected <= PLAYER_LOGGED )
		return;

	if ( gObj[iIndex].m_IfState.use && gObj[iIndex].m_IfState.type != 12 )
		return;

#ifndef BEREZNUK
	if( gObj[iIndex].m_PK_Level >= 4 )
	{
		pResult.Result = 7;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
#endif

	if ( g_BloodCastle.CheckCanEnter(btBridgeNumber) == false )
	{
		pResult.Result = 2;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( gObj[iIndex].pInventory[btInvisibleCourtItemPos].IsItem() == TRUE )
	{
		if ( gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Type == ITEMGET(13,18) )
		{
			iITEM_LEVEL = gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Level;
			dwITEM_SERIAL = gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Number;
			
			if ( BC_CLOACK_LEVEL_RANGE(iITEM_LEVEL)==FALSE)
			{
				pResult.Result = 1;
				DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
				return;
			}
		}
		else if ( gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Type == ITEMGET(13,47) &&
				  gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Durability > 0.0f ) //#unk
		{
#if (__CUSTOM__ == 1)
			if ( gObj[iIndex].ChangeUP2 == TRUE )
#else
			if ( g_MasterExp.IsEnabled(&gObj[iIndex]) )
#endif
			{
				iITEM_LEVEL = 8;
			}
			else
			{
				for ( int i=0;i<MAX_CLOACK_LEVEL;i++)
				{
					if ( g_BloodCastle.CheckEnterLevel(iIndex, i+1) == 0 )
					{
						iITEM_LEVEL = i+1;
						break;
					}
				}
			}

			dwITEM_SERIAL = gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Number;

			if ( BC_CLOACK_LEVEL_RANGE(iITEM_LEVEL) == FALSE )
			{
				pResult.Result = 1;
				DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
				return;
			}
		}
		else
		{
			pResult.Result = 1;
			DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}
	else
	{
		pResult.Result = 1;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

//#endif
	
	int iRESULT = g_BloodCastle.CheckEnterLevel(iIndex, iITEM_LEVEL);

	if ( iRESULT== 0 )
	{
		pResult.Result = 0;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
	}
	else 
	{
		if ( iRESULT == -1 )
		{
			pResult.Result = 4;
			DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if ( iRESULT == 1 )
		{
			pResult.Result = 3;
			DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		return;
	}
		
	int iBC_INDEX = g_BloodCastle.EnterUserBridge(iITEM_LEVEL-1, iIndex);

	if ( iBC_INDEX == -1 )
	{
		pResult.Result = 5;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( (gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Type == ITEMGET(13,18) ||
		(gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Type == ITEMGET(13,47)) &&
		gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Durability == 1.0f ))
	{
		gObjInventoryDeleteItem(iIndex, btInvisibleCourtItemPos);
		GCInventoryItemDeleteSend(iIndex, btInvisibleCourtItemPos, 1);
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
	}
	else if (gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Type == ITEMGET(13,47) && gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Durability > 1.0f )
	{
		gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Durability -= 1.0f;
		GCItemDurSend2(iIndex, btInvisibleCourtItemPos,(BYTE)gObj[iIndex].pInventory[btInvisibleCourtItemPos].m_Durability, 0);
	}

	gObj[iIndex].m_cBloodCastleIndex = iITEM_LEVEL-1;
	gObj[iIndex].m_cBloodCastleSubIndex = iBC_INDEX;
	gObj[iIndex].m_iBloodCastleEXP = 0;
	gObj[iIndex].m_IfState.state = 0;
	gObj[iIndex].m_IfState.type = 0;
	gObj[iIndex].m_IfState.use = 0;

	g_BloodCastle.SearchUserDeleteQuestItem(iIndex);
	char szTemp[256];
	wsprintf(szTemp, lMsg.Get(MSGGET(4, 147)), iITEM_LEVEL);
	GCServerMsgStringSend(szTemp, iIndex, 1);

	switch ( iITEM_LEVEL )
	{
		case 1:
			gObjMoveGate(iIndex, 66);
			break;
		case 2:
			gObjMoveGate(iIndex, 67);
			break;
		case 3:
			gObjMoveGate(iIndex, 68);
			break;
		case 4:
			gObjMoveGate(iIndex, 69);
			break;
		case 5:
			gObjMoveGate(iIndex, 70);
			break;
		case 6:
			gObjMoveGate(iIndex, 71);
			break;
		case 7:
			gObjMoveGate(iIndex, 80);
			break;
		case 8:
			gObjMoveGate(iIndex,271);
			break;
	}

	LogAddTD("[Blood Castle] (%d) (Account:%s, Name:%s) Entered Blood Castle (Invisible Cloak Serial:%d)",
		iITEM_LEVEL, gObj[iIndex].AccountID, gObj[iIndex].Name, dwITEM_SERIAL);
}



struct PMSG_RESULT_MOVECHAOSCASTLE
{
	PBMSG_HEAD2 h;	// C1:AF:01
	BYTE Result;	// 4
};



void CGRequestEnterChaosCastle(PMSG_REQ_MOVECHAOSCASTLE* lpMsg, int iIndex)
{
	if ( !OBJMAX_RANGE(iIndex))
	{
		LogAdd("return %s %d", __FILE__, __LINE__);
		return;
	}

	BYTE btChaosCastleNumber = lpMsg->iChaosCastleNumber-1;
	BYTE btGuardMailSetItemPos = lpMsg->iItemPos;
	int iITEM_LEVEL=0;
	DWORD dwITEM_SERIAL=0;
	PMSG_RESULT_MOVECHAOSCASTLE pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0xAF, 0x01, sizeof(pResult));
	pResult.Result = 0;

	if ( gObj[iIndex].Type != OBJ_USER || gObj[iIndex].Connected <= PLAYER_LOGGED )
		return;

	if ( BC_MAP_RANGE(gObj[iIndex].MapNumber) ||
	 	 CC_MAP_RANGE(gObj[iIndex].MapNumber) ||
		 DS_MAP_RANGE(gObj[iIndex].MapNumber) )
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, lMsg.Get(MSGGET(4, 200)));
		DataSend(iIndex, (LPBYTE)&pNotice, pNotice.h.size);
		return;
	}

	if ( gObj[iIndex].m_IfState.use && gObj[iIndex].m_IfState.type != 12 )
		return;

	if ( gPkLimitFree == FALSE )
	{
		int bIsPk = FALSE;

		if ( gObj[iIndex].PartyNumber >= 0 )
		{
			if( gParty.GetPartyPkLevel(gObj[iIndex].PartyNumber) >= 6 )
			{
				bIsPk = TRUE;
			}
		}
		else if( gObj[iIndex].m_PK_Level >= 6 )
		{
			bIsPk = TRUE;
		}
		
		
		if ( bIsPk == TRUE )
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, lMsg.Get(MSGGET(4, 201)));
			DataSend(iIndex, (LPBYTE)&pNotice, pNotice.h.size);
			return;
		}
	}

	if ( gObjDuelCheck(&gObj[iIndex]) == TRUE )
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, lMsg.Get(MSGGET(4, 202)));
		DataSend(iIndex, (LPBYTE)&pNotice, pNotice.h.size);
		return;
	}

	if ( gObj[iIndex].lpGuild )
	{
		if ( gObj[iIndex].lpGuild->WarState == 1 )
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, lMsg.Get(MSGGET(4, 203)));
			DataSend(iIndex, (LPBYTE)&pNotice, pNotice.h.size);
			return;
		}
	}

	if ( gObj[iIndex].pInventory[8].m_Type == ITEMGET(13,3) ||	// Dino
		 gObj[iIndex].pInventory[8].m_Type == ITEMGET(13,2) ||	// Uniria
		 gObj[iIndex].pInventory[8].m_Type == ITEMGET(13,37) )	// Fenrir
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, lMsg.Get(MSGGET(4, 204)));
		DataSend(iIndex, (LPBYTE)&pNotice, pNotice.h.size);
		return;
	}

	int bIsPk = FALSE;

	if ( gObj[iIndex].PartyNumber >= 0 )
	{
		if( gParty.GetPartyPkLevel(gObj[iIndex].PartyNumber) >= 5 )
		{
			bIsPk = TRUE;
		}
	}
	else if ( gObj[iIndex].m_PK_Level >= 4 )
	{
		bIsPk = TRUE;
	}

	if ( bIsPk == TRUE )
	{
		pResult.Result = 8;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	int iENTER_LEVEL = g_ChaosCastle.GetUserLevelToEnter(iIndex);

	if ( iENTER_LEVEL == -1 )
	{
		pResult.Result = 1;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( g_ChaosCastle.CheckCanEnter(iENTER_LEVEL) == false )
	{
		pResult.Result = 2;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if ( gObj[iIndex].pInventory[btGuardMailSetItemPos].IsItem() == TRUE )
	{
		if ( gObj[iIndex].pInventory[btGuardMailSetItemPos].m_Type == ITEMGET(13,29) ) // Armor of Guardman
		{
			iITEM_LEVEL = gObj[iIndex].pInventory[btGuardMailSetItemPos].m_Level;
			dwITEM_SERIAL = gObj[iIndex].pInventory[btGuardMailSetItemPos].m_Number;
		}
		else
		{
			pResult.Result = 1;
			DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}
	else
	{
		pResult.Result = 1;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if( IsTransformEquipment(iIndex) )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 205)), iIndex, 1);
		return;
	}

	BYTE btAttr = MapC[gObj[iIndex].MapNumber].GetAttr(gObj[iIndex].X,gObj[iIndex].Y);

	if( (btAttr & 1) != 1 )
	{
		return;
	}

	int iCC_INDEX = -1;

	if ( g_ChaosCastle.CheckUserEnterMoney(iIndex, iENTER_LEVEL) )
	{
		iCC_INDEX = g_ChaosCastle.EnterUserChaosCastle(iENTER_LEVEL, iIndex);

		if ( iCC_INDEX == -1 )
		{
			pResult.Result = 5;
			DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}
	else
	{
		pResult.Result = 7;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	BOOL bRES = g_ChaosCastle.PayUserEnterMoney(iIndex, iENTER_LEVEL);

	if ( bRES == FALSE )
	{
		g_ChaosCastle.LeaveUserChaosCastle(iENTER_LEVEL, iIndex);
		pResult.Result = 5;
		DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	gObjInventoryDeleteItem(iIndex, btGuardMailSetItemPos);
	GCInventoryItemDeleteSend(iIndex, btGuardMailSetItemPos, 1);

	DataSend(iIndex, (LPBYTE)&pResult, pResult.h.size);

	gObj[iIndex].m_cChaosCastleIndex = iENTER_LEVEL;
	gObj[iIndex].m_cChaosCastleSubIndex = iCC_INDEX;
	gObj[iIndex].m_IfState.state = 0;
	gObj[iIndex].m_IfState.type = 0;
	gObj[iIndex].m_IfState.use = 0;

	if ( gObj[iIndex].m_bPShopOpen == true )
		CGPShopReqClose(iIndex);

	if ( gObj[iIndex].PartyNumber >= 0 )
	{
		PMSG_PARTYDELUSER pMsg;
		int pindex = gParty.GetIndex(gObj[iIndex].PartyNumber, iIndex, gObj[iIndex].DBNumber);

		if ( pindex >= 0 )
		{
			pMsg.Number = pindex;
			CGPartyDelUser(&pMsg, iIndex);
		}
	}

	if ( gObj[iIndex].Class == CLASS_ELF && gObj[iIndex].m_RecallMon >= 0)
	{
		GCRecallMonLife(iIndex, 60, 0);
		gObjMonsterCallKill(iIndex);
	}

	char szTemp[256];
	wsprintf(szTemp, lMsg.Get(MSGGET(4, 206)), iENTER_LEVEL+1);
	GCServerMsgStringSend(szTemp, iIndex, 1);

	switch ( iENTER_LEVEL+1 )
	{
		case 1:
			gObjMoveGate(iIndex, 82);
			break;
		case 2:
			gObjMoveGate(iIndex, 83);
			break;
		case 3:
			gObjMoveGate(iIndex, 84);
			break;
		case 4:
			gObjMoveGate(iIndex, 85);
			break;
		case 5:
			gObjMoveGate(iIndex, 86);
			break;
		case 6:
			gObjMoveGate(iIndex, 87);
			break;
		case 7:
			gObjMoveGate(iIndex, 272);
			break;
	}

	LogAddTD("[Chaos Castle] (%d) (Account:%s, Name:%s) Entered Chaos Castle (Guard Mail Set Serial:%d)",
		iENTER_LEVEL+1, gObj[iIndex].AccountID, gObj[iIndex].Name, dwITEM_SERIAL);
}




void CGRequestRepositionUserInChaosCastle(PMSG_REQ_REPOSUSER_IN_CC * lpMsg, int aIndex)
{
	if ( !gObjIsConnected(aIndex) )
		return;

	if ( !CC_MAP_RANGE(gObj[aIndex].MapNumber) )
		return;

	if ( g_ChaosCastle.ObjSetPosition(aIndex, gObj[aIndex].X, gObj[aIndex].Y) == TRUE )
		gObj[aIndex].m_iChaosCastleBlowTime = GetTickCount();
}





void CGRequestEventEnterCount(PMSG_REQ_CL_ENTERCOUNT* lpMsg, int aIndex)
{
	if ( !lpMsg )
		return;

	switch ( lpMsg->btEventType )
	{
		case 0x02:
			EGReqBloodCastleEnterCount(aIndex);
			break;
	}
}

struct PMSG_REQ_2ANIV_SERIAL
{
	PBMSG_HEAD h;	// C1:08
	int iINDEX;	// 8
	char szUID[MAX_ACCOUNT_LEN+1];	// 8
	char SERIAL1[5];	// 13
	char SERIAL2[5];	// 18
	char SERIAL3[5];	// 1D
	int iMEMB_GUID;	// 24
};


void CGRequestLottoRegister(PMSG_REQ_2ANV_LOTTO_EVENT* lpMsg, int aIndex)
{
	PMSG_REQ_2ANIV_SERIAL pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x08, sizeof(pMsg));

	if ( gObj[aIndex].UseEventServer )
		return;

	gObj[aIndex].UseEventServer = TRUE;
	pMsg.iINDEX = aIndex;
	pMsg.iMEMB_GUID = gObj[aIndex].DBNumber;
	memcpy(pMsg.szUID, gObj[aIndex].AccountID, MAX_ACCOUNT_LEN);
	pMsg.szUID[MAX_ACCOUNT_LEN] = 0;
	memcpy(pMsg.SERIAL1, lpMsg->SERIAL1, 4);
	pMsg.SERIAL1[4] = 0;
	memcpy(pMsg.SERIAL2, lpMsg->SERIAL2, 4);
	pMsg.SERIAL2[4] = 0;
	memcpy(pMsg.SERIAL3, lpMsg->SERIAL3, 4);
	pMsg.SERIAL3[4] = 0;

	DataSendEventChip((PCHAR)&pMsg, sizeof(pMsg));

	LogAddTD("[Mu_2Anv_Event] [%s][%s] Register Lotto Number (Serial: %s-%s-%s)",
		gObj[aIndex].AccountID, gObj[aIndex].Name, pMsg.SERIAL1, pMsg.SERIAL2, pMsg.SERIAL3);
}


struct SDHP_CHARACTER_TRANSFER
{
	PBMSG_HEAD h;	// C1:CF
	char Account[10];	// 3
	short Number;	// E
};



void CGReqMoveOtherServer(PMSG_REQ_MOVE_OTHERSERVER * lpMsg, int aIndex)
{
	if ( !gEnableServerDivision)
		return;

	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddC(2, "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->m_MoveOtherServer )
		return;

	lpObj->m_MoveOtherServer = true;

	PMSG_DEFRESULT pResult;
	PHeadSetB((LPBYTE)&pResult, 0x99, sizeof(pResult));
	
	if ( !gObjJoominCheck(aIndex, lpMsg->LastJoominNumber))
	{
		pResult.result = 0;

		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
		lpObj->m_MoveOtherServer = false;

		LogAddTD("[CharTrasfer] Fail (JoominNumber) [%s][%s]",
			lpObj->AccountID, lpObj->Name);

		GCServerMsgStringSend("¹®Á¦ ¹ß»ý½Ã change@webzen.co.kr·Î ¹®ÀÇÇØ ÁÖ½Ã±â¹Ù¶ø´Ï´Ù", lpObj->m_Index, 1);

		return;
	}

	SDHP_CHARACTER_TRANSFER pCharTransfer;

	GJSetCharacterInfo(lpObj, lpObj->m_Index, FALSE);
	PHeadSetB((LPBYTE)&pCharTransfer, 0xCF, sizeof(pCharTransfer));
	pCharTransfer.Number = aIndex;
	memcpy(pCharTransfer.Account, lpObj->AccountID, sizeof(pCharTransfer.Account));

	cDBSMng.Send((PCHAR)&pCharTransfer, sizeof(pCharTransfer));

	LogAddTD("[CharTrasfer] Request Character Trasfer [%s][%s]",
		lpObj->AccountID, lpObj->Name);
}




void GCPacketCheckSumRecv(PMSG_PACKETCHECKSUM * aRecv, int aIndex)
{
	gPacketCheckSum.AddCheckSum(aIndex, aRecv->funcindex, aRecv->CheckSum);
}




void GCNPggSendCheckSum(int aIndex, _GG_AUTH_DATA * pggAuthData)
{
	PMSG_NPROTECTGGCHECKSUM pMsg;

	PHeadSetBE((LPBYTE)&pMsg, 0x73, sizeof(pMsg));
	memcpy(&pMsg.m_ggadCheckSum, pggAuthData, sizeof(pMsg.m_ggadCheckSum));

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}




void GCNPggCheckSumRecv(PMSG_NPROTECTGGCHECKSUM * lpMsg, int aIndex)
{
	/*LPOBJ lpObj = &gObj[aIndex];

	if ( !gObjIsConnected(aIndex))
		return;

	LogAddTD("[NPgg] Recv Checksum = %x,AuthValue = %x [%s][%s]",
		lpMsg->m_ggadCheckSum.dwIndex, lpObj->NPggCSAuth.m_AuthAnswer.dwIndex,
		lpObj->AccountID, lpObj->Name);

	memcpy(&gObj[aIndex].NPggCSAuth.m_AuthAnswer, &lpMsg->m_ggadCheckSum, sizeof(lpObj->NPggCSAuth.m_AuthAnswer));

	DWORD dwGGErrCode = gObj[aIndex].NPggCSAuth.CheckAuthAnswer();

	if ( dwGGErrCode )
	{
		LogAddTD("[NPgg] Invalid Checksum Detect Checksum = %x,AuthValue = %x [%s][%s]",
			lpMsg->m_ggadCheckSum.dwIndex, lpObj->NPggCSAuth.m_AuthAnswer.dwIndex,
			lpObj->AccountID, lpObj->Name);

		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 177)), aIndex, 0);
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 178)), aIndex, 0);
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 179)), aIndex, 0);

		CloseClient(aIndex);

		return;
	}

	lpObj->m_NPggCheckSumSendTime = GetTickCount();
	lpObj->m_bSentGGAuth = false;

	LogAddTD("[NPgg] Checksum Clear %x [%s][%s]",
		lpMsg->m_ggadCheckSum.dwIndex, lpObj->AccountID, lpObj->Name);
		*/
}



struct PMSG_REQ_DUEL_OK
{
	PBMSG_HEAD h;	// C1:AC
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char szName[10];	// 5
};


void CGDuelStartRequestRecv(PMSG_REQ_START_DUEL * lpMsg, int aIndex)
{
	int iDuelIndex = -1;
	char szTempText[256];

	if ( gObj[aIndex].CloseType != -1 )
		return;
	
	if ( gNonPK )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 174)), aIndex, 1);
		return;
	}

	if ( !gPkLimitFree )
	{
		if ( gObj[aIndex].m_PK_Level >= 6 )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 175)), aIndex, 1);
			return;
		}
	}

	if ( OBJMAX_RANGE(gObj[aIndex].m_iDuelUserReserved) )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 159)), aIndex, 1);
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 161)), aIndex, 1);
		return;
	}

	if ( OBJMAX_RANGE(gObj[aIndex].m_iDuelUser ) )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 160)), aIndex, 1);
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 161)), aIndex, 1);
		return;
	}

	if ( CC_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 207)), aIndex, 1);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if ( !PacketCheckTime(lpObj ) )
		return;

	iDuelIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if ( !OBJMAX_RANGE(iDuelIndex) )
	{
		LogAdd("error : %s %d (%d)", __FILE__, __LINE__, iDuelIndex);
		return;
	}

	if ( iDuelIndex == aIndex  )
		return;

	if ( !gPkLimitFree )
	{
		if ( gObj[iDuelIndex].m_PK_Level >= 6 )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 176)), aIndex, 1);
			return;
		}
	}

	char szDuelName[MAX_ACCOUNT_LEN+1]={0};
	char szDuelName2[MAX_ACCOUNT_LEN+1]={0};
	memcpy(szDuelName, gObj[iDuelIndex].Name, MAX_ACCOUNT_LEN);
	szDuelName[MAX_ACCOUNT_LEN] = 0;
	memcpy(szDuelName2, lpMsg->szName, MAX_ACCOUNT_LEN);
	szDuelName2[MAX_ACCOUNT_LEN] = 0;

	if ( strcmp(szDuelName, szDuelName2) )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 162)), aIndex, 1);
		return;
	}

	if ( IsDuelEnable(iDuelIndex) == FALSE )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 163)), aIndex, 1);
		return;
	}

	if ( lpObj->lpGuild && lpObj->lpGuild->WarState == 1 )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 164)), aIndex, 1);
		return;
	}

	if ( gObj[iDuelIndex].lpGuild && gObj[iDuelIndex].lpGuild->WarState == 1 )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 165)), aIndex, 1);
		return;
	}

	if ( gObjIsConnected(iDuelIndex) == FALSE )
		return;

	if ( gObj[iDuelIndex].Type == OBJ_MONSTER )
		return;

	if ( gObj[iDuelIndex].CloseCount >= 0 )
		return;

	BOOL bRetVal = FALSE;

	int n;
	for (n=0;n<MAX_SELF_DEFENSE;n++)
	{
		if ( lpObj->SelfDefense[n] >= 0 )
		{
			bRetVal = TRUE;
			break;
		}
	}

	if ( bRetVal )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 189)), aIndex, 1);
		return;
	}

	for (n=0;n<MAX_SELF_DEFENSE;n++)
	{
		if ( gObj[iDuelIndex].SelfDefense[n] >= 0 )
		{
			bRetVal = TRUE;
			break;
		}
	}

	if ( bRetVal )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 189)), aIndex, 1);
		return;
	}

	if ( DS_MAP_RANGE(gObj[iDuelIndex].MapNumber ) )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 180)), aIndex, 1);
		return;
	}

	if ( BC_MAP_RANGE(gObj[aIndex].MapNumber) )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 166)), aIndex, 1);
		return;
	}

	if ( lpObj->m_IfState.use > 0 )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 167)), aIndex, 1);
		return;
	}

	if ( gObj[iDuelIndex].m_IfState.use > 0 )
	{
		wsprintf(szTempText, lMsg.Get(MSGGET(4, 168)), gObj[iDuelIndex].Name);
		GCServerMsgStringSend(szTempText, aIndex, 1);
		return;
	}

	if ( OBJMAX_RANGE(gObj[iDuelIndex].m_iDuelUserRequested) )
	{
		wsprintf(szTempText, lMsg.Get(MSGGET(4, 169)), gObj[iDuelIndex].Name);
		GCServerMsgStringSend(szTempText, aIndex, 1);
		return;
	}

	if ( OBJMAX_RANGE(gObj[iDuelIndex].m_iDuelUserReserved) )
	{
		wsprintf(szTempText, lMsg.Get(MSGGET(4, 170)), gObj[iDuelIndex].Name);
		GCServerMsgStringSend(szTempText, aIndex, 1);
		return;
	}

	if ( OBJMAX_RANGE(gObj[iDuelIndex].m_iDuelUser) )
	{
		wsprintf(szTempText, lMsg.Get(MSGGET(4, 171)), gObj[iDuelIndex].Name);
		GCServerMsgStringSend(szTempText, aIndex, 1);
		return;
	}

	lpObj->m_iDuelUserReserved = iDuelIndex;
	lpObj->m_iDuelUser = -1;
	gObj[iDuelIndex].m_iDuelUserRequested = aIndex;

	PMSG_REQ_DUEL_OK pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xAC;
	pMsg.h.size = sizeof(pMsg);
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);
	memcpy(pMsg.szName, gObj[aIndex].Name, sizeof(pMsg.szName));

	DataSend(iDuelIndex, (LPBYTE)&pMsg, pMsg.h.size);

	wsprintf(szTempText, lMsg.Get(MSGGET(4, 172)), gObj[iDuelIndex].Name);
	GCServerMsgStringSend(szTempText, aIndex, 1);

	LogAddTD("[Duel] [%s][%s] Requested to Start Duel to [%s][%s]",
		lpObj->AccountID, lpObj->Name, gObj[iDuelIndex].AccountID, gObj[iDuelIndex].Name);
}

void CGDuelEndRequestRecv(PMSG_REQ_END_DUEL * lpMsg, int aIndex)
{
	int iDuelIndex = -1;

	if ( gObj[aIndex].CloseType != -1 ) 
		return;

	if ( OBJMAX_RANGE(gObj[aIndex].m_iDuelUserReserved) )
	{
		gObj[aIndex].m_iDuelUserReserved = -1;
	}

	if ( OBJMAX_RANGE(gObj[aIndex].m_iDuelUser) )
	{
		iDuelIndex = gObj[aIndex].m_iDuelUser;
		gObj[aIndex].m_iDuelUser = -1;

		PMSG_ANS_END_DUEL pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0xAA;
		pMsg.h.size = sizeof(pMsg);
		pMsg.h.subcode = 0x03;
		pMsg.bResult = 0;
		
		pMsg.NumberH = SET_NUMBERH(iDuelIndex);
		pMsg.NumberL = SET_NUMBERL(iDuelIndex);
		memcpy(pMsg.szName, gObj[iDuelIndex].Name, sizeof(pMsg.szName));

		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

		if ( iDuelIndex == aIndex )
		{
			return;
		}
	}
	else
	{
		LogAdd("error : %s %d (%d)", __FILE__, __LINE__, iDuelIndex);
		return;
	}

	if ( OBJMAX_RANGE(gObj[iDuelIndex].m_iDuelUser) )
	{
		if ( gObj[iDuelIndex].m_iDuelUser == aIndex )
		{
			gObj[iDuelIndex].m_iDuelUser = -1;

			PMSG_ANS_END_DUEL pMsg;

			pMsg.h.c = 0xC1;
			pMsg.h.headcode = 0xAA;
			pMsg.h.size = sizeof(pMsg);
			pMsg.h.subcode = 0x03;
			pMsg.bResult = 0;
			pMsg.NumberH = SET_NUMBERH(aIndex);
			pMsg.NumberL = SET_NUMBERL(aIndex);
			memcpy(pMsg.szName, gObj[aIndex].Name, sizeof(pMsg.szName));

			DataSend(iDuelIndex, (LPBYTE)&pMsg, pMsg.h.size);

			LogAddTD("[Duel] [%s][%s] Duel Ended [%s][%s]",
				gObj[aIndex].AccountID, gObj[aIndex].Name,
				gObj[iDuelIndex].AccountID, gObj[iDuelIndex].Name);
		}
	}
}


struct PMSG_ANS_START_DUEL
{
	PBMSG_HEAD h;	// C1:AA
	bool bDuelStart;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	char szName[10];	// 6
};



void CGDuelOkRequestRecv(PMSG_ANS_DUEL_OK * lpMsg, int aIndex)
{
	/*int iDuelIndex = -1;

	PMSG_ANS_START_DUEL pMsgSend;
	
	pMsgSend.h.c = 0xC1;
	pMsgSend.h.headcode = 0xAA;
	pMsgSend.h.size = sizeof(pMsgSend);

	if ( gObj[aIndex].CloseType != -1 ) 
		return;
	
	iDuelIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if ( OBJMAX_RANGE(iDuelIndex) )
	{
		if ( !gObjIsConnected(iDuelIndex) )
			return;

		if ( gObj[iDuelIndex].Type == OBJ_MONSTER )
			return;

		if ( gObj[iDuelIndex].CloseCount >= 0 )
			return;

		if ( lpMsg->bDuelOK )
		{
			if ( BC_MAP_RANGE(gObj[iDuelIndex].MapNumber) ||
				 CC_MAP_RANGE(gObj[iDuelIndex].MapNumber) ||
				 DS_MAP_RANGE(gObj[iDuelIndex].MapNumber) )
			{
				GCServerMsgStringSend(lMsg.Get(MSGGET(4, 207)), aIndex, 1);
				gObj[aIndex].m_iDuelUserRequested = -1;
				gObj[iDuelIndex].m_iDuelUserReserved = -1;
				memcpy(pMsgSend.szName, gObj[aIndex].Name, sizeof(pMsgSend.szName));
				pMsgSend.bDuelStart = false;

				DataSend(iDuelIndex, (LPBYTE)&pMsgSend, pMsgSend.h.size);
				return;
			}
		}

		if ( OBJMAX_RANGE(gObj[aIndex].m_iDuelUser ) )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 160)), aIndex, 1);
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 161)), aIndex, 1);
			gObj[aIndex].m_iDuelUserRequested = -1;
			gObj[iDuelIndex].m_iDuelUserReserved = -1;
			memcpy(pMsgSend.szName, gObj[aIndex].Name, sizeof(pMsgSend.szName));
			pMsgSend.bDuelStart = false;

			DataSend(iDuelIndex, (LPBYTE)&pMsgSend, pMsgSend.h.size);
			return;
		}

		if ( OBJMAX_RANGE(gObj[aIndex].m_iDuelUserReserved) )
		{
			GCServerMsgStringSend(lMsg.Get(MSGGET(4, 173)), aIndex, 1);
			gObj[aIndex].m_iDuelUserRequested = -1;
			gObj[iDuelIndex].m_iDuelUserReserved = -1;
			memcpy(pMsgSend.szName, gObj[aIndex].Name, sizeof(pMsgSend.szName));
			pMsgSend.bDuelStart = false;

			DataSend(iDuelIndex, (LPBYTE)&pMsgSend, pMsgSend.h.size);
			return;
		}

		if ( gObj[iDuelIndex].m_iDuelUserReserved == aIndex )
		{
			char szDuelName[MAX_ACCOUNT_LEN+1]={0};
			char szDuelName2[MAX_ACCOUNT_LEN+1]={0};
			memcpy(szDuelName, gObj[iDuelIndex].Name, MAX_ACCOUNT_LEN);
			szDuelName[MAX_ACCOUNT_LEN] = 0;
			memcpy(szDuelName2, lpMsg->szName, MAX_ACCOUNT_LEN);
			szDuelName2[MAX_ACCOUNT_LEN] = 0;

			if ( !strcmp(szDuelName, szDuelName2))
			{
				if ( lpMsg->bDuelOK == false)
				{
					gObj[aIndex].m_iDuelUserRequested = -1;
					gObj[iDuelIndex].m_iDuelUserReserved = -1;
					memcpy(pMsgSend.szName, gObj[aIndex].Name, sizeof(pMsgSend.szName));
					pMsgSend.bDuelStart = false;

					DataSend(iDuelIndex, (LPBYTE)&pMsgSend, pMsgSend.h.size);
				}
				else if ( lpMsg->bDuelOK == true )
				{
					gObj[iDuelIndex].m_iDuelUserReserved = -1;
					gObj[iDuelIndex].m_btDuelScore = 0;
					gObj[iDuelIndex].m_iDuelUser = aIndex;
					gObj[iDuelIndex].m_iDuelTickCount = GetTickCount();
					gObj[aIndex].m_iDuelUserRequested = -1;
					gObj[aIndex].m_iDuelUserReserved = -1;
					gObj[aIndex].m_btDuelScore = 0;
					gObj[aIndex].m_iDuelUser = iDuelIndex;
					gObj[aIndex].m_iDuelTickCount = GetTickCount();

					pMsgSend.bDuelStart = true;
					pMsgSend.NumberH = SET_NUMBERH(iDuelIndex);
					pMsgSend.NumberL = SET_NUMBERL(iDuelIndex);
					memcpy(pMsgSend.szName, szDuelName, sizeof(pMsgSend.szName));

					DataSend(aIndex, (LPBYTE)&pMsgSend, pMsgSend.h.size);

					pMsgSend.NumberH = SET_NUMBERH(aIndex);
					pMsgSend.NumberL = SET_NUMBERL(aIndex);
					memcpy(pMsgSend.szName, gObj[aIndex].Name, sizeof(pMsgSend.szName));

					DataSend(iDuelIndex, (LPBYTE)&pMsgSend, pMsgSend.h.size);

					GCSendDuelScore(gObj[aIndex].m_Index, gObj[iDuelIndex].m_Index);

					LogAddTD("[Duel] [%s][%s] Duel Started [%s][%s]", gObj[aIndex].AccountID, gObj[aIndex].Name,gObj[iDuelIndex].AccountID, gObj[iDuelIndex].Name);
				}
			}
			else
			{
				gObj[aIndex].m_iDuelUserRequested = -1;
				gObj[iDuelIndex].m_iDuelUserReserved = -1;
				pMsgSend.bDuelStart = false;

				DataSend(iDuelIndex, (LPBYTE)&pMsgSend, pMsgSend.h.size);
				return;
			}
		}
		else
		{
			gObj[aIndex].m_iDuelUserRequested = -1;
			gObj[iDuelIndex].m_iDuelUserReserved = -1;
			pMsgSend.bDuelStart = false;

			DataSend(iDuelIndex, (LPBYTE)&pMsgSend, pMsgSend.h.size);
			return;
		}
	}*/
}


/*void GCSendDuelScore(int aIndex1, int aIndex2)
{
	if ( !OBJMAX_RANGE(aIndex1) || !OBJMAX_RANGE(aIndex2))
		return;

	if ( !gObjIsConnected(aIndex1) || !gObjIsConnected(aIndex2) )
		return;

	if ( gObj[aIndex1].Type == OBJ_MONSTER || gObj[aIndex2].Type == OBJ_MONSTER )
		return;
	
	if ( gObj[aIndex1].CloseCount >= 0 || gObj[aIndex2].CloseCount >= 0 )
		return;

	PMSG_ANS_DUEL_SCORE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xAA;
	pMsg.h.subcode = 0x04;
	pMsg.h.size = sizeof(pMsg);
	pMsg.NumberH1 = SET_NUMBERH(aIndex1);
	pMsg.NumberL1 = SET_NUMBERL(aIndex1);
	pMsg.NumberH2 = SET_NUMBERH(aIndex2);
	pMsg.NumberL2 = SET_NUMBERL(aIndex2);
	pMsg.btDuelScore1 = gObj[aIndex1].m_btDuelScore;
	pMsg.btDuelScore2 = gObj[aIndex2].m_btDuelScore;

	DataSend(aIndex1, (LPBYTE)&pMsg, pMsg.h.size);
	DataSend(aIndex2, (LPBYTE)&pMsg, pMsg.h.size);

	if( gObj[aIndex1].m_iDuelRoom != -1 &&
		gObj[aIndex2].m_iDuelRoom != -1 &&
		gObj[aIndex1].m_iDuelRoom == gObj[aIndex2].m_iDuelRoom )
	{
		gDuelSystem.SendWatcherData(gObj[aIndex1].m_iDuelRoom,(LPBYTE)&pMsg,pMsg.h.size);
	}
}*/


struct PMSG_GETITEMINFO_FOR_PARTY
{
	PBMSG_HEAD h;	// C1:47
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	UINT ItemInfo;	// 8
	BYTE Level;	// C
};


void GCSendGetItemInfoForParty(int aIndex, CMapItem * lpItem)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj->PartyNumber >= 0)
	{
		PMSG_GETITEMINFO_FOR_PARTY pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x47;
		pMsg.h.size = sizeof(pMsg);
		pMsg.NumberH = SET_NUMBERH(aIndex);
		pMsg.NumberL = SET_NUMBERL(aIndex);
		pMsg.ItemInfo = lpItem->m_Type & (ITEMGET(MAX_TYPE_ITEMS-1, MAX_SUBTYPE_ITEMS-1));

		if ( lpItem->m_Option1 )
			pMsg.ItemInfo |= 0x2000;

		if ( lpItem->m_Option2 )
			pMsg.ItemInfo |= 0x4000;

		if ( lpItem->m_Option3 )
			pMsg.ItemInfo |= 0x8000;

		if ( lpItem->m_NewOption )
			pMsg.ItemInfo |= 0x10000;

		if ( lpItem->m_SetOption )
			pMsg.ItemInfo |= 0x20000;

		if ( g_kItemSystemFor380.Is380OptionItem( (lpItem != NULL)?((CItem *)&lpItem->m_Number):NULL) == TRUE )
			pMsg.ItemInfo |= 0x40000;

		pMsg.Level = lpItem->m_Level;
		
		int partycount = gParty.GetPartyCount(lpObj->PartyNumber);

		if ( partycount >= 0 )
		{
			for ( int n=0;n<MAX_USER_IN_PARTY;n++)
			{
				int number = gParty.m_PartyS[lpObj->PartyNumber].Number[n];

				if ( number >= 0 )
				{
					DataSend(number, (LPBYTE)&pMsg, pMsg.h.size);
				}
			}
		}
	}
}



struct PMSG_EFFECTINFO
{
	PBMSG_HEAD h;	// C1:48
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE btType;	// 5
};


void GCSendEffectInfo(int aIndex, BYTE btType)
{
	if ( OBJMAX_RANGE(aIndex) == FALSE )
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	PMSG_EFFECTINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x48;
	pMsg.h.size = sizeof(pMsg);
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);
	pMsg.btType = btType;

	if ( btType == 17 )
	{
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
	}

	MsgSendV2(lpObj, (LPBYTE)&pMsg, pMsg.h.size);
}




void CGRequestPetItemCommand(PMSG_REQUEST_PET_ITEM_COMMAND * lpMsg, int aIndex)
{
	if ( !gObjIsConnectedGP(aIndex))
	{
		//LogAddTD("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if ( !OBJMAX_RANGE(aIndex))
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpMsg->PetType )
		return;

	int iTargetIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	gDarkSpirit[aIndex].ChangeCommand(lpMsg->Command, iTargetIndex);
}




void CGRequestPetItemInfo(PMSG_REQUEST_PET_ITEMINFO * lpMsg, int aIndex)
{
	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddTD("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if ( !OBJMAX_RANGE(aIndex))
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpMsg->PetType != 0 && lpMsg->PetType != 1 )
		return;
	
	PMSG_SEND_PET_ITEMINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xA9, sizeof(pMsg));
	pMsg.PetType = lpMsg->PetType;
	pMsg.InvenType = lpMsg->InvenType;
	pMsg.nPos = lpMsg->nPos;

	if ( lpMsg->InvenType == 0 )	// Inventory
	{
		if ( !INVENTORY_RANGE(lpMsg->nPos))
		{
			LogAddTD("[PetItem] [%s][%s] RequestPetItem Invalid Iventory nPos %d",
				lpObj->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if ( lpObj->pInventory[lpMsg->nPos].IsItem() )
		{
			pMsg.Exp = lpObj->pInventory[lpMsg->nPos].m_PetItem_Exp;
			pMsg.Level = lpObj->pInventory[lpMsg->nPos].m_PetItem_Level;
			pMsg.Life = (BYTE)(lpObj->pInventory[lpMsg->nPos].m_Durability);

			DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
	else if ( lpMsg->InvenType == 1 )	// Warehouse
	{
		if ( lpObj->LoadWareHouseInfo != false )
		{
			if ( !WAREHOUSE_RANGE(lpMsg->nPos))
			{
				LogAddTD("[PetItem] [%s][%s] RequestPetItem Invalid WareHouse nPos %d",
					lpObj->AccountID, lpObj->Name, lpMsg->nPos);

				return;
			}

			if ( lpObj->pWarehouse[lpMsg->nPos].IsItem() )
			{
				pMsg.Exp = lpObj->pWarehouse[lpMsg->nPos].m_PetItem_Exp;
				pMsg.Level = lpObj->pWarehouse[lpMsg->nPos].m_PetItem_Level;
				pMsg.Life = (BYTE)lpObj->pWarehouse[lpMsg->nPos].m_Durability;

				DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
			}
		}
	}
	else if ( lpMsg->InvenType == 2 )	// Trade
	{
		if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 1 )
			return;

		if ( !TRADE_BOX_RANGE(lpMsg->nPos))
		{
			LogAddTD("[PetItem] [%s][%s] RequestPetItem Invalid Trade nPos %d",
				lpObj->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if ( lpObj->Trade[lpMsg->nPos].IsItem() )
		{
			pMsg.Exp = lpObj->Trade[lpMsg->nPos].m_PetItem_Exp;
			pMsg.Level = lpObj->Trade[lpMsg->nPos].m_PetItem_Level;
			pMsg.Life = (BYTE)lpObj->Trade[lpMsg->nPos].m_Durability;

			DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
	else if ( lpMsg->InvenType == 3 )	// Target Trade
	{
		if ( lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 1 )
			return;

		int iTargetIndex = gObj[aIndex].TargetNumber;

		if ( iTargetIndex < 0 )
			return;

		if ( !OBJMAX_RANGE(iTargetIndex) )
			return;

		if ( gObj[iTargetIndex].m_IfState.use < 1 || gObj[iTargetIndex].m_IfState.type != 1 )
			return;

		if ( !TRADE_BOX_RANGE(lpMsg->nPos))
		{
			LogAddTD("[PetItem] [%s][%s] RequestPetItem Invalid TargetTrade nPos %d",
				lpObj->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if ( gObj[iTargetIndex].Trade[lpMsg->nPos].IsItem() )
		{
			pMsg.Exp = gObj[iTargetIndex].Trade[lpMsg->nPos].m_PetItem_Exp;
			pMsg.Level = gObj[iTargetIndex].Trade[lpMsg->nPos].m_PetItem_Level;
			pMsg.Life = (BYTE)gObj[iTargetIndex].Trade[lpMsg->nPos].m_Durability;

			DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
	else if ( lpMsg->InvenType == 4 )	// Chaos
	{
		if ( !CHAOS_BOX_RANGE(lpMsg->nPos) )
		{
			LogAddTD("[PetItem] [%s][%s] RequestPetItem Invalid Chaos nPos %d",
				lpObj->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if ( lpObj->pChaosBox[lpMsg->nPos].IsItem() )
		{
			pMsg.Exp = lpObj->pChaosBox[lpMsg->nPos].m_PetItem_Exp;
			pMsg.Level = lpObj->pChaosBox[lpMsg->nPos].m_PetItem_Level;
			pMsg.Life = (BYTE)lpObj->pChaosBox[lpMsg->nPos].m_Durability;

			DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
	else if ( lpMsg->InvenType == 5 )	// Personal Shop
	{
		if ( !INVENTORY_RANGE(lpMsg->nPos) )
		{
			LogAddTD("[PetItem] [%s][%s] RequestPetItem Invalid Iventory nPos %d",
				lpObj->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if ( lpObj->m_iPShopDealerIndex != -1 )
		{
			if ( OBJMAX_RANGE(lpObj->m_iPShopDealerIndex))
			{
				if ( gObj[lpObj->m_iPShopDealerIndex].pInventory[lpMsg->nPos].IsItem() )
				{
					pMsg.Exp = gObj[lpObj->m_iPShopDealerIndex].pInventory[lpMsg->nPos].m_PetItem_Exp;
					pMsg.Level = gObj[lpObj->m_iPShopDealerIndex].pInventory[lpMsg->nPos].m_PetItem_Level;
					pMsg.Life = (BYTE)gObj[lpObj->m_iPShopDealerIndex].pInventory[lpMsg->nPos].m_Durability;

					DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
				}
			}
		}
	}	
}







struct PMSG_ANS_GUILDVIEWPORT
{
	PBMSG_HEAD h;	// C1:66
	int GuildNumber;	// 4
	BYTE btGuildType;	// 8
	BYTE UnionName[8];	// 9
	char GuildName[8];	// 11
	BYTE Mark[32];	// 19
};


void GCGuildViewportInfo(PMSG_REQ_GUILDVIEWPORT * aRecv, int aIndex)
{
	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddTD("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if ( !OBJMAX_RANGE(aIndex))
		return;

	LPOBJ lpObj = &gObj[aIndex];
	DWORD dwGuildNumber = aRecv->GuildNumber;
	_GUILD_INFO_STRUCT * lpGuildInfo = Guild.SearchGuild_Number(dwGuildNumber);

	if ( lpGuildInfo )
	{
		PMSG_ANS_GUILDVIEWPORT pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x66;
		pMsg.h.size = sizeof(pMsg);
		pMsg.GuildNumber = lpGuildInfo->Number;
		pMsg.btGuildType = lpGuildInfo->btGuildType;

		TUnionInfo * pUnionInfo = UnionManager.SearchUnion(lpGuildInfo->iGuildUnion);

		if ( pUnionInfo )
			memcpy(pMsg.UnionName, pUnionInfo->m_szMasterGuild, MAX_GUILD_LEN);
		else
			pMsg.UnionName[0] = '\0';

		strcpy(pMsg.GuildName, lpGuildInfo->Name);
		memcpy(pMsg.Mark, lpGuildInfo->Mark, sizeof(pMsg.Mark));

		DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
	}
	else
	{
		LogAddTD("¡Ú¡Ú¡Ù ±æµå Á¤º¸ Ã£À»¼ö ¾øÀ½. ÀÌ¸§ : [%s] ¹øÈ£ : %d",
			lpObj->Name, dwGuildNumber);
	}
}







void CGGuildAssignStatus(PMSG_GUILD_ASSIGN_STATUS_REQ * aRecv, int aIndex)
{
	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddTD("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if ( !OBJMAX_RANGE(aIndex))
		return;

	LPOBJ lpObj = &gObj[aIndex];
	PMSG_GUILD_ASSIGN_STATUS_RESULT pMsg={0};
	
	pMsg.h.set((LPBYTE)&pMsg, 0xE1, sizeof(pMsg));
	pMsg.btType = aRecv->btType;
	memcpy(pMsg.szTagetName, aRecv->szTagetName, sizeof(pMsg.szTagetName));

#if (ENABLETEST_ARCA == 1)
	int iArcaBattleState = g_ArcaBattle.GetState();
	if( iArcaBattleState > AB_STATE_CLOSED && iArcaBattleState < AB_STATE_CLOSE )
	{
		pMsg.btResult = 33;	//Define me
        DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}
#endif

	if ( lpObj->GuildNumber <= 0 || lpObj->lpGuild == NULL )
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_GUILD;
		DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	if ( gObj[aIndex].GuildStatus != G_MASTER )
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	char szTargetName[MAX_ACCOUNT_LEN+1]={0};
	memcpy(szTargetName, aRecv->szTagetName, MAX_ACCOUNT_LEN);

	if ( !strcmp(gObj[aIndex].Name, szTargetName))
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	if ( lpObj->lpGuild == NULL )
		return;

	if ( aRecv->btType == 1 || aRecv->btType == 2 )
	{
		int iSubMasterCount = 0;
		int iBattleMasterCount = 0;

		for ( int n=0;n<MAX_USER_GUILD;n++)
		{
			if ( lpObj->lpGuild->Use[n] > 0 )
			{
				if ( lpObj->lpGuild->GuildStatus[n] == G_SUB_MASTER )
				{
					iSubMasterCount++;
				}
				else if ( lpObj->lpGuild->GuildStatus[n] == G_BATTLE_MASTER )
				{
					iBattleMasterCount++;
				}
			}
		}

		if ( aRecv->btGuildStatus == G_SUB_MASTER )
		{
			if ( iSubMasterCount == 0 )
			{
				GDGuildReqAssignStatus(aIndex, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
				return;
			}

			pMsg.btResult = GUILD_ANS_NOTEXIST_EXTRA_STATUS;
			DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
			return;
		}
		else if ( aRecv->btGuildStatus == G_BATTLE_MASTER )
		{
			if ( iBattleMasterCount < ((lpObj->Level / 200 ) +1) )
			{
				GDGuildReqAssignStatus(aIndex, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
				return;
			}

			pMsg.btResult = GUILD_ANS_NOTEXIST_EXTRA_STATUS;
			DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
			return;
		}
		else if ( aRecv->btGuildStatus == G_PERSON )
		{
			GDGuildReqAssignStatus(aIndex, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
			return;
		}
		else
		{
			return;
		}
	}
	
	if ( aRecv->btType == 3 )
	{
		GDGuildReqAssignStatus(aIndex, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
	}
}




void CGGuildAssignType(PMSG_GUILD_ASSIGN_TYPE_REQ * aRecv, int aIndex)
{
	if ( !gObjIsConnectedGP(aIndex))
	{
		LogAddTD("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if ( !OBJMAX_RANGE(aIndex))
		return;

	LPOBJ lpObj = &gObj[aIndex];
	PMSG_GUILD_ASSIGN_TYPE_RESULT pMsg={0};
	
	pMsg.h.set((LPBYTE)&pMsg, 0xE2, sizeof(pMsg));
	pMsg.btGuildType = aRecv->btGuildType;
	
	if ( lpObj->GuildNumber <= 0 || lpObj->lpGuild == NULL )
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_GUILD;
		DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	if ( gObj[aIndex].GuildStatus != G_MASTER )
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}
	
	GDGuildReqAssignType(aIndex, aRecv->btGuildType);
}




void CGRelationShipReqJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ * aRecv, int aIndex)
{
	if ( g_CastleSiegeSync.GetCastleState() >= 5 && g_CastleSiegeSync.GetCastleState() <= 7 )
	{
		MsgOutput(aIndex, lMsg.Get(MSGGET(6, 196)));
		return;
	}

#if (ENABLETEST_ARCA == 1)
	int iArcaBattleState = g_ArcaBattle.GetState();

	if( iArcaBattleState > AB_STATE_CLOSED && iArcaBattleState < AB_STATE_CLOSE )
	{
#if( AB_DEBUG_TRACE == 1 )
		TRACE_LOG("aIndex, 0x51, 0xA5");
#endif
		GCResultSend(aIndex, 0x51, 0xA4);
		return;
	}
#endif

	int iTargetUserIndex = -1;
	
	/*if( aRecv->btRelationShipType == 2 && aRecv->btRequestType == 2 )
	{
		char szGuild[9]={0};
		memcpy(szGuild,aRecv->szGuild,8);

		_GUILD_INFO_STRUCT* lpGuild = Guild.SearchGuild(szGuild);

		if( lpGuild )
		{
			iTargetUserIndex = lpGuild->Index[0];
		}
	}
	else
	{*/
		iTargetUserIndex = MAKE_NUMBERW(aRecv->btTargetUserIndexH, aRecv->btTargetUserIndexL);
	//}

	if ( !OBJMAX_RANGE(aIndex) )
		return;

	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	if ( !OBJMAX_RANGE(iTargetUserIndex) || gObjIsConnected(&gObj[iTargetUserIndex]) == FALSE )
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

#if (ENABLETEST_NEWPVP == 1)
	if( g_NewPVP.IsDuel(gObj[aIndex]) || g_NewPVP.IsDuel(gObj[iTargetUserIndex]) )
	{
		 GCServerMsgStringSend(lMsg.Get(3431), aIndex, 1);
		 return;
	}
	// ----
	if( g_NewPVP.IsObserver(gObj[aIndex]) || g_NewPVP.IsObserver(gObj[iTargetUserIndex]) )
	{
		 GCServerMsgStringSend(lMsg.Get(3432), aIndex, 1);
		 return;
	}
#endif

	if ( !gObj[aIndex].lpGuild || !gObj[iTargetUserIndex].lpGuild )
	{
		GCResultSend(aIndex, 0x51, 5);
		return;
	}

	if ( gObj[aIndex].GuildStatus != G_MASTER && gObj[iTargetUserIndex].GuildStatus != G_MASTER)
	{
		GCResultSend(aIndex, 0x51, 4);
		return;
	}

	if ( gObj[aIndex].m_IfState.use > 0 )
	{
		GCResultSend(aIndex, 0x51, 6);
		return;
	}

	if ( gObj[iTargetUserIndex].m_IfState.use > 0 )
	{
		GCResultSend(aIndex, 0x51, 6);
		return;
	}

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg={0};

	PHeadSetB((LPBYTE)&ErrMsg, 0xE6, sizeof(ErrMsg));
	ErrMsg.btResult = 0;
	ErrMsg.btRequestType = aRecv->btRequestType;
	ErrMsg.btRelationShipType = aRecv->btRelationShipType;
	ErrMsg.btTargetUserIndexH = aRecv->btTargetUserIndexH;
	ErrMsg.btTargetUserIndexL = aRecv->btTargetUserIndexL;

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTargetObj = &gObj[iTargetUserIndex];

	if ( lpObj->GuildStatus != G_MASTER || lpTargetObj->GuildStatus != G_MASTER)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
		return;
	}

#if (__GENS_SYSTEM__ == 1)

	if( gGensSystem.m_CheckForAlliance )
	{
		if( aRecv->btRelationShipType != 2 )
		{
			if ( lpTargetObj->m_GensInfo.btFamily == GENS_NONE )
			{
				ErrMsg.btResult = GUILD_ANS_ALLYMASTER_NOGENS;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if ( lpObj->m_GensInfo.btFamily != lpTargetObj->m_GensInfo.btFamily )
			{
				ErrMsg.btResult = GUILD_ANS_DIFFERENT_GENS;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
	}
#endif

	_GUILD_INFO_STRUCT * lpGuildInfo = gObj[aIndex].lpGuild;
	_GUILD_INFO_STRUCT * lpTargetGuildInfo = gObj[iTargetUserIndex].lpGuild;
	BYTE btRelationShip = gObjGetRelationShip(lpObj, lpTargetObj);

	if ( aRecv->btRequestType == 1 )
	{
		if ( aRecv->btRelationShipType == 1 )
		{
			if ( lpTargetGuildInfo->Count < g_CastleMinGuildMember )
			{
				ErrMsg.btResult = GUILD_ANS_CANNOT_BE_UNION_MASTER;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if ( lpTargetGuildInfo->iGuildUnion != 0 && lpTargetGuildInfo->iGuildUnion == lpTargetGuildInfo->Number )
			{
				if ( UnionManager.GetGuildRelationShipCount(lpTargetGuildInfo->iGuildUnion, 1) >= g_GuildMaxAllianceMember  )
				{
					ErrMsg.btResult = GUILD_ANS_EXCEED_MAX_UNION_MEMBER;
					DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
					return;
				}
			}
		
			char szCastleOwnerGuildName[MAX_GUILD_LEN+1]={0};
			memcpy(szCastleOwnerGuildName, g_CastleSiegeSync.GetCastleOwnerGuild(), MAX_GUILD_LEN);

			if ( !strcmp(lpGuildInfo->Name, szCastleOwnerGuildName))
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_UNION;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if ( lpGuildInfo->iGuildUnion )
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_UNION;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if ( btRelationShip == 2 )
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RELATIONSHIP_RIVAL;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if ( lpTargetGuildInfo->iGuildUnion && lpTargetGuildInfo->Number != lpTargetGuildInfo->iGuildUnion )
			{
				ErrMsg.btResult = GUIDL_ANS_NOT_UNION_MASTER;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
		else if ( aRecv->btRelationShipType == 2 )
		{
			if ( lpGuildInfo->iGuildRival || lpTargetGuildInfo->iGuildRival )
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RIVAL;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if ( btRelationShip == 1 )
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RELATIONSHIP_UNION;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
	}
	else if ( aRecv->btRequestType == 2 )
	{
		if ( aRecv->btRelationShipType == 1 )
		{
			if ( lpGuildInfo->iGuildUnion == 0 )
			{
				ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
		else if ( aRecv->btRelationShipType == 2 )
		{
			if ( !lpGuildInfo->iGuildRival || !lpTargetGuildInfo->iGuildRival )
			{
				ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if ( lpGuildInfo->iGuildRival != lpTargetGuildInfo->Number || lpGuildInfo->Number != lpTargetGuildInfo->iGuildRival )
			{
				ErrMsg.btResult = GUILD_ANS_NOT_GUILD_RIVAL;
				DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
	}		
	
	if ( aRecv->btRequestType == 2 )
	{
		if ( aRecv->btRelationShipType == 1 )
		{
			GDUnionBreakOff(aIndex, lpGuildInfo->iGuildUnion);
			return;
		}
	}

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 14;
	lpObj->m_IfState.state = 0;
	lpObj->m_InterfaceTime = GetTickCount();

	lpTargetObj->m_IfState.use = 1;
	lpTargetObj->m_IfState.type = 14;
	lpTargetObj->m_IfState.state = 0;
	lpTargetObj->m_InterfaceTime = GetTickCount();

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ pMsg={0};

	PHeadSetB((LPBYTE)&pMsg, 0xE5, sizeof(pMsg));
	pMsg.btTargetUserIndexL = SET_NUMBERL(aIndex);
	pMsg.btTargetUserIndexH = SET_NUMBERH(aIndex);
	pMsg.btRequestType = aRecv->btRequestType;
	pMsg.btRelationShipType = aRecv->btRelationShipType;
	
	DataSend(iTargetUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
}




void CGRelationShipAnsJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS * aRecv, int aIndex)
{
	int iTargetUserIndex = MAKE_NUMBERW(aRecv->btTargetUserIndexH, aRecv->btTargetUserIndexL);

	if ( !OBJMAX_RANGE(aIndex) || !OBJMAX_RANGE(iTargetUserIndex))
		return;

	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	if ( gObjIsConnected(&gObj[iTargetUserIndex]) == FALSE )
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

	if ( aRecv->btResult == 1 )
	{
		if ( aRecv->btRequestType == 1 )
		{
			GDRelationShipReqJoin(iTargetUserIndex, aIndex, aRecv->btRelationShipType);
		}
		else if ( aRecv->btRequestType == 2 )
		{
			GDRelationShipReqBreakOff(aIndex, iTargetUserIndex, aRecv->btRelationShipType);
		}
	}
	else
	{
		PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg ={0};

		PHeadSetB((LPBYTE)&ErrMsg, 0xE6, sizeof(ErrMsg));
		ErrMsg.btResult = 0;
		ErrMsg.btRequestType = aRecv->btRequestType;
		ErrMsg.btRelationShipType = aRecv->btRelationShipType;
		ErrMsg.btTargetUserIndexH = aRecv->btTargetUserIndexH;
		ErrMsg.btTargetUserIndexL = aRecv->btTargetUserIndexL;
		ErrMsg.btResult = GUILD_ANS_CANCLE_REQUEST;

		DataSend(iTargetUserIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
	}

	if ( gObj[aIndex].m_IfState.use )
	{
		if ( gObj[aIndex].m_IfState.type == 14 )
		{
			gObj[aIndex].m_IfState.use = 0;
		}
	}

	if ( gObj[iTargetUserIndex].m_IfState.use )
	{
		if ( gObj[iTargetUserIndex].m_IfState.type == 14 )
		{
			gObj[iTargetUserIndex].m_IfState.use = 0;
		}
	}
}




void CGUnionList(PMSG_UNIONLIST_REQ * aRecv, int aIndex)
{
	if ( !OBJMAX_RANGE(aIndex))
		return;

	if ( !PacketCheckTime(&gObj[aIndex]))
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj == NULL )
	{
		return;
	}

	if ( gObjIsConnected(&gObj[aIndex]) == FALSE )
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

	if ( lpObj->lpGuild == NULL )
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

	if ( lpObj->lpGuild->iGuildUnion == 0 )
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

	GDUnionListSend(aIndex, lpObj->lpGuild->iGuildUnion);

}



void CGRelationShipReqKickOutUnionMember(PMSG_KICKOUT_UNIONMEMBER_REQ* aRecv, int aIndex)
{
	if ( !OBJMAX_RANGE(aIndex))
		return;

	if ( ::PacketCheckTime(&gObj[aIndex]) == FALSE )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if ( lpObj == NULL )
		return;

	if ( gObjIsConnected(&gObj[aIndex]) == FALSE )
	{
		GCResultSend(aIndex, 0x51, 3);
		MsgOutput(aIndex, "¡Ú Terminated User.");
		return;
	}

	if ( lpObj->lpGuild == NULL )
	{
		GCResultSend(aIndex, 0x51, 3);
		MsgOutput(aIndex, "¡Ù Terminated Guild.");
		return;
	}

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = {0};

	PHeadSetB((LPBYTE)&ErrMsg, 0xE6, sizeof(ErrMsg));
	ErrMsg.btResult = 0;
	ErrMsg.btRequestType = 2;
	ErrMsg.btRelationShipType = 1;
	ErrMsg.btTargetUserIndexH = 0;
	ErrMsg.btTargetUserIndexL = 0;

#if (ENABLETEST_ARCA == 1)
	int iArcaBattleState = g_ArcaBattle.GetState();

	if( iArcaBattleState > AB_STATE_CLOSED && iArcaBattleState < AB_STATE_CLOSE )
	{
		ErrMsg.btResult = 33;
		DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
		return;
	}
#endif

	if ( lpObj->lpGuild->iGuildUnion == 0 )
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
		DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);

		return;
	}

	if ( lpObj->GuildStatus != GUILD_MASTER || lpObj->lpGuild->Number != lpObj->lpGuild->iGuildUnion)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);

		return;
	}

	char szUnionMemberGuildName[MAX_GUILD_LEN+1] = {0};
	memcpy(szUnionMemberGuildName, aRecv->szTargetGuildName, MAX_GUILD_LEN);

	if ( !strcmp(lpObj->lpGuild->Name, szUnionMemberGuildName))
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		DataSend(aIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);

		return;
	}

	GDRelationShipReqKickOutUnionMember(aIndex, szUnionMemberGuildName);
}




void CGReqMapSvrAuth(PMSG_REQ_MAPSERVERAUTH * lpMsg, int iIndex)
{
	if ( lpMsg->btCliVersion[0] != szClientVersion[0] ||
		 lpMsg->btCliVersion[1] != szClientVersion[1] ||
		 lpMsg->btCliVersion[2] != szClientVersion[2] ||
		 lpMsg->btCliVersion[3] != szClientVersion[3] ||
		 lpMsg->btCliVersion[4] != szClientVersion[4] )
	{
		GCAnsMapSvrAuth(iIndex, 6);
		CloseClient(iIndex);

		return;
	}

	BYTE btSerial[17];

	btSerial[16]=0;
	memcpy(btSerial, lpMsg->btCliSerial, sizeof(lpMsg->btCliSerial));
	char id[MAX_ACCOUNT_LEN+1];
	id[MAX_ACCOUNT_LEN] = 0;
	memcpy(id, lpMsg->szAccountID, MAX_ACCOUNT_LEN);
	BuxConvert(id, MAX_ACCOUNT_LEN);

	if ( strcmp((char*)btSerial, szGameServerExeSerial) )
	{
		LogAddC(2, "error-L1: Serial error [%s] [%s]", id, btSerial);
		GCAnsMapSvrAuth(iIndex, 6);
		CloseClient(iIndex);
		return;
	}

	if ( g_ConnectMemberLoad == TRUE )
	{
		if ( ConMember.IsMember(id) == FALSE )
		{
			GCAnsMapSvrAuth(iIndex, 2);	
			return;
		}
	}

	LPOBJ lpObj = &gObj[iIndex];

	if ( PacketCheckTime(lpObj) == FALSE )
	{
		LogAdd(lMsg.Get(MSGGET(1, 217)), iIndex, id);
		CloseClient(iIndex);

		return;
	}

	if ( gObj[iIndex].Connected != PLAYER_CONNECTED )
	{
		LogAdd(lMsg.Get(MSGGET(1, 218)), iIndex, id);
		CloseClient(iIndex);

		return;
	}

	if ( gObj[iIndex].LoginMsgSnd != 0 )
	{
		if ( gObj[iIndex].LoginMsgSnd == 1 )
		{
			LogAdd(lMsg.Get(MSGGET(1, 219)), iIndex, id, gObj[iIndex].Ip_addr);
		}
		else
		{
			LogAdd("error : %d %s %d", gObj[iIndex].LoginMsgSnd, __FILE__, __LINE__);
		}

		return;
	}

	gObj[iIndex].ConnectCheckTime = GetTickCount(); 

	gObj[iIndex].LoginMsgSnd = 1;
	gObj[iIndex].LoginMsgCount++;
	gObj[iIndex].AccountID[MAX_ACCOUNT_LEN] = 0;
	memcpy(gObj[iIndex].AccountID, id, MAX_ACCOUNT_LEN);
	memset(gObj[iIndex].Name, 0, MAX_ACCOUNT_LEN);
	gObj[iIndex].m_cAccountItemBlock = 0;
	GJReqMapSvrAuth(iIndex, gObj[iIndex].AccountID, lpMsg->szCharName,
		lpMsg->iJoinAuthCode1, lpMsg->iJoinAuthCode2,
		lpMsg->iJoinAuthCode3, lpMsg->iJoinAuthCode4);

	LogAddTD("[MapServerMng] Map Server Join Send : [%s][%s](%d)",
		gObj[iIndex].AccountID, gObj[iIndex].Name, iIndex);


}


struct PMSG_ANS_MAPSERVERAUTH
{
	PBMSG_HEAD2 h;	// C1:B1:01
	BYTE iResult;	// 4
};


void GCAnsMapSvrAuth(int iIndex, int iResult)
{
	if ( !OBJMAX_RANGE(iIndex) )
	{
		LogAddC(2,"[MapServerMng] Packet Error JG [0x7B] - Index out of bound : %d",iIndex);
		return;
	}

	PMSG_ANS_MAPSERVERAUTH pMsgResult;

	PHeadSubSetB((LPBYTE)&pMsgResult, 0xB1, 0x01, sizeof(pMsgResult));
	pMsgResult.iResult = iResult;

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}



void CGReqCastleSiegeState(PMSG_REQ_CASTLESIEGESTATE * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	GS_GDReqOwnerGuildMaster(g_MapServerManager.GetMapSvrGroup(),iIndex);
#endif
}

struct PMSG_ANS_CASTLESIEGESTATE
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	char cCastleSiegeState;
	unsigned char btStartYearH;
	unsigned char btStartYearL;
	unsigned char btStartMonth;
	unsigned char btStartDay;
	unsigned char btStartHour;
	unsigned char btStartMinute;
	unsigned char btEndYearH;
	unsigned char btEndYearL;
	unsigned char btEndMonth;
	unsigned char btEndDay;
	unsigned char btEndHour;
	unsigned char btEndMinute;
	unsigned char btSiegeStartYearH;
	unsigned char btSiegeStartYearL;
	unsigned char btSiegeStartMonth;
	unsigned char btSiegeStartDay;
	unsigned char btSiegeStartHour;
	unsigned char btSiegeStartMinute;
	char cOwnerGuild[0x8];
	char cOwnerGuildMaster[0xA];
	unsigned char btCurRemainTime[4];
};

void GCAnsCastleSiegeState(int iIndex, int iResult, LPSTR lpszGuildName, LPSTR lpszGuildMaster)
{
#if (GS_CASTLE==1)

	if(lpszGuildName == NULL || lpszGuildMaster == NULL)
		return;

	PMSG_ANS_CASTLESIEGESTATE pMsgResult;

	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x00,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;

	memcpy(pMsgResult.cOwnerGuild,lpszGuildName,sizeof(pMsgResult.cOwnerGuild));
	memcpy(pMsgResult.cOwnerGuildMaster,lpszGuildMaster,sizeof(pMsgResult.cOwnerGuildMaster));

	_SYSTEMTIME tmStateStartDate = {0};
	_SYSTEMTIME tmStateEndDate = {0};
	_SYSTEMTIME tmSiegeStartDate = {0};

	int iCastleState = g_CastleSiege.GetCastleState();

	int bRETVAL = g_CastleSiege.GetCastleStateTerm(&tmStateStartDate,&tmStateEndDate);

	tmSiegeStartDate = g_CastleSiege.GetCastleLeftSiegeDate();

	if(bRETVAL == 0)
		iCastleState = -1;

	int iRemainTime = g_CastleSiege.GetCurRemainSec();
	pMsgResult.cCastleSiegeState = iCastleState;

	pMsgResult.btStartYearH = SET_NUMBERH(tmStateStartDate.wYear);
	pMsgResult.btStartYearL = SET_NUMBERL(tmStateStartDate.wYear);
	pMsgResult.btStartMonth = (BYTE)tmStateStartDate.wMonth;
	pMsgResult.btStartDay = (BYTE)tmStateStartDate.wDay;
	pMsgResult.btStartHour = (BYTE)tmStateStartDate.wHour;
	pMsgResult.btStartMinute = (BYTE)tmStateStartDate.wMinute;

	pMsgResult.btEndYearH = SET_NUMBERH(tmStateEndDate.wYear);
	pMsgResult.btEndYearL = SET_NUMBERL(tmStateEndDate.wYear);
	pMsgResult.btEndMonth = (BYTE)tmStateEndDate.wMonth;
	pMsgResult.btEndDay = (BYTE)tmStateEndDate.wDay;
	pMsgResult.btEndHour = (BYTE)tmStateEndDate.wHour;
	pMsgResult.btEndMinute = (BYTE)tmStateEndDate.wMinute;

	pMsgResult.btSiegeStartYearH = SET_NUMBERH(tmSiegeStartDate.wYear);
	pMsgResult.btSiegeStartYearL = SET_NUMBERL(tmSiegeStartDate.wYear);
	pMsgResult.btSiegeStartMonth = (BYTE)tmSiegeStartDate.wMonth;
	pMsgResult.btSiegeStartDay = (BYTE)tmSiegeStartDate.wDay;
	pMsgResult.btSiegeStartHour = (BYTE)tmSiegeStartDate.wHour;
	pMsgResult.btSiegeStartMinute = (BYTE)tmSiegeStartDate.wMinute;

	pMsgResult.btCurRemainTime[0] = ((iRemainTime << 16) & 0xFFFF) << 8;
	pMsgResult.btCurRemainTime[1] = ((iRemainTime << 16) & 0xFFFF) & 0xFF;
	pMsgResult.btCurRemainTime[2] = ((iRemainTime & 0xFFFF) & 0xFFFF) << 8;
	pMsgResult.btCurRemainTime[3] = ((iRemainTime & 0xFFFF) & 0xFFFF) & 0xFF;

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}




void CGReqRegCastleSiege(PMSG_REQ_REGCASTLESIEGE * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)

	if(g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGSIEGE)
	{
		GCAnsRegCastleSiege(iIndex,7,"");
		return;
	}

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
	{
		GCAnsRegCastleSiege(iIndex,6,"");
		return;
	}

	if(g_CastleSiege.CheckUnionGuildMaster(iIndex) == FALSE)
	{
		GCAnsRegCastleSiege(iIndex,0,"");
		return;
	}

	if(g_CastleSiege.CheckCastleOwnerMember(iIndex) != FALSE || g_CastleSiege.CheckCastleOwnerUnionMember(iIndex) != FALSE)
	{
		GCAnsRegCastleSiege(iIndex,3,"");
		return;
	}

	GS_GDReqRegAttackGuild(g_MapServerManager.GetMapSvrGroup(),iIndex);
#endif
}

struct PMSG_ANS_REGCASTLESIEGE
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	char szGuildName[0x8];
};

void GCAnsRegCastleSiege(int iIndex, int iResult, LPSTR lpszGuildName)
{
#if (GS_CASTLE==1)
	if(lpszGuildName == NULL)
		return;

	if(OBJMAX_RANGE(iIndex) == FALSE)
		return;

	PMSG_ANS_REGCASTLESIEGE pMsgResult;

	pMsgResult.h.set((PBYTE)&pMsgResult,0xb2,0x01,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;

	memcpy(pMsgResult.szGuildName,lpszGuildName,sizeof(pMsgResult.szGuildName));

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);

	if(gObjIsConnected(iIndex))
	{
		LogAddTD("[CastleSiege] [%s][%s] Registered Castle Siege (GUILD:%s) - Result:%d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,iResult);
	}
#endif
}


void CGReqGiveUpCastleSiege(PMSG_REQ_GIVEUPCASTLESIEGE * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)

	if(g_CastleSiege.GetCastleState() < CASTLESIEGE_STATE_REGSIEGE || g_CastleSiege.GetCastleState() > CASTLESIEGE_STATE_REGMARK)
	{
		GCAnsGiveUpCastleSiege(iIndex,3,0,0,"");
		return;
	}

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return;

	if(g_CastleSiege.CheckUnionGuildMaster(iIndex) == FALSE)
		return;

	GS_GDReqGuildSetGiveUp(g_MapServerManager.GetMapSvrGroup(),iIndex,lpMsg->btGiveUp);
#endif
}

struct PMSG_ANS_GIVEUPCASTLESIEGE
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	unsigned char btIsGiveUp;
	char szGuildName[0x8];
};

void GCAnsGiveUpCastleSiege(int iIndex, int iResult, bool bGiveUp, int iMarkCount, LPSTR lpszGuildName)
{
#if (GS_CASTLE==1)
	if(lpszGuildName == NULL)
		return;

	if(OBJMAX_RANGE(iIndex) == FALSE)
		return;

	PMSG_ANS_GIVEUPCASTLESIEGE pMsgResult;

	pMsgResult.h.set((BYTE *)&pMsgResult,0xB2,0x02,sizeof(pMsgResult));

	pMsgResult.btResult = iResult;
	pMsgResult.btIsGiveUp = bGiveUp;
	memcpy(pMsgResult.szGuildName,lpszGuildName,sizeof(pMsgResult.szGuildName));

	if(pMsgResult.btResult == 1 && iMarkCount > 0)
	{
		gObj[iIndex].Money += iMarkCount * 3000;

		if(gObj[iIndex].Money > 2000000000)
		{
			gObj[iIndex].Money = 2000000000;
		}

		GCMoneySend(iIndex,gObj[iIndex].Money);
	}

	DataSend(iIndex,(BYTE *)&pMsgResult,pMsgResult.h.size);

	if(gObjIsConnected(iIndex))
	{
		LogAddTD("[CastleSiege] [%s][%s] GiveUp Castle Siege (GUILD:%s) - Result:%d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,iResult);
	}
#endif
}

void CGReqGuildRegInfo(PMSG_REQ_GUILDREGINFO * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(lpMsg == NULL)
		return;

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return;

	GS_GDReqGuildMarkRegInfo(g_MapServerManager.GetMapSvrGroup(),iIndex);
#endif
}

struct PMSG_ANS_GUILDREGINFO
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	char szGuildName[0x8];
	unsigned char btGuildMark1;
	unsigned char btGuildMark2;
	unsigned char btGuildMark3;
	unsigned char btGuildMark4;
	unsigned char btIsGiveUp;
	unsigned char btRegRank;
};

void GCAnsGuildRegInfo(int iIndex, int iResult, CSP_ANS_GUILDREGINFO * lpMsgResult)
{
#if (GS_CASTLE==1)
	if(lpMsgResult == NULL)
		return;

	if(OBJMAX_RANGE(iIndex) == FALSE)
		return;

	PMSG_ANS_GUILDREGINFO pMsgResult;

	pMsgResult.h.set((BYTE *)&pMsgResult,0xB2,3,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btGuildMark1 = DWORD((DWORD(lpMsgResult->iRegMarkCount) >> 16) &0xFFFF) >> 8;
	pMsgResult.btGuildMark2 = DWORD((DWORD(lpMsgResult->iRegMarkCount) >> 16) &0xFFFF) & 0xFF;
	pMsgResult.btGuildMark3 = DWORD((DWORD(lpMsgResult->iRegMarkCount) & 0xFFFF) &0xFFFF) >> 8;
	pMsgResult.btGuildMark4 = DWORD((DWORD(lpMsgResult->iRegMarkCount) & 0xFFFF) &0xFFFF) & 0xFF;

	pMsgResult.btRegRank = lpMsgResult->btRegRank;
	pMsgResult.btIsGiveUp = lpMsgResult->bIsGiveUp;
	memcpy(pMsgResult.szGuildName,lpMsgResult->szGuildName,sizeof(pMsgResult.szGuildName));

	DataSend(iIndex,(BYTE*)&pMsgResult,pMsgResult.h.size);
#endif
}

#define CHECK_INV_POS(value) (((value)< 0)?FALSE:((value)> 63)?FALSE:TRUE)

struct PMSG_ANS_REGGUILDMARK
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	char szGuildName[0x8];
	unsigned char btGuildMark1;
	unsigned char btGuildMark2;
	unsigned char btGuildMark3;
	unsigned char btGuildMark4;
};

void CGReqRegGuildMark(PMSG_REQ_REGGUILDMARK * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(lpMsg == NULL)
		return;

	if(OBJMAX_RANGE(iIndex) == FALSE)
		return;

	if(gObj[iIndex].UseEventServer == TRUE)
		return;

	gObj[iIndex].UseEventServer = TRUE;

	if(g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGMARK)
	{
		gObj[iIndex].UseEventServer = FALSE;
		return;
	}

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
	{
		gObj[iIndex].UseEventServer = FALSE;
		return;
	}

	if(CHECK_INV_POS(lpMsg->btItemPos) == FALSE)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = {0};

		pMsgResult.h.set((BYTE*)&pMsgResult,0xB2,0x04,sizeof(pMsgResult));
		pMsgResult.btResult = 3;

		DataSend(iIndex,(BYTE*)&pMsgResult,pMsgResult.h.size);
		gObj[iIndex].UseEventServer = FALSE;
		return;
	}

	int iItemPos = lpMsg->btItemPos + 12;

	if(gObj[iIndex].pInventory[iItemPos].IsItem() == FALSE)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = {0};

		pMsgResult.h.set((BYTE*)&pMsgResult,0xB2,0x04,sizeof(pMsgResult));
		pMsgResult.btResult = 3;

		DataSend(iIndex,(BYTE*)&pMsgResult,pMsgResult.h.size);
		gObj[iIndex].UseEventServer = FALSE;
		return;
	}

	if(gObj[iIndex].pInventory[iItemPos].m_Type != ITEMGET(14,21)
		|| gObj[iIndex].pInventory[iItemPos].m_Level != 3
		|| gObj[iIndex].pInventory[iItemPos].m_Durability <= 0.0f)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = {0};

		pMsgResult.h.set((BYTE*)&pMsgResult,0xB2,0x04,sizeof(pMsgResult));
		pMsgResult.btResult = 3;

		DataSend(iIndex,(BYTE*)&pMsgResult,pMsgResult.h.size);
		gObj[iIndex].UseEventServer = FALSE;
		return;
	}

	GS_GDReqRegGuildMark(g_MapServerManager.GetMapSvrGroup(),iIndex,iItemPos);
#endif
}




void GCAnsRegGuildMark(int iIndex, int iResult, CSP_ANS_GUILDREGMARK * lpMsgResult)
{
#if (GS_CASTLE==1)
	if(lpMsgResult == NULL)
		return;

	if(OBJMAX_RANGE(iIndex) == FALSE)
		return;

	if(gObj[iIndex].pInventory[lpMsgResult->iItemPos].IsItem() == FALSE)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = {0};

		pMsgResult.h.set((BYTE*)&pMsgResult,0xB2,0x04,sizeof(pMsgResult));
		pMsgResult.btResult = 3;

		DataSend(iIndex,(BYTE*)&pMsgResult,pMsgResult.h.size);
		gObj[iIndex].UseEventServer = FALSE;
		return;
	}

	if((gObj[iIndex].pInventory[lpMsgResult->iItemPos].m_Type != ITEMGET(14,21)
		&& gObj[iIndex].pInventory[lpMsgResult->iItemPos].m_Level == 3)
		|| gObj[iIndex].pInventory[lpMsgResult->iItemPos].m_Durability <= 0.0f)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = {0};

		pMsgResult.h.set((BYTE*)&pMsgResult,0xB2,0x04,sizeof(pMsgResult));
		pMsgResult.btResult = 3;

		DataSend(iIndex,(BYTE*)&pMsgResult,pMsgResult.h.size);
		gObj[iIndex].UseEventServer = FALSE;
		return;
	}

	gObj[iIndex].pInventory[lpMsgResult->iItemPos].m_Durability -= 1.0f;

	if(gObj[iIndex].pInventory[lpMsgResult->iItemPos].m_Durability > 0.0f)
	{
		GCItemDurSend(iIndex,lpMsgResult->iItemPos,(BYTE)gObj[iIndex].pInventory[lpMsgResult->iItemPos].m_Durability,0);
	}
	else
	{
		gObjInventoryDeleteItem(iIndex,lpMsgResult->iItemPos);
		GCInventoryItemDeleteSend(iIndex,lpMsgResult->iItemPos,1);
	}

	PMSG_ANS_REGGUILDMARK pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult,0xB2,0x04,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;

	pMsgResult.btGuildMark1 = DWORD((DWORD(lpMsgResult->iRegMarkCount) >> 16) &0xFFFF) >> 8;
	pMsgResult.btGuildMark2 = DWORD((DWORD(lpMsgResult->iRegMarkCount) >> 16) &0xFFFF) & 0xFF;
	pMsgResult.btGuildMark3 = DWORD((DWORD(lpMsgResult->iRegMarkCount) & 0xFFFF) &0xFFFF) >> 8;
	pMsgResult.btGuildMark4 = DWORD((DWORD(lpMsgResult->iRegMarkCount) & 0xFFFF) &0xFFFF) & 0xFF;

	memcpy(pMsgResult.szGuildName,lpMsgResult->szGuildName,sizeof(pMsgResult.szGuildName));
	DataSend(iIndex,(BYTE*)&pMsgResult,pMsgResult.h.size);

	gObj[iIndex].UseEventServer = FALSE;
#endif
}




void CGReqNpcBuy(PMSG_REQ_NPCBUY * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(lpMsg == NULL)
		return;

	if(gObjIsConnected(iIndex) == FALSE)
		return;
	
	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return;

	if(g_CastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE
		|| (gObj[iIndex].GuildStatus != G_MASTER && gObj[iIndex].GuildStatus != G_SUB_MASTER))
	{
		LogAddC(2,"[CastleSiege] CGReqNpcBuy() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		return;
	}

	BYTE btResult = NULL;

	int bRETVAL = g_CastleSiege.CheckAddDbNPC(iIndex,lpMsg->iNpcNumber,lpMsg->iNpcIndex,&btResult);

	if(bRETVAL == FALSE)
	{
		GCAnsNpcBuy(iIndex,btResult,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		LogAddC(2,"[CastleSiege] CGReqNpcBuy() ERROR - CCastleSiege::CheckAddDbNPC() [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
	}
	else
	{
		LogAddC(2,"[CastleSiege] CGReqNpcBuy() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
	}
#endif
}


struct PMSG_ANS_NPCBUY
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	int iNpcNumber;
	int iNpcIndex;
};

void GCAnsNpcBuy(int iIndex, int iResult, int iNpcNumber, int iNpcIndex)
{
#if (GS_CASTLE==1)
	PMSG_ANS_NPCBUY pMsgResult;

	pMsgResult.h.set((BYTE *)&pMsgResult,0xB2,0x05,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;

	DataSend(iIndex,(BYTE *)&pMsgResult,pMsgResult.h.size);
#endif
}



void CGReqNpcRepair(PMSG_REQ_NPCREPAIR * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(lpMsg == NULL)
		return;

	if(gObjIsConnected(iIndex) == FALSE)
		return;

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return;

	if(g_CastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE
		|| (gObj[iIndex].GuildStatus != G_MASTER && gObj[iIndex].GuildStatus != G_SUB_MASTER))
	{
		LogAddC(2,"[CastleSiege] CGReqNpcRepair() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);
		return;
	}

	_CS_NPC_DATA pNpcData;

	int bRETVAL = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber,lpMsg->iNpcIndex,(_CS_NPC_DATA &)pNpcData);

	if(bRETVAL == FALSE)
	{
		GCAnsNpcRepair(iIndex,0,lpMsg->iNpcNumber,lpMsg->iNpcIndex,0,0);
		LogAddC(2,"[CastleSiege] CGReqNpcRepair() ERROR - CL Request Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex);
		return;
	}

	int iNpcIndex = pNpcData.m_iNPC_OBJINDEX;

	if(gObjIsConnected(iNpcIndex) == FALSE)
	{
		GCAnsNpcRepair(iIndex,0,lpMsg->iNpcNumber,lpMsg->iNpcIndex,0,0);
		LogAddC(2,"[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,pNpcData.m_iNPC_NUM,pNpcData.m_iNPC_INDEX);
		return;
	}

	if(gObj[iNpcIndex].Live == FALSE)
	{
		GCAnsNpcRepair(iIndex,0,lpMsg->iNpcNumber,lpMsg->iNpcIndex,0,0);
		LogAddC(2,"[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,pNpcData.m_iNPC_NUM,pNpcData.m_iNPC_INDEX);
		return;
	}

	if(gObj[iNpcIndex].MaxLife == gObj[iNpcIndex].Life || gObj[iNpcIndex].MaxLife < gObj[iNpcIndex].Life)
	{
		gObj[iNpcIndex].Life = gObj[iNpcIndex].MaxLife;
		GCAnsNpcRepair(iIndex,0,lpMsg->iNpcNumber,lpMsg->iNpcIndex,0,0);
		return;
	}

	int iRepairCost = 0;

	switch(lpMsg->iNpcNumber)
	{
	case 277:
		iRepairCost = (int)((gObj[iNpcIndex].MaxLife - gObj[iNpcIndex].Life) * 5.0f +  gObj[iNpcIndex].m_btCsNpcDfLevel * 1000000);
		break;
	case 283:
		iRepairCost = (int)((gObj[iNpcIndex].MaxLife - gObj[iNpcIndex].Life) * 3.0f +  (gObj[iNpcIndex].m_btCsNpcDfLevel + gObj[iNpcIndex].m_btCsNpcRgLevel) * 1000000);
		break;
	default:
		GCAnsNpcRepair(iIndex,0,lpMsg->iNpcNumber,lpMsg->iNpcIndex,0,0);
		return;
	}

	if(gObj[iIndex].Money < iRepairCost)
	{
		GCAnsNpcRepair(iIndex,3,lpMsg->iNpcNumber,lpMsg->iNpcIndex,0,0);
		LogAddC(2,"[CastleSiege] CGReqNpcRepair() ERROR - Money isn't enough [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,pNpcData.m_iNPC_NUM,pNpcData.m_iNPC_INDEX);
		return;
	}

	LogAddC(2,"[CastleSiege] CGReqNpcRepair() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)",
		gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,pNpcData.m_iNPC_NUM,pNpcData.m_iNPC_INDEX);

	GS_GDReqCastleNpcRepair(g_MapServerManager.GetMapSvrGroup(),iIndex,lpMsg->iNpcNumber,lpMsg->iNpcIndex,iRepairCost);
#endif
}

struct PMSG_ANS_NPCREPAIR
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcHP;
	int iNpcMaxHP;
};

void GCAnsNpcRepair(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcHP, int iNpcMaxHP)
{
#if (GS_CASTLE==1)
	PMSG_ANS_NPCREPAIR pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult,0xB2,0x06,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcHP = iNpcHP;
	pMsgResult.iNpcMaxHP = iNpcMaxHP;

	DataSend(iIndex,(BYTE*)&pMsgResult,pMsgResult.h.size);
#endif
}




void CGReqNpcUpgrade(PMSG_REQ_NPCUPGRADE * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(lpMsg == NULL)
		return;

	if(gObjIsConnected(iIndex) == FALSE)
		return;

	int iNEED_GEMOFDEFEND = 0;
	int iNEED_MONEY = 0;
	int iCUR_UPLVL = 0;
	int iNXT_UPLVL = 0;
	int iNXT_UPVALUE = 0;
	int bENABLE_UPGRADE = FALSE;

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return;

	if(g_CastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE
		|| gObj[iIndex].GuildStatus != G_MASTER)
	{
		LogAddTD("[CastleSiege] CGReqNpcUpgrade() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);
		return;
	}

	switch(lpMsg->iNpcNumber)
	{
	case 277:
	{
		_CS_NPC_DATA pNpcData;

		int bIsLive = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber,lpMsg->iNpcIndex,(_CS_NPC_DATA &)pNpcData);

		if(bIsLive != FALSE)
		{
			switch(lpMsg->iNpcUpType)
			{
			case 1:
				iCUR_UPLVL = pNpcData.m_iNPC_DF_LEVEL;
				iNXT_UPLVL = lpMsg->iNpcUpValue;

				if(iNXT_UPLVL >= 1 && iNXT_UPLVL <= CS_MAX_UPGRADE)
				{
					if(iNXT_UPLVL == iCUR_UPLVL + 1)
					{
						bENABLE_UPGRADE = TRUE;
						iNXT_UPVALUE = iNXT_UPLVL;
						iNEED_GEMOFDEFEND = g_iNpcUpDfLevel_CGATE[iNXT_UPLVL - 1][1];
						iNEED_MONEY = g_iNpcUpDfLevel_CGATE[iNXT_UPLVL - 1][2];
					}
				}
				break;
			case 3:
				{
					iCUR_UPLVL = 0;
					int iLV;

					for(iLV = 0; iLV < CS_MAX_UPGRADE; iLV++)
					{
						if(pNpcData.m_iNPC_MAXHP >= g_iNpcUpMaxHP_CGATE[iLV][0])
						{
							iCUR_UPLVL = iLV+1;
						}
					}

					iNXT_UPLVL = 0;

					for(iLV = 0; iLV < CS_MAX_UPGRADE; iLV++)
					{
						if(lpMsg->iNpcUpValue >= g_iNpcUpMaxHP_CGATE[iLV][0])
						{
							iNXT_UPLVL = iLV+1;
						}
					}

					if(iNXT_UPLVL >= 1 && iNXT_UPLVL <= CS_MAX_UPGRADE)
					{
						if(iNXT_UPLVL == iCUR_UPLVL + 1)
						{
							bENABLE_UPGRADE = TRUE;
							iNXT_UPVALUE = g_iNpcUpMaxHP_CGATE[iNXT_UPLVL - 1][0];
							iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CGATE[iNXT_UPLVL - 1][1];
							iNEED_MONEY = g_iNpcUpMaxHP_CGATE[iNXT_UPLVL - 1][2];
						}
					}
				}
				break;
			default:
				GCAnsNpcUpgrade(iIndex,5,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
				return;
			}
		}
		else
		{
			GCAnsNpcUpgrade(iIndex,7,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
			return;
		}
	}
		break;
	case 283:
	{
		_CS_NPC_DATA pNpcData;

		int bIsLive = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber,lpMsg->iNpcIndex,(_CS_NPC_DATA &)pNpcData);

		if(bIsLive != FALSE)
		{
			switch(lpMsg->iNpcUpType)
			{
			case 1:
				iCUR_UPLVL = pNpcData.m_iNPC_DF_LEVEL;

				iNXT_UPLVL = lpMsg->iNpcUpValue;

				if(iNXT_UPLVL >= 1 && iNXT_UPLVL <= CS_MAX_UPGRADE)
				{
					if(iNXT_UPLVL == iCUR_UPLVL + 1)
					{
						bENABLE_UPGRADE = TRUE;
						iNXT_UPVALUE = iNXT_UPLVL;

						iNEED_GEMOFDEFEND = g_iNpcUpDfLevel_CSTATUE[iNXT_UPLVL - 1][1];
						iNEED_MONEY = g_iNpcUpDfLevel_CSTATUE[iNXT_UPLVL - 1][2];
					}
				}
				break;
			case 2:
				iCUR_UPLVL = pNpcData.m_iNPC_RG_LEVEL;

				iNXT_UPLVL = lpMsg->iNpcUpValue;

				if(iNXT_UPLVL >= 1 && iNXT_UPLVL <= CS_MAX_UPGRADE)
				{
					if(iNXT_UPLVL == iCUR_UPLVL + 1)
					{
						bENABLE_UPGRADE = TRUE;
						iNXT_UPVALUE = iNXT_UPLVL;

						iNEED_GEMOFDEFEND = g_iNpcUpRgLevel_CSTATUE[iNXT_UPLVL - 1][1];
						iNEED_MONEY = g_iNpcUpRgLevel_CSTATUE[iNXT_UPLVL - 1][2];
					}
				}
				break;
			case 3:
				{
					iCUR_UPLVL = 0;
					int iLV;
					for(iLV = 0; iLV < CS_MAX_UPGRADE; iLV++)
					{
						if(pNpcData.m_iNPC_MAXHP >= g_iNpcUpMaxHP_CSTATUE[iLV][0])
						{
							iCUR_UPLVL = iLV+1;
						}
					}

					iNXT_UPLVL = 0;

					for(iLV = 0; iLV < CS_MAX_UPGRADE; iLV++)
					{
						if(lpMsg->iNpcUpValue >= g_iNpcUpMaxHP_CSTATUE[iLV][0])
						{
							iNXT_UPLVL = iLV+1;
						}
					}

					if(iNXT_UPLVL >= 1 && iNXT_UPLVL <= CS_MAX_UPGRADE)
					{
						if(iNXT_UPLVL == iCUR_UPLVL + 1)
						{
							bENABLE_UPGRADE = TRUE;
							iNXT_UPVALUE = g_iNpcUpMaxHP_CSTATUE[iNXT_UPLVL - 1][0];
							iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CSTATUE[iNXT_UPLVL - 1][1];
							iNEED_MONEY = g_iNpcUpMaxHP_CSTATUE[iNXT_UPLVL - 1][2];
						}
					}
				}
				break;
			default:
				GCAnsNpcUpgrade(iIndex,5,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
				LogAddTD("[CastleSiege] CGReqNpcUpgrade() ERROR - UpType doesn't Exist [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d",
					gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
				return;
			}
		}
		else
		{
			GCAnsNpcUpgrade(iIndex,7,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
			return;
		}
	}
		break;
	}

	if(bENABLE_UPGRADE == FALSE)
	{
		GCAnsNpcUpgrade(iIndex,6,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
		return;
	}

	if(gObj[iIndex].Money < iNEED_MONEY)
	{
		GCAnsNpcUpgrade(iIndex,3,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
		LogAddTD("[CastleSiege] CGReqNpcUpgrade() ERROR - Money is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Money:%d, Need Money:%d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue,iNXT_UPLVL,gObj[iIndex].Money,iNEED_MONEY);
		return;
	}

	int iGEMOFDEFEND_COUNT = 0;

	for(int x = 0; x < INVENTORY_SIZE; x++)
	{
		if(gObj[iIndex].pInventory[x].IsItem() == TRUE && gObj[iIndex].pInventory[x].m_Type == ITEMGET(14,31))
		{
			iGEMOFDEFEND_COUNT++;
		}
	}

	if(iGEMOFDEFEND_COUNT < iNEED_GEMOFDEFEND)
	{
		GCAnsNpcUpgrade(iIndex,4,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue);
		LogAddTD("[CastleSiege] CGReqNpcUpgrade() ERROR - Gem is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Gem:%d, Need Gem:%d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue,iNXT_UPLVL,iGEMOFDEFEND_COUNT,iNEED_GEMOFDEFEND);
		return;
	}

	int bRESULT = g_CastleSiege.PayForUpgradeDbNPC(iIndex,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue,iNXT_UPLVL -1);

	if(bRESULT == FALSE)
		return;

	gObj[iIndex].m_bIsCastleNPCUpgradeCompleted = TRUE;

	LogAddTD("[CastleSiege] [0xB2][0x07] CGReqNpcUpgrade() - Pay For Npc Upgrade (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)",
		lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,iNXT_UPVALUE);

	GS_GDReqCastleNpcUpgrade(g_MapServerManager.GetMapSvrGroup(),iIndex,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,iNXT_UPVALUE,iNXT_UPLVL -1);
	LogAddTD("[CastleSiege] CGReqNpcUpgrade() REQ OK - [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d",
		gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->iNpcNumber,lpMsg->iNpcIndex,lpMsg->iNpcUpType,lpMsg->iNpcUpValue,iNXT_UPLVL);
#endif
}

struct PMSG_ANS_NPCUPGRADE
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcUpType;
	int iNpcUpValue;
};

void GCAnsNpcUpgrade(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue)
{
#if (GS_CASTLE==1)

	PMSG_ANS_NPCUPGRADE pMsgResult;

	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x07,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcUpType = iNpcUpType;
	pMsgResult.iNpcUpValue = iNpcUpValue;

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}




void CGReqTaxMoneyInfo(PMSG_REQ_TAXMONEYINFO * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(lpMsg == NULL)
		return;

	if(strcmp(gObj[iIndex].GuildName,"")==NULL)
		return;

	if(g_CastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE
		|| gObj[iIndex].GuildStatus != G_MASTER)
	{
		LogAddC(2,"[CastleSiege] CGReqTaxMoneyInfo() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);
		return;
	}

	GS_GDReqTaxInfo(g_MapServerManager.GetMapSvrGroup(),iIndex);
	LogAddTD("[CastleSiege] CGReqTaxMoneyInfo() REQ OK - [%s][%s], Guild:(%s)(%d)",
		gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);

#endif
}


struct PMSG_ANS_TAXMONEYINFO
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	unsigned char btTaxRateChaos;
	unsigned char btTaxRateStore;
	unsigned char btMoney1;
	unsigned char btMoney2;
	unsigned char btMoney3;
	unsigned char btMoney4;
	unsigned char btMoney5;
	unsigned char btMoney6;
	unsigned char btMoney7;
	unsigned char btMoney8;
};

void GCAnsTaxMoneyInfo(int iIndex, int iResult, BYTE btTaxRateChaos, BYTE btTaxRateStore,__int64 i64Money)
{
#if (GS_CASTLE==1)
	PMSG_ANS_TAXMONEYINFO pMsgResult;

	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x08,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btTaxRateChaos = btTaxRateChaos;
	pMsgResult.btTaxRateStore = btTaxRateStore;

	pMsgResult.btMoney1 = DWORD((DWORD(i64Money >> 32) >> 16) & 0xFFFF) >> 8;
	pMsgResult.btMoney2 = DWORD((DWORD(i64Money >> 32) >> 16) & 0xFFFF) & 0xFF;
	pMsgResult.btMoney3 = DWORD((DWORD(i64Money >> 32) & 0xFFFF) & 0xFFFF) >> 8;
	pMsgResult.btMoney4 = DWORD((DWORD(i64Money >> 32) & 0xFFFF) & 0xFFFF) & 0xFF;

	pMsgResult.btMoney5 = DWORD((DWORD(i64Money & 0xFFFFFFFF) >> 16) & 0xFFFF) >> 8;
	pMsgResult.btMoney6 = DWORD((DWORD(i64Money & 0xFFFFFFFF) >> 16) & 0xFFFF) & 0xFF;
	pMsgResult.btMoney7 = DWORD((DWORD(i64Money & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) >> 8;
	pMsgResult.btMoney8 = DWORD((DWORD(i64Money & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) & 0xFF;

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}




void CGReqTaxRateChange(PMSG_REQ_TAXRATECHANGE * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(lpMsg == NULL)
		return;

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return;

	if(g_CastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE
		|| gObj[iIndex].GuildStatus != G_MASTER)
	{
		LogAddC(2,"[CastleSiege] CGReqTaxRateChange() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);
		return;
	}

	int iMaxTaxRate =0;

	switch(lpMsg->btTaxType)
	{
	case 1:
		iMaxTaxRate = 3;
		break;
	case 2:
		iMaxTaxRate = 3;
		break;
	case 3:
		iMaxTaxRate = 300000;
		break;
	}

	int iTaxRate = (( ( ( (lpMsg->btTaxRate4 & 0xFF) & 0xFF | ((lpMsg->btTaxRate3 & 0xFF) & 0xFF ) << 8)) &0xFFFF) & 0xFFFF) & 0xFFFF | ((( ( ( (lpMsg->btTaxRate2 & 0xFF) & 0xFF | ((lpMsg->btTaxRate1 & 0xFF) & 0xFF ) << 8)) &0xFFFF) & 0xFFFF) & 0xFFFF) << 16;

	if(iTaxRate < 0 || iTaxRate > iMaxTaxRate)
	{
		GCAnsTaxRateChange(iIndex,0,0,0);
		LogAddTD("[CastleSiege] CGReqTaxRateChange() ERROR - Tax Rate Out of Range [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->btTaxType,iTaxRate);
		return;
	}

	GS_GDReqTaxRateChange(g_MapServerManager.GetMapSvrGroup(),iIndex,lpMsg->btTaxType,iTaxRate);
	LogAddTD("[CastleSiege] CGReqTaxRateChange() REQ OK - [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,lpMsg->btTaxType,iTaxRate);

#endif
}


struct PMSG_ANS_TAXRATECHANGE
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	unsigned char btTaxType;
	unsigned char btTaxRate1;
	unsigned char btTaxRate2;
	unsigned char btTaxRate3;
	unsigned char btTaxRate4;
};

void GCAnsTaxRateChange(int iIndex, int iResult, BYTE btTaxType, int iTaxRate)
{
#if (GS_CASTLE==1)
	PMSG_ANS_TAXRATECHANGE pMsgResult;

	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x09,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btTaxType = btTaxType;
	
	pMsgResult.btTaxRate1 = DWORD((DWORD(iTaxRate) >> 16) &0xFFFF) >> 8;
	pMsgResult.btTaxRate2 = DWORD((DWORD(iTaxRate) >> 16) &0xFFFF) & 0xFF;
	pMsgResult.btTaxRate3 = DWORD((DWORD(iTaxRate) & 0xFFFF) &0xFFFF) >> 8;
	pMsgResult.btTaxRate4 = DWORD((DWORD(iTaxRate) & 0xFFFF) &0xFFFF) & 0xFF;

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}




void CGReqMoneyDrawOut(PMSG_REQ_MONEYDRAWOUT * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)

	if(lpMsg == NULL)
		return;

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return;

	if(g_CastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE
		|| gObj[iIndex].GuildStatus != G_MASTER)
	{
		LogAddC(2,"[CastleSiege] CGReqMoneyDrawOut() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);
		return;
	}

	int iMoneyChange = ((( ( ( (lpMsg->btMoney1 & 0xFF) & 0xFF | ((lpMsg->btMoney2 & 0xFF) & 0xFF ) << 8)) &0xFFFF) & 0xFFFF) & 0xFFFF) | ((( ( ( (lpMsg->btMoney3 & 0xFF) & 0xFF | ((lpMsg->btMoney4 & 0xFF) & 0xFF ) << 8)) &0xFFFF) & 0xFFFF) & 0xFFFF) << 16;

	if(iMoneyChange <= 0)
	{
		GCAnsMoneyDrawOut(iIndex,0,0);
		LogAddTD("[CastleSiege] CGReqMoneyDrawOut() ERROR - Req Money < 0 [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,iMoneyChange,gObj[iIndex].Money);
		return;
	}

	int bRETVAL = g_CastleSiege.CheckCastleHasMoney(iMoneyChange);

	if(bRETVAL == FALSE)
	{
		GCAnsMoneyDrawOut(iIndex,0,0);
		LogAddTD("[CastleSiege] CGReqMoneyDrawOut() ERROR - Castle Money is Low [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,iMoneyChange,gObj[iIndex].Money);
		return;
	}

	if(gObj[iIndex].Money + iMoneyChange > 2000000000)
	{
		GCAnsMoneyDrawOut(iIndex,0,0);
		return;
	}

	GS_GDReqCastleMoneyChange(g_MapServerManager.GetMapSvrGroup(),iIndex,-iMoneyChange);
	LogAddTD("[CastleSiege] CGReqMoneyDrawOut() REQ OK - [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d",
		gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,iMoneyChange,gObj[iIndex].Money);
#endif
}

struct PMSG_ANS_MONEYDRAWOUT
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	unsigned char btMoney1;
	unsigned char btMoney2;
	unsigned char btMoney3;
	unsigned char btMoney4;
	unsigned char btMoney5;
	unsigned char btMoney6;
	unsigned char btMoney7;
	unsigned char btMoney8;
};


void GCAnsMoneyDrawOut(int iIndex, int iResult, __int64 i64Money)
{
#if (GS_CASTLE==1)
	PMSG_ANS_MONEYDRAWOUT pMsgResult;

	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x10,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;

	pMsgResult.btMoney1 = DWORD((DWORD(i64Money >> 32) >> 16) & 0xFFFF) >> 8;
	pMsgResult.btMoney2 = DWORD((DWORD(i64Money >> 32) >> 16) & 0xFFFF) & 0xFF;
	pMsgResult.btMoney3 = DWORD((DWORD(i64Money >> 32) & 0xFFFF) & 0xFFFF) >> 8;
	pMsgResult.btMoney4 = DWORD((DWORD(i64Money >> 32) & 0xFFFF) & 0xFFFF) & 0xFF;

	pMsgResult.btMoney5 = DWORD((DWORD(i64Money & 0xFFFFFFFF) >> 16) & 0xFFFF) >> 8;
	pMsgResult.btMoney6 = DWORD((DWORD(i64Money & 0xFFFFFFFF) >> 16) & 0xFFFF) & 0xFF;
	pMsgResult.btMoney7 = DWORD((DWORD(i64Money & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) >> 8;
	pMsgResult.btMoney8 = DWORD((DWORD(i64Money & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) & 0xFF;

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}

struct PMSG_ANS_CSGATESTATE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btIndex1;
	BYTE btIndex2;
};


void GCAnsCsGateState(int iIndex, int iResult, int iGateIndex)
{
#if (GS_CASTLE==1)

	PMSG_ANS_CSGATESTATE pMsgResult;
	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x11,sizeof(pMsgResult));

	pMsgResult.btResult = iResult;
	pMsgResult.btIndex1 = SET_NUMBERH(iGateIndex & 0xFFFF);
	pMsgResult.btIndex2 = SET_NUMBERL(iGateIndex & 0xFFFF);

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}





void CGReqCsGateOperate(PMSG_REQ_CSGATEOPERATE * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return;

	if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		if(gObj[iIndex].m_btCsJoinSide != 1)
		{
			LogAddTD("[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)",
				gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);
			return;
		}
	}
	else if(g_CastleSiege.CheckCastleOwnerMember(iIndex) == FALSE && g_CastleSiege.CheckCastleOwnerUnionMember(iIndex) == FALSE)
	{
		LogAddTD("[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)",
				gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);
		return;
	}

	int iGateIndex = ( ((lpMsg->btIndex2 & 0xFF) & 0xFF) | ((lpMsg->btIndex1 & 0xFF) & 0xFF) << 8) & 0xFFFF;

	if(g_CastleSiege.CheckCsGateAlive(iGateIndex) == TRUE)
	{
		int bRETVAL = g_CastleSiege.OperateGate(iGateIndex,gObj[iGateIndex].m_iCsNpcExistVal,lpMsg->btOperate);

		if(bRETVAL == FALSE)
		{
			GCAnsCsGateOperate(iIndex,0,-1,0);
			LogAddTD("[CastleSiege] CGReqCsGateOperate() ERROR - Operate Gate Failed [%s][%s], Guild:(%s)(%d)",
				gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);
			return;
		}

		GCAnsCsGateOperate(iIndex,1,iGateIndex,lpMsg->btOperate);
		g_CastleSiege.SendCsGateStateViewPort(iGateIndex,lpMsg->btOperate);
		LogAddTD("[CastleSiege] CGReqCsGateOperate() OK - [%s][%s], Guild:(%s)(%d), DOOR:(%d)(X:%d,Y:%d)(STATUE:%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus,iGateIndex,gObj[iGateIndex].X,gObj[iGateIndex].Y,gObj[iGateIndex].m_btCsGateOpen);

	}
	else
	{
		GCAnsCsGateOperate(iIndex,2,-1,0);
		LogAddTD("[CastleSiege] CGReqCsGateOperate() ERROR - Gate Doesn't Exist [%s][%s], Guild:(%s)(%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);
	}

#endif
}


struct PMSG_ANS_CSGATEOPERATE
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	unsigned char btOperate;
	unsigned char btIndex1;
	unsigned char btIndex2;
};

void GCAnsCsGateOperate(int iIndex, int iResult, int iGateIndex, int iGateOperate)
{
#if (GS_CASTLE==1)

	PMSG_ANS_CSGATEOPERATE pMsgResult;

	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x12,sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERH(iGateIndex & 0xFFFF);
	pMsgResult.btIndex2 = SET_NUMBERL(iGateIndex & 0xFFFF);

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}


struct PMSG_ANS_CSGATECURSTATE
{
	PBMSG_HEAD2 h;
	unsigned char btOperate;
	unsigned char btIndex1;
	unsigned char btIndex2;
};

void GCAnsCsGateCurState(int iIndex, int iGateIndex, int iGateOperate)
{
#if (GS_CASTLE==1)
	PMSG_ANS_CSGATECURSTATE pMsgResult;
	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x13,sizeof(pMsgResult));

	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERH(iGateIndex & 0xFFFF);
	pMsgResult.btIndex2 = SET_NUMBERL(iGateIndex & 0xFFFF);

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}


struct PMSG_ANS_NOTIFYSWITCHPROC
{
	PBMSG_HEAD2 h;
	unsigned char btIndex1;
	unsigned char btIndex2;
	unsigned char btUserIndex1;
	unsigned char btUserIndex2;
	unsigned char btSwitchState;
};

void GCAnsCsAccessSwitchState(int iIndex, int iSwitchIndex, int iSwitchUserIndex, BYTE btSwitchState)
{
#if (GS_CASTLE==1)
	PMSG_ANS_NOTIFYSWITCHPROC pMsgResult;
	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x14,sizeof(pMsgResult));

	pMsgResult.btIndex1 = SET_NUMBERH(iSwitchIndex & 0xFFFF);
	pMsgResult.btIndex2 = SET_NUMBERL(iSwitchIndex & 0xFFFF);

	pMsgResult.btUserIndex1 = SET_NUMBERH(iSwitchUserIndex & 0xFFFF);
	pMsgResult.btUserIndex2 = SET_NUMBERL(iSwitchUserIndex & 0xFFFF);

	pMsgResult.btSwitchState = btSwitchState;

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}


struct PMSG_ANS_NOTIFYCROWNPROC
{
	PBMSG_HEAD2 h;
	unsigned char btCrownState;
	DWORD dwAccessTime;
};

void GCAnsCsAccessCrownState(int iIndex, BYTE btCrownState)
{
#if (GS_CASTLE==1)
	PMSG_ANS_NOTIFYCROWNPROC pMsgResult;
	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x15,sizeof(pMsgResult));
	pMsgResult.btCrownState = btCrownState;

	if(btCrownState == 0)
	{
		if(gObj[iIndex].m_iAccumulatedCrownAccessTime < 0 || gObj[iIndex].m_iAccumulatedCrownAccessTime > 30000)
		{
			gObj[iIndex].m_iAccumulatedCrownAccessTime = 0;
		}

		LogAddTD("[CastleSiege] [Reg. Attempt] Accumulated Crown AccessTime : %d [%s](%s)(%s)",
			gObj[iIndex].m_iAccumulatedCrownAccessTime,gObj[iIndex].GuildName,gObj[iIndex].AccountID,gObj[iIndex].Name);
	}

	if(btCrownState == 1)
	{
		LogAddTD("[CastleSiege] [Reg. Success] Accumulated Crown AccessTime : %d [%s](%s)(%s)",
			gObj[iIndex].m_iAccumulatedCrownAccessTime,gObj[iIndex].GuildName,gObj[iIndex].AccountID,gObj[iIndex].Name);
		gObj[iIndex].m_iAccumulatedCrownAccessTime = 0;
	}

	if(btCrownState == 2)
	{
		gObj[iIndex].m_iAccumulatedCrownAccessTime += GetTickCount() - g_CastleSiege.GetCrownAccessTickCount();

		if(gObj[iIndex].m_iAccumulatedCrownAccessTime < 0 || gObj[iIndex].m_iAccumulatedCrownAccessTime > 30000)
		{
			gObj[iIndex].m_iAccumulatedCrownAccessTime = 0;
		}

		LogAddTD("[CastleSiege] [Reg. Fail] Accumulated Crown AccessTime : %d [%s](%s)(%s)",
			gObj[iIndex].m_iAccumulatedCrownAccessTime,gObj[iIndex].GuildName,gObj[iIndex].AccountID,gObj[iIndex].Name);
	}

	pMsgResult.dwAccessTime = gObj[iIndex].m_iAccumulatedCrownAccessTime;
	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}


struct PMSG_ANS_NOTIFYCSSTART
{
	PBMSG_HEAD2 h;
	unsigned char btStartState;
};

void GCAnsCsNotifyStart(int iIndex, BYTE btStartState)
{
#if (GS_CASTLE==1)
	PMSG_ANS_NOTIFYCSSTART pMsgResult;
	pMsgResult.h.set((PBYTE)&pMsgResult,0xB2,0x17,sizeof(pMsgResult));
	pMsgResult.btStartState = btStartState;

	DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
#endif
}



struct PMSG_ANS_NOTIFYCSPROGRESS
{
	PBMSG_HEAD2 h;	// C1:B2:18
	BYTE btCastleSiegeState;	// 4
	BYTE szGuildName[8];	// 5
};


void GCAnsCsNotifyProgress(int iIndex, BYTE btCastleSiegeState, LPCSTR lpszGuildName)
{
	if ( lpszGuildName == NULL )
		return;

	PMSG_ANS_NOTIFYCSPROGRESS pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x18, sizeof(pMsgResult));
	pMsgResult.btCastleSiegeState = btCastleSiegeState;
	memset(pMsgResult.szGuildName, 0, sizeof(pMsgResult.szGuildName));
	memcpy(pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	DataSend(iIndex, (UCHAR*)&pMsgResult, pMsgResult.h.size);
}


struct PMSG_ANS_MAPSVRTAXINFO
{
	PBMSG_HEAD2 h;	// C1:B2:1A
	BYTE btTaxType;	// 4
	BYTE btTaxRate;	// 5
};


void GCAnsCsMapSvrTaxInfo(int iIndex, BYTE btTaxType, BYTE btTaxRate)
{
	PMSG_ANS_MAPSVRTAXINFO pMsgResult;
	
	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1A, sizeof(pMsgResult));
	pMsgResult.btTaxType = btTaxType;
	pMsgResult.btTaxRate = btTaxRate;

	DataSend(iIndex, (UCHAR*)&pMsgResult, pMsgResult.h.size);

}




void CGReqCsMiniMapData(PMSG_REQ_MINIMAPDATA * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(gObjIsConnected(iIndex) == FALSE)
		return;

	if(gObj[iIndex].m_btCsJoinSide <= 0 || g_CastleSiege.CheckUnionGuildMaster(iIndex) == FALSE)
	{
		GCAnsCsMiniMapData(iIndex,3);
	}
	else if(g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
	{
		GCAnsCsMiniMapData(iIndex,2);
	}
	else
	{
		g_CastleSiege.AddMiniMapDataReqUser(iIndex);
		GCAnsCsMiniMapData(iIndex,1);
	}
#endif
}



struct PMSG_ANS_MINIMAPDATA
{
	PBMSG_HEAD2 h;
	BYTE btResult;	// 4
};


void GCAnsCsMiniMapData(int iIndex, BYTE btResult)
{
	PMSG_ANS_MINIMAPDATA pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1B, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}



void CGReqStopCsMiniMapData(PMSG_REQ_STOPMINIMAPDATA * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(gObjIsConnected(iIndex) == FALSE)
		return;
	g_CastleSiege.DelMiniMapDataReqUser(iIndex);
#endif
}



void CGReqCsSendCommand(PMSG_REQ_CSCOMMAND * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(gObjIsConnected(iIndex) == FALSE)
		return;

	if(gObj[iIndex].m_btCsJoinSide <= 0 || g_CastleSiege.CheckUnionGuildMaster(iIndex) == FALSE)
		return;

	if(g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
		return;

	GCAnsCsSendCommand(gObj[iIndex].m_btCsJoinSide,lpMsg->btTeam,lpMsg->btX,lpMsg->btY,lpMsg->btCommand);

#endif
}


struct PMSG_ANS_CSCOMMAND
{
	PBMSG_HEAD2 h;
	unsigned char btTeam;
	unsigned char btX;
	unsigned char btY;
	unsigned char btCommand;
};

void GCAnsCsSendCommand(int iCsJoinSize, BYTE btTeam, BYTE btX, BYTE btY, BYTE btCommand)
{
#if (GS_CASTLE==1)
	PMSG_ANS_CSCOMMAND pMsgResult;
	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1D, sizeof(pMsgResult));

	pMsgResult.btTeam = btTeam;
	pMsgResult.btX = btX;
	pMsgResult.btY = btY;
	pMsgResult.btCommand = btCommand;

	for(int iIndex = OBJ_STARTUSERINDEX; iIndex < OBJMAX; iIndex++)
	{
		if(gObjIsConnected(iIndex) == FALSE)
			continue;

		if(gObj[iIndex].m_btCsJoinSide == iCsJoinSize && gObj[iIndex].MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			DataSend(iIndex,(PBYTE)&pMsgResult,pMsgResult.h.size);
		}
	}
#endif
}



struct PMSG_ANS_CSLEFTTIMEALARM
{
	PBMSG_HEAD2 h;	// C1:B2:1E
	BYTE btHour;	// 4
	BYTE btMinute;	// 5
};


void GCAnsCsLeftTimeAlarm(BYTE btHour, BYTE btMinute)
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	for (int iIndex = OBJ_STARTUSERINDEX;iIndex < OBJMAX;iIndex++)
	{
		if ( !gObjIsConnected(iIndex) )
		{
			continue;
		}

		DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	}

}






void GCAnsSelfCsLeftTimeAlarm(int iIndex, BYTE btHour, BYTE btMinute)
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	if ( !gObjIsConnected(iIndex))
	{
		return;
	}

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}






void CGReqCsSetEnterHuntZone(PMSG_REQ_CSHUNTZONEENTER * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(gObjIsConnected(iIndex) == FALSE)
		return;

	if(g_CastleSiege.CheckCastleOwnerMember(iIndex) == FALSE)
	{
		GCAnsCsSetEnterHuntZone(iIndex,2,lpMsg->btHuntZoneEnter);
		return;
	}

	if(g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		GCAnsCsSetEnterHuntZone(iIndex,0,lpMsg->btHuntZoneEnter);
		return;
	}

	g_CastleSiege.SetHuntZoneEnter(lpMsg->btHuntZoneEnter);
	GCAnsCsSetEnterHuntZone(iIndex,1,lpMsg->btHuntZoneEnter);
#endif
}



struct PMSG_ANS_CSHUNTZONEENTER
{
	PBMSG_HEAD2 h;	// C1:B2:1F
	BYTE btResult;	// 4
	BYTE btHuntZoneEnter;	// 5
};


void GCAnsCsSetEnterHuntZone(int iIndex, BYTE btResult, BYTE btEnterHuntZone)
{
	PMSG_ANS_CSHUNTZONEENTER pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1F, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;
	pMsgResult.btHuntZoneEnter = btEnterHuntZone;

	DataSend(iIndex, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}


struct PMSG_ANS_NPCDBLIST
{
	PWMSG_HEAD h;
	unsigned char btResult;
	int iCount;
};

void CGReqNpcDbList(PMSG_REQ_NPCDBLIST * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(lpMsg == NULL)
		return;

	if(strcmp(gObj[iIndex].GuildName,"") == NULL)
		return;

	if(g_CastleSiege.CheckGuildOwnCastle(gObj[iIndex].GuildName) == FALSE ||
		(gObj[iIndex].GuildStatus != G_MASTER && gObj[iIndex].GuildStatus != G_SUB_MASTER))
	{
		PMSG_ANS_NPCDBLIST pMsgResult;

		LogAddC(2,"[CastleSiege] CGReqNpcDbList() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)",
			gObj[iIndex].AccountID,gObj[iIndex].Name,gObj[iIndex].GuildName,gObj[iIndex].GuildStatus);

		pMsgResult.h.set((PBYTE)&pMsgResult,0xB3,sizeof(pMsgResult));
		pMsgResult.iCount = 0;
		pMsgResult.btResult = 2;
		DataSend(iIndex,(PBYTE)&pMsgResult,sizeof(pMsgResult));
		return;
	}

	switch(lpMsg->btMonsterCode)
	{
	case 1:
		g_CastleSiege.SendNpcStateList(iIndex,277);
		break;
	case 2:
		g_CastleSiege.SendNpcStateList(iIndex,283);
		break;
	}

#endif
}



void CGReqCsRegGuildList(PMSG_REQ_CSREGGUILDLIST * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)
	if(lpMsg == NULL)
		return;

	GS_GDReqAllGuildMarkRegInfo(g_MapServerManager.GetMapSvrGroup(),iIndex);
#endif
}


struct PMSG_ANS_CSATTKGUILDLIST
{
	PWMSG_HEAD h;
	unsigned char btResult;
	int iCount;
};


void CGReqCsAttkGuildList(PMSG_REQ_CSATTKGUILDLIST * lpMsg, int iIndex)
{
#if (GS_CASTLE==1)

	if(lpMsg == NULL)
		return;

	char cBUFFER[0x3FE];

	PMSG_ANS_CSATTKGUILDLIST * lpMsgSend = (PMSG_ANS_CSATTKGUILDLIST *)cBUFFER;
	PMSG_CSATTKGUILDLIST * lpMsgSendBody = (PMSG_CSATTKGUILDLIST*)&cBUFFER[sizeof(PMSG_ANS_CSATTKGUILDLIST)];

	int iCount = 0;

	lpMsgSend->btResult = g_CastleSiege.GetCsAttkGuildList(lpMsgSendBody,&iCount);
	lpMsgSend->iCount = iCount;

	if(lpMsgSend->iCount < 0)
		lpMsgSend->iCount = 0;

	if(lpMsgSend->iCount > 100)
		lpMsgSend->iCount = 100;

	lpMsgSend->h.set((PBYTE)lpMsgSend,0xB5,sizeof(PMSG_CSATTKGUILDLIST) * lpMsgSend->iCount + sizeof(PMSG_ANS_CSATTKGUILDLIST));
	DataSend(iIndex,(PBYTE)lpMsgSend,sizeof(PMSG_CSATTKGUILDLIST) * lpMsgSend->iCount + sizeof(PMSG_ANS_CSATTKGUILDLIST));

#endif
}




struct PMSG_ANS_USEWEAPON
{
	PBMSG_HEAD2 h;	// C1:B7:01
	BYTE btResult;	// [1:Attacker] [2:Targets]	4
	BYTE btObjIndexH;	// 5
	BYTE btObjIndexL;	// 6
	BYTE btWeaponType;	// [1:Slingshot attack] [2:Slingshot defense]	7
	BYTE btPointX;	// 8
	BYTE btPointY;	// 9
};



void CGReqWeaponUse(PMSG_REQ_USEWEAPON * aRecv, int iIndex)
{
	WORD wObjIndex = MAKE_NUMBERW(aRecv->btObjIndexH, aRecv->btObjIndexL);

	if ( !OBJMAX_RANGE(wObjIndex))
		return;

	if ( !OBJMAX_RANGE(iIndex))
		return;

	LPOBJ lpOwnerObj = &gObj[iIndex];
	LPOBJ lpWeaponObj = &gObj[wObjIndex];

	if ( lpOwnerObj->Type != OBJ_USER )
		return;

	if ( lpWeaponObj->Type != OBJ_NPC )
		return;

	PMSG_ANS_USEWEAPON pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, 0xB7, 0x01, sizeof(pMsg));
	pMsg.btResult = 1;

	if ( lpWeaponObj->Class == 221 )	// Slingshot attack
	{
		pMsg.btWeaponType = 1;
		pMsg.btObjIndexH = SET_NUMBERH(wObjIndex);
		pMsg.btObjIndexL = SET_NUMBERL(wObjIndex);
		g_CsNPC_Weapon.GetTargetPointXY(lpWeaponObj->Class, aRecv->btTargetIndex-1, pMsg.btPointX, pMsg.btPointY, TRUE);
	}
	else if( lpWeaponObj->Class == 222 )	// Slingshot defense
	{
		pMsg.btWeaponType = 2;
		pMsg.btObjIndexH = SET_NUMBERH(wObjIndex);
		pMsg.btObjIndexL = SET_NUMBERL(wObjIndex);
		g_CsNPC_Weapon.GetTargetPointXY(lpWeaponObj->Class, aRecv->btTargetIndex-1, pMsg.btPointX, pMsg.btPointY, TRUE);
	}

	DataSend(iIndex, (LPBYTE)&pMsg, sizeof(pMsg));

	pMsg.btResult = 2;

	for ( int n=0;n<MAX_VIEWPORT;n++)
	{
		if ( lpWeaponObj->VpPlayer2[n].type == OBJ_USER )
		{
			if ( lpWeaponObj->VpPlayer2[n].state )
			{
				if ( iIndex != lpWeaponObj->VpPlayer2[n].number )
				{
					DataSend(lpWeaponObj->VpPlayer2[n].number, (LPBYTE)&pMsg, sizeof(pMsg));
				}
			}
		}
	}

	gObjNotifyUseWeaponV1(lpOwnerObj, lpWeaponObj, pMsg.btPointX, pMsg.btPointY);
	g_CsNPC_Weapon.SetWeaponCalDamageInfo(wObjIndex, pMsg.btPointX, pMsg.btPointY, 10000);
}





void CGReqWeaponDamageValue(PMSG_REQ_WEAPON_DAMAGE_VALUE * aRecv, int iIndex)
{
	if ( !OBJMAX_RANGE(iIndex) )
		return;

	BYTE btTargetX = 0;
	BYTE btTargetY = 0;
	WORD wObjIndex = MAKE_NUMBERW(aRecv->btObjIndexH, aRecv->btObjIndexL);

	if ( g_CsNPC_Weapon.GetWeaponCalDamageInfo(wObjIndex, btTargetX, btTargetY) == TRUE )
	{
		gObjNotifyUseWeaponDamage(&gObj[wObjIndex], btTargetX, btTargetY);
	}
	else
	{
		MsgOutput(iIndex, lMsg.Get(MSGGET(6, 112)));
	}
}





struct PMSG_ANS_GUILDMARK_OF_CASTLEOWNER
{
	PBMSG_HEAD2 h;	// C1:B9:02
	BYTE GuildMarkOfCastleOwner[32];	// 4
};


void CGReqGuildMarkOfCastleOwner(PMSG_REQ_GUILDMARK_OF_CASTLEOWNER * aRecv, int iIndex)
{
	if ( !OBJMAX_RANGE(iIndex) )
		return;

	LPOBJ lpObj = &gObj[iIndex];
	char szGuildName[MAX_GUILD_LEN+1] = {0};

	memcpy(szGuildName, g_CastleSiegeSync.GetCastleOwnerGuild(), MAX_GUILD_LEN);

	if ( szGuildName )
	{
		_GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(szGuildName);

		if ( lpGuild != NULL )
		{
			PMSG_ANS_GUILDMARK_OF_CASTLEOWNER pMsg = {0};

			PHeadSubSetB((LPBYTE)&pMsg, 0xB9, 0x02, sizeof(pMsg));
			memcpy(pMsg.GuildMarkOfCastleOwner, lpGuild->Mark, sizeof(pMsg.GuildMarkOfCastleOwner));

			DataSend(iIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
}



void CGReqJewelMix(PMSG_REQ_JEWEL_MIX * lpMsg, int iIndex)
{
	if ( lpMsg == NULL )
		return;

	CJewelMixSystem::MixJewel(iIndex, lpMsg->btJewelType, lpMsg->btJewelMix);
}


struct PMSG_ANS_JEWEL_UNMIX
{
	PBMSG_HEAD2 h;	// C1:BC:[00:01]
	BYTE btResult;	// 4
};


void GCAnsJewelMix(int iIndex, int iResult)
{
	PMSG_ANS_JEWEL_UNMIX pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xBC, 0x00, sizeof(pMsg));
	pMsg.btResult = iResult;

	DataSend(iIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}



void CGReqJewelUnMix(PMSG_REQ_JEWEL_UNMIX * lpMsg, int iIndex)
{
	if ( lpMsg == NULL )
		return;

	CJewelMixSystem::UnMixJewel(iIndex, lpMsg->btJewelType, lpMsg->btJewelLevel, lpMsg->btJewelPos);
}




void GCAnsJewelUnMix(int iIndex, int iResult)
{
	PMSG_ANS_JEWEL_UNMIX pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xBC, 0x01, sizeof(pMsg));
	pMsg.btResult = iResult;

	DataSend(iIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}




void CGReqCrywolfInfo(PMSG_REQ_CRYWOLF_INFO* lpMsg, int iIndex)
{
	if ( !OBJMAX_RANGE(iIndex))
		return;

	LPOBJ lpObj = &gObj[iIndex];
	GCAnsCrywolfInfo(iIndex, g_CrywolfSync.GetOccupationState(), g_CrywolfSync.GetCrywolfState());
}


void GCAnsCrywolfInfo(int iIndex, BYTE btOccupationState, BYTE btCrywolfState)
{
	PMSG_ANS_CRYWOLF_INFO pMsg={0};

	PHeadSubSetB((LPBYTE)&pMsg, 0xBD, 0x00, sizeof(pMsg));
	pMsg.btOccupationState = btOccupationState;
	pMsg.btCrywolfState = btCrywolfState;
	
	DataSend(iIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}



struct PMSG_ANS_CRYWOLF_ALTAR_CONTRACT
{
	PBMSG_HEAD2 h;	// C1:BD:03
	BYTE btResult;	// 4
	BYTE btAltarState;	// 5
	BYTE btObjIndexH;	// 6
	BYTE btObjIndexL;	// 7
};


void CGReqAlatrContract(PMSG_REQ_CRYWOLF_ALTAR_CONTRACT* lpMsg, int iIndex)
{
	int iAltarIndex = MAKE_NUMBERW(lpMsg->btObjIndexH, lpMsg->btObjIndexL);

	if ( !OBJMAX_RANGE(iIndex))
		return;

	if ( !OBJMAX_RANGE(iAltarIndex))
		return;

	LPOBJ lpObj = &gObj[iIndex];
	LPOBJ lpAltarObj = &gObj[iAltarIndex];

	if ( !CRYWOLF_ALTAR_CLASS_RANGE(lpAltarObj->Class))
		return;

	if ( !gObjIsConnected(lpObj))
		return;

	PMSG_ANS_CRYWOLF_ALTAR_CONTRACT pMsg={0};

	PHeadSubSetB((LPBYTE)&pMsg, 0xBD, 0x03, sizeof(pMsg));
	pMsg.btResult = 0;
	pMsg.btAltarState = g_CrywolfNPC_Altar.GetAltarState(lpAltarObj->Class);
	pMsg.btObjIndexH = lpMsg->btObjIndexH;
	pMsg.btObjIndexL = lpMsg->btObjIndexL;

	if ( pMsg.btAltarState == 0 && 
		 lpObj->Type == OBJ_USER &&
		 lpObj->Class == CLASS_ELF &&
		 lpObj->Level >= MIN_ELF_LEVEL_ALTAR )
	{
		if ( g_CrywolfNPC_Altar.SetAltarUserIndex(iAltarIndex, lpAltarObj->Class, iIndex) != FALSE )
		{
			pMsg.btResult = 1;
		}
	}
	else
	{
		MsgOutput(iIndex, lMsg.Get(MSGGET(13, 9)));
	}

	DataSend(iIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}


struct PMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE
{
	PBMSG_HEAD2 h;	// C1:BD:09
	BYTE btPlusChaosRate;	// 4
};



void CGReqPlusChaosRate(PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE* lpMsg, int iIndex)
{
	if ( OBJMAX_RANGE(iIndex) == FALSE )
		return;

	PMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE pMsg={0};

	PHeadSubSetB((LPBYTE)&pMsg, 0xBD, 0x09, sizeof(pMsg));
	pMsg.btPlusChaosRate = g_CrywolfSync.GetPlusChaosRate();

	DataSend(iIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}


void CGReqKanturuStateInfo(PMSG_REQ_KANTURU_STATE_INFO* lpMsg, int iIndex)
{
	if ( OBJMAX_RANGE(iIndex) == FALSE )
		return;

	g_KanturuEntranceNPC.NotifyEntranceInfo(iIndex);
}



void GCReqEnterKanturuBossMap(PMSG_REQ_ENTER_KANTURU_BOSS_MAP* lpMsg, int iIndex)
{
	if ( OBJMAX_RANGE(iIndex) == FALSE )
		return;

	g_KanturuEntranceNPC.NotifyResultEnterKanturuBossMap(iIndex);
}

struct PMSG_NOTIFY_OBJECT_CREATION_STATE
{
	PBMSG_HEAD2 h;
	unsigned char btObjIndexH;
	unsigned char btObjIndexL;
	unsigned char btCreationState;
};

void GCSendObjectCreationState(int iObjectIndex)
{
#if (GS_CASTLE == 1)
	if(OBJMAX_RANGE(iObjectIndex) == FALSE)
		return;

	LPOBJ lpObj = &gObj[iObjectIndex];
	PMSG_NOTIFY_OBJECT_CREATION_STATE pMsg = {0};

	if(lpObj->Class == 278)
	{
		PHeadSubSetB((PBYTE)&pMsg,0xB9,0x01,sizeof(pMsg));
		pMsg.btObjIndexH = SET_NUMBERH(iObjectIndex);
		pMsg.btObjIndexH = SET_NUMBERL(iObjectIndex);
		pMsg.btCreationState = lpObj->m_btCreationState;

		MsgSendV2(lpObj,(PBYTE)&pMsg,sizeof(pMsg));
	}
#endif
}

void CGReqCastleHuntZoneEntrance(PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE * lpMsg,int iIndex)
{
	if(OBJMAX_RANGE(iIndex) == FALSE)
		return;

	LPOBJ lpObj = &gObj[iIndex];

	PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg,0xB9,5,sizeof( pMsg ));

	pMsg.btResult = FALSE;

	int iEnterTaxMoney = 0;
	int bPermission;

	iEnterTaxMoney = g_CastleSiegeSync.GetTaxHuntZone(lpObj->m_Index,TRUE);

	if(iEnterTaxMoney < 0)
		iEnterTaxMoney = 0;

	if(g_CastleSiege.GetHuntZoneEnter() != FALSE)
		bPermission = TRUE;

	if(lpObj->lpGuild != NULL)
	{
		if(g_CastleSiege.CheckCastleOwnerUnionMember(lpObj->m_Index) != FALSE)
			bPermission = TRUE;

		if(g_CastleSiege.CheckCastleOwnerMember(lpObj->m_Index) != FALSE) 
			bPermission = TRUE;
	}
	else if(g_CastleSiege.GetHuntZoneEnter() != FALSE)
	{
		bPermission = TRUE;
	}

	if(bPermission == TRUE)
	{
		if( lpObj->Money >= iEnterTaxMoney &&
			lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			if(gObjMoveGate(lpObj->m_Index,0x5F) == TRUE)
			{
				pMsg.btResult = TRUE;
				int iOldMoney = lpObj->Money;
				lpObj->Money -= iEnterTaxMoney;

				g_CastleSiegeSync.AddTributeMoney(iEnterTaxMoney);

				if(lpObj->Money < 0)
					lpObj->Money = 0;

				GCMoneySend(lpObj->m_Index,lpObj->Money);
				LogAddTD("[Castle HuntZone] [%s][%s] - Entrance TAX : %d - %d = %d",
					lpObj->AccountID,lpObj->Name,iOldMoney,iEnterTaxMoney,lpObj->Money);
			}
		}
	}

	DataSend(iIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void GCReqEnterBarracks(int aIndex)
{
	if ( OBJMAX_RANGE(aIndex) == FALSE )
		return;

	g_QuestInfo.NotifyResultEnterBarracks(aIndex);
}

void GCReqEnterBalgassLair(int aIndex)
{
	if ( OBJMAX_RANGE(aIndex) == FALSE )
		return;

	g_QuestInfo.NotifyResultEnterRefuge(aIndex);
}

struct PMSG_RESULT_MOVEILLUSIONTTEMPLE
{
	PBMSG_HEAD2 h;	// C1:AF:01
	BYTE Result;	// 4
};

void GCReqmoveIllusionTemple(PMSG_REQ_MOVEILLUSIONTEMPLE * lpMsg, int iIndex)
{
	if( OBJMAX_RANGE(iIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	g_IllusionTemple.Enter(iIndex,lpMsg->iTempleNumber - 1,lpMsg->iItemPos);
}

void GCIllusionSkillUseSend(PMSG_ILLUSION_REQ_SKILLUSE * lpMsg,int aIndex)
{
	if( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	WORD TargetIndex = MAKE_NUMBERW(lpMsg->tIndexH,lpMsg->tIndexL);

	g_IllusionTemple.UseSkill(aIndex,MAKE_NUMBERW(lpMsg->MagicH,lpMsg->MagicL),TargetIndex,lpMsg->State);
}


void GCSendEffectInfo(LPOBJ lpObj,int bUseType,int iOptionType,int iEffectType,int iTimeValue)
{
	PMSG_SENDEFFECT pMsg;
	PHeadSetB((LPBYTE)&pMsg,0x2D,sizeof(pMsg));

	pMsg.byEffectOption = bUseType;
	pMsg.wOptionType = iOptionType;
	pMsg.wEffectType = iEffectType;
	pMsg.iLeftTime = iTimeValue;
	pMsg.iEffect = 0;

	if( lpObj->Type == OBJ_USER )
	{
		DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	}
}


struct PMSG_ILLUSION_POINT_SEND
{
	PBMSG_HEAD2 h;
	BYTE Point;
};

void GCIllusionSendPoint(int aIndex,BYTE Point)
{
	PMSG_ILLUSION_POINT_SEND pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xBF,6,sizeof(pMsg));
	pMsg.Point = Point;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void GCSendExp(int aIndex,int AttackIndex,__int64 Exp,int Damage,int Type)
{
	PMSG_KILLPLAYER_EXT pMsg;
	PHeadSetBE((LPBYTE)&pMsg,0x9C,sizeof(pMsg));

	pMsg.NumberH = SET_NUMBERH(AttackIndex);
	pMsg.NumberL = SET_NUMBERL(AttackIndex);
	pMsg.ExpH = (DWORD)Exp >> 16;
	pMsg.ExpL = (DWORD)Exp & 0xFFFF;

#if (CUSTOM_DAMAGE==1)
	pMsg.DamageHW = SET_NUMBERHW(Damage);
	pMsg.DamageLW = SET_NUMBERLW(Damage);
#else
	pMsg.DamageH = SET_NUMBERH(Damage);
	pMsg.DamageL = SET_NUMBERL(Damage);
#endif

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void CGIllusionTempleReward(int aIndex)
{
	if( OBJMAX_RANGE(aIndex) == FALSE )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	g_IllusionTemple.DropReward(aIndex);
}

void CGOpenPCBangShop(LPBYTE aRecv,int aIndex)
{
	if( aRecv == NULL )
		return;

	if( !OBJMAX_RANGE(aIndex) )
		return;
}

struct PMSB_PCBANG_BUYRESULT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btPos;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

void CGBuyPCBangShop(LPBYTE aRecv,int aIndex)
{
	if( aRecv == NULL )
		return;

	if( PacketCheckTime(&gObj[aIndex]) == FALSE )
	{
		PMSB_PCBANG_BUYRESULT pMsg;

		PHeadSubSetB((LPBYTE)&pMsg,0xD0,0x05,sizeof(pMsg));
		pMsg.btResult = 6;
		pMsg.btPos = 0xFF;
		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}
}

void CGReqmoveDoubleGoer(LPBYTE aRecv,int aIndex)
{
	if( aRecv == NULL || !OBJMAX_RANGE(aIndex) )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	if( !gObjIsConnected(aIndex) )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	gDoubleGoer.EnterRequest((PMSG_DOUBLEGOER_REQ_ENTER*)aRecv,aIndex);
}

void CGReqEnterZone(PMSG_REQ_ENTERZONE* lpMsg,int aIndex)
{
	if( !OBJMAX_RANGE(aIndex) )
	{
		LogAdd("return %s %d",__FILE__,__LINE__);
		return;
	}

	if( lpMsg->btType == 1 )
	{
		g_ImperialGuardian.CGEnterPortal(aIndex,0);
	}
}

void CGReqmoveLorenMarket(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use != 1 || lpObj->m_IfState.type != 21 )
		return;
	
	lpObj->m_IfState.use = 0;
	lpObj->m_IfState.type = 0;


	if( gObj[aIndex].MapNumber == 79 )
	{
		gObjMoveGate(aIndex,17);
	}
	else
	{
		gObjMoveGate(aIndex,333);
	}
}

struct PMSG_ANS_MOVEACHERON
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

void CGReqMoveAcheron(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if( lpObj->m_IfState.use != 1 || lpObj->m_IfState.type != 21 )
		return;
	
	lpObj->m_IfState.use = 0;
	lpObj->m_IfState.type = 0;

	int bSpiritMap = FALSE;

	for(int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++)	{
		if( lpObj->pInventory[i].IsItem() &&
			lpObj->pInventory[i].m_Type == ITEMGET(13,146) )
		{
			bSpiritMap = TRUE;

			gObjInventoryItemSet(aIndex,i,(BYTE)-1);
			lpObj->pInventory[i].Clear();
			GCInventoryItemDeleteSend(aIndex,i,1);
			break;
		}
	}


	if( !bSpiritMap )
	{
		PMSG_ANS_MOVEACHERON pMsg;
		PHeadSubSetB((LPBYTE)&pMsg,0xF8,0x21,sizeof(pMsg));
		pMsg.btResult = 1;

		DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
		return;
	}

	gObjMoveGate(aIndex,417);


	/*if( gObj[aIndex].MapNumber == 79 )
	{
		gObjMoveGate(aIndex,17);
	}
	else
	{
		gObjMoveGate(aIndex,333);
	}*/
}

void CGWindowReqMove(PMSG_MOVE_REQUEST* lpMsg,int aIndex)
{
	if ( !OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if( !gObjIsConnected(aIndex) )
		return;


	if (g_SelfDefenseOn)
	{
		if ((GetTickCount() - gObj[aIndex].MySelfDefenseTime) < g_SelfDefenseTime * 1000) 
		{
			GCServerMsgStringSend(lMsg.Get(1133),aIndex,1);
			return;
		}
	}

	LPOBJ lpObj = &gObj[aIndex];
	int movereq = lpMsg->wMoveNumber;

	PMSG_MOVE_ANSWER pMsg;
	PHeadSetB((LPBYTE) &pMsg,0x8E,sizeof(pMsg));

	
	LogAdd("PMSG check: %d", lpMsg->CHECK);
	pMsg.CHECK = lpMsg->CHECK;

	/*if (lpObj->m_bPShopOpen)
	{
		::GCServerMsgStringSend("Please close your shop before changing locations", aIndex, 1);
		pMsg.btResult = 1;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	} */

	if ( lpObj->Teleport != 0 )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(6, 68)), lpObj->m_Index, 1);
		return;
	}


	if ( (lpObj->m_IfState.use) != 0 )
	{
		LogAdd("[CGWindow req move] m_ifState.use != 0");
		if ( lpObj->m_IfState.type  == 3 )
		{
			lpObj->TargetShopNumber = -1;
			lpObj->m_IfState.type = 0;
			lpObj->m_IfState.use = 0;
		}
	}

	if ( lpObj->m_IfState.use > 0 )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(6, 68)), lpObj->m_Index, 1);
		return;
	}
			
	if ( lpObj->IsInBattleGround != false )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(6, 68)), lpObj->m_Index, 1);
		return;
	}

	//Change to allow PK warping
	if ( lpObj->m_PK_Level >= 6)
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4, 101)), lpObj->m_Index, 1);
		return;
	}

	
	pMsg.btResult = gMoveCommand.Move(&gObj[aIndex], movereq);
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}

void GCMonkMagicAttackNumberSend(LPOBJ lpObj,int MagicNumber,int usernumber,int skillsuccess)
{
	PMSG_MAGICATTACK_RESULT pAttack;
	PHeadSetBE((LPBYTE)&pAttack, 0x4A, sizeof(pAttack));

	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);

	pAttack.SourceNumberH = SET_NUMBERH(lpObj->m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(lpObj->m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(usernumber);
	pAttack.TargetNumberL = SET_NUMBERL(usernumber);
	pAttack.TargetNumberH &= 0x7F;

	if ( skillsuccess )
		pAttack.TargetNumberH |= 0x80;

	if ( lpObj->Type == OBJ_USER )
		DataSend(lpObj->m_Index, (LPBYTE)&pAttack, pAttack.h.size);
	
	MsgSendV2(lpObj, (LPBYTE)&pAttack, pAttack.h.size);
}

void GCMonkMagicAttack(PMSG_MAGICATTACK* lpMsg,int aIndex)
{
	LPOBJ lpObj;
	LPOBJ lpTargetObj;
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CMagicInf* lpMagic;


	if( gObj[aIndex].CloseCount >= 0 )
		return;

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);

	if( MagicNumber == AT_SKILL_RAGEFIGHTER_DARKSIDE ||
		MagicNumber == AT_MSKILL_RF_DARKSIDE1 ||
		MagicNumber == AT_MSKILL_RF_DARKSIDE2 )
	{
		GCMonkMagicAttackNumberSend(&gObj[aIndex],MagicNumber,gObj[aIndex].m_wDarkSideTarget[0],TRUE);
		return;
	}

	if( usernumber < 0 || usernumber > OBJMAX-1 )
	{
		LogAdd("error :%s %d %d",__FILE__,__LINE__,usernumber);
		return;
	}

	if( gObj[aIndex].Live == FALSE || 
		gObj[aIndex].Connected <= PLAYER_LOGGED )
	{
		return;
	}

	lpObj = &gObj[aIndex];
	lpTargetObj = &gObj[usernumber];

	int attackret = gObjCheckAttackArea(aIndex,usernumber);

	if( attackret != 0 )
	{
		LogAddTD("[%s][%s] Try Attack In Not Attack Area [Protocol] (%d:%d,%d) errortype = %d",
			lpObj->AccountID,lpObj->Name,lpObj->MapNumber,lpObj->X,lpObj->Y,attackret);

		if( lpObj->Class == 275 )
			lpObj->TargetNumber = -1;
		if( bIsIgnorePacketSpeedHackDetect != 0 )
			return;
	}

	if ( lpObj->Type == OBJ_USER )
	{
		lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);

		if ( !lpMagic )
			return;

		if ( MagicDamageC.SkillGetRequireClass(lpObj->Class, lpObj->ChangeUP, lpObj->ChangeUP2 , lpMagic->m_Skill) < 1 )
			return;

		if ( MagicDamageC.CheckStatus(lpMagic->m_Skill, lpObj->GuildStatus) == 0 )
		{
			LogAddC(2, "[0x4A] CGRageAttack() - Invalid Status");
			return;
		}

		if ( MagicDamageC.CheckKillCount(lpMagic->m_Skill, lpObj->m_btKillCount) < 0 )
		{
			LogAddC(2, "[0x4A] CGRageAttack() - Invalid KillCount");
			return;
		}
	}
	else
	{
		lpMagic = gObjGetMagic(lpObj, MagicNumber);

		if ( !lpMagic )
			return;
	}

	if ( !gCheckSkillDistance(aIndex, usernumber, lpMagic->m_Skill))
	{
		DWORD dwTick = GetTickCount() - lpObj->m_dwSkillDistanceErrorTick;
		
		if ( dwTick > (g_iSkillDiatanceKickCheckTime*1000) )
		{
			lpObj->m_iSkillDistanceErrorCount = 0;
			lpObj->m_dwSkillDistanceErrorTick = GetTickCount();
		}

		lpObj->m_iSkillDistanceErrorCount++;

		if ( lpObj->m_iSkillDistanceErrorCount > g_iSkillDistanceKickCount )
		{
			if ( g_iSkillDistanceKick )
			{
				LogAddTD("[SKILL DISTANCE CHECK] [%s][%s] Kick Invalid Skill Area User. count(%d)",
					lpObj->AccountID, lpObj->Name, lpObj->m_iSkillDistanceErrorCount);

				CloseClient(lpObj->m_Index);
				return;
			}

			lpObj->m_iSkillDistanceErrorCount=0;
		}

		return;
	}
	GCMonkMagicAttackNumberSend(&gObj[aIndex],MagicNumber,usernumber,TRUE);
}

void GCMonkDarkSideTargetSelect(PMSG_MONK_DARKSIDE_RECV *lpMsg, int aIndex)
{
	WORD wMagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH,lpMsg->MagicNumberL);
	WORD wTargetIndex = MAKE_NUMBERW(lpMsg->NumberH,lpMsg->NumberL);
	WORD wTargetList[5];

	int i;
	for(i = 0; i < 5; i++)
	{
		wTargetList[i] = 10000;
	}

	if( gObj[aIndex].CloseCount >= 0 )
		return;

	CMagicInf* lpMagic = NULL;
	lpMagic = gObjGetMagicSearch(&gObj[aIndex],wMagicNumber);

	if( lpMagic == NULL )
		return;

	if( !gObjUseSkill.SkillMonkDarkSideGetTargetIndex(aIndex,wTargetIndex,lpMagic,&wTargetList[0]) )
		return;

	PMSG_MONK_DARKSIDE_ANS pDarkSideTargetList;
	memset(&pDarkSideTargetList,0,sizeof(pDarkSideTargetList));
	PHeadSetB((LPBYTE)&pDarkSideTargetList,0x4B,sizeof(pDarkSideTargetList));
	memcpy(pDarkSideTargetList.Target,wTargetList,sizeof(wTargetList));
	pDarkSideTargetList.MagicNumber = wMagicNumber;
	gObj[aIndex].m_btDarkSideTargetCount = 0;

	for(i = 0; i < 5; i++)
	{
		if( wTargetList[i] != 10000 )
		{
			gObj[aIndex].m_btDarkSideTargetCount++;
		}
	}

	memcpy(gObj[aIndex].m_wDarkSideTarget,wTargetList,sizeof(wTargetList));
	DataSend(aIndex,(LPBYTE)&pDarkSideTargetList,pDarkSideTargetList.h.size);
}

void CGSaveMuBotSettings(PMSG_MUBOT_SETTINGS_RECV* lpMsg,int aIndex)
{
	if( !gObjIsConnectedGP(aIndex) )
		return;

	DGMuBotSettingsSend(aIndex,gObj[aIndex].Name,&lpMsg->btDATA[0]);

	LogAddL("[MuBot] Option Save [%s][%s]",
		gObj[aIndex].AccountID,gObj[aIndex].Name);
}

void GCSendMuBotSettings(int aIndex,BYTE* lpData)
{
	if( !gObjIsConnected(aIndex) )
		return;

	PMSG_MUBOT_SETTINGS_RECV pMsg;
	PHeadSetW((LPBYTE)&pMsg,0xAE,sizeof(pMsg));

	pMsg.btType = 0;
	memcpy(pMsg.btDATA,lpData,sizeof(pMsg.btDATA));

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));

	LogAddL("[MuBot] Option Send [%s][%s]",
		gObj[aIndex].AccountID,gObj[aIndex].Name);
}

void CGRequestStartMuBot(PMSG_MUBOT_REQ_START* lpMsg,int aIndex)
{
	if (g_bMuBotSystemOn == FALSE) {
		return;
	}

	if (!gObjIsConnectedGP(aIndex)) {
		return;
	}

	PMSG_MUBOT_USE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x51, sizeof(pMsg));
	pMsg.Money = 0;
	pMsg.Time = 0;
	pMsg.Status = 1;

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->m_IfState.use == TRUE) {
		return;
	}

	if (lpObj->Level < g_iMuBotSystemUserLevel) {
		MsgOutput(aIndex, "Only characters above %d level can run MU Helper", g_iMuBotSystemUserLevel);
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}
	
	if (lpObj->MapNumber == MAP_INDEX_CASTLESIEGE
		|| lpObj->MapNumber == MAP_INDEX_PKFIELD) {
		MsgOutput(aIndex, "MU Helper cant run on this location", g_iMuBotSystemUserLevel);
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->m_bMUBOT = FALSE;
		return;
	}

	if (lpMsg->btType == 0) {
		if (lpObj->Money < g_iMuBotStage1Zen) {
			pMsg.Status = 2;
		}
		else {
			lpObj->Money -= g_iMuBotStage1Zen;
			GCMoneySend(lpObj->m_Index,lpObj->Money);
			pMsg.Status = 0;
			pMsg.Time = 0;
			pMsg.Money = g_iMuBotStage1Zen;
			lpObj->m_bMUBOT = TRUE;
		}
	} else {
		lpObj->m_bMUBOT = FALSE;
	}

	lpObj->m_iMUBOT_TIME = 0;
	lpObj->m_iMUBOT_STAGE = 0;
	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

struct PMSG_ATTACKSPEED
{
	PBMSG_HEAD2 h;
	int iAttackSpeed;
	int iMagicSpeed;
};

void GCAttackSpeedSend(int aIndex)
{
	if( !gObjIsConnected(aIndex) )
		return;

	PMSG_ATTACKSPEED pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xEC,0x30,sizeof(pMsg));

	pMsg.iAttackSpeed = gObj[aIndex].m_AttackSpeed;
	pMsg.iMagicSpeed = gObj[aIndex].m_MagicSpeed;

	DataSend(aIndex,(LPBYTE)&pMsg,pMsg.h.size);
}

void CGEnableUsePolymorphSkinrRecv(PMSG_REQ_USE_POLYMORPH_SKIN* lpMsg, int aIndex)	//1.01.06
{
	PMSG_MOVE pMove = { 0 };
	// ---
	if( lpMsg->EnableUsePolymorphSkin == 1 )
	{
		if( IsTransformCharcterSkin(gObj[aIndex].m_Change) )
		{
		  gObj[aIndex].m_EnableUseChangeSkin = 0;
		}
	 }
	 else
	 {
		gObj[aIndex].m_EnableUseChangeSkin = 1;
	 }
	 // ----
	 pMove.h.c			= 0xC1;
	 pMove.h.headcode	= 0xD7;
	 pMove.h.size		= 0xD;
	 pMove.X			= gObj[aIndex].X;
	 pMove.Y			= gObj[aIndex].Y;
	 PMoveProc(&pMove, aIndex);
	 LogAddTD("[UseCahngeSkinState][CGEnableUsePolymorphSkinrRecv] [ChaageEanbleState : %d] On(Use): 0 / Off: 1(Don't Use) ",
		gObj[aIndex].m_EnableUseChangeSkin, gObj[aIndex].AccountID, gObj[aIndex].Name);
	 gObjViewportListProtocolCreate(&gObj[aIndex]);
}

//S8 Emulator OK
void CGInventoryEquipment(_tagPMSG_REQ_INVENTORY_EQUIPMENT_ITEM * lpMsg, int iIndex) // leee Added
{
	if( !OBJMAX_RANGE(iIndex) )
	{
		return;
	}
	// ----
	LPOBJ lpObj = &gObj[iIndex];
	// ----
	if( !gObjIsConnected(iIndex) )
	{
		return;
	}
	// ----
	_tagPMSG_ANS_INVENTORY_EQUIPMENT_ITEM pMsg = { 0 };
	pMsg.btResult = 0;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x20, sizeof(_tagPMSG_ANS_INVENTORY_EQUIPMENT_ITEM));
	pMsg.btItemPos = lpMsg->btItemPos;
	// ----
	if( lpObj->pInventory[lpMsg->btItemPos].m_Durability == 0.0 )
	{
		pMsg.btResult = -1;
		DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}
	// ----
	switch( lpObj->pInventory[lpMsg->btItemPos].m_Type )
	{
	case ITEMGET(13, 128):
	case ITEMGET(13, 129):
		{
			if( !lpObj->m_btSculptPos )
			{
				if( lpMsg->btValue == 254 )
				{
					if( lpObj->pInventory[lpMsg->btItemPos].m_Durability == 255.0 )
					{
						lpObj->m_btSculptPos = lpMsg->btItemPos;
						lpObj->pInventory[lpMsg->btItemPos].m_Durability = 254.0;
						pMsg.btResult = -2;
					}
				}
				break;
			}
			// ----
			if(		lpObj->m_btSculptPos < INVETORY_WEAR_SIZE 
				||	lpObj->m_btSculptPos >= MAIN_INVENTORY_SIZE )
			{
				break;
			}
			// ----
			if(		lpMsg->btValue			== 255
				&&	lpObj->m_btSculptPos	== lpMsg->btItemPos )
			{
				if( lpObj->pInventory[lpMsg->btItemPos].m_Durability == 254.0 )
				{
					lpObj->pInventory[lpMsg->btItemPos].m_Durability = 255.0;
					pMsg.btResult = -1;
				}
				break;
			}
			// ----
			GCServerMsgStringSend(lMsg.Get(3480), lpObj->m_Index, 1);
            DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		}
		break;
		// --
	case ITEMGET(13, 130):
	case ITEMGET(13, 131):
	case ITEMGET(13, 132):
	case ITEMGET(13, 133):
		{
			if( !lpObj->m_btCharmPos )
			{
				if( lpMsg->btValue == 254 )
				{
					if( lpObj->pInventory[lpMsg->btItemPos].m_Durability == 255.0 )
					{
						lpObj->m_btCharmPos = lpMsg->btItemPos;
						lpObj->pInventory[lpMsg->btItemPos].m_Durability = 254.0;
						pMsg.btResult = -2;
					}
				}
				break;
			}
			// ----
			if(		lpObj->m_btCharmPos < INVETORY_WEAR_SIZE 
				||	lpObj->m_btCharmPos >= MAIN_INVENTORY_SIZE )
			{
				break;
			}
			// ----
			if(		lpMsg->btValue		== 255
				&&	lpObj->m_btCharmPos == lpMsg->btItemPos )
			{
				if( lpObj->pInventory[lpMsg->btItemPos].m_Durability == 254.0 )
				{
					lpObj->pInventory[lpMsg->btItemPos].m_Durability = 255.0;
					pMsg.btResult = -1;
				}
				break;
			}
			// ----
			GCServerMsgStringSend(lMsg.Get(3480), lpObj->m_Index, 1);
            DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		}
		break;
		// --
	case ITEMGET(13, 134):
		{
			if( !lpObj->m_btArtifactPos )
			{
				if( lpMsg->btValue == 254 )
				{
					if( lpObj->pInventory[lpMsg->btItemPos].m_Durability == 255.0 )
					{
						lpObj->m_btArtifactPos = lpMsg->btItemPos;
						lpObj->pInventory[lpMsg->btItemPos].m_Durability = 254.0;
						pMsg.btResult = -2;
					}
				}
				break;
			}
			// ----
			if(		lpObj->m_btArtifactPos < INVETORY_WEAR_SIZE 
				||	lpObj->m_btArtifactPos >= MAIN_INVENTORY_SIZE )
			{
				break;
			}
			// ----
			if(		lpMsg->btValue			== 255
				&&	lpObj->m_btArtifactPos	== lpMsg->btItemPos )
			{
				if( lpObj->pInventory[lpMsg->btItemPos].m_Durability == 254.0 )
				{
					lpObj->pInventory[lpMsg->btItemPos].m_Durability = 255.0;
					pMsg.btResult = -1;
				}
				break;
			}
			// ----
			GCServerMsgStringSend(lMsg.Get(3480), lpObj->m_Index, 1);
            DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		}
		break;
	}
	// ----
	gObjCalCharacter(iIndex);
	DataSend(iIndex,(LPBYTE)&pMsg, pMsg.h.size);
}

void ClientTestSend(int UserIndex, ClientTest* lpResult)
{
	LogAddC(2, "[DEBUG] Text: %s, Value: %d",
		lpResult->Text, lpResult->Value);
}

#if (ENABLETEST_NEWPVP == 1)

#endif