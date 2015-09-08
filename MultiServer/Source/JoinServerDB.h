#ifndef __JOINSERVERDB_H__
#define __JOINSERVERDB_H__

#include "CQuery.h"
#include "JoinServer.h"

int IsUser(char *szId, char *pass, char * JoominNumber, char & Block, int & DBNumber);
void MembStat_Connect(char * AccountId, char *ServerName, char *Ip);
void MembStat_Disconnect(char * AccountId);
void AccountBlock(char* szAccountId,int blockcode);
int CreateHeart();
int HeartEventAll(int heart_count, char *id, char *server, char *name);
int LoveHeartEventCheck(int number);
int HeartEventLuck(int heart_count, char *id, char *server, char *name);
int CreateHeartAll();
int GetBill(int iIndex,char* szId,SDHP_BILLSEARCH_RESULT * lpMsg);
void RemoveBill(char* szId);

void UpdateClientIp(char * szAccount,char* szIp);

extern CQuery g_JoinServerDB;
extern CQuery g_JoinServerConnectDB;

#endif