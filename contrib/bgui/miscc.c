#include "compilerspecific.h"

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

#ifndef _AROS
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

ASM ULONG ScaleWeight(REG(d2) ULONG e, REG(d3) ULONG f, REG(d4) ULONG a)
{
  ULONG r = UMult32(a,e);
  return UDivMod32((f >> 1) + r, f);
}



void MyPutChProc_StrLenfA(REG(d0) BYTE c, REG(a3) ULONG * putChData)
{
  (*putChData)++;
}

ASM ULONG StrLenfA(REG(a0) UBYTE * FormatString, REG(a1) ULONG * DataStream)
{
  ULONG c = 0;
  RawDoFmt(FormatString, DataStream, ((APTR)MyPutChProc_StrLenfA), &c);
  return c;
}



void MyPutChProc_SPrintfA(REG(d0) char c, REG(a3) char * PutChData)
{
  *PutChData++ = c;
}

ASM VOID SPrintfA(REG(a3) UBYTE * buffer, REG(a0) UBYTE * format, REG(a1) ULONG * args)
{
  RawDoFmt(format, args, ((APTR)MyPutChProc_SPrintfA), buffer);
}


ASM VOID LHook_Count(REG(a0) struct Hook * hook, REG(a1) ULONG chr, REG(a2) struct Locale * loc)
{
  hook->h_Data++;
}


ASM VOID LHook_Format(REG(a0) struct Hook * hook, REG(a1) ULONG chr, REG(a2) struct Locale *loc)
{
  char * cptr = (char *)hook->h_Data;
  *cptr++ = (char)chr;
  hook->h_Data = cptr;
}


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

ULONG AsmDoMethodA(REG(a2) Object * obj, REG(a1) Msg message)
{
  DoMethodA(obj, message);
}

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

ULONG AsmDoSuperMethodA( REG(a0) Class * cl, REG(a2) Object * obj, REG(a1) Msg message)
{
  DoSuperMethodA(cl,obj,message);
}

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

ULONG AsmCoerceMethodA( REG(a0) Class * cl, REG(a2) Object * obj, REG(a1) Msg message)
{
  CoerceMethodA(cl, obj, message);
}
