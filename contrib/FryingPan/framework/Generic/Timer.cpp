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

#include "Timer.h"
#include <libclass/exec.h>
#include <devices/timer.h>
#include <exec/ports.h>
#include "LibrarySpool.h"

using namespace GenNS;

Timer::Timer() 
{
   issued = 0;
   port = Exec->CreateMsgPort();
   treq = (timerequest*)Exec->CreateIORequest(port, sizeof(timerequest));
   Exec->OpenDevice("timer.device", UNIT_VBLANK, (IORequest*)treq, 0);
}

Timer::~Timer()
{
   AbortRequest();
   WaitRequest();
   Exec->CloseDevice((IORequest*)treq);
   Exec->DeleteIORequest((IORequest*)treq);
   Exec->DeleteMsgPort(port);
}

void Timer::WaitRequest()
{
   
   if (issued != 0) 
   {
      Exec->WaitPort(port);
      treq = (timerequest*)Exec->GetMsg(port);
      issued = 0;
   }
}

void Timer::AbortRequest()
{
   if (issued != 0)
   {
      Exec->AbortIO((IORequest*)treq);
      WaitRequest();
   }
}

void Timer::AddRequest(unsigned long duration)  // milliseconds ?
{
   /*
    * don't initiate another request unless this one is finished.
    */
   if (issued != 0)
   {
      struct timerequest *tr = (struct timerequest*)Exec->GetMsg(port);
      if (tr == 0)
         return;

      treq = tr;
      issued = 0;
   }
   treq->tr_node.io_Command = TR_ADDREQUEST;
   treq->tr_time.tv_secs  = (duration/1000);
   treq->tr_time.tv_micro = (duration % 1000) * 1000;
   Exec->SendIO((IORequest*)treq);
   issued = 1;
}

unsigned long Timer::GetSignals(void)
{
   return 1<<port->mp_SigBit;
}
