#ifndef API_H
#define API_H

#include "lib/lua52/lua.h"
#include "lib/lua52/lauxlib.h"
#include "lib/lua52/lualib.h"

#define API_TYPE_FONT "Font"
#define API_TYPE_PROCESS "Process"
#define API_TYPE_DIRMONITOR "Dirmonitor"
#define API_TYPE_NATIVE_PLUGIN "NativePlugin"

#define API_CONSTANT_DEFINE(L, idx, key, n) (lua_pushnumber(L, n), lua_setfield(L, idx - 1, key))

void api_load_libs(lua_State *L);

#endif
