/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define __NOLIBBASE__
#define NO_INLINE_STDARG
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <exec/memory.h>
#include <exec/semaphores.h>
#include <workbench/startup.h>
#include <Generic/Types.h>
#include <proto/utility.h>
#include <LibC/LibC.h>

#ifndef MEMF_SHARED
#define MEMF_SHARED 0
#endif

   void*                   __InternalMemPool;
   SignalSemaphore         __InternalSemaphore;
   BPTR                    stdin;
   BPTR                    stdout;
   WBStartup              *__WBStartup = 0;

   struct Library         *__InternalDOSBase;
   struct Library         *__InternalIntuitionBase;
   struct Library         *__InternalUtilityBase;
   struct Library         *SysBase;

#ifdef __AMIGAOS4__
   struct DOSIFace        *__InternalDOSIFace;
   struct IntuitionIFace  *__InternalIntuitionIFace;
   struct UtilityIFace    *__InternalUtilityIFace;
   struct ExecIFace       *IExec;
#endif

   static char dosname[] = "dos.library";
   static char intname[] = "intuition.library";
   static char utilname[] = "utility.library";


int __setup()
{
   struct Library* DOSBase = 0;
#ifdef __AMIGAOS4__
   struct DOSIFace* IDOS = 0;
#endif

   // 1. if we have no exec, QUIT!
   if (0 == SysBase)
      return false;
   
   // 2. Initialize semaphore
   InitSemaphore(&__InternalSemaphore);
   
   // 2.a Open Libraries
   __InternalDOSBase          = OpenLibrary(dosname, 37);
   __InternalIntuitionBase    = OpenLibrary(intname, 37);
   __InternalUtilityBase      = OpenLibrary(utilname, 37);
   DOSBase                    = __InternalDOSBase;

#ifdef __AMIGAOS4__
   __InternalDOSIFace         = (struct DOSIFace*)      GetInterface(__InternalDOSBase,      "main", 1, 0);
   __InternalIntuitionIFace   = (struct IntuitionIFace*)GetInterface(__InternalIntuitionBase,"main", 1, 0);
   __InternalUtilityIFace     = (struct UtilityIFace*)  GetInterface(__InternalUtilityBase,  "main", 1, 0);
   IDOS                       = __InternalDOSIFace;
#endif

   if (0 != __InternalDOSBase)
   {
      stdin    = Input();
      stdout   = Output();
   }

   // 3. Set up a memory pool. we will need it for our initializations (mem allocs etc)
   __InternalMemPool = CreatePool(MEMF_PUBLIC | MEMF_SHARED, 65536, 65536);
   if (0 == __InternalMemPool)
      return false;

/*   
   // 4. call initializers.
   if (false == set_call_funcs(__INIT_LIST__, 1, 1))
      return false;

   // 5. call global constructors
   if (false == set_call_funcs(__CTORS_LIST__, -1, 0))
      return false;
*/    
   return true;
}

void __getwbmessage()
{
   __WBStartup = 0;
   if (StartupFlags & SF_NoWBStartup)
      return;

   Process *p = (Process*)FindTask(0);
   if (0 == p->pr_CLI)
   {
#ifdef __AMIGAOS4__
      struct DOSIFace* IDOS = __InternalDOSIFace;
#else
      Library* DOSBase = __InternalDOSBase;
#endif

      BPTR lock;
      
      WaitPort(&p->pr_MsgPort);
      __WBStartup = (WBStartup*)GetMsg(&p->pr_MsgPort);
      
      /* make sure we have an adequate working dir */
      if ((0 != __WBStartup) && (0 != __WBStartup->sm_ArgList) && (0 != __WBStartup->sm_ArgList->wa_Lock))
      {
         lock = DupLock(__WBStartup->sm_ArgList->wa_Lock);
         lock = CurrentDir(lock);
         UnLock(lock);
      }
   }
}

void __freewbmessage()
{
#ifndef __MORPHOS__
   if (NULL != __WBStartup)
   {
      Forbid();
      ReplyMsg((Message*)__WBStartup);
   }
#endif
}

void __cleanup()
{
   // 1. if no exec, it is likely we did not run anything at all...
   if (0 == SysBase)
      return;
/*   
   // 2. call global destructors
   set_call_funcs(__DTORS_LIST__, 1, 0);
   
   // 3. call cleaners
   set_call_funcs(__EXIT_LIST__, -1, 0);
*/

   // 4. free memory pool
   DeletePool(__InternalMemPool);

#ifdef __AMIGAOS4__
   if (0 != __InternalIntuitionIFace)
      DropInterface((Interface*)__InternalIntuitionIFace);
   if (0 != __InternalDOSIFace)
      DropInterface((Interface*)__InternalDOSIFace);
   if (0 != __InternalUtilityIFace)
      DropInterface((Interface*)__InternalUtilityIFace);
#endif

   if (0 != __InternalDOSBase)
      CloseLibrary(__InternalDOSBase);
   if (0 != __InternalIntuitionBase)
      CloseLibrary(__InternalIntuitionBase);
   if (0 != __InternalUtilityBase)
      CloseLibrary(__InternalUtilityBase);
}

int _go() 
{
   int rc = 20;

   if (__setup())
   {
      __getwbmessage();
      rc = main();
      __cleanup();
      __freewbmessage();

   }

   return rc;
}

void __main()
{
}

extern "C" {
void _error(const char* text, ...)
{
#ifndef __AMIGAOS4__
   struct Library* DOSBase       = __InternalDOSBase;
   struct Library* IntuitionBase = __InternalIntuitionBase;
#else
   struct DOSIFace        *IDOS        = __InternalDOSIFace;
   struct IntuitionIFace  *IIntuition  = __InternalIntuitionIFace;
#endif
   // 20! really 20 -- OS4 has some extra fields, and i am preserving space.
   const IPTR es[] = 
   { 
      20,                              // size
      0,                               // window
      (IPTR)"<< ERROR >>",    // title
      (IPTR)text,             // body
      (IPTR)"OK"              // gadgets
   };

   AROS_SLOWSTACKFORMAT_PRE(text);
   if (0 != __InternalIntuitionBase)
      EasyRequestArgs(0, (struct EasyStruct*)&es, 0, AROS_SLOWSTACKFORMAT_ARG(text));
   else if (0 != __InternalDOSBase)
      VFPrintf(stdout, (char*)text, AROS_SLOWSTACKFORMAT_ARG(text));
   AROS_SLOWSTACKFORMAT_POST(text);
}
};

