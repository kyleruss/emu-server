// ------------------------------
// Decompiled by Hybrid
// 1.01.00
// ------------------------------

#include "stdafx.h"
#include "LuaFun.h"

#include "logproc.h"

static int Debug_Print(lua_State *L)
{
#ifdef _DEBUG
	// here must be some code
	// no code in executable, since it was build without _DEBUG
#endif

	return 0;
}

void Debug_AddDebugPrintTemp(lua_State *L)
{
	luaL_reg DebugGlue[] =
	{
		{"DebugPrint", Debug_Print},
		{NULL, NULL}
	};


	for (int i = 0; DebugGlue[i].name; ++i )
	{
		lua_pushcclosure(L, DebugGlue[i].func, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, DebugGlue[i].name);
	}
}


bool g_Generic_Call(lua_State *L, const char* func, const char* sig, ...)
{
	va_list vl;
	int narg, nres;  /* number of arguments and results */
    
	va_start(vl, sig);
	// lua_setfield(L, LUA_GLOBALSINDEX, func); -- webzen error ? ... 
	lua_getglobal(L,func);
    
	/* push arguments */
	narg = 0;
	while (*sig) 
	{  
		/* push arguments */
		switch (*sig++) 
		{
			case 'w':  /* word argument */
				lua_pushnumber(L, va_arg(vl, short));
			break;

			case 'd':  /* double argument */
				lua_pushnumber(L, va_arg(vl, double));
            break;
    
			case 'i':  /* int argument */
				lua_pushnumber(L, va_arg(vl, int));
            break;
    
			case 's':  /* string argument */
			{
				char* pszString = va_arg(vl, char *);
				lua_pushlstring(L, pszString, strlen(pszString));
			}
            break;
    
			case '>':
				goto endwhile;
    
			default:
				LogAdd("luacall_Generic_Call invalid option (%c)", *(sig - 1));
			return false;
        }
        narg++;
        luaL_checkstack(L, 1, "too many arguments");
	} 
endwhile:
    
	/* do the call */
	nres = strlen(sig);  /* number of expected results */

	if (lua_pcall(L, narg, nres, 0) != 0)  /* do the call */
	{
		LogAdd("luacall_Generic_Call error running function `%s': %s", func, lua_tolstring(L, -1, NULL));
		return false;
	}
    
	/* retrieve results */
	int nRetValCnt = nres;
	nres = -nres;  /* stack index of first result */
	while (*sig) 
	{  
		/* get results */
		switch (*sig++) 
		{
    
			case 'd':  /* double result */
				if (!lua_isnumber(L, nres))
				{
					LogAdd("luacall_Generic_Call wrong result type [%c][%d]", *(sig-1), nres);
					return false;
				}
				*va_arg(vl, double *) = lua_tonumber(L, nres);
            break;
    
			case 'i':  /* int result */
				if (!lua_isnumber(L, nres))
				{
					LogAdd("luacall_Generic_Call wrong result type [%c][%d]", *(sig-1), nres);
					return false;
				}
				*va_arg(vl, int *) = (int)lua_tonumber(L, nres);
            break;
    
			case 's':  /* string result */
				if (!lua_isstring(L, nres))
				{
					LogAdd("luacall_Generic_Call wrong result type [%c][%d]", *(sig-1), nres);
					return false;
				}
				*va_arg(vl, const char **) = lua_tolstring(L, nres, NULL);
            break;
    
			default:
				LogAdd("luacall_Generic_Call invalid option (%c)", *(sig - 1));
			return false;
		}
        nres++;
	}

	if(nRetValCnt)
		lua_settop(L, -nRetValCnt-1);

//	va_end(vl);

	return true;
}