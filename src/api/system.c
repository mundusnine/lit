#include <kinc/system.h>
#include <kinc/log.h>
#include <kinc/window.h>
#include <kinc/input/mouse.h>
#include <kinc/input/keyboard.h>
#include <krink/memory.h>
#include <krink/eventhandler.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "api.h"
#include "utfconv.h"
#include "nfd.h"
#ifdef _WIN32
  #include <windows.h>
#else
#include <limits.h>
#include <stdlib.h>
#endif
#ifdef __linux__
#include <sys/vfs.h>
#endif

bool in_foreground = true;

static kr_evt_event_t events[64] = {0};
static uint8_t num_events = 0;
static uint8_t curr_idx = 0;
void event_handler(kr_evt_event_t event){
  events[num_events++] = event;
}

int poll_event(kr_evt_event_t* e){
  if(curr_idx >= num_events){
    curr_idx = num_events = 0;
    return 0;
  }
  e->data = events[curr_idx].data;
  e->event = events[curr_idx].event;
  curr_idx++;
  return 1;
}

static const char* button_name(int button) {
  switch (button) {
    case 1  : return "left";
    case 2  : return "middle";
    case 3  : return "right";
    default : return "?";
  }
}

//SDL ref: https://github.com/libsdl-org/SDL/blob/SDL2/src/events/SDL_keyboard.c#L348
static char* key_name(char *dst, int sym) {
char* out = "unknown";
switch(sym){
case KINC_KEY_UNKNOWN :
  break;
case KINC_KEY_CANCEL:
  out = "cancel";
  break;
case KINC_KEY_HELP:
  out = "help";
  break;
case KINC_KEY_BACKSPACE:
  out = "backspace";
  break;
case KINC_KEY_TAB:
  out = "tab";
  break;
case KINC_KEY_CLEAR:
case KINC_KEY_RETURN:
  out = "return";
  break;
case KINC_KEY_SHIFT:
  out = "left shift";
  break;
case KINC_KEY_CONTROL:
  out = "left ctrl";
  break;
case KINC_KEY_ALT:
  out = "left alt";
  break;
case KINC_KEY_PAUSE:
  out = "pause";
  break;
case KINC_KEY_CAPS_LOCK:
  out = "capslock";
  break;
case KINC_KEY_KANA:
case KINC_KEY_EISU:
case KINC_KEY_JUNJA:
case KINC_KEY_FINAL:
case KINC_KEY_HANJA:
case KINC_KEY_ESCAPE:
  out = "escape";
case KINC_KEY_CONVERT:
case KINC_KEY_NON_CONVERT:
case KINC_KEY_ACCEPT:
case KINC_KEY_MODE_CHANGE:
case KINC_KEY_SPACE:
  out = "space";
  break;
case KINC_KEY_PAGE_UP:
  out = "pageup";
  break;
case KINC_KEY_PAGE_DOWN:
  out = "pagedown";
  break;
case KINC_KEY_END:
  out = "end";
  break;
case KINC_KEY_HOME:
  out = "home";
  break;
case KINC_KEY_LEFT:
  out = "left";
  break;
case KINC_KEY_UP:
  out = "up";
  break;
case KINC_KEY_RIGHT:
  out = "right";
  break;
case KINC_KEY_DOWN:
  out = "down";
  break;
case KINC_KEY_SELECT:
  out = "select";
  break;
case KINC_KEY_PRINT:
case KINC_KEY_EXECUTE:
  out = "execute";
  break;
case KINC_KEY_PRINT_SCREEN:
  out = "printscreen";
  break;
case KINC_KEY_INSERT:
  out = "insert";
  break;
case KINC_KEY_DELETE:
  out = "delete";
  break;
case KINC_KEY_0:
  out = "0";
  break;
case KINC_KEY_1:
  out = "1";
  break;
case KINC_KEY_2:
  out = "2";
  break;
case KINC_KEY_3:
  out = "3";
  break;
case KINC_KEY_4:
  out = "4";
  break;
case KINC_KEY_5:
  out = "5";
  break;
case KINC_KEY_6:
  out = "6";
  break;
case KINC_KEY_7:
  out = "7";
  break;
case KINC_KEY_8:
  out = "8";
  break;
case KINC_KEY_9:
  out = "9";
  break;
case KINC_KEY_COLON:
  out = ":";
  break;
case KINC_KEY_SEMICOLON:
  out = ";";
  break;
case KINC_KEY_LESS_THAN:
  out = "<";
  break;
case KINC_KEY_EQUALS:
  out = "=";
  break;
case KINC_KEY_GREATER_THAN:
  out = ">";
  break;
case KINC_KEY_QUESTIONMARK:
  out = "?";
  break;
case KINC_KEY_AT:
  out = "keypad @";
  break;
case KINC_KEY_A:
  out = "a";
  break;
case KINC_KEY_B:
  out = "b";
  break;
case KINC_KEY_C:
  out = "c";
  break;
case KINC_KEY_D:
  out = "d";
  break;
case KINC_KEY_E:
  out = "e";
  break;
case KINC_KEY_F:
  out = "f";
  break;
case KINC_KEY_G:
  out = "g";
  break;
case KINC_KEY_H:
  out = "h";
  break;
case KINC_KEY_I:
  out = "i";
  break;
case KINC_KEY_J:
  out = "j";
  break;
case KINC_KEY_K:
  out = "k";
  break;
case KINC_KEY_L:
  out = "l";
  break;
case KINC_KEY_M:
  out = "m";
  break;
case KINC_KEY_N:
  out = "n";
  break;
case KINC_KEY_O:
  out = "o";
  break;
case KINC_KEY_P:
  out = "p";
  break;
case KINC_KEY_Q:
  out = "q";
  break;
case KINC_KEY_R:
  out = "r";
  break;
case KINC_KEY_S:
  out = "s";
  break;
case KINC_KEY_T:
  out = "t";
  break;
case KINC_KEY_U:
  out = "u";
  break;
case KINC_KEY_V:
  out = "v";
  break;
case KINC_KEY_W:
  out = "w";
  break;
case KINC_KEY_X:
  out = "x";
  break;
case KINC_KEY_Y:
  out = "y";
  break;
case KINC_KEY_Z:
  out = "z";
  break;
case KINC_KEY_WIN:
case KINC_KEY_CONTEXT_MENU:
  out = "menu";
  break;
case KINC_KEY_SLEEP:
case KINC_KEY_NUMPAD_0:
  out = "keypad 0";
  break;
case KINC_KEY_NUMPAD_1:
  out = "keypad 1";
  break;
case KINC_KEY_NUMPAD_2:
  out = "keypad 2";
  break;
case KINC_KEY_NUMPAD_3:
  out = "keypad 3";
  break;
case KINC_KEY_NUMPAD_4:
  out = "keypad 4";
  break;
case KINC_KEY_NUMPAD_5:
  out = "keypad 5";
  break;
case KINC_KEY_NUMPAD_6:
  out = "keypad 6";
  break;
case KINC_KEY_NUMPAD_7:
  out = "keypad 7";
  break;
case KINC_KEY_NUMPAD_8:
  out = "keypad 8";
  break;
case KINC_KEY_NUMPAD_9:
  out = "keypad 9";
  break;
case KINC_KEY_MULTIPLY:
  out = "keypad *";
  break;
case KINC_KEY_ADD:
  out = "keypad +";
  break;
case KINC_KEY_SEPARATOR:
case KINC_KEY_SUBTRACT:
  out = "keypad -";
  break;
case KINC_KEY_DECIMAL:
  out = "keypad .";
  break;
case KINC_KEY_DIVIDE:
  out = "/";
  break;
case KINC_KEY_F1:
  out = "f1";
  break;
case KINC_KEY_F2:
  out = "f2";
  break;
case KINC_KEY_F3:
  out = "f3";
  break;
case KINC_KEY_F4:
  out = "f4";
  break;
case KINC_KEY_F5:
  out = "f5";
  break;
case KINC_KEY_F6:
  out = "f6";
  break;
case KINC_KEY_F7:
  out = "f7";
  break;
case KINC_KEY_F8:
  out = "f8";
  break;
case KINC_KEY_F9:
  out = "f9";
  break;
case KINC_KEY_F10:
  out = "f10";
  break;
case KINC_KEY_F11:
  out = "f11";
  break;
case KINC_KEY_F12:
  out = "f12";
  break;
case KINC_KEY_F13:
  out = "f13";
  break;
case KINC_KEY_F14:
  out = "f14";
  break;
case KINC_KEY_F15:
  out = "f15";
  break;
case KINC_KEY_F16:
  out = "f16";
  break;
case KINC_KEY_F17:
  out = "f17";
  break;
case KINC_KEY_F18:
  out = "f8";
  break;
case KINC_KEY_F19:
  out = "f19";
  break;
case KINC_KEY_F20:
  out = "f20";
  break;
case KINC_KEY_F21:
  out = "f21";
  break;
case KINC_KEY_F22:
  out = "f22";
  break;
case KINC_KEY_F23:
  out = "f23";
  break;
case KINC_KEY_F24:
  out = "f24";
  break;
case KINC_KEY_NUM_LOCK:
  out = "numlock";
  break;
case KINC_KEY_SCROLL_LOCK:
  out = "scrolllock";
  break;
case KINC_KEY_WIN_OEM_FJ_JISHO:
case KINC_KEY_WIN_OEM_FJ_MASSHOU:
case KINC_KEY_WIN_OEM_FJ_TOUROKU:
case KINC_KEY_WIN_OEM_FJ_LOYA:
case KINC_KEY_WIN_OEM_FJ_ROYA:
case KINC_KEY_CIRCUMFLEX:
case KINC_KEY_EXCLAMATION:
  out = "keypad !";
  break;
case KINC_KEY_DOUBLE_QUOTE:
case KINC_KEY_HASH:
  out = "keypad #";
  break;
case KINC_KEY_DOLLAR:
  out = "keypad $";
  break;
case KINC_KEY_PERCENT:
  out = "keypad %";
  break;
case KINC_KEY_AMPERSAND:
  out = "keypad &";
  break;
case KINC_KEY_UNDERSCORE:
case KINC_KEY_OPEN_PAREN:
  out = "keypad (";
  break;
case KINC_KEY_CLOSE_PAREN:
  out = "keypad )";
  break;
case KINC_KEY_ASTERISK:
  out = "*";
  break;
case KINC_KEY_PLUS:
  out = "keypad +";
  break;
case KINC_KEY_PIPE:
case KINC_KEY_HYPHEN_MINUS:
case KINC_KEY_OPEN_CURLY_BRACKET:
  out = "keypad {";
  break;
case KINC_KEY_CLOSE_CURLY_BRACKET:
  out = "keypad }";
  break;
case KINC_KEY_TILDE:
  out = "~";
  break;
case KINC_KEY_VOLUME_MUTE:
  out = "mute";
  break;
case KINC_KEY_VOLUME_DOWN:
  out = "volumedown";
  break;
case KINC_KEY_VOLUME_UP:
  out = "volumeup";
  break;
case KINC_KEY_COMMA:
  out = ",";
  break;
case KINC_KEY_PERIOD:
  out = ".";
  break;
case KINC_KEY_SLASH:
  out = "/";
  break;
case KINC_KEY_BACK_QUOTE:
  out = "`";
  break;
case KINC_KEY_OPEN_BRACKET:
  out = "[";
  break;
case KINC_KEY_BACK_SLASH:
  out = "\\";
  break;
case KINC_KEY_CLOSE_BRACKET:
  out = "]";
  break;
case KINC_KEY_QUOTE:
  out = "'";
  break;
case KINC_KEY_META:
case KINC_KEY_ALT_GR:
  out = "right alt";
  break;
case KINC_KEY_WIN_ICO_HELP:
case KINC_KEY_WIN_ICO_00:
case KINC_KEY_WIN_ICO_CLEAR:
case KINC_KEY_WIN_OEM_RESET:
case KINC_KEY_WIN_OEM_JUMP:
case KINC_KEY_WIN_OEM_PA1:
case KINC_KEY_WIN_OEM_PA2:
case KINC_KEY_WIN_OEM_PA3:
case KINC_KEY_WIN_OEM_WSCTRL:
case KINC_KEY_WIN_OEM_CUSEL:
case KINC_KEY_WIN_OEM_ATTN:
case KINC_KEY_WIN_OEM_FINISH:
case KINC_KEY_WIN_OEM_COPY:
case KINC_KEY_WIN_OEM_AUTO:
case KINC_KEY_WIN_OEM_ENLW:
case KINC_KEY_WIN_OEM_BACK_TAB:
case KINC_KEY_ATTN:
case KINC_KEY_CRSEL:
case KINC_KEY_EXSEL:
case KINC_KEY_EREOF:
case KINC_KEY_PLAY:
case KINC_KEY_ZOOM:
case KINC_KEY_PA1:
case KINC_KEY_WIN_OEM_CLEAR:
}
  strcpy(dst,out);
  char *p = dst;
  while (*p) {
    *p = tolower(*p);
    p++;
  }
  return dst;
}

	// KR_EVT_KEY_DOWN,
	// KR_EVT_KEY_UP,
	// KR_EVT_KEY_PRESS,
	// KR_EVT_MOUSE_MOVE,
	// KR_EVT_MOUSE_PRESS,
	// KR_EVT_MOUSE_RELEASE,
	// KR_EVT_MOUSE_SCROLL,
	// KR_EVT_FINGER_MOVE,
	// KR_EVT_FINGER_START,
	// KR_EVT_FINGER_END,
	// KR_EVT_PRIMARY_MOVE,
	// KR_EVT_PRIMARY_START,
	// KR_EVT_PRIMARY_END,
	// KR_EVT_FOREGROUND,
	// KR_EVT_BACKGROUND,
	// KR_EVT_PAUSE,
	// KR_EVT_RESUME,
	// KR_EVT_SHUTDOWN,
	// KR_EVT_WINDOW_SIZE_CHANGE
static int f_poll_event(lua_State *L) {
  char buf[16];
  int mx, my, wx, wy;
  kr_evt_event_t e;

top:
  if ( !poll_event(&e) ) {
    return 0;
  }

  switch (e.event) {
    case KR_EVT_SHUTDOWN:
      lua_pushstring(L, "quit");
      return 1;

    case KR_EVT_WINDOW_SIZE_CHANGE:
      lua_pushstring(L, "resized");
      lua_pushnumber(L, e.data.window.width);
      lua_pushnumber(L, e.data.window.height);
      return 3;
      // else if (e.window.event == SDL_WINDOWEVENT_EXPOSED) {
      //   rencache_invalidate();
      //   lua_pushstring(L, "exposed");
      //   return 1;
      // }
      // /* on some systems, when alt-tabbing to the window SDL will queue up
      // ** several KEYDOWN events for the `tab` key; we flush all keydown
      // ** events on focus so these are discarded */
      // if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
      //   SDL_FlushEvent(SDL_KEYDOWN);
      // }
      // goto top;

    case KR_EVT_DROP_FILE:
      int mx,my;
      kinc_mouse_get_position(0,&mx,&my);
      lua_pushstring(L, "filedropped");
      lua_pushstring(L, e.data.drop.filename);
      lua_pushnumber(L, mx);
      lua_pushnumber(L, my);
      return 4;

    case KR_EVT_BACKGROUND:
      in_foreground = false;
      goto top;
    case KR_EVT_FOREGROUND:
      in_foreground = true;
      goto top;
    case KR_EVT_KEY_DOWN:
      lua_pushstring(L, "keypressed");
      lua_pushstring(L, key_name(buf,e.data.key.keycode));
      return 2;

    case KR_EVT_KEY_UP:
      lua_pushstring(L, "keyreleased");
      lua_pushstring(L, key_name(buf,e.data.key.keycode));
      return 2;

    case KR_EVT_KEY_PRESS:
      if(e.data.key_press.character == '\r') goto top;
      lua_pushstring(L, "textinput");
      buf[0] = e.data.key_press.character;
      buf[1] =0;
      lua_pushstring(L,buf);
      return 2;

    case KR_EVT_MOUSE_PRESS:
      lua_pushstring(L, "mousepressed");
      lua_pushstring(L, button_name(e.data.mouse_button.button));
      lua_pushnumber(L, e.data.mouse_button.x);
      lua_pushnumber(L, e.data.mouse_button.y);
      lua_pushnumber(L, 1);//@TODO(JSN): support double clicks
      return 5;

    case KR_EVT_MOUSE_RELEASE:
      lua_pushstring(L, "mousereleased");
      lua_pushstring(L, button_name(e.data.mouse_button.button));
      lua_pushnumber(L, e.data.mouse_button.x);
      lua_pushnumber(L, e.data.mouse_button.y);
      return 4;

    case KR_EVT_MOUSE_MOVE:
      lua_pushstring(L, "mousemoved");
      lua_pushnumber(L, e.data.mouse_move.x);
      lua_pushnumber(L, e.data.mouse_move.y);
      lua_pushnumber(L, e.data.mouse_move.dx);
      lua_pushnumber(L, e.data.mouse_move.dy);
      return 5;

    case KR_EVT_MOUSE_SCROLL:
      lua_pushstring(L, "mousewheel");
      lua_pushnumber(L, -e.data.mouse_scroll.delta);
      return 2;

    // Eventually support finger...
    // case SDL_FINGERDOWN:
    //   SDL_GetWindowSize(window_renderer.window, &w, &h);

    //   lua_pushstring(L, "touchpressed");
    //   lua_pushinteger(L, (lua_Integer)(e.tfinger.x * w));
    //   lua_pushinteger(L, (lua_Integer)(e.tfinger.y * h));
    //   lua_pushinteger(L, e.tfinger.fingerId);
    //   return 4;

    // case SDL_FINGERUP:
    //   SDL_GetWindowSize(window_renderer.window, &w, &h);

    //   lua_pushstring(L, "touchreleased");
    //   lua_pushinteger(L, (lua_Integer)(e.tfinger.x * w));
    //   lua_pushinteger(L, (lua_Integer)(e.tfinger.y * h));
    //   lua_pushinteger(L, e.tfinger.fingerId);
    //   return 4;

    // case SDL_FINGERMOTION:
    //   SDL_PumpEvents();
    //   while (SDL_PeepEvents(&event_plus, 1, SDL_GETEVENT, SDL_FINGERMOTION, SDL_FINGERMOTION) > 0) {
    //     e.tfinger.x = event_plus.tfinger.x;
    //     e.tfinger.y = event_plus.tfinger.y;
    //     e.tfinger.dx += event_plus.tfinger.dx;
    //     e.tfinger.dy += event_plus.tfinger.dy;
    //   }
    //   SDL_GetWindowSize(window_renderer.window, &w, &h);

    //   lua_pushstring(L, "touchmoved");
    //   lua_pushinteger(L, (lua_Integer)(e.tfinger.x * w));
    //   lua_pushinteger(L, (lua_Integer)(e.tfinger.y * h));
    //   lua_pushinteger(L, (lua_Integer)(e.tfinger.dx * w));
    //   lua_pushinteger(L, (lua_Integer)(e.tfinger.dy * h));
    //   lua_pushinteger(L, e.tfinger.fingerId);
    //   return 6;
    default:
      goto top;
  }

  return 0;
}

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

static int sleep_ms(int milliseconds){ // cross-platform sleep function
#ifdef WIN32
    return Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    return nanosleep(&ts, NULL);
#else
    if (milliseconds >= 1000)
      sleep(milliseconds / 1000);
    return usleep((milliseconds % 1000) * 1000);
#endif
}

static int f_wait_event(lua_State *L) {
  double n = luaL_checknumber(L, 1);
  lua_pushboolean(L, sleep_ms(n * 1000));
  return 1;
}

static const char *cursor_opts[] = {
  "arrow",
  "ibeam",
  "sizeh",
  "sizev",
  "hand",
  NULL
};

static const int cursor_enums[] = {
  0,//SDL_SYSTEM_CURSOR_ARROW, @TODO verify
  1,//SDL_SYSTEM_CURSOR_IBEAM,
  7,//SDL_SYSTEM_CURSOR_SIZEWE,
  8,//SDL_SYSTEM_CURSOR_SIZENS,
  11//SDL_SYSTEM_CURSOR_HAND
};

static int f_set_cursor(lua_State *L) {
  int opt = luaL_checkoption(L, 1, "arrow", cursor_opts);
  int n = cursor_enums[opt];
  kinc_mouse_set_cursor(n);
  return 0;
}


static int f_set_window_title(lua_State *L) {
  const char *title = luaL_checkstring(L, 1);
  kinc_window_set_title(0, title);
  return 0;
}


static const char *window_opts[] = { "normal", "maximized", "fullscreen", 0 };
enum { WIN_NORMAL, WIN_MAXIMIZED, WIN_FULLSCREEN };

static int f_set_window_mode(lua_State *L) {
  int n = luaL_checkoption(L, 1, "normal", window_opts);
  
  if (n == WIN_NORMAL) { kinc_window_change_mode(0,KINC_WINDOW_MODE_WINDOW); }
  if (n == WIN_MAXIMIZED || n == WIN_FULLSCREEN) {
    #ifdef _WIN32
    kinc_window_change_mode(0,KINC_WINDOW_MODE_EXCLUSIVE_FULLSCREEN);
    #else
    kinc_window_change_mode(0,KINC_WINDOW_MODE_FULLSCREEN);
    #endif

  }
  return 0;
}

static int f_get_window_mode(lua_State *L) {
  kinc_window_mode_t mode = kinc_window_get_mode(0);
  if (mode == KINC_WINDOW_MODE_FULLSCREEN) {
    lua_pushstring(L, "fullscreen");
  } else if (mode == KINC_WINDOW_MODE_WINDOW) {
    lua_pushstring(L, "minimized");
  } else if (mode == KINC_WINDOW_MODE_EXCLUSIVE_FULLSCREEN) {
    lua_pushstring(L, "maximized");
  } else {
    lua_pushstring(L, "normal");
  }
  return 1;
}

static int f_set_window_bordered(lua_State *L) {
  int bordered = lua_toboolean(L, 1);
  if(bordered){
    kinc_window_change_features(0,KINC_WINDOW_FEATURE_MINIMIZABLE | KINC_WINDOW_FEATURE_RESIZEABLE | KINC_WINDOW_FEATURE_MAXIMIZABLE);
  }
  else{
    kinc_window_change_features(0,KINC_WINDOW_FEATURE_BORDERLESS | KINC_WINDOW_FEATURE_MINIMIZABLE | KINC_WINDOW_FEATURE_RESIZEABLE | KINC_WINDOW_FEATURE_MAXIMIZABLE);
  }
  return 0;
}

static int f_get_window_size(lua_State *L) {
  int x, y, w, h;
  x = kinc_window_x(0);
  y = kinc_window_y(0);
  w = kinc_window_width(0);
  h = kinc_window_height(0);
  lua_pushinteger(L, w);
  lua_pushinteger(L, h);
  lua_pushinteger(L, x);
  lua_pushinteger(L, y);
  return 4;
}


static int f_set_window_size(lua_State *L) {
  double w = luaL_checknumber(L, 1);
  double h = luaL_checknumber(L, 2);
  double x = luaL_checknumber(L, 3);
  double y = luaL_checknumber(L, 4);
  // SDL_SetWindowSize(window_renderer.window, w, h);
  // SDL_SetWindowPosition(window_renderer.window, x, y);
  // ren_resize_window(&window_renderer);
  kinc_window_resize(0,w,h);
  kinc_window_move(0,x,y);
  return 0;
}


static int f_window_has_focus(lua_State *L) {
  lua_pushboolean(L, in_foreground);
  return 1;
}


static int f_show_confirm_dialog(lua_State *L) {
  const char *title = luaL_checkstring(L, 1);
  const char *msg = luaL_checkstring(L, 2);

#if _WIN32
  int id = MessageBox(0, msg, title, MB_YESNO | MB_ICONWARNING);
  lua_pushboolean(L, id == IDYES);

#else
  // SDL_MessageBoxButtonData buttons[] = {
  //   { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
  //   { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" },
  // };
  // SDL_MessageBoxData data = {
  //   .title = title,
  //   .message = msg,
  //   .numbuttons = 2,
  //   .buttons = buttons,
  // };
  // int buttonid;
  // SDL_ShowMessageBox(&data, &buttonid);
  // lua_pushboolean(L, buttonid == 1);
#endif
  return 1;
}


static int f_chdir(lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  int err = chdir(path);
  if (err) { luaL_error(L, "chdir() failed"); }
  return 0;
}

// removes an empty directory
static int f_rmdir(lua_State *L) {
  const char *path = luaL_checkstring(L, 1);

#ifdef _WIN32
  LPWSTR wpath = utfconv_utf8towc(path);
  int deleted = RemoveDirectoryW(wpath);
  free(wpath);
  if (deleted > 0) {
    lua_pushboolean(L, 1);
  } else {
    lua_pushboolean(L, 0);
    push_win32_error(L, GetLastError());
    return 2;
  }
#else
  int deleted = remove(path);
  if(deleted < 0) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, strerror(errno));

    return 2;
  } else {
    lua_pushboolean(L, 1);
  }
#endif

  return 1;
}

static int f_mkdir(lua_State *L) {
  const char *path = luaL_checkstring(L, 1);

#ifdef _WIN32
  LPWSTR wpath = utfconv_utf8towc(path);
  if (wpath == NULL) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, UTFCONV_ERROR_INVALID_CONVERSION);
    return 2;
  }

  int err = _wmkdir(wpath);
  free(wpath);
#else
  int err = mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
#endif
  if (err < 0) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, strerror(errno));
    return 2;
  }

  lua_pushboolean(L, 1);
  return 1;
}


static int f_list_dir(lua_State *L) {
  const char *path = luaL_checkstring(L, 1);

  DIR *dir = opendir(path);
  if (!dir) {
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));
    return 2;
  }

  lua_newtable(L);
  int i = 1;
  struct dirent *entry;
  while ( (entry = readdir(dir)) ) {
    if (strcmp(entry->d_name, "." ) == 0) { continue; }
    if (strcmp(entry->d_name, "..") == 0) { continue; }
    lua_pushstring(L, entry->d_name);
    lua_rawseti(L, -2, i);
    i++;
  }

  closedir(dir);
  return 1;
}


#ifdef _WIN32
  #include <windows.h>
  #define PATH_MAX 260
  #define realpath(x, y) _fullpath(y, x, MAX_PATH)
#endif
static int f_absolute_path(lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  char res[PATH_MAX] = {0};
  realpath(path, res);
  if (!res[0]) { return 0; }
  lua_pushstring(L, res);
  return 1;
}


static int f_get_file_info(lua_State *L) {
  const char *path = luaL_checkstring(L, 1);

  struct stat s;
  int err = stat(path, &s);
  if (err < 0) {
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));
    return 2;
  }

  lua_newtable(L);
  lua_pushnumber(L, s.st_mtime);
  lua_setfield(L, -2, "modified");

  lua_pushnumber(L, s.st_size);
  lua_setfield(L, -2, "size");

  if (S_ISREG(s.st_mode)) {
    lua_pushstring(L, "file");
  } else if (S_ISDIR(s.st_mode)) {
    lua_pushstring(L, "dir");
  } else {
    lua_pushnil(L);
  }
  lua_setfield(L, -2, "type");

  return 1;
}

#if __linux__
// https://man7.org/linux/man-pages/man2/statfs.2.html

struct f_type_names {
  uint32_t magic;
  const char *name;
};

static struct f_type_names fs_names[] = {
  { 0xef53,     "ext2/ext3" },
  { 0x6969,     "nfs"       },
  { 0x65735546, "fuse"      },
  { 0x517b,     "smb"       },
  { 0xfe534d42, "smb2"      },
  { 0x52654973, "reiserfs"  },
  { 0x01021994, "tmpfs"     },
  { 0x858458f6, "ramfs"     },
  { 0x5346544e, "ntfs"      },
  { 0x0,        NULL        },
};

#endif

static int f_get_fs_type(lua_State *L) {
  #if __linux__
    const char *path = luaL_checkstring(L, 1);
    struct statfs buf;
    int status = statfs(path, &buf);
    if (status != 0) {
      return luaL_error(L, "error calling statfs on %s", path);
    }
    for (int i = 0; fs_names[i].magic; i++) {
      if (fs_names[i].magic == buf.f_type) {
        lua_pushstring(L, fs_names[i].name);
        return 1;
      }
    }
  #endif
  lua_pushstring(L, "unknown");
  return 1;
}

static lua_State* temp_L = NULL;
static void receive(char* text){
  lua_pushstring(temp_L, text);
  lua_pcall(temp_L,1,0,0);
}
static int f_get_clipboard(lua_State *L) {
  lua_rawget(L,1);
  temp_L = L;
  // kinc_copy_from_clipboard(receive);
  // char *text = "";
  // if (!text) { return 0; }
  // lua_pushstring(L, text);
  // SDL_free(text);
  return 0;
}


static int f_set_clipboard(lua_State *L) {
  const char *text = luaL_checkstring(L,1);
  kinc_copy_to_clipboard(text);
  return 0;
}

static int f_get_process_id(lua_State *L) {
#ifdef _WIN32
  lua_pushinteger(L, GetCurrentProcessId());
#else
  lua_pushinteger(L, getpid());
#endif
  return 1;
}

static int f_get_time(lua_State *L) {
  double n = kinc_time();
  lua_pushnumber(L, n);
  return 1;
}


static int f_sleep(lua_State *L) {
  double n = luaL_checknumber(L, 1);
  sleep_ms(n * 1000);
  return 0;
}


static int f_exec(lua_State *L) {
  size_t len;
  const char *cmd = luaL_checklstring(L, 1, &len);
  char *buf = kr_malloc(len + 32);
  if (!buf) { luaL_error(L, "buffer allocation failed"); }
#if _WIN32
  sprintf(buf, "cmd /c \"%s\"", cmd);
  WinExec(buf, SW_HIDE);
#else
  sprintf(buf, "%s &", cmd);
  int res = system(buf);
  (void) res;
#endif
  kr_free(buf);
  return 0;
}


static int f_fuzzy_match(lua_State *L) {
  size_t strLen, ptnLen;
  const char *str = luaL_checklstring(L, 1, &strLen);
  const char *ptn = luaL_checklstring(L, 2, &ptnLen);
  // If true match things *backwards*. This allows for better matching on filenames than the above
  // function. For example, in the lite project, opening "renderer" has lib/font_render/build.sh
  // as the first result, rather than src/renderer.c. Clearly that's wrong.
  bool files = lua_gettop(L) > 2 && lua_isboolean(L,3) && lua_toboolean(L, 3);
  int score = 0, run = 0, increment = files ? -1 : 1;
  const char* strTarget = files ? str + strLen - 1 : str;
  const char* ptnTarget = files ? ptn + ptnLen - 1 : ptn;
  while (strTarget >= str && ptnTarget >= ptn && *strTarget && *ptnTarget) {
    while (strTarget >= str && *strTarget == ' ') { strTarget += increment; }
    while (ptnTarget >= ptn && *ptnTarget == ' ') { ptnTarget += increment; }
    if (tolower(*strTarget) == tolower(*ptnTarget)) {
      score += run * 10 - (*strTarget != *ptnTarget);
      run++;
      ptnTarget += increment;
    } else {
      score -= 10;
      run = 0;
    }
    strTarget += increment;
  }
  if (ptnTarget >= ptn && *ptnTarget) { return 0; }
  lua_pushinteger(L, score - (int)strLen * 10);
  return 1;
}

#ifdef _WIN32
#define PATHSEP '\\'
#else
#define PATHSEP '/'
#endif

/* Special purpose filepath compare function. Corresponds to the
   order used in the TreeView view of the project's files. Returns true iff
   path1 < path2 in the TreeView order. */
static int f_path_compare(lua_State *L) {
  const char *path1 = luaL_checkstring(L, 1);
  const char *type1_s = luaL_checkstring(L, 2);
  const char *path2 = luaL_checkstring(L, 3);
  const char *type2_s = luaL_checkstring(L, 4);
  const int len1 = strlen(path1), len2 = strlen(path2);
  int type1 = strcmp(type1_s, "dir") != 0;
  int type2 = strcmp(type2_s, "dir") != 0;
  /* Find the index of the common part of the path. */
  int offset = 0, i;
  for (i = 0; i < len1 && i < len2; i++) {
    if (path1[i] != path2[i]) break;
    if (path1[i] == PATHSEP) {
      offset = i + 1;
    }
  }
  /* If a path separator is present in the name after the common part we consider
     the entry like a directory. */
  if (strchr(path1 + offset, PATHSEP)) {
    type1 = 0;
  }
  if (strchr(path2 + offset, PATHSEP)) {
    type2 = 0;
  }
  /* If types are different "dir" types comes before "file" types. */
  if (type1 != type2) {
    lua_pushboolean(L, type1 < type2);
    return 1;
  }
  /* If types are the same compare the files' path alphabetically. */
  int cfr = 0;
  int len_min = (len1 < len2 ? len1 : len2);
  for (int j = offset; j <= len_min; j++) {
    if (path1[j] == path2[j]) continue;
    if (path1[j] == 0 || path2[j] == 0) {
      cfr = (path1[j] == 0);
    } else if (path1[j] == PATHSEP || path2[j] == PATHSEP) {
      /* For comparison we treat PATHSEP as if it was the string terminator. */
      cfr = (path1[j] == PATHSEP);
    } else {
      cfr = (path1[j] < path2[j]);
    }
    break;
  }
  lua_pushboolean(L, cfr);
  return 1;
}


static const luaL_Reg lib[] = {
  { "poll_event",          f_poll_event          },
  { "wait_event",          f_wait_event          },
  { "set_cursor",          f_set_cursor          },
  { "set_window_title",    f_set_window_title    },
  { "set_window_mode",     f_set_window_mode     },
  { "get_window_mode",     f_get_window_mode     },
  { "set_window_bordered", f_set_window_bordered },
  { "get_window_size",     f_get_window_size     },
  { "set_window_size",     f_set_window_size     },
  { "window_has_focus",    f_window_has_focus    },
  { "show_confirm_dialog", f_show_confirm_dialog },
  { "chdir",               f_chdir               },
  { "rmdir",               f_rmdir               },
  { "mkdir",               f_mkdir               },
  { "list_dir",            f_list_dir            },
  { "absolute_path",       f_absolute_path       },
  { "get_file_info",       f_get_file_info       },
  { "get_clipboard",       f_get_clipboard       },
  { "set_clipboard",       f_set_clipboard       },
  { "get_process_id",      f_get_process_id      },
  { "get_time",            f_get_time            },
  { "sleep",               f_sleep               },
  { "exec",                f_exec                },
  { "fuzzy_match",         f_fuzzy_match         },
  { "path_compare",        f_path_compare        },
  { "get_fs_type",         f_get_fs_type         },
  { NULL, NULL }
};


int luaopen_system(lua_State *L) {
  luaL_newlib(L, lib);
  return 1;
}
