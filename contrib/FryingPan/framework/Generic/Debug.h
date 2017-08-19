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

#if !defined (__DEBUG_H) 
#define __DEBUG_H

#include "LibrarySpool.h"
#include "String.h"
#include "Thread.h"
#include "HookT.h"
#include <LibC/LibC.h>

namespace GenNS
{
   class DbgMsg;
   class DbgHandler;
   class DbgMaster;

   enum DbgLevel
   {
      Lvl_Info    =  1,
      Lvl_Dump    =  2,
      Lvl_Debug   =  4,
      Lvl_Warning =  8,
      Lvl_Error   = 16,
      Lvl_Failure = 32
   };

   class DbgCmd
   {
   protected:
      const DbgHandler *parent;
      DbgLevel          lvl;

   public:
      DbgCmd(const DbgHandler *pParent, DbgLevel lvl)
      {
         parent    = pParent;
         this->lvl = lvl;
      }

      virtual       ~DbgCmd()
      {
      };

      virtual void   Perform() = 0;
   };

   class DbgMsg : public DbgCmd
   {
   protected:
      String        *text;
      void          *data;
      uint16         len;
   public:

      DbgMsg(const DbgHandler *pParent, DbgLevel lvl, const char* sFmtStr, IPTR vFmtArr, void* pData, int lLen);
      virtual       ~DbgMsg(void);

      virtual const char  *Text();
      virtual void  *Data();
      virtual uint16 Length();
      virtual void   Perform();
   };

   class DbgPing : public DbgCmd
   {
   public:
      DbgPing(const DbgHandler *pParent)
         : DbgCmd(pParent, Lvl_Debug)
      {
      }

      virtual void   Perform()
      { 
         // we don't really have to do anything here.
      };
   };

   class DbgHandler
   {
      DbgHandler    *next;
      String         name;
      BPTR           fh1, fh2;
      unsigned       silent:1;
      unsigned       nodate:1;
      Task          *stask;
      Thread        *dest;
      uint16         flags;

   protected:
      void           PutDate() const;

   public:   
      DbgHandler(int, const char*);
      ~DbgHandler();
      void           DumpMsg(DbgMsg*) const;
      void           DoAsync(DbgLevel lvl, const char* sFmtString, IPTR vFmtArgs, void* bMemBlock, int lMemLen) const;
      void           DoSync(DbgLevel lvl, const char* sFmtString, IPTR vFmtArgs, void* bMemBlock, int lMemLen) const;

      void           InitH();
      void           ExitH();
   };

   class DbgMaster 
   {
   public:
      enum Cmd
      {
         Cmd_Message = 0,
         Cmd_InitH,
         Cmd_ExitH
      };

   private:
      Thread           *pThread;
      SignalSemaphore   ssem;
      Process          *proc;
      MsgPort          *MP;
      DbgHandler       *list;
      static DbgMaster *DebugMaster;
      static long       lNumInstances;

      HookT<DbgMaster, Cmd, void*>  hHkProc;

   protected:
      DbgMaster(void);
      virtual          ~DbgMaster(void);
      unsigned long     ProcMsgs(Cmd lCmd, void* pMsg);

   public:
      static Thread    *GetDebug();
      static void       FreeDebug();
      static bool       CleanUp(void);
   };
};
#define DEBUG
#if defined( DEBUG ) 
#define _D(l, a, b...)        __dbg->DoAsync(l, a, SIZEARRAY(b), 0, 0)
#define _DD(a,b)              __dbg->DoAsync(Lvl_Dump, 0, 0, a, b)
#define _DDT(a, b, c)         __dbg->DoAsync(Lvl_Dump, a, 0, b, c)
#define _ND(a)                __dbg = new DbgHandler(1, a);
#define _NDS(a)               __dbg = new DbgHandler(0, a);
#define USING_DEBUG           DbgHandler *__dbg
#define DECLARE_DEBUG         extern DbgHandler *__dbg;
#define DEFINE_DEBUG          DbgHandler *__dbg
#define DEBUG_ENGINE          __dbg
#define SET_DEBUG_ENGINE(v)   __dbg = (v)
#define _ED()                 delete __dbg

#define _createDebug(loud, name) setDebug(new DbgHandler(loud, name))
#define _destroyDebug()          delete getDebug();
#define _d(l, a, b...)           getDebug()->DoAsync(l, a, SIZEARRAY(b), 0, 0);
#define _dx(l, a, b...)          getDebug()->DoAsync(l, "%s: " a, SIZEARRAY((uint)__PRETTY_FUNCTION__, b), 0, 0);
#define _dd(a, b)                getDebug()->DoAsync(Lvl_Dump, 0, 0, a, b);
#define _ddt(a, b, c)            getDebug()->DoAsync(Lvl_Dump, a, 0, b, c);
#else
#define _D(a...)  //
#define _ND(a)    //
#define _NDS(a)   //
#define _ED()     //
#define _DD(a, b)
#define USING_DEBUG
#define DEFINE_DEBUG
#define DECLARE_DEBUG
#define DEBUG_ENGINE ((DbgHandler*)0)
#define SET_DEBUG_ENGINE(v)  
#define _DDT(a, b, c)

#define _createDebug(loud, name)
#define _destroyDebug()
#define _d(a, b...)
#define _dx(l, a, b...)
#define _dd(a, b)
#define _ddt(a, b, c)
#endif

#endif


