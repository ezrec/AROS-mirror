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

#include "Thread.h"
#include <exec/ports.h>
#include <dos/dos.h>
#include <exec/semaphores.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <libclass/utility.h>
#include <libclass/exec.h>
#include <libclass/dos.h>
#include <dos/dosextens.h>
#include "HookAttrT.h"
#include "String.h"
#include "Timer.h"
#include "LibrarySpool.h"

#ifndef __MORPHOS__
   #define NP_CodeType  TAG_IGNORE
   #define CODETYPE_PPC 0
#endif

using namespace GenNS;

void Thread::Create()
{
   Message  *m = 0;
   MsgPort  *p = 0;
               
   IPTR tags[] = {
      NP_Entry,      (IPTR)&__proc_init_code,
      NP_StackSize,  65536,
      NP_Name,       (IPTR)hName.Data(),
      NP_CodeType,   CODETYPE_PPC,
      TAG_DONE,      0
   };

   pProcess = DOS->CreateNewProc((TagItem*)tags);    
   p = Exec->CreateMsgPort();
   m = new Message; 
   m->mn_Length         = sizeof(Message);     
   m->mn_Node.ln_Name   = (char*)this;
   m->mn_ReplyPort      = p;
   Exec->PutMsg(&pProcess->pr_MsgPort, m);
   Exec->WaitPort(p);
   m = Exec->GetMsg(p);
   Exec->DeleteMsgPort (p); 
   delete m;
}

void Thread::Terminate()
{
   if (!Exec->AttemptSemaphore(pSemaphore))
   {
      Exec->Signal(&pProcess->pr_Task, lKillSignal);
      Exec->ObtainSemaphore(pSemaphore);  
   }
   Exec->ReleaseSemaphore(pSemaphore);
}

bool Thread::IsTerminated()
{
   if (Exec->AttemptSemaphore(pSemaphore))
   {
      Exec->ReleaseSemaphore(pSemaphore);
      return true;
   }
   return false;
}

void Thread::WaitTerminated()
{
   Exec->ObtainSemaphore(pSemaphore);  
   Exec->ReleaseSemaphore(pSemaphore);
}

unsigned long Thread::GetSignals()
{
   return lKillSignal | pTimer->GetSignals() | pPort->GetSignals();
}

bool Thread::HandleSignals(int32 ATimeout)
{
   return HandleSignals(ATimeout, 0);
}

bool Thread::HandleSignals(int32 ATimeout, uint32 ASignals)
{
   unsigned long lSignals = ASignals;
   unsigned long lRes;
  
   wasTimer = false;

   if ((ATimeout != 0) && (ATimeout != -1))
   {
      pTimer->AddRequest(ATimeout);   
   }
   else if (ATimeout == 0)
   {
      pTimer->AbortRequest();
   }
   
   lSignals |= GetSignals();
   do
   {
      pPort->HandleSignals();             // if there are any pending messages, handle them at once
      if (ATimeout)                       // if timeout has been set...
         lRes = Exec->Wait(lSignals);     // wait for any signal or timeout
      else                                // otherwise
         lRes = Exec->SetSignal(0, lSignals); // just check signals

      if (lRes & lKillSignal)             // if we got killed
         return true;                     // exit immediately 
      if (lRes &pTimer->GetSignals())
         wasTimer = true;
      if (lRes &pPort->GetSignals())      // if we received a new message
         pPort->HandleSignals();          // take care of all messages
      if (lRes &~pPort->GetSignals())     // otherwise, if it was a timer or any other signal
         return false;                    // quit & report
   } while (ATimeout);                    // eventually loop.
   return false;
}

bool Thread::IsTimerSignal()
{
   return wasTimer;
}

void Thread::__proc_init_code()
{
   Process          *p;                         // pointer to this process
   Message          *m;                         // pointer to received message
   Thread           *d;                         // pointer to the class

   p = (Process*) Exec->FindTask(0);            // get process message
   Exec->WaitPort(&p->pr_MsgPort);              // we expect our class pointer
   m = Exec->GetMsg(&p->pr_MsgPort);            // handle it
   d = (Thread*)m->mn_Node.ln_Name;             // get class pointer
   d->lKillSignal = 1<<Exec->AllocSignal(-1);   // we dont have to free it.
   d->pPort       = new Port(0);
   d->pTimer      = new Timer();
   Exec->ObtainSemaphore(d->pSemaphore);
   Exec->ReplyMsg(m);
   d->hHook(d, d->pData);
   delete d->pPort;
   delete d->pTimer;
   d->pProcess = 0;
   Exec->ReleaseSemaphore(d->pSemaphore);
};
    
Thread::Thread(const char* AName, const Hook *AHook, void* AData) : 
   hName(AName), 
   pData(AData),
   hHook(AHook)
{
   pSemaphore = new SignalSemaphore;
   Exec->InitSemaphore(pSemaphore);
   Create();
};

Thread::Thread(const char* AName) :
   hName(AName),
   pData(0),
   hHook(hDefault)
{
   hDefault.Initialize(this, &Thread::fDefaultHandler);
   pSemaphore  = new SignalSemaphore;
   Exec->InitSemaphore(pSemaphore); 
   Create();
}

Thread::~Thread()
{
   Terminate();
   delete pSemaphore;
}

void Thread::SetHandler(const Hook *AHandler)
{
   pPort->Handler = AHandler;
}

void Thread::DoAsync(unsigned long ACommand, void* ApData)
{
   pPort->DoAsync(ACommand, ApData);
}

unsigned long Thread::DoSync(unsigned long ACommand, void* ApData)
{
   return pPort->DoSync(ACommand, ApData);
}

void Thread::ProcMessages()
{
   pPort->HandleSignals();
}

unsigned long Thread::fDefaultHandler(struct Thread *pThis, void*)
{
   pThis->HandleSignals(0xffffffff);
   return 0;  
}
