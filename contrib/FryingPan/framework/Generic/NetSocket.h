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

#ifndef _NETSOCKET_H_
#define _NETSOCKET_H_

#include "Thread.h"
#include "RWSync.h"
#include "HookT.h"
#include <exec/libraries.h>
#include "VectorT.h"

class BSDSocketIFace;
struct sockaddr_in;

namespace GenNS
{
   class NetSocket 
   {
   public:
      enum SocketType
      {
         Skt_TCP,
         Skt_UDP
      };

      enum SocketEvent
      {
         SEv_DataReady     = 1,
         SEv_WriteReady    = 2,
         SEv_IncomingCall  = 4,
         SEv_OtherEvent    = 8,        // other signal reported,
         SEv_TimeOut       = 16,       // no change
         SEv_Disconnected  = 0x8000,

         SEv_AllEvents     = 0xffff
      };

      struct SocketID
      {
         int32    id;
         int32    domain;
         int32    type;
         int32    proto;
      };
   protected:
      static int32               stTrue;
      static int32               stFalse;

      int32                      lSocket;
      uint32                     lSignal;
      int32                      nSignal;
      bool                       bConnected;
      bool                       bServer;
      unsigned long              lThreadSignal;
      BSDSocketIFace            *bsock;
      int32                      error;
      sockaddr_in               *addr_local;
      sockaddr_in               *addr_remote;

   protected:
      virtual void               FConstruct(SocketID* skt, bool server);
      virtual bool               FConnect(char* AHost, int16 APort, SocketType stype);
      virtual void               FDisconnect();
      virtual bool               FBind(int16 APort, SocketType stype);
      virtual bool               FResolve(char* AHost, int APort, struct sockaddr_in *AAddr);
      virtual int32              FTypeToInet(SocketType type);
      virtual bool               FTryAccept(SocketID*);
      virtual int32              FReadBuffer(void* buff, int32 len);
      virtual int32              FWriteBuffer(void* buff, int32 len);
      virtual uint32             FGetStatus(uint32 millis, uint32 sigs, uint16 events);
      virtual void               FGetAddresses();
      virtual bool               FSetupSocket();

   public:
      NetSocket(SocketID *socket=0, bool isserver=false);
      virtual                   ~NetSocket();  

      virtual bool               Connect(char* AHost, int APort, SocketType=Skt_TCP);
      virtual void               Disconnect();
      virtual int32              ReadBuffer(void* buffer, int32 maxlen);
      virtual int32              WriteBuffer(void* buffer, int32 maxlen);
      virtual bool               Listen(int APort, SocketType=Skt_TCP);
      virtual int32              GetDataAvail();
      virtual bool               Accept(SocketID*);
      virtual void               SetSignals(uint32 signal);
      virtual uint32             GetSignals();
      virtual uint32             GetStatus();
      virtual uint32             WaitStatus(uint32 millis, uint32 sigs, uint16 events = SEv_AllEvents & ~SEv_WriteReady);
      virtual uint32             GetLocalIP();
      virtual uint16             GetLocalPort();
      virtual uint32             GetRemoteIP();
      virtual uint16             GetRemotePort();
      virtual int32              Errno();
   };
};
#endif //_NETSOCKET_H_

