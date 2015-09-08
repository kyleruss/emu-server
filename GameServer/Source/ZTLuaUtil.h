#pragma once

#include "stdafx.h"
#include "..\Lua\include\lua.hpp"
#include "..\LuaBridge\LuaBridge.h"
#include "GameMain.h"

class ZTLuaUtil	{
public:
	static void Bind(lua_State* Lua) {
		using namespace luabridge;

		getGlobalNamespace(Lua)
		.beginClass<ZTLuaUtil>("ZTUtil")
			.addStaticFunction("Include", &ZTLuaUtil::Include)
		.endClass();
	}

	static void Include(lua_State* Lua, const char* File) {
		int State = luaL_dofile(Lua, gDirPath.GetNewPath((char*)File));
		if (State != 0) {
			char ErrorText[MAX_PATH + 100] = { 0 };
			sprintf(ErrorText, "File %s not found", File);
			MessageBoxA(NULL, ErrorText, "ZTLuaUtil", MB_OK | MB_APPLMODAL);
			lua_pop(Lua, 1);
		}
	}
};