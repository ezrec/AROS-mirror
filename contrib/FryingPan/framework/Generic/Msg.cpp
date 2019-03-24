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

#include "Msg.h"
#include <libclass/exec.h>
#include "LibrarySpool.h"

using namespace GenNS;

GenNS::Msg::Msg(bool ASynchronous, unsigned long ACmd, void* AData)
{
   bSync = ASynchronous;
   lCmd  = ACmd;
   pData = AData;
   pPort = 0;
   mn_Node.ln_Name = (char*)this;
   if (bSync)
   {
      pPort = Exec->CreateMsgPort();
      mn_ReplyPort = pPort;
   }
}

GenNS::Msg::~Msg()
{
   if (pPort) 
   {
      Exec->DeleteMsgPort(pPort);
   }
}

void GenNS::Msg::Reply(unsigned long AResult)
{
   lResult = AResult;
   if (bSync)
   {
      Exec->ReplyMsg(this);
   }
   else
   {
      delete this;
   }
}

uint32 GenNS::Msg::WaitFor()
{
   if (bSync)
   {
      Exec->WaitPort(pPort);
      return lResult;
   }
   return 0;
}

sint GenNS::Msg::GetCommand()
{
   return lCmd;
}

void *GenNS::Msg::GetData()
{
   return pData;
}
