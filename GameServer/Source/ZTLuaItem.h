#pragma once

// copy of lua defines
enum ZTLuaItemType {
	ITEMTYPE_SWORD		= 0,
	ITEMTYPE_AXE		= 1,
	ITEMTYPE_MACE		= 2,
	ITEMTYPE_SPEAR 		= 3,
	ITEMTYPE_BOW 		= 4,
	ITEMTYPE_STAFF		= 5,
	ITEMTYPE_BOOK		= 6,
	ITEMTYPE_SHIELD		= 7,
	ITEMTYPE_ARMOR 		= 8,
	ITEMTYPE_HELM		= 9,
	ITEMTYPE_PANTS		= 10,
	ITEMTYPE_GLOVES		= 11,
	ITEMTYPE_BOOTS		= 12,
	ITEMTYPE_WINGS		= 13,
	ITEMTYPE_LORDPET	= 14,
	ITEMTYPE_PET		= 15,
	ITEMTYPE_ARROWS		= 16,
	ITEMTYPE_RING		= 17,
	ITEMTYPE_AMULET		= 18,
	ITEMTYPE_PENTAGRAM	= 19,
};

class ZTLuaItemInfo {
public:
	ZTLuaItemInfo() {}
	virtual ~ZTLuaItemInfo() {
		m_Name.clear();
	}

protected:
	std::string m_Name;
	BYTE m_Slot, m_ValidItem;
	WORD m_Type;
	DWORD m_Serial;
	BYTE m_Level, m_Luck, m_Skill, m_Option;
	BYTE m_HandType, m_Durability;
};

class ZTLuaItem : public ZTLuaItemInfo {
public:
	explicit ZTLuaItem(int ObjectIndex, BYTE EquipmentSlot) {
		if (!gObjIsConnected(ObjectIndex) 
			|| !gObj[ObjectIndex].pInventory[EquipmentSlot].IsItem()) {
			// error log
			return;
		}
		fillItemInfo(ObjectIndex, EquipmentSlot);
	}

	virtual ~ZTLuaItem() { }

	static void Bind(lua_State* Lua) {
		using namespace luabridge;
		getGlobalNamespace(Lua)
		.beginClass<ZTLuaItem>("ZTItem")
			.addConstructor<void(*) (int, BYTE)>()
			.addData("Name", &ZTLuaItem::m_Name)
			.addData("Type", &ZTLuaItem::m_Type)
			.addData("Serial", &ZTLuaItem::m_Type)
			.addData("Level", &ZTLuaItem::m_Level)
			.addData("Luck", &ZTLuaItem::m_Luck)
			.addData("Skill", &ZTLuaItem::m_Skill)
			.addData("Option", &ZTLuaItem::m_Option)
			.addData("HandType", &ZTLuaItem::m_HandType)
			.addData("Durability", &ZTLuaItem::m_Durability)
			.addFunction("isValid", &ZTLuaItem::isValid)
		.endClass();
	}

	BYTE isValid(BYTE ItemType) {
		if (!m_ValidItem) {
			return 0;
		}

		if (ItemType == ITEMTYPE_SWORD) {
			if (m_Type >= ITEMGET(0, 0) && m_Type <= ITEMGET(0, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_AXE) {
			if (m_Type >= ITEMGET(1, 0) && m_Type <= ITEMGET(1, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_MACE) {
			if (m_Type >= ITEMGET(2, 0) && m_Type <= ITEMGET(2, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_SPEAR) {
			if (m_Type >= ITEMGET(3, 0) && m_Type <= ITEMGET(3, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_BOW) {
			if (m_Type >= ITEMGET(4, 0) && m_Type <= ITEMGET(4, MAX_SUBTYPE_ITEMS)) {
				// maybe its bolt or arrow?
				if (m_Type == ITEMGET(4, 7) || m_Type == ITEMGET(4, 15)) {
					return 0;
				}
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_STAFF) {
			if (m_Type >= ITEMGET(5, 0) && m_Type <= ITEMGET(5, MAX_SUBTYPE_ITEMS)) {
				// maybe its book?
				if (m_Type == ITEMGET(5, 21) || m_Type == ITEMGET(5, 22)
					|| m_Type == ITEMGET(5, 23) || m_Type == ITEMGET(5, 40)) {
					return 0;
				}
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_BOOK) {
			if (m_Type >= ITEMGET(5, 0) && m_Type <= ITEMGET(5, MAX_SUBTYPE_ITEMS)) {
				// maybe its staff?
				if (m_Type != ITEMGET(5, 21) && m_Type != ITEMGET(5, 22)
					&& m_Type != ITEMGET(5, 23) && m_Type != ITEMGET(5, 40)) {
					return 0;
				}
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_SHIELD) {
			if (m_Type >= ITEMGET(6, 0) && m_Type <= ITEMGET(6, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_ARMOR) {
			if (m_Type >= ITEMGET(8, 0) && m_Type <= ITEMGET(8, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_HELM) {
			if (m_Type >= ITEMGET(7, 0) && m_Type <= ITEMGET(7, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_PANTS) {
			if (m_Type >= ITEMGET(9, 0) && m_Type <= ITEMGET(9, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_GLOVES) {
			if (m_Type >= ITEMGET(10, 0) && m_Type <= ITEMGET(10, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		else if (ItemType == ITEMTYPE_BOOTS) {
			if (m_Type >= ITEMGET(11, 0) && m_Type <= ITEMGET(11, MAX_SUBTYPE_ITEMS)) {
			}
			else {
				return 0;
			}
		}
		// finish me and migrate to lua, for what i here?

		return 1;
	}

protected:
	void fillItemInfo(int ObjectIndex, BYTE EquipmentSlot) {
		CItem* lpItem = &gObj[ObjectIndex].pInventory[EquipmentSlot];
		m_Name.assign(lpItem->GetName());
		m_Slot = EquipmentSlot;
		m_ValidItem = lpItem->m_IsValidItem;
		m_Type = lpItem->m_Type;
		m_Serial = lpItem->m_Number;
		m_Level = (BYTE)lpItem->m_Level;
		m_Luck = lpItem->m_Option1;
		m_Skill = lpItem->m_Option2;
		m_Option = lpItem->m_Option3;
		m_HandType = lpItem->m_TwoHand;
		m_Durability = (BYTE)lpItem->m_Durability;
	}
};
