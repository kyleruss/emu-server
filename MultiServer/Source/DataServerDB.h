#ifndef __DATASERVERDB_H__
#define __DATASERVERDB_H__
#include "CQuery.h"
#include "DataServer.h"

#define ITEMGET(x,y) (x*512+y)
#define MAX_CHARS	5

extern CQuery g_DataServerDB;

int GetAllChar(char* szId, char * szChar,int& ExVault);
int GetItemCount(int svrgroup);
int GetCharList(char* szId,LPSDHP_CHARLISTCOUNT pCount,LPSDHP_CHARLIST pCL);
int CreateCharacter(char * szId, char * szName,LPSDHP_CREATECHARRESULT lpResult);
int DeleteCharacter(char* szId,char* szName,char* szGuild,int G_Status);
int GetCharacterInfo(char* szAccountID,char* szName,LPSDHP_DBCHAR_INFORESULT lpResult);
void GetOptionData(char* szName,LPSDHP_SKILLKEYDATA_SEND lpResult);
void SetCharacterInfo(char* szName,LPSDHP_DBCHAR_INFOSAVE lpMsg);
int GetWarehouseData(char* szId,LPSDHP_GETWAREHOUSEDB_SAVE lpMsg);
void CreateWarehouseData(char* szId);
void SetWarehouseData(char* szId,LPSDHP_GETWAREHOUSEDB_SAVE lpMsg);
void SetInventoryData(char* szName,LPSDHP_DBCHAR_ITEMSAVE lpMsg);
DWORD GetNewSerial();
DWORD GetNewPet();
void GetPetInfo(LPRecv_PetItem_Info lpPet);
void SetPetInfo(LPSave_PetItem_Info lpPet);
void SetOptionData(char* szName,LPSDHP_SKILLKEYDATA lpData);
void GetCastleOwner(int wMapServerGroup,LPCSP_ANS_OWNERGUILDMASTER pResult);
int CastleNpcBuy(LPCSP_REQ_NPCBUY lpNpcInfo);
int CastleNpcRepair(LPCSP_REQ_NPCREPAIR lpNpcInfo,LPCSP_ANS_NPCREPAIR pResult);
int CastleNpcUpgrade(LPCSP_REQ_NPCUPGRADE lpNpcInfo);
int GetTaxInfo(int iMapServerGroup,LPCSP_ANS_TAXINFO lpResult);
int SetTaxInfo(LPCSP_REQ_TAXRATECHANGE lpInfo,LPCSP_ANS_TAXRATECHANGE lpResult);
int SiegeDateChange(LPCSP_REQ_SDEDCHANGE lpDateInfo);
int GuildMarkRegInfo(int iMapSvrGroup,char* szGuild,LPCSP_ANS_GUILDREGINFO lpResult);
int SiegeEndedChange(int iMapSvrGroup, int bSiegeEnded);
int CastleOwnerChange(int iMapSvrGroup, int bOccupied, char* szGuildName);
int RegAttackGuild(int iMapSvrGroup,char* szGuildName);
int RestartCastleState(int iMapSvrGroup);
int RegGuildMark(int iMapSvrGroup,char* szGuildName,int * G_Mark);
int GuildMarkReset(int iMapSvrGroup,char* szGuildName,int * G_Mark);
int GuildSetGiveUp(LPCSP_REQ_GUILDSETGIVEUP lpInfo,char* szGuildName,int * G_Mark);
int NpcRemove(int iMapSvrGroup,int NpcNumber,int NpcIndex);
int CastleTributeMoney(int iMapSvrGroup,int Money,__int64 * RetMoney);
int ResetCastleTaxInfo(int iMapSvrGroup);
int ResetSiegeGuildInfo(int iMapSvrGroup);
int ResetRegSiegeInfo(int iMapSvrGroup);
int GetCastleTotalInfo(int iMapSvrGroup,int CastleCycle,LPCSP_ANS_CSINITDATA lpMsg);
int GetCastleNpcInfo(int iMapSvrGroup,LPCSP_CSINITDATA lpMsgBody);
int GetAllGuildMarkRegInfo(int iMapSvrGroup,LPCSP_GUILDREGINFO lpMsgBody);
int FirstCreateNPC(int iMapSvrGroup,int count,LPCSP_NPCSAVEDATA lpNpc);
int GetCalcRegGuildList(int iMapSvrGrou,LPCSP_CALCREGGUILDLIST lpMsgBody);
int GetCsGuildUnionInfo(int count,LPCSP_CSGUILDUNIONINFO lpInfo,LPCSP_CSGUILDUNIONINFO lpOutInfo);
int SetSiegeGuildInfo(int iMapSvrGroup,int count,LPCSP_CSSAVETOTALGUILDINFO lpMsgBody);
int CsLoadTotalGuildInfo(int iMapSvrGroup,LPCSP_CSLOADTOTALGUILDINFO lpMsgBody);
int CastleNpcUpdate(int iMapSvrGroup,int count,LPCSP_NPCUPDATEDATA lpMsgBody);
int CrywolfInfoLoad(int iMapSvrGroup,LPCWP_ANS_CRYWOLFINFOLOAD lpResult);
int CrywolfInfoSave(int iMapSvrGroup,int iOccupy,int iState);
int InsertPeriodItem(LPMSG_REQ_PERIODITEM_INSERT lpMsg);
int GetPeriodItemList(int Guid,char* szName,LPERIODITEM_LIST lpItems);
int UpdatePeriodItem(LPMSG_REQ_PERIODITEM_UPDATE lpMsg);
int DeletePeriodItem(int Guid, char* szName);
int GetMasterInfo(LPMSG_ANS_MASTEREINFO lpMsg,char* szName);
int GetMonsterKillInfo(LPMSG_ANS_MONSTERKILLINFO lpMsg,char* szName);
int SaveMonsterKillInfo(LPMSG_SAVE_MONSTERKILLINFO lpMsg,char* szName);
int SaveMasterInfo(LPMSG_SAVE_MASTERINFO lpMsg,char* szName);
int GetSummonerCreate(PMSG_ANS_SUMMONER_CREATE * lpMsg,char* szAccountID);
int SaveSummonerCreate(char* szId);
void UpdateGameIDC(char* szId,char* szName);

void UpdateQuestInfoEx(PMSG_REQ_QUESTEXINFO_SAVE* szInfo,char*szName);
void ReadQuestInfoEx(char* szName,PMSG_ANS_QUESTEXINFO* lpMsg);

void JoinGens(char* szName,char* szMasterName,int iGens,PMSG_ANS_GENS_JOINREQUEST* lpMsg);
void GetGensInfo(char* szName,PMSG_ANS_GENS_INFO* pMsg);
void UpdateGens(char* szName,int iContribution,int iReward);
void LeaveGens(char* szName,int iGens,PMSG_ANS_LEAVE_GENS* lpMsg);
void GensServerInfo(int iServerCode,int iUpdateDays,int iPrizeDays,PMSG_ANS_GENS_SERVERINFO* lpMsg);

void ExpandWarehouse(char* szAccountID,int count);
void ExpandInventory(char* szAccountID,char* szName,int count);
void MuBotSaveOption(char* szName,LPMUBOT_SETTINGS_REQ_SAVE lpMsg);
void GetMuBotData(char* szName,LPMUBOT_SETTINGS_SEND lpMsg);

void GetErtelList(char* szAccount,char* szName,LPMSG_ANS_ERTELLIST lpMsg);
void SaveErtelList(char* szAccount,char* szName,LPMSG_SAVE_ERTELLIST lpMsg);

void UpdateDropStatistics();

void AchievUpdateMonsterData(BYTE Type, short MonsterID, LPSTR UserName);
void AchievUpdateEventData(BYTE Type, LPSTR UserName, AchievGDUpdateEvent* lpData);
void AchievUpdateMixData(BYTE Type, LPSTR UserName, bool Success);
#endif