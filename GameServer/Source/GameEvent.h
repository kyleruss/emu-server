// GameEvent.h: interface for the GameEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEEVENT_H__847E696C_185F_4163_887B_1973944B89F8__INCLUDED_)
#define AFX_GAMEEVENT_H__847E696C_185F_4163_887B_1973944B89F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if (__CUSTOM__ == 1)

#if (ANTI_CHEAT == 1)

struct PMSG_CHECK_ANTICHEAT
{
	PBMSG_HEAD h;
	BYTE type;
	WORD wWord1;
	WORD wAttackSpeed1;
	WORD wWord2;
	WORD wAttackSpeed2;
};

BOOL gObjSpeedCheck(int uIndex,PMSG_CHECK_ANTICHEAT* lpMsg,int headcode,int size);

#endif

class TCustomExp
{
public:
	TCustomExp();
	virtual ~TCustomExp();
public:
	void LoadData(char* szFileName);
	void LoadResetExp(char* szFileName);
	float GetExp(int MapNumber);
	float GetZen(int MapNumber);
	__int64 GetResetExp(LPOBJ lpObj,__int64 Exp);
	int GetJewelDrop(int MapNumber);

protected:
	std::map<int,float> m_ExpMap;
	std::map<int,float> m_ZenMap;
	std::map<int,float> m_ResetExpMap;
	std::map<int,int>	m_JewelDropMap;
};

extern TCustomExp gCustomExp;


#if (__CUSTOM_QUEST__ == 1)

#define MAX_QUEST_ITEM	3
#define MAX_QUEST_GROUP	4

struct MyQuest_STRUCT
{
	MyQuest_STRUCT() {
		m_IIndex = (BYTE)-1;
		m_ICount = 0;
		m_MIndex = (short)-1;
		m_MDropRate = 0;

		memset(m_szItemName,0,sizeof(m_szItemName));
		memset(m_szQuestInfo,0,sizeof(m_szQuestInfo));
	};

	BYTE m_IIndex;
	BYTE m_ICount;

	short m_MIndex;
	short m_MDropRate;

	char m_szItemName[32];
	char m_szQuestInfo[32]; 
};

struct MyQuest_QUEST
{
	MyQuest_QUEST() {
		m_RewardCP = -1;
		m_RewardItem = -1;
		m_RewardPts = -1;

	};

	MyQuest_STRUCT	m_Item[MAX_QUEST_ITEM];

	int	m_RewardCP;
	int	m_RewardItem;
	int m_RewardPts;
};

typedef std::vector<MyQuest_QUEST>	Quest_Info;

struct MyQuest_GROUP
{
	MyQuest_GROUP()	{
		m_Index = -1;
		m_QuestCount = 0;

		m_Quest.clear();
	};

	int m_Index;
	int m_QuestCount;
	Quest_Info	m_Quest;
};

struct MyQuest_USER
{
	short QuestGroup;
	short QuestIndex;
	short QuestState;
	short QuestItem[MAX_QUEST_ITEM];

	bool QuestLoaded;
};

class CMyQuest
{
public:
	CMyQuest()	{
		m_bLoaded = FALSE;
	};
	virtual ~CMyQuest()	{};

	// General System
	void LoadData(char* szFile);
	void NpcTalk(LPOBJ lpObj,LPOBJ lpNpc);
	void MonsterDie(LPOBJ lpObj,LPOBJ lpNpc);
	void GiveReward(MyQuest_QUEST* lpQuest,LPOBJ lpObj);
	BOOL CheckCondition(MyQuest_USER* lpQuestUser,MyQuest_QUEST* lpQuest);
	BOOL StartQuest(MyQuest_USER* lpQuestUser,int aIndex);

	MyQuest_QUEST* FindQuest(int Group,int Index);

	// User System

	MyQuest_USER* GetUser(int UserIndex);
	void InitUser(int UserIndex);
	void SaveUser(int UserIndex);
	void ReqUserInfo(int UserIndex);
	void AnsUserInfo(LPBYTE aRecv);

public:

	bool m_bLoaded;
	MyQuest_GROUP	QuestGroup[MAX_QUEST_GROUP];
	MyQuest_USER	QuestUser[OBJMAXUSER];
};

extern CMyQuest g_MyQuest;

#endif

#if (CUSTOM_PETS== 1)

#define CUSTOM_PET_MAXLEVEL	400

struct _CUSTOM_PET_INFO
{
	short monId;
	short reqLevel;

	BYTE reqItem_Index;
	BYTE reqItem_Number;
	BYTE reqItem_Count;

	int		MaxLife;
	short	DmgMin;
	short	DmgMax;
	short	AttRate;
	short	AttSpeed;
	short	Defense;
	short	Defrate;
};

struct _CUSTOM_PET_USERINFO
{
	BYTE	m_bLoaded;
	BYTE	m_bSummoned;
	short	m_Level;
	__int64	m_Exp;
};

typedef struct 
{
	PBMSG_HEAD2 h;
	int aIndex;
	char szName[10];
}PBMSG_REQSUMMONINFO, *LPBMSG_REQSUMMONINFO;

typedef struct
{
	PBMSG_HEAD2 h;
	BYTE result;

	int aIndex;
	char szName[11];

	int Level;
	__int64 Exp;
}PBMSG_ANSSUMMONINFO, *LPBMSG_ANSSUMMONINFO;

typedef struct 
{
	PBMSG_HEAD2 h;

	char szName[10];
	int level;
	__int64 exp;
}PBMSG_REQSAVESUMMON, *LPBMSG_REQSAVESUMMON;

class TCustomPets
{
public:
	TCustomPets();
	virtual ~TCustomPets();

	void Load(char* lpszFileName);
	void SummonRequest(int aIndex, char* szSummonId);
	void SummonInfo(int aIndex);

	void RequestSummonInfo(int aIndex,char* szName);
	void SummoInfoAnswer(LPBMSG_ANSSUMMONINFO lpMsg);
	void SaveSummonInfo(int aIndex,char* szName);

	void gObjLevelUp(int aIndex,__int64 exp);
	void SetSummon(int aIndex,BOOL bSummon);

private:
	__int64	m_i64Exp[CUSTOM_PET_MAXLEVEL+1];
	_CUSTOM_PET_USERINFO m_UserInfo[OBJMAXUSER];
	std::map<std::string, _CUSTOM_PET_INFO> m_PetInfo;

	float m_ExpPerc;
	char szCommand[32];
	char szInfoCommand[32];
};

extern TCustomPets	gCustomPets;

#endif // END CUSTOM PETS


#if (CUSTOM_OLYMP==1)

#define MAX_OLYMP_USER	8
#define MAX_OLYMP_RANK	10


#define OLYMP_STATE_IDLE		0
#define OLYMP_STATE_FIGHTING	1
#define OLYMP_STATE_ENDING		2
#define OLYMP_STATE_ENDCYCLE	3

class CCustomOlymp
{
	struct PSMG_OLP_NPCTALK
	{
		PBMSG_HEAD2 h;
		BYTE btOpen;
		BYTE btRegistered;
	};

	struct PMSG_OLP_MESSAGEBOX
	{
		PBMSG_HEAD2 h;
		BYTE btMessage;
		short aInfo1;
		short aInfo2;
	};

	struct OLP_REQ_OLYMPINITDATA
	{
		PBMSG_HEAD2 h;
		WORD wMapSvrNum;	// 4
		int iOlympEventCycle;	// 8
	};

	struct OLP_ANS_OLYMPINITDATA
	{
		PBMSG_HEAD2 h;
		int iResult;
		WORD wMapSvrNum;
		WORD wStartYear;
		BYTE btStartMonth;
		BYTE btStartDay;
		WORD wEndYear;
		BYTE btEndMonth;
		BYTE btEndDay;
		BYTE btFirstCreate;
	};

	struct OLP_REQ_SDEDCHANGE
	{
		PBMSG_HEAD2 h;
		WORD wMapSvrNum;	// 4
		int iIndex;	// 8
		WORD wStartYear;	// C
		BYTE btStartMonth;	// E
		BYTE btStartDay;	// F
		WORD wEndYear;	// 10
		BYTE btEndMonth;	// 12
		BYTE btEndDay;	// 13
	};

	struct OLP_ANS_SDEDCHANGE
	{
		PBMSG_HEAD2 h;
		int iResult;
		WORD wMapSvrNum;
		int iIndex;
		WORD wStartYear;
		BYTE btStartMonth;
		BYTE btStartDay;
		WORD wEndYear;
		BYTE btEndMonth;
		BYTE btEndDay;
	};

	struct OLP_REQ_REGFIGHT
	{
		PBMSG_HEAD2 h;
		WORD wMapSvrNum;
		int iIndex;
		int iClass;
		char szName[10];
	};

	struct OLP_ANS_REGFIGHT
	{
		PBMSG_HEAD2 h;
		BYTE btResult;
		int iIndex;
		char szName[10];
		int iBattleCount;
		int iBattlePts;
		int iLastRegTime;
	};

	struct OLP_ENFIGHT
	{
		PBMSG_HEAD2 h;
		WORD wMapSvrNum;
		int iEndResult;
		char szWinner[10];
		char szLooser[10];
	};

	struct OLP_REQ_RESET
	{
		PBMSG_HEAD2 h;
		WORD wMapSvrNum;
	};

	struct OLP_REQ_REWARD
	{
		PBMSG_HEAD2 h;
		WORD wMapSvrNum;
		int iIndex;
		char szName[10];
	};

	struct OLP_ANS_REWARD
	{
		PBMSG_HEAD2 h;
		BYTE btResult;
		BYTE btRank;
		BYTE btReward;
		int iIndex;
		char szName[10];
	};

	struct OLP_REQ_STATS
	{
		PBMSG_HEAD2 h;
		WORD wMapSvrNum;
		int iIndex;
		char szName[10];
	};

	struct OLP_ANS_STATS
	{
		PBMSG_HEAD2 h;
		int iIndex;
		char szName[10];
		BYTE btResult;
		int iPts;
		int iBattles;
	};

	struct _OLYMP_SCHEDULE_DATA
	{
		int m_bIN_USE;
		int m_iSTATE;
		int m_iADD_DAY;
		int m_iADD_HOUR;
		int m_iADD_MIN;
		int m_iGAP_SEC;

		_OLYMP_SCHEDULE_DATA()	{
			this->Clear();
		};
		virtual ~_OLYMP_SCHEDULE_DATA(){};

		void Clear()	{
			m_bIN_USE = 0;
			m_iSTATE = -1;
			m_iADD_DAY = -1;
			m_iADD_HOUR = -1; 
			m_iADD_MIN = -1;
			m_iGAP_SEC = 0;
		};
	};

	enum
	{
		OLYMP_NONE = -1,
		OLYMP_CLOSED = 0,
		OLYMP_WAITING = 1,
		OLYMP_ACTIVE = 2,
	};

	struct OLYMP_USER
	{
		int m_Index;
		int m_Score;
		char m_btROOM;
	};

	struct OLYMP_CLASS_STATS
	{
		short aStr;
		short aDex;
		short aVit;
		short aEne;
		short aLea;
	};

	struct PMSG_OLP_VIEWPORT_SELF
	{
		PBMSG_HEAD2 h;
		BYTE btFamily;
		int iRank;
		BYTE btSign;
		int iContribution;
		int iNextContribution;
	};

	class CCustomOlympSort	// Completed
	{
	public:

		bool operator()(struct OLYMP_USER * const lpObj1, struct OLYMP_USER * const lpObj2)
		{
			return ( lpObj1->m_Score >= lpObj2->m_Score ) ? TRUE : FALSE;
		}
	};



public:
	CCustomOlymp();
	virtual ~CCustomOlymp();
	static BYTE ScheduleStateCompFunc(_OLYMP_SCHEDULE_DATA & A,_OLYMP_SCHEDULE_DATA & B);

	BOOL LoadData(char* lpszFileName);
	BOOL LoadPreFixData(char* lpszFileName);

	
	void GetNextDay(_SYSTEMTIME * st, int iAfterDay, int iAfterHour, int iAfterMin, int iAfterSec);
	BOOL Ready(int iMapSvrGroup);
	void SetDataLoadState(int iDataLoadState);
	int GetDataLoadState();
	void Clear();
	void Reset();

	BOOL DataRequest();
	BOOL SetOlympInitData(OLP_ANS_OLYMPINITDATA* lpMsg);
	void GetStateDateTime(int iState,_SYSTEMTIME * tm);
	int GetStateGapSec(int iOlympState);
	BOOL Init();

	BOOL IsRegistered(int aIndex);
	BOOL RegUser(int aIndex,int pts);
	BOOL NpcTalk(LPOBJ lpNpc,LPOBJ lpObj);
	void CGRequestReg(int aIndex);
	void CGRequestReward(int aIndex);
	void CheckUsers();
	BOOL CheckSync();
	BOOL CheckOpen();

	void PrepareUser(int aIndex);
	void Start();

	void Run();
	void ProcState_IDLE();
	void ProcState_FIGHTING();
	void ProcState_ENDING();
	void ProcState_ENDCYCLE();

	void SetState(int iState,int bRemainSec);
	void SetState_IDLE(int bRemainSec);
	void SetState_FIGHTING(int bRemainSec);
	void SetState_ENDING(int bRemainSec);
	void SetState_ENDCYCLE(int bRemainSec);

	void GS_GDReqOlympInitData();
	void GS_DGAnsOlympInitData(LPBYTE lpRecv);
	void GS_GDReqOlympDateChange(int iMapSvrGroup, int aIndex, WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay);
	void GS_DGAnsOlympDateChange(LPBYTE lpRecv);
	void GS_DGAnsOlympReg(LPBYTE lpRecv);
	void GS_GDReqResetOlympInfo(int iMapSvrGroup);
	void GS_DGAnsOlympReward(LPBYTE aRecv);
	void GS_GDReqOlympStats(int aIndex);
	void GS_DGAnsOlympStats(LPBYTE aRecv);

	void EndFight(int iWinner,int iLooser,int bFail);
public:
	int m_bOlympEnable;
	int m_bDoRun;
	int m_iOlympCycle;
	int m_iMapSvrGroup;
	int m_iMinResetCount;

	int m_iFSTART_HOUR;
	int m_iFSTART_MIN;
	int m_iFEND_HOUR;
	int m_iFEND_MIN;

	int m_bFileDataLoadOK;
	int m_bDbDataLoadOK;
	int m_iOlympDataLoadState;

	int m_iOlympState;
	DWORD m_dwDbDataLoadTickCount;
	int m_iOLYMP_TICK_COUNT;

	int m_iOLYMP_REMAIN_MSEC;
	
	int m_bOPEN;
	int m_iFIGHT_REMAIN_MSEC;
	int m_iFIGHT_NOTIFY_COUNT;

	int m_dwFIGHT_START;
	BYTE m_bFIGHT_START;
	BYTE m_btNOTIFY_COUNT;

	OLYMP_CLASS_STATS m_StatData[MAX_TYPE_PLAYER];
	OLYMP_USER m_UserData[MAX_OLYMP_USER];
	int	m_UserCount;
	int m_RewardPts[MAX_OLYMP_RANK];

	int m_bFixOlympCycleStartDate;
	_SYSTEMTIME m_tmFixOlympCycleStartDate;
	_SYSTEMTIME m_tmStartDate;
	_SYSTEMTIME m_tmEndDate;
	_SYSTEMTIME m_tmNowDate;

	std::vector<_OLYMP_SCHEDULE_DATA> m_vtScheduleData;
	CRITICAL_SECTION m_critScheduleData;
};

extern CCustomOlymp g_CustomOlymp;

#endif // END CUSTOM_OLYMP

#if (CUSTOM_EXOPT==1)

#pragma pack(push)
#pragma pack(1)

struct CUSTOM_OPTIONS_EXHEADER
{
	int iOptionCount;
	int iEnchantOptCount;
	int iExcellentOptCount;
	int iItemCount;
};

struct CUSTOM_OPTIONS_NAME
{
	int iIndex;
	char szName[60];
};

struct CUSTOM_OPTIONS_ENCHANT
{
	int iId;
	int iIndex1[6];
	int iValue1[6];
	int iIndex2[6];
	int iValue2[6];
};

struct CUSTOM_OPTIONS_EXCELLENT
{
	int iId;
	int iValue[6];
};

struct CUSTOM_OPTIONS_LIST
{
	int Item;
	short EnchantId;
	short ExcellentId;
};

#pragma pack(pop)

typedef std::map<int,CUSTOM_OPTIONS_NAME> vtOptName;
typedef std::map<int,CUSTOM_OPTIONS_ENCHANT> vtEnchantOpt;
typedef std::map<int,CUSTOM_OPTIONS_EXCELLENT> vtExcellentOpt;
typedef std::map<int,CUSTOM_OPTIONS_LIST> vtOptList;

#define ENCHANT_OPTION_CRITDMG	0
#define ENCHANT_OPTION_EXCLDMG	1
#define ENCHANT_OPTION_DBLDMG	2
#define ENCHANT_OPTION_IGNDMG	3

class CNewOptions
{
public:
	CNewOptions();
	virtual ~CNewOptions();

	void LoadData(char* lpszFileName);

	int GetExcellentId(int ItemID);
	int GetEnchantId(int ItemID);
	BYTE GetExcellentOption(int OptId,int OptionID);
	void SetEnchantOptions(LPOBJ lpObj);
	void SetEnchantOption(LPOBJ lpObj,int Option,int Value);

protected:
	vtEnchantOpt	m_vtEnchantOpt;
	vtExcellentOpt	m_vtExcellentOpt;
	vtOptList		m_vtOptList;
};

extern CNewOptions gNewOptions;

#endif // ENC CUSTOM_EXOPT

#endif // END CUSTOM

#endif // !defined(AFX_GAMEEVENT_H__847E696C_185F_4163_887B_1973944B89F8__INCLUDED_)
