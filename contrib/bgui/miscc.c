#include "include/classdefs.h"
#include <utility/hooks.h>
#include <intuition/classes.h>
#include <intuition/cghooks.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <libraries/locale.h>

#ifndef BGUI_COMPILERSPECIFIC_H
#include <bgui/bgui_compilerspecific.h>
#endif

#ifndef __AROS__
BOOL DisplayAGuideInfo(struct NewAmigaGuide *nag, struct TagItem * tags)
{
  BOOL success = FALSE;
  struct Library * AmigaGuideBase;
  AmigaGuideBase = OpenLibrary("amigaguide.library",0);
  
  if (NULL != AmigaGuideBase)
  {
    AMIGAGUIDECONTEXT handle = OpenAmigaGuideA(nag, tags);
    if (0 != handle)
    {
      success = TRUE;
      CloseAmigaGuide(handle);
    }
    CloseLibrary(AmigaGuideBase);
  }
  else
    return FALSE;
}
#endif

//ASM ULONG ScaleWeight(REG(d2) ULONG e, REG(d3) ULONG f, REG(d4) ULONG a)
ASM REGFUNC3(ULONG, ScaleWeight,
        REGPARAM(D2, ULONG, e),
        REGPARAM(D3, ULONG, f),
        REGPARAM(D4, ULONG, a))
{
  ULONG r = UMult32(a,e);
  return UDivMod32((f >> 1) + r, f);
}
REGFUNC_END



//void MyPutChProc_StrLenfA(REG(d0) BYTE c, REG(a3) ULONG * putChData)
REGFUNC2(void, MyPutChProc_StrLenfA,
        REGPARAM(D0, BYTE, c),
        REGPARAM(A3, ULONG *, putChData))
{
  (*putChData)++;
}
REGFUNC_END

//ASM ULONG StrLenfA(REG(a0) UBYTE * FormatString, REG(a1) ULONG * DataStream)
ASM REGFUNC2(ULONG, StrLenfA, 
        REGPARAM(A0, UBYTE *, FormatString),
        REGPARAM(A1, ULONG *, DataStream))
{
  ULONG c = 0;
  RawDoFmt(FormatString, DataStream, ((APTR)MyPutChProc_StrLenfA), &c);
  return c;
}
REGFUNC_END



//void MyPutChProc_SPrintfA(REG(d0) char c, REG(a3) char **PutChData)
REGFUNC2(void, MyPutChProc_SPrintfA,
        REGPARAM(D0, char, c),
        REGPARAM(A3, char **, PutChData))
{
  **PutChData = c;
  *PutChData++;
}
REGFUNC_END

//ASM VOID SPrintfA(REG(a3) UBYTE * buffer, REG(a0) UBYTE * format, REG(a1) ULONG * args)
ASM REGFUNC3(VOID, SPrintfA,
        REGPARAM(A3, UBYTE *, buffer),
        REGPARAM(A0, UBYTE *, format),
        REGPARAM(A1, ULONG *, args))
{
  RawDoFmt(format, args, ((APTR)MyPutChProc_SPrintfA), &buffer);
}
REGFUNC_END


//ASM VOID LHook_Count(REG(a0) struct Hook * hook, REG(a1) ULONG chr, REG(a2) struct Locale * loc)
ASM REGFUNC3(VOID, LHook_Count,
        REGPARAM(A0, struct Hook *, hook),
        REGPARAM(A2, struct Locale *, loc),
        REGPARAM(A1, ULONG, chr))
{
  hook->h_Data++;
}
REGFUNC_END


//ASM VOID LHook_Format(REG(a0) struct Hook * hook, REG(a1) ULONG chr, REG(a2) struct Locale *loc)
ASM REGFUNC3(VOID, LHook_Format,
        REGPARAM(A0, struct Hook *, hook),
        REGPARAM(A2, struct Locale *, loc),
        REGPARAM(A1, ULONG, chr))
{
  char * cptr = (char *)hook->h_Data;
  *cptr++ = (char)chr;
  hook->h_Data = cptr;
}
REGFUNC_END


//ASM struct RastPort *BGUI_ObtainGIRPort(REG(a0) struct GadgetInfo * gi)
ASM REGFUNC1(struct RastPort *, BGUI_ObtainGIRPort,
        REGPARAM(A0, struct GadgetInfo *, gi))
{
  struct RastPort * rp;
  BYTE * userdata = NULL;
  
  if (NULL != gi)
  {
    if (NULL != gi->gi_Window)
      userdata = gi->gi_Window->UserData;

    if (NULL != gi->gi_Layer)
    {
      /* Does this make sense?? */
      LockLayerRom(gi->gi_Layer); // this function returns void!!
      UnlockLayerRom(gi->gi_Layer);
    }
  }
  rp = ObtainGIRPort(gi);
  if (NULL != rp)
    rp->RP_User = (APTR)userdata;
    
  return rp;
}
REGFUNC_END


ULONG AsmDoMethod(Object * obj, ULONG MethodID, ...)
{
     AROS_SLOWSTACKMETHODS_PRE(MethodID)
#if 0
     ASSERT_VALID_PTR(obj);
     ASSERT_VALID_PTR(OCLASS(obj));
#endif
     if (!obj)
       retval = 0L;
     else
       retval = CallHookPkt ((struct Hook *)OCLASS(obj)
                      , obj
                      , AROS_SLOWSTACKMETHODS_ARG(MethodID)
                );
     AROS_SLOWSTACKMETHODS_POST
}

//ULONG AsmDoMethodA(REG(a2) Object * obj, REG(a1) Msg message)
REGFUNC2(ULONG, AsmDoMethodA,
        REGPARAM(A2, Object *, obj),
        REGPARAM(A1, Msg, message))
{
  DoMethodA(obj, message);
}
REGFUNC_END

ULONG AsmDoSuperMethod( Class * cl, Object * obj, ULONG MethodID, ...)
{
   AROS_SLOWSTACKMETHODS_PRE(MethodID)
   if ((!obj) || (!cl))
       retval = 0L;
   else
       retval = CallHookPkt ((struct Hook *)cl->cl_Super
                       , obj
                       , AROS_SLOWSTACKMETHODS_ARG(MethodID)
                );
   AROS_SLOWSTACKMETHODS_POST
}

//ULONG AsmDoSuperMethodA( REG(a0) Class * cl, REG(a2) Object * obj, REG(a1) Msg message)
REGFUNC3(ULONG, AsmDoSuperMethodA,
        REGPARAM(A0, Class *, cl),
        REGPARAM(A2, Object *, obj),
        REGPARAM(A1, Msg, message))
{
  DoSuperMethodA(cl,obj,message);
}
REGFUNC_END

ULONG AsmCoerceMethod( Class * cl, Object * obj, ULONG MethodID, ...)
{
    AROS_SLOWSTACKMETHODS_PRE(MethodID)
    if ((!obj) || (!cl))
      retval = 0L;
    else
      retval = CallHookPkt ((struct Hook *)cl
                      , obj
                      , AROS_SLOWSTACKMETHODS_ARG(MethodID)
               );
    AROS_SLOWSTACKMETHODS_POST
}

//ULONG AsmCoerceMethodA( REG(a0) Class * cl, REG(a2) Object * obj, REG(a1) Msg message)
REGFUNC3(ULONG, AsmCoerceMethodA,
        REGPARAM(A0, Class *, cl),
        REGPARAM(A2, Object *, obj),
        REGPARAM(A1, Msg, message))
{
  CoerceMethodA(cl, obj, message);
}
REGFUNC_END
