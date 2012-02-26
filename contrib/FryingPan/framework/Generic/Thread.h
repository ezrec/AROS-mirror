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

#ifndef _THREAD_H_
#define _THREAD_H_

#include "Port.h"
#include "HookAttrT.h"
#include "HookT.h"
#include "Types.h"
#include "String.h"

struct Process;



namespace GenNS
{
   class Thread 
   {
      struct Process         *pProcess;
      struct SignalSemaphore *pSemaphore;
      String                  hName;
      void                   *pData;
      HookAttrT<Thread*, void*> hHook;
      class Timer            *pTimer;
      Port                   *pPort;
      unsigned long           lKillSignal;
      HookT<Thread, Thread*, void*> hDefault;
      bool                    wasTimer;

      static void             __proc_init_code();
      unsigned long           fDefaultHandler(struct Thread *pThis, void*);
   public:

      void                    Create();
      bool                    HandleSignals(int32 ATimeout);
      bool                    HandleSignals(int32 ATimeout, uint32 ASignals);
      void                    Terminate();
      bool                    IsTerminated();
      bool                    IsTimerSignal();
      void                    WaitTerminated();
      unsigned long           GetSignals();
      void                    SetHandler(const Hook *AHandler);
      void                    DoAsync(unsigned long ACommand, void* ApData);
      unsigned long           DoSync(unsigned long ACommand, void* ApData);
      void                    ProcMessages();
      Thread(const char* AName, const struct Hook *AHook, void* AData);
      Thread(const char* AName);
      ~Thread();
   };
};

#endif //_THREAD_H_
