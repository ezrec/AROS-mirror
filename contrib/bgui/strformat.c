/*
 * @(#) $Header$
 *
 * BGUI library
 * strformat.c -- Locale/exec style formatting.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.7  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.6  2003/01/18 19:10:01  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.5  2000/08/08 20:57:26  chodorowski
 * Minor fixes to build on Amiga.
 *
 * Revision 42.4  2000/08/08 14:11:55  chodorowski
 * Fixes to make BGUI compile on Amiga. Most notably this file defined
 * it's own sprintf() which clashed quite severely with the one in clib.
 * Does anyone know why this was done this way? The one in clib uses
 * (I think) it's own routines, while the one in this file uses SprintfA()
 * which in it's turn uses RawDoFmt() from exec.library. Are there any
 * big differences how they handle formatting of strings?
 * Anyway, it's disabled now, and everything seems to work...
 *
 * Revision 42.3  2000/07/03 21:21:00  bergers
 * Replaced stch_l & stcu_d and had to make a few changes in other places because of that.
 *
 * Revision 42.2  2000/05/29 00:40:24  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.1  2000/05/14 23:32:48  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:10:21  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:13  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1998/10/12 01:29:58  mlemos
 * Made the tprintf function divert the debug output to the serial port via
 * kprintf whenever the Tap program port is not available.
 * Made the tprintf code only compile when bgui.library is compiled for
 * debugging.
 *
 * Revision 41.10  1998/02/25 21:13:13  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:51  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

extern struct LocaleBase *LocaleBase;


//makeproto ASM ULONG CompStrlenF(REG(a0) UBYTE *fstring, REG(a1) ULONG *args)
makeproto ASM REGFUNC2(ULONG, CompStrlenF,
        REGPARAM(A0, UBYTE *, fstring),
        REGPARAM(A1, ULONG *, args))
{
   struct Hook    hook = { NULL, NULL, (HOOKFUNC)LHook_Count, NULL, (APTR)0 };
   struct Locale *loc;

   /*
    * Locale available?
    */
   if (LocaleBase && (loc = OpenLocale(NULL)))
   {
      /*
       * Count the formatted length.
       */
      FormatString(loc, fstring, (APTR)args, &hook);

      /*
       * Kill locale.
       */
      CloseLocale(loc);

      return (ULONG)hook.h_Data;
   };
   return StrLenfA(fstring, args);
}
REGFUNC_END

//makeproto ASM VOID DoSPrintF(REG(a0) UBYTE *buffer, REG(a1) UBYTE *fstring, REG(a2) ULONG *args)
makeproto ASM REGFUNC3(VOID, DoSPrintF,
        REGPARAM(A0, UBYTE *, buffer),
        REGPARAM(A1, UBYTE *, fstring),
        REGPARAM(A2, ULONG *, args))
{
   struct Hook    hook = { NULL, NULL, (HOOKFUNC)LHook_Format, NULL, NULL };
   struct Locale *loc;

   /*
    * Locale available?
    */
   if (LocaleBase && (loc = OpenLocale(NULL)))
   {
      /*
       * Init hook structure.
       */
      hook.h_Data = (APTR)buffer;

      /*
       * Format...
       */
      FormatString(loc, fstring, (APTR)args, &hook);

      /*
       * Kill locale.
       */
      CloseLocale(loc);
   }
   else
      SPrintfA(buffer, fstring, args);
}
REGFUNC_END

#ifdef __AROS__
#else
//makeproto void sprintf(char *buffer, const char *format, ...)
//{
//   SPrintfA(buffer, format, (ULONG *)&format + 1);
//}
#endif

extern __stdargs VOID KPutFmt( STRPTR format,  ULONG *values);

makeproto void tprintf(char *format, ...)
{
#ifdef DEBUG_BGUI
   char *buffer;
   struct Message *msg;
   struct MsgPort *tap;
   int len;
   
   Forbid();
   if ((tap = FindPort("Tap")))
   {
      if (buffer = AllocVec(4096, 0))
      {
         SPrintfA(buffer, format, (ULONG *)&format + 1);
      
         len = strlen(buffer) + sizeof(struct Message) + 1;
         if (msg = AllocVec(len, MEMF_CLEAR))
         {
            msg->mn_Length = len;
            strcpy((char *)msg + sizeof(struct Message), buffer);
            PutMsg(tap, msg);
         };
         FreeVec(buffer);
      };
   }
   else
      KPutFmt(format,(ULONG *)&format + 1);
   Permit();
#endif
}
