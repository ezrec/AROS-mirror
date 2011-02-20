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

BOOL DisplayAGuideInfo(struct NewAmigaGuide *nag, Tag tag, ...)
{
  AROS_SLOWSTACKTAGS_PRE_AS(tag, BOOL)
  struct Library * AmigaGuideBase;
  AmigaGuideBase = OpenLibrary("amigaguide.library",0);
 
  retval = FALSE;
  if (NULL != AmigaGuideBase)
  {
    AMIGAGUIDECONTEXT handle = OpenAmigaGuideA(nag, AROS_SLOWSTACKTAGS_ARG(tag));
    if (0 != handle)
    {
      retval = TRUE;
      CloseAmigaGuide(handle);
    }
    CloseLibrary(AmigaGuideBase);
  }
  AROS_SLOWSTACKTAGS_POST
}

ASM ULONG ScaleWeight(REG(d2) ULONG e, REG(d3) ULONG f, REG(d4) ULONG a)
{
  ULONG r = UMult32(a,e);
  return UDivMod32((f >> 1) + r, f);
}

//void MyPutChProc_StrLenfA(REG(d0) BYTE c, REG(a3) ULONG * putChData)
REGFUNC2(void, MyPutChProc_StrLenfA,
        REGPARAM(D0, BYTE, c),
        REGPARAM(A3, ULONG *, putChData))
{
  (*putChData)++;
}
REGFUNC_END

ASM ULONG StrLenfA(REG(a0) UBYTE * FormatString, REG(a1) IPTR * DataStream)
{
  ULONG c = 0;
  RawDoFmt(FormatString, DataStream, ((APTR)MyPutChProc_StrLenfA), &c);
  return c;
}

//void MyPutChProc_SPrintfA(REG(d0) char c, REG(a3) char **PutChData)
REGFUNC2(void, MyPutChProc_SPrintfA,
        REGPARAM(D0, char, c),
        REGPARAM(A3, char **, PutChData))
{
  **PutChData = c;
  (*PutChData)++;
}
REGFUNC_END

ASM VOID SPrintfA(REG(a3) UBYTE * buffer, REG(a0) UBYTE * format, REG(a1) IPTR * args)
{
  RawDoFmt(format, args, ((APTR)MyPutChProc_SPrintfA), &buffer);
}


//ASM VOID LHook_Count(REG(a0) struct Hook * hook, REG(a1) ULONG chr, REG(a2) struct Locale * loc)
ASM REGFUNC3(VOID, LHook_Count,
        REGPARAM(A0, struct Hook *, hook),
        REGPARAM(A1, ULONG, chr),
        REGPARAM(A2, struct Locale *, loc))
{
  hook->h_Data++;
}
REGFUNC_END


//ASM VOID LHook_Format(REG(a0) struct Hook * hook, REG(a1) ULONG chr, REG(a2) struct Locale *loc)
ASM REGFUNC3(VOID, LHook_Format,
        REGPARAM(A0, struct Hook *, hook),
        REGPARAM(A1, ULONG, chr),
        REGPARAM(A2, struct Locale *, loc))
{
  char * cptr = (char *)hook->h_Data;
  *cptr++ = (char)chr;
  hook->h_Data = cptr;
}
REGFUNC_END


ASM struct RastPort *BGUI_ObtainGIRPort(REG(a0) struct GadgetInfo * gi)
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

IPTR AsmDoMethod(Object * obj, ULONG MethodID, ...)
{
     AROS_SLOWSTACKMETHODS_PRE(MethodID)
     if (!obj)
       retval = 0L;
     else
       retval = CallHookPkt ((struct Hook *)OCLASS(obj)
                      , obj
                      , AROS_SLOWSTACKMETHODS_ARG(MethodID)
                );
     AROS_SLOWSTACKMETHODS_POST
}

IPTR AsmDoMethodA(REG(a2) Object * obj, REG(a1) Msg message)
{
  return DoMethodA(obj, message);
}

IPTR AsmDoSuperMethod( Class * cl, Object * obj, ULONG MethodID, ...)
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

IPTR AsmDoSuperMethodA( REG(a0) Class * cl, REG(a2) Object * obj, REG(a1) Msg message)
{
  return DoSuperMethodA(cl,obj,message);
}

IPTR AsmCoerceMethod( Class * cl, Object * obj, ULONG MethodID, ...)
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

IPTR AsmCoerceMethodA( REG(a0) Class * cl, REG(a2) Object * obj, REG(a1) Msg message)
{
  return CoerceMethodA(cl, obj, message);
}
