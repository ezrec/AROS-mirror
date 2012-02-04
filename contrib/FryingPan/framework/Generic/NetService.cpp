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

#include "LibrarySpool.h"
#include "NetService.h"
#include "Synchronizer.h"
#include <libclass/bsdsocket.h>
#include <libdata/bsdsocket/netdb.h>
#include <libdata/bsdsocket/in.h>
#include <libdata/bsdsocket/ioctl.h>
#include <libdata/bsdsocket/socketbasetags.h>
#include <proto/exec.h>
#include "LibrarySpool.h"

using namespace GenNS;

   int32          NetService::stTrue    = 1;
   int32          NetService::stFalse   = 0;

NetService::NetService() 
{
   FConstruct();
}

void NetService::FConstruct()
{
   lThreadSignal=0;

   hHkProc.Initialize(this, &NetService::FSocketProc);
   hHkEvent.Initialize(this, &NetService::FProcCommands);

   pSocketThread = new Thread("Socket Thread", hHkProc.GetHook(), (void*)0);
   pSocketThread->SetHandler(hHkEvent.GetHook());
}

NetService::~NetService()
{
   delete pSocketThread;
}
 
int NetService::FWriteSocket(int32 lSocket, void* pBuff, int lData)
{
   return bsock->send(lSocket, (UBYTE*)pBuff, lData, 0);
}

bool NetService::FResolve(char* AHost, int APort, sockaddr_in *AAddr)
{
   if (AHost)
   {
      hostent *pHost = bsock->gethostbyname(AHost);

      if (0 != pHost)
      {
         CopyMem(pHost->h_addr_list[0], &AAddr->sin_addr, sizeof(AAddr->sin_addr));   
      }
      else
      {
         AAddr->sin_addr.s_addr = bsock->inet_addr(AHost);
         if (AAddr->sin_addr.s_addr == 0xffffffff)
            return false;
      }   
   }
   else
   {
      AAddr->sin_addr.s_addr = INADDR_ANY;
   }
   
   AAddr->sin_family = AF_INET;
   AAddr->sin_port   = W2BE(APort);  
   return true;
}

NetService::Socket *NetService::FConnect(char* AHost, int16 APort, const Hook* hDataRdy, const Hook* hWriteRdy, const Hook* hClosed, SocketType stype)
{
   int32       lSocket;
   sockaddr_in addr;
   Socket     *s;
   uint32      lSockEvMask = FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE | FD_ERROR;
   
   lSocket = bsock->socket(AF_INET, FTypeToInet(stype), 0);    
   bsock->setsockopt(lSocket, SOL_SOCKET, SO_EVENTMASK, &lSockEvMask, sizeof(unsigned long));
   
   if (!FResolve(AHost, APort, &addr))
   {
      bsock->CloseSocket(lSocket);
      return 0;
   }

   error = bsock->setsockopt(lSocket, SOL_SOCKET, SO_KEEPALIVE, &stTrue, sizeof(stTrue));
   if (error < 0)
   {
      bsock->CloseSocket(lSocket);
      return 0;
   }

   error = bsock->connect(lSocket, (sockaddr*)&addr, sizeof(addr));
   if (error < 0)
   {
      bsock->CloseSocket(lSocket);
      return 0;
   }

   s = FAddSocket(lSocket, false, 0, hDataRdy, hWriteRdy, hClosed);
   FTryRead(s);
   return s;
}
 
int NetService::FDisconnect(NetService::Socket *socket)
{
   bsock->shutdown(socket->socket, 2);
   bsock->CloseSocket(socket->socket);
   FRemSocket(socket);
   return 0;   
}

NetService::Socket *NetService::FBind(char* AHost, int16 APort, const Hook* hAccept, SocketType stype)
{
   int32       lSocket;
   sockaddr_in addr;
   uint32      lSockEvMask = FD_CONNECT | FD_ACCEPT | FD_READ | FD_CLOSE | FD_ERROR;
   int32       pTrue       = 1;

   lSocket = bsock->socket(AF_INET, FTypeToInet(stype), 0);    
   bsock->setsockopt(lSocket, SOL_SOCKET, SO_EVENTMASK, &lSockEvMask, sizeof(unsigned long));
   bsock->IoctlSocket(lSocket, FIONBIO, (char*)&pTrue);
   bsock->IoctlSocket(lSocket, FIOASYNC, (char*)&pTrue);
   
   if (!FResolve(AHost, APort, &addr))
   {
      bsock->CloseSocket(lSocket);
      return false;
   }

   bsock->setsockopt(lSocket, SOL_SOCKET, SO_KEEPALIVE, &stTrue, sizeof(stTrue));

   error = bsock->bind(lSocket, (sockaddr*)&addr, sizeof(addr));
   if (error < 0)
   {
      bsock->CloseSocket(lSocket);
      return 0;
   }

   error = bsock->listen(lSocket, 5);
   if (error < 0)
   {
      bsock->CloseSocket(lSocket);
      return 0;
   }

   return FAddSocket(lSocket, true, hAccept, 0, 0, 0);
}

int32 NetService::FTypeToInet(SocketType stype)
{
   switch (stype)
   {
      case Skt_TCP:
         return SOCK_STREAM;
         break;
      case Skt_UDP:
         return SOCK_DGRAM;
         break;
      default:
         return SOCK_STREAM;
   };
}

NetService::Socket *NetService::FAddSocket(int32 sktid, bool listening, const Hook *accpt, const Hook *datardy, const Hook *wrtrdy, const Hook *closed)
{
   Socket *s = new Socket;
   
   s->socket            = sktid;
   s->listening         = listening;
   s->hOnAccept         = accpt;
   s->hOnDataAvailable  = datardy;
   s->hOnWriteReady     = wrtrdy;
   s->hOnClose          = closed;            

   hSockets << s;

   return s;
}

void NetService::FRemSocket(Socket *skt)
{
   hSockets >> skt;
   delete skt;
}

void NetService::FTryAccept(Socket *skt)
{
   sockaddr    saddr;
   int32       laddr;
   int32       sock = bsock->accept(skt->socket, &saddr, &laddr);
   Socket     *newskt;

   if (sock < 0)
      return;

//   MessageBox("Info", "New client connected!", "Ok", 0);

   newskt = FAddSocket(sock, false, 0, 0, 0, 0);

   if (0 == skt->hOnAccept.Call(newskt, &saddr))
   {
      skt->hOnClose.Call(skt, (void*)0);
      FDisconnect(newskt);
   }
   else
   {
      uint32      lSockEvMask = FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE | FD_ERROR;
   
      bsock->setsockopt(newskt->socket, SOL_SOCKET, SO_EVENTMASK, &lSockEvMask, sizeof(unsigned long));
      bsock->IoctlSocket(newskt->socket, FIOASYNC, (char*)&stTrue);
   
      FTryRead(newskt);
   }
   return;
}

int32 NetService::ReadBuffer(NetService::Socket *skt, void* buffer, int32 maxlen)
{
   SubStr_ReadBuffer rb;

   rb.socket = skt;
   rb.data   = buffer;
   rb.maxlen = maxlen;

   return pSocketThread->DoSync(Sub_Read, &rb);
}

int32 NetService::WriteBuffer(NetService::Socket *skt, void* buffer, int32 maxlen)
{
   SubStr_WriteBuffer wb;

   wb.socket = skt;
   wb.data   = buffer;
   wb.maxlen = maxlen <? 16384;

   return pSocketThread->DoSync(Sub_Write, &wb);
}

int32 NetService::FReadBuffer(Socket *skt, void* buff, int32 len)
{
   int32 bytes;

   bsock->IoctlSocket(skt->socket, FIONREAD, &bytes);
   if (bytes > 0)
   {
      bytes = bsock->recv(skt->socket, buff, bytes <? len, 0);
      return bytes;
   }
   else if (bytes < 0)
   {
      skt->hOnClose.Call(skt, (void*)0);
   }
   return 0;
}

int32 NetService::FWriteBuffer(Socket *skt, void* buff, int32 len)
{
   len = bsock->send(skt->socket, buff, len, 0);
   if (len > 0)
   {
      return len;
   }
   else
   {
      skt->hOnClose.Call(skt, (void*)0);
   }
   return 0;
}

void NetService::FTryRead(Socket *skt)
{
   int32 bytes = 0;

   bsock->IoctlSocket(skt->socket, FIONREAD, &bytes);
   if (bytes >= 0)
   {
      char test[4];
      int err;
      err = bsock->recv(skt->socket, &test, 0, MSG_PEEK);
      if (err < 0)
      {
         MessageBox("Info", "%ld - %ld", "Ok", ARRAY(err, bsock->Errno()));
      }
   }
   if (bytes > 0)
   {
      skt->hOnDataAvailable.Call(skt, bytes);
   }
}
   
NetService::Socket *NetService::FGetSocket(int32 skt)
{
   for (int i=0; i<hSockets.Count(); i++)
   {
      if (hSockets[i]->socket == skt)
         return hSockets[i];
   }
   return 0;
}

unsigned long NetService::FSocketProc(Thread *pThis, void* ASocket)
{
   fd_set   set, srd, swr, sxc;
   int      maxsock = 0;
   timeval  tv = 
   {  
      0,    // seconds
      0     // micro
   };

   bsock = BSDSocketIFace::GetInstance(0);
   
   lThreadSignal = 1<<AllocSignal(-1);
   
   bsock->SocketBaseTagList((TagItem*)ARRAY(SBTC_SIGEVENTMASK, lThreadSignal, 0));
   bsock->SetSocketSignals(0, lThreadSignal, 0);
   
   while (!pThis->HandleSignals(0xffffffff, lThreadSignal))  // if connected,don't wait.
   {

      int32  socket = 0;
      uint32 event = 0;

      for (socket = bsock->GetSocketEvents(&event);
           socket != -1;
           socket = bsock->GetSocketEvents(&event))
      {
         Socket *skt = FGetSocket(socket);
         switch (event)
         {
            case FD_ACCEPT:
               FTryAccept(skt);
               break;

            case FD_CONNECT:
               MessageBox("Info", "Socket %ld connected", "Ok", ARRAY(socket));
               break;

            case FD_READ:
               FTryRead(skt);
               break;

            case FD_WRITE:
               MessageBox("Info", "Socket %ld ready for write", "Ok", ARRAY(socket));
               break;

            case FD_ERROR:
               MessageBox("Info", "Socket %ld reported error", "Ok", ARRAY(socket));
               break;

            case FD_CLOSE:
               MessageBox("Info", "Socket %ld closed", "Ok", ARRAY(socket));
               break;
         };
      }

 /*
      MemZero(&set, sizeof(set));
      maxsock = 0;

      for (int i=0; i<hSockets.Count(); i++)
      {
         if (hSockets[i]->listening)
         {
            FTryAccept(hSockets[i]);
         }
         else
         {
            int32 skt = hSockets[i]->socket;
            FD_SET(skt, &set);
            maxsock = skt >? maxsock;
         }
      }

      CopyMem(&set, &srd, sizeof(set));
      CopyMem(&set, &swr, sizeof(set));
      CopyMem(&set, &sxc, sizeof(set));
      tv.tv_secs  = 0;
      tv.tv_micro = 0;
      bsock->WaitSelect(maxsock + 1, &srd, &swr, &sxc, &tv, 0);
      MessageBox("Info", "Read: %08lx %08lx, Write: %08lx %08lx, Except: %08lx %08lx", "Ok", ARRAY(srd.fds_bits[1], srd.fds_bits[0], swr.fds_bits[1], swr.fds_bits[0], sxc.fds_bits[1], sxc.fds_bits[0]));
      for (int i=0; i<hSockets.Count(); i++)
      {
         if (hSockets[i]->listening == false)
         {
            int32 skt = hSockets[i]->socket;
            if (FD_ISSET(skt, &srd))
               FTryRead(hSockets[i]);

//            if (FD_ISSET(skt, &swr))
//               FTryWrite(hSockets[i]);

//            if (FD_ISSEN[skt, &sxc))
//               FTryExcept(hSockets[i]);
            
         }
      }
*/
   };

   while (hSockets.Count() > 0)
   {
      hSockets[0]->hOnClose.Call(hSockets[0], (void*)0);
      FDisconnect(hSockets[0]);
   }

   bsock->FreeInstance();

   return 0;
}

unsigned long NetService::FProcCommands(unsigned long ACmd, void* AData)
{
   void *Res = 0;

   switch (ACmd)
   {
      case Sub_Connect:
         {
            SubStr_Connect *pConn = (SubStr_Connect*)AData;
            Res = FConnect(pConn->pHost, pConn->lPort, pConn->pDataAvail, pConn->pWriteReady, pConn->pClosed, pConn->eType);
         }
         break;
         
      case Sub_Disconnect:
         {
            SubStr_Disconnect *pDisc = (SubStr_Disconnect*)AData;
            FDisconnect(pDisc->socket);
         }
         break;
         
      case Sub_Listen:
         {
            SubStr_Listen *pBind = (SubStr_Listen*)AData;
            Res = FBind(0, pBind->lPort, pBind->pAcceptIncoming, pBind->eType);
         }
         break;

      case Sub_Read:
         {
            SubStr_ReadBuffer *pRead = (SubStr_ReadBuffer*)AData;
            Res = (void*)FReadBuffer(pRead->socket, pRead->data, pRead->maxlen);
         }
         break;

      case Sub_Write:
         {
            SubStr_WriteBuffer *pWrite = (SubStr_WriteBuffer*)AData;
            Res = (void*)FWriteBuffer(pWrite->socket, pWrite->data, pWrite->maxlen);
         }
         break;
   };
   return (unsigned long)Res;
}

NetService::Socket *NetService::Listen(int APort, const Hook* accept, SocketType stype)
{
   SubStr_Listen hSub;
   hSub.lPort           = APort;
   hSub.eType           = stype;
   hSub.pAcceptIncoming = accept;

   return (Socket*)pSocketThread->DoSync(Sub_Listen, &hSub);
}

NetService::Socket *NetService::Connect(char* AHost, int APort, const Hook* onData, const Hook* onWriteRdy, const Hook* onClose, SocketType stype)
{
   SubStr_Connect hSub;
   hSub.lPort        = APort;
   hSub.pHost        = AHost;
   hSub.eType        = stype;
   hSub.pDataAvail   = onData;
   hSub.pWriteReady  = onWriteRdy;
   hSub.pClosed      = onClose;
   
   return (Socket*)pSocketThread->DoSync(Sub_Connect, &hSub);
}

void NetService::Disconnect(NetService::Socket *skt)
{
   SubStr_Disconnect d;
   d.socket = skt;

   pSocketThread->DoSync(Sub_Disconnect, &d);
}

