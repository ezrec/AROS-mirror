#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "tarray.h"

int a[10] = {1,2,3,4,5,6,7,8,9,10};
Point p[10] = {{0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8},{8,9},{9,10}};
Point* pp[10];

int ma[10];
Point mp[10];
Point* mpp[10];

Array array;
Array* parray = &array;

#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
int lua_dofile (lua_State *L, const char *filename)
{
	int status = luaL_loadfile(L, filename) || lua_pcall(L, 0, LUA_MULTRET, 0);
	if (status && !lua_isnil(L, -1)) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error object is not a string)";
		fprintf(stderr, "%s\n", msg);
		fflush(stderr);
	}
	return status;
}
#endif

int main (void)
{
	int  tolua_tarray_open (lua_State*);
	lua_State* L = lua_open();
	int i;

	for (i=0; i<10; ++i)
	{
		pp[i] = &p[i];

		ma[i] = a[i];
		mp[i] = p[i];
		mpp[i] = pp[i];

		array.a[i] = a[i];
		array.p[i] = p[i];
		array.pp[i] = pp[i];
	}

	luaopen_base(L);
	tolua_tarray_open(L);

	lua_dofile(L,"tarray.lua");

	lua_close(L);
	return 0;
}

