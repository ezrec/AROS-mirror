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

#include "Port.h"
#include "Msg.h"
#include <exec/ports.h>
#include <utility/hooks.h>
#include <libclass/exec.h>
#include "LibrarySpool.h"

using namespace GenNS;

Port::Port(const Hook *pHook) 
{
   pPort = Exec->CreateMsgPort();
   Handler = pHook;
}

Port::~Port()
{
   MsgPort *pXPort = pPort;
   pPort = 0;
   
   if (pXPort)
   {
      Message *pMsg;
      while ((pMsg = Exec->GetMsg(pXPort)) != 0)
      {
         ((Msg*)pMsg->mn_Node.ln_Name)->Reply(0);
      }
      Exec->DeleteMsgPort(pXPort);
   }
}

uint32 Port::GetSignals()
{
   if (!pPort) return 0;
   return (1<<pPort->mp_SigBit);
}

void Port::DoAsync(sint ACmd, void* AData)
{
   if (!pPort) return;
   Exec->PutMsg(pPort, new Msg(false, ACmd, AData));
}

uint Port::DoSync(sint ACmd, void* AData)
{
   unsigned long lRes = 0;
   if (!pPort) return 0;
   Msg *pMsg = new Msg(true, ACmd, AData);
   Exec->PutMsg(pPort, pMsg);
   lRes = pMsg->WaitFor();
   delete pMsg;   
   return lRes;
}

void Port::HandleSignals()
{
   Message *pMessage;
   if (pPort)
   {
      if (Handler.IsValid())
      {
         while(true)
         {
            pMessage = Exec->GetMsg(pPort);
            if (pMessage == 0) 
               break;
            Msg *pMsg = (Msg*)pMessage->mn_Node.ln_Name;
            uint lVal = Handler(pMsg->GetCommand(), pMsg->GetData());
            pMsg->Reply(lVal);
         }  
      }  
   }
}
