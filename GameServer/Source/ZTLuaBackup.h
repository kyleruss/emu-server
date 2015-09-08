#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <iterator>
#include <fstream>
#include "ZTExpress.h"
#include "ZTLogger.h"
#include "GameMain.h"

struct ZTLuaScriptMember {
	std::string ScriptFile;
	std::string ScriptBody;
};

class ZTLuaBackup {
public:
	ZTLuaBackup() {
		THREAD_LOCK_CREATE(&m_CriticalSection);
	};

	~ZTLuaBackup() {
		THREAD_LOCK_DELETE(&m_CriticalSection);
	};

	void Load() {
		read(gDirPath.GetNewPath(FILE_LUA_CHARACTERBASICS));
		read(gDirPath.GetNewPath(FILE_LUA_SHAREDINFORMATION));
	}

	void Reload() {
		for (int i = 0; i < m_ScriptCollection.size(); i++) {
			m_ScriptCollection[i].ScriptBody.clear();
			THREAD_LOCK_START(&m_CriticalSection);
			std::ifstream ScriptStream(m_ScriptCollection[i].ScriptFile.c_str());
			if (!ScriptStream) {
				THREAD_LOCK_END(&m_CriticalSection);
				LuaLog.addLog("Script file not found: %s", m_ScriptCollection[i].ScriptFile.c_str());
				return;
			}
			ScriptStream.close();
			THREAD_LOCK_END(&m_CriticalSection);
			LuaLog.addLog("Script reloaded: %s", m_ScriptCollection[i].ScriptFile.c_str());
		}
	}

	void read(const char* ScriptFile) {
		THREAD_LOCK_START(&m_CriticalSection);
		std::ifstream ScriptStream(ScriptFile);
		if (!ScriptStream) {
			THREAD_LOCK_END(&m_CriticalSection);
			LuaLog.addLog("Script file not found: %s", ScriptFile);
			return;
		}
		THREAD_LOCK_END(&m_CriticalSection);
		ZTLuaScriptMember Member;
		Member.ScriptFile.assign(ScriptFile);
		Member.ScriptBody = std::string((std::istreambuf_iterator<char>(ScriptStream)), 
			std::istreambuf_iterator<char>());
		m_ScriptCollection.push_back(Member);
		LuaLog.addLog("Script loaded: %s", ScriptFile);
	}

	std::string get(const char* ScriptFile) {
		for (int i = 0; i < m_ScriptCollection.size(); i++) {
			if (!strcmp(m_ScriptCollection[i].ScriptFile.c_str(), ScriptFile)) {
				return m_ScriptCollection[i].ScriptBody;
			}
		}
		LuaLog.addLog("Script not found: %s", ScriptFile);
		return NULL;
	}

private:
	std::vector<ZTLuaScriptMember> m_ScriptCollection;
	CRITICAL_SECTION m_CriticalSection;
};

extern ZTLuaBackup g_LuaBackup;