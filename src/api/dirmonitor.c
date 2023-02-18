#include "api.h"
#include <kinc/threads/mutex.h>
#include <krink/eventhandler.h>
#include <kinc/threads/thread.h>
#include <krink/memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct dirmonitor {
  kinc_thread_t* thread;
  kinc_mutex_t* mutex;
  char buffer[64512];
  volatile int length;
  struct dirmonitor_internal* internal;
};


struct dirmonitor_internal* init_dirmonitor();
void deinit_dirmonitor(struct dirmonitor_internal*);
int get_changes_dirmonitor(struct dirmonitor_internal*, char*, int);
int translate_changes_dirmonitor(struct dirmonitor_internal*, char*, int, int (*)(int, const char*, void*), void*);
int add_dirmonitor(struct dirmonitor_internal*, const char*);
void remove_dirmonitor(struct dirmonitor_internal*, int);
int get_mode_dirmonitor();


static int f_check_dir_callback(int watch_id, const char* path, void* L) {
  lua_pushvalue(L, -1);
  if (path)
    lua_pushlstring(L, path, watch_id);
  else
    lua_pushnumber(L, watch_id);
  lua_call(L, 1, 1);
  int result = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return !result;
}

extern void event_handler(kr_evt_event_t event);

static void dirmonitor_check_thread(void* data) {
  struct dirmonitor* monitor = data;
  while (monitor->length >= 0) {
    if (monitor->length == 0) {
      int result = get_changes_dirmonitor(monitor->internal, monitor->buffer, sizeof(monitor->buffer));
      kinc_mutex_lock(monitor->mutex);
      if (monitor->length == 0)
        monitor->length = result;
      kinc_mutex_unlock(monitor->mutex);
    }
    //@TODO(JSN): Determine if we need this...
    // kr_evt_event_t evt = {0};
    // evt.event = KR_EVT_DIR_EVT;
    // event_handler(evt);
    // SDL_Delay(1);
    // SDL_Event event = { .type = DIR_EVENT_TYPE };
    // SDL_PushEvent(&event);
  }
}


static int f_dirmonitor_new(lua_State* L) {
  struct dirmonitor* monitor = lua_newuserdata(L, sizeof(struct dirmonitor));
  luaL_setmetatable(L, API_TYPE_DIRMONITOR);
  memset(monitor, 0, sizeof(struct dirmonitor));
  monitor->mutex = kr_malloc(sizeof(kinc_mutex_t));
  kinc_mutex_init(monitor->mutex);
  monitor->internal = init_dirmonitor();
  return 1;
}


static int f_dirmonitor_gc(lua_State* L) {
  struct dirmonitor* monitor = luaL_checkudata(L, 1, API_TYPE_DIRMONITOR);
  kinc_mutex_lock(monitor->mutex);
  monitor->length = -1;
  deinit_dirmonitor(monitor->internal);
  kinc_mutex_unlock(monitor->mutex);
  kinc_thread_wait_and_destroy(monitor->thread);
  return 0;
}


static int f_dirmonitor_watch(lua_State *L) {
  struct dirmonitor* monitor = luaL_checkudata(L, 1, API_TYPE_DIRMONITOR);
  lua_pushnumber(L, add_dirmonitor(monitor->internal, luaL_checkstring(L, 2)));
  if (!monitor->thread){
    monitor->thread = kr_malloc(sizeof(kinc_thread_t));//thread name: "dirmonitor_check_thread"
    kinc_thread_init(monitor->thread,dirmonitor_check_thread, monitor);
  }
  return 1;
}


static int f_dirmonitor_unwatch(lua_State *L) {
  remove_dirmonitor(((struct dirmonitor*)luaL_checkudata(L, 1, API_TYPE_DIRMONITOR))->internal, lua_tonumber(L, 2));
  return 0;
}


static int f_dirmonitor_check(lua_State* L) {
  struct dirmonitor* monitor = luaL_checkudata(L, 1, API_TYPE_DIRMONITOR);
  kinc_mutex_lock(monitor->mutex);
  if (monitor->length < 0)
    lua_pushnil(L);
  else if (monitor->length > 0) {
    if (translate_changes_dirmonitor(monitor->internal, monitor->buffer, monitor->length, f_check_dir_callback, L) == 0)
      monitor->length = 0;
    lua_pushboolean(L, 1);
  } else
    lua_pushboolean(L, 0);
  kinc_mutex_unlock(monitor->mutex);
  return 1;
}


static int f_dirmonitor_mode(lua_State* L) {
  int mode = get_mode_dirmonitor();
  if (mode == 1)
    lua_pushstring(L, "single");
  else
    lua_pushstring(L, "multiple");
  return 1;
}


static const luaL_Reg dirmonitor_lib[] = {
  { "new",      f_dirmonitor_new         },
  { "__gc",     f_dirmonitor_gc          },
  { "watch",    f_dirmonitor_watch       },
  { "unwatch",  f_dirmonitor_unwatch     },
  { "check",    f_dirmonitor_check       },
  { "mode",     f_dirmonitor_mode        },
  {NULL, NULL}
};


int luaopen_dirmonitor(lua_State* L) {
  kinc_threads_init();//@TODO(JSN): Revisite the position of this.
  luaL_newmetatable(L, API_TYPE_DIRMONITOR);
  luaL_setfuncs(L, dirmonitor_lib, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}
