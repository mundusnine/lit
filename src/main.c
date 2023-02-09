#include <stdio.h>
#include <stdlib.h>
#include <kinc/system.h>
#include <kinc/display.h>
#include <krink/system.h>
#include <krink/eventhandler.h>
#include "api/api.h"
#include "renderer.h"

#ifdef _WIN32
  #include <windows.h>
#elif __linux__
  #include <unistd.h>
#elif __APPLE__
  #include <mach-o/dyld.h>
#endif


static double get_scale(void) {
  kinc_display_mode_t mode = kinc_display_current_mode(kinc_primary_display());
#if _WIN32
  return mode.pixels_per_inch / 96.0;
#else
  return 1.0;
#endif
}


static void get_exe_filename(char *buf, int sz) {
#if _WIN32
  int len = GetModuleFileName(NULL, buf, sz - 1);
  buf[len] = '\0';
#elif __linux__
  char path[512];
  sprintf(path, "/proc/%d/exe", getpid());
  int len = readlink(path, buf, sz - 1);
  buf[len] = '\0';
#elif __APPLE__
  unsigned size = sz;
  _NSGetExecutablePath(buf, &size);
#else
  strcpy(buf, "./lit");
#endif
}

extern void event_handler(kr_evt_event_t event);
lua_State *L = NULL;

void update(void* data){
    (void) luaL_dostring(L,
    "xpcall(kore.run\n"
    ", function(err)\n"
    "  print('Error: ' .. tostring(err))\n"
    "  print(debug.traceback(nil, 2))\n"
    "  if kore and kore.on_error then\n"
    "    pcall(kore.on_error, err)\n"
    "  end\n"
    "  os.exit(1)\n"
    "end)");
}

int kickstart(int argc, char **argv) {
#ifdef _WIN32
  HINSTANCE lib = LoadLibrary("user32.dll");
  int (*SetProcessDPIAware)() = (void*) GetProcAddress(lib, "SetProcessDPIAware");
  SetProcessDPIAware();
#endif

  kinc_display_init();
  kinc_display_mode_t dm = kinc_display_current_mode(kinc_primary_display());

  
  kinc_init("lit",dm.width * 0.8,dm.height * 0.8,NULL,NULL);
  size_t mem_size = 1024 * 1024 * 1024;// 1 Gig
  void* memblck = malloc(mem_size);
  kr_init(memblck,mem_size,NULL,0);
  kr_evt_init();
  kr_evt_add_observer(event_handler);

  ren_init();

  L = luaL_newstate();
  luaL_openlibs(L);
  api_load_libs(L);


  lua_newtable(L);
  for (int i = 0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i + 1);
  }
  lua_setglobal(L, "ARGS");

  lua_pushstring(L, "1.11");
  lua_setglobal(L, "VERSION");

  lua_pushstring(L, kinc_system_id());
  lua_setglobal(L, "PLATFORM");

  lua_pushnumber(L, get_scale());
  lua_setglobal(L, "SCALE");

  char exename[2048];
  get_exe_filename(exename, sizeof(exename));
  lua_pushstring(L, exename);
  lua_setglobal(L, "EXEFILE");

  lua_pushnumber(L,0);
  lua_setglobal(L, "kore");

  (void) luaL_dostring(L,
    "xpcall(function()\n"
    "  SCALE = tonumber(os.getenv(\"LITE_SCALE\")) or SCALE\n"
    "  PATHSEP = package.config:sub(1, 1)\n"
    "  EXEDIR = EXEFILE:match(\"^(.+)[/\\\\].*$\")\n"
    "  package.path = EXEDIR .. '/data/?.lua;' .. package.path\n"
    "  package.path = EXEDIR .. '/data/?/init.lua;' .. package.path\n"
    "  kore = require('core')\n"
    "  kore.init()\n"
    "end, function(err)\n"
    "  print('Error: ' .. tostring(err))\n"
    "  print(debug.traceback(nil, 2))\n"
    "  if kore and kore.on_error then\n"
    "    pcall(kore.on_error, err)\n"
    "  end\n"
    "  os.exit(1)\n"
    "end)");
  kinc_set_update_callback(update,NULL);

  kinc_start();


  lua_close(L);
  
  free(memblck);
  return EXIT_SUCCESS;
}
