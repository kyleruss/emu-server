// ----------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------
#include "user.h"
// ----------------------------------------------------------------

#pragma pack(push, 1)
struct PMSG_VAULT_INFO
{
	PBMSG_HEAD2 h;
	int aIndex;
	char chAccountID[11];
	BYTE VaultID;
};
#pragma pack(pop)
// ----------------------------------------------------------------

#pragma pack(push, 1)
struct PMSG_VAULT_RESULT
{
	PBMSG_HEAD2 h;
	int aIndex;
	BYTE Result;
};
#pragma pack(pop)
// ----------------------------------------------------------------

class WareHouseSYSTEM
{
	public:
		WareHouseSYSTEM();
		~WareHouseSYSTEM();

		BOOL CheckRange(int aIndex);
		void DBSendVaultResult(PMSG_VAULT_RESULT * lpMsg);
		void DBSendVaultInfo(int aIndex, char * AccountID, BYTE VaultID);

};
extern WareHouseSYSTEM gWareHouseSYSTEM;