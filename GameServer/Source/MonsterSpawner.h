#pragma once

// import
#include "user.h"

// data
struct MonsterSpawnerItemInfo {
	short itemIndex;
	short itemCategory;
	short spawnMonsterId;
	short spawnMonsterCount;
	short spawnDuration;
	bool isPrivate;
	bool isPrivateParty;
	bool isPrivateGuild;
};

struct MonsterSpawnerWorkNode { // maybe better use class & ptr instance...
	MonsterSpawnerWorkNode() {
		ownerName.reserve(MAX_ACCOUNT_LEN + 1);
		monsterPool.clear();
		itemCode = -1;
		tickEnd = 0;
	}
	void setSlot(short UserIndex, MonsterSpawnerItemInfo* itemInfo) {
		if (UserIndex < 0 || UserIndex > OBJMAX) {
			return;
		}
		LPOBJ tmpUser = &gObj[UserIndex];
		if (tmpUser == NULL) {
			return;
		}
		ownerName.assign(tmpUser->Name);
		itemCode = ITEMGET(itemInfo->itemCategory, itemInfo->itemIndex);
		tickEnd = GetTickCount() + (itemInfo->spawnDuration * 60 * 1000);
		setSpot(UserIndex, itemInfo);
	}
	void clearSlot() {
		ownerName.assign("");
		itemCode = -1;
		tickEnd = 0;
		clearSpot();
		monsterPool.clear();
	}
	void setSpot(short UserIndex, MonsterSpawnerItemInfo* itemInfo) {
		if (UserIndex < 0 || UserIndex > OBJMAX) {
			return;
		}
		LPOBJ tmpUser = &gObj[UserIndex];
		if (tmpUser == NULL) {
			return;
		}
		for (int i = 0; i < itemInfo->spawnMonsterCount; i++) {
			int tmpMonsterIndex = gObjAddMonster(tmpUser->MapNumber);
			if (tmpMonsterIndex < 0) {
				return;
			}
			gObj[tmpMonsterIndex].m_PosNum = -1;
			gObj[tmpMonsterIndex].X	= tmpUser->X + rand() % 2;
			gObj[tmpMonsterIndex].Y	= tmpUser->Y + rand() % 2;
			gObj[tmpMonsterIndex].MapNumber	= tmpUser->MapNumber;
			gObj[tmpMonsterIndex].TX = gObj[tmpMonsterIndex].X;
			gObj[tmpMonsterIndex].TY = gObj[tmpMonsterIndex].Y;
			gObj[tmpMonsterIndex].m_OldX = gObj[tmpMonsterIndex].X;
			gObj[tmpMonsterIndex].m_OldY = gObj[tmpMonsterIndex].Y;
			gObj[tmpMonsterIndex].StartX = gObj[tmpMonsterIndex].X;
			gObj[tmpMonsterIndex].StartY = gObj[tmpMonsterIndex].Y;
			gObj[tmpMonsterIndex].m_ActState.Emotion = 1;
			gObj[tmpMonsterIndex].m_ActState.EmotionCount = 15;
			gObj[tmpMonsterIndex].Dir = rand() % 8;
			gObjSetMonster(tmpMonsterIndex, itemInfo->spawnMonsterId);
			monsterPool.push_back(tmpMonsterIndex);
		}
	}
	void clearSpot() {
		for (int i = 0; i < monsterPool.size(); i++) {
			gObjViewportListProtocolDestroy(&gObj[monsterPool[i]]);
			gObjViewportClose(&gObj[monsterPool[i]]);
			gObjDel(monsterPool[i]);
			gObj[monsterPool[i]].Live = 0;
			gObj[monsterPool[i]].DieRegen = 1;
		}
	}
	bool isOwner(char* OwnerName) {
		if (ownerName.compare(OwnerName) == NULL) {
			return true;
		}
		return false;
	}
	bool isMonster(short MonsterIndex) {
		for (int i = 0; i < monsterPool.size(); i++) {
			if (monsterPool[i] == MonsterIndex) {
				return true;
			}
		}
		return false;
	}
	bool isEmpty() {
		if (isOwner("")) {
			return true;
		}
		return false;
	}
	bool isExpired() {
		if (GetTickCount() >= tickEnd) {
			return true;
		}
		return false;
	}
	std::string ownerName;
	std::vector<short> monsterPool;
	int itemCode;
	DWORD tickEnd;
};

// monster spawner by item drop impletation
class MonsterSpawnerMng {
public:
	MonsterSpawnerMng();
	~MonsterSpawnerMng();

	void Load();
	void Init();
	void Read(const char* File);

	void procRun();
	void procRegen(short MonsterIndex);
	bool procCreate(short UserIndex, int ItemCode);
	
	bool isPrivate(short UserIndex, short MonsterIndex);
	bool isEventMonster(short MonsterIndex);
	bool isKeyItem(int ItemCode);
	int getEmptySlot();
	MonsterSpawnerItemInfo* getItemInfo(int ItemCode);

	// singleton
	static MonsterSpawnerMng* getInstance() {
		if (m_Instance == NULL) {
			m_Instance = new MonsterSpawnerMng();
		}
		return m_Instance;
	}

private:
	static MonsterSpawnerMng* m_Instance;
	std::vector<MonsterSpawnerWorkNode> m_WorkPool;
	std::vector<MonsterSpawnerItemInfo> m_ItemData;

private:
	bool m_Loaded;
};