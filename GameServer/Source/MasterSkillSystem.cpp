#include "stdafx.h"
#include "MasterSkillSystem.h"
#include "DBSockMng.h"
#include "GameMain.h"
#include "ObjCalCharacter.h"
#include "logproc.h"
#include "DSProtocol.h"
#include "..\common\winutil.h"
#include "..\common\readscript.h"
#include "BuffEffect.h"
#include "SkillAdditionInfo.h"
#include "CastleSiege.h"
#include "ChaosCastle.h"
#include "BloodCastle.h"
#include "IllusionTempleEvent.h"
#include "ObjUseSkill.h"
#include "zzzmathlib.h"
#include "MasterLevelSkillTreeSystem.h"
#include "ClassCalc.h"
#include "..\pugixml\pugixml.hpp"
#include "MapRateInfo.h"
#if (ENABLETEST_ZTLUA == 1)
#include "ZTLuaManager.h"
#endif
using namespace pugi;
CMasterExperience g_MasterExp;

CMasterExperience::CMasterExperience()
{

}

CMasterExperience::~CMasterExperience()
{

}

void CMasterExperience::Init()
{
	this->m_MinReset = 0;
	this->m_Mode = -1;
	this->m_MinMonsterLevel = 0;
	this->m_MaxMonsterLevel = 150;
	this->m_MapList.clear();
	this->m_MonsterList.clear();
}

void CMasterExperience::LoadInfo()
{
	this->Init();
	// ----
	xml_document Document;
	xml_parse_result Result = Document.load_file(gDirPath.GetNewPath(FILE_MASTERLEVEL));
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[MasterExperience] Loading fail [%s]", FILE_MASTERLEVEL);
		return;
	}
	// ----
	xml_node MasterLevel	= Document.child("masterlevel");
	xml_node MapList		= MasterLevel.child("maplist");
	xml_node MonsterList	= MasterLevel.child("monsterlist");
	xml_node Node;
	// -----
	this->m_MaxLevel = MasterLevel.child("maxlevel").text().as_int(0);
	this->m_MinReset = MasterLevel.child("leveling").attribute("minreset").as_int();
	this->m_Mode = MasterLevel.child("leveling").attribute("mode").as_int();
	// ----
	if( this->m_Mode == 0 || this->m_Mode == 4 )
	{
		this->m_MinMonsterLevel = MasterLevel.child("monsterlevel").attribute("min").as_int();
		this->m_MaxMonsterLevel = MasterLevel.child("monsterlevel").attribute("max").as_int();
	}
	// ----
	if( this->m_Mode == 1 || this->m_Mode == 3 || this->m_Mode == 4 )
	{
		for( Node = MapList.child("map"); Node; Node = Node.next_sibling() )
		{
			this->m_MapList.push_back(Node.attribute("id").as_int());
		}
	}
	// ----
	if( this->m_Mode == 2 || this->m_Mode == 3 )
	{
		for( Node = MonsterList.child("monster"); Node; Node = Node.next_sibling() )
		{
			this->m_MonsterList.push_back(Node.attribute("id").as_int());
		}
	}
	// ----
//	m_MinMonsterLevel = GetPrivateProfileInt("GameServerInfo","ML_MinMonsterKillLevel",110,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	
	char szTmp[6] = {0};
	GetPrivateProfileString("GameServerInfo","ML_AddExperience","1",szTmp,sizeof(szTmp)-1,gDirPath.GetNewPath(FILE_SERVER_COMMON));
	m_AddExp = (float)atof(szTmp);
	this->SetExpTable();
	//LogAddTD("MasterLevel Info Set MinMonsterKillLevel:%d, AddExpRate:%d", m_MinMonsterLevel,m_AddExp);
}

void CMasterExperience::SetExpTable()
{
	m_ExpTable.clear();

	if( m_ExpTable.capacity() > 0 ) {
		std::vector<__int64>().swap(m_ExpTable);
	}
	
#if (ENABLETEST_ZTLUA == 1)
	ZTLuaManager* LuaMng = new ZTLuaManager(FILE_LUA_CHARACTERBASICS);
	ZTLuaUnit::Bind(LuaMng->getLua());
	ZTLuaItem::Bind(LuaMng->getLua());

	LuaMng->Call("getExperienceTable", 1);
	m_ExpTable.push_back(0);

	for (int i = 1; i < m_MaxLevel + 2; i++) {
		luabridge::LuaRef ExpValue = (*LuaMng->getRef())[i];
		m_ExpTable.push_back((__int64)ExpValue.cast<double>());
	}

	ZTLuaManager::Delete(LuaMng);
#else
	__int64 Level = 0;
	__int64 Level2 = 0;
	__int64 Level3 = 0;
	__int64 Exp = 0;
	m_ExpTable.push_back(Exp);

	for (int i = 1; i < m_MaxLevel + 2; i++) {
		Level = i + MAX_CHAR_LEVEL;

		Exp = (Level + 9) * Level * Level * 10;
		Level2 = Level-255;
		Exp +=(Level2 + 9) * Level2 * Level2 * 1000;
		Exp = (Exp - 3892250000) / 2;

		if (i >= 200) {
			Level3 = (Level - 600) * (Level - 600);
			Exp *= (1 + (Level3 * 1.2) / 100000.0); 
		}

		m_ExpTable.push_back(Exp);
	}
#endif

	LogAddTD("Master level system exp table setting is complete");
}

void CMasterExperience::SetNextExp(LPOBJ lpObj)
{
	int Level = lpObj->MasterLevel + 1;

	if (Level < m_ExpTable.size()) {
		lpObj->MasterNextExp = m_ExpTable[Level];
	}
}

int CMasterExperience::LevelUp(LPOBJ lpObj,__int64 AddExp)
{
	if( IsEnabled(lpObj) == FALSE )
	{
		return FALSE;
	}

	if( lpObj->MasterLevel >= m_MaxLevel )
	{
		GCServerMsgStringSend(lMsg.Get(MSGGET(4,112)),lpObj->m_Index,1);
		return FALSE;
	}

	AddExp += (AddExp * m_ObjBill[lpObj->m_Index].GetMasterExp()) / 100;
	AddExp = __int64(AddExp * g_MapRateInfo.GetMasterExp(lpObj->MapNumber));

	if( lpObj->m_MPSkillOpt.MpsAddMLExp > 0.0f )
	{
		AddExp += AddExp*lpObj->m_MPSkillOpt.MpsAddMLExp/100.0f;
	}

	if( AddExp > 0 )
	{
		gObjSetExpPetItem(lpObj->m_Index, (int)AddExp);

#if (ENABLE_LOG_EXPERIENCE == 1)
		LogAddTD("ML Experience : Map[%d]-(%d,%d) [%s][%s](%d) %I64d %I64d",
			lpObj->MapNumber,lpObj->X,lpObj->Y,lpObj->AccountID,lpObj->Name,lpObj->MasterLevel,lpObj->MasterExperience,AddExp);
#endif

		if( (lpObj->MasterExperience + AddExp) < lpObj->MasterNextExp )
		{
			lpObj->MasterExperience += AddExp;
		}
		else
		{
			AddExp = 0;
			lpObj->MasterExperience = lpObj->MasterNextExp;
			lpObj->MasterLevel++;

#if (ENABLE_CUSTOM_CLASSCALC == 1)
			lpObj->MasterPoint += g_ClassCalc.GetLevelPoint(lpObj, 1, 0);
#else
			lpObj->MasterPoint++;
#endif

			gObjCalCharacter(lpObj->m_Index);

			lpObj->MaxLife += DCInfo.DefClass[lpObj->Class].LevelLife;
			lpObj->MaxMana += DCInfo.DefClass[lpObj->Class].LevelMana;

			lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
			lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;

			gObjCalcShieldPoint(lpObj);

			lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
			SetNextExp(lpObj);
			gObjSetBP(lpObj->m_Index);
			GJSetCharacterInfo(lpObj,lpObj->m_Index,0);
			LevelUpSend(lpObj);

			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
			GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
		}

		GCSendExp(lpObj->m_Index,(WORD)-1,AddExp,0,0);
	}

	return TRUE;
}

int CMasterExperience::IsEnabled(LPOBJ lpObj)
{
	if( lpObj == NULL )
		return FALSE;

	if( lpObj->bMasterInfoLoaded == FALSE )
		return FALSE;

	if( lpObj->Level >= 400 
		&& lpObj->ChangeUP2 == 1 
		&& lpObj->iResetCount >= this->m_MinReset )
		return TRUE;

	return FALSE;
}

int CMasterExperience::CheckMonsterLvl(LPOBJ lpObj, LPOBJ lpNpc)
{
	if( !IsEnabled(lpObj) )
	{
		return 1;
	}
	// ----
	if( this->m_Mode == 0 || this->m_Mode == 4 )
	{
		if( lpNpc->Level < this->m_MinMonsterLevel || lpNpc->Level > this->m_MaxMonsterLevel )
		{
			return 0;
		}
	}
	// ----
	bool Error;
	int Node;
	// ----
	if( this->m_Mode == 1 || this->m_Mode == 3 || this->m_Mode == 4 )
	{
		Error = true;
		// ----
		for( Node = 0; Node < this->m_MapList.size(); Node++ )
		{
			if( this->m_MapList[Node] == lpObj->MapNumber )
			{
				Error = false;
				break;
			}
		}
		// ----
		if( Error == true )
		{
			return 0;
		}
	}
	// ----
	if( this->m_Mode == 2 || this->m_Mode == 3 )
	{
		Error = true;
		// ----
		for( Node = 0; Node < this->m_MonsterList.size(); Node++ )
		{
			if( this->m_MonsterList[Node] == lpNpc->Class )
			{
				Error = false;
				break;
			}
		}
		// ----
		if( Error == true )
		{
			return 0;
		}
	}
	// ----
	return 1;
	/*if( IsEnabled(lpObj) )
	{
		if( lpNpc->Level < m_MinMonsterLevel )
		{
			return FALSE;
		}
	}
	return TRUE;*/
}

__int64 CMasterExperience::GetMaxExp(int Level)
{
	if (Level > 0 && Level < m_ExpTable.size()) {
		return m_ExpTable[Level];
	}
	return m_ExpTable[1];
}

int CMasterExperience::GetDecExp(LPOBJ lpObj)
{
	if( IsEnabled(lpObj) == FALSE )
	{
		return -1;
	}

	int DecExp = 0;

	if( lpObj->m_PK_Level <= 3 )
	{
		DecExp = 7;
	}
	else if( lpObj->m_PK_Level == 4 )
	{
		DecExp = 20;
	}
	else if( lpObj->m_PK_Level == 5 )
	{
		DecExp = 30;
	}
	else if( lpObj->m_PK_Level >= 6 )
	{
		DecExp = 40;
	}
return DecExp;
}

int CMasterExperience::GetSubMoney(LPOBJ lpObj)
{
	if( IsEnabled(lpObj) == FALSE )
	{
		return -1;
	}
return 4;
}

struct PMSG_REQ_MASTERINFO
{
	PBMSG_HEAD h;
	int Index;
	char szName[11];
};

void CMasterExperience::DGReqMasterInfo(LPOBJ lpObj)
{
	if( lpObj->bMasterInfoLoaded == TRUE )
	{
		return;
	}

	PMSG_REQ_MASTERINFO pMsg;
	pMsg.h.set((LPBYTE)&pMsg,0x30,sizeof(pMsg));
	pMsg.Index  = lpObj->m_Index;
	memcpy(pMsg.szName,lpObj->Name,10);
	pMsg.szName[10] = 0;

	cDBSMng.Send((char*)&pMsg,pMsg.h.size);
	LogAddTD("[%s][%s] Request master level info to DB",
		lpObj->AccountID,lpObj->Name);
}

typedef struct
{
	PBMSG_HEAD h;
	BYTE result;
	int Index;
	char szName[11];
	short MasterLevel;
	__int64 MasterExp;
	__int64 MasterNextExp;
	short MasterPoint;
}PMSG_ANS_MASTERINFO, *LPMSG_ANS_MASTEREINFO;

void CMasterExperience::GDAnsMasterInfo(LPBYTE aRecv)
{
	if( aRecv == NULL )
	{
		return;
	}

	LPMSG_ANS_MASTEREINFO lpMsg = (LPMSG_ANS_MASTEREINFO)aRecv;

	if( gObjIsConnected(lpMsg->Index) == FALSE )
	{
		return;
	}

	int iIndex = lpMsg->Index;
	LPOBJ lpObj = &gObj[iIndex];

	if( lpObj->bMasterInfoLoaded == TRUE )
	{
		return;
	}

	if( lpMsg->result == FALSE )
	{
		LogAddTD("[%s][%s] MasterLevel Info Load Fail",
			lpObj->AccountID,lpObj->Name);
	}
	else
	{
		if( lpMsg->result == TRUE )
		{
			lpObj->bMasterInfoLoaded = TRUE;

			if( lpMsg->MasterLevel == NULL &&
				lpMsg->MasterNextExp == NULL)
			{
				lpObj->MasterLevel = lpMsg->MasterLevel;
				lpObj->MasterExperience = lpMsg->MasterExp;
				lpObj->MasterNextExp = lpMsg->MasterNextExp;
				lpObj->MasterPoint = lpMsg->MasterPoint;
	
				LogAddTD("[%s][%s] MasterLevel Info First Set [MLevel:%d][MLExp:%I64d][MLNextExp:%I64d][MLPoint:%d]",
					lpObj->AccountID,lpObj->Name,lpObj->MasterLevel,lpObj->MasterExperience,lpObj->MasterNextExp,lpObj->MasterPoint);
				SaveMasterInfo(lpObj);
			}
			else
			{
				lpObj->MasterLevel = lpMsg->MasterLevel;
				lpObj->MasterExperience = lpMsg->MasterExp;
				lpObj->MasterNextExp = lpMsg->MasterNextExp;
				lpObj->MasterPoint = lpMsg->MasterPoint;
	
				LogAddTD("[%s][%s] Recv MasterLevel Info [MLevel:%d][MLExp:%I64d][MLNextExp:%I64d][MLPoint:%d]",
					lpObj->AccountID,lpObj->Name,lpObj->MasterLevel,lpObj->MasterExperience,lpObj->MasterNextExp,lpObj->MasterPoint);
			}
	
			lpObj->MaxLife = DCInfo.DefClass[lpObj->Class].Life + (lpObj->Level + lpObj->MasterLevel - 1) * DCInfo.DefClass[lpObj->Class].LevelLife  + ((lpObj->Vitality - DCInfo.DefClass[lpObj->Class].Vitality ) * DCInfo.DefClass[lpObj->Class].VitalityToLife);
	
			if( lpObj->Life > lpObj->MaxLife + lpObj->AddLife )
			{
				lpObj->Life = lpObj->MaxLife;
			}
			lpObj->MaxMana = DCInfo.DefClass[lpObj->Class].Mana + (lpObj->Level + lpObj->MasterLevel - 1) * DCInfo.DefClass[lpObj->Class].LevelMana  + ((lpObj->Energy - DCInfo.DefClass[lpObj->Class].Energy ) * DCInfo.DefClass[lpObj->Class].EnergyToMana);

			if( lpObj->Mana > lpObj->MaxMana + lpObj->AddMana )
			{
				lpObj->Mana = lpObj->MaxMana;
			}
//			gObjCalcMaxLifePower(lpObj->m_Index);
			gObjSetBP(lpObj->m_Index);
			gObjCalcShieldPoint(lpObj);

			lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;

			LogAddTD("[%s][%s] Reset Max Value For MasterLevel [MaxLife:%d][MaxMana:%d][MaxSD:%d]",
				lpObj->AccountID,lpObj->Name,lpObj->MaxLife,lpObj->MaxMana,lpObj->iShield);
			SendMasterInfo(lpObj);
			//g_MasterSkillSystem.CGReqGetMasterLevelSkillTree(lpObj->m_Index);
			GCReFillSend(lpObj->m_Index,(WORD)lpObj->Life,0xFF,0,lpObj->iShield);
			GCManaSend(lpObj->m_Index,(WORD)lpObj->Mana,0xFF,0,lpObj->BP);
		}
	}
	gObjCalCharacter(lpObj->m_Index);
	gObjCalcMLSkillItemOption(lpObj);
	g_MasterSkillSystem.CGReqGetMasterLevelSkillTree(lpObj->m_Index);
}

typedef struct
{
	PBMSG_HEAD h;
	char szName[11];
	WORD MasterLevel;
	__int64 MasterExp;
	__int64 MasterNextExp;
	WORD MasterPoints;
}PMSG_SAVE_MASTERINFO, *LPMSG_SAVE_MASTERINFO;

void CMasterExperience::SaveMasterInfo(LPOBJ lpObj)
{
	if( lpObj->bMasterInfoLoaded == FALSE )
	{
		return;
	}

	PMSG_SAVE_MASTERINFO pMsg;
	pMsg.h.set((LPBYTE)&pMsg,0x31,sizeof(pMsg));
	memcpy(pMsg.szName,lpObj->Name,10);
	pMsg.szName[10] = 0;

	pMsg.MasterLevel = lpObj->MasterLevel;
	pMsg.MasterExp = lpObj->MasterExperience;
	pMsg.MasterNextExp = lpObj->MasterNextExp;
	pMsg.MasterPoints = lpObj->MasterPoint;

	cDBSMng.Send((char*)&pMsg,pMsg.h.size);

	LogAddTD("[%s][%s] MasterLevel Info Save [MLevel:%d][MLExp:%I64d][MLNextExp:%I64d][MLPoint:%d]",
		lpObj->AccountID,lpObj->Name,lpObj->MasterLevel,lpObj->MasterExperience,lpObj->MasterNextExp,lpObj->MasterPoint);

}

struct PMSG_MASTERINFORESULT
{
	PBMSG_HEAD2 h;
	WORD Level;

	BYTE Exp[8];
	BYTE NextExp[8];

	WORD Points;

	WORD MaxLife;
	WORD MaxMana;
	WORD MaxShield;
	WORD MaxBP;

#if(FIX_HP==1)
	int MAXHP;
	int MAXMP;
	int MAXSD;
#endif
};

#pragma pack(push)
#pragma pack(4)

struct PMSG_MASTERSKILL_COUNT
{
	PWMSG_HEAD2	h;
	int count;				// 8
};

struct MASTERSKILL_STRUCT
{
	BYTE btMasterSkillId;
	BYTE btLevel;
	float value;
	float nextvalue;
};

struct PMSG_ANS_ADDMASTERSKILL
{
	PBMSG_HEAD2	h;
	BYTE btResult;			// 4
	WORD wLevelPoint;		// 6
	BYTE btMasterSkillId;	// 8
	DWORD dwSkillId;		// C
	BYTE btLevel;			// 10
	float value;			// 14
	float nextvalue;		// 18
};

#pragma pack(pop)

void CMasterExperience::SendMasterInfo(LPOBJ lpObj)
{
	if( lpObj->bMasterInfoLoaded == FALSE )
	{
		DGReqMasterInfo(lpObj);
		return;
	}

	PMSG_MASTERINFORESULT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xF3,0x50,sizeof(pMsg));
	pMsg.Level = lpObj->MasterLevel;

	pMsg.Exp[0] = DWORD((DWORD(lpObj->MasterExperience >> 32) >> 16) & 0xFFFF) >> 8;
	pMsg.Exp[1] = DWORD((DWORD(lpObj->MasterExperience >> 32) >> 16) & 0xFFFF) & 0xFF;
	pMsg.Exp[2] = DWORD((DWORD(lpObj->MasterExperience >> 32) & 0xFFFF) & 0xFFFF) >> 8;
	pMsg.Exp[3] = DWORD((DWORD(lpObj->MasterExperience >> 32) & 0xFFFF) & 0xFFFF) & 0xFF;
	pMsg.Exp[4] = DWORD((DWORD(lpObj->MasterExperience & 0xFFFFFFFF) >> 16) & 0xFFFF) >> 8;
	pMsg.Exp[5] = DWORD((DWORD(lpObj->MasterExperience & 0xFFFFFFFF) >> 16) & 0xFFFF) & 0xFF;
	pMsg.Exp[6] = DWORD((DWORD(lpObj->MasterExperience & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) >> 8;
	pMsg.Exp[7] = DWORD((DWORD(lpObj->MasterExperience & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) & 0xFF;

	pMsg.NextExp[0] = DWORD((DWORD(lpObj->MasterNextExp >> 32) >> 16) & 0xFFFF) >> 8;
	pMsg.NextExp[1] = DWORD((DWORD(lpObj->MasterNextExp >> 32) >> 16) & 0xFFFF) & 0xFF;
	pMsg.NextExp[2] = DWORD((DWORD(lpObj->MasterNextExp >> 32) & 0xFFFF) & 0xFFFF) >> 8;
	pMsg.NextExp[3] = DWORD((DWORD(lpObj->MasterNextExp >> 32) & 0xFFFF) & 0xFFFF) & 0xFF;
	pMsg.NextExp[4] = DWORD((DWORD(lpObj->MasterNextExp & 0xFFFFFFFF) >> 16) & 0xFFFF) >> 8;
	pMsg.NextExp[5] = DWORD((DWORD(lpObj->MasterNextExp & 0xFFFFFFFF) >> 16) & 0xFFFF) & 0xFF;
	pMsg.NextExp[6] = DWORD((DWORD(lpObj->MasterNextExp & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) >> 8;
	pMsg.NextExp[7] = DWORD((DWORD(lpObj->MasterNextExp & 0xFFFFFFFF) & 0xFFFF) & 0xFFFF) & 0xFF;

	pMsg.Points = lpObj->MasterPoint;
	pMsg.MaxLife = (WORD)(lpObj->AddLife + lpObj->MaxLife);
	pMsg.MaxMana = (WORD)(lpObj->AddMana + lpObj->MaxMana);
	pMsg.MaxShield = lpObj->iMaxShield + lpObj->iAddShield;
	pMsg.MaxBP = lpObj->MaxBP + lpObj->AddBP;

#if (FIX_HP==1)
	pMsg.MAXHP = (int)(lpObj->AddLife+lpObj->MaxLife);
	pMsg.MAXMP = (int)(lpObj->AddMana+lpObj->MaxMana);
	pMsg.MAXSD = (int)(lpObj->iAddShield+ lpObj->iMaxShield);
#endif

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
}

struct PMSG_MASTER_LEVELUP
{
	PBMSG_HEAD2 h;
	WORD Level;
	WORD LevelAdd;
	WORD Points;
	WORD MaxMoints;
	WORD MaxHP;
	WORD MaxMana;
	WORD MaxShield;
	WORD MaxBP;

#if (FIX_HP)
	int MAXHP;
	int MAXMP;
	int MAXSD;
#endif
};

void CMasterExperience::LevelUpSend(LPOBJ lpObj)
{
	PMSG_MASTER_LEVELUP pMsg;
	PHeadSubSetB((LPBYTE)&pMsg,0xF3,0x51,sizeof(pMsg));
	pMsg.Level = lpObj->MasterLevel;
	pMsg.LevelAdd = 1;
	pMsg.Points = lpObj->MasterPoint;
	pMsg.MaxMoints = 200;
	pMsg.MaxHP = (WORD)(lpObj->MaxLife + lpObj->AddLife);
	pMsg.MaxMana = (WORD)(lpObj->MaxMana + lpObj->AddMana);
	pMsg.MaxShield = lpObj->iMaxShield + lpObj->iAddShield;
	pMsg.MaxBP = lpObj->MaxBP + lpObj->AddBP;

#if (FIX_HP==1)
	pMsg.MAXHP = (int)(lpObj->AddLife+lpObj->MaxLife);
	pMsg.MAXMP = (int)(lpObj->AddMana+lpObj->MaxMana);
	pMsg.MAXSD = lpObj->iAddShield+lpObj->iMaxShield;
#endif

	DataSend(lpObj->m_Index,(LPBYTE)&pMsg,pMsg.h.size);
	GCSendEffectInfo(lpObj->m_Index,16);

	LogAddTD("[%s][%s] Master Level Up :%d, MLPoint:%d/%d",
		lpObj->AccountID,lpObj->Name,lpObj->MasterLevel,lpObj->MasterPoint,200);
}
