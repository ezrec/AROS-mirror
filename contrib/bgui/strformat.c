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
 * Revision 41.10  1998/02/25 21:13:13  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:51  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

extern struct Library *LocaleBase;

makeproto ASM ULONG CompStrlenF(REG(a0) UBYTE *fstring, REG(a1) ULONG *args)
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

makeproto ASM VOID DoSPrintF(REG(a0) UBYTE *buffer, REG(a1) UBYTE *fstring, REG(a2) ULONG *args)
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

makeproto void sprintf(char *buffer, char *format, ...)
{
   SPrintfA(buffer, format, (ULONG *)&format + 1);
}

makeproto void tprintf(char *format, ...)
{
   char *buffer;
   struct Message *msg;
   struct MsgPort *tap;
   int len;
   
   Forbid();
   tap = FindPort("Tap");
   Permit();
   
   if (tap)
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
   };
}
