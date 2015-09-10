#pragma once

#include "stdafx.h"
#include "..\Lua\include\lua.hpp"
#include "..\LuaBridge\LuaBridge.h"
#include "user.h"
#include "SProtocol.h"

class ZTLuaUnitInfo {
public:
	ZTLuaUnitInfo() {}

	virtual ~ZTLuaUnitInfo() {
		m_Account.clear();
		m_Name.clear();
	}

protected:
	std::string m_Account;
	std::string m_Name;
	int m_ObjectIndex;
	WORD m_Class;
	BYTE m_Authority, m_Type;
	short m_Level, m_LevelPoints, m_MasterLevel, m_MasterPoints;
	float m_Life, m_MaxLife, m_Mana, m_MaxMana;
	int m_Strength, m_Dexterity, m_Vitality, m_Energy, m_Command;
	int	m_AttackDamageMaxLeft, m_AttackDamageMinLeft, 
		m_AttackDamageMaxRight, m_AttackDamageMinRight;
	int	m_CurseDamageMin, m_CurseDamageMax;
	int m_AttackRating, m_SuccessfulBlocking;
	int m_AttackSpeed, m_MagicSpeed;
	int m_Defense, m_MagicDefense;
	int m_PartyNumber, m_GuildNumber;
	BYTE m_MapNumber, m_MapPosX, m_MapPosY;
	int m_MoneyInventory, m_MoneyWarehouse;
};

class ZTLuaUnit : public ZTLuaUnitInfo {
public:
	explicit ZTLuaUnit(int ObjectIndex) {
		if (!OBJMAX_RANGE(ObjectIndex)) {
			LogAddC(2, "[LUA] Failed to get unit (%d), reason: wrong index", ObjectIndex);
			return;
		}

		//if (!gObj[ObjectIndex].Type == OBJ_USER && gObj[ObjectIndex].Connected < PLAYER_LOGGED) {
		//	LogAddC(2, "[LUA] Failed to get unit (%d), reason: user is not logged", ObjectIndex);
		//	return;
		//}
		//else if (gObj[ObjectIndex].Connected < PLAYER_PLAYING) {
		//	LogAddC(2, "[LUA] Failed to get unit (%d), reason: object is not live", ObjectIndex);
		//	return;
		//}

		fillUnitInfo(ObjectIndex);
	}

	virtual ~ZTLuaUnit() { }

	static void Bind(lua_State* Lua) {
		using namespace luabridge;
		getGlobalNamespace(Lua)
		.beginClass<ZTLuaUnit>("ZTUnit")
			.addConstructor<void(*) (int)>()
			.addData("Account", &ZTLuaUnit::m_Account)
			.addData("Name", &ZTLuaUnit::m_Name)
			.addData("ObjectIndex", &ZTLuaUnit::m_ObjectIndex)
			.addData("Class", &ZTLuaUnit::m_Class)
			.addData("Authority", &ZTLuaUnit::m_Authority)
			.addData("Type", &ZTLuaUnit::m_Type)
			.addData("Level", &ZTLuaUnit::m_Level)
			.addData("LevelPoints", &ZTLuaUnit::m_LevelPoints)
			.addData("MasterLevel", &ZTLuaUnit::m_MasterLevel)
			.addData("MasterPoints", &ZTLuaUnit::m_MasterPoints)
			.addData("Life", &ZTLuaUnit::m_Life)
			.addData("MaxLife", &ZTLuaUnit::m_MaxLife)
			.addData("Mana", &ZTLuaUnit::m_Mana)
			.addData("MaxMana", &ZTLuaUnit::m_MaxMana)
			.addData("Strength", &ZTLuaUnit::m_Strength)
			.addData("Dexterity", &ZTLuaUnit::m_Dexterity)
			.addData("Vitality", &ZTLuaUnit::m_Vitality)
			.addData("Energy", &ZTLuaUnit::m_Energy)
			.addData("Command", &ZTLuaUnit::m_Command)
			.addData("AttackDamageMaxLeft", &ZTLuaUnit::m_AttackDamageMaxLeft)
			.addData("AttackDamageMinLeft", &ZTLuaUnit::m_AttackDamageMinLeft)
			.addData("AttackDamageMaxRight", &ZTLuaUnit::m_AttackDamageMaxRight)
			.addData("AttackDamageMinRight", &ZTLuaUnit::m_AttackDamageMinRight)
			.addData("CurseDamageMin", &ZTLuaUnit::m_CurseDamageMin)
			.addData("CurseDamageMax", &ZTLuaUnit::m_CurseDamageMin)
			.addData("AttackRating", &ZTLuaUnit::m_AttackRating)
			.addData("SuccessfulBlocking", &ZTLuaUnit::m_SuccessfulBlocking)
			.addData("AttackSpeed", &ZTLuaUnit::m_AttackSpeed)
			.addData("MagicSpeed", &ZTLuaUnit::m_MagicSpeed)
			.addData("Defense", &ZTLuaUnit::m_Defense)
			.addData("MagicDefense", &ZTLuaUnit::m_MagicDefense)
			.addData("PartyNumber", &ZTLuaUnit::m_PartyNumber)
			.addData("GuildNumber", &ZTLuaUnit::m_GuildNumber)
			.addData("MapNumber", &ZTLuaUnit::m_MapNumber)
			.addData("MapPosX", &ZTLuaUnit::m_MapPosX)
			.addData("MapPosY", &ZTLuaUnit::m_MapPosY)
			.addData("MoneyInventory", &ZTLuaUnit::m_MoneyInventory)
			.addData("MoneyWarehouse", &ZTLuaUnit::m_MoneyWarehouse)
			.addFunction("setAttackDamageMinRight", &ZTLuaUnit::setAttackDamageMinRight)
			.addFunction("setAttackDamageMaxRight", &ZTLuaUnit::setAttackDamageMaxRight)
			.addFunction("setAttackDamageMinLeft", &ZTLuaUnit::setAttackDamageMinLeft)
			.addFunction("setAttackDamageMaxLeft", &ZTLuaUnit::setAttackDamageMaxLeft)
			.addFunction("setMagicDamageMin", &ZTLuaUnit::setMagicDamageMin)
			.addFunction("setMagicDamageMax", &ZTLuaUnit::setMagicDamageMax)
			.addFunction("setCurseDamageMin", &ZTLuaUnit::setCurseDamageMin)
			.addFunction("setCurseDamageMax", &ZTLuaUnit::setCurseDamageMax)
			.addFunction("setAttackSpeed", &ZTLuaUnit::setAttackSpeed)
			.addFunction("setMagicSpeed", &ZTLuaUnit::setMagicSpeed)
			.addFunction("setDefense", &ZTLuaUnit::setDefense)
			.addFunction("setSuccessfulBlocking", &ZTLuaUnit::setSuccessfulBlocking)
			.addFunction("setLifePool", &ZTLuaUnit::setLifePool)
			.addFunction("setManaPool", &ZTLuaUnit::setManaPool)
			.addFunction("setStaminaPool", &ZTLuaUnit::setStaminaPool)
			.addFunction("setShieldPool", &ZTLuaUnit::setShieldPool)
			.addFunction("DebugOutput", &ZTLuaUnit::DebugOutput)
			.addFunction("DebugLog", &ZTLuaUnit::DebugLog)
		.endClass();
	}

	void setAttackDamageMinRight(int Value) {
		gObj[m_ObjectIndex].m_AttackDamageMinRight = Value;
	}

	void setAttackDamageMaxRight(int Value) {
		gObj[m_ObjectIndex].m_AttackDamageMaxRight = Value;
	}

	void setAttackDamageMinLeft(int Value) {
		gObj[m_ObjectIndex].m_AttackDamageMinLeft = Value;
	}

	void setAttackDamageMaxLeft(int Value) {
		gObj[m_ObjectIndex].m_AttackDamageMaxLeft = Value;
	}

	void setMagicDamageMin(int Value) {
		gObj[m_ObjectIndex].m_MagicDamageMin = Value;
	}

	void setMagicDamageMax(int Value) {
		gObj[m_ObjectIndex].m_MagicDamageMax = Value;
	}

	void setCurseDamageMin(int Value) {
		gObj[m_ObjectIndex].m_CurseDamageMin = Value;
	}

	void setCurseDamageMax(int Value) {
		gObj[m_ObjectIndex].m_CurseDamageMax = Value;
	}

	void setAttackSpeed(int Value) {
		gObj[m_ObjectIndex].m_AttackSpeed = Value;
	}

	void setMagicSpeed(int Value) {
		gObj[m_ObjectIndex].m_MagicSpeed = Value;
	}

	void setDefense(int Value) {
		gObj[m_ObjectIndex].m_Defense = Value;
	}

	void setSuccessfulBlocking(int Value) {
		gObj[m_ObjectIndex].m_SuccessfulBlocking = Value;
	}

	void setLifePool(float Value) {
		gObj[m_ObjectIndex].MaxLife = Value;
	}

	void setManaPool(float Value) {
		gObj[m_ObjectIndex].MaxMana = Value;
	}

	void setStaminaPool(int Value) {
		gObj[m_ObjectIndex].MaxBP = Value;
	}

	void setShieldPool(int Value) {
		gObj[m_ObjectIndex].iMaxShield = Value;
	}

	void DebugOutput(std::string Text) {
		MsgOutput(m_ObjectIndex, (char*)Text.c_str());
	}
	
	void DebugLog(std::string Text) {
		OutputDebugString(Text.c_str());
		//LogAddC(2, "[LUA] %s", (char*)Text.c_str());
	}
protected:
	void fillUnitInfo(int ObjectIndex) {
#if (ENABLETEST_ZTUNIT == 1)
		LPOBJ lpObject = &gObj[ObjectIndex];
		m_Account.assign(lpObject->AccountID);
		m_Name.assign(lpObject->Name);
		m_ObjectIndex = ObjectIndex;
		m_Class = lpObject->Class;
		m_Authority = lpObject->Authority;
		m_Type = lpObject->Type;
		m_Level = lpObject->Level;
		m_LevelPoints = lpObject->LevelUpPoint;
		m_MasterLevel = lpObject->MasterLevel;
		m_MasterPoints = lpObject->MasterPoint;
		m_Life = lpObject->Life;
		m_MaxLife = lpObject->MaxLife;
		m_Mana = lpObject->Mana;
		m_MaxMana = lpObject->MaxMana;
		m_Strength = lpObject->Strength + lpObject->AddStrength;
		m_Dexterity = lpObject->Dexterity + lpObject->AddDexterity;
		m_Vitality = lpObject->Vitality + lpObject->AddVitality;
		m_Energy = lpObject->Energy + lpObject->AddEnergy;
		m_Command = lpObject->Leadership + lpObject->AddLeadership;
		m_AttackDamageMaxLeft = lpObject->m_AttackDamageMaxLeft;
		m_AttackDamageMinLeft = lpObject->m_AttackDamageMinLeft;
		m_AttackDamageMaxRight = lpObject->m_AttackDamageMaxRight;
		m_AttackDamageMinRight = lpObject->m_AttackDamageMinRight;
		m_CurseDamageMin = lpObject->m_CurseDamageMin;
		m_CurseDamageMax = lpObject->m_CurseDamageMax;
		m_AttackRating = lpObject->m_AttackRating;
		m_SuccessfulBlocking = lpObject->m_SuccessfulBlocking;
		m_AttackSpeed = lpObject->m_AttackSpeed;
		m_MagicSpeed = lpObject->m_MagicSpeed;
		m_Defense = lpObject->m_Defense;
		m_MagicDefense = lpObject->m_MagicDefense;
		m_PartyNumber = lpObject->PartyNumber;
		m_GuildNumber = lpObject->GuildNumber;
		m_MapNumber = lpObject->MapNumber;
		m_MapPosX = lpObject->X;
		m_MapPosY = lpObject->Y;
		m_MoneyInventory = lpObject->Money;
		m_MoneyWarehouse = lpObject->WarehouseMoney;
#else
		LPOBJ lpObject = &gObj[ObjectIndex];
		m_Account.assign(lpObject->AccountID);
		m_Name.assign(lpObject->Name);
		m_ObjectIndex = ObjectIndex;
		m_Class = lpObject->Class;
		m_Authority = (BYTE)lpObject->Authority;
		m_Type = lpObject->Type;
		m_Level = lpObject->Level;
		m_LevelPoints = lpObject->LevelUpPoint;
		m_MasterLevel = lpObject->MasterLevel;
		m_MasterPoints = lpObject->MasterPoint;
		m_Life = lpObject->Life;
		m_MaxLife = lpObject->MaxLife;
		m_Mana = lpObject->Mana;
		m_MaxMana = lpObject->MaxMana;
		m_Strength = lpObject->Strength + lpObject->AddStrength;
		m_Dexterity = lpObject->Dexterity + lpObject->AddDexterity;
		m_Vitality = lpObject->Vitality + lpObject->AddVitality;
		m_Energy = lpObject->Energy + lpObject->AddEnergy;
		m_Command = lpObject->Leadership + lpObject->AddLeadership;
		m_AttackDamageMaxLeft = lpObject->m_AttackDamageMaxLeft;
		m_AttackDamageMinLeft = lpObject->m_AttackDamageMinLeft;
		m_AttackDamageMaxRight = lpObject->m_AttackDamageMaxRight;
		m_AttackDamageMinRight = lpObject->m_AttackDamageMinRight;
		m_CurseDamageMin = lpObject->m_CurseDamageMin;
		m_CurseDamageMax = lpObject->m_CurseDamageMax;
		m_AttackRating = lpObject->m_AttackRating;
		m_SuccessfulBlocking = lpObject->m_SuccessfulBlocking;
		m_AttackSpeed = lpObject->m_AttackSpeed;
		m_MagicSpeed = lpObject->m_MagicSpeed;
		m_Defense = lpObject->m_Defense;
		m_MagicDefense = lpObject->m_MagicDefense;
		m_PartyNumber = lpObject->PartyNumber;
		m_GuildNumber = lpObject->GuildNumber;
		m_MapNumber = lpObject->MapNumber;
		m_MapPosX = lpObject->X;
		m_MapPosY = lpObject->Y;
		m_MoneyInventory = lpObject->Money;
		m_MoneyWarehouse = lpObject->WarehouseMoney;
#endif
	}
};
