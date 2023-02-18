#include "api.h"
#include "renderer.h"
// #include "rencache.h"

#define FONT_FALLBACK_MAX 10

static int font_get_options(
  lua_State *L,
  ERenFontAntialiasing *antialiasing,
  ERenFontHinting *hinting,
  int *style
) {
  if (lua_gettop(L) > 2 && lua_istable(L, 3)) {
    lua_getfield(L, 3, "antialiasing");
    if (lua_isstring(L, -1)) {
      const char *antialiasing_str = lua_tostring(L, -1);
      if (antialiasing_str) {
        if (strcmp(antialiasing_str, "none") == 0) {
          *antialiasing = FONT_ANTIALIASING_NONE;
        } else if (strcmp(antialiasing_str, "grayscale") == 0) {
          *antialiasing = FONT_ANTIALIASING_GRAYSCALE;
        } else if (strcmp(antialiasing_str, "subpixel") == 0) {
          *antialiasing = FONT_ANTIALIASING_SUBPIXEL;
        } else {
          return luaL_error(
            L,
            "error in font options, unknown antialiasing option: \"%s\"",
            antialiasing_str
          );
        }
      }
    }
    lua_getfield(L, 3, "hinting");
    if (lua_isstring(L, -1)) {
      const char *hinting_str = lua_tostring(L, -1);
      if (hinting_str) {
        if (strcmp(hinting_str, "slight") == 0) {
          *hinting = FONT_HINTING_SLIGHT;
        } else if (strcmp(hinting_str, "none") == 0) {
          *hinting = FONT_HINTING_NONE;
        } else if (strcmp(hinting_str, "full") == 0) {
          *hinting = FONT_HINTING_FULL;
        } else {
          return luaL_error(
            L,
            "error in font options, unknown hinting option: \"%s\"",
            hinting
          );
        }
      }
    }
    int style_local = 0;
    lua_getfield(L, 3, "italic");
    if (lua_toboolean(L, -1))
      style_local |= FONT_STYLE_ITALIC;
    lua_getfield(L, 3, "bold");
    if (lua_toboolean(L, -1))
      style_local |= FONT_STYLE_BOLD;
    lua_getfield(L, 3, "underline");
    if (lua_toboolean(L, -1))
      style_local |= FONT_STYLE_UNDERLINE;
    lua_getfield(L, 3, "smoothing");
    if (lua_toboolean(L, -1))
      style_local |= FONT_STYLE_SMOOTH;
    lua_getfield(L, 3, "strikethrough");
    if (lua_toboolean(L, -1))
      style_local |= FONT_STYLE_STRIKETHROUGH;

    lua_pop(L, 5);

    if (style_local != 0)
      *style = style_local;
  }

  return 0;
}

static int f_load(lua_State *L) {
  const char *filename  = luaL_checkstring(L, 1);
  float size = luaL_checknumber(L, 2);
  int style = 0;
  ERenFontHinting hinting = FONT_HINTING_SLIGHT;
  ERenFontAntialiasing antialiasing = FONT_ANTIALIASING_SUBPIXEL;

  int ret_code = font_get_options(L, &antialiasing, &hinting, &style);
  if (ret_code > 0)
    return ret_code;
  
  RenFont **self = lua_newuserdata(L, sizeof(*self));
  *self = ren_load_font(filename, size);
  if (!*self) { luaL_error(L, "failed to load font"); }
  luaL_setmetatable(L, API_TYPE_FONT);
  return 1;
}

static int font_retrieve(lua_State* L, RenFont** fonts, int idx) {
  memset(fonts, 0, sizeof(RenFont*)*FONT_FALLBACK_MAX);
  if (lua_type(L, idx) != LUA_TTABLE) {
    fonts[0] = *(RenFont**)luaL_checkudata(L, idx, API_TYPE_FONT);
    return 0;
  }
  int len = luaL_len(L, idx); len = len > FONT_FALLBACK_MAX ? FONT_FALLBACK_MAX : len;
  for (int i = 0; i < len; i++) {
    lua_rawgeti(L, idx, i+1);
    fonts[i] = *(RenFont**) luaL_checkudata(L, -1, API_TYPE_FONT);
    lua_pop(L, 1);
  }
  return 1;
}

static int f_copy(lua_State *L) {
  RenFont* fonts[FONT_FALLBACK_MAX];
  int table = font_retrieve(L, fonts, 1);
  float size = luaL_checknumber(L, 2);
  int style = -1;
  ERenFontHinting hinting = -1;
  ERenFontAntialiasing antialiasing = -1;

  int ret_code = font_get_options(L, &antialiasing, &hinting, &style);
  if (ret_code > 0)
    return ret_code;

  if (table) {
    lua_newtable(L);
    luaL_setmetatable(L, API_TYPE_FONT);
  }
  for (int i = 0; i < FONT_FALLBACK_MAX && fonts[i]; ++i) {
    RenFont** font = lua_newuserdata(L, sizeof(RenFont*));
    *font = ren_load_font(fonts[i]->path, size);//@TODO(JSN): Support antialising and stuff.
    if (!*font)
      return luaL_error(L, "failed to copy font");
    luaL_setmetatable(L, API_TYPE_FONT);
    if (table)
      lua_rawseti(L, -2, i+1);
  }
  return 1;
}


static int f_set_tab_width(lua_State *L) {
  RenFont **self = luaL_checkudata(L, 1, API_TYPE_FONT);
  int n = luaL_checknumber(L, 2);
  ren_set_font_tab_width(*self, n);
  return 0;
}


static int f_gc(lua_State *L) {
  RenFont **self = luaL_checkudata(L, 1, API_TYPE_FONT);
  if (*self) { ren_free_font(*self); }
  return 0;
}


static int f_get_width(lua_State *L) {
  RenFont **self = luaL_checkudata(L, 1, API_TYPE_FONT);
  const char *text = luaL_checkstring(L, 2);
  lua_pushnumber(L, ren_get_font_width(*self, text) );
  return 1;
}


static int f_get_height(lua_State *L) {
  RenFont **self = luaL_checkudata(L, 1, API_TYPE_FONT);
  lua_pushnumber(L, ren_get_font_height(*self) );
  return 1;
}


static const luaL_Reg lib[] = {
  { "__gc",          f_gc            },
  { "load",          f_load          },
  { "copy",          f_copy          },
  { "set_tab_width", f_set_tab_width },
  { "get_width",     f_get_width     },
  { "get_height",    f_get_height    },
  { NULL, NULL }
};

int luaopen_renderer_font(lua_State *L) {
  luaL_newmetatable(L, API_TYPE_FONT);
  luaL_setfuncs(L, lib, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}
