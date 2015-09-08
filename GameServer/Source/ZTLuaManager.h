#pragma once

#include "stdafx.h"
#include "..\Lua\include\lua.hpp"
#include "..\LuaBridge\LuaBridge.h"
#include "ZTExpress.h"
#include "ZTLuaUnit.h"
#include "ZTLuaItem.h"
#include "ZTLuaBackup.h"
#include "ZTLuaUtil.h"
#include "ZTLogger.h"

using namespace luabridge;

#define ZTLUA_WARNING	0
#define ZTLUA_ERROR		1
#define ZTLUA_ISSUE		0xDEAD

class ZTLuaManager {
public:
	ZTLuaManager(const char* ScriptFile) : m_Lua(luaL_newstate()) {
		luaL_openlibs(m_Lua);
		ZTLuaUtil::Bind(m_Lua);
		m_Ref = new LuaRef(m_Lua);
		m_ScriptFile = std::string(gDirPath.GetNewPath((char*)ScriptFile));
		execString(g_LuaBackup.get(m_ScriptFile.c_str()).c_str());
	}

    ZTLuaManager(const ZTLuaManager&);
    ZTLuaManager& operator=(const ZTLuaManager&);

    ~ZTLuaManager() {
		if (m_Ref) {
			delete m_Ref;
			m_Ref = NULL;
		}
		lua_close(m_Lua);
	}

	lua_State* getLua() {
		return m_Lua;
	}

	LuaRef* getRef() {
		return m_Ref;
	}

	std::string getScriptName() {
		return m_ScriptFile;
	}

	inline static void Delete(ZTLuaManager* LuaMng) {
		if (LuaMng) {
			delete LuaMng;
			LuaMng = NULL;
		}
	}

	void execFile(const char* ScriptFile) {
		if (ScriptFile == NULL) {
			return;
		}
		Report(luaL_dofile(m_Lua, ScriptFile));
	}
	
	bool execString(const char* Text) {
		if (Text == NULL) {
			return false;
		}
		Report(luaL_dostring(m_Lua, Text));
		return true;
	}

	void execLive() {
		execFile(m_ScriptFile.c_str());
	}

	void Read(char const* Text) {
		try {
			*m_Ref = getGlobal(m_Lua, Text);
		}
		catch (luabridge::LuaException ex) {
			Report(ex.what(), ZTLUA_WARNING);
			try {
				*m_Ref = getGlobal(m_Lua, Text);
			}
			catch (luabridge::LuaException ex) {
				Report(ex.what(), ZTLUA_ERROR);
				*m_Ref = NULL;
			}
		}
	}

	void Call(char const* Text) {
		try {
			*m_Ref = getGlobal(m_Lua, Text)();
		}
		catch (luabridge::LuaException ex) {
			Report(ex.what(), ZTLUA_WARNING);
			try {
				*m_Ref = getGlobal(m_Lua, Text)();
			}
			catch (luabridge::LuaException ex) {
				Report(ex.what(), ZTLUA_ERROR);
				*m_Ref = NULL;
			}
		}
	}

	template <class P1>
	void Call(char const* Text, P1 p1) {
		try {
			*m_Ref = getGlobal(m_Lua, Text)(p1);
		}
		catch (luabridge::LuaException ex) {
			Report(ex.what(), ZTLUA_WARNING);
			try {
				*m_Ref = getGlobal(m_Lua, Text)(p1);
			}
			catch (luabridge::LuaException ex) {
				Report(ex.what(), ZTLUA_ERROR);
				*m_Ref = NULL;
			}
		}
	}

	template <class P1, class P2>
	void Call(char const* Text, P1 p1, P2 p2) {
		try {
			*m_Ref = getGlobal(m_Lua, Text)(p1, p2);
		}
		catch (luabridge::LuaException ex) {
			Report(ex.what(), ZTLUA_WARNING);
			try {
				execBackup();
				*m_Ref = getGlobal(m_Lua, Text)(p1, p2);
			}
			catch (luabridge::LuaException ex) {
				Report(ex.what(), ZTLUA_ERROR);
				*m_Ref = NULL;
			}
		}
	}

	template <class P1, class P2, class P3>
	void Call(char const* Text, P1 p1, P2 p2, P3 p3) {
		try {
			*m_Ref = getGlobal(m_Lua, Text)(p1, p2, p3);
		}
		catch (luabridge::LuaException ex) {
			Report(ex.what(), ZTLUA_WARNING);
			try {
				*m_Ref = getGlobal(m_Lua, Text)(p1, p2, p3);
			}
			catch (luabridge::LuaException ex) {
				Report(ex.what(), ZTLUA_ERROR);
				*m_Ref = NULL;
			}
		}
	}

	template <class P1, class P2, class P3, class P4>
	void Call(char const* Text, P1 p1, P2 p2, P3 p3, P4 p4) {
		try {
			*m_Ref = getGlobal(m_Lua, Text)(p1, p2, p3, p4);
		}
		catch (luabridge::LuaException ex) {
			Report(ex.what(), ZTLUA_WARNING);
			try {
				execBackup();
				*m_Ref = getGlobal(m_Lua, Text)(p1, p2, p3, p4);
			}
			catch (luabridge::LuaException ex) {
				Report(ex.what(), ZTLUA_ERROR);
				*m_Ref = NULL;
			}
		}
	}

	template <class P1, class P2, class P3, class P4, class P5>
	void Call(char const* Text, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
		try {
			*m_Ref = getGlobal(m_Lua, Text)(p1, p2, p3, p4, p5);
		}
		catch (luabridge::LuaException ex) {
			Report(ex.what(), ZTLUA_WARNING);
			try {
				execBackup();
				*m_Ref = getGlobal(m_Lua, Text)(p1, p2, p3, p4, p5);
			}
			catch (luabridge::LuaException ex) {
				Report(ex.what(), ZTLUA_ERROR);
				*m_Ref = NULL;
			}
		}
	}

	void Report(int State) {
		if (State != 0) {
			//LogAddC(0, "Report: %s\n", lua_tostring(m_Lua, State));
			MessageBoxA(NULL, lua_tostring(m_Lua, State), "ZTLuaManager", MB_OK | MB_APPLMODAL);
			LuaLog.addLog("Issue: %s\n", lua_tostring(m_Lua, State));
			lua_pop(m_Lua, 1);
		}
	}

	void Report(const char* Text, BYTE Type) {
		if (Type == ZTLUA_WARNING) {
			LogAddC(1, "Warning: %s\n", Text);
			LuaLog.addLog("Warning: %s\n", Text);
		} else if (Type == ZTLUA_ERROR) {
			LogAddC(2, "Error: %s\n", Text);
			LuaLog.addLog("Error: %s\n", Text);
			MessageBoxA(NULL, Text, "ZTLuaManager", MB_OK | MB_APPLMODAL);
		} else {
			LuaLog.addLog("Issue: %s\n", Text);
		}		
	}

private:
	lua_State* m_Lua;
	LuaRef* m_Ref;
	std::string m_ScriptFile;
};