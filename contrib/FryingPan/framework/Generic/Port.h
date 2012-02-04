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

#ifndef _PORT_H_
#define _PORT_H_

#include "HookAttrT.h"
#include "Synchronizer.h"
#include "Types.h"

namespace GenNS
{
   class Port 
   {
      struct MsgPort      *pPort;
   public:
      HookAttrT<sint, void*>  Handler;

      Port(const struct Hook *pHook);
      virtual             ~Port();
      uint32               GetSignals();
      void                 DoAsync(sint ACmd, void* AData);
      uint                 DoSync(sint ACmd, void* AData);
      void                 HandleSignals();
   };
};

#endif //_PORT_H_
