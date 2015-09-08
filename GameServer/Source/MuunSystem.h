#pragma once

#include "..\\include\\prodef.h"
#include "MuunSystemProtocol.h"

#define MUUN_DEBUG			FALSE
#define MUUN_INVENTORY_SIZE	62
#define MUUN_MAX_GRADE		6
#define MUUN_MAX_ACTIVEUSE	2
#define MUUN_MAX_INFO		100

// feature impletation state:
/*
	<OK> Decompiled class
	<> Decompiled parts
	<3> Compared calls to global
	missing: http://prntscr.com/6cy3wa, http://prntscr.com/6cy40t, http://prntscr.com/6cy4vm
	<> Compared calls to user object info
	<> Compared zzziteam info for new options
	<> Database connected
*/

// muun option info impletation
class CMuunOpt {
public:
	CMuunOpt() {};
	virtual ~CMuunOpt() {};
	void SetOptType(int iOptType) { m_iOptType = iOptType; };
	int GetOptType() { return m_iOptType; };
	void SetMuunLvVal(int* iMuunLvVal) { memcpy(m_iMuunLvVal, iMuunLvVal, sizeof(m_iMuunLvVal)); };
	int GetMuunLvVal(int iMuunLv) { return m_iMuunLvVal[iMuunLv]; };
    void GetMuunLvVal(int* iOutMuunLvVal) { iOutMuunLvVal = m_iMuunLvVal; };
	void SetConditionType(int ConditionType) { m_iConditionType = ConditionType; };
    int GetConditionType() { return m_iConditionType; };
    void SetConditionVal1(int iConditionVal1) { m_iConditionVal1 = iConditionVal1; };
    int GetConditionVal1() { return m_iConditionVal1; };
    void SetConditionVal2(int iConditionVal2) { m_iConditionVal2 = iConditionVal2; };
    int GetConditionVal2() { return m_iConditionVal2; };
	void Clear() {
		m_iOptType = 0;
		for (int i = 0; i < MUUN_MAX_GRADE; i++) {
			m_iMuunLvVal[i] = 0;
		}
		m_iConditionType = 0;
		m_iConditionVal1 = 0;
		m_iConditionVal2 = 0;
	}
private:
    int m_iOptType;
    int m_iMuunLvVal[MUUN_MAX_GRADE];
    int m_iConditionType;
    int m_iConditionVal1;
    int m_iConditionVal2;
};

// muun info impletation
class CMuunInfo : public CMuunOpt {
public:
	CMuunInfo() { m_iOptIndex = -1; };
	virtual ~CMuunInfo() {};
	void SetIndex(int iIndex) { m_iIndex = iIndex; };
	int GetIndex() { return m_iIndex; };
	void SetItemNum(int iItemNum) { m_iItemNum = iItemNum; };
	int GetItemNum() { return m_iItemNum; };
	void SetMuunType(int iMuunType) { m_iMuunType = iMuunType; };
	int GetMuunType() { return m_iMuunType; };
	void SetMuunRank(int iMuunRank) { m_iMuunRank = iMuunRank; };
	int GetMuunRank() { return m_iMuunRank; };
	void SetOptIndex(int iOptIndex) { m_iOptIndex = iOptIndex; };
	int GetOptIndex() { return m_iOptIndex; };
	void SetAddOptType(int iAddOptType) { m_iAddOptType = iAddOptType; };
	int GetAddOptType() { return m_iAddOptType; };
	void SetAddOptVal(int iAddOptVal) { m_iAddOptVal = iAddOptVal; };
	int GetAddOptVal() { return m_iAddOptVal; };
	void SetAddOptStart(long lAddOptStart) { m_lAddOptStart = lAddOptStart; };
	long GetAddOptStart() { return m_lAddOptStart; };
	void SetAddOptEnd(long lAddOptEnd) { m_lAddOptEnd = lAddOptEnd; };
	long GetAddOptEnd() { return m_lAddOptEnd; };
	void SetEvolutionMuunNum(int iEvolutionMuunNum) { m_iEvolutionMuunNum = iEvolutionMuunNum; };
	int GetEvolutionMuunNum() { return m_iEvolutionMuunNum; };
	void Clear() {
		CMuunOpt::Clear();
		m_iIndex = 0;
		m_iItemNum = 0;
		m_iMuunType = 0;
		m_iMuunRank = 0;
		m_iOptIndex = 0;
		m_iAddOptType = 0;
		m_iAddOptVal = 0;
		m_lAddOptStart = 0;
		m_lAddOptEnd = 0;
		m_iEvolutionMuunNum = 0;
	}
private:
    int m_iIndex;
    int m_iItemNum;
    int m_iMuunType;
    int m_iMuunRank;
    int m_iOptIndex;
    int m_iAddOptType;
    int m_iAddOptVal;
    long m_lAddOptStart;
    long m_lAddOptEnd;
    int m_iEvolutionMuunNum;
};

// muun info manager impletation
class CMuunInfoMng {
public:
	CMuunInfoMng() {};
	~CMuunInfoMng() {};
    bool LoadScriptMuunSystemInfo(char* lpszFileName);
    bool LoadScriptMuunSystemOption(char* lpszFileName);
    bool InsertMuunOptionInfo(CMuunOpt* pCMuunOpt);
	bool InsertMuunOptionInfo(CMuunOpt pCMuunOpt, int OptionIndex); // overloading for free memory in option info reading
    long ConvertStringToTime(char* pchTime);
	CMuunInfo* GetMuunInfo(int Index) { return &m_CMuunInfo[Index]; };
    CMuunInfo* GetMuunItemNumToMuunInfo(int iMuunItemNum);
	void CTimeToViewDate(long lDate);
private:
	std::vector<CMuunInfo> m_CMuunInfo; 
    //CMuunInfo m_CMuunInfo[MUUN_MAX_INFO];
};

// muun system impletation
struct _tagMUUN_EFFECT_LIST {
	_tagMUUN_EFFECT_LIST() {
		Clear();
	}
	void Clear() {
		nIndex = 0;
		nMuunItemNum = 0;
		nCategory = 0;
		bOptEnable = 0;
		nOptType = 0;
		nOptValue = 0;
		bAddOptEnable = 0;
		nAddOptType = 0;
		nAddOptValue = 0;
		nSetTime = 0;
		nDuration = 0;
		pCMuunInfo = 0;
	}
    int nIndex;
    int nMuunItemNum;
    BYTE nCategory;
    bool bOptEnable;
    BYTE nOptType;
    int nOptValue;
    bool bAddOptEnable;
    BYTE nAddOptType;
    int nAddOptValue;
    ULONG nSetTime;
    int nDuration;
    CMuunInfo* pCMuunInfo;
};

struct MUUNITEMDATA {
	void Clear() {
		bIsUsed = 0;
		dwSerial = 0;
		pCMuunInfo = 0;
		wItemCode = 0;
		btOptionType = 0;
	}
    bool bIsUsed;
    WORD wItemCode;
    BYTE btOptionType;
    DWORD dwSerial;
    CMuunInfo* pCMuunInfo;
};

struct MUUN_ITEM_PERIOD_DATA {
	void Clear() {
		bIsUsed = 0;
		for (int i = 0; i < MUUN_INVENTORY_SIZE; i++) {
			ItemData[i].Clear();
		}
	}
    bool bIsUsed;
    BYTE btUsedDataCount;
    WORD wUserIndex;
    DWORD dwUserGUID;
    OBJECTSTRUCT* lpUserObj;
    char chAccountID[MAX_ACCOUNT_LEN + 1];
    char chCharacterName[MAX_ACCOUNT_LEN + 1];
    MUUNITEMDATA ItemData[MUUN_INVENTORY_SIZE];
};

class CMuunSystem {
public:
	CMuunSystem() {};
	~CMuunSystem() {};
    bool LoadScriptMuunSystemInfo(char* lpszFileName);
    bool LoadScriptMuunSystemOption(char* lpszFileName);
    void GDReqLoadMuunInvenItem(OBJECTSTRUCT& obj);
    void DGLoadMuunInvenItem(_tagSDHP_ANS_DBMUUN_INVEN_LOAD* lpMsg);
    void GDReqSaveMuunInvenItem(OBJECTSTRUCT& obj);
    void GCSendConditionStatus(int aIndex, int iPos, int iStatus);
    bool IsMuunItem(int iItemNum);
    bool IsMuunItem(CItem* pCitem);
    bool IsStoneofEvolution(CItem* pCitem);
    bool IsStoneofEvolution(int iItemNum);
    int GetMuunRankOfMuunInfo(int iItemNum);
    int GetEvolutionMuunItemIndex(int iItemNum);
    void CGMuunInventoryUseItemRecv(PMSG_USEITEM_MUUN_INVEN* lpMsg, int aIndex);
    void GCMuunInventoryUseItemResult(int aIndex, int iUseType, int iResult);
    bool MuunItemLevelUp(OBJECTSTRUCT* lpObj, int source, int target);
    bool MuunItemEvolution(OBJECTSTRUCT* lpObj, int source, int target);
    bool MuunItemLifeGem(OBJECTSTRUCT* lpObj, int source, int target);
    void MuunItemDamage(OBJECTSTRUCT* lpObj, int damage);
    int MuunItemEquipment(int aIndex, int iPos, int iSource);
    bool SetUserMuunEffect(OBJECTSTRUCT* lpObj, int iMuunItmeNum, int iMuunLv, int iEquipPos);
    void SetAddOptTypeValue(_tagMUUN_EFFECT_LIST* pUserMuunEffect);
    bool RemoveUserMuunEffect(OBJECTSTRUCT* lpObj, int iEquipPos);
    bool GetMuunItemValueOfOptType(OBJECTSTRUCT* lpObj, int iMuunOtpIndex, int* EffectValue1, int* EffectValue2);
    void CalCharacterStat(int aIndex, int iOptType);
    void CalCharacterStat(int aIndex, CMuunInfo* pCMuunInfo);
    int AddMuunItemPeriodInfo(OBJECTSTRUCT* lpObj);
    bool RemoveMuunItemPeriodInfo(OBJECTSTRUCT* lpObj);
    bool ClearPeriodMuunItemData(OBJECTSTRUCT* lpObj, WORD wItemCode, DWORD dwSerial);
    bool IsCorrectUser(OBJECTSTRUCT* lpObj);
    int SetMuunItemAddPeriodData(OBJECTSTRUCT* lpObj, int iMuunItemNum, DWORD dwSerial);
    int AddMuunItmePeriodData(OBJECTSTRUCT* lpObj, WORD wItemCode, DWORD dwSerial, int iDuration);
    int AddMuunItmePeriodData(OBJECTSTRUCT* lpObj, WORD, ULONG, int, CMuunInfo*);
    bool RemovePeriodMunnItemData(OBJECTSTRUCT* lpObj, WORD wItemCode, DWORD dwSerial);
    bool SetDisableMuunItemToExpire(OBJECTSTRUCT* lpObj, int iInventoryPosition);
    int GetItemFromMuunInventory(OBJECTSTRUCT* lpObj, WORD wItemCode, DWORD dwSerial);
    void CheckMuunItemPeriodData(OBJECTSTRUCT* lpObj);
    void CheckEquipMuunItemConditionProc(OBJECTSTRUCT* lpObj);
    int CheckMuunItemConditionProc(_tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo);
    int CheckMuunItemCondition(OBJECTSTRUCT& obj, _tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo);
    int ChkMuunOptConditionTime(_tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo);
    int ChkMuunOptConditionDay(_tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo);
    int ChkMuunOptConditionLevel(OBJECTSTRUCT& obj, _tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo);
    int ChkMuunOptConditionMap(OBJECTSTRUCT& obj, _tagMUUN_EFFECT_LIST* pUserMuunEffect, CMuunInfo* pCMuunInfo);
    void CheckEquipMuunItemCondition(OBJECTSTRUCT* lpObj);
    void CheckMuunItemConditionLevelUp(OBJECTSTRUCT* lpObj);
    void CheckMuunItemMoveMapConditionMap(OBJECTSTRUCT* lpObj, int iMapNumber);
    long GetCurrentDate();
    long GetExpireDate(int iDuration);
    bool CheckExpireDate(DWORD dwItemExpireDate);
    DWORD GetLeftDate(long lExpireDate);
    bool CheckAddOptionExpireDate(DWORD dwStartDate, DWORD dwEndDate);
    void MsgIsMuunItemActive(OBJECTSTRUCT* lpObj, int iPos);
    void MsgMuunItemEffectValue(OBJECTSTRUCT* lpObj, int iPos);
private:
    CMuunInfoMng m_MuunInfoMng; // better use how base class
    MUUN_ITEM_PERIOD_DATA m_MuunItemPeriodData[1000]; // max user object?
};

// temp for calls comparing
void GCMuunEquipmentChange(int aIndex, int pos);
void CGMuunItemGetRequest(PMSG_MUUNITEM_GETREQUEST* lpMsg, int aIndex);
void CGMuunItemSellRequestRecv(PMSG_MUUN_ITEM_SELLREQUEST* lpMsg, int aIndex);

extern CMuunSystem g_CMuunSystem;