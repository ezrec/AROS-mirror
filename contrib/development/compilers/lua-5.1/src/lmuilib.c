/*
 * $Id$
 */

#include <stdio.h>

#define MUIMASTER_YES_INLINE_STDARG
#define DEBUG 1

#include <aros/debug.h>
#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/alib.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lmuilib.h"

//==============================================================================

#if 0
static void* luaL_toudata (lua_State* L, int ud, const char* tname)
{
   void* p = lua_touserdata(L, ud);
   if (p)
   {
      lua_getfield(L, LUA_REGISTRYINDEX, tname);
      if (!lua_getmetatable(L, ud))
      {
         lua_pop(L, 1);
         return NULL;
      }
      else if (!lua_rawequal(L, -1, -2))
         p = NULL;

      lua_pop(L, 2);
   }

   return p;
}
#endif

//==============================================================================

#define STRARRAY "strarray"

typedef STRPTR * Strarray;

//------------------------------------------------------------------------------

static Strarray toStrarray (lua_State *L, int index)
{
  Strarray *ps = (Strarray *)lua_touserdata(L, index);
  if (ps == NULL) luaL_typerror(L, index, STRARRAY);
  return *ps;
}

//------------------------------------------------------------------------------

static Strarray checkStrarray (lua_State *L, int index)
{
  Strarray *ps, sa;
  luaL_checktype(L, index, LUA_TUSERDATA);
  ps = (Strarray*)luaL_checkudata(L, index, STRARRAY);
  if (ps == NULL) luaL_typerror(L, index, STRARRAY);
  sa = *ps;
  if (!sa)
    luaL_error(L, "checkStrarray: Strarray is null");
  return sa;
}

//------------------------------------------------------------------------------

static Strarray *pushStrarray (lua_State *L, Strarray sa)
{
  Strarray *ps = (Strarray *)lua_newuserdata(L, sizeof(Strarray));
  *ps = sa;
  luaL_getmetatable(L, STRARRAY);
  lua_setmetatable(L, -2);
  return ps;
}

//------------------------------------------------------------------------------

static int Strarray_new (lua_State *L)
{
  int i;
  int cnt = lua_gettop(L);
  if (cnt < 1)
    luaL_error(L, "Strarray_new: need at least 1 string");
  // One additional element for the closing NULL
  STRPTR *str = AllocVec(sizeof(STRPTR) * (cnt + 1), MEMF_ANY | MEMF_CLEAR);
  if (str == NULL)
    luaL_error(L, "Strarray_new: can't allocate RAM");
  for (i=0; i < cnt; i++)
  {
    str[i] = StrDup(luaL_checkstring(L, i + 1));
    if (str[i] == NULL)
      luaL_error(L, "Strarray_new: can't allocate RAM");
  }
  pushStrarray(L, str);
  return 1;
}

//------------------------------------------------------------------------------

static const luaL_reg Strarray_methods[] = {
  {"new", Strarray_new},
  {0,0}
};

//------------------------------------------------------------------------------

static int Strarray_gc (lua_State *L)
{
  STRPTR * str = toStrarray(L, 1);
  int i = 0;
  if (str)
  {
    while (str[i])
    {
      D(bug("String %s freed\n", str[i]));
      FreeVec(str[i]);
      i++;
    }
    FreeVec(str);
  }
  D(bug("Strarray gargabe collector\n"));
  return 0;
}

//------------------------------------------------------------------------------

static const luaL_reg Strarray_meta[] = {
  {"__gc", Strarray_gc},
  {0, 0}
};

//==============================================================================

#define MUI "mui"

typedef APTR Muiobj;

//------------------------------------------------------------------------------

static Muiobj toMui (lua_State *L, int index)
{
  Muiobj *pm = (Muiobj *)lua_touserdata(L, index);
  if (pm == NULL) luaL_typerror(L, index, MUI);
  return *pm;
}

//------------------------------------------------------------------------------

static Muiobj checkMui (lua_State *L, int index)
{
  Muiobj *pm, mo;
  luaL_checktype(L, index, LUA_TUSERDATA);
  pm = (Muiobj*)luaL_checkudata(L, index, MUI);
  if (pm == NULL) luaL_typerror(L, index, MUI);
  mo = *pm;
  if (!mo)
    luaL_error(L, "checkMui: Muiobject is null");
  return mo;
}

//------------------------------------------------------------------------------

static Muiobj *pushMui (lua_State *L, Muiobj mo)
{
  Muiobj *pm = (Muiobj *)lua_newuserdata(L, sizeof(Muiobj));
  *pm = mo;
  luaL_getmetatable(L, MUI);
  lua_setmetatable(L, -2);
  return pm;
}

//------------------------------------------------------------------------------

static struct TagItem *createTagArray(lua_State *L, int start)
{
  int i;
  if (start < 1)
    luaL_error(L, "start must be at least '1'");
  int pairs = (lua_gettop(L) - start + 1) / 2;
  D(bug("top %d pairs %d\n", lua_gettop(L), pairs));
  if (pairs < 1)
    return 0;
  int param = start;
  struct TagItem *ti = AllocVec(sizeof(struct TagItem) * (pairs + 1), MEMF_ANY | MEMF_CLEAR);
  if (ti == NULL)
    luaL_error(L, "can't allocate RAM for TagItem");
  for (i=0; i < pairs; i++)
  {
    ti[i].ti_Tag = luaL_checkint(L, param);
    param++;
    D(bug("type %d\n", lua_type(L, param)));
    switch (lua_type(L, param))
    {
      case LUA_TNIL:
        luaL_error(L, "parameter must not be 'nil'");
        break;
      case LUA_TBOOLEAN:
        ti[i].ti_Data = lua_toboolean(L, param) ? TRUE : FALSE;
        break;
      case LUA_TUSERDATA:
        ti[i].ti_Data = *(IPTR *)lua_touserdata(L, param);
        break;
      case LUA_TNUMBER:
        ti[i].ti_Data = lua_tointeger(L, param);
        break;
      case LUA_TSTRING:
        ti[i].ti_Data = (IPTR)lua_tostring(L, param);
        break;
      default:
        luaL_error(L, "wrong type");
        break;
    }
    param++;
    D(bug("Tag %x Data %x\n", ti[i].ti_Tag, ti[i].ti_Data));
  }
  ti[pairs].ti_Tag = TAG_DONE;
  return ti;
}

//------------------------------------------------------------------------------

static int Muiobj_new (lua_State *L)
{
  struct TagItem * ti = createTagArray(L, 2);
  CONST_STRPTR class = luaL_checkstring(L, 1);
  APTR obj = MUI_NewObjectA(class, ti);
  FreeVec(ti);
  pushMui(L, obj);
  return 1;
}

//------------------------------------------------------------------------------

static int Muiobj_make (lua_State *L)
{
  int i;
  int class = luaL_checkinteger(L, 1);
  if (class == 0)
    luaL_error(L, "Muiobj_make: class must not be 0");

  int args = lua_gettop(L) - 1;
  if (args < 1)
    luaL_error(L, "Muiobj_make: need at least a method identifier");
  IPTR *ip = AllocVec(sizeof(IPTR) * args, MEMF_ANY | MEMF_CLEAR);
  if (ip == NULL)
    luaL_error(L, "Muiobj_make: can't allocate RAM");
  for (i=0; i< args; i++)
  {
    switch (lua_type(L, i + 2))
    {
      case LUA_TNIL:
        luaL_error(L, "Muiobj_make: argument must not be 'nil'");
        break;
      case LUA_TBOOLEAN:
        ip[i] = lua_toboolean(L, i + 2) ? TRUE : FALSE;
        break;
      case LUA_TUSERDATA:
        ip[i] = *(IPTR *)lua_touserdata(L, i + 2);
        break;
      case LUA_TNUMBER:
        ip[i] = lua_tointeger(L, i + 2);
        break;
      case LUA_TSTRING:
        ip[i] = (IPTR)lua_tostring(L, i + 2);
        break;
      default:
        luaL_error(L, "Muiobj_make: wrong type");
        break;
    }
  }

  APTR obj = MUI_MakeObjectA(class, ip);
  FreeVec(ip);
  pushMui(L, obj);
  return 1;
}

//------------------------------------------------------------------------------

static int Muiobj_domethod (lua_State *L)
{
  int i;
  Muiobj mo = checkMui(L, 1);
  int args = lua_gettop(L) - 1;
  if (args < 1)
    luaL_error(L, "Muiobj_domethod: need at least one method identifier");
  IPTR *ip = AllocVec(sizeof(IPTR) * args, MEMF_ANY | MEMF_CLEAR);
  if (ip == NULL)
    luaL_error(L, "Muiobj_domethod: can't allocate RAM");
  for (i=0; i< args; i++)
  {
    switch (lua_type(L, i + 2))
    {
      case LUA_TNIL:
        luaL_error(L, "Muiobj_domethod: argument must not be 'nil'");
        break;
      case LUA_TBOOLEAN:
        ip[i] = lua_toboolean(L, i + 2) ? TRUE : FALSE;
        break;
      case LUA_TUSERDATA:
        ip[i] = *(IPTR *)lua_touserdata(L, i + 2);
        break;
      case LUA_TNUMBER:
        ip[i] = lua_tointeger(L, i + 2);
        break;
      case LUA_TSTRING:
        ip[i] = (IPTR)lua_tostring(L, i + 2);
        break;
      default:
        luaL_error(L, "Muiobj_domethod: wrong type");
        break;
    }
  }

  DoMethodA(mo, (Msg)ip);
  FreeVec(ip);
  return 0;
}

//------------------------------------------------------------------------------

static int Muiobj_set (lua_State *L)
{
  Muiobj mo = checkMui(L, 1);
  IPTR tag = luaL_checkint(L, 2);
  IPTR value = 0;
  switch (lua_type(L, 3))
  {
      case LUA_TNIL:
        luaL_error(L, "Muiobj_set: parameter must not be 'nil'");
        break;
      case LUA_TBOOLEAN:
        value = lua_toboolean(L, 3) ? TRUE : FALSE;
        break;
      case LUA_TUSERDATA:
        value = (IPTR)checkMui(L, 3);
        break;
      case LUA_TNUMBER:
        value = lua_tointeger(L, 3);
        break;
      case LUA_TSTRING:
        value = (IPTR)lua_tostring(L, 3);
        break;
      default:
        luaL_error(L, "Muiobj_set: wrong type");
        break;
  }
  /*ULONG result = */SetAttrs(mo, tag, value, TAG_DONE);
//  if (result != 1)
//    luaL_error(L, "Muiobj_set: SetAttrs failed");
  return 0;
}

//------------------------------------------------------------------------------

static IPTR Muiobj_get_intern(lua_State *L)
{
  Muiobj mo = checkMui(L, 1);
  ULONG attrID = luaL_checkinteger(L, 2);
  IPTR storage = 0;
  GetAttr(attrID, mo, &storage);
  return storage;
}

//------------------------------------------------------------------------------

static int Muiobj_get_integer(lua_State *L)
{
  lua_pushinteger(L, Muiobj_get_intern(L));
  return 1;
}

//------------------------------------------------------------------------------

static int Muiobj_get_string(lua_State *L)
{
  lua_pushstring(L, (char *)Muiobj_get_intern(L));
  return 1;
}

//------------------------------------------------------------------------------

static int Muiobj_request(lua_State *L)
{
  int i;
  int cnt = lua_gettop(L);
  APTR app = NULL;
  if (lua_isuserdata(L, 1));
      app = toMui(L, 1);
  APTR win = NULL;
  if (lua_isuserdata(L, 2))
    win = *(APTR *)lua_touserdata(L, 2);
  LONGBITS flags = lua_tointeger(L, 3);
  const char *title = lua_tostring(L, 4);
  const char *gadgets = luaL_checkstring(L, 5);
  const char *format = luaL_checkstring(L, 6);
  IPTR *params = NULL;
  if (cnt > 6)
  {
    params = AllocVec(sizeof(IPTR) * (cnt - 6), MEMF_ANY | MEMF_CLEAR);
    if ( ! params)
      luaL_error(L, "Muiobj_request: can't allocate RAM");
    for (i = 0; i < cnt - 6; i++)
    {
      switch (lua_type(L, i + 7))
      {
        case LUA_TNUMBER:
          params[i] = luaL_checknumber(L, i + 7);
          break;
        case LUA_TSTRING:
          params[i] = (IPTR)luaL_checkstring(L, i + 7);
          break;
        default:
          luaL_error(L, "Muiobj_request: wrong type for parameter");
          break;
      }
    }
  }
  lua_pushinteger(L, MUI_RequestA(app, win, flags, title, gadgets, format, params ));
  FreeVec(params);
  return 1;
}

//------------------------------------------------------------------------------

static int Muiobj_dispose (lua_State *L)
{
  Muiobj mo = toMui(L, 1);
  MUI_DisposeObject(mo);
  *((Muiobj *)lua_touserdata(L, 1)) = NULL;
  return 0;
}

//------------------------------------------------------------------------------

static const luaL_reg Mui_methods[] = {
  {"new",      Muiobj_new},
  {"make",     Muiobj_make},
  {"domethod", Muiobj_domethod},
  {"set",      Muiobj_set},
  {"getint",   Muiobj_get_integer},
  {"getstr",   Muiobj_get_string},
  {"dispose",  Muiobj_dispose},
  {"request",  Muiobj_request},
  {0,0}
};

//------------------------------------------------------------------------------

static int Mui_gc (lua_State *L)
{
  D(bug("Mui object garbage collector\n"));
  return 0;
}

//------------------------------------------------------------------------------

static int Mui_tostring (lua_State *L)
{
  lua_pushfstring(L, "Object: %p", lua_touserdata(L, 1));
  return 1;
}

//------------------------------------------------------------------------------

static const luaL_reg Mui_meta[] = {
  {"__gc",       Mui_gc},
  {"__tostring", Mui_tostring},
  {0, 0}
};

//------------------------------------------------------------------------------

int luaopen_muilib (lua_State *L)
{
  luaL_openlib(L, MUI, Mui_methods, 0);
  luaL_newmetatable(L, MUI);
  luaL_openlib(L, 0, Mui_meta, 0);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);
  lua_rawset(L, -3);
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  luaL_openlib(L, STRARRAY, Strarray_methods, 0);
  luaL_newmetatable(L, STRARRAY);
  luaL_openlib(L, 0, Strarray_meta, 0);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);
  lua_rawset(L, -3);
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  return 1;
}

