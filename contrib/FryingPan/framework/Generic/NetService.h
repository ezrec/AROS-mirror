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

#include "Generic.h"
#include "Thread.h"
#include "RWSync.h"
#include "HookT.h"
#include <exec/libraries.h>
#include "VectorT.h"
#include "HookAttrT.h"

class BSDSocketIFace;

namespace GenNS
{
   class NetService : public Generic
   {
   public:
      struct Socket
      {
         int32       socket;
         bool        listening;
         void       *userData;
         HookAttrT    hOnAccept;       // Socket*, sockaddr*
         HookAttrT    hOnDataAvailable;
         HookAttrT    hOnWriteReady;
         HookAttrT    hOnClose;
      };

      enum SocketType
      {
         Skt_TCP,
         Skt_UDP
      };

   protected:
      enum SubCmd
      {
         Sub_NOP           =  0,       // no operation
         Sub_Disconnect,               // disconnect, close socket.
         Sub_Connect,                  // connect to remote host
         Sub_Listen,                   // bind to local port, prepare server
         Sub_Read,                     // :]
         Sub_Write,                    // :]
         Sub_User          =  1024     // first command you are allowed to use.
      };

   protected:
      struct SubStr_Connect
      {
         char       *pHost;
         int16       lPort;
         SocketType  eType;
         const Hook *pDataAvail;
         const Hook *pWriteReady;
         const Hook *pClosed;
      };

      struct SubStr_Listen
      {
         int16       lPort;
         SocketType  eType;
         const Hook *pAcceptIncoming;
      };

      struct SubStr_Disconnect
      {
         Socket     *socket;
      };

      struct SubStr_ReadBuffer
      {
         Socket     *socket;
         void       *data;
         int32       maxlen;
      };

      struct SubStr_WriteBuffer
      {
         Socket     *socket;
         void       *data;
         int32       maxlen;
      };

   protected:
      static int32            stTrue;
      static int32            stFalse;

      unsigned long           lThreadSignal;
      Thread                 *pSocketThread;
      RWSync                  hRWSync;
      BSDSocketIFace         *bsock;
      int32                   error;
      VectorT<Socket*>        hSockets;

   protected:
      HookT<NetService, Thread*, void*> hHkProc;
      HookT<NetService, uint32, void*>  hHkEvent;


   protected:
      virtual void               FConstruct();
      virtual Socket            *FConnect(char* AHost, int16 APort, const Hook* hDataRdy, const Hook* hWriteRdy, const Hook* hClosed, SocketType stype);
      virtual int                FDisconnect(Socket *socket);
      virtual Socket            *FBind(char* AHost, int16 APort, const Hook* hAccept, SocketType stype);
      virtual bool               FResolve(char* AHost, int APort, struct sockaddr_in *AAddr);
      virtual int                FWriteSocket(int32 lSocket, void* pBuff, int lData);
      virtual Socket            *FAddSocket(int32 sktid, bool listening, const Hook *accpt, const Hook *datardy, const Hook *wrtrdy, const Hook *closed);
      virtual void               FRemSocket(Socket *skt);
      virtual int32              FTypeToInet(SocketType type);
      virtual void               FTryAccept(Socket *skt);
      virtual void               FTryRead(Socket *skt);
      virtual int32              FReadBuffer(Socket *skt, void* buff, int32 len);
      virtual int32              FWriteBuffer(Socket *skt, void* buff, int32 len);
      virtual Socket            *FGetSocket(int32 skt);

   protected:  /* process */
      virtual unsigned long      FSocketProc(Thread*, void*);
      virtual unsigned long      FProcCommands(unsigned long ACmd, void* AData);
   public:
      NetService();
      virtual                   ~NetService();  

      virtual NetService::Socket *Connect(char* AHost, int APort, const Hook* onData=0, const Hook* onWriteReady=0, const Hook* onClose=0, SocketType=Skt_TCP);
      virtual void               Disconnect(NetService::Socket *skt);
      virtual int32              ReadBuffer(NetService::Socket *skt, void* buffer, int32 maxlen);
      virtual int32              WriteBuffer(NetService::Socket *skt, void* buffer, int32 maxlen);
      virtual NetService::Socket *Listen(int APort, const Hook* onAccept=0, SocketType=Skt_TCP);
   };
};

#endif //_NETSOCKET_H_

