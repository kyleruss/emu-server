#include "stdafx.h"
#include "MonsterSpawner.h"
#include "GameMain.h"
#include "..\pugixml\pugixml.hpp"

MonsterSpawnerMng* MonsterSpawnerMng::m_Instance = NULL;

MonsterSpawnerMng::MonsterSpawnerMng() {
	m_WorkPool.clear();
	//m_ItemData.clear();
}

MonsterSpawnerMng::~MonsterSpawnerMng() {
	// lifetime instance...
}

void MonsterSpawnerMng::Load() {
	m_Loaded = false;
	Init();
	Read(gDirPath.GetNewPath(FILE_CUSTOM_MONSTERSPAWNER));
}

void MonsterSpawnerMng::Init() {
	m_ItemData.clear();
}

void MonsterSpawnerMng::Read(const char *File) {
	using namespace pugi;
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	if (Result.status != status_ok) {
		MsgBox("[MonsterSpawnerMng] File %s not found!", File);
		return;
	}
	xml_node nodeMain = Document.child("monsterspawner");
	for (xml_node nodeIt = nodeMain.child("item"); nodeIt; nodeIt = nodeIt.next_sibling()) {
		MonsterSpawnerItemInfo newItem = { 0 };
		newItem.itemCategory = nodeIt.attribute("type").as_int(-1);
		newItem.itemIndex = nodeIt.attribute("index").as_int(-1);
		newItem.spawnMonsterId = nodeIt.child("spawn").attribute("monster").as_int(-1);
		newItem.spawnMonsterCount = nodeIt.child("spawn").attribute("count").as_int(-1);
		newItem.spawnDuration = nodeIt.child("spawn").attribute("duration").as_int(-1);
		newItem.isPrivate = nodeIt.child("private").attribute("active").as_bool();
		newItem.isPrivateParty = nodeIt.child("private").child("party").text().as_bool();
		newItem.isPrivateGuild = nodeIt.child("private").child("guild").text().as_bool();
		m_ItemData.push_back(newItem);
	}
	LogAddTD("[MonsterSpawnerMng] loaded %d node(s)", m_ItemData.size());
	m_Loaded = true;
}

void MonsterSpawnerMng::procRun() {
	for (int i = 0; i < m_WorkPool.size(); i++) {
		if (m_WorkPool[i].isEmpty()) {
			continue;
		}
		if (m_WorkPool[i].isExpired()) {
			LogAddTD("[MonsterSpawnerMng] [%s] spot duration is expired, slot cleared",
				m_WorkPool[i].ownerName.c_str());
			m_WorkPool[i].clearSlot();
		}
	}
}

void MonsterSpawnerMng::procRegen(short MonsterIndex) {
	gObj[MonsterIndex].X = gObj[MonsterIndex].StartX;
	gObj[MonsterIndex].Y = gObj[MonsterIndex].StartY;
	gObj[MonsterIndex].MTX = gObj[MonsterIndex].X;
	gObj[MonsterIndex].MTY = gObj[MonsterIndex].Y;
	gObj[MonsterIndex].m_ActState.Emotion = 1;
	gObj[MonsterIndex].m_ActState.EmotionCount = 1;
}

bool MonsterSpawnerMng::procCreate(short UserIndex, int ItemCode) {
	MonsterSpawnerItemInfo* tmpItemInfo = getItemInfo(ItemCode);
	if (tmpItemInfo == NULL) {
		return false;
	}
	int tmpSlotIndex = getEmptySlot();
	if (tmpSlotIndex == -1) {
		MonsterSpawnerWorkNode newNode;
		newNode.setSlot(UserIndex, tmpItemInfo);
		m_WorkPool.push_back(newNode);
	} else {
		m_WorkPool[tmpSlotIndex].setSlot(UserIndex, tmpItemInfo);
	}
	LogAddTD("[MonsterSpawnerMng] [%s][%s] spot has been created (monster: %d, count: %d, duration: %d, map: %d (%d:%d))", 
		gObj[UserIndex].AccountID, gObj[UserIndex].Name, tmpItemInfo->spawnMonsterId, tmpItemInfo->spawnMonsterCount,
		tmpItemInfo->spawnDuration, gObj[UserIndex].MapNumber, gObj[UserIndex].X, gObj[UserIndex].Y);
	return true;
}

bool MonsterSpawnerMng::isPrivate(short UserIndex, short MonsterIndex) {
	LPOBJ tmpAttacker = &gObj[UserIndex];
	LPOBJ tmpMonster = &gObj[MonsterIndex];
	LPOBJ tmpOwner = NULL;
	MonsterSpawnerWorkNode* tmpNode = NULL;
	MonsterSpawnerItemInfo* tmpItem = NULL;
	for (int i = 0; i < m_WorkPool.size(); i++) { // select node
		if (m_WorkPool[i].isMonster(MonsterIndex)) {
			tmpNode = &m_WorkPool[i];
			break;
		}
	}
	if (tmpNode == NULL) { // node not founded, spot unblocked
		return false;
	}
	if (tmpNode->isOwner(tmpAttacker->Name)) { // owner
		return false;
	}
	tmpItem = getItemInfo(tmpNode->itemCode);
	if (tmpItem == NULL) { // rules for node not founded, spot unblocked
		return false;
	}
	if (!tmpItem->isPrivate) { // spot not privated by rules
		return false;
	}
	for (int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++) { // select live owner
		if (gObj[i].Connected >= PLAYER_PLAYING) {
			if (tmpNode->isOwner(gObj[i].Name)) {
				tmpOwner = &gObj[i];
				break;
			}
		}
	}
	if (tmpOwner == NULL) { // spot have owner, but owner is offline, cant check rules - block by rules
		MsgOutput(UserIndex, "Monster is privated by %s", tmpNode->ownerName.c_str());
		return true;
	}
	if (tmpItem->isPrivateParty && tmpOwner->PartyNumber >= 0) { // spot is not privated for party members
		if (tmpOwner->PartyNumber == tmpAttacker->PartyNumber) {
			return false;
		}
	}
	if (tmpItem->isPrivateGuild && tmpOwner->GuildNumber > 0) { // spot is not privated for guild members
		if (tmpOwner->GuildNumber == tmpAttacker->GuildNumber) {
			return false;
		}
	}
	MsgOutput(UserIndex, "Monster is privated by %s", tmpNode->ownerName.c_str());
	return true;
}

bool MonsterSpawnerMng::isEventMonster(short MonsterIndex) {
	for (int i = 0; i < m_WorkPool.size(); i++) {
		if (m_WorkPool[i].isMonster(MonsterIndex)) {
			return true;
		}
	}
	return false;
}

bool MonsterSpawnerMng::isKeyItem(int ItemCode) {
	if (getItemInfo(ItemCode) != NULL) {
		return true;
	}
	return false;
}

int MonsterSpawnerMng::getEmptySlot() {
	for (int i = 0; i < m_WorkPool.size(); i++) {
		if (m_WorkPool[i].isEmpty()) {
			return i;
		}
	}
	return -1;
}

MonsterSpawnerItemInfo* MonsterSpawnerMng::getItemInfo(int ItemCode) {
	if (!m_Loaded) {
		return NULL;
	}
	for (int i = 0; i < m_ItemData.size(); i++) {
		if (ITEMGET(m_ItemData[i].itemCategory, m_ItemData[i].itemIndex) == ItemCode) {
			return &m_ItemData[i];
		}
	}
	return NULL;
}