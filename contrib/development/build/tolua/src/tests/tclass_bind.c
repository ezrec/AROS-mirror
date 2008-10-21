/*
** Lua binding: tclass
** Generated automatically by tolua++-1.0.92 on 01/06/07 22:58:24.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int tolua_tclass_open (lua_State* tolua_S);

#include "tclass.h"
Test::Tst_A* Test::Tst_A::last;
Test::Tst_B* Test::Tst_B::last;
Test::Tst_C* Test::Tst_C::last;

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_Test__Tst_Outside (lua_State* tolua_S)
{
 Test::Tst_Outside* self = (Test::Tst_Outside*) tolua_tousertype(tolua_S,1,0);
 delete self;
 return 0;
}

static int tolua_collect_Test__Tst_C (lua_State* tolua_S)
{
 Test::Tst_C* self = (Test::Tst_C*) tolua_tousertype(tolua_S,1,0);
 delete self;
 return 0;
}

static int tolua_collect_Test__Tst_B (lua_State* tolua_S)
{
 Test::Tst_B* self = (Test::Tst_B*) tolua_tousertype(tolua_S,1,0);
 delete self;
 return 0;
}

static int tolua_collect_Test__Tst_A__Tst_AA (lua_State* tolua_S)
{
 Test::Tst_A::Tst_AA* self = (Test::Tst_A::Tst_AA*) tolua_tousertype(tolua_S,1,0);
 delete self;
 return 0;
}

static int tolua_collect_Test__Tst_A__Tst_BB (lua_State* tolua_S)
{
 Test::Tst_A::Tst_BB* self = (Test::Tst_A::Tst_BB*) tolua_tousertype(tolua_S,1,0);
 delete self;
 return 0;
}

static int tolua_collect_Test__Tst_E (lua_State* tolua_S)
{
 Test::Tst_E* self = (Test::Tst_E*) tolua_tousertype(tolua_S,1,0);
 delete self;
 return 0;
}

static int tolua_collect_Test__Tst_A (lua_State* tolua_S)
{
 Test::Tst_A* self = (Test::Tst_A*) tolua_tousertype(tolua_S,1,0);
 delete self;
 return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"Test::Tst_Outside");
 tolua_usertype(tolua_S,"Test::Tst_C");
 tolua_usertype(tolua_S,"Test::Tst_B");
 tolua_usertype(tolua_S,"Test::Tst_Dummy");
 tolua_usertype(tolua_S,"Test::Tst_A::Tst_AA");
 tolua_usertype(tolua_S,"Test::Tst_A::Tst_BB");
 tolua_usertype(tolua_S,"Test::Tst_E");
 tolua_usertype(tolua_S,"Test::Tst_A");
}

/* get function: last of class  Test::Tst_A */
#ifndef TOLUA_DISABLE_tolua_get_Test__Tst_A_last_ptr
static int tolua_get_Test__Tst_A_last_ptr(lua_State* tolua_S)
{
 tolua_pushusertype(tolua_S,(void*)Test::Tst_A::last,"Test::Tst_A");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: last of class  Test::Tst_A */
#ifndef TOLUA_DISABLE_tolua_set_Test__Tst_A_last_ptr
static int tolua_set_Test__Tst_A_last_ptr(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!tolua_isusertype(tolua_S,2,"Test::Tst_A",0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  Test::Tst_A::last = ((Test::Tst_A*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Test::Tst_A */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_new00
static int tolua_tclass_Test_A_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_A",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_A* tolua_ret = (Test::Tst_A*)  new Test::Tst_A();
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_A");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Test::Tst_A */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_new00_local
static int tolua_tclass_Test_A_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_A",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_A* tolua_ret = (Test::Tst_A*)  new Test::Tst_A();
 tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"Test::Tst_A");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: a of class  Test::Tst_A */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_a00
static int tolua_tclass_Test_A_a00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_A",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_A* self = (Test::Tst_A*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'a'",NULL);
#endif
 {
  char* tolua_ret = (char*)  self->a();
 tolua_pushstring(tolua_S,(const char*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'a'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Tst_AA */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_AA_new00
static int tolua_tclass_Test_A_AA_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_A::Tst_AA",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_A::Tst_AA* tolua_ret = (Test::Tst_A::Tst_AA*)  new Test::Tst_A::Tst_AA();
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_A::Tst_AA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Tst_AA */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_AA_new00_local
static int tolua_tclass_Test_A_AA_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_A::Tst_AA",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_A::Tst_AA* tolua_ret = (Test::Tst_A::Tst_AA*)  new Test::Tst_A::Tst_AA();
 tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"Test::Tst_A::Tst_AA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  Tst_AA */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_AA_delete00
static int tolua_tclass_Test_A_AA_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_A::Tst_AA",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_A::Tst_AA* self = (Test::Tst_A::Tst_AA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'",NULL);
#endif
 delete self;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: aa of class  Tst_AA */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_AA_aa00
static int tolua_tclass_Test_A_AA_aa00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_A::Tst_AA",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_A::Tst_AA* self = (Test::Tst_A::Tst_AA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'aa'",NULL);
#endif
 {
  char* tolua_ret = (char*)  self->aa();
 tolua_pushstring(tolua_S,(const char*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'aa'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Tst_BB */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_BB_new00
static int tolua_tclass_Test_A_BB_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_A::Tst_BB",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_A::Tst_BB* tolua_ret = (Test::Tst_A::Tst_BB*)  new Test::Tst_A::Tst_BB();
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_A::Tst_BB");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Tst_BB */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_BB_new00_local
static int tolua_tclass_Test_A_BB_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_A::Tst_BB",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_A::Tst_BB* tolua_ret = (Test::Tst_A::Tst_BB*)  new Test::Tst_A::Tst_BB();
 tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"Test::Tst_A::Tst_BB");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  Tst_BB */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_BB_delete00
static int tolua_tclass_Test_A_BB_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_A::Tst_BB",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_A::Tst_BB* self = (Test::Tst_A::Tst_BB*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'",NULL);
#endif
 delete self;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Base of class  Tst_BB */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_A_BB_Base00
static int tolua_tclass_Test_A_BB_Base00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_A::Tst_BB",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_A::Tst_BB* self = (Test::Tst_A::Tst_BB*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Base'",NULL);
#endif
 {
  Test::Tst_A::Tst_AA* tolua_ret = (Test::Tst_A::Tst_AA*)  self->Base();
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_A::Tst_AA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Base'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: number of class  Test::Tst_A */
#ifndef TOLUA_DISABLE_tolua_get_Test__Tst_A_number
static int tolua_get_Test__Tst_A_number(lua_State* tolua_S)
{
  Test::Tst_A* self = (Test::Tst_A*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'number'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->get_number());
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: number of class  Test::Tst_A */
#ifndef TOLUA_DISABLE_tolua_set_Test__Tst_A_number
static int tolua_set_Test__Tst_A_number(lua_State* tolua_S)
{
  Test::Tst_A* self = (Test::Tst_A*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'number'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->set_number(((int)  tolua_tonumber(tolua_S,2,0))
)
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: pete of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_get_Test__Tst_E_pete
static int tolua_get_Test__Tst_E_pete(lua_State* tolua_S)
{
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'pete'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->pete);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: pete of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_set_Test__Tst_E_pete
static int tolua_set_Test__Tst_E_pete(lua_State* tolua_S)
{
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'pete'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->pete = ((Test::Tst_E::Pete) (int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_t<float> of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E_get_t_float_00
static int tolua_tclass_Test_E_get_t_float_00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_t<float>'",NULL);
#endif
 {
  float tolua_ret = (float)  self->get_t<float>();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_t_float_'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_pete of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E_get_pete00
static int tolua_tclass_Test_E_get_pete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
  Test::Tst_E::Pete p = ((Test::Tst_E::Pete) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_pete'",NULL);
#endif
 {
  self->get_pete(p);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_pete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator+ of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E__add00
static int tolua_tclass_Test_E__add00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,2,"const Test::Tst_E",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
  const Test::Tst_E* rvalue = ((const Test::Tst_E*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator+'",NULL);
#endif
 {
  Test::Tst_E& tolua_ret = (Test::Tst_E&)  self->operator+(*rvalue);
 tolua_pushusertype(tolua_S,(void*)&tolua_ret,"Test::Tst_E");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.add'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pars of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E_pars00
static int tolua_tclass_Test_E_pars00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
  int a = ((int)  tolua_tonumber(tolua_S,2,0xa));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pars'",NULL);
#endif
 {
  self->pars(a);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pars'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pars of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E_pars01
static int tolua_tclass_Test_E_pars01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
 {
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
  int a = ((int)  tolua_tonumber(tolua_S,2,0));
  int b = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pars'",NULL);
#endif
 {
  self->pars(a,b);
 }
 }
 return 0;
tolua_lerror:
 return tolua_tclass_Test_E_pars00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_pointer of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E_get_pointer00
static int tolua_tclass_Test_E_get_pointer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isuserdata(tolua_S,2,1,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
  void* a = ((void*)  tolua_touserdata(tolua_S,2,NULL));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_pointer'",NULL);
#endif
 {
  self->get_pointer(a);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_pointer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_ptr of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E_set_ptr00
static int tolua_tclass_Test_E_set_ptr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isuserdata(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
  void* p_ptr = ((void*)  tolua_touserdata(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_ptr'",NULL);
#endif
 {
  self->set_ptr(p_ptr);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_ptr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_ptr of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E_get_ptr00
static int tolua_tclass_Test_E_get_ptr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_ptr'",NULL);
#endif
 {
  void* tolua_ret = (void*)  self->get_ptr();
 tolua_pushuserdata(tolua_S,(void*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_ptr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: a of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_get_Test__Tst_E_a
static int tolua_get_Test__Tst_E_a(lua_State* tolua_S)
{
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'a'",NULL);
#endif
 tolua_pushusertype(tolua_S,(void*)&self->a,"Test::Tst_A");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: a of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_set_Test__Tst_E_a
static int tolua_set_Test__Tst_E_a(lua_State* tolua_S)
{
  Test::Tst_E* self = (Test::Tst_E*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'a'",NULL);
 if (!tolua_isusertype(tolua_S,2,"Test::Tst_A",0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->a = *((Test::Tst_A*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E_new00
static int tolua_tclass_Test_E_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  int tolua_var_1 = ((int)  tolua_tonumber(tolua_S,2,0));
 {
  Test::Tst_E* tolua_ret = (Test::Tst_E*)  new Test::Tst_E(tolua_var_1);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_E");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Test::Tst_E */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_E_new00_local
static int tolua_tclass_Test_E_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_E",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  int tolua_var_1 = ((int)  tolua_tonumber(tolua_S,2,0));
 {
  Test::Tst_E* tolua_ret = (Test::Tst_E*)  new Test::Tst_E(tolua_var_1);
 tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"Test::Tst_E");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: last of class  Test::Tst_B */
#ifndef TOLUA_DISABLE_tolua_get_Test__Tst_B_last_ptr
static int tolua_get_Test__Tst_B_last_ptr(lua_State* tolua_S)
{
 tolua_pushusertype(tolua_S,(void*)Test::Tst_B::last,"Test::Tst_B");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: last of class  Test::Tst_B */
#ifndef TOLUA_DISABLE_tolua_set_Test__Tst_B_last_ptr
static int tolua_set_Test__Tst_B_last_ptr(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!tolua_isusertype(tolua_S,2,"Test::Tst_B",0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  Test::Tst_B::last = ((Test::Tst_B*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Test::Tst_B */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_B_new00
static int tolua_tclass_Test_B_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_B",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_B* tolua_ret = (Test::Tst_B*)  new Test::Tst_B();
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_B");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Test::Tst_B */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_B_new00_local
static int tolua_tclass_Test_B_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_B",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_B* tolua_ret = (Test::Tst_B*)  new Test::Tst_B();
 tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"Test::Tst_B");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: b of class  Test::Tst_B */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_B_b00
static int tolua_tclass_Test_B_b00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_B",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_B* self = (Test::Tst_B*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'b'",NULL);
#endif
 {
  char* tolua_ret = (char*)  self->b();
 tolua_pushstring(tolua_S,(const char*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'b'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: create of class  Test::Tst_B */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_B_create00
static int tolua_tclass_Test_B_create00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_B",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_A* tolua_ret = (Test::Tst_A*)  Test::Tst_B::create();
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_A");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'create'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: create_void of class  Test::Tst_B */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_B_create_void00
static int tolua_tclass_Test_B_create_void00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_B",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  void* tolua_ret = (void*)  Test::Tst_B::create_void();
 tolua_pushuserdata(tolua_S,(void*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'create_void'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: last of class  Test::Tst_C */
#ifndef TOLUA_DISABLE_tolua_get_Test__Tst_C_last_ptr
static int tolua_get_Test__Tst_C_last_ptr(lua_State* tolua_S)
{
 tolua_pushusertype(tolua_S,(void*)Test::Tst_C::last,"Test::Tst_C");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: last of class  Test::Tst_C */
#ifndef TOLUA_DISABLE_tolua_set_Test__Tst_C_last_ptr
static int tolua_set_Test__Tst_C_last_ptr(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!tolua_isusertype(tolua_S,2,"Test::Tst_C",0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  Test::Tst_C::last = ((Test::Tst_C*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Test::Tst_C */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_luaC_new00
static int tolua_tclass_Test_luaC_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_C",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  int n = ((int)  tolua_tonumber(tolua_S,2,0));
 {
  Test::Tst_C* tolua_ret = (Test::Tst_C*)  new Test::Tst_C(n);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_C");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Test::Tst_C */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_luaC_new00_local
static int tolua_tclass_Test_luaC_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_C",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  int n = ((int)  tolua_tonumber(tolua_S,2,0));
 {
  Test::Tst_C* tolua_ret = (Test::Tst_C*)  new Test::Tst_C(n);
 tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"Test::Tst_C");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  Test::Tst_C */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_luaC_delete00
static int tolua_tclass_Test_luaC_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_C",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_C* self = (Test::Tst_C*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'",NULL);
#endif
 delete self;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: c of class  Test::Tst_C */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_luaC_c00
static int tolua_tclass_Test_luaC_c00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_C",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_C* self = (Test::Tst_C*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'c'",NULL);
#endif
 {
  char* tolua_ret = (char*)  self->c();
 tolua_pushstring(tolua_S,(const char*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'c'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: Test::Tst_create_aa */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_create_aa00
static int tolua_tclass_Test_create_aa00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_A::Tst_AA* tolua_ret = (Test::Tst_A::Tst_AA*)  Test::Tst_create_aa();
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_A::Tst_AA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'create_aa'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: Test::Tst_is_aa */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_is_aa00
static int tolua_tclass_Test_is_aa00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_A::Tst_AA",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_A::Tst_AA* obj = ((Test::Tst_A::Tst_AA*)  tolua_tousertype(tolua_S,1,0));
 {
  bool tolua_ret = (bool)  Test::Tst_is_aa(obj);
 tolua_pushboolean(tolua_S,(bool)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_aa'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: outside_func of class  Test::Tst_Outside */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_Outside_outside00
static int tolua_tclass_Test_Outside_outside00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Test::Tst_Outside",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_Outside* self = (Test::Tst_Outside*)  tolua_tousertype(tolua_S,1,0);
  lua_State* ls =  tolua_S;
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'outside_func'",NULL);
#endif
 {
  outside_func(self,ls);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'outside'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: outside_func of class  Test::Tst_Outside */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_Outside_outside_static00
static int tolua_tclass_Test_Outside_outside_static00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_Outside",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,2,"Test::Tst_Outside",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Test::Tst_Outside* instance = ((Test::Tst_Outside*)  tolua_tousertype(tolua_S,2,0));
  lua_State* ls =  tolua_S;
 {
  outside_func(instance,ls);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'outside_static'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Test::Tst_Outside */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_Outside_new00
static int tolua_tclass_Test_Outside_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_Outside",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_Outside* tolua_ret = (Test::Tst_Outside*)  new Test::Tst_Outside();
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Test::Tst_Outside");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Test::Tst_Outside */
#ifndef TOLUA_DISABLE_tolua_tclass_Test_Outside_new00_local
static int tolua_tclass_Test_Outside_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Test::Tst_Outside",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Test::Tst_Outside* tolua_ret = (Test::Tst_Outside*)  new Test::Tst_Outside();
 tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"Test::Tst_Outside");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_tclass_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_module(tolua_S,"Test",0);
 tolua_beginmodule(tolua_S,"Test");
  tolua_cclass(tolua_S,"Dummy","Test::Tst_Dummy","",NULL);
  tolua_beginmodule(tolua_S,"Dummy");
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"A","Test::Tst_A","",tolua_collect_Test__Tst_A);
  #else
  tolua_cclass(tolua_S,"A","Test::Tst_A","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"A");
   tolua_variable(tolua_S,"last",tolua_get_Test__Tst_A_last_ptr,tolua_set_Test__Tst_A_last_ptr);
   tolua_function(tolua_S,"new",tolua_tclass_Test_A_new00);
   tolua_function(tolua_S,"new_local",tolua_tclass_Test_A_new00_local);
   tolua_function(tolua_S,".call",tolua_tclass_Test_A_new00_local);
   tolua_function(tolua_S,"a",tolua_tclass_Test_A_a00);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"AA","Test::Tst_A::Tst_AA","",tolua_collect_Test__Tst_A__Tst_AA);
   #else
   tolua_cclass(tolua_S,"AA","Test::Tst_A::Tst_AA","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"AA");
    tolua_function(tolua_S,"new",tolua_tclass_Test_A_AA_new00);
    tolua_function(tolua_S,"new_local",tolua_tclass_Test_A_AA_new00_local);
    tolua_function(tolua_S,".call",tolua_tclass_Test_A_AA_new00_local);
    tolua_function(tolua_S,"delete",tolua_tclass_Test_A_AA_delete00);
    tolua_function(tolua_S,"aa",tolua_tclass_Test_A_AA_aa00);
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"BB","Test::Tst_A::Tst_BB","Test::Tst_A::Tst_AA",tolua_collect_Test__Tst_A__Tst_BB);
   #else
   tolua_cclass(tolua_S,"BB","Test::Tst_A::Tst_BB","Test::Tst_A::Tst_AA",NULL);
   #endif
   tolua_beginmodule(tolua_S,"BB");
    tolua_function(tolua_S,"new",tolua_tclass_Test_A_BB_new00);
    tolua_function(tolua_S,"new_local",tolua_tclass_Test_A_BB_new00_local);
    tolua_function(tolua_S,".call",tolua_tclass_Test_A_BB_new00_local);
    tolua_function(tolua_S,"delete",tolua_tclass_Test_A_BB_delete00);
    tolua_function(tolua_S,"Base",tolua_tclass_Test_A_BB_Base00);
   tolua_endmodule(tolua_S);
   tolua_variable(tolua_S,"number",tolua_get_Test__Tst_A_number,tolua_set_Test__Tst_A_number);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"E","Test::Tst_E","",tolua_collect_Test__Tst_E);
  #else
  tolua_cclass(tolua_S,"E","Test::Tst_E","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"E");
   tolua_constant(tolua_S,"ONE",Test::Tst_E::ONE);
   tolua_constant(tolua_S,"TWO",Test::Tst_E::TWO);
   tolua_variable(tolua_S,"pete",tolua_get_Test__Tst_E_pete,tolua_set_Test__Tst_E_pete);
   tolua_function(tolua_S,"get_t_float_",tolua_tclass_Test_E_get_t_float_00);
   tolua_function(tolua_S,"get_pete",tolua_tclass_Test_E_get_pete00);
   tolua_function(tolua_S,".add",tolua_tclass_Test_E__add00);
   tolua_function(tolua_S,"pars",tolua_tclass_Test_E_pars00);
   tolua_function(tolua_S,"pars",tolua_tclass_Test_E_pars01);
   tolua_function(tolua_S,"get_pointer",tolua_tclass_Test_E_get_pointer00);
   tolua_function(tolua_S,"set_ptr",tolua_tclass_Test_E_set_ptr00);
   tolua_function(tolua_S,"get_ptr",tolua_tclass_Test_E_get_ptr00);
   tolua_variable(tolua_S,"a",tolua_get_Test__Tst_E_a,tolua_set_Test__Tst_E_a);
   tolua_function(tolua_S,"new",tolua_tclass_Test_E_new00);
   tolua_function(tolua_S,"new_local",tolua_tclass_Test_E_new00_local);
   tolua_function(tolua_S,".call",tolua_tclass_Test_E_new00_local);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"B","Test::Tst_B","Test::Tst_A",tolua_collect_Test__Tst_B);
  #else
  tolua_cclass(tolua_S,"B","Test::Tst_B","Test::Tst_A",NULL);
  #endif
  tolua_beginmodule(tolua_S,"B");
   tolua_variable(tolua_S,"last",tolua_get_Test__Tst_B_last_ptr,tolua_set_Test__Tst_B_last_ptr);
   tolua_function(tolua_S,"new",tolua_tclass_Test_B_new00);
   tolua_function(tolua_S,"new_local",tolua_tclass_Test_B_new00_local);
   tolua_function(tolua_S,".call",tolua_tclass_Test_B_new00_local);
   tolua_function(tolua_S,"b",tolua_tclass_Test_B_b00);
   tolua_function(tolua_S,"create",tolua_tclass_Test_B_create00);
   tolua_function(tolua_S,"create_void",tolua_tclass_Test_B_create_void00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"luaC","Test::Tst_C","Test::Tst_B",tolua_collect_Test__Tst_C);
  #else
  tolua_cclass(tolua_S,"luaC","Test::Tst_C","Test::Tst_B",NULL);
  #endif
  tolua_beginmodule(tolua_S,"luaC");
   tolua_variable(tolua_S,"last",tolua_get_Test__Tst_C_last_ptr,tolua_set_Test__Tst_C_last_ptr);
   tolua_function(tolua_S,"new",tolua_tclass_Test_luaC_new00);
   tolua_function(tolua_S,"new_local",tolua_tclass_Test_luaC_new00_local);
   tolua_function(tolua_S,".call",tolua_tclass_Test_luaC_new00_local);
   tolua_function(tolua_S,"delete",tolua_tclass_Test_luaC_delete00);
   tolua_function(tolua_S,"c",tolua_tclass_Test_luaC_c00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"create_aa",tolua_tclass_Test_create_aa00);
  tolua_function(tolua_S,"is_aa",tolua_tclass_Test_is_aa00);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"Outside","Test::Tst_Outside","",tolua_collect_Test__Tst_Outside);
  #else
  tolua_cclass(tolua_S,"Outside","Test::Tst_Outside","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"Outside");
   tolua_function(tolua_S,"outside",tolua_tclass_Test_Outside_outside00);
   tolua_function(tolua_S,"outside_static",tolua_tclass_Test_Outside_outside_static00);
   tolua_function(tolua_S,"new",tolua_tclass_Test_Outside_new00);
   tolua_function(tolua_S,"new_local",tolua_tclass_Test_Outside_new00_local);
   tolua_function(tolua_S,".call",tolua_tclass_Test_Outside_new00_local);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_tclass (lua_State* tolua_S) {
 return tolua_tclass_open(tolua_S);
};
#endif

