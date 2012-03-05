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
#include "NetSocket.h"
#include "Synchronizer.h"
#include <libclass/bsdsocket.h>
#include <libdata/bsdsocket/netdb.h>
#include <libdata/bsdsocket/in.h>
#include <libdata/bsdsocket/ioctl.h>
#include <libdata/bsdsocket/socketbasetags.h>
#include <libclass/exec.h>
#include "LibrarySpool.h"
#include <LibC/LibC.h>
#include <libclass/dos.h>

using namespace GenNS;

struct timeval
{
   uint32      seconds;
   uint32      micro;
};

   int32          NetSocket::stTrue    = 1;
   int32          NetSocket::stFalse   = 0;

NetSocket::NetSocket(NetSocket::SocketID *socket, bool isserv) 
{
   FConstruct(socket, isserv);
}

void NetSocket::FConstruct(NetSocket::SocketID *socket, bool server)
{
   lThreadSignal=0;

   addr_local  = new sockaddr_in;
   addr_remote = new sockaddr_in;
   
   bsock       = BSDSocketIFace::GetInstance(0);
   bConnected  = -1;
   lSocket     = -1;

   bServer     = server;
   nSignal     = -1;

   if (bsock != 0)
   {
      int32 temp;

      temp = Exec->AllocSignal(-1);
      lSignal     = 1<<temp;
      SetSignals(lSignal);
      nSignal     = temp;
      lSocket     = (socket) ? bsock->ObtainSocket(socket->id, socket->domain, socket->type, socket->proto) : -1;
      bConnected  = (lSocket >= 0) ? true : false;
      if (lSocket)
      {
         FSetupSocket();
      }
   }
}

NetSocket::~NetSocket()
{
   SetSignals(0);
   FDisconnect();
   bsock->FreeInstance();
   delete addr_local;
   delete addr_remote;
}
 
bool NetSocket::FResolve(char* AHost, int APort, sockaddr_in *AAddr)
{
   if (AHost)
   {
      hostent *pHost = bsock->gethostbyname(AHost);

      if (0 != pHost)
      {
         Exec->CopyMem(pHost->h_addr_list[0], &AAddr->sin_addr, sizeof(AAddr->sin_addr));   
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

bool NetSocket::FSetupSocket()
{
   uint32      lSockEvMask = FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE | FD_ERROR;
   uint32      bs = 32768;

   if (lSocket == -1)
      return false;

   bsock->setsockopt(lSocket, SOL_SOCKET, SO_EVENTMASK, &lSockEvMask, sizeof(unsigned long));
   if (error < 0)
   {
      //request("", "Unable to set socket options", "Ok", 0);
      FDisconnect();
      return false;
   }

   error = bsock->setsockopt(lSocket, SOL_SOCKET, SO_KEEPALIVE, &stTrue, sizeof(stTrue));
   if (error < 0)
   {
      //request("", "Unable to set socket options", "Ok", 0);
      FDisconnect();
      return false;
   }

   error = bsock->setsockopt(lSocket, SOL_SOCKET, SO_SNDBUF, &bs, sizeof(bs));
   if (error < 0)
   {
      //request("", "Unable to set socket options", "Ok", 0);
      FDisconnect();
      return false;
   }

   error = bsock->setsockopt(lSocket, SOL_SOCKET, SO_RCVBUF, &bs, sizeof(bs));
   if (error < 0)
   {
      //request("", "Unable to set socket options", "Ok", 0);
      FDisconnect();
      return false;
   }
   bsock->IoctlSocket(lSocket, FIONBIO, (char*)&stFalse);
//   bsock->IoctlSocket(lSocket, FIOASYNC, (char*)&stTrue);

   return true;
}

bool NetSocket::FConnect(char* AHost, int16 APort, SocketType stype)
{
   sockaddr_in addr;
   
   FDisconnect();

   error = 0;
   lSocket = bsock->socket(AF_INET, FTypeToInet(stype), 0);    

   if (!FSetupSocket())
      return false;

   if (!FResolve(AHost, APort, &addr))
   {
      request("", "Unable to resolve host name", "Ok", 0);
      FDisconnect();
      return false;
   }

   error = bsock->connect(lSocket, (sockaddr*)&addr, sizeof(addr));
   if (error < 0)
   {
      request("", "Unable to connect", "Ok", 0);
      FDisconnect();
      return false;
   }

   bServer     = false;
   bConnected  = true;

   FGetAddresses();

   return true;
}

void NetSocket::FGetAddresses()
{
   if (!bConnected)
      return;
   if (bsock == 0)
      return;

   int32 i;
  
   i = sizeof(addr_local);
   bsock->getsocketname(lSocket, (sockaddr*)addr_local, &i);
   i = sizeof(addr_local);
   bsock->getpeername(lSocket, (sockaddr*)addr_remote, &i);

}

void NetSocket::FDisconnect()
{
   if (lSocket != -1)
   {
      if (bServer)
         bsock->shutdown(lSocket, 2);
      bsock->CloseSocket(lSocket);
   }
   lSocket     = -1;
   bConnected  = false;
   bServer     = false;
   return;   
}

bool NetSocket::FBind(int16 APort, SocketType stype)
{
   sockaddr_in addr;
   uint32      lSockEvMask = FD_CONNECT | FD_ACCEPT | FD_READ | FD_CLOSE | FD_ERROR;
   int32       pTrue       = 1;

   FDisconnect();

   lSocket = bsock->socket(AF_INET, FTypeToInet(stype), 0);    
   bsock->setsockopt(lSocket, SOL_SOCKET, SO_EVENTMASK, &lSockEvMask, sizeof(unsigned long));
   bsock->setsockopt(lSocket, SOL_SOCKET, SO_KEEPALIVE, &stTrue, sizeof(stTrue));
   bsock->IoctlSocket(lSocket, FIONBIO, (char*)&pTrue);
   bsock->IoctlSocket(lSocket, FIOASYNC, (char*)&pTrue);
   
   if (!FResolve(0, APort, &addr))
   {
      FDisconnect();
      return false;
   }

   error = bsock->bind(lSocket, (sockaddr*)&addr, sizeof(addr));
   if (error < 0)
   {
      FDisconnect();
      return false;
   }

   error = bsock->listen(lSocket, 5);
   if (error < 0)
   {
      FDisconnect();
      return false;
   }

   bServer     = true;
   bConnected  = true;
   FGetAddresses();
   return true;
}

int32 NetSocket::FTypeToInet(SocketType stype)
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

bool NetSocket::FTryAccept(NetSocket::SocketID *s)
{
   sockaddr    saddr;
   int32       laddr = sizeof(sockaddr);

/* FIXME: very very very basic stuff. need to allow other types too? not sure - dgram is connectionless.
 */
   s->id     = bsock->ReleaseSocket(bsock->accept(lSocket, &saddr, &laddr), 65536 + Utility->GetUniqueID());
   s->domain = AF_INET;
   s->type   = SOCK_STREAM;
   s->proto  = 0;

   return (s->id != -1);
}

int32 NetSocket::ReadBuffer(void* buffer, int32 maxlen)
{
   return FReadBuffer(buffer, maxlen);
}

int32 NetSocket::WriteBuffer(void* buffer, int32 maxlen)
{
   return FWriteBuffer(buffer, maxlen);
}

int32 NetSocket::FReadBuffer(void* buff, int32 len)
{
   if (len == 0)
      return 0;

   int32 bytes;

   /*
   do
   {
      bytes = WaitStatus(~0, 0);
      if (bytes & SEv_Disconnected)
         return -1;
   } while (0 == (bytes & SEv_DataReady));
   */

   //bsock->IoctlSocket(lSocket, FIONREAD, &bytes);
/*
   do
   {
*/
      bytes = bsock->recv(lSocket, buff, len, MSG_WAITALL);
/*
      if (Errno() != 11)   // EAGAIN
         break;
      DOS->Delay(1);
   } while (true);
*/      
   if (bytes <= 0)
   {
      FDisconnect();
      return bytes;
   }
   return bytes;
}
      
int32 NetSocket::Errno()
{
   return bsock->Errno();
}

int32 NetSocket::FWriteBuffer(void* buff, int32 len)
{
   int32 bs;
   int32 sent = 0;

   while (len > 0)
   {
      do
      {
         bs = WaitStatus(~0, 0, SEv_WriteReady | SEv_Disconnected);
         if (bs & SEv_Disconnected)
            return -1;
      } while (0 == (bs & SEv_WriteReady));

/*
      do
      {
*/
      bs = bsock->send(lSocket, &((char*)buff)[sent], len < 32768 ? len : 32768, 0);
/*
         if (Errno() != 11) // EAGAIN
            break;
         DOS->Delay(1);
      } while (true);
*/
      if (bs > 0)
         len -= bs, sent += bs;

   }
   return sent;
}
   
bool NetSocket::Listen(int APort, SocketType stype)
{
   return FBind(APort, stype);
}

bool NetSocket::Connect(char* AHost, int APort, SocketType stype)
{
   return FConnect(AHost, APort, stype);
}

void NetSocket::Disconnect()
{
   FDisconnect();
}

bool NetSocket::Accept(NetSocket::SocketID* sid)
{
   return FTryAccept(sid);
}

void NetSocket::SetSignals(uint32 signal)
{
   if (nSignal)
   {
      Exec->FreeSignal(nSignal);
      nSignal = -1; 
   }
   lSignal = signal;
   bsock->SocketBaseTagList((TagItem*)ARRAY(SBTC_SIGEVENTMASK, lSignal, 0));
   bsock->SetSocketSignals(0, lSignal, 0);
}

uint32 NetSocket::GetSignals()
{
   return lSignal;
}

int32 NetSocket::GetDataAvail()
{
   int32 bytes = 0;
   int32 error;
   error = bsock->IoctlSocket(lSocket, FIONREAD, &bytes);
   if (error < 0)
   {
      request("Info", "Disconnected? (get data avail)", "Ok", 0);
   }
   return bytes;
}

uint32 NetSocket::FGetStatus(uint32 millis, uint32 sigs, uint16 events)
{
   fd_set   rx, wx, ex;
   int32    sock;
   timeval  tv =
   {
      millis/1000,
      (millis % 1000) * 1000
   };

   if (!bConnected)
      return SEv_Disconnected;      // unmaskable...

   memset(&rx, 0, sizeof(rx));
   memset(&wx, 0, sizeof(rx));
   if (events & (SEv_DataReady | SEv_IncomingCall))
      FD_SET(lSocket, &rx);
   if (events & (SEv_WriteReady))
      FD_SET(lSocket, &wx);
   // oob
   Exec->CopyMem(&rx, &ex, sizeof(rx));

   if ((sock = bsock->WaitSelect(lSocket + 1, &rx, &wx, &ex, &tv, &sigs)) < 0)
   {
      return SEv_Disconnected;
   }

   //request("Info", "%ld %lx %lx %lx", "Ok", ARRAY(sock, rx.fds_bits[0], wx.fds_bits[0], ex.fds_bits[0]));

   sock = 0;
   if (sigs != 0)
      sock |= SEv_OtherEvent;

   if (FD_ISSET(lSocket, &rx))
   {
      if (!bServer)
      {
         sock |= SEv_DataReady;
      }
      else
         sock |= SEv_IncomingCall;
   }

   if (FD_ISSET(lSocket, &wx))
      sock |= SEv_WriteReady;

   Exec->SetSignal(sigs, sigs);

   return sock;
}

uint32 NetSocket::GetStatus()
{
   return FGetStatus(0, 0, SEv_AllEvents);
}

uint32 NetSocket::WaitStatus(uint32 millis, uint32 sigs, uint16 events)
{
   return FGetStatus(millis, sigs, events);
}

uint32 NetSocket::GetLocalIP()
{
   if (!bConnected)
      return 0;
   return addr_local->sin_addr.s_addr;
}

uint16 NetSocket::GetLocalPort()
{
   if (!bConnected)
      return 0;
   return addr_local->sin_port;
}

uint32 NetSocket::GetRemoteIP()
{
   if (!bConnected)
      return 0;
   return addr_remote->sin_addr.s_addr;
}

uint16 NetSocket::GetRemotePort()
{
   if (!bConnected)
      return 0;
   return addr_remote->sin_port;
}

