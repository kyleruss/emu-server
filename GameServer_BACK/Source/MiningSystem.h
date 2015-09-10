#if !defined(__MININGSYSTEM_H__)
#define __MININGSYSTEM_H__

#include "..\common\winutil.h"

#define MAX_MINING_LEVEL	5
#define MINING_OBJECT(X) ( (X >= 605 && X <= 607) ? TRUE : FALSE )
#define MINING_LEVEL(X) ( (X >= 0 && X < MAX_MINING_LEVEL) ? TRUE : FALSE )

struct PMSG_MINING_REQUEST
{
	PBMSG_HEAD2 h;	// C1:4C:00
	WORD aIndex;
	WORD tIndex;
	WORD tClass;
	BYTE MapNumber;
	BYTE Level;
};

struct PMSG_MINING_ANSWER
{
	PBMSG_HEAD2 h;	// C1:4C:00
	WORD aIndex;
	WORD tIndex;
	WORD tClass;
	BYTE MapNumber;
	BYTE Level;
	int iCount;
	int iResult;
};

struct PMSG_MINING_END
{
	PBMSG_HEAD2 h;	// C1:4C:03
	WORD aIndex;
	WORD tIndex;
	BYTE MapNumber;
};

struct PMSG_MINING_REWARD_REQUEST
{
	PBMSG_HEAD2 h;	// C1:4C:01
	WORD aIndex;
	WORD tIndex;
	WORD tClass;
	BYTE MapNumber;
	BYTE Level;
	BYTE bSuccess;
};

struct PMSG_MINING_REWARD_ANSWER
{
	PBMSG_HEAD2 h;	// C1:4C:01
	WORD aIndex;
	WORD tIndex;
	WORD tClass;
	BYTE MapNumber;
	BYTE Level;
	int iResult;
};

void CGMiningRequest(PMSG_MINING_REQUEST* lpMsg,int aIndex);
void CGRewardRequest(PMSG_MINING_REWARD_REQUEST* lpMsg,int aIndex);
void CGMiningEnd(PMSG_MINING_END* lpMsg,int aIndex);

struct _MINING_JEWEL
{
	_MINING_JEWEL()
	{
		iJewelID = -1;
		iPackID = -1;
	}

	int iJewelID;
	int iPackID;
};

struct _MINING_LEVEL
{
	_MINING_LEVEL()
	{
		iRate = 0;
		iCount = 0;
	}
	int iRate;
	int iCount;
};

struct _MINING_OBJECT
{
	_MINING_OBJECT()
	{
		iFinalRate = 0;
		iFinalCount = 0;
		iDurDec = 0;
	}

	int iFinalRate;
	int iFinalCount;
	int iDurDec;
	_MINING_LEVEL Level[MAX_MINING_LEVEL];
};

class CMiningSystem
{
public:
	CMiningSystem();
	virtual ~CMiningSystem();

public:
	void Init();
	void LoadData(char* script_file);

	void SetEnabled(bool bEnabled) { this->m_bEnabled = bEnabled; };
	bool GetEnabled(bool bEnabled) { return this->m_bEnabled; };

	_MINING_OBJECT* GetObject(int ObjClass);

	void ResetMiningInfo(LPOBJ lpObj);
	int CheckMining(LPOBJ lpObj,int ObjId,int ObjClass,int Level);
	void MiningRequest(LPOBJ lpObj,int ObjId,int ObjClass,int Level);
	void RewardRequest(LPOBJ lpObj,int ObjId,int ObjClass,int Level);
	void GetRewardCount(int count,int div,int& itemcount,int& left);

private:
	bool m_bEnabled;
	bool m_bLoaded;

	TRandomPoolMgr m_JewelRnd;
	std::vector<_MINING_JEWEL> m_vtJewelList;
	std::map<int,_MINING_OBJECT> m_mapObjectList;
};

extern CMiningSystem g_MiningSystem;

#endif